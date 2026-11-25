"""The baseline store (docs/test-plan.md C1): retail runs of e2e scenarios kept with a manifest and their measured
noise in the private repository Open-EnB/baselines, checked out at machine.py's `baselines`.

    python tools/harness/baseline.py capture NAME... [--runs 3] [--live [SERVER]] [--db-snapshot [AVATAR]]
                                     [--instance bl] [--store DIR]
    python tools/harness/baseline.py capture --set SET [NAME...] [--runs 3] ...     one e2e session a run (a set)
    python tools/harness/baseline.py verify [NAME...] [--store DIR]
    python tools/harness/baseline.py verify --run RUN [--client C] [--env E] [--store DIR]
    python tools/harness/baseline.py list [--store DIR]
    python tools/harness/baseline.py inputs [SET...] [--write] [--save-db] [--apply-db] [--store DIR]
    python tools/harness/baseline.py tools [--add EXE...] [--store DIR]

Layout of the store:  runs/<client>/<env>/<scenario>/<YYYYMMDD-HHMMSS>/   one stored run (UTC start time):
    manifest.json, e2e.log, events.log, antprox.out, d3d8shot.log, camera.txt, messages.log (if any),
    result-<scenario>.json, shot_*.png, packets.json ({scenario: [packets the client sent]}, e2e.sent_packets)
runs/<client>/<env>/<scenario>/noise.json: every stored run of the scenario against every other (raw shot_diff).
<client> is retail-<first 8 hex of the exe's SHA-256>; <env> is <machine label>-<W>x<H> (the shots' size).
.staging/ (git-ignored) holds e2e's session folders while capturing (e2e --out); a failed run stays there.

capture runs the retail client (a copy, D190) through `e2e.py --retail` once per run, one client each, until --runs
runs passed or one extra attempt failed; only a PASS is stored. With --set the scenarios (none named: e2e's default
set) run as one e2e session a run, stored as runs/<client>/<env>/<SET>/<run>/ when all passed in that one session
(manifest: set, order, scenarios; noise.json keyed by scenario, then shot). Each shot's draw log
(shot_NAME_<frame>.json, d3d8shot D3D8SHOT_DRAWLOG) is kept and compared structurally (the draw-log section). verify checks every stored file against its
manifest's SHA-256 (a Git LFS pointer is reported as "run git lfs pull"); it needs neither the game nor Windows.
verify --run compares an e2e run (a session folder, several joined by ',', an e2e --manifest file, or a stored
run's manifest.json) with the stored runs of the same client and env: the verdict, the packets (equal to at least
one stored run's) and each shot (its differing cells against the best-matching stored run at most noise.json's
max for that shot). One line per scenario, SAME or DIFF; exit 1 on any DIFF or problem.

inputs/ holds what a second device needs to repeat a run: captures/ (the replay set antprox /REPLAY reads, machine.py
`recordings`), records/<YYYYMMDD-HHMMSS>/ (e2e record's tracks), db/ (the test account's row and its avatars'
restore scripts, content/ scripts; README.md there). inputs/manifest.json: every file's SHA-256 and size, the
captures' replay ids and the scenarios that use them, a hash per set. `inputs` checks it (verify does too), with
SET names lists their files; --write rewrites it; --save-db dumps the database state; --apply-db restores it.
capture --live restores that state before each run (--no-db-restore: not), in place through Net7.exe's control
port, or with a restart (--restart; the default when a scenario is sp_*); the manifest's server says which.
capture keeps the audio log (e2e --audio; --no-audio: not) as audio-NAME.json per scenario (the audio section).
tools/antprox/ holds the proxy pair the harness runs (antprox-rs.exe, antreplay.exe; machine.py antprox_dir) with a
manifest.json (version, SHA-256, size, source path and its repo's HEAD); `tools --add EXE` copies and records one;
verify checks it.
"""
import argparse
import datetime
import difflib
import hashlib
import itertools
import json
import os
import re
import shutil
import struct
import subprocess
import sys
import time

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(os.path.dirname(HERE))
sys.path.insert(0, HERE)
import machine                                  # noqa: E402

SCHEMA = 1
LOGS = ('e2e.log', 'events.log', 'antprox.out', 'd3d8shot.log', 'camera.txt', 'messages.log')
LFS_HEAD = b'version https://git-lfs.github.com/spec/v1'
# The client's own settings: RegistryClass opens HKEY_LOCAL_MACHINE (the 32-bit view: WOW6432Node); a client run
# without elevation reads and writes UAC's per-user copy, HKCU\Software\Classes\VirtualStore\MACHINE\SOFTWARE\...,
# over it. Both are read (never written) and merged ('effective').
REG_KEY = r'SOFTWARE\Westwood Studios\Earth and Beyond'
REG_VSTORE = r'Software\Classes\VirtualStore\MACHINE\SOFTWARE\WOW6432Node\Westwood Studios\Earth and Beyond'
# release\ files the runs write or replace: not part of the data's identity
VOLATILE = {'client.exe', 'client.map', 'd3d8.dll', 'stubs.log', '_except.txt', 'd3d8shot.log', 'authlogin.log',
            'stderr.txt', 'chat.log'}
# Scenarios that need a session of their own: sb_emote_dock starts where the login leaves the character (the hangar,
# room 0; every other station scenario's own steps start in the concourse, room 1, and end there) and ends docked in
# the hangar again, so it can neither follow nor precede another scenario in one session (2026-11-24: sb_buysell
# after it, 'walk-to -44,-38: not moving'). sb_manufacture_make fails after sb_buysell in one session (2026-11-24:
# step 84, "wait-event antprox 'sent ITEM_BASE': not seen within 20 s") and passes alone. A set's capture runs each
# of them alone, as a scenario folder of its own
SESSION_OWN = ('sb_emote_dock', 'sb_manufacture_make')
SMALL = 16 * 1024               # data id: files up to this size are hashed by content, larger ones by size only


# ---------------------------------------------------------------- small helpers (portable)

def sha256_file(path):
    h = hashlib.sha256()
    with open(path, 'rb') as f:
        for block in iter(lambda: f.read(1 << 20), b''):
            h.update(block)
    return h.hexdigest()


def is_lfs_pointer(path):
    try:
        with open(path, 'rb') as f:
            return f.read(len(LFS_HEAD)) == LFS_HEAD
    except OSError:
        return False


def png_size(path):
    with open(path, 'rb') as f:
        head = f.read(24)
    if head[:8] != b'\x89PNG\r\n\x1a\n' or head[12:16] != b'IHDR':
        return None
    return list(struct.unpack('>II', head[16:24]))


def store_root(args):
    return os.path.abspath(getattr(args, 'store', None) or machine.get('baselines'))


def write_json(path, data):
    with open(path, 'w', encoding='utf-8', newline='\n') as f:
        json.dump(data, f, indent=1, sort_keys=False)
        f.write('\n')


def read_json(path):
    with open(path, encoding='utf-8') as f:
        return json.load(f)


def shot_keys(shots):
    """[(shot name, path)] -> [(key, path)]: the name, NAME#2 for its second shot of the scenario, ..."""
    seen, out = {}, []
    for name, path in shots:
        seen[name] = seen.get(name, 0) + 1
        out.append((name if seen[name] == 1 else f'{name}#{seen[name]}', path))
    return out


def data_id(root, ea_root=None):
    """The identity of a data root (release\\ and Data\\) without hashing gigabytes: a SHA-256 over the sorted
    (relative path, size) of every file, with the content hash of each file up to SMALL bytes; release\\'s files the
    runs write (VOLATILE, *.log), Data\\client\\ini and output (the instance's own copies) and the *.th6 thumbnails
    the client writes are left out."""
    lines, count, total = [], 0, 0
    for top in ('release', 'Data'):
        base = os.path.join(root, top)
        for d, dirs, files in os.walk(base):
            rel_d = os.path.relpath(d, root).replace('\\', '/')
            if rel_d.lower() in ('data/client/ini', 'data/client/output'):
                dirs[:] = []
                continue
            dirs.sort()
            for f in sorted(files):
                low = f.lower()
                if top == 'release' and d == base and (low in VOLATILE or low.endswith('.log')):
                    continue
                if low.endswith('.th6'):
                    continue
                p = os.path.join(d, f)
                size = os.path.getsize(p)
                rel = (rel_d + '/' + f).lower()
                lines.append(f'{rel}\t{size}\t{sha256_file(p) if size <= SMALL else ""}\n')
                count += 1
                total += size
    lines.sort()
    kind = 'ea' if ea_root and os.path.normcase(os.path.abspath(root)) == os.path.normcase(os.path.abspath(ea_root)) \
        else 'clientpoc-net7'
    return {'id': hashlib.sha256(''.join(lines).encode()).hexdigest(), 'kind': kind, 'files': count, 'bytes': total,
            'rule': f'sha256 over sorted (path, size, sha256 if <= {SMALL} bytes) of release/ and Data/ '
                    '(not Data/client/ini, Data/client/output, *.th6, release/ logs and run files)'}


def ini_settings(ini_dir):
    """Data\\client\\ini of the instance the run used: file count and a hash over (name, sha256); auth.ini apart
    (antprox rewrites its server URLs at every start)."""
    if not os.path.isdir(ini_dir):
        return None
    rows, auth = [], None
    for f in sorted(os.listdir(ini_dir), key=str.lower):
        p = os.path.join(ini_dir, f)
        if not os.path.isfile(p):
            continue
        if f.lower() == 'auth.ini':
            auth = sha256_file(p)
            continue
        rows.append(f'{f.lower()}\t{sha256_file(p)}\n')
    return {'files': len(rows) + (auth is not None), 'sha256': hashlib.sha256(''.join(rows).encode()).hexdigest(),
            'auth_ini_sha256': auth, 'rule': 'sha256 over sorted (name, sha256) of every file but auth.ini'}


# ---------------------------------------------------------------- Windows-only readings (None elsewhere)

def _reg_tree(hive, path, flags=0):
    import winreg
    try:
        key = winreg.OpenKey(hive, path, 0, winreg.KEY_READ | flags)
    except OSError:
        return None
    out = {}
    with key:
        i = 0
        while True:
            try:
                name, value, typ = winreg.EnumValue(key, i)
            except OSError:
                break
            if isinstance(value, bytes):
                value = value.hex()
            out[name or '(default)'] = value
            i += 1
        i = 0
        while True:
            try:
                sub = winreg.EnumKey(key, i)
            except OSError:
                break
            out[sub + '\\'] = _reg_tree(hive, path + '\\' + sub, flags)
            i += 1
    return out


