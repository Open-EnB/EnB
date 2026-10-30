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


if __name__ == '__main__':
    unittest.main()
