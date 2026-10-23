# Progress, decisions and open questions

Newest first within each section. Dates are when the entry was written.

## 1. Log

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
* Outside git: `.work/ws_*.py` (`ws_addpatch.py` records into `WWSHADE_API_PATCHES`,
  `ws_spec*.py` the patches, `ws_try.py` one-file variants, `ws_verify.sh`, `ws_dis.py`/
  `ws_refs.py`/`ws_callers.py`/`ws_findptr.py` client lookups), baseline
  `.work/ws_base_imports.json`.

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

Unattended session, 01:20-04:40, plan agreed beforehand; report: [overnight-report.md](overnight-report.md).

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
| D2 | 2026-10-22 | Install Visual C++ 6.0 as a tool for proving our work (byte-for-byte comparison) | user. Installed: VC6 SP4 in Hyper-V VMs (three, 2026-10-23), the client's exact compiler build ([toolchain.md](toolchain.md) §1) |
| D3 | 2026-10-22 | The code builds with both VC6 and modern MSVC | user |
| D4 | 2026-10-22 | Import EA code file by file, as each file is verified against the client | user |
| D5 | 2026-10-22 | SDKs: DirectX 8 from github.com/edgeforce/directx8, Miles and Bink from github.com/edgeforce/radtools | user. Checked: DX8 is 8.0 as the client; Bink matches; Miles usable with two fixes ([toolchain.md](toolchain.md) §2) |
| D6 | 2026-10-23 | E&B's own code starts with `app\shared\auxdata`, then `app\wwudp`, then the task kernel (`app\kernel`: `KernelClass::Run_One_Cycle`, the loop running the current, background and pending-delete tasks; with `app\main` and `app\client\tasks`), then `app\packets` ([app-plan.md](app-plan.md) §3) | user |
| D7 | 2026-10-23 | E&B's own code is held to byte-exactness everywhere, like the libraries. Check in with the user periodically if this becomes too costly | user |
| D8 | 2026-10-23 | No hybrid client (reconstructed functions patched into the retail one) for now: its results would need the user to validate them. Open to it again if it can be shown to verify results better than byte matching, without the user | user |
| D9 | 2026-10-23 | Move WW3D2's texture classes and `DX8Wrapper` to Zero Hour's as of 06/27/02, minus its later redesign (08/05/02: cube/volume textures, LOD reduction, virtual load tasks), one evidence-checked step at a time | user; overnight finding: Zero Hour's `dx8wrapper.obj` matches 12 client functions the tree lacks, and it unlocks wwshade. Reworded by the user 2026-10-23 after step 1 showed the client has the 06/27/02 abstraction but not the 08/05/02 redesign (library-matching.md §3.6.1) |
| D10 | 2026-10-23 | E&B's unknown additions keep neutral names (`m_UnknownXX`, evidence in a comment) until the code that uses them is rebuilt | user |
| D11 | 2026-10-23 | Compiler-generated code (STL template instances): extend the corpus as each app file needs it, not ahead of the app work | user |

## 3. Open decisions

* None. (D7 is to be revisited with the user if byte-exactness of E&B's code becomes too costly.)

## 4. Next steps

1. D9 continued: wwshade (56 in the tree; next, the client's older `ShdSubMeshClass` and renderer node, see the 2026-10-23 step 2 entry), then the texture near-misses (`TextureClass` constructors, `Init`, `Apply`, `Get_Reduction`; `DX8Wrapper`'s device code). Texture classes done; wwshade's WW3D2 features done (2026-10-23).
2. The remaining WWAudio gaps (`WWAudioClass` 40, `SoundSceneClass` 12; `SoundPseudo3DClass` and
   `FilteredSoundClass` +4 from 0x1A4) and WW3D2 near-misses (docs/data/gaps-*.md).
3. `meshmatdesc.cpp`'s inlining flip (an app-region helper at 0x4E9CB0 that VC6 now inlines).
4. E&B's own code (D6, D7): first the per-file workflow script ([app-plan.md](app-plan.md) §2),
   then `app\shared\auxdata`.
