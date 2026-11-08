// radar_system.cpp: the pass over the client's objects each frame, for the gutter and the starmap (0x7BC770-0x7BDE19); notes at the end
#include <string>
#include "radar_system.h"
#include "sclient.h"
#include "client.h"
#include "co.h"
#include "co_data.h"
#include "radar_data.h"
#include "gutter_data.h"
#include "starmap_data.h"
#include "gutter_radar.h"
#include "mouse_targeting.h"
#include "radar_system_data_cache.h"
#include "nav_list_builder.h"
#include "map_pip.h"
#include "starmap_pip.h"
#include "option.h"
#include "targeting.h"
#include "wwmath.h"
#include "rendobj.h"
#include "..\audio\soundlib.h"
#include "scene.h"
#include "..\common\unknown_navlist.h"
#include "..\shared\g_assert.h"
#include "..\shared\auxdata\auxdata.h"
#include "..\shared\auxdata\delta_template.h"

// starmap_pip.cpp's window (what the PDA's 0x7C8DB0 returns): starmap_pip.h's StarmapPipClass







template<class C> inline C *Unknown_Find_Valid(DeltaCompoundClass *root, const NameClass &name) { C *t = Unknown_Find_Field<C>(root, name); if (t && t->Unknown70) return(t); return(NULL); }	// NAME 1: neutral (D10): as avatar_pip.cpp's

// @0x7BC770
RadarSystemClass::RadarSystemClass(SClient *client) :
	Client(client),
	Hover(this),
	Target(this),
	WarpPath(NULL),
	TargetPath(NULL),
	ScanRange(0.0f),
	GutterCount(0),
	Unknown4C(false),
	Unknown4D(false)
{














	G_ASSERT(Client);
	WarpPath = new NavListBuilderClass(client);
	TargetPath = new NavListBuilderClass(client);
}

// @0x7BC930
RadarSystemClass::~RadarSystemClass()
{
	delete WarpPath;
	WarpPath = NULL;
	delete TargetPath;
	TargetPath = NULL;
	Client = NULL;
}

// @0x7BCA40
void RadarSystemClass::Unknown_7BCA40(bool arg)
{
	WarpPath->Clear();
	TargetPath->Clear();
}

// NOT EXACT: register (ours keeps a zero register and this in edi; the client gutter in ebx, starmap in edi, this in ebp)
// @0x7BCA70
void RadarSystemClass::Unknown_7BCA70(GutterRadarClass *gutter, StarmapPipClass *starmap, const RefCountPtr<RefCountClass> &scene)
{
	GutterCount = 0;
	if (!gutter && !starmap) {
		return;
	}
	ClientObjectClass *avatar = Client->Get_Avatar();







	FAIL_IF(!avatar) return;
	DeltaTemplateClass<int> *range = Unknown_Find_Valid<DeltaTemplateClass<int> >((DeltaCompoundClass *)avatar->AuxData, "ScanRange");
	if (range) {
		ScanRange = (float)range->Value;
	} else {
		ScanRange = 0.0f;
	}
	RadarSystemDataCacheClass entry(avatar, 0.0f);
	Vector3 avatar_pos = avatar->Unknown_24();
	avatar->Unknown_30();
	MouseTargetClass mouse_target(Target);
	bool on_starmap = (starmap && starmap->Unknown_693020()) ? true : false;
	if (Unknown4C != on_starmap) {
		Unknown4C = on_starmap;
		Unknown_7BD860(NULL);
	}
	if (gutter) {
		gutter->Set_Targets(Target, Hover, Unknown4C ? NULL : &mouse_target);
	}
	if (starmap) {
		starmap->Unknown_7C0440(Hover, Unknown4C ? &mouse_target : NULL);
	}
	HashNodeClass<ClientObjectClass *> *node = Client->ObjectList.First_Valid();
	if (Unknown4D && gutter && !Unknown4C) {
		while (node) {
			ClientObjectClass *co = node->Get_Record();
















			FAIL_IF(!co) continue;
			entry.Set_Object(co);
			if (!entry.Get_Is_Targetable()) {
				gutter->Update(&entry);
				GutterDataClass *gdata = entry.Get_Gutter_Data(false);
				if (gdata) {
					gdata->Remove_From_Scene();
				}
			}
			node = node->Next_Valid();
		}
		node = Client->ObjectList.First_Valid();
	}
	while (node) {
		ClientObjectClass *co = node->Get_Record();











		FAIL_IF(!co) continue;
		entry.Set_Object(co);
		bool valid = entry.Get_Is_Targetable();
		if (co->Unknown_24().Is_Valid() && valid) {
			Unknown_7BDD50(entry, scene);
			bool show_gutter = (gutter && Unknown_7BDBF0(entry));
			GutterDataClass *gdata = entry.Get_Gutter_Data(show_gutter);
			bool label = Unknown_7BDC70(entry);
			bool show_starmap = (starmap && label);
			StarmapDataClass *sdata = entry.Get_Starmap_Data(label);
			if (show_gutter || show_starmap) {
				RadarDataClass *rdata = entry.Get_Radar_Data(true);





























				FAIL_IF(!rdata) {
					node = node->Next_Valid();
					continue;
				}
				rdata->Unknown_7BB570();
				if (show_gutter) {
					gutter->Update(&entry);
					GutterCount++;
				} else if (gdata) {
					gdata->Remove_From_Scene();
				}
			} else if (gdata) {
				gdata->Remove_From_Scene();
			}
			if (show_starmap) {
				starmap->Unknown_7C0620(&entry);
			} else if (sdata) {
				sdata->Remove_From_Scene();
			}
		}
		node = node->Next_Valid();
	}
	ClientObjectClass *hit = mouse_target.Get_Target();
	if (hit && (Client->Get_Avatar()->Get_Game_ID() == hit->Get_Game_ID())) {
		hit = NULL;
	}
	Unknown_7BD860(hit);
	if (gutter) {
		gutter->Clear_Mouse_Target();
	}
	if (starmap) {
		starmap->Unknown_7C0910(Target, WarpPath, TargetPath);
	}
	if (Unknown4D && mouse_target.Get_Target()) {
		static ClientObjectClass *last;
		if (last != mouse_target.Get_Target()) {
			last = mouse_target.Get_Target();
			std::string text;
			last->Unknown_6FF7A0(text);
			text += ": RenderObjName: ";
			RenderObjClass *robj = last->RenderObject;
			if (!robj) {
				text += "No Render Object";
			} else if (robj->Get_Name()) {
				text += robj->Get_Name();
			} else if (robj->Get_Base_Model_Name()) {
				text += robj->Get_Base_Model_Name();
			} else {
				text += "No Name";
			}
			Client->Unknown_74D990(0x17, (char *)text.c_str(), 0);
		}
	}
}

