"""Modifier keys and chords in the scenario language and the poster (test-plan C6, IN-01).

    python -m unittest tools/harness/test_keys.py      (from the repository root; Windows: MapVirtualKeyW)
"""
import os
import sys
import unittest

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import clientrun  # noqa: E402
import e2e  # noqa: E402

KD, KU, SKD, SKU = 0x100, 0x101, 0x104, 0x105
SCAN = {0x10: 0x2A, 0x11: 0x1D, 0x12: 0x38, 0x49: 0x17, 0x46: 0x21, 0x70: 0x3B, 0x26: 0x48}


def lp(vk, up=False, alt=False, ext=False, repeat=False):
    v = 1 | (SCAN[vk] << 16) | ((1 << 24) if ext else 0) | ((1 << 29) if alt else 0)
    if up:
        v |= 3 << 30
    elif repeat:
        v |= 1 << 30
    return v


def scenario(text):
    return e2e.parse('timeout: 10\n' + text, 't')


class ParseKeys(unittest.TestCase):
    def test_numbers_stay_vks(self):
        self.assertEqual(clientrun.parse_keys('0x49'), [0x49])
        self.assertEqual(clientrun.parse_keys('13'), [13])

    def test_names_and_chords(self):
        self.assertEqual(clientrun.parse_keys('shift+i'), [0x10, 0x49])
        self.assertEqual(clientrun.parse_keys('Ctrl+Alt+F1'), [0x11, 0x12, 0x70])
        self.assertEqual(clientrun.parse_keys('alt+0x70'), [0x12, 0x70])
        self.assertEqual(clientrun.parse_keys('enter'), [0x0D])
        self.assertEqual(clientrun.parse_keys('`'), [0xC0])
        self.assertEqual(clientrun.parse_keys('num5'), [0x65])

    def test_bad(self):
        for bad in ('shift+', 'shift+shift', 'foo', '0x100', 'ctrl+0', ''):
            with self.assertRaises(ValueError, msg=bad):
                clientrun.parse_keys(bad)

    def test_name_round_trip(self):
        for spec in ('shift+i', 'ctrl+alt+f1', 'enter', 'up', '0x5B'):
            vks = clientrun.parse_keys(spec)
            self.assertEqual(clientrun.parse_keys('+'.join(clientrun.key_name(v) for v in vks)), vks)


class Messages(unittest.TestCase):
    def test_plain_key_unchanged(self):
        """A plain key's messages are what `key VK` always posted (stored runs stay comparable)."""
        got = clientrun.key_messages(clientrun.chord_edges([0x49]), set())
        self.assertEqual(got, [(KD, 0x49, clientrun.key_lparam(0x49)), (KU, 0x49, clientrun.key_lparam(0x49, True))])
        self.assertEqual(got, [(KD, 0x49, lp(0x49)), (KU, 0x49, lp(0x49, up=True))])

    def test_shift_chord(self):
        held = set()
        got = clientrun.key_messages(clientrun.chord_edges([0x10, 0x49]), held)
        self.assertEqual(got, [(KD, 0x10, lp(0x10)), (KD, 0x49, lp(0x49)), (KU, 0x49, lp(0x49, up=True)),
                               (KU, 0x10, lp(0x10, up=True))])
        self.assertEqual(held, set())

    def test_ctrl_chord(self):
        got = clientrun.key_messages(clientrun.chord_edges([0x11, 0x49]), set())
        self.assertEqual([m for m, _, _ in got], [KD, KD, KU, KU])

    def test_alt_chord_is_sys(self):
        """Alt+F1: WM_SYSKEYDOWN with the context bit 29 while Alt is down; Alt's own up WM_SYSKEYUP, bit 29 clear."""
        got = clientrun.key_messages(clientrun.chord_edges([0x12, 0x70]), set())
        self.assertEqual(got, [(SKD, 0x12, lp(0x12, alt=True)), (SKD, 0x70, lp(0x70, alt=True)),
                               (SKU, 0x70, lp(0x70, up=True, alt=True)), (SKU, 0x12, lp(0x12, up=True))])

    def test_ctrl_alt_is_not_sys(self):
        got = clientrun.key_messages(clientrun.chord_edges([0x11, 0x12, 0x46]), set())
        self.assertEqual([m for m, _, _ in got], [KD, KD, KD, KU, KU, KU])
        self.assertTrue(got[2][2] & (1 << 29))          # F with Alt down: the context bit still set

    def test_held_across_steps(self):
        held = set()
        clientrun.key_messages([(0x10, False)], held)
        self.assertEqual(held, {0x10})
        got = clientrun.key_messages(clientrun.chord_edges([0x49]), held)
        self.assertEqual(got, [(KD, 0x49, lp(0x49)), (KU, 0x49, lp(0x49, up=True))])
        self.assertEqual(held, {0x10})
        got = clientrun.key_messages([(0x10, False)], held)          # again while down: a keyboard's auto-repeat
        self.assertEqual(got, [(KD, 0x10, lp(0x10, repeat=True))])
        clientrun.key_messages([(0x10, True)], held)
        self.assertEqual(held, set())

    def test_extended_arrow_with_shift(self):
        got = clientrun.key_messages(clientrun.chord_edges([0x10, 0x26]), set())
        self.assertEqual(got[1], (KD, 0x26, lp(0x26, ext=True)))


class Scenario(unittest.TestCase):
    def test_key_chord(self):
        st = scenario('key shift+i\n')['steps'][0]
        self.assertEqual((st['keys'], st['vk'], st['spec']), ([0x10, 0x49], 0x49, 'shift+i'))
        self.assertEqual(e2e._desc(st), 'shift+i')

    def test_key_until(self):
        st = scenario("key ctrl+i until proxy 'x' every 1 within 5\n")['steps'][0]
        self.assertEqual(st['keys'], [0x11, 0x49])
        self.assertEqual(st['until']['within'], 5.0)

    def test_old_forms(self):
        sc = scenario('key 0x4F\nhold 0x41 for 1\nstray key 0x0D\nhold up for 0.5\n')
        self.assertEqual([s.get('vk') for s in sc['steps']], [0x4F, 0x41, 0x0D, 0x26])

    def test_held_pairs(self):
        sc = scenario('keydown shift\nkey i\nkeyup shift\n')
        self.assertEqual([s['op'] for s in sc['steps']], ['keydown', 'key', 'keyup'])
        scenario('keydown alt\nkey f1\nkeyup alt\n')
        scenario('keydown shift+ctrl\nkey i\nkeyup ctrl+shift\n')

    def test_held_errors(self):
        for text in ('keydown shift\nkey i\n',                  # never let go
                     'keyup shift\n',                           # not down
                     'keydown shift\nkeydown shift\nkeyup shift\n',
                     'keydown shift\nkey shift+i\nkeyup shift\n',
                     'key alt\n',                               # a lone Alt: the window menu
                     'key i+alt\n',
                     'keydown alt\nkeyup alt\n',
                     'key alt+space\n',                         # the system menu (D69)
                     'keydown alt\nkey space\nkeyup alt\n',
                     'key shift+nothing\n',
                     'keydown\n'):
            with self.assertRaises(e2e.ScenarioError, msg=text):
                scenario(text)

    def test_scenarios_parse(self):
        for name in [n[:-4] for n in os.listdir(e2e.SCN) if n.endswith('.scn')]:
            e2e.load(name)


if __name__ == '__main__':
    unittest.main()
