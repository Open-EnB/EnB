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

Status 2026-10-25: both met. Results per work item are in [library-matching.md](library-matching.md) §3.8;
the builders' rules are in [builders.md](builders.md).

## 1. What exists (2026-10-25)

* M1 is met on gates (a)-(d); the harness is `tests/capharness/` with `tools/harness/`.
* M2 is complete: the one-time supervised E&BConfig run (D63) passed 2026-10-25 (progress.md log); the repeatable check of gate (c) stays `tools/harness/vidtest_e2e.py`.
* The app library is at 458 byte-exact pairs, 54 ambiguous. Auxdata, the root schemas, wwudp, the kernel, the connection layer and the packet factory are rebuilt (newest entry of progress.md).
* The link step works: `tools/vc6/build.py` builds the eight libraries, `build.py link --program vidtest` links vidtest.exe (§3).
* The decode path's reference table is [decode-path.md](decode-path.md). VIDTEST's spec is [vidtest.md](vidtest.md).

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

The comparison scripts are Python (`tools/harness/`) and read the reference decoder's JSON output. The
C++ harness itself stays small.
### 2.2 Work items

Each item has a status line (results in [library-matching.md](library-matching.md) §3.8) and its open leftovers.

### M1.0 Inputs

* **What:** **Mostly done already** in the reference decoder: the message-level inputs are the `ea\*.trace.jsonl` imports above, and the checks exist (`the reference decoder roundtrip`; `aux-census`: all 43,978 decode, re-encode and match the typed model; `aux-log --first`: every field value and change, the reference for (c)). Left: (1) a datagram-level file for gate (a): `tools/harness/convert.py` reads the Alex dumps (`session: <id> rc4 key: <8 bytes>`, then `C->S`/`S->C seq <n> len <hex>` and the decrypted datagram, checksum first), the same parse as `EnB.Trace/Ea/the reference importer.cs`, which keeps only the messages; (2) save `aux-census` and `aux-log --first` output per capture under `.work/harness/ref/`; (3) the harness skeleton reads trace.jsonl and the datagram file and counts messages
* **Done when:** counts equal the reference decoder's
* **Size:** small
* **Status:** Done 2026-10-24 (tools/harness/README.md); results in library-matching.md §3.8.

### M1.1 Decode path

* **What:** Map the receive path from `AuxDataPacketClass::Process_Packet` 0x7364C0 to the leaves, and list every function on it that isn't byte-exact yet (encode.cpp's codecs 0x4A9400 / 0x4A8FD0 / 0x4A9710, the rest of the leaf types: item arrays, interpolators, records, policy where the decoders call it)
* **Done when:** a list in this file, each function with its status
* **Size:** small
* **Status:** Done 2026-10-24: [decode-path.md](decode-path.md).

### M1.2 auxdata completion

* **What:** The functions from M1.1, byte-exact where possible and functional where not (P1). This includes the remaining items in progress.md §4 step 2: the time field, the encoder `Add` functions, kv_pair_string, kv_list, the encode.cpp and policy.cpp rules
* **Done when:** harness (b) passes on a hand-built root
* **Size:** large
* **Status:** Decode path and encoder side done 2026-10-24 (library-matching.md §3.8); the hand-built root passes (`tools/harness/auxdata_smoke.py`).
* Open: the items the earlier note listed (packet entry 0x7364C0, root class 0x4A3990-0x4A4780, update collector 0x4A8990-0x4A8FC0, change table 0x4AB660-0x4ABCD0) were rebuilt under M1.3 except the packet entry (the packet classes, progress.md §4).

### M1.3 Root schemas

* **What:** The 13 root classes' constructors. A generator turns the extracted schema into C++ in the client's form (one `Add` per node, in order), and verify checks the result against the client (P2)
* **Done when:** (b) and (c) pass on the EA captures
* **Size:** medium
* **Status:** Gates (b) and (c) met 2026-10-24, constructors exact late the same day: library-matching.md §3.8.
* Open: Ship 0x6F66E0 (ours inlines the Buff array 0x707040, which the client calls, so its tail calls the colour copy constructor the client inlines; the stack slots follow), the collector's two Visits, 0x4AB7D0, 0x4ABB80, 0x4AE760, 0x4AE8D0, 0x4AEC70, 0x4AF190, 0x4AF3A0, 0x4A40B0 (register choices), the 17 leaf Reads (identical copies: see the report's proposal), DeltaCompoundClass::Add's out-of-line size() (D40)

### M1.4 wwudp

