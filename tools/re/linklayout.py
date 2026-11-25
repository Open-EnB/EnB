"""Incremental-link layout audit of client.exe's .text: was the image one full incremental link, or updated later?

The image builder's question (1) (week plan 2026-11-24 §2 D; findings in docs/binary.md §5). VC6's LINK /INCREMENTAL
lays out a full link by rules this tool checks, and an incremental update leaves marks those rules do not explain:

    pad rule      every contribution of an object named on the command line (the app's .obj files: the ones the
                  thunk table reaches) gets a slot of up16(S + S // 4) bytes, S its section's size, the rest int3;
                  library members are packed (alignment only) and get one reserve of about 25 % at the section end
    guests        a contribution inside another's slot, the two filling it exactly: placed later into free space
    short / long  a slot below the rule (the contribution grew in place) or above it (int3 left behind: a hole)
    thunk tail    an update appends new symbols' thunks after the table's last slot; a symbol renamed (a changed
                  signature) keeps its old thunk too, so its target has two thunks (a full link gives two thunks
                  to one address only for aliases: the scalar / vector deleting destructors ??_G / ??_E)
    object runs   own functions (not COMDAT, or COMDAT NODUPLICATES) of one object sit in one run in a full link

    python tools/re/linklayout.py                       retail client.exe (function starts from clientimage)
    python tools/re/linklayout.py --objects             also the object runs (symbols.json against build/*/obj)

Summary on stdout; contributions.tsv (every slot off the rule, guests with their hosts), thunks.tsv (the doubly
reached targets and the table's last 64 slots), runs.tsv (out-of-run pieces, --objects) in --out
(default .work/linklayout/).
"""
import argparse
import bisect
import collections
import datetime
import glob
import json
import os
import re
import struct
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import clientimage  # noqa: E402

REPO = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
SYMBOLS = os.path.join(REPO, 'docs', 'data', 'symbols.json')
# the deleting destructor's flag test: test byte [esp+8], 1 / test byte [ebp+8], 1 anywhere in its first 256 bytes,
# `mov al, [esp+8]; test al, 1` anywhere there, or an entry `mov al, [esp+4]` with `test al, 1` in its first 16 bytes
#  (up to 8 bytes of other instructions between the load and the test)
DTOR_TESTS = (b'\xf6\x44\x24\x08\x01', b'\xf6\x45\x08\x01')
DTOR_LOAD_TEST = re.compile(rb'\x8a\x44\x24\x08.{0,8}\xa8\x01', re.S)


def deleting_dtor_code(code):
    return (any(x in code for x in DTOR_TESTS) or DTOR_LOAD_TEST.search(code) is not None
            or (code[:4] == b'\x8a\x44\x24\x04' and b'\xa8\x01' in code[:16]))


