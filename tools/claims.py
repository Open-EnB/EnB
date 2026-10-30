#!/usr/bin/env python3
"""Claims: which not-yet-exact functions an agent is rebuilding (docs/github-plan.md section 4.5, P-GH-10).

    python tools/claims.py resolve UNIT             the functions UNIT covers (offline, master's data)
    python tools/claims.py list [--json]            live claims on GitHub
    python tools/claims.py check UNIT|ADDR          who holds any of these functions
    python tools/claims.py claim UNIT [--agent A] [--model M] [--accountable P] [--no-wait]
                                                    open a claim issue and wait for the bot's answer
    python tools/claims.py renew N | release N      comment /renew or /release on claim issue #N
    python tools/claims.py reconcile [--dry-run]    the Action's step (.github/workflows/claims.yml)

UNIT is one source file's window, `app\\client\\eff_o.cpp` (the client's __FILE__ path, `/` works too), a
range inside it, `app\\client\\cpacket.cpp 0x5A1000-0x5A3F00`, or a bare range `0x7D3CC0-0x7D5000` for code no
file is attributed yet. A claim covers the functions of docs/data/app-functions.json in it that are not yet
exact (neither `in_tree` nor `ambiguous` in docs/data/imports.json), at most CAP of them. Two claims conflict when
they share a function. A claim says which functions you rebuild, not which files you may edit.

A claim is a GitHub issue titled `claim: UNIT` (the issue form, or `claim` here). The Action runs `reconcile`
on every claim event, every comment and hourly; each run rebuilds the state from all open claim issues, so a
dropped run loses nothing. Each claim's state is a JSON marker in a comment by github-actions[bot] (markers in
anyone else's comments are ignored, a maintainer's excepted). Lifetime, counted only while it is the
contributor's turn (before a PR, while the PR is a draft, after a failed vc6-verify or a review asking for
changes, until the next push): NOPR_H hours without a PR (`/renew` adds as much, RENEWALS times), INACTIVE_D
days without the contributor's activity once a PR says `Claims #N`, TOTAL_D days in all; a PR closed unmerged
leaves GRACE_H hours; a merged PR closes the claim. PER_ACCOUNT live claims per account, maintainers excepted.

`gh` (authenticated) is needed for everything but `resolve`. Standard library only.
"""
import argparse
import datetime as dt
import json
import os
import re
import subprocess
import sys
import time

ROOT = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..'))
APP_FUNCTIONS = os.path.join(ROOT, 'docs', 'data', 'app-functions.json')
IMPORTS = os.path.join(ROOT, 'docs', 'data', 'imports.json')

CAP = 30
NOPR_H = 8
RENEWALS = 2
INACTIVE_D = 3
TOTAL_D = 7
GRACE_H = 24
PER_ACCOUNT = 3
TICK_CAP_H = 2          # the clock advances at most this much per reconcile (a stalled schedule costs no one)

BOT = 'github-actions[bot]'
MAINTAINER = {'OWNER', 'MEMBER', 'COLLABORATOR'}
MARK = re.compile(r'<!-- enb-claim (\{.*?\}) -->', re.S)
TITLE = re.compile(r'^\s*claim\s*:\s*(.+?)\s*$', re.I)
RANGE = re.compile(r'^(0x[0-9a-f]+)\s*-\s*(0x[0-9a-f]+)$', re.I)
CLAIMS_REF = re.compile(r'(?i)\bclaims?\s+#(\d+)')
LIVE = 'granted'


# ---------------------------------------------------------------- units

