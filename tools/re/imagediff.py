"""Whole-image diff: our linked client.exe against the retail client.exe, section by section.

The end goal's measure (2026-11-24): the bytes of the retail image that our linked image reproduces at the
same place. A byte counts only when our image holds the same value at the same RVA (headers: the same file
offset). Nothing is masked in the first figure: the link's time stamp and the checksum count as differences until they
are set. The second, masked figure leaves out what no build can give, Net-7's URL edits and the three link time stamps
(NET7_EDITS below), so it is the same
against retail (249f8524) and EA's client (--client ClientPOC_EA's).

    python tools/re/imagediff.py                    compare build/programs/client/client.exe (as it stands)
    python tools/re/imagediff.py --link --pool 1    link the client first (build.py link --program client)
    python tools/re/imagediff.py --ours X.exe --map X.map
    python tools/re/imagediff.py --exact [--link]    the exact build (build.py link --program client-exact)

Prints the summary (the metric, the header differences, one line per section, the layout table, the first
differing ranges) and writes everything to --out (default .work/imagediff/):
    summary.json    every figure printed, for progress.py and for comparing two runs
    ranges.tsv      every differing range: section, client VA, end, length, who holds it in each image
    layout.tsv      every name both images hold: client VA, our VA, displacement, section
    headers.tsv     every header field of both images

The layout table pairs names: symbols.json's (client address -> linker name) with our link map's publics and
statics. A pair "in place" sits at the client's address; a run is a stretch of consecutive paired client
symbols with one displacement (the order inside it is the client's). Byte-exact functions cannot give an
identical image until the link places them where the client's link did: this table is that work's measure.
"""
import argparse
import bisect
import collections
import hashlib
import json
import os
import re
import struct
import subprocess
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import clientimage  # noqa: E402

REPO = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
OURS = os.path.join(REPO, 'build', 'programs', 'client', 'client.exe')
SYMBOLS = os.path.join(REPO, 'docs', 'data', 'symbols.json')
DIRS = ['export', 'import', 'resource', 'exception', 'security', 'basereloc', 'debug', 'copyright', 'globalptr',
        'tls', 'loadconfig', 'boundimport', 'iat', 'delayimport', 'comdescriptor', 'reserved']
COFF = [('Machine', 'H'), ('NumberOfSections', 'H'), ('TimeDateStamp', 'I'), ('PointerToSymbolTable', 'I'),
        ('NumberOfSymbols', 'I'), ('SizeOfOptionalHeader', 'H'), ('Characteristics', 'H')]
OPT = [('Magic', 'H'), ('MajorLinkerVersion', 'B'), ('MinorLinkerVersion', 'B'), ('SizeOfCode', 'I'),
       ('SizeOfInitializedData', 'I'), ('SizeOfUninitializedData', 'I'), ('AddressOfEntryPoint', 'I'),
       ('BaseOfCode', 'I'), ('BaseOfData', 'I'), ('ImageBase', 'I'), ('SectionAlignment', 'I'),
       ('FileAlignment', 'I'), ('MajorOperatingSystemVersion', 'H'), ('MinorOperatingSystemVersion', 'H'),
       ('MajorImageVersion', 'H'), ('MinorImageVersion', 'H'), ('MajorSubsystemVersion', 'H'),
       ('MinorSubsystemVersion', 'H'), ('Win32VersionValue', 'I'), ('SizeOfImage', 'I'), ('SizeOfHeaders', 'I'),
       ('CheckSum', 'I'), ('Subsystem', 'H'), ('DllCharacteristics', 'H'), ('SizeOfStackReserve', 'I'),
       ('SizeOfStackCommit', 'I'), ('SizeOfHeapReserve', 'I'), ('SizeOfHeapCommit', 'I'), ('LoaderFlags', 'I'),
       ('NumberOfRvaAndSizes', 'I')]