// @0x7BD4C0
ClientObjectClass *RadarSystemClass::Unknown_7BD4C0()
{
	HashNodeClass<ClientObjectClass *> *node;
	if (Target) {
		node = Client->ObjectList.Find(((ClientObjectClass *)Target)->Get_Game_ID())->Next_Valid();
	} else {
		node = Client->ObjectList.First_Valid();
	}
	while (node) {
		ClientObjectClass *co = node->Get_Record();
























		FAIL_IF(!co) return(NULL);
		StarmapDataClass *sdata = Get_Starmap_Data(co, false);
		if (sdata && sdata->Is_In_Scene()) {
			return(co);
		}
		node = node->Next_Valid();
	}
	return(NULL);
}

// @0x7BD5D0
ClientObjectClass *RadarSystemClass::Unknown_7BD5D0()
{
	HashNodeClass<ClientObjectClass *> *node;
	if (Target) {
		node = Client->ObjectList.Find(((ClientObjectClass *)Target)->Get_Game_ID())->Prev_Valid();
	} else {
		node = Client->ObjectList.Last_Valid();
	}
	while (node) {
		ClientObjectClass *co = node->Get_Record();









		FAIL_IF(!co) return(NULL);
		StarmapDataClass *sdata = Get_Starmap_Data(co, false);
		if (sdata && sdata->Is_In_Scene()) {
			return(co);
		}
		node = node->Prev_Valid();
	}
	return(NULL);
}


// @0x7BD6E0
void RadarSystemClass::Unknown_7BD6E0(ClientObjectClass *target)
{
	if (target == Target) {
		return;
	}
	WarpPath->Clear();
	if (Target) {
		RadarDataClass *rdata = Get_Radar_Data(Target, false);
		if (rdata) {
			rdata->Unknown_7BB8C0(0);
		}
	}
	Target = target;
	if (target) {
		RadarDataClass *rdata = Get_Radar_Data(Target, true);














		FAIL_IF(!rdata) {
			Target = NULL;
			return;
		}
		if ((ClientObjectClass *)Target == (ClientObjectClass *)Hover) {
			Unknown_7BD860(NULL);
		}
		rdata->Unknown_7BB8C0(2);
		GutterDataClass *gdata = Get_Gutter_Data(Target, false);
		if (gdata) {
			gdata->Set_Click_Indicator(false, NULL);
		}
		if (!TargetPath->Contains(((ClientObjectClass *)Target)->Get_Game_ID(), true)) {
			int mode = Client->Get_Mode();
			if (mode == 1) {
				if (!WarpPath->Unknown_7B7C80(Client->Get_Avatar(), Target)) {
					SoundLibraryClass::Play_2D_Sound("starmap_path_statement_1.wav", 2);
				}
			} else if (!WarpPath->Unknown_7B7EF0(ScanRange, Client->Get_Avatar(), Target)) {
				SoundLibraryClass::Play_2D_Sound("starmap_path_statement_1.wav", 2);
			}
		}
	}
}

