# Overnight report, 2026-10-24

Unattended run from 00:02 to about 07:40, as agreed beforehand: a coordinator
and one builder subagent at a time (no parallel builds, since `verify` shares
`%TEMP%\enb_import` and the VMs' scratch), a heartbeat every 30 minutes, each verified gain that
loses nothing committed at once, never pushed, never `assemble`. The queue, the log and every
ruling you gave during the night are in `.work/night/plan.md`; each builder wrote
`.work/night/item*/report.md`. 65 commits (`git log --oneline b9b0c61..HEAD`).

## 1. Result

Client functions the repository reproduces byte for byte (in-tree counts):

| Library | Start of night | End of night | Gained |
|---|---|---|---|
| WW3D2 | 2,362 | 2,477 | +115 |
| WWAudio | 351 | 351 | 0 |
| WWLib | 557 | 565 | +8 |
| WWMath | 370 | 370 | 0 |
| WWSaveLoad | 80 | 80 | 0 |
| WWDebug | 67 | 81 | +14 |
| wwshade | 138 | 138 | 0 |
| **Total** | **3,925** | **4,062** | **+137** |

Against the night's starting pairs (`.work/night/imports.start.json`), the only lost
`(file, address)` pairs are COMDAT duplicates whose helper is still matched in another file
(your ruling D20): (animatedsoundmgr, 0x6CF410), (meshmatdesc, 0x497330), (meshmdl, 0x909500),
(ww3d, 0x909500), (texturefilter, 0x42F250/0x53F1C0/0x9217E0; the file is dropped and 0x94CA30
moved to texture), (wwmemlog and wwprofile, 0x9CEFF0).

## 2. What was done

| Item | Result |
|---|---|
| 1. D9 texture leftovers | +10: `Get_Reduction`, E&B's `TextureClass::Apply`, `Get_Texture_Memory_Usage` (Generals' split body), `Set_Mip_Mapping` in texture.cpp |
| 2. `DX8Wrapper` device code | +25: `USE_WWSHADE` for WW3D2, `Reset_Device`, `Unknown_Restore_Device` 0x921C50, transform cache, `Init` |
| 3. `GapFillerClass` | +3: Zero Hour's DMS fix (no `Add_Ref` on the mesh model) |
| 4. meshmatdesc "inlining flip" | 0: the premise was wrong (see §3) |
| 5. Distortion leftovers | +3: 0x995E20, 0x996180, 0x997DE0 (linkage, `expf`); the draw 0x9961E0 is still 369 rows off |
| 6. gaps.py false starts | tool fix: gaps at mid-instruction starts 5 -> 0 |
| 6b. Near-miss sweep for Zero Hour-only edits | +26 in 19 commits (animobj, scenes, sorting, texture loader, DDS, dx8caps, text) |
| 6c. `Post_Load_Process` | Zero Hour's body exact; committed in 6e after D20 |
| 6d. 6b's leads | +10 (`Alter_Avatar_HTree`, `ShaderClass::Apply`, `post_process`...); new tool `zhall.py` |
| 6i. Dated-comment sweep | +1; D9's cutoff measured (§4); proposal PS |
| 6j. Importer lock | `.work/locks/importer.lock` for every importer command (fa14686) |
| 6e. Specs allowed by D20 | +11: WW3D sort lists and `SHD_FLUSH`, animated sound parsing, `Post_Load_Process`, `MeshModelClass::operator=`, `WW3D::Init` |
| 6k. `dx8webbrowser.cpp` (D21) | +19, type library headers generated from E&B's own DLL |
| 6f. `TextureFilterClass` -> Renegade's filter members (D9) | +4, all four `TextureClass` constructors; texturefilter.cpp/.h dropped |
| 6g. `USE_WWSHADE` in wwshade | neutral; committed later as a soft setting (d2955f0, D26) |
| 6h. Verified function starts by default | 5d7079f; symbols 6,736 -> 6,858, gaps rows 652 -> 593 |
| 7. Per-file tool for E&B's own code | `tools/re/appfile.py` (c4d34c9); `bitset.cpp` 12/12 exact as a draft, not in the tree |
| 7b. Near-misses, round 2 | WW3D2 +3, WWDebug +14, WWLib +8: `Create_Device`, DDS ctor, `Release_Textures`; the memory log's 23 categories; E&B's 0xE30-byte `FastAllocatorGeneral` with out-of-line `Alloc`/`Free`/`Unknown_Reset_Statistics` |
| 8a. Tool and wording | `verify --pool K` (4b1f45b), `USE_WWSHADE` in wwshade (d2955f0), patch reasons reworded (a1cbc31) |
| Pool (separate agent) | `tools/vc6/pool.py` (b156da0), orphan reclaim and handoff-skill release step (92ca150) |
| Assessment | E&BConfig.exe / VIDTEST.EXE rebuildability, `.work/ebconfig/assessment.md` (§5) |

