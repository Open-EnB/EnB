"""Verify pull requests as they come, and land the proven ones (docs/github-plan.md 4.5.5, P-GH-10 steps 3-4):
a polling service, no runner.

    python tools/vc6/verify_watch.py once [--dry]       one pass over the open PRs (what the task runs)
    python tools/vc6/verify_watch.py launch [--ref R]   update the verifier worktree to origin/master (or R)
                                                        and run `once` from it
    python tools/vc6/verify_watch.py land PR [--dry]    build and open PR's landing now (a maintainer's decision)
    python tools/vc6/verify_watch.py automerge on|off   whether `once` lands eligible PRs by itself (default off)
    python tools/vc6/verify_watch.py install [--minutes N] | uninstall | status

GitHub never sends this host a job: a Windows scheduled task ("EnB verify watch", every N minutes, default 5,
while the user is logged on, without a window: conhost --headless) runs `launch`, which fetches master, moves
the dedicated worktree .work/wt/verifier there and runs master's own `once`. So the code that runs is always
master's, never a pull request's, and the PRs' C++ reaches only a build VM's compiler (verify_pr.py).

Verifying. `once` takes the open, non-draft PRs to master. A PR is verified (tools/vc6/verify_pr.py, its
status `vc6-verify` and one comment) when its head has no `vc6-verify` result yet, or its result was made on
an older master. A draft is skipped: its author is still working (a maintainer's run PR is verified when it
leaves draft at handoff). verify_pr.py's gate stops a PR that changes anything a verify would run on the
host; the PR is then marked `vc6-verify: pending` until a maintainer has read it and comments
`/verify-approved SHA` (at least 7 hex digits of the head commit; a new push needs a new approval).
A maintainer's own PRs (author association OWNER or MEMBER: the org's people) are approved by their authorship.

Landing (a contributor's PR; a maintainer's own PRs are merged by the maintainer as they are). A contributor
without VC6 cannot commit the verified record, so the PR is not merged as it is: its landing is one commit on
master, GitHub's test merge's tree plus the files the verify rewrote (imports.json, ...), authored by the PR's
author (other authors as Co-authored-by), pushed as the branch `verified/prN` and opened as a landing PR that
names the original and copies its `Claims #N` lines (so the claims land). The landing carries the original's
vc6-verify result; once the hosted `checks` pass on it, the next pass rebase-merges it (one commit, author
kept), comments on the original and closes it. A landing whose master moved is closed and rebuilt after the
original is verified again.
  A PR lands by itself when automerge is on and it is eligible: vc6-verify success on the current master,
  nothing lost, replay clean, the hosted `checks` passed on its head, it changes only C/C++ sources and headers
  (outside tools/ and .github/) and docs/data, and no maintainer has commented `/hold` (later than any
  `/unhold`). Anything else lands when a maintainer comments `/land SHA` naming its head (or runs `land PR`),
  provided it is verified on the current master and lost nothing.
D66 moves (every lost address held by another file, none lost outright) fail the verify until a maintainer
comments `/accept-d66 SHA` naming the head; the next pass verifies it again, and verify_pr.py then counts the
moves as accepted (success, named in the status and the comment), so the PR merges or lands like one that lost
nothing. A pair lost outright still fails: the user decides (D66) and no command passes it.
One pass at a time (.work/verify/watch.lock); log .work/verify/watch.log.
"""
import argparse
import datetime as dt
import json
import os
import re
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import wt  # noqa: E402
import verify_pr  # noqa: E402

ROOT = wt.ROOT
OUT = os.path.join(ROOT, '.work', 'verify')
LOG = os.path.join(OUT, 'watch.log')
LOCK = os.path.join(OUT, 'watch.lock')
AUTOMERGE = os.path.join(OUT, 'automerge')
VERIFIER = wt.wt_path('verifier')
TASK = 'EnB verify watch'
LANDING = 'verified/pr'
MAINTAINER = {'OWNER', 'MEMBER'}
APPROVE = re.compile(r'^/verify-approved\s+([0-9a-f]{7,40})\b', re.I | re.M)
LAND = re.compile(r'^/land\s+([0-9a-f]{7,40})\b', re.I | re.M)
HOLD = re.compile(r'^/(hold|unhold)\b', re.I | re.M)
CLAIMS_REF = re.compile(r'(?i)\bclaims?\s+#(\d+)')
WAITING = 'waiting for a maintainer to read it: /verify-approved '
# what a PR may change and still land without a maintainer's /land
BOT_FILES = [re.compile(p, re.I) for p in (r'^(?!tools/|\.github/).*\.(cpp|c|cc|h|hpp|inl)$', r'^docs/data/')]


