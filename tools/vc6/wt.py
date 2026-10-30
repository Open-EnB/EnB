"""Builder worktrees (D113): make one, list them, bring a branch's verified commits over, remove one.

    python tools/vc6/wt.py new NAME [--base REV]
    python tools/vc6/wt.py list
    python tools/vc6/wt.py bring-over NAME [--target BRANCH] [--lib X ...] [--pool K] [--no-replay] [--dry] [--keep]
    python tools/vc6/wt.py remove NAME [--delete-branch] [--force]

new: `git worktree add .work/wt/NAME -b wt/NAME BASE` (default HEAD); NTFS junctions .work/locks,
  .work/pool, .work/pseudo to the main tree's (one importer lock, one VM pool, Ghidra's saved
  pseudocode); copies .work/vc6_hosts.json, .work/harness/*.dgram and build/*/*.lib except app's
  (udpreplay needs app.lib: `build.py build --lib app --allow-dirty` in the worktree); records BASE in
  .work/wt/NAME.base. importer.py run inside the worktree takes NAME as its agent (scratch under
  the worktree's .work/NAME/tmp): `new` checks that.

bring-over: puts wt/NAME's commits on TARGET (default master) only when that gains pairs and loses
  none, never touching TARGET otherwise:
  1. the commits of wt/NAME not yet on TARGET (patch-equivalent ones skipped, merges refused);
  2. TARGET at the recorded base (or an ancestor of the branch): fast-forward to the branch tip;
     else cherry-pick them, in a scratch worktree .work/wt/NAME.bo at TARGET, where conflicts in
     docs/data/attempts.csv are resolved by the union of the rows, names-review.md / not-exact.md
     are regenerated (tools/re/names.py) and any other conflict (imports.json included) refuses;
  3. the libraries the commits touched (their directories, EA headers -> app as well, importer.py
     lines naming a library, imports.json blocks that changed; plus --lib) are verified there:
     `importer.py verify --lib ... --pool K`, then `importer.py replay` (0 problems, 0 files differ);
  4. pairs: the verified imports.json against TARGET's (nothing lost) and against the candidate's own
     record (equal: the commits record what the verify gives); with no library touched, the
     candidate's record against TARGET's;
  5. landing: under the importer lock, the worktree that has TARGET checked out (the main tree for
     master) gets each changed path 3-way merged with its uncommitted copy (git merge-file; attempts.csv
     by row union; an imports.json conflict on count lines only keeps the working copy's counts; any
     other conflict refuses), then `update-ref refs/heads/TARGET NEW OLD` (compare-and-swap), then the merged files
     are written and their index entries reset to the new HEAD.
  --dry stops before landing; --keep keeps the scratch worktree; logs go to .work/wt/NAME.bo.*.log.

remove: removes every junction inside the worktree first (rmdir, never a recursive delete through
  one), then `git worktree remove --force`, prune, the .base record; --delete-branch deletes wt/NAME.
  A dirty worktree is refused unless --force.

list: every worktree but the main one: name, recorded base, commits ahead of it, dirty or clean.

Promoted from .work/eval/setup.py and .work/p9/wt.py (setup), the coordinator's hand bring-over of
F6's branch (4f2a8b1) and commit_mine.py's compare-and-swap; K5, 2026-10-27.
"""
import argparse
import json
import os
import re
import shutil
import socket
import subprocess
import sys
import tempfile
import time

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import commit_rows as cr  # noqa: E402


def _main_root():
    """The main tree's root, also when this copy of wt.py runs from a worktree."""
    r = subprocess.run(['git', 'rev-parse', '--path-format=absolute', '--git-common-dir'], cwd=HERE,
                       capture_output=True, text=True)
    if r.returncode == 0 and r.stdout.strip():
        return os.path.normpath(os.path.dirname(r.stdout.strip()))
    return os.path.normpath(os.path.join(HERE, '..', '..'))


