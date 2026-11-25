"""behaudit.py's argument-slot check (the `args` check): every aligned call's arguments, ours vs the client's.

From PS's parameter-slot audit (.work/ps/slots_all.py, run 2026-11-24-night; its resolver unchanged). For one
function paired with its client address: explain.py's rows, trybatch.py's aligned call regions and, when the /FAs
listing is given, its frame-slot map; for each aligned call the arguments compared by value:
  pushes in the region before the call (nearest first = argument 1), ecx when both sides set it in the region
  (thiscall `this`), the slot of an indirect call (`call [reg + N]`), the callee when both sides name an address.
A pushed register is resolved backwards (mov / lea / xor / movzx, through nonvolatile registers, eax after a call
= that call's return); a memory operand by its size, base (resolved) and offset; a stack operand by the listing's
frame symbol mapped onto the client's slot (without a listing: by its esp / ebp offset); a relocation by its string
or symbols.json address.
audit() returns (explain result, class, diffs, proven, slot rows, compared, calls, notes); class: none (every
argument equal), codegen (differences only where the values could not be shown different: registers, order, a value
unresolved on one side), suspect (a different constant, member, global, string, slot, vtable slot, callee or
argument count reaches a call). Positive control: galaxy_map_render 0x7AC130 passing the wrong two flags to
0x7AD090 (run 2026-11-24; test_behaudit.py).
"""
import collections
import difflib
import functools
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import explain as X  # noqa: E402
import trybatch as TB  # noqa: E402
from clientimage import ClientImage  # noqa: E402
from objmatch import symbol_addresses, thunks_of  # noqa: E402

_IMG = ClientImage()
X.ClientImage = lambda *a, **k: _IMG
X._names_at = functools.lru_cache(None)(X._names_at)
NAMES_AT = X._names_at()
KNOWN = symbol_addresses()
THUNK = thunks_of(_IMG)

REG32 = ('eax', 'ebx', 'ecx', 'edx', 'esi', 'edi', 'ebp')
VOLATILE = ('eax', 'ecx', 'edx')
SUB = {'eax': ('ax', 'al', 'ah'), 'ebx': ('bx', 'bl', 'bh'), 'ecx': ('cx', 'cl', 'ch'), 'edx': ('dx', 'dl', 'dh'),
       'esi': ('si',), 'edi': ('di',), 'ebp': ('bp',)}
REG_OF = {s: r for r, subs in SUB.items() for s in subs}
REG_OF.update({r: r for r in REG32})
MEM = re.compile(r'^(?:(byte|word|dword|qword) ptr )?(?:\w+:)?\[([^\]]*)\]$')
LIST_REF = TB.LIST_REF


def split_ops(op):
    out, depth, cur = [], 0, ''
    for ch in op:
        if ch == '[':
            depth += 1
        elif ch == ']':
            depth -= 1
        if ch == ',' and depth == 0:
            out.append(cur.strip())
            cur = ''
        else:
            cur += ch
    if cur.strip():
        out.append(cur.strip())
    return out


def num(tok):
    tok = tok.strip()
    neg = tok.startswith('-')
    tok = tok.lstrip('-').strip()
    v = int(tok, 16) if tok.startswith('0x') else int(tok)
    return -v if neg else v


