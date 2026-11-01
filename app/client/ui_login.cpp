// Earth & Beyond client (reconstruction): app\Client\ui_login.cpp. Notes at the end of the file.
#include <stdio.h>
#include <time.h>
#include <string.h>
#include "sclient.h"
#include "tasks\tsk_login.h"
#include "rendobj.h"
#include "layer.h"
#include "camera.h"
#include "WWAudio.h"
#include <map>
#include <string>
#include "..\..\wwlib\ref_ptr.h"
#include "refcount.h"
#include "..\audio\soundlib.h"
#include "..\shared\unknown_debug.h"
#include "..\shared\auxdata\permstr.h"
#include "tutorial_manager.h"

// gadget.cpp's gadget, as far as this file calls it
class GadgetClass	// NAME 3: class-from-file (D24) for app\client\gadget.cpp
{
public:
	virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C();
	virtual void Unknown_10(); virtual void Unknown_14(); virtual void Unknown_18(); virtual void Unknown_1C();
	virtual void Unknown_20(); virtual void Unknown_24();
	virtual void Unknown_28(LayerClass *layer);			// +0x28 (the text gadgets: the window's layer)  // NAME 1 [layer]: what the constructor passes
	virtual void Unknown_2C(); virtual void Unknown_30(); virtual void Unknown_34(); virtual void Unknown_38(); virtual void Unknown_3C();
	virtual void Unknown_40(bool on);					// +0x40 (shown)  // NAME 1 [on]: neutral
	virtual void Unknown_44(); virtual void Unknown_48(); virtual void Unknown_4C();
	virtual void Unknown_50(); virtual void Unknown_54(); virtual void Unknown_58(); virtual void Unknown_5C();
	virtual void Unknown_60(); virtual void Unknown_64(); virtual void Unknown_68(); virtual void Unknown_6C();
	virtual void Unknown_70();
	virtual void Unknown_74(bool on);					// +0x74 (the text gadgets: the input focus)  // NAME 1 [on]: neutral
	char Unknown04[0x20 - 0x04];						// +0x04
	void *Unknown20;									// +0x20 (0x5AE9D0: the mouse-over sound only when set)
	char Unknown24[0x2C - 0x24];						// +0x24
	unsigned int Unknown2C_00 : 2;						// +0x2C bits 0-1
	unsigned int Unknown2C_02 : 1;						// +0x2C bit 2 (0x5AE9D0: the mouse-over sound)
	unsigned int Unknown2C_03 : 29;						// +0x2C
	bool Is_Unknown2C_02() { return(Unknown2C_02); }	// NAME 1: neutral (D10)
};

// fontmanager.h's FontStyleClass, FontManagerClass at 0xBE67C4, fontdatabase.h's FontStringFilesClass at 0xBE67BC (neither
// header can be included next to sclient.h: workflow.md 3.3), as far as this file calls them
class FontStyleClass : public RenderObjClass	// NAME 1 [FontStyleClass]: fontmanager.h's
{
public:
	void Unknown_6CE7D0(int unknown);					// 0x6CE7D0  // NAME 1 [unknown]: neutral
	char Unknown_Pad[0x2FC - sizeof(RenderObjClass)];
	float Unknown2FC;									// +0x2FC (the buttons' texts: 1.0, or 0.33 when off)
	char Unknown300[0x4C5 - 0x300];						// +0x300
	bool Unknown4C5;									// +0x4C5
	void Unknown_Set_2FC(float value) { Unknown2FC = value; Unknown4C5 = false; }	// NAME 1: neutral (D10), as ui_manu.cpp's
};
class LoginFontManagerClass	// NAME 1: neutral (D10): fontmanager.h's FontManagerClass
{
public:
	void Add(const RefCountPtr<RenderObjClass> &object);	// 0x6D1FB0
	void Remove(unsigned int id);						// 0x6D2220
	FontStyleClass *Get_Style(const char *name, unsigned int id);	// 0x6D25D0
	RefCountPtr<FontStyleClass> Unknown_6D2700(const char *name, const RefCountPtr<RenderObjClass> &model);	// 0x6D2700 (fontmanager.h: an unsigned int reference, the model's pointer)  // NAME 1 [Unknown_6D2700]: as fontmanager.h's; NAME 1 [model]: what the constructor passes
};
extern LoginFontManagerClass *Unknown_BE67C4;			// NAME 1 [Unknown_BE67C4]: 0xBE67C4
class FontStringFilesClass { public: void Load(const char *name); void Unload(const char *name); };	// NAME 2: fontdatabase.h's

// textgadg.cpp's text gadget (0x80 bytes, constructor 0x6AA810), as far as this file calls it
class TextGadgetClass : public GadgetClass	// NAME 3: class-from-file (D24) for app\client\textgadg.cpp
{
public:
	TextGadgetClass(InterfaceClass *iface, int length, const char *name, RenderObjClass *model, int a, int b);	// 0x6AA810  // NAME 1 [iface, length, name, model, a, b]: what the constructor passes
	void Set_Text(const char *text, int unknown);		// 0x6AA570  // NAME 1: what it does
	char *Get_Text();									// 0x6AA6C0 (a copy, deleted by the caller)  // NAME 1: what it does
	void Unknown_6AAD10();								// 0x6AAD10 (every frame)
	char Unknown30[0x39 - 0x30];						// +0x30
	bool Unknown39;										// +0x39 (the slot +0x60 clears it: tab)
	bool Unknown3A;										// +0x3A
	char Unknown3B;										// +0x3B
	bool Unknown3C;										// +0x3C
	char Unknown3D[0x44 - 0x3D];						// +0x3D
	FontStyleClass *Unknown44;							// +0x44 (its text)
	char Unknown48[0x5C - 0x48];						// +0x48
	bool Unknown5C;										// +0x5C (the password's: hidden)
	char Unknown5D[0x80 - 0x5D];						// +0x5D
};
extern const int Unknown_B08CC4;						// NAME 1: neutral (D10): what the text gadgets get

