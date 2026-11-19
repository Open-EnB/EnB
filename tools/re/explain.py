"""Classifies why a compiled function is not byte-exact: one line first, then the first mismatch.

    python explain.py file.obj SYMBOL CLIENT_ADDRESS [--n INDEX] [--window N] [--json]

SYMBOL: a function whose mangled name, demangled name ('Class::name') or bare method name equals it; only
when no name equals it, the functions whose mangled or demangled name contains it ('Class::pump' no longer
also selects pumpClose). `--n` picks among several (the candidates are listed when INDEX is out of range).
Both sides are disassembled as objdiff.py does,
then normalised: relocated fields in the object and image addresses in the client become G,
branch and call targets T; stack offsets ([esp + N], and [ebp +- N] in an ebp frame) S, as
.work/c/mask.py did. Output:

    class: <class>: <reason>
    the first mismatch in a short window (ours | client, `*` on differing rows)
    masked residue: N rows differ (stack offsets, targets, relocations masked); with registers also masked: M

Classes, in triage order (an earlier one masks the later ones; fix it first):
  relocation       an aligned instruction references another symbol or string than the client's
                   (a known symbol at another address, a different string, or an unknown symbol where
                   the client's target has another known name: a wrong declaration). Skipped for
                   calls when the call counts differ (that is the inline-budget symptom)
  misplaced-start  CLIENT_ADDRESS is no verified function start, or the client's window opens with
                   instructions ours lacks that end in a ret/jmp (another function's tail)
  frame            the stack frame size (`sub esp, N`, an ebp frame, an EH frame) differs
  field-offset     same instruction shape, a `[reg + N]` displacement differs (a class layout)
  literal          same instruction shape, an immediate differs
  register-only    identical once registers and stack slots are normalised (allocation; VC6 may
                   also reschedule around it): no switch reproduces these, the source decides
  order-only       the same instructions, registers included, in a different order
  inline-budget    a callee called at one site and inlined at another: call counts differ, a known
                   callee called on one side only, or the `??_H`/`??_I` vector-constructor
                   temporaries differ (workflow.md §3.1.2)
  shape            everything else
  branch           nothing differs once masked, but an aligned jump is short on one side and near on the
                   other, reaches another row, or the code lengths differ (the target mask hid a logic change)
  identical        nothing differs once stack offsets are masked (only slot numbers: re-run objmatch)

A switch's jump table and a two-level switch's byte table (the operands of `jmp dword ptr [reg*4 + T]` and
`mov cl, byte ptr [reg + T]`) end the code on both sides: they are compared apart (jump entries by rank, which
cases share a body and in which order; byte entries as they are) and printed as `switch ... table` lines, never
counted as residue rows; tables that differ under identical code make the class `shape`.

register-only and order-only are exclusive (order-only needs the registers equal), so their order in
the list doesn't matter. Read docs/workflow.md §3.1.2's table for the lever per class.
"""
import argparse
import collections
import difflib
import json
import os
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import capstone  # noqa: E402
from clientimage import ClientImage  # noqa: E402
from objmatch import CoffObject, RELOC_SIZE, demangle, symbol_addresses, thunks_of  # noqa: E402

MD = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
MD.detail = True
IMAGE = (0x400000, 0xD00000)
ABSOLUTE = {'__except_list'}
NUM = re.compile(r'(?<![\w.])(-?)(0x[0-9a-f]+|\d+)(?![\w])')
REG = re.compile(r'\b(e[abcd]x|e[sd]i|ebp|[abcd]x|[sd]i|bp|[abcd][lh])\b')
SYMBOLS_JSON = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 'docs', 'data', 'symbols.json')


class Ins:
    __slots__ = ('addr', 'size', 'mn', 'op', 'raw', 'refs', 'target', 'disp_off', 'imm_off')

    def __init__(self, ins):
        self.addr, self.size, self.mn, self.op = ins.address, ins.size, ins.mnemonic, ins.op_str
        self.raw = f'{ins.mnemonic} {ins.op_str}'.strip()
        self.refs = []      # [(kind, what)]: ('sym', name) / ('str', bytes) on ours, ('addr', va) on the client's
        self.target = None  # branch/call target: ('sym', name), ('addr', va) or ('local', offset)
        try:
            self.disp_off, self.imm_off = ins.disp_offset, ins.imm_offset
        except capstone.CsError:
            self.disp_off = self.imm_off = 0