# What the final link must set by other means than code: counted as different all the same.
SET_AT_THE_END = {'TimeDateStamp', 'CheckSum'}
# The masked figure (the user, 2026-11-27), printed beside the plain one: only the bytes no build can ever give.
# (1) Net-7's edits: the install's client.exe (249f8524, "retail") and EA's (ClientPOC_EA, 82663b2f) are the same
# 8,642,625 bytes but for these 11 .data ranges (732 bytes, compared 2026-11-27): the auth host, the web and store
# URLs (EA's earthandbeyond.ea.com / auth.east.ea.com against Net-7's net-7.org / enb-emulator.com). Masked, the
# figure is the same against either client. (2) The three link time stamps (12 bytes): the file header's
# TimeDateStamp (the update, 2004-03-17), the CodeView debug entry's TimeDateStamp and the NB10 record's signature
# (the full link, 2004-02-05). Not masked, as a build reproduces them: the CheckSum (0 in retail), .rsrc (retail's
# resources give all 8,156 bytes at retail's RVA), the rich header, the PDB path, the NB10 age.
NET7_EDITS = clientimage.RETAIL_DIFFERENCES      # D287: the two retail images' only difference
EA_SHA256 = clientimage.EA_SHA256
EXACT = os.path.join(REPO, 'build', 'programs', 'client-exact', 'client-exact.exe')


def stamp_ranges(client):
    """The three link time stamps as client file ranges [(offset, length)]."""
    pe = struct.unpack_from('<I', client.data, 0x3C)[0]
    out = [(pe + 8, 4)]                                  # COFF TimeDateStamp
    opt = pe + 24
    rva, size = struct.unpack_from('<II', client.data, opt + 96 + 8 * 6)      # the debug directory
    raw = client.at_rva(rva, size)[0]
    for i in range(0, size, 28):
        out.append((rva_to_file(client, rva + i + 4), 4))                     # the entry's TimeDateStamp
        ptr = struct.unpack_from('<I', raw, i + 24)[0]                       # PointerToRawData
        if client.data[ptr:ptr + 4] == b'NB10':
            out.append((ptr + 8, 4))                                         # the NB10 signature
    return out


def rva_to_file(img, rva):
    for s in img.sections:
        if s.rva <= rva < s.rva + img.backed(s):
            return s.raw + rva - s.rva
    raise ValueError(hex(rva))


def equal_at(client, ours, off, length):
    """Bytes of client[off:off + length] that ours holds at the same place: the same file offset in the headers
    and the overlay (by its offset from each overlay's start), the same RVA in a section."""
    over = len(client.data) - len(client.overlay)
    if off >= over:
        o = off - over
        return sum(1 for x, y in zip(client.overlay[o:o + length], ours.overlay[o:o + length]) if x == y)
    for s in client.sections:
        if s.raw <= off < s.raw + client.backed(s):
            rva = s.rva + off - s.raw
            a, _ = client.at_rva(rva, length)
            b, have = ours.at_rva(rva, length)
            return length - sum(e - st for st, e in diff_ranges(a, b, have))
    return sum(1 for x, y in zip(client.data[off:off + length], ours.data[off:off + length]) if x == y)


def masked(client, ours):
    """(masked client bytes, of them equal in ours at the same place): NET7_EDITS and the link time stamps."""
    ranges = [(rva_to_file(client, va - client.base), n) for va, n in NET7_EDITS] + stamp_ranges(client)
    return sum(n for _, n in ranges), sum(equal_at(client, ours, o, n) for o, n in ranges)


Section = collections.namedtuple('Section', 'name rva vsize raw rsize flags')