// gadget.cpp's buttons (100 bytes, 0x675240), as ui_manu.cpp makes them
class LoginButtonGadgetClass : public GadgetClass	// NAME 1: neutral (D10)
{
public:
	LoginButtonGadgetClass(RenderObjClass *robj, LayerClass *layer, int a, int b);	// 0x675240 (0x55, 3)  // NAME 1 [robj, layer, a, b]: as ui_manu.cpp's
	char Unknown30[0x64 - 0x30];						// +0x30
};
void Unknown_65EED0(GadgetClass *gadget);				// 0x65EED0 (cdecl, gadget.cpp): the gadget deleted  // NAME 1 [gadget]: as ui_cpit.cpp's
void Unknown_496AE0(RenderObjClass *robj, int unknown);	// 0x496AE0 (ro_funcs.cpp, cdecl)  // NAME 1 [robj, unknown]: as eff_o.cpp's
RefCountPtr<RenderObjClass> Unknown_497300(RenderObjClass *robj);	// 0x497300 (cdecl)  // NAME 1 [robj]: as ui_avatr.cpp's

// The object at *0xBFAEE0 (kernel.cpp's): its slot +0x14 makes a render object by name
class UnknownBFAEE0Class	// NAME 1 [UnknownBFAEE0Class]: kernel.cpp's
{
public:
	virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C(); virtual void Unknown_10();
	virtual RenderObjClass *Create_Render_Obj(const char *name);	// +0x14  // NAME 2: what it does (the asset manager's slot)
};
extern UnknownBFAEE0Class *Unknown_BFAEE0;				// NAME 1 [Unknown_BFAEE0]: 0xBFAEE0

// intrface.cpp's scope object (constructor 0x541980 over an interface, destructor 0x6E18B0), as ui_manu.cpp's
class LoginScopeClass	// NAME 1: neutral (D10)
{
public:
	LoginScopeClass(InterfaceClass *iface);				// 0x541980  // NAME 1 [iface]: neutral
	~LoginScopeClass();									// 0x6E18B0
	char Unknown00[8];
};

// The control keys event (0x10 bytes) and InterfaceClass +0x4C, as ui_manu.cpp declares them
struct LoginKeyEventStruct	// NAME 1: neutral (D10)
{
	LoginKeyEventStruct(short key) : Key(key), Unknown02(0), Unknown04(0.0f), Unknown08(0.0f), Unknown0C(0) {}	// NAME 1 [key]: neutral
	short Key;											// +0x00  // NAME 1: neutral (D10)
	short Unknown02;									// +0x02
	float Unknown04;									// +0x04 (stored as an immediate 0: a float)
	float Unknown08;									// +0x08
	int Unknown0C;										// +0x0C
};
class LoginKeysClass { public: void Unknown_721E60(const LoginKeyEventStruct *event, int id); };	// NAME 1: neutral (D10)  // NAME 1 [event, id]: what the constructor passes

// What the character view's +0x198 points to (avatar.cpp's functions), as far as this file calls it
class LoginViewUnknown198Class	// NAME 1: neutral (D10)
{
public:
	void Unknown_784C90(const char *name, int a);		// 0x784C90 ('RejectedOnce', 1)  // NAME 1 [name, a]: neutral
	void Unknown_784E00();								// 0x784E00
	bool Unknown_7863E0();								// 0x7863E0
	char Unknown000[0x220];								// +0x000
	bool Unknown220;									// +0x220 (0x5ADA50: the welcome speech waits for it)
	bool Is_Unknown220() { return(Unknown220); }		// NAME 1: neutral (D10)
};

// The character view (charview.cpp's CharacterViewClass, tsk_login.h), as far as this file calls it
class LoginViewClass	// NAME 1: neutral (D10)
{
public:
	virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C();
	virtual void Unknown_10(); virtual void Unknown_14(); virtual void Unknown_18(); virtual void Unknown_1C();
	virtual void Unknown_20(); virtual void Unknown_24();
	virtual LoginViewClass *Unknown_28();				// +0x28 (the interface's view gives the character view)
	void Unknown_67A250();								// 0x67A250
	void Unknown_67A670();								// 0x67A670
	void Unknown_67A6C0();								// 0x67A6C0
	void Unknown_67A8D0(int arg);						// 0x67A8D0  // NAME 1 [arg]: neutral
	void Unknown_67A9B0(int arg);						// 0x67A9B0 (the destructor: 0)  // NAME 1 [arg]: as ui_avatr.cpp's
	void Unknown_67AB20(int index, const char *name);	// 0x67AB20  // NAME 1 [index, name]: neutral
	void Unknown_67ACE0(int arg);						// 0x67ACE0  // NAME 1 [arg]: neutral
	bool Unknown_67AD30(int a, const char *name);		// 0x67AD30  // NAME 1 [a, name]: neutral
	void Unknown_67AE20(int arg);						// 0x67AE20  // NAME 1 [arg]: neutral
	void Unknown_67AE60(int arg);						// 0x67AE60  // NAME 1 [arg]: neutral
	bool Unknown_67AEA0(int arg);						// 0x67AEA0  // NAME 1 [arg]: neutral
	void Unknown_67B270();								// 0x67B270
	bool Unknown_67B2B0();								// 0x67B2B0 (as ui_avatr.cpp's)
	void Unknown_67B4F0(int arg);						// 0x67B4F0  // NAME 1 [arg]: neutral
	void Unknown_67C440(int a, int b);					// 0x67C440  // NAME 1 [a, b]: neutral
	void Unknown_67C7B0();								// 0x67C7B0