class Index:
    """Master's function starts, their files, and which are done."""

    def __init__(self, app_functions=APP_FUNCTIONS, imports=IMPORTS):
        with open(app_functions, encoding='utf-8') as fh:
            funcs = json.load(fh)['functions']
        self.starts = sorted(int(f['address'], 16) for f in funcs)
        self.by_file = {}
        for f in funcs:
            if f.get('file'):
                self.by_file.setdefault(f['file'].lower(), []).append(int(f['address'], 16))
        for v in self.by_file.values():
            v.sort()
        self.done = set()
        with open(imports, encoding='utf-8') as fh:
            libs = json.load(fh)
        for lib in libs.values():
            for rec in lib.get('files', {}).values():
                for key in ('in_tree', 'ambiguous'):
                    self.done.update(int(a, 16) for a in rec.get(key, []))

    def resolve(self, unit, cap=CAP):
        """(normalized unit, [addresses]) of the not-yet-exact functions UNIT covers; ValueError says why not."""
        text = unit.strip()
        m = TITLE.match(text)
        if m:
            text = m.group(1)
        text = text.strip('`').strip()
        path, rng = None, None
        parts = text.split()
        if len(parts) == 1 and RANGE.match(parts[0].replace(' ', '')):
            rng = parts[0]
        elif len(parts) >= 1 and re.search(r'\.(cpp|h|inl)$', parts[0], re.I):
            path = parts[0].replace('/', '\\')
            rest = ''.join(parts[1:])
            if rest:
                if not RANGE.match(rest):
                    raise ValueError(f'not a range: {rest!r} (expected 0xSTART-0xEND)')
                rng = rest
        else:
            rng = text.replace(' ', '')
            if not RANGE.match(rng):
                raise ValueError(f'not a unit: {unit!r} (a file such as app\\client\\eff_o.cpp, '
                                 'a file and 0xSTART-0xEND, or 0xSTART-0xEND)')
        lo = hi = None
        if rng:
            a, b = RANGE.match(rng).groups()
            lo, hi = int(a, 16), int(b, 16)
            if lo >= hi:
                raise ValueError(f'empty range {a}-{b}')
        if path:
            sel = self.by_file.get(path.lower())
            if sel is None:
                raise ValueError(f'no functions are attributed to {path} (docs/data/app-functions.json); '
                                 'claim a bare range instead')
            if rng:
                sel = [x for x in sel if lo <= x < hi]
                if not sel:
                    raise ValueError(f'{rng} holds none of {path}\'s functions')
        else:
            sel = [x for x in self.starts if lo <= x < hi]
        todo = [x for x in sel if x not in self.done]
        norm = (path or '') + (' ' if path and rng else '') + (f'0x{lo:X}-0x{hi:X}' if rng else '')
        if not todo:
            raise ValueError(f'{norm}: nothing left to rebuild ({len(sel)} functions, all exact)')
        if cap is not None and len(todo) > cap:
            end = todo[cap]
            raise ValueError(f'{norm}: {len(todo)} functions not yet exact, the cap is {cap}; claim a range, '
                             f'e.g. `{path + " " if path else ""}0x{todo[0]:X}-0x{end:X}` ({cap} functions)')
        return norm, todo


# ---------------------------------------------------------------- the clock

def parse_time(s):
    return dt.datetime.strptime(s, '%Y-%m-%dT%H:%M:%SZ').replace(tzinfo=dt.timezone.utc)


def fmt_time(t):
    return t.strftime('%Y-%m-%dT%H:%M:%SZ')


def turn(pr):
    """Whose turn it is on a linked open PR: 'contributor' or 'maintainer'.
    pr: {draft, verify ('failure'|'error'|'success'|'pending'|None), changes_requested (on the head commit)}"""
    if pr['draft'] or pr.get('verify') in ('failure', 'error') or pr.get('changes_requested'):
        return 'contributor'
    return 'maintainer'


