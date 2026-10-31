"""Generate the client's AuxData root classes (M1.3, D43) from the constructors in client.exe.

    python tools/re/auxroots.py generate [--client EXE]     writes app/client/aux_roots.h/.cpp
    python tools/re/auxroots.py schema [--schema JSON]      writes the extracted trees as docs/data/client-auxdata.json
    python tools/re/auxroots.py check [--schema JSON]       the extracted trees vs that committed schema
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
the 20-byte blobs' {1.0f, 0, 0, -1, 0}) are not extracted (ARRAY_VALUES has the 1.0f arrays'),
the factory's roots' constructors are inline in the header (the note at SEND_ROOTS), the nested classes'
out-of-line constructors are generated out of line where the client has them (a compound whose
recorded constructor stores its own vtable last: 0x6FB1E0, 0x752E60, ...) as `inline` definitions
(the client's are copies of inline functions, each right after its first caller; OOLINE_PLAIN
lists the exception), and a compound whose
constructor calls another compound's (Equipped's items: 0x6FA150 builds a Cargo item) derives from
that class. Player derives from
the second root class UnknownSendRootClass (vtable 0xAEAC40, ctor 0x4A3FF0) and calls its
Unknown_48 (0x4A40B0: the policy and the change table).
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
CLIENT = os.path.join(os.environ.get('ENB_RETAIL_INSTALL') or r'C:\Program Files (x86)\EA GAMES\Earth & Beyond', 'release', 'client.exe')
import roots  # noqa: E402
SCHEMA = os.path.join(REPO, 'docs', 'data', 'client-auxdata.json')
OUT_DIR = os.path.join(REPO, 'app', 'client')
NAMES = os.path.join(HERE, 'auxroots_names.json')     # tools/re/auxnames.py
# Members the extracted trees don't show (C++ members that are no fields), by class vtable: the
# offset and the number of ints, each set to 0 in the init list. Found by `auxroots.py layout`
# (the client's field offsets against the compiled ones) and checked in the client's stores.
PADS = os.path.join(HERE, 'auxroots_pads.json')

# The factory's create types (0x6F6000's switch) and the roots it builds; Player is game id 0.
ROOT_CTOR = {'Ship': 0x6F66E0, 'Component': 0x6FBDA0, 'Light': 0x6F9870, 'Decoration': 0x6F9C30,
             'Harvestable': 0x6F9ED0, 'ManufacturingLab': 0x6FA6F0, 'Player': 0x752510}
SEND_ROOTS = {'Player'}     # derived from UnknownSendRootClass (unknown_root.h)
# The factory's roots' constructors are inline functions in the header (the rest: SEND_ROOTS, in
# aux_roots.cpp): the client's out-of-line copies follow the factory 0x6F6000, their first caller
# (Ship 0x6F66E0 right after it), and the factory is exact only so (with 1.0f in ARRAY_VALUES).
# Nested constructors generated out of line and NOT inline (the rest are `inline`: see Gen.compound).
# Element (0x707430): the client's is inline too, but its only caller is the Buff array 0x707040,
# into which ours does not inline the Elements array's base (ARRAY_DERIVED; the client does), so
# an inline Element is lost there (0x707430); plain, it stays exact.
OOLINE_PLAIN = {0xB01F74}
# Arrays whose class derives from the template instance: the client calls the instance's
# constructor and then stores the array's own vtable and per-class global (Ship's BuffArray:
# 0x707040 stores 0xB01A98, Ship 0x6F73DA then 0xB00B0C and [0xC41600]; the two vtables differ
# only in the destructor slot). Array vtable -> (instance vtable, instance constructor).
ARRAY_DERIVED = {0xB00B0C: (0xB01A98, 0x707040),
                 0xB01EBC: (0xB01F18, None)}     # the Buff's Elements (0x7071F7, 0x707292: the base inlined)
# The value arrays' elements start from (delta_array.h's third argument), which auxextract doesn't
# record (a stack temporary): VisibleQuadrantDamagePercent's 1.0f (Ship 0x6F700D, Hulk 0x6F60FD).
ARRAY_VALUES = {'VisibleQuadrantDamagePercent': '1.0f'}
# Leaf defaults passed through a stack temporary (also not recorded), by (compound vtable, field):
# the equipped item's effect's Validity 0x100 (0x70245B, in the Equipped array 0x702240).
LEAF_VALUES = {(0xB01790, 'Validity'): 0x100}
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
    0xB01A40: ('20 bytes, constructed {1.0f, 0.0f, 0.0f, -1, 0.0f} (0x705640; Ship 0x6F70BE stores the zeros as immediates among register zeros: floats, workflow.md 3.1)',
               'float F00; float F04; float F08; int I0C; float F10; NAME() : F00(1.0f), F04(0.0f), F08(0.0f), I0C(-1), F10(0.0f) {} '
               'NAME(const NAME &v) { F00 = v.F00; F04 = v.F04; F08 = v.F08; I0C = v.I0C; F10 = v.F10; } '
               'NAME &operator=(const NAME &v) { F00 = v.F00; F04 = v.F04; F08 = v.F08; I0C = v.I0C; F10 = v.F10; return *this; }'),
    0xB016DC: ('12 bytes, constructed {-1, -1, -1} (the Mounts elements\' default, Ship 0x6F6E29)',
               'int I00; int I04; int I08; NAME() : I00(-1), I04(-1), I08(-1) {}'),
    0xB03694: ('16 bytes, zeroed (0x757B00)',
               'int I00; int I04; int I08; int I0C; NAME() : I00(0), I04(0), I08(0), I0C(0) {}'),
}
# The client's KVPair conversions of a blob type, where known (the rest compare with memcmp and print
# hex, functional only): vtable -> (type name, comparison, Value_To_String body, String_To_Value body).
# 0xB01A40: the KVPair's vtable 0xB01D00 (its destructor 0x7095B0): Get_Type_Name 0x708C60 returns
# 'ProjectedDecalClass', Get_Value_String 0x708C80 sprintf's '%f,%f,%f,%d,%f' (the size unused),
# Is_Changed 0x708CF0 the fields' == negated, Update_Saved 0x708D80 a field-by-field copy (operator=).
# Is_Changed is exact with a separate operator== and `!(a == b)`, but that form leaves no SYMBOL_PAD
# window for the six constructors (AR1 2026-10-31: 5/6 at best); the one-expression != keeps pad 15.
BLOB_CONV = {
    0xB01A40: ('ProjectedDecalClass',
               'inline bool operator!=(const NAME &a, const NAME &b) '
               '{ return !(a.F00 == b.F00 && a.F04 == b.F04 && a.F08 == b.F08 && a.I0C == b.I0C && a.F10 == b.F10); }',
               '{ sprintf(buf, "%f,%f,%f,%d,%f", value.F00, value.F04, value.F08, value.I0C, value.F10); return buf; }',
               None),
}


def extract(client):
    import auxextract as X
    tmp = os.path.join(REPO, '.work', 'auxroots.json')
    os.makedirs(os.path.dirname(tmp), exist_ok=True)
    X.main(client, tmp)
    return json.load(open(tmp))


def strip(x):
    if isinstance(x, dict):
        return {k: strip(v) for k, v in x.items() if k not in ('vt', 'ctors', 'at', 'default', 'value', 'rmask')}
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
        self.nodes = {}        # vt -> an extracted node (compounds)
        self.ooline = {}       # vt -> the client's out-of-line constructor of the class
        self.base = {}         # vt -> the vt of its base class (a compound class it derives from)
        self.nested = []       # (address, text): out-of-line constructors for aux_roots.cpp
        self.img = None
        self.pads = {int(k, 16): v for k, v in json.load(open(PADS)).items()} if os.path.exists(PADS) else {}
        self.layout = {}       # vt -> (class name, [(client offset, member)]) for `layout`

    # ---- the client's constructors

    # constructors shared by many nodes (the root bases, DeltaCompoundClass's out-of-line copy
    # 0x6F85F0, the interpolator's): not a class's own
    SHARED = {0x4A3BD0, 0x4A5170, 0x4A3FF0, 0x4A40B0, 0x6F85F0, 0x4A6110}

    def final_vt(self, addr):
        """The last compound vtable a constructor stores (its class's, when it is the class's own)."""
        if self.img is None:
            from clientimage import ClientImage
            self.img = ClientImage(CLIENT)
        last = None
        for ins in self.img.disasm(addr, 0x2000):
            if ins.mnemonic == 'mov' and ins.op_str.startswith('dword ptr [e') and ', 0x' in ins.op_str \
                    and ins.op_str.split('], ')[0].count('+') == 0:
                v = int(ins.op_str.split(', ')[-1], 16)
                if v in self.nodes:
                    last = v
            if ins.mnemonic == 'ret':
                break
        return last

    def prepare(self, roots):
        def walk(n, top):
            if n.get('type') in ('struct', 'array') and n.get('vt'):
                vt = int(n['vt'], 16)
                if not top:
                    self.nodes.setdefault(vt, n)
            for f in n.get('fields', []):
                walk(f, False)
            if 'element' in n:
                walk(n['element'], False)
        for r in roots.values():
            walk(r, True)
        for vt, n in self.nodes.items():
            if vt == INTERPOLATOR:
                continue
            for c in (int(x, 16) for x in n.get('ctors', [])):
                if c in self.SHARED:
                    continue
                fvt = self.final_vt(c)
                if fvt == vt:
                    self.ooline[vt] = c
                elif fvt is not None and n['type'] == 'struct' and self.nodes[fvt]['type'] == 'struct':
                    self.base[vt] = fvt
                    self.ooline[fvt] = c

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
        if f['type'] == 'bool':
            return 'true' if v else 'false'
        if f['type'] == 'money':
            return f'MoneyClass({i})'
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
            if inst is not None and vt in ARRAY_DERIVED:
                # a class of its own derived from the template instance (see ARRAY_DERIVED)
                base_vt, base_ctor = ARRAY_DERIVED[vt]
                if self.instances.setdefault(inst, base_vt) != base_vt:
                    raise SystemExit(f'{path}: {inst} has two vtables')
                lines = [f'// vtable 0x{hexu(vt)}: {inst} (vtable 0x{hexu(base_vt)}'
                         + (f', constructor 0x{hexu(base_ctor)}' if base_ctor else '') + ') '
                         f'with a vtable and per-class global of its own (e.g. {path}): the client builds the base, '
                         'then stores the two', tag,
                         f'class {name} : public {inst}', '{', 'public:',
                         f'\t{name}(const char *name, unsigned int count) : {inst}(name, count)', '\t{',
                         '\t\tUnknown04 = UnknownGlobal;', '\t}', '',
                         '\tstatic int UnknownGlobal;\t// the per-class global at +0x04', '};']
                self.decl[vt] = '\n'.join(lines)
                self.order.append(vt)
                self.nested.append((0, f'int {name}::UnknownGlobal;'))
                return name
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
                lines.append(f'\t\t\titem->Unknown80 = 0x{el["mask"]:X};')
            # (a do-while here, as delta_item_array.h's, and an out-of-line constructor (0x701B60)
            # gain 0x7014B0 and 0x7045F0 but lose 0x6FB1E0: VC6's inlining elsewhere in the file moves)
            lines += ['\t\t\tAdd(item);', '\t\t}', '\t}', '};']
            self.decl[vt] = '\n'.join(lines)
            self.order.append(vt)
            return name
        # a struct: members (inside the object) in offset order, the rest new'd
        base_alloc, base_off = self.alloc(n)
        members, body = [], []
        ship = root == 'Ship'
        fields, bcls = n['fields'], None
        late = []
        if vt in self.base:
            # the client builds the first fields through the base class's constructor
            bn = self.nodes[self.base[vt]]
            bcls = self.compound(bn, path)
            k = len(bn['fields'])
            if [f['name'] for f in fields[:k]] != [f['name'] for f in bn['fields']]:
                raise SystemExit(f'{path}: the base class {bcls} is not its first fields')
            fields = fields[k:]
        for i, f in enumerate(fields):
            if (vt, f['name']) in LEAF_VALUES:
                f = dict(f, value=LEAF_VALUES[vt, f['name']])
            cls = self.node_class(f, path + '.' + f['name'])
            fa, fo = self.alloc(f)
            arg = f'"{f["name"]}"'
            ctor_args = arg + (f', {f["count"]}' if f['type'] == 'array' else '')
            if f['type'] == 'array' and f['name'] in ARRAY_VALUES:
                ctor_args += ', ' + ARRAY_VALUES[f['name']]
            if cls == 'DeltaStringClass':
                ctor_args += ', "' + f.get('default', '').replace('\\', '\\\\').replace('"', '\\"') + '"'
            elif 'value' in f and not (f['type'] == 'money' and f['value'] == 0):
                ctor_args += ', ' + self.literal(f, cls)
            elif cls == 'DeltaTemplateClass<MoneyClass>':
                # a zeroed temporary (MoneyClass() leaves it as it is)
                ctor_args += ', MoneyClass(0)'
            elif cls == 'DeltaTemplateClass<DurationClass>':
                # the client passes a zeroed time (two zero stores into the temporary, 0x707430)
                ctor_args += ', 0'
            mask = f.get('mask', f.get('rmask'))
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
                if ref.startswith('&') and 'mask' not in f:
                    # a mask from a register: the client stores it after the last Add (Hulk 0x6F6000)
                    body.append(f'\tAdd({ref});')
                    late.append(f'\t{ref[1:]}.Unknown80 = 0x{mask:X};')
                elif ref.startswith('&'):
                    body.append(f'\tAdd({ref});')
                    body.append(f'\t{ref[1:]}.Unknown80 = 0x{mask:X};')
                else:
                    body.append(f'\t{{ DeltaClass *field = {ref}; Add(field); field->Unknown80 = 0x{mask:X}; }}')
        body += late
        self.layout[vt] = (name, [(off, m) for off, _, m, _ in members])
        for off, count in self.pads.get(vt, []):
            for k in range(count):
                members.append((off + 4 * k, 'int', f'Unknown{off + 4 * k:X}', '0'))
        members.sort()
        offs = [m[0] for m in members]
        if len(set(offs)) != len(offs):
            raise SystemExit(f'{path}: two members at one offset')
        if root:
            # Player's base is the second root class (vtable 0xAEAC40, ctor 0x4A3FF0: the name
            # argument is passed but not used); the factory's roots derive from UnknownRootClass
            basecls = 'UnknownSendRootClass' if root in SEND_ROOTS else 'UnknownRootClass'
            if root in SEND_ROOTS:
                # Player's constructor takes nothing (0x752510 ends `ret`) and passes 0
                init = [f'{basecls}(0, "{n["_cname"]}")']
                sig = f'{name}()'
            else:
                init = [f'{basecls}(unknown, "{n["_cname"]}")']
                sig = f'{name}(int unknown)'
        else:
            basecls = bcls or 'DeltaCompoundClass'
            init = [f'{basecls}(name)']
            sig = f'{name}(const char *name)'
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
        elif vt in self.ooline:
            # the client's out-of-line copy of an inline constructor: VC6 emits it right after its
            # first caller that didn't inline it (0x6FA150 after Harvestable 0x6F9ED0, 0x6FB1E0
            # after ManufacturingLab 0x6FA6F0, 0x707430 after the Buff array 0x707040), and an
            # inline callee changes the caller's inline budget (Player 0x752510 exact only so).
            # Not Element's (OOLINE_PLAIN): its caller, the Buff array, is inlined into ours.
            lines.append(f'\t{sig};\t// 0x{self.ooline[vt]:X}')
            self.nested.append((self.ooline[vt], '\n'.join(
                [f'// {path} (0x{self.ooline[vt]:X})'] + ([] if vt in OOLINE_PLAIN else ['inline']) +
                [f'{name}::{sig} :', '\t' + ',\n\t'.join(init), '{'] + body + ['}'])))
        else:
            lines.append(f'\t{sig} :')
            lines.append('\t\t' + ',\n\t\t'.join(init))
            lines.append('\t{')
            lines += ['\t' + b for b in body]
            lines.append('\t}')
        if members:
            lines.append('')
        for off, cls, m, _ in members:
            lines.append(f'\t{cls} {m};\t// +0x{off:X}' + ('  // NAME 1: neutral (D10): a member, not a field of the tree'
                                                          if cls == 'int' else ''))
        lines.append('};')
        self.decl[vt] = '\n'.join(lines)
        if root and not inline:
            # the client calls the class's own Unknown_48 (0x4A40B0 for Player, 0x4A5170 otherwise)
            ctor = [f'{name}::{sig} :', '\t' + ',\n\t'.join(init), '{'] + body + \
                   ['\tUnknown_48();' if root in SEND_ROOTS else '\tDeltaCompoundClass::Unknown_48();', '}']
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
                conv = BLOB_CONV.get(vt)
                if conv:
                    out.append(conv[1].replace('NAME', name))
                else:
                    out.append(f'inline bool operator!=(const {name} &a, const {name} &b) '
                               '{ return memcmp(&a, &b, sizeof(a)) != 0; }')
            conv = BLOB_CONV.get(vt) if kind != 'enum' else None
            ref = name if kind == 'enum' else f'const {name} &'
            out.append(f'inline const char *Type_Name({name} *) {{ return "{conv[0] if conv else name}"; }}')
            if conv:
                out.append(f'inline char *Value_To_String(char *buf, int, {ref}value) {conv[2]}')
                out.append(f'inline bool String_To_Value(const char *text, {name} *value) {conv[3]}' if conv[3] else
                           f'inline bool String_To_Value(const char *, {name} *) {{ return false; }}')
                continue
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


HEADER = r'''// Earth & Beyond client (reconstruction)
/*
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
// and its constructor is delta_money.cpp's (0x4A7080, called out of line as the client's)
extern template class DeltaTemplateClass<MoneyClass>;

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

// The class of co.cpp's objects (vtable 0xB0070C, constructor 0x6F2820, which keeps the root it
// makes at +0x88); only the factory is declared here. 0x6F6000 is its member (`ret 8`, called with
// the object in ecx) though it uses nothing of the object.
// NAME 1: neutral (D10), the class of co.cpp's objects (vtable 0xB0070C)
class UnknownCOClass
{
public:
	// 0x6F6000: the root for a CREATE's type, or 0 when the type has no aux data
	UnknownRootClass *Unknown_Create_Aux_Root(int type, int unknown);
};

%(inline_roots)s#endif //__AUX_ROOTS_H
'''

# VC6 symbol pad (HX1, 2026-10-28; workflow.md 3.3): every declaration the file sees (a header's member, an
# extern, an enumerator) moves the template constructors compiled at its end (0x6FB1E0, 0x702240,
# 0x7045F0, 0x7014B0, 0x701D70, 0x758210) by one step of a cycle of 64: each matches only in a window of
# it. The pad's enumerators put the file inside the windows; retune SYMBOL_PAD when a header aux_roots.cpp
# includes gains or loses declarations (.work/hx1/exp.py sweeps it by compiling aux_roots.cpp alone).
SYMBOL_PAD = 15
PAD = '''
// VC6 symbol pad (not the client's code): the constructors above match only when the count of the
// declarations this file sees falls in a window of a cycle of 64 (tools/re/auxroots.py SYMBOL_PAD)
enum AuxRootsSymbolPadType { %s };	// NAME 1: a build aid (HX1)
'''

SOURCE = r'''// Earth & Beyond client (reconstruction): GENERATED by tools/re/auxroots.py, do not edit.
// The aux-data roots' constructors and the CREATE factory (M1.3, D43). NOT EXACT (D42): functional.
// NAME 1 [aux_roots.cpp]: provisional file name (the client's constructors are in co.cpp 0x6F6000-0x6FBEF0 and the Player object 0x751BE0)
#include "aux_roots.h"

%(ctors)s
%(pad)s'''

SHIP_ADD = '''// Ship's narrow Add (0x6F8C70)
void DeltaShipClass::Add(int narrow, DeltaClass *child)
{
	DeltaCompoundClass::Add(child);
	if (narrow == 1) {
		child->Unknown80 = 0xCF;
	}
}'''

FACTORY = '''// the CREATE factory (0x6F6000)
UnknownRootClass *UnknownCOClass::Unknown_Create_Aux_Root(int type, int unknown)
{
	switch (type) {
%(cases)s
	}
	return 0;
}'''


def generate(doc, out_dir):
    g = Gen(doc)
    ctors = []
    inline_roots = []      # the factory's roots: inline (header), as the client's (see the note at SEND_ROOTS)
    roots = doc['roots']
    g.prepare(roots)
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
        text = (f'// {rname}' + (f' (0x{ROOT_CTOR[rname]:X})' if rname in ROOT_CTOR else ' (built in the factory 0x6F6000)')
                + '\n' + g.root_ctor)
        if rname in SEND_ROOTS:
            ctors.append((ROOT_CTOR.get(rname, 0x6F6000), text))
        else:
            inline_roots.append((ROOT_CTOR[rname], text.replace(f'\n{g.classes[int(r["vt"], 16)]}::', f'\ninline {g.classes[int(r["vt"], 16)]}::', 1)))
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
    h = HEADER.replace('%(types)s', g.type_decls()).replace('%(classes)s', classes).replace(
        '%(inline_roots)s', ''.join(text + '\n\n' for _, text in sorted(inline_roots)))
    # the client's order (its objects compile in address order: VC6's inlining depends on what came before)
    ctors += g.nested + [(0x6F8C70, SHIP_ADD), (0x6F6000, FACTORY.replace('%(cases)s', '\n'.join(cases)))]
    c = SOURCE.replace('%(ctors)s', '\n\n'.join(text for _, text in sorted(ctors, key=lambda x: x[0])))
    pad = PAD % ', '.join(f'AUX_ROOTS_PAD_{i:02d}' for i in range(SYMBOL_PAD)) if SYMBOL_PAD else ''
    c = c.replace('%(pad)s', pad)
    os.makedirs(out_dir, exist_ok=True)
    for name, text in (('aux_roots.h', h), ('aux_roots.cpp', c)):
        with open(os.path.join(out_dir, name), 'w', newline='\n') as f:
            f.write(text)
    print(f'{len(g.classes)} classes ({len(roots)} roots), {len(g.types)} value types -> {out_dir}')
    return g


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('cmd', choices=['generate', 'schema', 'check', 'tree', 'layout'])
    ap.add_argument('root', nargs='?')
    ap.add_argument('--client', default=CLIENT)
    ap.add_argument('--schema', default=SCHEMA)
    ap.add_argument('--out', default=OUT_DIR)
    a = ap.parse_args()
    doc = extract(a.client)
    if a.cmd == 'schema':
        out = {'source': doc['source'], 'generator': 'tools/re/auxextract.py', 'createTypes': doc['createTypes'],
               'playerRoot': doc['playerRoot'], 'roots': strip(doc['roots'])}
        with open(a.schema, 'w', encoding='utf-8', newline='\n') as f:
            json.dump(out, f, indent=1)
            f.write('\n')
        print('wrote', a.schema)
        return
    if a.cmd == 'check':
        ref = json.load(open(a.schema))
        same = strip(doc['roots']) == ref['roots'] and doc['createTypes'] == ref['createTypes']
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
    if a.cmd == 'layout':
        layout(doc, a.out)
        return
    generate(doc, a.out)


def layout(doc, out_dir):
    """Compile a probe of the generated header (offsetof of every member, as tools/vc6/layout.py)
    and list the members whose offset isn't the client's, with the gap before them."""
    g = generate(doc, out_dir)
    sys.path.insert(0, os.path.join(REPO, 'tools', 'vc6'))
    import vc6
    from layout import read_array
    work = os.path.join(REPO, '.work', 'auxlayout')
    os.makedirs(work, exist_ok=True)
    rows = []
    for vt, (name, mem) in sorted(g.layout.items()):
        for off, m in mem:
            rows.append((name, m, off))
    src = os.path.join(work, 'probe.cpp')
    lines = ['#include <stddef.h>', '#include "' + os.path.join(out_dir, 'aux_roots.h') + '"',
             'extern "C" const unsigned int ENB_LAYOUT[] = {']
    lines += [f'\toffsetof({c}, {m}),' for c, m, _ in rows] + ['};', '']
    with open(src, 'w', newline='\n') as f:
        f.write('\n'.join(lines))
    incs = [os.path.join(REPO, d) for d in ('app\\client', 'app\\shared\\auxdata', 'app\\shared', 'wwlib', 'wwdebug', 'wwmath')]
    out = os.path.join(work, 'obj')
    rc, text = vc6.compile_sources([src], incs, flags='/nologo /c /MT /W0 /O2 /Ob2 /GX', out=out)
    obj = os.path.join(out, 'probe.obj')
    if not os.path.exists(obj):
        print(text)
        raise SystemExit('the probe did not compile')
    got = read_array(obj, '_ENB_LAYOUT', len(rows))
    bad = 0
    last = None
    for (c, m, want), have in zip(rows, got):
        if have != want and c != last:
            print(f'{c}.{m}: +0x{have:X} here, +0x{want:X} in the client (gap 0x{want - have:X} before it)')
            bad += 1
            last = c
    print(f'{len(rows)} members, {bad} classes with a member off its client offset')


if __name__ == '__main__':
    main()
