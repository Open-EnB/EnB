// Earth & Beyond client (reconstruction): app\Client\ui_talkt.cpp (the client's __FILE__ 0xB8BC9C). Notes at the end.
#include <stdio.h>
#include <string>
#include "sclient.h"
#include "rendobj.h"
#include "layer.h"
#include "aabox.h"
#include "nstrdup.h"
#include "..\..\wwlib\ref_ptr.h"
#include "..\..\wwaudio\AudibleSound.h"
#include "client.h"
#include "gadget.h"
#include "unknown_675120.h"
#include "gd_uiback.h"		// UIBackGadget (0x674260)
#include "fontmanager.h"
#include "fontdatabase.h"
#include "dynamic_window_class.h"
#include "unknown_51c800.h"
#include "local_interface_sort.h"
#include "..\audio\soundlib.h"
#include "..\packets\pktalktreeaction.h"
#include "..\shared\unknown_font_split_string.h"
#include "..\shared\g_assert.h"

// The client at InterfaceClass +0x40, as ui_chat_panel.cpp declares it
class LineDisplayClientClass  // NAME 1: neutral (D10)
{
public:
	virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C();
	virtual void Unknown_10(); virtual void Unknown_14();
	virtual SClient *As_Space_Client();					// +0x18 (SClient's returns itself)  // NAME 3: ui_manu.cpp's FAIL_IF's
};

// ui_trade.cpp's window (0x190 bytes), as far as this file makes it
class UITradeClass : public Unknown5BB320Class  // NAME 3: class-from-file (D24) for ui_trade.cpp
{
public:
	UITradeClass(SClient *client);						// 0x650560
	char UnknownAC[0x190 - 0xAC];						// +0xAC
};

extern FontManagerClass *Unknown_BE67C4;				// NAME 1: neutral (D10)
#include "..\shared\unknown_4a1590.h"
void Unknown_42EB20();									// 0x42EB20  // NAME 1: neutral (D10)

struct UITalkTreeKeyEventStruct  // NAME 1: neutral (D10): as ui_avatr.cpp's
{
	UITalkTreeKeyEventStruct(short key) : Key(key), Unknown02(0), Unknown04(0), Unknown08(0), Unknown0C(0) {}	// NAME 1 [key]: neutral
	short Key;
	short Unknown02;
	float Unknown04;
	float Unknown08;
	int Unknown0C;
};
struct ControlKeyComboStruct; class ControlKeyListClass { public: void Add(const ControlKeyComboStruct &combo, void *handler); };

enum { NPC_LINE_COUNT = 6, ANSWER_COUNT = 5, BTTN_EXIT = 6, BTTN_MORE = 7, BTTN_SKILL = 8, BTTN_TRADE = 9, TALK_TREE_QUIT = -26 };	// NAME 3 [BTTN_MORE]: the FAIL_IF's; NAME 1 [the others]: neutral

static const char *Unknown_B8BBC4[] = { "TLI00A", "TLI02A", NULL };	// 0xB8BBC4: the meshes by Mode  // NAME 1: neutral (D10)

inline bool Quit_Talk_Tree(int action) { return(action < 0); }	// NAME 3: the FAIL_IF's
inline void Unknown_Empty(std::string &text) { text.erase(); }	// NAME 1: neutral (D10): a codeless layer
inline void Unknown_Clear1(std::vector<TalkTreeResponseStruct> &responses) { responses.clear(); }	// NAME 1: neutral (D10): a codeless layer
inline void Unknown_Clear(std::vector<TalkTreeResponseStruct> &responses) { Unknown_Clear1(responses); }	// NAME 1: neutral (D10): a codeless layer
inline void Unknown_Append(std::string &text, const char *more) { text += more; }	// NAME 1: neutral (D10): a codeless layer
inline bool Is_Unknown2C_03(GadgetClass *gadget) { return(gadget->Unknown2C_03); }	// NAME 1: neutral (D10)