	char Unknown04[0x14 - 0x04];						// +0x04
	TaskClass *Unknown014;								// +0x14 (its slot +0x08 gives the login task)
	char Unknown18[0x34 - 0x18];						// +0x18
	CharacterViewUnknown34Class *Get_Unknown034() { return(Unknown034); }	// NAME 1: neutral (D10)
	CharacterViewUnknown34Class *Unknown034;			// +0x34 (the message box)
	char Unknown38[0x68 - 0x38];						// +0x38
	CameraClass *Peek_Camera() { return(Unknown068); }	// NAME 1: neutral (D10)
	CameraClass *Unknown068;							// +0x68
	char Unknown6C[0x198 - 0x6C];						// +0x6C
	LoginViewUnknown198Class *Get_Unknown198() { return(Unknown198); }	// NAME 1: neutral (D10)
	LoginViewUnknown198Class *Unknown198;				// +0x198
	char Unknown19C[0x1B0 - 0x19C];						// +0x19C
	int Get_Unknown1B0() { return(Unknown1B0); }		// NAME 1: neutral (D10)
	int Unknown1B0;										// +0x1B0 (the step before)
	char Unknown1B4[0x1B8 - 0x1B4];						// +0x1B4
	RenderObjClass *Peek_Model() { return(Unknown1B8); }	// NAME 1: neutral (D10), as ui_avatr.cpp's
	RenderObjClass *Unknown1B8;							// +0x1B8 (the model: its texts by name)
	char Unknown1BC[0x1CF - 0x1BC];						// +0x1BC
	bool Unknown1CF;									// +0x1CF
	bool Unknown1D0;									// +0x1D0
	char Unknown1D1[0x3A4 - 0x1D1];						// +0x1D1
	SClientUnknown1338Class *Unknown3A4;				// +0x3A4 (env_map_manager.cpp's)
};

// The gadget events' callbacks (vtable 0xAEDB58) and what 0x65ECE0 returns, as ui_avatr.cpp declares them
class LoginCallbackBaseClass	// NAME 1: neutral (D10): vtable 0xAEDB58 (slot 0 0x51E220)
{
public:
	~LoginCallbackBaseClass() {}
	virtual void Call();
};
class LoginNotifierClass		// NAME 1: neutral (D10)
{
public:
	void Add(int id, LoginCallbackBaseClass *callback);		// 0x4F9740  // NAME 2: as sclient.cpp's
	void Remove(int id, LoginCallbackBaseClass *callback);	// 0x4F98D0  // NAME 2: as sclient.cpp's
};
LoginNotifierClass *Unknown_65ECE0();					// 0x65ECE0  // NAME 1 [Unknown_65ECE0]: neutral (D10)

class FlashingGadgetEvent	// NAME 5: the client's RTTI .?AVFlashingGadgetEvent@@
{
public:
	GadgetClass *Gadget;								// +0x00 (the FAIL_IF '!gadget')  // NAME 3: the FAIL_IF's
};

class SClientUnknown1338Class { public: void Unknown_760770(int base_asset, bool a); };	// NAME 1: neutral (D10): sclient.cpp's (env_map_manager.cpp)
bool Unknown_5FAE70();									// NAME 1 [Unknown_5FAE70]: ui_registration.cpp

// The settings store 0x7CA2D0 returns (tsk_game.h's Unknown_7CA2D0Class), as far as the constructor clears it
class LoginSettingsClass	// NAME 1: neutral (D10)
{
public:
	char Unknown00[0x60];								// +0x00
	void Unknown_7CB2E0(const char *url, int a);		// 0x7CB2E0 (the account management site)  // NAME 1 [url, a]: neutral
	void Unknown_Clear() { Unknown60.clear(); }			// NAME 1: neutral (D10)
	std::map<std::string, std::string> Unknown60;		// +0x60 (the constructor empties it)
};

// The login screen (0xDC bytes, table 0xAF2BF0; charview.cpp's 0x679160 makes it for step 1)
class LoginInterface : public InterfaceClass	// NAME 5: the client's RTTI (0xB817A8: SignalBaseClass::MemberCallback<GadgetClass,LoginInterface,...>)
{
public:
	LoginInterface(UIViewClass *view, LayerClass *layer);	// 0x5ACB80 (charview.cpp's 0x679160: the view and its +0x74)  // NAME 1 [view, layer]: what 0x679160 passes
	virtual ~LoginInterface();							// 0x5AD380 (slot 0x5AD350)
	virtual void Unknown_60();							// 0x5AD540 (every frame)
	virtual bool Unknown_68(int key);					// 0x5AD610 (the buttons)
	void Unknown_5ADA50();								// 0x5ADA50 (slot +0x60's first call)
	void Unknown_5AEA60();								// 0x5AEA60 (once the task's +0x1D is set: the account expiry)
	bool Unknown_5AE120();								// 0x5AE120: no character in any of the five slots
	void Unknown_5AE160();								// 0x5AE160: the buttons on
	void Unknown_5AE240();								// 0x5AE240: the buttons off
	void Unknown_5AE320();								// 0x5AE320 (FAIL_IF '!textptr')
	void Unknown_5AE390();								// 0x5AE390: the focus to the next text gadget
	void Unknown_5AE450();								// 0x5AE450: the name and password handed to the task
	void Unknown_5AE590();								// 0x5AE590: the account menu shown (G_ASSERT 'AccountMenu')
	void Unknown_5AE7F0();								// 0x5AE7F0: the account menu hidden (G_ASSERT 'AccountMenu')
	void Unknown_5AE8E0(bool on);						// 0x5AE8E0: the account menu's buttons  // NAME 1 [on]: neutral
	void Unknown_5AE9D0(FlashingGadgetEvent *event, GadgetClass *sender);	// 0x5AE9D0 (FAIL_IFs '!event', '!gadget': the event's gadget)  // NAME 3 [event]: the FAIL_IF's; NAME 1 [sender]: the callback's type

