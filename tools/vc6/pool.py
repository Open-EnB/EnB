"""A pool of VC6 build VMs in Hyper-V: differencing clones of a read-only base disk, created on
demand, leased to agents, stopped when idle.

    python pool.py list                          pool VMs: state, IP, lease, disk size
    python pool.py create [N]                    N new clones (next free numbers), left stopped
    python pool.py lease --agent NAME [--count K] [--holder-pid PID] [--wait S]
                                                 lease K VMs (free ones first, create if short),
                                                 start them, wait for SSH and cl, print
                                                 ENB_VC6_HOSTS=<user>@ip,...; at the cap, wait
                                                 up to S seconds (1800; 0: exit at once) in a
                                                 first-come-first-served queue
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
    ENB_VC6_HOSTS=<user>@172.x.x.x python tools/vc6/vc6.py check

Pool VMs are named enb-vc6-NN, their disks are C:\\EnBDevelopment\\Tools\\VMPool\\enb-vc6-NN.vhdx
(differencing, parent base-vc6.vhdx, which is never attached or written). Hardware copies the
master VM `Win10` (read only). Every path that starts, stops or deletes checks the name prefix
and that the VM's only disk is a differencing disk in the pool directory whose parent is the
base. VMs outside the pool are never touched. State (leases, last use) is .work/pool/leases.json,
guarded by .work/pool/leases.lock. The lock covers only load-decide-save steps (and the Hyper-V
listing they decide on): creating, starting, stopping, destroying a VM and waiting for it run
unlocked, the VM marked `busy` (pid, what) in the state meanwhile so no one else picks or counts it
wrongly. A lease that finds the pool at its cap queues: a ticket .work/pool/queue/<seq>-<pid>.json
(pid and its start time), served strictly in order; a ticket whose process is gone is dropped. A
lease stays `pending` until its VMs are ready: if the lease process dies before that, it is orphaned.
Nothing here changes vc6.py's defaults or .work/vc6_hosts.json.
Configuration: docs/toolchain.md §1.3.
"""
import argparse
import base64
import ctypes
import json
import random
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
QUEUE = os.path.join(STATE_DIR, 'queue')
LOCK_STALE = 300                # seconds; holders only load, list VMs, decide and save (D232)
WAIT = 1800                     # seconds a lease waits in the queue at the cap (--wait)
POLL = 5                        # seconds between a queued lease's checks
PROGRESS = 60                   # seconds between a queued lease's progress lines
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

def _proc_stat(pid):
    """/proc/<pid>/stat's fields after the command name (state first), or None."""
    try:
        with open(f'/proc/{int(pid)}/stat') as f:
            return f.read().rsplit(')', 1)[1].split()
    except (OSError, IndexError):
        return None


def _pid_alive(pid):
    """importer.py's test (fa14686): os.kill(pid, 0) would terminate the process on Windows.
    Elsewhere (CI runs the mocked tests on Linux) os.kill(pid, 0), with a zombie counted as gone."""
    if os.name != 'nt':
        try:
            os.kill(int(pid), 0)
        except ProcessLookupError:
            return False
        except PermissionError:
            return True
        st = _proc_stat(pid)
        return not (st and st[0] == 'Z')
    k = ctypes.windll.kernel32
    h = k.OpenProcess(0x1000, False, int(pid))   # PROCESS_QUERY_LIMITED_INFORMATION
    if not h:
        return ctypes.GetLastError() == 5         # access denied: it exists
    code = ctypes.c_ulong()
    ok = k.GetExitCodeProcess(h, ctypes.byref(code))
    k.CloseHandle(h)
    return not ok or code.value == 259            # STILL_ACTIVE


def _pid_started(pid):
    """The process's creation time (FILETIME ticks; /proc start-time ticks off Windows), or None
    if it can't be read."""
    if os.name != 'nt':
        st = _proc_stat(pid)
        return int(st[19]) if st and len(st) > 19 else None
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
            'lease_pid': os.getpid(), 'lease_start': _pid_started(os.getpid()), 'lease_time': time.time()}


def _same_process(pid, start):
    """pid names a running process and, when start is known, the one started then."""
    if not pid or not _pid_alive(pid):
        return False
    st = _pid_started(pid)
    return not (start and st and st != start)


def busy_live(s):
    """A process is creating, stopping or destroying this VM outside the lock (mark_busy)."""
    import socket
    b = (s or {}).get('busy')
    if not b:
        return False
    if b.get('machine') != socket.gethostname():
        return True                            # can't check another machine's pid
    return _same_process(b.get('pid'), b.get('start'))


def mark_busy(state, name, what):
    import socket
    s = state.setdefault(name, {'agent': None})
    s['busy'] = {'pid': os.getpid(), 'start': _pid_started(os.getpid()), 'machine': socket.gethostname(),
                 'what': what, 'since': now()}


