import re

HYPOTHESIS = ('0x522860 on sw18 (masked 0, BRANCH 14t): at 14 level cases the client jumps from a false Has_Field '
              'past the level compare (je to the Create), ours lands on the compare: the client\'s compare sits inside '
              'the Has_Field block (as in the Effect Range cases); behaviour identical (actual == base on that path). '
              'Move it in, line count kept; with the sum count swept 16-20 in case the budget moves')
FILE = 'app/client/item/client_item_property_builder.cpp'
SYMBOL = 'ClientItemPropertyBuilderClass::Unknown_522860'
ADDRESS = 0x522860

src = open(FILE, encoding='utf-8').read()
PAT = re.compile(r'\t\t\t\}\n\t\t\tif \(actual < base\) \{\n\t\t\t\tlevel = (\d);\n'
                 r'\t\t\t\} else if \(actual > base\) \{\n\t\t\t\tlevel = (\d);\n\t\t\t\}\n')
NEW = (r'\t\t\t\tif (actual < base) {\n\t\t\t\t\tlevel = \1;\n\t\t\t\t} else if (actual > base) {\n'
       r'\t\t\t\t\tlevel = \2;\n\t\t\t\t}\n\t\t\t}\n')
inside, n = PAT.subn(NEW, src)
assert n == 14, n
assert inside.count('\n') == src.count('\n')

P = '\tData.Peek();\n'


def with_sums(text, m):
    # the tree has 7 Peeks then 18 sums (Slot + 100..117); make it 25 - m Peeks, m sums
    block = P * 7 + ''.join('\t(void)(Slot + %d);\n' % (100 + i) for i in range(18))
    assert text.count(block) == 1
    return text.replace(block, P * (25 - m) + ''.join('\t(void)(Slot + %d);\n' % (100 + i) for i in range(m)))


FORMS = {'in14': [(src, inside)]}
for m in (16, 17, 19, 20, 21):
    FORMS['in14_sw%d' % m] = [(src, with_sums(inside, m))]
