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
     locates the functions that aren't byte-exact. Its pairings (and the `gaps-*.json` rows) can
     be wrong: confirm a pair with `objdiff.py` before working on it (§3).
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
     `tools/vc6/pool.py` instead of the default host (Win10 only since the old clones were retired on
     2026-10-24; released at the end; toolchain.md §1.3).
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

### 2.1 Adding an app file (E&B's own code, D23)

E&B's files are the importer's non-EA library `app` (`APP_LIBS` in importer.py): no EA copy,
every file in `RECONSTRUCTED`, under `app/` as in `e:\build\beta\G\app\...` (A1, lower-case
directories).

1. **Draft.** `python tools/re/appfile.py show app\shared\auxdata\x.cpp` gives the window, its
   asserts and their lines; `draft FILE --out .work/<item>/` writes a skeleton with `// @0xADDR`
   markers and the saved pseudocode. Write the C++ (names: conventions.md §1, D10, D24), with
   `#line N` before each assert in the draft, and run `appfile.py match DRAFT.cpp [-I DIR]` until
   every marker is EXACT.
2. **Tree file.** Write it at its client path (`app/shared/auxdata/x.cpp`, its header beside it)
   with the Write tool, CRLF. The asserts sit on the client's `__LINE__` through the layout (blank
   lines, comments; no `#line`, D25), so little fits above the first assert: put the
   reconstruction notes at the end of the file (as bitset.cpp does) and check with
   `grep -n 'G_ASSERT\|FAIL_IF'`. The assert macros: `#include "..\unknown_debug.h"` (relative).
3. **Record.** Add `('app', 'shared/auxdata/x.cpp')` and its header to `RECONSTRUCTED` with the
   evidence (lower-case paths with `/`); `load_state` makes the state entry from them. Two app
   `.cpp` files may not share a base name (it is the file key; `load_state` refuses it).
4. **Verify** as in §2: `importer.py verify --lib app` (`/MT /W3 /O2 /Ob2 /GX`, no `NDEBUG`,
   matched within 0x401000-0x8C1B50 only), pairs_cmp, `report`, `replay`, commit. Functions too
   short to search for (objmatch `short`: bitset's 10-byte destructor) aren't counted by verify:
   show them with `appfile.py match` on a copy with markers on the blank lines before each
   function (so no assert moves; `.work/night/item10/mark.py`), and say so in the commit.

`__FILE__`: the client's strings are absolute (`e:\build\beta\G\app\shared\auxdata\bitset.cpp`),
and VC6's `__FILE__` is the source path exactly as cl was given it (case kept). So verify names
app sources to cl as `e:\build\beta\G\...` (`vc6.compile_sources(source_alias=...)`: in the VM,
`subst E:` of `C:\EnBDevelopment\_vc6\alias_e`, whose `build\beta\G` is a junction to the
repository; every compile repeats the setup). Check a new file's string in the obj under
`%TEMP%\enb_import\verify-app\`. appfile.py drafts compile under their own path: their string
differs, their bytes don't (the string is reached through a relocation). Headers' strings in
the client are relative (`..\shared/auxdata/auxdata.h`, `..\..\wwlib\ref_ptr.h`): E&B's
compiles ran in a project directory with relative includes. Work out that form when the first
header with asserts is rebuilt.

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
  diff. It also swaps neighbours (`Render`/`Special_Render`), pairs a function with an unrelated
  one (asset manager ctor 0x909E60 vs the real 0x91C2C0; `Set_D3D_Base_Texture` vs a deleting
  dtor 0x94D5D0; `Set_Shader` 0xA03C50) or with template dtors (PivotMap, PKStraw, FileStraw):
  confirm with `objdiff.py` and `vtables.py` (2026-10-24).
* **meshmatdesc.cpp's index load order is noise.** Its three fragile functions (0x946EB0,
  0x947490, 0x947310) differ only in the load order of `UVSource[pass][stage]` index arguments,
  and that order moves with the number of symbols declared before meshmatdesc.h's classes
  (matrix4.h, refcount.h, even a dummy class): unrelated header edits (ww3d.h inline accessors)
  cycle which of them match. Not evidence (D18, D28). Corrected 2026-10-24: 0x4E9CB0, earlier
  blamed as an inlined app helper, is E&B's `::operator new` and is never inlined.
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
  (`"\a"`/`"\b"` in non-raw strings become control characters too.) For many mechanical caller
  edits, transform the whole file and cut hunks with difflib, widening context until each is
  unique (`.work/night/item6f/spec.py`); a short line can be a substring of a longer one.

Added 2026-10-24 (overnight run; details in `.work/night/item*/report.md`):

* **Check every EA copy for removed code.** A "register scheduling only" near-miss with a
  one-sided trailing block, or a callee-saved register shift, is usually extra or missing code:
  diff Renegade, Generals and Zero Hour (`//DMS`, ZH-tagged and commented-out lines, Renegade-only
  hacks like `b_wire` "(gth) Renegade-specific hack", HY "Earth and beyond" comments). With a
  Renegade base, Zero Hour's tagged edits are often in the client: scan the client's log, profile
  and device strings against the copies (`.work/night/item6b/strscan.py`). Zero Hour's file-header
  history lines list changes without inline markers: diff the dated topic against the client.
