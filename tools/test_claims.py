"""Tests for tools/claims.py: units, the clock, whose turn, which PR counts, trusted markers.

    python -m unittest discover -s tools -p "test_*.py"

Standard library only; no network (the GitHub side is exercised by a rehearsal, docs/github-plan.md 4.5.6).
"""
import datetime as dt
import json
import os
import sys
import tempfile
import unittest

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)

import claims  # noqa: E402

T0 = dt.datetime(2026, 11, 1, 12, 0, 0, tzinfo=dt.timezone.utc)
H = dt.timedelta(hours=1)


def index():
    funcs = [{'address': '0x1000', 'file': 'app\\a.cpp'}, {'address': '0x1010', 'file': 'app\\a.cpp'},
             {'address': '0x1020', 'file': 'app\\a.cpp'}, {'address': '0x1030'},
             {'address': '0x1040', 'file': 'app\\b.cpp'}]
    imports = {'app': {'files': {'a': {'in_tree': ['0x1010'], 'ambiguous': []}}}}
    d = tempfile.mkdtemp()
    for name, data in (('f.json', {'functions': funcs}), ('i.json', imports)):
        with open(os.path.join(d, name), 'w', encoding='utf-8', newline='\n') as f:
            json.dump(data, f)
    return claims.Index(os.path.join(d, 'f.json'), os.path.join(d, 'i.json'))


def granted(**kw):
    st = claims.new_state({'user': {'login': 'alice'}}, 'app\\a.cpp', [0x1000, 0x1020], T0)
    st.update(kw)
    return st


def run(st, hours, pr=None, step=1):
    """Advance hour by hour (the hourly sweep); return (state, verdict, hours run)."""
    t = claims.parse_time(st['last_tick'])
    for k in range(0, hours, step):
        t += step * H
        st, verdict = claims.advance(st, t, pr)
        if verdict:
            return st, verdict, k + step
    return st, None, hours


class Units(unittest.TestCase):
    def test_file_skips_exact(self):
        self.assertEqual(index().resolve('claim: app/a.cpp'), ('app\\a.cpp', [0x1000, 0x1020]))

    def test_file_range_and_bare_range(self):
        self.assertEqual(index().resolve('app\\a.cpp 0x1015-0x1030')[1], [0x1020])
        self.assertEqual(index().resolve('0x1020-0x1041')[1], [0x1020, 0x1030, 0x1040])

    def test_refusals(self):
        ix = index()
        for unit, why in (('app/zz.cpp', 'no functions'), ('0x1010-0x1011', 'nothing left'),
                          ('0x2000-0x1000', 'empty range'), ('hello', 'not a unit'),
                          ('app/a.cpp 0x5000-0x6000', 'holds none')):
            with self.assertRaises(ValueError) as e:
                ix.resolve(unit)
            self.assertIn(why, str(e.exception), unit)

    def test_cap(self):
        with self.assertRaises(ValueError) as e:
            index().resolve('0x1000-0x2000', cap=2)
        self.assertIn('0x1000-0x1030', str(e.exception))