	GadgetClass *AccountButtons[4];						// +0x84 (the account menu's 'BUTTON01' ..)  // NAME 1: neutral (D10)
	RefCountPtr<RenderObjClass> AccountMenu;			// +0x94 ('FE13A')  // NAME 3: the G_ASSERT's
	LoginViewClass *Get_Character_View() { return(CharacterView); }	// NAME 1: neutral (D10)
	LoginViewClass *CharacterView;						// +0x98 (the view's slot +0x28)  // NAME 1: tsk_login.h's
	LoginTaskClass *Get_Task() { return(Task); }		// NAME 1: neutral (D10)
	LoginTaskClass *Task;								// +0x9C (the character view's +0x14's slot +0x08)  // NAME 1: neutral (D10)
	int Unknown0A0;										// +0x0A0 (the account menu's step: 11, 12)
	TextGadgetClass *NameGadget;						// +0xA4 ('Name_fe01a')  // NAME 1: its model's name
	TextGadgetClass *PasswordGadget;					// +0xA8 ('Password_fe01a')  // NAME 1: its model's name
	bool Unknown0AC;									// +0xAC
	int Unknown0B0;										// +0xB0 (the rejections: 'RejectedOnce', 'RejectedTwice')
	GadgetClass *Buttons[5];							// +0xB4 ('BUTTON_NAME' ..)  // NAME 1: neutral (D10)
	bool Unknown0C8;									// +0xC8
	bool Unknown0C9;									// +0xC9
	bool Unknown0CA;									// +0xCA
	bool Unknown0CB;									// +0xCB
	bool Unknown0CC;									// +0xCC
	bool Unknown0CD;									// +0xCD
	bool Unknown0CE;									// +0xCE
	int Focus;											// +0xD0 (0 the name, 1 the password, -1 none)  // NAME 1: what it holds
	LayerClass *Layer;									// +0xD4  // NAME 2: what it holds
	bool Unknown0D8;									// +0xD8
	bool Unknown0D9;									// +0xD9
	bool Unknown0DA;									// +0xDA
};

// The window's FlashingGadgetEvent callback (vtable 0xAF2BBC over 0xAEDB58)
class LoginCallbackClass : public LoginCallbackBaseClass	// NAME 1: neutral (D10)
{
public:
	LoginCallbackClass(LoginInterface *object, void (LoginInterface::*function)(FlashingGadgetEvent *, GadgetClass *)) : Object(object), Function(function) {}
	virtual void Call();

	LoginInterface *Object;								// +0x04
	void (LoginInterface::*Function)(FlashingGadgetEvent *, GadgetClass *);	// +0x08 (8 bytes: two bases)
};

static const char *Unknown_B810DC[] = { "BUTTON_NAME", "BUTTON_PASSWORD", "BUTTON_NEW", "BUTTON_REGISTRA", "EXIT" };	// NAME 1: neutral (D10)
static const char *Unknown_B810F4[] = { "Name_fe01a", "Password_fe01a", "New_fe01a", "Register_fe01a", "Quit_fe01a" };	// NAME 1: neutral (D10)
static const char *Unknown_B8110C[] = { "BUTTON01", "BUTTON02", "BUTTON03", "BUTTON04" };	// NAME 1: neutral (D10)
static const char *Unknown_B8111C[] = { "BUTTON01_FE13A", "BUTTON02_FE13A", "BUTTON03_FE13A", "BUTTON04_FE13A" };	// NAME 1: neutral (D10)


