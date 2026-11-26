"""Library members: the objects retail's link pulled that ours does not, and the reverse (image builder IM4,
2026-11-25; week plan 2026-11-24 §2 D question 4; docs/binary.md §5.3).

Retail's library code (0x8C1B64-0xA420AC, binary.md §5) is the library members packed in link order, one run
per member. Our link map says which member (lib:object) holds each of our functions; symbols.json says which
name sits at each retail address. A retail function whose name our map holds once belongs to that member.
Walking retail's function starts (clientimage, verified) through the library region:

  - a start with a paired name carries its member;
  - unpaired starts between two starts of one member are that member's (functions not matched yet);
  - unpaired starts between two different members are a **gap**: code our link does not place there. A gap
    holding names of none of our functions, or no names, is a candidate member retail holds and our link
    does not pull (or pulls from a different library version); its size and its names are printed.

The reverse: each library member of our map (not an object on the command line, not app.lib's) with its
functions, how many pair with a retail address, and where those lie (retail's library region, its app region,
nowhere). A member with functions and no pair is ours only (or not matched yet: its names are not in
symbols.json).

    python tools/re/members.py --map build/programs/client-res/client.map [--out .work/im4/members]

Writes gaps.tsv (retail's unpaired stretches), ours.tsv (each of our members), runs.tsv (retail's library
region as member runs) to --out; prints the summary.
"""
import argparse
import bisect
import collections
import json
import os
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import clientimage  # noqa: E402

REPO = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
SYMBOLS = os.path.join(REPO, 'docs', 'data', 'symbols.json')
LIB_LO, LIB_HI = 0x8C1B64, 0xA420AC          # retail's library region (binary.md §5)
APP_LO = 0x429630
MAPLINE = re.compile(r'^ (\d{4}):([0-9a-f]{8})\s+(\S+)\s+([0-9a-f]{8}) (.)(?: (.))?\s+(\S+)?')


def read_map(lines):
    """[(va, name, member, is_function)] of a link map's .text (section 0001) publics and statics."""
    out = []
    for line in lines:
        m = MAPLINE.match(line)
        if m and m.group(1) == '0001':
            out.append((int(m.group(4), 16), m.group(3), m.group(7) or '', m.group(5) == 'f'))
    return out


def is_library_member(member):
    """A member of a library other than app.lib (lib:object); objects on the command line have no colon."""
    return ':' in member and not member.lower().startswith('app:')


def pair(client_syms, our_rows):
    """{client va: member} for client names our map holds exactly once."""
    where = collections.defaultdict(set)
    for va, name, member, _ in our_rows:
        where[name].add((va, member))
    out = {}
    for va, name in client_syms:
        hits = where.get(name)
        if hits and len(hits) == 1:
            out[va] = next(iter(hits))[1]
    return out


def walk(starts, paired, names, lo=LIB_LO, hi=LIB_HI):
    """Retail's library region as runs and gaps.

    starts: sorted retail function starts; paired: {va: member}; names: {va: name} (symbols.json).
    Returns (runs, gaps): runs [(start, end, member, functions)], gaps [(start, end, functions, before, after,
    named)] where `named` lists the gap's symbols.json names. A run's end is the next run's or gap's start."""
    marks = [(s, paired.get(s)) for s in starts if lo <= s < hi]
    n = len(marks)
    nxt = [None] * (n + 1)                     # nxt[i]: the first member at or after i
    for i in range(n - 1, -1, -1):
        nxt[i] = marks[i][1] if marks[i][1] is not None else nxt[i + 1]
    runs, gaps = [], []
    i = 0
    while i < n:
        s, m = marks[i]
        j = i
        if m is not None:
            # unpaired starts stay in the run when the same member follows them
            while j + 1 < n and (marks[j + 1][1] == m or (marks[j + 1][1] is None and nxt[j + 1] == m)):
                j += 1
            end = marks[j + 1][0] if j + 1 < n else hi
            runs.append((s, end, m, j - i + 1))
        else:
            while j + 1 < n and marks[j + 1][1] is None:
                j += 1
            end = marks[j + 1][0] if j + 1 < n else hi
            named = [names[v] for v, _ in marks[i:j + 1] if v in names]
            gaps.append((s, end, j - i + 1, runs[-1][2] if runs else None, nxt[j + 1], named))
        i = j + 1
    return runs, gaps


def classify(gap, starts, sources):
    """A gap's kind: 'held' when every function in it is named by an exact match of one of our objects (code our
    link holds whose names our map does not list: statics), 'part' when some are named, 'unnamed' otherwise."""
    s, e = gap[0], gap[1]
    inside = starts[bisect.bisect_left(starts, s):bisect.bisect_left(starts, e)]
    exact = sum(1 for v in inside if sources.get(v) == 'exact')
    if inside and exact == len(inside):
        return 'held'
    return 'part' if any(v in sources for v in inside) else 'unnamed'


def is_import_stub(member):
    """An import library's jmp [__imp_X] stub (KERNEL32:KERNEL32.dll)."""
    return member.lower().endswith('.dll')


def our_members(our_rows, paired):
    """{member: dict(functions, bytes, paired, in_lib, in_app)} for our library members' .text functions."""
    rows = sorted(our_rows)
    by_member = collections.OrderedDict()
    client_of = collections.defaultdict(list)
    for cva, member in paired.items():
        client_of[member].append(cva)
    for k, (va, name, member, is_fn) in enumerate(rows):
        if not is_library_member(member) or not is_fn:
            continue
        size = rows[k + 1][0] - va if k + 1 < len(rows) else 0
        e = by_member.setdefault(member, {'functions': 0, 'bytes': 0, 'first': va})
        e['functions'] += 1
        e['bytes'] += size
    for member, e in by_member.items():
        c = client_of.get(member, [])
        e['paired'] = len(c)
        e['in_lib'] = sum(1 for v in c if LIB_LO <= v < LIB_HI)
        e['in_app'] = sum(1 for v in c if APP_LO <= v < LIB_LO)
    return by_member


