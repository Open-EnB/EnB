"""Locations of the repositories and data outside this one that the tools read.

Everything is under C:\\EnBDevelopment except the retail install. Change a path here and every
tool follows. docs/toolchain.md section 3 lists what each one is for. A machine with other
locations sets environment variables instead: ENB_DEV_ROOT, ENB_EA_ROOT, ENB_SDK_ROOT,
ENB_EA_REFERENCE, ENB_LIVE_ERA_DATA, ENB_RETAIL_INSTALL (the repository itself stays at
C:\\EnBDevelopment\\EnB: the build VM mirrors that path, docs/setup-vc6.md).

    python tools/re/roots.py      prints every root and whether it exists
"""
import os


def _env(var, default):
    return os.environ.get(var) or default


DEV_ROOT = _env('ENB_DEV_ROOT', r'C:\EnBDevelopment')

# EA's published Westwood source (Renegade, Generals, Zero Hour, ...)
EA_ROOT = _env('ENB_EA_ROOT', os.path.join(DEV_ROOT, 'EA'))
# DirectX 8 and RAD Tools SDKs (radtools\include_enb is derived by tools/vc6/sdk_patch.py)
SDK_ROOT = _env('ENB_SDK_ROOT', os.path.join(DEV_ROOT, 'SDK'))
# SGI STL 3.3 (D87; the source of libs\sgistl's VC6-adapted hash_map headers)
SGI_STL = os.path.join(SDK_ROOT, 'stl-3.3')

# The EA captures' frozen reference data (D154): ea\NAME.trace.jsonl and ref\NAME.*.txt, unpacked by
# tools/harness/fetch_reference.py from the project's release archive
EA_REFERENCE = _env('ENB_EA_REFERENCE', os.path.join(DEV_ROOT, 'Tools', 'Media', 'ea-reference'))
EA_TRACES = os.path.join(EA_REFERENCE, 'ea')
EA_REF = os.path.join(EA_REFERENCE, 'ref')

# Live-Era-Data: EA's raw UDP packet dumps (capture_1..3 are public: trunk/capturedPackets of
# https://github.com/therealKyp/Earth-and-Beyond-server)
LIVE_ERA_DATA = _env('ENB_LIVE_ERA_DATA', os.path.join(DEV_ROOT, 'Live-Era-Data'))
LIVE_PACKETS = os.path.join(LIVE_ERA_DATA, 'enbserver3', 'ENB-Packets-Live')

# The retail install (client.exe, game data); read-only, never used as a working directory
RETAIL_INSTALL = _env('ENB_RETAIL_INSTALL', r'C:\Program Files (x86)\EA GAMES\Earth & Beyond')
RETAIL_CLIENT = os.path.join(RETAIL_INSTALL, 'release', 'client.exe')


def main():
    for name, val in sorted(globals().items()):
        if name.isupper() and isinstance(val, str):
            print('%-18s %-6s %s' % (name, 'ok' if os.path.exists(val) else 'MISSING', val))


if __name__ == '__main__':
    main()