def log(msg):
    os.makedirs(OUT, exist_ok=True)
    if os.path.exists(LOG) and os.path.getsize(LOG) > 2_000_000:
        os.replace(LOG, LOG + '.1')
    line = dt.datetime.now().strftime('%Y-%m-%d %H:%M:%S ') + msg
    with open(LOG, 'a', encoding='utf-8', newline='\n') as f:
        f.write(line + '\n')
    print(line, flush=True)


def gh_json(path):
    out = verify_pr.gh('api', path)
    return json.loads(out) if out.strip() else None


def take_lock():
    os.makedirs(OUT, exist_ok=True)
    if os.path.exists(LOCK):
        try:
            pid = int(open(LOCK).read().strip() or 0)
        except ValueError:
            pid = 0
        if pid and wt._pid_alive(pid):
            return False
    with open(LOCK, 'w', newline='\n') as f:
        f.write(str(os.getpid()))
    return True


def maintainer_comments(r, number):
    return [c for c in gh_json(f'repos/{r}/issues/{number}/comments?per_page=100') or []
            if c.get('author_association') in MAINTAINER]


def names_head(comments, rx, head):
    """True when a maintainer's comment matching RX names HEAD (at least 7 hex digits)."""
    return any(head.lower().startswith(sha.lower()) for c in comments for sha in rx.findall(c.get('body') or ''))


def held(comments):
    last = None
    for c in comments:
        for m in HOLD.findall(c.get('body') or ''):
            last = m.lower()
    return last == 'hold'


def set_status(r, head, state, desc):
    verify_pr.gh('api', '-X', 'POST', f'repos/{r}/statuses/{head}', '-f', f'state={state}', '-f',
                 f'context={verify_pr.CONTEXT}', '-f', f'description={desc[:139]}')


def hosted_checks(r, sha):
    """The hosted CI job `checks` on SHA: 'success', 'failure' or 'pending'."""
    runs = [x for x in (gh_json(f'repos/{r}/commits/{sha}/check-runs?per_page=100') or {}).get('check_runs', [])
            if x['name'] == 'checks']
    if not runs:
        return 'pending'
    run = max(runs, key=lambda x: x['id'])
    if run['status'] != 'completed':
        return 'pending'
    return 'success' if run['conclusion'] == 'success' else 'failure'


def record(n):
    p = os.path.join(OUT, f'pr{n}.json')
    return json.load(open(p, encoding='utf-8')) if os.path.exists(p) else {}


def save_record(n, rec):
    with open(os.path.join(OUT, f'pr{n}.json'), 'w', encoding='utf-8', newline='\n') as f:
        json.dump(rec, f, indent=1)


# ---------------------------------------------------------------- landing

def landable(rec, head, master):
    """Why PR's verified result cannot land now, or None."""
    if rec.get('head') != head:
        return 'not verified at its head'
    if rec.get('base') != master:
        return 'verified on an older master'
    if rec.get('verdict') != 'success':
        return 'vc6-verify is not success'
    if rec.get('lost') and not rec.get('accepted'):
        return 'it loses pairs (a maintainer decides, D66: /accept-d66 SHA for moves)'
    return None


def landing_commit(n, rec, title, master):
    """The landing commit (not pushed): the test merge's tree plus the files the verify rewrote, on MASTER."""
    merge, head = rec['merge'], rec['head']
    if wt.rev(merge + '^1') != master:
        raise RuntimeError(f'the test merge {merge[:9]} is not on master {master[:9]}')
    idx = os.path.join(tempfile.mkdtemp(prefix='enb-land-'), 'index')
    env = dict(os.environ, GIT_INDEX_FILE=idx)
    wt.git('read-tree', merge, env=env)
    keep = os.path.join(OUT, f'pr{n}.files')
    for p in rec.get('rewritten', []):
        blob = wt.git('hash-object', '-w', os.path.join(keep, *p.split('/'))).strip()
        wt.git('update-index', '--add', '--cacheinfo', f'100644,{blob},{p}', env=env)
    tree = wt.git('write-tree', env=env).strip()
    authors = []
    for line in wt.git('log', '--format=%an%x00%ae', f'{master}..{head}').splitlines():
        if line and line not in authors:
            authors.append(line)
    name, email = (authors[-1] if authors else 'unknown\0unknown').split('\0')
    co = [a.split('\0') for a in authors[:-1]]
    commits = wt.git('log', '--reverse', '--format=* %h %s', f'{master}..{head}').strip()
    rewritten = ', '.join(rec.get('rewritten', [])) or 'none'
    msg = (f'{title} (#{n})\n\n{rec["summary"]}. Landed from #{n} by the verify service: GitHub\'s test merge '
           f'{merge[:9]} verified on master {master[:9]} (tools/vc6/verify_pr.py), with the files the verify '
           f'rewrote: {rewritten}.\n\nThe pull request\'s commits:\n{commits}\n')
    if co:
        msg += '\n' + '\n'.join(f'Co-authored-by: {a} <{b}>' for a, b in co) + '\n'
    cenv = dict(os.environ, GIT_AUTHOR_NAME=name, GIT_AUTHOR_EMAIL=email)
    return _commit_tree(tree, master, msg, cenv)


