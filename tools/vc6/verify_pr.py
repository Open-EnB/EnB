"""Verify a pull request byte for byte before it reaches master (docs/github-plan.md 4.5.5, P-GH-10 step 2).

    python tools/vc6/verify_pr.py PR [--pool K] [--approved] [--no-post] [--keep] [--lib X ...]

1. Fetches master, the PR's head and GitHub's test merge (`pull/PR/merge`: master with the PR merged) as
   data: nothing of the PR is checked out in this tree. No test merge and not conflicting: GitHub hasn't built it
   yet (a PR opened moments ago), so the status is pending and exit NO_MERGE. A conflicting PR (GitHub's mergeable
   CONFLICTING; its `pull/PR/merge` is then missing or the last one, on an older master) fails with "rebase",
   unless it is a contributor's and the conflicts are confined to the generated documents (GENERATED: names.py's
   two): then the verify builds its own merge (git merge-tree, no checkout) and goes on, since step 3 rewrites
   those files anyway.
2. Gate: every file the PR changes must be one a verify only reads or sends to a build VM's compiler
   (C/C++ sources and headers outside tools/ and .github/, documentation, docs/data). Anything else (Python,
   scripts, workflows, libdefs.py's library table, .def files...) would run on this host: the run is refused
   until the maintainer has read the PR and passes --approved (the label `verify-approved` once the runner
   does this, step 3).
3. A detached worktree .work/wt/prPR.vf at the test merge (wt.py's furnish: the shared lock, pool and hosts),
   where the libraries the PR touches (wt.py's libs_touched: their directories, EA headers -> app too,
   libdefs.py / importer.py lines naming a library, ledger shards that changed; plus --lib) are verified on K
   pool VMs (`verify.py`; no replay since the freeze, D211: the libraries are plain source). For a contributor's
   PR the generated documents are then rewritten there (`tools/re/names.py`), so the landing carries them as the
   merged sources give them.
4. Pairs: the verified ledger (docs/data/imports, ledger.py) against master's record (lost, and of those the D66 moves and the lost
   outright) and against the PR's own record (equal: the PR records what the verify gives). The `// NOT EXACT`
   markers in the sources before and after.
5. Verdict `success` only when nothing is lost (or every loss is a D66 move, its address held by another file,
   and a maintainer accepted them: a comment `/accept-d66 SHA` naming the head, or --accept-d66) and, for a maintainer's PR (merged as it is),
   the record equals the verify; a contributor's PR lands through verify_watch.py, which carries the files the
   verify rewrote (.work/verify/prPR.files/), so a contributor without VC6 need not commit them. Posted as
   the commit status `vc6-verify` on the PR's head and as one PR comment (updated in place on reruns), and
   saved to .work/verify/prPR.json. --no-post prints only.

A lost pair that docs/data/accepted-corrections.json lists (a false pair the user ruled on, D173), whose
`moved_to` the same file gains in this verify (or whose `moved_to` is null: a decided loss, D175), is not a loss: it
is named in the status and the comment as corrected. The list is read from this checkout (master's), never from the PR.

This checkout must be origin/master after the fetch, with no local change under tools/ or to the corrections list
(normally .work/wt/verifier, which verify_watch.py moves there); anywhere else it refuses, except with --no-post,
which prints and posts nothing.

The tools run are this checkout's own (and, through the gate, the PR's equal them). The PR's C++ reaches only
a build VM's compiler.
"""
import argparse
import datetime as dt
import json
import os
import re
import shutil
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import wt  # noqa: E402
import ledger  # noqa: E402
import pairs_cmp  # noqa: E402

