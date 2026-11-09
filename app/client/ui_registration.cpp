// Earth & Beyond client (reconstruction): app\Client\ui_registration.cpp. Notes at the end of the file.
#include "..\..\wwlib\registry.h"
#include "..\..\wwlib\ini.h"
#include "fontmanager.h"
#include "sclient.h"
#include "tasks\tsk_login.h"
#include "charview.h"
#include "avatar.h"
#include "ui_combobox.h"
#include "textgadg.h"
#include <map>
#include <vector>
#include <algorithm>
#include <stdlib.h>
#include "ww3d.h"
#include "..\shared\g_assert.h"
#include <stdio.h>
#include <stdarg.h>
#include <string>

#include <time.h>
#include <string.h>
#include "..\..\wwlib\ref_ptr.h"
#include "layer.h"
#include "assetmgr.h"
#include "hanim.h"
#include "texture.h"
#include "dx8renderer.h"
#include "gd_robj.h"
#include "unknown_66b770.h"
#include "unknown_675120.h"
#include "fontdatabase.h"
#include "fontsys_cursor.h"
#include "local_interface_sort.h"
#include "tutorial_manager.h"
#define __UI_REGISTRATION_MEMBERS
#include "ui_registration.h"
extern FontManagerClass *Unknown_BE67C4;  // NAME 1: 0xBE67C4

// What the text gadgets get by field (their dynamic initializers open the object, 0x5EED20-0x5EF0A0), and the characters they also allow (client .data 0xB85ECC)
extern const int Unknown_B08C90;  // NAME 1 [Unknown_B08C90]: 0xB08C90 (textgadg.h's flags)
extern const int Unknown_B08C94;  // NAME 1 [Unknown_B08C94]: 0xB08C94 (textgadg.h's flags)
extern const int Unknown_B08C98;  // NAME 1 [Unknown_B08C98]: 0xB08C98 (textgadg.h's flags)
extern const int Unknown_B08C9C;  // NAME 1 [Unknown_B08C9C]: 0xB08C9C (textgadg.h's flags)
extern const int Unknown_B08CA0;  // NAME 1 [Unknown_B08CA0]: 0xB08CA0 (textgadg.h's flags)
extern const int Unknown_B08CB4;  // NAME 1 [Unknown_B08CB4]: 0xB08CB4 (textgadg.h's flags)
static const int Unknown_BE5DC8 = Unknown_B08CB4 | Unknown_B08CB8 | Unknown_B08CA0 | Unknown_B08CAC;  // NAME 1 [Unknown_BE5DC8]: 0xBE5DC8
static const int Unknown_BE5DC4 = Unknown_B08CB8 | Unknown_B08CA0 | Unknown_B08CAC;  // NAME 1 [Unknown_BE5DC4]: 0xBE5DC4
static const int Unknown_BE5DE4 = Unknown_B08CB8 | Unknown_B08CB4 | Unknown_B08CA0 | Unknown_B08CB0;  // NAME 1 [Unknown_BE5DE4]: 0xBE5DE4
static const int Unknown_BE5DCC = Unknown_B08C98 | Unknown_B08CB0;  // NAME 1 [Unknown_BE5DCC]: 0xBE5DCC
static const int Unknown_BE5DA8 = Unknown_B08CB8 | Unknown_B08CB4 | Unknown_B08C9C | Unknown_B08CB0;  // NAME 1 [Unknown_BE5DA8]: 0xBE5DA8
static const int Unknown_BE5DB4 = Unknown_B08C90 | Unknown_B08C9C | Unknown_B08CB0;  // NAME 1 [Unknown_BE5DB4]: 0xBE5DB4
static const int Unknown_BE5DA4 = Unknown_B08C9C | Unknown_B08CB0;  // NAME 1 [Unknown_BE5DA4]: 0xBE5DA4
static const int Unknown_BE5DBC = Unknown_B08C90 | Unknown_B08CB8 | Unknown_B08C98 | Unknown_B08CB0;  // NAME 1 [Unknown_BE5DBC]: 0xBE5DBC
static const int Unknown_BE5DD4 = Unknown_B08C90 | Unknown_B08CB8 | Unknown_B08CA0 | Unknown_B08CB0;  // NAME 1 [Unknown_BE5DD4]: 0xBE5DD4
static const int Unknown_BE5DD8 = Unknown_B08CB8 | Unknown_B08CB4 | Unknown_B08CA0 | Unknown_B08CB0;  // NAME 1 [Unknown_BE5DD8]: 0xBE5DD8
static const int Unknown_BE5DB8 = Unknown_B08C90 | Unknown_B08CA0 | Unknown_B08CAC;  // NAME 1 [Unknown_BE5DB8]: 0xBE5DB8
static const int Unknown_BE5DAC = Unknown_B08CB8 | Unknown_B08C94 | Unknown_B08CA0 | Unknown_B08CB0;  // NAME 1 [Unknown_BE5DAC]: 0xBE5DAC
static const int Unknown_BE5DD0 = Unknown_B08C98 | Unknown_B08CB0;  // NAME 1 [Unknown_BE5DD0]: 0xBE5DD0
static const int Unknown_BE5DC0 = Unknown_B08C90 | Unknown_B08CA0 | Unknown_B08CB0;  // NAME 1 [Unknown_BE5DC0]: 0xBE5DC0
static const int Unknown_BE5DB0 = Unknown_B08C98 | Unknown_B08CB0;  // NAME 1 [Unknown_BE5DB0]: 0xBE5DB0
static const int Unknown_BE5DE0 = Unknown_BE5DC8;  // NAME 1 [Unknown_BE5DE0]: 0xBE5DE0
static const int Unknown_BE5DDC = (Unknown_BE5DAC & ~Unknown_B08CB0) | Unknown_B08CAC;  // NAME 1 [Unknown_BE5DDC]: 0xBE5DDC
static const char *Unknown_B85ECC = "()-";  // NAME 1 [Unknown_B85ECC]: client .data 0xB85ECC
static const char *Unknown_B85ED0 = "@.-_";  // NAME 1 [Unknown_B85ED0]: client .data 0xB85ED0
static const char *Unknown_B85ED4 = "#.-";  // NAME 1 [Unknown_B85ED4]: client .data 0xB85ED4
static const char *Unknown_B85ED8 = "-";  // NAME 1 [Unknown_B85ED8]: client .data 0xB85ED8
static const char *Unknown_B85EDC = ",";  // NAME 1 [Unknown_B85EDC]: client .data 0xB85EDC

static const char *ProductEB = "EB";  // NAME 1 [ProductEB]: client .data 0xB85EE0 (0x5FA0B0 compares +0x10 with it)
static const char *ProductTrial = "EB-TRIAL";  // NAME 1 [ProductTrial]: client .data 0xB85EF4 (0x5FA0B0)
static const char *RegDataFile = "RG_REGDATA.INI";  // NAME 1 [RegDataFile]: client .data 0xB85EF8 (0x5F8FD0's INIClass)
static const char *RegistrationKey = "Software\\Westwood Studios\\Earth and Beyond\\Registration";  // NAME 1 [RegistrationKey]: client .data 0xB85EFC
static const char *RegistrationServerError = "Could not communicate with registration server.\nPlease try again later.";  // NAME 1 [RegistrationServerError]: client .data 0xB85F00

// The screen's pages by state (client .data 0xB85F08: the model, its font file)
struct RegistrationPageStruct  // NAME 1 [RegistrationPageStruct]: neutral (D10)
{
	const char *Name;			// NAME 1 [Name]: 'RG_A1'
	const char *Ini;			// NAME 1 [Ini]: 'rg_a1.ini'
};
static const RegistrationPageStruct RegistrationPages[] = {  // NAME 1 [RegistrationPages]: client .data 0xB85F08
	{ "RG_A1", "rg_a1.ini" },
	{ "RG_A2", "rg_a2.ini" },
	{ "RG_A3", "rg_a3.ini" },
	{ "RG_A5", "rg_a5.ini" },
	{ "RG_A6", "rg_a6.ini" },
	{ "RG_A7", "rg_a7.ini" },
	{ "RG_A10", "rg_a10.ini" },
	{ "RG_A8", "rg_a8.ini" },
	{ "RG_A9", "rg_a9.ini" },
	{ "RG_B1", "rg_b1.ini" },
	{ "RG_C1", "rg_c1.ini" },
	{ "RG_D1", "rg_D1.ini" },
	{ "RG_D3", "rg_D3.ini" },
	{ "RG_D4", "rg_D4.ini" },
	{ "RG_F1", "rg_F1.ini" },
	{ "RG_C2", "rg_c2.ini" },
	{ "RG_C3", "rg_c3.ini" },
	{ "RG_E1", "rg_e1.ini" },
	{ "RG_G1", "rg_G1.ini" },
	{ "RG_I1", "rg_I1.ini" },
	{ "RG_K1", "rg_K1.ini" },
	{ "RG_X2", "rg_X2.ini" },
	{ "RG_X1", "rg_X1.ini" },
	{ "RG_X3", "rg_X3.ini" },
	{ "None", "None" },
};

