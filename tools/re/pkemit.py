"""Draft the decoded packet classes (tools/re/pkdecode.py's .work/pkdecode/decoded.pkl) into app/packets:
one header and one .cpp per client object (docs/kernel-map.md section 6, step 5).

    python tools/re/pkemit.py [--write] [--skip CLASS ...] [--files NAME ...] [--markers DIR]

Prints the files and their classes; --write writes them (CRLF; the drafts overwrite the tree files: hand
edits to a drafted file go in this tool, or the file leaves the drafts with --skip). --markers DIR also
writes copies with `// @0xADDR` markers for appfile.py match. A file keeps the name its classes already
have in the tree (pkclasses.py's headers); functions listed in .work/pkdecode/notexact.txt (`ADDR what`)
get a `// NOT EXACT:` line (D64).
"""
import argparse
import os
import pickle
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.normpath(os.path.join(HERE, '..', '..'))
PKDIR = os.path.join(ROOT, 'app', 'packets')
WORK = os.path.join(ROOT, '.work', 'pkdecode')
sys.path.insert(0, HERE)
import pkdecode as gen  # noqa: E402
import pkclasses  # noqa: E402

# files the client names (asserts), by an address inside them
NAMED = {0x8622A0: 'gameedit\\pkgespwn', 0x874790: 'pk_player_object_relationship', 0x875910: 'pk_sub_parts',
         0x880340: 'pkdebug', 0x886640: 'pkfindmember', 0x895F50: 'pkjoblist', 0x8A9BB0: 'pktask',
         0x8ABED0: 'pkupdate', 0x89AB30: 'pklogoff'}
WIDTH = {'int': 4, 'float': 4, 'short': 2, 'unsigned short': 2, 'char': 1, 'unsigned char': 1, 'bool': 1,
         '__int64': 8, 'double': 8}
SLOT = {0x18: ('int', 'Get_Channel'), 0x1C: ('int', 'Get_Priority'), 0x20: ('bool', 'Is_Reliable'),
        0x24: ('bool', 'Close_Channel')}


def c_string(s):
    return '"' + s.replace('\\', '\\\\').replace('"', '\\"') + '"'


def pname(n):
    s = re.sub(r'(?<=[a-z0-9])([A-Z])', r'_\1', n)
    s = re.sub(r'(?<=[A-Z])([A-Z][a-z])', r'_\1', s)
    return s.lower()


def stub_line(f):
    c = list(gen.ci.disasm(f, 0x10))
    return gen.imm(c[0].op_str) if c and c[0].mnemonic == 'push' else None


def class_name_tag(cls):
    tag = cls['tagtext'].lstrip('/ ').strip()
    return ('  // ' + tag) if tag else ''


