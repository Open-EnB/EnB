#!/usr/bin/env python3
"""The coordinator's run branch, its draft PR and its claims (D148, docs/github-plan.md 4.5.3, P-GH-10 step 5).

    python tools/run_branch.py start [--date YYYY-MM-DD]   run/<date> from origin/master (or brought up to it),
                                                          pushed, with a draft PR to master (opened by `push`
                                                          once the branch has its first commit)
    python tools/run_branch.py claim UNIT --builder ID [--model M] [--split] [--wait S]
                                                          claim a builder's functions (tools/claims.py) and add
                                                          `Claims #N` to the run's PR; --split claims a big
                                                          file as several claims of at most 30 functions
    python tools/run_branch.py release N [--keep-line]      give claim #N back (and drop its line from the PR)
    python tools/run_branch.py push                         push the run branch (the pre-push check runs)
    python tools/run_branch.py ready --summary FILE         the PR's summary from FILE, out of draft
    python tools/run_branch.py status                       branch, PR, claims

Run start: master is verified by construction (every PR passes vc6-verify), so the run's baseline is
origin/master's imports.json. The PR opens as a draft at once: its claims stay alive while it is a draft (it is
the run's turn, docs/github-plan.md 4.5.2) and contributors see the run's work before master does. Push at each
heartbeat that brought work in; at handoff `ready` takes it out of draft, the verify service verifies it, and the
maintainer merges it. Builders never push (D148); this script is the coordinator's.
"""
import argparse
import datetime as dt
import os
import re
import subprocess
import sys
import time

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import claims as C  # noqa: E402

ROOT = os.path.normpath(os.path.join(HERE, '..'))
PENDING = os.path.join(ROOT, '.work', 'run_branch.claims')    # claims made before the run's PR exists
SUMMARY =re.compile(r'<!-- run-summary -->.*?<!-- /run-summary -->', re.S)


def git(*args, check=True):
    r = subprocess.run(['git', *args], cwd=ROOT, capture_output=True, text=True)
    if check and r.returncode:
        sys.exit(f'run_branch: git {" ".join(args)}: {r.stderr.strip()}')
    return r.stdout.strip() if check else r


def gh(*args):
    r = subprocess.run(['gh', *args], cwd=ROOT, capture_output=True, text=True, encoding='utf-8')
    if r.returncode:
        sys.exit(f'run_branch: gh {" ".join(args[:3])}: {r.stderr.strip() or r.stdout.strip()}')
    return r.stdout.strip()


def branch():
    b = git('branch', '--show-current')
    if not b.startswith('run/'):
        sys.exit(f'run_branch: the main tree is on {b!r}, not a run branch (start one: run_branch.py start)')
    return b


def run_pr(r, b):
    """The open PR of run branch B, or None."""
    for p in C.gh_all(f'repos/{r}/pulls?state=open&head={r.split("/")[0]}:{b}'):
        return p
    return None


def body_of(r, n):
    return C.gh(f'repos/{r}/pulls/{n}')['body'] or ''


def set_body(r, n, body):
    C.gh(f'repos/{r}/pulls/{n}', 'PATCH', {'body': body})


def initial_body(b):
    return (f'<!-- run-summary -->\nRun `{b}` in progress: the coordinator pushes as builders\' work comes in, '
            f'and takes this out of draft at handoff with the summary. The verify service verifies it then; the '
            f'maintainer merges it (D148).\n<!-- /run-summary -->\n\n')


def ensure_pr(r, b):
    """The run's PR, opened as a draft once the branch has a commit master lacks (GitHub refuses one before)."""
    p = run_pr(r, b)
    if p:
        print(f'PR #{p["number"]} is open for {b}' + (' (draft)' if p.get('draft') else ''))
        return p
    if git('rev-list', '--count', f'origin/master..{b}') == '0':
        print(f'{b} has no commits beyond master yet: `run_branch.py push` opens the draft PR after the first one')
        return None
    pending = open(PENDING).read().split() if os.path.exists(PENDING) else []
    url = gh('pr', 'create', '-R', r, '--draft', '--base', 'master', '--head', b, '--title', f'Run {b[4:]}',
             '--body', initial_body(b) + ''.join(f'Claims #{n}\n' for n in pending))
    print(f'draft PR: {url}' + (f' (claims {", ".join(pending)})' if pending else ''))
    if os.path.exists(PENDING):
        os.remove(PENDING)
    return run_pr(r, b)


