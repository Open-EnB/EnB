HYPOTHESIS = '0x6FF7A0 slots: closed vs open scopes for each NameClass lookup (code unchanged; slot grouping searched against the client: n1/o1/o2 own slots, n2/Title/Rank one)'
FILE = 'app/client/co.cpp'
SYMBOL = 'ClientObjectClass::Unknown_6FF7A0'
ADDRESS = 0x6FF7A0
import itertools

BASE_LINES = 39
HEAD = 'void ClientObjectClass::Unknown_6FF7A0(std::string &text)\n{\n'
def F(v):
    return 'Unknown_Find_Valid<DeltaStringClass>((DeltaCompoundClass *)AuxData, %s)' % v

def arm(first, fv, second, sv, closed_first, dbl_second):
    s = ''
    if closed_first:
        s += '\t\t{ NameClass %s("%s"); t = %s; }\n' % (fv, first, F(fv))
    else:
        s += '\t\tNameClass %s("%s");\n\t\tt = %s;\n' % (fv, first, F(fv))
    if dbl_second:
        s += '\t\tif (!t) { { NameClass %s("%s"); t = %s; } }\n' % (sv, second, F(sv))
    else:
        s += '\t\tif (!t) {\n\t\t\tNameClass %s("%s");\n\t\t\tt = %s;\n\t\t}\n' % (sv, second, F(sv))
    return s

def body(c_n1, c_o2, c_ti, c_ra, d_o1=False, d_n2=False):
    s = '\tDeltaStringClass *t;\n\tif (!Unknown_6F5A40()) {\n'
    s += arm('Name', 'n1', 'Owner', 'o1', c_n1, d_o1)
    s += '\t} else {\n'
    s += arm('Owner', 'o2', 'Name', 'n2', c_o2, d_n2)
    s += '\t}\n'
    s += '\tconst char *name = t ? t->String : "Unknown";\n'
    s += '\tif (Type == 0) {\n\t\ttext = std::string(name);\n\t} else {\n'
    if c_ti:
        s += '\t\tconst char *title; { NameClass ti("Title"); t = %s; title = t ? t->String : ""; }\n' % F('ti')
    else:
        s += '\t\tNameClass ti("Title");\n\t\tt = %s;\n\t\tconst char *title = t ? t->String : "";\n' % F('ti')
    s += '\t\tif (*title) {\n\t\t\ttext = std::string(title) + " " + name;\n\t\t} else {\n'
    if c_ra:
        s += '\t\t\tconst char *rank; { NameClass ra("Rank"); t = %s; rank = t ? t->String : ""; }\n' % F('ra')
    else:
        s += '\t\t\tNameClass ra("Rank");\n\t\t\tt = %s;\n\t\t\tconst char *rank = t ? t->String : "";\n' % F('ra')
    s += '\t\t\tif (*rank) {\n\t\t\t\ttext = std::string(rank) + " " + name;\n\t\t\t} else {\n\t\t\t\ttext = name;\n\t\t\t}\n\t\t}\n'
    s += '\t} Unknown_6F5A40();\n'
    k = s.count('\n')
    assert k <= BASE_LINES - 3, k
    return HEAD + s + '\n' * (BASE_LINES - 3 - k) + '}\n'

FORMS = {}
for bits in itertools.product((0, 1), repeat=6):
    name = 'f' + ''.join(map(str, bits))
    FORMS[name] = body(*bits)
