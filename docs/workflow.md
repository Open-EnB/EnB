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

* **What sends a Cast out of line: a non-tail nested multi-return expansion before a conditional call** (2026-10-29,
  F8's Fable round, calibrated on 0x6FEAC0 / 0x6FDBD0): the budget unit that pushes a find's `C *t = Cast(d);
  G_ASSERT(t);` out of line is a nested inline expansion with several returns in non-tail position followed by a
  conditional call. The same body flat, `return Cast(d)` in tail position (why IB1's Unknown_Find_Aux never moved a
  Cast), 1-3 dead calls, a loop in an inlined helper, asserts or accessors alone (10 nested, 35 in the caller): no
  effect. A dead spender starves only the expansions after it in source order: the same dead find at the top of
  0x6F3FA0 sends Rotate_X out, as the last statement it gives the client's calls. Refuted as the per-find spender:
  the accessors (mark / Value / AuxData / Client / Model / GameID), NameClass temporaries, G_ASSERT as an inline
  function (no effect anywhere in co.cpp), an empty ~PermString, Get_Lookup / Find wrappers, Is_Null / Type_Of in
  Find_Field or Cast, a checked-cast wrapper (moves 0x6FFC80 / 0x700400 / 0x6F3FA0's Casts, not these), NameClass's
  constructor and LookupClass::Find made inline-visible, /Zi and /Z7. Lead: in 0x6FD310 one static inline helper
  around the three finds (name built inside, mark test, array store, changed loop) sends all three Casts out with
  no dead code. `attempts.py --stash` writes an untracked docs/attempts/<ADDR>/NAME.txt: add it to the commit.
* **A switch's case order** (2026-10-29, LD4): 0x65AB10 became exact only with the cases written in the jump table's
  target-address order; in 0x514010 reordering the labels gave byte-identical code (VC6 normalises there). Try the
  table's order once; if the bytes don't move, the residue is elsewhere.
* **An inline element copy's form names the element's members** (2026-10-29, PK3): a `rep movsd` where the client
  copies member by member means the element has a member with a user copy constructor (Vector4 / Vector3; the
  same change flipped an out-of-line insert to inline, 0x8807B0). VC6 puts the loop pointer on the third dword of
  the first member with a copy constructor: when the client's pointer sits elsewhere, nest the members into
  sub-structs (0x87F320: two {int, Vector3} halves).
**Triage.** Classify a near miss first: `python tools/re/explain.py OBJ SYMBOL ADDR` prints one class,
the first mismatch and the masked residue. Fix the classes in this order, since an earlier one masks
the later ones: **relocation** (wrong or unresolved target, string or symbol) -> **misplaced start**
(the window opens like another function's tail) -> **frame** (frame size) -> **field offset** (same
shape, `[reg+N]` differs) -> **literal** -> **register-only** / **order-only** -> **inline budget**
(a callee called at one site and inlined at another; `??_H` temporaries) -> **shape**. Before any
retry read `python tools/re/attempts.py show ADDR` (builders.md §2). Switches: one
`tools/vc6/flagsweep.py` run rules them out (none has ever fixed a near miss, row below); never loop it.

**Two-attempt stop rule.** After two failed attempts on a register-only or order-only residue,
record it (`attempts.py record ADDR ... --status parked --blocker register|order --forms N
--evidence ...`, stash the best form) and move on. A register-only or order-only residue never gets a
third attempt by anyone: VC6 collapses such forms into a few canonical outputs (Make_Screen_Shot 0x9182C0
took ~110 forms to learn this); it waits for a lever proven on another function. An inline-budget, frame or
shape residue gets one Fable round under D72 and D89 (builders.md §4): 8 forms from stated hypotheses, then
a written explanation in attempts.csv and `LIMIT REACHED`; `attempts.py check` refuses a later retry
unless `--lever` cites a new row of this table.

| Symptom (what the diff or `explain.py` shows) | Lever (the source change that fixed it) | Proven (address, commit) |
|---|---|---|
| **inline budget** | | |
| A callee called at one site, inlined at another (or the reverse); a big constructor stops inlining partway | Every inline candidate in a file spends the caller's budget, which grows with the caller's size: fewer `Add`s anywhere, dead IL in a callee, an accessor instead of a same-bytes field read, a `SNAPSHOT_SAY` (§3.9) move where inlining stops. `#pragma inline_depth` doesn't reproduce it; templates use its state at the file's END | 0x700B10, 0x6F85F0 in Ship 0x6F66E0 / Player 0x752510; 0x6F66B0 in the factory 0x6F6000 (Ship still open: `attempts.py show 0x6F66E0`) |
| The client calls a callee out of line (the insert's last `size()`) that ours inlines | A dead inlined call spends the budget without emitting code: an unused `int index = Children.size();` before the call; a dead call *after* the call site changes earlier inlining too | `DeltaCompoundClass::Add` 0x4A5670, c0ae2f0 (D40) |
| Inlining differs although the caller's own code is the same | The budget counts loops inside inlined callees, even when the body is an empty release `WWASSERT`; empty `WWASSERT`s and dead loops in the caller itself count for nothing | builder C, 2026-10-25 (95f350d `Process_Foreground_Load` 0x949C60) |
| A `Transpose`'s `Vector4()` through `??_H` at only one site, every stack slot moved | Tiny accessors tip the budget: `view[r][c]` (two inline `operator[]`) against `view[r].X` | `PointGroupClass::Render` 0x954960, f9ae6b9 |
| Callers that inline a loop function call others out of line | `it++` in the loop spends their budget: `++it` | Release 0x826870 and the listener destructors, ba71802 |
| An out-of-line copy right after its first caller | It is an inline function not inlined there: make it `inline`. `/Ob2` inlines functions defined later in the file too; an EA `WWINLINE` the client calls out of line moves to the .cpp | 0x6FA150, 0x6FB1E0, 0x707430, 0x6F85F0 (6d32dcd, d64612e) |
| Stack slots swap in a function you didn't touch | Inlining depends on the whole file: an unrelated edit moves it; re-verify after generator changes. An emptied function (Generals' `#if 0` in `ThumbnailManagerClass::Save`) gets inlined: callers matching the other copy's call point at it | 0x6FB1E0 |
| Many callers missing, one inline to blame | Look for code compiled out in ours: restoring `Is_Disabled`'s `#ifdef G_CODE_BASE` matched 13 callers. One out-of-line `operator==` can hinge on the condition (`!=` keeps it out of line) | 8e78c70 |
| A helper the client keeps out of line with no caller | A global, not a static (statics vanish when inlined) | connect.cpp's host helpers 0x821890 / 0x821900, step 8 |
| **register-only, order-only** (two attempts, then the stop rule) | | |
| A constant loaded before a variable, or the reverse | Operand order: `elapsed * 0.05F * K` loads the constant first, `step = elapsed * 0.05F; step *= K;` loads `elapsed`; `a == b` vs `b == a`, `Pos > size` vs `size < Pos` differ (D18/D28: operand-order noise elsewhere) | |
| Registers differ around a call or an argument | A temporary, a local copy of an argument or a member read into a local before a virtual call changes registers; one shared variable reusing a dead argument slot can be what matches. `tools/vc6/trydiff.py` keeps a variant's object for `objdiff.py` / `explain.py` | 0x4A3D50 (f2f2a08), 0x4A91E0 (e093a67), Accept 0x82A6C0 (663057c) |
| A different frame slot for a helper object | A temporary (`X().Unregister(p)`) gets another slot than a named local | step 8 (memconn.cpp) |
| Registers of an inlined `DynamicVectorClass::Add` on a global | The global's linkage (file-static or not) orders them | |
| A load-order / allocation residue no local form moves | VC6 collapses local rewrites into a few canonical outputs; the lever is function-wide (other locals, a shared helper) or nowhere: stop after two attempts | Make_Screen_Shot 0x9182C0 / Update_Movie_Capture 0x918B20, parked (4246727; `attempts.py show 0x9182C0`) |
| Any residue, tried with compiler switches | No switch has fixed one: Make_Screen_Shot's U form under all 30 `/O1 /O2` x `/Ob0-2` x `/G3-/G6 /GB` (`flagsweep.py`, 2026-10-26: the library's own `/O2 /Ob2 /GB` best, 5 rows, nothing exact); Open-BFME's 113 near misses x every `/O /Ob /G /Oy` (MSVC 7.1). An allocation residue comes from the source | 996b68a |
| **shape: returns, tails, loops** | | |
| test/setne against neg/sbb; shared or separate `return false` tails | `if (g()) return true; return false;` gives test/setne, `return g();` neg/sbb; a shared `return false` tail is `if (p) {...; return true;} return false;`; separate failure returns share the success tail; an eax-wide 0/1 (Is_Dead) is `return(!a() && !b());` | |
| A plain `sete al` | `if (f() == 0) return(true); return(false);`; `return(f() == 0)` doesn't give it | step 8 |
| The client repeats a success tail (`Handle = 0; return true` three times) | A return inside each success block (one common tail, an `ok` flag or `&&` chains merge it) | `Try_Connect` 0x8290A0, 5285298 |
| Ours merges two failure tails the client keeps (`explain.py`: "the same callees, N vs M calls") | Open: `Listen` 0x82A470, the memconn option slot 0x826D00 (`attempts.py list --blocker shape`) | |
| The client leaves a loop by a jump to the single return | A `goto` to the single return | `Get_Log_Directory`, 6063d9e |
| `jb top` against `jae exit; jmp top` | A for loop gives `jb top`; `jae exit; jmp top` is a do-while behind a count test | the array templates' loops, 1945314 |
| `push_back` keeps 0 in a register and calls insert out of line | `push_back(T(x))`; three field stores into a named local don't | step 8 |
| **frame** | | |
| Each inlined copy has its own buffer slot | A function-scope buffer (`char item_name[256]` at the constructor's top) gives each inlined copy its own frame slot | 0x752E60 (frame 0x218, not 0x118) |
| The frame size differs (`explain.py`: frame) | The frame counts every local: a difference names a local too many or too few (a second RECT grew Make_Screen_Shot's by 0x10; the distortion draw's client frame is 0x18 bigger: locals not found) | 0x9182C0, 0x9961E0 (`attempts.py show`) |
| **constructors, destructors, vtables** | | |
| A destructor that is a lone `jmp` to the base's, or one that stores the vtable first | An implicit destructor stores no vtable; an explicit `{}` one stores it first | 0x4AA210 |
| Members stored before or after the vtable | An init list stores members before the vtable, body assignments after it (`GrowBy`, `HasWildcard` needed the body); the base's vtable store comes first and is dropped, the class's own is stored after its members | 0x4A8370 (bb241e2); the packet classes (builder B, 2026-10-25) |
| Virtual slots in the wrong order | Overloaded virtuals are laid out in reverse declaration order | dbgconn.cpp's two `Describe` (6063d9e) |
| A base function kept in a slot plus an empty own slot (`ret 4`) | A changed base signature hides the virtual: E&B's `Sound3DClass::Set_Velocity` takes two arguments (`ret 8`), so `SoundPseudo3DClass`'s and `Listener3DClass`'s one-argument overrides became new virtuals | f0a3f78 |
| **field offset** | | |
| `[reg+N]` differs in otherwise equal code | The displacement can belong to another class: `DX8PolygonRendererClass`'s `lea ecx,[eax+0xAC]` (client 0x84) was the mesh's, `MeshModelClass`'s layout. Trace the register before patching; `tools/re/layoutdiff.py` tallies offsets per class | |
| A `malloc`'d size larger than the class | Extra E&B members | `FastAllocatorGeneral` 0xE30, 9850f75 |
| **literal, floats** | | |
| One `exp` call where ours has two (or the reverse) | Two `(float)exp` of one argument merge, `expf` doesn't | |
| `fild qword` (high dword 0) + `fidiv dword` | Unsigned `(float)u / (float)v`: such output parameters are `unsigned` | |
| `mov dword [..], 0` among register-zero stores | That member is a float | |
| **reading the client** | | |
| `ret 8` / `ret N` with ecx set, or never read | Still a member function, even if it ignores `this` | |
| A body equal to two later functions | Both are inlined into it (Process_Messages) | |
| 0x9CCD10 (`xor eax,eax; ret`) in a vtable | A pure virtual slot: `= 0` | |
| A static function right after an unrelated one (misplaced start) | A static sits right after its first out-of-line caller | 0x4B43E0 after `Pop_Prefix` |
| C2660 on wwlib vector.h's placement `new[]` | Declaring `operator new[]` in a file makes VC6 resolve it there: declare the allocator's members instead of including FastAllocator.h | app/shared/unknown_new.cpp, 27812b5 |
| **levers found 2026-10-26/27 (S3, T4, W3, F3, L4, P4, H5)** | | |
| A register mismatch on a converted argument (`ntohl(*(int *)p)`) | Assign twice: `len = *(int *)data; len = ntohl(len);` | tcpconn.cpp (S3) |
| A pump loop duplicated / rotated by the compiler | `while (true)` where ours had `for (;;)` | tcpconn.cpp, udpconn.cpp (S3) |
| A callee the client inlines at one site and calls at another, nested | A nested inline method moves its own callees out of line (`RSAKeyClass::Decrypt` inline took the CRT helpers out) | `Pump_Pending` 0x82AA10, fb24feb |
| The client keeps a nested `_Construct` out of line | `std::distance(begin, it)` spends the budget that `it - begin` doesn't | `Remove_Dead` 0x822B50, 37028db |
| Locals share dead parameters' stack slots in the client | Declare the locals at the top of the function, not in a later block | udpconn.cpp (S3) |
| A virtual call in a destructor | `((Base &)*this).X()` | dbgconn (S3) |
| A dead `ntohs(Port)` / accessor call the client keeps | Keep it; constructor members in the initialiser list | udpconn ctor 0x82C680, 139c184 |
| Our inlined call that the client makes out of line (string::erase, size) | One or two dead `size()` calls *after* the call site push the earlier call out of line; only the first dead read inlines, the rest are called | `ClientTaskClass::Init` 0x766350, bb5578b (T4; D40) |
| Nested inline temporaries flip (the sixth `??_H` Transpose temp) | VC6 spends nested inlines in reverse source order: an inline cost placed *after* an expression (`Matrix4(view)` in a restore) decides whether that expression is inlined | distortion draw 0x9961E0, 342 -> 121 rows (W3) |
| The budget model (F3, confirmed by T3/H3/F5) | Function-wide, spent by every expansion anywhere in the function; unused plain locals add nothing; dead accessor reads add; template declarations visible in the translation unit (delta.h's include chain) move which instantiations inline, so a header change can gain and lose pairs in files that never use it. The budget is file-state dependent: a form that failed earlier is worth one re-run after the file's members change (L4's struct ctor broke the land ctor on 2026-10-26 and is exact on 10-27, F5) | Ship 0x6F66E0 probes; aux_roots 0x702240/0x7045F0 vs 0x7014B0/0x701D70/0x758210 (H3) |
| The client divides at run time where ours folds (`65535.0f / (WWMATH_PI/2)`) | A function-scope `static const float` reproduces the `fdiv` | `Prepare_Lighting` 0x46C730, cd3e56f (L4) |
| A caller's destructor gets extra EH states (7 -> 11) | Defining an out-of-line class's destructor in the same file inlines it into the caller's destructor; leave it declared only when its bytes occur at several client addresses anyway | land.cpp (L4) |
| A temporary for an `Add()` argument grows the frame | A cast temporary instead of a named local | opcode 128, 1c76fb5 (P4) |
| An order-only residue in a scheduled store pair | Model the 8-byte member as two ints (`Unknown68`) | opcode 108, 7d3e7fc (P4); candidate for 219's ctors 0x893970 / 0x893A70 |
| `std::string(const char *)` shows three inlining profiles in one file (`_Tidy` inline / `assign` out, the reverse, `_Eos` out) | Budget, not source: don't read the profile as a different form | tutorial_manager.cpp (H5) |
| A virtual `Set_Step` the client inlines | Call it qualified, `LoginTaskClass::Set_Step(...)`; a plain virtual call misses | tsk_login.cpp (T3) |
| A hashtable's `_M_delete_node` out of line in the client, inlined by ours; callers inlining more table helpers than the client | In a template VC6 compiles `p->~T()` by where `T` comes from: a function template's parameter (`destroy`, `_Destroy`) gives `push 0; call ??_G` (the scalar deleting dtor), the class template's own `_Val` in a member gives a direct `??1` call, and the callers then inline less. `_M_destroy_val` member in sgistl_adapt.py (H6, de316de: +10 whole-app, none lost). Members out of class, `inline`, `#pragma inline_depth`, allocator forms, find's body: 0 or losses | kv_list 0x4B6520, combo_ffactory 0x4E5230, lookup_table 0x4ECE50 |
| A hashtable's find called out of line in the client, the key's string constructed inline ("find out of line, key inline") | The caller-budget lever (H12, bfcc69a, 7 of 7): write the find as `const_iterator it = T.find(std::string(name)); return it == T.end() ? NULL : (*it).second;`. An iterator with two returns inlines find instead; callers that inline such a Find need no dead reads. Table-level changes don't help (H6, P-H6-1). Generalises to std::string-keyed finds: the `const_iterator` local is the essential part, the temporary, the ?: form and the `(*it).second` spelling are optional (H15, 4 of 4); it does not reach `const char *`-keyed tables (kv_list 0x4B6B60: no key-string destructor to order) | fontdatabase's finds and adds; ui_line_display 0x5A8B60 / 0x5A8C70, tutorial_manager 0x6ACF50, combo_ffactory 0x4E57A0 (H15, 0dabbce-e61ae6b) |
| Our loop hoists addresses into stack slots the client doesn't have; the client writes a temporary into a dead parameter slot | Move loop-invariant inline math (a Matrix3D multiply, Vector constructs) out of the loop: VC6 then spills nothing | `SegLineRendererClass::Render` 0x9BEB20, 5c24e98 (F4: one move after W3's 16 forms) |
| A repeated null test of a literal / 64-bit memory value: the client re-loads and tests again, ours threads it | Unsolved stop signal: VC6 keeps the second test when the tested value is a rematerialised constant or a 64-bit memory operand, threads it on a register value; 8 header and call forms plus a 30-flag sweep changed nothing. The lead: an E&B wwstring.h whose literal constructor does not call operator= | ShdMesh ctor 0xA08F80, 0xA0EC30; `Unknown_50` 0x7683F0 residue (F4) |
| Index-building loops: the client keeps a running row base | `int row1 = 1; ... row1 += nx` with the Adds in the client's order; also makes the static guards exact (`mov cl,al; or cl,N`) | distortion draw 0x9961E0, 1306 -> 1045 rows (F4; frame and loop registers remain) |
| An out-of-line loop function that is also inlined one level into its callers | A self tail-recursive `X_Rest(p, left)`; a plain `while` loop gets fully inlined | boolpars' `_Rest` functions (H8, d084b25) |
| A whitespace skip inlined at some sites, out of line at others | A macro, not an inline function | boolpars (H8) |
| A `static const int` read from memory where ours folds it | A class static defined after its uses | 0xAECC44, boolpars (H8) |
| `Value(...)` stores into a +4 subobject that keeps its own vtable | A member object, not a second base | boolpars' nodes (H8) |
| The prologue loads the arguments' members in the wrong order | A temporary or reference local that reads an argument member first decides the order: `const Vector3 &min = boxtest.SweepMin;` | HeightField Cast_AABox 0x465030, 727d349 (L5) |
| `[base + index]` operands swapped in a store | `mask[row + i]` with an unsigned row offset, not `row[i]` through a pointer | land's sea mask (L5) |
| The client copies a float argument before a bounds assert | An inline setter taking the float by value | spline.h Set (L5) |
| A second triangle's normal through TriClass | Set all three V pointers again before the second triangle | land Compute_Normal (L5) |
| A float constant loaded from memory where ours folds a literal | A named `const float` | attrib_dir (H7, 2026-10-27) |
| `v*(p*k+1)` folded where the client computes `v + (v*p*k + a)` | Spell it as the client computes it; read the old value before calculating | attrib_dir (H7) |
| A pointer-keyed hash find converts into a temporary per find | Keys as `unsigned int` | attrib_dir (H7) |
| Two FAIL_IFs sharing one exit | Nest them as `{} else {}` | attrib_dir (H7) |
| `pop_back` vs `erase(end()-1)` | The client uses both forms: try the other | attrib_dir_collective (H7) |
| A helper the client inlines, whose own callees the client calls | Route through the helper: its callees then go out of line | CharacterCreatorClass::Get_File_Name 0x78DE40 (T5) |
| A clamp: the client's `movsx` and compare | Write it through an int local: `v = x; x = v < 6 ? v : 6` | avatar_desc (T5) |
| A float copied through the FPU (`fld`/`fstp`) where ours uses `mov` | The value goes through an inline setter's parameter; a direct assignment gives `mov` | soundlib's dialog volume and Doppler members (H9, 1f32884) |
| An out-of-line 0x13-byte copy of an SGI hashtable iterator in the client | The `const_iterator(const iterator &)` conversion: the client declared a const_iterator | ui_line_display (H10) |
| `end()` read through the wrong register in a vector append | `push_back` and `insert(end(), x)` differ (`[ebx+8]` vs `[esi+0xa0]`): try the other | ui_line_display Flush_Lines (H10) |
| A callee the client calls out of line only inside an inlined function, inlined at the direct sites | Wrap the call in a one-line inline helper (depth +1): the budget drops the deepest nested expansion, not the biggest or the latest in source order (the depth counted is the chain's cost) | land's lightmap threads 0x46DA70 / 0x46DDD0, 9f51294 (F5) |
| A duplicated WWPROFILE destructor test at a branch's end | An early `return` in that branch | land (F5) |
| Both sums of a Vector2i computed before the two stores; `lea` addressing of the target | `Set(a, b)` on a reference local (`Vector2i &min = UpdateMin`); member stores don't give the `lea` | land (F5) |
| Register-only: the client loads the called object after the arguments | A named local for that object (`RsTrackClass *track = ...; track->X(a, b);`) | cpacket 0x737610, 0x737C10, 8235889 (T8) |
| `G_ASSERT(!"text")` tests the string pointer in the client | VC6 doesn't fold it: keep the form | cpacket (T8) |
| Levers from control_keys_manager (H11, 2026-10-27) | `push_back(&*it)` where `push_back(it)` misses (a `vector<T*>` fed from `vector<T>::iterator`); a reference local for `operator[]`'s result (`M &m = map[k]; m.Init()`); `end()` tested first in a find-returning getter; a copy walk over two iterators where indexing misses; the cleared-key case returning first; a struct with its own constructor where an aggregate initialiser misses | control_keys_manager.cpp, 84f61e0-fb9e15c |
| An SGI lookup result the client converts through `const_iterator(const iterator &)` | A `const_iterator` local; it also fixes the callers' budget | co_data's Has and Remove wrappers (H13, 8b47301) |
| One-byte temporaries where the client passes an empty tag type by reference | A fresh temporary per call | co_data (H13) |
| A dead `size()` read's position, not only its count, tunes the budget | One before the loop and one after: exact; two after: 0.81 | fontdatabase Save_Fonts 0x6C57B0, 23bdb2d (H12) |
| A class template instantiated in a header struct member moves a function's registers (`TPoint3D<int>`) | Define such structs in the .cpp after the affected functions | fontdatabase Load_Init (H12) |
| Calls in a `+` chain evaluated right to left where the client calls in source order | Separate `size += ...` statements | Get_Data_Size of 169, 171, 172, 82 (P6) |
| A base vtable store that survives in the client | A member initialiser reading through a reference that may alias `this` (a Matrix3D copy; an iterator whose constructor takes the container) | 30010, 888b60 (P6) |
| Several members reached through a pointer to a sub-block | Those members are a struct; helper members that ignore `this` (`ret N`, ecx unused) sit on the base | pkunknown_888b60 (P6) |
| The client loads packet fields or a member chain into eax and the other registers before the argument pushes | Read them through inline accessors | cpacket handlers 32-35, 66, 30, 98 (T10, c3b7f1b / 48b1d58) |
| An out-of-line "constructor" called through `??_H` (vector constructor iterator) without `mov eax, ecx` | A default-constructor closure: give the constructor default arguments | ControlKeyStruct 0x71BD70 (H14, b38289f) |
| The client shares one report call between two FAIL_IFs over an iterator | A `const_iterator` plus `key_ptr++` on its own line | Load_Keys 0x71BD90 (H14, 292c69f) |
| Nested table helpers out of line in the client, inlined in ours (a loop) | Wrap the loop in an inline helper (F5's depth lever, confirmed four more times by H14) | FontManagerClass's destructor, 0x6D1C10, Create, Set_Color (H14) |
| A packet destructor stores the class's own vtable in ours, not in the client | The implicit destructor (a declared empty one adds the store); where the client does store it, declare it | 129-133 implicit, 72 declared (P7) |
| A copy compiled to `rep movsd` in the client | A data struct member | pkrecustomize (P7) |
| A helper the client calls out of line from one packet | A file-static helper defined after its caller | 144's Get_String 0x872E30 (P7) |
| An aux-data lookup with `G_ASSERT(t)` (auxdata.h line 155) inlined in 84 callers | A template helper `Unknown_Find_Field<T>(root, name)`: `NameClass name(...)` a local in the caller, the lookup a separate statement (or the push lands before Get_Lookup), no ternary (setne/dec/and) | co.cpp 0x6F5E40 / 0x6F5ED0 / 0x6F58B0 / 0x7008B0 (T12, `.work/t12/aux_t12.diff`) |
| The client materialises `begin()` before a hashtable loop | Declare the iterator before the loop and assign it from `begin()` | inpdsp 0x429800, 0x429E50 (H18) |
| Separate failure tails after a find | An early `return false` when `find()` returns `end()` | inpdsp 0x429A20, 0x429D90 (H18) |
| The node construct of an operator[] push called out of line | Route the push through a one-line inline `Set_Stack` (the depth lever) | inpdsp 0x429AF0 (H18) |
| A member inlined at every site yet emitted out of line | A non-inline member defined after its callers | inpdsp 0x42A170 (H18) |
| A word load, `shr`, test-1 sequence on event flags | bool accessors on bit fields (plain shifts or bare bit fields give a byte test) | inpdsp 0x42A410 (H18) |
| An EH-tracked RefCountPtr local declared at the top of the function | Sends Find and std::string's `_Eos` out of line as the client has them (block-local: both inlined) | fontmanager 0x6D2700's calls (H16); try on 0x6D2580 |
| A large constructor loses a neighbouring inline after a header layout change | Not the changed statement: objdiff for a call that replaced inlined code first (a budget knock-on) | tsk_login 0x767240 under the char[] ship name (T14, d405690) |
| A string compare pushed out of line by a C-style reference cast between RefCountPtr types (it builds a temporary) | Cast through a pointer | cpacket ITEM_BASE 37 (T16) |
| `setne / test al,al` on a RefCountPtr test | `0 != ptr` (ref_ptr.h's template operator!=), not `if (ptr)` | client_lego_impl Init (H17) |
| `lea reg,[node+0x14]` on a table node's value | A reference local to the value: `T &v = (*it).second` | client_lego_impl Detach_All, Init (H17) |
| A frame 0x30 too small; int-move copies | A Matrix3D local (its user copy constructor); a float max as `min > r ? min : r` gives `test ah,0x41 / jne` | client_lego_impl Update_Radius (H17) |
| Two `sete` combined | A bitwise or of the tests: `(a == 0) \| (it == end)` | client_lego_impl Get_Lego_Transform (H17) |
| The const_iterator find lever on an int-keyed table | Works the same; a miss returned first | client_lego_impl Get_Bone_Map, Get_Knob (H17) |
| An aux-data type test (rtti.h, G_ASSERT 'b' line 32) inlined for some fields and called out of line for others | A class-static `UnknownRTTIClass<T>::Cast` (a function template with an explicit argument hits VC6's one-instance bug; a ternary gives the wrong shape); a cast nested three deep (wrapper, lookup, cast) exhausts the budget at the first expansions, two deep at the last; the dead-call lever (row 164) for the rest | co.cpp 0x700400, 0x6FFC80 (T17, 8b82d30) |
| A base's sub-block destructed in the wrong store order as plain fields | The block is a struct with its own inline destructor | client.cpp UnknownB02C90Class +0x0C (T17, 24d99bd) |
| A small callee the client keeps out of line in several callers | `for (int i = 0; i < 0; i++) {}` inside the callee: compiles to nothing, stops auto-inlining | sclient 0x745F30's four callers (T18) |
| Small residues in SClient's members | `++it` for `it++` (confirms ba71802); `&Packet(args)` passed directly instead of a named local; `while (true)` for `for (;;)`; a callback's vtable stored first: assign in the constructor body, not the initialiser; `snprintf` is VC6's `_snprintf` | sclient 0x74C690, 0x74BB50, 0x745350, 0x74BEC0 (T18) |

* **Small constant-fill loops and their written-out form differ (2026-10-28, CO1, 0x6F2820):** VC6 unrolls
  `for (i = 0; i < 5; i++) a[i] = 1;` into five stores with the 1 in a register, interleaved with the neighbours;
  five written-out assignments use another register and keep the stores together. Try the loop first.
* **A static inline helper called four times can use the inline budget up (2026-10-28, CO1, 0x6F2FE0):** a
  std::string's `_Tidy(true)` then stays out of line; writing the four blocks out fixed it.
* **Loops with the test at the top rotate and peel the first call (2026-10-28, CO1, 0x7251C0):** `while` /
  `for(;;)` over `strtok_r` peels the first `strtok_r(text)`; `do { tok = strtok_r(text, ...); text = NULL;
  if (tok) ... } while (tok);` keeps one call site.
* **`b ? x : -1` emits the x branch first;** `(b == NULL) ? -1 : x` gives the other order (2026-10-28, CO1, 0x725610).
* **Forms read from the listing (2026-10-28, PS1, postate.cpp):** a vtable store after the member stores is an
  init list; `esi = this + 8` is a struct assigned from a temporary; one call fed by a duration local comes out
  as four tail-duplicated pushes; an inline helper whose inlined and out-of-line copies must both match took
  `if (a == b) return(true); return(false);`; `Get_Position()` is out of line, the client inlines
  `Get_Transform().Get_Translation()`.
* **A destructor stores its own vtable first only when it is user-declared (2026-10-28, CR1, 0x6F3770):** if
  the client's destructor starts with the vtable store, write `virtual ~X() {}`.
* **A client function called directly, not through a `j_` thunk, is `static`** (2026-10-28, CR1, 0x6F51D0).
* **E&B's motion code (2026-10-28, EX1, extrap.cpp):** rotations are `In_Place_Pre_Rotate_Z` (rows 0 and 1,
  +8 / +0x18; a plain `Rotate_Z` leaves a field-offset residue); `float t = a; t += b;` gives the client's load
  order where `a + b` and `b + a` both don't; speed clamps are WWMath::Min / Max (by-value stack copy);
  quaternion compares are quat.h's `Equal_Within_Epsilon`; a returned Vector3 the client copies through a
  temporary wants an assignment, not an initialisation.
* **A nested inline is a whole-function budget (2026-10-26, `Key_Exchange` 0x829180):** under `/Ob2` an
  inline helper's own callees (a `std::set` iterator's `_Inc`) are inlined only while the function as a
  whole is under budget; removing unrelated code elsewhere in the function flips one nested call, so a
  missing nested inline is not fixed at its own statement. Byte readers with `len`/`offset` pairs: the
  client keeps a running offset (`offset += 4; offset += len`); `buf + len + 8` changes the arithmetic.
  A loader written in a constructor inlines every reader; the matching form was `KeyPair.Load(file_auto_ptr(...))`,
  and a named `file_auto_ptr` adds a 4-byte frame slot.
* **Inline-budget dead reads count as expansions, not bytes (2026-10-27, D93 eval):** `size()`, `length()`, `c_str()`,
  an inline getter all had the same effect, and only unreachable code after the last `return` moved anything
  (B-opus, Get_String 0x7F9700). A std::list destructor that deletes its elements then empties the list has three
  working forms for the client's inline erase with an out-of-line `it++` (0x886C20): `erase(begin(), end());
  PlayerList.size();` (A-opus), `++it` in the delete loop plus `clear()` (A-sonnet-1, A-haiku); `clear()` alone
  inlines neither, `erase(F, L)` alone inlines both (FIND_MEMBER 0x886890).
* **`#pragma auto_inline(off)` around a helper keeps it out of line where the empty-`for` lever does not (2026-10-27,
  B-sonnet-1, Set_Defaults 0x7F1AD0).** In a long function, extra inline calls or dead code anywhere move the
  inliner's switch to out-of-line calls earlier, never later. It also keeps a same-file destructor out of
  line when called from `delete` and from `delete []`'s __ehvec_dtor (2026-10-28, LD1, ~LandPatchClass 0x474560;
  an inline candidate there added 3 EH states to the caller).
* **Register levers from the packet rows (2026-10-27, D93 task A):** an entry pointer cached in a named local
  (`entry = *it`) before several `raw->Add` calls made Build 0x8869F0 exact (three builders independently); the
  client's `Add(..., char, char)` takes `movsx` arguments: declare them `char`, not `bool`; a `Vector3` member
  copies register-wise (z first for a Default copy), a `TPoint3D` as a structure.
* **Declare-then-assign iterator (2026-10-27, H22; P-H10-1 closed):** `T::iterator it; for (it = X.begin(); it != X.end();
  ++it)` keeps the iterator in registers and lays out begin()'s first-bucket search as the client does in hash_map / map
  delete loops: 0x5A7E80, 0x5A8100, 0x677410, 0x6ACD10, 0x5A58A0, 0x4D4490, option.cpp's 0x7FC600, and (inside an inlined
  helper) the LIMIT row 0x4B3850. Per function, not per file: a 30-file sweep gained nothing else and loses where the
  client uses the in-`for` declaration (fontdatabase, soundlib 0x43EA10, unique_design_id_lookup 0x5005F0). The extra
  line moves later `__LINE__`s: pair it with a removed blank line, or test it on the `for`'s own line first.
* **Out-of-line here, inlined there (2026-10-27, H19 / H21):** `#pragma auto_inline(off)` on the function plus an inline
  twin for the inlining sites (fontmanager 0x6D2580); a success test after the `if (x)` block, not inside it, gives the
  return-value EH flag (0x6D2700); a reference parameter copied into a plain local keeps ecx = `this` across a thiscall
  (`unsigned int key = id; Find(key)`). Dead reads at the top of a wrapper set an inline threshold that can be searched:
  3 too few, 4-5 right, 6 too many (R2, rtti.h's cast 0x598D80 out of line).
* **Tail merges and folded displacements (2026-10-27, S1):** when the client keeps the first of two identical failure
  tails, nest the first case's success path (`if (x != 0) { ...; break; } return(false);`), not both (0x826D00). An `int
  index` local subscripted twice keeps the -1 out of `[reg + idx*4 + disp]`; a slot pointer or an inline index helper
  folds it (0x82D8C0). Two RefCountPtr locals zeroed under one EH state are a struct local (H20, client_lego_impl).
  Separately: a `const_iterator` sent find out of line in co_player_relationship's lookup where this client inlines it
  (plain iterator, `it == end()` tested first: R1); an empty `~T() {}` on a value type adds EH states.

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
* **VC6 C1001 deducing `T` from `hash_map<std::string, T*>&` (2026-10-27, option.cpp):** template the helper on all
  of hash_map's parameters, or on the whole map (`template<class M> void f(M &)`); `T::iterator` in the
  signature also ICEs. VC6 has no `snprintf` (use `_snprintf`): the miss shows only as "1 errors" in verify's
  summary line and gains nothing. A VC6 `std::list` member at +0x10 puts its allocator byte there, head +0x14,
  size +0x18.

### 3.3 E&B app code

* **Layout clues from a NOT EXACT function** (2026-10-29, LD4): a wrong `push <size>` before the call to 0x4E9CB0
  (`new`) means the class's layout is wrong; a member declared as a raw pointer where the client's is a RefCountPtr
  (read through Peek) costs registers. A friend Create_Get / Create_NEW is found only once RefCountPtr<T> is
  instantiated: `enum { X = sizeof(RefCountPtr<T>) };` instantiates it without emitting code.
* **client.h's partial EffectObjectClass clashes with eff_o.h (C2011)** (2026-10-29, SC3): include eff_o.h before
  client.h / sclient.h; to add the include at the top without moving asserts, shorten the file's opening comment by
  a line. `#pragma auto_inline(off)` / `(on)` around a same-file callee the client calls out of line took 0x7511A0
  from 0.28 to 0.89 (put the pragmas on existing comment lines).
* **A base virtual whose signature differs from a subclass's same-named virtual adds a slot** (2026-10-28, UP1):
  it shifts the subclass's later slots (`Unknown86CRecordClass::Unknown_08(const Vector3 &)` against
  `InfluenceClass::Unknown_08()` lost CREATE 0x733700). Grep the subclasses before declaring one.
* **The client's HashList loops test only `Next()`** (`while (node && node->Next()) ... node = node->Next()`);
  `HashNodeClass::Next_Valid` adds a Prev test (`[eax+8]`) the client never has (2026-10-28, UP1).
* **aux_roots.cpp's constructors move with the translation unit's declaration count mod 64 (2026-10-28, HX1;
  replaces H3 / H6 / T5's swap and the 0x6FB1E0 canary):** every declaration (a header member, an enumerator, an
  `extern`) shifts the six constructors at the file's end (0x6FB1E0, 0x702240, 0x7045F0, 0x7014B0, 0x701D70,
  0x758210) along a cycle of 64 of VC6 tie-breaks (scheduling, stack-slot order); each is exact only in a window.
  The lever is `tools/re/auxroots.py`'s `SYMBOL_PAD` (an enum of N enumerators at the end of aux_roots.cpp,
  marked as not the client's; 16 since 45bf5c0, window 15..17). After any change to a header aux_roots.cpp
  includes, re-sweep the pad (`tools/vc6/auxpad.py sweep`, aux_roots.cpp alone, ~5 s a variant) and keep a value exact for
  all six. Windows are not pure shifts: a template declaration can move them apart so no pad exists (DeltaArrayClass's
  bounds accessor, P-DF1-2: keep such helpers local to the file). Explicit `template class` instantiations and
  padding between definitions move all six alike; VC6 rejects explicit instantiation of a constructor (C2909).
* **co.cpp's lookups and the inline budget (2026-10-28, UP1 / DF1 / IB1):** the budget cuts the bigger callee
  first and same-size callees in source order, so earlier finds go out of line first. auxdata.h's find is the
  client's early-return form (rtti.h's test inverted: `!= global -> NULL`; fc348e7). The lever for co.cpp's
  finds is a helper `Unknown_Find_Aux((C *)NULL, this, "Name")` that builds the NameClass before reading AuxData
  (one shared stack slot): 0x6F4610 112 -> 4 rows, 0x6FDBD0 34 -> 10; used everywhere it loses 20 exact
  functions, so the small finds keep their explicit NameClass. `#pragma auto_inline(off)` and `/Ob1` do not stop
  Unknown_Find_Field's inlining; for an out-of-line copy to compare, write a dummy with 40+ calls. A helper put on
  a new line moves every later `__LINE__` (~13 pairs): use an existing blank line.
  The client's find-if-set helper (bool copy 0x705450) is auxdata.h's find followed by "NULL unless +0x70 is
  set": co.cpp's `Unknown_Find_Valid` (DM1, acd5e81: 0x6FE1E0 96 -> 40 rows). VC6 did not inline 0x6FEAC0 into
  0x6FCB70 while 0x6FEAC0 was defined later in the file: an inline member defined before both reproduced it. Frame
  bytes the client has and ours lacks can come from a by-value getter's temporary (`Get_X_Vector()`, 0x6F4290).