* **Dates and authors place Zero Hour's edits (D9).** KM/KJM through 08/01/02 are in; 08/05/02+
  and every MW/GTH (Generals team) change are out, whatever the date. Use the bytes, not the
  client's PE timestamp (2004).
* **Enum values are evidence.** The client mapping gradient mode 5 showed Zero Hour's
  `GRADIENT_MODULATE2X` in shader.h.
* **Inline budget.** Trivial accessors consume VC6's inline budget even when their bytes equal a
  field read; an inlining switch late in a body can hinge on accessor-vs-field earlier.
  `WWINLINE` in an EA header doesn't mean E&B inlined it: if the client calls it out of line, move
  it to the .cpp. Check a `malloc`'d class size for extra E&B members (0xE30).
* **Linkage shows in inlined code.** An inlined `DynamicVectorClass::Add` on a global list orders
  its registers by linkage (file-static vs external/class-static), not by chance. Two
  `(float)exp` of one argument get merged, `expf` doesn't: when the client recomputes an `exp`,
  try `expf`.
* **`__LINE__` is an immediate.** E&B's asserts push it unrelocated, so app asserts must sit on
  the client's lines (D25).
* **COMDAT duplicates.** A gain that drops an unused helper can lose a duplicate
  `(file, 0x6CF410/0x909500/...)` pair: not a loss if the helper stays matched elsewhere (D20);
  name it in the commit message.
* **Replay and later blocks.** A patch whose anchor a later importer block rewrites (e.g.
  `WWSHADE_API_PATCHES`, `TEXTURE_FILTER_PATCHES`) fails in replay: put it in that later block.
  Stop chained commands on a replay failure.
* **Line endings.** `sed -i` in Git Bash turns CRLF tree files to LF (replay: "content differs"):
  use Python with `newline=''` or the Edit tool. Git's "LF will be replaced by CRLF" warning for
  importer.py is harmless. The Write tool drops tabs on whitespace-only spec lines: build anchors
  with `"\t"` or slice them from the tree text.
* **Tool quirks.** `tryvariants.py` edits one file only: test header changes with a
  backup/restore script around it (`.work/night/item1/hack_h.py`). `objdiff.py` with a plain name
  can pick a prefix-matching function: pass the mangled name. `verify --lib` takes one library per
  flag. `client.py callers` on a hot function crashes on cp1252 stdout: set
  `PYTHONIOENCODING=utf-8` and redirect. In Git Bash, `"app\shared\auxdata\$f.cpp"` swallows `$f`:
  use `/`.
* **Function starts are verified by default** (78b0a74, D27): `clientimage.function_starts()`
  keeps only decoded call targets. Pass `verified=False` only to audit, or to clean earlier
  outputs (pseudocode.py). Comparing generated outputs before and after a tool change: snapshot
  the verify objects first and run both sides on the snapshot.
* **`symbols.json` and bytematch lag.** They don't name recent matches: read names from the verify
  objects (`.work/night/item6i/names.py`).
* **The importer lock and child processes.** A script holding the lock that runs `importer.py`,
  `trial.py`, `tryvariants.py` or `trydiff.py` as a child deadlocks until `--lock-wait`: run
  children from an unlocked parent (as `trial_all.py`) or call in-process. `zhall.py` holds the
  lock for its whole run (~15 min for ww3d2); read-only `objdiff.py` on verify objects still works.
  Killing it mid-run leaves a spliced tree file and the lock: `git status`, restore, delete the
  lock. A transient `PermissionError` on tree writes can do the same: retry, then `git status`.