class Clock(unittest.TestCase):
    def test_no_pr_expires_after_8h(self):
        _, verdict, hours = run(granted(), 30)
        self.assertEqual(hours, claims.NOPR_H + 1)
        self.assertIn('no pull request', verdict)

    def test_renewals_extend(self):
        _, _, hours = run(granted(renewals=2), 40)
        self.assertEqual(hours, claims.NOPR_H * 3 + 1)

    def test_stalled_sweep_counts_at_most_2h(self):
        st, verdict = claims.advance(granted(), T0 + 20 * H, None)
        self.assertIsNone(verdict)
        self.assertEqual(st['nopr_used'], claims.TICK_CAP_H * 3600)

    def test_maintainers_turn_stops_the_clock(self):
        pr = {'number': 7, 'state': 'open', 'draft': False, 'verify': None, 'head': 'aa'}
        st, verdict, _ = run(granted(), 24 * 30, pr)
        self.assertIsNone(verdict)
        self.assertEqual(st['total_used'], 0)

    def test_draft_inactivity_expires_after_3_days(self):
        pr = {'number': 7, 'state': 'open', 'draft': True, 'verify': None, 'head': 'aa'}
        _, verdict, hours = run(granted(), 24 * 10, pr)
        self.assertEqual(hours, claims.INACTIVE_D * 24 + 1)
        self.assertIn('without activity', verdict)

    def test_push_resets_inactivity_but_not_total(self):
        st = granted()
        for k in range(8):                       # a new head every 23 hours: never inactive, 7 days in all
            pr = {'number': 7, 'state': 'open', 'draft': True, 'verify': None, 'head': f'h{k}'}
            st, verdict, _ = run(st, 23, pr)
            if verdict:
                break
        self.assertIn('7 days', verdict)

    def test_failed_verify_is_contributors_turn(self):
        self.assertEqual(claims.turn({'draft': False, 'verify': 'failure'}), 'contributor')
        self.assertEqual(claims.turn({'draft': False, 'verify': 'success'}), 'maintainer')
        self.assertEqual(claims.turn({'draft': False, 'verify': None, 'changes_requested': True}), 'contributor')

    def test_merged_lands_closed_has_grace(self):
        self.assertEqual(claims.advance(granted(), T0 + H, {'number': 7, 'state': 'merged'})[1], 'landed')
        closed = {'number': 7, 'state': 'closed', 'closed_at': claims.fmt_time(T0)}
        self.assertIsNone(claims.advance(granted(), T0 + 23 * H, closed)[1])
        self.assertIn('closed unmerged', claims.advance(granted(), T0 + 25 * H, closed)[1])


class Links(unittest.TestCase):
    def test_only_holder_or_maintainer_prs_count(self):
        prs = [{'number': 9, 'state': 'closed', 'author': 'mallory', 'maintainer': False},
               {'number': 8, 'state': 'merged', 'author': 'alice', 'maintainer': False},
               {'number': 10, 'state': 'open', 'author': 'owner', 'maintainer': True}]
        self.assertEqual(claims.pick_pr(prs, 'alice')['number'], 10)
        self.assertEqual(claims.pick_pr(prs[:2], 'alice')['number'], 8)
        self.assertIsNone(claims.pick_pr(prs[:1], 'alice'))

    def test_forged_marker_ignored(self):
        st = {'state': 'granted', 'holder': 'mallory'}
        forged = {'user': {'login': 'mallory'}, 'author_association': 'NONE',
                  'body': '<!-- enb-claim ' + json.dumps(st) + ' -->'}
        real = {'user': {'login': claims.BOT}, 'author_association': 'NONE',
                'body': claims.render(granted(), 'Granted.')}
        self.assertEqual(claims.read_marker([real, forged])[1]['holder'], 'alice')
        self.assertEqual(claims.read_marker([forged]), (None, None))

    def test_claims_reference(self):
        self.assertEqual(claims.CLAIMS_REF.findall('Claims #12\nclaims #3, Claim #4'), ['12', '3', '4'])


def pull(number, body, assoc='OWNER', state='open', login='owner'):
    return {'number': number, 'state': state, 'author_association': assoc, 'body': body, 'user': {'login': login},
            'updated_at': '2026-11-01T12:00:00Z', 'draft': True, 'head': {'sha': 'h'}, 'merged_at': None,
            'closed_at': None}