class Image:
    """A PE32 image: header fields in order, sections, and the bytes behind an RVA."""

    def __init__(self, path):
        self.path = path
        self.data = d = open(path, 'rb').read()
        if d[:2] != b'MZ':
            sys.exit(f'{path}: not an MZ image')
        self.pe = pe = struct.unpack_from('<I', d, 0x3c)[0]
        if d[pe:pe + 4] != b'PE\0\0':
            sys.exit(f'{path}: no PE signature')
        self.fields = collections.OrderedDict()
        self.fields['e_lfanew'] = pe
        self.fields['dos stub sha1'] = hashlib.sha1(d[0x40:pe]).hexdigest()[:12]
        at = pe + 4
        for group in (COFF, OPT):
            for name, fmt in group:
                self.fields[name] = struct.unpack_from('<' + fmt, d, at)[0]
                at += struct.calcsize('<' + fmt)
        for i in range(min(self.fields['NumberOfRvaAndSizes'], 16)):
            rva, size = struct.unpack_from('<II', d, at + 8 * i)
            self.fields[f'dir {DIRS[i]}'] = (rva, size)
        self.base = self.fields['ImageBase']
        table = pe + 24 + self.fields['SizeOfOptionalHeader']
        self.sections = []
        for i in range(self.fields['NumberOfSections']):
            name, vsize, rva, rsize, raw, _, _, _, _, flags = struct.unpack_from('<8sIIIIIIHHI', d, table + 40 * i)
            self.sections.append(Section(name.rstrip(b'\0').decode('latin1'), rva, vsize, raw, rsize, flags))
        self.headers_end = min([s.raw for s in self.sections if s.rsize] + [len(d)])
        last = max([s.raw + s.rsize for s in self.sections] + [self.headers_end])
        self.overlay = d[last:]

    def section(self, name):
        return next((s for s in self.sections if s.name == name), None)

    def backed(self, s):
        """Length of a section's bytes that the file holds (the rest of vsize is zero at load)."""
        return min(s.vsize, s.rsize) if s.vsize else s.rsize

    def at_rva(self, rva, length):
        """The file's bytes for [rva, rva + length); None where no section's raw data backs it."""
        out = bytearray(length)
        have = bytearray(length)                       # 1 where backed
        for s in self.sections:
            lo, hi = max(rva, s.rva), min(rva + length, s.rva + self.backed(s))
            if lo < hi:
                out[lo - rva:hi - rva] = self.data[s.raw + lo - s.rva:s.raw + hi - s.rva]
                have[lo - rva:hi - rva] = b'\1' * (hi - lo)
        return bytes(out), bytes(have)


def diff_ranges(a, b, absent=None):
    """Offsets where a and b differ, as [start, end) runs; `absent` (bytes, 0 = our image has no byte there)
    counts as different."""
    n = len(a)
    x = (int.from_bytes(a, 'little') ^ int.from_bytes(b, 'little')).to_bytes(n, 'little') if n else b''
    if absent is not None:
        x = bytes(p | (q ^ 1) for p, q in zip(x, absent)) if absent.count(0) else x
    return [(m.start(), m.end()) for m in re.finditer(rb'[^\x00]+', x)]


def merged(ranges, gap):
    """Runs closer than `gap` equal bytes read as one range (equal bytes by chance split a moved block)."""
    out = []
    for s, e in ranges:
        if out and s - out[-1][1] < gap:
            out[-1][1] = e
        else:
            out.append([s, e])
    return out


class Names:
    """Sorted (va, name) for 'who holds this address'."""

    def __init__(self, pairs, reach=0x4000):
        self.reach = reach                             # further than this from a name: nobody we know
        self.items = sorted(set(pairs))
        self.keys = [va for va, _ in self.items]

    def at(self, va):
        i = bisect.bisect_right(self.keys, va) - 1
        if i < 0:
            return ''
        start, name = self.items[i]
        if va - start > self.reach:
            return ''
        return name if va == start else f'{name}+0x{va - start:X}'


MAPLINE = re.compile(r'^ (\d{4}):([0-9a-f]{8})\s+(\S+)\s+([0-9a-f]{8}) (.)(?: (.))?\s+(\S+)?')


def read_map(path):
    """[(va, name, 'lib:object')] of the link map's publics and statics (absolute and import rows left out)."""
    out = []
    if not path or not os.path.exists(path):
        return out
    for line in open(path, encoding='latin1'):
        m = MAPLINE.match(line)
        if m and m.group(1) != '0000':
            out.append((int(m.group(4), 16), m.group(3), m.group(7) or ''))
    return out