def _merge(base, over):
    out = dict(base or {})
    for k, v in (over or {}).items():
        out[k] = _merge(out.get(k), v) if isinstance(v, dict) else v
    return out


def registry_settings():
    """The client's registry settings, read only: {'hklm', 'virtualstore', 'effective'}, or None off Windows."""
    try:
        import winreg
    except ImportError:
        return None
    hklm = _reg_tree(winreg.HKEY_LOCAL_MACHINE, REG_KEY, winreg.KEY_WOW64_32KEY)
    vs = _reg_tree(winreg.HKEY_CURRENT_USER, REG_VSTORE)
    return {'key': 'HKLM\\' + REG_KEY + ' (32-bit view)', 'hklm': hklm, 'virtualstore': vs,
            'effective': _merge(hklm, vs)}


def git_state():
    def git(*a):
        return subprocess.run(['git', '-C', ROOT] + list(a), capture_output=True, text=True).stdout.strip()
    try:
        return {'commit': git('rev-parse', 'HEAD'),
                'dirty': bool(git('status', '--porcelain', '--untracked-files=no'))}
    except OSError:
        return None


# ---------------------------------------------------------------- capture

def scenario_files(name):
    """{repo path: sha256} of the scenario, every file it includes, gadgets.txt and shot_mask.txt."""
    import e2e
    seen = []

    def loader(n):
        if n not in seen:
            seen.append(n)
        return e2e.file_loader(n)
    e2e.parse(loader(name), name, e2e.load_gadgets(), loader)
    paths = [os.path.join(e2e.SCN, n + '.scn') for n in seen]
    paths += [os.path.join(e2e.SCN, 'gadgets.txt'), e2e.SHOT_MASK]
    return {os.path.relpath(p, ROOT).replace('\\', '/'): sha256_file(p) for p in paths if os.path.exists(p)}


def antprox_used(live):
    path = machine.get('antprox' if live else 'antprox_replay')     # clientrun.antprox_exe; ANTPROX_EXE overrides both
    version = None
    try:                                # /DESCRIBE prints the options as JSON, with the version
        r = subprocess.run([path, '/DESCRIBE'], capture_output=True, text=True, timeout=20)
        version = json.loads(r.stdout).get('version')
    except (OSError, ValueError, subprocess.SubprocessError, AttributeError):
        pass
    return {'file': os.path.basename(path), 'version': version,
            'sha256': sha256_file(path) if os.path.exists(path) else None}


def antreplay_used(live):
    """The replay server antprox /REPLAY starts (a live run does not start it; recorded all the same)."""
    rp = machine.get('antreplay')
    return {'file': os.path.basename(rp), 'version': exe_version(rp), 'used': not live,
            'sha256': sha256_file(rp) if os.path.exists(rp) else None}


def exe_version(path):
    """A Windows exe's version resource (ProductVersion, else FileVersion), or None."""
    if sys.platform != 'win32' or not os.path.exists(path):
        return None
    try:
        r = subprocess.run(['powershell', '-NoProfile', '-Command',
                            f"$v=(Get-Item -LiteralPath '{path}').VersionInfo; if ($v.ProductVersion) "
                            "{$v.ProductVersion} else {$v.FileVersion}"], capture_output=True, text=True, timeout=60)
        return r.stdout.strip() or None
    except (OSError, subprocess.SubprocessError):
        return None


def store_run(store, client, label, name, session, result, start_utc, common, ini, db, log, set_scens=None,
              srv=None, secs=None):
    """Copies one PASS run from an e2e session folder into the store; returns the run folder. set_scens: a set's run
    ([(scenario, its result)] in the order run, all in this one session), stored under runs/<client>/<env>/<name>/
    with the scenarios listed in the manifest; else name is the one scenario and result its result. SRV: what was
    done to the server before the run (db_apply's 'server', or {'restore': 'none'}); SECS: the session's seconds."""
    import e2e
    names = [n for n, _ in set_scens] if set_scens else [name]
    all_shots = e2e.run_shots([session])
    first = next((s for n in names for s in all_shots.get(n, [])), None)
    size = png_size(first[1]) if first else None
    env = f'{label}-{size[0]}x{size[1]}' if size else f'{label}-noshots'
    stamp = time.strftime('%Y%m%d-%H%M%S', time.gmtime(start_utc))
    dest = os.path.join(store, 'runs', client, env, name, stamp)
    n = 1
    while os.path.exists(dest):
        n += 1
        dest = os.path.join(store, 'runs', client, env, name, f'{stamp}-{n}')
    os.makedirs(dest)
    for f in LOGS + tuple(f'result-{x}.json' for x in names):
        if os.path.exists(os.path.join(session, f)):
            shutil.copy2(os.path.join(session, f), os.path.join(dest, f))
    entries = []
    for x in names:
        shot_list, draw_list = [], []
        for key, path in shot_keys(all_shots.get(x, [])):
            shutil.copy2(path, os.path.join(dest, os.path.basename(path)))
            shot_list.append([key, os.path.basename(path)])
            dl = drawlog_path(path)
            if dl:
                shutil.copy2(dl, os.path.join(dest, os.path.basename(dl)))
                draw_list.append([key, os.path.basename(dl)])
        entries.append({'name': x, 'shots': shot_list, 'drawlogs': draw_list})
    audio, alog = session_audio(session, set_scens or [(name, result)])
    for en in entries:
        if en['name'] in audio:
            en['audio'] = f'audio-{en["name"]}.json'
            write_json(os.path.join(dest, en['audio']), audio[en['name']])
    if alog:
        alog['kept'] = alog['bytes'] < AUDIO_RAW_MAX
        if alog['kept']:
            shutil.copy2(os.path.join(session, 'audio.jsonl'), os.path.join(dest, 'audio.jsonl'))
    pk = e2e.sent_packets(session)
    write_json(os.path.join(dest, 'packets.json'), {x: pk.get(x, []) for x in names})
    files = {f: {'sha256': sha256_file(os.path.join(dest, f)), 'bytes': os.path.getsize(os.path.join(dest, f))}
             for f in sorted(os.listdir(dest))}
    sfiles = {}
    for x in names:
        sfiles.update(scenario_files(x))
    man = {'schema': SCHEMA, 'scenario': name,
           'captured_utc': datetime.datetime.fromtimestamp(start_utc, datetime.timezone.utc).strftime(
               '%Y-%m-%dT%H:%M:%SZ'),
           'client': {'name': client, 'exe_sha256': common['exe_sha256']},
           'env': env, 'resolution': size,
           'data': common['data'], 'settings': {'registry': common['registry'], 'ini': ini},
           'machine': common['machine'], 'proxy': common['proxy'], 'antprox': common['antprox'],
           'antreplay': common.get('antreplay'),
           'enb': common['enb'], 'scenario_files': dict(sorted(sfiles.items())),
           'server': dict(common['server'], db=db, **(srv or {'restore': 'none'})),
           'e2e_session': os.path.basename(session), 'audio_log': alog}
    if secs is not None:
        man['session_seconds'] = round(secs, 1)
    if set_scens:
        res = dict(set_scens)
        for e in entries:
            e['verdict'] = e2e.verdict(res[e['name']])
            e['failure'] = res[e['name']].get('failure')
        man.update(set=name, verdict='PASS' if all(e['verdict'] == 'PASS' for e in entries) else 'FAIL',
                   order=names, scenarios=entries)
        what = f'{len(names)} scenarios, {sum(len(e["shots"]) for e in entries)} shots, ' \
               f'{sum(len(e["drawlogs"]) for e in entries)} draw logs, {sum("audio" in e for e in entries)} audio'
    else:
        man.update(verdict=e2e.verdict(result), failure=result.get('failure'), shots=entries[0]['shots'],
                   drawlogs=entries[0]['drawlogs'], **({'audio': entries[0]['audio']} if 'audio' in entries[0] else {}))
        what = f'{len(entries[0]["shots"])} shots, {len(entries[0]["drawlogs"])} draw logs'
    man['files'] = files
    write_json(os.path.join(dest, 'manifest.json'), man)
    log(f'baseline: stored {os.path.relpath(dest, store)} ({what})')
    return dest