* **Do_Frame's lost-connection dialog matches only as an if / else of two complete calls** (2026-10-28, DF1).
* **udpreplay compiles its app files as one translation unit:** a header's macro gate is defined at the top of
  app_sources.cpp (udpreplay.py writes it, 2026-10-28, PK1).
* **The client puts the non-null case first wherever it tests `x == NULL`** (2026-10-28, EF1: 0x6E78A0, 0x6E8B60,
  0x6E8DD0): write `(n == 0) ? A : B` the client's way round, or `if (p) {...} else {null case}`.
* **An inline accessor in a call's argument list is evaluated before the other arguments** (`Unknown_Time(Client)`;
  fixed register residues in 0x6E6AD0 / 0x6E7080); an inline getter (`Get_Flag(bit)`) spends inline budget and can
  push the client's other callees back out of line (0x6E7ED0) (2026-10-28, EF1).
* **A member named `Node` hides wwlib's `Node<T>`: write `::Node<>`;** a watch node built as `DataNode<T*>(NULL)`
  gives the client's store order where `Node<T>` plus an int member doesn't; a local stand-in class for another file's
  effect overrides the base's pure virtual (C2259) and is padded to the client's `new` size (2026-10-28, EF1).
* **objdiff and explain.py mask jump targets** (2026-10-28, EF2): "identical" there while objmatch says `none` means
  compare the raw bytes; in 0x7396F0 two je displacements to a shared failure exit were the whole difference:
  `if (!x) return(true);` per test gives the client's exits.
