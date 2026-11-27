"""Re-score every accepted class rename with its own slots left out (review, 2026-11-26).

For each row of docs/data/class-renames.tsv (ours -> retail): the slots that carry the rename (their original
qualifier is ours, their name's is retail's) are un-named, so none of them is an anchor or bounds a sibling;
the anchors are recomputed from everything else, and the recorded names are tested against the near parts that
result. This answers three questions per rename at once:
  cascade      do the class's hits stand without each other (k of n near, against `anchors` in the table)?
  chance       C(n, k) x prod(forms x prob(near)) x the candidates tried for the class (TRIES below);
  null         the same test with retail's name + two salt characters, K salts (permnull.py): the pooled
               salted chances calibrate the modelled one.
Verdict: keep (chance < --chance), weak (the rest). Writes .work/names/rescore/renames.tsv; changes nothing.
export.py calls verdicts() and writes them into class-renames.tsv (columns near_alone, tries, chance_alone,
verdict); gapguess.State then grades a weak rename's anchors probable. The re-score itself runs with every
rename's anchors bounding (weak=()), so its result does not depend on an earlier run's verdicts.

Usage: python tools/re/thunknames/rescore.py [--state F] [--perm 1000] [--chance 1e-4]
"""
import argparse
import collections
import csv
import math
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import paths  # noqa: E402
import tier2_eval  # noqa: E402
from gapguess import State, bk, class_of, prob  # noqa: E402

# candidates tried per class, by the step that found the rename (the multiplier of the chance):
# round 1's token dictionary, the 1.86M-word dictionary, own variants (classsearch2.own_variants, about 60),
# the +/- Class / Struct suffixes of tier1c; model rounds: the class's candidates in proposals/.
TRIES = {'r1': 291000, 'cs2-dict': 1860000, 'cs2-own': 60, 't1c': 4}

_W = {}


def tries_for(ours, step, tier, cls_c):
    if tier in ('cs2-dict', 'cs2-own'):
        return TRIES[tier]
    if step == 'r1':
        return TRIES['r1']
    if step.startswith('t1c') or tier.startswith('t1c'):
        return TRIES['t1c']
    return max(1, len(cls_c.get(ours, ())))


def _init(state, perm, seed):
    tier2_eval.CONST = True
    st = State(state, weak=())
    by = collections.defaultdict(list)
    for i, n in enumerate(st.names):
        o = st.orig[i]
        if n and o and class_of(o) and class_of(n) and class_of(o) != class_of(n):
            by[(class_of(o), class_of(n))].append(i)
    _W.update(st=st, by=by, perm=perm, seed=seed, path=state)


def _one(pair):
    import numpy as np
    import permnull
    ours, retail = pair
    st = State(_W['path'], weak=())
    slots = _W['by'][pair]
    was_on = sum(i in st.onset for i in slots)
    names = [st.names[i] for i in slots]
    for i in slots:
        st.names[i] = None
    st.recompute()
    wins, keep = [], []
    for i, nm in zip(slots, names):
        w = st.window(i)
        if w:
            wins.append(w[2:])
            keep.append(nm)
    k, logw = 0, 0.0
    for (nlo, nhi), nm in zip(wins, keep):
        if nlo <= bk(nm) <= nhi:
            k += 1
            logw += math.log(len(tier2_eval.variants(nm)) * prob(nlo, nhi))
    n = len(keep)
    lc = (math.lgamma(n + 1) - math.lgamma(k + 1) - math.lgamma(n - k + 1) + logw) if k else 0.0
    null = np.ones(0)
    if _W['perm'] and n:
        salt = permnull.salts(_W['perm'], permnull.seed_of(retail, _W['seed']))
        kk, ch = permnull.salted(wins, keep, retail, retail, tier2_eval.variants, salt)
        null = np.where(kk >= 1, ch, 1.0)
    return pair, n, was_on, k, lc, null


