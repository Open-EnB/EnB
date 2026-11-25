"""The per-machine config: where this machine keeps what the harness reads outside the tree.

    python tools/harness/machine.py        prints each key's resolved path, where it came from and whether it exists

Keys (docs/test-plan.md section 1, "usable on any device"):
    client_data      ClientPOC: release\\ (the DLLs) and Data\\ (Net-7's edited data set) that clientrun.py runs over
    ea_data          ClientPOC_EA: the same over EA's final data set (--ea)
    retail_install   the game's install (read only, never a working directory: D69 / D143)
    antprox_dir      the folder of the proxy pair the harness runs: the store's tools/antprox (<baselines>/tools/
                     antprox: antprox-rs.exe, antreplay.exe and their manifest.json) by default
    antprox          the antprox live runs (--live) use: <antprox_dir>/antprox-rs.exe (the Rust port 1.2); without
                     that folder, AntaresProxy-Rust's target\release\antprox-rs.exe
    antprox_replay   the antprox replay runs (/REPLAY) use: <antprox_dir>/antprox.exe (the C# 1.1.0 release); without
                     that folder, AntaresProxy's releases\antprox-1.1.0-win-x64\antprox.exe. The split exists only
                     because antprox-rs 1.2 cannot replay ("the original profile (P1) is not built yet"); it goes
                     when it can (the user, 2026-11-25). ANTPROX_EXE overrides whichever of the two a run uses
    antreplay        the replay server antprox /REPLAY starts: <antprox_dir>/antreplay.exe (C# 1.1.0); antprox looks
                     for it beside itself, so clientrun.py passes /REPLAYEXE: when it is elsewhere; without the
                     folder, AntaresProxy's releases\antprox-1.1.0-win-x64\antreplay.exe
    net7_bin         the local Net-7 server's bin folder (--live)
    baselines        the checkout of the baseline store, Open-EnB/baselines (tools/harness/baseline.py)
    captures         AntaresProxy's replay captures
    recordings       the folder antprox /REPLAY reads its recordings from (/RECORDINGS:): the store's copy of the
                     replay set, <baselines>/inputs/captures by default (clientrun.py passes it when it exists)
    label            this machine's name in the store's <env> folder (default: a slug of the GPU's name)
    net7_control_port  Net7.exe's loopback control port (e2e.py db_restore); unset: `control_port=` in
                     <net7_bin>/Net7Config.cfg, else 3810 (Net7's ControlPort.cpp); 0: off

Each key resolves, first match wins: its environment variable (ENV below; ENB_RETAIL_INSTALL and ANTPROX_EXE are
the names the tools used before this file) > the config file (ENB_MACHINE, else ~/.enb/machine.json; a JSON object
of the keys above, machine.example.json) > a default under BASE, the folder that holds the MAIN checkout of this
repository (in a git worktree, ROOT/.git is a file: its gitdir leads to the main checkout's .git). Nothing runs a
subprocess at import; only `label`'s default asks the GPU (Windows; elsewhere 'unknown').
"""
import json
import os
import re
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(os.path.dirname(HERE))

ENV = {
    'client_data': 'ENB_CLIENT_DATA',
    'ea_data': 'ENB_EA_DATA',
    'retail_install': 'ENB_RETAIL_INSTALL',
    'antprox_dir': 'ENB_ANTPROX_DIR',
    'antprox': 'ANTPROX_EXE',
    'antprox_replay': 'ENB_ANTPROX_REPLAY',
    'antreplay': 'ENB_ANTREPLAY',
    'net7_bin': 'ENB_NET7_BIN',
    'baselines': 'ENB_BASELINES',
    'captures': 'ENB_CAPTURES',
    'recordings': 'ENB_RECORDINGS',
    'label': 'ENB_MACHINE_LABEL',
    'net7_control_port': 'ENB_NET7_CONTROL_PORT',
}
KEYS = list(ENV)
DIR_EXES = {'antprox': 'antprox-rs.exe', 'antprox_replay': 'antprox.exe',
            'antreplay': 'antreplay.exe'}                                   # their defaults inside antprox_dir
OVERRIDES = {'antprox_replay': 'ANTPROX_EXE'}   # read before the key's own variable: ANTPROX_EXE names the one proxy
VALUE_KEYS = ('label', 'net7_control_port')        # not paths
PATH_KEYS = [k for k in KEYS if k not in VALUE_KEYS]


def main_checkout(root=ROOT):
    """The main checkout of the repository at root: root itself, or for a worktree (root/.git a file
    'gitdir: <main>/.git/worktrees/NAME') the folder holding the common .git."""
    dotgit = os.path.join(root, '.git')
    if not os.path.isfile(dotgit):
        return os.path.abspath(root)
    try:
        with open(dotgit, encoding='utf-8') as f:
            line = f.read().strip()
    except OSError:
        return os.path.abspath(root)
    if not line.startswith('gitdir:'):
        return os.path.abspath(root)
    gitdir = line[7:].strip()
    if not os.path.isabs(gitdir):
        gitdir = os.path.join(root, gitdir)
    common = os.path.join(gitdir, 'commondir')
    if os.path.isfile(common):
        with open(common, encoding='utf-8') as f:
            c = f.read().strip()
        main_git = c if os.path.isabs(c) else os.path.join(gitdir, c)
    else:
        main_git = os.path.join(gitdir, '..', '..')
    return os.path.dirname(os.path.normpath(os.path.abspath(main_git)))


def base(root=ROOT):
    return os.path.dirname(main_checkout(root))


