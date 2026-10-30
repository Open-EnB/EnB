"""A pool of VC6 build VMs in Hyper-V: differencing clones of a read-only base disk, created on
demand, leased to agents, stopped when idle.

    python pool.py list                          pool VMs: state, IP, lease, disk size
    python pool.py create [N]                    N new clones (next free numbers), left stopped
    python pool.py lease --agent NAME [--count K] [--holder-pid PID]
                                                 lease K VMs (free ones first, create if short),
                                                 start them, wait for SSH and cl, print
                                                 ENB_VC6_HOSTS=Daniel@ip,...
    python pool.py release --agent NAME | VM...  end leases (VMs keep running until stop-idle)
    python pool.py stop-idle [--minutes M]       shut down unleased VMs idle for M minutes (10)
    python pool.py reclaim [--max-age H] [--prune [--keep K]]
                                                 release orphaned leases, stop idle free VMs,
                                                 report unsafe ones, optionally destroy spare
                                                 stopped ones (also run by lease and create)
    python pool.py destroy VM... | --all-free    delete unleased pool VMs and their disks

A lease records its holder: a long-lived process (--holder-pid, else $ENB_HOLDER_PID, else
$CLAUDE_PID: the agent session's claude.exe; never the short-lived `python pool.py` itself) with
its start time, the machine, $CLAUDE_CODE_SESSION_ID and the lease time. A lease is orphaned when
that process is gone (or its pid now names a process started later) or the lease is older than
--max-age hours (8).

Use the printed line as the environment of vc6.py / importer.py:
    ENB_VC6_HOSTS=Daniel@172.x.x.x python tools/vc6/vc6.py check

Pool VMs are named enb-vc6-NN, their disks are C:\\EnBDevelopment\\Tools\\VMPool\\enb-vc6-NN.vhdx
(differencing, parent base-vc6.vhdx, which is never attached or written). Hardware copies the
master VM `Win10` (read only). Every path that starts, stops or deletes checks the name prefix
and that the VM's only disk is a differencing disk in the pool directory whose parent is the
base. VMs outside the pool are never touched. State (leases, last use) is .work/pool/leases.json,
guarded by .work/pool/leases.lock. Nothing here changes vc6.py's defaults or .work/vc6_hosts.json.
Configuration: docs/toolchain.md §1.3.
"""
import argparse
import base64
import ctypes
import json
import os
import subprocess
import sys
import time

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(os.path.dirname(HERE))
sys.path.insert(0, HERE)
import vc6  # noqa: E402

from vc6config import CFG  # noqa: E402

POOL_DIR = CFG['pool_dir']
BASE = os.path.join(POOL_DIR, 'base-vc6.vhdx')
PREFIX = CFG['prefix']
TEMPLATE_VM = CFG['template_vm']  # hardware settings are copied from it (read only)
SWITCH = CFG['switch']
# a second pool (another prefix, e.g. a setup test) keeps its own leases
STATE_DIR = os.path.join(REPO, '.work', 'pool' if PREFIX == 'enb-vc6-' else 'pool-' + PREFIX.strip('-'))
STATE = os.path.join(STATE_DIR, 'leases.json')
LOCK = os.path.join(STATE_DIR, 'leases.lock')
LOCK_STALE = 900                # seconds; a live holder may stop or create several VMs
DEFAULT_MAX = CFG['max_vms']    # 7 on a 32 GB host; dynamic memory, ~1.1 GB steady per VM (user, 2026-10-27)
RESERVE_GB = CFG['reserve_gb']  # host memory that must stay free after a clone's startup memory
STARTUP_GB = 2                  # startup memory of a clone; Win10's 4 GB failed clone starts on a loaded host
MAX_AGE = 8                     # hours after which any lease counts as orphaned
KEEP = 2                        # free stopped VMs `--prune` keeps
IDLE_MIN = 10                   # minutes a free running VM may idle before reclaim stops it


# ---------------------------------------------------------------- PowerShell

def ps(script, check=True):
    """Run a PowerShell script (Hyper-V cmdlets); returns stdout. Encoded, so no quoting issues."""
    enc = base64.b64encode(("$ErrorActionPreference='Stop'\n" + script).encode('utf-16-le')).decode()
    p = subprocess.run(['powershell', '-NoProfile', '-NonInteractive', '-EncodedCommand', enc],
                       capture_output=True, text=True, encoding='utf-8', errors='replace')
    if check and p.returncode != 0:
        raise SystemExit(f'PowerShell failed: {p.stderr.strip() or p.stdout.strip()}')
    return p.stdout


