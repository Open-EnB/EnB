"""Finds the functions of compiled .obj files in client.exe, byte for byte.

Each function in an object is turned into a byte pattern in which every relocated field (calls,
jumps to other functions, addresses of data and strings) is a wildcard, because those are only
fixed at link time. The pattern is searched for in client.exe's .text. Results per function:

  exact      found once: this is the function, unchanged
  multiple   found more than once (short or common code; not proof on its own)
  none       not found: the client's function differs, is inlined, or was not linked

    python objmatch.py [--client PATH] [--json OUT] file.obj...
"""
import argparse
import json
import os
import re
import struct
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from clientimage import ClientImage, DEFAULT_CLIENT  # noqa: E402

MIN_FIXED = 12   # a pattern needs at least this many non-wildcard bytes to be searched

RELOC_SIZE = {0x06: 4, 0x07: 4, 0x14: 4, 0x0B: 4, 0x0A: 2, 0x01: 2, 0x02: 2}


class CoffObject:
    """The parts of a COFF .obj that function matching needs."""

    def __init__(self, path):
        d = self.data = open(path, 'rb').read()
        self.path = path
        machine, nsec, _, symptr, nsym, optsize, _ = struct.unpack_from('<HHIIIHH', d, 0)
        if machine != 0x14C:
            raise ValueError(f'{path}: not an x86 object')
        self.sections = []
        for i in range(nsec):
            o = 20 + optsize + 40 * i
            name, vsize, va, rsize, raw, relptr, _, nrel, _, flags = struct.unpack_from('<8sIIIIIIHHI', d, o)
            relocs = [struct.unpack_from('<IIH', d, relptr + 10 * r) for r in range(nrel)]
            self.sections.append({'name': name.rstrip(b'\0').decode('latin1'), 'size': rsize, 'raw': raw,
                                  'relocs': relocs, 'flags': flags})
        strtab = symptr + 18 * nsym
        self.symbols = []
        i = 0
        while i < nsym:
            o = symptr + 18 * i
            raw = d[o:o + 8]
            if raw[:4] == b'\0\0\0\0':
                off = struct.unpack_from('<I', raw, 4)[0]
                name = d[strtab + off:d.index(b'\0', strtab + off)]
            else:
                name = raw.rstrip(b'\0')
            value, secnum, typ, cls, naux = struct.unpack_from('<IhHBB', d, o + 8)
            self.symbols.append({'name': name.decode('latin1'), 'value': value, 'section': secnum,
                                 'type': typ, 'class': cls, 'index': i})
            i += 1 + naux

    def functions(self):
        """[(name, section index, start, end)] for every function defined in a code section."""
        by_sec = {}
        for s in self.symbols:
            if s['section'] > 0 and (s['type'] >> 4) == 2 and s['class'] in (2, 3):
                by_sec.setdefault(s['section'] - 1, []).append(s)
        out = []
        for sec, syms in by_sec.items():
            info = self.sections[sec]
            if not info['flags'] & 0x20:  # IMAGE_SCN_CNT_CODE
                continue
            syms.sort(key=lambda s: s['value'])
            for k, s in enumerate(syms):
                end = syms[k + 1]['value'] if k + 1 < len(syms) else info['size']
                out.append((s['name'], sec, s['value'], end))
        return out

    def pattern(self, sec, start, end):
        """(regex bytes, fixed byte count, length) for a function, relocations as wildcards."""
        info = self.sections[sec]
        body = bytearray(self.data[info['raw'] + start:info['raw'] + end])
        # Trailing alignment padding is not part of the function.
        while body and body[-1] in (0xCC, 0x90):
            body.pop()
        wild = [False] * len(body)
        for va, _, typ in info['relocs']:
            n = RELOC_SIZE.get(typ, 4)
            for k in range(va - start, va - start + n):
                if 0 <= k < len(body):
                    wild[k] = True
        parts, fixed = [], 0
        for b, w in zip(body, wild):
            if w:
                parts.append(b'.')
            else:
                parts.append(re.escape(bytes([b])))
                fixed += 1
        return b''.join(parts), fixed, len(body)


def demangle(name):
    """Enough of MSVC demangling for reports and source lookup: ?Method@Class@@... -> Class::Method,
    ??0Class@@... -> Class::Class, ??1Class@@... -> Class::~Class, ??4Class@@... -> Class::operator=."""
    if not name.startswith('?'):
        return name.lstrip('_').split('@')[0]
    special = {'0': None, '1': '~', '4': 'operator=', '8': 'operator==', '9': 'operator!=',
               'A': 'operator[]', 'H': 'operator+', 'G': 'operator-', 'D': 'operator*',
               '_G': 'scalar deleting dtor', '_E': 'vector deleting dtor'}
    m = re.match(r'\?\?(_[A-Z]|[0-9A-Z])([^@?]+)@', name)
    if m and m.group(1) in special:
        code, cls = m.group(1), m.group(2)
        method = cls if code == '0' else '~' + cls if code == '1' else special[code]
        return f'{cls}::{method}'
    m = re.match(r'\?([^@?]+)@([^@?]*)@', name)
    if not m:
        return name
    method, cls = m.group(1), m.group(2)
    return f'{cls}::{method}' if cls else method


def match_objects(img, paths, region=None):
    """region=(lo, hi): search only [lo, hi) of .text (E&B's app region for app files: a pattern
    that also occurs in the libraries is still exact there)."""
    start, end, raw = img.section('.text')
    text = img.data[raw:raw + (end - start)]
    if region:
        lo, hi = max(region[0], start), min(region[1], end)
        text, start = text[lo - start:hi - start], lo
    results = []
    for path in paths:
        obj = CoffObject(path)
        for name, sec, s, e in obj.functions():
            pat, fixed, length = obj.pattern(sec, s, e)
            r = {'object': os.path.basename(path), 'symbol': name, 'name': demangle(name),
                 'length': length, 'fixed': fixed}
            if fixed < MIN_FIXED:
                r['status'] = 'short'
                r['addresses'] = []
            else:
                hits = [start + m.start() for m in re.finditer(pat, text, re.S)][:5]
                r['addresses'] = [f'0x{a:06X}' for a in hits]
                r['status'] = 'exact' if len(hits) == 1 else 'multiple' if hits else 'none'
            results.append(r)
    return results


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('objects', nargs='+')
    ap.add_argument('--client', default=DEFAULT_CLIENT)
    ap.add_argument('--json')
    ap.add_argument('-q', action='store_true', help='summary only')
    a = ap.parse_args()
    img = ClientImage(a.client)
    results = match_objects(img, a.objects)
    if a.json:
        json.dump(results, open(a.json, 'w'), indent=1)
    counts = {}
    for r in results:
        counts[r['status']] = counts.get(r['status'], 0) + 1
        if not a.q:
            print(f"{r['status']:8s} {r['object']:16s} {r['name'][:48]:48s} len={r['length']:5d} "
                  f"{' '.join(r['addresses'][:3])}")
    print(' '.join(f'{k}={v}' for k, v in sorted(counts.items())), f'total={len(results)}')


if __name__ == '__main__':
    main()