def advance(st, now, pr):
    """Advance claim state ST (granted) to NOW. PR: None, or the linked PR {state: open|merged|closed, draft,
    verify, changes_requested, head, author_active (an ISO time of the PR author's latest comment), closed_at}.
    Returns (state, verdict): verdict None (still live), 'landed', or an expiry reason."""
    st = dict(st)
    last = parse_time(st['last_tick'])
    step = max(0.0, min((now - last).total_seconds(), TICK_CAP_H * 3600))
    st['last_tick'] = fmt_time(now)
    if pr and pr['state'] == 'merged':
        st['pr'] = pr['number']
        return st, 'landed'
    if pr and pr['state'] == 'closed':
        st['pr'] = pr['number']
        if (now - parse_time(pr['closed_at'])).total_seconds() > GRACE_H * 3600:
            return st, f'its PR #{pr["number"]} was closed unmerged over {GRACE_H} h ago'
        return st, None
    if pr is None:
        st['nopr_used'] += step
        st['total_used'] += step
        if st['nopr_used'] > NOPR_H * 3600 * (1 + st['renewals']):
            return st, f'no pull request within {NOPR_H * (1 + st["renewals"])} h'
    else:
        st['pr'] = pr['number']
        if pr['head'] != st.get('head') or (pr.get('author_active') and pr['author_active'] > st.get('active', '')):
            st['inactive_used'] = 0.0
            st['head'] = pr['head']
            if pr.get('author_active'):
                st['active'] = max(pr['author_active'], st.get('active', ''))
        if turn(pr) == 'contributor':
            st['inactive_used'] += step
            st['total_used'] += step
        if st['inactive_used'] > INACTIVE_D * 86400:
            return st, f'{INACTIVE_D} days without activity on PR #{pr["number"]} while it was the contributor\'s turn'
    if st['total_used'] > TOTAL_D * 86400:
        return st, f'{TOTAL_D} days of contributor time in all'
    return st, None


def new_state(issue, unit, funcs, now):
    return {'v': 1, 'state': LIVE, 'unit': unit, 'funcs': [f'0x{a:X}' for a in funcs], 'holder': issue['user']['login'],
            'granted': fmt_time(now), 'last_tick': fmt_time(now), 'renewals': 0, 'nopr_used': 0.0,
            'inactive_used': 0.0, 'total_used': 0.0, 'pr': None, 'head': None, 'seen': 0}


# ---------------------------------------------------------------- GitHub

def repo():
    r = os.environ.get('GITHUB_REPOSITORY')
    if r:
        return r
    out = subprocess.run(['gh', 'repo', 'view', '--json', 'nameWithOwner', '-q', '.nameWithOwner'],
                         cwd=ROOT, capture_output=True, text=True)
    return out.stdout.strip() or 'Open-EnB/EnB'


def gh(path, method='GET', fields=None):
    cmd = ['gh', 'api', '-X', method, path]
    for k, v in (fields or {}).items():
        cmd += ['-f', f'{k}={v}']
    r = subprocess.run(cmd, capture_output=True, text=True, encoding='utf-8')
    if r.returncode:
        raise RuntimeError(f'gh api {method} {path}: {r.stderr.strip() or r.stdout.strip()}')
    return json.loads(r.stdout) if r.stdout.strip() else None


def gh_all(path):
    out, page = [], 1
    sep = '&' if '?' in path else '?'
    while True:
        chunk = gh(f'{path}{sep}per_page=100&page={page}')
        out += chunk
        if len(chunk) < 100:
            return out
        page += 1


def trusted(c):
    return c['user']['login'] == BOT or c.get('author_association') in MAINTAINER


def read_marker(comments):
    """(comment, state) of the newest trusted marker, or (None, None)."""
    for c in reversed(comments):
        if trusted(c):
            m = MARK.search(c.get('body') or '')
            if m:
                return c, json.loads(m.group(1))
    return None, None


def render(st, note, number='N'):
    lines = [note, '']
    if st['state'] == LIVE:
        lines += [f'**Claimed by @{st["holder"]}:** `{st["unit"]}`, {len(st["funcs"])} functions not yet exact '
                  f'({st["funcs"][0]} .. {st["funcs"][-1]}).', '',
                  f'Open a pull request (a draft counts) with `Claims #{number}` in its description within '
                  f'{NOPR_H * (1 + st["renewals"])} h of contributor time; `/renew` adds {NOPR_H} h '
                  f'({RENEWALS - st["renewals"]} left), `/release` gives it back. Then: {INACTIVE_D} days without '
                  f'your activity while it is your turn, {TOTAL_D} days in all (docs/github-plan.md 4.5.2).', '',
                  f'Used: {st["nopr_used"] / 3600:.1f} h before a PR, {st["inactive_used"] / 3600:.1f} h inactive, '
                  f'{st["total_used"] / 3600:.1f} h in all' + (f'; PR #{st["pr"]}' if st.get('pr') else '') + '.']
    lines += ['', '<!-- enb-claim ' + json.dumps(st, sort_keys=True) + ' -->']
    return '\n'.join(lines)


