# Progress, decisions and open questions

Newest first within each section. Dates are when the entry was written.

## 1. Log

### 2026-10-24 (overnight): Zero Hour's dated edits, D9 finished, wwlib's E&B allocator; 3,925 -> 4,062

Unattended run 00:02-07:40 PDT: a coordinator and one builder subagent at a time (no parallel
builds: `verify` shares `%TEMP%\enb_import` and the VMs' scratch), queue and log in
`.work/night/plan.md`, a heartbeat every 30 min, each builder's report in
`.work/night/item*/report.md`. 65 commits `2f6c80c..e88f4e9`, none pushed. Morning report:
[overnight-report-2.md](overnight-report-2.md).

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
| D2 | 2026-10-22 | Install Visual C++ 6.0 as a tool for proving our work (byte-for-byte comparison) | user. Installed: VC6 SP4 in Hyper-V VMs (three, 2026-10-23; since 2026-10-24 Win10 plus the pool, D30), the client's exact compiler build ([toolchain.md](toolchain.md) §1) |
| D3 | 2026-10-22 | The code builds with both VC6 and modern MSVC | user |
| D4 | 2026-10-22 | Import EA code file by file, as each file is verified against the client | user |
| D5 | 2026-10-22 | SDKs: DirectX 8 from github.com/edgeforce/directx8, Miles and Bink from github.com/edgeforce/radtools | user. Checked: DX8 is 8.0 as the client; Bink matches; Miles usable with two fixes ([toolchain.md](toolchain.md) §2) |
| D6 | 2026-10-23 | E&B's own code starts with `app\shared\auxdata`, then `app\wwudp`, then the task kernel (`app\kernel`: `KernelClass::Run_One_Cycle`, the loop running the current, background and pending-delete tasks; with `app\main` and `app\client\tasks`), then `app\packets` ([app-plan.md](app-plan.md) §3) | user |
| D7 | 2026-10-23 | E&B's own code is held to byte-exactness everywhere, like the libraries. Check in with the user periodically if this becomes too costly | user |
| D8 | 2026-10-23 | No hybrid client (reconstructed functions patched into the retail one) for now: its results would need the user to validate them. Open to it again if it can be shown to verify results better than byte matching, without the user | user |
| D9 | 2026-10-23 | E&B follows Zero Hour's KM/KJM (shader system) line through 08/01/02, without the 08/05/02+ texture redesign and without the Generals team's MW/GTH changes. So `TextureFilterClass` (08/05/02 KM) is dropped: `TextureClass` has Renegade's five filter members (texturefilter.cpp/.h `NOT_LINKED`) | user; overnight finding: Zero Hour's `dx8wrapper.obj` matches 12 client functions the tree lacks, and it unlocks wwshade. Reworded by the user 2026-10-23 after step 1 showed the client has the 06/27/02 abstraction but not the 08/05/02 redesign (library-matching.md §3.6.1), and again 2026-10-24 after the dated-comment sweep (`.work/night/item6i/report.md`: bytes place every dated ZH edit; MW 0/3, GTH 10/12 absent); filter members folded in by the user 2026-10-24 (all four constructors exact, 8f10486) |
| D10 | 2026-10-23 | E&B's unknown additions keep neutral names (`m_UnknownXX`, evidence in a comment) until the code that uses them is rebuilt | user |
| D11 | 2026-10-23 | Compiler-generated code (STL template instances): extend the corpus as each app file needs it, not ahead of the app work | user |
| D12 | 2026-10-23 | Compile each library with the flags E&B used, wherever byte evidence shows them (wwshade: `/GX /Ob1`; importer `LIB_FLAGS`, `bytematch_all.sh` `LIBFLAGS`) | user: "match how E&B built things whenever you have evidence to support it" |
| D13 | 2026-10-23 | EA files the client does not link are dropped from the tree (importer `NOT_LINKED`, with the evidence): first `shdlegacyw3d.cpp`/`.h`; then WWAudio's `AudioSaveLoad.cpp`/`.h` (2026-10-23) | user |
| D14 | 2026-10-23 | `ENABLE_WWPROFILE` in WWDebug too, although its bytes are neutral (verify: 67 before and after, none lost): consistent with a project-wide define. Soft: drop it if it ever costs a match | user |
| D15 | 2026-10-23 | `G_CODE_BASE` in WWAudio stays an importer patch removing its two `#ifdef`s, not a `/D` in `LIB_FLAGS` (same bytes) | user: only `WWAudio.cpp` uses it, so the bytes can't show a project-wide define |
| D16 | 2026-10-23 | E&B members whose use is now rebuilt (`m_Unknown148`, `m_Unknown164/168/16C`, `m_Unknown19A5/19A6`, ...) keep their D10 names, with the comments in the headers, until the app code shows E&B's names | user |
| D17 | 2026-10-23 | The two wwshade `"UnNamed"` constructors are left as near-misses; revisit only on new evidence | user |
| D19 | 2026-10-23 | E&B's screen-distortion module in WW3D2 (0x995D20-0x998200, between dx8texman.cpp and boxrobj.cpp; first described as a cube-map list) is rebuilt as `ww3d2/unknown_distortion.cpp`/`.h` (a reconstructed file), neutral names (D10) until E&B's own turn up; the function `WW3D::Set_Render_Device` calls is `Unknown_Reset_Distortion_List` | user (name chosen after the module turned out not to be about cube maps) |
| D18 | 2026-10-23 | Don't include `matrix4.h` first in `ww3d2/meshmatdesc.cpp` (it would win back `Make_UV_Array_Unique` 0x947310, but no EA copy has that order). Keep it on record until a better fix is found; revisit if it becomes WW3D2's last outstanding item | user; kept 2026-10-24 after item 4 showed the cause is symbol-count noise, not an inlining flip |
| D20 | 2026-10-24 | Losing a COMDAT-duplicate `(file, address)` pair, where the helper stays matched in another file, is not a loss. Name each such pair in the commit message | user (03:40) |
| D21 | 2026-10-24 | `dx8webbrowser.cpp`'s type library: `BrowserEngine.tlh`/`.tli` generated by VC6 from E&B's own `BrowserEngine.dll` are committed as reconstructed files, `#include` instead of `#import` (original kept as a comment); no binaries in the repository (option A) | user (04:40); done 761d6ac |
| D22 | 2026-10-24 | Import Zero Hour's shd6/shd7/shd8 bump shaders into wwshade, with the shader headers rebuilt as text from the client as reconstructed files (PS) | user (06:25); not yet done |
| D23 | 2026-10-24 | E&B's own files enter the importer as a non-EA library `app` (all reconstructed, `/GX`, `NDEBUG` off, verify limited to the app region) | user (06:25); not yet done |
| D24 | 2026-10-24 | App classes the client doesn't name get Westwood `XxxClass` names (`BitSetClass`) | user (06:25); conventions.md §1 |
| D25 | 2026-10-24 | App asserts sit on their real `__LINE__` through the file layout (blank lines, comments) in tree files; `#line` only in drafts | user (06:38) |
| D26 | 2026-10-24 | `USE_WWSHADE`: WW3D2 built with it (under D12, evidence: the `SHD_*` wrapper calls); wwshade too, as a soft consistency setting like D14 (neutral in the bytes) | user (03:47, 06:25); done 4ae710d, 7ecb645 |
| D27 | 2026-10-24 | Verified function starts (decoded call targets) are the default for every tool | user (03:47); done 78b0a74 |
| D28 | 2026-10-24 | Keep ww3d.h's unused Renegade `MinStaticSortLevel`/`MaxStaticSortLevel` declarations (noise compensation for meshmatdesc after fdd0a81, not evidence); revisit when the noise is understood | user (06:25) |
| D29 | 2026-10-24 | Keep 62b895b's protected `D3DTexture` (needed by c35cc34, which writes it as 0x94B660 does); patch reasons reworded | user (06:38); done e88f4e9 |
| D30 | 2026-10-24 | Build VMs come from a pool of differencing clones of `Tools\VMPool\base-vc6.vhdx`, no warm sync cache (each leasee syncs its tree), cap 5; the old fixed clones are retired | user (04:40, 06:25); done 87f38c3, c7b8bcc; clones retired 07:00 ([toolchain.md](toolchain.md) §1.3) |
| D31 | 2026-10-24 | Sessions release their leased VMs in the handoff skill; `pool.py lease`/`create` reclaim orphaned leases first | user (06:25); done 6e298b5 |
| D32 | 2026-10-24 | Builders stay serial (one at a time); parallel worktrees not needed yet | user |
| D33 | 2026-10-24 | Keep the neutral names from the night's item 7b: `MEM_CLIENTOBJECT`..`MEM_PARTICLES` (from the client's name table 0xBCB228), `FastAllocatorGeneral::m_UnknownE00`..`m_UnknownE20`, `Unknown_Reset_Statistics` (D10) | user |
| D34 | 2026-10-24 | Keep evidence-backed changes that gain nothing countable when the client's form is shown: first `DazzlePrototypeClass`'s Renegade virtual destructor (0x96EF00 has the client's form but is too short to count; vtable 0xB12598 has 4 slots) | user |
| D35 | 2026-10-24 | texture.h's comment on the protected `D3DTexture` cites 0x94B660 (c35cc34), not `Apply` 0x94BC80 (D29) | user |
| D36 | 2026-10-24 | VIDTEST.EXE is rebuilt as a working program, not byte-exact: proof that the tree links into a small app that loads E&B's original art and renders a 3D scene (the 2002 library variant and incremental link aren't needed for that). E&BConfig.exe not pursued | user |

