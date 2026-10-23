"""Compares compiled vtables with the client's: which virtual functions E&B added or removed.

    python vtables.py --objs DIR [--class NAME]... [--seed NAME=ADDR]... [--list]

For every vtable in the objects (`??_7Class@@6B@`), reads its slots (the functions its
relocations point at). Using the functions already proven byte-exact (objmatch over the same
objects, plus docs/data/bytematch-*.json), finds the client's copy of the vtable in .rdata, then
aligns the two slot lists. Reports, per class: the client vtable address, slots that match, and
the slots E&B's table has that ours doesn't (and the reverse), with neighbouring names, which
locates the virtual functions added or removed in E&B's headers. Also names client functions
reached only through vtables.

Anchors are the functions proven byte-exact plus every function docs/data/symbols.json names.
Slots of the class's own functions weigh more than inherited ones: a derived class whose own code
does not match yet would otherwise land on its base class's table (whose inherited slots are the
same). When only inherited slots anchor a table, the report says so.

--seed NAME=ADDR gives the client address for a class: its vtable, or any function in it (a method
address found in the client, e.g. by `client.py refs`). The table holding that pointer is used,
whatever the anchors say. --list prints every slot side by side, both sides named.
"""
import argparse
import bisect
import difflib
import glob
import json
import os
import re
import struct
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from clientimage import ClientImage  # noqa: E402
from objmatch import CoffObject, demangle, match_objects  # noqa: E402
from client import Names  # noqa: E402

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
    path = os.path.join(DATA, 'symbols.json')
    if os.path.exists(path):
        for e in json.load(open(path)):
            if e.get('kind') == 'function' and e.get('symbol'):
                key = e['symbol'] if e['symbol'].startswith('?') else ('name', e['symbol'])
                known.setdefault(key, int(e['address'], 16))
    return known


def resolve(known, slots):
    """Client address of each slot, or None. A slot is found by its mangled symbol, or by its
    plain `Class::Method` name (symbols proven by strings) when no other slot shares the name."""
    plain = [demangle(x) for x in slots]
    out = []
    for x, d in zip(slots, plain):
        a = known.get(x)
        if a is None and plain.count(d) == 1:
            a = known.get(('name', d))
        out.append(a)
    return out


def own(cls, sym):
    """Whether a slot's function belongs to the class itself (not inherited)."""
    return demangle(sym).startswith(cls + '::')


OWN_WEIGHT = 10
_THUNKS = None


def thunks(img):
    """{incremental-link thunk: target}. Vtables of app-region classes point at the thunks."""
    global _THUNKS
    if _THUNKS is None:
        _THUNKS = img.incremental_thunks()
    return _THUNKS


_FREQ = None


def frequency(img):
    """How many .rdata dwords point at each function (through thunks): a function in every
    table (RefCountClass::Delete_This) says little about which table is which."""
    global _FREQ
    if _FREQ is None:
        start, end, raw = img.section('.rdata')
        t0, t1, _ = img.section('.text')
        th = thunks(img)
        _FREQ = {}
        for o in range(raw, raw + (end - start) - 3, 4):
            v = struct.unpack_from('<I', img.data, o)[0]
            if t0 <= v < t1:
                v = th.get(v, v)
                _FREQ[v] = _FREQ.get(v, 0) + 1
    return _FREQ


def slot_value(img, va):
    """The function a vtable slot reaches, through its thunk if it has one."""
    v = struct.unpack_from('<I', img.data, img.offset(va))[0]
    return thunks(img).get(v, v)


def score_at(img, base, anchors, rdata):
    """(weighted score, slots matched, own slots matched) of a table placed at `base`. A slot
    weighs more when it is the class's own function and when few tables point at it."""
    start, end, raw = rdata
    freq = frequency(img)
    score = n = n_own = 0
    rarest = None
    for i, a, is_own in anchors:
        o = raw + (base - start) + 4 * i
        if raw <= o < raw + (end - start) - 4 and slot_value(img, base + 4 * i) == a:
            n += 1
            n_own += is_own
            f = max(1, freq.get(a, 1))
            score += (OWN_WEIGHT if is_own else 1) / f
            rarest = f if rarest is None else min(rarest, f)
    return score, n, n_own, rarest


RARE = 4   # a slot pointing at a function in at most this many tables identifies the table


