// Earth & Beyond client (reconstruction): app\Client\ui_chat_window.cpp. Notes at the end of the file.
#define __INTRFACE_MEMBERS
#include <stdio.h>
#include "ui_chat_window.h"
#include "client.h"
#include "rendobj.h"
#include "fontmanager.h"
#include "fontdatabase.h"
#include "fontsys_cursor.h"
#include "textgadg.h"
#include "gd_slider.h"
#include "unknown_675120.h"
#include "chat_window.h"
#include "unknown_67d430.h"
#include "local_interface_sort.h"
#include "control_keys_manager.h"
#include "unknown_signalbase.h"
#include "..\shared\g_assert.h"

// chatgadg.cpp's text entry (0x80 bytes, constructor 0x65BFD0, table 0xAF99D8), as far as this file calls it
class Unknown65BFD0Class : public TextBaseGadgetClass	// NAME 1: neutral (D10)
{
public:
	void Unknown_65E7C0(const char *text);				// 0x65E7C0 (slot +0xAC's)  // NAME 1 [text]: neutral
	void Unknown_65EBF0(char *buffer, int size, int &cursorpos, bool &insert);	// 0x65EBF0: the text, the cursor position and the insert mode  // NAME 1 [buffer, size, cursorpos, insert]: what 0x571470 does with them
};

// The interface event dispatcher (0x540A20 returns its function-local static at 0xBE5AF0), as ui_chat_panel.cpp declares it
class UnknownEventDispatcherClass;
UnknownEventDispatcherClass *Unknown_540A20();			// 0x540A20 (intrface.cpp)  // NAME 1: neutral (D10)

// What InterfaceClass::Client points to, as far as this file calls it (as ui_chat_panel.cpp declares it)
class LineDisplayClientClass  // NAME 1: neutral (D10)
{
public:
	virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C();
	virtual void Unknown_10(); virtual void Unknown_14();
	virtual SClient *As_Space_Client();					// +0x18 (ClientClass::Unknown_18; SClient's returns itself)  // NAME 3: ui_manu.cpp's FAIL_IF's
};

#define InterfaceMesh Unknown84		// the asserts' name for +0x84 (the header's Unknown84 serves the files that read it)

// (inline in 0x56FD30)
inline ChatWinStruct::ChatWinStruct(GadgetSlider *slider, unsigned int index) :
	ChatWindow(NULL)
{
	ChatWindow = new ChatWindowClass(slider, 100, (index < 1) ? index : 1);
}

// (inline in 0x56F560)
inline ChatWinStruct::~ChatWinStruct()
{
	delete ChatWindow;
	ChatWindow = NULL;
}

// 0x56F420
ChatWindowInterface::ChatWindowInterface(SClient *client, Unknown67D430Class *history, Unknown65BFD0Class *edit) :
	UIChatPanelBaseClass(client),
	ChatHistory(history),
	UnknownA4(edit),
	UnknownDC(NULL),
	UnknownE4(false)
{
	Unknown_BE67BC->Load("fc_chat.ini");
}

// 0x56F560  // NOT EXACT: functional (CW2): the client calls the vector's empty _Destroy 0x571DE0 out of line, ours inlines it; one register later (0.58)
ChatWindowInterface::~ChatWindowInterface()
{
	Unknown64.Unknown_687150(UnknownA4);
	UnknownA4 = NULL;
	if (UnknownDC) {
		GadgetClass::Destroy(UnknownDC);
		UnknownDC = NULL;
	}
	for (std::vector<ChatWinStruct *>::iterator it = ChatWindows.begin(); it != ChatWindows.end(); it++) {
		delete *it;
	}
	ChatWindows.erase(ChatWindows.begin(), ChatWindows.end());
	Unknown_BE67BC->Unload("fc_chat.ini");
}

































// 0x56F780
void ChatWindowInterface::Unknown_54()
{
	FAIL_IF(!Interface_Mesh()) return;
	UnknownB8 = Create_NEW(new FontSys("Chat_Line", 0, NULL));
	UnknownB8->Set_Sort_Level(10);
	UnknownD8 = Unknown_685680(this, Create_Peek((RenderObjClass *)Unknown44));
	UnknownD8->Set_Sort_Level(9);
	Unknown64.Unknown_6870E0(UnknownA4, 7, View, true, true);
	Unknown_10();
	Unknown_5715F0(false, true);
}




















