"""Compare the initialised data of compiled objects with client.exe's bytes (D201; AU1's data audit).

Exact functions can't show a wrong table, so this checks the tables themselves. Run it when wanted,
not once per run. It reads existing objects and client.exe and writes nothing unless --json is given.

Where a data symbol sits in the client:
  1. an exact function (objmatch) whose relocation reaches the symbol: the client's dword at that
     spot minus the object's addend;
  2. else the symbol's bytes (relocations masked) found once in the client's .data / .rdata;
  3. else, for a plain `_Unknown_XXXXXX` name, the address in the name (not for names that embed a
     function address, such as `HORIZON_SCALE_*` or a `first@` static local).
Each symbol is sized from the next symbol of its section; the last one of a section ends at its
last non-zero byte (the rest is section padding). A relocated pointer compares by target: the string
text for a literal, the client address for a function or data. String literals, vtables, RTTI, EH data
and `___stl_prime_list` copies are left out.

    python datacmp.py [--objs build/app/obj] [--match FILE] [--json FILE] [--list-unplaced]

--match: an objmatch --json result (else objmatch runs here, ~72 s, on the app region with the
importer's settings). Exit 0 whether or not there are mismatches.
"""
import argparse
import collections
import glob
import json
import os
import re
import struct
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from objmatch import CoffObject, RELOC_SIZE, match_objects  # noqa: E402
from clientimage import ClientImage, DEFAULT_CLIENT  # noqa: E402

APP_REGION = (0x401000, 0x8C1B50)   # tools/vc6/importer.py APP_LIBS['app']['region']
EH_PREFIXES = ('__ehfuncinfo', '__unwind', '__catch', '__tryblock', '__ip2', '__cppx', '__ehhandler')
PLAIN_UNKNOWN = re.compile(r'^_Unknown_([0-9A-F]{6})$')
MIN_FIXED = 6   # fewer fixed bytes than this: too weak to search


def is_data_sec(info):
    n = info['name'].split('$')[0]
    return n in ('.data', '.rdata')


def category(name, secname):
    if name.startswith('??_C@'):
        return 'literal'
    if name.startswith('??_7'):
        return 'vtable'
    if name.startswith('??_R'):
        return 'rtti'
    if name.startswith(EH_PREFIXES) or secname.startswith('.xdata'):
        return 'eh'
    if name == '___stl_prime_list':
        return 'stl'
    return 'real' if name.startswith('__real@') else 'data'


def data_symbols(o):
    """[(symbol, section, lo, hi)] of initialised data symbols, each ending where the next begins."""
    bysec = collections.defaultdict(list)
    for s in o.symbols:
        if s['section'] > 0 and s['class'] in (2, 3):
            info = o.sections[s['section'] - 1]
            if s['class'] == 3 and s['name'] == info['name']:
                continue
            bysec[s['section'] - 1].append(s)
    out = []
    for sec, syms in bysec.items():
        info = o.sections[sec]
        if not is_data_sec(info) or info['flags'] & 0x80:   # uninitialised
            continue
        syms.sort(key=lambda s: s['value'])
        for k, s in enumerate(syms):
            if k + 1 < len(syms):
                hi = syms[k + 1]['value']
                # the compiler aligns the next symbol to 8 (a double, a struct holding one): when the
                # last dword before it is zero and not relocated, it is that padding (_Unknown_B7DB60)
                if hi % 8 == 0 and hi - s['value'] >= 8 and not any(o.data[info['raw'] + hi - 4:info['raw'] + hi]) \
                        and not any(hi - 4 <= va < hi for va, _, _ in info['relocs']):
                    hi -= 4
            else:   # last of the section: its end is not known, the padding after it is zero
                body = o.data[info['raw'] + s['value']:info['raw'] + info['size']]
                hi = s['value'] + len(body.rstrip(b'\0'))
                for va, _, typ in info['relocs']:   # a relocated field is part of it even when zero
                    if s['value'] <= va < info['size']:
                        hi = max(hi, va + RELOC_SIZE.get(typ, 4))
            out.append((s, sec, s['value'], hi))
    return out


