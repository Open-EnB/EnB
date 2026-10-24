# Working on the libraries: workflow and known traps

How the library work is done day to day, and what has gone wrong before. Written for whoever
picks the work up next. The tools themselves are listed in tools/re/README.md; the VMs are
described in docs/toolchain.md §1.

## 1. Working state outside git

| What | Where | How to recreate |
|---|---|---|
| Byte-match evidence for every EA copy (R/G/Z), needed by `importer.py refine` and `assemble` (`--evidence`) | `.work/bm2` (git-ignored) | `tools/vc6/bytematch_all.sh` (all three copies, each against its own libraries; `RAD` points at `SDK\radtools\include_enb`, written by `tools/vc6/sdk_patch.py`) |
| The tree's own compiled objects, read by `gaps.py`, `layoutdiff.py`, `vtables.py`, `symbols.py` | `%TEMP%\enb_import\verify-<lib>` | `python tools/vc6/importer.py verify` |
| The build VMs' addresses | `.work/vc6_hosts.json` | `python tools/vc6/vc6.py discover` (after the VMs restart: DHCP) |
| Scratch scripts, backups of `importer.py` and `imports.json`, logs | `.work/` | disposable |
| The importer lock: one run at a time of `importer.py` (every command), `trial.py`, `tryvariants.py`, `trydiff.py` (and anything calling `importer.compile_and_match`/`save_state`), held until the process exits; JSON with agent (`ENB_AGENT`, else user@host), pid, command, start. A second run prints `importer in use by ...` and waits `--lock-wait` s (env `ENB_LOCK_WAIT`, default 600), then exits 1; a dead holder (same host) or one older than 4 h is taken over | `.work/locks/importer.lock` | nothing to do: removed at exit; if a run dies it is taken over by the next one (delete it by hand only if no importer is running) |

`.work/` holds nothing that the repository depends on. Every kept change lives in the tree and
in `tools/vc6/importer.py` (`PATCHES`, `APPENDS`, `NEW_FILES`, `RECONSTRUCTED`).

## 2. The loop for one fix

1. **Find it.**
   * `python tools/re/gaps.py --title ww3d2 --out docs/data/gaps-ww3d2 %TEMP%\enb_import\verify-ww3d2\*.obj`
     locates the functions that aren't byte-exact.
   * `layoutdiff.py --gaps ... --objs ...` ranks classes by consistent member-offset shifts.
   * `vtables.py --objs ... [--class C]` aligns vtables.
   * `objdiff.py OBJ SYMBOL ADDRESS` shows one function's instruction-level difference.
   * `tools/vc6/layout.py CLASS HEADER` prints a class's offsets as VC6 lays it out.
2. **Find the evidence in the client.** Usually that's a constructor: it initialises members in
   declaration order, so it shows the true layout. Then look for the EA copy that has the member
   or virtual. Unknown E&B additions get neutral names (`m_UnknownXX`), with the evidence in a
   comment.
3. **Write it as a patch** (old text, new text, reason with the client address) in a spec file in
   `.work/`, and apply it with `tools/vc6/addpatch.py --block NAME SPEC.py`: it applies it to the
   tree file and records it in that `importer.py` block (the current block for wwshade's WW3D2:
   `WWSHADE_API_PATCHES`). Write the spec with a file tool, not a shell heredoc: escapes such as
   `\t`, `\r\n` and `\x` get mangled by the shell. Every anchor must occur exactly once. To find
   one function's form quickly, try variants first: `tools/vc6/tryvariants.py LIB FILE V.py`.
4. **Check.**
   * Save `docs/data/imports.json` first.
   * Run `importer.py verify --lib ...` for every library that includes the changed header.
     WWLib headers reach every library. Add `--pool K` to build on K VMs leased from
     `tools/vc6/pool.py` instead of the three fixed ones (released at the end; toolchain.md §1.3).
   * Compare the sets of `(file, address)` pairs before and after. Compare sets, not totals:
     a total can stay level while functions are traded.
5. **Keep or drop.**
   * Keep the change only if it gains and loses nothing. Any accepted loss must be written down,
     in the commit message and in docs/library-matching.md.
   * Then run `importer.py report` and `importer.py replay` (it must report 0 differences),
     commit, and add a row to docs/library-matching.md §3.6.1.
6. **Then refine.** `importer.py refine --evidence .work/bm2 --lib L --file F.cpp` often
   splices in the other copy's functions once the header fits.

