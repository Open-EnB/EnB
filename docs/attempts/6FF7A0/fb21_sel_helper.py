HYPOTHESIS = '0x6FF7A0 slots: the whole Name / Owner selection as one inline helper (Get_Name 0x6FF4B0 has the same four unshared slots and one inline layer less): nested ifs or early returns inside it, on Find_Valid or Find_Field with its own +0x70 tests; Title / Rank open or closed'
FILE = 'app/client/co.cpp'
SYMBOL = 'ClientObjectClass::Unknown_6FF7A0'
ADDRESS = 0x6FF7A0

BASE_LINES = 39
HEAD = 'void ClientObjectClass::Unknown_6FF7A0(std::string &text)\n{\n'
ANCHOR = '}\n\n// 0x6FF7A0 the name label'
AUX = '(DeltaCompoundClass *)o->AuxData'
def FV(v):
    return 'Unknown_Find_Valid<DeltaStringClass>((DeltaCompoundClass *)AuxData, %s)' % v
def HV(v):
    return 'Unknown_Find_Valid<DeltaStringClass>(%s, %s)' % (AUX, v)
def HF(v):
    return 'Unknown_Find_Field<DeltaStringClass>(%s, %s)' % (AUX, v)
P = 'static inline DeltaStringClass *Unknown_Name_Sel(ClientObjectClass *o) { DeltaStringClass *t; '
E = ' }	// NAME 1: neutral (D10)'

HELPERS = {
    # nested ifs on Find_Valid (the tree's form, moved into the helper)
    'x1': P + 'if (!o->Unknown_6F5A40()) { NameClass n1("Name"); t = %s; if (!t) { NameClass o1("Owner"); t = %s; } } else { NameClass o2("Owner"); t = %s; if (!t) { NameClass n2("Name"); t = %s; } } return(t);' % (HV('n1'), HV('o1'), HV('o2'), HV('n2')) + E,
    # early returns on Find_Field, both arms blocks
    'x3': P + 'if (!o->Unknown_6F5A40()) { NameClass n1("Name"); t = %s; if (t && t->Unknown70) return(t); NameClass o1("Owner"); t = %s; if (t && t->Unknown70) return(t); return(NULL); } else { NameClass o2("Owner"); t = %s; if (t && t->Unknown70) return(t); NameClass n2("Name"); t = %s; if (t && t->Unknown70) return(t); return(NULL); }' % (HF('n1'), HF('o1'), HF('o2'), HF('n2')) + E,
    # early returns, the second arm at the helper's own level
    'x5': P + 'if (!o->Unknown_6F5A40()) { NameClass n1("Name"); t = %s; if (t && t->Unknown70) return(t); NameClass o1("Owner"); t = %s; if (t && t->Unknown70) return(t); return(NULL); } NameClass o2("Owner"); t = %s; if (t && t->Unknown70) return(t); NameClass n2("Name"); t = %s; if (t && t->Unknown70) return(t); return(NULL);' % (HF('n1'), HF('o1'), HF('o2'), HF('n2')) + E,
    # early returns on Find_Valid
    'x6': P + 'if (!o->Unknown_6F5A40()) { NameClass n1("Name"); t = %s; if (t) return(t); NameClass o1("Owner"); return(%s); } NameClass o2("Owner"); t = %s; if (t) return(t); NameClass n2("Name"); return(%s);' % (HV('n1'), HV('o1'), HV('o2'), HV('n2')) + E,
    # switch on the type inside the helper, Find_Valid
    'x7': P + 'switch (o->Type) { case 1: { NameClass o2("Owner"); t = %s; if (!t) { NameClass n2("Name"); t = %s; } } break; default: { NameClass n1("Name"); t = %s; if (!t) { NameClass o1("Owner"); t = %s; } } break; } return(t);' % (HV('o2'), HV('n2'), HV('n1'), HV('o1')) + E,
}

def body(ti, ra, tail):
    s = '\tDeltaStringClass *t = Unknown_Name_Sel(this);\n'
    s += '\tconst char *name = t ? t->String : "Unknown";\n'
    s += '\tif (Type == 0) {\n\t\ttext = std::string(name);\n\t} else {\n'
    if ti:
        s += '\t\tconst char *title; { NameClass ti("Title"); t = %s; title = t ? t->String : ""; }\n' % FV('ti')
    else:
        s += '\t\tNameClass ti("Title");\n\t\tt = %s;\n\t\tconst char *title = t ? t->String : "";\n' % FV('ti')
    s += '\t\tif (*title) {\n\t\t\ttext = std::string(title) + " " + name;\n\t\t} else {\n'
    if ra:
        s += '\t\t\tconst char *rank; { NameClass ra("Rank"); t = %s; rank = t ? t->String : ""; }\n' % FV('ra')
    else:
        s += '\t\t\tNameClass ra("Rank");\n\t\t\tt = %s;\n\t\t\tconst char *rank = t ? t->String : "";\n' % FV('ra')
    s += '\t\t\tif (*rank) {\n\t\t\t\ttext = std::string(rank) + " " + name;\n\t\t\t} else {\n\t\t\t\ttext = name;\n\t\t\t}\n\t\t}\n'
    s += '\t} Unknown_6F5A40();\n' if tail else '\t}\n'
    k = s.count('\n')
    assert k <= BASE_LINES - 3, k
    return HEAD + s + '\n' * (BASE_LINES - 3 - k) + '}\n'

src = open('app/client/co.cpp', encoding='utf-8').read()
a = src.index(HEAD)
b = src.index('\n}\n', a) + 3
OLD = src[a:b]
assert OLD.count('\n') == BASE_LINES, OLD.count('\n')
assert src.count(ANCHOR) == 1

FORMS = {}
for h in HELPERS:
    for ti, ra in ((0, 0), (1, 1)):
        for tail in (1, 0):
            FORMS['%s_%d%d%s' % (h, ti, ra, '' if tail else 'n')] = [(ANCHOR, '}\n' + HELPERS[h] + '\n// 0x6FF7A0 the name label'), (OLD, body(ti, ra, tail))]
