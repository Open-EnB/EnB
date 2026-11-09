// Earth & Beyond client (reconstruction): app\client\ui_attrb_skills.cpp, the character info window's attributes tab (the
// client's file per its asserts, 'e:\build\beta\G\app\Client\ui_attrb_skills.cpp'). Notes at the end of the file.
#include "sclient.h"
#include "rendobj.h"
#include "..\..\wwlib\ref_ptr.h"
#include "gadget.h"
#include "gd_slider.h"
#include "fontmanager.h"
#include "..\shared\g_assert.h"
#include "ui_attrb_skills.h"

extern FontManagerClass *Unknown_BE67C4;			// NAME 1 [Unknown_BE67C4]: 0xBE67C4

#include "gd_attrb_skills.h"











// What the skill base's 0x62D710 returns, as far as this file calls it
class SkillDataPeekClass  // NAME 1: neutral (D10)
{
public:
	bool Unknown_6A5440();								// 0x6A5440
};
class SlotDataClass { public: int Unknown_6A7990(); void Unknown_6A7860(); int Unknown_6A7A00(unsigned int index); char Unknown00[0x50]; };	// +0x00; 0x6A7990 (0x54A200: zero, then 0x6A7860) / 0x6A7860 / 0x6A7A00 (+0x44's element, else -1)  // NAME 1 [index]: neutral
class SkillDataClass : public SlotDataClass  // NAME 3: skill_data.cpp
{
public:
	SkillDataPeekClass Unknown50;						// +0x50
};

// intrlist.cpp's list (InterfaceClass +0x58; intrface.h declares it only under __INTRFACE_MEMBERS), as far as this file calls it
class InterfaceUnknown58Class  // NAME 1: neutral (D10)
{
public:
	void Unknown_686CC0(RenderObjClass *robj, SceneClass *scene);	void Unknown_686DD0(RenderObjClass *robj);	// 0x686CC0 / 0x686DD0
};

// 0x549640
UIAttrbSkillsClass::UIAttrbSkillsClass(SClient *client) :
	Unknown62D1F0Class(client),
	UnknownD4(NULL),
	UnknownD8(NULL),
	UnknownDC(NULL),
	UnknownE0(NULL),
	UnknownE4(false),
	UnknownE8(0),
	UnknownEC(-1),
	UnknownF0(0),
	UnknownF4(0),
	UnknownF8(0)
{
}

// 0x549830
UIAttrbSkillsClass::~UIAttrbSkillsClass()
{
	((InterfaceUnknown58Class *)&Unknown4C[0x58 - 0x4C])->Unknown_686DD0(InterfaceMesh.Peek());	// (InterfaceClass +0x58)
	Unknown_54B050();
	UnknownFC = NULL;
	if (UnknownD4) {
		UnknownD4->Unknown_66E460(RefCountPtr<RenderObjClass>(), 0, 0);
		UnknownD4 = NULL;
	}
	if (UnknownD8) {
		UnknownD8->Unknown_66E460(RefCountPtr<RenderObjClass>(), 0, 0);
		UnknownD8 = NULL;
	}
	UnknownDC = NULL;
	UnknownE0 = NULL;
	Unknown_BE67C4->Remove(InterfaceMesh.Peek());
	Unknown_BE67BC->Unload("fa_att.ini");
}

// What InterfaceClass's +0x40 points to, as far as this file calls it (as ui_cinfo.cpp declares it)
class LineDisplayClientClass  // NAME 1: neutral (D10)
{
public:
	virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C();
	virtual void Unknown_10(); virtual void Unknown_14();
	virtual SClient *As_Space_Client();					// +0x18 (as ui_manu.cpp's)  // NAME 3: the FAIL_IF's in ui_manu.cpp
};
#include "..\audio\soundlib.h"
int Unknown_82F350(int race, int prof);				// chartemplate.cpp's 0x82F350 (race and profession to the class index)  // NAME 1: neutral (D10), as chartemplate.cpp's
#include "ui_attrb.h"