class Side:
    """One side's rows with what the resolver needs."""

    def __init__(self, rows, client, ebp, slotof=None, listing=None):
        self.rows, self.client, self.ebp = rows, client, ebp
        self.slotof = slotof or {}      # ours: frame symbol -> the client's (normalised) slot
        self.listing = listing          # ours: listing instruction texts, index-aligned with rows (or None)
        # join points: a row another row branches to, or the row after an unconditional jmp / ret; the
        # resolver never walks back past one (the value could come from another path)
        targets = {ins.target[1] for ins, _ in rows if ins.target and ins.target[0] in ('local', 'addr')
                   and ins.mn != 'call'}
        self.join = [False] * len(rows)
        for k, (ins, _) in enumerate(rows):
            if ins.addr in targets or (k and rows[k - 1][0].mn in ('jmp', 'ret')):
                self.join[k] = True

    def ref_value(self, k):
        """A relocation of row k as a comparable value."""
        ins = self.rows[k][0]
        for kind, what in ins.refs:
            if kind == 'str':
                return ('str', what.rstrip(b'\0').lower())
            if kind == 'sym':
                addrs = KNOWN.get(what)
                return ('sym', frozenset(addrs) if addrs else what)
            if kind == 'addr':
                got = _IMG.cstr(what, 512) if hasattr(_IMG, 'cstr') else None
                if isinstance(got, str) and len(got) >= 1 and all(32 <= ord(c) < 127 or c in '\r\n\t' for c in got):
                    return ('str', got.encode('latin1').lower(), what)
                return ('sym', frozenset([THUNK.get(what, what)]))
        return ('G',)

    start = 0       # the current region's first row: esp offsets are taken back to the depth there

    def depth_rel(self, k):
        """Bytes pushed between the region's start and row k (None before the start)."""
        if k < self.start:
            return None
        d = 0
        for j in range(self.start, k):
            ins = self.rows[j][0]
            if ins.mn == 'ret':
                return None     # the rows after a return are reached at another depth
            if ins.mn == 'push':
                d += 4
            elif ins.mn == 'pop':
                d -= 4
            elif ins.mn in ('sub', 'add') and ins.op.startswith('esp, '):
                try:
                    n = num(ins.op.split(', ')[1])
                except ValueError:
                    return None
                d += n if ins.mn == 'sub' else -n
        return d

    def stack_value(self, k, inner):
        """A stack operand [esp + n] / [ebp - n]: the frame symbol (ours, by the listing) or the slot, an esp
        offset taken back to the region start's depth."""
        rel = self.depth_rel(k)
        if self.listing is not None and k < len(self.listing) \
                and self.listing[k].split()[0] == self.rows[k][0].mn:
            refs = LIST_REF.findall(self.listing[k])
            if refs:
                name, reg, delta = refs[0]
                name = name + '$' if name + '$' in self.slotof else name
                if reg == 'ebp':
                    return ('stack', name, 0)
                if rel is None:
                    return ('stack?', name)
                return ('stack', name, int(delta or 0) - rel)
        m = re.match(r'^(esp|ebp)(?: ([+-]) (0x[0-9a-f]+|\d+))?$', inner)
        if m:
            v = int(m.group(3), 0) if m.group(3) else 0
            v = -v if m.group(2) == '-' else v
            if m.group(1) == 'esp':
                if rel is None:
                    return ('stack?', inner)
                v -= rel
            return ('slot', m.group(1), v)
        return ('stack?', inner)

    def mem_value(self, k, size, inner, depth):
        if 'G' in self.rows[k][1] and ('[' in self.rows[k][1]):
            # a relocated address inside the brackets: a global (plus an index register maybe)
            rv = self.ref_value(k)
            if not self.client and rv[0] == 'sym' and isinstance(rv[1], frozenset):
                m = re.search(r'(0x[0-9a-f]+|\d+)$', inner)     # ours: the symbol plus the addend in the brackets
                if m and REG_OF.get(inner.split()[0]) is None:
                    rv = ('sym', frozenset(a + int(m.group(1), 0) for a in rv[1]))
            idx = [t for t in re.split(r'\s*[+*]\s*', inner) if REG_OF.get(t.strip())]
            return ('gmem', size, rv, len(idx))
        if re.match(r'^(esp|ebp)\b', inner) and (inner.startswith('esp') or self.ebp):
            # a slot stored earlier in the same block (a temporary): the value is the store's, not the variable's
            for j in range(k - 1, max(-1, k - 40), -1):
                ins = self.rows[j][0]
                if ins.mn in ('push', 'pop', 'call') or (ins.mn in ('sub', 'add') and ins.op.startswith('esp')):
                    break
                ops = split_ops(ins.op)
                if ops and ops[0].endswith('[' + inner + ']') and ins.mn in ('mov', 'fstp', 'fst', 'fistp', 'fist'):
                    return ('?', 'stored temporary')
                if self.join[j]:
                    break
            return ('frame', size, self.stack_value(k, inner))
        m = re.match(r'^(\w+)(?: \+ (\w+)(?:\*(\d))?)?(?: ([+-]) (0x[0-9a-f]+|\d+))?$', inner)
        if not m:
            return ('mem?', size, inner)
        base = m.group(1)
        off = int(m.group(5), 0) * (-1 if m.group(4) == '-' else 1) if m.group(5) else 0
        if m.group(2) and not REG_OF.get(m.group(2)):
            # [reg + 0x10] parsed with the number as the 'index'
            try:
                off = num(m.group(2))
            except ValueError:
                return ('mem?', size, inner)
            idx = None
        else:
            idx = m.group(2)
        bv = self.reg_value(k, base, depth + 1) if REG_OF.get(base) else ('?', base)
        iv = (self.reg_value(k, idx, depth + 1), m.group(3)) if idx else None
        if iv is None and bv[0] == 'addr' and len(bv) == 5 and bv[4] is None:
            return ('mem', size, bv[2], bv[3] + off, None)     # [&[x + a] + b] is [x + a + b]
        return ('mem', size, bv, off, iv)

    def operand_value(self, k, op, depth):
        op = op.strip()
        if REG_OF.get(op) == op:
            return self.reg_value(k, op, depth + 1)
        mm = MEM.match(op)
        if mm:
            return self.mem_value(k, mm.group(1) or 'dword', mm.group(2).strip(), depth)
        try:
            v = num(op) & 0xFFFFFFFF
        except ValueError:
            v = None
        if v is not None:
            if 'G' in self.rows[k][1] and self.rows[k][0].refs:
                return self.ref_value(k)     # a relocated immediate: an address (offset G), not a constant
            return ('imm', v)
        return ('?', op)

    def reg_value(self, k, reg, depth=0):
        """The value REG holds just before row k (linear, backwards)."""
        if depth > 6:
            return ('deep',)
        reg = REG_OF.get(reg, reg)
        for j in range(k - 1, max(-1, k - 80), -1):
            if j + 1 < len(self.join) and self.join[j + 1]:
                return ('join', reg)
            ins = self.rows[j][0]
            mn, ops = ins.mn, split_ops(ins.op)
            if mn == 'call':
                if reg == 'eax':
                    return ('ret', TB.call_key(ins, KNOWN))
                if reg in VOLATILE:
                    return ('clobbered',)
                continue
            if mn in ('push', 'cmp', 'test', 'jmp') or mn.startswith('j') or not ops:
                continue
            dst = ops[0]
            dreg = REG_OF.get(dst)
            if mn in ('xchg',) and reg in (REG_OF.get(ops[0]), REG_OF.get(ops[-1])):
                return ('?', 'xchg')
            if dreg != reg:
                if mn in ('rep', 'cdq') and reg in ('ecx', 'edx'):
                    return ('?', mn)
                continue
            if dst != reg:      # a partial write (al, cx)
                if mn in ('mov', 'movzx') and len(ops) == 2:
                    return ('part', dst, self.operand_value(j, ops[1], depth))
                return ('?', mn + ' ' + ins.op)
            if mn == 'mov' and len(ops) == 2:
                return self.operand_value(j, ops[1], depth)
            if mn in ('movzx', 'movsx') and len(ops) == 2:
                return ('ext', mn, self.operand_value(j, ops[1], depth))
            if mn == 'lea' and len(ops) == 2:
                v = self.operand_value(j, ops[1], depth)
                return ('addr',) + v[1:] if v[0] in ('mem', 'frame', 'gmem') else ('lea?', v)
            if mn in ('xor', 'sub') and len(ops) == 2 and REG_OF.get(ops[1]) == reg:
                return ('imm', 0)
            if mn == 'or' and len(ops) == 2 and ops[1] in ('0xffffffff', '-1'):
                return ('imm', 0xFFFFFFFF)
            if mn == 'pop':
                return ('?', 'pop')
            if mn in ('add', 'sub') and len(ops) == 2 and not REG_OF.get(ops[1]) and not MEM.match(ops[1]) \
                    and 'G' not in self.rows[j][1]:
                try:
                    imm = num(ops[1])
                except ValueError:
                    imm = None
                if imm is not None:     # pointer arithmetic: reg + K is the address [reg + K]
                    prev = self.reg_value(j, reg, depth + 1)
                    imm = imm if mn == 'add' else -imm
                    if prev[0] == 'addr' and len(prev) == 5 and prev[4] is None:
                        return ('addr', prev[1], prev[2], prev[3] + imm, None)
                    return ('addr', 'dword', prev, imm, None)
            return ('op', mn, self.operand_value(j, ops[1], depth) if len(ops) > 1 else None,
                    self.reg_value(j, reg, depth + 1))
        return ('entry', reg)


