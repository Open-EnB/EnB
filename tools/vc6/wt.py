"""Builder worktrees (D113): make one, list them, bring a branch's verified commits over, remove one.

    python tools/vc6/wt.py new NAME [--base REV]
    python tools/vc6/wt.py list
    python tools/vc6/wt.py bring-over NAME [--target BRANCH] [--lib X ...] [--pool K] [--no-replay] [--dry] [--keep]
    python tools/vc6/wt.py bring-over A B..C D [--batch-name N] [--target BRANCH] [--lib X ...] [--pool K] [--trailer T]
                                      [--no-replay] [--dry] [--picks-only] [--resume] [--keep]
    python tools/vc6/wt.py remove NAME [--delete-branch] [--force]

new: `git worktree add .work/wt/NAME -b wt/NAME BASE` (default HEAD); NTFS junctions .work/locks,
  .work/pool, .work/pseudo to the main tree's (one importer lock, one VM pool, Ghidra's saved
  pseudocode); copies .work/vc6_hosts.json, .work/harness/*.dgram and build/*/*.lib except app's
  (udpreplay needs app.lib: `build.py build --lib app --allow-dirty` in the worktree); records BASE in
  .work/wt/NAME.base. importer.py run inside the worktree takes NAME as its agent (scratch under
  the worktree's .work/NAME/tmp): `new` checks that.

bring-over: puts wt/NAME's commits on TARGET (default master) only when the loss rule below holds, never
  touching TARGET otherwise:
  1. the commits of wt/NAME not yet on TARGET (patch-equivalent ones skipped, merges refused);
  2. TARGET at the recorded base (or an ancestor of the branch): fast-forward to the branch tip;
     else cherry-pick them, in a scratch worktree .work/wt/NAME.bo at TARGET, where conflicts in
     docs/data/attempts.csv are resolved by the union of the rows, names-review.md / not-exact.md
     are regenerated (tools/re/names.py) and any other conflict (imports.json included) refuses;
  3. the libraries the commits touched (their directories, EA headers -> app as well, importer.py
     lines naming a library, imports.json blocks that changed; plus --lib) are verified there:
     `importer.py verify --lib ... --pool K`, then `importer.py replay` (0 problems, 0 files differ);
  4. pairs: the verified imports.json against TARGET's (the loss rule) and against the candidate's own
     record (equal: the commits record what the verify gives); with no library touched, the
     candidate's record against TARGET's;
  5. landing: under the importer lock, the worktree that has TARGET checked out (the main tree for
     master) gets each changed path 3-way merged with its uncommitted copy (git merge-file; attempts.csv
     by row union; an imports.json conflict on count lines only keeps the working copy's counts; any
     other conflict refuses), then `update-ref refs/heads/TARGET NEW OLD` (compare-and-swap), then the merged files
     are written and their index entries reset to the new HEAD.
  --dry stops before landing; --keep keeps the scratch worktree; logs go to .work/wt/NAME.bo.*.log.
  Every importer call waits --lock-wait seconds (3000) for the lock.

The loss rule (D202), single and batch, by pairs_cmp.py against TARGET: any address lost outright refuses and names
  the addresses. Losses that are all D66 moves or shared holders land, are printed, and the merge still needs the
  maintainer's `/accept-d66` (verify_pr.py).

Builder worktrees: a tracked file modified in .work/wt/NAME stops the landing unless it is generated (GENERATED);
  generated leftovers are named and left alone: the landing reads the branch, nothing is checked out or discarded.

Batch form (two or more branches, a range, or --batch-name; D202): one verify over several finished branches.
  1. each branch in the order given, cherry-picked onto one scratch worktree .work/wt/N.bo at TARGET (N: --batch-name,
     default batch-FIRST-LAST). Commits patch-equivalent to what is already there are skipped, as is any that comes
     out empty. A builder based on another's branch brings its own commits only: `A..B` for wt/A..wt/B, or plain B
     when .work/wt/B.base records that base (a base not on TARGET is pointed out).
  2. conflicts: GENERATED take TARGET's side (regenerated below); attempts.csv keeps both sides' rows; importer.py
     keeps both sides where both only add lines, and only if the result compiles. Anything else stops with the file
     and the commit named, TARGET untouched, the scratch worktree left mid-pick: finish the pick by hand and rerun the
     same command with --resume, or `wt.py remove N` to drop it.
  3. `importer.py verify` once (the libraries the batch touches, plus --lib); every library's `in tree:` line must be
     in the log. The loss rule. `report`, names.py, notexact.py; the regenerated files are committed with a generated
     subject and --trailer (default the newest Co-Authored-By on TARGET); `replay`: 0 problems, 0 files differ.
  4. landing: the worktree holding TARGET must have no tracked file modified (.claude/settings.json aside, unless the
     batch changes it): checked at the start and again under the lock. TARGET moves by compare-and-swap, so a TARGET
     that moved during the verify is refused and named as well; rerun.
  --picks-only stops after step 1 (no verify); --dry stops before step 4; the scratch worktree goes unless --keep or
  a conflict stopped the batch. Logs: .work/wt/N.bo.*.log.

remove: removes every junction inside the worktree first (rmdir, never a recursive delete through
  one), then `git worktree remove --force`, prune, the .base record; --delete-branch deletes wt/NAME.
  A dirty worktree is refused unless --force.

list: every worktree but the main one: name, recorded base, commits ahead of it, dirty or clean.

Promoted from .work/eval/setup.py and .work/p9/wt.py (setup), the coordinator's hand bring-over of
F6's branch (4f2a8b1) and commit_mine.py's compare-and-swap; K5, 2026-10-27. The batch form: the coordinator's
land_batch.sh, land_final.sh and land_fix.sh of 2026-11-06 (D202).
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
import warnings

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import commit_rows as cr  # noqa: E402
import pairs_cmp  # noqa: E402


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
IMPORTER = 'tools/vc6/importer.py'
GENERATED = (IMPORTS, *NAMES_OUT, 'docs/imports.md')    # rewritten by verify, report, names.py, notexact.py
CORRECTIONS = 'docs/data/accepted-corrections.json'
TRACKED_ELSEWHERE = '.claude/settings.json'             # tracked, the maintainer's (never blocks a batch by itself)
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
    if os.path.exists(bo + '.state'):
        os.remove(bo + '.state')
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


def target_holders(target):
    """The worktrees that have TARGET checked out, the main tree included."""
    holder = [p for p, b, _ in worktrees() if b == target]
    if git('symbolic-ref', '-q', 'HEAD', check=False).stdout.decode().strip() == 'refs/heads/' + target:
        holder.insert(0, ROOT)
    return holder


def dirty_tracked(path):
    """The tracked paths with uncommitted changes (staged or not) in the worktree at path."""
    out = git('status', '--porcelain', '-uno', '--no-renames', cwd=path)
    return [l[3:].strip().strip('"') for l in out.splitlines() if l.strip()]


def dirty_target(target, changing=()):
    """'worktree: files' for each holder of TARGET with a modified tracked file, or None."""
    bad = []
    for wt in target_holders(target):
        d = [p for p in dirty_tracked(wt) if p != TRACKED_ELSEWHERE or p in changing]
        if d:
            bad.append(f'{os.path.relpath(wt, ROOT) if wt != ROOT else "main tree"}: {", ".join(d[:10])}')
    return '; '.join(bad) or None


def land(target, old, new, wait=900, refuse_dirty=False):
    """Compare-and-swap TARGET from OLD to NEW and bring the worktree that has it checked out along."""
    holder = target_holders(target)
    hold_lock(wait)
    try:
        plan = {}
        paths = git('diff', '--name-only', '--no-renames', old, new).splitlines()
        if refuse_dirty:
            d = dirty_target(target, paths)
            if d:
                return f'{target} is dirty, not fast-forwarded ({d}); nothing landed'
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


def corrections_at(rev_):
    """The accepted corrections of that commit (a branch can't accept its own losses)."""
    t = blob(f'{rev_}:{CORRECTIONS}')
    return json.loads(t)['corrections'] if t else []


def judge_losses(before, after, target, old, what):
    """D202 on two imports.json texts: exits naming the addresses lost outright; else pairs_cmp's dict."""
    r = pairs_cmp.compare(json.loads(before or '{}'), json.loads(after or '{}'), corrections_at(old))
    if r['lost_outright']:
        where = [x for x in r['lost'] if x[2] in set(r['lost_outright'])]
        sys.exit(f'wt: {what} loses {len(r["lost_outright"])} address(es) outright against {target}: '
                 f'{r["lost_outright"]} (in {where[:20]}); {target} untouched')
    return r


def say_losses(r):
    """The landing's pairs, each kind of non-loss spelled out (D202)."""
    gained = r['gained'] + r['gained_tiebreak']
    print(f'pairs: gained {len(gained)}, lost outright 0, D66 moves {len(r["moves"])}, shared holders '
          f'{len(r["shared_holders"])}' + (f': {gained[:10]}' if gained else ''))
    for lib, f, to, addr in r['moves']:
        print(f'  D66 move {addr}: {lib}/{f} -> {", ".join(l + "/" + n for l, n in to)}')
    for lib, f, to, addr in r['shared_holders']:
        print(f'  shared holder less {addr}: {lib}/{f}; still held by {", ".join(l + "/" + n for l, n in to)}')
    for lib, f, addr in r['with_caller']:
        print(f'  tie-break gone with its caller (D66 vii) {addr}: {lib}/{f}')
    for e, (lib, f, addr) in r['corrected']:
        print(f'  corrected ({e["decision"]}) {addr}: {lib}/{f}')
    if r['lost']:
        print(f'NOTE: {len(r["lost"])} pair(s) moved or lost a holder: the merge still needs the maintainer\'s '
              f'/accept-d66 (D202)')


def check_builders(names):
    """Exits on a modified non-generated tracked file in a builder's worktree; generated leftovers are only named
    (the landing reads the branch: nothing is checked out, so nothing can be discarded)."""
    for name in names:
        path = wt_path(name)
        if not os.path.isdir(path):
            continue
        if git('symbolic-ref', '-q', 'HEAD', cwd=path, check=False).stdout.decode().strip() != 'refs/heads/wt/' + name:
            continue
        dirty = dirty_tracked(path)
        other = [p for p in dirty if p not in GENERATED]
        if other:
            sys.exit(f'wt: {path} has uncommitted changes outside the generated files: {other[:10]}: commit or drop '
                     f'them first; nothing done')
        if dirty:
            print(f'{name}: uncommitted generated leftovers left alone: {", ".join(dirty)}')


def verify_finished(log, libs):
    """The libraries of LIBS whose `in tree:` line is missing from a verify's log."""
    text = open(log, encoding='utf-8', errors='replace').read()
    return [lib for lib in libs if not re.search(r'(?m)^%s\s+in tree: ' % re.escape(lib), text)]


def run_verify(a, bo, libs, target):
    """importer.py verify in bo, with the long lock wait; exits unless it ran to its last line."""
    print(f'verify --lib {" ".join(libs)} --pool {a.pool} in {bo} (log {bo}.verify.log)', flush=True)
    args = [sys.executable, IMPORTER, 'verify', '--pool', str(a.pool), '--lock-wait', str(a.lock_wait)]
    for lib in libs:
        args += ['--lib', lib]
    rc, tail = run_logged(args, bo, bo + '.verify.log')
    if rc:
        sys.exit(f'wt: verify failed ({rc}): {tail}; {target} untouched')
    short = verify_finished(bo + '.verify.log', libs)
    if short:
        sys.exit(f'wt: verify did not finish {short}: {tail}; {target} untouched')


def run_replay(a, bo, target):
    print('replay', flush=True)
    rc, tail = run_logged([sys.executable, IMPORTER, 'replay', '--lock-wait', str(a.lock_wait)], bo, bo + '.replay.log')
    if rc or not any(': 0 problems, 0 files differ' in t for t in tail):
        sys.exit(f'wt: replay: {tail}; {target} untouched')


def add_only_union(base, ours, theirs):
    """(text, None) keeping both sides of a 3-way merge in which both sides only add lines, or (None, why). What git
    merges by itself stays merged; a line both sides added once; the result has to compile."""
    if base is None or ours is None or theirs is None:
        return None, 'added or deleted on one side'
    with tempfile.TemporaryDirectory(dir=WTDIR) as td:
        fs = []
        for tag, t in (('ours', ours), ('base', base), ('theirs', theirs)):
            fp = os.path.join(td, tag)
            with open(fp, 'wb') as f:
                f.write(t.encode('utf-8', 'surrogateescape'))
            fs.append(fp)
        r = subprocess.run(['git', 'merge-file', '-p', '--diff3', '-L', 'ours', '-L', 'base', '-L', 'theirs', *fs],
                           capture_output=True)
    if r.returncode < 0:
        return None, 'git merge-file failed'
    res, mode, o, b, t = [], None, [], [], []
    for line in r.stdout.decode('utf-8', 'surrogateescape').split('\n'):
        if line == '<<<<<<< ours':
            mode, o, b, t = 'o', [], [], []
        elif line == '||||||| base' and mode == 'o':
            mode = 'b'
        elif line == '=======' and mode == 'b':
            mode = 't'
        elif line == '>>>>>>> theirs' and mode == 't':
            if any(x.strip() for x in b):
                return None, f'both sides change existing lines ({b[0].strip()[:60]!r})'
            res.extend(o + [x for x in t if x not in o])
            mode = None
        elif mode == 'o':
            o.append(line)
        elif mode == 'b':
            b.append(line)
        elif mode == 't':
            t.append(line)
        else:
            res.append(line)
    text = '\n'.join(res)
    try:
        with warnings.catch_warnings():
            warnings.simplefilter('ignore')
            compile(text, IMPORTER, 'exec')
    except (SyntaxError, ValueError) as e:
        return None, f'the union does not compile ({e})'
    return text, None


def stage_text(bo, path, text):
    sha = git('hash-object', '-w', '--no-filters', '--stdin', cwd=bo, inp=text.encode('utf-8', 'surrogateescape')).strip()
    mode = git('ls-files', '-s', '--', path, cwd=bo).split()[0]
    git('update-index', '--cacheinfo', f'{mode},{sha},{path}', cwd=bo)
    git('checkout-index', '-f', '--', path, cwd=bo)


def resolve_batch(bo, conflicted, log):
    """Resolve what D202 lets a batch resolve; returns (file, why) of the first conflict it can't, or None."""
    for p in conflicted:
        if p not in GENERATED and p != ATTEMPTS and p != IMPORTER:
            return p, 'not a generated or row file'
    union = None
    if IMPORTER in conflicted:
        union, why = add_only_union(blob(':1:' + IMPORTER, bo), blob(':2:' + IMPORTER, bo), blob(':3:' + IMPORTER, bo))
        if why:
            return IMPORTER, why
    for p in conflicted:
        if p == ATTEMPTS:
            stage_text(bo, p, union_rows(blob(':1:' + p, bo), blob(':2:' + p, bo) or '', blob(':3:' + p, bo) or ''))
            log.write('  attempts.csv: both sides\' rows\n')
        elif p == IMPORTER:
            stage_text(bo, p, union)
            log.write('  importer.py: both sides\' added lines, compiles\n')
        else:
            git('checkout', '--ours', '--', p, cwd=bo)
            git('add', '--', p, cwd=bo)
            log.write(f'  {p}: the target\'s side (regenerated later)\n')
    return None


def batch_spec(spec):
    """(label, worktree name, tip, low) of `NAME`, `wt/NAME` or `A..B`. low: only the commits above it are the
    branch's own (A, or the recorded base of NAME when it has one); None without a record."""
    def br(x):
        x = x[3:] if x.startswith('wt/') else x
        if not ok('rev-parse', '--verify', '-q', 'refs/heads/wt/' + x):
            sys.exit(f'wt: no branch wt/{x}')
        return x
    if '..' in spec:
        lo, _, hi = spec.partition('..')
        lo, hi = br(lo.rstrip('.')), br(hi.lstrip('.'))
        return f'{lo}..{hi}', hi, rev('refs/heads/wt/' + hi), rev('refs/heads/wt/' + lo)
    name = br(spec)
    low = None
    if os.path.exists(base_file(name)):
        low = open(base_file(name)).read().strip()
    return name, name, rev('refs/heads/wt/' + name), low


def batch_commits(bo, tip, low):
    """The branch's commits still to pick onto bo's HEAD, oldest first (merges refused)."""
    head = rev('HEAD', cwd=bo)
    keep = None
    if low:
        keep = set(git('rev-list', f'{low}..{tip}').split())
    if git('rev-list', '--merges', f'{head}..{tip}' if not low else f'{low}..{tip}').strip():
        sys.exit(f'wt: {tip[:9]} has merge commits; refused')
    cand = git('rev-list', '--reverse', '--no-merges', '--right-only', '--cherry-pick', f'{head}...{tip}').split()
    return [c for c in cand if keep is None or c in keep]


def pick_batch(bo, specs, state, log):
    """Cherry-pick every branch of the batch in bo; a conflict it can't resolve leaves the pick in progress, saves
    state and returns the refusal."""
    env = dict(os.environ, GIT_EDITOR='true')
    done = set(state['done'])
    for label, _, tip, low in specs:
        todo = [c for c in batch_commits(bo, tip, low) if c not in done]
        print(f'{label}: {len(todo)} commit(s)', flush=True)
        for c in todo:
            subj = git('log', '-1', '--format=%h %s', c).strip()[:90]
            r = git('cherry-pick', c, cwd=bo, check=False, env=env)
            log.write(f'cherry-pick {label} {subj}: {r.returncode}\n')
            if r.returncode:
                conflicted = git('diff', '--name-only', '--diff-filter=U', cwd=bo).splitlines()
                stop = resolve_batch(bo, conflicted, log) if conflicted else None
                if stop:
                    state['done'] = sorted(done | {c})      # --resume: the person finishes this one
                    with open(bo + '.state', 'w', newline='\n') as f:
                        json.dump(state, f)
                    return f'{label}: commit {subj}: conflict in {stop[0]}: {stop[1]}'
                if not git('diff', '--cached', '--name-only', 'HEAD', cwd=bo).strip():
                    git('cherry-pick', '--skip', cwd=bo, env=env)   # nothing new: already there
                    log.write('  empty, skipped\n')
                else:
                    r = git('cherry-pick', '--continue', cwd=bo, check=False, env=env)
                    if r.returncode:
                        return f'{label}: cherry-pick --continue of {subj}: ' \
                               f'{r.stderr.decode(errors="replace").strip()[-300:]}'
            done.add(c)
    state['done'] = sorted(done)
    return None


def regen_and_commit(a, bo, labels, res, target, old):
    """report, names.py, notexact.py, and the regenerated files committed; exits on anything else changed."""
    steps = [('report', [IMPORTER, 'report', '--lock-wait', str(a.lock_wait)]),
             ('names', ['tools/re/names.py']), ('notexact', ['tools/re/notexact.py'])]
    for tag, args in steps:
        rc, tail = run_logged([sys.executable, *args], bo, f'{bo}.{tag}.log')
        if rc or any('giving up' in t for t in tail):
            sys.exit(f'wt: {tag}: {tail}; {target} untouched')
    other = [p for p in dirty_tracked(bo) if p not in GENERATED]
    if other:
        sys.exit(f'wt: regenerating changed other files as well: {other[:10]}; {target} untouched')
    git('add', '--', *GENERATED, cwd=bo)
    if not git('diff', '--cached', '--name-only', cwd=bo).strip():
        print('the regenerated files equal the commits\' own')
        return
    trailer = a.trailer
    if not trailer:
        for line in git('log', '-50', '--format=%(trailers:key=Co-Authored-By)', old).splitlines():
            if line.strip():
                trailer = line.strip()
                break
    msg = (f'Record the verified pairs of {", ".join(labels)} after landing them together: one verify over all of '
           f'them, no source change, {len(res["gained"]) + len(res["gained_tiebreak"])} gained, {len(res["moves"])} '
           f'D66 moves, 0 lost outright\n')
    if trailer:
        msg += f'\n{trailer}\n'
    git('commit', '-q', '-F', '-', cwd=bo, inp=msg.encode())


def cmd_batch(a):
    specs = [batch_spec(s) for s in a.names]
    labels = [s[0] for s in specs]
    name = a.batch_name or 'batch-' + specs[0][1] + ('-' + specs[-1][1] if len(specs) > 1 else '')
    check_name(name)
    check_builders([s[1] for s in specs])
    tref = 'refs/heads/' + a.target
    bo = wt_path(name) + '.bo'
    for label, n, tip, low in specs:
        if low and '..' not in label and ok('merge-base', '--is-ancestor', low, tip):
            below = git('rev-list', '--count', f'{rev(tref)}..{low}').strip()
            if below != '0':
                print(f'note: {n} is based on {low[:9]}, {below} commit(s) not on {a.target}: only its own commits '
                      f'are picked; the rest must come from another branch of this batch', flush=True)
    if a.resume:
        if not os.path.isdir(bo) or not os.path.exists(bo + '.state'):
            sys.exit(f'wt: nothing to resume at {bo}')
        state = json.load(open(bo + '.state'))
        if state['target'] != a.target or state['labels'] != labels:
            sys.exit(f'wt: {bo} holds {state["labels"]} for {state["target"]}; refused')
        if ok('rev-parse', '--verify', '-q', 'CHERRY_PICK_HEAD', cwd=bo):
            sys.exit(f'wt: {bo}: a cherry-pick is still in progress: finish it (git cherry-pick --continue) first')
        if dirty_tracked(bo):
            sys.exit(f'wt: {bo} has uncommitted changes: {dirty_tracked(bo)[:10]}')
        old = state['old']
        if rev(tref) != old:
            sys.exit(f'wt: {a.target} moved ({old[:9]} -> {rev(tref)[:9]}) since the batch began: remove {name} and rerun')
    else:
        if os.path.isdir(bo):
            sys.exit(f'wt: {bo} exists (a stopped batch?): --resume after finishing its pick, or `wt.py remove {name}`')
        old = rev(tref)
        state = {'target': a.target, 'labels': labels, 'old': old, 'done': []}
    d = dirty_target(a.target)
    if d:
        sys.exit(f'wt: {a.target} is dirty ({d}); commit or drop that first; nothing done')
    print(f'batch {name}: {", ".join(labels)} onto {a.target} {old[:9]} in {bo}', flush=True)
    if not a.resume:
        git('worktree', 'add', '--detach', bo, old)
        furnish(bo, libs=False, pseudo=False)
    keep = a.keep
    try:
        logp = bo + '.log'
        with open(logp, 'a' if a.resume else 'w', newline='\n', encoding='utf-8') as log:
            why = pick_batch(bo, specs, state, log)
        if why:
            keep = True
            sys.exit(f'wt: {why}\nwt: batch stopped, {a.target} untouched; the scratch worktree stays at {bo}, mid-pick: '
                     f'finish the pick there (git cherry-pick --continue) and rerun with --resume, '
                     f'or `wt.py remove {name}` (log {logp})')
        if rev('HEAD', cwd=bo) == old:
            print(f'nothing to bring over: every commit of {", ".join(labels)} is on {a.target}')
            return
        if a.picks_only:
            print(f'--picks-only: {git("rev-list", "--count", f"{old}..HEAD", cwd=bo).strip()} commit(s) picked cleanly')
            return
        libs = libs_touched(old, rev('HEAD', cwd=bo), a.lib)
        if libs:
            run_verify(a, bo, libs, a.target)
        else:
            print('no library touched: no verify; pairs from the records')
        with open(os.path.join(bo, *IMPORTS.split('/')), encoding='utf-8') as f:
            after = f.read()
        res = judge_losses(blob(f'{old}:{IMPORTS}'), after, a.target, old, 'the batch')
        regen_and_commit(a, bo, labels, res, a.target, old)
        if not a.no_replay:
            run_replay(a, bo, a.target)
            other = dirty_tracked(bo)
            if other:
                sys.exit(f'wt: replay left changes: {other[:10]}; {a.target} untouched')
        new = rev('HEAD', cwd=bo)
        say_losses(res)
        if a.dry:
            print(f'--dry: would land {old[:9]} -> {new[:9]} on {a.target}')
            return
        now = rev(tref)
        if now != old:
            sys.exit(f'wt: {a.target} moved ({old[:9]} -> {now[:9]}) during the batch, not fast-forwarded; rerun')
        why = land(a.target, old, new, a.lock_wait, refuse_dirty=True)
        if why:
            sys.exit(f'wt: {why}')
        print(f'landed: {a.target} {old[:9]} -> {new[:9]}')
        print(git('log', '--format=  %h %s', f'{old}..{new}').rstrip()[:2000])
    finally:
        if not keep and os.path.isdir(bo):
            drop_worktree(bo)
            if os.path.exists(bo + '.state'):
                os.remove(bo + '.state')


def cmd_bring_over(a):
    if len(a.names) > 1 or a.batch_name or a.resume or a.picks_only or '..' in a.names[0]:
        return cmd_batch(a)
    a.name = a.names[0]
    check_name(a.name)
    check_builders([a.name])
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
        before_text, cand_text = blob(f'{old}:{IMPORTS}'), blob(f'{new}:{IMPORTS}')
        cand = pairs_of(cand_text)
        res = judge_losses(before_text, cand_text, a.target, old, 'the branch\'s imports.json record')
        if libs:
            run_verify(a, bo, libs, a.target)
            with open(os.path.join(bo, *IMPORTS.split('/')), encoding='utf-8') as f:
                verified_text = f.read()
            verified = pairs_of(verified_text)
            res = judge_losses(before_text, verified_text, a.target, old, 'the verify')
            if verified != cand:
                sys.exit(f'wt: the commits record other pairs than the verify gives (verify-only '
                         f'{sorted(verified - cand)[:5]}, record-only {sorted(cand - verified)[:5]}): commit '
                         f'the verified record in the branch and rerun; {a.target} untouched')
            if not a.no_replay:
                run_replay(a, bo, a.target)
        else:
            print('no library touched: no verify; pairs from the records')
        say_losses(res)
        if a.dry:
            print(f'--dry: would land {old[:9]} -> {new[:9]} on {a.target}')
            return
        why = land(a.target, old, new, a.lock_wait)
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
    p.add_argument('names', nargs='+', metavar='NAME',
                   help='a worktree name; two or more, or A..B (A\'s branch below B\'s own commits): the batch form')
    p.add_argument('--target', default='master', help='the branch to land on (tests: a scratch branch)')
    p.add_argument('--batch-name', help='batch form (also for one branch): the scratch worktree .work/wt/N.bo')
    p.add_argument('--trailer', help='batch: the record commit\'s trailer line (default the newest on the target)')
    p.add_argument('--picks-only', action='store_true', help='batch: stop after the cherry-picks, no verify')
    p.add_argument('--resume', action='store_true', help='batch: continue in the scratch worktree after a hand-finished pick')
    p.add_argument('--lock-wait', type=int, default=3000, help='seconds each importer call waits for the lock')
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
