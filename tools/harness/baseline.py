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
    manifest.json, e2e.log, events.log, antprox.out, d3d8shot.log, camera.txt, camera-track.txt (capture
    --camera-track), messages.log (if any; filtered
    above MESSAGES_RAW_MAX, the manifest's messages_log),
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
verify --run compares an e2e run (a session folder, the e2e --out folder: its newest run's session folders, several
joined by ',', an e2e --manifest file, or a stored run's manifest.json) with the stored runs of the same client and
env: the verdict, the packets (equal to at least one stored run's) and each shot (its differing cells against the
best-matching stored run at most noise.json's max for that shot). One line per scenario, SAME, DIFF, NOT COMPARABLE
or RETAIL-UNSTABLE (the only differences are audio streams / 3D sounds or draw logs the stored retail runs disagree
on among themselves, leave one out); then every scenario not compared, and why: the run's left out by NAME
arguments, a stored set's the run has no result for (SKIP under --set). Exit 1 on any DIFF or problem, or a SKIP;
2 a stale build; 4 RETAIL-UNSTABLE (no DIFF); 3 not comparable (neither); 0 all SAME.

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

try:
    import numpy as _np                         # optional: the multiset compare's large groups
except ImportError:
    _np = None

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(os.path.dirname(HERE))
sys.path.insert(0, HERE)
import machine                                  # noqa: E402

SCHEMA = 1
LOGS = ('e2e.log', 'events.log', 'antprox.out', 'd3d8shot.log', 'camera.txt', 'camera-track.txt', 'messages.log')
# messages.log (the client's -ENABLE_MESSAGES stream) is stored as it is only up to MESSAGES_RAW_MAX; a larger one (a
# space session's: 49-58 MB, 99 % effect_description.cpp's three asserts a frame and frameint.cpp's frame-rate line)
# is stored filtered (filter_messages: the frame-rate lines dropped, an assert repeated since the last other line
# folded to one '[repeated N x]' line) and cut at MESSAGES_MAX; the full log stays in the e2e session folder
# (.staging, git-ignored) and the manifest's messages_log gives its size, SHA-256 and what was done.
MESSAGES_RAW_MAX = 1 << 20
MESSAGES_MAX = 4 << 20
MESSAGES_DROP = re.compile(r'^STAT\s+Render: Frame Rate =')
MESSAGES_TS = re.compile(r'^(\d\d:\d\d:\d\d\.\d+)\s+')
MESSAGES_ASSERT = re.compile(r'^ERROR\s+(?:G_ASSERT|FAIL_IF):\s')
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


def filter_messages(src, dst, limit=MESSAGES_MAX):
    """Writes DST, the stored copy of the messages.log SRC: every line but frameint.cpp's per-frame 'Render: Frame
    Rate' (dropped, counted), and a G_ASSERT / FAIL_IF line whose text (after the timestamp) already came since the
    last other kept line folded: at that next line (or the end) one '<last time> [repeated N x] <text>' line per
    folded text, in the order first seen. Cut at LIMIT bytes on a line boundary with a '[cut: ...]' line. Returns
    {'bytes', 'sha256', 'lines' of SRC, 'stored_bytes', 'dropped', 'folded', 'cut'}."""
    h = hashlib.sha256()
    info = {'bytes': 0, 'lines': 0, 'dropped': 0, 'folded': 0, 'cut': False}
    pending = {}                    # assert text -> [repeats, last timestamp], since the last other line
    out_bytes = [0]
    cut_lines = [0]

    def put(fo, s):
        b = s.encode('utf-8', 'replace')
        if info['cut'] or out_bytes[0] + len(b) > limit:
            info['cut'] = True
            cut_lines[0] += 1
            return
        fo.write(b)
        out_bytes[0] += len(b)

    def flush(fo):
        for text, (n, ts) in pending.items():
            if n:
                put(fo, f'{ts} [repeated {n} x] {text}\n')
        pending.clear()

    with open(src, 'rb') as fi, open(dst, 'wb') as fo:
        for raw in fi:
            h.update(raw)
            info['bytes'] += len(raw)
            info['lines'] += 1
            line = raw.decode('utf-8', 'replace').rstrip('\r\n')
            m = MESSAGES_TS.match(line)
            ts, body = (m.group(1), line[m.end():]) if m else ('', line)
            if MESSAGES_DROP.match(body):
                info['dropped'] += 1
                continue
            if MESSAGES_ASSERT.match(body):
                if body in pending:
                    pending[body][0] += 1
                    pending[body][1] = ts
                    info['folded'] += 1
                    continue
                pending[body] = [0, ts]
            else:
                flush(fo)
            put(fo, line + '\n')
        flush(fo)
        if info['cut']:
            tail = f'[cut: {cut_lines[0]} more lines past {limit} bytes; the full log is the e2e session\'s]\n'
            fo.write(tail.encode('utf-8'))
            out_bytes[0] += len(tail)
    info['sha256'] = h.hexdigest()
    info['stored_bytes'] = out_bytes[0]
    return info


def store_messages(session, dest):
    """messages.log of SESSION into DEST: as it is up to MESSAGES_RAW_MAX, else filter_messages' copy. Returns the
    manifest's messages_log ({'bytes', 'sha256', 'kept': 'full' | 'filtered', ...}) or None without a log."""
    src = os.path.join(session, 'messages.log')
    if not os.path.exists(src):
        return None
    if os.path.getsize(src) <= MESSAGES_RAW_MAX:
        shutil.copy2(src, os.path.join(dest, 'messages.log'))
        return {'bytes': os.path.getsize(src), 'sha256': sha256_file(src), 'kept': 'full'}
    info = filter_messages(src, os.path.join(dest, 'messages.log'))
    info['kept'] = 'filtered'
    return info


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
        if f != 'messages.log' and os.path.exists(os.path.join(session, f)):
            shutil.copy2(os.path.join(session, f), os.path.join(dest, f))
    for f in sorted(os.listdir(session)):   # e2e db-avatars dumps (UI-01: the created character as stored)
        if re.fullmatch(r'db-[A-Za-z0-9_-]+\.sql', f):
            shutil.copy2(os.path.join(session, f), os.path.join(dest, f))
    mlog = store_messages(session, dest)
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
    if mlog:
        man['messages_log'] = mlog
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
        'server': dict({'mode': 'live', 'server': args.live}, **({'antprox_args': args.antprox_arg}
                                                                 if args.antprox_arg else {}))
        if args.live else {'mode': 'replay'},
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
                cmd += ['--live', args.live] + [f'--antprox-arg={a}' for a in args.antprox_arg or []]
            if not args.no_audio:
                cmd += ['--audio']
            if args.camera_track:
                cmd += ['--camera-track']
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
                import e2e
                sql = os.path.join(staging, snaps[-1], e2e.db_snapshot_file(args.db_snapshot)) if snaps else None
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
    fields, first, deltas, bad, field_draws = {}, None, [], {}, []
    mat = {k: 0.0 for k in DRAW_MATRICES}
    for i, j in pairs:
        fa, fb = a['draws'][i][0], b['draws'][j][0]
        diff = sorted(k for k in set(fa) | set(fb) if fa.get(k) != fb.get(k))
        for k in diff:
            fields.setdefault(k, i)
            field_draws.append((i, k))
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
            'fields': fields, 'field_draws': field_draws, 'pairs': pairs, 'first': first, 'matrix': mat,
            'deltas': deltas, 'bad': bad, 'sets': kinds_same}


# The multiset compare (2026-11-25): a scene that moves (a playback's asteroids switching LOD, ships changing the draw
# order) reads as DIFF in the ordered compare above although every state, texture and object is there. Here a frame
# is a multiset of draw states (every flat field but the LOD's prims and verts). A state of the run that no stored
# run has passes when its nearest stored state differs only in fields that differ so between the stored runs
# themselves (the same rule the other way for a stored state the run lacks); each state's draws are paired with the
# stored run's nearest first by world translation, and each pair's world matrices must be within that stored draw's
# own spread over the other stored runs (times DRAW_MATRIX_SLACK, plus DRAW_MATRIX_FLOOR: a still draw stays still);
# the draw counts per state, the textures as a set and the frame's views and projections within the stored runs'
# largest difference among themselves (the counts and matrices times DRAW_MATRIX_SLACK). Each measure is taken
# against the stored run the run is closest to in it.
MULTISET_LOD = ('prims', 'verts')
_STATE_HEAD = ('call', 'prim', 'fvf', 'stride', 'tex0.w', 'tex0.h', 'tex0.format', 'tex1.w', 'tex1.h', 'tex1.format')


def _state(flat):
    return tuple(sorted((k, v) for k, v in flat.items() if k not in MULTISET_LOD))


def _texture(flat, s):
    v = tuple(flat.get(f'tex{s}.{k}') for k in TEX_FIELDS)
    return None if all(x is None for x in v) else v


def _ms_prep(log):
    """A loaded draw log -> {'groups': {state: [(draw index, world)]}, 'tex': set, 'cams': {kind: [matrix]}}."""
    from collections import defaultdict
    if '_ms' in log:
        return log['_ms']
    groups = defaultdict(list)
    for i, (f, m) in enumerate(log['draws']):
        groups[_state(f)].append((i, m['world'], (f.get('prims'), f.get('verts'))))
    tex = {t for f, _ in log['draws'] for t in (_texture(f, 0), _texture(f, 1)) if t}
    cams = {}
    for kind in ('view', 'proj'):
        xs = [m for k, m in log['sets'] if k == kind and m is not None]
        cams[kind] = [(n, m, None) for n, m in enumerate(xs) if m not in xs[:n]]
    log['_ms'] = {'groups': dict(groups), 'tex': tex, 'cams': cams}
    return log['_ms']


def _pair_nearest(xa, xb):
    """Two lists of (key, matrix, lod) -> [(key a, key b, element difference)]: the draws of the same LOD (prims,
    verts: the same mesh) paired nearest first by translation, then the rest nearest first (an object that switched
    its LOD)."""
    def tr(m):
        return m[12:15] if m and len(m) >= 15 and None not in m[12:15] else (0.0, 0.0, 0.0)
    if len(xa) * len(xb) > 400 and _np is not None:      # numpy's order: the same (lod, distance, i, j) keys
        ta, tb = _np.array([tr(x[1]) for x in xa], float), _np.array([tr(x[1]) for x in xb], float)
        dist = ((ta[:, None, :] - tb[None, :, :]) ** 2).sum(axis=2)
        lod = _np.array([[x[2] != y[2] for y in xb] for x in xa])
        ii, jj = _np.meshgrid(_np.arange(len(xa)), _np.arange(len(xb)), indexing='ij')
        order = _np.lexsort((jj.ravel(), ii.ravel(), dist.ravel(), lod.ravel()))
        cand = zip(ii.ravel()[order].tolist(), jj.ravel()[order].tolist())
    else:
        cand = ((i, j) for _, _, i, j in sorted(
            (xa[i][2] != xb[j][2], sum((x - y) ** 2 for x, y in zip(tr(xa[i][1]), tr(xb[j][1]))), i, j)
            for i in range(len(xa)) for j in range(len(xb))))
    used_a, used_b, out = set(), set(), []
    want = min(len(xa), len(xb))
    for i, j in cand:
        if i in used_a or j in used_b:
            continue
        used_a.add(i)
        used_b.add(j)
        out.append((xa[i][0], xb[j][0], _mdelta(xa[i][1], xb[j][1])))
        if len(out) == want:
            break
    return out


def _state_fields(a, b):
    return [k for (k, x), (_, y) in zip(a, b) if x != y]


def _nearest_fields(state, pool):
    """The fields in which `state` differs from its nearest state of `pool` (same field names; the same draw call,
    primitive, FVF and textures' sizes first), or None when pool has no state with the same fields."""
    names = [k for k, _ in state]
    head = {k: v for k, v in state if k in _STATE_HEAD}
    best = None
    for near in (True, False):
        for s in pool:
            if [k for k, _ in s] != names or (near and any(dict(s).get(k) != v for k, v in head.items())):
                continue
            d = _state_fields(state, s)
            if best is None or len(d) < len(best):
                best = d
        if best is not None:
            return best
    return best


def _pair_stats(px, py):
    """What the multiset compare needs of two prepared logs, kept in px for py (a leave-one-out asks again):
    'missing' {state of px not in py: _nearest_fields}, 'draws', 'textures', 'cams', 'pairs' [(draw of px, draw of py,
    world difference)]."""
    memo = px.setdefault('memo', {})
    if id(py) not in memo:
        gx, gy = px['groups'], py['groups']
        back = py.get('memo', {}).get(id(px))
        memo[id(py)] = {
            'missing': {st: _nearest_fields(st, gy) for st in gx if st not in gy},
            'draws': back['draws'] if back else _count_distance(px, py),
            'textures': len(px['tex'] ^ py['tex']),
            'cams': back['cams'] if back else _cams_distance(px, py),
            'pairs': [(i, j, d) for j, i, d in back['pairs']] if back else
            [(i, j, d) for st in set(gx) & set(gy) for i, j, d in _pair_nearest(gx[st], gy[st])]}
    return memo[id(py)]


def _odd_states(px, py, varying):
    """States of px missing from py whose nearest py state differs in a field outside `varying` -> [(state, fields)]."""
    return [(st, d) for st, d in _pair_stats(px, py)['missing'].items() if d is None or not set(d) <= varying]


def multiset_noise(stored):
    """Loaded stored logs -> what they differ by among themselves: 'varying' (fields in which a state of one differs
    from the nearest state of another), 'draws' (the largest multiset distance of the draw states), 'textures' (the
    largest number of textures in one and not the other), 'cams' (the largest view / projection difference), 'spread'
    ([{draw index: the largest world difference to its partner in another run}] per log)."""
    preps = [_ms_prep(lg) for lg in stored]
    nz = {'varying': set(), 'draws': 0, 'textures': 0, 'cams': 0.0, 'spread': [{} for _ in stored]}
    for x, y in itertools.permutations(range(len(stored)), 2):
        ps = _pair_stats(preps[x], preps[y])
        for d in ps['missing'].values():
            nz['varying'] |= set(d or [])
        if x > y:
            continue
        nz['draws'] = max(nz['draws'], ps['draws'])
        nz['textures'] = max(nz['textures'], ps['textures'])
        nz['cams'] = max(nz['cams'], ps['cams'])
        for i, j, d in ps['pairs']:
            nz['spread'][x][i] = max(nz['spread'][x].get(i, 0.0), d)
            nz['spread'][y][j] = max(nz['spread'][y].get(j, 0.0), d)
    return nz


def _count_distance(pa, pb):
    return sum(abs(len(pa['groups'].get(s, ())) - len(pb['groups'].get(s, ())))
               for s in set(pa['groups']) | set(pb['groups']))


def _cams_distance(pa, pb):
    return max((d for kind in ('view', 'proj') for _, _, d in
                _pair_nearest(pa['cams'][kind], pb['cams'][kind])), default=0.0)


def _state_desc(state):
    f = dict(state)
    t = f"tex {f['tex0.w']}x{f['tex0.h']} fmt {f['tex0.format']}" if f.get('tex0.w') is not None else 'no texture'
    fvf = f'{f["fvf"]:#x}' if isinstance(f.get('fvf'), int) else f.get('fvf')
    return f"{f.get('call')} prim {f.get('prim')}, fvf {fvf}, {t}"


def multiset_measures(mine, stored, nz):
    """The run's loaded draw log against each loaded stored one -> one dict per stored log: 'extra' / 'missing'
    (_odd_states both ways), 'draws', 'textures', 'cams', 'worlds' ([(draw of mine, draw of the stored log,
    difference, allowed)] of the pairs over their allowance)."""
    pm = _ms_prep(mine)
    out = []
    for k, lg in enumerate(stored):
        pk = _ms_prep(lg)
        ps = _pair_stats(pm, pk)
        worlds = []
        for i, j, d in ps['pairs']:
            allowed = DRAW_MATRIX_SLACK * nz['spread'][k].get(j, 0.0) + DRAW_MATRIX_FLOOR
            if d > allowed:
                worlds.append((i, j, d, allowed))
        out.append({'extra': _odd_states(pm, pk, nz['varying']), 'missing': _odd_states(pk, pm, nz['varying']),
                    'draws': ps['draws'], 'textures': ps['textures'], 'cams': ps['cams'], 'worlds': sorted(worlds)})
    return out


def _ms_score(mine, stored):
    """The run's log against the stored ones: each measure against the stored log nearest in it -> {'states': odd
    states both ways, 'textures', 'draws', 'cams', 'worlds': draws out of their stored spread, 'detail': ...}."""
    nz = multiset_noise(stored)
    ms = multiset_measures(mine, stored, nz)
    st = min(ms, key=lambda m: len(m['extra']) + len(m['missing']))
    wo = min(ms, key=lambda m: len(m['worlds']))
    return {'states': len(st['extra']) + len(st['missing']), 'textures': min(m['textures'] for m in ms),
            'draws': min(m['draws'] for m in ms), 'cams': min(m['cams'] for m in ms), 'worlds': len(wo['worlds']),
            'st': st, 'wo': wo}


MULTISET_MEASURES = ('states', 'textures', 'draws', 'cams', 'worlds')


def _ms_score(mine, stored):
    """The run's log against the stored ones, each measure against the stored log nearest in it -> {'states': odd
    states both ways, 'textures', 'draws', 'cams', 'worlds': draws out of their stored spread, 'still': those of
    them whose stored draw never moved between the stored runs, 'nz': multiset_noise, 'st' / 'wo': the details}."""
    nz = multiset_noise(stored)
    ms = multiset_measures(mine, stored, nz)
    st = min(ms, key=lambda m: len(m['extra']) + len(m['missing']))
    wo = min(ms, key=lambda m: len(m['worlds']))
    still = min(sum(1 for _, j, _, lim in m['worlds'] if lim <= DRAW_MATRIX_FLOOR) for m in ms)
    return {'states': len(st['extra']) + len(st['missing']), 'textures': min(m['textures'] for m in ms),
            'draws': min(m['draws'] for m in ms), 'cams': min(m['cams'] for m in ms), 'worlds': len(wo['worlds']),
            'still': still, 'nz': nz, 'st': st, 'wo': wo}


LEAVE_ONE_OUT = ('cams', 'worlds', 'still')


def multiset_verdict(mine, stored, reasons=None):
    """A run's loaded draw log against the loaded stored ones, as multisets -> (True | False, text). No draw state
    may be odd (_odd_states); the textures and the draw counts per state may differ from the nearest stored run as
    much as two stored runs differ (the counts times DRAW_MATRIX_SLACK); the views' difference and the number of
    draws out of their stored spread (and of those, draws that never moved in the stored runs) may be as large as a
    stored run's own against the other stored runs (leave one out: a playback's objects move by the shot's timing,
    so retail has draws out of the spread two runs measure too); with one stored run all must be zero. REASONS (a
    set) gets what failed: 'states:FIELD' for each field an odd state differs in from its nearest stored one
    ('states:new STATE' when none has its fields), 'textures', 'draws', 'cams', 'worlds'."""
    score = _ms_score(mine, stored)
    why = set()
    nz = score['nz']
    allowed = {k: 0 for k in LEAVE_ONE_OUT}
    if len(stored) > 1:
        for k in range(len(stored)):
            own = _ms_score(stored[k], stored[:k] + stored[k + 1:])
            for m in LEAVE_ONE_OUT:
                allowed[m] = max(allowed[m], own[m])
    parts = []
    if score['states']:
        st = score['st']
        s, d = (st['extra'] or st['missing'])[0]
        for s1, d1 in st['extra'] + st['missing']:
            why |= {f'states:{f}' for f in d1} if d1 else {f'states:new {_state_desc(s1)}'}
        parts.append(f'{len(st["extra"])} draw states not stored, {len(st["missing"])} stored ones missing (e.g. '
                     f'{"extra" if st["extra"] else "missing"} {_state_desc(s)}: '
                     f'{", ".join((d or ["no stored state with its fields"])[:4])})')
    if score['textures'] > nz['textures']:
        why.add('textures')
        parts.append(f'{score["textures"]} textures differ (stored runs by up to {nz["textures"]})')
    if score['draws'] > DRAW_MATRIX_SLACK * nz['draws']:
        why.add('draws')
        parts.append(f'draw counts per state {score["draws"]} apart (stored runs up to {nz["draws"]})')
    if score['cams'] > allowed['cams'] + DRAW_MATRIX_FLOOR:
        why.add('cams')
        parts.append(f'views / projections up to {score["cams"]:.4g} apart (a stored run against the others: '
                     f'{allowed["cams"]:.4g})')
    if score['worlds'] > allowed['worlds'] or score['still'] > allowed['still']:
        why.add('worlds')
        i, j, d, lim = max(score['wo']['worlds'], key=lambda w: w[2] - w[3])
        parts.append(f'world matrices of {score["worlds"]} draws out of the stored spread, {score["still"]} of them '
                     f'still in every stored run (a stored run against the others: {allowed["worlds"]}, '
                     f'{allowed["still"]}; worst {_draw_desc(mine, i)} {d:.4g}, allowed {lim:.4g})')
    if parts:
        if reasons is not None:
            reasons |= why
        return False, 'as a multiset: ' + '; '.join(parts)
    return True, (f'the same as a multiset (draw counts per state {score["draws"]} apart, stored runs up to '
                  f'{nz["draws"]}; {score["worlds"]} draws out of the stored spread, a stored run against the others '
                  f'{allowed["worlds"]})')


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


def drawlog_verdict(mine_path, stored_paths, nzd, reasons=None):
    """A run's draw log for one shot against the stored ones -> (True SAME | False DIFF | None nothing to compare,
    text). SAME: equal to one stored run's log, or different only in fields and draw counts the stored runs vary in
    themselves, with every matrix within that stored run's own spread for the draw it pairs with (noise.json
    matrix_draws, times DRAW_MATRIX_SLACK, plus DRAW_MATRIX_FLOOR; a draw whose matrices never varied: the floor).
    REASONS (a set): what made a DIFF (multiset_verdict's; 'no log' / 'lfs' when there was nothing to compare)."""
    if not stored_paths:
        return None, 'no draw log stored'
    if not mine_path:
        if reasons is not None:
            reasons.add('no log')
        return False, 'no draw log in the run'
    nzd = nzd or {}
    allowed = set(nzd.get('varying', []))
    lo, hi = nzd.get('draws', [None, None])
    sets_spread = (nzd.get('matrix_spread') or {}).get('sets', 0.0)
    mine = load_drawlog(mine_path)
    best, loaded = None, []
    for p in stored_paths:
        if is_lfs_pointer(p):
            if reasons is not None:
                reasons.add('lfs')
            return False, f'{os.path.basename(p)} is a Git LFS pointer: run git lfs pull'
        run = os.path.basename(os.path.dirname(p))
        spread = (nzd.get('matrix_draws') or {}).get(run, {})

        def tol(j, k, spread=spread):
            v = sets_spread if j is None else spread.get(str(j), {}).get(k, 0.0)
            return float('inf') if v is None else DRAW_MATRIX_SLACK * v + DRAW_MATRIX_FLOOR
        loaded.append(load_drawlog(p))
        c = compare_drawlogs(mine, loaded[-1], tol)
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
        n_draws = len({i for i, f in c['field_draws'] if f in bad_fields})
        parts.append(f'first difference {_draw_desc(mine, i)}: {", ".join(here[:6])}'
                     + (f' (+{len(here) - 6})' if len(here) > 6 else '')
                     + f'; {len(bad_fields)} fields differ in {n_draws} of {len(c["pairs"])} paired draws')
    if c['bad']:
        parts.append('matrices ' + ', '.join(f'{k} in {n} draws from draw {f0} (up to {w:.4g})'
                                             for k, (n, f0, w) in sorted(c['bad'].items())))
    ok, text = multiset_verdict(mine, loaded, reasons)
    if ok:          # the ordered compare's differences are a moving scene's: order, LOD, matrices within the spread
        return True, f'{mine["count"]} draws, {text} (in order: {"; ".join(parts[1:])})'
    return False, '; '.join(parts) + '; ' + text


def drawlog_stability(stored_paths):
    """Retail against retail, leave one out (brief CS2, c2; CS's loo.py): [(run, text, reasons)] of each stored run
    whose draw log for one shot drawlog_verdict calls DIFF against the other stored runs' logs and their own noise
    (drawlog_noise of the others); [] when every one passes or fewer than two are stored (nothing measured)."""
    if len(stored_paths) < 2:
        return []
    run_of = lambda p: os.path.basename(os.path.dirname(p))     # noqa: E731
    out = []
    for i, p in enumerate(stored_paths):
        others = stored_paths[:i] + stored_paths[i + 1:]
        nzd = drawlog_noise({run_of(q): q for q in others}, [run_of(q) for q in others])
        why = set()
        ok, text = drawlog_verdict(p, others, nzd, why)
        if ok is False:
            out.append((run_of(p), text, why))
    return out


def drawlog_unstable(run_reasons, loo):
    """A run's draw-log DIFF is RETAIL-UNSTABLE when every reason it failed for (drawlog_verdict's REASONS) is one
    a stored run fails for against the others too (drawlog_stability's): retail's runs disagree there among
    themselves. A reason no left-out stored run shows (a field, a measure retail agrees on) keeps it DIFF."""
    shared = set().union(*(w for _, _, w in loo)) if loo else set()
    return bool(run_reasons) and run_reasons <= shared


# ---------------------------------------------------------------- packets, leave one out (brief CS3, c9)

def _packets_by_name(lst):
    out = {}
    for it in lst:
        n, _, size = it.partition(' ')
        out.setdefault(n, []).append(size)
    return out


def packet_reasons(a, b):
    """The kinds of difference between two packet lists of one scenario (e2e.sent_packets: 'NAME BYTES' in order):
    'count:NAME' NAME sent a different number of times (the random loot's INVENTORY_MOVE takes), 'size:NAME' sent
    in sizes the other list lacks (or as often in another mix of sizes), 'order' every other packet the same but in
    another order. set() when equal."""
    if a == b:
        return set()
    pa, pb = _packets_by_name(a), _packets_by_name(b)
    out = set()
    for n in set(pa) | set(pb):
        x, y = pa.get(n, []), pb.get(n, [])
        if len(x) != len(y):
            out.add('count:' + n)
        if (x and y and set(x) != set(y)) or (len(x) == len(y) and sorted(x) != sorted(y)):
            out.add('size:' + n)
    named = {r.split(':', 1)[1] for r in out}
    if [p for p in a if p.partition(' ')[0] not in named] != [p for p in b if p.partition(' ')[0] not in named]:
        out.add('order')
    return out


def _packet_reason_text(reason, mine, theirs):
    kind, _, n = reason.partition(':')
    if kind == 'order':
        return 'the order'
    x, y = _packets_by_name(mine).get(n, []), _packets_by_name(theirs).get(n, [])
    if kind == 'count':
        return f'{n} x{len(x)} (x{len(y)})'
    return f'{n} sizes {"/".join(sorted(set(x)))} ({"/".join(sorted(set(y)))})'


def _nearest(mine, others):
    """(index, reasons) of the packet list in others nearest mine: fewest kinds of difference, then fewest packets
    apart (difflib)."""
    import difflib
    best = None
    for i, t in enumerate(others):
        r = packet_reasons(mine, t)
        sm = difflib.SequenceMatcher(None, mine, t, autojunk=False)
        apart = len(mine) + len(t) - 2 * sum(m.size for m in sm.get_matching_blocks())
        key = (len(r), apart)
        if best is None or key < best[0]:
            best = (key, i, r)
    return (best[1], best[2]) if best else (None, set())


def packet_stability(stored):
    """Retail against retail, leave one out: stored is [(run, packets)]; [(run, reasons, nearest run)] of each stored
    run whose packets equal no other stored run's, with the kinds of difference against its nearest other run
    (packet_reasons). [] when every one equals another or fewer than two are stored (nothing measured)."""
    if len(stored) < 2:
        return []
    out = []
    for i, (run, pk) in enumerate(stored):
        others = stored[:i] + stored[i + 1:]
        if any(pk == o for _, o in others):
            continue
        j, r = _nearest(pk, [o for _, o in others])
        out.append((run, r, others[j][0]))
    return out


def packet_verdict(mine, stored):
    """(ok, text) for the run's packets of one scenario against the stored runs' ([(run, packets)]): True when equal
    to one; RETAIL_UNSTABLE when against some stored run every kind of difference (packet_reasons) is one a stored
    run shows against its nearest other stored run too, leave one out (c7's rule: the same kind of difference retail
    itself varies in: the random loot's takes, the job board's list); else False (DIFF), naming the kinds against
    the nearest stored run and those retail's own runs show."""
    theirs = [p for _, p in stored]
    head = f'packets {len(mine)}, stored {"/".join(str(len(t)) for t in theirs)}'
    if mine in theirs:
        return True, head
    if not stored:
        return False, head + ' (no stored run)'
    loo = packet_stability(stored)
    shared = set().union(*(r for _, r, _ in loo)) if loo else set()
    per = [packet_reasons(mine, t) for t in theirs]
    ok_at = [i for i, r in enumerate(per) if r and r <= shared]
    if ok_at:
        j, _ = _nearest(mine, [theirs[k] for k in ok_at])
        i = ok_at[j]
        lr = next(x for x in loo if per[i] & x[1])
        return RETAIL_UNSTABLE, (
            f'{head}: against {stored[i][0]} only in '
            + ', '.join(_packet_reason_text(r, mine, theirs[i]) for r in sorted(per[i]))
            + f', and so do {len(loo)} of the {len(stored)} stored runs, leave one out ({lr[0]} against {lr[2]}: '
            + ', '.join(_packet_reason_text(r, dict(stored)[lr[0]], dict(stored)[lr[2]]) for r in sorted(lr[1])) + ')')
    i, r = _nearest(mine, theirs)
    return False, (f'{head} (equal to none): against the nearest, {stored[i][0]}, '
                   + ', '.join(_packet_reason_text(x, mine, theirs[i]) for x in sorted(r))
                   + (f'; the stored runs, leave one out, differ only in {", ".join(sorted(shared))}' if shared else
                      '; the stored runs agree' if len(stored) > 1 else '; one stored run: nothing measured'))


# ---------------------------------------------------------------- noise

def stored_runs(scen_dir):
    return sorted(d for d in os.listdir(scen_dir) if os.path.isfile(os.path.join(scen_dir, d, 'manifest.json')))


def run_entries(scen_dir, name, superseded=None):
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
                    'packets': read_json(os.path.join(d, 'packets.json')).get(name, []),
                    'scn': (man.get('scenario_files') or {}).get(f'tools/harness/e2e/{name}.scn')})
    # like with like (2026-11-25): runs of an older text of the scenario's own file (play1.scn's shot moved to a
    # fixed playback time) are left out once a newer text has been captured
    newest = out[-1]['scn'] if out else None
    if newest is not None:
        old = [x['run'] for x in out if x['scn'] not in (None, newest)]
        out = [x for x in out if x['scn'] in (None, newest)]
        if superseded is not None:
            superseded.extend(old)
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


