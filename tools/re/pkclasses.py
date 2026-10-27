"""Generate the packet classes the factory's switch makes, and the switch itself (docs/kernel-map.md 5).

    python tools/re/pkclasses.py [--check]

From docs/data/packet-factory.txt (tools/re/pkfactory.py) and the client image:

* app/packets/pk_classes.h (GENERATED): one class per opcode, deriving from PacketClass (pk.h),
  `new`ed with the case's size: a constructor from the raw message (declared: the classes'
  files aren't rebuilt) and a filler to that size. The four classes whose constructor the client
  inlines over a shared base's (opcodes 22, 32, 33, 34) get an inline constructor over that base.
* app/packets/pk.cpp, between the markers `// BEGIN pkclasses.py` / `// END pkclasses.py`: the
  switch 0x869790, its cases in the client's source order (the order of their unwind states), the
  default's assert on the client's line 1078.
* tests/capharness/udpreplay/pk_ctor_stubs.cpp (GENERATED): every declared constructor for the
  capture harness, each handing its raw message to Conn_Record (conn_replay.cpp).

Names (D56): the client's when a profile label names the class ('XPacketClass::Process_Packet()',
untagged), else the class's Print text ('X {', NAME 3), else the reference decoder's opcode name
(the Net-7 opcode table, NAME 2: no structure check), else Packet_XXXXClass (NAME 1).
--check: write nothing, exit 1 if a generated file would change.
"""
import argparse
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
from clientimage import ClientImage  # noqa: E402

