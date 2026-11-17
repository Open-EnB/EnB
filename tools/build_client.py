"""Builds the reconstructed client from a fresh clone, one step after another (docs/build-your-own.md).

    python tools/build_client.py [--skip-vm] [--no-standin] [--pool K] [--yes]

Each step is skipped when its result is already there, so the script can be run again after a
failure, or after `git pull` to rebuild:

1. checks: the clone's place (C:\\EnBDevelopment\\EnB, which the VM mirrors), Python, git, ssh;
2. the build VM (docs/setup-vc6.md): when none is set up (.work/vc6_config.json and the pool's base
   disk), tools/vc6/setup/New-BuildVM.ps1, 30-60 minutes, unattended (asks first unless --yes);
3. the SDKs (docs/toolchain.md §2): DirectX 8 and RAD's headers cloned to C:\\EnBDevelopment\\SDK
   from the repositories the project used, at their commits, every file checked against its
   SHA-256; then tools/vc6/sdk_patch.py's include_enb;
4. the import libraries: Miles' (mkmss32.py lib, from the committed mss32.def) and EA.Com's login
   component's (mkauthlogin.py), in the VM;
5. the workspace (tools/vc6/dsp.py) built by VC6's IDE in the VM (tools/vc6/msdev.py): every
   library, then `main`, the faithful exe. Its link lists the functions not rebuilt yet as
   unresolved: that list is the reconstruction's open work, and main.exe is not written until it
   is empty;
6. the stand-in build (`clientpoc`, unless --no-standin): main's objects and libraries with a
   generated stand-in for each unresolved function, a runnable exe;
7. the results copied to build/client/: client.exe (the stand-in build), its map and the logs.
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

# docs/toolchain.md §2: the copies the project used, and the files the build reads from them
SDK_REPOS = {
    'directx8': ('https://github.com/edgeforce/directx8.git', 'e6fa10f'),
    'radtools': ('https://github.com/edgeforce/radtools.git', 'f42d146'),
}
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


def run(args, **kw):
    print('   $', ' '.join(args), flush=True)
    return subprocess.run(args, cwd=REPO, **kw)


def py(script, *args, log=None):
    """Run one of the project's Python tools; its output to a log under build/client/logs."""
    cmd = [sys.executable, os.path.join(REPO, script)] + list(args)
    if not log:
        r = run(cmd)
        return r.returncode
    os.makedirs(os.path.dirname(log), exist_ok=True)
    print('   $', ' '.join(cmd[1:]), '>', os.path.relpath(log, REPO), flush=True)
    with open(log, 'w', encoding='utf-8') as f:
        r = subprocess.run(cmd, cwd=REPO, stdout=f, stderr=subprocess.STDOUT)
    return r.returncode


def fail(text):
    print('\nSTOPPED: ' + text, flush=True)
    sys.exit(1)


def sha256(path):
    h = hashlib.sha256()
    with open(path, 'rb') as f:
        for b in iter(lambda: f.read(1 << 20), b''):
            h.update(b)
    return h.hexdigest()


def check_host():
    if os.path.normcase(REPO) != os.path.normcase(os.path.join(ROOT, 'EnB')):
        fail(f'the clone must be at {ROOT}\\EnB (it is {REPO}): the build VM mirrors {ROOT} at the same '
             'path, so include paths and the client\'s __FILE__ strings match (docs/setup-vc6.md §1)')
    if sys.version_info < (3, 10):
        fail('Python 3.10 or later is needed')
    for tool in ('git', 'ssh', 'scp'):
        if not shutil.which(tool):
            fail(f'{tool} is not on PATH (docs/build-your-own.md "What you need")')
    print('   clone, Python, git and OpenSSH found')


def vm_ready():
    import vc6config
    cfg = vc6config.load()
    base = os.path.join(cfg['pool_dir'], 'base-vc6.vhdx')
    return os.path.exists(vc6config.CONFIG_FILE) and os.path.exists(base), base


def ensure_vm(yes):
    ok, base = vm_ready()
    if ok:
        print(f'   the build VM is set up (pool base {base})')
        return
    if not shutil.which('7z') and not os.path.exists(r'C:\Program Files\7-Zip\7z.exe'):
        fail('7-Zip is needed to unpack the VC6 media (https://www.7-zip.org/)')
    print('   no build VM yet: tools/vc6/setup/New-BuildVM.ps1 makes one (30-60 minutes, unattended; it downloads\n'
          '   Visual C++ 6.0 SP4 from the Internet Archive and a Windows 10 evaluation image from Microsoft,\n'
          '   about 6 GB, checking every download against its published hash; docs/setup-vc6.md)')
    if not yes and input('   make it now? [y/N] ').strip().lower() != 'y':
        fail('no build VM (run again with --yes, or follow docs/setup-vc6.md)')
    r = run(['powershell', '-NoProfile', '-ExecutionPolicy', 'Bypass', '-File',
             os.path.join(VC6_DIR, 'setup', 'New-BuildVM.ps1')])
    if r.returncode or not vm_ready()[0]:
        fail('New-BuildVM.ps1 did not finish (its message above; docs/setup-vc6.md §6)')