PARAMS = [({}, None)]     # the current function's ({parameter: listing offset}, client shift)
TABLE = [{}]
REGION0 = [False]
UNKNOWN = ('?', 'join', 'entry', 'clobbered', 'deep', 'mem?', 'stack?', 'lea?', 'G')


def unresolved(v):
    if not isinstance(v, tuple) or not v:
        return True
    if v[0] in UNKNOWN:
        return True
    if v[0] == 'mem' and unresolved(v[2]) and v[3] == 0:
        return True
    return False


def cmp_value(a, b, slotmap):
    """'same' | 'codegen' | a description of a proven difference."""
    if a == b:
        return 'same'
    if unresolved(a) or unresolved(b):
        return 'codegen'
    ka, kb = a[0], b[0]
    if ka == 'imm' and kb == 'imm':
        return f'constant {a[1]:#x} vs {b[1]:#x}'
    if ka in ('str', 'sym') or kb in ('str', 'sym'):
        return cmp_ref(a, b)
    if ka == 'ret' and kb == 'ret':
        x, y = a[1], b[1]
        if isinstance(x, int) and isinstance(y, int) and x != y:
            return f'return of 0x{x:06X} vs 0x{y:06X}'
        return 'codegen'
    if ka in ('mem', 'addr') and kb == ka and len(a) == 5 and len(b) == 5:
        if a[1] != b[1] and ka == 'mem':
            return f'{a[1]} vs {b[1]} read'
        if unresolved(a[2]) != unresolved(b[2]) or a[2][0] == 'op' or b[2][0] == 'op' \
                or (unresolved(a[2]) and a[2] != b[2]):
            return 'codegen'    # offsets from bases not shown equal
        if a[3] != b[3]:
            return f'{"member" if ka == "mem" else "address of member"} +{a[3]:#x} vs +{b[3]:#x}'
        c = cmp_value(a[2], b[2], slotmap)
        if c != 'same' and not c.startswith('codegen'):
            return 'base: ' + c
        if (a[4] is None) != (b[4] is None):
            return 'codegen'
        return 'codegen' if c.startswith('codegen') else ('same' if a[4] == b[4] else 'codegen')
    if ka in ('frame', 'addr') and kb == ka:
        if ka == 'frame' and a[1] != b[1]:
            return f'{a[1]} vs {b[1]} stack read'
        sa, sb = a[-1], b[-1]
        if isinstance(sa, tuple) and isinstance(sb, tuple) and sa[:1] == ('stack',) and sb[:1] == ('slot',):
            seen = slotmap.get(sa[1])
            norm = sb[2] - sa[2]
            params, shift = PARAMS[0]
            if REGION0[0] and shift is not None:
                shift = 0     # depths counted from the entry on both sides
            if shift is not None and sa[1] in params:
                if norm == params[sa[1]] + shift:
                    return 'same'
                other = [n for n, o in params.items() if o + shift == norm]
                if other:
                    return f'parameter {sa[1]} vs the client\'s {other[0]} (shift {shift:+d})'
                return 'codegen'
            if shift is not None and sa[1] not in params and not sa[1].startswith('$T') and slotmap.get(sa[1]) and TABLE[0].get(sa[1], 0) < 0:
                other = [n for n, o in params.items() if o + shift == norm]
                if other:
                    return f'local {sa[1]} vs the client\'s parameter {other[0]} (shift {shift:+d})'
            if seen and norm in seen:
                return 'same'
            if ka == 'addr':
                return 'codegen'    # a local's address (an out parameter): its slot is the frame layout's
            others = sorted(n for n, c in slotmap.items() if n != sa[1] and norm in c and len(c) == 1
                            and c[norm] >= 2)
            if seen and len(seen) == 1 and norm not in seen and seen.most_common(1)[0][1] >= 2 and others:
                return (f'frame slot {sa[1]} (client {seen.most_common(1)[0][0]:+d}) vs {norm:+d}'
                        f' = ours {others[0]}')
        return 'codegen'
    if ka == 'gmem' and kb == 'gmem':
        if a[1] != b[1]:
            return f'{a[1]} vs {b[1]} global read'
        c = cmp_ref(a[2], b[2])
        return c
    if ka in ('ext', 'part') and kb == ka:
        return cmp_value(a[-1], b[-1], slotmap)
    if ka == 'op' or kb == 'op':
        return 'codegen'
    if {ka, kb} <= {'imm', 'mem', 'gmem', 'frame', 'addr', 'ret', 'ext', 'part'} and ka != kb:
        if {ka, kb} == {'ext', 'mem'} or {ka, kb} == {'part', 'mem'} or {ka, kb} == {'ext', 'part'}:
            return 'codegen'
        if 'ret' in (ka, kb):
            return 'codegen'
        if 'frame' in (ka, kb) or (ka == 'addr' and len(a) == 3) or (kb == 'addr' and len(b) == 3):
            return 'codegen'    # a value through a frame slot or a local's address: a temporary, not provable
        return f'{show(a)} vs {show(b)}'
    return 'codegen'


