---
name: coordinator-pacing
description: Pace an unattended coordinator run of parallel builders by API usage and host memory. Use at the start of a coordinator session, at every heartbeat, at every builder report, and when draining to a handoff; also when the user asks about usage, quota, memory or "how many builders".
---

# Coordinator pacing

The two real limits on parallel builders are the account's usage windows (a five-hour session window and a
weekly window, both with reset times) and the host's memory (each pool VM takes about 1.1 GB once running,
2 GB at startup). The user wants the quota spent, not left over at a reset, and wants a session to end cleanly,
with the handoff written, before a cap. This skill makes both measurable in one command and turns the numbers
into a launch allowance. Everything else about a run (briefs, disjoint files, shared records, the plan log) is in
`docs/builders.md` and `docs/workflow.md`.

The script: `.claude/skills/coordinator-pacing/scripts/pacing.py` (reads the local Claude Code login token
from `~/.claude/.credentials.json` for the usage figures; needs the pool, `tools/vc6/pool.py`, and
`tools/vc6/pairs_cmp.py`).

## At the start of a run

0. **The run branch and its draft PR** (D148, docs/github-plan.md 4.5.3): `python tools/run_branch.py start`
   makes `run/<date>` from origin/master (or brings an existing one up to it; an empty `Start run <date>` commit
   when it has none, `--co-author` for its trailer), pushes it, opens the run's PR as a draft at once (the claims'
   home) and writes the baseline below. Then `python tools/claims.py list`: assign builders only functions no live
   claim holds (contributors' claims included).
1. Record the run's baseline: `run_branch.py start` writes `.work/imports.<sha>.json` from origin/master (master is
   verified by construction). Every heartbeat compares HEAD against it, so a loss anywhere in the run shows up.
2. Open the plan (`.work/run-<date>/plan.md`) with a Log section; every heartbeat, launch, result and user
   instruction gets one dated line there. It is the handoff's source.
3. Create a heartbeat cron (40 minutes has worked) whose prompt tells you to run the heartbeat command, log any
   builder report not yet logged, and launch only within the allowance. Delete it when the run ends.
4. Ask the user (or read the plan) for the hard limits of the run: the time the handoff must be written, whether
   to spend the quota before a reset, and whether launches may run into the next window.

## At every heartbeat and every builder report

```
python .claude/skills/coordinator-pacing/scripts/pacing.py heartbeat --plan .work/run-<date>/plan.md \
    --builders <N running now> --baseline .work/imports.<sha>.json --handoff HH:MM \
    [--accept 0x47F300,0x7F0EC0]
```

`--accept` names addresses whose loss the user already accepted under D66 (the two land copies above, since
2026-10-27), so they do not block launches; any other loss sets the allowance to 0.

It prints and logs: both usage windows with reset times; host free memory, the pool's running and leased VMs;
whether HEAD's ledger (docs/data/imports) parses and which addresses are lost outright against the baseline (pairs that
reappear under another file are rename moves, D95(c), not losses); the burn rate observed since the previous
heartbeat line (per builder, per hour), or the defaults when there is none; the projection at the handoff time
with the builders running now; and the allowance: how many more builders may start.

The rules behind the allowance (defaults in the script, all overridable):

- **Memory:** one more builder per 2 GB free above a 3 GB floor. A stopped clone starts on lease and takes its
  startup memory, so count it before it exists.
- **Usage:** a builder costs about 0.5 % of the week per hour, the coordinator's own burn included (Opus; Sonnet
  about a fifth, Haiku a twentieth; measured below),
  and 4 % of the session window per hour. Launch only while the projection at the handoff stays under the cap
  (92 %). Never launch under 5 % of the week. Seven Opus builders burned about 1 % of the session window per
  minute at their heaviest; the observed rate in the log is the better guide once two heartbeats exist.