def unmark_busy(names, gone=()):
    """Clear this process's busy marks on names; drop the free entries of names in gone."""
    with Lock():
        state = load()
        for n in names:
            s = state.get(n)
            if s and (s.get('busy') or {}).get('pid') == os.getpid():
                del s['busy']
        for n in gone:
            if n in state and not state[n].get('agent'):
                del state[n]
        save(state)


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
    lp = s.get('lease_pid')
    if s.get('pending') and lp and s.get('machine') == socket.gethostname() \
            and not _same_process(lp, s.get('lease_start')):
        return f'lease process {lp} ended before its VMs were ready'
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
                    with open(LOCK) as fh:
                        info = json.load(fh)
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
                time.sleep(random.uniform(0.02, 0.1))   # short and jittered: queued leases poll it

    def __exit__(self, *exc):
        # A waiter reading the lock file has it open for a moment, and Windows then refuses the
        # delete (sharing violation): retry, or the lock stays behind under a live pid until
        # LOCK_STALE (one way a lease process "held" it for 15 minutes, 2026-11-01).
        for _ in range(250):
            try:
                os.remove(LOCK)
                return
            except PermissionError:
                time.sleep(0.02)
            except OSError:
                return
        print(f'pool: could not delete {LOCK}', file=sys.stderr)


def _mtime(path):
    try:
        return os.path.getmtime(path)
    except OSError:
        return time.time()


def load():
    try:
        with open(STATE) as fh:
            return json.load(fh)
    except (OSError, ValueError):
        return {}


def save(state):
    tmp = STATE + '.tmp'
    with open(tmp, 'w', newline='\n') as fh:
        json.dump(state, fh, indent=1, sort_keys=True)
    os.replace(tmp, STATE)


def now():
    return time.strftime('%Y-%m-%d %H:%M:%S')


def ago(stamp):
    return time.time() - time.mktime(time.strptime(stamp, '%Y-%m-%d %H:%M:%S'))


# ---------------------------------------------------------------- Hyper-V actions

def taken_names(state, vms):
    """Pool VMs that exist plus those a live process is creating (leased or busy, not yet listed)."""
    return {v['Name'] for v in vms} | {n for n, s in state.items() if s.get('agent') or busy_live(s)}


def plan_names(count, max_vms, state, vms):
    """The next free clone names for count new VMs, or None at the cap. Call with the lock held;
    the caller records them in the state (leased or mark_busy) before it saves and unlocks."""
    if not os.path.isfile(BASE):
        raise SystemExit(f'base disk {BASE} missing')
    taken = taken_names(state, vms)
    if len(taken) + count > max_vms:
        return None
    names, n = [], 1
    while len(names) < count:
        name = f'{PREFIX}{n:02d}'
        n += 1
        if name in taken or os.path.exists(os.path.join(POOL_DIR, name + '.vhdx')) \
                or os.path.exists(os.path.join(POOL_DIR, name)):
            continue
        names.append(name)
    return names


def cap_message(count, max_vms, state, vms):
    return f'pool cap: {len(taken_names(state, vms))} VMs exist, {count} more would exceed --max {max_vms}'


def make_clone(name):
    """Create the stopped clone `name` (planned by plan_names). Runs without the lock."""
    disk = os.path.join(POOL_DIR, name + '.vhdx')
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
    print(f'created {name} ({disk})', flush=True)


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


def destroy_vm(v):
    """Delete pool VM v (unleased, checked) and its disk. Runs without the lock: the caller marked
    it busy, and drops its state entry afterwards (unmark_busy(..., gone=))."""
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
    print(f'destroyed {n}')


def is_free(state, v):
    s = state.get(v['Name'], {})
    return not s.get('agent') and not busy_live(s) and not safety(v)


def release_orphans(state, max_age):
    """End leases whose holder is gone (orphaned()) and clear busy marks of ended processes.
    Call with the lock held; saves."""
    changed = False
    for n, s in list(state.items()):
        why = orphaned(s, max_age)
        if why:
            print(f"reclaim: releasing {n} (leased by {s['agent']} since {s.get('since')}): {why}",
                  file=sys.stderr)
            state[n] = freed(s, reclaimed=why)
            changed = True
        if state[n].get('busy') and not busy_live(state[n]):
            del state[n]['busy']
            changed = True
    if changed:
        save(state)


def run_marked(work, names, gone=()):
    """Run work() on VMs this process marked busy, then clear the marks (and drop the entries of
    gone); the lock is not held meanwhile."""
    try:
        work()
    finally:
        unmark_busy(names, gone)