def _session_orders(dirs, man):
    """The scenarios of an e2e run in the order run, one list per client session: a stored manifest's order (one
    session), else each folder's e2e.log 'scenario NAME' lines (a run continued in a fresh session after a failure
    has a folder per session)."""
    if man is not None:
        return [list(man.get('order', [man.get('scenario')]))]
    out, seen = [], set()
    for d in dirs:
        log = os.path.join(d, 'e2e.log')
        if os.path.exists(log):
            order = []
            with open(log, encoding='utf-8', errors='replace') as f:
                for line in f:
                    p = line.split()
                    if len(p) >= 3 and p[1] == 'scenario' and p[2] not in seen:
                        order.append(p[2])
                        seen.add(p[2])
            out.append(order)
    return out


def _run_order(dirs, man):
    """The scenarios of an e2e run in the order run (every session's, _session_orders, in turn)."""
    return [n for o in _session_orders(dirs, man) for n in o]


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


AUDIO_CUTS = ('end', 'input')


def audio_cut_of(results):
    """The cut a session's results allow: 'input' when every scenario after the first has a tick_input (e2e since
    2026-11-25 evening), else 'end'."""
    later = [res for _, res in results[1:]]
    return 'input' if later and all(res.get('tick_input') is not None for res in later) else 'end'


