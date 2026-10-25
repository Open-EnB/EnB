# The decode path (M1.1)

The client's AUX_DATA receive path, from `AuxDataPacketClass::Process_Packet` 0x7364C0 to the leaves, with
each function's status as of 2026-10-24. Moved verbatim from milestones.md §2.3; reference data about the client.

Mapped 2026-10-24 from `AuxDataPacketClass::Process_Packet` 0x7364C0 to the leaves (scripts and
the full call graph: `.work/milestones/m1/m11/`, untracked, historical). **Exact** = the address is in
`docs/data/imports.json` app `in_tree` (the importer's unique `exact` match); **in tree** = defined
in `app/shared` but not counted exact; **missing** = no tree code yet. Sizes are from the
disassembly (start to last instruction). Names ending in `?` are descriptive guesses.

**How it works.** `Process_Packet` wraps the payload in a `LinearBufferClass` (0x400) and picks
the root: game id 0 → `[client+0x12C0]`, else the object found by 0x727070, root at `obj+0x88`.
It calls 0x4A3FA0 on the root with `(buf, &base)` (`base`: 8 bytes at `client+0x10C0`), which reads
one encoding byte. **0 → record form**: the static codec at 0xBE3AB4 (vtable 0xAEB29C, slot 0
0x4A8FD0) reads three id lists (updated → slot +0x10, defaulted → +0x18, cleared → +0x1C), each id
resolved by `LookupClass::Get_Delta`; the lookup is built on first use (0x4A5510 → 0x4AC250, a
visitor 0xAEB318 over the tree). **Non-zero → tree form**: the heap codec `[0xBE3B00]` (0x4A9B80;
vtables 0xAEB2AC: Decode 0x4A9350, header bits 0x4A9710; visitor 0xAEB2A0 at +4: Visit(leaf)
0x4A9390, Visit(compound) 0x4A9400) is double dispatch: `root->Accept` (+0x08) → Visit(compound)
reads the 4 header bits and the per-child bitsets, then each child's Accept; Visit(leaf) switches
on the mode: 0 → +0x10 (Read +0x2C, then +0x3C), 1 → +0x18, 2 → +0x1C. The policy, the
`RecordTableClass` and the interpolators (apart from one +0x3C override) are not on the decode
path. Item arrays and records are compounds and decode through the shared compound slots. All
indirect calls are resolved: the Delta slots over all 87 Delta vtables. Ghidra's pseudocode for
0x4A9400 is garbled; use `client.py dis` for it.

| Layer | Address | Name / class | Size | Source | Status | Reached |
|---|---|---|---|---|---|---|
| entry | 0x7364C0 | `AuxDataPacketClass::Process_Packet` | 315 | app\client\cpacket.cpp (not in tree) | missing: not rebuilt (it needs the packet base class, `ClientClass` +0x10C0 / +0x12C0, 0x727070 and the profiler; the harness does its body: `LinearBufferClass(data, len, 0x400, false)`, then `root->Decode(buf, base)`) | opcode 0x1B |
| entry | 0x736650 | `AuxDataPlayerVarPacketClass::Process_Packet` (0x1C, also calls 0x4A3FA0) | 199 | cpacket.cpp | missing | opcode 0x1C |
| entry | 0x727070 | find game object by id? (99 callers) | 137 | app\client\client.cpp | missing | id ≠ 0 |
| entry | 0x4AABE0 / 0x4AACB0 | `LinearBufferClass` ctor / dtor | 149 / 17 | auxdata/linearbu.cpp | exact | direct |
| select | 0x4A3FA0 | `DeltaCompoundClass::Decode(buf, base)`: the encoding byte | 58 | unattributed object 0x4A3990-0x4A4780 (also compound base ctor 0x4A3BD0) | functional, byte-exact under appfile.py match; in stubs/app_stubs.cpp until the object has a file (not counted) | direct |
| select | 0x4A9B80 | `Get_Tree_Codec` (`[0xBE3B00]`) | 6 | encode.cpp | in tree (`short`: too short to count) | direct |
| select | 0x4A9190, 0x4A9970, 0x4A99E0 | static init: `RecordCodec`, `TreeCodecHolder`, `TreeCodecHolderClass` ctor | 11, 27, 67 | encode.cpp | 0x4A99E0 exact; the two initialisers in tree (`short`) | CRT init |
| tree | 0x4A9350 | `TreeCodecClass::Decode` | 46 | encode.cpp | exact | 0xAEB2AC slot 0 |
| tree | 0x4A9390 | `TreeCodecClass::Visit(DeltaClass *)` | 72 | encode.cpp | exact | 0xAEB2A0 slot 0 |
| tree | 0x4A9400 | `TreeCodecClass::Visit(DeltaCompoundClass *)` (assert line 401) | 622 | encode.cpp | exact (2026-10-24 evening: early returns, the mode chosen inside the "any state" test) | 0xAEB2A0 slot 1 |
| tree | 0x4A9710 | `TreeCodecClass::Read_Header` | 243 | encode.cpp | exact | 0xAEB2AC slot 1 |
| record | 0x4A8FD0 | `RecordCodecClass::Decode` (3 id lists) | 352 | encode.cpp | exact | 0xAEB29C slot 0 |
| record | 0x4A5510 | `DeltaCompoundClass::Get_Lookup` | 136 | auxdata/delta.cpp | exact | direct |
| record | 0x4AC850 | `LookupClass::Get_Delta(int)` | 89 | auxdata/lookup.cpp | exact | direct |
| record | 0x4AC250 | `LookupClass::LookupClass(owner)` | 271 | lookup.cpp | exact | first use |
| record | 0x4AC3B0 / 0x4AC560 | `LookupBuilderClass::Visit` (leaf / compound) | 327 / 340 | lookup.cpp | exact | 0xAEB318 slots 0/1 |
| record | 0x4ACC60, 0x4ACF30, 0x4ACF00, 0x4ACBB0 | `vector::insert`, map `_Insert`, `pair` ctor, `begin` | 521, 613, 22, 14 | lookup.cpp (STL instances) | 0x4ACF30 exact; the other three exact, ambiguous address (D54) | direct |
| record | 0x4AD280, 0x4AD300 | map `_Dec`, ? | 81, 23 | name.cpp object ("not rebuilt" range) | in tree via lookup.cpp's map (`multiple`, copies differ in their `_Nil`) | direct |
| record | 0x4B0860 | `DeltaVisitorClass::Visit_Children` | 68 | unattributed | functional, in stubs/app_stubs.cpp until the object has a file | direct |
| record | 0x4ADBB0 / 0x4ADE20 | `NameClass::Get_Rest` / PermString intern | 119 / 359 | auxdata/permstr.cpp | exact | direct |
| delta | 0x4A4FE0 / 0x4A5650 | `DeltaClass::Accept` / `DeltaCompoundClass::Accept` | 14 / 15 | auxdata/delta.cpp | in tree (3 identical copies: `multiple`) | slot +0x08 |
| delta | 0x4A4E80 / 0x4A5300 | `DeltaClass` / `DeltaCompoundClass::Unknown_10` | 42 / 113 | delta.cpp | exact | slot +0x10 |
| delta | 0x4A4DB0, 0x4A4DD0, 0x4A5410, 0x4A5490 | `Unknown_18` / `Unknown_1C` (leaf, compound) | 14, 14, 81, 81 | delta.cpp | exact | slots +0x18/+0x1C |
| delta | 0x4A4F20, 0x4A4F50, 0x4A4F80 | `DeltaClass::Unknown_3C` / `_40` / `_44` | 27, 27, 20 | delta.cpp | exact | slots +0x3C/+0x40/+0x44 |
| delta | 0x4A68E0 | interpolator `Unknown_3C` override (vtable 0xAEAE54) | 51 | auxdata/delta_interpolator.cpp | exact | slot +0x3C |
| delta | 0x4A3C70 | `DeltaClass::Read` default (compounds) | 5 | delta.h (inline) | in tree (26 copies) | slot +0x2C |
| leaf | 0x4A6DF0 | `DeltaTemplateClass<float>::Read` (0xAEAEB0, 4 bytes) | 66 | delta_template.h | exact, ambiguous address (D54: 18 copies, the same apart from the header's `__FILE__` directory) | slot +0x2C |
| leaf | 0x4A7470 | money Read (0xAEAFB8, 8) | 85 | delta_money.cpp | exact | slot +0x2C |
| leaf | 0x4A7CA0 | string Read (0xAEB0E4, length + text) | 219 | delta_string.cpp | exact | slot +0x2C |
| leaf | 0x4A81D0 | time Read (0xAEB1EC, 4, minus base) | 114 | delta_time.cpp | exact | slot +0x2C |
| leaf | 0x7013F0, 0x702B70, 0x702D90, 0x702FB0, 0x7031D0, 0x703C90, 0x7040F0, 0x704310, 0x704530, 0x704BF0, 0x704E10, 0x705030, 0x705390 | 4-byte template Reads (int32 0xB00D34, ...) | 13 × 66 | unattributed object 0x7008B0-0x70A010 (after co.cpp) | functional: the template's Read (delta_template.h), byte-identical to 0x4A6DF0; counted when M1.3's files instantiate it | slot +0x2C |
| leaf | 0x757D50, 0x757F70, 0x7578D0, 0x758770 | 4-byte template Reads | 4 × 66 | unattributed object 0x751BE0-0x75A4E0 (Player ctor's) | functional, as above | slot +0x2C |
| leaf | 0x757B00 | 16-byte Read (0xB03694) | 116 | same object | functional: the template's Read for a 16-byte T whose constructor zeroes it (M1.3 supplies T) | slot +0x2C |
| leaf | 0x703490, 0x703EC0, 0x706F60, 0x705640 | bool (1), color (12), 0xB016DC (12), 0xB01A40 (20) Reads | 66, 95, 106, 155 | 0x700xxx object | functional: the template's Read with T's size (1, 12, 12, 20; 0x705640's T starts as {1.0f, 0, 0, -1, 0}); M1.3 supplies the types | slot +0x2C |
| helper | 0x4AACE0 | `LinearBufferClass::Read(dst, n)` | 135 | linearbu.cpp | exact | everywhere |
| helper | 0x4AB280 / 0x4AB2A0 | `LinearBufferClass::Get_Pos` / seek? | 4 / 43 | linearbu.cpp | in tree (12 copies) / exact | from 0x4A9710 |
| helper | 0x4A47B0, 0x4A4AA0 / 0x4A4960 | `BitSetClass` ctor, bit test / dtor | 136, 72 / 10 | auxdata/bitset.cpp | exact / in tree (5 copies) | from 0x4A9400/0x4A9710 |
| helper | 0x4E9410, 0x4E9CB0, 0x4E9D10 | `G_ASSERT`, E&B `operator new` / `delete` | 39, 18, 22 | unknown_debug.h (header only) | missing | direct |

**Roots** (top level only; M1.3): CREATE factory 0x6F6000 (1,258 B, co.cpp; byte index table
0x6F6518, 11-entry jump table 0x6F64EC; called from 0x6F2820, 839 B, the CREATE handler?); Ship
0x6F66E0 (6,352 B, 232 calls: `Add` ×55, ship narrow Add 0x6F8C70 ×54, `DeltaClass` ctor ×33,
leaf ctor 0x700B80 ×33, nested compound 0x6F85F0 ×9); Player 0x752510 (1,895 B, 70 calls);
Harvestable 0x6F9ED0 (498), ManufacturingLab 0x6FA6F0 (2,201), Decoration 0x6F9C30 (237), Light
0x6F9870 (570), Component 0x6FBDA0 (123). Shared builders: `DeltaCompoundClass::Add` 0x4A5670
(405 B, in tree, not exact), compound base ctor 0x4A3BD0 (92, missing), 0x6F8C70 (32), 0x6F85F0
(76), 0x6FA150 (642) (co.cpp, missing); 0x4A4D40 and 0x4A78F0 exact. **M1.3 (2026-10-24):** all eleven roots and their 42 nested compounds are generated (app/client/aux_roots.cpp); 0x4A3BD0 (`UnknownRootClass`, unknown_root.cpp), 0x6F8C70, 0x6F85F0 exact, 0x6FA150 not.

**Size of M1.2 (per-packet path; roots and startup init excluded).** 24 functions on the path are
already exact.

* **In tree, not exact: 6 functions, 114 B** (0x4A4FE0, 0x4A5650, 0x4A3C70, 0x4A4960, 0x4AB280,
  0x4A6DF0). None differs from the client: their bytes occur 3-26 times in client.exe, so
  objmatch says `multiple`/`short` and the importer counts only unique `exact`. Counting them
  is a matter of how the importer counts, not of code.
* **Missing: 41 functions, 5,953 B.**
  * Packet, codec select and both decoders: 9 functions, 2,013 B (0x7364C0, 0x727070, 0x4A3FA0,
    0x4A9B80, 0x4A9350, 0x4A9390, 0x4A9400, 0x4A9710, 0x4A8FD0).
  * Record-form lookup build (first use): 10 functions, 2,280 B (0x4AC250, 0x4AC3B0, 0x4AC560,
    0x4ACC60, 0x4ACF30, 0x4ACF00, 0x4ACBB0, 0x4AD280, 0x4AD300, 0x4B0860).
  * Leaf Reads in client files: 22 functions, 1,660 B. 17 of them (1,122 B) are the 4-byte
    `DeltaTemplateClass<T>::Read` body, which comes with its instantiating files (and will also
    match only as `multiple`). The 5 distinct ones are 538 B.
  * Also: startup init 0x4A9190 / 0x4A9970 / 0x4A99E0 (105 B), `G_ASSERT` / `new` / `delete`
    (79 B), and 0x736650 (199 B) for the 0x1C form.
