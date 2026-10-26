# Progress, decisions and open questions

Newest first within each section. Dates are when the entry was written.

## 1. Log

### 2026-10-25 (evening): D63's supervised E&BConfig run passed; M2 complete

The one-time acceptance check of M2 gate (c): the user ran the original `E&BConfig.exe` (build 8168,
2004) unelevated from the EBCONFIG copy at `build/programs/vidtest/e2e_ebconfig/`, with our vidtest
(build 2026-10-24 22:43) in place as `VIDTEST.EXE`, and pressed Performance Test. Our vidtest ran the
three tests to the end at the configured 1280x1024x32 windowed and exited 1; E&BConfig read its
`results.ini` and displayed Fill Rate FPS 3529, Polygon Budget 30000, Star Budget 2000, Land Polygon
Budget 28000, Land LOD 7, Texture Reduction 0, Peak Poly Rate 57655 KPolys/s, every value as written
(`vidtest.log` line 358). "Free Texture + AGP RAM = -10.486 MB" is E&BConfig's own `atoi` wrapping our
`FreeTextureRam=4284481536` (a 4 GB card) to -10485760 and dividing by 10^6: the 2004 tool's limit, not
a defect of ours (the original VIDTEST reports the same figure on this card; `vidtest_e2e.py` emulates
the wrap). The user cancelled at "Press OK to save settings". Registry: the WOW6432Node, VirtualStore
and 64-bit `Westwood Studios` keys were exported to `.work/regbackup/` before the run and are
byte-identical after it. No message box appeared. M2 is complete on all four gates.

**D53 seen in the game.** The user's client had `TextureReduction=2` in both Render keys (the real
WOW6432Node key and its VirtualStore copy), written by the original VIDTEST's signed comparison on a 4 GB
card: every texture loaded at a quarter of its size. With the value set to 0 (the user, by `reg add`,
backups in `.work/regbackup/`) and the original client relaunched, screenshots of the same views
(`.work/texdetail/`, local) show the planet in Endriago at about twice the fine detail (edge energy
2.58 -> 5.01 on the cropped planet), the ship hull and the login-screen avatar modestly crisper (1.09x,
1.11x: their textures are small to begin with). So the original tool's bug cost players most of the
planet and nebula detail on any card over 2 GB; our vidtest's fix (D53) is the setting to ship.

### 2026-10-25 (later): the C# proxy audited, renamed the reference decoder (the reference decoder), its test-harness repo retired; EnB paths follow

The D79 question was settled as an audit of `the reference decoder` plus the first housekeeping there (that
repository's `docs/progress.md` has the detail). Four read-only audits (architecture and code quality,
docs vs code vs EnB's findings, harness-merge completeness, rename inventory; Opus 214k + 259k, Sonnet
126k + 99k) then: its 2026-10-22 work committed as found (d41ff27, 277 tests), `the reference traces`
retired (its 12 commits were already subtree-merged; 22 recordings hash-checked into the proxy's
`captures/`, 217 MB of replay runs discarded; c4a48c3), the rename to **the reference decoder** (folder
`C:\EnBDevelopment\the reference decoder`, `the reference decoder.slnx`, `the reference decoder.exe`, `the reference decoder.Proxy.*`, `antlink`, pipe
`the reference decoder.Link.v1`; f84a591, d16bf94, ccf8567; `the reference decoder`, `Net7.Protocol` and every citation of the
original 2.46 unchanged), its design.md split into seven docs with EnB's corrections applied (bbfc74e,
16c9dc6, a792172), and a private tool's coordinated change (edc103e its own 2026-10-22 work, 1036d23
the new exe, pipe, log names and the old launcher id read as an alias).

* **EnB's findings that superseded the proxy's docs** (now cited there): the EA-UDP datagram layout
  (`app/wwudp/cordcon.h`: CRC-32 at 0x8B3EC0, flags byte, ack count, per-datagram RC4 key, the ack list
  the proxy's `the reference importer` never decoded), the packet factory's 263 opcodes above 0x0FFF, the client's
  class names for 0x21/0xB7/0xB8 and the 0x9E/0xA0 split, LoginTaskClass's vtable at 0xB040D4 (its
  login offsets were 8 too high), 11 AuxData roots not 13, difference 4 in library-matching.md §3.8.
* **What that project knows and EnB's docs don't** (for later): the login and character-select screens
  (`docs/research/client-ui.md` there), the action message layouts, authlogin.dll's flags and offsets,
  the MVAS addresses, the 2.46 opcode census, byte-order fixes, AuxData behaviour per ship kind.