def audio_slices(events, results, cut='end'):
    """{scenario: its events} of one session's audio log; RESULTS [(name, result)] in the order run. A scenario
    whose result has no tick_end (an older e2e) gets the whole log when it is the session's only one, else none.
    CUT 'end': a scenario's slice ends at its own tick_end; 'input' (brief CS2, c3): at the next scenario's first
    input (its tick_input), so what the scenario's last input started and the client plays only after the scenario's
    end (sp_galaxymap's message_1.wav, 1 ms into sp_warp's slice) stays with it; the last scenario ends at its
    tick_end, and a next scenario without a tick_input falls back to the 'end' boundary."""
    events = [e for e in events if e.get('ev') not in ('log', 'bad-line')]
    if len(results) == 1 and results[0][1].get('tick_end') is None:
        return {results[0][0]: events}
    out, lo = {}, None
    for k, (name, res) in enumerate(results):
        hi = res.get('tick_end')
        if hi is None:
            continue
        nxt = results[k + 1][1].get('tick_input') if cut == 'input' and k + 1 < len(results) else None
        if nxt is not None and nxt > (lo if lo is not None else -1):
            hi = nxt                # read before the input is posted: the client answers it at a later tick
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


def session_audio(session, results, cut=None):
    """{scenario: audio_summary} of SESSION's audio.jsonl, and the log's {bytes, sha256, events}; ({}, None)
    without one. CUT None: the finest the results allow (audio_cut_of); each summary records its 'cut'."""
    import audiolog
    path = os.path.join(session, audiolog.LOG_NAME)
    if not os.path.isfile(path):
        return {}, None
    events = audiolog.load(path)
    names = audio_names(events)
    cut = cut or audio_cut_of(results)
    out = {n: dict(audio_summary(ev, names), cut=cut) for n, ev in audio_slices(events, results, cut).items()}
    return out, {'bytes': os.path.getsize(path), 'sha256': sha256_file(path), 'events': len(events)}


