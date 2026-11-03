#!/bin/sh
# The coordinator's POC check at a commit (D187: agy builders' own POC runs crash in WW3D::Set_Gamma, so they prove nothing):
#   sh tools/agy/poc_check.sh NAME COMMIT [LOG]      builds app in worktree .work/wt/NAME, links clientpoc, runs it 40 s
# With CHECK_HARNESS=1 it also runs clienttest (D182) and udpreplay alex3 + handshake.
N=$1; C=$2
R=$(cd "$(dirname "$0")/../.." && pwd)
L=${3:-$R/.work/poc-check-$N.log}
cd "$R" || exit 1
[ -d .work/wt/$N ] || python tools/vc6/wt.py new $N --base $C > "$L" 2>&1
cd .work/wt/$N || exit 1
mkdir -p build
cp -r "$R/build/art" build/ 2>/dev/null
cp -r "$R/build/d3d8shot" build/ 2>/dev/null
cp "$R/build/art/MSS32.DLL" build/ 2>/dev/null
echo "== build $(date +%T)" >> "$L"
python tools/vc6/build.py build --lib app --allow-dirty --pool 1 >> "$L" 2>&1
echo "== link $(date +%T)" >> "$L"
python tools/vc6/build.py link --program clientpoc --pool 1 >> "$L" 2>&1
echo "== run $(date +%T)" >> "$L"
python tools/harness/clientpoc.py --seconds 40 --shots 60000,120000 run >> "$L" 2>&1
echo "== poc exit $? $(date +%T)" >> "$L"
if [ "$CHECK_HARNESS" = 1 ]; then
  echo "== clienttest $(date +%T)" >> "$L"
  python tools/harness/clienttest.py build >> "$L" 2>&1
  python tools/harness/clienttest.py run >> "$L" 2>&1; echo "clienttest exit $?" >> "$L"
  echo "== udpreplay $(date +%T)" >> "$L"
  python tools/vc6/pool.py lease --agent $N > .work/lease.txt 2>&1
  IP=$(grep -o '[0-9]*\.[0-9]*\.[0-9]*\.[0-9]*' .work/lease.txt | head -1)
  ENB_VC6_HOSTS=Daniel@$IP python tools/harness/udpreplay.py build >> "$L" 2>&1
  python tools/harness/udpreplay.py run alex3 >> "$L" 2>&1; echo "udpreplay alex3 exit $?" >> "$L"
  python tools/harness/udpreplay.py handshake >> "$L" 2>&1; echo "handshake exit $?" >> "$L"
  python tools/vc6/pool.py release --agent $N >> "$L" 2>&1
fi
echo "== done $(date +%T)" >> "$L"
grep -E '^== |compiled|exe written|still running|crash|checks|exit' "$L" | tail -12