// 0x56F9F0  // NOT EXACT: functional (CW2): ours keeps 0 in ebx for the FontSys arguments and the null tests, the client pushes constants (0.55)
bool ChatWindowInterface::Unknown_98(const char *name, const char *ini)
{
	if (!UIChatPanelBaseClass::Unknown_98(name, ini)) return(false);
	DestBttnText = Create_NEW(new FontSys("Chat_Dest_Bttn", 0, NULL));
	Viewport.TLBone = InterfaceMesh->Get_Bone_Index("chat_vp_tl");
	Viewport.BRBone = InterfaceMesh->Get_Bone_Index("chat_vp_br");
	UnknownD4 = Create_Get(InterfaceMesh->Get_Sub_Object_By_Name("TEXTBOX"));
	RefCountPtr<RenderObjClass> dest_box = Create_Get(InterfaceMesh->Get_Sub_Object_By_Name("DEST_BOX"));
	if (dest_box) {
		UnknownDC = new ButtonGadgetClass(dest_box.Peek(), 0x55, 3);
		Unknown64.Unknown_6870E0(UnknownDC, 1, View, true, true);
		InterfaceMesh->Add_Sub_Object_To_Bone(DestBttnText.Peek(), InterfaceMesh->Get_Sub_Object_Bone_Index(dest_box.Peek()));
		UnknownDC->Unknown_98(DestBttnText.Peek());
	} else {
		G_ASSERT(false);
	}
	return(true);
}



// 0x56FD30  // NOT EXACT: functional (CW2): frame 0x54 vs the client's 0x50 and reserve's _Destroy inlined (0.56)
void ChatWindowInterface::Unknown_A0(unsigned int count)
{
	FAIL_IF(!ChatWindows.empty()) return;
	ChatWindows.reserve(count);
	for (unsigned int i = 0; i < count; i++) {
		GadgetSlider *slider = Unknown_56D8D0(i + 8, 0);
		slider->Unknown_66E460(InterfaceMesh, false, i);
		ChatWinStruct *chat_win = new ChatWinStruct(slider, i);
		ChatWindows.push_back(chat_win);
		char name[32];
		sprintf(name, "vp%d_tl", i);
		chat_win->TLBone = InterfaceMesh->Get_Bone_Index(name);













		G_ASSERT(chat_win->TLBone);
		sprintf(name, "vp%d_br", i);
		chat_win->BRBone = InterfaceMesh->Get_Bone_Index(name);

		G_ASSERT(chat_win->BRBone);
	}
	Unknown_A4();
	ChatHistory->Unknown_67D660(count);
	for (unsigned int index = 0; ; index++) {
		int window;
		const Unknown67D430EntryStruct &entry = ChatHistory->Unknown_67D980(index, &window);
		if (entry.Text.length() == 0) break;
		if (window < 0) continue;
		ChatWindowClass *chat_win = Unknown_571810(window);










		FAIL_IF(!chat_win) continue;
		chat_win->Unknown_0C(Unknown_5719A0(entry), entry.Text);
	}
	Unknown_CC();
}

// 0x5701C0
void ChatWindowInterface::Unknown_58()
{
	UnknownA4->Unknown_40(true);
	if (UnknownDC->Unknown_60()) {
		Unknown_5715F0(true, true);
		Unknown_571470(true);
	}
	UIChatPanelBaseClass::Unknown_58();
}

// 0x570210
void ChatWindowInterface::Unknown_5C()
{
	UnknownA4->Unknown_40(false);
	if (UnknownB8->Peek_Scene()) UnknownB8->Remove();
	UnknownD8->Unknown_685DD0();
	InterfaceClass::Unknown_5C();
}

// 0x570260  // NOT EXACT: functional (CW2): register residue around Create_Peek(Peek_Scene()) (eax / edx swapped; 0.81)
void ChatWindowInterface::Unknown_7C(UIViewClass *view, bool arg)
{
	InterfaceClass::Unknown_7C(view, arg);
	for (std::vector<ChatWinStruct *>::iterator it = ChatWindows.begin(); it != ChatWindows.end(); it++) {
		ChatWinStruct *chat_win = *it; chat_win->ChatWindow->Unknown_67E180(Create_Peek(Peek_Scene()));
	}
}