ROOT = wt.ROOT
SELF = os.path.normpath(os.path.join(HERE, '..', '..'))     # this script's own checkout (pairs_cmp.ROOT)
assert os.path.normcase(SELF) == os.path.normcase(pairs_cmp.ROOT)
OUT = os.path.join(ROOT, '.work', 'verify')
CONTEXT = 'vc6-verify'
GATED = 3                       # exit status: the gate stopped it (verify_watch.py asks for approval)
NO_MERGE = 4                    # exit status: GitHub's test merge isn't built yet (pending; the next pass retries)
NO_MERGE_DESC = 'GitHub has not built the test merge yet: the next pass verifies it'
MARK = '<!-- enb-verify -->'
MAINTAINER = {'OWNER', 'MEMBER'}
# The generated documents a PR commits (CONTRIBUTING.md) and every landing on master rewrites: tools/re/names.py
# writes both from the sources. A conflict confined to them is not the contributor's to solve (PR #226, 2026-11-20):
# the verify merges the PR itself and regenerates them, and the landing carries the regenerated files.
GENERATED = ('docs/data/names-review.md', 'docs/data/not-exact.md')
# a maintainer accepts a PR's D66 moves (every lost address held by another file) for one head
ACCEPT = re.compile(r'^/accept-d66\s+([0-9a-f]{7,40})\b', re.I | re.M)
# what a verify only reads, or sends to a VM's compiler
READ_ONLY = [re.compile(p, re.I) for p in (
    r'^(?!tools/|\.github/).*\.(cpp|c|cc|h|hpp|inl)$',
    r'^.*\.md$',
    r'^docs/data/',
    r'^tests/.*\.(ini|txt)$',
    r'^docs/attempts/.*\.(txt|diff|patch)$',     # an attempt's stash (attempts.csv's column; PR #228); its .py stay gated
)]


def rewritten_files(vf):
    """(rewritten, removed): the tracked files the verify changed in vf, plus new ledger shards; the shards it
    deleted (a file the state no longer has)."""
    rewritten, removed = [], []
    for line in wt.git('status', '--porcelain', '--untracked-files=all', '--no-renames', cwd=vf).splitlines():
        code, p = line[:2], line[3:].strip().strip('"')
        if not p:
            continue
        if code == '??':
            if p.startswith(wt.IMPORTS + '/'):
                rewritten.append(p)
        elif 'D' in code:
            removed.append(p)
        else:
            rewritten.append(p)
    return sorted(rewritten), sorted(removed)


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


def conflict_split(conflicted):
    """(generated, beyond): a merge's conflicted paths, those in GENERATED and the rest."""
    conflicted = sorted({p.strip().replace('\\', '/') for p in conflicted if p.strip()})
    return [p for p in conflicted if p in GENERATED], [p for p in conflicted if p not in GENERATED]


def own_merge(master, head, pr):
    """(commit, generated, beyond): this service's own merge of HEAD into MASTER, for a PR GitHub calls
    conflicting (it then keeps the last test merge, on an older master, or has none). Built in the object
    store only (merge-tree: no checkout). A conflict in a file outside GENERATED gives no commit (None) and
    names it in BEYOND; the GENERATED files it holds with conflict markers are rewritten by regenerate()."""
    r = wt.git('merge-tree', '--write-tree', '--name-only', '--no-messages', master, head, check=False)
    out = r.stdout.decode('utf-8', 'surrogateescape').splitlines()
    if r.returncode not in (0, 1) or not out:
        sys.exit(f'verify_pr: git merge-tree: {r.stderr.decode(errors="replace").strip()}')
    generated, beyond = conflict_split(out[1:])
    if beyond:
        return None, generated, beyond
    msg = f'Merge #{pr} ({head[:9]}) into master {master[:9]} (verify_pr.py: conflicts only in generated documents)\n'
    env = dict(os.environ, GIT_AUTHOR_NAME='verify_pr', GIT_AUTHOR_EMAIL='verify_pr@localhost',
               GIT_COMMITTER_NAME='verify_pr', GIT_COMMITTER_EMAIL='verify_pr@localhost')
    commit = wt.git('commit-tree', out[0].strip(), '-p', master, '-p', head, '-m', msg, env=env).strip()
    return commit, generated, beyond