def capture(args):
    import clientrun
    import e2e
    store = store_root(args)
    if not os.path.isdir(os.path.join(store, '.git')):
        print(f'baseline: {store} is not a checkout of the store (git clone it there first)')
        return 2
    staging = os.path.join(store, '.staging')
    os.makedirs(staging, exist_ok=True)
    if args.set and not args.names:
        args.names = e2e.scenario_names(bool(args.live))      # e2e's default set
    own = [n for n in args.names if n in SESSION_OWN] if args.set else []
    if own:
        print(f'baseline: {", ".join(own)} need a session of their own (SESSION_OWN): captured alone, not in the set',
              flush=True)
    for n in args.names:
        e2e.load(n)                     # a scenario error before any client starts
    exe = clientrun.RETAIL
    exe_sha = sha256_file(exe)
    client = 'retail-' + exe_sha[:8]
    gpu = machine.gpu_info()
    label = machine.get('label')
    common = {
        'exe_sha256': exe_sha,
        'data': data_id(clientrun.CLIENT_DATA_ROOT, clientrun.EA_DATA_ROOT),
        'registry': registry_settings(),
        'machine': {'label': label, 'gpu': gpu and gpu['name'], 'driver': gpu and gpu['driver'],
                    'os': machine.os_version()},
        'proxy': {'d3d8_sha256': sha256_file(clientrun.PROXY) if os.path.exists(clientrun.PROXY) else None},
        'antprox': antprox_used(bool(args.live)),
        'antreplay': antreplay_used(bool(args.live)),
        'enb': git_state(),
        'server': {'mode': 'live', 'server': args.live} if args.live else {'mode': 'replay'},
    }
    if not args.live:                   # the recordings antprox /REPLAY reads (clientrun.start_antprox)
        recs = clientrun.recordings_folder()
        common['server']['recordings'] = {'folder': recs, 'sha256': recs and folder_set_hash(recs),
                                          'store_copy': bool(recs) and os.path.normcase(os.path.abspath(recs)) ==
                                          os.path.normcase(os.path.join(store, INPUTS, 'captures'))}
    restore = bool(args.live) and not args.no_db_restore
    # a restart before each run: --restart / --no-restart, by default when a space scenario is in the list (their
    # MOBs, asteroids and spawned mobs stay in the server's memory across an in-place restore)
    restart = bool(args.live) and (args.restart if args.restart is not None else
                                   any(n.startswith('sp_') for n in args.names))
    if args.live:
        print('baseline: the server before each run: '
              + ('restarted' if restart else 'restored in place' if restore else 'left as it is')
              + (', the database state applied' if restore else ''), flush=True)
    if restore and not db_state_files(store):
        print(f'baseline: --live restores the database state from {store}\\inputs\\db before each run, and there is '
              'none (baseline.py inputs --save-db, or --no-db-restore)')
        return 2
    ini_dir = os.path.join(clientrun.Inst(args.instance, retail=True).base, 'Data', 'client', 'ini')
    # a set: one e2e session over all its scenarios per run (one client); else one scenario a client, each from the
    # restored state. Either way the database state is restored once per run, before its client starts
    units = ([(args.set, [n for n in args.names if n not in own])] + [(n, [n]) for n in own]) if args.set else \
        [(n, [n]) for n in args.names]
    stored = {u: 0 for u, _ in units}
    attempt = 0
    for unit, names in units:
        failed = 0
        while stored[unit] < args.runs and failed <= args.retries:
            attempt += 1
            db_state = None
            if restore or restart:
                db_state = db_apply(store, lambda m: print(m, flush=True), restart=True if restart else None,
                                    scripts=restore)
                if not db_state['ready']:
                    print('baseline: the restore failed or Net7.exe did not come back after it; stopping')
                    return 1
            t0 = time.time()
            stamp = time.strftime('%Y%m%d-%H%M%S')
            man_path = os.path.join(staging, f'{stamp}-e2e-run.json')
            out_path = os.path.join(staging, f'{stamp}-e2e.out')
            cmd = [sys.executable, os.path.join(HERE, 'e2e.py'), '--retail', '--instance', args.instance, '--keep',
                   '--out', staging, '--manifest', man_path]
            if args.live:
                cmd += ['--live', args.live]
            if not args.no_audio:
                cmd += ['--audio']
            if args.db_snapshot is not None:
                cmd += ['--db-snapshot', str(args.db_snapshot)]
            cmd += ['run'] + names
            print(f'baseline: run {attempt}: {unit}' + (f' ({len(names)} scenarios, one session)' if unit == args.set else '')
                  + f' (log {out_path})', flush=True)
            with open(out_path, 'w', encoding='utf-8') as fo:
                rc = subprocess.run(cmd, stdout=fo, stderr=subprocess.STDOUT, cwd=ROOT).returncode
            secs = time.time() - t0
            ini = ini_settings(ini_dir)
            clientrun.clean(args.instance)
            db = {'restored_before': db_state and db_state['files']} if restore else {}
            srv = db_state['server'] if db_state else {'restore': 'none'}
            if args.db_snapshot is not None:
                snaps = sorted(d for d in os.listdir(staging) if d.endswith('-db-snapshot')
                               and os.path.getmtime(os.path.join(staging, d)) >= t0 - 1)
                sql = os.path.join(staging, snaps[-1], f'avatar{args.db_snapshot}.sql') if snaps else None
                db.update(avatar=args.db_snapshot,
                          sha256=sha256_file(sql) if sql and os.path.exists(sql) else None)
            if not os.path.exists(man_path):
                print(f'baseline: run {attempt}: e2e exit {rc} and no run manifest; see {out_path}')
                failed += 1
                continue
            rows = read_json(man_path)['scenarios']
            if args.set and unit == args.set:
                outs = sorted({r['out'] or '' for r in rows})
                bad = [r for r in rows if r['verdict'] != 'PASS'
                       or not os.path.exists(os.path.join(r['out'] or '', f'result-{r["name"]}.json'))]
                if bad or len(outs) != 1 or [r['name'] for r in rows] != names:
                    why = '; '.join(f'{r["name"]} {r["verdict"]} ({(r.get("failure") or "")[:80]})' for r in bad[:3])
                    print(f'baseline: run {attempt}: {unit} not stored: {len(bad)} of {len(rows)} not PASS'
                          + (f', {len(outs)} sessions (a fresh one after a failure)' if len(outs) != 1 else '')
                          + (f': {why}' if why else '') + f'; left in {", ".join(outs)}')
                    failed += 1
                    continue
                res = [(r['name'], read_json(os.path.join(outs[0], f'result-{r["name"]}.json'))) for r in rows]
                store_run(store, client, label, unit, outs[0], None, t0, common, ini, db or None,
                          lambda m: print(m, flush=True), set_scens=res, srv=srv, secs=secs)
                print(f'baseline: run {attempt}: {unit}: one session, {secs:.0f} s', flush=True)
                stored[unit] += 1
                continue
            for r in rows:
                res_path = os.path.join(r['out'] or '', f'result-{r["name"]}.json')
                if r['verdict'] != 'PASS' or not os.path.exists(res_path):
                    print(f'baseline: run {attempt}: {r["name"]} {r["verdict"]} ({(r.get("failure") or "")[:100]}); '
                          f'left in {r["out"]}')
                    failed += 1
                    continue
                store_run(store, client, label, r['name'], r['out'], read_json(res_path), t0, common, ini,
                          db or None, lambda m: print(m, flush=True), srv=srv)
                stored[r['name']] += 1
    for unit, _ in units:
        envs = os.path.join(store, 'runs', client)
        for env in sorted(os.listdir(envs)) if os.path.isdir(envs) else []:
            d = os.path.join(envs, env, unit)
            if os.path.isdir(d):
                nz = write_noise(d)
                print(f'baseline: {client}/{env}/{unit}: {len(nz["runs"])} runs; ' + noise_summary(nz), flush=True)
    short = [u for u, _ in units if stored[u] < args.runs]
    if short:
        print(f'baseline: fewer than {args.runs} PASS runs stored for {", ".join(short)}')
        return 1
    return 0


# ---------------------------------------------------------------- draw logs (d3d8shot D3D8SHOT_DRAWLOG; test-plan C3)
#
# A shot's draw log, shot_NAME_<frame>.json beside its .png (d3d8shot.c, above Log_Draw: the schema). Compared
# structurally: each draw flattened to {field: value} (call prim prims verts stride fvf; tex0.* / tex1.*: the
# texture's creation parameters and content hash; rs.NAME; ts0.NAME / ts1.NAME), the draws of two logs aligned on
# (call, prim, fvf, stride) with difflib (a draw more or fewer does not shift every later one), then field by field.
# The matrices (each draw's view, projection and world, and the frame's SetTransform sequence) compare with a
# tolerance: per draw, the stored run's own spread for it times DRAW_MATRIX_SLACK plus DRAW_MATRIX_FLOOR.

DRAW_FIELDS = ('call', 'prim', 'prims', 'verts', 'stride', 'fvf')
TEX_FIELDS = ('type', 'w', 'h', 'format', 'levels', 'usage', 'pool', 'hash')
DRAW_MATRICES = ('view', 'proj', 'world')
DRAW_MATRIX_FLOOR = 1e-4        # "%.7g" of values up to the thousands: below this two prints are the same matrix
DRAW_MATRIX_SLACK = 1.5


def drawlog_path(shot_path):
    """The draw log beside a shot (shot_NAME_<frame>.png -> .json), or None."""
    if not shot_path:
        return None
    p = os.path.splitext(shot_path)[0] + '.json'
    return p if os.path.isfile(p) else None


def load_drawlog(path):
    """A draw log -> {'draws': [(flat fields, {view, proj, world})], 'sets': [(kind, matrix)], 'count': n}."""
    d = read_json(path)
    texs, views, projs = d.get('textures', []), d.get('views', []), d.get('projections', [])
    rsn, tsn = d.get('render_states', []), d.get('stage_states', [])

    def at(lst, i):
        return lst[i] if isinstance(i, int) and 0 <= i < len(lst) else None
    draws = []
    for dr in d.get('draws', []):
        flat = {f: dr.get(f) for f in DRAW_FIELDS}
        for s, i in enumerate((dr.get('tex') or [-1, -1])[:2]):
            t = at(texs, i) or {}
            for k in TEX_FIELDS:
                flat[f'tex{s}.{k}'] = t.get(k)
        for n, v in zip(rsn, dr.get('rs', [])):
            flat[f'rs.{n}'] = v
        for s, vals in enumerate(dr.get('ts', [])):
            for n, v in zip(tsn, vals):
                flat[f'ts{s}.{n}'] = v
        draws.append((flat, {'view': at(views, dr.get('view')), 'proj': at(projs, dr.get('proj')),
                             'world': dr.get('world')}))
    sets = [('view', at(views, s['view'])) if 'view' in s else ('proj', at(projs, s.get('proj')))
            for s in d.get('sets', [])]
    return {'draws': draws, 'sets': sets, 'count': len(draws)}


def _mdelta(a, b):
    """The largest element difference of two matrices (None: absent or NaN; equal when both are)."""
    if a is None or b is None:
        return 0.0 if a is b else float('inf')
    worst = 0.0
    for x, y in zip(a, b):
        if x is None or y is None:
            if x is not y:
                return float('inf')
            continue
        worst = max(worst, abs(x - y))
    return worst if len(a) == len(b) else float('inf')