def client_symbols(path=SYMBOLS):
    return [(int(s['address'], 16), s['symbol'], s.get('kind', '')) for s in json.load(open(path))]


def layout(client, ours_img, client_syms, our_map):
    """Pair names; returns rows (client va, our va, name, client section) and the figures."""
    ours = collections.defaultdict(list)
    for va, name, _ in our_map:
        ours[name].append(va)
    rows = []
    for va, name, kind in sorted(client_syms):
        hits = ours.get(name)
        if hits and len(hits) == 1:                    # a static name twice in our map pairs with nothing
            sec = next((s.name for s in client.sections if s.rva <= va - client.base < s.rva + max(s.vsize, s.rsize)), '?')
            rows.append((va, hits[0], name, sec))
    by_sec = collections.OrderedDict()
    for sec in [s.name for s in client.sections]:
        part = [r for r in rows if r[3] == sec]
        if not part:
            continue
        runs = []
        for va, mine, name, _ in part:
            d = mine - va
            if runs and runs[-1]['displacement'] == d:
                runs[-1]['count'] += 1
                runs[-1]['client_end'] = va
            else:
                runs.append({'displacement': d, 'count': 1, 'client_start': va, 'client_end': va, 'first': name})
        order = sum(1 for p, q in zip(part, part[1:]) if q[1] > p[1])
        by_sec[sec] = {
            'paired': len(part), 'in_place': sum(1 for r in part if r[0] == r[1]), 'runs': len(runs),
            'in_client_order': order, 'steps': max(len(part) - 1, 0),
            'longest_runs': sorted(runs, key=lambda r: -r['count'])[:5],
        }
    return rows, by_sec


def compare(client, ours, gap):
    """Per client section: the bytes equal at the same RVA, and the differing ranges (client VAs)."""
    out = []
    for s in client.sections:
        n = client.backed(s)
        a = client.data[s.raw:s.raw + n]
        b, have = ours.at_rva(s.rva, n)
        exact = diff_ranges(a, b, have)
        differing = sum(e - st for st, e in exact)
        mine = ours.section(s.name)
        # the same bytes at the same offset of the same-named section: what would match if the section moved
        by_offset = None
        if mine and mine.rva != s.rva:
            m = min(n, ours.backed(mine))
            by_offset = m - sum(e - st for st, e in diff_ranges(a[:m], ours.data[mine.raw:mine.raw + m]))
        out.append({
            'name': s.name, 'va': client.base + s.rva, 'vsize': s.vsize, 'rsize': s.rsize, 'bytes': n,
            'equal': n - differing, 'absent': have.count(0),
            'ours': None if not mine else {'va': ours.base + mine.rva, 'vsize': mine.vsize, 'rsize': mine.rsize},
            'equal_by_section_offset': by_offset,
            'ranges': [(client.base + s.rva + st, client.base + s.rva + e) for st, e in merged(exact, gap)],
        })
    return out


def header_rows(client, ours):
    rows = []
    for k in client.fields:
        a, b = client.fields[k], ours.fields.get(k)
        rows.append((k, a, b, a == b))
    names = [s.name for s in client.sections] + [s.name for s in ours.sections if not client.section(s.name)]
    for n in names:
        a, b = client.section(n), ours.section(n)
        for f in ('rva', 'vsize', 'raw', 'rsize', 'flags'):
            x, y = (getattr(a, f) if a else None), (getattr(b, f) if b else None)
            rows.append((f'section {n} {f}', x, y, x == y))
    return rows


def show(v):
    if v is None:
        return '-'
    if isinstance(v, tuple):
        return '%08X+%X' % v
    return '%X' % v if isinstance(v, int) else str(v)


