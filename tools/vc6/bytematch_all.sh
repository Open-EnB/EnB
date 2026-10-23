#!/usr/bin/env bash
# Compiles every WWVegas library from each EA copy (Renegade R, Generals G, Zero Hour Z) with the
# client's compiler and flags, each copy against its own sibling libraries, byte-matches the
# objects against client.exe and writes docs/data/bytematch-<lib>.md/.json.
#
#   bash tools/vc6/bytematch_all.sh [WORKDIR]
set -u
HERE="$(cd "$(dirname "$0")" && pwd)"
TOOLS="$(dirname "$HERE")"
DATA="$(dirname "$TOOLS")/docs/data"
WORK="${1:-${TEMP:-/tmp}/enb_bytematch}"
EA=C:/EnBDevelopment/EA
DX=C:/EnBDevelopment/SDK/directx8/include
RAD=C:/EnBDevelopment/SDK/radtools/include_enb   # tools/vc6/sdk_patch.py
mkdir -p "$WORK"

declare -A ROOT=(
  [R]=$EA/CnC_Renegade/Code
  [G]=$EA/CnC_Generals_Zero_Hour/Generals/Code/Libraries/Source/WWVegas
  [Z]=$EA/CnC_Generals_Zero_Hour/GeneralsMD/Code/Libraries/Source/WWVegas
)
# Directory names differ by copy.
declare -A NAME=(
  [R:wwlib]=wwlib [R:ww3d2]=ww3d2 [R:wwmath]=WWMath [R:wwaudio]=WWAudio [R:wwdebug]=wwdebug [R:wwsaveload]=wwsaveload
  [G:wwlib]=WWLib [G:ww3d2]=WW3D2 [G:wwmath]=WWMath [G:wwaudio]=WWAudio [G:wwdebug]=WWDebug [G:wwsaveload]=WWSaveLoad
  [Z:wwlib]=WWLib [Z:ww3d2]=WW3D2 [Z:wwmath]=WWMath [Z:wwaudio]=WWAudio [Z:wwdebug]=WWDebug [Z:wwsaveload]=WWSaveLoad [Z:wwshade]=wwshade
)
# wwdebug.h in Generals and Zero Hour includes ..\..\..\..\gameengine\include\common\debug.h
declare -A SYNC=(
  [R]=""
  [G]="--sync $EA/CnC_Generals_Zero_Hour/Generals/Code/GameEngine/Include/Common"
  [Z]="--sync $EA/CnC_Generals_Zero_Hour/GeneralsMD/Code/GameEngine/Include/Common"
)

incs() {  # include flags for copy $1: its own WW libraries, then the SDKs
  local t=$1 out=""
  for l in wwlib wwdebug wwmath wwsaveload wwaudio ww3d2 wwshade; do
    local n=${NAME[$t:$l]:-}
    [ -n "$n" ] && [ -d "${ROOT[$t]}/$n" ] && out="$out -I ${ROOT[$t]}/$n"
  done
  echo "$out -I $DX -I $RAD"
}

# flags where a library's differ from vc6.py's default (importer.py LIB_FLAGS)
declare -A LIBFLAGS=( [wwshade]="/nologo /c /MT /W3 /O2 /Ob1 /GX /D _MBCS /D ENABLE_WWPROFILE" [wwaudio]="/nologo /c /MT /W3 /O2 /Ob2 /D _MBCS" )

for lib in wwlib wwdebug wwmath wwsaveload wwaudio ww3d2 wwshade; do
  runs=""
  fl=${LIBFLAGS[$lib]:-}
  for t in R G Z; do
    n=${NAME[$t:$lib]:-}
    [ -z "$n" ] && continue
    src="${ROOT[$t]}/$n"
    ls "$src"/*.cpp >/dev/null 2>&1 || continue
    out="$WORK/$t-$lib"
    rm -rf "$out"
    python "$HERE/vc6.py" compile --out "$out" ${fl:+--flags "$fl"} ${SYNC[$t]} $(incs $t) "$src"/*.cpp > "$out.log" 2>&1
    nobj=$(ls "$out"/*.obj 2>/dev/null | wc -l)
    nsrc=$(ls "$src"/*.cpp | wc -l)
    [ "$nobj" -eq 0 ] && { echo "$t $lib: nothing compiled"; continue; }
    printf '%s %-10s %3d/%3d files  ' "$t" "$lib" "$nobj" "$nsrc"
    python "$TOOLS/re/objmatch.py" -q --json "$out.json" "$out"/*.obj
    runs="$runs $t=$out.json"
  done
  [ -n "$runs" ] && python "$TOOLS/re/matchreport.py" --title "$lib" --flags "${fl:-/MT /W3 /O2 /Ob2}" \
    --out "$DATA/bytematch-$lib" $runs | sed "s/^/  -> /"
done
