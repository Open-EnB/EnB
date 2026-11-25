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
| `lo3` | the Miles import library in the long format (`lib /convert`) | **665,282 (7.70 %)** | `e_lfanew` 0x128 as retail (headers 3,708 -> 3,875 of 4,096); the best trial |
| `upd3` | `lo` then an incremental link after two changed objects (COFF stamp and @comp.id changed) | | NB10 age 2, the debug entry's stamp and the NB10 signature kept, the rich header and `e_lfanew` rewritten: retail's marks (§5) |

In place the trials gain little past `wl`: every section after `.text`'s first bytes is displaced until the code
sizes match (`.text` 0x67E0CC against 0x6E58DD). The pieces below are measured on their own terms.

**Question 3: the pieces a VC6 link can reproduce.**

| Piece | Retail | Reproduced by | Bytes |
|---|---|---|---|
| `.rsrc` | 0xC47000, 0x1FDC bytes: VERSIONINFO (product "main", `main.exe`, its comment the build's user and time: Tue Mar 16 2004 05:46:17 PM, 01:46:17 UTC, 23 s before the file header's stamp: the update rebuilt the resources), a TYPELIB (`MSFT`, 1,544 bytes), DIALOGs 104-112 (captions "Earth & Beyond", "Create Character", ...), one STRINGTABLE block (the build's time, date and user); CVTRES 5.00.1735 (rich pair 6/1735) | `rsrc.py extract` writes them as a .res in the order of their data; LINK's CVTRES (the VM's is 5.00.1735) gives the section byte for byte | 8,156, all once `.rsrc` starts at 0xC47000 (the sections before it end where retail's do) |
| Entry point | 0xA2B674 = `_WinMainCRTStartup` (LIBCMT wincrt0.obj, exact) | LINK's default entry, nothing to set | 4; in place when the library region lines up to wincrt0 (`lo3`: 0x9C94AE) |
| Debug directory | one CodeView entry, 28 bytes at `.rdata`'s start; NB10 record (65 bytes) after the last section | `/pdb:` with retail's path gives SizeOfData 65 and 60 of the record's 65 bytes; an incremental update after a changed object gives age 2 (`upd3`) | 28 + 65; left: three link-time stamps (file header 0x4057AE00, debug entry and NB10 signature 0x4021A831: 12 bytes) that LINK 6.00 takes from the clock (no option), and PointerToRawData (the file's layout) |
| DOS header and stub | LINK 6.00's 0x80 bytes | every link: 128 of 128 equal once `e_lfanew` is 0x128 (`lo3`) | 128 |
| Rich header | 0x80-0x120: 17 pairs, key 0xDF765B66 | `rich.py`: the key is 0x80 + the stub's bytes rotated + each pair rotated by its count; it recomputes on retail and every trial. With retail's pairs over `lo3`'s stub the key is retail's. `lo3` has retail's 17 kinds; its order differs in the first five (LINK lists a kind by first sight, newest first) | 160 bytes, equal only with retail's counts and order |
| `e_lfanew` and the PE header after it | 0x128 | the rich block's end + 8 in `lo`, `lo3` and the one client build; 24 bytes after it in `objs`, `res`, `pdb`, 16 in `lo2` (the slack follows the inputs; its rule is not known) | +167 header bytes (`lo2` -> `lo3`) |

The rich header's counts that still differ in `lo3` (`rich.py --diff`): C++ 12.00.8799 objects **1,054 against 914**
(retail's link took 140 more translation units of E&B's and Westwood's code than ours: files merged in our tree or
members it pulls that ours lacks; not resolved), C 12.00.8797 201 against 204, cvtomf/aliasobj 7291 11 against 10,
Import0 312 against 299 (13 imported functions: retail imports GDI32 29 and USER32 60, ours 17 and 59). The
import-member pair 4/8447 is 96 as retail's once mss32.lib is long format (81 Miles imports + 9 Bink + 6
descriptors), and 19/8034 is 21 with VC6's `DDRAW.LIB` (the DX8 SDK's ddraw.lib carries 37/8755, a kind retail
does not have).

**Question 4: the library members** (`members.py` on the `res` map; `.work/im4/members/`).