// NOT EXACT: 0x64B6E0 (the constructor: the +0x174 stores scheduled after the fill's setup)
UITalkTreeClass::UITalkTreeClass(SClient *client, int mode) :
	Unknown5BB320Class(new UIMeshClass(Unknown_B8BBC4[mode], 1), client),
	Mode(mode),
	FontFile(NULL),
	NPCLineWidth(0),
	PlayerLineWidth(0),
	NextLine(0),
	NPCSplit(NULL),
	Unknown150(false),
	Unknown154(0),
	Unknown158(0),
	Unknown164(false),
	Unknown174(-1),
	Unknown178(0),
	Unknown17C(0)
{
	memset(Buttons, 0, sizeof(Buttons));
	memset(Unknown16C, 0, sizeof(Unknown16C));
}

// 0x64B940
UITalkTreeClass::~UITalkTreeClass()
{
	UITalkTreeClass::Unknown_5C();
	Unknown_BE67C4->Remove(InterfaceMesh->Peek_Mesh_Object());
	Unknown_BE67BC->Unload(FontFile);
	delete [] FontFile;
	FontFile = NULL;
	Unknown_Empty(NPCText);
	if (ActiveUI) {
		ActiveUI = NULL;
	}
	if (TradeUI) {
		TradeUI = NULL;
	}
	delete NPCSplit;
	NPCSplit = NULL;
}

// 0x64BCA0
void UITalkTreeClass::Unknown_54()
{
	char name[32];
	sprintf(name, "fa_%s.ini", Unknown_B8BBC4[Mode]);
	FontFile = nstrdup(name);
	Unknown_BE67BC->Load(FontFile);
	Unknown_BE67C4->Add(InterfaceMesh->Peek_Mesh_Object());
	for (int lp = 0; lp < NPC_LINE_COUNT; lp++) {
		sprintf(name, "Talk%02d_%s", lp, Unknown_B8BBC4[Mode]);
		NPCLines[lp] = Create_Peek(Unknown_BE67C4->Get_Style(name, InterfaceMesh->Peek_Mesh_Object()));




















































		G_ASSERT(NPCLines[lp]);
	}
	NPCSplit = new FontSplitStringClass(NPCLines[0]->Font);
	for (int i = 0; i < ANSWER_COUNT; i++) {
		sprintf(name, "ANSWER%02d", i);
		Buttons[i + 1] = new ButtonGadgetClass(InterfaceMesh, name, 0x55, 4);
		Unknown64.Unknown_6870E0(Buttons[i + 1], i + 1, View, 1, 0);
	}
	ButtonGadgetClass *button = new ButtonGadgetClass(InterfaceMesh, "MORE", 0x55, 4);
	Buttons[BTTN_MORE] = button;
	sprintf(name, "More_%s", Unknown_B8BBC4[Mode]);
	button->Unknown_98(Unknown_BE67C4->Get_Style(name, InterfaceMesh->Peek_Mesh_Object()));
	Unknown64.Unknown_6870E0(button, BTTN_MORE, View, 1, 0);
	GadgetClass::Set_Tooltip(Buttons[BTTN_MORE], "UI_TALKTREE", "MORE");
	button = new ButtonGadgetClass(InterfaceMesh, "SKILL", 0x55, 4);
	Buttons[BTTN_SKILL] = button;
	sprintf(name, "Skill_%s", Unknown_B8BBC4[Mode]);
	button->Unknown_98(Unknown_BE67C4->Get_Style(name, InterfaceMesh->Peek_Mesh_Object()));
	Unknown64.Unknown_6870E0(button, BTTN_SKILL, View, 1, 0);
	GadgetClass::Set_Tooltip(Buttons[BTTN_SKILL], "UI_TALKTREE", "SKILLS");
	button = new ButtonGadgetClass(InterfaceMesh, "TRADE", 0x55, 4);
	Buttons[BTTN_TRADE] = button;
	sprintf(name, "Trade_%s", Unknown_B8BBC4[Mode]);
	button->Unknown_98(Unknown_BE67C4->Get_Style(name, InterfaceMesh->Peek_Mesh_Object()));
	Unknown64.Unknown_6870E0(button, BTTN_TRADE, View, 1, 0);
	GadgetClass::Set_Tooltip(Buttons[BTTN_TRADE], "UI_TALKTREE", "TRADE");
	Buttons[BTTN_EXIT] = new ButtonGadgetClass(InterfaceMesh, "EXIT", 0x55, 3);
	Unknown64.Unknown_6870E0(Buttons[BTTN_EXIT], BTTN_EXIT, View, 1, 0);
	GadgetClass::Set_Tooltip(Buttons[BTTN_EXIT], "UI_TALKTREE", "EXIT");
	RefCountPtr<RenderObjClass> frame = Create_Get(InterfaceMesh->Get_Sub_Object_By_Name("frame01"));
	Unknown64.Unknown_6870E0(new UIBackGadget(frame), 12, View, 1, 0);
	((ControlKeyListClass *)&Unknown4C)->Add(*(const ControlKeyComboStruct *)&UITalkTreeKeyEventStruct(0x1B), (void *)11);
	RenderObjClass *npc_line = InterfaceMesh->Get_Sub_Object_By_Name("TALK00");
	if (npc_line) {
		float width = npc_line->Get_Bounding_Box().Extent.X * 2;
		NPCLineWidth = (int)(width * 0.95f);
		npc_line->Release_Ref();
	} else {

















		G_ASSERT(npc_line);
		NPCLineWidth = 400;
	}
	RenderObjClass *player_line = InterfaceMesh->Get_Sub_Object_By_Name("ANSWER00");
	if (player_line) {
		float width = player_line->Get_Bounding_Box().Extent.X * 2;
		PlayerLineWidth = (int)(width * 0.95f);
		player_line->Release_Ref();
	} else {



		G_ASSERT(player_line);
		PlayerLineWidth = NPCLineWidth;
	}
	sprintf(name, "Reply_%s", Unknown_B8BBC4[Mode]);
	Reply = Create_Peek(Unknown_BE67C4->Get_Style(name, InterfaceMesh->Peek_Mesh_Object()));
	sprintf(name, "NAME_%s", Unknown_B8BBC4[Mode]);
	NPCName = Create_Peek(Unknown_BE67C4->Get_Style(name, InterfaceMesh->Peek_Mesh_Object()));
	Unknown_64C7B0(std::string(" "));
	Unknown168 = Create_NEW(new DynamicWindowClass("TALK_TREE_WINDOW", 10));
	Unknown168->Set_Level(3);
	Unknown_10();
	Unknown_64D230(2);
	Unknown_64D230(4);
}

