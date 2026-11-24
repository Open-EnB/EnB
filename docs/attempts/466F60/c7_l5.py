HYPOTHESIS = '0x466F60 round 5: the X / Y spacing copies declared before the Unknown11 early return (VC6 sinks them past the test as int-move copies into temps), with the address-taken DWORD one of round 4'
FILE = 'app/shared/landdmap.cpp'
SYMBOL = 'LandUnknown466BE0Class::Unknown_466F60'
ADDRESS = 0x466F60

OLD_SC = '''	if (!Unknown11) {
		return;
	}
	float size = (float)(unsigned int)Field->Get_Size();
	float xs = Field->Get_Spacing() * size;
	float ys = Field->Get_YSpacing() * size;
	float zs = Field->Get_VScale() * size;
	D3DXMATRIX scale;
	D3DXMatrixScaling(&scale, xs, ys, zs);
'''
OLD_T = '''	float tess[4];
	tess[0] = TessellationRate;
	tess[1] = 0.0f;
	tess[2] = 32.0f;
	*(DWORD *)&tess[3] = 1;
	DX8Wrapper::_Get_D3D_Device8()->SetVertexShaderConstant(4, tess, 1);
'''
DEV = 'DX8Wrapper::_Get_D3D_Device8()->'
NEW_T = '\tDWORD one = 1;\n\tfloat tess[4];\n\ttess[0] = TessellationRate; tess[1] = 0.0f; tess[2] = 32.0f;\n\ttess[3] = *(float *)&one;\n\t' + DEV + 'SetVertexShaderConstant(4, tess, 1);\n\n'

def pad(s, n):
    k = s.count('\n')
    assert k <= n, (k, s)
    return s + '\n' * (n - k)

RET = '\tif (!Unknown11) {\n\t\treturn;\n\t}\n'
SZ = '\tfloat size = (float)(unsigned int)Field->Get_Size();\n'
SC = {
    'xy_pre': '\tfloat xs = Field->Get_Spacing();\n\tfloat ys = Field->Get_YSpacing();\n' + RET + SZ + '\tD3DXMATRIX scale;\n\tD3DXMatrixScaling(&scale, xs * size, ys * size, Field->Get_VScale() * size);\n',
    'xyz_pre': '\tfloat xs = Field->Get_Spacing();\n\tfloat ys = Field->Get_YSpacing();\n\tfloat zs = Field->Get_VScale();\n' + RET + SZ + '\tD3DXMATRIX scale; D3DXMatrixScaling(&scale, xs * size, ys * size, zs * size);\n',
    'v3_pre': '\tVector3 sp(Field->Get_Spacing(), Field->Get_YSpacing(), Field->Get_VScale());\n' + RET + SZ + '\tD3DXMATRIX scale;\n\tD3DXMatrixScaling(&scale, sp.X * size, sp.Y * size, sp.Z * size);\n',
    'arr3_pre': '\tfloat sp[3] = { Field->Get_Spacing(), Field->Get_YSpacing(), Field->Get_VScale() };\n' + RET + SZ + '\tD3DXMATRIX scale;\n\tD3DXMatrixScaling(&scale, sp[0] * size, sp[1] * size, sp[2] * size);\n',
    'size_pre': SZ + '\tfloat xs = Field->Get_Spacing();\n\tfloat ys = Field->Get_YSpacing();\n' + RET + '\tD3DXMATRIX scale;\n\tD3DXMatrixScaling(&scale, xs * size, ys * size, Field->Get_VScale() * size);\n',
    'arr3_post': RET + '\tfloat sp[3] = { Field->Get_Spacing(), Field->Get_YSpacing(), Field->Get_VScale() };\n' + SZ + '\tD3DXMATRIX scale;\n\tD3DXMatrixScaling(&scale, sp[0] * size, sp[1] * size, sp[2] * size);\n',
}
FORMS = {}
for s in SC:
    FORMS[s] = [(OLD_SC, pad(SC[s], 9)), (OLD_T, NEW_T)]