ROOT = os.path.normpath(os.path.join(HERE, '..', '..'))
FACTORY = os.path.join(ROOT, 'docs', 'data', 'packet-factory.txt')
HEADER = os.path.join(ROOT, 'app', 'packets', 'pk_classes.h')
PKCPP = os.path.join(ROOT, 'app', 'packets', 'pk.cpp')
STUBS = os.path.join(ROOT, 'tests', 'capharness', 'udpreplay', 'pk_ctor_stubs.cpp')
DEFAULT_LINE = 1078
CLIENTSTUB = (0x867AA0, 0x8696C0)   # app\packets\clientstub.cpp's stubs
CLIENTSTUB_FILE = 0xBB2DE4          # its __FILE__ string
CLIENTSTUB_CPP = os.path.join(ROOT, 'app', 'packets', 'clientstub.cpp')
# Members beyond the constructor, for the classes whose inline virtuals pk.cpp emits (0x86E5B0-
# 0x86E850, through the vtables of the classes the switch builds with inline constructors):
# key: opcode, or 'base 0xCTOR' for a base the factory never makes. (declarations, harness stubs
# for what is declared out of line). {C} is the class's name.
OVERRIDES = {
    21: ([
        'virtual int Get_Priority() { return(0x7F); }\t// +0x1C 0x86E5B0',
        'virtual int Get_Type();\t// +0x28 0x8872E0  // NAME 2: what the derived classes\' return (their opcode)',
        'virtual bool Unknown_2C();\t// +0x2C 0x887300',
        'virtual const char *Get_Name();\t// +0x30 0x887320  // NAME 2: what the derived classes\' return (a name)',
    ], [
        'int {C}::Get_Type() { return(0x15); }',
        'bool {C}::Unknown_2C() { return(false); }',
        'const char *{C}::Get_Name() { return("{C}"); }',
    ]),
    22: ([
        'virtual int Get_Type() { return(0x16); }\t// +0x28 0x86E5D0',
        'virtual bool Unknown_2C() { return(false); }\t// +0x2C 0x86E5F0',
        'virtual const char *Get_Name() { return("FireEquipped"); }\t// +0x30 0x86E610',
    ], []),
    'base 0x89D8A0': ([
        'virtual ~{C}();\t// +0x00 0x89D870 / 0x89D970',
        'virtual int Get_Channel() { return(-2); }\t// +0x18 0x86E6A0',
        'virtual int Get_Priority() { return(0); }\t// +0x1C 0x86E680',
        'virtual int Get_Type() = 0;\t// +0x28  // NAME 2: what the derived classes\' return (their opcode)',
        'virtual const char *Get_Name() = 0;\t// +0x2C  // NAME 2: what the derived classes\' return (a name)',
    ], [
        '{C}::~{C}() {}',
    ]),
    32: ([
        'virtual bool Process_Packet(void *client);\t// +0x08 0x736A20 (cpacket.cpp)',
        'virtual int Get_Type() { return(0x20); }\t// +0x28 0x86E6C0',
        'virtual const char *Get_Name() { return("PriorityMessage"); }\t// +0x2C 0x86E6E0',
    ], [
        'bool {C}::Process_Packet(void *) { return(true); }',
    ]),
    33: ([
        'virtual bool Process_Packet(void *client);\t// +0x08 0x736AE0 (cpacket.cpp)',
        'virtual int Get_Type() { return(0x21); }\t// +0x28 0x86E750',
        'virtual const char *Get_Name() { return("QueueMessage"); }\t// +0x2C 0x86E770',
    ], [
        'bool {C}::Process_Packet(void *) { return(true); }',
    ]),
    34: ([
        'virtual bool Process_Packet(void *client);\t// +0x08 0x736BA0 (cpacket.cpp)',
        'virtual int Get_Type() { return(0x22); }\t// +0x28 0x86E7E0',
        'virtual const char *Get_Name() { return("PushMessage"); }\t// +0x2C 0x86E800',
    ], [
        'bool {C}::Process_Packet(void *) { return(true); }',
    ]),
    54: ([	# the handler is tsk_game.cpp's (builder T3's step 9, 2026-10-26)
        'virtual bool Process_Packet(void *client);	// +0x08 0x766FC0 (tsk_game.cpp)',
    ], [
        'bool {C}::Process_Packet(void *) { return(true); }',
    ]),
    111: ([	# the handler is tsk_login.cpp's (builder T3's step 9, 2026-10-26)
        'virtual bool Process_Packet(void *client);	// +0x08 0x76B730 (tsk_login.cpp)',
    ], [
        'bool {C}::Process_Packet(void *) { return(true); }',
    ]),
    112: ([	# the handler is tsk_login.cpp's (builder T3's step 9, 2026-10-26)
        'virtual bool Process_Packet(void *client);	// +0x08 0x76B520 (tsk_login.cpp)',
    ], [
        'bool {C}::Process_Packet(void *) { return(true); }',
    ]),
    117: ([	# the handler is tsk_login.cpp's (builder T3's step 9, 2026-10-26)
        'virtual bool Process_Packet(void *client);	// +0x08 0x76B4A0 (tsk_login.cpp)',
    ], [
        'bool {C}::Process_Packet(void *) { return(true); }',
    ]),
    118: ([	# the handler is tsk_login.cpp's (builder T3's step 9, 2026-10-26)
        'virtual bool Process_Packet(void *client);	// +0x08 0x76B9C0 (tsk_login.cpp)',
    ], [
        'bool {C}::Process_Packet(void *) { return(true); }',
    ]),
    119: ([	# its scalar deleting destructor 0x76B470 is emitted in tsk_login.cpp (builder T3's step 9)
        'virtual ~{C}() {}	// +0x00 0x76B470 (inline: emitted in tsk_login.cpp)',
    ], []),
}

# Names the client gives a class outside its packet code (builder T4's step 9: the log string of the
# handler): opcode -> (name, tag or None for the client's own name)
NAMES = {
    118: ('GlobalLoginPacketClass', None),	# tsk_login.cpp's handler 0x76B9C0 logs 'GlobalLoginPacketClass recv'
}

# Fields instead of the unknown bytes, for classes whose fields another file's code reads (builder
# T4's step 9 views in tsk_game.cpp / tsk_login.cpp): opcode -> member lines (they must fill the class)
FIELDS = {
    54: [	# tsk_game.cpp's handler 0x766FC0 hands Address and Port to Redirect
        'int Unknown0C;\t// +0x0C',
        'unsigned long Address;\t// +0x10  // NAME 1: handed to Redirect (tsk_game.cpp 0x766FC0)',
        'unsigned short Port;\t// +0x14  // NAME 1: handed to Redirect (tsk_game.cpp 0x766FC0)',
    ],
    117: [	# tsk_login.cpp's handler 0x76B4A0: the log 'GlobalErrorPacketClass recv: %d : %d : %s'
        'int Unknown0C;\t// +0x0C',
        'int Error;\t// +0x10  // NAME 1: the log \'GlobalErrorPacketClass recv: %d : %d : %s\' (tsk_login.cpp)',
        'std::string Message;\t// +0x14  // NAME 1: the same log',
    ],
}