// 0x64C7B0
void UITalkTreeClass::Unknown_64C7B0(const std::string &name)
{











	FAIL_IF(!NPCName) return;
	NPCName->Change_Text(name.c_str(), 0);
}

// 0x64C800
void UITalkTreeClass::Unknown_58()
{
	if (!Active) {
		InterfaceSortScopeClass sort(this);
		if (ActiveUI) {
			Unknown_64DF80(false);
		} else {
			SoundLibraryClass::Play_2D_Sound("message_1.wav", 2);
			Unknown_64EDC0(true);
		}
		Unknown158 = 0;
		Unknown5BB320Class::Unknown_58();
	}
}

// NOT EXACT: 0x64C8D0 (Unknown_5C: inline budget, _Eos / _Tidy out of line in the client)
void UITalkTreeClass::Unknown_5C()
{
	if (ActiveUI) {
		if (Active) {

































			G_ASSERT(!ActiveUI->Is_Active());
		} else {
			ActiveUI->Unknown_5C();
			ActiveUI = NULL;
		}
	} else {
		InterfaceMesh->Unknown_5B9830();
		InterfaceMesh->Unknown_5B9930(-1);
		Unknown_64D230(2);
		Unknown_64D230(4);
		Unknown_64C7B0(std::string(" "));
		SClient *sclient = Client->As_Space_Client();
		sclient->Send_Packet(&SelectTalkTreePacketClass(sclient->Unknown1138, TALK_TREE_QUIT));
	}
	Unknown_64E640();
	Unknown_64D100();
	if (Active) {
		Unknown5BB320Class::Unknown_5C();
	}
}

// 0x64CC40
void UITalkTreeClass::Unknown_7C(UIViewClass *view, bool arg)
{
	if (View != view) {
		if (TradeUI) {
			TradeUI->Unknown_7C(view, false);
		}
		Unknown5BB320Class::Unknown_7C(view, arg);
	}
}

// NOT EXACT: 0x64CC90 (Unknown_90: the vector's _Destroy out of line in the client)
void UITalkTreeClass::Unknown_90(TalkTreePacketClass *packet)
{



































	FAIL_IF(!packet) return;
	if (!Active && !ActiveUI) {
		Unknown_58();
	}
	Responses.clear();
	packet->Swap_Responses(Responses);
	NPCSplit->Split(packet->NPCSay.c_str(), (float)NPCLineWidth, 0);
	if (NPCSpeechHandle) { NPCSpeechHandle->Stop(true); NPCSpeechHandle = NULL; } SpeechFile.erase();	// Unknown_64E640, written inline
	SpeechFile = packet->NPCSoundFileName;
	Unknown_64DB20(true);
}

