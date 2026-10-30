"""Builds a symbol map of client.exe from everything proven so far, and a coverage report.

    python symbols.py [--objs DIR...] [--out ../../docs/data]
    python symbols.py --apply-named     only apply symbols_named.json to the existing symbols.json
                                        (no regeneration: a regeneration is not neutral, workflow.md §3.4)

Sources, best first (a name from a better source wins):
  exact     functions proven byte-identical: docs/data/bytematch-*.json (EA copies and the
            Microsoft libraries) plus objmatch over the given object directories
  vtable    client vtables located from the objects' vtables; slots aligned with ours name
            functions that have no bytes of their own to match (e.g. inline virtuals compiled
            into E&B's code), and each vtable itself is named
  named     a name given by hand with its evidence (tools/re/symbols_named.json: a constructor's
            `new` size and vtable store, a caller's assert...); it overrides a wrong strings match
  strings   string-anchored matches (docs/data/funcmatch.json, high/medium confidence)
  thunk     incremental-link thunks named after their target (j_<name>)

Writes:
  symbols.json        [{address, name, symbol, kind, source[, proof][, length]}]; `proof` (D114(c)) is the
                      non-exact evidence (vtable, strings) an exact name also has
  ghidra_symbols.py   a Ghidra script that labels the program with these names
  coverage.md         how much of .text is identified, by region and source
"""
import argparse
import bisect
import glob
import json
import os
import struct
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from clientimage import ClientImage  # noqa: E402
from objmatch import CoffObject, demangle, match_objects  # noqa: E402
from vtables import confident, find_in_client, vptr_stored, vtables_in  # noqa: E402

HERE = os.path.dirname(os.path.abspath(__file__))
DATA = os.path.normpath(os.path.join(HERE, '..', '..', 'docs', 'data'))
RANK = {'exact': 0, 'vtable': 1, 'named': 2, 'strings': 3, 'thunk': 4}
NAMED = os.path.join(HERE, 'symbols_named.json')
LIB_START = 0x8C1B50


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--objs', action='append', default=[], help='object directories (vtables, exact names)')
    ap.add_argument('--out', default=DATA)
    ap.add_argument('--apply-named', action='store_true',
                    help='apply symbols_named.json to the existing symbols.json only (no regeneration)')
    a = ap.parse_args()
    img = ClientImage()
    starts, thunks = img.function_starts()
    t0, t1, _ = img.section('.text')
    syms = {}   # address -> entry
    if a.apply_named:
        for r in json.load(open(os.path.join(a.out, 'symbols.json'))):
            syms[int(r['address'], 16)] = r

    def put(addr, symbol, kind, source, length=None):
        if symbol.startswith(('_$E', 'j__$E')):
            return      # D94(b): compiler-generated, numbered per translation unit (P-H7-2)
        e = syms.get(addr)
        if e and RANK[e['source']] <= RANK[source]:
            # D114(c): an exact name keeps the best non-exact proof of the same symbol (a vtable slot, a
            # string) in `proof`, so objmatch's self-name round (D88) still sees that evidence
            if e['source'] == 'exact' and source in ('vtable', 'named', 'strings') and e['symbol'] == symbol \
                    and RANK[source] < RANK[e.get('proof', 'thunk')]:
                e['proof'] = source
            return
        new = {'address': f'0x{addr:06X}', 'symbol': symbol, 'name': demangle(symbol) if symbol.startswith(('?', '_')) else symbol,
               'kind': kind, 'source': source}
        if e and source == 'exact' and e['symbol'] == symbol and e['source'] in ('vtable', 'named', 'strings'):
            new['proof'] = e['source']
        syms[addr] = new
        if length:
            syms[addr]['length'] = length

    def put_named():
        for m in json.load(open(NAMED)):
            addr = int(m['address'], 16)
            e = syms.get(addr)
            if a.apply_named and e and e['source'] in ('strings', 'thunk', 'named'):
                del syms[addr]      # a hand-given name replaces a weaker one in place
            put(addr, m['symbol'], 'function', 'named')

    def put_thunks():
        for th, target in thunks.items():
            if target in syms and syms[target]['kind'] == 'function':
                e = syms.get(th)
                if a.apply_named and e and e['source'] == 'thunk' and e['symbol'] != 'j_' + syms[target]['symbol']:
                    del syms[th]    # the target was renamed
                put(th, 'j_' + syms[target]['symbol'], 'thunk', 'thunk')

    if a.apply_named:
        put_named()
        put_thunks()
        finish(a, img, starts, thunks, syms, 0)
        return

    # 1. exact: report JSONs
    for f in sorted(glob.glob(os.path.join(DATA, 'bytematch-*.json'))):
        for addr, v in json.load(open(f)).items():
            e = next(iter(v.values()))
            put(int(addr, 16), e.get('symbol') or e['function'], 'function', 'exact', e.get('length'))
    # ... and the given objects (the repository's own compiled libraries)
    objs = [p for d in a.objs for p in sorted(glob.glob(os.path.join(d, '*.obj')))]
    known = {}
    for r in match_objects(img, objs) if objs else []:
        if r['status'] == 'exact':
            addr = int(r['addresses'][0], 16)
            put(addr, r['symbol'], 'function', 'exact', r['length'])
            known.setdefault(r['symbol'], addr)
    for addr, e in syms.items():
        known.setdefault(e['symbol'], addr)

    # 2. vtables
    rdata = img.section('.rdata')
    tables = {}
    for p in objs:
        try:
            for vt, slots in vtables_in(CoffObject(p)).items():
                if len(slots) > len(tables.get(vt, [])):
                    tables[vt] = slots
        except Exception:
            continue
    vt_named = 0
    for vt, slots in tables.items():
        base, score, score_own, rarest = find_in_client(img, slots, known, rdata, vt[4:].split('@@')[0])
        if not base or score < 2 or not confident(score_own, rarest):
            continue
        stored = vptr_stored(img, [base + 4 * k for k in range(-4, 5)])
        if stored and base not in stored:
            continue        # slots were inserted/removed before the anchors: don't trust the alignment
        put(base, vt, 'vtable', 'vtable')
        client = []
        for i in range(len(slots)):
            o = img.offset(base + 4 * i)
            v = struct.unpack_from('<I', img.data, o)[0]
            if not (t0 <= v < t1):
                break
            client.append(v)
        if len(client) != len(slots):
            continue        # different length: alignment not safe slot by slot
        # all anchored slots must agree before naming the others
        if any(known.get(s) not in (None, thunks.get(c, c)) and known.get(s) != c for s, c in zip(slots, client)):
            continue
        for s, c in zip(slots, client):
            target = thunks.get(c, c)
            if s != 'purecall' and not s.startswith('__purecall'):
                if target not in syms:
                    vt_named += 1
                put(target, s, 'function', 'vtable')

    # 3. named by hand, with evidence
    put_named()

    # 4. string-anchored
    try:
        for m in json.load(open(os.path.join(DATA, 'funcmatch.json'))):
            if m['confidence'] in ('high', 'medium'):
                put(int(m['client'], 16), m['function'], 'function', 'strings')
    except OSError:
        pass

    # 5. thunks
    put_thunks()
    finish(a, img, starts, thunks, syms, vt_named)


