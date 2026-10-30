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
6. **Report to the user** in five lines or fewer: what you wrote and where, what is
   uncommitted, and the single prompt to paste into the new session, e.g.
   `Read CLAUDE.md and the handoff, then continue with <next step>.`
7. Ask before committing the handoff file(s). Never push `main` (D148). After the commit, the coordinator's run
   PR: release the claims whose builders brought nothing in (`python tools/run_branch.py release N`), write the
   PR summary to `.work/run-<date>/pr-summary.md` (what landed, pairs before and after, what is parked, the
   decisions waiting for the user), then `python tools/run_branch.py ready --summary .work/run-<date>/pr-summary.md`:
   it pushes the run branch and takes the PR out of draft. The verify service posts `vc6-verify` within minutes;
   the user merges. Report the PR's link.