// 0x64CF90
const char *UITalkTreeClass::Unknown_64CF90()
{
	if (!NPCText.empty()) {
		return(NPCText.c_str());
	}
	return(NULL);
}

// 0x64CFC0
const char *UITalkTreeClass::Unknown_64CFC0()
{
	if (!PlayerText.empty()) {
		return(PlayerText.c_str());
	}
	return(NULL);
}

// 0x64CFF0
void UITalkTreeClass::Unknown_64CFF0()
{
	NPCText.erase();
}

// 0x64D100
void UITalkTreeClass::Unknown_64D100()
{
	Unknown_Empty(PlayerText);
	for (int lp = 1; lp <= ANSWER_COUNT; lp++) {
		Buttons[lp]->Unknown_40(false);
	}
	Unknown_64DF80(true);
	if (Unknown174 != -1) {
		Unknown174 = -1;
		Unknown168->Remove_All(true);
		Unknown168->Remove_From_Scene();
	}
}

// 0x64D230
void UITalkTreeClass::Unknown_64D230(int action)
{
	switch (action) {
		case 0:



















			G_ASSERT(false);
			return;
		case 1:
			Buttons[BTTN_SKILL]->Unknown_40(true);
			return;
		case 2:
			Buttons[BTTN_SKILL]->Unknown_40(false);
			return;
		case 3:
			Buttons[BTTN_TRADE]->Unknown_40(true);
			return;
		case 4:
			Buttons[BTTN_TRADE]->Unknown_40(false);
			return;
		case 5:
			Unknown_64E430(!Active);
			return;
		case 6:
		case 7:
			Unknown_64EDC0(false);
			return;
	}








	FAIL_IF(!Quit_Talk_Tree(action)) action = -30;
	Unknown158 = action;
}

// 0x64D360
void UITalkTreeClass::Unknown_64D360(char *text)
{
	int out = 0;
	bool keep = true;
	for (int in = 0; text[in] != 0; in++) {
		if (keep || text[in] != ' ') {
			if (text[in] == '/') {
				keep = false;
			} else if (keep) {
				if (out != in) {
					text[out] = text[in];
				}
				out++;
			}
		} else {
			keep = true;
		}
		if (text[in] == '?' && text[in + 1] == '.') {
			in++;
		}
	}
	text[out] = 0;
}

// 0x64D3E0
bool UITalkTreeClass::Unknown_64D3E0()
{
	return(Quit_Talk_Tree(Unknown158));
}

// NOT EXACT: 0x64D400 (Unknown_68: the second append's _Grow out of line in the client)
bool UITalkTreeClass::Unknown_68(int event_id)
{
	switch (event_id) {
		case 6:
		case 11:
			SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
			Unknown154 = TALK_TREE_QUIT;
			Unknown150 = true;
			return(true);
		case 7:
			if (!Buttons[BTTN_MORE]->Unknown_60()) {
				return(true);
			}
			SoundLibraryClass::Play_2D_Sound("fontclick.wav", 2);
			Buttons[BTTN_MORE]->Unknown_40(false);
			Unknown_64DB20(false);
			return(true);
		case 9:
			SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
			Unknown_64E430(false);
			return(true);
		case 12:
			return(true);
	}
	event_id--;











































	FAIL_IF(event_id < 0) return(true);



	FAIL_IF(event_id >= (int)Responses.size()) return(true);
	Unknown154 = Responses[event_id].Unknown00;
	Unknown_64D100();
	PlayerText += Responses[event_id].Text;
	Unknown_Append(PlayerText, " ");
	SoundLibraryClass::Play_2D_Sound("fontclick.wav", 2);
	Unknown150 = true;
	return(true);
}

