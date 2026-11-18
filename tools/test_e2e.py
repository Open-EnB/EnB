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

    def test_wait_quiet_and_click_until(self):
        sc = P("wait-quiet files 1.5\nwait-quiet proxy 0.5 within 9\n"
               "click 640,512 until antprox 'VERSION_RESPONSE' every 0.25 within 100\n"
               "click 1,2 until msg x redo 1\n")
        q, q2, c, c2 = sc['steps']
        self.assertEqual((q['source'], q['quiet'], q['within']), ('proxy', 1.5, 30.0))      # files: the proxy's log
        self.assertEqual((q2['quiet'], q2['within']), (0.5, 9.0))
        self.assertEqual((c['x'], c['until']['source'], c['until']['every'], c['until']['within'], c['until']['redo']),
                         (640, 'antprox', 0.25, 100.0, 0))
        self.assertEqual((c2['until']['every'], c2['until']['within'], c2['until']['redo']), (0.5, 60.0, 1))
        for bad in ("wait-quiet proxy\n", "wait-quiet any 1\n", "wait-quiet proxy soon\n", "click 1,2 until msg\n"):
            with self.assertRaises(e2e.ScenarioError, msg=bad):
                P(bad)

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
        self.assertEqual(names[len(names) - len(e2e.LAST):], e2e.LAST)
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

    def test_bounded_window_streaming_log_and_watches(self):
        d = tempfile.mkdtemp()
        a = os.path.join(d, 'antprox.out')
        p = os.path.join(d, 'd3d8shot.log')
        log = os.path.join(d, 'events.log')
        t = [10.0]
        ev = e2e.Events({'antprox': a, 'proxy': p}, clock=lambda: t[0], log=log, window=3)
        try:
            with open(p, 'w', encoding='utf-8') as f:
                f.write('         1 tagged input read\n')
            ev.poll()
            self.assertTrue(e2e._has_tag_hook(ev))
            w_crash = ev.watch('antprox', re.compile(r'^line 2$'), 0)
            with open(a, 'w', encoding='utf-8') as f:
                for i in range(6):
                    f.write(f'line {i}\n')
            t[0] = 12.5
            ev.poll()
            self.assertEqual(len(ev.lines), 3)
            self.assertEqual(ev.total, 7)
            self.assertEqual(ev.base, 4)
            self.assertEqual([x[2] for x in ev.lines], ['line 3', 'line 4', 'line 5'])
            self.assertTrue(e2e._has_tag_hook(ev))              # kept even after evicted from window
            self.assertIsNotNone(w_crash.hit)                   # watch caught line 2 in the same burst
            self.assertEqual(w_crash.hit[2], 'line 2')
            with open(log, encoding='utf-8') as f:
                logged = f.read().splitlines()                  # streamed before close()
            self.assertEqual(len(logged), 7)
            self.assertIn('tagged input read', logged[0])
            self.assertIn('line 5', logged[-1])
        finally:
            ev.close()

    def test_fold_repeated_asserts_across_timestamps_and_epochs(self):
        d = tempfile.mkdtemp()
        m = os.path.join(d, 'messages.log')
        s = os.path.join(d, 'stubs.log')
        log = os.path.join(d, 'events.log')
        t = [0.0]
        ev = e2e.Events({'msg': m, 'stubs': s}, clock=lambda: t[0], log=log, window=100)
        try:
            with open(m, 'w', encoding='utf-8') as f:
                f.write('01:00:00.000001 ERROR G_ASSERT: ASSERTION FAILED: a  effect_description.cpp(266)\n')
                f.write('01:00:00.000002 ERROR FAIL_IF: ERROR: b  effect_description.cpp(318)\n')
                f.write('01:00:00.000003 ERROR G_ASSERT: ASSERTION FAILED: a  effect_description.cpp(266)\n')
                f.write('01:00:00.000004 ERROR FAIL_IF: ERROR: b  effect_description.cpp(318)\n')
                f.write('01:00:00.000005 INFO  Render: Frame Rate: 60\n')
            t[0] = 1.0
            ev.poll()
            self.assertEqual(len(ev.lines), 3)
            self.assertEqual(ev.last['msg'], 1.0)
            with open(m, 'a', encoding='utf-8') as f:
                f.write('01:00:01.000001 ERROR G_ASSERT: ASSERTION FAILED: a  effect_description.cpp(266)\n')
            t[0] = 2.0
            ev.poll()
            self.assertEqual(len(ev.lines), 3)                  # repeat folded, not appended
            self.assertEqual(ev.last['msg'], 2.0)               # wait-quiet still sees activity
            ev.mark_epoch()                                     # new input step / expect-no allows next firing
            with open(m, 'a', encoding='utf-8') as f:
                f.write('01:00:02.000001 ERROR G_ASSERT: ASSERTION FAILED: a  effect_description.cpp(266)\n')
            t[0] = 3.0
            ev.poll()
            self.assertEqual(len(ev.lines), 4)
            ev.close()
            with open(log, encoding='utf-8') as f:
                text = f.read()
            self.assertIn('[repeated 2x]', text)
            self.assertIn('[repeated 1x]', text)
        finally:
            ev.close()