def cmd_start(a):
    r = C.repo()
    if git('status', '--porcelain', '--untracked-files=no'):
        sys.exit('run_branch: the tree has uncommitted changes; commit or stash them first')
    git('fetch', '-q', 'origin')
    b = 'run/' + (a.date or dt.date.today().isoformat())
    if git('rev-parse', '--verify', '-q', 'refs/heads/' + b, check=False).returncode == 0:
        git('switch', '-q', b)
        m = git('merge', '--no-edit', 'origin/master', check=False)
        if m.returncode:
            sys.exit(f'run_branch: merging origin/master into {b} conflicts; resolve, commit, rerun\n{m.stdout}')
        print(f'{b}: brought up to origin/master')
    else:
        git('switch', '-q', '-c', b, 'origin/master')
        print(f'{b}: new, from origin/master {git("rev-parse", "--short", "origin/master")}')
    base = os.path.join(ROOT, '.work', f'imports.{git("rev-parse", "--short", "origin/master")}.json')
    with open(base, 'w', encoding='utf-8', newline='\n') as f:
        f.write(git('show', 'origin/master:docs/data/imports.json') + '\n')
    print(f'baseline: {os.path.relpath(base, ROOT)} (coordinator-pacing --baseline)')
    push = subprocess.run(['git', 'push', '-u', 'origin', b], cwd=ROOT)
    if push.returncode:
        sys.exit('run_branch: push failed (see the pre-push check above)')
    ensure_pr(r, b)
    live = C.live_claims(r)
    print(f'{len(live)} live claims: assign builders only unclaimed functions (python tools/claims.py list)')


def open_claim(r, unit, funcs, a):
    body = (f'### Unit\n\n{unit}\n\n### Agent\n\nClaude Code, coordinator run builder {a.builder}\n\n'
            f'### Model\n\n{a.model or "_No response_"}\n\n### Accountable person\n\nthe maintainer (run '
            f'{branch()})\n')
    issue = C.gh(f'repos/{r}/issues', 'POST', {'title': f'claim: {unit}', 'body': body})
    return issue['number']


def answer(r, n, wait=180):
    """The claim's state once the Action (or, when it does not answer, a local reconcile) has decided."""
    end = time.time() + wait
    while time.time() < end:
        time.sleep(10)
        _, st = C.read_marker(C.gh_all(f'repos/{r}/issues/{n}/comments'))
        if st:
            return st
    print('the Claims Action did not answer; reconciling here (as the maintainer)')
    C.reconcile()
    return C.read_marker(C.gh_all(f'repos/{r}/issues/{n}/comments'))[1] or {}


def cmd_claim(a):
    r, b = C.repo(), branch()
    p = run_pr(r, b) or ensure_pr(r, b)
    ix = C.Index()
    text = ' '.join(a.unit)
    if a.split:
        unit, funcs = ix.resolve(text, cap=None)
        path = unit.split(' ')[0] if '.' in unit.split(' ')[0] else ''
        chunks = [funcs[i:i + C.CAP] for i in range(0, len(funcs), C.CAP)]
        units = []
        for k, ch in enumerate(chunks):
            end = chunks[k + 1][0] if k + 1 < len(chunks) else ch[-1] + 1
            units.append(((path + ' ') if path else '') + f'0x{ch[0]:X}-0x{end:X}')
    else:
        units = [ix.resolve(text)[0]]
    granted = []
    for unit in units:
        n = open_claim(r, unit, None, a)
        st = answer(r, n, a.wait)
        if st.get('state') == C.LIVE:
            granted.append(n)
            print(f'#{n} granted: {unit} ({len(st["funcs"])} functions)')
        else:
            print(f'#{n} not granted: {unit} (see the issue)')
    if granted and p:
        body = body_of(r, p['number']).rstrip('\n') + '\n' + ''.join(f'Claims #{n}\n' for n in granted)
        set_body(r, p['number'], body)
        print(f'PR #{p["number"]}: ' + ', '.join(f'Claims #{n}' for n in granted))
    elif granted:
        with open(PENDING, 'a', newline='\n') as f:
            f.write(''.join(f'{n}\n' for n in granted))
        print(f'no PR yet: Claims #{", #".join(map(str, granted))} go into it when `push` opens it '
              f'(within {C.NOPR_H} h, or the claims expire)')
    return 0 if len(granted) == len(units) else 1


