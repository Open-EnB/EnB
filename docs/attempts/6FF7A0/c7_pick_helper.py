HYPOTHESIS = '0x6FF7A0 slots: the Name/Owner pick as one inline helper called with (Name, Owner) / (Owner, Name) (its locals flattened per instance: n1/o1/o2/n2 own slots), helper on Find_Valid or on Find_Field with its own +0x70 tests; Title / Rank scopes open or closed'
FILE = 'app/client/co.cpp'
SYMBOL = 'ClientObjectClass::Unknown_6FF7A0'
ADDRESS = 0x6FF7A0

BASE_LINES = 39
HEAD = 'void ClientObjectClass::Unknown_6FF7A0(std::string &text)\n{\n'
ANCHOR = '}\n\n// 0x6FF7A0 the name label'
AUX = '(DeltaCompoundClass *)o->AuxData'
def FV(v):
    return 'Unknown_Find_Valid<DeltaStringClass>((DeltaCompoundClass *)AuxData, %s)' % v

HELPERS = {
    'p1': 'static inline DeltaStringClass *Unknown_Name_Pick(ClientObjectClass *o, const char *first, const char *second) { NameClass a(first); '
          'DeltaStringClass *t = Unknown_Find_Valid<DeltaStringClass>(%s, a); if (!t) { NameClass b(second); t = Unknown_Find_Valid<DeltaStringClass>(%s, b); } return(t); }	// NAME 1: neutral (D10)' % (AUX, AUX),
    'p2': 'static inline DeltaStringClass *Unknown_Name_Pick(ClientObjectClass *o, const char *first, const char *second) { NameClass a(first); '
          'DeltaStringClass *t = Unknown_Find_Field<DeltaStringClass>(%s, a); if (t && t->Unknown70) return(t); NameClass b(second); t = Unknown_Find_Field<DeltaStringClass>(%s, b); if (t && t->Unknown70) return(t); return(NULL); }	// NAME 1: neutral (D10)' % (AUX, AUX),
    'p3': 'static inline DeltaStringClass *Unknown_Name_Pick(ClientObjectClass *o, const char *first, const char *second) { NameClass a(first); '
          'DeltaStringClass *t = Unknown_Find_Field<DeltaStringClass>(%s, a); if (t && t->Unknown70) return(t); { NameClass b(second); t = Unknown_Find_Field<DeltaStringClass>(%s, b); } if (t && t->Unknown70) return(t); return(NULL); }	// NAME 1: neutral (D10)' % (AUX, AUX),
}

def body(sel, c_ti, c_ra):
    s = '\tDeltaStringClass *t;\n'
    if sel == 'if':
        s += '\tif (!Unknown_6F5A40()) {\n\t\tt = Unknown_Name_Pick(this, "Name", "Owner");\n\t} else {\n\t\tt = Unknown_Name_Pick(this, "Owner", "Name");\n\t}\n'
    else:
        s += '\tif (!Unknown_6F5A40()) t = Unknown_Name_Pick(this, "Name", "Owner"); else t = Unknown_Name_Pick(this, "Owner", "Name");\n'
    s += '\tconst char *name = t ? t->String : "Unknown";\n'
    s += '\tif (Type == 0) {\n\t\ttext = std::string(name);\n\t} else {\n'
    if c_ti:
        s += '\t\tconst char *title; { NameClass ti("Title"); t = %s; title = t ? t->String : ""; }\n' % FV('ti')
    else:
        s += '\t\tNameClass ti("Title");\n\t\tt = %s;\n\t\tconst char *title = t ? t->String : "";\n' % FV('ti')
    s += '\t\tif (*title) {\n\t\t\ttext = std::string(title) + " " + name;\n\t\t} else {\n'
    if c_ra:
        s += '\t\t\tconst char *rank; { NameClass ra("Rank"); t = %s; rank = t ? t->String : ""; }\n' % FV('ra')
    else:
        s += '\t\t\tNameClass ra("Rank");\n\t\t\tt = %s;\n\t\t\tconst char *rank = t ? t->String : "";\n' % FV('ra')
    s += '\t\t\tif (*rank) {\n\t\t\t\ttext = std::string(rank) + " " + name;\n\t\t\t} else {\n\t\t\t\ttext = name;\n\t\t\t}\n\t\t}\n'
    s += '\t} Unknown_6F5A40();\n'
    k = s.count('\n')
    assert k <= BASE_LINES - 3, k
    return HEAD + s + '\n' * (BASE_LINES - 3 - k) + '}\n'

import re
src = open('app/client/co.cpp', encoding='utf-8').read()
a = src.index('void ClientObjectClass::Unknown_6FF7A0(std::string &text)\n{\n')
b = src.index('\n}\n', a) + 3
OLD = src[a:b]
assert OLD.count('\n') == BASE_LINES, OLD.count('\n')

FORMS = {}
for h in HELPERS:
    for ti in (0, 1):
        for ra in (0, 1):
            FORMS['%s_t%d%d' % (h, ti, ra)] = [(ANCHOR, '}\n' + HELPERS[h] + '\n// 0x6FF7A0 the name label'), (OLD, body('if', ti, ra))]