// 0x570340
void ChatWindowInterface::Unknown_60()
{
	Unknown_571470(false);
	for (std::vector<ChatWinStruct *>::iterator it = ChatWindows.begin(); it != ChatWindows.end(); it++) {
		(*it)->ChatWindow->Unknown_67FFD0();
	}
}




























// 0x570390
bool ChatWindowInterface::Unknown_68(int key)
{
	switch (key) {
		case 1:
			((SignalMap *)Unknown_540A20())->emit(this, ChatWindowInterfaceEvent(1));
			break;
		case 2:
			Unknown_C4(1, 0);
			return(true);
		case 3:
			Unknown_C8(1, 0);
			return(true);
		case 4:
			Unknown_BC(1, 0);
			return(true);
		case 5:
			Unknown_C0(1, 0);
			return(true);
		case 6:
			Unknown_A8();
			return(true);
	}
	unsigned int index = key - 8;
	if (index <= ChatWindows.size()) {
		ChatWindowClass *chat_win = Unknown_571810(index);
		FAIL_IF(!chat_win) return(true);
		chat_win->Unknown_680670();
		return(true);
	}
	return(false);
}




// 0x570510
void ChatWindowInterface::Unknown_70(ControlKeysManager *manager)
{

	FAIL_IF(!manager) return;
	manager->Add_Keys((ControlKeyListClass *)&Unknown4C, std::string("Chat Page Up"), (void *)2, 0);
	manager->Add_Keys((ControlKeyListClass *)&Unknown4C, std::string("Chat Page Down"), (void *)3, 0);
	manager->Add_Keys((ControlKeyListClass *)&Unknown4C, std::string("Scroll Chat Up"), (void *)4, 0);
	manager->Add_Keys((ControlKeyListClass *)&Unknown4C, std::string("Scroll Chat Down"), (void *)5, 0);
	manager->Add_Keys((ControlKeyListClass *)&Unknown4C, std::string("Toggle Chat Window"), (void *)6, 0);
}

// 0x5708F0
void ChatWindowInterface::Unknown_74(ControlKeysManager *manager)
{

	FAIL_IF(!manager) return;
	manager->Remove_Keys((ControlKeyListClass *)&Unknown4C, std::string("Chat Page Up"), 0);
	manager->Remove_Keys((ControlKeyListClass *)&Unknown4C, std::string("Chat Page Down"), 0);
	manager->Remove_Keys((ControlKeyListClass *)&Unknown4C, std::string("Scroll Chat Up"), 0);
	manager->Remove_Keys((ControlKeyListClass *)&Unknown4C, std::string("Scroll Chat Down"), 0);
	manager->Remove_Keys((ControlKeyListClass *)&Unknown4C, std::string("Toggle Chat Window"), 0);
}

// 0x570CD0  // NOT EXACT: functional (CW2): the inlined 0x571A70's RectClass stores (the client stores Bottom first; frame 8 vs 0xC; 0.57)
void ChatWindowInterface::Unknown_A4()
{
	Unknown_571A70(&Viewport);
	Unknown_571470(true);
	for (std::vector<ChatWinStruct *>::iterator it = ChatWindows.begin(); it != ChatWindows.end(); it++) {
		Unknown_571A70(*it);
		(*it)->ChatWindow->Unknown_6804E0((*it)->Rect);
	}
}

// 0x570E40  // NOT EXACT: functional (CW2): as 0x570CD0 (0.70)
void ChatWindowInterface::Unknown_CC()
{
	Unknown_571A70(&Viewport);
	Unknown_571470(true);
	for (std::vector<ChatWinStruct *>::iterator it = ChatWindows.begin(); it != ChatWindows.end(); it++) {
		(*it)->ChatWindow->Unknown_680700(false);
	}
}

// 0x570F20
Unknown65BFD0Class *ChatWindowInterface::Unknown_570F20()
{
	return((UnknownA4 != NULL && UnknownA4->Active) ? UnknownA4 : NULL);
}

