# Builders: startup set, rules, models, report

A *builder* is a subagent given one work item by a coordinator session. The user talks to the
coordinator, not to you: never stop to ask; write questions as proposals (with a recommendation)
in your report. Decisions are the user's (D-numbers in [progress.md](progress.md) §2).

## 1. Read first, by kind of work (sections only, never whole docs)

Everyone: `CLAUDE.md`; this file; `docs/workflow.md` §1-§2 (the fix loop) and §3.0 (traps for
everyone); the newest log entry of `docs/progress.md`; your item's own section in
`docs/milestones.md` or the brief the coordinator gave you. Then by kind:

| Kind | Also read | About |
|---|---|---|
| **App code** (E&B's own files under `app/`) | workflow.md §2.1 (adding an app file), §3.1 VC6 forms, §3.2 templates, §3.3 app-code forms, §3.4 verify and pairs, §3.6 RE tools, and your subsystem's bullets in §3.9; conventions.md §1 | ~43 KB |
| **Westwood library** (`ww3d2`, `wwlib`, ...) | workflow.md §3.1, §3.2 when templates are involved, §3.4, §3.5 build VMs, §3.6, and your library's bullets in §3.9; `docs/library-matching.md` §3.6.1 (your library's rows) | ~39 KB |
| **Harness, test or program** (`tests/`, `programs/`, `tools/harness/`) | workflow.md §3.5, §3.7 programs and unattended runs, §3.8 hygiene; in `tools/harness/README.md` the rows and quirks of the tools you use (its protocol notes only when you touch the wire format) | ~35 KB |

`grep -n '^#' docs/workflow.md` gives the section lines. Decisions cited by a section (D9, D20,
D23-D25, D54, D56, D62, D66, D69) are one table row each in progress.md §2; read those rows.

## 2. Rules

**Verification (CLAUDE.md, D66).** Library or app code changes are kept only if the set of
`(file, address)` pairs gains and loses nothing: save `docs/data/imports.json`, run
`importer.py verify --lib X [--pool K]`, compare with `tools/vc6/pairs_cmp.py`; then `report`,
`replay` (0 differences), the §3.6.1 row in `docs/library-matching.md`, commit. D66 names the
three kinds of non-loss (say which in the commit); any other loss waits for the user.

**Shared tree (D62).** Other builders may be working in the same tree. Their changes must not be
*compiled* into your verify: their untracked files aren't, unless `importer.py` lists them; check
`git diff` of shared files (importer.py, app headers, `stubs/app_stubs.cpp`, `docs/data/`). Commit
only your own hunks; keep the uncommitted window short (edit, verify, commit). Never wait for the
other's tree to be clean: that deadlocked two builders for an hour. `index.lock` on commit: wait
and retry. The importer lock serialises `verify`.

**Safety (D69).** Never a modal dialog: vidtest's `-nomsgbox` (implied by `-frames`/`-shot`), and
every new program needs the same. Full screen is allowed. Images only from the program's own back
buffer, never a screen grab. Never run a program with the game's install folder as working
directory or write into it; reading its files in place and copying them out (into build/) are fine (D143). No registry changes (the original E&BConfig runs only supervised, with a backup).
Running the tree's own programs unattended is allowed (D48).

**Names (D56, conventions.md §1).** Tag every name chosen without the client's own evidence
`// NAME n: evidence` at the end of its declaring line; run `python tools/re/names.py` and commit
`docs/data/names-review.md` with the change. Functional code written from pseudocode carries
`// NOT EXACT:` above the function (D64; names.py lists them in `docs/data/not-exact.md`).