def audio_cut_stored(stored):
    """The cut the stored summaries were made with: 'input' only when every one says so (older ones: 'end')."""
    return 'input' if stored and all(a.get('cut') == 'input' for a in stored) else 'end'


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


RETAIL_UNSTABLE = 'RETAIL-UNSTABLE'
# verify --run --audio-only's word for each audio_verdict
AUDIO_ONLY_WORD = {True: 'SAME', False: 'DIFF', RETAIL_UNSTABLE: RETAIL_UNSTABLE, None: 'NONE'}


def name_samples(summaries, data_dir=None):
    """Names the samples the summaries' sequences give only as hash:size (a 3D sample has no load event naming it;
    a set's later scenario lacks the load) from the client data's files and mixes (audiolog.namer and its cache;
    DATA_DIR default machine.py client_data's Data): returns copies with those names added; the summaries as they
    are when nothing can be named (no data, off Windows)."""
    keys = {x[2] for a in summaries if a for x in a['sequence']
            if x[2] and ':' in str(x[2]) and x[2] not in a.get('names', {})}
    if not keys:
        return summaries
    try:
        import audiolog
        import machine
        data = data_dir or os.path.join(machine.get('client_data'), 'Data')
        found = audiolog.namer([{'ev': 'load', 'hash': k.split(':')[0], 'size': int(k.split(':')[1])} for k in keys],
                               data)
    except Exception:           # noqa: BLE001: naming is a reading aid; without it the hashes stay
        return summaries
    return [dict(a, names={**{k: v for k, v in found.items() if k in keys}, **a.get('names', {})}) if a else a
            for a in summaries]