// 0x570F50  // NOT EXACT: functional (CW2): register residue (0.78)
void ChatWindowInterface::Unknown_AC(const char *text)
{
	InterfaceSortScopeClass scope(this);
	UnknownA4->Unknown_65E7C0(text);
	if (!UnknownB8->Peek_Scene()) UnknownB8->Add(Peek_Scene());
}


















// 0x570FF0  // NOT EXACT: functional (CW2): the inlined 0x5719A0 (0.66)
void ChatWindowInterface::Unknown_B0()
{
	FAIL_IF(!ChatHistory) return;
	int window = 0;
	const Unknown67D430EntryStruct &entry = ChatHistory->Unknown_67DA60(0, &window);
	if (window < 0) return;
	ChatWindowClass *chat_win = Unknown_571810(window);




	FAIL_IF(!chat_win) return;
	chat_win->Unknown_08(Unknown_5719A0(entry), entry.Text);
}




// 0x571170
void ChatWindowInterface::Unknown_B4(const char *text)
{
	FAIL_IF(!DestBttnText) return;
	DestBttnText->Change_Text(text, 0);
}

// 0x5711C0  // NOT EXACT: functional (CW2): the window's table in edx, the client's in eax (0.84)
bool ChatWindowInterface::Unknown_B8(int key)
{
	for (std::vector<ChatWinStruct *>::iterator it = ChatWindows.begin(); it != ChatWindows.end(); it++) {
		(*it)->ChatWindow->Unknown_14((const char *)key);
	}
	return(false);
}


















// 0x571210  // NOT EXACT: functional (CW2): written from the pseudocode, not aligned (0.76)
void ChatWindowInterface::Unknown_A8()
{
	const char *names[] = { "NEW_FRAME01", "center bar", NULL };
	UnknownE4 = !UnknownE4;
	for (const char **name = names; *name != NULL; name++) {
		RefCountPtr<RenderObjClass> robj = Create_Get(InterfaceMesh->Get_Sub_Object_By_Name(*name));
		if (robj) robj->Set_Hidden(UnknownE4);
	}
	int bar = 0;
	for (std::vector<ChatWinStruct *>::iterator it = ChatWindows.begin(); it != ChatWindows.end(); it++) {
		ChatWinStruct *window = *it;
		FAIL_IF(!window || !window->ChatWindow) continue;
		window->ChatWindow->Unknown_680700(UnknownE4);
		char bar_name[32];
		sprintf(bar_name, "bar_cargo%02d", bar++);
		RefCountPtr<RenderObjClass> robj = Create_Get(InterfaceMesh->Get_Sub_Object_By_Name(bar_name));
		if (robj) robj->Set_Hidden(UnknownE4);
	}
}

// 0x571430
void ChatWindowInterface::Unknown_571430()
{
	for (std::vector<ChatWinStruct *>::iterator it = ChatWindows.begin(); it != ChatWindows.end(); it++) {
		(*it)->ChatWindow->Unknown_6806C0();
	}
}

// 0x571470
void ChatWindowInterface::Unknown_571470(bool force)
{
	if (UnknownA4 == NULL) return;
	Unknown65BFD0Class *edit = Unknown_570F20();
	if (edit != NULL) {
		Unknown_5715F0(true, false);
		bool changed = edit->Changed;
		edit->Changed = false;
		if (changed || force) {
			char buffer[512];
			int cursorpos;
			bool insert; FontSys *line = UnknownB8.Peek();
			edit->Unknown_65EBF0(buffer, sizeof(buffer), cursorpos, insert);
			int width = (int)(Viewport.Rect.Right - Viewport.Rect.Left);
			char *text = TextGadgetClass::Unknown_6AB190(buffer, (float)width, line->Font, cursorpos); UnknownB8->Change_Text(text, 0);
			Vector3 position(Viewport.Rect.Left, (Viewport.Rect.Top - Viewport.Rect.Bottom) * 0.5f + Viewport.Rect.Bottom, 0.0f);
			UnknownB8->Set_Position(position);
			UnknownD8->Unknown_685AB0(UnknownB8, cursorpos, insert, false);
		}
	} else {
		Unknown_5715F0(false, false);
	}
}

