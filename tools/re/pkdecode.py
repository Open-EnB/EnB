"""Decode the simple packet classes from the client (docs/kernel-map.md section 6, step 5).

    python tools/re/pkdecode.py [--report]      analyse; writes .work/pkdecode/decoded.pkl
    python tools/re/pkemit.py [--write] ...     drafts the files from it (app/packets/pk*.cpp / .h)

A class is drafted when every part decodes:
  * the constructor from the raw message: PacketClass's inline constructor, zero stores to members
    (the initialiser list) and RawPacketClass::Get calls on members;
  * Build: `new RawPacketClass(opcode, Get_Data_Size())` and Add calls on members;
  * Get_Data_Size: a constant;
  * Print: sprintf(buffer, fmt, ...) or _snprintf(buffer, len, fmt, ...) of members and constants;
  * the destructor trivial; slots +0x18..+0x24 the base's or one-instruction bodies; slots from +0x28
    clientstub.cpp's stubs only;
  * other constructors (functions storing the vtable) from parameters and constants, else left out.
Files: the client's objects end with the guarded static every <string> includer has (0xC416FC).
"""
import argparse
import bisect
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.normpath(os.path.join(HERE, '..', '..'))
WORK = os.path.join(ROOT, '.work', 'pkdecode')
sys.path.insert(0, HERE)
from clientimage import ClientImage  # noqa: E402

ci = ClientImage()
starts, th = ci.function_starts()
PKDIR = os.path.join(ROOT, 'app', 'packets')

GET = {0x86ED20: ('raw', None), 0x86EDF0: ('ref', 'int'), 0x86EE20: ('ref', 'short'), 0x86EE50: ('ref', 'bool'),
       0x86EE90: ('ref', 'float'), 0x86EDB0: ('ref', '__int64')}
ADD = {0x86E870: ('raw', None), 0x86E920: ('ref', 'int'), 0x86E960: ('ref', 'short'), 0x86E9A0: ('ref', 'bool'),
       0x86E9E0: ('ref', 'float'), 0x86E8D0: ('ref', '__int64')}
NEW, LBCTOR, SPRINTF, SNPRINTF = 0x4E9CB0, 0x4AAB60, 0xA266DA, 0xA26513
BASE_VT, SERIAL = 0xAEDE88, 0xBF4128
CLIENTSTUB = (0x867AA0, 0x8696C0)
BASE_SLOTS = {0x14: 0x86F010, 0x18: 0x52B2D0, 0x1C: 0x52B2F0, 0x20: 0x52B310, 0x24: 0x52B330}
REGS = ('eax', 'ebx', 'ecx', 'edx', 'esi', 'edi', 'ebp')
LOW8 = {'al': 'eax', 'bl': 'ebx', 'cl': 'ecx', 'dl': 'edx'}
STATIC_END = bytes([0x8A, 0x0D, 0xFC, 0x16, 0xC4, 0x00])     # mov cl, byte ptr [0xC416FC]


class Fail(Exception):
    pass


def resolve(t):
    return th.get(t, t)


def fsize(f):
    i = bisect.bisect_left(starts, f)
    return starts[i + 1] - f


def code(f):
    out = []
    for ins in ci.disasm(f, fsize(f) + 0x10):
        out.append(ins)
        if ins.mnemonic == 'ret':
            break
    return out


def slot(vt, off):
    return resolve(ci.u32(vt + off))


def isimm(s):
    s = s.strip()
    return bool(re.match(r'^-?(0x[0-9a-f]+|\d+)$', s))


def imm(s):
    s = s.strip()
    neg = s.startswith('-')
    s = s.lstrip('-')
    v = int(s, 16) if s.startswith('0x') else int(s)
    return -v if neg else v


MEM = re.compile(r'^(?:(byte|word|dword|qword) ptr )?\[(\w+)(?: ([+-]) (0x[0-9a-f]+|\d+))?\]$')


def mem(op):
    m = MEM.match(op.strip())
    if not m:
        return None
    off = imm(m.group(4)) if m.group(4) else 0
    if m.group(3) == '-':
        off = -off
    return m.group(1), m.group(2), off


