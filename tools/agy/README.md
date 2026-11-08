# agy builders (D187)

Builders through the Antigravity CLI (`agy`, on the maintainer's Google subscription; no API key). D187 admits
**Gemini 3.8 Flash (high)** for bounded POC and triage items (the functions of one file, 30-60 minutes), next to D72's
Claude builders. Not Opus through agy: its per-model quota ran out after one 8-minute item (2026-11-02).

| Script | Does |
|---|---|
| `launch.sh ID [MODEL] [TIMEOUT]` | one headless builder in worktree `.work/wt/ID`; `RUN_DIR` names the run folder with `brief-ID.md` and `common.md`; stream to `ID.jsonl`, start / exit in `ID.err` |
| `poc_check.sh NAME COMMIT [LOG]` | the coordinator's POC run at a commit (a worktree, app build, clientpoc link, 40 s run); `CHECK_HARNESS=1` adds clienttest and udpreplay |
| `rebase_gen.sh ID run/<date> [BASELINE]` | `wt.py rebase ID --onto run/<date>` then `wt.py bring-over` (bring-over alone now resolves generated-file conflicts) |

A run (the coordinator-pacing skill's "agy builders" section has the pacing):

1. `python tools/vc6/wt.py new ID`, claim its functions (`run_branch.py claim`), write `RUN_DIR/brief-ID.md`
   (a deadline, the claimed range; name every WinMain stand-in that opens a dialog and say it stays: 0x847D20, 0x846C90).
   `common.md`: the run's base and baseline, the worktree rules (`--pool 1`), verify and pairs_cmp, commit by name with
   `Co-Authored-By: Gemini 3.8 Flash (High) via Antigravity <noreply@google.com>`, never push.
2. `RUN_DIR=... sh tools/agy/launch.sh ID gemini-3.8-flash-high 55m` in the background (a permission rule for the
   launcher avoids the auto-mode check's refusals).
3. At its exit: the report is the last line's `result.response`, the cost `result.usage.total_tokens`; then
   `python tools/vc6/wt.py bring-over ID --target run/<date> --pool 1` (without `--target` it lands on master), or
   `wt.py rebase ID --onto run/<date>` when it refuses for another file's conflict (the builder resolves it).
4. The builders' POC runs crash in WW3D::Set_Gamma (a process agy starts gets no usable D3D device): run
   `poc_check.sh` yourself after landing POC code, and with `CHECK_HARNESS=1` before the handoff.