* **Type libraries.** vc6.py syncs only `SOURCE_EXT` (a DLL `#import` fails C1083; `.tlh`/`.tli`
  were added). `#import` writes absolute `c:\enbdevelopment\_vc6\obj_*\` paths into the `.tlh`:
  strip them before committing. E&B's `BrowserEngine.dll` differs from EA's (same GUIDs, extra
  methods): generate from E&B's (D21).
* **Line endings of generated files.** A generated tree file (e.g. D22's shader headers) written
  with mixed CRLF and LF shows as modified in `git status` with an empty diff. Write CRLF
  throughout (Python `open(..., 'w', newline='')` with `\r\n`), like the EA files.
* **App `__FILE__` strings.** A `.cpp`'s `__FILE__` in the client is absolute
  (`e:\build\beta\G\app\...`) and must come out byte-identical, so app sources are compiled through
  `vc6.py`'s `source_alias` (§2.1). Headers' strings are relative (`..\shared/auxdata/...`): the
  first app header with an assert needs E&B's project-directory include form worked out first.
* **App code forms (2026-10-24, auxdata).** VC6 `/Ob2` inlines functions defined *later* in the
  same file, so an inlined helper needn't be defined above its callers. Operand order changes
  bytes (`Pos > size` vs `size < Pos`), and so does a store in the constructor body vs the
  member-init list (`GrowBy`, `HasWildcard` needed the body). `if (g()) return true; return false;`
  gives test/setne where `return g();` gives neg/sbb. Overloaded virtuals are laid out in reverse
  declaration order. An explicit specialisation declaration doesn't stop VC6 inlining a template
  member; `extern template` does (warning C4231).
* **appfile.py match.** It puts the draft's folder first on the include path (a scratch copy of a
  tree header hides the tree's), ignores `// @ADDR` markers on in-class inline functions (define
  them out of class to check), and reports explicit specialisations as "exact (no marker)".
* **Docs are UTF-8.** Never write a doc as Latin-1/cp1252: a `§` written that way made
  progress.md invalid UTF-8 (fixed in fdd1fbd) and the Edit tool then refuses the file. If it
  happens, repair it (`.work/day/fix_utf8.py`), don't work around it.
* **Templates and statics in app code.** VC6 compiles a class template's virtual functions whenever
  the class is instantiated (adding a virtual to `DeltaTemplateClass` compiled `Set_Value` for
  money: `MoneyClass` needed `operator!=`). A virtual call on a member object is devirtualised
  unless cast to a reference (`((DeltaClass &)EndTime).Unknown_00(v)`). A static function is
  called directly, without an incremental-link thunk, and its code sits right after the first
  function that calls it out of line (the join 0x4B43E0 follows `Pop_Prefix`).
* **appfile.py sizes and names.** Its client size can include a following static or initialiser,
  so an exact match shows PREFIX with a larger client size (`Pop_Prefix`, `Get_Rest`,
  `~KVPairClass`): check the bytes. It pairs markers with functions by name, so same-named
  overloads get mixed up: rename them while drafting.
* **EA-UDP ack lists.** An ack list follows when bit 0 of the datagram's type byte is set (type 3
  exists too, not only 1): byte 5 is the entry count, then one header byte, then 4-byte entries
  (flag, channel, 16-bit big-endian seq). the reference decoder's `the reference importer` guesses the block start instead
  and loses 28 / 12 messages in the Alex dumps; use `tools/harness/convert.py`'s datagram files.
  The Alex dumps also lose bytes in the middle (not the end) of 3 datagrams per file (`X` lines).
* **Garbled pseudocode at 0x4A9400.** Ghidra's output for the tree-form decoder is wrong; read
  `client.py dis` instead.
* **Running a built exe from Python.** `subprocess` on Windows can't start a relative path written
  with `/`; make it absolute (`compare.py --exe`).
* **Linking: LIBCPMT, not LIBCP.** With `/MT` the C++ library is `LIBCPMT.LIB`; `LIBCP.LIB` is the
  single-threaded one and lacks `std::_Lockit::_Lockit()`. The VMs have only VC98's libraries:
  `tools/vc6/build.py` copies the DX8 SDK libs over. Its unresolved list covers only what the
  libraries reference among themselves; a program's own code adds more, and LIB members are
  pulled only on demand.
