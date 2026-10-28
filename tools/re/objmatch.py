"""Finds the functions of compiled .obj files in client.exe, byte for byte.

Each function in an object is turned into a byte pattern in which every relocated field (calls,
jumps to other functions, addresses of data and strings) is a wildcard, because those are only
fixed at link time. The pattern is searched for in client.exe's .text. Results per function:

  exact      found once: this is the function, unchanged
  multiple   found more than once (short or common code; not proof on its own). `count` gives
             the number of copies (`addresses` lists the first 5), `identical` whether all
             copies are the same function, relocated fields included (REL32 targets compared)
  none       not found: the client's function differs, is inlined, or was not linked

A `multiple` result is broken by the relocations' targets (D80, `break_tie`): each candidate is
checked against what the object's relocations point at, the string literals (their text against
the client's string at the candidate's relocated field) and the symbols symbols.json knows (their
address against the field's target). A candidate with a contradiction drops out; if one is left
and at least one target confirmed it, the function is `exact` there (`tiebreak` is set).

What D80 leaves `multiple` gets a second tie-break (D88, the "self name"): of the candidates no
target contradicts, the one address symbols.json names with the function's own mangled symbol
from a proof other than an exact match (a vtable slot, a string; an exact entry counts by the
proof it keeps, `proof`, D114(c)) is credited, `exact` with
`tiebreak` and `selfname` set. It runs only once D80 resolves nothing more, so D80 always decides
first, and its credits feed D80's next round like any exact function.

What neither resolves gets the object's client range (D100, P-P6-1): of the survivors, the one
address inside the object's own range (first..last address of its exact functions that are its
alone: not COMDAT copies of inline, template or compiler-generated functions, which the linker
may place with another object) is credited, `exact` with `tiebreak` and `range` set. The range
also covers the object's exact COMDATs that no other object is exact at, within 0x1000 bytes of
either end of that own range (D114(b), P-M1-2). It runs only when D80 and the self name resolve
nothing more; its credits feed D80's next round.

    python objmatch.py [--client PATH] [--json OUT] file.obj...
"""
import argparse
import collections
import json
import os
import re
import struct
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from clientimage import ClientImage, DEFAULT_CLIENT  # noqa: E402

MIN_FIXED = 12   # a pattern needs at least this many non-wildcard bytes to be searched


def generated(name):
    """D94(b): a compiler-generated `_$E<n>` name (a dynamic initialiser or atexit destructor) is
    numbered per translation unit, so the same name is a different function in each object: never
    evidence across objects."""
    return name.startswith('_$E')

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
            if cls == 3 and naux and secnum > 0 and value == 0 and typ == 0 \
                    and name.decode('latin1') == self.sections[secnum - 1]['name']:
                # a section definition: its aux record holds the COMDAT selection (D100)
                self.sections[secnum - 1]['select'] = d[o + 18 + 14]
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

    def own(self, sec):
        """True if the code section holds a function of this translation unit alone (not a COMDAT,
        or a COMDAT with selection NODUPLICATES): not an inline, template or compiler-generated
        copy that the linker may take from another object."""
        info = self.sections[sec]
        return not info['flags'] & 0x1000 or info.get('select') == 1

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

    def relocated(self, sec, start, length):
        """[(offset in the function, size, type)] of the relocated fields of a function."""
        out = []
        for va, _, typ in self.sections[sec]['relocs']:
            if start <= va < start + length:
                out.append((va - start, RELOC_SIZE.get(typ, 4), typ))
        return out


def _reloc_targets(self, sec, start, length):
    """[(offset in the function, size, type, symbol dict, addend bytes)] of the relocated fields."""
    by_index = getattr(self, '_by_index', None)
    if by_index is None:
        by_index = self._by_index = {s['index']: s for s in self.symbols}
    info = self.sections[sec]
    out = []
    for va, symidx, typ in info['relocs']:
        if start <= va < start + length:
            n = RELOC_SIZE.get(typ, 4)
            out.append((va - start, n, typ, by_index.get(symidx), self.data[info['raw'] + va:info['raw'] + va + n]))
    return out


def _literal(self, sym):
    """The bytes (terminator included) of a string-literal symbol (`??_C@...`), or None."""
    if sym is None or sym['section'] <= 0 or not sym['name'].startswith('??_C@'):
        return None
    info = self.sections[sym['section'] - 1]
    o = info['raw'] + sym['value']
    end = info['raw'] + info['size']
    step = 2 if sym['name'].startswith('??_C@_1') else 1
    while o + step <= end:
        o += step
        if self.data[o - step:o] == b'\0' * step:
            return self.data[info['raw'] + sym['value']:o]
    return None