class RunClaims(unittest.TestCase):
    """A maintainer run's claims, as lines in its PR's body."""

    def test_round_trip_and_lines_outside_ignored(self):
        lines = [('0x1000-0x1030', 'TK16', 'opus'), ('app\\b.cpp', 'TK17', '')]
        body = claims.set_run_lines('summary\nclaim: 0x5000-0x6000 (X, y)\n', lines)
        self.assertIn('claim: 0x1000-0x1030 (TK16, opus)\n', body)
        self.assertIn('claim: app\\b.cpp (TK17)\n', body)
        self.assertEqual(claims.run_lines(body), lines)          # the line outside the block is not a claim
        body2 = claims.set_run_lines(body, lines[1:])
        self.assertEqual(claims.run_lines(body2), lines[1:])
        self.assertEqual(body2.count(claims.RUN_OPEN), 1)
        self.assertTrue(body2.startswith('summary\nclaim: 0x5000-0x6000 (X, y)\n'))
        self.assertEqual(claims.run_lines('no block here'), [])

    def test_file_range_unit_parses(self):
        body = claims.set_run_lines('', [])
        body = body.replace(claims.RUN_CLOSE, 'claim: `app\\a.cpp 0x1015-0x1030` (TK1, sonnet)\n' + claims.RUN_CLOSE)
        self.assertEqual(claims.run_lines(body), [('app\\a.cpp 0x1015-0x1030', 'TK1', 'sonnet')])

    def test_only_open_maintainer_prs_hold(self):
        body = claims.set_run_lines('', [('app\\a.cpp', 'TK1', 'opus'), ('0x1010-0x1011', 'TK2', '')])
        pulls = [pull(78, body), pull(79, body, assoc='CONTRIBUTOR'), pull(80, body, state='closed')]
        got = claims.run_claims(pulls, index())
        self.assertEqual([(c['pr'], c['unit'], c['funcs']) for c in got],
                         [(78, 'app\\a.cpp', ['0x1000', '0x1020']), (78, '0x1010-0x1011', [])])
        self.assertEqual(got[0]['builder'], 'TK1')
        self.assertEqual(claims.label_of(got[0]), 'PR #78')

    def test_overlap_with_issue_claims(self):
        issue = claims.issue_entry({'number': 12, 'updated_at': 'x'}, granted())
        run = claims.run_claims([pull(78, claims.set_run_lines('', [('0x1030-0x1041', 'TK1', '')]))], index())
        live = [issue] + run
        self.assertEqual([claims.label_of(c) for c in claims.overlap([0x1020], live)], ['#12'])
        self.assertEqual([claims.label_of(c) for c in claims.overlap([0x1040, 0x1000], live)], ['#12', 'PR #78'])
        self.assertEqual(claims.overlap([0x1010], live), [])
        self.assertEqual(claims.holders(live)['0x1040'], 'PR #78')


class Fake:
    """gh and gh_all stand-ins: canned answers by path, every call recorded."""

    def __init__(self, issues=(), pulls=(), comments=None, state=None):
        self.issues, self.pulls, self.comments, self.state = list(issues), list(pulls), comments or {}, state
        self.calls = []

    def gh_all(self, path, limit=None):
        self.calls.append(('GET*', path))
        if '/pulls?' in path:
            return self.pulls
        if '/comments' in path:
            return self.comments.get(int(path.split('/issues/')[1].split('/')[0]), [])
        return self.issues

    def gh(self, path, method='GET', fields=None, payload=None, raw=False, missing_ok=False):
        self.calls.append((method, path, payload or fields))
        if raw:
            return self.state
        if method == 'GET':
            return None
        return {'sha': 'abc', 'id': 1}


class Patched(unittest.TestCase):
    def setUp(self):
        self.saved = {k: getattr(claims, k) for k in ('gh', 'gh_all', 'repo', 'CACHE')}
        self.tmp = tempfile.mkdtemp()
        claims.CACHE = os.path.join(self.tmp, 'cache.json')
        claims.repo = lambda: 'o/r'

    def tearDown(self):
        for k, v in self.saved.items():
            setattr(claims, k, v)

    def use(self, fake):
        claims.gh, claims.gh_all = fake.gh, fake.gh_all
        return fake