def decl(cls, ctors):
    m = cls['members']
    out = ['// opcode %d (0x%04X, %s): vtable 0x%X, constructor 0x%X, 0x%X bytes' % (
        cls['op'], cls['op'], cls['opname'], cls['vt'], cls['ctor'], cls['size'])]
    out.append('class %s : public PacketClass%s' % (cls['name'], class_name_tag(cls)))
    out += ['{', 'public:']
    for f, sig, _ in ctors:
        out.append('\t%s(%s);\t// 0x%X' % (cls['name'], sig, f))
    out.append('\t%s(RawPacketClass *raw);\t// 0x%X' % (cls['name'], cls['ctor']))
    out.append('\tvirtual RawPacketClass *Build();\t// +0x04 0x%X' % cls['build'])
    if cls['pp'] != 0x52B240:
        if gen.CLIENTSTUB[0] <= cls['pp'] < gen.CLIENTSTUB[1]:
            out.append('\tvirtual bool Process_Packet(void *client);\t// +0x08 0x%X (clientstub.cpp l.%d)' % (
                cls['pp'], stub_line(cls['pp'])))
        else:
            out.append('\tvirtual bool Process_Packet(void *client);\t// +0x08 0x%X (not rebuilt)' % cls['pp'])
    out.append('\tvirtual int Get_Data_Size();\t// +0x0C 0x%X' % cls['sizef'])
    out.append('\tvirtual int Print(char *buffer, int len);\t// +0x10 0x%X' % cls['printf'])
    byoff = {mv['off']: mv for mv in m}
    for off in sorted(cls['over']):
        f, body = cls['over'][off]
        typ, nm = SLOT[off]
        if isinstance(body, tuple):
            body = byoff[body[1]]['cname']
        out.append('\tvirtual %s %s() { return(%s); }\t// +0x%02X 0x%X' % (typ, nm, body, off, f))
    if cls.get('gettype'):
        tag = "  // NAME 2: what it returns (the opcode Build sends), as MessageLinePacketClass's"
        if cls['gettype'] < cls['sdd']:
            out.append('\tvirtual int Get_Type() { return(%d); }\t// +0x28 0x%X%s' % (cls['op'], cls['gettype'], tag))
        else:
            out.append('\tvirtual int Get_Type();\t// +0x28 0x%X%s' % (cls['gettype'], tag))
    for off, f in cls['extra']:
        out.append('\tvirtual bool Unknown_%02X(void *a);\t// +0x%02X 0x%X (clientstub.cpp l.%d)  '
                   '// NAME 1: neutral (D10)' % (off, off, f, stub_line(f)))
    out.append('')
    pos = 0xC
    for mv in m:
        if mv['off'] > pos:
            out.append('\tchar Unknown%02X[0x%X];\t// +0x%02X' % (pos, mv['off'] - pos, pos))
        arr = '[%d]' % mv['count'] if mv['count'] else ''
        tag = ("  // NAME 3: the client's Print text '%s='" % mv['cname']) if mv['named'] else ''
        out.append('\t%s %s%s;\t// +0x%02X%s' % (mv['ctype'], mv['cname'], arr, mv['off'], tag))
        pos = mv['off'] + mv['size']
    if cls['size'] > pos:
        out.append('\tchar Unknown%02X[0x%X];\t// +0x%02X' % (pos, cls['size'] - pos, pos))
    out += ['};', '']
    return out


def init_list(cls, acts, params):
    """`: A(0), B(x)` for the stores before the vtable's, in declaration order; the body's for those after."""
    byoff = {mv['off']: mv for mv in cls['members']}
    early, late = [], []
    for mv in cls['members']:
        if mv['ctype'] == 'std::string' and mv.get('init') == '':
            early.append((mv['off'], mv['cname'], '""'))
    for a in acts:
        if a[0] in ('get', 'string'):
            if a[0] == 'string' and a[2] == '' and not any(e[0] == a[1] for e in early):
                early.append((a[1], byoff[a[1]]['cname'], '""'))
            continue
        mv = byoff.get(a[1])
        if mv is None or mv['size'] != a[2] or mv['count']:
            raise gen.Fail('store to +0x%X' % a[1])
        if a[0] == 'param':
            val = params[a[3]]
        else:
            val = str(a[3]) if a[3] < 0x80000000 else str(a[3] - 0x100000000)
            if mv['ctype'] == 'float':
                raise gen.Fail('float constant')
        (late if a[4] else early).append((mv['off'], mv['cname'], val))
    early.sort()
    il = (' : ' + ', '.join('%s(%s)' % (n, v) for _, n, v in early)) if early else ''
    return il, ['\t%s = %s;' % (n, v) for _, n, v in late]


def field_ctor(cls, f, dec):
    acts, nargs, eh = dec
    byoff = {mv['off']: mv for mv in cls['members']}
    params = {}
    sig = []
    for k in range(nargs):
        st = [a for a in acts if a[0] == 'param' and a[3] == k]
        if len(st) != 1:
            raise gen.Fail('parameter %d' % k)
        mv = byoff.get(st[0][1])
        if mv is None or mv['count']:
            raise gen.Fail('parameter %d member' % k)
        params[k] = pname(mv['cname'])
        sig.append('%s %s' % (mv['ctype'], params[k]))
    il, body = init_list(cls, acts, params)
    text = ['%s::%s(%s)%s' % (cls['name'], cls['name'], ', '.join(sig), il), '{'] + body + ['}']
    return ', '.join(sig), text


