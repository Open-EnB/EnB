# Milestones M1 and M2 (accepted 2026-10-24: D42-D48)

Two milestones for the next stretch of work. Both test the rebuild by running it, not only by
matching bytes. Once both are met, work returns to the client kernel and the rest of
[progress.md](progress.md) §4.

* **M1: a capture harness.** The rebuilt `app\shared\auxdata` and `app\wwudp` classes, linked
  into a small test program, process the stored packet captures ([prior-work.md](prior-work.md)
  §3) and give the same results as the reference decoder's `the reference decoder`.
* **M2: a working VIDTEST.** A program equivalent to `EBCONFIG\VIDTEST.EXE`, built on the tree's
  Westwood libraries (the 2004 client versions, not VIDTEST's 2002 ones), renders E&B's original
  art: models, textures, lighting, the test scenes. It doesn't need to be byte-exact (D36).

The two milestones share one new piece of work: **the project's first link step** (§3). Once the
tree's code is linked into an executable, every untested assumption in it, such as stubs and
guessed field layouts, shows up when the program runs.

The user accepted all seven proposals in §5 (P1-P7) on 2026-10-24, recorded as D42-D48 in
progress.md. P4 was refined after a check: VIDTEST's scene classes follow the client's layout
(D45).

## 1. What exists

| | State (2026-10-24) |
|---|---|
| auxdata | 21 files in `app/shared/auxdata` and `app/shared`, app library 136 byte-exact functions. Delta model, leaf Read/Write, bitset, names, lookup, buffers, part of encode.cpp (the codecs) and policy.cpp |
| Root schemas | Not started. The 13 roots are built in client constructors (Ship 0x6F66E0, Player 0x752510, ...; CREATE factory 0x6F6000) outside `auxdata`. the reference decoder's extractor already reads them out of the client (`the reference game model` JSON: Ship 1,933 nodes, Player 5,596) |
| wwudp | Not started. Client files `cordmgr.cpp`, `cordcon.cpp`, `bandwidthaccounting.cpp` (26 functions attributed so far, more unattributed). The protocol is described in the reference decoder `docs/design.md` §16.4. The crypto it uses, WWLib's `RC4Class`, `PKey` and XMP, is already imported |
| Westwood libraries | ww3d2 2477, wwshade 159, wwmath 370, wwlib 565, wwaudio 351, wwsaveload 80, wwdebug 81 byte-exact. Everything else is compiled from EA's source plus patches, and has never run |
| Link | None. The VMs compile objects for `verify`; no `.lib` or `.exe` has been built |
| VIDTEST | `.work/ebconfig/assessment.md`: 437 app functions (about 120K), of which 252 also occur in client.exe. Plain Win32 window, D3D8 through WW3D2, no Miles or Bink. Its art is in `C:\Program Files (x86)\EA GAMES\Earth & Beyond\EBCONFIG` (171 files) |

**Captures.** The EA captures are at `C:\EnBDevelopment\Live-Era-Data\enbserver3\ENB-Packets-Live`.
prior-work.md had `C:\EnBDevelopment\enbserver3\...`; corrected with this plan.

| Set | Form | Content |
|---|---|---|
| `capture_1..3.txt` (EA) | annotated datagrams; handshake fields labelled, RC4 session key printed, messages decoded to `Length / Opcode / payload` | about 120,000 datagrams, 165,882 messages |
| `Alex3-fixed`, `Alex7-fixed` (EA) | per session: RC4 key, then decrypted datagrams with sequence numbers | 26,078 + 2,779 messages; 4,651 AUX_DATA |
| `the reference traces\captures\ea\*.trace.jsonl` | the reference decoder's imports of the five EA files (`the reference decoder import-ea`, 2026-10-21), one game message per line `{t, channel, dir, op, name, len, data}`, plus `note` (packet numbers) for `capture_N` and `seq` for the Alex sessions | 124,208 / 16,418 / 25,256 / 26,078 / 2,779 messages; 43,978 AUX_DATA in `capture_1..3` (34,070 / 4,741 / 5,167), 4,658 in the Alex sessions. Message level only: datagrams, block headers and ack lists are gone |
| `the reference traces\captures\session_*` (Net-7) | pcapng + trace.jsonl, client to proxy over TCP | 2026-10-19/21 recordings, with three known AuxData encoding quirks |

All AUX_DATA in the EA captures (43,978) decode and re-encode byte for byte in the reference decoder.
That gives a complete reference to compare our C++ against.

## 2. M1: the capture harness

### 2.1 The test

A console program `capharness.exe`, built with VC6 and linked against the tree's `app` library and
wwlib. It reads a capture and runs two layers:

1. **Transport (wwudp).** EA-UDP datagrams go into `CordCon`'s receive path through a replay
   socket instead of a real one. The Alex dumps are already decrypted, but each session's RC4 key
   is recorded, so the harness encrypts them again with that key and gets back the bytes that
   were on the wire. That way our `RC4Class` and the checksum are tested too.
   Output: the messages that `CordCon` delivers, in order, with fragments joined and resends
   dropped.
2. **AuxData.** Every AUX_DATA message (opcode 0x1B, and the player-variable form) goes through
   the client's path: codec select 0x4A3FA0, then the tree-form or record-form decoder, into the
   right root object. It is then written back out.
   Output: the re-encoded bytes, and a dump of the tree's values.

**M1 is met when all of these pass:**

* **(a) Transport.** The message sequence `CordCon` delivers from the Alex sessions equals the reference decoder's
  `the reference importer` sequence (26,078 and 2,779 messages). The 180 blocks the reference decoder can't read (ack lists)
  are now read as ack lists. For each client-to-server datagram in the capture, the harness
  checks that our `CordCon` would accept its ack list.
* **(b) AuxData round trip.** All 43,978 EA AUX_DATA messages decode without an assert, and
  re-encode byte for byte.
* **(c) AuxData values.** The value dump equals the reference decoder's decode, field by field, for every message.
* **(d) Net-7 recordings.** Their AuxData messages give the same results, apart from the three
  known Net-7 differences, which the harness reports and counts instead of treating as failures.

The comparison script is Python (`tools/harness/compare.py`) and reads the reference decoder's JSON output. The
C++ harness itself stays small.

### 2.2 Work items

| Item | Content | Done when | Size |
|---|---|---|---|
| **M1.0** Inputs | **Mostly done already** in the reference decoder: the message-level inputs are the `ea\*.trace.jsonl` imports above, and the checks exist (`the reference decoder roundtrip`; `aux-census`: all 43,978 decode, re-encode and match the typed model; `aux-log --first`: every field value and change, the reference for (c)). Left: (1) a datagram-level file for gate (a): `tools/harness/convert.py` reads the Alex dumps (`session: <id> rc4 key: <8 bytes>`, then `C->S`/`S->C seq <n> len <hex>` and the decrypted datagram, checksum first), the same parse as `EnB.Trace/Ea/the reference importer.cs`, which keeps only the messages; (2) save `aux-census` and `aux-log --first` output per capture under `.work/harness/ref/`; (3) the harness skeleton reads trace.jsonl and the datagram file and counts messages | counts equal the reference decoder's. **Done 2026-10-24** (tools/harness/README.md): the ack list is decoded (type bit 0: count in byte 5, one header byte, 4-byte entries), so blocks fill every datagram; the .dgram files hold all 26,078 / 2,779 the reference decoder messages in order plus 28 / 12 that the reference importer's start-offset guess lost (4 of them AUX_DATA); capharness's counts equal the reference decoder's `dump --summary` for all 8 traces | small |
| **M1.1** Decode path | Map the receive path from `AuxDataPacketClass::Process_Packet` 0x7364C0 to the leaves, and list every function on it that isn't byte-exact yet (encode.cpp's codecs 0x4A9400 / 0x4A8FD0 / 0x4A9710, the rest of the leaf types: item arrays, interpolators, records, policy where the decoders call it) | a list in this file, each function with its status. **Done 2026-10-24: §2.3** | small |
| **M1.2** auxdata completion | The functions from M1.1, byte-exact where possible and functional where not (P1). This includes the remaining items in progress.md §4 step 2: the time field, the encoder `Add` functions, kv_pair_string, kv_list, the encode.cpp and policy.cpp rules | harness (b) passes on a hand-built root. **Decode path 2026-10-24** (§2.3): encode.cpp's codecs and lookup.cpp's build exact apart from 0x4A9400 (functional), app 136 -> 145 pairs (ambiguous 16 -> 19); 0x4A3FA0 and 0x4B0860 functional in stubs/ (objects unattributed); the leaf Reads are the template's. Encoder side and key/value path: encode.h, the record table's Add and the records, the record-form writer, kv_pair_string.cpp, the KV pair template, the time field (delta_time.cpp), a functional kv_list.cpp: app 145 -> 164 (ambiguous 19 -> 24); the app stubs are down to the debug hooks, the globals, MoneyClass's conversions and strconv_lib.h's (vidtest links, 14 symbols stubbed). **Hand-built root passes** (`tools/harness/auxdata_smoke.py`: both forms decode and re-encode byte for byte). Open: the packet entry 0x7364C0 (client classes), the root class's object 0x4A3990-0x4A4780 (root encode 0x4A3D50), the update collector 0x4A8990-0x4A8FC0 and its change table 0x4AB660-0x4ABCD0 (what the encoder decides to send) | large |
| **M1.3** Root schemas | The 13 root classes' constructors. A generator turns the extracted schema into C++ in the client's form (one `Add` per node, in order), and verify checks the result against the client (P2) | (b) and (c) pass on the EA captures. **Gates met 2026-10-24** (tools/harness/README.md): the client has 11 roots (the factory 0x6F6000's ten and Player 0x752510). `tools/re/auxroots.py` (symbolic execution, `auxextract.py`, a copy of the reference decoder's extractor that also records vtables, positions and defaults; its trees equal client-auxdata.json) generates `app/client/aux_roots.h/.cpp` (PROVISIONAL name): 53 classes, one per compound vtable, with the client's `delta_array.h` / `delta_item_array.h` templates; Component, Decoration, Light and Ship's narrow Add exact, the rest functional (NOT EXACT, D42). The root object's rebuilt part in `app/shared/auxdata/unknown_root.cpp` (PROVISIONAL): the clock, UnknownRootClass 0x4A3BD0/0x4A3D30, Decode 0x4A3FA0, Visit_Children 0x4B0860 (out of stubs/). app 164 -> 202 pairs (ambiguous 24 -> 31), none lost; class names from Net-7 AuxClasses / the reference game model matched by structure (D56 tags, tools/re/auxnames.py). Harness `tests/capharness/auxreplay` + `tools/harness/auxreplay.py`: (b) all 43,978 EA AUX_DATA decode without an assert, 43,977 re-encode byte for byte (the one that doesn't: capture_1 #24128, a Hulk header byte 0x55 the client's encoder can't write); the Alex dumps' 4,289 / 366 (D52) all pass; (c) every field change equals the reference decoder's `aux-log --first` (681,376 + 147,771 + 337,019 + 37,601 + 34,626). Open: the root encode 0x4A3D50 and the second root class 0xAEAC40 (Player's base, with the collector), exactness of Ship / Player / ManufacturingLab / Harvestable / the factory / the nested classes | medium |
| **M1.4** wwudp | `CordMgr`, `CordCon`, bandwidth accounting: handshake (SYN1/ACK1/SYN2/ACK2), checksum, block types, fragments, sequence and resend handling, ack lists, close. Socket and timer behind the class's own interface, replaced by a replay socket in the harness | harness (a) passes | large |
| **M1.5** Net-7 recordings | Message-level replay of the TCP recordings, plus (d). Whether the client's TCP transport is also in `app\wwudp` is checked here (P6) | (d) passes | small |

