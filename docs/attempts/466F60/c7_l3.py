HYPOTHESIS = '0x466F60 scaling round 3: X / Y through a 2-vector or array local (Z read directly), before or after the size (v3_ctor_first fixed the GetTransform / camview registers but kept X, Y, Z on the x87)'
FILE = 'app/shared/landdmap.cpp'
SYMBOL = 'LandUnknown466BE0Class::Unknown_466F60'
ADDRESS = 0x466F60

OLD = '''	float size = (float)(unsigned int)Field->Get_Size();
	float xs = Field->Get_Spacing() * size;
	float ys = Field->Get_YSpacing() * size;
	float zs = Field->Get_VScale() * size;
	D3DXMATRIX scale;
	D3DXMatrixScaling(&scale, xs, ys, zs);
'''

def pad(s, n=6):
    k = s.count('\n')
    assert k <= n, (k, s)
    return s + '\n' * (n - k)

SZ = '\tfloat size = (float)(unsigned int)Field->Get_Size();\n'
XY = 'Field->Get_Spacing(), Field->Get_YSpacing()'
C2 = '\tD3DXMATRIX scale;\n\tD3DXMatrixScaling(&scale, sp.%s * size, sp.%s * size, Field->Get_VScale() * size);\n'
C3 = '\tD3DXMATRIX scale;\n\tD3DXMatrixScaling(&scale, sp.%s * size, sp.%s * size, sp.%s * size);\n'
CA = '\tD3DXMATRIX scale;\n\tD3DXMatrixScaling(&scale, sp[0] * size, sp[1] * size, %s * size);\n'

FORMS = {
    'v2_after': pad(SZ + '\tVector2 sp(%s);\n' % XY + C2 % ('X', 'Y')),
    'v2_first': pad('\tVector2 sp(%s);\n' % XY + SZ + C2 % ('X', 'Y')),
    'd2_first': pad('\tD3DXVECTOR2 sp(%s);\n' % XY + SZ + C2 % ('x', 'y')),
    'arr3_first': pad('\tfloat sp[3] = { Field->Get_Spacing(), Field->Get_YSpacing(), Field->Get_VScale() };\n' + SZ + CA % 'sp[2]'),
    'arr3_after': pad(SZ + '\tfloat sp[3] = { Field->Get_Spacing(), Field->Get_YSpacing(), Field->Get_VScale() };\n' + CA % 'sp[2]'),
    'arr2_first': pad('\tfloat sp[2] = { Field->Get_Spacing(), Field->Get_YSpacing() };\n' + SZ + CA % 'Field->Get_VScale()'),
    'v3_first_zdirect': pad('\tVector3 sp(Field->Get_Spacing(), Field->Get_YSpacing(), Field->Get_VScale());\n' + SZ + C2 % ('X', 'Y')),
    'v3_first_zyx': pad('\tVector3 sp(Field->Get_Spacing(), Field->Get_YSpacing(), Field->Get_VScale());\n' + SZ + '\tD3DXMATRIX scale;\n\tfloat zs = sp.Z * size; float ys = sp.Y * size; float xs = sp.X * size;\n\tD3DXMatrixScaling(&scale, xs, ys, zs);\n'),
}
FORMS = {k: [(OLD, v)] for k, v in FORMS.items()}
