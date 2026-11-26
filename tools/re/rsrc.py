"""The client's own resources: retail's .rsrc as a .res file for a trial link, and a .rsrc against retail's.

Image builder IM4, 2026-11-25 (week plan 2026-11-24 §2 D question 3; docs/binary.md §5.3). client.exe's
.rsrc (0xC47000, 0x1FDC bytes, CVTRES 5.00.1735 in the rich header) holds 12 resources: a VERSIONINFO, a
TYPELIB, nine DIALOGs and one STRINGTABLE block. No outside data: `extract` writes them, in the order their data
sits in retail's section, as a 32-bit .res file that LINK takes on its command line (it runs CVTRES on it).

    python tools/re/rsrc.py extract -o build/res/client.res      retail's resources as a .res
    python tools/re/rsrc.py show [EXE]                           the tree of an image (default retail)
    python tools/re/rsrc.py compare OURS.exe                     our .rsrc against retail's

`compare` prints the bytes equal by section offset (as imagediff's "by section offset") and the bytes equal once
our section is moved to retail's RVA (the data entries' OffsetToData are RVAs: rebased by the two sections'
difference). The rebased figure is what the section brings into place once the sections before it end where
retail's do.

What a .res cannot carry into the image: the memory flags (not stored in a PE), and the directories'
TimeDateStamp / version fields, which CVTRES fills (retail's are 0).
"""
import argparse
import os
import struct
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import clientimage  # noqa: E402

RT = {1: 'CURSOR', 2: 'BITMAP', 3: 'ICON', 4: 'MENU', 5: 'DIALOG', 6: 'STRING', 9: 'ACCELERATOR',
      10: 'RCDATA', 12: 'GROUP_CURSOR', 14: 'GROUP_ICON', 16: 'VERSION', 24: 'MANIFEST'}
# RC's default memory flags per type (MOVEABLE | PURE [| DISCARDABLE]); not stored in a PE image.
MEMFLAGS = {5: 0x1030, 6: 0x1030, 4: 0x1030, 16: 0x0030}


def sections(data):
    """[(name, rva, vsize, raw, rsize)] of a PE32 image."""
    pe = struct.unpack_from('<I', data, 0x3c)[0]
    n, opt = struct.unpack_from('<H12xH', data, pe + 6)
    table = pe + 24 + opt
    out = []
    for i in range(n):
        name, vsize, rva, rsize, raw = struct.unpack_from('<8sIIII', data, table + 40 * i)
        out.append((name.rstrip(b'\0').decode('latin1'), rva, vsize, raw, rsize))
    return out


def rsrc_section(data):
    """(rva, bytes) of the image's .rsrc (the resource directory's section), or None."""
    pe = struct.unpack_from('<I', data, 0x3c)[0]
    rva, size = struct.unpack_from('<II', data, pe + 24 + 96 + 8 * 2)
    if not rva:
        return None
    for name, srva, vsize, raw, rsize in sections(data):
        if srva <= rva < srva + max(vsize, rsize):
            return srva, data[raw:raw + min(vsize, rsize) if vsize else rsize]
    return None


def _name(sec, off):
    n = struct.unpack_from('<H', sec, off)[0]
    return sec[off + 2:off + 2 + 2 * n].decode('utf-16-le')


def parse(sec, rva):
    """The tree: (dirs, resources). dirs: [(offset, characteristics, stamp, major, minor, named, ids)];
    resources: [dict(type, name, lang, data_rva, size, codepage, entry)] in tree order (type, name, lang as int
    or str). `entry` is the data entry's offset in the section."""
    dirs, res = [], []

    def walk(off, path):
        ch, stamp, ma, mi, nn, ni = struct.unpack_from('<IIHHHH', sec, off)
        dirs.append((off, ch, stamp, ma, mi, nn, ni))
        for i in range(nn + ni):
            name, target = struct.unpack_from('<II', sec, off + 16 + 8 * i)
            key = _name(sec, name & 0x7fffffff) if name & 0x80000000 else name
            if target & 0x80000000:
                walk(target & 0x7fffffff, path + [key])
            else:
                drva, size, cp, _ = struct.unpack_from('<IIII', sec, target)
                t, n_ = (path + [None, None])[:2]
                res.append({'type': t, 'name': n_, 'lang': key, 'data_rva': drva, 'size': size,
                            'codepage': cp, 'entry': target})
    walk(0, [])
    return dirs, res


def _id(v):
    if isinstance(v, int):
        return struct.pack('<HH', 0xffff, v)
    return v.encode('utf-16-le') + b'\0\0'


def res_entry(rtype, name, lang, payload, memflags=None, data_version=0, version=0, characteristics=0):
    """One resource of a 32-bit .res file: its header (DWORD-aligned) and its data padded to a DWORD."""
    ids = _id(rtype) + _id(name)
    ids += b'\0' * (-len(ids) % 4)
    if memflags is None:
        memflags = MEMFLAGS.get(rtype, 0x0030) if isinstance(rtype, int) else 0x0030
    tail = struct.pack('<IHHII', data_version, memflags, lang, version, characteristics)
    header_size = 8 + len(ids) + len(tail)
    return (struct.pack('<II', len(payload), header_size) + ids + tail + payload
            + b'\0' * (-len(payload) % 4))


