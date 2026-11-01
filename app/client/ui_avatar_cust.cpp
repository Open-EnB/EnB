// Earth & Beyond client (reconstruction): app\Client\ui_avatar_cust.cpp. Notes at the end of the file.
#include "sclient.h"
#include "sbaseview.h"
#include "..\shared\auxdata\auxdata.h"
#include "..\shared\auxdata\delta_template.h"
#include "..\shared\money.h"
#include "..\packets\pkrecustomize.h"
#include "..\packets\pkstarbasetow.h"
#include "ww3d.h"
#include "rendobj.h"
#include "matrix3d.h"
#include "texture.h"
#include "..\..\wwlib\ref_ptr.h"

// gadget.cpp's gadget, as far as this file calls it
class GadgetClass	// NAME 3: class-from-file (D24) for app\client\gadget.cpp
{
public:
	virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C();
	virtual void Unknown_10(); virtual void Unknown_14(); virtual void Unknown_18(); virtual void Unknown_1C();
	virtual void Unknown_20(); virtual void Unknown_24(); virtual void Unknown_28(); virtual void Unknown_2C();
	virtual void Unknown_30(); virtual void Unknown_34(); virtual void Unknown_38(); virtual void Unknown_3C();
	virtual void Unknown_40(bool on);					// +0x40 (shown; 0x551B20 passes its argument to every gadget)  // NAME 1 [on]: neutral

	void Unknown_6603A0(int arg, UIViewClass *view);	// 0x6603A0  // NAME 1 [arg, view]: neutral
	void Unknown_6607D0();								// 0x6607D0

	char Unknown04[0x2C - 0x04];						// +0x04
	unsigned int Unknown2CLow : 11;						// +0x2C
	unsigned int Unknown2C800 : 1;						// +0x2C bit 0x800 (0x550670 sets it with 0x6603A0, clears it with 0x6607D0)
	RenderObjClass *Unknown30;							// +0x30 (0x550A10 hands it to 0x497300)
	void Unknown_6608F0();								// 0x6608F0 (0x550A10, after the model's turn)
};

// The client at InterfaceClass +0x40, as this file calls it
class LineDisplayClientClass  // NAME 1: neutral (D10)
{
public:
	virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C();
	virtual void Unknown_10(); virtual void Unknown_14();
	virtual SClient *As_Space_Client();					// +0x18 (as ui_manu.cpp's)  // NAME 3: the FAIL_IF's in ui_manu.cpp

	char Unknown04[0x112C - 0x04];						// +0x04
	unsigned AvatarID;									// +0x112C (0x54EC30)  // NAME 3: client.h's ClientClass::AvatarID
};

// An object whose first member is a gadget (the window's +0x308 / +0x31C, as far as 0x551B20 reads them)
struct UIAvatarCustGadgetHolderStruct	// NAME 1: neutral (D10)
{
	GadgetClass *Unknown00;								// +0x00
};

RefCountPtr<RenderObjClass> Unknown_497300(RenderObjClass *robj);	// 0x497300 (cdecl, ro_funcs.cpp's neighbourhood)  // NAME 1 [robj]: neutral
void Unknown_495FD0(RenderObjClass *robj, TextureClass *texture, int unknown);	// 0x495FD0 (ro_funcs.cpp, cdecl): the texture onto the model  // NAME 1 [robj, texture, unknown]: as ui_manu.cpp's

// The object at *0xBFAEE0 (kernel.cpp's), as far as this file calls it: its slot +0x30 gives a texture by name
class UnknownBFAEE0Class	// NAME 1 [UnknownBFAEE0Class]: kernel.cpp's
{
public:
	virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C();
	virtual void Unknown_10(); virtual void Unknown_14(); virtual void Unknown_18(); virtual void Unknown_1C();
	virtual void Unknown_20(); virtual void Unknown_24(); virtual void Unknown_28(); virtual void Unknown_2C();
	virtual TextureClass *Get_Texture(const char *name, int a, int b, int c);	// +0x30 (0x5513E0: the name, 0, 0, 1)  // NAME 2: what it does (the asset manager's slot)
};
extern UnknownBFAEE0Class *Unknown_BFAEE0;				// NAME 1 [Unknown_BFAEE0]: 0xBFAEE0