def compare_drawlogs(a, b, tol=None):
    """Two loaded draw logs -> {'counts': [na, nb], 'unmatched': draws without a partner, 'first_unmatched': index
    in a, 'fields': {field: first draw of a where it differs}, 'first': [draw, [fields]] or None, 'matrix':
    {view, proj, world, sets: the largest difference}, 'deltas': [[i, j, {kind: difference}]] of the paired draws
    whose matrices differ above DRAW_MATRIX_FLOOR, 'bad': {kind: [count, first draw of a, worst]} of the matrices
    over tol(j, kind) (default DRAW_MATRIX_FLOOR), 'sets': whether the SetTransform kinds are the same}."""
    tol = tol or (lambda j, k: DRAW_MATRIX_FLOOR)

    def sig(dr):
        f = dr[0]
        return (f['call'], f['prim'], f['fvf'], f['stride'])
    sm = difflib.SequenceMatcher(None, [sig(x) for x in a['draws']], [sig(x) for x in b['draws']], autojunk=False)
    pairs, unmatched, first_unmatched = [], 0, None
    for op, i1, i2, j1, j2 in sm.get_opcodes():
        if op == 'equal' or (op == 'replace' and i2 - i1 == j2 - j1):
            pairs.extend(zip(range(i1, i2), range(j1, j2)))
        else:
            unmatched += max(i2 - i1, j2 - j1)
            if first_unmatched is None:
                first_unmatched = i1
    fields, first, deltas, bad = {}, None, [], {}
    mat = {k: 0.0 for k in DRAW_MATRICES}
    for i, j in pairs:
        fa, fb = a['draws'][i][0], b['draws'][j][0]
        diff = sorted(k for k in set(fa) | set(fb) if fa.get(k) != fb.get(k))
        for k in diff:
            fields.setdefault(k, i)
        if diff and first is None:
            first = [i, diff]
        dd = {}
        for k in DRAW_MATRICES:
            d = _mdelta(a['draws'][i][1][k], b['draws'][j][1][k])
            mat[k] = max(mat[k], d)
            if d > DRAW_MATRIX_FLOOR:
                dd[k] = d
            if d > tol(j, k):
                n, f0, w = bad.get(k, [0, i, 0.0])
                bad[k] = [n + 1, f0, max(w, d)]
        if dd:
            deltas.append([i, j, dd])
    kinds_same = [k for k, _ in a['sets']] == [k for k, _ in b['sets']]
    mat['sets'] = max((_mdelta(x[1], y[1]) for x, y in zip(a['sets'], b['sets'])), default=0.0) if kinds_same \
        else float('inf')
    if mat['sets'] > tol(None, 'sets'):
        bad['sets'] = [1, 0, mat['sets']]
    return {'counts': [a['count'], b['count']], 'unmatched': unmatched, 'first_unmatched': first_unmatched,
            'fields': fields, 'first': first, 'matrix': mat, 'deltas': deltas, 'bad': bad, 'sets': kinds_same}


def _canon(log):
    return json.dumps([log['draws'], log['sets']], sort_keys=True)


def _rnd(v):
    return None if v == float('inf') else round(v, 7)


def drawlog_noise(paths, runs):
    """{run: draw log path} of one shot -> noise.json's 'drawlog' entry: whether the stored runs' logs are the same,
    and when not what varies (draw count range, fields, draws without a partner, the matrices' spread overall and
    per draw: 'matrix_draws' {run: {draw index in that run: {kind: the largest difference to another run}}})."""
    for p in paths.values():
        if is_lfs_pointer(p):
            raise SystemExit(f'{p}: a Git LFS pointer: run git lfs pull')
    logs = {r: load_drawlog(p) for r, p in sorted(paths.items())}
    counts = [lg['count'] for lg in logs.values()]
    varying, unmatched = set(), 0
    spread = {k: 0.0 for k in DRAW_MATRICES + ('sets',)}
    per_draw = {r: {} for r in logs}
    sets_same = True
    for a, b in itertools.combinations(sorted(logs), 2):
        c = compare_drawlogs(logs[a], logs[b])
        varying |= set(c['fields'])
        unmatched = max(unmatched, c['unmatched'])
        sets_same &= c['sets']
        for k, v in c['matrix'].items():
            spread[k] = max(spread[k], v)
        for i, j, dd in c['deltas']:
            for run, idx in ((a, i), (b, j)):
                e = per_draw[run].setdefault(str(idx), {})
                for k, v in dd.items():
                    e[k] = max(e.get(k, 0.0), v)
    return {'runs': sorted(logs), 'runs_without': [r for r in runs if r not in logs],
            'identical': len({_canon(lg) for lg in logs.values()}) <= 1,
            'draws': [min(counts), max(counts)], 'varying': sorted(varying), 'unmatched_max': unmatched,
            'sets_same': sets_same,
            'matrix_spread': {k: _rnd(v) for k, v in spread.items()},
            'matrix_draws': {r: {i: {k: _rnd(v) for k, v in sorted(e.items())} for i, e in
                                 sorted(per_draw[r].items(), key=lambda x: int(x[0]))} for r in sorted(per_draw)},
            'bytes': [min(os.path.getsize(p) for p in paths.values()), max(os.path.getsize(p) for p in paths.values())]}


def _draw_desc(log, i):
    if i is None or i >= log['count']:
        return f'draw {i}'
    f = log['draws'][i][0]
    t = f"tex {f['tex0.w']}x{f['tex0.h']} fmt {f['tex0.format']}" if f.get('tex0.w') is not None else 'no texture'
    return f"draw {i} ({f['call']} prim {f['prim']} x{f['prims']}, fvf {f['fvf']:#x}, {t})" if isinstance(
        f.get('fvf'), int) else f'draw {i}'


def drawlog_verdict(mine_path, stored_paths, nzd):
    """A run's draw log for one shot against the stored ones -> (True SAME | False DIFF | None nothing to compare,
    text). SAME: equal to one stored run's log, or different only in fields and draw counts the stored runs vary in
    themselves, with every matrix within that stored run's own spread for the draw it pairs with (noise.json
    matrix_draws, times DRAW_MATRIX_SLACK, plus DRAW_MATRIX_FLOOR; a draw whose matrices never varied: the floor)."""
    if not stored_paths:
        return None, 'no draw log stored'
    if not mine_path:
        return False, 'no draw log in the run'
    nzd = nzd or {}
    allowed = set(nzd.get('varying', []))
    lo, hi = nzd.get('draws', [None, None])
    sets_spread = (nzd.get('matrix_spread') or {}).get('sets', 0.0)
    mine = load_drawlog(mine_path)
    best = None
    for p in stored_paths:
        if is_lfs_pointer(p):
            return False, f'{os.path.basename(p)} is a Git LFS pointer: run git lfs pull'
        run = os.path.basename(os.path.dirname(p))
        spread = (nzd.get('matrix_draws') or {}).get(run, {})

        def tol(j, k, spread=spread):
            v = sets_spread if j is None else spread.get(str(j), {}).get(k, 0.0)
            return float('inf') if v is None else DRAW_MATRIX_SLACK * v + DRAW_MATRIX_FLOOR
        c = compare_drawlogs(mine, load_drawlog(p), tol)
        bad_fields = sorted((f for f in c['fields'] if f not in allowed), key=lambda f: (c['fields'][f], f))
        bad_count = c['unmatched'] > nzd.get('unmatched_max', 0) or (
            c['unmatched'] and lo is not None and not lo <= mine['count'] <= hi)
        if not (bad_fields or c['bad'] or bad_count):
            return True, f'{mine["count"]} draws'
        score = (bad_count and c['unmatched'], len(bad_fields), sum(n for n, _, _ in c['bad'].values()))
        if best is None or score < best[0]:
            best = (score, c, bad_fields, bad_count)
    _, c, bad_fields, bad_count = best
    parts = [f'{c["counts"][0]} draws, stored {c["counts"][1]}']
    if bad_count:
        parts.append(f'{c["unmatched"]} without a partner from {_draw_desc(mine, c["first_unmatched"])}')
    if bad_fields:
        i = c['fields'][bad_fields[0]]
        here = [f for f in bad_fields if c['fields'][f] == i]
        parts.append(f'first difference {_draw_desc(mine, i)}: {", ".join(here[:6])}'
                     + (f' (+{len(here) - 6})' if len(here) > 6 else '')
                     + f'; {len(bad_fields)} fields differ in all')
    if c['bad']:
        parts.append('matrices ' + ', '.join(f'{k} in {n} draws from draw {f0} (up to {w:.4g})'
                                             for k, (n, f0, w) in sorted(c['bad'].items())))
    return False, '; '.join(parts)


# ---------------------------------------------------------------- noise

def stored_runs(scen_dir):
    return sorted(d for d in os.listdir(scen_dir) if os.path.isfile(os.path.join(scen_dir, d, 'manifest.json')))


def run_entries(scen_dir, name):
    """The stored runs of scenario name in scen_dir (a scenario's folder, or a set's that ran it):
    [{'run', 'dir', 'verdict', 'shots', 'drawlogs', 'packets', 'order'}]."""
    out = []
    for r in stored_runs(scen_dir):
        d = os.path.join(scen_dir, r)
        man = read_json(os.path.join(d, 'manifest.json'))
        if 'set' in man:
            e = next((x for x in man['scenarios'] if x['name'] == name), None)
            if e is None:
                continue
        else:
            e = man
        out.append({'run': r, 'dir': d, 'verdict': e['verdict'], 'shots': e.get('shots', []),
                    'drawlogs': e.get('drawlogs', []), 'order': man.get('order', [man['scenario']]),
                    'audio': e.get('audio') and os.path.join(d, e['audio']),
                    'packets': read_json(os.path.join(d, 'packets.json')).get(name, [])})
    return out


def dir_scenarios(scen_dir):
    """The scenarios a stored folder holds: a set's (its newest run's order), else the folder's own name."""
    runs = stored_runs(scen_dir)
    if not runs:
        return [os.path.basename(scen_dir)]
    man = read_json(os.path.join(scen_dir, runs[-1], 'manifest.json'))
    return list(man['order']) if 'set' in man else [man['scenario']]


def scenario_noise(entries):
    """One scenario's noise over its stored runs (run_entries): per shot the raw shot_diff pairs and the draw logs;
    whether packets and verdicts were equal."""
    import e2e
    shots, drawlogs = {}, {}
    for en in entries:
        for key, f in en['shots']:
            shots.setdefault(key, {})[en['run']] = os.path.join(en['dir'], f)
        for key, f in en['drawlogs']:
            drawlogs.setdefault(key, {})[en['run']] = os.path.join(en['dir'], f)
    out_shots = {}
    for key in sorted(shots):
        pairs = []
        for a, b in itertools.combinations(sorted(shots[key]), 2):
            if is_lfs_pointer(shots[key][a]) or is_lfs_pointer(shots[key][b]):
                raise SystemExit(f'{shots[key][a]}: a shot is still a Git LFS pointer: run git lfs pull')
            d = e2e.shot_diff(shots[key][a], shots[key][b])
            pairs.append({'a': a, 'b': b, 'differ': d['differ'], 'mean': round(d['mean'], 3),
                          **({'size': d['size']} if 'size' in d else {})})
        out_shots[key] = {'runs': sorted(shots[key]), 'pairs': pairs,
                          'max_differ': max((p['differ'] for p in pairs), default=0),
                          'max_mean': max((p['mean'] for p in pairs), default=0.0),
                          'cells': e2e.SHOT_GRID * e2e.SHOT_GRID}
        if key in drawlogs:
            out_shots[key]['drawlog'] = drawlog_noise(drawlogs[key], [en['run'] for en in entries])
    packets = {en['run']: en['packets'] for en in entries}
    verdicts = {en['run']: en['verdict'] for en in entries}
    pk_equal = len({json.dumps(v) for v in packets.values()}) <= 1
    v_equal = len(set(verdicts.values())) <= 1
    au = audio_noise(entries)
    return {'shots': out_shots, 'packets_equal': pk_equal, **({} if pk_equal else {'packets': packets}),
            'verdicts_equal': v_equal, **({} if v_equal else {'verdicts': verdicts}),
            **({'audio': au} if au else {})}