def ensure_sdks():
    for name, (url, commit) in SDK_REPOS.items():
        d = os.path.join(SDK, name)
        if not os.path.isdir(d):
            os.makedirs(SDK, exist_ok=True)
            # autocrlf off: the hashes are of the files as committed
            if run(['git', 'clone', '-q', '-c', 'core.autocrlf=false', url, d]).returncode:
                fail(f'could not clone {url}: put your own copy of the SDK in {d} (docs/toolchain.md §2)')
            run(['git', '-C', d, 'checkout', '-q', commit])
    bad = [f for f, h in SDK_HASHES.items()
           if not os.path.exists(os.path.join(SDK, f)) or sha256(os.path.join(SDK, f)) != h]
    if bad:
        fail('these SDK files are missing or differ from the copies the project used (docs/toolchain.md §2): '
             + ', '.join(bad))
    print(f'   {len(SDK_HASHES)} SDK files match their hashes')
    if not os.path.isdir(os.path.join(SDK, 'radtools', 'include_enb')):
        if py('tools/vc6/sdk_patch.py'):
            fail('sdk_patch.py failed')


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
    ap.add_argument('--skip-vm', action='store_true', help='do not make a build VM (ENB_VC6_HOSTS names one)')
    ap.add_argument('--no-standin', action='store_true', help='build the libraries and main only')
    ap.add_argument('--pool', type=int, default=1, metavar='K', help='pool VMs to lease (default 1)')
    ap.add_argument('--yes', action='store_true', help='make the build VM without asking')
    a = ap.parse_args()
    logs = os.path.join(OUT, 'logs')
    os.makedirs(logs, exist_ok=True)

    step(1, 'checks')
    check_host()
    step(2, 'the build VM')
    if a.skip_vm or os.environ.get('ENB_VC6_HOSTS'):
        print('   using ENB_VC6_HOSTS' if os.environ.get('ENB_VC6_HOSTS') else '   skipped')
    else:
        ensure_vm(a.yes)
        lease(a.pool)
    step(3, 'the SDKs')
    ensure_sdks()
    step(4, 'the import libraries')
    ensure_import_libs(logs)

    import msdev
    step(5, 'the workspace, built by VC6 (every library, then main)')
    rc, text = msdev.make('main', out=os.path.join(OUT, 'msdev'))
    if 'error C' in text or 'fatal error C' in text:
        fail('a compile error (build/client/msdev/main/msdev.log)')
    n = unresolved_count(text)
    exe = os.path.join(OUT, 'msdev', 'main', 'client.exe')
    if rc == 0 and os.path.exists(exe):
        print('   main linked: client.exe is the reconstruction alone')
    else:
        print(f'   main compiled; its link leaves {n} symbols unresolved (not rebuilt yet: the open work)')
    if not a.no_standin:
        step(6, 'the stand-in build (clientpoc)')
        rc, text = msdev.make(msdev.dsp.STANDIN, out=os.path.join(OUT, 'msdev'))
        src = os.path.join(OUT, 'msdev', msdev.dsp.STANDIN)
        exe = os.path.join(src, msdev.dsp.STANDIN + '.exe')
        if not os.path.exists(exe):
            fail('the stand-in build did not link (build/client/msdev/clientpoc/msdev.log)')
        step(7, 'results')
        shutil.copy2(exe, os.path.join(OUT, 'client.exe'))
        shutil.copy2(exe[:-4] + '.map', os.path.join(OUT, 'client.map'))
        pass1 = os.path.join(src, 'link_pass1.log')
        gen = unresolved_count(open(pass1, encoding='latin1').read()) if os.path.exists(pass1) else 0
        print(f'   {os.path.join(OUT, "client.exe")} (stand-in build: {gen} generated stand-ins, '
              f'the rest of main\'s {n} by hand in programs\\clientpoc and stubs\\)')
    json.dump({'unresolved': n}, open(os.path.join(OUT, 'summary.json'), 'w'), indent=1)
    print('\ndone. Running it: docs/build-your-own.md "Running what you built"')


if __name__ == '__main__':
    main()
