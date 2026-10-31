import io, os
F = 'app/shared/landdmap.cpp'
_src = io.open(os.path.join(r'C:\EnBDevelopment\EnB', F), encoding='utf-8', newline='').read().replace('\r\n', '\n')
_a = _src.index('void LandUnknown466BE0Class::Unknown_466F60(RenderInfoClass &rinfo)\n')
_b = _src.index('void LandUnknown466BE0Class::Create_Displacement_Map(void)\n')
OLD = _src[_a:_b]

DEV = 'DX8Wrapper::_Get_D3D_Device8()->'

SCALE = {
    'u': """	float size = (float)(unsigned int)Field->Get_Size();
	D3DXMATRIX scale;
	D3DXMatrixScaling(&scale, Field->Get_Spacing() * size, Field->Get_YSpacing() * size, Field->Get_VScale() * size);
""",
}

SCALE['xy'] = """	float size = (float)(unsigned int)Field->Get_Size();
	float xs = Field->Get_Spacing();
	float ys = Field->Get_YSpacing();
	D3DXMATRIX scale;
	D3DXMatrixScaling(&scale, xs * size, ys * size, Field->Get_VScale() * size);
"""
SCALE['yx'] = """	float size = (float)(unsigned int)Field->Get_Size();
	float ys = Field->Get_YSpacing();
	float xs = Field->Get_Spacing();
	D3DXMATRIX scale;
	D3DXMatrixScaling(&scale, xs * size, ys * size, Field->Get_VScale() * size);
"""
SCALE['xyz'] = """	float size = (float)(unsigned int)Field->Get_Size();
	float xs = Field->Get_Spacing() * size;
	float ys = Field->Get_YSpacing() * size;
	float zs = Field->Get_VScale() * size;
	D3DXMATRIX scale;
	D3DXMatrixScaling(&scale, xs, ys, zs);
"""
SCALE['vec'] = """	float size = (float)(unsigned int)Field->Get_Size();
	Vector3 spacing(Field->Get_Spacing(), Field->Get_YSpacing(), Field->Get_VScale());
	D3DXMATRIX scale;
	D3DXMatrixScaling(&scale, spacing.X * size, spacing.Y * size, spacing.Z * size);
"""

SCALE['zyx'] = """	float size = (float)(unsigned int)Field->Get_Size();
	float zs = Field->Get_VScale() * size;
	float ys = Field->Get_YSpacing() * size;
	float xs = Field->Get_Spacing() * size;
	D3DXMATRIX scale;
	D3DXMatrixScaling(&scale, xs, ys, zs);
"""
SCALE['top'] = """	float size = (float)(unsigned int)Field->Get_Size();
	float xs = Field->Get_Spacing() * size;
	float ys = Field->Get_YSpacing() * size;
	float zs = Field->Get_VScale() * size;
	D3DXMATRIX scale;
	D3DXMATRIX proj;
	D3DXMATRIX view;
	D3DXMATRIX world;
	D3DXMatrixScaling(&scale, xs, ys, zs);
"""

TRANS = {
    'loop': """	for (int r = 0; r < 4; r++) {
		for (int c = 0; c < 3; c++) {
			view.m[r][c] = camview[c][r];
		}
	}
	view.m[0][3] = 0.0f;
	view.m[1][3] = 0.0f;
	view.m[2][3] = 0.0f;
	view.m[3][3] = 1.0f;
""",
    'flat': """	view._11 = camview[0][0];
	view._12 = camview[1][0];
	view._13 = camview[2][0];
	view._14 = 0.0f;
	view._21 = camview[0][1];
	view._22 = camview[1][1];
	view._23 = camview[2][1];
	view._24 = 0.0f;
	view._31 = camview[0][2];
	view._32 = camview[1][2];
	view._33 = camview[2][2];
	view._34 = 0.0f;
	view._41 = camview[0][3];
	view._42 = camview[1][3];
	view._43 = camview[2][3];
	view._44 = 1.0f;
""",
}

STATIC = {
    'struct': """	static LandDMapConstantStruct consts(0.0f, 1.0f, 0.5f, 2.0f);
	%sSetVertexShaderConstant(11, &consts, 1);
""" % DEV,
}