Each fix's evidence is in its commit message; the log entry in progress.md lists the addresses.

## 3. Problems found and how they were handled

* **The meshmatdesc inlining flip did not exist.** 0x4E9CB0 is E&B's `::operator new`, never
  inlined. The three fragile functions differ in index load order, which moves with the number
  of symbols declared before the classes. Docs corrected (workflow.md §3, the old report's §3
  with a dated note); only 0x947310 is lost (D18).
* **Gains that dropped an unused helper lost duplicate pairs.** Builders reverted them and asked;
  your ruling (D20) let 6e apply them, each duplicate named in its commit message.
* **WW3D's sort lists (694a02b) cost real meshmatdesc pairs** through the same symbol-count
  noise. Kept the unused Renegade `MinStaticSortLevel`/`MaxStaticSortLevel` declarations in
  ww3d.h to compensate (D28, your ruling): noise compensation, not evidence.
* **gaps.py located functions at mid-instruction addresses** (E8 bytes inside other
  instructions): 216 of 30,033 starts were false. Fixed for gaps.py, then for every tool (D27).
  Its pairings can still be wrong (neighbours swapped, template dtors): builders confirmed pairs
  with objdiff.
* **Replay failures from patch anchors** that a later importer block rewrites; a chained commit
  went through after "1 problems" and was amended. Rule added: put such patches in the later
  block, stop on a replay failure.
* **A killed `zhall.py` left a spliced wwlib/ini.cpp and the lock behind**; restored from git,
  lock deleted.
* **Concurrent tools could collide** on the verify scratch: the importer lock (6j) now
  serialises them.

All new traps are in workflow.md §3.

## 4. Decisions taken during the night

All yours, recorded in progress.md §2:

* D20, COMDAT-duplicate pairs still matched elsewhere are not losses.
* `TextureFilterClass` -> Renegade's filter members (folded into D9);
  `USE_WWSHADE` for WW3D2 (D26, under D12); verified function starts by default (D27); D18 kept.
* 6i first, then 6e-7; the importer lock.
* PW option A (D21); the VM pool without a warm sync cache (D30); build the pool.
* D9 reworded ("E&B follows Zero Hour's KM/KJM (shader system) line through 08/01/02,
  without the 08/05/02+ texture redesign and without the Generals team's MW/GTH changes"); PS
  (D22); app files as a non-EA library `app` (D23); `XxxClass` names for app classes (D24); keep
  the ww3d.h declarations (D28); `USE_WWSHADE` in wwshade, soft like D14 (D26); retire the two old
  clones; `verify --pool K`; sessions release their VMs and the pool reclaims orphans (D31);
  cap 5.
* App asserts on their real `__LINE__` lines through the file layout, `#line` only in
  drafts (D25); keep 8f5cbfe and reword its reason (D29); reword the four outdated
  `TextureFilterClass` patch reasons.
* Builders stay serial for now (D32).

## 5. Questions for you

Also in progress.md §3.

1. **Neutral names from 7b** (`MEM_CLIENTOBJECT`..`MEM_PARTICLES`, `m_UnknownE00`..`E20`,
   `Unknown_Reset_Statistics`): keep them under D10? Recommended: yes.
2. **Dazzle destructor** (0x96EF00): the client's form, but too short to count (+0, reverted;
   `.work/night/item7b/spec_dazzle.py`). Keep it as evidence? Recommended: yes.
3. **texture.h's comment** still cites `Apply` 0x94BC80 as the reason `D3DTexture` is protected,
   while the real need is 5534f64 / 0x94B660 (D29). Leave it, or reword it with the next texture
   change?
4. **VIDTEST.EXE / E&BConfig.exe.** Neither is a quick win. Both are September 2002 builds, so
   they need a second variant of the libraries (~650-950 functions differ or are unlinked: a
   second patch set or a branch). VIDTEST uses the client's toolchain but needs ~437 app functions
   (Land, stars, decorations, splines, test harness) at their 2002 version. E&BConfig's app is
   tiny but needs a VC6 SP5 VM, static MFC 4.2 and WWAudio at its 2002 version. Recommended: not
   now; if ever, VIDTEST first, after a cheap stub-link proof (progress.md §4 item 7). Side
   finding: the client's 96 Rich product-id-4 objects are the Miles/Bink import libraries
   (toolchain.md §1.2).

## 6. Where things are

* Commits: `git log --oneline b9b0c61..HEAD` (65). Nothing was pushed.
* Next steps: progress.md §4 (first: PS, +19 wwshade; then the `app` library with `bitset.cpp`).
* Night state: `.work/night/plan.md`, `.work/night/item*/report.md` and their scripts and specs;
  `.work/ebconfig/` for the assessment.
* Build VMs: Win10 is the only fixed host (`.work/vc6_hosts.json`); pool VMs
  `enb-vc6-01`/`02` are stopped and free (`python tools/vc6/pool.py list`). The two old clones
  are gone.