def audio_family(a, snd):
    """A sound's family: its file name (a stream's own, a sample's from the load events) with every digit run as
    '#', so the client's random variants of one sound share it (TerranWander07-2.mp3 and TerranWander01-5.mp3:
    TerranWander#-#.mp3); a sample of no known name stays its hash:size."""
    name = a.get('names', {}).get(snd, snd)
    if name == snd and ':' in str(snd):
        return snd
    stem, ext = os.path.splitext(str(name))
    return re.sub(r'\d+', '#', stem) + ext


def audio_streams(a):
    """{'KIND FAMILY': [(event, loop, sound), ...]}: each family's streams' or 3D samples' own events of the strict
    sequence, in order (audio_family)."""
    out = {}
    for ev, kind, snd, loop in audio_strict(a):
        out.setdefault(f'{kind} {audio_family(a, snd)}', []).append((ev, loop, snd))
    return out


def _shape(items):
    return [(ev, loop) for ev, loop, _ in items]


def _loo(vals):
    """Leave one out: every value equals another one's."""
    return all(any(vals[i] == vals[j] for j in range(len(vals)) if j != i) for i in range(len(vals)))


def audio_stability(stored):
    """Retail against retail, leave one out (brief CS2, c2: the measurement behind RETAIL-UNSTABLE): 'order' whether
    every stored run's whole strict sequence equals another stored run's; per family ('KIND FAMILY', audio_streams)
    'shapes' whether every stored run's events of it (event and loop, the variant left out; a run without it: none)
    equal another stored run's, and 'variants' the same with the variants' names (which of the random variants
    played); 'runs' the count. With fewer than two stored runs nothing is measured: everything reads stable (DIFF)."""
    seqs = [audio_strict(a) for a in stored]
    per = [audio_streams(a) for a in stored]
    keys = sorted(set().union(*per)) if per else []
    if len(stored) < 2:
        return {'runs': len(stored), 'order': True, 'shapes': {k: True for k in keys},
                'variants': {k: True for k in keys}}
    return {'runs': len(stored), 'order': _loo(seqs),
            'shapes': {k: _loo([_shape(p.get(k, [])) for p in per]) for k in keys},
            'variants': {k: _loo([p.get(k, []) for p in per]) for k in keys}}


