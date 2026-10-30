#!/usr/bin/env bash
# Byte-matches the precompiled static libraries linked into client.exe: VC6 SP4's C runtime
# (LIBCMT), C++ standard library (LIBCPMT) and COM support (COMSUPP), fetched from the build VM,
# and the DirectX 8.0 SDK's D3DX (d3dx8.lib). No compiling: the objects inside the .lib files are
# the code. Writes docs/data/bytematch-{libcmt,libcpmt,comsupp,d3dx8}.md/.json.
#
#   bash tools/vc6/runtime_match.sh
set -eu
HERE="$(cd "$(dirname "$0")" && pwd)"
REPO="$(dirname "$(dirname "$HERE")")"
RE="$REPO/tools/re"
W="$REPO/.work/libs"
HOST="${ENB_VC6_HOST:?set ENB_VC6_HOST=<user>@<ip> (pool.py lease prints it)}"
KEY="${ENB_VC6_KEY:-$HOME/.ssh/enb_vc6}"
VCLIB="C:/Program Files (x86)/Microsoft Visual Studio/VC98/Lib"
mkdir -p "$W"
for f in LIBCMT.LIB LIBCPMT.LIB COMSUPP.LIB; do
  scp -q -i "$KEY" "$HOST:$VCLIB/$f" "$W/"
done
cp C:/EnBDevelopment/SDK/directx8/lib/d3dx8.lib "$W/"

match() {  # lib file, short name, title
  rm -rf "$W/o_$2"
  python "$RE/libextract.py" "$W/$1" "$W/o_$2"
  python "$RE/objmatch.py" -q --json "$W/m_$2.json" "$W/o_$2"/*.obj
  python "$RE/matchreport.py" --title "$3" --out "$REPO/docs/data/bytematch-$2" "L=$W/m_$2.json"
}
match LIBCMT.LIB libcmt "C runtime (LIBCMT.LIB, VC6 SP4)"
match LIBCPMT.LIB libcpmt "C++ standard library (LIBCPMT.LIB, VC6 SP4)"
match COMSUPP.LIB comsupp "COM support (COMSUPP.LIB, VC6 SP4)"
match d3dx8.lib d3dx8 "D3DX 8.0 (d3dx8.lib, DirectX 8.0 SDK)"