def regenerate(vf):
    """Rewrite GENERATED in the worktree vf from its sources (its own names.py: master's, through the gate).
    Why it failed, or None."""
    rc, tail = wt.run_logged([sys.executable, os.path.join('tools', 're', 'names.py')], vf, vf + '.names.log')
    if rc:
        return f'names.py failed ({rc}): {" / ".join(tail)}'
    for p in GENERATED:
        with open(os.path.join(vf, *p.split('/')), encoding='utf-8', errors='replace') as f:
            if any(l.startswith(('<<<<<<< ', '>>>>>>> ')) for l in f):
                return f'{p} still holds conflict markers after names.py'
    return None


def off_master(head, master, dirty):
    """Why this checkout cannot judge a PR (None when it can): its HEAD must be origin/master, with no local change
    to the tools or the accepted corrections, since those decide the verdict (2026-11-03: a run from a run branch's
    checkout read a corrections list without the merged D189 entries and failed PR #147)."""
    if head != master:
        return f'this checkout is at {head[:9]}, not origin/master {master[:9]}'
    if dirty:
        return 'this checkout has local changes: ' + ', '.join(dirty)
    return None


def moves_and_outright(lost, verified):
    holders = {}
    for lib, f, a in verified:
        holders.setdefault(a, []).append((lib, f))
    moves = [(lib, f, holders[a], a) for lib, f, a in lost if a in holders]
    outright = sorted({a for _, _, a in lost if a not in holders})
    return moves, outright