def _stream_name(key, mine):
    """'KIND FAMILY' and the run's own variants of it."""
    own = sorted({_snd(mine, snd) for _, _, snd in audio_streams(mine).get(key, [])})
    return key + (f' ({", ".join(own[:3])}{" ..." if len(own) > 3 else ""})' if own and own != [key.split(' ', 1)[1]]
                  else '')


def audio_verdict(mine, stored, nz):
    """(ok, text) for MINE (an audio_summary or None) against the stored runs' summaries: ok None when nothing is
    stored ("no audio stored") or the run has none. When the streams/3D sequence equals no stored run's, each sound
    family (audio_family) whose events in the run match no stored run's is weighed: its shape (events, loops) equal
    to a stored run's, it differs in which variant played; RETAIL_UNSTABLE when the stored runs disagree on that
    themselves, leave one out (audio_stability: the variants, or the shape when the run's shape is not stored),
    else DIFF (False). Every family matching but the interleaving differing: RETAIL_UNSTABLE when the stored runs'
    whole sequences fail leave one out too, else DIFF. A 2D count out of the stored range is DIFF."""
    if not stored:
        return None, 'no audio stored'
    if mine is None:
        return None, 'no audio in the run'
    st = audio_strict(mine)
    why, shaky = [], []
    if all(st != audio_strict(a) for a in stored):
        d = max((_audio_diff(mine, a) for a in stored), key=lambda kt: kt[0])     # the longest agreement
        stab = audio_stability(stored)
        own = audio_streams(mine)
        theirs = [audio_streams(a) for a in stored]
        firm, loose = [], []
        for k in sorted(set(own) | set(stab['shapes'])):
            mk = own.get(k, [])
            if any(mk == t.get(k, []) for t in theirs):
                continue
            if any(_shape(mk) == _shape(t.get(k, [])) for t in theirs):
                (loose if not stab['variants'].get(k, True) else firm).append(k)    # another variant played
            else:
                (loose if not stab['shapes'].get(k, True) else firm).append(k)
        if firm or (not loose and stab['order']):
            why.append(f'streams/3D {d[1]} (run first, stored second)'
                       + (f'; stable in the {stab["runs"]} stored runs and different: '
                          + ', '.join(_stream_name(k, mine) for k in firm[:4])
                          + (f' (+{len(firm) - 4})' if len(firm) > 4 else '') if firm else
                          f'; every sound\'s own events as stored, the order differs, the {stab["runs"]} stored runs '
                          'agree on it'))
        elif loose:
            shaky.append(f'streams/3D: ' + ', '.join(_stream_name(k, mine) for k in loose[:4])
                         + (f' (+{len(loose) - 4})' if len(loose) > 4 else '')
                         + f' differ, and so do the {stab["runs"]} stored runs among themselves, leave one out '
                         f'({d[1]})')
        else:
            shaky.append(f'streams/3D: every sound\'s own events as stored, the order differs, and so do the '
                         f'{stab["runs"]} stored runs\' orders among themselves, leave one out ({d[1]})')
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
        return False, '; '.join(why + shaky)
    if shaky:
        return RETAIL_UNSTABLE, '; '.join(shaky)
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


def _is_session(d):
    """An e2e session folder: its own e2e.log or result-*.json."""
    return os.path.isfile(os.path.join(d, 'e2e.log')) or any(
        f.startswith('result-') and f.endswith('.json') for f in os.listdir(d))


def out_sessions(top, out=print):
    """An e2e --out folder -> its newest run's session folders, oldest first; a session folder (or anything not a
    folder) -> [top]. The sessions are the subfolders <YYYYmmdd-HHMMSS>-<tag> with an e2e.log (e2e names them by
    its STAMP, so their names sort by start). A run is one session, plus one for the rest after each failure
    (e2e.run_all), so its sessions' scenarios do not repeat: walking from the newest, a session that holds a
    scenario a newer one already holds belongs to an earlier run into the same --out; those are named and left out,
    as are sessions without a result older than the run's first."""
    if not os.path.isdir(top) or _is_session(top):
        return [top]
    import e2e
    subs = sorted(os.path.join(top, f) for f in os.listdir(top)
                  if os.path.isfile(os.path.join(top, f, 'e2e.log')))
    if not subs:
        return [top]
    run, seen = [], set()
    for k in range(len(subs) - 1, -1, -1):
        names = set(e2e.run_results([subs[k]]))
        if names & seen:
            old = subs[:k + 1]
            out(f'baseline: {top}: {len(old)} older session folder(s) of an earlier run left out '
                f'({", ".join(os.path.basename(s) for s in old)}; repeat {", ".join(sorted(names & seen))}); '
                'give one as --run to compare it')
            break
        seen |= names
        run.insert(0, subs[k])
    while len(run) > 1 and not e2e.run_results([run[0]]):
        run.pop(0)
    if len(run) > 1 or run[0] != top:
        out(f'baseline: {top}: an e2e --out folder: {len(run)} session(s) '
            + ', '.join(f'{os.path.basename(s)} ({len(e2e.run_results([s]))} results)' for s in run))
    return run