def _commit_tree(tree, parent, msg, env):
    r = subprocess.run(['git', 'commit-tree', tree, '-p', parent], cwd=ROOT, input=msg.encode('utf-8'),
                       capture_output=True, env=env)
    if r.returncode:
        raise RuntimeError('git commit-tree: ' + r.stderr.decode(errors='replace'))
    return r.stdout.decode().strip()


def open_landing(r, p, rec, master, dry=False):
    n = p['number']
    sha = landing_commit(n, rec, p['title'], master)
    claims = sorted({int(x) for x in CLAIMS_REF.findall(p.get('body') or '')})
    body = (f'Lands #{n} by @{p["user"]["login"]} as one commit: its content plus the files the verify rewrote '
            f'({", ".join(rec.get("rewritten", [])) or "none"}), so a contributor without VC6 need not commit them '
            f'(docs/github-plan.md 4.5.5).\n\nvc6-verify of #{n}: {rec["summary"]}.\n\n'
            + ''.join(f'Claims #{c}\n' for c in claims)
            + '\nOpened by `tools/vc6/verify_watch.py`; merged by it once `checks` pass.')
    if dry:
        print(f'(dry) landing for #{n}: {sha[:9]} on master {master[:9]}')
        print(wt.git('show', '--stat', '--format=%an <%ae>%n%B', sha)[:3000])
        print(body)
        return sha
    branch = f'{LANDING}{n}'
    push = subprocess.run(['git', 'push', '--force', 'origin', f'{sha}:refs/heads/{branch}'], cwd=ROOT,
                          capture_output=True, text=True)
    if push.returncode:
        raise RuntimeError('git push: ' + (push.stderr or push.stdout)[-500:])
    set_status(r, sha, 'success', f'verified as #{n}: {rec["summary"]}')
    existing = gh_json(f'repos/{r}/pulls?state=open&head={r.split("/")[0]}:{branch}') or []
    if existing:
        m = existing[0]['number']
        verify_pr.gh('api', '-X', 'PATCH', f'repos/{r}/pulls/{m}', '-f', f'body={body}')
    else:
        out = verify_pr.gh('pr', 'create', '-R', r, '--base', 'master', '--head', branch,
                           '--title', f'Land #{n}: {p["title"]}', '--body', body)
        m = int(out.strip().rsplit('/', 1)[-1])
    rec['landing'] = {'sha': sha, 'pr': m, 'merge': rec['merge']}
    save_record(n, rec)
    log(f'#{n}: landing {sha[:9]} opened as #{m}')
    return sha


def tend_landings(r, prs, master):
    """Merge landings whose checks passed; close those that went stale or lost their original."""
    by_number = {p['number']: p for p in prs}
    me = None
    for lp in prs:
        ref = lp['head']['ref']
        if not ref.startswith(LANDING):
            continue
        me = me or verify_pr.gh('api', 'user', '-q', '.login').strip()
        if lp['user']['login'] != me:
            continue
        n = int(ref[len(LANDING):])
        orig, m, sha = by_number.get(n), lp['number'], lp['head']['sha']

        def drop(why):
            verify_pr.gh('pr', 'close', str(m), '-R', r, '--delete-branch', '--comment', f'Closed: {why}.')
            log(f'#{n}: landing #{m} closed ({why})')
        if orig is None:
            drop(f'#{n} is no longer open')
            continue
        if not wt.ok('cat-file', '-e', sha):
            wt.git('fetch', '-q', 'origin', f'+refs/heads/{ref}:refs/enb/landing/{n}', check=False)
        if not wt.ok('cat-file', '-e', sha) or wt.rev(sha + '^1') != master:
            drop('master moved; it is rebuilt after the original is verified again')
            continue
        rec = record(n)
        if rec.get('head') != orig['head']['sha']:
            drop(f'#{n} has new commits')
            continue
        state = hosted_checks(r, sha)
        if state == 'failure':
            log(f'#{n}: landing #{m}: checks failed; left for the maintainer')
            continue
        if state != 'success':
            continue
        verify_pr.gh('pr', 'merge', str(m), '-R', r, '--rebase', '--delete-branch')
        landed = gh_json(f'repos/{r}/pulls/{m}')
        verify_pr.gh('pr', 'close', str(n), '-R', r, '--comment',
                     f'Landed as {landed.get("merge_commit_sha", "?")[:9]} through #{m}, with the verified record. '
                     'Thank you!')
        log(f'#{n}: landed through #{m}')


