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
- **Usage:** the coordinator costs about 0.6 % of the week per hour by itself and each Opus builder about 0.1 %
  more (Sonnet about a fifth of that, Haiku a twentieth; the fit measured below; `--coordinator-rate`,
  `--weekly-rate`); the session window is taken as eight times those (`--session-ratio`: assumed, not measured).
  With a previous heartbeat line in the plan, both parts are scaled to the burn observed since. Launch only while
  the projection at the handoff stays under the cap (92 %). Never launch under 5 % of the week. Seven Opus
  builders burned about 1 % of the session window per minute at their heaviest. An "allowance 0" from usage now
  means the coordinator's own hours to the handoff do not fit: shorten the run, do not override it.
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
Measured on the night of 2026-11-10: Fable builders cost about 2.5-3 % of the week per million builder tokens with the
coordinator in (4 % -> 15 % in 3.3 h with two to six running), five times the assumed 1.2 + 0.3 per builder; Opus builders
on known-pattern files moved the meter about 1 point an hour with two to four running.

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
- Model by item (D72 as revised by D96): the table in docs/builders.md §4. A short, bounded item (one trial, one batch, three functions) is
  worth 30 to 60 minutes; a file is worth 2 to 2.5 hours.
- Log the launch: time, builder id, item, stop time, how many builders are now running.
- Push only at a heartbeat that found builders' work brought into the run branch since the last push: `python
  tools/run_branch.py push` (the pre-push check takes about 2.5 minutes). Contributors see the work before master
  does; the run's claims live while its PR is open, pushed or not, and the hosted checks skip the draft.
- Landing (D223): give a finished branch to the `lander` subagent (Agent tool, `subagent_type: lander`; Sonnet)
  with the worktree names in order, the target `run/<date>` and the addresses whose loss the user accepted,
  rather than running `wt.py bring-over` yourself; several finished at once go to one lander as one batch, and
  only one lander runs at a time. It returns one line per branch; a refusal names who acts next (resume that
  builder with the line).
- Checks and filing (D226): the harness check below and a POC run or replay go to the `checker` subagent
  (`subagent_type: checker`; Sonnet) with the commit or worktree, which checks, and for the POC the input, the
  seconds and the question; it returns one line per program, and a failure is still your health item. At a
  drain, the builders' traps, proposals and leftovers go to the `doc-filer` subagent (`subagent_type:
  doc-filer`; Sonnet) with the reports and the run branch; the log entry stays yours.
- When a report arrives: log the result line (pairs before and after, what is parked, proposals), file the
  levers and traps in workflow.md §3, open proposals for the user in progress.md §3, then run the heartbeat
  command again (the report changes the builder count and the memory).
- Filing a trap (D223; the 2026-11-10 rewrite cut §3 from 332 KB to 196 KB, most of it repeats filed per run):
  `grep -n` its key terms in workflow.md first. Already said: file nothing, or add the one missing fact to that
  bullet. A VC6 form goes under its `explain.py` class in §3.1.2 (a table row or a bullet under 900 characters,
  which `variants.py` cuts at); a one-file note under that file's `####` heading in §3.9; a landing, push, claim or
  pacing trap in this skill, not in workflow.md. §3.0, §3.1.1 and §3.3.1 are every builder's startup reading: add
  there only what a builder must know before its first edit. Write the rule, how to recognise the case and at most
  one example: no run date, builder id, commit hash or pair count, and never a bullet titled after a run. A trap a
  tool now refuses is one line naming the tool; a trap whose cause was fixed is deleted.
- In the same pass, prune progress.md §4 (Next steps): delete each item the report completes, and cut a part-done
  item down to what is left (its remaining addresses / functions). Never annotate an item as done ("made exact
  tonight", "done in run X"): what landed belongs in the plan log and the handoff's log entry, not in §4. Add the
  report's new leads to §4 only as open work, each filed under its numbered category (near misses in "Parked
  leftovers", interim placements in "Provisional placements", tool items in "Tools", the rest in "Work items" under its
  file), never as a dated or per-run bullet ("Leads of <run>", "Near misses of <date>"). No builder edits progress.md during a run, so this is safe at any time.

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

## Landing and run-branch traps

