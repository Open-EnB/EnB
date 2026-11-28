"""inlineprof.py on made rows (no VM, no object): decision paths per value, cross-jumps, the strings and distances."""
import os
import sys
import types
import unittest

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import inlineprof as P  # noqa: E402


def ins(addr, text, target=None):
    mn, _, op = text.partition(' ')
    return types.SimpleNamespace(addr=addr, mn=mn, op=op, raw=text, target=target), text


def rows():
    """A back-out site at 0x01 (cmp eax, 1 after the member read): 1 -> call 0xA, 0x17 -> call 0xB, other -> 0xC
    through a cross-jump; a second site at 0x20 whose other path calls an unlettered 0xD."""
    return [ins(0x00, 'mov eax, dword ptr [esi + 0xd4]'),
            ins(0x01, 'cmp eax, 1'),
            ins(0x02, 'jne 0x5', ('addr', 0x05)),
            ins(0x03, 'call 0xa', ('addr', 0xA)),
            ins(0x04, 'jmp 0x40', ('addr', 0x40)),
            ins(0x05, 'cmp eax, 0x17'),
            ins(0x06, 'jne 0x8', ('addr', 0x08)),
            ins(0x07, 'call 0xb', ('addr', 0xB)),
            ins(0x08, 'jmp 0x30', ('addr', 0x30)),
            ins(0x1F, 'mov eax, dword ptr [edi + 0xd4]'),
            ins(0x20, 'cmp eax, 1'),
            ins(0x21, 'je 0x40', ('addr', 0x40)),
            ins(0x22, 'test eax, eax'),
            ins(0x23, 'je 0x26', ('addr', 0x26)),
            ins(0x24, 'call 0xd', ('addr', 0xD)),
            ins(0x26, 'call 0xb', ('addr', 0xB)),
            ins(0x30, 'call 0xc', ('addr', 0xC)),
            ins(0x40, 'ret')]


class Profile(unittest.TestCase):
    def prof(self, **k):
        opts = dict(letters={0xA: 'a', 0xB: 'b', 0xC: 'c'}, site=r'^cmp (e[acd]x), 1$',
                    near=r'^mov eax, dword ptr \[e\w\w \+ 0xd4\]$', values=[P.OTHER, 0x17, 0])
        opts.update(k)
        return P.Profiler(**opts)

    def test_paths(self):
        p = self.prof()
        got = p.sites(rows())
        self.assertEqual([a for a, _, _ in got], [0x01, 0x20])
        self.assertEqual(got[0][1], 'cbc')        # other: cross-jumped to 0xC; 0x17: 0xB; 0: like other
        self.assertEqual(got[1][1], '??b')        # other / 0x17: unlettered 0xD; 0: test eax, eax taken
        self.assertEqual(got[1][2], 'a b')        # the calls before it, by letter
        self.assertEqual(p.site_string(rows()), 'c?/b?/cb')
        self.assertEqual(p.calls(rows()), 'abbc')

    def test_near_and_memory_operand(self):
        self.assertEqual(self.prof(near=r'^nothing$').sites(rows()), [])
        r = rows()
        r[5] = ins(0x05, 'cmp eax, dword ptr [ebx]')
        self.assertEqual(self.prof().sites(r)[0][1], '???')      # the jne after a memory compare ends the path

    def test_parse_and_dist(self):
        self.assertEqual(P.parse_map('0x6047D0=T,0x606150=t'), {0x6047D0: 'T', 0x606150: 't'})
        self.assertEqual(P.parse_map('Get_Selected@UIComboBox=0x5FFFE0', int), {'Get_Selected@UIComboBox': 0x5FFFE0})
        self.assertEqual(P.parse_values('other,0x17'), [P.OTHER, 0x17])
        self.assertEqual(P.dist('9898', '9898'), 0)
        self.assertEqual(P.dist('98', '988'), 1)
        self.assertEqual(P.dist('', '66'), 2)
        lines = P.profile_lines([('f1', '98', '98', 'T', 'T', 3, 4), ('f2', '99', '98', 'T', 'T', 1, 2)])
        self.assertEqual(lines[0].split(), ['CLIENT', '98', 'T'])
        self.assertTrue(lines[1].startswith('f1') and 'd=0+0' in lines[1])

    def test_unknown_name_key(self):
        p = self.prof()
        r = ins(0x50, 'call 0', ('sym', '?Unknown_5F3770@RegistrationInterface@@QAEXXZ'))[0]
        self.assertEqual(p.key(r), 0x5F3770)
        p = self.prof(names={'Get_Selected@UIComboBox': 0x5FFFE0}, letters={0x5FFFE0: 'G'})
        r = ins(0x50, 'call 0', ('sym', '?Get_Selected@UIComboBoxClass@@QAEHXZ'))[0]
        self.assertEqual(p.label(r), 'G')

    def test_targets(self):
        import tempfile
        d = tempfile.mkdtemp()
        for form in ('a', 'b'):
            os.makedirs(os.path.join(d, form, 'obj'))
            open(os.path.join(d, form, 'obj', 'x.obj'), 'wb').close()
        self.assertEqual([f for f, _ in P.targets([d])], ['a', 'b'])
        open(os.path.join(d, 'b', 'obj', 'y.obj'), 'wb').close()
        with self.assertRaises(SystemExit):
            P.targets([d])
        self.assertEqual([f for f, _ in P.targets([d], 'x')], ['a', 'b'])


if __name__ == '__main__':
    unittest.main()