// fontmanager.h's FontManagerClass at 0xBE67C4 (its header cannot be included next to sclient.h: workflow.md 3.3)
class FontSys : public RenderObjClass { public: void Change_Text(const char *text, int unknown); };	// NAME 3: the client's profile label 'FontSys::Change_Text' (0x6CDBC0)  // NAME 1 [text, unknown]: as loadingview.cpp's
class UIAvatarCustFontManagerClass { public: FontSys *Unknown_6D25D0(const char *name, RenderObjClass *object); };	// NAME 1: neutral (D10): fontmanager.h's Get_Style  // NAME 1 [name, object]: as loadingview.cpp's
extern UIAvatarCustFontManagerClass *Unknown_BE67C4;	// NAME 1 [Unknown_BE67C4]: 0xBE67C4

// money.cpp's comparisons (cdecl, not rebuilt)
bool operator==(const MoneyClass &a, const MoneyClass &b);	// 0x4F0600  // NAME 1 [a, b]: neutral
bool operator>=(const MoneyClass &a, const MoneyClass &b);	// 0x4F0740  // NAME 1 [a, b]: neutral

// The window's parts: the mesh's bone and its buy button (0xB7A3xx-0xB7A460, as far as 0x550A10 reads it)
struct UIAvatarCustPartStruct	// NAME 1: neutral (D10)
{
	const char *Bone;		// NAME 2: what 0x550A10 hands to Get_Bone_Index
	const char *Button;		// NAME 1: neutral ('metals_buy_avatar00')
};
static UIAvatarCustPartStruct UIAvatarCustMetals[3] = {	// NAME 1: neutral (D10)
	{ "metal00", "metals_buy_avatar00" },
	{ "metal01", NULL },
	{ "metal02", NULL },
};

// avatar.cpp's avatar, as far as this file calls it (the window's +0xBC)
class UIAvatarCustAvatarClass	// NAME 1: neutral (D10)
{
public:
	void Unknown_780790(int part, int value);			// 0x780790 (0x550970: 1-4 for the window's parts 3-6)  // NAME 1 [part, value]: neutral
	void Unknown_780810(SClientUnknown1338Class *unknown);	// 0x780810 (0x550970 passes the client's Unknown1338)  // NAME 1 [unknown]: neutral

	char Unknown00[0xF8];								// +0x00
	AvatarDescClass *Get_Desc() { return(&Unknown0F8); }	// NAME 1: neutral (D10)
	AvatarDescClass Unknown0F8;							// +0xF8 (the new look: 0x551BF0 prices it against the window's +0xC0)
};

template<class C> inline C *Unknown_Find_Valid(DeltaCompoundClass *root, const char *name) { NameClass n(name); C *t = Unknown_Find_Field<C>(root, n); if (t && t->Unknown70) return(t); return(NULL); }	// NAME 1: neutral (D10)
template<class C> inline C *Unknown_Find_Changed(DeltaCompoundClass *root, const char *name) { NameClass n(name); C *t = Unknown_Find_Field<C>(root, n); if (t && t->Unknown70 > 1) return(t); return(NULL); }	// NAME 1: neutral (D10)

// What the window holds at +0x368..+0x378 (its destructor 0x585BF0 not inlined; 0x54F2B0 deletes them)
class UIAvatarCustUnknown368Class	// NAME 1: neutral (D10)
{
public:
	~UIAvatarCustUnknown368Class();						// 0x585BF0
};