class Reconcile(Patched):
    def test_issue_overlapping_a_run_pr_is_refused_and_state_written(self):
        run_body = claims.set_run_lines('', [('0x1030-0x1041', 'TK1', 'opus')])
        f = self.use(Fake(issues=[{'number': 5, 'title': 'claim: 0x1020-0x1041', 'user': {'login': 'bob'},
                                   'author_association': 'NONE', 'labels': [], 'updated_at': 'u'},
                                  {'number': 6, 'title': 'claim: app/a.cpp', 'user': {'login': 'carol'},
                                   'author_association': 'NONE', 'labels': [], 'updated_at': 'u'}],
                          pulls=[pull(78, run_body)]))
        import contextlib
        import io
        with contextlib.redirect_stdout(io.StringIO()):
            claims.reconcile(now=T0, index=index())
        posts = [c for c in f.calls if c[0] == 'POST' and c[1].endswith('/comments')]
        self.assertIn('Refused: overlaps live claim PR #78', posts[0][2]['body'])
        self.assertIn('Granted.', posts[1][2]['body'])
        tree = [c for c in f.calls if c[1].endswith('/git/trees')][0][2]['tree'][0]
        self.assertEqual(tree['path'], 'claims.json')
        doc = json.loads(tree['content'])
        self.assertEqual(sorted(claims.label_of(c) for c in doc['claims']), ['#6', 'PR #78'])
        self.assertIn(('POST', 'repos/o/r/git/refs', {'ref': 'refs/heads/claims-state', 'sha': 'abc'}),
                      f.calls)                               # the branch was missing: created
        self.assertTrue(any(c[0] == 'PATCH' and c[1] == 'repos/o/r/issues/5' for c in f.calls))   # closed


class StateFile(Patched):
    DOC = {'v': 1, 'generated': '2026-11-01T12:00:00Z',
           'claims': [{'kind': 'pr', 'pr': 78, 'holder': 'o', 'unit': '0x1000-0x1030', 'funcs': ['0x1000']}]}

    def test_one_call_then_cached_then_fresh(self):
        f = self.use(Fake(state=json.dumps(self.DOC)))
        got, doc = claims.live_claims('o/r')
        self.assertEqual((got[0]['pr'], doc['source'], len(f.calls)), (78, 'state', 1))
        self.assertEqual(f.calls[0][1], 'repos/o/r/contents/claims.json?ref=claims-state')
        claims.live_claims('o/r')
        self.assertEqual(len(f.calls), 1)                    # the cache answered
        claims.live_claims('o/r', fresh=True)
        self.assertEqual(len(f.calls), 2)

    def test_cache_expires_and_takes_grants(self):
        self.use(Fake(state=json.dumps(self.DOC)))
        claims.live_claims('o/r')
        c = claims.read_cache()
        self.assertIsNotNone(claims.cached('o/r', now=c['fetched'] + claims.CACHE_S - 1))
        self.assertIsNone(claims.cached('o/r', now=c['fetched'] + claims.CACHE_S + 1))
        self.assertIsNone(claims.cached('other/repo'))
        claims.update_cache(add=[{'kind': 'pr', 'pr': 78, 'unit': 'u2', 'funcs': ['0x2000']}])
        self.assertEqual([x['unit'] for x in claims.read_cache()['claims']], ['0x1000-0x1030', 'u2'])
        claims.update_cache(drop=lambda x: x['unit'] == '0x1000-0x1030')
        self.assertEqual([x['unit'] for x in claims.read_cache()['claims']], ['u2'])

    def test_missing_file_falls_back_to_the_scan(self):
        st = granted()
        bot = {'id': 3, 'user': {'login': claims.BOT}, 'author_association': 'NONE', 'body': claims.render(st, 'x')}
        f = self.use(Fake(state=None, issues=[{'number': 12, 'updated_at': 'u'}], comments={12: [bot]}))
        import contextlib
        import io
        err = io.StringIO()
        with contextlib.redirect_stderr(err):
            got, doc = claims.live_claims('o/r')
        self.assertIn('warning', err.getvalue())
        self.assertEqual((doc['source'], [claims.label_of(c) for c in got]), ('scan', ['#12']))
        self.assertGreater(len(f.calls), 1)

    def test_age_note(self):
        doc = dict(self.DOC, source='state', fetched=claims.parse_time(self.DOC['generated']).timestamp() + 60)
        note = claims.age_note(doc, now=doc['fetched'] + 30)
        self.assertIn('2 min ago', note)
        self.assertIn('cache 30 s old', note)