def _run_order(dirs, man):
    """The scenarios of an e2e run in the order run: an e2e --manifest's list, else e2e.log's 'scenario NAME' lines."""
    if man is not None:
        return list(man.get('order', [man.get('scenario')]))
    order = []
    for d in dirs:
        log = os.path.join(d, 'e2e.log')
        if os.path.exists(log):
            with open(log, encoding='utf-8', errors='replace') as f:
                for line in f:
                    p = line.split()
                    if len(p) >= 3 and p[1] == 'scenario' and p[2] not in order:
                        order.append(p[2])
    return order


def compute_noise(scen_dir):
    import e2e
    runs = stored_runs(scen_dir)
    name = os.path.basename(scen_dir)
    rule = (f'e2e.shot_diff raw (no shot_mask), {e2e.SHOT_GRID}x{e2e.SHOT_GRID} cells, a cell differs above '
            f'{e2e.SHOT_TOL} mean |difference| per channel, every {e2e.SHOT_STEP}nd pixel')
    mans = {r: read_json(os.path.join(scen_dir, r, 'manifest.json')) for r in runs}
    if any('set' in m for m in mans.values()):
        orders = {r: m.get('order', []) for r, m in mans.items()}
        names = []
        for o in orders.values():
            names += [n for n in o if n not in names]
        o_equal = len({json.dumps(o) for o in orders.values()}) <= 1
        return {'schema': SCHEMA, 'set': name, 'runs': runs, 'rule': rule,
                'order_equal': o_equal, **({} if o_equal else {'orders': orders}),
                'scenarios': {n: scenario_noise(run_entries(scen_dir, n)) for n in names}}
    return {'schema': SCHEMA, 'scenario': name, 'runs': runs, 'rule': rule, **scenario_noise(run_entries(scen_dir, name))}


def write_noise(scen_dir):
    nz = compute_noise(scen_dir)
    write_json(os.path.join(scen_dir, 'noise.json'), nz)
    return nz


def noise_summary(nz):
    def dl(v):
        d = v.get('drawlog')
        if not d:
            return ''
        n = d['draws'][0] if d['draws'][0] == d['draws'][1] else f'{d["draws"][0]}-{d["draws"][1]}'
        return f' ({n} draws, ' + ('identical)' if d['identical'] else f'{len(d["varying"])} fields vary)')

    def one(sn):
        s = ', '.join(f'{k} max {v["max_differ"]}/{v["cells"]}{dl(v)}' for k, v in sn['shots'].items()) or 'no shots'
        au = sn.get('audio')
        return s + ('; packets equal' if sn['packets_equal'] else '; PACKETS DIFFER') + \
            ('' if sn['verdicts_equal'] else '; VERDICTS DIFFER') + \
            ('' if not au else '; audio streams/3D ' + ('identical' if au['strict_identical'] else 'DIFFER')
             + ', 2D counts ' + ('equal' if au['loose_equal'] else 'vary'))
    if 'set' in nz:
        sc = nz['scenarios']
        worst = max((v['max_differ'] for s in sc.values() for v in s['shots'].values()), default=0)
        logs = [v['drawlog'] for s in sc.values() for v in s['shots'].values() if v.get('drawlog')]
        return (f'set of {len(sc)} scenarios, {sum(len(s["shots"]) for s in sc.values())} shots (max {worst}/256), '
                f'draw logs {sum(d["identical"] for d in logs)} of {len(logs)} identical; packets equal in '
                f'{sum(s["packets_equal"] for s in sc.values())} of {len(sc)}'
                + (f'; audio streams/3D identical in {sum(s["audio"]["strict_identical"] for s in sc.values() if s.get("audio"))}'
                   f' of {sum(1 for s in sc.values() if s.get("audio"))}, 2D counts equal in '
                   f'{sum(s["audio"]["loose_equal"] for s in sc.values() if s.get("audio"))}'
                   if any(s.get('audio') for s in sc.values()) else '; no audio')
                + ('' if nz.get('order_equal', True) else '; ORDER DIFFERS'))
    return one(nz)


# ---------------------------------------------------------------- audio (the mss32log proxy's audio.jsonl; test-plan C2)
#
# A session's audio.jsonl (e2e --audio) is cut per scenario by the results' tick_start / tick_end (GetTickCount, the
# log's "tick"): a scenario gets every event after the previous scenario's end up to its own end (the first one the
# client's start and the login too; what follows the last end, the client's stop, is left out). Each is stored as
# audio-NAME.json, compact and deterministic: the ordered open / start / stop / end / pause / resume / close events
# ([event, handle kind, sound, loop count at a start, frame, ms]), the sounds' names (a memory sample's `load` names
# its file) and per sound the count of every event kind; never the per-frame volume / position stream itself. The raw
# log is kept beside it only when under AUDIO_RAW_MAX. Compared: the streams' and 3D samples' sequence (kinds m, t)
# strictly, equal to one stored run's; the 2D samples (s: the UI sounds that follow the cursor's hovers) by count of
# each (event, sound) within the stored runs' range.

AUDIO_SEQ = ('open', 'start', 'stop', 'end', 'pause', 'resume', 'close')
AUDIO_STRICT = 'mt'             # handle kinds compared in order: streams, 3D samples
AUDIO_RAW_MAX = 1 << 20         # the raw audio.jsonl is stored only below this size


def _akind(h):
    return h[0] if isinstance(h, str) and h[:1].isalpha() else '?'


def audio_slices(events, results):
    """{scenario: its events} of one session's audio log; RESULTS [(name, result)] in the order run. A scenario
    whose result has no tick_end (an older e2e) gets the whole log when it is the session's only one, else none."""
    events = [e for e in events if e.get('ev') not in ('log', 'bad-line')]
    if len(results) == 1 and results[0][1].get('tick_end') is None:
        return {results[0][0]: events}
    out, lo = {}, None
    for name, res in results:
        hi = res.get('tick_end')
        if hi is None:
            continue
        out[name] = [e for e in events if (lo is None or e.get('tick', 0) > lo) and e.get('tick', 0) <= hi]
        lo = hi
    return out


def audio_summary(events, names):
    """The stored form of one scenario's audio events (audio-NAME.json)."""
    loop, seq, counts, used = {}, [], {}, set()
    for e in events:
        ev, h = e.get('ev'), e.get('h')
        snd = e.get('snd') or (e.get('file') if ev == 'open' else None)
        if ev == 'loop' and h:
            loop[h] = e.get('v')
        if snd:
            used.add(snd)
            c = counts.setdefault(snd, {})
            c[ev] = c.get(ev, 0) + 1
        if ev in AUDIO_SEQ:
            seq.append([ev, _akind(h), snd, loop.get(h) if ev == 'start' else None, e.get('f'), e.get('t')])
    return {'schema': 1, 'events': len(events), 'frames': any('f' in e for e in events),
            'ticks': [events[0].get('tick'), events[-1].get('tick')] if events else None,
            'sequence': seq, 'names': {k: names[k] for k in sorted(used) if k in names},
            'counts': {k: dict(sorted(v.items())) for k, v in sorted(counts.items())}}


def audio_names(events):
    """{'hash:size': file} from the memory samples' load events (their `type` is the file name)."""
    return {f'{e["hash"]}:{e["size"]}': e['type'] for e in events
            if e.get('ev') == 'load' and e.get('hash') and e.get('type')}


def session_audio(session, results):
    """{scenario: audio_summary} of SESSION's audio.jsonl, and the log's {bytes, sha256, events}; ({}, None)
    without one."""
    import audiolog
    path = os.path.join(session, audiolog.LOG_NAME)
    if not os.path.isfile(path):
        return {}, None
    events = audiolog.load(path)
    names = audio_names(events)
    out = {n: audio_summary(ev, names) for n, ev in audio_slices(events, results).items()}
    return out, {'bytes': os.path.getsize(path), 'sha256': sha256_file(path), 'events': len(events)}


def audio_strict(a):
    return [tuple(x[:4]) for x in a['sequence'] if x[1] in AUDIO_STRICT]


def audio_loose(a):
    c = {}
    for x in a['sequence']:
        if x[1] not in AUDIO_STRICT:
            k = f'{x[0]} {x[1]} {x[2]}'
            c[k] = c.get(k, 0) + 1
    return c


def _snd(a, snd):
    return a.get('names', {}).get(snd, snd)


def _audio_diff(a, b):
    """(index, text) of the first difference of two summaries' strict sequences; None when equal."""
    sa, sb = audio_strict(a), audio_strict(b)

    def item(x, src):
        return f'{x[0]} {x[1]} {_snd(src, x[2])}' + (f' loop {x[3]}' if x[3] is not None else '')
    for k, (x, y) in enumerate(zip(sa, sb)):
        if x != y:
            return k, f'item {k}: {item(x, a)} against {item(y, b)}'
    if len(sa) != len(sb):
        n = min(len(sa), len(sb))
        x, src = (sa[n], a) if len(sa) > n else (sb[n], b)
        return n, f'{len(sa)} items against {len(sb)}: item {n} {item(x, src)} only in ' +             ('the first' if src is a else 'the second')
    return None


def audio_first_diff(a, b):
    d = _audio_diff(a, b)
    return d and d[1]


def audio_noise(entries):
    """One scenario's audio over its stored runs (run_entries with an 'audio' file): whether the strict sequence
    was identical, each 2D (event, sound) count's range, the event counts."""
    have = [(en['run'], read_json(en['audio'])) for en in entries if en.get('audio')]
    if not have:
        return None
    first = have[0][1]
    diffs = [(r, audio_first_diff(first, a)) for r, a in have[1:]]
    diffs = [(r, d) for r, d in diffs if d]
    loose = [audio_loose(a) for _, a in have]
    keys = sorted(set().union(*loose))
    rng = {k: [min(c.get(k, 0) for c in loose), max(c.get(k, 0) for c in loose)] for k in keys}
    return {'runs': [r for r, _ in have], 'strict_kinds': AUDIO_STRICT,
            'strict_identical': not diffs, 'strict_items': sorted({len(audio_strict(a)) for _, a in have}),
            **({'strict_first_diff': {r: d for r, d in diffs}} if diffs else {}),
            'loose_equal': all(lo == hi for lo, hi in rng.values()), 'loose': rng,
            'events': [min(a['events'] for _, a in have), max(a['events'] for _, a in have)],
            'frames': all(a.get('frames') for _, a in have)}