def eligible(r, p, rec, master, comments):
    """(lands by itself?, why not)."""
    why = landable(rec, p['head']['sha'], master)
    if why:
        return False, why
    if held(comments):
        return False, 'held by /hold'
    paths = wt.git('diff', '--name-only', rec['base'], rec['merge']).splitlines()
    other = [x for x in paths if not any(rx.search(x) for rx in BOT_FILES)]
    if other:
        return False, f'changes {len(other)} file(s) beyond sources and docs/data (needs /land SHA): {other[:3]}'
    if hosted_checks(r, p['head']['sha']) != 'success':
        return False, 'the hosted checks have not passed on its head'
    return True, None


# ---------------------------------------------------------------- the pass

def once(dry=False):
    if not take_lock():
        print('another pass is running')
        return
    try:
        r = verify_pr.repo()
        master = gh_json(f'repos/{r}/branches/master')['commit']['sha']
        wt.git('fetch', '-q', 'origin', '+refs/heads/master:refs/remotes/origin/master')
        prs = gh_json(f'repos/{r}/pulls?state=open&base=master&per_page=100') or []
        todo = 0
        for p in sorted(prs, key=lambda p: p['number']):
            n, head = p['number'], p['head']['sha']
            if p.get('draft') or p['head']['ref'].startswith(LANDING):
                continue
            st = gh_json(f'repos/{r}/commits/{head}/status')
            mine = [s for s in st.get('statuses', []) if s['context'] == verify_pr.CONTEXT]
            rec = record(n)
            done = mine and mine[0]['state'] in ('success', 'failure') and rec.get('head') == head
            maint = p.get('author_association') in MAINTAINER
            comments = None
            why = None
            if not done:
                why = 'new head ' + head[:9]
            elif rec.get('base') != master:
                why = f'master moved ({str(rec.get("base"))[:9]} -> {master[:9]})'
            elif rec.get('verdict') == 'failure' and rec.get('moves') and not rec.get('outright') \
                    and not rec.get('accepted'):
                comments = maintainer_comments(r, n)
                if verify_pr.accepted_by(comments, head):
                    why = 'D66 moves accepted (/accept-d66)'
            if why:
                comments = comments if comments is not None else maintainer_comments(r, n)
                approved = maint or names_head(comments, APPROVE, head)
                if mine and mine[0]['state'] == 'pending' and mine[0]['description'].startswith(WAITING) \
                        and not approved:
                    continue                    # still waiting for the maintainer's approval
                todo += 1
                log(f'#{n}: {why}' + (' (approved)' if approved and not maint else ''))
                if dry:
                    continue
                args = [sys.executable, os.path.join(HERE, 'verify_pr.py'), str(n)] + (['--approved'] if approved else [])
                with open(os.path.join(OUT, f'pr{n}.run.log'), 'w', encoding='utf-8', newline='\n') as f:
                    rc = subprocess.run(args, cwd=ROOT, stdout=f, stderr=subprocess.STDOUT).returncode
                if rc == verify_pr.GATED:
                    set_status(r, head, 'pending', WAITING + head[:7])
                    verify_pr.gh('api', '-X', 'POST', f'repos/{r}/issues/{n}/comments', '-f', 'body='
                                 f'This pull request changes files that would run on the verifying host (see the list '
                                 f'in `verify_pr.py`\'s gate, docs/github-plan.md 4.5.5). A maintainer reads them, then '
                                 f'comments `/verify-approved {head[:7]}` to verify this commit.')
                    log(f'#{n}: gated, waiting for /verify-approved {head[:7]}')
                    continue
                log(f'#{n}: verify_pr exit {rc} (0 success, 1 failure; .work/verify/pr{n}.run.log)')
                rec = record(n)
            if maint or dry:
                continue                        # a maintainer's PR is merged by the maintainer as it is
            if rec.get('landing', {}).get('merge') == rec.get('merge') and rec.get('base') == master:
                continue                        # its landing is open for this result
            comments = comments if comments is not None else maintainer_comments(r, n)
            asked = names_head(comments, LAND, head)
            ok, why = eligible(r, p, rec, master, comments)
            if asked and landable(rec, head, master) is None:
                ok = True
            if ok and (asked or os.path.exists(AUTOMERGE)):
                try:
                    open_landing(r, p, rec, master)
                except RuntimeError as e:
                    log(f'#{n}: landing failed: {e}')
        if not dry:
            tend_landings(r, prs, master)
        if not todo:
            print(f'nothing to verify ({len(prs)} open PRs)')
    finally:
        if os.path.exists(LOCK):
            os.remove(LOCK)


