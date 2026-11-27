"""The land's horizon tables as the client writes them (test-plan SP-12, bug B-29): run, collect and compare .pld files.

    python tools/harness/pldcheck.py run --instance NAME [--retail | --exe PATH] [--runs N] --out DIR [--scenario S]
                                         [--admin-level 90] [--keep]
    python tools/harness/pldcheck.py compare --retail DIR... --ours DIR... [--label L] [--table normal_index_1E4]
    python tools/harness/pldcheck.py show FILE

A planet surface's LandClass::Load_Assets (0x4691D0) loads its horizon tables from the height map's .pld when one is
newer than the .pgm, the colour map and the ini; otherwise it computes them (Compute_Horizons 0x46E670: the normal
step 0x46F120 fills the normal-index table +0x1E4 through Nearest_Normal 0x46F0A0) and writes them (Save_PLD
0x46ECF0): the count (4 bytes), +0x1E4 (count bytes), +0x1E8 and +0x1EC (count shorts each), +0x1F0, +0x1F4, +0x1F8
(count bytes each), the 256 normals +0x1FC (Vector3 each). The file is the client's own dump of the tables: nothing
reads or writes the process.

run: for each of N runs, prepares the clientrun.py instance (an instance's Data\\client\\output is its own copy, its
other Data folders junctions to CLIENT_DATA_ROOT's), deletes every .pld in the instance's release folder and its own
output folder (never in a junction: the shared data is not touched), runs `e2e.py --live --db-snapshot 7 --keep`
with the scenario (default sp_land_pld: Endriago; sp_land_pld_all: the eight surfaces by wormhole, which needs
--admin-level 90, Elanor's Net-7 level set before each run and put back after it), and copies each .pld the client wrote during the run into DIR/run-K/ with
run.json (the exe's SHA-256, the verdict, the files). A .pld written into a shared folder (through a junction) is
reported, not copied. The instance is kept between the runs and cleaned at the end (`--keep` keeps it).

compare: retail run to run first (every retail run's file against the first's, table by table: the precondition),
then each of ours against retail's first: SAME, or per table the number of differing entries and the first ones
(index, grid x / y, retail's value, ours'), the entries where retail's own runs differ (the retail spread: six edge
cells of +0x1EC..+0x1F8 on most maps) left out and counted. --table T: the verdict on T alone (B-29: the normal
index table). Exit 0 when retail is stable and ours equals it, 1 otherwise.
"""
import argparse
import glob
import hashlib
import json
import math
import os
import shutil
import struct
import subprocess
import sys
import time

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)

# (name, element size) in Save_PLD's order after the count; the normals are 256 Vector3s after them
TABLES = [('normal_index_1E4', 1), ('table_1E8', 2), ('table_1EC', 2), ('table_1F0', 1), ('table_1F4', 1),
          ('table_1F8', 1)]
NORMALS = ('normals_1FC', 256 * 12)


class PldError(ValueError):
    pass


def parse(data):
    """A .pld's bytes -> {'count', table name: bytes, 'normals_1FC': bytes, 'extra': bytes after the normals}."""
    if len(data) < 4:
        raise PldError(f'{len(data)} bytes: no count')
    count = struct.unpack_from('<I', data, 0)[0]
    out, pos = {'count': count}, 4
    for name, size in TABLES:
        n = count * size
        if pos + n > len(data):
            raise PldError(f'{name}: needs {n} bytes at {pos}, the file has {len(data) - pos}')
        out[name] = data[pos:pos + n]
        pos += n
    out[NORMALS[0]] = data[pos:pos + NORMALS[1]]
    if len(out[NORMALS[0]]) != NORMALS[1]:
        raise PldError(f'normals: {len(out[NORMALS[0]])} of {NORMALS[1]} bytes')
    pos += NORMALS[1]
    out['extra'] = data[pos:]
    return out


def load(path):
    with open(path, 'rb') as f:
        return parse(f.read())


def side_of(count):
    s = math.isqrt(count)
    return s if s * s == count else None


def values(blob, size):
    if size == 1:
        return list(blob)
    if size == 2:
        return list(struct.unpack(f'<{len(blob) // 2}H', blob))
    return list(struct.unpack(f'<{len(blob) // 4}f', blob))       # normals: floats