- **Health:** no launch while HEAD's ledger does not parse or an address is lost outright. Fix first
  (usually a builder's commit script: see workflow.md §3.8).
- **Harness check (D182):** at a heartbeat after any landed commit that rebuilds a file clienttest or udpreplay links
  (app code under app/, wlib / wnet, their stubs), run them at HEAD on a quiet copy: a worktree at HEAD (`python
  tools/vc6/wt.py new hc`, `build.py build --lib app --allow-dirty` there, copy build/art/MSS32.DLL) or the main tree when no
  builder has uncommitted app edits; then `clienttest.py build` + `run` (103 checks, exit 0, trace equal to expected.txt) and
  `udpreplay.py build`, `run alex3`, `handshake` (54/54). Log the result; a failure is a health item: give it to one bounded
  builder (Sonnet: tests/ stand-ins only, never app code) before the next launches. Always once more before the handoff.

Observed on 2026-10-27: seven builders at a time on a seven-VM pool left 3 to 4 GB free on the host.
Usage, measured 2026-11-08 over every Claude-builder run's heartbeat lines 2026-10-30..11-08b (`scripts/burnrate.py`,
agy / API runs left out): 106 h, 223 builder-hours, 104 % of the week, so **0.47 % per builder-hour with the
coordinator included** (runs 0.34-0.63 %). A fit splitting the two gives the coordinator ~0.6 %/h and each builder
~0.1 %/h, loosely (whole-percent readings, coordinator and builders always overlap): extra builders cost less than
the all-in rate. The single 2026-10-27 run's 1.3-2.5 % was the old default and overstated it about fourfold.
Rerun `python .claude/skills/coordinator-pacing/scripts/burnrate.py` from the repo root to refresh the figure.

## Launching

- **Claim the builder's functions first:** `python tools/run_branch.py claim UNIT --builder <id> --model <m>`
  (`--split` for a file over 30 not-yet-exact functions: one claim per 30). It adds a line `claim: UNIT (ID, MODEL)`
  to the run PR's claims block, granted at once: no issue, no email. A claim that is not granted means someone else
  (a contributor's issue, another run, this run's other builder) holds part of it: give the builder another item.
  Put the granted ranges in the brief and tell the builder they are claimed (it checks only outside them); a
  builder that finds its work leading into a claimed range stops and reports.