// The avatar customisation window (ui_avatar_cust.cpp's __FILE__), as far as this file's functions so far use it
class StarbaseAvatarCreationInterface : public InterfaceClass	// NAME 3: the client's RTTI (0xB7AC88: SignalBaseClass::MemberCallback<GadgetClass,StarbaseAvatarCreationInterface,...>, the FlashingGadgetEvent handler 0x551320's)
{
public:
	void Unknown_54E3C0();								// 0x54E3C0
	void Unknown_54E610();								// 0x54E610 (0x54E3C0, then the gadgets hidden while the view's +0x34 is active)
	void Unknown_54F2B0();								// 0x54F2B0 (+0x368..+0x378 deleted)
	void Unknown_54F6F0(int a, int b, int c, int d);	// 0x54F6F0 (0x550550: 1, 0, 0, 0)  // NAME 1 [a, b, c, d]: neutral
	void Unknown_550550(int dir);						// 0x550550 (the zoom +0x314 a thirtieth out (1) or in (2), between 0.5 and +0x318; the zoom buttons +0x238 / +0x23C)  // NAME 1 [dir]: neutral
	void Unknown_550670();								// 0x550670 (the mode and part buttons' state)
	void Unknown_550970(int value);						// 0x550970  // NAME 1 [value]: neutral
	void Unknown_551BF0();								// 0x551BF0 (the cost of the changes: recustomization_cost.cpp's 0x8493F0)
	void Unknown_550A10();								// 0x550A10 (the three metal models turn, the mesh's bones follow)
	void Unknown_54EC30();								// 0x54EC30 (0x551A80, on the confirmation's first button)
	void Unknown_5513E0(const char *name);				// 0x5513E0 (the texture of that name onto the mesh's COLOR_PALETTE)  // NAME 1 [name]: neutral
	void Unknown_551520(bool any);						// 0x551520 (the credits and cost texts)  // NAME 1 [any]: neutral
	void Unknown_551900();								// 0x551900 (the confirmation box)
	void Unknown_551A80();								// 0x551A80 (the boxes' answers)
	void Unknown_551B20(bool on);						// 0x551B20 (every gadget shown or hidden)  // NAME 1 [on]: neutral
	void Unknown_551C40(const RecustomizationCostClass &cost);	// 0x551C40 (the costs kept, +0x3CC set)  // NAME 1 [cost]: neutral
	void Unknown_551C80();								// 0x551C80 (STARBASE_REQUEST, action 10)

	UIMeshClass *Unknown084;							// +0x84 (the window's mesh)
	char Unknown88[0xB0 - 0x88];						// +0x88
	int Unknown0B0;										// +0xB0 (the mode: 1, 2)
	SBaseViewClass *Unknown0B4;							// +0xB4 (its Client's Unknown1338; 0x551A80 calls 0x69F440)
	GadgetClass *Unknown0B8;							// +0xB8
	UIAvatarCustAvatarClass *Unknown0BC;				// +0xBC
	AvatarDescClass Unknown0C0;							// +0xC0 (the old look: 0x551BF0)
	GadgetClass *Unknown1CC[14];						// +0x1CC ([0] on in mode 1, [1] in mode 2)
	GadgetClass *Unknown204[15];						// +0x204 ([10]-[12]: parts 0-2)
	GadgetClass *Unknown240[11];						// +0x240 ([4]-[7]: parts 3-6; [8]-[10] the metal models' gadgets)
	Matrix3D Unknown26C;								// +0x26C (the models' turning frames)
	Matrix3D Unknown29C;								// +0x29C
	Matrix3D Unknown2CC;								// +0x2CC
	char Unknown2FC[0x308 - 0x2FC];						// +0x2FC
	UIAvatarCustGadgetHolderStruct *Unknown308;			// +0x308
	char Unknown30C[0x314 - 0x30C];						// +0x30C
	float Unknown314;									// +0x314 (the zoom: 0x550550)
	float Unknown318;									// +0x318 (the zoom's largest)
	UIAvatarCustGadgetHolderStruct *Unknown31C;			// +0x31C
	char Unknown320[0x358 - 0x320];						// +0x320
	int Unknown358;										// +0x358 (the part, 0-6)
	int Unknown35C;										// +0x35C
	bool Unknown360;									// +0x360
	bool Unknown361;									// +0x361 (0x54E610: set while the gadgets are hidden)
	char Unknown362[0x368 - 0x362];						// +0x362
	UIAvatarCustUnknown368Class *Unknown368;			// +0x368
	UIAvatarCustUnknown368Class *Unknown36C;			// +0x36C
	UIAvatarCustUnknown368Class *Unknown370;			// +0x370
	UIAvatarCustUnknown368Class *Unknown374;			// +0x374
	UIAvatarCustUnknown368Class *Unknown378;			// +0x378
	char Unknown37C[0x380 - 0x37C];						// +0x37C
	MoneyClass Unknown380;								// +0x380 (the credits: the AuxData's Money)
	MoneyClass Unknown388;								// +0x388 (the changes' cost)
	RecustomizationCostClass Unknown390;				// +0x390 (the costs per slot: 0x551C40)
	bool Unknown3CC;									// +0x3CC (set once the costs are in)
};