def body(scale, trans, static):
    return ("""void LandUnknown466BE0Class::Unknown_466F60(RenderInfoClass &rinfo)
{
	if (!Unknown11) {
		return;
	}
""" + SCALE[scale] + """
	D3DXMATRIX proj;
	D3DXMATRIX view;
	D3DXMATRIX world;
	DEV GetTransform(D3DTS_PROJECTION, &proj);
	DEV GetTransform(D3DTS_VIEW, &view);
	DEV GetTransform(D3DTS_WORLD, &world);
	DX8Wrapper::Apply_Render_State_Changes();

	Matrix3D camview = rinfo.Camera.Get_View_Matrix();
""" + TRANS[trans] + """
	D3DXMATRIX flip(1.0f, 0.0f, 0.0f, 0.0f,
						0.0f, 1.0f, 0.0f, 0.0f,
						0.0f, 0.0f, -1.0f, 0.0f,
						0.0f, 0.0f, 0.0f, 1.0f);
	D3DXMATRIX wvp;
	D3DXMatrixMultiply(&wvp, &scale, &view);
	D3DXMatrixMultiply(&wvp, &wvp, &proj);
	D3DXMatrixTranspose(&wvp, &wvp);
	D3DXMATRIX d3dview;
	D3DXMatrixMultiply(&d3dview, &view, &flip);
	DEV SetTransform(D3DTS_VIEW, &d3dview);
	DEV SetVertexShaderConstant(6, &wvp, 4);

	DWORD fog;
	DEV GetRenderState(D3DRS_FOGENABLE, &fog);
	DEV GetRenderState(D3DRS_FOGSTART, (DWORD *)&FogStart);
	DEV GetRenderState(D3DRS_FOGEND, (DWORD *)&FogEnd);
	FogRange = FogEnd - FogStart;
	DEV SetVertexShaderConstant(10, &FogStart, 1);
""" + STATIC[static] + """
	if (Unknown0C) {
		Unknown_468240(rinfo.Camera, scale);
	}
	Unknown_468D50();

	float tess[4];
	tess[0] = TessellationRate;
	tess[1] = 0.0f;
	tess[2] = 32.0f;
	*(DWORD *)&tess[3] = 1;
	DEV SetVertexShaderConstant(4, tess, 1);
	D3DXVECTOR4 c5(1.0f, 0.0f, 0.0f, 0.0f);
	DEV SetVertexShaderConstant(5, &c5, 1);

	DEV SetTextureStageState(4, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
	DEV SetTextureStageState(4, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
	DEV SetTextureStageState(4, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);
	DEV SetTextureStageState(4, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
	DEV SetTextureStageState(4, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
	DEV SetTexture(4, DisplacementMap);
	DEV SetVertexShader(VertexShader);
	if (Unknown12) {
		DEV SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	}
	DEV SetStreamSource(0, VertexBuffer, 0x34);
	DEV SetIndices(IndexBuffer, 0);
	DEV SetTransform(D3DTS_WORLD, &scale);
	DEV DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, VertexCount, 0, IndexCount / 3);
	if (Unknown12) {
		DEV SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	}
	DEV SetTransform(D3DTS_VIEW, &view);
	DEV SetTransform(D3DTS_WORLD, &world);
}

""").replace('DEV ', DEV)

STRUCT_ANCHOR = 'LandUnknown466BE0Class *Unknown_BDC0D8;'
STRUCT = STRUCT_ANCHOR + """

// NAME 1: unknown; a vertex shader constant with an empty destructor (the client's function-local static registers 0x4676B0
// through atexit)
struct LandDMapConstantStruct
{
	LandDMapConstantStruct(float x, float y, float z, float w)	{ V[0] = x; V[1] = y; V[2] = z; V[3] = w; }
	~LandDMapConstantStruct(void)	{ }
	float V[4];
};"""

TESS_OLD = """	float tess[4];
	tess[0] = TessellationRate;
	tess[1] = 0.0f;
	tess[2] = 32.0f;
	*(DWORD *)&tess[3] = 1;
"""
TESS = {
 'dw': """	DWORD tess[4];
	tess[0] = *(DWORD *)&TessellationRate;
	tess[3] = 1;
	tess[1] = 0;
	tess[2] = 0x42000000;
""",
 'w_first': """	float tess[4];
	tess[0] = TessellationRate;
	*(DWORD *)&tess[3] = 1;
	tess[1] = 0.0f;
	tess[2] = 32.0f;
""",
}
def B(sc, tess=None):
    b = body(sc, 'flat', 'struct')
    if sc == 'top':
        b = b.replace("""
	D3DXMATRIX proj;
	D3DXMATRIX view;
	D3DXMATRIX world;
	DX8""", """
	DX8""", 1)
    if tess:
        assert TESS_OLD in b
        b = b.replace(TESS_OLD, TESS[tess])
    return b
V = {}
for sc in ('zyx', 'top'):
    V[sc] = [(F, STRUCT_ANCHOR, STRUCT), (F, OLD, B(sc))]
for t in TESS:
    V['xyz_' + t] = [(F, STRUCT_ANCHOR, STRUCT), (F, OLD, B('xyz', t))]
COMPILE = [F]
WATCH = ['0x466F60']