def _symbol_class(self, name):
    for sym in self.symbols:
        if sym['name'] == name and sym['section'] > 0:
            return sym['class']
    return None


def _symbol_at(self, index):
    if not hasattr(self, '_by_index'):
        self._by_index = {s['index']: s for s in self.symbols}
    return self._by_index.get(index)


CoffObject.symbol_at = _symbol_at
CoffObject.symbol_class = _symbol_class
CoffObject.reloc_targets = _reloc_targets
CoffObject.literal = _literal

_symbol_addresses = None
_self_names = None
_thunk_cache = {}


def thunks_of(img):
    """{thunk VA: target VA} of the client's incremental-link stubs (calls and vtable slots reach
    most functions through them)."""
    if id(img) not in _thunk_cache:
        _thunk_cache[id(img)] = img.incremental_thunks()
    return _thunk_cache[id(img)]



def symbol_addresses():
    """{mangled name: {addresses}} of symbols.json (functions and vtables)."""
    global _symbol_addresses
    if _symbol_addresses is None:
        _symbol_addresses = {}
        path = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 'docs', 'data', 'symbols.json')
        try:
            for e in json.load(open(path)):
                if not generated(e['symbol']):
                    _symbol_addresses.setdefault(e['symbol'], set()).add(int(e['address'], 16))
        except (OSError, ValueError, KeyError):
            pass
    return _symbol_addresses


def self_names():
    """D88: {mangled name: {addresses}} of the functions symbols.json names from a proof other than
    an exact match (a vtable slot, a string): the self names that break a `multiple`. An exact entry
    counts when it keeps such a proof (`proof`, D114(c))."""
    global _self_names
    if _self_names is None:
        _self_names = {}
        path = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 'docs', 'data', 'symbols.json')
        try:
            for e in json.load(open(path)):
                if e.get('kind') == 'function' and (e.get('source') != 'exact' or e.get('proof')) \
                        and not generated(e['symbol']):
                    _self_names.setdefault(e['symbol'], set()).add(int(e['address'], 16))
        except (OSError, ValueError, KeyError):
            pass
    return _self_names


def vtable_check(obj, sym, raw, known, img):
    """For a candidate holding `raw` as the address of the object's vtable `sym` (`??_7...`): True
    if every slot whose function has a known address holds it and at least one does, False if one
    differs, None if no slot is known."""
    info = obj.sections[sym['section'] - 1]
    seen = False
    for va, symidx, typ in info['relocs']:
        if typ != 0x06 or va < sym['value']:
            continue
        slot = obj.symbol_at(symidx)
        if slot is None or slot['class'] != 2 or slot['name'] not in known:
            continue
        o = img.offset(raw + va - sym['value'])
        if o is None:
            return False
        have = thunks_of(img).get(struct.unpack_from('<I', img.data, o)[0], struct.unpack_from('<I', img.data, o)[0])
        if have not in known[slot['name']]:
            return False
        seen = True
    return True if seen else None


def break_tie(obj, sec, start, length, text, base, hits, img, known=None):
    """D80: of the candidate addresses `hits` that match one masked pattern, those the object's
    relocation targets do not contradict. Returns (survivors, confirmed) with `confirmed` the
    survivors that at least one target confirmed: a string literal whose text is the client's
    string at the field, or a symbol symbols.json knows whose address is the field's target."""
    def norm(b):
        # a __FILE__ keeps the file's name only (each E&B project has its own copy of a header's)
        if b'\\' in b or b'/' in b:
            return re.split(rb'[\\/]', b)[-1].lower()
        return b
    known = symbol_addresses() if known is None else known
    fields = obj.reloc_targets(sec, start, length)
    survivors, confirmed = [], []
    for h in hits:
        o = h - base
        ok, sure = True, False
        for k, n, typ, sym, addend in fields:
            if n != 4 or sym is None or o + k + 4 > len(text):
                continue
            raw = struct.unpack('<I', text[o + k:o + k + 4])[0]
            lit = obj.literal(sym)
            if lit is not None and typ == 0x06:
                add = struct.unpack('<I', addend)[0]
                if add >= len(lit):
                    continue
                want = lit[add:]
                co = img.offset(raw)
                if co is None:
                    ok = False
                    break
                have = img.data[co:co + len(want)]
                # the client's whole string: a header's __FILE__ there can be shorter than ours
                # ('..\shared/auxdata/auxdata.h' against 'e:\build\...\client\..\shared\auxdata\auxdata.h')
                end = img.data.find(b'\0', co)
                whole = img.data[co:end] if end >= 0 else have
                if have == want or norm(whole) == norm(want.rstrip(b'\0')) and want[-1:] == b'\0':
                    sure = True
                else:
                    ok = False
                    break
            elif (typ == 0x06 and sym['section'] > 0 and sym['name'].startswith('??_7') and sym['name'] not in known
                  and struct.unpack('<I', addend)[0] == 0):
                v = vtable_check(obj, sym, raw, known, img)
                if v is False:
                    ok = False
                    break
                sure = sure or v is True
            elif sym['class'] == 2 and sym['name'] in known and typ in (0x06, 0x14):
                target = raw if typ == 0x06 else h + k + 4 + struct.unpack('<i', text[o + k:o + k + 4])[0]
                target -= struct.unpack('<I', addend)[0] if typ == 0x06 else 0
                if thunks_of(img).get(target, target) in known[sym['name']]:
                    sure = True
                else:
                    ok = False
                    break
        if ok:
            survivors.append(h)
            if sure:
                confirmed.append(h)
    return survivors, confirmed