// 0x54E610
void StarbaseAvatarCreationInterface::Unknown_54E610()
{
	Unknown_54E3C0();
	if (Unknown0B4->Unknown34->Active) {
		if (!Unknown361) {
			Unknown361 = true;
			Unknown_551B20(false);
			return;
		}
	}
	if (!Unknown0B4->Unknown34->Active && Unknown361) {
		Unknown361 = false;
		Unknown_551B20(true);
		Unknown_550670();
	}
}

// NOT EXACT: register (0.77, 273 bytes against 0x112: the client loads Client's AvatarID through eax / ecx where VC6 here takes edx, and the packet's look into esi; attempts.py show 0x54EC30)
// 0x54EC30
void StarbaseAvatarCreationInterface::Unknown_54EC30()
{
	SClient *client = Client->As_Space_Client();
	client->Unknown_74C530(Client->AvatarID);
	client->Unknown_74C3B0(Client->AvatarID, *Unknown0BC->Get_Desc());
	UIStarbaseClass *starbase = ((SBaseViewClass *)client->Unknown135C)->Get_Starbase().Peek();
	if (starbase) {
		starbase->Unknown_6435E0();
	}
	client->Send_Packet(&RecustomizeAvatarDonePacketClass(client->Unknown1138, *Unknown0BC->Get_Desc()));
}

// 0x54F2B0
void StarbaseAvatarCreationInterface::Unknown_54F2B0()
{
	if (Unknown368) {
		delete Unknown368;
		Unknown368 = NULL;
	}
	if (Unknown36C) {
		delete Unknown36C;
		Unknown36C = NULL;
	}
	if (Unknown370) {
		delete Unknown370;
		Unknown370 = NULL;
	}
	if (Unknown374) {
		delete Unknown374;
		Unknown374 = NULL;
	}
	if (Unknown378) {
		delete Unknown378;
		Unknown378 = NULL;
	}
}

// 0x550550
void StarbaseAvatarCreationInterface::Unknown_550550(int dir)
{
	float step = Unknown314 * 0.033333335f;
	if (dir == 1) {
		Unknown314 = WWMath::Max(Unknown314 - step, 0.5f);
		if (Unknown314 == 0.5f) {
			Unknown204[13]->Unknown_40(false);
		}
		Unknown204[14]->Unknown_40(true);
	} else if (dir == 2) {
		Unknown314 = WWMath::Min(step + Unknown314, Unknown318);
		if (Unknown314 == Unknown318) {
			Unknown204[14]->Unknown_40(false);
		}
		Unknown204[13]->Unknown_40(true);
	}
	Unknown_54F6F0(1, 0, 0, 0);
	Unknown_551BF0();
}

// 0x550670
void StarbaseAvatarCreationInterface::Unknown_550670()
{
	if (Unknown0B0 == 1) {
		Unknown1CC[0]->Unknown2C800 = true;
		Unknown1CC[0]->Unknown_6603A0(0, NULL);
	} else {
		Unknown1CC[0]->Unknown2C800 = false;
		Unknown1CC[0]->Unknown_6607D0();
	}
	if (Unknown0B0 == 2) {
		Unknown1CC[1]->Unknown2C800 = true;
		Unknown1CC[1]->Unknown_6603A0(0, NULL);
	} else {
		Unknown1CC[1]->Unknown2C800 = false;
		Unknown1CC[1]->Unknown_6607D0();
	}
	if (Unknown358 == 0) {
		Unknown204[10]->Unknown_6607D0();
		Unknown204[10]->Unknown2C800 = true;
		Unknown204[10]->Unknown_6603A0(0, NULL);
	} else {
		Unknown204[10]->Unknown2C800 = false;
		Unknown204[10]->Unknown_6607D0();
	}
	if (Unknown358 == 1) {
		Unknown204[11]->Unknown2C800 = true;
		Unknown204[11]->Unknown_6603A0(0, NULL);
	} else {
		Unknown204[11]->Unknown2C800 = false;
		Unknown204[11]->Unknown_6607D0();
	}
	if (Unknown358 == 2) {
		Unknown204[12]->Unknown2C800 = true;
		Unknown204[12]->Unknown_6603A0(0, NULL);
	} else {
		Unknown204[12]->Unknown2C800 = false;
		Unknown204[12]->Unknown_6607D0();
	}
	if (Unknown358 == 3) {
		Unknown240[4]->Unknown_6607D0();
		Unknown240[4]->Unknown2C800 = true;
		Unknown240[4]->Unknown_6603A0(0, NULL);
	} else {
		Unknown240[4]->Unknown2C800 = false;
		Unknown240[4]->Unknown_6607D0();
	}
	if (Unknown358 == 4) {
		Unknown240[5]->Unknown2C800 = true;
		Unknown240[5]->Unknown_6603A0(0, NULL);
	} else {
		Unknown240[5]->Unknown2C800 = false;
		Unknown240[5]->Unknown_6607D0();
	}
	if (Unknown358 == 5) {
		Unknown240[6]->Unknown2C800 = true;
		Unknown240[6]->Unknown_6603A0(0, NULL);
	} else {
		Unknown240[6]->Unknown2C800 = false;
		Unknown240[6]->Unknown_6607D0();
	}
	if (Unknown358 == 6) {
		Unknown240[7]->Unknown2C800 = true;
		Unknown240[7]->Unknown_6603A0(0, NULL);
	} else {
		Unknown240[7]->Unknown2C800 = false;
		Unknown240[7]->Unknown_6607D0();
	}
}

