#!/bin/sh
# A headless builder through the Antigravity CLI (agy, the maintainer's Google subscription; D187):
#   RUN_DIR=.work/run-<date>-agy sh tools/agy/launch.sh ID [MODEL] [TIMEOUT]
#   e.g. RUN_DIR=.work/run-2026-11-03-agy sh tools/agy/launch.sh eam6 gemini-3.8-flash-high 55m
# The builder works in its own worktree .work/wt/<ID> (make it first: python tools/vc6/wt.py new ID).
# Prompt = $RUN_DIR/brief-ID.md + $RUN_DIR/common.md. The stream goes to $RUN_DIR/ID.jsonl: its last line's
# result.usage.total_tokens is the builder's cost (never the builder's own figure); start / exit in $RUN_DIR/ID.err.
ID=$1; MODEL=${2:-gemini-3.8-flash-high}; TIMEOUT=${3:-55m}
R=$(cd "$(dirname "$0")/../.." && pwd)
D=$R/${RUN_DIR:?set RUN_DIR to the run folder holding brief-ID.md and common.md}
WT=$R/.work/wt/$ID
cd "$WT" || { echo "no worktree $WT: python tools/vc6/wt.py new $ID" >&2; exit 1; }
WTWIN=$(cygpath -w "$WT" 2>/dev/null || echo "$WT")
PROMPT="You are builder $ID in the E&B reconstruction repo. Your worktree (cwd, work only here, by absolute path): $WTWIN (branch wt/$ID). Your brief: $(cat "$D/brief-$ID.md")

$(cat "$D/common.md")"
echo "start $(date '+%F %T') model $MODEL timeout $TIMEOUT" > "$D/$ID.err"
# --mode auto is not available on the maintainer's account; --dangerously-skip-permissions is what lets agy edit and run.
# MSYS_NO_PATHCONV: Git Bash would rewrite a leading / in an argument into a Windows path.
MSYS_NO_PATHCONV=1 ENB_AGENT=$ID agy -p "$PROMPT" \
  --model "$MODEL" --dangerously-skip-permissions --print-timeout "$TIMEOUT" \
  --output-format stream-json > "$D/$ID.jsonl" 2>> "$D/$ID.err"
echo "exit $? at $(date '+%F %T')" >> "$D/$ID.err"
