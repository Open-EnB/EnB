"""Makes authlogin.lib, the import library of EA.Com's login component, from tools/vc6/authlogin.def (D90 9a).

    python tools/vc6/mkauthlogin.py [--out DIR]      build DIR/authlogin/authlogin.lib (default build/) in a VM

authlogin.dll exports two undecorated names, AuthLoginLib_GetAuthorize and
AuthLoginLib_GetComponentFactory (the client imports only the factory, docs/binary.md). Unlike
Miles' decorated stdcall names (mkmss32.py), undecorated names are what VC6's `lib /def` handles:
the symbol `_AuthLoginLib_GetComponentFactory` (`__imp__...` for dllimport) imports the name as
exported (name type "no prefix"). The script checks that with dumpbin. Nothing of the DLL is
compiled, and no program here may load the real DLL (EA's servers are gone; Net-7's patched copy
talks to Net-7: a test supplies its own stand-in DLL exporting the factory).
"""
import argparse
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(os.path.dirname(HERE))
sys.path.insert(0, HERE)
DEF = os.path.join(HERE, 'authlogin.def')
LIB = os.path.join('authlogin', 'authlogin.lib')     # under the build directory (build.py links it when present)


def build_lib(out):
    import vc6
    d = os.path.join(out, 'authlogin')
    os.makedirs(d, exist_ok=True)
    w = vc6.WORK + '\\mkauthlogin'
    vc6.run_bat([f'if exist "{w}" rmdir /s /q "{w}"', f'mkdir "{w}"'])
    vc6.scp(DEF, vc6.remote(w + '\\authlogin.def'))
    vc6.run_bat([
        f'cd /d "{w}"',
        'lib /nologo /def:authlogin.def /machine:ix86 /out:authlogin.lib > build.log 2>&1',
        'dumpbin /headers authlogin.lib > headers.txt 2>&1',
        'dumpbin /linkermember:1 authlogin.lib > linkermember.txt 2>&1',
    ])
    for f in ('authlogin.lib', 'build.log', 'headers.txt', 'linkermember.txt'):
        vc6.scp(vc6.remote(w + '\\' + f), os.path.join(d, f))
    vc6.ssh(f'cmd /c rmdir /s /q "{w}"', check=False)
    hdr = open(os.path.join(d, 'headers.txt'), encoding='latin1').read()
    syms = re.findall(r'Symbol name\s*:\s*(\S+)', hdr)
    names = re.findall(r'Name\s*:\s*(\S+)', hdr)
    print(f'{d}\\authlogin.lib: symbols {syms}, imported names {names}; build.log:')
    print(open(os.path.join(d, 'build.log'), encoding='latin1').read().strip()[-800:])
    want = {'_AuthLoginLib_GetAuthorize', '_AuthLoginLib_GetComponentFactory'}
    if not want <= set(syms):
        raise SystemExit('authlogin.lib lacks ' + ', '.join(sorted(want - set(syms))))


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--out', default=os.path.join(REPO, 'build'))
    build_lib(ap.parse_args().out)


if __name__ == '__main__':
    main()