// NOT EXACT: 0x64D770 (Unknown_60: this in edi, one slot fewer)
void UITalkTreeClass::Unknown_60()
{
	Unknown5BB320Class::Unknown_60();
	if (Unknown150) {
		int response = Unknown154;
		Unknown150 = false;
		SClient *sclient = Client->As_Space_Client();
		sclient->Send_Packet(&SelectTalkTreePacketClass(sclient->Unknown1138, response));
	}
	if (Unknown158 < 0) {
		Unknown_5C();
	}
	if (!SpeechFile.empty() && !NPCSpeechHandle) {
		NPCSpeechHandle = Create_Get(SoundLibraryClass::Play_Dialog(SpeechFile.c_str(), 1));
		if (!NPCSpeechHandle) {
			SpeechFile.erase();
		}
	}
	Unknown_64E7A0();
}

// 0x64DA40
void UITalkTreeClass::Unknown_64()
{
	if (ActiveUI) {
		if (Unknown158 != 0) {
			ActiveUI->Unknown_5C();
			ActiveUI = NULL;
			Unknown_5C();
		} else if (!ActiveUI->Is_Active()) {
			if (Unknown164) {
				Unknown158 = TALK_TREE_QUIT;
				ActiveUI = NULL;
				Unknown_5C();
			} else {
				Unknown_58();
				ActiveUI = NULL;
			}
		}
	}
}

// 0x64DB20
void UITalkTreeClass::Unknown_64DB20(bool restart)
{
	if (restart) {
		NextLine = 0;










































		FAIL_IF(NPCSpeechHandle) {
			Unknown_64E640();
		}
	}
	NPCText.erase();
	bool shown = Unknown_64DD50();
	bool more = NextLine < (int)NPCSplit->Count();
	if (!Unknown_64DF80(more)) {
		more = true;
		if (!shown) {
			Unknown154 = 0;
			Unknown150 = true;
		}
	}
	Buttons[BTTN_MORE]->Unknown_40(more);
	Reply->Set_Hidden(more);
}

// 0x64DD50
bool UITalkTreeClass::Unknown_64DD50()
{
	bool shown = false;
	for (int lp = 0; lp < NPC_LINE_COUNT; lp++) {
		const char *line = NPCSplit->Get(NextLine++);
		if (line) {
			char buffer[1024];
			Bounded_Strcpy(buffer, sizeof(buffer), line);
			Unknown_64D360(buffer);
			NPCLines[lp]->Change_Text(buffer, 0);
			NPCText += line;
			NPCText += " ";
			shown = true;
		} else {
			NPCLines[lp]->Change_Text(NULL, 0);
		}
	}
	return(shown);
}

bool UITalkTreeClass::Unknown_64DF80(bool clear)
{
	int count = clear ? 0 : Responses.size();
	Unknown_42EB20();
	for (int lp = 0; lp < ANSWER_COUNT; lp++) {
		char name[64];
		sprintf(name, "Answer%02d_%s", lp, Unknown_B8BBC4[Mode]);
		FontSys *font = Unknown_BE67C4->Get_Style(name, InterfaceMesh->Peek_Mesh_Object());






















		FAIL_IF(!font) continue;
		if (lp < count) {
			TalkTreeResponseStruct &entry = Responses[lp];
			FontSplitStringClass split(font->Font, entry.Text.c_str(), (float)PlayerLineWidth, 0);
			std::string text(split.Get(0));
			if (split.Count() > 1) {
				Unknown16C[lp] = true;
				text += "...";
			} else {
				Unknown16C[lp] = false;
			}
			font->Change_Text(text.c_str(), 0);
			font->Set_Hidden(false);
			Buttons[lp + 1]->Unknown_40(true);
		} else {
			font->Set_Hidden(true);
			Buttons[lp + 1]->Unknown_40(false);
			Unknown16C[lp] = false;
		}
	}
	return(count > 0);
}

// 0x64E370
void UITalkTreeClass::Unknown_64E370(const RefCountPtr<InterfaceClass> &ui, bool quit)
{
	if (!Active) {
		Unknown_58();
	}






	G_ASSERT(!ActiveUI);
	ActiveUI = ui;
	Unknown164 = quit;
	Unknown_5C();
	ActiveUI->Unknown_7C(View, true);
}

// 0x64E430
void UITalkTreeClass::Unknown_64E430(bool quit)
{
	if (!TradeUI) {
		TradeUI = Create_NEW(new UITradeClass(Client->As_Space_Client()));
		TradeUI->Unknown_54();
	}
	Unknown_64E370(TradeUI, quit);
}