// 0x5715F0  // NOT EXACT: functional (CW2): register residue around Create_Peek(Peek_Scene()) (0.71)
void ChatWindowInterface::Unknown_5715F0(bool on, bool force)
{
	if (on == UnknownDC->Unknown_60() && !force) return;
	if (on) {
		if (!Unknown44) return;
		InterfaceSortScopeClass scope(this);
		UnknownDC->Unknown_40(true);
		UnknownDC->Unknown_8C(false);
		if (!UnknownB8->Peek_Scene()) UnknownB8->Add(Peek_Scene());
		UnknownD4->Set_Hidden(false);
		UnknownD8->Unknown_685A30(Create_Peek(Peek_Scene()));
	} else {
		UnknownDC->Unknown_40(false);
		UnknownDC->Unknown_8C(true);
		if (UnknownB8->Peek_Scene()) UnknownB8->Remove();
		UnknownD4->Set_Hidden(true);
		UnknownD8->Unknown_685A30(RefCountPtr<SceneClass>());
	}
}

// 0x571810
ChatWindowClass *ChatWindowInterface::Unknown_571810(unsigned int index)
{
	if (index >= ChatWindows.size()) return(NULL);
	return(ChatWindows[index]->ChatWindow);
}

// 0x571860
void ChatWindowInterface::Unknown_BC(int lines, unsigned int index)
{
	ChatWindowClass *chat_win = Unknown_571810(index);
	if (chat_win) chat_win->Unknown_10(lines);
}

// 0x5718B0
void ChatWindowInterface::Unknown_C0(int lines, unsigned int index)
{
	ChatWindowClass *chat_win = Unknown_571810(index);
	if (chat_win) chat_win->Unknown_10(-lines);
}

// 0x571900
void ChatWindowInterface::Unknown_C4(int pages, unsigned int index)
{
	ChatWindowClass *chat_win = Unknown_571810(index);
	if (chat_win) chat_win->Unknown_10(chat_win->Unknown48 * pages);
}

// 0x571950
void ChatWindowInterface::Unknown_C8(int pages, unsigned int index)
{
	ChatWindowClass *chat_win = Unknown_571810(index);
	if (chat_win) chat_win->Unknown_10(-(int)(chat_win->Unknown48 * pages));
}











































// 0x5719A0  // NOT EXACT: functional (CW2): defined inline so its callers inline it as the client's do; no out-of-line copy (the client has one, uncalled)
inline Vector3 ChatWindowInterface::Unknown_5719A0(const Unknown67D430EntryStruct &entry)
{
	int type = entry.Get_Type();
	Vector3 color(1.0f, 1.0f, 1.0f);
	FAIL_IF(!Client) return(color);
	SClient *sclient = Client->As_Space_Client();

	FAIL_IF(!sclient) return(color);
	color = sclient->Unknown13CC.Unknown_765DF0(type);
	return(color);
}



// 0x571A70  // NOT EXACT: functional (CW2): defined inline so its callers inline it as the client's do; no out-of-line copy (the client has one, uncalled)
inline void ChatWindowInterface::Unknown_571A70(ChatViewportStruct *vp)
{
	FAIL_IF(!vp) return;
	const Matrix3D &tl_tm = InterfaceMesh->Get_Bone_Transform(vp->TLBone);
	Vector2 tl(tl_tm[0][3], tl_tm[1][3]);
	const Matrix3D &br_tm = InterfaceMesh->Get_Bone_Transform(vp->BRBone);
	vp->Rect.Set(tl.X, tl.Y, br_tm[0][3], br_tm[1][3]);
}

/*
**	The chat window (CW2, 2026-11-08): __FILE__ 0xB7D76C ('e:\build\beta\G\app\Client\ui_chat_window.cpp'),
**	0x56F420-0x571EAF (the <string> initialiser 0x571EB0 ends the object; 0x56F400 / 0x56F410 before it are the
**	previous object's adjustor thunks). ChatWindowInterface is the client's RTTI name; its table 0xAF0390 (with
**	0xAF0384 / 0xAF0378 for the bases). The split window derived from it (ui_chat_window_split.cpp, 0x571F00) is what
**	the chat panel makes. 0x571810, 0x5719A0 and 0x571A70 are out of line and uncalled: the file's callers inline
**	them. 0x571B30-0x571E80 are the object's std::vector<ChatWinStruct *> instances.
*/
