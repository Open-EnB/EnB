// docs/attempts/0x5790C0: the tree's form after builder L10's rounds (run 2026-11-11), register-only; see attempts.csv
// 0x5790C0: owner NULL (the first constructor): the gadgets on Layer; else on an empty layer
void UIComboBoxClass::Init(InterfaceClass *owner)
{
	Anim = new HAnimComboClass(2);
	Unknown_BE67BC->Load("fc_combobox.ini");
	int scale = 1;
	if (Style.size() == 0) {
		Mesh = new UIMeshClass("Combo00", 1);
		WidthAnim = Create_Get(WW3DAssetManager::Get_Instance()->Get_HAnim("Combo00.Combo00-1"));
		DropAnim = Create_Get(WW3DAssetManager::Get_Instance()->Get_HAnim("Combo00.Combo00-2"));
	} else {
		Mesh = new UIMeshClass(Style.c_str(), 1);
		char anim_name[0x40];
		_snprintf(anim_name, 0x40, "%s.%s-1", Style.c_str(), Style.c_str());
		WidthAnim = Create_Get(WW3DAssetManager::Get_Instance()->Get_HAnim(anim_name));
		_snprintf(anim_name, 0x40, "%s.%s-2", Style.c_str(), Style.c_str());
		DropAnim = Create_Get(WW3DAssetManager::Get_Instance()->Get_HAnim(anim_name));
		if (Style == "Combo01") {
			scale = 4;
		}
	}
	Mesh->Peek_Object()->Set_Sort_Level(4);
	Anim->Set_Motion(0, WidthAnim.Peek());
	Anim->Set_Motion(1, DropAnim.Peek());
	Anim->Set_Weight(0, 1.0f);
	Anim->Set_Weight(1, 1.0f);
	Anim->Set_Frame(0, (float)(Width * scale));
	Mesh->Peek_Object()->Set_Animation(Anim);
	RenderObjClass *drop = Mesh->Unknown_5B8B40("DROP_DOWN00");
	drop->Set_Sort_Level(4);
	drop->Set_Hidden(1);
	drop->Release_Ref();
#line 310
	G_ASSERT(AttachBone);
	Model->Add_Sub_Object_To_Bone(Mesh->Peek_Object(), AttachBone);
	Matrix3D tm = Mesh->Unknown_5B9210("TEXTFIELD01");
	int right = (int)tm.Get_X_Translation();
	for (int i = 0; i < MaxRows; i++) {
		char bone[10];
		_snprintf(bone, 10, "TBONE%02d", i + 1);
		Matrix3D bone_tm = Mesh->Unknown_5B9210(bone);
		int left = (int)bone_tm.Get_X_Translation();
		UIComboBoxTextClass *row = NULL;
		if (owner) {
			row = new UIComboBoxTextClass(Mesh, bone, right - left);
		} else {
			row = new UIComboBoxTextClass(Mesh, bone, Layer, right - left);
		}
		row->Unknown_65FFA0(-3);
		row->Unknown_40(false);
		row->Unknown_8C(true);
		Unknown64.Unknown_6870E0(row, i + 3, View, true, false);
		Rows.push_back(row);
	}
	if (owner) {
		Arrow = new ButtonGadgetClass(Mesh, "ARROWBOX00", 5, 3);
	} else {
		Arrow = new ButtonGadgetClass(Mesh, "ARROWBOX00", *Layer, 5, 3);
	}
	Arrow->Unknown_65FFA0(-2);
	Unknown_496AE0(Arrow->RObj, false);
	Unknown64.Unknown_6870E0(Arrow, 1, View, true, false);
	Matrix3D left_tm = Mesh->Unknown_5B9210("TEXTFIELD00");
	int left = (int)left_tm.Get_X_Translation();
	tm = Mesh->Unknown_5B9210("TEXTFIELD02");
	right = (int)tm.Get_X_Translation();
	if (owner) {
		TextField = new UIComboBoxTextClass(Mesh, "TEXTFIELD00", right - left);
	} else {
		TextField = new UIComboBoxTextClass(Mesh, "TEXTFIELD00", Layer, right - left);
	}
	TextField->Unknown_65FFA0(-1);
	TextField->RObj->Set_Sort_Level(5);
	Unknown64.Unknown_6870E0(TextField, 1, View, true, false);
	if (owner) {
		Slider = new GadgetSlider(false, NULL);
	} else {
		Slider = new GadgetSlider(false, Layer);
	}
	Slider->Unknown_65FFA0(-2);
	Unknown64.Unknown_6870E0(Slider, 2, View, true, false);
	Selected = 0;
}
