"""Compiles source files with the client's compiler (VC6 SP4) in the build VM.

The VM mirrors C:\\EnBDevelopment at the same path on its own disk, so include paths and
__FILE__ strings are the same on both sides. Each run packs the directories it needs (source
files only) into a tar, copies it over SSH, compiles, and brings the objects back.

    python vc6.py compile [--flags "..."] [-I dir]... [-D name]... [--sync dir]... --out DIR file.cpp...
    python vc6.py run "command"            run a command in the VM after VCVARS32
    python vc6.py check                    print compiler and linker versions

Paths given to compile must be under C:\\EnBDevelopment. Configuration: docs/toolchain.md §1.1.
"""
import argparse
import io
import json
import os
import subprocess
import sys
import tarfile
import time

HOST = os.environ.get('ENB_VC6_HOST', 'Daniel@DESKTOP-1J2STKG.mshome.net')
KEY = os.path.expanduser(os.environ.get('ENB_VC6_KEY', '~/.ssh/enb_vc6'))
VCVARS = r'C:\Program Files (x86)\Microsoft Visual Studio\VC98\Bin\VCVARS32.BAT'
ROOT = r'C:\EnBDevelopment'
WORK = ROOT + r'\_vc6'          # scratch area in the VM (and the tar drop)
SOURCE_EXT = ('.c', '.cpp', '.h', '.hpp', '.inl', '.inc', '.asm', '.def', '.rc')

# The flags the client's libraries were built with: Renegade's release configuration (static CRT,
# no /GX, no /G6). Proven by byte matching (docs/library-matching.md 3.5). Override with --flags.
DEFAULT_FLAGS = '/nologo /c /MT /W3 /O2 /Ob2'
DEFAULT_DEFINES = ['WIN32', '_WINDOWS', 'WIN32_LEAN_AND_MEAN', 'WINVER=0x400', 'NDEBUG']

BS = chr(92)


def ssh_args():
    return ['-i', KEY, '-o', 'BatchMode=yes', '-o', 'ConnectTimeout=10']


def ssh(command, check=True):
    p = subprocess.run(['ssh'] + ssh_args() + [HOST, command], capture_output=True, text=True,
                       encoding='latin1')
    if check and p.returncode != 0:
        sys.stderr.write(p.stdout + p.stderr)
        raise SystemExit(f'ssh failed ({p.returncode}): {command[:120]}')
    return p


def scp(src, dst):
    p = subprocess.run(['scp', '-q'] + ssh_args() + [src, dst], capture_output=True, text=True)
    if p.returncode != 0:
        raise SystemExit(f'scp failed: {p.stderr.strip()}')


def remote(path):
    """scp target for a Windows path in the VM."""
    return f"{HOST}:{path.replace(BS, '/')}"


def under_root(path):
    full = os.path.abspath(path)
    rel = os.path.relpath(full, ROOT)
    if rel.startswith('..'):
        raise SystemExit(f'{path} is not under {ROOT}')
    return full, rel


SYNC_CACHE = os.path.join(os.environ.get('TEMP', '.'), 'enb_vc6_synccache.json')


def sync(dirs):
    """Send the source files of each directory (not recursive) to the VM.

    Only files changed since the last sync are sent (a local cache of size and mtime per file),
    and files deleted locally are deleted in the VM too. Delete the cache file to force a full
    sync (for example after reverting the VM to a checkpoint).
    """
    try:
        cache = json.load(open(SYNC_CACHE))
    except (OSError, ValueError):
        cache = {}
    buf = io.BytesIO()
    count = 0
    removed = []
    with tarfile.open(fileobj=buf, mode='w') as tar:
        for d in sorted(set(dirs)):
            full, rel = under_root(d)
            present = set()
            for f in os.listdir(full):
                p = os.path.join(full, f)
                if os.path.isfile(p) and f.lower().endswith(SOURCE_EXT):
                    key = os.path.join(rel, f).replace(BS, '/')
                    present.add(key)
                    st = os.stat(p)
                    stamp = [st.st_size, st.st_mtime_ns]
                    if cache.get(key) != stamp:
                        tar.add(p, arcname=key)
                        cache[key] = stamp
                        count += 1
            prefix = rel.replace(BS, '/') + '/'
            present_low = {k.lower() for k in present}
            for key in [k for k in cache if k.startswith(prefix) and '/' not in k[len(prefix):]]:
                if key not in present:
                    if key.lower() in present_low:
                        del cache[key]   # renamed by case only: the VM's file is the new one
                        continue
                    removed.append(key)
                    del cache[key]
    ensure_work()
    if count:
        local = os.path.join(os.environ.get('TEMP', '.'), 'enb_vc6_sync.tar')
        open(local, 'wb').write(buf.getvalue())
        scp(local, remote(WORK + r'\sync.tar'))
        ssh(f'cmd /c tar -xf "{WORK}\\sync.tar" -C "{ROOT}" && del "{WORK}\\sync.tar"')
    for i in range(0, len(removed), 40):
        files = ' '.join('"' + ROOT + BS + k.replace('/', BS) + '"' for k in removed[i:i + 40])
        ssh(f'cmd /c del /q {files} 2>nul', check=False)
    json.dump(cache, open(SYNC_CACHE, 'w'))
    return count