def audio_verdict(mine, stored, nz):
    """(ok, text) for MINE (an audio_summary or None) against the stored runs' summaries: ok None when nothing is
    stored ("no audio stored") or the run has none."""
    if not stored:
        return None, 'no audio stored'
    if mine is None:
        return None, 'no audio in the run'
    st = audio_strict(mine)
    why = []
    if all(st != audio_strict(a) for a in stored):
        d = max((_audio_diff(mine, a) for a in stored), key=lambda kt: kt[0])     # the longest agreement
        why.append(f'streams/3D {d[1]} (run first, stored second)')
    rng = (nz or {}).get('loose')
    if rng is None:
        lo = [audio_loose(a) for a in stored]
        rng = {k: [min(c.get(k, 0) for c in lo), max(c.get(k, 0) for c in lo)] for k in set().union(*lo)}
    mc = audio_loose(mine)
    for k in sorted(set(rng) | set(mc)):
        lo, hi = rng.get(k, [0, 0])
        if not lo <= mc.get(k, 0) <= hi:
            ev, kind, snd = k.split(' ', 2)
            why.append(f'2D {ev} {_snd(mine, snd) if snd in mine.get("names", {}) else _snd(stored[0], snd)} '
                       f'x{mc.get(k, 0)}, stored {lo}' + (f'-{hi}' if hi != lo else ''))
            break
    if why:
        return False, '; '.join(why)
    return True, f'{len(st)} stream/3D items, {sum(mc.values())} 2D'


# ---------------------------------------------------------------- inputs: what a second device needs to repeat a run

INPUTS = 'inputs'
INPUTS_MANIFEST = 'inputs/manifest.json'
INPUT_SETS = ('captures', 'records', 'db')
# antprox /REPLAY's ids (antreplay's RecordingLibrary.Number: one entry per avatar of each recording under
# /RECORDINGS:, ordered by the avatar's login date). 'scenario': a scenario's comment names the id with the recording;
# 'rule': the date order (all five EA recordings are 2004, before the Net-7 ones), not observed in a log.
REPLAY_IDS = {
    'ea/alex3': ([1], 'scenario'),
    'session_2026-10-19_023139': ([6], 'rule'),
    'session_2026-10-19_115352': ([7], 'scenario'),
    'session_2026-10-21_111654': ([8, 9], 'scenario'),
    'csharp/session_2026-10-21_174700': ([10], 'scenario'),
    'ea/alex7': ([], 'rule: one of 2..5'),
    'ea/capture_1': ([], 'rule: one of 2..5'),
    'ea/capture_2': ([], 'rule: one of 2..5'),
    'ea/capture_3': ([], 'rule: one of 2..5'),
}
DB_ACCOUNT = 1                  # the test account (admin) whose row and avatars are the database state


def set_hash(entries):
    """The identity of a set of files: SHA-256 over sorted 'relative path TAB sha256' lines."""
    return hashlib.sha256(''.join(f'{p}\t{h}\n' for p, h in sorted(entries)).encode()).hexdigest()


def folder_files(top):
    """{path relative to top with '/': absolute path} of every file under top."""
    out = {}
    for d, dirs, files in os.walk(top):
        dirs.sort()
        for f in files:
            p = os.path.join(d, f)
            out[os.path.relpath(p, top).replace('\\', '/')] = p
    return out


def folder_set_hash(top):
    return set_hash((rel, sha256_file(p)) for rel, p in folder_files(top).items())


def scenario_refs():
    """What the scenarios (and gadgets.txt) name: ({replay id: [scenario]}, {record folder 'HHMMSS-record':
    [scenario]})."""
    import e2e
    import re
    plays, records = {}, {}
    for f in sorted(os.listdir(e2e.SCN)):
        if not f.endswith('.scn') and f != 'gadgets.txt':
            continue
        with open(os.path.join(e2e.SCN, f), encoding='utf-8') as fh:
            text = fh.read()
        name = f[:-4] if f.endswith('.scn') else f
        for m in sorted(set(re.findall(r"^text '/play (\d+)'", text, re.M))):
            plays.setdefault(int(m), []).append(name)
        for m in sorted(set(re.findall(r'(\d{6})-record', text))):
            records.setdefault(m, []).append(name)
    return plays, records


def build_inputs_manifest(store):
    top = os.path.join(store, INPUTS)
    plays, records = scenario_refs()
    files, sets = {}, {}
    for rel, p in sorted(folder_files(top).items()):
        if rel == 'manifest.json':
            continue
        e = {'sha256': sha256_file(p), 'bytes': os.path.getsize(p)}
        parts = rel.split('/')
        if parts[0] == 'captures' and rel.endswith('.pcapng'):
            key = rel[len('captures/'):-len('.pcapng')]
            ids, src = REPLAY_IDS.get(key, ([], 'unknown'))
            e.update(replay_ids=ids, replay_ids_from=src,
                     scenarios=sorted({s for i in ids for s in plays.get(i, [])}))
        elif parts[0] == 'records' and len(parts) == 3:
            e['scenarios'] = records.get(parts[1].split('-')[-1], [])
        files[rel] = e
    for s in INPUT_SETS:
        mine = {r: e for r, e in files.items() if r.startswith(s + '/')}
        sets[s] = {'files': len(mine), 'bytes': sum(e['bytes'] for e in mine.values()),
                   'sha256': set_hash((r[len(s) + 1:], e['sha256']) for r, e in mine.items())}
    return {'schema': SCHEMA,
            'rule': "files: every file under inputs/ but this one; sets.<set>.sha256: SHA-256 over sorted 'path TAB "
                    "sha256' lines, paths relative to the set's folder (captures' is the recordings folder's identity "
                    "a replay run's manifest records)",
            'sets': sets, 'files': files}


def check_inputs(store):
    """(manifest or None, {set: [problem]})."""
    path = os.path.join(store, INPUTS_MANIFEST)
    try:
        man = read_json(path)
    except (OSError, ValueError) as e:
        return None, {'manifest': [f'{INPUTS_MANIFEST} unreadable ({e})']}
    probs = {s: [] for s in INPUT_SETS}
    have = folder_files(os.path.join(store, INPUTS))
    for rel, info in sorted(man.get('files', {}).items()):
        s = rel.split('/')[0]
        p = have.get(rel)
        if p is None:
            probs.setdefault(s, []).append(f'{rel}: missing')
        elif sha256_file(p) != info['sha256']:
            probs.setdefault(s, []).append(f'{rel}: a Git LFS pointer, not the file: run git lfs pull'
                                           if is_lfs_pointer(p) else f'{rel}: SHA-256 differs from the manifest')
    for rel in sorted(have):
        if rel != 'manifest.json' and rel not in man.get('files', {}):
            probs.setdefault(rel.split('/')[0], []).append(f'{rel}: not in the manifest')
    return man, probs


def report_inputs(store):
    """Prints one line per input set; returns the number of sets with problems."""
    man, probs = check_inputs(store)
    bad = 0
    for s, ps in probs.items():
        info = (man or {}).get('sets', {}).get(s, {})
        print(f'inputs/{s}: {info.get("files", 0)} files, {info.get("bytes", 0) / 1e6:.1f} MB '
              + ('ok' if not ps else f'{len(ps)} problems'))
        for p in ps[:20]:
            print('    ' + p)
        bad += bool(ps)
    return bad


def db_state_files(store):
    """The database state's restore scripts in apply order: inputs/db/account*.sql, then avatar*.sql."""
    d = os.path.join(store, INPUTS, 'db')
    if not os.path.isdir(d):
        return []
    names = sorted(f for f in os.listdir(d) if f.endswith('.sql'))
    return [os.path.join(d, f) for f in names if f.startswith('account')] + \
        [os.path.join(d, f) for f in names if f.startswith('avatar')]


def scripts_hash(files):
    """One hash over {inputs path: sha256} (the restore scripts as applied, in order)."""
    return hashlib.sha256(''.join(f'{k} {v}\n' for k, v in files.items()).encode()).hexdigest()


def db_apply(store, log=print, restart=None, scripts=True):
    """Puts the store's database state into the local Net-7 database: in place through the control port, or with
    RESTART (True: always; None: e2e's default, ENB_NET7_RESTART) Net7.exe stopped and started again around it
    (e2e.db_restore). SCRIPTS False: no scripts, only the restart. Returns {'files': {inputs path: sha256},
    'ready': bool, 'server': {restore, seconds, db_scripts, status, control_port}}, or None when the store has no
    state to apply."""
    import e2e
    paths = db_state_files(store) if scripts else []
    if scripts and not paths:
        return None
    info = {}
    ok = e2e.db_restore(paths, machine.get('net7_bin'), log, tag='baseline: db', restart=restart, info=info)
    files = {os.path.relpath(p, store).replace('\\', '/'): sha256_file(p) for p in paths}
    return {'files': files, 'ready': ok,
            'server': {'restore': info.get('restore'), 'seconds': info.get('seconds'),
                       'db_scripts': scripts_hash(files) if files else None, 'status': info.get('status'),
                       'control_port': info.get('control_port'), 'failed': info.get('failed') and
                       os.path.relpath(info['failed'], store).replace('\\', '/')}}