#include "assetmgr.h"
#include "gd_robj.h"
#include "unknown_675120.h"







// 0x549AA0
void UIAttrbSkillsClass::Unknown_54()
{
	InterfaceMesh = Create_NEW(WW3DAssetManager::Get_Instance()->Create_Render_Obj("TSI12B"));
	FAIL_IF(!InterfaceMesh) return;

	((InterfaceUnknown58Class *)&Unknown4C[0x58 - 0x4C])->Unknown_686CC0(InterfaceMesh.Peek(), (SceneClass *)Unknown44);	// (InterfaceClass +0x58)
	Unknown_BE67BC->Load("fa_att.ini");
	Unknown_BE67C4->Add(InterfaceMesh.Peek());

	UnknownC0 = Create_NEW(new Unknown546A80Class(Client->As_Space_Client(), InterfaceMesh));
	UnknownC0->Unknown_5417D0(this);
	UnknownC0->Unknown_54();

	UnknownFC = Create_NEW(new Unknown548D60Class(Client->As_Space_Client()));
	UnknownFC->Unknown_5417D0(this);
	UnknownFC->Unknown_54();

	UnknownD4 = new GadgetSlider(false);
	UnknownD4->Unknown_66E460(Create_Peek(InterfaceMesh.Peek()), false, 1);
	Unknown64.Unknown_6870E0(UnknownD4, 0x138A, View, 1, 0);

	UnknownD8 = new GadgetSlider(false);
	UnknownD8->Unknown_66E460(Create_Peek(InterfaceMesh.Peek()), false, 0);
	Unknown64.Unknown_6870E0(UnknownD8, 0x138B, View, 1, 0);

	RefCountPtr<RenderObjClass> robj;
	robj = Create_Get(InterfaceMesh->Get_Sub_Object_By_Name("MINUS"));


	G_ASSERT(robj);
	UnknownE0 = new ButtonGadgetClass(robj.Peek(), 5, 5);
	Unknown64.Unknown_6870E0(UnknownE0, 0x1388, View, 1, 0);
	GadgetClass::Set_Tooltip(UnknownE0, "UI_ATTRB_SKILLS", "MINIMIZE");



	robj = Create_Get(InterfaceMesh->Get_Sub_Object_By_Name("PLUS_00"));
	G_ASSERT(robj);
	UnknownDC = new ButtonGadgetClass(robj.Peek(), 5, 5);
	Unknown64.Unknown_6870E0(UnknownDC, 0x1389, View, 1, 0);
	GadgetClass::Set_Tooltip(UnknownDC, "UI_ATTRB_SKILLS", "MAXIMIZE");

	RefCountPtr<RenderObjClass> frame = Create_Get(InterfaceMesh->Get_Sub_Object_By_Name("new_frame"));
	Unknown_62F4D0(frame, 0x138C);
	Unknown62D1F0Class::Unknown_54();
}












#include "..\shared\auxdata\auxdata.h"
#include "aux_roots.h"				// (after auxdata.h, as ui_buff.cpp)
// auxdata.h's find, set (+0x70) or changed since (+0x70 above 1), else NULL; the dead size() read spends the inline budget so the find is called out of line, as the client's (ui_missn.cpp's 0x5C4C90 needs six)
template<class C> inline C *Unknown_Find_Set_Field(DeltaCompoundClass *root, const NameClass &name) { root->Children.size(); C *t = Unknown_Find_Field<C>(root, name); return((t && t->Unknown70) ? t : NULL); }	// NAME 1: neutral (D10)
template<class C> inline C *Unknown_Find_Changed_Field(DeltaCompoundClass *root, const NameClass &name) { root->Children.size(); C *t = Unknown_Find_Field<C>(root, name); return((t && t->Unknown70 > 1) ? t : NULL); }	// NAME 1: neutral (D10)
// 0x54A640
// NOT EXACT: frame (similarity 0.68: the client's frame is 0x54, one NameClass slot shared by the two finds and the font in arg's slot; this keeps two NameClass slots and the font apart, 0x8C; the code is the client's)
void UIAttrbSkillsClass::Unknown_54A640(bool arg)
{
	DeltaCompoundClass *aux_data = (DeltaCompoundClass *)Client->As_Space_Client()->AuxData;
	DeltaTemplateClass<int> *points;
	if (arg) {
		NameClass name("RPGInfo.SkillPoints"); points = Unknown_Find_Set_Field<DeltaTemplateClass<int> >(aux_data, name);
	} else {
		NameClass name("RPGInfo.SkillPoints"); points = Unknown_Find_Changed_Field<DeltaTemplateClass<int> >(aux_data, name);
	}
	if (!points) return;

	UnknownF4 = points->Value;
	char text[32];
	_snprintf(text, 32, "%d", UnknownF4);
	RefCountPtr<FontSys> font = Create_Peek(Unknown_BE67C4->Get_Style("SKILL_VAL_ATT", InterfaceMesh.Peek()));
	if (font) {
		font->Change_Text(text, 0);
	}
	if (UnknownFC->Active && UnknownEC >= 0) {
		Unknown_54B400();




	}
}