def diff_indices(a, b):
    """{table: [differing entry indices]} for every table that differs (the normals by normal, not by component);
    'extra' for bytes after the normals; {'count': None} when the counts differ."""
    if a['count'] != b['count']:
        return {'count': None}
    res = {}
    for name, size in TABLES + [(NORMALS[0], 4)]:
        if a[name] == b[name]:
            continue
        va, vb = values(a[name], size), values(b[name], size)
        idx = [i for i in range(len(va)) if va[i] != vb[i]]
        res[name] = sorted({i // 3 for i in idx}) if name == NORMALS[0] else idx
    if a['extra'] != b['extra']:
        res['extra'] = [0]
    return res


def spread(ref, others):
    """{table: set of entry indices} where any of OTHERS differs from REF: retail's own spread, run to run."""
    out = {}
    for o in others:
        for name, idx in diff_indices(ref, o).items():
            out.setdefault(name, set()).update(idx)
    return out


def diff_tables(a, b, first=5, skip=None):
    """{table: (differing entries, [(index, x, y, a's value, b's value)...first], entries left out)} for every table
    that differs outside SKIP ({table: indices}, retail's spread); a count that differs is {'count': (a, b)}."""
    d = diff_indices(a, b)
    if 'count' in d:
        return {'count': (a['count'], b['count'])}
    side = side_of(a['count'])
    skip = skip or {}
    res = {}
    for name, idx in d.items():
        left = [i for i in idx if i in skip.get(name, ())]
        idx = [i for i in idx if i not in skip.get(name, ())]
        if not idx:
            continue
        if name == NORMALS[0]:
            va, vb = values(a[name], 4), values(b[name], 4)
            rows = [(i, None, None, tuple(va[3 * i:3 * i + 3]), tuple(vb[3 * i:3 * i + 3])) for i in idx[:first]]
        elif name == 'extra':
            rows = []
        else:
            size = dict(TABLES)[name]
            va, vb = values(a[name], size), values(b[name], size)
            rows = [(i, i % side if side else None, i // side if side else None, va[i], vb[i]) for i in idx[:first]]
        res[name] = (len(idx), rows, len(left))
    return res


def fmt_diff(d, spread_note=None):
    """SAME / DIFF with each table's differing entries; SPREAD_NOTE ({table: entries of ours inside retail's spread,
    left out}) is named after SAME."""
    note = ''
    if spread_note:
        note = ' (' + ', '.join(f'{k}: {v} in the retail spread' for k, v in spread_note.items() if v) + ')'
        note = '' if note == ' ()' else note
    if not d:
        return 'SAME' + note
    if 'count' in d:
        return f'DIFF count {d["count"][0]} vs {d["count"][1]}'
    parts = []
    for name, (n, rows, left) in d.items():
        ex = ', '.join(f'#{i}' + (f' ({x},{y})' if x is not None else '') + f' {va}->{vb}' for i, x, y, va, vb in rows)
        parts.append(f'{name}: {n} differ' + (f' [{ex}]' if ex else '') + (f', {left} more in the retail spread'
                                                                           if left else ''))
    return 'DIFF ' + '; '.join(parts) + note


def sha256(path):
    h = hashlib.sha256()
    with open(path, 'rb') as f:
        for chunk in iter(lambda: f.read(1 << 20), b''):
            h.update(chunk)
    return h.hexdigest()


def _own_dir(path):
    """True for a real folder of the instance, False for a junction or link into the shared data."""
    return os.path.isdir(path) and not os.path.islink(path) and not os.path.isjunction(path)


def pld_dirs(inst):
    """The instance's own folders on the client's search path (main_client.cpp's sub-directories): its release
    folder, Data/client and its output copy; a junction among them is left out."""
    client = os.path.join(inst.base, 'Data', 'client')
    return [d for d in (inst.dir, client, os.path.join(client, 'output')) if _own_dir(d)]


def shared_dirs(inst):
    """The search path's folders that lie in the shared data (junctions): a .pld written there is reported."""
    data = os.path.join(inst.base, 'Data')
    cand = [os.path.join(data, 'common'), os.path.join(data, 'common', 'heightfields'),
            os.path.join(data, 'client', 'output')]
    return [d for d in cand if os.path.isdir(d) and not _own_dir(d)
            or os.path.isdir(d) and os.path.realpath(d) != os.path.abspath(d)]


AVATAR = 7          # Elanor, _login_space.scn's character (e2e --db-snapshot 7)


def admin_level(value):
    """Sets Elanor's Net-7 admin level (net7_user.avatar_info.admin of avatar 7: AccountManager.cpp reads it at login
    into the player's AdminLevel) to VALUE and returns the old one. sp_land_pld_all needs 90 (SDEV): Net-7 refuses
    /wormhole out of a sector that is not space for any other level (PlayerConnection.cpp
    Player::HandleWormholeRequest, `AdminLevel() != 90`); the store's rows have 100. cmd_run puts the old value back
    after each run, failed or not (e2e's --db-snapshot dumps the rows after this change, so its restore keeps it)."""
    import e2e
    q = f'SELECT admin FROM avatar_info WHERE avatar_id={AVATAR}'
    old = int(e2e._db_run(e2e.db_exec(f'mysql -uroot @PW -N -B -e "{q}" {e2e.DB_SCHEMA}')).split()[0])
    if old != int(value):
        u = f'UPDATE avatar_info SET admin={int(value)} WHERE avatar_id={AVATAR}'
        e2e._db_run(e2e.db_exec(f'mysql -uroot @PW -e "{u}" {e2e.DB_SCHEMA}'))
        now = int(e2e._db_run(e2e.db_exec(f'mysql -uroot @PW -N -B -e "{q}" {e2e.DB_SCHEMA}')).split()[0])
        if now != int(value):
            raise SystemExit(f'pldcheck: avatar {AVATAR} admin is {now}, not {value}')
        print(f'pldcheck: avatar {AVATAR} admin {old} -> {value}', flush=True)
    return old


def cmd_run(a):
    import clientrun
    if a.retail and a.exe:
        raise SystemExit('--exe and --retail exclude each other')
    exe = None if a.retail else os.path.abspath(a.exe or clientrun.CLIENT_EXE)
    os.makedirs(a.out, exist_ok=True)
    e2e = os.path.join(HERE, 'e2e.py')
    results = []
    for k in range(1, a.runs + 1):
        inst = clientrun.Inst(a.instance, exe, a.retail)
        inst.prepare()
        removed = []
        for d in pld_dirs(inst):
            for p in glob.glob(os.path.join(d, '*.pld')):
                os.remove(p)
                removed.append(p)
        run_dir = os.path.join(a.out, f'run-{k}')
        os.makedirs(run_dir, exist_ok=True)
        cmd = [sys.executable, e2e, '--live', '--db-snapshot', '7', '--keep', '--instance', a.instance,
               '--out', os.path.join(run_dir, 'e2e')]
        cmd += ['--retail'] if a.retail else ['--exe', exe]
        cmd += ['run', a.scenario]
        t0 = time.time()
        before = admin_level(a.admin_level) if a.admin_level is not None else None
        try:
            with open(os.path.join(run_dir, 'e2e.out'), 'w') as log:
                rc = subprocess.run(cmd, stdout=log, stderr=subprocess.STDOUT,
                                    cwd=os.path.dirname(os.path.dirname(HERE))).returncode
        finally:
            if before is not None:
                admin_level(before)
        with open(os.path.join(run_dir, 'e2e.out'), errors='replace') as f:
            lines = [ln.rstrip() for ln in f if ln.strip()]
        verdict = next((ln for ln in reversed(lines) if a.scenario in ln and
                        any(w in ln for w in ('PASS', 'FAIL', 'XFAIL', 'XPASS'))), lines[-1] if lines else '')
        written, shared = [], []
        for d in pld_dirs(inst):
            for p in glob.glob(os.path.join(d, '*.pld')):
                if os.path.getmtime(p) >= t0 - 1:
                    shutil.copy2(p, os.path.join(run_dir, os.path.basename(p)))
                    written.append({'file': os.path.basename(p), 'from': p, 'sha256': sha256(p),
                                    'bytes': os.path.getsize(p)})
        for d in shared_dirs(inst):
            for p in glob.glob(os.path.join(d, '*.pld')):
                if os.path.getmtime(p) >= t0 - 1:
                    shared.append(p)
        r = {'run': k, 'retail': bool(a.retail), 'exe': inst.src_exe, 'exe_sha256': sha256(inst.src_exe),
             'instance': a.instance, 'scenario': a.scenario, 'rc': rc, 'verdict': verdict,
             'seconds': round(time.time() - t0, 1), 'removed': removed, 'written': written,
             'written_shared': shared}
        with open(os.path.join(run_dir, 'run.json'), 'w') as f:
            json.dump(r, f, indent=1)
        results.append(r)
        print(f'pldcheck: run {k}: rc {rc}, {verdict.strip()}, {r["seconds"]} s; wrote '
              + (', '.join(f'{w["file"]} {w["sha256"][:12]}' for w in written) or 'no .pld')
              + (f'; IN A SHARED FOLDER: {", ".join(shared)}' if shared else ''), flush=True)
    if not a.keep:
        clientrun.clean(a.instance)
    return 0 if all(r['rc'] == 0 and r['written'] for r in results) else 1


def run_files(d):
    """{file name: path} of a run folder's .pld files; a series folder (run-K inside) gives each run's."""
    runs = sorted(glob.glob(os.path.join(d, 'run-*'))) or [d]
    return [(r, {os.path.basename(p).lower(): p for p in glob.glob(os.path.join(r, '*.pld'))}) for r in runs]


def cmd_compare(a):
    retail = [x for d in a.retail for x in run_files(d)]
    ours = [x for d in (a.ours or []) for x in run_files(d)]
    retail = [x for x in retail if x[1]]
    if not retail:
        print('pldcheck: no retail .pld')
        return 1
    ref_dir, ref = retail[0]
    stable, same = True, True
    key = a.table
    print(f'pldcheck: reference {ref_dir} ({", ".join(sorted(ref))})' + (f'; verdict on {key}' if key else ''))
    for name in sorted(ref):
        base = load(ref[name])
        side = side_of(base['count'])
        print(f'{name}: count {base["count"]}' + (f' ({side} x {side})' if side else ''))
        others = []
        for d, files in retail[1:]:
            if name not in files:
                print(f'  retail {d}: MISSING')
                stable = False
                continue
            t = load(files[name])
            others.append(t)
            dd = diff_tables(base, t)
            stable &= not (dd if not key else {k: v for k, v in dd.items() if k in (key, 'count')})
            print(f'  retail {d}: {fmt_diff(dd)}')
        sp = spread(base, others)
        if sp:
            print('  the retail spread: ' + ', '.join(f'{k} {len(v)} entries' + (
                f' ({", ".join(f"{i % side},{i // side}" for i in sorted(v)[:8])})' if side and k != NORMALS[0] else '')
                for k, v in sp.items()))
        for d, files in ours:
            if name not in files:
                print(f'  ours ({a.label}) {d}: MISSING')
                same = False
                continue
            t = load(files[name])
            dd = diff_tables(base, t, skip=sp)
            inside = {k: len(set(v) & sp.get(k, set())) for k, v in diff_indices(base, t).items() if k != 'count'}
            same &= not (dd if not key else {k: v for k, v in dd.items() if k in (key, 'count')})
            print(f'  ours ({a.label}) {d}: {fmt_diff(dd, inside)}')
    what = key or 'every table'
    print(f'pldcheck: retail run to run {"STABLE" if stable else "UNSTABLE"} on {what} over {len(retail)} runs'
          + (f'; ours ({a.label}) {"SAME" if same else "DIFF"} on {what} over {len(ours)} runs'
             ' (outside the retail spread)' if ours else ''))
    return 0 if stable and same else 1


def cmd_show(a):
    t = load(a.file)
    side = side_of(t['count'])
    print(f'{a.file}: count {t["count"]}' + (f' ({side} x {side})' if side else '') + f', extra {len(t["extra"])} bytes')
    for name, size in TABLES:
        v = values(t[name], size)
        print(f'  {name}: {len(set(v))} distinct values, min {min(v)}, max {max(v)}')
    return 0


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    sub = ap.add_subparsers(dest='command', required=True)
    r = sub.add_parser('run')
    r.add_argument('--instance', required=True)
    r.add_argument('--retail', action='store_true')
    r.add_argument('--exe')
    r.add_argument('--runs', type=int, default=1)
    r.add_argument('--out', required=True)
    r.add_argument('--scenario', default='sp_land_pld')
    r.add_argument('--keep', action='store_true', help='keep the instance after the last run')
    r.add_argument('--admin-level', type=int, metavar='N',
                   help="Elanor's Net-7 admin level during each run, put back after it "
                        '(sp_land_pld_all: 90, the only level Net-7 lets /wormhole out of a planet surface)')
    c = sub.add_parser('compare')
    c.add_argument('--retail', nargs='+', required=True)
    c.add_argument('--ours', nargs='*')
    c.add_argument('--label', default='ours')
    c.add_argument('--table', choices=[t for t, _ in TABLES] + [NORMALS[0]],
                   help='the verdict on this table alone (B-29: normal_index_1E4); the rest printed')
    s = sub.add_parser('show')
    s.add_argument('file')
    a = ap.parse_args(argv)
    return {'run': cmd_run, 'compare': cmd_compare, 'show': cmd_show}[a.command](a)


if __name__ == '__main__':
    sys.exit(main())
