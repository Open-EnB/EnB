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

**Corrections** (87,303 variants of the 7,882 off names: access, virtual, static, const, calling convention, a
parameter's or the return type's integer kind, pointer cv, pointer <-> reference, class key, case, `??_G` / `??_E`;
91 names the parser does not cover). Near hits per kind against the null (the same variants against the window moved
to a random place, 20 times; it keeps each variant's distance from our bucket, which matters: an int edit of the last
parameter moves the bucket by a median 53, the tail of the name is not mixed):

| Kind | Near hits / null | Reading |
|---|---|---|
| access (public / protected / private) | 53 / 7.0 | real: 46 above chance (protected virtual destructors `MAE` with their `??_G`, private helpers `AAE`) |
| int (H / I / J / K / _N ...) | 21 / 3.8 | real, 17 above chance; can change code |
| const (`QAE` -> `QBE`) | 18 / 2.3 | real, 16 above chance (`KVTripleListClass::Find`, the `Has_*` packet getters, `Xtime` operators) |
| pointer <-> reference | 8 / 1.9 | real, 6 above |
| pointer cv (`PAV` <-> `PBV`) | 5 / 1.6 | real, 3 above |
| case, ret, callconv, virtual, static, `??_G` / `??_E`, class key | 13 / 14.7, 6 / 4.1, 5 / 3.8, 2 / 3.3, 1 / 1.0, 0 / 0.7, 0 / 1.2 | chance: none confirmed (callconv is what byte-exact functions predict) |

107 off names have exactly one near hit; the 85 of the five real kinds are the proposals (47 access, 18 const,
8 pointer <-> reference, 7 int, 5 pointer cv; about 13 of them expected by chance; 31 class-consistent: the same
change confirmed at another slot of the class). 81 of the 85 are `exact` in symbols.json (the name comes from our
object), and none of the corrected names exists in our objects: each is a change of the source declaration, and
symbols.json follows the objects. 78 are name-only (access, const, pointer cv, pointer <-> reference): no code byte
changes expected, so no crediting change; the 7 int ones need a compile (signedness can change code). The link: the
one client build (non-incremental, from `app.lib`) does not order by name, so nothing moves; in an incremental link
(the `objs` trial) each correction moves one thunk into retail's bucket.