* **EnB fixes from its audit:** prior-work.md no longer claims an EA-UDP transport exists in the proxy
  (only the `the reference importer` importer does); tools/harness/README.md's EA-UDP table says flags (not type) and
  names the ack entry's first byte as cordcon.h does (kind 0/1/2), and cites auxdata.md §7 items 1-3
  (not §7.1-§7.3); toolchain.md §3, `roots.py` (`REF_TRACES_ROOT` is now the proxy's `captures/`),
  `pkfactory.py` and `refs.sh` point at `the reference decoder`.
* The proxy's re-architecture (server profiles, client-side TCP and EA-UDP, patch manifests) is a
  proposal with decisions A1-A12 pending in that repository (`.work/audit/architecture-proposal.md`);
  its EA-UDP transport will be built from `app/wwudp`. Not EnB work.

### 2026-10-25: housekeeping audit; D62-D79; builders.md; traps 100 -> 74; milestones split; tools promoted

Four read-only audits (`.work/audit/traps.md`, `decisions.md`, `tools.md`, `startup.md`; two on Opus,
two on Sonnet) then three edit agents on disjoint files. Found: D32 (serial builders) contradicted the
approved parallel runs; D23 (NDEBUG off) contradicted D61; D36 vs the E&BConfig check; D7 vs `NOT EXACT`
code beyond D42's scope; D39 as written didn't compile; D10/D16 vs D56; the builder brief, the pairs
comparer and several standing safety rules existed only in the git-ignored `.work/`; a builder read
about 105 KB (27k tokens) at startup, 82% of it milestones.md and workflow.md §3.

* **Decisions** D62-D79 (6477bb8), including yesterday's proposals: D62 parallel builders (replaces D32),
  D63 the E&BConfig check is one-time, D64 `NOT EXACT` general with a tracker, D66 pair-loss classes
  (0x8C1520 is class iii), D69 safety rules numbered, D71 attribution from the harness, D72 subagent
  models, D73 imports (wlib/wnet, tcpmgr/tcpcon, BinkMovie; CordArrayClass's remove on wlib), D74-D78
  tool and define items, D79 the the reference decoder note. D10, D16, D23, D32, D36, D39, D43 reworded.
* **[builders.md](builders.md)** (75920e0): the tracked builder brief: startup set per kind, rules, the
  model table, report format. CLAUDE.md names the pool, `pairs_cmp.py`, D27, D69, D71.
* **workflow.md §3** (a0121ec, b619ec3): 100 traps / 37 KB -> 74 / 18 KB in §3.0 Everyone plus nine
  topic buckets; 24 duplicate groups merged; §2.1 fixed for D41/D61; tool quirks moved to the three
  READMEs (0386ac4). kernel-map.md: the four "WM_QUIT sets +0x0C" claims corrected, header current.
* **milestones.md** 51 -> 20 KB (c826e08): one heading per item; results in library-matching.md §3.8,
  the decode-path table in [decode-path.md](decode-path.md); §4 here collapsed (76e8472); the overnight
  reports archived under `docs/archive/` (65956fb).
* **Tools** (79a79ec ... cba6d0d): `tools/vc6/pairs_cmp.py`, `tools/re/roots.py` (every external root;
  toolchain.md §3), the rerunnable night scripts promoted (zhall, dated, strscan, mark), `notexact.py`
  -> `docs/data/not-exact.md` (38 functions in 15 files), `tools/vc6/README.md`; `compare.py` retired
  (its staged deletion landed in 6477bb8). vidtest_e2e/exitcodes kept separate (little shared code).
* Startup set per builder now about 43 KB (app), 39 KB (library), 35 KB (harness).
* the reference decoder `docs/research/auxdata.md` §7 item 9 added in that repository's working tree (D79).
* Models and tokens: audits Opus 107k + 127k, Sonnet 96k + 78k; edits Opus 134k, Sonnet 168k + 108k.
* Open: the Net7 repositories question (D79) before housekeeping closes; the supervised E&BConfig
  acceptance check (D63).

### 2026-10-24 (evening, unsupervised): M1 and M2 met; transport, kernel, connections, packet factory; app 251 -> 458

Plan and log: `.work/milestones/plan-7h.md`. Two builders in parallel, coordinator heartbeat.
Both builders were cut off by the API usage limit 19:40-21:10 and resumed.

* **M1 met**, gates (a)-(d) ([milestones.md](milestones.md) §2): M1.4 rebuilt `app\wwudp` (CordCon,
  CordMgr, bandwidthaccounting.cpp; 29c3245 ... e0e82f8, addc2ad): the Alex datagrams re-encrypted
  with their keys go through our CordCon: alex3 26,106 messages in order, all 14,812 client ack lists
  accepted, the peer accepts all our datagrams, 54/54 handshakes (WWLib's XMP at run time); the
  `loop` test over a lossy link delivers everything. M1.5 (26e8ed7, bcdb31d): every Net-7 AUX_DATA
  decodes and equals the reference decoder's aux-log, Net-7's known differences counted, a fourth one found.
  The client's TCP transport is app\common\tcpconn.cpp on WWOnline's TCPMgr/TCPCon (D47 finding).
* **M2 met** apart from the supervised E&BConfig run: land matched from the original's draw calls
  (a d3d8 proxy dump: 3a37637, 2e216a9, c00bf3c), frames 500-900 within 1.7-3.2%; gate (d) clean in
  a checked build (78644e0). D61 NDEBUG (2fade9a).
* M1.3 leftovers (f2f2a08 ... c6867b3): the update collector, change table, root encode, second root
  class, policy.cpp; client-path re-encode of the EA captures 39,716 / 43,978 byte for byte (the rest
  differ only in server-policy fields). One accepted loss, 0x8C1520 (a false match while MoneyClass
  was a bare __int64; library-matching.md row): for the user to confirm. Root constructors (d64612e
  ... a67e4b6): Player 0x752510 and the factory 0x6F6000 exact (VC6's inline budget model, workflow.md).
* Kernel ([kernel-map.md](kernel-map.md), 8ab13d1): kernel.cpp (cba72d7, 18 exact) with kerneltest;
  the connection layer circbuff/connect/udpconn (3742a73, 9026165, ff8123e); MenuTask and the movie
  tasks (faecc74; movietest plays the game's Westwood logo through BinkMovie); pk.cpp's helpers, the
  factory 0x8696C0 and its 16 KB switch 0x869790 byte for byte with 263 generated packet classes
  (38f055c, c105a36, e6fea6e, `tools/re/pkfactory.py`, `pkclasses.py`). udpreplay `conn` runs the Alex
  messages through UDPConnection, the stream framing and the client's factory.
* Names: pre-D56 names tagged (f727a18); the review list is `docs/data/names-review.md`.
* Proposals waiting for the user: import WWOnline wlib/wnet (Zero Hour matchbot) plus Renegade's
  RenegadeGR tcpmgr/tcpcon; import Renegade's BinkMovie; ENABLE_WWPROFILE as an app define; the logger
  stand-in; count the 17 template leaf Reads as pairs; names for unknown_changes.* and UnknownCOClass;
  CordMgr::Connect returning the session id; CordArrayClass's remove with wlib; funcmatch's string
  evidence to check `__FILE__`; the draw dump as M2's regression check; Net-7 difference 4 into
  the reference decoder's auxdata.md.

### 2026-10-24 (afternoon): milestones M1 and M2 (D42-D61); app 136 -> 251, first linked programs

Plan: [milestones.md](milestones.md) (M1 capture harness, M2 working VIDTEST); VIDTEST spec:
[vidtest.md](vidtest.md). Coordinator plus builders, two in parallel from M2.1 on.

* M1.0 (5af559a): `tools/harness/convert.py` turns the Alex dumps into datagram files and decodes the
  EA-UDP ack list (type bit 0); it holds all 26,078 / 2,779 the reference decoder messages plus 28 / 12 the reference decoder lost
  (D52). the reference decoder references saved in `.work/harness/ref/`. `capharness.exe`, the first VC6 program.
* M1.1 (ff1db4f): the decode path mapped (milestones.md §2.3). M1.2 (7ca6a71 ... 795b23d): codecs,
  lookup build, encoder records, kv_pair_string, the time field: app 136 -> 164. D54's ambiguous
  column (a666c27). M1.3 (a35535c, 8303cf4): the 11 roots and 42 nested classes generated
  (`tools/re/auxroots.py`), names matched to Net-7's AuxClasses by structure (D59): app 164 -> 202;
  `auxreplay`: all 43,978 EA AUX_DATA decode, 43,977 re-encode byte for byte (D60), every field
  change equals the reference decoder's aux-log (gate (c)), Alex 4,289 / 366 pass.
* L1 (12d7347): `tools/vc6/build.py` builds all eight libraries. L2 (d7c38ab ... fbe117b): the 12
  never-imported EA files (D49, wwlib 565 -> 586, wwaudio 351 -> 360), mss32.lib from the game's DLL
  (D51, `mkmss32.py`), `stubs/` (D44), the first executable `programs/vidtest`. D55: ww3d2/pot.cpp
  NOT_LINKED (9d6635d, WW3D2 2477 -> 2475, D20 duplicates).
* M2.0 (fae8e7a): VIDTEST's spec. M2.1-M2.3 (25c0008 ... 066ace7): BST29A, every W3D scene, sky,
  overlay, text, the three tests, results.ini, exit codes, E&BConfig's launch (no registry), a d3d8.dll
  proxy capturing the original's frames (frame 300 within 3.2%). M2.4 (e47e935, 5c182eb): stars (8
  exact), splines, land, decorations: app 202 -> 251; Max Polys 30729 / PolyBudget 30000 as the
  original; land frames 35-40% different (orientation open). D61 NDEBUG (2fade9a, pairs unchanged).
* Names: D56 certainty tags and `tools/re/names.py` (b61f6af); pre-D56 names tagged (f727a18).
* Lessons (workflow.md §3): never a modal dialog or a screen grab in unattended runs (two
  message boxes reached the user); never run a program in the game's install folder; two builders
  waiting for each other's clean tree deadlocked for an hour (rule replaced).
* Open: M1.4 (wwudp, gate (a)) and M1.5; M2 gate (d), land orientation; the supervised E&BConfig
  run with the user once all features are done.

### 2026-10-24: the kv encoder, the interpolator's Unknown_00, encode.cpp and policy.cpp starts; app 92 -> 136

* `app/shared/kv_encoder_base.cpp`/`.h` (22cfb49, with the interpolator below: 33): `KVEncoderBaseClass`
  (vtable 0xAEB62C, the destructor only; `Data` +4, the pair list +8) and its Data (0x3C bytes:
  `PrefixList` list<string> +0, `FlagList` list<int> +0xC, `Flags` +0x18, the joined prefix +0x1C, the
  last key +0x2C). Exact: dtor, Make_Key, Get/Set/Push/Pop_Flags, Push/Pop/Set_Prefix, the static join
  0x4B43E0 (called directly, no thunk; inlined in Push_Prefix and Set_Prefix), Unknown_Test 0x4B4820
  (the even 0x155 and odd 0x2AA bits of `Flags & mask`), Add_String 0x4B48A0, plus the STL instances
  they use. The constructor 0x4B3850 differs in register allocation only (`this` in ebx).
* `kv_pair.cpp`/`.h` (`KVPairClass`, vtable 0xAEB7EC: dtor + 6 pure; the key a `PermString`, two
  flags), `kv_pair_string.h` (`StringValueClass` moved here from delta_string.h, `KVPairStringClass`),
  `kv_list.h`; the typed accessor/pair templates `KVAccessorClass<T>`/`KVPairTemplateClass<T>`
  (float: 0xAE78D0/0xAE7860) and the member template `Add_Value<T>` in the encoder header.
  `DeltaChildVisitorClass` now derives from `KVEncoderBaseClass` (slot 0 is the base's destructor).
* `delta_interpolator.cpp`: Unknown_00 0x4A6930 (EndTime/ChangePerTick through their vtables via
  `((DeltaClass &)EndTime).Unknown_00`, then "CurrentValue") and `DeltaInterpolatorValueClass`
  (0xAEAF08: Get 0x4A6B30 exact, Clone 0x4A6AF0 exact but a twin). `delta_template.h` (e0952d6):
  the generic Write/Read (lines 91/110) and `Unknown_00` with `DeltaTemplateAccessorClass<T>`
  (Set 0x4A6F00); the float Read/Write/Unknown_00 are exact but `multiple` (int twins in game
  code), so uncounted. MoneyClass got `operator!=` (VC6 instantiates a template's virtuals with the
  class, so Set_Value is compiled for money).
* name.cpp `Get_Rest` 0x4ADBB0 (c410fef); lookup.cpp `Find` 0x4AC9F0 (3383224): the map at +0x10 is
  `std::map<const char *, DeltaClass *>` (the key copied to a temporary). The name tests
  0x4ACA90/0x4ACB20 call find 0x4ACBD0 out of line; every variant tried inlines it.
* `encode.cpp` (be443af, 4): `RecordTableClass` (`Records` +0, `Count` +4; ctor line 512, dtor, two
  loops over the records' slots 4/8). `policy.cpp` (6065697, 3): `CompiledPolicyClass` dtor 0x4AE850,
  `Get_Action(int id, int *value)` 0x4AED50 (line 334), `PolicyClass::Get_Action` 0x4AF490 (444).
* Not done: the time/money constructors 0x4A8150/0x4A7080 (byte-identical to each other: `multiple`,
  0 countable; and `template class X<DurationClass>;` in delta_time.cpp is error C2949 next to
  delta_template.h's `extern template`: §3), the time field's Unknown_00
  0x4A8330/0x4A8630 and accessor 0xAEB244 (ticks to wall-clock time, a guarded static).
  Drafts and scripts: `.work/day/D/` (`layout.py` lays a tree file out so asserts land on given
  lines; `m.sh` runs appfile match with the mirror's include dirs).

### 2026-10-24: auxdata continued: interpolator, string, name, permstr, linearbu; app 39 -> 92

* `delta_interpolator.cpp`/`.h` (83c2f38, 7): `DeltaInterpolatorClass` (vtable 0xAEAE54, size 0x250;
  `EndTime` +0xA0, `ChangePerTick` +0x130, `StartValue` +0x1B8, `LastTime` +0x240, `LastValue` +0x248,
  a flag +0x24C). Set 0x4A6590 needed a user-defined `DurationClass::operator=` (duration.h: the
  implicit copy schedules the two stores after the vtable load). The time field's constructor
  is out of line (0x4A8150) where the float fields' inline: `extern template class
  DeltaTemplateClass<DurationClass>;` in delta_template.h (VC6 extension, C4231) gives it; an explicit
  specialisation declaration of the constructor is ignored by VC6. Unknown_00 0x4A6930 left.
* `delta_string.cpp`/`.h` (bb3b4cf, 9): `DeltaStringClass` (0xAEB0E4, `String` +0x84), its value
  object for the encoder (`DeltaStringValueClass` 0xAEB13C on `StringValueClass` 0xAEB150) and
  `DeltaChildVisitorClass::Add_String` (kv_encoder_base.cpp's 0x4B48A0) in delta.h.
* `name.cpp` + `permstr.h` (28c1e60, 12): `NameClass` = `Name`, `Parts[MAX_NAME_LENGTH]` (10),
  `Count` (-1 until `Split`), `HasWildcard`; `PermString` (the client's name, from 0x4ADFF0's message)
  is one interned pointer. `Append` adds `rhs.Count` inside its copy loop (the client's bug, kept).
* `permstr.cpp` (cc9dd7b, 6): the hash table (`s_Table`/`s_Size` 1511/`s_Count`), a counted
  `PermStringInitClass` static in permstr.h (every including object has one: the client's 0x4A60A0,
  0x4ADC50 ...), `Find` (one-at-a-time hash).
* `linearbu.cpp` (300dff7, 18 + 3 four-byte getters at markers): `LinearBufferClass` size 0x1C
  (`Buffer`, +4 unknown, `Size`, `Capacity`, `GrowBy`, `Pos`, `OwnsBuffer`).
* delta.cpp's `Unknown_20` 0x4A53A0 (cba68ae): the base call goes after the `if`. `Add` 0x4A5670 is
  still not exact: the client calls `size()` 0x4A58C0 out of line in `_Last = S + size() + M` and
  lays out the `_Last - P < M` branch's calls differently: a different `<vector>` form (D11).
* VC6 inlines functions defined later in the same file (NameClass::Match_Parts, permstr's
  Is_Prime and entry constructor, linearbu's Seek/Read/Grow). Drafts: `.work/day/C/`.

### 2026-10-24: the Delta object model (app\shared\auxdata); app 11 -> 39

* `delta.cpp`/`delta.h` (845c0c9): `DeltaClass` (the reference decoder's DeltaBase; D24 name, D10
  `Unknown_XX` virtuals/`m_UnknownXX` members) and `DeltaCompoundClass`. Layout from the ctor
  0x4A4D40: vtable 0xAEADC8 (a one-slot interface, `_purecall`) then 0xAEAD70 (18 slots); +0x04 a
  per-class global (0xBE3A78; the compound's 0xBE3A74), +0x08 `Name` and +0x3C `FullName` (NameClass,
  0x34 bytes, ctor 0x4AD380), +0x70 a 0/1/2 state, +0x74 `Parent`, +0x78 the slot in the parent,
  +0x7C a running index, +0x80 = 0x3FF; size 0x84. Compound: vtable 0xAEABE4 (19 slots, +0x48 new),
  `Children` std::vector at +0x84, `Lookup` +0x94 (the assert's name, made by `Get_Lookup`, line
  227), +0x98 the subtree's index count; size 0x9C; its ctor is inline (0x4A6110 inlines it).
  0xAEAB88 is a class derived from it (dtor 0x4A3D00), not DeltaCompound itself. 21 functions; not
  exact: `Unknown_20` 0x4A53A0 (register-save placement) and `Add` 0x4A5670 (the client calls
  vector::size 0x4A58C0 out of line inside the inlined insert).
* Slots 0x2C/0x30 are `Read`/`Write` (the leaves' overrides use linearbu's 0x4AACE0/0x4AAFC0);
  `Read`'s second parameter is an 8-byte base the time field subtracts (`const DurationClass &`).
  Leaves are `DeltaTemplateClass<T>` (delta_template.h, whose `__FILE__` is absolute).
* `delta_money.cpp` (5ced1fe, 2: the 8-byte money at +0x88), `delta_time.cpp` (6641861, 2: 4 bytes on
  the wire; `duration.h`'s `As_Int` with `FAIL_IF(Time > INT_MAX)` on line 190), `lookup.cpp`
  (8588371, 3 index accessors at lines 67/78/90). New headers holding only what is used: name.h,
  lookup.h, linearbu.h, delta_template.h, `app/shared/money.h`, `app/shared/duration.h`.
* Header `__FILE__`: `..\shared/duration.h` is an include directory `..` plus
  `"shared/duration.h"` (VC6 joins them with `\`), so E&B ran cl in a directory one below `app`
  (e.g. `app\client`). Ours include `..\duration.h` relative to the .cpp: the string differs, the
  bytes don't (relocation). Open: see §3 (header `__FILE__` strings).
* Drafts in `.work/day/B/`: `i1.cpp` (`DeltaInterpolatorClass::Set` 0x4A6590, same length as the
  client's, one `pop esi` scheduled differently; fields `EndTime` +0xA0, `ChangePerTick` +0x130,
  `StartValue` +0x1B8 from the ctor's strings, `LastTime` +0x240), `d2.cpp` (delta.cpp's misses),
  `vary.py` (variant runner for appfile drafts).

### 2026-10-24: D23, E&B's own code in the importer (`app`); bitset.cpp, app 0 -> 11

* The importer has a non-EA library `app` (`APP_LIBS`): every file `RECONSTRUCTED`, paths under
  `app/` as in `e:\build\beta\G\app`, `/MT /W3 /O2 /Ob2 /GX` without `NDEBUG`, objmatch searching
  only the app region 0x401000-0x8C1B50. `load_state` builds its entry from `RECONSTRUCTED`;
  assemble/refine skip it; replay and report handle its subdirectories. Every other library is
  unchanged, none lost (249c365).
* `__FILE__`: the client's strings are absolute, and VC6 takes the source path as cl was given
  it. verify names app sources `e:\build\beta\G\...` through a `subst E:` and a junction in the VM
  (vc6.py `source_alias`), so bitset.obj's string is the client's byte for byte.
* `app/shared/auxdata/bitset.cpp`/`.h`: `BitSetClass` (D24), asserts on lines 8-113 through the
  layout (D25), the reconstruction notes at the end of the file. verify counts 11 of the 12; the
  10-byte destructor is too short to search for and is exact at its appfile marker.
* `app/shared/unknown_debug.h`: G_ASSERT 0x4E9410 and FAIL_IF 0x4E9450. Their object lies in
  app\shared between event.cpp and ini_manager.cpp (alphabetical link order), with E&B's operator
  new/delete, and has no `__FILE__` string: the directory is known, the name neutral (D10).

### 2026-10-24: D22 (PS), Zero Hour's bump shaders in wwshade; wwshade 138 -> 159

* The 14 shdpp.exe headers (`*.vsh_code.h`/`*.psh_code.h`) EA's release lacks are reconstructed
  from the client's .data 0xBCB8FC-0xBCEBB7 (`.work/d22/gen_headers.py`). They are shdpp's own
  form, DWORD arrays (text, NUL padded to a DWORD, a 0 terminator), not `char[]`:
  `ShdHW*Shader::Create(DWORD*)` (0xA0F3E0, 0xA0F5A0) takes them, and a `char[]` would pick the
  file-name overload. Each object's .data equals the client's bytes. The text has Zero Hour's
  instructions with the macro names substituted (e8f39a8).
* `ProjectorClass::Peek_Mapper` comes from Zero Hour's projector.h (0xA126D0 reads Mapper at +0xC8).
  The six .cpp are copied from Zero Hour with E&B's `ShdInterfaceClass(def)` base call (new block
  `SHD_BUMP_PATCHES`). Gains +21: the 19 from 6i plus both `Setup_Self_Shadow_Info` (0xA126D0,
  0xA15240). None lost, replay 0 differences (0de15b9).
* No near-misses left in the new files. Init, destructors, Copy_Vertex_Stream and Peek_Texture are
  exact but `multiple`. The files' out-of-line `DX8Wrapper::Set_DX8_Texture_Stage_State` differs
  from ww3d2's 0x9217E0 (0.47), as in the older wwshade files.

### 2026-10-24 (overnight): Zero Hour's dated edits, D9 finished, wwlib's E&B allocator; 3,925 -> 4,062

Unattended run 00:02-07:40 PDT: a coordinator and one builder subagent at a time (no parallel
builds: `verify` shares `%TEMP%\enb_import` and the VMs' scratch), queue and log in
`.work/night/plan.md`, a heartbeat every 30 min, each builder's report in
`.work/night/item*/report.md`. 65 commits `2f6c80c..e88f4e9`, none pushed. Morning report:
[overnight-report-2.md](archive/overnight-report-2.md).

* D9's texture leftovers (items 1, 6f): `Get_Reduction` 0x94BC20, `TextureClass::Apply` E&B's
  form 0x94BC80, `Get_Texture_Memory_Usage` one non-virtual base function with Generals' split
  body 0x94C1D0, `Set_Mip_Mapping` compiled in texture.cpp 0x94CA20 (a658d43..11e300b); Zero Hour's
  08/05/02 `TextureFilterClass` dropped for Renegade's five filter members, all four constructors
  0x94B240/0x94B3D0/0x94B5B0/0x94B660 exact, texturefilter.cpp/.h `NOT_LINKED` (8f10486, c35cc34,
  new block `TEXTURE_FILTER_PATCHES`).
* `DX8Wrapper` device code (item 2, 4ae710d..854e5f5, +25): WW3D2 built `/D USE_WWSHADE`,
  `Reset_Device` E&B's body, E&B's lost-device restore `Unknown_Restore_Device` 0x921C50 (D10),
  transform cache, `Init` zeroing; `Create_Device` without `DX8Wrapper_PreserveFPU` 0x921EF0 (9f11db9).
* Zero Hour-only edits in Renegade-based files (items 3, 6b, 6d, 6e): `//DMS`, ZH-tagged and
  commented-out code found by diffing every EA copy and by scanning the client's log/profile
  strings (`.work/night/item6b/strscan.py`, `fdiff.py`; `item6d/zhall.py` splices every
  differing donor function and reports gained/lost). `GapFillerClass` 0x959A80, animobj, scenes,
  sorting loops to `Get_Max_Textures_Per_Pass`, texture loader HSV shift, DDS guards, dx8caps
  (Matrox Parhelia), `Alter_Avatar_HTree` 0x98A2D0, `ShaderClass::Apply` with `GRADIENT_MODULATE2X`
  0x93B510, `post_process` 0x9BCC40, WW3D's static sort lists and `SHD_FLUSH` (fdd0a81),
  `Post_Load_Process` 0x9475E0 (b02b85e), `WW3D::Init` 0x917660.
* Dated-comment sweep (item 6i, `.work/night/item6i/dated.py`: 1,116 dated comments + 1,770 VSS
  `$Modtime`s against the client's bytes) fixed D9's cutoff: Zero Hour's KM/KJM (shader system)
  line is in through 08/01/02; the 08/05/02+ texture redesign and the Generals team's MW/GTH
  changes are out, whatever their date. D9 reworded accordingly (§2). Gain: the asset manager's
  07/01/02 shader-loader registration 0x91C2C0 (a2a0fcc).
* `dx8webbrowser.cpp` from Zero Hour (item 6k, 761d6ac, +19): `BrowserEngine.tlh`/`.tli` generated
  by VC6 from E&B's own `BrowserEngine.dll` (same GUIDs as EA's, plus `querydata` and
  `Post_MultiPart_Form`) as reconstructed files (D21); vc6.py syncs `.tlh`/`.tli`.
* Distortion (item 5, 11f65aa): external linkage of the list and an inline `expf` give 0x995E20,
  0x996180, 0x997DE0; the draw 0x9961E0 is still 369 masked rows.
* wwdebug/wwlib (item 7b): the memory log's 23 categories (client name table 0xBCB228, c7fc28c,
  +11), `FastAllocatorGeneral`'s 0xE30-byte E&B layout with out-of-line `Alloc` 0x9DAE10, `Free`
  0x9DAF60, `Unknown_Reset_Statistics` 0x9DADA0 (9850f75..debed6b), `Read_Frame` 0x9EB4C0.
* Item 4: the meshmatdesc "inlining flip" did not exist: 0x4E9CB0 is E&B's `::operator new`
  (`FastAllocatorGeneral::Get_Allocator()->Alloc`), never inlined; the three fragile functions
  differ in index load order that moves with the number of symbols declared before the classes
  (noise). Only 0x947310 is lost (D18).
* Tools: `gaps.py` and every tool use verified function starts (98a0280, 78b0a74; symbols 6,736 ->
  6,858, gaps rows 652 -> 593); importer lock `.work/locks/importer.lock` (8c0516a); VM pool
  `tools/vc6/pool.py` (87f38c3, orphan reclaim 6e298b5) and `importer.py verify --pool K`
  (c7b8bcc); `tools/re/appfile.py` for E&B's own files (5df8a3e: `app\shared\auxdata\bitset.cpp`
  12/12 exact as a draft in `.work/night/item7/try/`, not in the tree).
* Old build VMs retired (07:00): the two `VC6 SP4 (8799)` clones removed, `Tools\VMClones\1,2`
  deleted; `.work/vc6_hosts.json` lists only Win10 (192.168.193.108); `enb-vc6-01/02` stopped, free.
* E&BConfig.exe / VIDTEST.EXE assessment (`.work/ebconfig/assessment.md`): neither is a quick
  win; open question in §3.
* Verified: each commit by its builder (verify of the affected libraries, pair sets vs
  `.work/night/imports.start.json`, `replay` 0 differences), cumulative pairs checked by the
  coordinator after each item. In-tree counts: WW3D2 2,362 -> 2,477, WWDebug 67 -> 81, WWLib 557
  -> 565; WWAudio 351, WWMath 370, WWSaveLoad 80, wwshade 138 unchanged; total 3,925 -> 4,062
  (+137). Lost pairs, all COMDAT duplicates still matched elsewhere (D20): (animatedsoundmgr,
  0x6CF410), (meshmatdesc, 0x497330), (meshmdl, 0x909500), (ww3d, 0x909500), (texturefilter,
  0x42F250/0x53F1C0/0x9217E0; file dropped; 0x94CA30 moved to texture), (wwmemlog and wwprofile,
  0x9CEFF0).
* Decisions: D20-D32 (user, during the night; §2), D9 reworded, D18 kept. Open: §3.
* Next step: D22 (PS, §4 item 1), from `.work/night/item6i/report.md` (PS) and its trial script
  `.work/night/item6i/shdtry.py`.

### 2026-10-23: D19's distortion draw rebuilt (not byte-exact); its neighbours are uncountable

* `UnknownDistortionClass::Unknown_Render_Distortion` (0x9961E0) written from the client's
  disassembly (0a4514e): the camera-facing square projected to whole pixels, the on-screen part
  of the back buffer copied to a `TextureClass` (clamp, no filtering), world/view/projection set to
  identity with `_PresetOpaque2DShader` (depth always) and PRELIT_NODIFFUSE, a grid of 8-pixel
  cells in three function-static `DynamicVectorClass` lists with each texture coordinate sent
  through `Unknown_Distort`, drawn through the dynamic VB/IB, view and projection restored. Kept
  as the client has them: uv0 scaled by the width only, uv1 by the height only; the early returns
  for fewer than 2 cells leave the view and projection unrestored.
* Not byte-exact: 6,672 bytes vs 6,759; 369 of ~1,530 instructions differ with stack offsets and
  relocations masked. Left: the view restore (the client builds a `??_H` Matrix4 temporary, ours
  doesn't), the index loop's rotation and registers, the static guards' registers, a frame 0x18
  bytes bigger in the client (0x208 vs 0x1F0: locals not found). Forms that did help: early
  returns with their own cleanup copies; `Get_Translation(Vector3 *)`; `y1 >= height ? ... : y1`;
  `sw`/`sh` before the clamped size; a `Vector2 offset` temporary; `i0`/`i1` index locals.
  Tried and worse: `RefCountPtr` locals (the client doesn't null-check texture or back buffer),
  `Vector2` sum for the vertex, `for (int ...)` loops, hoisted `v`, index-count orderings.
* Neighbours 0x997EA0..0x9981A0: no source needed. They are
  `DynamicVectorClass<UnknownDistortionClass *>`'s template members (plus static cleanups
  0x997C50/90/D0, 0x997D90, 0x998120); our object compiles each byte-identical, but the client has
  33-140 identical copies, so `multiple`, 0 counted (new trap in workflow.md §3). 0x9981F0 is an
  8-byte `this`-adjusting thunk (too short to count).
* `Unknown_Add_To_Distortion_List` 0x995E20: only `add ecx, eax` vs `add eax, ecx` (registers).
* Verified: `importer.py verify --lib ww3d2` 0 errors, 2,362 in tree (unchanged); pair sets +12
  (template instances in unknown_distortion), 0 lost; `report`; `replay` 0 differences.
* Tools: none in tools/. Scratch in `.work/`: `dist_compile.py` (compile the file alone, objdiff the
  draw), `dist_mask.py` (diff with esp offsets/relocations/immediates masked), `dist_try.py`
  (score variants), `dist_head.py` (exact sets vs HEAD), `objmatch_all.py` (objmatch without the
  5-address cap), `dist_v*.py` variant specs, `imports.before-draw.json`.
* Decisions: none. Open: item 1 rewritten.
* Next step: another open item (WW3D2 near-misses, `GapFillerClass`, `_Get_Total_*_Size`), or the
  draw's frame size if a lead turns up.

### 2026-10-23: saved Ghidra pseudocode (`pseudocode.py`); REA timeouts confirmed

* REA's raised timeouts (3d44d68) work: the first `batch_decompile` came back after Ghidra's
  analysis (285 s), moved to a background task at Claude Code's 120 s mark. Correction to the
  entry below: the distortion session's retries did **not** work. All four REA calls (line groups
  0x97BAF0/0x97C900/0x97C360; distortion 0x9961E0 and 0x997EA0..0x9981A0) failed with
  `provider_timeout`, hidden because the ~85 KB-per-item error payloads overflowed the result.
* Then 0x9961E0 failed differently: Ghidra defines no function there (reached only from the
  vtable at `.rdata` 0xB13984) and REA can't create one. So: `tools/re/pseudocode.py` +
  `tools/re/ghidra/DecompileTargets.java` (20541ad) run Ghidra headless, create a function at
  every `clientimage.py` start, name them as `client.py` does, and save pseudocode for every start
  not byte-exact to `.work/pseudo/`: 25,063 ok, 0 failed, 119 refused (false starts
  mid-instruction, all in the library region). Full run 80 s once the project exists.
* CLAUDE.md: the saved pseudocode is now step 3 of the tool order, ahead of REA (user).
  workflow.md §3: the vtable-only trap updated, one new (Ghidra's dotted-path and `&` limits).
* Noticed, not followed up: gaps-ww3d2 places `~MeshModelClass::Init_For_NPatch_Rendering?` at
  0x95A482, mid-instruction; the location is probably wrong.
* Verified: the run's counts above; spot checks of 0x9961E0 (28 KB) and 0x97BAF0 (callees named).
  No library changed, no `importer.py verify` needed.
* Next step: rebuild the distortion draw 0x9961E0 (D19) from
  `python tools/re/pseudocode.py --show 0x9961E0`, then its neighbours 0x997EA0..0x9981A0.
* Outside git: `.work/pseudo/` (59 MB, `index.tsv`), `.work/ghidra/` (inputs, a copy of
  client.exe, `headless.log`), the Ghidra project in `%LOCALAPPDATA%\EnB\ghidra`.

### 2026-10-23: WW3D2 particles, textures, line groups (after the mapper swap)

* WW3D2 2,310 -> 2,352 (+42); gaps 347 -> 316; `symbols.json` 6,703 -> 6,736 (none gone). Every step
  verified by pair sets (`importer.py verify`), `replay` (0 differences) and committed on its own
  (dbdb1d6..7eaf120); each fix is a row in library-matching.md §3.6.1. Full verify at the end: all
  seven libraries 0 errors, WW3D2 the only one changed.
* D18 taken (user): meshmatdesc's byte-only include-order fix is not applied; kept on record.
* Particles (cluster 2 of the near-miss entry, done but for one function):
  * `PointGroupClass::Render` and `LineGroupClass::Render` take the object's transform (E&B; the
    app passes static matrices). `LineGroupClass::Render` rebuilt with three E&B members (transform
    setup, index fill, vertex fill; names not known, D10): all four match.
  * `ParticleEmitterClass`: emitter-space emission when `UnknownFlag` (+0xFD) is set (two E&B
    members), constructor (`FutureStartTime` and `EmitRemain`), `Create_New_Particles`
    (reciprocal frame time, by-value lerp), `Build_Definition` passing the flag,
    `Unknown_Get_Position` (0x97E260).
  * `ParticleBufferClass`: Zero Hour's line groups (`GroupID`, `Render_Line`), E&B's bounds in
    object space (`Update_Cached_Bounding_Volumes`), Zero Hour's `Scale` (+
    `SegLineRendererClass::Scale`). `ParticleEmitterDefClass::Save_Extra_Info` writes `m_ExtraInfo`.
  * Left: `PointGroupClass::Render` (0x954960) matches structurally but its block-scope stack
    slots are placed differently throughout; no lead.
* `WW3D::Set_Render_Device` (cluster 3): both overloads call E&B's `Unknown_Reset_Distortion_List`
  (0x995DC0). Matched in fact but not counted: the overloads are now twins (objmatch `multiple`).
  The E&B module behind it (a screen-distortion effect) is D19.
* Textures (cluster 4, D9): `Peek_D3D_Base_Texture` without `LastAccessed` (+4, `TextureClass::Init`
  among them), E&B's `Invalidate` (Zero Hour's commented-out "battlefield version" plus the
  load-task test), `Invalidate_Old_Unused_Textures` keeping last frame's count (+ an E&B getter),
  `Get_Texture_Information` taking a `StringClass &`, `TextureLoader::_UnknownInvalidationTime`
  (0xC01290, set by the app). Left: the four `_Get_Total_*_Size` functions inline two different
  memory-usage bodies split on `Initialized` (not found in any EA copy); the `TextureClass`
  constructors, `Apply`, `Get_Reduction` and `DX8Wrapper`'s device code as before.
* Not started: `GapFillerClass` (cluster 5).
* Tools: none new. One trap added to workflow.md §3 (twin overloads after an added call).
* Verified: as above. Nothing run live. REA opened once (Ghidra hit its 330 s startup deadline on
  the first batch; the retry worked but was not needed in the end); closed.
* Decisions: D18 (user). Open: D19 (proposed below).
* Next step: D19's module if the user agrees; else `GapFillerClass`, the `_Get_Total_*_Size`
  family, or the remaining D9 texture items.
* Outside git: `.work/*_edit.py`, `*_variants*.py`, `*_spec.py`, `*_reasons.py`, `*_edited/` for
  each step; `.work/imports.mapper-base.json` (the session's baseline), `.work/pairs_cmp.py`.

### 2026-10-23: WW3D2 texture mappers from Zero Hour

* WW3D2 2,242 -> 2,310 (+68 client functions); gaps 407 -> 347; mapper.obj, matrixmapper.obj and
  vertmaterial.obj now have no gaps. Other libraries unchanged (full `verify`, 0 errors).
* The previous entry was wrong about the cluster: the client *has* Zero Hour's texture-matrix
  mappers. The vtables (`vtables.py --seed`, 0xB10EAC/0xB10F00/0xB10F24) are in Zero Hour's slot
  order with `Calculate_Texture_Matrix` at slot 8, and `ScaleTextureMapperClass::Apply` 0x9322F0
  (inherited by `LinearOffsetTextureMapperClass`) sets `D3DTS_TEXTURE0+Stage`. Zero Hour's copy
  matched only 2 functions in the evidence because of its own headers; against the tree it
  matches everything. Trap added to workflow.md §3.
* Done: `mapper.cpp`/`.h` and `matrixmapper.cpp`/`.h` swapped to Zero Hour's (`trial.py`); the
  Renegade-copy Grid patches in `PATCHES` dropped (Zero Hour has them). New importer block
  `MAPPER_PATCHES`: Zero Hour's 16-float `Matrix4` constructor/`Init` (11/2001), and
  `VertexMaterialClass::Load_W3D` (0x92F2A0) building the WS mappers from the Args INI plus the
  `GRID_WS` cases (Renegade's single function otherwise).
* Pair sets against the session's baseline: 71 gained; 3 lost, of which 2 are not real
  (`LinearOffsetTextureMapperClass::Reset` 0x932CC0 stays matched in mapper.obj, only
  vertmaterial's twin went; `operator*(Matrix3D)` 0x7E6E40 is an app-region duplicate still matched
  in 4 files). The real loss: `MeshMatDescClass::Make_UV_Array_Unique` 0x947310 (register
  allocation only; meshmatdesc.cpp's known fragility, now that `mapper.h` includes `matrix4.h`).
  meshmatdesc.cpp gained 2 at the same time (0x946EB0, 0x947490), net +1. Accepted and
  written down (library-matching.md §3.6.1), as for the Grid change.
* `symbols.json` 6,580 -> 6,703 (none gone); `gaps-ww3d2`, `imports.md`, `coverage.md` regenerated.
* Verified: `importer.py verify` (all libraries), pair sets, `report`, `replay` (0 differences).
  Nothing run live.
* D18 (user): don't include `matrix4.h` first in meshmatdesc.cpp, though `tryvariants.py` shows it
  wins 0x947310 back and loses nothing in that file (no EA copy has that include order). Kept on
  record; revisit if it becomes WW3D2's last outstanding item.
* Next step: one of the remaining clusters (particles, `Set_Render_Device`'s E&B call, textures
  under D9, `GapFillerClass`).
* Outside git: `.work/m4_16f.py`, `vm_ws_edit.py` (the edits as made), `mapper_spec.py`,
  `mapper_reasons.py`, `mapper_drop_patches.py`, `mapper_docs.py`; `.work/mapper_edited/` (edited
  files, matched the addpatch result); `.work/imports.mapper-base.json` (the baseline);
  `.work/mmd_variants.py` (the meshmatdesc variants). REA not opened.

### 2026-10-23: WW3D2 near-misses: mesh rendering per mesh model

* WW3D2 2,202 -> 2,242, none lost (pair sets compared at every step); wwshade 138 unchanged.
  Five commits, 0941037..c5fcd0e; each fix a row in library-matching.md §3.6.1.
* Decisions D15-D17 recorded (the three open questions of the previous entry, all as recommended).
* The main find: E&B's mesh rendering is **per mesh model**, as in Generals: polygon renderers live
  in `MeshModelClass` (+0x84; the member was already declared, the code still used `MeshClass`'s
  list) and `DX8MeshRendererClass::Register_Mesh_Type`/`Unregister_Mesh_Type` take the model and
  keep a `_RegisteredMeshList` (0xC07618). `MeshClass` has no user lighting and no list of its own
  (size 0xA8); nor has `RenderObjClass` any user-lighting persistence (Generals' factory
  `Save`/`Load`). Kept from Renegade: the sort-level test, the delayed material passes, the
  gap-filler removal. New importer block `MESH_MODEL_RENDERER_PATCHES` (91 patches, written with
  the new `tools/vc6/mkspec.py`). +34.
* `MeshClass`'s vtable has one virtual of its own (slot 113, 0x941310, returns 0x750) where
  Renegade has `Save_User_Lighting`/`Load_User_Lighting`: `Unknown_Virtual_113` (D10).
* Smaller Zero Hour pieces (importer `PATCHES`): `compute_static_sort_levels`' screen case,
  `SegLineRendererClass::Render`'s `rgbas` argument, `MeshClass::Make_Unique(bool)`,
  `MeshGeometryClass::operator=` cloning the cull tree, `TextureLoadTaskClass`'s `HSVShift`
  zeroed; `MeshClass::Cast_Ray` without `IgnoreTranslucentMeshes`. +10.
* Clusters found, not started (each a session):
  1. `mapper.cpp` (~50 gaps): the client's `LinearOffsetTextureMapperClass` constructor 0x9329F0 is
     Zero Hour's (start offset +0x28, clamp flag +0x30, five arguments), yet Zero Hour's
     `mapper.cpp` matches only 2 functions (Renegade's 18): E&B has ZH's members without its
     texture-matrix `Apply`. `vtables.py` mis-seeds the class (0xB10E4C): seed it.
  2. Particles: `PointGroupClass::Render` takes a second argument (the transform, `[esi+0x1C]`,
     from `ParticleBufferClass::Render_Particles` 0x95DC90); `ParticleEmitterClass` has E&B fields
     at +0xE4 and +0xFD (`Emit` 0x97E330, `Build_Definition` 0x97EDE0).
  3. `WW3D::Set_Render_Device` (0x917790, 0x9177E0) calls an unknown E&B function 0x995DC0 on
     success (reads the device resolution, compares with 0x400).
  4. Texture (D9): the client's `Peek_D3D_Base_Texture` seems not to set `LastAccessed`
     (`ZTextureClass::Apply` 0x94D190); `TextureLoadTaskClass::Begin_*_Load` differ in argument
     evaluation only (Zero Hour's versions are the excluded 08/05/02 redesign).
  5. `GapFillerClass` (3 functions, 0.75-0.78): register scheduling only; cause not found.
* Tools: `tools/vc6/mkspec.py` (tree edits -> addpatch spec). Three traps added to workflow.md §3.
* `symbols.json` 6,551 -> 6,580; `gaps-ww3d2` regenerated (407 gaps, 461 at the start).
* Verified: every step by `importer.py verify` for ww3d2 and wwshade (pair sets before/after),
  `report`, and `replay` (0 differences). Nothing run live; nothing waits on the user.
* Decisions: D15-D17 taken by the user this session (as recommended).
* Open questions: none new.
* Next step: one of the clusters above; mapper.cpp has the most functions. Start by seeding its
  vtables (`vtables.py --class LinearOffsetTextureMapperClass --seed ...=0x9329F0 --list`) and
  comparing the constructors against Zero Hour's member list.
* Outside git: `.work/mmr_*.py`, `rendobj_edit.py`, `*_spec.py`, `*_reasons.py` (edits and specs
  as applied); `.work/mmr_edited/` (edited files, matched the replayed tree); `.work/odbatch.py`
  (condensed objdiffs of the top gaps), `.work/fn.py` (print one function). VMs at
  192.168.193.108, .202.231, .207.76 (`vc6.py check` passed). REA not opened this session.

### 2026-10-23: WWAudio complete (0 gaps); wwshade's ShdMeshClass

* WWAudio 298 -> 351, every function our source compiles now byte-exact (`gaps-wwaudio`: 0);
  wwshade 136 -> 138; WW3D2 2,202 unchanged in every verify. All in importer `PATCHES` (wwaudio)
  and `WWSHADE_EARLY_PATCHES` (wwshade), one commit per step, each in library-matching.md §3.6.1.
* E&B's WWAudio changes found (names not known keep D10 names):
  * `Sound3DClass`: a software Doppler effect. `Set_Velocity` takes a second float (elapsed ms)
    and sets the pitch from the velocity relative to the listener (constants 0.85, 3.4, 1/33.3);
    `On_Frame_Update` re-enables Renegade's `#if 0` auto-velocity behind `_UnknownDopplerFlag`
    (0xC404F8, the app's "Doppler_Enabled", 0x51461C) and alone updates Miles;
    `Set_Listener_Transform` tracks the listener velocity (`m_Unknown148`, `m_Unknown160`);
    `m_Unknown164/168/16C` are floats. The one-argument `Set_Velocity` of `SoundPseudo3DClass` and
    `Listener3DClass` therefore hide it (new vtable slots): the session's first guess, a non-const
    `Vector3 &` in `SoundPseudo3DClass`, was replaced (same bytes).
  * `WWAudioClass`: a pause-all flag (`m_Unknown19A5`, set by the app; `AudibleSoundClass::Play`
    queues sounds paused; `Unknown_Resume_All` 0x9F4720 resumes) and a volume lock
    (`m_Unknown19A6`, set by the new `Temp_Disable_Audio`); a per-kind instance limit
    (`AudibleSoundClass::m_Unknown80` kind, `m_Unknown7C` maximum, default 4) in
    `Reprioritize_Playlist` (`Unknown_Count_Instances` 0x9F1090), `Get_2D_Sample`, `Get_3D_Sample`;
    the `-NOAUDIO` check (`#ifdef G_CODE_BASE` compiled: it stops `Is_Disabled` being inlined, +13
    callers); `Add_To_Playlist` drops replayed sounds from `m_CompletedSounds`; providers tried
    D3DSOUND, EAX, PSEUDO, A3D, DOLBY.
  * `SoundSceneObjClass`: a deferred release (destructor queues `m_UserObj`/`m_AttachedObject`;
    flushed by `WWAudioClass::On_Frame_Update` and `Shutdown`).
  * Miles: E&B's `mss.h` had `AIL_startup()` call `MSS_auto_cleanup()` (`sdk_patch.py` restores
    it; toolchain.md §2 fix 4: a full link needs `MSS_auto_cleanup`, a static member of the SDK's
    mss32.lib, which the generated import library lacks). Also the Miles lock or mutex in several
    functions, and `Delayed_Release_Thread_Proc` skips passes while `AIL_background()`.
* `AudioSaveLoad.cpp`/`.h` dropped (D13: no save/load subsystem in the client). Accepted loss:
  its three pairs, shared helpers still matched by other files.
* wwshade: `ShdMeshClass::Update_Cached_Bounding_Volumes` (`operator *`, as `MeshClass`) and
  `Load_W3D` (`Name` set inline, no sub-mesh names). `Render` already matched (stale note fixed).
  `Get_Num_Vertices`, `Get_Sub_Mesh_Count`, `Peek_Sub_Mesh` are not in the client.
* The `"UnNamed"` null test (open question below): ruled out `/GF` off, `/Gf`, `/Oa`, `/Ow`, `/Op`,
  `_NO_INLINING`, and the literal as a const pointer, pointer variable, array, cast or with an
  explicit hint. The client threads the same jump in WWAudio's `Add_Logical_Type` (a variable
  string), not with the literal.
* Tools: `tools/vc6/trydiff.py` (compile one variant, keep its object for `objdiff.py`),
  `tools/re/objrel.py` (a function's relocations by symbol). Five traps added to workflow.md §3.
* `symbols.json` 6,494 -> 6,551; `gaps-wwaudio` (0) and `gaps-wwshade` (5) regenerated.
* Commits: d32f1b3..(this handoff) on master. Nothing pushed.
* Verified: every step by `importer.py verify` for each library including the changed header
  (pair sets compared before and after; WW3D2 whenever a WWAudio header changed) and `replay`
  (0 differences). Nothing run live; nothing waits on the user.
* Decisions: none taken by the user this session; D10 and D13 applied.
* Open questions:
  1. `G_CODE_BASE`: kept as a patch removing the two `#ifdef`s in `WWAudio.cpp`; defining it for
     WWAudio (`LIB_FLAGS`) gives the same bytes. Recommendation: keep the patch (only this file uses
     it, so the bytes can't tell a project-wide define from a local change).
  2. Names for the E&B members now understood (`m_Unknown148` the listener's velocity,
     `m_Unknown164/168` the pitch range, `m_Unknown19A5` pause-all, ...): D10 allows renaming once
     their code is rebuilt. Recommendation: keep the neutral names with the comments now in the
     headers, since E&B's real names are still unknown; rename only if the app code shows them.
  3. The `"UnNamed"` null test (above). Recommendation: leave it; the only two functions affected
     are these constructors.
* Next step: WW3D2 near-misses and the texture ones (D9: `TextureClass` constructors, `Init`,
  `Apply`, `Get_Reduction`; `DX8Wrapper` device code). Start by regenerating
  `docs/data/gaps-ww3d2` (`gaps.py --title ww3d2 --out docs/data/gaps-ww3d2
  %TEMP%\enb_import\verify-ww3d2\*.obj`) and take its highest-similarity entries, checking each
  pairing against the vtable first (the WWAudio pairings were often wrong).
* Outside git: the REA session (client.exe, analysed) may still be open; `.work/*_spec.py`,
  `*_v.py` as applied; `.work/pc_*.c` pseudocode; `.work/pairs_cmp.py`.

### 2026-10-23: wwshade's E&B code; `_MBCS` and `ENABLE_WWPROFILE` builds

* Last session's regenerated evidence had gone to `%TEMP%\enb_bytematch`, not `.work/bm2`
  (trap added); new `tools/vc6/refresh_expected.py` refreshes plan counts from new evidence
  (wwshade best copy alone 46 -> 61, 0 files below their copy).
* wwshade 118 -> 136, none lost (importer `WWSHADE_EARLY_PATCHES`): E&B's fixed-function light
  setup `ShdHWVertexShader::Unknown_Set_Lights` 0xA0FC60 (name not known, D10) and its calls at the
  end of the Simple, Gloss Mask and Cube Map DX6 `Apply_Shared`; materials set there, not in
  `Apply_Instance`; definition defaults (`Ambient` 0,0,0; cube map `Diffuse` 0,0,0; copy ctors assign
  in their bodies); Bump Specular always DX6; Zero Hour's `Get_Pixel_Shader_Minor_Version`
  (`WWSHADE_API_PATCHES`).
* Library flags (D12; importer `LIB_FLAGS`, `bytematch_all.sh LIBFLAGS`):
  * `_MBCS` in wwshade and WWAudio: `_mbslen` 0xA2BCAB has four callers, all there. WWAudio +2.
  * `ENABLE_WWPROFILE` in wwshade, WW3D2, WWAudio, WWMath (`WWProfileSampleClass`; its dtor
    0x444160 is called 212 times by E&B's own code, so probably project-wide). WW3D2 2,179 -> 2,202,
    WWAudio 266 -> 298, WWMath +1, wwshade +2. E&B dropped two samplers: `ShdDX8RendererClass::Flush`
    0xA08060, `ParticleEmitterClass::On_Frame_Update` 0x97DEF0 (patched out). Accepted losses:
    twin pairings only (library-matching.md §3.5, §3.6.1).
* Open: `ShdDefClass(classid)` 0xA0EC30 and the `ShdMeshClass` ctor 0xA08F80 differ only in the
  `StringClass("UnNamed")` inline: the client tests the pointer again after the `_mbslen` call, ours
  folds the test (not `/O1`, not `_MB_MAP_DIRECT`, all EA copies of the ctor the same).
* Evidence regenerated with the new flags (`.work/bm2`), plan counts refreshed; refine: WWMath +2
  (`matrix4` 0x8DEE60, 0x8DEEC0 from Zero Hour); its wwshade splice of Zero Hour's
  `ShdBumpSpecDefClass::Init` was a twin match undoing an E&B patch, reverted (trap added).
* `docs/data/gaps-wwshade.*` generated for the first time (7 left), `gaps-wwaudio` regenerated (48);
  `symbols.json` 6,438 -> 6,494.
* Commits: 23bd311..66236bf on master. Nothing pushed.
* Verified: every step by `importer.py verify` (pair sets compared before/after) and `replay`
  (0 differences). Nothing run live; nothing waits on the user.
* Decisions: D14 (user): `ENABLE_WWPROFILE` in WWDebug too (neutral, none lost; soft). The other
  flag changes apply D12 (user); the helper's neutral name applies D10.
* Open questions:
  1. The `"UnNamed"` null test (above). Recommendation: leave it until `ShdMeshClass`'s ctor is
     worked on; it is the same issue in both functions.
* Next step: `ShdMeshClass::Update_Cached_Bounding_Volumes` 0xA09ED0, then `Load_W3D` 0xA0A380:
  `python tools/re/objdiff.py %TEMP%\enb_import\verify-wwshade\shdmesh.obj
  "?Update_Cached_Bounding_Volumes@ShdMeshClass@@MBEXXZ" 0xA09ED0`
  (gaps-wwshade pairs `ShdMeshClass` with app-region addresses; use the vtable/ctor addresses).
  After that, the 48 WWAudio gaps (`docs/data/gaps-wwaudio.md`, regenerated this session).
* Outside git: `.work/bm2` is the current evidence (new flags); `.work/bm2_prev`,
  `.work/bm2_old_noGX` older sets (deletable). Specs as applied: `.work/{lights,cdef,defs,
  gm_inst,caps,bsinit,prof,w3prof}_spec.py`; `.work/pairs_cmp.py` (pair-set diff of two
  `imports.json`), `.work/sub_flags.py` (compile a library with given flags). VMs:
  `.work/vc6_hosts.json` (192.168.193.108, .202.231, .207.76).

### 2026-10-23: D9 step 3, E&B's earlier wwshade; wwshade's flags

* **wwshade was built `/GX /Ob1`** (the other libraries `/Ob2`, no `/GX`): EH frames in its
  functions; on the tree, `/GX` +16 own functions, `/Ob1` +15 more, none lost; Zero Hour's copy
  alone 46 -> 61. `importer.py` `LIB_FLAGS`/`flags_for` and `bytematch_all.sh` `LIBFLAGS` carry it;
  bm2 evidence regenerated. Kept by the user (D12).
* The client's wwshade rebuilt where it is earlier than Zero Hour's (importer
  `WWSHADE_EARLY_PATCHES`, 69 patches, each with its address): renderer node on
  `MultiListObjectClass` only, `MeshContainerClass` with a visible list per pass, no list
  containers, sorting or skins; `ShdInterfaceClass(def)` without class id or comparison/opacity
  virtuals; `ShdSubMeshClass` without visible range, sorting, legacy conversion, skins, influence
  chunk; no LegacyW3D shader or legacy loader (class ids 1..5, 5 force links, one loader);
  E&B's "Cube Map (unsupported)" shader on a D3DX cube texture. WW3D2: Zero Hour's fill light
  (`lightenvironment.obj` now complete). WWLib: Zero Hour's const `RefCountClass`.
* Byte-exact: wwshade 56 -> 118, WW3D2 2,173 -> 2,179 (+7 pairs), others unchanged, against
  `.work/sub_base_imports.json`. wwshade's 20 lost pairs are mispairings (other libraries' inline
  copies, still matched there; `0x93ED00` Renegade's `MeshClass::Clone`, which only the old
  no-`/GX` `ShdMeshClass::Clone` matched; the real 0xA092C0 now matches). `replay`: 0 differences.
  `report` regenerated.
* `shdlegacyw3d.cpp`/`.h` dropped (D13): importer `NOT_LINKED` keeps them out of plan, assemble and
  the state. wwshade now compiles with 0 errors; nothing gained or lost.
* `symbols.json` regenerated (6,317 -> 6,438 names; 0x93ED00 is now `MeshClass::Clone`).
* Not yet refreshed: the plan-time "best copy alone" totals and per-file `expected` counts in
  `imports.json` still come from the old no-`/GX` wwshade evidence (verify prints 46, and 3 files
  "below their copy"); a `refine --lib wwshade` against the new bm2 should update them.
* Commit: this entry's work, on master. Nothing pushed.
* Next session: E&B helper 0xA0FC60 (lights from the render state, end of three `Apply_Shared`);
  cube map definition functions (0xA0A840-0xA0AD60, a `.dds` path); `ShdMeshClass` ctor, `Render`,
  `Load_W3D`.
* Outside git: `.work/sub_*.py` (edits as applied, variants, `sub_mkspec.py` diff -> addpatch spec,
  `sub_cmp.py` pair comparison, `sub_flags.py` compile with given flags).

### 2026-10-23: tools for the next sessions (client.py, vtables.py, addpatch.py, tryvariants.py)

* New `tools/re/client.py` (`dis`, `name`, `refs`, `callers`): the lookups the wwshade step
  wrote as throwaway scripts, with addresses named from symbols.json, the gaps reports and
  the thunk table.
* `vtables.py`: anchors also from symbols.json (by plain name for string-proven ones), through
  incremental-link thunks; the class's own and rare slots weigh more; a table on slots that
  most tables share is "not placed" instead of a base class's table; tables end at the next
  stored vtable; `--seed Class=ADDR` and `--list`. Checked on ww3d2 against the old version:
  no primary table moved except `XSurface` (now 0xAFC29C, 16 own anchors) and
  `TexProjectClass` (0xB11120, all 8 slots); `SoundRenderObjClass`/`StreakLineClass` no longer
  land on `RenderObjClass`'s table; reported differences 214 -> 57.
* `symbols.json` regenerated (6,100 -> 6,317 names, this session's matches included).
  Removed: the names of an E&B class derived from `TexProjectClass` (table 0xAEA764, its
  `Pre_Render_Update` 0x4941A0), which the old alignment had given `TexProjectClass`'s, and
  `StreakLineClass::Clone` on a shared stub (0x9CCD10).
* Promoted from `.work`: `tools/vc6/addpatch.py --block NAME SPEC.py` (apply patches and record
  them in an importer block; replaces the `d9_`/`ws_addpatch.py` copies) and
  `tools/vc6/tryvariants.py LIB FILE V.py` (one file's variants compiled alone). Both tested:
  a new block replays with 0 differences; a no-op variant gains and loses nothing.
* REA tested: its first request runs Ghidra's full analysis (over 2 minutes), every session.
  A `close_binary` snapshot (14 KB) only caches answers already given; reopening with it, a new
  address waited over 2 minutes again. Names set in REA are lost on close. CLAUDE.md's tool
  order now says so (client.py first, REA only for pseudocode, opened early). CLAUDE.md is not
  tracked by git.
* Commits: 60e66ab (wwshade's WW3D2, merged into master by fast-forward), 8501576 (tools),
  this entry. Nothing pushed.
* Next session: start with the step-2 entry's "Next" (the client's older `ShdSubMeshClass` and
  renderer node). Find `shdsubmesh.obj` in the client with `client.py refs`/`callers` from the
  node constructor's calls (0xA082D0), and the classes' tables with `vtables.py --seed`.

### 2026-10-23: D9 step 2, wwshade's WW3D2

* Each wwshade compile error checked against the client before adding anything: the Zero Hour
  WW3D2 features it confirms were added (importer `WWSHADE_API_PATCHES`, 61 patches, each with
  its address). Self-shadow bit, `Get_Max_Lights`, `Apply_Default_State` (an earlier form),
  `Set_Ambient`/`Ambient_Color`, two vertex streams, non-FVF vertex buffers, `Light_Environment`/
  `Render_Info`, `MAX_TEXTURE_STAGES` = 8 (dx8renderer on `MeshMatDescClass::MAX_TEX_STAGES`, as
  Zero Hour). This reverses an earlier note that it stays 2: `render_state.Textures` has 8 slots
  in the client. Details and addresses: library-matching.md §3.6.1.
* E&B's own additions found: `Set_Light_Environment(env, flag)` (0x926460; the shader renderer
  passes 0 to only record the environment) and a setter for `Render_Info`. Neutral names
  (`unknown_set_lights`, `Unknown_Set_Render_Info`) per D10.
* E&B's wwshade is older than Zero Hour's in places: no skin/sorting/visible-polygon code in the
  renderer node or `ShdSubMeshClass`, `Register_Mesh` only creates the node, `Cast_Ray` keeps
  Renegade's `IgnoreTranslucentMeshes`. Zero Hour features asked for only by that code
  (`CheckTranslucent`, `MultiListIterator::Last`, `MeshGeometryClass::get_deformed_vertices`)
  were not added.
* Byte-exact client functions: WW3D2 2,148 -> 2,173, wwshade 20 -> 56, WWAudio unchanged; sets
  of `(file, address)` compared against the session's starting `imports.json`
  (`.work/ws_base_imports.json`): none lost. `importer.py replay`: 0 files differ. `report` regenerated.
* Committed on `d9-texture` and merged into master. Three new traps in workflow.md §3.
* Decided by the user: commit and merge `d9-texture` into master; the neutral names stay (D10).
* Open: as before, Renegade's `Set_Render_Target(TextureClass *)` and `Get_Reduction`.
* Next: rebuild the client's older wwshade code from its disassembly. `ShdSubMeshClass` without
  `FirstVisiblePolygon`/`VisiblePolygonCount`/`Sorting` (and `Init_From_Legacy_Mesh_Model`,
  `ShdMeshClass::Init_From_Legacy_Mesh`/`Load_W3D` uses), `MeshContainerClass::Register_Mesh`
  (0xA081D0), the node constructor (0xA082D0: streams, then index buffer, polygons 0..count) and
  `Flush` (0xA08790), dropping `Get_Deformed_Vertices`. First find where `shdsubmesh.obj` sits
  in the client (gaps.py misplaces it). Then `TEX_CUBEMAP`, `const Add_Ref`, the
  `VertexMaterialClass` parsers. The client's `MeshClass::Cast_Ray` (0x940530) has no
  translucency test: a WW3D2 near-miss.
* Outside git: `.work/ws_spec*.py` (the patches as applied), `ws_verify.sh` (verify ww3d2,
  wwaudio, wwshade and compare with the baseline `.work/ws_base_imports.json`). The other
  `ws_*.py` helpers are superseded by `tools/re/client.py`, `tools/vc6/addpatch.py` and
  `tryvariants.py` (next entry).

### 2026-10-23: D9 step 1, the texture system

* E&B's texture system is Zero Hour's as of its 06/27/02 "texture class abstraction" (Kenny
  Mitchell's change logs in Zero Hour's files), without the 08/05/02 "redesign (revisited)".
  Client evidence: the vtables at 0xB119A0 (`TextureBaseClass`, the two trackers, `TextureClass`,
  `ZTextureClass`, 6 slots each, no cube or volume tables); the base constructor 0x94B020 (5
  arguments, `D3DTexture` at +0x34: no `IsReducible`); nothing calls `D3DXCreateCubeTexture`/
  `D3DXCreateVolumeTexture`; the load task has no vtable; Zero Hour's byte-exact Z textures,
  depth render targets, shader setters and draw-call statistics. Details: library-matching.md §3.6.1.
* Done as Zero Hour's files where the client is Zero Hour's (`texture`, `texproject`,
  `dx8texman`, `bitmaphandler`, `texturethumbnail` and four headers) and as Renegade's files with
  Zero Hour's 06/27/02 parts patched in where it is Renegade's (`DX8Wrapper`, `textureloader`,
  `ddsfile`). 122 patches (19 files) in importer.py's new `TEXTURE_API_PATCHES` block, each with its evidence.
* Byte-exact client functions: WW3D2 2,060 -> 2,148, wwshade 13 -> 20; none lost (two inline
  instances moved from `texture.obj` to other objects). `importer.py replay`: 0 files differ.
* Commits 8c83880, 13ea6b4 on branch `d9-texture` (master unchanged, nothing pushed).
  `trial.py --errors` writes every compile error. Four new traps in workflow.md §3.
* Verified: VC6 verify of ww3d2, wwaudio, wwshade (sets compared by address against the
  pre-D9 imports.json), `importer.py replay` 0 differences, `report` regenerated. Not run:
  the other libraries' verify (their headers didn't change), `symbols.py`/coverage regeneration.
* Decision: D9 reworded by the user (§2) after this step.
* Open: (1) merge `d9-texture` into master? Recommend yes once the user has looked.
  (2) Renegade's `Set_Render_Target(TextureClass *)` kept without evidence; recommend checking
  for an app-region caller before removing. (3) `Get_Reduction` has Renegade's body, not
  byte-exact; recommend trying Zero Hour's (needs `WW3D::Is_Large_Texture_Extra_Reduction_Enabled`).
* Next: wwshade. Run `python tools/vc6/vc6.py discover`, then
  `python tools/vc6/importer.py verify --lib wwshade` and work through its compile errors
  (`Apply_Default_State`, `Get_Ambient`, two vertex streams, `Get_Max_Lights`, self-shadowing),
  each checked against the client before adding. Then the texture near-misses.
* Outside git: `.work/d9_*.py` (patch builders: `d9_lines.py` Editor, `d9_addpatch.py`,
  `d9_rmpatch.py` undo, `d9_verify.sh`, `d9_cmp.py`/`d9_addr.py` compare), baseline
  `.work/d9_base_imports.json` (pre-D9 state). VMs 192.168.193.108, .202.231, .207.76.

### 2026-10-23: importer replay; decisions D6-D11

* `importer.py replay` rebuilds every library in `.work/replay` from the EA copies plus the
  decisions in `docs/data/imports.json` (header origins, source copies, splices, then PATCHES,
  annotations, APPENDS, NEW_FILES, RECONSTRUCTED), and compares the result with the repository
  file by file. It now reproduces the git tree exactly: 7 libraries, 0 files differ.
* The first run found records that disagreed with the tree; the records were fixed, not the tree
  (no source file changed, so no compiled byte changed): `lightenvironment` kept a stale
  `headers_from: Z` after f8cad49 returned its header to Renegade's; seven header origins swapped
  by refine had never been written to the `headers` map; `predlod.h` (Z) and `streak.h` (G) were
  recorded under the wrong copy (they differ only in the banner line); `INI.H`'s spelling.
* The `headers` map is now the one authority for header origins; `trial.py` and refine's header
  swaps keep it and `headers_from` in step. Replay is part of the fix loop
  ([workflow.md](workflow.md) §2 step 5).
* The user answered the overnight report's questions: D6-D11 in §2 (start order auxdata ->
  wwudp -> task kernel -> packets; byte-exact everywhere; no hybrid client for now; move the
  texture system to Zero Hour's; unknowns keep neutral names; STL corpus grown on demand).
* Next session: D9, WW3D2's texture classes and `DX8Wrapper` from Zero Hour (§4 step 1). Start
  with `python tools/vc6/vc6.py discover` if the VMs restarted; `.work/bm2` holds the evidence.
  Keep `importer.py replay` at 0 differences before each commit.

### 2026-10-23 (overnight): layouts, vtables, symbol map, app survey

Unattended session, 01:20-04:40, plan agreed beforehand; report: [overnight-report.md](archive/overnight-report.md).

* Byte-exact client functions in the tree 3,121 -> 3,408 (WW3D2 1,862 -> 2,060, WWAudio 184 -> 264,
  WWLib 553 -> 557, wwshade 8 -> 13). Every fix with its evidence: [library-matching.md](library-matching.md)
  §3.6.1. One loss accepted and documented (`meshmatdesc.cpp`, 2 functions, an inlining flip).
* New tools: `tools/re/vtables.py` (client vtables aligned slot by slot; note VC6 orders overloaded
  virtuals in reverse declaration order), `symbols.py` (docs/data/symbols.json, a Ghidra script,
  [coverage.md](coverage.md): 6,100 symbols), `classify_gaps.py`, `app_inventory.py`
  ([app-inventory.md](app-inventory.md)), `tools/vc6/trial.py`, `trial_all.py`, `layout.py`,
  `sdk_patch.py`; three build VMs used in parallel, addressed by IP.
* `importer.py refine` fixed: whole-file and header swaps had been accepted on a net gain; now no
  previously matched function may be lost.
* E&B's own code surveyed; proposal for where to start: [app-plan.md](app-plan.md) (decisions D6-D8).
* Found: E&B's `DX8Wrapper` and texture classes follow Zero Hour's (vertex/pixel shader state, Z
  textures): Zero Hour's `dx8wrapper.obj` matches 12 client functions the tree lacks. Moving the
  texture system to Zero Hour's is the largest remaining WW3D2 step and unlocks wwshade.
* Found: `importer.py assemble` restarts from the base copies (it is the pipeline's first stage),
  so the tree cannot yet be rebuilt from the EA sources and the recorded decisions. The tree in
  git is the authoritative result until a replay step exists.

### 2026-10-23: gaps (step 4), assembly comments

* Gap locator (`tools/re/gaps.py`) and layout-difference finder (`tools/re/layoutdiff.py`,
  `tools/vc6/layout.py`). Three header fixes found from member offsets
  ([library-matching.md](library-matching.md) §3.6.1): WW3D2 in tree 1,354 -> 1,681.
* Decided (user): inline assembly stays wherever the client has it (an earlier "no assembly" rule
  was withdrawn), and every assembly block gets a comment with its C equivalent
  ([conventions.md](conventions.md), "Inline assembly"). 37 comments inserted into imported files
  by `tools/vc6/annotations.py` (re-applied on import; verified: no bytes changed), and the
  RC4Class blocks commented by hand.

### 2026-10-22: import (step 2) and RC4Class (step 3)

* Libraries imported into the repository by `tools/vc6/importer.py` ([imports.md](imports.md)):
  file by file from the best-matching EA copy, whole-file swaps and single-function splices from
  the other copies where they add byte-exact functions and lose none (checked tree-wide when a
  header changes). The repository now reproduces more client functions than any single EA copy:
  WW3D2 1,354, WWLib 553, WWMath 367, WWAudio 182, WWSaveLoad 80, WWDebug 67; wwshade only 6 of
  46 (needs Zero Hour's WW3D2 interfaces).
* `RC4Class` reconstructed from the client, all five functions byte-exact; the importer keeps
  reconstructed files (`RECONSTRUCTED`).
* Out of scope, proven: the VC6 SP4 C runtime, C++ library and COM support, D3DX 8.0; linker
  padding and compiler EH stubs. E&B's own code was compiled with `/GX`
  ([library-matching.md](library-matching.md) §3.7).

### 2026-10-22: byte matching

* `tools/vc6/vc6.py` compiles in the VM; `tools/re/objmatch.py`, `objdiff.py`, `matchreport.py`
  match compiled functions against the client.
* The client's libraries were compiled with `/MT /O2 /Ob2`, no `/GX`, no `/G6`: Renegade's
  release flags.
* **2,708 client functions proven byte-identical** to EA source (WW3D2 1,502, WWLib 553, WWMath
  369, WWAudio 175, WWSaveLoad 80, WWDebug 66, wwshade 46): [library-matching.md](library-matching.md) §3.5.
  First pass compared Renegade and Zero Hour only (2,650) and compiled Zero Hour against Renegade's
  `wwdebug` headers because Zero Hour's `wwdebug.h` includes its game engine's `debug.h`, which the
  wrapper didn't copy to the VM. The user pointed out the Generals and Zero Hour trees are both in
  the clone; rerun with all three copies, each against its own libraries (`bytematch_all.sh`).
* Correction: WW3D2 is mostly **Renegade's** core with some later features that Zero Hour also
  has, not Zero Hour's (the string census only saw the new features). WWMath and WWAudio are
  Renegade's; WWSaveLoad and wwshade Zero Hour's; WWLib is mixed per file.
* `RC4Class` is E&B's own variant with inline assembly (§3.6).

### 2026-10-22: VC6 build VM

* VM up (Windows 10, SSH with key `~/.ssh/enb_vc6`), base VC6 then SP5 installed and tested
  ([toolchain.md](toolchain.md) §1.1). Both compile, link and run a test program.
* Neither is the client's compiler: RTM stamps build 8168, SP5 stamps 8966, the client has 8799.
* **SP4 is the client's compiler**: reverted to the RTM checkpoint, installed SP4, and it stamps
  8799 like the client, with the same CRT build mix ([toolchain.md](toolchain.md) §1.2).

### 2026-10-22: decisions D1–D5; SDKs checked

* D1–D5 settled (§2).
* DirectX 8 and RAD SDKs cloned to `C:\EnBDevelopment\SDK` and checked against the client
  ([toolchain.md](toolchain.md)): the client used DirectX **8.0** (`Direct3DCreate8(120)`), as the
  SDK; Bink's 9 imports all match; Miles' header is right but its import library is older than the
  game's `Mss32.dll` (30 of 81 imports missing), so we generate the import library from the DLL.

### 2026-10-22: project started

* Repository created at `C:\EnBDevelopment\EnB`, laid out after the original tree
  ([source-tree.md](source-tree.md)).
* EA's Renegade and Generals/Zero Hour repositories cloned to `C:\EnBDevelopment\EA`.
* Facts about the binary recorded ([binary.md](binary.md)): VC6 SP4 (build 8799, toolchain.md), incremental link, built
  2004-03-16, PDB `G\app\main\client_release\main.pdb`.
* The original source tree recovered from `__FILE__` strings: 472 paths
  ([source-tree.md](source-tree.md), [data/source-paths.txt](data/source-paths.txt)).
* Coding conventions written from the client's own strings and EA's Westwood code
  ([conventions.md](conventions.md)).
* First library matching ([library-matching.md](library-matching.md)): WW3D2, WWLib, WWDebug and
  wwshade follow **Zero Hour**; WWAudio follows **Renegade**; wwphys, wwnet and wwui are not
  in the client. 300 client functions matched by strings (about 170 that are not duplicates).
  Zero Hour's WW3D2 contains E&B code (`..\data\client\mixfiles\`, `global.th6`).
* WWLib did drift between the titles (about one file in five has code changes); the client's
  `rc4.cpp` is Zero Hour's.
* Tools in `tools/re` ([tools/re/README.md](../tools/re/README.md)).

## 2. Decisions

| # | Date | Decision | Why |
|---|---|---|---|
| A1 | 2026-10-22 | The repository root stands for `G\`; directories as in the original, lower case | the original layout is known; mirroring it keeps include paths (`..\..\wwlib\ref_ptr.h`) meaningful |
| A2 | 2026-10-22 | Names come from the client first (strings, RTTI, asserts), then from Westwood convention | the client is the only source of E&B's own names |
| A3 | 2026-10-22 | EA clones live outside the repository, at `C:\EnBDevelopment\EA`; SDKs at `C:\EnBDevelopment\SDK` | they are references; what we import is copied in with its origin recorded |
| D1 | 2026-10-22 | The repository is GPL v3 (EA's code, with EA's additional terms) | user |
| D2 | 2026-10-22 | Install Visual C++ 6.0 as a tool for proving our work (byte-for-byte comparison) | user. Installed: VC6 SP4 in Hyper-V VMs (three, 2026-10-23; since 2026-10-24 Win10 plus the pool, D30), the client's exact compiler build ([toolchain.md](toolchain.md) §1) |
| D3 | 2026-10-22 | The code builds with both VC6 and modern MSVC | user |
| D4 | 2026-10-22 | Import EA code file by file, as each file is verified against the client | user |
| D5 | 2026-10-22 | SDKs: DirectX 8 from github.com/edgeforce/directx8, Miles and Bink from github.com/edgeforce/radtools | user. Checked: DX8 is 8.0 as the client; Bink matches; Miles usable with two fixes ([toolchain.md](toolchain.md) §2) |
| D6 | 2026-10-23 | E&B's own code starts with `app\shared\auxdata`, then `app\wwudp`, then the task kernel (`app\kernel`: `KernelClass::Run_One_Cycle`, the loop running the current, background and pending-delete tasks; with `app\main` and `app\client\tasks`), then `app\packets` ([app-plan.md](app-plan.md) §3) | user |
| D7 | 2026-10-23 | E&B's own code is held to byte-exactness everywhere, like the libraries. Check in with the user periodically if this becomes too costly | user |
| D8 | 2026-10-23 | No hybrid client (reconstructed functions patched into the retail one) for now: its results would need the user to validate them. Open to it again if it can be shown to verify results better than byte matching, without the user | user |
| D9 | 2026-10-23 | E&B follows Zero Hour's KM/KJM (shader system) line through 08/01/02, without the 08/05/02+ texture redesign and without the Generals team's MW/GTH changes. So `TextureFilterClass` (08/05/02 KM) is dropped: `TextureClass` has Renegade's five filter members (texturefilter.cpp/.h `NOT_LINKED`) | user; overnight finding: Zero Hour's `dx8wrapper.obj` matches 12 client functions the tree lacks, and it unlocks wwshade. Reworded by the user 2026-10-23 after step 1 showed the client has the 06/27/02 abstraction but not the 08/05/02 redesign (library-matching.md §3.6.1), and again 2026-10-24 after the dated-comment sweep (`.work/night/item6i/report.md`: bytes place every dated ZH edit; MW 0/3, GTH 10/12 absent); filter members folded in by the user 2026-10-24 (all four constructors exact, 8f10486) |
| D10 | 2026-10-23 | E&B's unknown additions keep neutral names (`m_UnknownXX`, evidence in a comment) until the code that uses them is rebuilt. Since D65 (2026-10-25) such a name is NAME 1 on D56's scale: it is replaced whenever better evidence appears, the tag recording it | user |
| D11 | 2026-10-23 | Compiler-generated code (STL template instances): extend the corpus as each app file needs it, not ahead of the app work | user |
| D12 | 2026-10-23 | Compile each library with the flags E&B used, wherever byte evidence shows them (wwshade: `/GX /Ob1`; importer `LIB_FLAGS`, `bytematch_all.sh` `LIBFLAGS`) | user: "match how E&B built things whenever you have evidence to support it" |
| D13 | 2026-10-23 | EA files the client does not link are dropped from the tree (importer `NOT_LINKED`, with the evidence): first `shdlegacyw3d.cpp`/`.h`; then WWAudio's `AudioSaveLoad.cpp`/`.h` (2026-10-23) | user |
| D14 | 2026-10-23 | `ENABLE_WWPROFILE` in WWDebug too, although its bytes are neutral (verify: 67 before and after, none lost): consistent with a project-wide define. Soft: drop it if it ever costs a match | user |
| D15 | 2026-10-23 | `G_CODE_BASE` in WWAudio stays an importer patch removing its two `#ifdef`s, not a `/D` in `LIB_FLAGS` (same bytes) | user: only `WWAudio.cpp` uses it, so the bytes can't show a project-wide define |
| D16 | 2026-10-23 | E&B members whose use is now rebuilt (`m_Unknown148`, `m_Unknown164/168/16C`, `m_Unknown19A5/19A6`, ...) keep their D10 names, with the comments in the headers, until the app code shows E&B's names (D65: tracked as NAME 1 under D56) | user |
| D17 | 2026-10-23 | The two wwshade `"UnNamed"` constructors are left as near-misses; revisit only on new evidence | user |
| D19 | 2026-10-23 | E&B's screen-distortion module in WW3D2 (0x995D20-0x998200, between dx8texman.cpp and boxrobj.cpp; first described as a cube-map list) is rebuilt as `ww3d2/unknown_distortion.cpp`/`.h` (a reconstructed file), neutral names (D10) until E&B's own turn up; the function `WW3D::Set_Render_Device` calls is `Unknown_Reset_Distortion_List` | user (name chosen after the module turned out not to be about cube maps) |
| D18 | 2026-10-23 | Don't include `matrix4.h` first in `ww3d2/meshmatdesc.cpp` (it would win back `Make_UV_Array_Unique` 0x947310, but no EA copy has that order). Keep it on record until a better fix is found; revisit if it becomes WW3D2's last outstanding item | user; kept 2026-10-24 after item 4 showed the cause is symbol-count noise, not an inlining flip |
| D20 | 2026-10-24 | Losing a COMDAT-duplicate `(file, address)` pair, where the helper stays matched in another file, is not a loss. Name each such pair in the commit message | user (03:40) |
| D21 | 2026-10-24 | `dx8webbrowser.cpp`'s type library: `BrowserEngine.tlh`/`.tli` generated by VC6 from E&B's own `BrowserEngine.dll` are committed as reconstructed files, `#include` instead of `#import` (original kept as a comment); no binaries in the repository (option A) | user (04:40); done 761d6ac |
| D22 | 2026-10-24 | Import Zero Hour's shd6/shd7/shd8 bump shaders into wwshade, with the shader headers rebuilt from the client's data (0xBCB8FC-0xBCEBB7) as reconstructed files, in `shdpp.exe`'s format: `DWORD` arrays holding the shader text, NUL, zero padding, 0 terminator (PS; `DWORD` approved by the user 2026-10-24, since `ShdHW*Shader::Create(DWORD*)` takes them and `char[]` would pick `Create(char *file_name)`) | user (06:25); done 2026-10-24 (e8f39a8, 0de15b9: headers as shdpp's DWORD arrays, since `Create(DWORD*)` takes them) |
| D23 | 2026-10-24 | E&B's own files enter the importer as a non-EA library `app` (all reconstructed, `/GX`, `NDEBUG` on since D61 (first written as off), verify limited to the app region) | user (06:25); done 249c365 (importer `APP_LIBS`; sources compiled as `e:\build\beta\G\...` for the client's `__FILE__`) |
| D24 | 2026-10-24 | App classes the client doesn't name get Westwood `XxxClass` names (`BitSetClass`) | user (06:25); conventions.md §1 |
| D25 | 2026-10-24 | App asserts sit on their real `__LINE__` through the file layout (blank lines, comments) in tree files; `#line` only in drafts | user (06:38) |
| D26 | 2026-10-24 | `USE_WWSHADE`: WW3D2 built with it (under D12, evidence: the `SHD_*` wrapper calls); wwshade too, as a soft consistency setting like D14 (neutral in the bytes) | user (03:47, 06:25); done 4ae710d, 7ecb645 |
| D27 | 2026-10-24 | Verified function starts (decoded call targets) are the default for every tool | user (03:47); done 78b0a74 |
| D28 | 2026-10-24 | Keep ww3d.h's unused Renegade `MinStaticSortLevel`/`MaxStaticSortLevel` declarations (noise compensation for meshmatdesc after fdd0a81, not evidence); revisit when the noise is understood | user (06:25) |
| D29 | 2026-10-24 | Keep 62b895b's protected `D3DTexture` (needed by c35cc34, which writes it as 0x94B660 does); patch reasons reworded | user (06:38); done e88f4e9 |
| D30 | 2026-10-24 | Build VMs come from a pool of differencing clones of `Tools\VMPool\base-vc6.vhdx`, no warm sync cache (each leasee syncs its tree), cap 5; the old fixed clones are retired | user (04:40, 06:25); done 87f38c3, c7b8bcc; clones retired 07:00 ([toolchain.md](toolchain.md) §1.3) |
| D31 | 2026-10-24 | Sessions release their leased VMs in the handoff skill; `pool.py lease`/`create` reclaim orphaned leases first | user (06:25); done 6e298b5 |
| D32 | 2026-10-24 | ~~Builders stay serial (one at a time); parallel worktrees not needed yet~~ Superseded by D62 (2026-10-25) | user |
| D33 | 2026-10-24 | Keep the neutral names from the night's item 7b: `MEM_CLIENTOBJECT`..`MEM_PARTICLES` (from the client's name table 0xBCB228), `FastAllocatorGeneral::m_UnknownE00`..`m_UnknownE20`, `Unknown_Reset_Statistics` (D10) | user |
| D34 | 2026-10-24 | Keep evidence-backed changes that gain nothing countable when the client's form is shown: first `DazzlePrototypeClass`'s Renegade virtual destructor (0x96EF00 has the client's form but is too short to count; vtable 0xB12598 has 4 slots) | user |
| D35 | 2026-10-24 | texture.h's comment on the protected `D3DTexture` cites 0x94B660 (c35cc34), not `Apply` 0x94BC80 (D29) | user |
| D36 | 2026-10-24 | VIDTEST.EXE is rebuilt as a working program, not byte-exact: proof that the tree links into a small app that loads E&B's original art and renders a 3D scene (the 2002 library variant and incremental link aren't needed for that). E&BConfig.exe isn't rebuilt (reworded 2026-10-25 with D63: the original only launches our vidtest) | user |
| D37 | 2026-10-24 | App class names from the auxdata work: `DeltaClass`, `DeltaCompoundClass`, `DeltaTemplateClass`, `NameClass`, `LookupClass`, `LinearBufferClass`, `MoneyClass`, `DurationClass`; `PermString` keeps the client's own name (its debug message at 0x4ADFF0), without the suffix; the header of the assert functions is `app/shared/unknown_debug.h` (`Unknown_Assert_Failed`, `Unknown_Fail_If_Report`, D10) | user (accepted builders' proposals) |
| D38 | 2026-10-24 | Names chosen from behaviour stand: `Is_Parent_Of`, `Is_Parent_Or_Same`, `Match_Parts` (name.cpp), `Unknown_Set_Size` (linearbu.cpp), `StringValueClass`, `Add_String` (delta_string.cpp, delta.h), `PermStringInitClass` (permstr.h), `Unknown_Debug_Printf` 0x509DC0 (dbgmgr.cpp) | user |
| D39 | 2026-10-24 | The time and money field instances `DeltaTemplateClass<DurationClass>` / `<MoneyClass>` are explicit specialisations (`template<>` constructor, Read, Write) in delta_time.cpp / delta_money.cpp, declared `extern template` where the roots use them (aux_roots.h for money). Reworded 2026-10-25 to the form the tree has: the first wording (`template class ...` instantiations next to the header's `extern template`) doesn't compile in VC6 (C2949, C2909) | user |
| D40 | 2026-10-24 | `DeltaCompoundClass::Add` 0x4A5670 (D11): check the client's other `vector<T*>::insert` copies for the out-of-line `size()` 0x4A58C0 before changing the STL corpus | user |
| D41 | 2026-10-24 | Header `__FILE__` strings in app code: leave them for now (ours include `..\duration.h`; the string differs, bytes and lines match through the relocation); run app compiles in `e:\build\beta\G\app\<dir>` with `/I ..` and the client's include spellings when strings are compared (whole-image check) or the project directory's name is known | user |
| D42 | 2026-10-24 | Milestones M1 (capture harness on the rebuilt auxdata and wwudp classes) and M2 (working VIDTEST on the tree's libraries) drive the work until both are met; then back to the kernel ([milestones.md](milestones.md)). Functional code on their path that isn't byte-exact yet is allowed, marked `// NOT EXACT:` (P1) | user |
| D43 | 2026-10-24 | The 11 AuxData root constructors (first written as 13) are generated from the extracted schemas in the client's code form, then verified like any app file (P2) | user |
| D44 | 2026-10-24 | Missing functions at link time are stubbed in `stubs/` only, never in library files; each stub logs its first call (P3) | user |
| D45 | 2026-10-24 | New programs: `tests/capharness/`, `tools/harness/` (Python side), `programs/vidtest/` for VIDTEST's own code (WinMain, tests, `results.ini`: no source path survives in VIDTEST.EXE). VIDTEST's E&B scene classes follow the client's layout (`app/shared/stars.cpp`, `spline.cpp`, ...): they share the client's strings, including `\\.\mailslot\G\...` (P4) | user |
| D46 | 2026-10-24 | VIDTEST's E&B classes (Land, stars, decorations, splines) are rebuilt from the client's 2004 versions where the client has them, byte-exact there; VIDTEST's 2002 code only where the client lacks it (P5) | user |
| D47 | 2026-10-24 | M1's transport gate uses the EA captures; the Net-7 TCP recordings are tested at message level unless their transport is in `app\wwudp` (P6) | user |
| D48 | 2026-10-24 | Build on pool VMs; run vidtest on the host (the VMs have no 3D hardware). Running the tree's own programs unattended is allowed, as E&B's own code is (P7) | user |
| D49 | 2026-10-24 | L2: the 12 EA files never imported (wwlib TARGA.CPP, int.cpp, blwstraw.cpp, blowfish.cpp, stimer.cpp, slnode.cpp, win.cpp; wwsaveload definitionfactory.cpp; wwaudio soundstreamhandle.cpp, Utils.cpp; wwshade shddeffactory.cpp, shdforcelinks.cpp) are imported as they are, each checked with a verify ([milestones.md](milestones.md) §3.1) | user |
| D50 | 2026-10-24 | L2's link line adds vfw32, comsupp and ddraw | user |
| D51 | 2026-10-24 | Miles: `mss32.lib` is generated from the game's own `Mss32.dll` exports by `tools/vc6/mkmss32.py`: a generated `.def` linked into a dummy DLL, keeping LINK's import library (VC6's `lib /def` drops the stdcall names' leading underscore; reworded 2026-10-24, user), not stubbed and not the SDK's older lib | user |
| D52 | 2026-10-24 | M1's AUX_DATA totals for the Alex sessions come from `tools/harness/convert.py`'s datagram files (4,289 / 366), the 4 messages the reference decoder lost reported as "not in the reference decoder" | user |
| D53 | 2026-10-24 | VIDTEST O7: our vidtest fixes the signed `FreeTextureRam` comparison (GPUs reporting 2 GB or more), rather than copying the original's `TextureReduction=2` | user |
| D54 | 2026-10-24 | A function whose compiled bytes occur at several places in client.exe, all identical copies (objmatch `multiple`, e.g. template leaf Reads), counts as "exact, ambiguous address" in a separate column beside each library's pair count; the `(file, address)` pair counts and the before/after pair rule are unchanged | user |
| D55 | 2026-10-24 | `ww3d2/pot.cpp` is NOT_LINKED; wwmath's identical `pot.cpp` supplies `Find_POT` / `Find_POT_Log2` (both copies matched the same two client functions, and a program linking both fails on the duplicate). Its 2 ww3d2 pairs are dropped as D20 duplicates | user |
| D56 | 2026-10-24 | Name certainty: every name chosen without the client's own evidence carries a tag comment where it is declared, `// NAME n: evidence`, scored 5 = the client names it (RTTI, assert, `__FILE__`, `Class::Method` string), 4 = EA's Westwood source names it, or a Net-7 / the reference decoder name confirmed by matching structure, 3 = built by convention from a client-named file or sibling (D24 class-from-file, a `.h` paired with a known `.cpp`), 2 = describes what the code does, 1 = neutral placeholder (`Unknown_xx`, D10). 4-5 are settled; 3 and below are listed for review by `tools/re/names.py` in the generated `docs/data/names-review.md` (untagged `Unknown*` names count as 1). When better evidence appears the tag is raised and the name leaves the list ([conventions.md](conventions.md) §1) | user |
| D57 | 2026-10-24 | M1.2's names: `encode.h` (NAME 2) and `strconv_lib.h` (NAME 3) stay, tracked under D56; `KVPairClass`'s virtuals keep descriptive names (`Clone`, `Get_Type_Name`, `Get_Value_String`, ...; NAME 2) rather than offset names. The file for the compound/root functions 0x4A3990-0x4A4780 and 0x4B0860 waits for M1.3, using Net-7's `AuxClasses` and the reference decoder's `the reference game model` names as structure-matched hints | user |
| D58 | 2026-10-24 | Provisional file names stay, tracked under D56: `app/shared/auxdata/unknown_root.*` (the compound/root object, no `__FILE__`), `app/client/aux_roots.*` (the generated roots; the client's code is in co.cpp and an unattributed object at 0x751BE0: the generator writes into co.cpp once that is rebuilt), `app/shared/heightfield.*`, `app/shared/land.*` | user |
| D59 | 2026-10-24 | AuxData class names: nested compounds `Delta<Net-7 name without Aux>Class` (NAME 4, matched child by child against Net-7's `AuxClasses` and `the reference game model`, `tools/re/auxroots_names.json`), roots `Delta<client string>Class` (NAME 3); `DeltaInterpolatorClass` keeps its name (Net-7's AuxPercent / C# Ramp; the client interpolates) | user |
| D60 | 2026-10-24 | M1 gate (b) is met with one documented exception: capture_1 #24128 (a Hulk, header byte 0x55 under a parent marking that child cleared) decodes but can't be re-encoded by the client's encoder; the harness doesn't keep header bytes as read | user |
| D61 | 2026-10-24 | The app library compiles with NDEBUG (in APP_LIBS' defines; G_ASSERT / FAIL_IF don't depend on it), confirmed by a verify that changes no pair; files stop defining it themselves | user |
| D62 | 2026-10-25 | Builders may run in parallel on disjoint items (replaces D32), under the shared-tree rule: never compile the other builder's changes into a verify (check `git diff` of shared files: importer.py, app headers, stubs, docs/data), commit only your own hunks, keep the uncommitted window short, never wait for a clean tree (that deadlocked both builders for an hour on 2026-10-24). The importer lock serialises `verify` ([builders.md](builders.md)) | user (approved for the 2026-10-24 unsupervised run; recorded 2026-10-25) |
| D63 | 2026-10-25 | M2 gate (c)'s repeatable check is `tools/harness/vidtest_e2e.py` (E&BConfig's command line, results.ini). A supervised run of the original E&BConfig with the user, registry backed up first, is a one-time acceptance check, not a gate. D36 reworded: E&BConfig.exe isn't rebuilt. **Passed 2026-10-25** (log entry of that evening): every displayed value as our results.ini, registry unchanged | user |
| D64 | 2026-10-25 | `// NOT EXACT:` is a general interim marker for app code written functionally from pseudocode, not only on the M1/M2 path (D42); D7 stays the target. `tools/re/names.py` lists every marked function in the generated `docs/data/not-exact.md` | user |
| D65 | 2026-10-25 | D10/D16's neutral names are NAME 1 on D56's scale, replaced whenever better evidence appears (conventions.md §1 reworded) | user |
| D66 | 2026-10-25 | Pair losses. Not losses, named in the commit message: (i) a COMDAT duplicate kept matched in another file (D20); (ii) the same source compiled into two files (D55); (iii) a false match exposed by a type fix (first: 0x8C1520, `KVPairTemplateClass<__int64>`'s string setter, matched by the money instance only while `MoneyClass` was a bare `__int64`). Anything else is a loss and waits for the user before the commit. D54's ambiguous column stays as is: the 17 identical template leaf Reads are not pairs | user |
| D67 | 2026-10-25 | Test programs live under `tests/` (D45 extended: kerneltest and movietest in tests/capharness). The wlib stand-ins (app/common/dictionary.h, critsec.h, app/wwudp/wtime.h, parts of cordcon.h) stay until D73's import replaces them with the imported files at the client's paths (`libs\wwonline\wlib`), then are deleted | user |
| D68 | 2026-10-25 | [kernel-map.md](kernel-map.md)'s 10-step reconstruction order is adopted (extends D6); steps 1-5 and 7 done or partly done on 2026-10-24 | user (backfilled) |
| D69 | 2026-10-25 | Safety rules for every run of our programs and every session (amends D48): never a modal dialog (`-nomsgbox` or its equivalent in every program); full screen allowed; no screen grabs, images only from the program's own back buffer; never the game's install folder as working directory; no registry changes (the original E&BConfig only in a supervised run with a registry backup); never push; never `importer.py assemble` on a tree with work; `git add` by name, `.claude/settings.json` stays out | user (each given on 2026-10-24; numbered 2026-10-25) |
| D70 | 2026-10-25 | Inline assembly stays wherever the client has it, with a C-equivalent comment | user (2026-10-23; backfilled) |
| D71 | 2026-10-25 | A commit's `Co-Authored-By` line names the model that wrote the commit (the line that session's harness gives); the docs carry no literal model name | user |
| D72 | 2026-10-25 | Subagent models: the coordinator runs Fable; builders (verify loops, byte matching, RE, programs) run Opus by default; Fable for a subagent only when the expected total cost is lower: a small-input reasoning puzzle (inline budget, codegen form) after two failed Opus attempts, or a one-shot analysis whose wrong answer would cost a builder run; Sonnet for read-only audits, cross-referencing, doc moves, log digging and generator reruns (anything a tool checks afterwards; Explore agents too); Haiku for grep-level searches and log triage, never tree code. Plan logs record model and tokens per builder ([builders.md](builders.md) §4) | user |
| D73 | 2026-10-25 | Import WWOnline's wlib and wnet (Zero Hour's matchbot), Renegade's `Code\Tools\RenegadeGR` tcpmgr/tcpcon and Renegade's BinkMovie, under D4 (file by file, each verified). `CordArrayClass`'s remove is rebuilt on the imported wlib and its stand-in deleted (D67). Unblocks kernel-map steps 8-9 | user |
| D74 | 2026-10-25 | `ENABLE_WWPROFILE` goes into the app library's defines, kept if a verify changes no pair (D14/D26 footing; evidence since D14: WWAudio +32, WWMath +1, WW3D2 +20 pairs) | user |
| D75 | 2026-10-25 | The assert functions' reporter 0x4BC130 (the logger) is a reconstructed stand-in with D10 names and D56 tags | user |
| D76 | 2026-10-25 | `CordMgr::Connect` keeps the client's signature; the harness reads the session id another way, unless the client's bytes return it | user |
| D77 | 2026-10-25 | funcmatch's string evidence requires the client function's `__FILE__` to match the EA function's file; symbols.json regenerated (0x829C80 is tcpconn.cpp's constructor by accepted socket, not `PacketManagerClass::Clear_Socket_Error`) | user |
| D78 | 2026-10-25 | The d3d8 draw dump (`tools/harness/run_reference.py`, `drawdump.py`) is M2's standard regression check, beside the frame comparison | user |
| D79 | 2026-10-25 | Net-7 difference 4 (fields named in two sets) is a one-line note in the reference decoder's `docs/research/auxdata.md` §7 for that project's coordinator. the reference decoder and a private tool need their own audit and re-architecture later, outside this project's scope; their design docs are now likely less complete than this project's for transport and auxdata. The user revisits this before housekeeping closes | user Settled 2026-10-25: the audit and first housekeeping were done (the proxy is now the reference decoder, `C:\EnBDevelopment\the reference decoder`); the rest is that project's. |

## 3. Open decisions

* D7 is to be revisited with the user if byte-exactness of E&B's code becomes too costly; D64's
  `docs/data/not-exact.md` shows how much functional code is waiting.
* Which bool-return form gives a bare `setcc al` without the `xor` (moved from workflow.md §3).

## 4. Next steps

Milestones M1 and M2 are met (D42, D63; [milestones.md](milestones.md)). Order and detail of the
client's own code: [kernel-map.md](kernel-map.md) §6. Adding a file: [workflow.md](workflow.md) §2.1.

1. Left over from finished work: the shaders' Init, destructors, Copy_Vertex_Stream and Peek_Texture
   are byte-exact but `multiple` (verify cannot credit them; D54 column). The app library's debug
   functions 0x4E9410/0x4E9450 (they call a reporter on an object at 0xBDA908; D75) and E&B's
   operator new/delete 0x4E9CB0/0x4E9D10 are open. delta.cpp's `Add` (D11/D40) and the twin
   constructors 0x4A7080/0x4A8150 (uncounted) remain; so do the name tests 0x4ACA90/0x4ACB20 and
   kv_encoder_base's constructor 0x4B3850.
2. The D73 imports (wlib/wnet, tcpmgr/tcpcon, BinkMovie) unblock kernel-map steps 6 and 8-9
   (tcpconn.cpp, connect.cpp's factories, the game and login tasks).
3. Ship 0x6F66E0: VC6's inline budget (ours inlines the Buff array 0x707040, which the client calls);
   see the M1.3 leftovers in library-matching.md 3.8.
4. The packet classes' own code 0x85C5F0-0x8B1EB0 (kernel-map step 5 and after).
5. Tools and defines (D74-D78): ENABLE_WWPROFILE in the app defines (D74), the funcmatch
   `__FILE__` rule (D77) and the draw-dump regression check (D78) are decided; carry them out where
   not yet done. `--pool K` exists for `importer.py verify` only (c7b8bcc); `bytematch_all.sh` and
   `importer.py refine` still lack it.
6. `WW3D::Make_Screen_Shot` 0x9182C0: Zero Hour's gamma/format plus E&B's JPG case through a wwlib
   JPEG writer (ctor 0x9E3850, dtor 0x9E3860, save 0x9E3870; class after srandom.cpp); 0.85, only
   register scheduling left (`.work/night/item7b/v_mss.py`, `run_mss.py`; untracked, historical).
7. D19's distortion draw 0x9961E0 (369 masked rows): the client builds 6 `??_H` Transpose temps
   with out-of-line `Vector4()`, ours 5; look for missing inline calls/temporaries earlier in the
   function (fits the 0x18-bigger frame).
8. `PointGroupClass::Render` 0x954960 (slot shuffle in inlined Transpose temps); the meshmatdesc
   noise mechanism (then drop D28's declarations and revisit D18).
   Other WW3D2 leads: `Process_Foreground_Load` 0x949C60 (inline budget), `Compute_Caps` 0x9B0160,
   `Update_Movie_Capture` 0x918B20, `Create_Device`'s format retry, `CreateBrowser`'s app callers
   0x7CA5C0/0x7CAD00 (E&B's `querydata`). wwshade: the two `"UnNamed"` constructors (D17).
