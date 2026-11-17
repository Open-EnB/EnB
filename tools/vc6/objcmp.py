"""Compares two builds' COFF objects (dsp.py / msdev.py's IDE build against build.py's, D248).

    python tools/vc6/objcmp.py A_DIR B_DIR [--libs X,Y] [--show N]

A_DIR and B_DIR hold <lib>/obj/*.obj (build.py's --out layout; msdev.py fetch-objs writes the same).
Two objects count as the same when every section has the same bytes and the same relocations, each
relocation compared by its target symbol's name: the symbol table itself is not compared. It holds
the source path (the `.file` record: build.py names EA files by their real path, the IDE build by the
client's e:\\build\\beta\\G\\...), whose length moves every later symbol's index, and the compiler's
internal labels ($L1234), whose numbers differ between the two builds; neither is code. `--strict`
compares the whole file but the header's time stamp. For a differing object it names the sections.
Same sections and relocations mean the same (file, address) pairs: an IDE build whose every object is
the same as build.py's gives verify's result.
"""
import argparse
import os
import re
import struct
import sys


LABEL = re.compile(r'^\$([LT])\d+$')


def symbols(data):
    """{index: name} of a COFF object's symbol table ($L labels unnumbered)."""
    symtab, nsym = struct.unpack_from('<II', data, 8)
    strtab = symtab + nsym * 18
    out = {}
    i = 0
    while i < nsym:
        off = symtab + i * 18
        raw = data[off:off + 8]
        if raw[:4] == b'\0\0\0\0':
            p = strtab + struct.unpack_from('<I', raw, 4)[0]
            name = data[p:data.index(b'\0', p)].decode('latin1')
        else:
            name = raw.rstrip(b'\0').decode('latin1')
        m = LABEL.match(name)
        out[i] = '$' + m.group(1) if m else name
        i += 1 + data[off + 17]
    return out


def sections(data, named=False):
    """[(name, raw bytes, relocations)] of a COFF object; relocations as bytes, or with named=True as
    [(offset, type, target symbol's name)]."""
    nsec = struct.unpack_from('<H', data, 2)[0]
    opt = struct.unpack_from('<H', data, 16)[0]
    symtab, nsym = struct.unpack_from('<II', data, 8)
    strtab = symtab + nsym * 18
    syms = symbols(data) if named else None
    out = []
    for i in range(nsec):
        off = 20 + opt + i * 40
        name = data[off:off + 8].rstrip(b'\0').decode('latin1')
        if name.startswith('/'):
            p = strtab + int(name[1:])
            name = data[p:data.index(b'\0', p)].decode('latin1')
        size, ptr, prel, _, nrel = struct.unpack_from('<IIIIH', data, off + 16)
        rel = data[prel:prel + nrel * 10]
        if named:
            rel = [(struct.unpack_from('<I', rel, k)[0], struct.unpack_from('<H', rel, k + 8)[0],
                    syms.get(struct.unpack_from('<I', rel, k + 4)[0], '?')) for k in range(0, len(rel), 10)]
        out.append((name, data[ptr:ptr + size] if ptr else b'', rel))
    return out


def same(a, b, strict=False):
    """True, 'order' (the same sections, the COMDATs in another order: the same code, laid out
    otherwise in the exe), or False."""
    if strict:
        return a[:4] == b[:4] and a[8:] == b[8:]
    sa, sb = sections(a, True), sections(b, True)
    if sa == sb:
        return True
    key = lambda s: (s[0], s[1], repr(s[2]))  # noqa: E731
    return 'order' if sorted(sa, key=key) == sorted(sb, key=key) else False


def diff_sections(a, b):
    sa, sb = sections(a, True), sections(b, True)
    if len(sa) != len(sb):
        return ['section count %d != %d' % (len(sa), len(sb))]
    out = set()
    for x, y in zip(sa, sb):
        if x[0] != y[0]:
            out.add(f'{x[0]} / {y[0]}')
        elif x[1] != y[1]:
            out.add(x[0])
        elif x[2] != y[2]:
            out.add(x[0] + ' (relocations)')
    return sorted(out)


def compare(adir, bdir, libs=None, strict=False):
    res = {}
    for lib in sorted(os.listdir(adir)):
        if libs and lib not in libs:
            continue
        ao, bo = os.path.join(adir, lib, 'obj'), os.path.join(bdir, lib, 'obj')
        if not os.path.isdir(ao) or not os.path.isdir(bo):
            continue
        an = {f.lower(): f for f in os.listdir(ao) if f.lower().endswith('.obj')}
        bn = {f.lower(): f for f in os.listdir(bo) if f.lower().endswith('.obj')}
        r = {'same': [], 'order': [], 'differ': {}, 'only_a': sorted(set(an) - set(bn)), 'only_b': sorted(set(bn) - set(an))}
        for k in sorted(set(an) & set(bn)):
            a = open(os.path.join(ao, an[k]), 'rb').read()
            b = open(os.path.join(bo, bn[k]), 'rb').read()
            s = same(a, b, strict)
            if s == 'order':
                r['order'].append(k)
            elif s:
                r['same'].append(k)
            else:
                r['differ'][k] = diff_sections(a, b)
        res[lib] = r
    return res


def main():
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    ap.add_argument('a')
    ap.add_argument('b')
    ap.add_argument('--libs', help='comma-separated library names')
    ap.add_argument('--show', type=int, default=10, help='differing objects listed per library')
    ap.add_argument('--strict', action='store_true', help='compare whole files (the time stamp aside)')
    x = ap.parse_args()
    res = compare(x.a, x.b, set(x.libs.split(',')) if x.libs else None, x.strict)
    bad = 0
    for lib, r in res.items():
        code = {k: v for k, v in r['differ'].items() if any(not s.startswith(('.rdata', '.data', '.drectve')) for s in v)}
        print(f"{lib:12} same {len(r['same']):4}  COMDAT order {len(r['order']):3}  differ {len(r['differ']):4} (code {len(code)})  "
              f"only A {len(r['only_a'])}  only B {len(r['only_b'])}")
        for k, v in list(r['differ'].items())[:x.show]:
            print(f'    {k}: {", ".join(v)}')
        for k in (r['only_a'] + r['only_b'])[:x.show]:
            print(f'    one side only: {k}')
        bad += len(r['differ']) + len(r['only_a']) + len(r['only_b'])
    sys.exit(1 if bad else 0)


if __name__ == '__main__':
    main()