def _run_input(run):
    """RUN -> (e2e run folders, a stored manifest or None). An e2e --out folder stands for its newest run's session
    folders (out_sessions)."""
    import e2e
    if os.path.basename(run) == 'manifest.json' or os.path.isfile(os.path.join(run, 'manifest.json')):
        d = os.path.dirname(run) if run.endswith('manifest.json') else run
        return [d], read_json(os.path.join(d, 'manifest.json'))
    dirs = []
    for d in e2e.run_dirs(run):
        dirs += [s for s in out_sessions(d) if s not in dirs]
    return dirs, None


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
    sessions = _session_orders(dirs, man)
    order = [n for o in sessions for n in o]
    mine_audio = {cut: {} for cut in AUDIO_CUTS}      # the run's audio cut as the stored runs were cut
    if man is not None:                 # a stored run: its audio-NAME.json files, as they were cut
        for en in (man.get('scenarios') or [dict(man, name=man.get('scenario'))]):
            if en.get('audio'):
                for cut in AUDIO_CUTS:
                    mine_audio[cut][en['name']] = read_json(os.path.join(dirs[0], en['audio']))
    else:
        for d in dirs:                  # the results read from this folder (its 'out' may name where it ran)
            here = [(n, results[n]) for n in e2e.run_results([d]) if n in results]
            here.sort(key=lambda nr: order.index(nr[0]) if nr[0] in order else 1 << 20)
            for cut in AUDIO_CUTS:
                mine_audio[cut].update(session_audio(d, here, cut)[0])
    unstable = 0
    holders = [(c, e, s, d, dir_scenarios(d)) for c, e, s, d in scenario_dirs(store)
               if (not client or c == client) and (not env or e == env)]
    bad, apart, used = 0, 0, set()
    audio_tally = {}
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
        superseded = []
        ents = run_entries(d, name, superseded)
        nz = read_json(os.path.join(d, 'noise.json'))
        nzs = nz['scenarios'].get(name, {}) if 'set' in nz else nz
        what, notes, hard = [], [], []     # hard: differences no scenario order explains (the verdict, a shot)
        shaky = []                          # differences the stored retail runs share among themselves
        v = e2e.verdict(results[name])
        stored_v = sorted({en['verdict'] for en in ents})
        if v not in stored_v:
            hard.append(f'verdict {v}, stored {"/".join(stored_v)}')
        mine = packets.get(name, [])
        p_ok, p_text = packet_verdict(mine, [(en['run'], en['packets']) for en in ents])
        if p_ok is False:
            what.append(p_text)
        elif p_ok == RETAIL_UNSTABLE:
            shaky.append(p_text)
        stored_order = ents[-1]['order'] if ents else []
        used.add((c, e, s))
        if len(held) > 1 or len(order) > 1:
            mine_s = next((o for o in sessions if name in o), None)
            before_mine = mine_s[:mine_s.index(name)] if mine_s else None
            before_theirs = stored_order[:stored_order.index(name)] if name in stored_order else None
            fresh = mine_s is not None and mine_s is not sessions[0]
            prev_mine = before_mine[-1] if before_mine else None
            prev_theirs = before_theirs[-1] if before_theirs else None
            if prev_mine != prev_theirs:
                notes.append(f'run after {prev_mine or "the login"}' + (' (a fresh session)' if fresh else '')
                             + f', stored after {prev_theirs or "the login"} (scenario order differs)')
            elif before_mine != before_theirs:
                notes.append(f'run after {prev_mine} in a fresh session from {mine_s[0]}, stored after it in one '
                             f'from {stored_order[0] if stored_order else "?"} (scenario order differs)' if fresh else
                             f'run after {prev_mine} as stored, earlier scenarios differ (scenario order differs)')
        stored_shots, stored_logs = {}, {}
        for en in ents:
            for key, f in en['shots']:
                stored_shots.setdefault(key, []).append(os.path.join(en['dir'], f))
            for key, f in en['drawlogs']:
                stored_logs.setdefault(key, []).append(os.path.join(en['dir'], f))
        mine_shots = dict(shot_keys(shots.get(name, [])))
        logs_same, logs_none, logs_ms = 0, 0, 0
        for key in sorted(mine_shots):
            why = set()
            ok, text = drawlog_verdict(drawlog_path(mine_shots[key]), stored_logs.get(key, []),
                                       nzs.get('shots', {}).get(key, {}).get('drawlog'), why)
            if ok is None:
                logs_none += 1
            elif ok:
                logs_same += 1
                logs_ms += 'as a multiset' in text
            else:
                loo = drawlog_stability(stored_logs.get(key, []))
                if drawlog_unstable(why, loo):  # retail's own runs fail the same way, leave one out
                    shaky.append(f'draw log {key}: {text} (and {len(loo)} of the {len(stored_logs[key])} stored runs '
                                 f'against the others, leave one out, for {", ".join(sorted(why))}: {loo[0][0]}: '
                                 f'{loo[0][1][:160]})')
                else:
                    what.append(f'draw log {key}: {text}' + (
                        f' (the stored runs, leave one out, differ only in {", ".join(sorted(set().union(*(w for _, _, w in loo))))})'
                        if loo else ''))
        within = 0
        for key in sorted(set(stored_shots) | set(mine_shots)):
            if key not in mine_shots or key not in stored_shots:
                hard.append(f'shot {key} ' + ('not taken' if key not in mine_shots else 'not in the baseline'))
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
        stored_audio = [read_json(en['audio']) for en in ents if en.get('audio')]
        named = name_samples([mine_audio[audio_cut_stored(stored_audio)].get(name)] + stored_audio)
        a_ok, a_text = audio_verdict(named[0], named[1:], nzs.get('audio'))
        if getattr(args, 'audio_only', False):     # AU-01: the audio verdict alone, one line a scenario
            k = AUDIO_ONLY_WORD[a_ok]
            audio_tally[k] = audio_tally.get(k, 0) + 1
            print(f'{name}: AUDIO {k} {a_text}' + (f' ({"; ".join(notes)})' if notes else '')
                  + f'  [{c}/{e}/{s}, {len(ents)} runs]')
            continue
        if a_ok is False:
            what.append(f'audio {a_text}')
        elif a_ok == RETAIL_UNSTABLE:
            shaky.append(f'audio {a_text}')
        line = f'{len(mine)} packets, shots {within} of {len(set(stored_shots) | set(mine_shots))} within noise'
        line += (f', draw logs {logs_same} of {len(mine_shots)} the same' + (f' ({logs_ms} as a multiset)' if logs_ms
                                                                            else '')
                 if logs_same or not logs_none else '') + \
            (f', no draw log stored for {logs_none}' if logs_none else '')
        line += f', audio the same ({a_text})' if a_ok is True else f', {a_text}'
        # packets, shots, draw logs and audio carry state from the scenario before (texture address modes, draw
        # order, matrices: retail itself differs so): after another scenario than stored they are not comparable
        apart_here = bool(what or shaky) and not hard and bool(notes)
        # RETAIL-UNSTABLE (brief CS2, c2): the only differences are in audio streams / draw logs the stored retail
        # runs disagree on among themselves (leave one out): neither SAME nor DIFF, never a pass
        shaky_here = bool(shaky) and not (hard or what) and not apart_here
        if apart_here:
            head = 'NOT COMPARABLE (scenario order differs: compare a run in the stored order) ' + '; '.join(
                what + [f'{RETAIL_UNSTABLE}: {x}' for x in shaky])
        elif shaky_here:
            head = f'{RETAIL_UNSTABLE} ' + '; '.join(shaky)
        else:
            head = ('DIFF ' + '; '.join(hard + what + [f'also {RETAIL_UNSTABLE}: {x}' for x in shaky])
                    if hard or what else 'SAME ' + line)
        print(f'{name}: {head}' + (f' ({"; ".join(notes)})' if notes else '') + f'  [{c}/{e}/{s}, {len(ents)} runs'
              + (f'; {len(superseded)} of an older {name}.scn left out' if superseded else '') + ']')
        bad += bool(hard or what) and not apart_here
        apart += apart_here
        unstable += shaky_here
    if getattr(args, 'audio_only', False):
        print('baseline verify --run --audio-only: ' + ', '.join(f'{audio_tally.get(k, 0)} {k}' for k in
                                                                 ('SAME', RETAIL_UNSTABLE, 'DIFF', 'NONE'))
              + (f', {bad} without a baseline' if bad else ''))
        return 1 if bad or audio_tally.get('DIFF') else 4 if audio_tally.get(RETAIL_UNSTABLE) else 0
    missed = skipped_scenarios(args, dirs, results, names, holders, used)
    print(f'baseline verify --run: {len(names) - bad - apart - unstable} of {len(names)} scenarios the same as the '
          'baseline'
          + (f', {unstable} {RETAIL_UNSTABLE} (differ only where the stored retail runs differ among themselves)'
             if unstable else '')
          + (f', {apart} not comparable (run after another scenario than stored)' if apart else '')
          + (f'; {missed} stored scenario(s) of the set not in the run (named above)' if missed else ''))
    return 1 if bad or (missed and args.set) else 4 if unstable else 3 if apart else 0