// 0x550970
void StarbaseAvatarCreationInterface::Unknown_550970(int value)
{
	if (Unknown0B0 != 2) {
		return;
	}
	Unknown35C = value;
	switch (Unknown358) {
		case 3: Unknown0BC->Unknown_780790(1, value); break;
		case 4: Unknown0BC->Unknown_780790(2, value); break;
		case 5: Unknown0BC->Unknown_780790(3, value); break;
		case 6: Unknown0BC->Unknown_780790(4, value); break;
	}
	Unknown0BC->Unknown_780810(Unknown0B4->Get_Client()->Unknown1338);
	Unknown_551BF0();
}

// NOT EXACT: register (0.71, 1713 bytes against 0x6A8: the frame time's second global loads into ebp, pushed early, and the copies' registers rotate from there; attempts.py show 0x550A10)
// 0x550A10
void StarbaseAvatarCreationInterface::Unknown_550A10()
{
	Unknown_Find_Field<DeltaTemplateClass<float> >(NULL, *(NameClass *)this); Unknown_Find_Field<DeltaTemplateClass<float> >(NULL, *(NameClass *)this); Unknown_Find_Field<DeltaTemplateClass<float> >(NULL, *(NameClass *)this); Unknown_Find_Field<DeltaTemplateClass<float> >(NULL, *(NameClass *)this); Unknown_Find_Field<DeltaTemplateClass<float> >(NULL, *(NameClass *)this);	// dead finds (D130; not the client's): they spend VC6's inline budget, so the third Rotate_Y stays out of line as the client's (matrix3d.h's copy 0x551270; four were enough before 0x5513E0-0x551C80 joined the file, with four now it is inlined)
	if (Unknown0B0 != 2) {
		return;
	}
	float time = WW3D::Get_Frame_Time() * 0.001f;
	float slow = time * 0.3f;
	float fast = time + time;
	RefCountPtr<RenderObjClass> model = Unknown_497300(Unknown240[8]->Unknown30);
	if (model) {
		Matrix3D tm = model->Get_Transform();
		if (Unknown35C == 0) {
			Unknown26C.Rotate_Y(fast);
		} else {
			Unknown26C.Rotate_Y(slow);
		}
		int bone = Unknown084->Peek_Object()->Get_Bone_Index(UIAvatarCustMetals[0].Bone);
		Unknown084->Peek_Object()->Capture_Bone(bone);
		Unknown084->Peek_Object()->Control_Bone(bone, Unknown26C);
		Matrix3D newtm(Unknown26C);
		newtm[0][3] = tm[0][3];
		newtm[1][3] = tm[1][3];
		newtm[2][3] = tm[2][3];
		model->Set_Transform(newtm);
		Unknown240[8]->Unknown_6608F0();
	}
	model = Unknown_497300(Unknown240[9]->Unknown30);
	if (model) {
		Matrix3D tm = model->Get_Transform();
		if (Unknown35C == 1) {
			Unknown29C.Rotate_Y(fast);
		} else {
			Unknown29C.Rotate_Y(slow);
		}
		int bone = Unknown084->Peek_Object()->Get_Bone_Index(UIAvatarCustMetals[1].Bone);
		Unknown084->Peek_Object()->Capture_Bone(bone);
		Unknown084->Peek_Object()->Control_Bone(bone, Unknown29C);
		Matrix3D newtm(Unknown29C);
		newtm[0][3] = tm[0][3];
		newtm[1][3] = tm[1][3];
		newtm[2][3] = tm[2][3];
		model->Set_Transform(newtm);
		Unknown240[9]->Unknown_6608F0();
	}
	model = Unknown_497300(Unknown240[10]->Unknown30);
	if (model) {
		Matrix3D tm = model->Get_Transform();
		float angle = slow;
		if (Unknown35C == 2) {
			angle = fast;
		}
		Unknown2CC.Rotate_Y(angle);
		int bone = Unknown084->Peek_Object()->Get_Bone_Index(UIAvatarCustMetals[2].Bone);
		Unknown084->Peek_Object()->Capture_Bone(bone);
		Unknown084->Peek_Object()->Control_Bone(bone, Unknown2CC);
		Matrix3D newtm(Unknown2CC);
		newtm[0][3] = tm[0][3];
		newtm[1][3] = tm[1][3];
		newtm[2][3] = tm[2][3];
		model->Set_Transform(newtm);
		Unknown240[10]->Unknown_6608F0();
	}
}