def slot(size):
    """The slot a full incremental link gives a contribution of `size` bytes: up16(size + size // 4)."""
    return (size + size // 4 + 15) // 16 * 16


def hidden_starts(body, base, lo, hi, starts):
    """16-aligned starts after an int3 run that follows code ending in nop fill or a return (a contribution the
    start list missed: no thunk, no direct call), inside [lo, hi), not already in `starts`."""
    have = set(starts)
    out = []
    for m in re.finditer(rb'\xCC+', body[lo - base:hi - base]):
        e = lo + m.end()
        if e % 16 or e >= hi or e in have or body[e - base] == 0xCC:
            continue
        b = lo + m.start() - base
        if b > 0 and (body[b - 1] in (0x90, 0xC3) or (b >= 3 and body[b - 3] == 0xC2)):
            out.append(e)
    return out


def contributions(body, base, starts, lo, hi):
    """[(start, size, pad)] for each start in [lo, hi): size = its bytes before the trailing int3, pad = the int3
    run up to the next start (or `hi`)."""
    ss = sorted(s for s in set(starts) if lo <= s < hi) + [hi]
    out = []
    for a, b in zip(ss, ss[1:]):
        seg = body[a - base:b - base]
        size = len(seg.rstrip(b'\xCC'))
        out.append((a, size, b - a - size))
    return out


def classify(contribs):
    """Walk the contributions in address order. Returns [{start, size, pad, cls, excess, guests}], cls one of
    rule / host (its slot holds guests that fill it exactly) / short (slot below the rule) / long (above it)."""
    out = []
    i = 0
    while i < len(contribs):
        a, s, p = contribs[i]
        want = slot(s)
        if s + p == want:
            out.append({'start': a, 'size': s, 'pad': p, 'cls': 'rule', 'excess': 0, 'guests': []})
            i += 1
            continue
        if s + p < want:
            j, end = i + 1, a + s + p
            while j < len(contribs) and end < a + want:
                end = contribs[j][0] + contribs[j][1] + contribs[j][2]
                j += 1
            if end == a + want:
                out.append({'start': a, 'size': s, 'pad': p, 'cls': 'host', 'excess': 0,
                            'guests': [c for c in contribs[i + 1:j]]})
                i = j
                continue
        out.append({'start': a, 'size': s, 'pad': p, 'cls': 'short' if s + p < want else 'long',
                    'excess': s + p - want, 'guests': []})
        i += 1
    return out


def first_fit(body, base, lo, guests):
    """For each guest (start, size): how many int3 runs of the final image between `lo` and it could have held it
    (a 16-aligned start with room for its bytes). Zero for every guest placed first-fit from `lo` (holes freed
    after a placement excepted)."""
    runs = [(lo + m.start(), lo + m.end()) for m in re.finditer(rb'\xCC+', body[lo - base:max(g for g, _ in guests) - base])] \
        if guests else []
    out = []
    for g, n in guests:
        fits = [(x, y) for x, y in runs if y <= g and y - (x + 15) // 16 * 16 >= n]
        out.append((g, n, len(fits), fits[0] if fits else None))
    return out


def thunk_table(body, base, limit=64):
    """The incremental-link table at the start of .text: [(slot VA, target VA)] of consecutive `jmp rel32`
    (int3 before the first one skipped; stops after `limit` bytes that are not a thunk)."""
    out = []
    i, misses = 0, 0
    end = base + len(body)
    while i + 5 <= len(body) and misses < limit:
        if body[i] == 0xE9:
            t = base + i + 5 + struct.unpack_from('<i', body, i + 1)[0]
            if base <= t < end:
                out.append((base + i, t))
                i += 5
                misses = 0
                continue
        i += 1
        misses += 1
    return out


def doubles(table, names, code_at):
    """Targets reached by two or more thunks: {target: (slot indexes, kind)}, kind 'deleting dtor' (a ??_G / ??_E
    name, or the flag test in its own first 256 bytes: code_at(va, n) gives at most n of the function's bytes)
    or 'other'."""
    pos = collections.defaultdict(list)
    for i, (_, t) in enumerate(table):
        pos[t].append(i)
    out = {}
    for t, ix in pos.items():
        if len(ix) < 2:
            continue
        n = names.get(t, '')
        code = code_at(t, 256)
        dtor = n.startswith(('??_G', '??_E')) or (not n and deleting_dtor_code(code))
        out[t] = (ix, 'deleting dtor' if dtor else 'other')
    return out


def own_functions(obj_glob):
    """{external own function name: {lib/object}} over our objects (objmatch.CoffObject's own(): not COMDAT, or
    COMDAT NODUPLICATES)."""
    import objmatch
    owner = collections.defaultdict(set)
    for p in glob.glob(obj_glob):
        lib = os.path.basename(os.path.dirname(os.path.dirname(p)))
        if lib in ('chk', 'stubs', 'client', 'programs'):
            continue
        try:
            o = objmatch.CoffObject(p)
        except (ValueError, struct.error):
            continue
        ext = {s['name'] for s in o.symbols if s['class'] == 2}
        for name, sec, _, _ in o.functions():
            if name in ext and o.own(sec):
                owner[name].add(f'{lib}/{os.path.basename(p)}')
    return owner


def object_runs(seq):
    """seq: [(va, object)] in address order. Returns (runs, pieces): runs [{obj, n, start, end}], pieces the runs
    of an object other than its longest one."""
    runs = []
    for va, ob in seq:
        if runs and runs[-1]['obj'] == ob:
            runs[-1]['n'] += 1
            runs[-1]['end'] = va
        else:
            runs.append({'obj': ob, 'n': 1, 'start': va, 'end': va})
    by = collections.defaultdict(list)
    for i, r in enumerate(runs):
        by[r['obj']].append(i)
    pieces = []
    for ob, ix in by.items():
        main = max(ix, key=lambda i: runs[i]['n'])
        pieces += [dict(runs[i], main=runs[main]) for i in ix if i != main]
    return runs, sorted(pieces, key=lambda r: r['start'])


def stamps(data):
    """(file header stamp, [(debug entry type, stamp, NB10 signature or None, age or None, pdb path or None)])."""
    pe = struct.unpack_from('<I', data, 0x3c)[0]
    stamp = struct.unpack_from('<I', data, pe + 8)[0]
    opt = pe + 24
    rva, size = struct.unpack_from('<II', data, opt + 96 + 8 * 6)
    nsec, optsize = struct.unpack_from('<H', data, pe + 6)[0], struct.unpack_from('<H', data, pe + 20)[0]
    off = None
    for i in range(nsec):
        vs, va, rs, raw = struct.unpack_from('<IIII', data, opt + optsize + 40 * i + 8)
        if va <= rva < va + max(vs, rs):
            off = raw + rva - va
    out = []
    for i in range(size // 28 if off is not None else 0):
        _, ts, _, _, typ, sz, _, ptr = struct.unpack_from('<IIHHIIII', data, off + 28 * i)
        rec = data[ptr:ptr + sz]
        if rec[:4] == b'NB10':
            _, sig, age = struct.unpack_from('<III', rec, 4)
            out.append((typ, ts, sig, age, rec[16:].split(b'\0')[0].decode('latin1')))
        else:
            out.append((typ, ts, None, None, None))
    return stamp, out


def rel32_into(body, base, starts, lo, hi):
    """Count the `call` / `jmp rel32` decoded by a linear sweep from each start whose target is in [lo, hi)."""
    import capstone
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    n = collections.Counter()
    ss = sorted(starts)
    for k, s in enumerate(ss):
        e = ss[k + 1] if k + 1 < len(ss) else base + len(body)
        for addr, size, mn, op in md.disasm_lite(body[s - base:e - base], s):
            if size == 5 and mn in ('call', 'jmp') and body[addr - base] in (0xE8, 0xE9) and lo <= int(op, 16) < hi:
                n[mn] += 1
    return n


def main():
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    ap.add_argument('--client', default=clientimage.DEFAULT_CLIENT)
    ap.add_argument('--objects', action='store_true', help='also the object runs (symbols.json against build/*/obj)')
    ap.add_argument('--objs', default=os.path.join(REPO, 'build', '*', 'obj', '*.obj'),
                    help='--objects: our objects (a worktree has no build/: give the main tree)')
    ap.add_argument('--out', default=os.path.join(REPO, '.work', 'linklayout'))
    a = ap.parse_args()
    ci = clientimage.ClientImage(a.client)
    ts, te, raw = ci.section('.text')
    body = ci.data[raw:raw + te - ts]
    starts, thunks = ci.function_starts()
    targets = set(thunks.values())
    app_lo, app_hi = min(targets), max(targets)
    names = {int(s['address'], 16): s['symbol'] for s in json.load(open(SYMBOLS)) if s.get('kind') == 'function'}
    os.makedirs(a.out, exist_ok=True)
    utc = lambda t: datetime.datetime.fromtimestamp(t, datetime.timezone.utc).strftime('%Y-%m-%d %H:%M:%S UTC')
    stamp, debug = stamps(ci.data)
    print(f'stamps: file header {stamp:#x} {utc(stamp)}; ' + '; '.join(
        f'debug entry type {t} {s:#x} {utc(s)}' + (f', NB10 signature {g:#x} {utc(g)} age {age} {pdb}' if g else '')
        for t, s, g, age, pdb in debug))

    # the pad rule over the app region (the thunk table's targets): its last contribution included
    last = contributions(body, ts, starts, app_hi, te)[0]
    hi = last[0] + last[1] + last[2]
    hidden = hidden_starts(body, ts, app_lo, hi, starts)
    rows = classify(contributions(body, ts, list(starts) + hidden, app_lo, hi))
    cls = collections.Counter(r['cls'] for r in rows)
    print(f'app region {app_lo:#x}-{hi:#x}: {len(rows):,} slots ({len(hidden)} starts found after int3 fill); '
          f'up16(S + S/4) {cls["rule"]:,} ({100.0 * cls["rule"] / len(rows):.2f} %), hosts of guests {cls["host"]} '
          f'({sum(len(r["guests"]) for r in rows)} guests), short {cls["short"]}, long {cls["long"]} '
          f'(int3 beyond the rule {sum(r["excess"] for r in rows if r["cls"] == "long"):,} bytes)')
    lib = contributions(body, ts, starts, hi, te)
    fill = [(m.start() + ts, m.end() - m.start()) for m in re.finditer(rb'\xCC{4096,}', body)]
    tail = next(((x, n) for x, n in fill if x > hi), None)
    if tail:
        packed = sum(1 for x, s, p in lib if x < tail[0] and p < 16)
        nlib = sum(1 for x, s, p in lib if x < tail[0])
        print(f'library region {hi:#x}-{tail[0]:#x}: {nlib:,} contributions, {packed:,} with under 16 bytes of int3 '
              f'after them; then {tail[1]:,} bytes of int3 = {100.0 * tail[1] / (tail[0] - hi):.2f} % of the library bytes')
    guests = [(g[0], g[1]) for r in rows if r['cls'] == 'host' for g in r['guests']]
    ff = first_fit(body, ts, app_lo, guests)
    fits = collections.Counter(min(k, 2) for _, _, k, _ in ff)
    print(f'guests by the int3 runs of the final image before them that could hold them: none {fits[0]}, one '
          f'{fits[1]}, more {fits[2]} (of {len(ff)}; first fit from the region start leaves none)')
    with open(os.path.join(a.out, 'contributions.tsv'), 'w') as f:
        f.write('start\tsize\tpad\tclass\texcess\tguest_of\tearlier_holes_that_fit\tname\n')
        fit = {g: k for g, _, k, _ in ff}
        for r in rows:
            if r['cls'] == 'rule':
                continue
            f.write(f"0x{r['start']:X}\t{r['size']}\t{r['pad']}\t{r['cls']}\t{r['excess']}\t\t\t{names.get(r['start'], '')}\n")
            for g, s, p in r['guests']:
                f.write(f"0x{g:X}\t{s}\t{p}\tguest\t\t0x{r['start']:X}\t{fit.get(g, '')}\t{names.get(g, '')}\n")

    table = thunk_table(body, ts)
    tg = [t for _, t in table]
    ups = sum(1 for x, y in zip(tg, tg[1:]) if y > x)
    size = {r['start']: r['size'] for r in rows}
    size.update({g: s for r in rows for g, s, _ in r['guests']})
    dd = doubles(table, names, lambda va, n: body[va - ts:va - ts + min(n, size.get(va, n))])
    kinds = collections.Counter(k for _, k in dd.values())
    print(f'thunk table {table[0][0]:#x}-{table[-1][0] + 5:#x}: {len(table):,} thunks, {len(set(tg)):,} targets; steps to a '
          f'higher target {ups:,} of {len(tg) - 1:,}; targets with two thunks {len(dd):,}: {dict(kinds)}')
    n = len(table)
    other = {t: v for t, v in dd.items() if v[1] == 'other'}
    for k in (7, 20, 100):
        print(f'  last {k:3d} slots: second thunks {sum(1 for i in range(n - k, n) if tg[i] in dd)}, of them not a '
              f'deleting destructor {sum(1 for i in range(n - k, n) if tg[i] in other)}; targets off the pad rule '
              f'{sum(1 for i in range(n - k, n) if tg[i] in {r["start"] for r in rows if r["cls"] != "rule"} | set(g for g, _ in guests))}')
    print(f'  targets with two thunks, not deleting destructors: {len(other)}, their later thunk in the last 1 % of the '
          f'table: {sum(1 for ix, _ in other.values() if max(ix) >= 0.99 * n)}')
    nm = [names[t] for t in tg if t in names]
    print(f'  steps to a higher decorated name {sum(1 for x, y in zip(nm, nm[1:]) if y > x):,} of {len(nm) - 1:,} named')
    calls = rel32_into(body, ts, [s for s in starts if s >= app_lo] + hidden, table[0][0], table[-1][0] + 5)
    print(f'  call / jmp rel32 into the table (sweep from the starts above it): call {calls["call"]:,}, jmp {calls["jmp"]:,}')
    off = {r['start']: r['cls'] for r in rows if r['cls'] != 'rule'}
    off.update({g: 'guest' for g, _ in guests})
    with open(os.path.join(a.out, 'thunks.tsv'), 'w') as f:
        f.write('slot\ttarget\tslots_of_target\tkind\tpad_rule\tname\n')
        shown = set()
        for i in list(range(max(0, n - 64), n)) + sorted(j for ix, _ in dd.values() for j in ix):
            if i in shown:
                continue
            shown.add(i)
            s, t = table[i]
            ix, kind = dd.get(t, ([i], ''))
            f.write(f"0x{s:X}\t0x{t:X}\t{','.join(hex(table[j][0]) for j in ix)}\t{kind}\t{off.get(t, 'rule')}\t{names.get(t, '')}\n")

    if a.objects:
        owner = own_functions(a.objs)
        seq = [(va, next(iter(owner[nm]))) for va, nm in sorted(names.items()) if len(owner.get(nm, ())) == 1]
        runs, pieces = object_runs(seq)
        nobj = len({r['obj'] for r in runs})
        split = {p['obj'] for p in pieces}
        twice = collections.Counter(names.values())
        named2 = [p for p in pieces if all(twice[names[va]] > 1 for va, ob in seq if p['start'] <= va <= p['end'] and ob == p['obj'])]
        print(f'object runs: {len(seq):,} named functions with one owning object, {nobj} objects, {nobj - len(split)} in one '
              f'run; {len(pieces)} out-of-run pieces ({sum(p["n"] for p in pieces)} functions), {len(named2)} of them '
              f'names symbols.json holds at two addresses')
        rs = [r['start'] for r in runs]

        def obj_at(va):
            i = bisect.bisect_right(rs, va) - 1
            return runs[i]['obj'] if i >= 0 and va <= runs[i]['end'] else None
        objs = [obj_at(t) for t in tg]
        known = [(x, y) for x, y in zip(objs, objs[1:]) if x and y]
        print(f'  thunk table: adjacent slots whose targets both lie in a run {len(known):,}, of one object '
              f'{sum(1 for x, y in known if x == y):,}')
        with open(os.path.join(a.out, 'runs.tsv'), 'w') as f:
            f.write('start\tend\tcount\tobject\tmain_start\tmain_end\tmain_count\tname_twice\tfirst_name\n')
            for p in pieces:
                m = p['main']
                first = names[p['start']]
                f.write(f"0x{p['start']:X}\t0x{p['end']:X}\t{p['n']}\t{p['obj']}\t0x{m['start']:X}\t0x{m['end']:X}\t{m['n']}"
                        f"\t{'yes' if p in named2 else 'no'}\t{first}\n")
    print(f'tables in {os.path.relpath(a.out, REPO)}')
    return 0


if __name__ == '__main__':
    sys.exit(main())
