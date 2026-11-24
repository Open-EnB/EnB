HYPOTHESIS = '0x466F60 scaling: the client copies X / Y spacing into stack temps (int moves) before the multiplies; inline-parameter temps (helper with float by-value params, D3DXVECTOR3 ctor / operator*), locals before the size'
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
OLD_FN = 'void LandUnknown466BE0Class::Unknown_466F60(RenderInfoClass &rinfo)\n'

def pad(s, n=6):
    k = s.count('\n')
    assert k <= n, (k, s)
    return s + '\n' * (n - k)

H_SCALE = ('static inline void Unknown_Scaling(D3DXMATRIX *m, float x, float y, float z, float s) { D3DXMatrixScaling(m, x * s, y * s, z * s); }	// NAME 1: neutral (D10)\n')
H_SCALE2 = ('static inline void Unknown_Scaling(D3DXMATRIX *m, float x, float y, float z) { D3DXMatrixScaling(m, x, y, z); }	// NAME 1: neutral (D10)\n')

FORMS = {
    'helper5': [(OLD_FN, H_SCALE + OLD_FN), (OLD, pad('''	float size = (float)(unsigned int)Field->Get_Size();
	D3DXMATRIX scale;
	Unknown_Scaling(&scale, Field->Get_Spacing(), Field->Get_YSpacing(), Field->Get_VScale(), size);
'''))],
    'helper4': [(OLD_FN, H_SCALE2 + OLD_FN), (OLD, pad('''	float size = (float)(unsigned int)Field->Get_Size();
	D3DXMATRIX scale;
	Unknown_Scaling(&scale, Field->Get_Spacing() * size, Field->Get_YSpacing() * size, Field->Get_VScale() * size);
'''))],
    'd3dvec_mul': [(OLD, pad('''	float size = (float)(unsigned int)Field->Get_Size();
	D3DXVECTOR3 s = D3DXVECTOR3(Field->Get_Spacing(), Field->Get_YSpacing(), Field->Get_VScale()) * size;
	D3DXMATRIX scale;
	D3DXMatrixScaling(&scale, s.x, s.y, s.z);
'''))],
    'locals_first': [(OLD, pad('''	float xs = Field->Get_Spacing();
	float ys = Field->Get_YSpacing();
	float size = (float)(unsigned int)Field->Get_Size();
	D3DXMATRIX scale;
	D3DXMatrixScaling(&scale, xs * size, ys * size, Field->Get_VScale() * size);
'''))],
    'locals_first_z': [(OLD, pad('''	float xs = Field->Get_Spacing();
	float ys = Field->Get_YSpacing();
	float zs = Field->Get_VScale();
	float size = (float)(unsigned int)Field->Get_Size();
	D3DXMATRIX scale;
	D3DXMatrixScaling(&scale, xs * size, ys * size, zs * size);
'''))],
    'vec3_mul': [(OLD, pad('''	float size = (float)(unsigned int)Field->Get_Size();
	Vector3 s = Vector3(Field->Get_Spacing(), Field->Get_YSpacing(), Field->Get_VScale()) * size;
	D3DXMATRIX scale;
	D3DXMatrixScaling(&scale, s.X, s.Y, s.Z);
'''))],
    'arr2': [(OLD, pad('''	float size = (float)(unsigned int)Field->Get_Size();
	float sp[2]; sp[0] = Field->Get_Spacing(); sp[1] = Field->Get_YSpacing();
	D3DXMATRIX scale;
	D3DXMatrixScaling(&scale, sp[0] * size, sp[1] * size, Field->Get_VScale() * size);
'''))],
}
