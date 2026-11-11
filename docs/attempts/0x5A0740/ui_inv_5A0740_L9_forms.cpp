// 0x5A0740, builder L9 2026-11-11: forms that did not beat the tree's (0.68). Each is the function's body as given to .work/mk.py.

// ---- form i1: a result variable set to !Is_Busy() in the drop part, one return(handled): 0.50, seven mov al,1
bool UIInvClass::Unknown_5A0740(SlotDataGadgetClass *slot_gadget)
{
	SClient *sclient = Client->As_Space_Client();
	FAIL_IF(!sclient) return(true);
	int from_slot = SlotDrag->Get_Slot();





	FAIL_IF(from_slot == InvDataClass::NO_SLOT) return(true);
	UIInvGadgetClass *gadget = (UIInvGadgetClass *)slot_gadget; bool handled = true;
	if (!SlotDrag->Unknown_08()) {
		if (gadget->As_SlotDataShortcutGadget()) {
			if (sclient->Unknown_750B60()) {
				sclient->Unknown_74D990(0x11, "Cannot perform action while incapacitated.", 0);
				return(true);
			}
			sclient->Send_Packet(&EquipUsePacketClass(sclient->ShipID, InvType, from_slot));
			return(true);
		} else if (SlotDrag->Unknown1C == 1 || SlotDrag->Unknown1C == 3) {
			if (SlotDrag->Unknown20) {
				if (sclient->CurrentView == sclient->Unknown135C) {
					int to_inv;
					const char *to_name;
					switch (InvType) {
						case 3:
							to_inv = 1;
							to_name = "cargo";
							break;
						case 1:
							to_inv = 3;
							to_name = "vault";
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
				InvDataPeekClass *peek = SlotData->Unknown_10()->Unknown_688830();
				InvDataPeekItemStruct *item = peek->Get_Item(from_slot, true);
				switch (InvType) {
					case INV_VENDOR:
						switch (SlotDrag->Unknown1C) {
							case 3:
								Unknown_5A02E0(true);
								sclient->Print(0, "Buying stack of %s.", item->Name.c_str());
								break;
							case 1:
								Unknown_5A02E0(false);
								sclient->Print(0, "Buying %s.", item->Name.c_str());
								break;
						}
						break;
					case 1:
						if (sclient->Unknown1298 && sclient->Unknown1298->Unknown_64EEA0()) {
							switch (SlotDrag->Unknown1C) {
								case 3: {
									InvDataPeekClass *peek = SlotData->Unknown_10()->Unknown_688830();
									int num = peek->Get_Stack_Count(from_slot);
									if (num) {
										InvTransaction->Unknown_690AA0(InvType, from_slot, num, true, 4, 0);
										sclient->Print(0, "Selling stack of %d %s.", num, item->Name.c_str());
									}
									break;
								}
								case 1:
									InvTransaction->Unknown_690AA0(InvType, from_slot, 1, true, 4, 0);
									sclient->Print(0, "Selling %s.", item->Name.c_str());
									break;
							}
						}
						break;
				}
			}
		}
	} else {
		GadgetClass *target = GadgetClass::Unknown_65F5F0((GadgetListManagerClass *)Client->As_Space_Client()->Unknown1278, true, false);
		Unknown66CB20Class *button;
		if (target && (button = (Unknown66CB20Class *)target->Unknown_14()) != NULL) {
			if (ShortcutUI) {
				ShortcutUI->Create_Shortcut(from_slot, button);
				ShortcutUI->Save_Current();
			}
		} else if (!gadget->As_SlotDataShortcutGadget() && target) {
			SlotDataGadgetClass *inv_gadget = (SlotDataGadgetClass *)target->Unknown_10();
			if (inv_gadget && inv_gadget->Unknown_670950() && ((SlotDataClass *)inv_gadget->Unknown_670950())->Unknown_10()) {
				if (!((UIInvSlotGadgetStruct *)inv_gadget)->Unknown68) return(true);
				Unknown_5A0D00(from_slot, inv_gadget);
			} else {
				if (!target->Unknown_1C() && !target->Unknown_18()) return(true);
				Unknown_5A0EB0(from_slot, SlotDrag->Unknown1C);
			}
			handled = !Is_Busy();
		}
	}
	return(handled);
}

// ---- form e1: the drop part first in source (if (SlotDrag->Unknown_08()) ... else ...): the layout follows the source, 0.35
bool UIInvClass::Unknown_5A0740(SlotDataGadgetClass *slot_gadget)
{
	SClient *sclient = Client->As_Space_Client();
	FAIL_IF(!sclient) return(true);
	int from_slot = SlotDrag->Get_Slot();





	FAIL_IF(from_slot == InvDataClass::NO_SLOT) return(true);
	UIInvGadgetClass *gadget = (UIInvGadgetClass *)slot_gadget;
	if (SlotDrag->Unknown_08()) {
		GadgetClass *target = GadgetClass::Unknown_65F5F0((GadgetListManagerClass *)Client->As_Space_Client()->Unknown1278, true, false);
		Unknown66CB20Class *button;
		if (target && (button = (Unknown66CB20Class *)target->Unknown_14()) != NULL) {
			if (ShortcutUI) {
				ShortcutUI->Create_Shortcut(from_slot, button);
				ShortcutUI->Save_Current();
			}
		} else if (!gadget->As_SlotDataShortcutGadget() && target) {
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
	} else {
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
						case 3:
							to_inv = 1;
							to_name = "cargo";
							break;
						case 1:
							to_inv = 3;
							to_name = "vault";
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
				InvDataPeekClass *peek = SlotData->Unknown_10()->Unknown_688830();
				InvDataPeekItemStruct *item = peek->Get_Item(from_slot, true);
				switch (InvType) {
					case INV_VENDOR:
						switch (SlotDrag->Unknown1C) {
							case 3:
								Unknown_5A02E0(true);
								sclient->Print(0, "Buying stack of %s.", item->Name.c_str());
								break;
							case 1:
								Unknown_5A02E0(false);
								sclient->Print(0, "Buying %s.", item->Name.c_str());
								break;
						}
						break;
					case 1:
						if (sclient->Unknown1298 && sclient->Unknown1298->Unknown_64EEA0()) {
							switch (SlotDrag->Unknown1C) {
								case 3: {
									InvDataPeekClass *peek = SlotData->Unknown_10()->Unknown_688830();
									int num = peek->Get_Stack_Count(from_slot);
									if (num) {
										InvTransaction->Unknown_690AA0(InvType, from_slot, num, true, 4, 0);
										sclient->Print(0, "Selling stack of %d %s.", num, item->Name.c_str());
									}
									break;
								}
								case 1:
									InvTransaction->Unknown_690AA0(InvType, from_slot, 1, true, 4, 0);
									sclient->Print(0, "Selling %s.", item->Name.c_str());
									break;
							}
						}
						break;
				}
			}
		}
	}
	return(true);
}

// ---- form l1: an inline forwarder around Unknown_5A02E0: still inlined
bool UIInvClass::Unknown_5A0740(SlotDataGadgetClass *slot_gadget)
{
	SClient *sclient = Client->As_Space_Client();
	FAIL_IF(!sclient) return(true);
	int from_slot = SlotDrag->Get_Slot();
//PRE static inline bool UIInv_Buy(UIInvClass *inv, bool all) { return(inv->Unknown_5A02E0(all)); }




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
						case 3:
							to_inv = 1;
							to_name = "cargo";
							break;
						case 1:
							to_inv = 3;
							to_name = "vault";
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
				InvDataPeekClass *peek = SlotData->Unknown_10()->Unknown_688830();
				InvDataPeekItemStruct *item = peek->Get_Item(from_slot, true);
				switch (InvType) {
					case INV_VENDOR:
						switch (SlotDrag->Unknown1C) {
							case 3:
								UIInv_Buy(this, true);
								sclient->Print(0, "Buying stack of %s.", item->Name.c_str());
								break;
							case 1:
								UIInv_Buy(this, false);
								sclient->Print(0, "Buying %s.", item->Name.c_str());
								break;
						}
						break;
					case 1:
						if (sclient->Unknown1298 && sclient->Unknown1298->Unknown_64EEA0()) {
							switch (SlotDrag->Unknown1C) {
								case 3: {
									InvDataPeekClass *peek = SlotData->Unknown_10()->Unknown_688830();
									int num = peek->Get_Stack_Count(from_slot);
									if (num) {
										InvTransaction->Unknown_690AA0(InvType, from_slot, num, true, 4, 0);
										sclient->Print(0, "Selling stack of %d %s.", num, item->Name.c_str());
									}
									break;
								}
								case 1:
									InvTransaction->Unknown_690AA0(InvType, from_slot, 1, true, 4, 0);
									sclient->Print(0, "Selling %s.", item->Name.c_str());
									break;
							}
						}
						break;
				}
			}
		}
	} else {
		GadgetClass *target = GadgetClass::Unknown_65F5F0((GadgetListManagerClass *)Client->As_Space_Client()->Unknown1278, true, false);
		Unknown66CB20Class *button;
		if (target && (button = (Unknown66CB20Class *)target->Unknown_14()) != NULL) {
			if (ShortcutUI) {
				ShortcutUI->Create_Shortcut(from_slot, button);
				ShortcutUI->Save_Current();
			}
		} else if (!gadget->As_SlotDataShortcutGadget() && target) {
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
	}
	return(true);
}

