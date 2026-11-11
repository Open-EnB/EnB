// 0x6F1390 SplineRandomSeekEffectClass::Unknown_1C: one form (L4, 2026-11-10), order-only.
// The tree's form: the frame and all 141 instructions are the client's. The one difference, 0x6F1496-0x6F149F:
// the facing vector's X (Matrix3D::Rotate_Vector's first row) is `fld tm[0][0]; fmul rate.X; fld tm[0][1];
// fmul rate.Y` in the client and `fld rate.X; fmul tm[0][0]; fld rate.Y; fmul tm[0][1]` here (rate.X is the
// member multiplied by the length just before). Not retried (order-only).
// 0x6F1390
void SplineRandomSeekEffectClass::Unknown_1C()
{
	Unknown6F0580EffectClass::Unknown_1C();
	float time = (float)(Client->Unknown10BC - StartTime);
	Vector3 point;
	((Curve3DClass &)Spline).Evaluate(time, &point);
	Vector3 rate;
	((HermiteSpline3DClass &)Spline).Evaluate_Derivative(time, &rate);
	Vector3 direction = TargetPosition - SourcePosition;
	float length = direction.Length();
	point.X *= length;
	rate.X *= length;
	Matrix3D tm = Unknown_82ECB0(direction, SourcePosition);
	Vector3 position = tm * point;
	Vector3 facing;
	Matrix3D::Rotate_Vector(tm, rate, &facing);
	RObj->Set_Transform(Unknown_82ECB0(facing, position));
}
