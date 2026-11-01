#!/usr/bin/env python3
"""The coordinator's run branch, its draft PR and its claims (D148, docs/github-plan.md 4.5.3, P-GH-10 step 5).

    python tools/run_branch.py start [--date YYYY-MM-DD] [--co-author 'NAME <EMAIL>']
                                                          run/<date> from origin/master (or brought up to it),
                                                          pushed, with its draft PR to master opened at once (an
                                                          empty commit `Start run <date>` when the branch has none)
    python tools/run_branch.py claim UNIT --builder ID [--model M] [--split]
                                                          claim a builder's functions in the run PR's body (one
                                                          line each in its run-claims block, granted at once);
                                                          refused when any live claim holds one of them; --split
                                                          claims a big file as several claims of at most 30
    python tools/run_branch.py release UNIT|BUILDER         drop the run's claim lines for UNIT or builder ID
    python tools/run_branch.py release N [--keep-line]      (an old issue claim) comment /release on #N and drop
                                                          its `Claims #N` line
    python tools/run_branch.py push                         push the run branch (the pre-push check runs)
    python tools/run_branch.py ready --summary FILE [--co-author 'NAME <EMAIL>']
                                                          the progress card regenerated and committed
                                                          (tools/progress.py), the PR's summary from FILE, out of draft
    python tools/run_branch.py status                       branch, PR, claims
    python tools/run_branch.py cleanup [--dry]              after a run's PR merged: delete the local branches (and their
                                                          worktrees) whose every commit is in origin/master, keep
                                                          and list those with commits of their own, fast-forward
                                                          local master, list leftover remote branches

Run start: master is verified by construction (every PR passes vc6-verify), so the run's baseline is
origin/master's imports.json. The PR opens as a draft at once, so the run's claims have a home: they are lines
in its body (claims.py's run-claims block, `claim: UNIT (BUILDER, MODEL)`), live while the PR is open (draft or
not) and ended by its merge or close; no issues, no bot comments, no emails. The Claims Action reads them on
every edit of the body and refuses any contributor's claim that overlaps them. Push at heartbeats that brought
builders' work in; at handoff `ready` takes it out of draft, the verify service verifies it, and the maintainer
merges it. Builders never push (D148); this script is the coordinator's. Its GitHub calls go through claims.py's
budget floor.
"""
import argparse
import datetime as dt
import os
import re
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import claims as C  # noqa: E402

ROOT = os.path.normpath(os.path.join(HERE, '..'))
SUMMARY =re.compile(r'<!-- run-summary -->.*?<!-- /run-summary -->', re.S)


def git(*args, check=True):
    r = subprocess.run(['git', *args], cwd=ROOT, capture_output=True, text=True)
    if check and r.returncode:
        sys.exit(f'run_branch: git {" ".join(args)}: {r.stderr.strip()}')
    return r.stdout.strip() if check else r


def gh(*args):
    C.budget_tick()
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
    return C.set_run_lines(
        f'<!-- run-summary -->\nRun `{b}` in progress: the coordinator pushes as builders\' work comes in, '
        f'and takes this out of draft at handoff with the summary. The verify service verifies it then; the '
        f'maintainer merges it (D148).\n<!-- /run-summary -->\n\nThe run\'s claims (tools/run_branch.py claim; '
        f'held while this PR is open, docs/github-plan.md 4.5.3):\n', [])