def confident(n_own, rarest):
    """Whether anchors place a table: a rare slot, or two of the class's own functions.
    One slot that most tables share (RefCountClass::Delete_This) places nothing."""
    return n_own >= 2 or (rarest is not None and rarest <= RARE)


def find_in_client(img, slots, known, rdata, cls):
    """Best .rdata position for a vtable, by the known slots that line up (own slots weigh
    more). Returns (base, slots matched, own slots matched, fewest tables sharing a matched slot)."""
    start, end, raw = rdata
    data = img.data
    anchors = [(i, a, own(cls, x)) for i, (x, a) in enumerate(zip(slots, resolve(known, slots))) if a]
    if not anchors:
        return None, 0, 0, None
    via = {}
    for th, t in thunks(img).items():
        via.setdefault(t, []).append(th)
    candidates = set()
    for i0, a0, _ in anchors:
        for v in [a0] + via.get(a0, []):
            needle = struct.pack('<I', v)
            pos = data.find(needle, raw, raw + (end - start))
            while pos >= 0:
                if (pos - raw) % 4 == 0:
                    candidates.add(start + (pos - raw) - 4 * i0)
                pos = data.find(needle, pos + 1, raw + (end - start))
    best = (0, None, 0, 0, None)
    for base in sorted(candidates):
        sc, n, n_own, rarest = score_at(img, base, anchors, rdata)
        if sc > best[0]:
            best = (sc, base, n, n_own, rarest)
    return best[1], best[2], best[3], best[4]


def table_from_seed(img, addr):
    """The vtable a seed address belongs to: the address itself when the code stores it as a
    vtable pointer, else the start of the table holding a pointer to it (the stored vtable
    pointer nearest below it, in the same run of code pointers)."""
    if vptr_stored(img, [addr]):
        return addr
    start, end, raw = img.section('.rdata')
    t0, t1, _ = img.section('.text')
    needle = struct.pack('<I', addr)
    found = []
    pos = img.data.find(needle, raw, raw + (end - start))
    while pos >= 0:
        if (pos - raw) % 4 == 0:
            va = start + (pos - raw)
            run = [va]
            while t0 <= img.u32(run[-1] - 4) < t1:
                run.append(run[-1] - 4)
            stored = vptr_stored(img, run)
            if stored:
                found.append(max(stored))
        pos = img.data.find(needle, pos + 1, raw + (end - start))
    if len(found) > 1:
        print(f'seed 0x{addr:X} is in {len(found)} tables: '
              + ', '.join(f'0x{f:X}' for f in found) + '; using the first', file=sys.stderr)
    return found[0] if found else None


_STORED = None


def stored_vptrs(img):
    """Every .rdata address the code stores through `mov dword ptr [reg], imm32`: constructors
    and destructors write a vtable's start, never an address inside it."""
    global _STORED
    if _STORED is None:
        t0, t1, raw = img.section('.text')
        r0, r1, _ = img.section('.rdata')
        text = img.data[raw:raw + (t1 - t0)]
        _STORED = set()
        for m in re.finditer(rb'\xC7[\x00\x01\x02\x03\x06\x07]', text):
            o = m.start() + 2
            if o + 4 <= len(text):
                v = struct.unpack_from('<I', text, o)[0]
                if r0 <= v < r1:
                    _STORED.add(v)
    return _STORED