def verdicts(state, pairs, steps, chance=1e-4):
    """{(ours, retail): (near_alone, tries, chance_alone, verdict)} for export.py (no salted null)."""
    import multiprocessing
    cls_c = tier2_eval.load_props(paths.PROPOSALS)[0]
    tier = {}
    for r in csv.DictReader(open(paths.HITS), delimiter='\t'):
        o, n = class_of(r.get('before') or ''), class_of(r['name'])
        if o and n and o != n:
            tier.setdefault((o, n), r['tier'])
    with multiprocessing.Pool(initializer=_init, initargs=(state, 0, 0)) as pool:
        res = pool.map(_one, pairs, chunksize=2)
    out = {}
    for pair, n, was_on, k, lc, _ in res:
        t = tries_for(pair[0], steps.get(pair, ''), tier.get(pair, ''), cls_c)
        ch = min(1.0, math.exp(lc) * t) if k else 1.0
        out[pair] = (k, t, f'{ch:.2g}', 'keep' if ch < chance else 'weak')
    return out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--state', default=paths.STATE)
    ap.add_argument('--perm', type=int, default=1000)
    ap.add_argument('--seed', type=int, default=0)
    ap.add_argument('--chance', type=float, default=1e-4)
    ap.add_argument('--out', default=paths.workdir('rescore'))
    a = ap.parse_args()
    import multiprocessing
    import numpy as np
    ren = list(csv.DictReader(open(paths.RENAMES), delimiter='\t'))
    cls_c = tier2_eval.load_props(paths.PROPOSALS)[0]
    tier = {}
    for r in csv.DictReader(open(paths.HITS), delimiter='\t'):
        o, n = class_of(r.get('before') or ''), class_of(r['name'])
        if o and n and o != n:
            tier.setdefault((o, n), r['tier'])
    pairs = [(r['ours'], r['retail']) for r in ren]
    with multiprocessing.Pool(initializer=_init, initargs=(a.state, a.perm, a.seed)) as pool:
        res = pool.map(_one, pairs, chunksize=2)
    null = np.concatenate([x[5] for x in res]) if a.perm else np.ones(0)
    if len(null):
        print(f'salted null: {len(pairs)} renames x {a.perm} salts; modelled chance (one candidate) against the rate:')
        for e in range(2, 8):
            t = 10.0 ** -e
            print(f'  under 1e-{e}: {int((null < t).sum()):6d} of {len(null)} ({float((null < t).mean()) / t:5.2f}x)')
    step = {(r['ours'], r['retail']): r['first_step'] for r in ren}
    rows, verdicts = [], collections.Counter()
    for (ours, retail), n, was_on, k, lc, _ in res:
        t = tries_for(ours, step[(ours, retail)], tier.get((ours, retail), ''), cls_c)
        ch = min(1.0, math.exp(lc) * t) if k else 1.0
        v = 'keep' if ch < a.chance else 'weak'
        verdicts[v] += 1
        rows.append([ours, retail, n, was_on, k, t, f'{ch:.2g}', step[(ours, retail)], v])
    rows.sort(key=lambda r: -float(r[6]))
    os.makedirs(a.out, exist_ok=True)
    out = os.path.join(a.out, 'renames.tsv')
    with open(out, 'w', newline='') as f:
        w = csv.writer(f, delimiter='\t', lineterminator='\n')
        w.writerow(['ours', 'retail', 'slots', 'anchors', 'near_without_siblings', 'tries', 'chance', 'first_step',
                    'verdict'])
        w.writerows(rows)
    print(f'{out}: {dict(verdicts)}; expected false among the kept: '
          f'{sum(float(r[6]) for r in rows if r[8] == "keep"):.3g}')
    lost = sum(r[3] - r[4] for r in rows if r[4] < r[3])
    print(f'anchors that are not near once their class is left out: {lost} in '
          f'{sum(1 for r in rows if r[4] < r[3])} renames')
    for r in rows:
        if r[8] == 'weak':
            print('  weak: ' + '  '.join(str(x) for x in r[:8]))


if __name__ == '__main__':
    main()
