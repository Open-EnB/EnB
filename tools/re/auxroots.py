"""Generate the client's AuxData root classes (M1.3, D43) from the constructors in client.exe.

    python tools/re/auxroots.py generate [--client EXE]     writes app/client/aux_roots.h/.cpp
    python tools/re/auxroots.py check [--schema JSON]       the extracted trees vs the reference decoder's schema
    python tools/re/auxroots.py tree ROOT                   prints one extracted tree

The trees come from tools/re/auxextract.py (symbolic execution of the 11 roots' constructors: the
CREATE factory 0x6F6000's ten and the Player root 0x752510). Each compound the client builds has
its own vtable, so each becomes one C++ class: the roots `Delta<Name>Class` (derived from
UnknownRootClass, unknown_root.h; `<Name>` is the client's string; D56 score 3), the nested
compounds `Delta<X>Class` where tools/re/auxnames.py matched Net-7's `Aux<X>` (or a C# class) by
structure (tools/re/auxroots_names.json; score 4, the evidence in the `// NAME` tag), else
`Unknown<VTABLE>Class` (score 1). Arrays are the client's templates where it has one
(DeltaArrayClass<T>, DeltaItemArrayClass<T>). A compound's fields that sit inside
its object are members (declared in offset order, constructed in the init list); the ones the
client allocates are `new`ed. Children are added in the client's order (the wire order), masks
other than 0x3FF stored after the Add (Ship's narrow Add 0x6F8C70 is a member of its class). Arrays
are classes whose constructor takes the count and adds `new` elements named "%d" (the client's
format 0xB71A6C, its assert 'item' in ..\\shared/auxdata/delta_array.h line 71).

Leaf types by vtable: string DeltaStringClass, float/int/bool/time/money DeltaTemplateClass<T>
(T = float, int, bool, DurationClass, MoneyClass), 0xAEAE54 DeltaInterpolatorClass; every other
leaf vtable gets a value type of its own (enum: `Unknown<VT>Enum`; 12-byte colour, 12/16/20-byte
blobs: `Unknown<VT>Type`) with the conversions KVPairTemplateClass needs.

The output is functional, NOT EXACT (D42): default values (the client's 'Default' strings,
1.0f arrays, the 20-byte blobs' {1.0f, 0, 0, -1, 0}) are not extracted, the nested classes'
out-of-line constructors (the 0x700xxx / 0x75xxxx objects) are inline here, and Player derives
from UnknownRootClass where the client uses the second root class (vtable 0xAEAC40, ctor 0x4A3FF0).
"""
import argparse
import json
import os
import re
import struct
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(os.path.dirname(HERE))
sys.path.insert(0, HERE)
CLIENT = r'C:\Program Files (x86)\EA GAMES\Earth & Beyond\release\client.exe'
SCHEMA = r'C:\EnBDevelopment\the reference decoder\src\Shared\the reference game model\Schema\client-auxdata.json'
OUT_DIR = os.path.join(REPO, 'app', 'client')
NAMES = os.path.join(HERE, 'auxroots_names.json')     # tools/re/auxnames.py

# The factory's create types (0x6F6000's switch) and the roots it builds; Player is game id 0.
ROOT_CTOR = {'Ship': 0x6F66E0, 'Component': 0x6FBDA0, 'Light': 0x6F9870, 'Decoration': 0x6F9C30,
             'Harvestable': 0x6F9ED0, 'ManufacturingLab': 0x6FA6F0, 'Player': 0x752510}
INLINE_ROOT_VT = {'Planet': 0xB00894, 'Stargate': 0xB007DC, 'Starbase': 0xB00838}   # built in the factory

