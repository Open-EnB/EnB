#!/usr/bin/env python3
"""Unreferenced members of one app file: client function starts nothing calls (D195).

    python tools/re/unref.py app/client/avatar.cpp     (or a unit name: avatar)
    python tools/re/unref.py avatar --all              also those already in the tree
    python tools/re/unref.py avatar --range 0x77B870-0x78AA00

VC6 auto-inlines a member nobody references into its callers, and the member's own body stays in the
exe: that is how the client ends up calling a nested helper (??_G, clear, erase ...) out of line in one
function and inlining it in another. This lists, in the file's address range (the first to the last
function docs/data/app-functions.json attributes to it), each verified function start (D27) with
  - no `call` / `jmp rel32` to it or to its incremental-link thunk anywhere in .text, and
  - no dword holding its address or its thunk's in .rdata / .data (vtables, tables),
that is not yet in the tree (neither in_tree nor ambiguous in the ledger docs/data/imports; --all shows those too).

One line per address: address, size (to the next start), `text-ref` when .text holds its address as a
dword (push offset, a function pointer: it has a user, so it is not a missing-caller case) and
`in-tree`/`todo`; then a count line. Standard library plus clientimage.py (capstone for the starts).
"""
import argparse
import os
import re
import struct
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
sys.path.insert(0, os.path.normpath(os.path.join(HERE, '..')))
from clientimage import DEFAULT_CLIENT, ClientImage  # noqa: E402
from claims import Index  # noqa: E402  (the file -> functions and done sets, as `claims.py resolve` reads them)


def resolve_file(idx, unit):
    """The app-functions key for UNIT: a path (either slash) or a bare unit name such as `avatar`."""
    key = unit.strip().replace('/', '\\').lower()
    if key in idx.by_file:
        return key
    for cand in (key + '.cpp', 'app\\client\\' + key + '.cpp'):
        if cand in idx.by_file:
            return cand
    hits = [k for k in idx.by_file if k.endswith('\\' + key) or k.endswith('\\' + key + '.cpp')]
    if len(hits) == 1:
        return hits[0]
    raise SystemExit(f'{unit}: ' + ('ambiguous: ' + ', '.join(sorted(hits)) if hits
                                    else 'no functions attributed to it in docs/data/app-functions.json'))


def references(img, targets):
    """(call/jmp-referenced, rdata/data-referenced, text-dword-referenced) subsets of TARGETS (starts and thunks)."""
    start, end, raw = img.section('.text')
    body = img.data[raw:raw + (end - start)]
    thunks = img.incremental_thunks()
    called = set()
    for m in re.finditer(rb'[\xE8\xE9]', body):
        o = m.start()
        if o + 5 > len(body):
            break
        dst = (start + o + 5 + struct.unpack_from('<i', body, o + 1)[0]) & 0xFFFFFFFF
        if dst in targets and (start + o) not in thunks:
            called.add(dst)
    data = set()
    for name in ('.rdata', '.data'):
        s, e, r = img.section(name)
        blob = img.data[r:r + (e - s)]
        for o in range(0, len(blob) - 3, 1):
            v = struct.unpack_from('<I', blob, o)[0]
            if v in targets:
                data.add(v)
    textdw = set(img.dword_refs(targets))
    return called, data, textdw


def main():
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0], epilog=__doc__.split('\n', 1)[1],
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('unit', help='an app file (app/client/avatar.cpp) or its name (avatar)')
    ap.add_argument('--range', help='0xSTART-0xEND instead of the file\'s own range')
    ap.add_argument('--all', action='store_true', help='also list the starts already in the tree')
    ap.add_argument('--client', default=DEFAULT_CLIENT)
    a = ap.parse_args()

    idx = Index()
    key = resolve_file(idx, a.unit)
    mine = idx.by_file[key]
    lo, hi = mine[0], mine[-1] + 1
    if a.range:
        m = re.match(r'^(0x[0-9a-f]+)\s*-\s*(0x[0-9a-f]+)$', a.range.strip(), re.I)
        if not m:
            raise SystemExit(f'not a range: {a.range!r}')
        lo, hi = int(m.group(1), 16), int(m.group(2), 16)

    img = ClientImage(a.client)
    starts, thunks = img.function_starts()
    inv = {}
    for t, tgt in thunks.items():
        inv.setdefault(tgt, []).append(t)
    cand = [s for s in starts if lo <= s < hi]
    targets = set()
    for s in cand:
        targets.add(s)
        targets.update(inv.get(s, []))
    called, data, textdw = references(img, targets)

    def referenced(s, group):
        return any(x in group for x in [s] + inv.get(s, []))

    nxt = {s: starts[i + 1] for i, s in enumerate(starts[:-1])}
    n = tree = 0
    for s in cand:
        if referenced(s, called) or referenced(s, data):
            continue
        intree = s in idx.done
        tree += intree
        if intree and not a.all:
            continue
        n += 1
        size = nxt.get(s, s) - s
        flags = ('text-ref ' if referenced(s, textdw) else '') + ('in-tree' if intree else 'todo')
        print(f'0x{s:X} size 0x{size:X} no-caller {flags}')
    print(f'{n} unreferenced starts listed in {key} 0x{lo:X}-0x{hi:X} '
          f'({len(cand)} starts; {tree} unreferenced already in the tree)')


if __name__ == '__main__':
    main()
