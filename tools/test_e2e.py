"""Tests for tools/harness/e2e.py: the scenario parser, the event stream and matcher, the gen summary, verdicts.

    python -m unittest discover -s tools -p "test_*.py"

Standard library only; no client is run.
"""
import os
import re
import sys
import tempfile
import unittest

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, 'harness'))

import e2e  # noqa: E402

GADGETS = {'hud.navmap': (140, 946)}


def P(text, loader=None):
    return e2e.parse(text, 't', GADGETS, loader)


class ParserTest(unittest.TestCase):
    def test_headers_and_steps(self):
        sc = P("name: a test\nexpected-fail: Unknown_1 is a stand-in\ntimeout: 90\n"
               "wait-event antprox 'received START_ACK' within 120\nclick gadget hud.navmap\nclick 10,20\n"
               "key 0x0D\ntext 'hello world'\nalive 2.5\nshot open_1\nexpect-no CRASH\n"
               "expect-event stubs '^POC input: replayed'\n")
        self.assertEqual(sc['expected_fail'], 'Unknown_1 is a stand-in')
        self.assertEqual(sc['timeout'], 90)
        ops = [s['op'] for s in sc['steps']]
        self.assertEqual(ops, ['wait-event', 'click', 'click', 'key', 'text', 'alive', 'shot', 'expect-no',
                               'expect-event'])
        w = sc['steps'][0]
        self.assertEqual((w['source'], w['pattern'].pattern, w['within']), ('antprox', 'received START_ACK', 120))
        self.assertEqual((sc['steps'][1]['x'], sc['steps'][1]['y'], sc['steps'][1]['gadget']), (140, 946, 'hud.navmap'))
        self.assertEqual((sc['steps'][2]['x'], sc['steps'][2]['gadget']), (10, None))
        self.assertEqual(sc['steps'][3]['vk'], 13)
        self.assertEqual(sc['steps'][4]['text'], 'hello world')
        self.assertEqual(sc['steps'][5]['seconds'], 2.5)
        self.assertEqual(sc['steps'][7]['pattern'].pattern, '^CRASH:')
        self.assertEqual(sc['steps'][7]['source'], 'stubs')
        self.assertEqual(sc['steps'][8]['within'], 15.0)       # expect-event's default

    def test_stand_in_step(self):
        sc = P("stand-in '^STUB first call: POC Unknown5CE240Class'\n")
        st = sc['steps'][0]
        self.assertEqual((st['op'], st['source']), ('stand-in', 'stubs'))
        with self.assertRaises(e2e.ScenarioError):
            P("stand-in\n")

    def test_stray_step(self):
        sc = P("stray click gadget hud.navmap\nstray key 0x0D\n")
        self.assertEqual([(s['op'], s['what']) for s in sc['steps']], [('stray', 'click'), ('stray', 'key')])
        self.assertEqual((sc['steps'][0]['x'], sc['steps'][1]['vk']), (140, 13))
        for bad in ("stray\n", "stray key\n", "stray tap 1,2\n"):
            with self.assertRaises(e2e.ScenarioError, msg=bad):
                P(bad)

    def test_comments_and_blank_lines(self):
        sc = P("# a comment\n\n  alive 1  # trailing\n")
        self.assertEqual(len(sc['steps']), 1)

    def test_include_marks_own_steps(self):
        texts = {'base': "messages: on\nalive 1\nkey 13\n"}
        sc = P("include base\nalive 2\n", texts.get)
        self.assertEqual([s['op'] for s in sc['steps']], ['alive', 'key', 'alive'])
        self.assertEqual(sc['own'], 2)
        self.assertTrue(sc['messages'])                       # an included header that needs msglisten carries over

    def test_errors(self):
        for bad in ("bogus 1\n", "click gadget nowhere\n", "click 10\n", "wait-event nosuch 'x'\n",
                    "wait-event antprox '('\n", "key enter\n", "shot 'a b'\n", "text a b\n", "alive\n",
                    "unknown: header\nalive 1\n", "", "expect-no stubs\n", "wait-event antprox x within soon\n"):
            with self.assertRaises(e2e.ScenarioError, msg=bad):
                P(bad)

    def test_header_after_step_is_a_step_error(self):
        with self.assertRaises(e2e.ScenarioError):
            P("alive 1\nname: late\n")

    def test_the_repo_scenarios_parse(self):
        names = e2e.scenario_names()
        self.assertEqual(names[:6], e2e.ORDER)
        for n in names:
            sc = e2e.load(n)
            self.assertTrue(sc['steps'])

    def test_gadgets_file(self):
        d = tempfile.mkdtemp()
        p = os.path.join(d, 'g.txt')
        open(p, 'w').write("# x\na.b 1 2  # evidence\n")
        self.assertEqual(e2e.load_gadgets(p), {'a.b': (1, 2)})
        open(p, 'w').write("a.b 1\n")
        with self.assertRaises(e2e.ScenarioError):
            e2e.load_gadgets(p)


