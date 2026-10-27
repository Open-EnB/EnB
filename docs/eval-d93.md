# D93: the model evaluation (2026-10-27)

The user's decision (D93, progress.md §2): run the same reconstruction brief with Opus 5.5, Sonnet 5.5 and
Haiku 5.5 on two fixed tasks, score by re-verification, and revise D72 (which model runs what) from the
scores. **The next coordinator session runs this first**, after the weekly reset of 2026-10-27 12:59, on a
quiet tree (no other builder until the candidates are launched and the base commit is fixed).

## 1. Tasks

| Task | What | Why |
|---|---|---|
| A, pattern | three packet classes, one file each, chosen at launch from P5's "unstarted, similar size" list (its report of 2026-10-27 10:45 in `.work/run-2026-10-26/plan.md`); fall back to `python tools/re/pkdecode.py` for opcodes with 2-4 functions and no new base class | most remaining app work looks like this; udpreplay is a functional check |
| B, puzzle | `option.cpp` (0x7FD960-0x7FE0E0, six SGI hash_map instances over the string hash 0x42DB00); fallback `co_player_relationship.cpp` (0x70D280 / 0x70D860, needs ClientObjectClass +0x12C/+0x130 in client.h) | §3.1's levers and the two-attempt rule decide the result; both files are reserved for this |

## 2. Candidates and runs

| Run | Model | Task | Worktree / branch |
|---|---|---|---|
| A-opus | Opus 5.5 | A | `.work/eval/A-opus`, branch `eval/A-opus` |
| A-sonnet-1, A-sonnet-2 | Sonnet 5.5 | A | likewise |
| A-haiku | Haiku 5.5 | A | likewise |
| B-opus, B-sonnet-1, B-sonnet-2, B-haiku | as above | B | likewise |

Fable is not a candidate builder (D72/D89 already price it). The coordinator stays on Fable for the eval
session; the Opus-coordinator trial is a later session (user, D93).

## 3. Protocol

1. **Base commit:** HEAD of a quiet tree, recorded in the eval log `.work/eval/log.md` with
   `git show HEAD:docs/data/imports.json > .work/eval/base.json`.
2. **Worktrees:** `git worktree add .work/eval/<run> -b eval/<run> <base>`; in each, junction `.work/locks`
   and `.work/pool` to the main tree's (workflow.md §3.4/§3.5: `cmd /c mklink /J`), copy `.work/vc6_hosts.json`.
3. **Pool:** eight runs at once need `pool.py create` up to `--max 8` (2 GB startup clones; check host free
   memory first: one more VM per 2 GB above a 3 GB floor), or two waves of four (A then B). Each candidate
   leases its own VM (`--agent eval-<run>`).