LEAF = {
    0xAEB0E4: ('DeltaStringClass', None),
    0xAEAEB0: ('DeltaTemplateClass<float>', None),
    0xB00D34: ('DeltaTemplateClass<int>', None),
    0xB00D8C: ('DeltaTemplateClass<bool>', None),
    0xAEB1EC: ('DeltaTemplateClass<DurationClass>', None),
    0xAEAFB8: ('DeltaTemplateClass<MoneyClass>', None),
}
INTERPOLATOR = 0xAEAE54
# Value types whose constructor the client shows (the leaf constructors' default argument, the Reads'
# local): the colour's does nothing (a stack temporary is copied), 0x705640's T starts as
# {1.0f, 0, 0, -1, 0}, 0x757B00's 16 bytes are zeroed.
BLOB_FORMS = {
    0xB00A00: ('three floats (a colour), constructor empty',
               'float R; float G; float B; NAME() {} NAME(const NAME &v) { R = v.R; G = v.G; B = v.B; }'),
    0xB01A40: ('20 bytes, constructed {1.0f, 0, 0, -1, 0} (0x705640)',
               'float F00; int I04; int I08; int I0C; int I10; NAME() : F00(1.0f), I04(0), I08(0), I0C(-1), I10(0) {}'),
    0xB03694: ('16 bytes, zeroed (0x757B00)',
               'int I00; int I04; int I08; int I0C; NAME() : I00(0), I04(0), I08(0), I0C(0) {}'),
}


def extract(client):
    import auxextract as X
    tmp = os.path.join(REPO, '.work', 'auxroots.json')
    os.makedirs(os.path.dirname(tmp), exist_ok=True)
    X.main(client, tmp)
    return json.load(open(tmp))


def strip(x):
    if isinstance(x, dict):
        return {k: strip(v) for k, v in x.items() if k not in ('vt', 'ctors', 'at', 'default', 'value')}
    if isinstance(x, list):
        return [strip(v) for v in x]
    return x


def ident(name):
    s = re.sub(r'\W', '_', name)
    return 'm_' + s if s[:1].isdigit() else s


def hexu(v):
    return f'{v:X}'