def main():
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument('--map', required=True, help='our link map (a trial link: every member kept)')
    ap.add_argument('--client', default=clientimage.DEFAULT_CLIENT)
    ap.add_argument('--symbols', default=SYMBOLS)
    ap.add_argument('--out', default=os.path.join(REPO, '.work', 'members'))
    ap.add_argument('--top', type=int, default=25)
    a = ap.parse_args()
    rows = read_map(open(a.map, encoding='latin1'))
    syms = [(int(s['address'], 16), s['symbol']) for s in json.load(open(a.symbols)) if s.get('kind') != 'thunk']
    names = {}
    for va, n in syms:
        names.setdefault(va, n)
    sources = {int(x['address'], 16): x.get('source') for x in json.load(open(a.symbols)) if x.get('kind') != 'thunk'}
    paired = pair(syms, rows)
    starts = clientimage.ClientImage(a.client).function_starts()[0]
    runs, gaps = walk(starts, paired, names)
    kinds = [classify(g, starts, sources) for g in gaps]
    ours = our_members(rows, paired)
    os.makedirs(a.out, exist_ok=True)
    with open(os.path.join(a.out, 'runs.tsv'), 'w') as f:
        f.write('start\tend\tbytes\tfunctions\tmember\n')
        for s, e, m, c in runs:
            f.write(f'{s:#x}\t{e:#x}\t{e - s}\t{c}\t{m}\n')
    with open(os.path.join(a.out, 'gaps.tsv'), 'w') as f:
        f.write('start\tend\tbytes\tfunctions\tkind\tbefore\tafter\tnamed\n')
        for (s, e, c, b, af, nm), k in zip(gaps, kinds):
            f.write(f'{s:#x}\t{e:#x}\t{e - s}\t{c}\t{k}\t{b}\t{af}\t{" ".join(nm)}\n')
    with open(os.path.join(a.out, 'ours.tsv'), 'w') as f:
        f.write('member\tfunctions\tbytes\tpaired\tin_lib\tin_app\tour_first\n')
        for m, e in ours.items():
            f.write(f"{m}\t{e['functions']}\t{e['bytes']}\t{e['paired']}\t{e['in_lib']}\t{e['in_app']}\t{e['first']:#x}\n")

    lib_bytes = LIB_HI - LIB_LO
    run_members = collections.Counter(m for _, _, m, _ in runs)
    gap_bytes = sum(e - s for s, e, *_ in gaps)
    print(f'retail library region {LIB_LO:#x}-{LIB_HI:#x} ({lib_bytes:,} bytes): {len(runs):,} member runs of '
          f'{len(run_members):,} of our members ({lib_bytes - gap_bytes:,} bytes), {len(gaps):,} gaps '
          f'({gap_bytes:,} bytes, {sum(g[2] for g in gaps):,} functions)')
    split = [m for m, c in run_members.items() if c > 1]
    print(f'members in more than one run: {len(split)} (a gap or another member inside them)')
    for k in ('held', 'part', 'unnamed'):
        part = [g for g, kk in zip(gaps, kinds) if kk == k]
        print(f'  {k:8} {len(part):>4} gaps {sum(g[1] - g[0] for g in part):>8,} bytes {sum(g[2] for g in part):>5} functions')
    print('largest gaps not held (all in gaps.tsv):')
    for s, e, c, b, af, nm in sorted([g for g, k in zip(gaps, kinds) if k != 'held'],
                                     key=lambda g: g[0] - g[1])[:a.top]:
        print(f'  {s:#x}-{e:#x} {e - s:>7,} bytes {c:>4} fn  after {b}  before {af}  '
              f'{len(nm)} named{": " + ", ".join(nm[:3]) if nm else ""}')
    stubs = [m for m in ours if is_import_stub(m)]
    print(f'import stubs (jmp [__imp_X], one member per DLL): {len(stubs)} DLLs, '
          f'{sum(ours[m]["functions"] for m in stubs)} stubs; left out below')
    ours = collections.OrderedDict((m, e) for m, e in ours.items() if not is_import_stub(m))
    none = [(m, e) for m, e in ours.items() if e['paired'] == 0]
    elsewhere = [(m, e) for m, e in ours.items() if e['paired'] and not e['in_lib']]
    print(f'our library members with .text functions: {len(ours):,}; paired with retail: '
          f'{len(ours) - len(none):,}; with no pair: {len(none):,} ({sum(e["bytes"] for _, e in none):,} bytes); '
          f'paired only outside retail\'s library region: {len(elsewhere)}')
    by_lib = collections.Counter(m.split(':')[0] for m, _ in none)
    print('  no pair, by library: ' + ', '.join(f'{k} {v}' for k, v in by_lib.most_common()))
    for m, e in sorted(none, key=lambda x: -x[1]['bytes'])[:a.top]:
        print(f"  {m:40} {e['functions']:>4} fn {e['bytes']:>7,} bytes")
    for m, e in elsewhere[:a.top]:
        print(f"  elsewhere: {m:40} {e['paired']} paired, {e['in_app']} in retail's app region")


if __name__ == '__main__':
    main()
