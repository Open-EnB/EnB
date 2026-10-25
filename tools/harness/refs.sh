#!/bin/sh
# Saves the reference decoder's reference outputs per trace under .work/harness/ref/ (tools/harness/README.md)
# external roots: tools/re/roots.py (refdecoder_EXE, N7_CAPTURES)
HERE=$(cd "$(dirname "$0")" && pwd)
REPO=$(cd "$HERE/../.." && pwd)
N7=/c/EnBDevelopment/the reference decoder/src/Harness/the reference decoder/bin/Release/net10.0/the reference decoder.exe
CAP=/c/EnBDevelopment/the reference traces/captures
OUT=$REPO/.work/harness/ref
mkdir -p $OUT
for f in $CAP/ea/*.trace.jsonl $CAP/session_*.trace.jsonl; do
  b=$(basename $f .trace.jsonl)
  "$N7" dump "$f" --summary > $OUT/$b.summary.txt 2>&1
  for cmd in aux-census roundtrip; do
    "$N7" $cmd "$f" > $OUT/$b.$cmd.txt 2>&1; echo "$b $cmd exit $?" >> $OUT/exitcodes.txt
  done
  "$N7" aux-log "$f" --first > $OUT/$b.aux-log-first.txt 2>&1; echo "$b aux-log exit $?" >> $OUT/exitcodes.txt
done
echo done >> $OUT/exitcodes.txt