class Gen:
    def __init__(self, doc):
        self.hints = json.load(open(NAMES)) if os.path.exists(NAMES) else {}
        self.tags = {}
        self.doc = doc
        self.types = {}        # vt -> (kind, size)  value types of their own
        self.classes = {}      # vt -> class name (compounds)
        self.shape = {}        # vt -> shape signature
        self.order = []        # compound vts, children before parents
        self.decl = {}         # vt -> declaration text
        self.notes = {}        # vt -> example path
        self.instances = {}    # array template instance -> vt

    # ---- leaves

    def leaf_class(self, n):
        vt = int(n['vt'], 16)
        if vt in LEAF:
            return LEAF[vt][0]
        t = n['type']
        if t == 'enum':
            name = f'Unknown{hexu(vt)}Enum'
            self.types[vt] = ('enum', 4, name)
        else:
            size = 12 if t == 'color' else int(t[5:])
            name = f'Unknown{hexu(vt)}Type'
            self.types[vt] = ('blob', size, name)
        return f'DeltaTemplateClass<{name}>'

    def name_for(self, n, vt, root):
        """A compound's class name and its D56 tag (tools/re/auxnames.py's structure matches)."""
        h = self.hints.get(f'{vt:#x}') or (self.hints.get(root) if root else None) or {}
        n7, cs, ev = h.get('net7'), h.get('cs'), h.get('evidence', '')
        both = ' / '.join(x for x in (f'Net-7 {n7}' if n7 else '', f'C# {cs}' if cs else '') if x)
        if root:
            return (f'Delta{root}Class', f'// NAME 3: the client\'s root string "{root}", Westwood XxxClass (D24); '
                    f'structure = {both or "-"} ({ev or "no match"})')
        if n7 and '/' not in n7 and '!=' not in ev:
            return (f'Delta{n7[3:]}Class', f'// NAME 4: Net-7 {n7} confirmed by structure ({ev})'
                    + (f'; C# {cs}' if cs else ''))
        if n7 and '/' not in n7:
            return (f'Delta{n7[3:]}Class', f'// NAME 3: Net-7 {n7} at the same place in the tree, its child count '
                    f'differs ({ev})' + (f'; C# {cs}' if cs else ''))
        if cs:
            return (f'Delta{cs}Class', f'// NAME 4: C# {cs} confirmed by structure ({ev})')
        return (f'Unknown{hexu(vt)}Class', f'// NAME 1: neutral (no Net-7 or C# match), vtable 0x{hexu(vt)}')

    @staticmethod
    def literal(f, cls):
        """The C++ for a leaf's default value (the client's store at +0x84 or its ctor argument)."""
        v = f['value']
        if f['type'] == 'float':
            x = struct.unpack('<f', struct.pack('<I', v))[0]
            return repr(x) + 'f' if '.' in repr(x) or 'e' in repr(x) else repr(x) + '.0f'
        i = v - (1 << 32) if v & 0x80000000 else v
        if f['type'] == 'enum':
            return f'({cls[len("DeltaTemplateClass<"):-1]}){i}'
        return str(i)

    # ---- compounds

    def sig(self, n):
        if n['type'] == 'struct':
            return ('S', n.get('vt'), tuple((f['name'], self.sig(f), f.get('mask')) for f in n['fields']))
        if n['type'] == 'array':
            return ('A', n.get('vt'), self.sig(n['element']))
        return ('L', n.get('vt'))

    @staticmethod
    def alloc(n):
        a = int(n.get('at', '0'), 16)
        return (a & ~0xFFFFF, a & 0xFFFFF) if a >= 0x10000000 else (0, a)

    def node_class(self, n, path):
        if n['type'] in ('struct', 'array'):
            vt = int(n['vt'], 16)
            if vt == INTERPOLATOR:
                return 'DeltaInterpolatorClass'
            return self.compound(n, path)
        return self.leaf_class(n)

    def compound(self, n, path, root=None):
        vt = int(n['vt'], 16)
        s = self.sig(n)
        if vt in self.shape:
            if self.shape[vt] != s:
                raise SystemExit(f'{path}: vtable {vt:#x} has two shapes')
            return self.classes[vt]
        self.shape[vt] = s
        name, tag = self.name_for(n, vt, root)
        self.classes[vt] = name
        self.tags[vt] = tag
        self.notes[vt] = path
        if n['type'] == 'array':
            el = n['element']
            ecls = self.node_class(el, path + '.N')
            # the client's templates: delta_array.h (leaf elements made with the array's value),
            # delta_item_array.h (compound elements), one vtable per instance
            inst = None
            if ecls.startswith('DeltaTemplateClass<') and el.get('mask') is None:
                inst = f'DeltaArrayClass<{ecls[len("DeltaTemplateClass<"):-1]}>'
            elif el['type'] == 'struct' and ecls != 'DeltaInterpolatorClass' and el.get('mask') is None:
                inst = f'DeltaItemArrayClass<{ecls}>'
            if inst is not None and self.instances.setdefault(inst, vt) == vt:
                self.classes[vt] = inst
                return inst
            lines = [f'// vtable 0x{hexu(vt)}: an array of {ecls} (e.g. {path}, {n["count"]} elements)', tag,
                     f'class {name} : public DeltaCompoundClass', '{', 'public:',
                     f'\t{name}(const char *name, int count) : DeltaCompoundClass(name)', '\t{',
                     '\t\tfor (int i = 0; i < count; i++) {',
                     '\t\t\tchar item_name[16];',
                     '\t\t\tsprintf(item_name, "%d", i);',
                     f'\t\t\t{ecls} *item = new {ecls}(item_name' + (', ""' if ecls == 'DeltaStringClass' else '') + ');',
                     '\t\t\tif (item == 0) {', '\t\t\t\tG_ASSERT(item);', '\t\t\t\tbreak;', '\t\t\t}']
            if el.get('mask') is not None:
                lines.append(f'\t\t\titem->m_Unknown80 = 0x{el["mask"]:X};')
            lines += ['\t\t\tAdd(item);', '\t\t}', '\t}', '};']
            self.decl[vt] = '\n'.join(lines)
            self.order.append(vt)
            return name
        # a struct: members (inside the object) in offset order, the rest new'd
        base_alloc, base_off = self.alloc(n)
        members, body = [], []
        ship = root == 'Ship'
        for i, f in enumerate(n['fields']):
            cls = self.node_class(f, path + '.' + f['name'])
            fa, fo = self.alloc(f)
            arg = f'"{f["name"]}"'
            ctor_args = arg + (f', {f["count"]}' if f['type'] == 'array' else '')
            if cls == 'DeltaStringClass':
                ctor_args += ', "' + f.get('default', '').replace('\\', '\\\\').replace('"', '\\"') + '"'
            elif 'value' in f:
                ctor_args += ', ' + self.literal(f, cls)
            mask = f.get('mask')
            if fa == base_alloc and fo > base_off:
                m = ident(f['name'])
                members.append((fo - base_off, cls, m, ctor_args))
                ref = '&' + m
            else:
                ref = f'new {cls}({ctor_args})'
            if ship:
                body.append(f'\tAdd({1 if mask == 0xCF else 0}, {ref});')
                if mask not in (None, 0xCF):
                    raise SystemExit(f'{path}.{f["name"]}: mask {mask:#x}')
            elif mask is None:
                body.append(f'\tAdd({ref});')
            else:
                if ref.startswith('&'):
                    body.append(f'\tAdd({ref});')
                    body.append(f'\t{ref[1:]}.m_Unknown80 = 0x{mask:X};')
                else:
                    body.append(f'\t{{ DeltaClass *field = {ref}; Add(field); field->m_Unknown80 = 0x{mask:X}; }}')
        members.sort()
        offs = [m[0] for m in members]
        if len(set(offs)) != len(offs):
            raise SystemExit(f'{path}: two members at one offset')
        if root:
            init = [f'UnknownRootClass(unknown, "{n["_cname"]}")']
            sig = f'{name}(int unknown)'
            basecls = 'UnknownRootClass'
        else:
            init = ['DeltaCompoundClass(name)']
            sig = f'{name}(const char *name)'
            basecls = 'DeltaCompoundClass'
        init += [f'{m}({a})' for _, _, m, a in members]
        lines = [f'// vtable 0x{hexu(vt)}' + (f', constructor 0x{ROOT_CTOR[root]:X}' if root in ROOT_CTOR else '')
                 + ('' if root else f' (e.g. {path})'), tag,
                 f'class {name} : public {basecls}', '{', 'public:']
        inline = root in INLINE_ROOT_VT or root == 'Hulk'
        if inline:
            body = body + ['\tDeltaCompoundClass::Unknown_48();']
        if root and not inline:
            lines.append(f'\t{sig};')
            if ship:
                lines.append('\tvoid Add(DeltaClass *child) { DeltaCompoundClass::Add(child); }')
                lines.append('\tvoid Add(int narrow, DeltaClass *child);\t// 0x6F8C70: narrow sets the mask to 0xCF')
        else:
            lines.append(f'\t{sig} :')
            lines.append('\t\t' + ',\n\t\t'.join(init))
            lines.append('\t{')
            lines += ['\t' + b for b in body]
            lines.append('\t}')
        if members:
            lines.append('')
        for off, cls, m, _ in members:
            lines.append(f'\t{cls} {m};\t// +0x{off:X}')
        lines.append('};')
        self.decl[vt] = '\n'.join(lines)
        if root and not inline:
            ctor = [f'{name}::{sig} :', '\t' + ',\n\t'.join(init), '{'] + body + \
                   ['\tDeltaCompoundClass::Unknown_48();', '}']
            self.root_ctor = '\n'.join(ctor)
        self.order.append(vt)
        return name

    def type_decls(self):
        out = []
        for vt, (kind, size, name) in sorted(self.types.items()):
            out.append(f'// NAME 1: the value type of the leaf vtable 0x{hexu(vt)} (no name in the client, Net-7 or C#)')
            if kind == 'enum':
                out.append(f'enum {name} {{ UNKNOWN_{hexu(vt)}_0 }};\t// vtable 0x{hexu(vt)}, 4 bytes')
            else:
                if vt in BLOB_FORMS:
                    what, body = BLOB_FORMS[vt]
                    out.append(f'struct {name} {{ {body.replace("NAME", name)} }};\t// vtable 0x{hexu(vt)}, {what}')
                else:
                    out.append(f'struct {name} {{ unsigned char Data[{size}]; {name}() {{ memset(Data, 0, sizeof(Data)); }} }};'
                               f'\t// vtable 0x{hexu(vt)}, {size} bytes (zeroed: a guess)')
                out.append(f'inline bool operator!=(const {name} &a, const {name} &b) '
                           '{ return memcmp(&a, &b, sizeof(a)) != 0; }')
            ref = name if kind == 'enum' else f'const {name} &'
            out.append(f'inline const char *Type_Name({name} *) {{ return "{name}"; }}')
            if kind == 'enum':
                out.append(f'inline char *Value_To_String(char *buf, int size, {name} value) '
                           '{ _snprintf(buf, size, "%d", (int)value); return buf; }')
                out.append(f'inline bool String_To_Value(const char *text, {name} *value) '
                           '{ int v; if (sscanf(text, "%d", &v) != 1) return false; '
                           f'*value = ({name})v; return true; }}')
            else:
                out.append(f'inline char *Value_To_String(char *buf, int size, {ref}value) '
                           '{ return Unknown_Hex_String(buf, size, (const unsigned char *)&value, sizeof(value)); }')
                out.append(f'inline bool String_To_Value(const char *, {name} *) {{ return false; }}')
        return '\n'.join(out)