// ---- form n3a: every early return the literal true and return(true) last (no result variable): 0.68, 1174 bytes; the return block lands after the first FAIL_IF
bool UIInvClass::Unknown_5A0740(SlotDataGadgetClass *slot_gadget)
{
	SClient *sclient = Client->As_Space_Client();
	FAIL_IF(!sclient) return(true);
	int from_slot = SlotDrag->Get_Slot();





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
						case 3:
							to_inv = 1;
							to_name = "cargo";
							break;
						case 1:
							to_inv = 3;
							to_name = "vault";
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
				InvDataPeekClass *peek = SlotData->Unknown_10()->Unknown_688830();
				InvDataPeekItemStruct *item = peek->Get_Item(from_slot, true);
				switch (InvType) {
					case INV_VENDOR:
						switch (SlotDrag->Unknown1C) {
							case 3:
								Unknown_5A02E0(true);
								sclient->Print(0, "Buying stack of %s.", item->Name.c_str());
								break;
							case 1:
								Unknown_5A02E0(false);
								sclient->Print(0, "Buying %s.", item->Name.c_str());
								break;
						}
						break;
					case 1:
						if (sclient->Unknown1298 && sclient->Unknown1298->Unknown_64EEA0()) {
							switch (SlotDrag->Unknown1C) {
								case 3: {
									InvDataPeekClass *peek = SlotData->Unknown_10()->Unknown_688830();
									int num = peek->Get_Stack_Count(from_slot);
									if (num) {
										InvTransaction->Unknown_690AA0(InvType, from_slot, num, true, 4, 0);
										sclient->Print(0, "Selling stack of %d %s.", num, item->Name.c_str());
									}
									break;
								}
								case 1:
									InvTransaction->Unknown_690AA0(InvType, from_slot, 1, true, 4, 0);
									sclient->Print(0, "Selling %s.", item->Name.c_str());
									break;
							}
						}
						break;
				}
			}
		}
	} else {
		GadgetClass *target = GadgetClass::Unknown_65F5F0((GadgetListManagerClass *)Client->As_Space_Client()->Unknown1278, true, false);
		Unknown66CB20Class *button;
		if (target && (button = (Unknown66CB20Class *)target->Unknown_14()) != NULL) {
			if (ShortcutUI) {
				ShortcutUI->Create_Shortcut(from_slot, button);
				ShortcutUI->Save_Current();
			}
		} else if (!gadget->As_SlotDataShortcutGadget() && target) {
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
	}
	return(true);
}