def to_res(items):
    """A .res file: the empty first entry, then each (type, name, lang, payload) in the given order."""
    out = res_entry(0, 0, 0, b'', memflags=0)
    for t, n, lang, payload in items:
        out += res_entry(t, n, lang, payload)
    return out


def read_res(data):
    """[(type, name, lang, payload)] of a .res file, the empty first entry left out."""
    out, at = [], 0
    while at + 8 <= len(data):
        size, hsize = struct.unpack_from('<II', data, at)
        p = at + 8
        keys = []
        for _ in range(2):
            if struct.unpack_from('<H', data, p)[0] == 0xffff:
                keys.append(struct.unpack_from('<H', data, p + 2)[0])
                p += 4
            else:
                e = p
                while data[e:e + 2] != b'\0\0':
                    e += 2
                keys.append(data[p:e].decode('utf-16-le'))
                p = e + 2
        lang = struct.unpack_from('<H', data, at + hsize - 10)[0]
        payload = data[at + hsize:at + hsize + size]
        if size or keys != [0, 0]:
            out.append((keys[0], keys[1], lang, payload))
        at += hsize + size + (-size % 4)
    return out


def image_resources(data):
    """(section rva, section bytes, dirs, resources sorted by data RVA, each with its payload)."""
    srva, sec = rsrc_section(data)
    dirs, res = parse(sec, srva)
    for r in res:
        r['payload'] = sec[r['data_rva'] - srva:r['data_rva'] - srva + r['size']]
    return srva, sec, dirs, sorted(res, key=lambda r: r['data_rva'])


def rebased(sec, srva, to_rva):
    """Our section's bytes with every data entry's OffsetToData moved from srva to to_rva."""
    out = bytearray(sec)
    for r in parse(bytes(sec), srva)[1]:
        struct.pack_into('<I', out, r['entry'], r['data_rva'] - srva + to_rva)
    return bytes(out)


def equal_bytes(a, b, length):
    """Bytes of a[:length] that b holds at the same offset (b shorter: the rest counts as different)."""
    return sum(1 for x, y in zip(a[:length], b[:length]) if x == y)


def compare(ours, client):
    """Figures for our .rsrc against retail's: dict(length, by_offset, rebased, ours_rva, client_rva)."""
    crva, csec = rsrc_section(client)
    pe = struct.unpack_from('<I', client, 0x3c)[0]
    length = struct.unpack_from('<I', client, pe + 24 + 96 + 8 * 2 + 4)[0]
    got = rsrc_section(ours)
    if not got:
        return {'length': length, 'by_offset': 0, 'rebased': 0, 'ours_rva': None, 'client_rva': crva}
    orva, osec = got
    return {'length': length, 'by_offset': equal_bytes(csec, osec, length),
            'rebased': equal_bytes(csec, rebased(osec, orva, crva), length), 'ours_rva': orva, 'client_rva': crva}


def _label(v, top=False):
    return RT.get(v, str(v)) if top and isinstance(v, int) else str(v)


def main():
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    sub = ap.add_subparsers(dest='cmd', required=True)
    e = sub.add_parser('extract')
    e.add_argument('--client', default=clientimage.DEFAULT_CLIENT)
    e.add_argument('-o', '--out', required=True)
    s = sub.add_parser('show')
    s.add_argument('exe', nargs='?', default=clientimage.DEFAULT_CLIENT)
    c = sub.add_parser('compare')
    c.add_argument('ours')
    c.add_argument('--client', default=clientimage.DEFAULT_CLIENT)
    a = ap.parse_args()
    if a.cmd == 'extract':
        _, _, _, res = image_resources(open(a.client, 'rb').read())
        out = to_res([(r['type'], r['name'], r['lang'], r['payload']) for r in res])
        os.makedirs(os.path.dirname(os.path.abspath(a.out)), exist_ok=True)
        open(a.out, 'wb').write(out)
        print(f'{a.out}: {len(res)} resources, {len(out):,} bytes')
    elif a.cmd == 'show':
        srva, sec, dirs, res = image_resources(open(a.exe, 'rb').read())
        print(f'.rsrc at RVA {srva:#x}, {len(sec):,} bytes; {len(dirs)} directories, stamps / characteristics / '
              f'versions {sorted({d[1:5] for d in dirs})}')
        for r in res:
            print(f"  {_label(r['type'], True):8} {_label(r['name']):6} lang {r['lang']:5} data {r['data_rva']:#x} "
                  f"+{r['size']:<5} codepage {r['codepage']}")
    else:
        f = compare(open(a.ours, 'rb').read(), open(a.client, 'rb').read())
        if f['ours_rva'] is None:
            print(f".rsrc: ours has none; retail {f['length']:,} bytes at RVA {f['client_rva']:#x}")
        else:
            print(f".rsrc: retail {f['length']:,} bytes at RVA {f['client_rva']:#x}, ours at {f['ours_rva']:#x}: "
                  f"{f['by_offset']:,} equal by section offset, {f['rebased']:,} equal at retail's RVA "
                  f"({100 * f['rebased'] / f['length']:.1f} %)")


if __name__ == '__main__':
    main()