* **What:** `CordMgr`, `CordCon`, bandwidth accounting: handshake (SYN1/ACK1/SYN2/ACK2), checksum, block types, fragments, sequence and resend handling, ack lists, close. Socket and timer behind the class's own interface, replaced by a replay socket in the harness
* **Done when:** harness (a) passes
* **Size:** large
* **Status:** Gate (a) met 2026-10-24, see library-matching.md §3.8.
* Open: CordMgr::Pump's tail (each connection's pump, Pump_Connections: needs its timer), Compute_Ticks; exactness of the rest needs WWOnline's wlib (LinkedList, ArrayList, Wtime) and wnet (UDP)

### M1.5 Net-7 recordings

* **What:** Message-level replay of the TCP recordings, plus (d). Whether the client's TCP transport is also in `app\wwudp` is checked here (P6)
* **Done when:** (d) passes
* **Size:** small
* **Status:** Gate (d) met 2026-10-24, see library-matching.md §3.8. D47: the TCP transport is not in `app\wwudp` (it is `app\common\tcpconn.cpp`, 0x828C80-0x82A830).
* Open: the TCP replay (not started). It takes tcpconn.cpp (25 functions), the stream base and circbuff.cpp (about 15), a `TCPCon` / `TCPMgr` header, a replay connection fed from the pcapng streams, and the session key recovered with the server's side 0x829E50 (details in library-matching.md §3.8; kernel-map.md steps 6, 8-9).

The order is M1.0, then M1.1, then M1.2 and M1.3 (they can overlap), then M1.4, then M1.5.
This follows D6 (auxdata, then wwudp).

### 2.3 The decode path (M1.1)

The layer-by-layer table of the receive path moved to [decode-path.md](decode-path.md).

## 3. The link step (shared)

| Item | Content | Done when |
|---|---|---|
| **L1** Libraries | `tools/vc6/build.py`: compile each library completely, every file and not only the ones that match, with its verify flags, then `LIB` it into `wwlib.lib`, `ww3d2.lib`, and so on, plus `app.lib`. Runs on a pool VM (`--pool`); the output goes to `build/` (git-ignored) | eight `.lib` files. **Done 2026-10-24** (library-matching.md §3.8) |
| **L2** First executable | Link a minimal WinMain against them with LIBCMT, `d3dx8.lib`, winmm, ws2_32 and the rest. Fix the unresolved externals: NOT_LINKED files, declared-only functions (D28-type declarations), E&B hooks the libraries call back into. Each fix is either a real function or a stub under `stubs/` that is listed and logs when it is called (P3) | a linked `.exe` that starts, `WW3D::Init` succeeds, and the window clears to a colour. **Done 2026-10-24** (library-matching.md §3.8) |

L2's stub list doubles as a to-do list: each stub marks a function the client has and we haven't
written yet.

### 3.1 L1 and L2 results

Moved to [library-matching.md](library-matching.md) §3.8 (library table, the 125 unresolved externals by cause, what the first link took).

### 3.2 How the programs are built and run

Run on a leased VM after `build.py` and `mkmss32.py lib`:

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

**Stubs** (`stubs/`, D44; each logs its first call to OutputDebugString, stderr and `ENB_STUB_LOG`/`stubs.log`; asserts log every call). As of 2026-10-25 only these remain (the L2-time table, as of 2026-10-24, is in library-matching.md §3.8; most of its entries have been rebuilt):

| File | Stub | Client | Note |
|---|---|---|---|
| app_stubs.cpp | `Unknown_Report_Message`, the reporter of the assert functions (rebuilt 2026-10-25 in app/shared/unknown_debug.cpp; logs each call: category, text, file, line) | 0x4BC260 | D75's stand-in |
| app_stubs.cpp | `Unknown_Debug_Printf` (logs the text) | ? | permstr.cpp's statistics line |
| app_stubs.cpp | data: `DeltaUnknownGlobal`, `DeltaCompoundUnknownGlobal`, `DeltaStringClass::UnknownGlobal` (0xBE3A9C) | | 0, cannot log |
| app_stubs.cpp | not stubs, kept here until strconv_lib.cpp is rebuilt: `Value_To_String` / `String_To_Value` for float and unsigned (0x4A3200 / 0x4A3240 / 0x4A3590 / 0x4A35C0) | | client behaviour |
| app_stubs.cpp | MoneyClass: `Type_Name`, `Value_To_String`, `String_To_Value` | 0x4F0AB0, 0x4F0BE0, 0x4F0640 | log when called |
| wwaudio_stubs.cpp | `ListenerHandleClass::Initialize` (declared in listenerhandle.h, defined in no EA copy; does nothing) | ? | |

## 4. M2: VIDTEST

### 4.1 The test