**Attempts.** Read `python tools/re/attempts.py show ADDR` before retrying any function (rows and
stashed forms in `docs/data/attempts.csv`, `docs/attempts/<ADDR>/`; `attempts.py check FILES` before
committing a retry); record every near miss you abandon (`attempts.py record ADDR ... --status
partial|parked|blocked`, with the forms tried and `explain.py`'s class as `--blocker`). A function too short for the matcher
but byte-identical to the client (objdiff 0 rows) gets an exact row (D99). Before a register residue is marked
LIMIT REACHED, its second round includes a tryv2 sweep of loop forms in the inlined helpers it calls (D98).

**Build VMs (D30, D31).** Lease from the pool (`python tools/vc6/pool.py lease ...`, or
`importer.py verify --pool K`); release everything you leased before you finish. Never use the
base image. VMs are addressed by IP (clones share a computer name).

**Git.** Never `importer.py assemble`; never push; never `git reset --hard`, `git checkout -- .`
or `git clean` on anything you didn't create. `git add` only your files, by name (never `-A` or
`.`; `.claude/settings.json` stays out). **The index is shared with the other builders**: a bare
`git commit` takes whatever they have staged, and a path commit (`git commit -- <files>`) takes the
whole working-tree copy of a shared file, other builders' uncommitted hunks included. So: files
only you touch, `git add <files> && git commit -- <the same files>`; any commit that includes a
shared file (`tools/vc6/importer.py`, app headers, `stubs/*`, `docs/data/*`, `docs/library-matching.md`)
goes through `python tools/vc6/commit_mine.py MSGFILE SPEC.json` (your whole files, your imports.json entries (`--file`, headers rows
included) and your rows of attempts.csv / not-exact.md / names-review.md; it lands by compare-and-swap). Never `git reset`,
`commit --amend` or `commit-tree` to repair a sweep: report it.
Commit each finished, working piece right away: one imperative subject stating the evidence or
gain (see `git log`), counts before -> after; end the message with the `Co-Authored-By` line your
harness gives you (D71).

**Scripts and docs.** Python with backslashes goes through the Write tool, never a shell heredoc
(escapes get mangled). Keep each file's existing line endings: the Write tool and Git Bash
`sed -i` write LF into CRLF files; check with a byte read before committing. Docs stay UTF-8. Don't
edit `docs/progress.md` (the coordinator writes the log); in `docs/milestones.md` touch only your
own item's section. New traps go in your report, not in workflow.md. Where things go: `tests/`
for test programs, `programs/` for VIDTEST, `tools/harness/` for the Python side (D45, D67).

**Deadline.** Check it with `date`. When it passes, stop at the next clean point, revert
uncommitted unverified work you made, and report.

**Context.** Noisy output to `.work/<item>/*.log`; read only what you need. RE tool order:
`docs/data/*`, `tools/re/client.py dis|name|refs|callers`, `python tools/re/pseudocode.py --show
ADDR`, REA last (5 minutes of start-up).

## 3. Report (final message, under 40 lines)

1. Commits (hash, subject); counts before -> after where relevant.
2. What was built and how to run it.
3. What didn't work, so nobody repeats it: every NOT EXACT function with `explain.py`'s class, and the
   `attempts.csv` rows you wrote (`attempts.py record`), one line each.
4. Proposals needing the user (with a recommendation each).
5. New traps for workflow.md §3 (the coordinator files them).
6. What's left and the best next lead. Confirm the tree is clean of your work and VMs released.
7. Model and approximate tokens used (the coordinator logs them, §4).

## 4. Which model runs what (D72)

| Role | Model | Why |
|---|---|---|
| Coordinator | Opus | the user's choice (D96, after D93's evaluation) |
| Builders: verify loops, byte matching, RE, programs | **Opus** (default) | their tokens are mostly tool output; a stronger model doesn't cut the iteration count |
| A reasoning puzzle with small input (an inline-budget, frame or shape near-miss) **after two failed Opus attempts**, under D89's limits (never a register-only or order-only residue; one Fable round per function, 8 forms (every variant compile counts, scratch probes included: D141), 4 functions per session, 6 in the queue, one Fable builder at a time; `attempts.py show` says LIMIT REACHED); or a one-shot analysis whose wrong answer would cost a builder run | Fable | the only cases where the expected total is lower |
| Read-only audits, cross-referencing, doc moves, log digging, generator reruns: anything a tool checks afterwards (verify, names.py, a diff); Explore agents | Sonnet | cheap, and the check catches mistakes |
| Grep-level searches, log triage | Haiku | never edits tree code |

The plan log of each run (`.work/<run>/plan.md`) records model and tokens per builder, so this
table gets evidence. Rule of thumb for Fable: estimate Fable's cost for one attempt against Opus's
cost times the attempts you expect; choose Fable only when that is lower.
