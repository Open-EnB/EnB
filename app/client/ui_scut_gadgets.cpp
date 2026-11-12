// Earth & Beyond client (reconstruction): app\Client\ui_scut_gadgets.cpp. Notes at the end of the file.
#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
#include <list>
#include "sclient.h"
#include "rendobj.h"
#include "..\..\wwlib\ref_ptr.h"
#include "gadget.h"
#include "gd_scut_proxy.h"
#include "..\audio\soundlib.h"
#include "..\shared\auxdata\permstr.h"	// (its PermStringInitClass static 0xBE5E1C, initialised by 0x613680, atexit 0x6136B0)
#include "..\shared\g_assert.h"
#include "ini.h"
#include "co.h"
#include "ui_scut_gadgets.h"

char *Unknown_8B44E0(__int64 value, char *buffer);	// 0x8B44E0: _i64toa(value, buffer, 10), as pkguildplayeronline.h  // NAME 1: neutral placeholder (D10)

static const char *ShortcutIni = "shortcut.ini";	// 0xB88EA0  // NAME 2: what it holds
// @0x6136D0
UIScutGadgetsClass::UIScutGadgetsClass(const std::string &name, SClient *sclient) : ShortcutBeenCreated(false)
{








































	FAIL_IF(!sclient) return;
	char id[24];
	Unknown_8B44E0(sclient->Ticket.Character_ID, id);
	Section = std::string(id) + "_" + name;
	Init_Name(sclient);
	Load(Saved);
}

// @0x613A20
UIScutGadgetsClass::~UIScutGadgetsClass()
{
}

// @0x613BA0
void UIScutGadgetsClass::Set_UI(const RefCountPtr<UIScutClass> &ui)
{
	if (ShortcutUI) {
		Delete_All();
	}
	ShortcutUI = ui;
	if (ShortcutUI) {
		Init_Name((ClientClass *)ShortcutUI->Client);
	}
}

// @0x613D70
void UIScutGadgetsClass::Clear_UI()
{
	if (ShortcutUI) {
		Init_Name((ClientClass *)ShortcutUI->Client);
		Delete_All();
		ShortcutUI = NULL;
	}
}

// @0x613F50
int UIScutGadgetsClass::Get_State()
{
	if (ShortcutUI && ShortcutUI->Is_Active()) {
		return (Saved.size() != 0) ? STATE_NOT_RESTORED : STATE_RESTORED;
	}
	return STATE_NO_UI;
}

// @0x613F80
bool UIScutGadgetsClass::Restore()
{























	FAIL_IF(Get_State() != STATE_NOT_RESTORED) return(true);
	bool restored = Restore(Saved);
	Saved.clear();
	return(restored);
}

// @0x614040
bool UIScutGadgetsClass::Restore(const std::list<ShortcutDataStruct> &list)
{
	bool restored = false;
	for (std::list<ShortcutDataStruct>::const_iterator it = list.begin(); it != list.end(); ++it) {
		RObjGadgetClass *gadget = Unknown_14((*it).Key.c_str());
		if (gadget) {
			bool added = ShortcutUI->Unknown_610D00(gadget, (*it).Bar, (*it).Group, (*it).Button);
			if (added) {
				ShortcutGadgets.push_back(gadget);
				restored = true;
			} else {







				G_ASSERT(added);
				GadgetClass::Destroy(gadget);
				gadget = NULL;
			}
		} else {

			G_ASSERT(!restored);
		}
	}
	return(restored);
}

// @0x614360
RObjGadgetClass *UIScutGadgetsClass::Create_Shortcut(int slot, UIScutProxyGadgetClass *button)
{












	FAIL_IF(!ShortcutUI) return(NULL);
	RObjGadgetClass *gadget = Unknown_0C(slot);
	if (!gadget) return(NULL);
	if (button) {
		button->Set_Shortcut(gadget);
	} else {
		if (!ShortcutUI->Unknown_610C70(gadget, Unknown_18()) && !ShortcutUI->Unknown_610B00(gadget)) {
			GadgetClass::Destroy(gadget);
			return(NULL);
		}
	}
	if (Get_State() == STATE_RESTORED) {
		SoundLibraryClass::Play_2D_Sound("shortcut_created_sn_1.wav", 2);
	}
	ShortcutGadgets.push_back(gadget);
	ShortcutBeenCreated = true;
	return(gadget);
}

// @0x614660
bool UIScutGadgetsClass::Delete_Shortcut(int scut)
{


























	FAIL_IF((unsigned)scut >= ShortcutGadgets.size()) return(false);
	RObjGadgetClass *scut_gadg = ShortcutGadgets[scut];

	FAIL_IF(!scut_gadg) return(false);
	ShortcutGadgets.erase(ShortcutGadgets.begin() + scut);
	GadgetClass::Destroy(scut_gadg);
	return(true);
}

inline void UIScutGadgetsClass::Delete_Gadgets()
{
	int lp = ShortcutGadgets.size();
	while (lp--) {
		Delete_Shortcut(lp);
	}
}

// @0x614730
void UIScutGadgetsClass::Delete_All()
{
	Build(Saved);
	Save(Saved);
	Delete_Gadgets();
}

