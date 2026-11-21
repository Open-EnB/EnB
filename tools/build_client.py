"""Builds the reconstructed client: the import libraries, the workspace, every library and the client.

    python tools/build_client.py --local                 inside a build VM (tools/Build-Client.ps1 runs it there)
    python tools/build_client.py [--pool K]              on the host, against pool VMs (the maintainer's set-up)

A newcomer runs tools/Build-Client.ps1, which needs no Python on the host: it makes the build VM,
asks before downloading anything, copies the tree, the SDKs and a Python into the VM, and runs
this script there with --local (docs/build-your-own.md). This script downloads nothing.

1. the SDKs (docs/toolchain.md §2) are checked against their SHA-256 (they must be in
   C:\\EnBDevelopment\\SDK already); then tools/vc6/sdk_patch.py's include_enb;
2. the import libraries: Miles' (mkmss32.py lib, from the committed mss32.def) and EA.Com's login
   component's (mkauthlogin.py);
3. the workspace (tools/vc6/dsp.py) built by VC6's IDE (tools/vc6/msdev.py): every library, then the
   client project (dsp.CLIENT_PROJECT: WinMain and every library, no stand-ins, as build.py's
   `link --program client`). Its link lists the functions not rebuilt yet as unresolved: that list is
   the reconstruction's open work, and the client writes no exe until it is empty;
4. the results in build/client/: client.exe, its map, the logs, summary.json.
"""
import argparse
import hashlib
import json
import os
import shutil
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(HERE)
VC6_DIR = os.path.join(HERE, 'vc6')
sys.path.insert(0, VC6_DIR)
ROOT = r'C:\EnBDevelopment'
SDK = os.path.join(ROOT, 'SDK')
OUT = os.path.join(REPO, 'build', 'client')

# docs/toolchain.md §2: the files the build reads from the SDKs (tools/Build-Client.ps1 has the same list)
SDK_HASHES = {
    r'directx8\include\d3d8.h': '3ca75d0512eb882de054bf8d93c1a25dc356e3681d83eaf4e15e7d46d2eb80b0',
    r'directx8\include\d3dx8.h': '4dda934ffc04ca0a7ee7551ee524c1eb9e5ec85cb345202ea0661c71feff122c',
    r'directx8\lib\d3d8.lib': '3c9f0dc543c41c4b03d380ae47591133db4718dd4bd6c0b739c97a4aa9a493dd',
    r'directx8\lib\d3dx8.lib': '32148094cebbbe9b55f7769787cdde5926ca01014e072844f5152d25072e1f44',
    r'directx8\lib\dxguid.lib': '70a74a5e9c195f73db8d6a9bd082611825d07605b67fc7bceb10171d2c194cce',
    r'radtools\include\bink.h': '25c9a43ae85386cd0d9e00fe5d1f3670814c7a6be2e7c5d8e9952a5fa8674f99',
    r'radtools\include\mss.h': '721bac19260c63445524acc8a027e25307c4ddaa8e47990a72b7109567047377',
    r'radtools\include\rad.h': '528d9fea30775f34af20c8779b44435e1d7e3ad1ad6c24e879aa81ac40719b69',
    r'radtools\lib\binkw32.lib': '4ef062e19174050cf251454c92670bca256e5be3dfee0ba6ae4aa2b54ce907d1',
}


def step(n, text):
    print(f'\n== {n}. {text}', flush=True)


def py(script, *args, log):
    """Run one of the project's Python tools, its output to a log under build/client/logs."""
    cmd = [sys.executable, os.path.join(REPO, script)] + list(args)
    os.makedirs(os.path.dirname(log), exist_ok=True)
    print('   $', ' '.join(cmd[1:]), '>', os.path.relpath(log, REPO), flush=True)
    with open(log, 'w', encoding='utf-8') as f:
        return subprocess.run(cmd, cwd=REPO, stdout=f, stderr=subprocess.STDOUT).returncode


def fail(text):
    print('\nSTOPPED: ' + text, flush=True)
    sys.exit(1)


def sha256(path):
    """An SDK file's SHA-256 as docs/toolchain.md §2 records it: a header (.h) with CRLF line ends (the
    hashes come from a Windows checkout; GitHub's archives and an autocrlf=false clone have LF)."""
    data = open(path, 'rb').read()
    if path.lower().endswith('.h'):
        data = data.replace(b'\r\n', b'\n').replace(b'\n', b'\r\n')
    return hashlib.sha256(data).hexdigest()


