"""Tests for tools/progress.py, tools/re/declcensus.py and tools/docs_check.py.

    python -m unittest discover -s tools -p "test_*.py"

Standard library only; the two data tests read the committed docs/data of this checkout.
"""
import os
import sys
import unittest

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
sys.path.insert(0, os.path.join(HERE, 're'))

import declcensus  # noqa: E402
import progress  # noqa: E402

SAMPLE = '''
// NAME 2: a tagged global
int Counter = 0;
class FooClass : public Base {
public:
	FooClass(int a, const char *name);
	int Get_Value() const { return(m_Value); }
	int m_Value;	// NAME 3: by convention
	int m_Unknown10;
	void Run(float dt);
};
void FooClass::Run(float dt)
{
	int i = 0;
	for (int j = 0; j < 3; j++) {
		unsigned long total = 0;
	}
	Helper(dt);
}
enum Mode { MODE_A, MODE_B = 2 };
'''


class CensusTests(unittest.TestCase):
    def setUp(self):
        self.c = declcensus.score_all([declcensus.Census('sample.cpp', SAMPLE)])[0]

    def names(self, kind):
        return [d.name for d in self.c.decls if d.kind == kind]

    def test_kinds(self):
        self.assertEqual(self.names('type'), ['FooClass', 'Mode'])
        self.assertIn('Counter', self.names('global'))
        self.assertEqual(sorted(self.names('param')), ['a', 'dt', 'dt', 'name'])
        self.assertEqual(sorted(self.names('local')), ['i', 'j', 'total'])
        self.assertIn('MODE_B', self.names('member'))
        self.assertEqual(sorted(n for n in self.names('function')), ['FooClass', 'Get_Value', 'Run', 'Run'])

    def test_scores(self):
        score = {d.name: d.score for d in self.c.decls if d.kind in ('global', 'member')}
        self.assertEqual(score['Counter'], 2)
        self.assertEqual(score['m_Value'], 3)
        self.assertEqual(score['m_Unknown10'], 1)         # untagged placeholder (D10)
        self.assertEqual(score['MODE_A'], 5)              # untagged: counts as 5 (D56)

    def test_out_of_class_definition_takes_the_declaration_score(self):
        text = 'class A {\n int Get();	// NAME 3: x\n};\nint A::Get() { return 1; }\n'
        c = declcensus.score_all([declcensus.Census('a.cpp', text)])[0]
        self.assertEqual([d.score for d in c.decls if d.kind == 'function'], [3, 3])


class ProgressTests(unittest.TestCase):
    def test_dense_span_ignores_strays(self):
        addrs = [0x100] + [0x900000 + 0x40 * k for k in range(20)] + [0xF00000]
        self.assertEqual(progress.dense_span(addrs), (0x900000, 0x900000 + 0x40 * 19))
        self.assertIsNone(progress.dense_span([1, 2, 3]))

    def test_function_size_uses_next_start_and_alignment(self):
        tab = progress.Tables.__new__(progress.Tables)
        tab.starts, tab.lengths, tab.text_end, tab.estimated_sizes = [0x1000, 0x1100, 0x2000], {0x1000: 0x30}, 0x3000, 0
        self.assertEqual(tab.size(0x1000), 0x30)            # length 0x30, aligned end 0x1030
        self.assertEqual(tab.size(0x1100), 0x800 if 0x2000 - 0x1100 > 0x800 else 0x2000 - 0x1100)
        self.assertEqual(tab.size(0x2000, 0x20), 0x20)

    def test_not_exact_row(self):
        m = progress.NOT_EXACT_ROW.match('| `app/common/connect.cpp:800` | `ConnectionManagerClass::Remove_Dead` | marker | x |')
        self.assertEqual(m.groups(), ('app/common/connect.cpp', '800', 'ConnectionManagerClass::Remove_Dead'))

    def test_states_add_up_on_the_committed_data(self):
        src = progress.Src(None)
        tab = progress.Tables(src)
        r = progress.measure(src, tab)
        self.assertEqual(sum(r.counts.values()), r.total)
        self.assertTrue(all(v >= 0 for v in r.counts.values()))
        self.assertGreater(r.counts['exact'], 0)
        rows = progress.region_table(r)
        self.assertEqual(sum(row['size'] for row in rows), r.total)


if __name__ == '__main__':
    unittest.main()