def linked_prs(r):
    """{claim number: [PRs naming it]} among the 300 most recently updated PRs."""
    out = {}
    for p in gh_all(f'repos/{r}/pulls?state=all&sort=updated&direction=desc')[:300]:
        state = 'open' if p['state'] == 'open' else ('merged' if p.get('merged_at') else 'closed')
        for n in {int(x) for x in CLAIMS_REF.findall(p.get('body') or '')}:
            out.setdefault(n, []).append({
                'number': p['number'], 'state': state, 'draft': p.get('draft', False), 'head': p['head']['sha'],
                'author': p['user']['login'], 'maintainer': p.get('author_association') in MAINTAINER,
                'closed_at': p.get('closed_at')})
    return out


def pick_pr(candidates, holder):
    """The PR that counts for a claim: by its holder or a maintainer (so no one else can land or expire it),
    open before merged before closed."""
    rank = {'open': 2, 'merged': 1, 'closed': 0}
    ok = [p for p in candidates or [] if p['author'] == holder or p['maintainer']]
    return max(ok, key=lambda p: (rank[p['state']], p['number'])) if ok else None


def pr_detail(r, pr):
    """Add verify, changes_requested and author_active to an open linked PR."""
    sts = gh(f'repos/{r}/commits/{pr["head"]}/status')
    v = [s for s in sts.get('statuses', []) if s['context'] == 'vc6-verify']
    pr['verify'] = v[0]['state'] if v else None
    revs = gh_all(f'repos/{r}/pulls/{pr["number"]}/reviews')
    revs = [x for x in revs if x.get('author_association') in MAINTAINER and x.get('commit_id') == pr['head']]
    pr['changes_requested'] = bool(revs) and revs[-1]['state'] == 'CHANGES_REQUESTED'
    com = [c for c in gh_all(f'repos/{r}/issues/{pr["number"]}/comments') if c['user']['login'] == pr['author']]
    pr['author_active'] = com[-1]['created_at'] if com else None
    return pr


# ---------------------------------------------------------------- reconcile (the Action)

