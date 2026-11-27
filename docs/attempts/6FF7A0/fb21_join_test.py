HYPOTHESIS = '0x6FF7A0 slots: the whole Name / Owner selection as one inline helper (Get_Name 0x6FF4B0 has the same four unshared slots and one inline layer less): sdg4 has the slots and all but two jump targets: the first lookup of each arm as Find_Field with its own t && t->Unknown70 test (t known non-null on its valid exit, so the join test is jumped past), the fallback through the valid-or-NULL inline (its exit keeps the join test)'
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

FI = 'static __forceinline DeltaStringClass *Unknown_Valid_F(DeltaCompoundClass *root, const NameClass &name) { DeltaStringClass *t = Unknown_Find_Field<DeltaStringClass>(root, name); if (t && t->Unknown70) return(t); return(NULL); }	// NAME 1: neutral (D10)\n'
def HI(v):
    return 'Unknown_Valid_F(%s, %s)' % (AUX, v)
def arm(k, a, af, b, bf):
    if k == 'g1':
        return '{ NameClass %s("%s"); t = %s; if (!(t && t->Unknown70)) t = NULL; if (!t) { NameClass %s("%s"); t = %s; if (!(t && t->Unknown70)) t = NULL; } }' % (a, af, HF(a), b, bf, HF(b))
    if k == 'g2':
        return '{ NameClass %s("%s"); DeltaStringClass *f = %s; t = (f && f->Unknown70) ? f : NULL; if (!t) { NameClass %s("%s"); f = %s; t = (f && f->Unknown70) ? f : NULL; } }' % (a, af, HF(a), b, bf, HF(b))
    if k == 'g3':
        return '{ NameClass %s("%s"); DeltaStringClass *f = %s; if (f && f->Unknown70) t = f; else t = NULL; if (!t) { NameClass %s("%s"); f = %s; if (f && f->Unknown70) t = f; else t = NULL; } }' % (a, af, HF(a), b, bf, HF(b))
    if k == 'g4':
        return '{ NameClass %s("%s"); t = %s; if (!t) { NameClass %s("%s"); t = %s; } }' % (a, af, HI(a), b, bf, HI(b))
def helper(k):
    def arm2(k, a, af, b, bf):
        if k in ('s6', 's6c'):
            return '{ NameClass %s("%s"); t = %s; if (!(t && t->Unknown70)) { NameClass %s("%s"); t = %s; } }' % (a, af, HF(a), b, bf, HI(b))
        if k == 's6b':
            return '{ NameClass %s("%s"); t = %s; if (!t || !t->Unknown70) { NameClass %s("%s"); t = %s; } }' % (a, af, HF(a), b, bf, HI(b))
        if k == 's8':
            return '{ NameClass %s("%s"); t = %s; if (!(t && t->Unknown70)) { NameClass %s("%s"); t = %s; } }' % (a, af, HF(a), b, bf, HV(b))
        if k == 's9':
            return '{ NameClass %s("%s"); t = %s; if (!(t && t->Unknown70)) { NameClass %s("%s"); DeltaStringClass *f = %s; t = (f && f->Unknown70) ? f : NULL; } }' % (a, af, HF(a), b, bf, HF(b))
        return arm('g4', a, af, b, bf)
    A = arm2(k, 'n1', 'Name', 'o1', 'Owner')
    B = arm2(k, 'o2', 'Owner', 'n2', 'Name')
    sw = 'switch (o->Type) { default: %s break; case 1: %s break; }' % (A, B)
    if k == 's6c': sw = 'switch (o->Type) { case 1: %s break; default: %s break; }' % (B, A)
    ret = ' return(t ? t : NULL);' if k == 's3' else ' return(t);'
    return FI + P + sw + ret + E
HELPERS = {}
for k in ('s3', 's6', 's6b', 's6c', 's8', 's9'):
    HELPERS[k] = helper(k)

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
    FORMS[h] = [(ANCHOR, '}\n' + HELPERS[h] + '\n// 0x6FF7A0 the name label'), (OLD, body(1, 1, 0))]
