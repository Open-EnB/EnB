"""Locations of the repositories and data outside this one that the tools read.

Everything is under C:\\EnBDevelopment except the retail install. Change a path here and every
tool follows. docs/toolchain.md section 3 lists what each one is for.

    python tools/re/roots.py      prints every root and whether it exists
"""
import os

DEV_ROOT = r'C:\EnBDevelopment'

# EA's published Westwood source (Renegade, Generals, Zero Hour, ...)
EA_ROOT = os.path.join(DEV_ROOT, 'EA')
# DirectX 8 and RAD Tools SDKs (radtools\include_enb is derived by tools/vc6/sdk_patch.py)
SDK_ROOT = os.path.join(DEV_ROOT, 'SDK')

# Net-7 server emulator, C++ (AuxClasses = the aux-data classes the server writes)
NET7_ROOT = os.path.join(DEV_ROOT, 'Net7')
NET7_AUXCLASSES = os.path.join(NET7_ROOT, 'Net7', 'AuxClasses')

# the reference decoder (C#): game model, Westwood keys, the reference decoder
REFDECODER_ROOT = os.path.join(DEV_ROOT, 'the reference decoder')
N7_OBJECTS = os.path.join(REFDECODER_ROOT, 'src', 'Shared', 'the reference game model', 'Objects')
N7_SCHEMA = os.path.join(REFDECODER_ROOT, 'src', 'Shared', 'the reference game model', 'Schema', 'client-auxdata.json')
N7_WESTWOOD_KEYS = os.path.join(REFDECODER_ROOT, 'src', 'Shared', 'the reference key table', 'the reference key table')
refdecoder_EXE = os.path.join(REFDECODER_ROOT, 'src', 'Harness', 'the reference decoder', 'bin', 'Release',
                           'net10.0', 'the reference decoder.exe')

# the reference traces: the reference decoder traces (captures\ea\*.trace.jsonl, captures\session_*.trace.jsonl)
REF_TRACES_ROOT = os.path.join(DEV_ROOT, 'the reference traces')
N7_CAPTURES = os.path.join(REF_TRACES_ROOT, 'captures')

# Live-Era-Data: EA's raw UDP packet dumps
LIVE_ERA_DATA = os.path.join(DEV_ROOT, 'Live-Era-Data')
LIVE_PACKETS = os.path.join(LIVE_ERA_DATA, 'enbserver3', 'ENB-Packets-Live')

# The retail install (client.exe, game data); read-only, never used as a working directory
RETAIL_INSTALL = r'C:\Program Files (x86)\EA GAMES\Earth & Beyond'
RETAIL_CLIENT = os.path.join(RETAIL_INSTALL, 'release', 'client.exe')


def main():
    for name, val in sorted(globals().items()):
        if name.isupper() and isinstance(val, str):
            print('%-18s %-6s %s' % (name, 'ok' if os.path.exists(val) else 'MISSING', val))


if __name__ == '__main__':
    main()
