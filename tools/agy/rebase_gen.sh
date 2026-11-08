#!/bin/sh
# Rebase a builder worktree onto the run branch and land it: `wt.py rebase ID --onto run/<date>` (generated files take
# the run's side, attempts.csv keeps both sides' rows, then verify of the touched libraries, report, names.py,
# notexact.py and a commit of the generated files), then `wt.py bring-over ID --target run/<date>`.
#   sh tools/agy/rebase_gen.sh ID run/<date> [BASELINE_IMPORTS_JSON]
# Since 2026-11-07 a call to wt.py (pipeline-plan §4 item 2); same CLI. Exit 2 when the rebase stops on a conflict in a
# file that is not generated, or on an uncommitted non-generated file (generated leftovers are dropped and said): wt.py
# prints the commands to finish it (`wt.py rebase ID --continue`) or abort it. Single bring-over now resolves
# generated conflicts by itself; this script is for a branch you want rebased in place first.
# Several branches at once: `wt.py bring-over A B C --target run/<date>` (one verify over all of them).
ID=$1; BR=${2:?the run branch, e.g. run/2026-11-02}
R=$(cd "$(dirname "$0")/../.." && pwd)
B=${3:-}
case "$B" in ''|/*|[A-Za-z]:*) ;; *) B=$(pwd)/$B ;; esac
L=$R/.work/rebase-$ID.log
cd "$R" || exit 1
python tools/vc6/wt.py rebase "$ID" --onto "$BR" --pool 1 > "$L" 2>&1 || { tail -8 "$L"; exit 2; }
[ -n "$B" ] && (cd "$R/.work/wt/$ID" && python "$R/tools/vc6/pairs_cmp.py" "$B" docs/data/imports | grep -E '^(gained|lost outright) ') >> "$L"
python tools/vc6/wt.py bring-over "$ID" --target "$BR" --pool 1 >> "$L" 2>&1
echo "bring-over rc=$?" >> "$L"
tail -4 "$L"