def ps_json(script):
    out = ps(script).strip()
    return json.loads(out) if out else None


def q(s):
    """A PowerShell single-quoted string."""
    return "'" + str(s).replace("'", "''") + "'"


def pool_vms():
    """Every Hyper-V VM whose name has the pool prefix, with disks and addresses."""
    vms = ps_json(r"""
$r = @(Get-VM -Name '%s*' -ErrorAction SilentlyContinue | ForEach-Object {
  $vm = $_
  [pscustomobject]@{
    Name = $vm.Name; State = "$($vm.State)"; Uptime = [int]$vm.Uptime.TotalSeconds
    Disks = @(Get-VMHardDiskDrive -VM $vm | ForEach-Object {
      $h = Get-VHD -Path $_.Path
      [pscustomobject]@{ Path = $_.Path; Type = "$($h.VhdType)"; Parent = $h.ParentPath; Size = $h.FileSize } })
    Snapshots = @(Get-VMSnapshot -VM $vm).Count
    IPs = @((Get-VMNetworkAdapter -VM $vm).IPAddresses)
  } })
ConvertTo-Json -InputObject $r -Depth 5 -Compress
""" % PREFIX) or []
    # the template may share the prefix (enb-vc6-template): it is never a pool VM
    return sorted([v for v in vms if v['Name'].startswith(PREFIX) and v['Name'] != TEMPLATE_VM], key=lambda v: v['Name'])


def same(a, b):
    return os.path.normcase(os.path.abspath(a or '')) == os.path.normcase(os.path.abspath(b or ''))


def safety(vm):
    """Why this VM must not be touched, or None if it is one of ours."""
    if not vm['Name'].startswith(PREFIX):
        return 'name lacks the pool prefix'
    if vm.get('Snapshots'):
        return 'it has checkpoints'
    if len(vm['Disks']) != 1:
        return f'{len(vm["Disks"])} disks (expected 1)'
    d = vm['Disks'][0]
    if not same(os.path.dirname(d['Path']), POOL_DIR):
        return f'disk {d["Path"]} is outside {POOL_DIR}'
    if same(d['Path'], BASE):
        return 'its disk is the base disk'
    if d['Type'] != 'Differencing' or not same(d['Parent'], BASE):
        return f'disk {d["Path"]} is not a differencing child of {BASE}'
    return None


def checked(vm):
    why = safety(vm)
    if why:
        raise SystemExit(f'refusing to touch {vm["Name"]}: {why}')
    return vm


def ipv4(vm):
    for ip in vm.get('IPs') or []:
        if '.' in ip and not ip.startswith('169.254.'):
            return ip
    return None


def get_vm(name):
    for v in pool_vms():
        if v['Name'] == name:
            return v
    raise SystemExit(f'no pool VM named {name}')


# ---------------------------------------------------------------- state and lock

def _pid_alive(pid):
    """importer.py's test (d3d2036): os.kill(pid, 0) would terminate the process on Windows."""
    k = ctypes.windll.kernel32
    h = k.OpenProcess(0x1000, False, int(pid))   # PROCESS_QUERY_LIMITED_INFORMATION
    if not h:
        return ctypes.GetLastError() == 5         # access denied: it exists
    code = ctypes.c_ulong()
    ok = k.GetExitCodeProcess(h, ctypes.byref(code))
    k.CloseHandle(h)
    return not ok or code.value == 259            # STILL_ACTIVE


def _pid_started(pid):
    """The process's creation time (FILETIME ticks), or None if it can't be read."""
    k = ctypes.windll.kernel32
    h = k.OpenProcess(0x1000, False, int(pid))
    if not h:
        return None
    t = [ctypes.c_ulonglong() for _ in range(4)]
    ok = k.GetProcessTimes(h, *[ctypes.byref(x) for x in t])
    k.CloseHandle(h)
    return t[0].value if ok else None


