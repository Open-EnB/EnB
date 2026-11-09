// @0x653450
void UITradeClass::Unknown_653450()
{
	SlotDataGadgetClass *button = Unknown88[0];
	FAIL_IF(!button) return;

	SlotDataRenderClass *sd_render = button->SlotRender;
	FAIL_IF(!sd_render) return;

	InvTradeRenderClass *trade_render = (InvTradeRenderClass *)sd_render->Unknown_24();
	FAIL_IF(!trade_render) return;
	InvDataPeekClass *peek = trade_render->Peek_Inv_Data()->Unknown_688830();
	Unknown17C.clear();
	bool by_name = OptionClass::Instance->Get_Bool("Trade_Sort_By_Name");
	for (int i = 0; i < SlotData->Unknown_6A7990(); i++) {
		int slot = SlotData->Unknown_6A7A00(i);
		MoneyClass price = peek->Unknown_689DA0(slot);
		std::string name;
		name = "";
		InvDataPeekItemStruct *item = peek->Get_Item(slot, true);
		if (item) name = item->Name;
		std::vector<int>::iterator it;
		for (it = Unknown17C.begin(); it != Unknown17C.end(); it++) {
			if (!by_name) {
				MoneyClass other = peek->Unknown_689DA0(*it);
				if (operator<(price, other)) break;
			} else {
				std::string other("");
				InvDataPeekItemStruct *other_item = peek->Get_Item(*it, true);
				if (other_item) other = other_item->Name;
				if (name.compare(other) < 0) break;
			}
		}
		Unknown17C.insert(it, 1, slot);
	}
	Unknown_B0();
}
