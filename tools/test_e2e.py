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
        self.assertEqual(sc['steps'][7]['source'], 'any')        # the proxy's d3d8shot.log or a stubs.log
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


class RecordTest(unittest.TestCase):
    def test_record_rows_every_button_message(self):
        rows = []
        for line in ('input: untagged mouse 0x201 at 60, 680', 'input: untagged mouse 0x202 at 120, 610',
                     'input: untagged mouse 0x203 at 60, 680', 'input: untagged mouse 0x205 at 5, 6',
                     'input: untagged key 0x41', 'input: replayed mouse 0x201 at 1, 2'):
            m = e2e._REC_INPUT.search(line)
            if m:
                rows.append(f'key 0x{m.group(2)}' if m.group(2) else
                            e2e._REC_BUTTON[m.group(1)] + f' {m.group(3)},{m.group(4)}')
        self.assertEqual(rows, ['click 60,680', 'up 120,610', 'dblclick 60,680', 'rup 5,6', 'key 0x41'])


class CompareTest(unittest.TestCase):
    def test_sent_packets_replay_and_live(self):
        lines = ['    1.00 e2e     scenario a',
                 '    1.10 antprox [ 4] connection #2: received LOGIN 0x0002, 40 bytes',
                 '    1.20 antprox [ 4] connection #2: received KEEP_ALIVE 0x0010, 4 bytes',
                 '    2.00 e2e     scenario b',
                 '    2.01 antprox 18/11/26 08:21:01.000 [    4] Sending [0x0002] via UDP = received LOGIN',
                 '    2.02 proxy   frame 84560: mark b (no Stub_Log_Mark export)',
                 '    2.10 antprox 18/11/26 08:21:02.358 [    4] Sending [0x0027] via UDP = received INVENTORY_MOVE',
                 '    2.20 antprox 18/11/26 08:21:02.400 [    4] Sending [0x0056] via UDP = received STARBASE_AVATAR_CHANGE',
                 '    2.30 antprox 18/11/26 08:21:02.500 [    4] Sending [0x3008] via UDP = received OP_',
                 '    2.40 antprox 18/11/26 08:21:02.600 [    4] <SERVER TO CLIENT UDP> ----> 001d [0027] = sent MESSAGE_STRING']
        with tempfile.TemporaryDirectory() as d:
            with open(os.path.join(d, 'events.log'), 'w', encoding='utf-8') as f:
                f.write('\n'.join(lines) + '\n')
            self.assertEqual(e2e.sent_packets(d), {'a': ['LOGIN 40'], 'b': ['INVENTORY_MOVE', 'OP_3008']})

    def test_same_millisecond_on_two_connections_is_name_ordered(self):
        pre = '    5.00 antprox 19/11/26 07:26:51.629 [   10] antreplay: '
        for first, second in (('Master connection #1: received MASTER_JOIN 0x0035, 64 bytes',
                               'Sector connection #3: received VERB_REQUEST 0x005a, 12 bytes'),
                              ('Sector connection #3: received VERB_REQUEST 0x005a, 12 bytes',
                               'Master connection #1: received MASTER_JOIN 0x0035, 64 bytes')):
            lines = ['    1.00 e2e     scenario a',
                     '    1.10 antprox [ 4] antreplay: 07:26:50.000 Sector connection #3: received LOGIN 0x0002, 137 bytes',
                     pre + '07:26:51.629 ' + first, pre + '07:26:51.629 ' + second,
                     '    1.30 antprox [ 4] antreplay: 07:26:51.844 Sector connection #4: received LOGIN 0x0002, 137 bytes',
                     '    1.40 antprox [ 4] antreplay: 07:26:51.844 Sector connection #4: received ZZ 0x0003, 1 bytes',
                     '    1.50 antprox [ 4] antreplay: 07:26:51.844 Sector connection #4: received AA 0x0004, 1 bytes']
            with tempfile.TemporaryDirectory() as d:
                with open(os.path.join(d, 'events.log'), 'w', encoding='utf-8') as f:
                    f.write('\n'.join(lines) + '\n')
                self.assertEqual(e2e.sent_packets(d)['a'], ['LOGIN 137', 'MASTER_JOIN 64', 'VERB_REQUEST 12',
                                                            'LOGIN 137', 'ZZ 1', 'AA 1'])   # one connection: as sent