Counting: the "in tree" totals that verify prints count each client function once. The
`(file, address)` pairs in `imports.json` also include app-region duplicates (inline functions
compiled into E&B's objects), so they run higher.

## 3. Known traps

* **`assemble` is not a rebuild; `replay` is.** `importer.py assemble` restarts every library
  from its base copy and drops every later decision. Don't run it on a tree with work in it; if
  you do, `git checkout -- <lib>` restores it. `importer.py replay` rebuilds the libraries in
  `.work/replay` from the EA copies plus what `imports.json` records, then compares them with
  the repository file by file (no VM needed, a few seconds). Run it before committing: a
  difference means a decision isn't recorded (or is recorded wrongly), and the record must be
  fixed, not the tree.
* **Header origins live in one place.** In each library's `headers` map in `imports.json`. A
  file's `headers_from` only says that its header was swapped along with it. `trial.py` and
  refine's header swaps update both; a hand-made header change must update `headers` too.
* **Refine checks only the file it refines.** A header swap is checked across every library, but
  a function splice is not. Run a verify of the affected libraries before committing.
* **VC6 lists overloaded virtuals in reverse declaration order.** Read the client's slots with
  that in mind. Not knowing it produced a wrong "unknown virtual" once, corrected the same night.
* **`vtables.py` and classes whose own code doesn't match yet.** Inherited slots alone don't
  identify a table: the tool now says "not placed" (or warns "only inherited slots") instead
  of reporting the base class's table (it used to put such classes on `RenderObjClass`'s
  0xAE84F8). Seed it with any method's client address: `--seed Class=ADDR` (`client.py refs`
  finds the table holding a known method). Tables now end at the next table the code stores,
  so the old "16 extra slots" over-read is gone. Run it after regenerating `symbols.json`
  (`symbols.py`): its anchors come from there too.
* **`gaps.py` can misplace a whole object.** It anchors on matched neighbours. If the only match
  is a shared helper (`??_H` vector iterators), the object's gaps land in the wrong region. This
  happened with `rinfo.obj`, placed in the app region: find the real code by an instruction
  pattern instead.
* **`gaps.py` can pair overloads wrongly.** For example, the default constructor was paired with
  the client's copy constructor. Check the argument count (`ret N`, `[esp+N]`) before trusting a
  diff.
* **Inlining is fragile in some files.** `meshmatdesc.cpp` flips VC6's automatic inlining of an
  app-region helper (0x4E9CB0) when unrelated headers change: the documented loss in
  library-matching.md.
* **Patch anchors change when files change copy.** A `PATCHES` entry written for Zero Hour's file
  doesn't apply to Renegade's. `assemble` reports "patch ... no longer applies".
* **`verify` keeps only the first 50 compile errors.** One badly broken file (e.g. a whole-file
  swap) fills them and hides errors elsewhere: a library losing functions with no error listed
  for that file usually means this. Fix the loud file first, or verify with it restored.
* **A "none" for a callee proves nothing.** Byte matching masks call targets, so a function can
  be absent from the evidence and still be what the client calls. Before patching a call site
  to avoid a function, follow the client's call (it was so with Zero Hour's
  `Peek_Thumbnail_Instance_From_Any_Manager`, 0x91AD40).