def holder(pid_arg):
    """What a new lease records about its holder (see the module docstring)."""
    import socket
    src, pid = None, None
    for s, v in (('--holder-pid', pid_arg), ('ENB_HOLDER_PID', os.environ.get('ENB_HOLDER_PID')),
                 ('CLAUDE_PID', os.environ.get('CLAUDE_PID'))):
        if v:
            src, pid = s, int(v)
            break
    if pid is not None and not _pid_alive(pid):
        raise SystemExit(f'holder pid {pid} (from {src}) is not running')
    if pid is None:
        print('pool: no --holder-pid, ENB_HOLDER_PID or CLAUDE_PID: this lease expires only by age',
              file=sys.stderr)
    return {'holder_pid': pid, 'holder_src': src, 'holder_start': _pid_started(pid) if pid else None,
            'machine': socket.gethostname(), 'session': os.environ.get('CLAUDE_CODE_SESSION_ID'),
            'lease_pid': os.getpid(), 'lease_time': time.time()}


def orphaned(s, max_age):
    """Why lease s has no holder any more, or None."""
    import socket
    if not s.get('agent'):
        return None
    t = s.get('lease_time')
    if t is None and s.get('since'):           # leases from before holders were recorded
        t = time.time() - ago(s['since'])
    if t is not None and time.time() - t > max_age * 3600:
        return f'older than {max_age:g} h'
    pid = s.get('holder_pid')
    if pid and s.get('machine') == socket.gethostname():
        if not _pid_alive(pid):
            return f'holder pid {pid} is gone'
        st = _pid_started(pid)
        if s.get('holder_start') and st and st != s['holder_start']:
            return f'holder pid {pid} now names another process'
    return None


class Lock:
    """leases.json's lock: a file created with O_CREAT|O_EXCL holding pid and time. A lock whose
    holder is dead or that is older than LOCK_STALE seconds is taken over."""

    def __enter__(self):
        os.makedirs(STATE_DIR, exist_ok=True)
        deadline = time.time() + LOCK_STALE
        while True:
            try:
                fd = os.open(LOCK, os.O_CREAT | os.O_EXCL | os.O_WRONLY)
                os.write(fd, json.dumps({'pid': os.getpid(), 'time': time.time()}).encode())
                os.close(fd)
                return self
            except FileExistsError:
                try:
                    info = json.load(open(LOCK))
                    stale = not _pid_alive(int(info['pid'])) or time.time() - info['time'] > LOCK_STALE
                except (OSError, ValueError, KeyError, TypeError):
                    stale = time.time() - _mtime(LOCK) > LOCK_STALE
                if stale:
                    try:
                        os.remove(LOCK)
                    except OSError:
                        pass
                    continue
                if time.time() > deadline:
                    raise SystemExit(f'could not take {LOCK}')
                time.sleep(0.2)

    def __exit__(self, *exc):
        try:
            os.remove(LOCK)
        except OSError:
            pass


def _mtime(path):
    try:
        return os.path.getmtime(path)
    except OSError:
        return time.time()


def load():
    try:
        return json.load(open(STATE))
    except (OSError, ValueError):
        return {}


def save(state):
    tmp = STATE + '.tmp'
    json.dump(state, open(tmp, 'w'), indent=1, sort_keys=True)
    os.replace(tmp, STATE)


def now():
    return time.strftime('%Y-%m-%d %H:%M:%S')


def ago(stamp):
    return time.time() - time.mktime(time.strptime(stamp, '%Y-%m-%d %H:%M:%S'))


# ---------------------------------------------------------------- Hyper-V actions