def ensure_pr(r, b):
    """The run's PR, opened as a draft (the branch has a commit master lacks: `start` makes sure of it)."""
    p = run_pr(r, b)
    if p:
        print(f'PR #{p["number"]} is open for {b}' + (' (draft)' if p.get('draft') else ''))
        return p
    if git('rev-list', '--count', f'origin/master..{b}') == '0':
        sys.exit(f'run_branch: {b} has no commits beyond master, so GitHub cannot open its PR; rerun `start`')
    url = gh('pr', 'create', '-R', r, '--draft', '--base', 'master', '--head', b, '--title', f'Run {b[4:]}',
             '--body', initial_body(b))
    print(f'draft PR: {url}')
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
    if git('rev-list', '--count', f'origin/master..{b}') == '0':
        # GitHub opens no PR for a branch without commits of its own, and the claims need the PR at once
        msg = f'Start run {b[4:]}' + (f'\n\nCo-Authored-By: {a.co_author}' if a.co_author else '')
        git('commit', '-q', '--allow-empty', '-m', msg)
        print(f'{b}: empty commit `Start run {b[4:]}` (the draft PR needs one)')
    base = os.path.join(ROOT, '.work', f'imports.{git("rev-parse", "--short", "origin/master")}.json')
    with open(base, 'w', encoding='utf-8', newline='\n') as f:
        f.write(git('show', 'origin/master:docs/data/imports.json') + '\n')
    print(f'baseline: {os.path.relpath(base, ROOT)} (coordinator-pacing --baseline)')
    push = subprocess.run(['git', 'push', '-u', 'origin', b], cwd=ROOT)
    if push.returncode:
        sys.exit('run_branch: push failed (see the pre-push check above)')
    ensure_pr(r, b)
    live, doc = C.live_claims(r, fresh=True)
    print(f'{len(live)} live claims ({C.age_note(doc)}): assign builders only unclaimed functions '
          '(python tools/claims.py list)')


def split_units(ix, text, split):
    """The units to claim for TEXT: one, or with SPLIT one per CAP not-yet-exact functions."""
    if not split:
        return [ix.resolve(text)[0]]
    unit, funcs = ix.resolve(text, cap=None)
    path = unit.split(' ')[0] if '.' in unit.split(' ')[0] else ''
    chunks = [funcs[i:i + C.CAP] for i in range(0, len(funcs), C.CAP)]
    units = []
    for k, ch in enumerate(chunks):
        end = chunks[k + 1][0] if k + 1 < len(chunks) else ch[-1] + 1
        units.append(((path + ' ') if path else '') + f'0x{ch[0]:X}-0x{end:X}')
    return units


def grant(ix, units, own_lines, others, builder, model, pr_number, holder, now=None):
    """Which UNITS can join the run PR's claim lines OWN_LINES, given OTHERS (the state file's live claims of
    everyone but this PR). Returns (all lines, [(unit, why refused)], the new state-file entries)."""
    mine = C.run_claims([{'state': 'open', 'author_association': 'OWNER', 'number': pr_number,
                          'user': {'login': holder}, 'body': C.set_run_lines('', own_lines)}], ix)
    stamp = C.fmt_time(now or dt.datetime.now(dt.timezone.utc).replace(microsecond=0))
    lines, refused, added = list(own_lines), [], []
    for unit in units:
        norm, funcs = ix.resolve(unit)
        hits = C.overlap(funcs, others + mine + added)
        if hits:
            names = sorted({C.label_of(c) + (f' ({c["builder"]})' if c.get('builder') else '') for c in hits})
            refused.append((norm, 'overlaps ' + ', '.join(names)))
            continue
        lines.append((norm, builder, model))
        added.append({'kind': 'pr', 'pr': pr_number, 'holder': holder, 'unit': norm, 'builder': builder,
                      'model': model, 'funcs': [f'0x{x:X}' for x in funcs], 'updated_at': stamp})
    return lines, refused, added


def cmd_claim(a):
    r, b = C.repo(), branch()
    p = run_pr(r, b)
    if not p:
        sys.exit(f'run_branch: {b} has no open PR to hold the claims; run `run_branch.py start` first')
    ix = C.Index()
    units = split_units(ix, ' '.join(a.unit), a.split)
    live, _ = C.live_claims(r, fresh=True)
    others = [c for c in live if not (c.get('kind') == 'pr' and c.get('pr') == p['number'])]
    lines, refused, added = grant(ix, units, C.run_lines(p.get('body')), others, a.builder, a.model, p['number'],
                                  p['user']['login'])
    for unit, why in refused:
        print(f'not granted: {unit}: {why} (python tools/claims.py list)')
    if added:
        set_body(r, p['number'], C.set_run_lines(p.get('body'), lines))
        C.update_cache(add=added)
        for c in added:
            print(f'PR #{p["number"]} claims {c["unit"]} ({len(c["funcs"])} functions) for {a.builder}')
    return 0 if not refused else 1