ROOT = _main_root()
WTDIR = os.path.join(ROOT, '.work', 'wt')
JUNCTIONS = ('locks', 'pool', 'pseudo')
IMPORTS = 'docs/data/imports.json'
ATTEMPTS = 'docs/data/attempts.csv'
NAMES_OUT = ('docs/data/names-review.md', 'docs/data/not-exact.md')
LOCK = os.path.join(ROOT, '.work', 'locks', 'importer.lock')
# top-level directories of the libraries (importer.py LIB_TREE / APP_LIBS)
LIB_TOP = {'app': 'app', 'binkmovie': 'binkmovie', 'ww3d2': 'ww3d2', 'wwaudio': 'wwaudio', 'wwdebug': 'wwdebug',
           'wwlib': 'wwlib', 'wwmath': 'wwmath', 'wwsaveload': 'wwsaveload', 'wwshade': 'wwshade',
           'libs/wwonline/wlib': 'wlib', 'libs/wwonline/wnet': 'wnet'}
EA_LIBS = {'binkmovie', 'ww3d2', 'wwaudio', 'wwdebug', 'wwlib', 'wwmath', 'wwsaveload', 'wwshade', 'wlib', 'wnet'}
COUNT_LINE = re.compile(r'^\s*"(ambiguous_total|in_tree_total)": \d+,?\s*$')


def git(*args, cwd=ROOT, check=True, inp=None, env=None):
    """git's stdout as text (bytes when inp is bytes)."""
    r = subprocess.run(['git', *args], cwd=cwd, input=inp, capture_output=True, env=env)
    if check and r.returncode:
        sys.exit(f'wt: git {" ".join(args)} (in {cwd}): {r.stderr.decode(errors="replace").strip()}')
    return r if not check else r.stdout.decode('utf-8', 'surrogateescape')


def ok(*args, cwd=ROOT):
    return subprocess.run(['git', *args], cwd=cwd, capture_output=True).returncode == 0


def rev(r, cwd=ROOT):
    return git('rev-parse', '--verify', '-q', r + '^{commit}', cwd=cwd).strip()


def wt_path(name):
    return os.path.join(WTDIR, name)


def base_file(name):
    return os.path.join(WTDIR, name + '.base')


def check_name(name):
    if not re.fullmatch(r'[A-Za-z0-9][A-Za-z0-9._-]*', name) or name.endswith('.bo'):
        sys.exit(f'wt: bad worktree name {name!r} (letters, digits, . _ -; not ending in .bo)')


# ---------------------------------------------------------------- setup and removal

def _junction(dst, src):
    if os.path.lexists(dst):
        return
    r = subprocess.run(['cmd', '/c', 'mklink', '/J', dst, src], capture_output=True, text=True)
    if r.returncode:
        sys.exit(f'wt: mklink /J {dst} {src}: {r.stdout}{r.stderr}')


def furnish(path, libs=True, pseudo=True):
    """Junctions, VM hosts, captures and the non-app libraries for the worktree at path."""
    os.makedirs(os.path.join(path, '.work', 'harness'), exist_ok=True)
    for j in JUNCTIONS:
        src = os.path.join(ROOT, '.work', j)
        if (j != 'pseudo' or pseudo) and os.path.isdir(src):
            _junction(os.path.join(path, '.work', j), src)
    hosts = os.path.join(ROOT, '.work', 'vc6_hosts.json')
    if os.path.exists(hosts):
        shutil.copyfile(hosts, os.path.join(path, '.work', 'vc6_hosts.json'))
    if not libs:
        return
    hd = os.path.join(ROOT, '.work', 'harness')
    for g in sorted(os.listdir(hd)) if os.path.isdir(hd) else ():
        if g.endswith('.dgram'):
            shutil.copyfile(os.path.join(hd, g), os.path.join(path, '.work', 'harness', g))
    bd = os.path.join(ROOT, 'build')
    for lib in sorted(os.listdir(bd)) if os.path.isdir(bd) else ():
        f = os.path.join(bd, lib, lib + '.lib')
        if lib != 'app' and os.path.isfile(f):
            os.makedirs(os.path.join(path, 'build', lib), exist_ok=True)
            shutil.copyfile(f, os.path.join(path, 'build', lib, lib + '.lib'))


