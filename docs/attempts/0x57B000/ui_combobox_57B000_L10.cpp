// docs/attempts/0x57B000: the tree's form after builder L10's rounds (run 2026-11-11), inline-budget; see attempts.csv
// 0x57B000
void UIComboBoxClass::Add_Item(const char *text)
{
	NumRows++;
	NumRows = min(MaxRows, NumRows);
	Items.insert(Items.end(), 1, std::string(text));
	if (Items.size() == 1) {
		Selected = 0;
		Show_Selected();
	}
}