def cmp_ref(a, b):
    if a[0] == 'str' and b[0] == 'str':
        if X.same_string(a[1], b[1]):
            return 'same'
        if all(re.search(rb'[.](cpp|h|inl)$', x[1]) for x in (a, b)):
            return 'codegen (__FILE__ of an inlined header assert: D283)'
        return f'string {a[1][:24]!r} vs {b[1][:24]!r}'
    if a[0] == 'sym' and b[0] == 'sym':
        x, y = a[1], b[1]
        if isinstance(x, frozenset) and isinstance(y, frozenset):
            return 'same' if x & y else f'global {fmt_addrs(x)} vs {fmt_addrs(y)}'
        if isinstance(x, str) and isinstance(y, frozenset):
            if any(x in NAMES_AT.get(v, ()) for v in y):
                return 'same'
        return 'codegen'
    if 'str' in (a[0], b[0]) and 'sym' in (a[0], b[0]):
        s = a if a[0] == 'sym' else b
        if isinstance(s[1], str):
            return 'codegen'
        return f'{show(a)} vs {show(b)}'
    return 'codegen'


def fmt_addrs(s):
    return '/'.join(f'0x{x:06X}' for x in sorted(s)[:2])


def show(v, d=0):
    if not isinstance(v, tuple):
        return str(v)
    k = v[0]
    if k == 'imm':
        return f'{v[1]:#x}'
    if k == 'str':
        return repr(v[1][:20])
    if k == 'sym':
        return fmt_addrs(v[1]) if isinstance(v[1], frozenset) else X.demangle(v[1])[:30]
    if k == 'mem' and d < 3:
        return f'[{show(v[2], d + 1)}+{v[3]:#x}]'
    if k == 'addr' and len(v) == 5 and d < 3:
        return f'&[{show(v[2], d + 1)}+{v[3]:#x}]'
    if k in ('frame', 'addr') and d < 3:
        return ('&' if k == 'addr' else '') + 'frame:' + show(v[-1], d + 1)
    if k == 'stack':
        return v[1]
    if k == 'slot':
        return f'{v[1]}{v[2]:+d}'
    if k == 'ret':
        return f'ret({v[1]:#x})' if isinstance(v[1], int) else f'ret({str(v[1])[:20]})'
    if k == 'gmem':
        return f'{v[1]}[{show(v[2], d + 1)}]'
    if k in ('ext', 'part') and d < 3:
        return f'{k}({show(v[-1], d + 1)})'
    if k == 'entry':
        return f'entry:{v[1]}'
    return k