## 3. Open decisions

* (D7 is to be revisited with the user if byte-exactness of E&B's code becomes too costly.)

## 4. Next steps

1. D22 (PS): Zero Hour's shd6/shd7/shd8 bump shaders into wwshade, +19 (0xA0FF60..0xA15580,
   `.work/night/item6i/shdtry.py`). Needs a class-id-less `ShdInterfaceClass(def)`, shd8's
   `Setup_Self_Shadow_Info` calls `Peek_Mapper` (ZH lacks it), and the shader headers rebuilt as
   `char[]` text from the client (from 0x7CB8FE) as reconstructed files.
2. D23: the importer's non-EA library `app`; move `bitset.cpp` (12/12 exact draft in
   `.work/night/item7/try/`) and its header into the tree under D24/D25; then `lookup.cpp` (6
   functions, all with asserts) or `delta_money`/`delta_time` (need `DeltaBase`'s layout: ctor
   0x4A4D40; the reference decoder's docs/research/auxdata.md §3, [prior-work.md](prior-work.md)). Tool: `tools/re/appfile.py`.
3. `WW3D::Make_Screen_Shot` 0x9182C0: Zero Hour's gamma/format plus E&B's JPG case through a wwlib
   JPEG writer (ctor 0x9E3850, dtor 0x9E3860, save 0x9E3870; class after srandom.cpp); 0.85, only
   register scheduling left (`.work/night/item7b/v_mss.py`, `run_mss.py`).
4. D19's distortion draw 0x9961E0 (369 masked rows): the client builds 6 `??_H` Transpose temps
   with out-of-line `Vector4()`, ours 5; look for missing inline calls/temporaries earlier in the
   function (fits the 0x18-bigger frame).
5. `PointGroupClass::Render` 0x954960 (slot shuffle in inlined Transpose temps); the meshmatdesc
   noise mechanism (then drop D28's declarations and revisit D18).
   Other WW3D2 leads: `Process_Foreground_Load` 0x949C60 (inline budget), `Compute_Caps` 0x9B0160,
   `Update_Movie_Capture` 0x918B20, `Create_Device`'s format retry, `CreateBrowser`'s app callers
   0x7CA5C0/0x7CAD00 (E&B's `querydata`). wwshade: the two `"UnNamed"` constructors (D17).
6. Tools: `--pool K` for `bytematch_all.sh` and `importer.py refine`.
7. D36, a working VIDTEST: link the tree's libraries into an executable for the first time (a
   link step the project hasn't done; the client's link settings in [binary.md](binary.md)), with a
   new small app (WinMain, device setup through `WW3D::Init`/`Set_Render_Device`, an asset manager
   pointed at `EBCONFIG`'s art, a camera and a scene) that loads E&B's W3D/DDS/TGA files and renders
   them. VIDTEST's own behaviour is the spec (`.work/ebconfig/assessment.md` §1: command line
   `-device -resx -resy -depth -windowed -filter`, `results.ini` output); its E&B game classes
   (Land, stars, decorations, splines) are optional extras, not needed for the first scene.
