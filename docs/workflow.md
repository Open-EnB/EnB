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

§3.0 is for everyone. Then app code: §3.1.1 and §3.3.1 up front; §3.1.2 and §3.3.2 looked up by symptom; §3.2,
§3.4, §3.6, §3.8. A Westwood library: §3.1.1 up front (§3.1.2 by symptom), §3.4-§3.6, §3.8 (§3.2 if
template-heavy). A harness or program: §3.5, §3.7, §3.8. §3.9 is looked up by file or library name (grep its
`####` headings for the files you touch), never read whole. Tool usage is in tools/re/README.md,
tools/vc6/README.md and tools/harness/README.md.

### 3.0 Everyone

The rules themselves (pair sets against the baseline, the `// EnB:` block (D211), D69 safety, `git add` by name,
UTF-8 docs, never waiting for a clean tree) are in CLAUDE.md and builders.md §2; this section holds the traps.

* **Other builders' uncommitted edits reach your verify.** Tracked edits, files the ledger lists and a
  half-edited widely included header all compile into your verify: their compile errors show as your losses,
  their gains as yours, and verify writes all of it into the working ledger shards (docs/data/imports/), which
  can change again between your verify and your commit. udpreplay.py build and `build.py link --allow-dirty`
  likewise compile into the shared build/. Symptom: a non-zero error count on verify's last line or in
  `compile_errors`, changes in others' files: read both before `pairs_cmp.py`, compare against HEAD's shards
  (`git show HEAD:...`) over the whole library, commit your own blocks, and only then restore the shards (`git
  checkout docs/data/imports` before the commit discards gains commit_mine still needs). Draft in a scratch
  copy (`appfile.py match` on `.work/<id>/m/`), touch the tree only to verify and commit at once (another
  builder can overwrite an uncommitted edit within minutes); harness builds go in a worktree. A verify
  compiles the tree as it stands when it gets the importer lock, not when launched (check the .obj times under
  `.work/<agent>/tmp/enb_import/`). Before changing a member's type, grep the other builders' working copies,
  not only HEAD; list your own files by name, never from `git status`.
* **What the shared index and working copy take from you.** Another builder's `commit_mine` or path commit
  takes your ledger hunks while the files they name are still untracked: commit a new file with its records in
  one step. Never write a hand-built ledger shard into the working copy (commit_mine takes the file as it is;
  build a private record elsewhere). names.py reads the working tree, others' untracked drafts and line shifts
  included: commit through commit_mine and restore the leftover diff. `git stash` is shared across worktrees.
* **The importer lock** (§1). It is global across worktrees: a verify can wait ~10 minutes (`--lock-wait
  3000`). A script holding it that starts `importer.py`, `tryvariants.py` or `trydiff.py` as a child
  deadlocks: start children from an unlocked parent or call in-process; release it between variants of a long
  experiment. `tools/vc6/zhall.py` holds it ~15 min (ww3d2); read-only `tools/re/objdiff.py` still works. A
  killed run or a transient `PermissionError` on a tree write (tryv2's restore included) leaves a half-written
  file: `git status`, restore, delete the lock.
* **Shell heredocs, `sed` and `printf` mangle escapes silently.** `\n`, `\t`, `\v`, `\r\n`, `\0`, `\\b`, `\\1`
  become control characters, NULs or the wrong regex; backslash paths halve (`sed -i '3a\#include
  "..\\..\\x.h"'`); `"app\shared\$f.cpp"` swallows `$f`; `IFS=:` splits C++ `::` names. Symptom: a file that
  fails to compile for everyone, a regex that deletes what it should keep. Write scripts and specs with the
  Write tool and save them in `.work/`; in Python use raw strings, forward slashes or `chr(92)`
  (`'..\shared\a'` puts a BEL in an `#include`), and `ast.parse` a patched script. The Write tool drops tabs
  on whitespace-only lines: anchor with `"\t"`.
* **Git Bash rewrites arguments that start with `/`.** A `// @0xADDR` or `// 0x...` argument arrives as `/
  @...` or a path and silently matches nothing; `'/nologo ...'` and `/usage` become `C:/Program Files/Git/...`
  paths. Prefix the command with `MSYS_NO_PATHCONV=1` (then pass `C:/` script paths) or use PowerShell.
  Windows Python wants `C:/` paths, not `/c/...`; `bash` started from Python is WSL's (name Git's bash.exe);
  Git Bash's `ps` / `pgrep` do not see Windows processes (use `tasklist`); `cat > /dev/null` waits on stdin.