// @0x7BD860
void RadarSystemClass::Unknown_7BD860(ClientObjectClass *hover)
{
	if (hover == Hover) {
		return;
	}
	if (Hover) {
		RadarDataClass *rdata = Get_Radar_Data(Hover, false);
		if (rdata) {
			rdata->Unknown_7BB8C0(0);
		}
	}
	Hover = hover;
	if (hover) {
		RadarDataClass *rdata = Get_Radar_Data(Hover, true);













		FAIL_IF(!rdata) {
			Hover = NULL;
			return;
		}
		rdata->Unknown_7BB8C0(1);
	}
}

// @0x7BD920
void RadarSystemClass::Unknown_7BD920(int index)
{
	SoundLibraryClass::Play_2D_Sound("starmap_pass_buoy_1.wav", 2);
	if (!TargetPath->Set_Index(index)) {
		TargetPath->Clear();
	}
}

// @0x7BD960
const NavigationListClass &RadarSystemClass::Unknown_7BD960()
{
	return(*TargetPath);
}

// @0x7BD980
bool RadarSystemClass::Unknown_7BD980()
{
	TargetingClass *targeting = Client->Targeting;
	if (targeting) {
		if (targeting->Unknown10) {
			return(false);
		}
	} else {







		G_ASSERT(targeting);
	}
	TargetPath->Unknown_7B7EF0(ScanRange, Client->Get_Avatar(), Target);
	*WarpPath = *TargetPath;
	return(true);
}

// @0x7BDBF0
bool RadarSystemClass::Unknown_7BDBF0(RadarSystemDataCacheClass &entry)
{
	if (Unknown4C && ((ClientObjectClass *)Hover == entry.ClientObject)) {
		return(true);
	}
	if (entry.ClientObject == (ClientObjectClass *)Target) {
		return(true);
	}
	if (!OptionClass::Instance->Get_Bool("Show_Self_Name") && entry.IsPlayer) {
		return(false);
	}
	return(entry.Get_Is_In_Range());
}

// @0x7BDC70
bool RadarSystemClass::Unknown_7BDC70(RadarSystemDataCacheClass &entry)
{
	ClientObjectClass *co = entry.ClientObject;
















	FAIL_IF(!co) return(false);
	if (entry.IsPlayer) {
		return(true);
	}
	if (entry.Get_Navigation_Data()) {
		return(true);
	}
	if (co->Unknown_6F5A40()) {
		RadarDataClass *rdata = entry.Get_Radar_Data(true);






		FAIL_IF(!rdata) return(false);
		int label = OptionClass::Instance->Get_Int("Starmap_Label");
		if (label != 0) {
			if (label == 1) {
				return(rdata->Unknown78);
			}
			return(true);
		}
	}
	return(false);
}

// @0x7BDD50
void RadarSystemClass::Unknown_7BDD50(RadarSystemDataCacheClass &entry, const RefCountPtr<RefCountClass> &scene)
{
	if (entry.Get_Navigation_Data()) {





		FAIL_IF(!entry.ClientObject) return;
		RenderObjClass *robj = entry.ClientObject->RenderObject;
		if (robj) {
			if (entry.Get_Is_In_Range()) {
				if (!robj->Peek_Scene()) {
					robj->Add((SceneClass *)scene.Peek());
				}
			} else if (robj->Peek_Scene()) {
				robj->Remove();
			}
		}
	}
}

// @0x7BDDF0
void RadarSystemWatchClass::Unknown_Watch_0C()
{
	ClientObjectClass *object = Object;
	if (Owner->Target == object) {
		Owner->Unknown_7BD6E0(NULL);
	}
	if (Owner->Hover == object) {
		Owner->Unknown_7BD860(NULL);
	}
}

/*
**	Notes
**
**	The object: 0x7BC770 (the constructor; the client's __FILE__ 0xB9F270, G_ASSERT 'Client' at line 64) to 0x7BDE19,
**	before radar_system_data_cache.cpp's 0x7BDFC0.
*/
