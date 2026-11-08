# Integration pipeline and the EA freeze (plan, 2026-11-07)

This plan was agreed with the user on 2026-11-07, after the day run (PR #173). It records what failed when builders' work was
brought onto the run branch, the tooling that fixes it, and the end of importing from EA. It has two parts:

* **Next session (§3, §4).** Freeze the EA libraries as plain source. Split `importer.py` and shard `imports.json`. Build tooling
  items 1-6. The session runs on a quiet tree, in serial: the coordinator works alone or with one builder at a time, and no
  run's builders are working.
* **Recorded for later, not implemented next session (§5).** Builders integrate their own work (the hybrid form). Builders are
  spread across the app as a soft launch rule. The ruah assessment.

## 1. What failed on 2026-11-07

Builders' worktrees kept their work apart without trouble. Every failure came when work was brought onto the run branch:

| Failure | Cause | Fixed by |
|---|---|---|
| `imports.json` conflicted on almost every landing | It is one 1 MB generated file that every verify rewrites (11 libraries, ~775 file blocks, plus count lines) | §3.3 sharding; item 2 |
| A landing was refused: "the commits record other pairs" (WE8) | A hand rebase took the run's side of `imports.json` and did not regenerate it | item 2 |
| WE9's five `attempts.csv` rows were dropped | The coordinator's `--ours` on a hand rebase | item 1 |
| A landing aborted with "run moved while landing" | The coordinator committed progress.md while `rebase_gen.sh` was landing WE4 | item 3 |
| A branch built on another builder's unlanded branch carried those commits again, under new hashes | Stacked branches have no record of their base | item 4 |
| A1's worktree was removed before it landed; A7 was nearly left unlanded | Nothing lists finished but unlanded branches, or protects them | item 5 |
| The POC link broke twice (HF1, HF2), and clienttest once with LNK2005 (shiputils / ui_talkt), on commits whose verify was clean | A landed app definition collided with a stand-in in `programs/clientpoc/poc_standins.cpp` / `poc_world*.cpp`, `stubs/app_stubs.cpp` or `tests/clienttest/client_stubs.cpp` | item 6 |
| `importer.py` was a shared file that most commits touched (~570 commits since 2026-10-25) | Every new app file adds a `RECONSTRUCTED` entry | §3.2 |
| Each verify queued 5-15 minutes for the importer lock with 5-7 builders | One lock, one verify per landing, plus builders' own verifies | none of these items; it limits §5.1 |

## 2. Why freeze the EA libraries

`importer.py` does four jobs:

1. **Import from EA** (`plan`, `assemble`, `refine`, `add`). It chose one copy per library (Renegade R or Zero Hour Z) and one
   copy per file, and spliced in functions from the other copies where byte matching showed a gain. This is finished, apart from
   the final refine pass (§3.1). Future work is reconstruction from the client.
2. **Keep the EA libraries derivable from EA's copies** (`PATCHES`, `APPENDS`, `NEW_FILES`, `EXTRA_FILES`, `replay`, and
   `annotations.py` re-applied on every import). Today there are 919 patch edits in 148 files:

   | Library | Patch edits |
   |---|---|
   | ww3d2 | 649 |
   | wwshade | 83 |
   | wwaudio | 57 |
   | wlib | 56 |
   | wwlib | 30 |
   | wnet | 30 |
   | wwmath | 8 |
   | wwdebug | 4 |
   | binkmovie | 2 |

   There are also 26 non-app `RECONSTRUCTED` files, 1 append, 1 new file and 13 extra files. The patches are now client
   reconstructions, such as `WWLIB_TINY_PATCHES`' RegistryClass code. Writing them as Python strings inside a 5,949-line script,
   applied through a `.work/` script (a CLAUDE.md hard rule), makes them slow to write, hard to review, and a shared file that
   builders conflict on.
3. **Measure** (`verify`). It compiles a library on the pool and byte-matches it against client.exe, which gives the
   (file, address) pairs.
