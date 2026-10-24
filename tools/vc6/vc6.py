"""Compiles source files with the client's compiler (VC6 SP4) in the build VM.

The VM mirrors C:\\EnBDevelopment at the same path on its own disk, so include paths and
__FILE__ strings are the same on both sides. Each run packs the directories it needs (source
files only) into a tar, copies it over SSH, compiles, and brings the objects back.

    python vc6.py compile [--flags "..."] [-I dir]... [-D name]... [--sync dir]... --out DIR file.cpp...
    python vc6.py run "command"            run a command in the VM after VCVARS32
    python vc6.py check                    print compiler and linker versions (every VM)
    python vc6.py discover                 find the build VMs (SSH on the Default Switch) and save
                                           their addresses in .work/vc6_hosts.json

Several identical VMs (clones) can be used at once: each is addressed by IP (clones share a
computer name, so names are not reliable), has its own sync cache, and a thread chooses its VM
with use_host(). Without a hosts file, ENB_VC6_HOST or the first discovered VM is used.

Paths given to compile must be under C:\\EnBDevelopment. Configuration: docs/toolchain.md §1.1.
"""
import argparse
import io
import json
import os
import subprocess
import sys
import tarfile
import threading
import time

USER = 'Daniel'
HOSTS_FILE = os.path.join(os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__)))),
                          '.work', 'vc6_hosts.json')
_local = threading.local()


def hosts():
    """The build VMs, as user@ip. ENB_VC6_HOSTS (comma-separated) or ENB_VC6_HOST restricts a
    process to some of them, so two jobs can run side by side on different VMs."""
    if os.environ.get('ENB_VC6_HOSTS'):
        return [h.strip() for h in os.environ['ENB_VC6_HOSTS'].split(',') if h.strip()]
    if os.environ.get('ENB_VC6_HOST'):
        return [os.environ['ENB_VC6_HOST']]
    try:
        return json.load(open(HOSTS_FILE))
    except (OSError, ValueError):
        raise SystemExit(f'no build VMs known: run `python vc6.py discover` (or set ENB_VC6_HOST)')


def use_host(host):
    """Choose the VM this thread works with."""
    _local.host = host


def current_host():
    return getattr(_local, 'host', None) or hosts()[0]


def _tag():
    """A file-name-safe tag for this VM and thread (local temporary files)."""
    return current_host().split('@')[-1].replace('.', '_') + f'_{threading.get_ident()}'
KEY = os.path.expanduser(os.environ.get('ENB_VC6_KEY', '~/.ssh/enb_vc6'))
VCVARS = r'C:\Program Files (x86)\Microsoft Visual Studio\VC98\Bin\VCVARS32.BAT'
ROOT = r'C:\EnBDevelopment'
WORK = ROOT + r'\_vc6'          # scratch area in the VM (and the tar drop)
SOURCE_EXT = ('.c', '.cpp', '.h', '.hpp', '.inl', '.inc', '.asm', '.def', '.rc', '.tlh', '.tli')

# The flags the client's libraries were built with: Renegade's release configuration (static CRT,
# no /GX, no /G6). Proven by byte matching (docs/library-matching.md 3.5). Override with --flags.
DEFAULT_FLAGS = '/nologo /c /MT /W3 /O2 /Ob2'
DEFAULT_DEFINES = ['WIN32', '_WINDOWS', 'WIN32_LEAN_AND_MEAN', 'WINVER=0x400', 'NDEBUG']

BS = chr(92)


def ssh_args():
    return ['-i', KEY, '-o', 'BatchMode=yes', '-o', 'ConnectTimeout=10']


