# Working on the libraries: workflow and known traps

How the library work is done day to day, and what has gone wrong before. Written for whoever
picks the work up next. The tools themselves are listed in tools/re/README.md; the VMs are
described in docs/toolchain.md §1.

## 1. Working state outside git

| What | Where | How to recreate |
|---|---|---|
| Byte-match evidence for every EA copy (R/G/Z); reference only since the freeze (D211), no tool takes it as input | `.work/bm2` (git-ignored) | `tools/vc6/bytematch_all.sh` (all three copies, each against its own libraries; `RAD` points at `SDK\radtools\include_enb`, written by `tools/vc6/sdk_patch.py`) |
| The tree's own compiled objects, read by `gaps.py`, `layoutdiff.py`, `vtables.py`, `symbols.py` | `%TEMP%\enb_import\verify-<lib>` | `python tools/vc6/verify.py` |
| The build VMs' addresses | `.work/vc6_hosts.json` | `python tools/vc6/vc6.py discover` (after the VMs restart: DHCP) |
| Scratch scripts, ledger snapshots (`ledger.py export -o .work/...`), logs | `.work/` | disposable |
| The importer lock: one run at a time of `verify.py`, `report.py`, `tryvariants.py`, `trydiff.py` (and anything calling `verify.compile_and_match`/`ledger.save_state`; `vc6lock.py`), held until the process exits; JSON with agent (`ENB_AGENT`, else user@host), pid, command, start. A second run prints `importer in use by ...` and waits `--lock-wait` s (env `ENB_LOCK_WAIT`, default 600), then exits 1; a dead holder (same host) or one older than 4 h is taken over | `.work/locks/importer.lock` | nothing to do: removed at exit; if a run dies it is taken over by the next one (delete it by hand only if no importer is running) |

`.work/` holds nothing that the repository depends on. Every kept change lives in the tree. In an
EA-derived file it is also described in the file's end-of-file `// EnB:` block (D211;
conventions.md, "E&B changes to an EA-derived file"). What matches is the ledger `docs/data/imports/`
(`tools/vc6/ledger.py`: one JSON shard per source file, which also holds a reconstructed file's
description, `RECONSTRUCTED`).

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
   or virtual. Unknown E&B additions get neutral names in the library's own style (`UnknownXX`;
   `m_UnknownXX` in WWAudio, whose members are `m_`-prefixed: conventions.md §1), with the evidence
   in a comment.
3. **Edit the file directly** (D211), with a file tool, not a shell heredoc (escapes such as
   `\t`, `\r\n` and `\x` get mangled by the shell). Then add an entry to the file's end-of-file
   block: `// EnB:`, the function or declaration, what changed, the client address
   (conventions.md, "E&B changes to an EA-derived file"). A file changed for the first time gets
   the block and the banner note: `**	Modified for the Earth & Beyond client reconstruction: each
   change is recorded at the end of this file.` replaces the bare `**` line under EA's copyright,
   one line for one. Never put a comment line above code in an EA-derived file (§3.0). To find
   one function's form quickly, try variants first: `tools/vc6/tryvariants.py LIB FILE V.py`.
4. **Check.**
   * Save the ledger first: `python tools/vc6/ledger.py export -o .work/<item>/before.json`.
   * Run `verify.py --lib ...` for every library that includes the changed header.
     WWLib headers reach every library. Add `--pool K` to build on K VMs leased from
     `tools/vc6/pool.py` instead of the default host (the hosts in `.work/vc6_hosts.json`;
     released at the end; toolchain.md §1.3).
   * Compare the sets of `(file, address)` pairs before and after. Compare sets, not totals:
     a total can stay level while functions are traded.
5. **Keep or drop.**
   * Keep the change only if it gains and loses nothing. Any accepted loss must be written down,
     in the commit message and in docs/library-matching.md.
   * Then run `report.py` and commit.

Counting: the "in tree" totals that verify prints count each client function once. The
`(file, address)` pairs in the ledger also include app-region duplicates (inline functions
compiled into E&B's objects), so they run higher.

### 2.1 Adding an app file (E&B's own code, D23)

E&B's files are the non-EA library `app` (`APP_LIBS` in libdefs.py): no EA copy,
every file in `RECONSTRUCTED` (a ledger shard with a `description`), under `app/` as in `e:\build\beta\G\app\...` (A1, lower-case
directories).

1. **Draft.** `python tools/re/appfile.py show app\shared\auxdata\x.cpp` gives the window, its
   asserts and their lines; `draft FILE --out .work/<item>/` writes a skeleton with `// @0xADDR`
   markers and the saved pseudocode. Write the C++ (names: conventions.md §1, D10, D24), with
   `#line N` before each assert in the draft, and run `appfile.py match DRAFT.cpp [-I DIR]` until
   every marker is EXACT.
2. **Tree file.** Write it at its client path (`app/shared/auxdata/x.cpp`, its header beside it)
   with the Write tool. The asserts sit on the client's `__LINE__` through the layout (blank
   lines, comments; no `#line`, D25), so little fits above the first assert: put the
   reconstruction notes at the end of the file (as bitset.cpp does) and check with
   `grep -n 'G_ASSERT\|FAIL_IF'`. The assert macros: `#include "..\g_assert.h"` (relative).
3. **Record.** `python tools/vc6/ledger.py describe app shared/auxdata/x.cpp "EVIDENCE"`, and the
   same for its header (lower-case paths with `/`): the shards `docs/data/imports/app/x.json` and
   `docs/data/imports/app/headers/shared/auxdata/x.h.json` hold the description (`RECONSTRUCTED`);
   verify fills in the rest (`ledger.load_state` makes the state entry from the descriptions). Two app
   `.cpp` files may not share a base name (it is the file key; `describe` and `load_state` refuse it).