4. **Keep the ledger** (`imports.json`, `report`). `pairs_cmp.py`, the D66 / D202 loss rule, landings, the heartbeat, the hosted
   verify service, not-exact.md, names-review.md and coverage all read it.

The freeze retires jobs 1 and 2. Jobs 3 and 4 become their own tools, with a sharded ledger.

The pairs at stake at HEAD (1c60b231):

| Library | Pairs in tree |
|---|---|
| app | 12,549 |
| ww3d2 | 3,746 |
| wwaudio | 780 |
| wwlib | 780 |
| wwmath | 540 |
| wwshade | 487 |
| wwsaveload | 170 |
| wlib | 145 |
| wwdebug | 114 |
| wnet | 66 |
| binkmovie | 41 |

The freeze must lose none of them.

## 3. The freeze and the sharded ledger (next session)

### 3.0 Preconditions

* PR #173 has merged, and `run_branch.py cleanup` has run. The work goes on a new run branch from origin/master, with a baseline
  `.work/imports.<sha>.json` from `run_branch.py start`.
* No builder is running. Nothing else lands until §3.2 and §3.3 have landed: they rewrite files every builder touches.
* `importer.py replay` exits 0 at the base, which proves the tree equals EA's copies plus the recorded edits. If it does not,
  stop and find out why before freezing: the freeze takes the tree as it stands.

### 3.1 Final refine pass, one library at a time

`refine` has no `--dry` flag; it rewrites the tree and `imports.json`. Either add one (preferred and small: report each file swap
and splice, with its gained and lost pairs, and write nothing), or run refine in a throwaway worktree (`wt.py new FR`) and treat
its diff as the dry result.

1. Refresh the evidence first: `tools/vc6/bytematch_all.sh .work/bm2` (~4 min). Stale evidence already misled refine once
   (tools/vc6/README.md: after the wwshade flag change it still saw 46, not 61).
2. For each EA library (binkmovie, wlib, wnet, ww3d2, wwaudio, wwdebug, wwlib, wwmath, wwsaveload, wwshade):
   `importer.py refine --evidence .work/bm2 --lib L` (dry). Then, for anything it proposes: verify, `pairs_cmp.py`, and keep it
   only if it gains and loses nothing. Each kept library is an ordinary commit, made before the freeze.
3. **Report to the user, then stop.** Before any of §3.2 starts, report each library's result: what refine proposed, what was
   kept, and the pairs gained and lost. Wait for the user's go-ahead before §3.2. Any loss waits for the user, as always. The same
   figures go into the session's progress.md entry.

### 3.2 Freeze: the patches become source with reconstruction comments

The tree already holds the patched text, since it is replay's output. The freeze therefore changes no code. It annotates the code,
then deletes the machinery that produced it.

1. **One block at the end of each file.** Every EA-derived file that E&B changed gets one comment block, appended after its last
   line: after the include guard's `#endif` in a header. Nothing is inserted above existing code, so no line moves, and
   `__LINE__` in WWASSERT / FAIL_IF / WWDEBUG_SAY keeps its value. A comment inserted above an assert would move it, and the
   pair would be lost; WE6 lost 0x8444C0 that way on 2026-11-07. The block's form:

   ```
   // Earth & Beyond client (reconstruction): changes to <copy>'s <file> (frozen <date> from importer.py)
   // EnB: <Class::Function> (line N at the freeze): what changed and why; evidence (e.g. client 0x9D1600)
   // EnB: <Class::Function>: from <other copy>'s <file> (byte match)
   ```

   The block's first line is conventions.md's header line for our own code (§4, file headers), so the block reads as ours
   beneath EA's file.

   * Each entry names the function or declaration it describes. Line numbers are given as at the freeze, since later edits will
     move them; the name is what stays valid.
   * Every entry line starts with `// EnB:`, so `grep -rn '// EnB:'` lists every E&B change. That replaces imports.md's
     per-patch list.
   * Record the rule in conventions.md (§ Statements and comments), next to the "reconstructed function" rule: E&B changes to an
     EA-derived file are described in its end-of-file EnB block, never inline. Later changes add their entry to the block.