The order is M1.0, then M1.1, then M1.2 and M1.3 (they can overlap), then M1.4, then M1.5.
This follows D6 (auxdata, then wwudp). Items M1.4 and M1.5 also exercise WWLib's `RC4Class`
and `PKey` at run time for the first time.

### 2.3 The decode path (M1.1)

Mapped 2026-10-24 from `AuxDataPacketClass::Process_Packet` 0x7364C0 to the leaves (scripts and
the full call graph: `.work/milestones/m1/m11/`). **Exact** = the address is in
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
| tree | 0x4A9400 | `TreeCodecClass::Visit(DeltaCompoundClass *)` (assert line 401) | 622 | encode.cpp | functional, NOT EXACT (same instructions; VC6 duplicates the early epilogues) | 0xAEB2A0 slot 1 |
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

## 3. The link step (shared)

| Item | Content | Done when |
|---|---|---|
| **L1** Libraries | `tools/vc6/build.py`: compile each library completely, every file and not only the ones that match, with its verify flags, then `LIB` it into `wwlib.lib`, `ww3d2.lib`, and so on, plus `app.lib`. Runs on a pool VM (`--pool`); the output goes to `build/` (git-ignored) | eight `.lib` files. **Done 2026-10-24: §3.1** |
| **L2** First executable | Link a minimal WinMain against them with LIBCMT, `d3dx8.lib`, winmm, ws2_32 and the rest. Fix the unresolved externals: NOT_LINKED files, declared-only functions (D28-type declarations), E&B hooks the libraries call back into. Each fix is either a real function or a stub under `stubs/` that is listed and logs when it is called (P3) | a linked `.exe` that starts, `WW3D::Init` succeeds, and the window clears to a colour. **Done 2026-10-24: §3.2** |

