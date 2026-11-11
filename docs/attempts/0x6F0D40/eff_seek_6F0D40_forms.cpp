// 0x6F0D40 SpiralSeekEffectClass::Unknown_1C: forms tried (L4, 2026-11-10). None exact.
// The client: frame 0xB0; t [ebp-4], radius_rate [ebp-8], the kept product radius * s [ebp-0xC] (stored with fst,
// then fstp into point.Y), point [ebp-0x1C], rate [ebp-0x28], direction [ebp-0x34], the sine [ebp-0x38]; spin,
// the cosine and radius stay on the FPU stack. After Length()'s fsqrt: fld st(0); fmul t; then point.X and rate.X
// are stored from the stack in that order (0x6F0E3D-0x6F0E4E).

// Form A (the tree's: frame 0xB0, the sine's slot and the kept product are the client's; rate, direction, point
// are in another order in the frame, and rate.X is stored with fst before the multiply):
// 0x6F0D40
void SpiralSeekEffectClass::Unknown_1C()
{
	Unknown6F0580EffectClass::Unknown_1C();
	float t = (float)(Client->Unknown10BC - StartTime) * InvDuration;
	float spin = (float)Duration * SpinRate;
	float s = (float)sin(spin * t);
	float c = (float)cos(spin * t);
	float radius = (t - t * t) * Radius * 4.0f;
	float radius_rate = (1.0f - (t + t)) * Radius * 4.0f;
	Vector3 point;
	Vector3 rate;
	point.Y = radius * s;
	point.Z = radius * c;
	rate.Y = radius_rate * s + radius * c * spin;
	rate.Z = radius_rate * c - radius * s * spin;
	Vector3 direction = TargetPosition - SourcePosition;
	rate.X = direction.Length();
	point.X = rate.X * t;
	Matrix3D tm = Unknown_82ECB0(direction, SourcePosition);
	Vector3 position = tm * point;
	Vector3 facing;
	Matrix3D::Rotate_Vector(tm, rate, &facing);
	RObj->Set_Transform(Unknown_82ECB0(facing, position));
}

// Form B (verify round 1): as A, but
//	rate.Y = radius_rate * s + point.Z * spin;
//	rate.Z = radius_rate * c - point.Y * spin;
// point and rate are where the client's are; the frame is 0xAC (no kept product: point.Y is read back), the
// sine sits at [ebp-0x10].

// Form C (verify round 2): named float y, z, y_rate, z_rate, then Vector3 point(length * t, y, z) and
// Vector3 rate(length, y_rate, z_rate) after the length: frame 0xBC, spin and the cosine spilled. Worse.

// Form D (verify round 3): B with `float y = radius * s; point.Y = y; float z = radius * c; point.Z = z;` and
// `float length = direction.Length(); point.X = length * t; rate.X = length;`: frame 0xAC, no kept product,
// the length stored to its own slot (fst) before the multiply.

// Forms S1-S4 (tryv2, .work/seek/v1.py): A's head with the tail of D (S1: fst to the length's slot), the sine and
// cosine written out in each use with a named angle (S2, S3: the cosine is not kept on the stack, fmul st(2)).

// tryv2 sets 2 and 3 (.work/seek/v2.py, v3.py; each compiled, none exact). Head M1 is A's with
// `rate.Y = radius_rate * s + point.Z * spin;` (one read through point; the kept product stays). Frame orders
// (highest first; the client's is point, rate, direction):
//	A  (no read; rate.X = Length(); point.X = rate.X * t)			rate, direction, point
//	M1 + that tail													rate, point, direction
//	M1 + `point.X = direction.Length(); rate.X = point.X; point.X *= t;`	point, direction, rate
//	M1 + `rate.X = point.X = direction.Length(); point.X *= t;`		point, direction, rate
//	M1 + `point.X = rate.X = direction.Length(); point.X *= t;`		rate, point, direction
//	B  (both reads through point)									point, rate, direction (no kept product)
// A named `float length` (either order of the two stores) takes [ebp-4] and moves t and the sine.
// In every form the length reaches a vector member with fst before the multiply; the client's fld st(0) with
// both stores after the pushes (point.X, then rate.X) is not found. What is left: a form that reads point more
// often than rate without reading point.Y (the kept product), and the tail.
