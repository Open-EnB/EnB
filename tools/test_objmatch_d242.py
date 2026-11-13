"""Tests for objmatch's two D242 crediting rounds (2026-11-12):

  P-C1-1  object_order: the tied own functions of one object that share one set of candidates, as many as the
          candidates, all inside the object's range, take them in object order (not COMDATs; each must land
          between its nearest exact own functions in object order)
  P-C1-2  xcu_position: an object's `_$E` initialisers are the consecutive CRT table entries in its stretch, n-th
          to n-th; the `_$E` functions their bytes reach (call / push targets) follow

    python -m unittest discover -s tools -p "test_*.py"

Standard library only; objects, the client text and the table are fakes.
"""
import collections
import os
import re
import struct
import sys
import unittest

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, 're'))

import objmatch  # noqa: E402


class FakeObj:
    """sections: {sec: (bytes, own, [(offset, size, type, target name)])}; xcu: initialiser names."""

    def __init__(self, sections, xcu=()):
        self.secs = sections
        self.syms = {}
        self.sections = [{'name': '.text', 'relocs': []}]
        if xcu:
            self.sections.append({'name': '.CRT$XCU', 'relocs': [(4 * i, self.sym(n)['index'], 6)
                                                                 for i, n in enumerate(xcu)]})

    def sym(self, name):
        if name not in self.syms:
            self.syms[name] = {'name': name, 'index': len(self.syms), 'class': 3, 'section': 1, 'type': 0x20}
        return self.syms[name]

    def symbol_at(self, idx):
        return next((s for s in self.syms.values() if s['index'] == idx), None)

    def own(self, sec):
        return self.secs[sec][1]

    def pattern(self, sec, start, end):
        body, _, relocs = self.secs[sec]
        wild = set()
        for k, n, _, _ in relocs:
            wild.update(range(k, k + n))
        pat = b''.join(b'.' if i in wild else re.escape(body[i:i + 1]) for i in range(len(body)))
        return pat, len(body) - len(wild), len(body)

    def reloc_targets(self, sec, start, length):
        return [(k, n, typ, self.sym(t), b'\0\0\0\0') for k, n, typ, t in self.secs[sec][2]]


def res(obj, sec, name, addr=None, status=None, hits=None):
    r = {'symbol': name, 'status': status or ('exact' if addr is not None else 'multiple'),
         'addresses': [f'0x{addr:06X}'] if addr is not None else [f'0x{h:06X}' for h in hits or []]}
    return (r, obj, sec, 0, len(obj.secs[sec][0]))


class ObjectOrder(unittest.TestCase):
    def setUp(self):
        body = b'\x55\x8b\xec\x5d\xc3'
        self.obj = FakeObj({1: (body, True, []), 2: (body, True, []), 3: (body, True, []), 4: (body, True, []),
                            5: (body, False, []), 6: (body, False, [])})

    def run_rule(self, items, exact, rng=(0x400, 0x900), claimed=None, obj_called=None):
        results = [res(self.obj, sec, n, a) for sec, n, a in exact] + [x[:5] for x in items]
        pending = [x for x in items]
        surv = {id(x[0]): x[5] for x in items}
        return objmatch.object_order(pending, set(), surv, {id(self.obj): rng},
                                     collections.defaultdict(set, claimed or {}), obj_called or {}, results)

    def tie(self, sec, name, hits):
        return res(self.obj, sec, name, hits=hits) + (hits,)

    def test_two_own_functions_in_object_order(self):
        a, b = self.tie(2, 'Start', [0x600, 0x700]), self.tie(3, 'Start_Root', [0x600, 0x700])
        got = self.run_rule([b, a], [(1, 'Before', 0x500), (4, 'After', 0x800)])
        self.assertEqual(sorted((x[0]['symbol'], ad) for x, ad in got), [('Start', 0x600), ('Start_Root', 0x700)])

    def test_comdats_stay_tied(self):
        a, b = self.tie(5, 'Inl1', [0x600, 0x700]), self.tie(6, 'Inl2', [0x600, 0x700])
        self.assertEqual(self.run_rule([a, b], [(1, 'Before', 0x500)]), [])

    def test_neighbour_between_contradicts(self):
        # object order Start (2), Mid (3, exact at 0x650), Start_Root (4): Start_Root must lie above 0x650 and
        # Start below it; the candidates 0x600 / 0x620 cannot satisfy both
        a, b = self.tie(2, 'Start', [0x600, 0x620]), self.tie(4, 'Start_Root', [0x600, 0x620])
        self.assertEqual(self.run_rule([a, b], [(3, 'Mid', 0x650)]), [])

    def test_outside_range_or_claimed_or_foreign_tie(self):
        a, b = self.tie(2, 'Start', [0x600, 0x980]), self.tie(3, 'Start_Root', [0x600, 0x980])
        self.assertEqual(self.run_rule([a, b], []), [])                                  # 0x980 outside the range
        a, b = self.tie(2, 'Start', [0x600, 0x700]), self.tie(3, 'Start_Root', [0x600, 0x700])
        self.assertEqual(self.run_rule([a, b], [], claimed={0x700: {'Other'}}), [])
        c = self.tie(4, 'Third', [0x700, 0x780])                                           # survives at 0x700 too
        self.assertEqual(self.run_rule([a, b, c], []), [])

    def test_group_size_must_equal_candidates(self):
        a, b = self.tie(2, 'Start', [0x600, 0x700, 0x780]), self.tie(3, 'Start_Root', [0x600, 0x700, 0x780])
        self.assertEqual(self.run_rule([a, b], []), [])

    def test_own_calls_contradict(self):
        a, b = self.tie(2, 'Start', [0x600, 0x700]), self.tie(3, 'Start_Root', [0x600, 0x700])
        self.assertEqual(self.run_rule([a, b], [], obj_called={(id(self.obj), 'Start'): {0x700}}), [])