def prologue_depth(rows):
    """Bytes the prologue puts below the return address, or None: up to the first call or branch, the SEH
    frame's pushes (to the fs:[0] store), sub esp, and pushes of registers not written before (the saves)."""
    d, written = 0, set()
    eh_end = next((k for k, (ins, _) in enumerate(rows[:16])
                   if ins.mn == 'mov' and ins.op.replace(' ', '').startswith('dwordptrfs:')), -1)
    for k, (ins, _) in enumerate(rows[:30]):
        if ins.mn == 'call' or ins.mn.startswith('j') or ins.mn == 'ret':
            break
        ops = split_ops(ins.op)
        if ins.mn == 'push':
            if k <= eh_end:
                d += 4
            elif REG_OF.get(ops[0]) == ops[0] and ops[0] not in written:
                d += 4
        elif ins.mn == 'sub' and ins.op.startswith('esp, '):
            try:
                d += num(ins.op.split(', ')[1])
            except ValueError:
                return None
        elif ins.mn == 'add' and ins.op.startswith('esp, '):
            return None
        if ops and REG_OF.get(ops[0]) and ins.mn not in ('push', 'cmp', 'test'):
            written.add(REG_OF[ops[0]])
    return d


def prologue_end(rows):
    """Rows before this index are the prologue: the SEH frame and the register saves."""
    end = 0
    for k, (ins, t) in enumerate(rows[:16]):
        if 'fs:' in ins.raw or ins.raw in ('mov ebp, esp',) or re.match(r'^sub esp, ', ins.raw):
            end = k + 1
    k = end
    while k < len(rows) and rows[k][0].mn == 'push' and REG_OF.get(rows[k][0].op) == rows[k][0].op:
        k += 1
    return k


