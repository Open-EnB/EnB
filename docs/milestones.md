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
| **M1.0** Inputs | **Mostly done already** in the reference decoder: the message-level inputs are the `ea\*.trace.jsonl` imports above, and the checks exist (`the reference decoder roundtrip`; `aux-census`: all 43,978 decode, re-encode and match the typed model; `aux-log --first`: every field value and change, the reference for (c)). Left: (1) a datagram-level file for gate (a): `tools/harness/convert.py` reads the Alex dumps (`session: <id> rc4 key: <8 bytes>`, then `C->S`/`S->C seq <n> len <hex>` and the decrypted datagram, checksum first), the same parse as `EnB.Trace/Ea/the reference importer.cs`, which keeps only the messages; (2) save `aux-census` and `aux-log --first` output per capture under `.work/harness/ref/`; (3) the harness skeleton reads trace.jsonl and the datagram file and counts messages | counts equal the reference decoder's | small |
| **M1.1** Decode path | Map the receive path from `AuxDataPacketClass::Process_Packet` 0x7364C0 to the leaves, and list every function on it that isn't byte-exact yet (encode.cpp's codecs 0x4A9400 / 0x4A8FD0 / 0x4A9710, the rest of the leaf types: item arrays, interpolators, records, policy where the decoders call it) | a list in this file, each function with its status | small |
| **M1.2** auxdata completion | The functions from M1.1, byte-exact where possible and functional where not (P1). This includes the remaining items in progress.md §4 step 2: the time field, the encoder `Add` functions, kv_pair_string, kv_list, the encode.cpp and policy.cpp rules | harness (b) passes on a hand-built root | large |
| **M1.3** Root schemas | The 13 root classes' constructors. A generator turns the extracted schema into C++ in the client's form (one `Add` per node, in order), and verify checks the result against the client (P2) | (b) and (c) pass on the EA captures | medium |
| **M1.4** wwudp | `CordMgr`, `CordCon`, bandwidth accounting: handshake (SYN1/ACK1/SYN2/ACK2), checksum, block types, fragments, sequence and resend handling, ack lists, close. Socket and timer behind the class's own interface, replaced by a replay socket in the harness | harness (a) passes | large |
| **M1.5** Net-7 recordings | Message-level replay of the TCP recordings, plus (d). Whether the client's TCP transport is also in `app\wwudp` is checked here (P6) | (d) passes | small |

The order is M1.0, then M1.1, then M1.2 and M1.3 (they can overlap), then M1.4, then M1.5.
This follows D6 (auxdata, then wwudp). Items M1.4 and M1.5 also exercise WWLib's `RC4Class`
and `PKey` at run time for the first time.

## 3. The link step (shared)

| Item | Content | Done when |
|---|---|---|
| **L1** Libraries | `tools/vc6/build.py`: compile each library completely, every file and not only the ones that match, with its verify flags, then `LIB` it into `wwlib.lib`, `ww3d2.lib`, and so on, plus `app.lib`. Runs on a pool VM (`--pool`); the output goes to `build/` (git-ignored) | eight `.lib` files |
| **L2** First executable | Link a minimal WinMain against them with LIBCMT, `d3dx8.lib`, winmm, ws2_32 and the rest. Fix the unresolved externals: NOT_LINKED files, declared-only functions (D28-type declarations), E&B hooks the libraries call back into. Each fix is either a real function or a stub under `stubs/` that is listed and logs when it is called (P3) | a linked `.exe` that starts, `WW3D::Init` succeeds, and the window clears to a colour |

L2's stub list doubles as a to-do list: each stub marks a function the client has and we haven't
written yet.

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