class GuardRails(unittest.TestCase):
    def test_backoff_doubles_to_a_cap(self):
        waits = []
        self.assertIsNone(claims.answer_backoff(lambda: None, budget_s=200, sleep=waits.append))
        self.assertEqual(waits, [5, 10, 20, 40, 60, 60, 60])
        n = iter([None, None, 'ok'])
        self.assertEqual(claims.answer_backoff(lambda: next(n), sleep=lambda s: None), 'ok')

    def test_budget_floor(self):
        saved, env = claims.budget, os.environ.pop('GITHUB_ACTIONS', None)
        try:
            reset = T0 + H
            claims.budget = lambda: (499, reset)
            ok, why = claims.budget_ok()
            self.assertFalse(ok)
            self.assertIn('499', why)
            self.assertIn(claims.fmt_time(reset), why)
            claims.budget = lambda: (500, reset)
            self.assertEqual(claims.budget_ok(), (True, ''))
            claims.budget = lambda: (None, None)               # rate_limit unreadable: do not block
            self.assertTrue(claims.budget_ok()[0])
            os.environ['GITHUB_ACTIONS'] = 'true'
            claims.budget = lambda: (0, reset)
            self.assertTrue(claims.budget_ok()[0])             # the Action's token has its own budget
        finally:
            claims.budget = saved
            os.environ.pop('GITHUB_ACTIONS', None)
            if env is not None:
                os.environ['GITHUB_ACTIONS'] = env

    def test_floor_checked_on_first_and_every_20th_call(self):
        saved, checks = claims.budget_ok, []
        claims.budget_ok = lambda: (checks.append(1), (True, ''))[1]
        claims._calls[0] = 0
        try:
            for _ in range(41):
                claims.budget_tick()
        finally:
            claims.budget_ok = saved
            claims._calls[0] = 0
        self.assertEqual(len(checks), 3)


class RunBranch(unittest.TestCase):
    def test_grant_refuses_overlaps_with_others_and_itself(self):
        import run_branch
        others = [claims.issue_entry({'number': 12, 'updated_at': 'u'},
                                     claims.new_state({'user': {'login': 'bob'}}, 'b', [0x1040], T0))]
        lines, refused, added = run_branch.grant(
            index(), ['0x1000-0x1021', '0x1020-0x1031', '0x1030-0x1041'], [('0x1000-0x1010', 'TK0', '')], others,
            'TK1', 'opus', 78, 'owner', now=T0)
        self.assertEqual([x[0] for x in lines], ['0x1000-0x1010', '0x1020-0x1031'])
        self.assertEqual([(u, w) for u, w in refused], [('0x1000-0x1021', 'overlaps PR #78 (TK0)'),
                                                        ('0x1030-0x1041', 'overlaps #12, PR #78 (TK1)')])
        self.assertEqual(added[0]['funcs'], ['0x1020', '0x1030'])

    def test_release_lines(self):
        import run_branch
        lines = [('0x1000-0x1010', 'TK0', ''), ('app\\a.cpp', 'TK1', 'opus'), ('0x1030-0x1041', 'TK1', '')]
        self.assertEqual(run_branch.release_lines(lines, 'TK1'), lines[:1])
        self.assertEqual(run_branch.release_lines(lines, 'app/a.cpp', 'app\\a.cpp'), [lines[0], lines[2]])


if __name__ == '__main__':
    unittest.main()
