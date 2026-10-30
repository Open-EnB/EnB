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
   makes `run/<date>` from origin/master (or brings an existing one up to it), pushes it, opens the run's PR as a
   draft and writes the baseline below. Then `python tools/claims.py list`: assign builders only functions no
   live claim holds (contributors' claims included).
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
whether HEAD's imports.json parses and which addresses are lost outright against the baseline (pairs that
reappear under another file are rename moves, D95(c), not losses); the burn rate observed since the previous
heartbeat line (per builder, per hour), or the defaults when there is none; the projection at the handoff time
with the builders running now; and the allowance: how many more builders may start.

The rules behind the allowance (defaults in the script, all overridable):

- **Memory:** one more builder per 2 GB free above a 3 GB floor. A stopped clone starts on lease and takes its
  startup memory, so count it before it exists.
- **Usage:** a builder costs about 2 % of the week per hour (Opus; Sonnet about a fifth, Haiku a twentieth),
  and 4 % of the session window per hour. Launch only while the projection at the handoff stays under the cap
  (92 %). Never launch under 5 % of the week. Seven Opus builders burned about 1 % of the session window per
  minute at their heaviest; the observed rate in the log is the better guide once two heartbeats exist.
- **Health:** no launch while HEAD's imports.json does not parse or an address is lost outright. Fix first
  (usually a builder's commit script: see workflow.md §3.8).

Observed on 2026-10-27: seven builders at a time on a seven-VM pool left 3 to 4 GB free on the host;
usage ran 1.3 to 2.5 % of the week per hour per Opus builder depending on the item.

## Launching

- **Claim the builder's functions first:** `python tools/run_branch.py claim UNIT --builder <id> --model <m>`
  (`--split` for a file over 30 not-yet-exact functions: one claim per 30). It adds `Claims #N` to the run's PR. A
  claim that is not granted means someone else holds part of it: give the builder another item. Put the granted
  ranges in the brief; a builder that finds its work leading into a claimed range stops and reports.
- Give every brief a hard stop time and a report time, both earlier than the handoff by the time you need to
  log the result and file its traps (about 15 minutes per report).
- Model by item (D72, until the D93 evaluation revises it): Opus for byte-exact rebuilding, Sonnet for bounded
  tools fixes and audits, Haiku for searches. A short, bounded item (one trial, one batch, three functions) is
  worth 30 to 60 minutes; a file is worth 2 to 2.5 hours.
- Log the launch: time, builder id, item, stop time, how many builders are now running.
- When a heartbeat finds builders' work brought into the run branch since the last push, `python
  tools/run_branch.py push` (the pre-push check takes about 2.5 minutes): the run's claims stay alive while its PR
  is a draft, and contributors see the work before master does.
- When a report arrives: log the result line (pairs before and after, what is parked, proposals), file the
  levers and traps in workflow.md §3, open proposals for the user in progress.md §3, then run the heartbeat
  command again (the report changes the builder count and the memory).

## Draining to a handoff

- Decide the last launch from the projection: once the budget to the handoff is spoken for, say in the log
  that the window is closed, and keep it closed unless the user reopens it.
- While the last builders run, draft the progress.md entry and the §4 refresh as scripts in `.work/coord/`
  (the Write tool, never a heredoc) with slots for the last results; run them after the final report.
- Then: `python tools/vc6/pool.py stop-idle --minutes 0`, `git status` clean except `.claude/settings.json`,
  HEAD's imports.json parses, delete the heartbeat cron, log the end with the final usage figures.
- Claims: `python tools/run_branch.py release N` for each claim whose builder brought nothing in (it drops the
  `Claims #N` line too); the others land with the run's PR. At handoff `run_branch.py ready --summary FILE`
  (the handoff skill) takes the PR out of draft; the verify service verifies it and the maintainer merges it.
- If the user says to spend the quota before a reset, keep launching bounded items up to the allowance and
  let them run into the next window; the handoff then gets an addendum after the last report. If the user says
  nothing new starts, close the window at once and log it.

## Session caps

If the session window caps mid-run, the builders stop where they are with their leases intact; nothing is lost.
At the reset, check HEAD's health, then resume each one from its transcript (SendMessage) with a new stop time.
Plan the run so this does not happen: the projection above exists for that.
