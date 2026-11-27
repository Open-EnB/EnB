HYPOTHESIS = ('0x522860 (residue 56; Modes 0x5266A7 28/60 rows): NX found T23_R1_J2 (23 top Peeks, one dead Peek after '
              'the Refining push_back, two after the join) gives the client\'s whole Modes profile but cuts case 26\'s '
              'three assigns; B31 pool lever: dead (void)(Slot + k) sums grow the pool without a Peek\'s spend, so with '
              'them case 26 keeps its expansions while the nested Modes spend stays')
FILE = 'app/client/item/client_item_property_builder.cpp'
SYMBOL = 'ClientItemPropertyBuilderClass::Unknown_522860'
ADDRESS = 0x522860

P = '\tData.Peek();\n'
TOP = P * 25
REF = '\t\t\t\tif (mode_flags & 8) modes.push_back("Refining");\n'
JOIN = '\t\t\t\tstd::string text = String_Join(modes, ", ");\n'


def sums(n, v='Slot'):
    return ''.join(' (void)(%s + %d);' % (v, k) for k in range(n))


def form(peeks=25, s=0, r=0, j=0, swap=0):
    lines = [P] * (peeks - swap) + ['\t(void)(Slot + %d);\n' % (100 + i) for i in range(swap)] + ['\n'] * (25 - peeks)
    if s:
        lines[0] = lines[0][:-1] + sums(s) + '\n'
    h = [(TOP, ''.join(lines))]
    if r:
        h.append((REF, REF[:-1] + ' Data.Peek();' * r + '\n'))
    if j:
        h.append((JOIN, JOIN[:-1] + ' Data.Peek();' * j + '\n'))
    return h


FORMS = {}
OLDFORMS = None
for s in (1, 2, 4):
    FORMS['b_s%d' % s] = form(s=s)
for s in (0, 1, 2, 3, 4, 6):
    FORMS['T23R1J2_s%d' % s] = form(peeks=23, s=s, r=1, j=2)
for s in (0, 2):
    FORMS['T25R1J2_s%d' % s] = form(s=s, r=1, j=2)
for m in (2, 4):
    FORMS['T25sw%dR1J2' % m] = form(swap=m, r=1, j=2)

HYPOTHESIS = ('0x522860: b4 sw13 (13 of the 25 top Peeks swapped for dead sums) gives the client Modes profile modulo '
              'stack slots (masked 46) but case 26 string ctor at 0x525362 goes out of line; grow the pool with extra '
              'sums (case 26 is earlier) or drop a Peek, swap counts 13-17')
FORMS = {}
for s in (1, 2, 3, 4, 6):
    FORMS['sw13_s%d' % s] = form(swap=13, s=s)
FORMS['sw13_T24'] = form(peeks=24, swap=13)
FORMS['sw13_T23'] = form(peeks=23, swap=13)
FORMS['sw14_s2'] = form(swap=14, s=2)
FORMS['sw15_s2'] = form(swap=15, s=2)
for m in (16, 17, 18):
    FORMS['sw%d' % m] = form(swap=m)
