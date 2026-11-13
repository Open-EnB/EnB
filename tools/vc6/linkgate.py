"""Link check (pipeline-plan §4 item 6b): build app.lib in a tree, link the three programs there, and refuse on a
duplicate definition or an unresolved symbol the baseline does not list.

    python tools/vc6/linkgate.py check [--tree DIR] [--pool K] [--lib X ...] [--write-baseline]

In TREE (default: the tree holding this script; wt.py bring-over: its scratch worktree NAME.bo), on K VMs leased
from pool.py (vc6lock.pool_lease, released afterwards; default 1):
  1. the non-app libraries the tree lacks are copied from the main tree's build/ (as `wt.py new` does);
  2. `build.py build --lib app --allow-dirty` (plus --lib X: the other libraries the landing touched), so app.lib is
     the tree's own;
  3. `build.py link --program clientpoc --no-deploy` (its gen_stubs second pass and /force:unresolved as always; the
     POC's release folder is not touched);
  4. `clienttest.py build` (with its own gen_stubs pass);
  5. `udpreplay.py build` (app\wwudp and the connection sources compiled into the program, against the tree's
     libraries; D244, P-H1-1: a landing that breaks udpreplay's link is refused like one that breaks the POC's).
Each step logs to LOG.<step>.log. Then judge() reads build/programs/{clientpoc,clienttest,udpreplay}/summary.json:
refused on
a compile error, any LNK2005 / LNK1169 (a duplicate definition), an unresolved symbol (mangled name; after the
gen_stubs pass, i.e. what gen_stubs cannot write) not in BASELINE, or no exe. Symbols of the baseline that are gone
are printed (update it with --write-baseline).

BASELINE: tools/vc6/link_baseline.json, read from TREE: {program: [mangled names]}, the unresolved symbols a link
of the run branch leaves (templates and by-value classes gen_stubs skips: the POC's known set). A recorded
allowlist rather than a second link of the target, which would double the cost; a landing that must add one says
so by changing the file (it shows in its diff). --write-baseline rewrites it from this run's summaries.
"""
import argparse
import json
import os
import shutil
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
TREE = os.path.dirname(os.path.dirname(HERE))
BASELINE = 'tools/vc6/link_baseline.json'
PROGRAMS = ('clientpoc', 'clienttest', 'udpreplay')
TOUCH = ('app/', 'stubs/', 'programs/clientpoc/', 'tests/clienttest/', 'tests/udpreplay/')
DUPLICATE = ('LNK2005', 'LNK1169')


def touches(paths):
    """Whether any of PATHS is one the link depends on (the app, the stubs, the three programs' sources)."""
    return any(p.replace('\\', '/').startswith(TOUCH) for p in paths)


def main_root(tree):
    r = subprocess.run(['git', 'rev-parse', '--path-format=absolute', '--git-common-dir'], cwd=tree,
                       capture_output=True, text=True)
    return os.path.normpath(os.path.dirname(r.stdout.strip())) if r.returncode == 0 else tree


def read_baseline(tree):
    p = os.path.join(tree, *BASELINE.split('/'))
    if not os.path.exists(p):
        return {}
    with open(p, encoding='utf-8') as f:
        return {k: v for k, v in json.load(f).items() if k in PROGRAMS}


def write_baseline(tree, summaries):
    data = {prog: sorted(s['unresolved']) for prog, s in summaries.items() if s}
    with open(os.path.join(tree, *BASELINE.split('/')), 'w', newline='\n', encoding='utf-8') as f:
        json.dump(data, f, indent=1)
        f.write('\n')


def judge(summaries, baseline):
    """(problems, notes) from {program: summary.json's dict or None} against {program: [unresolved]}."""
    problems, notes = [], []
    for prog in PROGRAMS:
        s = summaries.get(prog)
        if not s:
            problems.append(f'{prog}: no link summary (the build or the link stopped before it; see the logs)')
            continue
        if s.get('cl_errors'):
            problems.append(f'{prog}: {len(s["cl_errors"])} compile error(s): ' + '; '.join(s['cl_errors'][:3]))
        dups = [l for l in s.get('link_errors', []) if any(c in l for c in DUPLICATE)]
        if dups:
            problems.append(f'{prog}: duplicate definitions ({len(dups)} line(s)): ' + '; '.join(dups[:5]))
        known = set(baseline.get(prog, ()))
        new = sorted(set(s.get('unresolved', {})) - known)
        if new:
            problems.append(f'{prog}: {len(new)} unresolved symbol(s) not in {BASELINE}: ' + ', '.join(new[:10]))
        gone = sorted(known - set(s.get('unresolved', {})))
        if gone:
            notes.append(f'{prog}: {len(gone)} baseline symbol(s) now resolved (--write-baseline drops them): '
                         + ', '.join(gone[:5]))
        if not s.get('exe') and not dups and not new and not s.get('cl_errors'):
            problems.append(f'{prog}: no exe written ({"; ".join(s.get("link_errors", [])[:3])})')
        if s.get('exe') and not new and not dups:
            notes.append(f'{prog}: linked, {len(s.get("unresolved", {}))} unresolved (all in the baseline)')
    return problems, notes