def pushes(side, i0, i1, start):
    """[(row index, value)] of the pushes in rows[i0:i1] (the call at i1-1), nearest the call first."""
    out = []
    lo = max(i0, start)
    # only the call's own basic block: pushes before a join point may belong to another path
    lo = max([lo] + [x for x in range(lo + 1, i1) if side.join[x]])
    for k in range(lo, i1 - 1):
        ins = side.rows[k][0]
        if ins.mn != 'push':
            continue
        op = ins.op.strip()
        v = side.operand_value(k, op, 0)
        if v[0] == 'entry' and v[1] != 'ecx':
            continue        # a register save, not an argument
        if v[0] == 'join' and v[1] in ('ebx', 'esi', 'edi', 'ebp') and not any(
                (split_ops(i.op) or [''])[0] in (v[1],) + SUB.get(v[1], ()) and i.mn not in ('push', 'cmp', 'test')
                for i, _ in side.rows[:k]):
            continue        # a late register save (after an early return), not an argument
        if 'G' in side.rows[k][1] and v[0] in ('imm', '?'):
            v = side.ref_value(k)
        out.append((k, v))
    return out[::-1]


THISCALL = re.compile(r'@@[A-Z][A-Z]E')


def thiscall(ins):
    """Ours calls a __thiscall (mangled ...@@QAE / UAE ...) or an indirect (virtual) function: ecx is `this`."""
    t = ins.target
    if t is None:
        return True
    return t[0] == 'sym' and bool(THISCALL.search(t[1]))


def ecx_in(side, i0, i1):
    for k in range(i1 - 2, i0 - 1, -1):
        ins = side.rows[k][0]
        ops = split_ops(ins.op)
        if ops and REG_OF.get(ops[0]) == 'ecx' and ins.mn not in ('push', 'cmp', 'test'):
            return side.reg_value(k + 1, 'ecx')
        if ins.mn == 'call' or side.join[k]:
            return None     # set in another block (or not at all): not shown
    return None


def ind_key(ins):
    """An indirect call's key: its masked operand (a vtable slot keeps its offset)."""
    return 'indirect ' + X.mask_regs(re.sub(r'\[esp[^\]]*\]', '[esp+S]', ins.op))


def norm_keys(L, R):
    """Call keys both sides can share: the client address, ours through symbols.json or the client's names there;
    an indirect call is 'indirect'."""
    kl = []
    for ins, _ in L:
        if ins.mn != 'call':
            continue
        k = TB.call_key(ins, KNOWN)
        kl.append(ind_key(ins) if k is None else k)
    names = {k for k in kl if isinstance(k, str) and not k.startswith('indirect')}
    kr = []
    for ins, _ in R:
        if ins.mn != 'call':
            continue
        k = TB.call_key(ins, KNOWN)
        if k is None:
            kr.append(ind_key(ins))
            continue
        hit = names & NAMES_AT.get(k, set())
        kr.append(next(iter(hit)) if hit else k)
    return kl, kr


def all_args(side, keys, start):
    """{call key: {argument index: set of shown values}} over every call of one side."""
    out = collections.defaultdict(lambda: collections.defaultdict(set))
    calls = [k for k, (ins, _) in enumerate(side.rows) if ins.mn == 'call']
    prev = 0
    for c, key in zip(calls, keys):
        side.start = prev
        for n, (_, v) in enumerate(pushes(side, prev, c + 1, start)):
            out[key][n].add(show(v))
        prev = c + 1
    return out


