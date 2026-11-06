// Earth & Beyond client (reconstruction): app\Client\ui_popupmsg.cpp. Notes at the end of the file.
#include <stdio.h>
#include "sclient.h"
#include "tasks\tsk_login.h"
#include "rendobj.h"
#include "layer.h"
#include "..\..\wwlib\ref_ptr.h"
#include "refcount.h"
#include "wwstring.h"
#include "..\shared\g_assert.h"
#include "..\shared\auxdata\permstr.h"

// gadget.cpp's gadget: gadget.h
#include "gadget.h"

// fontmanager.h's FontManagerClass at 0xBE67C4 and its text style; fontdatabase.h's FontStringFilesClass at 0xBE67BC
// fontmanager.h (with fontdatabase.h; its 0xBE67B8 is tsk_game.h's since P-H12-3)

#include "fontmanager.h"

extern FontManagerClass *Unknown_BE67C4;			// NAME 1 [Unknown_BE67C4]: 0xBE67C4

// unknown_675120.cpp's button gadget (the screen's seven buttons)
#include "unknown_675120.h"

// intrface.cpp's scope object (constructor 0x541980 over an interface, destructor 0x6E18B0)
#include "local_interface_sort.h"

// The character view: charview.h's CharacterViewClass
#include "charview.h"

void Unknown_496AE0(RenderObjClass *robj, bool make_geometry_unique);	// 0x496AE0 (ro_funcs.cpp, cdecl)  // NAME 1 [robj, make_geometry_unique]: as eff_o.cpp's
bool Unknown_496110(RenderObjClass *robj, float a, float b, float c);	// 0x496110 (ro_funcs.cpp, cdecl): a model's highlight  // NAME 1 [robj, a, b, c]: as ui_group.cpp's

// The asset manager (*0xBFAEE0)
#include "assetmgr.h"	// WW3DAssetManager: 0xBFAEE0 is WW3DAssetManager::TheInstance (its constructor 0x91C2C0 stores it)







// The control keys event (0x10 bytes) and InterfaceClass +0x4C, as ui_login.cpp declares them
struct PopupKeyEventStruct	// NAME 1: neutral (D10)
{
	PopupKeyEventStruct(short key) : Key(key), Unknown02(0), Unknown04(0.0f), Unknown08(0.0f), Unknown0C(0) {}	// NAME 1 [key]: neutral
	short Key;											// +0x00  // NAME 1: neutral (D10)
	short Unknown02;									// +0x02
	float Unknown04;									// +0x04
	float Unknown08;									// +0x08
	int Unknown0C;										// +0x0C
};
struct ControlKeyComboStruct; class ControlKeyListClass { public: void Add(const ControlKeyComboStruct &combo, void *handler); void Remove(const ControlKeyComboStruct &combo); };	// InterfaceClass +0x4C: control_keys_manager.h's key list (Add 0x721E60, Remove 0x721EF0), as far as this file calls it

// The settings store 0x7CA2D0 returns (tsk_game.h's Unknown_7CA2D0Class), as far as the site's link opens through it
class PopupSettingsClass { public: void Unknown_7CB2E0(const char *url, int a); };	// NAME 1: neutral (D10)  // NAME 1 [url, a]: as ui_login.cpp's

#include "ww3d.h"										// WW3D::Get_Frame_Time (0xBFAE3C / 0xBFAE40: WW3D::SyncTime / PreviousSyncTime, read inline)

#define NUM_SITES 6										// NAME 3: the G_ASSERT's

