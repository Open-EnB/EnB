#!/bin/sh
# Land a builder worktree whose cherry-pick `wt.py bring-over` refused for generated-file conflicts (two builders from
# one base both rewrite imports.json / names-review.md): rebase it onto the run branch, take the run's side of each
# generated file (attempts.csv: both sides' rows), regenerate, reverify, then bring it over.
#   sh tools/agy/rebase_gen.sh ID run/<date> [BASELINE_IMPORTS_JSON]
# Stops (exit 2) on any conflict in a file that is not generated: resolve that one by hand, and on a modified tracked
# file in the worktree that is not generated (uncommitted generated leftovers are dropped, by name, and said).
# Several branches at once: `wt.py bring-over A B C --target run/<date>` (one verify over all of them).
ID=$1; BR=${2:?the run branch, e.g. run/2026-11-02}
R=$(cd "$(dirname "$0")/../.." && pwd)
B=${3:-}
# the baseline is read from inside the worktree: make it absolute while the caller's directory still counts
case "$B" in ''|/*|[A-Za-z]:*) ;; *) B=$(pwd)/$B ;; esac
L=$R/.work/rebase-$ID.log
GEN="docs/data/imports.json docs/data/names-review.md docs/data/not-exact.md docs/imports.md docs/data/attempts.csv"
cd "$R/.work/wt/$ID" || exit 1
: > "$L"
for f in $(git diff --name-only HEAD); do
  case " docs/data/imports.json docs/data/names-review.md docs/data/not-exact.md docs/imports.md " in
    *" $f "*) git checkout HEAD -- "$f"; echo "generated leftover dropped: $f" | tee -a "$L" ;;
    *) echo "uncommitted non-generated file: $f; nothing done" | tee -a "$L"; exit 2 ;;
  esac
done
git rebase "$BR" >> "$L" 2>&1
while [ -d "$(git rev-parse --git-path rebase-merge)" ]; do
  U=$(git diff --name-only --diff-filter=U)
  for f in $U; do
    case " $GEN " in *" $f "*) ;; *) echo "non-generated conflict: $f" | tee -a "$L"; exit 2;; esac
  done
  for f in $U; do
    if [ "$f" = docs/data/attempts.csv ]; then
      git show :2:$f > .work/att.ours; git show :3:$f > .work/att.theirs
      python -c "o=open('.work/att.ours',encoding='utf-8').read().splitlines();t=open('.work/att.theirs',encoding='utf-8').read().splitlines();s=set(o);open('$f','w',encoding='utf-8',newline='\n').write('\n'.join(o+[x for x in t if x not in s])+'\n')"
    else
      git checkout --ours -- $f
    fi
  done
  python tools/re/names.py >> "$L" 2>&1
  python tools/re/notexact.py >> "$L" 2>&1
  python tools/vc6/importer.py verify --lib app --pool 1 --lock-wait 3000 >> "$L" 2>&1
  python tools/vc6/importer.py report --lock-wait 3000 >> "$L" 2>&1
  git add $GEN
  GIT_EDITOR=true git rebase --continue >> "$L" 2>&1 || true
done
[ -n "$B" ] && python tools/vc6/pairs_cmp.py "$B" docs/data/imports.json | grep -E '^(gained|lost outright) ' >> "$L"
cd "$R" && python tools/vc6/wt.py bring-over $ID --target "$BR" --pool 1 >> "$L" 2>&1
echo "bring-over rc=$?" >> "$L"
tail -4 "$L"