def defs(cls):
    """[(address, lines)] of the class's out-of-line functions, and the field constructors' signatures."""
    out = []
    ctors = []
    for f, dec, err in cls['others']:
        if dec is None:
            continue
        try:
            sig, text = field_ctor(cls, f, dec)
        except gen.Fail:
            continue
        ctors.append((f, sig, None))
        out.append((f, text))
    byoff = {mv['off']: mv for mv in cls['members']}
    il, _ = init_list(cls, cls['acts'], {})
    get = []
    for a in cls['acts']:
        mv = byoff[a[1]]
        if a[0] != 'get':
            if a[4]:
                v = a[3] if a[3] < 0x80000000 else a[3] - 0x100000000
                get.append('\t%s = %d;' % (mv['cname'], v))
            continue
        if a[2] == 'raw':
            get.append('\traw->Get(%s%s, sizeof(%s));' % ('' if mv['count'] else '&', mv['cname'], mv['cname']))
        elif a[2] == 'string':
            get.append('\traw->Get(&%s);' % mv['cname'])
        elif a[2] == 'cstr':
            first = not any('char *string' in g for g in get)
            get.append('\t%sstring = raw->Get_String();' % ('char *' if first else ''))
            get.append('\t%s = string;' % mv['cname'])
            get.append('\tdelete [] string;')
        else:
            get.append('\traw->Get(%s);' % mv['cname'])
    uses = any('raw->' in g for g in get)
    out.append((cls['ctor'], ['%s::%s(RawPacketClass *%s)%s' % (cls['name'], cls['name'], 'raw' if uses else '', il),
                              '{'] + get + ['}']))
    b = ['RawPacketClass *%s::Build()' % cls['name'], '{',
         '\tRawPacketClass *raw = new RawPacketClass(%s, Get_Data_Size());' % (
             'Get_Type()' if cls.get('gettype') else cls['op'])]
    for off, kind, n in cls['adds']:
        mv = byoff[off]
        if kind == 'raw':
            b.append('\traw->Add(%s%s, sizeof(%s));' % ('' if mv['count'] else '&', mv['cname'], mv['cname']))
        elif kind == 'cstr':
            b.append('\traw->Add_String(%s.c_str());' % mv['cname'])
        else:
            b.append('\traw->Add(%s);' % mv['cname'])
    b += ['\treturn(raw);', '}']
    out.append((cls['build'], b))
    if cls.get('gettype') and cls['gettype'] > cls['sdd']:
        out.append((cls['gettype'], ['int %s::Get_Type()' % cls['name'], '{', '\treturn(%d);' % cls['op'], '}']))
    terms = []
    for off, kind, n in cls['adds']:
        nm = byoff[off]['cname']
        terms.append('RawPacketClass::Get_Size(%s)' % nm if kind == 'string' else
                     '%s.length() + 1' % nm if kind == 'cstr' else 'sizeof(%s)' % nm)
    expr = ' + '.join(terms) if terms else '0'
    out.append((cls['sizef'], ['int %s::Get_Data_Size()' % cls['name'], '{', '\treturn(%s);' % expr, '}']))
    args = []
    for k in cls['pkind']:
        if k[0] == 'str':
            args.append(c_string(k[1]))
        elif k[0] == 'int':
            args.append(str(k[1]))
        elif k[0] == 'm':
            mv = byoff[k[1]]
            args.append(mv['cname'] + ('[%d]' % k[2] if k[2] is not None else ''))
        elif k[0] == 'a':
            args.append(byoff[k[1]]['cname'])
        elif k[0] == 'c':
            args.append(byoff[k[1]]['cname'] + '.c_str()')
    fmt = c_string(cls['fmt'])
    if cls['pfn'] == gen.SPRINTF:
        p = ['int %s::Print(char *buffer, int)' % cls['name'], '{',
             '\treturn(sprintf(%s));' % ', '.join(['buffer', fmt] + args), '}']
    else:
        p = ['int %s::Print(char *buffer, int len)' % cls['name'], '{',
             '\treturn(_snprintf(%s));' % ', '.join(['buffer', 'len', fmt] + args), '}']
    out.append((cls['printf'], p))
    return out, ctors