Regression check (D78): after any change to a library or to vidtest, run `python tools/harness/drawcheck.py run`
(frame 900's draw-call groups, original vs ours, against `drawcheck_baseline.json`; tools/harness/README.md, drawcheck.py);
it must print PASS.

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

**Status 2026-10-25.** M2 is met on all four gates: (a), (b), (d), and (c) both by its repeatable check, `tools/harness/vidtest_e2e.py`, and by the one-time supervised E&BConfig run of D63 (passed 2026-10-25, evening: the original E&BConfig displayed every value our vidtest wrote; registry unchanged). Per-gate figures: library-matching.md §3.8 "M2 gate status".

### 4.2 Work items

### M2.0 Spec

* **What:** Open VIDTEST.EXE in REA and write down its program flow: WinMain, device setup, the scene list and camera paths, each test's loop and measurement, and `results.ini`. Map its 437 app functions to client functions (252 already match) and to WW3D2 calls. Output: `docs/vidtest.md`
* **Done when:** the spec names every scene, test and output key
* **Size:** medium
* **Status:** Done (spec in vidtest.md; results in progress.md's log).

### M2.1 One model

* **What:** After L2: an asset manager on `EBCONFIG`, a camera, a light and one ship (`p?v?01a`) with its DDS textures, rendered in a loop. A screenshot is taken through `WW3D::Make_Screen_Shot` (progress.md §4 step 3) or a plain `D3DX` save
* **Done when:** screenshot compared with the original's
* **Size:** medium
* **Status:** Done (spec in vidtest.md; results in progress.md's log).

### M2.2 Scenes and text

* **What:** All W3D scenes, backdrops, `font.tga` text overlay, the command line, windowed and fullscreen modes
* **Done when:** (a) for the W3D scenes
* **Size:** medium
* **Status:** Done (spec in vidtest.md; results in progress.md's log).

### M2.3 Tests

* **What:** Fill Rate and Polygon Budget tests, `results.ini`
* **Done when:** (b) for these two, (c)
* **Size:** small
* **Status:** Done (spec in vidtest.md; results in progress.md's log).

### M2.4 E&B scene classes

* **What:** Land (terrain, lightmap, LOD), stars, decorations, splines, and the Land LOD test (P5)
* **Done when:** (a) and (b) complete, then (d)
* **Size:** large
* **Status:** Mostly done 2026-10-24; results in library-matching.md §3.8.
* 2026-10-26 (builder L3): spln_db.cpp rebuilt as the client's (0x4845F0-0x489830, HashListClass of CRC-keyed entries; +45 pairs, 30 of 50 functions exact, the rest marked NOT EXACT) and the height field on the client's layout (+3 pairs); VIDTEST loads the same 52 splines, drawcheck PASS. StarLayer Load 0x481DB0 (order-only) and StarField Init 0x4829E0 (register-only) parked in attempts.csv.
* 2026-10-27 (builder L4): LandClass on the client's 0x1228-byte layout (land.h); exact: Create, ctor, dtor, the small virtuals, Update_Lightmap_Full 0x46D900 and its worker/swap, Prepare_Lighting 0x46C730, the colour map get/set, the patch's Init / Draw, the index-buffer setters, the +0x1218 vector (land 21 -> 44 pairs, the vector templates included). Functional, NOT EXACT: Update_Lightmap_ManualThreaded / _Threaded (inline-budget: the client copies the lighting record as one struct, operator= 0x4744C0), Upload_Lightmap 0x46D6B0 (frame), Apply_Colors 0x46D410 (shape), Build_Vertices 0x474790 (frame), Build_Index_Buffers 0x478240. VIDTEST draws with programs/vidtest/vidland.cpp (the former functional land, as VidtestLandClass) until LandClass can draw; its trees now use the client's two-triangle Sample (VIDTEST.EXE's own 0x402AE0 -> 0x403400; frames closer to the original).
* 2026-10-27 (builder L5): exact: spln_db's Evaluate_Curve 0x485620 / Evaluate_Color 0x485790 on a minimal spline.h, spline.cpp's Resize 0x483820, the land's Load_PLD / Save_PLD 0x46EB60 / 0x46ECF0, Init_Horizon_Tables 0x46EF60, the height field's Cast_AABox 0x465030 and vector<Vector3>::insert 0x4663E0, plus template and COMDAT copies (land 47 -> 60, heightfield 4 -> 7, spln_db 61 -> 63, spline 0 -> 1 pairs with LandClass::Cast_AABox 0x46C6B0). Functional, NOT EXACT: Load_Assets 0x4691D0 (inline-budget), Limit_Levels 0x46AA40 over std::deque<Vector2i>, Update_Patches 0x46B3C0, Compute_Lighting 0x46CAE0 (order, 64 rows), Compute_Horizons 0x46E670 (21 rows) and its buffer step 0x473290, Render 0x474B30 (not run), Save_Sea_Mask 0x46BCC0, the height field's Cast_Ray 0x4647C0, Cast_OBBox 0x4654E0 and Get_Triangles 0x465D30.
* 2026-10-27 (builder L6): exact: the horizon scanner 0x4733C0, the companion object in new app/shared/landdmap.cpp (ctor 0x466BE0, dtor 0x466DE0 / 0x466D40, 0x466E70, 0x466F10, the DMap registry reader 0x468120, 0x467910; it is the Matrox displacement-mapped land, off on other hardware); functional, NOT EXACT: the horizon steps 0x46E120, 0x46F120, 0x470F80 / 0x472F80, the scanners 0x473780 / 0x473BC0 (frame), the companion's Matrox-only draw 0x466F60 and helpers (app +10 pairs). A trial in a worktree (not committed) linked VIDTEST on LandClass: Render 0x474B30 draws the land (the land group now has the original's A8R8G8B8 lightmap key; 170 draws / 97,580 prims against the original's 212 / 32,780), but without the decorations: drawcheck FAILs on the tree group and the land frames 600-900 differ 11-15% (trees missing; frames 1-550 0.9-4.0%).
* 2026-10-28 (builder LD1, D101): the four undefined land symbols placed (P-L6-2): ~LandPatchClass 0x474560, ~LandPatchRecordClass 0x4749D0 (both kept out of line by `#pragma auto_inline(off)`) and 0x477E20 exact, 0x93D6C0 is WW3D2's TexProjectClass::Peek_Material_Pass (the +0x1218 vector holds TexProjectClass; land +3 pairs). VIDTEST draws its land with LandClass (P-L6-1): vidland.* deleted, the trees in programs/vidtest/viddeco.* (VIDTEST's own code, a render object drawn after the land, by the land's patch records' visibility and level). The LOD table 0xB7188C has eight entries (VIDTEST's Land LOD 7 read past our six: every patch at level 0). drawcheck frame 900: ours 2,191 draws / 83,028 prims / 173,408 verts, the prims and verts the original's (the land group 212 / 32,780 exactly; the trees 1,344 draws of the original's 23,695 prims where it draws 340); sbs mean 2.6-2.8% (vidland 1.9-2.4%), frames 600-900 2.8-3.0% (vidland 1.5-2.4%); results.ini LandPolyBudget 28000, LandLOD 7 as before.
* 2026-10-28 (builder LD2): the lighting difference was VIDTEST's sun: Update_Lighting 0x4250B0 takes acos(sun.z) (0x5286A0 is the CRT's acos), so the record's SunAngle is the elevation; with asin LandClass shadowed 6,720 texels the original lights. The lightmap at frame 900 now equals the original's (dea5245). The trees come from the client's code: Create_Decorations 0x479D00 and Build_Decorations 0x47A2F0 (functional) fill the patch records Render draws; viddeco.* deleted. drawcheck frame 900: ours 1,187 / 83,028 / 173,408 in 50 groups, every group the original's (baseline updated); sbs mean 2.0-2.1%, frames 600-900 1.4-2.0%. The .dcb reader 0x4795D0, Load_Prototype_Names 0x478970 and Sample 0x463BF0 exact (Get_Height takes a Vector3); the .dcb writer 0x479A50 NOT EXACT (frame).
* 2026-10-28 (builder LD3): D120 (drawcheck, vidtest_sbs, run_vidtest default to `-art build/art`, tools/harness/artdir.py); D119's Land_Set_Ref back to the client's Create_Get / Create_NEW (0x474080 ambiguous, D66 (vi)); the ini writer 0x4791D0 (NOT EXACT frame: stack slots only); new app/client/mainview.cpp with the zone loader 0x5137F0 and the wind loader 0x513B90 byte-exact (app +4 pairs with two template copies). drawcheck PASS, sbs mean 1.2 %. The stars / orbital loader 0x513700 is exact but blocked by clienttest's fake main view (attempts.csv). Build_Decorations' RefCountPtr form tried (no gain: the client calls vector<RefCountPtr<MeshClass>>::insert(it, n, x) 0x47D690 out of line).
* Open: Build_Decorations / Create_Decorations byte-exact, 0x513700 (clienttest), the ini reader 0x478B50 / writer 0x4791D0 exact.

## 5. Proposals

Decided as D42-D48 (progress.md §2; P5 used the client's 2004 versions, e.g. `Land::Update_Lightmap_ManualThreaded` 0x46DA70 in both); the standing rules among them (P1 NOT EXACT marker, now D64; P3 stubs; P4 placement, D45/D67; P7 running) are in [builders.md](builders.md).

## 6. Order and pacing

Builders may run in parallel on disjoint items (D62), each verifying and committing its own byte-exact gains; the milestone checks run after each item. Both milestones are met; the original order was:

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