def main():
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    ap.add_argument('--ours', default=OURS)
    ap.add_argument('--exact', action='store_true', help='compare the exact build (build.py link --program client-exact)')
    ap.add_argument('--map', help="our link map (default: the exe's name with .map)")
    ap.add_argument('--client', default=clientimage.DEFAULT_CLIENT)
    ap.add_argument('--link', action='store_true', help='link the client first (build.py link --program client)')
    ap.add_argument('--pool', type=int, default=1, help='--link: VMs to lease')
    ap.add_argument('--out', default=os.path.join(REPO, '.work', 'imagediff'))
    ap.add_argument('--gap', type=int, default=16, help='ranges closer than this many equal bytes are one (16)')
    ap.add_argument('--ranges', type=int, default=12, help='differing ranges printed (all go to ranges.tsv)')
    a = ap.parse_args()
    program = 'client-exact' if a.exact else 'client'
    if a.exact and a.ours == OURS:
        a.ours = EXACT

    if a.link:
        log = os.path.join(REPO, '.work', 'log', 'imagediff-link.log')
        os.makedirs(os.path.dirname(log), exist_ok=True)
        cmd = [sys.executable, os.path.join(REPO, 'tools', 'vc6', 'build.py'), 'link', '--program', program,
               '--pool', str(a.pool)]
        if subprocess.run(cmd, stdout=open(log, 'w'), stderr=subprocess.STDOUT).returncode:
            sys.exit(f'the link failed: {log}')
    if not os.path.exists(a.ours):
        sys.exit(f'{a.ours}: no linked client (run with --link, or tools/vc6/build.py link --program client)')
    clientimage.ClientImage(a.client)                  # the SHA-256 check: either retail client (D287)
    client, ours = Image(a.client), Image(a.ours)
    os.makedirs(a.out, exist_ok=True)

    # headers: every byte of the client's header area against ours at the same file offset
    hn = client.headers_end
    hdiff = diff_ranges(client.data[:hn], ours.data[:hn].ljust(hn, b'\0'))
    head_equal = hn - sum(e - s for s, e in hdiff)
    rows = header_rows(client, ours)
    with open(os.path.join(a.out, 'headers.tsv'), 'w') as f:
        f.write('field\tclient\tours\tsame\n')
        for k, x, y, same in rows:
            f.write(f'{k}\t{show(x)}\t{show(y)}\t{"yes" if same else "NO"}\n')

    secs = compare(client, ours, a.gap)
    over_equal = sum(1 for x, y in zip(client.overlay, ours.overlay) if x == y)
    total = len(client.data)
    counted = hn + sum(s['bytes'] for s in secs) + len(client.overlay)
    equal = head_equal + sum(s['equal'] for s in secs) + over_equal
    mask_n, mask_eq = masked(client, ours)

    our_map = read_map(a.map or os.path.splitext(a.ours)[0] + '.map')
    csyms = client_symbols()
    lay_rows, lay = layout(client, ours, csyms, our_map)
    cnames = Names((va, n) for va, n, _ in csyms)
    onames = Names((va, f'{n} [{o}]') for va, n, o in our_map)
    with open(os.path.join(a.out, 'ranges.tsv'), 'w') as f:
        f.write('section\tclient_va\tend\tlength\tclient_holds\tours_holds\n')
        for s in secs:
            for st, e in s['ranges']:
                f.write(f"{s['name']}\t0x{st:X}\t0x{e:X}\t{e - st}\t{cnames.at(st)}\t{onames.at(st)}\n")
    with open(os.path.join(a.out, 'layout.tsv'), 'w') as f:
        f.write('client_va\tour_va\tdisplacement\tsection\tname\n')
        for va, mine, name, sec in lay_rows:
            f.write(f'0x{va:X}\t0x{mine:X}\t{mine - va}\t{sec}\t{name}\n')

    summary = {
        'client': {'path': a.client, 'size': total, 'sha256': hashlib.sha256(client.data).hexdigest()},
        'ours': {'path': os.path.relpath(a.ours, REPO), 'size': len(ours.data),
                 'sha256': hashlib.sha256(ours.data).hexdigest(), 'mtime': int(os.path.getmtime(a.ours))},
        'identical': client.data == ours.data,
        'equal_bytes': equal, 'client_bytes': total, 'counted_bytes': counted,
        'percent': round(100.0 * equal / total, 4),
        'masked': {'bytes': mask_n, 'equal_in_mask': mask_eq, 'equal_bytes': equal - mask_eq,
                   'client_bytes': total - mask_n, 'percent': round(100.0 * (equal - mask_eq) / (total - mask_n), 4),
                   'what': 'NET7_EDITS (11 .data ranges) and the three link time stamps'},
        'headers': {'bytes': hn, 'equal': head_equal,
                    'different_fields': [k for k, _, _, same in rows if not same]},
        'overlay': {'client': len(client.overlay), 'ours': len(ours.overlay), 'equal': over_equal},
        'sections': [{k: v for k, v in s.items() if k != 'ranges'} | {'ranges': len(s['ranges'])} for s in secs],
        'layout': lay,
    }
    json.dump(summary, open(os.path.join(a.out, 'summary.json'), 'w'), indent=1)

    print(f"image    {'IDENTICAL' if summary['identical'] else 'differs'}: {equal:,} of {total:,} client bytes equal "
          f"at the same place ({summary['percent']:.2f} %); ours is {len(ours.data):,} bytes")
    m = summary['masked']
    print(f"masked   {m['equal_bytes']:,} of {m['client_bytes']:,} bytes equal ({m['percent']:.2f} %) without the "
          f"{mask_n:,} bytes of Net-7's 11 URL edits and the 3 link time stamps (the same figure against EA's client)")
    bad = [(k, x, y) for k, x, y, same in rows if not same]
    print(f'headers  {head_equal:,} of {hn:,} bytes equal; {len(bad)} of {len(rows)} fields differ'
          + (' (headers.tsv):' if bad else ''))
    for k, x, y in bad[:14]:
        late = '   (set at the end)' if k in SET_AT_THE_END else ''
        print(f'           {k:28s} client {show(x):>14s}   ours {show(y):>14s}{late}')
    if len(bad) > 14:
        print(f'           ... {len(bad) - 14} more')
    print('section    client VA    bytes        equal      %     ranges   ours')
    for s in secs:
        o = s['ours']
        mine = 'absent' if not o else f"VA {o['va']:X} vsize {o['vsize']:X}" + (
            '' if s['equal_by_section_offset'] is None else f", {s['equal_by_section_offset']:,} equal by section offset")
        pct = 100.0 * s['equal'] / s['bytes'] if s['bytes'] else 100.0
        print(f"{s['name']:8s}   {s['va']:08X}  {s['bytes']:>10,}  {s['equal']:>10,}  {pct:5.1f}  {len(s['ranges']):>8,}   {mine}")
    if client.overlay or ours.overlay:
        print(f"overlay    client {len(client.overlay)} bytes after the last section, ours {len(ours.overlay)}; {over_equal} equal")
    print('layout (names in both images: symbols.json against our map)')
    print('section    paired  in place   runs  steps in client order  longest run (displacement, first name)')
    for sec, v in lay.items():
        r = v['longest_runs'][0]
        print(f"{sec:8s}  {v['paired']:>7,}  {v['in_place']:>8,}  {v['runs']:>5,}  {v['in_client_order']:>7,} of {v['steps']:<7,}"
              f"  {r['count']:,} at {r['displacement']:+#x} from 0x{r['client_start']:X} {r['first'][:48]}")
    shown = 0
    print(f'first differing ranges (gap {a.gap}; all in {os.path.relpath(a.out, REPO)}\\ranges.tsv):')
    for s in secs:
        for st, e in s['ranges'][:max(1, a.ranges // max(len(secs), 1))]:
            if shown < a.ranges:
                print(f"  {s['name']:7s} 0x{st:X}-0x{e:X} {e - st:>9,}  client {cnames.at(st)[:44] or '?'} | ours {onames.at(st)[:60] or '-'}")
                shown += 1
    return 0 if summary['identical'] else 1


if __name__ == '__main__':
    sys.exit(main())