def ops(ins):
    return [o.strip() for o in ins.op_str.split(',')] if ins.op_str else []


def is_trivial_dtor(f):
    c = code(f)
    return len(c) == 2 and c[0].mnemonic == 'mov' and ops(c[0]) == ['dword ptr [ecx]', hex(BASE_VT)] and \
        c[1].mnemonic == 'ret'


def is_trivial_dtor_call(sdd):
    """The scalar deleting destructor calls a destructor that only stores PacketClass's vtable."""
    for ins in code(sdd):
        if ins.mnemonic == 'call' and isimm(ins.op_str):
            return is_trivial_dtor(resolve(imm(ins.op_str)))
    return False


def slot_body(f):
    """One-instruction bodies of the base's slots: a C++ return expression or ('member', off, width)."""
    c = code(f)
    if len(c) != 2 or c[1].mnemonic != 'ret' or c[1].op_str:
        raise Fail('slot body')
    i = c[0]
    o = ops(i)
    if i.mnemonic == 'mov' and o[0] == 'al' and isimm(o[1]):
        return 'true' if imm(o[1]) == 1 else str(imm(o[1]))
    if i.mnemonic == 'xor' and o == ['al', 'al']:
        return 'false'
    if i.mnemonic == 'xor' and o == ['eax', 'eax']:
        return '0'
    if i.mnemonic == 'or' and o == ['eax', '0xffffffff']:
        return '-1'
    if i.mnemonic == 'mov' and o[0] == 'eax':
        m = mem(o[1])
        if m and m[1] == 'ecx':
            return ('member', m[2], m[0])
        if isimm(o[1]):
            v = imm(o[1])
            return str(v if v < 0x80000000 else v - 0x100000000)
    raise Fail('slot body: %s %s' % (i.mnemonic, i.op_str))