def _names_at():
    out = collections.defaultdict(set)
    try:
        for e in json.load(open(SYMBOLS_JSON)):
            out[int(e['address'], 16)].add(e.get('symbol') or e.get('name'))
    except (OSError, ValueError, KeyError):
        pass
    return out


def is_branch(ins):
    return ins.mn.startswith('j') or ins.mn in ('call', 'loop', 'jecxz')


def num_value(tok_sign, tok):
    v = int(tok, 16) if tok.startswith('0x') else int(tok)
    return -v if tok_sign else v


def replace_num(text, value, where):
    """Replace one numeric token equal to value (mod 2**32): `where` = 'disp' (inside []) or 'imm'."""
    hits = []
    for m in NUM.finditer(text):
        if (num_value(m.group(1), m.group(2)) - value) % (1 << 32) == 0:
            inside = text.rfind('[', 0, m.start()) > text.rfind(']', 0, m.start())
            hits.append((inside, m))
    pick = [m for inside, m in hits if inside == (where == 'disp')] or [m for _, m in hits]
    if not pick:
        if where == 'disp' and '[' in text:  # a zero displacement capstone omits
            k = text.index(']')
            return text[:k] + ' + G' + text[k:]
        return text
    m = pick[-1] if where == 'imm' else pick[0]
    return text[:m.start()] + 'G' + text[m.end():]


def obj_side(obj, sec, s, e):
    info = obj.sections[sec]
    body = obj.data[info['raw'] + s:info['raw'] + e]
    relocs = obj.reloc_targets(sec, s, e - s)
    out = []
    for ci in MD.disasm(body, 0):
        ins = Ins(ci)
        text = ins.raw
        mine = [(off - ci.address, n, typ, sym, add) for off, n, typ, sym, add in relocs
                if ci.address <= off < ci.address + ci.size]
        for off, n, typ, sym, add in sorted(mine, key=lambda r: r[0]):
            name = sym['name'] if sym else '?'
            if name in ABSOLUTE:  # the linker resolves it to a constant (fs:[0]), not an image address
                continue
            if typ == 20 and is_branch(ins):  # REL32 call/jmp to a symbol
                ins.target = ('sym', name)
                text = f'{ins.mn} T'
                continue
            value = int.from_bytes(add, 'little', signed=False)
            text = replace_num(text, value, 'disp' if off == ins.disp_off else 'imm')
            lit = obj.literal(sym)
            ins.refs.append(('str', lit) if lit is not None else ('sym', name))
        if is_branch(ins) and ins.target is None and ci.operands and ci.operands[0].type == capstone.x86.X86_OP_IMM:
            ins.target = ('local', ci.operands[0].imm)
            text = f'{ins.mn} T'
        out.append((ins, text))
    return out


def client_side(img, addr, length):
    o = img.offset(addr)
    out = []
    thunks = thunks_of(img)
    for ci in MD.disasm(img.data[o:o + length], addr):
        ins = Ins(ci)
        text = ins.raw
        if is_branch(ins) and ci.operands and ci.operands[0].type == capstone.x86.X86_OP_IMM:
            t = ci.operands[0].imm
            ins.target = ('addr', thunks.get(t, t))
            text = f'{ins.mn} T'
        else:
            vals = []
            for m in NUM.finditer(text):
                v = num_value(m.group(1), m.group(2))
                if IMAGE[0] <= v < IMAGE[1]:
                    vals.append(v)
            for v in vals:
                inside = 'disp' if f'[0x{v:x}' in text or f' 0x{v:x}]' in text else 'imm'
                text = replace_num(text, v, inside)
                ins.refs.append(('addr', v))
        out.append((ins, text))
        if ci.address + ci.size - addr >= length:
            break
    return out


JUMP = re.compile(r'^jmp dword ptr \[e\w\w\*4(?: \+ (0x[0-9a-f]+|\d+))?\]$')
BYTE_LOAD = re.compile(r'^(mov|movzx) \w+, byte ptr \[e\w\w(?: \+ (0x[0-9a-f]+|\d+))?\]$')


def table_refs(rows, where):
    """[(table start, 'jump' | 'byte')] a switch's rows read: `jmp dword ptr [reg*4 + TABLE]` and a two-level
    switch's `mov cl, byte ptr [reg + TABLE]`; where(ins) gives the table's address (or None)."""
    out = []
    for ins, _ in rows:
        kind = 'jump' if JUMP.match(ins.raw) else 'byte' if BYTE_LOAD.match(ins.raw) else None
        if kind:
            t = where(ins)
            if t is not None and t > ins.addr:
                out.append((t, kind))
    return sorted(set(out))