def accepted_by(comments, head):
    """The first maintainer comment whose `/accept-d66 SHA` names HEAD (at least 7 hex digits), or None."""
    for c in comments:
        if (c.get('body') or '').startswith(MARK):
            continue                            # this service's own report quotes the command
        if c.get('author_association') in MAINTAINER and any(
                head.lower().startswith(sha.lower()) for sha in ACCEPT.findall(c.get('body') or '')):
            return c
    return None


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
    if res.get('own_merge') is not None:
        what = (f'Verified the verify service\'s own merge `{res["merge"][:9]}` (master `{res["base"][:9]}` + head '
                f'`{pr["head"][:9]}`) on {res["when"]}: the pull request conflicts with master only in generated '
                f'documents (' + ', '.join(f'`{p}`' for p in res['own_merge']) + '), which the landing regenerates. '
                'No rebase is needed, whatever GitHub\'s merge box says.')
    elif res['merge'] == '-':
        what = f'Not verified: head `{pr["head"][:9]}` does not merge into master `{res["base"][:9]}` ({res["when"]}).'
    else:
        what = (f'Verified GitHub\'s test merge `{res["merge"][:9]}` (master `{res["base"][:9]}` + head '
                f'`{pr["head"][:9]}`) on {res["when"]}.')
    lines = [f'### {CONTEXT}: {res["verdict"]}', '', res['summary'], '', what, '']
    if res.get('libs'):
        lines.append('Libraries verified: ' + ', '.join(f'`{x}`' for x in res['libs']) + '.')
    if res.get('reasons'):
        lines += ['', '**Why not success:**'] + [f'* {x}' for x in res['reasons']]
    if res.get('notes'):
        lines += [''] + [f'Note: {x}.' for x in res['notes']]
    for key, title in (('gained', 'Gained'), ('lost', 'Lost')):
        rows = res.get(key) or []
        if rows:
            lines += ['', f'<details><summary>{title}: {len(rows)}</summary>', '', '```']
            lines += [f'{lib} {f} {a}' for lib, f, a in rows[:400]]
            lines += ['```', '</details>']
    if res.get('corrected'):
        lines += ['', 'Accepted corrections (docs/data/accepted-corrections.json; not losses):']
        lines += [f'* {lib} {f} {a} ' + (f'-> {to}' if to else '(decided loss)') + f' (accepted: {dec})'
                  for lib, f, a, to, dec in res['corrected']]
    if res.get('moves'):
        lines += ['', f'D66 moves (a lost pair whose address another file holds): {len(res["moves"])}; '
                      f'lost outright: {len(res["outright"])}.']
    if res.get('accepted'):
        acc = res['accepted']
        lines += ['', f'D66 moves accepted by {acc["by"]} for `{acc["sha"]}`'
                      + (f' ({acc["url"]})' if acc.get('url') else '') + '.']
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
    ap.add_argument('--no-replay', action='store_true', help='ignored: no replay since the freeze (D211)')
    ap.add_argument('--accept-d66', action='store_true',
                    help='the maintainer accepts the PR\'s D66 moves (as a comment `/accept-d66 SHA` does)')
    a = ap.parse_args()

    r = repo()
    info = json.loads(gh('pr', 'view', str(a.pr), '-R', r, '--json',
                         'number,headRefOid,baseRefName,mergeable,state,author,isCrossRepository,labels'))
    pr = {'number': a.pr, 'head': info['headRefOid'], 'author': info['author']['login']}
    assoc = json.loads(gh('api', f'repos/{r}/pulls/{a.pr}')).get('author_association')
    maint = assoc in ('OWNER', 'MEMBER')
    if info['state'] != 'OPEN':
        sys.exit(f'verify_pr: PR #{a.pr} is {info["state"].lower()}')
    if info['baseRefName'] != 'master':
        sys.exit(f'verify_pr: PR #{a.pr} targets {info["baseRefName"]}, not master')
    os.makedirs(OUT, exist_ok=True)
    res = {'pr': a.pr, 'head': pr['head'], 'when': dt.datetime.now(dt.timezone.utc).strftime('%Y-%m-%d %H:%M UTC'),
           'reasons': []}
    rec = f'pr{a.pr}'                  # pr{N}.test off master: never the record verify_watch.py lands from

    def finish(verdict, summary):
        res['verdict'], res['summary'] = verdict, summary
        res['report'] = report(pr, res)
        with open(os.path.join(OUT, f'{rec}.json'), 'w', encoding='utf-8', newline='\n') as f:
            json.dump(res, f, indent=1, default=list)
        print(res['report'])
        if not a.no_post:
            post(r, pr, res)
            print(f'\nposted {CONTEXT}={verdict} on {pr["head"][:9]} and the PR comment')
        sys.exit(0 if verdict == 'success' else 1)

    pre = f'refs/enb/pr/{a.pr}'
    fetch = wt.git('fetch', '-q', 'origin', '+refs/heads/master:refs/remotes/origin/master',
                   f'+refs/pull/{a.pr}/head:{pre}/head', check=False)
    if fetch.returncode:
        sys.exit(f'verify_pr: git fetch: {fetch.stderr.decode(errors="replace").strip()}')
    # the test merge by itself: GitHub has none for a PR that conflicted from its first push, none yet for a PR
    # opened moments ago, and for a PR that began to conflict when master moved it keeps the last one, on the old
    # master (PR #226: that one was verified again on every pass, and its base never became master's)
    conflicting = info['mergeable'] == 'CONFLICTING'
    fetch = wt.git('fetch', '-q', 'origin', f'+refs/pull/{a.pr}/merge:{pre}/merge', check=False)
    if fetch.returncode and not conflicting:
        if f'refs/pull/{a.pr}/merge' in fetch.stderr.decode(errors='replace'):
            # GitHub has not built the test merge yet (a PR opened or pushed moments ago: mergeable UNKNOWN)
            print(f'GitHub has not built the test merge of #{a.pr} yet (mergeable {info["mergeable"]}): pending')
            if not a.no_post:
                gh('api', '-X', 'POST', f'repos/{r}/statuses/{pr["head"]}', '-f', 'state=pending',
                   '-f', f'context={CONTEXT}', '-f', f'description={NO_MERGE_DESC}')
            sys.exit(NO_MERGE)
        sys.exit(f'verify_pr: git fetch: {fetch.stderr.decode(errors="replace").strip()}')
    # SELF, not wt.ROOT: wt.ROOT is always the main tree (git's common dir), while the tools and the corrections list
    # (pairs_cmp.ROOT) that judge the PR are this script's own checkout, normally .work/wt/verifier (2026-11-03: the
    # first version checked wt.ROOT and refused PR #150 because the main tree was on a branch)
    master = wt.rev('refs/remotes/origin/master')
    dirty = [l[3:] for l in wt.git('status', '--porcelain', '--untracked-files=no', '--', 'tools',
                                   'docs/data/accepted-corrections.json', cwd=SELF).splitlines() if l.strip()]
    why = off_master(wt.rev('HEAD', cwd=SELF), master, dirty)
    if why and not a.no_post:
        sys.exit(f'verify_pr: {why}. The verdict comes from this checkout\'s tools and accepted corrections, so it '
                 f'runs only from master: .work/wt/verifier (verify_watch.py launch moves it there). --no-post runs '
                 f'it here anyway and prints only (testing a tools change).')
    if why:
        rec = f'pr{a.pr}.test'
        print(f'note: {why}; --no-post: printing only (record .work/verify/{rec}.json)')
    head = wt.rev(f'{pre}/head')
    if head != pr['head']:
        sys.exit(f'verify_pr: the PR moved while fetching ({pr["head"][:9]} -> {head[:9]}); rerun')
    if conflicting:
        res.update(merge='-', base=master)
        if maint:                       # merged as it is, so GitHub must be able to merge it
            res['reasons'].append('the PR conflicts with master; rebase it')
            finish('failure', 'conflicts with master: rebase the PR')
        merge, generated, beyond = own_merge(master, head, a.pr)
        if beyond:
            res['reasons'].append('the PR conflicts with master in ' + ', '.join(f'`{p}`' for p in beyond[:10])
                                  + (f' and {len(beyond) - 10} more' if len(beyond) > 10 else '') + '; rebase it')
            finish('failure', 'conflicts with master: rebase the PR')
        wt.git('update-ref', f'{pre}/merge', merge)         # kept from gc: verify_watch.py lands from it
        res['own_merge'] = generated
        print(f'the PR conflicts with master only in generated documents ({", ".join(generated) or "none now"}): '
              f'verifying this service\'s own merge {merge[:9]}')
    else:
        merge = wt.rev(f'{pre}/merge')
    old = wt.rev(merge + '^1')
    res.update(merge=merge, base=old)
    if old != wt.rev('refs/remotes/origin/master'):
        print(f'note: GitHub\'s test merge is on master {old[:9]}; master is now '
              f'{wt.rev("refs/remotes/origin/master")[:9]} (GitHub recomputes it shortly)')

    paths = wt.git('diff', '--name-only', old, merge).splitlines()
    gated = needs_approval(paths)
    if gated and not a.approved:
        print('these files would run on this host or change how the verify runs:\n  ' + '\n  '.join(gated))
        print('verify_pr: read the PR, then rerun with --approved (nothing was verified or posted)')
        sys.exit(GATED)

    vf = wt.wt_path(f'pr{a.pr}') + '.vf'
    if os.path.isdir(vf):
        wt.drop_worktree(vf)
    wt.git('worktree', 'add', '--detach', vf, merge)
    wt.furnish(vf, libs=False, pseudo=False)
    try:
        libs = wt.libs_touched(old, merge, a.lib)
        res['libs'] = libs
        before = wt.pairs_of(ledger.load(old, repo=ROOT))
        cand = wt.pairs_of(ledger.load(merge, repo=ROOT))
        verified = cand
        if libs:
            print(f'verify --lib {" ".join(libs)} --pool {a.pool} in {vf} (log {vf}.verify.log)', flush=True)
            args = [sys.executable, *wt.tool(vf, 'verify.py', 'verify'), '--pool', str(a.pool)]
            for lib in libs:
                args += ['--lib', lib]
            rc, tail = wt.run_logged(args, vf, vf + '.verify.log')
            if rc:
                res['reasons'].append(f'verify.py failed ({rc}): {" / ".join(tail)}')
                finish('failure', 'verify failed (compile errors?); see the log on the maintainer\'s host')
            verified = wt.pairs_of(ledger.load(None, repo=vf))
        else:
            print('no library touched: nothing to compile')
        if not maint:
            # a contributor's landing carries the generated documents as master's tools write them from the merged
            # sources, whatever the PR committed (and whatever a conflict left in them)
            why = regenerate(vf)
            if why:
                res['reasons'].append(why)
                finish('failure', 'the generated documents could not be rewritten; see the log on the maintainer\'s host')
        # the files the verify rewrote (ledger shards, imports.md, the generated documents): verify_watch.py lands
        # them with the PR
        keep = os.path.join(OUT, f'{rec}.files')
        if os.path.isdir(keep):
            shutil.rmtree(keep)
        rewritten, removed = rewritten_files(vf)
        for p in rewritten:
            dst = os.path.join(keep, *p.split('/'))
            os.makedirs(os.path.dirname(dst), exist_ok=True)
            shutil.copyfile(os.path.join(vf, *p.split('/')), dst)
        res['rewritten'] = rewritten
        res['removed'] = removed
        gained, lost = sorted(verified - before), sorted(before - verified)
        corrected, lost = pairs_cmp.split_corrections(lost, gained, pairs_cmp.load_corrections())
        pad_held, lost = pairs_cmp.split_pad_held(lost)       # D279
        moves, outright = moves_and_outright(lost, verified)
        res.update(gained=gained, lost=lost, moves=moves, outright=outright, pad_held=pad_held,
                   corrected=[(lib, f, a, e['moved_to'], e['decision']) for e, (lib, f, a) in corrected])
        ne_before, ne_after = not_exact_count(old), not_exact_count(merge)
        if lost and not outright:
            if a.accept_d66:
                res['accepted'] = {'by': gh('api', 'user', '-q', '.login').strip(), 'sha': head[:9]}
            else:
                c = accepted_by(json.loads(gh('api', f'repos/{r}/issues/{a.pr}/comments?per_page=100')), head)
                if c:
                    res['accepted'] = {'by': c['user']['login'], 'sha': head[:9], 'url': c.get('html_url')}
        if lost and not res.get('accepted'):
            res['reasons'].append(f'{len(lost)} pair(s) lost against master ({len(moves)} D66 moves, '
                                  f'{len(outright)} lost outright): the maintainer decides (D66)'
                                  + ('' if outright else f'; a maintainer accepts the moves with '
                                                         f'`/accept-d66 {head[:7]}`'))
        res['record_differs'] = bool(libs) and verified != cand
        if res['record_differs']:
            why = (f'the PR\'s ledger records other pairs than the verify gives (verify only '
                   f'{len(verified - cand)}, record only {len(cand - verified)})')
            if maint:
                res['reasons'].append(why + ': commit the verified record')
            else:
                res['notes'] = [why + '; the landing commit carries the verified record (no need to commit it)']
        summary = (f'{len(gained):+d} pairs, {len(lost)} lost'
                   + (f' (D66 moves, accepted by {res["accepted"]["by"]})' if res.get('accepted') else '')
                   + (f', {len(corrected)} corrected (accepted: '
                      f'{", ".join(sorted({e["decision"] for e, _ in corrected}))})' if corrected else '')
                   + f', NOT EXACT {ne_before} -> {ne_after}'
                   + (f'; libraries {", ".join(libs)}' if libs else '; no library touched'))
        finish('failure' if res['reasons'] else 'success', summary)
    finally:
        if not a.keep and os.path.isdir(vf):
            wt.drop_worktree(vf)


if __name__ == '__main__':
    main()