def release_lines(lines, what, norm=None):
    """LINES [(unit, builder, model)] without those naming WHAT (a builder ID or a unit; NORM: WHAT resolved)."""
    return [x for x in lines if x[1] != what and x[0] != what and x[0] != norm]


def cmd_release(a):
    r = C.repo()
    if len(a.what) == 1 and a.what[0].isdigit():
        return release_issue(r, int(a.what[0]), a.keep_line)
    p = run_pr(r, branch())
    if not p:
        sys.exit('run_branch: the run has no open PR')
    text = ' '.join(a.what)
    try:
        norm = C.Index().resolve(text, cap=None)[0]
    except ValueError:
        norm = None
    lines = C.run_lines(p.get('body'))
    keep = release_lines(lines, text, norm)
    if len(keep) == len(lines):
        sys.exit(f'run_branch: no claim line of PR #{p["number"]} names {text!r}')
    set_body(r, p['number'], C.set_run_lines(p.get('body'), keep))
    gone = {x[0] for x in lines} - {x[0] for x in keep}
    C.update_cache(drop=lambda c: c.get('kind') == 'pr' and c.get('pr') == p['number'] and c['unit'] in gone)
    print(f'PR #{p["number"]}: released ' + '; '.join(sorted(gone)))
    return 0


def release_issue(r, number, keep_line):
    C.gh(f'repos/{r}/issues/{number}/comments', 'POST', {'body': '/release'})
    print(f'#{number}: /release commented')
    if not keep_line:
        p = run_pr(r, branch())
        if p:
            body = p.get('body') or ''
            new = re.sub(rf'(?im)^claims?\s+#{number}\s*\n?', '', body)
            if new != body:
                set_body(r, p['number'], new)
                print(f'PR #{p["number"]}: Claims #{number} removed')
    C.update_cache(drop=lambda c: c.get('kind') == 'issue' and c.get('number') == number)
    return 0


def cmd_push(a):
    b = branch()
    if subprocess.run(['git', 'push', 'origin', b], cwd=ROOT).returncode:
        sys.exit('run_branch: push failed (see the pre-push check above)')
    ensure_pr(C.repo(), b)


CARD = ['README.md', 'docs/progress.svg', 'docs/progress_chart.svg', 'docs/progress_map.svg',
        'docs/data/progress-history.csv']


def progress_card(co_author=None):
    """The README's progress card and history from the run's committed data, committed on the run branch (the card
    reaches master with each run's PR; master takes no bot commits, D148)."""
    if git('status', '--porcelain', '--', *CARD):
        sys.exit('run_branch: the progress card files have uncommitted changes; commit or revert them first')
    if subprocess.run([sys.executable, os.path.join(HERE, 'progress.py'), '--history', '--today', '--readme'],
                      cwd=ROOT).returncode:
        sys.exit('run_branch: tools/progress.py failed')
    if git('status', '--porcelain', '--', *CARD):
        git('add', '--', *CARD)
        msg = 'Regenerate the progress card and history for the run (tools/progress.py)'
        git('commit', '-q', '-m', msg + (f'\n\nCo-Authored-By: {co_author}' if co_author else ''), '--', *CARD)


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
    progress_card(a.co_author)
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
        old = sorted({int(x) for x in C.CLAIMS_REF.findall(p.get('body') or '')})
        print(f'PR #{p["number"]}' + (' (draft)' if p.get('draft') else '') + ': run claims:')
        for unit, builder, model in C.run_lines(p.get('body')):
            print(f'  {unit} ({builder}' + (f', {model}' if model else '') + ')')
        if old:
            print(f'  issue claims named by `Claims #N`: {old}')
    live, doc = C.live_claims(r)
    print(f'{len(live)} live claims in all ({C.age_note(doc)}):')
    for c in live:
        print('  ' + C.describe(c))