def create(count, max_vms):
    """New stopped clones with the next free numbers. Call with the lock held."""
    if not os.path.isfile(BASE):
        raise SystemExit(f'base disk {BASE} missing')
    have = {v['Name'] for v in pool_vms()}
    if len(have) + count > max_vms:
        raise SystemExit(f'pool cap: {len(have)} VMs exist, {count} more would exceed --max {max_vms}')
    made, n = [], 1
    while len(made) < count:
        name = f'{PREFIX}{n:02d}'
        n += 1
        disk = os.path.join(POOL_DIR, name + '.vhdx')
        if name in have or os.path.exists(disk) or os.path.exists(os.path.join(POOL_DIR, name)):
            continue
        ps(r"""
$t = Get-VM -Name %(tpl)s
$mem = Get-VMMemory -VM $t
$fw = Get-VMFirmware -VM $t
New-VHD -Path %(disk)s -ParentPath %(base)s -Differencing | Out-Null
try {
  $vm = New-VM -Name %(name)s -Generation $t.Generation -Path %(dir)s -VHDPath %(disk)s -MemoryStartupBytes %(startup)dGB -SwitchName %(switch)s
  Set-VM -VM $vm -ProcessorCount $t.ProcessorCount -DynamicMemory -MemoryMinimumBytes $mem.Minimum -MemoryMaximumBytes $mem.Maximum -AutomaticCheckpointsEnabled $false -AutomaticStartAction Nothing -AutomaticStopAction ShutDown -CheckpointType Disabled
  Set-VMMemory -VM $vm -Buffer $mem.Buffer -Priority $mem.Priority
  if ($fw.SecureBoot -eq 'On') { Set-VMFirmware -VM $vm -EnableSecureBoot On -SecureBootTemplate $fw.SecureBootTemplate } else { Set-VMFirmware -VM $vm -EnableSecureBoot Off }
  Set-VMFirmware -VM $vm -FirstBootDevice (Get-VMHardDiskDrive -VM $vm)
  Enable-VMIntegrationService -VM $vm -Name 'Guest Service Interface'
} catch { Get-VM -Name %(name)s -ErrorAction SilentlyContinue | Remove-VM -Force; Remove-Item -LiteralPath %(disk)s -ErrorAction SilentlyContinue; throw }
""" % dict(tpl=q(TEMPLATE_VM), disk=q(disk), base=q(BASE), name=q(name), dir=q(POOL_DIR), switch=q(SWITCH), startup=STARTUP_GB))
        checked(get_vm(name))
        made.append(name)
        print(f'created {name} ({disk})')
    return made


def free_gb():
    """The host's free physical memory, in GB."""
    return float(ps('[math]::Round((Get-CimInstance Win32_OperatingSystem).FreePhysicalMemory / 1MB, 2)').strip())


def start_vm(name):
    vm = checked(get_vm(name))
    if vm['State'] != 'Running':
        free = free_gb()
        if free < STARTUP_GB + RESERVE_GB:
            raise SystemExit(f'host memory: {free:.1f} GB free, a clone needs {STARTUP_GB} GB and '
                             f'{RESERVE_GB} GB must stay free (ENB_VC6_RESERVE_GB); not starting {name}')
        ps(f'Start-VM -Name {q(name)}')


def stop_vm(name, hard=False):
    vm = checked(get_vm(name))
    if vm['State'] == 'Off':
        return
    ps(f'Stop-VM -Name {q(name)} ' + ('-TurnOff -Force' if hard else '-Force'))


def wait_ready(name, timeout=600):
    """Wait for the VM's IPv4 address, SSH with our key, and VC6's cl. Returns (ip, seconds, cl line)."""
    t0 = time.time()
    ip = None
    while time.time() - t0 < timeout:
        ip = ipv4(get_vm(name))
        if ip:
            break
        time.sleep(3)
    if not ip:
        raise SystemExit(f'{name}: no IPv4 address after {timeout} s')
    host = f'{vc6.USER}@{ip}'
    # The host key of a pool IP is accepted on first sight (pool IPs only; clones share the base's key).
    while time.time() - t0 < timeout:
        p = subprocess.run(['ssh'] + vc6.ssh_args() + ['-o', 'StrictHostKeyChecking=accept-new', host,
                           'cmd /c echo pool-ok'], capture_output=True, text=True, encoding='latin1')
        if p.returncode == 0 and 'pool-ok' in p.stdout:
            secs = time.time() - t0
            vc6.use_host(host)
            c = vc6.run_bat(['cl 2>&1 | findstr /i version'])
            if 'Version' not in c.stdout:
                raise SystemExit(f'{name} ({ip}): SSH works but cl does not run:\n{c.stdout}{c.stderr}')
            return ip, secs, c.stdout.strip()
        if 'Host key verification failed' in p.stderr or 'REMOTE HOST IDENTIFICATION' in p.stderr:
            raise SystemExit(f'{name} ({ip}): host key mismatch in known_hosts; check and fix by hand:\n{p.stderr}')
        time.sleep(3)
    raise SystemExit(f'{name} ({ip}): SSH/cl not ready after {timeout} s')


def reset_sync_cache(ip):
    """A leased VM may sit at an IP another VM used before: forget vc6.py's sync cache for it."""
    vc6.use_host(f'{vc6.USER}@{ip}')
    vc6.reset_sync_cache()


def idle_secs(v, s):
    idle = ago(s['released']) if s.get('released') else v['Uptime']
    return min(idle, v['Uptime'])


