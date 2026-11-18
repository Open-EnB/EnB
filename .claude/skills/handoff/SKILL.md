---
name: handoff
description: Wrap up the current session so a fresh one can resume cheaply. Use when the user says "handoff", "wrap up", "end session", or when context is large (over about 250k tokens) and work is at a natural break.
disable-model-invocation: true
---

# Handoff

Goal: leave the repo's state documents accurate, so the next session starts from a small
file instead of from this session's history. You do NOT clear the context yourself; you
prepare the handoff and tell the user to run `/clear` (or start a new session).

## Steps

1. **Find the target.** Read the `## Handoff target` section of this repo's `CLAUDE.md`.
   It names the file(s) to update and whether to overwrite or append. If it is missing, ask.
2. **Collect facts, don't recall them.** Run `git status --short`, `git log --oneline -15`,
   `git diff --stat` and `python tools/eol.py fix` (D147: working copies a Windows tool left CRLF). List uncommitted work explicitly. Never claim something was
   committed, built or tested unless a command in this session showed it.
3. **Release VMs this session leased.** Run `python tools/vc6/pool.py release --agent <name>`
   for each agent name this session leased under, then `python tools/vc6/pool.py stop-idle
   --minutes 0`. Run `python tools/vc6/pool.py list` and report any VM still leased (by whom).
4. **Write the handoff** (keep it under about 60 lines; prefer links to prose):
   - **Done**: what changed, with commit hashes or file paths.
   - **Verified vs not**: separate "tested live by the user", "tests/build pass", and
     "written but not run". The user does the live testing; say what is waiting on them.
   - **Decisions**: each with date and who decided (user vs. proposed by you). Use the
     repo's existing numbering (D#, F#, R#, Q#) and do not invent new ones.
   - **Open questions**: numbered, with your recommendation for each.
   - **Next step**: ONE concrete next action, plus the exact command(s) to start it.
   - **Traps found**: anything that cost time this session (also add to the repo's
     known-traps doc if it has one).
   - **State outside git**: scratch files, VM addresses, running processes, captures.
5. **Don't duplicate.** If a fact already lives in a design doc, link to its section.
   Update milestone/status tables in place instead of restating them.
   **Sweep the Next steps list** (progress.md §4) before writing the entry: for each item, check its
   addresses against the ledger `docs/data/imports/` (in_tree) and `docs/data/not-exact.md`, and its other claims
   against the tree. Delete items that are done; rewrite part-done items to say only what is left. Never mark
   an item done in place: its history goes in the log entry. New leads go in as open work only.
6. **Report to the user** in five lines or fewer: what you wrote and where, what is
   uncommitted, and the single prompt to paste into the new session, e.g.
   `Read CLAUDE.md and the handoff, then continue with <next step>.`
7. Ask before committing the handoff file(s). Never push `main` (D148). After the commit, the coordinator's run
   PR: release the claims whose builders brought nothing in (`python tools/run_branch.py release N`), write the
   PR summary to `.work/run-<date>/pr-summary.md` (what landed, pairs before and after, what is parked, the
   calls the coordinator made under D246 with any valve losses by address, and the few items D246 (d) leaves
   to the user), then `python tools/run_branch.py ready --summary .work/run-<date>/pr-summary.md`:
   it pushes the run branch and takes the PR out of draft. **When the run changed
   `docs/data/accepted-corrections.json` (D240 / D246 entries added, regained ones deleted), `ready` first opens a
   second PR holding nothing but that file (`accept-corrections-run-<date>`, from origin/master, D268) and names it
   atop the run PR's summary**: the verify service judges a PR with master's corrections list, never the PR's
   (D202), so the run PR's `vc6-verify` fails on those losses until the user merges the corrections PR; it then
   re-verifies on its own. `python tools/run_branch.py corrections` does that step alone (rerun it after a later
   corrections change; it updates the same PR). The user merges the corrections PR (D261's self-merge does not
   cover it); never put any other file on it. The verify service posts `vc6-verify` within minutes; the user
   merges. Report both PRs' links, the corrections PR first.