// NOT EXACT: scheduling (the client clears the buttons in the member stores' run, ours before it, and loads
// the model's address before the font manager for Add) (0x5ACB80)
LoginInterface::LoginInterface(UIViewClass *view, LayerClass *layer) :
	InterfaceClass(view, (int)layer->Peek_Scene()),
	CharacterView(NULL),
	Task(NULL),
	Unknown0A0(0),
	NameGadget(NULL),
	PasswordGadget(NULL),
	Unknown0AC(false),
	Unknown0B0(0),
	Unknown0C8(false),
	Unknown0C9(false),
	Unknown0CA(false),
	Unknown0CB(false),
	Unknown0CC(false),
	Unknown0CD(false),
	Unknown0CE(false),
	Focus(-1),
	Layer(layer),
	Unknown0D8(false),
	Unknown0D9(false),
	Unknown0DA(false)
{
	for (int i = 0; i < 5; i++) {
		Buttons[i] = NULL;
	}
	for (int j = 0; j < 4; j++) {
		AccountButtons[j] = NULL;
	}
	((LoginSettingsClass *)Unknown_7CA2D0())->Unknown_Clear();
	CharacterView = ((LoginViewClass *)View)->Unknown_28();
	Task = (LoginTaskClass *)Get_Character_View()->Unknown014->Unknown_08();
	Get_Task()->Close_Global_Connection();
	Get_Task()->Unknown_64();
	LoginScopeClass scope(this);
	Get_Character_View()->Unknown_67A250();
	Get_Character_View()->Unknown_67AB20(0, "FE01A");
	RefCountPtr<RenderObjClass> model = Unknown_497300(CharacterView->Peek_Model());
	Get_Character_View()->Unknown068->Set_Clip_Planes(1.0f, 1000.0f);
	Get_Character_View()->Unknown_67AB20(3, "comp00a");
	CharacterView->Unknown_67C440(0, 1);
	CharacterView->Unknown_67AB20(4, "FE_NAR_AGG");
	CharacterView->Unknown_67A6C0();
	CharacterView->Unknown_67B4F0(0);
	((FontStringFilesClass *)Unknown_BE67BC)->Load("fa_fe01a.ini");
	Unknown_BE67C4->Add(model);
	for (int n = 0; n < 5; n++) {
		RefCountPtr<RenderObjClass> robj = Create_Get(model->Get_Sub_Object_By_Name(Unknown_B810DC[n]));
		Unknown_496AE0(robj.Peek(), 0);
		Buttons[n] = new LoginButtonGadgetClass(robj.Peek(), layer, 0x55, 3);
		Unknown64.Unknown_6870E0(Buttons[n], n + 1, View, 1, 1);
	}
	NameGadget = new TextGadgetClass(this, 0x14, "Name_fe01a", model.Peek(), Unknown_B08CC4, 0);
	NameGadget->Set_Text(Get_Task()->Username, 0);
	NameGadget->Unknown_28(Layer);
	Unknown64.Unknown_6870E0(NameGadget, 7, View, 1, 1);
	PasswordGadget = new TextGadgetClass(this, 0x14, "Password_fe01a", model.Peek(), Unknown_B08CC4, 0);
	PasswordGadget->Unknown5C = true;
	PasswordGadget->Set_Text("", 0);
	PasswordGadget->Unknown_28(Layer);
	Unknown64.Unknown_6870E0(PasswordGadget, 8, View, 1, 1);
	((LoginKeysClass *)Unknown4C)->Unknown_721E60(&LoginKeyEventStruct(0x1B), 5);
	((LoginKeysClass *)Unknown4C)->Unknown_721E60(&LoginKeyEventStruct(0x0D), 3);
	((LoginKeysClass *)Unknown4C)->Unknown_721E60(&LoginKeyEventStruct(0x09), 9);
	RefCountPtr<FontStyleClass> banner = Unknown_BE67C4->Unknown_6D2700("Banner_fe01a", model);
	RefCountPtr<FontStyleClass> demo = Unknown_BE67C4->Unknown_6D2700("DemoBanner_fe01a", model);
	if (Unknown_5FADE0()) {
		if (banner) banner->Set_Hidden(true);
		if (demo) demo->Set_Hidden(false);
	} else {
		if (banner) banner->Set_Hidden(false);
		if (demo) demo->Set_Hidden(true);
	}
	if (CharacterView->Unknown_67AD30(0, "FE01A.FE01A-1")) {
		CharacterView->Unknown_67AE20(0);
	}
	Unknown0CA = true;
	Unknown_5AE240();
	CharacterView->Unknown3A4->Unknown_760770(100000, true);
	void (LoginInterface::*function)(FlashingGadgetEvent *, GadgetClass *) = &LoginInterface::Unknown_5AE9D0;
	LoginNotifierClass *notifier = Unknown_65ECE0();
	notifier->Add(0, new LoginCallbackClass(this, function));
	if (WWAudioClass::Get_Instance()) {
		WWAudioClass::Get_Instance()->Unknown_Resume_All();
	}
}


// 0x5AD380
LoginInterface::~LoginInterface()
{
	if (CharacterView->Unknown198) {
		CharacterView->Unknown198->Unknown_784E00();
	}
	Unknown_5C();
	for (int i = 0; i < 5; i++) {
		if (Buttons[i]) {
			Unknown_65EED0(Buttons[i]);
			Buttons[i] = NULL;
		}
	}
	Unknown_65EED0(NameGadget);
	Unknown_65EED0(PasswordGadget);
	NameGadget = NULL;
	PasswordGadget = NULL;
	Unknown_BE67C4->Remove((unsigned int)CharacterView->Peek_Model());
	((FontStringFilesClass *)Unknown_BE67BC)->Unload("fa_fe01a.ini");
	CharacterView->Unknown_67A9B0(0);
	void (LoginInterface::*function)(FlashingGadgetEvent *, GadgetClass *) = &LoginInterface::Unknown_5AE9D0;
	LoginNotifierClass *notifier = Unknown_65ECE0();
	LoginCallbackClass callback(this, function);
	notifier->Remove(0, &callback);
}


// 0x5AD540
void LoginInterface::Unknown_60()
{
	bool tab = false;
	Unknown_5ADA50();
	if (NameGadget) {
		NameGadget->Unknown_6AAD10();
		if (NameGadget->Unknown39) {
			NameGadget->Unknown39 = false;
			Unknown_5AE390();
		}
	}
	if (PasswordGadget) {
		PasswordGadget->Unknown_6AAD10();
		if (PasswordGadget->Unknown39) {
			PasswordGadget->Unknown39 = false;
			Focus = -1;
			tab = true;
		}
	}
	if (PasswordGadget->Unknown3A) {
		PasswordGadget->Unknown3A = false;
		Unknown0AC = false;
	} else if (tab || AccountMenu) {
		Unknown0AC = false;
	} else if (Unknown0AC && !PasswordGadget->Unknown3C) {
		Unknown_5AE450();
	}
}