def destroy_vm(v, state):
    """Delete pool VM v (unleased, checked) and its disk. Call with the lock held."""
    n = v['Name']
    checked(v)
    disk = v['Disks'][0]['Path']
    stop_vm(n, hard=True)
    ps(f'Remove-VM -Name {q(n)} -Force')
    os.remove(disk)          # checked() proved it is a differencing child in POOL_DIR
    cfg = os.path.join(POOL_DIR, n)
    if os.path.isdir(cfg) and same(os.path.dirname(cfg), POOL_DIR):
        import shutil
        shutil.rmtree(cfg, ignore_errors=True)
    state.pop(n, None)
    print(f'destroyed {n}')


def release_orphans(state, max_age):
    """End leases whose holder is gone (orphaned()). Call with the lock held; saves."""
    for n, s in list(state.items()):
        why = orphaned(s, max_age)
        if why:
            print(f"reclaim: releasing {n} (leased by {s['agent']} since {s.get('since')}): {why}",
                  file=sys.stderr)
            state[n] = {'agent': None, 'released': now(), 'reclaimed': why}
    save(state)


def tidy(state, idle_min, keep=None, exclude=()):
    """Among free pool VMs not in exclude: report unsafe ones (never touched), stop running ones
    idle for idle_min minutes, and with keep set destroy stopped ones beyond the keep
    lowest-numbered. Free entries of VMs that no longer exist are dropped. Call with the lock
    held; saves."""
    vms = pool_vms()
    have = {v['Name'] for v in vms}
    for n in [n for n, s in state.items() if n not in have and not s.get('agent')]:
        del state[n]
    for v in vms:
        s = state.get(v['Name'], {})
        if s.get('agent'):
            continue
        why = safety(v)
        if why:
            print(f"reclaim: {v['Name']} is free but NOT SAFE, left alone: {why}", file=sys.stderr)
            continue
        if v['Name'] in exclude or v['State'] != 'Running':
            continue
        idle = idle_secs(v, s)
        if idle >= idle_min * 60:
            print(f"reclaim: stopping {v['Name']} (free, idle {idle / 60:.0f} min)", file=sys.stderr)
            stop_vm(v['Name'])
    if keep is not None:
        spare = [v for v in pool_vms() if v['State'] == 'Off' and v['Name'] not in exclude
                 and not state.get(v['Name'], {}).get('agent') and not safety(v)]
        for v in spare[keep:]:
            destroy_vm(v, state)
    save(state)


# ---------------------------------------------------------------- commands

def cmd_list(a):
    state = load()
    vms = pool_vms()
    if not vms:
        print('no pool VMs')
    for v in vms:
        s = state.get(v['Name'], {})
        lease = f"leased by {s['agent']} since {s['since']}" if s.get('agent') else 'free'
        if s.get('agent'):
            lease += f" (holder pid {s['holder_pid']})" if s.get('holder_pid') else ' (no holder pid)'
        size = sum(d['Size'] for d in v['Disks']) / 2**30
        why = safety(v)
        print(f"{v['Name']}  {v['State']:<8} {ipv4(v) or '-':<16} {lease:<48} disk {size:5.2f} GB"
              + (f'  [NOT SAFE: {why}]' if why else ''))


def cmd_create(a):
    with Lock():
        state = load()
        release_orphans(state, a.max_age)
        tidy(state, a.idle_minutes, a.keep if a.prune else None)
        spare = [v['Name'] for v in pool_vms() if not state.get(v['Name'], {}).get('agent') and not safety(v)]
        if spare:
            print(f"note: free VMs exist ({', '.join(spare)}); lease reuses them before creating", file=sys.stderr)
        create(a.n, a.max)


def cmd_reclaim(a):
    with Lock():
        state = load()
        release_orphans(state, a.max_age)
        tidy(state, a.idle_minutes, a.keep if a.prune else None)


