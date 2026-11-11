// 0x6F1AD0 LinearRandomSeekEffectClass::Unknown_1C: forms tried (L4, 2026-11-10). None exact.
// Form A (the tree's; order-only: the frame 0x58 and the 96 instructions are the client's). The client adds the
// squared length as z*z + x*x + y*y (0x6F1B3E-0x6F1B4F) where Vector3::Length2 gives z*z + y*y + x*x, and in the
// translation's second and third rows it loads the matrix element first (0x6F1BAB-0x6F1BB4) where this loads
// the point's.
// 0x6F1AD0
void LinearRandomSeekEffectClass::Unknown_1C()
{
	Unknown6F0580EffectClass::Unknown_1C();
	float time = (float)(Client->Unknown10BC - StartTime);
	Vector3 point;
	((Curve3DClass &)Curve).Evaluate(time, &point);
	Vector3 direction = TargetPosition - SourcePosition;
	point.X *= direction.Length();
	Matrix3D tm = Unknown_82ECB0(direction, SourcePosition);
	tm.Set_Translation(tm * point);
	RObj->Set_Transform(tm);
}

// Form B (verify round 2): `tm.Translate(point);` for `tm.Set_Translation(tm * point);`: frame 0x54 against the
// client's 0x58, 92 instructions against 96. Worse.