2. **The block script** (`.work/freeze/annotate.py`, saved, not a heredoc). It builds one block per file from:
   * each `PATCHES` edit `(old, new, note)`. The script finds `new` in the tree file exactly once (or stops and names the edit),
     takes the enclosing function's name and the line, and writes the note as the entry;
   * splices from other copies (imports.json's `merged`): a "from <copy>'s <file>" entry;
   * `APPENDS` / `NEW_FILES` / `EXTRA_FILES`: an entry saying what was appended or added and why;
   * the 26 non-app `RECONSTRUCTED` files, which are ours whole: their description becomes the block's opening entry, in place of
     a header comment at the top, which would move every line.
   * **The banner note.** In every file that gets a block, the line `MODIFIED_NOTE` writes under EA's copyright is reworded to
     `**	Modified for the Earth & Beyond client reconstruction: each change is recorded at the end of this file.` A file that
     differs from its EA copy but lacks the note gets the note: it replaces the banner's bare `**` line, as `mark_modified`
     does today, so no line moves. The note is the only edit above the block, and it is one line for one.
3. **The script's checks.**
   * For every file, the old text is an exact prefix of the new text, apart from the banner note's one line. The block and that
     line are the only changes, and the line count above the block is unchanged.
   * The file ended with a newline before the block is added.
   * It lists the files given a block and the number of entries in each, and every patch it could not place.
   * Any of these failing stops the freeze.
4. **Verify everything:** every library, app included, because EA headers are included by app code. Run `pairs_cmp.py` against
   the baseline; it must show 0 lost. End-of-file comments move no line and change no bytes, so any loss means a bug in the
   script.
5. **Delete the machinery:**
   * `PATCHES`, `APPENDS`, `NEW_FILES` and `EXTRA_FILES`.
   * The `plan`, `assemble`, `refine`, `add` and `replay` commands.
   * `addpatch.py`.
   * `annotations.py`'s re-application. Its comments are already in the tree and stay as ordinary text; check its other users
     first.
   * The bytematch evidence's role as an input; `.work/bm2` stays as reference.
   * `wt.py bring-over`'s and `verify_pr.py`'s `replay` steps. `verify_pr.py` picks libraries from "importer.py lines naming a
     library"; it should pick them from the changed paths instead.
6. **Docs, in the same commit series:**
   * CLAUDE.md: drop the `assemble` hard rule, and replace "Patches go in importer.py ..." with "edit the source; describe E&B
     changes to an EA-derived file in its end-of-file `// EnB:` block".
   * conventions.md:
     * Replace "in imported EA files they [comments] are added through `tools/vc6/annotations.py`".
     * In §4's "Files imported from EA's C&C releases", the example line becomes the new banner note.
     * "importer.py writes that line itself (`MODIFIED_NOTE`) on every assemble and replay ... don't edit imported files by
       hand" becomes: edit the file, keep the note, add the change's entry to the end-of-file block.
   * workflow.md: update the fix loop (§2, step 6 "Then refine") and the traps that name replay or patches.
   * library-matching.md: a closing note on the freeze.
   * tools/vc6/README.md: the importer section.
   * A decision entry for the freeze (the next D number) in progress.md.
7. **Keep EA's copies.** `C:\EnBDevelopment\EA` stays as reference and provenance: who wrote what. imports.json's `copy`, `best`
   and `merged` fields stay as frozen provenance in the ledger.

### 3.3 Split importer.py; shard imports.json

After §3.2, what remains of `importer.py` is:

* the library definitions (paths, `dest()`, compile flags, `tree_includes`, which build.py uses);
* `verify` and `report`;
* the agent and lock handling;
* `RECONSTRUCTED`'s ~950 app file descriptions.

Thirteen tools import the module: build.py, trial.py, trial_all.py, tryv2.py, tryvariants.py, trydiff.py, flagsweep.py, auxpad.py,
zhall.py, refresh_expected.py, appfile.py, dated.py and run_reference.py. Split it into:

| Module | Holds |
|---|---|
| `tools/vc6/libdefs.py` | The libraries, flags, include paths and `dest()` |
| `tools/vc6/ledger.py` | Load and save the sharded ledger (below), `export` to a single-file snapshot, and the per-library totals, computed rather than stored |
| `tools/vc6/verify.py` | Today's `importer.py verify`, same options (`--lib`, `--pool`, `--lock-wait`, `--agent`, `--holder-pid`) |
| `tools/vc6/report.py` | Today's `importer.py report` (imports.md) |
| `tools/vc6/importer.py` | For one transition period, a thin shim that re-exports and forwards `verify` / `report` with a deprecation line, so the 13 importers and every brief and doc keep working while they move over |

**The sharded ledger:** `docs/data/imports/<lib>/<file>.json`, one shard per source file, and `.../<lib>/headers/<header>.json`
for the `headers` rows.

* A shard holds that file's block as today: `in_tree`, `tiebreak`, `ambiguous`, `shared`, `expected`, `short_known`, and for EA
  files `copy` / `best` / `merged`. It also holds the file's description, which today is its `RECONSTRUCTED` entry. Adding an
  app file then no longer touches a shared Python file.
* The count lines (`in_tree_total`, `totals`, `ambiguous_total`) stop being stored. `ledger.py` computes them, and `report.py` /
  progress.py print them. Stored counts are a conflict on every landing even when the shards themselves do not conflict.
* JSON is written sorted, one array element per line, so git's text merge resolves additions on both sides of a shard and diffs
  stay readable.
* Result: two landings conflict in the ledger only when they touch the same source file. Claims already make that rare, and when
  it happens it is real. A move (D95(c)) or tie-break (D166) touches two shards, which is accurate.

**Consumers to move** (19 found by `grep -l imports.json` over tools/ and .claude/):

* **Tools:** run_branch.py, claims.py, progress.py, dated.py, triage.py, unref.py, variants.py, pins_run.py, test_verify_pr.py,
  commit_mine.py, commit_rows.py, pairs_cmp.py, trial.py, verify_pr.py, verify_watch.py, wt.py, rebase_gen.sh and pacing.py.
* **Readers:** read through `ledger.load(path_or_rev)`, which returns today's dict shape. Most moves are then one line.
* **pairs_cmp.py:** takes two ledgers, as directories, git revs or snapshot files.
* **Baselines:** `.work/imports.<sha>.json` stay single files, written by `ledger.py export --rev SHA`, so pacing.py's
  `--baseline` and old baselines keep working.
* **commit_mine.py / commit_rows.py:** their `imports.json` entry merging becomes whole-shard selection; most of it can go.
* **The hosted verify service** (verify_pr.py, verify_watch.py): change it in lockstep, deploy it before the first PR that has
  shards, and say so in that PR.

**Migration:**

1. A one-shot `ledger.py split docs/data/imports.json` creates the shards.
2. `ledger.py export` of the result must equal the old file, except for the stored counts, which are compared with the computed
   ones.
3. Run one full verify on the split tree. Its export must equal the pre-split verify's: 0 gained, 0 lost.
4. Delete `docs/data/imports.json` in the same commit.
5. Update the CI job "Generated documents are up to date" if it lists the file.

### 3.4 The order of work next session

The ledger change touches every tool, so nothing lands between §3.2 and the end of §3.3.

1. **Item 1** (tiny), committed first.
2. **§3.1** refine pass, then the report to the user. Wait for the go-ahead before step 3.
3. **§3.2** freeze.
4. **§3.3** split and shard.
5. **Item 2**, which is mostly done by §3.3.
6. **Item 5.**
7. **Item 3.**
8. **Item 4.**
9. **Item 6.**

If the session runs short, stop at a landed boundary: after the freeze, or after the shards. Items 3-6 can come in the session
after. At the end, run the D182 harness check (`CHECK_HARNESS=1 sh tools/agy/poc_check.sh`), then a PR.

## 4. Tooling items 1-6 (next session)

**1. `attempts.csv` merges by row union.** Add `docs/data/attempts.csv merge=union` to `.gitattributes`. This is safe today: the
file has 2,316 records, none with a multi-line field. Union keeps both sides' lines without conflict markers, so the same row
edited on both sides comes out twice. Add a check (in docs_check.py, or a test) that the file parses and has no duplicate
`(date, address)` rows, so a duplicate is caught and not silently kept. This covers every hand or builder rebase. `bring-over`'s
batch form already unions rows.

**2. Regenerate, don't merge, generated files.** After §3.3 the ledger rarely conflicts. The markdown files are always
regenerated:

* **The rule** for `docs/imports.md`, `coverage.md`, `app-inventory.md`, `docs/data/not-exact.md` and `names-review.md`: on
  conflict, take either side, then regenerate. A git merge driver would need per-clone git config and does not run on GitHub, so
  the rule lives in our tools instead.
* **Single-branch `wt.py bring-over`:** do what the batch form and `rebase_gen.sh` already do (generated files take the target's
  side, then verify, report, names.py and notexact.py) instead of refusing.
* **A new `wt.py rebase ID [--onto B]`** for a builder's own worktree: the same resolution and the same regeneration, ending in a
  commit of the generated files. This makes the WE8 failure, a hand rebase without regeneration, impossible by tool, and §5.1
  needs it anyway. `rebase_gen.sh` becomes a call to it.

**3. Landings tolerate unrelated commits on the target.** Today a landing refuses if the run branch moved at all ("moved while
landing"). It should instead re-check under the importer lock: if the new commits touch no path the landing touches and no
generated file, rebase the landing's result onto them and move by compare-and-swap. If they touch shared paths, refuse as now. A
coordinator's progress.md commit then never aborts a landing, without relying on discipline. If that proves awkward, the fallback
is `run_branch.py commit FILES -m MSG`, which commits under the importer lock and queues behind a landing.

**4. Stacked branches are recorded and landed in order.**

* `wt.py new CHILD --base wt/PARENT` records the parent and its tip in `.work/wt/CHILD.base`, as today, plus the parent's name.
* **When the parent lands:** `wt.py list` marks the child as "parent landed: rebase", and `wt.py rebase CHILD` runs `git rebase
  --onto run/<date> <recorded parent tip>`, the child's own commits only.
* **When the child is landed first:** `bring-over` refuses it unless the parent is in the same batch, placed before it.

This is the one idea worth taking from ruah (§5.3): merging in dependency order.

**5. Unlanded work is listed and protected.**

* `wt.py remove` refuses a worktree whose branch has commits not on the run branch or master (patch-equivalent ones count as
  landed), unless `--force`. That is the A1 case.
* `wt.py list --unlanded` prints finished but unlanded branches, and pacing.py's heartbeat prints the same line. That is the A7
  case.
* At the drain, the skill's checklist gains "`wt.py list --unlanded` is empty, or each one is named in the log with why".

**6. A pre-landing gate for stand-in collisions and links.** In `bring-over`, after the verify:

* **(a) Stand-in check, always, in seconds.** For every function a landing defines anew in app/ (new pairs, and definitions that
  are new in the diff), search `programs/clientpoc/poc_standins.cpp`, `programs/clientpoc/poc_world*.cpp`,
  `programs/clientpoc/poc_gadget_*.cpp`, `stubs/app_stubs.cpp` and `tests/clienttest/client_stubs.cpp` for a definition of the
  same symbol. Refuse with the list. The builder, or a bounded Sonnet fix, deletes the stand-in in the same landing.
* **(b) Link check, when the landing touches app/ or the stubs.** `build.py link --program clientpoc` and `clienttest.py build`
  on the landing's leased VM. Refuse on an unresolved symbol or LNK2005. In a batch landing, run it once per batch, so the cost
  is spread.
* `--no-link` skips (b), with a printed warning, for doc-only or tools-only landings.

Between them, (a) and (b) catch both HF1/HF2 breaks and the clienttest LNK2005 before landing. The D182 check stays, for runtime
regressions.

## 5. Recorded, not implemented next session

### 5.1 Builders integrate their own work (hybrid form of idea 1)

**The flow:**

1. The builder finishes its item and runs `wt.py rebase ID --onto run/<date>` (item 2).
2. In its worktree, the builder runs a full verify of the libraries it touched, `pairs_cmp.py` against the run's tip ledger,
   item 6's stand-in check and link check, and, for POC work, the POC run. agy builders cannot run the POC (D187); for them the
   coordinator runs `poc_check.sh`.
3. The builder reports "integrated at <sha> on run tip <sha>": its pairs against the tip (gained / lost / moves) and the link
   results.
4. The coordinator lands it:
   * If the tip has not moved, fast-forward it.
   * If the tip moved but only through other shards and paths, `bring-over` lands it with item 2's mechanical regeneration and
     item 3's tolerance.
   * A real conflict in a non-generated file, or any loss, goes back to the builder with its context still loaded (SendMessage),
     or to the user if the loss is not caused by the builder's work.

**The rules builders need:**

* A builder never accepts a loss under D66. It reports the loss: D66 calls are the user's.
* A loss the builder did not cause, such as another landing's neighbour effect (NM1's case), is reported as such, not "fixed".
* The brief's stop time leaves room for integration: about 20 minutes, more with the lock busy.

**Why not now:**

* Integration goes stale quickly. With 5-7 builders the tip moves several times while a builder integrates, and each retry is a
  full verify through one importer lock that already queues 5-15 minutes.
* Waiting costs Claude usage.
* It pays only once items 2-4 make regeneration mechanical and the ledger rarely conflicts; that is what makes retries cheap.

**Prerequisites:** items 2-6 landed. A measure of the lock's queue time per verify after sharding.

**Then:** trial it with 2-3 builders before making it the rule in builders.md.

### 5.2 Spread builders across the app (soft launch rule, idea 3)

**The rule,** added to the coordinator-pacing skill's launch section and to builders.md:

* With more than 2 builders running, at most 2 work on one critical path, for example a POC goal's chain.
* Those 2 are stacked on purpose (item 4) and land in order.
* The rest go to app areas far from the critical path and from each other: different address ranges, and no shared header that
  any of them will edit.

**Claims grow a header field.** `run_branch.py claim UNIT --headers co.h,money.h` names the headers a builder will edit. A second
claim that names the same header is warned about at launch, not refused. Shared headers caused the MoneyClass / co.h collisions
and aux_roots' SYMBOL_PAD shifts (workflow.md §3.4).

**It is soft because:**

* A goal-driven run is a chain by nature: backdrop -> camera -> orbitals -> ship -> lights on 2026-11-07. Spreading would slow
  the user's item.
* Spreading does not stop global effects: stand-ins (item 6 does) and aux_roots' padding.
* Unfamiliar files cost each new builder more startup reading.

The coordinator states in the plan log when it breaks the rule, and why.

### 5.3 ruah (github.com/ruah-dev): assessed, not adopted

**What it is:** ruah-orch and ruah-cli, a Node 18 CLI, MIT. On 2026-11-07, ruah-orch had 12 stars and no releases. It gives:

* a worktree per task;
* file-glob claims, with overlapping claims rejected at task creation;
* merges in dependency order (children before their parent), behind checks read from `.claude/governance.md`;
* executors for Claude Code, Codex, aider and others.

**Why not:**

* It solves isolation, which already works here (`wt.py`).
* Its README names nothing about generated files, shared build resources (our pool, the importer lock) or link-time effects, and
  disclaims semantic conflicts. Those are where we failed.
* Glob claims are coarser than our function-range claims.
* Windows support is not stated.
* It installs its own Claude Code skill and hook into `.claude/`, which collides with our tracked `settings.json` and our
  launchers (Agent, `tools/agy/launch.sh`).

**Borrowed:** merging in dependency order, as item 4.