* **Small forms that moved registers (2026-10-28, EF2):** a Get_String size of 251 means `char[252]` (char[256] added
  a stack slot, 0x6EB9B0); `if (x < 0.0f) x = 0.0f` compiles to `test ah,1`, `WWMath::Max(x, 0)` to `test ah,0x41`;
  which pointer is named decides the register (a named ClientObjectClass * gave `mov ecx,[eax+0xAC]`, 0x733590);
  element-wise copies give the client's integer moves where Get_Z_Vector() / Set() don't (0x6E82A0).
* **`#pragma auto_inline(off)` / `(on)` around a same-file, non-inline definition keeps /Ob2 from inlining it into
  later callers** (2026-10-28, CS1: co.cpp 0x6F3ED0, 0x6F3FA0 0.35 -> 0.62): put the pragmas on existing blank lines so
  no assert moves; it does not reach in-class or inline functions (rtti.h's `Cast`).
* **After an inlined float-returning call** (2026-10-28, CS1): `fld x; fsub st(1); ...; fstp st(0)` is a named float
  local holding the result, `fsubr` the plain expression; a named pointer local (`HeightFieldClass *field = ...`)
  decides which register holds the object and which `&pos`.
* **An implicit copy of an anonymous union `{bool; char[]}` copies every member** (bool first, then the array loop)
  (2026-10-28, PK2): write `operator=` out inline instead of changing the layout; an `int` index gives `jl` where
  the client has `jb`: use `unsigned`. objdiff masks jump targets, so compare masked bytes when objmatch says `none`
  (`.work/pk2/patcmp.py OBJ SYM ADDR`).
* **A `std::string` member set in the initialiser list (`Message("")`) puts the vtable store after the string's
  `_Grow`**; assigning it in the body gives the client's order (2026-10-28, PK2: 0x864A10).
* **A float argument's form names its declaration** (2026-10-28, MV1): `push imm` is a literal; `mov reg,[mem]; push`
  with duplicated .rdata values (0.95 several times at 0xAECEF0-0xAECF40) is a named `static const float` (0x50CC10,
  0x50CE00 exact). A hidden output pointer as first argument plus two Vector3s in the frame is a Vector3 returned by
  value (0x50EE60 0.42 -> 1.0); a float copied with `fld` / `fstp` instead of `mov` comes from an inline float getter.
* **A float member passed by value after a by-value virtual call matches only through a named local**
  (`float radius = Unknown04; Update(Unknown_24(), radius);`): the client's extra stack slot is that local
  (2026-10-28, DO1). At a file boundary, check the `__FILE__` the function pushes before trusting asserts.txt's file
  (px.cpp's 0x44BA70 is filed under dynamic_obj.cpp).
* **Forms seen in px.cpp / px_wrapper.cpp** (2026-10-28, PX1): a run of `mov [x], 0` immediates with no zero register
  is float members set to 0.0f (typed `int` they give `xor reg` and register stores); `lea reg,[this+N]` then stores
  through reg is an inlined `memset` of a member array; a find loop loading `end()` before `begin()` is
  `std::find(begin, end, x)` (arguments right to left), with the not-found return first; whether VC6 inlines
  Is_Valid / Fabs depends on the wrapping comparison: a static `Is_Close(const Vector3 &, const Vector3 &)` made
  0x456900 exact.
* **A VC6 friend like `Create_Peek(T *)` binds to whichever RefCountPtr<X> is already instantiated** (2026-10-28, HN2):
  a cast argument can pick the wrong one (C2440 in ref_ptr.h:249). Before adding a class to sclient.h, grep the app
  .cpp files for a local definition (sclient.cpp defines PDAInterfaceClass itself). To keep a file's line count,
  declare a destructor on its constructor's line. tryv2 variants that add or remove a line show false losses
  (0x514270 / 0x514390): keep variants line-neutral.
* **Check Renegade's Commando / wwphys for E&B debug code before rebuilding it by hand** (2026-10-28, MV2): a local
  `static StringClass` pair and the WWProfileIterator calls matched console.cpp's profile display byte for byte
  (0x510440); the sea tint is phys.cpp's `#if 0` quad (0x510D40). co.cpp's `Unknown_Find_Valid` is local to co.cpp;
  `DeltaTemplateClass<int>` needs aux_roots.h for `Type_Name(int *)` (else C2665 in kv_pair.h).
* **More px forms** (2026-10-28, PX2): `float d = other->Radius; d += r;` gives `fld other; fadd r` (0x4534A0); a
  one-case `switch (Type)` in an inline helper gives the client's adjusted `this`; `PxObject *o = ...; Vector3 pos =
  o->Pos;` against a `const Vector3 &` decides the client's `add eax,4` (0x452D50 / 0x452020); a cached loop end is
  `for (it = begin(), end = end(); it != end; ...)` (0x44F2F0). Guarded `<string>` statics bound app\shared objects
  too (0x451BD0, 0x452750): a way to find a string-less object's range. To tell live code from dormant, look for
  call, jump and pointer references to the function and its incremental thunk (`.work/px2/thunkrefs.py`).
* **sclient.cpp forms** (2026-10-28, SC1): `Create_Peek(X *)` needs `RefCountPtr<X>` instantiated earlier in the file
  (VC6's injected friends exist only after instantiation; a `sizeof` typedef does it); a body the client repeats
  inline (0x744D80) is an inline member defined before its callers; a packet class the client destroys out of line
  needs a declared `virtual ~X();` defined empty; `Send_Packet(&TempPacket(...))` is a temporary (a named local costs
  rows). Rebuilding a callee can pull a whole object into clienttest's link (control_keys_manager.obj needed a
  0x42A1A0 stand-in).
* **cpacket.cpp handler forms** (2026-10-28, CP1): an early `return(true)` at a branch's end keeps per-branch tails
  VC6 would merge (65 exact, most of 62); two exits testing the profile flag in bl are early returns (160); a ternary
  argument `f(c ? a : b, 1)` puts the result in a saved register where the client has if / else of two whole calls
  (234 / 235); a FAIL_IF's text is `#expr`, so a helper that matches the bytes but changes the condition's tokens
  changes the client's string: don't keep it. Which Is_Valid_Float calls go out of line follows the inline budget
  (62: Orientation.W, Position.Z, Orientation.Z, ImpartedVelocity.Z in that order).
* **VC6 lays out switch cases in source order** (2026-10-28, CP2): read the jump table's targets against code addresses
  and write the cases in the client's code order (0x73D170 exact in one step, 33 cases). An early return on a
  RefCountPtr temporary matched as `if (a == NULL || a->Get() == NULL) return(true);` (a nested `!= NULL` gives setne
  for sete); a field copied through a named block local fixed a store-order residue (TRADE 31).
* **A neighbour's FAIL_IF / G_ASSERT line shows where a function's source ends** (2026-10-28, SC2): 0x74EB00 had to
  end before 0x74EDB0's line 4804, which pointed at the inline 0x750210; the blank run before a file's notes can
  hold a later function's asserts (`appfile.py show`: 0x7518A0 owns lines 5459-5470). `int id = list.front();` before
  `std::find` and a named local for a singleton gave the client's forms (0x74FA30 / 0x74FEC0, 0x751460).
* **Asserts sit on the client's lines (D25).** To add code above a logged line, remove as many
  blank lines from the same function's stretch (cordmgr.cpp; `.work/milestones/a3/relines.py`);
  pad inside a function, not only before it (Get(char **); pk.cpp's helpers take single blanks).
  `appfile.py match` markers replace lines, never add them.
* **co.cpp's constructor calls the aux-root factory 0x6F6000** (2026-10-28, CO1): a test program linking co.obj
  then drags in aux_roots.obj (LNK2005 on delta, name, lookup) unless it has a stand-in, as clienttest does.
  A newly built app file can clash with a test program's hand-written stand-ins in the same way: the rebuilding commit
  deletes them (postate.obj vs client_stubs.cpp, 24 LNK2005).
* **`__FILE__`: §2.1;** header strings stay as they are (D41).
* **Client-style virtual calls on members:** a call through the object's own class (reference
  included) is devirtualised; cast to the BASE class reference (`((DeltaClass &)EndTime)`,
  `((VectorClass<T> &)Entries).Clear()`).
* **Files that profile need `#define ENABLE_WWPROFILE`** (Encrypt_Data 116 bytes without, 355 with).
  A `new` loses its EH state when the same-file ctor can't throw; a same-file callee may inline.
* **Assert names can be reversed** (`readbytes == bytes` is `cmp ebp, eax`, the wanted size in
  `readbytes`); FAIL_IFs sharing a string share a variable name.

* **app\packets:** objects are linked in alphabetical order. The guarded static at the end of every
  object (`mov cl, [0xC416FC]`, atexit 0x42B320) is `std::ctype<unsigned short>::id` from
  `<string>`: a reliable file boundary. `tools/re/pkdecode.py --report` says why a class doesn't
  decode yet; `pkemit.py --write` drafts the files (builder B, 2026-10-25).

* **A class holding an `__int64` needs `#pragma pack(push, 4)`** or VC6 pads the vtable pointer (ClientClass 0x1150
  bytes, tsk_game.h; T6, 2026-10-27). An inlined FAIL_IF can carry another file's `__FILE__` (co.cpp's line 2461 inside
  client.cpp's 0x7283B0): explain.py classes it as relocation; a source of that file cannot produce it.

* **A static member `hash_map<K, OwnClass>` declared inside its own class fails with VC6 C2079** (H11): define it at
  file scope. Byte-identical twins that differ only by a relocation (0x71ADF0 / 0x71AE20) count only once their caller
  is in the file. A tryv2 variant that changes the line count moves every later assert (accessors show as "lost"):
  keep variants line-neutral by joining statements on one line.
* **iostreams (H8, 2026-10-27):** `std::stringstream` instances compile byte-exact from VC6's own `<sstream>`
  (unique_design_id_lookup.cpp, 115 client instances 0x500AB0-0x509C40): other iostream users follow it; the three
  that still differ (operator<<(int), operator>>(int&), stringstream::str) are library code, no extra include helps.
* **One `__FILE__` window can mix two files** (appfile.py): `.work/h8/layout.py` places each `#line N` from a draft
  and pads inside a function when several asserts share it. explain.py reports a static's atexit `$E` as a false
  relocation before the real class.
* **A `VectorClass<T>` member needs `operator==` / `operator!=` declared on T** even when no constructor is emitted
  (VC6 compiles VectorClass's virtuals; declaration-only operators on CameraModeStruct; T7, 2026-10-27).
  Any program that constructs an SClient needs a stub for the registry read 0x75A280 (D69).

### 3.4 Importer, verify and pairs

* **A struct change can move an unrelated struct's identical COMDAT** (2026-10-29, LD5): a 32-byte element's
  _Construct matched ColorizationRegionStruct's identical copy (0x62A560); changing the element moved the pair:
  a D66 (i) move, not a loss. `wt.py bring-over` refuses it (P-CS1-2): land such a commit through commit_mine from
  the main tree with the worktree-verified imports.json block.
* **A tryv2 variant's gains can be another type's identical code** (2026-10-29, PK4): KeyMappingStruct with a
  user operator= "gained" vector<Vector3>::erase 0x47D3D0, with a copy constructor 0x4667D0 / 0x466820: folding
  onto a twin, not progress. Check that a gained address belongs to the function you changed.
* **names.py rewrites rows outside your own files** (2026-10-29, CS2): clienttest's client_stubs.cpp rows (line
  numbers shifted) and the score-5 count. commit_mine takes only your rows; restore the leftover diff afterwards.
  docs/imports.md on HEAD can be stale (`importer.py report` regenerates ~100 unrelated lines): don't sweep that
  into your commit; the coordinator regenerates it.
* **objdiff shows the client's float immediates as REL** (2026-10-29, LD4), so its row counts overstate a
  difference: judge by pairs_cmp.
* **`commit_mine.py --file` leaves out imports.json's `in_tree_total` line** (2026-10-29, SC3): add
  `--refresh-totals`, or the count needs a follow-up commit (a45ccfb).
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

* **`appfile.py match` on a draft:** A draft placed next to its
  original (so relative includes resolve) leaves a `<dir>/obj/` folder: delete it before replay.
  `tools/vc6/mkspec.py` splits paths as `lib/file` and fails on `libs/wwonline/...`.

* **Compare against HEAD's imports.json, not a saved working copy:** another builder's verify rewrites the
  working copy with your uncommitted tree already in it (`git show HEAD:docs/data/imports.json` is the before-copy;
  2026-10-26, L4, F3). Compare over the whole app: a template header visible to other files moves their pairs.
* **commit_mine.py hunks (2026-10-26/27):** keys must cover every `-U0` hunk of yours, and a key matches an
  identical hunk anywhere in the file (c9d9e0c missed two; P4 found another builder's identical line taken):
  after the commit `git diff docs/data/imports.json` must no longer show your addresses.
* **VC6 hits C1001 on out-of-class specialisations of member templates and mangles member-template functions
  without their template arguments** (P7): a stand-in defines them as plain overloads in its own minimal class
  declaration (tests/capharness/udpreplay/shipcust_stubs.cpp). `appfile.py match --diff` works only on a marked function.
* **Including a packet header mid-file in tsk_login.cpp turns a handler order-only** (P9: 0x76B520 with 114's header):
  keep the one-line views with the real signatures (P-P9-3). A flag parameter the client stores with `movzx` is
  `bool`; a cast temporary for a count leaves the frame 4 bytes short: name the local (118, 112). P6's commit_json
  method appends new entries at the end of imports.json's sorted maps (a duplicate key after another repair,
  9593edb): `.work/p9/commit_wt.py` re-sorts and refuses until HEAD parses; checks.sh is still to promote (wt.py covers
  the setup). pkclasses.py can write a handler stand-in twice into pk_ctor_stubs.cpp
  (the OVERRIDES section; fixed 95c07a1). Same-shape template instances stay `multiple` until their relocation
  targets have names in symbols.json.
* **A pack(4) member changes a packet class's sizeof (0x5C for 0x60) and the factory switch 0x869790 in pk.cpp stops
  matching** (P6, 2026-10-27): check it whenever a packet layout changes. Put a needed `#include` after the asserted
  code so the asserts keep the client's lines. Python `replace` with `\n` in a CRLF header gives mixed endings.
* **Packet stand-ins belong in tests/capharness/udpreplay/, never stubs/app_stubs.cpp** (P5, 2026-10-27): a stub that
  calls RawPacketClass drags pk.obj and ~280 externals into VIDTEST's link (ee8fa6f, fixed 0f87385). A new header line
  above pk.h's PacketClass shifts its assert lines and loses ~400 pairs. Game-editor packets whose Process_Packet
  returns true are not in the switch, so pk_classes.h does not list them: find them by unmatched objects.
* **Between a worktree merge and commit_mine another builder's verify can rewrite imports.json** (1e35db6, 4fd16e4):
  check HEAD's in_tree for your files after every commit.
* **A command chain that commits after `replay` must stop on "N files differ"** (L3 committed through it; the
  difference was another builder's untracked files). Draft `obj/` folders left under app/ are reported by replay.
* **D66(i) in practice:** a rewrite that stops emitting an inline COMDAT copy drops that (file, address) pair while
  the address stays matched elsewhere (land's 0x447C40 / 0x47F300 / 0x7F0EC0, c0f8569): name them in the commit.

* **Template bodies in a header that tsk_login.h includes broke kerneltest** (T5).
* **Worktree verifies:** `wt.py new` / `wt.py bring-over` (67c2b7d).

* **clienttest builds only committed files** (T11): in a worktree, `git add` a new file before `build.py`. A
  `RefCountPtr<T>` member in a header breaks every file that builds the class without T complete (clienttest):
  keep it a raw pointer until the constructor is rebuilt. Levers that matched co.cpp (`if (!Is_X()) return x;`,
  the one-case switch, `Get_Float` / `Set_Floats` / `SphereClass::Init`) are in co.cpp's notes.
* **Never split attempts.csv on line endings** (T12 truncated 157 rows): its rows hold quoted multi-line text; use
  `attempts.py` or the csv module. `build.py --lib` refuses a dirty library: commit a detached WIP inside the
  worktree before `build.py` / `clienttest.py build`. A header `__FILE__` the client keeps relative
  (`..\shared/auxdata/auxdata.h`) defeated D80's tie-break until f308ceb read the client's string to its NUL.
* **A zero-context imports.json patch applied after master moved lands lines in another block** (H18's 2af9be0,
  repaired in 89315ee): a commit script must refuse unless HEAD equals the verify base; `commit_mine.py --file NAME`
  (3772169) selects by block span instead. In Git Bash prefix a command taking `// 0x...` arguments with `MSYS_NO_PATHCONV=1`; in VC6 `#line N`
  alone switches `__FILE__` to the full path and `#line N "..\\shared/..."` needs escaped backslashes (T15).
* **A verify worktree has no built app.lib** (P8): run `build.py build --lib app --allow-dirty` there (the worktree is yours alone) before `udpreplay build`.
* sgistl's `insert_unique_noresize` fails in VC6 (C2899):
  `hash_map::insert` is unusable, the client's inserts go through `operator[]` (P-H17-1). A file's canary function
  (client_lego_impl: the bone map constructor 0x72D5F0) flips with the file's contents: check it each verify.
* A tryv2 PermissionError (a concurrent replay) leaves a variant in the tree
  file: regenerate it. Verifies made while other builders' files fail to compile add COMDAT instances to your
  file's ambiguous list.
* **A generated docs/data file committed whole by another builder brings back rows you removed** (T10's b0104e7 to
  not-exact.md; H15): after the others commit, check HEAD's generated files still hold your change. Record the second
  of two `attempts.py record` rows only after the first commit, or name the carrying commit.
* A multi-line
  `// NOT EXACT:` comment above asserts shifts `__LINE__` (10 tsk_login pairs lost): keep it to one line.
* Each builder passes `--agent NAME` (or works in a wt.py worktree): scratch is private (999f780).
* **`attempts.py record` has no `--lever` (2026-10-27, F7):** put the lever in the evidence text; `attempts.py check --lever` takes it.
* **tryv2.py** writes the tree file back with `open(..., 'w')` and can fail with
  OSError 22 when another process holds the file: the tree stays intact but later variants are skipped silently, so
  check the log's variant lines. importer.py is CRLF: a patch-insertion script must convert its anchor newlines.
* Commit before restoring imports.json: `git checkout docs/data/imports.json` after a verify discards the gains
  commit_mine still needs.
  `build.py build --lib app` refuses a dirty library; `--allow-dirty` tests uncommitted files.
* **A template copy an eval or trial pairs to file X may come from X's own non-exact code (2026-10-27, O1):** 0x603D70
  (`std::string::assign(const char *)`, 17 callers all in 0x5FBD30) paired to option.cpp on two Sonnet branches only
  because their non-exact Set_Defaults called it out of line. Before crediting or porting one: `client.py callers ADDR`
  and the obj's relocations (`.work/o1/who.py OBJ SYMBOL`). A COMDAT placed after file F's last function puts every
  lower caller in F's object (H21); one placed inside a window can belong to code the linker dropped (H19, 0x42C330).
* **Shared-tree verifies show other builders' uncommitted work (2026-10-27, R1 / H21 / H22):** their compile errors appear
  as losses and their gains as yours; commit only your blocks. `attempts.py
  record` takes `--score` float, `--forms` int, and a function-only `--stash`. A file-static reached by displacement
  (`[GutterColors+8]`) is unrelocated in objdiff: make it a global (R1).
* **symbols.json regeneration is not neutral (2026-10-27, M2 / M3):** symbols.py used to promote D88's self-named functions to
  `source: exact`, deleting the evidence that made them exact (-323 pairs); it now keeps the proof (`proof`, ae85fb4).
  Regenerate only with a full verify of every library. An object's client range never comes from imports.json's
  `in_tree` (it holds COMDAT copies elsewhere): use the object's own non-COMDAT / NODUPLICATES exact functions (M1).
  `build.py build --lib app --allow-dirty` compiles tracked files only: `git add` a new .cpp first (M1). Park scripts go
  stale when part of the work lands separately: diff the park folder against HEAD before using it (M1).

### 3.5 Build VMs and vc6.py

* **tryv2 without `--pool` uses `.work/vc6_hosts.json`, which can name a stopped VM** (2026-10-28, LD2): lease
  one and set `ENB_VC6_HOSTS=Daniel@IP`.
* **Address VMs by IP** (`ENB_VC6_HOST`/`ENB_VC6_HOSTS`, from `pool.py`); after a restart
  `vc6.py discover`.
* **vc6.py syncs only `SOURCE_EXT`** (else C1083). `#import`'s `.tlh` holds absolute
  `c:\enbdevelopment\_vc6\obj_*\` paths: strip them before committing.
* **Linking.** `/MT` needs `LIBCPMT.LIB`, not `LIBCP.LIB`; mss32's import library is
  `tools/vc6/mkmss32.py`'s (D51). `build.py` builds committed files only and, without `--report`,
  overwrites `.work/milestones/l1/unresolved.txt`. A stub constructor emits its vtable, so every
  virtual needs a stub: link every object.
* **Checked builds** (`ENB_CHECKED=1`) change mangled names: build every library the same way.

* **Pool leases (2026-10-26/27):** a lease can fail for 25 minutes with "Unable to allocate 4096 MB" while a
  memory-heavy program runs on the host: retry in a loop. Never pass `pool.py lease --max-age H`: it reclaims
  the other builders' leases (H5 released P4's). Lease holder pids die across an API outage, so the lease is
  reclaimed silently: re-lease and check `pool.py list`. When every VM is leased, verify with `ENB_VC6_HOST`
  pointing at your own lease and no `--pool`. A worktree verify on a freshly leased VM can fail with "cannot open
  linearbu.h": delete `%TEMP%\enb_vc6_synccache_<ip>.json` or run a full verify first.
* **tryv2.py and an untracked include directory:** another builder's uncommitted `#include` of a directory the VM
  sync doesn't know breaks every builder's tryv2 compile (C1083) with no change in your tree (libs/sgistl while
  untracked). `.work/f3/tryv3.py`'s EXTRA_INC is the fix to promote.

* **The VM's `e:\build\beta\G` junction pointed at the last worktree that compiled on it** (H7, 2026-10-27): a
  main-tree verify then compiled another builder's worktree and new files failed with C1083. vc6.py makes the
  junction again on every compile since ab8355a; results from enb-vc6-02 between 04:36 and 05:25 were suspect.
### 3.6 RE tools, Ghidra and REA

* **Sweeps: take addresses from attempts.csv or the NOT EXACT markers, not gaps.py** (2026-10-29, PK4): gaps.py's
  app pairings are often wrong (a 2-3 instruction client window, or a neighbour's address: 0x4EDAD0 for
  0x4EDBE0). Counting `rep movsd`: a struct copy has `mov ecx, imm` before it; an inlined string memcpy has
  `shr ecx,2` before and `rep movsb` after.
* **`gaps.py` pairings can be wrong** (whole objects via shared helpers, overloads, neighbours,
  template dtors): check `ret N`, confirm with `objdiff.py` and `vtables.py`.
* **`objdiff.py`'s `*` lines include relocations** (`0` vs `REL`) and `__imp__` calls as `call
  REL` (`objrel.py`); pass mangled names.
* **The packet factory's vtable slots point at incremental-link thunks** (0x40xxxx, e.g. slot +0x08
  `Process_Packet`): resolve them with `ClientImage.incremental_thunks()` before looking the target up in
  imports.json (PF1, 2026-10-28; `.work/pf1/handlers.py`).
* **`vtables.py --seed Class=ADDR`** for classes whose code doesn't match yet; rerun after
  `symbols.py`.
* **Pseudocode can be wrong** (check `client.py dis`); REA can't decompile vtable-only functions
  (0x9961E0, from `.rdata` 0xB13984): `pseudocode.py --show`.
* **appfile.py:** a draft-folder copy shadows `"x.h"` but not `"..\x.h"`; in-class inline markers are ignored; a following static can enlarge the client
  size (PREFIX); same-named overloads get mixed up: rename while drafting.
* **Names lag.** `symbols.json`/bytematch miss recent matches (read the verify objects); asserts.txt
  and funcmatch take the nearest path, not `__FILE__` (0x829C80 is tcpconn.cpp's ctor). names.py
  reads the working tree, tracked files only: in a shared tree use a clean `git worktree`.
* **Tool changes:** compare outputs on a snapshot of the verify objects. Starts are verified (D27).
* **`tryvariants.py` edits one file** (wrap header changes in backup/restore). `client.py
  callers` on hot functions: `PYTHONIOENCODING=utf-8`, redirect.

* **`tryvariants.py`** rejects app files in subdirectories; its variant strings must use `\n` (it converts
  them; `\r\n` writes double CRs). `tools/vc6/tryv2.py` handles several files and subdirectories: use it
  for app code.
* **Parallel compiles on one VM** (`flagsweep.py --jobs`): `vc6._compile`'s object directory was named by
  pid and millisecond only, so two threads could swap objects (fixed: the thread is in the name); the
  first sync of a new directory must run alone (one `sync.tar` per VM).

* **Packet objects (2026-10-26):** `PacketClass::Unknown_14`'s second argument is pushed early (`push -1`
  well before the call), so a disassembly filter on `push -1` hides it. A class's inline virtuals
  (Get_Channel, Get_Type, Get_Name) come before its scalar deleting destructor in the object; an
  out-of-line one comes after Build. pkdecode's range bound was exclusive and cut off the last object
  (fixed in d95d5e8); check the last class when a scan ends at a round address.

* **Ghidra can attribute an argument pushed for a later call to the call before it** (H10, 2026-10-27): a pushed `1`
  belonged to a sort object's constructor (`ret 8`), not to 0x541800 (no arguments). Check each callee's `ret N`.
  Including ref_ptr.h needs unknown_debug.h first (G_ASSERT).
* In Git Bash `cmd //c mklink //J` makes broken links: use PowerShell
  `New-Item -ItemType Junction`. A `__FILE__` that differs only in case (`app\Client\`) differs in the string, not the bytes.
* **`client.py dis` crashes on a cp1252 console:** set `PYTHONIOENCODING=utf-8`. `attempts.py check` reports pk.h's
  parked 0x8770A0 for every packet file: `--ack 0x8770A0`. Constructors fall outside `appfile.py`'s `__FILE__`
  windows; `client.py refs` misses vtable references that point at thunks (mapper, 2026-10-27).
* **`appfile.py match` drafts (2026-10-27, D93 eval):** A gameedit draft's
  relative include is one level deeper (`..\..\..\wwlib\nstrdup.h`). `explain.py --window 200` shows a whole
  function; `--n` is the candidate index, not a row count.
* **Pseudocode `__thiscall` can be a cdecl free function (2026-10-27, B-haiku):** check the `ret N` and the caller's pushes
  before writing a member. symbols.json names 0xAEDE88 PushMessageLinePacketClass's vtable; it is PacketClass's (pk.h),
  so explain.py's "relocation" class on handlers that destroy a packet temporary is a false lead (T19). For a large
  function with a different instruction count, `.work/t20/align.py` lines up explain.py's `--window` columns with difflib.
  Python's `subprocess` 'bash' on this host is not Git Bash: use `C:\Program Files\Git\usr\bin\bash.exe` (H20).
* **`_$E` initialisers are `short` (2026-10-27, M3):** never exact by themselves; D115's round locates one by its relocated
  fields (atexit, the global) inside the object's range +-0x1000. ref_ptr.h: a script that finds `G_ASSERT(0 != Referent)`
  by text also hits ref_ptr.h's new header note (F7): match the whole statement line.

### 3.7 Harnesses, programs, unattended runs

* **clienttest stand-ins** (2026-10-29, SC3): `#define protected public` in a test file changes a member's mangled
  name (`1` becomes `2`) and gen_stubs quietly writes a second one (DX8Wrapper::D3DDevice): reach the member through
  a derived class. DX8Wrapper::Init clears D3DDevice after the static initialisers, so install a fake device from
  main (d3d_stubs.cpp). A rebuilt member that calls one WWAudio member pulls in WWAudio.obj (LNK2005 on
  client_stubs.cpp's `_theInstance`): stand in for each WWAudio member the new code calls.
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

* **Any program that constructs LandClass reaches RegistryClass(create=true) through 0x468120** (L6, 2026-10-27; D69):
  call `RegistryClass::Set_Read_Only(true)` first. LandClass::Load_Assets rewrites the .pld beside the art whenever
  loading it fails: run vidtest against a copy of the art (`-art`), never the install folder, until that is guarded.
  The land companion (vtable 0xAE8974, displacement maps, Matrox-only) needs its own file: inside land.cpp the land's
  destructor inlines its delete and loses 0x46A480 / 0x4691A0.

* **A worktree file list built from `git status` picks up other builders' half-done files** (T10: P6's pkupdate.cpp
  showed hundreds of false losses): list your own files by name. clienttest's fake model tables need a `__stdcall`
  stand-in for any slot that takes an argument (Set_LOD_Bias, slot 75).
* **A shared-tree verify compiles another builder's half-edited file and empties its block in the shared
  imports.json** (T18's sclient): verify in a worktree and put the worktree-verified block back before committing
  (`.work/t18/fixblock.py`). Including auxdata.h in sclient.cpp breaks kv_pair.h's Type_Name overloads (C2665;
  blocks 0x750B60). clienttest's gen_stubs cannot write a stand-in for a class-by-value return or a
  `const RefCountPtr<T>&` parameter (P-T18-2); its hand-written stubs for SClient members must be deleted in the
  same commit that rebuilds the member (P-T18-1).
* **tryv2 can hit a PermissionError restoring the tree file and leave a variant in it** (T17, H14): grep after every
  run; copy `tryv2-base` objects out right away (shared between builders). A worktree script must `git checkout -- .`
  before `checkout --detach`, or it aborts and the next verify runs on a stale tree. Pure-virtual placeholders for
  not-yet-rebuilt slots of a base no program instantiates are an accepted interim (T17's 0x7251C0): they must
  become real before a program instantiates a derived class.
* explain.py reports a false relocation on PacketClass's vtable 0xAEDE88 (a COMDAT name shared with
  PushMessageLinePacketClass): the masked residue is what counts (P7, P8, T16). Handlers without asserts can be
  moved so the asserted ones sit on the client's lines (T16: cpacket blocks 0x737520-0x737C10 and 0x735D50-0x736DA0).
* **clienttest links build/app/app.lib as it is (2026-10-28, PS1):** run `build.py build --lib app` first; a stale
  lib hid 24 LNK2005 and new unresolved symbols. After a fixpad / assert-line script, check every assert of the
  file, not the first few: a moved line broke postate's type 15 constructor silently.
* **A rebuilt handler whose callees are generated NULL-returning stand-ins can crash a test program**
  (2026-10-28, CR1): list it in `gen_stubs.py`'s `NOT_DRIVEN` until its callees exist (empty since HN1's fake
  asset manager, 7089f1d). co.cpp's loader 0x6F51D0 reads basset.ini, Set_Model's radius 0x6F39F0 and the mount
  points 0x7251C0 read cbasset.ini, both from the working directory (clienttest's copies in its data folder).
  ~ClientObjectClass deletes AuxData through slot +0x04: a fake root's table needs a `__stdcall(unsigned)` there.
* **A RawPacketClass just written with Add can't be read back by a raw constructor** (its cursor is at the end): push
  it through `Memory->Incoming` (2026-10-28, CP2). symbol_substitution.cpp's symbols are `$Name$`, not `%Name%`.
* **clienttest synthetic sections that create models run after section (3)**, or they renumber its models; a packet
  whose member classes are gen_stubs (no vtable) can't go through Build: call Process_Packet on a value-built
  packet (2026-10-28, CP1).
* **clienttest's fake views need every pointer the real body dereferences** (2026-10-28, HN2): set a fake only for
  the section that needs it, so other handlers' FAIL_IF paths stay covered (the radar stays NULL for WARP_INDEX 156).
* **Rebuilding a callee clienttest drives: run clienttest before landing** (2026-10-28, LD3): a real body on its fake
  objects stops the run (0x513700 on the fake main view, no StarField; expected.txt line 2289). `build.py build`
  skips an untracked new app file: commit in the worktree before building for clienttest.
* **`gen_stubs --link` skips data globals** (`MonoClass Mono`): write them by hand in client_stubs.cpp (2026-10-28, UP1).
* **LandClass writes the .pld beside the art it reads (2026-10-28, LD1):** VIDTEST and drawcheck run on LandClass
  now; always pass `-art` / `--art` with a copy of EBCONFIG made with `cp -rp` (without -p the .pld's date changes
  and Save_PLD rewrites it). drawcheck, vidtest_sbs and run_vidtest default to `build/art` when it exists (D120, `tools/harness/artdir.py`)
  and warn when it doesn't; a fresh clone or worktree has none: make it first.
* **Before tuning land lighting, diff the lightmap dumps texel by texel** (2026-10-28, LD2): `run_reference --dump`'s
  `tex_00900_*.bmp` against ours, read with the .pld's horizons; every shadowed texel lit in the original pointed at
  the sun input (0x5286A0 is the CRT's acos: SunAngle is the elevation), not at the land code.
* **capstone shows the CRT's `fpatan` helpers as plain calls** (2026-10-28, LD2): read the callee's body before
  trusting a doc's asin / acos.
* **SimpleSceneClass adds at the head and renders from the head:** an object meant to draw after another is
  added before it (2026-10-28, LD1: the trees' shadows were overdrawn by the land).
* **clienttest stand-ins (2026-10-28, DF1):** Play_3D_Sound beside Play_2D_Sound (else all of soundlib.obj links,
  LNK2005); a by-value KernelEventClass parameter or return needs a hand-written stand-in (gen_stubs now skips them).
* **clienttest (T9, 2026-10-27; `python tools/harness/clienttest.py build|run`):** hand-written stand-ins must cover
  every member of an app.lib object the program reaches, or the linker pulls the whole object and reports LNK2005 on
  the rest (app.lib changes under you when other builders verify: soptions.obj did). A stand-in object whose real
  constructor isn't run needs `Node.Set(object)` before Add_Object. VC6 with /GX catches an access violation in a
  constructor and unwinds before crashing: flush the trace per line. When cpacket.cpp gains handlers, re-run
  clienttest and refresh expected.txt with `--update` after checking the difference (the tasks builder owns that).
* **udpreplay's gate is `conn`, not `run` (2026-10-27):** A-haiku compared `udpreplay.py run alex7` (fails at HEAD,
  one rejected datagram) and reported a pre-existing failure; `conn alex3` / `conn alex7` pass at every eval branch.
  In a worktree, `udpreplay.py build` needs build/app/app.lib (`build.py build --lib app --allow-dirty` first) and a
  baseline worktree needs the libraries and `.work/harness/*.dgram` copied in (`wt.py new` does the copies).
* **Run tools by absolute path from a worktree (2026-10-27, A-haiku):** tools that write the tree refuse a cwd in another tree (420f76e); run them by absolute path so the guard can see it.

### 3.8 Git, docs and scripting hygiene

* **The Edit tool turns a stray `\r\r\n` into `\r\n`** (2026-10-29, LD4: sclient.h), so git shows a whole-file diff:
  restore the byte before committing.
* **Deleting a tracked app file breaks build.py's definitions scan (FileNotFoundError)** (2026-10-29, PK3) until
  the deletion is staged: `git rm --cached` your own file (names.py had the same crash; fixed in 7fd99e8).
* **`\a` in a non-raw Python string becomes BEL** (2026-10-29, SC3): an `#include` written as
  `'..\shared\a...'` lands with a control character; use raw strings or forward slashes (a §3.0 variant).
* **While other builders land on imports.json, automate the bring-over rebase** (2026-10-28, SC2):
  `.work/sc2/rebase.sh` runs rebase, verify, clienttest, udpreplay and replay in one cycle.
* **A Python edit that appends a declaration to a line ending in a `//` comment swallows it** (C2039): prepend
  instead (2026-10-28, CP2).
* **Git Bash turns a `'// 0x...'` argument into a path** (2026-10-28, PX2): prefix the command with
  `MSYS_NO_PATHCONV=1`. `sed -i` on a CRLF file silently misses a pattern ending at the line end (the `\r`).
* **`wt.py remove` deletes the worktree's `.work/` scratch** (2026-10-28, MV2): keep scripts you want in the main
  tree's `.work/`. Export `ENB_VC6_HOSTS` in the same Bash call as every `build.py build`, or it compiles on
  whichever VM vc6_hosts.json names (possibly another builder's lease).
* **`build.py` lists files with `git ls-files`** (2026-10-28, DO1): in a worktree a new file needs only `git add`
  before `build.py build --allow-dirty` (LD3's "commit first" is the stricter form).
* **In a rebase or bring-over conflict, `git show HEAD:file` gives LF endings** (2026-10-28, PK2): read a saved
  working copy instead; `checkout --ours` takes master's generated files, then verify again and re-record your
  attempts rows before `rebase --continue`.
* **`appfile.py match` reports the previous function as PREFIX when a start is missing from function_starts**
  (2026-10-28, PX1: 0x457670): give it its own `// @` marker.
* **`appfile.py match` needs `-D ENABLE_WWPROFILE` for app files** or drafts differ from verify (2026-10-28, MV1).
* **`appfile.py match` wants bare `// @0xADDR` marker lines** (2026-10-28, CS1): stripping a trailing comment can make a
  declaration look like a definition (co.cpp's Unknown_7294D0); use `--allow-missing`.
* **`wt.py bring-over` refuses a D119 ambiguity move** (2026-10-28, LD3): land it with commit_mine from the main
  tree (`.work/ld3/land.sh`); master's `in_tree_total` is then one high until the next verify rewrites it.
* **names.py files a NOT EXACT marker under the next function when a static helper's signature spans two lines**
  (2026-10-28, LD3: 0x47C6A0's marker shows under Build_Decorations). `attempts.py record` takes an integer
  `--forms` and only its fixed `--blocker` list.
* **`wt.py bring-over` refusing on an imports.json conflict** (2026-10-28, LD2): `git rebase master` in the
  worktree, take master's imports.json, re-verify, union attempts.csv, run names.py, `git rebase --continue`.
* **After a bring-over, `git rebase master` in the worktree can conflict on names-review.md:** `git rebase --skip`
  drops the commit that already landed; "master moved while landing": rerun (2026-10-28, EF2).
* **When master moves every few minutes, `wt.py bring-over` keeps conflicting on imports.json** (2026-10-28, DM1):
  verify in the worktree, then commit_mine from the main tree with the whole file and its imports entry copied
  over (`commit_rows.set_entries`; `.work/dm1/land.sh`).
* **`wt.py bring-over` refuses an imports.json cherry-pick conflict when master has moved** (2026-10-28, HX1):
  recreate the commit in a fresh worktree from master (`cherry-pick -n`, take HEAD's imports.json, re-verify).
* **tryv2 refuses an anchor that occurs twice** (co.cpp's two IsInPVP finds); a long experiment script that holds
  the importer lock for 10+ minutes blocks others' report and replay: release it between variants (2026-10-28, IB1).
* **tryvariants.py only sees a library folder's top level** (`app/packets/x.cpp` can't be tried; `.work/pk1/tv.py`
  and `hv.py` can); **build.py compiles tracked files only** (a new app file is harness-tested after its commit, or
  with `--allow-dirty` and a private `--out`) (2026-10-28, PK1).
* **A wt.py worktree checks out CRLF (autocrlf) where the main tree's programs/ files are LF** (2026-10-28,
  LD1): scripts that edit them handle both.
* **`wt.py bring-over` refuses while the main tree has another builder's uncommitted generated file**
  (docs/imports.md): never commit imports.md from a worktree; the next `report` run regenerates it (2026-10-28, UP1).
* **The Bash tool starts a fresh shell per call** (2026-10-28, EX1): export `ENB_AGENT` and `ENB_VC6_HOSTS` in
  every call, or appfile.py runs on whichever VM vc6_hosts.json lists. In Python edit scripts, write include
  paths as raw strings (`"..\app"` once became a BEL).
* **Moving a declaration block "up to the next blank line" cuts classes that contain one** (2026-10-28, CR1,
  SBaseViewClass): end the block at an explicit marker.
* **commit_mine keys on a RECONSTRUCTED continuation line must be in the changed line itself** (2026-10-28,
  PS1): a key on the entry's first line misses an edit to its continuation; the hunk lands in the next commit.
* **The Write tool writes LF:** after writing a tracked doc or source, restore CRLF if the file had
  it (python: read bytes, replace b'\n' with b'\r\n' where not already). Check line ends before
  `\r\n` anchors; Python's Windows text mode writes CRLF: match `\r?\n` later.
* **Generated tree files: CRLF throughout** (`newline=''`, `\r\n`); a mixed one (D22's shader
  headers) shows modified with an empty diff. Git's CRLF warning for importer.py is harmless.
* **The Write tool drops tabs on whitespace-only spec lines:** anchor with `"\t"` or tree slices.
* **A slice between two `str.index()` markers** is empty when the second comes first, and
  `replace('', x)` destroys the file: assert it. A short anchor can be part of a longer line.
* **Git Bash:** `"app\shared\auxdata\$f.cpp"` swallows `$f`: use `/`.

* **The git index is shared between parallel builders:** a bare `git commit` sweeps their staged
  hunks into your commit, and `git commit -- <path>` takes a shared file's whole working copy.
  Own files by path; anything touching importer.py, headers, stubs or docs/data through
  `tools/vc6/commit_mine.py` (builders.md). `git stash` is shared across worktrees too.

* **Git Bash `sed -i` rewrites a CRLF file as LF even when its pattern matches nothing** (it did to
  tcpconn.cpp). Edit tree files only with Python opened with `newline=''`.

* **Never write a hand-built `docs/data/imports.json` into the shared working copy** (2026-10-26, two
  sweeps): the other builder's `commit_mine` takes the whole file as it is, and any verify rewrites it.
  Build a private record elsewhere and restore the working copy at once. Before any commit that
  includes imports.json, `pairs_cmp.py` against HEAD's copy shows foreign pairs (gains you didn't make).

* **Untracked files are not protected by the private index:** another builder's `commit_mine` or path
  commit takes your importer.py / imports.json hunks while the files they name are still untracked
  (5e7f96d took pkflag's). Commit a new file with its records in the same step.

* **Shared builds (2026-10-26/27):** `udpreplay.py build` writes the shared `build/` and `conn` runs whatever exe
  is there, so another builder's half-done file can break or taint your run: build before `conn`, or build in a
  clean worktree of HEAD plus your files. Two `static` tables with the same name break udpreplay's single-TU
  build.
* **git worktrees check files out as CRLF while the shared tree's land.cpp / land.h are LF** (L6): convert the
  anchors of an edit script to the file's own newline (`newline=''`, detect `\r\n`).
* **Python text-mode `open(p, 'w')` writes CRLF into LF tree files; `grep -c $'\r$'` miscounts in Git Bash** (H13):
  `newline=''` both ways, count bytes with Python. `build.py link --allow-dirty` compiles other builders' uncommitted
  files into the shared build/ and overwrites `.work/milestones/l1/unresolved.txt`.
* **`open(p, 'w').write(open(p).read()...)` truncates p to empty** (the write handle opens first): read, then
  write. A draft regenerated from a template overwrites an applied fix: check the tree file before verifying.
  `names.py --tree DIR` regenerates both from another worktree (cdf3e23). In a shared tree names.py now also sees other builders' untracked drafts: commit through commit_mine (row selection) and restore the working copies after (F7).

* **Run `python tools/vc6/build.py link` for the programs after adding a function to a library file or touching
  stubs/ or pk.h** (L5, 2026-10-27: a missing Resize, then ee8fa6f's stub change pulled pk.obj and 280 externals
  into VIDTEST). A commit subject must not claim a program links unless that command ran at that commit.
* **`__LINE__` files (2026-10-27, S1 / R1 / R2):** new code goes after the file's last `__LINE__` user, or every later
  assert moves; a tryv2 variant that adds or removes one line shows false losses: pair it with an opposite edit; a
  trailing blank line before an appfile `@N` anchor eats a line. A tryv2 edit anchored on a loop line can match twice
  (H22): anchor on the line before it.
### 3.9 Library-specific

* **wnet (E&B's TCPCon):** a reset connection is `CONNRESET` (-16): reads and writes end quietly on it
  and `isConnected` treats it as closed; `pumpReads` needs a dead `retval = -1;` (the client's stray
  `or edi, edi`).

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
* **Packets (2026-10-27, D93 task A):** pkemit.py's NAMED keys must be addresses inside their own object (pkgespwn
  was keyed 0x8622A0, inside 0x861DB0-0x862320; now 0x862350). pkclasses.py sees a class as rebuilt only when
  `// opcode N (` sits directly above `class X`; a comment between them duplicates it in pk_classes.h.
  Game-editor objects share one `__FILE__`: read the assert string (0xBB2410: gameedit\pkgesave.cpp) before naming
  a pkunknown_* file, and rename the whole object (A-haiku split 30027 / 30028 across two files instead).
* **Packets (2026-10-27, P10):** when a class in pkclasses.py's OVERRIDES is rebuilt with inline virtuals, empty its
  harness stub list (pk_ctor_stubs.cpp would redefine them); an object without an assert ends at the guarded-static
  atexit call inside its last Print. cpacket.cpp calling SoundLibraryClass needs a clienttest stand-in (wwaudio's globals
  break the link, T19).