BEGIN = '// BEGIN pkclasses.py (generated: the switch 0x869790; do not edit by hand)'
END = '// END pkclasses.py'


def rebuilt_classes():
    """The classes rebuilt in their own headers (app/packets/*.h, gameedit/*.h): opcode -> (name, include).

    A rebuilt class is declared after a line `// opcode N (...)`, as pk_classes.h declares it; pk_classes.h
    then includes its header instead of declaring it, and the harness gets no constructor for it."""
    out = {}
    base = os.path.join(ROOT, 'app', 'packets')
    for sub in ('', 'gameedit'):
        d = os.path.join(base, sub)
        if not os.path.isdir(d):
            continue
        for fn in sorted(os.listdir(d)):
            if not fn.endswith('.h') or (not sub and fn in ('pk.h', 'pk_classes.h')):
                continue
            lines = open(os.path.join(d, fn)).read().splitlines()
            for i, l in enumerate(lines[:-1]):
                m = re.match(r'^// opcode (\d+) \(', l)
                c = re.match(r'^class (\w+)', lines[i + 1])
                if m and c:
                    out[int(m.group(1))] = (c.group(1), (sub + '\\' if sub else '') + fn)
    return out


def rebuilt_names():
    """The names of every class a rebuilt header (app/packets/*.h but pk.h / pk_classes.h) declares:
    a base the factory never makes is rebuilt when its name is among them (pk_classes.h then neither
    declares it nor gives the harness its constructor or OVERRIDES' stubs)."""
    out = set()
    base = os.path.join(ROOT, 'app', 'packets')
    for sub in ('', 'gameedit'):
        d = os.path.join(base, sub)
        if not os.path.isdir(d):
            continue
        for fn in sorted(os.listdir(d)):
            if not fn.endswith('.h') or (not sub and fn in ('pk.h', 'pk_classes.h')):
                continue
            for l in open(os.path.join(d, fn)).read().splitlines():
                m = re.match(r'^class (\w+)', l)
                if m:
                    out.add(m.group(1))
    return out