def vptr_stored(img, candidates):
    """The candidates that the code stores as a vtable pointer."""
    stored = stored_vptrs(img)
    return [c for c in candidates if c in stored]


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--objs', required=True)
    ap.add_argument('--class', dest='classes', action='append')
    ap.add_argument('--seed', action='append', default=[], metavar='CLASS=ADDR',
                    help="the client's vtable for CLASS, or the address of a function in it")
    ap.add_argument('--list', action='store_true', help='print every slot, both sides named')
    ap.add_argument('--json')
    a = ap.parse_args()
    img = ClientImage()
    seeds = {}
    for sd in a.seed:
        c, _, v = sd.partition('=')
        seeds[c] = int(v, 16)
    if seeds:
        a.classes = sorted(set(a.classes or []) | set(seeds))
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
    seed_table = {}
    for vt, slots in tables.items():
        cls = vt[4:].split('@@')[0]
        if cls in seeds:
            cur = seed_table.get(cls)
            if cur is None or vt.endswith('@@6B@') or (not cur.endswith('@@6B@') and len(slots) > len(tables[cur])):
                seed_table[cls] = vt
    report = []
    names = None
    placed = {}       # class -> its primary table's client address
    unplaced = {}     # class -> tables not placed
    for vt, slots in sorted(tables.items()):   # a class's primary table (@@6B@) sorts first
        cls = vt[4:].split('@@')[0]
        if a.classes and cls not in a.classes:
            continue
        seeded = seed_table.get(cls) == vt
        if cls in seeds and not seeded:
            continue
        if seeded:
            base = table_from_seed(img, seeds[cls])
            if not base:
                print(f'{cls}: seed 0x{seeds[cls]:X} is not in any vtable the code stores', file=sys.stderr)
                continue
            anchors = [(i, a, own(cls, x)) for i, (x, a) in enumerate(zip(slots, resolve(known, slots))) if a]
            _, score, score_own, _ = score_at(img, base, anchors, rdata)
        else:
            base, score, score_own, rarest = find_in_client(img, slots, known, rdata, cls)
            if base and (not confident(score_own, rarest)
                         or (not vt.endswith('@@6B@') and placed.get(cls) == base)):
                base = None
            if not base:
                unplaced[cls] = unplaced.get(cls, 0) + 1
                continue
            # E&B may have inserted or removed slots before the anchors: prefer the nearby address
            # that the code actually stores as a vtable pointer.
            stored = vptr_stored(img, [base + 4 * k for k in range(-4, 5)])
            if stored and base not in stored:
                base = min(stored, key=lambda c: abs(c - base))
        n = len(slots) + 16
        client = [struct.unpack_from('<I', img.data, img.offset(base) + 4 * i)[0] for i in range(n)]
        raw_client = client
        # stop the client table where it stops pointing into .text
        t0, t1, _ = img.section('.text')
        m = next((i for i, v in enumerate(client) if not (t0 <= v < t1)), n)
        nxt = next((i for i in range(1, m) if base + 4 * i in stored_vptrs(img)), m)
        client = [thunks(img).get(v, v) for v in raw_client[:nxt]]
        ours = resolve(known, slots)
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
        if vt.endswith('@@6B@'):
            placed[cls] = base
        placed.setdefault(('any', cls), base)
        report.append({'class': cls, 'client_vtable': f'0x{base:06X}', 'slots': len(slots),
                       'client_slots': len(client), 'anchored': score, 'anchored_own': score_own,
                       'seeded': seeded, 'differences': diffs})
        if diffs or a.classes:
            print(f"{cls}: client vtable 0x{base:06X}{' (seeded)' if seeded else ''}, {len(slots)} slots here, "
                  f"{len(client)} in the client, {score} anchored ({score_own} own)")
            if score_own == 0 and not seeded:
                print(f"   warning: only inherited slots anchor this table; it may be a base class's. "
                      f"Seed it: --seed {cls}=ADDR (a method's client address)")
            for d in diffs:
                print(f"   {d['op']:8s} after {d['after'][:40]:40s} ours {d['ours'][:4]} client {d['client'][:4]}"
                      f" (slot {d['our_slot']} / {d['client_slot']})")
            if a.list:
                if names is None:
                    names = Names(img)
                for i in range(max(len(slots), len(client))):
                    o = demangle(slots[i]) if i < len(slots) else ''
                    c = client[i] if i < len(client) else None
                    cn = (names.of(c) or '') if c is not None else ''
                    mark = '=' if c is not None and i < len(ours) and ours[i] == c else ' '
                    cs = f'0x{c:06X}' if c is not None else ''
                    print(f"   {i:3d} {mark} {cs:9s} {cn[:45]:45s} | {o}")
    if a.classes:
        have = {vt[4:].split('@@')[0] for vt in tables}
        for cls in sorted(set(a.classes) - have):
            print(f"{cls}: no vtable of this class in the objects ({a.objs}); compile its file, or use "
                  f"objects that have it (e.g. an EA copy's in .work/bm2)")
        for cls, k in sorted(unplaced.items()):
            if ('any', cls) in placed:
                print(f"{cls}: {k} more table(s) of this class not placed (secondary bases)")
            else:
                print(f"{cls}: not placed (no slot known in the client, or only slots most tables share). "
                      f"Seed it: --seed {cls}=ADDR (its vtable, or a method's client address)")
    if a.json:
        json.dump(report, open(a.json, 'w'), indent=1)


if __name__ == '__main__':
    main()
