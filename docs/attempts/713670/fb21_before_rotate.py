HYPOTHESIS = '0x713670 residue 6 (x87 operand order in the inlined Rotate_Z(angle) rows 1-2): the form of what stands before it (Translate by components, the identity copy, where angle is declared and how it is summed) or the (s, c) overload decides which operand is on the stack first'
FILE = 'app/client/extrap.cpp'
SYMBOL = 'Unknown713520Class::Unknown_1C'
ADDRESS = 0x713670

TR = '\t\t\ttransform.Translate(center);\n'
RZ = '\t\t\ttransform.Rotate_Z(angle);\n'
AN = '\t\t\tfloat angle = dt * OrbitRate + OrbitAngle;\n'
ID = '\tMatrix3D transform = Matrix3D::Identity;\n'
CE = '\t\t\tcenter = object->PositionalState->Unknown_1C(time);\n'
src = open(FILE, encoding='utf-8').read()
a = src.index('void Unknown713520Class::Unknown_1C(')
b = src.index('\n}\n', a)
FN = src[a:b]
for x in (TR, RZ, AN, ID, CE):
    assert FN.count(x) == 1, x

FORMS = {
    'tr3': [(TR, '\t\t\ttransform.Translate(center.X, center.Y, center.Z);\n')],
    'idc': [(ID, '\tMatrix3D transform(Matrix3D::Identity);\n')],
    'idm': [(ID, '\tMatrix3D transform; transform.Make_Identity();\n')],
    'an_sum': [(AN, '\t\t\tfloat angle = OrbitAngle + dt * OrbitRate;\n')],
    'an_const': [(AN, '\t\t\tconst float angle = dt * OrbitRate + OrbitAngle;\n')],
    'an_after': [(AN + TR, TR + AN)],
    'sc': [(RZ, '\t\t\ttransform.Rotate_Z(WWMath::Sin(angle), WWMath::Cos(angle));\n')],
    'sc2': [(RZ, '\t\t\t{ float s = WWMath::Sin(angle); float c = WWMath::Cos(angle); transform.Rotate_Z(s, c); }\n')],
    'cs2': [(RZ, '\t\t\t{ float c = WWMath::Cos(angle); float s = WWMath::Sin(angle); transform.Rotate_Z(s, c); }\n')],
    'ctmp': [(CE, '\t\t\t{ Vector3 c = object->PositionalState->Unknown_1C(time); center = c; }\n')],
    'trv': [(TR, '\t\t\ttransform.Translate(Vector3(center));\n')],
}
