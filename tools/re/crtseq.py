"""The per-object CRT initialiser check: retail's app-stretch .CRT$XCU table against ours predicted from the objects.

IM8's zseq.py (run 2026-11-26) with IM7's objseq.py / rkinds.py, as one standing tool. Retail's table (XCU_TABLE in
.data, the entries whose initialiser, through its incremental-link thunk, lies in the app region below LIB_LO) as
kinds, with its zero slots (Z) kept: CTYPE (reads the ctype<unsigned short>::id guard 0xC416FC), PERMSTR (first call
PermStringInitClass's constructor), IOS / WIOS (ios_base::Init / _Winit), other. Ours: each object of the order list
classified from the .obj (each .CRT$XCU relocation -> the initialiser's section -> its relocations: the ctype guard,
else the first call that is not atexit), then the zero slots the incremental link pads after an object of n entries
(1 -> 0, 2-6 -> 1, 7-11 -> 2, 12-17 -> 3, 18+ -> 4: measured on IM8's all trial, binary.md §5.6). The two sequences
(CTYPE, PERMSTR, X for the rest, Z) are aligned and each difference printed with retail's addresses and our objects.

    crtseq.py --build DIR --order LIST [--retail-kinds OUT.json] [--all-kinds]

LIST: one object per line, relative to DIR (e.g. app/obj/inpdsp.obj) or absolute, in the link order to test (a
trial link's order, or retail's as IM7 measured it); other lines are skipped. --all-kinds compares IOS / WIOS apart
(default: with `other` as X, as zseq did). Exit status 0 when the sequences agree, 1 otherwise.
"""
import argparse
import collections
import difflib
import json
import os
import struct
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)

CTYPE_GUARD = 0xC416FC
CTYPE_ID = '??_B?1???id@?$ctype@G@std@@'
LIB_LO = 0x8C1B64                 # retail's library region starts here (members.py, binary.md §5)
REL32 = 0x14
SHOW = 12                         # entries printed per side of one difference


def pad(n):
    """Zero slots the incremental link leaves after an object with n XCU entries (IM8's measurement)."""
    return 0 if n <= 1 else 1 if n <= 6 else 2 if n <= 11 else 3 if n <= 17 else 4


def coff(data):
    """([(section name, [(va, symbol index, type)])], {symbol index: (name, section, value)}) of a COFF object."""
    nsec, _, symptr, nsym, opt = struct.unpack_from('<HIIIH', data, 2)
    secs = []
    for i in range(nsec):
        o = 20 + opt + 40 * i
        name = data[o:o + 8].rstrip(b'\0')
        rptr, nrel = struct.unpack_from('<I', data, o + 24)[0], struct.unpack_from('<H', data, o + 32)[0]
        secs.append((name, [struct.unpack_from('<IIH', data, rptr + 10 * k) for k in range(nrel)]))
    strtab = symptr + 18 * nsym
    syms = {}
    i = 0
    while i < nsym:
        o = symptr + 18 * i
        if data[o:o + 4] == b'\0\0\0\0':
            off = struct.unpack_from('<I', data, o + 4)[0]
            name = data[strtab + off:data.index(b'\0', strtab + off)].decode('latin1')
        else:
            name = data[o:o + 8].rstrip(b'\0').decode('latin1')
        syms[i] = (name, struct.unpack_from('<h', data, o + 12)[0], struct.unpack_from('<I', data, o + 8)[0])
        i += 1 + data[o + 17]
    return secs, syms


def call_kind(name):
    if 'PermStringInitClass' in name:
        return 'PERMSTR'
    if '_Winit' in name:
        return 'WIOS'
    if 'Init@ios_base' in name:
        return 'IOS'
    return 'other'


def object_kinds(data):
    """[kind] of one object's .CRT$XCU entries, in relocation order."""
    secs, syms = coff(data)
    out = []
    for name, rels in secs:
        if name != b'.CRT$XCU':
            continue
        for _, si, _ in sorted(rels):
            fn, sec, _ = syms[si]
            if sec <= 0:
                out.append('other')
                continue
            frels = sorted(secs[sec - 1][1])
            if any(syms[s2][0].startswith(CTYPE_ID) for _, s2, _ in frels):
                out.append('CTYPE')
                continue
            first = next((syms[s2][0] for _, s2, t2 in frels if t2 == REL32 and 'atexit' not in syms[s2][0]), None)
            out.append(call_kind(first) if first else 'other')
    return out


