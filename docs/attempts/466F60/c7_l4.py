HYPOTHESIS = '0x466F60 round 4: the second store of 1 (+0x4C, after the pushes, never read) is an address-taken DWORD one read back as the float bits of constant 4 (VC6 keeps the store, sinks it to the call); with the round-3 scaling forms'
FILE = 'app/shared/landdmap.cpp'
SYMBOL = 'LandUnknown466BE0Class::Unknown_466F60'
ADDRESS = 0x466F60

OLD_SC = '''	float size = (float)(unsigned int)Field->Get_Size();
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

def pad(s, n):
    k = s.count('\n')
    assert k <= n, (k, s)
    return s + '\n' * (n - k)

SZ = '\tfloat size = (float)(unsigned int)Field->Get_Size();\n'
SC = {
    'arr3': '\tfloat sp[3] = { Field->Get_Spacing(), Field->Get_YSpacing(), Field->Get_VScale() };\n' + SZ + '\tD3DXMATRIX scale;\n\tD3DXMatrixScaling(&scale, sp[0] * size, sp[1] * size, sp[2] * size);\n',
    'v3zyx': '\tVector3 sp(Field->Get_Spacing(), Field->Get_YSpacing(), Field->Get_VScale());\n' + SZ + '\tD3DXMATRIX scale;\n\tfloat zs = sp.Z * size; float ys = sp.Y * size; float xs = sp.X * size;\n\tD3DXMatrixScaling(&scale, xs, ys, zs);\n',
    'v3': '\tVector3 sp(Field->Get_Spacing(), Field->Get_YSpacing(), Field->Get_VScale());\n' + SZ + '\tD3DXMATRIX scale;\n\tD3DXMatrixScaling(&scale, sp.X * size, sp.Y * size, sp.Z * size);\n',
}
DEV = 'DX8Wrapper::_Get_D3D_Device8()->'
T = {
    'base': None,
    'one_f': '\tDWORD one = 1;\n\tfloat tess[4];\n\ttess[0] = TessellationRate; tess[1] = 0.0f; tess[2] = 32.0f;\n\ttess[3] = *(float *)&one;\n\t' + DEV + 'SetVertexShaderConstant(4, tess, 1);\n',
    'one_f_first': '\tfloat tess[4];\n\tDWORD one = 1;\n\ttess[0] = TessellationRate; tess[1] = 0.0f; tess[2] = 32.0f;\n\ttess[3] = *(float *)&one;\n\t' + DEV + 'SetVertexShaderConstant(4, tess, 1);\n',
    'one_v4': '\tDWORD one = 1;\n\tD3DXVECTOR4 tess(TessellationRate, 0.0f, 32.0f, *(float *)&one);\n\t' + DEV + 'SetVertexShaderConstant(4, &tess, 1);\n',
    'one_dw': '\tDWORD one = 1;\n\tfloat tess[4];\n\ttess[0] = TessellationRate; tess[1] = 0.0f; tess[2] = 32.0f;\n\t*(DWORD *)&tess[3] = *(DWORD *)&one;\n\t' + DEV + 'SetVertexShaderConstant(4, tess, 1);\n',
}
FORMS = {}
for s in SC:
    for t in T:
        h = [(OLD_SC, pad(SC[s], 6))]
        if T[t]:
            h.append((OLD_T, pad(T[t], 6)))
        FORMS['%s_%s' % (s, t)] = h
for t in T:
    if T[t]:
        FORMS['sc0_%s' % t] = [(OLD_T, pad(T[t], 6))]