class XcuPosition(unittest.TestCase):
    BASE = 0x1000

    def setUp(self):
        self.saved = dict(objmatch._xcu)
        self.addCleanup(lambda: (objmatch._xcu.clear(), objmatch._xcu.update(self.saved)))
        # obj: _$E3 (initialiser: call _$E1; push _$E2; ret), F exact at 0x1100, _$E6 (initialiser), _$E1, _$E2
        init = b'\xe8\0\0\0\0\x68\0\0\0\0\xc3'
        self.obj = FakeObj({1: (init, True, [(1, 4, 0x14, '_$E1'), (6, 4, 0x06, '_$E2')]),
                            2: (b'\x90' * 8, True, []),
                            3: (b'\xb8\0\0\0\0\xc3', True, [(1, 4, 0x06, 'x')]),
                            4: (b'\x31\xc0\x40\xc3', True, []),
                            5: (b'\x33\xc9\x41\xc3', True, [])}, xcu=['_$E3', '_$E6'])
        self.other = FakeObj({1: (b'\xcc' * 4, True, [])})
        text = bytearray(b'\xcc' * 0x400)

        def put(a, b):
            text[a - self.BASE:a - self.BASE + len(b)] = b
        put(0x1000, b'\xe8' + struct.pack('<i', 0x1050 - 0x1005) + b'\x68' + struct.pack('<I', 0x1060) + b'\xc3')
        put(0x1050, b'\x31\xc0\x40\xc3')
        put(0x1060, b'\x33\xc9\x41\xc3')
        put(0x1100, b'\x90' * 8)
        put(0x1200, b'\xb8\x00\x00\x00\x00\xc3')
        self.text = bytes(text)

    def results(self, extra=()):
        o = self.obj
        return [res(o, 1, '_$E3', status='short'), res(o, 2, 'F', 0x1100), res(o, 3, '_$E6', status='short'),
                res(o, 4, '_$E1', status='short'), res(o, 5, '_$E2', status='short'),
                res(self.other, 1, 'G', 0x1300), res(self.other, 1, 'H', 0x0F80), *extra]

    def run_rule(self, table, results=None):
        objmatch._xcu['table'] = table
        results = results or self.results()
        got = objmatch.xcu_position(None, results, {id(self.obj): (0x1100, 0x1100), id(self.other): (0x1300, 0x1300)},
                                    self.text, self.BASE, collections.defaultdict(set), {})
        return sorted((x[0]['symbol'], a, k) for x, a, k in got)

    def test_table_position_and_linked(self):
        # 0x0F00 lies below the other object's exact 0x0F80: not in this object's stretch
        self.assertEqual(self.run_rule([0x0F00, 0x1000, 0x1200, 0x1310]),
                         [('_$E1', 0x1050, 'linked'), ('_$E2', 0x1060, 'linked'),
                          ('_$E3', 0x1000, 'table'), ('_$E6', 0x1200, 'table')])

    def test_extra_entry_in_the_stretch_refuses(self):
        self.assertEqual(self.run_rule([0x1000, 0x1180, 0x1200]), [])

    def test_entry_where_pattern_fails_refuses(self):
        self.assertEqual(self.run_rule([0x1000, 0x1210]), [])

    def test_exact_initialiser_must_sit_at_its_entry(self):
        o = self.obj
        rs = self.results()
        rs[0] = res(o, 1, '_$E3', 0x1010)
        self.assertEqual(self.run_rule([0x1000, 0x1200], rs), [])

    def test_not_past_the_next_object(self):
        # _$E6's window ends below the other object's exact 0x1300: an entry there is not this object's
        self.assertEqual(self.run_rule([0x1000, 0x1300]), [])


if __name__ == '__main__':
    unittest.main()
