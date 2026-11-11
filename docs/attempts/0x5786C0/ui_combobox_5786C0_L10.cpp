// docs/attempts/0x5786C0: the tree's form after builder L10's rounds (run 2026-11-11), inline-budget; see attempts.csv
// 0x5786C0
UIComboBoxClass::UIComboBoxClass(InterfaceClass *owner, LayerClass *layer, RenderObjClass *model, const char *name, int select_key, int toggle_key, int width, int max_rows, const char *style) :
	InterfaceClass(owner->Get_View(), (int)layer->Peek_Scene()),
	AttachBone(0),
	Model(model),
	Style(""),
	Layer(layer),
	Width(width),
	MaxRows(max_rows),
	NumRows(0),
	Owner(owner),
	SelectKey(select_key),
	ToggleKey(toggle_key),
	State(0),
	RowsShown(0),
	Arrow(NULL),
	Slider(NULL),
	TopItem(0),
	Selected(0),
	Label(""),
	ArrowHidden(false)
{
	if (style && *style) {
		Style = style;
	}
	AttachBone = Model->Get_Bone_Index(name);
	Init(NULL);
	InterfaceClass::Unknown_58();
}