def importer_agent(path):
    """The agent importer.py takes when run in path (its _worktree_agent default)."""
    code = 'import sys, warnings; warnings.simplefilter("ignore"); sys.path.insert(0, "tools/vc6"); ' \
           'import importer; print(importer._AGENT)'
    env = {k: v for k, v in os.environ.items() if k != 'ENB_AGENT'}
    r = subprocess.run([sys.executable, '-c', code], cwd=path, capture_output=True, text=True, env=env)
    return r.stdout.strip().splitlines()[-1] if r.returncode == 0 and r.stdout.strip() else f'? ({r.stderr.strip()[-200:]})'


def cmd_new(a):
    check_name(a.name)
    path = wt_path(a.name)
    if os.path.lexists(path):
        sys.exit(f'wt: {path} exists')
    if ok('rev-parse', '--verify', '-q', 'refs/heads/wt/' + a.name):
        sys.exit(f'wt: branch wt/{a.name} exists (wt.py remove {a.name} --delete-branch, or another name)')
    base = rev(a.base)
    os.makedirs(WTDIR, exist_ok=True)
    git('worktree', 'add', path, '-b', 'wt/' + a.name, base)
    furnish(path)
    with open(base_file(a.name), 'w', newline='\n') as f:
        f.write(base + '\n')
    agent = importer_agent(path)
    print(f'worktree {path}\nbranch   wt/{a.name} at {base[:9]} ({git("log", "-1", "--format=%s", base).strip()[:70]})')
    print(f'importer agent there: {agent}' + ('' if agent == a.name else f'  (WARNING: expected {a.name})'))
    print(f'hint: work in it by absolute path; importer.py verify there runs as --agent {a.name} by default; '
          f'udpreplay needs `python tools/vc6/build.py build --lib app --allow-dirty` there first; '
          f'land with `python {os.path.join(ROOT, "tools", "vc6", "wt.py")} bring-over {a.name}`')


def _links_inside(path):
    """Junctions and directory symlinks under path, without descending into them."""
    found, stack = [], [path]
    while stack:
        d = stack.pop()
        try:
            it = list(os.scandir(d))
        except OSError:
            continue
        for e in it:
            p = e.path
            if os.path.isjunction(p) or e.is_symlink():
                found.append(p)
            elif e.is_dir(follow_symlinks=False) and e.name != '.git':
                stack.append(p)
    return found


def unlink_all(path):
    """Remove every junction / directory link inside path (the link only, never its target)."""
    for p in _links_inside(path):
        if os.path.isjunction(p):
            subprocess.run(['cmd', '/c', 'rmdir', p], capture_output=True)
        else:
            try:
                os.rmdir(p) if os.path.isdir(p) else os.unlink(p)
            except OSError:
                pass
    left = _links_inside(path)
    if left:
        sys.exit(f'wt: could not remove the links {left}; nothing deleted')


def drop_worktree(path):
    unlink_all(path)
    git('worktree', 'remove', '--force', path, check=False)
    if os.path.isdir(path):
        unlink_all(path)            # re-check before the only recursive delete
        shutil.rmtree(path, ignore_errors=True)
    git('worktree', 'prune')


def cmd_remove(a):
    path = wt_path(a.name)
    if os.path.isdir(path):
        dirty = git('status', '--porcelain', cwd=path, check=False)
        if dirty.returncode == 0 and dirty.stdout.strip() and not a.force:
            sys.exit(f'wt: {path} has uncommitted changes:\n{dirty.stdout.decode(errors="replace")}'
                     'commit or drop them, or pass --force')
        drop_worktree(path)
        print(f'removed {path}')
    else:
        git('worktree', 'prune')
        print(f'no worktree at {path}')
    bo = path + '.bo'
    if os.path.isdir(bo):
        drop_worktree(bo)
        print(f'removed {bo}')
    if os.path.exists(base_file(a.name)):
        os.remove(base_file(a.name))
    if a.delete_branch and ok('rev-parse', '--verify', '-q', 'refs/heads/wt/' + a.name):
        print(git('branch', '-D', 'wt/' + a.name).strip())