class EventsTest(unittest.TestCase):
    def test_tail_two_growing_files(self):
        d = tempfile.mkdtemp()
        a, b = os.path.join(d, 'a'), os.path.join(d, 'b')
        t = [100.0]
        ev = e2e.Events({'antprox': a, 'stubs': b}, clock=lambda: t[0])
        self.assertEqual(ev.poll(), 0)                         # missing files are fine
        open(a, 'w').write('one\r\ntw')
        t[0] = 101.5
        ev.poll()
        self.assertEqual(ev.lines, [(1.5, 'antprox', 'one')])  # a partial line waits for its end
        open(a, 'a').write('o\n')
        open(b, 'w').write('STUB first call: gen X\n')
        ev.poll()
        self.assertEqual([l[1:] for l in ev.lines], [('antprox', 'one'), ('antprox', 'two'),
                                                     ('stubs', 'STUB first call: gen X')])

    def test_find_from_mark_and_source(self):
        lines = [(0, 'antprox', 'received START_ACK'), (1, 'stubs', 'CRASH: code C0000005'),
                 (2, 'antprox', 'received START_ACK again')]
        pat = re.compile('START_ACK')
        self.assertEqual(e2e.find(lines, 'antprox', pat), 0)
        self.assertEqual(e2e.find(lines, 'antprox', pat, 1), 2)  # only lines since the mark
        self.assertIsNone(e2e.find(lines, 'stubs', pat))
        self.assertEqual(e2e.find(lines, 'any', re.compile('^CRASH:')), 1)
        self.assertIsNone(e2e.find(lines, 'msg', pat))


class SummaryTest(unittest.TestCase):
    def test_gen_lines_in_order_once(self):
        text = ['STUB first call: gen A::B', 'STUB first call: POC X', 'STUB first call: gen C',
                'STUB first call: gen A::B', 'POC input: NOT REPLAYED (dropped) 0x201 wparam 0x1 lparam 0x0']
        self.assertEqual(e2e.gen_lines(text), ['A::B', 'C'])
        self.assertEqual(len(e2e.human_lines(text)), 1)

    def test_verdicts(self):
        base = {'name': 'n', 'gen': [], 'gen_own': [], 'human_dropped': 0, 'failure': None}
        self.assertEqual(e2e.verdict(dict(base, passed=True, expected_fail=None)), 'PASS')
        self.assertEqual(e2e.verdict(dict(base, passed=False, expected_fail=None)), 'FAIL')
        self.assertEqual(e2e.verdict(dict(base, passed=False, expected_fail='stand-in',
                                          failure='stand-in hit: t:3: STUB first call: POC X')), 'XFAIL')
        # any other failure of an expected-fail scenario (a crash, a missed event) is a real FAIL
        self.assertEqual(e2e.verdict(dict(base, passed=False, expected_fail='stand-in', failure='the client exited')),
                         'FAIL')
        self.assertEqual(e2e.verdict(dict(base, passed=True, expected_fail='stand-in')), 'XPASS')
        line = e2e.summary_line(dict(base, passed=True, expected_fail=None, gen=['A', 'B'], gen_own=['B'],
                                     human_dropped=2))
        self.assertEqual(line, 'PASS  n: gen 2 hit, 1 own (B); 2 untagged (human) input dropped')


if __name__ == '__main__':
    unittest.main()