_work_ready = False


def ensure_work():
    global _work_ready
    if not _work_ready:
        ssh(f'cmd /c if not exist "{WORK}" mkdir "{WORK}"')
        _work_ready = True


def run_bat(lines):
    """Run batch lines in the VM after VCVARS32; returns the CompletedProcess."""
    ensure_work()
    bat = '@echo off\r\ncall "' + VCVARS + '" >nul\r\n' + '\r\n'.join(lines) + '\r\n'
    local = os.path.join(os.environ.get('TEMP', '.'), 'enb_vc6_run.bat')
    open(local, 'w', newline='').write(bat)
    scp(local, remote(WORK + r'\run.bat'))
    return ssh(f'cmd /c "{WORK}\\run.bat"', check=False)


def compile_sources(sources, includes=(), defines=None, flags=DEFAULT_FLAGS, out='.', extra_sync=()):
    """Compile sources in the VM; objects land in `out`. Returns (returncode, compiler output)."""
    defines = DEFAULT_DEFINES if defines is None else defines
    srcs = [under_root(s) for s in sources]
    incs = [under_root(i) for i in includes]
    # extra_sync: directories reached only through relative #include paths (not on the -I list)
    sync([os.path.dirname(f) for f, _ in srcs] + [f for f, _ in incs] + [under_root(d)[0] for d in extra_sync])
    stamp = time.strftime('%Y%m%d%H%M%S')
    objdir = f'{WORK}\\obj_{stamp}'
    cmd = [flags] + [f'/I "{f}"' for f, _ in incs] + [f'/D "{d}"' for d in defines]
    cmd += [f'/Fo"{objdir}\\\\"'] + [f'"{f}"' for f, _ in srcs]
    p = run_bat([f'mkdir "{objdir}"', 'cl ' + ' '.join(cmd),
                 f'cd /d "{objdir}" && tar -cf ..\\obj_{stamp}.tar *.obj 2>nul'])
    os.makedirs(out, exist_ok=True)
    local = os.path.join(out, 'objs.tar')
    try:
        scp(remote(f'{WORK}\\obj_{stamp}.tar'), local)
        with tarfile.open(local) as tar:
            tar.extractall(out)
        os.remove(local)
    except SystemExit:
        pass  # nothing compiled
    ssh(f'cmd /c rmdir /s /q "{objdir}" & del "{WORK}\\obj_{stamp}.tar"', check=False)
    return p.returncode, p.stdout + p.stderr


def main():
    ap = argparse.ArgumentParser()
    sub = ap.add_subparsers(dest='cmd', required=True)
    c = sub.add_parser('compile')
    c.add_argument('sources', nargs='+')
    c.add_argument('-I', dest='includes', action='append', default=[])
    c.add_argument('-D', dest='defines', action='append')
    c.add_argument('--flags', default=DEFAULT_FLAGS)
    c.add_argument('--out', required=True)
    c.add_argument('--sync', action='append', default=[],
                   help='extra directory to copy (reached by a relative #include)')
    r = sub.add_parser('run')
    r.add_argument('command')
    sub.add_parser('check')
    a = ap.parse_args()
    if a.cmd == 'compile':
        rc, text = compile_sources(a.sources, a.includes, a.defines, a.flags, a.out, a.sync)
        print(text)
        sys.exit(rc)
    if a.cmd == 'run':
        p = run_bat([a.command])
        print(p.stdout + p.stderr)
        sys.exit(p.returncode)
    if a.cmd == 'check':
        p = run_bat(['cl 2>&1 | findstr /i version', 'link 2>&1 | findstr /i version'])
        print(p.stdout + p.stderr)


if __name__ == '__main__':
    main()
