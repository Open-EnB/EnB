"""Tier 2 evaluation: model proposals (tier2/out/*.tsv) against the hash.

CLASS lines: candidate X for our class C. Every open slot of C is renamed C -> X (every C@ spelling) with each
access of its kind (Q A I / U M E / S K C); a slot counts once. Score: near hits k of n slots, chance =
C(n, k) * prod(forms * prob(near_i)) * (candidates tried for C); prob() counts an unsplit bucket twice (gapguess).
Two nulls. The cross-class one (every class's candidates on every other class) is NOT a null, it is a search: its
hits are sibling classes' true names, so it prints them as CROSS HIT for a look and judges nothing. The salted one
(--perm K, permnull.py: every candidate + two salt characters, K salts) is the null: its pooled best-per-class
chances give the threshold at which the expected false classes of this run stay under --fwer.
METHOD lines: slot's name with the method replaced, our class or the accepted X, each access.
Accepted (class chance < --chance and k >= 2; methods: a near hit with the class accepted or anchored) -> guesses
for gapguess.py (tier2/guesses.tsv). Prints per class and per model batch.

Usage: python tools/re/thunknames/tier2_eval.py --state F [--chance 1e-4] [--perm 1000 [--fwer 0.05]]
"""
import argparse
import collections
import csv
import glob
import math
import os
import re

import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import paths  # noqa: E402  (also puts tools/re on sys.path)
from mangle import rename as rename_class  # noqa: E402
from gapguess import State, prob, bk, class_of  # noqa: E402

ACC = {c: g for g in ('QAI', 'UME', 'SKC') for c in g}


def acc_pos(name):
    """Index of the access letter in a member name (after the qualifier's '@@'), or None."""
    k = name.find('@@')
    if k < 0 or k + 2 >= len(name):
        return None
    return k + 2 if name[k + 2] in ACC else None


CONST = False   # --const: also flip the this-cv (A <-> B) of member functions (name-only, byte-identical code)


def variants(name):
    out = _variants(name)
    if name.startswith(('??_G', '??_E')):
        out = out + [('??_E' if v.startswith('??_G') else '??_G') + v[4:] for v in out]
    if CONST:
        more = []
        for v in out:
            p = acc_pos(v)
            if p is not None and not v.startswith(('??0', '??1', '??_G', '??_E')) and v[p] in 'QAIUME' \
                    and p + 1 < len(v) and v[p + 1] in 'AB':
                more.append(v[:p + 1] + ('B' if v[p + 1] == 'A' else 'A') + v[p + 2:])
        out = out + more
    return out


def _variants(name):
    p = acc_pos(name)
    if p is None or name.startswith(('??_G', '??_E')):
        out = [name]
        if name.startswith(('??_G', '??_E')):
            out = [name[:p] + c + name[p + 1:] for c in 'UME'] if p else [name]
        return out
    return [name[:p] + c + name[p + 1:] for c in ACC[name[p]]]


def score(st, slots, c, x, ntries):
    hits, logw = [], 0.0
    for i in slots:
        lo, hi, nlo, nhi = st.window(i)
        vs = variants(rename_class(st.names[i], c, x))
        for v in vs:
            if nlo <= bk(v) <= nhi:
                hits.append((i, v))
                logw += math.log(len(vs) * prob(nlo, nhi))
                break
    n, k = len(slots), len(hits)
    if k == 0:
        return 1.0, hits
    lc = math.lgamma(n + 1) - math.lgamma(k + 1) - math.lgamma(n - k + 1) + logw + math.log(max(1, ntries))
    return min(1.0, math.exp(lc)), hits


def load_props(props):
    """Every proposal file under `props`: ({class: [candidates]}, {class: [(slot name, method)]}, {class: batch})."""
    cls_c = collections.defaultdict(list)
    meth = collections.defaultdict(list)
    batch_of = {}
    for f in sorted(glob.glob(os.path.join(props, '**', '*.tsv'), recursive=True)):
        b = os.path.basename(f)[:-4]
        for line in open(f):
            p = [x.strip() for x in line.rstrip('\n').split('\t')]
            if len(p) >= 3 and p[0] == 'CLASS' and re.fullmatch(r'[A-Za-z_]\w*', p[2]):
                if p[2] not in cls_c[p[1]]:
                    cls_c[p[1]].append(p[2])
                batch_of[p[1]] = b
            elif len(p) >= 4 and p[0] == 'METHOD' and re.fullmatch(r'[A-Za-z_]\w*', p[3]):
                meth[p[1]].append((p[2], p[3]))
                batch_of.setdefault(p[1], b)
    return cls_c, meth, batch_of