def retail_kinds(img, names):
    """[(slot, kind or 'Z', initialiser va)] of retail's app stretch of the XCU table, zero slots inside it kept."""
    import capstone
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)

    def follow(t):
        o = img.offset(t)
        if o is not None and img.data[o] == 0xE9:
            return t + 5 + struct.unpack_from('<i', img.data, o + 1)[0]
        return t

    def kind(va):
        o = img.offset(va)
        for ins in md.disasm(bytes(img.data[o:o + 96]), va):
            if '0x%x' % CTYPE_GUARD in ins.op_str:
                return 'CTYPE'
            if ins.mnemonic == 'call':
                try:
                    t = follow(int(ins.op_str, 16))
                except ValueError:
                    return 'other'
                n = names.get(t, hex(t))
                if 'atexit' in n:
                    continue
                return call_kind(n)
            if ins.mnemonic == 'ret':
                break
        return 'other'

    lo, hi = img.XCU_TABLE
    out, started = [], False
    for slot in range(lo, hi, 4):
        t = img.u32(slot)
        if not t:
            if started:
                out.append((slot, 'Z', 0))
            continue
        f = follow(t)
        if f >= LIB_LO:
            if started:
                break
            continue
        started = True
        out.append((slot, kind(f), f))
    while out and out[-1][1] == 'Z':
        out.pop()
    return out


def our_kinds(build, order_lines, read=lambda p: open(p, 'rb').read()):
    """[(kind or 'Z', object name)] predicted for ORDER_LINES' objects (missing objects listed in the second value)."""
    out, missing = [], []
    for line in order_lines:
        line = line.split('#')[0].strip()
        if not line.endswith('.obj'):
            continue
        p = line if os.path.isabs(line) else os.path.join(build, line)
        try:
            ks = object_kinds(read(p))
        except OSError:
            missing.append(line)
            continue
        name = os.path.basename(line)
        out += [(k, name) for k in ks] + [('Z', name)] * pad(len(ks))
    while out and out[-1][0] == 'Z':
        out.pop()
    return out, missing


def coarse(k, all_kinds=False):
    return k if k in ('CTYPE', 'PERMSTR', 'Z') or (all_kinds and k in ('IOS', 'WIOS')) else 'X'


def compare(retail, ours, all_kinds=False):
    """The report lines and whether the two sequences agree."""
    ra = [coarse(k, all_kinds) for _, k, _ in retail]
    oa = [coarse(k, all_kinds) for k, _ in ours]
    lines = ['retail %d slots, %d zeros %s; ours %d slots, %d zeros %s' % (
        len(ra), ra.count('Z'), dict(collections.Counter(k for k in ra if k != 'Z')),
        len(oa), oa.count('Z'), dict(collections.Counter(k for k in oa if k != 'Z')))]
    sm = difflib.SequenceMatcher(a=ra, b=oa, autojunk=False)
    same = True
    for tag, i1, i2, j1, j2 in sm.get_opcodes():
        if tag == 'equal':
            continue
        same = False
        rr = ' '.join('%s:%#x' % (ra[i], retail[i][2]) if retail[i][2] else 'Z' for i in range(i1, min(i2, i1 + SHOW)))
        oo = ' '.join('%s:%s' % (oa[j], ours[j][1]) for j in range(j1, min(j2, j1 + SHOW)))
        rr += ' ... +%d' % (i2 - i1 - SHOW) if i2 - i1 > SHOW else ''
        oo += ' ... +%d' % (j2 - j1 - SHOW) if j2 - j1 > SHOW else ''
        prev = next((retail[i][2] for i in range(i1 - 1, -1, -1) if retail[i][2]), 0)
        at_r = '%#x' % prev if prev else '-'
        at_o = ours[j1 - 1][1] if j1 else '-'
        lines.append(f'{tag} after retail {at_r} / ours {at_o}: retail [{rr}] ours [{oo}]')
    return lines, same


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    ap.add_argument('--build', required=True, help='the build folder the order list is relative to')
    ap.add_argument('--order', required=True, help='the object order (one object per line)')
    ap.add_argument('--retail-kinds', help='also write retail\'s [[va, kind], ...] here')
    ap.add_argument('--all-kinds', action='store_true', help='IOS / WIOS apart from other')
    a = ap.parse_args(argv)
    import clientimage
    import members
    img = clientimage.ClientImage()
    names = {}
    for s in json.load(open(members.SYMBOLS, encoding='utf-8')):
        if s.get('kind') != 'thunk':
            names.setdefault(int(s['address'], 16), s['symbol'])
    retail = retail_kinds(img, names)
    if a.retail_kinds:
        json.dump([[va, k] for _, k, va in retail if k != 'Z'], open(a.retail_kinds, 'w'))
    ours, missing = our_kinds(a.build, open(a.order, encoding='utf-8').read().splitlines())
    lines, same = compare(retail, ours, a.all_kinds)
    if missing:
        lines.insert(1, 'missing objects %d: %s' % (len(missing), ' '.join(missing[:8])))
    print('\n'.join(lines))
    return 0 if same else 1


if __name__ == '__main__':
    sys.exit(main())