def cmd_lease(a):
    who = holder(a.holder_pid)
    with Lock():
        state = load()
        release_orphans(state, a.max_age)
        vms = pool_vms()
        free = [v for v in vms if not state.get(v['Name'], {}).get('agent') and not safety(v)]
        # running free VMs first (no boot), then stopped ones
        free.sort(key=lambda v: (v['State'] != 'Running', v['Name']))
        names = [v['Name'] for v in free[:a.count]]
        if len(names) < a.count:
            names += create(a.count - len(names), a.max)
        for n in names:
            state[n] = dict(who, agent=a.agent, since=now())
        save(state)
        tidy(state, a.idle_minutes, a.keep if a.prune else None, exclude=names)
    try:
        for n in names:
            start_vm(n)
        hosts = []
        for n in names:
            ip, secs, cl = wait_ready(n, a.timeout)
            reset_sync_cache(ip)
            print(f'{n}: {ip} ready in {secs:.0f} s ({cl})', file=sys.stderr)
            hosts.append(f'{vc6.USER}@{ip}')
    except BaseException:
        with Lock():
            state = load()
            for n in names:
                if state.get(n, {}).get('agent') == a.agent:
                    state[n] = {'agent': None, 'released': now()}
            save(state)
        raise
    with Lock():
        state = load()
        for n, h in zip(names, hosts):
            state[n]['host'] = h
        save(state)
    print('ENB_VC6_HOSTS=' + ','.join(hosts))


def cmd_release(a):
    with Lock():
        state = load()
        done = []
        for n, s in state.items():
            if s.get('agent') and ((a.agent and s['agent'] == a.agent) or n in a.vms):
                state[n] = {'agent': None, 'released': now()}
                done.append(n)
        save(state)
    print('released ' + (', '.join(done) or 'nothing'))


def cmd_stop_idle(a):
    with Lock():
        state = load()
        for v in pool_vms():
            s = state.get(v['Name'], {})
            if v['State'] != 'Running' or s.get('agent') or safety(v):
                continue
            idle = idle_secs(v, s)
            if idle >= a.minutes * 60:
                print(f"stopping {v['Name']} (idle {idle / 60:.0f} min)")
                stop_vm(v['Name'])


def cmd_destroy(a):
    with Lock():
        state = load()
        vms = {v['Name']: v for v in pool_vms()}
        if a.all_free:
            names = [n for n in vms if not state.get(n, {}).get('agent')]
        else:
            names = a.vms
        for n in names:
            if n not in vms:
                raise SystemExit(f'no pool VM named {n}')
            if state.get(n, {}).get('agent'):
                raise SystemExit(f'{n} is leased by {state[n]["agent"]}: release it first')
            checked(vms[n])
        for n in names:
            destroy_vm(vms[n], state)
        save(state)


def main():
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    sub = ap.add_subparsers(dest='cmd', required=True)
    sub.add_parser('list')
    c = sub.add_parser('create')
    c.add_argument('n', nargs='?', type=int, default=1)
    c.add_argument('--max', type=int, default=DEFAULT_MAX)
    rc = sub.add_parser('reclaim')
    le = sub.add_parser('lease')
    le.add_argument('--agent', required=True)
    le.add_argument('--count', type=int, default=1)
    le.add_argument('--max', type=int, default=DEFAULT_MAX)
    le.add_argument('--timeout', type=int, default=600, help='seconds to wait for each VM')
    le.add_argument('--holder-pid', type=int,
                    help='long-lived process whose exit orphans the lease (default $ENB_HOLDER_PID, $CLAUDE_PID)')
    for x in (c, rc, le):
        x.add_argument('--max-age', type=float, default=MAX_AGE, help='hours after which a lease is orphaned')
        x.add_argument('--idle-minutes', type=float, default=IDLE_MIN, help='stop free VMs idle this long')
        x.add_argument('--prune', action='store_true', help='destroy free stopped VMs beyond --keep')
        x.add_argument('--keep', type=int, default=KEEP)
    r = sub.add_parser('release')
    r.add_argument('--agent')
    r.add_argument('vms', nargs='*')
    s = sub.add_parser('stop-idle')
    s.add_argument('--minutes', type=float, default=10)
    d = sub.add_parser('destroy')
    d.add_argument('vms', nargs='*')
    d.add_argument('--all-free', action='store_true')
    a = ap.parse_args()
    if a.cmd == 'release' and not (a.agent or a.vms):
        ap.error('release needs --agent or VM names')
    if a.cmd == 'destroy' and bool(a.vms) == a.all_free:
        ap.error('destroy needs VM names or --all-free (not both)')
    {'list': cmd_list, 'create': cmd_create, 'lease': cmd_lease, 'release': cmd_release,
     'stop-idle': cmd_stop_idle, 'destroy': cmd_destroy, 'reclaim': cmd_reclaim}[a.cmd](a)


if __name__ == '__main__':
    main()