// @0x614840
void UIScutGadgetsClass::Clean()
{
	int size = ShortcutGadgets.size();
	for (int lp = 0; lp < size; lp++) {
		RObjGadgetClass *scut_gadg = ShortcutGadgets[lp];






































		FAIL_IF(!scut_gadg) continue;
		if (!scut_gadg->Unknown_88() && Unknown_BE5F24 != scut_gadg && Unknown_BE5F28 != scut_gadg) {
			if (Delete_Shortcut(lp)) {
				lp--;
				size--;
			}
		}
	}
}

// @0x614980
void UIScutGadgetsClass::Load(std::list<ShortcutDataStruct> &list)
{
	INIClass ini(ShortcutIni);
	const char *section = Section.c_str();
	ShortcutBeenCreated = ini.Get_Bool(section, "ShortcutBeenCreated", false);
	int num = ini.Get_Int(section, "Num", 0);
	for (int lp = 0; lp < num; lp++) {
		char entry[100];
		sprintf(entry, "%d_Bar", lp);
		int bar = ini.Get_Int(section, entry, -1);









		FAIL_IF(bar < 0) continue;
		sprintf(entry, "%d_Group", lp);
		int group = ini.Get_Int(section, entry, -1);

		FAIL_IF(group < 0) continue;
		sprintf(entry, "%d_Button", lp);
		int button = ini.Get_Int(section, entry, -1);

		FAIL_IF(button < 0) continue;
		sprintf(entry, "%d_Key", lp);
		char key[64];
		ini.Get_String(section, entry, "", key, 64);
		list.push_back(ShortcutDataStruct(std::string(key), bar, group, button));
	}
}

// @0x614D90
void UIScutGadgetsClass::Save(const std::list<ShortcutDataStruct> &list)
{
	INIClass ini(ShortcutIni);
	const char *section = Section.c_str();
	ini.Clear(section);
	if (Name.length()) {
		ini.Put_String(section, "Name", Name.c_str());
	}
	char entry[100];
	int count = 0;
	for (std::list<ShortcutDataStruct>::const_iterator it = list.begin(); it != list.end(); ++it) {
		sprintf(entry, "%d_Bar", count);
		ini.Put_Int(section, entry, (*it).Bar);
		sprintf(entry, "%d_Group", count);
		ini.Put_Int(section, entry, (*it).Group);
		sprintf(entry, "%d_Button", count);
		ini.Put_Int(section, entry, (*it).Button);
		sprintf(entry, "%d_Key", count);
		ini.Put_String(section, entry, (*it).Key.c_str());
		count++;
	}
	ini.Put_Int(section, "Num", count);
	ini.Put_Bool(section, "ShortcutBeenCreated", ShortcutBeenCreated);
	ini.Save(ShortcutIni);
}

// @0x614FB0
void UIScutGadgetsClass::Build(std::list<ShortcutDataStruct> &list)
{
	Clean();
	list.clear();
	int button = 0;
	int group = 0;
	int bar = 0;
	for (std::vector<RObjGadgetClass *>::iterator it = ShortcutGadgets.begin(); it != ShortcutGadgets.end(); ++it) {
		RObjGadgetClass *gadget = *it;





















		FAIL_IF(!gadget) continue;
		bool found = ShortcutUI->Unknown_610E10(gadget, &bar, &group, &button);

		FAIL_IF(!found) continue;
		std::string key;
		if (Unknown_10(gadget, key)) {
			list.push_back(ShortcutDataStruct(key, bar, group, button));
		}
	}
}

// @0x6152C0
void UIScutGadgetsClass::Save_Current()
{
	std::list<ShortcutDataStruct> list;
	Build(list);
	Save(list);
}

// @0x6153B0
void UIScutGadgetsClass::Init_Name(ClientClass *client)
{
	if (Name.length() == 0) {



		FAIL_IF(!client) return;
		ClientObjectClass *avatar = client->Get_Avatar();
		if (avatar) {
			avatar->Unknown_6FF7A0(Name);
		} else {
			Name = "";
		}
	}
}


/*
**	Reconstruction notes. The client's __FILE__ 0xB88EBC names the file; the asserts' lines (65, 135, 160, 166, 187, 235, 238, 304,
**	334, 338, 342, 408, 411, 434) and the object's edges place it: it opens with the PermStringInitClass static's initialiser 0x613680
**	(after ui_scut.cpp's adjustor thunks 0x613660 / 0x613670) and its .data with the 'shortcut.ini' pointer 0xB88EA0; its template
**	instances run to 0x6159F0, called only from here (0x614040, 0x614360, 0x613A20, 0x614980, 0x614FB0), ui_shard.cpp following.
**	Client 0x6136D0-0x6153B0, plus the instances: RefCountPtr's destructor 0x6154F0, the vector's 0x615530 / size 0x615570 / _Destroy
**	0x615620, the list's destructor 0x6155A0, erase range 0x615640, _Buynode 0x6156C0, erase 0x615700, iterator++ 0x615750, the
**	vector's _Ucopy 0x615770, _Ufill 0x6157B0, std::copy 0x6157F0, std::_Construct 0x615830 (ShortcutDataStruct) and 0x6159F0 (a pointer).
**	Delete_Gadgets is an inline layer with no code of its own: without it 0x613BA0 inlines Delete_Shortcut where the client calls it, and
**	0x613D70 inlines std::copy; a layer around Build / Save instead (Store) gives the same 16 exact, so which one the source had is open.
*/