def inputs_cmd(args):
    import e2e
    store = store_root(args)
    top = os.path.join(store, INPUTS)
    if args.save_db:
        d = os.path.join(top, 'db')
        print('baseline: ' + e2e.db_account_snapshot(DB_ACCOUNT, d))
        for av in e2e.db_account_avatars(DB_ACCOUNT):
            print('baseline: ' + e2e.db_snapshot(av, d, e2e.DB_AVATAR_TABLES))
    if args.save_db or args.write:
        man = build_inputs_manifest(store)
        write_json(os.path.join(store, INPUTS_MANIFEST), man)
        print(f'baseline: wrote {INPUTS_MANIFEST}: ' + ', '.join(f'{s} {v["files"]} files' for s, v in man['sets'].items()))
    if args.apply_db:
        r = db_apply(store, lambda m: print(m, flush=True))
        if r is None:
            print('baseline: no inputs/db/*.sql in the store')
            return 1
        if not r['ready']:
            return 1
    bad = report_inputs(store)
    if args.names:                      # list the named sets' files
        man = read_json(os.path.join(store, INPUTS_MANIFEST))
        for rel, e in man['files'].items():
            if rel.split('/')[0] in args.names:
                extra = (f' replay {e["replay_ids"]}' if 'replay_ids' in e else '') + \
                    (f' used by {", ".join(e["scenarios"])}' if e.get('scenarios') else '')
                print(f'  {rel}  {e["bytes"]}{extra}')
    return 1 if bad else 0


# ---------------------------------------------------------------- tools/antprox: the proxy pair the harness runs

TOOLS = 'tools/antprox'
TOOLS_MANIFEST = 'tools/antprox/manifest.json'


def tool_entry(src, dest):
    """The manifest row of DEST, a copy of SRC: version, SHA-256, size, where it came from and that repo's commit."""
    version = None
    if os.path.basename(dest).lower().startswith('antprox'):
        try:
            r = subprocess.run([dest, '/DESCRIBE'], capture_output=True, text=True, timeout=20)
            version = json.loads(r.stdout).get('version')
        except (OSError, ValueError, subprocess.SubprocessError, AttributeError):
            pass
    version = version or exe_version(dest)
    repo = commit = None
    try:
        g = subprocess.run(['git', '-C', os.path.dirname(os.path.abspath(src)), 'rev-parse', '--show-toplevel',
                            'HEAD'], capture_output=True, text=True)
        if g.returncode == 0:
            repo, commit = g.stdout.split()[:2]
    except OSError:
        pass
    return {'version': version, 'sha256': sha256_file(dest), 'bytes': os.path.getsize(dest),
            'copied_from': os.path.abspath(src), 'repo': repo and os.path.basename(os.path.normpath(repo)),
            'repo_head_when_copied': commit}


def check_tools(store):
    """[problem] of tools/antprox against its manifest (none when the store has no such folder)."""
    top = os.path.join(store, TOOLS)
    if not os.path.isdir(top):
        return []
    try:
        man = read_json(os.path.join(store, TOOLS_MANIFEST))
    except (OSError, ValueError) as e:
        return [f'{TOOLS_MANIFEST} unreadable ({e})']
    probs = []
    for f, info in sorted(man.get('files', {}).items()):
        p = os.path.join(top, f)
        if not os.path.isfile(p):
            probs.append(f'{f}: missing')
        elif sha256_file(p) != info['sha256']:
            probs.append(f'{f}: a Git LFS pointer, not the file: run git lfs pull' if is_lfs_pointer(p)
                         else f'{f}: SHA-256 differs from the manifest')
    for f in sorted(os.listdir(top)):
        if f != 'manifest.json' and f not in man.get('files', {}):
            probs.append(f'{f}: not in the manifest')
    return probs


def report_tools(store):
    if not os.path.isdir(os.path.join(store, TOOLS)):
        return 0
    probs = check_tools(store)
    n = len(os.listdir(os.path.join(store, TOOLS))) - 1
    print(f'{TOOLS}: {n} files ' + ('ok' if not probs else f'{len(probs)} problems'))
    for p in probs[:20]:
        print('    ' + p)
    return int(bool(probs))


def tools_cmd(args):
    """tools [--add EXE...]: copy each EXE into tools/antprox and record it in the manifest; then check."""
    store = store_root(args)
    top = os.path.join(store, TOOLS)
    if args.add:
        os.makedirs(top, exist_ok=True)
        mpath = os.path.join(store, TOOLS_MANIFEST)
        man = read_json(mpath) if os.path.isfile(mpath) else {
            'schema': SCHEMA, 'rule': 'the proxy executables the harness runs (machine.py antprox_dir); each file '
                                      'with its version, SHA-256, size, the path it was copied from and that '
                                      "repository's HEAD at the copy", 'files': {}}
        for src in args.add:
            dest = os.path.join(top, os.path.basename(src))
            if os.path.normcase(os.path.abspath(src)) != os.path.normcase(os.path.abspath(dest)):
                shutil.copy2(src, dest)
            man['files'][os.path.basename(src)] = tool_entry(src, dest)
            print(f'baseline: {TOOLS}/{os.path.basename(src)} {man["files"][os.path.basename(src)]["sha256"][:16]}')
        write_json(mpath, man)
    return 1 if report_tools(store) else 0


# ---------------------------------------------------------------- verify, list

def scenario_dirs(store):
    """[(client, env, scenario, folder)] of the store."""
    out = []
    runs = os.path.join(store, 'runs')
    for c in sorted(os.listdir(runs)) if os.path.isdir(runs) else []:
        for e in sorted(os.listdir(os.path.join(runs, c))):
            for s in sorted(os.listdir(os.path.join(runs, c, e))):
                d = os.path.join(runs, c, e, s)
                if os.path.isdir(d):
                    out.append((c, e, s, d))
    return out


def check_run_folder(run_dir):
    """Problems of one stored run folder against its manifest: [text]."""
    probs = []
    try:
        man = read_json(os.path.join(run_dir, 'manifest.json'))
    except (OSError, ValueError) as e:
        return [f'manifest.json unreadable ({e})']
    files = man.get('files') or {}
    for f, info in sorted(files.items()):
        p = os.path.join(run_dir, f)
        if not os.path.isfile(p):
            probs.append(f'{f}: missing')
        elif sha256_file(p) != info['sha256']:
            probs.append(f'{f}: a Git LFS pointer, not the file: run git lfs pull' if is_lfs_pointer(p)
                         else f'{f}: SHA-256 differs from the manifest (corrupt or changed)')
    for f in sorted(os.listdir(run_dir)):
        if f != 'manifest.json' and f not in files:
            probs.append(f'{f}: not in the manifest')
    return probs


def verify_store(args):
    store = store_root(args)
    if not os.path.isdir(os.path.join(store, 'runs')):
        print(f'baseline: no runs/ in {store}')
        return 1
    in_bad = 0
    if not args.names and os.path.isfile(os.path.join(store, INPUTS_MANIFEST)):
        in_bad = report_inputs(store)
    if not args.names:
        in_bad += report_tools(store)
    bad, n_runs, n_files = in_bad, 0, 0
    for c, e, s, d in scenario_dirs(store):
        if args.names and s not in args.names:
            continue
        runs = stored_runs(d)
        probs = []
        for r in runs:
            n_runs += 1
            n_files += len(read_json(os.path.join(d, r, 'manifest.json')).get('files', {}))
            probs += [f'{r}/{p}' for p in check_run_folder(os.path.join(d, r))]
        if not os.path.isfile(os.path.join(d, 'noise.json')):
            probs.append('noise.json missing')
        else:
            nz = read_json(os.path.join(d, 'noise.json'))
            if nz.get('runs') != runs:
                probs.append(f'noise.json covers {nz.get("runs")}, the stored runs are {runs}')
        print(f'{c}/{e}/{s}: {len(runs)} runs ' + ('ok' if not probs else f'{len(probs)} problems'))
        for p in probs[:20]:
            print('    ' + p)
        bad += bool(probs)
    print(f'baseline verify: {n_runs} runs, {n_files} files; ' + ('all match' if not bad else
                                                                  f'{bad} scenarios or input sets with problems'))
    return 1 if bad else 0


def _run_input(run):
    """RUN -> (e2e run folders, a stored manifest or None)."""
    import e2e
    if os.path.basename(run) == 'manifest.json' or os.path.isfile(os.path.join(run, 'manifest.json')):
        d = os.path.dirname(run) if run.endswith('manifest.json') else run
        return [d], read_json(os.path.join(d, 'manifest.json'))
    return e2e.run_dirs(run), None


# The top-level folders the client build is linked from (build.py: app plus the EA-derived and E&B libraries,
# programs/client, the stand-ins): a commit to any of them after an exe's link makes that exe stale
CLIENT_SOURCES = ('app', 'binkmovie', 'libs', 'programs', 'stubs', 'ww3d2', 'wwaudio', 'wwdebug', 'wwlib', 'wwmath',
                  'wwsaveload', 'wwshade')
_EXE_RE = re.compile(r'; exe (.+?), instance ')


def run_exe(dirs):
    """The client exe an e2e run started, from its e2e.log's session line -> (path, retail) or (None, False)."""
    for d in dirs:
        p = os.path.join(d, 'e2e.log')
        if not os.path.isfile(p):
            continue
        with open(p, encoding='utf-8', errors='replace') as f:
            head = list(itertools.islice(f, 40))
        for ln in head:
            m = _EXE_RE.search(ln)
            if m:
                retail = any('(the retail client)' in x for x in head)
                return m.group(1), retail
    return None, False


def pe_link_time(path):
    """PATH's link time -> (epoch seconds, 'pe' | 'mtime'): the PE header's TimeDateStamp, else the file's mtime."""
    with open(path, 'rb') as f:
        head = f.read(4096)
    if head[:2] == b'MZ' and len(head) >= 0x40:
        off = struct.unpack_from('<I', head, 0x3C)[0]
        if off + 12 <= len(head) and head[off:off + 4] == b'PE\0\0':
            t = struct.unpack_from('<I', head, off + 8)[0]
            if t:
                return t, 'pe'
    return int(os.path.getmtime(path)), 'mtime'


def source_commit_times(root=ROOT):
    """Commit times (newest first) of every commit touching CLIENT_SOURCES in ROOT's checkout."""
    r = subprocess.run(['git', 'log', '--format=%ct', 'HEAD', '--'] + list(CLIENT_SOURCES), cwd=root,
                       capture_output=True, text=True)
    return [int(x) for x in r.stdout.split()] if r.returncode == 0 else []


def _when(t):
    return datetime.datetime.fromtimestamp(t).strftime('%Y-%m-%d %H:%M:%S')


