HYPOTHESIS = ('B4 fn2 batch 3: the client\'s X and Y temps sit in two unrelated slots (Y on the int64\'s high half, X at '
              '+0x50), so they are two separate by-value temporaries, each with one live component (dead stores removed, '
              'workflow.md 3.1.2 frame row): Vector3 / Vector2 copy-constructed temporaries of the Get_Scale() view per '
              'argument, Z read straight from the field')
FILE = 'app/shared/landdmap.cpp'
SYMBOL = ''
ADDRESS = 0x466F60

OLD = ('\tfloat spacing[3] = { Field->Get_Spacing(), Field->Get_YSpacing(), Field->Get_VScale() };\n'
       '\tfloat size = (float)(unsigned int)Field->Get_Size();\n'
       '\tD3DXMATRIX scale;\n'
       '\tD3DXMatrixScaling(&scale, spacing[0] * size, spacing[1] * size, spacing[2] * size);\n'
       '\n\n\n')
SZ = '\tfloat size = (float)(unsigned int)Field->Get_Size();\n'
DM = '\tD3DXMATRIX scale;\n'
REF = '\tconst Vector3 &sc = Field->Get_Scale();\n'


def pad(s, n=7):
    k = s.count('\n')
    assert k <= n
    return s + '\n' * (n - k)


CALLS = {
    'v3tmp_ref': REF + SZ + DM + '\tD3DXMatrixScaling(&scale, Vector3(sc).X * size, Vector3(sc).Y * size, sc.Z * size);\n',
    'v3tmp_ref_all': REF + SZ + DM + '\tD3DXMatrixScaling(&scale, Vector3(sc).X * size, Vector3(sc).Y * size, Vector3(sc).Z * size);\n',
    'v3tmp_get': SZ + DM + '\tD3DXMatrixScaling(&scale, Vector3(Field->Get_Scale()).X * size, Vector3(Field->Get_Scale()).Y * size, Field->Get_VScale() * size);\n',
    'v3tmp_get_z': SZ + DM + '\tD3DXMatrixScaling(&scale, Vector3(Field->Get_Scale()).X * size, Vector3(Field->Get_Scale()).Y * size, Field->Get_Scale().Z * size);\n',
    'v3named2': SZ + DM + '\tVector3 a = Field->Get_Scale(); Vector3 b = Field->Get_Scale();\n\tD3DXMatrixScaling(&scale, a.X * size, b.Y * size, Field->Get_VScale() * size);\n',
    'v3named2_b': '\tVector3 a = Field->Get_Scale(); Vector3 b = Field->Get_Scale();\n' + SZ + DM + '\tD3DXMatrixScaling(&scale, a.X * size, b.Y * size, Field->Get_VScale() * size);\n',
    'v3named1_xy': SZ + DM + '\tVector3 a = Field->Get_Scale();\n\tD3DXMatrixScaling(&scale, a.X * size, a.Y * size, Field->Get_Scale().Z * size);\n',
    'f_tmp': SZ + DM + '\tD3DXMatrixScaling(&scale, float(Field->Get_Scale().X) * size, float(Field->Get_Scale().Y) * size, Field->Get_VScale() * size);\n',
}
FORMS = {k: [(OLD, pad(v))] for k, v in CALLS.items()}
