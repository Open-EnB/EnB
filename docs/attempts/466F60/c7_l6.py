HYPOTHESIS = '0x466F60 round 6 (probe, not a landing form): does a by-value struct copy of the X / Y spacing (adjacent members +0x0C / +0x10) give the client int-move temps; reached through a raw offset only to measure'
FILE = 'app/shared/landdmap.cpp'
SYMBOL = 'LandUnknown466BE0Class::Unknown_466F60'
ADDRESS = 0x466F60

OLD = '''	float spacing[3] = { Field->Get_Spacing(), Field->Get_YSpacing(), Field->Get_VScale() };
	float size = (float)(unsigned int)Field->Get_Size();
	D3DXMATRIX scale;
	D3DXMatrixScaling(&scale, spacing[0] * size, spacing[1] * size, spacing[2] * size);
'''
SP = '(*(const %s *)((const char *)Field + 0x0C))'
FORMS = {
    'v2copy_after': [(OLD, '''	float size = (float)(unsigned int)Field->Get_Size();
	Vector2 sp = %s;
	D3DXMATRIX scale;
	D3DXMatrixScaling(&scale, sp.X * size, sp.Y * size, Field->Get_VScale() * size);
''' % (SP % 'Vector2'))],
    'v2copy_first': [(OLD, '''	Vector2 sp = %s;
	float size = (float)(unsigned int)Field->Get_Size();
	D3DXMATRIX scale;
	D3DXMatrixScaling(&scale, sp.X * size, sp.Y * size, Field->Get_VScale() * size);
''' % (SP % 'Vector2'))],
    'v3copy_first': [(OLD, '''	Vector3 sp = %s;
	float size = (float)(unsigned int)Field->Get_Size();
	D3DXMATRIX scale;
	D3DXMatrixScaling(&scale, sp.X * size, sp.Y * size, sp.Z * size);
''' % (SP % 'Vector3'))],
    'v3copy_after': [(OLD, '''	float size = (float)(unsigned int)Field->Get_Size();
	Vector3 sp = %s;
	D3DXMATRIX scale;
	D3DXMatrixScaling(&scale, sp.X * size, sp.Y * size, sp.Z * size);
''' % (SP % 'Vector3'))],
    'v2ref_after': [(OLD, '''	float size = (float)(unsigned int)Field->Get_Size();
	const Vector2 &sp = %s;
	D3DXMATRIX scale;
	D3DXMatrixScaling(&scale, sp.X * size, sp.Y * size, Field->Get_VScale() * size);
''' % (SP % 'Vector2'))],
}
