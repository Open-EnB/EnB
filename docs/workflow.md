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
4. **Verify** as in §2: `importer.py verify --lib app` (`/MT /W3 /O2 /Ob2 /GX`, `NDEBUG`
   defined: D61, G_ASSERT / FAIL_IF don't depend on it; matched within 0x401000-0x8C1B50 only),
   `tools/vc6/pairs_cmp.py`, `report`, `replay`, commit. Functions
   too short to search for (objmatch `short`: bitset's 10-byte destructor) aren't counted by verify:
   show them with `appfile.py match` on a copy with markers on the blank lines before each
   function (so no assert moves; `.work/night/item10/mark.py`, untracked, historical), and say so
   in the commit.

`__FILE__`: the client's strings are absolute (`e:\build\beta\G\app\shared\auxdata\bitset.cpp`),
and VC6's `__FILE__` is the source path exactly as cl was given it (case kept). So verify names
app sources to cl as `e:\build\beta\G\...` (`vc6.compile_sources(source_alias=...)`: in the VM,
`subst E:` of `C:\EnBDevelopment\_vc6\alias_e`, whose `build\beta\G` is a junction to the
repository; every compile repeats the setup). Check a new file's string in the obj under
`%TEMP%\enb_import\verify-app\`. appfile.py drafts compile under their own path: their string
differs, their bytes don't (the string is reached through a relocation). Headers' strings in
the client are relative (`..\shared/auxdata/auxdata.h`, `..\..\wwlib\ref_ptr.h`): E&B's
compiles ran in a project directory with relative includes. Leave them (D41): the string
differs, the bytes and lines match through the relocation.

## 3. Known traps

§3.0 is for everyone; then app code: 3.1-3.4, 3.6, 3.8, its 3.9 part; a Westwood library: 3.1,
3.4-3.6, 3.8, 3.9 (3.2 if template-heavy); a harness or program: 3.5, 3.7, 3.8. Tool usage is in
tools/re/README.md, tools/vc6 and tools/harness/README.md.

### 3.0 Everyone

* **`assemble` is not a rebuild; `replay` is.** `assemble` restarts every library from its base
  copy, dropping every later decision: never on a tree with work in it (`git checkout -- <lib>`
  restores it). `replay` rebuilds `.work/replay` from the EA copies plus `imports.json` and compares
  (no VM, seconds): before committing it must report 0 differences; fix the record, not the tree.
  Stop chained commands on a replay failure.
* **Pair sets, not totals, against the session's baseline** (`tools/vc6/pairs_cmp.py`). Out-of-line
  copies of inline functions (`Set_Texture` 0x47F3F0, `Set_DX8_Light` 0x984090) come and go with
  their callers' inlining: a loss against the last step needn't be one against the baseline.
* **The importer lock** (§1). A script holding it that starts `importer.py`, `trial.py`,
  `tryvariants.py` or `trydiff.py` as a child deadlocks: start children from an unlocked parent
  (as `trial_all.py`) or call in-process. `tools/vc6/zhall.py` holds it ~15 min (ww3d2); read-only
  `objdiff.py` still works. A killed run or a transient `PermissionError` on a tree write leaves a
  spliced file: `git status`, restore, delete the lock.
* **Shared tree: never wait for a clean tree** (it deadlocked two builders for an hour). Keep the
  other's work out of your verify: untracked files aren't compiled unless importer.py lists them,
  tracked edits and uncommitted importer.py entries are (2 encode.cpp gains showed in another's
  run): compare per file, restore imports.json; `build.py link` then reports stub compile errors.
  Commit only your own hunks, soon; in doubt the coordinator sets a turn order.
* **Scripts and specs through the Write tool, never a heredoc:** Bash mangles `\n`, `\t`, `\r\n`,
  `\x`; in non-raw Python strings `"\a"`/`"\b"` are control characters.
* **CRLF.** Git Bash `sed -i` and the Write tool turn CRLF into LF (replay: "content differs"): use
  the Edit tool or Python `newline=''`, and restore CRLF after a Write (§3.8).
* **Docs stay UTF-8.** A `§` written as cp1252 made progress.md invalid and the Edit tool refused
  it: repair (`.work/day/fix_utf8.py`, untracked, historical), don't work around it.
* **Unattended runs: no modal dialogs, no screen grabs; full screen is fine** (user, 2026-10-24).
  A MessageBox (WW3D's "hardware does not support the requested settings") waits on the live
  desktop: route boxes to the log (vidtest `-nomsgbox`, implied by `-frames`/`-shot`/`-shots`).
  Shots come from the program's own D3D back buffer: a screen-DC grab takes the user's screen,
  PrintWindow is black for D3D8.
* **Never the game's install folder as working directory.** ThumbnailManager writes
  `..\data\client\mixfiles\*.th6` relative to it (from `EBCONFIG`: the real data). Stay in
  `build/` (`vidtest -art DIR`); run originals in a copy (`build/programs/vidtest/ref_ebconfig/`).
* **`git add` by name,** never `-A` or `.`: the tree holds other builders' files.

### 3.1 VC6 code generation

* **The inline budget.** Every inline candidate in a file spends the caller's budget, which grows
  with the caller's size: fewer `Add`s anywhere, dead IL in a callee, an accessor instead of a
  same-bytes field read, a `SNAPSHOT_SAY` (§3.9) move where inlining stops. Big constructors stop
  partway (0x700B10, 0x6F85F0 in Ship 0x6F66E0 / Player 0x752510; 0x6F66B0 in the factory
  0x6F6000). `#pragma inline_depth` doesn't reproduce it; templates use its state at the file's END.
* **An out-of-line copy right after its first caller is an inline function** not inlined there:
  make it `inline` (0x6FA150, 0x6FB1E0, 0x707430, 0x6F85F0). `/Ob2` inlines functions defined
  later in the file too. An EA `WWINLINE` the client calls out of line moves to the .cpp.
* **Inlining depends on the whole file:** an unrelated edit can swap stack slots elsewhere
  (0x6FB1E0); re-verify after generator changes. An emptied function (Generals' `#if 0` in
  `ThumbnailManagerClass::Save`) gets inlined: callers matching the other copy's call point at it.
* **Many callers missing, one inline to blame:** look for code compiled out in ours (restoring
  `Is_Disabled`'s `#ifdef G_CODE_BASE` matched 13 callers). One out-of-line `operator==` can hinge
  on the condition (`!=` keeps it out of line).
* **Operand order and temporaries.** `elapsed * 0.05F * K` loads the constant first, `step =
  elapsed * 0.05F; step *= K;` loads `elapsed`; `a == b` vs `b == a`, `Pos > size` vs `size < Pos`
  differ. A temporary, a local copy of an argument (0x4A3D50) or a member read into a local before
  a virtual call changes registers; one shared variable reusing a dead argument slot can be what
  matches (0x4A91E0). `tools/vc6/trydiff.py` keeps a variant's object for `objdiff.py`.
* **Ctor/dtor forms.** An implicit destructor stores no vtable (a lone `jmp` to the base's); an
  explicit `{}` one stores it first (0x4AA210). An init list stores members before the vtable,
  body assignments after it (0x4A8370; `GrowBy`, `HasWildcard` needed the body).
* **Return forms.** `if (g()) return true; return false;` gives test/setne, `return g();` neg/sbb;
  a shared `return false` tail is `if (p) {...; return true;} return false;`; separate failure
  returns share the success tail; an eax-wide 0/1 (Is_Dead) is `return(!a() && !b());`.
* **Loops and frames.** A for loop gives `jb top`; `jae exit; jmp top` is a do-while behind a count
  test. A function-scope buffer gives each inlined copy its own frame slot.
* **Overloaded virtuals are laid out in reverse declaration order.**
* **A changed base signature shows up as hidden virtuals:** a base function kept in a slot plus an
  empty own slot (`ret 4`). E&B's `Sound3DClass::Set_Velocity` takes two arguments (`ret 8`), so
  `SoundPseudo3DClass`'s and `Listener3DClass`'s one-argument overrides became new virtuals.
* **A displacement can belong to another class:** `DX8PolygonRendererClass`'s `lea ecx,[eax+0xAC]`
  (client 0x84) was the mesh's, `MeshModelClass`'s layout. Trace the register before patching.
* **Linkage and float forms.** An inlined `DynamicVectorClass::Add` on a global orders registers by
  linkage (file-static or not). Two `(float)exp` of one argument merge, `expf` doesn't. `fild qword`
  (high dword 0) + `fidiv dword` is unsigned `(float)u / (float)v`: such output parameters are
  `unsigned`.
* **Reading the client.** `ret 8` with ecx set is a member even if it ignores `this`. `mov dword
  [..], 0` among register-zero stores is a float. A body equal to two later functions is both
  inlined (Process_Messages). 0x9CCD10 (`xor eax,eax; ret`) fills pure virtual slots: `= 0`. A
  static function sits right after its first out-of-line caller (0x4B43E0 after `Pop_Prefix`). A
  `malloc`'d class size can show extra E&B members (0xE30).

### 3.2 Templates and STL

* **Pointer/POD instances count only in D54's ambiguous column.** `VectorClass<T *>` members,
  `??_G` of empty bases: same bytes for every `T`, 30-140 client copies, objmatch lists 5 (uncap
  `[:5]` for all: D19's 0x997EA0..0x9981A0). Don't rebuild them for pairs. D54 ignores `__FILE__`'s
  directory; copies differing by EH handlers, `_Nil` statics or vtables don't count.
* **Explicit instantiation.** Not a constructor alone (C2909), not after `extern template` (C2949):
  specialise, static members before the constructor (C2908). Only `extern template` (C4231) stops
  VC6 inlining a template member.
* **Template virtuals compile on instantiation** (a `DeltaTemplateClass` virtual compiled
  `Set_Value` for money: `MoneyClass` needed `operator!=`).
* **`std::string s(x); v.push_back(s);` isn't `v.push_back(std::string(x))`:** use the local.

### 3.3 E&B app code

* **Asserts sit on the client's lines (D25).** To add code above a logged line, remove as many
  blank lines from the same function's stretch (cordmgr.cpp; `.work/milestones/a3/relines.py`);
  pad inside a function, not only before it (Get(char **); pk.cpp's helpers take single blanks).
  `appfile.py match` markers replace lines, never add them.
* **`__FILE__`: §2.1;** header strings stay as they are (D41).
* **Client-style virtual calls on members:** a call through the object's own class (reference
  included) is devirtualised; cast to the BASE class reference (`((DeltaClass &)EndTime)`,
  `((VectorClass<T> &)Entries).Clear()`).
* **Files that profile need `#define ENABLE_WWPROFILE`** (Encrypt_Data 116 bytes without, 355 with).
  A `new` loses its EH state when the same-file ctor can't throw; a same-file callee may inline.
* **Assert names can be reversed** (`readbytes == bytes` is `cmp ebp, eax`, the wanted size in
  `readbytes`); FAIL_IFs sharing a string share a variable name.

### 3.4 Importer, verify and pairs

* **Header origins live in `imports.json`'s `headers` map** (`headers_from`: swapped with the
  file). `trial.py` and refine update both; a hand-made header change must update `headers`.
* **Refine** checks only its file (splices aren't checked elsewhere): verify the affected
  libraries. It pairs by name (Renegade's `TextureClass::Apply_Null` is ZH's
  `TextureBaseClass::Apply_Null`: never spliced) and can't add a function (`PATCHES`).
* **Refine can undo an E&B patch through a twin:** it put ZH's `ShdBumpSpecDefClass::Init` over
  E&B's DX6-only one (it matches Bump Diffuse's 0xA0B9B0). Check each new pair's address belongs to
  the file (`client.py name`); else revert the file and its `imports.json` entry.
* **A twin is not a match.** `new`-and-construct functions (`MeshClass::Clone` 0x93ED00,
  `ShdMeshClass::Clone`) match across libraries: a "loss" in another library's region may still be
  matched in its own (`imports.json`).
* **What verify can't count.** Overloads differing only in callees become one pattern at both
  addresses (`WW3D::Set_Render_Device` 0x917790 / 0x9177E0), not identical copies for D54 either:
  `objmatch.py OBJ` finding both is the evidence, 0 gain. Small generated inline virtuals and
  too-short functions never count: show them with `objdiff.py` / `appfile.py match` in the commit.
* **COMDAT duplicates (D20):** a dropped `(file, 0x6CF410/0x909500/...)` pair matched elsewhere is
  no loss; name it in the commit.
* **Patch anchors.** A patch for one copy's file fails on another's (`assemble`: "no longer
  applies"); one whose anchor a later block rewrites (`WWSHADE_API_PATCHES`,
  `TEXTURE_FILTER_PATCHES`) fails in replay: put it in that block.
* **`verify` keeps only the first 50 compile errors:** a library losing functions with no error for
  that file means a loud file (a whole-file swap) hid them. Fix it first.
* **A "none" for a callee proves nothing** (targets are masked): follow the client's call first
  (ZH's `Peek_Thumbnail_Instance_From_Any_Manager`, 0x91AD40).
* **Low evidence can be the copy's own headers:** ZH's `mapper.cpp` matched 2 in `.work/bm2`, +56
  in the tree. Read the client's vtable (`vtables.py --seed`) and `trial.py` before "X without Y".
* **Library flags (D12).** wwshade `/GX /Ob1`, the other EA libraries `/Ob2` without `/GX`
  (library-matching.md §3.5), app `/GX` (D23). Signs: EH prologs (`push -1; push handler; mov eax,
  fs:[0]`) only in the client, or a file's callers all calling what ours inlines. Compile with
  `importer.compile_and_match`, not `vc6.py` defaults; give new flags to `bytematch_all.sh`.
* **`bytematch_all.sh .work/bm2`:** without it, output goes to `%TEMP%\enb_bytematch` and refine
  reads stale evidence; then `refresh_expected.py .work/bm2 LIB`.
* **Many hunks: edit first, generate the spec after** (`tools/vc6/mkspec.py` from `git diff`, then
  `git checkout`, `addpatch.py`, `cmp` against a backup); cut mechanical edits with difflib,
  widening context until unique (`.work/night/item6f/spec.py`, untracked, historical).

### 3.5 Build VMs and vc6.py

* **Address VMs by IP** (`ENB_VC6_HOST`/`ENB_VC6_HOSTS`, from `pool.py`); after a restart
  `vc6.py discover`.
* **vc6.py syncs only `SOURCE_EXT`** (else C1083). `#import`'s `.tlh` holds absolute
  `c:\enbdevelopment\_vc6\obj_*\` paths: strip them before committing.
* **Linking.** `/MT` needs `LIBCPMT.LIB`, not `LIBCP.LIB`; mss32's import library is
  `tools/vc6/mkmss32.py`'s (D51). `build.py` builds committed files only and, without `--report`,
  overwrites `.work/milestones/l1/unresolved.txt`. A stub constructor emits its vtable, so every
  virtual needs a stub: link every object.
* **Checked builds** (`ENB_CHECKED=1`) change mangled names: build every library the same way.

### 3.6 RE tools, Ghidra and REA

* **`gaps.py` pairings can be wrong** (whole objects via shared helpers, overloads, neighbours,
  template dtors): check `ret N`, confirm with `objdiff.py` and `vtables.py`.
* **`objdiff.py`'s `*` lines include relocations** (`0` vs `REL`) and `__imp__` calls as `call
  REL` (`objrel.py`); pass mangled names.
* **`vtables.py --seed Class=ADDR`** for classes whose code doesn't match yet; rerun after
  `symbols.py`.
* **Pseudocode can be wrong** (check `client.py dis`); REA can't decompile vtable-only functions
  (0x9961E0, from `.rdata` 0xB13984): `pseudocode.py --show`.
* **appfile.py:** a draft-folder copy shadows `"x.h"` but not `"..\x.h"`; WW3D2 users need `-I`
  per library; in-class inline markers are ignored; a following static can enlarge the client
  size (PREFIX); same-named overloads get mixed up: rename while drafting.
* **Names lag.** `symbols.json`/bytematch miss recent matches (read the verify objects); asserts.txt
  and funcmatch take the nearest path, not `__FILE__` (0x829C80 is tcpconn.cpp's ctor). names.py
  reads the working tree, tracked files only: in a shared tree use a clean `git worktree`.
* **Tool changes:** compare outputs on a snapshot of the verify objects. Starts are verified (D27).
* **`tryvariants.py` edits one file** (wrap header changes in backup/restore). `client.py
  callers` on hot functions: `PYTHONIOENCODING=utf-8`, redirect.

### 3.7 Harnesses, programs, unattended runs

* **No registry changes (D69):** E&BConfig (HKLM `Software\Westwood Studios\Earth and
  Beyond\Render`) only in a supervised run with a registry backup.
* **Exes from Python by absolute path** (`subprocess` can't start a relative `/` path).
* **Programs:** `WW3DAssetManager` before `WW3D::Init` (else `ShatterSystem::Init` crashes); an
  HLOD's LOD 0 can be empty (`Get_LOD_Count()-1`, recursively); `SortingRendererClass::Flush()`
  before a shot taken before `End_Render`.
* **Custom render objects:** no `Get_Num_Polys`/`Get_Cost` override unless the client's class has
  one (land vtable 0xAE8994 hasn't): it takes other scenes' polygon budget.
* **Kernel tests set `Quit` directly:** the pump (0x443E90, in Process_Messages) tests WM_QUIT only
  after GetMessage returns nonzero, so a posted quit doesn't stop the kernel.
* **Capturing originals, VIDTEST frames, d3d8 proxies:** tools/harness/README.md.

### 3.8 Git, docs and scripting hygiene

* **The Write tool writes LF:** after writing a tracked doc or source, restore CRLF if the file had
  it (python: read bytes, replace b'\n' with b'\r\n' where not already). Check line ends before
  `\r\n` anchors; Python's Windows text mode writes CRLF: match `\r?\n` later.
* **Generated tree files: CRLF throughout** (`newline=''`, `\r\n`); a mixed one (D22's shader
  headers) shows modified with an empty diff. Git's CRLF warning for importer.py is harmless.
* **The Write tool drops tabs on whitespace-only spec lines:** anchor with `"\t"` or tree slices.
* **A slice between two `str.index()` markers** is empty when the second comes first, and
  `replace('', x)` destroys the file: assert it. A short anchor can be part of a longer line.
* **Git Bash:** `"app\shared\auxdata\$f.cpp"` swallows `$f`: use `/`.

### 3.9 Library-specific

* **Zero Hour porting.** D9 places ZH edits. A one-sided trailing block or a callee-saved register
  shift is usually removed code: diff Renegade, Generals, ZH (`//DMS`, ZH tags, commented-out
  lines, `b_wire` "(gth) Renegade-specific hack", HY "Earth and beyond"), scan the client's strings
  (`tools/re/strscan.py`), diff ZH's file-header history topics.
  Enum values are evidence (gradient mode 5: `GRADIENT_MODULATE2X`).
* **ww3d2.** A `SNAPSHOT_SAY` costs inline budget (`Apply_Default_State` 0x9270D0): try them first
  when inline calls switch at another point. meshmatdesc's 0x946EB0, 0x947490, 0x947310 differ in
  `UVSource` load order that moves with unrelated headers: noise (D18, D28). 0x4E9CB0 is E&B's
  `::operator new`, never inlined. dx8webbrowser: E&B's `BrowserEngine.dll` (D21).
* **wwshade** is older than ZH in places: an error asking for a ZH WW3D2 feature isn't evidence
  (`CheckTranslucent`, `MultiListIterator::Last`, `get_deformed_vertices`). Registered names and
  class-id bounds show missing modules (no LegacyW3D).
* **aux_roots:** auxextract misses defaults via a register (`or reg,-1`, byte stores, masks) or a
  stack temporary (1.0f arrays, Validity 0x100): check the bytes before each constructor call.
* **Connection layer / wwudp.** wwlib's pk.h shadows app/packets/pk.h: include `..\packets\pk.h`.
  `CordCon::read` with peek adds to `BytesRead_`. `CordCon::Is_Closed` is `cmp bl, 1` in Connect,
  Is_Connected and the destructor, a plain test in Send_Data and Receive.
* **Packets.** The switch 0x869790 (`tools/re/pkclasses.py`) puts the 5000/30000 bodies last: take
  source order from unwind states. The match: `packet = new X(raw); break;`, `packet` uninitialised,
  the default `packet = 0;` before its assert (`return(new X(raw));` drops `mov esi, eax`).