// 0x5513E0
void StarbaseAvatarCreationInterface::Unknown_5513E0(const char *name)
{
	RefCountPtr<RenderObjClass> mesh = Unknown_497300(Unknown084->Peek_Object());
	RefCountPtr<RenderObjClass> palette = Create_Get(mesh->Get_Sub_Object_By_Name("COLOR_PALETTE"));
	TextureClass *texture = Unknown_BFAEE0->Get_Texture(name, 0, 0, 1);
	if (texture) {
		Unknown_495FD0(palette.Peek(), texture, 0);
		texture->Release_Ref();
	}
}

// 0x551520
void StarbaseAvatarCreationInterface::Unknown_551520(bool any)
{
	DeltaCompoundClass *aux = (DeltaCompoundClass *)Client->As_Space_Client()->AuxData;
	DeltaTemplateClass<MoneyClass> *money;
	if (any) {
		money = Unknown_Find_Valid<DeltaTemplateClass<MoneyClass> >(aux, "Money");
	} else {
		money = Unknown_Find_Changed<DeltaTemplateClass<MoneyClass> >(aux, "Money");
	}
	if (money) {
		Unknown380 = money->Value;
		FontSys *text = Unknown_BE67C4->Unknown_6D25D0("credits_buy_avatar00", Unknown084->Peek_Object());
		if (text) {
			std::string line("Credits: ");
			line += Unknown380.Unknown_4F00C0(true);
			text->Change_Text(line.c_str(), 0);
		}
	}
	FontSys *text = Unknown_BE67C4->Unknown_6D25D0("cost_buy_avatar00", Unknown084->Peek_Object());
	if (text) {
		std::string line("Cost: ");
		line += Unknown388.Unknown_4F00C0(true);
		text->Change_Text(line.c_str(), 0);
	}
}

// 0x551900
void StarbaseAvatarCreationInterface::Unknown_551900()
{
	if (Unknown388 == MoneyClass()) {
		View->Get_Unknown34()->Unknown_588A70("NO_CHANGES", 0, NULL, 0x140, 0, 0);
	} else if (Unknown380 >= Unknown388) {
		char text[128];
		_snprintf(text, sizeof(text), "Total changes cost %s credits. Proceed with update?", Unknown388.Unknown_4F00C0(false).c_str());
		View->Get_Unknown34()->Unknown_588A70("AVATAR_CONFIRMATION", 0, text, 0x140, 0, 0);
	} else {
		View->Get_Unknown34()->Unknown_588A70("NEED_FUNDS", 0, NULL, 0x140, 0, 0);
	}
}

// 0x551A80
void StarbaseAvatarCreationInterface::Unknown_551A80()
{
	if (View->Get_Unknown34()->Is_Active()) {
		int button = View->Get_Unknown34()->Unknown_5890B0("AVATAR_CONFIRMATION");
		if (button) {
			if (button == 1) {
				Unknown_54EC30();
				Unknown0B4->Unknown_69F440();
			}
			View->Get_Unknown34()->Unknown_5C();
		}
		if (View->Get_Unknown34()->Unknown_5890B0("NEED_FUNDS")) {
			View->Get_Unknown34()->Unknown_5C();
		}
		if (View->Get_Unknown34()->Unknown_5890B0("NO_CHANGES")) {
			View->Get_Unknown34()->Unknown_5C();
		}
	}
}