* **Refine pairs functions by name.** A class renamed between copies (Renegade's
  `TextureClass::Apply_Null` is Zero Hour's `TextureBaseClass::Apply_Null`) is never spliced; compare
  such functions by hand. It also cannot add a function a file lacks: those are `PATCHES`.
* **An emptied function gets inlined.** Code switched off in a copy (Generals' `#if 0` around
  `ThumbnailManagerClass::Save`) lets VC6 inline what's left into its callers, and the callers stop
  matching. A caller that matches the other copy's bytes, with a call where ours has none, points
  at it.
* **A `SNAPSHOT_SAY` costs inline budget.** It compiles to nothing in a release build, but VC6
  still counts the inlined `WW3D::Is_Snapshot_Activated()` against the caller's inlining budget.
  Adding or removing one moves which later inline calls get expanded (`Apply_Default_State`
  0x9270D0: one opening `SNAPSHOT_SAY` too many inlined one more `Set_DX8_Render_State` and one
  fewer `Set_Texture`). When a long function of inline calls switches from inline to call at a
  different point than the client, try the `SNAPSHOT_SAY`s first. `.work/ws_try.py` compiles
  variants of one file quickly.
* **Compare against the session's baseline, not only the last step.** Out-of-line copies of
  inline functions (`Set_Texture` 0x47F3F0, `Set_DX8_Light` 0x984090) come and go with the
  callers' inlining; a step that stops emitting one looks like a loss against the previous step
  but not against the baseline.
* **E&B's wwshade is older than Zero Hour's in places.** A wwshade compile error that asks for
  a Zero Hour WW3D2 feature is not evidence for it: check the calling function in the client
  first (for `CheckTranslucent` the client has Renegade's test; `MultiListIterator::Last` and
  `get_deformed_vertices` are used only by code the client does not have).
* **Not every library was built with the same flags.** wwshade was built `/GX /Ob1`, the
  others `/Ob2` without `/GX` (library-matching.md §3.5). Signs of a flag difference rather than
  a source difference: EH prologs (`push -1; push handler; mov eax, fs:[0]`) in the client but not
  in ours, or a caller that calls what ours inlines in every function of a file. Compile with
  `importer.compile_and_match` (it takes `LIB_FLAGS`), not `vc6.py` defaults, and give a new
  library's flags to `bytematch_all.sh` too.
* **A twin function is not a match.** Small functions that only `new` and call a constructor
  (`MeshClass::Clone` 0x93ED00 and `ShdMeshClass::Clone`) or return a constant match each other
  across libraries. When a fix "loses" an address in another library's region, check that the
  address is still matched in its own library (`imports.json`) before counting it as a loss.
* **The client may lack a whole module.** Strings registered by a file (`REGISTER_SHDDEF`
  names), force-link lists and loop bounds over class ids tell which modules are linked: E&B
  has no LegacyW3D shader, so its compile errors needed no fix.
* **`bytematch_all.sh` writes to `%TEMP%\enb_bytematch` unless given a directory.** Run it as
  `tools/vc6/bytematch_all.sh .work/bm2`, or the evidence `refine`/`assemble` read stays old
  (it did after the wwshade flag change: refine and the plan counts still saw 46, not 61).
  After new evidence, `tools/vc6/refresh_expected.py .work/bm2 LIB` refreshes a library's plan
  totals and per-file `expected` counts without touching its tree decisions.
* **Refine can undo an E&B patch through a twin.** It splices in another copy's function when that
  one matches *any* client address, and relocations are masked, so a twin elsewhere counts. It
  put Zero Hour's `ShdBumpSpecDefClass::Init` back over E&B's DX6-only one, because Zero Hour's
  matches Bump Diffuse's `Init` (0xA0B9B0). After a refine, read the diff of every file it
  touched and check each new pair's address belongs to that file (`client.py name`); revert
  the file and its `imports.json` entry if not.
* **Clones share a computer name.** Always address the VMs by IP (`ENB_VC6_HOST` or
  `ENB_VC6_HOSTS`).
* **`objdiff.py` shows import calls as `call REL`.** Our `call dword ptr [__imp__AIL_lock@0]`
  and a direct `call rel32` look the same in its left column; the client's shows as
  `call dword ptr [REL]`. A difference there is not real until `tools/re/objrel.py OBJ SYMBOL`
  shows a non-`__imp__` target. Lines with immediates like `0xffffffff` against the client's
  `REL` are display only too.
* **A changed base signature shows up as hidden virtuals.** When a derived class's vtable keeps
  the base's function in a slot and has an extra empty slot of its own (`ret 4`), check the base
  function's `ret N`: E&B gave `Sound3DClass::Set_Velocity` a second argument (`ret 8`), so the
  unchanged one-argument overrides in `SoundPseudo3DClass` and `Listener3DClass` became new
  virtuals. Each sits where its declaration puts it among the class's own slots.
* **Many callers missing, one inline to blame.** If a small function inlines everywhere in ours
  but the client calls it, look for code compiled out in ours that makes it bigger in E&B's:
  restoring `Is_Disabled`'s `#ifdef G_CODE_BASE` block stopped the inlining and matched 13
  callers at once. The reverse (an `operator==` the client calls out of line in one place)
  can depend on how the condition is written (`!=` keeps it out of line).
* **Operand order and statement splits matter in float code.** `elapsed * 0.05F * K` loads the
  constant first; the client's `step = elapsed * 0.05F; step *= K;` loads `elapsed` first.
  `sound == list[i]` and `list[i] == sound` compare in different operand orders; a temporary
  (`int max = obj->m;` before the call) changes register use. When a reconstruction differs only
  in a few loads, `tools/vc6/trydiff.py` keeps the variant's object for `objdiff.py`.
* **REA's Ghidra startup can exceed its deadline.** On 2026-10-23 every `batch_decompile` failed with
  `provider_timeout` ("did not finish importing and analyzing the target within its 330000 ms startup
  deadline"), retries included; the failure arrives only after the background task ends, as a saved
  JSON whose items carry `error`, not `pseudocode`. The fix is `REA_GHIDRA_STARTUP_TIMEOUT_MS` (e.g.
  900000) in the environment that starts the REA server, then a restart; check an item's `status`
  before reading the file. Set since 2026-10-23: `REA_GHIDRA_STARTUP_TIMEOUT_MS=900000` in the `rea`
  server's `env` (`~/.claude.json`) and `MCP_TOOL_TIMEOUT=1200000` in `~/.claude/settings.json` `env`
  (Claude Code's own tool-call limit, kept above REA's).
* **REA can't decompile vtable-only functions.** Ghidra's auto-analysis defines no function at an
  address only reached through a vtable (0x9961E0, the distortion draw, referenced only from
  `.rdata` 0xB13984), and REA has no tool to create one: `batch_decompile` fails with "No procedure
  exists at the requested address" (or "no loaded source mapping"), `resolve_containing_procedure`
  says `not_in_procedure`. Use `tools/re/pseudocode.py` instead: it runs Ghidra headless, creates a
  function at every start `clientimage.py` knows, and saves pseudocode to `.work/pseudo/`
  (`--show ADDR` prints it). A batch result over the token limit is saved to a file: read its
  `normalized_result.items[].pseudocode`, and check it isn't Ghidra's log (failed items carry it,
  ~85 KB each, so even an all-error batch overflows and hides the error message).
* **Ghidra refuses a project path with a `.`-prefixed element** ("Path element starting with '.'
  is not permitted"), so a headless project can't live in `.work/`; `pseudocode.py` keeps it in
  `%LOCALAPPDATA%\EnB\ghidra`. Passing the install path's `Earth & Beyond` to `analyzeHeadless.bat`
  is also unsafe (`&`): it works on a copy, `.work/ghidra/client.exe`.
* **A displacement difference can belong to another class.** `DX8PolygonRendererClass`'s
  constructor differed only in `lea ecx,[eax+0xAC]` (client 0x84): the register held the *mesh*
  argument, so the layout that differed was `MeshClass`'s (in fact the list lived in
  `MeshModelClass` in E&B). Trace the register to its source before patching a header.
* **`objdiff.py` marks relocated operands as differences.** Our object shows a relocated field as
  `0` / `[0]` (or `[esi + REL]`), the client as `REL`: those `*` lines are not differences. Real
  ones are other instructions, extra pushes, or code only one side has.
* **Adding the same call to twin overloads makes them uncountable.** `objmatch` wildcards call
  targets, so two overloads that differ only in their callees become one pattern found at both
  addresses (`multiple`), and `verify` credits neither (`WW3D::Set_Render_Device`, 0x917790 and
  0x9177E0). Check with `objmatch.py OBJ`: both found exactly at their two client addresses is the
  evidence; record the change with a 0 gain.
* **Pointer/POD template instances are never counted.** `VectorClass<T *>` / `DynamicVectorClass<T *>`
  members (`Clear`, `ID`, `Resize`, `operator==`, deleting destructors) and `??_G` of empty bases
  compile to the same bytes for every `T`, so the client has 30-140 copies and `objmatch` reports
  `multiple` with only the first 5 addresses. To see whether a gap's address is among them, run a
  copy of `objmatch.py` without the `[:5]` cap (D19's 0x997EA0..0x9981A0, 2026-10-23); don't try
  to rebuild them.
* **A copy's low evidence count can be its own headers' fault.** Zero Hour's `mapper.cpp` matched
  only 2 client functions in `.work/bm2` (compiled with Zero Hour's own WW3D2 headers), yet
  swapped into the tree it matched all of them (+56): the client's vtables had Zero Hour's slot
  order all along. Before concluding "E&B has X without Y" from the evidence, read the client's
  vtable (`vtables.py --seed`) and `trial.py` the copy against the current tree.
* **Edit first, generate the spec after.** For a change spanning many hunks, edit the tree files
  directly (scripts in `.work/`), verify, then `tools/vc6/mkspec.py` turns `git diff` into an
  addpatch spec (one patch per hunk, `old` unique both as a plain substring and at a line start);
  `git checkout` the files and run `addpatch.py`, then `cmp` against a backup of the edited files.
  A Python heredoc in Bash mangles `\n` in patch scripts: write them with the file tool.