def worktrees():
    """(path, branch or None, head) of every worktree but the main tree."""
    out, cur = [], {}
    for line in git('worktree', 'list', '--porcelain').splitlines() + ['']:
        if not line:
            if cur:
                out.append(cur)
            cur = {}
        else:
            k, _, v = line.partition(' ')
            cur[k] = v
    res = []
    for w in out:
        p = os.path.normpath(w['worktree'])
        if os.path.normcase(p) == os.path.normcase(ROOT):
            continue
        b = w.get('branch', '')
        res.append((p, b[len('refs/heads/'):] if b.startswith('refs/heads/') else None, w.get('HEAD', '')))
    return res


def cmd_list(a):
    rows = [('name', 'branch', 'base', 'ahead', 'state', 'path')]
    for p, b, head in worktrees():
        name = os.path.basename(p)
        base = '-'
        if os.path.normcase(os.path.dirname(p)) == os.path.normcase(WTDIR) and os.path.exists(base_file(name)):
            base = open(base_file(name)).read().strip()
        if base == '-' and b:
            base = git('merge-base', 'master', b, check=False).stdout.decode().strip() or '-'
        ahead = git('rev-list', '--count', f'{base}..{head}').strip() if base != '-' else '?'
        st = git('status', '--porcelain', '-uno', cwd=p, check=False)
        n = len(st.stdout.splitlines()) if st.returncode == 0 else -1
        state = 'clean' if n == 0 else ('?' if n < 0 else f'dirty ({n})')
        rows.append((name, b or '(detached)', base[:9], ahead, state, os.path.relpath(p, ROOT)))
    w = [max(len(r[i]) for r in rows) for i in range(6)]
    for r in rows:
        print('  '.join(c.ljust(w[i]) for i, c in enumerate(r)).rstrip())


# ---------------------------------------------------------------- bring-over

def pairs_of(text):
    """(lib, file, address) triples of an imports.json text (pairs_cmp.py's set)."""
    data = json.loads(text) if text else {}
    out = set()
    for lib, entry in data.items():
        for f, v in entry.get('files', {}).items():
            for x in v.get('in_tree', []):
                out.add((lib, f, x if isinstance(x, str) else str(x)))
    return out


def blob(spec, cwd=ROOT):
    r = git('cat-file', 'blob', spec, cwd=cwd, check=False)
    return r.stdout.decode('utf-8', 'surrogateescape') if r.returncode == 0 else None


def union_rows(base, ours, theirs):
    """attempts.csv: OURS plus the records THEIRS added over BASE (none removed)."""
    have = {raw.rstrip('\r\n') for raw, _ in cr.csv_records(ours)}
    old = {raw.rstrip('\r\n') for raw, _ in cr.csv_records(base or '')}
    eol = '\r\n' if '\r\n' in ours else '\n'
    out = ours if ours.endswith(('\n', '\r\n')) or not ours else ours + eol
    for raw, row in cr.csv_records(theirs):
        k = raw.rstrip('\r\n')
        if row is not None and k not in have and k not in old:
            out += k + eol
            have.add(k)
    return out


def libs_touched(old, new, extra):
    libs = set(extra)
    names = git('diff', '--name-only', old, new).splitlines()
    for p in names:
        for top, lib in LIB_TOP.items():
            if p.lower().startswith(top + '/'):
                libs.add(lib)
                if lib in EA_LIBS and p.lower().endswith(('.h', '.hpp', '.inl')):
                    libs.add('app')         # the app includes the EA headers (ref_ptr.h)
        if p.startswith(('stubs/', 'app/')):
            libs.add('app')
    if 'tools/vc6/importer.py' in names:
        d = git('diff', '-U0', old, new, '--', 'tools/vc6/importer.py')
        changed = '\n'.join(l for l in d.splitlines() if l[:1] in '+-' and not l.startswith(('+++', '---')))
        for top, lib in LIB_TOP.items():
            if re.search(r"(?i)(?<![\w/])(%s)[/\\]|'%s'" % (re.escape(top.split('/')[-1]), lib), changed):
                libs.add(lib)
    a, b = blob(f'{old}:{IMPORTS}'), blob(f'{new}:{IMPORTS}')
    if a != b:
        ja, jb = json.loads(a or '{}'), json.loads(b or '{}')
        for lib in set(ja) | set(jb):
            if ja.get(lib, {}).get('files') != jb.get(lib, {}).get('files'):
                libs.add(lib)
    return sorted(libs)


