# client.exe: facts about the binary

The target is the retail client as installed (Net-7 era install):

| | |
|---|---|
| Path | `C:\Program Files (x86)\EA GAMES\Earth & Beyond\release\client.exe` |
| SHA-256 | `249f852496f341c00361be95b66e7c0becc2c39e7cba026f9227e38b602651c4` (the same client the earlier protocol research used) |
| Size | 8,642,625 bytes (the file date is the install's, not the build's) |
| PE timestamp | 0x4057AE00 = **2004-03-17 01:46:40 UTC** (the file header; the debug directory and the PDB record carry 0x4021A831 = 2004-02-05 02:19:29 UTC, age 2: §5) |
| PDB | `e:\build\beta\G\app\main\client_release\main.pdb` (not available) |
| Image base | 0x400000 |

All addresses in these docs are virtual addresses in this image.

## 1. Toolchain

From the Rich header:

| Product id | Build | Count | Meaning |
|---|---|---|---|
| 11 (C++ compiler, VC6) | 8799 | 1054 | **Visual C++ 6.0**, build 8799 = **Service Pack 4**, confirmed by compiling ([toolchain.md](toolchain.md) §1.2): E&B's and Westwood's C++ |
| 10 (C compiler, VC6) | 8797, 8799 | 202 | VC6 C objects |
| 4 | 8447 | 96 | import-library members made by LIB 6.00.8447 (SP4's): 81 mss32 + 9 binkw32 imports + 2x3 descriptors ([toolchain.md](toolchain.md) §1.2, 2026-10-24) |
| 19 | 8022, 8034 | 35 | import libraries |
| 14 | 7299 | 45 | VC6 resource/old linker objects |
| 28, 29 | 8830 | 1, 66 | objects from a newer toolchain: likely DirectX 8 SDK or Miles/Bink static libraries |
| 6 | 1735 | 1 | CVTRES |

Linker 6.0, **incremental** (/INCREMENTAL): the app's functions are reached through a table of
5-byte `jmp` thunks at the start of `.text` (0x401005–0x41964B, about 20,000 stubs).

## 2. Sections

| Section | VA | Size |
|---|---|---|
| `.text` | 0x401000 | 0x6E58DD |
| `.rdata` | 0xAE7000 | 0x84B41 |
| `.data` | 0xB6C000 | 0xD6F14 |
| `.idata` | 0xC43000 | 0x38D9 |
| `.rsrc` | 0xC47000 | 0x1FDC |
| `.reloc` | 0xC49000 | 0x5CB2E |

**Inside `.text`:**

| Range | Contents |
|---|---|
| 0x401005–0x41964B | incremental-link thunk table |
| ~0x429630–0x8C1B50 | E&B's own objects (`G\app\...`), all reached through thunks |
| ~0x8C1B50–0xAE58DD | statically linked libraries, no thunks. From the string-anchored matches: WWMath near 0x8C8000, WW3D2 ~0x909000–0x9C5000, WWLib ~0x9C7000–0x9E9300, WWDebug ~0x9EA000–0x9EC000, WWAudio ~0x9EE000–0xA04000, WWSaveLoad 0xA049C0, wwshade ~0xA09000–0xA10000, then the CRT and the rest. Rough; see [data/funcmatch.md](data/funcmatch.md) |

| 0xA420AC–0xAA1C00 | incremental-link padding (`int3`), 382 KB |
| 0xAA1C00–0xAE2830 | compiler-generated C++ exception-handling stubs for E&B's code (259 KB) |

What of this is precompiled or generated rather than written: [library-matching.md](library-matching.md) §3.7.

Function discovery: thunk targets (app code), plus `call rel32` targets and padding after
`ret` (library code). This finds about 30,000 functions (`tools/re/clientimage.py`).

## 3. Imports

`d3d8.dll` is not imported: WW3D2's `DX8Wrapper::Init` loads `D3D8.DLL` and `Direct3DCreate8`
by name, as in Renegade and Generals. Imported DLLs:

| DLL | Used for |
|---|---|
| `mss32.dll` (81 imports) | Miles Sound System, under WWAudio |
| `binkw32.dll` | Bink video |
| `WS2_32.dll` | sockets (`app\common\tcpconn.cpp`, `udpconn.cpp`, `app\wwudp`) |
| `authlogin.dll` (`AuthLoginLib_GetComponentFactory`) | the login component (replaced by Net-7's) |
| `WININET`, `urlmon`, `ole32`, `OLEAUT32` | the in-game browser (`BrowserEngine.dll`) and HTTP |
| `DDRAW.dll` (`DirectDrawCreate`) | WWLib's `ddraw.cpp` (video card detection) |
| `AVIFIL32.dll` | WW3D2's `FrameGrabClass` (movie capture) |
| `KERNEL32`, `USER32`, `GDI32`, `ADVAPI32`, `WINMM`, `SHELL32`, `COMCTL32` | Windows |

## 4. Strings and RTTI

* 31,661 printable strings of 5+ characters.
* 472 source paths under `e:\build\beta\G\` ([source-tree.md](source-tree.md)).
* 230 distinct `Class::Method` names in strings ([data/class-methods.txt](data/class-methods.txt)).
* 87 RTTI type descriptors ([data/rtti.txt](data/rtti.txt)): about 30 of them are E&B's own
  (`ClientObjectClass`, `GadgetClass`, `SlotDataClass`, `GClass`, `CameraModuleClass`, the
  `...Event` classes), the rest the C++ library. So the client was compiled with /GR, but RTTI
  descriptors were emitted only for classes used with `dynamic_cast` or `typeid`.
* Assert expressions with their source file: [data/asserts.txt](data/asserts.txt). The file
  attribution is approximate: strings follow object order, and a file's first assert expression
  can come just before its `__FILE__` string, so it gets attributed to the previous file.

## 5. The link's layout: a full incremental link, then one incremental update

Measured 2026-11-25 (image builder, week plan 2026-11-24 §2 D question 1) with `tools/re/linklayout.py
--objects` on retail (the figures below are its output or sums over its tables in `.work/linklayout/`). Answer: retail's `.text` is
**a full incremental link** (2004-02-05, the PDB's signature) **updated once by an incremental link**
(2004-03-17, the file header's stamp; NB10 age 2) after a handful of objects were recompiled. Nearly everything
sits where the full link put it; the update's marks are local.

**The stamps.** File header 0x4057AE00 = 2004-03-17 01:46:40 UTC. The one debug directory entry (type 2,
CodeView) carries 0x4021A831 = 2004-02-05 02:19:29 UTC and points at a 65-byte NB10 record with the same
signature, **age 2**, `e:\build\beta\G\app\main\client_release\main.pdb`. A clean incremental link of ours gives
age 1 and one stamp everywhere: the update rewrote the file header and the PDB (age +1) but not the debug entry's
stamp or the PDB signature.

**The full link's rules, as the image shows them.**

| Rule | Figure |
|---|---|
| App objects (the `.obj` files on the command line: every thunk target, 0x429630-0x8C1B64) get a slot of **up16(S + S/4)** each, S the contribution's size (the COMDAT section's, compiler nop fill included), int3 after | 19,865 of 19,934 slots (99.65 %); the 69 others are 20 hosts of guests and 48 slots short or long, all at the update's places below, and the last app slot (0x8C1B50, at the library boundary) |
| Library members (0x8C1B64-0xA420AC) are packed, alignment only, with no thunks | 8,029 of 8,030 contributions with under 16 bytes of int3 after them |
| One reserve after the library code | 392,020 int3 at 0xA420AC = 24.90 % of the 1,574,216 library bytes (our trial link of libraries only, `client-inc`: 1,258,622 = 25.08 % of 5,018,050) |
| The thunk table at the start of `.text` | 5 int3, then 19,983 contiguous `jmp rel32` 0x401005-0x419650 (18,691 targets), then 65,504 int3 to the first app slot 0x429630 |
| Thunk order: LINK's symbol hash table, not target, name or object (rule below) | steps to a higher target 8,029 of 19,982 (40 %); to a higher decorated name 7,058 of 14,276 (49 %); adjacent slots of one object 59 of 6,066 pairs with both objects known |
| Two thunks for one address in a full link: aliases only | 1,286 targets are deleting destructors (`??_G` / `??_E` names, or the flag test in the body) |
| An object's own functions in one run, in its section order | 761 of 770 objects (11,434 named functions with one owning object of ours); the 13 out-of-run pieces (37 functions) are 7 names symbols.json holds at two addresses (byte-identical small functions: `Set_Color` 0x6E5460 / 0x969300, ...) and our tree's placements (`DeltaVisitorClass::Visit_Children` 0x4B0860 in unknown_root.cpp sits in delta_visitor's run; `Unknown_684E00` in deffont.cpp; `Bitmap2DObjClass`'s ctor 0x974420 in bmp2d.cpp sits in render2d's run); none at the end of `.text` |

**The update's marks** (every slot off the rule, grouped; `contributions.tsv`):

| Where | What | Reading |
|---|---|---|
| ui_registration.obj 0x5F1A10, 0x5FB9F0, 0x5FBD30, 0x600000 | slots 16-64 bytes short of the rule | grew in place within their pad |
| after its own run, 0x60A060-0x60D750 | 0x60A060 short (the table's **last** thunk, 0x41964B, its only one), then 21 slots with 9,328 bytes of int3 beyond the rule (of the region's 12,864), around its `std::` COMDATs over `RegistrationUnknown578Struct` (`_Sort`, `_Unguarded_insert`, `_Construct`, ...) | the old copies' space left as int3 |
| **guests**: 17 contributions of 11-944 bytes inside other functions' slots from 0x4297A0 (the first app slot's pad) to 0x472C80, each pair filling the host's up16(S + S/4) exactly | the adjustor thunk 0x4297A0 (`sub ecx, 0x84; jmp RegistrationInterface`'s deleting dtor), `_Sort_0` / `_Median` / `copy_backward` over `RegistrationUnknown578Struct`, its copy ctor 0x44D040, `_Construct<StringClass>` 0x430BA0, ...; their callers are mostly in ui_registration's range (0x5FA0B0-0x60CA7A), else other guests, vtables or a later object sharing the COMDATs (0x7D3CC0-0x7DF6E3) | ui_registration's COMDATs placed **first fit** from the start of the region: 8 have no int3 run before them in the final image that could hold them, the other 9 only one (0x444390's 3,024 bytes, freed after them) |
| `_WinMain@16` 0x526DE0 | a guest of 3,040 bytes filling 0x522860's slot to its end; no earlier int3 run of 3,040 bytes | WinMain moved there in the update |
| 0x444240-0x445560 (our main.obj's neighbourhood: `Unknown_445330`, the exception filter) | 12 slots off the rule, among them 0x444380's 3,008 bytes of int3 beyond the rule | recompiled; its hole (3,024 int3 bytes in all) is 16 bytes short of WinMain's 3,040 |
| pkdirlock.obj 0x8816D0-0x882550 | 8 slots 16 bytes short; two 16-byte `Get_Data_Size` guests inside their own object's slots | grew in place (the ctors' new parameters) |
| the packet ctors 0x8667A0, 0x866870 (`Packet_0074Class`) | 16 bytes short | grew in place |
| the thunk table's last 7 slots, 0x41962D-0x41964B | the five DirLock constructors (`...@@QAE@HPBD_JUUnknown_DirLockStruct@@H@Z` etc.), 0x8667A0 and 0x60A060; the first six also have an older thunk inside the table (0x40B15E, 0x40499E, 0x401294, 0x419033, 0x4181E2, 0x418AA7) | new symbols appended by the update: the constructors' decorated names changed (a signature change), their old symbols' thunks left in place pointing at the same addresses. These six are the only targets with two thunks that are not deleting destructors |

With seven thunks appended, the full link's table ended at 0x41962D, and 0x41962D + 0x10000 rounded up to 16 is
the first app slot 0x429630: consistent with a 64 KB thunk reserve (7 to 9 appended thunks fit that reading; not
verified against a link of ours).

**What it implies.** The code bytes come from the final sources (the update recompiled the changed objects; no
older source state is needed for any function's bytes). Placement is the cost:
- our client links the app from `app.lib` (`LNK4001: no object files specified`), so no input is incrementally
  linkable: the trial link `client-inc` has no thunk table and no per-function padding. Retail's layout needs the
  app's objects on the link line, in retail's object order (the runs above give it for the paired objects);
- 81,747 `call` / `jmp rel32` in `.text` reach a thunk slot (70,892 calls, 10,855 jumps), so the thunk table's
  order decides most app code bytes in place: its rule is the next question;
- reproducing the update means a full link with stand-ins of the changed objects at their old sizes and old symbol
  names (the slots and holes above give the sizes to 16 bytes; the DirLock constructors' old signatures are
  unknown), then an incremental link of the final objects; or accepting the update's marks (48 slots, 22
  guests, 7 thunks, under 30 KB of `.text`) as a documented residue.

### 5.1 The trial link with the app's objects, and the thunk table's order rule

Measured 2026-11-25 (image builder IM2, week plan 2026-11-24 §2 D question 2).

**The `objs` trial link.** `build.py link --program client --tag objs --objs LIST --link-flag=-debug
--link-flag=-incremental:yes --link-flag=-fixed:no` puts the 651 app objects that the one client build pulls from
`app.lib` on the link line before the libraries (app.lib stays after them; it adds 2 members the one build does not
pull, spline.obj and unknown_px_callbacks.obj, 44 symbols, packed among the libraries). Object order: each object's
median retail address of its own (non-COMDAT) functions that `.work/imagediff/layout.tsv` places in the app range
(480 objects), then the 171 with none placed in the one build's map order (`.work/im2/objorder.py`). No LNK4001; the
image is 8,134,718 bytes (retail 8,642,625). imagediff: **645,557 of 8,642,625 bytes in place (7.47 %)** (`.text`
511,769 = 7.1 %), against 2.64 % for `client-inc` and 1.38 % for the one client build.

| The full link's rules, on our image | Figure |
|---|---|
| Thunk table | 0x401005-0x416A4F, 17,730 thunks, 16,610 targets (the same start as retail's; retail 19,983) |
| 64 KB reserve | first app slot 0x426A50 = up16(table end + 0x10000): retail's 0x429630 = up16(0x41962D + 0x10000) is the same rule |
| up16(S + S/4) | 16,610 of 16,610 app functions at a section start (S from the object's section) |
| Libraries packed, reserve | 9,052 of 9,053 contributions under 16 bytes of int3; then 399,774 int3 = 24.58 % of the library bytes (retail 24.90 %) |

**The order rule.** The thunk table is written in the order of LINK's external-symbol hash table. LINK.EXE 6.00
(the toolchain's, 0x432343-0x432391) hashes a decorated name byte by byte, c signed:
`h = h + (c << 18) + c + ((h >> 1) ^ h)` (32-bit), then `h %= 0x1ECA9D3B`; the table is a linear-hashing table
(Larson): bucket = `h % maxp`, or `h % (2 * maxp)` when that is below the split pointer p; a bucket splits when
16 * count / size exceeds 0x30. The thunks follow the buckets in index order, and in a bucket the symbol first seen
**last** comes first (its chain is newest first; first sight = the objects' COFF symbol tables in link order,
definitions and references alike). `tools/re/linklayout.py` has the hash (`lhash`, `bucket`, `fit_split`,
`predict_order`) and `--thunk-order`.

| Candidate (brief IM2) | Our `objs` table: adjacent pairs reproduced | Retail |
|---|---|---|
| object order, then each object's defined functions in symbol-table order | 1 of 16,029 (0.0 %) | 1 of 12,041 |
| COFF symbol-table order, first sight (defined or not) | 1 of 16,609 | 0 of 12,132 |
| first reference in link order (relocations, then definitions) | 3 of 16,609 | 4 of 12,132 |
| per section: its function, then its relocations' targets | 4 of 16,609 | 4 of 12,132 |
| our `objs` table's own order | (itself) | 148 of 11,553 (1.3 %) |
| **LINK's hash order** (maxp 8192, p fitted: 5,839 ours, 6,856 retail), chains newest first by symbol-table first sight | **13,514 of 13,546 (99.76 %)** pairs whose targets have one name each (15,532 of 17,729 with the aliases' first map name) | names on the order below |

How it was found: a second trial link with the object order reversed (`--tag objsrev`) kept the table's order
between buckets (longest common order 66.9 % of names) and reversed it inside them (adjacent pairs reversed 29.5 %);
probe links of C functions with chosen names (`.work/im2/hashprobe.py`: 52 single letters, `aa`..`bz`, `f0000`..`f3999`
forwards and backwards) gave bucket groups no textbook hash fits; the hash was then read from LINK.EXE's code
(`.work/im2/findhash.py`: a byte loop with shifts ending in a `div`). Every probe order is non-decreasing in its
buckets (53 of 53, 1001 of 1001, 4001 of 4001 slots).

**Retail follows the same rule; it checks names.** Retail's named slots (symbols.json; the deleting destructors under
both `??_G` and `??_E`) against the hash with maxp 8192: the longest run of non-decreasing buckets is greatest at
p = 6,856 (more symbols than our link: 1,017 more buckets at load 3, about 3,000 symbols); on it lie **2,791 of the
10,675 names without "Unknown"** (26.1 %), and **7 of the 3,602 "Unknown" names** (0.2 %, chance: those names are
ours, retail's hash to elsewhere). By kind, without "Unknown": destructors `??1` 33 %, other names 24 %, `std::` 21 %,
deleting destructors 20 %, constructors 16 %. A name on the order is retail's exact decorated name (calling
convention, const, every parameter type included); one off it is not (or its slot's neighbours are wrong). Per slot:
`.work/linklayout/thunknames.tsv` (slot, target, bucket, on_order, name).

**What it implies.** The thunk table's order is a function of the set of external symbol names (and their first-sight
order inside a bucket), not of the object order: reproducing retail's table, and with it the 81,747 rel32 operands
that reach it, needs retail's exact decorated name for every thunked function (about 2,800 of 14,277 named slots
have it now; 5,706 slots have no name) and the same total symbol count in LINK's table (p = 6,856 against our 5,839),
then the object order for the chains. The same hash makes the table a name oracle: a proposed name for a slot can be
checked against its neighbours' buckets without a link.

### 5.2 The name check: corrections the thunk order confirms

Measured 2026-11-25 (image builder IM3) with `tools/re/thunkname.py` on retail and symbols.json (tables in
`.work/im3/`: `namecheck.tsv` per named slot, `proposals.tsv`). The anchors are §5.1's on-order names (2,791
without "Unknown", 7 with); each other slot's real name hashes into the window between the anchors around it, and
near the bucket its slot position interpolates (thunks fill the 15,048 buckets about evenly: 94.7 % of the anchors,
each left out in turn, lie within 3 buckets of it, 88.3 % within 2).

| Slots | Figure |
|---|---|
| off-order names without "Unknown" | 7,884; 7,882 with anchors on both sides (80 with both adjacent slots anchors); window median 13 buckets (45 of width 1, 868 up to 4), near part median 6 (a random name lands in it 0.040 %) |
| "Unknown" names | 3,590 off, all with anchors on both sides (47 with both adjacent slots anchors); the same widths: a candidate name costs one hash, a hit is 1 in about 2,500 by chance |
| unnamed slots | 5,704 with anchors on both sides, the same widths |
| our bucket fits once one bounding anchor is dropped | 18 (5 "Unknown": the chance rate), state `off-conflict` |

Our off names' own buckets lie uniformly against their windows (13 within 16 buckets of it, 282 within 256;
uniform gives 17 and 268): the off names are not nearly right, most are other names (deleting destructors of
classes RTTI names lie on the order 30 of 36, those of classes we named 693 of 2,087).

**Corrections** (76,043 variants of 6,697 off names: access, virtual, static, const, calling convention, a
parameter's or the return type's integer kind, pointer cv, pointer <-> reference, class key, case, `??_G` / `??_E`;
91 names the parser does not cover; the 1,185 members of `std::` classes are left out: their signatures are the
library headers', and their hits were impossible ones such as `vector::insert`'s `size_type`). Near hits per kind against the null (the same variants against the window moved
to a random place, 20 times; it keeps each variant's distance from our bucket, which matters: an int edit of the last
parameter moves the bucket by a median 53, the tail of the name is not mixed):

| Kind | Near hits / null | Reading |
|---|---|---|
| access (public / protected / private) | 53 / 6.7 | real: 46 above chance (protected virtual destructors `MAE` with their `??_G`, private helpers `AAE`) |
| int (H / I / J / K / _N ...) | 20 / 4.2 | real, 16 above chance; can change code |
| const (`QAE` -> `QBE`) | 18 / 2.2 | real, 16 above chance (`KVTripleListClass::Find`, the `Has_*` packet getters, `Xtime` operators) |
| pointer <-> reference | 7 / 1.7 | real, 5 above (`PAVLayerClass` -> `AAVLayerClass` in three interfaces' constructors, `PAVKVListClass` -> `AAV` in `KVEncoderClass` and its base) |
| pointer cv (`PAV` <-> `PBV`) | 4 / 1.1 | real, 3 above |
| case, ret, callconv, virtual, static, `??_G` / `??_E`, class key | 13 / 14.4, 6 / 3.5, 4 / 3.6, 2 / 3.2, 1 / 1.2, 0 / 0.2, 0 / 0.5 | chance: none confirmed (callconv is what byte-exact functions predict) |

103 off names have exactly one near hit; the 82 of the five real kinds are the proposals (47 access, 18 const,
7 pointer <-> reference, 6 int, 4 pointer cv; about 12 of them expected by chance; 31 class-consistent: the same
change confirmed at another slot of the class). 78 of the 82 are `exact` in symbols.json (the name comes from our
object), and none of the corrected names exists in our objects: each is a change of the source declaration, and
symbols.json follows the objects. 76 are name-only (access, const, pointer cv, pointer <-> reference): no code byte
changes expected, so no crediting change; the 6 int ones need a compile (signedness can change code). The link: the
one client build (non-incremental, from `app.lib`) does not order by name, so nothing moves; in an incremental link
(the `objs` trial) each correction moves one thunk into retail's bucket.

### 5.3 The missing pieces, and the library members

Measured 2026-11-25 (image builder IM4, week plan 2026-11-24 §2 D questions 3 and 4) with trial links of
run/2026-11-25's objects (the app.lib build of 05:13, master 9cd27adf5) and `tools/re/rsrc.py`, `rich.py`,
`members.py`; the link wrapper is `.work/im4/pdblink.py` (build.py's link with extra batch lines: a `subst E:`
for the PDB path, a second incremental link, the libraries' order, a replaced library). The one client build at the
run's head: **118,494 of 8,642,625 bytes in place (1.37 %)**. Each trial adds to the row before it:

| Trial (`--tag`) | Added | imagediff in place | What it shows |
|---|---|---|---|
| `objs` | IM2's object list (§5.1) on the current objects | 632,912 (7.32 %) | the baseline (IM2's 7.47 % was the objects of 2026-11-24 23:11) |
| `res` | `build/res/client.res` (`rsrc.py extract`) on the object list | 637,349 (7.37 %) | `.rsrc` 8,156 of 8,156 bytes equal at retail's RVA, 8,132 by section offset (the 12 data entries' RVAs), 0 in place: it lands at 0xBD0000, inside retail's `.data`, whose zeros give the +4,437 by chance |
| `pdb` | `/pdb:e:\build\beta\G\app\main\client_release\main.pdb` (E: a subst in the VM) | 637,395 | the NB10 record is 65 bytes as retail's, 60 equal (the signature, the age); the debug entry's SizeOfData |
| `wl` | wlib's 8 pulled members, wnet's 4 and wwlib's rawfile.obj on the line (retail's order) | 664,637 (7.69 %) | +27,288 over `res` (`.text` +20,800, `.rdata` +6,149); runs 2,689 -> 2,434 |
| `lo` | retail's library order (below) | 665,587 (7.70 %) | member order: longest common order 49.7 % -> 85.6 % of retail's 533 library members |
| `lo2` | WINMM and OLE32 with the first import libraries, VC6's `DDRAW.LIB` for the DX8 SDK's, the Miles import library naming `mss32.dll` | 665,115 (7.70 %) | the 17 import descriptors in retail's order with retail's DLL names; the rich header's 17 (product, build) kinds are retail's |
| `lo3` | the Miles import library in the long format (`lib /convert`) | 665,282 (7.70 %) | `e_lfanew` 0x128 as retail (headers 3,708 -> 3,875 of 4,096) |
| `all` | **every app object** on the line, the 94 that the one client build does not pull among them (745 + wlib's 9 + wnet's 4 + rawfile.obj, each by the median of its own ledger addresses) | **1,269,778 (14.69 %)** | `.text` 902,268 in place (12.5 %), `.rdata` 217,355, `.data` 76,614, `.reloc` 67,716; `.text` 0x6E8DE1 against 0x6E58DD (+13,572 bytes); 19,996 thunks (retail 19,983, 7 of them the update's), first app slot 0x429690 (0x429630), the library region from 0x8C3868 (0x8C1B64) to 0xA4510E (0xA420AC); the import directory equal to retail's (17 DLLs, every count; `.idata` 0x38D9 as retail); `e_lfanew` 0x130 again; the best trial |
| `upd3` | `lo` then an incremental link after two changed objects (COFF stamp and @comp.id changed) | | NB10 age 2, the debug entry's stamp and the NB10 signature kept, the rich header and `e_lfanew` rewritten: retail's marks (§5) |

In place the trials up to `lo3` gain little past `wl`: every section after `.text`'s first bytes is displaced until
the code sizes match (`lo3`'s `.text` 0x67E0CC against 0x6E58DD); `all` brings them within three pages. The pieces
below are measured on their own terms.

**Question 3: the pieces a VC6 link can reproduce.**

| Piece | Retail | Reproduced by | Bytes |
|---|---|---|---|
| `.rsrc` | 0xC47000, 0x1FDC bytes: VERSIONINFO (product "main", `main.exe`, its comment the build's user and time: Tue Mar 16 2004 05:46:17 PM, 01:46:17 UTC, 23 s before the file header's stamp: the update rebuilt the resources), a TYPELIB (`MSFT`, 1,544 bytes), DIALOGs 104-112 (captions "Earth & Beyond", "Create Character", ...), one STRINGTABLE block (the build's time, date and user); CVTRES 5.00.1735 (rich pair 6/1735) | `rsrc.py extract` writes them as a .res in the order of their data; LINK's CVTRES (the VM's is 5.00.1735) gives the section byte for byte | 8,156, all once `.rsrc` starts at 0xC47000 (the sections before it end where retail's do) |
| Entry point | 0xA2B674 = `_WinMainCRTStartup` (LIBCMT wincrt0.obj, exact) | LINK's default entry, nothing to set | 4; in place when the library region lines up to wincrt0 (`lo3`: 0x9C94AE) |
| Debug directory | one CodeView entry, 28 bytes at `.rdata`'s start; NB10 record (65 bytes) after the last section | `/pdb:` with retail's path gives SizeOfData 65 and 60 of the record's 65 bytes; an incremental update after a changed object gives age 2 (`upd3`) | 28 + 65; left: three link-time stamps (file header 0x4057AE00, debug entry and NB10 signature 0x4021A831: 12 bytes) that LINK 6.00 takes from the clock (no option), and PointerToRawData (the file's layout) |
| DOS header and stub | LINK 6.00's 0x80 bytes | every link: 128 of 128 equal once `e_lfanew` is 0x128 (`lo3`) | 128 |
| Rich header | 0x80-0x120: 17 pairs, key 0xDF765B66 | `rich.py`: the key is 0x80 + the stub's bytes rotated + each pair rotated by its count; it recomputes on retail and every trial. With retail's pairs over `lo3`'s stub the key is retail's. `lo3` has retail's 17 kinds; its order differs in the first five (LINK lists a kind by first sight, newest first) | 160 bytes, equal only with retail's counts and order |
| `e_lfanew` and the PE header after it | 0x128 | the rich block's end + 8 in `lo`, `lo3` and the one client build; 24 bytes after it in `objs`, `res`, `pdb`, 16 in `lo2` and `all` (the slack follows the inputs; its rule is not known) | +167 header bytes (`lo2` -> `lo3`) |

The rich header's counts that still differ in `lo3` (`rich.py --diff`): C++ 12.00.8799 objects 1,054 against 914,
C 12.00.8797 201 against 204, cvtomf/aliasobj 7291 11 against 10, Import0 312 against 299 (13 imported functions:
retail imports GDI32 29 and USER32 60, ours 17 and 59). In `all` Import0 is retail's (the 94 objects import the 13)
and the C++ objects are **1,007 against 1,054**: retail's link took 47 more translation units of E&B's and
Westwood's code than ours (files merged in our tree, or sources not reconstructed; not resolved). The
import-member pair 4/8447 is 96 as retail's once mss32.lib is long format (81 Miles imports + 9 Bink + 6
descriptors), and 19/8034 is 21 with VC6's `DDRAW.LIB` (the DX8 SDK's ddraw.lib carries 37/8755, a kind retail
does not have).

**Question 4: the library members** (`members.py` on the `res` map; `.work/im4/members/`).

| Finding | Evidence | In the trial |
|---|---|---|
| **94 of app.lib's 745 objects are retail's and the one client build does not pull them** (nothing else of ours references them; the trials before `all` pulled two, spline and unknown_px_callbacks, from app.lib among the libraries): bandwidthaccounting, the expression parser's 10 expr_*, attrib_dir and attrib_dir_collective, boolpars, htmlview, ini_manager, lookup_table, kv_acc_time, the 4 packet objects pkgeaset / pkunknown_85d7b0 / _861760 / _861b30, ...); retail linked its app objects from the command line (§5), where LINK keeps an object whether anything references it or not | every one of the 94 holds own (not shared) ledger addresses in retail's app region; 39 of them have functions named in symbols.json (exact). 78 of retail's 1,211 static initialisers lie in 60 of them and never run in the one client build: 55 a header static's guard and its atexit, 20 their own objects' globals (unused by the rest of ours), 3 Delta type numbers taken from DeltaUnknownCounter 0xBE3B44 (ui_hdamg 0x59C6C0, ui_mount 0x5C8820): the one build numbers those types in another order (ids compared inside the process only) | `all`: +604,496 bytes over `lo3` |
| wlib (configfile, critsec, streamer, unknown_8b45b0, wdebug, wstring, wtime, xtime), wnet (tcpcon, tcpmgr, tcpqueue, udp) and wwlib's rawfile.obj are **app objects** in retail: thunked, in the app region | their 233 paired functions all lie in retail's app region (0x683BE0-0x8C0600), none in its library region | `wl`: +27,288 bytes |
| Retail's library order: WS2_32, WINMM, ADVAPI32, KERNEL32, USER32, GDI32, OLE32, wwmath, d3dx8, ww3d2, wwlib, wwdebug, VFW32, wwaudio, mss32, wwsaveload, COMCTL32, SHELL32, authlogin, wwshade, WININET, binkmovie, LIBCPMT, LIBCMT, COMSUPP, OLEAUT32, URLMON, DDRAW, binkw32 | the member runs (wwmath 0x8C1F10, d3dx8 0x8E3B36, ww3d2 0x909540, wwlib 0x9C7850, wwdebug 0x9E9F70, wwaudio 0x9EE9B0, wwsaveload 0xA03C30, wwshade 0xA07CE0, binkmovie 0xA16250, LIBCPMT 0xA18314, LIBCMT 0xA25A2C); the import stubs between them (WS2_32's at 0x8C1B64 before wwmath, AVIFIL32's at 0x9EE980, mss32's 81 at 0xA03A10, COMCTL32 / SHELL32 / authlogin at 0xA07CB0, WININET at 0xA159A0, binkw32 at 0xA41C3A); the import descriptors' order | `lo2`: descriptors in retail's order; members in order 85.6 % (d3dx8, LIBCPMT, wwshade, binkmovie 100 %, LIBCMT 86 %, ww3d2 76 %, wwaudio 59 %: inside a library the members come in the order LINK resolves their symbols, not by the library's order) |
| Retail's ddraw import library is VC6's `DDRAW.LIB` | rich 19/8034 21, no 37/8755 | `lo2` |
| Retail's Miles import library: long format, LIB 6.00.8447, DLL name `mss32.dll` | rich 4/8447 96; the descriptor's name; ours (`mkmss32.py`) links its stub as `Mss32.dll` and LINK 6.00 writes short-format members | `lo2`, `lo3` |
| ww3d2's **w3dexclusionlist.obj is ours only** (7 functions, 2,000 bytes) | pulled by hanimmgr.cpp's `Free_All_Anims_With_Exclusion_List` (and `Create_Asset_List` beside it): neither is in retail, whose `Free_All_Anims` 0x964BE0 is followed by `Add_Anim` 0x964C40; no name, RTTI or string of the class in retail; nothing calls the two | still linked |
| wwlib's stimer.obj (2 functions, 128 bytes), imported for ddraw.cpp (EXTRA_FILES) | no retail address named | still linked |
| The CRT, LIBCPMT, COMSUPP and d3dx8: no member difference shown | `.work/libs/m_*.json`: every member exact in retail but not in our map shares its addresses with a member ours pulls (identical code, `cprintf`/`output`, `wfopen`/`fopen`, d3dx8's inline vectors ...) or lies inside another function (`_wctype.obj`'s 17-byte `isw*` inside `isalpha`); our members without an exact retail address are the matcher's `short` (23 bytes or less) or `multiple` (memcpy and memmove, both in retail at 0xA25F50 and 0xA2C150); open: `wcscmp.obj` (53 bytes, `none`). LIBCIMT (the old iostreams, 6 members) has no match data | |
| Retail's library region outside our members' runs: 239 gaps, 103,866 bytes | `held` 36 gaps, 30,023 bytes (every function an exact match: statics our map does not list); `part` 18, 10,901; `unnamed` 185, 62,942 bytes, 743 functions: in our libraries' blocks about 30 KB (ww3d2 13,600, wwlib 4,640, wwmath 4,336, ...: their unmatched functions; binkmovie's 0xA16C90 gap of 12 is subtitlemanager.obj's place, its names not in symbols.json), in the CRT's about 37 KB (LIBCMT 15,106; LIBCPMT 17,419, the app's `std::` COMDATs placed among its members; COMSUPP/LIBCMT 4,408) | |

App objects: every one of the 745 holds retail addresses (the ledger's `in_tree`); §5's object runs are their
placement measure. With every app object linked, our libraries' members not pulled are wwmath's colmathsphere,
euler, obbox, ode, vehiclecurve, ww3d2's meshbuild, polyinfo, renderobjectrecycler, soundrobj, streak, streakrender,
stripoptimizer, txt, txt2d, w3d_dep, wwaudio's listenerhandle and wwsaveload's parameter and wwsaveload.obj: none
with more than one own function named in retail (`.work/im4/unpulled.py`).

**What it implies (proposals for the user; nothing of the one client build changed).** In the order of their
bytes: (1) every app object on the link line, wlib's, wnet's and rawfile.obj with them (the incremental `objs` link,
with §5.1's thunk order; `all` is the measure); (2) retail's library order, VC6's DDRAW.LIB, a long-format `mss32.lib` naming `mss32.dll`
(`mkmss32.py`: `/out:mss32.dll`, then `lib /convert`); (3) the resources from client.exe (`rsrc.py extract`, no outside
data) on the link line; (4) `/pdb:` with retail's path (an E: subst in the VM); (5) the three stamps and the age
only by an incremental update and the link times themselves, which LINK takes from the clock: left to imagediff's
"set at the end" unless the user wants a post-link stamp. In the code (the coordinator's call, D246):
hanimmgr.cpp's two exclusion-list functions and w3dexclusionlist.cpp are not retail's.

### 5.4 The trial at the run's head, LINK's e_lfanew rule, the .text surplus and the 47 objects

Measured 2026-11-25 (image builder IM5, week plan 2026-11-24 §2 D) on run/2026-11-25 with §5.1's thunk-order name
corrections and hanimmgr.cpp's exclusion-list functions and w3dexclusionlist.cpp dropped (§5.3; wt/IM5A, every
library rebuilt there), with `.work/im4/pdblink.py` and `objorder3.py` as in §5.3's `all`. The one client build of
that tree: **117,726 of 8,642,625 bytes in place (1.36 %)**. The `all` trial: **1,274,523 (14.75 %)**, from §5.3's
1,269,778: `.text` 907,247 in place, its size 0x6E8181 against retail's 0x6E58DD (**+10,404 bytes**, from +13,572);
headers 3,901 of 4,096 (`e_lfanew` 0x128 as retail's, by the rule below); the rich header's C++ 12.00.8799 count
1,006 against 1,054 (the exclusion list was one of ours only).

**`e_lfanew` is a function of the rich key.** LINK.EXE 6.00 sizes the rich block at 0x43FDD0: it walks the modules,
counts each @comp.id (newest first), computes the key (§5.3), and allocates 8 * (pairs + (key >> 5) % 3) + 0x20
bytes; 0x445828 stores e_lfanew = 0x80 + that size (the stub's 0x80). So after 'Rich' and the key come 8, 16 or 24
zero bytes as `(key >> 5) % 3` is 0, 1 or 2: no input of its own. Retail's key 0xDF765B66 gives 0 (0x128).
`rich.py` prints the rule's e_lfanew beside the image's; it holds on retail, the one client build, `client-inc`,
`objs`, `objsrev` (24 bytes after), clienttest (24), udpreplay, clientpoc and this `all` (8). An image gets retail's
0x128 once its pairs (and so its key) are retail's; until then it is 8 bytes in three keys.

**Where `.text` grows** (`tools/re/objsizes.py --map ... --ledger ...`, `growth.tsv`): the functions named once in
both images (symbols.json, our map), kept where the two orders agree, are fixed points; the displacement change
from each to the next, summed per object, telescopes to the region's growth.

| Region | Growth | Where |
|---|---|---|
| app (0x429630-0x8C1B64) | +7,328 | up to 0x4E9CB0 about -1,300; 0x4E9CB0-0x5296D0 +5,700, of which WinMain's guest place (the update, §5: our WinMain in main_client.obj's run, retail's at 0x526DE0 in 0x522860's slot; -21,200 then +25,088 around it) +3,888; 0x5296D0-0x66B770 +3,000 (single functions in ui_attrb_skills, ui_avatr, ui_class, ui_manu, ui_mfd, ui_starbase ... larger or smaller, mostly cancelling); flat to the end |
| library (0x8C1B64-0xA420AC) | +2,080 | ww3d2 +1,648 (light.obj +1,088, below), LIBCPMT +3,478 (xstrcoll: the app's `std::` COMDATs placed among its members), wwshade -1,968 and COMSUPP -1,446 (block edges); wwmath, d3dx8, wwaudio, wwsaveload exact (0); member-order swaps inside ww3d2 and wwaudio cancel (assetmgr -40,384 / line3d +37,408, render2dsentence / decalmsh 28,864, pot / colmathline 12,320, ...) |

Functions of ours retail does not have, found by the growth inside one member's run (each `none` in the gaps
reports, nothing calls them, Renegade's 2002 GTH additions as D9 excludes the Generals team's changes):
`LightClass::Is_Within_Attenuation_Radius` and `Compute_Lighting` (2/13/2002 gth; retail's
`Get_Obj_Space_Bounding_Box` 0x953AC0 is followed by `Load_W3D` 0x953AE0; 1,088 bytes) and
`ChunkLoadClass::Peek_Next_Chunk` (3/4/2002 gth; `Open_Chunk` 0x9D4310 is followed by `Close_Chunk` 0x9D4370; 144
bytes). Dropped on wt/IM5B (definitions only: blanking their declarations in light.h / chunkio.h moved
bmp2d.cpp's registers and cost Bitmap2DObjClass's constructor 0x974420 its pair).

**The CRT initialisers.** Retail's table has 1,211 entries, ours (the `all` trial) 1,251. Per library block they are
equal (wwmath 38, d3dx8 2, ww3d2 127, wwlib 21, wwdebug 1, wwaudio 16, wwsaveload 7, wwshade 31-32, binkmovie /
LIBCPMT / LIBCMT / COMSUPP 27-28); the app region holds **940 in retail, 980 in ours** (each .obj's `.CRT$XCU`
relocations give ours exactly). 21 of the 40 are `std::ctype<unsigned short>::id`'s guarded initialiser (`mov cl,
[guard]` ...: 575 in retail's app region, 596 in ours): objects of ours that instantiate the wide `ctype` facet
where retail's do not (a header difference, 32 bytes of code and a 48-byte slot each). By retail's object runs, 39
objects of ours carry one initialiser more than retail's run of the same object (two in money.obj and
unknown_46c730.obj): unknown_445740, land, unknown_46c730, unknown_474530, unknown_root, encode, unknown_changes,
lookup, policy, delta_visitor, kv_acc, kv_acc_string, kv_acc_time, expr_lexer_matchers, money, unknown_51c800,
main_client, client_item_property_special_maker, ui_bio, ui_login, ui_mfd_extended_context_strings, ui_popupmsg,
unknown_644e50, unknown_646170, gd_gauge, unknown_675120, drag_mouse, loadingview, viewbttn, unknown_6ca0c0,
fonttexture, mousetbl, enbscene, nlp, unknown_7e1e40, faction_and_reputation, unknown_galaxy, pal_mgr, udp
(`.work/im5/xcu.py`).

**The 47 C++ objects** (rich 11/8799: 1,054 in retail, 1,006 in `all`; the kind is the build's own compiler, so
only E&B's and Westwood's objects count: the MS libraries carry other builds). They hold no code of their own:
retail's app region outside our objects' runs is 143 gaps of 87,520 bytes and 295 functions, every one between
two of our objects and the largest WinMain's guest place (20,944 bytes, 2 functions) and unmatched functions at
an object's edge (11,680 in unknown_6ca0c0's, 6,544 and 6,096 in unknown_46c730's); the library region's gaps in
our libraries' blocks are under 1,700 bytes each (statics; the large ones are LIBCPMT's, LIBCMT's and d3dx8's);
and of retail's 940 app initialisers one (0x46C710) lies outside our objects' runs. So the 47 are translation
units with no function and no initialiser of their own: data-only files (tables, strings, constants) or files
whose code is all COMDATs another object supplies first, whose data our tree defines in other objects. Their
data placement in `.data` / `.rdata` (object order) is the next measure.

What it implies (proposals, nothing of the one client build changed): (1) wt/IM5B's three functions out of
ww3d2 / wwlib (1,232 bytes of `.text`); (2) the `ctype<unsigned short>` instantiations: find the header that makes
21 objects instantiate the wide facet (their includes against a retail-like object's); (3) the 47 code-less
objects from `.data` / `.rdata` (each data-only object of retail is a run of globals in object order).

### 5.5 The extra initialisers' causes, and the 47 code-less objects in the data sections

Measured 2026-11-25 (image builder IM6, week plan 2026-11-24 Â§2 D) on run/2026-11-25 (wt/IM6A, every library
rebuilt there) with the scripts in `.work/im6/` (`seg.py`, `incall2.py` / `pp.py`, `datamap2.py`, `datalayout.py`,
`dataspan.py`, `spanref.py`).

**The initialisers (Â§5.4's 40).** Retail's table is read per object with two rules the code shows: an object's
`.CRT$XCU` entries are contiguous, `<string>`'s guarded `ctype<unsigned short>::id` initialiser is its last
(workflow 3.3), and an initialiser's code lies in its object's code, a header static's (permstr.h's
PermStringInit) at the object's start, the ctype guard at its end; so a PermStringInit right after an object's
guard or code opens the next object. Ours per object come from the `.obj` files: a walk of each file's includes that
follows `#ifdef` / `#if defined` (`pp.py`) predicts all 745 objects' ctype, PermStringInit and ios initialisers
exactly (595 + 150, 134 + 611, 2 + 743). The 40 extra are no single header: 31 objects reach `<string>` (a header
that includes it for `std::string`, which `<vector>`'s `<stdexcept>` / `<xstring>` already declares; only
`<string>` itself instantiates the facet), 17 reach permstr.h, money.cpp includes `<iostream>`. wt/IM6A cuts
33 of them (980 -> 947 app initialisers) where the client's table says so, with no pair lost but two (the commits name each object's evidence; D240 rows: the font split costs FontDatabaseClass::Load_Init 0x6BF330 two instructions' order; message_filter's guard credit 0x4BDFB0, which the table gives to message_server's object, whose own `<string>` costs 0x4BCB50):
land.h's `<string>` (4 objects), money.cpp's `<iostream>` (2), permstr.h in six .cpp files whose credited
PermStringInit opens the next object (0x52CF40, 0x669F60, 0x7231C0, 0x560880, 0x5AF980, 0x5DBF70),
KVEncoderClass out of delta.h into kv_encoder.h (6), header splits by class (expr_lexer_match.h,
packets/relationship.h, kv_accessor.h, string_value.h, kv_string_encoder.h, fontsys.h, fontstring.h, smouse.h,
packets/packet_data.h) each included where its declarations stood, and unknown_7cef30.h's `<string>` moved into its
two .cpp files, unknown_675120.cpp takes UIMeshClass from ui_mesh.h, not sclient.h, and unknown_4be9d0.h std::string from <xstring>. Left (7 initialisers net; the objects named per retail run, some of them attribution noise at run edges): the objects that include a large part of the client
(unknown_6af220's guard 0x6B2410 sits after ui_guild_list's misplaced run, a pair of run-edge noise; viewbttn, mainview, mv3dview, ui_analyze, loadingview, main_client, starmap_data:
their .cpp's own include lists, not one header), interface_scene (hash_map.h's `<string>` for `hash<string>`: 11
other objects reach `<string>` only through it and keep the guard in retail), message_server (the guard 0x4BDFB0 is its own; `<string>` in it moves 0x4BCB50), and the PermStringInits of objects that reach permstr.h through kv_pair.h /
name.h (avatar_desc, unknown_445740, ui_mfd_extended_context_strings, unknown_644e50, unknown_646170, loadingview).
At wt/IM6A's head (22861ff89) the one client build has 119,292 of 8,642,625 bytes in place (1.38 %) and §5.3's
`all` trial 1,270,813 (14.70 %; `.text` 0x6E85F1, +11,540 over retail's on this run head; `e_lfanew` 0x130 by the key's
rule); at its second commit the two were 119,023 and 1,268,026.

**The last seven, read again.** The run-edge attribution above misreads two things: a retail initialiser's kind
when a `mov ecx, imm32` holds an 0xE8 byte (seq.py's byte scan; `.work/im7/rkinds.py` decodes with capstone:
retail's 940 are 577 guards, 119 PermStringInits, 2 ios, 242 others, permstr.cpp's own inlined one among the others),
and an initialiser's owner at a run edge. Aligned per kind against our objects' `.CRT$XCU` entries in the all
trial's object order (`.work/im7/objseq.py`, read from the `.obj` files: no link needed), the 7 were 5 PermStringInits
(fp_ita, loadingview, unknown_644e50, unknown_646170, ui_mfd_extended_context_strings), wnet's udp.obj's
PermStringInit and 2 `<string>` guards (viewbttn, and main_client, whose code is WinMain's guest of the update: the 15
guards from 0x517210 to 0x521B00 are the 15 objects' around it in our order, none is left for it), less message_server's guard 0x4BDFB0 (the D240 row).
The other files named above hold their guard in retail, inside their own code: mv3dview 0x7F0F30 (given to ww3d2's
collect.obj above), interface_scene 0x6DA900, mainview 0x514C40, ui_analyze 0x546860, loadingview 0x692740,
starmap_data 0x7BF7D0; avatar_desc's object opens with its PermStringInit 0x4BE920 (through the thunk 0x40FF65).
Cut, each by its object's table: loadingview.cpp without unknown_root.h (its clock Unknown_Get_Ticks declared
locally), unknown_644e50 / unknown_646170 without the auxdata.h of ui_starbase.cpp's preamble, fp_ita.cpp
without permstr.h (the PermStringInit 0x448BE0 after unknown_445740's guard is unknown_445740's: the table's one
entry out of code order, its run [0x448BE0, 0x448BB0] between two zero slots, and its static 0xBDC064 right before
unknown_445740's FollowUsesPursue 0xBDC065; the code order alone, the run-edge rule, gave it to fp_ita), udp.cpp without permstr.h (0x8C0690 after its
guard opens unknown_login_data's object). App-region initialisers 947 -> 942 (app 936, wlib / wnet 6). Left: ui_mfd_extended_context_strings (its
base's header, ui_mfd_extended_context.h, the client's `__FILE__`, reads DeltaCompoundClass +0x70 inline, so delta.h
and name.h's permstr.h; a static-free name.h costs 73 objects that open with one, a static-free kv_encoder.h costs
avatar_desc's 0x4BE920 and 0x4BE950), viewbttn and main_client (every `<string>` path through pk.h, client.h, co.h,
option.h, soundlib.h, hash_map.h and the rest: those headers without it leave 200 objects that hold the guard in retail
with none, so the original's `<string>` includes sat in other files than ours: a census of who uses std::string's
operators, not a split), message_server (`<string>` anywhere in its file, last line included, drops an EH state from
the destructor 0x4BCB50: the CritSec member's unwind). imagediff.py, before -> after: the one client build 118,895 -> 118,418 of 8,642,625 bytes in place (1.37 %), §5.3's
`all` trial 1,289,036 -> 1,286,618 (14.89 %; after the first three cuts alone 1,286,976). Fewer bytes in place for code
that is the client's: each cut initialiser and its atexit stub take their slots out, and the code after them moves
against runs that lined up only by our other displacements (`.text` is still larger than retail's), so neither
figure measures these cuts; 0x448BE0 sits after unknown_445740's guard in the client, where no source order of ours
puts a header static, so giving it back to its object costs its code position too.
The objects' own runs (§5's first question) over the app objects with `linklayout.py --objects`: 513 of 514 named
objects in one run; the one function outside, DeltaVisitorClass::Visit_Children 0x4B0860, is our placement in
unknown_root.cpp (delta_visitor.cpp's notes give it that object), not a mark of the update.

**The 47 objects: data.** Retail's addresses of our globals come from the DIR32 relocations of every exact
function (relaudit's reading; statics and literals keyed by object). Placed by our owning object and ordered by
link index, the data sections follow our object order: `.rdata` 1,832 symbols of 587 objects, 1,809 on the
longest rising chain; `.bss` 617 of 259, 609; `.data` 456 of 167, 402 (the 40 others are `.bss`'s first objects,
past `.data`'s raw end). Literals and float constants are pooled (`""` at 0xB6E944 is read by 227 objects), so
they are left out. Off the chain, with retail's neighbours:

| Retail | Ours | Between (retail order) | Reading |
|---|---|---|---|
| 0xBA8C20-0xBA8C50, 13 `char *` (the chat channel names, "General" ... "Enforcers") | ui_chat_panel.cpp's `Unknown_BA8C20` ... | chartemplate.obj and color_common.obj (ours between: chat_defs, circbuff, item_template) | chat_defs.cpp's globals, not ui_chat_panel's |
| 0xB90C60 `_Unknown_B90C60` (5) | ui_starbase.cpp | pdaint.obj and sbaseview.obj (ours between: two objects) | another object's static |
| 0xB08C90-0xB08CC4, 13 `const int` | gadget.obj | recustomization_cost and bandwidthaccounting | per-object copies of a header's consts (relaudit's case): the majority copy, no placement |
| 0xB0804C `const int` | gd_attrb_skills.obj | ship_pip and time_mgr | the same |
| `___stl_prime_list` at 0xAE70AC, 0xAE7270, 0xAECC50, 0xAF8ED0, 0xAFF270 | 5 objects | each the first includer's | SGI's per-object static const |
| 0xBDBE48, 0xBDC064 `_PermStringInit` | unknown_445740, fp_ita | main / unknown_445560 | the PermStringInit statics of the neighbour, as the CRT table says |
| 0xC404F8-0xC41554 (5 symbols) | htmlview (4), mainview | after the last app object | past every app object's `.bss` |
| 0xB10ED0 vtable `DynamicVectorClass<PlayingSoundStruct *>`, 0xB26858 `bad_cast`'s throw info | unknown_fontsound, money | after the app objects | COMDATs LINK placed with the libraries' |

Spans (`dataspan.py`: retail's distance between two anchors against ours, our objects' plain sections and the
pooled COMDATs each owns first): `.data` +148,064 bytes over 140 anchor objects, `.rdata` +41,068 over 58, `.bss`
+23,891 over 259. The large ones are tables and literal pools of objects of ours (guild_strings' GM texts
0xBA9B00-0xBAE0B8, ui_registration's 0xB85EF8-0xB8892C, sclient's chat commands 0xB98088-0xB9B9D4,
co's 0xB0069C-0xB01FE4 `.rdata`, ...): `spanref.py` finds every one read from the objects between its anchors and,
for pooled literals and floats, from objects anywhere: none is data that no object of ours reads. Retail's
`__FILE__` strings name 437 `.cpp` files: every one is a file of ours (wnet's tcpqueue.cpp in libs/), so no file
with an assert was merged into another.

**What the 47 are.** No code of their own (Â§5.4), no initialiser, no data run of their own that the relocations of
our exact functions reach, and no assert: translation units whose release build compiles to nothing linkable
(files under `#ifdef` of a debug or tool build, declaration-only files; E&B's objects are on the command line, so
LINK keeps an empty object and counts it in the rich header), or data read only by functions not yet exact
(those spans are listed above). They cost the image only the rich header's C++ count (and through the key,
`e_lfanew`, Â§5.4); 47 empty C++ objects on the trial's link line would raise its count to retail's (not tried: the C and cvtomf
counts differ too).

### 5.6 The last initialisers: WinMain's object; the object order rule; the trials at the run's head

Measured 2026-11-26 (image builder IM8, week plan 2026-11-24 §2 D) on run/2026-11-26 (wt/IM8, every library
rebuilt there) with the scripts in `.work/im8/` (`objwin.py`, `ownrange.py`, `strcut.py`, `objalpha.py`,
`objoracle.py`, `objlcs.py`, `secorder.py`, `zeros_walk.py`, `zseq.py`, `inplace.py`, `trial2.sh`,
`trials_after.sh`, `trials_before.sh`).

**The initialisers: 942 -> 940, retail's count.** Per object (§5.5's alignment, each initialiser's owner by the
object's own ledger range, `ownrange.py`):

| Object | Ours before | Retail | Cut |
|---|---|---|---|
| ui_mfd_extended_context_strings | PermStringInit + guard | guard only (run 0x5C1090-0x5C1F60; its neighbours ui_mfd_extended_context 0x5BF460, harvestable 0x5BF6E0 and ui_mfdm 0x5C1F90 open with one) | ours reached permstr.h only through ui_mfd_extended_context.h -> kv_encoder.h -> delta.h -> name.h, which that header needs for the Watch class's inline `Update_Context` (`Field->Unknown70`; its FAIL_IF l.65 / G_ASSERT l.119 carry the client's `__FILE__` 0xB82458). The base class `UIMFDExtendedContextClass` moves to `ui_mfd_extended_context_base.h` (interim, D137), which strings.h includes: no pair moved |
| main_client, main | main_client's guard; main.obj's run 0x4441E0-0x445560 as one object | no main_client object (no run, no CRT entry: WinMain 0x526DE0 is the update's guest); the generators' four initialisers, a zero slot, then ios_base::Init 0x4442C0, _Winit 0x445260, the guard 0x445540, a zero slot | **WinMain is main.obj's**: that run holds 3,024 bytes of int3 at 0x444390, after ios_base::Init's atexit 0x444380, 16 bytes short of the 3,040-byte WinMain the update moved out (§5), and main_client.cpp (`__FILE__` `e:\build\beta\G\app\main\main_client.cpp`) calls main.cpp's hooks and reads its tick count. main.cpp now `#include`s main_client.cpp (after `<iostream>`, before the hooks; alone main_client.cpp compiles to nothing, `#ifdef MAIN_CPP` on a blank line). The zero slot after the generators ends an object (below): they are `unknown_4441e0.cpp`'s (name provisional, interim D137). Cost: five pairs lost outright, D240 (0x4442C0, 0x444380, 0x445260, 0x445300, 0x445540: byte-identical, objmatch's CRT table round no longer widens main.obj's range, which reaches 0x526DE0, down to them) |
| viewbttn | PermStringInit + guard | PermStringInit 0x6B5650 only (its run ends 0x6B602D; unknown_6b6050 owns 0x6B6180) | not cut: 16 headers in its include closure include `<string>` (pk.h, client.h, co.h, option.h, soundlib.h, money.h, avatar_desc.h, hash_map.h, tsk_login.h, ...); without them 201 objects that hold the guard in ours and in retail lose it, and no header outside viewbttn's closure covers them (`strcut.py`: the greedy cover's first pick is stdio.h) |
| message_server | none | guard 0x4BDFB0 | not added: `<string>` anywhere in it costs the destructor 0x4BCB50 an EH state (attempts.csv, 3 forms; D240 row) |

So the app region has 940 initialisers as retail's, with two per-object differences left (viewbttn one more,
message_server one fewer).

**The CRT table's zero slots: the incremental link's padding, and object boundaries.** Our `all` trial's table,
walked against the link order with each object's `.CRT$XCU` entry count (`zeros_walk.py`): its 169 zero slots in the
app stretch all lie between two objects, none inside one, and every object of two or more entries is followed by
zeros (1 entry: none, 448 of 448; 2-6 entries: one, 155 of 155; 7, 8 and 11: two; 18: four); retail's 155 app
zeros lie between two objects 137 of the 138 times our ledger knows both sides, and the 138th lay inside our
main.obj, between the generators' Random4 initialiser 0x444240 and ios_base::Init 0x4442C0. So retail's object there
ends after the four generators: they move to their own object, `unknown_4441e0.cpp` (name provisional, interim
D137; main.cpp keeps `<iostream>`, the include of main_client.cpp, the hooks and the guard: retail's next segment,
IOS / WIOS / guard and a zero). Retail's table against ours predicted from the `.obj` files with that padding
(`zseq.py`): 1,107 tokens (167 zeros) against 1,108 (168), 940 initialisers each; the differences left are message_server's and viewbttn's
guards (above), and cordcon.obj's (two entries, the AutoPoolClass allocator's initialiser 0x84DF30 and the guard 0x856870, and no zero after them in retail: the one place retail has an object of two entries without one; not resolved). A third, event.obj's zero, is cut: retail lists event's ActiveIterators 0x4E8670, then 0x4E93B0, 0x4E93E0 and g_assert's map 0x4E9490 with one zero after 0x4E9490 only, so event's object has one entry and g_assert's three: the two 12-byte statics `{"", "", -1}` 0xBE5638 / 0xBE5628 (nothing refers to them; their `""` is 0xBDA908, the literal g_assert's reporters push) move to g_assert.cpp, before the reporters, and its map after them (the client's code order: 0x4E93B0, 0x4E93E0, the reporters 0x4E9410 / 0x4E9450, the map's 0x4E9490); the
two initialisers' pairs move with them (no pair lost).

**Question 2: the object order.** Retail's objects (§5.3's `all` order, each object by the median of its own
retail addresses) follow the project's source folders, alphabetical inside each (`objalpha.py`): of 612 adjacent
pairs of named objects of one source directory, 544 rise (88.9 %); the 68 falls are folder-group starts (`ui_*` ->
`camctl`, `gd_uisld` -> `acticon`, `wnd_file` -> `bar2d`, `udpconn` -> `chartemplate`, ...) and 33 of them our
`packets/pk*` file names, which are ours (the client names packets by class). So an `unknown_*` object's real name
sorts between its named neighbours: 86 of the 108 are bounded on both sides, 17 on one (`.work/im8/objoracle.tsv`;
unknown_5b7ab0 between ui_manu_util and ui_mesh, unknown_6af220 between tutorial_manager and user_click_indicator).
The one client build's object order (app.lib members, as LINK resolves them) keeps 48 of 662 common objects in
retail's order (7.3 %); the `objs` / `all` trials take retail's.

**Question 2: the thunk table's order rule, closed on a whole link.** On the `all` trial (`linklayout.py
--thunk-order --map`): 19,999 thunks, 18,712 targets (retail 19,983 / 18,691), and every one of the 19,999 named
slots on LINK's hash order (maxp 8192, split pointer fitted at p = 6,769; §5.1's `objs` trial 5,839). Retail's
fitted p is 6,856: LINK's table held about 87 x 3 = 260 more external symbols in retail's link (one bucket splits
per three symbols at the 0x30 / 16 load) than in the `all` trial's. Every thunk's bucket depends on p, so the
table is 0 bytes in place in the trial (imagediff's first range, 0x401006-0x4196A0, 99,994 bytes), and with it the
81,747 rel32 operands that reach it (§5): retail's table needs retail's symbol count before its names.
**The padding between functions** stays §5.1's measure (app functions in slots of up16(S + S/4), 16,610 of 16,610
on the `objs` trial; library members packed); not re-measured here (linklayout.py's slot audit reads retail only).
The data sections' padding is the CRT table's above.

**Question 3: the missing pieces, as trial-link figures at this head** (each a trial without one piece, against the
`all` trial with every piece):

| Trial (`--tag`) | imagediff in place | Against `all` | What it shows |
|---|---|---|---|
| `all` (every piece: §5.3's object list, retail's library order, VC6 DDRAW.LIB, long-format mss32, `build/res/client.res`, `/pdb:` retail's path) | **1,287,968 (14.90 %)** | | `.text` 909,124 in place, size 0x6E7565 against 0x6E58DD (**+7,304 bytes**, from §5.5's +11,540); headers 3,709 of 4,096 (`e_lfanew` 0x130 by §5.4's key rule: the rich C++ count is 1,007 against 1,054); the file 8,650,817 bytes against 8,642,625 |
| `nores`: without `client.res` | 1,300,735 (15.05 %) | +12,767 | `.rsrc` absent (its 906 bytes in place by chance go), but `.reloc` then starts at retail's RVA 0xC49000 (83,989 bytes in place against 71,387) and the file is 8,642,625 bytes, retail's length: every section after `.text` is 0x2000 bytes late in `all` (`.text` and `.data` larger), and `.rsrc`'s 0x2000 is what then lands `.reloc` beyond retail's; headers 3,874 (16 rich pairs without CVTRES's: the key gives 0x128). `.rsrc` itself is 8,144 of 8,156 bytes equal by section offset in `all`: the piece is right and costs only until `.text` is retail's size |
| `nopdb`: without `/pdb:` and the E: subst | 1,287,922 (14.90 %) | -46 | the NB10 record after the last section: 14 of its 65 bytes equal (60 with retail's path) |
| entry point | | | `AddressOfEntryPoint` 0x62D2E4 against 0x62B674 in every trial: `_WinMainCRTStartup` (LIBCMT's wincrt0, exact) sits 0x1C70 bytes late, `.text`'s surplus before the library region; in place once `.text` is retail's size, nothing to set |
| DOS stub | | | LINK 6.00's 128 bytes in every trial: equal once `e_lfanew` is 0x128 (`nores`), the key rule's (§5.4) |

The one client build has three sections (no `.idata`, `.rsrc` or `.reloc`): those pieces count only on a trial link.

**Question 4: the library members** at this head (`members.py --map` on the `all` trial): of our 585 library
members with code, 550 pair with retail's; the 35 without a pair (9,546 bytes) are §5.3's: LIBCMT 24 (short or
duplicated functions, `wcscmp.obj`), LIBCIMT 6 (the old iostreams, no match data), LIBCPMT 2, binkmovie's
subtitlemanager.obj (its names are not in symbols.json), mss32-long's import member and wwlib's **stimer.obj (ours
only: 2 functions, 128 bytes, no retail address named; ddraw.cpp's EXTRA_FILES import)**. Retail's library region
has 239 gaps (103,866 bytes) outside our members' runs, as in §5.3. Nothing changed since §5.3 but the
exclusion-list drop (§5.4).

**Figures before -> after (imagediff's summary line).** The one client build: run/2026-11-26's head (cacc2b687)
118,649 of 8,642,625 bytes in place (1.37 %) -> wt/IM8 118,693 (1.37 %); the `all` trial 1,312,998 (15.19 %) -> 1,287,968 (14.90 %). Neither measures the cuts (§5.5:
each cut initialiser takes its slot and stub out, and the code after it moves against runs that lined up only by our
other displacements). The `all` trial's -25,030 is WinMain's: the trial reproduces the full link, which put WinMain
in main.obj's slot at 0x444390, and retail is the update's image with WinMain at 0x526DE0, so our app code from
0x444390 to 0x526DE0 now sits 3,808 bytes (its slot) later than retail's instead of in front of it.

**What it implies (proposals for the user; nothing of the one client build's link changed).** §5.3's list stands, in
the order of its bytes: every app object on the link line in retail's order (now known as the project's folder
order, alphabetical inside), retail's library order and import libraries, the resources, the PDB path. Two findings
for the code (the coordinator's call, D246): the zero slots of retail's CRT table are a per-object initialiser check
that needs no attribution of code (`zseq.py`: three objects differ, viewbttn, message_server and cordcon); and the
thunk table needs retail's external-symbol count (about 260 more than ours) before any name in it can land in place.

### 5.7 Retail's extra external symbols, cordcon's zero slot, and the one client build's object order

Measured 2026-11-26 (image builder IM9, week plan 2026-11-24 §2 D) on tools/2026-11-26-night (c35ef215a, every
library rebuilt in wt/IM9) with the scripts in `.work/im9/` (`mkpad.py`, `pscan.py`, `ptail.py`, `mapkinds.py`,
`datakinds.py`, `relocsec.py`, `dataonly.py`, `strsets.py`, `ptrtables.py`, `slotcmp.py`, `pullsim.py`,
`maporder.py`, `arorder.py`, `inclist.py`, `inclink.py`, `drectve.py`, `cal.sh`, `q3.sh`). The trials are §5.3's
`all` (IM8's `trial2.sh` wrapper) and the one client build with one input changed (`--tag`; nothing of the one
client build's options changed).

| Trial (`--tag`) | Changed | imagediff in place | What it shows |
|---|---|---|---|
| the one client build | | **118,647 of 8,642,625 (1.37 %)** | the baseline at this head |
| `all` | §5.3's every-piece trial | **1,294,263 (14.98 %)** | split pointer p = 6,765 (fitted, every one of 19,993 slots on the order) |
| `pad` | `all` + one object of 279 unreferenced external `int`s in `.bss` (`mkpad.py`) | 1,294,421 | p = 6,858: +93 |
| `pad558` | `all` + 558 of them | 1,289,490 | p = 6,949: +184 |
| `pad273` | `all` + 273 of them | 1,294,435 | p = 6,856: retail's |
| `emp47` | `all` + 47 empty C++ objects (copies of main_client.obj: a `.drectve` and `@comp.id` only) | 1,294,291 | p = 6,765: an object without externals adds none; the rich header's C++ count 1,054 = retail's (§5.5's reading of the 47 holds), `e_lfanew` still 0x130 (the C and cvtomf counts differ) |
| `reord` | the one client build with app.lib's members in retail's order (`arorder.py`) | 118,647 | the exe byte-identical but for the 2 bytes of the stamp |
| `inc` | the one client build + `/include:` of each pulled object's first own symbol, in retail's order | 117,813 | object order 651 of 652 in retail's; +53 KB (`/include` keeps COMDATs `/OPT:REF` drops; unknown_px_callbacks pulled) |
| `inc2` | the same with symbols the one build's map keeps (645 objects; 6 have none of their own) | **119,780 (1.39 %)** | 645 of 651 in retail's order (99.1 %), the same 651 objects, +4,096 bytes of file |
| `drec` | the 645 `-include:` as directives in main.obj's `.drectve` (no link option) | 118,631 | 644 of 651 (98.9 %): main.obj first, then retail's order |

**Question 1: retail's extra external symbols.** *How many.* Retail's split pointer is pinned to **p = 6,856**
by the names at the boundary, not only by §5.1's fit (whose score is flat: 2,795-2,811 on-order names over
p = 6,700-6,975): `??1TradePacketClass@@UAE@XZ` (base bucket 6,856) and `?setBufferedWrites@TCPMgr@@QAEDPAVTCPCon@@D@Z`
(6,858) sit at their base buckets between on-order neighbours (unsplit, p ≤ 6,856), `?Get_Data_Size@NoteListPacketClass@@UAEHXZ`
(6,855) and `??0AuxDataPacketClass@@QAE@HPBXH@Z` (6,846) at their split buckets 15,047 and 15,038, the table's last
(p > 6,855) (`ptail.py`). LINK.EXE's table (0x432320: the split test `(count << 4) / size > 0x30` runs on every
lookup, before the insert) grows by one bucket per three symbols on the trials (279 -> +93, 558 -> +184), so
retail's link held **273 ± 3 more external symbols** than the `all` trial (6,856 - 6,765 = 91 buckets). An object
with no externals adds nothing (`emp47`: 47 objects, +0), so the 47 code-less C++ objects of §5.4 are not the
cause unless they define externals.

*What they are not* (retail against the `all` trial, each figure from both images the same way):

| Kind | Retail | `all` | Difference |
|---|---|---|---|
| thunked app functions (the full link's table: 19,983 less the update's 7) | 19,976 | 19,993 | -17 |
| imports (`__imp_`, their thunks, the descriptors) | 17 DLLs, 423 functions | the same | 0 |
| RTTI type descriptor names (`.?A...@@`) | 87 | 87, the same names | 0 |
| data targets of code relocations (`datakinds.py`): strings / vtables / FPU operands / other | 9,599 / 2,313 / 814 / 9,332 | 9,603 / 2,316 / 780 / 9,313 | -4 / -3 / +34 / +19 |
| data reached only from data relocations (`dataonly.py`): strings / other | 2,199 / 4,759 | 2,161 / 4,755 | +38 / +4 |
| library region (`.text` after the last thunk target to the reserve) | 1,574,216 bytes | 1,575,460 | -1,244 bytes |

So at most about 70 of the 273 are pooled constants a relocation reaches (`??_C@` literals reached from string
tables, +38; `__real@` constants, at most +34), and the functions and imports balance. The other ~200 are
externals no relocation reaches: data nobody reads (a defined, unreferenced global, as the `pad` objects' are),
or library-internal symbols. Retail's `.data` is 6,208 bytes larger in memory (0xD6F14 against 0xD56D4) and
1-2 pages larger on disk; 888 of those bytes are below (the `"2184"` statics, file-static, so not in LINK's
table); the rest, ~5.3 KB, is the size ~200 unreferenced globals of 4-40 bytes would take. Not resolved further:
no relocation names them.

*What retail's count buys.* `pad273` gives the thunk table retail's bucket function: it moves the names whose
base bucket lies in [6,765, 6,856) (91 of 8,192 base buckets, about 1.1 % of the thunks) into retail's split
bucket. §5.6's "every thunk's bucket depends on p" overstated it: the other 98.9 % of the buckets are the same
for any p in that range, and the table's bytes in place follow the slot positions, which depend on every earlier
thunk (names and count). Thunk range 0x401006-0x4196A0, bytes equal at the same address (`rangesum.py`): `all`
41,608 of 99,994 (the `E9` bytes and equal high bytes of rel32), `pad` 41,693, `pad273` 41,682; slots naming
retail's function at retail's address (`slotcmp.py`): `all` 27, `pad` 34, `pad273` 32. The table waits
for the names (§5.2), not for the count.

*A finding on the way* (data, not externals): **222 retail objects hold a file-static `char *` to `"2184"`**
(0xB6FA8C, the auth product id: tsk_login.cpp's `AUTH_PRODUCT`, ui_registration.cpp's `RegistrationGameId`), the
first 4 bytes of each object's `.data` (0xB6FA88, 0xB7965C, ...: 222 data relocations, none from code). Ours have
it in those two files only, so 220 objects' `.data` is 4 bytes short; the button-name tables of the `ui_*` objects
start right after it (`ptrtables.py`: retail's tables are ours plus that entry). A header of E&B's that 222 files
include defines it (`static char *... = "2184";`); not found here which (the coordinator's call, D246: a census of
the 222 objects against our include lists, as §5.5 did for `<string>`).

**Question 2: cordcon.obj's zero slot.** *The padding rule.* The incremental link pads every data contribution by
S / 5 bytes (integer), the next contribution then aligned: `.CRT$XCU` 4 bytes -> no zero, 8-24 -> one, 28-44 ->
two, 72 -> four (§5.6's table); `.bss` bandwidthaccounting.obj 124 bytes in retail (0xBF4040-0xBF40BC, by its
code's references) + 24 -> 0xBF40D4, aligned 8 -> 0xBF40D8, cordcon's pool (ours: 120 + 24 -> 0xBF5B78 the same
way). So no zero after two initialisers means two contributions of 4 bytes. *Causes tested.* (a) The pool's
initialiser in a COMDAT `.CRT$XCU` (a template-static form): ruled out, its code 0x84DF30 is the unguarded
`xor eax, eax`, 5 stores, `atexit` of nav_list_builder's 0x7B7B60, whose object keeps its zero (0xB6CD10) with the
same two entries; VC6 guards a template static's initialiser instead (the ctype guard's form). (b) The update
(cordcon recompiled): no slot of cordcon's run is off the up16(S + S/4) rule, and a contribution cannot lose its
pad in place. (c) **Two objects**: the client's run 0x84DF30-0x8568A0 holds the pool's initialiser and atexit stub,
CordChannelClass's constructors, operator=, Copy, destructor and Reset (0x84DFC0-0x84E770), then **a block of
template instantiations** (vector<CordMessageClass> 0x84E8A0-0x84EDB0, CordListClass's destructor 0x84EDF0,
ArrayList<unsigned>, std::copy / fill / copy_backward, CordMessageClass's operator= 0x84F150, CordListClass::Add
0x84F220, ArrayList::get / add) **before CordCon's constructor 0x84F640**; VC6 emits a translation unit's
instantiations after its own functions (our one cordcon.obj puts the same block at 0x855D20, after CordCon's code).
So the client's object ends at 0x84F640: the pool and CordChannelClass are a file of their own, sorting between
bandwidthaccounting and cordcon, with one CRT entry (0x84DF30), and cordcon.obj has one (its `<string>` guard
0x856870): no zero after either. wt/IM9A splits it so (`app/wwudp/unknown_84df30.cpp`, name provisional,
interim D137; cordcon.cpp keeps its lines, the asserts on 521, 632, 977): no pair lost outright (pairs_cmp: CordChannelClass's pairs and the instantiations' move to the new file), and the CRT table predicted from the objects (IM8's `zseq.py`) is retail's at cordcon (1,107 tokens and 167 zeros each; the two differences left are message_server's guard 0x4BDFB0 and one guard more in local_interface_sort.obj).

**Question 3: the one client build's object order.** LINK pulls app.lib's members **breadth-first by first
reference**: one list of undefined symbols, from `_WinMain@16` (wincrt0's, pulled first for the entry point), each
pulled member's undefined externals appended in its COFF symbol-table order, the list walked front to back; a
symbol an unpulled member defines pulls that member. That model (`pullsim.py`, the objects' symbol tables only)
gives the one client build's order exactly: 651 of 651 in order, 648 of 650 adjacent pairs (a depth-first walk:
15.7 %). The librarian's member order plays no part (`reord`: the exe is byte-identical but the stamp; app.lib's
members are in reverse `dir /b` order, LIB.EXE's), nor does the dictionary's. So retail's order (the folders'
order, alphabetical inside, §5.6) cannot come from app.lib's build: it needs the objects named to LINK in that
order, by the command line (§5.3's `objs` / `all`), by `/include:` (`inc2`: 99.1 %), or by `-include:` directives
in main.obj (`drec`: 98.9 %, main.obj first; a `#pragma comment(linker, "/include:...")` list in main.cpp would
do the same with no link option, at the cost of 645 lines of non-client source). In the non-incremental one
client build the order is worth +1,133 bytes in place (`inc2`): retail's layout is the incremental link's, so
order alone moves little.

**Figures before -> after (imagediff's summary line).** The one client build: at tools/2026-11-26-night
(c35ef215a, the trials above) 118,647 of 8,642,625 bytes in place (1.37 %); on run/2026-11-26-night (f6c75de35)
118,455 (1.37 %) -> wt/IM9A (the cordcon split) 117,969 (1.36 %): the split moves CordChannelClass's code and
instantiations in the non-incremental link's breadth-first order, a figure that does not measure a
client-evidenced cut (§5.5). The `all` trial: 1,294,263 (14.98 %) at c35ef215a; 1,291,269 -> 1,291,293 (14.94 %)
on run/2026-11-26-night with the split (unknown_84df30.obj before cordcon.obj on the line).

**Proposals (the user's: the one client build's options; nothing changed).** (1) To take retail's object order in
the one client build: the app objects on its link line in retail's order (or an `/include:` list), +1,133 bytes in
place as a non-incremental link, the full gain only with §5.3's incremental link. (2) None needs retail's symbol
count: it lands by itself when the names and the missing unreferenced globals do. For the code (the coordinator's
call, D246): the header that gives 222 objects their `"2184"` static.