// 0x551B20
void StarbaseAvatarCreationInterface::Unknown_551B20(bool on)
{
	int i;
	for (i = 0; i < 14; i++) {
		if (Unknown1CC[i]) Unknown1CC[i]->Unknown_40(on);
	}
	for (i = 0; i < 15; i++) {
		if (Unknown204[i]) Unknown204[i]->Unknown_40(on);
	}
	for (i = 0; i < 11; i++) {
		if (Unknown240[i]) Unknown240[i]->Unknown_40(on);
	}
	if (Unknown0B8) Unknown0B8->Unknown_40(on);
	if (Unknown31C) Unknown31C->Unknown00->Unknown_40(on);
	if (Unknown308) Unknown308->Unknown00->Unknown_40(on);
}

// 0x551BF0
void StarbaseAvatarCreationInterface::Unknown_551BF0()
{
	if (Unknown3CC) {
		AvatarDescClass *newavatar = &Unknown0BC->Unknown0F8;
		Unknown388 = Unknown390.Unknown_8493F0(&Unknown0C0, newavatar);
	}
}

// 0x551C40
void StarbaseAvatarCreationInterface::Unknown_551C40(const RecustomizationCostClass &cost)
{
	Unknown390.Data = cost.Data;
	Unknown3CC = true;
}

// 0x551C80
void StarbaseAvatarCreationInterface::Unknown_551C80()
{
	SClient *client = Client->As_Space_Client();
	int id = client->Unknown1138;
	client->Send_Packet(&StarbaseRequestPacketClass(id, 10, 0));
}

/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\ui_avatar_cust.cpp (its __FILE__, 0x551320's FAIL_IFs on lines 2210 / 2214)
**
**	The avatar customisation window, StarbaseAvatarCreationInterface (the client's RTTI: the type
**	descriptor of its FlashingGadgetEvent callback, 0xB7AC88, sits in this file's literals): so far
**	0x550670, 0x550970 and 0x550A10 (NOT EXACT; matrix3d.h's out-of-line Rotate_Y 0x551270 comes with
**	it) (TG4, claim #63), and 0x5513E0-0x551C80 after the FAIL_IFs of 0x551320 (TK6, claim #87): the
**	palette texture, the credits and cost texts, the confirmation box and its answers, the gadgets
**	shown, the cost of the changes, the costs kept, the STARBASE_REQUEST. The file's evidence: they sit
**	between ui_attrb_skills.cpp's last functions (0x54B370) and ui_avatar_dialog.cpp's first (0x551FB0,
**	its asserts from line 85); the literals of 0x5513E0-0x551C80 ('COLOR_PALETTE' 0xB7AA2C, 'Cost: '
**	0xB7ABC0 to 'NO_CHANGES' 0xB7AC70) lie in this file's block of literals, around its __FILE__
**	(0xB7AB88), before the callback's type name and ui_avatar_dialog.cpp's first literal (0xB7AD0C).
**	Before 0x550670 (TK15, claims #119 / #120): 0x54E610 (the gadgets hidden while the view's +0x34 is
**	active), 0x54EC30 (NOT EXACT: the new look sent, RECUSTOMIZE_AVATAR_DONE; it brings SBaseViewClass's
**	Get_Starbase 0x54EDA0 and the packet's implicit destructor 0x54EDD0 out of line), 0x54F2B0 (+0x368..+0x378
**	deleted) and 0x550550 (the zoom). The file starts at 0x54B760, its PermStringInitClass static (0xBE5B34,
**	atexit 0x54B790): 0x54B370 is ui_attrb_skills.cpp's (its FAIL_IF, line 577), and 0x54B740 / 0x54B750 are
**	adjustor thunks to 0x549800 (ui_attrb_skills.cpp's class); the constructor 0x54B7B0 ('buy_avatar00') and
**	the destructor 0x54BAB0 follow.
**	The class is declared only as far as these use it; the rest of the file (about 2,200 lines before
**	the FAIL_IFs, 0x54B7B0-0x550670) is not rebuilt. +0xC0 is the old look (an AvatarDescClass, 0x10C
**	bytes, up to the gadgets at +0x1CC), the avatar's +0xF8 the new one.
*/