// 0x5AD610
bool LoginInterface::Unknown_68(int key)
{
	if (Unknown0C8) return(false);
	if (Unknown0C9) return(false);
	if (Unknown0CA) return(false);
	if (Unknown0CB) return(false);
	if (Unknown0CC) return(false);
	if (Unknown0CD) return(false);
	if (!AccountMenu) {
		switch (key) {
			case 1:
				Focus = 0;
				PasswordGadget->Unknown_74(false);
				NameGadget->Unknown_74(true);
				NameGadget->Set_Text(NULL, 0);
				Unknown0AC = false;
				break;
			case 2:
				Focus = 1;
				NameGadget->Unknown_74(false);
				PasswordGadget->Unknown_74(true);
				PasswordGadget->Set_Text(NULL, 0);
				Unknown0AC = true;
				break;
			case 3:
				Unknown_5AE450();
				break;
			case 5:
				Unknown_5AE240();
				CharacterView->Unknown_67B270();
				break;
			case 4:
				NameGadget->Unknown_74(false);
				PasswordGadget->Unknown_74(false);
				Unknown_5AE240();
				Unknown_5AE590();
				break;
			case 9:
				Unknown_5AE390();
				break;
			default:
				return(false);
		}
		SoundLibraryClass::Play_2D_Sound("Login_Mouse_Click", 2);
		return(true);
	}
	SoundLibraryClass::Play_2D_Sound("ButtonClick02.wav", 2);
	switch (key) {
		case 4:
		case 5:
			Unknown_5AE7F0();
			Unknown_5AE160();
			return(false);
		case 1:
			Unknown0D8 = true;
			CharacterView->Unknown034->Unknown_588A70("YES_NO", 0, "This will exit Earth & Beyond and take you to the EA.COM Account Management Website.\nDo you wish to proceed?", 320, 0, 0);
			return(false);
		case 2:
			if (Unknown_5FAE70()) {
				CharacterView->Unknown198->Unknown_784E00();
				Unknown_5AE7F0();
				Unknown0A0 = 12;
				CharacterView->Unknown_67A8D0(1);
				Unknown0CE = true;
				if (CharacterView->Unknown_67AD30(0, "FE01A.FE01A-2")) {
					CharacterView->Unknown_67AE20(0);
				} else {
					CharacterView->Unknown_67AE60(0);
				}
				Unknown_5AE320();
				Unknown0CB = true;
			} else {
				CharacterView->Unknown034->Unknown_588A70("OK", 0, "Sorry, This feature is not implemented yet.", 320, 0, 0);
			}
			break;
		case 3:
			if (Unknown_5FAE70()) {
				CharacterView->Unknown198->Unknown_784E00();
				Unknown_5AE7F0();
				Unknown0A0 = 11;
				CharacterView->Unknown_67A8D0(1);
				Unknown0CE = true;
				if (CharacterView->Unknown_67AD30(0, "FE01A.FE01A-2")) {
					CharacterView->Unknown_67AE20(0);
				} else {
					CharacterView->Unknown_67AE60(0);
				}
				Unknown_5AE320();
				Unknown0CB = true;
			} else {
				CharacterView->Unknown034->Unknown_588A70("OK", 0, "Sorry, This feature is not implemented yet.", 320, 0, 0);
			}
			break;
	}
	return(false);
}


// 0x5ADA50
void LoginInterface::Unknown_5ADA50()
{
	if (CharacterView->Get_Unknown198()->Is_Unknown220() && Unknown0CA && CharacterView->Unknown_67AEA0(0)) {
		Unknown0CA = false;
		Unknown0C9 = false;
		Unknown_5AE160();
		std::string speech;
		if (CharacterView->Get_Unknown1B0() == 9) {
			speech = "WelcomeReg";
		} else {
			speech = "Welcome";
		}
		if (!TutorialManagerClass::Instance->Is_Completed(speech.c_str())) {
			CharacterView->Get_Unknown198()->Unknown_784C90(speech.c_str(), 0);
			TutorialManagerClass::Instance->Set_Completed("Welcome");
			TutorialManagerClass::Instance->Set_Completed("WelcomeReg");
		} else {
			char name[64];
			_snprintf(name, 64, "ReturningUser%d", time(NULL) % 3);
			if (!TutorialManagerClass::Instance->Is_Completed(name)) {
				CharacterView->Get_Unknown198()->Unknown_784C90(name, 1);
				TutorialManagerClass::Instance->Set_Completed("ReturningUser0");
				TutorialManagerClass::Instance->Set_Completed("ReturningUser1");
			}
			Focus = 1;
			PasswordGadget->Unknown_74(true);
			PasswordGadget->Set_Text(NULL, 0);
			Unknown0AC = true;
		}
	}
	if (Unknown0CE) {
		if (CharacterView->Get_Unknown198()->Unknown_7863E0()) {
			Unknown0CE = false;
		}
	}
	if (!Unknown0CE && Unknown0CB && CharacterView->Unknown_67AEA0(0)) {
		CharacterView->Get_Unknown198()->Unknown_784E00();
		if (Unknown0A0 == 10) {
			Get_Task()->Set_Step(3, false);
		} else if (Unknown0A0 == 11) {
			Get_Task()->Set_Step(10, false);
		} else if (Unknown0A0 == 12) {
			Get_Task()->Set_Step(11, false);
		} else {
			CharacterView->Unknown1CF = true;
			CharacterView->Unknown1D0 = true;
			Get_Task()->Go_Next_Step();
			CharacterView->Unknown_67C7B0();
		}
		CharacterView->Unknown_67ACE0(4);
		CharacterView->Unknown_67A670();
	}
	if (Get_Task()->Unknown001D) {
		if (!Unknown0DA) {
			Unknown0DA = true;
			Unknown_5AEA60();
		}
		if (!CharacterView->Get_Unknown034()->Unknown07C) {
			Get_Task()->Unknown001D = false;
			Unknown_7CA2D0()->Set_String("lkey", Get_Task()->Ticket_Info.Login_Key.c_str());
			if (Unknown_5AE120()) {
				Unknown0A0 = 10;
			} else {
				Unknown0A0 = 13;
			}
			CharacterView->Unknown_67A8D0(1);
			Unknown0CE = true;
			if (CharacterView->Unknown_67AD30(0, "FE01A.FE01A-2")) {
				CharacterView->Unknown_67AE20(0);
			} else {
				CharacterView->Unknown_67AE60(0);
			}
			Unknown_5AE320();
			Unknown0CB = true;
		}
	}
	if (CharacterView->Unknown_67B2B0()) {
		Unknown_5AE160();
	}
	if (CharacterView->Get_Unknown034()->Unknown07C && !Unknown0DA) {
		if (!Unknown0C9) {
			Unknown0C9 = true;
			Unknown_5AE240();
		}
		Unknown0D9 = false;
	} else if (Unknown0C9 && !Unknown0D9 && !Unknown0CA) {
		Unknown0C9 = false;
		Unknown_5AE160();
	}
	if (Unknown0D8) {
		int button = CharacterView->Get_Unknown034()->Unknown_5890B0("YES_NO");
		if (button) {
			if (button == 1) {
				CharacterView->Get_Unknown034()->Unknown_5C();
				((LoginSettingsClass *)Unknown_7CA2D0())->Unknown_7CB2E0("https://jump-subs.ea.com/SubscribeEntry.jsp?prodID=REG-EB&iPath=3&CancelURL=http%3A%2F%2Fwww.earthandbeyond.ea.com%2F", 1);
				Unknown0D8 = false;
				Get_Task()->Unknown_6C();
				return;
			}
			CharacterView->Get_Unknown034()->Unknown_5C();
			Unknown0D8 = false;
		}
	}
}