def split_tables(rows, refs, end, entries):
    """(code rows, tables): the rows from the first table start on are the switch tables (VC6 puts them after the
    function's last ret), not code. entries(start, kind, size) gives a table's entries: jump targets (addresses on
    the rows' scale) or bytes. A jump entry becomes the index of the code row it reaches (None: no row starts there),
    so two sides compare whatever their code addresses. tables: [{'kind', 'at', 'entries'}]."""
    starts = [t for t, _ in refs]
    if not starts:
        return rows, []
    cut = starts[0]
    code = [r for r in rows if r[0].addr < cut]
    index = {ins.addr: k for k, (ins, _) in enumerate(code)}
    tables = []
    for k, (t, kind) in enumerate(refs):
        size = (starts[k + 1] if k + 1 < len(starts) else end) - t
        got = entries(t, kind, size, k + 1 == len(starts))
        if kind == 'jump':
            got = [index.get(x) for x in got]
        tables.append({'kind': kind, 'at': t, 'entries': got})
    return code, tables


def _ranks(xs):
    order = {v: k for k, v in enumerate(sorted({x for x in xs if x is not None}))}
    return [order.get(x) for x in xs]


def compare_tables(tl, tr):
    """[{'kind', 'ours', 'client', 'at', 'n', 'diff'}]: the tables paired by kind in order, entries compared one
    to one (diff counts the differing and the unpaired entries)."""
    out = []
    for kind in ('jump', 'byte'):
        a = [t for t in tl if t['kind'] == kind]
        b = [t for t in tr if t['kind'] == kind]
        for k in range(max(len(a), len(b))):
            x = a[k]['entries'] if k < len(a) else []
            y = b[k]['entries'] if k < len(b) else []
            if kind == 'jump':      # by rank: which cases share a body and in what order the bodies come
                x, y = _ranks(x), _ranks(y)
            diff = sum(1 for p, q in zip(x, y) if p != q) + abs(len(x) - len(y))
            out.append({'kind': kind, 'ours': len(x), 'client': len(y), 'n': max(len(x), len(y)), 'diff': diff,
                        'at': b[k]['at'] if k < len(b) else None})
    return out


def branch_shape(L, R, sm):
    """What the masks hide (SR2): {'bytes': (ours, client), 'size': rows, 'target': rows} where size counts the
    aligned (equal-text) jumps that are short on one side and near on the other, and target those whose local
    target, as a row index carried through the alignment, is another row than the client's. A logic change that
    only stretches a short jump into a near one leaves the masked residue at 0 and shows here."""
    il = {ins.addr: k for k, (ins, _) in enumerate(L)}
    ir = {ins.addr: k for k, (ins, _) in enumerate(R)}
    to_r = {}
    for tag, i1, i2, j1, j2 in sm.get_opcodes():
        if tag == 'equal':
            to_r.update({i1 + k: j1 + k for k in range(i2 - i1)})
    size = target = 0
    for i, j in to_r.items():
        a, b = L[i][0], R[j][0]
        if not (a.mn.startswith('j') or a.mn in ('loop', 'jecxz')) or a.mn != b.mn:
            continue
        if not (a.target and a.target[0] == 'local' and b.target and b.target[0] == 'addr'):
            continue
        if a.size != b.size:
            size += 1
        ti, tj = il.get(a.target[1]), ir.get(b.target[1])
        if (ti is None) != (tj is None) or (ti in to_r and to_r[ti] != tj):
            target += 1
    return {'bytes': (sum(i.size for i, _ in L), sum(i.size for i, _ in R)), 'size': size, 'target': target}


def branch_differs(b):
    return bool(b['size'] or b['target'] or b['bytes'][0] != b['bytes'][1])