def camel(op_name):
    return ''.join(w[:1] + w[1:].lower() for w in op_name.split('_') if w)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--check', action='store_true')
    ap.add_argument('--marker', action='store_true', help='keep the // @0x869790 marker (an appfile.py match draft)')
    ap.add_argument('--pk', default=PKCPP, help='the pk.cpp to fill (default: the tree file)')
    a = ap.parse_args()
    ci = ClientImage()
    th = ci.incremental_thunks()

    def slot(vt, off):
        v = ci.u32(vt + off)
        return th.get(v, v)

    def strings_in(f, size=0x100):
        out = []
        if not (0x401000 <= f < 0x8C1B50):
            return out
        for ins in ci.disasm(f, size):
            if ins.mnemonic in ('push', 'mov') and '0x' in ins.op_str:
                try:
                    v = int(ins.op_str.rsplit(' ', 1)[-1], 16)
                except ValueError:
                    continue
                if 0xA00000 < v < 0xC00000:
                    s = ci.cstr(v)
                    if s:
                        out.append(s)
            if ins.mnemonic == 'ret':
                break
        return out

    rows = []
    for line in open(FACTORY):
        if line.startswith('#') or not line.strip():
            continue
        p = line.split()
        rows.append(dict(op=int(p[0]), opname=p[2], size=int(p[3], 16), ctor=int(p[4], 16), vt=int(p[5], 16),
                         inline=len(p) > 6 and p[6] == '*'))

    # cases in source order: the unwind state each case's new sets
    def state(op):
        if op <= 0xED:
            t = ci.u32(0x86D1CC + 4 * op)
        elif op == 5000:
            t = 0x86CD03
        elif 5001 <= op <= 5011:
            t = ci.u32(0x86D584 + 4 * (op - 5001))
        elif op == 30000:
            t = 0x86CED2
        else:
            t = ci.u32(0x86D5B0 + 4 * ci.data[ci.offset(0x86D5F0 + op - 30001)])
        for ins in ci.disasm(t, 0x40):
            if ins.mnemonic == 'mov' and (ins.op_str.startswith('dword ptr [esp + 0x10], ')
                                          or ins.op_str.startswith('byte ptr [esp + 0x10], ')):
                return int(ins.op_str.split(', ')[1], 16)
        raise SystemExit('no state for opcode %d' % op)

    for r in rows:
        r['state'] = state(r['op'])
        label = [s for s in strings_in(slot(r['vt'], 8)) if re.match(r'^\w+Packet(Class)?::Process_Packet', s)]
        prt = [s for s in strings_in(slot(r['vt'], 0x10), 0x200) if re.match(r'^[A-Za-z_]\w* ?\{', s)]
        if label:
            r['name'], r['tag'] = label[0].split('::')[0], None
        elif prt:
            base = re.match(r'^([A-Za-z_]\w*)', prt[0]).group(1)
            r['name'] = base + ('PacketClass' if not base.endswith('Packet') else 'Class')
            r['tag'] = "NAME 3: the client's Print text '%s {' and the PacketClass convention" % base
        elif r['opname'] != '?':
            r['name'] = camel(r['opname']) + 'PacketClass'
            r['tag'] = "NAME 2: the reference decoder's opcode name %s (the Net-7 opcode table; no structure check)" % r['opname']
        else:
            r['name'] = 'Packet_%04XClass' % r['op']
            r['tag'] = 'NAME 1: neutral (D10): opcode 0x%04X' % r['op']
        if r['op'] in NAMES:
            r['name'], r['tag'] = NAMES[r['op']]
    seen = {}
    for r in sorted(rows, key=lambda r: r['state']):
        if r['name'] in seen:
            r['name'] = r['name'].replace('Class', '_%dClass' % r['op'])
            r['tag'] = (r['tag'] or 'the client\'s name') + '; opcode added (another opcode\'s class has the name)'
        seen[r['name']] = r
    rebuilt = rebuilt_classes()
    for r in rows:
        if r['op'] in rebuilt:
            r['name'], r['rebuilt'] = rebuilt[r['op']]
    by_ctor = {}
    for r in rows:
        if not r['inline']:
            by_ctor[r['ctor']] = r

    # clientstub.cpp's stubs (`G_ASSERT(0); return(true);` on line N): the vtable slots holding one
    def stub_line(f):
        ins = list(ci.disasm(f, 0x20))
        if len(ins) >= 2 and ins[0].mnemonic == 'push' and ins[1].op_str == hex(CLIENTSTUB_FILE):
            return int(ins[0].op_str, 16)
        return None
    def vt_stubs(vt):
        out = []
        offs = [8]
        off = 0x28   # the classes' own virtuals after PacketClass's ten
        while 0x401000 <= slot(vt, off) < 0x8C1B50:
            offs.append(off)
            off += 4
        for off in offs:
            f = slot(vt, off)
            if CLIENTSTUB[0] <= f < CLIENTSTUB[1]:
                line = stub_line(f)
                if line:
                    out.append((off, f, line))
        return out

    # the inlined constructors' bases: another opcode's class, or one the factory never makes
    extra = {}
    for r in rows:
        if r['inline']:
            if r['ctor'] in by_ctor:
                r['base'] = by_ctor[r['ctor']]
            else:
                if r['ctor'] not in extra:
                    vt = None
                    for ins in ci.disasm(r['ctor'], 0x200):
                        if ins.mnemonic == 'mov' and ins.op_str.startswith('dword ptr [') and ', 0x' in ins.op_str:
                            v = int(ins.op_str.rsplit(', ', 1)[1], 16)
                            if 0xAD0000 <= v < 0xBA0000:
                                vt = v
                        if ins.mnemonic == 'ret':
                            break
                    extra[r['ctor']] = dict(ctor=r['ctor'], vt=vt, size=r['size'], name=None, derived=[])
                extra[r['ctor']]['derived'].append(r)
                r['base'] = extra[r['ctor']]
    for e in extra.values():
        # the derived classes' names share a suffix: 'PriorityMessageLine', 'QueueMessageLine' -> MessageLine
        names = [d['name'][:-len('PacketClass')] for d in e['derived'] if d['name'].endswith('PacketClass')]
        suffix = ''
        if len(names) > 1:
            k = 1
            while all(len(n) >= k for n in names) and len(set(n[-k:] for n in names)) == 1:
                k += 1
            suffix = names[0][-(k - 1):] if k > 1 else ''
            m = re.search(r'[A-Z]', suffix)
            suffix = suffix[m.start():] if m else ''
        if suffix:
            e['name'] = suffix + 'PacketClass'
            e['tag'] = 'NAME 3: by convention from its derived classes %s' % ', '.join(d['name'] for d in e['derived'])
        else:
            e['name'] = 'Unknown_%XPacketClass' % e['ctor']
            e['tag'] = 'NAME 1: neutral (D10)'
    base_rebuilt = rebuilt_names()
    for e in extra.values():
        e['rebuilt'] = e['name'] in base_rebuilt

    # the stubs each class declares (not those it inherits from its base)
    for r in list(extra.values()) + rows:
        b = r.get('base')
        r['stubs'] = [(off, f, line) for off, f, line in vt_stubs(r['vt'])
                      if not (b and b.get('vt') and slot(b['vt'], off) == f)]
    stubs_all = sorted((line, f, off, r['name']) for r in list(extra.values()) + rows for off, f, line in r['stubs'])
    if len(set(f for _, f, _, _ in stubs_all)) != len(stubs_all):
        raise SystemExit('a clientstub.cpp stub is declared by two classes')

    # ---- the header
    h = []
    h.append('/*')
    h.append('**\tEarth & Beyond client (reconstruction)')
    h.append('**\tPortions Copyright Electronic Arts Inc., released under the GNU General Public License v3.')
    h.append('**')
    h.append('**\tGENERATED by tools/re/pkclasses.py from docs/data/packet-factory.txt: do not edit by hand.')
    h.append('**\tPROVISIONAL file name: the packet classes are declared in many client headers (app\\packets')
    h.append('**\tpk*.h, gameedit\\pkge*.h); here only what the factory 0x869790 needs: each class\'s size (the')
    h.append('**\tcase\'s new) and its constructor from the raw message (not rebuilt yet: declared). Members')
    h.append('**\tare a filler to the client\'s size; the virtuals are PacketClass\'s until the classes are')
    h.append('**\trebuilt.')
    h.append('*/')
    h.append('')
    h.append('#if defined(_MSC_VER)')
    h.append('#pragma once')
    h.append('#endif')
    h.append('')
    h.append('#ifndef PK_CLASSES_H  // NAME 1 [pk_classes.h]: provisional file name (D37)')
    h.append('#define PK_CLASSES_H')
    h.append('')
    h.append('#include "pk.h"')
    for inc in sorted(set(r['rebuilt'] for r in rows if r.get('rebuilt'))):
        h.append('#include "%s"\t// rebuilt' % inc)
    h.append('')

    def tag(t):
        return ('  // ' + t) if t else ''

    def key(r):
        return r['op'] if 'op' in r else 'base 0x%X' % r['ctor']

    def decl(r, base='PacketClass', base_size=0xC, inline_base=None):
        members, _ = OVERRIDES.get(key(r), ([], []))
        out = []
        out.append('// opcode %d (0x%04X, %s): vtable 0x%X, constructor 0x%X, 0x%X bytes' % (
            r['op'], r['op'], r['opname'], r['vt'], r['ctor'], r['size']) if 'op' in r else
            '// the base of %s: vtable 0x%X, constructor 0x%X' % (
                ', '.join(d['name'] for d in r['derived']), r['vt'] or 0, r['ctor']))
        out.append('class %s : public %s%s' % (r['name'], base, tag(r.get('tag'))))
        out.append('{')
        out.append('public:')
        if inline_base:
            out.append('\t%s(RawPacketClass *raw) : %s(raw) {}\t// inline (the case stores the vtable)' % (r['name'], base))
        else:
            out.append('\t%s(RawPacketClass *raw);\t// 0x%X' % (r['name'], r['ctor']))
        for off, f, line in r.get('stubs', []):
            if off == 8:
                out.append('\tvirtual bool Process_Packet(void *client);\t// +0x08 0x%X (clientstub.cpp l.%d)' % (f, line))
            else:
                out.append('\tvirtual bool Unknown_%02X(void *a);\t// +0x%02X 0x%X (clientstub.cpp l.%d)  '
                           '// NAME 1: neutral (D10)' % (off, off, f, line))
        for m in members:
            out.append('\t' + m.replace('{C}', r['name']))
        if key(r) in FIELDS:
            out.append('')
            for f in FIELDS[key(r)]:
                out.append('\t' + f)
        elif r['size'] > base_size:
            out.append('\tchar Unknown%02X[0x%X];\t// +0x%02X' % (base_size, r['size'] - base_size, base_size))
        out.append('};')
        out.append('')
        return out

    for e in sorted(extra.values(), key=lambda e: e['ctor']):
        if not e['rebuilt']:
            h += decl(e)
    done = set()
    for r in sorted(rows, key=lambda r: r['state']):
        if r['inline'] and r['base'].get('op') is not None and r['base']['op'] not in done:
            b = r['base']
            h += decl(b)
            done.add(b['op'])
        if r['op'] in done:
            continue
        if r.get('rebuilt'):
            done.add(r['op'])
            continue
        if r['inline']:
            h += decl(r, r['base']['name'], r['base']['size'], True)
        else:
            h += decl(r)
        done.add(r['op'])
    h.append('#endif')
    header = '\r\n'.join(h) + '\r\n'

    # ---- the switch, in pk.cpp between the markers
    pk = open(a.pk, 'rb').read().decode('utf-8').replace('\r\n', '\n').split('\n')
    i = pk.index(BEGIN)
    j = pk.index(END)
    sw = [BEGIN, '// @0x869790' if a.marker else '', 'PacketClass *Unknown_869790(RawPacketClass *raw)', '{',
          '\tPacketClass *packet;', '\tswitch (raw->Header.Opcode) {']
    for r in sorted(rows, key=lambda r: r['state']):
        sw.append('\t\tcase %d:' % r['op'])
        sw.append('\t\t\tpacket = new %s(raw);' % r['name'])
        sw.append('\t\t\tbreak;')
    sw.append('\t\tdefault:')
    first = i + len(sw) + 1   # the line the next one gets (1-based)
    pad = DEFAULT_LINE - first
    if pad < 1:
        raise SystemExit('the switch runs past line %d' % DEFAULT_LINE)
    sw.append('\t\t\t// (spacing kept: the assert is on the client\'s line %d)' % DEFAULT_LINE)
    sw += [''] * (pad - 2)
    sw.append('\t\t\tpacket = 0;')
    sw.append('\t\t\tG_ASSERT(0);')
    sw += ['\t\t\tbreak;', '\t}', '\treturn(packet);', '}', END]
    new_pk = pk[:i] + sw + pk[j + 1:]
    assert new_pk[DEFAULT_LINE - 1].strip() == 'G_ASSERT(0);', new_pk[DEFAULT_LINE - 1]
    pkcpp = '\r\n'.join(new_pk)

    # ---- the harness's constructors
    st = ['// GENERATED by tools/re/pkclasses.py: the packet classes\' constructors for the capture harness',
          '// (udpreplay --conn): each hands its raw message to Conn_Record (conn_replay.cpp).',
          '#include "..\\..\\..\\app\\packets\\pk_classes.h"', '',
          'void Conn_Record(PacketClass *packet, RawPacketClass *raw);', '']
    for e in sorted(extra.values(), key=lambda e: e['ctor']):
        if not e['rebuilt']:
            st.append('%s::%s(RawPacketClass *raw) { Conn_Record(this, raw); }' % (e['name'], e['name']))
    for r in sorted(rows, key=lambda r: r['state']):
        if not r['inline'] and not r.get('rebuilt'):
            st.append('%s::%s(RawPacketClass *raw) { Conn_Record(this, raw); }' % (r['name'], r['name']))
    st.append('')
    st.append('// The rebuilt classes\' Process_Packet defined in a file not rebuilt (cpacket.cpp, ...)')
    for r in sorted(rows, key=lambda r: r['state']):
        pp = slot(r['vt'], 8)
        if r.get('rebuilt') and pp != 0x52B240 and not (CLIENTSTUB[0] <= pp < CLIENTSTUB[1]):
            st.append('bool %s::Process_Packet(void *) { return(true); }\t// 0x%X' % (r['name'], pp))
    st.append('')
    st.append('// What OVERRIDES declares out of line (the classes\' own files, not rebuilt)')
    allr = list(extra.values()) + rows
    for r in sorted(allr, key=lambda r: str(key(r))):
        if r.get('rebuilt') and 'op' not in r:
            continue
        for d in OVERRIDES.get(key(r), ([], []))[1]:
            d = d.replace('{C}', r['name'])
            if any(s.startswith(d) for s in st):	# a rebuilt class's handler stub is above already (P9)
                continue
            st.append(d)
    stubs = '\n'.join(st) + '\n'

    # ---- clientstub.cpp: each stub on its client line (D25), the notes after the last one
    cs = ['/*',
          '**\tEarth & Beyond client (reconstruction): e:\\build\\beta\\G\\app\\Packets\\clientstub.cpp',
          '**\tGENERATED by tools/re/pkclasses.py: do not edit by hand (notes at the end).',
          '*/',
          '',
          '#include "pk_classes.h"',
          '#include "..\\shared\\unknown_debug.h"',
          '']
    for line, f, off, name in stubs_all:
        if len(cs) > line - 2:
            raise SystemExit('clientstub.cpp: line %d is taken' % line)
        cs += [''] * (line - 2 - len(cs))
        fn = 'Process_Packet(void *)' if off == 8 else 'Unknown_%02X(void *)' % off
        cs.append('bool %s::%s\t// 0x%X' % (name, fn, f))
        cs.append('{\tG_ASSERT(0); return(true); }')
        assert len(cs) == line
    cs += ['',
           '/*',
           '**\tclientstub.cpp, client 0x%X-0x%X: the %d packets the client only sends (or a server makes),' % (
               CLIENTSTUB[0], stubs_all[-1][1] + 0x1C, len(stubs_all)),
           '**\teach Process_Packet (and opcode 72\'s slot +0x28) an assert that returns true, on the client\'s',
           '**\tlines %d-%d (D25). The class of each: the vtable slot that holds it (pk_classes.h).' % (
               stubs_all[0][0], stubs_all[-1][0]),
           '**\tNot rebuilt: the guarded static 0x869690 after the last stub (atexit 0x42B320, which many',
           '**\tapp objects have: its header is not known).',
           '*/',
           '']
    clientstub = '\r\n'.join(cs)

    changed = []
    for path, text, mode in ((HEADER, header, 'b'), (a.pk, pkcpp, 'b'), (STUBS, stubs, 't'),
                             (CLIENTSTUB_CPP, clientstub, 'b')):
        old = open(path, 'rb').read().decode('utf-8') if os.path.exists(path) else None
        new = text if mode == 'b' else text
        if old != new:
            changed.append(path)
            if not a.check:
                open(path, 'wb').write(new.encode('utf-8'))
    named = sum(1 for r in rows if r['tag'] is None)
    print('%d classes (%d named by the client, %d by Print, %d by opcode name, %d neutral), %d bases; %s' % (
        len(rows), named, sum(1 for r in rows if r['tag'] and r['tag'].startswith('NAME 3')),
        sum(1 for r in rows if r['tag'] and r['tag'].startswith('NAME 2')),
        sum(1 for r in rows if r['tag'] and r['tag'].startswith('NAME 1')), len(extra),
        ('changed: ' + ', '.join(os.path.relpath(c, ROOT) for c in changed)) if changed else 'unchanged'))
    return 1 if (a.check and changed) else 0


if __name__ == '__main__':
    sys.exit(main())