def run_logged(args, cwd, log, env=None):
    with open(log, 'w', newline='\n', encoding='utf-8', errors='replace') as f:
        r = subprocess.run(args, cwd=cwd, stdout=f, stderr=subprocess.STDOUT, env=env)
    tail = open(log, encoding='utf-8', errors='replace').read().splitlines()[-3:]
    return r.returncode, tail


def cherry_pick(bo, commits, log):
    """Cherry-pick commits in the scratch worktree; resolve the row files; None or the refusal."""
    env = dict(os.environ, GIT_EDITOR='true')
    touched_names = False
    for c in commits:
        subj = git('log', '-1', '--format=%h %s', c).strip()[:90]
        r = git('cherry-pick', c, cwd=bo, check=False, env=env)
        log.write(f'cherry-pick {subj}: {r.returncode}\n')
        if set(NAMES_OUT) & set(git('diff-tree', '--no-commit-id', '--name-only', '-r', c).splitlines()):
            touched_names = True
        if r.returncode == 0:
            continue
        conflicted = git('diff', '--name-only', '--diff-filter=U', cwd=bo).splitlines()
        if not conflicted and not git('diff', '--cached', '--name-only', cwd=bo).strip():
            git('cherry-pick', '--skip', cwd=bo, env=env)       # empty once applied: already there
            log.write('  empty, skipped\n')
            continue
        other = [p for p in conflicted if p != ATTEMPTS and p not in NAMES_OUT]
        if other:
            git('cherry-pick', '--abort', cwd=bo, check=False)
            return f'cherry-pick of {subj} conflicts in {", ".join(other)} (refused; rebase the branch by hand)'
        if ATTEMPTS in conflicted:
            merged = union_rows(blob(':1:' + ATTEMPTS, bo), blob(':2:' + ATTEMPTS, bo) or '',
                                blob(':3:' + ATTEMPTS, bo) or '')
            sha = git('hash-object', '-w', '--no-filters', '--stdin', cwd=bo,
                      inp=merged.encode('utf-8', 'surrogateescape')).strip()
            git('update-index', '--cacheinfo', f'100644,{sha},{ATTEMPTS}', cwd=bo)
            git('checkout-index', '-f', '--', ATTEMPTS, cwd=bo)
            log.write('  attempts.csv: rows unioned\n')
        names = [p for p in conflicted if p in NAMES_OUT]
        if names:
            git('checkout', '--ours', '--', *names, cwd=bo)
            rc, tail = run_logged([sys.executable, 'tools/re/names.py'], bo, log.name + '.names')
            if rc:
                git('cherry-pick', '--abort', cwd=bo, check=False)
                return f'names.py failed while resolving {subj}: {tail}'
            git('add', '--', *NAMES_OUT, cwd=bo)
            log.write('  names-review.md / not-exact.md regenerated\n')
        if not git('diff', '--cached', '--name-only', 'HEAD', cwd=bo).strip():
            git('cherry-pick', '--skip', cwd=bo, env=env)       # the resolution left nothing new
            log.write('  empty after resolution, skipped\n')
            continue
        r = git('cherry-pick', '--continue', cwd=bo, check=False, env=env)
        if r.returncode:
            git('cherry-pick', '--abort', cwd=bo, check=False)
            return f'cherry-pick --continue of {subj}: {r.stderr.decode(errors="replace").strip()[-300:]}'
    if touched_names:
        rc, tail = run_logged([sys.executable, 'tools/re/names.py'], bo, log.name + '.names')
        if rc:
            return f'names.py failed: {tail}'
        if git('status', '--porcelain', '--', *NAMES_OUT, cwd=bo).strip():
            git('add', '--', *NAMES_OUT, cwd=bo)
            git('commit', '-q', '--amend', '--no-edit', cwd=bo)
            log.write('names-review.md / not-exact.md regenerated into the last commit\n')
    return None