def build_guard(exe, retail, allow_stale, times=None, out=print):
    """verify --run's staleness guard for a run of our build: prints the exe's link time and SHA-256 and, when the
    tree's client sources have a commit newer than the link, a first line 'STALE BUILD: ...'. Returns True when
    verify may go on (retail, fresh, or --allow-stale)."""
    if retail:
        if exe and os.path.isfile(exe):
            t, how = pe_link_time(exe)
            out(f'exe: {exe} (retail), linked {_when(t)} ({how}), sha256 {sha256_file(exe)}')
        return True
    if not exe:
        out('exe: not named in the run (no e2e.log session line): build age not checked')
        return True
    if not os.path.isfile(exe):
        out(f'STALE BUILD: exe {exe} not found: link time unknown')
        return bool(allow_stale)
    t, how = pe_link_time(exe)
    times = source_commit_times() if times is None else times
    newer = [c for c in times if c > t]
    if newer:
        out(f'STALE BUILD: exe linked {_when(t)}, sources changed {_when(max(newer))} ({len(newer)} commits since)')
    out(f'exe: {exe}, linked {_when(t)} ({how}), sha256 {sha256_file(exe)}'
        + (f'; newest source commit {_when(times[0])}' if times else ''))
    return not newer or bool(allow_stale)


def verify_run(args):
    import e2e
    store = store_root(args)
    dirs, man = _run_input(args.run)
    if man is None:                     # an e2e run (a stored run is retail)
        exe, retail = run_exe(dirs)
        if not build_guard(exe, retail, getattr(args, 'allow_stale', False)):
            return 2
    client = args.client or (man and man['client']['name'])
    env = args.env or (man and man['env'])
    results, packets, shots = e2e.run_results(dirs), e2e.run_packets(dirs), e2e.run_shots(dirs)
    for d in dirs:                      # a stored run's packets.json is the same reading, kept
        if os.path.isfile(os.path.join(d, 'packets.json')):
            packets.update(read_json(os.path.join(d, 'packets.json')))
    names = [n for n in results if not args.names or n in args.names]
    if not names:
        print(f'baseline: no result-*.json in {args.run}')
        return 1
    order = _run_order(dirs, man)
    mine_audio = {}
    if man is not None:                 # a stored run: its audio-NAME.json files
        for en in (man.get('scenarios') or [dict(man, name=man.get('scenario'))]):
            if en.get('audio'):
                mine_audio[en['name']] = read_json(os.path.join(dirs[0], en['audio']))
    else:
        for d in dirs:
            here = [(n, r) for n, r in results.items() if os.path.normcase(os.path.abspath(r.get('out') or ''))
                    == os.path.normcase(os.path.abspath(d))] or ([(n, r) for n, r in results.items()]
                                                                if len(dirs) == 1 else [])
            here.sort(key=lambda nr: order.index(nr[0]) if nr[0] in order else 1 << 20)
            mine_audio.update(session_audio(d, here)[0])
    holders = [(c, e, s, d, dir_scenarios(d)) for c, e, s, d in scenario_dirs(store)
               if (not client or c == client) and (not env or e == env)]
    bad = 0
    for name in names:
        # a set folder when --set names it; else a run of several scenarios prefers a set that ran this one, and a
        # run of one scenario the scenario's own folder
        cands = [h for h in holders if name in h[4] and (not args.set or h[2] == args.set)]
        if not args.set and len(cands) > 1:
            pref = [h for h in cands if (len(h[4]) > 1) == (len(results) > 1)]
            cands = pref or cands
        if len(cands) != 1:
            print(f'{name}: DIFF ' + ('no stored baseline' if not cands else
                                      f'{len(cands)} baselines ({", ".join(h[0] + "/" + h[1] + "/" + h[2] for h in cands)})'
                                      ': give --client, --env or --set'))
            bad += 1
            continue
        c, e, s, d, held = cands[0]
        ents = run_entries(d, name)
        nz = read_json(os.path.join(d, 'noise.json'))
        nzs = nz['scenarios'].get(name, {}) if 'set' in nz else nz
        what, notes = [], []
        v = e2e.verdict(results[name])
        stored_v = sorted({en['verdict'] for en in ents})
        if v not in stored_v:
            what.append(f'verdict {v}, stored {"/".join(stored_v)}')
        mine = packets.get(name, [])
        theirs = [en['packets'] for en in ents]
        if mine not in theirs:
            what.append(f'packets {len(mine)}, stored {"/".join(str(len(t)) for t in theirs)} (equal to none)')
        stored_order = ents[-1]['order'] if ents else []
        if len(held) > 1 or len(order) > 1:
            before_mine = order[:order.index(name)] if name in order else None
            before_theirs = stored_order[:stored_order.index(name)] if name in stored_order else None
            if before_mine != before_theirs:
                notes.append(f'run after {before_mine[-1] if before_mine else "the login"}, stored after '
                             f'{before_theirs[-1] if before_theirs else "the login"} (scenario order differs)')
        stored_shots, stored_logs = {}, {}
        for en in ents:
            for key, f in en['shots']:
                stored_shots.setdefault(key, []).append(os.path.join(en['dir'], f))
            for key, f in en['drawlogs']:
                stored_logs.setdefault(key, []).append(os.path.join(en['dir'], f))
        mine_shots = dict(shot_keys(shots.get(name, [])))
        logs_same, logs_none = 0, 0
        for key in sorted(mine_shots):
            ok, text = drawlog_verdict(drawlog_path(mine_shots[key]), stored_logs.get(key, []),
                                       nzs.get('shots', {}).get(key, {}).get('drawlog'))
            if ok is None:
                logs_none += 1
            elif ok:
                logs_same += 1
            else:
                what.append(f'draw log {key}: {text}')
        within = 0
        for key in sorted(set(stored_shots) | set(mine_shots)):
            if key not in mine_shots or key not in stored_shots:
                what.append(f'shot {key} ' + ('not taken' if key not in mine_shots else 'not in the baseline'))
                continue
            ptrs = [p for p in stored_shots[key] if is_lfs_pointer(p)]
            if ptrs:
                what.append(f'shot {key}: stored file is a Git LFS pointer: run git lfs pull')
                continue
            best = min(e2e.shot_diff(mine_shots[key], p)['differ'] for p in stored_shots[key])
            limit = nzs.get('shots', {}).get(key, {}).get('max_differ', 0)
            if best > limit:
                what.append(f'shot {key} {best} cells (noise max {limit})')
            else:
                within += 1
        a_ok, a_text = audio_verdict(mine_audio.get(name), [read_json(en['audio']) for en in ents if en.get('audio')],
                                     nzs.get('audio'))
        if a_ok is False:
            what.append(f'audio {a_text}')
        line = f'{len(mine)} packets, shots {within} of {len(set(stored_shots) | set(mine_shots))} within noise'
        line += (f', draw logs {logs_same} of {len(mine_shots)} the same' if logs_same or not logs_none else '') + \
            (f', no draw log stored for {logs_none}' if logs_none else '')
        line += f', audio the same ({a_text})' if a_ok else f', {a_text}'
        print(f'{name}: {"DIFF " + "; ".join(what) if what else "SAME " + line}'
              + (f' ({"; ".join(notes)})' if notes else '') + f'  [{c}/{e}/{s}, {len(ents)} runs]')
        bad += bool(what)
    print(f'baseline verify --run: {len(names) - bad} of {len(names)} scenarios the same as the baseline')
    return 1 if bad else 0


def list_store(args):
    store = store_root(args)
    rows = scenario_dirs(store)
    if not rows:
        print(f'baseline: {store}: no stored runs')
        return 0
    for c, e, s, d in rows:
        runs = stored_runs(d)
        nzp = os.path.join(d, 'noise.json')
        summ = noise_summary(read_json(nzp)) if os.path.exists(nzp) else 'no noise.json'
        print(f'{c}  {e}  {s}: {len(runs)} runs ({runs[0]} .. {runs[-1]}); {summ}' if runs else f'{c} {e} {s}: 0 runs')
    return 0


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    ap.add_argument('command', choices=['capture', 'verify', 'list', 'inputs', 'tools'])
    ap.add_argument('names', nargs='*')
    ap.add_argument('--store', help="the store's checkout (default machine.py baselines)")
    ap.add_argument('--runs', type=int, default=3, help='capture: PASS runs to store per scenario (default 3)')
    ap.add_argument('--retries', type=int, default=1, help='capture: failed attempts allowed before giving up (1)')
    ap.add_argument('--live', nargs='?', const='localhost', metavar='SERVER', help='capture: e2e --live [SERVER]')
    ap.add_argument('--db-snapshot', nargs='?', const=6, type=int, metavar='AVATAR', help='capture: e2e --db-snapshot')
    ap.add_argument('--restart', action=argparse.BooleanOptionalAction, default=None,
                    help='capture --live: restart Net7.exe before each run (default: when a scenario is sp_*; '
                         '--no-restart: restore in place)')
    ap.add_argument('--no-audio', action='store_true', help='capture: without the audio log (e2e --audio is on)')
    ap.add_argument('--no-db-restore', action='store_true',
                    help="capture --live: do not put the store's database state (inputs/db) back before each run")
    ap.add_argument('--write', action='store_true', help='inputs: rewrite inputs/manifest.json from the files')
    ap.add_argument('--save-db', action='store_true',
                    help="inputs: dump the test account's row and its avatars into inputs/db (then --write)")
    ap.add_argument('--apply-db', action='store_true',
                    help="inputs: put inputs/db's state into the local Net-7 database (Net7.exe stopped, restarted)")
    ap.add_argument('--add', nargs='+', metavar='EXE', help='tools: copy EXE into tools/antprox and record it')
    ap.add_argument('--instance', default='bl', help='capture: the clientrun.py instance (default bl)')
    ap.add_argument('--run', help='verify: an e2e run (folder, folders joined by ",", e2e manifest) or a stored manifest')
    ap.add_argument('--client', help='verify --run: the stored client (retail-XXXXXXXX)')
    ap.add_argument('--env', help='verify --run: the stored env (<label>-<W>x<H>)')
    ap.add_argument('--allow-stale', action='store_true',
                    help='verify --run: go on when our exe is older than the newest client-source commit')
    ap.add_argument('--set', help='capture: the scenarios as ONE e2e session a run, stored as this set (no names: '
                                  "e2e's default set); verify --run: compare with this stored set")
    args = ap.parse_args(argv)
    if args.command == 'capture':
        if not args.names and not args.set:
            ap.error('capture NAME... | capture --set SET [NAME...]')
        return capture(args)
    if args.command == 'verify':
        return verify_run(args) if args.run else verify_store(args)
    if args.command == 'inputs':
        return inputs_cmd(args)
    if args.command == 'tools':
        return tools_cmd(args)
    return list_store(args)


if __name__ == '__main__':
    sys.exit(main())