def same_copies(text, base, hits, relocs, img=None):
    """True if every copy found at `hits` (VAs; text[0] is at VA `base`) is the same function,
    the masked fields included: a relative call/jump (REL32) must reach the same target, any
    other relocated field must hold the same value. Two functions that differ only in their
    callees (or vtables) match one masked pattern but are not identical copies. One exception
    (with `img`): an absolute reference to a C string compares the string, minus any directory
    part, because each E&B project has its own copy of a header's `__FILE__` (0x4A6DF0's is
    `e:\\build\\beta\\G\\app\\shared\\auxdata\\delta_template.h`, 0x7013F0's
    `..\\shared/auxdata/delta_template.h`)."""
    def cstring(va):
        o = img.offset(va)
        if o is None:
            return None
        end = img.data.find(b'\0', o, o + 512)
        s = img.data[o:end] if end > o else b''
        if not s or any(c < 0x20 or c > 0x7E for c in s):
            return None
        return ('str', re.split(rb'[\\/]', s)[-1].lower())

    def fields(h):
        o = h - base
        out = []
        for k, n, typ in relocs:
            raw = text[o + k:o + k + n]
            if typ == 0x14 and n == 4:
                out.append(h + k + 4 + struct.unpack('<i', raw)[0])
            elif typ == 0x06 and n == 4 and img is not None:
                va = struct.unpack('<I', raw)[0]
                out.append(cstring(va) or va)
            else:
                out.append(bytes(raw))
        return out
    first = fields(hits[0])
    return all(fields(h) == first for h in hits[1:])


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
    results, pending = [], []
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
                hits = [start + m.start() for m in re.finditer(pat, text, re.S)]
                r['addresses'] = [f'0x{a:06X}' for a in hits[:5]]
                r['status'] = 'exact' if len(hits) == 1 else 'multiple' if hits else 'none'
                if len(hits) > 1:
                    # every copy, and whether they are all the same function (D54)
                    r['count'] = len(hits)
                    r['identical'] = same_copies(text, start, hits, obj.relocated(sec, s, length), img)
                    pending.append((r, obj, sec, s, length, hits))
            results.append((r, obj, sec, s, length))
    # D80: what the exact functions show about the link (an external symbol's address) helps to break
    # the ties of the others; repeat until nothing more resolves (a destructor found once settles the
    # scalar deleting destructors that call it).
    thunk = thunks_of(img)
    known = {k: {thunk.get(a, a) for a in v} for k, v in symbol_addresses().items()}
    done = set()

    # one client function is one source function (VC6 does not fold identical code): an address two
    # different symbols would take, or that a symbol of another name already holds, is left alone
    claimed = collections.defaultdict(set)
    sole = collections.defaultdict(set)  # symbol -> the addresses it is exact at with no other symbol there
    for r, obj, sec, s, length in results:
        if r['status'] == 'exact':
            claimed[int(r['addresses'][0], 16)].add(r['symbol'])
    for r, obj, sec, s, length in results:
        if r['status'] == 'exact' and claimed[int(r['addresses'][0], 16)] == {r['symbol']}:
            sole[r['symbol']].add(int(r['addresses'][0], 16))

    def learn(r, obj, sec, s, length, addr):
        # A COMDAT copy of an inline function can be byte-identical to another function of its
        # object (dbgconn.obj's inline ~ForwardingConnectionClass and ~StatsConnectionClass 0x8251A0,
        # P1): when the symbol also matches an address of its own elsewhere, the shared one is the
        # other function's and must not become a "known" target that confirms the wrong twin.
        shared = claimed[addr] - {r['symbol']}
        if r['symbol'] and not generated(r['symbol']) and obj.symbol_class(r['symbol']) == 2 \
                and not (shared and sole[r['symbol']] - {addr}):
            known.setdefault(r['symbol'], set()).add(addr)
        o = addr - start
        for k, n, typ, sym, addend in obj.reloc_targets(sec, s, length):
            if n != 4 or sym is None or sym['class'] != 2 or obj.literal(sym) is not None or generated(sym['name']):
                continue
            if typ == 0x06:
                v = struct.unpack('<I', text[o + k:o + k + 4])[0] - struct.unpack('<I', addend)[0]
            elif typ == 0x14:
                v = addr + k + 4 + struct.unpack('<i', text[o + k:o + k + 4])[0]
            else:
                continue
            known.setdefault(sym['name'], set()).add(thunk.get(v, v))
    for r, obj, sec, s, length in results:
        if r['status'] == 'exact':
            learn(r, obj, sec, s, length, int(r['addresses'][0], 16))
    # D88: when D80 resolves nothing more, the self name breaks a tie: the one survivor that
    # symbols.json names with the function's own symbol (from a vtable slot or a string, not an
    # exact match); its credits may let D80 resolve more, so the loop goes on
    # D100: when the self name resolves nothing more either, the range breaks a tie: the one survivor
    # inside the object's client range (first..last address of its own exact functions, not COMDAT
    # copies the linker may take from another object); then back to D80
    phase = 'd80'
    changed = True
    while changed:
        changed = False
        proposals = []
        selfname = phase == 'selfname'
        ranges = {}
        if phase == 'range':
            for r, obj, sec, s, length in results:
                if r['status'] == 'exact' and obj.own(sec):
                    a = int(r['addresses'][0], 16)
                    lo, hi = ranges.get(id(obj), (a, a))
                    ranges[id(obj)] = (min(lo, a), max(hi, a))
            # D114(b): an exact COMDAT that only this object is exact at, within RANGE_GAP of either
            # end of the object's own range, widens it (the linker placed that copy with this object)
            holders = collections.defaultdict(set)
            for r, obj, sec, s, length in results:
                if r['status'] == 'exact':
                    holders[int(r['addresses'][0], 16)].add(id(obj))
            wide = dict(ranges)
            for r, obj, sec, s, length in results:
                if r['status'] == 'exact' and not obj.own(sec) and id(obj) in ranges:
                    a = int(r['addresses'][0], 16)
                    lo, hi = ranges[id(obj)]
                    if holders[a] == {id(obj)} and lo - RANGE_GAP <= a <= hi + RANGE_GAP:
                        wlo, whi = wide[id(obj)]
                        wide[id(obj)] = (min(wlo, a), max(whi, a))
            ranges = wide
        for item in pending:
            r, obj, sec, s, length, hits = item
            if id(r) in done:
                continue
            survivors, confirmed = break_tie(obj, sec, s, length, text, start, hits, img, known)
            if phase == 'd80':
                if len(survivors) == 1 and confirmed:
                    proposals.append((item, survivors[0]))
            elif phase == 'selfname':
                own = [h for h in survivors if h in self_names().get(r['symbol'], ())]
                if len(own) == 1:
                    proposals.append((item, own[0]))
            elif id(obj) in ranges:
                lo, hi = ranges[id(obj)]
                # an address another function is already exact at is not this one's (one client function
                # is one source function)
                inside = [h for h in survivors if lo <= h <= hi and not claimed[h] - {r['symbol']}]
                if len(inside) == 1:
                    proposals.append((item, inside[0]))
        by_addr = collections.defaultdict(set)
        for (r, *_), a in proposals:
            by_addr[a].add(r['symbol'])
        for item, a in proposals:
            r, obj, sec, s, length, hits = item
            if len(by_addr[a]) > 1 or claimed[a] - {r['symbol']}:
                continue
            r['status'] = 'exact'
            r['addresses'] = [f'0x{a:06X}']
            r['tiebreak'] = True
            if selfname:
                r['selfname'] = True
            if phase == 'range':
                r['range'] = True
            r.pop('count'), r.pop('identical')
            claimed[a].add(r['symbol'])
            learn(r, obj, sec, s, length, a)
            done.add(id(r))
            changed = True
        if phase == 'd80':
            if not changed:
                phase, changed = 'selfname', True   # D80 is done: one self-name round
        elif changed:
            phase = 'd80'                           # back to D80 after a round that credited
        elif phase == 'selfname':
            phase, changed = 'range', True          # neither D80 nor the self name: one range round
    return [x[0] for x in results]


RANGE_GAP = 0x1000   # D114(b): how far beyond its own range an object's lone exact COMDAT may lie


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