def summary_path(tree, prog):
    return os.path.join(tree, 'build', 'programs', prog, 'summary.json')


def furnish_libs(tree):
    """The main tree's non-app libraries the tree lacks (build/X/X.lib), copied in; the copied names."""
    root = main_root(tree)
    bd = os.path.join(root, 'build')
    out = []
    if os.path.normcase(os.path.abspath(root)) == os.path.normcase(os.path.abspath(tree)) or not os.path.isdir(bd):
        return out
    for lib in sorted(os.listdir(bd)):
        f = os.path.join(bd, lib, lib + '.lib')
        dst = os.path.join(tree, 'build', lib, lib + '.lib')
        if lib != 'app' and os.path.isfile(f) and not os.path.exists(dst):
            os.makedirs(os.path.dirname(dst), exist_ok=True)
            shutil.copyfile(f, dst)
            out.append(lib)
    return out


def steps(tree, libs):
    py = sys.executable
    build = [py, 'tools/vc6/build.py', 'build', '--allow-dirty'] + [x for lib in ['app', *libs] for x in ('--lib', lib)]
    return [('build', build),
            ('clientpoc', [py, 'tools/vc6/build.py', 'link', '--program', 'clientpoc', '--no-deploy']),
            ('clienttest', [py, 'tools/harness/clienttest.py', 'build']),
            ('udpreplay', [py, 'tools/harness/udpreplay.py', 'build'])]


def run(tree, libs=(), pool=1, log=None, runner=None):
    """The build and the three links in TREE (on POOL leased VMs; 0: the hosts already set); {program: summary or
    None}. RUNNER(args, cwd, logfile) -> returncode (tests stub it)."""
    log = log or os.path.join(tree, '.work', 'linkgate')
    runner = runner or _run_logged
    copied = furnish_libs(tree)
    if copied:
        print(f'link check: copied the main tree\'s {", ".join(copied)} libraries', flush=True)
    for prog in PROGRAMS:
        if os.path.exists(summary_path(tree, prog)):
            os.remove(summary_path(tree, prog))

    def go():
        for tag, args in steps(tree, libs):
            print(f'link check: {tag} (log {log}.{tag}.log)', flush=True)
            rc = runner(args, tree, f'{log}.{tag}.log')
            if tag == 'build' and rc:
                print(f'link check: the app build failed ({rc}); see {log}.build.log', flush=True)
                return
    if pool:
        sys.path.insert(0, HERE)
        import vc6lock
        with vc6lock.pool_lease(pool):
            go()
    else:
        go()
    out = {}
    for prog in PROGRAMS:
        p = summary_path(tree, prog)
        out[prog] = None
        if os.path.exists(p):
            with open(p, encoding='utf-8') as f:
                out[prog] = json.load(f)
    return out


def _run_logged(args, cwd, logfile):
    os.makedirs(os.path.dirname(logfile), exist_ok=True)
    with open(logfile, 'w', newline='\n', encoding='utf-8', errors='replace') as f:
        return subprocess.run(args, cwd=cwd, stdout=f, stderr=subprocess.STDOUT).returncode


def gate(tree, libs=(), pool=1, log=None, runner=None):
    """run() then judge() against TREE's baseline; prints the notes; returns the problems."""
    summaries = run(tree, libs, pool, log, runner)
    problems, notes = judge(summaries, read_baseline(tree))
    for n in notes:
        print('link check:', n)
    return problems, summaries


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    ap.add_argument('cmd', choices=['check'])
    ap.add_argument('--tree', default=TREE)
    ap.add_argument('--pool', type=int, default=1, help='VMs to lease (0: use ENB_VC6_HOSTS as set)')
    ap.add_argument('--lib', action='append', default=[], help='build this library too (one the change touched)')
    ap.add_argument('--log', help='log prefix (default TREE/.work/linkgate)')
    ap.add_argument('--write-baseline', action='store_true', help=f'rewrite {BASELINE} from this run')
    a = ap.parse_args(argv)
    tree = os.path.abspath(a.tree)
    problems, summaries = gate(tree, a.lib, a.pool, a.log)
    if a.write_baseline and all(summaries.values()):
        write_baseline(tree, summaries)
        print(f'link check: {BASELINE} written')
    for p in problems:
        print('link check: REFUSED:', p)
    if not problems:
        print('link check: passed')
    return 1 if problems else 0


if __name__ == '__main__':
    sys.exit(main())
