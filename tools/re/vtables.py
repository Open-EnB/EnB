"""Compares compiled vtables with the client's: which virtual functions E&B added or removed.

    python vtables.py --objs DIR [--class NAME]...

For every vtable in the objects (`??_7Class@@6B@`), reads its slots (the functions its
relocations point at). Using the functions already proven byte-exact (objmatch over the same
objects, plus docs/data/bytematch-*.json), finds the client's copy of the vtable in .rdata, then
aligns the two slot lists. Reports, per class: the client vtable address, slots that match, and
the slots E&B's table has that ours doesn't (and the reverse), with neighbouring names, which
locates the virtual functions added or removed in E&B's headers. Also names client functions
reached only through vtables.
"""
import argparse
import bisect
import difflib
import glob
import json
import os
import struct
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from clientimage import ClientImage  # noqa: E402
from objmatch import CoffObject, demangle, match_objects  # noqa: E402

DATA = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 'docs', 'data'))


def vtables_in(obj):
    """{vtable symbol: [slot target symbol names]}"""
    out = {}
    for s in obj.symbols:
        if not s['name'].startswith('??_7') or s['section'] <= 0:
            continue
        sec = obj.sections[s['section'] - 1]
        start = s['value']
        slots = {}
        for va, symidx, typ in sec['relocs']:
            if typ == 0x06 and va >= start:
                target = next((x for x in obj.symbols if x['index'] == symidx), None)
                if target:
                    slots[(va - start) // 4] = target['name']
        n = max(slots) + 1 if slots else 0
        # stop at the first gap: the next vtable in a shared section, or RTTI data
        names = []
        for i in range(n):
            if i not in slots:
                break
            names.append(slots[i])
        out[s['name']] = names
    return out


def known_addresses(img, objs):
    """mangled symbol -> client address, for functions proven byte-exact."""
    known = {}
    for r in match_objects(img, objs):
        if r['status'] == 'exact':
            known.setdefault(r['symbol'], int(r['addresses'][0], 16))
    for f in glob.glob(os.path.join(DATA, 'bytematch-*.json')):
        for addr, v in json.load(open(f)).items():
            for e in v.values():
                if 'symbol' in e:
                    known.setdefault(e['symbol'], int(addr, 16))
    return known


def find_in_client(img, slots, known, rdata):
    """Best .rdata position for a vtable, by the number of known slots that line up."""
    start, end, raw = rdata
    data = img.data
    best = (0, None)
    anchors = [(i, known[s]) for i, s in enumerate(slots) if s in known]
    if not anchors:
        return None, 0
    i0, a0 = anchors[0]
    needle = struct.pack('<I', a0)
    pos = data.find(needle, raw, raw + (end - start))
    while pos >= 0:
        base = start + (pos - raw) - 4 * i0
        if (pos - raw) % 4 == 0:
            score = 0
            for i, a in anchors:
                o = raw + (base - start) + 4 * i
                if raw <= o < raw + (end - start) - 4 and struct.unpack_from('<I', data, o)[0] == a:
                    score += 1
            if score > best[0]:
                best = (score, base)
        pos = data.find(needle, pos + 1, raw + (end - start))
    return best[1], best[0]


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--objs', required=True)
    ap.add_argument('--class', dest='classes', action='append')
    ap.add_argument('--json')
    a = ap.parse_args()
    img = ClientImage()
    objs = sorted(glob.glob(os.path.join(a.objs, '*.obj')))
    known = known_addresses(img, objs)
    by_addr = {}
    for sym, addr in known.items():
        by_addr.setdefault(addr, sym)
    rdata = img.section('.rdata')
    tables = {}
    for p in objs:
        for vt, slots in vtables_in(CoffObject(p)).items():
            if len(slots) > len(tables.get(vt, [])):
                tables[vt] = slots
    report = []
    for vt, slots in sorted(tables.items()):
        cls = vt[4:].split('@@')[0]
        if a.classes and cls not in a.classes:
            continue
        base, score = find_in_client(img, slots, known, rdata)
        if not base:
            continue
        n = len(slots) + 16
        client = [struct.unpack_from('<I', img.data, img.offset(base) + 4 * i)[0] for i in range(n)]
        # stop the client table where it stops pointing into .text
        t0, t1, _ = img.section('.text')
        m = next((i for i, v in enumerate(client) if not (t0 <= v < t1)), n)
        client = client[:m]
        ours = [known.get(s) for s in slots]
        left = [f'{x:X}' if x else f'?{i}' for i, x in enumerate(ours)]
        right = [f'{x:X}' for x in client]
        sm = difflib.SequenceMatcher(None, left, right, autojunk=False)
        diffs = []
        for op, i1, i2, j1, j2 in sm.get_opcodes():
            if op == 'equal':
                continue
            if op == 'replace' and i2 - i1 == j2 - j1 and all(left[i].startswith('?') for i in range(i1, i2)):
                continue   # our slot unknown, client's present: same slot, not a difference
            ctx = demangle(slots[i1 - 1]) if i1 > 0 else '(start)'
            diffs.append({'op': op, 'ours': [demangle(s) for s in slots[i1:i2]],
                          'client': [f'0x{x:06X}' for x in client[j1:j2]],
                          'client_slot': j1, 'our_slot': i1, 'after': ctx})
        report.append({'class': cls, 'client_vtable': f'0x{base:06X}', 'slots': len(slots),
                       'client_slots': len(client), 'anchored': score, 'differences': diffs})
        if diffs or a.classes:
            print(f"{cls}: client vtable 0x{base:06X}, {len(slots)} slots here, {len(client)} in the client, "
                  f"{score} anchored")
            for d in diffs:
                print(f"   {d['op']:8s} after {d['after'][:40]:40s} ours {d['ours'][:4]} client {d['client'][:4]}"
                      f" (slot {d['our_slot']} / {d['client_slot']})")
    if a.json:
        json.dump(report, open(a.json, 'w'), indent=1)


if __name__ == '__main__':
    main()