// 0x54A860
void UIAttrbSkillsClass::Unknown_54A860(int skill)
{
	UnknownEC = skill;
	Unknown_54B400();
	Unknown_54A900();
}



// 0x54ACE0
void UIAttrbSkillsClass::Unknown_54ACE0()
{
	bool active = Active;
	if (active) {
		Unknown62D1F0Class::Unknown_5C();
	}
	Unknown_54B050();
	unsigned id = ((ClientClass *)Client)->AvatarID; Unknown77E3F0Class *avatar = Client->As_Space_Client()->Unknown_74C840(id);
	if (avatar) {
		int index = Unknown_82F350(avatar->Unknown108, avatar->Unknown10C);
		SkillDataClass *list = Unknown_62D710();
		int count = list->Unknown_6A7990();
		for (int i = 0; i < count; i++) {
			int skill = list->Unknown_6A7A00(i);
			Unknown664500Class *gadget = new Unknown664500Class();
			gadget->Unknown_663DF0(this, skill, index);
			Unknown64.Unknown_6870E0(gadget, i + 5005, View, 1, 1);
			UnknownC4.push_back(gadget);
		}
		if (active) {
			Unknown62D1F0Class::Unknown_58();
		}
	}
}



































































































// 0x54A900
void UIAttrbSkillsClass::Unknown_54A900()
{
	for (std::vector<Unknown664500Class *>::iterator it = UnknownC4.begin(); it != UnknownC4.end(); it++) {
		(*it)->Unknown_6647F0();
	}
	Unknown_62D710();
	int skip = 0;
	int index = UnknownE8;
	int level = 10 - UnknownF0;
	int count = UnknownC4.size();
	for (int i = 0; i < UnknownF0 && index < count; i++) {
		if (skip == 0) {
			Unknown664500Class *gadget = UnknownC4[index];
			FAIL_IF(!gadget) continue;
			gadget->Unknown_28(&Layer);
			if (gadget->Unknown_6645A0(level, level < 9)) {
				skip = 1;
			}
			if (UnknownEC == index) {
				gadget->Unknown_664890(true);
			}
			index++;
		} else {
			skip--;
		}
		level++;
	}
	if (UnknownE4) {
		UnknownD4->Unknown_66FA90((float)(UnknownF8 + UnknownC4.size()), (float)UnknownF0, true);
	} else {
		UnknownD8->Unknown_66FA90((float)(UnknownF8 + UnknownC4.size()), (float)UnknownF0, true);
	}
}















#pragma auto_inline(off)		// (+0x60 calls it out of line: VC6 would inline it)
// 0x54AB20
void UIAttrbSkillsClass::Unknown_54AB20()
{
	if (Unknown_62D710()->Unknown50.Unknown_6A5440()) {
		int index = UnknownE8;
		int count = UnknownC4.size();
		for (int i = 0; i < UnknownF0 && index < count; i++, index++) {
			Unknown664500Class *gadget = UnknownC4[index];
			FAIL_IF(!gadget) continue;
			gadget->Unknown_A4(false);
		}
	}
}
#pragma auto_inline(on)










