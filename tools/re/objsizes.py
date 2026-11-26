"""Object sizes: each of our objects' bytes in a trial link against its run in retail's .text, and retail's
stretches no object of ours covers (image builder IM5, 2026-11-25; docs/binary.md §5.4).

A full incremental link gives each app object one run of function slots (up16(S + S/4) each, §5), and packs each
library member into one run (§5): an object's run in retail is the stretch from its first paired function to the
start of the next object's run. members.py's walk does this for the library region; here it runs over both
regions with every object of our map (the command line's and the libraries' members alike), so that

  - per object, our bytes (its .text slots in our map) minus its retail run's bytes is what it adds to .text:
    larger functions (not exact yet), functions retail does not have, or retail functions inside the run
    that are ours elsewhere;
  - retail's gaps (starts between two different objects' runs that pair with none of ours) are code our link
    has nowhere near: unmatched functions at an object's edge, or objects retail linked and ours does not.

Pairs: the ledger's own addresses (`ledger.py export`: each file's in_tree less its shared ones) give each of our
E&B and Westwood objects its exact functions in retail; symbols.json's names, held once in our map, add the rest
(the CRT, d3dx8). Retail functions not exact yet pair with nothing: they fall inside their object's run when the
same object's functions lie on both sides, else in a gap.

    python tools/re/objsizes.py --map build/programs/client-all/client.map --ledger LEDGER.json [--out DIR]

The displacement view (`growth.tsv`): every function our map and symbols.json name once is a fixed point (retail
va, our va). Walking our objects in our map's order, each object's first fixed point gives the displacement there;
an object's growth is the displacement at the next object's first fixed point less its own: what the object (its
bytes up to the next object's first named function) adds to everything after it. The growths telescope: their sum
over a region is the displacement change across it, whatever the objects' own unmatched functions or boundaries.
Fixed points where the two orders disagree are dropped first (a longest increasing run of retail addresses).

Writes objects.tsv (object, region, our bytes, retail bytes, delta, functions, paired, runs) and gaps.tsv to --out
(default .work/objsizes); prints the totals per region and library and the largest deltas and gaps.
"""
import argparse
import collections
import json
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import clientimage  # noqa: E402
import members  # noqa: E402

REPO = members.REPO
APP_LO, LIB_LO, LIB_HI = members.APP_LO, members.LIB_LO, members.LIB_HI


def library_of(member):
    """'cmdline' for an object on the link line (the map names it x.obj), else the library of lib:object."""
    if ':' in member:
        return member.split(':')[0].lower()
    return 'cmdline'


def our_objects(rows):
    """{object: [functions, bytes, first va]} over a map's .text rows: a row's bytes run to the next row."""
    rows = sorted(rows)
    out = collections.OrderedDict()
    for k, (va, name, member, is_fn) in enumerate(rows):
        size = rows[k + 1][0] - va if k + 1 < len(rows) else 0
        e = out.setdefault(member, [0, 0, va])
        e[0] += 1 if is_fn else 0
        e[1] += size
    return out


def retail_runs(runs):
    """{object: [bytes, runs, regions]} from walk's runs."""
    out = collections.defaultdict(lambda: [0, 0, set()])
    for s, e, m, _ in runs:
        r = out[m]
        r[0] += e - s
        r[1] += 1
        r[2].add('app' if s < LIB_LO else 'lib')
    return out


def table(ours, theirs, paired_count):
    """[(object, region, our bytes, retail bytes, delta, functions, paired, runs)] sorted by delta."""
    rows = []
    for m, (fn, b, _) in ours.items():
        rb, nr, reg = theirs.get(m, (0, 0, set()))
        region = '+'.join(sorted(reg)) or 'none'
        rows.append((m, region, b, rb, b - rb, fn, paired_count.get(m, 0), nr))
    for m, (rb, nr, reg) in theirs.items():
        if m not in ours:
            rows.append((m, '+'.join(sorted(reg)), 0, rb, -rb, 0, paired_count.get(m, 0), nr))
    rows.sort(key=lambda r: -r[4])
    return rows