class Audit:
    def __init__(self, img, objs, results):
        self.img, self.objs = img, objs
        self.fn_addr, self.fn_by_name = {}, collections.defaultdict(set)
        for r in results:
            if r['status'] == 'exact' and len(r['addresses']) == 1:
                a = int(r['addresses'][0], 16)
                self.fn_addr[(r['object'], r['symbol'])] = a
                self.fn_by_name[r['symbol']].add(a)
        self.votes = collections.defaultdict(collections.Counter)   # (obj, symbol) -> client addresses
        for f, o in objs.items():
            self.collect_votes(f, o)
        self.text_cache = {}

    def collect_votes(self, f, o):
        for name, sec, s, e in o.functions():
            base = self.fn_addr.get((f, name))
            if base is None:
                continue
            info = o.sections[sec]
            for va, symidx, typ in info['relocs']:
                if typ != 0x06 or not s <= va < e:
                    continue
                sym = o.symbol_at(symidx)
                if sym is None or sym['section'] <= 0 or not is_data_sec(o.sections[sym['section'] - 1]):
                    continue
                addend = struct.unpack_from('<I', o.data, info['raw'] + va)[0]
                try:
                    cv = self.img.u32(base + va - s)
                except Exception:
                    continue
                self.votes[(f, sym['name'])][(cv - addend) & 0xFFFFFFFF] += 1

    def target(self, f, o, sym):
        """('addr', client address) / ('str', literal bytes) of a relocation target, else None."""
        if sym is None:
            return None
        if sym['section'] <= 0:   # defined in another object
            c = self.fn_by_name.get(sym['name'])
            if c and len(c) == 1:
                return ('addr', next(iter(c)))
            for (ff, nm), cnt in self.votes.items():
                if nm == sym['name']:
                    return ('addr', cnt.most_common(1)[0][0])
            return None
        if sym['name'].startswith('??_C@'):
            lit = o.literal(sym)
            return ('str', lit) if lit else None
        a = self.fn_addr.get((f, sym['name']))
        if a:
            return ('addr', a)
        v = self.votes.get((f, sym['name']))
        return ('addr', v.most_common(1)[0][0]) if v else None

    def search(self, o, info, lo, hi):
        """The one client address whose bytes equal the symbol's with relocations masked, or None."""
        body = o.data[info['raw'] + lo:info['raw'] + hi]
        wild = [False] * len(body)
        for va, _, typ in info['relocs']:
            for k in range(va - lo, va - lo + RELOC_SIZE.get(typ, 4)):
                if 0 <= k < len(body):
                    wild[k] = True
        fixed = [b for b, w in zip(body, wild) if not w]
        if len(fixed) < MIN_FIXED or not any(fixed):
            return None
        pat = re.compile(b''.join(b'.' if w else re.escape(bytes([b])) for b, w in zip(body, wild)), re.S)
        hits = []
        for name in ('.data', '.rdata'):
            if name not in self.text_cache:
                va, vs, raw, rs = self.img.sections[name]
                self.text_cache[name] = (va, self.img.data[raw:raw + min(vs, rs)])
            va, blob = self.text_cache[name]
            hits += [va + m.start() for m in pat.finditer(blob)]
            if len(hits) > 1:
                return None
        return hits[0] if hits else None

    def compare(self, f, o, sec, lo, hi, ca):
        """(differences, masked pointer count) of the object's bytes [lo, hi) against the client at ca."""
        info = o.sections[sec]
        size = hi - lo
        off = self.img.offset(ca)
        if off is None:   # beyond the raw data (BSS): zero-filled in memory, nothing initialised there
            cl = bytes(size)
        else:
            cl = self.img.data[off:off + size]
        ours = bytes(o.data[info['raw'] + lo:info['raw'] + hi])
        skip, diffs, masked = set(), [], 0
        for va, symidx, typ in info['relocs']:
            if not lo <= va < hi:
                continue
            rel = va - lo
            skip.update(range(rel, rel + RELOC_SIZE.get(typ, 4)))
            if typ != 0x06 or rel + 4 > size:
                continue
            masked += 1
            t = self.target(f, o, o.symbol_at(symidx))
            if t is None:
                continue
            addend = struct.unpack_from('<I', ours, rel)[0]
            cv = struct.unpack_from('<I', cl, rel)[0]
            if t[0] == 'addr':
                if (t[1] + addend) & 0xFFFFFFFF != cv:
                    diffs.append((rel, 'ptr', '0x%X' % ((t[1] + addend) & 0xFFFFFFFF), '0x%X' % cv))
            else:
                want = t[1].rstrip(b'\0')
                got = self.img.cstr(cv)
                if got is None or got.encode('latin1') != want:
                    diffs.append((rel, 'str', want.decode('latin1'), got))
        k = 0
        while k < size:
            if k not in skip and ours[k] != cl[k]:
                n = 1
                while k + n < size and n < 8 and k + n not in skip and ours[k + n] != cl[k + n]:
                    n += 1
                diffs.append((k, 'bytes', ours[k:k + n].hex(' '), cl[k:k + n].hex(' ')))
                k += n
            else:
                k += 1
        diffs.sort(key=lambda d: d[0])
        return diffs, masked