def _pid_alive(pid):
    """importer.py's test (os.kill(pid, 0) would end the process on Windows)."""
    import ctypes
    k = ctypes.windll.kernel32
    h = k.OpenProcess(0x1000, False, pid)
    if not h:
        return ctypes.GetLastError() == 5
    code = ctypes.c_ulong()
    good = k.GetExitCodeProcess(h, ctypes.byref(code))
    k.CloseHandle(h)
    return not good or code.value == 259


def hold_lock(wait=900):
    """importer.py's lock (same file and format), so no verify rewrites imports.json while landing."""
    me = {'agent': 'wt.py bring-over', 'host': socket.gethostname(), 'pid': os.getpid(),
          'cmd': ' '.join(sys.argv), 'start': time.time()}
    limit, said = time.time() + wait, None
    while True:
        try:
            fd = os.open(LOCK, os.O_CREAT | os.O_EXCL | os.O_WRONLY)
            os.write(fd, json.dumps(me).encode())
            os.close(fd)
            return
        except FileExistsError:
            pass
        try:
            h = json.load(open(LOCK))
            who = f"{h.get('agent')} (pid {h.get('pid')})"
        except (OSError, ValueError):
            h, who = None, None
        if h and h.get('host') == me['host'] and not _pid_alive(h.get('pid', 0)):
            print(f'wt: importer lock: taking over from {who}: process gone', flush=True)
            try:
                os.remove(LOCK)
            except OSError:
                pass
            continue
        if who and who != said:
            print(f'wt: importer lock held by {who}, waiting', flush=True)
            said = who
        if time.time() > limit:
            sys.exit('wt: importer lock still held; nothing landed')
        time.sleep(2)


def release_lock():
    try:
        if json.load(open(LOCK)).get('pid') == os.getpid():
            os.remove(LOCK)
    except (OSError, ValueError):
        pass


def merge_work(wt, path, old, new):
    """The new working-copy text of path in worktree wt (blob form), None to delete, or
    ('conflict', why). Unchanged when the working copy already equals NEW."""
    o, n = blob(f'{old}:{path}'), blob(f'{new}:{path}')
    full = os.path.join(wt, path)
    w = None
    if os.path.exists(full):
        sha = git('hash-object', '-w', '--', path, cwd=wt).strip()
        w = blob(sha, wt)
    if w == n:
        return 'same'
    if w == o:
        return n
    if o is None or n is None or w is None:
        return ('conflict', 'added/deleted on one side, changed in the working copy')
    if path == ATTEMPTS:                            # rows: the uncommitted ones plus the landed ones
        return union_rows(o, w, n)
    with tempfile.TemporaryDirectory(dir=WTDIR) as td:
        fs = []
        for tag, t in (('work', w), ('old', o), ('new', n)):
            fp = os.path.join(td, tag)
            with open(fp, 'wb') as f:
                f.write(t.encode('utf-8', 'surrogateescape'))
            fs.append(fp)
        r = subprocess.run(['git', 'merge-file', '-p', '-L', 'work', '-L', 'old', '-L', 'new', *fs],
                           capture_output=True)
        out = r.stdout.decode('utf-8', 'surrogateescape')
    if r.returncode == 0:
        return out
    if r.returncode < 0 or path != IMPORTS:
        return ('conflict', f'{r.returncode} conflicting hunk(s) with the uncommitted copy')
    res, mode, ours, theirs = [], None, [], []     # imports.json: count lines keep the working copy's
    for line in out.splitlines(keepends=True):
        if line.startswith('<<<<<<< '):
            mode, ours, theirs = 'o', [], []
        elif line.startswith('=======') and mode == 'o':
            mode = 't'
        elif line.startswith('>>>>>>> ') and mode == 't':
            if not all(COUNT_LINE.match(x) for x in ours + theirs):
                return ('conflict', 'imports.json conflicts with the uncommitted copy beyond count lines')
            res.extend(ours)
            mode = None
        elif mode == 'o':
            ours.append(line)
        elif mode == 't':
            theirs.append(line)
        else:
            res.append(line)
    return ''.join(res)