HEADER = r'''/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	GENERATED by tools/re/auxroots.py from client.exe (SHA-256 249f8524...51c4): do not edit.
**	Original location: the roots' constructors are in app\client\co.cpp (0x6F6000-0x6FBEF0) and,
**	for Player, an unattributed object (0x751BE0-0x75A4E0); the nested classes' out-of-line
**	constructors in the objects after them (0x7008B0-0x70A010, 0x75xxxx). File name PROVISIONAL.
**
**	// NAME 1 [aux_roots.h]: provisional file name (the client's code is in co.cpp and an unattributed object)
**	The aux-data root classes (M1.3, D43): one class per compound vtable, fields in the client's
**	order. NOT EXACT (D42): functional; see the generator's docstring for what differs.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __AUX_ROOTS_H
#define __AUX_ROOTS_H

#include <stdio.h>
#include <string.h>
#include "..\shared\auxdata\unknown_root.h"
#include "..\shared\auxdata\delta_template.h"
#include "..\shared\auxdata\delta_string.h"
#include "..\shared\auxdata\delta_interpolator.h"
#include "..\shared\auxdata\delta_array.h"
#include "..\shared\auxdata\delta_item_array.h"
#include "..\shared\money.h"
#include "..\shared\strconv_lib.h"
#include "..\shared\unknown_debug.h"

// The money field's Read and Write are delta_money.cpp's specialisations
template<> bool DeltaTemplateClass<MoneyClass>::Read(LinearBufferClass &buf, const DurationClass &);
template<> bool DeltaTemplateClass<MoneyClass>::Write(LinearBufferClass &buf);

// ------------------------------------------------------------------------- value types

// The client's conversions for int and bool are strconv_lib.cpp's (0x4A30A0-0x4A35F0, not rebuilt)
inline const char *Type_Name(int *) { return "int"; }
inline char *Value_To_String(char *buf, int size, int value) { _snprintf(buf, size, "%d", value); return buf; }
inline bool String_To_Value(const char *text, int *value) { return sscanf(text, "%d", value) == 1; }
inline const char *Type_Name(bool *) { return "bool"; }
inline char *Value_To_String(char *buf, int size, bool value) { _snprintf(buf, size, "%s", value ? "true" : "false"); return buf; }
inline bool String_To_Value(const char *text, bool *value) { *value = strcmp(text, "true") == 0; return true; }
inline char *Unknown_Hex_String(char *buf, int size, const unsigned char *data, int n)
{
	int k = 0;
	for (int i = 0; i < n && k + 3 <= size; i++) {
		k += _snprintf(buf + k, size - k, "%02x", data[i]);
	}
	if (k < size) {
		buf[k] = 0;
	}
	return buf;
}

%(types)s

// ------------------------------------------------------------------------- the compounds

%(classes)s

// ------------------------------------------------------------------------- the factory

// 0x6F6000 (co.cpp): the root for a CREATE's type, or 0 when the type has no aux data
UnknownRootClass *Unknown_Create_Aux_Root(int type, int unknown);

#endif //__AUX_ROOTS_H
'''