def ledger_pairs(ledger, members_in_map):
    """{retail va: our member} from a ledger export: each file's own addresses (in_tree less shared), its member
    the map's `stem.obj` (an object on the command line) or `lib:stem.obj` (a library member)."""
    have = {m.lower(): m for m in members_in_map}
    out = {}
    for lib, st in ledger.items():
        if not isinstance(st, dict) or 'files' not in st:
            continue
        for key, f in st['files'].items():
            stem = os.path.splitext(os.path.basename(f.get('name', key)))[0].lower()
            m = have.get(f'{stem}.obj') or have.get(f'{lib}:{stem}.obj')
            if not m:
                continue
            shared = set(f.get('shared', []))
            for a in f.get('in_tree', []):
                if a not in shared:
                    out[int(a, 16)] = m
    return out


def lis(seq):
    """Indices of a longest strictly increasing subsequence of seq."""
    import bisect
    tails, tail_i, prev = [], [], [None] * len(seq)
    for i, v in enumerate(seq):
        k = bisect.bisect_left(tails, v)
        if k == len(tails):
            tails.append(v)
            tail_i.append(i)
        else:
            tails[k] = v
            tail_i[k] = i
        prev[i] = tail_i[k - 1] if k else None
    out, i = [], tail_i[-1] if tail_i else None
    while i is not None:
        out.append(i)
        i = prev[i]
    return out[::-1]


def fixed_points(syms, rows, lo=APP_LO, hi=LIB_HI):
    """[(our va, retail va, member)]: functions named once in our map and in symbols.json, in our order, kept
    where both orders agree (a longest increasing run of retail addresses)."""
    where = collections.defaultdict(list)
    for va, name, member, is_fn in rows:
        if is_fn:
            where[name].append((va, member))
    pts = []
    for cva, name in syms:
        hits = where.get(name)
        if hits and len(hits) == 1 and lo <= cva < hi:
            pts.append((hits[0][0], cva, hits[0][1]))
    pts.sort()
    return [pts[i] for i in lis([p[1] for p in pts])]


def growth(syms, rows, lo=APP_LO, hi=LIB_HI):
    """[(member, our va, retail va, growth, fixed points, inside)] in our order: each object's growth is the sum,
    over its fixed points, of the displacement change to the next fixed point (the bytes from it to the next one,
    ours less retail's: the rest of the object, and any objects without a fixed point before the next). `inside` is
    the part between two fixed points of the object itself: its own functions' sizes (not exact, or one image's
    only), free of object order and of the code between objects."""
    pts = fixed_points(syms, rows, lo, hi)
    acc = collections.OrderedDict()
    for k, (ova, cva, m) in enumerate(pts):
        e = acc.setdefault(m, [ova, cva, 0, 0, 0])
        e[3] += 1
        if k + 1 < len(pts):
            g = (pts[k + 1][0] - ova) - (pts[k + 1][1] - cva)
            e[2] += g
            if pts[k + 1][2] == m:
                e[4] += g
    return [(m, e[0], e[1], e[2], e[3], e[4]) for m, e in acc.items()]


