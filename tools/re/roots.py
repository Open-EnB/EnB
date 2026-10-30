"""Locations of the repositories and data outside this one that the tools read.

Everything is under C:\\EnBDevelopment except the retail install. Change a path here and every
tool follows. docs/toolchain.md section 3 lists what each one is for. A machine with other
locations sets environment variables instead: ENB_DEV_ROOT, ENB_EA_ROOT, ENB_SDK_ROOT,
ENB_NET7_ROOT, ENB_refdecoder_ROOT, ENB_LIVE_ERA_DATA, ENB_RETAIL_INSTALL (the repository itself stays
at C:\\EnBDevelopment\\EnB: the build VM mirrors that path, docs/setup-vc6.md).

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

# Net-7 server emulator, C++ (AuxClasses = the aux-data classes the server writes)
NET7_ROOT = _env('ENB_NET7_ROOT', os.path.join(DEV_ROOT, 'Net7'))
NET7_AUXCLASSES = os.path.join(NET7_ROOT, 'Net7', 'AuxClasses')

# the reference decoder (the reference decoder, C#; was the reference decoder until 2026-10-25): game model, Westwood keys, the reference decoder
REFDECODER_ROOT = _env('ENB_refdecoder_ROOT', os.path.join(DEV_ROOT, 'the reference decoder'))
N7_OBJECTS = os.path.join(REFDECODER_ROOT, 'src', 'Shared', 'the reference game model', 'Objects')
N7_SCHEMA = os.path.join(REFDECODER_ROOT, 'src', 'Shared', 'the reference game model', 'Schema', 'client-auxdata.json')
N7_WESTWOOD_KEYS = os.path.join(REFDECODER_ROOT, 'src', 'Shared', 'the reference key table', 'the reference key table')
refdecoder_EXE = os.path.join(REFDECODER_ROOT, 'src', 'Harness', 'the reference decoder', 'bin', 'Release',
                           'net10.0', 'the reference decoder.exe')

# the reference decoder traces (captures\ea\*.trace.jsonl, captures\session_*.trace.jsonl): moved into the proxy
# repository's captures/ when the reference traces was retired (2026-10-25)
REF_TRACES_ROOT = REFDECODER_ROOT
N7_CAPTURES = os.path.join(REF_TRACES_ROOT, 'captures')

# Live-Era-Data: EA's raw UDP packet dumps
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