4. **Verify** as in §2: `verify.py --lib app` (`/MT /W3 /O2 /Ob2 /GX`, `NDEBUG`
   defined: D61, G_ASSERT / FAIL_IF don't depend on it; matched within 0x401000-0x8C1B50 only),
   `tools/vc6/pairs_cmp.py`, `report.py`, commit. Functions
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

* **A pair that holds only with other files' exact text is a near miss (D203, 2026-11-07):** never work round
  other code (keeping a header byte-equal, dropping a needed edit) to keep a pair won earlier. nav_list_builder's
  0x7B7EF0 and its vector destructors 0x7B8ED0 / 0x7B8F10 are the first: their loss is accepted under D66 (name
  D203 in the commit; the coordinator's heartbeat `--accept`s them). Aim for both changes exact at once.
* **An uncommitted app header edit reaches other builders' verifies half done** (2026-10-29, PX4): UI1's verify
  compiled PX4's new px_collision.cpp against HEAD's px_collision.h and wrote C2511 plus an empty px_collision entry
  into the working imports.json. Draft in a scratch copy (`appfile.py match` on `.work/<id>/m/`), touch the tree
  only to verify and commit at once, and re-verify before `commit_mine --file` (it takes the working copy's entry).
* **Before changing a member's type, grep the other builders' working copies, not only HEAD** (2026-10-29, HL3):
  "nothing outside fontdatabase.* reads BoxOffset" was true at HEAD but not in the shared tree (LS1's uncommitted
  fontmanager.cpp read `def->BoxOffset[0]`).
* **No comment line above code in an EA-derived file (D211).** The asserts (`WWASSERT`,
  `WWDEBUG_SAY`, `FAIL_IF`) compile `__LINE__`: a line added above them moves it, and the pair is
  lost (WE6 lost 0x8444C0 that way). Describe the change in the file's end-of-file `// EnB:` block;
  the banner note replaces the bare `**` line under EA's copyright, one line for one.
* **A gain with no text change is not a gain (2026-11-07).** A full `importer.py verify` once
  credited 10 extra pairs (wwaudio/sound3dhandle, `ListenerHandleClass` methods
  0xA029B0-0xA02C00) with no source change; a rerun gave the baseline again. Rerun the verify
  before keeping such a gain. Likewise a single-file trial (`zhall.py`, `tryv2.py`, the retired
  `importer.py refine`) can report +1 for an address another file already holds: refine's
  single-file gains were nothing in a full verify. Confirm with a full library verify.
* **Pair sets, not totals, against the session's baseline** (`tools/vc6/pairs_cmp.py`). Out-of-line
  copies of inline functions (`Set_Texture` 0x47F3F0, `Set_DX8_Light` 0x984090) come and go with
  their callers' inlining: a loss against the last step needn't be one against the baseline.
* **The importer lock** (§1). A script holding it that starts `importer.py`,
  `tryvariants.py` or `trydiff.py` as a child deadlocks: start children from an unlocked parent
  or call in-process. `tools/vc6/zhall.py` holds it ~15 min (ww3d2); read-only
  `objdiff.py` still works. A killed run or a transient `PermissionError` on a tree write leaves a
  half-written file: `git status`, restore, delete the lock.
* **Shared tree: never wait for a clean tree** (it deadlocked two builders for an hour). Keep the
  other's work out of your verify: untracked files aren't compiled unless importer.py lists them,
  tracked edits and uncommitted importer.py entries are (2 encode.cpp gains showed in another's
  run): compare per file, restore imports.json; `build.py link` then reports stub compile errors.
  Commit only your own hunks, soon; in doubt the coordinator sets a turn order.
* **Scripts and specs through the Write tool, never a heredoc:** Bash mangles `\n`, `\t`, `\r\n`,
  `\x`; in non-raw Python strings `"\a"`/`"\b"` are control characters.
* **Line endings: LF everywhere** (D147, `.gitattributes`). Every checkout and worktree is LF; the Write
  tool, the Edit tool, Git Bash `sed` and Python need no line-ending handling (no `newline=''`, no `\r\n`
  anchors). If a Windows tool wrote CRLF (Python text mode without `newline='\n'`): `python tools/eol.py fix`.
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
  Reading its files in place and copying them out are allowed (D143), e.g. build/art/MSS32.DLL for clienttest (D142).
* **`git add` by name,** never `-A` or `.`: the tree holds other builders' files.
* **Heredoc Python mangles regex escapes silently** (2026-11-01, NW2): `\\b` / `\\1` in a heredoc-written regex deleted the names it should have kept and the variant failed to compile. Use the Write tool, as §1 says.
* **Daylight saving** (2026-11-01): at 02:00 PDT the clock fell back to 01:00 PST mid-run; `date` jumps back an hour, so stop times read against it run an hour longer in real time. Give stop times with the zone.
* **Shared-tree hazards (2026-11-01 day run):** another builder overwrote an uncommitted ui_login.cpp edit within minutes (TG): commit each verified edit at once. A verify run while someone's widely included header is half-edited writes losses for every file into the working imports.json (1024 errors, 5,358 functions; GC, CV, FNT1): read verify's last-line error count and `compile_errors` before pairs_cmp or `commit_mine --file`, and restore from a known-good snapshot plus HEAD's entries (`.work/gc/merge_imports.py`). Bash heredocs keep mangling escapes: `'\\0'` became NUL bytes in a .cpp, `'\\r\\n'` became `\r`, backslash paths halved, `IFS=:` split C++ `::` names, and Python's default text mode writes CRLF (`tools/eol.py fix`): use the Write / Edit tools and `chr(92)` (TG, LINK, CT2, LG, MC).
* **Host memory (2026-11-02 run):** `pool.py lease` (and `appfile.py --pool`) exits at once with no VM when the host is short of memory, it does not wait: retry in a loop (sleep 45-60 s) until the output holds an IP (25 minutes on 2026-11-01 with four headless builders, a variants.py job and five VMs on 32 GB). Claude Code's memory reaper kills background shells (a coordinator's `run_in_background` launchers; builders' lease-wait loops); a process started detached (PowerShell Start-Process) survives. Headless builders: at most 2-3 at once on this host.

* **Heredoc and `sed`/`printf` mangle backslashes again (2026-11-02 night run, VC, GAD3, MSG2, DBG, DISP, LF):** a heredoc turned `\v` / `\t` / `\n` / `\r\n` in include paths and client path strings into control characters (rs.cpp failed to compile for ~3 min for everyone; `sed a\` in Git Bash too). Write files with the Write tool; client paths in importer.py `RECONSTRUCTED` need doubled backslashes (`\\b`, DG).
* **A half-done edit by a running builder breaks the others' verifies (2026-11-02 night run, VO, CV1, SCR, GAD3):** an uncommitted aux_roots.h / strconv_lib.h / auxroots.py edit (second inline `Type_Name`) broke 10 app files; verify in the shared tree also picks up another builder's uncommitted files (losses in client_lego_impl were VC's) and writes a polluted imports.json (read compile_errors before trusting a verify; the final quiet-tree verify must regenerate it). Even a declaration added to strconv_lib.h moves aux_roots' pad window (LD1); never wrap an `auxpad.py` sweep in `timeout` (it left a pad-54 aux_roots.cpp in the tree). A concurrent `pseudocode.py` shares `.work/ghidra/targets.tsv` and clobbers it (MSG): rerun if "ok N" is short.
* **Worktree setup (2026-11-02 night run, UIB, DISP, SCR, MSG, CV2):** `wt.py new` then build/link needs `--pool 1` (else FileNotFoundError cl.log) and `ENB_VC6_HOST(S)` set to a leased VM; copy build/art and build/d3d8shot (d3d8shot.dll, MSS32.DLL) in; `git add -N` every new file before a build (else gen stubs silently stand in); `wt.py remove` keeps the wt/NAME branch unless `--delete-branch`. A leased VM can turn Off mid-session: release and re-lease. A worktree-at-HEAD build takes ~3 min.
* **CI runs the tool tests on Linux (2026-11-08, PR #174):** a test that reaches a Windows-only call fails there though it passes here: `wt.py new` furnishes junctions with `cmd /c mklink`, so tests calling it stub `wt.furnish` (test_wt_git.py's Stacked). Stub any `cmd`, `mklink` or VM call in a test.
* **A lease named `importer-<pid>` with no holder pid may be live (2026-11-07b):** the hosted verify's run (verify_watch.py once -> verify_pr.py -> verify) leases that way; the coordinator released one mid-run, taking it for its own harness check's leftover. Check the pid (`tasklist`, its command line) before releasing a lease you did not take.
* **`poc_check.sh NAME COMMIT LOG` needs an absolute LOG (2026-11-07b):** it cds into .work/wt/NAME first, so a relative log path writes nowhere; leave LOG out (default .work/poc-check-NAME.log).
* **POC runs show 29 `combo_ffactory.cpp(101)` FAIL_IF lines (2026-11-02 night run):** retail behaviour (duplicate data rows; MIX's line), not a regression.
* **Heartbeat rate from the 1 % weekly-usage steps misleads (2026-11-02 night run, coordinator):** the weekly figure moves in whole percent steps, so a heartbeat's rate over 5-15 minutes reads 0 or 1 % per step and swings the launch allowance; measure over an hour or more before scaling builders up or down.
* **A worktree build with a stale `ENB_VC6_HOST(S)` deletes app.lib (2026-11-02 night run, KRN2, 04:19):** a build on a just-released lease fails every file, deletes app.lib, and the POC then silently runs the old exe. Use `--pool 1`, and check the build output says 'compiled' before trusting a POC run. `wt.py remove` needs `--force` when files were copied in.
* **An Edit ending in blank lines can add a line (2026-11-02 night run, KRN2):** check `git diff --numstat` after such an Edit when the file's line numbers matter (`__LINE__` asserts).
* **Headers and EA sources, last round (2026-11-02 night run, HDR 60b483bf, EAM3 9f59bf88):** `#include <string>` added to a widely included app header changes code in other files that include it (message.h cost message_server.cpp's 0x4BCB50 outright: give such a declaration its own small header); a declaration moved into a header shows in names-review as an "untagged neutral name" at its first use. Before reverse engineering an asset-manager or render-state class, look for EA's published source: `C:\EnBDevelopment\EA\CnC_Renegade\Code\ww3d2\hueshift\enbassetmgr.cpp` is E&B's ENBAssetManager (Make_*_Unique, Recolor_*, Create_Render_Obj with HSV). Definition order inside a file can drop an EH frame (0x48CF90 keeps the client's frameless form only when defined before 0x48CB40); REF_PTR_SET's null test disappears when the cache-or-new choice is an inline helper with two returns.
* **Coordinator landings and launches (2026-11-08 night run, coordinator):** run `wt.py bring-over` in the background, never in a foreground shell call: a 600 s tool timeout moved one to the background and stopping that task left its python (the importer lock, lease `importer-<pid>`) running on; check the pid's command line (`Get-CimInstance Win32_Process`) before anything else. A Git Bash wait loop does not see Windows python processes (`pgrep`, `ps`): wait on the landing log's `landed:` line or on `tasklist //FI "PID eq N"`; a second bring-over started while one lands refuses cleanly ("run moved while landing ... nothing landed, rerun"). `tools/agy/launch.sh` wants `RUN_DIR` relative to the repo (an absolute `/c/...` is joined onto it). In PowerShell, unquoted `0x...` arguments arrive as integers (`.work/coord/claim_many.ps1` normalises them). A file claim (`run_branch.py claim FILE`) counts only functions its index thinks not exact: gd_text_editor.cpp came back "all exact" with four NOT EXACT functions in not-exact.md, and ui_talkt / ui_mfd's claims missed 0x64B6E0 / 0x5BECF0: claim near misses by address range.

### 3.1 VC6 code generation

* **Adjacent scalar deletes (2026-11-02 evening run, EAM5):** VC6 /O2 merges the stack cleanups of consecutive `delete p;`
  calls (`add esp, 8`); the client's per-call `add esp, 4` comes from `if (p != NULL) delete p;`.

* **Stack slots and FPU copies (2026-10-31, TK18, landdmap 0x467A50 / 0x468920):** two address-taken Lock pointers
  the client keeps in one slot need each lock in its own `{}` block. A Vector3 local copied from a vertex stays on the
  FPU across a loop; copying through a POD (D3DVECTOR / D3DXVECTOR3) gives the client's integer moves. A 16-bit `imul`
  in an index is `(unsigned short)a * (unsigned short)b + x`. tryv2.py does not keep its objects: `.work/tk18/keep.py`
  (same spec, each variant's .obj in `.work/tk18/obj-N`) does, for objdiff (worth promoting into tools/).
* **Float stores (2026-10-31, TK15, ui_avatar_cust 0x54E3C0):** where the client's zero stores come from an integer
  register but its other values go fld / fstp, route the non-zero values through an inline `(float &dst, float value)`
  setter (a plain assignment copies them through eax; 0.44 -> exact). A float constant loaded from rdata and negated
  with fchs is a file `static const float` (VC6 does not fold it). A chain of `G1 == x || G2 == x` tests laid out with
  the else blocks first is an inline bool helper.
* **VC6 /Ob2 inlines a non-inline member defined later in the same file** (2026-10-31, TK10, ui_line_display): a client
  function with no code callers whose body matches part of a not-exact neighbour was probably inlined there: call it
  from the neighbour (Find_Ephemeral 0x5A8B60 into Show_Ephemeral 0x5A7A80). The inlined callee's form decides whether
  its temporaries share stack slots (a ternary gave a 0x10 bigger frame than if / return, though both compile alike out
  of line). Budget levers can work only together (tutorial_manager Reload 0x6AD7B0: the iterator before the loop and
  the implicit key string; the ctor 0x6AC600: an early return and one dead size() read): try pairs before giving up.
* **One extra no-code inline layer moves VC6's inline budget** (2026-10-31, TK2, attrib_dir Detach_Mod 0x4D4BD0): one
  inline handle method left exactly the client's first operator= call out of line, two left two. Try it before
  declaring an inline-budget residue final. **`int i < sizeof(a)/sizeof(a[0])` compiles to `jb`:** cast the count to
  int for the client's `jl`.
* **Sibling blocks share stack slots** (2026-10-29, PX4): a client frame equal to the sum of its cases' locals
  means one case's locals sit at function scope (0x454730: frame 0x368 -> the client's 0x588). `if (g()) return(true);
  return(false);` gives the client's test / setne.
* **Small budget levers from HL2's round** (2026-10-29): an extra inline accessor in a test (`def->Get_Type()`)
  can push an earlier string assign out of line and pull a later destructor inline: try it when a sibling without
  the test is already exact (Play_2D_Sound 0x43ED30 against Play_3D_Sound). When the client calls a file's own
  helpers out of line in every caller, try `#pragma auto_inline(off)` around their definitions (fixed Load_String's
  frame and dead parameter slots). A helper put into a blank-line area must replace exactly as many lines as it
  takes: one short moved Start_Step's `__LINE__` and showed as a lost pair.
* **A large struct copy inlines only through an explicit inline operator=** (2026-10-29, FU1): the implicit one is
  called out of line, and can move a load in an unrelated function that includes the header (H21's Load_Init
  knock-on). A member whose inline twin is 'own begin / ++ called out of line' is a .cpp function the caller
  auto-inlines one level deeper: define it out of line to gain its uncalled copy and keep the caller exact.
* **When one function family inlines both less and more than the client, look for shared static helpers**
  (2026-10-29, F9's Fable round: the verb object's five, +6): stop spending budget forms, the client's candidate set
  differs. Traces: distinct stack slots per copy (two count slots in one ctor), a `set::begin() const` copy called
  while `_Tree::begin()` is inlined elsewhere, int3 padding where out-of-line helper copies would sit. Inside an
  inlined static helper a set-walking `for` leaves `_Inc` out of line; `it = begin(); while (it != end()) { ...;
  ++it; }` inlines it. In a non-inlined body VC6 gives identical bytes for it++ / ++it, iterator / const_iterator,
  += / append(ptr) and for / while: those are not levers there.
  HL1 (2026-10-29): the while form also works on a template helper's vector walk in a header: attrib_dir.h's
  AttribIteratorListClass::Advance / Invalidate as `i = begin(); while (i != end()) { ...; ++i; }` made two
  functions exact (one inlining more, one more in a destructor). Try the while form on every inlined helper loop
  before looking for static helpers; keep line numbers (put statements on shared lines), or every later function
  with asserts is lost. The int3 padding trace is useless under the incremental link (every function is padded).
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
  IB2 (2026-10-29): a helper made only of the find and the mark test, returning t in tail position, compiles
  byte-identical to the flat code; the spender needs work after the find's expansion inside the helper (the
  loop / memset). Two helpers (Spot, Line) with Blotch's find explicit gave the client's calls exactly.
  IB3 (2026-10-29): one more inline with early returns wrapped around the template find spends the budget on the
  find itself (Unknown_Find_Field goes out of line before the Cast moves): a depth lever applies to the callee one
  level below the wrapper, not to the deepest one.
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
| A header macro change (D165's `&&` FAIL_IF) makes callers inline more, or less, than the client | The macro's form changes what it costs the caller's inline budget: put the old cost back with one dead inline read at the right spot (a dead `text.size();`, a dead `WWAudioClass::Get_Instance();`), or one more implicit conversion among a run of explicit ones; a `return` that became `break` can move a merged return block | FI1 2943f972: co 0x6FFFF0, sclient 0x750260, ui_cpit 0x57F240, client_lego_impl 0x72D5F0 |
| The client calls a callee out of line (the insert's last `size()`) that ours inlines | A dead inlined call spends the budget without emitting code: an unused `int index = Children.size();` before the call; a dead call *after* the call site changes earlier inlining too | `DeltaCompoundClass::Add` 0x4A5670, 33a4c97 (D40) |
| Inlining differs although the caller's own code is the same | The budget counts loops inside inlined callees, even when the body is an empty release `WWASSERT`; empty `WWASSERT`s and dead loops in the caller itself count for nothing | builder C, 2026-10-25 (f9e4a92 `Process_Foreground_Load` 0x949C60) |
| A `Transpose`'s `Vector4()` through `??_H` at only one site, every stack slot moved | Tiny accessors tip the budget: `view[r][c]` (two inline `operator[]`) against `view[r].X` | `PointGroupClass::Render` 0x954960, 2cfad68 |
| Callers that inline a loop function call others out of line | `it++` in the loop spends their budget: `++it` | Release 0x826870 and the listener destructors, f35ce66 |
| An out-of-line copy right after its first caller | It is an inline function not inlined there: make it `inline`. `/Ob2` inlines functions defined later in the file too; an EA `WWINLINE` the client calls out of line moves to the .cpp | 0x6FA150, 0x6FB1E0, 0x707430, 0x6F85F0 (fe6b08f, b568424) |
| Stack slots swap in a function you didn't touch | Inlining depends on the whole file: an unrelated edit moves it; re-verify after generator changes. An emptied function (Generals' `#if 0` in `ThumbnailManagerClass::Save`) gets inlined: callers matching the other copy's call point at it | 0x6FB1E0 |
| Many callers missing, one inline to blame | Look for code compiled out in ours: restoring `Is_Disabled`'s `#ifdef G_CODE_BASE` matched 13 callers. One out-of-line `operator==` can hinge on the condition (`!=` keeps it out of line) | 377d4cc |
| A helper the client keeps out of line with no caller | A global, not a static (statics vanish when inlined) | connect.cpp's host helpers 0x821890 / 0x821900, step 8 |
| **register-only, order-only** (two attempts, then the stop rule) | | |
| A constant loaded before a variable, or the reverse | Operand order: `elapsed * 0.05F * K` loads the constant first, `step = elapsed * 0.05F; step *= K;` loads `elapsed`; `a == b` vs `b == a`, `Pos > size` vs `size < Pos` differ (D18/D28: operand-order noise elsewhere) | |
| Registers differ around a call or an argument | A temporary, a local copy of an argument or a member read into a local before a virtual call changes registers; one shared variable reusing a dead argument slot can be what matches. `tools/vc6/trydiff.py` keeps a variant's object for `objdiff.py` / `explain.py` | 0x4A3D50 (6b687cb), 0x4A91E0 (fb52504), Accept 0x82A6C0 (4ba58f7) |
| A different frame slot for a helper object | A temporary (`X().Unregister(p)`) gets another slot than a named local | step 8 (memconn.cpp) |
| Registers of an inlined `DynamicVectorClass::Add` on a global | The global's linkage (file-static or not) orders them | |
| A load-order / allocation residue no local form moves | VC6 collapses local rewrites into a few canonical outputs; the lever is function-wide (other locals, a shared helper) or nowhere: stop after two attempts | Make_Screen_Shot 0x9182C0 / Update_Movie_Capture 0x918B20, parked (85d3506; `attempts.py show 0x9182C0`) |
| Any residue, tried with compiler switches | No switch has fixed one: Make_Screen_Shot's U form under all 30 `/O1 /O2` x `/Ob0-2` x `/G3-/G6 /GB` (`flagsweep.py`, 2026-10-26: the library's own `/O2 /Ob2 /GB` best, 5 rows, nothing exact); Open-BFME's 113 near misses x every `/O /Ob /G /Oy` (MSVC 7.1). An allocation residue comes from the source | f4a950d |
| **shape: returns, tails, loops** | | |
| test/setne against neg/sbb; shared or separate `return false` tails | `if (g()) return true; return false;` gives test/setne, `return g();` neg/sbb; a shared `return false` tail is `if (p) {...; return true;} return false;`; separate failure returns share the success tail; an eax-wide 0/1 (Is_Dead) is `return(!a() && !b());` | |
| A plain `sete al` | `if (f() == 0) return(true); return(false);`; `return(f() == 0)` doesn't give it | step 8 |
| The client repeats a success tail (`Handle = 0; return true` three times) | A return inside each success block (one common tail, an `ok` flag or `&&` chains merge it) | `Try_Connect` 0x8290A0, 6d2803d |
| Ours merges two failure tails the client keeps (`explain.py`: "the same callees, N vs M calls") | Open: `Listen` 0x82A470, the memconn option slot 0x826D00 (`attempts.py list --blocker shape`) | |
| The client leaves a loop by a jump to the single return | A `goto` to the single return | `Get_Log_Directory`, 25ccd76 |
| `jb top` against `jae exit; jmp top` | A for loop gives `jb top`; `jae exit; jmp top` is a do-while behind a count test | the array templates' loops, 36015e9 |
| `push_back` keeps 0 in a register and calls insert out of line | `push_back(T(x))`; three field stores into a named local don't | step 8 |
| **frame** | | |
| Each inlined copy has its own buffer slot | A function-scope buffer (`char item_name[256]` at the constructor's top) gives each inlined copy its own frame slot | 0x752E60 (frame 0x218, not 0x118) |
| The frame size differs (`explain.py`: frame) | The frame counts every local: a difference names a local too many or too few (a second RECT grew Make_Screen_Shot's by 0x10; the distortion draw's client frame is 0x18 bigger: locals not found) | 0x9182C0, 0x9961E0 (`attempts.py show`) |
| **constructors, destructors, vtables** | | |
| A destructor that is a lone `jmp` to the base's, or one that stores the vtable first | An implicit destructor stores no vtable; an explicit `{}` one stores it first | 0x4AA210 |
| Members stored before or after the vtable | An init list stores members before the vtable, body assignments after it (`GrowBy`, `HasWildcard` needed the body); the base's vtable store comes first and is dropped, the class's own is stored after its members | 0x4A8370 (5ec7760); the packet classes (builder B, 2026-10-25) |
| Virtual slots in the wrong order | Overloaded virtuals are laid out in reverse declaration order | dbgconn.cpp's two `Describe` (25ccd76) |
| A base function kept in a slot plus an empty own slot (`ret 4`) | A changed base signature hides the virtual: E&B's `Sound3DClass::Set_Velocity` takes two arguments (`ret 8`), so `SoundPseudo3DClass`'s and `Listener3DClass`'s one-argument overrides became new virtuals | 2f9bb70 |
| **field offset** | | |
| `[reg+N]` differs in otherwise equal code | The displacement can belong to another class: `DX8PolygonRendererClass`'s `lea ecx,[eax+0xAC]` (client 0x84) was the mesh's, `MeshModelClass`'s layout. Trace the register before patching; `tools/re/layoutdiff.py` tallies offsets per class | |
| A `malloc`'d size larger than the class | Extra E&B members | `FastAllocatorGeneral` 0xE30, ac56d39 |
| **literal, floats** | | |
| One `exp` call where ours has two (or the reverse) | Two `(float)exp` of one argument merge, `expf` doesn't | |
| `fild qword` (high dword 0) + `fidiv dword` | Unsigned `(float)u / (float)v`: such output parameters are `unsigned` | |
| `mov dword [..], 0` among register-zero stores | That member is a float | |
| **reading the client** | | |
| `ret 8` / `ret N` with ecx set, or never read | Still a member function, even if it ignores `this` | |
| A body equal to two later functions | Both are inlined into it (Process_Messages) | |
| 0x9CCD10 (`xor eax,eax; ret`) in a vtable | A pure virtual slot: `= 0` | |
| A static function right after an unrelated one (misplaced start) | A static sits right after its first out-of-line caller | 0x4B43E0 after `Pop_Prefix` |
| C2660 on wwlib vector.h's placement `new[]` | Declaring `operator new[]` in a file makes VC6 resolve it there: declare the allocator's members instead of including FastAllocator.h | app/shared/unknown_new.cpp, 66535af |
| **levers found 2026-10-26/27 (S3, T4, W3, F3, L4, P4, H5)** | | |
| A register mismatch on a converted argument (`ntohl(*(int *)p)`) | Assign twice: `len = *(int *)data; len = ntohl(len);` | tcpconn.cpp (S3) |
| A pump loop duplicated / rotated by the compiler | `while (true)` where ours had `for (;;)` | tcpconn.cpp, udpconn.cpp (S3) |
| A callee the client inlines at one site and calls at another, nested | A nested inline method moves its own callees out of line (`RSAKeyClass::Decrypt` inline took the CRT helpers out) | `Pump_Pending` 0x82AA10, 6a2d0dd |
| The client keeps a nested `_Construct` out of line | `std::distance(begin, it)` spends the budget that `it - begin` doesn't | `Remove_Dead` 0x822B50, a844f2b |
| Locals share dead parameters' stack slots in the client | Declare the locals at the top of the function, not in a later block | udpconn.cpp (S3) |
| A virtual call in a destructor | `((Base &)*this).X()` | dbgconn (S3) |
| A dead `ntohs(Port)` / accessor call the client keeps | Keep it; constructor members in the initialiser list | udpconn ctor 0x82C680, e5661c2 |
| Our inlined call that the client makes out of line (string::erase, size) | One or two dead `size()` calls *after* the call site push the earlier call out of line; only the first dead read inlines, the rest are called | `ClientTaskClass::Init` 0x766350, 6bd0611 (T4; D40) |
| Dead `size()` reads at the function's own top level change nothing | They spend budget only inside an inlined callee: a find-if-set wrapper holding 1-2 reads makes 0x598370 exact, 0 or 3+ don't (D130's limit counts the reads written, 6 per function) | ui_group 0x598370, 400bb63d (UG2, 2026-10-31) |
| An empty inline wrapper around auxdata.h's find | Enough to send the find out of line, but the budget it frees lets later callees inline instead | ui_group 0x596770 / 0x592C50 (UG2, 2026-10-31) |
| A small function whose client calls the *first* case's helper out of line (one `_Eos`) | A D130 dead spender placed *after* the last `return` (unreachable, marked not the client's) can be the lever where one at the top fails: the top forms sent later cases out of line, the end one exactly the first case's | ui_manu 0x5B6060, a0da65f7 (MU2, 2026-10-31) |
| Nested inline temporaries flip (the sixth `??_H` Transpose temp) | VC6 spends nested inlines in reverse source order: an inline cost placed *after* an expression (`Matrix4(view)` in a restore) decides whether that expression is inlined | distortion draw 0x9961E0, 342 -> 121 rows (W3) |
| The budget model (F3, confirmed by T3/H3/F5) | Function-wide, spent by every expansion anywhere in the function; unused plain locals add nothing; dead accessor reads add; template declarations visible in the translation unit (delta.h's include chain) move which instantiations inline, so a header change can gain and lose pairs in files that never use it. The budget is file-state dependent: a form that failed earlier is worth one re-run after the file's members change (L4's struct ctor broke the land ctor on 2026-10-26 and is exact on 10-27, F5) | Ship 0x6F66E0 probes; aux_roots 0x702240/0x7045F0 vs 0x7014B0/0x701D70/0x758210 (H3) |
| The client divides at run time where ours folds (`65535.0f / (WWMATH_PI/2)`) | A function-scope `static const float` reproduces the `fdiv` | `Prepare_Lighting` 0x46C730, dad084d (L4) |
| A caller's destructor gets extra EH states (7 -> 11) | Defining an out-of-line class's destructor in the same file inlines it into the caller's destructor; leave it declared only when its bytes occur at several client addresses anyway | land.cpp (L4) |
| A temporary for an `Add()` argument grows the frame | A cast temporary instead of a named local | opcode 128, 2e5cd01 (P4) |
| An order-only residue in a scheduled store pair | Model the 8-byte member as two ints (`Unknown68`) | opcode 108, 27bcb40 (P4); candidate for 219's ctors 0x893970 / 0x893A70 |
| `std::string(const char *)` shows three inlining profiles in one file (`_Tidy` inline / `assign` out, the reverse, `_Eos` out) | Budget, not source: don't read the profile as a different form | tutorial_manager.cpp (H5) |
| A virtual `Set_Step` the client inlines | Call it qualified, `LoginTaskClass::Set_Step(...)`; a plain virtual call misses | tsk_login.cpp (T3) |
| A hashtable's `_M_delete_node` out of line in the client, inlined by ours; callers inlining more table helpers than the client | In a template VC6 compiles `p->~T()` by where `T` comes from: a function template's parameter (`destroy`, `_Destroy`) gives `push 0; call ??_G` (the scalar deleting dtor), the class template's own `_Val` in a member gives a direct `??1` call, and the callers then inline less. `_M_destroy_val` member in sgistl_adapt.py (H6, 51782f6: +10 whole-app, none lost). Members out of class, `inline`, `#pragma inline_depth`, allocator forms, find's body: 0 or losses | kv_list 0x4B6520, combo_ffactory 0x4E5230, lookup_table 0x4ECE50 |
| A hashtable's find called out of line in the client, the key's string constructed inline ("find out of line, key inline") | The caller-budget lever (H12, 085a407, 7 of 7): write the find as `const_iterator it = T.find(std::string(name)); return it == T.end() ? NULL : (*it).second;`. An iterator with two returns inlines find instead; callers that inline such a Find need no dead reads. Table-level changes don't help (H6, P-H6-1). Generalises to std::string-keyed finds: the `const_iterator` local is the essential part, the temporary, the ?: form and the `(*it).second` spelling are optional (H15, 4 of 4); it does not reach `const char *`-keyed tables (kv_list 0x4B6B60: no key-string destructor to order) | fontdatabase's finds and adds; ui_line_display 0x5A8B60 / 0x5A8C70, tutorial_manager 0x6ACF50, combo_ffactory 0x4E57A0 (H15, 9d82ed9-d648f87) |
| Our loop hoists addresses into stack slots the client doesn't have; the client writes a temporary into a dead parameter slot | Move loop-invariant inline math (a Matrix3D multiply, Vector constructs) out of the loop: VC6 then spills nothing | `SegLineRendererClass::Render` 0x9BEB20, 68b9100 (F4: one move after W3's 16 forms) |
| A repeated null test of a literal / 64-bit memory value: the client re-loads and tests again, ours threads it | Unsolved stop signal: VC6 keeps the second test when the tested value is a rematerialised constant or a 64-bit memory operand, threads it on a register value; 8 header and call forms plus a 30-flag sweep changed nothing. The lead: an E&B wwstring.h whose literal constructor does not call operator= | ShdMesh ctor 0xA08F80, 0xA0EC30; `Unknown_50` 0x7683F0 residue (F4) |
| Index-building loops: the client keeps a running row base | `int row1 = 1; ... row1 += nx` with the Adds in the client's order; also makes the static guards exact (`mov cl,al; or cl,N`) | distortion draw 0x9961E0, 1306 -> 1045 rows (F4; frame and loop registers remain) |
| An out-of-line loop function that is also inlined one level into its callers | A self tail-recursive `X_Rest(p, left)`; a plain `while` loop gets fully inlined | boolpars' `_Rest` functions (H8, 51201d4) |
| A whitespace skip inlined at some sites, out of line at others | A macro, not an inline function | boolpars (H8) |
| A `static const int` read from memory where ours folds it | A class static defined after its uses | 0xAECC44, boolpars (H8) |
| `Value(...)` stores into a +4 subobject that keeps its own vtable | A member object, not a second base | boolpars' nodes (H8) |
| The prologue loads the arguments' members in the wrong order | A temporary or reference local that reads an argument member first decides the order: `const Vector3 &min = boxtest.SweepMin;` | HeightField Cast_AABox 0x465030, 4a0fd03 (L5) |
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
| A float copied through the FPU (`fld`/`fstp`) where ours uses `mov` | The value goes through an inline setter's parameter; a direct assignment gives `mov` | soundlib's dialog volume and Doppler members (H9, ed0a28f) |
| An out-of-line 0x13-byte copy of an SGI hashtable iterator in the client | The `const_iterator(const iterator &)` conversion: the client declared a const_iterator | ui_line_display (H10) |
| `end()` read through the wrong register in a vector append | `push_back` and `insert(end(), x)` differ (`[ebx+8]` vs `[esi+0xa0]`): try the other | ui_line_display Flush_Lines (H10) |
| A callee the client calls out of line only inside an inlined function, inlined at the direct sites | Wrap the call in a one-line inline helper (depth +1): the budget drops the deepest nested expansion, not the biggest or the latest in source order (the depth counted is the chain's cost) | land's lightmap threads 0x46DA70 / 0x46DDD0, 615b995 (F5) |
| Many repeated inlinable calls all out of line in ours, the budget spent on later expansions (the reverse of F5's depth lever) | The repeated calls sit in an inline helper: write it as a macro, and they inline again | Render's sea Adds 0x474B30 (LD7, 55f1d2c5) |
| `and ~0x40000` / `or 1` on the render-state flags beside a `??_H` Matrix4 / Matrix3D array | `Set_Transform(D3DTS_WORLD, Matrix4(true))` or `(Matrix3D(true))`, not `Set_World_Identity` | Render 0x474B30 (LD7) |
| A duplicated WWPROFILE destructor test at a branch's end | An early `return` in that branch | land (F5) |
| Both sums of a Vector2i computed before the two stores; `lea` addressing of the target | `Set(a, b)` on a reference local (`Vector2i &min = UpdateMin`); member stores don't give the `lea` | land (F5) |
| An unreferenced client function next to a NOT EXACT caller | Often the out-of-line copy of a non-static helper the caller inlines: define it non-static, non-inline in the .cpp; the copy matches and the callers keep inlining it | land 0x46EE60..0x46F0A0, 0x470EF0, 0x474590 (LD6, dd60da4c) |
| A 12-byte local holding a translation | `Matrix3D::Get_Translation(Vector3 *)`, the out-parameter overload; `Get_Translation()` gives another frame | Save_Decorations 0x479A50 (LD6) |
| `test ecx,ecx; sete` where ours has `test cl,cl` on a flag tested then flipped | The flag is an `int`, not a `bool` | Build_Index_Buffers 0x478240 (LD6) |
| Index scheduling off in a function whose same-file accessors ours expands by hand | Call the client's accessors (`Get_Height(i, j)`, `Get_Vertex`): they fix the scheduling and spend the inline budget the client spends | heightfield 0x464600, 0x465D30 / 0x466180, 0x4654E0 (LD8, 8d47d1ed) |
| A whole function's register allocation off in a strtok loop | One `char *token` for every strtok result | Load_PGM 0x4637C0 (LD8) |
| An early `if (x == NULL) return(false);` keeps its own epilogue where the client shares the FAIL_IF exit | Wrap the rest in `if (x != NULL) { ... } return(false);` | scriptedmovie 0x807600 / 0x8078D0 / 0x807BE0 (SM2, 2cf99b80) |
| ebx / ebp saved up front in the client, after the early exit in ours | Declare locals at first use and return per branch; top-initialised locals plus one final return delay the pushes | Play_Audio 0x806BD0 (SM2, 5bf43599) |
| An out-of-line 16-byte copy (`mov eax,ecx; ... ret 4`, 0x80B100, 0x780AC0) | An inline user `operator=` the budget cut: declare one | scriptedmovie's destructor (SM2) |
| One inline source that fixes the inlined copy and breaks the out-of-line one, or the reverse | Expected: the same inline compiles differently in each place; judge by pairs over both | 0x801D70 / 0x807520 (SM2) |
| The client still indexes `array[index]` in a branch that tested `index == 0` (ours folds to `array[0]`) | It tested the member the local was copied from, not the local | ui_group 0x592A80 (UG1) |
| One shared indexed load before an if / else of two calls | A named pointer: `if ((p = a[i].Peek()) != NULL)` | ui_group 0x593180 (UG1) |
| A caller that expands a find loop matches only with `x.end()` re-read each pass | Don't cache `end()` in the loop | Get_Member inside Add_Member (UG1) |
| The destructors that expand a member struct's destructor gain an EH state | An empty user destructor on the struct: drop it | ui_group (UG1) |
| Two NameClass locals sharing one stack slot in the client | Put each inside an inline helper (a find-by-name helper); two block-scoped locals in one function get two slots | ui_manu 0x5B3640 / 0x5B3850 (MU1) |
| `setne` / `test` on a compare result | `bool changed = a != b; if (changed)`; a plain `if` or an inline compare helper doesn't give it | ui_manu 0x5B67C0 (MU1) |
| A pointer kept in a register across two member stores where ours reloads it after a float store through `RefCountPtr->` | An inline setter on the class | ui_manu 0x5B4850 (MU1) |
| A static `__int64` initialised at load time | `static __int64 x = 5;` gets a dynamic initialiser in VC6 | ui_manu 0x5AF9D0 (MU1) |
| A packet constructor's player id loaded before its other arguments | Read the ids through inline accessors (`Unknown_Avatar_ID(Client)`, `Unknown_Player_ID(Client)`) | ui_cpit 0x581CE0 / 0x583510 / 0x583250 (CP1, e83e04a1) |
| The client expands std::string assign in some of many temporaries, not all | Mix explicit and implicit constructions: the split tunes the budget (18 temporaries: half and half was exact) | ui_cpit (CP1) |
| A per-branch pointer reload | `(font = style.Peek())->X` | ui_cpit 0x5845A0 (CP1) |
| Switch cases with frame slots of their own | Temporaries, not named locals (named locals share one slot) | ui_cpit (CP1) |
| The client's zero register (`xor ebx,ebx; cmp reg,ebx; push ebx`) | Write the `= NULL` store in both branches | ui_scut 0x612430 (SC2, f5074a74) |
| A 4-argument `_Sort_0` call (push 0; push a garbage byte) | std::sort with a comparison functor; a functor `operator()` giving `mov eax,1` returns int | ui_scut 0x6129C0 (SC2) |
| `mov eax,key; dec eax; je` | A one-case `switch` with the default written first | ui_scut 0x610A40 (SC2) |
| An inlined same-file base slot scheduled differently, its out-of-line copy exact | Call it qualified: `Base::Unknown_04(list)` | ui_scut 0x610800 / 0x611220 (SC2) |
| A frame off in a loop over a computed index | A hoisted invariant in a named local: `int slot = Bar*3+1;` | ui_scut 0x611300 / 0x611710 (SC2) |
| A same-file member defined later in the file inlined in ours, called out of line by the client | VC6 /Ob2 also inlines members defined after the caller: `#pragma auto_inline(off)` around the callee | ui_chat_panel 0x56C0C0 into 0x5630B0, 0x5685C0 into 0x56C0C0 (CH1) |
| A register-only residue around a cdecl `Create_Get(f(x->member))` | An inline accessor for the member | ui_chat_panel 0x563DC0, 0x56BAA0, 0x562F50 (CH1) |
| EH state numbers or the bigger inline expansion on the wrong branch | Lay out `if (a) X else Y` with the client's then-branch | ui_chat_panel 0x563910, 0x565F80 case 5 (CH1) |
| `fmul dword` where ours has `fmul qword` | `float * literal` folds to the double: write `DEG_TO_RADF(x)` or `(float)atof(p) * k` | scriptedmovie 0x806AF0 (SM1, ac324bf5) |
| A direct member store or load scheduled elsewhere than the client's | Store through a local pointer to the member: `RenderStateListClass *list = &RenderStateList; list->Current = x;` | rs_track 0x460940 (RT1, 9c7ac3b2) |
| A near jcc where a short one fits, or `mov eax,[m]; cmp eax,ebx` for our `cmp [m],ebx`, in an "if NULL create; Add_Ref; return" getter | Return inside the creating branch too: VC6 sizes the jump before it merges the duplicated tail | proj_man 0x492290, 0x492380 (PM1, f72041ed) |
| `fld`/`fstp` (or a stack spill) where ours passes a field straight through | The source called an inline accessor (ProjectionDefClass's Get_Intensity / Get_NearZ / Get_FarZ) | proj_man (PM1) |
| A derived class's destructor that is a bare `jmp` to the base's | The compiler-generated destructor; an empty user-written one resets the vtables first | proj_man 0x494180 (PM1) |
| Overloaded virtuals in the wrong slots | MSVC lays them out in reverse declaration order (CullSystemClass::Collect_Objects(Frustum) slot 1, (OBBox) slot 2) | proj_man (PM1) |
| A strtok path walk with one strtok call at the loop top | `while (name) { next = strtok(NULL, sep); if (!next) { last step; break; } ...; name = next; }` | rs_track (RT1) |
| A RefCountPtr released twice in a row | `REF_PTR_RELEASE(ptr)` | scriptedmovie (SM1) |
| A direct call to a client virtual where ours goes through the vtable | Qualify it: `client->ClientClass::Add_Object(...)` | scriptedmovie (SM1) |
| A call order or frame off around by-value arguments | `Get_Client()` inside the argument list; a named copy before a by-value argument; block-scoped temporaries | scriptedmovie 0x808330 and others (SM1) |
| Which helpers an out-of-line insert calls | A non-inline same-file helper holding the push_back emits exactly those: a probe to identify them, not a shippable form | Get_Triangles (LD8) |
| Register-only: the client loads the called object after the arguments | A named local for that object (`RsTrackClass *track = ...; track->X(a, b);`) | cpacket 0x737610, 0x737C10, 070f7a5 (T8) |
| `G_ASSERT(!"text")` tests the string pointer in the client | VC6 doesn't fold it: keep the form | cpacket (T8) |
| Levers from control_keys_manager (H11, 2026-10-27) | `push_back(&*it)` where `push_back(it)` misses (a `vector<T*>` fed from `vector<T>::iterator`); a reference local for `operator[]`'s result (`M &m = map[k]; m.Init()`); `end()` tested first in a find-returning getter; a copy walk over two iterators where indexing misses; the cleared-key case returning first; a struct with its own constructor where an aggregate initialiser misses | control_keys_manager.cpp, 0452f48-28f0000 |
| An SGI lookup result the client converts through `const_iterator(const iterator &)` | A `const_iterator` local; it also fixes the callers' budget | co_data's Has and Remove wrappers (H13, 6ab411c) |
| One-byte temporaries where the client passes an empty tag type by reference | A fresh temporary per call | co_data (H13) |
| A dead `size()` read's position, not only its count, tunes the budget | One before the loop and one after: exact; two after: 0.81 | fontdatabase Save_Fonts 0x6C57B0, fd367bf (H12) |
| A class template instantiated in a header struct member moves a function's registers (`TPoint3D<int>`) | Define such structs in the .cpp after the affected functions | fontdatabase Load_Init (H12) |
| Calls in a `+` chain evaluated right to left where the client calls in source order | Separate `size += ...` statements | Get_Data_Size of 169, 171, 172, 82 (P6) |
| A base vtable store that survives in the client | A member initialiser reading through a reference that may alias `this` (a Matrix3D copy; an iterator whose constructor takes the container) | 30010, 888b60 (P6) |
| Several members reached through a pointer to a sub-block | Those members are a struct; helper members that ignore `this` (`ret N`, ecx unused) sit on the base | pkunknown_888b60 (P6) |
| The client loads packet fields or a member chain into eax and the other registers before the argument pushes | Read them through inline accessors | cpacket handlers 32-35, 66, 30, 98 (T10, 2f63e8a / bcd8401) |
| An out-of-line "constructor" called through `??_H` (vector constructor iterator) without `mov eax, ecx` | A default-constructor closure: give the constructor default arguments | ControlKeyStruct 0x71BD70 (H14, e92de96) |
| The client shares one report call between two FAIL_IFs over an iterator | A `const_iterator` plus `key_ptr++` on its own line | Load_Keys 0x71BD90 (H14, 2ee80ec) |
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
| A large constructor loses a neighbouring inline after a header layout change | Not the changed statement: objdiff for a call that replaced inlined code first (a budget knock-on) | tsk_login 0x767240 under the char[] ship name (T14, 3d6999c) |
| A string compare pushed out of line by a C-style reference cast between RefCountPtr types (it builds a temporary) | Cast through a pointer | cpacket ITEM_BASE 37 (T16) |
| `setne / test al,al` on a RefCountPtr test | `0 != ptr` (ref_ptr.h's template operator!=), not `if (ptr)` | client_lego_impl Init (H17) |
| `lea reg,[node+0x14]` on a table node's value | A reference local to the value: `T &v = (*it).second` | client_lego_impl Detach_All, Init (H17) |
| A frame 0x30 too small; int-move copies | A Matrix3D local (its user copy constructor); a float max as `min > r ? min : r` gives `test ah,0x41 / jne` | client_lego_impl Update_Radius (H17) |
| Two `sete` combined | A bitwise or of the tests: `(a == 0) \| (it == end)` | client_lego_impl Get_Lego_Transform (H17) |
| The const_iterator find lever on an int-keyed table | Works the same; a miss returned first | client_lego_impl Get_Bone_Map, Get_Knob (H17) |
| An aux-data type test (rtti.h, G_ASSERT 'b' line 32) inlined for some fields and called out of line for others | A class-static `UnknownRTTIClass<T>::Cast` (a function template with an explicit argument hits VC6's one-instance bug; a ternary gives the wrong shape); a cast nested three deep (wrapper, lookup, cast) exhausts the budget at the first expansions, two deep at the last; the dead-call lever (row 164) for the rest | co.cpp 0x700400, 0x6FFC80 (T17, 2c8ac53) |
| A base's sub-block destructed in the wrong store order as plain fields | The block is a struct with its own inline destructor | client.cpp UnknownB02C90Class +0x0C (T17, 3d26286) |
| A small callee the client keeps out of line in several callers | `for (int i = 0; i < 0; i++) {}` inside the callee: compiles to nothing, stops auto-inlining | sclient 0x745F30's four callers (T18) |
| Small residues in SClient's members | `++it` for `it++` (confirms f35ce66); `&Packet(args)` passed directly instead of a named local; `while (true)` for `for (;;)`; a callback's vtable stored first: assign in the constructor body, not the initialiser; `snprintf` is VC6's `_snprintf` | sclient 0x74C690, 0x74BB50, 0x745350, 0x74BEC0 (T18) |

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
* **VC6 inline budget, 2026-11-01 night run (several builders):** dead `X.size();` statements spend the budget non-monotonically (CordCon's ctor: 4 gave 0.91, 5 gave 0.30, 6+ exact: sweep the count, don't bisect; CC1). The budget is spent earliest site first: inline getters on later calls push an early small inline (map begin(), string _Tidy / _Eos) out of line (LG1, 0x5ACB80 / 0x5ADA50). Dead locals cost nothing, and a dead inline read after a call site does not change that site's inlining: spend is forward only (FB1). VC6 inlines member functions defined later in the same file (CordMgr's Send_Go_Away / SYN1 / SYN2 into Pump; NW3); `#pragma auto_inline(off)` around an empty or small function defined later keeps it out of line (HV1, CC1's Reset). A 2-argument `Dictionary::remove(key, temp)` frees enough budget for remove itself to inline (+190 bytes); wlib's 1-argument wrapper keeps it out of line (NW1).
* **VC6 SP4 value threading (2026-11-01, FB1, `.work/fb1/x/jt*.cpp`):** after `if (x == C)` on a register local, a later `x == K` test is folded on every path where x is defined, through copies, unions, any function size and no-code statements between; the test survives only for relational compares, undefined values (the variable becomes a stack slot) or real code between (then constant-assigned paths unfold too). pumpConnections 0x8BE270 and Send_Data 0x82D420 are LIMIT REACHED on it.
* **Bool and exit forms (2026-11-01, LG1 / LG3):** `return x ? true : false;` drops the `xor eax,eax` before setne / sete and the `mov al,dl` copy (`!= 0`, `(bool)`, `!empty()` keep them; 0x443B90, 0x443020). `p ? p->f() : false` inside an && test loads the member pointer straight into ecx for the virtual call (0x76B000). Early returns after a big local object repeat its destructor chain per return; nested ifs give the client's single exit (0x5AEA60). In switch cases, if / else + `break` versus `return` decides which copy of a shared tail the cross-jump keeps (0x5AD610). A 2-entry clear loop compiles to `xor ecx; mov; mov`; floats initialised to 0 store an immediate 0, ints the zero register (LG2).
* **Forms found on the 2026-11-01 day run (UI kit, input, fonts, network):** VC6 /Ob2 inlines functions and static free functions defined *later* in the same file: write the client's out-of-line helpers at their own positions or wrap them in `#pragma auto_inline(off)` (UI 0x6607D0, HV Get_Value); a new header with inline bodies can move a neighbour file's budget (gadget.h into ui_login 0x5AE390: auto_inline around it). Dead `length()` reads at a large function's top level do spend budget, from the end backwards (HV, Parse), against §3.3's note for small ones. Sibling block scopes share frame slots; a local declared later at the enclosing level does not overlap a closed block: place declarations from `sub esp` and `lea [esp+X]` (CC2). Buffer fills whose stores VC6 reorders: write the fields through inline pointer helpers (`Put_Long(p, v)`; Compute_Ticks exact; NW), and the order of two member stores can pick the stack slots behind them (ACK1). Bit tests: the client's `shr N; test cl,1` comes from an inline bool getter or a `(bool)` cast; a plain bit-field gives `test ah,4` / `test byte ptr [x+3],0x10` (UI, TG, GAD4); two bit tests in nested ifs with the body repeated give the byte load + two `test al` (GAD4). `GetKeyState`'s down bit matches only as `((unsigned)GetKeyState(k) >> 15) & 1` (INP). `delete p;` without `if` keeps immediate 0 stores (FNT1). `new T(a, b)` inside a call merges the null tests and inlines the template member; assign after new (INP). Switch case bodies are laid out in source order and the EH state numbers give it away (CV: 0x67A9B0, 0x67AB20, 0x679160 exact). A same-named overload of a base virtual takes the slot right after the overridden one (FNT1: rename it); the implicit dtor of a FontClass + RefCountClass class stores no vtables, a written empty one does (0x6BBCC0). A slot ending `ret 4` with nothing in eax takes an out-parameter (TG). An empty inline destructor on a local iterator gives the client's EH frame (UI 0x661940). Inline-budget fingerprints: `??_H` with the out-of-line Vector4 ctor 0x468E70, or a function the client inlines elsewhere (CV). Matching forms: a bool local for the return, a `DynamicVectorClass &` local for an inlined Add on a global (GC); a Vector3 slot result passed member by member: `Vector3 c = f(); g(c.X, c.Y, c.Z);` (GAD4).
* **More forms (2026-11-01 day run, late reports):** a `ret 4` function that writes its result through eax with no copy on the early return takes an out-reference (`void Get_Bounds(AABoxClass &box)`), not a by-value return (DW1, TG). Virtual calls inside a constructor / destructor mean the client called a base-class inline helper or a same-file member (DynamicMeshClass::Vertex), not the virtual directly (BTN). A `ret 8` constructor keeps its unread second parameter (BTN). `c ? f(c) : f('n')` gives VC6's branch-pushed argument with one merged call (BTN). A vector member at the start of a ctor's init list copies a byte from the argument's stack slot into the allocator (+0x0C): not a member (DW1). `if (it)` loops match with `it != NULL`; a never-re-read stack temporary is a reference bound to an element (`RefCountPtr<T> &old = Elements[i]`); a loop bound the client computes once wants a local, not the condition (DW1). `x * 2.0f` compiles to `fadd st(0),st(0)`; a `max()` pair's evaluation order shows named floats vs constructor arguments (DW1). `#pragma inline_depth(0)` around a ctor keeps an inlined member ctor out of line (IS 0x6DA930). In a switch, `default: G_ASSERT(false); case 0: ...` reproduces the client's EH states (LS).
* **Forms the Batch variants found (2026-11-01 evening run, D184 pilot; Opus 5.5, 11 of 39 made exact):** the operand VC6 reads first decides its register: cache it in a local declared first, or read it through an inline accessor (ClientClass::Set_Debug_Mode 0x728240, 0x5B10A0, 0x7451D0). A call to a later-defined member inlined under /Ob2 evaluates its arguments before the inlined test (PxBox 0x44BBD0 / 0x44BC10: the client reads Mask before testing List). An early return when not found, or the NULL return first in an if / else, reorders destructor sites and slots (0x597920, 0x6D1480); a conditional expression instead of an if (0x6AA340). `time >= Time` with the operands swapped keeps VC6 from deriving a range fact for the else branch's loop test (0x711520).

* **Inline-budget levers and D185 variants (2026-11-02 night run, NET, UDP, CV1, CV2, MSG2, GAD3, SM; VO, VC, PF):**
  - One inline layer too many or too few: `it._Inc()` written out removes a layer (NET, Key_Exchange 0x829180); a thin file-static inline forwarder adds one (UDP, Send_Data 0x82D420: `Find_Ptr` -> `getPointer`); a file-static inline wrapper one level down worked twice (CV2: pop wrapper with early returns 0x67CBD0, push wrapper 0x67C920; early returns give an iterator temp its own stack slot); a static inline accessor (gadget.h / split_string.h getters, `Get_Client()`) for member arguments. Where the wrapper sits around an STL call whose dead reads fail, try the other level.
  - Dead reads spend the inline budget and their placement matters (CV1: 7 dead reads in 0x678450; MSG2: three dead `Queue.front()`); dead `Dictionary` getSize/getEntries spend nothing (UDP); an inline member with no params for out-of-line blocks, and one with no callers is a plain member after its caller (CV1, SCR). Dead definitions fix operand order: VC6 lea order follows the first definition (`nbytes = ebytes = 0`, NET); removing a dead labelled block changes layout.
  - NOT EXACT bodies repeating later members: call them (CV1). Loop rotation: while on the other condition plus if-break when an enclosing if decides it (GAD3); an unrotated loop is `bool more` + `while(more)` + break (MSG2); `while(true)` and a two-step ntohl (NET). A WWPROFILE reporter guarded by `if` matches only as `if(!cond) return` (LD1).
  - **Variants and `__LINE__`:** a D185 variant that changes the line count moves every later assert (VO, VC, PF): pad with blank lines after the function (also behind a `#pragma`), `try` pads shorter variants with blank lines after and the applier must too; replace a removed NOT EXACT comment by a blank line above functions with asserts; variants drop `// NAME` tags and address comments (re-add); removing an `#include` moves asserts; byte-identical twins report losses and gains (apply to both, VC).
  - Constructors store in declaration order; `Member(NULL)` in the init list puts the store before the vptr stores; a dead `fstp st(0)` from a named float feeding `1.0f - t` (SM). VC6 inlines a later member of the same .cpp, so an out-of-line call in the client means another object: split the file (LD1; DISP: an appfile EXACT in one file but none in the client's object = separate object).
  - **Register and order levers, later round (2026-11-02, RS, CV2 round 3 47dd3add, KRN bb93b766, GS, RPS 20b0c157, DG round 2):** a member pointer that VC6 puts in ecx because it becomes `this`: read it into a local first (RS); a member argument pushed before `mov ecx,eax` after `new`: read it through a file-static inline accessor (CV2 0x67BF20 / 0x67A440, proven on two functions; retry parked register residues next to `new X(..., member)`); a float member copy as `fld` / `fstp`: the same file-static inline accessor (RS); loop-bound reads come before dead reads (GS); a missing EH `-1` store: an explicit `ptr = NULL;` as the last statement (GS); the client's empty destructor stores the derived vtables: declare an empty destructor (RPS); a ref-counted member set is release-then-add-ref (REF_PTR_SET aliasing, RPS); `(uchar & 0x80) != 0` gives `shr al,7` (RPS); /Ob2 budget: splitting a callee's expression over locals changes how many calls inline (KRN); a `goto`-label block's placement decides which sibling-branch statements merge (DG round 2). Variant appliers must restore every touched line (GS).
  - **Register and branch levers (2026-11-05 night run, CS2 c57fdf80 / cf91b286 / 90d823f0):** a member call's `this` read through a file-static inline accessor (`View_Of(this)->f(args)`) makes VC6 load ecx before the arguments (proven twice, ui_class 0x576AC0 / 0x575D50); a statement repeated at the end of both if / else branches is tail-merged and its constant kept in a register (0x576270's mask in ebp); a shared `return(true)` reached by `break` keeps the constant live over the whole switch (set before the jump table), returning in place loads it per case (0x575660); a backward step through a local (`prev = index - 1; index = prev < 0 ? 8 : prev`) gives test / jge (0x5746C0). Dead-read sweeps in large constructors only bite at 30+ reads (ctor 0x574710: 36-60 dead `size()` reads put four string assigns' `_Grow` out of line): sweep up to 100.
  - **Calling-convention and inlining levers (2026-11-05 night run, CS3, ui_shipcust 451e6173 / bff3641a):** a client `ret N` on a "free" helper that its callers reach with `mov ecx, this` is a thiscall member with `this` unused; `#pragma auto_inline(off)` fits any small member every client caller calls out of line (0x621E20, 0x6251F0, 0x627730); VC6 normalises a switch on cases 1..3 as `lea eax,[x-1]; sub eax,0`; a class whose ctor / dtor store no vtable of their own holds the described struct as a member instead of deriving from it (Unknown620D20Class::Desc).
  - **Source-order and constant levers (2026-11-05 night run, CS5, ui_shard 7fd9094d / c44f5000 / 4f8d4698, ship_pip):** switch case bodies follow source order: read the jump table's address order and write the cases in it; `(a*b)*c` with explicit parentheses compiles differently from `a*b*c` (0x80CE10); an else-if re-reading a member avoids a hoisted load where a bool local does not (0x616A10); an `or ecx, edi` where an immediate was assumed is two or'ed .rdata constants (`Unknown_B08CAC | Unknown_B08C9C`, 0x615A50); vector insert / erase callers matched with clear() plus exactly 4-5 dead size() reads (984bae18).
  - **The inline budget is spent from the end (2026-11-05 night run, CS6, 37dce85e / cd34248f):** dead reads placed after the last inlined call site (end of the last case / the switch) flip earlier sites' ~RefCountPtr out of line; the same reads before the helper or at the function's top do nothing (0x54DFE0: 2, 0x556B20: 6, D130's limit; 0x554E80 would need ~18). An inline setter for one member store gives the client's eax and the eax / edx rotation after it; a local pointer for two stores through one member gives its single load and tail merge. The client writes each paired key as its own case (no `key ==` test), letting VC6 inline a later small member and giving each converted temporary its own slot (0x54CBC0).
  - **Base or member, loop form, inline copies (2026-11-05 night run, CS7, ui_shipcust 10d4052c / aa6979e6 / d376ae05 / 8763165f):** a null-checked `this ? this + off : 0` around an inlined member dtor means the class is a base, not a member (Unknown620D20Class, ShipCustomizeInterface's second base at +0x84: +10); a `while` loop with `it++` at the end of the body costs less inline budget than the `for` header form (the map loops' `_Min`); Rotate_Y called in both arms of an if (tail-merged from fcos on) is different code from one Rotate_Y on a ternary; a dead `size()` in a function inlined elsewhere spends budget at every inlined copy, a set-never-read local bool spends only where it stands.
  - **Float compares and vector forms (2026-11-05 night run, CS8, decal 7ebdaa75 / 960c5212):** a compare against a float literal or a local const folds to `fcomp qword`; the client's `fld dword; fcompp` comes from a function-static `static const float`. `vector::insert(end(), 1, x)` written out is inlined, push_back keeps the 3-argument insert out of line; clear() on each array gives the client's pattern (the first erase out of line, later ones inline with copy out of line).
  - **By-value structs and dead-read sweeps (2026-11-05 night run, CS9, ui_shipcust 39b165a9):** a client `sub esp, N; mov eax, esp;` stores before a call is a struct passed by value (Vector3 / Vector2), even where the callee's bytes equal a version taking separate floats (0x627730 / 0x6251F0). A dead-read sweep is not monotonic (0x623080: 6 reads 0.50, 12 0.55, 14 0.35, 17 0.76): sweep every count. Put dead reads on an existing line (the return or `}`): a variant that adds a line moves later FAIL_IF lines and shows as false losses.
  - **Cross-jumps need an un-duplicated tail (2026-11-05 night run, CS10, tsk_login 521bd231):** in an if / else whose arms end in identical calls, VC6 cross-jumps only when the shared tail statement is not tail-duplicated: one `return true` at the end instead of one per branch made 0x7698E0's two sends merge (145 -> 60 rows); `ok = false` in each arm duplicates `pop ebp` into both and blocks it (0x76A7B0).
  - **Output pointers, matrix setters, init lists (2026-11-05 night run, CS13, unknown_585920 d64d7cf3):** a client function that reads its first argument as an output pointer and leaves eax unset returns void through a pointer, not a struct (0x5860B0: the callers kept their pairs); a float copied by fld / fstp into a matrix element is Matrix3D's inline Set_X / Y_Translation(float), not `m[0][3] = x`; a vtable stored after the member stores means the members are in the initialiser list (0x671300). VC6 propagates every local constant: a bool / int local never reproduces the client's `push ebx; mov ebx,1` before a jump table (tsk_login 0x769680, 8 forms).
  - **Conditional arguments, by-value pairs, sparse switches (2026-11-05 night run, CS14, gd_slider ea1041f4 / 35031724):** a base virtual call with a `cond ? true : x` argument matched only as an if / else with two calls (VC6 merges them, 0x66EE20); two floats the client copies through a temporary and an integer `mov` are a by-value Vector2 assignment (0x66FD80's +0xA4 / +0xAC); a switch on 1 and 5 whose cases share one `return` gives the client's `dec / sub 4` chain (0x66EF00).
  - **MAX macros, grouped constants, conditional ors (2026-11-05 night run, CS16, gd_slider 9888ef69 / 745726af):** WWMath::Max(member, x) takes its arguments by value and spills the member to a stack temporary: where the client reloads the member, use always.h's `MAX(a, b)` (a plain ternary gave `fcom mem` with the compare reversed); `-480.0f * y + 240.0f` compiles to fsubr, `(-480.0f * y) + 240.0f` to the client's fmul / fadd; `a == this || b == this` lays the else branch first, `a == this ? true : b == this` gives the client's order. Which operand VC6 loads first in a commutative add can flip when an unrelated expression elsewhere changes: re-check every compare after each edit.
  - **RefCountPtr member retypes (2026-11-05 night run, CS15, avatar.cpp 0e9e8dde .. 7a099e79):** the ctor's EH states (one per member) show which members are RefCountPtrs; retype one member at a time. A retype can flip x87 or `lea` operand order in a function that uses the member only after its loop: the inline `operator->` / conversion after the site is the cause, and neither operand order nor dead reads fix it (a raw-alias variant is the quick diagnostic; 0x781410 keeps BodyObj raw). A release sequence repeated in two places with a store order different from an existing member's is two written copies (if / else); with the same order it is that member inlined (0x7896B0 / 0x788EF0). An early `return` kept in place beside an EH dtor comes from setting a value the next test rejects (`frames = 0` before `frames < 3`, 0x7829B0); a second `return(false)` is merged and moved to the end.
  - **Twin files and stale NOT EXACT notes (2026-11-05 night run, CS17, ui_avatr / ui_avatar_cust c55ff176 .. 0c22f31b, +20):** forms from exact ui_avatar_cust functions made their ui_avatr twins exact or nearly (0x54FAA0, 0x54F390, 0x54F6F0, 0x54FFC0): port the twin's form before trying levers. A NOT EXACT comment can hide a functional misread (0x5502F0's last test read the wrong bool; that was its stack-slot residue). Parenthesise each translation's product (`(width * u) + left - center`) for the client's FPU order (two LIMIT REACHED rows went exact). A getter chain (`Get_Client()->Peek_Environment_Map_Manager()`, `Peek_Unknown3A4()`) at the call sites after the key handler matched; before it, it lowered the score.
  - **Shared slot parameters (2026-11-05 night run, CS18, avatar.cpp 46f73e6a / 3329533f / 03754bfc, +14):** retyping a shared `RenderObjClass **slot` parameter to `RefCountPtr<...> *` forces every caller's member at once: retype and verify the group together (the client's proof: Create_Get 0x47D2E0, then RefCountPtr's operator= inline into `*slot`). A client test of a part name's address constant (`mov ecx, imm; test ecx, ecx`) means the blocks were written out, not looped over an array.
  - **An EA inline before a stack-slot chase (2026-11-05 night run, CS19, decal 6af22ef5):** `new T; *p = *src` written out shares a temporary's slot differently from vertmaterial.h's inline `Clone()` inside the call's argument (`Create_Get(source->Clone())` gave operator new's temporary the client's [esp+0x30]): look for the EA inline first.
  - **Vector differences and written-out subscripts (2026-11-05 night run, CS20, d10090e7 / 8771ca25):** build the differences as a Vector2 first, then divide member by member, for the client's x87 order (0x785C00); parenthesise both products of a blend, `(keep * U) + (rest * W)` (0x781570, a LIMIT REACHED row); a local index (`int i = a*2+b; T1[i]; T2[i]`) gives `[reg*4+disp]`, the client's `lea / shl; [reg+disp]` comes from the expression written out in each subscript (0x55BFB0).
  - **Loop offsets, RefCountPtr tie-breaks, const floats (2026-11-05 day run, AR1, avatar 59cded21 / b4b90990 / 78191260, +6):** write a loop's start offsets as `i * step + k` in the body, not a running `at += step`: VC6's strength-reduced induction variables are then set up after the count test, as in the client, with their own frame slot (0x782CA0). A `lea` or x87 operand-order flip after a loop caused by RefCountPtr inlines: try a raw-pointer read of the member as a diagnostic; if it restores the order, the cause is the inline expansion count, not the operand order (0x782FF0). A float the client loads with `mov eax, [.rdata]` rather than an immediate store is a named const: `static const float x = 0.5f` (0x7831C0). A `sete` before a null test comes from a bool local (`bool fresh = !flag;`). `#pragma inline_depth(0)` around a function emits the compiler's ??_G / clear copies out of line, byte-exact, where the client calls them; inline_depth(1..3), auto_inline(off), an out-of-class / virtual dtor or a member operator delete do not make VC6 call ??_G.
  - **Map instantiations, inline budget, x87 clamps (2026-11-05 day run, SC1, ui_ship_cust b938161a / 44e58a24, +82):** two classes over one std::map value type share COMDAT symbols and the matcher trades their template addresses between the files: give each file its own struct when the client's `_Nil` globals differ (0xBE5E28 vs 0xBE5E44; the exposed false matches are D66 (iii)). A function defined later in the same file and inlined at a call site spends inline budget like dead reads do: use the client's own helper call (0x61F1D0) rather than `X.size()` padding. x87 clamps: separate `WWMath::Min` then `WWMath::Max` statements match the client; if-chains, `Clamp` and the MIN / MAX macros do not. In a file with asserts, add code by taking lines from a blank run before them; never copy a stale draft over the tree (it drops later edits).
  - **Later-defined inlines, ?: arguments, string-op budget (2026-11-05 day run, BF1, ui_cinfo c8645365, +14):** VC6 inlined 0x5741A0 into 0x573E80 though it is defined after it: 'defined later is not inlined' does not always hold. A `?:` choosing between two call arguments compiles to a cmov-like load; the client's `cmp / jne / push A / jmp / push B` is an if / else with a call in each branch (0x5734D0, 0x5741A0). 0x5615C0 inlines the later string ops and calls the earlier ones out of line (assign, _Tidy), the opposite of ours: the budget is not a plain source-order budget.
  - **Large x87 functions, Cross_Product's 3-argument form (2026-11-05 day run, LD7, land 0x46F120 1784 -> 688):** the masked residue swings without reason there: two forms that each helped often score worse combined, and the closest form is not always the best-scoring; compare diffs by hand. The 3-argument `Vector3::Cross_Product(a, b, &r)` is its own function in the client: since r may alias a or b, the inputs stay in memory, constant terms are not folded and a zero sits in a frame slot (`fsubr [zero]`); the returning form folds all of that. Unsigned sums show as `shr`, not `sar` (0x470F80).
  - **Dead reads after a return, Rotate_Vector's alias test (2026-11-05 day run, SB2, sbaseview e8ee868f, +5):** dead `size()` reads placed after a `return` (unreachable) still spend inline budget: two after each of 0x6A2120's returns gave the client's mix (assign and _Tidy out of line, _Eos inline), reads before the return did not. The static `Matrix3D::Rotate_Vector` folds its alias test only when both `in` and `out` are locals; its unused W stores are dead-store-eliminated. A helper shared by two functions would not inline even with __forceinline.
  - **Late edi pushes, allocator byte stores, covariant returns (2026-11-05 day run, TB1, ui_attrb_skills / ui_missn / ui_faction, +51):** a slot that saves edi only in its range-test branch comes out exact as `if (index == cur) A; else B; return(true);` (two early returns push edi on entry; 0x58DB30, 0x5C4EA0). A ctor's byte store from an argument slot can be a VC6 vector's allocator copied from an uninitialised temporary, not a parameter (0x549640 +0xC4). VC6 has no covariant return types: inline overrides returning `this` keep the base's return type. `#pragma auto_inline(off)` keeps later-defined callees out of line where the client calls them (0x54AB20, 0x54B050). FAIL_IF stringises its unexpanded argument, so a local `#define` can supply the client's expression text.
  - **Out-of-line helpers come from unreferenced members (2026-11-05 day run, AR2, avatar fc7cfe2a .. 5caa1964, +31):** when the client calls ??_G / clear / erase out of line in one function and inlines them elsewhere, look for an unreferenced .cpp member (no call / thunk reference in .text, no vtable reference) that VC6 auto-inlines into the caller: that extra inline layer leaves the nested helpers outside the budget (avatar 0x781C50 for 0x784700 / 0x77C080, 0x781B20 / 0x781B50 for 0x782200). List them with `python tools/re/unref.py FILE` (D195). VC6 counts inlined call sites, not callee size: a dead 0x781B20() spends the same as a dead size(); dead spenders after a call push the deepest expansions out first (erase's copy, then erase), never clear or ??_G alone: real member calls supply the budget instead. `list::clear()` is an extra inline layer over `erase(begin, end)`, as vector's is.
  - **Dead `add eax, off`, Rotate_Vector without the alias test, Vector4 slots (2026-11-05 day run, SB3, sbaseview 23ee29df / ae061444):** an inline `RefCountPtr::operator->` (or `.Peek()`) on a member of a call result (`((SClient *)x->Unknown_18())->ChatPanel->f()`) leaves a dead `add eax, off` after the load; a block-local `SClient *client = ...; client->ChatPanel->f()` removes it (0x69F8F0). Matrix3D::Rotate_Vector's alias test never folds when its input is a member: a file-static copy without the test matches the client, and VC6 puts the X term first unless the row is written `X + (Y + Z)` (0x6A1C90). Writing `tm[i][j]` by hand in a function that builds a Matrix3D pulls in the Row array's eh vector constructor. Contiguous float slots that reach a setter through fld / fstp are an aggregate: Vector4 matched; float[4] and Vector3 + float scored worse (0x6A0590). A `new` size 4 bytes over the class can be a stray `#pragma pack(4)` (ui_manu.h's UIManuClass, 0xBC8).
  - **Find wrappers, flag-driven block order, float returns (2026-11-05 day run, TB2, ui_missn / ui_attrb_skills / ui_faction, +41):** dead `size()` reads inside a wrapper before `Unknown_Find_Field` move the whole find out of line (6 for 0x5C4C90, 1 for 0x54A640); reads at the function's end do nothing, and once the budget is spent extra dead reads compile into real `size()` calls. A bool flag set in each branch then `if (flag)` reproduces the client's block order and shared blocks (0x5C5330, 0x54B140, 0x5C4C90). A file-static inline returning a float member passes it by fld / fstp; reading the member directly passes it by mov (0x54B140). Reading an id into a local before `Client->As_Space_Client()->f(id)` gives the push-before-vtable-load order (0x54A480); `Peek_Mesh_Object()` and a direct `Unknown1C` read allocate argument registers differently (0x5C5AC0). push_back of a copy of the pointer a FAIL_IF tested gives the store after the null test (0x5C4FC0). `bool stripped = false` before `in_tag` gives an immediate store (0x5C69D0). A file static defined after its caller is inlined unless `#pragma auto_inline(off)` surrounds its definition. Functions with no callers are still emitted (0x54B400).
  - **Literal summands, init lists, Matrix3D temporaries (2026-11-05 day run, AR3, avatar 2a25c533 .. d0bea9ff, +25):** VC6 moves a literal summand to the end of a sum (`x += a - b + 0.2f` adds x before the 0.2): a `static const float` or a float local for the first sum keeps the client's order (0x786EB0, 0x787130); a static const also stops a member load being hoisted above a branch (0x77D9F0). A ctor whose stores all come before the vtable write has every member in the init list, in declaration order; byte compares on a parameter mean a bool (0x77B870). `Matrix3D m = obj->Get_Bone_Transform(...)` emits ??_H after the call, a default construct then assignment before it. Use the static `Matrix3D::Rotate_Vector(tm, in, &out)` where the client has no temporary (0x787F40). A ctor moved out of a header keeps its stores in the body (nlp 0x78EB50). `near` is a Windows macro (C2181).
  - **Reference forms and commutative loads, wrapped bodies (2026-11-05 day run, GS1, gd_slider d0b2fecf / 048df636):** how an AABoxClass reference is formed decides which operand of a commutative float add VC6 loads first: from a `?:` reference `Extent.X + Center.X` loads Center first whatever the source order; an if / else into a pointer, then bound to a reference, loads in the client's order (0x66EF00; ~30 operand / parenthesis forms did nothing). Wrapping a body in `if (cond) { ... } return(false);` rather than an early `if (!cond) return(false);` shares the epilogue with a later `return(false)` (an assert path) and removed a switch case's cross-jump (0x66F330). Variants that add lines move later `__LINE__`s and show false losses elsewhere in the file: balance the line count first. Count instruction rows from `appfile.py match --diff` (`.work/gs1/tryv.py`), not the similarity score: 0.81 stayed while rows went 3 -> 0.
  - **A second vptr in a copy ctor, NULL re-tests, float arguments (2026-11-05 day run, AR4, avatar 7b332aeb / 80cc0af1, +2):** an implicit copy ctor that stores a second vptr before the member copies means the member's class has a polymorphic base: look for a `_purecall`-only vtable that only the copy ctor references (0xB046F4; the default ctor's base store is dead and does not show). A redundant NULL re-test (`cmp eax, ebx` on a value already tested) comes from `if (a && b) {...} else if (a) {...}`; a top-level `if (!a) return;` then `else if (a)` is folded away. An inlined float-returning helper passed as an argument spills to a stack temp (`fstp [esp+N]; mov; push`); the client's `push ecx; fstp [esp]` means the expression was written out in the argument (0x783620).
  - **Get_Client() in constructor arguments, element writes, shared Vector3 temporaries (2026-11-05 day run, SB4, sbaseview 501238d9 / fd69da82, mainview 39d3b17f, +8 and 13 tie-break):** a RefCountPtr assignment where the client loads `[edi]` and stores the member before releasing the temporary, with push-before-`lea` around Create_NEW, is fixed by the `Get_Client()` inline accessor in the constructor's arguments, and not only in the same statement (in mainview 0x50BA60 it fixed the lights' loop after it; two LIMIT REACHED rows cleared). Reading a member into a local before a temporary packet's constructor fixed register choices in another case of the same switch (0x69F8F0). A reference local to an array element schedules member stores late: write `Channels[i].X` in full (0x69F710). One Vector3 local shared by several Set_* calls changes which slot a later RefCountPtr temporary gets (0x69B5E0).
  - **Template layers as inline spenders (2026-11-05 day run, SC2, ui_ship_cust 52ab5b92):** a hand-written `notifier->Add(0, new MemberCallback(...))` inlines the callback ctor; SignalMap's template `connect(sender, object, &member)` is one extra inline layer and leaves it out of line as the client has it (AR2's lever with a header template instead of an unreferenced member). With 8+ dead reads after it, the layer itself goes out of line: use the shallowest template that fits. When the client copies a Matrix3D through ??_H in some blocks but not others, the budget runs out partway through the function: dead reads at the top move that point, reads at the end do not; the dead-read sweep is not monotonic (0x61DF80: 12 reads 0.60, 24 reads 0.45).
  - **Setters, predicates and dead reads on a copy's line (2026-11-05 evening, SC3, ui_ship_cust c3bd14d8 .. 596c8181):** a file-static `Set_Float(float &dst, float v)` gives the client's `fld` / `fstp` for a `static const float` store (a direct assignment compiles to `mov eax`). An inline predicate (`Is_Pressed(g)` returning `a == g || b == g`) lays each if's true block right after its test, as the client does; the expression written out puts the true blocks at the function's end. One dead read, or a file-static wrapper, on a struct copy's line decides how deep its nested `operator=` calls inline (0x61F250). The dead-read count that moves an expansion out of line depends on the file's other inlines and is not monotonic: re-sweep after every structural change (0x61A4D0: 3-5 reads call `_Min` out of line, 0-2 inline it, 6+ put `_Inc` out of line instead). tryv2.py once failed to restore a tree file (OSError 22) and left a variant in place: `git status` after every tryv2 run.
  - **Repeated destructor sites of one local (2026-11-05 night run, EX1, expr_math_parser_impl 0x4D10E0):** when the budget lets only some of a local's destructor sites inline, ours sends the textually first site out of line first. A dead inlined read placed after all the sites changes which one goes out; dead reads before or between them change nothing. The client's choice of a block-exit site over the earlier returns probably means a scope that closes before those returns (untried).
  - **Zero in ebp, float getters (2026-11-05 night run, ST1, ui_starbase 0x646360 / c7a76527):** with many integer zeros in one function VC6 caches 0 in ebp; a float-typed struct where the client has one (ui_class.cpp's float-field key event for `ControlKeyComboStruct(0x1B, 0)`) cuts the count. GadgetSlider's Value is read through an inline float getter (fld / fstp, not mov). A file's own small functions defined after their callers still inline into them (/Ob2) where the client calls them: `#pragma auto_inline(off)` around them (co.cpp's precedent).
  - **auto_inline pragmas and markers, a qualified call as an inline level (2026-11-05 night run, EX2, expr_math_tree 4fb2fa20 / dfd16dd1):** put `#pragma auto_inline(off)` / `(on)` on the blank lines around the getters so the asserts keep their `__LINE__`, and before the `// @` marker: `appfile.py match` reads a pragma after a marker as a function named auto_inline. Where the client's inlined push_back insert leaves size() out of line and ours inlines it, a qualified call to a sibling member (`Class::Visit_Leaf(node)`) adds the client's inline level, a more natural form than a dead `size()` read (both matched).
  - **Clearing forms, SGI hash_map node dtors (2026-11-05 night run, IE1 / EX3, ui_item_examination_attributes 34df6434, expr_lexer_match_list 0210dcf6):** `Set_Contents(NULL)` vs a `RefCountPtr<>()` temporary and `clear()` vs `erase(begin, end)` inline differently (`??_G(0)` calls vs inlined destroy loops): try both (0x533000). A named `Vector2 min_size` local where a temporary made the frame 4 bytes larger (0x532BA0). A hash_map node with an inline recursive dtor: VC6 calls the node's ??_G out of line from the outer owner and inlines it in the recursion; `delete node` in both places matched.
  - **Creator branches, RefCountPtr tests, EH frames per instantiation (2026-11-05 night run, EX4, expr_math_float / expr_math_int 7900a6cd .. 148b8eaa, +188):** a group creator matches as `if (name == "x") return Create_NEW((Base *)new X(l, r));` in every branch; one shared `node` and a single `Create_NEW(node)` gives `mov eax, esi` (0x4CC760). `if (result = f(...))` on a RefCountPtr goes through the pointer conversion (0x4C8370); `!= NULL` computes `setne bl` early. One template can compile with or without an EH frame for `new` depending on the instantiating file (float 0x4C8920 none, int 0x4CC760 one; throw() fixes one, breaks the other), and `appfile.py match` can call a template "exact" at the other instantiation's address: check the address. A switch matched only with its cases in the client's EH-state order (2, 1, 5), not its layout order. A loop count hoisted into a local before the loop gives the client's counted loop.
  - **Base order, chained-call arguments, deleting dtors (2026-11-05 night run, IE2, ui_item_examination* 2b1e9989 .. 56377e74, +112):** with a non-polymorphic first base VC6 moves the polymorphic base (RefCountClass) to offset 0: a refcount at +0x1C needs a polymorphic first base (0x531660: `inc [eax+4]` vs `[eax+0x1C]`). `a->f()->g(this->member)` loads the member before calling f: a statement and a local of its own gave the client's ebx (0x532640). Member reads as call arguments rotated eax / ecx / edx through a 1.4 KB function; inline getters or locals fixed it (0x53A2F0). A deleting dtor inlines the class's dtor only when the class's own-side interface has no virtual dtor (0x537880). `appfile.py match` paired a wrong 16x16 icon-size static with an identical initialiser (0x531630): read the initialiser's disassembly.
  - **Request packets, deletes in auto-inlined members (2026-11-05 night run, FR1, ui_friends d93d36ba .. 8c18fec0, +39):** `int id = Client->Unknown1138; ClientChatRequestPacketClass packet(N, id);` gives the client's lea-before-load order (0x591350, 0x5910D0; SB4's lever). Where a same-file member (0x5906C0) is auto-inlined into its caller (0x5908B0), the client's out-of-line ??_G / erase there came only with the `delete` inside a file-static inline helper in that member (the member stays exact). intrface.h's `__INTRFACE_MEMBERS` define goes on an existing header-comment line so no `__LINE__` moves. Editing a widely included header: give `commit_mine` `--path <header>` too, or its shifted names-review rows stay behind. Three parallel appfile.py runs can fail "objs.tar missing" (one obj directory): two at a time.
  - **NOT EXACT tags shift asserts, EH states as type evidence (2026-11-05 night run, PT1, ui_ptrade 57538c56 .. 7c685492):** adding `// NOT EXACT:` lines while copying a draft into the tree moves every later assert down: re-fit the blank lines (`.work/pt1/totree.py`) or keep the tags in the draft. An empty user-declared destructor (MoneyClass) adds an EH state to its owner's destructor: the state count tells a member's type (+0xE0 MoneyClass, +0xAC / B0 / B4 RefCountPtr in 0x5E1D80).
  - **Ids in locals, two inline layers, 64-bit forms, loop bounds (2026-11-05 night run, PT2, ui_ptrade 793fcdbb .. df4a4b33):** `int id = ((ClientClass *)Client)->Unknown1134;` before the call fixed the registers in 0x5E26F0 / 0x5E37F0 / 0x5E3C90 / 0x5E39F0 (and an early return's in-place epilogue). A file-static helper over ui_avatar_cust's Unknown_Find_Valid template sends the find 0x5466A0 out of line (two layers; one gave 0.48; dead Peek() reads nothing). `money = MoneyClass()` gives the client's `xor eax / xor ecx` store and stops 0 being cached in ebx; a dead high-half store of a MoneyClass member means it was read through an `__int64` local. `for (id = 1; id < 5; id++)` with `[id - 1]` subscripts matched 0x5E1FA0 (from 0 with `i + 1`: 0.68; `<= 4`: jle). `return(x->Active ? false : true)` gives `mov al / test / sete`, `!x->Active` adds `xor eax, eax` (0x5E3EF0). VC6 rejects a second `for (int i ...)` in one scope (C2374).
  - **Peek accessors for branch-local loads, a loop index that looks like a cached zero (2026-11-05 night run, PG1, ui_ping 75067ef6 .. 4cf1e2c8):** where the client loads a model in each branch and keeps the else block out of line, an inline accessor (`UIPing_Peek_Model(Button)`) matched; the bare member kept both blocks inline (0.78), a hoisted local 0.80 (0x5D85B0). Two if / else calls the client merged into one call with a branch-pushed argument: write them as calls, not a bool expression (that gives `setl`). A zero that seems cached in ebx through a loop may be a loop index from 0: look for `inc ebx` in the listing (don't filter inc / dec) before chasing registers (0x5D7D60: `Add_Object(Rows[i], i)`).
  - **The inline budget spent on the file's own helpers (2026-11-05 night run, CP1 / AP1, ui_playeropt 6cd200be .. 9c33e068, avatar_pip 92ffce31 / 36d572b9):** /Ob2 inlines same-file functions defined later; the client's 0x5D9410 spends its budget on an inlined call of its own 0x5DA570 (0x5DAB50 on 0x5DAD50): written out by hand, the budget expands the vector's `_Destroy`, `++` and `_Tidy` instead. Call the client's apparent wrapper. A member expanded inside another expanded member leaves its own nested calls out of line (avatar_pip 0x7E36B0, 0x7E3A20). A function the client never inlines (0x5DA2B0) stayed out of line written with an early return (`if (!on) return;`) rather than `if (on && Selection < 0)`. `clear()` vs `erase(begin(), end())` is one inline level. `Peek_Unknown84()` / `Peek()` + `Is_Active()` instead of bare members fixed register-only misses (0x5D9100, 0x5DA0D0). Two Matrix3D(true) locals in an EH function built through ??_H until early expansions went out of line: the budget, not the ctor. `commit_mine` without `"whole"` in the spec commits only the data rows.
  - **A ctor emits the base's inline virtuals, end-of-file inline state (2026-11-06 night run, AN1, ui_analyze 1cb4eae9 .. 7265bec6):** defining a ctor emits the vtable, and a class that doesn't declare its own overrides emits the base's inline virtuals with it (InterfaceClass::Unknown_54 cost 0x515690 in 11 files): declare the client's overrides (+0x54 0x5439F0, +0x60 0x5443F0) even before they are defined. A later helper the client inlines at one site and calls from another (0x546000): repeat its body at the inlining site and wrap its definition in auto_inline(off). Templates take the inline state at the end of the file: `inline_depth(1)` there stops Cast inside an inlined find and also Release_Ref inside ~RefCountPtr. 0x5461C0 matched only as a switch (an if chain 0.56).
  - **Bit tests through bool accessors (2026-11-06 night run, AT2, ui_attrb 29adfe67):** plain `(x >> n) & 1` folds to `test byte, mask` like a bare bit field; the client's `mov eax,[p+4]; shr eax,n; test al,1` came only from bit fields read through bool accessors, in a union with the whole word for the `== 0` test (H18, 0x548D60). `int id = ((ClientClass *)Client)->AvatarID;` before `Send_Packet(&SkillPacketClass(id, 1, m))` fixed the register order (PT2's lever, 0x548C50).
  - **auto_inline(off) against same-file inlining (2026-11-08 night run, A4, TL1, A2):** `#pragma auto_inline(off)` / `(on)` on blank lines around a small same-file helper stops /Ob2 inlining it into later callers without changing the helper's own bytes (rs_list 0x45C430 exact around 0x45B990); and a function defined later in the file is inlined into earlier callers once it exists (ui_scan 0x60E580 fell 0.85 -> 0.36 when 0x60EFE0 / 0x60F1A0 were added; the pragma around them restored it). An explicit template instantiation inside such a region keeps that template out of line for some types (`Unknown_Find_Field<DeltaStringClass>` became the client's 0x514BD0), not for others (the Reputation lookup 0x610370). A function-scope `#pragma inline_depth` does not stop /Ob2 inlining a same-TU member, and `#pragma inline_depth(1)` at a file's end wrecks the file (CW2: ui_chat_window's 21 exact fell to 8).
  - **Array fill index forms (2026-11-08 night run, RH2):** `&a[n++]` twice and `&a[n]; n++;` twice change VC6's register allocation and induction pointers a lot (enbscene 0x75FB00 0.59 vs 0.84).
  - **8-byte members and by-value DurationClass (2026-11-08 night run, TL2):** an 8-byte member (DurationClass, `__int64`) in a class with a vtable makes VC6 pad the vtable pointer to 8 and the class grows (Unknown59E2C0Class 0x48 -> 0x50): `#pragma pack(push, 4)` as client.h does. The client's `sub esp,8; mov eax,esp; mov [esp+N],esp` for a by-value DurationClass argument means a user-defined copy constructor; but any change to duration.h or delta_template.h, even an unused declaration, flips aux_roots' 8 inline-budget pairs (and slot_data_render 0x6A7C00): try such an edit in a draft and run a full verify before keeping it.
  - **EH states give a switch's source order (2026-11-08 night run, PD2):** the EH state numbers a constructor's switch cases set follow their source order: starmap_pip 0x7C3220's are 4, 3, 0, 1, 2, and writing the cases in that order made it exact.
  - **Integer moves for a float pair (2026-11-08 night run, PD3):** where the client copies members with integer `mov`s and the draft keeps them on the FPU stack (`fst`), the members are one Vector3 (here a Vector2 + float pair, starmap_pip +0x68..+0x70) copied as a block.
  - **Overload slot order and string init forms (2026-11-08 night run, CW3):** VC6 orders RenderObjClass's Set_Animation overloads in reverse (the combo one at slot 0x94, the no-argument one at 0xA0; 0x90 is Update_Sub_Object_Transforms). For `std::string`, `name(p)` and `name; name = p;` give different inline profiles: only the second gave the client's _Tidy / strlen / out-of-line assign(PBDI) order with the EH state store between them.
  - **Out-parameters for by-value bodies (2026-11-08 night run, CW2):** writing the value into a caller-supplied `Vector3 &` reproduces a client body that a by-value return copies (the form stashed at docs/attempts/5719A0, P-CW2-1).
  - **Small forms that went exact (2026-11-08 night run, A5, A6, RH1):** an early `if (!robj) return false;` keeps a later struct's initialisation at function scope, its stores before the call's argument setup (galaxy_map_render 0x7A99F0); `Matrix3D::Transform_Vector` emits a run-time alias check (`cmp edx, eax; jne`) where `mulVector3` stores straight into the destination; a frame 4 bytes bigger came from `char name[MAX_PATH + 1]`; an eax / edx swap in a copy constructor went away reading through inline getters (rs_hanim).
* **Dead `size()` reads spend the inline budget (2026-11-08c, W1 / W2 / LV1):** one to three dead `x.size()` reads (before a loop, after a default-constructed string, or after the call sites) make /Ob2 call a later helper out of line as the client does: 0x572EE0, 0x6821F0, 0x6812C0, 0x646A00, 0x665940 went exact. Four or more undo it; it fails where the client inlines deeper than ours (0x4B5D70) or the residue is a shape. Where the client stores a string's EH state before its inline assign, the source default-constructs then assigns (`std::string s; s = x;`).
* **Loop forms VC6 rewrites (2026-11-08c, OD1 / OD2):** a one-trip loop whose body has an index ternary (`f(i == 0 ? A : B)`) is removed: write if / else to keep the client's loop. A search loop stays top-tested (NULL return last) only as `while (true) { if (end) return NULL; if (hit) return &x[i]; i++; }`; `for (;;)`, goto or the test in the head rotate it.
* **Inline copies come out in definition order (2026-11-08c, SC1):** VC6 emits pending inline copies in source definition order: define nested constructors before their root to get the client's depth-first order (Player's tree, 0x7551D0 / 0x753C60). An explicit empty destructor stores the vptrs; the compiler's does not: a client destructor with no vptr stores means no user-written one (W2, 0x67F230).
* **The inline-budget rule, measured (2026-11-09 night run, IB1 / IB2 / CP2; lab `.work/ib1/e1-e11.py`, census `.work/ib1/census.txt`, IB2's variant runner `.work/ib2/vr.py`, `.work/ib2/scan_fr.patch`):** the per-caller budget is raised by the caller's own non-inline code; a callee costs its *source* size (dead code counts, which is why dead reads spend it); `inline`, order of definition, explicit instantiation and `auto_inline(off)` do not matter, `__forceinline` ignores the budget. Top-level call sites spend in source order (the last ones lose first); inline calls after a site take that site's nested budget; expansions before a site take the later sites' top-level budget. **Levers:** (a) two inline layers with the NameClass built in the inner one and the root copied to a local (replaces D130's dead spenders); (b) one inline call after an inlined helper. **By-reference root:** a `char *` root is read through a `char *const &` cast in the inner layer, and a root read *after* the NameClass needs the helper to take it by reference (0x583790 exact, CP2). **Freed-budget trap:** lever (a) or deleting a dead spender frees budget and pulls *later* code inline (check the whole file's pairs, not the target). `?:` between two inline finds expands both branches (CP2: `WarpAvailable ?:` per branch in 0x5821C0). An unused *named* `size()` local spends the caller's budget like a dead read (IP1: fixing it made 0x527F40's frame right). Trivial inline accessors spent nothing in IB1 / IB2's cases (IR2's `Get_Filter()` moved it in another function: test per function). **Counter-examples where the rule did not decide it:** ui_missn 0x5C4C90, ui_group 0x597E40, lookup.cpp's std::map find 0x4ACA90 / 0x4ACB20, ui_cpit's 0x5821C0 (0.62) and 0x5806E0 (0.66, whole-function budget), 0x57D6C0 (0.58). The census's `o` entries in small callers are the next candidates.
* **Constructor / destructor and bitfield forms (2026-11-09 night run, IP1, XR1, FN1):** a bitfield tested through an inline `Is_X()` gives shift-then-test (write the inline accessor, not `flags & BIT`); an `&` test of two bit-set values may go through `rend()`'s form (XR1); a destructor's *definition order* sets the EH state numbers of the classes involved; a function that only returns and is referenced only from unwind funclets is an empty user-written destructor; byte-identical destructors are attributed by their funclets (XR1, D66 (vi) territory: 0x7E1CA0 / 0x7E1CF0 against 0x604150).
* **Argument and loop forms (2026-11-09 night run, FN1, JB1, TM1):** a by-value struct argument is copied from the caller's stack copy (FN1: FontSoundStruct in Play 0x6BD270); an `if` / `else` on a bool moves the pushes where `?:` does not; a four-iteration zero loop is unrolled; per-case calls the client tail-merges are written as separate calls per case (JB1); an inline player-id accessor sits before the virtual call it feeds (JB1); the avatar id comes from the inline `Get_Avatar_ID()`, not `->AvatarID` (TM1, three functions); an inlined `std::string::find` needs `find(s, 0, strlen(s))` inside `while (>= 0)`; `switch` cases follow the source order of the client's pushes; `memset` versus a loop decides a register order (TM1).
* **One class out of line, another inline means two objects (2026-11-09 night run, XR2):** a function emitted out of line from one class but inlined from another (the `_com_ptr_t` wrapper copies in unknown_7dfed0 / ui_registration) is two COMDATs in two objects: check which TU the client's copy belongs to before chasing the shape. A large `#import` header changes register allocation elsewhere in the translation unit (the full msxml3.tlh lost ui_registration 0x5FA5F0; msxml.dll's whole header does not, D222). **`#import` the client's own type library, found by its GUIDs:** a coclass CLSID is data, so pairs never see it; msxml3.dll's `DOMDocument` is `F6D90F11`, not the client's `2933BF90`, and the tree built a different object than retail until D222 (`.work/msxml/guids.py` searches client.exe for a GUID list).
* **Forms from the guild and font windows (2026-11-09 night run, GL1, FN2):** empty a VC6 vector with `clear()`, not `erase(begin(), end())` (GL1: the two differ in the inlined body); two `Change_Text` calls in an `if` / `else` are not a ternary; an esi / edi or al / cl swap around an Active test goes away reading it through the inline `Is_Active()` (GL1: 0x81CB90, dtor 0x81E1E0's inlined Unknown_5C). FontSys's own `Bits` (+0x2BC) shadows `RenderObjClass::Bits` (+0x18): the client's 0x6D60C0 reads the base one, and the shadowing was a real bug in the POC (FN2); a member copied to a local lets VC6 thread the tests; `Vector3::operator[]` spends inline budget, `.X` does not; of several inline `Play` copies the last is the most inlined (0x6D60C0 may need a little budget spent between the copies).
* **Assignment as a spender (2026-11-09 day run, MF1):** `T x; x = T(...);` through a user-written inline `operator=` spends the inline budget where the matching constructor does not: a zero-code spender placed after a site that cuts that site's deepest nested inline (ui_mfd 0x5BE120: DurationClass assigned after the two-layer find sends Cast 0x5BF350 out of line as the client, exact). A struct with only the compiler's bitwise operator= spends nothing this way. Try it before dead spenders (D130) when an object with a user operator= is built after the site.
* **Loop, clamp and while forms (2026-11-09 day run, FN4, MT1):** VC6 unrolls a small zeroing loop with its own zero register: four ecx stores next to a shared ebx zero mean a separate `for (i < 4)` loop, not explicit stores or memset (0x6CE7D0 exact). A two-value clamp can swap esi / edi / ebp choices no spelling changes (0x6CD950, ~120 forms): record "register" and move on. A loop over a struct array whose client base pointer sits at a later member comes out exact with a reference local, `UIMountPointStruct &p = Points[i];` (ui_mount 0x5C7630). A test of a loop-invariant float before the first pass (`fld; fabs; fcomp` ahead of the loop) means `while`, not `do-while`. A variant runner that edits the tracked file breaks once a variant is committed: make each edit conditional (`.work/fn4/w.py`).
* **Fill and layer forms (2026-11-09 day run, FN3):** an inline `memset` of a member array computes `lea edi` after `mov ecx` / `xor eax`; where the client has `lea edi` first, write the clear as a byte (element) loop, which VC6 turns into the same `rep stos` with the `lea` first (FontSys +0x303: 0x6CD6F0, 0x6CDAB0; about 23 of the client's inline zero-fills are lea-first against 65 count-first, `.work/fn3/scanms.py`). A callee at the caller's top level is inlined ahead of nested ones even with dead spenders before it: spenders cut the deepest expansions (0x6CE4E0: the copy's `operator=`), never that callee, so a top-level callee the client calls out of line means a layer in the source, not a budget form (0x6CE4E0 exact through the file-local inline `Clip_Text`; about 20 budget forms failed). `appfile.py match --pool` on an untracked copy next to the tree file is a ~16 s variant runner with no importer lock (`.work/fn3/run.sh` + `mk.py` in the FN3 worktree); delete the copy before verify, or verify compiles it too.



### 3.2 Templates and STL

* **VC6's `vector::clear()` is an extra inline layer over `erase(begin(), end())`** (2026-10-31, TK8, cordcon Reset
  0x84E770): it changes which `_Destroy` / `??_G(0)` calls stay out of line. Making a callee exact can inline a shared
  template everywhere and lose its pair (insert(P,x) 0x855C30): check pairs_cmp, and win it back by fixing a client
  caller (openc).
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
* **A hash_map loop's iterator declared in the `for` header (2026-10-31, TK3, fontdatabase 0x6C4F50):** VC6 makes it
  `begin()`'s return slot and keeps its node in memory. The client declares it before the loop and assigns it in the
  `for` (`iterator it; for (it = T.begin(); ...)`); a const_iterator copy puts the table pointer in a register instead.
* **A hash_map value whose destructor the client never runs** (2026-10-31, TK1, P-H11-2 solved: operator[] 0x71D4D0
  leaves its pair temporary alive, 0x7198B0 frees nodes bare): give the value class a private, empty, inline destructor
  and make the `std::hash_map<K, V>` its friend (a private destructor alone is C2248 in operator[]). It also frees
  operator[]'s inline budget, sending the hashtable copy constructor out of line as the client has it.
* **STL and wlib containers, 2026-11-01 night run:** SGI hash_map members the source leaves default-constructed (100 buckets already): writing `Systems(100)` moves `_M_initialize_buckets` inline into another member (GM1). Partial inlining depends on the iterator type: a hash_map iterator member declared `const_iterator` (begin()'s iterator converted) reproduced begin() out of line and size() called; an extra inline helper layer around begin() also pushes size() out of line, forwarding-only wrappers change nothing (GM2). `vector::resize(0)` shows as an out-of-line size(), `test; jbe`, then `erase(begin, end)` after a zero store (CC1). E&B's wlib arraylist.h getPointer sets the out pointer NULL on a miss (NW6, 61199041): don't write call-site NULL forms.
* **VC6 mangles a function template as `?Name@@...`, not `??$Name@@...` (2026-11-09 night run, IB1):** a COMDAT name that lacks `??$` is not evidence against a template: compare against the client's symbol by the plain `?Name@@` form before concluding the function is not a template instance.


### 3.3 E&B app code

* **Stand-ins on the POC path (2026-11-04 night run, CRE1, SHIP, LIGHT, DEC):** a stand-in class used with `new` needs the
  real size (ui_avatr's ShipPip stand-in was 0xE0 bytes, the real ctor wrote 0xF4 and operator new crashed later); a
  stand-in's ctor / dtor defined in the same file, even last with `auto_inline(off)`, inlines into exact callers or breaks
  `$E` tie-breaks: put it in a separate file; renaming a file's stand-in to the client's class name links the real calls
  without including the real header (including charview.h in ui_avatr.cpp changed 3 exact functions). Lines added or removed
  above a FAIL_IF move its `__LINE__` (charview's stand-in removal lost 6 exact functions, 3 `#include`s at the top of
  ro_funcs lost 2): keep the line count or add at the end. gen stubs returning a NULL `const char *` (0x82F3C0) crash string
  concatenation; gen_stubs cannot stub a function with a `std::vector<struct>` parameter (the call links to 0). gen stand-ins
  also hide behind `extern` globals: avatar.cpp set `Unknown_BFAE52` instead of WW3D::OverbrightModifyOnLoad (grep `extern
  .*Unknown_` when a WW3D static is involved). Declare 0x80FF20 `float *` as its other callers do (`const Vector3 &`
  mangles differently); a stand-in declaration whose return type differs from the real one (0x495FD0 `void` vs `bool`)
  is another mangled name and stays a gen, and a stand-in class name (UIAvatrUnknown194Class) keeps real functions as gens:
  use the real header. Exact does not prove the right helper: `Create_Get` and `Create_Peek` compile to identical bytes,
  only the call target differs (ViewClass::Unknown_10's Get freed the quit box: QUIT, 2026-11-04); check the client's
  target with `client.py dis`.

* **Renamed vtable slots and header wrappers (2026-11-02 evening run, EAM4):** an inline compatibility wrapper for a renamed
  slot makes callers inline it instead of the client's `call dword ptr [eax + slot]` (rs_robj 0x45F280): change the callers.
* **`bool` parameters (2026-11-02 evening run, ROF):** `mov al, byte ptr [esp + ...]` on a parameter means `bool`; an `int`
  declaration changes the callers' code (ro_funcs' callers in ui_login / ui_cpit / eff_o still declare `int`).
* **A placeholder declaration hides a written helper (2026-11-02 evening run, AUD):** tsk_login declared `void
  Unknown_43FA80()` for SoundLibraryClass::Flush_Playlist (written, too short for the matcher), so gen_stubs stood in for it;
  grep the callers' declarations before writing a "missing" function.

* **An uncalled run of a class's inline members (ctor, copy ctor, operator=) whose incremental thunks are never called
  either** comes from non-inline definitions in the .cpp: same-file callers still inline them and the copies are emitted
  (2026-10-31, TK19, proj_man's ShadowTexClass 0x4916B0 / 0x491730 / 0x4917C0).
* **A function-local static `RefCountPtr<T>` (a lookup's null return) makes its atexit destructor inline Release_Ref,
  which needs `T` complete** (2026-10-31, TK16, galaxy_map): a forward declaration fails; an empty shell class deriving
  from the base works. The lookups match as `if (it == Map.end()) return null; return (*it).second;` (the `!=` form
  merges the two null returns).
* **A local set to NULL outside a loop whose address is taken** (push_back) keeps the client's dead store and frees the
  argument slot for the loop counter (2026-10-31, TK8, cordcon Add_Channels 0x84FAA0). appfile.py markers on two
  constructors with the same demangled name land on the wrong one: check with objdiff and the mangled name.
* **Renaming a packet class's member breaks other files that read it** (2026-10-31, TK9, pkupdate Planet's Unknown14
  -> Position): `grep 'packet->Unknown14'` missed extrap.cpp, and verify's C2039 dropped all 79 of extrap's pairs. Grep
  for `->Member` across app/ before renaming; any "1 errors" in verify's summary is a stop. **The client's float
  min / max loops keep each extreme as a separate `float`** (landdmap 0x468BE0: Vector3 locals 0.41, six floats exact).
* **VC6's inline budget moves with functions added later in the same file** (2026-10-31, TK6, ui_avatar_cust): adding
  0x5513E0-0x551C80 made 0x550A10 inline a third Rotate_Y, and 0x551BF0's form flips with 0x551520's body: re-run every
  marker of the file after adding functions. An out-of-line function with no callers right after a FAIL_IF handler is
  what an earlier function in the file inlines. A register rotation from the first instruction (`this` in ebp, not esi)
  was fixed by reading members through inline getters (0x556630).
* **A member store / load VC6 hoists above an earlier member store** (2026-10-31, TK11, dbgconn 0x824EC0), where the
  client keeps source order: put the members in a sub-object with an inline method (its own `this` stops the hoist);
  29 orderings of the flat class did not. tryvariants.py finds only a library's top-level files (`app/common/x.cpp`:
  "no such file"): for app files use appfile.py match on a marked copy (`.work/tk11/run_count.py`).
* **aux_roots.cpp's budget-bound functions flip on any declaration added to the headers it includes** (2026-10-31,
  TK5): one more Type_Name overload in strconv_lib.h, a declaration in delta_template.h or money.h, or a member operator
  removed from MoneyClass loses 6 aux_roots pairs (0x7014B0, 0x701D70, 0x7045F0, 0x7048A0, 0x705860, 0x705A10). Test
  header edits against an unchanged copy of aux_roots.cpp (appfile); declare locally in the .cpp when it's enough.
* **A key value built from 16-bit halves** (2026-10-31, TK1, control_keys_manager 0x71AEF0) is read as two words,
  `(hi << 16) | lo`, not one dword cast. **A loop the client runs in one branch only can look like a shared tail**
  (0x71C710): check where that branch returns before restructuring.
* **VC6 folds a constant product into one constant** (2026-10-31, TG4): `x * 0.5f * 1000.0f` and `x * 0.001f * 50.0f`
  compile to one fmul; the client's two fmuls need the intermediate in a named local (cameraeffects, ui_combobox).
  A flag the client stores before an unrelated load needed `volatile` (ui_combobox 0x57AC70 / 0x57AD50); a clamp
  matching `ja` (not `jae`) is Clamp_Int's form on unsigned, not always.h's min or std::_cpp_min (0x57A330).
* **RenderObjClass's slot numbers in the client don't match a count of the tree's rendobj.h** (2026-10-31, TG4): read
  the slots from the compiled object (objdiff) instead; 0x180 / 0x184 are Is_Hidden / Set_Hidden.
* **A hand-written `if (p) p->Release_Ref(); p = NULL;` schedules the store after the next load** (2026-10-31, TG3,
  sbaseview 0x69CDE0): the client's `RefCountPtr = NULL` stores first. Where the client does this on a raw-pointer
  member, cast the member to `RefCountPtr<T> &` (or make the member a RefCountPtr).
* **Land-family register and FPU forms** (2026-10-31, TG2): a local declared in the branch that uses it, not at the
  top, changes register allocation (Apply_Colors 0x46D410 exact with `extra` in its else branch); a float local for a
  shared value keeps it on the FPU and reorders the stores with an fxch (Get_Triangles 0x465D30 exact with `float y`
  for v2.Y and v3.Y); one texel index n with the pointers built from it gives the client's stepped pointers instead
  of i-indexed addressing (0x46D410); when the client's strength reduction keeps runtime base differences
  (`ebp - (ebp + 2)`), the source had a separate pointer per channel, since one pointer with constant offsets folds
  to displacements (0x46D6B0).
* **A getter whose caller copies the result through eax** (2026-10-31, TG1): it returns the struct by value. Read the
  caller before trying more body forms: a `Vector2 result`, member stores and one `return(result)` made avatar_desc's
  seven Get_*_Offset 0x4C1B80-0x4C2240 exact at once. An int copy of a float with Z = 0 stored first, into a
  TPoint3D argument, is TPoint3D's base-copy constructor over a TPoint2D passed as a temporary (`TPoint3D<float>(uv)`,
  Save_Defaults 0x4C2360).
* **Moving a class out of a file with asserts** (2026-10-31, LC1): replace the removed lines with as many blank
  lines, so every later `__LINE__` stays on the client's line (`wc -l` before and after must agree), before verifying.
  A string literal shared with a function in another range keeps both functions in one object *only when no other
  object uses it* (corrected 2026-10-31, TK6: literals ARE merged across objects; the object linked first holds
  them, so ui_avatr.cpp pushes ui_avatar_cust.cpp's 'COLOR_PALETTE': a shared literal's address is not file evidence): 0x7FFEA0 stays in scriptedmovie.cpp with 0x803310 ('CREATE_OBJECT'). A
  regex that strips `((T *)x)` casts also eats the call parentheses of `f((T *)x)` (C2065 on verify).
* **fontmanager.h / fontdatabase.h cannot be included next to sclient.h** (2026-10-29, UI1): tsk_game.h declares
  Unknown_BE67B8 with another type; SClientFontDatabaseClass is declared in sclient.h for now. VC6 inlines a
  same-file function defined later in the file too (0x802B20 inlines 0x807520).
* **Object boundaries and vtables** (2026-10-29, LS1): a guarded `atexit` static initialiser in the middle of a range
  ends an object: use it as the file boundary when there is no __FILE__; the link order (name-sorted .obj files)
  then bounds the real name. Name overloaded virtuals differently (Get_Text_Width / Get_Char_Width) to keep MSVC
  from grouping the overloads in the vtable.
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
  marked as not the client's; 16 since 8102bcf, window 15..17; 58 since 2026-11-09 SC2, window 57..59, and `auxroots.py` has a DEF_ORDER). After any change to a header aux_roots.cpp
  includes, re-sweep the pad (`tools/vc6/auxpad.py sweep`, aux_roots.cpp alone, ~5 s a variant) and keep a value exact for
  all six (10 since 93031785, AR2; it moved 15 -> 13 -> 24 -> 10 in one session, renames included: re-sweep after
  a rename too, even with the same number of declarations). Renaming a generated type changes mangled names the
  self names rely on (0x706610 lost): add `replaces` entries to `tools/re/symbols_named.json` and run
  `symbols.py --apply-named` (it rewrites those entries as source `named`, dropping length and proof: neutral
  here); out-of-line conversions need their client addresses named there before their callers match (AR2). A separate operator== among aux_roots.h's value types (free or member) pulls the
  six windows apart so no pad has all six exact; a single negated `!=` expression keeps a window (AR1, 2026-10-31).
  A sweep killed mid-run leaves its VM leased under `importer-PID` and a pad variant in aux_roots.cpp:
  `pool.py release VM --agent importer-PID`, then check out aux_roots.*. Windows are not pure shifts: a template declaration can move them apart so no pad exists (DeltaArrayClass's
  bounds accessor, P-DF1-2: keep such helpers local to the file). Explicit `template class` instantiations and
  padding between definitions move all six alike; VC6 rejects explicit instantiation of a constructor (C2909).
* **co.cpp's lookups and the inline budget (2026-10-28, UP1 / DF1 / IB1):** the budget cuts the bigger callee
  first and same-size callees in source order, so earlier finds go out of line first. auxdata.h's find is the
  client's early-return form (rtti.h's test inverted: `!= global -> NULL`; c5f115e). The lever for co.cpp's
  finds is a helper `Unknown_Find_Aux((C *)NULL, this, "Name")` that builds the NameClass before reading AuxData
  (one shared stack slot): 0x6F4610 112 -> 4 rows, 0x6FDBD0 34 -> 10; used everywhere it loses 20 exact
  functions, so the small finds keep their explicit NameClass. `#pragma auto_inline(off)` and `/Ob1` do not stop
  Unknown_Find_Field's inlining; for an out-of-line copy to compare, write a dummy with 40+ calls. A helper put on
  a new line moves every later `__LINE__` (~13 pairs): use an existing blank line.
  The client's find-if-set helper (bool copy 0x705450) is auxdata.h's find followed by "NULL unless +0x70 is
  set": co.cpp's `Unknown_Find_Valid` (DM1, d6798b3: 0x6FE1E0 96 -> 40 rows). VC6 did not inline 0x6FEAC0 into
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
* **Trying a shared header's macro without touching it:** copy the header into a private folder with the same include guard (`.work/fi1/inc/fail_if_new.h`), replace each compiled file's line-1 comment with `#include "fail_if_new.h"` (no line moves) and pass `EXTRA_INC = ['libs/sgistl', '.work/fi1/inc']` to tryv2.py (spec: `.work/fi1/v2.py`). Other builders never compile a half-done header. `tryvariants.py` refuses app subdirectories ("no such file in the tree"): use tryv2.py there (FI1, 2026-10-31).
* **One `__FILE__` window can mix two files** (appfile.py): `.work/h8/layout.py` places each `#line N` from a draft
  and pads inside a function when several asserts share it. explain.py reports a static's atexit `$E` as a false
  relocation before the real class.
* **A `VectorClass<T>` member needs `operator==` / `operator!=` declared on T** even when no constructor is emitted
  (VC6 compiles VectorClass's virtuals; declaration-only operators on CameraModeStruct; T7, 2026-10-27).
  Any program that constructs an SClient needs a stub for the registry read 0x75A280 (D69).
* **Frames and vector forms (2026-11-04 day run, NET):** neither declaration order nor where a local is declared moves VC6's frame slots (120 ACK2 orders, 11 Pump_Connections positions: identical output): don't sweep them. `push_back(x)` keeps `insert(P, n, x)` out of line as the client has it, where `insert(end(), 1, x)` inlines it (pumpInternal 0.18 -> 0.76). A comment-only rewrite can shift pinned `__LINE__` values without losing a pair (D185's d3ab3e4c moved cordcon's FAIL_IFs 3 lines): check the client's pushed line numbers after any line-count change. objdiff.py picks the wrong function when one name prefixes another (Pump / Pump_Connections): `--n`. The mangled names fix parameter types (buildMsgHeader `...IED`: pass the int fields uncast; a `(char)` cast makes byte loads).
* **Matrix3D inlines (2026-11-04 day run, PIP):** a Matrix3D inline (Translate(x, y, z)) the client calls out of line at only some sites came out of line once its block was a nested inline (a file-static inline helper, ship_pip's `Unknown_Zoom`, NAME 1); dead reads before it changed nothing. A float constant loaded with `fld` (not `mov 0`) and stored through `[esp+N]` pairs a row apart (0x20) points to a Matrix3D temporary in the client; no `Set(Vector3)` / `Matrix3D(Vector3)` form reproduced it yet. Variant runner swapping one `// @ADDR` function in an untracked copy: `.work/wt/pip/.work/vf.py ADDR NAME variant...`.
* **Shrink-wrapping (2026-11-04 day run, HTML):** VC6 pushes callee-saved registers late (after early returns; the late ones popped first in every later epilogue) only when a single branch region after the split uses them; two sibling branches using them push everything at entry. Return-value merging, `if (x != NULL) {...}` wrapping, else-if chains and locals declared up front did not change it. The same residue: htmlview Draw_Item 0x7767A0, Value_To_String 0x84C470, ScriptedMovie Play_Audio 0x806BD0: prove a lever on a test function first.
* **sbaseview forms (2026-11-04 day run, SBV):** `appfile.py match --diff` prints nothing unless the function has a `// @0xADDR` marker (put it in a scratch copy: an extra line shifts the asserts' `__LINE__`). A clamp the client writes as `fld const; fcomp st(1)` matches `WWMath::Min(1.0f, v)` / `Max(0.0f, v)` with the constant first (ifs / ternaries 0.79-0.82). A field the client reads before a virtual call (ActionPacket arguments, +0x84) matched only through an inline accessor. gen_stubs leaves a declared-only member returning std::string unresolved: hand stub in stubs/app_stubs.cpp. A `pool.py lease --agent` VM can show as another holder's later (release then finds nothing).
* **aux_roots pad windows and stand-ins (2026-11-04 day run, STRC):** a SYMBOL_PAD sweep can show 5/6 with identical totals across 40+ pads before a narrow window (43..45) turns up: sweep all of 0..63 before concluding. The window moves with aux_roots' inputs: re-sweep after every rebase that touches them, even when the generated output is unchanged (STRC's 43..45 became 33..40 once SCI's changes landed; 44 then lost 8 pairs outright). After a killed sweep, a pad's results reach the log only once the importer lock frees. Moving a conversion out of line clashes with udpreplay's hand stand-ins (LNK2005 through app.lib): grep tests/ for each signature.
* **Destructors and state stores (2026-11-04 day run, SCI):** an empty user-declared VC6 destructor stores its own vtable first, an implicit one doesn't: a vtable store at the start of a client dtor means the dtor is declared (auxroots.py USER_DTOR). A state store before an out-of-line call means that callee's body isn't visible in the client's TU (e.g. a full specialisation with out-of-line members: delta_money.h). Lock-free SYMBOL_PAD sweep: `appfile.py match` on untracked `aux_roots_pN.cpp` copies with your own leased VM (~9 s per pad; `.work/wt/sci/.work/sci/padsweep.py`); `auxpad.py sweep` stalls on the importer lock. An `appfile.py match` obj dir holds only the last run's object.
* **New-file order and plain members (2026-11-04 day run, BUFF):** a static helper whose asserts sit after a big caller can still be defined before that caller in source (the client emits it after either way); defined after, the UIBuffInfoStruct dtor COMDAT 0x561A60 stopped matching. A member that never uses `this` (ret 4) and has no callers is a plain member the earlier functions inline (0x561B40). A bool stored and reloaded as a dword (`mov [esp+x],bl; mov ecx,[esp+x]`) comes from a plain `bool good = ...` local passed to both calls. A leased VM can be stopped mid-session by the pool: on an ssh timeout check `pool.py list` and lease again.
* **Switch returns (2026-11-04 day run, BIO):** case labels sharing one `return` compile to a range check (test / jle / cmp 2 / jg); the client's dec / je chain needs one `return(true)` per case (ui_bio 0x560800). `appfile.py match` with an explicit `-I app/client ...` list fails on co.h's `..\packets\` include: use the default `-I auto`.
* **Dead reads on a FAIL_IF path (2026-11-04 day run, AVS2):** where the client inlines `~list`'s `erase(first, last)` but calls `it++` / `erase(it)` out of line on a FAIL_IF return path, two dead `size()` reads on the copied list flip it (ui_avsel 0x55F140, after three rounds of other forms). Keep such edits line-neutral (remove blank lines elsewhere): a +2-line variant "lost" four later functions on `__LINE__` alone.
* **Inlining and EH (2026-11-04 day run, EAM, REG2):** a client function that inlines a same-file helper one level and calls it recursively out of line is the source calling the helper, not a hand-copied body (a copy gets the helper inlined twice: explain.py's "N calls more"; Create_Render_Obj 0x48B220). To keep a `new`'s EH state with a same-file constructor that can't throw, define the constructor after its caller (control_key_list 0x721E60). A key buffer's size moves stack slots: 0x5F8FD0 needs `char[15]`, [16] swaps the INIClass and the buffer. Variants without the importer lock: `appfile.py match` on an untracked copy next to the tree file (app/client/X_v.cpp) or a draft under .work/ (EAM's runner `.work/wt/eam/.work/eam/runv2.py`); tryv2.py has no `--pool`: set `ENB_VC6_HOST(S)` to a leased VM first. Git Bash `sed -i '3a\#include "..\\..\\x.h"'` mangles the backslashes too. If `pseudocode.py --show` answers "not decompiled yet" for everything, `.work/pseudo/` was emptied: extract `.work/pseudo-backup/pseudo-*.zip` into it, then `--all`. Cause on 2026-11-04 (twice): the coordinator removed wt.py worktrees with plain `git worktree remove`, which on Windows deletes through the worktree's `.work/pseudo` junction into the main tree's folder. Remove worktrees only with `python tools/vc6/wt.py remove ID` (or `run_branch.py cleanup`), which rmdirs the junctions first.
* **ui_avatar_cust (2026-11-04 day run, UAC):** a window dtor whose callback's base ctor is out of line (vtable over 0xAEDB58): `Get_Signal_Map().Remove(sender, this, &fn, Event())` over unknown_signalbase.h's `EventCallbackClass<EVENT>` (a named local + disconnect inlines it); removing a derived class's own Event_Type override loses other files' short-known MemberCallback pairs (0x541B50, 0x541BB0, 0x53D130). In drag functions `tm[2][3] = 0` is the client's (a missing 12th dword of the matrix copy). Forms that move spills / scheduling: box edges or float locals declared up front, a Vector2 set member by member, an inline Set(u, v) (arguments right to left), Matrix3D Set_X / Set_Y_Translation instead of `tm[i][3]` stores, a named local for a virtual's result; static const floats give fld-constant-first code (the client uses literals). Adding lines above a FAIL_IF function moves its `__LINE__`: re-pad the blank run before it.
* **Create_Get / Create_Peek and stand-ins (2026-11-04 day run, CGP):** swapping the two can cost a pair through D100: two same-bytes COMDATs of one object propose one address and neither is credited (ui_cpit 0x584DF0; fixed by D192: the range round now drops a survivor the object's own exact callers contradict). A dead assign after the final `return;`, on the last body line to keep `__LINE__`, is a working D130 spender for std::string `_Grow` budgets. A named local for one argument of a 3-argument cdecl call swaps the eax / edx / ecx use (0x5B42F0). Local stand-in classes and free declarations hide the client's symbols (UIManuButtonGadgetClass, Unknown_65EFB0): explain.py's "relocation: unresolved … a different declaration?" line finds them. `importer.py verify` can give up on the importer lock and leave imports.json stale, so pairs_cmp shows a false 0 / 0: grep the verify log for "giving up" first. Python on Windows writes CRLF in text mode: `newline=''` for tree files.
* **aux_roots (2026-11-04 day run, AUXR):** a root built inline in another file's function is extracted with auxextract's `run(start, this, regs0={'esi': this}, stop=addr)` (INLINE_SEND); its classes go in aux_roots_send.h, not aux_roots.h, or the SYMBOL_PAD windows split (3/6 at best). Once sclient.cpp news DeltaPlayerClass or the Hull root, clienttest pulls aux_roots.obj and hits LNK2005 on the factory stand-in: stand in each symbol only aux_roots.obj defines (tests/clienttest/aux_roots_stubs.cpp). Get_Value_String needs a `const T &` parameter in Value_To_String (by value: another register); Set_Value_String 0x75A140 passes the value itself to sscanf (a client bug, keep it). auxpad.py rejects `--pool` despite its docstring: `sweep --agent ID` leases its own VM. A quick exactness check: an appfile draft of sclient.cpp with `// @0xADDR` marker lines (~3 min).
* **ui_class forms (2026-11-04 day run, CLS):** INI code calls `Find_Entry(section, entry)` directly (EA's `Is_Present` adds an `entry == 0` test VC6 does not fold for an array argument); `x / 180.0f * WWMATH_PI` and DEG_TO_RADF fold into one fmul, the client's two fmuls need the divide as its own statement; a nested inline can go out of line inside its caller where the client repeats the body; two Get_String calls in an if / else tail-merge at the call (a ternary argument does not match); a `for (;;)` counter gets rotated and peeled, the client's `int i = 0; while (true) { ... i++; }` keeps i in memory with the increment after the locals' destructors; 0xBDA908 is the pooled "" literal (strlen + assign), not a static std::string; dead `Descriptions.size()` reads move the inline budget (2-3 gave 0x576AC0's 17 inlined assigns).
* **ui_registration (2026-11-04 day run, REG):** a `#pragma inline_depth(N)` appended at a file's end changes how other functions in that file inline template members (templates take the inline state at the file's end): a lever to try on out-of-line template calls, but check every marker of the file. A callee returning a file-local struct by value is skipped by gen_stubs: hand stand-in in stubs/app_stubs.cpp (else clienttest LNK1120). RegistrationInterface's 0x5FB330 / 0x5FB540 / 0x5FB750 are members (0x5FA900 calls 0x5FB330 with ecx = this).
* **App-code forms, 2026-11-01 night run:** a shared epilogue can be file state, not source: write the functions before it in the file first (galaxy_map's Add_Data / Remove_Data went exact unchanged; GM1). Members passed as arguments land in the client's registers when read through inline accessors, and a field read before an out-of-line call in the same expression means an inline getter (VC6 loads a plain field after the call) (LG2 0x55E370 / 0x55E2E0, LG1, HV2). A bitfield bit tested as `mov; shr; test al,1` needs a bool getter returning the bitfield (a plain test gives `test byte,4`; LG2, LG3's Pre_Run). Two adjacent floats set to (0, 0) store out of order where the client had a Vector2 with an inline ctor (GM2). A frame 4 bytes larger as the only difference: look for a struct holding an 8-aligned `__int64` (LG1's LoginTicketInfoClass). A field loaded from 0xAECC44 is `UniqueDesignIDLookupClass::INVALID_ID`, not a literal 0 (GM1). `bit8` returned as `bool` shows as `setne` after an inlined Dictionary::remove (HV1).
* **Object layout, 2026-11-01 night run:** an unreferenced function in an app object with incremental-link thunks (after Create) is a non-inline .cpp function, and an uncalled function can be a lone out-of-line member that matches as a normal pair (GM1, NW1 0x82DCB0). An uncalled inline member (operator== used only through a vector) gets its client copy only from an out-of-line definition (NW4). A static function is stored by its address with no incremental thunk: it marks a file-local function and an object boundary (HV1). A new `#include` in an app .cpp must replace a blank line, or every assert below it moves (GM1). Log calls are pinned like asserts: cordmgr.cpp's on lines 592 / 618 / 1272, and E&B's INFMSG pushes `__LINE__` as an immediate before the " " string 0xB6E948 (cordcon: 355, 390, 1068, 1141, 1749), so code added above them removes the same number of blank padding lines (NW3, CC2; two commits broke it without a lost pair).
* **Client facts from the 2026-11-01 day run:** E&B's INFMSG keeps matchbot's `char timebuf[40]` while strftime is told 80 (pumpClose exact), and any harness linking CordCon needs old iostream (LIBCIMT) (CC2). ViewClass slot +0x18 stays `int` (MainViewClass overrides it; a bool base opened a new slot, -15 pairs); keep inline accessors (Peek_Model, Get_Unknown198) instead of direct member reads (ui_login -12); 0xBFAEE0 is WW3DAssetManager::TheInstance: `Get_Instance()`, not a file-local extern (CV). E&B's rendobj.h: slot 0x184 Set_Hidden, 0x180 Is_Hidden: check slots by compiling, not by counting `virtual` lines (GAD4). A member called `Surface` hides wwlib's type (`::Surface &`; FNT1). The tree's WW3DAssetManager is already 0x408C (MC). Retyping a base's `void Unknown_XX()` virtuals gives C2555 in every stand-in subclass that redeclares placeholders: grep descendants first (`.work/gc/derived.py`); a header whose inline ctor copies `RefCountPtr<CameraClass>` must include camera.h (C2027 in other includers; SORT1). A compact fixed-line assert / FAIL_IF bounds where a file can start too (gd_resize, GAD4). VC6 puts out-of-line copies of inlines among a file's own functions, so they are not object boundaries (INP). A template body identical across instances (MemberCallback's Is_Equal / Call / ??_G) pairs only inside the compiling file's window (LG).
* **A derived window's `View` member hides InterfaceClass::View (2026-11-05 night run, CS21):** a member named `View` in a window class shadows InterfaceClass's +0x48: 0x61ADA0 compiled `[ebp+0x230]` where the client reads `[ebp+0x48]` until the member was renamed.
* **Two floats through a reference (2026-11-06, GP1):** a by-value 8-byte argument loaded as `add eax, N; mov edx,[eax+4]; mov eax,[eax]; push edx; push eax` is two float parameters read through a reference local (`const T &p = g->M; f(p.X, p.Y)`), not a struct; getters, a by-value struct and casts did not give it (0x53F9E0). `+0` / `0-` sums in a rotation mean `Matrix3D tm(pos); tm.Rotate_X(a)`, with zero locals from folded multiplies; a 16-byte value read as X, Y, Z, W / tan(fov/2) is a SphereClass (0x5406A0).
* **Accessors, clamps and multiply order (2026-11-06, VB1, MP1):** an inline accessor can decide a register: `mov eax,[ecx+N]` against our `mov edx,[ecx+N]`, or a swapped eax / edx around an interface member, when a plain member read is passed on: try `Get_X() { return X; }` (0x81F610, 0x692E00). An unexpected self-store of a parameter (`mov [esp+0x3c],ebx`) marks an inlined by-value callee: a sibling being called, not repeated (0x81F6A0 / 0x81FA40). A one-slot `new`-temporary difference went away with the iterator assigned inside the `if` condition (0x81F890). An FP-only clamp with the bounds in stack slots is not `WWMath::Clamp` when the value comes from memory: a local `if (v<lo) v=lo; else if (v>hi) v=hi; return v;` is exact; Clamp stays exact when the value is on the FPU from a preceding expression (0x7A5980 / 0x7A5E10). `(a * b) * c` and `a * b * c` differ when c is a float local on the FPU (0x7A5E10). A FAIL_IF on `!iface_ptr` over a RefCountPtr parameter, cached in a register afterwards, is a raw local `InterfaceClass *iface_ptr = iface.Peek();` on the assert's own line (0x692B30). A RefCountPtr<InterfaceClass> passed to InterfaceSortClass's constructor gives C2440 in ref_ptr.h(249) naming RenderObjClass, not the caller, and drops the whole file's pairs: pass `.Peek()`. When padding a draft's `#line` gaps, pad before the signature only for the function's first assert.
* **`<string>`'s initialiser marks object ends; forms from chat_window (2026-11-06, CH1):** every object including `<string>` ends its code with the same 32-byte guarded initialiser of `std::ctype<unsigned short>::id` (guard 0xC416FC): scan the app range for `8A 0D FC 16 C4 00 B0 01 84 C8 75 08` as boundary evidence. `memset(member, -1, sizeof)` instead of a fill loop removed the client's zero register in 0x67D430 and changed the whole constructor's register choice. A vector at +4 shows up as a `mov byte ptr [this+4]` allocator copy, which reads like a `bool` member. A deleting-only destructor with no first vptr store (0x6815A0) is an implicit destructor: declaring `~T(){}` adds the store. `attempts.py record --forms` takes a count; prose goes in `--evidence`, and `--blocker` is an enum.
* **A codeless inline layer for two budgets (2026-11-06, SG1, 0x613BA0 / 0x613D70):** when one caller calls a nested callee out of line while the direct caller inlines it, and a sibling keeps only std::copy out of line, one codeless inline layer fixed both at once. It worked around either half of the inlined function; both layers at once lost 0x613D70.
* **`importer.py verify` without `--lock-wait` (2026-11-06, SG1):** it gives up when another builder holds the lock and leaves imports.json as it was, so a following pairs_cmp silently reads stale data: check verify's last line first.
* **Hoisting, x87 stores, setters and friends (2026-11-06, ST1):** VC6 hoists common code out of if/else arms and merges tails: a `fld Frame` before the branch and `push ecx; fstp [esp]` before a `cmp` are plain arms with the call repeated in each; a shared float local gives different code (0x7C8980). `fld m; fst [tmp]; mov eax,[tmp]; fstp m` around an inlined setter comes from an inline accessor returning float: `Set_Frame(Get_Frame())` (0x7C8980, 0x7C6BF0). A frame 0xC larger than expected with z unused is `Matrix3D tm = ...; Vector3 pos = tm.Get_Translation();` (0x7C8490). An eax / edx rotation that diverges after a store came from an inline setter: `opt->Set_Value(x)` against `opt->Value = x` (0x7C6700). Create_Peek(this) on a derived class needs RefCountPtr<Derived> instantiated first (`enum { X = sizeof(RefCountPtr<UIStarmapClass>) };`), or the friend resolves to the base's and the COMDAT goes missing (0x7C81F0). A by-value parameter added to a shared header needs a forward declaration, or files that don't include the type lose all their pairs.
* **Overrides on a stand-in base; small float forms (2026-11-06, PS1):** a class deriving from a stand-in base must declare every override the client's table has: without Get / Set_Sort_Level, D188's vtable slots gave two addresses for RenderObjClass::Get_Sort_Level and avatar, land, rs_track and stars lost 0x462FF0 / 0x463010 with no change to their code. If unrelated files lose short pairs, run `vtables.py --seed C=ADDR --list` on the new object first. Four floats loaded in reverse (fld, fld, fld, then stores) mean `RectClass::Set(l,t,r,b)`. `Vector2::Set(x, y)` stores directly where `= Vector2(x, y)` goes through a temp. A double literal `0.5 / x * y` assigned to a float gives `fdivr qword`. asserts.txt can attribute an assert to the preceding __FILE__ (0xB9F7AC is target_find.cpp's, not starmap_pip_support.cpp's): check the pushed __FILE__ with `client.py refs`.
* **`importer.py` commands that give up on the lock (2026-11-06, PS1, CH1):** `report` exits 0 after "still held; giving up"; `verify` leaves imports.json stale, so under a queue a landing is refused with "the commits record other pairs than the verify gives". Pass `--lock-wait 3000` (`wt.py bring-over` and `tools/agy/rebase_gen.sh` do); a refused landing's cure is `wt.py bring-over ID --batch-name N` (it commits the verified record).
* **Forms from gd_attrb_skills and ui_verb (2026-11-06, AS1, VB2):** assert lines running backwards against execution are each the else of its if: `if (x) {...} else { G_ASSERT(x); }` (0x663DF0). `fild dword ptr [rdata]` holding a constant is an extern const int defined after its use; a literal and an int local both fold (0x664D5B). `mov ebx,[this+N]` before a branch, later `add ebx,0x50`, is a reference local `T &data = *ptr;` (0x663BE0). A pointer walked in a guarded loop, with `lea` after the empty test and `mov eax,ebx; add ebx,4`, is `array[i++]->f()` by index (0x6647F0). Consecutive assert lines with a store between them go on one line: `X = ...; G_ASSERT(X);`. A stack buffer of 0x84 bytes with a pushed size of 0x80 is `char x[129]` (0x659710, 0x659010). `Buttons.clear()` lets an uncalled function at VC6's auto-inline limit inline into its caller where `erase(begin, end)` stops it (0x659870). A find taking its value by value is `std::find_if` with a one-field predicate struct; `std::find` takes `const T &` (0x659F80). Inline accessors on every read spend enough budget to push a find out of line where dead `size()` calls barely did (0x658430). `sete cl; mov [slot],cl; mov edx,[slot]; and edx,0xff` is a named bool local passed as an int (0x658430: that alone made 1698 bytes exact).
* **Forms from the MFD objects (2026-11-06, MM1):** a named local fixes a 3-register rotation (eax / ecx / edx): `DeltaSkillClass *skill`, `UIMFDClass *owner = Owner`, or an inline accessor around `co->Client` made 0x5BF790, 0x5C0D30, 0x5BFB00 and 0x5C1220 exact at once: sweep these first. A `static inline` wrapper around `vector::erase(begin, end)` or `list::clear()` puts exactly one more callee out of line (`std::copy`, `iterator++`). Temporaries share stack slots with named locals, named locals don't share with each other: a frame too small by a matrix or vector means a named copy (`Matrix3D light_tm = camera`). A scheduling difference on a constant can be a sub-object: `Vector2 Size(-1, -1)` instead of two floats (0x5C2000). `objdiff` / `appfile --diff` hide literal differences behind relocations and masks: "none" with no visible rows means compare raw bytes (`.work/pk2/patcmp.py`): 0x3DCCCCCB against 0x3DCCCCCC was the whole of 0x5C2920.
* **Tail merges and values through sub-objects (2026-11-06, GS1, SK1):** a client `push A; jmp; push B; common pushes; call` is an if/else with the whole call in both arms (0x670890); a `push ebx` of a parameter just compared with a constant has the call after the if/else, because repeating it in the arms, or an early return, folds the constant (0x670AB0, 0x6A7020). `add ecx,0x50` after the argument load with `xor ecx,ecx` on a null path is an inline member on the returned class calling the sub-object's method (`p->Get_Level(x)`); `p->base_member.F(x)` gives `lea ecx,[eax+0x50]` (0x6A6EC0). The order of the high words in `add` / `adc` over two values returned through hidden pointers comes from `operator+` on the value class, with a `__int64 now` local before the calls (0x6A6550). .rdata literals descend in order of first use, so the highest address gives a switch's first source arm (0x6A7150). When the first assert sits on a low line, stand-ins must fit above it: one line each.
* **Sets and budgets (2026-11-06, SL1):** a ctor's `_Buynode` came out of line, with `_Init` inlined, only through two codeless inline layers used at every call site, with no dead read (0x630490). A set's destructor has three forms in one function: ~_Tree out of line; inlined with `erase` and `begin` out of line; fully inlined; which a site gets moves with spends made elsewhere in the function. A no-code spend after `_Tree::swap` changes the frame by 4 and the `set_difference` form, both earlier in the function (0x6309F0). When an assert's line leaves no room for a NOT EXACT comment, put the preceding one-line function on one line; a `NOT EXACT` mention in an ordinary comment also adds a not-exact.md row. `RefCountClass` has two virtuals here, so a derived class's first slot is +0x08.
* **std::sort's element, conditional bools, delayed stores (2026-11-06, TF1):** an empty default constructor on the element struct took std::_Sort from the 0x75EA60 form to 0x7C4E30's; a user copy constructor or operator= pushes _Median and the partition out of line. `cond ? f() : true` keeps `sete` / `test al` where `||`, `== true` and a bool initialised from `||` fold them. `float d = f(); entry.Distance = d;` delays `fstp` past the pushes of a following call; a direct assignment or cast does not. A loop comparing a pointer member to a register with `mov eax,edx; cmp eax,ecx` is `std::find` with `operator==(T *)`; `co == Object` against `Object == co` decides the operand order. A `#define` in another header (MAX_VISIBLE_GROUP_MEMBERS in ui_group.h) against an enum of that name drops the including file's pairs with C2059 in the new header, and pairs_cmp shows only the mass loss.
* **A killed verify or `appfile.py match` leaves its pool lease held (2026-11-06, TF1):** release with `pool.py release --agent NAME`. Don't put a shell `timeout` on verify: the lock queue can take many minutes.
* **Forwarders (2026-11-06, RP1, ro_proxy):** float-returning forwarders get the client's `jmp [eax+N]` tail call only as a ternary, `return (p != NULL) ? p->F() : k;`; the if form gives `call` (0x4805D0-0x480630). A copy constructor can use the default base `RenderObjClass()`, not `RenderObjClass(src)`: a call with no push, members copied one by one (0x47FB60). Similarity 0.78 with every diff line looking equal is a jump displacement hidden by `REL`: compare masked bytes (0x47FCD0's `je` skipped the NULL store: `if (p) { p->Release_Ref(); p = NULL; }`, not REF_PTR_RELEASE). Overloads' slot order is reversed, so a forwarder at +0x88 is the `const char *` overload of Remove_Sub_Objects_From_Bone. `appfile.py match` needs `-I wwlib -I ww3d2 -I wwmath -I wwdebug -I wwsaveload -I wwaudio` for an app/shared draft. The client's ProxyRObj Cast_* / Intersect_* call its own slot, not the proxied object's (recursing when one is set): exact as written, kept.
* **Sunk blocks and block-scoped slots (2026-11-06, TE1):** a `||` condition whose then-block returns gets sunk to the function's end; an inline bool helper (`Slider->Is_Held()`) kept it in place (0x6720A0). Stack slots are shared only by block-scoped locals: moving `float width, height` into the loop body fixed 0x6732F0's frame. A `char buffer[1028]` in Ghidra can be `char[1025]`. Ghidra's `param_2` used as a float means a local packed into the argument's slot (0x671CB0). A forward declaration of `std::basic_string` with its three parameters works in stredit.h, where `#include <string>` must not go.
* **Reference locals, line-sensitive headers (2026-11-06, TS1):** `mov eax,[pip]; add eax,0x2C` before an inlined std::string ctor with `mov eax,[eax]` after it is `RefCountPtr<T> &avatar = pip->Member;` declared before the string (0x64F630). To rebase an interim class in `sclient.h` on a later base, move it to its own header and include it on a blank line after the base: no line numbers shift.
* **Transform through Get_Transform (2026-11-06, RO1):** a render object read as `Transform` (no `mov edx,[esi]; call [edx+0x4C]` in the client) must not use Get_Transform(), which validates through slot +0x4C; ProxyRObj::Render does use it, so check each function. Set_Offset 0x47F860 is exact only as `Vector3 pos(offset); pos += Transform.Get_Translation();`.
* **An exact result can depend on the include list (2026-11-06, SR1):** a function exact under `appfile.py match` can be non-exact under verify: in 0x6A7C00 two reloads swapped because verify's include list has `SDK\directx8\include`, which appfile's automatic list omits. Check with objmatch on `.work/<agent>/tmp/enb_import/verify-app/X.obj`; a fast sweep under verify's real compile: `vc6.compile_sources` with `importer.tree_includes()` and `source_alias`, then objmatch (~3 s per form, `.work/sr1/vsweep3.py`). Include order was the lever (`assetmgr.h` first), and it is fragile: changing that file's includes can bring the swap back. Never wrap `importer.py` in `timeout`: it orphans a VM lease named importer-PID (release with `pool.py release --agent importer-PID`).
* **Doubled tests and folded constants (2026-11-06, TK1):** `RefCountPtr` members written `p = NULL` inline a check; the client's doubled test is `if (p) { p = NULL; }` (0x64B940). `(int)(x * 2 * 0.95f)` folds the constants; the client's `fadd st0,st0; fmul` comes from a float local first: `float w = x * 2; (int)(w * 0.95f)` (0x64BCA0). `Responses.clear()` against `erase(begin, end)` decides whether a struct's implicit operator= goes out of line (0x64CC90). Spend sensitivity is non-local: a no-code spend before a second append pushed the first append's _Eos out of line (0x64D400). `explain.py` on a draft with an assert prints "relocation: string differs" (the draft's own __FILE__) and hides the real class. `appfile.py match` for an app/client draft needs the draft inside app/client with auto -I, `--pool` and `-D ENABLE_WWPROFILE`; under .work it fails with C1083.
* **A queued `importer.py report` loads importer.py when it starts, not when it gets the lock (2026-11-06, SE1):** a file added to RECONSTRUCTED while it waits is left out with no warning, though imports.json is read fresh: rerun after editing importer.py. `.assign(p)` against `= p` moves an inline budget limit by one call: an out-of-line destructor at one return can tail-merge with the last return's copy (0x84B360). With no markers in a file, `appfile.py match --diff NAME` prints nothing; `objdiff.py OBJ MANGLED ADDR` works.
* **Arm order and unsigned tests (2026-11-06, CH2):** Ghidra's `if (x == 0)` can invert the source's arms: 0x6818C0 went from 0.40 to exact as `if (Split) {loop} else {single}`. `History.push_back(x)` gives the out-of-line insert(it, x) (0x682A20); `insert(end(), x)` inlines down to insert(end, 1, x). `test r,r; jbe` is an unsigned `x > 0`; `!= 0` gives `je` (0x682080). `mov eax,[ecx]; ret` at 0x680DD0 is RefCountPtr::operator-> out of line. `appfile.py match` overwrites .work/<id>/appfile/obj when another draft runs: copy the .obj before objdiff. Chain a tree writer and verify with `&&`: a failed writer leaves verify compiling new headers with old sources.
* **A combined batch can flip a function with no related source change (2026-11-06, LB1, 0x597080):** one new member declaration in a widely included header was enough for an order-only swap (D18, D28). Try swapping the compare's operands at the mismatch, and test the form across several header states with dummy declarations. `appfile.py match` can disagree with verify on such noise (its `-I auto` includes and missing `source_alias`): for a loss only verify shows, compile with verify's includes, defines, flags and alias (`.work/lb1/one.py`).
* **Allocator slots and later helpers (2026-11-06, IR1):** 0x68BF10 copies its allocator from the bool argument's own slot: `Unknown44("All", *(std::allocator<char> *)&arg)` reproduces it, a form that matches, not evidence of the source. A non-static helper defined later in the file (0x68FA30) keeps its out-of-line copy and still inlines into an earlier function with its FAIL_IF folded.
* **Landing many branches (2026-11-06, the coordinator):** two verifies per landing behind the lock took 20-30 minutes each with six builders running; one cherry-picked batch with one verify landed eight: `wt.py bring-over A B C --target run/<date>` (`tools/vc6/README.md`; `A..B` for a builder based on another's branch). Branches from one base conflict on importer.py's RECONSTRUCTED rows (both sides add: keep both) and on same-spot header additions. A batch can lose a function no builder lost alone (0x597080), so the landing check matters. D202: bring-over refuses an address lost outright; D66 moves and shared holders land, printed, and the merge still needs the maintainer's `/accept-d66`. A same-spot source conflict stays by hand: the batch stops with the file and commit named; finish the pick in `N.bo` and rerun with `--resume`. Read a landing's exit before removing its worktree: the branch survives, the worktree's drafts do not.
* **Helpers can trigger caller inlining (2026-11-06, GP1):** an early-return helper around a find can make a same-file function small enough that its callers inline it (0x5404E0 into three callers): check every marker after adding one.
* **Members that ignore ecx; overrides under two bases (2026-11-06, MF1):** a method whose client code ends in `ret N` and ignores ecx is still a member: making it `static` inlines it away and the marker reports "defined but not compiled". An override in a class with two bases cannot share a name with a slot of a different return type in the other base. A qualified `Base::operator =` gives the unchecked this-adjust where a cast adds a null test.
* **Find out of line, Get_Value inline (2026-11-06, TR1, 0x7C5690):** one codeless inline layer around a find plus 16-24 no-code finds gives the client's mix; spends alone push Get_Value out first (31 spends) and the find only at 32. Worth trying on ui_mfd's 0x5BDED0 / 0x5BE120.
* **Float forms (2026-11-06, TR1):** a client `fadd 4.0` between `floor`'s result and the next `floor` call is `float w = floor(a); float h = Max(b); w += 4.0f; h = floor(h) + 4.0f`. `y * -480.0f + 240.0f` in one expression folds to `240 - 480 * y` (fsubr): the client's fadd needs two statements.
* **A two-int copy that loads the high word first (2026-11-06, TR1):** with one destination load it came from `int *to = ...; int j0 = from[0]; int j1 = from[1]; to[0] = j0; to[1] = j1;`. A struct or `__int64` copy, or [1] first, does not give it.
* **A PermStringInitClass static opens an object (2026-11-06, TR1):** it does not end the previous one; two earlier notes had a boundary wrong by that amount. IR2 found the same at inv_render's end: check `__FILE__` before trusting a boundary.
* **Trivial accessors move the inline budget (2026-11-06, IR2):** 0x68EB10 went exact only with `Get_Filter()`; 0x68CCC0 went from 317 to 109 rows with Get_Slot, Get_Type and Get_Name. In long functions the early sites lose inlining, not the late ones. Not universal: accessors did not flip 0x68C6B0 or 0x68F510.
* **Client time locals; `__int64` members (2026-11-06, IR2):** `DurationClass now((unsigned)client->Unknown10BC)` explains a register holding 0 (the high half) and an unused stack slot beside the low half. An `__int64` in a struct aligns it to 8, shifting later members.
* **`#include "soundlib.h"` compiles under appfile.py but fails in verify (2026-11-06, IR2):** the whole file's pairs then show as lost. Use `..\audio\soundlib.h`, and read compile_errors first when a file loses everything.
* **FPU forms (2026-11-06, GR1, GR2):** a `(float)` cast stops VC6 moving a negation: `-(a * inv) * scale` with doubles becomes `-(a*inv*scale)`; `(float)(-(a * inv)) * scale` keeps the client's `fchs` before the `fmul`, with no spill (a `float inv` local does spill: keep it `double`). `if (c) X = K; else X = -K;` compiles to `mov [m], imm`; a named `float e` assigned in each arm, then `X = e`, gives the client's `fld K; fstp X` (a file-static `const` gives an integer copy). A `Vector3 pt` with `pt.X *= size; pt.Y *= size; pt.X += ...` gave a client sequence the combined expression did not.
* **New-expression temporaries (2026-11-06, GR1):** a `RefCountPtr<Derived>` passed to `new T(const RefCountPtr<Base> &)` makes the temporary with its conditional-destroy flag (`test bl,1`, `and ebx,~1`): the implicit conversion reproduces it. A factory's RefCountPtr<Derived> assigned to RefCountPtr<Base> goes through the template `operator=`, with a different register order from the same-type one (GD1, 0x674310).
* **Evenly spaced .rdata floats are not file statics (2026-11-06, GR1):** 0.0125f at 0xB0608C is shared with gutter_data's 0x7B5BF0: a literal merged across objects.
* **Frame, loop and base shapes (2026-11-06, GD1):** a frame 0x14 bytes bigger with a lone register copy can be a by-value `AABoxClass` of which only 2 floats are used (0x674310). objdiff shows `jl` against `jb` as a masked starred line, and a filter dropping `REL` lines hides it: a pointer loop gives `jb`, `for (int i...)` over a table reference `jl`. A static's place in .text says nothing about its place in the source. A cpp's local partial base can hide a smaller real class (real GadgetClass 0x30, ui_cpit's local one 0x50): `new` sizes need explicit padding in the subclasses when moving to the real header.
* **Destructor and static forms (2026-11-06, SD1):** `call dword ptr [vtable+slot]` in a destructor is a virtual call whose vptr VC6 has just stored; written directly it comes out as a direct `call`, so the client's form points to an inlined helper (0x6A4710). Unwind states tell a member's destructor from a cleanup method. An `atexit` stub that is just `ret` means the static's type has an empty destructor (a wrapper struct gives the bytes). `resize(n)` whose temporary lives to the function's end is `T blank; v.resize(n, blank)`. `<string>`'s `clear()` against `erase(begin, end)` changes what is inlined elsewhere in the function (0x814430). A member `operator+` and a free one schedule differently (0x6A6550).
* **Late declarations and arrays (2026-11-06, GR2):** a forward declaration on its own line in cpacket.cpp shifted every later `__LINE__` and lost 41 functions: use `class X *` inside the declaration. `IntersectionResultClass` declared before the block keeps VC6 from calling the array constructor; inside the block it emits `??_H`. An empty base destructor the client calls must be out of line (~ClientObjectDataClass, in co_data.cpp). Once a real class replaces a generated stub, its constructor's `new` of a stubbed class leaves no vtable: the stubbed class's start has to be rebuilt too.
* **`$E` flag initialisers (2026-11-06, RG1):** they are identical (`multiple`) or too short (`short`) in appfile, and 0x5EED20-0x5EF070 are mostly not known function starts, so they don't count as pairs: only 0x5EF080 scored.
* **Inlining and control flow (2026-11-06, RD1):** dead no-code inline calls inside a callee can tune its inlined copy in callers: seven to nine `Get_Unknown78()` in 0x7BB960 keep 0x70AC60 out of line in 0x7BB1D0, with 0x7BB960 staying exact, and ten or more break it (matches the bytes; not evidence of the source; spends in the caller did nothing). A `push_back` chain gives the out-of-line `insert(pos,1,x)` where a direct `insert(end,1,x)` is fully inlined. `std::swap(a,b)` stays a call where `a.swap(b)` inlines. A `mov edi,eax` with no movzx before `and ecx,2; cmp cl,2` is an int return and an int local, though Ghidra says byte. A node walked as `cur = node; node = cur->Next_Valid();` decides a whole function's register allocation (0x7B8210 went exact on that alone). A shared `sub esp; asserts; jmp` exit where the client repeats epilogues was the EH frame of an inlined vector insert, not the return statements.
* **Constructors and templates (2026-11-06, KV1):** an EH state around `new X(...)` with an out-of-line constructor means the constructor is in another TU (`#pragma auto_inline(off)` in the same TU gives no state): split the file. Vtable store before a member's call: `Value = f();` in the constructor body; a member initialiser stores after it and the base vtable store stays. A VC6 vector member's allocator byte sits first, so "a member at +0xC, vector data at +0x10" is one vector at +0xC, and a constructor "reading an extra bool" is the default allocator argument. VC6 gives C1001 on `template<> void C::M<T>(...);` and C2893 on an out-of-line member template. A byte-identical `?:` with swapped registers can be argument order (right to left): swap the constructor's parameters. `odd = false;` then passing `odd` differs from passing `false` (0x4B5010). SYMBOL_PAD reacts to removing declarations too (10 members out of delta.h broke pad 36). `appfile.py match` on a marked copy shifts `__LINE__`: put markers on existing blank lines.
* **Link stand-ins (2026-11-06, HF3):** gen_stubs skips `const int` data and by-value `std::string` returns, and ui_registration.h pulls in many UI headers: a new object can break clienttest's or the POC's link with those.
* **Statics, frames and appends (2026-11-07, SN1):** a file `static` vector the asserts read unqualified (`_AllSkills`) gives other registers in its users, a plain global loses the guarded atexit stubs; only a class static member gives both (7 functions across two files flipped): look for it in parked files whose statics are `_Xxx`. A frame 4 bytes too big from separate int temporaries (`push_back(0)`, `push_back(atoi(p))`): one inline helper used at both sites shares the slot. `trainer += x` against `trainer.append(x)` decides whether a later `+=` inlines; one dead `size()` before the last append fixed 0x815720. An assert one line low shows as a cast-text difference.
* **Bases, members and loops (2026-11-07, UM1, UP1, CM1, CS1):** a non-polymorphic base at +4 under a vptr shows as the base destructor called through `this ? this+4 : 0`; as a member it costs the ctor and dtor (0x5B8120, 0x5B8640). `while ((node = ...) != NULL)` where a `for` re-reading its head swapped registers (0x5B92D0). A bare `return a == b;` gives `xor eax,eax; cmp; sete al`; the client's form without the xor is `?:` or an if. `char pad[n]` after a bool packs at alignment 1: use int arrays. An override with another parameter list leaves the class abstract (C2259): verify drops the file and pairs_cmp shows mass "lost outright". A local pointer copy stops tail-merging identical `*out = 0; return;` arms. The inline budget grows with the caller's own code: dead statements at a function's end let earlier call sites inline (0x799E40). `Matrix3D(-v)` copies the translation through the FPU; `Matrix3D m(true); m.Set_Translation(-v);` gives the integer copy.
* **Galaxy map forms (2026-11-07, GM3, GM4):** an iterator constructed in place (not assigned later) stays in memory as the client's (0x7A6AB0). `ret 8` with no eax is an out-parameter function. A `??_G` with flag 0 inside an inlined `clear()` is a one-member struct element with an implicit destructor. A static helper called directly sits right after its caller. A bitfield read-modify-write before the vptr stores is a member struct with its own constructor. A `Vector2 uv[4]` array gives an `and esp,-8` ebp frame; four locals give the esp frame. `Vector3 step = delta; step /= (float)count;` keeps X in memory. Plain copies (`float a = x; float b = x;`) give integer moves, chained assignment `fst`. The `<string>` initialiser at an object's end is not always a verified start: scan raw bytes.
* **INI and accel forms (2026-11-07, RS1, KV2):** `INIClass::Is_Present(name)` compiles to setne/test, `Find_Section` / `Section_Present` to a plain test. A `std::queue` member (not a bare deque) leaves two of push_back's three `_Construct` calls out of line. Codeless `Get_Current()` reads kept Update out of line in its small file's caller (0x498E00).
* **A pair held by header text (2026-11-07, RS1):** nav_list_builder's 0x7B7EF0 is exact only with the exact text of the headers nav_list_builder.cpp includes (client.h, sclient.h -> ui_mesh.h); any change there swaps two stack slots, and its two vector destructors (0x7B8ED0, 0x7B8F10) then lose their caller among 174 identical copies. A branch that edits those headers loses all three: under D203 that loss is accepted (name it in the commit, record them in attempts.csv); don't work around it.
* **Real set-ups in the POC and clienttest (2026-11-08 night run, PD1, CW1):** when a stand-in's class goes real (pdaint.cpp's PDAInterfaceClass), its set-ups run for real and create every window they own: each class the tree lacks needs a POC stand-in (a generated constructor leaves no vtable and the POC crashes; PD1's poc_world3.cpp / poc_world4.cpp), and clienttest's fake objects must cover every function that goes from stub to real. clienttest links ui_chat_panel.obj and sclient.obj, so a real function there that clienttest stubs for its trace gives LNK2005 (a `standins: keep` marker does not help; linkgate catches it). build.py compiles committed files only: a POC built before a new .cpp is committed gets the generated stub of its constructor (no vtable) and crashes (PD2: in PDAInterfaceClass::Unknown_54); commit first, or check build/app/obj. Removing a local class block from an app .cpp shifts every later assert's __LINE__ (radar_system.cpp lost 10 pairs): keep its lines as blank or comment lines. gd_text_editor.cpp's FAIL_IF(!Slider) on line 361 depends on the D25 blank lines at 350-357: any line added or removed before it needs a 1:1 adjustment.
* **A matched constructor ties the vtable (2026-11-08 night run, SB1):** once a class's constructor is exact, D188 ties its vtable to the client's, and every client override the class doesn't declare gives the base's inline virtual two addresses: it is lost in every file (InterfaceClass's 0x515690 / 0x5156D0 / 0x515710 / 0x515730 for UIStarbaseClass): declare all of the client's overrides from its table first. The inline budget is per function and can be steered from later code (two inline accessors used after the init list kept the lists' begin() out of line); a run of constant member stores can need a struct with its own constructor to land after the EH-state store. A gen-stubbed constructor made real pulls its object into the POC and clienttest links: hand stand-ins for its by-value-return callees.
* **Create_Peek on an uninstantiated type (2026-11-08 night run, RH1):** `Create_Peek(T *)` resolves to the `RefCountClass *` overload (C2440 in ref_ptr.h(249)) until `RefCountPtr<T>` has been instantiated in the object; a named `RefCountPtr<T>` local fixes it (and raised enbscene 0x75F8B0 0.72 -> 0.78).
* **sclient.cpp declares its own `class UIChatPanelClass` / ChatPanelInterface (2026-11-08b, CW4):** a new call from sclient.cpp to a panel method needs the method declared there (else C2039 and verify drops all of sclient.cpp: a mass 'lost'); add it on an existing line so FAIL_IF's line numbers stay.
* **Removing a NOT EXACT marker line shifts `__LINE__` (2026-11-08b, VB1, VB3, OM1):** the asserts below it, the function just made exact included. Put a blank or `// 0xADDR` line in its place; for any retype or splice in a file with asserts, `git diff --numstat` must be net 0 per file (OM1's first retype try, +4 lines, lost 10 functions).
* **delta_array.h reaches aux_roots.h (2026-11-09 day run, MT1):** a declaration added to delta_array.h (Get_Value for inv_data_peek) cost aux_roots 0x702240 / 0x7045F0 and sclient 0x74D990 / 0x758210; money.h's pattern fixes it: an `#ifdef __DELTA_ARRAY_MEMBERS` guard only its users define.
* **sclient's 0x74D990 is pad-sensitive (2026-11-08c, C2 / W2):** it is defined after `#include "aux_roots.h"` and its pair moves with the declarations sclient.cpp sees, like aux_roots' padded constructors: an extra header at sclient.cpp's top, UnknownCOClass leaving aux_roots.h, or FontSys's implicit destructor each lost it. Check it with any change to aux_roots.h, co.h, fontmanager.h or sclient.cpp's includes. Since D206 aux_roots' window was one pad wide (52); SC1's placement moved SYMBOL_PAD to 7 with a SEND_SYMBOL_PAD of 35 (2026-11-09 SC2: SYMBOL_PAD 58, window 57..59; SEND_SYMBOL_PAD 40).
* **New app files this run are provisional (2026-11-09 night run, D137):** unknown_7cef30.cpp (the subs-interface account client, RegistrationUnknown1ACClass), unknown_7dfed0.cpp (the registration reader over MSXML), unknown_7e1e40.cpp (the reply download), ui_log.cpp, ui_guild.cpp, ui_jobs.h (UIJobsClass over Unknown5BB320Class, out of sbaseview.h), client_item_property_builder.h: their names and class homes are interim. Stand-ins for by-value `std::string` returns are skipped by gen_stubs (XR1: hand-write them in `stubs/app_stubs.cpp`, delete them when the real code lands, as XR2 did).


### 3.4 Importer, verify and pairs

* **A header static's `_$E` copy is no placement evidence** (NM1, 2026-11-07): the guard-0xC416FC initialiser has 533 copies in
  the app; one found inside a file's window is often the neighbour's. The client's CRT initialiser table (.CRT$XCU, 0xB6427C-0xB6E59C)
  lists them in link order: check it before trusting a range credit (P-IR3-1).
* **New declarations in a shared header can shift aux_roots' SYMBOL_PAD** (AF1: MoneyClass's members lost 8; WE8: DeltaClass::Is_Valid
  lost 24): put members only one file needs behind a guard macro defined in that file, then `auxpad.py sweep`.
* **An npos use added to a file can make its other near misses exact** (WE6, lego_identifier): re-verify a file's NOT EXACT functions
  after adding code to it. A comment edited after verify can shift a `__LINE__` assert (WE6's lost 0x8444C0): grep FAIL_IF lines first.
* **A shared `<string>` initialiser left unpaired is a range question, not a missing include (2026-11-05 evening, SC4, D196):** every `<string>` includer has a byte-identical guarded initialiser of `std::ctype<unsigned short>::id` (guard 0xC416FC, atexit 0x42B320): check the .obj for its `_$E` before editing includes. `importer.py verify --lib a,b` silently verifies nothing: repeat `--lib` per library. A bare `verify` without `--pool` while another agent holds the importer lock falls through to stale hosts (ssh timeouts, nothing done): use `--pool K`.
* **The verify service judges a PR with master's tools and master's accepted-corrections.json (2026-11-05 evening, PR #163):** a PR's own objmatch change or corrections entry does not count for it, an outright loss passes no command and the ruleset has no bypass. A false pair a PR exposes needs its corrections entry merged first (PR #164); a coordinator's in-run `--accept` is not that ruling.
* **Landing builders' new files (2026-11-04 night run):** registering a new file in importer.py (`RECONSTRUCTED`)
  can add pairs, so re-verify and commit docs/data/imports.json with it (bring-over refuses a branch whose record
  differs from its verify). bring-over refuses a branch with merge commits: `wt.py rebase ID --onto run/<date>` rebases it (the merge
  dropped). appfile.py's definition regex reads `static void (Name)(` as a function named `void` (harmless; verify is right).

* **Other builders' verifies replace `%TEMP%\enb_import\verify-app`** (2026-10-31, TK12): a new file's .obj can vanish
  minutes after your verify; objdiff the appfile.py match object (`.work/<id>/appfile/obj/`). attempts.py `--blocker`
  has no class for "identical copies waiting on a tie-break": policy.cpp's 0x4AFAD0 precedent uses `relocation`.
* **Caller tie-break (D166, 2026-10-31, TB1):** an identical-copies function (D54's ambiguous column) becomes a pair when
  our callers reach one of its addresses and every client call or jump to that address sits in a function the same
  file is exact at (nothing may hold the address as data; the linker's merged copy with callers everywhere stays
  ambiguous). The pair is in `in_tree` and the file's `tiebreak` list. It exists only in verify/`compile_and_match`
  (`match_objects(callers=True)`), not in `appfile.py match` or `objmatch.py`: a draft check can miss it, and losing
  its caller's pair loses it too (`pairs_cmp.py` names that D66 (vii)). Anything that rewrites a file's `in_tree` without verify
  can leave `tiebreak` stale: verify afterwards. pairs_cmp's "with its caller" is approximate (another pair of
  the same file lost too; it sees no call edges). Readers of pairs_cmp's output parse one labelled list per line
  (`gained tie-break` and the `lost tie-break` lines follow `gained` / `lost`): pacing.py's two-line regex broke.
* **D174's caller tie-break extensions** (2026-11-01, TB1; `objmatch.caller_tiebreak`): the copies need not be
  byte-identical (they may differ in a relocation target, e.g. an EH handler address; the callers decide) and the
  candidates are D80's survivors; a call from an EH unwind funclet (client 0xAA8000-0xACD000) counts as a call by every
  parent whose FuncInfo unwind map lists it (`funclet_parents`: a parent not exact keeps the callee tied); several
  symbols of one file with the same bytes and relocations proposing one address (the linker folded them) credit the
  first by name (`folded`; since D194, when all the proposers are COMDATs with one `body_sig`, in any files, every
  one is credited, as D193's folded short pairs); and the caller tie-break's credits feed D80 / self name / range / initialiser, which run
  again, then the caller tie-break again, until neither credits. A late credit can therefore change what an early
  phase resolves: check every `lost` against the client's layout.
* **A member added to a locally declared base shifts every derived class's padded fields** (2026-10-30, MU1): in a
  draft it silently broke 10 exact functions; re-match the whole file after any base change.
* **Two builders' adjacent RECONSTRUCTED rows in importer.py form one hunk** (2026-10-30, MU1 / CP1): commit_mine took
  the other's rows before its files existed. Any `-U0` hunk that holds another builder's adjacent insertion goes with yours. Add your rows and commit them at once, or keep them apart.
* **commit_mine `--file X` takes the working copy's imports.json entry as it is when the commit runs**
  (2026-10-29, PX3): another builder's verify can rewrite it between your verify and your commit (c50da63 lost its
  entry; 928fc7f restored it). After every commit check `git show HEAD:docs/data/imports.json` for your addresses.
  `build.py` skips untracked files: `git add` a new app file before `build.py build --allow-dirty`.
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
  `--refresh-totals`, or the count needs a follow-up commit (a74439b).
  Without `--file NAME` at all it leaves the file's whole imports.json entry out, silently (LD7's 55f1d2c5,
  repaired in 361a0ec0). `--refresh-totals` copies `in_tree_total` from the working imports.json, another builder's
  uncommitted gains included (LD8's 8d47d1ed recorded 4,791; SM1's 3271dea7 accounted for it).
* **Header origins live in `imports.json`'s `headers` map** (`headers_from`: swapped with the
  file). A header change must update `headers` (history: the import's copy swaps set both).
* **A function taken from another EA copy can undo an E&B change through a twin:** ZH's
  `ShdBumpSpecDefClass::Init` once replaced E&B's DX6-only one (it matches Bump Diffuse's 0xA0B9B0).
  Check each new pair's address belongs to the file (`client.py name`); else revert the file and
  its `imports.json` entry.
* **A twin is not a match.** `new`-and-construct functions (`MeshClass::Clone` 0x93ED00,
  `ShdMeshClass::Clone`) match across libraries: a "loss" in another library's region may still be
  matched in its own (`imports.json`).
* **What verify can't count.** Overloads differing only in callees become one pattern at both
  addresses (`WW3D::Set_Render_Device` 0x917790 / 0x9177E0), not identical copies for D54 either:
  `objmatch.py OBJ` finding both is the evidence, 0 gain. Small generated inline virtuals and
  too-short functions never count: show them with `objdiff.py` / `appfile.py match` in the commit.
* **COMDAT duplicates (D20):** a dropped `(file, 0x6CF410/0x909500/...)` pair matched elsewhere is
  no loss; name it in the commit.
* **`verify` keeps only the first 50 compile errors:** a library losing functions with no error for
  that file means a loud file (a whole-file swap) hid them. Fix it first.
* **A "none" for a callee proves nothing** (targets are masked): follow the client's call first
  (ZH's `Peek_Thumbnail_Instance_From_Any_Manager`, 0x91AD40).
* **Low evidence can be the copy's own headers:** ZH's `mapper.cpp` matched 2 in `.work/bm2`, +56
  in the tree. Read the client's vtable (`vtables.py --seed`) before "X without Y".
* **Library flags (D12).** wwshade `/GX /Ob1`, the other EA libraries `/Ob2` without `/GX`
  (library-matching.md §3.5), app `/GX` (D23). Signs: EH prologs (`push -1; push handler; mov eax,
  fs:[0]`) only in the client, or a file's callers all calling what ours inlines. Compile with
  `importer.compile_and_match`, not `vc6.py` defaults; give new flags to `bytematch_all.sh`.

* **`appfile.py match` on a draft:** A draft placed next to its
  original (so relative includes resolve) leaves a `<dir>/obj/` folder: delete it before committing.

* **Compare against HEAD's imports.json, not a saved working copy:** another builder's verify rewrites the
  working copy with your uncommitted tree already in it (`git show HEAD:docs/data/imports.json` is the before-copy;
  2026-10-26, L4, F3). Compare over the whole app: a template header visible to other files moves their pairs.
* **commit_mine.py hunks (2026-10-26/27):** keys must cover every `-U0` hunk of yours, and a key matches an
  identical hunk anywhere in the file (205afc9 missed two; P4 found another builder's identical line taken):
  after the commit `git diff docs/data/imports.json` must no longer show your addresses.
* **VC6 hits C1001 on out-of-class specialisations of member templates and mangles member-template functions
  without their template arguments** (P7): a stand-in defines them as plain overloads in its own minimal class
  declaration (tests/udpreplay/shipcust_stubs.cpp). `appfile.py match --diff` works only on a marked function.
* **Including a packet header mid-file in tsk_login.cpp turns a handler order-only** (P9: 0x76B520 with 114's header):
  keep the one-line views with the real signatures (P-P9-3). A flag parameter the client stores with `movzx` is
  `bool`; a cast temporary for a count leaves the frame 4 bytes short: name the local (118, 112). P6's commit_json
  method appends new entries at the end of imports.json's sorted maps (a duplicate key after another repair,
  326ca2d): `.work/p9/commit_wt.py` re-sorts and refuses until HEAD parses; checks.sh is still to promote (wt.py covers
  the setup). pkclasses.py can write a handler stand-in twice into pk_ctor_stubs.cpp
  (the OVERRIDES section; fixed e4bec18). Same-shape template instances stay `multiple` until their relocation
  targets have names in symbols.json.
* **A pack(4) member changes a packet class's sizeof (0x5C for 0x60) and the factory switch 0x869790 in pk.cpp stops
  matching** (P6, 2026-10-27): check it whenever a packet layout changes. Put a needed `#include` after the asserted
  code so the asserts keep the client's lines.
* **Packet stand-ins belong in tests/udpreplay/, never stubs/app_stubs.cpp** (P5, 2026-10-27): a stub that
  calls RawPacketClass drags pk.obj and ~280 externals into VIDTEST's link (969f244, fixed 9ba36c5). A new header line
  above pk.h's PacketClass shifts its assert lines and loses ~400 pairs. Game-editor packets whose Process_Packet
  returns true are not in the switch, so pk_classes.h does not list them: find them by unmatched objects.
* **Between a worktree merge and commit_mine another builder's verify can rewrite imports.json** (569503f, e4896de):
  check HEAD's in_tree for your files after every commit.
* **Draft `obj/` folders left under app/** are not compiled but end up in a careless `git add`: delete them.
* **D66(i) in practice:** a rewrite that stops emitting an inline COMDAT copy drops that (file, address) pair while
  the address stays matched elsewhere (land's 0x447C40 / 0x47F300 / 0x7F0EC0, 01ac8b7): name them in the commit.

* **Template bodies in a header that tsk_login.h includes broke kerneltest** (T5).
* **Worktree verifies:** `wt.py new` / `wt.py bring-over` (f584ed0).

* **clienttest builds only committed files** (T11): in a worktree, `git add` a new file before `build.py`. A
  `RefCountPtr<T>` member in a header breaks every file that builds the class without T complete (clienttest):
  keep it a raw pointer until the constructor is rebuilt. Levers that matched co.cpp (`if (!Is_X()) return x;`,
  the one-case switch, `Get_Float` / `Set_Floats` / `SphereClass::Init`) are in co.cpp's notes.
* **Never split attempts.csv on line endings** (T12 truncated 157 rows): its rows hold quoted multi-line text; use
  `attempts.py` or the csv module. `build.py --lib` refuses a dirty library: commit a detached WIP inside the
  worktree before `build.py` / `clienttest.py build`. A header `__FILE__` the client keeps relative
  (`..\shared/auxdata/auxdata.h`) defeated D80's tie-break until b4c8a64 read the client's string to its NUL.
* **A zero-context imports.json patch applied after master moved lands lines in another block** (H18's e0ad711,
  repaired in ebad207): a commit script must refuse unless HEAD equals the verify base; `commit_mine.py --file NAME`
  (9263c0c) selects by block span instead. In Git Bash prefix a command taking `// 0x...` arguments with `MSYS_NO_PATHCONV=1`; in VC6 `#line N`
  alone switches `__FILE__` to the full path and `#line N "..\\shared/..."` needs escaped backslashes (T15).
* **A verify worktree has no built app.lib** (P8): run `build.py build --lib app --allow-dirty` there (the worktree is yours alone) before `udpreplay build`.
* sgistl's `insert_unique_noresize` compiles in VC6 since f918e190 (sgistl_adapt.py drops `typename` from the
  `pair<typename hashtable<...>::...>` out-of-class return types; it failed with C2899 before, P-H17-1, P-H7-1):
  `hash_map::insert` is usable now. Keep the existing explicit specializations of it at the top of attrib_dir.cpp /
  ini_manager.cpp: removing them, or an explicit instantiation in their place, changes VC6's code for the table's
  other members and loses 13 pairs (QT1, 2026-10-30). A file's canary function
  (client_lego_impl: the bone map constructor 0x72D5F0) flips with the file's contents: check it each verify.
* A tryv2 PermissionError (a concurrent tree reader) leaves a variant in the tree
  file: regenerate it. Verifies made while other builders' files fail to compile add COMDAT instances to your
  file's ambiguous list.
* **A generated docs/data file committed whole by another builder brings back rows you removed** (T10's c9f7c26 to
  not-exact.md; H15): after the others commit, check HEAD's generated files still hold your change. Record the second
  of two `attempts.py record` rows only after the first commit, or name the carrying commit.
* A multi-line
  `// NOT EXACT:` comment above asserts shifts `__LINE__` (10 tsk_login pairs lost): keep it to one line.
* Each builder passes `--agent NAME` (or works in a wt.py worktree): scratch is private (f8a697c).
* **`attempts.py record` has no `--lever` (2026-10-27, F7):** put the lever in the evidence text; `attempts.py check --lever` takes it.
* **tryv2.py** writes the tree file back with `open(..., 'w')` and can fail with
  OSError 22 when another process holds the file: the tree stays intact but later variants are skipped silently, so
  check the log's variant lines. It uses vc6_hosts.json's default host, which can time out over ssh (172.17.2.20,
  2026-10-31, TG1): lease a clone (`pool.py lease --agent X --count 1`) and export the ENB_VC6_HOSTS line it prints.
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
  `source: exact`, deleting the evidence that made them exact (-323 pairs); it now keeps the proof (`proof`, 0253f43).
  Regenerate only with a full verify of every library. A tool fed by symbols.json must not lose its evidence when
  symbols.json records that tool's own result (2026-10-31, AR1: a regeneration lost 268 outright): the proof was kept
  only for the very same symbol, so a vtable slot's `??_E` dropped when the byte-identical `??_G` became exact (244;
  `same()` now treats them as one), and D115 stopped registering an initialiser target it already knew (7; known
  targets now register too). Before regenerating, check every tie-break that reads symbols.json for this. An object's client range never comes from imports.json's
  `in_tree` (it holds COMDAT copies elsewhere): use the object's own non-COMDAT / NODUPLICATES exact functions (M1).
  `build.py build --lib app --allow-dirty` compiles tracked files only: `git add` a new .cpp first (M1). Park scripts go
  stale when part of the work lands separately: diff the park folder against HEAD before using it (M1).
* **A symbols.json regeneration feeds a run's late-phase results into its early phases** (2026-10-31, SY1, D173): with
  the run's own names in symbols.json, D80's `known` completes a vtable chain at once, so D171's self name settles a
  `??_G` before D100's range round does, and a pair can move (0x723AB0 -> 0x4B2AA0, a false range pair). Not only lost
  proofs (AR1): check every `lost outright` against the client's vtable layout (slot count, slot 0's thunk) before
  calling it a loss. symbols.py without `--objs` reads no objects: copy every library's verify-* folder and pass each.
* **Shared tree, 2026-11-01 night run:** a builder's uncommitted header is compiled into the others' verifies and recorded as their gains (f343bc6a recorded ~CordMgr before NW6's arraylist.h landed; HEAD was inconsistent ~10 minutes). A HEAD that briefly does not compile shows as many losses in a worktree verify: read `compile_errors` before pairs_cmp (NW6). With `ENB_AGENT` set, verify writes its objects under `.work/<agent>/tmp/enb_import/verify-<lib>`; `%TEMP%\enb_import` may hold another builder's stale objects (NW4). A file's dynamic initialisers (`$E` symbols, <string>'s guard, PermStringInit) are not counted by verify: D99 rows (NW5). Tools' function starts lack file-local statics stored without a thunk (appfile reported 0x77A5B0 PREFIX, its size running on to 0x77A710; verify still counted it; HV1).
* **Matching tools (2026-11-01 day run):** appfile.py sees a `// @0x` marker only within 5 lines above the definition, with only blank or `//` lines between (a `/* */` block or a `#pragma` hides it or is read as the name; NW, FNT1, HV); appfile.py match resolves a header's relative includes from the tree (edit the tree header; MC); drafts including wwstring.h need `-I auto` (INP); tryvariants.py can't take an app file in a subdirectory (SORT1); a variant at another line differs only in its FAIL_IF / G_ASSERT `push <line>` (FNT1). `attempts.py check` flags any address named in a comment; `record --forms` takes a count, text in `--evidence` (NW, SORT1). commit_mine's whole-file docs/library-matching.md swept another builder's row (c70265fb): use a hunk key (UI). **Byte-exact matching doesn't check which global a relocation names**: combo_ffactory matched while asking the wrong factory and the POC recursed (LINK): run the program.
* **Pairs (2026-11-01 day run, late reports):** a new file that instantiates another file's inline COMDATs makes them "held by two objects", and D114(b) then stops widening the other object's range: its range-credited COMDATs near the boundary are lost outright. Name them in tools/re/symbols_named.json from structural evidence (the unwind funclets' `add ecx, member; jmp`), then `symbols.py --apply-named` (IS; P-IS-1). appfile.py match needs `ENB_AGENT=<you>` (it writes to the shared .work/appfile/ otherwise) and, for client files, `-I app/client -I libs/sgistl -I wwlib -I wwmath -I ww3d2 -I wwdebug -I wwsaveload -I wwaudio -I app/shared`; it says PREFIX when the client's size includes int3 padding (verify still counts it). mark.py's NAME=ADDR can't name `operator=` (BTN). The Edit tool can add or drop a trailing blank line: grep the FAIL_IF / G_ASSERT lines against HEAD after a line-keeping edit (LS: -6 pairs); with a pinned assert line (D25) pay for each added function with padding lines in the same script (FS). A friend `Create_Peek(T*)` inside RefCountPtr<T> resolves only once RefCountPtr<T> is instantiated: declare the named local first (IS).
* **Shared COMDATs (2026-11-01 evening run, IS1):** crediting a COMDAT two objects hold to every holder's range lets the later object's range reach over the first object's code (the linker keeps the copy with the object it meets first); the two objects' tied template instances then propose one address and both lose it. Widen only the nearest holder (P-IS-2). `importer.py verify --pool K` exits when the host has under 4 GB free beyond the VMs it would start: use `--pool` = the VMs already running. `wt.py bring-over NAME` lands on local master by default: in a run, pass `--target run/<date>` (the coordinator, 2026-11-01).
* **Variants (2026-11-01 evening run, D184 pilot):** a variant that changes a function's line count moves every later `__LINE__` (G_ASSERT / FAIL_IF) in the file: it can look exact and lose later functions (Set_Debug_Mode: -4); `variants.py try` pads or trims each variant to the original line count, and a function can go exact by its assert's line alone (0x7381C0). `try` compares one object's exact set by address: a filter, never verify or pairs_cmp.
* **Shared work in one tree (2026-11-02 run):** commit_mine with a shared header in "files" takes names-review rows from the working copy (other builders' line shifts): leave the header's rows out and regenerate before the PR; a hunk key equal to a file's stem also matches another builder's importer entry that names that file: use the full key `('app', 'client/x.`. Replacing a stand-in block by a shorter `#include` shifts every later FAIL_IF / G_ASSERT `__LINE__` (FM: 6 ui_login functions lost their pair until padded). `explain.py --n 2` tracebacks with one match.

- **A failed vc6-verify: check where and how often it ran before blaming the PR** (2026-11-03, PR #147). verify_pr.py
  judges with its own checkout's tools and accepted-corrections.json, so it now refuses off origin/master (PR #149);
  run it from `.work/wt/verifier`. Two verifies of one PR at once share `.work/wt/prN.vf`: the second drops the first's
  worktree mid-build and both fail. Low host memory (MSBuild's `.NET Host` workers: `dotnet build-server shutdown`)
  shows as compile errors.
* **Exact at a caller can lose a folded short pair (2026-11-05 night run, CS1, 0x559350):** making a function exact can lose a D188 short pair at a linker-folded address it calls: `objmatch.short_known` drops an address two symbols claim (an inline avatar.h Get_Desc made 0x559350 / 0x559490 exact but lost ui_avatar_cust's 0x551D20, a folded RefCountPtr Peek / operator->). Check `lost outright` for short pairs before keeping such a change. Since D193 (CS4, 5697f45d) the claimants are all credited when they are COMDAT bodies with the same bytes and relocations (Get_Desc then landed with 0x551D20 kept); different bodies still drop.

* **A duplicated guarded initialiser flips with the file's range (2026-11-05 night run, ST1, ui_starbase):** when the client's object holds two byte-identical copies of one template static's guarded initialiser (`std::ctype<unsigned short>::id`, 0x646120 / 0x648380) and ours compiles one, D100 credits it to whichever copy the file's own range reaches first; moving the range (a new function lower in the file) shows the other as lost outright. Not a code loss: the ruling is the user's (D197 accepted 0x648380 -> 0x646120; D196's kind).
* **Pairs cannot see wrong initialised data (2026-11-06, CT1, ui_class 0xB7DC8C / 0xB7DC98):** a table reached through a relocation can be wrong while every function using it stays exact; both tables were, and Next from Enforcer landed on Scout. Compare hand-copied tables with the client's bytes.
* **`build.py link --program clientpoc` does not rebuild app.lib (2026-11-06, CT1):** the exe gets a fresh date but holds old code. Run `build.py --lib app` first and check the changed bytes in the exe.
* **Read a template's value type from Ghidra's mangled name (2026-11-06, CT1, chartemplate):** `...I_` is unsigned int; `hash_map<string, int>` lost 0x502A90 / 0x503030 by falling on unique_design_id_lookup's copies.
* **asserts.txt can misattribute at an object's edge (2026-11-06, CT1):** 0xBA8FE8 / 0xBA9018, listed under chartemplate.cpp, are chat_defs.cpp's (0x830770's `__FILE__`).
* **A renamed function can pass a harness build silently (2026-11-06, RN1, D198):** `build.py --lib app` refuses a dirty tree unless `--allow-dirty`, leaving app.lib stale; clienttest then generates a stand-in for the new name ("gen Message_Error") and shows only an expected.txt mismatch. Stale `build/app/obj/<old file>.obj` can also link in after a file rename: delete them.
* **Harnesses need `ENB_VC6_HOSTS` (2026-11-06, RN1):** set it from the line `pool.py lease --agent ID --count N` prints.
* **EA data work (2026-11-06, EA1):** the Castanet manifest's 16-byte hash is MD5 with each 8-byte half reversed; key it on full paths (basenames lose rows); size equality misled for 8 files. RTPatch's path search patched `common\blood.ini` at the demo's `client\ini` copy. 7z's case-insensitive lookup collides MSS32.DLL and mss32.dll. The project's client.exe carries Net-7's URL edits in `.data` (P-EA1-2); `.work/eadata/ea2004/` has EA's own.
* **A base's implicit destructor costs another file's pairs (2026-11-05 night run, PT2):** deriving from Unknown_674260Class (no declared dtor) made ui_ptrade.obj emit copies of its dtor and deleting dtor, and unknown_675120's 0x675020 / 0x675150 were lost outright (credited to 0x674230 / 0x674F60). Declare the base dtor where the client calls it out of line (0x6742F0). gen_stubs skips functions returning a class by value (MoneyClass operator+): hand-stub them for clienttest / clientpoc.
* **A deleting dtor matched to a twin class (2026-11-05 night run, IE1):** while a vtable-emitting ctor / dtor is not yet exact, an identical deleting destructor can pair "exact" with another class's twin (the attributes panel's ??_G took the row's 0x533A90 instead of 0x532FD0); it settles once the ctor is exact. Don't commit the vtable's table before then. appfile.py's "PREFIX ... the client's function is N bytes" can be only int3 padding after the client's ret (0x4C7AE0, EX3): check the disassembly; verify counts it exact.
* **D188 shorts and tie-breaks moved by retypes and new instances (2026-11-08 night run, RH1, CW1):** retyping a base virtual whose body is empty loses its D188 short pairs everywhere: objmatch's short_known skips a symbol when symbols.json names another one at the address; add `replaces` rows to `tools/re/symbols_named.json` and run `symbols.py --apply-named`. A new template instance in an object (MemberCallback ??_G copies) can propose an address in objmatch's range round and take another function's tie-break (0x562410 lost; P-CW1-1). pairs_cmp's "lost shared holder" line can be very long: summarise it, don't tail it. A verify queued behind the importer lock compiles the tree as it is when it starts, not when it was launched (CW2). A new app file isn't compiled by verify.py until it is `git add -N`'d and has a ledger shard (`ledger.py describe`): RH3's first two verifies built nothing new. `appfile.py match` applies `// @ADDR` markers in order among functions with the same demangled name (mark every overloaded constructor); its PREFIX verdict can be the client's inter-object int3 padding counted in app.size. In a worktree, verify.py compiles into `.work/<worktree>/tmp/enb_import/`, not `%TEMP%\enb_import\`.
* **A variant's gain can still lose outright in the full verify (2026-11-08b, VB1):** `variants.py try` judged 0x5913E0's v6 gain-only; the full app verify lost 0x592350 outright. Every applied variant goes through verify / pairs_cmp, never on try's word.
* **objmatch's range round and byte-identical ??_G copies (2026-11-08b, OM1):** a class's ??_G and a template callback's ??_G can be byte-identical with the call masked; when the class's destructor lives in another file there is no name or range evidence, only the callee's own bytes (D213's refutation) tell them apart.
* **The importer lock is global and not first-come-first-served (2026-11-08c):** every worktree's verify, report.py, auxpad sweep and bring-over takes the same lock; with 6-9 builders a verify waited 20-50 minutes and the default 600 s wait gave up. Pass `--lock-wait 6000` to verify and to `wt.py bring-over`, set ENB_LOCK_WAIT for auxpad.py (no `--pool`; it leases itself and fails at once on low memory), iterate with the lock-free `appfile.py match FILE --pool`, and run the full verify once per batch of gains. agy builders' 75-minute timeout expired in the queue three times (G3, G4, G5).
* **An accepted loss must be on the target branch (2026-11-08c):** `wt.py bring-over` reads accepted-corrections.json at the target's tip, so cherry-pick the correction onto the run branch before the landing, as well as opening its PR to master (verify_pr reads master's).
* **verify-<lib> scratch is shared (2026-11-08c, K1):** `%TEMP%\enb_import\verify-<lib>` is written by every worktree's verify: copy your objects out at once before vtables.py or objdiff. A symbols_named.json rename alone can change pairs (K2: sclient 0x706FF0): rerun the verify before keeping such a gain (§3.0).
* **sclient's SEND_SYMBOL_PAD moves with its headers (2026-11-08c evening, PH1):** after SC1 sclient.cpp's padded
constructors (0x758210, 0x702240) and 0x74D990 hang on aux_roots_send.h's SEND_SYMBOL_PAD, whose window shifts with any
header sclient.cpp includes (sbaseview.h, fontmanager.h moved it 35 -> 40 -> 51); aux_roots.cpp's SYMBOL_PAD window was
the single value 16 (2026-11-09 SC2: 58, window 57..59; SEND_SYMBOL_PAD 40). Re-sweep after every landing that touches such a header (PH1's lock-free sweep: ~3 minutes); the
pad cycles mod 64. Single-file compiles of sclient / aux_roots show a constant set of false losses: compare pads with
each other and decide with the full verify. Builders branched before such a landing must rebase and re-sweep.
* **Pad windows: appfile match is not the importer path (2026-11-09 night run, SC2, TM1):** `appfile.py match` and `importer.compile_and_match` (verify) pad sclient.cpp / aux_roots.cpp through different windows, so a SEND pad that matches in appfile can lose 0x74D990 / 0x758210 / 0x702240 in verify. Sweep through `importer.compile_and_match` under the lock (SC2's `.work/sc2/sendpad_imp.py`), with `ENB_LOCK_WAIT=6000` for the sweep; a definition-order change moves one window. Current values: SYMBOL_PAD 58 (window 57..59), SEND_SYMBOL_PAD 40 (the only value keeping 0x74D990 / 0x758210 / 0x702240); `tools/re/auxroots.py`'s DEF_ORDER is a build aid (the PrimaryCategory constructor 0x6FB1E0 is defined last); the pad comment in auxroots.py is stale. A header member added for one caller in a header sclient.cpp includes moves the pad window (TM1: option.h's accessor; fixed by reading +0x78 through a local inline in ui_log; IP2: a client.h inline accessor moved 0x74D990 too, so keep it file-local).
* **Importer and stubs, 2026-11-09 night run:** a SYMBOL_PAD / SEND pad change needs a rebase and a re-sweep when a landing touched the headers after your base (the bring-over refused TM1 for losing 0x74D990 outright). symbols.json's 0xA297A7 `_wcscoll` is `strcoll`; check a libc name's body before relying on it.
* **Visitor-slot retypes and guarded statics (2026-11-09 night run, IP2):** retyping a visitor slot's parameters renames the short stubs it shares, and the renamed rows lose D188 pairs until `symbols_named.json` carries them (IP2 added 8 rows that replace the generated ones). A file's guarded `<string>` static closes its object: code placed after it lands in the next one.
* **Vtable tables and pooled literals (2026-11-09 night run, IP3):** an attribute subclass's float constant (distance's 0.001f at 0xAEDC40) sits right after its own vtable in .rdata (percent's 100.0f is a shared constant elsewhere, 0xAEA214): count a table's slots up to its 0 entry, not up to the next constant. The pooled empty string literal 0xBDA908 lies in .bss, so `ClientImage.cstr()` returns None for it: read "" there.
* **sclient.h members and VC6 access forms (2026-11-09 night run, TR1):** a single non-inline member declaration added to sclient.h cost sclient 0x74D990 outright; a file-gated `#ifdef __UI_TRADE_MEMBERS` block (as money.h does) keeps it and loses nothing. Reading the message box as `Get_View()->Get_Unknown34()` (through eax) or `View->Unknown34` (ecx) differs per function: try both. Naming a member's intermediate results as locals spends inline budget where moving its definition later does not (0x652EB0); an unsigned index test gives the client's `jae`.
* **appfile PREFIX and int3 padding (2026-11-09 night run, GL1):** `appfile.py match` reports PREFIX (ours a prefix of the client's) when the client function's measured size runs on into the int3 padding after it (0x81EEA0: 402 bytes, padding to 0x81F0DF); verify counts such a function exact, and verify's count decides.



### 3.5 Build VMs and vc6.py

* **A builder stopped mid-verify keeps its pool leases (2026-11-05 evening, SC4):** the VMs stay leased and running; the verify service then cannot start a clone (it needs 6 GB free: 2 GB plus a 4 GB reserve) and posts "verify failed (compile errors?)" (the reason is in .work/verify/prN.json). `pool.py list`, `pool.py release --agent importer-N VMs`, `stop-idle --minutes 0`.
* **`appfile.py match --pool` can return another builder's object** (2026-10-31, TK14: ui_avatar_cust.obj's functions,
  then KeyError on '??1AvatarCreationInterface@@UAE@XZ' while a builder ran the twin file on a shared pool VM): rerun at
  once. `run_branch.py claim` can hang minutes after a GitHub API rate limit ends; `claims.py check` shows the grant
  before the command returns.
* **At most 4 parallel `appfile.py match --pool` jobs** (2026-10-30, RT1): more made pool.py half-create a clone
  (enb-vc6-07, Off with a 0 GB disk; destroyed with `pool.py destroy`), and parallel matches collide in
  `.work/appfile/appfile/obj`. In Git Bash, `--flags '/nologo ...'` is rewritten into a path (MSYS): prefix
  `MSYS_NO_PATHCONV=1`.
* **tryv2 without `--pool` uses `.work/vc6_hosts.json`, which can name a stopped VM** (2026-10-28, LD2): lease
  one and set `ENB_VC6_HOSTS=<user>@IP`.
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
  junction again on every compile since 096a45b; earlier results from a shared VM were suspect.
* **Pool contention, 2026-11-01 night run (9-10 builders on 7 VMs):** `importer.py verify --pool 1` and `appfile.py --pool` exit at once with "pool cap: 7 VMs exist" when every VM is leased, and `pool.py lease` can print it while `list` shows a free VM (a race): retry in a loop (`.work/gm2/retry.sh`, `.work/nw6/v5.sh`). A `pool.py lease` process held `.work/pool/leases.lock` 15 minutes (01:37-01:52) and blocked every lease and release until the 900 s stale takeover; check `pool.py list` and the lock's holder before assuming a hung VM. A failed lease whose last line is exported unchecked falls back to the stale VM in `.work/vc6_hosts.json` (ssh timeout; LG3). Lock waits ran 20-30 minutes: `--lock-wait 3600` on verify. With the lock busy, a scratch copy compiles alone on a leased VM in ~20 s (`.work/nw5/mc.sh`: vc6.py compile with the library's flags; Git Bash needs `MSYS_NO_PATHCONV=1` or `/nologo` becomes a path).
* **Pool (2026-11-01 day run):** appfile.py --pool exits at once on "host memory ... not starting" instead of waiting (retry after minutes; HV); `timeout N` around `importer.py verify` kills it while it waits on the lock (LG); subprocess `bash` can resolve to WSL's bash (empty output): use Git's bash.exe by full path (NW).
* **pool.py release --agent (2026-11-05 night run, CS13):** it releases every VM leased under that agent: not while your own POC build holds a lease.

* **A one-off C1083 on a tracked header (2026-11-05 night run, HC3):** a worktree's first `build.py build --lib app` failed one file with "Cannot open include file '..\packets\pkcolorization.h'" (tracked, unchanged); the rerun built 386/386. Rerun once before blaming a commit, and never run clienttest on an app.lib with a failed file (its 535 unresolved and trace diff are that file's). In a worktree `build/art` is a directory: copy MSS32.DLL into it.
* **appfile.py without --pool (2026-11-05 night run, PG1):** it tries the default host 172.17.3.98 (an ssh timeout when that VM is down): use `--pool` with ENB_AGENT set.

### 3.6 RE tools, Ghidra and REA

* **Never delete `.work/pseudo/` (2026-11-03, the user):** a plain `pseudocode.py` run decompiles only functions not
  yet exact in symbols.json, so the old "delete and rerun" refresh lost every file of a function that had become
  exact since (found when the P-API-5 (b) batch runs needed them); triage.py's `build` and appfile.py's drafts read
  those files. Refresh with `pseudocode.py --all` (every start, in place), then `--check` (exit 1 when a start lacks a
  file; it also lists triage requests without one). The folder was found empty on 2026-11-03 (18:52): worktrees
  reach it through a junction (wt.py), so delete a worktree only with `wt.py remove`, never a recursive delete or a
  bare `git worktree remove`. A zip copy sits in `.work/pseudo-backup/` (no junction points there).

* **objmatch finds symbols.json at `__file__/../../docs/data`** (2026-11-01, TB1): a copy of objmatch.py outside
  tools/re silently runs without symbols (D80 and the self name do nothing; 21 false "losses" in a baseline). Measure
  variants from inside tools/re, or patch the path. `.work/tb1/measure.py objmatch OUT` scores a module over the last
  verify's objects in ~80 s.
* **`explain.py OBJ 'Class::name' ADDR` matches by name prefix** (2026-10-31, TK17: 'CordCon::pump' compared
  pumpClose): pass the mangled name when one name prefixes another. commit_mine.py takes imports.json's block key
  (`cordcon`, not `wwudp/cordcon.cpp`) and needs `{"whole": [...]}` in the spec to include the .cpp itself (`--path`
  takes only the rows).
* **`tools/re/symbols.py` with no arguments regenerates symbols.json, ghidra_symbols.py and coverage.md from the
  default objects** (2026-10-31, TK13: a stray call gutted symbols.json by 159k lines; restored from HEAD at once). Never
  call it without `--objs` / `--out` from a builder; the regeneration is the coordinator's, on a quiet tree (3.4).
* **On a big function objdiff's similarity hardly moves** (2026-10-31, CT1, ui_group 0x5946D0, ~2.7 KB): read
  explain.py's instruction count and register-masked residue instead, and compare the call relocations per callee
  (`objrel.py`) to see which callees inline.
* **explain.py's client window can run through int3 padding into the next stub** (2026-10-31, TG2, 0x466F60: 773
  client instructions against 388 real, up to an atexit stub), inflating the residue: trim the window to the
  function's end (`.work/tg2/dd.py OBJ SYM ADDR END [r]`).
* **`appfile.py match` reads a `#pragma auto_inline(off)` line as a function** (2026-10-30, SC2: "the object lacks
  auto_inline"): put the pragma above the `// @0x` marker and pass `--allow-missing`.
* **Probe several forms in one compile** (2026-10-30, SM2): add members V1..V6 to the draft's header copy, define the
  variants at the draft's end, objdiff each against the client address (`.work/sm2/gen_*.py`). Every variant counts
  as a form (D141).
* **docs/data/asserts.txt's file column is shifted** (2026-10-30, PM1, CH1): around 0xB71324, and 0xB7CB6C-0xB7CD30
  (filed under ui_buff.cpp, ui_chat_panel.cpp's). Earlier note: shifted by one around 0xB71324: `CullObject != 0` is
  proj_man.h line 207 and `texProject != NULL` (0xB71368) is rs_proj_sup.cpp line 58. Check a file attribution
  against the neighbouring rows' line numbers there.
* **names.py run while a tryv2 variant is in the tree** (2026-10-30, LD8) writes the variant's line numbers into
  not-exact.md: rerun names.py on the restored file before committing.
* **Check objmatch's status of the STL helpers a caller should call out of line** (2026-10-30, LD7) in your variant's
  obj before working toward them: `multiple` (identical client copies, e.g. vector<RefCountPtr<T>>::insert 0x47D690)
  never becomes a pair, however byte-identical; only the caller itself can gain.
* **`appfile.py match` on a client/ file** (2026-10-29, LS1): put the draft in `.work/<id>/client/` so the
  `..\shared\` relative includes resolve, and use `-I auto` (it finds wwdebug / sgistl; a hand-written list misses them).
* **`appfile.py match`: leave `-I` out** (2026-10-29, S8): `-I app/common` fails on wwudp's mempool.h; the default
  `-I auto` finds the headers. `commit_mine.py --path` only selects rows; a whole file needs SPEC `"whole"`.
  Coordinators: check a §4 item's addresses against imports.json and attempts.py before briefing it (S8's five
  items were all done or at LIMIT).
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

* **`tryvariants.py`** rejects app files in subdirectories; its variant strings use `\n`. `tools/vc6/tryv2.py` handles several files and subdirectories: use it
  for app code.
* **Parallel compiles on one VM** (`flagsweep.py --jobs`): `vc6._compile`'s object directory was named by
  pid and millisecond only, so two threads could swap objects (fixed: the thread is in the name); the
  first sync of a new directory must run alone (one `sync.tar` per VM).

* **Packet objects (2026-10-26):** `PacketClass::Unknown_14`'s second argument is pushed early (`push -1`
  well before the call), so a disassembly filter on `push -1` hides it. A class's inline virtuals
  (Get_Channel, Get_Type, Get_Name) come before its scalar deleting destructor in the object; an
  out-of-line one comes after Build. pkdecode's range bound was exclusive and cut off the last object
  (fixed in 0df698f); check the last class when a scan ends at a round address.

* **Ghidra can attribute an argument pushed for a later call to the call before it** (H10, 2026-10-27): a pushed `1`
  belonged to a sort object's constructor (`ret 8`), not to 0x541800 (no arguments). Check each callee's `ret N`.
  Including ref_ptr.h needs g_assert.h first (G_ASSERT).
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
  On Windows, Python's `subprocess` 'bash' may resolve to WSL's bash, not Git Bash: use Git's
  `C:\Program Files\Git\usr\bin\bash.exe` (H20).
* **`_$E` initialisers are `short` (2026-10-27, M3):** never exact by themselves; D115's round locates one by its relocated
  fields (atexit, the global) inside the object's range +-0x1000. ref_ptr.h: a script that finds `G_ASSERT(0 != Referent)`
  by text also hits ref_ptr.h's new header note (F7): match the whole statement line.
* **RE tools, 2026-11-01 night run:** `appfile.py match` pairs functions only by a `// @0xADDR` marker directly above the definition (more than ~4 comment lines above, or the tree's `Exact` / `NOT EXACT` comments, don't attach); without `ENB_AGENT` its objects land in the shared `.work/appfile/appfile/obj` and another builder's run overwrites them (FB1, LG1, HV2, CC1). tryv2 deletes its objects: `.work/lg3/keep.py` keeps each variant's .obj for objdiff / explain. tryvariants with `#define private public` mangles members as public (`QAE`): objdiff needs that symbol name (NW2). objdiff.py prints every relocated call as `call REL`, so an IAT `call dword ptr [REL]` always shows as a mismatch even when the bytes are equal (HV2). explain.py's window can run on into the next function (0x8BE270's inline-budget label came from growVector 0x8BE7A0; NW2).
* **appfile.py drafts and include guards (2026-11-05 night run, CS10):** a draft-folder copy of a shared header cannot override it when its include guard is already defined by earlier includes: give the copy a unique name and `#include` it on the draft's line 1 (a comment line).
* **appfile scratch headers stay on the VM (2026-11-05 night run, CS12):** a scratch header beside a marked copy (`.work/<id>/shared/x.h`, reached by a relative include) stays on the VM after you delete it locally; the next appfile match then fails with C1083: put an unmodified copy back. `attempts.py record --forms` takes a count; the description goes in `--evidence`.
* **appfile.py on avatar.cpp and concurrent drafts (2026-11-05 night run, CS18):** `appfile.py match` on avatar.cpp needs `-I C:/EnBDevelopment/SDK/radtools/include_enb -I C:/EnBDevelopment/SDK/directx8/include` (mss.h). Concurrent appfile runs on sibling drafts fail at random with `..\audio\surfacetypes.h` not found even with distinct ENB_AGENT: run them one at a time. A failed verify records its compile_errors only in the working imports.json: find the error with appfile before restoring the saved copy.
* **attempts.py check by member name; two ship-customisation files (2026-11-05 night run, CS19):** `attempts.py check` matches by bare member name across classes (decal's Load is flagged by StarLayerClass::Load / HeightFieldClass::Load rows). The client has both ui_ship_cust.cpp (0x6186A0-0x61E8xx, not in the tree) and ui_shipcust.cpp (from 0x620E40): check the window with `appfile.py show` before placing anything in 0x618xxx-0x61Exxx.
* **appfile.py's -I list and worktree links (2026-11-05 night run, CS20):** `appfile.py match` with only explicit `-I` sends no headers to the VM, and a VM holding an old copy fails with C1083 `..\audio\surfacetypes.h`: add `-I auto` to the explicit list (probably CS18's "random" failures too). A worktree's `build.py link` uses vc6_hosts.json's default host, which can be unreachable: lease with `pool.py lease --agent ID` and pass ENB_VC6_HOSTS.
* **Batch API variants (2026-11-08b, RT1, VB1-VB3):** use `variants.py try --fit` (the 11-01 splice dropped blank lines, so asserts moved and right variants failed on a `push <line>`); when applying, reproduce the line counts with blank lines in the file's padding, not the fit's packed braces or `#line`; restore the `// NAME` tags variants drop from signature lines; check what a gained address really is (a gain through an invented forwarder can be the client's out-of-line template copy, 0x561C40); distrust an 'exact' that needs inline asm (D70; 0x455590 was a wrong pick); don't apply a gain-only variant that lowers its own function's similarity by more than 0.1; a stash in docs/attempts holds one function at given lines: check they still hold it before splicing.
* **Small tool quirks (2026-11-08b, RT1, VB2, SY1):** explain.py's 'relocation: Fail_If_Report vs Unknown_Fail_If_Report' is a naming difference in symbols.json (0x4E9450), not a code one; every `appfile.py match` clears the agent's obj directory; `attempts.py --blocker` takes only its own classes and fails silently on another from a script; clientimage.py's dword_refs misses a vtable's reference to a function the table holds through a thunk: find the E9 thunk first, then references to it.
* **symbols.json feeds objmatch (2026-11-08b, SY1):** objmatch reads docs/data/symbols.json (self names D88 / D213, known vtables), and symbols.py reads the existing file through objmatch, so a regeneration changes verdicts and depends on the file it replaces: regenerate from the landed file and run a full verify of every library after the regeneration, not only before.
* **appfile.py quirks (2026-11-08c, W1 / W2 / LV1 / AT3):** `// @0xADDR` with a trailing comment is ignored, and a plain `// 0xADDR` line between the marker and the function overrides it: put the bare marker directly above. It reads stdin (`< /dev/null` in loops). It resolves `#include "..\..\ww3d2\x.h"` relative to the draft, and explicit -I dirs may not upload changed headers outside them. It can disagree with verify for sclient.cpp. A trial that changes a function's line count sends every later function to `none` (their asserts move). objdiff stars a row whose relocated immediate is 0; symbols.json's `_wfopen` / `_fwprintf` at 0xA27B12 / 0xA284C7 are fopen / fprintf (explain.py's false relocation class).
* **RE tools, 2026-11-09 night run:** `appfile.py match` mixes overloads (two same-named functions share one line): use `objdiff` with the mangled name (XR1). `explain.py` masks literals, so `""` and `" "` look equal: compare the string's address (FN1: Change_Text's `" "` at 0xB6E948 was a real functional fix). vr.py (IB2's variant runner) inserts markers that shift `__LINE__`: re-run the exact-form check without the markers before keeping a variant (CP2).


### 3.7 Harnesses, programs, unattended runs

* **The POC run of 2026-11-07 (WE2-WE8):** an image that "stays on top" can be a stale back buffer (the POC runs windowed,
  space mode clears no colour): check the view renders before chasing its removal. A gen stand-in of a bool render hook (Pre_Render)
  answers false and hides every hooked model; an uninitialised gen stand-in (co.h's AccelClass) corrupts a float every frame: grep the
  stubs log for `gen` lines on the path first. A black model with AMBIENT=0 in a draw dump is a missing light, not a texture
  (`drawdump.py`). Retail's frame count at a given time varies (in world at 250000-290000; slower POC needs `--seconds 140`) and
  `D3D8SHOT_MAX` caps the frames logged: dump a dense spread and keep the last frame with ship draws (fvf 0112).
* **A landed app definition collides with stand-ins in three places** (HF1, HF2): programs/clientpoc/poc_standins.cpp /
  poc_world*.cpp, stubs/app_stubs.cpp and tests/clienttest/client_stubs.cpp; and new callers can make app.lib pull an object that
  clienttest also compiles itself (shiputils, ui_talkt): LNK2005 in clienttest with a clean verify. Grep all three, link both programs.
  `python tools/vc6/standins.py check` finds the first kind in seconds (wt.py bring-over refuses on it); a stand-in that
  must stay (the app object is never pulled) gets `// standins: keep (why)` on or just above its line. The second kind
  needs the link: `python tools/vc6/linkgate.py check` (bring-over runs it; an unresolved symbol outside
  tools/vc6/link_baseline.json refuses too).
* **A test's stand-in goes stale when app code is pointed at a library symbol (kerneltest, 2026-11-04):** c759a57b
  replaced kernel.cpp's `Unknown_BFAEE0` with `WW3DAssetManager::Get_Instance()`; kerneltest.cpp still defined the old
  global, so the link pulled ww3d2.lib(assetmgr.obj) and, a chain later, ww3d.obj, which clashes with taskstest.cpp's
  WW3D stand-ins (LNK2005). A rename or retype that touches a file a test compiles (kerneltest.py SOURCES, clienttest's
  list) moves the test's stand-in too. Which symbol pulled an object: link with `/VERBOSE` (`set LINK=/VERBOSE` in the
  link batch; `Found X / Referenced in Y / Loaded lib(obj)` chains back to the program's own object).
* **`build.py` compiles one `cl` per flag set (2026-11-04):** FILE_FLAGS (wwlib's rawfile.cpp `/GX`, P-B2) had made
  `build.py` stop at wwlib's flag assert since 2026-11-02, so `build/wwlib` stayed old; libraries built before then
  are stale.

* **POC and the retail reference (2026-11-04 night run, LIGHT, CRE2, SHIP, DEC, CTFIX):** `build.py link --out DIR` reads
  the libraries from DIR (`<lib>\<lib>.lib`, `_sys`): copy the worktree's `build\*\*.lib` there first. `clienttest.py build`
  needs ENB_VC6_HOSTS (`pool.py lease --agent ID` prints it; `--agent` is required). clienttest links the worktree's whole
  app.lib: a newly real function anywhere removes its `gen` lines from the trace, and a real function that collides with a
  client_stubs.cpp stand-in fails the link (LNK2005): shiputils.cpp is now compiled into clienttest with 0x80FF20 renamed (it
  needs a device). clientpoc input times are ms from the window's start (the full creation flow needs `--seconds 150`);
  `T down X Y` / `T up X Y` drag; D3D8SHOT_EVERY saves nothing without `--shots`; D3D8SHOT_DUMP runs ~2,200 fps (against
  ~3,500): pick a dump's frame from a run with the dump set; the main tree's build/d3d8shot/d3d8.dll may predate d3d8shot.c
  (no `draw N: material` lines): rebuild before diffing dumps; d3d8shot.log logs every CreateFileA (grep it). A step hook
  that jumps to creation's galaxy select must create the narrator (0x67AB20 / 0x67C440) or ui_shard's ctor crashes.
  A POC link with unresolved symbols still writes the exe: calls to them jump into the PE header (EIP 0x400003): read
  the link log's `unresolved:` lines first. `clientpoc.py replay` does not say whether the client exited: read
  antprox.out's "Client exited" line (P-QUIT-2). appfile.py match in a worktree needs `--pool` (with ENB_AGENT set).
  Retail reference runs (`--exe` a copy of the retail client.exe): `find_window` takes the terms dialog (#32770) as the input
  window (input to it is lost when it closes); the retail client reads the real cursor, so LIGHT's
  `.work/wt/light/.work/light/` has a d3d8shot proxy with poc_input-style user32 hooks (D3D8SHOT_INPUT=1) and a BM_CLICK on
  "I Agree" (the dialog's own input; P-LIGHT-1 proposes `clientpoc.py --retail`).

* **POC (2026-11-03 afternoon run):** a POC link takes `build/<lib>/<lib>.lib` as they are: a library fixed in
  source but not rebuilt ships the old code with no warning (the cursor fix c3427718 was missing from every exe until
  ww3d2.lib was rebuilt; check a fix's function in the exe at its client.map address). `importer.py verify` does not
  rewrite build/app/app.lib: `build.py build --lib app` before linking. `build.py link --out DIR` needs every library
  under DIR (copy the whole build/). Shots count presented frames, not ms (the login renders 2,000-4,000 fps; use
  D3D8SHOT_EVERY with D3D8SHOT_MAX). Per-instance runs: `clientpoc.py run|replay --instance NAME --exe EXE [--messages]`
  (ClientPOC\inst\NAME\release, own Data\client\ini: antprox /REPLAY rewrites auth.ini at start; `clientpoc.py clean
  --instance NAME` when done, `--all` / `--older HOURS` for the coordinator: it unlinks the instance's junctions into
  ClientPOC\Data, never follows them, and keeps an instance whose client.exe still runs); replay takes
  d3d8.dll from the worktree's own build/d3d8shot; symbolize crashes with the instance's own client.map, never
  retail's. `clienttest.py build` has no --pool: lease a VM and set ENB_VC6_HOSTS when others build. The gen stand-ins
  define `Unknown_XXXXXX` data globals that may really be a library's statics (WW3D 0xBFAE4F-0xBFAE52: the app wrote
  them, ww3d2 never saw them) and zero-fill globals the client starts at 1: check `client.py refs ADDR` first. The
  client's message server (-ENABLE_MESSAGES) sends only while a listener is connected and accepts at most once a
  second: start-up reports before its first run are lost (the POC tees them into stubs.log).
* **POC runs started by agy builders crash at startup (2026-11-02 evening run):** every clientpoc run an agy builder made
  (runs 63-69) crashed in WW3D::Set_Gamma (render device not set) while the coordinator's runs of the same code at master
  and the run's HEAD ran 40 s; a process agy starts apparently gets no usable D3D adapter. Builders' POC checks prove
  nothing: the coordinator runs the POC itself (`tools/agy/poc_check.sh NAME COMMIT`, D187).
* **agy builders (2026-11-02 evening run):** `--mode auto` is not available on the user's account (`--dangerously-skip-permissions`
  suffices); under Git Bash set `MSYS_NO_PATHCONV=1` (a leading `/` in an argument becomes `C:/Program Files/Git/...`); agy runs
  its commands in PowerShell whatever the brief says; take tokens from the stream's last line (`result.usage`), never from
  the builder's report (off by 10x or more). A brief must say which WinMain stand-ins open dialogs: 0x847D20 shows the ROC
  text in retail too (the brief that said "the POC never reaches it" let the builder remove its stand-in).

* **clienttest after a rebuild** (2026-10-29, UI1): a rebuilt function in an object the link already pulls runs its
  real code on the fakes (START crashed until the fake view held render objects); hand stand-ins in client_stubs.cpp
  give LNK2005 once the function is rebuilt (delete them in the same commit); gen_stubs writes pointer parameter
  types as `class`, so a `struct` parameter never resolves; clienttest.py now deletes the previous trace first (it
  used to compare a stale one when the exe could not start). Harness builds compile other builders' untracked
  app/packets/*.cpp: build from a worktree.
* **clienttest links build/app.lib** (2026-10-29, FU1, F9): rebuild it with `tools/vc6/build.py build --lib app
  --allow-dirty` before comparing with expected.txt, or other builders' committed work shows as differences.
* **`wt.py new` copies no build/art** (2026-10-29, F9): a worktree clienttest run exits 3221225781
  (STATUS_DLL_NOT_FOUND, MSS32.DLL) until build/art/MSS32.DLL is copied in. A clienttest exe built before other
  builders' commits differs from expected.txt: rebuild before comparing.
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
  asset manager, 1248b9d). co.cpp's loader 0x6F51D0 reads basset.ini, Set_Model's radius 0x6F39F0 and the mount
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
* **Run tools by absolute path from a worktree (2026-10-27, A-haiku):** tools that write the tree refuse a cwd in another tree (2836859); run them by absolute path so the guard can see it.
* **udpreplay, 2026-11-01:** a new app file that defines a stubbed function must delete its stub in `stubs/app_stubs.cpp` and join udpreplay.py's source list, or every program linking app.lib + stubs fails with LNK2005 / LNK2001 (unknown_verb.cpp, 7e0061be; verify does not show it; NW3, CC2). `udpreplay.py build` fails with PermissionError while another builder's udpreplay.exe runs: retry (NW7). Harness counters live outside the client's classes (D176: `tests/udpreplay/cord_counters.h`, `CORD_HARNESS_COUNT`).
* **The POC client and clienttest (2026-11-01 day run):** `build.py link` doesn't rebuild app: `build --lib app` first, or the POC runs stale objects (INP); `build.py build` used to skip wlib / wnet under libs\wwonline (fixed, LINK). gen_stubs: one compile error in the generated file loses every stand-in (guarded now); it skips template-pointer signatures (a test program then needs the real .cpp; GC). The POC writes *.th6 thumbnails into ClientPOC\Data\client\mixfiles, as retail does; its deploy folder and poc_window.cpp are shared, and others' uncommitted window edits broke the display twice (rerun after they land; CV). clienttest: fixing an LNK2005 layer exposes the next (an hour per round); fake objects called through a slot need a stdcall slot with the callee's argument count; once kernel / input code is real, give it a fake task on a real KernelClass and an empty KeyQueueClass; its crash address prints only when the exe is run by hand; build tests in a worktree (`wt.py new`, `build --lib app --allow-dirty`, copy build/art/MSS32.DLL), not against the shared app.lib (CT, CT2). udpreplay's loop0 can differ once (the 90 s give-up reads time(0)): rerun first (CC2).
* **clienttest's failure layers (2026-11-01 day run, CT3):** after the LNK2001 / LNK2005 layers come a missing DLL (0xC0000135 at load, no trace file: stand in the import, e.g. `_imp__AuthLoginLib_GetComponentFactory`), then a pre-main static-initialiser crash (no trace file either: a link stub calling Trace before Out exists), then runtime crashes in newly real code. After a crash `.work/harness/clienttest/out.txt` is the last good run's; the real output is clienttest.out.txt. `.work/ct3-dbg.py` (ctypes, Wow64GetThreadContext) prints a 32-bit exe's exception address and stack.
* **POC (2026-11-01 day run, late reports):** build.py builds only `git ls-files` sources: a new uncommitted file is silently left out and gen_stubs stands in for it: `git add -N FILE` before a POC run (IS). A symbol gen_stubs skips (a `SignalBaseClass::CallbackClass *` parameter) is linked at 0 by /force:unresolved: the crash is EIP 0x400003 with no caller (the image's 'MZ' pops the return address) (FS). Before moving a ui_* file onto a real class header, check that it doesn't declare its own stand-in of a base (ui_group / ui_manu's GadgetClass: C2011 / C2555; BTN).
* **clienttest after POC work (2026-11-02 run):** a gen stub that tolerated a null `this` becomes a real function in app.lib and crashes clienttest (exit 0xC0000005 after a link fix: smouse 0x515340 on SClient +0x1274): give the test a zeroed fake object. New Win32 imports in app code (WinInet in tasks/settings_store.cpp) need the library in clienttest.py's and build.py clientpoc's `sys` lists (CT4). `build.py build --lib app --allow-dirty` before a clienttest link, or it links a stale app.lib. (fontdatabase.h's 0xBE67B8 no longer clashes with tsk_game.h's: one type since P-H12-3, 2026-11-03.)

* **clienttest and POC stand-ins when app functions become real (2026-11-02 night run, MSG, MSG2, DBG, LD1, LF, PF, UIB, MV):** fakes need the real size and vtable once an app function is real (MSG: ViewUnknown34Class 0x1F8); making a table-calling member real while its object's ctor is still gen crashes at run time (MSG2, d043eeba; verify does not catch it); a function in app.lib becoming real while a harness stubs that file's others pulls the object in (LNK2005, DBG): stub every symbol, and clienttest.py strips udpreplay's stub for a function that became real (LF, 04968dfa); a new app object can pull wlib operators (Wstring ==/!=, `operator=`) into clienttest, which then needs hand stand-ins (LD1, LF, MV). App externs for WW3D statics get zero POC stand-ins: use `WW3D::Get_Frame_Time()` / `Get_Sync_Time()` (PF: frozen frame clock kept text alpha at 0).
* **POC runs (2026-11-02 night run, SCR, PI, CV2, MV, DISP):** a run in the shared tree crashes from others' dirty files: use a clean worktree (copy d3d8shot.dll / MSS32.DLL); the login runs ~5,000 fps (shot frames in tens of thousands, per-frame deltas ~0 ms); `clientpoc` link fails with PermissionError / deploy fails while another client.exe runs (retry ~45 s); the real 0x442AC0 reads the registry (read-only), so `ENB_POC_WINDOWED` / `--fullscreen` no longer act; runs.md row numbers collide (read the last row before appending). `open(p,'w',newline=bad)` truncates the file before raising (PI). Reports of a clienttest crash at HEAD in a worktree: first check the link (UIB, HC2 items).
* **POC input and agy (2026-11-02 agy night run):** the client reads the cursor through GetCursorPos / GetMessagePos (not the message's lParam), moves it with SetCursorPos and takes input only while GetForegroundWindow is its window (input.cpp 0x42EE90): drive it with `clientpoc.py --input FILE` (poc_input.cpp's virtual cursor), never the real desktop. A builder's POC-only edit (programs/clientpoc) must be linked to count: `build.py build --lib app` does not compile programs/clientpoc, and CSEL's forwarders never linked (reverted); link with `--out DIR` so the deployed exe is not replaced. Header-local inline `return false` slots (textgadg.h's 0x65C920 "folded") are real: check the client's vtable before suspecting them (`clientimage.ClientImage().u32(vt + 4 * slot)`, then the thunk). agy's stream `usage` is per turn: sum the lines for a running item; `agy -p /usage` in Git Bash needs `MSYS_NO_PATHCONV=1` (else it is a prompt); Python heredocs in Git Bash turn `'\\n'` into a real newline (write scripts with the Write tool).
* **Worktree builds (2026-11-04 day run, REG):** `build.py link --program clientpoc` in a worktree deploys into the shared `ClientPOC\release` (its deploy check compares `--out` with the worktree's own build/), and `--out <wt>\.work\poc\bin` fails (it looks for the libraries there): builders don't link POC exes in worktrees; the coordinator runs POC checks from the run branch. clienttest in a worktree needs `ENB_VC6_HOST` set to a leased VM (else a stale host) and build/art copied in (else 0xC0000135). The importer lock is global across worktrees: verifies can wait ~10 min (set `ENB_LOCK_WAIT` high).
* **Retail replays (D190, 2026-11-04 day run, TOOLS):** `clientpoc.py replay --retail` needs the d3d8shot proxy rebuilt with the input hooks (`python tools/harness/run_reference.py build --pool 1`, then copy build/d3d8shot into a worktree); the retail client.exe is `<install>\release\client.exe`. Retail character select presents only a few thousand frames in ~50 s: shots at 300 / 1000 / 3000, not POC-style tens of thousands. With `--retail` the run's kill ends antprox before it logs "Client exited", so D191's line says "still running when killed". Draw dumps: `D3D8SHOT_DUMP=N`, compared with `tools/harness/dumpcmp.py A B [--frame N]`.
* **Host load and appfile objects (2026-11-05 night run, CS1):** check the host's free memory before trusting a stalled POC replay: with a 7 GB game running the frame rate collapsed and the ms-timed input could not finish the flow. `appfile.py` without ENB_AGENT writes its objects to the shared `.work/appfile/appfile/obj`: set ENB_AGENT to the builder id. 
* **A builder's own POC check (2026-11-05 night run, CS3 / CS5 / CS6):** builders improvised `--out build2` and missing `--exe` paths (one replay silently ran another build). The aligned form: `build.py link --program clientpoc` in your worktree (its default build/ deploys into ClientPOC\release: fine, the release exe is shared and any build may sit there), then `clientpoc.py replay --instance <your id> --exe <worktree>\build\programs\clientpoc\clientpoc.exe --input FILE`: the instance runs that exe, whatever release holds. The exe is named clientpoc.exe there (deployed as client.exe); `--exe` must exist (clientpoc.py stops otherwise). `.work/coord/pocfull.sh` is the coordinator's (instance coord).
* **Short effects in POC shots (2026-11-05 night run, CS12):** shots count presented frames and the frame rate varies ~10% between runs: locate a 0.5 s effect (the avatar teleport) with a sweep of up to 64 shots around its time, and pick the frame by time / pixel stats (`.work/cs12/pick.py DIR`) before opening one image.
* **The POC stand-in sweep (2026-11-05 night run, RS1 / CS12 / CS14):** a missing effect or control in the POC is most often a generated stand-in (stubs.log `gen ...`, returning 0) or an empty body on its path: run the flow, list the stand-ins called after the screen appears, rebuild those (the class screen's planet turn, teleport sparkle and skill slider were all this). A caller's declaration with another signature than the real function (a local stand-in class, `void` vs `bool`) links to a gen even when the real one is exact.
* **A corrected caller signature reaches real code in the harness (2026-11-05 night run, HC / HF):** fixing an app caller's declaration to the real signature (afa1bba5's Unknown_6603A0) changes the mangled name, so clienttest's fake of the old form no longer intercepts it and the real function runs on the harness's fake object (0xC0000005). After a signature fix, grep tests/ for the old form and fake what the real function reads instead (d3f654e6). A constant defined only in programs/clientpoc/ (poc_data.cpp) needs its twin in clienttest's stand-ins (4f94141c).
* **Git Bash rewrites a leading `//` in an argument (2026-11-06, GP1):** a `// @0xADDR` argument arrived as `/ @...`; use `MSYS_NO_PATHCONV=1` with `C:/` script paths.
* **`build.py link`'s summary and `attempts.py check`'s noise (2026-11-06, MF1):** `build.py link` prints "clientpoc: exe NOT written; 569 unresolved" and still deploys an exe built with generated stand-ins: read the 'deployed' line, not that summary. `attempts.py check FILE` also flags functions compiled into the file through headers (0x726100 ClientClass for ui_mfd.cpp).
* **clienttest after app code lands (2026-11-06, HF1):** when a stand-in's class becomes real (VerbManagerClass, verbmanager.h), one C2027 in clienttest.cpp or client_stubs.cpp gives ~600 unresolved and LNK2005 lines: read the `cl:` lines first. A stand-in for a function in an app.lib member other code already pulls in (ui_verb.obj) gives LNK2005: compile the real file into clienttest with the function renamed (`clienttest.py`'s APP list, shiputils' precedent). A fresh worktree has no `build/art/MSS32.DLL`: `clienttest.py run` then says "wrote no trace (exit 3221225781)", 0xC0000135.
* **Scratch tests of landing tools (2026-11-06, LD1, DC1):** put test commits outside library directories (a comment line in an EA-derived file moves `__LINE__`, §3.0). A wait loop on a landing's verify log hangs if the batch stops at a conflict first: also check the process is alive. `tools/re/datacmp.py --objs` takes a `C:/` path on Windows Python, not `/c/...`. The Write tool can create CRLF, and an inline Python replace with `\\0` put a real NUL in a script: `ast.parse` after patching one that way.
* **Reading a kerneltest build failure (2026-11-06, KT1):** a compile error in taskstest.cpp shows up as hundreds of unrelated unresolved symbols and LNK2005 lines, all fallout (the BINKMovie ones were): read the `cl:` lines first (`grep -v unresolved`). A worktree has no build/, so pass the main tree's as `--out`. `pool.py lease` and `release` both take `--agent NAME`; a bare IP released nothing.
* **Worktree POC and clienttest runs (2026-11-08 night run, PD1, TL1):** a worktree has no build/art or build/d3d8shot: copy both in before `clientpoc.py replay --exe <worktree>...` (a missing d3d8.dll otherwise), and build/art/MSS32.DLL before clienttest (exit 0xC0000135). `appfile.py match` draft copies under `.work/<id>/m/` resolve their includes and compile, so forms can be tried while a verify runs on the tree; `--diff` on a tree file itself prints nothing (only `// @0xADDR` markers count): copy it there and rewrite the marker (SB2). A generated stub for FontSys 0x6CE420 (vsnprintf, then Change_Text) leaves every formatted text in the POC empty (chat lines, inventory counts): CW2's poc_world.cpp stand-in fills them. WE1's replay never selects a target: the target lines (ui_scan 0x60F1A0) need a replay with a targetable object.
* **clienttest in a builder worktree and its seams (2026-11-08b, CW4, VB2, RN1):** a fresh worktree needs `build.py build --lib app` there (build/app/app.lib) and MSS32.DLL at `<worktree>\build\art\MSS32.DLL`, else 0xC0000135 and no trace. To redirect a call made inside one app object instead of stubbing the caller (which collides with the real definition), add `#define NAME Clienttest_NAME` to that file's wrapper in clienttest.py and define the renamed method in client_stubs.cpp (sclient's Unknown_563760). The coordinator's harness check needs `ENB_VC6_HOSTS` from a pool lease: without it the builds fail and clienttest runs a stale exe.
* **clienttest and real effect code (2026-11-08c evening, HF1):** once app.lib's eff_o.obj links in, clienttest needs
tests/clienttest/data/effect.ini (EA's shipped EFFECT sections for the descs a session asks for, and their links) and
eff_ base assets in basset.ini, numbered from 1001 so other models keep their numbers; concrete effect classes not yet
rebuilt need stand-ins on their real ends. A fresh worktree's clienttest exits 0xC0000135 until build/art/MSS32.DLL
is copied in. bring-over's report.py step ignores --lock-wait: set ENB_LOCK_WAIT=6000 too.

### 3.8 Git, docs and scripting hygiene

* **Landing in a busy run (2026-11-07):** a run-branch commit made while a landing runs is replayed under (wt.py lands
  on top of it) when it touches none of the landing's paths and no generated file; one touching those still refuses
  ("moved ... while landing ... and they touch <paths>": rerun). A branch on another builder's unlanded branch: make it
  with `wt.py new C --base wt/P` (records P); land P first or `bring-over P C` in that order; after P lands `wt.py list`
  says "parent landed: rebase" and `wt.py rebase C` replays C's own commits only (no hand `git rebase --onto`: it skips
  the regeneration, and bring-over refuses "the commits record other pairs"). poc_check.sh's LOG must be an absolute path.
* **Coordinator merges (2026-11-03 afternoon run):** never resolve a C++ source conflict by keeping both sides (it
  duplicated gd_slider's code: 3 lost outright, caught by the verify); append-append of new functions is the one safe
  case. `tools/agy/rebase_gen.sh` once re-verified only app (a library's new file dropped out of the record); it is
  now `wt.py rebase`, which verifies every library the branch touches. `symbols.py` needs
  `--objs` for every verify-* folder and a good symbols.json as its input (it reads its own output: a gutted one cost 16
  pairs, a weak one moved template twins). TaskStop of a background shell leaves its child processes (two redo
  scripts ran in one worktree): check Win32_Process before restarting. A worktree reset onto a new base needs its wt
  base file updated, or bring-over refuses 'merge commits'.
* **`wt.py bring-over` lands on master by default (2026-11-02 evening run):** pass `--target run/<date>`; without it, it
  compared against a stale local master and refused (3 'lost' pairs that were master's age, not the builder's).
* **Generated files are regenerated, never merged (2026-11-02 evening run; tools since 2026-11-07):** on a conflict
  bring-over (single and batch) and `wt.py rebase ID [--onto B]` take the target's side, then verify, report, names.py,
  notexact.py and commit the result (attempts.csv: both sides' rows). Never rebase a worktree by hand (WE8: no
  regeneration); `wt.py rebase` stops on any other conflict and prints how to finish (`--continue`) or abort.

* **`attempts.py record`:** `--forms` is a count, the forms' text goes in `--evidence`, and `--blocker` takes only
  the listed classes (`order`, `inline-budget`, ...) (2026-10-31, TK5).
* **commit_mine skips imports.json's library totals** (2026-10-31, TK7): commit_mine skips imports.json's library totals without `--refresh-totals` (which takes other
  builders' counts too): the coordinator refreshes totals at the quiet tree.
* **`attempts.py record` in a shell loop with `set -- $a` cuts a symbol at its first space** (2026-10-31, TK4):
  quote each field or call it once per row.
* **Deleting a tracked app file breaks build.py's definitions scan (FileNotFoundError)** (2026-10-29, PK3) until
  the deletion is staged: `git rm --cached` your own file (names.py had the same crash; fixed in 5f62144).
* **`\a` in a non-raw Python string becomes BEL** (2026-10-29, SC3): an `#include` written as
  `'..\shared\a...'` lands with a control character; use raw strings or forward slashes (a §3.0 variant).
* **While other builders land on imports.json, automate the bring-over rebase** (2026-10-28, SC2):
  `.work/sc2/rebase.sh` runs rebase, verify, clienttest, udpreplay and replay in one cycle.
* **A Python edit that appends a declaration to a line ending in a `//` comment swallows it** (C2039): prepend
  instead (2026-10-28, CP2).
* **Git Bash turns a `'// 0x...'` argument into a path** (2026-10-28, PX2): prefix the command with
  `MSYS_NO_PATHCONV=1`.
* **`wt.py remove` deletes the worktree's `.work/` scratch** (2026-10-28, MV2): keep scripts you want in the main
  tree's `.work/`. It refuses a branch with unlanded commits (A1) unless `--force`; `wt.py list --unlanded` lists them. Export `ENB_VC6_HOSTS` in the same Bash call as every `build.py build`, or it compiles on
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
* **`wt.py bring-over` refusing on an imports.json conflict** (2026-10-28, LD2): now `wt.py rebase NAME --onto
  master` (takes master's side, re-verifies, unions attempts.csv, regenerates, commits).
* **After a bring-over, `git rebase master` in the worktree can conflict on names-review.md:** `git rebase --skip`
  drops the commit that already landed; "master moved ... while landing" (its new commits touch the landing's paths or a
  generated file; unrelated ones are landed on top of): rerun (2026-10-28, EF2).
* **When master moves every few minutes, `wt.py bring-over` keeps conflicting on imports.json** (2026-10-28, DM1):
  verify in the worktree, then commit_mine from the main tree with the whole file and its imports entry copied
  over (`commit_rows.set_entries`; `.work/dm1/land.sh`).
* **`wt.py bring-over` refuses an imports.json cherry-pick conflict when master has moved** (2026-10-28, HX1):
  recreate the commit in a fresh worktree from master (`cherry-pick -n`, take HEAD's imports.json, re-verify).
* **tryv2 refuses an anchor that occurs twice** (co.cpp's two IsInPVP finds); a long experiment script that holds
  the importer lock for 10+ minutes blocks others' verify and report: release it between variants (2026-10-28, IB1).
* **tryvariants.py only sees a library folder's top level** (`app/packets/x.cpp` can't be tried; `.work/pk1/tv.py`
  and `hv.py` can); **build.py compiles tracked files only** (a new app file is harness-tested after its commit, or
  with `--allow-dirty` and a private `--out`) (2026-10-28, PK1).
* **`wt.py bring-over` refuses while the main tree has another builder's uncommitted generated file**
  (docs/imports.md): never commit imports.md from a worktree; the next `report` run regenerates it (2026-10-28, UP1).
* **The Bash tool starts a fresh shell per call** (2026-10-28, EX1): export `ENB_AGENT` and `ENB_VC6_HOSTS` in
  every call, or appfile.py runs on whichever VM vc6_hosts.json lists. In Python edit scripts, write include
  paths as raw strings (`"..\app"` once became a BEL).
* **Moving a declaration block "up to the next blank line" cuts classes that contain one** (2026-10-28, CR1,
  SBaseViewClass): end the block at an explicit marker.
* **commit_mine keys on a RECONSTRUCTED continuation line must be in the changed line itself** (2026-10-28,
  PS1): a key on the entry's first line misses an edit to its continuation; the hunk lands in the next commit.
* **The Write tool drops tabs on whitespace-only spec lines:** anchor with `"\t"` or tree slices.
* **A slice between two `str.index()` markers** is empty when the second comes first, and
  `replace('', x)` destroys the file: assert it. A short anchor can be part of a longer line.
* **Git Bash:** `"app\shared\auxdata\$f.cpp"` swallows `$f`: use `/`.

* **The git index is shared between parallel builders:** a bare `git commit` sweeps their staged
  hunks into your commit, and `git commit -- <path>` takes a shared file's whole working copy.
  Own files by path; anything touching importer.py, headers, stubs or docs/data through
  `tools/vc6/commit_mine.py` (builders.md). `git stash` is shared across worktrees too.

* **Never write a hand-built `docs/data/imports.json` into the shared working copy** (2026-10-26, two
  sweeps): the other builder's `commit_mine` takes the whole file as it is, and any verify rewrites it.
  Build a private record elsewhere and restore the working copy at once. Before any commit that
  includes imports.json, `pairs_cmp.py` against HEAD's copy shows foreign pairs (gains you didn't make).

* **Untracked files are not protected by the private index:** another builder's `commit_mine` or path
  commit takes your importer.py / imports.json hunks while the files they name are still untracked
  (7ae0040 took pkflag's). Commit a new file with its records in the same step.

* **Shared builds (2026-10-26/27):** `udpreplay.py build` writes the shared `build/` and `conn` runs whatever exe
  is there, so another builder's half-done file can break or taint your run: build before `conn`, or build in a
  clean worktree of HEAD plus your files. Two `static` tables with the same name break udpreplay's single-TU
  build.
* **`build.py link --allow-dirty` compiles other builders' uncommitted
  files** into the shared build/ and overwrites `.work/milestones/l1/unresolved.txt`.
* **`open(p, 'w').write(open(p).read()...)` truncates p to empty** (the write handle opens first): read, then
  write. A draft regenerated from a template overwrites an applied fix: check the tree file before verifying.
  `names.py --tree DIR` regenerates both from another worktree (43b8144). In a shared tree names.py now also sees other builders' untracked drafts: commit through commit_mine (row selection) and restore the working copies after (F7).

* **Run `python tools/vc6/build.py link` for the programs after adding a function to a library file or touching
  stubs/ or pk.h** (L5, 2026-10-27: a missing Resize, then 969f244's stub change pulled pk.obj and 280 externals
  into VIDTEST). A commit subject must not claim a program links unless that command ran at that commit.
* **`__LINE__` files (2026-10-27, S1 / R1 / R2):** new code goes after the file's last `__LINE__` user, or every later
  assert moves; a tryv2 variant that adds or removes one line shows false losses: pair it with an opposite edit; a
  trailing blank line before an appfile `@N` anchor eats a line. A tryv2 edit anchored on a loop line can match twice
  (H22): anchor on the line before it.
* **A rename across our files must leave an imported library's own names alone (2026-10-30, D159):** app code
  reaches into imported classes by their names (soundlib.cpp's `using AudibleSoundClass::m_Unknown7C`, WWAudio's
  `m_` style); renaming `m_UnknownXX` in app/ broke it (12 compile errors in the verify, soundlib's pairs lost).
  Check every renamed name against the libraries' headers, and run a full verify.
* **commit_mine and worktrees, 2026-11-01 night run:** commit_mine's `-U0` hunks merge adjacent added lines across builders: a library-matching.md row, an imports.md line, a RECONSTRUCTED entry or (with `--path` / `"whole"`) every attempts row of the file next to another builder's uncommitted one travels with yours. Put new rows somewhere other than next to another builder's (CC1, NW4, NW6, LG1). `wt.py bring-over NAME` always lands branch `wt/NAME`, `--dry` still runs the full verify, and bring-over refuses while names-review.md or library-matching.md has another builder's uncommitted hunk (commit_mine from the main tree gets around it; NW3, NW6). Never chain a `cp` into the tree after a script that can fail with `;` or `||`: make each step depend on its exit code (CC2).

* **Stand-ins and staged stubs, later round (2026-11-02 night run, EAM 9fe25fe0, LF round 2):** a gen stand-in for a data pointer is NULL, so newly real code that dereferences it crashes the POC: use the tree's symbol instead (EAM, `Unknown_C006A0` / DX8Wrapper::CurrentCaps). clienttest stages udpreplay's stub files: when a stub function became real, strip it from the staged stub or the link has it twice (LF round 2).
* **Scripting slips (2026-11-02 night run, EAM, DG round 2):** Python: build the content before `open(path, 'wb')`, or an exception leaves the file truncated (EAM). `importer.py` RECONSTRUCTED paths need doubled backslashes in the script (DG round 2).
* **Commit and marker mechanics (2026-11-02 night run, SCR, GAD3, SM, LF, DG, MV, MSG2, CV2, PF, MV):** `// @0xADDR` markers sit alone on the line directly above the definition (not after a comment block, not trailing); an appfile PREFIX counts as exact with padding; appfile prints EXACT in capitals. `commit_mine --path` does not commit content (use SPEC whole, PF); its hunk keys must be on changed lines (LF); `--addr ADDR --path attempts.csv` commits a row only (CV2). A stale ` M` stat after commit_mine is cosmetic (DG). PermStringInit `$E70/$E68` pairs and implicit dtors out of line for funclets need D99 rows (SCR, UIB). Removing an extern `NAME` tag drops its row out of names-review on names.py (FT); `Unknown_BFAE52` is another global, match exact names. Identical template instances in one object's range need caller-evidence names; `Create_Peek` binds after `RefCountPtr<T>` instantiates (MV). A removed D66(i) function (a copy no longer emitted but held by other files, e.g. 0x510CC0, 0x77B030) is not a loss.
* **commit_mine.py --path (2026-11-05 night run, CS3 / CS5):** `commit_mine.py --path FILE` commits only that file's rows, not the source: list the source under `"whole"` in the SPEC.
* **commit_mine.py's imports.json entry (2026-11-05 night run, CS9):** it needs `--file <unit>` (e.g. `--file ui_shipcust`) to take the file's imports.json entry; the SPEC's "files" key does not.
* **Comment line counts and NAME tags (2026-11-05 night run, CS17):** removing or shortening a NOT EXACT comment moves later `__LINE__`s (it cost 0x55C280 / 0x551320 until the line was put back): keep the comment's line count. A second `// NAME` tag on a line swallows the first in names-review: put a new getter on another existing line with its own tag.
* **Sweep loops (2026-11-05 day run, AR1):** check the variant index before any parsing that can raise, or a failed apply spins the loop forever.
* **Bash defaults, MSYS paths, appfile's shared obj dir (2026-11-05 day run, SB2):** an expression ending in `}` inside `${5:-...}` truncates the default value; Git Bash turns a `'// ...'` argument into `'/ ...'` (set MSYS_NO_PATHCONV=1). `.work/appfile/appfile/obj` is shared between builders: a concurrent match can fail with FileNotFoundError on the .obj; rerun.
* **Stand-in renamed to the real class (2026-11-05 day run, HC1 / HC2, 607ac770 -> fd2ddbf8):** renaming a stand-in to the real class links the real member into clienttest (cpacket's RECUSTOMIZE called the real 0x61F250 on the test's 4-byte fake and crashed; ui_ship_cust.obj needed money.cpp's operator>=): run the D182 check after renames.
* **Concurrent replays (2026-11-05 day run, AR2):** `importer.py replay` can fail with PermissionError while another builder's replay holds `.work/replay`: chain replay and commit with `&&`, never `;`, and rerun on the error.
* **A verify compiles the tree when it gets the lock (2026-11-05 day run, TB2):** `importer.py verify` compiles the tree as it stands when it gets the importer lock, not when launched: check the .obj times under `.work/<agent>/tmp/enb_import/verify-app` before trusting what was verified.
* **Removing a line from an assert file (2026-11-05 day run, IM1, 3e1075ab):** deleting a `#define` (or any line) in a file with `__LINE__` asserts shifts every later assert: replace the line with a one-line comment instead.
* **Background jobs (2026-11-05 day run, AR3):** a Bash `&` job is killed when the tool call ends: use the tool's run_in_background.
* **clienttest and link aliases (2026-11-05 day run, HC3, c79cd2d8):** gen_stubs writes one signature per name (pointer-returning) and `clienttest.py` does not apply `link_aliases.txt`: a void overload of the same name (GadgetClass::Unknown_08 / _14) needs a hand stand-in in tests/clienttest/client_stubs.cpp. A stand-in class the tree now declares for real (UIPTradeClass) must leave clienttest.cpp (C2011).
* **Git Bash and Python on Windows (2026-11-06, IR2):** `//` in a Python argument becomes `/` unless `MSYS_NO_PATHCONV=1` is set; `bash` started from Python is WSL's, so name the Git path.
* **`poc_check.sh`'s log and branch (2026-11-06, the coordinator):** the log path must be absolute (the script changes directory before writing), and a leftover `wt/NAME` branch stops it at once: `wt.py remove NAME --delete-branch` first. Both failed silently behind a `&`: start it as a tracked background task.
* **POC replay read-outs (2026-11-06, the coordinator):** `poc_check.sh` with CHECK_HARNESS=1 runs clienttest with no lease and it fails at once with FileNotFoundError: lease first and set ENB_VC6_HOST(S). `--shots` counts Presents, and the menus run at about 7,000 a second (1,800 to 2,300 averaged over a run): for character select in the 44 s input3-quit replay use about 290000, not 2400. `client exit: still running when this run killed it` was printed on two runs whose antprox logged `Client exited` right after the quit click; the third run of the same input said `exited by itself`: read antprox's time before calling it a hang.
* **A failed helper's pipe (2026-11-06, SD1):** `pad.py | tail -0` killed the script with a closed pipe and a commit went in with `// @0x` markers. `pseudocode.py` drops call arguments in long functions (0x600000, 0x5FBD30, RG1): `client.py dis` with the pushes is the check. `attempts.py record`: `--forms` is an int, `--blocker` an enum.
* **Runner scripts (2026-11-06, RD1):** `subprocess.run(['bash', ...])` from Windows Python can pick another bash than Git's and fail in seconds, leaving the old log in place, so a sweep looks like identical results: use the full path to Git's bash.exe. Never hard-code a leased VM's IP in a runner: a new lease can return another VM, and a stray compile lands on a VM someone else may hold. Importer text written from a bash heredoc loses its doubled backslashes (KV1): the Write tool only.
* **POC world runs and landings (2026-11-07, WE1, RS1, CS1, coordinator):** a call to a function gen_stubs could not write (a struct by value, `std::vector<struct>`, a Matrix3D return, a non-const global object, a constructor taking a class by value) jumps to 0x400000 and the header's `pop edx` eats the return address: match the link log's unresolved list to the objects (`.work/WE1/coffsyms.py`), hand-write the stand-in. Name POC crash stacks from the worktree's own clientpoc.map (`__validdrive+0x...` is the map's CRT tail). The world frame: `D3D8SHOT_EVERY=2000 D3D8SHOT_MAX=500` with `--shots 1`, keep the highest frame. A real app function clienttest reaches needs its data in the harness (CM1's 0x79B2B0 and CS1's 0x799E40 dereferenced a NULL CameraManager). VC6's decorated name of a static data member encodes its access. A library header edit (wwaudio/WWAudio.h) gets its `// EnB:` entry in that file's end-of-file block (D211). A builder based on another builder's unlanded branch: `wt.py new ID --base wt/OTHER`, then §3.8's stacked-branch rule (`wt.py rebase ID` after OTHER lands; the old `ONTO=... rebase_onto.sh` is no longer needed), or a net-diff commit on a fresh worktree when its own commits conflict with what landed since. The importer lock is shared by every worktree: `--lock-wait 3000`. `cat > /dev/null` in a Bash call waits on stdin; Git Bash has no `pgrep`; MSYS rewrites `'// @0x'` arguments (`MSYS_NO_PATHCONV=1`).
* **Git Bash and attempts.py arguments (2026-11-08 night run, PD1):** in Git Bash a Python argument starting with `//` is path-converted, so a marker pattern like `// 0x...` silently matches nothing (PowerShell, or `MSYS_NO_PATHCONV=1`); `attempts.py record --forms` takes a count, the forms' text goes in `--evidence`.
* **Python edit scripts on Windows (2026-11-08b, SY1):** `open(p, 'w')` writes CRLF; write with `newline='\n'` (the tree is LF, D147).
* **Rediscovered hygiene, 2026-11-09 night run (JB1):** Git Bash rewrites `//` arguments (`MSYS_NO_PATHCONV=1`, see the notes under 3.4 and 3.6); parallel `appfile.py match` runs in one worktree share `.work/appfile/appfile/obj` (run them one at a time or in separate worktrees); scripts go through the Write tool, not a heredoc.


### 3.9 Library-specific

* **wlib (wwonline) (2026-10-31, TK4):** Renegade's `Code\Tools\RenegadeGR\wtime.cpp` is the client's revision
  (no localtime_r wrapper, no "aa" format), not matchbot's: look for older per-file copies in RenegadeGR before
  changing matchbot's. wlib is built with /GX (`LIB_FLAGS['wlib']`, +17 across wtime / configfile / monod / streamer /
  xtime): in VC6 a by-value return of a class with a destructor leaves a construction flag (`push ecx; mov [esp+8],0`)
  even without an EH frame, a cheap sign of /GX. `tools/re/names.py` does not scan libs/wwonline (its NAME tags miss
  names-review.md).
  (TK7) wstring.cpp's and configfile.cpp's client revision is E&B's own, matchbot's with E&B's changes (its `// EnB:` block)
  (RenegadeGR has neither; matchbot, mangler and Renegade's Tools\wlib are one text; its tell: no Joe Howes / MDC
  additions). gaps.py pairs wlib's configfile wrongly (the client's object is 0x8B23C0-
  0x8B3E70: ctor, dtor, clear, readFile, enumerate, getString x2, getInt x4): confirm with `client.py callers`. A static
  helper whose last caller is one function gets inlined (Get_Date_From_Day into getTime once getYDay is gone).
* **cordcon's header-field reads go through Cord_Get_Small / Cord_Get_Seq** (2026-10-31, TK17), out of line but
  inlined once declared before the caller: that gives the client's `mov eax, n` / `add edi, eax` and register choice
  (examinePacket 0x8514D0 0.62 -> 0.73). buildHeader, write and read still use the byte-wise Read_* / Write_* statics.
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
* **wnet / wwudp (2026-11-01):** wnet's udp.h defines `close` / `read` / `write` as macros under `_WINDOWS`: app files that include it `#undef` them (app/wwudp/udp.h forwards to wnet's and does; NW7). E&B's UDP is 0x2C bytes (a second socket, a list of CONNRESET peers; NW5, 9727b6d6); udp.cpp includes <string> and app's permstr.h directly (interim). CordCon::write takes `(channel, len, data, priority)` (NW1 / CC1).