def land(n, dry):
    r = verify_pr.repo()
    master = gh_json(f'repos/{r}/branches/master')['commit']['sha']
    wt.git('fetch', '-q', 'origin', '+refs/heads/master:refs/remotes/origin/master',
           f'+refs/pull/{n}/head:refs/enb/pr/{n}/head', f'+refs/pull/{n}/merge:refs/enb/pr/{n}/merge')
    p = gh_json(f'repos/{r}/pulls/{n}')
    rec = record(n)
    why = landable(rec, p['head']['sha'], master)
    if why:
        sys.exit(f'verify_watch: #{n} cannot land: {why} (verify it first: verify_pr.py {n})')
    open_landing(r, p, rec, master, dry)


def launch(ref):
    wt.git('fetch', '-q', 'origin', '+refs/heads/master:refs/remotes/origin/master')
    target = wt.rev(ref if ref else 'refs/remotes/origin/master')
    if not os.path.isdir(VERIFIER):
        wt.git('worktree', 'add', '--detach', VERIFIER, target)
        wt.furnish(VERIFIER, libs=False, pseudo=False)
    else:
        wt.git('checkout', '-q', '--detach', '--force', target, cwd=VERIFIER)
    script = os.path.join(VERIFIER, 'tools', 'vc6', 'verify_watch.py')
    if not os.path.exists(script):
        log(f'launch: {target[:9]} has no tools/vc6/verify_watch.py yet; nothing to do')
        return
    subprocess.run([sys.executable, script, 'once'], cwd=VERIFIER)


def task_command(minutes):
    py = sys.executable
    me = os.path.join(ROOT, 'tools', 'vc6', 'verify_watch.py')
    tr = f'conhost.exe --headless "{py}" "{me}" launch'
    return ['schtasks', '/Create', '/TN', TASK, '/SC', 'MINUTE', '/MO', str(minutes), '/TR', tr, '/F']


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = ap.add_subparsers(dest='cmd', required=True)
    sub.add_parser('once').add_argument('--dry', action='store_true')
    sub.add_parser('launch').add_argument('--ref')
    p = sub.add_parser('land')
    p.add_argument('pr', type=int)
    p.add_argument('--dry', action='store_true')
    sub.add_parser('automerge').add_argument('state', choices=['on', 'off'])
    sub.add_parser('install').add_argument('--minutes', type=int, default=5)
    sub.add_parser('uninstall')
    sub.add_parser('status')
    a = ap.parse_args()
    if a.cmd == 'once':
        once(a.dry)
    elif a.cmd == 'launch':
        launch(a.ref)
    elif a.cmd == 'land':
        land(a.pr, a.dry)
    elif a.cmd == 'automerge':
        os.makedirs(OUT, exist_ok=True)
        if a.state == 'on':
            open(AUTOMERGE, 'w').close()
        elif os.path.exists(AUTOMERGE):
            os.remove(AUTOMERGE)
        print(f'automerge {a.state}')
    elif a.cmd == 'install':
        subprocess.run(task_command(a.minutes), check=True)
        print(f'installed "{TASK}": every {a.minutes} min while you are logged on; log {LOG}')
    elif a.cmd == 'uninstall':
        subprocess.run(['schtasks', '/Delete', '/TN', TASK, '/F'], check=True)
    else:
        subprocess.run(['schtasks', '/Query', '/TN', TASK, '/V', '/FO', 'LIST'])
        print('automerge', 'on' if os.path.exists(AUTOMERGE) else 'off')
        if os.path.exists(LOG):
            print(''.join(open(LOG, encoding='utf-8').readlines()[-10:]))


if __name__ == '__main__':
    main()