class SummaryTest(unittest.TestCase):
    def test_gen_lines_in_order_once(self):
        text = ['STUB first call: gen A::B', 'STUB first call: POC X', 'STUB first call: gen C',
                'STUB first call: gen A::B', 'input: NOT REPLAYED (dropped) 0x201 wparam 0x1 lparam 0x0']
        self.assertEqual(e2e.gen_lines(text), ['A::B', 'C'])
        self.assertEqual(len(e2e.human_lines(text)), 1)

    def test_crash_place_ignores_the_data_address(self):
        a = {'crash': ['CRASH: code C0000005 at 00400007 (address 1EF08400) stack: 004EBDA5 004EB915 0048812A 0052A1E7 004BABDF']}
        b = {'crash': ['CRASH: code C0000005 at 00400007 (address 1ED5C600) stack: 004EBDA5 004EB915 0048812A 0052A1E7 006BD100']}
        c = {'crash': ['CRASH: code C0000005 at 00400007 (address 1ED5C600) stack: 004EBDA5 004EB915 0048812A 0052A1E0']}
        self.assertEqual(e2e.crash_place(a), e2e.crash_place(b))
        self.assertNotEqual(e2e.crash_place(a), e2e.crash_place(c))
        self.assertIsNone(e2e.crash_place({'crash': []}))

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
        # only a PASS keeps the client for the next scenario: an XFAIL's stand-in left the world changed
        self.assertTrue(e2e.keeps_session(dict(base, passed=True, expected_fail=None)))
        self.assertFalse(e2e.keeps_session(dict(base, passed=False, expected_fail='stand-in',
                                                failure='stand-in hit: t:3: STUB first call: POC X')))
        self.assertFalse(e2e.keeps_session(dict(base, passed=False, expected_fail=None)))
        line = e2e.summary_line(dict(base, passed=True, expected_fail=None, gen=['A', 'B'], gen_own=['B'],
                                     human_dropped=2))
        self.assertEqual(line, 'PASS  n: gen 2 hit, 1 own (B); 2 untagged (human) input dropped')


class CompareTest(unittest.TestCase):
    def test_sent_packets_replay_and_live(self):
        lines = ['    1.00 e2e     scenario a',
                 '    1.10 antprox [ 4] connection #2: received LOGIN 0x0002, 40 bytes',
                 '    1.20 antprox [ 4] connection #2: received KEEP_ALIVE 0x0010, 4 bytes',
                 '    2.00 e2e     scenario b',
                 '    2.10 antprox 18/11/26 08:21:02.358 [    4] Sending [0x0027] via UDP = received INVENTORY_MOVE',
                 '    2.20 antprox 18/11/26 08:21:02.400 [    4] Sending [0x0056] via UDP = received STARBASE_AVATAR_CHANGE',
                 '    2.30 antprox 18/11/26 08:21:02.500 [    4] Sending [0x3008] via UDP = received OP_',
                 '    2.40 antprox 18/11/26 08:21:02.600 [    4] <SERVER TO CLIENT UDP> ----> 001d [0027] = sent MESSAGE_STRING']
        with tempfile.TemporaryDirectory() as d:
            with open(os.path.join(d, 'events.log'), 'w', encoding='utf-8') as f:
                f.write('\n'.join(lines) + '\n')
            self.assertEqual(e2e.sent_packets(d), {'a': ['LOGIN 40'], 'b': ['INVENTORY_MOVE', 'OP_3008']})


if __name__ == '__main__':
    unittest.main()