def tidy(idle_min, keep=None, exclude=()):
    """Among free pool VMs not in exclude: report unsafe ones (never touched), stop running ones
    idle for idle_min minutes, and with keep set destroy stopped ones beyond the keep
    lowest-numbered. Free entries of VMs that no longer exist are dropped. Takes the lock to decide
    and mark; stops and destroys without it. Call without the lock."""
    with Lock():
        state = load()
        vms = pool_vms()
        have = {v['Name'] for v in vms}
        for n in [n for n, s in state.items() if n not in have and not s.get('agent') and not busy_live(s)]:
            del state[n]
        stop = []
        for v in vms:
            s = state.get(v['Name'], {})
            if s.get('agent') or busy_live(s):
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
                stop.append(v['Name'])
        doomed = []
        if keep is not None:
            spare = [v for v in vms if v['State'] == 'Off' and v['Name'] not in exclude and is_free(state, v)]
            doomed = spare[keep:]
        for n in stop:
            mark_busy(state, n, 'stopping')
        for v in doomed:
            mark_busy(state, v['Name'], 'destroying')
        save(state)
    if not (stop or doomed):
        return

    def work():
        for n in stop:
            stop_vm(n)
        for v in doomed:
            destroy_vm(v)
    run_marked(work, stop + [v['Name'] for v in doomed], gone=[v['Name'] for v in doomed])


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
        if busy_live(s):
            lease += f" [{s['busy']['what']} by pid {s['busy']['pid']}]"
        size = sum(d['Size'] for d in v['Disks']) / 2**30
        why = safety(v)
        print(f"{v['Name']}  {v['State']:<8} {ipv4(v) or '-':<16} {lease:<48} disk {size:5.2f} GB"
              + (f'  [NOT SAFE: {why}]' if why else ''))
    try:
        waiting = sorted(f for f in os.listdir(QUEUE) if f.endswith('.json'))
    except FileNotFoundError:
        waiting = []
    for i, f in enumerate(waiting, 1):
        try:
            with open(os.path.join(QUEUE, f)) as fh:
                t = json.load(fh)
            print(f"queue {i}: {t.get('agent')} wants {t.get('count')} (pid {t.get('pid')}, since {t.get('since')})")
        except (OSError, ValueError):
            pass


def cmd_create(a):
    with Lock():
        release_orphans(load(), a.max_age)
    tidy(a.idle_minutes, a.keep if a.prune else None)
    with Lock():
        state = load()
        vms = pool_vms()
        spare = [v['Name'] for v in vms if is_free(state, v)]
        if spare:
            print(f"note: free VMs exist ({', '.join(spare)}); lease reuses them before creating", file=sys.stderr)
        names = plan_names(a.n, a.max, state, vms)
        if names is None:
            raise SystemExit(cap_message(a.n, a.max, state, vms))
        for n in names:
            mark_busy(state, n, 'creating')
        save(state)
    run_marked(lambda: [make_clone(n) for n in names], names)


def cmd_reclaim(a):
    with Lock():
        release_orphans(load(), a.max_age)
    tidy(a.idle_minutes, a.keep if a.prune else None)


# ---------------------------------------------------------------- the lease queue

def tickets():
    """The live queue tickets, first come first (file names); tickets whose process has ended are
    deleted. Call with the lock held."""
    try:
        files = sorted(f for f in os.listdir(QUEUE) if f.endswith('.json'))
    except FileNotFoundError:
        return []
    live = []
    for f in files:
        p = os.path.join(QUEUE, f)
        try:
            with open(p) as fh:
                t = json.load(fh)
            ok = _same_process(int(t['pid']), t.get('start'))
        except (OSError, ValueError, KeyError, TypeError):
            ok = False
        if ok:
            live.append(f)
        else:
            drop_ticket(f)
    return live


def add_ticket(agent, count, queue):
    """Join the queue behind every live ticket. Call with the lock held."""
    os.makedirs(QUEUE, exist_ok=True)
    f = f'{(int(queue[-1].split("-")[0]) + 1 if queue else 1):08d}-{os.getpid()}.json'
    tmp = os.path.join(QUEUE, f + '.tmp')
    with open(tmp, 'w', newline='\n') as fh:
        json.dump({'pid': os.getpid(), 'start': _pid_started(os.getpid()), 'agent': agent,
                   'count': count, 'since': now()}, fh)
    os.replace(tmp, os.path.join(QUEUE, f))
    return f


def drop_ticket(f):
    try:
        os.remove(os.path.join(QUEUE, f))
    except OSError:
        pass


def allot(state, vms, count, max_vms):
    """(names, new) for count VMs: free ones first (running ones, then stopped), then new clone
    names within the cap; None if they don't fit now. Call with the lock held."""
    free = sorted((v for v in vms if is_free(state, v)), key=lambda v: (v['State'] != 'Running', v['Name']))
    names = [v['Name'] for v in free[:count]]
    new = []
    if len(names) < count:
        new = plan_names(count - len(names), max_vms, state, vms)
        if new is None:
            return None
    return names + new, new