_SESSION_HEAD = re.compile(r' session of \d+: (.+?); exe ')


def e2e_results_in(dirs, base):
    """{scenario: result} of the run folder among DIRS named BASE."""
    import e2e
    return e2e.run_results([d for d in dirs if os.path.basename(d) == base])


def skipped_scenarios(args, dirs, results, names, holders, used, out=print):
    """verify --run: names every scenario it did not compare, and why: the run's results left out by NAME arguments,
    and each set's stored scenarios the run has no result for (a set the run was compared with, or --set's). A
    scenario e2e listed in a session's head line but wrote no result for was not run (a stop after a repeated crash
    or a failed login) or its session died; one never listed was not in the run's e2e command, or its session
    folder was not given. Returns the count of the sets' scenarios the run lacks (verify fails on them under --set)."""
    unnamed = [n for n in results if n not in names]
    if unnamed:
        out(f'not compared: {", ".join(unnamed)} (in the run, not named on the command line)')
    listed = {}
    for d in dirs:
        log = os.path.join(d, 'e2e.log')
        if os.path.isfile(log):
            with open(log, encoding='utf-8', errors='replace') as f:
                for line in itertools.islice(f, 5):
                    m = _SESSION_HEAD.search(line)
                    if m:
                        for n in m.group(1).split(', '):
                            listed.setdefault(n, os.path.basename(d))
    sets = [h for h in holders if len(h[4]) > 1 and ((h[0], h[1], h[2]) in used or (args.set and h[2] == args.set))]
    missed = 0
    for c, e, s, d, held in sets:
        miss = [n for n in held if n not in results and (not args.names or n in args.names)]
        if not miss:
            continue
        missed += len(miss)
        why = {}
        for n in miss:
            if n not in listed:
                w = ("never listed in the run's sessions: not in its e2e command, or its session folder not given "
                     '(give the e2e --out folder)')
            else:
                failed = [r for r, res in e2e_results_in(dirs, listed[n]).items() if not res.get('passed')]
                w = (f'listed in session {listed[n]}, which ended after {failed[-1]} failed: e2e ran the rest in a '
                     'fresh session folder (give the e2e --out folder)' if failed else
                     f'listed in session {listed[n]}, no result: not run (e2e stopped the run) or its client died')
            why.setdefault(w, []).append(n)
        for w, ns in why.items():
            out(f'{"SKIP" if args.set else "not compared"}: {", ".join(ns)} ({len(ns)} of the set\'s {len(held)}: '
                f'{w})  [{c}/{e}/{s}]')
    return missed


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


def _db_snapshot_arg(text):
    import e2e
    return e2e.db_snapshot_arg(text)


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    ap.add_argument('command', choices=['capture', 'verify', 'list', 'inputs', 'tools'])
    ap.add_argument('names', nargs='*')
    ap.add_argument('--store', help="the store's checkout (default machine.py baselines)")
    ap.add_argument('--runs', type=int, default=3, help='capture: PASS runs to store per scenario (default 3)')
    ap.add_argument('--retries', type=int, default=1, help='capture: failed attempts allowed before giving up (1)')
    ap.add_argument('--live', nargs='?', const='localhost', metavar='SERVER', help='capture: e2e --live [SERVER]')
    ap.add_argument('--db-snapshot', nargs='?', const=6, type=_db_snapshot_arg, metavar='AVATAR',
                    help="capture: e2e --db-snapshot (an avatar id, or 'schema': the whole net7_user schema)")
    ap.add_argument('--restart', action=argparse.BooleanOptionalAction, default=None,
                    help='capture --live: restart Net7.exe before each run (default: when a scenario is sp_*; '
                         '--no-restart: restore in place)')
    ap.add_argument('--antprox-arg', action='append', metavar='ARG',
                    help="capture --live: e2e --antprox-arg ARG (repeatable; /NOPATCHES: retail unpatched, as ours "
                         "runs; UI-01's creation records differ by Net-7's class patches); the manifest's server records them")
    ap.add_argument('--no-audio', action='store_true', help='capture: without the audio log (e2e --audio is on)')
    ap.add_argument('--camera-track', action='store_true',
                    help="capture: e2e --camera-track (camera-track.txt, the camera's path; camtrack.py compares)")
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
    ap.add_argument('--audio-only', action='store_true',
                    help='verify --run: the audio verdict alone (test-plan AU-01: streams and 3D sounds in order, '
                         '2D sounds by count), one line a scenario; exit 1 on a DIFF, 4 RETAIL-UNSTABLE')
    ap.add_argument('--allow-stale', action='store_true',
                    help='verify --run: go on when our exe is older than the newest client-source commit')
    ap.add_argument('--set', help='capture: the scenarios as ONE e2e session a run, stored as this set (no names: '
                                  "e2e's default set); verify --run: compare with this stored set")
    args = ap.parse_intermixed_args(argv)   # names after an option too (capture --set SET NAME...)
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
