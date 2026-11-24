HYPOTHESIS = '0x466F60 scaling: a spacing vector local (Vector3 / D3DXVECTOR3, ctor / member-wise / Set) whose X, Y VC6 keeps in memory and Z propagates (vec3_mul showed exactly that split)'
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
ARGS = 'Field->Get_Spacing(), Field->Get_YSpacing(), Field->Get_VScale()'
CALL = '\tD3DXMATRIX scale;\n\tD3DXMatrixScaling(&scale, sp.%s * size, sp.%s * size, sp.%s * size);\n'

FORMS = {
    'v3_ctor': pad(SZ + '\tVector3 sp(%s);\n' % ARGS + CALL % ('X', 'Y', 'Z')),
    'v3_ctor_first': pad('\tVector3 sp(%s);\n' % ARGS + SZ + CALL % ('X', 'Y', 'Z')),
    'v3_members': pad(SZ + '\tVector3 sp; sp.X = Field->Get_Spacing(); sp.Y = Field->Get_YSpacing(); sp.Z = Field->Get_VScale();\n' + CALL % ('X', 'Y', 'Z')),
    'v3_set': pad(SZ + '\tVector3 sp; sp.Set(%s);\n' % ARGS + CALL % ('X', 'Y', 'Z')),
    'v3_assign': pad(SZ + '\tVector3 sp = Vector3(%s);\n' % ARGS + CALL % ('X', 'Y', 'Z')),
    'd3v_ctor': pad(SZ + '\tD3DXVECTOR3 sp(%s);\n' % ARGS + CALL % ('x', 'y', 'z')),
    'd3v_ctor_first': pad('\tD3DXVECTOR3 sp(%s);\n' % ARGS + SZ + CALL % ('x', 'y', 'z')),
    'v3_scaled': pad(SZ + '\tVector3 sp(%s);\n' % ARGS + '\tsp *= size;\n\tD3DXMATRIX scale;\n\tD3DXMatrixScaling(&scale, sp.X, sp.Y, sp.Z);\n'),
}
FORMS = {k: [(OLD, v)] for k, v in FORMS.items()}