def land(target, old, new):
    """Compare-and-swap TARGET from OLD to NEW and bring the worktree that has it checked out along."""
    holder = [p for p, b, _ in worktrees() if b == target]
    if git('symbolic-ref', '-q', 'HEAD', check=False).stdout.decode().strip() == 'refs/heads/' + target:
        holder.insert(0, ROOT)
    hold_lock()
    try:
        plan = {}
        paths = git('diff', '--name-only', '--no-renames', old, new).splitlines()
        for wt in holder:
            for p in paths:
                m = merge_work(wt, p, old, new)
                if isinstance(m, tuple):
                    return f'{os.path.relpath(os.path.join(wt, p), ROOT)}: {m[1]}; nothing landed'
                if m != 'same':
                    plan[(wt, p)] = m
        r = git('update-ref', '-m', 'wt.py bring-over', 'refs/heads/' + target, new, old, check=False)
        if r.returncode:
            now = git('rev-parse', 'refs/heads/' + target).strip()
            return f'{target} moved to {now[:9]} while landing (built on {old[:9]}); nothing landed, rerun'
        for (wt, p), text in plan.items():
            full = os.path.join(wt, p)
            if text is None:
                os.remove(full)
                continue
            sha = git('hash-object', '-w', '--no-filters', '--stdin', cwd=wt,
                      inp=text.encode('utf-8', 'surrogateescape')).strip()
            data = git('cat-file', '--filters', f'--path={p}', sha, cwd=wt, check=False).stdout
            os.makedirs(os.path.dirname(full), exist_ok=True)
            with open(full, 'wb') as f:
                f.write(data)
        for wt in holder:
            for _ in range(5):
                r = git('reset', '-q', new, '--', *paths, cwd=wt, check=False)
                if r.returncode in (0, 1):
                    break
                time.sleep(2)
            print(f'{os.path.relpath(wt, ROOT) if wt != ROOT else "main tree"}: '
                  f'{sum(1 for k in plan if k[0] == wt)} file(s) written, index entries of {len(paths)} reset')
        return None
    finally:
        release_lock()