def write_bmp(path, w, h, pixel, top_down=False):
    """A 24-bit BMP whose pixel(x, y) -> (b, g, r), y counted from the top."""
    import struct
    stride = (w * 3 + 3) & ~3
    rows = []
    for y in range(h):
        row = b''.join(bytes(pixel(x, y)) for x in range(w))
        rows.append(row + b'\0' * (stride - len(row)))
    if not top_down:
        rows.reverse()
    body = b''.join(rows)
    hdr = struct.pack('<2sIHHI', b'BM', 54 + len(body), 0, 0, 54)
    info = struct.pack('<IiiHHIIiiII', 40, w, -h if top_down else h, 1, 24, 0, len(body), 0, 0, 0, 0)
    with open(path, 'wb') as f:
        f.write(hdr + info + body)


def write_run(d, name, verdict_passed, packets, shots, failure=None):
    """A run folder as e2e writes it: events.log, e2e.log, result-NAME.json, shot_*.bmp."""
    import json
    os.makedirs(d, exist_ok=True)
    with open(os.path.join(d, 'events.log'), 'w', encoding='utf-8') as f:
        f.write(f'    1.00 e2e     scenario {name}\n')
        for i, p in enumerate(packets):
            f.write(f'    1.{i + 10} antprox [ 4] connection #2: received {p} 0x0002, 40 bytes\n')
    with open(os.path.join(d, 'e2e.log'), 'w', encoding='utf-8') as f:
        f.write(f'07:00:00 scenario {name}\n')
        for i, (shot, pixel) in enumerate(shots):
            f.write(f'07:00:01 step {name}:{i}: shot {shot}\n07:00:01   shot {shot} taken\n')
            write_bmp(os.path.join(d, f'shot_{shot}_{100 + i:05d}.bmp'), 32, 32, pixel)
    with open(os.path.join(d, f'result-{name}.json'), 'w', encoding='utf-8') as f:
        json.dump({'name': name, 'passed': verdict_passed, 'expected_fail': None, 'failure': failure,
                   'out': d}, f)