4. **Launch:** all runs of a wave in one response, same brief (§4), same base, same deadline (2.5 h), no
   messages between them, no coordinator help beyond the brief. Model per the table (the Agent tool's `model`).
5. **Scoring (coordinator, not the reports):** for each branch, in its worktree:
   `python tools/vc6/importer.py verify --lib app --pool 1`, `python tools/vc6/pairs_cmp.py .work/eval/base.json
   docs/data/imports.json`, `replay`; task A also `udpreplay.py build`, `conn alex3`, `conn alex7`.
   Score table columns: exact functions / functions in the file(s); pairs gained; pairs lost (any loss not
   named under D66 is a fail); tokens (from the task notification); dollars (Opus 1.0, Sonnet 0.2, Haiku 0.05
   per token, relative); wall-clock; process (commit_mine used, gains-only commits, attempts.csv rows with
   explain.py classes, two-attempt stop, parked/LIMIT functions untouched, tree clean, VM released: one point
   each); honesty (claims in the report that the re-verify contradicts: list them); levers and traps (quality).
6. **Merge:** the best branch per task (or the union of exact functions, if two branches have disjoint
   exact sets) is rebased onto master and committed with the normal gains-only check; the others are kept as
   branches for a week, then deleted.
7. **Outcome:** a progress.md log entry with the score table, and a D72 revision proposed row by row
   (verify-loop builders, audits, Fable's puzzle role), for the user.

## 4. The brief (identical for every run; replace <run>, <task>, <model>)

> You are builder <run> on the Earth & Beyond client reconstruction at C:\EnBDevelopment\EnB\.work\eval\<run>
> (a VC6 byte-exact rebuild of client.exe; CLAUDE.md is the project's rules; this is your own worktree on
> branch eval/<run>: commit there, never in the main tree). The coordinator session is your only contact with
> the user: never stop to ask; write questions as proposals with a recommendation in your final report.
>
> Read first, sections only: CLAUDE.md; docs/builders.md (all of it); the "App code" row's sections of
> docs/workflow.md (§1-§2, §2.1 adding an app file, §3.0, §3.1 in full, §3.2, §3.3, §3.4, §3.6, §3.8, and the
> packets bullets in §3.9); the newest entry of docs/progress.md; progress.md §2 rows D10, D20, D56, D64, D66,
> D87, D89; tools/re/README.md's rows for explain.py, attempts.py, client.py, pseudocode.py, appfile.py,
> vtables.py [task A: pkdecode.py, pkemit.py, pkclasses.py and tools/harness/README.md's udpreplay rows; task B:
> `git log --format='%h %s' -12 -- app/shared/lookup_table.cpp app/shared/boolpars.cpp app/client/buff_database.cpp`
> and those files, the SGI table's earlier users].
>
> Your item: [task A: rebuild the packet classes for opcodes <n1>, <n2>, <n3>, each in its own file per pkemit's
> conventions, byte-exact, with udpreplay's alex3 and alex7 round trips PASS and their counts in each commit
> subject. Task B: rebuild client\option.cpp (0x7FD960-0x7FE0E0, six SGI hash_map instances over the string
> hash 0x42DB00) as app/client/option.cpp per workflow.md §2.1, its table on libs/sgistl as the earlier files
> do, byte-exact where the client has the functions and `// NOT EXACT:` (one line) where only pseudocode gives
> them (D64).]
>
> Rules: every commit gains and loses nothing (before-copy from `git show HEAD:docs/data/imports.json`,
> `python tools/vc6/importer.py verify --lib app --pool 1` with `--agent eval-<run>` semantics of the pool
> (lease, release what you lease, retry a lease that fails with "Unable to allocate", never `--max-age`),
> `python tools/vc6/pairs_cmp.py` over the whole app, `replay` with 0 differences); commit each verified gain
> right away, by path, never a bare `git commit`, never `git add -A`, imperative subject stating the evidence,
> ending with the Co-Authored-By line your harness gives you; never push, never assemble. Shared records
> (docs/data/imports.json, importer.py, stubs/, docs/data/attempts.csv, names-review.md, not-exact.md) through
> tools/vc6/commit_mine.py keyed by your addresses and file names. Classify every near miss with
> `python tools/re/explain.py OBJ SYMBOL ADDR`, follow §3.1's triage order and the two-attempt stop rule, read
> `attempts.py show ADDR` before any retry, never touch a LIMIT REACHED function, record every near miss you
> abandon with `attempts.py record`. Names: `// NAME n: evidence` without client evidence. No `sed -i` (CRLF);
> scripts through the Write tool into .work/eval/<run>/.work/ (a heredoc mangles backslashes). Set
> PYTHONIOENCODING=utf-8 for client.py; `appfile.py match` needs `-I` for app/common, app/packets, app/shared,
> app/client/tasks, app/kernel, radtools' include_enb and the directx8 SDK. Safety (D69): no modal dialogs, no
> registry changes, never the game's install folder as working directory. Keep context small (`| tail -30`).
>
> Time limit: 2.5 hours from now. At the limit (or when done), stop with the worktree clean of uncommitted
> work and every VM released, then report in builders.md §3's format (under 40 lines): commits with counts
> before -> after, what was built, what didn't work (NOT EXACT functions with explain.py's class, attempts.csv
> rows written), proposals with recommendations, new traps, what's left, model (<model>) and approximate tokens.