// 0x64E640
void UITalkTreeClass::Unknown_64E640()
{
	if (NPCSpeechHandle) {
		NPCSpeechHandle->Stop(true);
		NPCSpeechHandle = NULL;
	}
	SpeechFile.erase();
}

// NOT EXACT: 0x64E7A0 (Unknown_64E7A0: one slot fewer)
void UITalkTreeClass::Unknown_64E7A0()
{
	RefCountPtr<RenderObjClass> bttn_obj;
	int active = -1;
	for (int lp = 1; lp <= ANSWER_COUNT; lp++) {
		if (!Buttons[lp]->Unknown_60()) {
			break;
		}
		if (Buttons[lp]->Is_Unknown2C_02() && Is_Unknown2C_03(Buttons[lp])) {
			bttn_obj = Create_Get(Buttons[lp]->Peek_Render_Obj());
			active = lp - 1;
			break;
		}
	}
	if (active == Unknown174) {
		return;
	}
	Unknown174 = active;
	Unknown168->Remove_All(true);
	if (Unknown174 == -1 || !Unknown16C[Unknown174]) {
		Unknown168->Remove_From_Scene();
		return;
	}
















	FAIL_IF(!bttn_obj) return;
	char name[128];
	sprintf(name, "FULL_ANSWER__%s", Unknown_B8BBC4[2]);
	RefCountPtr<FontSys> font = Create_NEW(new FontSys(name, 0, 0));



	FAIL_IF(!font) return;
	FontSplitStringClass split(font->Font, Responses[Unknown174].Text.c_str(), (float)PlayerLineWidth, 0);
	int count = split.Count();
	for (int i = 0; i < count; i++) {
		if (!font) {
			font = Create_NEW(new FontSys(name, 0, 0));




			FAIL_IF(!font) continue;
		}
		font->Change_Text(split.Get(i), 0);
		RefCountPtr<Unknown51C800Class> element = Create_NEW(new Unknown51C800Class(font.Peek(), 0x10));
		Unknown168->Add_Object(element.Peek(), i);
		font = NULL;
	}
	InterfaceSortScopeClass sort(this);
	const AABoxClass &box = bttn_obj->Get_Bounding_Box();
	Vector2 center(box.Center.X, box.Center.Y);
	Unknown168->Set_Center(center);
	Unknown168->Add_To_Scene(Peek_Scene());
}

// 0x64EDC0
void UITalkTreeClass::Unknown_64EDC0(bool more_text)
{





	FAIL_IF(!Buttons[BTTN_MORE]) return;
	FontSys *more = Buttons[BTTN_MORE]->Unknown34;
	FAIL_IF(!more) return;
	if (more_text) {
		more->Change_Text("More", 0);
		GadgetClass::Set_Tooltip(Buttons[BTTN_MORE], "UI_TALKTREE", "MORE");
	} else {
		more->Change_Text("Done", 0);
		GadgetClass::Set_Tooltip(Buttons[BTTN_MORE], "UI_TALKTREE", "DONE");
	}
}

// 0x64EEA0
bool UITalkTreeClass::Unknown_64EEA0()
{
	if (ActiveUI) {
		return(ActiveUI.Peek() == (InterfaceClass *)TradeUI.Peek());
	}
	return(false);
}

// 0x64EED0
void UITalkTreeClass::Unknown_64EED0()
{
	if (ActiveUI) {
		ActiveUI->Unknown_5C();
		ActiveUI = NULL;
		Unknown_5C();
	} else if (Active) {
		Unknown_5C();
	}
}

/*
** Reconstruction notes. Placement: the asserts name this file (__FILE__ 0xB8BC9C, lines 171-920). The object runs from
** 0x64B6E0 to the response vector's destructor 0x64EF40 and its COMDATs; ui_talkt_space.cpp follows at 0x64F310.
** Line 267's FAIL_IF '!NPCName' and line 812's G_ASSERT '!ActiveUI' occur twice: 0x64C7B0 and 0x64E370 inlined in their callers.
** Member names come from the asserts where they have one ('Buttons[BTTN_MORE]', 'NPCLines[lp]', 'NPCSpeechHandle', 'ActiveUI',
** 'Responses'); UITradeClass is ui_trade.cpp's (0x650560), declared here as far as this file makes it.
*/