SOURCE = r'''// Earth & Beyond client (reconstruction): GENERATED by tools/re/auxroots.py, do not edit.
// The aux-data roots' constructors and the CREATE factory (M1.3, D43). NOT EXACT (D42): functional.
// NAME 1 [aux_roots.cpp]: provisional file name (the client's constructors are in co.cpp 0x6F6000-0x6FBEF0 and the Player object 0x751BE0)
#include "aux_roots.h"

%(ctors)s

void DeltaShipClass::Add(int narrow, DeltaClass *child)
{
	DeltaCompoundClass::Add(child);
	if (narrow == 1) {
		child->m_Unknown80 = 0xCF;
	}
}

UnknownRootClass *Unknown_Create_Aux_Root(int type, int unknown)
{
	switch (type) {
%(cases)s
	}
	return 0;
}
'''


def generate(doc, out_dir):
    g = Gen(doc)
    ctors = []
    roots = doc['roots']
    for rname, r in roots.items():
        r = dict(r)
        if rname in INLINE_ROOT_VT:
            r['vt'] = f'{INLINE_ROOT_VT[rname]:#x}'
            r['at'] = '0x0'
            r['fields'] = [dict(r['fields'][0], vt='0xaeb0e4', at='0xa0')]
        if 'vt' not in r:
            raise SystemExit(f'{rname}: no vtable')
        r['_cname'] = rname
        g.root_ctor = None
        g.compound(r, rname, root=rname)
        if g.root_ctor is None:
            continue
        ctors.append(f'// {rname}' + (f' (0x{ROOT_CTOR[rname]:X})' if rname in ROOT_CTOR else ' (built in the factory 0x6F6000)')
                     + '\n' + g.root_ctor)
    by_root = {}
    for t, rn in doc['createTypes'].items():
        by_root.setdefault(rn, []).append(int(t))
    cases = []
    # the client's source order of the cases (the factory's EH states rise in it)
    order = ['Ship', 'Hulk', 'Planet', 'Starbase', 'Light', 'Decoration', 'Stargate', 'ManufacturingLab',
             'Harvestable', 'Component']
    for rn, ts in sorted(by_root.items(), key=lambda kv: order.index(kv[0]) if kv[0] in order else 99):
        cases += [f'\t\tcase {t}:' for t in sorted(ts)]
        cases.append(f'\t\t\treturn new Delta{rn}Class(unknown);')
    classes = '\n\n'.join(g.decl[vt] for vt in g.order)
    h = HEADER.replace('%(types)s', g.type_decls()).replace('%(classes)s', classes)
    c = SOURCE.replace('%(ctors)s', '\n\n'.join(ctors)).replace('%(cases)s', '\n'.join(cases))
    os.makedirs(out_dir, exist_ok=True)
    for name, text in (('aux_roots.h', h), ('aux_roots.cpp', c)):
        with open(os.path.join(out_dir, name), 'w', newline='\r\n') as f:
            f.write(text)
    print(f'{len(g.classes)} classes ({len(roots)} roots), {len(g.types)} value types -> {out_dir}')


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('cmd', choices=['generate', 'check', 'tree'])
    ap.add_argument('root', nargs='?')
    ap.add_argument('--client', default=CLIENT)
    ap.add_argument('--schema', default=SCHEMA)
    ap.add_argument('--out', default=OUT_DIR)
    a = ap.parse_args()
    doc = extract(a.client)
    if a.cmd == 'check':
        n7 = json.load(open(a.schema))
        same = strip(doc['roots']) == n7['roots'] and doc['createTypes'] == n7['createTypes']
        print('extracted trees equal', a.schema if same else 'DIFFER from ' + a.schema)
        raise SystemExit(0 if same else 1)
    if a.cmd == 'tree':
        def show(x, ind=0):
            print(' ' * ind + f"{x['name']} {x['type']} vt={x.get('vt')} at={x.get('at')} ctors={x.get('ctors')}"
                  f" count={x.get('count', '')} mask={x.get('mask', '')}")
            for f in x.get('fields', []):
                show(f, ind + 2)
            if 'element' in x:
                show(x['element'], ind + 2)
        show(doc['roots'][a.root])
        return
    generate(doc, a.out)


if __name__ == '__main__':
    main()