static const char *RegistrationMonths[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec", 0 };  // NAME 1 [RegistrationMonths]: client .data 0xB85FD0 (combo box 0x14)

static const char *StatusStrings[] = {  // NAME 1 [StatusStrings]: client .data 0xB86004
	"Please Wait...",
	"Validating Login Information...",
	"Retrieving Entitlement Status...",
	"Retrieving Billing Information...",
	"Retrieving Subscription Information...",
	"Retrieving Pricing Options...",
	"Retrieving Credit Card List...",
	"Retrieving GameTime Data...",
	"Creating EA Account...",
	"Transmitting Billing Information...",
	"Applying Registration Code...",
	"Applying GameTime Code...",
	"Validating AOL Login Information...",
	"Requesting Member Name...",
	"Requesting Member Password...",
	"Your Account has expired...",
	"Your Account has expired...",
	0
};

static const char *PaymentTypeNames[] = {  // NAME 1 [PaymentTypeNames]: client .data 0xB86048 (0x5F9EB0's G_ASSERT 'typetext')
	0,
	0,
	"Credit Card",
	"GameTime",
	0,
	"Check",
	0
};

static const char *PaymentTypeTexts[] = {  // NAME 1 [PaymentTypeTexts]: client .data 0xB86064 (the descriptions, as 0xB86048's order)
	0,
	0,
	"EA accepts most major US and International Credit Cards, including: Visa, MasterCard, Discover and American Express.",
	"GameTime is a paid block of time that allows you to play Earth & Beyond for a fixed period of time.  You need a GameTime code to use this option.",
	0,
	"EA accepts personal checks for payment for your Earth & Beyond account.  To pay by check, you must contact a customer service representative.",
	0
};

static const int DaysInMonth[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };  // NAME 1 [DaysInMonth]: client .rdata 0xAF6F58 (0x5F9460)

extern int Unknown_BE8AD4;  // NAME 1 [Unknown_BE8AD4]: 0xBE8AD4 (0x7CF190 zeroes it; the registration requests' last argument)
void __cdecl Unknown_5FAE90(RegistrationInterface *self, const char *format, ...);
extern UIComboBoxClass *Unknown_BE5BC4;	// NAME 1: neutral (D10): the open combo box (ui_combobox.cpp)
extern FontStringFilesClass *Unknown_BE67BC;  // NAME 1: 0xBE67BC (fontdatabase.h's)
extern const int Unknown_B08CBC;  // NAME 1 [Unknown_B08CBC]: 0x800, what 0x5F2D30 adds to the text gadgets' flags
extern const int Unknown_B08CC0;  // NAME 1 [Unknown_B08CC0]: 0x1000
void Unknown_496AE0(RenderObjClass *robj, bool make_geometry_unique);
void Unknown_Debug_Printf(const char *format, ...);
class RegistrationSettingsClass { public: void Unknown_7CB240(int a); void Unknown_7CB2E0(const char *url, int a); };  // NAME 1 [RegistrationSettingsClass, a, url]: as ui_login.cpp's

static const char *RegistrationSubscribeURL = "https://jump-subs.ea.com/SubscribeEntry.jsp?prodID=REG-EB&iPath=3&CancelURL=http%3A%2F%2Fwww.earthandbeyond.ea.com%2F";  // NAME 1 [RegistrationSubscribeURL]: the literal 0xB8139C (0x5F2280)

struct RegistrationKeyStruct  // NAME 1: neutral (D10): 0x10 bytes, 0x721E60's first argument
{
	RegistrationKeyStruct(unsigned short key) : Key(key), Modifiers(0), Unknown04(0), Unknown08(0), Unknown0C(0) {}  // NAME 1 [key]: the key's code
	unsigned short Key;			// NAME 1 [Key]: the key's code
	unsigned short Modifiers;	// NAME 1 [Modifiers]: as ControlKeyComboStruct's
	int Unknown04;
	int Unknown08;
	int Unknown0C;
};
class RegistrationKeyListClass { public: void Add(const RegistrationKeyStruct &combo, void *handler); };  // NAME 1: neutral (D10): InterfaceClass +0x4C (ui_login.cpp declares it likewise) PLACEHOLDER

class RegistrationFontClass : public FontSys { public: void Unknown_6D5EF0(bool on); };  // NAME 1 [RegistrationFontClass, on]: neutral (D10): 0x6D5EF0 (0x5F8DB0)
static const int CardDoubled[10] = { 0, 2, 4, 6, 8, 1, 3, 5, 7, 9 };  // NAME 1 [CardDoubled]: client .rdata 0xAF6F88 (0x603820: a digit doubled, Luhn)

static inline RegistrationUnknown1ACClass *Get_Unknown1AC()  // NAME 1 [Get_Unknown1AC]: 0xBDA960, inlined in 0x5EF0B0
{
	static RegistrationUnknown1ACClass instance;
	return(&instance);
}



































































































// @0x5EF0B0
RegistrationInterface::RegistrationInterface(UIViewClass *view, LayerClass *layer, int mode) :
	InterfaceClass(view, (int)layer->Peek_Scene()),
	Unknown98(""),
	Unknown9C(""),
	UnknownA0(""),
	UnknownBC(NULL),
	UnknownC0(NULL),
	UnknownC4(false),
	UnknownC5(false),
	UnknownC6(false),
	UnknownC7(false),
	UnknownC8(false),
	UnknownC9(false),
	UnknownCA(false),
	UnknownCC(0),
	Layer(layer),
	UnknownD4(mode),
	Unknown118(false),
	Unknown119(false),
	Unknown11A(false),
	Unknown11B(false),
	Unknown11D(false),
	Unknown11E(false),
	Unknown148(0),
	Unknown1AC(Get_Unknown1AC()),
	StatusIndex(0),
	Unknown1C4(0),
	Unknown1C8(true),
	Unknown1C9(false),
	Unknown1CA(false),
	Unknown1CB(false),
	Unknown1CC(false),
	Unknown1CD(false),
	Unknown1CE(false),
	Unknown1CF(false),
	Unknown1D0(false),
	Unknown1D1(false),
	Unknown1D2(false),
	Unknown1D3(false),
	Unknown1D4(false),
	Unknown1D5(false),
	Unknown1D8(""),
	Unknown1E8(""),
	Unknown208(""),
	Unknown228(""),
	Unknown268(2),
	Unknown26C(8),
	Unknown384(0),
	Unknown388(0),
	Unknown38C(2)
	// +0x560 / +0x564 are Unknown478.UnknownE8 / UnknownEC: its constructor zeroes them

{
	if (mode == 3) {
		UnknownD4 = 0x17;
		Unknown11D = true;
		Unknown1C8 = false;
		Unknown1C9 = false;
	} else if (mode == 0x14) {
		UnknownD4 = 1;
		Unknown11B = true;
	} else {
		UnknownD4 = 0x17;
		Unknown11B = false;
		Unknown11C = false;
		Unknown11D = false;
	}
	UnknownBC = (CharacterViewClass *)View->Unknown_28();
	UnknownC0 = (LoginTaskClass *)UnknownBC->Unknown14->Unknown_08();
	InterfaceSortScopeClass scope(this);
	UnknownBC->Unknown_67AB20(3, "comp00a");
	UnknownBC->Unknown_67AB20(4, "FE_NAR_AGG");
	UnknownBC->Unknown_67A6C0();
	((RegistrationKeyListClass *)Unknown4C)->Add(RegistrationKeyStruct(0x1B), (void *)0x2D);
	((RegistrationKeyListClass *)Unknown4C)->Add(RegistrationKeyStruct(0x0D), (void *)6);
	Unknown_5F8FD0();
	Unknown1AC->Unknown_7CEF30(&Sub14C, Unknown98, Unknown9C, UnknownA0, this);
	Unknown1B8 = Create_Get(WW3DAssetManager::Get_Instance()->Get_Texture("RG_Radio_off.tga", (MipCountType)0, (WW3DFormat)0, true));
	Unknown1B4 = Create_Get(WW3DAssetManager::Get_Instance()->Get_Texture("RG_Radio_on.tga", (MipCountType)0, (WW3DFormat)0, true));
	Unknown3C4.Unknown90 = true;
	Unknown3C4.Unknown91 = true;
	time_t now = time(NULL);
	struct tm *today = gmtime(&now);
	G_ASSERT(today);
	Unknown124 = *today;
	Unknown3C4.Unknown7E = (short)(Unknown124.tm_year + 1900);
	Unknown3C4.Unknown7C = (char)Unknown124.tm_mday;
	Unknown3C4.Unknown7D = (char)(Unknown124.tm_mon + 1);
	UnknownBC->Unknown_67AB20(0, "RG_FRAME");
	UnknownBC->Unknown_67B4F0(0);
	UnknownD8.push_back(UnknownD4);
	Unknown_5F3770(UnknownD4);
	TheDX8MeshRenderer.Invalidate();
}

// @0x5F0F10
RegistrationInterface::~RegistrationInterface()
{
	Unknown1B8 = NULL;
	Unknown1B4 = NULL;
	RegistrationUnknown1ACClass::Unknown_7CF190();
	Unknown_5C();
	InterfaceClass::Unknown_5C();
	Unknown_5F3600();
	UnknownBC->Unknown_67A670();
	UnknownBC->Unknown_67A9B0(0);
	UnknownBC->Unknown_67A9B0(3);
	WW3DAssetManager::Get_Instance()->Free_Assets();
	UnknownC0->Unknown0018->Unknown_515400();
}

// @0x5F1660
void RegistrationInterface::Unknown_5F1660()
{
	Unknown1B8 = NULL;
	Unknown1B4 = NULL;
	RegistrationUnknown1ACClass::Unknown_7CF190();
	Unknown_5C();
	InterfaceClass::Unknown_5C();
	Unknown_5F3600();
	UnknownBC->Unknown_67A670();
	UnknownBC->Unknown_67A9B0(0);
	UnknownBC->Unknown_67A9B0(3);
	WW3DAssetManager::Get_Instance()->Free_Assets();
	UnknownC0->Unknown0018->Unknown_515400();
}

// @0x5F1730
void RegistrationInterface::Unknown_60()
{
	Unknown_5F2280();
	for (std::map<int, RegistrationTextStruct>::iterator text = UnknownF8.begin(); text != UnknownF8.end(); text++) {
		(*text).second.Gadget->Unknown_6AAD10();
	}
	std::map<int, RegistrationComboStruct>::iterator combo = Unknown108.begin();
	while (combo != Unknown108.end()) {
		(*combo).second.Combo->Unknown_60();
		combo++;
	}
	if (UnknownD4 == 0x16) {
		float frames = (float)Unknown1C0->Get_Num_Frames();
		float frame = Unknown1BC->Get_Frame(0);
		float seconds = WW3D::Get_Frame_Time() * 0.001f;
		frame = seconds * 30.0f + frame;
		if (frame > frames) {
			frame = 0.0f;
		}
		Unknown1BC->Set_Frame(0, frame);
		DialogRenderObj->Update_Sub_Object_Transforms();
	}
	if (Unknown1C4 != 0 && --Unknown1C4 == 0) {
		int width;
		int height;
		int bits;
		bool windowed;
		WW3D::Get_Device_Resolution(width, height, bits, windowed);
		RenderObjClass *label = DialogRenderObj->Get_Sub_Object_By_Name("LABEL_TEXT");
		Unknown_7CA2D0()->Unknown_7CA5C0(4, label, NULL);
		UnknownC0->Unknown0018->Unknown_5153B0();
		label->Release_Ref();
	}
}

// @0x5F1940
void RegistrationInterface::Unknown_5F1940()
{
	if (UnknownD4 == 0x16) {
		float frames = (float)Unknown1C0->Get_Num_Frames();
		float frame = Unknown1BC->Get_Frame(0);
		float seconds = WW3D::Get_Frame_Time() * 0.001f;
		frame = seconds * 30.0f + frame;
		if (frame > frames) {
			frame = 0.0f;
		}
		Unknown1BC->Set_Frame(0, frame);
		DialogRenderObj->Update_Sub_Object_Transforms();
	}
}

// @0x5F1A10
bool RegistrationInterface::Unknown_68(int key)
{
	UIComboBoxClass *open = Unknown_BE5BC4;
	if (UnknownC5 || !Active) {
		return false;
	}
	switch (key) {
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 8:
		case 9:
		case 0xD:
		case 0xE:
		case 0xF:
		case 0x10:
		case 0x11:
		case 0x12:
		case 0x17:
		case 0x18:
			Unknown_5F3470(UnknownE8[key].Group);
			UnknownE8[key].Gadget->Unknown_90(Unknown1B4.Peek());
			UnknownE8[key].Selected = true;
			return false;
		case 6:
			Unknown_5FBD30();
			return true;
		case 7:
			if ((UnknownD4 == 1 && Unknown11B) || (UnknownD4 == 0 && Unknown11D) || UnknownD4 == 0x17) {
				Unknown_5F86F0();
				return true;
			} else {
				int state = Unknown_5F8A90();
				if (UnknownD8.size() != 0) {
					UnknownD8.pop_back();
				}
				Unknown_5F3770(state);
				return true;
			}
		case 10:
		case 0xB:
		case 0xC:
		case 0x13:
		case 0x1A:
		case 0x1B:
		case 0x1C:
		case 0x1D:
		case 0x1E:
		case 0x1F:
		case 0x20:
		case 0x21:
		case 0x23:
		case 0x24:
		case 0x25:
		case 0x26:
		case 0x2B:
			if (Unknown148 != key && Unknown148 != 0) {
				UnknownF8[Unknown148].Gadget->Unknown_74(false);
			}
			if (key != 0) {
				UnknownF8[key].Gadget->Unknown_74(true);
			}
			Unknown148 = key;
			return true;
		case 0x14:
		case 0x16:
			if (UnknownD4 == 10) {
				Unknown_5F9460();
				return false;
			}
			break;
		case 0x29:
			{
				Unknown384 = Unknown108[0x29].Combo->Selected;
				RegistrationUnknown578Struct &entry = Unknown578[Unknown384];
				Unknown270.Unknown00 = entry.Unknown00;
				Unknown270.Unknown10 = entry.Unknown10;
				Unknown270.Unknown20 = entry.Unknown20;
				Unknown270.Unknown24 = entry.Unknown24;
				Unknown270.Unknown34 = entry.Unknown34;
				Unknown270.Unknown44 = entry.Unknown44;
				Unknown270.Unknown48 = entry.Unknown48;
				Unknown270.Unknown4C = entry.Unknown4C;
				const char *text = Unknown578[Unknown384].Unknown34.c_str();
				ManagedFontClass *font = Unknown_BE67C4->Find(DialogRenderObj.Peek());
				if (font) {
					font->Unknown_6D0A70("WRAP_LABEL_DESC_RG_F1", text);
					return false;
				}
			}
			break;
		case 0x2A:
			{
				Unknown388 = Unknown108[0x2A].Combo->Selected;
				Unknown38C = Unknown598[Unknown388];
				const char *text = PaymentTypeTexts[Unknown38C];
				ManagedFontClass *font = Unknown_BE67C4->Find(DialogRenderObj.Peek());
				if (font) {
					font->Unknown_6D0A70("WRAP_EXPLAIN_TEXT_RG_D1", text);
					return false;
				}
			}
			break;
		case 0x2D:
			if (Unknown11B || Unknown11D) {
				Unknown_5F2990(false);
				Unknown_5F2A80(false, NULL);
				UnknownCA = true;
				UnknownC7 = true;
				UnknownBC->Get_Unknown198()->Unknown_784E00();
				UnknownBC->Unknown_67A8D0(true);
				UnknownC0->Go_Next_Step();
			} else if (UnknownD4 != 0x16 && UnknownD4 != 0x15 && !UnknownC6 && !UnknownC7) {
				UnknownC5 = false;
				Unknown119 = true;
				UnknownBC->Get_Unknown34()->Unknown_588A70("YES_NO", 0, "You will not be able to play\nEarth & Beyond\nuntil you complete the\nregistration process.\n\nAre you sure you want to quit the registration process?", 0x140, 0, 0);
				return true;
			}
			return true;
		case 0x2E:
			UnknownD8.push_back(UnknownD4);
			Unknown_5F3770(0x15);
			return false;
		case 0x2F:
			if (!UnknownF8[Unknown148].Gadget->Active) {
				TextBaseGadgetClass *gadget = UnknownF8[Unknown148].Gadget;
				if (gadget->Unknown39) {
					gadget->Unknown39 = false;
					if (UnknownF8[Unknown148].Unknown10 != 0) {
						UnknownF8[Unknown148].Gadget->Unknown_74(false);
						Unknown_5F8CD0(UnknownF8[Unknown148].Unknown10, true);
						return true;
					}
					Unknown_5F8CD0(Unknown148, false);
				}
			}
			return true;
		case 0x30:
			if (open == NULL) {
				Unknown_5F2990(true);
				for (std::map<int, RegistrationComboStruct>::iterator combo = Unknown108.begin(); combo != Unknown108.end(); combo++) {
					if ((*combo).second.Combo) {
						(*combo).second.Combo->Unknown_57B3C0(1);
					}
				}
			} else {
				if (Unknown148 != 0) {
					UnknownF8[Unknown148].Gadget->Unknown_74(false);
				}
				Unknown148 = 0;
				Unknown_5F2990(false);
				for (std::map<int, RegistrationComboStruct>::iterator combo = Unknown108.begin(); combo != Unknown108.end(); combo++) {
					if ((*combo).second.Combo != open) {
						(*combo).second.Combo->Unknown_57B3C0(0);
					}
				}
			}
			break;
	}
	return false;
}

// @0x5F2280
void RegistrationInterface::Unknown_5F2280()
{
	Unknown_5FB0C0();
	if (UnknownD4 == 0xC || UnknownD4 == 0xD) {
		TextBaseGadgetClass *gadget = UnknownF8[0x23].Gadget;
		bool changed = gadget->Changed;
		gadget->Changed = false;
		if (changed) {
			if (gadget->Active) {
				char *text = gadget->Get_Text();
				std::vector<RegistrationCardPrefixStruct>::iterator prefix = Unknown88.begin();
				for (; prefix != Unknown88.end(); prefix++) {
					if ((*prefix).Is_Prefix_Of(text)) {
						break;
					}
				}
				if (prefix != Unknown88.end() && (*prefix).Id != 0) {
					std::vector<RegistrationUnknown5A8Struct>::iterator entry = Unknown5A8.begin();
					for (; entry != Unknown5A8.end(); entry++) {
						if ((*entry).Unknown00 == (*prefix).Id) {
							break;
						}
					}
					if (entry != Unknown5A8.end()) {
						Unknown108[0x22].Combo->Unknown_57AC70(entry - Unknown5A8.begin());
					}
				}
				delete[] text;
			}
			gadget->Changed = true;
		}
	}
	if (UnknownBC->Get_Unknown198()->Is_Unknown220() && UnknownC6) {
		UnknownC6 = false;
		UnknownC5 = false;
		Unknown_5F2990(true);
		for (std::map<int, RegistrationComboStruct>::iterator combo = Unknown108.begin(); combo != Unknown108.end(); combo++) {
			if ((*combo).second.Combo) {
				(*combo).second.Combo->Unknown_57B3C0(1);
			}
		}
	}
	if (UnknownCA && UnknownBC->Get_Unknown198()->Unknown_7863E0()) {
		UnknownCA = false;
		if (UnknownC7) {
			UnknownC7 = false;
			UnknownC0->Go_Next_Step();
		}
	}
	if (Unknown119) {
		int button = UnknownBC->Get_Unknown34()->Unknown_5890B0("YES_NO");
		if (button != 0) {
			if (button == 1) {
				UnknownBC->Get_Unknown34()->Unknown_5C();
				Unknown119 = false;
				Unknown_5F1660();
				UnknownC0->Unknown_6C();
			} else {
				UnknownBC->Get_Unknown34()->Unknown_5C();
				Unknown119 = false;
			}
		}
		button = UnknownBC->Get_Unknown34()->Unknown_5890B0("OK");
		if (button != 0) {
			UnknownBC->Get_Unknown34()->Unknown_5C();
			Unknown119 = false;
			Unknown_5F1660();
			UnknownC0->Unknown_6C();
		}
	}
	if (Unknown11A) {
		int button = UnknownBC->Get_Unknown34()->Unknown_5890B0("YES_NO");
		if (button != 0) {
			if (button == 1) {
				UnknownBC->Get_Unknown34()->Unknown_5C();
				((RegistrationSettingsClass *)Unknown_7CA2D0())->Unknown_7CB2E0(RegistrationSubscribeURL, 1);
				Unknown11A = false;
				UnknownC0->Unknown_6C();
			} else {
				UnknownBC->Get_Unknown34()->Unknown_5C();
				Unknown11A = false;
			}
		}
	}
	bool shown = UnknownBC->Get_Unknown34()->Is_Active();
	if (shown) {
		if (!UnknownC5) {
			UnknownC5 = true;
			Unknown_5F2990(false);
			for (std::map<int, RegistrationComboStruct>::iterator combo = Unknown108.begin(); combo != Unknown108.end(); combo++) {
				if ((*combo).second.Combo) {
					(*combo).second.Combo->Unknown_57B3C0(0);
				}
			}
		}
	} else if (UnknownC5) {
		UnknownC5 = false;
		Unknown_5F2990(true);
		for (std::map<int, RegistrationComboStruct>::iterator combo = Unknown108.begin(); combo != Unknown108.end(); ++combo) {
			if ((*combo).second.Combo) {
				(*combo).second.Combo->Unknown_57B3C0(1);
			}
		}
	}
}

// @0x5F2950
bool RegistrationCardPrefixStruct::Is_Prefix_Of(const char *text) const
{
	return strncmp(text, Prefix.c_str(), Prefix.size()) == 0 ? true : false;
}

// @0x5F2990
void RegistrationInterface::Unknown_5F2990(bool on)
{
	std::map<int, RegistrationButtonStruct>::iterator button = UnknownE8.begin();
	while (button != UnknownE8.end()) {
		(*button).second.Gadget->Unknown_40(on);
		if ((*button).second.Style) {
			RenderObjClass *model = !(*button).second.InFrame ? DialogRenderObj.Peek() : UnknownBC->Peek_Model();
			FontSys *style = Unknown_BE67C4->Get_Style((*button).second.Style, model);
			if (style) {
				style->Unknown_Set_2FC(on ? 1.0f : 0.33f);
			}
		}
		button++;
	}
}

// @0x5F2A80
void RegistrationInterface::Unknown_5F2A80(int on, UIComboBoxClass *except)
{
	std::map<int, RegistrationComboStruct>::iterator combo = Unknown108.begin();
	while (combo != Unknown108.end()) {
		if ((*combo).second.Combo != except) {
			(*combo).second.Combo->Unknown_57B3C0(on);
		}
		combo++;
	}
}





















































































































// @0x5F2B20
void RegistrationInterface::Unknown_5F2B20(int id, const char *name, const char *style, unsigned short key, unsigned short group, bool in_frame)
{
	RenderObjClass *robj = !in_frame ? DialogRenderObj->Get_Sub_Object_By_Name(name) : UnknownBC->Peek_Model()->Get_Sub_Object_By_Name(name);
	G_ASSERT(robj);
	Unknown_496AE0(robj, false);
	ButtonGadgetClass *gadget = new ButtonGadgetClass(robj, *Layer, 5, 3);
	RegistrationButtonStruct button;
	button.Gadget = gadget;
	button.RObj = robj;
	button.Name = name;
	button.Style = style;
	button.Key = key;
	button.Group = group;
	button.Selected = false;
	button.InFrame = in_frame;
	if (group != 0) {
		gadget->Unknown_90(Unknown1B8.Peek());
	}
	UnknownE8.insert(std::map<int, RegistrationButtonStruct>::value_type(id, button));
	Unknown64.Unknown_6870E0(gadget, id, View, true, false);
	if (key != 0) {
		((RegistrationKeyListClass *)Unknown4C)->Add(RegistrationKeyStruct(key), (void *)id);
	}
	robj->Release_Ref();
}

// @0x5F2D30
void RegistrationInterface::Unknown_5F2D30(int id, const char *name, const char *style, int length, const char *text, int next, bool password, unsigned int flags, const char *chars)
{
	RegistrationTextStruct value;
	value.Unknown04 = id;
	value.Unknown08 = (int)style;
	value.Unknown0C = (int)name;
	value.Unknown10 = next;
	TextGadgetClass *gadget = new TextGadgetClass(this, length, style, DialogRenderObj.Peek(), flags | Unknown_B08CBC | Unknown_B08CC0, chars);
	value.Gadget = gadget;
	gadget->Cursor->Set_Color(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	gadget->Unknown_28(Layer);
	gadget->Set_Text(text, 0);
	gadget->Unknown5C = password;
	gadget->Unknown3B = true;
	UnknownF8.insert(std::map<int, RegistrationTextStruct>::value_type(id, value));
	Unknown64.Unknown_6870E0(gadget, 0x2F, View, true, false);
	Unknown_5F2B20(id, name, NULL, 0, 0, false);
}

// @0x5F2F90
void RegistrationInterface::Unknown_5F2F90(int id, const char *name, int a, int b)
{
	RegistrationComboStruct value;
	value.Unknown04 = id;
	value.Unknown08 = (int)name;
	value.Combo = new UIComboBoxClass(this, Layer, DialogRenderObj.Peek(), name, id, 0x30, a, b, 0);
	Unknown108.insert(std::map<int, RegistrationComboStruct>::value_type(id, value));
}

// @0x5F3110
void RegistrationInterface::Unknown_5F3110()
{
	for (std::map<int, RegistrationButtonStruct>::iterator button = UnknownE8.begin(); button != UnknownE8.end(); button = UnknownE8.begin()) {
		Unknown64.Unknown_687150((*button).second.Gadget);
		UnknownE8.erase(UnknownE8.begin());
	}
	for (std::map<int, RegistrationTextStruct>::iterator text = UnknownF8.begin(); text != UnknownF8.end(); text = UnknownF8.begin()) {
		(*text).second.Gadget->Unknown_74(false);
		Unknown64.Unknown_687150((*UnknownF8.begin()).second.Gadget);
		UnknownF8.erase(UnknownF8.begin());
	}
	for (std::map<int, RegistrationComboStruct>::iterator combo = Unknown108.begin(); combo != Unknown108.end(); combo = Unknown108.begin()) {
		UIComboBoxClass *box = (*combo).second.Combo;
		box->Unknown_7C(NULL, false);
		if (box) {
			box->Release_Ref();
		}
		Unknown108.erase(Unknown108.begin());
	}
	Unknown148 = 0;
}

// @0x5F3250
void RegistrationInterface::Unknown_5F3250(int id)
{
	unsigned short group = UnknownE8[id].Group;
	for (std::map<int, RegistrationButtonStruct>::iterator button = UnknownE8.begin(); button != UnknownE8.end(); button++) {
		if ((*button).second.Group == group) {
			(*button).second.Gadget->Unknown_90(Unknown1B8.Peek());
			(*button).second.Selected = false;
		}
	}
	TextureClass *on = Unknown1B4.Peek();
	UnknownE8[id].Gadget->Unknown_90(on);
	UnknownE8[id].Selected = true;
}

// @0x5F33E0
RenderObjClass *RegistrationInterface::Unknown_5F33E0(unsigned short group)
{
	std::map<int, RegistrationButtonStruct>::iterator button = UnknownE8.begin();
	while (button != UnknownE8.end()) {
		if ((*button).second.Group == group && (*button).second.Selected) {
			return((*button).second.RObj);
		}
		button++;
	}
	return(NULL);
}

// @0x5F3470
void RegistrationInterface::Unknown_5F3470(unsigned short group)
{
	std::map<int, RegistrationButtonStruct>::iterator button = UnknownE8.begin();
	while (button != UnknownE8.end()) {
		if ((*button).second.Group == group) {
			(*button).second.Gadget->Unknown_90(Unknown1B8.Peek());
			(*button).second.Selected = false;
		}
		button++;
	}
}

// @0x5F3510
void RegistrationInterface::Unknown_5F3510(int id, bool on)
{
	ButtonGadgetClass *gadget = UnknownE8[id].Gadget;
	const char *style = UnknownE8[id].Style;
	gadget->Unknown_8C(on);
	if (style) {
		FontSys *font = Unknown_BE67C4->Get_Style(style, DialogRenderObj.Peek());
		if (font) {
			font->Set_Hidden(on != 0);
		}
	}
}

// @0x5F3600
void RegistrationInterface::Unknown_5F3600()
{
	Unknown_5F3110();
	switch (UnknownD4) {
		case 0x16:
			Unknown1C0 = NULL;
			delete Unknown1BC;
			Unknown1BC = NULL;
			break;
		case 0x15:
			((RegistrationSettingsClass *)Unknown_7CA2D0())->Unknown_7CB240(4);
			UnknownC0->Unknown0018->Unknown_515400();
			break;
	}
	if (Unknown118) {
		Unknown_BE67C4->Remove(UnknownBC->Peek_Model());
		Unknown_BE67BC->Unload("RG_FRAME.INI");
		Unknown_BE67C4->Remove(DialogRenderObj.Peek());
		Unknown_BE67BC->Unload(RegistrationPages[UnknownD4].Ini);
		Unknown118 = false;
	}
	RenderObjClass *frame = UnknownBC->Peek_Model();
	if (frame) {
		frame->Remove_Sub_Object(DialogRenderObj.Peek());
		DialogRenderObj = NULL;
	}
}























































































































































































































// @0x5F3770
void RegistrationInterface::Unknown_5F3770(int state)
{
	Unknown_5C();
	InterfaceClass::Unknown_5C();
	Unknown_5F3600();
	if (state == 0x18) {
		UnknownD4 = 0x18;
		InterfaceClass::Unknown_58();
		Unknown_58();
		TheDX8MeshRenderer.Invalidate();
		return;
	}
	RenderObjClass *frame = UnknownBC->Peek_Model();
	DialogRenderObj = Create_Get(WW3DAssetManager::Get_Instance()->Create_Render_Obj(RegistrationPages[state].Name));
	G_ASSERT(DialogRenderObj);
	frame->Add_Sub_Object(DialogRenderObj.Peek());
	Unknown_BE67BC->Load("RG_FRAME.INI");
	Unknown_BE67C4->Add(frame);
	Unknown_BE67BC->Load(RegistrationPages[state].Ini);
	Unknown_BE67C4->Add(DialogRenderObj.Peek());
	Unknown118 = true;
	Unknown_5F2B20(0x2E, "Button_Privacy", "BUTTON_PRIVACY_RG_FRAME", 0, 0, true);
	Unknown_5F8DB0(state != 0x15 && state != 0x16);
	Unknown_Debug_Printf("meshname: %s   fontini: %s \n", RegistrationPages[state].Name, RegistrationPages[state].Ini);
	UnknownD4 = state;
	UnknownBC->Get_Unknown198()->Unknown_784E00();
	switch (UnknownD4) {
		case 0:
			Unknown_5F2B20(1,"RADIO_YES",0,0,1,0);
			Unknown_5F2B20(2,"RADIO_NO",0,0,1,0);
			Unknown_5F2B20(5,"RADIO_FORGOT",0,0,1,0);
			Unknown_5F3250(Unknown268);
			Unknown_5F2B20(0x2d,"LEFT_BUTTON","LEFT_BUTTON_RG_A1",0,0,0);
			Unknown_5F2B20(6,"RIGHT_BUTTON","RIGHT_BUTTON_RG_A1",0,0,0);
			if (!TutorialManagerClass::Instance->Is_Completed("REG1")) {
				UnknownBC->Get_Unknown198()->Unknown_784C90("REG1", false);
				TutorialManagerClass::Instance->Set_Completed("REG1");
			}
			break;
		case 1:
			Unknown_5F2D30(10,"Entry_Username","Entry_Username_RG_A2",0x10,Unknown1D8.c_str(),0xb,0, Unknown_BE5DC8,0);
			Unknown_5F2D30(0xb,"Entry_Password","Entry_Password_RG_A2",0x10,Unknown1E8.c_str(),10,1, Unknown_BE5DC4,Unknown_B85EDC);
			Unknown_5F2B20(0x2d,"LEFT_BUTTON","LEFT_BUTTON_RG_A2",0,0,0);
			Unknown_5F2B20(6,"RIGHT_BUTTON","RIGHT_BUTTON_RG_A2",0,0,0);
			Unknown_5F2B20(7,"MIDDLE_BUTTON","MIDDLE_BUTTON_RG_A2",0,0,0);
			if (!TutorialManagerClass::Instance->Is_Completed("REG2")) {
				UnknownBC->Get_Unknown198()->Unknown_784C90("REG2", false);
				TutorialManagerClass::Instance->Set_Completed("REG2");
			}
			if (Unknown1D8.empty()) {
				Unknown_5F8CD0(10, true);
			}
			break;
		case 2:
			Unknown_5F2B20(1,"RADIO_YES",0,0,1,0);
			Unknown_5F2B20(2,"RADIO_NO",0,0,1,0);
			Unknown_5F3250((2 - (Unknown1C9)));
			Unknown_5F2B20(0x2d,"LEFT_BUTTON","LEFT_BUTTON_RG_A3",0,0,0);
			Unknown_5F2B20(6,"RIGHT_BUTTON","RIGHT_BUTTON_RG_A3",0,0,0);
			Unknown_5F2B20(7,"MIDDLE_BUTTON","MIDDLE_BUTTON_RG_A3",0,0,0);
			if (!TutorialManagerClass::Instance->Is_Completed("REG4")) {
				UnknownBC->Get_Unknown198()->Unknown_784C90("REG4", false);
				TutorialManagerClass::Instance->Set_Completed("REG4");
			}
			break;
		case 3:
			Unknown_5F2D30(10,"Entry_Username","Entry_Username_RG_A5",0x10,Unknown1D8.c_str(),0xb,0, Unknown_BE5DC8,0);
			Unknown_5F2D30(0xb,"Entry_Password","Entry_Password_RG_A5",0x10,Unknown1E8.c_str(),0xc,1, Unknown_BE5DC4,0);
			Unknown_5F2D30(0xc,"Entry_Retype","Entry_Retype_RG_A5",0x10,Unknown1F8.c_str(),10,1, Unknown_BE5DC4,0);
			Unknown_5F2B20(0x2d,"LEFT_BUTTON","LEFT_BUTTON_RG_A5",0,0,0);
			Unknown_5F2B20(6,"RIGHT_BUTTON","RIGHT_BUTTON_RG_A5",0,0,0);
			Unknown_5F2B20(7,"MIDDLE_BUTTON","MIDDLE_BUTTON_RG_A5",0,0,0);
			if (!TutorialManagerClass::Instance->Is_Completed("REG3")) {
				UnknownBC->Get_Unknown198()->Unknown_784C90("REG3", false);
				TutorialManagerClass::Instance->Set_Completed("REG3");
			}
			if (Unknown1D8.empty()) {
				Unknown_5F8CD0(10, true);
			}
			break;
		case 4:
			Unknown_5F2D30(10,"Entry_Username","ENTRY_USERNAME_RG_A6",0x10,Unknown1D8.c_str(),0xb,0, Unknown_BE5DC8,0);
			Unknown_5F2D30(0xb,"Entry_Password","ENTRY_PASSWORD_RG_A6",0x10,Unknown208.c_str(),10,1, Unknown_BE5DC4,0);
			Unknown_5F2B20(0x2d,"LEFT_BUTTON","LEFT_BUTTON_RG_A6",0,0,0);
			Unknown_5F2B20(6,"RIGHT_BUTTON","RIGHT_BUTTON_RG_A6",0,0,0);
			Unknown_5F2B20(7,"MIDDLE_BUTTON","MIDDLE_BUTTON_RG_A6",0,0,0);
			if (!TutorialManagerClass::Instance->Is_Completed("REG5")) {
				UnknownBC->Get_Unknown198()->Unknown_784C90("REG5", false);
				TutorialManagerClass::Instance->Set_Completed("REG5");
			}
			if (Unknown1D8.empty()) {
				Unknown_5F8CD0(10, true);
			}
			break;
		case 5:
			Unknown_5F2D30(0xb,"Entry_Password","ENTRY_PASSWORD_RG_A7",0x10,Unknown1E8.c_str(),0xc,1, Unknown_BE5DC4,0);
			Unknown_5F2D30(0xc,"Entry_Retype","ENTRY_RETYPE_RG_A7",0x10,Unknown1F8.c_str(),0xb,1, Unknown_BE5DC4,0);
			Unknown_5F2B20(0x2d,"LEFT_BUTTON","LEFT_BUTTON_RG_A7",0,0,0);
			Unknown_5F2B20(6,"RIGHT_BUTTON","RIGHT_BUTTON_RG_A7",0,0,0);
			Unknown_5F2B20(7,"MIDDLE_BUTTON","MIDDLE_BUTTON_RG_A7",0,0,0);
			if (!TutorialManagerClass::Instance->Is_Completed("REG6")) {
				UnknownBC->Get_Unknown198()->Unknown_784C90("REG6", false);
				TutorialManagerClass::Instance->Set_Completed("REG6");
			}
			if (Unknown1E8.empty()) {
				Unknown_5F8CD0(0xb, true);
			}
			break;
		case 6:
			Unknown_5F2B20(8,"RADIO_NAME",0,0,1,0);
			Unknown_5F2B20(9,"RADIO_PASSWORD",0,0,1,0);
			Unknown_5F3250(Unknown26C);
			Unknown_5F2B20(0x2d,"LEFT_BUTTON","LEFT_BUTTON_RG_A10",0,0,0);
			Unknown_5F2B20(6,"RIGHT_BUTTON","RIGHT_BUTTON_RG_A10",0,0,0);
			Unknown_5F2B20(7,"MIDDLE_BUTTON","MIDDLE_BUTTON_RG_A10",0,0,0);
			break;
		case 7:
			Unknown_5F2D30(0x13,"ENTRY_EMAIL","ENTRY_EMAIL_RG_A8",0x28,Unknown258.c_str(),0,0,Unknown_BE5DDC, Unknown_B85ED0);
			Unknown_5F2B20(0x2d,"LEFT_BUTTON","LEFT_BUTTON_RG_A8",0,0,0);
			Unknown_5F2B20(6,"RIGHT_BUTTON","RIGHT_BUTTON_RG_A8",0,0,0);
			Unknown_5F2B20(7,"MIDDLE_BUTTON","MIDDLE_BUTTON_RG_A8",0,0,0);
			if (Unknown258.empty()) {
				Unknown_5F8CD0(0x13, true);
			}
			break;
		case 8:
			Unknown_5F2D30(10,"ENTRY_USERNAME","ENTRY_NAME_RG_A9",0x10,Unknown248.c_str(),0,0,Unknown_BE5DE0, 0);
			Unknown_5F2B20(0x2d,"LEFT_BUTTON","LEFT_BUTTON_RG_A9",0,0,0);
			Unknown_5F2B20(6,"RIGHT_BUTTON","RIGHT_BUTTON_RG_A9",0,0,0);
			Unknown_5F2B20(7,"MIDDLE_BUTTON","MIDDLE_BUTTON_RG_A9",0,0,0);
			if (Unknown248.empty()) {
				Unknown_5F8CD0(10, true);
			}
			break;
		case 9:
			Unknown_5F2B20(0xd,"Radio_01",0,0,1,0);
			Unknown_5F2B20(0xe,"Radio_02",0,0,1,0);
			Unknown_5F2B20(0xf,"Radio_03",0,0,1,0);
			Unknown_5F2B20(0x10,"Radio_04",0,0,1,0);
			Unknown_5F2B20(0x11,"Radio_05",0,0,1,0);
			Unknown_5F2B20(0x12,"Radio_06",0,0,1,0);
			Unknown_5F3250(0x12);
			Unknown_5F2B20(0x2d,"LEFT_BUTTON","LEFT_BUTTON_RG_B1",0,0,0);
			Unknown_5F2B20(6,"RIGHT_BUTTON","RIGHT_BUTTON_RG_B1",0,0,0);
			Unknown_5F8520("LABEL_SUGGEST01_RG_B1", Unknown3C4.Unknown00[0].c_str());
			Unknown_5F8520("LABEL_SUGGEST02_RG_B1", Unknown3C4.Unknown00[1].c_str());
			Unknown_5F8520("LABEL_SUGGEST03_RG_B1", Unknown3C4.Unknown00[2].c_str());
			Unknown_5F8520("LABEL_SUGGEST04_RG_B1", Unknown3C4.Unknown00[3].c_str());
			Unknown_5F8520("LABEL_SUGGEST05_RG_B1", Unknown3C4.Unknown00[4].c_str());
			if (!TutorialManagerClass::Instance->Is_Completed("REG7")) {
				UnknownBC->Get_Unknown198()->Unknown_784C90("REG7", false);
				TutorialManagerClass::Instance->Set_Completed("REG7");
			}
			break;
		case 10:
			{
				char buffer[6];
				Unknown_5F2D30(0x13, "ENTRY_E-MAIL", "ENTRY_EMAILADDRESS_RG_C1", 0x28, Unknown3C4.Unknown5C.c_str(), 0, 0, Unknown_BE5DAC, Unknown_B85ED0);
				Unknown_5F2F90(0x16, "COMBO_YEAR", 5, 5);
				Unknown_5F98C0(1900, Unknown124.tm_year + 1900);
				if (Unknown3C4.Unknown7E < 1) {
					Unknown108[0x16].Combo->Unknown_57AC70(0);
				} else {
					_snprintf(buffer, 5, "%d", (int)Unknown3C4.Unknown7E);
					Unknown108[0x16].Combo->Unknown_57AD50(buffer);
				}
				Unknown_5F2F90(0x14, "COMBO_MONTH", 4, 5);
				Unknown108[0x14].Combo->Unknown_57B1D0(RegistrationMonths);
				Unknown108[0x14].Combo->Unknown_57AC70(Unknown3C4.Unknown7D < 1 ? 0 : Unknown3C4.Unknown7D - 1);
				Unknown_5F2F90(0x15, "COMBO_DAY", 2, 5);
				Unknown_5F9460();
				Unknown108[0x15].Combo->Unknown_57AC70(Unknown3C4.Unknown7C < 1 ? 0 : Unknown3C4.Unknown7C - 1);
				Unknown_5F2B20(0x17, "RADIO_MALE", 0, 0, 1, false);
				Unknown_5F2B20(0x18, "RADIO_FEMALE", 0, 0, 1, false);
				Unknown_5F3250((Unknown3C4.Unknown58 != 0) + 0x17);
				Unknown_5F2F90(0x19, "COMBO_COUNTRY", 0x2C, 7);
				Unknown_5F9700();
				if (Unknown3C4.Unknown80.size() == 0) {
					Unknown108[0x19].Combo->Unknown_57AD50(UnknownA4);
				} else {
					Unknown108[0x19].Combo->Unknown_57AD50(Unknown3C4.Unknown80.c_str());
				}
				Unknown_5F2B20(0x2d,"LEFT_BUTTON","LEFT_BUTTON_RG_C1",0,0,0);
				Unknown_5F2B20(6,"RIGHT_BUTTON","RIGHT_BUTTON_RG_C1",0,0,0);
				Unknown_5F2B20(7,"MIDDLE_BUTTON","MIDDLE_BUTTON_RG_C1",0,0,0);
				if (!TutorialManagerClass::Instance->Is_Completed("REG8")) {
					UnknownBC->Get_Unknown198()->Unknown_784C90("REG8", false);
					TutorialManagerClass::Instance->Set_Completed("REG8");
				}
				if (Unknown3C4.Unknown5C.empty()) {
					Unknown_5F8CD0(0x13, true);
				}
			}
			break;
		case 0xB:
			Unknown_5F2F90(0x2A, "COMBO_OPTION", 0x14, std::_cpp_min(5, (int)Unknown598.size()));
			Unknown_5F9EB0();
			Unknown108[0x2A].Combo->Unknown_57AC70(Unknown388);
			Unknown38C = Unknown598[Unknown388];
			Unknown_5F2B20(0x2d,"LEFT_BUTTON","LEFT_BUTTON_RG_D1",0,0,0);
			Unknown_5F2B20(6,"RIGHT_BUTTON","RIGHT_BUTTON_RG_D1",0,0,0);
			Unknown_5F2B20(7,"MIDDLE_BUTTON","MIDDLE_BUTTON_RG_D1",0,0,0);
			Unknown_5F84E0("WRAP_EXPLAIN_TEXT_RG_D1", PaymentTypeTexts[Unknown38C]);
			if (!TutorialManagerClass::Instance->Is_Completed("REG14")) {
				UnknownBC->Get_Unknown198()->Unknown_784C90("REG14", false);
				TutorialManagerClass::Instance->Set_Completed("REG14");
			}
			break;
		case 0xC:
			{
				char buffer[6];
				Unknown_5F2D30(0x1a,"ENTRY_NAME","ENTRY_FIRSTNAME_RG_D3",0x32,Unknown478.Unknown04.c_str(),0x1b,0, Unknown_BE5DA8,0);
				Unknown_5F2D30(0x1b,"ENTRY_LASTNAME","ENTRY_LASTNAME_RG_D3",0x32,Unknown478.Unknown14.c_str(),0x1c,0, Unknown_BE5DA8,0);
				Unknown_5F2D30(0x1c,"ENTRY_ADDRESS1","ENTRY_ADDRESS1_RG_D3",0x32,Unknown478.Unknown24.c_str(),0x1d,0, Unknown_BE5DE4,Unknown_B85ED4);
				Unknown_5F2D30(0x1d,"ENTRY_ADDRESS2","ENTRY_ADDRESS2_RG_D3",0x32,Unknown478.Unknown34.c_str(),0x1e,0, Unknown_BE5DE4,Unknown_B85ED4);
				Unknown_5F2D30(0x1e,"ENTRY_CITY","ENTRY_CITY_RG_D3",0x32,Unknown478.Unknown44.c_str(),0x1f,0, Unknown_BE5DD8,0);
				Unknown_5F2D30(0x1f,"ENTRY_STATE","ENTRY_STATE_RG_D3",2,Unknown478.Unknown54.c_str(),0x20,0,Unknown_BE5DB4, 0);
				Unknown_5F2D30(0x20,"ENTRY_ZIP","ENTRY_ZIP_RG_D3",10,Unknown478.Unknown74.c_str(),0x21,0,Unknown_BE5DBC, Unknown_B85ED8);
				Unknown_5F2D30(0x21,"ENTRY_PHONE","ENTRY_PHONE_RG_D3",0xf,Unknown478.Unknown84.c_str(),0x23,0, Unknown_BE5DCC,Unknown_B85ECC);
				Unknown_5F2D30(0x23,"ENTRY_CARDNUMBE","ENTRY_CARD_RG_D3",0x10,Unknown478.UnknownA8.c_str(),0,0, Unknown_BE5DD0,0);
				Unknown_5F2F90(0x19, "COMBO_COUNTRY", 0x2C, 7);
				Unknown_5F9700();
				if (Unknown478.Unknown64.empty()) {
					if (Unknown3C4.Unknown80.size() == 0) {
						Unknown108[0x19].Combo->Unknown_57AD50(UnknownA4);
					} else {
						Unknown108[0x19].Combo->Unknown_57AD50(Unknown3C4.Unknown80.c_str());
						Unknown108[0x19].Combo->Unknown_57B3C0(0);
					}
				} else {
					Unknown108[0x19].Combo->Unknown_57AD50(Unknown478.Unknown64.c_str());
					Unknown108[0x19].Combo->Unknown_57B3C0(0);
				}
				Unknown_5F2F90(0x22, "COMBO_CARDTYPE", 0x13, 4);
				Unknown_5F9A90();
				Unknown108[0x22].Combo->Unknown_57AC70(0);
				{
					std::vector<RegistrationUnknown5A8Struct>::iterator entry = Unknown5A8.begin();
					std::vector<RegistrationUnknown5A8Struct>::iterator last = Unknown5A8.end();
					for (; entry != last; entry++) {
						if ((*entry).Unknown00 == Unknown478.Unknown94.Unknown00) {
							break;
						}
					}
					if (entry != last) {
						Unknown108[0x22].Combo->Unknown_57AC70(entry - Unknown5A8.begin());
					}
				}
				Unknown_5F2F90(0x14, "COMBO_MONTH", 4, 5);
				Unknown108[0x14].Combo->Unknown_57B1D0(RegistrationMonths);
				Unknown108[0x14].Combo->Unknown_57AC70(Unknown478.UnknownE8 < 1 ? 0 : Unknown478.UnknownE8 - 1);
				Unknown_5F2F90(0x16, "COMBO_YEAR", 5, 5);
				Unknown_5F98C0(Unknown124.tm_year + 1900, Unknown124.tm_year + 1910);
				if (Unknown478.UnknownEC < 1) {
					Unknown108[0x16].Combo->Unknown_57AC70(0);
				} else {
					_snprintf(buffer, 5, "%d", Unknown478.UnknownEC);
					Unknown108[0x16].Combo->Unknown_57AD50(buffer);
				}
				Unknown_5F84E0("WRAP_LABEL_TEXT_RG_D3", !Unknown1CD ? "\nPlease enter your billing information below." : "The information below is for Identification/Age verification purposes only.  YOU WILL NOT BE BILLED!");
				Unknown_5F2B20(0x2d,"LEFT_BUTTON","LEFT_BUTTON_RG_D3",0,0,0);
				Unknown_5F2B20(6,"RIGHT_BUTTON","RIGHT_BUTTON_RG_D3",0,0,0);
				Unknown_5F2B20(7,"MIDDLE_BUTTON","MIDDLE_BUTTON_RG_D3",0,0,0);
				Unknown_5F3510(7, Unknown1D0);
				if (!Unknown1CD) {
					if (!TutorialManagerClass::Instance->Is_Completed("REG13")) {
						UnknownBC->Get_Unknown198()->Unknown_784C90("REG13", false);
						TutorialManagerClass::Instance->Set_Completed("REG13");
					}
				}
				if (Unknown478.Unknown04.empty()) {
					Unknown_5F8CD0(0x1A, true);
				}
			}
			break;




































































































































		case 0xD:
			{
				char buffer[6];
				Unknown_5F2D30(0x1a,"ENTRY_NAME","ENTRY_FIRSTNAME_RG_D4",0x32,Unknown478.Unknown04.c_str(),0x1b,0, Unknown_BE5DA8,0);
				Unknown_5F2D30(0x1b,"ENTRY_LASTNAME","ENTRY_LASTNAME_RG_D4",0x32,Unknown478.Unknown14.c_str(),0x1c,0, Unknown_BE5DA8,0);
				Unknown_5F2D30(0x1c,"ENTRY_ADDRESS1","ENTRY_ADDRESS1_RG_D4",0x32,Unknown478.Unknown24.c_str(),0x1d,0, Unknown_BE5DE4,Unknown_B85ED4);
				Unknown_5F2D30(0x1d,"ENTRY_ADDRESS2","ENTRY_ADDRESS2_RG_D4",0x32,Unknown478.Unknown34.c_str(),0x1e,0, Unknown_BE5DE4,Unknown_B85ED4);
				Unknown_5F2D30(0x1e,"ENTRY_CITY","ENTRY_CITY_RG_D4",0x32,Unknown478.Unknown44.c_str(),0x1f,0, Unknown_BE5DD8,0);
				Unknown_5F2D30(0x1f,"ENTRY_STATE","ENTRY_STATE_RG_D4",0x14,Unknown478.Unknown54.c_str(),0x20,0, Unknown_BE5DA4,0);
				Unknown_5F2D30(0x20,"ENTRY_ZIP","ENTRY_ZIP_RG_D4",10,Unknown478.Unknown74.c_str(),0x21,0,Unknown_BE5DD4, Unknown_B85ED8);
				Unknown_5F2D30(0x21,"ENTRY_PHONE","ENTRY_PHONE_RG_D4",0xf,Unknown478.Unknown84.c_str(),0x23,0, Unknown_BE5DCC,Unknown_B85ECC);
				Unknown_5F2D30(0x23,"ENTRY_CARDNUMBE","ENTRY_CARD_RG_D4",0x14,Unknown478.UnknownA8.c_str(),0x25,0, Unknown_BE5DD0,0);
				Unknown_5F2D30(0x25,"ENTRY_MONTH","ENTRY_MONTH_RG_D4",2,Unknown478.UnknownC8.c_str(),0x26,0,Unknown_BE5DB0, 0);
				Unknown_5F2D30(0x26,"ENTRY_YEAR","ENTRY_YEAR_RG_D4",4,Unknown478.UnknownD8.c_str(),0x24,0,Unknown_BE5DB0,0);
				Unknown_5F2D30(0x24,"ENTRY_ISSUER","ENTRY_ISSUER_RG_D4",2,Unknown478.UnknownB8.c_str(),0,0,Unknown_BE5DC0, 0);
				Unknown_5F2F90(0x19, "COMBO_COUNTRY", 0x2C, 7);
				Unknown_5F9700();
				if (Unknown478.Unknown64.empty()) {
					if (Unknown3C4.Unknown80.size() == 0) {
						G_ASSERT(0);
						Unknown108[0x19].Combo->Unknown_57AD50(UnknownA4);
					} else {
						Unknown108[0x19].Combo->Unknown_57AD50(Unknown3C4.Unknown80.c_str());
						Unknown108[0x19].Combo->Unknown_57B3C0(0);
					}
				} else {
					Unknown108[0x19].Combo->Unknown_57AD50(Unknown478.Unknown64.c_str());
					Unknown108[0x19].Combo->Unknown_57B3C0(0);
				}
				Unknown_5F2F90(0x22, "COMBO_CARDTYPE", 0x13, 4);
				Unknown_5F9A90();
				Unknown108[0x22].Combo->Unknown_57AC70(0);
				{
					std::vector<RegistrationUnknown5A8Struct>::iterator entry = Unknown5A8.begin();
					std::vector<RegistrationUnknown5A8Struct>::iterator last = Unknown5A8.end();
					for (; entry != last; entry++) {
						if ((*entry).Unknown00 == Unknown478.Unknown94.Unknown00) {
							break;
						}
					}
					if (entry != last) {
						Unknown108[0x22].Combo->Unknown_57AC70(entry - Unknown5A8.begin());
					}
				}
				Unknown_5F2F90(0x14, "COMBO_MONTH", 4, 5);
				Unknown108[0x14].Combo->Unknown_57B1D0(RegistrationMonths);
				Unknown108[0x14].Combo->Unknown_57AC70(Unknown478.UnknownE8 < 1 ? 0 : Unknown478.UnknownE8 - 1);
				Unknown_5F2F90(0x16, "COMBO_YEAR", 5, 5);
				Unknown_5F98C0(Unknown124.tm_year + 1900, Unknown124.tm_year + 1910);
				if (Unknown478.UnknownEC < 1) {
					Unknown108[0x16].Combo->Unknown_57AC70(0);
				} else {
					_snprintf(buffer, 5, "%d", Unknown478.UnknownEC);
					Unknown108[0x16].Combo->Unknown_57AD50(buffer);
				}
				Unknown_5F84E0("WRAP_LABEL_TEXT_RG_D4", !Unknown1CD ? "\nPlease enter your billing information below." : "The information below is for Identification/Age verification purposes only.  YOU WILL NOT BE BILLED!");
				Unknown_5F2B20(0x2d,"LEFT_BUTTON","LEFT_BUTTON_RG_D4",0,0,0);
				Unknown_5F2B20(6,"RIGHT_BUTTON","RIGHT_BUTTON_RG_D4",0,0,0);
				Unknown_5F2B20(7,"MIDDLE_BUTTON","MIDDLE_BUTTON_RG_D4",0,0,0);
				Unknown_5F3510(7, Unknown1D0);
				if (!Unknown1CD) {
					if (!TutorialManagerClass::Instance->Is_Completed("REG13")) {
						UnknownBC->Get_Unknown198()->Unknown_784C90("REG13", false);
						TutorialManagerClass::Instance->Set_Completed("REG13");
					}
				}
				if (Unknown478.Unknown04.empty()) {
					Unknown_5F8CD0(0x1A, true);
				}
			}
			break;
		case 0xE:
			Unknown_5F2F90(0x29, "COMBO_OPTION", 0x2D, std::_cpp_min(5, (int)Unknown578.size()));
			Unknown_5F9C90();
			Unknown108[0x29].Combo->Unknown_57AC70(Unknown384);
			Unknown270 = Unknown578[Unknown384];
			Unknown_5F84E0("WRAP_LABEL_DESC_RG_F1", Unknown270.Unknown34.c_str());
			Unknown_5F2B20(0x2d,"LEFT_BUTTON","LEFT_BUTTON_RG_F1",0,0,0);
			Unknown_5F2B20(6,"RIGHT_BUTTON","RIGHT_BUTTON_RG_F1",0,0,0);
			Unknown_5F2B20(7,"MIDDLE_BUTTON","MIDDLE_BUTTON_RG_F1",0,0,0);
			if (!TutorialManagerClass::Instance->Is_Completed("REG15")) {
				UnknownBC->Get_Unknown198()->Unknown_784C90("REG15", false);
				TutorialManagerClass::Instance->Set_Completed("REG15");
			}
			break;
		case 0xF:
			Unknown_5F2B20(1, "RADIO_YES1", 0, 0, 1, false);
			Unknown_5F2B20(2, "RADIO_NO1", 0, 0, 1, false);
			Unknown_5F3250(2 - (Unknown3C4.Unknown90 != 0));
			Unknown_5F2B20(3, "RADIO_YES2", 0, 0, 2, false);
			Unknown_5F2B20(4, "RADIO_NO2", 0, 0, 2, false);
			Unknown_5F3250(4 - (Unknown3C4.Unknown91 != 0));
			Unknown_5F2B20(0x2d,"LEFT_BUTTON","LEFT_BUTTON_RG_C2",0,0,0);
			Unknown_5F2B20(6,"RIGHT_BUTTON","RIGHT_BUTTON_RG_C2",0,0,0);
			Unknown_5F2B20(7,"MIDDLE_BUTTON","MIDDLE_BUTTON_RG_C2",0,0,0);
			if (!TutorialManagerClass::Instance->Is_Completed("REG9")) {
				UnknownBC->Get_Unknown198()->Unknown_784C90("REG9", false);
				TutorialManagerClass::Instance->Set_Completed("REG9");
			}
			break;
		case 16:
			Unknown_5F2D30(0x13,"ENTRY_PARENT","ENTRY_PARENT_RG_C3",0x28,Unknown3C4.Unknown6C.c_str(),0,0, Unknown_BE5DAC,Unknown_B85ED0);
			Unknown_5F2B20(0x2d,"LEFT_BUTTON","LEFT_BUTTON_RG_C3",0,0,0);
			Unknown_5F2B20(6,"RIGHT_BUTTON","RIGHT_BUTTON_RG_C3",0,0,0);
			Unknown_5F2B20(7,"MIDDLE_BUTTON","MIDDLE_BUTTON_RG_C3",0,0,0);
			if (!TutorialManagerClass::Instance->Is_Completed("REG10")) {
				UnknownBC->Get_Unknown198()->Unknown_784C90("REG10", false);
				TutorialManagerClass::Instance->Set_Completed("REG10");
			}
			if (Unknown3C4.Unknown6C.empty()) {
				Unknown_5F8CD0(0x13, true);
			}
			break;
		case 0x11:
			Unknown_5F2D30(0x2B, "ENTRY_CODE", "ENTRY_CODE_RG_E1", 0x13, Unknown228.c_str(), 0x2B, 0, Unknown_BE5DB8, 0);
			Unknown_5F2B20(0x2d,"LEFT_BUTTON","LEFT_BUTTON_RG_E1",0,0,0);
			Unknown_5F2B20(6,"RIGHT_BUTTON","RIGHT_BUTTON_RG_E1",0,0,0);
			Unknown_5F2B20(7,"MIDDLE_BUTTON","MIDDLE_BUTTON_RG_E1",0,0,0);
			Unknown_5F3510(7, Unknown1D0);
			if (Unknown228.size() == 0 || Unknown1D0) {
				Unknown_5F8CD0(0x2B, true);
			}
			if (!TutorialManagerClass::Instance->Is_Completed("REG11")) {
				UnknownBC->Get_Unknown198()->Unknown_784C90("REG11", false);
				TutorialManagerClass::Instance->Set_Completed("REG11");
			}
			break;
		case 0x12:
			Unknown_5F2B20(0x2d,"LEFT_BUTTON","LEFT_BUTTON_RG_G1",0,0,0);
			Unknown_5F2B20(6,"RIGHT_BUTTON","RIGHT_BUTTON_RG_G1",0,0,0);
			Unknown_5F2B20(7,"MIDDLE_BUTTON","MIDDLE_BUTTON_RG_G1",0,0,0);
			if (!Unknown1CD) {
				if (!TutorialManagerClass::Instance->Is_Completed("REG16")) {
					UnknownBC->Get_Unknown198()->Unknown_784C90("REG16", false);
					TutorialManagerClass::Instance->Set_Completed("REG16");
				}
			} else {
				Unknown_5F84E0("WRAP_LABEL_RG_G1", "We are ready to activate your Trial Subscription.\n\nClick the NEXT button to proceed.");
			}
			break;
		case 0x13:
			Unknown_5FAC90();
			if (Unknown11B) {
				Unknown_5F84E0("WRAP_LABEL_TEXT_RG_I1", "Your GameTime Code has been applied to your Account.\n\nClick \"Go Play!\" to log into Earth & Beyond!");
			}
			Unknown_5F2B20(6, "MIDDLE_BUTTON", "MIDDLE_BUTTON_RG_I1", 0, 0, false);
			if (!TutorialManagerClass::Instance->Is_Completed("REG17")) {
				UnknownBC->Get_Unknown198()->Unknown_784C90("REG17", false);
				TutorialManagerClass::Instance->Set_Completed("REG17");
			}
			break;
		case 0x14:
			Unknown_5F2D30(0x2B, "ENTRY_CODE", "ENTRY_CODE_RG_K1", 0x13, Unknown238.c_str(), 0x2B, 0, Unknown_BE5DB8, 0);
			Unknown_5F2B20(0x2d,"LEFT_BUTTON","LEFT_BUTTON_RG_K1",0,0,0);
			Unknown_5F2B20(6,"RIGHT_BUTTON","RIGHT_BUTTON_RG_K1",0,0,0);
			Unknown_5F2B20(7,"MIDDLE_BUTTON","MIDDLE_BUTTON_RG_K1",0,0,0);
			if (Unknown238.size() == 0 || Unknown1D0) {
				Unknown_5F8CD0(0x2B, true);
			}
			if (!TutorialManagerClass::Instance->Is_Completed("REG12")) {
				UnknownBC->Get_Unknown198()->Unknown_784C90("REG12", false);
				TutorialManagerClass::Instance->Set_Completed("REG12");
			}
			break;
		case 0x15:
			Unknown1C4 = 10;
			Unknown_5F2B20(7, "MIDDLE_BUTTON", "MIDDLE_BUTTON_RG_X2", 0, 0, false);
			break;
		case 0x16:
			Unknown1BC = new HAnimComboClass(1);
			Unknown1C0 = Create_Get(WW3DAssetManager::Get_Instance()->Get_HAnim("RG_X1.RG_X1a"));
			Unknown1BC->Set_Motion(0, Unknown1C0.Peek());
			Unknown1BC->Set_Weight(0, 1.0f);
			DialogRenderObj->Set_Animation(Unknown1BC);
			break;
		case 0x17:
			Unknown_5F2B20(0x2D, "LEFT_BUTTON", "LEFT_BUTTON_RG_X3", 0, 0, false);
			Unknown_5F2B20(6, "RIGHT_BUTTON", "RIGHT_BUTTON_RG_X3", 0, 0, false);
			break;
	}
	InterfaceClass::Unknown_58();
	Unknown_58();
	TheDX8MeshRenderer.Invalidate();
}

// @0x5F84E0
void RegistrationInterface::Unknown_5F84E0(const char *name, const char *text)
{
	ManagedFontClass *font = Unknown_BE67C4->Find(DialogRenderObj.Peek());
	if (font) {
		font->Unknown_6D0A70(name, text);
	}
}

// @0x5F8520
void RegistrationInterface::Unknown_5F8520(const char *name, const char *text)
{
	FontSys *style = Unknown_BE67C4->Get_Style(name, DialogRenderObj.Peek());
	if (style) {
		style->Change_Text(text, 0);
	}
}

// @0x5F8560
void RegistrationInterface::Unknown_5F8560()
{
	if ((UnknownD4 == 1 && Unknown11B) || (UnknownD4 == 0 && Unknown11D) || UnknownD4 == 0x17) {
		Unknown_5F2990(false);
		for (std::map<int, RegistrationComboStruct>::iterator it = Unknown108.begin(); it != Unknown108.end(); it++) {
			if ((*it).second.Combo) {
				(*it).second.Combo->Unknown_57B3C0(0);
			}
		}
		UnknownCA = true;
		UnknownC7 = true;
		UnknownBC->Get_Unknown198()->Unknown_784E00();
		UnknownBC->Unknown_67A8D0(true);
		UnknownC0->Go_Next_Step();
	} else {
		int state = 0;
		if (UnknownD8.size() != 0) {
			state = UnknownD8.back();
		}
		if (UnknownD8.size() != 0) {
			UnknownD8.pop_back();
		}
		Unknown_5F3770(state);
	}
}

// @0x5F86F0
void RegistrationInterface::Unknown_5F86F0()
{
	Unknown_5F2990(false);
	std::map<int, RegistrationComboStruct>::iterator it = Unknown108.begin();
	while (it != Unknown108.end()) {
		if ((*it).second.Combo) {
			(*it).second.Combo->Unknown_57B3C0(0);
		}
		it++;
	}
	UnknownCA = true;
	UnknownC7 = true;
	UnknownBC->Get_Unknown198()->Unknown_784E00();
	UnknownBC->Unknown_67A8D0(true);
	UnknownC0->Go_Next_Step();
}

// @0x5F87D0
void RegistrationInterface::Unknown_5F87D0()
{
	UnknownD8.push_back(UnknownD4);
}

// @0x5F89F0
int RegistrationInterface::Unknown_5F89F0()
{
	int state = Unknown_5F8A90();
	if (UnknownD8.size() != 0) {
		UnknownD8.pop_back();
	}
	return(state);
}

// @0x5F8A90
int RegistrationInterface::Unknown_5F8A90()
{
	if (UnknownD8.size() != 0) {
		return(UnknownD8.back());
	}
	return(0);
}

// @0x5F8AD0
void RegistrationInterface::Unknown_5F8AD0()
{
	if (UnknownF8[Unknown148].Unknown10 != 0) {
		UnknownF8[Unknown148].Gadget->Unknown_74(false);
		int next = UnknownF8[Unknown148].Unknown10;
		if (Unknown148 != next && Unknown148 != 0) {
			UnknownF8[Unknown148].Gadget->Unknown_74(false);
		}
		int focus = 0;
		if (next != 0) {
			UnknownF8[next].Gadget->Unknown_74(true);
			focus = next;
		}
		Unknown148 = focus;
	} else {
		if (Unknown148 != 0) {
			UnknownF8[Unknown148].Gadget->Unknown_74(false);
		}
		Unknown148 = 0;
	}
}

// @0x5F8CD0
void RegistrationInterface::Unknown_5F8CD0(int id, bool on)
{
	if (Unknown148 != id && Unknown148 != 0) {
		UnknownF8[Unknown148].Gadget->Unknown_74(false);
	}
	int focus = 0;
	if (id != 0) {
		UnknownF8[id].Gadget->Unknown_74(on);
		focus = id;
	}
	Unknown148 = on ? focus : 0;
}

// @0x5F8DB0
void RegistrationInterface::Unknown_5F8DB0(bool arg)
{
	RenderObjClass *frame = UnknownBC->Peek_Model();
	RenderObjClass *button = frame->Get_Sub_Object_By_Name("Button_Privacy");
	FontSys *style = Unknown_BE67C4->Get_Style("BUTTON_PRIVACY_RG_FRAME", frame);
	if (arg) {
		button->Set_Hidden(0);
		if (style) {
			style->Unknown4E4 = true;
			style->Set_Animation_Hidden(0);
			((RegistrationFontClass *)style)->Unknown_6D5EF0(0);
			style->Set_Hidden(0);
		}
	} else {
		button->Set_Hidden(1);
		if (style) {
			style->Unknown4E4 = false;
			style->Set_Animation_Hidden(1);
			((RegistrationFontClass *)style)->Unknown_6D5EF0(1);
			style->Set_Hidden(1);
		}
	}
	button->Release_Ref();
}

// @0x5F8EA0
bool RegistrationInterface::Unknown_5F8EA0(int month, int day, int year)
{
	if (year < 1900) {
		return(false);
	}
	if (month <= 0) {
		return(false);
	}
	int days = DaysInMonth[month - 1];
	bool leap = true;
	if (year % 4 != 0 || (year % 400 != 0 && year % 100 == 0)) {
		leap = false;
	}
	if (leap && month == 2) {
		days = 29;
	} else if (month < 1 || month > 12) {
		return(false);
	}
	if (day >= 1 && day <= days) {
		return(true);
	}
	return(false);
}

// @0x5F8F60
bool RegistrationInterface::Unknown_5F8F60(const char *text)
{
	if (text && strlen(text) != 0) {
		if (strchr(text, '@') && strchr(text, '.')) {
			return(true);
		}
	}
	return(false);
}

// @0x5F8FD0
void RegistrationInterface::Unknown_5F8FD0()
{
	INIClass ini(RegDataFile);
	ini.Get_String(Unknown98, "Connection", "regserverurl", "");
	ini.Get_String(Unknown9C, "Connection", "regserverpage", "");
	ini.Get_String(UnknownA0, "Connection", "regserverver", "");
	ini.Get_String(UnknownA4, "Settings", "defaultcountry", "");
	ini.Get_String(UnknownA4, "Countries", UnknownA4, "");
	ini.Get_String(UnknownA8, "Settings", "csrphone", "");
	int count = ini.Entry_Count("Countries");
	int i;
	for (i = 0; i < count; i++) {
		StringClass country;
		char key[15];
		_snprintf(key, 15, "country%03d", i);
		ini.Get_String(country, "Countries", key, "");
		UnknownAC.push_back(country);
	}
	int numprefixes = ini.Get_Int("CardPrefixes", "numprefixes", 0);
	Unknown88.clear();
	for (i = 0; i < numprefixes; i++) {
		StringClass prefix;
		char key[15];
		_snprintf(key, 15, "card%02dprefix", i);
		ini.Get_String(prefix, "CardPrefixes", key, "");
		RegistrationCardPrefixStruct entry;
		entry.Prefix = prefix;
		_snprintf(key, 15, "card%02did", i);
		entry.Id = ini.Get_Int("CardPrefixes", key, 0);
		Unknown88.push_back(entry);
	}
}

// NOT EXACT: shape (explain.py: 14 calls in ours, 13 in the client; the value_type temporaries' stack slots differ)
// @0x5F9460
void RegistrationInterface::Unknown_5F9460()
{
	int month = Unknown108[0x14].Combo->Selected;
	int day = Unknown108[0x15].Combo->Selected;
	int days = DaysInMonth[month];
	UIComboBoxClass *years = Unknown108[0x16].Combo;
	int year = (years->Selected < years->Items.size()) ? atoi(years->Items[years->Selected].c_str()) : 0;
	if (month == 1 && Unknown_5F8EA0(2, 29, year)) {
		days = 29;
	}
	UIComboBoxClass *days_box = Unknown108[0x15].Combo;
	days_box->Unknown_57B210();
	for (int i = 1; i <= days; i++) {
		char text[5];
		_snprintf(text, sizeof(text), "%02d", i);
		days_box->Unknown_57B000(text);
	}
	Unknown108[0x15].Combo->Unknown_57AC70(day);
}

// @0x5F9700
void RegistrationInterface::Unknown_5F9700()
{
	for (std::vector<StringClass>::iterator it = UnknownAC.begin(); it != UnknownAC.end(); it++) {
		Unknown108[0x19].Combo->Unknown_57B000(*it);
	}
}

// @0x5F98C0
void RegistrationInterface::Unknown_5F98C0(int from, int to)
{
	for (int i = from; i <= to; i++) {
		char text[6];
		_snprintf(text, sizeof(text), "%d", i);
		Unknown108[0x16].Combo->Unknown_57B000(text);
	}
}

// @0x5F9A90
void RegistrationInterface::Unknown_5F9A90()
{
	for (std::vector<RegistrationUnknown5A8Struct>::iterator it = Unknown5A8.begin(); it != Unknown5A8.end(); it++) {
		Unknown108[0x22].Combo->Unknown_57B000((*it).Unknown04.c_str());
	}
}

// @0x5F9C90
void RegistrationInterface::Unknown_5F9C90()
{
	for (std::vector<RegistrationUnknown578Struct>::iterator it = Unknown578.begin(); it != Unknown578.end(); it++) {
		if (std::find((*it).Unknown4C.begin(), (*it).Unknown4C.end(), Unknown38C) != (*it).Unknown4C.end()) {
			Unknown108[0x29].Combo->Unknown_57B000((*it).Unknown24.c_str());
		}
	}
}








































































































































































































// @0x5F9EB0
void RegistrationInterface::Unknown_5F9EB0()
{
	for (std::vector<int>::iterator it = Unknown598.begin(); it != Unknown598.end(); it++) {
		const char *typetext = PaymentTypeNames[*it];
		G_ASSERT(typetext);
		Unknown108[0x2A].Combo->Unknown_57B000(typetext);
	}
}

// NOT EXACT: field-offset (explain.py; 340 instructions each: register choice in the inlined operator= into +0x2CC and the 'EB' compare)
// @0x5FA0B0
void RegistrationInterface::Unknown_5FA0B0()
{
	Unknown578 = Unknown1AC->Unknown_7D3570();
	Unknown598.clear();
	std::sort(Unknown578.begin(), Unknown578.end());
	std::vector<RegistrationUnknown578Struct>::iterator it = Unknown578.begin();
	Unknown1CD = false;
	while (it != Unknown578.end()) {
		bool found = false;
		if ((*it).Unknown10 == ProductTrial && (!Unknown1CF || Unknown11D)) {
			Unknown1CD = true;
			Unknown270 = *it;
			Unknown38C = 1;
			return;
		}
		if ((*it).Unknown10 == ProductEB) {
			for (std::vector<int>::iterator type = (*it).Unknown4C.begin(); type != (*it).Unknown4C.end(); type++) {
				if (PaymentTypeNames[*type]) {
					if (std::find(Unknown598.begin(), Unknown598.end(), *type) == Unknown598.end()) {
						if (*type == 3) {
							Unknown2CC = *it;
						}
						Unknown598.push_back(*type);
					}
					found = true;
				}
			}
		}
		if (!found) {
			it = Unknown578.erase(it);
		} else {
			it++;
		}
	}
	if (Unknown598.size() != 0) {
		Unknown598.push_back(5);
	}
}

// @0x5FA5F0
void RegistrationInterface::Unknown_5FA5F0()
{
	Unknown588 = Unknown1AC->Unknown_7D3570();
	std::sort(Unknown588.begin(), Unknown588.end());
	Unknown328 = Unknown588[0];
}

// @0x5FA900
bool RegistrationInterface::Unknown_5FA900()
{
	Unknown1D0 = true;
	if (Unknown11B) {
		StatusIndex = 0xB;
		Unknown_5FAE90(this, 0);
		if (Unknown1AC->Unknown_7D9790(Unknown1D8.c_str(), Unknown1E8.c_str(), Unknown328.Unknown00.c_str(), Unknown328.Unknown4C[0],
				Unknown_5FB330(Unknown238).c_str(), 0, 0, true, Unknown_BE8AD4)) {
			Unknown_5F8DB0(false);
			Unknown_5F3770(0x16);
			Unknown_5FAE90(this, "Registering GameTime...");
			UnknownBC->Get_Unknown198()->Unknown_784E00();
			return true;
		}
	} else {
		StatusIndex = 8;
		if (!Unknown1D1 && !Unknown1D3) {
			if (Unknown1AC->Unknown_7D4E30(Unknown1D8.c_str(), Unknown1E8.c_str(), &Unknown3C4,
					Unknown1C9 ? Unknown208.c_str() : 0, true, 0, Unknown_BE8AD4)) {
				Unknown_5F8DB0(false);
				Unknown_5F3770(0x16);
				Unknown_5FAE90(this, "Creating EA Account...");
				UnknownBC->Get_Unknown198()->Unknown_784C90("REG21", false);
				return true;
			}
		} else {
			if (Unknown3C4.Unknown80.length() == 0) {
				Unknown3C4.Unknown80 = "US";
			}
			if (Unknown1AC->Unknown_7D5600(Unknown1D8.c_str(), Unknown1E8.c_str(), &Unknown3C4, false, Unknown_BE8AD4)) {
				Unknown_5F8DB0(false);
				Unknown_5F3770(0x16);
				Unknown_5FAE90(this, "Updating EA Account...");
				UnknownBC->Get_Unknown198()->Unknown_784C90("REG21", false);
				return true;
			}
		}
	}
	return false;
}

// @0x5FAC90
void RegistrationInterface::Unknown_5FAC90()
{
	{
		RegistryClass reg(RegistrationKey, true);
		reg.Set_Bool("Registered", true);
		reg.Set_Bool("Trial", Unknown1CD);
	}
	RegFlushKey(HKEY_LOCAL_MACHINE);
}

// @0x5FAD50
bool Unknown_5FAD50()
{
	RegistryClass reg(RegistrationKey, true);
	return reg.Get_Bool("Registered", false);
}

// @0x5FADE0
bool Unknown_5FADE0()
{
	RegistryClass reg(RegistrationKey, true);
	return reg.Get_Bool("Trial", false);
}

// @0x5FAE70
bool Unknown_5FAE70()
{
	return true;
}

// @0x5FAE90
void __cdecl Unknown_5FAE90(RegistrationInterface *self, const char *format, ...)
{
	char buffer[1024];
	if (format) {
		va_list args;
		va_start(args, format);
		vsprintf(buffer, format, args);
		va_end(args);
		FontManagerClass *fm = Unknown_BE67C4;
		RenderObjClass *font_id = self->DialogRenderObj.Peek();
		ManagedFontClass *font = fm->Find(font_id);
		if (font) {
			font->Unknown_6D0A70("WRAP_TEXT_MAIN_RG_X1", buffer);
		}
	} else {
		FontManagerClass *fm = Unknown_BE67C4;
		const char *text = StatusStrings[self->StatusIndex];
		ManagedFontClass *font = fm->Find(self->DialogRenderObj.Peek());
		if (font) {
			font->Unknown_6D0A70("WRAP_TEXT_MAIN_RG_X1", text);
		}
	}
}

// @0x5FAF60
void RegistrationInterface::Unknown_5FAF60(int arg)
{
	char buffer[1024];
	const char *str = Sub14C.Slot12(arg);
	_snprintf(buffer, 1024, "%s...", str);
	RenderObjClass *font_id = DialogRenderObj.Peek();
	ManagedFontClass *font = Unknown_BE67C4->Find(font_id);
	if (font) {
		font->Unknown_6D0A70("WRAP_STATUS_TEXT_RG_X1", buffer);
	}
}

// @0x5FAFF0
void RegistrationInterface::Unknown_5FAFF0(const char *text)
{
	Unknown_5F8DB0(0);
	Unknown_5F3770(0x16);
	Unknown_5FAE90(this, text);
}

// @0x5FB030
void RegistrationInterface::Unknown_5FB030()
{
	int width;
	int height;
	int bits;
	bool windowed;
	WW3D::Get_Device_Resolution(width, height, bits, windowed);
	RenderObjClass *label = DialogRenderObj->Get_Sub_Object_By_Name("LABEL_TEXT");
	Unknown_7CA2D0()->Unknown_7CA5C0(4, label, NULL);
	UnknownC0->Unknown0018->Unknown_5153B0();
	label->Release_Ref();
}

// NOT EXACT: inline-budget (explain.py: the client calls the second append(1, '-') and the first end()'s _Freeze out of line)
// @0x5FB0C0
void RegistrationInterface::Unknown_5FB0C0()
{
	if (UnknownD4 != 0x11 && UnknownD4 != 0x14) {
		return;
	}
	TextBaseGadgetClass *gadget = UnknownF8[0x2B].Gadget;
	bool changed = gadget->Changed;
	gadget->Changed = false;
	if (!changed) {
		return;
	}
	if (gadget->Active) {
		char *text = gadget->Get_Text();
		std::string number;
		int count = 1;
		for (unsigned int i = 0; i < min(strlen(text), 19u); i++) {
			if (text[i] != '-') {
				number += text[i];
				if ((count & 3) == 0) {
					number += '-';
				}
				count++;
			}
		}
		delete text;
		if (number.length() != 0 && *(number.end() - 1) == '-') {
			number.erase(number.end() - 1);
		}
		gadget->Set_Text(number.c_str(), false);
	}
	gadget->Changed = true;
}

// @0x5FB330
std::string RegistrationInterface::Unknown_5FB330(std::string str)
{
	std::string copy = str;
	for (std::string::iterator it = copy.begin(); it != copy.end(); ) {
		if (*it == '-') {
			it = copy.erase(it);
		} else {
			++it;
		}
	}
	return copy;
}

// @0x5FB540
std::string RegistrationInterface::Unknown_5FB540(std::string str)
{
	std::string copy = str;
	for (std::string::iterator it = copy.begin(); it != copy.end(); ) {
		if (*it == ' ') {
			it = copy.erase(it);
		} else {
			++it;
		}
	}
	return copy;
}


















































































































































































































































































// @0x5FB750
bool RegistrationInterface::Unknown_5FB750(const char *test)
{
	FAIL_IF(!test || *test == '\0') return false;
	for (;;) {
		if (*test == ',') {
			return true;
		}
		if (*test++ == '\0') {
			break;
		}
	}
	return false;
}

// @0x5FB7A0
void __cdecl Unknown_5FB7A0(RegistrationInterface *self, const char *format, ...)
{
	char buffer[1024];
	if (format) {
		va_list args;
		va_start(args, format);
		vsprintf(buffer, format, args);
		va_end(args);
		self->UnknownBC->Get_Unknown34()->Unknown_588A70("OK", 0, buffer, 0x140, 0, 0);
	} else {
		_snprintf(buffer, 1024, "%s\n(%d)", self->Unknown390.Unknown14.c_str(), self->Unknown390.Unknown10);
		self->UnknownBC->Get_Unknown34()->Unknown_588A70("OK", 0, buffer, 0x140, 0, 0);
	}
}

// @0x5FB890
void RegistrationInterface::Unknown_5FB890()
{
	char buffer[1024];
	if (Unknown390.Unknown10 != -1) {
		_snprintf(buffer, 1024, "%s\n(%d)", RegistrationServerError, Unknown390.Unknown10);
	} else {
		_snprintf(buffer, 1024, "%s", RegistrationServerError);
	}
	UnknownBC->Get_Unknown34()->Unknown_588A70("OK", 0, buffer, 0x140, 0, 0);
}

// @0x5FB950
void RegistrationInterface::Unknown_5F00E0()
{
	if (UnknownD4 == 0x16) {
		Unknown_5FAF60(Sub14C.Slot11());
	}
}

// NOT EXACT: field-offset (explain.py's first mismatch; the shape is inline-budget: the client calls the map's begin 0x606960, the iterator's
// operator++(int) 0x604DF0 and pop_back's copy 0x60A060 / _Destroy 0x605950 out of line, and destroys the reply under one EH state)
// @0x5FB9F0
void RegistrationInterface::Unknown_5F0100()
{
	Unknown390 = Unknown1AC->Unknown_7D1AC0();
	if (Unknown390.Unknown10 == -1) {
		Unknown_5FB890();
	} else {
		UnknownBC->Get_Unknown34()->Unknown_588A70("OK", 0, Unknown390.Unknown14.c_str(), 0x140, 0, 0);
	}
	if ((UnknownD4 == 1 && Unknown11B) || (UnknownD4 == 0 && Unknown11D) || UnknownD4 == 0x17) {
		Unknown_5F2990(0);
		for (std::map<int, RegistrationComboStruct>::iterator it = Unknown108.begin(); it != Unknown108.end(); it++) {
			if ((*it).second.Combo) {
				(*it).second.Combo->Unknown_57B3C0(0);
			}
		}
		UnknownCA = true;
		UnknownC7 = true;
		UnknownBC->Get_Unknown198()->Unknown_784E00();
		UnknownBC->Unknown_67A8D0(true);
		UnknownC0->Go_Next_Step();
	} else {
		int state = 0;
		if (UnknownD8.size() != 0) {
			state = UnknownD8.back();
		}
		if (UnknownD8.size() != 0) {
			UnknownD8.pop_back();
		}
		Unknown_5F3770(state);
	}
}

// @0x603820
bool RegistrationInterface::Unknown_603820(const char *number)
{
	std::string text(number);
	if (text.size() > 0x10 || text.size() < 0xF) {
		return(false);
	}
	int sum = 0;
	std::string::iterator it = text.end();
	for (int i = 0; i < (int)text.size(); i++) {
		it--;
		int digit = *it - '0';
		if (i % 2 != 0) {
			digit = CardDoubled[digit];
		}
		sum += digit;
	}
	return(sum % 10 == 0);
}

// @0x6039E0
int RegistrationInterface::Unknown_6039E0(const char *number)
{
	std::vector<RegistrationCardPrefixStruct>::iterator prefix = Unknown88.begin();
	std::vector<RegistrationCardPrefixStruct>::iterator last = Unknown88.end();
	while (prefix != last && !(*prefix).Is_Prefix_Of(number)) {
		prefix++;
	}
	if (prefix != Unknown88.end()) {
		return((*prefix).Id);
	}
	return(0);
}

// @0x603A60
bool RegistrationInterface::Unknown_603A60(int month, int day, int year, int age)
{
	time_t now = time(NULL);
	struct tm today = *gmtime(&now);
	if (today.tm_mon < month - 1 || (today.tm_mon == month - 1 && today.tm_mday < day)) {
		return((today.tm_year - year) + 1899 >= age);
	}
	return((today.tm_year - year) + 1900 >= age);
}

// @0x603B20
void RegistrationInterface::Unknown_603B20()
{
	if (Unknown3C4.Unknown80 == "US") {
		Unknown_5F3770(0xC);
	} else {
		Unknown_5F3770(0xD);
	}
}

/*
**	Reconstruction notes (builder ureg2, 2026-11-02)
**
**	ui_registration.cpp (the client's object: asserts lines 369-3637; client 0x5EF0B0-0x5FBD30+).
**	- 0x5FAD50 / 0x5FADE0 (UREG): demo/registration predicates reading "Registered" and "Trial".
**	- 0x5FAE70: trivial registration predicate returning true (byte-exact, short, 3 bytes).
**	- 0x5FAE90: wrap text formatter helper (byte-exact, 154 bytes).
**	- 0x5FAF60: status text formatter (byte-exact, builder reg 2026-11-04: the font key read into a local
**	  before FontManagerClass::Find, as 0x5FAE90 does; inlined into 0x5FB950).
**	- 0x5FAFF0: registration window status helper (byte-exact, 37 bytes).
**	- 0x5FB330: hyphen stripper string helper (byte-exact, 401 bytes).
**	- 0x5FB540: space stripper string helper (byte-exact, 401 bytes).
**
**	Builder reg, 2026-11-04:
**	- RegistrationInterface is an InterfaceClass with a second base at +0x84 (the constructor 0x5EF0B0 stores
**	  0xAF7184 there, then the final table 0xAF6FF4, whose slot 0 is the adjustor 0x4297A0 'sub ecx, 0x84' to the
**	  deleting destructor 0x5F06C0): RegistrationCallbackClass, three empty slots overridden with 0x600000 /
**	  0x5FB950 / 0x5FB9F0, which run with this at +0x84.
**	- 0x5FB750: the ',' test (FAIL_IF line 2868, also inlined into 0x5FBD30), byte-exact; __stdcall like 0x5FB330
**	  (no caller; a member that ignores this compiles the same).
**	- 0x5FB7A0 / 0x5FB890: the 'OK' message boxes through the view at +0xBC (UIViewClass::Get_Unknown34), the
**	  reply's error number +0x3A0 and text +0x3A4 ('%s\n(%d)'), byte-exact.
**	- 0x5FB030 (no caller): the 'LABEL_TEXT' sub-object handed to the settings store's 0x7CA5C0, then the login
**	  task's (+0xC0) software mouse 0x5153B0; the device resolution is read and unused, byte-exact.
**	- 0x5FB950: the status line refresh in state 0x16 (0x5FAF60 inlined with Sub14C's slot 0x2C), byte-exact.
**	- 0x5FAC90 (0x5F3770's caller): 'Registered' 1 and 'Trial' +0x1CD written, then RegFlushKey(HKLM), byte-exact.
**	- 0x5FB9F0 (slot +0x0C): the reply's error shown, then on (state 1 and +0x11B) / (state 0 and +0x11D) / state
**	  0x17 the combo boxes reset (0x57B3C0) and the login task's Go_Next_Step, else the previous state popped.
**	  Functional: the client calls the map's begin 0x606960, the iterator's operator++(int) 0x604DF0 and
**	  pop_back's copy / _Destroy out of line (as the map inserts 0x5F9700 / 0x5F98C0 / 0x5F9EB0 call begin and
**	  pair's constructor 0x6074E0), and destroys the reply under one EH state with only its first string's
**	  _Tidy inlined; ours inlines all of them. An empty ~RegistrationErrorStruct and a named local were tried.
**	- The map at +0x108: 12-byte values whose first dword is a UIComboBoxClass * (0x5FBD30 reads its Selected
**	  through the out-of-line getter 0x5FFFE0, 'mov eax, [ecx + 0xE0]'). 0x5FFFC0 is UIViewClass::Get_Unknown34
**	  emitted out of line for 0x5FBD30; 0x600000 is slot +0x04 of 0xAF6FF4 (the reply handler): all three belong
**	  to this object (triage's run 0x5FFFC0-0x603660).
**	- 0x5F0A80: a template instance the map / vector members bring in (verify matches it in this object).
**	- The combo boxes' fillers, byte-exact: 0x5F9700 (the countries +0xAC into box 0x19), 0x5F98C0 (the numbers
**	  from..to, '%d', into box 0x16), 0x5F9A90 (+0x5A8's texts into box 0x22), 0x5F9C90 (+0x578's entries whose
**	  id list holds +0x38C into box 0x29), 0x5F9EB0 (the payment types +0x598 into box 0x2A; G_ASSERT 'typetext'
**	  line 2342). Unknown108[id] is the map's operator[]: its insert inlined with begin, --, _Insert and pair's
**	  constructor out of line, as the client has them. The status table ends at 0xB86048, where the payment type
**	  names and descriptions follow (two 7-entry tables).
**	- 0x5F9460 (functional): the day box refilled for the month (DaysInMonth 0xAF6F58; 29 when 0x5F8EA0 accepts
**	  29 February of the year box's year). 0x5FB0C0 (functional): the card number field (map +0xF8, id 0x2B)
**	  regrouped in fours with '-', at most 19 characters, in states 0x11 / 0x14.
**
**	Builder reg2, 2026-11-04:
**	- 0x5FB330 / 0x5FB540 / 0x5FB750 and 0x5F8DB0 are RegistrationInterface members (0x5FA900 sets ecx = this).
**	- 0x5FA900 (byte-exact): the account request: with +0x11B the GameTime one (0x7D9790, +0x238 through 0x5FB330;
**	  the client's own out-of-line c_str 0x553A60 and vector operator[] 0x604130 come out of ours too), else
**	  create (0x7D4E30) or, with +0x1D1 / +0x1D3, update (0x7D5600; +0x3C4's +0x80 defaults to 'US').
**	- 0x5FA5F0 (byte-exact): +0x588 = 0x7D3570's vector, sorted (operator< 0x60BCF0 compares +0x48, by value),
**	  its first copied to +0x328. That makes +0x328 (and +0x270 / +0x2CC, 0x5FA0B0) 0x5C-byte entries:
**	  RegistrationUnknown578Struct is four strings, three ints and the id vector.
**	- 0x5F8FD0 (byte-exact): RG_REGDATA.INI ([Connection], [Settings], [Countries] into +0xAC, [CardPrefixes]
**	  into +0x88). The key buffer is char[15] (with [16] the INIClass and the buffer swap stack slots).
**	- 0x5FA0B0 (functional): +0x578 = 0x7D3570's entries, +0x598 cleared, sorted; an 'EB-TRIAL' entry (taken
**	  while +0x1CF is clear or +0x11D set) goes to +0x270 with +0x1CD and +0x38C = 1; 'EB' entries add their
**	  payment types with a name (0xB86048) to +0x598 (type 3's entry to +0x2CC); the others are erased; a non-empty
**	  +0x598 gets 5 appended. The string compares are operator== (the client calls compare(const char *) 0x4CBA10
**	  out of line); the residue is register choice in the inlined operator= into +0x2CC.
**	- 0x600000 (not written): the reply handler, a switch on StatusIndex (+0x1B0, this at +0x84) with shared tails;
**	  its case 1 copies 0x7D21C0's account record into +0x3C4 (five strings in a loop, so +0x3C4 starts with a
**	  std::string[5]; ints +0x50..+0x58, strings +0x5C / +0x6C, bytes +0x7C / +0x7D, short +0x7E, +0x80, bytes
**	  +0x90 / +0x91, two more strings).
**
**	Builder RG1, 2026-11-06:
**	- The object opens at 0x5EED20: 17 dynamic initializers of the text fields' flags (Unknown_BE5DC8 .. Unknown_BE5DDC,
**	  written in their order at the top of the file; 0xB08C90-0xB08CB8 are textgadg.h's flags), then the constructor
**	  0x5EF0B0, and runs at least to the out-of-line vector helpers 0x60B760-0x60C350.
**	- The classes moved to ui_registration.h under __UI_REGISTRATION_MEMBERS (provisional, D137): they filled the lines
**	  above the constructor's G_ASSERT at line 369. Only this file defines the macro, so main_client.cpp sees what it did.
**	- DialogRenderObj is the client's DialogRenderObj (G_ASSERT line 1281), a RefCountPtr<RenderObjClass> (destructor 0x4740B0).
**	- The +0x14C member is Sub14CImplClass (0x60 bytes, tables 0xAF70DC / 0xAF70C8; slots 0x7DFED0-0x7E15B0 in their own
**	  object) over Sub14CClass (table 0xAF7130: pure but for 0x5F0170 / 0x5F0280 / 0x5F02A0) and RegistrationCallbackClass.
**	- +0x1AC is a function-static RegistrationUnknown1ACClass at 0xBDA960 (destructor 0x5F0680, atexit 0x5F0660).
**	- The screen: 0x5F3770(state) loads RegistrationPages[state] (0xB85F08; state 0x18 only leaves), switch on the state;
**	  G_ASSERT(0) line 1693 is in case 0xD, an empty country. Exact helpers: 0x5F3110, 0x5F2990, 0x5F2A80, 0x5F33E0,
**	  0x5F3470, 0x5F3510, 0x5F3600, 0x5F84E0, 0x5F8520, 0x5F86F0, 0x5F87D0, 0x5F89F0, 0x5F8A90, 0x5F8DB0, 0x5F8F60, 0x603B20.
**	- A map walk that inlines _Min is `it = begin(); while (it != end()) { ...; it++; }` (workflow.md 3.1): 0x5F2990, 0x5F2A80,
**	  0x5F33E0, 0x5F3470, 0x5F86F0. A function with the `for` form keeps the call.
**	- Slot 0x90 of the model in 0x5F1730 is Update_Sub_Object_Transforms, not Set_Animation (0x94, case 0x16 of 0x5F3770).
**	- 0x5FBD30 (Next, 17 KB, FAIL_IF lines 2868 / 3637, callers 0x5F1A10 key 6) and 0x600000 (the reply handler, 14 KB) are
**	  still not written. 0x600000's switch is on StatusIndex with one inlined error box per reply; the 0xF4-byte entries
**	  (+0x568, destructor 0x449CC0, vectors of 0x30 and 0x4C bytes inside) come from 0x7D3CC0.
*/