#include "local_interface_sort.h"
// 0x54A200
// NOT EXACT: register (similarity 0.71: the inlined 0x54B370 keeps the count in ecx and the gadget in edx, the client the reverse; 0x54B140 inlines it as the client does)
void UIAttrbSkillsClass::Unknown_58()
{
	if (!Active) {
		InterfaceSortScopeClass sort(this);
		Unknown62D1F0Class::Unknown_58();
		if (!Unknown_62D710()->Unknown_6A7990()) {
			Unknown_62D710()->Unknown_6A7860();
		}
		UnknownEC = -1;
		UnknownF0 = 5;
		UnknownE4 = false;
		UnknownDC->Unknown_40(true);
		UnknownE0->Unknown_40(false);
		UnknownD4->Unknown_40(false);
		UnknownD8->Unknown_40(true);
		UnknownC0->Unknown_58();
		Unknown_54B370();
		Unknown_54A900();
		Unknown_54A640(true);
	}
}









// 0x54A480
bool UIAttrbSkillsClass::Unknown_68(int key)
{
	switch (key) {
		case 5000:
			UnknownE4 = false;
			Unknown_54A900();
			break;
		case 5001:
			UnknownE4 = true;
			Unknown_54A900();
			break;
		case 5002:
		case 5003:
			break;
		default:
			if (key >= 5005) {
				SoundLibraryClass::Play_2D_Sound("fontclick.wav", 2);
				UnknownEC = key - 5005;
				Unknown_54B400();




				Unknown_54A900();
				return(true);
			}
	}
	return(Unknown62D1F0Class::Unknown_68(key));
}

#pragma auto_inline(off)		// (the destructor calls it out of line: VC6 would inline it)
// 0x54B050
void UIAttrbSkillsClass::Unknown_54B050()
{
	G_ASSERT(!Is_Active());


	for (std::vector<Unknown664500Class *>::iterator it = UnknownC4.begin(); it != UnknownC4.end(); it++) {
		Unknown664500Class *gadget = *it;
		FAIL_IF(!gadget) continue;
		gadget->Unknown_664430();
		Unknown64.Unknown_687150(gadget);
		GadgetClass::Destroy(gadget);
	}
	UnknownC4.clear();
	Unknown_BC();
}
#pragma auto_inline(on)


static inline float Slider_Value(GadgetSlider *slider) { return(slider->Value); }	// NAME 1: neutral (D10): the value through x87, as the client passes it
// 0x54B140
void UIAttrbSkillsClass::Unknown_54B140()
{
	bool slider = false;
	bool common = false;
	if (UnknownE4) {
		if (UnknownF0 == 10) return;
		UnknownF0++;
		if (UnknownF0 == 6) {
			slider = true;
		} else if (UnknownF0 == 10) {
			UnknownC0->Unknown_5C();
			common = true;
		}
	} else {
		if (UnknownF0 == 5) return;
		UnknownF0--;
		if (UnknownF0 == 5) {
			common = true;
		} else if (UnknownF0 == 9) {
			UnknownC0->Unknown_58();
			slider = true;
		}
	}
	if (slider) {
			if (UnknownE4) {
				UnknownD4->Unknown_A4(Slider_Value(UnknownD8), false);
			} else {
				UnknownD8->Unknown_A4(Slider_Value(UnknownD4), false);
			}
			UnknownDC->Unknown_40(false);
			UnknownE0->Unknown_40(false);
			UnknownD4->Unknown_40(false);
			UnknownD8->Unknown_40(false);
		Unknown_54A900();
		return;
	}
	if (common) {
			Unknown_54B370();
			UnknownDC->Unknown_40(!UnknownE4);
			UnknownE0->Unknown_40(UnknownE4);
			UnknownD4->Unknown_40(UnknownE4);
			UnknownD8->Unknown_40(!UnknownE4);
	}
	Unknown_54A900();
}





