class ShotCompareTest(unittest.TestCase):
    def test_bmp_bottom_up_and_top_down(self):
        with tempfile.TemporaryDirectory() as d:
            for td in (False, True):
                p = os.path.join(d, f'a{td}.bmp')
                write_bmp(p, 5, 3, lambda x, y: (x, y, 7), top_down=td)
                w, h, n, rows = e2e.read_bmp(p)
                self.assertEqual((w, h, n), (5, 3, 3))
                self.assertEqual(rows[2][3 * 4:3 * 4 + 3], bytes((4, 2, 7)))

    def test_png_as_session_close_writes_it(self):
        import bmp2png
        with tempfile.TemporaryDirectory() as d:
            b = os.path.join(d, 's.bmp')
            write_bmp(b, 6, 4, lambda x, y: (x, y, 9))                   # (b, g, r)
            w, h, rows = bmp2png.read_bmp(b)
            bmp2png.write_png(os.path.join(d, 's.png'), w, h, rows)
            pw = e2e.read_shot(os.path.join(d, 's.png'))
            self.assertEqual(pw, e2e.read_shot(b))                       # both RGB
            self.assertEqual(pw[3][3][5 * 3:5 * 3 + 3], bytes((9, 3, 5)))
            self.assertEqual(e2e.shot_diff(b, os.path.join(d, 's.png'), grid=2, step=1)['differ'], 0)

    def test_shot_diff_cells_and_tolerance(self):
        with tempfile.TemporaryDirectory() as d:
            a, b, c = (os.path.join(d, n) for n in ('a.bmp', 'b.bmp', 'c.bmp'))
            write_bmp(a, 32, 32, lambda x, y: (10, 10, 10))
            write_bmp(b, 32, 32, lambda x, y: (200, 10, 10) if x >= 24 and y < 8 else (12, 10, 10))
            write_bmp(c, 16, 32, lambda x, y: (10, 10, 10))
            self.assertEqual(e2e.shot_diff(a, a, grid=4, step=1)['differ'], 0)
            r = e2e.shot_diff(a, b, grid=4, tol=16, step=1)
            self.assertEqual((r['differ'], r['box']), (1, (24, 0, 32, 8)))   # the +2 elsewhere is under tol
            self.assertIn('size', e2e.shot_diff(a, c, grid=4))

    def test_compare_runs_same_and_diff(self):
        grey = lambda x, y: (50, 50, 50)
        red = lambda x, y: (50, 50, 250) if x < 16 else (50, 50, 50)
        with tempfile.TemporaryDirectory() as d:
            ours, ret = os.path.join(d, 'o'), os.path.join(d, 'r')
            write_run(ours, 'a', True, ['LOGIN', 'TALK'], [('x', grey)])
            write_run(ret, 'a', True, ['LOGIN', 'TALK'], [('x', grey)])
            write_run(ours + '2', 'b', False, ['LOGIN'], [('y', red)], failure='expect-event: not seen')
            write_run(ret + '2', 'b', True, ['LOGIN', 'TALK'], [('y', grey)])
            rows = e2e.compare_runs([ours, ours + '2'], [ret, ret + '2'])
            self.assertEqual([(r[0], r[1]) for r in rows], [('a', 'SAME'), ('b', 'DIFF')])
            self.assertEqual(rows[0][2], '2 packets, shots 1 of 1 alike')
            self.assertIn('ours FAIL (expect-event: not seen), retail PASS', rows[1][2])
            self.assertIn('packets ours 1, retail 2 (first -TALK 40)', rows[1][2])
            self.assertIn('shots y ', rows[1][2])

    def test_mask_rules(self):
        with tempfile.TemporaryDirectory() as d:
            m = os.path.join(d, 'mask.txt')
            with open(m, 'w') as f:
                f.write('# x\n* 1,2-3,4  # planet\nw_entered skip\nw_stopped cells\nw_* mean 8\n')
            mask = e2e.load_shot_mask(m)
            self.assertEqual(e2e.mask_for('w_entered', mask), ([(1, 2, 3, 4)], ('skip',)))
            self.assertEqual(e2e.mask_for('w_stopped', mask), ([(1, 2, 3, 4)], None))
            self.assertEqual(e2e.mask_for('w_cargo', mask), ([(1, 2, 3, 4)], ('mean', 8.0)))
            self.assertEqual(e2e.mask_for('hot_keys', mask), ([(1, 2, 3, 4)], None))
            with open(m, 'w') as f:
                f.write('w_* bright\n')
            self.assertRaises(ValueError, e2e.load_shot_mask, m)
            self.assertEqual(len(e2e.load_shot_mask()), len([1 for l in open(e2e.SHOT_MASK)
                                                             if l.split('#')[0].strip()]))   # the shipped mask parses

    def test_mean_rule_in_compare(self):
        dark = lambda x, y: (40, 40, 40)
        bright = lambda x, y: (52, 52, 52)
        with tempfile.TemporaryDirectory() as d:
            ours, ret = os.path.join(d, 'o'), os.path.join(d, 'r')
            write_run(ours, 'play1', True, [], [('play1_20s', dark), ('play1_stopped', dark), ('w_entered', dark)])
            write_run(ret, 'play1', True, [], [('play1_20s', bright), ('play1_stopped', dark), ('w_entered', bright)])
            rows = e2e.compare_runs([ours], [ret])
            self.assertEqual(rows[0][1], 'DIFF')
            self.assertEqual(rows[0][2], 'shots play1_20s brightness 40 vs 52')

    def test_run_dirs_manifest(self):
        import json
        with tempfile.TemporaryDirectory() as d:
            m = os.path.join(d, 'm.json')
            with open(m, 'w') as f:
                json.dump({'scenarios': [{'name': 'a', 'out': 'X'}, {'name': 'b', 'out': 'X'},
                                         {'name': 'c', 'out': 'Y'}, {'name': 'd', 'out': None}]}, f)
            self.assertEqual(e2e.run_dirs(m), ['X', 'Y'])
            self.assertEqual(e2e.run_dirs('P,Q'), ['P', 'Q'])


class DbSnapshotTest(unittest.TestCase):
    def test_dump_command(self):
        argv = e2e.db_dump_cmd(6, ('avatar_level_info', 'avatar_inventory_items'))
        self.assertEqual(argv[:6], ['docker', 'exec', '-i', 'net7-mysql', 'sh', '-c'])
        self.assertIn('-p"$MYSQL_ROOT_PASSWORD"', argv[6])
        self.assertIn('--no-create-info', argv[6])
        self.assertIn('--where="avatar_id=6"', argv[6])
        self.assertTrue(argv[6].endswith('net7_user avatar_level_info avatar_inventory_items'))
        self.assertNotIn('--add-drop-table', argv[6])

    def test_restore_script_touches_only_the_avatars_rows(self):
        sql = e2e.db_restore_sql(6, "INSERT INTO `avatar_level_info` (`avatar_id`) VALUES (6);\n",
                                 ('avatar_level_info', 'avatar_info'))
        lines = sql.splitlines()
        self.assertEqual(lines[:3], ['SET FOREIGN_KEY_CHECKS=0;', 'SET autocommit=0;', 'START TRANSACTION;'])
        self.assertEqual(lines[3:5], ['DELETE FROM `avatar_level_info` WHERE `avatar_id` = 6;',
                                      'DELETE FROM `avatar_info` WHERE `avatar_id` = 6;'])
        self.assertEqual(lines[-2:], ['COMMIT;', 'SET FOREIGN_KEY_CHECKS=1;'])
        self.assertFalse(re.search(r'\b(DROP|ALTER|TRUNCATE|CREATE)\b', sql))

    def test_default_tables_are_keyed_by_avatar(self):
        self.assertIn('avatar_inventory_items', e2e.DB_TABLES)
        self.assertIn('avatar_level_info', e2e.DB_TABLES)       # the credits
        self.assertNotIn('avatar_data', e2e.DB_TABLES)          # identity, never restored
        self.assertNotIn('accounts', e2e.DB_TABLES)

    def test_option_needs_live(self):
        self.assertEqual(e2e.main(['run', 'sb_buysell', '--db-snapshot']), 2)



