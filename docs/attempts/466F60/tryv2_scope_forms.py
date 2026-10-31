F = 'app/shared/landdmap.cpp'
SC_OLD = """	float size = (float)(unsigned int)Field->Get_Size();
	float xs = Field->Get_Spacing() * size;
	float ys = Field->Get_YSpacing() * size;
	float zs = Field->Get_VScale() * size;
	D3DXMATRIX scale;
	D3DXMatrixScaling(&scale, xs, ys, zs);
"""
SC_BLK = """	D3DXMATRIX scale;
	{
		float size = (float)(unsigned int)Field->Get_Size();
		float xs = Field->Get_Spacing() * size;
		float ys = Field->Get_YSpacing() * size;
		float zs = Field->Get_VScale() * size;
		D3DXMatrixScaling(&scale, xs, ys, zs);
	}
"""
CAM_HEAD = "\tMatrix3D camview = rinfo.Camera.Get_View_Matrix();\n"
CAM_TAIL = "\tview._44 = 1.0f;\n"
TESS_OLD = """	float tess[4];
	tess[0] = TessellationRate;
	tess[1] = 0.0f;
	tess[2] = 32.0f;
	*(DWORD *)&tess[3] = 1;
	DX8Wrapper::_Get_D3D_Device8()->SetVertexShaderConstant(4, tess, 1);
	D3DXVECTOR4 c5(1.0f, 0.0f, 0.0f, 0.0f);
	DX8Wrapper::_Get_D3D_Device8()->SetVertexShaderConstant(5, &c5, 1);
"""
TESS_BLK = "\t{\n" + ''.join('\t' + l + '\n' for l in TESS_OLD.splitlines()) + "\t}\n"
TESS_ONE = """	DWORD one = 1;
	float tess[4];
	tess[0] = TessellationRate;
	tess[1] = 0.0f;
	tess[2] = 32.0f;
	*(DWORD *)&tess[3] = one;
	DX8Wrapper::_Get_D3D_Device8()->SetVertexShaderConstant(4, tess, one);
	D3DXVECTOR4 c5(1.0f, 0.0f, 0.0f, 0.0f);
	DX8Wrapper::_Get_D3D_Device8()->SetVertexShaderConstant(5, &c5, 1);
"""


def cam_block(src):
    return [(F, CAM_HEAD, "\t{\n" + CAM_HEAD.replace('\t', '\t\t', 1)), (F, CAM_TAIL, CAM_TAIL + "\t}\n")]


V = {
    'scale_blk': [(F, SC_OLD, SC_BLK)],
    'cam_blk': cam_block(None),
    'tess_blk': [(F, TESS_OLD, TESS_BLK)],
    'tess_one': [(F, TESS_OLD, TESS_ONE)],
    'scale_cam_tess_blk': [(F, SC_OLD, SC_BLK)] + cam_block(None) + [(F, TESS_OLD, TESS_BLK)],
}
COMPILE = [F]
WATCH = ['0x466F60']