// 0x54B370
void UIAttrbSkillsClass::Unknown_54B370()
{
	UnknownF8 = 0;
	int count = UnknownF0;
	for (std::vector<Unknown664500Class *>::reverse_iterator it = UnknownC4.rbegin(); it != UnknownC4.rend(); it++) {
		Unknown664500Class *gadget = *it;
		FAIL_IF(!gadget) return;
		if (gadget->Unknown110) {
			count--;
			UnknownF8++;
		}
		count--;
		if (count <= 0) {
			break;
		}
	}
}
// 0x54B400
void UIAttrbSkillsClass::Unknown_54B400()
{
	unsigned id = ((ClientClass *)Client)->AvatarID; Unknown77E3F0Class *avatar = Client->As_Space_Client()->Unknown_74C840(id);
	if (avatar) {
		int index = Unknown_82F350(avatar->Unknown108, avatar->Unknown10C);
		((Unknown548D60Class *)UnknownFC.Peek())->Unknown_548D60(Unknown_62D710(), Unknown_62D710()->Unknown_6A7A00(UnknownEC), index);
	}
}

// 0x54A3C0
void UIAttrbSkillsClass::Unknown_5C()
{
	if (Active) {
		UnknownFC->Unknown_5C();
		UnknownC0->Unknown_5C();
		Unknown62D1F0Class::Unknown_5C();
	}
}

// 0x54A400
void UIAttrbSkillsClass::Unknown_60()
{
	Unknown62D1F0Class::Unknown_60();
	Unknown_54B140();
	int value = UnknownE4 ? UnknownD4->Value : UnknownD8->Value;
	if (UnknownE8 != value) {
		UnknownE8 = value;
		Unknown_54A900();
	}
	Unknown_54A640(false);
	Unknown_54AB20();
}

// 0x54A5B0
void UIAttrbSkillsClass::Unknown_7C(UIViewClass *view, bool arg)
{
	if (view) {
		Layer.Set_Scene((SceneClass *)view->Unknown_50());
		Layer.Set_Camera((CameraClass *)view->Unknown_54());
	} else {
		Layer.Set_Scene(NULL);
		Layer.Set_Camera(NULL);
	}
	UnknownFC->Unknown_7C(view, false);
	UnknownC0->Unknown_7C(view, false);
	Unknown62D1F0Class::Unknown_7C(view, arg);
}

// 0x54ABE0
void UIAttrbSkillsClass::Unknown_54ABE0()
{
	int value = UnknownE4 ? UnknownD4->Value : UnknownD8->Value;
	if (UnknownE8 != value) {
		UnknownE8 = value;
		Unknown_54A900();
	}
}

// 0x54AC50
void UIAttrbSkillsClass::Unknown_B0()
{
}

// 0x54AC70
void UIAttrbSkillsClass::Unknown_CC()
{
	Unknown_54ACE0();
}

// 0x54AC90
void UIAttrbSkillsClass::Unknown_D0()
{
	for (std::vector<Unknown664500Class *>::iterator it = UnknownC4.begin(); it != UnknownC4.end(); it++) {
		Unknown664500Class *gadget = *it;
		if (gadget->Unknown_60()) {
			gadget->Unknown_664500();
		}
	}
}
/*
**	Notes (TB1, 2026-11-05)
**
**	The client's object 0x549640-~0x54B75F: the constructor 0x549640 first, right after ui_attrb.cpp's PermStringInitClass
**	static (0x5495F0 / atexit 0x549620, the end of ui_attrb.obj); its destructor 0x549830 (deleting 0x549800) and the
**	slots 0x549AA0.. carry ui_attrb_skills.cpp's asserts; the adjustor thunks 0x54B740 (-4) / 0x54B750 (-0x38) of the
**	deleting destructor close it. 0x5738B0 (ui_cinfo.cpp) makes it, the character info window's first tab. The
**	constructor's byte store to +0xC4 is the vector's allocator copied from an uninitialised temporary (VC6's
**	vector(const _A& = _A())), not an argument.
*/