* **Running the tree's programs.** `WW3DAssetManager` must exist before `WW3D::Init`, or
  `Set_Render_Device` crashes in `ShatterSystem::Init`. d3d8.dll is loaded at run time (no import).
  Windowed `-depth 16` still gets the desktop's 32-bit back buffer. Screenshots: read the program's
  own D3D back buffer. A screen-DC grab captures whatever window is on top (the user's screen),
  and PrintWindow gives black for D3D8 windows.
* **Stubs and vtables.** A stub constructor makes the stub file emit that class's vtable, so every
  virtual then needs a stub too; check by linking every object, not only the .lib members.
  `build.py`'s default `--report` overwrites `.work/milestones/l1/unresolved.txt`.
* **mss32.lib.** VC6's `lib /def` drops the leading underscore of stdcall names, so imports don't
  bind to Mss32.dll's `_AIL_x@n` exports; `tools/vc6/mkmss32.py` links a dummy DLL and keeps its
  import library.
* **VC6 explicit instantiation.** A constructor can't be instantiated on its own (C2909), nor the
  class after the header's `extern template` (C2949): use an explicit specialisation, with its
  static members specialised before the constructor (C2908).
* **Never run a program in the game's install folder.** ThumbnailManager writes
  `..\data\client\mixfiles\*.th6` relative to the working directory, which from `EBCONFIG` is the
  real game's data. Keep the working directory in `build/` and point a file factory at the art
  (`vidtest -art DIR`); run originals in a copy (`build/programs/vidtest/ref_ebconfig/`).
* **Rendering W3D models.** An HLOD's LOD 0 can be empty: set every HLOD, recursively, to
  `Get_LOD_Count()-1`. Call `SortingRendererClass::Flush()` before a back-buffer screenshot taken
  before `End_Render`, or alpha geometry is missing.
* **Capturing an original program.** A d3d8.dll proxy in a copy of its folder (d3d8.dll is not a
  KnownDLL) reads its back buffer and logs its file opens (`tools/harness/run_reference.py`).
  Put its options before the subcommand: `run_reference.py --frames 1,30 run -- -resx 800`.
* **VIDTEST's look depends on library switches.** Without `Enable_Overbright_Modify_On_Load` the
  models are dim (62.5% of frame 300 differs; about 3% with it). Ships appear only with the
  PredictiveLODOptimizer running each frame. The overlay's texture animations start at a phase
  set by the machine's uptime: not a bug. `-sync` has no visible effect when windowed. A
  full-screen quad needs vertices (1,1), (1,-1), (-1,-1), (-1,1) and indices 0,2,1,0,3,2, or
  it is culled.
* **Unattended runs on the host: no modal dialogs.** The user's desktop is live. A modal MessageBox
  (e.g. WW3D's "hardware does not support the requested settings") waits for a click (2026-10-24:
  two reached the user). Every test run routes message boxes to the log. Full-screen test runs are
  fine (user, 2026-10-24). vidtest's `-nomsgbox` (implied by `-frames`/`-shot`/`-shots`) does this.
* **E&BConfig and VIDTEST.** E&BConfig starts VIDTEST with a command line that has no program name,
  so the CRT's argument parsing drops `-device` and both programs use device 0. Its registry writes
  (HKLM `Software\Westwood Studios\Earth and Beyond\Render`, 32-bit) go to HKCU VirtualStore
  unless it runs elevated. `PeakPolyRate` is about polygons per frame times frames presented, since
  the whole run takes about a second on a modern GPU. `build.py link` reports stub compile errors
  while another builder has uncommitted app/ headers.
* **D54's ambiguous count.** Each E&B project keeps its own copy of a header's `__FILE__` (absolute
  in auxdata, relative in client files), so the "identical copies" test compares C strings without
  their directory. Copies that differ by EH-handler references or per-instantiation `_Nil` statics
  or vtables are not identical and don't count.
* **VC6 constructor and destructor forms.** An implicit destructor stores no vtable; an explicit
  `{}` one stores it first (0x4AA210). An init list stores members before the vtable, assignments
  in the body store the vtable first (0x4A8370). VC6 reuses dead argument slots for later locals:
  one shared variable can be what matches (0x4A91E0).
* **build.py builds committed files only.** A new uncommitted .cpp shows up as unresolved symbols. Without
  `--report`, `build.py` overwrites `.work/milestones/l1/unresolved.txt`.
* **The client's Delta model has no "default" state.** Updated and defaulted both set
  `m_Unknown70 = 2`; a harness keeps its own unknown/default/value state. `DeltaCompoundClass::
  Unknown_18` recurses into children only when `m_Unknown70 == 0`, `Unknown_1C` only when it is
  non-zero: expand compounds to their leaves yourself. The Player root derives from the second
  root class (vtable 0xAEAC40, ctor 0x4A3FF0, with the update collector), not from 0xAEAB88.
* **appfile.py match and includes.** A header included by a relative path (`#include "..\x.h"`)
  is found next to the including file, so a copy in the draft folder doesn't shadow it. App files
  that include WW3D2 headers need `-I` for every library directory.
* **Custom render objects.** Don't override `Get_Num_Polys` or `Get_Cost` unless the client's class
  does (the land vtable 0xAE8994 doesn't): an override takes polygon budget from the other scenes.
* **Shared tree, two builders.** "Wait until the other's tree is clean" deadlocked two builders for
  an hour. The rule is: the other builder's changes must not be compiled into your verify (their
  untracked files aren't, unless importer.py lists them); commit only your own hunks; keep the
  uncommitted window short. When in doubt the coordinator sets a turn order.
* **d3d8 proxies.** d3d8 keeps a writable copy of the device vtable per device and rewrites some
  slots after CreateDevice: Set* hooks there see nothing; read state with Get* at draw time and
  re-hook at each Present. The proxy must pin itself (LoadLibrary on its own path): WW3D::Shutdown
  frees d3d8.dll while patched vtables still point into it. Default-pool textures (the original's
  lightmap) can't be locked: GetSurfaceLevel + CopyRects into an image surface.
* **Comparing VIDTEST frames.** Its sun follows GetTickCount: compare at the original's angle (from
  d3d8shot.log's stamps, `-sunangle`). Pixel diffs can't tell geometry from texture problems: dump
  the draws (`run_reference.py --dump`, `drawdump.py`).
* **Checked builds.** WWASSERT / WWDEBUG_* are compiled out in the release libraries as in the
  client; `ENB_CHECKED=1 build.py --out build/chk` compiles them in. A checked build changes some
  mangled names (`private=public`): build every library the same way. A full build also compiles
  another builder's uncommitted importer.py entries.
* **App files that profile** need `#define ENABLE_WWPROFILE` (without it Encrypt_Data compiles to
  116 bytes instead of 355). A `new` loses its EH state when the constructor in the same file can't
  throw; an out-of-line callee in the same file (the CRC) may get inlined.
* **EA-UDP dumps.** The Alex dumps start at byte 5 of the wire datagram: bytes 0-4 are the type
  (5 client, 4 server), the session id and the sender's datagram counter (the dump's `seq`). The
  checksum (the client's CRC 0x8B3EC0) covers the whole datagram including those bytes.
* **Verify with another builder's tracked edits present** picks them up (2 encode.cpp gains showed
  in someone else's run): compare per file and restore imports.json. names.py reads the working
  tree: regenerate names-review.md in a clean `git worktree` when others have uncommitted names.
* **Net-7 recordings.** The 2022-09-25 recording has no client side of the sector (proxy-to-server
  only): run it with `n7aux.py --server-side`. Net-7's encoder differs from EA's in a fourth way
  beyond the reference decoder's auxdata.md §7: fields named in two sets (Value and Default, a Whole header
  `0f` for EA's `07`); the client applies one, so such messages re-encode differently.
* **asserts.txt's file attribution** gives each string the nearest preceding path: tcpqueue.cpp gets
  about 60 asserts of which only 2 are its own. funcmatch's string evidence doesn't check `__FILE__`
  either (symbols.json's 0x829C80 `PacketManagerClass::Clear_Socket_Error` is tcpconn.cpp's
  constructor).
* **Client-style virtual calls on objects.** A cast to the object's own class reference still
  devirtualises (and inlines) the call; cast to the BASE class reference
  (`((VectorClass<T> &)Entries).Clear()`). `fild qword` (high dword 0) then `fidiv dword` is
  `(float)u / (float)v` of unsigned values.
* **Blank stretches inside functions** (cordmgr.cpp's Pump, Process_CORD_Packet) keep later asserts
  on the client's lines: to add code above a logged line, remove as many blank lines from the same
  function's stretch (`.work/milestones/a3/relines.py`).
* **Prior-work offsets.** the reference decoder's login.md gives LoginTaskClass's vtable as 0xB040CC; it is
  0xB040D4 (its slot offsets are 8 too high). app-functions.json puts the connection base's inline
  slots 0x726AF0-0x726BF0 in client.cpp.
* **Edit scripts that slice between two `str.index()` markers** get an empty string when the second
  marker comes first, and `replace('', x)` then destroys the file: assert the slice before replacing.