def finish(a, img, starts, thunks, syms, vt_named):
    rows = [syms[k] for k in sorted(syms)]
    os.makedirs(a.out, exist_ok=True)
    json.dump(rows, open(os.path.join(a.out, 'symbols.json'), 'w'), indent=0)
    write_ghidra(rows, os.path.join(a.out, 'ghidra_symbols.py'))
    if not a.apply_named:   # its vtable count belongs to the last regeneration
        write_coverage(img, starts, thunks, rows, os.path.join(a.out, '..', 'coverage.md'), vt_named)
    by = {}
    for r in rows:
        by[(r['kind'], r['source'])] = by.get((r['kind'], r['source']), 0) + 1
    print(f'{len(rows)} symbols; ' + ', '.join(f'{k[0]}/{k[1]}={v}' for k, v in sorted(by.items())))


def write_ghidra(rows, path):
    with open(path, 'w', encoding='utf-8') as f:
        f.write('# Ghidra script (Jython): labels client.exe with the names in symbols.json.\n'
                '# Generated by tools/re/symbols.py. Run it from the Script Manager on client.exe\n'
                '# (SHA-256 249f8524...51c4).\n'
                '# @category EnB\n'
                'from ghidra.program.model.symbol import SourceType\n'
                'SYMBOLS = [\n')
        for r in rows:
            name = r['name'].replace('::', '__').replace('~', 'dtor_').replace(' ', '_')
            name = ''.join(c if (c.isalnum() or c == '_') else '_' for c in name)[:200]
            f.write(f"    (0x{int(r['address'], 16):08X}, {name!r}, {r['kind']!r}),\n")
        f.write(']\n'
                'st = currentProgram.getSymbolTable()\n'
                'for addr, name, kind in SYMBOLS:\n'
                '    a = toAddr(addr)\n'
                '    if kind in ("function", "thunk") and getFunctionAt(a) is None:\n'
                '        createFunction(a, None)\n'
                '    st.createLabel(a, name, SourceType.IMPORTED)\n'
                'print("labelled %d addresses" % len(SYMBOLS))\n')


def write_coverage(img, starts, thunks, rows, path, vt_named):
    t0, t1, _ = img.section('.text')
    func_rows = [r for r in rows if r['kind'] == 'function']
    named = {int(r['address'], 16): r for r in func_rows}

    def flen(a):
        k = bisect.bisect_right(starts, a)
        return min((starts[k] if k < len(starts) else t1) - a, 0x4000)

    regions = {'app (E&B, 0x401000-0x8C1B50)': (t0, LIB_START), 'libraries (0x8C1B50-end)': (LIB_START, t1)}
    lines = ['# client.exe coverage (generated by tools/re/symbols.py)', '',
             'Functions named so far, by region and by the kind of evidence. "Exact" means proven byte '
             'for byte; "vtable" named through an aligned vtable; "strings" matched by shared strings '
             '(lower confidence; the column includes the few names given by hand with evidence, '
             'tools/re/symbols_named.json). Thunks (the incremental-link table) are not counted as functions.', '',
             '| Region | Functions found | Named | Exact | vtable | strings | Named bytes (approx.) |',
             '|---|---|---|---|---|---|---|']
    thunk_set = set(thunks)
    for label, (lo, hi) in regions.items():
        fs = [s for s in starts if lo <= s < hi and s not in thunk_set]
        nm = [named[s] for s in fs if s in named]
        cnt = {k: sum(1 for r in nm if r['source'] == k) for k in ('exact', 'vtable', 'strings', 'named')}
        cnt['strings'] += cnt['named']
        nbytes = sum(r.get('length') or flen(int(r['address'], 16)) for r in nm)
        lines.append(f"| {label} | {len(fs)} | {len(nm)} | {cnt['exact']} | {cnt['vtable']} | {cnt['strings']} | {nbytes // 1024} KB |")
    lines += ['', f'Functions first named through vtables in this run: {vt_named}.', '',
              'The library region also holds about 640 KB of linker padding and compiler-generated '
              'exception-handling stubs (docs/library-matching.md §3.7), which are not functions to name.', '']
    open(path, 'w', encoding='utf-8').write('\n'.join(lines) + '\n')


if __name__ == '__main__':
    main()
