"""Generate the client's AuxData root classes (M1.3, D43) from the constructors in client.exe.

    python tools/re/auxroots.py generate [--client EXE]     writes app/client/aux_roots.h/.cpp
    python tools/re/auxroots.py schema [--schema JSON]      writes the extracted trees as docs/data/client-auxdata.json
    python tools/re/auxroots.py check [--schema JSON]       the extracted trees vs that committed schema
    python tools/re/auxroots.py tree ROOT                   prints one extracted tree

The trees come from tools/re/auxextract.py (symbolic execution of the 12 roots' constructors: the
CREATE factory 0x6F6000's ten, the Player root 0x752510 and the Hull root SClient 0x7518A0 builds inline). Each compound the client builds has
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
blobs: `Unknown<VT>Type`; the client's name where its KVPair's Get_Type_Name gives one, CLIENT_TYPES)
with the conversions KVPairTemplateClass needs.

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
SEND_ROOTS = {'Player', 'Hull'}     # derived from UnknownSendRootClass (unknown_root.h)
# Send roots the client builds inline in another file's function (auxextract.py INLINE_SEND): their
# constructor is inline in the header and takes the base's first argument (Hull: SClient 0x7518A0
# passes its +0x112C).
INLINE_SEND = {'Hull': 0x7518A0}
# Send roots whose constructor (and its nested out-of-line constructors) the client compiles as copies of
# inline functions in its first caller's object: Player 0x752510 follows SClient 0x7518A0 (sclient.cpp's
# object 0x7405E0-0x75A280: Player's tree 0x752510-0x7551D0 sits between 0x7518A0 and 0x7555B0, its only
# callers 0x7518A0 / 0x755600). Their definitions go to aux_roots_send.h, which sclient.cpp includes,
# children first and the root's last: VC6 emits the pending inline copies in definition order, and
# Player's destructor 0x7551D0 / RPGInfo's 0x753C60 are exact only when the enum instances' destructors
# (0x753C20 / 0x753C40, from RPGInfo's constructor) come before them. Placement interim (D137, SC1).
SEND_HEADER_ROOTS = {'Player': 0x7518A0}
# The factory's roots' constructors are inline functions in the header (the rest: SEND_ROOTS, in
# aux_roots.cpp): the client's out-of-line copies follow the factory 0x6F6000, their first caller
# (Ship 0x6F66E0 right after it), and the factory is exact only so (with 1.0f in ARRAY_VALUES).
# Nested constructors generated out of line and NOT inline (the rest are `inline`: see Gen.compound).
# Element (0x707430): the client's is inline too, but its only caller is the Buff array 0x707040,
# into which ours does not inline the Elements array's base (ARRAY_DERIVED; the client does), so
# an inline Element is lost there (0x707430); plain, it stays exact.
# Element is FAMILY_MOVED into aux_buff.h (AB 2026-11-18): `inline` there for the objects that number its int and
# bool at parse time (ui_buff 0x561C10, as the client's inline one does), plain under AUX_ROOTS_ALL (aux_roots.cpp,
# its one definition: inline, ours goes into the Elements base, out of line here, and 0x707430 is lost).
OOLINE_PLAIN = {0xB01F74}
# Compounds whose destructor the client declares (an empty body: VC6 stores the class's vtable first,
# which an implicit destructor leaves out): Harvestable 0x6FA5E0, the Buffs array (inlined: the address of its store).
USER_DTOR = {0xB010C0: 0x6FA5E0,
             0xB00B0C: 0x6F8EA9}   # Ship.BuffArray, inlined in Ship's destructor 0x6F8D30
# Arrays whose class derives from the template instance: the client calls the instance's
# constructor and then stores the array's own vtable and per-class global (Ship's BuffArray:
# 0x707040 stores 0xB01A98, Ship 0x6F73DA then 0xB00B0C and [0xC41600]; the two vtables differ
# only in the destructor slot). Array vtable -> (instance vtable, instance constructor).
# Both are instances of one class template (delta_item_array.h's DeltaDerivedItemArrayClass<T>, AB 2026-11-18):
# the client numbers their globals (0xC416A4 Elements', 0xC41600 Buffs') as template statics, in an object's
# end-of-file (guarded) initialiser, and their bases' (0xC41694, 0xC41570) a round later (ui_buff 0x561CB0 /
# 0x561D00, ship_stats_display 0x6A3CA0 / 0x6A3D30, co 0x707A20 / 0x709B80); the two destructors (0x707810,
# 0x703A90) have one form. The typedef keeps the class's line count (blank lines: aux_roots.h's padding).
DERIVED_TEMPLATE = 'DeltaDerivedItemArrayClass'
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
    # C5 (AR, run 2026-11-16): its slot +0x00 0x703D50 builds KVPairTemplateClass<Vector3> (0xAEB484) with the
    # accessor 0xB01B2C, whose Set 0x705FD0 compares with fld / fcomp: wwmath's Vector3 (aux_roots.h declares
    # strconv_lib.cpp's conversions 0x4A3750 / 0x4A3810 and Type_Name 'Vector3')
    0xB00A00: ('DeltaTemplateClass<Vector3>', None),
}
INTERPOLATOR = 0xAEAE54
# Value types whose constructor the client shows (the leaf constructors' default argument, the Reads'
# local): the colour's does nothing (a stack temporary is copied), 0x705640's T starts as
# {1.0f, 0, 0, -1, 0}, 0x757B00's 16 bytes are zeroed.
BLOB_FORMS = {
    0xB01A40: ('20 bytes, constructed {1.0f, 0.0f, 0.0f, -1, 0.0f} (0x705640; Ship 0x6F70BE stores the zeros as immediates among register zeros: floats, workflow.md 3.1)',
               'float F00; float F04; float F08; int I0C; float F10; NAME() : F00(1.0f), F04(0.0f), F08(0.0f), I0C(-1), F10(0.0f) {} '
               'NAME(const NAME &v) { F00 = v.F00; F04 = v.F04; F08 = v.F08; I0C = v.I0C; F10 = v.F10; } '
               'NAME &operator=(const NAME &v) { F00 = v.F00; F04 = v.F04; F08 = v.F08; I0C = v.I0C; F10 = v.F10; return *this; }'),
    0xB016DC: ('12 bytes, constructed {-1, -1, -1} (the Mounts elements\' default, Ship 0x6F6E29); the copy constructor is the client\'s: sclient\'s 0x751BE0 calls it out of line (0x752170)',
               'int I00; int I04; int I08; NAME() : I00(-1), I04(-1), I08(-1) {} '
               'NAME(const NAME &v) { I00 = v.I00; I04 = v.I04; I08 = v.I08; }'),
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
# The value types' client names (NAME 5), by leaf vtable: the string their KVPair's Get_Type_Name
# (slot +0x08) returns, inline (`mov eax, str`) or through an out-of-line Type_Name(value) of another
# file. Leaf vtable -> (name, KVPair vtable, Get_Type_Name, the string's source). The KVPairs of
# 0xB0122C, 0xB013EC, 0xB01394, 0xB0133C return 'int' inline (0x708690, 0x708800, 0x708970,
# 0x708AE0): their types keep neutral names, Type_Name 'int' (INT_NAMED).
CLIENT_TYPES = {
    0xB01844: ('EffectGeneratorUsageBF', 0xB0194C, 0x707BA0, 'Type_Name 0x835370'),
    0xB0189C: ('TargetGeneratorSummaryBF', 0xB01984, 0x707D10, 'Type_Name 0x84BE40'),
    0xB018F4: ('GameActionValidityBitFlag', 0xB019BC, 0x707E80, 'Type_Name 0x83D270'),
    0xB017EC: ('ItemStateEnum', 0xB019F4, 0x707FF0, 'inline'),
    0xB00A58: ('GuildRankType', 0xB01AF4, 0x708240, 'Type_Name 0x83D8B0'),
    0xB009A8: ('InterruptStateType', 0xB01B40, 0x7083B0, 'inline'),
    0xB0100C: ('WW3DLightAttenuationType', 0xB01B78, 0x708520, 'inline'),
    0xB01A40: ('ProjectedDecalClass', 0xB01D00, 0x708C60, 'inline'),
    0xB03694: ('SkillLevelAvailabilityStruct', 0xB03AB4, 0x759CE0, 'inline'),
    0xB03A24: ('HullPreviewStatusEnum', 0xB03A7C, 0x759B50, 'inline'),
    0xB016DC: ('InvSlotStruct', 0xB01E28, 0x709C80, 'inline'),   # its slot +0x00 0x706DF0 builds that KVPair (C5)
}
# An enum's inline conversions where the client's KVPair shows another form than the default
# (_snprintf / sscanf into a local, == 1): leaf vtable -> (Value_To_String's value parameter, its body,
# String_To_Value's body).
# 0xB03A24: Get_Value_String 0x759B70 sprintf's '%d' and returns the buffer; Set_Value_String 0x75A140
# sscanf's '%d' and tests the result for nonzero, passing the (uninitialised) value itself, not its address:
# its slot is the text argument's, so the client pushes the text pointer ([esp+4]) and Sets from that slot.
ENUM_FORMS = {0xB03A24: ('const NAME &value', '{ sprintf(buf, "%d", value); return buf; }',
                         '{ return sscanf(text, "%d", *value) != 0; }')}
INT_NAMED = {0xB0122C: 0x708690, 0xB013EC: 0x708800, 0xB01394: 0x708970, 0xB0133C: 0x708AE0}
# The enums' conversions the client calls out of line (other files' functions, not rebuilt; stand-ins in
# stubs/app_stubs.cpp): leaf vtable -> (Type_Name(value) or None when inline, String_To_Value,
# Value_To_String), from the KVPair's slots +0x08 (Get_Type_Name), +0x10 (Set_Value_String 0x709680..)
# and +0x0C (Get_Value_String). An out-of-line Type_Name takes the saved value (`push [ecx+0x10]`):
# the KVPair's Get_Type_Name is specialised to pass it, as shipcust_desc.h does for ProfessionType.
# Conversions now rebuilt in the tree: declared as ENUM_EXTERN says, no stand-in (target_generator_summary.cpp's, GD1; item_effect.cpp's, E1; item_state.cpp's, C5 2026-11-12; game_action_validity.cpp's, O24 2026-11-14; unknown_82ecb0.cpp's, A4 2026-11-14).
ENUM_REBUILT = {0xB0189C, 0xB01844, 0xB017EC, 0xB018F4, 0xB0100C}
ENUM_EXTERN = {
    0xB01844: (0x835370, 0x835390, 0x835590),
    0xB0189C: (0x84BE40, 0x84BE60, 0x84C060),
    0xB018F4: (0x83D270, 0x83D290, 0x83D490),
    0xB017EC: (None, 0x842930, 0x842970),
    0xB00A58: (0x83D8B0, 0x83D7D0, 0x83D860),
    0xB009A8: (None, 0x83DA80, 0x83D9E0),
    0xB0100C: (None, 0x82F180, 0x82F1D0),
    0xB0122C: (None, 0x8456C0, 0x845780),
    0xB013EC: (None, 0x8458C0, 0x845910),
    0xB01394: (None, 0x846040, 0x846090),
    0xB0133C: (None, 0x846110, 0x846160),
}
# A blob's operator!= where the client's Set_Value shows it (the rest memcmp): InvSlotStruct's (0xB016DC)
# accessor Set 0x7080C0 compares the three ints (cmp/jne), the delta's first.
BLOB_NE = {
    # 0xB03694: its KVPair's Is_Changed 0x759D30 compares the four ints (cmp/jne), the saved value's first
    0xB03694: 'inline bool operator!=(const NAME &a, const NAME &b) { return a.I00 != b.I00 || a.I04 != b.I04 || '
              'a.I08 != b.I08 || a.I0C != b.I0C; }',
    0xB016DC: 'inline bool operator!=(const NAME &a, const NAME &b) { return a.I00 != b.I00 || a.I04 != b.I04 || a.I08 != b.I08; }',
}
# A blob's conversions the client calls out of line (another file's functions, not rebuilt; stand-ins in
# stubs/aux_roots_stubs.cpp): leaf vtable -> (String_To_Value, Value_To_String), from the KVPair's
# Set_Value_String / Get_Value_String. 0xB03694: 0x75A190 calls 0x84A600 (text, &value), 0x759D00 calls
# 0x84A560 (buf, size, &value), both in app\Common\skill_level_availability.cpp ('%d,%d,%d,%d', 0xBB0C88).
BLOB_EXTERN = {0xB03694: (0x84A600, 0x84A560)}
BLOB_CONV = {
    # 0xB016DC: Get_Value_String 0x709CA0 sprintf's '%d,%d,%d' (0xB94490, the size unused); Set_Value_String 0x709E10
    # sscanf's it into a default-constructed local, true on three fields (C5)
    0xB016DC: ('InvSlotStruct',
               'inline bool operator!=(const NAME &a, const NAME &b) { return a.I00 != b.I00 || a.I04 != b.I04 || a.I08 != b.I08; }',
               '{ sprintf(buf, "%d,%d,%d", value.I00, value.I04, value.I08); return buf; }',
               '{ return sscanf(text, "%d,%d,%d", &value->I00, &value->I04, &value->I08) == 3; }'),
    0xB01A40: ('ProjectedDecalClass',
               'inline bool operator!=(const NAME &a, const NAME &b) '
               '{ return !(a.F00 == b.F00 && a.F04 == b.F04 && a.F08 == b.F08 && a.I0C == b.I0C && a.F10 == b.F10); }',
               '{ sprintf(buf, "%f,%f,%f,%d,%f", value.F00, value.F04, value.F08, value.I0C, value.F10); return buf; }',
               # Set_Value_String 0x709A90: sscanf into a default-constructed local, true on five fields
               '{ return sscanf(text, "%f,%f,%f,%d,%f", &value->F00, &value->F04, &value->F08, &value->I0C, '
               '&value->F10) == 5; }'),
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


# The value types whose committed declarations are hand edits (AR2 2026-11-17 and earlier: the *_defs.h
# includes, Vector3, InvSlotStruct's and ProjectedDecalClass's evidence comments): leaf vtable -> (the C++
# value type the members use, None for the generated name; the exact text aux_roots.h holds in place of the
# generated declarations, '' when another entry's text covers it). The text keeps the generated block's line
# count where the file had it (its blank lines are __LINE__ padding: workflow.md 3.9, aux_roots.cpp). 0xB00A00
# (Vector3) is a LEAF, not a value type of its own: its text sits between 0xB009A8's and 0xB00A58's.
HAND_TYPES = {
    0xB009A8: (None, r'''#include "..\common\interrupt_defs.h"	// InterruptStateType (KVPair vtable 0xB01B40, leaf vtable 0xB009A8) and its converters 0x83D9E0 / 0x83DA80



'''),
    0xB00A00: (None, r'''// leaf vtable 0xB00A00's value type is wwmath's Vector3: its slot +0x00 0x703D50 builds KVPairTemplateClass<Vector3> (vtable 0xAEB484, kv_encoder.cpp's) with the accessor 0xB01B2C, whose Set 0x705FD0 compares with fld / fcomp (Vector3's operator!=)
#include "vector3.h"
inline const char *Type_Name(Vector3 *) { return "Vector3"; }	// KVPairTemplateClass<Vector3>::Get_Type_Name 0x4B24F0
char *Value_To_String(char *buf, int size, const Vector3 &value);	// 0x4A3750 (strconv_lib.cpp)
bool String_To_Value(const char *text, Vector3 *value);	// 0x4A3810 (strconv_lib.cpp)
'''),
    0xB00A58: (None, r'''#include "..\common\guild_defs.h"	// GuildRankType (KVPair vtable 0xB01AF4, leaf vtable 0xB00A58), its Type_Name 0x83D8B0 and converters 0x83D7D0 / 0x83D860


template<> inline const char *KVPairTemplateClass<GuildRankType>::Get_Type_Name() const { return Type_Name(Value); }	// 0x708240

'''),
    0xB011D4: ('unsigned int', r'''// leaf vtable 0xB011D4's value type is unsigned int: its KVAccessorClass table 0xAEB258 (slot 0 KVAccessorClass<unsigned int>'s ??_G) is stored by 0x706D80
#include "..\common\manufacturing_defs.h"	// the subcategory, mode, validity and difficulty types (vtables 0xB0122C, 0xB013EC, 0xB01394, 0xB0133C) and their converters
inline const char *Type_Name(UnknownB0122CEnum *) { return "int"; }	// the KVPair's Get_Type_Name 0x708690 says 'int'
inline const char *Type_Name(UnknownB0133CEnum *) { return "int"; }	// the KVPair's Get_Type_Name 0x708AE0 says 'int'
inline const char *Type_Name(UnknownB01394Enum *) { return "int"; }	// the KVPair's Get_Type_Name 0x708970 says 'int'
inline const char *Type_Name(UnknownB013ECEnum *) { return "int"; }	// the KVPair's Get_Type_Name 0x708800 says 'int'














'''),
    0xB0122C: (None, r''''''),
    0xB0133C: (None, r''''''),
    0xB01394: (None, r''''''),
    0xB013EC: (None, r''''''),
    0xB016DC: (None, r'''// NAME 5: the client's Get_Type_Name string 'InvSlotStruct' (KVPair vtable 0xB01E28, 0x709C80 inline, string 0xB94480), leaf vtable 0xB016DC: its slot +0x00 0x706DF0 builds that KVPair (the Mounts elements)
struct InvSlotStruct { int I00; int I04; int I08; InvSlotStruct() : I00(-1), I04(-1), I08(-1) {} InvSlotStruct(const InvSlotStruct &v) { I00 = v.I00; I04 = v.I04; I08 = v.I08; } };	// vtable 0xB016DC, 12 bytes, constructed {-1, -1, -1} (the Mounts elements' default, Ship 0x6F6E29); the copy constructor is the client's: sclient's 0x751BE0 calls it out of line (0x752170)
inline bool operator!=(const InvSlotStruct &a, const InvSlotStruct &b) { return a.I00 != b.I00 || a.I04 != b.I04 || a.I08 != b.I08; }	// the accessor's Set 0x7080C0 compares the three ints (cmp / jne)
inline const char *Type_Name(InvSlotStruct *) { return "InvSlotStruct"; }
inline char *Value_To_String(char *buf, int, const InvSlotStruct &value) { sprintf(buf, "%d,%d,%d", value.I00, value.I04, value.I08); return buf; }	// Get_Value_String 0x709CA0 (the size unused)
inline bool String_To_Value(const char *text, InvSlotStruct *value) { return sscanf(text, "%d,%d,%d", &value->I00, &value->I04, &value->I08) == 3; }	// Set_Value_String 0x709E10'''),
    0xB01A40: (None, r'''// NAME 5: the client's Get_Type_Name string 'ProjectedDecalClass' (KVPair vtable 0xB01D00, 0x708C60 inline), leaf vtable 0xB01A40; member ProjDecalAssetID from 0x7633E0 FAIL_IF
struct ProjectedDecalClass { float F00; float F04; float F08; int ProjDecalAssetID; float F10; ProjectedDecalClass() : F00(1.0f), F04(0.0f), F08(0.0f), ProjDecalAssetID(-1), F10(0.0f) {} ProjectedDecalClass(const ProjectedDecalClass &v) { F00 = v.F00; F04 = v.F04; F08 = v.F08; ProjDecalAssetID = v.ProjDecalAssetID; F10 = v.F10; } ProjectedDecalClass &operator=(const ProjectedDecalClass &v) { F00 = v.F00; F04 = v.F04; F08 = v.F08; ProjDecalAssetID = v.ProjDecalAssetID; F10 = v.F10; return *this; } };	// vtable 0xB01A40, 20 bytes, constructed {1.0f, 0.0f, 0.0f, -1, 0.0f} (0x705640; Ship 0x6F70BE stores the zeros as immediates among register zeros: floats, workflow.md 3.1)
inline bool operator!=(const ProjectedDecalClass &a, const ProjectedDecalClass &b) { return !(a.F00 == b.F00 && a.F04 == b.F04 && a.F08 == b.F08 && a.ProjDecalAssetID == b.ProjDecalAssetID && a.F10 == b.F10); }
inline const char *Type_Name(ProjectedDecalClass *) { return "ProjectedDecalClass"; }
inline char *Value_To_String(char *buf, int, const ProjectedDecalClass &value) { sprintf(buf, "%f,%f,%f,%d,%f", value.F00, value.F04, value.F08, value.ProjDecalAssetID, value.F10); return buf; }
inline bool String_To_Value(const char *text, ProjectedDecalClass *value) { return sscanf(text, "%f,%f,%f,%d,%f", &value->F00, &value->F04, &value->F08, &value->ProjDecalAssetID, &value->F10) == 5; }'''),
    0xB03588: ('ProfessionType', r'''#include "..\common\prof.h"	// leaf vtable 0xB03588's value type: KVAccessorClass table 0xAEC9C8 is stored by both ~KVPointerAccessor<ProfessionType> 0x4F76E0 and this accessor's destructor 0x7595F0
#include "..\common\race.h"	// leaf vtable 0xB035E0's value type: KVAccessorClass table 0xAEC9DC is shared with shipcust_desc's RaceType accessor likewise
#ifndef KVPAIR_PROF_RACE_TYPE_NAMES	// the profession's and race's pairs name their type from the saved value (prof.cpp 0x848CA0, race.cpp 0x848FF0)
#define KVPAIR_PROF_RACE_TYPE_NAMES
template<> inline const char *KVPairTemplateClass<ProfessionType>::Get_Type_Name() const { return Type_Name(Value); }	// 0x4F6ED0
template<> inline const char *KVPairTemplateClass<RaceType>::Get_Type_Name() const { return Type_Name(Value); }	// 0x4F70C0
#endif'''),
    0xB035E0: ('RaceType', r''''''),
    0xB03694: (None, r'''#include "..\common\skill_level_availability.h"	// SkillLevelAvailabilityStruct (KVPair vtable 0xB03AB4, leaf vtable 0xB03694) and its converters 0x84A560 / 0x84A600




'''),
    0xB03970: ('FormationState', r'''// NAME 5: the client's Get_Type_Name string 'FormationState' (KVPair 0x759E80 hands the saved value to Type_Name 0x835CF0; 0x759EA0 / 0x75A200 call 0x835D80 / 0x835D10), leaf vtable 0xB03970: the groups' 'Formation'
#include "..\common\faction_and_reputation.h"	// FormationState and its three converters (4 bytes)
template<> inline const char *KVPairTemplateClass<FormationState>::Get_Type_Name() const { return Type_Name(Value); }	// 0x759E80

'''),
}


# The family gates (AS, run 2026-11-18): each aux-data constructor body in aux_roots.h / aux_roots_send.h is
# compiled only under `AUX_ROOTS_ALL` or its family's `AUX_CTORS_<FAMILY>` (app/client/aux_<family>.h), so an
# object numbers (the Delta templates' per-class globals, workflow.md 3.4) exactly the families it sees, as the
# client's objects do. In-class constructors (by the class's Delta<X>Class name) get an `#else` declaration;
# out-of-class inline definitions are wrapped whole (with their comment lines).
FAMILY_INCLASS = {
    'Shake': 'SHIP', 'Effect': 'ITEM', 'EquipItem': 'ITEM', 'ShipInv': 'SHIPINV', 'Attachment': 'LEGO',
    'Lego': 'LEGO', 'Elements': 'BUFF', 'Buff': 'BUFF', 'Buffs': 'BUFF', 'Stats': 'SHIP', 'SubCategory': 'MANU',
    'Category': 'MANU', 'MissionStage': 'MISSION', 'Faction': 'FACTION', 'Reputation': 'FACTION',
    'Hulk': 'HULK', 'Planet': 'PLANET', 'Starbase': 'STARBASE', 'Stargate': 'STARGATE', 'Hull': 'HULL',
}
FAMILY_OUTOFCLASS = {
    'Ship': 'SHIP', 'Light': 'LIGHT', 'Decoration': 'DECORATION', 'Harvestable': 'HARVESTABLE',
    'ManufacturingLab': 'MANU', 'Component': 'COMPONENT', 'RPGInfo': 'SKILL', 'Skill': 'SKILL',
    'SkillAbility': 'SKILL', 'Mission': 'MISSION', 'GroupInfo': 'GROUP', 'GroupMember': 'GROUP', 'Player': 'PLAYER',
}
# aux_roots.cpp's nested constructors that live in a family header (the families' objects compile them too),
# included back where aux_roots.cpp defined them: address -> (header, the include line's note after the
# constructor's comment). The generator writes the body into the header in place (OUT_DIR only).
FAMILY_MOVED = {0x6FA150: ('aux_item.h', "the item family's constructor"),
                0x6FB1E0: ('aux_manu.h', "the manufacturing family's"),
                0x707430: ('aux_buff.h', "the buff family's constructor")}
# The group classes in a header of their own (DN, run 2026-11-18): an object parses them before aux_roots.h's when it
# includes aux_group.h / aux_group_ship.h first, and the Delta templates number in the order their classes are first
# instantiated (the members' declarations), so it numbers the group's types first as the client's objects do:
# radar_system 0x7BDE30 / 0x7BDEA0 (T<int>, T<FormationState>, T<bool>, then the ship's; I<GroupMember> first),
# co_player_relationship 0x70D830, starmap_pip 0x7C3150, ui_starbase 0x644BE0 (T<int>, T<FormationState>, T<bool>).
# aux_roots.h includes aux_roots_group.h where the classes were (line-neutral) and aux_formation.h where FormationState
# was; the group's constructors are in aux_group_ctors.h, which aux_roots_group.h includes unless the object compiles
# the player's tree (sclient: there aux_roots_send.h includes it in their place after the mission's, VC6 emitting the
# out-of-line copies in definition order, 0x7548D0 / 0x754DB0). Their gate leaves out AUX_ROOTS_ALL: aux_roots.cpp
# and co.cpp never compiled them (aux_roots_send.h is not theirs).
# The send roots' classes before the ship's (DN, run 2026-11-18): sclient's numbering initialiser 0x758CD0 stores the
# item's types (T<int>, T<float>, the effect's, the equipped item's), then Hull's HullPreviewStatusEnum (0xBDA90C), then
# the player's tree (the skill's, RPGInfo's, FormationState), the ship's last (GuildRankType, Vector3,
# InterruptStateType), and 0x7598E0 the ship inventory's and items' arrays, then the player's tree's, then the lego's,
# buff's and ship's: the client's sclient.cpp parsed those classes first. So the value types (aux_roots_types.h), the
# class families (SPLITS below) and Hull (aux_roots_hull.h, out of aux_roots_send.h) are headers of their own,
# included back where they were (aux_roots.h line-neutral): an object that includes aux_roots.h alone sees the classes
# in the same order as before, aux_ship_player.h includes the item's, the ship inventory's, Hull's and the player's
# tree first. The types header leaves out delta_interpolator.h (its
# DeltaInterpolatorClass instantiates T<float> before the item's T<int>; aux_roots.h includes it as before).
TYPES_MARK = '// ------------------------------------------------------------------------- value types'
CLASSES_MARK = '// ------------------------------------------------------------------------- the compounds'
# The class headers, in aux_roots.h's order: (header, first class, last class, what it holds, its includes). Each family
# header (aux_<family>.h) includes its classes' headers before aux_roots.h, in the order the client's objects number
# them: item objects T<int> then T<float> (inv_render 0x68FAA0, ui_jobs, ui_ptrade), lego objects T<int>, T<float>,
# I<Attachment> (client_lego_impl 0x731490), skill objects the skill's first and T<float> last (skill_data 0x6A5210),
# the player's (ui_faction, ui_missn, avatar_pip) the skill's, the item's, then the mission's, faction's, group's and
# player's, the manufacturing objects the item's then their own (ui_manu, sbaseview, ui_refinery: 0xC41630 before T<bool>).
SPLITS = [
    ('aux_roots_item.h', 'DeltaItemClass', 'DeltaEquipItemClass', 'The item classes (Item, Effect, EquipItem).',
     ['aux_roots_types.h']),
    ('aux_roots_shipinv.h', 'DeltaShipInvClass', 'DeltaShipInvClass', "The ship inventory's class.", ['aux_roots_item.h']),
    ('aux_roots_lego.h', 'DeltaAttachmentClass', 'DeltaLegoClass', 'The lego classes (Attachment, Lego).',
     ['aux_roots_types.h']),
    # its members instantiate T<bool> before delta_interpolator.h's T<float>: radar_system_data_cache / gutter_data
    # number T<bool>, T<float> in that order (CRT entries 656 / 639, aux_decoration.h)
    ('aux_roots_decoration.h', 'DeltaDecorationClass', 'DeltaDecorationClass', "The decoration's class.",
     ['aux_roots_types.h']),
    ('aux_roots_manu.h', 'DeltaSubCategoryClass', 'DeltaManufacturingLabClass',
     "The manufacturing classes (SubCategory, Category, PrimaryCategory, KnownFormula, the lab's root).",
     ['aux_roots_item.h']),
    ('aux_roots_skill.h', 'DeltaSkillClass', 'DeltaRPGInfoClass', 'The skill classes (Skill, SkillAbility, RPGInfo).',
     ['aux_roots_types.h']),
    ('aux_roots_player.h', 'DeltaMissionStageClass', 'DeltaPlayerClass',
     "The player's tree after the skill's: MissionStage, Mission, Faction, Reputation, the group's, Player.",
     ['aux_roots_skill.h', 'aux_roots_item.h']),
]
# Every object's G initialiser numbers I<Attachment> (0xC41580) before the ship inventory's arrays (co 0x707A20,
# ship_stats_display 0x6A3CA0, radar_system 0x7BDEA0): aux_roots.h includes the lego header first, in the ship
# inventory's place (and the ship inventory's in the lego's): ('aux_roots_shipinv.h', 'aux_roots_lego.h'). Off: no aux
# pad keeps aux_roots.cpp's 0x6FB1E0 with it (DN's sweep, 2026-11-18: 9-12 trade it for 0x702240 / 0x7045F0).
SPLIT_SWAP = None
GROUP_VTS = (0xB039C8, 0xB038B8)       # GroupMember, GroupInfo (adjacent in the class order)
GROUP_CTORS = {0x7548D0, 0x754DB0}     # GroupInfo, GroupMember (in Player's tree, SEND_HEADER_ROOTS)
FORMATION_VT = 0xB03970

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
            name = CLIENT_TYPES[vt][0] if vt in CLIENT_TYPES else f'Unknown{hexu(vt)}Enum'
            self.types[vt] = ('enum', 4, name)
        else:
            size = 12 if t == 'color' else int(t[5:])
            name = CLIENT_TYPES[vt][0] if vt in CLIENT_TYPES else f'Unknown{hexu(vt)}Type'
            self.types[vt] = ('blob', size, name)
        if vt in HAND_TYPES and HAND_TYPES[vt][0]:
            name = HAND_TYPES[vt][0]
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
        if f['type'] == 'enum' and cls != 'DeltaTemplateClass<unsigned int>':
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
            elif ecls == 'DeltaStringClass' and el.get('mask') is None:
                # string elements too (0xB00E9C, MountBoneNames: 0x701B60 asserts in delta_item_array.h, line 60,
                # and builds DeltaStringClass(name, ""), delta_string.h's default; 0x6F8880 inlines its destructor)
                inst = 'DeltaItemArrayClass<DeltaStringClass>'
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
                         '\t\tUnknown04 = UnknownGlobal;', '\t}'] + \
                        ([f'\t~{name}() {{}}\t// user-declared: the client stores the vtable 0x{hexu(vt)} first (0x{USER_DTOR[vt]:X})']
                         if vt in USER_DTOR else []) + ['',
                         '\tstatic int UnknownGlobal;\t// the per-class global at +0x04', '};']
                # the class's place, as an instance of the derived template (see DERIVED_TEMPLATE)
                count = family_gate('\n'.join(lines)).count('\n') + 1
                lines = [lines[0].replace(f': {inst} (', f': {DERIVED_TEMPLATE}<{ecls}> on {inst} (', 1), tag,
                         f'typedef {DERIVED_TEMPLATE}<{ecls}> {name};']
                self.decl[vt] = '\n'.join(lines + [''] * (count - len(lines)))
                self.order.append(vt)
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
            if root in INLINE_SEND:
                init = [f'{basecls}(unknown, "{n["_cname"]}")']
                sig = f'{name}(int unknown)'
            elif root in SEND_ROOTS:
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
        lines = [f'// vtable 0x{hexu(vt)}' + (f', constructor 0x{ROOT_CTOR[root]:X}' if root in ROOT_CTOR else f', built inline in 0x{INLINE_SEND[root]:X}' if root in INLINE_SEND else '')
                 + ('' if root else f' (e.g. {path})'), tag,
                 f'class {name} : public {basecls}', '{', 'public:']
        inline = root in INLINE_ROOT_VT or root == 'Hulk' or root in INLINE_SEND
        if inline:
            body = body + ['\tUnknown_48();' if root in INLINE_SEND else '\tDeltaCompoundClass::Unknown_48();']
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
                [f'// {path} (0x{self.ooline[vt]:X})'] +
                (['inline'] if vt not in OOLINE_PLAIN else
                 ['#if !defined(AUX_ROOTS_ALL)', 'inline', '#endif'] if self.ooline[vt] in FAMILY_MOVED else []) +
                [f'{name}::{sig} :', '\t' + ',\n\t'.join(init), '{'] + body + ['}'])))
        else:
            lines.append(f'\t{sig} :')
            lines.append('\t\t' + ',\n\t\t'.join(init))
            lines.append('\t{')
            lines += ['\t' + b for b in body]
            lines.append('\t}')
        if vt in USER_DTOR:
            lines.append(f'	~{name}() {{}}	// 0x{USER_DTOR[vt]:X}: user-declared (it stores the vtable first)')
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

    def type_decls(self, only=None, skip=()):
        out = []
        for vt in sorted(set(self.types) | {0xB00A00}):
            if (only is not None and vt not in only) or vt in skip:
                continue
            if vt in HAND_TYPES:
                if HAND_TYPES[vt][1]:
                    out.append(HAND_TYPES[vt][1])
                continue
            kind, size, name = self.types[vt]
            if vt in CLIENT_TYPES:
                cn, kv, gtn, src = CLIENT_TYPES[vt]
                out.append(f'// NAME 5: the client\'s Get_Type_Name string \'{cn}\' (KVPair vtable 0x{hexu(kv)}, '
                           f'0x{gtn:X}' + (' inline' if src == 'inline' else f', {src}') + f'), leaf vtable 0x{hexu(vt)}')
            elif vt in INT_NAMED:
                out.append(f'// NAME 1: the value type of the leaf vtable 0x{hexu(vt)} (its KVPair\'s Get_Type_Name '
                           f'0x{INT_NAMED[vt]:X} says \'int\'; no name in Net-7 or C#)')
            else:
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
                elif vt in BLOB_NE:
                    out.append(BLOB_NE[vt].replace('NAME', name))
                else:
                    out.append(f'inline bool operator!=(const {name} &a, const {name} &b) '
                               '{ return memcmp(&a, &b, sizeof(a)) != 0; }')
            conv = BLOB_CONV.get(vt) if kind != 'enum' else None
            ref = name if kind == 'enum' else f'const {name} &'
            tname = 'int' if vt in INT_NAMED else CLIENT_TYPES[vt][0] if vt in CLIENT_TYPES else conv[0] if conv else name
            ext = ENUM_EXTERN.get(vt) if kind == 'enum' else None
            if ext:
                tn, s2v, v2s = ext
                if tn:
                    out.append(f'const char *Type_Name({name} value);\t// 0x{tn:X} (\'{tname}\')')
                    out.append(f'template<> inline const char *KVPairTemplateClass<{name}>::Get_Type_Name() const '
                               f'{{ return Type_Name(Value); }}\t// 0x{CLIENT_TYPES[vt][2]:X}')
                else:
                    out.append(f'inline const char *Type_Name({name} *) {{ return "{tname}"; }}')
                out.append(f'bool String_To_Value(const char *text, {name} *value);\t// 0x{s2v:X}')
                out.append(f'char *Value_To_String(char *buf, int size, {name} value);\t// 0x{v2s:X}')
                continue
            out.append(f'inline const char *Type_Name({name} *) {{ return "{tname}"; }}')
            bext = BLOB_EXTERN.get(vt) if kind != 'enum' else None
            if bext:
                out.append(f'bool String_To_Value(const char *text, {name} *value);\t// 0x{bext[0]:X}')
                out.append(f'char *Value_To_String(char *buf, int size, const {name} &value);\t// 0x{bext[1]:X}')
                continue
            if conv:
                out.append(f'inline char *Value_To_String(char *buf, int, {ref}value) {conv[2]}')
                out.append(f'inline bool String_To_Value(const char *text, {name} *value) {conv[3]}' if conv[3] else
                           f'inline bool String_To_Value(const char *, {name} *) {{ return false; }}')
                continue
            if kind == 'enum' and vt in ENUM_FORMS:
                out.append(f'inline char *Value_To_String(char *buf, int, {ENUM_FORMS[vt][0].replace("NAME", name)}) {ENUM_FORMS[vt][1]}')
                out.append(f'inline bool String_To_Value(const char *text, {name} *value) {ENUM_FORMS[vt][2]}')
            elif kind == 'enum':
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

    def stub_source(self):
        """stubs/aux_roots_stubs.cpp (D44): stand-ins for the ENUM_EXTERN conversions, the value as a number."""
        out = [STUBS_HEAD]
        for vt, (kind, size, name) in sorted(self.types.items()):
            if vt in HAND_TYPES:
                continue        # declared by the headers HAND_TYPES includes, their conversions rebuilt there
            ext = ENUM_EXTERN.get(vt) if kind == 'enum' else None
            bext = BLOB_EXTERN.get(vt) if kind != 'enum' else None
            if bext:
                out.append(f'bool String_To_Value(const char *, {name} *)\n{{\n'
                           f'\tSTUB_CALLED("String_To_Value({name}) (0x{bext[0]:X})");\n\treturn false;\n}}\n')
                out.append(f'char *Value_To_String(char *buf, int size, const {name} &value)\n{{\n'
                           f'\tSTUB_CALLED("Value_To_String({name}) (0x{bext[1]:X})");\n'
                           f'\t_snprintf(buf, size, "%d,%d,%d,%d", value.I00, value.I04, value.I08, value.I0C);\n\treturn buf;\n}}\n')
                continue
            if not ext or vt in ENUM_REBUILT:
                continue
            tn, s2v, v2s = ext
            tname = 'int' if vt in INT_NAMED else CLIENT_TYPES[vt][0]
            if tn:
                out.append(f'const char *Type_Name({name})\n{{\n\treturn "{tname}";\t// the client\'s 0x{tn:X}\n}}\n')
            out.append(f'bool String_To_Value(const char *text, {name} *value)\n{{\n'
                       f'\tSTUB_CALLED("String_To_Value({name}) (0x{s2v:X})");\n'
                       f'\tint v;\n\tif (sscanf(text, "%d", &v) != 1) {{\n\t\treturn false;\n\t}}\n'
                       f'\t*value = ({name})v;\n\treturn true;\n}}\n')
            out.append(f'char *Value_To_String(char *buf, int size, {name} value)\n{{\n'
                       f'\tSTUB_CALLED("Value_To_String({name}) (0x{v2s:X})");\n'
                       f'\t_snprintf(buf, size, "%d", (int)value);\n\treturn buf;\n}}\n')
        # no stand-in left (every conversion rebuilt): the committed file ends in two blank lines
        return '\n'.join(out) + ('\n\n' if len(out) == 1 else '')


STUBS = os.path.join(REPO, 'stubs', 'aux_roots_stubs.cpp')
STUBS_HEAD = r'''// Earth & Beyond client (reconstruction): GENERATED by tools/re/auxroots.py, do not edit.
// Link-time stand-ins (D44) for the aux-data value types' conversions the client calls out of line
// (other files' functions, not rebuilt: ENUM_EXTERN). Each logs its first call (stub_log.h); Type_Name
// returns the client's string, the others treat the value as a number.
#include <stdio.h>
#include "stub_log.h"
#include "..\app\client\aux_roots.h"
'''


# The roots other files build inline (INLINE_SEND) and the value types only they use, in a header of their
# own: aux_roots.cpp doesn't include it, so they don't move its SYMBOL_PAD windows (workflow.md 3.3; with
# Hull in aux_roots.h no pad had all six constructors exact, 3/6 at best).
SEND_HEADER = r'''// Earth & Beyond client (reconstruction): GENERATED by tools/re/auxroots.py, do not edit.
// The aux-data roots other files build inline (auxroots.py INLINE_SEND: Hull, SClient 0x7518A0), the
// value types only they use and the constructors their file compiles (SEND_HEADER_ROOTS: Player's tree,
// interim D137). NOT EXACT (D42) where aux_roots.h is.
// NAME 1 [aux_roots_send.h]: provisional file name (a build aid: kept out of aux_roots.cpp, workflow.md 3.3)

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __AUX_ROOTS_SEND_H
#define __AUX_ROOTS_SEND_H

#include "aux_roots.h"

%(types)s

%(classes)s

%(defs)s#endif //__AUX_ROOTS_SEND_H
'''

# The group's headers (GROUP_VTS: the note there).
GROUP_HEADER = r'''// Earth & Beyond client (reconstruction): GENERATED by tools/re/auxroots.py, do not edit.
// The group classes (Player.GroupInfo and its members), in a header of their own so that an object can parse them
// before aux_roots.h's classes (aux_group.h, aux_group_ship.h): the Delta templates number in the order their
// classes are first instantiated (auxroots.py GROUP_VTS). aux_roots.h includes it where they were. NOT EXACT (D42)
// where aux_roots.h is.
// NAME 1 [aux_roots_group.h]: provisional file name (the client's headers are not named by any string)

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __AUX_ROOTS_GROUP_H
#define __AUX_ROOTS_GROUP_H

#include "..\shared\auxdata\delta_template.h"
#include "..\shared\auxdata\delta_string.h"
#include "..\shared\auxdata\delta_item_array.h"
#include "aux_formation.h"

%(classes)s

// the constructors here, unless the object compiles the player's tree (aux_roots_send.h includes them in its order)
#if !defined(AUX_CTORS_PLAYER)
#include "aux_group_ctors.h"
#endif

#endif //__AUX_ROOTS_GROUP_H
'''

GROUP_CTORS_HEADER = r'''// Earth & Beyond client (reconstruction): GENERATED by tools/re/auxroots.py, do not edit.
// The group's constructors (Player's tree: GroupInfo 0x7548D0, GroupMember 0x754DB0), behind the group family's
// gate only: aux_roots_group.h includes them, or aux_roots_send.h in their place in Player's tree (auxroots.py
// GROUP_VTS). NOT EXACT (D42) where aux_roots.h is.
// NAME 1 [aux_group_ctors.h]: provisional file name (the client's headers are not named by any string)

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __AUX_GROUP_CTORS_H
#define __AUX_GROUP_CTORS_H

#include "aux_roots_group.h"

%(defs)s#endif //__AUX_GROUP_CTORS_H
'''

SPLIT_HEADER = r'''// Earth & Beyond client (reconstruction): GENERATED by tools/re/auxroots.py, do not edit.
// %(what)s
// Included back in its place (aux_roots.h; Hull's in aux_roots_send.h); the family headers (aux_<family>.h) include
// it first, in the order the client's objects number the classes (auxroots.py SPLITS).
// NOT EXACT (D42) where aux_roots.h is.
// NAME 1 [%(name)s]: provisional file name (the client's headers are not named by any string)

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef %(guard)s
#define %(guard)s

%(includes)s
%(body)s

#endif //%(guard)s
'''
TYPES_WHAT = "The aux-data value types: their type names and conversions (aux_roots.h's value types)."
HULL_WHAT = "The Hull root (built inline in SClient 0x7518A0) and its value type."
TYPES_INCLUDES = r'''#include <stdio.h>
#include <string.h>
#include "..\shared\auxdata\unknown_root.h"
#include "..\shared\auxdata\delta_template.h"
#include "..\shared\auxdata\delta_string.h"
#include "..\shared\auxdata\delta_array.h"
#include "..\shared\auxdata\delta_item_array.h"
#include "..\shared\money.h"
#include "..\shared\strconv_lib.h"
#include "..\shared\g_assert.h"
#include "..\shared\auxdata\delta_money.h"
'''

FORMATION_HEADER = r'''// Earth & Beyond client (reconstruction): GENERATED by tools/re/auxroots.py, do not edit.
// The groups' value type FormationState and its pair's type name, for aux_roots.h and aux_roots_group.h (auxroots.py
// GROUP_VTS).
// NAME 1 [aux_formation.h]: provisional file name (the client's headers are not named by any string)

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __AUX_FORMATION_H
#define __AUX_FORMATION_H

#include "..\shared\kv_pair.h"

%(formation)s

#endif //__AUX_FORMATION_H
'''

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
#include "..\shared\g_assert.h"

// The money field is delta_money.h's specialisation: every member out of line in delta_money.cpp
// (the constructor 0x4A7080 and destructor 0x4A7100 are called, the destructor under an EH state
// as one that can throw: Player's destructor 0x7551D0, the Item destructors 0x6FA4C0 / 0x702750)
#include "..\shared\auxdata\delta_money.h"


// ------------------------------------------------------------------------- value types

// The int and bool conversions and type names are strconv_lib.h's (the client's are out of line,
// unknown_strconv.cpp 0x4A30A0-0x4A3640)
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

// The factory 0x6F6000 is ClientObjectClass's member (co.h; `ret 8`, called with the object in ecx)
// though it uses nothing of the object; aux_roots.cpp defines it.

%(inline_roots)s#endif //__AUX_ROOTS_H
'''

# VC6 symbol pad (HX1, 2026-10-28; workflow.md 3.3): every declaration the file sees (a header's member, an
# extern, an enumerator) moves the template constructors compiled at its end (0x6FB1E0, 0x702240,
# 0x7045F0, 0x7014B0, 0x701D70, 0x758210) by one step of a cycle of 64: each matches only in a window of
# it. The pad's enumerators put the file inside the windows; retune SYMBOL_PAD when a header aux_roots.cpp
# includes gains or loses declarations (.work/hx1/exp.py sweeps it by compiling aux_roots.cpp alone).
SYMBOL_PAD = 42
# Definition order in aux_roots.cpp, a second lever (SC2, 2026-11-08d): with DeltaItemArrayClass<DeltaStringClass>
# (MountBoneNames) instantiated, no pad holds the six with the client's address order (0x7014B0 and 0x7045F0
# share only 57..59, where 0x6FB1E0 fails); PrimaryCategory's inline constructor (0x6FB1E0) defined last moves
# its window alone, and 57..59 hold all of them. Address -> the sort key used instead (not the client's order).
DEF_ORDER = {0x6FB1E0: 0x707431}
PAD = '''
// VC6 symbol pad (not the client's code): the constructors above match only when the count of the
// declarations this file sees falls in a window of a cycle of 64 (tools/re/auxroots.py SYMBOL_PAD)
enum AuxRootsSymbolPadType { %s };	// NAME 1: a build aid (HX1)
'''

# The same pad for sclient.cpp, at the end of aux_roots_send.h (SC1, 2026-11-08): with Player's tree there,
# DeltaItemArrayClass<DeltaFactionClass>'s constructor 0x758210 (Reputation's) compiles in sclient.cpp and is
# exact only in a window of its declaration count (.work/sc1/scsweep.py: 33-37 since the testeffects.ini
# loader 0x74CA90 joined sclient.cpp; there sclient's shared copy of 0x702240 is out of its window, aux_roots keeps it).
# It moves sclient's 0x74D990 too (defined after aux_roots_send.h): 41-61 hold 0x758210, 0x74D990 and 0x702240 since
# UnknownCOClass joined co.h, sclient.cpp includes control_keys_manager.h at its top, SB3 changed sbaseview.h and
# WF1 made FontSys's destructor implicit
# (PH1, .work/ph1/sweep.py sc). 2026-11-09b PD1: sclient.cpp including gadget.h (Do_Frame calls
# GadgetClass::Process_Input by name) moved the window to 9-14 (importer path, .work/pd1/sendpad_imp.py); SB1's
# sbaseview.h / pkstarbase_avatar_change.h retypes moved it to 36-38 (PD1, same sweep). AV1's avatar.h in sclient.cpp (the +0x1394 values as
# CharacterViewAvatarClass): 40-42 (and 21 alone; same sweep). TX1's WW3D::Sync by name in Do_Frame (was the gen
# stand-in Unknown_918200): 42-44 (and 22-23; same sweep). GS1's sclient.cpp rebuilds (0x743380, 0x744B60, 0x744E50)
# and ui_scut.h's duplicate 0x5417D0 gone: 29-31 (and 26; same sweep). GS1's 0x744560 (pkuitrigger.h after auxdata.h):
# 46-49 (and 44; same sweep). ST1's sbaseview.h retype (AvatarUI +0x19C onto ui_avatar_cust.h's
# StarbaseAvatarCreationInterface, the stand-in class gone): 1-7 (and 62-63; .work/st1/sendpad_imp.py).
# TC1's ui_stringentry.h (UIPTradeCreditsClass out of sclient.h): 0x758210 and 0x702240 both only at 33 and 55-57: 56.
SEND_SYMBOL_PAD = 23
SEND_PAD = '''// VC6 symbol pad (not the client's code): sclient.cpp's copy of 0x758210 matches only when the count of
// the declarations it sees falls in a window (tools/re/auxroots.py SEND_SYMBOL_PAD)
enum AuxRootsSendSymbolPadType { %s };	// NAME 1: a build aid (SC1)

'''

SOURCE = r'''// Earth & Beyond client (reconstruction): GENERATED by tools/re/auxroots.py, do not edit.
// The aux-data roots' constructors and the CREATE factory (M1.3, D43). NOT EXACT (D42): functional.
// NAME 1 [aux_roots.cpp]: provisional file name (the client's constructors are in co.cpp 0x6F6000-0x6FBEF0 and the Player object 0x751BE0)
#include "aux_roots_all.h"
#include "co.h"	// ClientObjectClass: the factory 0x6F6000 is its member

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
UnknownRootClass *ClientObjectClass::Unknown_Create_Aux_Root(int type, int unknown)
{
	switch (type) {
%(cases)s
	}
	return 0;
}'''


def family_gate(text):
    """text (aux_roots.h / aux_roots_send.h) with each constructor body behind its family's gate (FAMILY_INCLASS,
    FAMILY_OUTOFCLASS; AS's .work/as/gate.py)."""
    def cond(fam):
        return '#if defined(AUX_ROOTS_ALL)' + ''.join(' || defined(AUX_CTORS_%s)' % f
                                                      for f in (fam if isinstance(fam, tuple) else (fam,)))
    L = text.split('\n')
    out = []
    i = 0
    cls = None
    while i < len(L):
        line = L[i]
        m = re.match(r'^class Delta(\w+)Class\b', line)
        if m:
            cls = m.group(1)
        m = re.match(r'^\tDelta(\w+)Class\((.*?)\)( :.*| \{.*)?$', line)
        if m and m.group(1) == cls and cls in FAMILY_INCLASS and m.group(3):
            # in-class constructor: to the line closing its body ('\t}' or the one-liner itself)
            j = i
            if not line.rstrip().endswith('}'):
                while L[j] != '\t}':
                    j += 1
            out.append(cond(FAMILY_INCLASS[cls]))
            out.extend(L[i:j + 1])
            out += ['#else', '\tDelta%sClass(%s);' % (cls, m.group(2)), '#endif']
            i = j + 1
            continue
        m = re.match(r'^(inline )?Delta(\w+)Class::Delta(\w+)Class\(', line)
        if m and m.group(2) == m.group(3) and m.group(2) in FAMILY_OUTOFCLASS:
            # out-of-class inline definition: from its comment line(s) / 'inline' line to its closing '}'
            k = len(out)
            while k > 0 and (out[k - 1].startswith('//') or out[k - 1] == 'inline'):
                k -= 1
            j = i
            while L[j] != '}':
                j += 1
            head = out[k:]
            del out[k:]
            out.append(cond(FAMILY_OUTOFCLASS[m.group(2)]))
            out += head + L[i:j + 1] + ['#endif']
            i = j + 1
            continue
        out.append(line)
        i += 1
    return '\n'.join(out)


