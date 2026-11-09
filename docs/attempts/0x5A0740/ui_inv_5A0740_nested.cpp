bool UIInvClass::Unknown_5A0740(SlotDataGadgetClass *slot_gadget)
{
	SClient *sclient = Client->As_Space_Client();
#line 744
	FAIL_IF(!sclient) return(true);
	int from_slot = SlotDrag->Get_Slot();
#line 751
	FAIL_IF(from_slot == InvDataClass::NO_SLOT) return(true);
	UIInvGadgetClass *gadget = (UIInvGadgetClass *)slot_gadget;
	if (!SlotDrag->Unknown_08()) {
		if (gadget->As_SlotDataShortcutGadget()) {
			if (sclient->Unknown_750B60()) {
				sclient->Unknown_74D990(0x11, "Cannot perform action while incapacitated.", 0);
			} else {
				sclient->Send_Packet(&EquipUsePacketClass(sclient->ShipID, InvType, from_slot));
			}
		} else if (SlotDrag->Unknown1C == 1 || SlotDrag->Unknown1C == 3) {
			if (SlotDrag->Unknown20) {
				if (sclient->CurrentView == sclient->Unknown135C) {
					int to_inv;
					const char *to_name;
					switch (InvType) {
						case 1:
							to_inv = 3;
							to_name = "vault";
							break;
						case 3:
							to_inv = 1;
							to_name = "cargo";
							break;
						default:
							return(true);
					}
					InvDataPeekClass *peek = SlotData->Unknown_10()->Unknown_688830();
					int num = peek->Get_Stack_Count(from_slot);
					if (num) {
						if (SlotDrag->Unknown1C == 1) num = 1;
						InvTransaction->Unknown_690AA0(InvType, from_slot, num, true, to_inv, -1);
						InvDataPeekItemStruct *item = peek->Get_Item(from_slot, false);
						if (item) {
							if (num == 1) {
								sclient->Print(0, "Transferring %s to %s.", item->Name.c_str(), to_name);
							} else {
								sclient->Print(0, "Transferring stack of %d %s to %s.", num, item->Name.c_str(), to_name);
							}
						}
					}
				}
			} else {
				InvDataPeekItemStruct *item = SlotData->Unknown_10()->Unknown_688830()->Get_Item(from_slot, true);
				switch (InvType) {
					case 1:
						if (sclient->Unknown1298 && sclient->Unknown1298->Unknown_64EEA0()) {
							switch (SlotDrag->Unknown1C) {
								case 1:
									InvTransaction->Unknown_690AA0(InvType, from_slot, 1, true, 4, 0);
									sclient->Print(0, "Selling %s.", item->Name.c_str());
									break;
								case 3: {
									int num = SlotData->Unknown_10()->Unknown_688830()->Get_Stack_Count(from_slot);
									if (num) {
										InvTransaction->Unknown_690AA0(InvType, from_slot, num, true, 4, 0);
										sclient->Print(0, "Selling stack of %d %s.", num, item->Name.c_str());
									}
									break;
								}
							}
						}
						break;
					case INV_VENDOR:
						switch (SlotDrag->Unknown1C) {
							case 1:
								Unknown_5A02E0(false);
								sclient->Print(0, "Buying %s.", item->Name.c_str());
								break;
							case 3:
								Unknown_5A02E0(true);
								sclient->Print(0, "Buying stack of %s.", item->Name.c_str());
								break;
						}
						break;
				}
			}
		}
		return(true);
	}
	GadgetClass *target = GadgetClass::Unknown_65F5F0((GadgetListManagerClass *)Client->As_Space_Client()->Unknown1278, true, false);
	Unknown66CB20Class *button;
	if (target && (button = (Unknown66CB20Class *)target->Unknown_14()) != NULL) {
		if (!ShortcutUI) return(true);
		ShortcutUI->Create_Shortcut(from_slot, button);
		ShortcutUI->Save_Current();
		return(true);
	}
	if (gadget->As_SlotDataShortcutGadget()) return(true);
	if (!target) return(true);
	SlotDataGadgetClass *inv_gadget = (SlotDataGadgetClass *)target->Unknown_10();
	if (inv_gadget && inv_gadget->Unknown_670950() && ((SlotDataClass *)inv_gadget->Unknown_670950())->Unknown_10()) {
		if (!((UIInvSlotGadgetStruct *)inv_gadget)->Unknown68) return(true);
		Unknown_5A0D00(from_slot, inv_gadget);
	} else {
		if (!target->Unknown_1C() && !target->Unknown_18()) return(true);
		Unknown_5A0EB0(from_slot, SlotDrag->Unknown1C);
	}
	return(!Is_Busy());
}