// 0x5AE120
bool LoginInterface::Unknown_5AE120()
{
	int empty = 0;
	for (int i = 0; i < 5; i++) {
		if (Task->Characters[i].ID == 0) {
			empty++;
		}
	}
	bool result = (empty == 5);
	return(result);
}


// 0x5AE160
void LoginInterface::Unknown_5AE160()
{
	if (AccountMenu) {
		Unknown_5AE8E0(true);
		return;
	}
	for (int i = 0; i < 5; i++) {
		if (Buttons[i]) {
			Buttons[i]->Unknown_40(true);
			FontStyleClass *text = Unknown_BE67C4->Get_Style(Unknown_B810F4[i], (unsigned int)CharacterView->Peek_Model());
			if (text) {
				text->Unknown_Set_2FC(1.0f);
			}
		}
	}
	NameGadget->Unknown44->Unknown_Set_2FC(1.0f);
	PasswordGadget->Unknown44->Unknown_Set_2FC(1.0f);
}


// 0x5AE240
void LoginInterface::Unknown_5AE240()
{
	if (AccountMenu) {
		Unknown_5AE8E0(false);
		return;
	}
	for (int i = 0; i < 5; i++) {
		if (Buttons[i]) {
			Buttons[i]->Unknown_40(false);
			FontStyleClass *text = Unknown_BE67C4->Get_Style(Unknown_B810F4[i], (unsigned int)CharacterView->Peek_Model());
			if (text) {
				text->Unknown_Set_2FC(0.33f);
			}
		}
	}
	NameGadget->Unknown44->Unknown_Set_2FC(0.33f);
	PasswordGadget->Unknown44->Unknown_Set_2FC(0.33f);
}














// 0x5AE320
void LoginInterface::Unknown_5AE320()
{
	FontStyleClass *textptr = Unknown_BE67C4->Get_Style("Banner_fe01a", (unsigned int)CharacterView->Peek_Model());
	FAIL_IF(!textptr) return;
	textptr->Unknown_6CE7D0(2);
}


// 0x5AE390
void LoginInterface::Unknown_5AE390()
{
	Focus++;
	if (Focus == 2) {
		Focus = -1;
	}
	if (Focus == 0) {
		PasswordGadget->Unknown_74(false);
		NameGadget->Unknown_74(true);
		NameGadget->Set_Text(NULL, 0);
	} else if (Focus == 1) {
		NameGadget->Unknown_74(false);
		PasswordGadget->Unknown_74(true);
		PasswordGadget->Set_Text(NULL, 0);
		Unknown0AC = true;
	}
}


// 0x5AE450
void LoginInterface::Unknown_5AE450()
{
	Unknown0D9 = true;
	Unknown0C9 = true;
	Unknown0AC = false;
	NameGadget->Unknown_74(false);
	PasswordGadget->Unknown_74(false);
	Unknown_5AE240();
	char *name = NameGadget->Get_Text();
	char *password = PasswordGadget->Get_Text();
	Task->Set_Username(name);
	Task->Set_Password(password);
	delete name;
	delete password;
	CharacterView->Unknown198->Unknown_784E00();
	if (Task->Unknown_4C()) {
		if (Unknown0B0 < 1) {
			CharacterView->Unknown198->Unknown_784C90("RejectedOnce", 1);
		} else {
			CharacterView->Unknown198->Unknown_784C90("RejectedTwice", 1);
		}
		Unknown0B0++;
		Unknown_5AE160();
		Unknown0C9 = false;
		Unknown0D9 = false;
	}
}















// 0x5AE590
void LoginInterface::Unknown_5AE590()
{
	Unknown_5C();
	RenderObjClass *model = CharacterView->Unknown1B8;
	AccountMenu = Create_Get(Unknown_BFAEE0->Create_Render_Obj("FE13A"));
	G_ASSERT(AccountMenu);
	model->Add_Sub_Object(AccountMenu.Peek());
	((FontStringFilesClass *)Unknown_BE67BC)->Load("fa_fe13a.ini");
	Unknown_BE67C4->Add(AccountMenu);
	for (int i = 0; i < 4; i++) {
		RefCountPtr<RenderObjClass> robj = Create_Get(AccountMenu->Get_Sub_Object_By_Name(Unknown_B8110C[i]));
		Unknown_496AE0(robj.Peek(), 0);
		AccountButtons[i] = new LoginButtonGadgetClass(robj.Peek(), Layer, 0x55, 3);
		Unknown64.Unknown_6870E0(AccountButtons[i], i + 1, View, 1, 0);
	}
	Unknown_5AE8E0(true);
	InterfaceClass::Unknown_58();
	Unknown_58();
}














// 0x5AE7F0
void LoginInterface::Unknown_5AE7F0()
{
	Unknown_5C();
	for (int i = 0; i < 4; i++) {
		Unknown64.Unknown_687150(AccountButtons[i]);
		AccountButtons[i] = NULL;
	}
	Unknown_BE67C4->Remove((unsigned int)AccountMenu.Peek());
	((FontStringFilesClass *)Unknown_BE67BC)->Unload("fa_fe13a.ini");
	RenderObjClass *model = CharacterView->Unknown1B8;
	G_ASSERT(AccountMenu);
	model->Remove_Sub_Object(AccountMenu.Peek());
	AccountMenu = NULL;
	InterfaceClass::Unknown_58();
	Unknown_58();
}