def defaults(root=ROOT):
    b = base(root)
    return {
        'client_data': os.path.join(b, 'ClientPOC'),
        'ea_data': os.path.join(b, 'ClientPOC_EA'),
        'retail_install': r'C:\Program Files (x86)\EA GAMES\Earth & Beyond',
        # the Rust antprox with the C# antreplay 1.1.0 (the user, 2026-11-24 / 25), from antprox_dir when that folder
        # exists (resolve); these are the fallbacks without it
        'antprox_dir': None,             # <baselines>/tools/antprox when asked
        'antprox': os.path.join(b, 'AntaresProxy-Rust', 'target', 'release', 'antprox-rs.exe'),
        'antprox_replay': os.path.join(b, 'AntaresProxy', 'releases', 'antprox-1.1.0-win-x64', 'antprox.exe'),
        'antreplay': os.path.join(b, 'AntaresProxy', 'releases', 'antprox-1.1.0-win-x64', 'antreplay.exe'),
        'net7_bin': os.path.join(b, 'Net7', 'bin_local'),
        'baselines': os.path.join(b, 'baselines'),
        'captures': os.path.join(b, 'AntaresProxy', 'captures'),
        'recordings': None,              # <baselines>/inputs/captures when asked (baselines resolved first)
        'label': None,                   # gpu_label() when asked
        'net7_control_port': None,       # e2e.net7_control_port reads Net7Config.cfg then
    }


def config_path(environ=None):
    environ = os.environ if environ is None else environ
    return environ.get('ENB_MACHINE') or os.path.join(os.path.expanduser('~'), '.enb', 'machine.json')


def load_config(path):
    if not path or not os.path.isfile(path):
        return {}
    with open(path, encoding='utf-8') as f:
        data = json.load(f)
    if not isinstance(data, dict):
        raise SystemExit(f'{path}: not a JSON object')
    unknown = sorted(set(data) - set(KEYS) - {'comment'})
    if unknown:
        raise SystemExit(f'{path}: unknown keys {unknown} (known: {KEYS})')
    return data


def resolve(key, environ=None, root=ROOT):
    """-> (value, source): source 'env NAME', 'config PATH' or 'default'."""
    if key not in ENV:
        raise KeyError(key)
    environ = os.environ if environ is None else environ
    for name in (OVERRIDES.get(key), ENV[key]):
        v = environ.get(name) if name else None
        if v:
            return v, 'env ' + name
    cfg = config_path(environ)
    data = load_config(cfg)
    if data.get(key) or (key == 'net7_control_port' and data.get(key) is not None):
        v = data[key]
        if key == 'net7_control_port':
            return str(v), 'config ' + cfg
        if key != 'label' and not os.path.isabs(v):
            v = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(cfg)), v))
        return v, 'config ' + cfg
    v = defaults(root)[key]
    if key == 'label' and v is None:
        v = gpu_label()
    elif key == 'recordings' and v is None:
        v = os.path.join(resolve('baselines', environ, root)[0], 'inputs', 'captures')
    elif key == 'antprox_dir':
        v = os.path.join(resolve('baselines', environ, root)[0], 'tools', 'antprox')
    elif key in DIR_EXES:
        d = resolve('antprox_dir', environ, root)[0]
        if os.path.isdir(d):
            return os.path.join(d, DIR_EXES[key]), 'default (antprox_dir)'
        return v, 'default: no antprox_dir folder, the fallback'
    return v, 'default'


def get(key):
    return resolve(key)[0]


# ---------------------------------------------------------------- the machine itself (Windows; None elsewhere)

def gpu_info():
    """{'name', 'driver'} of the display adapter that drives a screen (Win32_VideoController), else None."""
    if sys.platform != 'win32':
        return None
    try:
        r = subprocess.run(['powershell', '-NoProfile', '-Command',
                            'Get-CimInstance Win32_VideoController | Select-Object Name,DriverVersion,'
                            'CurrentHorizontalResolution | ConvertTo-Json'],
                           capture_output=True, text=True, timeout=60)
        data = json.loads(r.stdout)
    except (OSError, ValueError, subprocess.SubprocessError):
        return None
    if isinstance(data, dict):
        data = [data]
    live = [d for d in data if d.get('CurrentHorizontalResolution')] or data
    if not live:
        return None
    return {'name': live[0].get('Name'), 'driver': live[0].get('DriverVersion')}


def slug(text):
    s = re.sub(r'[^a-z0-9]+', '-', (text or '').lower()).strip('-')
    for w in ('nvidia-', 'amd-', 'intel-r-', 'intel-'):
        if s.startswith(w):
            s = s[len(w):]
    return s or 'unknown'


def gpu_label():
    g = gpu_info()
    return slug(g['name']) if g else 'unknown'


def os_version():
    import platform
    if sys.platform == 'win32':
        v = sys.getwindowsversion()
        return f'Windows {v.major}.{v.minor}.{v.build}'
    return platform.platform()


def main():
    for k in KEYS:
        v, src = resolve(k)
        state = '' if k in VALUE_KEYS or v is None else ('ok     ' if os.path.exists(v) else 'MISSING')
        print(f'{k:15} {state:7} {v}  ({src})')
    print(f'config file: {config_path()} ({"present" if os.path.isfile(config_path()) else "absent"}); '
          f'BASE {base()}')
    print('antprox serves live runs and antprox_replay replay runs only because antprox-rs 1.2 cannot replay yet ("the '
          'original profile (P1) is not built yet"); one proxy again when it can')


if __name__ == '__main__':
    main()