def align_regions(L, R):
    """trybatch.regions, aligned on normalised call keys (difflib): ([(i0, i1, j0, j1, label)], every call paired)."""
    cl = [i for i, (ins, _) in enumerate(L) if ins.mn == 'call']
    cr = [j for j, (ins, _) in enumerate(R) if ins.mn == 'call']
    kl, kr = norm_keys(L, R)
    sm = difflib.SequenceMatcher(None, kl, kr, autojunk=False)
    pairs = [(cl[b.a + k], cr[b.b + k]) for b in sm.get_matching_blocks() for k in range(b.size)]
    if len(cl) == len(cr) and len(pairs) < len(cl):
        # same count: pair the unequal keys by position inside each replace block of equal size
        pairs = []
        for tag, a1, a2, b1, b2 in sm.get_opcodes():
            if tag == 'equal' or (tag == 'replace' and a2 - a1 == b2 - b1 and all(
                    isinstance(kl[a1 + m], str) or isinstance(kr[b1 + m], str) for m in range(a2 - a1))):
                pairs += [(cl[a1 + m], cr[b1 + m]) for m in range(a2 - a1)]
    out, i0, j0 = [], 0, 0
    for i, j in pairs:
        out.append((i0, i + 1, j0, j + 1, TB.call_label(R[j][0]) if R[j][0].target else TB.call_label(L[i][0])))
        i0, j0 = i + 1, j + 1
    out.append((i0, len(L), j0, len(R), 'end'))
    return out, len(pairs) == len(cl) == len(cr)