def ssh(command, check=True):
    p = subprocess.run(['ssh'] + ssh_args() + [current_host(), command], capture_output=True, text=True,
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
    return f"{current_host()}:{path.replace(BS, '/')}"


def under_root(path):
    full = os.path.abspath(path)
    rel = os.path.relpath(full, ROOT)
    if rel.startswith('..'):
        raise SystemExit(f'{path} is not under {ROOT}')
    return full, rel


def sync_cache_path():
    return os.path.join(os.environ.get('TEMP', '.'), 'enb_vc6_synccache_' + current_host().split('@')[-1] + '.json')


def sync(dirs):
    """Send the source files of each directory (not recursive) to the VM.

    Only files changed since the last sync are sent (a local cache of size and mtime per file),
    and files deleted locally are deleted in the VM too. Delete the cache file to force a full
    sync (for example after reverting the VM to a checkpoint).
    """
    try:
        cache = json.load(open(sync_cache_path()))
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
        local = os.path.join(os.environ.get('TEMP', '.'), f'enb_vc6_sync_{_tag()}.tar')
        open(local, 'wb').write(buf.getvalue())
        scp(local, remote(WORK + r'\sync.tar'))
        ssh(f'cmd /c tar -xf "{WORK}\\sync.tar" -C "{ROOT}" && del "{WORK}\\sync.tar"')
    for i in range(0, len(removed), 40):
        files = ' '.join('"' + ROOT + BS + k.replace('/', BS) + '"' for k in removed[i:i + 40])
        ssh(f'cmd /c del /q {files} 2>nul', check=False)
    json.dump(cache, open(sync_cache_path(), 'w'))
    return count


_work_ready = set()


def ensure_work():
    if current_host() not in _work_ready:
        ssh(f'cmd /c if not exist "{WORK}" mkdir "{WORK}"')
        _work_ready.add(current_host())


def run_bat(lines):
    """Run batch lines in the VM after VCVARS32; returns the CompletedProcess."""
    ensure_work()
    bat = '@echo off\r\ncall "' + VCVARS + '" >nul\r\n' + '\r\n'.join(lines) + '\r\n'
    name = f'run_{_tag()}.bat'
    local = os.path.join(os.environ.get('TEMP', '.'), 'enb_vc6_' + name)
    open(local, 'w', newline='').write(bat)
    scp(local, remote(WORK + '\\' + name))
    return ssh(f'cmd /c "{WORK}\\{name}"', check=False)


def compile_sources(sources, includes=(), defines=None, flags=DEFAULT_FLAGS, out='.', extra_sync=()):
    """Compile sources in the VM; objects land in `out`. Returns (returncode, compiler output)."""
    defines = DEFAULT_DEFINES if defines is None else defines
    srcs = [under_root(s) for s in sources]
    incs = [under_root(i) for i in includes]
    # extra_sync: directories reached only through relative #include paths (not on the -I list)
    dirs = [os.path.dirname(f) for f, _ in srcs] + [f for f, _ in incs] + [under_root(d)[0] for d in extra_sync]
    sync(dirs)
    rc, text = _compile(srcs, incs, defines, flags, out)
    if 'C1083' in text and _missing_local_file(text):
        # A file the VM should have is missing (the VM's copy and the sync cache disagree):
        # forget the cache and send everything again, once.
        reset_sync_cache()
        sync(dirs)
        rc, text = _compile(srcs, incs, defines, flags, out)
    return rc, text


def reset_sync_cache():
    try:
        os.remove(sync_cache_path())
    except OSError:
        pass


def _missing_local_file(text):
    """True if a C1083 error names a file that exists on this side under ROOT."""
    import re
    for m in re.finditer(r"Cannot open (?:include|source) file: '([^']+)'", text):
        name = m.group(1)
        if os.path.isabs(name) and os.path.exists(name):
            return True
        if not os.path.isabs(name):
            return True   # relative names: can't tell cheaply, assume the VM is out of date
    return False


def _compile(srcs, incs, defines, flags, out):
    stamp = time.strftime('%Y%m%d%H%M%S') + f'_{os.getpid()}_{int(time.time() * 1000) % 1000}'
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
    sub.add_parser('discover')
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
        for h in hosts():
            use_host(h)
            p = run_bat(['cl 2>&1 | findstr /i version', 'link 2>&1 | findstr /i version'])
            print(h, ' | '.join(l.strip() for l in (p.stdout + p.stderr).splitlines() if l.strip()))
    if a.cmd == 'discover':
        discover()


def discover(network='192.168.192.0/20'):
    """Find the build VMs: SSH on the Default Switch network that accepts our key and has VC6."""
    import concurrent.futures as cf
    import ipaddress
    import socket

    def probe(ip):
        s = socket.socket()
        s.settimeout(0.5)
        try:
            s.connect((str(ip), 22))
            return str(ip)
        except OSError:
            return None
        finally:
            s.close()
    with cf.ThreadPoolExecutor(512) as ex:
        found = [r for r in ex.map(probe, ipaddress.ip_network(network).hosts()) if r]
    good = []
    for ip in found:
        p = subprocess.run(['ssh'] + ssh_args() + ['-o', 'StrictHostKeyChecking=accept-new', f'{USER}@{ip}',
                           f'cmd /c if exist "{VCVARS}" echo vc6'], capture_output=True, text=True)
        if 'vc6' in p.stdout:
            good.append(f'{USER}@{ip}')
    os.makedirs(os.path.dirname(HOSTS_FILE), exist_ok=True)
    json.dump(good, open(HOSTS_FILE, 'w'), indent=1)
    print(f'{len(good)} build VMs: {", ".join(good)} -> {HOSTS_FILE}')


if __name__ == '__main__':
    main()