def check_sdks(logs):
    bad = [f for f, h in SDK_HASHES.items()
           if not os.path.exists(os.path.join(SDK, f)) or sha256(os.path.join(SDK, f)) != h]
    if bad:
        fail('these SDK files are missing or differ from the copies the project used (docs/toolchain.md §2; '
             'tools/Build-Client.ps1 offers to download them): ' + ', '.join(bad))
    print(f'   {len(SDK_HASHES)} SDK files match their hashes')
    if not os.path.isdir(os.path.join(SDK, 'radtools', 'include_enb')):
        if py('tools/vc6/sdk_patch.py', log=os.path.join(logs, 'sdk_patch.log')):
            fail('sdk_patch.py failed (build/client/logs/sdk_patch.log)')


def lease(k):
    """Lease K pool VMs for the rest of the run (released at exit)."""
    import atexit
    import vc6lock
    cm = vc6lock.pool_lease(k, os.getpid())
    cm.__enter__()
    atexit.register(cm.__exit__, None, None, None)


def ensure_import_libs(logs):
    if not os.path.exists(os.path.join(REPO, 'build', 'mss32', 'mss32.lib')):
        if py('tools/vc6/mkmss32.py', 'lib', log=os.path.join(logs, 'mkmss32.log')):
            fail('mkmss32.py lib failed (build/client/logs/mkmss32.log)')
    if not os.path.exists(os.path.join(REPO, 'build', 'authlogin', 'authlogin.lib')):
        if py('tools/vc6/mkauthlogin.py', log=os.path.join(logs, 'mkauthlogin.log')):
            fail('mkauthlogin.py failed (build/client/logs/mkauthlogin.log)')
    print('   mss32.lib and authlogin.lib ready')


def unresolved_count(text):
    import build as B
    return len({m.group(2) or m.group(3) for m in B.UNRES.finditer(text)})


def main():
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    ap.add_argument('--local', action='store_true', help='run inside the build VM (vc6.py local mode)')
    ap.add_argument('--rebuild', action='store_true', help='compile everything again')
    ap.add_argument('--pool', type=int, default=1, metavar='K', help='host mode: pool VMs to lease (default 1)')
    a = ap.parse_args()
    if a.local:
        os.environ['ENB_VC6_LOCAL'] = '1'     # before vc6.py is imported
    if os.path.normcase(REPO) != os.path.normcase(os.path.join(ROOT, 'EnB')):
        fail(f'the tree must be at {ROOT}\\EnB (it is {REPO}): include paths and the client\'s __FILE__ strings')
    logs = os.path.join(OUT, 'logs')
    os.makedirs(logs, exist_ok=True)
    if not a.local and not os.environ.get('ENB_VC6_HOSTS'):
        lease(a.pool)

    step(1, 'the SDKs')
    check_sdks(logs)
    step(2, 'the import libraries')
    ensure_import_libs(logs)

    import msdev
    step(3, 'the workspace, built by VC6 (every library, then the client)')
    project = msdev.dsp.CLIENT_PROJECT
    rc, text = msdev.make(project, a.rebuild, out=os.path.join(OUT, 'msdev'))
    if 'error C' in text:
        fail(f'a compile error (build/client/msdev/{project}/msdev.log)')
    n = unresolved_count(text)
    exe = os.path.join(OUT, 'msdev', project, 'client.exe')
    summary = {'unresolved': n}
    if rc != 0 or not os.path.exists(exe):
        json.dump(summary, open(os.path.join(OUT, 'summary.json'), 'w'), indent=1)
        fail(f'the client did not link: {n} symbols unresolved (not rebuilt yet: build/client/msdev/{project}/msdev.log)')
    step(4, 'results')
    shutil.copy2(exe, os.path.join(OUT, 'client.exe'))
    shutil.copy2(exe[:-4] + '.map', os.path.join(OUT, 'client.map'))
    print(f'   {os.path.join(OUT, "client.exe")}: the reconstruction alone')
    json.dump(summary, open(os.path.join(OUT, 'summary.json'), 'w'), indent=1)
    if not a.local:     # Build-Client.ps1 says it after copying the results back
        print('\ndone. Running it: docs/build-your-own.md "Running what you built"')


if __name__ == '__main__':
    main()