def cmd_cleanup(a):
    """Local branches whose every commit is in origin/master (merged, or the same patch by `git cherry`) are deleted
    with their worktrees; branches with commits of their own are kept and listed; local master is fast-forwarded;
    remote branches other than master and the current run's are listed (GitHub deletes a merged PR's branch)."""
    git('fetch', '-q', '--prune', 'origin')
    cur = git('branch', '--show-current')
    trees = {}
    for block in git('worktree', 'list', '--porcelain').split('\n\n'):
        lines = dict(l.split(' ', 1) for l in block.splitlines() if ' ' in l)
        if lines.get('branch', '').startswith('refs/heads/'):
            trees[lines['branch'][len('refs/heads/'):]] = lines['worktree']
    gone, kept = [], []
    for b in git('for-each-ref', '--format=%(refname:short)', 'refs/heads/').split():
        if b in (cur, 'master'):
            continue
        own = [l for l in git('cherry', 'origin/master', b).splitlines() if l.startswith('+')]
        if own:
            kept.append(f'{b} ({len(own)} commit(s) of its own)')
            continue
        if b in trees and os.path.normcase(os.path.abspath(trees[b])) != os.path.normcase(ROOT):
            if git('-C', trees[b], 'status', '--porcelain', check=False).stdout.strip():
                kept.append(f'{b} (its worktree {trees[b]} has uncommitted changes)')
                continue
            if not a.dry:
                git('worktree', 'remove', trees[b])
        if not a.dry:
            git('branch', '-D', b)
        gone.append(b)
    if cur != 'master' and not a.dry:
        git('fetch', '-q', 'origin', 'master:master', check=False)
    print(f'{"would delete" if a.dry else "deleted"} {len(gone)} local branch(es): {", ".join(gone) or "none"}')
    print(f'kept {len(kept)}: {"; ".join(kept) or "none"}')
    remote = [l.split('refs/heads/', 1)[1] for l in git('ls-remote', '--heads', 'origin').splitlines()]
    left = [x for x in remote if x not in ('master', cur) and not x.startswith('verified/')]
    print(f'remote branches besides master and {cur}: {", ".join(left) or "none"}'
          + (' (delete a merged one with `git push origin --delete NAME` once its PR is merged)' if left else ''))


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = ap.add_subparsers(dest='cmd', required=True)
    p = sub.add_parser('start')
    p.add_argument('--date')
    p.add_argument('--co-author', help="the start commit's Co-Authored-By value (D71)")
    p = sub.add_parser('claim')
    p.add_argument('unit', nargs='+')
    p.add_argument('--builder', required=True)
    p.add_argument('--model', default='')
    p.add_argument('--split', action='store_true')
    p = sub.add_parser('release')
    p.add_argument('what', nargs='+', help='a unit or builder ID (a run claim), or N (an old issue claim)')
    p.add_argument('--keep-line', action='store_true')
    sub.add_parser('push')
    rd = sub.add_parser('ready')
    rd.add_argument('--summary', required=True)
    rd.add_argument('--co-author', help="the card commit's Co-Authored-By value (D71), e.g. 'Claude Opus 5.5 <noreply@anthropic.com>'")
    sub.add_parser('status')
    sub.add_parser('cleanup').add_argument('--dry', action='store_true')
    a = ap.parse_args()
    return {'start': cmd_start, 'claim': cmd_claim, 'release': cmd_release, 'push': cmd_push,
            'ready': cmd_ready, 'status': cmd_status, 'cleanup': cmd_cleanup}[a.cmd](a) or 0


if __name__ == '__main__':
    sys.exit(main())
