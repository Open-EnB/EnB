// docs/attempts/0x578350: the tree's form after builder L10's rounds (run 2026-11-11), shape; see attempts.csv
// 0x578350
void UIComboBoxTextClass::Init()
{
	Text = new FontSys("Combobox_Default", NULL, NULL);
	RObj = Text;
	Unknown2C_0A = true;
	Mesh->Peek_Object()->Add_Sub_Object_To_Bone(Text, Bone);
	Unknown_497240(Mesh->Peek_Object(), false);
	Vector3 pos = Text->Get_Position();
	float height = Text->Get_Bounding_Box().Extent.Y;
	Text->Unknown4E4 = false;
	Text->Unknown_6D88F0((int)pos.X, (int)(height + pos.Y), (int)(Width + pos.X), (int)(pos.Y - height));
}