def decode_ctor(f, vt, raw_arg):
    """A constructor: the member actions [(kind, off, ...)], its argument count, an EH frame."""
    c = code(f)
    regs = {'ecx': ('this', 0)}
    esp = 0             # bytes pushed since entry
    pushes = []
    acts = []           # ('zero'|'imm', off, width, value) | ('param', off, width, argno) | ('get', off, kind, n)
    seen_vt = False
    serial = 0
    eh = False
    for ins in c:
        m, o = ins.mnemonic, ops(ins)
        if m == 'push':
            v = o[0]
            pushes.append(regs.get(v) if v in REGS else ('imm', imm(v)) if isimm(v) else None)
            esp += 4
            continue
        if m == 'pop':
            esp -= 4
            if pushes:
                pushes.pop()
            regs[o[0]] = None
            continue
        if m == 'ret':
            break
        if m == 'mov' and o[1] == 'dword ptr fs:[0]':
            eh = True
            regs[o[0]] = None
            continue
        if m == 'mov' and o[0].startswith('dword ptr fs:'):
            continue
        if m in ('add', 'sub') and o[0] == 'esp':
            esp += -imm(o[1]) if m == 'add' else imm(o[1])
            continue
        if m == 'xor' and o[0] == o[1] and o[0] in REGS:
            regs[o[0]] = ('imm', 0)
            continue
        if m == 'inc' and o[0] in REGS:
            regs[o[0]] = ('serial+1',) if regs.get(o[0]) == ('serial',) else None
            continue
        if m == 'lea' and o[0] in REGS:
            mm = mem(o[1])
            b = regs.get(mm[1]) if mm else None
            if b and b[0] == 'this':
                regs[o[0]] = ('this', b[1] + mm[2])
                continue
            raise Fail('ctor: lea')
        if m == 'mov':
            dst, src = o
            md, ms = mem(dst), mem(src)
            if dst in REGS:
                if src in REGS:
                    regs[dst] = regs.get(src)
                elif isimm(src):
                    regs[dst] = ('imm', imm(src))
                elif ms and ms[1] == 'esp':
                    a = ms[2] - esp
                    regs[dst] = ('arg', (a - 4) // 4) if a >= 4 and a % 4 == 0 else ('local', a)
                elif src == 'dword ptr [%s]' % hex(SERIAL):
                    regs[dst] = ('serial',)
                    serial += 1
                else:
                    raise Fail('ctor: mov')
                continue
            if md:
                width = {'byte': 1, 'word': 2, 'dword': 4}.get(md[0], 4)
                if md[1] == 'esp' or dst == 'dword ptr [%s]' % hex(SERIAL):
                    continue
                base = regs.get(md[1])
                if not base or base[0] != 'this':
                    raise Fail('ctor: store')
                off = base[1] + md[2]
                val = ('imm', imm(src)) if isimm(src) else regs.get(src)
                if off == 0:
                    if val == ('imm', vt):
                        seen_vt = True
                        continue
                    raise Fail('ctor: vtable')
                if off == 4 and val == ('imm', 0):
                    continue
                if off == 8 and val == ('serial',):
                    continue
                if val is None:
                    raise Fail('ctor: store of unknown')
                if val[0] == 'imm':
                    acts.append(('zero' if val[1] == 0 else 'imm', off, width, val[1], seen_vt))
                elif val[0] == 'arg':
                    acts.append(('param', off, width, val[1], seen_vt))
                else:
                    raise Fail('ctor: store of %s' % val[0])
                continue
            raise Fail('ctor: mov')
        if m == 'call':
            t = resolve(imm(o[0])) if isimm(o[0]) else None
            if t in GET and raw_arg:
                kind, typ = GET[t]
                if kind == 'raw':
                    p, n = pushes[-1], pushes[-2]
                    if not p or p[0] != 'this' or not n or n[0] != 'imm':
                        raise Fail('ctor: Get args')
                    acts.append(('get', p[1], 'raw', n[1]))
                    pushes = pushes[:-2]
                    esp -= 8
                else:
                    p = pushes[-1]
                    if not p or p[0] != 'this':
                        raise Fail('ctor: Get args')
                    acts.append(('get', p[1], typ, None))
                    pushes = pushes[:-1]
                    esp -= 4
                for r in ('eax', 'ecx', 'edx'):
                    regs[r] = None
                continue
            raise Fail('ctor: call')
        if m == 'nop':
            continue
        raise Fail('ctor: %s' % m)
    if not seen_vt or serial != 2:
        raise Fail('ctor: not a packet constructor')
    nargs = imm(c[-1].op_str) // 4 if c[-1].op_str else 0
    return acts, nargs, eh


def decode_build(f):
    c = code(f)
    regs = {'ecx': ('this', 0)}
    pushes = []
    opcode = None
    adds = []
    vcalls = 0
    for ins in c:
        m, o = ins.mnemonic, ops(ins)
        if m == 'push':
            v = o[0]
            pushes.append(regs.get(v) if v in REGS else ('imm', imm(v)) if isimm(v) else None)
            continue
        if m == 'pop':
            if pushes:
                pushes.pop()
            regs[o[0]] = None
            continue
        if m == 'mov' and len(o) == 2:
            dst, src = o
            md = mem(dst)
            if dst in REGS and src in REGS:
                regs[dst] = regs.get(src)
                continue
            if md and md[0] == 'word' and md[2] == 2 and isimm(src):
                opcode = imm(src)
                continue
            if dst in REGS:
                regs[dst] = None
            continue
        if m == 'lea' and o[0] in REGS:
            mm = mem(o[1])
            b = regs.get(mm[1]) if mm else None
            regs[o[0]] = ('this', b[1] + mm[2]) if b and b[0] == 'this' else None
            continue
        if m in ('add', 'sub') and o[0] in REGS and isimm(o[1]):
            b = regs.get(o[0])
            d = imm(o[1]) if m == 'add' else -imm(o[1])
            regs[o[0]] = ('this', b[1] + d) if b and b[0] == 'this' else None
            continue
        if m == 'call':
            if not isimm(o[0]):
                if o[0] in ('dword ptr [eax + 0xc]', 'dword ptr [edx + 0xc]', 'dword ptr [ecx + 0xc]'):
                    vcalls += 1
                    pushes = []
                    regs['eax'] = None
                    continue
                raise Fail('Build: call')
            t = resolve(imm(o[0]))
            if t == NEW:
                if pushes[-1:] != [('imm', 0x22C)]:
                    raise Fail('Build: new')
                pushes = []
                regs['eax'] = None
                continue
            if t == LBCTOR:
                pushes = []
                continue
            if t in ADD:
                kind, typ = ADD[t]
                if kind == 'raw':
                    p, n = pushes[-1], pushes[-2]
                    if not p or p[0] != 'this' or not n or n[0] != 'imm':
                        raise Fail('Build: Add args')
                    adds.append((p[1], 'raw', n[1]))
                    pushes = pushes[:-2]
                else:
                    p = pushes[-1]
                    if not p or p[0] != 'this':
                        raise Fail('Build: Add args')
                    adds.append((p[1], typ, None))
                    pushes = pushes[:-1]
                for r in ('eax', 'ecx', 'edx'):
                    regs[r] = None
                continue
            raise Fail('Build: call')
        if m == 'ret':
            break
        if m in ('xor', 'test', 'cmp', 'je', 'jne', 'jmp', 'jz', 'jnz', 'nop'):
            if m == 'xor' and o[0] in REGS:
                regs[o[0]] = None
            continue
    if opcode is None or vcalls != 1:
        raise Fail('Build: shape')
    return opcode, adds


def decode_size(f):
    c = code(f)
    if len(c) == 2 and c[1].mnemonic == 'ret' and not c[1].op_str:
        o = ops(c[0])
        if c[0].mnemonic == 'mov' and o[0] == 'eax' and isimm(o[1]):
            return imm(o[1])
        if c[0].mnemonic == 'xor' and o == ['eax', 'eax']:
            return 0
    raise Fail('Get_Data_Size')


SPEC = re.compile(r'%[-+ #0]*\d*(?:\.\d+)?(?:I64|hh|h|ll|l)?[diouxXfeEgGcsp]')


def decode_print(f):
    """sprintf's / _snprintf's: the function, the format, the arguments after it."""
    c = code(f)
    regs = {'ecx': ('this', 0)}
    esp = 0
    stack = []
    fpu = []
    fn = None
    for ins in c:
        m, o = ins.mnemonic, ops(ins)
        if m == 'push':
            v = o[0]
            if v in REGS:
                stack.append(regs.get(v))
            elif isimm(v):
                stack.append(('imm', imm(v)))
            else:
                mm = mem(v)
                b = regs.get(mm[1]) if mm else None
                if b and b[0] == 'this' and mm[0] == 'dword':
                    stack.append(('member', b[1] + mm[2], 'dword'))
                elif mm and mm[1] == 'esp':
                    stack.append(('arg', (mm[2] - esp - 4) // 4))
                else:
                    raise Fail('Print: push')
            esp += 4
            continue
        if m == 'sub' and o[0] == 'esp':
            n = imm(o[1])
            if n % 8:
                raise Fail('Print: sub esp')
            for _ in range(n // 8):
                stack.append(('pending',))
            esp += n
            continue
        if m == 'fld':
            mm = mem(o[0])
            b = regs.get(mm[1]) if mm else None
            if b and b[0] == 'this' and mm[0] in ('dword', 'qword'):
                fpu.append(('member', b[1] + mm[2], 'float' if mm[0] == 'dword' else 'double'))
                continue
            raise Fail('Print: fld')
        if m == 'fstp' and o[0] == 'qword ptr [esp]':
            k = max(i for i, s in enumerate(stack) if s == ('pending',))
            stack[k] = fpu.pop()
            continue
        if m == 'xor' and o[0] == o[1] and o[0] in REGS:
            regs[o[0]] = ('imm', 0)
            continue
        if m == 'mov' and o[0] in LOW8 and mem(o[1]) and mem(o[1])[0] == 'byte':
            # `xor edx, edx; mov dl, byte ptr [ecx + n]`: a bool (VC6's bool to int)
            mm = mem(o[1])
            b = regs.get(mm[1])
            full = LOW8[o[0]]
            if b and b[0] == 'this' and regs.get(full) == ('imm', 0):
                regs[full] = ('member', b[1] + mm[2], 'bool')
                continue
            raise Fail('Print: mov byte')
        if m in ('mov', 'movsx', 'movzx') and o[0] in REGS:
            src = o[1]
            mm = mem(src)
            if src in REGS:
                regs[o[0]] = regs.get(src)
                continue
            if mm and mm[1] == 'esp':
                regs[o[0]] = ('arg', (mm[2] - esp - 4) // 4)
                continue
            b = regs.get(mm[1]) if mm else None
            if b and b[0] == 'this':
                regs[o[0]] = ('member', b[1] + mm[2], (m, mm[0]))
                continue
            if isimm(src):
                regs[o[0]] = ('imm', imm(src))
                continue
            raise Fail('Print: mov')
        if m == 'lea' and o[0] in REGS:
            mm = mem(o[1])
            b = regs.get(mm[1]) if mm else None
            if b and b[0] == 'this':
                regs[o[0]] = ('addr', b[1] + mm[2])
                continue
            raise Fail('Print: lea')
        if m == 'call':
            t = resolve(imm(o[0])) if isimm(o[0]) else None
            if t in (SPRINTF, SNPRINTF):
                fn = t
                continue
            raise Fail('Print: call')
        if m == 'add' and o[0] == 'esp':
            continue
        if m == 'ret':
            if ins.op_str != '8':
                raise Fail('Print: ret')
            break
        raise Fail('Print: %s' % m)
    if fn is None or any(s is None or s == ('pending',) for s in stack):
        raise Fail('Print: arguments')
    args = list(reversed(stack))
    n0 = 1 if fn == SPRINTF else 2
    if args[:n0] != [('arg', i) for i in range(n0)]:
        raise Fail('Print: buffer')
    args = args[n0:]
    if not args or args[0][0] != 'imm':
        raise Fail('Print: format')
    fmt = ci.cstr(args[0][1])
    if fmt is None:
        raise Fail('Print: format string')
    rest = []
    for a in args[1:]:
        if a[0] == 'imm':
            s = ci.cstr(a[1]) if 0xA00000 <= a[1] < 0xC00000 else None
            if s is not None and all(32 <= ord(ch) < 127 for ch in s):
                rest.append(('str', s))
            else:
                rest.append(('int', a[1]))
        elif a[0] in ('member', 'addr'):
            rest.append(a)
        else:
            raise Fail('Print: argument')
    if len(SPEC.findall(fmt)) != len(rest):
        raise Fail('Print: specifiers')
    return fn, fmt, rest


def field_names(fmt):
    """The names Print gives its arguments: 'GameID=%d', 'HSV=%f %f %f' (HSV three times)."""
    names = []
    last = None
    pos = 0
    for m in SPEC.finditer(fmt):
        before = fmt[pos:m.start()]
        w = re.search(r'([A-Za-z_][A-Za-z_0-9]*)\s*[=:]\s*$', before)
        if w:
            last = w.group(1)
        elif not re.match(r'^[\s,]*$', before):
            last = None
        names.append(last)
        pos = m.end()
    return names


TSIZE = {'int': 4, 'short': 2, 'bool': 1, 'float': 4, '__int64': 8}


def analyse(op, name, vt, ctor, size):
    cls = dict(op=op, name=name, vt=vt, ctor=ctor, size=size)
    if not is_trivial_dtor_call(slot(vt, 0)):
        raise Fail('destructor')
    acts, nargs, eh = decode_ctor(ctor, vt, True)
    if nargs != 1:
        raise Fail('ctor: args')
    opcode, adds = decode_build(slot(vt, 4))
    if opcode != op:
        raise Fail('Build: opcode')
    dsize = decode_size(slot(vt, 0xC))
    fn, fmt, rest = decode_print(slot(vt, 0x10))
    over = {}
    for off, base in BASE_SLOTS.items():
        f = slot(vt, off)
        if f != base:
            if off == 0x14:
                raise Fail('slot +0x14')
            over[off] = (f, slot_body(f))
    extra = []
    off = 0x28
    while 0x401000 <= slot(vt, off) < 0x8C1B50:
        f = slot(vt, off)
        if not (CLIENTSTUB[0] <= f < CLIENTSTUB[1]):
            raise Fail('slot +0x28..')
        extra.append((off, f))
        off += 4
    cls.update(acts=acts, adds=adds, dsize=dsize, pfn=fn, fmt=fmt, pargs=rest, pp=slot(vt, 8), over=over,
               extra=extra, build=slot(vt, 4), sizef=slot(vt, 0xC), printf=slot(vt, 0x10), sdd=slot(vt, 0))
    mems = {}

    def note(off, size, typ):
        if off < 0xC or off + size > cls['size']:
            raise Fail('member outside')
        cur = mems.get(off)
        if cur and cur['size'] != size:
            raise Fail('member sizes')
        if not cur:
            mems[off] = cur = dict(off=off, size=size, typ=None, names=[])
        if typ and not cur['typ']:
            cur['typ'] = typ
        return cur
    for a in acts:
        if a[0] == 'get':
            note(a[1], a[3] if a[2] == 'raw' else TSIZE[a[2]], None if a[2] == 'raw' else a[2])
        else:
            note(a[1], a[2], None)
    gets = [(a[1], a[2], a[3]) for a in acts if a[0] == 'get']
    if gets != list(adds):
        raise Fail('Get / Add differ')
    if sum(mems[g[0]]['size'] for g in gets) != dsize:
        raise Fail('size and fields differ')
    pkind = []
    for a, nm in zip(rest, field_names(fmt)):
        if a[0] == 'member':
            off, how = a[1], a[2]
            typ, w = {'float': ('float', 4), 'double': ('double', 8), 'dword': ('int', 4), 'bool': ('bool', 1),
                      ('mov', 'dword'): ('int', 4), ('movsx', 'word'): ('short', 2),
                      ('movzx', 'word'): ('unsigned short', 2), ('movsx', 'byte'): ('char', 1),
                      ('movzx', 'byte'): ('unsigned char', 1)}.get(how, (None, None))
            if typ is None:
                raise Fail('Print: read')
            host = None
            for mo, mv in mems.items():
                if mo <= off < mo + mv['size']:
                    host = mv
            if host is None:
                host = note(off, w, None)
            if host['off'] == off and host['size'] == w:
                host['ptyp'] = typ
                host['names'].append(nm)
                pkind.append(('m', off, None))
            else:
                if host.get('elem') not in (None, (typ, w)) or host['size'] % w or (off - host['off']) % w:
                    raise Fail('Print: array')
                host['elem'] = (typ, w)
                host['names'].append(nm)
                pkind.append(('m', host['off'], (off - host['off']) // w))
        elif a[0] == 'addr':
            host = mems.get(a[1])
            if host is None or host['typ']:
                raise Fail('Print: address')
            host['elem'] = ('char', 1)
            host['names'].append(nm)
            pkind.append(('a', a[1], None))
        else:
            pkind.append((a[0], a[1], None))
    for off, (f, body) in over.items():
        if isinstance(body, tuple):
            note(body[1], {'dword': 4, 'word': 2, 'byte': 1}[body[2] or 'dword'], 'int')
    cls['pkind'] = pkind
    for mv in mems.values():
        typ = mv.get('ptyp') or mv['typ']
        mv['count'] = None
        if mv.get('elem'):
            et, w = mv['elem']
            typ = et
            if mv['size'] != w or any(k[0] == 'a' and k[1] == mv['off'] for k in pkind):
                mv['count'] = mv['size'] // w
        elif typ is None:
            typ = {4: 'int', 2: 'short', 1: 'char', 8: '__int64'}.get(mv['size'])
            if typ is None:
                typ, mv['count'] = 'char', mv['size']
        if mv['ptyp'] if 'ptyp' in mv else False:
            pass
        w = {'int': 4, 'float': 4, 'short': 2, 'unsigned short': 2, 'char': 1, 'unsigned char': 1, 'bool': 1,
             '__int64': 8, 'double': 8}[typ]
        if (mv['count'] or 1) * w != mv['size']:
            raise Fail('member type')
        mv['ctype'] = typ
        mv['align'] = w
    pos = 0xC
    for o in sorted(mems):
        mv = mems[o]
        if o % mv['align'] or o < pos:
            raise Fail('layout')
        pos = o + mv['size']
    used = set()
    for o in sorted(mems):
        mv = mems[o]
        nm = [n for n in mv['names'] if n]
        n = nm[0] if nm else 'Unknown%02X' % o
        if n in used or n in ('Build', 'Print', 'Connection', 'Serial'):
            n = '%s_%02X' % (n, o)
        used.add(n)
        mv['cname'] = n
        mv['named'] = bool(nm)
    cls['members'] = [mems[o] for o in sorted(mems)]
    return cls


def other_ctors(cls, lo, hi):
    """The file's other functions storing the class's vtable, decoded where they are simple."""
    out = []
    i = bisect.bisect_left(starts, lo)
    while i < len(starts) and starts[i] < hi:
        f = starts[i]
        i += 1
        if f == cls['ctor']:
            continue
        if not any(ins.mnemonic == 'mov' and ins.op_str.endswith(', ' + hex(cls['vt'])) for ins in code(f)):
            continue
        try:
            acts, nargs, eh = decode_ctor(f, cls['vt'], False)
        except Fail as e:
            out.append((f, None, str(e)))
            continue
        out.append((f, (acts, nargs, eh), None))
    return out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--report', action='store_true')
    a = ap.parse_args()
    rows = []
    for line in open(os.path.join(ROOT, 'docs', 'data', 'packet-factory.txt')):
        if line.startswith('#') or not line.strip():
            continue
        p = line.split()
        rows.append(dict(op=int(p[0]), opname=p[2], size=int(p[3], 16), ctor=int(p[4], 16), vt=int(p[5], 16),
                         inline=len(p) > 6 and p[6] == '*'))
    txt = open(os.path.join(PKDIR, 'pk_classes.h')).read().splitlines()
    for fn in sorted(os.listdir(PKDIR)):
        if fn.endswith('.h') and fn not in ('pk.h', 'pk_classes.h'):
            txt += open(os.path.join(PKDIR, fn)).read().splitlines()
    names = {}
    for i, l in enumerate(txt):
        m = re.match(r'^// opcode (\d+) ', l)
        if m:
            c = re.match(r'^class (\w+) : public (\w+)(.*)$', txt[i + 1])
            names[int(m.group(1))] = (c.group(1), c.group(2), c.group(3).strip())
    ends = []
    o0, o1 = ci.offset(0x85C5F0), ci.offset(0x8B1EB0)
    k = ci.data.find(STATIC_END, o0, o1)
    while k >= 0:
        ends.append(0x85C5F0 + (k - o0))
        k = ci.data.find(STATIC_END, k + 1, o1)
    spans = []
    lo = 0x85C5F0
    for e in ends:
        spans.append((lo, e))
        lo = starts[bisect.bisect_right(starts, e)]
    files = []
    ok = fail = 0
    reasons = {}
    for lo, hi in spans:
        classes = []
        allc = []
        for r in sorted(rows, key=lambda r: r['ctor']):
            if not (lo <= r['ctor'] < hi):
                continue
            allc.append(r['op'])
            if r['inline'] or r['op'] not in names or names[r['op']][1] != 'PacketClass':
                continue
            nm, base, tags = names[r['op']]
            try:
                cls = analyse(r['op'], nm, r['vt'], r['ctor'], r['size'])
                cls['opname'], cls['tagtext'] = r['opname'], tags
                cls['others'] = other_ctors(cls, lo, hi)
                classes.append(cls)
                ok += 1
            except Fail as e:
                fail += 1
                reasons.setdefault(str(e)[:40], []).append(r['op'])
        files.append(dict(lo=lo, hi=hi, classes=classes, ops=allc))
    print('%d files, %d classes drafted, %d not' % (len(spans), ok, fail))
    if a.report:
        for k, v in sorted(reasons.items(), key=lambda kv: -len(kv[1])):
            print('  %3d %s: %s' % (len(v), k, ' '.join(map(str, v[:14]))))
    import pickle
    os.makedirs(WORK, exist_ok=True)
    pickle.dump(files, open(os.path.join(WORK, 'decoded.pkl'), 'wb'))


if __name__ == '__main__':
    main()