# functions appfile.py match found not exact: {address: what differs} (.work/pkB/notexact.txt)
NOTEXACT = {}
_p = os.path.join(WORK, 'notexact.txt')
if os.path.exists(_p):
    for _l in open(_p):
        if _l.strip():
            _a, _r = _l.rstrip('\n').split(' ', 1)
            NOTEXACT[int(_a, 16)] = _r

# drafted files that hold classes written by hand too: never rewritten (their decoded classes are kept as drafted)
HAND = {'pkmessagechatstream', 'pkserverready', 'pkjoblist', 'pktask', 'pkgadget', 'pkglobalconnect', 'pkspline', 'pknote', 'pkpetitionstuck', 'pkmessagetime', 'pkmodechange', 'pkmoderequest', 'pkguildplayeronline', 'pkgmeditplayerinventory', 'pkgmviewmissionlog', 'pkgmdestroyitem', 'pkgmresetmission', 'pkgpsrequest', 'pkincapacitancerequest', 'pkguildranknamesrequestclient', 'pkconfirmedaction', 'pktriggeremote', 'pkcta_request', 'pkserverparameters', 'pksetclienttype', 'pknotelist', 'pkmanufactureitemcatagorypacket', 'pkstarbasetow', 'pkdebug', 'pkinventorymove', 'pknamedecal', 'pkverblist', 'pkrequesttime', 'pkwarpindex', 'pksoundcommand', 'pkupdate', 'pkpointeffect', 'pk_player_object_relationship', 'pkclientchat', 'pkmessagestring', 'pkgroup', 'pkdialog', 'pkunknown_877c80', 'pkmove', 'pknavdelete', 'pkunknown_888b60', 'pkmessageline', 'pkcolorization'}   # opcode 106 by hand (builder P, 2026-10-26); 62, 9, 137, 165, 29, 30, 98, 99, 20, 153, 139, 174, 35, 36, 17 (builder P3, 2026-10-26)