_PW = {}


def _perm_init(state, props, const, k, seed):
    global CONST
    CONST = const
    st = State(state)
    open_by = collections.defaultdict(list)
    for i, n in enumerate(st.names):
        c = n and class_of(n)
        if c and i not in st.onset and st.window(i):
            open_by[c].append(i)
    _PW.update(st=st, open_by=open_by, cls_c=load_props(props)[0], k=k, seed=seed)


def _perm_class(c):
    """The salted null for class c: per salt, the best chance over its candidates (k >= 2, x candidates tried)."""
    import numpy as np
    import permnull
    st, slots, cands = _PW['st'], _PW['open_by'][c], _PW['cls_c'][c]
    wins = [st.window(i)[2:] for i in slots]
    names = [st.names[i] for i in slots]
    salt = permnull.salts(_PW['k'], permnull.seed_of(c, _PW['seed']))
    best = np.ones(_PW['k'])
    raw = []
    for x in cands:
        k, ch = permnull.salted(wins, names, c, x, variants, salt)
        raw.append(np.where(k >= 2, ch, 1.0))
        best = np.minimum(best, np.where(k >= 2, np.minimum(1.0, ch * len(cands)), 1.0))
    return c, best, np.concatenate(raw) if raw else np.ones(0)


def perm_null(state, props, const, classes, k, seed, fwer, chance, log=print):
    """Runs the salted null over `classes`; returns the threshold (<= chance) whose expected false classes in
    this run stay under fwer, and prints the calibration of the modelled chance."""
    import multiprocessing
    import numpy as np
    with multiprocessing.Pool(initializer=_perm_init, initargs=(state, props, const, k, seed)) as pool:
        res = pool.map(_perm_class, classes, chunksize=4)
    best = np.concatenate([b for _, b, _ in res])
    raw = np.concatenate([r for _, _, r in res])
    log(f'salted null: {len(classes)} classes x {k} salts = {len(best)} class draws, {len(raw)} candidate draws')
    log('  threshold   candidate draws under it (modelled chance: rate / threshold)   class draws under it   '
        'expected false classes in this run')
    thr = None
    for e in range(2, 9):
        t = 10.0 ** -e
        rr = float((raw < t).mean()) if len(raw) else 0.0
        rb = float((best < t).mean())
        exp = rb * len(classes)
        log(f'  1e-{e}        {int((raw < t).sum()):8d} ({rr / t:5.2f}x)                            '
            f'{int((best < t).sum()):8d}              {exp:.3f}')
        if thr is None and t <= chance and exp <= fwer:
            thr = t
    worst = sorted(((float(b.min()), c) for c, b, _ in res))[:5]
    log('  lowest salted class chances: ' + ', '.join(f'{c} {v:.1g}' for v, c in worst))
    floor = 1.0 / len(best)
    log(f'  (one class draw = {floor:.1g} of the pool: figures under about {3 * floor:.0g} x classes are zero by '
        f'resolution, not by proof)')
    return thr if thr is not None else 1e-8


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--state', default=paths.STATE)
    ap.add_argument('--chance', type=float, default=1e-4)
    ap.add_argument('--out', default=paths.workdir('tier2'))
    ap.add_argument('--props', default=paths.PROPOSALS, help='directory of proposal TSVs (searched recursively)')
    ap.add_argument('--null-sample', type=int, default=0, help='other classes\' candidates sampled per class (0: all)')
    ap.add_argument('--guesses', default='guesses.tsv')
    ap.add_argument('--const', action='store_true')
    ap.add_argument('--seed', type=int, default=0, help='null sample seed (deterministic per class and seed)')
    ap.add_argument('--perm', type=int, default=0, metavar='K',
                    help='the salted null with K salts per candidate (permnull.py); sets the accept threshold')
    ap.add_argument('--fwer', type=float, default=0.05,
                    help='with --perm: the expected false classes allowed in the whole run')
    ap.add_argument('--no-cross', action='store_true', help='skip the cross-class search')
    ap.add_argument('--methods', action='store_true',
                    help='also emit the METHOD lines\' single-slot hits (no threshold: the cloud rounds\' rule). '
                         'Off by default: tier2_methods.py scores method names, with a threshold and a null')
    a = ap.parse_args()
    global CONST
    CONST = a.const
    st = State(a.state)
    open_by = collections.defaultdict(list)
    for i, n in enumerate(st.names):
        c = n and class_of(n)
        if c and i not in st.onset and st.window(i):
            open_by[c].append(i)
    cls_c, meth, batch_of = load_props(a.props)
    classes = [c for c in cls_c if open_by.get(c)]
    print(f'classes with proposals {len(cls_c)} (with open slots {len(classes)}), class candidates '
          f'{sum(len(v) for v in cls_c.values())}, method proposals {sum(len(v) for v in meth.values())}')
    if a.perm:
        a.chance = perm_null(a.state, a.props, a.const, classes, a.perm, a.seed, a.fwer, a.chance)
        print(f'accept threshold from the salted null: {a.chance:g} (expected false classes <= {a.fwer:g})')
    # the cross-class search (not a null: a hit is a sibling's true name more often than chance)
    null_best = []
    for c in ([] if a.no_cross else classes):
        others = [x for d in classes if d != c for x in cls_c[d] if x not in cls_c[c]]
        if a.null_sample and len(others) > a.null_sample:
            import random
            import zlib
            others = random.Random(zlib.crc32(c.encode()) + a.seed).sample(others, a.null_sample)
        best = min(((score(st, open_by[c], c, x, len(cls_c[c]))[0], x) for x in others), default=(1.0, ''))
        null_best.append(best[0])
        if best[0] < a.chance:
            print(f'  CROSS HIT (look at it, not accepted): {c} with another class\'s candidate {best[1]} at '
                  f'{best[0]:.2g} (x {len(others)} tried / {len(cls_c[c])} counted)')
    nb = sorted(null_best)
    print('cross-class (other classes\' candidates; best chance per class): ' +
          ' '.join(f'{x:.1g}' for x in nb[:8]) + f' ... under {a.chance:g}: {sum(x < a.chance for x in nb)}'
          f' of {len(nb)}')
    accepted = {}
    rows = []
    per_batch = collections.Counter()
    print('class                              candidate                          hits/slots  chance  batch')
    for c in sorted(classes, key=lambda c: batch_of[c]):
        res = sorted((score(st, open_by[c], c, x, len(cls_c[c])) + (x,) for x in cls_c[c]), key=lambda r: r[0])
        ch, hits, x = res[0]
        rank = cls_c[c].index(x) + 1
        ok = ch < a.chance and len(hits) >= 2
        print(f'{c:34s} {x:34s} {len(hits):3d}/{len(open_by[c]):<3d}  {ch:7.1g}  {batch_of[c]} '
              f'(rank {rank}){"  ACCEPT" if ok else ""}')
        if ok:
            accepted[c] = x
            per_batch[batch_of[c]] += len(hits)
            for i, v in hits:
                rows.append([f'0x{st.table[i][0]:X}', v, 't2-class', f'{c}->{x} chance {ch:.2g} {batch_of[c]}'])
    # methods
    mh = collections.Counter()
    for c, props in (meth.items() if a.methods else ()):
        slots = {st.names[i]: i for i in open_by.get(c, [])}
        x = accepted.get(c, c)
        tried = collections.Counter(nm for nm, _ in props)
        for nm, m in props:
            i = slots.get(nm)
            if i is None:
                continue
            base = rename_class(nm, c, x)
            mm = re.match(r'\?([A-Za-z_]\w*)@', base)
            if not mm:
                continue
            new = '?' + m + base[mm.end(1):]
            lo, hi, nlo, nhi = st.window(i)
            for v in variants(new):
                if nlo <= bk(v) <= nhi:
                    ch = tried[nm] * 3 * prob(nlo, nhi)
                    mh[batch_of.get(c, '?')] += 1
                    rows.append([f'0x{st.table[i][0]:X}', v, 't2-method',
                                 f'{c}::{nm} -> {m}; class {"accepted " + x if c in accepted else "ours"}; '
                                 f'chance {ch:.2g} {batch_of.get(c, "?")}'])
                    print(f'  method hit: {v}  (chance {ch:.2g}, class {"accepted" if c in accepted else "ours"})')
                    break
    os.makedirs(a.out, exist_ok=True)
    with open(os.path.join(a.out, a.guesses), 'w', newline='') as f:
        w = csv.writer(f, delimiter='\t', lineterminator='\n')
        w.writerow(['slot', 'name', 'tier', 'note'])
        w.writerows(rows)
    print(f'accepted classes {len(accepted)}, their slots {sum(per_batch.values())} by batch {dict(per_batch)}; '
          f'method hits {sum(mh.values())} by batch {dict(mh)}')


if __name__ == '__main__':
    main()