def reconcile(dry_run=False, now=None, index=None):
    r = repo()
    now = now or dt.datetime.now(dt.timezone.utc).replace(microsecond=0)
    index = index or Index()
    issues = [i for i in gh_all(f'repos/{r}/issues?state=open') if 'pull_request' not in i and TITLE.match(i['title'])]
    issues.sort(key=lambda i: i['number'])
    prs = linked_prs(r)
    log = []

    def act(desc, fn):
        log.append(desc)
        if not dry_run:
            fn()

    def write(issue, marker, st, note, new_comment=False):
        body = render(st, note, issue['number'])
        if marker is None or new_comment:
            act(f'#{issue["number"]}: comment ({note[:60]})',
                lambda: gh(f'repos/{r}/issues/{issue["number"]}/comments', 'POST', {'body': body}))
        else:
            act(f'#{issue["number"]}: update state',
                lambda: gh(f'repos/{r}/issues/comments/{marker["id"]}', 'PATCH', {'body': body}))

    def close(issue, reason):
        act(f'#{issue["number"]}: close ({reason})',
            lambda: gh(f'repos/{r}/issues/{issue["number"]}', 'PATCH', {'state': 'closed', 'state_reason': reason}))

    def label(issue, name):
        if name not in {x['name'] for x in issue.get('labels', [])}:
            act(f'#{issue["number"]}: label {name}',
                lambda: gh(f'repos/{r}/issues/{issue["number"]}/labels', 'POST', {'labels[]': name}))

    held = {}                                   # function -> claim number
    per_holder = {}
    pending = []
    for issue in issues:
        comments = gh_all(f'repos/{r}/issues/{issue["number"]}/comments')
        marker, st = read_marker(comments)
        label(issue, 'claim')
        if st and st.get('state') == LIVE:
            # commands since the last run
            for c in comments:
                if c['id'] <= st.get('seen', 0) or c['user']['login'] == BOT:
                    continue
                st['seen'] = c['id']
                cmd = (c.get('body') or '').strip().split()[:1]
                who, maint = c['user']['login'], c.get('author_association') in MAINTAINER
                if cmd == ['/release'] and (who == st['holder'] or maint):
                    st['state'] = 'released'
                    st['by'] = who
                elif cmd == ['/renew'] and who == st['holder']:
                    if st.get('pr') or pick_pr(prs.get(issue['number']), st['holder']):
                        st['note'] = 'renew refused: a pull request names this claim; its own clock applies'
                    elif st['renewals'] >= RENEWALS:
                        st['note'] = f'renew refused: {RENEWALS} renewals used'
                    else:
                        st['renewals'] += 1
                        st['note'] = f'renewed: {NOPR_H * (1 + st["renewals"])} h before a pull request'
            if st['state'] == 'released':
                write(issue, marker, st, f'Released by @{st["by"]}.', new_comment=True)
                close(issue, 'completed')
                continue
            pr = pick_pr(prs.get(issue['number']), st['holder'])
            if pr and pr['state'] == 'open':
                pr = pr_detail(r, pr)
            note = st.pop('note', None)
            st, verdict = advance(st, now, pr)
            if verdict == 'landed':
                st['state'] = 'landed'
                write(issue, marker, st, f'Landed with PR #{pr["number"]}.', new_comment=True)
                close(issue, 'completed')
                continue
            if verdict:
                st['state'] = 'expired'
                write(issue, marker, st, f'Expired: {verdict}. The functions are free to claim again.',
                      new_comment=True)
                label(issue, 'claim-expired')
                close(issue, 'not_planned')
                continue
            write(issue, marker, st, note or 'Live.', new_comment=bool(note))
            for a in st['funcs']:
                held[a] = issue['number']
            if issue.get('author_association') not in MAINTAINER:
                per_holder[st['holder']] = per_holder.get(st['holder'], 0) + 1
        else:
            pending.append((issue, marker))
    for issue, marker in pending:
        who = issue['user']['login']
        seen = max([c['id'] for c in gh_all(f'repos/{r}/issues/{issue["number"]}/comments')] or [0])
        try:
            unit, funcs = index.resolve(issue['title'])
        except ValueError as e:
            write(issue, None, {'v': 1, 'state': 'refused', 'holder': who}, f'Refused: {e}')
            close(issue, 'not_planned')
            continue
        clash = sorted({held[f'0x{a:X}'] for a in funcs if f'0x{a:X}' in held})
        if clash:
            write(issue, None, {'v': 1, 'state': 'refused', 'holder': who},
                  'Refused: overlaps live claim ' + ', '.join(f'#{n}' for n in clash) + '. `python tools/claims.py '
                  'list` shows what is held.')
            close(issue, 'not_planned')
            continue
        if issue.get('author_association') not in MAINTAINER and per_holder.get(who, 0) >= PER_ACCOUNT:
            write(issue, None, {'v': 1, 'state': 'refused', 'holder': who},
                  f'Refused: @{who} already holds {PER_ACCOUNT} live claims.')
            close(issue, 'not_planned')
            continue
        st = new_state(issue, unit, funcs, now)
        st['seen'] = seen
        write(issue, None, st, 'Granted.')
        for a in st['funcs']:
            held[a] = issue['number']
        per_holder[who] = per_holder.get(who, 0) + 1
    for line in log:
        print(('(dry run) ' if dry_run else '') + line)
    print(f'reconcile: {len(issues)} open claim issues, {len(set(held.values()))} live after this run')


