"""Builder worktrees (D113): make one, list them, bring a branch's verified commits over, remove one.

    python tools/vc6/wt.py new NAME [--base REV]
    python tools/vc6/wt.py list [--unlanded]
    python tools/vc6/wt.py bring-over NAME [--target BRANCH] [--lib X ...] [--pool K] [--dry] [--keep] [--no-link]
    python tools/vc6/wt.py bring-over A B..C D [--batch-name N] [--target BRANCH] [--lib X ...] [--pool K] [--trailer T]
                                      [--dry] [--picks-only] [--resume] [--keep] [--no-link]
    python tools/vc6/wt.py rebase NAME [--onto BRANCH] [--continue] [--lib X ...] [--pool K] [--trailer T]
    python tools/vc6/wt.py remove NAME [--delete-branch] [--force]

new: `git worktree add .work/wt/NAME -b wt/NAME BASE` (default HEAD); NTFS junctions .work/locks,
  .work/pool, .work/pseudo to the main tree's (one importer lock, one VM pool, Ghidra's saved
  pseudocode); copies .work/vc6_hosts.json, .work/harness/*.dgram and build/*/*.lib except app's
  (udpreplay needs app.lib: `build.py build --lib app --allow-dirty` in the worktree); records BASE in
  .work/wt/NAME.base. verify.py run inside the worktree takes NAME as its agent (scratch under
  the worktree's .work/NAME/tmp): `new` checks that.
  Stacked branches (pipeline-plan §4 item 4): `--base wt/PARENT` records PARENT's tip as the base plus a line
  `parent PARENT` (one-line records of older worktrees read as no parent). Until PARENT's commits are on TARGET
  (own_commits finds none up to the recorded tip), bring-over refuses NAME unless PARENT comes before it in the same
  batch; `list` shows "on wt/PARENT (unlanded)" or "(parent landed: rebase)"; `rebase NAME` then replays NAME's own
  commits only (`git rebase --onto ONTO <recorded parent tip>`) and drops the parent line (`--onto wt/PARENT` keeps it,
  recording PARENT's new tip).

bring-over: puts wt/NAME's commits on TARGET (default master) only when the loss rule below holds, never
  touching TARGET otherwise:
  1. the commits of wt/NAME not yet on TARGET (patch-equivalent ones skipped, merges refused), only those above the
     recorded base (the branch's own, as the batch form picks them; a stacked branch's parent must be on TARGET);
  2. TARGET at the recorded base (or an ancestor of the branch, the base on TARGET): fast-forward to the branch tip;
     else cherry-pick them, in a scratch worktree .work/wt/NAME.bo at TARGET, conflicts resolved as in
     the batch form (step 2 there: generated files take TARGET's side, attempts.csv both sides' rows);
     any other conflict refuses (`wt.py rebase NAME --onto TARGET` then stops on it for the builder);
  3. the libraries the commits touched (their directories, EA headers -> app as well, libdefs.py or
     importer.py lines naming a library, ledger shards that changed; plus --lib) are verified there:
     `verify.py --lib ... --pool K` (no replay since the freeze, D211: the libraries are plain source);
  4. pairs: the verified ledger against TARGET's (the loss rule) and against the candidate's own
     record (equal: the commits record what the verify gives); with no library touched, the
     candidate's record against TARGET's. When step 2 took a generated file's TARGET side: no record
     check; report, names.py, notexact.py (app_inventory.py for its files) and a commit of the
     regenerated files instead (the batch form's regen_and_commit);
  4a. stand-ins (pipeline-plan §4 item 6a, always, seconds): standins.py check over TARGET..the candidate (the
     verified ledger's new app pairs too) refuses a function defined anew in app/ that a hand stand-in still defines
     (programs/clientpoc/poc_standins.cpp, poc_world*.cpp, poc_gadget_*.cpp, stubs/app_stubs.cpp,
     tests/clienttest/client_stubs.cpp), naming symbol and file:line; the batch form runs it too, after its verify;
  4b. link (item 6b, when the commits touch app/, stubs/, programs/clientpoc/ or tests/clienttest/; else skipped,
     printed): linkgate.py in the scratch worktree on --pool K VMs: build.py build --lib app (plus the other touched
     libraries; the rest copied from the main tree's build/), the POC linked (--no-deploy) and clienttest built;
     refused on LNK2005 / LNK1169, a compile error, no exe, or an unresolved symbol not in
     tools/vc6/link_baseline.json (the POC's known set gen_stubs cannot write). Once per batch in the batch form.
     --no-link skips it with a warning (doc-only or tools-only landings). Minutes: the app build. The harness run
     (D182) stays separate;
  5. landing: under the importer lock, the worktree that has TARGET checked out (the main tree for
     master) gets each changed path 3-way merged with its uncommitted copy (git merge-file; attempts.csv
     by row union; any other conflict, a ledger shard's included, refuses), then `update-ref refs/heads/TARGET NEW OLD` (compare-and-swap), then the merged files
     are written and their index entries reset to the new HEAD. TARGET moved past OLD meanwhile (pipeline-plan §4
     item 3; a coordinator's commit): when those commits touch none of the paths the landing's commits touch and no
     generated file, the landing's commits are replayed onto TARGET's tip (plumbing on a temporary index; no verify:
     nothing it verified changed) and the compare-and-swap retried; otherwise it refuses naming the overlapping paths.
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
  2. conflicts: GENERATED take TARGET's side (regenerated below; a ledger shard keeps the branch's `description`
     when only the branch changed it); attempts.csv keeps both sides' rows; libdefs.py and importer.py
     keep both sides where both only add lines, and only if the result compiles. Anything else stops with the file
     and the commit named, TARGET untouched, the scratch worktree left mid-pick: finish the pick by hand and rerun the
     same command with --resume, or `wt.py remove N` to drop it.
  3. `verify.py` once (the libraries the batch touches, plus --lib); every library's `in tree:` line must be
     in the log. The loss rule. report.py, names.py, notexact.py; the regenerated files are committed with a generated
     subject and --trailer (default the newest Co-Authored-By on TARGET). The stand-in check and the link check
     (single form's 4a, 4b), once for the whole batch.
  4. landing: the worktree holding TARGET must have no tracked file modified (.claude/settings.json aside, unless the
     batch changes it): checked at the start and again under the lock. TARGET moves by compare-and-swap; a TARGET
     that moved during the verify is handled as in the single form's step 5 (unrelated commits: replayed onto;
     shared paths or generated files: refused, named; rerun).
  --picks-only stops after step 1 (no verify); --dry stops before step 4; the scratch worktree goes unless --keep or
  a conflict stopped the batch. Logs: .work/wt/N.bo.*.log.

Generated files (GENERATED, ledger shards; pipeline-plan §4 item 2): never merged. On a conflict wt.py takes the
  target's side and regenerates: verify (the touched libraries' shards), report.py (imports.md), names.py
  (names-review.md, not-exact.md), notexact.py, app_inventory.py (app-inventory.md, app-functions.json, only when one
  of them conflicted); coverage.md takes the target's side until symbols.py's next run (it needs every library's
  verify objects). No git merge driver: it would need per-clone config and does not run on GitHub.

rebase: a builder's own worktree .work/wt/NAME (wt/NAME checked out, nothing modified but generated files, which
  are dropped): `git rebase` onto --onto (default the run branch wt/NAME was based on: the run/* branch holding the
  recorded base, the main tree's current one first; else the main tree's run branch). Each conflict: generated files
  take the onto side, attempts.csv both sides' rows, libdefs.py/importer.py both sides' added lines (the batch's
  resolve_batch); anything else stops the rebase there, file named, with the commands to finish it (resolve, `git
  add`, `wt.py rebase NAME --continue`) or abort it (`git rebase --abort`); state in .work/wt/NAME.rebase. Then:
  verify of the libraries the branch's own commits touch (--pool K), report, names.py, notexact.py, and the
  regenerated files committed ("Record the verified pairs of wt/NAME after rebasing it onto ..."); the base record
  becomes the onto commit. A loss outright against the onto branch is printed, not refused (bring-over refuses it).
  No hand rebases: they skip the regeneration (WE8). tools/agy/rebase_gen.sh is this plus a bring-over.

remove: removes every junction inside the worktree first (rmdir, never a recursive delete through
  one), then `git worktree remove --force`, prune, the .base record; --delete-branch deletes wt/NAME.
  A dirty worktree is refused unless --force, and so is a branch with unlanded commits (own_commits below; the
  refusal lists them, sha and subject; with --force they are printed with how to recover the branch).

list: every worktree but the main one: name, recorded base, commits ahead of it, dirty or clean (plus the parent of a
  stacked branch: "on wt/P (unlanded)", "(parent landed: rebase)" when it landed under new hashes, or "(parent
  landed)" when its recorded tip itself is on a run branch or master: nothing to rebase for).
  --unlanded: the wt/* branches (with a worktree or not) that have commits landed on none of master,
  origin/master and the local run/* branches; landed = reachable, or a commit there with the same author time,
  author and subject (a cherry-pick or rebase, whatever its conflict resolution did), or patch-equivalent
  (`git cherry`), or touching generated files only. Every such branch counts as finished but unlanded (a builder
  still at work shows too). First line: `unlanded    N wt/* branch(es) ...: A (2), B (1)`, which pacing.py's
  heartbeat prints; then each branch's commits. run_branch.py cleanup uses the same test (own_commits).

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
import ledger  # noqa: E402
import pairs_cmp  # noqa: E402
import linkgate  # noqa: E402
import standins  # noqa: E402


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
IMPORTS = ledger.REL                                    # docs/data/imports: one shard per source file
OLD_IMPORTS = ledger.OLD_REL                            # the single file before the split
ATTEMPTS = 'docs/data/attempts.csv'
NAMES_OUT = ('docs/data/names-review.md', 'docs/data/not-exact.md')
IMPORTER = 'tools/vc6/importer.py'
LIBDEFS = 'tools/vc6/libdefs.py'
DEFS = (LIBDEFS, IMPORTER)                              # Python files that name libraries (add-only union)
# written from client.exe by app_inventory.py (cheap: rerun when it conflicted) and symbols.py (coverage.md: with
# symbols.json, from every library's verify objects; a conflict takes the target's side until the next regeneration)
APP_INV = ('docs/app-inventory.md', 'docs/data/app-functions.json')
COVERAGE = 'docs/coverage.md'
GENERATED = (OLD_IMPORTS, *NAMES_OUT, 'docs/imports.md', *APP_INV, COVERAGE)
# the rule (pipeline-plan §4 item 2): a conflict in any of them takes either side (wt.py: the target's), then the
# file is regenerated: verify (shards of the touched libraries), report.py, names.py, notexact.py, app_inventory.py


def generated(p):
    """A file a generator rewrites: GENERATED or a ledger shard."""
    return p in GENERATED or p.startswith(IMPORTS + '/')


def tool(bo, new, old):
    """tools/vc6/NEW in the worktree bo, or OLD (importer.py's form) when bo predates the split."""
    return (['tools/vc6/' + new] if os.path.exists(os.path.join(bo, 'tools', 'vc6', new))
            else [IMPORTER, old])
CORRECTIONS = 'docs/data/accepted-corrections.json'
TRACKED_ELSEWHERE = '.claude/settings.json'             # tracked, the maintainer's (never blocks a batch by itself)
LOCK = os.path.join(ROOT, '.work', 'locks', 'importer.lock')
# top-level directories of the libraries (importer.py LIB_TREE / APP_LIBS)
LIB_TOP = {'app': 'app', 'binkmovie': 'binkmovie', 'ww3d2': 'ww3d2', 'wwaudio': 'wwaudio', 'wwdebug': 'wwdebug',
           'wwlib': 'wwlib', 'wwmath': 'wwmath', 'wwsaveload': 'wwsaveload', 'wwshade': 'wwshade',
           'libs/wwonline/wlib': 'wlib', 'libs/wwonline/wnet': 'wnet'}
EA_LIBS = {'binkmovie', 'ww3d2', 'wwaudio', 'wwdebug', 'wwlib', 'wwmath', 'wwsaveload', 'wwshade', 'wlib', 'wnet'}


def git(*args, cwd=None, check=True, inp=None, env=None):
    """git's stdout as text (bytes when inp is bytes). cwd: default the main tree."""
    cwd = ROOT if cwd is None else cwd
    r = subprocess.run(['git', *args], cwd=cwd, input=inp, capture_output=True, env=env)
    if check and r.returncode:
        sys.exit(f'wt: git {" ".join(args)} (in {cwd}): {r.stderr.decode(errors="replace").strip()}')
    return r if not check else r.stdout.decode('utf-8', 'surrogateescape')


def ok(*args, cwd=None):
    return subprocess.run(['git', *args], cwd=ROOT if cwd is None else cwd, capture_output=True).returncode == 0


def rev(r, cwd=None):
    return git('rev-parse', '--verify', '-q', r + '^{commit}', cwd=cwd).strip()


def wt_path(name):
    return os.path.join(WTDIR, name)


def base_file(name):
    return os.path.join(WTDIR, name + '.base')


def read_base(name):
    """(base, parent) from .work/wt/NAME.base: line 1 the base commit (None without a record); a later line
    `parent P` when NAME was made with --base wt/P (the base is then P's tip at that time). Older one-line records
    read as no parent."""
    if not os.path.exists(base_file(name)):
        return None, None
    with open(base_file(name), encoding='utf-8') as f:
        lines = [l.strip() for l in f.read().splitlines() if l.strip()]
    parent = next((l.split(None, 1)[1] for l in lines[1:] if l.startswith('parent ') and len(l.split()) > 1), None)
    return (lines[0] if lines else None), parent


def write_base(name, base, parent=None):
    with open(base_file(name), 'w', newline='\n') as f:
        f.write(base + '\n' + (f'parent {parent}\n' if parent else ''))


def stacked_on(name, upstreams):
    """(parent, parent tip, landed) when NAME was made on wt/PARENT, else None. landed: the parent's commits up to the
    recorded tip are all on UPSTREAMS (own_commits finds none)."""
    base, parent = read_base(name)
    if not parent or not base:
        return None
    return parent, base, not own_commits(base, upstreams)


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
    m = re.fullmatch(r'(?:refs/heads/)?wt/([A-Za-z0-9][A-Za-z0-9._-]*)', a.base)
    parent = m.group(1) if m and ok('rev-parse', '--verify', '-q', 'refs/heads/wt/' + m.group(1)) else None
    os.makedirs(WTDIR, exist_ok=True)
    git('worktree', 'add', path, '-b', 'wt/' + a.name, base)
    furnish(path)
    write_base(a.name, base, parent)
    agent = importer_agent(path)
    print(f'worktree {path}\nbranch   wt/{a.name} at {base[:9]} ({git("log", "-1", "--format=%s", base).strip()[:70]})')
    if parent:
        print(f'stacked on wt/{parent} (its tip {base[:9]} recorded): once wt/{parent} lands, `wt.py rebase {a.name}` '
              f'replays only wt/{a.name}\'s own commits; bring-over refuses wt/{a.name} before it (or batch them, '
              f'{parent} first)')
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
    branch = 'refs/heads/wt/' + a.name
    if ok('rev-parse', '--verify', '-q', branch):
        own = own_commits(branch, landed_refs())
        listing = ''.join(f'  {c[:9]} {s[:100]}\n' for c, s in own)
        if own and not a.force:
            sys.exit(f'wt: wt/{a.name} has {len(own)} commit(s) on neither master nor a run branch (patch-equivalent '
                     f'copies count as landed):\n{listing}land them (wt.py bring-over {a.name} --target run/<date>), '
                     f'or pass --force to drop them; nothing removed')
        if own:
            print(f'--force: wt/{a.name} has {len(own)} unlanded commit(s):\n{listing}'
                  + (f'recover with `git branch wt/{a.name} {own[-1][0][:12]}` while git keeps it'
                     if a.delete_branch else f'wt/{a.name} itself is kept'))
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
    for f in (bo + '.state', path + '.rebase'):
        if os.path.exists(f):
            os.remove(f)
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


# ---------------------------------------------------------------- unlanded work (pipeline-plan §4 item 5)

def landed_refs(cwd=None):
    """Where a commit counts as landed: master, origin/master and every local run/* branch."""
    refs = [r for r in ('refs/heads/master', 'refs/remotes/origin/master')
            if ok('rev-parse', '--verify', '-q', r, cwd=cwd)]
    return refs + git('for-each-ref', '--format=%(refname)', 'refs/heads/run/', cwd=cwd).split()


_IDS = {}


def _identities(up, cwd=None):
    """{(author time, author, subject)} of every non-merge commit reachable from UP (cached per process): a
    cherry-pick or rebase keeps all three, whatever its conflict resolution did to the diff."""
    key = (rev(up, cwd=cwd), cwd or ROOT)
    if key not in _IDS:
        out = git('log', '--no-merges', '--format=%at%x09%an%x09%s', up, cwd=cwd)
        _IDS[key] = set(out.splitlines())
    return _IDS[key]


def own_commits(branch, upstreams, cwd=None):
    """[(sha, subject)], oldest first: BRANCH's commits landed on none of UPSTREAMS. Landed: reachable from one; a
    commit there with the same author time, author and subject (a cherry-pick or rebase of it, even one whose
    generated files took the target's side); patch-equivalent to one of its commits (`git cherry`); or touching
    generated files only (regenerated anyway). Shared by `remove`, `list --unlanded`, pacing.py and run_branch.py
    cleanup."""
    if not upstreams:
        raise ValueError('own_commits: no upstream to compare with')
    out = git('rev-list', '--reverse', '--no-merges', '--format=%at%x09%an%x09%s', branch, '--not', *upstreams,
              cwd=cwd).splitlines()
    reach = [(c[len('commit '):].strip(), ident) for c, ident in zip(out[0::2], out[1::2])]
    if not reach:
        return []
    known = set().union(*(_identities(up, cwd) for up in upstreams))
    left = {c for c, ident in reach if ident not in known}
    for c in sorted(left):
        names = git('diff-tree', '--no-commit-id', '--name-only', '-r', '--root', c, cwd=cwd).splitlines()
        if names and all(generated(p) for p in names):
            left.discard(c)
    for up in upstreams:
        if not left:
            break
        left &= {l[2:].strip() for l in git('cherry', up, branch, cwd=cwd).splitlines() if l.startswith('+ ')}
    return [(c, ident.split('\t', 2)[-1]) for c, ident in reach if c in left]


def unlanded_branches(cwd=None):
    """[(name, [(sha, subject)])] of the wt/* branches with commits landed nowhere (own_commits); every such branch
    counts as finished-but-unlanded (whether its builder still runs is not looked at)."""
    ups = landed_refs(cwd)
    out = []
    for ref in git('for-each-ref', '--format=%(refname:short)', 'refs/heads/wt/', cwd=cwd).split():
        own = own_commits(ref, ups, cwd)
        if own:
            out.append((ref[3:], own))
    return out


def unlanded_line(rows):
    """The one line `list --unlanded` starts with and pacing.py's heartbeat prints."""
    return (f'unlanded    {len(rows)} wt/* branch(es) with commits on neither master nor a run branch'
            + (': ' + ', '.join(f'{n} ({len(c)})' for n, c in rows) if rows else ''))


def cmd_list(a):
    if a.unlanded:
        rows = unlanded_branches()
        print(unlanded_line(rows))
        for name, own in rows:
            where = os.path.relpath(wt_path(name), ROOT) if os.path.isdir(wt_path(name)) else 'no worktree'
            print(f'  {name}  ({where})')
            for c, s in own[-10:]:
                print(f'    {c[:9]} {s[:100]}')
            if len(own) > 10:
                print(f'    ... {len(own) - 10} older')
        return
    rows = [('name', 'branch', 'base', 'ahead', 'state', 'path')]
    ups = None
    for p, b, head in worktrees():
        name = os.path.basename(p)
        base, stack = '-', ''
        if os.path.normcase(os.path.dirname(p)) == os.path.normcase(WTDIR) and os.path.exists(base_file(name)):
            base = read_base(name)[0] or '-'
            ups = landed_refs() if ups is None else ups
            st = stacked_on(name, ups)
            if st:                      # landed as is (its tip reachable): nothing to rebase for
                same = st[2] and any(ok('merge-base', '--is-ancestor', st[1], r) for r in ups)
                stack = f'; on wt/{st[0]}' + (' (unlanded)' if not st[2] else ' (parent landed)' if same
                                              else ' (parent landed: rebase)')
        if base == '-' and b:
            base = git('merge-base', 'master', b, check=False).stdout.decode().strip() or '-'
        ahead = git('rev-list', '--count', f'{base}..{head}').strip() if base != '-' else '?'
        st = git('status', '--porcelain', '-uno', cwd=p, check=False)
        n = len(st.stdout.splitlines()) if st.returncode == 0 else -1
        state = ('clean' if n == 0 else ('?' if n < 0 else f'dirty ({n})')) + stack
        rows.append((name, b or '(detached)', base[:9], ahead, state, os.path.relpath(p, ROOT)))
    w = [max(len(r[i]) for r in rows) for i in range(6)]
    for r in rows:
        print('  '.join(c.ljust(w[i]) for i, c in enumerate(r)).rstrip())


# ---------------------------------------------------------------- bring-over

def pairs_of(data):
    """(lib, file, address) triples of a ledger (ledger.load's dict, or a single-file JSON text; pairs_cmp.py's set)."""
    if not isinstance(data, dict):
        data = json.loads(data) if data else {}
    out = set()
    for lib, entry in data.items():
        for f, v in entry.get('files', {}).items():
            for x in v.get('in_tree', []):
                out.add((lib, f, x if isinstance(x, str) else str(x)))
    return out


def blob(spec, cwd=None):
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
    for defs in DEFS:
        if defs not in names:
            continue
        d = git('diff', '-U0', old, new, '--', defs)
        changed = '\n'.join(l for l in d.splitlines() if l[:1] in '+-' and not l.startswith(('+++', '---')))
        for top, lib in LIB_TOP.items():
            if re.search(r"(?i)(?<![\w/])(%s)[/\\]|'%s'" % (re.escape(top.split('/')[-1]), lib), changed):
                libs.add(lib)
    # the ledger: a changed file block or header shard (library.json is the verify's own record)
    for p in names:
        parts = p.split('/')
        if p.startswith(IMPORTS + '/') and len(parts) > 4 and parts[4] != ledger.LIBRARY:
            libs.add(parts[3])
    if OLD_IMPORTS in names:                    # across the split, or between two revisions before it
        ja, jb = ledger.load(old, repo=ROOT), ledger.load(new, repo=ROOT)
        for lib in set(ja) | set(jb):
            if ja.get(lib, {}).get('files') != jb.get(lib, {}).get('files'):
                libs.add(lib)
    return sorted(libs)


def run_logged(args, cwd, log, env=None):
    with open(log, 'w', newline='\n', encoding='utf-8', errors='replace') as f:
        r = subprocess.run(args, cwd=cwd, stdout=f, stderr=subprocess.STDOUT, env=env)
    tail = open(log, encoding='utf-8', errors='replace').read().splitlines()[-3:]
    return r.returncode, tail


def cherry_pick(bo, commits, log, took):
    """Cherry-pick commits in the scratch worktree, conflicts resolved as in a batch (resolve_batch: generated files
    take the target's side and are added to TOOK, to be regenerated; attempts.csv by row union); None or the
    refusal."""
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
        stop = resolve_batch(bo, conflicted, log, took) if conflicted else None
        if stop:
            git('cherry-pick', '--abort', cwd=bo, check=False)
            return (f'cherry-pick of {subj} conflicts in {stop[0]} ({stop[1]}; refused: `wt.py rebase '
                    f'NAME --onto TARGET` in the builder\'s worktree stops there for the builder to resolve)')
        if not git('diff', '--cached', '--name-only', 'HEAD', cwd=bo).strip():
            git('cherry-pick', '--skip', cwd=bo, env=env)       # the resolution left nothing new
            log.write('  empty after resolution, skipped\n')
            continue
        r = git('cherry-pick', '--continue', cwd=bo, check=False, env=env)
        if r.returncode:
            git('cherry-pick', '--abort', cwd=bo, check=False)
            return f'cherry-pick --continue of {subj}: {r.stderr.decode(errors="replace").strip()[-300:]}'
    if touched_names and not took:          # with TOOK, regen_and_commit regenerates them after the verify
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
    """vc6lock.py's importer lock (same file and format), so no verify rewrites the ledger while landing."""
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
        with open(LOCK) as f:
            mine = json.load(f).get('pid') == os.getpid()
        if mine:
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
    return ('conflict', f'{r.returncode} conflicting hunk(s) with the uncommitted copy')


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


def touched_by(lo, hi):
    """Every path a commit in LO..HI touches (per commit: a change made and undone still counts)."""
    out = git('log', '--no-renames', '--format=', '--name-only', f'{lo}..{hi}')
    return {p for p in out.splitlines() if p.strip()}


def replay(old, new, onto):
    """NEW's commits above OLD rebuilt on ONTO (plumbing, no worktree: a temporary index holds ONTO's tree and each
    commit's own changes are applied to it); authors and messages kept. Only for paths ONTO's commits since OLD left
    alone, so no commit can conflict. The new tip."""
    commits = git('rev-list', '--reverse', '--topo-order', f'{old}..{new}').split()
    if git('rev-list', '--merges', f'{old}..{new}').strip():
        raise ValueError(f'{new[:9]} has merge commits above {old[:9]}')
    with tempfile.TemporaryDirectory(dir=WTDIR) as td:
        env = dict(os.environ, GIT_INDEX_FILE=os.path.join(td, 'index'))
        git('read-tree', onto, env=env)
        parent = onto
        for c in commits:
            info = []
            f = git('diff-tree', '-r', '--no-renames', '--no-commit-id', '--raw', '-z', c + '^', c).split('\0')[:-1]
            for meta, path in zip(f[0::2], f[1::2]):        # -z: ':MODE MODE SHA SHA S' NUL path NUL
                _, mode, _, sha, status = meta.lstrip(':').split(' ')
                info.append(f'0 {"0" * 40}\t{path}' if status == 'D' else f'{mode} {sha}\t{path}')
            if info:
                git('update-index', '--index-info', env=env, inp=('\n'.join(info) + '\n').encode('utf-8', 'surrogateescape'))
            tree = git('write-tree', env=env).strip()
            raw = git('cat-file', 'commit', c).encode('utf-8', 'surrogateescape')
            head, _, msg = raw.partition(b'\n\n')
            author = [l for l in head.split(b'\n') if l.startswith(b'author ')][0][len(b'author '):].decode(
                'utf-8', 'surrogateescape')
            m = re.fullmatch(r'(.*) <(.*)> (\S+ \S+)', author)
            cenv = dict(os.environ, GIT_AUTHOR_NAME=m.group(1), GIT_AUTHOR_EMAIL=m.group(2), GIT_AUTHOR_DATE=m.group(3))
            parent = git('commit-tree', tree, '-p', parent, '-F', '-', env=cenv, inp=msg).strip()
        return parent


def land_onto(target, old, new, now):
    """(NEW replayed onto NOW, None) when TARGET's commits OLD..NOW touch no path NEW's commits touch and no generated
    file; else (None, why naming the overlapping paths)."""
    if not ok('merge-base', '--is-ancestor', old, now):
        return None, f'{target} moved to {now[:9]}, which does not contain {old[:9]} (rewritten?); nothing landed'
    theirs, mine = touched_by(old, now), touched_by(old, new)
    shared = sorted(theirs & mine)
    gen = sorted(p for p in theirs - mine if generated(p))
    n = git('rev-list', '--count', f'{old}..{now}').strip()
    if shared or gen:
        what = '; '.join(x for x in (shared and 'paths this landing changes as well: ' + ', '.join(shared[:10]),
                                     gen and 'generated files: ' + ', '.join(gen[:10])) if x)
        return None, (f'{target} moved {old[:9]} -> {now[:9]} while landing ({n} commit(s)) and they touch {what}; '
                      f'nothing landed, rerun')
    try:
        tip = replay(old, new, now)
    except ValueError as e:
        return None, f'{target} moved {old[:9]} -> {now[:9]} while landing and {e}; nothing landed, rerun'
    print(f'{target} moved {old[:9]} -> {now[:9]} while landing ({n} commit(s), touching none of the landing\'s paths '
          f'and no generated file): the landing replayed onto it, {new[:9]} -> {tip[:9]}', flush=True)
    return tip, None


def land(target, old, new, wait=900, refuse_dirty=False):
    """Compare-and-swap TARGET from OLD to NEW and bring the worktree that has it checked out along. When TARGET
    moved past OLD meanwhile by commits unrelated to the landing (land_onto), NEW's commits are replayed onto it
    first. (why, landed tip): why None when it landed."""
    holder = target_holders(target)
    hold_lock(wait)
    try:
        base, tip = old, new
        for _ in range(5):              # a plain `git commit` on TARGET doesn't take the lock: re-check each time
            now = rev('refs/heads/' + target)
            if now != base:
                tip, why = land_onto(target, old, new, now)
                if why:
                    return why, None
                base = now
            plan = {}
            paths = git('diff', '--name-only', '--no-renames', base, tip).splitlines()
            if refuse_dirty:
                d = dirty_target(target, paths)
                if d:
                    return f'{target} is dirty, not fast-forwarded ({d}); nothing landed', None
            for wt in holder:
                for p in paths:
                    m = merge_work(wt, p, base, tip)
                    if isinstance(m, tuple):
                        return f'{os.path.relpath(os.path.join(wt, p), ROOT)}: {m[1]}; nothing landed', None
                    if m != 'same':
                        plan[(wt, p)] = m
            r = git('update-ref', '-m', 'wt.py bring-over', 'refs/heads/' + target, tip, base, check=False)
            if r.returncode == 0:
                break
        else:
            now = rev('refs/heads/' + target)
            return f'{target} kept moving while landing (now {now[:9]}, built on {old[:9]}); nothing landed, rerun', None
        new, old = tip, base
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
        for wt in holder if paths else ():
            for _ in range(5):
                r = git('reset', '-q', new, '--', *paths, cwd=wt, check=False)
                if r.returncode in (0, 1):
                    break
                time.sleep(2)
            print(f'{os.path.relpath(wt, ROOT) if wt != ROOT else "main tree"}: '
                  f'{sum(1 for k in plan if k[0] == wt)} file(s) written, index entries of {len(paths)} reset')
        return None, new
    finally:
        release_lock()


def corrections_at(rev_):
    """The accepted corrections of that commit (a branch can't accept its own losses)."""
    t = blob(f'{rev_}:{CORRECTIONS}')
    return json.loads(t)['corrections'] if t else []


def judge_losses(before, after, target, old, what):
    """D202 on two ledgers (ledger.load's dicts): exits naming the addresses lost outright; else pairs_cmp's dict."""
    r = pairs_cmp.compare(before or {}, after or {}, corrections_at(old))
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


def standin_gate(old, new, old_ledger, new_ledger, target):
    """Pipeline-plan §4 item 6a: exits when OLD..NEW defines anew in app/ a function a hand stand-in still defines
    (standins.py; the new app pairs of NEW_LEDGER over OLD_LEDGER named by symbols.json as well)."""
    found, notes = standins.check(ROOT, old, new, old_ledger, new_ledger)
    for n in notes:
        print('stand-ins: note:', n)
    if found:
        sys.exit('wt: ' + '\n'.join(standins.report(found)) + f'\nwt: refused; {target} untouched')
    print('stand-ins: no collision')


def link_gate(a, bo, old, new, target):
    """Pipeline-plan §4 item 6b: when OLD..NEW touches app/, stubs/, programs/clientpoc/ or tests/clienttest/,
    linkgate.py in the scratch worktree BO (app.lib built there, the POC and clienttest linked, a.pool VMs); exits on
    a duplicate definition, an unresolved symbol not in its baseline, a compile error or no exe."""
    paths = git('diff', '--name-only', old, new).splitlines()
    if getattr(a, 'no_link', False):
        print('WARNING: --no-link: the link check (POC and clienttest) is skipped; the landing may still break them')
        return
    if not linkgate.touches(paths):
        print('link check: skipped, no path under ' + ', '.join(linkgate.TOUCH) + ' touched')
        return
    libs = [lib for lib in libs_touched(old, new, ()) if lib != 'app' and lib in EA_LIBS]
    problems, _ = linkgate.gate(bo, libs, a.pool, bo + '.link')
    if problems:
        sys.exit('wt: link check refused:\n' + '\n'.join('  ' + p for p in problems)
                 + f'\nwt: {target} untouched (logs {bo}.link.*.log)')
    print('link check: passed')


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
        other = [p for p in dirty if not generated(p)]
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
    """verify.py in bo, with the long lock wait; exits unless it ran to its last line."""
    print(f'verify --lib {" ".join(libs)} --pool {a.pool} in {bo} (log {bo}.verify.log)', flush=True)
    args = [sys.executable, *tool(bo, 'verify.py', 'verify'), '--pool', str(a.pool), '--lock-wait', str(a.lock_wait)]
    for lib in libs:
        args += ['--lib', lib]
    rc, tail = run_logged(args, bo, bo + '.verify.log')
    if rc:
        sys.exit(f'wt: verify failed ({rc}): {tail}; {target} untouched')
    short = verify_finished(bo + '.verify.log', libs)
    if short:
        sys.exit(f'wt: verify did not finish {short}: {tail}; {target} untouched')


def add_only_union(base, ours, theirs, name=IMPORTER):
    """(text, None) keeping both sides of a 3-way merge in which both sides only add lines, or (None, why). What git
    merges by itself stays merged; a line both sides added once; the result has to compile."""
    if base is None or ours is None or theirs is None:
        return None, 'added or deleted on one side'
    os.makedirs(WTDIR, exist_ok=True)
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
            compile(text, name, 'exec')
    except (SyntaxError, ValueError) as e:
        return None, f'the union does not compile ({e})'
    return text, None


def shard_union(base, ours, theirs):
    """A conflicted ledger shard in a batch: (None, None) to take the target's side (the verify rewrites its pairs),
    or (text, None) when only the branch changed its `description` (kept: no verify writes it), or (None, why)
    when both sides changed it."""
    try:
        b, o, t = (json.loads(x) if x else {} for x in (base, ours, theirs))
    except ValueError:
        return None, 'not JSON'
    bd, od, td = b.get('description'), o.get('description'), t.get('description')
    if td == bd or td == od:
        return None, None
    if od != bd:
        return None, 'both sides changed its description'
    if not o:
        return None, None
    o['description'] = td
    return ledger.dumps(o), None


def stage_text(bo, path, text):
    sha = git('hash-object', '-w', '--no-filters', '--stdin', cwd=bo, inp=text.encode('utf-8', 'surrogateescape')).strip()
    mode = git('ls-files', '-s', '--', path, cwd=bo).split()[0]
    git('update-index', '--cacheinfo', f'{mode},{sha},{path}', cwd=bo)
    git('checkout-index', '-f', '--', path, cwd=bo)


def resolve_batch(bo, conflicted, log, took=None):
    """Resolve what D202 lets a batch resolve (also a single bring-over's pick and `rebase`: in both, the index's
    stage 2 is the target's side); returns (file, why) of the first conflict it can't, or None. TOOK (a set) gains
    every generated file resolved, which the caller regenerates."""
    for p in conflicted:
        if not generated(p) and p != ATTEMPTS and p not in DEFS:
            return p, 'not a generated or row file'
    union = {}
    for p in DEFS:
        if p in conflicted:
            union[p], why = add_only_union(blob(':1:' + p, bo), blob(':2:' + p, bo), blob(':3:' + p, bo), p)
            if why:
                return p, why
    shard = {}
    for p in conflicted:
        if p.startswith(IMPORTS + '/'):
            shard[p], why = shard_union(blob(':1:' + p, bo), blob(':2:' + p, bo), blob(':3:' + p, bo))
            if why:
                return p, why
    for p in conflicted:
        if p == ATTEMPTS:
            stage_text(bo, p, union_rows(blob(':1:' + p, bo), blob(':2:' + p, bo) or '', blob(':3:' + p, bo) or ''))
            log.write('  attempts.csv: both sides\' rows\n')
        elif p in union:
            stage_text(bo, p, union[p])
            log.write(f'  {p}: both sides\' added lines, compiles\n')
        elif shard.get(p) is not None:
            stage_text(bo, p, shard[p])
            log.write(f'  {p}: the target\'s side, the branch\'s description (pairs regenerated later)\n')
        elif blob(':2:' + p, bo) is None:
            git('rm', '-q', '--', p, cwd=bo)                # deleted on the target's side
            log.write(f'  {p}: the target\'s side (deleted)\n')
        else:
            git('checkout', '--ours', '--', p, cwd=bo)
            git('add', '--', p, cwd=bo)
            log.write(f'  {p}: the target\'s side (regenerated later)\n')
        if took is not None and generated(p):
            took.add(p)
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
    return name, name, rev('refs/heads/wt/' + name), read_base(name)[0]


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
                took = set(state.get('took', []))
                stop = resolve_batch(bo, conflicted, log, took) if conflicted else None
                state['took'] = sorted(took)
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


def record_subject(what, res):
    """The generated commit's message: WHAT (e.g. 'of A, B after landing them together: one verify over all of
    them'), then the pairs."""
    lost = len(res.get('lost_outright', []))
    return (f'Record the verified pairs {what}, no source change, {len(res["gained"]) + len(res["gained_tiebreak"])} '
            f'gained, {len(res["moves"])} D66 moves, {lost} lost outright')


def regen_and_commit(a, bo, subject, target, old, took=()):
    """report, names.py, notexact.py (app_inventory.py when TOOK holds one of its files), and the regenerated files
    committed with SUBJECT and a.trailer (default the newest Co-Authored-By below OLD); exits on anything else
    changed. TARGET only names what stays untouched in the messages."""
    steps = [('report', tool(bo, 'report.py', 'report')),
             ('names', ['tools/re/names.py']), ('notexact', ['tools/re/notexact.py'])]
    if set(APP_INV) & set(took):
        steps.append(('appinv', ['tools/re/app_inventory.py']))
    if COVERAGE in took:
        print(f'note: {COVERAGE} took {target}\'s side; symbols.py rewrites it with symbols.json (every library\'s '
              f'verify objects) at the next regeneration')
    for tag, args in steps:
        rc, tail = run_logged([sys.executable, *args], bo, f'{bo}.{tag}.log')
        if rc or any('giving up' in t for t in tail):
            sys.exit(f'wt: {tag}: {tail}; {target} untouched')
    other = [p for p in dirty_tracked(bo) if not generated(p)]
    if other:
        sys.exit(f'wt: regenerating changed other files as well: {other[:10]}; {target} untouched')
    git('add', '-A', '--', *[g for g in (IMPORTS, *GENERATED) if os.path.exists(os.path.join(bo, *g.split('/')))],
        cwd=bo)
    if not git('diff', '--cached', '--name-only', cwd=bo).strip():
        print('the regenerated files equal the commits\' own')
        return
    trailer = a.trailer
    if not trailer:
        for line in git('log', '-50', '--format=%(trailers:key=Co-Authored-By)', old).splitlines():
            if line.strip():
                trailer = line.strip()
                break
    msg = subject + '\n'
    if trailer:
        msg += f'\n{trailer}\n'
    git('commit', '-q', '-F', '-', cwd=bo, inp=msg.encode())


def order_stacked(names, target):
    """Exit when a branch of NAMES (in landing order) is stacked on a parent whose commits are not on TARGET and the
    parent is not earlier in NAMES (pipeline-plan §4 item 4: a child never lands its parent's commits, nor before it)."""
    for i, n in enumerate(names):
        st = stacked_on(n, ['refs/heads/' + target])
        if not st or st[2] or st[0] in names[:i]:
            continue
        p = st[0]
        where = ('it comes after wt/' + n + ' in this batch: put it before' if p in names[i + 1:]
                 else 'land it first, or put it before wt/' + n + ' in one batch')
        sys.exit(f'wt: wt/{n} is stacked on wt/{p} (tip {st[1][:9]}), whose commits are not on {target} yet: {where} '
                 f'(`wt.py bring-over {p} {n} --target {target}`); nothing done')


def cmd_batch(a):
    specs = [batch_spec(s) for s in a.names]
    labels = [s[0] for s in specs]
    name = a.batch_name or 'batch-' + specs[0][1] + ('-' + specs[-1][1] if len(specs) > 1 else '')
    check_name(name)
    check_builders([s[1] for s in specs])
    tref = 'refs/heads/' + a.target
    bo = wt_path(name) + '.bo'
    order_stacked([s[1] for s in specs if '..' not in s[0]], a.target)
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
            print(f'note: {a.target} moved ({old[:9]} -> {rev(tref)[:9]}) since the batch began: the landing replays '
                  f'onto it if those commits touch none of the batch\'s paths and no generated file, else refuses')
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
        res = judge_losses(ledger.load(old, repo=ROOT), ledger.load(None, repo=bo), a.target, old, 'the batch')
        regen_and_commit(a, bo, record_subject(f'of {", ".join(labels)} after landing them together: one verify '
                                              f'over all of them', res), a.target, old, state.get('took', ()))
        new = rev('HEAD', cwd=bo)
        say_losses(res)
        standin_gate(old, new, ledger.load(old, repo=ROOT), ledger.load(new, repo=ROOT), a.target)
        link_gate(a, bo, old, new, a.target)
        if a.dry:
            print(f'--dry: would land {old[:9]} -> {new[:9]} on {a.target}')
            return
        why, landed = land(a.target, old, new, a.lock_wait, refuse_dirty=True)
        if why:
            sys.exit(f'wt: {why}')
        print(f'landed: {a.target} {old[:9]} -> {landed[:9]}')
        print(git('log', '--format=  %h %s', f'{old}..{landed}').rstrip()[:2000])
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
    order_stacked([a.name], a.target)
    base = read_base(a.name)[0] or git('merge-base', old, tip).strip()
    if git('rev-list', '--merges', f'{base}..{tip}').strip():
        sys.exit(f'wt: {branch} has merge commits after its base; refused')
    commits = git('rev-list', '--reverse', '--no-merges', '--right-only', '--cherry-pick',
                  f'{old}...{tip}').split()
    own = None
    if ok('merge-base', '--is-ancestor', base, tip):     # only the branch's own commits, as the batch form picks them
        own = set(git('rev-list', f'{base}..{tip}').split())
        commits = [c for c in commits if c in own]
        if not ok('merge-base', '--is-ancestor', base, old):
            print(f'note: {branch} is based on {base[:9]}, not on {a.target}: only its own commits are picked')
    if not commits:
        sys.exit(f'wt: nothing to bring over: every commit of {branch} is on {a.target}')
    print(f'{branch}: {len(commits)} commit(s) to bring onto {a.target} {old[:9]} (base {base[:9]})')
    bo = wt_path(a.name) + '.bo'
    logp = bo + '.log'
    if os.path.isdir(bo):
        drop_worktree(bo)
    ff = ok('merge-base', '--is-ancestor', old, tip) and (own is None or ok('merge-base', '--is-ancestor', base, old))
    git('worktree', 'add', '--detach', bo, tip if ff else old)
    furnish(bo, libs=False, pseudo=False)
    why = None
    took = set()                # generated files whose conflicts took the target's side (regenerated below)
    try:
        with open(logp, 'w', newline='\n', encoding='utf-8') as log:
            if ff:
                log.write(f'fast-forward: {a.target} {old[:9]} is an ancestor of {tip[:9]}\n')
                print(f'fast-forward: {a.target} is at or behind the branch')
            else:
                if old != base:
                    print(f'{a.target} moved past the base ({base[:9]} -> {old[:9]}): cherry-picking')
                why = cherry_pick(bo, commits, log, took)
        if why:
            sys.exit(f'wt: {why}; {a.target} untouched (log {logp})')
        new = rev('HEAD', cwd=bo)
        if new == old:
            print(f'nothing to bring over: every commit of {branch} was already on {a.target}')
            return
        libs = libs_touched(old, new, a.lib)
        before, cand_rec = ledger.load(old, repo=ROOT), ledger.load(new, repo=ROOT)
        cand = pairs_of(cand_rec)
        res = judge_losses(before, cand_rec, a.target, old, 'the branch\'s ledger record')
        after_rec = cand_rec
        if libs:
            run_verify(a, bo, libs, a.target)
            verified_rec = after_rec = ledger.load(None, repo=bo)
            verified = pairs_of(verified_rec)
            res = judge_losses(before, verified_rec, a.target, old, 'the verify')
            if verified != cand and not took:
                sys.exit(f'wt: the commits record other pairs than the verify gives (verify-only '
                         f'{sorted(verified - cand)[:5]}, record-only {sorted(cand - verified)[:5]}): commit '
                         f'the verified record in the branch and rerun; {a.target} untouched')
        else:
            print('no library touched: no verify; pairs from the records')
        if took:        # generated conflicts took the target's side: regenerate them and commit the result
            print(f'generated-file conflicts took {a.target}\'s side: {", ".join(sorted(took)[:8])}'
                  + (' ...' if len(took) > 8 else '') + '; regenerating')
            regen_and_commit(a, bo, record_subject(f'of wt/{a.name} after bringing it over onto {a.target}: its '
                                                   f'generated-file conflicts took {a.target}\'s side and were '
                                                   f'regenerated', res), a.target, old, took)
            new = rev('HEAD', cwd=bo)
        say_losses(res)
        standin_gate(old, new, before, after_rec, a.target)
        link_gate(a, bo, old, new, a.target)
        if a.dry:
            print(f'--dry: would land {old[:9]} -> {new[:9]} on {a.target}')
            return
        why, landed = land(a.target, old, new, a.lock_wait)
        if why:
            sys.exit(f'wt: {why}')
        print(f'landed: {a.target} {old[:9]} -> {landed[:9]}')
        print(git('log', '--format=  %h %s', f'{old}..{landed}').rstrip()[:2000])
        if ff and landed == tip:
            write_base(a.name, landed)
        else:
            print(f'note: {branch} keeps its own copies; a later bring-over skips them (patch-equivalent)')
    finally:
        if not a.keep and os.path.isdir(bo):
            drop_worktree(bo)


# ---------------------------------------------------------------- rebase

def read_text(path):
    with open(path, encoding='utf-8') as f:
        return f.read()


def current_branch(cwd=None):
    return git('symbolic-ref', '-q', '--short', 'HEAD', cwd=cwd, check=False).stdout.decode().strip()


def run_branch_of(name):
    """The run branch wt/NAME was based on: a run/* branch holding its recorded base (the main tree's current
    branch first, else the newest), else the main tree's current branch when it is a run branch."""
    cur = current_branch()
    base = read_base(name)[0]
    if base and ok('cat-file', '-e', base + '^{commit}'):
        holders = git('for-each-ref', '--sort=-committerdate', '--format=%(refname:short)', '--contains', base,
                      'refs/heads/run/').split()
        if cur in holders:
            return cur
        if holders:
            return holders[0]
    if cur.startswith('run/'):
        return cur
    sys.exit(f'wt: no run branch holds the base of {name} ({(base or "none recorded")[:9]}) and the main tree is on '
             f'{cur or "a detached HEAD"}: pass --onto BRANCH')


def rebase_in_progress(path):
    for d in ('rebase-merge', 'rebase-apply'):
        p = git('rev-parse', '--git-path', d, cwd=path).strip()
        if os.path.isdir(p if os.path.isabs(p) else os.path.join(path, p)):
            return True
    return False


def unmerged(path):
    return sorted(set(git('diff', '--name-only', '--diff-filter=U', cwd=path).splitlines()))


def rebase_steps(path, log, took, start=None):
    """Drive a rebase in PATH to its end, each conflict resolved by resolve_batch (generated files: the target's
    side, into TOOK; attempts.csv: both sides' rows; libdefs.py / importer.py: both sides' added lines). START: the
    `git rebase` arguments that begin it (None: carry on with the one in progress). None when it finished, else
    (file, why) with the rebase left stopped there."""
    env = dict(os.environ, GIT_EDITOR='true')
    if start:
        r = git('rebase', *start, cwd=path, check=False, env=env)
        log.write(f'git rebase {" ".join(start)}: {r.returncode}\n')
        if r.returncode and not rebase_in_progress(path):
            return '-', r.stderr.decode(errors='replace').strip()[-300:]
    while rebase_in_progress(path):
        conflicted = unmerged(path)
        if conflicted:
            stop = resolve_batch(path, conflicted, log, took)
            if stop:
                return stop
        head = rev('HEAD', cwd=path)
        staged = git('diff', '--cached', '--name-only', 'HEAD', cwd=path).strip()
        r = git('rebase', '--continue' if staged else '--skip', cwd=path, check=False, env=env)
        log.write(f'git rebase {"--continue" if staged else "--skip"}: {r.returncode}\n')
        if r.returncode and rebase_in_progress(path) and not unmerged(path) and rev('HEAD', cwd=path) == head:
            return '-', r.stderr.decode(errors='replace').strip()[-300:]
    return None


def regenerate_rebased(a, path, st):
    """After `rebase`: verify the libraries the branch's own commits touch (--pool K), report, names.py, notexact.py,
    and the generated files committed (subject like the batch form's)."""
    onto, name = st['onto_sha'], a.name
    target = f'wt/{name} (rebased; its generated files not yet regenerated)'
    libs = libs_touched(onto, rev('HEAD', cwd=path), a.lib)
    if libs:
        run_verify(a, path, libs, target)
    else:
        print('no library touched: no verify')
    res = pairs_cmp.compare(ledger.load(onto, repo=ROOT) or {}, ledger.load(None, repo=path) or {},
                            corrections_at(onto))
    if res['lost_outright']:
        print(f'WARNING: against {st["onto"]} the branch loses {len(res["lost_outright"])} address(es) outright: '
              f'{res["lost_outright"][:20]}; a bring-over would refuse it')
    regen_and_commit(a, path, record_subject(f'of wt/{name} after rebasing it onto {st["onto"]} {onto[:9]}: '
                                             f'generated files regenerated, not merged', res),
                     target, onto, st['took'])


def cmd_rebase(a):
    """wt.py rebase NAME [--onto B] [--continue]: see the module docstring."""
    check_name(a.name)
    path, branch = wt_path(a.name), 'wt/' + a.name
    statef, logp = path + '.rebase', path + '.rebase.log'
    if not os.path.isdir(path):
        sys.exit(f'wt: no worktree at {path}')
    me = os.path.join(ROOT, 'tools', 'vc6', 'wt.py')
    if a.cont:
        if not os.path.exists(statef):
            sys.exit(f'wt: nothing to continue ({statef} missing): `wt.py rebase {a.name}` starts one')
        st = json.loads(read_text(statef))
        start = None
        if not rebase_in_progress(path) and not ok('merge-base', '--is-ancestor', st['onto_sha'], 'HEAD', cwd=path):
            os.remove(statef)
            sys.exit(f'wt: {branch} does not contain {st["onto"]} {st["onto_sha"][:9]} (the rebase was aborted?): '
                     f'start again with `wt.py rebase {a.name}`')
    else:
        if rebase_in_progress(path):
            sys.exit(f'wt: a rebase is in progress in {path}: `python {me} rebase {a.name} --continue` after resolving '
                     f'it, or `git -C {path} rebase --abort`')
        if current_branch(path) != branch:
            sys.exit(f'wt: {path} is not on {branch}')
        dirty = dirty_tracked(path)
        other = [p for p in dirty if not generated(p)]
        if other:
            sys.exit(f'wt: {path} has uncommitted changes outside the generated files: {other[:10]}: commit or drop '
                     f'them first; nothing done')
        if dirty:
            git('checkout', 'HEAD', '--', *dirty, cwd=path)
            print(f'generated leftovers dropped (regenerated below): {", ".join(dirty)}')
        onto_name = a.onto or run_branch_of(a.name)
        st = {'onto': onto_name, 'onto_sha': rev(onto_name), 'old': rev('HEAD', cwd=path), 'took': [], 'parent': None}
        if ok('merge-base', '--is-ancestor', st['onto_sha'], st['old']):
            print(f'{branch} already contains {onto_name} {st["onto_sha"][:9]}: nothing to rebase')
            return
        start = [st['onto_sha']]
        stack = stacked_on(a.name, [st['onto_sha']])
        if stack:                       # pipeline-plan §4 item 4: replay the child's own commits only
            parent, ptip, landed = stack
            if re.fullmatch(r'(?:refs/heads/)?wt/' + re.escape(parent), onto_name):
                st['parent'] = parent   # onto the parent's newer tip: still stacked on it
            elif not landed:
                sys.exit(f'wt: {branch} is stacked on wt/{parent} (tip {ptip[:9]}), whose commits are not on {onto_name} '
                         f'yet: land wt/{parent} first (or `wt.py rebase {a.name} --onto wt/{parent}`); nothing done')
            if ok('merge-base', '--is-ancestor', ptip, st['old']):
                start = ['--onto', st['onto_sha'], ptip]
                print(f'stacked on wt/{parent}: only the commits above its recorded tip {ptip[:9]} are replayed')
        print(f'rebasing {branch} {st["old"][:9]} onto {onto_name} {st["onto_sha"][:9]} (log {logp})', flush=True)
    with open(logp, 'a', newline='\n', encoding='utf-8') as log:
        took = set(st['took'])
        stop = rebase_steps(path, log, took, start)
        st['took'] = sorted(took)
    with open(statef, 'w', newline='\n') as f:
        json.dump(st, f)
    if stop:
        at = git('log', '-1', '--format=%h %s', 'REBASE_HEAD', cwd=path, check=False).stdout.decode().strip()[:90]
        file_ = stop[0]
        sys.exit(f'wt: the rebase of {branch} onto {st["onto"]} stopped at {at or "?"}: '
                 + (f'conflict in {file_} ({stop[1]})' if file_ != '-' else stop[1]) + '\n'
                 f'finish it:  resolve {file_ if file_ != "-" else "it"} in {path}, then\n'
                 + (f'            git -C {path} add {file_}\n' if file_ != '-' else '')
                 + f'            python {me} rebase {a.name} --continue   (the same resolution for the rest, then the '
                 f'regeneration)\n'
                 f'or give up: git -C {path} rebase --abort   ({branch} back at {st["old"][:9]})')
    print(f'rebased: {branch} {st["old"][:9]} -> {rev("HEAD", cwd=path)[:9]} on {st["onto"]}'
          + (f'; generated conflicts took its side: {", ".join(st["took"][:8])}' if st['took'] else ''), flush=True)
    regenerate_rebased(a, path, st)
    os.remove(statef)
    write_base(a.name, st['onto_sha'], st.get('parent'))
    print(f'done: {branch} at {rev("HEAD", cwd=path)[:9]} '
          f'({git("log", "-1", "--format=%s", "HEAD", cwd=path).strip()[:80]}); base recorded {st["onto_sha"][:9]}')


def main():
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    sub = ap.add_subparsers(dest='cmd', required=True)
    p = sub.add_parser('new')
    p.add_argument('name')
    p.add_argument('--base', default='HEAD')
    p = sub.add_parser('list')
    p.add_argument('--unlanded', action='store_true',
                   help='the wt/* branches with commits on neither master nor a run branch (patch-equivalence counted)')
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
    p.add_argument('--no-replay', action='store_true', help='ignored: no replay since the freeze (D211)')
    p.add_argument('--dry', action='store_true', help='stop before landing')
    p.add_argument('--keep', action='store_true', help='keep the scratch worktree NAME.bo')
    p.add_argument('--no-link', action='store_true',
                   help='skip the link check (item 6b; doc-only or tools-only landings), with a warning')
    p = sub.add_parser('rebase')
    p.add_argument('name')
    p.add_argument('--onto', help='the branch to rebase onto (default the run branch wt/NAME was based on)')
    p.add_argument('--continue', dest='cont', action='store_true',
                   help='carry on after resolving a conflict the rebase stopped at (then the regeneration)')
    p.add_argument('--lib', action='append', default=[], help='verify this library as well (repeatable)')
    p.add_argument('--pool', type=int, default=1)
    p.add_argument('--lock-wait', type=int, default=3000, help='seconds each importer call waits for the lock')
    p.add_argument('--trailer', help='the generated commit\'s trailer line (default the newest on the onto branch)')
    p = sub.add_parser('remove')
    p.add_argument('name')
    p.add_argument('--delete-branch', action='store_true')
    p.add_argument('--force', action='store_true',
                   help='remove a worktree with uncommitted changes or a branch with unlanded commits')
    a = ap.parse_args()
    os.makedirs(WTDIR, exist_ok=True)
    {'new': cmd_new, 'list': cmd_list, 'bring-over': cmd_bring_over, 'rebase': cmd_rebase,
     'remove': cmd_remove}[a.cmd](a)


if __name__ == '__main__':
    main()