class FakeNet7:
    """A loopback UDP responder standing in for Net7.exe's control port: REPLY(cmd) -> the reply text, or None
    (no reply); every command received is kept in .got."""
    def __init__(self, reply):
        import socket
        import threading
        self.reply, self.got = reply, []
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sock.bind(('127.0.0.1', 0))
        self.sock.settimeout(0.1)
        self.port = self.sock.getsockname()[1]
        self.stop = False
        self.thread = threading.Thread(target=self.serve, daemon=True)
        self.thread.start()

    def serve(self):
        while not self.stop:
            try:
                data, addr = self.sock.recvfrom(512)
            except OSError:
                continue
            cmd = data.decode()
            self.got.append(cmd)
            r = self.reply(cmd)
            if r is not None:
                self.sock.sendto(r.encode(), addr)

    def close(self):
        self.stop = True
        self.thread.join(1)
        self.sock.close()


class Net7ControlTest(unittest.TestCase):
    READY = 'READY players=0 saves=idle ssl=1 quiesced=0'

    def setUp(self):
        self.saved = {k: getattr(e2e, k) for k in ('_db_run', 'net7_stop', 'net7_start', '_udp_bound',
                                                   'net7_control_port', 'NET7_SAVE_WAIT')}
        self.calls = []
        e2e._udp_bound = lambda port: False
        e2e.net7_stop = lambda b: self.calls.append('stop')
        e2e.net7_start = lambda b, within=240, port=None: self.calls.append('start') or True

    def tearDown(self):
        for k, v in self.saved.items():
            setattr(e2e, k, v)
        if getattr(self, 'fake', None):
            self.fake.close()

    def serve(self, reply):
        self.fake = FakeNet7(reply)
        e2e.net7_control_port = lambda net7bin=None: self.fake.port
        return self.fake

    def scripts(self, n=3):
        d = tempfile.mkdtemp()
        out = []
        for i in range(n):
            out.append(os.path.join(d, f's{i}.sql'))
            with open(out[-1], 'w', encoding='utf-8') as f:
                f.write(f'-- script {i}\n')
        return out

    def test_status_parse(self):
        st = e2e.net7_status(self.READY)
        self.assertEqual((st['state'], st['saves'], st['quiesced']), ('READY', 'idle', '0'))
        self.assertIsNone(e2e.net7_status(None))

    def test_ready_needs_quiesced_0(self):
        state = {'q': '1'}
        self.serve(lambda c: f'READY players=0 saves=idle ssl=1 quiesced={state["q"]}')
        self.assertFalse(e2e.net7_ready(self.fake.port))
        state['q'] = '0'
        self.assertTrue(e2e.net7_ready(self.fake.port))

    def test_in_place_failure_names_the_script(self):
        f = self.serve(lambda c: {'STATUS': self.READY, 'QUIESCE': 'QUIESCED', 'RESUME': 'RESUMED'}.get(c))
        paths = self.scripts()

        def run(argv, stdin=None):
            if 'script 1' in stdin:
                raise RuntimeError('mysql: exit 1: ERROR 1062')
            self.calls.append(stdin.strip())
            return ''
        e2e._db_run = run
        logs, info = [], {}
        ok = e2e.db_restore(paths, 'x', logs.append, tag='t', info=info)
        self.assertFalse(ok)
        self.assertEqual(self.calls, ['-- script 0'])           # stopped at the failure, nothing after it
        self.assertTrue(any('FAILED at ' + paths[1] in m and '1 of 3' in m for m in logs), logs)
        self.assertIn('RESUME', f.got)                          # logins allowed again
        self.assertEqual((info['restore'], info['failed']), ('in-place', paths[1]))

    def test_in_place_ok_records_info(self):
        self.serve(lambda c: {'STATUS': self.READY, 'QUIESCE': 'QUIESCED', 'RESUME': 'RESUMED'}.get(c))
        e2e._db_run = lambda argv, stdin=None: ''
        info = {}
        self.assertTrue(e2e.db_restore(self.scripts(2), 'x', lambda m: None, info=info))
        self.assertEqual(info['restore'], 'in-place')
        self.assertEqual(info['status'], self.READY)
        self.assertEqual(info['control_port'], self.fake.port)
        self.assertNotIn('stop', self.calls)

    def test_quiesce_failure_resumes_then_waits_for_saves(self):
        state = {'busy': 2}

        def reply(c):
            if c == 'STATUS':
                if self.fake.got.count('QUIESCE') and state['busy']:
                    state['busy'] -= 1
                    return 'READY players=1 saves=busy ssl=1 quiesced=1'
                return 'READY players=1 saves=idle ssl=1 quiesced=0'
            return {'QUIESCE': 'ERROR timeout players=1 saves=busy', 'RESUME': 'RESUMED'}.get(c)
        f = self.serve(reply)
        e2e._db_run = lambda argv, stdin=None: self.calls.append('sql') or ''
        info = {}
        self.assertTrue(e2e.db_restore(self.scripts(1), 'x', lambda m: None, info=info))
        q = f.got.index('QUIESCE')
        self.assertEqual(f.got[q + 1], 'RESUME')
        self.assertGreaterEqual(f.got[q + 2:].count('STATUS'), 3)  # busy, busy, idle before the stop
        self.assertEqual(self.calls, ['stop', 'sql', 'start'])
        self.assertEqual(info['restore'], 'restart')

    def test_busy_saves_wait_is_bounded(self):
        self.serve(lambda c: 'READY players=1 saves=busy ssl=1 quiesced=0')
        t0 = __import__('time').time()
        idle, last = e2e.net7_wait_saves(self.fake.port, within=0.6)
        self.assertFalse(idle)
        self.assertIn('saves=busy', last)
        self.assertLess(__import__('time').time() - t0, 3)

    def test_control_port_from_cfg_and_machine(self):
        e2e.net7_control_port = self.saved['net7_control_port']
        d = tempfile.mkdtemp()
        old = os.environ.pop('ENB_NET7_CONTROL_PORT', None)
        try:
            self.assertEqual(e2e.net7_control_port(d), 3810)          # no cfg
            with open(os.path.join(d, 'Net7Config.cfg'), 'w') as fo:
                fo.write('domain=localhost\r\ncontrol_port=3911\r\nmysql_user=net7\r\n')
            self.assertEqual(e2e.net7_control_port(d), 3911)
            os.environ['ENB_NET7_CONTROL_PORT'] = '4000'
            self.assertEqual(e2e.net7_control_port(d), 4000)
        finally:
            os.environ.pop('ENB_NET7_CONTROL_PORT', None)
            if old is not None:
                os.environ['ENB_NET7_CONTROL_PORT'] = old

    def test_port_0_means_off(self):
        self.assertIsNone(e2e.net7_control('STATUS', port=0))


class CameraTrackTest(unittest.TestCase):
    """--camera-track: each new frame of the proxy's camera file once, every view of it."""

    @unittest.skipUnless(sys.platform == 'win32', 'GetTickCount')
    def test_track_appends_new_frames_with_all_views(self):
        import time
        with tempfile.TemporaryDirectory() as d:
            cam, out = os.path.join(d, 'camera.txt'), os.path.join(d, 'camera-track.txt')
            m = ' '.join(['1.00000', '0.00000', '0.00000', '0.00000'] * 4)

            def write(frame):
                with open(cam + '.tmp', 'w') as f:
                    f.write(f'frame {frame} view 0 of 3: {m}\nframe {frame} view 1 of 3: {m}\n')
                os.replace(cam + '.tmp', cam)
            write(10)
            tr = e2e.CameraTrack(cam, out, time.time())
            time.sleep(0.15)
            write(12)
            time.sleep(0.15)
            tr.stop()
            with open(out) as f:
                lines = [ln.split() for ln in f]
            self.assertEqual([ln[2] for ln in lines], ['10', '12'])
            self.assertTrue(all(ln[3] == '2' and len(ln) == 4 + 32 for ln in lines))


if __name__ == '__main__':
    unittest.main()