def obj_tables(obj, sec, s, e, L):
    """split_tables for our side (row addresses are offsets from the function's start)."""
    rel = {}
    for off, n, typ, sym, add in obj.reloc_targets(sec, s, e - s):
        if sym and sym['section'] == sec + 1 and n == 4:
            rel[off] = sym['value'] + int.from_bytes(add, 'little') - s
    refs = table_refs(L, lambda ins: rel.get(ins.addr + ins.disp_off) if ins.disp_off else None)
    body = obj.data[obj.sections[sec]['raw'] + s:obj.sections[sec]['raw'] + e]

    def entries(t, kind, size, last):
        if kind == 'byte':
            got = bytearray(body[t:t + size])
            while last and got and got[-1] in (0xCC, 0x90):
                got.pop()
            return list(got)
        out = []
        for k in range(size // 4):
            if t + 4 * k not in rel:
                break
            out.append(rel[t + 4 * k])
        return out
    return split_tables(L, refs, e - s, entries)


def client_tables(img, addr, end, R):
    """split_tables for the client's side (absolute addresses; the window ends at end)."""
    def where(ins):
        m = re.search(r'\+ (0x[0-9a-f]+)\]$', ins.raw)
        v = int(m.group(1), 16) if m else None
        return v if v is not None and addr < v < end else None
    refs = table_refs(R, where)
    first = refs[0][0] if refs else end

    def entries(t, kind, size, last):
        o = img.offset(t)
        if kind == 'byte':
            got = bytearray(img.data[o:o + size])
            while last and got and got[-1] in (0xCC, 0x90):
                got.pop()
            return list(got)
        out = []
        for k in range(size // 4):
            v = int.from_bytes(img.data[o + 4 * k:o + 4 * k + 4], 'little')
            if not addr <= v < first:
                break
            out.append(v)
        return out
    return split_tables(R, refs, end, entries)


def ebp_frame(rows):
    return any(i.mn == 'mov' and i.op == 'ebp, esp' for i, _ in rows[:6])


def same_string(a, b):
    """C strings as objmatch compares them: case aside, and a __FILE__ path by its file name."""
    a, b = a.rstrip(b'\0').lower(), b.rstrip(b'\0').lower()
    if a == b:
        return True
    if re.search(rb'\.(cpp|h|c|inl)$', a) and re.search(rb'\.(cpp|h|c|inl)$', b):
        return a.split(b'\\')[-1] == b.split(b'\\')[-1]
    return False


def mask_stack(t, ebp):
    t = re.sub(r'\[esp( \+ (0x[0-9a-f]+|\d+))?\]', '[esp+S]', t)
    if ebp:
        t = re.sub(r'\[ebp [+-] (0x[0-9a-f]+|\d+)\]', '[ebp+S]', t)
    t = re.sub(r'^(sub|add) esp, (0x[0-9a-f]+|\d+)$', r'\1 esp, F', t)
    return t


def mask_field(t):
    def f(m):
        inner = m.group(1)
        if inner.startswith('esp') or inner == 'G' or 'S' in inner.split()[-1:]:
            return m.group(0)
        inner = re.sub(r' [+-] (0x[0-9a-f]+|\d+)$', '', inner)
        return f'[{inner} + D]'
    return re.sub(r'\[([^\]]+)\]', f, t)


def mask_literal(t):
    out = []
    last = 0
    for m in NUM.finditer(t):
        inside = t.rfind('[', 0, m.start()) > t.rfind(']', 0, m.start())
        if not inside:
            out.append(t[last:m.start()] + 'K')
            last = m.end()
    out.append(t[last:])
    return ''.join(out)


def mask_regs(t):
    return REG.sub('R', t)


def residue(a, b):
    sm = difflib.SequenceMatcher(None, a, b, autojunk=False)
    return sum(max(i2 - i1, j2 - j1) for tag, i1, i2, j1, j2 in sm.get_opcodes() if tag != 'equal'), sm


def frame_of(rows):
    eh = any('fs:[0]' in i.raw.replace(' ', '') or 'fs:[G]' in t for i, t in rows[:12])
    size = None
    for i, _ in rows[:10]:
        if i.mn == 'sub' and i.op.startswith('esp, '):
            size = i.op.split(', ')[1]
            break
    return (size, ebp_frame(rows), eh)


def pick(funcs, symbol):
    """The functions SYMBOL names: those whose mangled name, demangled name or bare method name equals it;
    when none does, those whose mangled or demangled name contains it."""
    exact = [f for f in funcs if symbol in (f[0], demangle(f[0]), demangle(f[0]).rsplit('::', 1)[-1])]
    return exact or [f for f in funcs if symbol in f[0] or symbol in demangle(f[0])]


def explain(obj_path, symbol, address, n=0, window=3):
    obj = CoffObject(obj_path)
    cands = pick(obj.functions(), symbol)
    if not cands:
        raise SystemExit('no such symbol')
    if not 0 <= n < len(cands):
        raise SystemExit(f'--n {n}: {len(cands)} candidate(s) (--n is the index among them, from 0): '
                         + ', '.join(f'{k} {f[0]}' for k, f in enumerate(cands[:12])))
    name, sec, s, e = cands[n]
    img = ClientImage()
    addr = int(address, 16)
    # the window ends at the nearest verified or proven start (symbols.json), else it runs into the next function
    starts = sorted(set(img.function_starts()[0]) | set(_names_at()))
    nxt = next((x for x in starts if x > addr), addr + (e - s) + 64)
    length = max(1, min(nxt - addr, (e - s) * 2 + 64))
    L, R = obj_side(obj, sec, s, e), client_side(img, addr, length)
    # a switch's jump / byte tables after the last ret are data: compared apart, never as residue rows
    L, tl = obj_tables(obj, sec, s, e, L)
    R, tr = client_tables(img, addr, addr + length, R)
    tables = compare_tables(tl, tr)
    # trailing padding of ours (int3 / nop) is not part of the function
    while L and L[-1][0].mn in ('int3', 'nop'):
        L.pop()
    while R and R[-1][0].mn in ('int3', 'nop'):
        R.pop()
    eL, eR = ebp_frame(L), ebp_frame(R)
    SL = [mask_stack(t, eL) for _, t in L]
    SR = [mask_stack(t, eR) for _, t in R]
    n_s, sm = residue(SL, SR)
    n_r, _ = residue([mask_regs(x) for x in SL], [mask_regs(x) for x in SR])
    BL, BR = [t for _, t in L], [t for _, t in R]
    names_at = _names_at()
    known = symbol_addresses()

    result = {'symbol': demangle(name), 'mangled': name, 'address': f'0x{addr:06X}', 'ours': len(L),
              'client': len(R), 'residue': n_s, 'residue_regs': n_r, 'tables': tables,
              'branch': branch_shape(L, R, sm)}
    branch = result['branch']

    def done(cls, reason, at=None):
        result.update({'class': cls, 'reason': reason})
        return result, (L, R, SL, SR, sm, at)

    calls_l = [i.target for i, _ in L if i.mn == 'call' and i.target and i.target[0] == 'sym']
    calls_r = [i.target for i, _ in R if i.mn == 'call' and i.target]
    n_calls_l = sum(1 for i, _ in L if i.mn == 'call')
    n_calls_r = sum(1 for i, _ in R if i.mn == 'call')

    # 1. relocation targets on aligned (equal-shape) rows. A pair counts only when neither side's
    # target occurs anywhere on the other side: when the shapes differ elsewhere, difflib can pair
    # one assert's string with another's.
    def cstring(va):
        got = img.cstr(va, 1024)
        return got.encode('latin1') if isinstance(got, str) else None
    ours_strs = [v for i, _ in L for k, v in i.refs if k == 'str']
    ours_syms = {v for i, _ in L for k, v in i.refs + ([i.target] if i.target else []) if k == 'sym'}
    ours_known = set().union(*[known.get(x, set()) for x in ours_syms]) if ours_syms else set()
    client_vals = {v for i, _ in R for k, v in i.refs + ([i.target] if i.target else []) if k == 'addr'}
    client_strs = [s for s in (cstring(v) for v in client_vals) if s]
    for tag, i1, i2, j1, j2 in sm.get_opcodes():
        if tag != 'equal':
            continue
        for k in range(i2 - i1):
            li, ri = L[i1 + k][0], R[j1 + k][0]
            pairs = []
            if li.target and li.target[0] == 'sym' and ri.target and n_calls_l == n_calls_r:
                pairs.append((li.target, ri.target))
            pairs += list(zip(li.refs, ri.refs))
            for (lk, lv), (rk, rv) in pairs:
                if rk != 'addr':
                    continue
                if lk == 'str':
                    got = img.cstr(rv, len(lv) + 1) if hasattr(img, 'cstr') else None
                    got = got.encode('latin1') if isinstance(got, str) else got
                    if got is not None and not same_string(got, lv) and \
                            not any(same_string(got, s) for s in ours_strs) and \
                            not any(same_string(lv, s) for s in client_strs):
                        return done('relocation', f'string differs: ours {lv.rstrip(bytes(1))!r}, client '
                                    f'{got.rstrip(bytes(1))!r} at 0x{rv:06X}' +
                                    (f' ({n_calls_l} calls in ours, {n_calls_r} in the client: an inlined '
                                     "callee's assert? then inline-budget)" if n_calls_l != n_calls_r else ''),
                                    (i1 + k, j1 + k))
                    continue
                addrs = known.get(lv)
                there = names_at.get(rv, set())
                if addrs and rv not in addrs and not (there & {lv}) and not (addrs & client_vals) \
                        and rv not in ours_known and not (there & ours_syms):
                    return done('relocation', f'ours references {demangle(lv)} (0x{min(addrs):06X}), the client '
                                f'0x{rv:06X} {"/".join(sorted(map(demangle, there))) or "(unnamed)"}', (i1 + k, j1 + k))
                if not addrs and there and lv not in there and not lv.startswith(('$', '__real', '??_C')) \
                        and rv not in ours_known and not (there & ours_syms):
                    return done('relocation', f'unresolved: ours references {lv}, the client 0x{rv:06X} is '
                                f'{"/".join(sorted(there))} (a different declaration?)', (i1 + k, j1 + k))

    # 2. misplaced start
    if addr not in set(starts):
        return done('misplaced-start', f'0x{addr:06X} is not a verified function start', (0, 0))
    ops = sm.get_opcodes()
    if ops and ops[0][0] in ('insert', 'replace') and ops[0][3] == 0 and ops[0][4] > 0:
        head = R[:ops[0][4]]
        if any(i.mn in ('ret', 'jmp') for i, _ in head) and len(ops) > 1 and ops[1][0] == 'equal':
            return done('misplaced-start', f'the client window opens with {ops[0][4]} instructions ours lacks, '
                        f'ending a function ({head[-1][0].raw})', (0, 0))

    # 3. frame
    fl, fr = frame_of(L), frame_of(R)
    if fl != fr and n_calls_l == n_calls_r:  # with other calls, inlined callees' locals move the frame
        what = []
        if fl[0] != fr[0]:
            what.append(f'sub esp {fl[0]} vs {fr[0]}')
        if fl[1] != fr[1]:
            what.append(f'ebp frame {fl[1]} vs {fr[1]}')
        if fl[2] != fr[2]:
            what.append(f'EH frame {fl[2]} vs {fr[2]}')
        k = next((x for x, (i, _) in enumerate(L[:10]) if i.mn == 'sub' and i.op.startswith('esp')), 0)
        kr = next((x for x, (i, _) in enumerate(R[:10]) if i.mn == 'sub' and i.op.startswith('esp')), 0)
        return done('frame', 'frame differs: ' + ', '.join(what) + ' (ours vs client)', (k, kr))

    # rows of replace blocks paired one to one
    paired = []
    for tag, i1, i2, j1, j2 in ops:
        if tag == 'replace':
            for k in range(min(i2 - i1, j2 - j1)):
                paired.append((i1 + k, j1 + k))
    first_diff = next(((i1, j1) for tag, i1, i2, j1, j2 in ops if tag != 'equal'), None)
    if n_s == 0 and branch_differs(branch):
        return done('branch', f"nothing differs once masked, but the branches do: {branch['size']} short/near, "
                    f"{branch['target']} to another row, code {branch['bytes'][0]}/{branch['bytes'][1]} bytes "
                    '(a logic difference the target mask hides)', first_diff)
    if n_s == 0 and any(t['diff'] for t in tables):
        t = next(t for t in tables if t['diff'])
        return done('shape', f"the code agrees but a switch {t['kind']} table differs ({t['diff']} of {t['n']} "
                    'entries: case order or case bodies)', None)
    if n_s == 0:
        return done('identical', 'nothing differs once stack offsets are masked' +
                    (' (stack slots differ)' if BL != BR else ''), first_diff)

    # 4. field offsets, 5. literals
    for cls, fn, why in (('field-offset', mask_field, 'a [reg + N] displacement differs'),
                         ('literal', mask_literal, 'an immediate differs')):
        for a, b in paired:
            if SL[a] != SR[b] and fn(SL[a]) == fn(SR[b]):
                return done(cls, f'{why}: ours `{L[a][0].raw}`, client `{R[b][0].raw}`', (a, b))

    # 6./7. register-only, order-only
    cl, cr = collections.Counter(SL), collections.Counter(SR)
    if cl == cr:
        return done('order-only', f'the same {len(SL)} instructions, registers included, in another order', first_diff)
    if collections.Counter(map(mask_regs, SL)) == collections.Counter(map(mask_regs, SR)):
        same_seq = [mask_regs(x) for x in SL] == [mask_regs(x) for x in SR]
        return done('register-only', 'identical once registers are normalised' +
                    ('' if same_seq else ' (and VC6 schedules the loads around the allocation)'), first_diff)

    # 8. inline budget: a callee on one side only (a merged or split tail calls the same ones)
    why = []
    tl = {t[1] for t in calls_l}
    tr = {t[1] for t in calls_r}
    ours_addrs = set().union(*[known.get(x, set()) for x in tl]) if tl else set()
    unknown_ours = {x for x in tl if not known.get(x)}
    only_ours = sorted(demangle(x) for x in tl if known.get(x) and not (known[x] & tr))
    unmatched_r = {a for a in tr if a not in ours_addrs and not (names_at.get(a, set()) & tl)}
    only_client = sorted({'/'.join(sorted(map(demangle, names_at[a]))) for a in unmatched_r if names_at.get(a)})
    if only_ours:
        why.append('called only in ours: ' + ', '.join(only_ours[:3]))
    if len(unmatched_r) > len(unknown_ours):
        why.append(f'{len(unmatched_r)} callees only in the client' +
                   (': ' + ', '.join(only_client[:3]) if only_client else ''))
    hl = sum(1 for i, _ in L for k, v in i.refs + ([i.target] if i.target else [])
             if isinstance(v, str) and v.startswith(('??_H', '??_I')))
    hr = sum(1 for i, _ in R if i.target and any(n.startswith(('??_H', '??_I')) for n in names_at.get(i.target[1], ())))
    if hl != hr:
        why.append(f'??_H/??_I temporaries {hl} in ours, {hr} in the client')
    if why:
        if n_calls_l != n_calls_r:
            why.insert(0, f'{n_calls_l} calls in ours, {n_calls_r} in the client')
        return done('inline-budget', '; '.join(why), first_diff)
    if n_calls_l != n_calls_r:
        return done('shape', f'the same callees, {n_calls_l} calls in ours and {n_calls_r} in the client: '
                    'a merged or split tail (return forms, workflow.md 3.1)', first_diff)
    hint = f' (mostly registers: {n_r} rows remain once registers are masked)' if n_r * 3 <= n_s else ''
    return done('shape', f'{n_s} masked rows differ; no simpler class fits' + hint, first_diff)


def show(result, ctx, window):
    L, R, SL, SR, sm, at = ctx
    print(f"class: {result['class']}: {result['reason']}")
    print(f"{result['symbol']} ({result['ours']} instructions) vs client {result['address']} ({result['client']})")
    if at is not None:
        a, b = at
        lo_a, lo_b = max(0, a - window), max(0, b - window)
        for k in range(2 * window + 4):
            ia, ib = lo_a + k, lo_b + k
            lt = f'{L[ia][0].addr:5x} {L[ia][0].raw}' if ia < len(L) else ''
            rt = f'{R[ib][0].addr:06x} {R[ib][0].raw}' if ib < len(R) else ''
            diff = not (ia < len(SL) and ib < len(SR) and SL[ia] == SR[ib])
            print(f"{'*' if diff else ' '} {lt:46.46s} | {rt}")
    print(f"masked residue: {result['residue']} rows differ (stack offsets, targets, relocations masked); "
          f"with registers also masked: {result['residue_regs']}")
    b = result.get('branch')
    if b and branch_differs(b):
        print(f"branches: {b['size']} short/near, {b['target']} to another row; code {b['bytes'][0]}/{b['bytes'][1]} bytes")
    for t in result.get('tables', ()):
        print(f"switch {t['kind']} table" + (f" 0x{t['at']:06X}" if t['at'] else '') +
              f": {t['ours']}/{t['client']} entries, " + ('equal' if not t['diff'] else f"{t['diff']} differ"))


def main():
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    ap.add_argument('obj')
    ap.add_argument('symbol')
    ap.add_argument('address')
    ap.add_argument('--n', type=int, default=0)
    ap.add_argument('--window', type=int, default=3)
    ap.add_argument('--json', action='store_true')
    a = ap.parse_args()
    result, ctx = explain(a.obj, a.symbol, a.address, a.n, a.window)
    if a.json:
        print(json.dumps(result))
    else:
        show(result, ctx, a.window)


if __name__ == '__main__':
    main()