// The end screen (0xD4 bytes, tables 0xAF5B70 / 0xAF5B64 / 0xAF5B58; charview.cpp's 0x679160 makes it for mode 0)
class Unknown5DB670InterfaceClass : public InterfaceClass	// NAME 1: neutral (D10): charview.cpp's
{
public:
	Unknown5DB670InterfaceClass(UIViewClass *view, LayerClass *layer);	// 0x5DB670  // NAME 1 [view, layer]: as LoginInterface's
	virtual ~Unknown5DB670InterfaceClass();				// 0x5DB9F0 (slot 0x5DB9C0)
	virtual void Unknown_60();							// 0x5DBB70 (every frame: the 30 second count down)
	virtual bool Unknown_68(int key);					// 0x5DBD00 (the buttons: 1-6 a site, 7 exit)
	virtual void Unknown_7C(UIViewClass *view, bool arg);	// 0x5DBB00 (the model off the old view's scene, onto the new)
	void Unknown_5DBD50(int site);						// 0x5DBD50: the site's page opened  // NAME 1 [site]: the G_ASSERT's
	const char *Get_Site_URL(int site);					// 0x5DBEB0 (G_ASSERT 'site >= 0 && site < NUM_SITES')  // NAME 1: what it returns; NAME 3 [site]: the G_ASSERT's

	LayerClass Layer;									// +0x84 (a copy of the constructor's layer)  // NAME 1: what it holds
	LoginTaskClass *Task;								// +0xA8 (the character view's +0x14's slot +0x08)  // NAME 1: as LoginInterface's
	CharacterViewClass *CharacterView;					// +0xAC (the view's slot +0x28)  // NAME 1: as LoginInterface's
	RefCountPtr<RenderObjClass> Model;					// +0xB0 ('end_screen00')  // NAME 1: what it holds
	float Unknown0B4;									// +0xB4 (seconds shown; -1 before the first frame)
	GadgetClass *Buttons[7];							// +0xB8 ('logo00' .. 'exit')  // NAME 1: neutral (D10)
};

static const char *Unknown_B846BC[] = { "logo00", "logo01", "logo02", "logo03", "logo04", "ebweb", "exit" };	// NAME 1: neutral (D10)
static const char *Unknown_B846DC[NUM_SITES] = {	// NAME 1: neutral (D10)
	"http://www.ebgames.com/ebx/categories/products/product.asp?pf_id=224120",
	"http://www.gamestop.com/product.asp?product%5Fid=644886",
	"http://www.amazon.com/exec/obidos/ASIN/B000067O0P/qid=1027574831/sr=8-154/ref=sr_8_154/103-3882812-1780658",
	"http://www.enb-emulator.com",
	"http://www.enb-emulator.com",
	"http://www.enb-emulator.com"
};

// 0x5DB670
Unknown5DB670InterfaceClass::Unknown5DB670InterfaceClass(UIViewClass *view, LayerClass *layer) :
	InterfaceClass(view, (int)layer->Peek_Scene()),
	Layer(*layer),
	Task(NULL),
	CharacterView(NULL),
	Model(NULL),
	Unknown0B4(-1.0f)
{
	InterfaceSortScopeClass scope(this);
	CharacterView = (CharacterViewClass *)View->Unknown_28();
	Task = (LoginTaskClass *)CharacterView->Unknown14->Unknown_08();
	CharacterView->Unknown_67A250();
	Model = Create_Get(WW3DAssetManager::Get_Instance()->Create_Render_Obj("end_screen00"));
	Model->Add(Layer.Peek_Scene());
	Unknown_BE67BC->Load("end_screen00.ini");
	Unknown_BE67C4->Add(Model);
	for (int i = 0; i < 7; i++) {
		if (Unknown_B846BC[i]) {
			RefCountPtr<RenderObjClass> robj = Create_Get(Model->Get_Sub_Object_By_Name(Unknown_B846BC[i]));
			Unknown_496AE0(robj.Peek(), 0);
			Unknown_496110(robj.Peek(), 0.85f, 0.85f, 0.85f);
			Buttons[i] = new ButtonGadgetClass(robj.Peek(), Layer, 5, 3);
			Unknown64.Unknown_6870E0(Buttons[i], i + 1, View, 1, 0);
		}
	}
	((ControlKeyListClass *)Unknown4C)->Add(*(const ControlKeyComboStruct *)&PopupKeyEventStruct(0x1B), (void *)7);
}