def run(img, objs, results):
    au = Audit(img, objs, results)
    rows = []
    for f, o in objs.items():
        for s, sec, lo, hi in data_symbols(o):
            info = o.sections[sec]
            cat = category(s['name'], info['name'])
            if cat in ('literal', 'vtable', 'rtti', 'eh', 'stl') or hi <= lo:
                continue
            row = {'obj': f, 'sym': s['name'], 'size': hi - lo, 'cat': cat, 'client': None,
                   'how': None, 'status': 'unplaced', 'diffs': [], 'masked': 0}
            v = au.votes.get((f, s['name']))
            if v:
                row['client'], row['how'] = v.most_common(1)[0][0], 'reloc'
            else:
                a = au.search(o, info, lo, hi)
                if a is not None:
                    row['client'], row['how'] = a, 'bytes'
                else:
                    m = PLAIN_UNKNOWN.match(s['name'])
                    if m:
                        row['client'], row['how'] = int(m.group(1), 16), 'name'
            if row['client'] is not None:
                diffs, masked = au.compare(f, o, sec, lo, hi, row['client'])
                row.update(diffs=diffs, masked=masked, status='mismatch' if diffs else 'match')
            rows.append(row)
    return rows


def main():
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    ap.add_argument('--objs', default=os.path.join('build', 'app', 'obj'), help='folder of .obj files')
    ap.add_argument('--match', help='objmatch --json result for these objects (else objmatch runs here)')
    ap.add_argument('--json', help='write every row here')
    ap.add_argument('--list-unplaced', action='store_true', help='list the symbols no rule placed')
    ap.add_argument('--client', default=DEFAULT_CLIENT)
    a = ap.parse_args()
    paths = sorted(glob.glob(os.path.join(a.objs, '*.obj')))
    if not paths:
        sys.exit(f'no .obj files in {a.objs}')
    img = ClientImage(a.client)
    if a.match:
        results = json.load(open(a.match))
    else:
        print(f'running objmatch on {len(paths)} objects (~72 s) ...', file=sys.stderr)
        results = match_objects(img, paths, APP_REGION, callers=True)
    objs = {}
    for p in paths:
        try:
            objs[os.path.basename(p)] = CoffObject(p)
        except Exception as e:
            print('skip', p, e, file=sys.stderr)
    rows = run(img, objs, results)
    if a.json:
        json.dump(rows, open(a.json, 'w', newline='\n'), indent=1)
    st = collections.Counter(r['status'] for r in rows)
    masked = sum(r['masked'] for r in rows)
    print(f"objects {len(objs)}  symbols {len(rows)}  compared {st['match'] + st['mismatch']}  "
          f"mismatches {st['mismatch']}  not placed {st['unplaced']}  masked pointers {masked}")
    for r in rows:
        if r['status'] != 'mismatch':
            continue
        off, kind, ours, theirs = r['diffs'][0]
        print(f"{r['obj']}  {r['sym'][:60]}  0x{r['client']:X} ({r['how']})  +0x{off:X} {kind}: "
              f"ours {ours!r} client {theirs!r}  [{len(r['diffs'])} diffs]")
    if a.list_unplaced:
        for r in rows:
            if r['status'] == 'unplaced':
                print(f"unplaced  {r['obj']}  {r['sym'][:70]}  {r['cat']} {r['size']} bytes")
    return 0


if __name__ == '__main__':
    sys.exit(main())
