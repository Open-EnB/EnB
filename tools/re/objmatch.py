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
either end of that own range (D114(b), P-M1-2; D196, 2026-11-05: at the upper end the widening is
chained through the COMDATs only this object is exact at, each measured from the range the previous
ones widened, so a run of template copies longer than 0x1000 after a file's last own function reaches
the file's static initialisers, ui_ship_cust's 0x620CA0; the lower end and D183's shared COMDATs are
measured from the own range only: chaining them let ui_shipcust's range run down over ui_ship_cust's
copies of a map type the two files shared), and an exact COMDAT that two or more objects are
exact at, at one address under one symbol (D183, P-IS-1), for the holder whose own range is
strictly nearest to it (inside it counts as 0; a tie widens none). Measured 2026-11-01: crediting
it to every holder lets a later object's range reach over the first object's code (inpkeys' range
over inpdsp's basic_string copies at 0x42A820) and two objects' tied functions then propose one
address (inpdsp's 0x42A900, 0x42AAE0 lost). It runs only when D80 and the self name resolve
nothing more; its credits feed D80's next round. A survivor the object's own exact functions
contradict is not inside (P-CGP-1): when their relocated calls to the symbol reach other addresses
only, the symbol is there, not at the survivor (two same-bytes COMDATs of one object would otherwise
both propose one address and neither be credited). In the range round (not D115's) the range is also
widened over the spots where `initialiser_targets` locates the object's own `_$E` static initialisers and
their atexit functions (D209, P-IR3-1 as narrowed by NM1, V7d; D80 amended 2026-11-08), so a tie lying
among them is inside, with three guards, because a header static's initialiser has hundreds of
byte-identical copies (533 of the guard-0xC416FC one) and the one found in a window is often the
neighbour's: (a) a spot two objects locate widens neither; (b) no widening that brings a second copy of
one of the object's still-tied `_$E` functions into the range; (c) in the app region only, a spot below
the range must start a zero-separated run of the client's CRT initialiser table (.CRT$XCU, merged into
.data: the longest run of zero or .text dwords, `xcu_previous`), which lists each object's initialisers
in link order. USE_XCU switches (c). When several symbols propose one address in the range round,
the one whose own evidence there is strictly the strongest is credited and the others stay tied
(P-CW1-1, 2026-11-08; `range_evidence`, compared as a tuple). A proposer is refuted when one of its
calls goes to a function defined in its own object whose bytes do not fullmatch where that call lands
(a template callback's ??_G calling its own inline destructor, against the class's ??_G, ui_chat_panel's
0x562410); if one proposer is left, it is credited. Of several left, the strongest: first whether the
client names the address with the symbol itself (D88 / D171's self names, or an exact function's call
or reference to the symbol reaches it), then how many of its calls go to functions of the object's own
code (not COMDATs) and land inside the object's range there; a tie at the top, or no evidence, still
drops them all. USE_RANGE_EVIDENCE switches it.

Such a COMDAT is `shared` (D183's guard): set on each holder's exact result; importer.py lists it in
the file's `shared` list. A shared result is not placement evidence: a D166 caller tie-break that
needed one (as our caller, or as the function a client call site sits in) is labelled
`shared_callers` (importer.py's `shared_callers` list). Skipping them instead lost five tie-break
pairs (0x4668F0, 0x5199F0 x2, 0x62A5D0 x2), so they are labelled, not skipped.

The self name also comes from the run itself (D171, P-AR1-1): a vtable whose address the run has
learned (an exact function's relocation to `??_7X@@6B@`, e.g. a constructor's vptr store) names its
slots' functions, slot k of our table being the function at the client's table + 4k (through its
thunk); and a short function (too few fixed bytes to search) at an address the run knows (a call's
target) whose pattern holds there teaches its relocations' targets the same way (`??1` of a template
accessor: `mov [ecx], ??_7KVAccessorClass<T>; ret`, whose table then names `??_G`).

What the range leaves gets the static initialiser (D115, P-R2-1 narrowed): a tied constructor or
destructor (`??0` / `??1`) is credited to the one survivor that the object's rebuilt `_$E` static
initialiser calls, or registers through its atexit function (`initialiser_targets`), `exact` with
`tiebreak` and `initialiser` set. Finds and other helpers stay tied. It runs only when the range
resolves nothing more; its credits feed D80's next round.

What the static initialiser leaves gets the CRT table (D242, 2026-11-12; it feeds D80's next round) (P-C1-2, amends D94(b); `xcu_position`, USE_XCU_POSITION): the client's initialiser table 0xB6C000-0xB6D700
lists every object's `.CRT$XCU` initialisers in link order, so an object's n-th initialiser is the n-th of the
consecutive table entries in its stretch (each initialiser's window: between its nearest exact own functions in
object order, within the range +- RANGE_GAP, short of the other objects' exact own functions; as many entries as
initialisers, each fullmatching its pattern, none held by another symbol, an exact initialiser at its own entry);
the `_$E` functions those initialisers' bytes reach (the wrapper called, the atexit stub pushed) follow, `exact`
with `xcu` set to 'table' or 'linked'. The `_$E` names stay unused across objects: the position is the proof.

A searched match counts only at a function start (D189(2)): an address the client's thunk table or a
call / jump reaches, or one right after padding (int3 / nop) or after the end of another function (a
`ret`, `ret n`, `jmp`, an indirect `jmp`, or a jump table's last entry), or a 16-byte aligned one (a
function after a switch's byte index table, Hue_Shift 0x94EB20). A hit inside a function (XMP's
Write_Marker without its 0x66 operand-size prefix, 0x9E4F11) is dropped before anything else sees it;
`unanchored` counts the dropped hits. ANCHOR_HITS switches the rule.

A `short` function (fewer than MIN_FIXED fixed bytes: too little to search) is credited only at an
address the run knows independently of its bytes (D188): a slot of a vtable whose address is known
(symbols.json's vtables, or one D171 learned, `vtable`; one an exact function's vptr store gave,
`vptr`), or the target of a call (REL32) in an exact function's relocations (`call`). Its pattern
must fullmatch there, no relocation target may contradict it (D80's checks), the proofs must name one
address, and no other symbol may hold it. Never by pattern alone. One exception (D193): COMDAT
bodies with the same bytes and the same relocations (a linker-folded inline, such as RefCountPtr's
Peek / operator-> at 0x551D20) that two symbols prove at one address are all credited there, not
dropped. Such a result is `exact` with
`short_known` set to the kind of proof; one in a COMDAT section (an inline, template or
compiler-generated function) is also `shared` (D183's guard: it credits the address, not a file
placement). The round runs when the static initialiser resolves nothing more; its credits feed D80's
next round like any exact function.

    python objmatch.py [--client PATH] [--json OUT] file.obj...
"""
import argparse
import bisect
import collections
import json
import os
import re
import struct
import sys
import threading

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from clientimage import ClientImage, DEFAULT_CLIENT  # noqa: E402

MIN_FIXED = 12   # a pattern needs at least this many non-wildcard bytes to be searched
# D189(2): searched hits count only at a function start (function_start). The user accepted its four
# corrections (0x9CEEF7, 0x9F367D, 0x924875, 0x8437A4: mid-function hits, 2026-11-03)
ANCHOR_HITS = True


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
        # a vtable's slot 0 is the class's deleting destructor: symbols.py names it `??_E` (vector),
        # VC6 emits `??_G` (scalar) for a class never new[]'d. Either name is the same slot's proof.
        for k, v in list(_self_names.items()):
            if k.startswith(('??_E', '??_G')):
                _self_names.setdefault(('??_G' if k[3] == 'E' else '??_E') + k[4:], set()).update(v)
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


def vtable_slots(obj):
    """D171: [(vtable symbol, [(offset, slot symbol)])] of the vtables an object defines."""
    if not hasattr(obj, '_vtable_slots'):
        bysec = collections.defaultdict(list)
        for s in obj.symbols:
            if s['section'] > 0:
                bysec[s['section']].append(s['value'])
        out = []
        for sym in obj.symbols:
            if sym['section'] <= 0 or not sym['name'].startswith('??_7'):
                continue
            nxt = min([v for v in bysec[sym['section']] if v > sym['value']] or [1 << 30])
            sl = []
            for va, symidx, typ in obj.sections[sym['section'] - 1]['relocs']:
                if typ == 0x06 and sym['value'] <= va < nxt:
                    slot = obj.symbol_at(symidx)
                    if slot is not None:
                        sl.append((va - sym['value'], slot['name']))
            out.append((sym['name'], sl))
        obj._vtable_slots = out
    return obj._vtable_slots


def learn_short(img, objs, known, thunk):
    """D171: the relocation targets of short functions at addresses the run knows, where their
    pattern holds."""
    for obj in objs:
        if not hasattr(obj, '_shorts'):
            obj._shorts = []
            for name, sec, s, e in obj.functions():
                pat, fixed, length = obj.pattern(sec, s, e)
                if fixed < MIN_FIXED and length >= 5:
                    obj._shorts.append((name, sec, s, length, re.compile(pat, re.S)))
        for name, sec, s, length, pat in obj._shorts:
            addrs = known.get(name)
            if not addrs or len(addrs) != 1:
                continue
            a = next(iter(addrs))
            o = img.offset(a)
            if o is None or not pat.fullmatch(img.data[o:o + length]):
                continue
            for k, n, typ, sym, addend in obj.reloc_targets(sec, s, length):
                if n != 4 or sym is None or sym['class'] != 2 or obj.literal(sym) is not None or generated(sym['name']):
                    continue
                if typ == 0x06:
                    v = struct.unpack_from('<I', img.data, o + k)[0] - struct.unpack('<I', addend)[0]
                elif typ == 0x14:
                    v = a + k + 4 + struct.unpack_from('<i', img.data, o + k)[0]
                else:
                    continue
                known.setdefault(sym['name'], set()).add(thunk.get(v, v))


def run_self_names(img, objs, known, thunk):
    """D171: self_names() plus the slots of the vtables the run has learned (one address each)."""
    learn_short(img, objs, known, thunk)
    out = {k: set(v) for k, v in self_names().items()}

    def norm(x):
        return '??_G' + x[4:] if x.startswith('??_E') else x
    # an address symbols.json already names (a vtable slot, a string) keeps that name: a learned slot
    # naming another function there adds nothing (WSClassicEnvironmentMapperClass's table points at
    # WSEnvMapperClass's 0x9306E0)
    owners = collections.defaultdict(set)
    for k, v in out.items():
        for a in v:
            owners[a].add(norm(k))
    for obj in objs:
        for name, sl in vtable_slots(obj):
            addrs = known.get(name)
            if not addrs or len(addrs) != 1:
                continue
            vt = next(iter(addrs))
            for off, slot in sl:
                o = img.offset(vt + off)
                if o is None or generated(slot):
                    continue
                v = struct.unpack_from('<I', img.data, o)[0]
                v = thunk.get(v, v)
                if owners.get(v) and norm(slot) not in owners[v]:
                    continue
                if len(symbol_addresses().get(norm(slot), ())) > 1 or len(symbol_addresses().get(slot, ())) > 1:
                    continue    # symbols.json has the symbol at several addresses: which one is this table's?
                out.setdefault(slot, set()).add(v)
                if slot.startswith(('??_E', '??_G')):
                    out.setdefault(('??_G' if slot[3] == 'E' else '??_E') + slot[4:], set()).add(v)
    # a self name is one address: a symbol named at several (two classes of one name, aux_roots' and
    # delta.cpp's DeltaCompoundClass; KVAccessorClass<unsigned> tables) names none of them
    return {k: v for k, v in out.items() if len(v) == 1}


def match_objects(img, paths, region=None, callers=False):
    """region=(lo, hi): search only [lo, hi) of .text (E&B's app region for app files: a pattern
    that also occurs in the libraries is still exact there). callers=True: D166's caller tie-break
    (caller_tiebreak) for identical copies the other rules leave tied; such a result is 'exact'
    with `caller_tiebreak` set."""
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
                anchored = [h for h in hits if not ANCHOR_HITS or function_start(img, h)]     # D189(2)
                if len(anchored) < len(hits):
                    r['unanchored'] = len(hits) - len(anchored)
                hits = anchored
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
                if sym['name'].startswith('??_7'):
                    vptrs[sym['name']].add(v)            # D188: a vptr store (constructor / destructor)
            elif typ == 0x14:
                v = addr + k + 4 + struct.unpack('<i', text[o + k:o + k + 4])[0]
                called[sym['name']].add(thunk.get(v, v) & 0xFFFFFFFF)   # D188: a call's target
                obj_called[(id(obj), sym['name'])].add(thunk.get(v, v) & 0xFFFFFFFF)   # P-CGP-1
            else:
                continue
            known.setdefault(sym['name'], set()).add(thunk.get(v, v))
    called = collections.defaultdict(set)   # D188: {symbol: {addresses}} the exact functions call
    vptrs = collections.defaultdict(set)    # D188: {vtable symbol: {addresses}} their vptr stores give
    obj_called = collections.defaultdict(set)   # P-CGP-1: {(object, symbol): {addresses}} its exact functions call
    for r, obj, sec, s, length in results:
        if r['status'] == 'exact':
            learn(r, obj, sec, s, length, int(r['addresses'][0], 16))
    shorts = [x for x in results if x[0]['status'] == 'short' and x[4] > 0]
    folded_at = collections.defaultdict(lambda: collections.defaultdict(set))  # D193: {address: {COMDAT body: {symbols}}}
    # D88: when D80 resolves nothing more, the self name breaks a tie: the one survivor that
    # symbols.json names with the function's own symbol (from a vtable slot or a string, not an
    # exact match); its credits may let D80 resolve more, so the loop goes on
    # D100: when the self name resolves nothing more either, the range breaks a tie: the one survivor
    # inside the object's client range (first..last address of its own exact functions, not COMDAT
    # copies the linker may take from another object); then back to D80
    # D174 (P-TK2-1(b)): the caller tie-break's credits feed D80, the self name and the range like any
    # exact function, and what those credit may let the caller tie-break resolve more: repeat both
    while True:
        phase = 'd80'
        changed = True
        while changed:
            changed = False
            proposals = []
            selfname = phase == 'selfname'
            if selfname:
                names = run_self_names(img, list({id(x[1]): x[1] for x in results}.values()), known, thunk)
            ranges = {}
            if phase in ('range', 'init', 'xcu'):
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
                grown = True
                while grown:    # chained: a COMDAT within RANGE_GAP of the widened range widens it too
                    grown = False
                    for r, obj, sec, s, length in results:
                        if r['status'] == 'exact' and not obj.own(sec) and id(obj) in ranges:
                            a = int(r['addresses'][0], 16)
                            wlo, whi = wide[id(obj)]
                            if wlo <= a <= whi:
                                continue
                            # D196: only a COMDAT this object alone is exact at chains, and only upwards (an
                            # object's copies and initialisers follow its own code); D183: one held by several
                            # objects, all under this symbol, when this holder's own range is strictly the
                            # nearest of the holders', and anything below, are measured from the own range
                            # (chained, ui_shipcust's range ran down over ui_ship_cust's copies of their
                            # shared map type on 217fd903: 0x628E40 lost)
                            lo, hi = ranges[id(obj)][0], (whi if holders[a] == {id(obj)} else ranges[id(obj)][1])
                            if (holders[a] == {id(obj)} or claimed[a] == {r['symbol']} and all(
                                    range_distance(ranges[id(obj)], a) < range_distance(ranges[o], a)
                                    for o in holders[a] if o != id(obj) and o in ranges)) and \
                                    lo - RANGE_GAP <= a <= hi + RANGE_GAP:
                                wide[id(obj)] = (min(wlo, a), max(whi, a))
                                grown = True
                ranges = wide
            if phase == 'range':
                # D209 (IR3): the object's static initialisers (and their atexit functions) located by
                # initialiser_targets inside the range widen it, so a tie lying among them is inside
                located = collections.defaultdict(set)
                initialiser_targets(results, pending, ranges, text, start, known, thunk, located)
                # P-IR3-1 narrowed (NM1): (a) a spot two objects locate is a copy, evidence for neither; (b) a
                # widening that takes in a second copy of one of the object's tied initialisers over-reaches
                finders = collections.defaultdict(set)
                for key, spots in located.items():
                    for a in spots:
                        finders[a].add(key)
                tied = collections.defaultdict(list)
                for r, obj, sec, s, length, hits in pending:
                    if id(r) not in done and generated(r['symbol']):
                        tied[id(obj)].append(hits)
                xprev = xcu_previous(img) if USE_XCU and region else {}   # the table marks TU boundaries (zero slots) only in the app stretch
                for key, spots in located.items():
                    if any(len(finders[a]) > 1 for a in spots):
                        continue
                    lo, hi = ranges[key]
                    nlo, nhi = min(lo, min(spots)), max(hi, max(spots))
                    if any(sum(nlo <= h <= nhi for h in hits) >= 2 and
                           sum(nlo <= h <= nhi for h in hits) > sum(lo <= h <= hi for h in hits)
                           for hits in tied[key]):
                        continue
                    if any(a < lo and a in xprev and xprev[a] and xprev[a] not in spots for a in spots):
                        continue
                    ranges[key] = (nlo, nhi)
            registered = initialiser_targets(results, pending, ranges, text, start, known, thunk) if phase == 'init' else {}
            if phase == 'short':
                # D188: a short function at an address the run knows independently of its bytes
                got = short_known(img, shorts, text, start, known, thunk, called, vptrs, claimed, folded_at)
                for (r, obj, sec, s, length), a, kind in got:
                    if not obj.own(sec):
                        folded_at[a][body_sig(obj, sec, s, length)].add(r['symbol'])
                    r['status'] = 'exact'
                    r['addresses'] = [f'0x{a:06X}']
                    r['short_known'] = kind
                    claimed[a].add(r['symbol'])
                    learn(r, obj, sec, s, length, a)
                    changed = True
                shorts = [x for x in shorts if x[0]['status'] == 'short']
                phase = 'd80' if changed else 'done'
                continue
            if phase == 'xcu':
                # D242 P-C1-2: the `_$E` initialisers by their CRT table position, and what their bytes reach
                got = xcu_position(img, results, ranges, text, start, claimed, thunk) if USE_XCU_POSITION else []
                for (r, obj, sec, s, length), a, kind in got:
                    if r['status'] == 'multiple':
                        r['tiebreak'] = True
                        r.pop('count', None), r.pop('identical', None)
                        done.add(id(r))
                    r['status'] = 'exact'
                    r['addresses'] = [f'0x{a:06X}']
                    r['xcu'] = kind
                    claimed[a].add(r['symbol'])
                    learn(r, obj, sec, s, length, a)
                    changed = True
                shorts = [x for x in shorts if x[0]['status'] == 'short']
                phase = 'd80' if changed else 'short'
                changed = True
                continue
            for item in pending:
                r, obj, sec, s, length, hits = item
                if id(r) in done:
                    continue
                survivors, confirmed = break_tie(obj, sec, s, length, text, start, hits, img, known)
                if phase == 'd80':
                    if len(survivors) == 1 and confirmed:
                        proposals.append((item, survivors[0]))
                elif phase == 'selfname':
                    own = [h for h in survivors if h in names.get(r['symbol'], ())]
                    if len(own) == 1:
                        proposals.append((item, own[0]))
                elif phase == 'init':
                    # D115: a tied constructor / destructor is the one the object's static initialiser
                    # calls or registers; finds and other helpers stay tied
                    if r['symbol'].startswith(('??0', '??1')):
                        own = [h for h in survivors if h in registered.get(r['symbol'], ())]
                        if len(own) == 1:
                            proposals.append((item, own[0]))
                elif id(obj) in ranges:
                    lo, hi = ranges[id(obj)]
                    # an address another function is already exact at is not this one's (one client function
                    # is one source function)
                    inside = [h for h in survivors if lo <= h <= hi and not claimed[h] - {r['symbol']}]
                    # P-CGP-1: nor one the object's own exact callers contradict (their relocated calls to
                    # this symbol reach another address: ui_cpit's Create_Peek<FontSys> 0x546510, not the
                    # same-bytes Create_Peek<VertexMaterialClass> 0x584DF0)
                    reach = obj_called.get((id(obj), r['symbol']))
                    if reach:
                        inside = [h for h in inside if h in reach]
                    if len(inside) == 1:
                        proposals.append((item, inside[0]))
            by_addr = collections.defaultdict(set)
            for (r, *_), a in proposals:
                by_addr[a].add(r['symbol'])
            winner = {}
            if phase == 'range' and USE_RANGE_EVIDENCE:
                # P-CW1-1: several symbols proposing one address in the range round: the one whose own evidence
                # there is strictly the strongest is credited (range_evidence), the others stay tied
                best = collections.defaultdict(dict)
                for item, a in proposals:
                    if len(by_addr[a]) > 1:
                        r, obj, sec, s, length, hits = item
                        e = range_evidence(obj, sec, s, length, text, start, a, ranges.get(id(obj)), thunk,
                                           names.get(r['symbol'], ()), known.get(r['symbol'], ()))
                        best[a][r['symbol']] = min(best[a].get(r['symbol'], e), e)   # each copy of the symbol must hold
                for a, ev in best.items():
                    left = {k: e for k, e in ev.items() if e[0]}     # not refuted by a callee's bytes
                    if len(left) == 1:
                        winner[a] = next(iter(left))
                    elif left:
                        top = max(left.values())
                        if top > (True, False, 0) and sum(e == top for e in left.values()) == 1:
                            winner[a] = next(k for k, e in left.items() if e == top)
            for item, a in proposals:
                r, obj, sec, s, length, hits = item
                if len(by_addr[a]) > 1 and winner.get(a) != r['symbol'] or claimed[a] - {r['symbol']}:
                    continue
                r['status'] = 'exact'
                r['addresses'] = [f'0x{a:06X}']
                r['tiebreak'] = True
                if selfname:
                    r['selfname'] = True
                if phase == 'range':
                    r['range'] = True
                if phase == 'init':
                    r['initialiser'] = True
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
            elif phase == 'range':
                phase, changed = 'init', True           # nor the range: one static initialiser round (D115)
            elif phase == 'init':
                phase, changed = 'xcu', True            # nor the initialiser: the CRT table round (D242),
                                                        # then one short round (D188)
        if not callers:
            break
        credited = caller_tiebreak(img, results, pending, claimed, text, start, thunk, known)
        if not credited:
            break
        for item in credited:
            r, obj, sec, s, length, hits = item
            done.add(id(r))
            learn(r, obj, sec, s, length, int(r['addresses'][0], 16))
    for r, *_ in results:
        r.pop('shared', None)
        if r['status'] != 'exact' or not r.get('shared_callers'):
            r.pop('shared_callers', None)
    shared = shared_pairs(results)
    for r, obj, sec, *_ in results:
        if r['status'] == 'exact' and (int(r['addresses'][0], 16), r['symbol']) in shared:
            r['shared'] = True
        if r.get('short_known') and not obj.own(sec):
            r['shared'] = True      # D188: a short COMDAT credits the address, not a file placement
    return [x[0] for x in results]


def function_start(img, va):
    """D189(2): True if a searched hit at `va` may be a function's start: a thunk's or a call / jump's
    target, the start of .text, or the byte after padding (int3 / nop) or after the end of another
    function (`ret`, `ret n`, `jmp rel32` / `rel8`, an indirect `jmp`, or a jump table's last entry, a
    .text address), or a 16-byte aligned address (a function after a switch's byte index table,
    Hue_Shift 0x94EB20). Never an unaligned hit inside a function."""
    start, end, raw = img.section('.text')
    if va == start:
        return True
    sites, thunks = call_sites(img)
    if va in sites or va in _thunk_targets(img):
        return True
    o = img.offset(va)
    d = img.data
    if o is None or o < 7:
        return False
    if d[o - 1] in (0xCC, 0x90, 0xC3) or d[o - 3] == 0xC2 or d[o - 5] == 0xE9 or d[o - 2] == 0xEB:
        return True
    # an indirect jmp (FF /4): jmp reg, jmp [reg], jmp [reg+disp8], jmp [disp32] / [reg+disp32]
    if d[o - 2] == 0xFF and (0xE0 <= d[o - 1] <= 0xE7 or 0x20 <= d[o - 1] <= 0x27 and d[o - 1] not in (0x24, 0x25)):
        return True
    if d[o - 3] == 0xFF and 0x60 <= d[o - 2] <= 0x67 and d[o - 2] != 0x64:
        return True
    if d[o - 6] == 0xFF and (d[o - 5] == 0x25 or 0xA0 <= d[o - 5] <= 0xA7 and d[o - 5] != 0xA4):
        return True
    # after a jump table (its last entry a .text address), or 16-byte aligned (after a switch's index bytes)
    return start <= struct.unpack_from('<I', d, o - 4)[0] < end or va % 16 == 0


_targets = {}


def _thunk_targets(img):
    if id(img) not in _targets:
        _targets[id(img)] = set(thunks_of(img).values())
    return _targets[id(img)]


def body_sig(obj, sec, s, length):
    """A function's bytes and relocations (offset, size, type, symbol name, addend): equal for copies
    the linker may fold into one client function (D174, D193)."""
    info = obj.sections[sec]
    return (bytes(obj.data[info['raw'] + s:info['raw'] + s + length]),
            tuple((k, n, typ, sym and sym['name'], bytes(add)) for k, n, typ, sym, add in obj.reloc_targets(sec, s, length)))


def short_known(img, shorts, text, base, known, thunk, called, vptrs, claimed, folded_at=None):
    """D188: [(item, address, kind)] for the `short` functions at an address the run knows independently
    of their bytes: a call's target in an exact function (`call`), or a slot of a vtable whose address is
    known (`vptr` when an exact function's vptr store gave it, `vtable` otherwise: symbols.json, D171).
    The pattern must fullmatch there, D80's checks must leave it, the proofs must name one address, and
    neither another symbol already exact there nor another short function may take it.
    D193: except a linker-folded COMDAT: when every claimant of the address (this round's proposals and
    the symbols an earlier short round credited there, `folded_at` {address: {body_sig: {symbols}}}) is
    a COMDAT body with the same bytes and relocations, all are credited."""
    folded_at = folded_at if folded_at is not None else {}
    objs = list({id(x[1]): x[1] for x in shorts}.values())
    slot_of = collections.defaultdict(lambda: collections.defaultdict(set))   # symbol -> address -> kinds
    names = self_names()
    owners = collections.defaultdict(set)
    for k, v in names.items():
        for a in v:
            owners[a].add(k[4:] if k.startswith(('??_E', '??_G')) else k)
    for obj in objs:
        for vt, sl in vtable_slots(obj):
            addrs = known.get(vt)
            if not addrs or len(addrs) != 1:
                continue
            va = next(iter(addrs))
            kind = 'vptr' if va in vptrs.get(vt, ()) else 'vtable'
            for off, slot in sl:
                o = img.offset(va + off)
                if o is None or generated(slot):
                    continue
                v = struct.unpack_from('<I', img.data, o)[0]
                v = thunk.get(v, v)
                n = slot[4:] if slot.startswith(('??_E', '??_G')) else slot
                if owners.get(v) and n not in owners[v]:
                    continue        # symbols.json names another function there
                if known.get(slot) and v not in known[slot]:
                    continue        # the slot's symbol is known elsewhere: our table differs from the client's
                    # here (ChatPanelInterface's +0x64 is its own empty 0x563260, ours inherits 0x5156B0)
                slot_of[slot][v].add(kind)
    for name, addrs in called.items():
        for a in addrs:
            slot_of[name][a].add('call')
    props = []
    for item in shorts:
        r, obj, sec, s, length = item
        if obj.symbol_class(r['symbol']) != 2 or generated(r['symbol']):
            continue
        cands = slot_of.get(r['symbol'])
        if not cands or len(cands) != 1:
            continue
        a, kinds = next(iter(cands.items()))
        o = a - base
        if not 0 <= o <= len(text) - length:
            continue
        others = claimed[a] - {r['symbol']}
        if others:
            # D193: only symbols an earlier short round credited there as the same folded COMDAT body
            earlier = folded_at.get(a, {})
            if obj.own(sec) or len(earlier) != 1 or not others <= next(iter(earlier.values())) or \
                    body_sig(obj, sec, s, length) not in earlier:
                continue
        pat = obj.pattern(sec, s, s + length)[0]
        if not re.fullmatch(pat, text[o:o + length], re.S):
            continue
        survivors, _ = break_tie(obj, sec, s, length, text, base, [a], img, known)
        if survivors != [a]:
            continue
        kind = 'call' if 'call' in kinds else 'vptr' if 'vptr' in kinds else 'vtable'
        props.append((item, a, kind))
    by_addr = collections.defaultdict(set)
    bodies = collections.defaultdict(set)       # D193: the COMDAT bodies proposed at each address
    for (r, obj, sec, s, length), a, _ in props:
        by_addr[a].add(r['symbol'])
        bodies[a].add(body_sig(obj, sec, s, length) if not obj.own(sec) else ('own', id(r)))
    for a, v in folded_at.items():
        if a in bodies:
            bodies[a] |= set(v)
    return [p for p in props if len(by_addr[p[1]]) == 1 or len(bodies[p[1]]) == 1 and
            all(b[0] != 'own' for b in bodies[p[1]])]


def range_distance(rng, a):
    """How far address a lies outside the range (lo, hi); 0 inside it."""
    return 0 if rng[0] <= a <= rng[1] else min(abs(a - rng[0]), abs(a - rng[1]))


def shared_pairs(results):
    """D183: the (address, symbol) pairs that two or more objects are exact at."""
    by = collections.defaultdict(set)
    for r, obj, *_ in results:
        if r['status'] == 'exact':
            by[(int(r['addresses'][0], 16), r['symbol'])].add(id(obj))
    return {k for k, v in by.items() if len(v) > 1}


_call_sites = {}
_call_lock = threading.Lock()


def call_sites(img):
    """({target VA: [site VA]} of every E8 / E9 rel32 in .text, the incremental thunk table). A byte
    that only looks like E8 adds a site, which can only make D166 more careful."""
    with _call_lock:
        if id(img) not in _call_sites:
            start, end, raw = img.section('.text')
            body = img.data[raw:raw + (end - start)]
            sites = collections.defaultdict(list)
            for m in re.finditer(rb'[\xE8\xE9]', body):
                o = m.start()
                if o + 5 > len(body):
                    break
                dst = (start + o + 5 + struct.unpack_from('<i', body, o + 1)[0]) & 0xFFFFFFFF
                sites[dst].append(start + o)
            _call_sites[id(img)] = (sites, thunks_of(img))
        return _call_sites[id(img)]


_funclets = {}
FUNCLET_MAX = 0x40   # an unwind funclet is a few instructions (mov ecx, [ebp-x]; jmp dtor)


def funclet_parents(img):
    """D174 (P-TK3-2): ([funclet start VA], {funclet start: (end VA, {parent's push-site VA})}).
    A function with C++ unwinding pushes its handler stub (`push offset __ehhandler$F`; the stub is
    `mov eax, offset FuncInfo; jmp ___CxxFrameHandler`), and FuncInfo (magic 0x19930520, maxState,
    pUnwindMap, ...) lists the unwind funclets as {toState, action} entries. The funclets live apart
    from their parent (client.exe 0xAA8000-0xACD000); a funclet ends at the next funclet or stub, or
    FUNCLET_MAX bytes on. A funclet the linker shares between functions has every parent's push site."""
    with _call_lock:
        if id(img) in _funclets:
            return _funclets[id(img)]
        start, end, raw = img.section('.text')
        body = img.data[raw:raw + (end - start)]
        data = img.data
        stubs = {}
        for m in re.finditer(rb'\xB8(....)\xE9', body, re.S):
            fi = struct.unpack_from('<I', body, m.start() + 1)[0]
            off = img.offset(fi)
            if off is None or off + 12 > len(data) or struct.unpack_from('<I', data, off)[0] != 0x19930520:
                continue
            stubs[start + m.start()] = off
        pushes = collections.defaultdict(set)
        for m in re.finditer(rb'(?=\x68(....))', body, re.S):
            v = struct.unpack_from('<I', body, m.start() + 1)[0]
            if v in stubs:
                pushes[v].add(start + m.start())
        parents = collections.defaultdict(set)
        for stub, off in stubs.items():
            if not pushes[stub]:
                continue
            nstate, pmap = struct.unpack_from('<iI', data, off + 4)
            moff = img.offset(pmap)
            if moff is None or not 0 < nstate < 0x1000:
                continue
            for i in range(nstate):
                if moff + 8 * i + 8 > len(data):
                    break
                act = struct.unpack_from('<I', data, moff + 8 * i + 4)[0]
                if start <= act < end:
                    parents[act] |= pushes[stub]
        bounds = sorted(set(parents) | set(stubs))
        out = {}
        for f in parents:
            i = bisect.bisect_right(bounds, f)
            nxt = bounds[i] if i < len(bounds) else end
            out[f] = (min(nxt, f + FUNCLET_MAX), parents[f])
        _funclets[id(img)] = (sorted(out), out)
        return _funclets[id(img)]


def folded(items):
    """D174 (P-TK5-1): True if the tied items are functions of one object with the same bytes and the
    same relocations (symbols and addends): copies the linker may fold into one client function."""
    r0, obj0, sec0, s0, len0, _ = items[0]

    def sig(obj, sec, s, length):
        info = obj.sections[sec]
        return (bytes(obj.data[info['raw'] + s:info['raw'] + s + length]),
                [(k, n, typ, sym and sym['name'], bytes(add)) for k, n, typ, sym, add in obj.reloc_targets(sec, s, length)])
    first = sig(obj0, sec0, s0, len0)
    return all(obj is obj0 and length == len0 and sig(obj, sec, s, length) == first
               for r, obj, sec, s, length, _ in items[1:])


def caller_tiebreak(img, results, pending, claimed, text, base, thunk, known=None):
    """D166: a function found at several client addresses is the one our own callers reach. Our
    caller C (a function of the same object, exact at A) has a relocated call to X's symbol; the
    call at A reaches one of X's addresses. X takes that address only when every client call or
    jump to it comes from a function the same object is exact at (or from X itself) and nothing
    holds the address as data (a vtable slot, a pushed pointer): the linker's merged copy has callers
    in every file and stays tied. No other symbol may be exact there or propose the same address.
    Rounds repeat until nothing changes (an address credited makes its own callees resolvable); a
    round's proposals are applied together, so the result does not depend on the order of the
    objects. Returns the pending items it credited.
    D174: the copies need not be byte-identical (P-TK3-1: they may differ in a relocation's target,
    e.g. an EH handler's address; the callers decide); the candidates are D80's survivors (those no
    relocation target contradicts, P-TK2-1(a)); a call from an EH unwind funclet counts as a call by
    its parent function(s) (P-TK3-2, `funclet_parents`); and when several symbols of one object with
    the same bytes and relocations propose one address (the linker folded them: lookup_table's four
    Create_NEW<...> at 0x4EE9F0), the first by name takes it (P-TK5-1, `folded` set).
    D194: when every proposer is a COMDAT and all share one `body_sig` (any objects), all are credited."""
    todo = []
    for it in pending:
        r, obj, sec, s, length, hits = it
        if r['status'] != 'multiple':
            continue
        survivors, _ = break_tie(obj, sec, s, length, text, base, hits, img, known)
        todo.append((it, set(survivors)))
    if not todo:
        return []
    todo.sort(key=lambda t: (t[0][0]['object'], t[0][0]['symbol']))
    sites, thunks = call_sites(img)
    fstarts, funclets = funclet_parents(img)
    alias = collections.defaultdict(set)
    for th, dst in thunks.items():
        alias[dst].add(th)
    data = img.data

    def inside(site, order, funcs):
        i = bisect.bisect_right(order, site) - 1
        return i >= 0 and site < order[i] + funcs[order[i]]

    def refs_ok(addr, xlen, funcs):
        """funcs: {start: length} of the functions the object is exact at."""
        names = {addr} | alias.get(addr, set())
        if any(data.find(struct.pack('<I', a)) >= 0 for a in names):
            return False
        order = sorted(funcs)
        n = 0
        for a in names:
            for site in sites.get(a, ()):
                if site in thunks:
                    continue                      # the thunk's own jump
                n += 1
                if addr <= site < addr + xlen:
                    continue                      # X calling itself
                if inside(site, order, funcs):
                    continue
                j = bisect.bisect_right(fstarts, site) - 1
                if j >= 0 and site < funclets[fstarts[j]][0] and \
                        all(inside(p, order, funcs) for p in funclets[fstarts[j]][1]):
                    continue                      # an unwind funclet of the object's exact functions
                return False
        return n > 0

    credited = []
    changed = True
    while changed:
        changed = False
        proposals = []
        shared = shared_pairs(results)   # D183: another object holds it too: labelled when it decides
        for item, hits in todo:
            r, obj, sec, s, length, _ = item
            if r['status'] != 'multiple':
                continue
            funcs, callers, unshared = {}, [], {}
            for r2, obj2, sec2, s2, len2 in results:
                if obj2 is obj and r2['status'] == 'exact':
                    a2 = int(r2['addresses'][0], 16)
                    funcs[a2] = max(funcs.get(a2, 0), len2)
                    if (a2, r2['symbol']) not in shared:
                        unshared[a2] = max(unshared.get(a2, 0), len2)
                    if r2['symbol'] != r['symbol']:
                        callers.append((sec2, s2, len2, a2))
            cands, bad, via = set(), False, set()
            for sec2, s2, len2, a2 in callers:
                for k, n, typ, sym, addend in obj.reloc_targets(sec2, s2, len2):
                    if sym is None or sym['name'] != r['symbol']:
                        continue
                    if n != 4 or typ != 0x14:
                        bad = True
                        continue
                    o = a2 - base
                    v = (a2 + k + 4 + struct.unpack('<i', text[o + k:o + k + 4])[0]) & 0xFFFFFFFF
                    cands.add(thunk.get(v, v))
                    via.add(a2)
            if bad or len(cands) != 1:
                continue
            a = next(iter(cands))
            if a not in hits or claimed[a] - {r['symbol']} or not refs_ok(a, length, funcs):
                continue
            # D183: a decision that needed a shared function (every caller shared, or a call site that only
            # a shared function holds) is labelled
            r['shared_callers'] = not (via & set(unshared)) or not refs_ok(a, length, unshared)
            proposals.append((item, a))
        by_addr = collections.defaultdict(list)
        for item, a in proposals:
            by_addr[a].append(item)
        for a, items in by_addr.items():
            if len({it[0]['symbol'] for it in items}) > 1:
                # D194: COMDAT bodies with one body_sig (byte-identical, linker-folded, as D193's
                # short_known): every proposer is credited
                if all(not it[1].own(it[2]) for it in items) and \
                        len({body_sig(*it[1:5]) for it in items}) == 1:
                    for it in items:
                        it[0]['folded'] = True
                elif not folded(items):
                    continue
                else:
                    items = [min(items, key=lambda it: it[0]['symbol'])]
                    items[0][0]['folded'] = True
            for item in items:
                r = item[0]
                if not r.get('shared_callers'):
                    r.pop('shared_callers', None)
                r['status'] = 'exact'
                r['addresses'] = [f'0x{a:06X}']
                r['caller_tiebreak'] = True
                r.pop('count', None)
                r.pop('identical', None)
                claimed[a].add(r['symbol'])
                credited.append(item)
                changed = True
    return credited


def initialiser_targets(results, pending, ranges, text, base, known, thunk, located=None):
    """D115: {symbol: {addresses}} of the functions the objects' rebuilt static initialisers call or
    register. A `_$E` initialiser is usually too short to match (`short`), so it is located by its
    links instead: a pattern hit inside the object's range (widened by RANGE_GAP) where every relocated
    field to a symbol already known points at that symbol's address (atexit, a global), no field points
    at an address known under another name, at least one field is confirmed that way, and the `_$E`
    atexit destructor it pushes matches its own pattern at the pushed address under the same rules (the
    static's address equal in both). An initialiser with exactly one such hit (or an exact one) gives
    its and its destructor's other code targets: the constructor it calls, the destructor the atexit
    function jumps to. A target that is a copy of another of the object's functions and not of the
    symbol's own pattern contradicts the hit (two statics' initialisers of the same shape, inpkeys')."""
    owners = collections.defaultdict(set)
    for name, addrs in known.items():
        for a in addrs:
            owners[a].add(name)
    by_obj = collections.defaultdict(list)
    for item in results:
        if generated(item[0]['symbol']):
            by_obj[id(item[1])].append(item)
    at = collections.defaultdict(lambda: collections.defaultdict(set))   # obj -> address -> its copies' symbols
    for r, obj, sec, s, length in results:
        if r['status'] == 'exact':
            at[id(obj)][int(r['addresses'][0], 16)].add(r['symbol'])
    for r, obj, sec, s, length, hits in pending:
        for h in hits:
            at[id(obj)][h].add(r['symbol'])
    out = collections.defaultdict(set)
    for key, items in by_obj.items():
        if key not in ranges:
            continue
        defs = {r['symbol']: (r, obj, sec, s, length) for r, obj, sec, s, length in items}

        def check(fn, h):
            """None if the function can't be at h, else (confirmations, links, [(symbol, target)])."""
            r, obj, sec, s, length = fn
            o = h - base
            pat = obj.pattern(sec, s, s + length)[0]
            if o < 0 or not re.fullmatch(pat, text[o:o + length], re.S):
                return None
            conf, links, opens = 0, {}, []
            for k, n, typ, sym, addend in obj.reloc_targets(sec, s, length):
                if n != 4 or sym is None or typ not in (0x06, 0x14):
                    continue
                v = struct.unpack('<I', text[o + k:o + k + 4])[0]
                v = (v - struct.unpack('<I', addend)[0]) & 0xFFFFFFFF if typ == 0x06 \
                    else (h + k + 4 + struct.unpack('<i', text[o + k:o + k + 4])[0]) & 0xFFFFFFFF
                t = thunk.get(v, v)
                name = sym['name']
                if generated(name):
                    if name in defs and name != r['symbol']:
                        links[name] = t
                    continue
                if name in known:
                    if t not in known[name]:
                        return None
                    conf += 1
                    # still the initialiser's target: symbols.json may name it from an earlier run's
                    # match that this very rule made (a regeneration must not take the evidence away)
                    opens.append((name, t))
                elif owners.get(t) or (at[key].get(t) and name not in at[key][t]):
                    return None
                else:
                    opens.append((name, t))
            return conf, links, opens

        for name, fn in defs.items():
            r, obj, sec, s, length = fn
            if r['status'] == 'exact':
                hits = [int(r['addresses'][0], 16)]
            else:
                lo, hi = ranges[key]
                a0 = max(lo - RANGE_GAP - base, 0)
                a1 = min(hi + RANGE_GAP - base + length, len(text))
                pat = obj.pattern(sec, s, s + length)[0]
                hits = [base + a0 + m.start() for m in re.finditer(b'(?=' + pat + b')', text[a0:a1], re.S)]
            good = []
            for h in hits:
                c = check(fn, h)
                if c is None or not (c[0] or r['status'] == 'exact'):
                    continue
                conf, links, opens = c
                ok = True
                spots = [h]
                for ln, la in links.items():
                    lc = check(defs[ln], la)
                    if lc is None:
                        ok = False
                        break
                    mine = dict(opens)
                    if any(mine.get(sym, t) != t for sym, t in lc[2]):
                        ok = False      # the static's address differs between the two
                        break
                    opens = opens + lc[2]
                    spots.append(la)
                if ok:
                    good.append((opens, spots))
            if len(good) == 1:
                for sym, t in good[0][0]:
                    out[sym].add(t)
                if located is not None:
                    located[key].update(good[0][1])
    return out


USE_XCU = True
_xcu = {}


def xcu_previous(img):
    """{initialiser address: the value of the slot before it} of the CRT initialiser table (.CRT$XCU, merged into
    .data: the longest run of dwords that are 0 or .text addresses)."""
    if 'prev' not in _xcu:
        ts, te, _ = img.section('.text')
        ds, de, raw = img.section('.data')
        vals = struct.unpack_from('<%dI' % (min(de - ds, len(img.data) - raw) // 4), img.data, raw)
        best, cur, nz, bnz = (0, 0), 0, 0, 0
        for i, v in enumerate(vals):
            if v == 0 or ts <= v < te:
                cur += 1
                nz += v != 0
                if nz > bnz:
                    best, bnz = (i - cur + 1, i + 1), nz
            else:
                cur, nz = 0, 0
        prev = {}
        for i in range(best[0] + 1, best[1]):
            if vals[i]:
                prev.setdefault(vals[i], vals[i - 1])
        _xcu['prev'] = prev
    return _xcu['prev']


USE_XCU_POSITION = True     # D242 P-C1-2: the `_$E` initialisers credited by their CRT table position
XCU_TABLE = (0xB6C000, 0xB6D700)


def xcu_table(img):
    """The non-zero entries of the client's CRT initialiser table (.CRT$XCU merged into .data, XCU_TABLE), in table
    order: the initialisers of every object, in link order."""
    if 'table' not in _xcu:
        out = []
        for va in range(XCU_TABLE[0], XCU_TABLE[1], 4):
            o = img.offset(va)
            v = struct.unpack_from('<I', img.data, o)[0] if o is not None else 0
            if v:
                out.append(v)
        _xcu['table'] = out
    return _xcu['table']


def xcu_initialisers(obj):
    """The symbols an object's .CRT$XCU sections list (its dynamic initialisers), in section and slot order."""
    out = []
    for info in obj.sections:
        if info['name'].startswith('.CRT$XC') and info['name'] not in ('.CRT$XCA', '.CRT$XCZ'):
            for va, symidx, typ in sorted(info['relocs']):
                sym = obj.symbol_at(symidx)
                if sym is not None:
                    out.append(sym['name'])
    return out


def _fullmatch_at(obj, sec, s, length, text, base, a):
    o = a - base
    if length <= 0 or o < 0 or o + length > len(text):
        return False
    return re.fullmatch(obj.pattern(sec, s, s + length)[0], text[o:o + length], re.S) is not None


def _object_order(results):
    """{id(obj): [((sec, start), address or None, item)]} of each object's own functions in object order (section,
    then offset: the order /Gy's linker keeps, library-matching.md 3.6), with the address when the function is
    exact."""
    out = collections.defaultdict(list)
    for item in results:
        r, obj, sec, s, length = item
        if obj.own(sec):
            out[id(obj)].append(((sec, s), int(r['addresses'][0], 16) if r['status'] == 'exact' else None, item))
    for v in out.values():
        v.sort(key=lambda x: x[0])
    return out


def _neighbours(order, key):
    """(address of the nearest exact own function before KEY in object order, of the nearest after), None if none."""
    before = [a for k, a, _ in order if k < key and a is not None]
    after = [a for k, a, _ in order if k > key and a is not None]
    return (before[-1] if before else None), (after[0] if after else None)


def xcu_position(img, results, ranges, text, base, claimed, thunk):
    """D242 P-C1-2 (amends D94(b)): [(item, address, kind)] of `_$E` functions credited by the CRT initialiser table.
    The table lists every object's initialisers in link order, so the n-th initialiser an object's .CRT$XCU lists
    is the n-th table entry in the object's stretch. Per object: each initialiser's window is the gap between its
    nearest exact own functions in object order, clipped to the object's range +- RANGE_GAP and to the exact own
    functions of the other objects of the run; the table entries lying in the union of those windows must be
    consecutive in the table, as many as the initialisers, each the n-th initialiser's (inside its own window,
    its pattern fullmatching there, no other symbol there; an initialiser already exact must sit at its entry).
    Then the `_$E` functions they reference (the constructor wrapper they call, the atexit stub they push or jump
    to, transitively) are credited at the targets the initialisers' bytes give, where their patterns fullmatch.
    kind is 'table' or 'linked'. The names stay unused across objects: the position is the proof."""
    table = xcu_table(img)
    index = {a: i for i, a in enumerate(table)}
    orders = _object_order(results)
    own_exact = sorted((a, k) for k, v in orders.items() for _, a, _ in v if a is not None)
    by_obj = collections.defaultdict(dict)
    for item in results:
        by_obj[id(item[1])][item[0]['symbol']] = item
    out = []
    for key, defs in by_obj.items():
        if key not in ranges:
            continue
        obj = next(iter(defs.values()))[1]
        inits = xcu_initialisers(obj)
        if not inits or any(n not in defs or not generated(n) for n in inits):
            continue
        lo, hi = ranges[key]
        below = [a for a, k in own_exact if a < lo and k != key]
        above = [a for a, k in own_exact if a > hi and k != key]
        blo = max(lo - RANGE_GAP, below[-1] + 1 if below else 0)
        bhi = min(hi + RANGE_GAP, above[0] - 1 if above else 0xFFFFFFFF)
        wins = []
        for n in inits:
            r, _, sec, s, length = defs[n]
            p, q = _neighbours(orders[key], (sec, s))
            wins.append((max(blo, p + 1 if p is not None else blo), min(bhi, q - 1 if q is not None else bhi)))
        run = sorted({t for t in table for w in wins if w[0] <= t <= w[1]})
        if len(run) != len(inits) or any(index[b] != index[a] + 1 for a, b in zip(run, run[1:])):
            continue
        ok = True
        for n, (w0, w1), a in zip(inits, wins, run):
            r, _, sec, s, length = defs[n]
            if not w0 <= a <= w1 or claimed[a] - {n} or (r['status'] == 'exact' and int(r['addresses'][0], 16) != a) \
                    or r['status'] != 'exact' and not _fullmatch_at(obj, sec, s, length, text, base, a):
                ok = False
                break
        if not ok:
            continue
        got = {n: a for n, a in zip(inits, run)}
        todo = list(got.items())
        while todo:         # what the initialisers' own bytes reach: the wrapper they call, the atexit stub they push
            n, a = todo.pop()
            r, _, sec, s, length = defs[n]
            o = a - base
            for k, sz, typ, sym, addend in obj.reloc_targets(sec, s, length):
                if sz != 4 or sym is None or sym['name'] not in defs or not generated(sym['name']) \
                        or sym['name'] in got or typ not in (0x06, 0x14) or o + k + 4 > len(text):
                    continue
                v = struct.unpack('<I', text[o + k:o + k + 4])[0]
                v = (v - struct.unpack('<I', addend)[0]) & 0xFFFFFFFF if typ == 0x06 \
                    else (a + k + 4 + struct.unpack('<i', text[o + k:o + k + 4])[0]) & 0xFFFFFFFF
                v = thunk.get(v, v)
                tr, _, tsec, ts, tlen = defs[sym['name']]
                if claimed[v] - {sym['name']} or (tr['status'] == 'exact' and int(tr['addresses'][0], 16) != v) \
                        or tr['status'] != 'exact' and not _fullmatch_at(obj, tsec, ts, tlen, text, base, v):
                    continue
                got[sym['name']] = v
                todo.append((sym['name'], v))
        for n, a in got.items():
            if defs[n][0]['status'] != 'exact':
                out.append((defs[n], a, 'table' if n in inits else 'linked'))
    return out


USE_RANGE_EVIDENCE = True


def range_evidence(obj, sec, start, length, text, base, a, rng, thunk, own_names, own_known):
    """P-CW1-1: (callees hold, named, own calls) of a range-round proposal at `a`, compared as a tuple.
    `callees hold`: False when one of its relative calls / jumps goes to a function defined in this object
    whose own bytes (relocations masked) do not fullmatch where the call lands at `a` (through the thunk): the
    symbol is not at `a` (a template callback's ??_G calling its own inline destructor, against a class's ??_G
    calling a destructor of another file). `named`: the client names `a` with the function's own symbol
    (symbols.json or a vtable slot the run learned: D88 / D171's self names; or an exact function's call or
    reference to the symbol reaches `a`). `own calls`: how many of its calls go to a function of the object's
    own code (not a COMDAT: the linker keeps it with this object) and land inside the object's range (`rng`)."""
    named = a in own_names or a in own_known
    calls = 0
    o = a - base
    funcs = getattr(obj, '_funcs', None)
    if funcs is None:
        funcs = obj._funcs = {(sc, st): en for nm, sc, st, en in obj.functions()}
    for k, n, typ, sym, addend in obj.reloc_targets(sec, start, length):
        if typ != 0x14 or n != 4 or sym is None or sym['section'] <= 0 or (sym['type'] >> 4) != 2 \
                or o + k + 4 > len(text):
            continue
        t = a + k + 4 + struct.unpack('<i', text[o + k:o + k + 4])[0]
        t = thunk.get(t, t) & 0xFFFFFFFF
        end = funcs.get((sym['section'] - 1, sym['value']))
        if end is not None:
            pat, fixed, ln = obj.pattern(sym['section'] - 1, sym['value'], end)
            if ln and (not 0 <= t - base <= len(text) - ln or not re.fullmatch(pat, text[t - base:t - base + ln], re.S)):
                return (False, named, 0)
        if rng and obj.own(sym['section'] - 1) and rng[0] <= t <= rng[1]:
            calls += 1
    return (True, named, calls)


RANGE_GAP = 0x1000   # D114(b): how far beyond its own range an object's lone (or D183 shared) exact COMDAT may lie


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
        json.dump(results, open(a.json, 'w', newline='\n'), indent=1)
    counts = {}
    for r in results:
        counts[r['status']] = counts.get(r['status'], 0) + 1
        if not a.q:
            print(f"{r['status']:8s} {r['object']:16s} {r['name'][:48]:48s} len={r['length']:5d} "
                  f"{' '.join(r['addresses'][:3])}")
    print(' '.join(f'{k}={v}' for k, v in sorted(counts.items())), f'total={len(results)}')


if __name__ == '__main__':
    main()