| Finding | Evidence | In the trial |
|---|---|---|
| wlib (configfile, critsec, streamer, unknown_8b45b0, wdebug, wstring, wtime, xtime), wnet (tcpcon, tcpmgr, tcpqueue, udp) and wwlib's rawfile.obj are **app objects** in retail: thunked, in the app region | their 233 paired functions all lie in retail's app region (0x683BE0-0x8C0600), none in its library region | `wl`: +27,288 bytes |
| Retail's library order: WS2_32, WINMM, ADVAPI32, KERNEL32, USER32, GDI32, OLE32, wwmath, d3dx8, ww3d2, wwlib, wwdebug, VFW32, wwaudio, mss32, wwsaveload, COMCTL32, SHELL32, authlogin, wwshade, WININET, binkmovie, LIBCPMT, LIBCMT, COMSUPP, OLEAUT32, URLMON, DDRAW, binkw32 | the member runs (wwmath 0x8C1F10, d3dx8 0x8E3B36, ww3d2 0x909540, wwlib 0x9C7850, wwdebug 0x9E9F70, wwaudio 0x9EE9B0, wwsaveload 0xA03C30, wwshade 0xA07CE0, binkmovie 0xA16250, LIBCPMT 0xA18314, LIBCMT 0xA25A2C); the import stubs between them (WS2_32's at 0x8C1B64 before wwmath, AVIFIL32's at 0x9EE980, mss32's 81 at 0xA03A10, COMCTL32 / SHELL32 / authlogin at 0xA07CB0, WININET at 0xA159A0, binkw32 at 0xA41C3A); the import descriptors' order | `lo2`: descriptors in retail's order; members in order 85.6 % (d3dx8, LIBCPMT, wwshade, binkmovie 100 %, LIBCMT 86 %, ww3d2 76 %, wwaudio 59 %: inside a library the members come in the order LINK resolves their symbols, not by the library's order) |
| Retail's ddraw import library is VC6's `DDRAW.LIB` | rich 19/8034 21, no 37/8755 | `lo2` |
| Retail's Miles import library: long format, LIB 6.00.8447, DLL name `mss32.dll` | rich 4/8447 96; the descriptor's name; ours (`mkmss32.py`) links its stub as `Mss32.dll` and LINK 6.00 writes short-format members | `lo2`, `lo3` |
| ww3d2's **w3dexclusionlist.obj is ours only** (7 functions, 2,000 bytes) | pulled by hanimmgr.cpp's `Free_All_Anims_With_Exclusion_List` (and `Create_Asset_List` beside it): neither is in retail, whose `Free_All_Anims` 0x964BE0 is followed by `Add_Anim` 0x964C40; no name, RTTI or string of the class in retail; nothing calls the two | still linked |
| wwlib's stimer.obj (2 functions, 128 bytes), imported for ddraw.cpp (EXTRA_FILES) | no retail address named | still linked |
| The CRT, LIBCPMT, COMSUPP and d3dx8: no member difference shown | `.work/libs/m_*.json`: every member exact in retail but not in our map shares its addresses with a member ours pulls (identical code, `cprintf`/`output`, `wfopen`/`fopen`, d3dx8's inline vectors ...) or lies inside another function (`_wctype.obj`'s 17-byte `isw*` inside `isalpha`); our members without an exact retail address are the matcher's `short` (23 bytes or less) or `multiple` (memcpy and memmove, both in retail at 0xA25F50 and 0xA2C150); open: `wcscmp.obj` (53 bytes, `none`). LIBCIMT (the old iostreams, 6 members) has no match data | |
| Retail's library region outside our members' runs: 239 gaps, 103,866 bytes | `held` 36 gaps, 30,023 bytes (every function an exact match: statics our map does not list); `part` 18, 10,901; `unnamed` 185, 62,942 bytes, 743 functions: in our libraries' blocks about 30 KB (ww3d2 13,600, wwlib 4,640, wwmath 4,336, ...: their unmatched functions; binkmovie's 0xA16C90 gap of 12 is subtitlemanager.obj's place, its names not in symbols.json), in the CRT's about 37 KB (LIBCMT 15,106; LIBCPMT 17,419, the app's `std::` COMDATs placed among its members; COMSUPP/LIBCMT 4,408) | |

App objects: every one of the 651 the trials link holds retail addresses (the ledger's `in_tree`); §5's object runs
are their placement measure.

**What it implies (proposals for the user; nothing of the one client build changed).** In the order of their
bytes: (1) the app objects on the link line with wlib's, wnet's and rawfile.obj (the incremental `objs` link, with
§5.1's thunk order); (2) retail's library order, VC6's DDRAW.LIB, a long-format `mss32.lib` naming `mss32.dll`
(`mkmss32.py`: `/out:mss32.dll`, then `lib /convert`); (3) the resources from client.exe (`rsrc.py extract`, no outside
data) on the link line; (4) `/pdb:` with retail's path (an E: subst in the VM); (5) the three stamps and the age
only by an incremental update and the link times themselves, which LINK takes from the clock: left to imagediff's
"set at the end" unless the user wants a post-link stamp. In the code (the coordinator's call, D246):
hanimmgr.cpp's two exclusion-list functions and w3dexclusionlist.cpp are not retail's.
