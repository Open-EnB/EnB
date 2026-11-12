// 0x65DE90  // NOT EXACT: functional (D64)
bool ChatGadgetClass::Unknown_65DE90(KernelEventClass *event)
{
	SClient *sclient = (SClient *)Client->As_Space_Client();
	FAIL_IF(!sclient) return(false);
	int key = ControlKeysManager::Get_Key_Id(sclient, ModuleName, *(ControlKeyValueStruct *)event);
	if (key == -1) {
		return(false);
	}
	ChatPanelInterface *chatpanel = sclient->ChatPanel.Peek();
	switch (key) {
		case 0:
			if (Active) break;
			Edit->Clear();
			if (chatpanel == NULL) break;
			if (!chatpanel->Unknown_564F50()) break;
			goto activate;
		case 1:
			if (chatpanel == NULL) break;
			if (!chatpanel->Unknown_5646A0(4)) break;
		activate:
			if (!Active) {
				SoundLibraryClass::Play_2D_Sound("message_1.wav", 2);
				Unknown_74(true);
			}
			Changed = true;
			return(true);
		case 2:
			if (chatpanel == NULL) break;
			if (!chatpanel->Unknown_5646A0(3)) break;
			goto activate;
		case 3:
			if (chatpanel == NULL) break;
			if (!chatpanel->Unknown_5646A0(2)) break;
			goto activate;
		case 4:
			if (chatpanel != NULL) {
				chatpanel->Unknown_5646A0(1);
			}
			goto activate;
		case 6:
			if (Active) break;
			Edit->Insert("/");
			Unknown3B = true;
			goto activate;
		case 7:
			if (Unknown_65E840()) {
				goto activate;
			}
		case 5:
			if (chatpanel != NULL) {
				chatpanel->Unknown_5646A0(0);
			}
			goto activate;
		case 8:
			if (chatpanel == NULL) break;
			if (!chatpanel->Unknown_5646A0(9)) break;
			goto activate;
		case 9:
			if (chatpanel != NULL) {
				chatpanel->Unknown_567540(1);
			}
			break;
		case 10:
			Unknown_65E5E0();
			if (!Active) Unknown_65CCD0();
			break;
		case 11:
			{
				ClientObjectClass *avatar = Client->Get_Avatar();
				if (avatar != NULL) {
					Matrix3D transform = avatar->Unknown_28(true); Vector3 position = transform.Get_Translation();
					char text[0x100];
					sprintf(text, "Position (%1.2f, %1.2f, %1.2f) ", position.X * 0.001f, position.Y * 0.001f, position.Z * 0.001f);
					Edit->Insert(text);
					Changed = true;
				}
			}
			if (!Active) Unknown_65CCD0();
			break;
		default:
			if (key > 0x14 && key < 0x1E) {
				if (Unknown_65E3E0(key - 0x14)) {
					if (!Active) {
						Unknown_65CCD0();
					} else {
						Changed = true;
					}
				}
				break;
			}
			if (key < 0x0C || key > 0x14) {
				return(false);
			}
			if (Active) {
				Unknown_65E250(key - 0x0B);
				break;
			}
			Unknown_65E3E0(key - 0x0B);
			Unknown3B = true;
			goto activate;
	}
	return(true);
}
