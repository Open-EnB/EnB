"""Verify a pull request byte for byte before it reaches master (docs/github-plan.md 4.5.5, P-GH-10 step 2).

    python tools/vc6/verify_pr.py PR [--pool K] [--approved] [--no-post] [--keep] [--lib X ...]

1. Fetches master, the PR's head and GitHub's test merge (`pull/PR/merge`: master with the PR merged) as
   data: nothing of the PR is checked out in this tree.
2. Gate: every file the PR changes must be one a verify only reads or sends to a build VM's compiler
   (C/C++ sources and headers outside tools/ and .github/, documentation, docs/data). Anything else (Python,
   scripts, workflows, importer.py's patch tables, .def files...) would run on this host: the run is refused
   until the maintainer has read the PR and passes --approved (the label `verify-approved` once the runner
   does this, step 3).
3. A detached worktree .work/wt/prPR.vf at the test merge (wt.py's furnish: the shared lock, pool and hosts),
   where the libraries the PR touches (wt.py's libs_touched: their directories, EA headers -> app too,
   importer.py lines naming a library, imports.json blocks that changed; plus --lib) are verified on K pool VMs
   (`importer.py verify`), then `importer.py replay` (the tree equals what importer.py's records produce).
4. Pairs: the verified imports.json against master's record (lost, and of those the D66 moves and the lost
   outright) and against the PR's own record (equal: the PR records what the verify gives). The `// NOT EXACT`
   markers in the sources before and after.
5. Verdict `success` only when nothing is lost, the record equals the verify and replay is clean; posted as
   the commit status `vc6-verify` on the PR's head and as one PR comment (updated in place on reruns), and
   saved to .work/verify/prPR.json. --no-post prints only.

The tools run are this checkout's own (and, through the gate, the PR's equal them). The PR's C++ reaches only
a build VM's compiler.
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

ROOT = wt.ROOT
OUT = os.path.join(ROOT, '.work', 'verify')
CONTEXT = 'vc6-verify'
MARK = '<!-- enb-verify -->'
# what a verify only reads, or sends to a VM's compiler
READ_ONLY = [re.compile(p, re.I) for p in (
    r'^(?!tools/|\.github/).*\.(cpp|c|cc|h|hpp|inl)$',
    r'^.*\.md$',
    r'^docs/data/',
    r'^tests/.*\.(ini|txt)$',
)]


def gh(*args):
    r = subprocess.run(['gh', *args], cwd=ROOT, capture_output=True, text=True, encoding='utf-8')
    if r.returncode:
        sys.exit(f'verify_pr: gh {" ".join(args[:3])}: {r.stderr.strip() or r.stdout.strip()}')
    return r.stdout


def repo():
    return os.environ.get('GITHUB_REPOSITORY') or gh('repo', 'view', '--json', 'nameWithOwner', '-q',
                                                     '.nameWithOwner').strip()


def needs_approval(paths):
    return [p for p in paths if not any(rx.search(p) for rx in READ_ONLY)]


def not_exact_count(rev):
    """`// NOT EXACT` markers in the C/C++ sources at REV (D64)."""
    r = wt.git('grep', '-c', '// NOT EXACT', rev, '--', '*.cpp', '*.c', '*.h', '*.inl', check=False)
    return sum(int(l.rsplit(':', 1)[1]) for l in r.stdout.decode(errors='replace').splitlines() if ':' in l)


def moves_and_outright(lost, verified):
    holders = {}
    for lib, f, a in verified:
        holders.setdefault(a, []).append((lib, f))
    moves = [(lib, f, holders[a], a) for lib, f, a in lost if a in holders]
    outright = sorted({a for _, _, a in lost if a not in holders})
    return moves, outright


def post(r, pr, res):
    state = 'success' if res['verdict'] == 'success' else 'failure'
    desc = res['summary'][:139]
    gh('api', '-X', 'POST', f'repos/{r}/statuses/{pr["head"]}', '-f', f'state={state}', '-f', f'context={CONTEXT}',
       '-f', f'description={desc}')
    body = MARK + '\n' + res['report']
    me = gh('api', 'user', '-q', '.login').strip()
    mine = [c for c in json.loads(gh('api', f'repos/{r}/issues/{pr["number"]}/comments?per_page=100'))
            if c['user']['login'] == me and (c.get('body') or '').startswith(MARK)]
    if mine:
        gh('api', '-X', 'PATCH', f'repos/{r}/issues/comments/{mine[-1]["id"]}', '-f', f'body={body}')
    else:
        gh('api', '-X', 'POST', f'repos/{r}/issues/{pr["number"]}/comments', '-f', f'body={body}')


def report(pr, res):
    lines = [f'### {CONTEXT}: {res["verdict"]}', '', res['summary'], '',
             f'Verified GitHub\'s test merge `{res["merge"][:9]}` (master `{res["base"][:9]}` + head '
             f'`{pr["head"][:9]}`) on {res["when"]}.', '']
    if res.get('libs'):
        lines.append('Libraries verified: ' + ', '.join(f'`{x}`' for x in res['libs']) + '.')
    if res.get('reasons'):
        lines += ['', '**Why not success:**'] + [f'* {x}' for x in res['reasons']]
    for key, title in (('gained', 'Gained'), ('lost', 'Lost')):
        rows = res.get(key) or []
        if rows:
            lines += ['', f'<details><summary>{title}: {len(rows)}</summary>', '', '```']
            lines += [f'{lib} {f} {a}' for lib, f, a in rows[:400]]
            lines += ['```', '</details>']
    if res.get('moves'):
        lines += ['', f'D66 moves (a lost pair whose address another file holds): {len(res["moves"])}; '
                      f'lost outright: {len(res["outright"])}.']
    lines += ['', '_`python tools/vc6/verify_pr.py` on the maintainer\'s host (docs/github-plan.md 4.5.5)._']
    return '\n'.join(lines)


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('pr', type=int)
    ap.add_argument('--pool', type=int, default=2)
    ap.add_argument('--approved', action='store_true', help='the maintainer has read the PR\'s non-C++ changes')
    ap.add_argument('--no-post', action='store_true')
    ap.add_argument('--keep', action='store_true', help='keep the worktree')
    ap.add_argument('--lib', action='append', default=[])
    ap.add_argument('--no-replay', action='store_true')
    a = ap.parse_args()

    r = repo()
    info = json.loads(gh('pr', 'view', str(a.pr), '-R', r, '--json',
                         'number,headRefOid,baseRefName,mergeable,state,author,isCrossRepository,labels'))
    pr = {'number': a.pr, 'head': info['headRefOid'], 'author': info['author']['login']}
    if info['state'] != 'OPEN':
        sys.exit(f'verify_pr: PR #{a.pr} is {info["state"].lower()}')
    if info['baseRefName'] != 'master':
        sys.exit(f'verify_pr: PR #{a.pr} targets {info["baseRefName"]}, not master')
    os.makedirs(OUT, exist_ok=True)
    res = {'pr': a.pr, 'head': pr['head'], 'when': dt.datetime.now(dt.timezone.utc).strftime('%Y-%m-%d %H:%M UTC'),
           'reasons': []}

    def finish(verdict, summary):
        res['verdict'], res['summary'] = verdict, summary
        res['report'] = report(pr, res)
        with open(os.path.join(OUT, f'pr{a.pr}.json'), 'w', encoding='utf-8', newline='\n') as f:
            json.dump(res, f, indent=1, default=list)
        print(res['report'])
        if not a.no_post:
            post(r, pr, res)
            print(f'\nposted {CONTEXT}={verdict} on {pr["head"][:9]} and the PR comment')
        sys.exit(0 if verdict == 'success' else 1)

    pre = f'refs/enb/pr/{a.pr}'
    fetch = wt.git('fetch', '-q', 'origin', '+refs/heads/master:refs/remotes/origin/master',
                   f'+refs/pull/{a.pr}/head:{pre}/head', f'+refs/pull/{a.pr}/merge:{pre}/merge', check=False)
    if fetch.returncode:
        if f'refs/pull/{a.pr}/merge' in fetch.stderr.decode(errors='replace'):
            res.update(merge='-', base=wt.rev('refs/remotes/origin/master') if wt.ok('rev-parse', 'origin/master') else '-')
            res['reasons'].append('GitHub has no test merge: the PR conflicts with master; rebase it')
            finish('failure', 'conflicts with master: rebase the PR')
        sys.exit(f'verify_pr: git fetch: {fetch.stderr.decode(errors="replace").strip()}')
    head, merge = wt.rev(f'{pre}/head'), wt.rev(f'{pre}/merge')
    if head != pr['head']:
        sys.exit(f'verify_pr: the PR moved while fetching ({pr["head"][:9]} -> {head[:9]}); rerun')
    old = wt.rev(merge + '^1')
    res.update(merge=merge, base=old)
    if old != wt.rev('refs/remotes/origin/master'):
        print(f'note: GitHub\'s test merge is on master {old[:9]}; master is now '
              f'{wt.rev("refs/remotes/origin/master")[:9]} (GitHub recomputes it shortly)')

    paths = wt.git('diff', '--name-only', old, merge).splitlines()
    gated = needs_approval(paths)
    if gated and not a.approved:
        print('these files would run on this host or change how the verify runs:\n  ' + '\n  '.join(gated))
        sys.exit('verify_pr: read the PR, then rerun with --approved (nothing was verified or posted)')

    vf = wt.wt_path(f'pr{a.pr}') + '.vf'
    if os.path.isdir(vf):
        wt.drop_worktree(vf)
    wt.git('worktree', 'add', '--detach', vf, merge)
    wt.furnish(vf, libs=False, pseudo=False)
    try:
        libs = wt.libs_touched(old, merge, a.lib)
        res['libs'] = libs
        before = wt.pairs_of(wt.blob(f'{old}:{wt.IMPORTS}'))
        cand = wt.pairs_of(wt.blob(f'{merge}:{wt.IMPORTS}'))
        verified = cand
        if libs:
            print(f'verify --lib {" ".join(libs)} --pool {a.pool} in {vf} (log {vf}.verify.log)', flush=True)
            args = [sys.executable, 'tools/vc6/importer.py', 'verify', '--pool', str(a.pool)]
            for lib in libs:
                args += ['--lib', lib]
            rc, tail = wt.run_logged(args, vf, vf + '.verify.log')
            if rc:
                res['reasons'].append(f'importer.py verify failed ({rc}): {" / ".join(tail)}')
                finish('failure', 'verify failed (compile errors?); see the log on the maintainer\'s host')
            with open(os.path.join(vf, *wt.IMPORTS.split('/')), encoding='utf-8') as f:
                verified = wt.pairs_of(f.read())
            if not a.no_replay:
                print('replay', flush=True)
                rc, tail = wt.run_logged([sys.executable, 'tools/vc6/importer.py', 'replay'], vf, vf + '.replay.log')
                if rc or not any(': 0 problems, 0 files differ' in t for t in tail):
                    res['reasons'].append('importer.py replay differs: a library file changed outside importer.py\'s '
                                          f'records ({" / ".join(tail)})')
        else:
            print('no library touched: nothing to compile')
        gained, lost = sorted(verified - before), sorted(before - verified)
        moves, outright = moves_and_outright(lost, verified)
        res.update(gained=gained, lost=lost, moves=moves, outright=outright)
        ne_before, ne_after = not_exact_count(old), not_exact_count(merge)
        if lost:
            res['reasons'].append(f'{len(lost)} pair(s) lost against master ({len(moves)} D66 moves, '
                                  f'{len(outright)} lost outright): the maintainer decides (D66)')
        if libs and verified != cand:
            res['reasons'].append(f'the PR\'s imports.json records other pairs than the verify gives (verify only '
                                  f'{len(verified - cand)}, record only {len(cand - verified)}): commit the verified '
                                  'record')
        summary = (f'{len(gained):+d} pairs, {len(lost)} lost, NOT EXACT {ne_before} -> {ne_after}'
                   + (f'; libraries {", ".join(libs)}' if libs else '; no library touched'))
        finish('failure' if res['reasons'] else 'success', summary)
    finally:
        if not a.keep and os.path.isdir(vf):
            wt.drop_worktree(vf)


if __name__ == '__main__':
    main()