// ---- form j2: the result variable returned by the FAIL_IFs and the shortcut part, the literal from the transfer on: the same bytes as the tree's (0.74)
bool UIInvClass::Unknown_5A0740(SlotDataGadgetClass *slot_gadget)
{
	bool handled = true; SClient *sclient = Client->As_Space_Client();
	FAIL_IF(!sclient) return(handled);
	int from_slot = SlotDrag->Get_Slot();





	FAIL_IF(from_slot == InvDataClass::NO_SLOT) return(handled);
	UIInvGadgetClass *gadget = (UIInvGadgetClass *)slot_gadget;
	if (!SlotDrag->Unknown_08()) {
		if (gadget->As_SlotDataShortcutGadget()) {
			if (sclient->Unknown_750B60()) {
				sclient->Unknown_74D990(0x11, "Cannot perform action while incapacitated.", 0);
				return(handled);
			}
			sclient->Send_Packet(&EquipUsePacketClass(sclient->ShipID, InvType, from_slot));
			return(handled);
		} else if (SlotDrag->Unknown1C == 1 || SlotDrag->Unknown1C == 3) {
			if (SlotDrag->Unknown20) {
				if (sclient->CurrentView == sclient->Unknown135C) {
					int to_inv;
					const char *to_name;
					switch (InvType) {
						case 3:
							to_inv = 1;
							to_name = "cargo";
							break;
						case 1:
							to_inv = 3;
							to_name = "vault";
							break;
						default:
							return(handled);
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
								return(true);
							}
							sclient->Print(0, "Transferring stack of %d %s to %s.", num, item->Name.c_str(), to_name);
						}
					}
				}
			} else {
				InvDataPeekClass *peek = SlotData->Unknown_10()->Unknown_688830();
				InvDataPeekItemStruct *item = peek->Get_Item(from_slot, true);
				switch (InvType) {
					case INV_VENDOR:
						switch (SlotDrag->Unknown1C) {
							case 3:
								Unknown_5A02E0(true);
								sclient->Print(0, "Buying stack of %s.", item->Name.c_str());
								break;
							case 1:
								Unknown_5A02E0(false);
								sclient->Print(0, "Buying %s.", item->Name.c_str());
								break;
						}
						break;
					case 1:
						if (sclient->Unknown1298 && sclient->Unknown1298->Unknown_64EEA0()) {
							switch (SlotDrag->Unknown1C) {
								case 3: {
									InvDataPeekClass *peek = SlotData->Unknown_10()->Unknown_688830();
									int num = peek->Get_Stack_Count(from_slot);
									if (num) {
										InvTransaction->Unknown_690AA0(InvType, from_slot, num, true, 4, 0);
										sclient->Print(0, "Selling stack of %d %s.", num, item->Name.c_str());
									}
									break;
								}
								case 1:
									InvTransaction->Unknown_690AA0(InvType, from_slot, 1, true, 4, 0);
									sclient->Print(0, "Selling %s.", item->Name.c_str());
									break;
							}
						}
						break;
				}
			}
		}
	} else {
		GadgetClass *target = GadgetClass::Unknown_65F5F0((GadgetListManagerClass *)Client->As_Space_Client()->Unknown1278, true, false);
		Unknown66CB20Class *button;
		if (target && (button = (Unknown66CB20Class *)target->Unknown_14()) != NULL) {
			if (ShortcutUI) {
				ShortcutUI->Create_Shortcut(from_slot, button);
				ShortcutUI->Save_Current();
			}
		} else if (!gadget->As_SlotDataShortcutGadget() && target) {
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
	}
	return(true);
}
