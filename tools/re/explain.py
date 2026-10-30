"""Classifies why a compiled function is not byte-exact: one line first, then the first mismatch.

    python explain.py file.obj SYMBOL_SUBSTRING CLIENT_ADDRESS [--n INDEX] [--window N] [--json]

Both sides are disassembled as objdiff.py does (same symbol lookup, `--n` picks among matches),
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
                   temporaries differ (workflow.md §3.1)
  shape            everything else
  identical        nothing differs once stack offsets are masked (only slot numbers: re-run objmatch)

register-only and order-only are exclusive (order-only needs the registers equal), so their order in
the list doesn't matter. Read docs/workflow.md §3.1's table for the lever per class.
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


def explain(obj_path, symbol, address, n=0, window=3):
    obj = CoffObject(obj_path)
    cands = [f for f in obj.functions() if symbol in f[0] or symbol in demangle(f[0])]
    if not cands:
        raise SystemExit('no such symbol')
    name, sec, s, e = cands[n]
    img = ClientImage()
    addr = int(address, 16)
    # the window ends at the nearest verified or proven start (symbols.json), else it runs into the next function
    starts = sorted(set(img.function_starts()[0]) | set(_names_at()))
    nxt = next((x for x in starts if x > addr), addr + (e - s) + 64)
    length = max(1, min(nxt - addr, (e - s) * 2 + 64))
    L, R = obj_side(obj, sec, s, e), client_side(img, addr, length)
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
              'client': len(R), 'residue': n_s, 'residue_regs': n_r}

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