# the names the files have in the tree (a file keeps its name when more of its classes are drafted)
KEEP = dict((op, inc[:-2]) for op, (name, inc) in pkclasses.rebuilt_classes().items())


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--write', action='store_true')
    ap.add_argument('--skip', nargs='*', default=[])
    ap.add_argument('--files', nargs='*')
    ap.add_argument('--markers', help='also write DIR/<file>.cpp with // @0xADDR markers (appfile.py match)')
    a = ap.parse_args()
    files = pickle.load(open(os.path.join(WORK, 'decoded.pkl'), 'rb'))
    emitted = []
    for fl in files:
        good = []
        texts = []
        for cls in fl['classes']:
            if cls['name'] in a.skip:
                continue
            try:
                d, ctors = defs(cls)
            except gen.Fail as e:
                print('  skip %s: %s' % (cls['name'], e))
                continue
            good.append((cls, d, ctors))
        if not good:
            continue
        name = None
        for g in good:
            if g[0]['op'] in KEEP:
                name = KEEP[g[0]['op']]
        for addr, nm in NAMED.items():
            if fl['lo'] <= addr < fl['hi']:
                name = nm
        tag = 'the client names the file (an assert)' if name else None
        if name in KEEP.values() and name not in NAMED.values():
            real = [g[0]['name'] for g in good if not g[0]['name'].startswith('Packet_')]
            tag = ("NAME 1 [%s]: provisional, pk + its first named class's name" % name) if real and \
                not name.startswith('pkunknown_') else \
                "NAME 1 [%s]: provisional, neutral (D10): the object's first address" % name
        if name is None:
            real = [g[0]['name'] for g in good if not g[0]['name'].startswith('Packet_')]
            if real:
                base = re.sub(r'(_\d+)?(Packet)?Class$', '', real[0]).strip('_')
                name = 'pk' + base.lower()
                tag = "NAME 1 [%s]: provisional, pk + its first named class's name" % name
            else:
                name = 'pkunknown_%x' % fl['lo']
                tag = "NAME 1 [%s]: provisional, neutral (D10): the object's first address" % name
        if name in [e[0] for e in emitted]:
            name = '%s_%x' % (name, fl['lo'])
        if name == 'pklogoff' or name in HAND or (a.files and name not in a.files):
            continue
        if os.path.exists(os.path.join(PKDIR, name + '.cpp')) and not a.write:
            print('  exists: %s' % name)
        h = ['/*', '**\tEarth & Beyond client (reconstruction)',
             '**\tPortions Copyright Electronic Arts Inc., released under the GNU General Public License v3.', '**',
             '**\tThis file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).',
             '**\tOriginal location: e:\\build\\beta\\G\\app\\packets\\%s.h (%s)' % (
                 name, 'the name provisional: see the .cpp' if tag.startswith('NAME') else 'the .cpp named by an assert'),
             '**', '**\tThe packet classes of client 0x%X-0x%X rebuilt so far (opcode %s); the file\'s other classes' % (
                 fl['lo'], fl['hi'], ', '.join(str(g[0]['op']) for g in good)),
             '**\tare still declared in pk_classes.h.', '*/', '', '#if defined(_MSC_VER)', '#pragma once', '#endif', '']
        guard = re.sub(r'\W', '_', os.path.basename(name)).upper() + '_H'
        h += ['#ifndef %s%s' % (guard, ('  // ' + tag) if tag.startswith('NAME') else ''), '#define %s' % guard, '',
              '#include "%spk.h"' % ('..\\' if '\\' in name else ''), '']
        for cls, d, ctors in good:
            h += decl(cls, ctors)
        h += ['#endif', '']
        c = ['// Earth & Beyond client (reconstruction): app\\packets\\%s.cpp. Notes at the end of the file.' % name,
             '#include "%s.h"' % os.path.basename(name), '#include <stdio.h>', '']
        funcs = sorted((f, t) for cls, d, ctors in good for f, t in d)
        mk = list(c)
        for f, t in funcs:
            note = NOTEXACT.get(f)
            c += [''] + (['// NOT EXACT: %s' % note] if note else []) + t
            mk += ['', '// @0x%X' % f] + t
        if a.markers:
            os.makedirs(a.markers, exist_ok=True)
            open(os.path.join(a.markers, os.path.basename(name) + '.cpp'), 'wb').write(
                '\r\n'.join(mk + ['']).encode('utf-8'))
        c += ['', '', '/*', '**\tReconstruction notes (%s.cpp)' % os.path.basename(name), '**',
              '**\t* Client 0x%X-0x%X (the object ends with the guarded static every <string> includer has,' % (
                  fl['lo'], fl['hi']),
              '**\t  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (tools/re/pkdecode.py,',
              '**\t  pkemit.py): members from the raw message\'s Get / Build\'s Add (sizes) and Print (types,',
              '**\t  names: \'Name=%d\'), the constructors\' stores in the initialiser list.']
        if tag.startswith('NAME'):
            c.append('**\t* The file name is provisional (%s).' % tag.split(': ', 1)[1])
        rest = [op for op in fl['ops'] if op not in [g[0]['op'] for g in good]]
        if rest:
            c.append('**\t* Not rebuilt yet: opcode %s (declared in pk_classes.h).' % ', '.join(map(str, rest)))
        c += ['*/', '']
        emitted.append((name, [g[0]['op'] for g in good], len(funcs)))
        print('%-40s %s  (%d functions)' % (name, ' '.join(str(g[0]['op']) for g in good), len(funcs)))
        if a.write:
            for ext, lines in (('.h', h), ('.cpp', c)):
                p = os.path.join(PKDIR, name + ext)
                os.makedirs(os.path.dirname(p), exist_ok=True)
                open(p, 'wb').write('\r\n'.join(lines).encode('utf-8'))
    if a.write:
        open(os.path.join(WORK, 'emitted.txt'), 'w').write(
            '\n'.join('%s %s %d' % (n, ','.join(map(str, o)), k) for n, o, k in emitted) + '\n')
    print(len(emitted), 'files')


if __name__ == '__main__':
    main()