* **Line endings: LF everywhere** (D147, `.gitattributes`). Every checkout and worktree is LF; Git normalises
  what it stores, but until a CRLF working file is fixed, byte compares and edit anchors see the CRs. Python's
  text mode writes CRLF on Windows (`open(p, 'w')`, `Path.write_text`): write with `newline='\n'` (or bytes),
  read in the default text mode, and assert each replacement's match count. After a scripted edit `python
  tools/eol.py check` (and `fix`). (Conflicting reports, unresolved: D147's test found the Write tool writes
  LF, one later report saw it create CRLF; `eol.py check` settles each case.) A CRLF file list breaks `xargs git add` (strip the CRs).

### 3.1 VC6 code generation

#### 3.1.1 Read first

**Triage.** Classify a near miss first: `python tools/re/explain.py OBJ SYMBOL ADDR` prints one class,
the first mismatch and the masked residue. Fix the classes in this order, since an earlier one masks
the later ones: **relocation** (wrong or unresolved target, string or symbol) -> **misplaced start**
(the window opens like another function's tail) -> **frame** (frame size) -> **field offset** (same
shape, `[reg+N]` differs) -> **literal** -> **register-only** / **order-only** -> **inline budget**
(a callee called at one site and inlined at another; `??_H` temporaries) -> **shape**. Before any
retry read `python tools/re/attempts.py show ADDR` (builders.md §2). Compiler switches: one
`tools/vc6/flagsweep.py` run rules them out; never loop it. No switch has ever fixed a near miss (every
`/O1 /O2` x `/Ob0-2` x `/G3-/G6 /GB` combination on Make_Screen_Shot 0x9182C0; the library's own
`/O2 /Ob2 /GB` was best): an allocation residue comes from the source. Count instruction rows from
`appfile.py match --diff` or objdiff, not the similarity score (0.81 stayed while rows went 3 -> 0), and
believe verify over `explain.py`'s class. In large x87 functions the masked residue swings: two forms that
each helped can score worse combined; compare diffs by hand.

**Two-attempt stop rule.** After two failed attempts on a register-only or order-only residue,
record it (`attempts.py record ADDR ... --status parked --blocker register|order --forms N
--evidence ...`, stash the best form) and move on. A register-only or order-only residue never gets a
third attempt by anyone: VC6 collapses local rewrites into a few canonical outputs (Make_Screen_Shot 0x9182C0
took ~110 forms to learn this; a two-value clamp swapped esi / edi / ebp under ~120 forms); the lever is
function-wide or nowhere, and it waits for a lever proven on another function. An inline-budget, frame or
shape residue gets one Fable round under D72 and D89 (builders.md §4): 8 forms from stated hypotheses, then
a written explanation in attempts.csv and `LIMIT REACHED`; `attempts.py check` refuses a later retry
unless `--lever` cites a new row of the table in §3.1.2.

* **A pair that holds only with other files' exact text is a near miss (D203):** never work round other code
  (keeping a header byte-equal, dropping a needed edit) to keep a pair won earlier; aim for both changes exact
  at once, and name D203 in the commit when such a loss is accepted under D66 (the first: nav_list_builder's
  0x7B7EF0).
* **A gain with no text change is not a gain:** a full verify once credited 10 pairs (wwaudio
  sound3dhandle) with no source change and a rerun gave the baseline: rerun before keeping such a gain. A
  single-file trial (`tryv2.py`, `zhall.py`) can report +1 for an address another file already holds: confirm
  with a full library verify.
* **The inline budget (/Ob2), measured:** each caller has a budget, raised by its own non-inline code. A
  callee costs its *source* size: dead code counts, which is why dead reads spend budget; `inline`, the order of
  definition, explicit instantiation and `auto_inline(off)` do not change a callee's cost (`auto_inline(off)`
  only removes it from the candidates), `__forceinline` ignores the budget. Top-level call sites spend in
  source order, so the last ones lose first; an expansion *before* a site takes the later sites' top-level
  budget; an inline call or dead read *after* a site takes that site's *nested* budget and sends its deepest
  nested expansion out of line, never the site's own top-level callee. Unused plain locals cost nothing; an
  unused *named* `size()` local costs like a dead read. Templates take the inline state at the file's END.
* **Budget: what the model explains (resolves "spend is forward only" against "spent from the end"):** a dead
  read after a site never changes whether that site's top-level callee inlines (the "forward only" reports);
  it does push that site's nested expansions out (reads after the last inlined site flipping earlier sites'
  `~RefCountPtr`, `_Eos` or Cast out of line; reads or assigns after the final `return`, unreachable, still
  count). Reads at the end do nothing when no site before them has a nested expansion left to cut. So a
  callee the client calls out of line at the top level means a missing source layer, not a budget form.
  Freed-budget trap: removing a spender or adding a layer pulls *later* code inline: judge the whole file.
  (Conflicting reports, unresolved: in some long functions the early sites lost inlining first.)
* **Dead spenders (D130) and sweeps:** dead `size()` / `length()` / `c_str()` / accessor reads, `str == ""`, a
  dead find, an unused type test or a user `operator=` assignment spend budget without code. Per call or
  per callee size: conflicting reports, unresolved. D130: at most 6 per function, each marked not the
  client's and each needed; a real form that spends the same budget (the client's own helper call, a
  qualified sibling call, an inline layer, an accessor) replaces them. The effect is not monotonic in the
  count (4 reads 0.91, 5 0.30, 6 exact) and depends on the file's other inlines: sweep every count, don't
  bisect, re-sweep after each structural change. Counts above 6 are diagnostics only (a parked function).
* **Later-defined same-file functions are inlined (/Ob2):** VC6 inlines a non-inline member or static free
  function defined *after* its caller in the same .cpp (a non-static one also keeps its out-of-line copy), and
  a new member added near an exact caller. Where the client calls it out of line, put `#pragma
  auto_inline(off)` / `(on)` on existing blank lines around its definition (the helper keeps its bytes; also
  for a destructor reached from `delete` and `delete []`), or define it at the client's position. The pragma
  does not reach in-class or `inline` functions (rtti.h's Cast) and does not stop Unknown_Find_Field's
  inlining. `#pragma inline_depth(N)` at a file's end changes every template member's inlining in the file:
  a lever, but check every marker (one file fell from 21 exact to 8); around one function, inline_depth(1..3) does
  nothing, but inline_depth(0) does act (it sends that function's template callees out of line, see below).
* **`__LINE__` moves with every line above it:** FAIL_IF / G_ASSERT / WWASSERT / WWDEBUG_SAY push `__LINE__`,
  so any added or removed line above them (a comment, a multi-line `// NOT EXACT:`, an `#include`, a deleted
  `#define`, a pragma, a helper, a shorter `#include` replacing a stand-in block, a trailing blank line from
  the Edit tool, a variant or marker a runner inserts) moves every later assert: later functions go `none`,
  variants show false losses and can look exact while losing later ones. In an EA-derived file a comment
  line above an assert loses its pair: E&B notes go in the end-of-file `// EnB:` block (D211). Keep the
  line count (techniques: "Keeping `__LINE__`" in §3.1.2; a member line added above data_binder.h's asserts shifts their immediates and explain.py then says `literal`, hiding the real class: put additions on an existing line) and check: `git diff --numstat`, then every
  FAIL_IF / G_ASSERT line against HEAD and the client's pushed line numbers, not only the pair count. A function that goes `none` after a `#define` or
  guard line is removed or added above it is this, not inline depth: compare the push immediates (`client.py dis`,
  our .obj) before looking for an inline cause; a blank or comment line in its place keeps every line.
* **Re-verify the whole file after a change:** shrinking a function (or wrapping a find in an early-return
  helper) can make it small enough to inline into callers that then stop matching; added code can make the
  file's other near misses exact (re-verify its NOT EXACT functions); a header with inline bodies, a changed
  macro (FAIL_IF's form) or an unused declaration in a widely included header (duration.h,
  delta_template.h) moves the budget in files that never use it; even a body-less member declaration in a widely included class can move a stack slot in a function that never names it (prefer a `friend` declaration for an app file's access to protected members). Try such edits in a draft and run a full
  verify; compare pairs over every file that includes the header. Whether a header HAS an inline function at all (not
  its name) can flip register allocation in other files that include it (a forwarder removed from ui_mesh.h
  changed one constructor's registers; so did removing an accessor): `appfile.py match` compiles a draft apart
  from the tree and can say exact where the tree build is not, so the verify decides.

#### 3.1.2 Look up by symptom

Find the row for what `explain.py` or the diff shows; the bullets after the table add detail per class.

| Symptom (what the diff or `explain.py` shows) | Lever (the source change that fixed it) | Proven (one address) |
|---|---|---|
| **inline budget** | | |
| A callee called at one site, inlined at another (or the reverse); a big constructor stops inlining partway | The budget model (§3.1.1): fewer `Add`s anywhere, dead IL in a callee, an accessor instead of a same-bytes field read, a `SNAPSHOT_SAY` (§3.9) move where inlining stops. `#pragma inline_depth` doesn't reproduce it | 0x700B10 in Ship 0x6F66E0 |
| A header macro change (D165's `&&` FAIL_IF) makes callers inline more, or less, than the client | Put the old cost back with one dead inline read at the right spot (`text.size();`, `WWAudioClass::Get_Instance();`), or one more implicit conversion among explicit ones; a `return` that became `break` can move a merged return block | co 0x6FFFF0 |
| The client inlines `~list`'s `erase(first, last)` but calls `it++` / `erase(it)` out of line on a FAIL_IF return path | Two dead `size()` reads on the copied list on that path | |
| The client calls a callee out of line (an insert's last `size()`, string::erase) that ours inlines | One or two dead inlined reads *after* the call site (its nested budget, §3.1.1); only the first dead read inlines, the rest are called | `DeltaCompoundClass::Add` 0x4A5670 |
| Dead `size()` reads at the function's own top level change nothing | They spend only inside an inlined callee: a find-if-set wrapper holding 1-2 reads; their position matters (one before the loop and one after: exact; two after: 0.81) | ui_group 0x598370 |
| Inlining differs although the caller's own code is the same | The budget counts loops inside inlined callees, even an empty release `WWASSERT` loop; empty `WWASSERT`s and dead loops in the caller itself count for nothing | `Process_Foreground_Load` 0x949C60 |
| A `Transpose`'s `Vector4()` through `??_H` at only one site, every stack slot moved | Tiny accessors tip the budget: `view[r][c]` (two inline `operator[]`) against `view[r].X`; `Vector3::operator[]` spends, `.X` does not | `PointGroupClass::Render` 0x954960 |
| Callers that inline a loop function call others out of line | `it++` spends budget `++it` does not (also on list iterators) | Release 0x826870 |
| An out-of-line copy right after its first caller; a member inlined at every site yet emitted out of line | An inline function not inlined there: make it `inline`, or a non-inline member defined after its callers (§3.1.1). An EA `WWINLINE` the client calls out of line moves to the .cpp | 0x6FA150 |
| Stack slots swap in a function you didn't touch | Inlining depends on the whole file: an unrelated edit moves it. An emptied function (`#if 0`) gets inlined: callers matching the other copy's call point at it | 0x6FB1E0 |
| Many callers missing, one inline to blame | Code compiled out in ours: restoring `Is_Disabled`'s `#ifdef G_CODE_BASE` matched 13 callers. One out-of-line `operator==` can hinge on the condition (`!=` keeps it out) | |
| A helper the client keeps out of line with no caller | A global, not a static (statics vanish when inlined) | connect.cpp 0x821890 |
| A callee the client calls out of line only inside an inlined function (nested table helpers, a node construct, a `_Construct`), inlined at the direct sites | The depth lever: wrap the call or the loop in a one-line inline helper (one more layer); the budget drops the deepest nested expansion | land 0x46DA70 |
| Many repeated inlinable calls all out of line in ours, the budget spent on later expansions (the depth lever's reverse) | The repeated calls sit in an inline helper: write it as a macro, and they inline again; a whitespace skip inlined at some sites only is a macro too | Render 0x474B30 |
| The client keeps a nested `_Construct` out of line | `std::distance(begin, it)` spends the budget that `it - begin` doesn't | `Remove_Dead` 0x822B50 |
| An empty inline wrapper around a find | Enough to send the find out of line, but the freed budget lets later callees inline | ui_group 0x596770 |
| A small function whose client calls the *first* case's helper out of line (one `_Eos`) | A D130 spender after the last `return` (unreachable) where one at the top fails | ui_manu 0x5B6060 |
| Nested inline temporaries flip (the sixth `??_H` Transpose temp) | An inline cost placed *after* an expression (`Matrix4(view)` in a restore) decides whether that expression is inlined | 0x9961E0 |
| `std::string(const char *)` shows three inlining profiles in one file | Budget, not source: don't read the profile as a different form | tutorial_manager.cpp |
| A hashtable's `_M_delete_node` out of line in the client, inlined by ours | In a template `p->~T()` on a function template's parameter gives `push 0; call ??_G`, the class template's own `_Val` a direct `??1`: libs/sgistl destroys through a member (`__p->~_Val()`) | kv_list 0x4B6520 |
| A hashtable's find called out of line, the key's string constructed inline; an SGI result the client converts through `const_iterator(const iterator &)` (a 0x13-byte copy) | `const_iterator it = T.find(std::string(name)); return it == T.end() ? NULL : (*it).second;`: the `const_iterator` local is the essential part, also for int keys (miss returned first); not for `const char *`-keyed tables. Table-level changes don't help. Not universal: one lookup the client inlines needed a plain iterator with `it == end()` tested first | ui_line_display 0x5A8B60 |
| The client shares one report call between two FAIL_IFs over an iterator | A `const_iterator` plus `key_ptr++` on its own line | Load_Keys 0x71BD90 |
| A caller's destructor gets extra EH states (7 -> 11) | Defining an out-of-line class's destructor in the same file inlines it into the caller's: leave it declared only when its bytes occur at several client addresses | land.cpp |
| A virtual `Set_Step` (or same-file base slot) the client inlines | Call it qualified, `LoginTaskClass::Set_Step(...)`; a qualified sibling call also adds one inline level | tsk_login.cpp |
| An aux-data lookup with `G_ASSERT(t)` inlined in many callers | A template helper `Unknown_Find_Field<T>(root, name)`: `NameClass name(...)` a local in the caller, the lookup a separate statement, no ternary | co.cpp 0x6F5E40 |
| An aux-data type test (rtti.h) inlined for some fields and called for others | A class-static `UnknownRTTIClass<T>::Cast` (a function template with an explicit argument hits VC6's one-instance bug); a cast nested three deep exhausts the budget at the first expansions, two deep at the last; then dead reads (§3.1.1) | co.cpp 0x700400 |
| An EH-tracked RefCountPtr local declared at the top of the function | Sends Find and `_Eos` out of line as the client has them (block-local: both inlined) | fontmanager 0x6D2700 |
| A large constructor loses a neighbouring inline after a header layout change | A budget knock-on, not the changed statement: objdiff for a call that replaced inlined code first | tsk_login 0x767240 |
| A string compare pushed out of line by a C-style reference cast between RefCountPtr types | Cast through a pointer (the reference cast builds a temporary) | cpacket ITEM_BASE 37 |
| The client expands std::string assign in some of many temporaries | Mix explicit and implicit constructions: the split tunes the budget | ui_cpit |
| Which helpers an out-of-line insert calls | A probe, not a shippable form: a non-inline same-file helper holding the push_back emits exactly those | Get_Triangles |
| The client calls a `hash_map()` constructor out of line with the hashtable constructor inlined in it, ours inlines it | A missing inline layer, not budget: a struct that derives from (or holds) the hash_map and has an implicit constructor sends `hash_map()` out of line (gives the client's frame too) | LookupTableSetClass::Load 0x4EC7D0 |
| One `hash_map()` is flat where the client calls it out of line, but calls `_M_initialize_buckets` out of line where it is inlined into a large owner constructor | Two inline layers of the same source: no header form gives both (a flat `hashtable(n)` gains the first and loses the second; `m(100)` initializers spend the owner's budget). Leave sgistl as it is; attempts row 0x71DCE0 | ckm 0x71DCE0 vs OptionClass 0x7F0F60 |
| Temporaries' stack slots or flags differ, and they follow the inline decisions; arguments built from `const char *` | VC6 builds an implicit-conversion temporary before the explicit ones in one argument list: write the string arguments as implicit conversions (only that form gave the client's order, slots and flags) | ClientClass constructor 0x726100 |
| Dead reads change later sites' inlining in narrow steps, and a position change does nothing | They act by their total and the callee they inline, not by position (before the function's last branch): sweep every count (7 exact, 8 over); an inline accessor costs about two thirds of a `size()` | client_lego_impl Add 0x72B7F0 |
| `if (a && b)` against two nested `if`s: the same bytes, but callers inline differently | The cost is in the source's structure: try both forms when a caller's inlining is off | unknown_6af220.cpp |
| A wrapper template calling through a second template | Costs one inline level (§3.1.1): count it | signal_list.cpp |
| A static helper defined later in the file is inlined into earlier callers; the client calls it | `#pragma auto_inline(off)` around the helper. The client's callers are the evidence (`client.py callers`: a function with no callers in the client is always inlined there, keep it outside the bracket); definition order is not an inline barrier in VC6 | steering `Predict` / `Avoid_Obstacles`; damtype's section reader |
| The client calls a function out of line from one caller but its other callers inline it | Try `auto_inline(off)` only after an `appfile.py match` over those other callers shows they still inline it | avatar.cpp |
| The client inlines `std::string` assign at the later call sites of a long function, not the earlier ones | The budget ran out inside the function: classify inline-budget, park | ui_starbase 0x637130 |
| A callee taking a by-value `Vector2` stays out of line as in the client | Keep the by-value parameter (a const reference inlines it) | 0x543200 |
| A float max the client compiles as `fld X; fld Y; fcompp` | A double compare: `(double)X > Y`, or an inline float-returning getter on both sides; plain float compares give `fld X; fcomp Y` in every order | mv3dsnd |
| **register-only, order-only** (two attempts, then the stop rule) | | |
| A constant loaded before a variable, or the reverse | Operand order: `elapsed * 0.05F * K` loads the constant first, `step = elapsed * 0.05F; step *= K;` loads `elapsed`; `a == b` vs `b == a`, `Pos > size` vs `size < Pos` differ (D18/D28) | |
| Registers differ around a call or an argument | A temporary, a local copy of an argument, or a member / id read into a local before the call (`int id = Client->X; Packet p(N, id);`); one shared variable reusing a dead argument slot. `tools/vc6/trydiff.py` keeps a variant's object for `objdiff.py` / `explain.py` | Accept 0x82A6C0 |
| Register-only: the client loads the called object after the arguments | A named local for that object (`RsTrackClass *track = ...; track->X(a, b);`) | cpacket 0x737610 |
| The client loads packet fields, ids or a member chain before the argument pushes; a member pushed before `mov ecx,eax` after `new` | Read them through (file-static) inline accessors (`Get_Avatar_ID()`, `Unknown_Player_ID(Client)`); `View_Of(this)->f(args)` loads ecx before the arguments | cpacket handlers 32-35 |
| A different frame slot for a helper object | A temporary (`X().Unregister(p)`) gets another slot than a named local | memconn.cpp |
| Registers of an inlined `DynamicVectorClass::Add` on a global | The global's linkage (file-static or not) orders them; or a `DynamicVectorClass &` local | |
| A register mismatch on a converted argument (`ntohl(*(int *)p)`) | Assign twice: `len = *(int *)data; len = ntohl(len);` | tcpconn.cpp |
| A virtual call in a destructor | `((Base &)*this).X()` | dbgconn |
| The prologue loads the arguments' members in the wrong order | A temporary or reference local that reads the member first: `const Vector3 &min = boxtest.SweepMin;` | Cast_AABox 0x465030 |
| `[base + index]` operands swapped in a store | `mask[row + i]` with an unsigned row offset, not `row[i]` through a pointer | land's sea mask |
| A pointer kept in a register across two member stores | An inline setter on the class; a local pointer for two stores through one member gives one load | ui_manu 0x5B4850 |
| A direct member store or load scheduled elsewhere | Store through a local pointer to the member: `RenderStateListClass *list = &RenderStateList; list->Current = x;` | rs_track 0x460940 |
| ebx / ebp saved up front in the client, after the early exit in ours | Declare locals at first use and return per branch | Play_Audio 0x806BD0 |
| The client's zero register (`xor ebx,ebx; cmp reg,ebx; push ebx`) | Write the `= NULL` store in both branches | ui_scut 0x612430 |
| A per-branch pointer reload; one shared indexed load before two calls | `(font = style.Peek())->X`; `if ((p = a[i].Peek()) != NULL)` | ui_cpit 0x5845A0 |
| The client still indexes `array[index]` after testing `index == 0` | It tested the member the local was copied from, not the local | ui_group 0x592A80 |
| An order-only residue in a scheduled store pair | Model the 8-byte member as two ints | opcode 108 |
| A class template in a header struct member moves registers (`TPoint3D<int>`) | Define such structs in the .cpp after the affected functions | fontdatabase Load_Init |
| Calls in a `+` chain evaluated right to left | Separate `size += ...` statements | Get_Data_Size |
| A whole function's registers off in a strtok loop | One `char *token` for every strtok result | Load_PGM 0x4637C0 |
| Index scheduling off where ours expands same-file accessors by hand | Call the client's accessors (`Get_Height(i, j)`): they fix the scheduling and spend the budget the client spends | heightfield 0x464600 |
| A clamp: the client's `movsx` and compare | Through an int local: `v = x; x = v < 6 ? v : 6` | avatar_desc |
| `end()` read through the wrong register in a vector append | `push_back` and `insert(end(), x)` differ: try the other; also `pop_back` vs `erase(end()-1)` | Flush_Lines |
| A repeated null test of a literal / 64-bit memory value: the client re-tests, ours threads it | Unsolved stop signal (VC6 SP4 value threading): after `if (x == C)` on a register local a later `x == K` folds on every path through copies, unions and no-code statements; the test survives only for relational compares, undefined values or real code between. 8 header and call forms plus a 30-flag sweep changed nothing | ShdMesh ctor 0xA08F80 |
| A float read through an inline getter goes through the FPU, a direct member read uses integer moves; an x87 `fst` / `fstp` double store | The product written twice in the temporaries' constructor arguments gives the `fst` / `fstp`; use the getter or the member read the client's bytes show | eff_seek 0x6F1C40, debrsint.cpp |
| An order-only `fild` residue next to two float locals | An unsigned frame time converted once into a named float, after the other float local | user_click_indicator.cpp |
| Inlined file-static helpers get the client's registers only with the new value as the first argument (old value in ecx / esi) | Argument order of an inline helper moves registers: try both | recustomization_cost.cpp |
| **shape: returns, tails, loops, switches** | | |
| test/setne against neg/sbb; shared or separate `return false` tails | `if (g()) return true; return false;` gives test/setne, `return g();` neg/sbb; a shared `return false` tail is `if (p) {...; return true;} return false;`; an eax-wide 0/1 is `return(!a() && !b());` | |
| A plain `sete al` | `if (f() == 0) return(true); return(false);`; `return(f() == 0)` doesn't | |
| `setne` / `test` on a compare result | `bool changed = a != b; if (changed)` | ui_manu 0x5B67C0 |
| The client repeats a success tail (`Handle = 0; return true` three times) | A return inside each success block (one tail, an `ok` flag or `&&` chains merge it) | `Try_Connect` 0x8290A0 |
| Ours merges two failure tails the client keeps | Nest the first case's success path (`if (x != 0) { ...; break; } return(false);`); early `return false` when `find()` returns `end()`. Still open: `Listen` 0x82A470 (`attempts.py list --blocker shape`) | 0x826D00 |
| An early `if (x == NULL) return(false);` keeps its own epilogue where the client shares the FAIL_IF exit | Wrap the rest in `if (x != NULL) { ... } return(false);` | scriptedmovie 0x807600 |
| Two FAIL_IFs sharing one exit | Nest them as `{} else {}` | attrib_dir |
| A duplicated WWPROFILE destructor test at a branch's end | An early `return` in that branch; a guarded reporter matches only as `if(!cond) return` | land |
| The client leaves a loop by a jump to the single return | A `goto` to the single return | `Get_Log_Directory` |
| `jb top` against `jae exit; jmp top` | A for loop gives `jb top`; `jae exit; jmp top` is a do-while behind a count test | the array templates |
| A pump loop duplicated / rotated | `while (true)` where ours had `for (;;)`; a loop with a mid-body break rotates as `for (;;)`, not as `while (true)` | tcpconn.cpp |
| A near jcc where a short one fits in an "if NULL create; Add_Ref; return" getter | Return inside the creating branch too | proj_man 0x492290 |
| A strtok path walk with one strtok call at the loop top | `while (name) { next = strtok(NULL, sep); if (!next) { last step; break; } ...; name = next; }` | rs_track |
| `push_back` keeps 0 in a register and calls insert out of line | `push_back(T(x))`; three field stores into a named local don't | |
| A copy walk, a getter or a map entry from control_keys_manager-style code | `push_back(&*it)` for a `vector<T*>` fed from `vector<T>::iterator`; `M &m = map[k]; m.Init()`; `end()` tested first in a find-returning getter; a copy walk over two iterators where indexing misses; the cleared-key case returning first; a struct with its own constructor where an aggregate initialiser misses | control_keys_manager.cpp |
| The client materialises `begin()` before a hashtable loop | Declare the iterator before the loop: `T::iterator it; for (it = X.begin(); ...)` (per function, not per file) | inpdsp 0x429800 |
| A caller that expands a find loop matches only with `x.end()` re-read each pass | Don't cache `end()` in the loop | ui_group Add_Member |
| `mov eax,key; dec eax; je` | A one-case `switch` with the default written first | ui_scut 0x610A40 |
| A 4-argument `_Sort_0` call (push 0; push a garbage byte) | std::sort with a comparison functor; `operator()` giving `mov eax,1` returns int | ui_scut 0x6129C0 |
| An out-of-line loop function also inlined one level into its callers | A self tail-recursive `X_Rest(p, left)`; a plain `while` gets fully inlined | boolpars |
| EH state numbers or the bigger inline expansion on the wrong branch | Lay out `if (a) X else Y` with the client's then-branch | ui_chat_panel 0x563910 |
| A word load, `shr`, test-1 sequence on flags | bool accessors on bit fields (the "Bit tests" bullet below) | inpdsp 0x42A410 |
| A bitfield test compiled as `shr` / `test cl,1`; `if (!Is_Active())` loads `al` first | An explicit `(bool)` cast on the test; `if (!Active)` (the member) compares the member instead | ui_dynamicwindow.cpp |
| `if (a \|\| b) {X} else {Y}` lays Y first while the EH states follow source order; `while (flag)` gives the unrotated loop | Known layouts: judge by the bytes before choosing the form; a bool helper written `if (find != end) return(true); return(false);` gives `je` / `mov al,1`, `return(find != end)` gives `setne` | unknown_6af220.cpp |
| A switch whose cases push the full argument list and share one call | The client has a call per case with a `break`, not a shared tail | user_click_indicator.cpp |
| `setne dl; mov [esp+N],dl; mov eax,[esp+N]; push eax` before a call taking a bool | A named bool local: `bool b = expr; f(b);` | shiputils 0x80F470 |
| `test al,al; setne al` after a bool call, or a bare `setne` for a byte / int compare returned as bool | `return x ? true : false;` / `return (a != b) ? true : false;`; `!!x` does not give it | ui_starbase, recustomization_cost.cpp |
| A loop helper whose first call is peeled (do-while, `for(;;)`, goto); the client loads a constant on one exit | Only `while (true)` with a return at each exit keeps the client's loop; the constant on one exit means two return statements, not one after the loop | avatar.cpp |
| The if / else block order is swapped | A `goto` into an if-body, or a `bool ||` guard, swaps the blocks: avoid both | emote_data.cpp |
| Several "return zero" paths | Each is a separate early return: Ghidra's nested if-structure is misleading | steering (unknown_445740.cpp) |
| A loop bound compiled to a signed `jl` against an unsigned count | An `(int)` cast on the count | |
| A `box.Extent` read the client does through one pointer (`add eax, 0xC`) | `const Vector3 &extent = robj->Get_Bounding_Box().Extent;` | eff_pass.cpp |
| A callee returns int in eax but a caller reads `al` as bool, and declaration and definition will not link | C linkage (`extern "C"`) on both | distance_calc.h 0x831F90 |
| **frame** | | |
| Each inlined copy has its own buffer slot | A function-scope buffer gives each inlined copy its own frame slot | 0x752E60 |
| The frame size differs | The frame counts every local: a difference names a local too many or too few (a second RECT: +0x10; `char name[MAX_PATH + 1]`: +4) | 0x9182C0 |
| A temporary for an `Add()` argument grows the frame | A cast temporary instead of a named local | opcode 128 |
| Locals share dead parameters' stack slots in the client | Declare the locals at the top of the function, not in a later block | udpconn.cpp |
| Switch cases with frame slots of their own | Temporaries, not named locals (named locals share one slot) | ui_cpit |
| Two NameClass locals sharing one stack slot in the client | Put each inside an inline helper; two block-scoped locals get two slots | ui_manu 0x5B3640 |
| A frame off in a loop over a computed index | A hoisted invariant in a named local: `int slot = Bar*3+1;` | ui_scut 0x611300 |
| Our loop hoists addresses into stack slots the client doesn't have | Move loop-invariant inline math (a Matrix3D multiply) out of the loop | 0x9BEB20 |
| Index-building loops: the client keeps a running row base | `int row1 = 1; ... row1 += nx` with the Adds in the client's order; also makes the static guards exact (`mov cl,al; or cl,N`) | 0x9961E0 |
| A frame 0x30 too small; int-move copies | A Matrix3D local (its user copy constructor) | client_lego_impl |
| A 12-byte local holding a translation | `Matrix3D::Get_Translation(Vector3 *)`, the out-parameter overload | 0x479A50 |
| One-byte temporaries where the client passes an empty tag type by reference | A fresh temporary per call | co_data |
| A float member initialised before the next unwind state | It belongs to the inline member whose constructor initialises it, not to the caller's frame | ui_power.cpp |
| **constructors, destructors, vtables** | | |
| A destructor that is a lone `jmp` to the base's, or one that stores the vtables first | The compiler-generated destructor stores no vtable; a user-declared `virtual ~X() {}` stores them first (and adds an EH state to its owner's destructor): declare one exactly where the client stores | 0x4AA210 |
| Members stored before or after the vtable | An init list stores members before the vtable (in declaration order), body assignments after it; the base's vtable store comes first and is dropped | 0x4A8370 |
| A base vtable store that survives in the client | A member initialiser reading through a reference that may alias `this` | pkunknown 0x888B60 |
| Virtual slots in the wrong order | Overloaded virtuals are laid out in reverse declaration order; a same-named overload of a base virtual takes the slot right after the overridden one | dbgconn.cpp's `Describe` |
| A base function kept in a slot plus an empty own slot (`ret 4`) | A changed base signature hides the virtual: the derived overrides became new virtuals | 2f9bb70 |
| `Value(...)` stores into a +4 subobject that keeps its own vtable | A member object, not a second base | boolpars |
| A base's sub-block destructed in the wrong store order as plain fields | The block is a struct with its own inline destructor | client.cpp +0x0C |
| The destructors that expand a member struct's destructor gain an EH state | An empty user destructor on the struct: drop it | ui_group |
| An out-of-line "constructor" called through `??_H` without `mov eax, ecx` | A default-constructor closure: give the constructor default arguments | 0x71BD70 |
| An out-of-line 16-byte copy (`mov eax,ecx; ... ret 4`) | An inline user `operator=` the budget cut: declare one | 0x80B100 |
| A derived destructor stores its base's vtable last and its members' EH states are shifted by 1; or the client's implicit destructor stores no own vtable | A base class with an empty inline virtual destructor adds that state and store (as does an empty user-declared one on the class itself): when the client shows neither, declare no destructor | SelectorClass; PassEffectClass |
| **field offset** | | |
| `[reg+N]` differs in otherwise equal code | The displacement can belong to another class: trace the register before patching; `tools/re/layoutdiff.py` tallies offsets per class | |
| A `malloc`'d / `new` size larger than the class | Extra E&B members, or a stray `#pragma pack(4)` | `FastAllocatorGeneral` |
| Several members reached through a pointer to a sub-block | Those members are a struct | pkunknown 0x888B60 |
| **literal, floats** | | |
| One `exp` call where ours has two (or the reverse) | Two `(float)exp` of one argument merge, `expf` doesn't | |
| `fild qword` (high dword 0) + `fidiv dword` | Unsigned `(float)u / (float)v`: such parameters are `unsigned` | |
| `mov dword [..], 0` among register-zero stores | That member is a float | |
| A float constant loaded from memory (or divided at run time) where ours folds | A named or function-scope `static const float` (VC6 does not fold it) | `Prepare_Lighting` 0x46C730 |
| A float copied through the FPU (`fld`/`fstp`) where ours uses `mov` | The value goes through an inline setter's parameter or an inline getter | soundlib |
| `v*(p*k+1)` folded where the client computes `v + (v*p*k + a)` | Spell it as the client computes it; read the old value before calculating | attrib_dir |
| `fmul dword` where ours has `fmul qword` | `float * literal` folds to the double: `DEG_TO_RADF(x)` or `(float)atof(p) * k` | 0x806AF0 |
| A static `__int64` initialised at load time | `static __int64 x = 5;` gets a dynamic initialiser in VC6 | ui_manu 0x5AF9D0 |
| A `static const int` read from memory where ours folds it | A class static defined after its uses | boolpars |
| **reading the client** | | |
| `ret 8` / `ret N` with ecx set, or never read | Still a member function, even if it ignores `this` | |
| A body equal to two later functions | Both are inlined into it | Process_Messages |
| 0x9CCD10 (`xor eax,eax; ret`) in a vtable | A pure virtual slot: `= 0` | |
| A static function right after an unrelated one (misplaced start) | A static sits right after its first out-of-line caller | 0x4B43E0 |
| A helper the client calls out of line from one site | A file-static helper defined after its caller | 0x872E30 |
| An unreferenced client function next to a not-exact caller | The out-of-line copy of a non-static helper the caller inlines: define it non-static, non-inline in the .cpp; a callerless function beside siblings inside `auto_inline(off)` may be such a copy of an everywhere-inlined one: keep it outside the bracket | land 0x46EE60 |
| C2660 on wwlib vector.h's placement `new[]` | Declaring `operator new[]` in a file makes VC6 resolve it there: declare the allocator's members instead of including FastAllocator.h | unknown_new.cpp |
| A direct call to a client virtual where ours goes through the vtable | Qualify it: `client->ClientClass::Add_Object(...)` | scriptedmovie |
| A call order or frame off around by-value arguments | `Get_Client()` inside the argument list; a named copy before a by-value argument; block-scoped temporaries | 0x808330 |
| A RefCountPtr released twice in a row | `REF_PTR_RELEASE(ptr)` | scriptedmovie |
| `G_ASSERT(!"text")` tests the string pointer | VC6 doesn't fold it: keep the form | cpacket |
| One inline source that fixes the inlined copy and breaks the out-of-line one | Expected: judge by pairs over both | 0x801D70 |
| `setne / test al,al` on a RefCountPtr test | `0 != ptr` (ref_ptr.h's template operator!=), not `if (ptr)` | client_lego_impl Init |
| `lea reg,[node+0x14]` on a table node's value | A reference local to the value: `T &v = (*it).second` | client_lego_impl |
| Two `sete` combined | A bitwise or of the tests: `(a == 0) \| (it == end)` | client_lego_impl |
| A second triangle's normal through TriClass | Set all three V pointers again before the second triangle | land Compute_Normal |
| Both sums of a Vector2i computed before the two stores; `lea` addressing | `Set(a, b)` on a reference local; member stores don't give the `lea` | land |

**Inline budget: more levers** (the model and the dead-read rule are in §3.1.1)

* **A function with no caller in the client is its neighbour's inline layer:** draft its body as that layer, not as a
  function of its own (land.cpp's Load_Prototype_Names). **Dead reads act by their total**, not their place: sweep the
  count of identical reads. **Your own statement mass moves the budget both ways:** statements added to a function
  that is itself inlined can stop an inline in its caller, and fewer can start one.
* **Levers that closed inline-budget and order near misses elsewhere:** step with `Next_Valid()`; an unused argument
  for a `ret N` mismatch; increments inside conditions; the object read into a local before the call; `clear()` for
  `erase(begin(), end())`; the member passed instead of the local; the client's own member as the missing inline
  layer; plain `RefCountPtr` elements instead of a one-member struct; an inline overload layer; dead reads in a shared
  inline constructor; a case's own copy of the next case's body.
* **An extra inline layer moves the budget:** one no-code inline layer (a handle method, a file-static
  forwarder, a qualified sibling call, a template like SignalMap's `connect`) leaves one more nested call
  out of line; writing `it._Inc()` out removes a layer. One layer left exactly the client's first operator=
  out of line, two left two. Try it before declaring a residue final; with 8+ dead reads after it the layer
  itself goes out: use the shallowest one. A layer acts on the callee one level below it, not the deepest.
  Where a wrapper around an STL call fails, try the other level. A helper the client inlines whose own
  callees it calls: route the call through the helper. Levers can work only together: try pairs.
* **A top-level callee the client calls out of line is a source layer:** spenders cut the deepest
  expansions, never a top-level callee: write the helper the client had (a file-local inline `Clip_Text`;
  about 20 budget forms failed first). `clear()` is one inline layer over `erase(begin(), end())` (vector
  and list); `x.clear()` through the class's own inlined clear member is another.
* **Shared static helpers when a family inlines both less and more:** stop spending budget forms; the client's
  candidate set differs. Traces: distinct stack slots per copy, a `set::begin() const` copy called while
  `_Tree::begin()` inlines elsewhere. Inside an inlined helper a set-walking `for` leaves `_Inc` out of line;
  `it = begin(); while (it != end()) { ...; ++it; }` inlines it (also costs less budget than the `for` header):
  try the while form on every inlined helper loop first. In a non-inlined body it++ / ++it, iterator /
  const_iterator, += / append(ptr) and for / while give identical bytes: no lever there.
* **A non-tail nested multi-return expansion before a conditional call sends a Cast out of line:** the unit
  that pushes a find's `C *t = Cast(d); G_ASSERT(t);` out is a nested inline with several returns in non-tail
  position followed by a conditional call; the same body flat or `return Cast(d)` in tail position does
  nothing. Two layers (find + mark test inside a helper, the NameClass built in the inner one, the root copied
  to a local) or one inline call after the helper reproduce it. A `char *` root is read through a
  `char *const &` cast in the inner layer; a root read after the NameClass needs the helper to take it by
  reference. `?:` between two inline finds expands both branches. Where the rule did not decide it:
  `attempts.py list --blocker inline-budget` (ui_missn 0x5C4C90, lookup.cpp 0x4ACA90).
* **Assignment as a spender:** `T x; x = T(...);` through a user-written inline `operator=` spends budget where
  the matching constructor does not: placed after a site, it cuts that site's deepest nested inline (a
  DurationClass assigned after a two-layer find sent Cast out of line, ui_mfd 0x5BE120). A struct with only
  the compiler's bitwise operator= spends nothing. Try it before D130 dead spenders. A large struct copy
  inlines only through an explicit inline operator=; the implicit one is called out of line.
* **Repeated destructor sites of one local:** when the budget lets only some sites inline, ours sends the
  textually first site out first; a dead read after all the sites changes which one goes out, reads before
  or between them do nothing. A client that picks a block-exit site may have a scope closing before the
  returns (untried). With repeated `??_H` Matrix3D copies in some blocks only, the budget runs out partway:
  dead reads at the top move that point.
* **`#pragma inline_depth(0)` around a function** emits the compiler's ??_G / clear copies out of line,
  byte-exact, where the client calls them, and keeps an inlined member ctor out of line; inline_depth(1..3),
  auto_inline(off), an out-of-class or virtual dtor or a member operator delete do not make VC6 call ??_G.
  inline_depth around a function did not reproduce out-of-line Casts (dead finds did). An explicit template
  instantiation inside an auto_inline(off) region keeps that template out of line for some types only.
* **Clearing forms inline differently:** `Set_Contents(NULL)` vs a `RefCountPtr<>()` temporary, and `clear()`
  vs `erase(begin, end)` (`??_G(0)` calls vs inlined destroy loops): try both. Top-level code empties a VC6
  vector with `clear()`; `vector::insert(end(), 1, x)` written out is inlined while push_back keeps the
  3-argument insert out of line. A hash_map node with an inline recursive dtor: `delete node` in both places.
* **`std::string` construction forms:** `name(p)` and `std::string name; name = p;` give different inline
  profiles; where the client stores the string's EH state before its inline assign, default-construct then
  assign. An inlined `std::string::find` needs `find(s, 0, strlen(s))` inside `while (>= 0)`.
* **A RefCountPtr retype flips operand order after a loop:** retyping a member to RefCountPtr can flip x87
  or `lea` operand order in a function that uses it only after its loop; the inline `operator->` is the
  cause and neither operand order nor dead reads fix it. A raw-pointer read of the member is the quick
  diagnostic. The ctor's EH states (one per member) show which members are RefCountPtrs: retype one at a
  time; a shared `RenderObjClass **slot` parameter forces every caller's member at once: verify the group.
* **Inline copies come out in definition order:** VC6 emits pending inline copies in source definition
  order: define nested constructors before their root for the client's depth-first order. Of several
  inline copies of one function the last is the most inlined. A function emitted out of line from one
  class but inlined from another is two COMDATs in two objects: check which TU owns the client's copy.
* **Budget fingerprints:** `??_H` with the out-of-line Vector4 ctor 0x468E70, or a function the client inlines
  elsewhere, means a budget residue (int3 padding where helper copies would sit is no trace under the
  incremental link). A helper shared by two functions would not inline even with `__forceinline`
  (unexplained against the model). A dead `size()` in a function inlined elsewhere spends at every inlined
  copy; a set-never-read local bool spends only where it stands. The budget is not a plain source-order
  budget for string ops: one client function inlined later string ops and called earlier ones (0x5615C0).
* **A function inlined into a not-exact neighbour:** a client function with no code callers whose bytes match
  part of a not-exact neighbour was inlined there: call it from the neighbour. An unreferenced .cpp member
  (`python tools/re/unref.py FILE`, D195) that VC6 auto-inlines is often the layer that leaves nested
  helpers (??_G, clear, erase) out of line; a member whose inline twin is "own begin / ++ called out of line"
  is such a .cpp function: define it out of line. Inside an auto-inlined member, a `delete` in a file-static
  inline helper gave the client's out-of-line ??_G / erase. Functions with no callers are still emitted.
* **Keeping a later-defined helper out of line, other ways:** an empty `for (int i = 0; i < 0; i++) {}` in the
  callee compiles to nothing and stops auto-inlining (less reliably than the pragma). A helper the client
  inlines at one site and calls at another: keep it out of line and repeat its body (or an inline twin) at
  the inlining site. For an out-of-line copy of an always-inlined template (Unknown_Find_Field) to compare,
  write a dummy caller with 40+ calls.
* **STL call forms move what goes out of line:** `push_back(x)` keeps `insert(P, n, x)` out of line,
  `insert(end(), 1, x)` inlines it; `std::swap(a,b)` stays a call where `a.swap(b)` inlines; `clear()` against
  `erase(begin, end)` decides whether a struct's implicit operator= goes out of line and changes inlining
  elsewhere in the function; a `static inline` wrapper around `vector::erase(begin, end)` or `list::clear()`
  puts exactly one more callee out of line; a `std::queue` member leaves two of push_back's three
  `_Construct` calls out of line; `resize(n)` whose temporary lives to the function's end is `T blank;
  v.resize(n, blank)`; a find taking its value by value is `std::find_if` with a one-field predicate struct.
* **String appends and set / sort forms:** `.assign(p)` against `= p` moves a budget limit by one call;
  `s += x` against `s.append(x)` decides whether a later `+=` inlines; a spend before a second append pushed
  the first append's `_Eos` out. A set's destructor has three forms in one function (~_Tree out of line;
  inlined with `erase` / `begin` out of line; fully inlined) chosen by spends elsewhere. An empty default ctor
  on std::sort's element struct switches `_Sort`'s form; a user copy ctor or operator= pushes `_Median` and
  the partition out of line. A sort helper differing only by registers: read both keys into int locals.
* **Codeless layers and dead calls inside a callee:** one codeless inline layer can fix two budgets at once (a
  nested callee inlined in one caller, a sibling keeping only std::copy out); a ctor's `_Buynode` out of line
  with `_Init` inlined came only through two layers at every site; one layer around a find plus many no-code
  finds gave "find out of line, Get_Value inline" (over D130: diagnostic). Dead no-code inline calls inside a
  callee tune its inlined copy in callers (7-9 kept a call out, 10 broke it). These match bytes; they are not
  evidence of the source.
* **Accessors as spends:** trivial inline accessors can move the budget more than dead `size()` reads (one
  function exact only with `Get_Filter()`, another 317 -> 109 rows with three getters), though they spent
  nothing in other lab cases: test per function and check every marker. An inline getter (`Get_Flag(bit)`) can
  push the client's other callees out of line; naming a member's intermediate results as locals spends budget
  where moving its definition later does not. Since the budget grows with the caller's own code, dead
  statements at a function's end let earlier sites inline; `clear()` vs `erase(begin, end)` can let an
  uncalled function at the auto-inline limit inline.

* **A dead read inside the callee sends the callee out of line** (ui_inv.cpp, UIInvClass::Unknown_5A0740's
  helper): the dead-read rule of §3.1.1 also acts inside the callee, not only after the call site.
* **A file-local `static` the client keeps as a function but every caller inlines is never emitted:** the object
  then lacks the function. Give it external linkage.
* **Inline inside inline:** one caller calls a function out of line where another inlines it when the first caller
  inlines an intermediate helper that holds the call (one inline level deeper, VC6 keeps it out of line): rebuild
  that intermediate as its own inline function. A wrapper the client inlines can be closed by rewriting it as a
  layer over its siblings; an out-of-line `map::find` inside an inlined `Find_Child` comes only from calling
  `Find_Child` itself; call the client's own helper functions (an rtti `Cast`) so nested calls stay out of line.
* **A destructor's inlined helper whose callees all go out of line:** write `Clear(); X.Invalidate(...)` (the inline
  after the call site spends the budget); an SGI `const_iterator` conversion the client calls out of line needs an
  inline layer, a file-static helper holding the find and its test. Landing a member body can make a same-file
  caller inline it: re-check the callers after the landing.

**Register-only, order-only** (two attempts, then the stop rule)

* **The operand VC6 reads first decides its register:** cache it in a local declared first, or read it
  through an inline accessor (also a member pointer that becomes `this`; an entry pointer `entry = *it`
  before several `raw->Add` calls). `float t = a; t += b;` gives a load order `a + b` and `b + a` both miss.
  Dead definitions fix operand order too: lea order follows the first definition (`nbytes = ebytes = 0`).
  Which operand of a commutative add loads first can flip when an unrelated expression changes: re-check
  every compare after each edit; an AABox reference bound from an if / else (not a `?:`) loaded in the
  client's order.
* **Argument types and by-value structs:** `Add(..., char, char)` with `movsx` arguments takes `char`, not
  `bool`; a flag tested then flipped with `test ecx,ecx; sete` is an `int`. A client `sub esp, N; mov eax, esp`
  before a call is a struct passed by value (Vector3 / Vector2, a DurationClass with a user copy ctor), even
  where a separate-floats callee has equal bytes. A `Vector3` member copies register-wise, a `TPoint3D` as a
  structure. A by-value struct argument is copied from the caller's stack copy.
* **Zeros in registers:** with many integer zeros VC6 caches 0 in ebp / ebx; `money = MoneyClass()` gives
  `xor eax / xor ecx` and stops the caching; a float-typed struct where the client has one cuts the count.
  Floats initialised to 0 store an immediate 0, ints the zero register. A zero that seems cached in ebx
  through a loop may be a loop index from 0: look for `inc ebx` before chasing registers. `delete p;` without
  `if` keeps immediate 0 stores. VC6 propagates every local constant: a bool / int local never reproduces a
  `push ebx; mov ebx,1` before a jump table.
* **Branch-local loads and pushes:** an inline accessor (`Peek_Model(Button)`) reproduces a model loaded in
  each branch; an `if` / `else` on a bool moves pushes where `?:` does not; an else-if re-reading a member
  avoids a hoisted load where a bool local does not; a static const stops a member load hoisting above a
  branch. A reference local to an array element schedules member stores late: write `Channels[i].X`. One
  Vector3 local shared by several Set_* calls changes a later temporary's slot. The reverse also occurs: a
  loop whose client base pointer sits at a later member wants `UIMountPointStruct &p = Points[i];`.
* **More register levers:** a reference parameter copied into a plain local keeps ecx = `this` across a
  thiscall (`unsigned int key = id; Find(key)`); a call to a later-defined member inlined under /Ob2
  evaluates its arguments before the inlined test; buffer fills VC6 reorders go through inline pointer
  helpers (`Put_Long(p, v)`), and the order of two member stores can pick the slots behind them; a Vector3
  result passed member by member is `Vector3 c = f(); g(c.X, c.Y, c.Z);`.
* **Register side effects of spenders and accessors:** a D130 spender written
  `*(NameClass *)&param` takes the parameter's address and moves registers, `*(NameClass *)this` does not;
  an inline `operator->` on a member of a call result leaves a dead `add eax, off`: a block-local pointer
  (`SClient *client = ...; client->ChatPanel->f()`) removes it; a member copied to a local lets VC6 thread
  its tests; a pointer-keyed hash find that converts per find wants `unsigned int` keys.
* **A named local or pointer decides the register:** a 3-register rotation (eax / ecx / edx) goes away with a
  named local (`UIMFDClass *owner = Owner`) or an inline accessor around the read: sweep these first; a named
  local for one argument of a 3-argument cdecl call swaps eax / edx / ecx; which pointer is named decides the
  register (`HeightFieldClass *field = ...` decided the object's and `&pos`'s). A local declared in the
  branch that uses it, not at the top, changes allocation. Reading a member as `Get_View()->Get_X()` (through
  eax) or `View->X` (ecx) differs per function: try both.
* **An inline accessor decides a register:** `mov eax,[ecx+N]` against our `mov edx,[ecx+N]`, or swapped eax /
  edx around an interface member: try `Get_X() { return X; }`. A field read before an out-of-line or virtual
  call in the same expression is an inline getter (VC6 loads a plain field after the call); an accessor in an
  argument list is evaluated before the other arguments. A rotation that diverges after a store came from an
  inline setter (`opt->Set_Value(x)`, not `opt->Value = x`); one from the first instruction (`this` in ebp,
  not esi) went away with inline getters.
* **Reference locals:** `mov ebx,[this+N]` before a branch and `add ebx,0x50` later is `T &data = *ptr;`;
  `mov eax,[p]; add eax,0x2C` before an inlined std::string ctor is `RefCountPtr<T> &r = p->Member;` declared
  before the string; `add eax,N; mov edx,[eax+4]; mov eax,[eax]; push edx; push eax` is two floats read through
  a reference local (`const T &p = g->M; f(p.X, p.Y)`), not a struct. `add ecx,0x50` after the argument load
  with `xor ecx,ecx` on a null path is an inline member of the returned class calling the sub-object's method
  (`p->Get_Level(x)`; `p->base.F(x)` gives `lea ecx,[eax+0x50]`).
* **Loop and walk forms that set registers:** `while ((node = ...) != NULL)` where a `for` re-reading its head
  swapped registers; a cached end is `for (it = begin(), end = end(); it != end; ...)`; a `for (;;)` counter is
  rotated and peeled, `int i = 0; while (true) { ... i++; }` keeps i in memory with the increment after the
  locals' destructors; a pointer walk with `lea` after the empty test and `mov eax,ebx; add ebx,4` is
  `array[i++]->f()`; an iterator constructed in place (not assigned later) stays in memory; a find loop loading
  `end()` before `begin()` is `std::find(begin, end, x)` with the not-found return first.
* **Bools and ints that look like bytes:** `mov al, byte ptr [esp+N]` on a parameter, or a flag the client
  stores with `movzx`, is `bool` (an `int` changes the callers); `sete cl; mov [slot],cl; mov edx,[slot]; and
  edx,0xff` is a named bool local passed as an int; `mov [esp+x],bl; mov ecx,[esp+x]` is `bool good = ...`
  passed to two calls; `mov edi,eax` with no movzx before `and ecx,2; cmp cl,2` is an int return and local
  though Ghidra says byte; `bit8` returned as `bool` shows as `setne`. The mangled name fixes parameter types
  (`...IED`: pass int fields uncast; a `(char)` cast makes byte loads).
* **Unexpected stores and stack values:** a self-store of a parameter (`mov [esp+0x3c],ebx`) marks an inlined
  by-value callee: a sibling is called, not repeated; two floats set to (0, 0) out of order were a Vector2
  with an inline ctor; a scheduling difference on a constant can be a sub-object (`Vector2 Size(-1, -1)`);
  `memset(member, -1, sizeof)` against a fill loop changes a whole ctor's registers; a local set to NULL
  outside a loop whose address is taken (push_back) keeps the client's dead store and frees the argument slot
  for the loop counter.
* **Store order against source order:** VC6 hoists a member store / load above an earlier member store where
  the client keeps source order: put those members in a sub-object with an inline method (its own `this` stops
  the hoist; reorderings of the flat class do not); constant member stores that must land after the EH-state
  store, or a bitfield read-modify-write before the vptr stores, are a member struct with its own ctor. A
  `std::string` member in the init list puts the vtable store after `_Grow` (assign it in the body);
  `Value = f();` in the body stores before a member's call, an initialiser after it. A flag stored before an
  unrelated load needed `volatile`; `float d = f(); entry.Distance = d;` delays `fstp` past later pushes.
* **Operand and argument order:** an order-only swap can come from unrelated header text (one new member in a
  widely included header flipped a function; D18, D28): swap the compare's operands at the mismatch and test
  across header states with dummy declarations. `co == Object` vs `Object == co` decides the order
  (`std::find` with `operator==(T *)`). A byte-identical `?:` with swapped registers can be argument order
  (right to left): swap the ctor's parameters. `odd = false;` then passing `odd` differs from passing `false`.
  The high words' order in `add` / `adc` over values returned through hidden pointers comes from a member
  `operator+` (a free one schedules differently). A field copied through a named block local, or a node built
  as `DataNode<T*>(NULL)`, fixed store-order residues.
* **Shrink-wrapping (late callee-saved pushes):** VC6 pushes callee-saved registers late (after early returns;
  the late ones popped first) only when a single branch region after the split uses them; two sibling branches
  using them push everything at entry. Return-value merging, `if (x != NULL)` wrapping, else-if chains and
  up-front locals did not change it in the open cases (htmlview Draw_Item 0x7767A0, Value_To_String 0x84C470,
  Play_Audio 0x806BD0), against the table row "declare locals at first use": prove a lever on a test
  function first.
* **A member read loaded between a table load and its test:** needs a named local, or the register order differs.

**Shape: returns, tails, loops, switches**

* **Shape forms seen once:** a shared tail after a switch merges a case's ending: one `return` per case keeps them
  apart; two `return(call())` statements give the tail-merged form; return by value gives a dead store of 0; `break`
  against `if` can differ by one jump target, visible only in the bytes; a derived-to-base reference conversion is
  null-tested (`test` / `je`), so pass the form the client passes; a strip width written as a conditional
  expression in the call argument.
* **Bool returns:** `return x ? true : false;` drops the `xor eax,eax` before setne / sete and the `mov al,dl`
  (`!= 0`, `(bool)`, `!empty()` keep them); `return(x->Active ? false : true)` gives `mov al / test / sete`,
  `!x->Active` adds `xor eax, eax`. `p ? p->f() : false` inside an && loads the member pointer straight into
  ecx. A `sete` before a null test comes from a bool local (`bool fresh = !flag;`); a bool local for the
  return also matches; `if (result = f())` on a RefCountPtr goes through the pointer conversion, `!= NULL`
  computes `setne` early.
* **Exits and cross-jumps:** early returns after a big local object repeat its destructor chain; nested ifs
  give a single exit, and `if (cond) { ... } return(false);` shares the epilogue with a later
  `return(false)`. VC6 cross-jumps identical arm endings only when the shared tail is not tail-duplicated:
  one `return true` at the end, not one per branch (`ok = false` in each arm blocks it). In switch cases
  if / else + `break` versus `return` decides which copy of a shared tail survives; a shared `return(true)`
  reached by `break` keeps the constant live over the switch. A bool flag set in each branch then
  `if (flag)` reproduces block order and shared blocks. An early return kept in place beside an EH dtor
  comes from a value the next test rejects (`frames = 0` before `frames < 3`).
* **Early returns move destructor sites and pushes:** an early return when not found, or the NULL return first
  in an if / else, reorders destructor sites and slots (a conditional expression instead of an if can too);
  a success test after the `if (x)` block, not inside it, gives the return-value EH flag; a slot that saves
  edi only in its range-test branch is `if (index == cur) A; else B; return(true);` (two early returns push
  edi on entry); an early `if (!robj) return false;` keeps a later struct's initialisation at function
  scope; a function the client never inlines stayed out of line written `if (!on) return;`, not with `&&`.
  A group creator matches as `if (name == "x") return Create_NEW((Base *)new X(l, r));` per branch.
* **Calls per branch, not expressions:** the client's `cmp / jne / push A / jmp / push B` with one call is
  `if (x) f(A); else f(B);` (VC6 merges the calls); `?:` between arguments gives a cmov-like load, a bool
  expression gives `setl`. Per-case calls the client tail-merges are separate calls per case; two
  `Change_Text` calls in an if / else are not a ternary; a statement repeated at the end of both branches is
  tail-merged with its constant in a register. Ghidra shows cross-jumped calls as one call through a
  variable: write two calls.
* **Loop forms VC6 rewrites:** `while` / `for(;;)` with the test at the top rotates and peels the first call
  (`strtok_r`); `do { tok = ...; } while (tok);` keeps one call site; an unrotated loop is `bool more` +
  `while (more)` + break, or `while` on the other condition plus if-break. A one-trip loop whose body has an
  index ternary is removed: write if / else. A search loop stays top-tested only as `while (true) { if (end)
  return NULL; if (hit) return &x[i]; i++; }`. A loop-invariant float test before the first pass means
  `while`, not `do-while`. A loop bound computed once wants a local; `for (id = 1; id < 5; id++)` with
  `[id - 1]` and `for (i = 0; ...)` with `[i + 1]` differ.
* **Loop offsets and fills:** write a loop's start offsets as `i * step + k` in the body, not a running
  `at += step`, for the client's induction setup after the count test. `&a[n++]` vs `&a[n]; n++;` change
  registers and induction pointers. A small constant-fill or zero loop is unrolled with its value in a
  register, interleaved with neighbours; written-out stores keep them together and use another register;
  a separate `for (i < 4)` zero loop gives its own zero register next to a shared one. An inline `memset`
  computes `lea edi` after `mov ecx` / `xor eax`; `lea edi` first is a byte (element) loop that VC6 turns into
  the same `rep stos`.
* **Switches:** case bodies are laid out in source order: write the cases in the order the jump table's
  targets, or the EH state numbers the cases set, show (a switch's EH order beats its layout order); where
  reordering gives identical bytes VC6 normalised it: the residue is elsewhere. A one-case switch on 1..3
  is `lea eax,[x-1]; sub eax,0`; a `sub eax,0 / dec eax` chain with a shared tail is a `switch`; cases 1 and
  5 sharing one `return` give `dec / sub 4`. `default: G_ASSERT(false); case 0:` reproduces EH states. Paired
  keys each as their own case (no `key ==` test). A case with its own copy of the tail and one that breaks
  to the code after gives a cross-jump into that tail. An if chain may be a switch in the client.
* **Value threading, redundant tests:** a redundant NULL re-test comes from `if (a && b) {...} else if (a)
  {...}`; a top-level `if (!a) return;` then `else if (a)` folds away. `time >= Time` with the operands
  swapped keeps VC6 from deriving a range fact for the else branch's loop test. `int result = wcscoll(...);
  if (result < 0) break;` gives `cmp eax,ebx`. A backward step through a local (`prev = index - 1; index =
  prev < 0 ? 8 : prev`) gives test / jge.
* **Inline predicates and helpers lay out blocks:** an inline predicate (`Is_Pressed(g)` returning
  `a == g || b == g`) puts each if's true block right after its test; written out, the true blocks go to the
  function's end. `a == this || b == this` lays the else branch first, `a == this ? true : b == this` gives
  the other order; `b ? x : -1` emits the x branch first, `(b == NULL) ? -1 : x` the reverse; a chain of
  `G1 == x || G2 == x` tests with the else blocks first is an inline bool helper. A `goto` label's placement
  decides which sibling-branch statements merge.
* **Whole calls in both arms:** VC6 hoists common code out of if / else arms and merges tails. A client
  `push A; jmp; push B; common pushes; call` is an if / else with the whole call in both arms, as is a `fld
  Frame` before the branch with `push ecx; fstp [esp]` before a `cmp`; a ternary argument `f(c ? a : b, 1)`
  puts the result in a saved register instead; a shared float local gives yet another form. A `push ebx` of a
  parameter just compared with a constant has the call after the if / else (repeating it in the arms, or an
  early return, folds the constant). Two Get_String calls in an if / else tail-merge at the call.
* **Separate exits:** an early `return(true)` at each branch's end keeps per-branch tails VC6 would merge;
  `if (!x) return(true);` per test gives separate je exits; a local pointer copy stops tail-merging identical
  `*out = 0; return;` arms; an early return on a RefCountPtr temporary matched as `if (a == NULL || a->Get()
  == NULL) return(true);`. A shared `sub esp; asserts; jmp` exit where the client repeats epilogues was the EH
  frame of an inlined vector insert, not the return statements.
* **Which arm comes first:** Ghidra's `if (x == 0)` can invert the source's arms (`if (Split) {loop} else
  {single}` went 0.40 -> exact); the client puts the non-null case first wherever it tests `x == NULL`; assert
  lines running backwards against execution are each the else of its if (`if (x) {...} else { G_ASSERT(x);
  }`); a `||` condition whose then-block returns is sunk to the function's end unless an inline bool helper
  wraps it; .rdata literals descend in order of first use, so the highest address gives a switch's first
  source arm; a loop run in one branch only can look like a shared tail: check where that branch returns.
* **Switch cases that share a return:** case labels sharing one `return` compile to a range check (test / jle
  / cmp 2 / jg); the client's dec / je chain needs one `return(true)` per case. Reading the jump table's targets
  against code addresses made 33 cases exact in one step.

* **A three-case switch the client reaches through a jump table needs a `case 0`** (sbase_info.cpp's room
  loader); `Is_Int` / `Is_Float` / `Is_String` tests (item_info.cpp's field types) are an if-chain, not a switch.
* **A shared `return true` exit** (the client's single `mov al,1` the early returns reach): write the early
  returns as the literal `true` and use a result variable from the transfer's first return on
  (UIInvClass::Unknown_5A0740, 0.68 to 0.74; a register-only residue remains). Its placement is the lever.
* **A `virtual bool` override with no `return` at all:** is error C4716; a missing return on one path only is
  warning C4715 and eax keeps what it had (the client's form). `return ++global` is mov / inc / mov / ret; `if (p)
  f(p, 0); else f(NULL, 0);` keeps the client's branch; a `jle` loop is `i <= N`.
* **Twin loops and merged tails:** twin loops reuse one count variable, and a counter the client spills after the
  loop wants the count kept in a second variable; per-branch locals (own buffer each) stop cross-jumping; a separate
  `result < 0` per sort branch gives the merged tails.

**Frame and stack slots**

* **Stack slots follow scopes:** sibling block scopes share frame slots; a client frame equal to the sum of
  its cases' locals means one case's locals sit at function scope. A local declared later at the enclosing
  level does not overlap a closed block: place declarations from `sub esp` and `lea [esp+X]`. Two
  address-taken locals the client keeps in one slot need each its own `{}` block; an inlined callee's form
  decides whether its temporaries share slots (a ternary gave a 0x10 bigger frame than if / return); early
  returns in a wrapper give an iterator temp its own slot. A named local where a temporary grew the frame by 4.
* **Position within a scope does not move slots:** only block-scoped locals share slots (moving `float width,
  height` into the loop body fixed a frame); declaration order and where in one scope a local is declared do
  not move VC6's frame slots (120 orders: identical): don't sweep them. A one-slot `new`-temporary difference
  went away with the iterator assigned inside the `if` condition. A float member passed by value after a
  by-value virtual call matches only through a named local (`float radius = X; Update(Get(), radius);`).
* **Frame size clues:** 4 bytes larger: a struct holding an 8-aligned `__int64` (shifting later members), or
  separate int temporaries (`push_back(0)`, `push_back(atoi(p))`; one inline helper at both sites shares the
  slot); 4 bytes short: a cast temporary for a count (name the local); 0xC larger with z unused: `Matrix3D tm =
  ...; Vector3 pos = tm.Get_Translation();`; 0x14 larger with a lone register copy: a by-value AABoxClass of
  which 2 floats are used; extra bytes: a by-value getter's temporary (`Get_X_Vector()`). Too small by a
  matrix or vector: a named copy (`Matrix3D light_tm = camera`): temporaries share slots with named locals,
  named locals don't share with each other.
* **Buffer and array sizes:** a Get_String size of 251 means `char[252]`; a 0x84-byte buffer with a pushed size
  of 0x80 is `char x[129]`; Ghidra's `char buffer[1028]` can be `char[1025]`; a key buffer's size moves slots
  (`char[15]` vs [16] swapped the INIClass and the buffer); Ghidra's `param_2` used as a float is a local packed
  into the argument's slot; `char pad[n]` after a bool packs at alignment 1 (use int arrays); a `Vector2 uv[4]`
  array gives an `and esp,-8` ebp frame, four locals the esp frame; `IntersectionResultClass` declared before
  the block avoids the array ctor, inside the block it emits `??_H`.
* **A frame 4 bytes larger with a named local and a `new` temp in separate slots:** split the function into inline
  helpers each with its own buffer; `char[1025]` with 1024 passed also gives a one-dword-larger frame.

**Constructors, destructors, vtables**

* **Constructor forms seen once:** a vtable store after the member initialisers says whose `+0x04` member it is (a
  listener base of 4 bytes, the subclass's own member after it); a file-static pointer global gives the client's
  release sequence, an extern one does not.
* **Reading construction from the listing:** a vtable store after the member stores is an init list (every
  member in it, declaration order; `Member(NULL)` there stores before the vptrs); `esi = this + 8` is a struct
  assigned from a temporary; a ctor moved out of a header keeps its stores in the body; a callback's vtable
  stored first: assign in the constructor body. A vector member at the start of an init list copies a byte
  from an argument slot into the allocator (+0x0C): not a member. A `ret 8` ctor keeps its unread second
  parameter. Virtual calls in a ctor / dtor mean a base inline helper or same-file member, not the virtual.
* **Destructor and EH-state evidence:** a function that only returns, referenced only from unwind funclets,
  is an empty user-written destructor; a destructor's definition order sets the EH state numbers; an empty
  user-declared destructor on a member adds an EH state to its owner's (the count tells the member's type);
  an empty inline dtor on a local iterator gives the client's EH frame. A deleting dtor inlines the class's
  dtor only when the class's own-side interface has no virtual dtor. A missing EH `-1` store: an explicit
  `ptr = NULL;` last. A hash_map / RefCountPtr release repeated with another store order is two written
  copies; with the same order it is a member inlined. Two RefCountPtr locals under one EH state are a struct.
* **Bases, members and vtables:** a null-checked `this ? this + off : 0` around an inlined member dtor means a
  base, not a member; a class whose ctor / dtor store no own vtable holds the struct as a member; with a
  non-polymorphic first base VC6 moves the polymorphic base (RefCountClass) to offset 0. An implicit copy
  ctor storing a second vptr means the member's class has a polymorphic base (a `_purecall`-only vtable).
  Defining a ctor emits the vtable and, for undeclared overrides, the base's inline virtuals: declare the
  client's overrides. VC6 has no covariant return types. A `ret 4` slot with nothing in eax takes an
  out-parameter; a ref-counted member set is release-then-add-ref.
* **Element copies name the element's members:** a `rep movsd` where the client copies member by member
  means the element has a member with a user copy constructor (Vector4 / Vector3); a copy compiled to
  `rep movsd` in the client is a data struct. VC6 puts the loop pointer on the third dword of the first member
  with a copy constructor: when the client's pointer sits elsewhere, nest the members into sub-structs.
  Contiguous float slots reaching a setter through fld / fstp are an aggregate (Vector4, not float[4]);
  integer `mov`s copying a float pair mean one Vector3 / Vector2 block.
* **Declared or implicit destructor, more evidence:** a deleting-only destructor with no vptr store is
  implicit; `call dword ptr [vtable+slot]` right after the vptr store is an inlined helper's virtual call
  (written directly it is a direct `call`); an `atexit` stub that is just `ret` means the static's type has an
  empty destructor; a `??_G` with flag 0 inside an inlined `clear()` is a one-member struct element with an
  implicit destructor; an empty base destructor the client calls must be out of line; a packet class the
  client destroys out of line needs a declared `virtual ~X();` defined empty; unwind states tell a member's
  destructor from a cleanup method.
* **EH states and callee visibility:** a `new` loses its EH state when the same-file ctor can't throw: define
  that ctor after its caller; a state store before an out-of-line call means the callee's body isn't visible
  in the client's TU (a full specialisation with out-of-line members, delta_money.h); a body the client
  repeats inline is an inline member defined before its callers. Definition order can drop an EH frame (one
  function kept the client's frameless form only when defined before its neighbour). REF_PTR_SET's null test
  disappears when the cache-or-new choice is an inline helper with two returns. A function that inlines a
  same-file helper one level and calls it recursively out of line calls the helper; a hand copy inlines twice.

* **An extra vtable-pointer store in a destructor the client keeps bare:** a declared empty destructor stores
  the vtable pointer (gd_scut_proxy.cpp); a bit-field read needs a `(bool)` cast.
* **Zero-fills:** `memset` of 0x10-0x14 bytes is unrolled stores, 0x20 bytes `rep stosd`; a `rep stosd` after the
  vtable stores is the member-array memset.

**Field offset, layout**

* **Layout facts:** an 8-byte member (DurationClass, `__int64`) in a class with a vtable pads the vptr to 8
  and grows the class: `#pragma pack(push, 4)` as client.h does. A dead high-half store means the member was
  read through an `__int64` local. Members reached through a pointer to a sub-block are a struct; helper
  members that ignore `this` sit on the base. Two classes over one std::map value type share COMDATs and the
  matcher trades their template addresses: give each file its own struct when the client's `_Nil` globals
  differ (the exposed false matches are D66 (iii)).
* **Allocators and bases in a layout:** a VC6 vector member's allocator byte sits first, so "a member at +0xC,
  vector data at +0x10" is one vector at +0xC, a `mov byte ptr [this+4]` allocator copy reads like a bool
  member, and a ctor "reading an extra bool" is the default allocator argument; copying an allocator from a
  bool argument's slot matched as `*(std::allocator<char> *)&arg` (matches, not evidence). A non-polymorphic
  base at +4 under a vptr shows as the base dtor called through `this ? this+4 : 0` (as a member it costs the
  ctor and dtor). An implicit copy of an anonymous union `{bool; char[]}` copies every member: write
  `operator=` out inline. ClientClass (0x1150 bytes, tsk_game.h) is the `pack(push, 4)` example.

**Relocation**

* **Symbols that differ only in decoration or relocation:** a file-static reached by displacement
  (`[GutterColors+8]`) is unrelocated in objdiff: make it a global. VC6's decorated name of a static data
  member encodes its access (public / protected / private change the mangled name). A file `static` the
  asserts read unqualified (`_AllSkills`) gives other registers in its users and a plain global loses the
  guarded atexit stubs; only a class static member gives both (look for it in parked files whose statics are
  `_Xxx`).
* **`explain.py` says relocation for a function whose `__FILE__` text sits in an aux header (auxdata.h):** the real
  class can be inline-budget; compare the call counts (23 against 26) before chasing names.

**Literal, floats**

* **Floats through integer registers or the FPU:** an int-register copy of a float member means a reference
  accessor or a plain assignment; an inline accessor is the form when the client copies the float through the FPU
  (`fld` / `fstp`); `c + 0, 0 - s` is `Matrix3D::Rotate_Z` on an identity; `WWMath::Sin` / `Cos` are asm inlines; a
  `Vector3` with Z 0 stands in for a `Vector2`, and a `Vector3` temporary inside a `new` expression.
* **x87 order and constants:** parenthesise each product (`(width * u) + left - center`, `(keep * U) +
  (rest * W)`); `(a*b)*c` differs from `a*b*c`; `-480.0f * y + 240.0f` compiles to fsubr, `(-480.0f * y) +
  240.0f` to fmul / fadd; VC6 moves a literal summand to the end of a sum (a static const or float local keeps
  the order); build differences as a Vector2 then divide per member; `x * 2.0f` is `fadd st(0),st(0)`; a
  compare against a literal or local const folds to `fcomp qword`, the client's `fld dword; fcompp` is a
  function-static `static const float`; a float the client loads with `mov eax,[.rdata]` is a named const.
* **Float copies and returns:** route non-zero float stores through an inline `(float &dst, float value)`
  setter where the client uses fld / fstp and zeros come from an integer register; a file-static inline
  returning a float member passes it by fld / fstp, a direct member read by mov; an inlined float-returning
  helper in an argument spills to a stack temp, the client's `push ecx; fstp [esp]` is the expression written
  out in the argument; a dead `fstp st(0)` comes from a named float feeding `1.0f - t`. A Vector3 local
  copied from a vertex stays on the FPU across a loop; copying through a POD (D3DVECTOR) gives integer moves.
* **Clamps and min / max:** speed clamps are WWMath::Min / Max (by-value stack copy): separate
  `WWMath::Min` then `WWMath::Max` statements match, if-chains, `Clamp` and the macros do not; where the client
  reloads the member, always.h's `MAX(a, b)` (a ternary reverses the compare); a float max as
  `min > r ? min : r` gives `test ah,0x41 / jne`. A `max()` pair's evaluation order shows named floats vs
  constructor arguments.
* **Matrix and vector helpers:** a float copied by fld / fstp into a matrix element is Matrix3D's inline
  Set_X / Y_Translation, not `m[0][3] = x`; `Matrix3D::Transform_Vector` emits a run-time alias check where
  `mulVector3` stores straight; the static `Matrix3D::Rotate_Vector(tm, in, &out)` folds its alias test only
  when both are locals (a member input never folds: a file-static copy without the test, terms as
  `X + (Y + Z)`); the 3-argument `Vector3::Cross_Product(a, b, &r)` keeps inputs in memory and a zero in a
  frame slot. `Matrix3D m = obj->Get_Bone_Transform(...)` emits ??_H after the call; writing `tm[i][j]` by hand
  pulls in the Row array's eh vector constructor. `Set_Transform(D3DTS_WORLD, Matrix4(true))`, not
  `Set_World_Identity`, gives the render-state flag ops. Rotate_Y in both if arms differs from one on a ternary.
* **Integer forms:** `int i < sizeof(a)/sizeof(a[0])` compiles to `jb`: cast the count to int for `jl`. A
  16-bit `imul` in an index is `(unsigned short)a * (unsigned short)b + x`; unsigned sums show as `shr`. A
  local index (`int i = a*2+b; T1[i]; T2[i]`) gives `[reg*4+disp]`, the expression written out in each
  subscript gives `lea / shl; [reg+disp]`; an `int index` subscripted twice keeps the -1 out of the
  displacement. `(uchar & 0x80) != 0` gives `shr al,7`. An `or ecx, edi` where an immediate was assumed is two
  or'ed .rdata constants. `GetKeyState`'s down bit matches only as `((unsigned)GetKeyState(k) >> 15) & 1`.
* **Bit tests:** the client's `mov eax,[p+4]; shr eax,n; test al,1` comes only from bit fields read through
  inline bool accessors (`Is_X()`), in a union with the whole word for the `== 0` test; plain
  `(x >> n) & 1`, `flags & BIT` and bare bit fields fold to `test byte, mask`. Two bit tests in nested ifs
  with the body repeated give a byte load + two `test al`. A bit-field flag is set through a `bool` local.
  An `&` test of two bit-set values may go through `rend()`'s form. Three stores through `lea edx` are a
  memset of a 3-element array; `xor eax` before `lea edi` is memset, not an element loop.
* **Folded constants:** VC6 folds a constant product into one constant (`x * 0.5f * 1000.0f`, `x / 180.0f *
  WWMATH_PI`, DEG_TO_RADF, `(int)(x * 2 * 0.95f)`): the client's two fmuls need the intermediate as a named
  float local or its own statement; `y * -480.0f + 240.0f` folds to `240 - 480 * y` (fsubr); `(a * b) * c`
  and `a * b * c` differ when c is a float local on the FPU; a `(float)` cast stops VC6 moving a negation
  (`(float)(-(a * inv)) * scale` keeps `fchs` before `fmul`; keep `inv` double); a double literal `0.5 / x * y`
  assigned to a float gives `fdivr qword`.
* **How a float value was declared:** `push imm` is a literal; `mov reg,[mem]; push` with duplicated .rdata
  values is a named `static const float` (which gives fld-constant-first code where the client used
  literals); `fild dword ptr [rdata]` holding a constant is an extern const int defined after its use; `if (c)
  X = K; else X = -K;` compiles to `mov [m], imm`, a named `float e` set in each arm then `X = e` gives `fld K;
  fstp X` (a file-static const gives an integer copy). Evenly spaced .rdata floats are literals merged across
  objects, not file statics.
* **Float locals and the FPU:** a float local for a shared value stays on the FPU and reorders stores with an
  fxch; `fld x; fsub st(1); ...; fstp st(0)` after an inlined float call is a named float local holding the
  result, `fsubr` the plain expression; `float d = other->Radius; d += r;` gives `fld other; fadd r`; `fld m;
  fst [tmp]; mov eax,[tmp]; fstp m` around an inlined setter is `Set_Frame(Get_Frame())`; `Vector3 step =
  delta; step /= (float)count;` keeps X in memory; `pt.X *= size; ...` differs from the combined expression;
  a client `fadd 4.0` between `floor` calls is `float w = floor(a); ...; w += 4.0f;`.
* **Integer against FPU copies:** element-wise and plain copies (`float a = x; float b = x;`) give integer
  moves, chained assignment `fst`, `Get_Z_Vector()` / `Set()` the FPU; `Matrix3D(-v)` copies the translation
  through the FPU, `Matrix3D m(true); m.Set_Translation(-v);` as integers; a two-int copy loading the high word
  first with one destination load is `int *to = ...; int j0 = from[0]; int j1 = from[1]; to[0] = j0; to[1] =
  j1;` (a struct or `__int64` copy does not give it); a run of `mov [x], 0` with no zero register is float
  members set to 0.0f; `lea reg,[this+N]` then stores through reg is an inlined `memset` of a member array.
* **Signed, unsigned and compare forms:** an `int` index gives `jl` where the client has `jb`: use `unsigned`
  (a pointer loop gives `jb`; an unsigned index test gives `jae`); `test r,r; jbe` is an unsigned `x > 0`,
  `!= 0` gives `je`; a bare `return a == b;` gives `xor eax,eax; cmp; sete al`, the client's form without the
  xor is `?:` or an if; `cond ? f() : true` keeps `sete` / `test al` where `||` folds it; a nested `!= NULL`
  gives setne for sete; a key built from 16-bit halves is `(hi << 16) | lo`, not one dword cast.
* **Keeping `__LINE__` (the rule is in §3.1.1):** put new statements, dead reads, pragmas, includes and
  defines on existing lines or blank / comment lines (shorten the opening comment by a line; a needed
  `#include` can go after the asserted code; new code after the file's last `__LINE__` user); declare a
  destructor on its ctor's line; join statements; replace a deleted line by a one-line comment and keep NOT
  EXACT comments one line (a `NOT EXACT` mention in any comment also adds a not-exact.md row); with no room
  for one, put the preceding one-line function on one line. Pay for each added function with padding in the
  same script (D25); pad a draft's `#line` gaps before the signature only for the first assert; pair a
  variant's added line with an opposite edit.
* **`__LINE__` tool traps:** a trailing blank line before an appfile `@N` anchor eats a line; a tryv2 edit
  anchored on a loop line can match twice (anchor on the line before); variant runners that insert markers
  (vr.py) shift lines: re-run the exact-form check without them; after a fixpad / assert-line script check
  every assert of the file (a moved line broke postate's type 15 ctor silently); a function can go exact by
  its assert line alone; scratch test commits of landing tools go outside library directories. In an
  EA-derived file the banner note replaces the bare `**` line under EA's copyright, one line for one. `#line N`
  alone switches `__FILE__` to the full path; `#line N "..\\shared/..."` needs escaped backslashes.

* **File-scope `const float` operand order** (gd_gauge.cpp, gd_pip_scene.cpp, gd_proxy.cpp): where a
  file-scope const stands in the expression decides the instruction's operand order; try it in both positions
  before reaching for the named function-static form above.
* **Float calls:** `atan(x)` compiles to `fld1; fpatan`, `atan2(x, 1.0)` loads a double; `Min(Max(x, 0), C)` reloads
  x; `fabs()` on a float kept as a double local gives `fst qword`; `(e + e) * 0.95f` gives `fadd st0,st0`; an inline
  accessor for a member next to a known constant (`Get_Avatar_ID()`) gives `push 0`.

**Reading the client**

* **Out-parameters, thunks and calling conventions:** a `ret 4` function writing its result through eax with
  no copy on the early return takes an out-reference (`void Get_Bounds(AABoxClass &box)`); a function that
  reads its first argument as an output pointer and leaves eax unset returns void through a pointer; a
  by-value return the client copies through a temporary wants an assignment, not an initialisation. A client
  function called directly, not through a `j_` thunk, is `static`. A `ret N` helper reached with
  `mov ecx, this` is a thiscall member with `this` unused (non-static). `new T(a, b)` inside a call merges
  the null tests.
* **Which object a function lives in:** VC6 inlines a later member of the same .cpp, so an out-of-line call in
  the client where ours inlines can mean another object: split the file (an appfile exact in one file but
  none in the client's object). An EH state around `new X(...)` with an out-of-line ctor means the ctor is in
  another TU. Templates can compile with or without an EH frame for `new` depending on the instantiating file.
  A large `#import` header changes register allocation elsewhere in the translation unit.
* **Port a twin's form first:** functions duplicated across twin files (an exact ui_avatar_cust function and
  its ui_avatr twin) take the exact twin's form before any lever; an EA inline (`Create_Get(source->Clone())`)
  before a stack-slot chase; a getter chain the client uses (`Get_Client()->Peek_...()`) at the call sites. A
  NOT EXACT comment can hide a functional misread: re-read the listing.
* **Small idioms:** `T::iterator it; for (it = X.begin(); ...)` keeps the iterator in registers in hash_map /
  map delete loops (the extra line moves `__LINE__`); `if (it)` loops match with `it != NULL`; a never-re-read
  stack temporary is a reference bound to an element; `push_back` of a copy of the pointer a FAIL_IF tested
  stores after the null test; `snprintf` is VC6's `_snprintf`; `&Packet(args)` passed directly is a temporary;
  a virtual call made before the pushes is its own statement first. `near` is a Windows macro (C2181); VC6
  rejects a second `for (int i ...)` in one scope (C2374). FAIL_IF stringises its unexpanded argument: a
  local `#define` can supply the client's expression text.
* **Deletes and dead calls the client keeps:** consecutive `delete p;` calls share one stack cleanup
  (`add esp, 8`); the client's per-call `add esp, 4` is `if (p != NULL) delete p;`. A dead `ntohs(Port)` or
  accessor call the client keeps: keep it. A test of a part name's address constant (`mov ecx, imm; test ecx,
  ecx`) means blocks written out, not a loop over an array. Removing a dead labelled block changes layout.
  SGI hash iterators: begin / end / find results kept in locals change what is inlined.
* **Returns through memory:** a caller copying the result through eax means a getter returning the struct by
  value (`Vector2 result`, member stores, one `return(result)`); a hidden output pointer as first argument plus
  two Vector3s in the frame is a Vector3 returned by value; a method ending in `ret N` that ignores ecx is
  still a member (`static` inlines it away: the marker reports "defined but not compiled"); an int copy of a
  float with Z = 0 stored first into a TPoint3D argument is TPoint3D's base-copy ctor over a TPoint2D
  temporary.
* **Counting `rep movsd`:** a struct copy has `mov ecx, imm` before it; an inlined string memcpy has `shr
  ecx,2` before and `rep movsb` after.
* **VC6 compiler limits:** C1001 on `template<> void C::M<T>(...);` and on out-of-class specialisations of
  member templates, C2893 on an out-of-line member template, C2909 on explicit instantiation of a ctor:
  define them as plain overloads in a minimal class declaration of your own (member-template functions are
  mangled without their template arguments). A `VectorClass<T>` member needs `operator==` / `operator!=` on
  T even when no ctor is emitted. A static member `hash_map<K, OwnClass>` inside its own class fails with
  C2079: define it at file scope. A friend `Create_Peek(T*)` inside RefCountPtr<T> resolves only once
  RefCountPtr<T> is instantiated: declare the named local first.

### 3.2 Templates and STL

* **VC6's `vector::clear()` is an extra inline layer over `erase(begin(), end())`** (cordcon Reset 0x84E770):
  it changes which `_Destroy` / `??_G(0)` calls stay out of line. Making a callee exact can inline a shared
  template everywhere and lose its pair: check pairs_cmp, and win it back by fixing a client caller.
* **Pointer/POD instances count only in D54's ambiguous column.** `VectorClass<T *>` members, `??_G` of empty
  bases: same bytes for every `T`, 30-140 client copies, objmatch lists 5 (uncap `[:5]` for all: D19's
  0x997EA0..0x9981A0). Don't rebuild them for pairs. D54 ignores `__FILE__`'s directory; copies differing by
  EH handlers, `_Nil` statics or vtables don't count.
* **Explicit instantiation.** Not a constructor alone (C2909), not after `extern template` (C2949):
  specialise, static members before the constructor (C2908). Only `extern template` (C4231) stops VC6 inlining
  a template member.
* **Template virtuals compile on instantiation** (a `DeltaTemplateClass` virtual compiled `Set_Value` for
  money: `MoneyClass` needed `operator!=`).
* **`std::string s(x); v.push_back(s);` isn't `v.push_back(std::string(x))`:** use the local.
* **VC6 C1001 deducing `T` from `hash_map<std::string, T*>&`:** template the helper on all of hash_map's
  parameters, or on the whole map (`template<class M> void f(M &)`); `T::iterator` in the signature also ICEs.
  VC6 has no `snprintf` (use `_snprintf`): the miss shows only as "1 errors" in verify's summary line and
  gains nothing. A VC6 `std::list` member at +0x10 puts its allocator byte there, head +0x14, size +0x18.
* **A hash_map loop's iterator declared in the `for` header** (fontdatabase 0x6C4F50): VC6 makes it
  `begin()`'s return slot and keeps its node in memory. The client declares it before the loop and assigns it
  in the `for` (`iterator it; for (it = T.begin(); ...)`); a const_iterator copy puts the table pointer in a
  register instead. Write the not-found test first (sbase_info.cpp). A map's key member in its owner must
  have the map's exact key type.
* **A template's instance order in the client is its source shape** (data_binder.h's accessors, co_data.cpp):
  the order the instances are emitted in tells the order the source defines and uses them; changing the
  emission order changes EH stores and registers; a function defined later in the file is inlined into the
  earlier ones; an explicit and an implicit `std::string` conversion are different forms.
* **Two classes whose functions differ in one argument type are one template instantiated in that file**
  (ui_inv.cpp's UIInvStripOfClass<MODEL>): explicitly instantiate its constructor at the file's end (17
  functions exact, all on the first form).
* **A hash_map value whose destructor the client never runs** (operator[] 0x71D4D0 leaves its pair temporary
  alive, 0x7198B0 frees nodes bare): give the value class a private, empty, inline destructor and make the
  `std::hash_map<K, V>` its friend (a private destructor alone is C2248 in operator[]). It also frees
  operator[]'s inline budget, sending the hashtable copy constructor out of line as the client has it.
* **SGI hash_map and vector inlining.** Leave hash_map members the source default-constructs as they are (100
  buckets already): writing `Systems(100)` moves `_M_initialize_buckets` inline into another member. Partial
  inlining depends on the iterator type: a member declared `const_iterator` (begin()'s iterator converted)
  reproduced begin() out of line and size() called; an extra inline helper layer around begin() also pushes
  size() out of line, forwarding-only wrappers change nothing. `vector::resize(0)` shows as an out-of-line
  size(), `test; jbe`, then `erase(begin, end)` after a zero store.
* **VC6 mangles a function template as `?Name@@...`, not `??$Name@@...`:** a COMDAT name that lacks `??$` is
  not evidence against a template: compare against the client's symbol by the plain `?Name@@` form before
  concluding the function is not a template instance.
* **Identical template instances in one object's range need caller-evidence names;** `Create_Peek` binds after
  `RefCountPtr<T>` instantiates.

### 3.3 E&B app code

#### 3.3.1 Read first

* **Asserts sit on the client's lines (D25):** the `__LINE__` rule is 3.1.1; in app files keep every change
  line-neutral per file (`git diff --numstat` net 0 for a retype or splice). To add code above a logged line,
  remove as many blank lines from the same function's stretch (pad inside a function, not only before it;
  `.work/milestones/a3/relines.py`); a new `#include` or forward declaration replaces a blank line (or goes
  inside a declaration: `class X *`); a removed class block or NOT EXACT line becomes blank or `// 0xADDR`
  lines. Log calls are pinned like asserts: E&B's INFMSG pushes `__LINE__` as an immediate before the " "
  string 0xB6E948 (cordcon, cordmgr). `appfile.py match` markers replace lines, never add them.
* **`__FILE__`: §2.1;** header strings stay as they are (D41). A `__FILE__` that differs only in case
  (`app\Client\`) differs in the string, not the bytes.
* **Assert text is evidence of the source:** a FAIL_IF's text is `#expr`, so a helper that matches the bytes
  but changes the condition's tokens changes the client's string (don't keep it); names can be reversed
  (`readbytes == bytes` is `cmp ebp, eax`, the wanted size in `readbytes`); FAIL_IFs sharing a string share a
  variable name; consecutive assert lines with a store between them go on one line (`X = ...; G_ASSERT(X);`);
  an assert one line low shows as a cast-text difference. A FAIL_IF on `!iface_ptr` over a RefCountPtr
  parameter, cached in a register afterwards, is a raw local `InterfaceClass *iface_ptr = iface.Peek();` on
  the assert's own line.
* **Where a function's source ends:** a neighbour's FAIL_IF / G_ASSERT line bounds it (a function had to end
  before the next one's line 4804); the blank run before a file's notes can hold a later function's asserts
  (`appfile.py show`); one `__FILE__` window can mix two files (`.work/h8/layout.py` places each `#line N`
  from a draft). An inlined FAIL_IF can carry another file's `__FILE__` (co.cpp's line inside client.cpp's
  0x7283B0): explain.py calls it relocation, and no source of the compiled file can produce it. At a file
  boundary check the `__FILE__` the function pushes before trusting asserts.txt (3.6).
* **Object boundaries (placement evidence):** objects link in name order, so a boundary bounds the file name.
  Ends of objects: a guarded `atexit` static initialiser in mid-range; every object including `<string>` ends
  with the 32-byte guarded `std::ctype<unsigned short>::id` initialiser (guard 0xC416FC; scan raw bytes for
  `8A 0D FC 16 C4 00 B0 01 84 C8 75 08`, it is not always a verified start; pairing questions: 3.4), so code
  placed after a file's guarded `<string>` static lands in the next object. A PermStringInitClass static opens
  an object, it does not end the previous one; a file's own guarded static initialiser can also sit at the START of its object (proj_man.cpp's cos initialiser 0x491660), and a queue row's range can run past a guard: scan for the guard bytes before trusting a row's end. A static function stored by address with no incremental thunk
  marks a file-local function and a boundary.
* **What is not placement evidence:** out-of-line copies of inlines (VC6 puts them among the file's own
  functions), a static's place in .text, shared string or float literals (merged across objects; the object
  linked first holds them), and a header static's `_$E` copy: the guarded `<string>` initialiser has 533
  copies and one inside a file's window is often the neighbour's; the client's CRT initialiser table
  (.CRT$XCU, 0xB6427C-0xB6E59C) lists them in link order: check it before trusting a range credit (P-IR3-1).
  An object's client range never comes from the ledger's `in_tree` (it holds COMDAT copies elsewhere): use the
  object's own non-COMDAT / NODUPLICATES exact functions. A COMDAT placed after file F's last function puts
  every lower caller in F's object; one placed inside a window can belong to code the linker dropped
  (0x42C330).
* **Where an uncalled out-of-line copy comes from:** an unreferenced function with incremental-link thunks is
  a non-inline .cpp function; an uncalled run of a class's inline members (ctor, copy ctor, operator=) whose
  thunks are never called comes from non-inline .cpp definitions that same-file callers still inline; an
  uncalled inline member (operator== used through a vector) gets its client copy only from an out-of-line
  definition; an out-of-line function with no callers right after a FAIL_IF handler, or a member that never
  uses `this` (ret 4), is what earlier functions in the file inline.
* **Definition order in a file:** a shared epilogue or a function's form can be file state: write the
  functions before it first (two went exact unchanged). A static helper whose asserts sit after a big caller
  can still be defined before it (the client emits it after either way); a static helper called directly sits
  right after its caller. Adding functions changes earlier functions' inlining: re-run every marker of the
  file (3.1.1).
* **A declaration that mangles differently from the definition links to a do-nothing gen stand-in:** a free
  `Unknown_<ADDR>` declaration, a caller's local stand-in class, a `struct` / `class` mismatch (PAU vs PAV), a
  `const Vector3 &` where the definition has `float *`, a `void` / `bool` return mismatch, or an `extern
  Unknown_...` global standing for a library static (WW3D::OverbrightModifyOnLoad; 0xBFAEE0 is
  `WW3DAssetManager::Get_Instance()`) all compile and link silently, and the call does nothing or returns 0
  (Do_Frame's free Unknown_65F780 was GadgetClass::Process_Input: no click reached any gadget). Recognise:
  stubs.log `gen` lines (in the POC's in-world run) whose `client.py name ADDR` exists in the tree;
  explain.py's "relocation: unresolved ... a different declaration?". Fix: call the real name and include its
  header (a new include: D25 above and the pads in 3.4; when an include costs a pair, declare locally). Grep
  the callers' declarations before writing a "missing" function.
* **Renames and retypes break other files:** grep `->Member` across app/ (and the run branch's head) before
  renaming a member or replacing a char-array stand-in (`->Unknown0..\[0x`); a by-value parameter added to a
  shared header needs a forward declaration. One compile error drops every pair of that file (C2039, C2059
  from a `#define` in another header against an enum, C2259, C2440 in ref_ptr.h, C1083), and pairs_cmp shows
  only a mass "lost outright": any error in verify's summary is a stop; it names no file, so compile the one
  file (3.4).
* **Check Renegade's Commando / wwphys for E&B debug code before rebuilding it by hand:** a profile display
  matched console.cpp byte for byte (0x510440); the sea tint is phys.cpp's `#if 0` quad (0x510D40).
* **The symbol pads (SYMBOL_PAD, SEND_SYMBOL_PAD) are in 3.4:** source comments and tools that cite
  "workflow.md 3.3" for the pad rule mean 3.4's two pad bullets (`auxpad.py`, `sendpad.py`).

#### 3.3.2 Look up

**Declarations and headers**

* **Create_Peek / Create_Get bind to whichever RefCountPtr<X> is already instantiated** (VC6 injects the
  friends only on instantiation): until `RefCountPtr<T>` exists in the object, `Create_Peek(T *)` resolves to
  the base's or `RefCountClass *` overload (C2440 in ref_ptr.h(249), naming another class than the caller) or
  the COMDAT goes missing. Instantiate first: a named `RefCountPtr<T>` local, or `enum { X =
  sizeof(RefCountPtr<T>) };` (no code). A RefCountPtr<InterfaceClass> passed to InterfaceSortClass's
  constructor gives the same C2440: pass `.Peek()`.
* **`RefCountPtr<T> x = Create_Peek(p)` of the same type is elided:** the client's copy and release want
  `RefCountPtr<T> x = RefCountPtr<T>(Create_Peek(p))`; a factory's RefCountPtr result passed on to the next call wants a named local (the nested call came out 4 bytes short).
* **An inline empty virtual in a base header is emitted by every file that makes a derived class's table:** a per-file
  view that declares it out of line hides those pairs. A base whose constructor the client calls out of line belongs in its own .cpp.
* **RefCountPtr<T> needs T complete:** an inline constructor with RefCountPtr<T> members in a shared header
  fails (C2027 in ref_ptr.h) in every includer where T is incomplete: define it in the one .cpp that uses it,
  or include T's header (camera.h for RefCountPtr<CameraClass>). A function-local static `RefCountPtr<T>` (a
  lookup's null return) makes its atexit destructor inline Release_Ref: an empty shell class deriving from the
  base works where a forward declaration fails; such lookups match as `if (it == Map.end()) return null;
  return (*it).second;`.
* **A member name hides a library type or base member:** `Node` hides wwlib's `Node<T>` (write `::Node<>`),
  `Surface` hides `::Surface`, and a window's own `View` shadows InterfaceClass::View at +0x48 (the client
  reads `[ebp+0x48]` where ours read the derived member).
* **A `//` comment between two declarations a macro joins swallows the second.**
* **`#include <string>` in a widely included app header changes code in every includer** (message.h cost
  message_server.cpp's 0x4BCB50 outright): give such a declaration its own small header. A declaration moved
  into a header shows in names-review as an "untagged neutral name" at its first use.
* **A minimal local class with the real class name** (UIGuildClass : ChatPanelUIBase) makes a call link to the
  real member, line-neutral, while the real class has no header; replace it with the header when one exists.
  Before adding a class to sclient.h, grep the app .cpp files for a local definition (sclient.cpp defines
  PDAInterfaceClass itself). To rebase an interim class in sclient.h on a later base, move it to its own
  header included on a blank line after the base.

* **A member that ignores `this` looks like a static function in the disassembly** (item_info.cpp): check that
  its callers pass `this` in ecx before declaring it static.
* **A member rename must grep the derived classes' files too** (ChatPanelUIBase): a derived class in another
  file keeps using the old name and fails there, not in the file you built. The same holds for files landed on the
  run branch after yours was cut: after the rebase they show as N lost outright (their compile fails): grep the
  rebased tree for the old names. `udpreplay` compiles files (effect_description.cpp, item_effect.cpp) that neither
  verify's app build nor the link gate's two programs reach.

**Classes, vtables and layout**

* **Declare every override the client's vtable has:** once a constructor is exact, D188 ties the vtable to the
  client's, and an override the class doesn't declare gives the base's inline virtual two addresses: it is
  lost in every file (a stand-in base included); if unrelated files lose short pairs, run `vtables.py --seed
  C=ADDR --list` on the new object. A base virtual whose signature differs from a subclass's same-named one
  adds a slot and shifts the later ones (grep the subclasses first); an override with another parameter list
  leaves the class abstract (C2259); in a class with two bases an override cannot share a name with a slot of
  another return type in the other base. A qualified `Base::operator =` gives the unchecked this-adjust where
  a cast adds a null test.
* **Read slot numbers from the compiled object, not by counting `virtual` lines:** RenderObjClass 0x180 /
  0x184 are Is_Hidden / Set_Hidden; RefCountClass has two virtuals here, so a derived class's first slot is
  +0x08; ViewClass +0x18 stays `int` (a bool base opened a new slot); overloads' slot order is reversed (a
  forwarder at +0x88 is Remove_Sub_Objects_From_Bone's `const char *` overload); name overloaded virtuals
  differently (Get_Text_Width / Get_Char_Width) to keep MSVC from grouping them.
* **A renamed vtable slot:** an inline compatibility wrapper makes callers inline it instead of the client's
  `call dword ptr [eax + slot]`: change the callers. Retyping a base's `void Unknown_XX()` virtuals gives
  C2555 in every stand-in subclass that redeclares placeholders: grep descendants first
  (`.work/gc/derived.py`). `tools/re/slotrename.py` renames a slot and its uses across the classes; a use it cannot
  type stays marked UNRESOLVED (finish by hand); mind CRLF files.
* **Client-style virtual calls on members:** a call through the object's own class (reference included) is
  devirtualised; cast to the base class reference (`((DeltaClass &)EndTime)`, `((VectorClass<T>
  &)Entries).Clear()`).
* **Pure-virtual placeholders** for not-yet-rebuilt slots of a base no program instantiates are an accepted
  interim (0x7251C0): they must become real before a program instantiates a derived class.
* **Layout clues from a NOT EXACT function:** a wrong `push <size>` before the call to 0x4E9CB0 (`new`) means
  the class's layout is wrong; a raw-pointer member where the client's is a RefCountPtr (read through Peek)
  costs registers.

**Stand-in classes in app sources (link and POC stand-ins: 3.7)**

* **A stand-in class must have the real size and slots:** one used with `new` needs the client's size (a 0xE0
  stand-in whose real ctor wrote 0xF4 crashed later), and a local partial base can hide a smaller real class
  (real GadgetClass 0x30, a local one 0x50: pad the subclasses when moving to the real header); a local
  stand-in for another file's class overrides the base's pure virtuals (C2259). A stand-in's ctor / dtor
  defined in the same file, even last with `auto_inline(off)`, inlines into exact callers or breaks `$E`
  tie-breaks: put it in a separate file. Renaming a stand-in to the client's class name links the real calls
  without the real header; a stand-in class name keeps the real functions as gens. When the first assert sits
  on a low line, stand-ins must fit above it: one line each. Before moving a ui_* file onto a real class
  header, check that it doesn't declare its own stand-in of a base (ui_group / ui_manu's GadgetClass: C2011 /
  C2555).

**Library types in app code**

* **Releasing a RefCountPtr:** a hand-written `if (p) p->Release_Ref(); p = NULL;` schedules the store after
  the next load where the client's `RefCountPtr = NULL` stores first (on a raw member, cast it to
  `RefCountPtr<T> &`); the client's doubled test is `if (p) { p = NULL; }`; a `je` skipping the NULL store is
  `if (p) { p->Release_Ref(); p = NULL; }`, not REF_PTR_RELEASE.
* **Exact does not prove the right helper:** `Create_Get` and `Create_Peek` compile to identical bytes and
  only the call target differs (a Get freed the quit box): check the client's target with `client.py dis`.
* **RefCountPtr conversions:** a `RefCountPtr<Derived>` passed to `new T(const RefCountPtr<Base> &)` makes the
  temporary with its conditional-destroy flag (`test bl,1`, `and ebx,~1`): the implicit conversion reproduces
  it; a RefCountPtr<Derived> assigned to RefCountPtr<Base> goes through the template `operator=`, with another
  register order than the same-type one. `mov eax,[ecx]; ret` is RefCountPtr::operator-> out of line.
* **HashList walks:** the client uses both forms, and the test decides: `while (node && node->Next()) ... node
  = node->Next()` tests only `Next()`; `HashNodeClass::Next_Valid` adds the Is_Valid test (`[eax+8]`), and
  `cur = node; node = cur->Next_Valid();` decides a whole function's register allocation (0x7B8210 exact on
  that alone). Match the test the client has.
* **Matrix3D and vector forms:** `+0` / `0-` sums in a rotation mean `Matrix3D tm(pos); tm.Rotate_X(a)`; a
  16-byte value read as X, Y, Z, W / tan(fov/2) is a SphereClass; four floats loaded in reverse then stored
  mean `RectClass::Set(l,t,r,b)`; `Vector2::Set(x, y)` stores directly where `= Vector2(x, y)` goes through a
  temp. A Matrix3D inline (Translate) the client calls out of line at only some sites came out of line once
  its block was a file-static inline helper. A float constant loaded with `fld` and stored a row apart (0x20)
  through `[esp+N]` points to a Matrix3D temporary (no form reproduced it yet). `Vector3 pos(offset); pos +=
  Transform.Get_Translation();` is Set_Offset's form.
* **Transform against Get_Transform():** a render object read as `Transform` (no `call [edx+0x4C]` in the
  client) must not use Get_Transform(), which validates through slot +0x4C; some functions (ProxyRObj::Render)
  do use it: check each.
* **WWMath clamps:** `if (x < 0.0f) x = 0.0f` compiles to `test ah,1`, `WWMath::Max(x, 0)` to `test ah,0x41`;
  a clamp matching `ja` (not `jae`) is Clamp_Int on unsigned, not always.h's min or std::_cpp_min; the
  client's `fld const; fcomp st(1)` matches `WWMath::Min(1.0f, v)` / `Max(0.0f, v)` with the constant first.
  An FP-only clamp with the bounds in stack slots is a local `if (v<lo) v=lo; else if (v>hi) v=hi;` when the
  value comes from memory, `WWMath::Clamp` when it is on the FPU from a preceding expression. Float min / max
  loops keep each extreme as a separate `float`.
* **INI calls:** INI code calls `Find_Entry(section, entry)` directly (EA's `Is_Present` adds an `entry == 0`
  test VC6 does not fold for an array argument); `INIClass::Is_Present(name)` compiles to setne / test,
  `Find_Section` / `Section_Present` to a plain test.
* **Signal maps and MemberCallback:** the client's `disconnect(Collection, this, &F, GadgetEvent())` is the
  templated form (the signal-map call before the temporary callback is built), not `disconnect(void*,
  MemberCallback(...))`; a dtor whose callback's base ctor is out of line is `Get_Signal_Map().Remove(sender,
  this, &fn, Event())` over unknown_signalbase.h's `EventCallbackClass<EVENT>`; removing a derived class's own
  Event_Type override loses other files' short MemberCallback pairs. A template body identical across
  instances (MemberCallback's Is_Equal / Call / ??_G) pairs only inside the compiling file's window.
* **iostreams:** `std::stringstream` compiles byte-exact from VC6's own `<sstream>`
  (unique_design_id_lookup.cpp); operator<<(int), operator>>(int&) and stringstream::str still differ as
  library code: no include helps.
* **Client constants, not literals:** a field loaded from 0xAECC44 is `UniqueDesignIDLookupClass::INVALID_ID`;
  0xBDA908 is the pooled "" literal (strlen + assign), not a static std::string; E&B's INFMSG keeps `char
  timebuf[40]` while strftime is told 80; `DurationClass now((unsigned)client->Unknown10BC)` explains a
  register holding 0 (the high half) and an unused slot beside the low half; the tree's WW3DAssetManager is
  already 0x408C bytes.

### 3.4 Importer, verify and pairs

Shared-tree effects on verify and commits (other builders' uncommitted work, the working ledger, commit_mine)
are in 3.0.

* **`verify.py --lib`** takes `--lib a --lib b` or `--lib a,b`; a name the ledger does not hold exits non-zero.
* **A new app file is not compiled until it is `git add -N`'d and has a ledger shard** (`ledger.py describe`):
  before that the verify builds nothing new and reports no change. A file added while a verify waits for the
  lock gives "a description without a file block".
* **Verify keeps only the first 50 compile errors per library** (`compile_errors` in the library's
  `docs/data/imports/<lib>/library.json`): a library losing functions with no error for that file means a loud
  file hid them; fix it first. A failed verify records its errors only in the working ledger: read them (or rerun
  the file through `appfile.py match`) before restoring the shards. A HEAD that briefly does not compile shows as
  many losses in a worktree verify: read `compile_errors` before pairs_cmp. The log's "N errors" names no file
  ("2 errors" there means build.py shows FAILED; a rebased retype that removes a type landed files still use shows only so): compile the suspect file alone with
  `importer.compile_and_match` (verify's includes, defines, flags and `source_alias`) to see the errors.
* **Verify compiles the tree as it is:** chain a tree writer and verify with `&&` (a failed writer leaves verify
  compiling new headers with old sources), and a worktree script must `git checkout -- .` before `checkout
  --detach`, or the checkout aborts and the next verify runs on a stale tree.
* **The importer lock is global, first-come among new-code waiters (D238, tools/vc6/lockq.py, `importer.py
  lock-status`):** a ticket untouched for 120 s loses its place, and a worktree made before that code (or before any
  lock change) waits without a ticket and starves behind the queue: rebase it or copy the lock files. Every verify, report.py, auxpad.py /
  sendpad.py sweep and `wt.py bring-over` takes it, and with 6-9 builders a verify waited 20-50 minutes; the
  default wait (600 s, vc6lock, `ENB_LOCK_WAIT`) gives up. Pass `--lock-wait 6000` to verify.py and
  `wt.py bring-over` (wt.py's default is 3000; it passes it on to its report.py step), set `ENB_LOCK_WAIT` for auxpad.py, iterate with the lock-free
  `appfile.py match FILE --pool` (about 1 minute or ~20 s a form, against a 30-minute lock wait; `--allow-missing`
  runs a variant of one function in ~15 s; for draft rounds `build.py --pool 1 --allow-dirty` plus `explain.py`
  also take no lock), and run the full verify once per batch of gains. A queued verify compiles the
  tree as it is when it starts, not when it was launched. Never wrap verify or an `auxpad.py` sweep in a shell
  `timeout`: it kills the job while it waits, orphans a lease named importer-PID (3.5) and can leave a pad
  variant in aux_roots.cpp. A verify started with `&` inside a foreground call is the same orphan. A pad sweep takes
  the lock once per value unless `--hold` (once for the sweep, released in a finally); `sendpad.py --first-window`
  tests the current pad first and stops at a run of three full values (it saves no per-value sets).
* **Verify scratch is private only with an agent:** with `--agent NAME`, `ENB_AGENT`, or inside a wt.py worktree
  (automatic) verify writes `.work/<agent>/tmp/enb_import/verify-<lib>`; a main-tree run without an agent uses
  `%TEMP%\enb_import`, which every other such run overwrites (a new file's .obj vanishes minutes later). Copy the
  objects out before vtables.py or objdiff.
* **`wt.py bring-over NAME` without `--target`** lands on the main tree's branch when that is `run/*` and refuses
  on any other; in a run still pass `--target run/<date>`.
* **`ledger.py describe` takes the plain path:** `describe app shared/damtype.h`, not `headers/shared/damtype.h` (that writes a doubled `headers/headers/` shard; Tools item: make describe strip it).
* **A `ledger.py describe` while a verify waits:** makes that verify exit 1; describe before or after a verify, not
  while one waits. A file-static helper byte-exact in objmatch stays uncredited by verify when same-bytes copies
  live outside the file's range (progress.md §4 Tools).
* **A background verify's output lags:** its "waiting" lines can be minutes stale even with PYTHONUNBUFFERED: read `importer.py lock-status` instead.
* **Library flags (D12):** library-matching.md §3.5 (wwshade `/GX /Ob1`, the other EA libraries `/Ob2` without
  `/GX`, app `/GX`, D23). Signs of wrong flags: EH prologs (`push -1; push handler; mov eax, fs:[0]`) only in the
  client, or a file's callers all calling what ours inlines. Compile through verify's path
  (`importer.compile_and_match`), not vc6.py defaults.
* **What verify can't count.** Overloads differing only in callees become one pattern at both addresses
  (`WW3D::Set_Render_Device` 0x917790 / 0x9177E0), not identical copies for D54 either: `objmatch.py OBJ` finding
  both is the evidence, 0 gain. Small generated inline virtuals and too-short functions never count: show them
  with `objdiff.py` / `appfile.py match` in the commit. Same-shape template instances stay `multiple` until their
  relocation targets have names in symbols.json.
* **Byte-exact matching does not check which global a relocation names, and pairs cannot see wrong initialised
  data:** a function can match while calling the wrong factory, and a hand-copied table reached through a
  relocation can be wrong while every function using it stays exact (a class-select table sent Next from
  Enforcer to Scout). Compare hand-copied tables with the client's bytes and run the program.
* **A "none" for a callee proves nothing** (targets are masked): follow the client's call first.
* **A dropped COMDAT copy is a move, not a loss (D20, D66(i)):** a rewrite that stops emitting an inline COMDAT
  copy, or a struct change that moves an identical COMDAT onto another type's copy, drops a (file, address) pair
  while the address stays matched elsewhere; pairs_cmp shows it as a D66 move. pairs_cmp's "lost shared holder"
  entries are D66 (i) too while other files still hold the address (an inline slot shared by 45 files is listed
  as lost when one stops holding it). Name it in the commit.
  Example: a 32-byte element's `_Construct` moved to ColorizationRegionStruct's identical 0x62A560.
  `wt.py bring-over` refuses such a commit (P-CS1-2); the coordinator lands it.
* **Compare pair sets against the session's baseline, not the last step:** out-of-line copies of inline functions
  (`Set_Texture` 0x47F3F0) come and go with their callers' inlining.
* **A gained address can be another function's identical code:** check that each gained address belongs to the
  function you changed (`client.py name`, `client.py callers ADDR`, the obj's relocations) before keeping or
  porting it. Cases seen: a variant's gain is another type's byte-identical template instance; a template copy
  pairs to file X only because X's non-exact caller calls it out of line; a gain through an invented forwarder is
  the client's out-of-line template copy; `new`-and-construct functions match across libraries, so a loss in one
  library's region can still be matched in its own; while a vtable-emitting ctor / dtor is not exact, an
  identical deleting destructor pairs with a twin class's `??_G` (it settles once the ctor is exact: don't commit
  the vtable's table before). Example: a user `operator=` "gained" `vector<Vector3>::erase` 0x47D3D0.
* **A member added to a locally declared base shifts every derived class's padded fields:** re-match the whole
  file after any base change (a draft silently broke 10 exact functions).
* **A base's implicit destructor costs another file's pairs:** deriving from a base with no declared dtor makes
  your object emit copies of its dtor and deleting dtor, and the base's own file loses them outright (credited to
  your copies). Declare the base dtor where the client calls it out of line.
* **Caller tie-break (D166, D174):** an identical-copies function (D54's ambiguous column) becomes a pair when
  our callers reach one of its addresses and every client call or jump to it sits in a function the same file is
  exact at (no data reference; the linker's merged copy with callers everywhere stays ambiguous). D174: the
  copies may differ in a relocation target (such twins count only once their caller is in the file); a call from
  an EH unwind funclet (0xAA8000-0xACD000) counts for every parent whose unwind map lists it; same-bytes symbols
  of one file proposing one address credit the first by name (`folded`; since D194 every COMDAT with one
  `body_sig` is credited). The credits feed D80 / self name / range / initialiser, which rerun until nothing
  changes, so a late credit can change an early phase: check every `lost` against the client's layout. The pair
  exists only in verify (`match_objects(callers=True)`), not in `appfile.py match` or `objmatch.py`; losing its
  caller's pair loses it too (D66 (vii)); anything that rewrites a file's `in_tree` without verify can leave its
  `tiebreak` list stale. pairs_cmp's "with its caller" is approximate (it sees no call edges); its output is one
  labelled list per line (`gained tie-break` / `lost tie-break` follow `gained` / `lost`).
* **One tree file can be several client objects, and objmatch's own-range rule assumes one:** a file that spans
  objects (land.cpp: three) credits across the boundaries wrongly. Splitting it regained functions and moved the
  rest by D66 between the files (interim names, D137).
* **Shared COMDATs move range credits (D114(b)):** a new file that instantiates another file's inline COMDATs
  makes them "held by two objects", and the other object's range stops widening: its range-credited COMDATs near
  the boundary are lost outright. Name them in `tools/re/symbols_named.json` from structural evidence (unwind
  funclets' `add ecx, member; jmp`), then `symbols.py --apply-named` (P-IS-1). objmatch's tie-break never proposes a
  template destructor whose only callers are unwind funclets: name those the same way (no source change). appfile's `exact` for such a destructor may point at a neighbour file's identical copy: trace the funclet to its owner (`clientimage.eh_stub_area()`) and add the symbols_named row. Crediting a COMDAT to every holder
  lets the later object's range reach over the first object's code (the linker keeps the copy with the object it
  meets first): widen only the nearest holder (P-IS-2). A new template instance (e.g. MemberCallback `??_G`
  copies) can propose an address in objmatch's range round and take another function's tie-break (P-CW1-1).
* **Byte-identical `??_G` copies with no name or range evidence:** a class's `??_G` and a template callback's can
  be byte-identical with the call masked; when the class's destructor lives in another file only the callee's own
  bytes (D213) tell them apart. Byte-identical destructors are also told apart by the unwind funclets that
  reference them (D66 (vi); 0x7E1CA0 / 0x7E1CF0 against 0x604150).
* **Short pairs (D188) are dropped when two symbols claim the address:** `objmatch.short_known` drops an address
  two symbols claim (since D193 all claimants are credited when they are COMDAT bodies with the same bytes and
  relocations; different bodies still drop), and skips a symbol when symbols.json names another one at the
  address. So making a caller exact can lose a folded short pair it calls, and retyping a base virtual with an
  empty body or a visitor slot's parameters renames the short stubs and loses their pairs until
  `tools/re/symbols_named.json` carries `replaces` rows (then `symbols.py --apply-named`). Renaming a generated or
  exact-vtable type likewise changes the mangled names its symbols_named.json rows rely on: add `replaces`
  entries (a retype or the rename of a short virtual does the same; `--apply-named` rewrites them as source `named`, dropping length and proof); out-of-line conversions
  need their client addresses named there before their callers match. A symbols_named.json
  rename alone can change pairs: rerun the verify before keeping such a gain. Check `lost outright` for short
  pairs before keeping a change. Example: an inline `Get_Desc` lost a folded RefCountPtr `Peek` / `operator->`.
* **symbols.json regeneration is not neutral:** objmatch reads docs/data/symbols.json (self names D88 / D213,
  known vtables, D115's initialiser targets) and symbols.py reads the existing file through objmatch, so a
  regeneration changes verdicts and depends on the file it replaces. With the run's own names, D80's `known`
  completes a vtable chain at once and a pair can move (a D171 self name settles a `??_G` before D100's range
  round: a false range pair). Regenerate from the landed file, only on a quiet tree (the coordinator's job), and
  run a full verify of every library after it; check every `lost outright` against the client's vtable layout
  (slot count, slot 0's thunk) before calling it a loss. symbols.py reads only the objects given: copy every
  library's verify-* folder and pass each as `--objs` (with none it refuses). A tool fed by symbols.json must keep its evidence when symbols.json
  records that tool's own result (a regeneration once lost 268 pairs that way).
* **Guarded `<string>` initialisers are range questions, not missing code:** every `<string>` includer compiles a
  byte-identical guarded initialiser of `std::ctype<unsigned short>::id` (guard 0xC416FC, atexit 0x42B320; the
  client has 533 copies). An `_$E` initialiser is `short`, never exact by its bytes: objmatch's D115 round locates
  it by its relocated fields (atexit, the global) within the object's range +-0x1000, and D100 credits it to
  whichever copy the file's range reaches first. So: one left unpaired is a range question (check the .obj for
  its `_$E` before editing includes); when the client's object holds two copies and ours compiles one, moving the
  range (a new function lower in the file) shows the other as lost outright: not a code loss, the user's ruling
  (D196, D197; see accepted corrections below); a copy inside a file's window is no placement evidence (3.3). A guarded initialiser marks an object's end, and its
  absence from the client's object says which headers that file never saw (before adding an `#include`).
  Other dynamic initialisers (PermStringInit's `$E70` / `$E68` pairs) and implicit destructors emitted out of
  line for funclets need D99 rows.
* **Accepted corrections must be on master, and on the target branch, before landing (D202):** verify_pr.py and
  verify_watch.py judge a PR with master's tools and read accepted-corrections.json from master, never from the
  PR; `wt.py bring-over` reads it at the target's tip. A PR's own objmatch change or corrections entry does not
  count for it, and a coordinator's in-run `--accept` is not that ruling. `/accept-d66 SHA` (the maintainer)
  accepts D66 moves only and is ignored when any pair is lost outright. An accepted outright loss (a D66 (vi)
  twin: a guarded atexit pair that turns ambiguous because the client carries its own byte-identical instance,
  shown as `lost outright` by pairs_cmp / wt.py; a duplicated guarded initialiser): land its correction alone in a
  small PR to master (no library: it passes at once, and master's move reruns the run PR), cherry-pick it onto the
  run branch before the landing, and name it in the commit. Precedent: 0x5495A0, 0x604150.
* **aux_roots.cpp's SYMBOL_PAD (`tools/re/auxroots.py`, `tools/vc6/auxpad.py`):** the six padded constructors at
  aux_roots.cpp's end (0x6FB1E0, 0x702240, 0x7045F0, 0x7014B0, 0x701D70, 0x758210) move with the number of
  declarations the file sees (a header member, an enumerator, an `extern`, a removed member; even one added to
  strconv_lib.h) along a 64-step cycle of tie-breaks, each exact only in a window. The lever is auxroots.py's
  SYMBOL_PAD (with DEF_ORDER and USER_DTOR there; never copy its value elsewhere). After any change to a header
  aux_roots.cpp includes, a rename with the same count, or a rebase touching its inputs (even when the generated
  output is unchanged), re-sweep all of 0..63 with `auxpad.py sweep` (a window can be narrow, with identical
  totals elsewhere, and can split). Test such header edits against an unchanged aux_roots.cpp, and declare locally
  in the .cpp when that is enough. No pad window exists when: a separate `operator==` sits among aux_roots.h's
  value types (a single negated `!=` expression keeps one); a template declaration such as DeltaArrayClass's
  bounds accessor is visible (keep such helpers local); a root's classes built inline in another file go into
  aux_roots.h instead of aux_roots_send.h (extract them with auxextract's `run(start, this, regs0={'esi': this},
  stop=addr)`). Explicit `template class` instantiations and padding between definitions move all six alike.
  auxpad.py: `sweep` leases its own VM (`--pool` says the same); a sweep killed mid-run
  leaves its VM leased under `importer-PID` and a pad variant in aux_roots.cpp (`pool.py release VM --agent
  importer-PID`, then check out aux_roots.*); its results reach the log only once the importer lock frees. An
  `appfile.py match` on untracked `aux_roots_pN.cpp` copies with your own VM sweeps without the lock, but appfile
  pads through a different window than verify: use it to narrow candidates, decide with `auxpad.py sweep`.
* **SEND_SYMBOL_PAD (`tools/vc6/sendpad.py`):** aux_roots_send.h's SEND_SYMBOL_PAD keeps sclient.cpp's padded
  constructors 0x758210 and 0x702240 exact (different instances, not twins), and 0x74D990, which sclient.cpp
  defines after `#include "aux_roots.h"`. The window moves with any declaration or definition-order change
  sclient.cpp sees (an extra header at its top, a class leaving aux_roots.h, FontSys's implicit destructor; the
  headers: `python tools/vc6/sendpad.py --includes`), cycles mod 64, can split, and is unaffected by .cpp files
  sclient.cpp does not include. Never store or copy a pad value and never sweep only a neighbourhood: after any
  landing that touches such a header, rebase, then run `python tools/vc6/sendpad.py --pool 1 --lock-wait 6000`
  (the 0..63 sweep through verify's path; it sets the pad to the window's middle); `wt.py bring-over NAME`
  runs it (unless `--no-sendpad`) when the verify loses sclient.cpp pairs and the diff touches such a header. One sweep base
  at a time: a second landing needs a rebase and a new sweep (bring-over refuses a branch losing 0x74D990 or
  0x758210 outright). The sweep's watch set comes from HEAD's ledger (a verify that
  already lost 0x758210 no longer drops it from the set): check 0x758210 / 0x702240 in `.work/log/sendpad.log`. `appfile.py match` and single-file compiles pad through a
  different window than verify (a constant set of false losses): decide with sendpad.py / the full verify only.
  A member added to a header that sclient.cpp or aux_roots.cpp includes flips pad-sensitive pairs beyond the
  watch list (client_item_property_builder.cpp in §3.9 has the case).
* **More pad-sensitive headers:** declarations added to a header reached through sbaseview.h move sclient's send pad 0x758210 (a declaration stays local in the .cpp that needs it); replacing aux_roots.h's views with an include of a new header lost the six padded constructors (0x702240, 0x7045F0): keep aux_roots.cpp's declaration count, with money.h's guard pattern. Out-of-class definitions of member templates compile in VC6 (only explicit specialisations hit C1001): a way to add a member without changing a pad-sensitive header (a plain overload in kv_encoder_base.h cost the send pad and 0x597080).
* **Names inside a header guard macro** (money.h's `__X_ALL` pattern) cannot be enumerators of a type declared outside the guard: use anonymous enums, with casts where an enum value is returned or passed (C2440 / C2665).
* **Keeping both pads still:** put members only one file needs behind a guard macro defined only by its users
  (money.h's pattern; a declaration added to delta_array.h reaches aux_roots and sclient: `#ifdef
  __DELTA_ARRAY_MEMBERS`), and keep a single caller's accessor file-local. A hash_map / list / vector member in a header sclient.cpp includes can leave no pad window at all (the signal classes' members sit behind `__SIGNAL_MEMBERS` for that). A stopped sendpad.py leaves its pool lease: release it.

### 3.5 Build VMs and vc6.py

* VM addressing, `SOURCE_EXT` sync and `.tlh` paths, LIBCPMT, mss32, stub vtables and checked builds:
  tools/vc6/README.md (Quirks).
* **Tools without `--pool` use the default VM host, which can be stopped:** tryv2.py, `build.py link` in a
  worktree, `appfile.py match` without `--pool`, harnesses and scratch compiles take `ENB_VC6_HOSTS`, else
  `.work/vc6_hosts.json`'s default host (symptom: an ssh timeout, nothing done). Lease with
  `pool.py lease --agent ID --count N` and export the `ENB_VC6_HOSTS=...` line it prints; check the lease
  succeeded first (exporting a failed lease's last line falls back to the stale host). appfile.py: use `--pool`
  with `ENB_AGENT` set. When every VM is leased, point `ENB_VC6_HOST` at your own lease and verify without
  `--pool`. Export `ENB_AGENT` and `ENB_VC6_HOSTS` in every Bash call (each call is a fresh shell), or build.py /
  appfile.py compile on whichever VM vc6_hosts.json names, possibly another builder's lease; never hard-code a
  leased VM's IP in a runner. `clienttest.py build` has no `--pool`: set `ENB_VC6_HOSTS`; in a worktree give
  build.py `--pool 1` (without it build / link failed with FileNotFoundError on cl.log). A build on a
  just-released lease fails every file and deletes app.lib, and the POC then silently runs the old exe: check the
  build output says 'compiled'.
* **A killed or stopped job keeps its pool lease:** a killed verify, auxpad sweep or `appfile.py match` stays
  leased (under its agent, or `importer-PID`): `pool.py release --agent NAME`. TaskStop on a Git Bash loop of
  `appfile.py --pool` runs leaves the Windows python children re-leasing VMs as the agent: kill them by PID
  (`Get-CimInstance Win32_Process`) and release. A builder stopped mid-verify leaves its VMs leased and
  running; the verify service then
  cannot start a clone (it needs 6 GB free: 2 GB plus a 4 GB reserve) and posts "verify failed (compile
  errors?)" (the reason is in `.work/verify/prN.json`). `pool.py list`, `pool.py release --agent importer-N VMs`,
  `stop-idle --minutes 0`.
* **`pool.py release --agent X` releases every VM leased under X,** your own POC build's lease included.
* **`pool.py lease --max-age H` reclaims the other builders' leases:** pool.py still accepts it on `lease`; never
  pass it. Lease holder pids die across an API outage and the lease is reclaimed silently: re-lease and check
  `pool.py list`.
* **Leases change under you:** `pool.py lease` and `release` take `--agent NAME` (a bare IP released nothing). The
  pool can stop or turn Off a leased VM mid-session: on an ssh timeout check `pool.py list`, release and lease
  again. A VM leased with `--agent` can later show as another holder's (release then finds nothing).
* **Pool contention:** at the cap a lease (`pool.py lease`, `verify.py --pool`, `appfile.py --pool`) waits in a
  first-come queue (`--wait S`, 1800; `pool.py list` shows the queue; D232) instead of exiting, and leases.lock is
  held only to decide and save. `verify.py --pool` leases before it takes the importer lock (D234). The queue
  does not wait for host memory: a lease can fail for 25 minutes with "Unable to allocate 4096 MB" while a
  memory-heavy program runs on the host: retry. With the lock busy, a scratch copy
  compiles alone on a leased VM in ~20 s (vc6.py compile with the library's flags).
* **Host memory:** `verify.py --pool K` exits when the host lacks memory for the VMs it would start (2 GB startup
  plus the reserve each): pass `--pool` = the VMs already running. `appfile.py --pool` exits at once on "host
  memory ... not starting" instead of waiting, and `pool.py lease` exits at once with no VM: retry in a loop
  (sleep 45-60 s) until the output holds an IP. Low host memory (MSBuild's `.NET Host`
  workers: `dotnet build-server shutdown`) shows as compile errors.
* **At most 4 parallel `appfile.py match --pool` jobs:** more made pool.py half-create a clone (Off, 0 GB disk;
  remove it with `pool.py destroy`). (Reported once; the limit is not measured.)
* **`appfile.py match --pool` refuses a stale object** (the object older than the draft, or lacking a marked
  non-inline function): rerun.
* **A worktree verify on a freshly leased VM can fail with "cannot open linearbu.h":** delete
  `%TEMP%\enb_vc6_synccache_<ip>.json` or run a full verify first.
* **A one-off C1083 on a tracked, unchanged header:** a worktree's first `build.py build --lib app` failed one file
  with "Cannot open include file '..\packets\pkcolorization.h'"; the rerun built every file. Rerun once before
  blaming a commit; a new file can fail the same way while other worktrees verify (read `compile_errors` in the app library.json).
* **A pool VM failing with "tar: Can't unlink" during the source sync:** rerun once; it passed the second time.
* **Background jobs:** a Bash `&` job is killed when the tool call ends (`cmd &` also leaves a verify detached):
  use the tool's run_in_background. Claude
  Code's memory reaper can kill background shells (lease-wait loops); a process started detached (PowerShell
  `Start-Process`) survives. TaskStop of a background shell leaves its child processes: check Win32_Process before
  restarting.

### 3.6 RE tools, Ghidra and REA

* `.work/pseudo/`: never delete it, and remove a worktree only with `wt.py remove` (CLAUDE.md, RE tools step 3).
  A zip copy sits in `.work/pseudo-backup/`.
* **appfile.py markers:** `--diff` shows only the marked functions. A `// @0xADDR` marker attaches to the first line within the next 5 that is neither
  blank, `//` nor `#pragma`, so a `/* */` block in between is read as the definition (a `#pragma
  auto_inline(off)` between marker and definition is skipped; it still counts toward the 5 lines). Stripping a trailing comment can make a
  declaration look like a definition. A marker with a trailing comment is ignored (put the bare marker directly
  above); the tree's `Exact` / `NOT EXACT` comments don't attach; in-class inline definitions are ignored
  (define them out of class). Markers apply in order among functions with the same demangled name: mark every
  overload, or rename while drafting, since same-named overloads (two constructors) get mixed up: check with
  objdiff and the mangled name. appfile prints EXACT in capitals. Reported, not verified against the code (which
  skips `//` lines): a plain `// 0xADDR` line between marker and function overrides it. `--diff NAME` prints
  nothing for a demangled name, for a function without a marker, or on a tree file (copy it to a draft): use
  `objdiff.py .work/<agent>/appfile/obj/X.obj MANGLED ADDR` (the name from `objdiff.py --list`). Constructors
  can fall outside `appfile.py show`'s `__FILE__` windows. mark.py's `NAME=ADDR` can't name `operator=`.
* **appfile.py invocation:** `--pool` takes no count; a marker is exactly `// @0xADDR` on its own line; a header copy
  placed beside the draft is not used (the tree's header is); parallel runs under one agent name share one objects
  directory, so give each run its own `--agent`; `-D V=1` breaks `<utility>`; the markers over blank lines count the
  blank lines; a static generated stand-in is reported "defined but not compiled"; `explain.py` wants appfile's own
  object path (`.work/<agent>/appfile/obj/X.obj`).
* **appfile.py, `match` and `show` limits:** `match` wipes the agent's `appfile/obj` on every run, so two runs of same-named drafts under one `--agent` overwrite each other's object (FileNotFoundError or another variant's result): run variants in series or one `ENB_AGENT` each. `match` refuses "the object lacks void" for a parenthesised definition `static void (Name)(...)` unless `--allow-missing`. `show` refuses a file app-functions.json does not attribute (a new file): a throwaway wrapper that sets the range's `file` in memory gives show and draft (Tools item: a `--range` option). A `#line` in a draft is easy to lose when the file goes into the tree: count the padding from the draft without it.
* **appfile.py, markers and includes:** a `// @0xADDR` marker skips a `template <...>` line on its own, so it never attaches to a template function: join the template line to the definition line. `--diff` prints nothing once the marker line is replaced by a `// NOT EXACT` line: run explain.py on `.work/<agent>/appfile/obj/X.obj`. A draft outside app/ with relative `..\..\wwlib\x.h` includes needs `-I app/shared -I wwlib -I wwdebug -I wwmath`; `-I DIR` did not resolve app/shared's subdirectory includes (`auxdata\permstr.h`) even absolute: run `match` on the tree path with the default `-I auto`. A draft can be tried lock-free while your own verify waits: copy the .cpp and the edited header into `.work/<id>/dr/` (reported: quoted includes resolve from the draft's folder first, against the invocation bullet above: check which header the object used).
* **appfile.py PREFIX means "ours is a prefix of the client's measured size":** the client size runs to the next
  verified function start, so it can include int3 padding after the client's `ret`, inter-object padding, or a
  file-local static / initialiser stored without a thunk (the tools' function starts lack those). Verify counts
  such a function exact, and verify's count decides; check the disassembly (`client.py dis`) for the padding or
  the following symbol. A start missing from the function starts makes match report the previous function as
  PREFIX: give it its own marker. Example: 0x81EEA0, 402 bytes, padded to 0x81F0DF. The last function before a
  PermStringInit static reports PREFIX the same way, and REL shows on float immediates. A trailing jump table or `$E` initialiser makes PREFIX mean exact; conversely
  appfile can say EXACT where verify does not (a jump table's bytes, a stale callee name in symbols.json).
* **explain.py's class can be wrong:** it says inline-budget for a function followed by a static initialiser (read
  objdiff first), and register-only where the lever was an inline-budget one (an `x.clear()` two layers down).
* **appfile.py false exacts:** one template compiled from two files (with and without an EH frame) can match at
  the other instantiation's address, and a static can pair with another one whose initialiser is identical (a
  16x16 icon size, 0x531630): check the address and read the initialiser's disassembly.
* **appfile.py and verify can disagree:** appfile's automatic include list omits `SDK\directx8\include` and
  verify's `source_alias`, so a function exact under appfile can be non-exact under verify (two reloads swapped),
  and `#include "soundlib.h"` compiles under appfile but fails in verify (use `..\audio\soundlib.h`). For a
  result only verify shows, compile with verify's includes, defines, flags and alias (`vc6.compile_sources` with
  `importer.tree_includes()` and `source_alias`, then objmatch, ~3 s a form: `.work/sr1/vsweep3.py`), or
  objmatch verify's own object under `.work/<agent>/tmp/enb_import/verify-app/`. Include order can be a lever,
  and a fragile one.
* **appfile.py scratch:** without `ENB_AGENT` its objects land in the shared `.work/appfile/appfile/obj`, where
  another builder's run overwrites them; every match clears the agent's obj directory, which then holds only the
  last run's object (copy the .obj before objdiff). Concurrent matches under one agent fail ("objs.tar missing",
  FileNotFoundError on the .obj): run one at a time per agent and worktree (two at once was reported to work;
  not re-tested). In a worktree it needs `--pool`. A match takes ~1 min per form under lock contention. It reads
  stdin (`< /dev/null` in loops; reported). Its definition regex reads `static void (Name)(` as a function named
  `void` (harmless).
* **appfile.py markers:** a marker on the blank line above an `extern` attaches to the extern, not to the function
  below; a function followed by the 32-byte `$E` initialiser takes a PREFIX (verify credits both); `attempts.py
  record --forms` takes a count.
* **appfile.py includes:** `-I auto` is the default; an explicit `-I` list without `auto` sends no headers to
  the VM, so a VM holding an old copy fails with C1083: add `-I auto` to the list. A header's relative includes
  resolve from the including file. Where a client/ draft may sit (conflicting reports, unresolved): in
  `.work/<id>/client/` so `..\shared\` resolves; under `.work/<id>/m/` (resolves and compiles while a verify
  runs); or only inside app/client (under .work it failed with C1083). A gameedit draft's relative include is
  one level deeper (`..\..\..\wwlib\nstrdup.h`; reported before `-I auto` became the default). An explicit `-I
  app/client ...` list failed on co.h's `..\packets\` include. Edit the tree header, not a copy, for a header's
  own relative includes. A draft-folder copy of a shared header can't override it when its include guard is
  already defined: give the copy a unique name and `#include` it on the draft's line 1. A scratch header beside
  a marked copy stays on the VM after you delete it locally, and the next match fails with C1083: put an
  unmodified copy back. Reported before `-I auto` mixing (not rechecked): avatar.cpp needed `-I
  C:/EnBDevelopment/SDK/radtools/include_enb -I C:/EnBDevelopment/SDK/directx8/include` (mss.h), an app/shared
  draft needed `-I wwlib -I ww3d2 -I wwmath -I wwdebug -I wwsaveload -I wwaudio`, and concurrent runs on sibling
  drafts failed at random with `..\audio\surfacetypes.h` not found even with distinct `ENB_AGENT` (run them one
  at a time).
* **asserts.txt and funcmatch take the nearest preceding path, not the function's `__FILE__`:** a string at an
  object's edge, or in a shifted run, is filed under the neighbouring file. Check an attribution against the
  neighbouring rows' line numbers and the `__FILE__` the function itself pushes (`client.py refs` on the
  string). Known shifted runs: around
  0xB71324 and 0xB7CB6C-0xB7CD30 (filed under ui_buff.cpp, ui_chat_panel.cpp's). Example: 0xBA8FE8 / 0xBA9018,
  listed under chartemplate.cpp, are chat_defs.cpp's (0x830770's `__FILE__`).
* **`explain.py OBJ 'Class::name' ADDR`** takes the function whose mangled, demangled or bare method name equals
  the argument, and falls back to substrings only when none does. `--n` is the candidate index among overloads,
  not a row count (out of range lists the candidates); `--window 200` shows a whole function.
* **explain.py's client window can run through int3 padding into the next function or stub,** inflating the
  residue and naming another function's lever (773 client instructions against 388 real): trim the window to the
  function's end. False classes: 'relocation: X vs Unknown_X' can be a symbols.json naming difference, not code
  (Fail_If_Report 0x4E9450); on a draft with an assert it prints "relocation: string differs" (the draft's own
  `__FILE__`) and hides the real class; a static's atexit `$E` shows as a relocation before the real class;
  and libc names in symbols.json can be wrong (0xA297A7 `_wcscoll` is `strcoll`; 0xA27B12 / 0xA284C7 `_wfopen` /
  `_fwprintf` are fopen / fprintf): check a libc body before relying on its name.
* **On a big function objdiff's similarity hardly moves:** (appfile's `none` / similarity can also come from relocation-only differences: count the non-REL `*` rows in `--diff`) read explain.py's instruction count and
  register-masked residue instead, and compare the call relocations per callee (`objrel.py`) to see which callees
  inline. For a function with a different instruction count, `.work/t20/align.py` lines up explain.py's
  `--window` columns.
* **objdiff.py display:** it prints every relocated call as `call REL`, so an IAT `call dword ptr [REL]` always
  shows as a mismatch even when the bytes are equal; it stars a row whose relocated immediate is 0; a file-static
  reached by displacement (`[GutterColors+8]`) shows unrelocated. More in tools/re/README.md (Quirks).
* **objdiff, `appfile.py --diff` and explain.py mask targets and literals:** "identical" there while objmatch says
  `none`, or a similarity like 0.78 with every visible line equal, means compare raw bytes
  (`.work/pk2/patcmp.py OBJ SYM ADDR`): a jump displacement (two `je` to a shared exit), a `jl` against `jb` (a
  starred masked line; a filter dropping `REL` lines hides it) or a literal (0x3DCCCCCB against 0x3DCCCCCC; `""`
  against `" "`: compare the string's address) was the whole difference.
* **objmatch finds symbols.json at `__file__/../../docs/data`:** a copy of objmatch.py outside tools/re silently
  runs without symbols (D80 and the self name do nothing; false "losses"): measure variants from inside tools/re.
* **`tools/re/symbols.py` with no arguments refuses** (it used to regenerate symbols.json, ghidra_symbols.py and
  coverage.md without any object's names); a regeneration is the coordinator's (3.4, symbols.json regeneration).
* **Check objmatch's status of the STL helpers a caller should call out of line** in your variant's obj before
  working toward them: `multiple` (identical client copies, e.g. `vector<RefCountPtr<T>>::insert` 0x47D690) never
  becomes a pair, however byte-identical; only the caller itself can gain.
* **variants.py:** use `variants.py try --fit` (it keeps the definition's line count and puts each assert on its
  line). When applying, restore every touched line, reproduce the line counts with blank lines after the function
  (in the file's padding), not the fit's packed braces or `#line`, and re-add the `// NAME` tags and address
  comments variants drop. `try` compares one object's exact set by address: a filter, never verify or pairs_cmp;
  every applied variant goes through the full verify (a gain-only try lost another address outright).
  Byte-identical twin functions report losses and gains: apply the form to both. Distrust an 'exact' that needs
  inline asm (D70); don't apply a gain-only variant that lowers its own function's similarity by more than 0.1; a
  stash in docs/attempts holds one function at given lines: check they still hold it before splicing. A best form stashed there may never have been committed: compare the tree's residue with the attempts row before a round.
* **tryv2.py:** its single-file verdict on register allocation needs the full verify to confirm. It refuses an anchor that occurs twice. It writes tree files with `open(..., 'w')`: a concurrent
  reader makes it fail (OSError 22 / PermissionError, also while restoring), later variants are skipped silently
  and a variant can stay in the tree file: check the log's variant lines and grep / `git status` after every run.
  It deletes its variant objects (`.work/lg3/keep.py` keeps each .obj); copy `tryv2-base` objects out at once
  (shared between builders). To try a shared header's macro without touching it, copy the header into a private
  folder with the same include guard, replace each compiled file's line-1 comment with `#include "<copy>.h"` (no
  line moves) and pass `EXTRA_INC = ['libs/sgistl', '<folder>']` (`.work/fi1/v2.py`); other builders never compile
  a half-done header.
* **Other variant runners:** tryvariants.py edits one top-level library file and refuses app subdirectories ("no
  such file in the tree"; tools/vc6/README.md): use tryv2.py, or `appfile.py match --pool` on an untracked marked
  copy next to the tree file (`X_v.cpp`; ~16 s a variant, no importer lock; delete the copy before verify, or
  verify compiles it too). With `#define private public` members mangle as public (`QAE`), and objdiff needs that
  name. To probe several forms in one compile, add members V1..V6 to the draft's header copy and objdiff each
  (each counts as a form, D141).
* **Text appended by a script:** text appended to an `#include` line is dropped by the preprocessor, and text appended after a
  `//` comment on a header line is commented out (tryv2 helpers): add a line, then check `__LINE__`; a tryv2 variant replaces the whole file
  when its anchors repeat in it.
* **names.py run while a tryv2 variant is in the tree** writes the variant's line numbers into not-exact.md: rerun
  names.py on the restored file before committing.
* **attempts.py:** never split attempts.csv on line endings (its rows hold quoted multi-line text; one split
  truncated 157 rows): use attempts.py or the csv module. `record` takes `--score` float, `--forms` a count,
  `--stash` (function only), the description in `--evidence`; it has no `--lever` (`check --lever` has) and
  refuses a `--blocker` outside its classes (no class for "identical copies waiting on a tie-break": use
  `relocation`). `check` matches by bare member name across classes, flags any address named in a comment, and
  reports pk.h's parked 0x8770A0 for every packet file (`--ack 0x8770A0`). `--stash` writes an untracked
  docs/attempts/<ADDR>/NAME.txt: add it to the commit.
* **Sweeps: take addresses from attempts.csv or the NOT EXACT markers, not gaps.py:** its app pairings are often
  wrong (a 2-3 instruction client window, or a neighbour's address); see tools/re/README.md on gaps.py.
* **References through incremental-link thunks:** vtable slots can point at thunks (0x40xxxx; the packet
  factory's `Process_Packet` slot +0x08): resolve them with `ClientImage.incremental_thunks()` before looking the
  target up. `client.py refs` and clientimage.py's `dword_refs` miss a vtable's reference to a function the table
  holds through a thunk: find the E9 thunk first, then references to it. Live or dormant code: look for call,
  jump and pointer references to the function and to its incremental thunk (`.work/px2/thunkrefs.py`).
* **Vtables and .rdata:** count a table's slots up to its 0 entry, not up to the next constant (a subclass's
  float constant can sit right after its own vtable). The pooled empty string literal 0xBDA908 lies in .bss, so
  `ClientImage.cstr()` returns None for it: read "" there. symbols.json can name a base's vtable after a derived
  class (0xAEDE88 is PacketClass's, not PushMessageLinePacketClass's), so explain.py's "relocation" class on code
  that destroys such a temporary is a false lead. Header-local inline `return false` slots are real (textgadg.h's
  0x65C920 "folded"): read the client's slot (`clientimage.ClientImage().u32(vt + 4 * slot)`, then the thunk)
  before suspecting them.
* **Pseudocode and Ghidra:** pseudocode can be wrong (check `client.py dis`); pseudocode.py drops call arguments
  in long functions (0x600000: the pushes in `client.py dis` are the check), and a concurrent `pseudocode.py`
  run clobbers the shared `.work/ghidra/targets.tsv` (rerun if "ok N" is short). capstone shows the CRT's
  `fpatan` helpers as plain calls: read the callee's body before trusting a doc's asin / acos. A `__thiscall`
  can be a cdecl free function, and Ghidra can attribute an argument pushed for a later call to the call before
  it: check each callee's `ret N` and the caller's pushes before writing a member. Read a template's value type
  from Ghidra's mangled name (`...I_` is unsigned int: a `hash_map<string, int>` lost two pairs to the other
  instance).
* **Scripts on Windows:** Python's `subprocess` 'bash' can resolve to WSL's bash (empty output): use Git's
  `C:\Program Files\Git\usr\bin\bash.exe` by full path. In Git Bash `cmd //c mklink //J` makes broken links: use
  PowerShell `New-Item -ItemType Junction`. `client.py dis` on a cp1252 console: `PYTHONIOENCODING=utf-8`.
* **Tool changes:** compare outputs on a snapshot of the verify objects. Starts are verified (D27).
* **Placement evidence from raw scans:** a raw call scan counts the unwind funclets (0xA9F000 and above) as callers (a
  destructor "with 14 callers" may have none in code): filter them. A tree comment that names the next object's first
  address makes that address look like the file's own in an address grep: a mention is not a placement. A scan of a function's
  immediates for source paths can hit by accident (0x449EA0 "references" two files; app-functions.json leaves it unattributed): check
  the assert call. `.inl` asserts (spline.inl) mark template code instantiated in the using object, not the .cpp of the same name.
  The map's function counts can omit `<string>` initialisers and adjustor thunks.

### 3.7 Harnesses, programs, unattended runs

Builds and links (every program: clienttest, udpreplay, kerneltest, VIDTEST, the POC):

* **Build app.lib before linking.** `build.py link`, `clienttest.py build`, `udpreplay.py build` and the POC
  link take `build/<lib>/<lib>.lib` as they are, and `importer.py verify` does not rewrite app.lib: run
  `build.py build --lib app` (`--allow-dirty` for your uncommitted files) first. A stale lib hides LNK2005s
  and unresolved symbols, shows other builders' committed work as differences from expected.txt, or ships old
  code with no warning (a library fixed in source but not rebuilt: check the function in the exe at its
  client.map address). `build.py link --program clientpoc` gives the exe a fresh date with old code inside. A
  dirty tree makes `build.py --lib app` refuse unless `--allow-dirty`, leaving app.lib stale: a renamed
  function then passes the harness build as a gen stand-in ("gen Message_Error") with only an expected.txt
  mismatch. After a file rename delete its stale `build/app/obj/<old file>.obj` (it can still link in). Never
  run clienttest on an app.lib with a failed file (the unresolved list and trace diff are that file's).
  `build.py link --out DIR` reads every library from DIR: copy the whole build/ there.
* **build.py builds tracked files only** (`git ls-files`). `build.py --lib` refuses a dirty library unless
  `--allow-dirty`; even then a new untracked file is silently left out and a gen stand-in (of its constructor,
  say) takes its place: `git add -N FILE` (or commit it, a detached WIP commit inside the worktree will do)
  before building. A tracked app file you delete breaks build.py's definitions scan (FileNotFoundError) until
  the deletion is staged (`git rm --cached`) (not re-verified since the report). POC-only edits under
  programs/clientpoc count only once linked: `build.py build --lib app` does not compile them.
* **A worktree has no build/app.lib.** `wt.py new` copies the other libraries, the .dgram captures, build/art
  (MSS32.DLL in it) and build/d3d8shot as the main tree has them at that moment. In a fresh worktree run
  `build.py build --lib app --allow-dirty` (before `udpreplay build` or any link). A worktree made before
  2026-11-10, or before the main tree had the folders, lacks them: clienttest "wrote no trace (exit 3221225781)"
  (0xC0000135, STATUS_DLL_NOT_FOUND); a POC replay without d3d8.dll; copy them in. kerneltest in a worktree: pass
  the main tree's build/ as `--out`.
* **Link the programs after adding a function to a library file or touching stubs/ or pk.h** (`python
  tools/vc6/build.py link`): a stub in stubs/app_stubs.cpp that calls RawPacketClass pulled pk.obj and ~280
  externals into VIDTEST, so packet stand-ins go in tests/udpreplay/, never stubs/app_stubs.cpp. A commit
  subject must not claim a program links unless that command ran at that commit. New Win32 imports in app code
  (WinInet) need the library in clienttest.py's and build.py clientpoc's `sys` lists; any harness linking
  CordCon needs the old iostream (LIBCIMT); a file-local `Stub_Log_Line` declaration needs `extern "C"` (else
  LNK2005). Any program that constructs an SClient needs a stub for the registry read 0x75A280 (D69).
* **A link with unresolved symbols still writes and deploys the exe.** `build.py link` prints "clientpoc: exe
  NOT written; N unresolved" and deploys anyway: read the 'deployed' line and the link log's `unresolved:`
  lines. A call to an unresolved symbol (/force:unresolved) jumps into the PE header: EIP 0x400003 with no
  caller (the header's bytes pop the return address). Match the unresolved list to its objects
  (`.work/WE1/coffsyms.py`) and hand-write the stand-in.
* **Reading a link failure.** One compile error (often C2027 once a stand-in class became real) gives hundreds
  of unresolved and LNK2005 lines, all fallout: read the `cl:` lines first (`grep -v unresolved`). Fixing one
  LNK2005 layer exposes the next. Which symbol pulled an object: `set LINK=/VERBOSE` in the link batch (`Found
  X / Referenced in Y / Loaded lib(obj)` chains back to the program's own object).
* **Shared build/.** `udpreplay.py build` writes the shared build/ and `conn` runs whatever exe is there:
  build right before `conn`, or in a clean worktree. Two `static` tables with the same name break udpreplay's
  single-TU build. A build or deploy fails with PermissionError while another builder's udpreplay.exe or
  client.exe runs: retry after ~45 s.

Stand-ins and gen_stubs (clienttest, udpreplay, kerneltest, the POC):

* **e2e.py (D237, tools/harness/README.md):** every POC check builds `--lib binkmovie` before the link, else `Play`
  is a generated stand-in and the start-up movie runs to its end; `build.py` has no `--lock-wait`; a trigger file
  must be renamed into place; input is tagged 0x5043 and `ENB_POC_INPUT_TAG=1` drops untagged input.
* **Retiring a stand-in must not take its neighbours with it** (deleting a hand stand-in from stubs/app_stubs.cpp
  also removed the CRT wrappers beside it): gen_stubs' generated stand-ins return null, `standins.py check` and the
  link gate pass, and only a POC run shows it (a crash on a null inside the real function that copies it). Run
  the e2e set after every landing that retires a stand-in (D237). The reverse: a stand-in for a function written
  later becomes LNK2005; delete it in the commit that adds the function.
* **A landed app definition collides with stand-ins** in three places: programs/clientpoc/poc_standins.cpp /
  poc_world*.cpp, stubs/app_stubs.cpp and tests/clienttest/client_stubs.cpp. `python tools/vc6/standins.py
  check` finds them (wt.py bring-over refuses on it); a stand-in that must stay gets `// standins: keep (why)`
  on or just above its line. New callers can also make app.lib pull an object clienttest compiles itself
  (LNK2005 with a clean verify): `python tools/vc6/linkgate.py check` (bring-over runs it). A new app file
  that defines a stubbed function deletes its stub and joins udpreplay.py's source list. A `// standins: keep`
  marker does not help when clienttest links the object itself (it links ui_chat_panel.obj and sclient.obj): a
  real function there that clienttest stubs gives LNK2005, which linkgate catches.
* **Generated stand-ins answer 0.** A gen stand-in of a bool hook answers false (Pre_Render hides every hooked
  model), of a data pointer is NULL (newly real code that dereferences it crashes: use the tree's symbol, e.g.
  DX8Wrapper::CurrentCaps), of a data global `Unknown_XXXXXX` may really be a library's static or a global the
  client starts at 1 (check `client.py refs ADDR`); an uninitialised gen object corrupts memory every frame. A
  missing effect or control in the POC is most often a gen stand-in or empty body on its path: grep stubs.log
  for `gen` lines called after the screen appears and rebuild those. The reverse also crashes: a 0-returning
  gen stub kept its callers on the "no item" path, and the real function returns data that walks them into
  other gen stand-ins (an access violation in the shortcut bar's gadget constructor): run the in-world replay
  on a baseline exe and on the change before landing. A gen stub returning a NULL `const char *` crashes
  string concatenation. App externs for WW3D statics get no POC stand-in: call `WW3D::Get_Frame_Time()` /
  `Get_Sync_Time()`.
* **A class going real runs its set-ups for real.** They create every window the class owns, so each class the
  tree lacks needs a POC stand-in (a generated constructor leaves no vtable and the POC crashes); once a real
  class replaces a gen stub, its constructor's `new` of a still-stubbed class leaves no vtable: rebuild that
  class's start too, and look for an already exact class under another name. A POC stand-in class a real
  caller reaches needs every virtual slot the caller uses (a proxy button without +0xA4 called 0); slot-window
  virtuals the POC filled with empty windows come back real. Swapping a window stand-in for the real class
  (ui_chat_panel, cpacket) pulls app.lib's window object in: it needs the window's .rdata constants
  (poc_data.cpp, tests/clienttest/ui_avsel_stubs.cpp) and clienttest's fakes of its callees renamed per object
  through clienttest.py's `#define`; run `linkgate.py check` after the switch (an `#include` that replaces
  exactly one stand-in line keeps every `__LINE__`). A `RefCountPtr<T>` member in a header breaks every file
  that builds the class without T complete (clienttest): keep it a raw pointer until the constructor is
  rebuilt. clienttest's fakes must cover every function that goes from stub to real; removing a clienttest
  stub means the test sets the state the real code reads (expected.txt updated).
* **What gen_stubs cannot write.** A struct or class by value (parameter or return: std::string, Vector3,
  KernelEventClass, MoneyClass operator+), `std::vector<struct>` (the call links to 0), a Matrix3D return, a
  non-const global object, `const int` data, `const RefCountPtr<T>&`, template-pointer signatures, data
  globals (`--link` skips `MonoClass Mono`). Its output lists them as `// skipped`: hand-write them
  (client_stubs.cpp for clienttest, stubs/app_stubs.cpp for the programs) or compile the real .cpp, and delete
  them when the real code lands. A VC6 member-template stand-in defines the out-of-class specialisations as
  plain overloads in its own minimal class declaration (tests/udpreplay/shipcust_stubs.cpp). It writes pointer
  parameters as `class` (a `struct` parameter mangles PAU and never resolves), one pointer-returning signature
  per name (a void overload such as GadgetClass::Unknown_08 needs a hand stand-in; clienttest.py does not
  apply `link_aliases.txt`), and a second, differently mangled member under `#define protected public` (reach
  the member through a derived class). One compile error in the generated file used to lose every stand-in
  (guarded now).
* **A rename or signature fix strands the harness's stand-ins.** Pointing app code at a library symbol, fixing
  a caller's declaration (the mangled name changes) or renaming a stand-in to the real class leaves the test's
  old stand-in behind: it pulls a library object (LNK2005 a chain later) or no longer intercepts, and the real
  function runs on a fake (0xC0000005). After such a change grep tests/ (kerneltest.py SOURCES, clienttest's
  lists) for the old form and run the D182 check. A stand-in class the tree now declares for real leaves
  clienttest.cpp (C2011). A file-local declaration of another file's function must spell the same parameter
  and return types (`void` vs `bool` included): otherwise it links to a gen even when the real one is exact
  (clienttest LNK1120 on InvDataClass::Unknown_688830). Template bodies added to a header that tsk_login.h
  includes broke kerneltest.
* **Stand-ins for an object the link already pulls.** A stand-in for one member of an app.lib or library
  object other code pulls gives LNK2005: stand in for every member the program reaches (each WWAudio member
  the new code calls; Play_3D_Sound beside Play_2D_Sound, else all of soundlib.obj links; wlib Wstring
  operators a new app object pulls), or compile the real file into clienttest with the function renamed
  (clienttest.py's APP list), or redirect one call with `#define NAME Clienttest_NAME` in that file's wrapper
  in clienttest.py and define the renamed method in client_stubs.cpp. Hand stand-ins are deleted in the commit
  that rebuilds the function (grep tests/ for each signature, udpreplay included); a faked app function made
  real: rename the fake, then link. A rebuilt callee or gen-stubbed constructor can pull its whole object in
  (hand stand-ins for its by-value-return callees; control_keys_manager.obj needed one); co.obj drags in
  aux_roots.obj once sclient.cpp news DeltaPlayerClass or the Hull root: stand in each symbol only
  aux_roots.obj defines (tests/clienttest/aux_roots_stubs.cpp). ui_registration.h pulls in many UI headers.
  clienttest.py strips udpreplay's staged stub of a function that became real. A constant defined only in
  programs/clientpoc/ (poc_data.cpp) needs its twin in clienttest's stand-ins.

clienttest (`python tools/harness/clienttest.py build|run`):

* **clienttest's failure layers.** LNK2001 / LNK2005 first; then a missing DLL (0xC0000135 at load, no trace
  file: copy MSS32.DLL or stand in the import, e.g. `_imp__AuthLoginLib_GetComponentFactory`); then a pre-main
  static-initialiser crash (no trace file: a link stub calling Trace before Out exists); then runtime crashes
  in newly real code. After a crash `.work/harness/clienttest/out.txt` is the last good run's; the real output
  is clienttest.out.txt. The crash address prints only when the exe runs by hand; `.work/ct3-dbg.py` (ctypes,
  Wow64GetThreadContext) prints a 32-bit exe's exception address and stack. VC6 with /GX catches an access
  violation in a constructor and unwinds before crashing: flush the trace per line. A reported clienttest
  crash at HEAD in a worktree: check the link first.
* **Rebuilding a callee clienttest drives: run clienttest before landing.** A rebuilt function in an object
  the link pulls runs its real code on the fakes (0x513700 on the fake main view, no StarField). Fakes then
  need the real size and vtable; a table-calling member made real while its object's constructor is still gen
  crashes at run time (verify does not catch it); a gen stub that tolerated a null `this` crashes once real:
  give the test a zeroed fake object. When cpacket.cpp gains handlers, re-run and refresh expected.txt with
  `--update` after checking the difference.
  A real class that indexes a sub-object through a zeroed fake crashes (0xC0000005; DebrisInterfaceClass +0x58 indexes `Fields[Mode]`):
  grow the fake with one zeroed sub-object.
* **linkgate, clienttest and `build.py` build only tracked files** (`git add -N` a new file or commit, then run them;
  linkgate's occasional sync.tar failure passes on a rerun; run `linkgate.py check --tree .` after any rebase of a
  header-changing branch; `--tree` needs a path); clienttest has its own library list: a commit
  that adds a Win32 import (ShellExecuteExA) fails linkgate with an unresolved `__imp__...` until the library (SHELL32.LIB) is added to clienttest.py and
  the program's build.py entry. ATL headers compile in the VMs. A virtual destructor in a layout header breaks the
  harness stand-ins: run linkgate after the layout commit, before the first verify.
* **clienttest's fakes.** Set a fake only for the section that needs it, so other handlers' FAIL_IF paths stay
  covered (the radar stays NULL for WARP_INDEX 156). Fake objects called through a slot need a `__stdcall`
  slot with the callee's argument count (Set_LOD_Bias, slot 75; ~ClientObjectClass deletes AuxData through
  slot +0x04: `__stdcall(unsigned)`). A stand-in object whose real constructor isn't run needs
  `Node.Set(object)` before Add_Object. DX8Wrapper::Init clears D3DDevice after the static initialisers:
  install the fake device from main (d3d_stubs.cpp). Real kernel / input code needs a fake task on a real
  KernelClass and an empty KeyQueueClass. Synthetic sections that create models run after section (3), or they
  renumber its models; a packet whose member classes are gen stubs (no vtable) can't go through Build: call
  Process_Packet on a value-built packet.
* **clienttest's data files.** co.cpp's loader 0x6F51D0 reads basset.ini, Set_Model's radius 0x6F39F0 and the
  mount points 0x7251C0 read cbasset.ini, from the working directory (clienttest's copies in its data folder).
  Once eff_o.obj links in it needs tests/clienttest/data/effect.ini (EA's shipped EFFECT sections for the
  descs a session asks for, and their links) and eff_ base assets in basset.ini numbered from 1001; concrete
  effect classes not yet rebuilt need stand-ins on their real ends. A real app function clienttest reaches
  needs its data in the harness (a NULL CameraManager crashed 0x79B2B0).

udpreplay:

* **udpreplay's gate is `conn`, not `run`:** `run alex7` fails at HEAD (one rejected datagram); `conn alex3` /
  `conn alex7` pass. A baseline worktree needs the libraries and `.work/harness/*.dgram` (`wt.py new` copies
  them). loop0 can differ once (the 90 s give-up reads time(0)): rerun first. Harness counters live outside
  the client's classes (D176: `tests/udpreplay/cord_counters.h`, `CORD_HARNESS_COUNT`). udpreplay compiles its
  app files as one translation unit: a header's macro gate is defined at the top of app_sources.cpp (written
  by tools/harness/udpreplay.py).

The POC client (instances and your own build: builders.md §2 "POC instances"):

* **Running it.** Run clientpoc.py from the main tree with `--exe`; instances keep their saved options (a
  stale option changes the run). Name crash stacks from the run's own clientpoc.map, never retail's
  (`__validdrive+0x...` is the map's CRT tail). Read the run's `client exit:` line: "still running when this
  run killed it" also appears when antprox logged `Client exited` right after the quit: compare antprox's time
  before calling it a hang. Check the host's free memory before trusting a stalled replay (a 7 GB game
  collapsed the frame rate). The POC writes *.th6 thumbnails into ClientPOC\Data\client\mixfiles, as retail
  does; the real 0x442AC0 reads the registry (read-only). The client's message server (-ENABLE_MESSAGES) sends
  only while a listener is connected, at most once a second: start-up reports before its first run are lost
  (the POC tees them into stubs.log). WinMain stand-ins that open dialogs (0x847D20 shows the ROC text in
  retail too) stay (D69). `.work/poc/runs.md` row numbers collide: read the last row before appending.
* **Input.** The client reads the cursor through GetCursorPos / GetMessagePos, moves it with SetCursorPos and
  takes input only while it is the foreground window (input.cpp 0x42EE90): drive it with `clientpoc.py --input
  FILE` (poc_input.cpp's virtual cursor), never the real desktop. Input times are ms from the window's start
  (the full creation flow needs `--seconds 150`); `T down X Y` / `T up X Y` drag; a click at 43,928 opens the
  inventory, 87,928 is the character button (`.work/hc2/in2b.txt`). A typed letter that hits hotkeys means
  control_keys.ini was not read. A replay that never selects a target can't show target lines (ui_scan
  0x60F1A0); antprox does not answer CLIENT_CHAT_REQUEST 0x00A3 (antprox's work, not the client's).
* **POC shot timing is not one number.** `--shots 1` is needed for any shot, and shots count presented frames,
  not ms: login runs ~5,000-15,000 frames/s, the world ~670, in-world shots around frame 330000-420000, retail
  ~4x slower late; the rate varies ~10 % between runs. Use `D3D8SHOT_EVERY=20000 D3D8SHOT_MAX=60` for the POC
  (`EVERY=2000 MAX=400` also works; `EVERY=10000` with `--retail`), map a frame to time with d3d8shot.log's
  ticks, dump a dense spread and keep the last frame with ship draws (fvf 0112). A 0.5 s effect needs a sweep
  of up to 64 shots around its time, picked by time / pixel stats (`.work/cs12/pick.py DIR`) before opening
  one image. An image that "stays on top" can be a stale back buffer (space mode clears no colour). 29
  `combo_ffactory.cpp(101)` FAIL_IF lines are retail behaviour (duplicate data rows), not a regression.
* **Draw dumps.** `D3D8SHOT_DUMP=N`, compared with `tools/harness/dumpcmp.py A B [--frame N]`; a dump run is
  slower (~2,200 fps against ~3,500): pick the frame from a run with the dump set. The main tree's
  build/d3d8shot/d3d8.dll may predate d3d8shot.c (no `draw N: material` lines): rebuild before diffing.
  d3d8shot.log logs every CreateFileA. A black model with AMBIENT=0 in a dump is a missing light, not a
  texture (`drawdump.py`).
* **Retail reference runs.** `clientpoc.py replay --retail` needs the d3d8shot proxy rebuilt with the input
  hooks (`python tools/harness/run_reference.py build --pool 1`, copy build/d3d8shot into a worktree); the
  retail exe is `<install>\release\client.exe`. `find_window` takes the terms dialog (#32770) as the input
  window; with `--retail` the kill ends antprox before it logs "Client exited", so the exit line says "still
  running when killed". Retail character select presents only a few thousand frames in ~50 s: shots at 300 /
  1000 / 3000.

Programs and tests (VIDTEST, kerneltest, drawcheck):

* **Exes from Python by absolute path** (`subprocess` can't start a relative `/` path). Tools that write the
  tree refuse a cwd in another tree: run them by absolute path from a worktree.
* **Programs:** `WW3DAssetManager` before `WW3D::Init` (else `ShatterSystem::Init` crashes); an HLOD's LOD 0
  can be empty (`Get_LOD_Count()-1`, recursively); `SortingRendererClass::Flush()` before a shot taken before
  `End_Render`. Custom render objects: tools/harness/README.md.
* **Kernel tests set `Quit` directly:** the pump (0x443E90, in Process_Messages) tests WM_QUIT only after
  GetMessage returns nonzero, so a posted quit doesn't stop the kernel.
* **Capturing originals, VIDTEST frames, d3d8 proxies:** tools/harness/README.md.
* **Any program that constructs LandClass reaches RegistryClass(create=true) through 0x468120** (D69): call
  `RegistryClass::Set_Read_Only(true)` first. LandClass writes the .pld beside the art it reads whenever
  loading it fails: always pass `-art` / `--art` with a copy of EBCONFIG made with `cp -rp` (without -p the
  .pld's date changes and Save_PLD rewrites it). drawcheck, vidtest_sbs and run_vidtest default to `build/art`
  when it exists (D120, `tools/harness/artdir.py`) and warn when it doesn't.

### 3.8 Git, docs and scripting hygiene

The shared index, heredocs, Git Bash paths and line endings: §3.0.

commit_mine.py:

* **commit_mine's `-U0` hunks merge adjacent added lines across builders:** a library-matching.md row, an
  imports.md line, a ledger description or (with `--path` / `"whole"`) every attempts row of the file next to
  another builder's uncommitted one travels with yours: put new rows somewhere other than next to another
  builder's. Hunk keys must be on changed lines (a key on an entry's first line misses an edit to its
  continuation; the hunk lands in the next commit) and specific: a key matches any hunk containing it, another
  builder's identical line included, so use a full key (`('app', 'client/x.`), not a file's stem. A whole-file
  path (`docs/library-matching.md` in "whole") sweeps other builders' rows: use a hunk key. attempts.csv,
  not-exact.md and names-review.md go row by row (only your rows travel).
* **What each commit_mine form commits.** `--path FILE` commits only that file's rows, not the source: list
  the source under `"whole"` in the SPEC; `--file <unit>` takes the unit's ledger entry (the SPEC's "files"
  key does not) as the working copy holds it when the commit runs: after every commit check `git show
  HEAD:docs/data/imports/<lib>/<name>.json` for your addresses. `--addr ADDR --path attempts.csv` commits one
  row. A widely included header you edited shifts names-review rows: (conflicting reports, unresolved) one
  says give commit_mine `--path <header>` too or the shifted rows stay behind; another says a header listed in
  "files" takes rows from the working copy with other builders' shifts, so leave its rows out and regenerate
  before the PR. names.py also rewrites rows of others' files: restore the leftover diff after your commit. A
  stale ` M` stat after commit_mine is cosmetic. commit_mine skips the library totals unless
  `--refresh-totals` (which takes other builders' counts too): leave totals to the coordinator.

wt.py:

* **`wt.py remove` deletes the worktree's `.work/` scratch:** keep scripts you want in the main tree's
  `.work/`. It refuses a branch with unlanded commits unless `--force` (also needed when files were copied
  in); `wt.py list --unlanded` lists them; `--delete-branch` drops wt/NAME. Never rebase a worktree by hand:
  `wt.py rebase ID` regenerates the generated files and stops on any other conflict (`--continue` or abort).

* **A script that reads and writes one file in one expression empties it** (open for write before the read ran):
  read into a variable first.

attempts.py and names.py:

* **`attempts.py record`** refuses a non-integer `--forms` and a `--blocker` outside its list (the forms' text
  goes in `--evidence`). In a shell loop, `set -- $a` cuts a symbol at its first space: quote each field or
  call once per row. `attempts.py check FILE` also flags functions compiled into the file through headers
  (0x726100 ClientClass for ui_mfd.cpp).
* **names.py quirks.** It files a NOT EXACT marker under the next function when a static helper's signature
  spans two lines (not re-verified). A second `// NAME` tag on a line swallows the first in names-review: put
  a new getter on another line with its own tag. Removing an extern `NAME` tag drops its row on the next run;
  match exact names (`Unknown_BFAE52` is another global than `Unknown_BFAE4F`). `names.py --tree DIR`
  regenerates from another worktree.

Edit scripts:

* **`open(p, 'w')` truncates p before anything else runs:** `open(p, 'w').write(open(p).read()...)` empties p,
  and an exception between `open(p, 'wb')` and the write (a bad `newline=` argument included) leaves it empty:
  build the content first, then write. Never copy a draft over the tree file: a stale or template-regenerated
  draft drops later edits and applied fixes; apply the form as an edit and check the tree file before
  verifying. Park scripts go stale when part of the parked work lands separately: diff the park folder against
  HEAD before using it.
* **Anchors and slices.** A Python edit that appends a declaration to a line ending in a `//` comment swallows
  it (C2039): prepend instead. A slice between two `str.index()` markers is empty when the second comes first,
  and `replace('', x)` destroys the file: assert it. A short anchor can be part of a longer line. Moving a
  declaration block "up to the next blank line" cuts classes that contain one: end the block at an explicit
  marker. A regex that strips `((T *)x)` casts also eats the call parentheses of `f((T *)x)` (C2065 on
  verify).
* **Sweeps and runners.** Check the variant index before any parsing that can raise, or a failed apply spins
  the loop forever. A runner that edits the tracked file breaks once a variant is committed: make each edit
  conditional on the old text being present. Run `git status` after every tryv2.py run (it once failed to
  restore a tree file, OSError 22, and left a variant in place; not re-verified, its docstring says it
  restores whatever happens). A failed helper's pipe can go unnoticed (`pad.py | tail -0` killed the script
  and a commit went in with `// @0x` markers): check exit codes. Chain dependent steps with `&&`, never `;` or
  `||` (a `cp` into the tree after a failed script; `importer.py replay` then commit, which can fail with
  PermissionError while another builder's replay holds `.work/replay`: rerun, not re-verified).
  `subprocess.run(['bash', ...])` from Windows Python can pick another bash and fail in seconds, leaving the
  old log in place (the sweep looks identical): use the full path to Git's bash.exe. In Bash, an expression
  ending in `}` inside `${5:-...}` truncates the default value.
* **A rename across our files must leave an imported library's own names alone:** app code reaches into
  imported classes by their names (soundlib.cpp's `using AudibleSoundClass::m_Unknown7C`); renaming
  `m_UnknownXX` in app/ broke it (12 compile errors, soundlib's pairs lost). Check every renamed name against
  the libraries' headers, and run a full verify (D159).

### 3.9 Library-specific

Looked up by file or library name, never read whole: grep the `####` headings for the files you touch. Headings
are alphabetical; add a note under its file's heading (merge with what is there) or add a heading.

#### app/packets (packet classes, pk.cpp, pk.h)

* The switch 0x869790 (`tools/re/pkclasses.py`) puts the 5000/30000 bodies last: take source order from unwind
  states. The match: `packet = new X(raw); break;`, `packet` uninitialised, the default `packet = 0;` before
  its assert (`return(new X(raw));` drops `mov esi, eax`). A pack(4) member changes a packet class's sizeof
  (0x5C for 0x60) and the switch stops matching: check it whenever a packet layout changes. Game-editor
  packets whose Process_Packet returns true are not in the switch, so pk_classes.h does not list them: find
  them by unmatched objects.
* A new header line above pk.h's PacketClass shifts its assert lines (~400 pairs). wwlib's pk.h shadows
  app/packets/pk.h: include `..\packets\pk.h`.
* `tools/re/pkdecode.py --report` says why a class doesn't decode yet; `pkemit.py --write` drafts the files.
  pkemit.py's NAMED keys must be addresses inside their own object. pkclasses.py sees a class as rebuilt only
  when `// opcode N (` sits directly above `class X`; a comment between them duplicates it in pk_classes.h.
  When a class in pkclasses.py's OVERRIDES is rebuilt with inline virtuals, empty its harness stub list
  (pk_ctor_stubs.cpp would redefine them).
* Objects: a class's inline virtuals (Get_Channel, Get_Type, Get_Name) come before its scalar deleting
  destructor; an out-of-line one comes after Build. An object without an assert ends at the guarded-static
  atexit call inside its last Print; handlers without asserts can be moved so the asserted ones sit on the
  client's lines. Game-editor objects share one `__FILE__`: read the assert string (0xBB2410:
  gameedit\pkgesave.cpp) before naming a pkunknown_* file, and rename the whole object.
* `PacketClass::Unknown_14`'s second argument is pushed early (`push -1` well before the call), so a
  disassembly filter on `push -1` hides it. explain.py reports a false relocation on PacketClass's vtable
  0xAEDE88 (a COMDAT name shared with PushMessageLinePacketClass): the masked residue is what counts. A
  RawPacketClass just written with Add can't be read back by a raw constructor (its cursor is at the end):
  push it through `Memory->Incoming`. Renaming a packet class's member breaks other files that read it
  (extrap.cpp read pkupdate's Planet Unknown14). symbol_substitution.cpp's symbols are `$Name$`, not `%Name%`.

#### aux_roots.cpp (auxextract)

* A `*BF` enum redeclared in app/common breaks every file that includes aux_roots.h (EffectGeneratorUsageBF).

* auxextract misses defaults via a register (`or reg,-1`, byte stores, masks) or a stack temporary (1.0f
  arrays, Validity 0x100): check the bytes before each constructor call.
* Get_Value_String needs a `const T &` parameter in Value_To_String (by value: another register);
  Set_Value_String 0x75A140 passes the value itself to sscanf (a client bug, keep it).
  `DeltaTemplateClass<int>` needs aux_roots.h for `Type_Name(int *)` (else C2665 in kv_pair.h). Its 8
  inline-budget pairs flip with any change to duration.h / delta_template.h (see there).

#### avatar.cpp

* 0x781410 keeps BodyObj a raw pointer (a RefCountPtr retype flips its post-loop operand order).

#### button-name tables (ui_*.cpp)

* A file's table can start at the pooled "not used" string (0xB85C38) and be indexed by id; a wrong table
  start showed only as one load-order swap.

#### callback_manager.cpp / timer_manager.cpp

* VC6 guards the atexit destructor of a plain class static too, so a guarded initialiser points to a template
  static, not to a plain class one.

#### chat windows (cht01..cht05, ui_chat_panel.h)

* ClientChatRequestPacketClass: request 6 (join) has the text and "" in Unknown24 / Unknown34, 21 'none' in
  Unknown34 and the channel in Unknown24; 4 create, 7 leave. Every chat window cht05..cht01
  (0x5E3F00-0x5EAB3F) ends with its own adjustor thunk pair: each is its own object. `if (state != State)
  return;` gives the CLIENT_CHAT handlers' layout (0x5E8FF0).
* UIChatPanelWindowClass (+0xF8 / +0x104) is really ChatPanelUIBase.

#### client.cpp / ClientClass's constructor (0x726100)

* Adding a line to client.cpp loses nine assert-line functions. The constructor's three string arguments are implicit conversions
  ("Null Item", "", ""); it stays one inline-budget notch too rich at four sites (`_Eos` twice, `assign(ptr,n)`, the HashNodeClass
  constructor 0x729260, which that form loses in client.cpp). Lead: lower the constructor's budget before the Add statement through headers.

#### control_keys_manager.cpp (Load_Definitions 0x71B1F0)

* The out-of-line `hash_map()` 0x71DCE0 is a missing inline layer (struct row in §3.1.2): ContextMapStruct plus three D130 dead
  `_DefaultPSW.size()` reads, which act by total and callee, not position. Residue: First_Valid 0x71BCC0 out of line in the client at
  the section line, and the fourth KeyMappingStruct temporary's array constructor inline.

#### client_item_property_builder.cpp / builder.h

* builder.h's four inline FAIL_IFs fix the file's line count. builder.h and an item property's effect header
  include each other: an appfile draft hits the cycle. A visitor-slot class retype needs `replaces` rows in
  symbols_named.json. DeltaItemArrayClass::Get_Size (the client's FAIL_IF text) costs five pairs: 0x702240,
  0x7045F0, 0x758210, 0x6A4F40, 0x597080 (symbol-table sensitive); left out.

#### client_lego_impl.cpp

* Its canary function (the bone map constructor 0x72D5F0) flips with the file's contents: check it each
  verify.
* Add 0x72B7F0 and Get_Lego_Transform 0x72B000 are exact with D130 dead reads (marked). aux_roots' inline-budget pairs flip with
  lego_identifier.h and client_lego_impl.h too; 0x72D5F0 flips with the file's inline state. An inline `LegoSplitStruct::Get_Name()`
  accessor makes Get_Lego_Transform exact without dead code but loses 0x7045F0 and aux_roots' hold of 0x702240 (lego_identifier.h reaches it
  through co.h).

#### co.cpp

* The inline budget cuts the bigger callee first and same-size callees in source order, so earlier finds go
  out of line first. auxdata.h's find is the client's early-return form (rtti.h's test inverted: `!= global ->
  NULL`). The lever for its finds is `Unknown_Find_Aux((C *)NULL, this, "Name")`, which builds the NameClass
  before reading AuxData (one shared stack slot); used everywhere it loses functions, so the small finds keep
  their explicit NameClass. The find-if-set helper (bool copy 0x705450) is auxdata.h's find plus "NULL unless
  +0x70 is set": co.cpp's local `Unknown_Find_Valid`. An inline member defined before both reproduced the
  client's not-inlining of 0x6FEAC0 into 0x6FCB70. Other levers that matched: `if (!Is_X()) return x;`, the
  one-case switch, `Get_Float` / `Set_Floats` / `SphereClass::Init`.
* The aux-data Cast: refuted as the per-find spender: the accessors (mark / Value / AuxData / Client / Model /
  GameID), NameClass temporaries, G_ASSERT as an inline function, an empty ~PermString, Get_Lookup / Find
  wrappers, Is_Null / Type_Of in Find_Field or Cast, a checked-cast wrapper (moves 0x6FFC80 / 0x700400 /
  0x6F3FA0's Casts only), NameClass's ctor and LookupClass::Find made inline-visible, /Zi and /Z7. In 0x6FD310
  one static inline helper around the three finds sends all three Casts out with no dead code; two helpers
  (Spot, Line) with Blotch's find explicit gave the client's calls. A dead find at the top of 0x6F3FA0 sends
  Rotate_X out; as the last statement it gives the client's calls.
* Its constructor calls the aux-root factory 0x6F6000 (test links: §3.7); its loader reads basset.ini /
  cbasset.ini from the working directory (§3.7 clienttest's data files).

#### connect (Key_Exchange 0x829180)

* Byte readers keep a running offset (`offset += 4; offset += len`); `buf + len + 8` changes the arithmetic;
  the loader matched as `KeyPair.Load(file_auto_ptr(...))` (a named `file_auto_ptr` adds a 4-byte slot).

#### cordcon

* Header-field reads go through Cord_Get_Small / Cord_Get_Seq, out of line but inlined once declared before
  the caller: that gives the client's `mov eax, n` / `add edi, eax` and register choice (examinePacket
  0x8514D0). buildHeader, write and read still use the byte-wise Read_* / Write_* statics. `CordCon::read`
  with peek adds to `BytesRead_`. `CordCon::Is_Closed` is `cmp bl, 1` in Connect, Is_Connected and the
  destructor, a plain test in Send_Data and Receive. CordCon::write takes `(channel, len, data, priority)`.

#### cpacket.cpp

* Two exits testing the profile flag in bl are early returns; which Is_Valid_Float calls go out of line
  follows the inline budget (Orientation.W, Position.Z, Orientation.Z, ImpartedVelocity.Z in that order). A
  forward declaration on its own line shifted every later `__LINE__` (41 functions): use `class X *` inside
  the declaration. Calling SoundLibraryClass needs a clienttest stand-in (wwaudio's globals break the link).

#### debrsint.cpp / frameint.cpp (the main view's windows)

* POC: a settings.ini without Debris00 crashes in DebrisInterfaceClass (+0x58 indexes `Fields[Mode]`), as the client would. FastAllocator.h (wwlib)
  carries E&B getters in its EnB block: a change there needs a wwlib verify.

#### duration.h / delta_template.h

* A member added to the DeltaItemArrayClass template cannot be padded (no send pad restores sclient's addresses):
  put it on the base DeltaCompoundClass (`Get_Size()`, from the client's FAIL_IF texts).
* Any change, even an unused declaration, flips aux_roots' 8 inline-budget pairs and slot_data_render
  0x6A7C00: draft and full verify first.

#### EA data (.work/eadata)

* The Castanet manifest's 16-byte hash is MD5 with each 8-byte half reversed; key it on full paths (basenames
  lose rows); size equality misled for 8 files. RTPatch's path search patched `common\blood.ini` at the demo's
  `client\ini` copy. 7z's case-insensitive lookup collides MSS32.DLL and mss32.dll. The project's client.exe
  carries Net-7's URL edits in `.data` (P-EA1-2); `.work/eadata/ea2004/` has EA's own.

#### eff_robj.cpp / eff_seek.cpp (effects)

* cpacket.cpp's local `Unknown6EF4C0Class` stand-in: the real class loses 0x738970. 0x6EF4C0 is exact only with one D130 dead read in
  0x6EEEA0. Replacing a stand-in class in eff_o.cpp changes clienttest's trace stand-ins and expected.txt. `appfile.py match` outside
  app/client needs `-I auto -I app/client`. Curve members need the base-reference cast for virtual calls.

#### extrap.cpp (motion code)

* Rotations are `In_Place_Pre_Rotate_Z` (rows 0 and 1, +8 / +0x18; `Rotate_Z` leaves a field-offset residue);
  quaternion compares are quat.h's `Equal_Within_Epsilon`.

#### FontSys (font files)

* FontSys's own `Bits` (+0x2BC) shadows `RenderObjClass::Bits` (+0x18): the client's 0x6D60C0 reads the base
  one (the shadowing was a real POC bug); 0x6D60C0 may need a little budget spent between its inline `Play`
  copies.
* A generated stub for 0x6CE420 (vsnprintf, then Change_Text) leaves every formatted text in the POC empty
  (chat lines, inventory counts): poc_world.cpp's stand-in fills them.

#### gadget.h / ui_manu.cpp / ui_manu.h

* gadget.h's GadgetClass is 0x30 bytes; ui_manu.cpp's local one is 0x34 and pads UIManuCheckButtonClass from
  +0x34, so a copy over gadget.h puts Checked at +0x60, not +0x64: pad from 0x30. ui_manu.h's UIManuClass had
  a stray `#pragma pack(4)` (new size 0xBC8, 4 over).

#### gd_text_editor.cpp

* FAIL_IF(!Slider) on line 361 depends on the D25 blank lines at 350-357: any line added or removed before it
  needs a 1:1 adjustment.

#### intrface.h (InterfaceClass)

* The `__INTRFACE_MEMBERS` define goes on an existing header-comment line so no `__LINE__` moves.
* InterfaceClass::Unknown_54 is emitted with every ctor of a class that doesn't declare its overrides (cost
  0x515690 in 11 files): declare the client's overrides (+0x54 0x5439F0, +0x60 0x5443F0).

#### land.cpp and the land family (landdmap, Apply_Colors, Get_Triangles)

* The land companion (vtable 0xAE8974, displacement maps, Matrox-only) needs its own file: inside land.cpp the
  land's destructor inlines its delete and loses 0x46A480 / 0x4691A0. No `Get_Num_Polys`/`Get_Cost` override
  unless the client's class has one (land vtable 0xAE8994 hasn't). Program safety (.pld, registry): §3.7.
* Before tuning land lighting, diff the lightmap dumps texel by texel (`run_reference --dump`'s
  `tex_00900_*.bmp` against ours, read with the .pld's horizons): every shadowed texel lit in the original
  pointed at the sun input (0x5286A0 is the CRT's acos: SunAngle is the elevation), not at the land code.
* One texel index n with the pointers built from it gives the client's stepped pointers instead of i-indexed
  addressing (0x46D410); where the client's strength reduction keeps runtime base differences (`ebp - (ebp +
  2)`), the source had a separate pointer per channel (one pointer with constant offsets folds to
  displacements, 0x46D6B0); `extra` declared in its else branch made Apply_Colors exact; `float y` for v2.Y
  and v3.Y made Get_Triangles 0x465D30 exact.

#### mv3dview.cpp / mv3dsnd.cpp / meshsnd.cpp / mclient.cpp

* Four objects: mclient.obj 0x7EC3E0-0x7ED140 (the "task helper" 0x7ECC80 is inside it), meshsnd.obj
  0x7ED150-0x7EDCA0, mv3dsnd.obj 0x7EDCB0-0x7EEF50, mv3dview.obj 0x7EEF60-0x7F0F50. asserts.txt credits
  'movie3d.ini' to mv3dsnd.

#### nav_list_builder.cpp

* A POD in a vector whose `insert` helpers are "multiple" wants a user copy constructor (NavLinkStruct's explicit
  one gave 0x7B92A0 / 0x7B95E0 / 0x7B9630 / 0x7B9670).
* 0x7B7EF0 is exact only with the exact text of the headers it includes (client.h, sclient.h -> ui_mesh.h);
  any change swaps two stack slots, and its two vector destructors (0x7B8ED0, 0x7B8F10) then lose their caller
  among 174 identical copies. Under D203 that loss is accepted: name it in the commit and record it in
  attempts.csv; don't work around it.

#### PlayerList destructor (std::list, 0x886C20)

* A destructor that deletes the elements then empties the list: the client's inline erase with an out-of-line
  `it++` comes from `erase(begin(), end()); PlayerList.size();` or `++it` in the delete loop plus `clear()`;
  `clear()` alone inlines neither, `erase(F, L)` alone both.

#### postate.cpp

* One call fed by a duration local comes out as four tail-duplicated pushes; an inline helper whose inlined
  and out-of-line copies must both match took `if (a == b) return(true); return(false);`; `Get_Position()` is
  out of line, the client inlines `Get_Transform().Get_Translation()`.

#### px.cpp / px_wrapper.cpp

* Whether VC6 inlines Is_Valid / Fabs depends on the wrapping comparison: a static `Is_Close(const Vector3 &,
  const Vector3 &)` made 0x456900 exact. A one-case `switch (Type)` in an inline helper gives the client's
  adjusted `this`; `PxObject *o = ...; Vector3 pos = o->Pos;` against a `const Vector3 &` decides the client's
  `add eax,4` (0x452D50 / 0x452020). Guarded `<string>` statics bound app\shared objects too (0x451BD0,
  0x452750).

#### ref_ptr.h

* A `Create` returning `RefCountPtr<Derived>` wants a named local of that type (item property files).
* Including it needs g_assert.h first (G_ASSERT). A script that finds `G_ASSERT(0 != Referent)` by text also
  hits ref_ptr.h's header note: match the whole statement line.

#### ro_proxy.cpp (ProxyRObj)

* Float-returning forwarders get the client's `jmp [eax+N]` tail call only as a ternary, `return (p != NULL) ?
  p->F() : k;` (the if form gives `call`); a copy constructor can use the default base `RenderObjClass()`,
  members copied one by one; the client's Cast_* / Intersect_* call ProxyRObj's own slot, not the proxied
  object's (recursing when one is set): exact as written, kept.

#### sbase_info.cpp

* A `permstr` static appears in a file through avatar_desc.h (it reaches permstr.h).

#### sclient.cpp / sclient.h / client.h

* A class sclient.cpp newly sees whole (six audio classes; BridgeInterface's enums and uncalled members) can leave
  no pad window: BridgeInterface's first answer was a member guard in the header; the unguarded form worked once
  its deleted `#define` lines became blank lines (a removed line costs `__LINE__`). sendpad's "no value" line reads as if the pad moved; appfile is no predictor for sclient's pad.
* client.h's partial EffectObjectClass clashes with eff_o.h (C2011): include eff_o.h before client.h /
  sclient.h. Including auxdata.h in sclient.cpp breaks kv_pair.h's Type_Name overloads (C2665; blocks
  0x750B60). Headers sclient.cpp includes take ChatPanelUIBase from ui_chat_panel_base.h, not ui_chat_panel.h;
  a new call from sclient.cpp to a panel method needs the method declared where sclient.cpp sees the class.
* `Create_Peek(X *)` needs `RefCountPtr<X>` instantiated earlier in the file; `Send_Packet(&TempPacket(...))`
  is a temporary (a named local costs rows). Do_Frame's lost-connection dialog matches only as an if / else of
  two complete calls. `int id = list.front();` before `std::find` and a named local for a singleton gave the
  client's forms (0x74FA30 / 0x74FEC0, 0x751460). A quick exactness check: an appfile draft of sclient.cpp
  with `// @0xADDR` marker lines (~3 min).

#### settings_store.cpp / unknown_6af220.cpp

* Declarations added to tsk_game.h flip ui_radio_main 0x5E9410 in verify's path only (the BrowserType enum's position compensates). The five
  `map<string,string>` instances 0x6B15E0, 0x6B1A40, 0x6B1EC0, 0x6B1F40, 0x6B1FC0 belong to the window object 0x6AF220: exact settings_store code calls
  them out of line as the client does, so the window's file must hold them.

#### sgistl hash_map (attrib_dir.cpp, ini_manager.cpp)

* `insert_unique_noresize` compiles (sgistl_adapt.py), so `hash_map::insert` is usable; keep the existing
  explicit specialisations at the top of attrib_dir.cpp / ini_manager.cpp: removing them, or an explicit
  instantiation in their place, changes VC6's code for the table's other members (13 pairs).

#### ship customisation (ui_ship_cust.cpp, ui_shipcust.cpp)

* The client has both ui_ship_cust.cpp (0x6186A0-0x61E8xx, not in the tree) and ui_shipcust.cpp (from
  0x620E40): check the window with `appfile.py show` before placing anything in 0x618xxx-0x61Exxx.

#### shiputils.cpp

* `appfile.py match` refuses the file ("the object lacks void") unless `--allow-missing`: the parenthesised `static void (Unknown_80EAF0)(...)` definition.

#### stredit.h

* `#include <string>` must not go there: a forward declaration of `std::basic_string` with its three
  parameters works.

#### tsk_login.cpp / tsk_login.h

* Including a packet header mid-file turns a handler order-only: keep the one-line views with the real
  signatures (P-P9-3). Template bodies in a header tsk_login.h includes broke kerneltest.

#### ui_avatar_cust.cpp

* In drag functions `tm[2][3] = 0` is the client's (a missing 12th dword of the matrix copy). Forms that move
  spills / scheduling: box edges or float locals declared up front, a Vector2 set member by member, an inline
  Set(u, v), Matrix3D Set_X / Set_Y_Translation instead of `tm[i][3]` stores, a named local for a virtual's
  result. Adding 0x5513E0-0x551C80 made 0x550A10 inline a third Rotate_Y, and 0x551BF0's form flips with
  0x551520's body.

#### ui_avatar_dialog.cpp

* A `std::string` member initialiser is inlined only when the +0x1C member is read through `Peek_Mesh_Object()`; a
  direct field read calls it.

#### ui_class.cpp

* The float-field key event `ControlKeyComboStruct(0x1B, 0)` cuts 0x646360's zero count; GadgetSlider's Value
  is read through an inline float getter. Two Get_String calls in an if / else tail-merge at the call; dead
  `Descriptions.size()` reads move the inline budget (2-3 gave 0x576AC0's 17 inlined assigns).

#### ui_combobox.cpp / cameraeffects.cpp

* The client's two fmuls need the intermediate in a named local; a flag the client stores before an unrelated
  load needed `volatile` (0x57AC70 / 0x57AD50).

#### ui_dynamicwindow.cpp (the dynamic window family)

* In the family a factory's RefCountPtr result passed on wants a named local (bullet in §3.3.2).

#### ui_hot_keys.cpp

* Key-name lists passed to `Add_Keys` / `Remove_Keys` all as implicit `const char *` give the client's assign split.
  `(float)i64 / (float)i64` is `fidiv`; the client's two `fild qword` and `fdivp` are `(double)` casts. A toggle
  with flag-dependent arguments: if / else with constants per call (passing `!flag` gives `sete`).

#### ui_mfd.cpp

* Worth trying the "find out of line, Get_Value inline" codeless-layer form (target_reticle.cpp 0x7C5690) on
  0x5BDED0 / 0x5BE120.

#### ui_pterm.cpp / pt_data.h

* VC6 inlines a member defined later in the same .cpp: a tiny function the client calls out of line lives in another
  object (unknown_5de6e0.cpp). Read `Layer.Scene` directly (`Peek_Scene` is out of line); reset a shared pointer to
  NULL inside the `if (p)` block; a ternary computed before a `new` is a local; a switch with shared break / return
  compiles to a compare chain, a `return(true)` per case gives the jump table.

#### ui_registration.cpp (msxml, D222)

* `#import` the client's own type library, found by its GUIDs: a coclass CLSID is data, so pairs never see it;
  msxml3.dll's `DOMDocument` is `F6D90F11`, not the client's `2933BF90` (`.work/msxml/guids.py` searches
  client.exe for a GUID list). The full msxml3.tlh lost 0x5FA5F0; msxml.dll's whole header does not. The
  `_com_ptr_t` wrapper copies in unknown_7dfed0 / ui_registration are two COMDATs in two objects.
* RegistrationInterface's 0x5FB330 / 0x5FB540 / 0x5FB750 are members (0x5FA900 calls 0x5FB330 with ecx =
  this). The `$E` flag initialisers 0x5EED20-0x5EF070 are identical (`multiple`) or too short (`short`) in
  appfile and mostly not known function starts, so they don't count as pairs (file attribution of that range
  not verified).

#### ui_shard.cpp (character creation)

* A step hook that jumps to creation's galaxy select must create the narrator (0x67AB20 / 0x67C440) or
  ui_shard's constructor crashes.

#### ui_starbase.cpp

* Retyping UIStarbaseClass +0x190 breaks the destructor 0x633B50. Integer copies of a point go through a named
  TPoint3D (`Get_Point`); a vector member the client initialises as `Member(0)` is written that way.
* The file is padded with blank lines so FAIL_IF `__LINE__` values match the client: copy a draft whole or keep line counts, and replace a removed NOT EXACT line with a plain comment instead of deleting it.
* COLLISION_TYPE_0 is 2 and COLLISION_TYPE_1 is 4: check the client's push.

#### vector<string>::erase (0x4FC7C0)

* The client's call is always one inline layer below the call site (a dtor calls its own `Unknown_5C()`, which
  reaches the clear helper then `clear()`); fills use `push_back`, not `insert(end(), x)` (the client's last
  push_backs show as insert(it, 1, x) calls). Exact: 0x5E76B0.

#### wlib (libs/wwonline)

* Renegade's `Code\Tools\RenegadeGR\wtime.cpp` is the client's revision (no localtime_r wrapper, no "aa"
  format), not matchbot's: look for older per-file copies in RenegadeGR before changing matchbot's.
  wstring.cpp's and configfile.cpp's client revision is E&B's own, matchbot's with E&B's changes (its `//
  EnB:` block); matchbot, mangler and Renegade's Tools\wlib are one text (tell: no Joe Howes / MDC additions).
* wlib is built with /GX (`LIB_FLAGS['wlib']`): a by-value return of a class with a destructor leaves a
  construction flag (`push ecx; mov [esp+8],0`) even without an EH frame, a cheap sign of /GX.
  `tools/re/names.py` does not scan libs/wwonline (not re-verified).
* gaps.py pairs configfile wrongly (the client's object is 0x8B23C0-0x8B3E70: ctor, dtor, clear, readFile,
  enumerate, getString x2, getInt x4): confirm with `client.py callers`. A static helper whose last caller is
  one function gets inlined (Get_Date_From_Day into getTime once getYDay is gone).
* arraylist.h's getPointer sets the out pointer NULL on a miss: don't write call-site NULL forms. A 2-argument
  `Dictionary::remove(key, temp)` frees enough budget for remove itself to inline; wlib's 1-argument wrapper
  keeps it out of line. Dead getSize / getEntries spend nothing.

#### wnet (E&B's TCPCon, UDP)

* A reset connection is `CONNRESET` (-16): reads and writes end quietly on it and `isConnected` treats it as
  closed; `pumpReads` needs a dead `retval = -1;` (the client's stray `or edi, edi`). wnet's udp.h defines
  `close` / `read` / `write` as macros under `_WINDOWS`: app files that include it `#undef` them
  (app/wwudp/udp.h forwards to wnet's and does).

#### ww3d2

* A `SNAPSHOT_SAY` costs inline budget (`Apply_Default_State` 0x9270D0): try them first when inline calls
  switch at another point. meshmatdesc's 0x946EB0, 0x947490, 0x947310 differ in `UVSource` load order that
  moves with unrelated headers: noise (D18, D28). 0x4E9CB0 is E&B's `::operator new`, never inlined.
  dx8webbrowser: E&B's `BrowserEngine.dll` (D21).
* Before reverse engineering an asset-manager or render-state class, look for EA's published source:
  `C:\EnBDevelopment\EA\CnC_Renegade\Code\ww3d2\hueshift\enbassetmgr.cpp` is E&B's ENBAssetManager
  (Make_*_Unique, Recolor_*, Create_Render_Obj with HSV).
* SimpleSceneClass adds at the head and renders from the head: an object meant to draw after another is added
  before it. RenderObjClass's Set_Animation overloads are in reverse order: the combo one at slot 0x94, the
  no-argument one at 0xA0; 0x90 is Update_Sub_Object_Transforms.

#### wwshade

* Older than ZH in places: an error asking for a ZH WW3D2 feature isn't evidence (`CheckTranslucent`,
  `MultiListIterator::Last`, `get_deformed_vertices`). Registered names and class-id bounds show missing
  modules (no LegacyW3D).

#### wwudp (app/wwudp)

* E&B's UDP is 0x2C bytes (a second socket, a list of CONNRESET peers); udp.cpp includes <string> and app's
  permstr.h directly (interim). pk.h shadowing: see app/packets.

#### Zero Hour porting (every EA library)

* D9 places ZH edits. A one-sided trailing block or a callee-saved register shift is usually removed code:
  diff Renegade, Generals, ZH (`//DMS`, ZH tags, commented-out lines, `b_wire` "(gth) Renegade-specific hack",
  HY "Earth and beyond"), scan the client's strings (`tools/re/strscan.py`), diff ZH's file-header history
  topics. Enum values are evidence (gradient mode 5: `GRADIENT_MODULATE2X`).