# ---------------------------------------------------------------- CLI

def live_claims(r):
    out = []
    for i in gh_all(f'repos/{r}/issues?state=open&labels=claim'):
        if 'pull_request' in i:
            continue
        _, st = read_marker(gh_all(f'repos/{r}/issues/{i["number"]}/comments'))
        if st and st.get('state') == LIVE:
            out.append((i['number'], st))
    return out


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = ap.add_subparsers(dest='cmd', required=True)
    p = sub.add_parser('resolve')
    p.add_argument('unit', nargs='+')
    p = sub.add_parser('list')
    p.add_argument('--json', action='store_true')
    p = sub.add_parser('check')
    p.add_argument('unit', nargs='+')
    p = sub.add_parser('claim')
    p.add_argument('unit', nargs='+')
    p.add_argument('--agent', default='')
    p.add_argument('--model', default='')
    p.add_argument('--accountable', default='')
    p.add_argument('--no-wait', action='store_true')
    for name in ('renew', 'release'):
        sub.add_parser(name).add_argument('number', type=int)
    sub.add_parser('reconcile').add_argument('--dry-run', action='store_true')
    a = ap.parse_args()

    if a.cmd == 'resolve':
        unit, funcs = Index().resolve(' '.join(a.unit), cap=None)
        print(f'{unit}: {len(funcs)} functions not yet exact' + (f' (over the cap of {CAP})' if len(funcs) > CAP else ''))
        for f in funcs:
            print(f'  0x{f:X}')
        return 0
    if a.cmd == 'reconcile':
        reconcile(a.dry_run)
        return 0
    r = repo()
    if a.cmd == 'list':
        claims = live_claims(r)
        if a.json:
            print(json.dumps([dict(st, number=n) for n, st in claims], indent=1))
        for n, st in claims:
            print(f'#{n:<5} @{st["holder"]:<20} {st["unit"]:<45} {len(st["funcs"]):>3} functions'
                  + (f'  PR #{st["pr"]}' if st.get('pr') else f'  {st["nopr_used"] / 3600:.1f} h before a PR'))
        if not a.json:
            print(f'{len(claims)} live claims')
        return 0
    if a.cmd == 'check':
        text = ' '.join(a.unit)
        if re.fullmatch(r'0x[0-9a-fA-F]+', text):
            want = {int(text, 16)}
        else:
            want = set(Index().resolve(text, cap=None)[1])
        hits = [(n, st) for n, st in live_claims(r) if want & {int(x, 16) for x in st['funcs']}]
        for n, st in hits:
            print(f'held: #{n} @{st["holder"]} {st["unit"]}')
        print('free' if not hits else f'{len(hits)} live claim(s) overlap')
        return 1 if hits else 0
    if a.cmd in ('renew', 'release'):
        gh(f'repos/{r}/issues/{a.number}/comments', 'POST', {'body': '/' + a.cmd})
        print(f'commented /{a.cmd} on #{a.number}; the Action answers there')
        return 0
    # claim
    unit, funcs = Index().resolve(' '.join(a.unit))
    body = (f'### Unit\n\n{unit}\n\n### Agent\n\n{a.agent or "_No response_"}\n\n### Model\n\n{a.model or "_No response_"}'
            f'\n\n### Accountable person\n\n{a.accountable or "_No response_"}\n')
    issue = gh(f'repos/{r}/issues', 'POST', {'title': f'claim: {unit}', 'body': body})
    print(f'opened #{issue["number"]} for {unit} ({len(funcs)} functions): {issue["html_url"]}')
    if a.no_wait:
        return 0
    for _ in range(36):
        time.sleep(5)
        c, st = read_marker(gh_all(f'repos/{r}/issues/{issue["number"]}/comments'))
        if st:
            print(c['body'].split('\n', 1)[0])
            return 0 if st.get('state') == LIVE else 1
    print('no answer from the Action in 3 minutes; see the issue')
    return 2


if __name__ == '__main__':
    sys.exit(main())