L2's stub list doubles as a to-do list: each stub marks a function the client has and we haven't
written yet.

### 3.1 L1 results (2026-10-24)

**Done.** `python tools/vc6/build.py --pool 1` (about 50 s on one VM) compiles every committed
`.cpp` of the eight libraries with verify's flags, defines, include paths and, for `app`, the
`e:\build\beta\G` `__FILE__` alias (all imported from importer.py), runs `LIB`, and writes
`build/<lib>/` (git-ignored): the `.lib`, `obj/`, `cl.log`, `symbols.txt`, `members.txt`,
`summary.json`. It refuses to build a library with uncommitted changes (`--allow-dirty`
overrides). `build.py unresolved` then lists the unresolved externals into
`.work/milestones/l1/unresolved.txt`, with a heuristic cause per symbol.

Every file compiles: the tree holds exactly the files verify compiles (assemble only copied files
with evidence), so "every file" and "verify's files" are the same set today.

| Library | Files compiled | Failed | Warnings | .lib bytes | Members |
|---|---|---|---|---|---|
| wwdebug | 3 | 0 | 0 | 95,800 | 3 |
| wwlib | 66 | 0 | 0 | 732,476 | 66 |
| wwmath | 35 | 0 | 0 | 602,930 | 35 |
| wwsaveload | 11 | 0 | 0 | 289,302 | 11 |
| wwaudio | 17 | 0 | 0 | 655,642 | 17 |
| ww3d2 | 109 | 0 | 7 (6 × C4530 in w3d_dep.cpp's iostream use without /GX, 1 × C4101) | 4,265,406 | 109 |
| wwshade | 20 | 0 | 0 | 467,188 | 20 |
| app | 14 | 0 | 3 (C4231, `extern template` in delta_template.h) | 170,324 | 14 |

**Unresolved externals: 125 symbols** (referenced, defined by none of the eight libraries nor by
LIBCMT, LIBCPMT, OLDNAMES, kernel32, user32, gdi32, advapi32, ole32, oleaut32, uuid, winmm,
ws2_32, d3d8, d3dx8, dxguid). By cause:

* **Miles: 66** (wwaudio: 65 `AIL_*` plus `MSS_auto_cleanup`). Needs `mss32.lib` (not in the SDK
  directory) or stubs.
* **More system libraries: 12.** VFW32 (8 `AVIFile*`/`AVIStream*`, ww3d2 framgrab.cpp), COMSUPP
  (3 `_com_*`, ww3d2 dx8webbrowser.cpp), ddraw.lib (`DirectDrawCreate`, wwlib ddraw.cpp).
* **EA files never imported (no byte evidence): 27 symbols from 12 files.** wwlib `TARGA.CPP` (9,
  for ww3d2's texture loader and screenshots, `Targa::Open(const char*, long)` included),
  `int.cpp` (4, `Int<64>` statics for pk.cpp), `blwstraw.cpp` (2), `blowfish.cpp`, `stimer.cpp`,
  `slnode.cpp` (`DEFINE_AUTO_POOL(GenericSLNode)`, for wwlib, wwsaveload, ww3d2), `win.cpp`
  (`MainWindow`, `ProgramInstance`: likely set by E&B's WinMain); wwsaveload
  `definitionfactory.cpp` (2, also used by wwaudio); wwaudio `soundstreamhandle.cpp`, `Utils.cpp`
  (`MMSLockClass` critical section); wwshade `shddeffactory.cpp` (2), `shdforcelinks.cpp`
  (`SHD_Force_Links`, which the client has at 0xA08C70). Each is either imported as is (it has no
  pairs to lose) or checked against the client first.
* **Declared only: 1 in the libraries.** `ListenerHandleClass::Initialize(SoundBufferClass *)`
  (wwaudio listenerhandle.h, no definition in any EA copy).
* **app: 19.** E&B debug hooks from `unknown_debug.h` (`Unknown_Assert_Failed`,
  `Unknown_Fail_If_Report`, `Unknown_Debug_Printf`, `Unknown_Get_Ticks`) and the globals
  `DeltaUnknownGlobal`, `DeltaCompoundUnknownGlobal`, `DeltaStringClass::UnknownGlobal`; the
  missing auxdata functions of §2.3 (`LookupClass::LookupClass`, `RecordTableClass::Add`,
  `KVListClass::Add`, `KVPairStringClass` ctor, `KVPairTemplateClass<float>`'s dtor and six
  virtuals, `DeltaTemplateClass<DurationClass>` ctor).

Nothing in the list points at a failed compile, a NOT_LINKED file or a header/body mismatch. The
list is L2's starting to-do list. Caveat: a library-only link resolves only what the objects
reference; L2's `WinMain` will add references (WW3D::Init's callees are already in), and a
`.lib` pulls members only on demand, so some of these may never be needed by a minimal program.

### 3.2 L2 results (2026-10-24)

**Done.** `build/programs/vidtest/vidtest.exe` (913,408 bytes) starts on the host, `WW3D::Init`
and `WW3D::Set_Render_Device` return `WW3D_ERROR_OK` on the RTX 3060 Ti (640x480, windowed, 32
and 16 bpp), every frame clears to dark blue (0, 0, 0.5), and it exits with code 0 both after
`-frames N` and when its window is closed. No stub was called and none was linked.

**How it is built and run** (after `build.py` and `mkmss32.py lib`, on a leased VM):

```
python tools/vc6/build.py link --program vidtest          # -> build/programs/vidtest/
python tools/harness/run_vidtest.py --frames 120          # or --mode close, or -- -depth 16
```

The link line `build.py link` writes (`link.rsp`, in the VM):

```
link /nologo /subsystem:windows /machine:ix86 /incremental:no /nodefaultlib /out:vidtest.exe
     /map:vidtest.map obj\*.obj app.lib wwshade.lib ww3d2.lib wwaudio.lib wwsaveload.lib
     wwmath.lib wwlib.lib wwdebug.lib mss32.lib stubs.lib LIBCMT.LIB LIBCPMT.LIB OLDNAMES.LIB
     KERNEL32.LIB USER32.LIB GDI32.LIB ADVAPI32.LIB OLE32.LIB OLEAUT32.LIB UUID.LIB WINMM.LIB
     WS2_32.LIB VFW32.LIB COMSUPP.LIB d3d8.lib d3dx8.lib dxguid.lib ddraw.lib
```

`programs/vidtest/vidtest.cpp` is compiled with WW3D2's flags (`/MT /W3 /O2 /Ob2 /D
ENABLE_WWPROFILE /D USE_WWSHADE`, vc6's defines), `stubs/*.cpp` the same plus `/GX`, into
`stubs.lib`, so a stub is linked only when something references it. The map shows what the
program pulled: 82 ww3d2 objects, 33 wwlib, 22 wwmath, 22 wwshade, 10 wwsaveload, 3 wwdebug,
62 from d3dx8; none from wwaudio or app (no Miles import: `AnimatedSoundMgrClass::Initialize`
doesn't reach it). d3d8.dll is loaded at run time by `DX8Wrapper::Init`, as in Renegade.

**What it took.**

* **D49: the 12 EA files** (§3.1), imported by `importer.py add` (`EXTRA_FILES`; plan keeps them),
  each from its library's base copy: wwlib's seven and wwaudio's two from Renegade, wwsaveload's
  and wwshade's from Zero Hour. `TARGA.CPP` is Renegade's on evidence: the client's `YFlip`
  (0x9D33D0) is the byte loop, Zero Hour's is an asm `_swapBytes`. `soundstreamhandle.cpp` needs
  `AIL_open_stream_by_sample`, which `sdk_patch.py` now declares (toolchain.md Miles fix 2).
  verify: **+33 pairs, none lost**: TARGA.CPP 21 (wwlib 565 -> 586), soundstreamhandle.cpp 12
  (wwaudio 351 -> 360).
* **wwshade's `SHD_Force_Links`** called `_Force_Link_LegacyW3DShader` (shdlegacyw3d.cpp is
  NOT_LINKED); the client's (0xA08C70) links five shaders, so that line is patched out.
* **D51: `mss32.lib`** from the game's `Mss32.dll` (344 exports): `tools/vc6/mkmss32.py def` writes
  `tools/vc6/mss32.def`, `mkmss32.py lib` builds `build/mss32/mss32.lib`. VC6's `lib /def` can't
  (it imports every .def name with the name type "no prefix": `AIL_shutdown@0` would import
  "AIL_shutdown@0", the DLL exports "_AIL_shutdown@0"), so the script links a stub DLL with one
  empty `dllexport` per name and keeps LINK's import library, plus `MSS_auto_cleanup`. A probe
  built against it ran `AIL_startup`/`AIL_shutdown` in the game's DLL.
* **D50:** VFW32, COMSUPP and ddraw on the link line.
* **WinMain needs the asset manager first.** Without `new WW3DAssetManager` before `WW3D::Init`
  (VIDTEST's step 4), `Set_Render_Device` crashed in `ShatterSystem::Init` (it asks the asset
  manager for htrees during device creation). The crash report vidtest writes (`CRASH:` lines,
  named by `tools/harness/symbolize.py`) found it.

**Unresolved externals, library level** (`build.py unresolved`, every object of the eight
libraries, `.work/milestones/l2/unresolved.txt`): **125 -> 20, all 20 defined by `stubs/`.** A
link of every library object (not `.lib` members on demand) with all stubs resolves too, apart from
one duplicate: `Find_POT`/`Find_POT_Log2` are in both `ww3d2/pot.cpp` and `wwmath/pot.cpp`
(proposal below).

**Stubs** (`stubs/`, D44; each logs its first call to OutputDebugString, stderr and
`ENB_STUB_LOG`/`stubs.log`; asserts log every call):

| File | Stub | Client | Linked into vidtest | Called |
|---|---|---|---|---|
| app_stubs.cpp | `Unknown_Assert_Failed`, `Unknown_Fail_If_Report` (log each call: expr, file, line) | 0x4E9410, 0x4E9450 | no | no |
| app_stubs.cpp | `Unknown_Debug_Printf` (logs the text), `Unknown_Get_Ticks` (returns `GetTickCount()`) | ? | no | no |
| app_stubs.cpp | `LookupClass::LookupClass` (members default-constructed), `KVListClass::Add`, `RecordTableClass::Add`, `KVPairStringClass::KVPairStringClass` | 0x4AC250, 0x4B6760, 0x4A9C30, 0x4B7FC0 | no | no |
| app_stubs.cpp | `KVPairTemplateClass<float>`: destructor, `Unknown_04`..`Unknown_18` (7) | vtable 0xAE7860 | no | no |
| app_stubs.cpp | `KVPairStringClass`: destructor, `Unknown_04`..`Unknown_18` (7; the stub constructor's vtable needs them) | 0x4B80E0, 0x4B8250.. | no | no |
| app_stubs.cpp | `DeltaTemplateClass<DurationClass>::Unknown_00` | ? | no | no |
| app_stubs.cpp | data, can't log: `DeltaUnknownGlobal`, `DeltaCompoundUnknownGlobal`, `DeltaStringClass::UnknownGlobal` (0xBE3A9C), `DeltaTemplateClass<DurationClass>::UnknownGlobal` (0xBE3AA4), all 0 | | no | |
| app_stubs.cpp | not a stub: `DeltaTemplateClass<DurationClass>`'s constructor, the header's body as an explicit specialisation (VC6 can't instantiate it after the header's `extern template`) | 0x4A8150 | no | |
| wwaudio_stubs.cpp | `ListenerHandleClass::Initialize` (declared in listenerhandle.h, defined in no EA copy; does nothing) | ? | no | no |

24 logging stubs, 4 data stubs, 1 real definition; none linked into or called by vidtest. They
are the app library's link to-do list for M1 (§2.3) and the first audio one.

## 4. M2: VIDTEST

### 4.1 The test

`vidtest.exe` (our build) does what VIDTEST.EXE does:

* Command line: `-device -resx -resy -depth -windowed -sync -filter`. E&BConfig.exe launches it
  with `VIDTEST.EXE -device "%s" -resx %d -resy %d -depth %d -windowed %s -filter %d`.
* It loads the W3D scenes: the station and backdrop scenes `mu01a`, `back01`, `ths01`, `jvc01`,
  `bst29a`, `pvs01`..`pvs04`, and the player ships `p?v?01a`. Textures come from DDS and TGA files,
  text from `font.tga`.
* It runs the Fill Rate, Polygon Budget and Land LOD tests, and writes `results.ini`
  (`FillRateFPS`, `PeakPolyRate`, `FreeTextureRam`, `PolyBudget`, `LandPolyBudget`, `LandLOD`,
  `StarBudget`, `TextureReduction`).
* It uses E&B's own scene classes: terrain from `land.ini` (`RIVER4.PLD`, height, colour and
  detail maps), stars from `stars.ini`, decorations from `decoration.ini`, and splines from
  `splines.wsd`.

**The reference is the original program, run on the same machine.** VIDTEST.EXE runs on the host
today. With the same command line and art, its frames are the reference images for ours. Pixel
equality isn't expected, since timing and filtering differ, but the same models, textures, lighting
and layout must be visible.

**M2 is met when all of these pass:**

* **(a) Rendering.** Each test scene renders on the host's GPU, and a screenshot of it shows the same
  models, textures and lighting as the original's screenshot of the same scene.
* **(b) Tests.** All three tests run to the end, and `results.ini` gets values in the same range
  as the original's on the same machine.
* **(c) End to end.** E&BConfig.exe's "Performance Test" button, in a copy of the `EBCONFIG`
  folder, runs our vidtest.exe instead of the original and reads its `results.ini`.
* **(d) Errors.** No asserts during the run. Every stub that is called is listed and justified.

### 4.2 Work items

| Item | Content | Done when | Size |
|---|---|---|---|
| **M2.0** Spec | Open VIDTEST.EXE in REA and write down its program flow: WinMain, device setup, the scene list and camera paths, each test's loop and measurement, and `results.ini`. Map its 437 app functions to client functions (252 already match) and to WW3D2 calls. Output: `docs/vidtest.md` | the spec names every scene, test and output key | medium |
| **M2.1** One model | After L2: an asset manager on `EBCONFIG`, a camera, a light and one ship (`p?v?01a`) with its DDS textures, rendered in a loop. A screenshot is taken through `WW3D::Make_Screen_Shot` (progress.md §4 step 3) or a plain `D3DX` save | screenshot compared with the original's | medium |
| **M2.2** Scenes and text | All W3D scenes, backdrops, `font.tga` text overlay, the command line, windowed and fullscreen modes | (a) for the W3D scenes | medium |
| **M2.3** Tests | Fill Rate and Polygon Budget tests, `results.ini` | (b) for these two, (c) | small |
| **M2.4** E&B scene classes | Land (terrain, lightmap, LOD), stars, decorations, splines, and the Land LOD test (P5) | (a) and (b) complete, then (d) | large |

M2.0 can start straight away, alongside M1, because it only reads VIDTEST.EXE. M2.1 needs L1 and
L2 first.

## 5. Proposals (decisions for the user)

| # | Proposal | Recommendation |
|---|---|---|
| P1 | **Functional code in tree files.** A function on the harness or VIDTEST path that isn't byte-exact yet may be written functionally from pseudocode, with a `// NOT EXACT:` comment above it. Verify counts don't change, because it only counts exact functions, and the comment marks the function for later matching | Yes. Without it, M1 waits until every function on the decode path is exact |
| P2 | **Generated root schemas.** The 13 root constructors are generated from the extracted schema in the client's code form, then verified like any other app file. Rows that don't match stay generated, marked as in P1 | Yes. The constructors are long straight-line `Add` sequences: a generator is quicker and less error-prone than writing them by hand |
| P3 | **Stubs.** Missing functions are stubbed in a separate `stubs/` directory, never in the library files. Every stub logs its first call | Yes |
| P4 | **Where the new programs go.** `tests/capharness/` for the harness, `programs/vidtest/` for VIDTEST, `tools/harness/` for the Python side. None of these are under `app/`, which mirrors the client's `G\app` | Yes. Accepted with a refinement (D45): VIDTEST.EXE keeps no source paths, but its scene classes share the client's strings (`\\.\mailslot\G\SplineDatabaseUpdate`, `stars.ini`, `decoration.ini`, `HEIGHT_MAP`, `Land::Update_Lightmap_ManualThreaded`), so they go where the client has them (`app/shared/stars.cpp`, `spline.cpp`, ...). Only VIDTEST's own code (`results.ini`, `land.ini`, the tests) goes in `programs/vidtest/` |
| P5 | **VIDTEST's E&B classes.** Land, stars, decorations and splines are written from the client's 2004 versions where the client has them (`Land::Update_Lightmap_ManualThreaded` is at 0x46DA70 in both), and from VIDTEST's 2002 code only where it doesn't. That keeps the work useful for the client | Yes, and treat them as client reconstruction work, byte-exact where the client has them |
| P6 | **The Net-7 TCP recordings.** M1's transport gate (a) uses the EA captures only. The Net-7 recordings are tested at the message level (d). Their TCP transport is tested only if it turns out to be in `app\wwudp` | Yes |
| P7 | **Building and running.** Build on a pool VM. Run the harness on the host or a VM. Run vidtest on the **host**: the Hyper-V VMs have no 3D hardware, and D3D8 on a basic display adapter is untested. Running our own built programs on the host unattended falls under the night rule "starting E&B's own code unattended", now extended to the tree's own programs | Yes |

## 6. Order and pacing

Work goes in the usual way: one builder at a time with time limits, and every byte-exact gain
verified and committed as before. The milestone checks run after each item.

1. M1.0, M1.1 (small; they set the M1 to-do list), with M2.0 in a parallel read-only agent.
2. L1, L2 (the link step; this unblocks M2 and lets the harness be built).
3. M1.2, M1.3, then M1.4 (the main work), with M2.1 to M2.3 alternating when a builder is waiting
   on the VM pool.
4. M1.5, then M2.4.

**Risks**

* **L2 may produce many unresolved externals**, for example from WW3D2's calls into E&B code. The
  stub rule (P3) keeps that from blocking progress.
* **Code that has never run may crash.** The byte-exact functions are the client's own code, so
  they are safe. Patched-but-unmatched functions and guessed E&B layouts are the risk. Each such
  crash points at a real bug in the reconstruction, which is useful in itself.
* **wwudp is mostly unattributed.** Its size is unknown until M1.4 starts; the 26 attributed
  functions are a floor, not an estimate.
* **"A day or so" is ambitious.** M1.0, M1.1, L1 and M2.0 fit in a day. The rest depends on L2 and
  on how big wwudp turns out to be. Progress against the milestone checks is logged in
  progress.md as usual.