- **The GitHub API budget** (the maintainer's 5,000 calls an hour; a 19-builder run hit the limit on 2026-10-31):
  `claims.py`/`run_branch.py` stop below 500 remaining calls and print the reset time. If they do, launch nothing
  that needs a claim until then, and do not retry in a loop. `claims.py list`/`check` answer from a 5-minute cache.
- Give every brief a hard stop time and a report time, both earlier than the handoff by the time you need to
  log the result and file its traps (about 15 minutes per report).
- Model by item (D72, until the D93 evaluation revises it): Opus for byte-exact rebuilding, Sonnet for bounded
  tools fixes and audits, Haiku for searches. A short, bounded item (one trial, one batch, three functions) is
  worth 30 to 60 minutes; a file is worth 2 to 2.5 hours.
- Log the launch: time, builder id, item, stop time, how many builders are now running.
- Push only at a heartbeat that found builders' work brought into the run branch since the last push: `python
  tools/run_branch.py push` (the pre-push check takes about 2.5 minutes). Contributors see the work before master
  does; the run's claims live while its PR is open, pushed or not, and the hosted checks skip the draft.
- When a report arrives: log the result line (pairs before and after, what is parked, proposals), file the
  levers and traps in workflow.md §3, open proposals for the user in progress.md §3, then run the heartbeat
  command again (the report changes the builder count and the memory).
- In the same pass, prune progress.md §4 (Next steps): delete each item the report completes, and cut a part-done
  item down to what is left (its remaining addresses / functions). Never annotate an item as done ("made exact
  tonight", "done in run X"): what landed belongs in the plan log and the handoff's log entry, not in §4. Add the
  report's new leads to §4 only as open work. No builder edits progress.md during a run, so this is safe at any time.

## Draining to a handoff

- Decide the last launch from the projection: once the budget to the handoff is spoken for, say in the log
  that the window is closed, and keep it closed unless the user reopens it.
- While the last builders run, draft the progress.md entry and the §4 refresh as scripts in `.work/coord/`
  (the Write tool, never a heredoc) with slots for the last results; run them after the final report.
- Then: `python tools/vc6/pool.py stop-idle --minutes 0`, `python tools/harness/clientpoc.py clean --all` (POC
  instances; a running client's is kept), `git status` clean except `.claude/settings.json`,
  HEAD's ledger parses, `python tools/vc6/wt.py list --unlanded` is empty, or each one is named in the log with
  why (the heartbeat's `unlanded` line counts them; `wt.py remove` refuses such a branch without `--force`), delete
  the heartbeat cron, log the end with the final usage figures.
- Claims: `python tools/run_branch.py release <builder id>` (or `release UNIT`) for each claim whose builder
  brought nothing in; the others end when the run's PR merges. An old issue claim: `release N` (it drops the
  `Claims #N` line too). At handoff `run_branch.py ready --summary FILE
  --co-author 'NAME <EMAIL>'` (the handoff skill; first `python tools/re/names.py` and `tools/re/notexact.py`, and commit
  what they change: the PR check 'Generated documents are up to date' fails on any line shift a builder left unregenerated) regenerates and commits the README's progress card
  (tools/progress.py), takes the PR out of draft; the verify service verifies it and the maintainer merges it.
- Cleanup, once no builder is running (at handoff, and again at the next run's start, after the previous PR merged):
  `python tools/run_branch.py cleanup` deletes the local branches and worktrees whose every commit is already in
  origin/master (merged, or landed by wt.py's test: same author time, author and subject, or the same patch), keeps and lists the ones with commits of their own (say in the log
  which, and why they are kept), fast-forwards local master and lists remote branches other than master and the
  current run's (GitHub deletes a merged PR's branch; a leftover one is deleted with the user's say-so). Never
  while a builder works: a fresh builder worktree with no commits yet looks merged.
- If the user says to spend the quota before a reset, keep launching bounded items up to the allowance and
  let them run into the next window; the handoff then gets an addendum after the last report. If the user says
  nothing new starts, close the window at once and log it.

## agy builders (D187)

Gemini 3.8 Flash (high) through the Antigravity CLI runs on the user's Google subscription, outside the Claude windows;
`tools/agy/README.md` has the steps. `pacing.py` cannot see agy's quota (no CLI or API figure), so:

- **Ask the user for the agy window's % used** at the start and once mid-run (agy's own display); calibrate with the
  streams: about 1 % of the 5-hour window per 100k `result.usage.total_tokens` (2026-11-02); a Flash item costs 0.6-1.6M.
  Plan the launches so the window is spent by its reset, the last ones ending before it.
- **A 429** (`RESOURCE_EXHAUSTED`, "Individual quota reached", with a reset time) in a stream stops agy launches until the
  reset. Quotas are per model: Flash only (Opus through agy ran out in one short item).
- **Memory and VMs** count as for any builder (3-4 at a time on this host); each works in its own `wt.py` worktree.
- **Each exit:** log `result.usage` (never the builder's own token figure), land with `wt.py bring-over ID --target
  run/<date>` (several finished at once: `wt.py bring-over A B C --target run/<date>`, one verify;
  both resolve and regenerate generated-file conflicts; another file's conflict: `wt.py rebase ID`, which stops on it
  for the builder), run the heartbeat; after POC code lands, the POC
  check is yours (`tools/agy/poc_check.sh`): builders' own POC runs crash at startup and prove nothing.
- **Briefs** name every WinMain stand-in that opens a dialog (0x847D20, 0x846C90) and say it stays (D69).

## Session caps

If the session window caps mid-run, the builders stop where they are with their leases intact; nothing is lost.
At the reset, check HEAD's health, then resume each one from its transcript (SendMessage) with a new stop time.
Plan the run so this does not happen: the projection above exists for that.