Landing and bring-over (the lander does the landing; these are what you see around it):
- **A batch can lose a function no builder lost alone** (one new declaration in a widely included header flips
  it): the landing's verify is the check that matters. D202: bring-over refuses an address lost outright; D66
  moves and shared holders land, printed, and still need the maintainer's `/accept-d66`.
- **If you run `wt.py bring-over` yourself, run it in the background**, never in a foreground call (a 600 s
  timeout moved one to the background, and stopping that task left its python, the importer lock and lease
  `importer-<pid>` running). Wait on the log's `landed:` line or `tasklist //FI "PID eq N"` (Git Bash can't see
  Windows processes); a wait loop on the verify log hangs if the batch stops at a conflict. `--dry` still runs the full verify.
- **During a landing:** a second bring-over refuses cleanly ("run moved while landing ... nothing landed, rerun");
  a run-branch commit made meanwhile is replayed under it when it touches none of the landing's paths and no
  generated file, otherwise rerun. Read a landing's exit before removing its worktree: the branch survives, the
  worktree's drafts do not.
- **Landing from the main tree instead:** a D66(i) COMDAT move (P-CS1-2), a D119 ambiguity move (not
  re-verified: no D119 handling found in wt.py) and a landing blocked by another builder's uncommitted hunk in
  names-review.md or library-matching.md go through commit_mine from the main tree with the worktree-verified
  shards (after a D119 move master's `in_tree_total` is one high until the next verify).
- **docs/imports.md on HEAD can be stale:** `report.py` regenerates ~100 unrelated lines; builders leave it out,
  you regenerate it.
- **A branch that commits its own SEND_SYMBOL_PAD cannot land once that value does not hold on the run head:** the
  landing's retune leaves tools/re/auxroots.py dirty and wt.py's regen_and_commit refuses ("regenerating changed other
  files as well"). Repair: `wt.py rebase ID`, commit in the branch the pad that holds on the run head, land. Briefs
  should forbid committing a pad, or say this.
- **The first landing on an unmoved run head must carry its pad:** bring-over's fast-forward path (target still at the
  branch's base) checks the branch's committed ledger record before any retune, so a branch whose header change needs a
  new pad and that left the pad uncommitted is refused "lost outright". Have that builder commit the pad
  (`tools/vc6/sendpad.py --watch-from <base>`: a plain sweep takes its watch set from HEAD's ledger, which already
  recorded the loss, and picks a wrong value), or land a pad-neutral branch first. Every later branch leaves the pad
  uncommitted and the landing retunes it.
- **The importer lock is the limit on parallel builders, not memory:** it is not first-come (a verify waited 50 minutes
  while later ones were served), a waiting verify keeps its VM, a sendpad sweep holds it 8 to 26 minutes, and report.py
  takes it too (it gives up after 600 s: `--lock-wait`). With six builders a verify cost 30 minutes. Briefs should
  point at the lock-free loops (workflow.md §3.4: `appfile.py match --pool`, `build.py --pool 1 --allow-dirty` +
  explain.py) and one verify per commit; keep a slot of lock time for the landings from two hours before the drain.
- **Stacked builders (`--base wt/P`) pay a rebase with its own verify** after P lands, and conflict when another landed
  branch touched the same file (a shared header's users): prefer landing P first when the lock is free.
- **A `wt.py rebase` record goes stale when another branch lands after it** (the next bring-over refuses the branch
  for the newly landed functions, "lost outright"): rebase immediately before the landing, and only a branch that
  needs it (a conflict, a landed parent); bring-over cherry-picks unrebased branches itself. Read the whole rebase
  log, not its last line: `grep WARNING` shows "the branch loses N address(es) outright", which names a real
  interaction with landed code and would refuse the whole batch that branch is in. bring-over has no accept flag: a
  user-accepted address must be an entry in `docs/data/accepted-corrections.json` on the target first.
- **After `wt.py rebase`, HEAD's ledger is the record commit that already recorded sclient.cpp's loss,** so sendpad.py's
  watch set misses the lost addresses and its printed window is wrong: read the per-value table in
  `.work/log/sendpad.log` for the values where 0x758210, 0x702240 and 0x74D990 are all exact, and set the middle of that window.
- **A worktree with uncommitted generated files (names-review.md, imports.md) makes the lander refuse at step 1:**
  discard them (`git checkout -- FILE`); the landing regenerates them.
- **A lander that hands back while its rebase or verify still runs cannot report a second time** (the harness refuses
  it): read `.work/log/land-ID.log`'s `landed:` line.
- **After a host reboot:** the pool VMs are Off and start on lease; run `vc6.py discover`, resume cut-off builders from
  their transcripts with SendMessage, and read the landing logs for what completed before relaunching a lander.

Stacked branches and merges:
- **A builder on another's unlanded branch:** `wt.py new C --base wt/P`; land P first or `bring-over P C` in that
  order (`A..B` for a range). After P lands, `wt.py list` says "parent landed: rebase" and `wt.py rebase C`
  replays C's own commits (no hand `git rebase --onto`); when they conflict with what landed, make a net-diff
  commit on a fresh worktree. A branch with merge commits is refused: `wt.py rebase ID --onto run/<date>`
  (the merge is dropped); a worktree reset onto a new base needs its wt base file updated, or bring-over refuses
  'merge commits'.
- **Never resolve a C++ source conflict by keeping both sides** (it duplicated gd_slider's code); appended new
  functions on both sides is the one safe case. `symbols.py` needs `--objs` for every verify-* folder and a good
  symbols.json as input (it reads its own output); commit_mine's library totals: `--refresh-totals` at a quiet
  tree.

PRs and the verify service:
- **A failed vc6-verify: check where and how often it ran before blaming the PR.** Two verifies of one PR at once
  share `.work/wt/prN.vf`: the second drops the first's worktree mid-build and both fail (reported once; not
  rechecked against verify_watch.py). verify_pr.py refuses to run off origin/master: run it from
  `.work/wt/verifier`.
- **A lease named `importer-<pid>` with no holder pid may be live** (the hosted verify: verify_watch.py ->
  verify_pr.py -> verify): check the pid's command line (`tasklist`, `Get-CimInstance Win32_Process`) before
  releasing a lease you did not take.

Claims and briefs:
- **`run_branch.py claim FILE` counts only functions its index thinks not exact** (a file came back "all exact"
  with four NOT EXACT functions): claim near misses by address range. It can hang minutes after a GitHub API rate
  limit ends; `claims.py check` shows the grant first. In PowerShell unquoted `0x...` arguments arrive as integers
  (`.work/coord/claim_many.ps1` normalises them).
- **Check a §4 item's addresses against the ledger and attempts.py before briefing it** (five items of one brief
  were all done or at LIMIT).

Stop times, launchers and POC checks:
- **Stop times carry the zone** (the clock fell back an hour at 02:00 PDT on 2026-11-01; next change 2027-03-14).
  Budget the importer lock's queue into them (agy's 75-minute timeout expired in it three times).
- **Claude Code's memory reaper kills `run_in_background` launchers:** start headless builders detached
  (PowerShell Start-Process). Burn-rate readings move in whole-percent steps: measure over an hour or more, not
  5-15 minutes.
- **`poc_check.sh NAME COMMIT LOG`:** LOG must be absolute (the script cds into .work/wt/NAME; leave it out for
  .work/poc-check-NAME.log); a leftover `wt/NAME` branch stops it at once (`wt.py remove NAME --delete-branch`);
  start it as a tracked background task (behind `&` it failed silently). With CHECK_HARNESS=1 it runs clienttest
  and udpreplay: lease first and set ENB_VC6_HOST(S). `.work/coord/pocfull.sh` is the coordinator's (instance
  coord). `clientpoc.py clean --all` / `--older HOURS` unlinks instances' junctions into ClientPOC\Data and keeps
  an instance whose client.exe still runs. For character select in the 44 s input3-quit replay, shoot at ~290000
  frames, not 2400 (menus run ~7,000 Presents a second).
- **agy:** `--mode auto` is not available on the account (`--dangerously-skip-permissions` suffices); under Git
  Bash set `MSYS_NO_PATHCONV=1` (also for `agy -p /usage`); agy runs its commands in PowerShell whatever the brief
  says; the stream's `usage` is per turn: sum the lines for a running item. `tools/agy/launch.sh` wants `RUN_DIR`
  relative to the repo.

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
