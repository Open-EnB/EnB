// 0x6EF740 Unknown6EF4C0EffectClass::Unknown_1C (eff_robj.cpp): the forms tried (L2, 2026-11-10)
//
// Form 1 (frame: 0x54 vs the client's 0x48; identical once stack offsets are masked): a named pos
//	if (SourceObject && Get_Flag(7)) {
//		Transform.Adjust_Translation(UnknownE4 * SourceObject->Unknown_30());
//	}
//	Vector3 pos = Transform.Get_Translation();
//	Vector3 screen;
//	if (Camera->Project(screen, pos) == CameraClass::INSIDE_FRUSTUM) {
//		pos.X = screen.X + UnknownDC.X;
//		pos.Y = screen.Y + UnknownDC.Y;
//		pos.Z = 0;
//		RObj->Set_Transform(Matrix3D(1));
//		RObj->Set_Position(pos);
//		RObj->Set_Hidden(false);
//	} else {
//		RObj->Set_Hidden(true);
//	}
//
// Form 2 (frame 0x54, 14 masked rows): one named pos for the product and the translation
//	Vector3 pos;
//	if (SourceObject && Get_Flag(7)) {
//		pos = UnknownE4 * SourceObject->Unknown_30();
//		Transform.Adjust_Translation(pos);
//	}
//	pos = Transform.Get_Translation();
//	(the rest as form 1)
//
// Form 3 (the tree's: the client's frame 0x48 and its 96 instructions, order-only): the translation as Project's
// temporary (it shares the product's slot, as the client's), pos constructed in the branch
//	Vector3 screen;
//	if (Camera->Project(screen, Transform.Get_Translation()) == CameraClass::INSIDE_FRUSTUM) {
//		Vector3 pos(screen.X + UnknownDC.X, screen.Y + UnknownDC.Y, 0);
//		RObj->Set_Transform(Matrix3D(1));
//		RObj->Set_Position(pos);
//	(the client stores the matrix's [esp+0x28] / [esp+0x2c] zeros before pos.X's fstp and pos.Y's fstp before
//	the matrix's second 1.0f; ours stores them three and one instructions later)
//
// Form 4 (shape, 14 masked rows): Set_Position(Vector3(screen.X + UnknownDC.X, screen.Y + UnknownDC.Y, 0)) after
// Set_Transform: the sums are computed after the first call, where the client computes them before it.