// 0x5DB9F0
Unknown5DB670InterfaceClass::~Unknown5DB670InterfaceClass()
{
	Unknown_5C();
	Unknown_BE67C4->Unknown_6D2310(Model);
	Unknown_BE67BC->Unload("end_screen00.ini");
	Model = NULL;
}

// 0x5DBB00
void Unknown5DB670InterfaceClass::Unknown_7C(UIViewClass *view, bool arg)
{
	if (View) {
		Model->Remove();
	}
	InterfaceClass::Unknown_7C(view, arg);
	if (View) {
		Model->Add(Layer.Peek_Scene());
	}
}

// 0x5DBB70
void Unknown5DB670InterfaceClass::Unknown_60()
{
	float elapsed = WW3D::Get_Frame_Time() * 0.001f;
	if (Unknown0B4 < 0.0f) {
		Unknown0B4 = 0.0f;
	} else {
		if (elapsed >= 0.5f) {
			elapsed = 0.5f;
		}
		Unknown0B4 += elapsed;
		if (Unknown0B4 > 30.0f) {
			Task->Unknown_7693E0();
		}
	}
	char text[64];
	_snprintf(text, 64, "Exit in %d sec", (int)(30.0f - Unknown0B4 + 0.5));
	RefCountPtr<FontSys> style = Create_Peek(Unknown_BE67C4->Get_Style("EXIT_end_screen00", Model.Peek()));
	style->Change_Text(text, 0);
}

// 0x5DBD00
bool Unknown5DB670InterfaceClass::Unknown_68(int key)
{
	switch (key) {
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
			Unknown_5DBD50(key - 1);
		case 7:
			Task->Unknown_7693E0();
			return(true);
	}
	return(false);
}

// 0x5DBD50
void Unknown5DB670InterfaceClass::Unknown_5DBD50(int site)
{
	StringClass url = Get_Site_URL(site);
	if (!url.Is_Empty()) {
		((PopupSettingsClass *)Unknown_7CA2D0())->Unknown_7CB2E0(url, 0);
	}
}



// 0x5DBEB0 (inlined in 0x5DBD50 before it: /Ob2 inlines functions defined later in the file)
const char *Unknown5DB670InterfaceClass::Get_Site_URL(int site)
{
	G_ASSERT(site >= 0 && site < NUM_SITES);
	if (site >= 0 && site < NUM_SITES) {
		return(Unknown_B846DC[site]);
	}
	return("");
}

/*
**	Reconstruction notes (app\Client\ui_popupmsg.cpp, the end screen: charview.cpp's 0x679160 makes it for mode 0,
**	CHARACTER_POPUP_MESSAGE).
**
**	The class has no RTTI (no callback type names it): Unknown5DB670InterfaceClass, charview.cpp's neutral name, 0xD4
**	bytes over intrface.cpp's InterfaceClass (the constructor calls 0x540A90 directly), tables 0xAF5B70 / 0xAF5B64 /
**	0xAF5B58; its own slots are the destructor, +0x60 (0x5DBB70, every frame: 'Exit in %d sec', the exit after 30
**	seconds), +0x68 (0x5DBD00, the buttons) and +0x7C (0x5DBB00). The screen loads 'end_screen00' with its seven
**	buttons ('logo00' .. 'logo04', 'ebweb', 'exit'; 1-6 open a site's page through the settings store, 7 exits).
**	The object runs from the constructor 0x5DB670 (after ui_popup_browser.cpp's window) through the adjustor thunks
**	0x5DBF50 / 0x5DBF60 to the PermStringInitClass static at 0x5DBF70 (permstr.h's). Its one assert sits on the
**	client's line 196 ('site >= 0 && site < NUM_SITES', 0x5DBEB0, which 0x5DBD50 inlines) (D25).
**
**	Every function of the window is here and exact. Forms: slot +0x68 is a switch whose cases 1-6 fall through into
**	7's exit (if / else forms branch the other way round); the site's URL is copied into a StringClass (its
**	Is_Empty against the null char); the frame's text style is held through Create_Peek (0x546510, out of line).
**	The class is declared here only; charview.cpp declares the constructor's view of it (D137: no header yet).
*/