// 0x5AE8E0
void LoginInterface::Unknown_5AE8E0(bool on)
{
	for (int i = 0; i < 4; i++) {
		if (AccountButtons[i]) {
			AccountButtons[i]->Unknown_40(on);
			FontStyleClass *text = Unknown_BE67C4->Get_Style(Unknown_B8111C[i], (unsigned int)AccountMenu.Peek());
			if (text) {
				text->Unknown_Set_2FC(on ? 1.0f : 0.33f);
			}
		}
	}
	if (Unknown_5FADE0()) {
		AccountButtons[2]->Unknown_40(false);
		FontStyleClass *text = Unknown_BE67C4->Get_Style("BUTTON03_FE13A", (unsigned int)AccountMenu.Peek());
		if (text) {
			text->Unknown_Set_2FC(0.33f);
		}
	}
}





// 0x5AE9D0
void LoginInterface::Unknown_5AE9D0(FlashingGadgetEvent *event, GadgetClass *sender)
{
	FAIL_IF(!event) return;
	GadgetClass *gadget = event->Gadget;


	FAIL_IF(!gadget) return;
	if (gadget->Unknown20 && gadget->Is_Unknown2C_02()) {
		SoundLibraryClass::Play_2D_Sound("Login_Mouse_Over_Buttons", 2);
	}
}


// 0x5AEA60
void LoginInterface::Unknown_5AEA60()
{
	LoginTicketInfoClass info = Task->Ticket_Info;
	time_t created = info.Create_Time;
	struct tm today = *gmtime(&created);
	today.tm_hour = 0;
	today.tm_isdst = 0;
	today.tm_min = 0;
	today.tm_sec = 0;
	today.tm_wday = 0;
	today.tm_yday = 0;
	time_t now = mktime(&today);
	if (now != -1) {
		struct tm expiry;
		memset(&expiry, 0, sizeof(expiry));
		sscanf(info.Entitlement_Expiry.c_str(), "%d/%d/%d", &expiry.tm_mon, &expiry.tm_mday, &expiry.tm_year);
		expiry.tm_mon--;
		expiry.tm_year -= 1900;
		time_t expires = mktime(&expiry);
		if (expires != -1) {
			int days = (int)(difftime(expires, now) * (1.0 / 86400.0) - 1.0);
			if (days < 0) {
				days = 0;
			}
			if (days <= 7) {
				char text[1024];
				if (!Unknown_5FADE0()) {
					_snprintf(text, 1024, "It appears your Earth & Beyond account will expire on %s. Please use the \"Account Management\" menu to update your billing information or add a Game Time code.", info.Entitlement_Expiry.c_str());
				} else {
					char when[20];
					if (days == 0) {
						strcpy(when, "today");
					} else {
						_snprintf(when, 20, "in %d days", days);
					}
					_snprintf(text, 1024, "Your Trial of Earth & Beyond will expire %s. If you like Earth & Beyond and wish to continue your adventures, you can buy an Earth & Beyond Upgrade Registration Code at www.eabuddystore.com <http://www.eabuddystore.com>. Enter your new Registration Code by using the \"Account Management\" menu on the login screen and you'll be back in the galaxy in just minutes.\n\nThank you for playing Earth & Beyond!", when);
				}
				CharacterViewUnknown34Class *box = CharacterView->Unknown034;
				box->Unknown_588A70("OK_SMALL", 0, text, 320, 0, 0);
			}
		}
	}
}

/*
**	Reconstruction notes (app\Client\ui_login.cpp, the login screen: charview.cpp's 0x679160 makes it for step 1).
**
**	The class is LoginInterface (the client's RTTI: its FlashingGadgetEvent callback's type name 0xB817A8,
**	SignalBaseClass::MemberCallback<GadgetClass,LoginInterface,...>), 0xDC bytes over intrface.cpp's InterfaceClass
**	(the constructor calls 0x540A90 directly), tables 0xAF2BF0 / 0xAF2BE4 / 0xAF2BD8; its own slots are the
**	destructor, +0x60 (0x5AD540, every frame) and +0x68 (0x5AD610, the buttons). The object runs from the constructor
**	0x5ACB80 (after the adjustor thunks 0x5ACB60 / 0x5ACB70 of the file before) to the PermStringInitClass static at
**	0x5AF980 (permstr.h's); 0x5AF130-0x5AF970 are the settings map's template members, the callback's virtuals and
**	the adjustor thunks this file emits. Its asserts sit on the client's lines 713 ('!textptr', 0x5AE320, which
**	0x5AD610 and 0x5ADA50 inline), 787 / 826 ('AccountMenu') and 862 / 866 ('!event', '!gadget') (D25).
**
**	Every function of the window is here; the constructor is NOT EXACT (its note above it). 0x5ADA50 (slot +0x60's
**	first call: the welcome speech and the step changes) reads the view's and the task's members through inline
**	getters: they spend the inline budget, so the string's _Tidy and _Eos stay out of line as in the client.
**	Slot +0x68 takes its block order from if /
**	else plus break in the account cases (the shared box last); the account expiry box 0x5AEA60 nests its ifs (one
**	exit through the ticket copy's destructors) and its frame says LoginTicketInfoClass is 0xA0 bytes. Forms: the zero test over the five characters keeps its count in a named bool (0x5AE120); the account
**	menu's button loop indexes the name table (a jl pointer loop, 0x5AE590); the gadget's
**	mouse-over flag is a bitfield read through an inline getter (shr / test al, 1: 0x5AE9D0); the model's texts are
**	looked up through an inline Peek_Model (the font manager loaded before the model, 0x5AE160 / 0x5AE320).
*/