def main():
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument('--map', required=True, help='our link map (a trial link with every app object on the line)')
    ap.add_argument('--client', default=clientimage.DEFAULT_CLIENT)
    ap.add_argument('--symbols', default=members.SYMBOLS)
    ap.add_argument('--ledger', help='a ledger export (tools/vc6/ledger.py export -o FILE)')
    ap.add_argument('--out', default=os.path.join(REPO, '.work', 'objsizes'))
    ap.add_argument('--top', type=int, default=25)
    a = ap.parse_args()
    rows = [r for r in members.read_map(open(a.map, encoding='latin1')) if not members.is_import_stub(r[2])]
    syms = [(int(s['address'], 16), s['symbol']) for s in json.load(open(a.symbols)) if s.get('kind') != 'thunk']
    names = {}
    for va, n in syms:
        names.setdefault(va, n)
    paired = members.pair(syms, rows)
    if a.ledger:
        lp = ledger_pairs(json.load(open(a.ledger)), {r[2] for r in rows})
        moved = sum(1 for v, m in lp.items() if v in paired and paired[v] != m)
        paired.update(lp)
        print(f'pairs: {len(paired):,} ({len(lp):,} from the ledger, {moved} of them named for another object)')
    starts = clientimage.ClientImage(a.client).function_starts()[0]
    runs, gaps = members.walk(starts, paired, names, lo=APP_LO, hi=LIB_HI)
    ours = our_objects(rows)
    theirs = retail_runs(runs)
    pc = collections.Counter(paired.values())
    tab = table(ours, theirs, pc)
    os.makedirs(a.out, exist_ok=True)
    with open(os.path.join(a.out, 'objects.tsv'), 'w', newline='\n') as f:
        f.write('object\tregion\tours\tretail\tdelta\tfunctions\tpaired\truns\n')
        for r in tab:
            f.write('\t'.join(str(x) for x in r) + '\n')
    with open(os.path.join(a.out, 'gaps.tsv'), 'w', newline='\n') as f:
        f.write('start\tend\tbytes\tfunctions\tregion\tbefore\tafter\tnamed\n')
        for s, e, c, b, af, nm in gaps:
            f.write(f'{s:#x}\t{e:#x}\t{e - s}\t{c}\t{"app" if s < LIB_LO else "lib"}\t{b}\t{af}\t{" ".join(nm)}\n')

    for reg, lo, hi in (('app', APP_LO, LIB_LO), ('lib', LIB_LO, LIB_HI)):
        part = [r for r in tab if r[1] == reg]
        g = [x for x in gaps if lo <= x[0] < hi]
        ob = sum(r[2] for r in part)
        rb = sum(r[3] for r in part)
        gb = sum(x[1] - x[0] for x in g)
        print(f'{reg}: {len(part)} objects with one region; ours {ob:,} bytes, their retail runs {rb:,} '
              f'(delta {ob - rb:+,}); retail gaps {len(g)} ({gb:,} bytes, {sum(x[2] for x in g)} functions)')
        by = collections.defaultdict(lambda: [0, 0, 0])
        for r in part:
            k = library_of(r[0])
            by[k][0] += 1
            by[k][1] += r[4]
            by[k][2] += 1 if r[4] > 0 else 0
        print('  by library (objects, delta, larger): ' +
              ', '.join(f'{k} {v[0]} {v[1]:+,} {v[2]}' for k, v in sorted(by.items(), key=lambda x: -abs(x[1][1]))))
    other = [r for r in tab if r[1] not in ('app', 'lib')]
    print(f'objects in both regions or none: {len(other)} (ours {sum(r[2] for r in other):,} bytes, '
          f'retail {sum(r[3] for r in other):,})')
    print(f'largest deltas (all in {os.path.join(a.out, "objects.tsv")}):')
    for r in tab[:a.top]:
        print(f'  {r[0]:44} {r[1]:7} ours {r[2]:>7,} retail {r[3]:>7,} {r[4]:+7,}  {r[5]} fn, {r[6]} paired, {r[7]} runs')
    print('smallest deltas:')
    for r in tab[-a.top // 2:]:
        print(f'  {r[0]:44} {r[1]:7} ours {r[2]:>7,} retail {r[3]:>7,} {r[4]:+7,}  {r[5]} fn, {r[6]} paired, {r[7]} runs')
    gr = growth(syms, rows)
    with open(os.path.join(a.out, 'growth.tsv'), 'w', newline='\n') as f:
        f.write('object\tour_va\tretail_va\tgrowth\tfixed_points\tinside\n')
        for m, ova, cva, g, n, ins in gr:
            f.write(f'{m}\t{ova:#x}\t{cva:#x}\t{g}\t{n}\t{ins}\n')
    for reg, lo, hi in (('app', APP_LO, LIB_LO), ('lib', LIB_LO, LIB_HI)):
        part = [r for r in gr if lo <= r[2] < hi]
        print(f'growth over the retail {reg} region: {sum(r[3] for r in part):+,} bytes over {len(part)} objects '
              f'with fixed points ({sum(r[4] for r in part):,}); growing {sum(r[3] for r in part if r[3] > 0):+,}, '
              f'shrinking {sum(r[3] for r in part if r[3] < 0):+,}; inside objects {sum(r[5] for r in part):+,} '
              f'(growing {sum(r[5] for r in part if r[5] > 0):+,}, shrinking {sum(r[5] for r in part if r[5] < 0):+,})')
        for m, ova, cva, g, n, ins in sorted(part, key=lambda r: -abs(r[5]))[:a.top]:
            print(f'  {m:44} retail {cva:#x} ours {ova:#x} inside {ins:+7,}, growth {g:+7,}  ({n} fixed points)')
    print('largest gaps:')
    for s, e, c, b, af, nm in sorted(gaps, key=lambda x: x[0] - x[1])[:a.top]:
        print(f'  {s:#x}-{e:#x} {e - s:>7,} bytes {c:>4} fn  after {b}  before {af}  '
              f'{len(nm)} named{": " + ", ".join(nm[:2]) if nm else ""}')


if __name__ == '__main__':
    main()