def audit(obj_path, symbol, addr, listing_text):
    result, (L, R, SL, SR, sm, at) = X.explain(obj_path, symbol, f'0x{addr:X}')
    eL, eR = X.ebp_frame(L), X.ebp_frame(R)
    slot_rows, table, ins = [], {}, None
    slotmap = {}
    PARAMS[0] = ({}, None)
    if listing_text is not None:
        try:
            table, ins = TB.listing_body(listing_text, symbol)
            rows = TB.slot_map(table, ins, L, R, sm)
            slot_rows = TB.slot_lines(rows)
            for off, name, c in rows:
                if c:
                    slotmap[name] = c
            # parameters sit at a fixed distance from the client's: 0 in two ebp frames, else the difference
            # of the two prologues' depths (pushes and sub esp)
            params = {n: o for n, o in table.items() if o > 0 and re.fullmatch(r'_\w+?\$', n) and not re.search(r'\$\d', n)}
            if eL and eR:
                shift = 0
            elif not eL and not eR:
                dl, dr = prologue_depth(L), prologue_depth(R)
                shift = None if dl is None or dr is None else dr - dl
            else:
                shift = None
            PARAMS[0] = (params, shift); TABLE[0] = table
            # the listing ends with the unwind funclets (text$x): index-aligned when the first rows agree
            bad = sum(1 for k in range(min(len(ins), len(L))) if ins[k].split()[0] != L[k][0].mn)
            if len(ins) < len(L) or bad > len(L) // 50:
                ins = None
            else:
                ins = ins[:len(L)]
        except ValueError:
            ins = None
    # listing frame refs give symbol names: normalise to the symbol's own name (no '$')
    ours = Side(L, False, eL, slotmap, ins)
    theirs = Side(R, True, eR)
    sL, sR = prologue_end(L), prologue_end(R)
    diffs, compared, notes = [], 0, []
    regs, aligned = align_regions(L, R)
    vals_l, vals_r, keys = collections.defaultdict(list), collections.defaultdict(list), {}
    for k, (i0, i1, j0, j1, label) in enumerate(regs):
        if label == 'end':
            continue
        cl, cr = L[i1 - 1][0], R[j1 - 1][0]
        inner_l = sum(1 for x in range(i0, i1 - 1) if L[x][0].mn == 'call')
        inner_r = sum(1 for x in range(j0, j1 - 1) if R[x][0].mn == 'call')
        where = f'call{k} @0x{cr.addr:06X} {label[:40]}'
        ours.start, theirs.start = i0, j0
        REGION0[0] = (i0 == 0 and j0 == 0)
        if cl.target is None and cr.target is None:
            ml, mr = MEM.match(cl.op), MEM.match(cr.op)
            if ml and mr:
                vl = ours.mem_value(i1 - 1, 'dword', ml.group(2).strip(), 0)
                vr = theirs.mem_value(j1 - 1, 'dword', mr.group(2).strip(), 0)
                c = cmp_value(vl, vr, slotmap)
                compared += 1
                if c != 'same':
                    diffs.append((where, 'target', show(vl), show(vr), c))
        if inner_l or inner_r:
            notes.append(f'{where}: unaligned calls inside')
            continue
        ours.start, theirs.start = i0, j0
        REGION0[0] = (i0 == 0 and j0 == 0)
        pl, pr = pushes(ours, i0, i1, sL), pushes(theirs, j0, j1, sR)
        if len(pl) != len(pr):
            branchy = any(ours.join[x] for x in range(i0 + 1, i1)) or any(theirs.join[x] for x in range(j0 + 1, j1))
            diffs.append((where, 'count', str(len(pl)), str(len(pr)),
                          ('codegen (count across branches) ' if branchy else '')
                          + f'{len(pl)} pushes vs {len(pr)}: ' + ' '.join(show(v) for _, v in pl)[:60] + ' | '
                          + ' '.join(show(v) for _, v in pr)[:60]))
        for n, ((_, vl), (_, vr)) in enumerate(zip(pl, pr)):
            compared += 1
            vals_l[(label, n)].append(show(vl))
            vals_r[(label, n)].append(show(vr))
            c = cmp_value(vl, vr, slotmap)
            if c != 'same':
                diffs.append((where, f'arg{n + 1}', show(vl), show(vr), c))
                keys[len(diffs) - 1] = (label, n)
        el, er = ecx_in(ours, i0, i1), ecx_in(theirs, j0, j1)
        if el is not None and er is not None and thiscall(cl):
            compared += 1
            c = cmp_value(el, er, slotmap)
            if c != 'same':
                diffs.append((where, 'ecx', show(el), show(er), c))
    # each side's value also reaches the same callee's argument elsewhere on the other side: reordered blocks
    if keys:
        kl, kr = norm_keys(L, R)
        al, ar = all_args(ours, kl, sL), all_args(theirs, kr, sR)
        pushed_l = {show(ours.ref_value(x)) for x, (ins, t) in enumerate(L) if ins.mn == 'push' and 'G' in t}
        pushed_r = {show(theirs.ref_value(x)) for x, (ins, t) in enumerate(R) if ins.mn == 'push' and 'G' in t}
        at = {ins.addr: m for m, ins in enumerate(i for i, _ in R if i.mn == 'call')}
        for k, (label, n) in keys.items():
            d = diffs[k]
            m = re.search(r'@0x([0-9A-F]+)', d[0])
            if d[4].startswith('codegen') or not m or int(m.group(1), 16) not in at:
                continue
            key = kr[at[int(m.group(1), 16)]]
            sl, sr = al.get(key, {}).get(n, set()), ar.get(key, {}).get(n, set())
            if d[2] in sr and d[3] in sl:
                diffs[k] = d[:4] + ('codegen (reordered: both values reach this argument on both sides)',)
            elif d[4].startswith(('string', 'global')) and d[2] in pushed_r and d[3] in pushed_l:
                # a string / global pushed on both sides somewhere (the client shares a call between cases)
                diffs[k] = d[:4] + ('codegen (reordered: both strings / globals are pushed on both sides)',)
    proven = [d for d in diffs if not d[4].startswith('codegen') and d[1] != 'count']
    counts = [d for d in diffs if d[1] == 'count' and not d[4].startswith('codegen')]
    # a push count that differs is a missing / extra argument only when every call aligns and the totals differ
    if counts:
        tl = sum(len(pushes(ours, a, b, sL)) for a, b, c, d, lab in regs if lab != 'end')
        tr = sum(len(pushes(theirs, c, d, sR)) for a, b, c, d, lab in regs if lab != 'end')
        if tl != tr and aligned:
            proven += counts
        else:
            why = 'totals agree' if tl == tr else 'calls not all aligned'
            diffs = [d[:4] + (f'codegen (count: {why}) ' + d[4],) if d[1] == 'count' and not d[4].startswith('codegen') else d for d in diffs]
    if not aligned:
        notes.append('calls not all aligned')
    cls = 'suspect' if proven else ('codegen' if diffs else 'none')
    return result, cls, diffs, proven, slot_rows, compared, len(regs) - 1, notes