def cmd_release(a):
    r = C.repo()
    C.gh(f'repos/{r}/issues/{a.number}/comments', 'POST', {'body': '/release'})
    print(f'#{a.number}: /release commented')
    if not a.keep_line:
        p = run_pr(r, branch())
        if p:
            body = body_of(r, p['number'])
            new = re.sub(rf'(?im)^claims?\s+#{a.number}\s*\n?', '', body)
            if new != body:
                set_body(r, p['number'], new)
                print(f'PR #{p["number"]}: Claims #{a.number} removed')


def cmd_push(a):
    b = branch()
    if subprocess.run(['git', 'push', 'origin', b], cwd=ROOT).returncode:
        sys.exit('run_branch: push failed (see the pre-push check above)')
    ensure_pr(C.repo(), b)


def cmd_ready(a):
    r, b = C.repo(), branch()
    p = run_pr(r, b)
    if not p:
        sys.exit(f'run_branch: {b} has no open PR')
    summary = open(a.summary, encoding='utf-8').read().strip()
    body = body_of(r, p['number'])
    block = f'<!-- run-summary -->\n{summary}\n<!-- /run-summary -->'
    body = SUMMARY.sub(lambda _: block, body) if SUMMARY.search(body) else block + '\n\n' + body
    set_body(r, p['number'], body)
    cmd_push(a)
    if p.get('draft'):
        gh('pr', 'ready', str(p['number']), '-R', r)
    print(f'PR #{p["number"]} ready for review; the verify service verifies it within minutes')


def cmd_status(a):
    r = C.repo()
    b = git('branch', '--show-current')
    p = run_pr(r, b) if b.startswith('run/') else None
    ahead = git('rev-list', '--count', f'origin/{b}..{b}', check=False)
    print(f'branch {b}; unpushed commits: {ahead.stdout.strip() if ahead.returncode == 0 else "? (not pushed yet)"}')
    if p:
        mine = {int(x) for x in C.CLAIMS_REF.findall(p.get('body') or '')}
        print(f'PR #{p["number"]}' + (' (draft)' if p.get('draft') else '') + f': claims {sorted(mine)}')
    for n, st in C.live_claims(r):
        print(f'  #{n} @{st["holder"]} {st["unit"]} ({len(st["funcs"])} functions)')


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = ap.add_subparsers(dest='cmd', required=True)
    sub.add_parser('start').add_argument('--date')
    p = sub.add_parser('claim')
    p.add_argument('unit', nargs='+')
    p.add_argument('--builder', required=True)
    p.add_argument('--model', default='')
    p.add_argument('--split', action='store_true')
    p.add_argument('--wait', type=int, default=180, help='seconds to wait for the Claims Action before reconciling here')
    p = sub.add_parser('release')
    p.add_argument('number', type=int)
    p.add_argument('--keep-line', action='store_true')
    sub.add_parser('push')
    sub.add_parser('ready').add_argument('--summary', required=True)
    sub.add_parser('status')
    a = ap.parse_args()
    return {'start': cmd_start, 'claim': cmd_claim, 'release': cmd_release, 'push': cmd_push,
            'ready': cmd_ready, 'status': cmd_status}[a.cmd](a) or 0


if __name__ == '__main__':
    sys.exit(main())
