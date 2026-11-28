"""Write the tracked name-recovery data (docs/binary.md §5.11):

  docs/data/thunk-names.tsv       the state: every named thunk slot (slot, target, name, original = IM3's name
                                  where it differs, bucket, on_order, how it was found)
  docs/data/thunk-name-hits.tsv   every accepted hit, in order (step, slot, name, before, tier, note, where,
                                  window, near, bucket, tested, chance, group, in_client, round)
  docs/data/class-renames.tsv     our class -> retail class (derived from the two above)

Usage:
  python tools/re/thunknames/export.py --state F                        # copy F to the state, refresh class-renames
  python tools/re/thunknames/export.py --state F --import STEP=HITS ... [--round1 R1.tsv]
                                  # also (re)build the hits file from per-step hits.tsv files, in the given order

class-renames.tsv columns: ours, retail, slots (our named slots now spelled with retail's class), anchors (of
them on the order), destructor_anchor (a ??_G / ??_E / ??1 anchor: the compiler-fixed test), best_chance (the
lowest per-slot chance among this pair's hits, gapguess's forms tested x width / buckets; NOT the class score,
which is in the step's log; empty for round 1 / propagation), first_step, confidence, split_suspect.
confidence: A = a destructor anchor and >= 3 anchors, or >= 6 anchors; B = >= 3 anchors; C = the rest.
split_suspect: our class also maps to another retail class, or an anchor still spells our class name.
near_alone, tries, chance_alone, verdict: rescore.py's class score with the class's own slots left out (how many
of them are still near, the candidates tried for the class, C(n, k) x prod x tries, keep / weak at 1e-4). A weak
rename's anchors are graded probable by gapguess.State (they bound no window); builders take `keep` rows only.
The state's `grade` column (confirmed / probable: why) is gapguess.State's, written on every export.
"""
import argparse
import collections
import csv
import os
import shutil
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import paths  # noqa: E402
from gapguess import State, HIT_COLS  # noqa: E402
from mangle import class_of, contains  # noqa: E402

COLS = ['step'] + HIT_COLS


def import_hits(steps, round1):
    rows, seen = [], set()
    if round1:
        for r in csv.DictReader(open(round1), delimiter='\t'):
            if r['original'] and '_Construct' not in r['how']:
                rows.append(dict(step='r1', slot=r['slot'], target=r['target'], name=r['name'], before=r['original'],
                                 tier='r1', note=r['how'], where='', window='', near='', bucket=r['bucket'],
                                 tested='', chance='', group='', in_client='', round='1'))
    for spec in steps:
        step, path = spec.split('=', 1)
        for r in csv.DictReader(open(path), delimiter='\t'):
            rows.append(dict(r, step=step))
    out = []
    for r in rows:
        key = (r['slot'], r['name'])
        if key not in seen:
            seen.add(key)
            out.append(r)
    with open(paths.HITS, 'w', newline='') as f:
        w = csv.DictWriter(f, COLS, delimiter='\t', lineterminator='\n', extrasaction='ignore')
        w.writeheader()
        w.writerows(out)
    print(f'{paths.HITS}: {len(out)} hits from {len(steps)} steps' + (' + round 1' if round1 else ''))


def class_renames(st):
    hits = list(csv.DictReader(open(paths.HITS), delimiter='\t')) if os.path.exists(paths.HITS) else []
    best, first = {}, {}
    for r in hits:
        a, b = class_of(r.get('before') or ''), class_of(r['name'])
        if a and b and a != b:
            first.setdefault((a, b), r['step'])
            if r.get('chance'):
                c = float(r['chance'])
                best[(a, b)] = min(best.get((a, b), c), c)
    pairs = collections.defaultdict(lambda: dict(slots=0, anchors=0, dtor=0))
    for i, n in enumerate(st.names):
        o = st.orig[i]
        if not n or not o:
            continue
        a, b = class_of(o), class_of(n)
        if a and b and a != b:
            p = pairs[(a, b)]
            p['slots'] += 1
            if i in st.onset:
                p['anchors'] += 1
                p['dtor'] += n.startswith(('??_G', '??_E', '??1'))
    maps = collections.Counter(a for a, b in pairs if pairs[(a, b)]['anchors'])
    anchors = [st.names[i] for i in st.onset]
    rows = []
    import rescore
    live = [k for k, p in sorted(pairs.items()) if p['anchors']]
    ver = rescore.verdicts(paths.STATE, live, {k: first.get(k, 'prop') for k in live})
    for (a, b), p in sorted(pairs.items()):
        if not p['anchors']:
            continue
        conf = 'A' if (p['dtor'] and p['anchors'] >= 3) or p['anchors'] >= 6 else ('B' if p['anchors'] >= 3 else 'C')
        split = maps[a] > 1 or any(contains(n, a) for n in anchors)
        rows.append([a, b, p['slots'], p['anchors'], int(bool(p['dtor'])),
                     f'{best[(a, b)]:.2g}' if (a, b) in best else '', first.get((a, b), 'prop'), conf, int(split),
                     *ver[(a, b)]])
    with open(paths.RENAMES, 'w', newline='') as f:
        w = csv.writer(f, delimiter='\t', lineterminator='\n')
        w.writerow(['ours', 'retail', 'slots', 'anchors', 'destructor_anchor', 'best_chance', 'first_step',
                    'confidence', 'split_suspect', 'near_alone', 'tries', 'chance_alone', 'verdict'])
        w.writerows(rows)
    c = collections.Counter(r[7] for r in rows)
    print(f'{paths.RENAMES}: {len(rows)} class renames (A {c["A"]}, B {c["B"]}, C {c["C"]}; '
          f'split suspects {sum(r[8] for r in rows)}; weak {sum(r[12] == "weak" for r in rows)})')


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--state', required=True)
    ap.add_argument('--import', dest='steps', nargs='*', default=None, metavar='STEP=HITS')
    ap.add_argument('--round1')
    a = ap.parse_args()
    if os.path.abspath(a.state) != os.path.abspath(paths.STATE):
        shutil.copyfile(a.state, paths.STATE)
    st = State(paths.STATE, weak=())
    if a.steps is not None:
        import_hits(a.steps, a.round1)
    class_renames(st)                      # writes the verdicts the grades read
    st = State(paths.STATE)
    st.write(paths.STATE)                  # on / off and the grade column as the current rules give them
    print(f'{paths.STATE}: {st.summary()}')


if __name__ == '__main__':
    main()