def family_body(out_dir, header, body):
    """Write a FAMILY_MOVED constructor's body into its family header in place (from the body's comment line to
    its closing '}'); warn when the header has no such block."""
    path = os.path.join(out_dir, header)
    first = body.split('\n', 1)[0]
    try:
        with open(path, encoding='latin-1', newline='') as f:
            text = f.read()
    except OSError:
        print(f'auxroots: {header} missing: its constructor ({first}) is not written', file=sys.stderr)
        return
    i = text.find(first + '\n')
    j = text.find('\n}\n', i)
    if i < 0 or j < 0:
        print(f'auxroots: {header} has no block {first!r}: not written', file=sys.stderr)
        return
    new = text[:i] + body + text[j + 2:]
    if new != text:
        with open(path, 'w', encoding='latin-1', newline='\n') as f:
            f.write(new)
        print(f'auxroots: {header}: {first} rewritten')


def generate(doc, out_dir):
    g = Gen(doc)
    ctors = []
    inline_roots = []      # the factory's roots: inline (header), as the client's (see the note at SEND_ROOTS)
    roots = doc['roots']
    g.prepare(roots)
    send_vts, send_types = set(), set()     # INLINE_SEND's classes and value types: aux_roots_send.h
    send_defs = []                          # SEND_HEADER_ROOTS' constructors: aux_roots_send.h
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
        before = (set(g.order), set(g.types))
        nested_before = len(g.nested)
        g.compound(r, rname, root=rname)
        if rname in SEND_HEADER_ROOTS:
            # the nested constructors first (address order), the root's last (the note at SEND_HEADER_ROOTS)
            send_defs += sorted(g.nested[nested_before:], key=lambda x: x[0])
            del g.nested[nested_before:]
        if rname in INLINE_SEND:
            send_vts |= set(g.order) - before[0]
            send_types |= set(g.types) - before[1]
        if g.root_ctor is None:
            continue
        text = (f'// {rname}' + (f' (0x{ROOT_CTOR[rname]:X})' if rname in ROOT_CTOR else ' (built in the factory 0x6F6000)')
                + '\n' + g.root_ctor)
        if rname in SEND_HEADER_ROOTS:
            send_defs.append((ROOT_CTOR[rname], text.replace(f'\n{g.classes[int(r["vt"], 16)]}::', f'\ninline\n{g.classes[int(r["vt"], 16)]}::', 1)))
        elif rname in SEND_ROOTS:
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
    classes = '\n\n'.join(g.decl[vt] for vt in g.order if vt not in send_vts)
    send = SEND_HEADER.replace('%(types)s', g.type_decls(only=send_types)).replace(
        '%(classes)s', '\n\n'.join(g.decl[vt] for vt in g.order if vt in send_vts)).replace(
        '%(defs)s', ''.join(text + '\n\n' for _, text in send_defs) +
        (SEND_PAD % ', '.join(f'AUX_ROOTS_SEND_PAD_{i:02d}' for i in range(SEND_SYMBOL_PAD)) if SEND_SYMBOL_PAD else ''))
    h = HEADER.replace('%(types)s', g.type_decls(skip=send_types)).replace('%(classes)s', classes).replace(
        '%(inline_roots)s', ''.join(text + '\n\n' for _, text in sorted(inline_roots)))
    send = family_gate(send)
    h = family_gate(h)
    # the group's own headers (GROUP_VTS): the classes, FormationState and the constructors out of the two
    group_classes = '\n\n'.join(g.decl[vt] for vt in GROUP_VTS)
    assert h.count(group_classes) == 1, 'the group classes are not adjacent in aux_roots.h'
    h = h.replace(group_classes, '#include "aux_roots_group.h"' + '\n' * group_classes.count('\n'))
    formation = HAND_TYPES[FORMATION_VT][1]
    assert h.count(formation) == 1
    h = h.replace(formation, '#include "aux_formation.h"' + '\n' * formation.count('\n'))
    group_defs = [text for a, text in send_defs if a in GROUP_CTORS]
    old = family_gate(''.join(text + '\n\n' for text in group_defs))
    gated = old.replace('#if defined(AUX_ROOTS_ALL) || defined(AUX_CTORS_GROUP)', '#if defined(AUX_CTORS_GROUP)')
    assert gated.count('#if defined(AUX_CTORS_GROUP)') == len(GROUP_CTORS)
    assert send.count(old) == 1, 'the group constructors are not adjacent in aux_roots_send.h'
    send = send.replace(old, '#include "aux_group_ctors.h"\n\n')
    group = GROUP_HEADER.replace('%(classes)s', group_classes)
    group_ctors = GROUP_CTORS_HEADER.replace('%(defs)s', gated)
    formation_h = FORMATION_HEADER.replace('%(formation)s', formation)
    # the send roots' classes before the ship's (SPLIT_HEADERS: the note there): the value types, the item classes,
    # Hull and the player's tree each in a header of their own, included back in their places (line-neutral)
    by_name = {name: vt for vt, name in g.classes.items()}
    order = [vt for vt in g.order if vt not in send_vts]

    def cut(text, first, last, header):
        """text with the classes first..last (class names, adjacent in `order`) replaced by an include of header
        (and as many blank lines as the block had); returns (text, the block)."""
        a = text.index(g.decl[by_name[first]].split('\n', 1)[0] + '\n')
        nxt = order[order.index(by_name[last]) + 1]
        b = text.index('\n' + g.decl[nxt].split('\n', 1)[0] + '\n', a)
        block = text[a:b].rstrip('\n')
        return text[:a] + f'#include "{header}"' + '\n' * block.count('\n') + text[a + len(block):], block
    ts = h.index(TYPES_MARK)
    te = h.index(CLASSES_MARK)
    types_block = h[ts:te].rstrip('\n')
    h = h[:ts] + '#include "aux_roots_types.h"' + '\n' * types_block.count('\n') + h[ts + len(types_block):]
    split_files = []
    for header, first, last, what, incs in SPLITS:
        h, block = cut(h, first, last, header)
        guard = '__' + header.upper().replace('.', '_')
        split_files.append((header, SPLIT_HEADER % {'name': header, 'guard': guard, 'what': what, 'body': block,
                                                    'includes': ''.join(f'#include "{x}"\n' for x in incs)}))
    if SPLIT_SWAP:
        a, b = (f'#include "{x}"' for x in SPLIT_SWAP)
        assert h.count(a) == 1 and h.count(b) == 1
        h = h.replace(a, '\0').replace(b, a).replace('\0', b)
    hull_types = g.type_decls(only=send_types)
    hull_classes = '\n\n'.join(g.decl[vt] for vt in g.order if vt in send_vts)
    hull_block = family_gate(hull_types + '\n\n' + hull_classes)
    assert send.count(hull_block) == 1, 'the send types and classes are not adjacent in aux_roots_send.h'
    send = send.replace(hull_block, '#include "aux_roots_hull.h"')
    types_h = SPLIT_HEADER % {'name': 'aux_roots_types.h', 'guard': '__AUX_ROOTS_TYPES_H', 'what': TYPES_WHAT,
                              'includes': TYPES_INCLUDES, 'body': types_block}
    hull_h = SPLIT_HEADER % {'name': 'aux_roots_hull.h', 'guard': '__AUX_ROOTS_HULL_H', 'what': HULL_WHAT,
                             'includes': '#include "aux_roots_types.h"\n', 'body': hull_block}
    # the client's order (its objects compile in address order: VC6's inlining depends on what came before)
    moved = {}      # FAMILY_MOVED: header -> the body, an include line in its place
    for k, (a, text) in enumerate(g.nested):
        if a in FAMILY_MOVED:
            header, note = FAMILY_MOVED[a]
            moved[header] = text
            g.nested[k] = (a, f'#include "{header}"\t{text.split(chr(10), 1)[0]}: {note}')
    ctors += g.nested + [(0x6F8C70, SHIP_ADD), (0x6F6000, FACTORY.replace('%(cases)s', '\n'.join(cases)))]
    c = SOURCE.replace('%(ctors)s', '\n\n'.join(text for _, text in sorted(ctors, key=lambda x: DEF_ORDER.get(x[0], x[0]))))
    pad = PAD % ', '.join(f'AUX_ROOTS_PAD_{i:02d}' for i in range(SYMBOL_PAD)) if SYMBOL_PAD else ''
    c = c.replace('%(pad)s', pad)
    os.makedirs(out_dir, exist_ok=True)
    for name, text in (('aux_roots.h', h), ('aux_roots.cpp', c), ('aux_roots_send.h', send), ('aux_roots_group.h', group),
                       ('aux_group_ctors.h', group_ctors), ('aux_formation.h', formation_h), ('aux_roots_types.h', types_h),
                       ('aux_roots_hull.h', hull_h)) + tuple(split_files):
        with open(os.path.join(out_dir, name), 'w', newline='\n') as f:
            f.write(text)
    if os.path.normcase(os.path.abspath(out_dir)) == os.path.normcase(OUT_DIR):
        for header, body in sorted(moved.items()):
            family_body(out_dir, header, body)
        with open(STUBS, 'w', newline='\n') as f:
            f.write(g.stub_source())
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