def cmd_bring_over(a):
    check_name(a.name)
    branch = 'wt/' + a.name
    tip = rev('refs/heads/' + branch)
    tref = 'refs/heads/' + a.target
    old = rev(tref)
    base = open(base_file(a.name)).read().strip() if os.path.exists(base_file(a.name)) else \
        git('merge-base', old, tip).strip()
    if git('rev-list', '--merges', f'{base}..{tip}').strip():
        sys.exit(f'wt: {branch} has merge commits after its base; refused')
    commits = git('rev-list', '--reverse', '--no-merges', '--right-only', '--cherry-pick',
                  f'{old}...{tip}').split()
    if not commits:
        sys.exit(f'wt: nothing to bring over: every commit of {branch} is on {a.target}')
    print(f'{branch}: {len(commits)} commit(s) to bring onto {a.target} {old[:9]} (base {base[:9]})')
    bo = wt_path(a.name) + '.bo'
    logp = bo + '.log'
    if os.path.isdir(bo):
        drop_worktree(bo)
    ff = ok('merge-base', '--is-ancestor', old, tip)
    git('worktree', 'add', '--detach', bo, tip if ff else old)
    furnish(bo, libs=False, pseudo=False)
    why = None
    try:
        with open(logp, 'w', newline='\n', encoding='utf-8') as log:
            if ff:
                log.write(f'fast-forward: {a.target} {old[:9]} is an ancestor of {tip[:9]}\n')
                print(f'fast-forward: {a.target} is at or behind the branch')
            else:
                if old != base:
                    print(f'{a.target} moved past the base ({base[:9]} -> {old[:9]}): cherry-picking')
                why = cherry_pick(bo, commits, log)
        if why:
            sys.exit(f'wt: {why}; {a.target} untouched (log {logp})')
        new = rev('HEAD', cwd=bo)
        if new == old:
            print(f'nothing to bring over: every commit of {branch} was already on {a.target}')
            return
        libs = libs_touched(old, new, a.lib)
        before, cand = pairs_of(blob(f'{old}:{IMPORTS}')), pairs_of(blob(f'{new}:{IMPORTS}'))
        lost = sorted(before - cand)
        if lost:
            sys.exit(f'wt: the branch\'s imports.json record loses {len(lost)} pair(s) against {a.target}: '
                     f'{lost[:10]}; {a.target} untouched')
        gained = sorted(cand - before)
        if libs:
            print(f'verify --lib {" ".join(libs)} --pool {a.pool} in {bo} (log {bo}.verify.log)', flush=True)
            args = [sys.executable, 'tools/vc6/importer.py', 'verify', '--pool', str(a.pool)]
            for lib in libs:
                args += ['--lib', lib]
            rc, tail = run_logged(args, bo, bo + '.verify.log')
            if rc:
                sys.exit(f'wt: verify failed ({rc}): {tail}; {a.target} untouched')
            with open(os.path.join(bo, *IMPORTS.split('/')), encoding='utf-8') as f:
                verified = pairs_of(f.read())
            lost, gained = sorted(before - verified), sorted(verified - before)
            if lost:
                sys.exit(f'wt: verify loses {len(lost)} pair(s) against {a.target}: {lost[:10]}; '
                         f'{a.target} untouched (D66: the user decides)')
            if verified != cand:
                sys.exit(f'wt: the commits record other pairs than the verify gives (verify-only '
                         f'{sorted(verified - cand)[:5]}, record-only {sorted(cand - verified)[:5]}): commit '
                         f'the verified record in the branch and rerun; {a.target} untouched')
            if not a.no_replay:
                print('replay', flush=True)
                rc, tail = run_logged([sys.executable, 'tools/vc6/importer.py', 'replay'], bo, bo + '.replay.log')
                if rc or not any(': 0 problems, 0 files differ' in t for t in tail):
                    sys.exit(f'wt: replay: {tail}; {a.target} untouched')
        else:
            print('no library touched: no verify; pairs from the records')
        print(f'pairs: gained {len(gained)}, lost 0' + (f': {gained[:10]}' if gained else ''))
        if a.dry:
            print(f'--dry: would land {old[:9]} -> {new[:9]} on {a.target}')
            return
        why = land(a.target, old, new)
        if why:
            sys.exit(f'wt: {why}')
        print(f'landed: {a.target} {old[:9]} -> {new[:9]}')
        print(git('log', '--format=  %h %s', f'{old}..{new}').rstrip()[:2000])
        if ff and new == tip:
            with open(base_file(a.name), 'w', newline='\n') as f:
                f.write(new + '\n')
        else:
            print(f'note: {branch} keeps its own copies; a later bring-over skips them (patch-equivalent)')
    finally:
        if not a.keep and os.path.isdir(bo):
            drop_worktree(bo)


def main():
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    sub = ap.add_subparsers(dest='cmd', required=True)
    p = sub.add_parser('new')
    p.add_argument('name')
    p.add_argument('--base', default='HEAD')
    sub.add_parser('list')
    p = sub.add_parser('bring-over')
    p.add_argument('name')
    p.add_argument('--target', default='master', help='the branch to land on (tests: a scratch branch)')
    p.add_argument('--lib', action='append', default=[], help='verify this library as well (repeatable)')
    p.add_argument('--pool', type=int, default=1)
    p.add_argument('--no-replay', action='store_true')
    p.add_argument('--dry', action='store_true', help='stop before landing')
    p.add_argument('--keep', action='store_true', help='keep the scratch worktree NAME.bo')
    p = sub.add_parser('remove')
    p.add_argument('name')
    p.add_argument('--delete-branch', action='store_true')
    p.add_argument('--force', action='store_true', help='remove a worktree with uncommitted changes')
    a = ap.parse_args()
    os.makedirs(WTDIR, exist_ok=True)
    {'new': cmd_new, 'list': cmd_list, 'bring-over': cmd_bring_over, 'remove': cmd_remove}[a.cmd](a)


if __name__ == '__main__':
    main()
