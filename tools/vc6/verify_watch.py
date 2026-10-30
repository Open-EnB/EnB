"""Verify pull requests as they come (docs/github-plan.md 4.5.5, P-GH-10 step 3): a polling service, no runner.

    python tools/vc6/verify_watch.py once [--dry]       one pass over the open PRs (what the task runs)
    python tools/vc6/verify_watch.py launch [--ref R]   update the verifier worktree to origin/master (or R)
                                                        and run `once` from it
    python tools/vc6/verify_watch.py install [--minutes N] | uninstall | status

GitHub never sends this host a job: a Windows scheduled task ("EnB verify watch", every N minutes, default 5,
while the user is logged on, without a window: conhost --headless) runs `launch`, which fetches master, moves
the dedicated worktree .work/wt/verifier there and runs master's own `once`. So the code that runs is always
master's, never a pull request's, and the PRs' C++ reaches only a build VM's compiler (verify_pr.py).

`once` takes the open, non-draft PRs to master. A PR is verified (tools/vc6/verify_pr.py, its status
`vc6-verify` and one comment) when its head has no `vc6-verify` result yet, or its result was made on an
older master. A draft is skipped: its author is still working (a maintainer's run PR is verified when it
leaves draft at handoff). verify_pr.py's gate stops a PR that changes anything a verify would run on the
host; the PR is then marked `vc6-verify: pending` until a maintainer has read it and comments
`/verify-approved SHA` (at least 7 hex digits of the head commit; a new push needs a new approval).
A maintainer's own PRs (author association OWNER, MEMBER or COLLABORATOR) are approved by their authorship.
One pass at a time (.work/verify/watch.lock); log .work/verify/watch.log.
"""
import argparse
import datetime as dt
import json
import os
import re
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import wt  # noqa: E402
import verify_pr  # noqa: E402

ROOT = wt.ROOT
OUT = os.path.join(ROOT, '.work', 'verify')
LOG = os.path.join(OUT, 'watch.log')
LOCK = os.path.join(OUT, 'watch.lock')
VERIFIER = wt.wt_path('verifier')
TASK = 'EnB verify watch'
MAINTAINER = {'OWNER', 'MEMBER', 'COLLABORATOR'}
APPROVE = re.compile(r'^/verify-approved\s+([0-9a-f]{7,40})\b', re.I | re.M)
WAITING = 'waiting for a maintainer to read it: /verify-approved '


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


def approved_sha(r, number, head):
    """True when a maintainer commented /verify-approved naming HEAD."""
    for c in gh_json(f'repos/{r}/issues/{number}/comments?per_page=100') or []:
        if c.get('author_association') not in MAINTAINER:
            continue
        for sha in APPROVE.findall(c.get('body') or ''):
            if head.lower().startswith(sha.lower()):
                return True
    return False


def set_status(r, head, state, desc):
    verify_pr.gh('api', '-X', 'POST', f'repos/{r}/statuses/{head}', '-f', f'state={state}', '-f',
                 f'context={verify_pr.CONTEXT}', '-f', f'description={desc[:139]}')


def once(dry=False):
    if not take_lock():
        print('another pass is running')
        return
    try:
        r = verify_pr.repo()
        master = gh_json(f'repos/{r}/branches/master')['commit']['sha']
        prs = gh_json(f'repos/{r}/pulls?state=open&base=master&per_page=100') or []
        todo = 0
        for p in sorted(prs, key=lambda p: p['number']):
            n, head = p['number'], p['head']['sha']
            if p.get('draft'):
                continue
            st = gh_json(f'repos/{r}/commits/{head}/status')
            mine = [s for s in st.get('statuses', []) if s['context'] == verify_pr.CONTEXT]
            rec_path = os.path.join(OUT, f'pr{n}.json')
            rec = json.load(open(rec_path, encoding='utf-8')) if os.path.exists(rec_path) else {}
            done = mine and mine[0]['state'] in ('success', 'failure') and rec.get('head') == head
            if done and rec.get('base') == master:
                continue
            if done and rec.get('base') != master:
                why = f'master moved ({str(rec.get("base"))[:9]} -> {master[:9]})'
            else:
                why = 'new head ' + head[:9]
            maint = p.get('author_association') in MAINTAINER
            approved = maint or approved_sha(r, n, head)
            if mine and mine[0]['state'] == 'pending' and mine[0]['description'].startswith(WAITING) and not approved:
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
                             f'This pull request changes files that would run on the verifying host (see the list in '
                             f'`verify_pr.py`\'s gate, docs/github-plan.md 4.5.5). A maintainer reads them, then '
                             f'comments `/verify-approved {head[:7]}` to verify this commit.')
                log(f'#{n}: gated, waiting for /verify-approved {head[:7]}')
            else:
                log(f'#{n}: verify_pr exit {rc} (0 success, 1 failure; .work/verify/pr{n}.run.log)')
        if not todo:
            print(f'nothing to verify ({len(prs)} open PRs)')
    finally:
        if os.path.exists(LOCK):
            os.remove(LOCK)


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
    sub.add_parser('install').add_argument('--minutes', type=int, default=5)
    sub.add_parser('uninstall')
    sub.add_parser('status')
    a = ap.parse_args()
    if a.cmd == 'once':
        once(a.dry)
    elif a.cmd == 'launch':
        launch(a.ref)
    elif a.cmd == 'install':
        subprocess.run(task_command(a.minutes), check=True)
        print(f'installed "{TASK}": every {a.minutes} min while you are logged on; log {LOG}')
    elif a.cmd == 'uninstall':
        subprocess.run(['schtasks', '/Delete', '/TN', TASK, '/F'], check=True)
    else:
        subprocess.run(['schtasks', '/Query', '/TN', TASK, '/V', '/FO', 'LIST'])
        if os.path.exists(LOG):
            print(''.join(open(LOG, encoding='utf-8').readlines()[-10:]))


if __name__ == '__main__':
    main()