def freed(s, **extra):
    """A released state entry; a busy mark (a VM being created) survives the release."""
    d = dict({'agent': None, 'released': now()}, **extra)
    if s.get('busy'):
        d['busy'] = s['busy']
    return d


def acquire(a, who):
    """Record a lease of a.count VMs (pending until ready; new clones planned and marked busy) and
    return (names, new). Leases are served first come, first served: with others queued, or at the
    cap, this process queues a ticket and waits up to a.wait seconds (0: exit at once), printing
    a progress line every PROGRESS seconds. The lock is held only to check and record."""
    t0, said, ticket = time.time(), None, None
    try:
        while True:
            with Lock():
                state = load()
                release_orphans(state, a.max_age)
                queue = tickets()
                if ticket not in queue:
                    ticket = None                        # deleted by hand: queue again at the end
                ahead = queue.index(ticket) if ticket else len(queue)
                if ahead == 0:
                    vms = pool_vms()
                    got = allot(state, vms, a.count, a.max)
                    if got:
                        names, new = got
                        for n in names:
                            state[n] = dict(who, agent=a.agent, since=now(), pending=True)
                        for n in new:
                            mark_busy(state, n, 'creating')
                        save(state)
                        if ticket:
                            drop_ticket(ticket)
                            ticket = None
                        return names, new
                    why = cap_message(a.count, a.max, state, vms)
                else:
                    why = f'pool queue: {ahead} lease(s) waiting ahead'
                if a.wait <= 0:
                    raise SystemExit(why)
                if ticket is None:
                    ticket = add_ticket(a.agent, a.count, queue)
                    queue.append(ticket)
                pos, total = queue.index(ticket) + 1, len(queue)
            waited = time.time() - t0
            if waited >= a.wait:
                raise SystemExit(f'pool: no VM after {waited:.0f} s in the queue (--wait {a.wait}); {why}')
            if said is None or time.time() - said >= PROGRESS:
                print(f'pool: {a.agent} waits for {a.count} VM(s): queue position {pos} of {total}, '
                      f'{waited:.0f} s of {a.wait} ({why})', file=sys.stderr, flush=True)
                said = time.time()
            time.sleep(POLL)
    finally:
        if ticket:
            drop_ticket(ticket)


def cmd_lease(a):
    if not 1 <= a.count <= a.max:
        raise SystemExit(f'pool: --count {a.count} can never be served with --max {a.max}')
    who = holder(a.holder_pid)
    names, new = acquire(a, who)
    try:
        if new:
            run_marked(lambda: [make_clone(n) for n in new], new)
        tidy(a.idle_minutes, a.keep if a.prune else None, exclude=names)
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
                s = state.get(n, {})
                if s.get('agent') == a.agent and s.get('lease_pid') == os.getpid():
                    state[n] = freed(s)
            save(state)
        raise
    with Lock():
        state = load()
        for n, h in zip(names, hosts):
            s = state.get(n, {})
            if s.get('agent') == a.agent:
                s['host'] = h
                s.pop('pending', None)
        save(state)
    print('ENB_VC6_HOSTS=' + ','.join(hosts))


def cmd_release(a):
    with Lock():
        state = load()
        done = []
        for n, s in state.items():
            if s.get('agent') and ((a.agent and s['agent'] == a.agent) or n in a.vms):
                state[n] = freed(s)
                done.append(n)
        save(state)
    print('released ' + (', '.join(done) or 'nothing'))


def cmd_stop_idle(a):
    tidy(a.minutes)


def cmd_destroy(a):
    with Lock():
        state = load()
        vms = {v['Name']: v for v in pool_vms()}
        if a.all_free:
            names = [n for n in vms if not state.get(n, {}).get('agent') and not busy_live(state.get(n))]
        else:
            names = a.vms
        for n in names:
            if n not in vms:
                raise SystemExit(f'no pool VM named {n}')
            if state.get(n, {}).get('agent'):
                raise SystemExit(f'{n} is leased by {state[n]["agent"]}: release it first')
            if busy_live(state.get(n)):
                raise SystemExit(f'{n} is busy ({state[n]["busy"]["what"]} by pid {state[n]["busy"]["pid"]})')
            checked(vms[n])
        for n in names:
            mark_busy(state, n, 'destroying')
        save(state)
    run_marked(lambda: [destroy_vm(vms[n]) for n in names], names, gone=names)


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
    le.add_argument('--wait', type=int, default=WAIT,
                    help='seconds to wait in the queue while the pool is at its cap (0: exit at once)')
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
