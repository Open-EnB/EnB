# Overnight report, 2026-10-23

Unattended session from 01:20 to about 04:40 PDT, following the plan agreed beforehand: five blocks
(WW3D2 layouts and vtables; the smaller libraries' gaps; a symbol map; a survey of E&B's own code
with a proposal; docs). Work was committed locally, step by step. Nothing was pushed, and nothing
outside `.work/` was deleted. Each kept change was checked with `importer.py verify` on the
affected libraries before it was committed, and a final full verify reproduced every count below.

## 1. Result

Client functions the repository reproduces byte for byte:

| Library | Start of night | End of night | Gained |
|---|---|---|---|
| WW3D2 | 1,862 | 2,060 | +198 |
| WWAudio | 184 | 264 | +80 |
| WWLib | 553 | 557 | +4 |
| WWMath | 367 | 367 | 0 |
| WWSaveLoad | 80 | 80 | 0 |
| WWDebug | 67 | 67 | 0 |
| wwshade | 8 | 13 | +5 |
| **Total** | **3,121** | **3,408** | **+287** |

With the out-of-scope runtime libraries (docs/library-matching.md §3.7), docs/coverage.md now
names 4,707 of the 11,343 functions in the library region (about 975 KB of code), and 256 in E&B's
own region.

## 2. What was done

**Block 1, WW3D2 and WWAudio.** docs/library-matching.md §3.6.1 lists every fix with its evidence.

* The three build VMs are now addressed by IP and used in parallel. The clones share one computer
  name, so addressing them by name had sent compiles to clones with stale files.
* `tools/re/vtables.py` finds each compiled vtable in the client and aligns the slots. Fixes it
  found:
  * `RenderObjClass`: Renegade's four user-lighting virtuals are not virtual in E&B, and Zero
    Hour's `Remove_Sub_Objects_From_Bone(int)` is there (+143 in total).
  * VC6 lists overloaded virtuals in reverse declaration order. Not knowing that led to a wrong
    guess earlier in the night, an unknown E&B virtual at slot 40. It was corrected the same
    night.
* `RenderInfoClass` has Zero Hour's `Texture_Projector` (+14).
* Particle system: Zero Hour's emitter, plus E&B's own future-start field and file flag, and Zero
  Hour's line groups in the particle buffer.
* Taken from Zero Hour: rings, spheres, dazzle, scene IDs, the `DX8Caps` flags, the grid mapper's
  `Offset`, the Screen sort category and the shader class ID. `LightEnvironmentClass` stays
  Renegade's. Six more files came from Zero Hour by batch trials.
* WWAudio: the Miles API is declared `dllimport` (+30), and E&B's extra fields are added to
  `AudibleSoundClass` and `Sound3DClass` (+50).

**Block 2, the smaller libraries.**

* Gaps are classified as linked-but-different or probably unlinked (docs/data/gaps-*-classified.md).
* E&B's `Pick_Random_Number` is reconstructed: it works unsigned and guards the full-range mask
  (+4 WWLib).

**Block 3, the symbol map.**

* docs/data/symbols.json: 6,100 symbols, each recorded with how it was proven.
* docs/data/ghidra_symbols.py labels a Ghidra project.
* docs/coverage.md reports coverage.

**Block 4, E&B's own code.**

* docs/app-inventory.md: 18,690 functions, of which 5,898 are attributed to their source file.
* docs/app-plan.md: a proposal only, with decisions D6–D8 for you.
* A corpus of VC6 standard-library instantiations (tools/stl) accounts for about 340 app functions
  with certainty, and up to 2,900 counting short ones. It also confirms the app code was compiled
  with `/GX`.

**Block 5, docs.** Updated progress.md (log, open decisions, next steps), library-matching.md,
the tool and docs READMEs, and project memory. Wrote this report.

## 3. Problems found and how they were handled

* **The refine tool accepted lossy swaps.** Refine took Zero Hour's `shader.cpp` for 4 functions
  gained and 3 lost. I reverted it and fixed the tool: a whole-file or header swap must now keep
  every function that matched before. The 2 functions that change actually added came in by a
  function-level splice instead.
* **One accepted loss, documented.** The grid-mapper fix (+4) costs 2 functions in
  `meshmatdesc.cpp`. With some header changes, VC6 inlines a small app-region helper (0x4E9CB0)
  into `Install_UV_Array`, which the client calls instead. Zero Hour's `shader.h` causes the same
  flip. I don't understand why yet.
  *Correction (2026-10-24):* 0x4E9CB0 is E&B's `::operator new`
  (`FastAllocatorGeneral::Get_Allocator()->Alloc`) and is never inlined. The three functions
  differ in index load order, which moves with the number of symbols declared before
  meshmatdesc.h's classes (noise). The documented loss is now only 0x947310 (D18);
  see workflow.md §3.
* **The tree can't yet be rebuilt from the EA sources.** `importer.py assemble` is the
  pipeline's first stage: it restarts every library from its base copy. Nothing replays the
  recorded decisions (copies, header swaps, splices, patches) on top. Tonight's trial showed 49
  WW3D2 files differing from git (the tree was restored at once). Until a replay step exists, the
  tree in git is the authoritative result. This is next step 1 in progress.md.

## 4. Not done, and why

* **The texture system.** Zero Hour's `dx8wrapper.obj` matches 12 client functions that our tree
  lacks: vertex/pixel shader state, Z textures, and render targets with depth buffers. So E&B's
  `DX8Wrapper` and texture classes follow Zero Hour's. Moving WW3D2's texture system over is the
  largest remaining step. It would unlock those 12 functions, `TexProjectClass` (whose client
  destructor releases Zero Hour's `DepthStencilTarget`) and most of wwshade. A dry-run trial
  touches dozens of files written against Renegade's texture API, so I didn't start it without
  you.
* **wwshade** is at 13 of 46 and waits on the texture system above.
* **Code differences, not layouts.** The remaining WW3D2 near-misses (`WW3DAssetManager`,
  `SortingRendererClass`, `MeshClass`) and most of the WWAudio gaps (`WWAudioClass` 40,
  `SoundSceneClass` 12) differ in code rather than in member layout.

## 5. Questions for you

1. **D6–D8 in docs/app-plan.md.**
   * Where should work on E&B's own code start? The proposal is `auxdata`, then `wwudp`, the
     kernel and tasks, then packets.
   * Should E&B's code be held to byte-exactness everywhere, or only for protocol and game-state
     code?
   * Should we aim early for a hybrid, with reconstructed functions patched into the retail
     client so each can be tested live?
2. **Texture system.** Should I move WW3D2's texture classes and `DX8Wrapper` to Zero Hour's next
   (several hours, one evidence-checked step at a time), or leave it until later?
3. **Unknown names.** E&B's additions are named neutrally (`m_Unknown7C`, `m_Unknown168`,
   `UnknownFlag`, ...), with their evidence in comments. Should I try to name them from how the app
   code uses them, or leave them until that code is rebuilt?
4. **Replay step.** Shall I build `importer.py replay` before anything else, so the tree's
   provenance can be checked end to end?
5. **Compiler-generated code.** Extend the STL corpus until every template instance in the app
   region is named, either before the app work starts or as each app file needs it?

## 6. Where things are

* Commits: `git log --oneline e1c6cb9^..HEAD`. Nothing was pushed.
* Results: docs/imports.md (per file), docs/coverage.md, docs/data/symbols.json.
* Working files: `.work/` (git-ignored), which holds logs, evidence and the variant experiments.
