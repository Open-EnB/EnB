// Earth & Beyond client (reconstruction)
#include "sclient.h"
#include "..\shared\g_assert.h"
#include "..\..\ww3d2\rendobj.h"
#include "..\..\ww3d2\scene.h"
#include "..\..\ww3d2\ww3d.h"
#include "..\..\ww3d2\render2d.h"
unsigned int Unknown_Get_Ticks();	// 0x4A3B40, the aux-data clock (unknown_root.h's, not included: the client's object opens with no PermStringInit)
#include "mss.h"
#include <string.h>
#include "..\shared\auth_product.h"
#include "fontmanager.h"	// FontSys, FontManagerClass (0xBE67C4) and fontdatabase.h's FontStringFilesClass (0xBE67BC)


extern FontManagerClass *Unknown_BE67C4;	// NAME 1: neutral (D10)
#include "fontdatabase.h"	// FontStringFilesClass at 0xBE67BC (P-H12-3)


#include "galaxy_map_robj.h"	// GalaxyMapLineRObjClass (constructor 0x7B3E30, 0xFC bytes)
#include "local_interface_sort.h"
#include "..\..\ww3d2\assetmgr.h"
#include "..\..\wwlib\ini.h"
#include "WWAudio.h"
// the 0xFC-byte render object at +0x64: 0x691830 news a GalaxyMapLineRObj (constructor 0x7B3E30) into it
// (sclient.h holds it as RefCountPtr<GalaxyMapLineRObj>: its instances are galaxy_map_render.cpp's Line's)


















// 0x691570 (the destructor 0x691680, deleting 0x691650)
LoadingViewClass::LoadingViewClass(ClientTaskClass *ctask) :
	Unknown65B810ViewClass(ctask, 0),
	Unknown50(0),
	Unknown54(0),
	Unknown68(false),
	Unknown69(false)
{
	G_ASSERT(ctask);
}

LoadingViewClass::~LoadingViewClass()
{
}

// 0x691770: slot +0x04: the base's Init, then both scenes' fog off
void LoadingViewClass::Unknown_04()
{
	UIViewClass::Unknown_04();
	Unknown1C->Set_Fog_Enable(false);
	Unknown24->Set_Fog_Enable(false);
}

// 0x6917A0: slot +0x10: UIViewClass's Shutdown (past the second view's 0x65B8B0), then the loading screen's objects
void LoadingViewClass::Unknown_10()
{
	UIViewClass::Unknown_10();
	Unknown58 = NULL;
	Unknown5C = NULL;
	Unknown60 = NULL;
	Unknown64 = NULL;
}

// START's teardown as 0x691830 inlines it: the inline layer leaves its RefCountPtr clears out of line (0x5988A0, 0x6926B0)
static inline void Release_Screen(LoadingViewClass *view)	// NAME 1: what it does
{
	if (view->Unknown68) {
		if (view->Unknown64) {
			view->Unknown64->Remove();
		}
		view->Unknown58->Remove();
		Unknown_BE67C4->Unknown_6D2310(view->Unknown58);
		Unknown_BE67BC->Unload("fc_map_load00.ini");
		view->Unknown58 = NULL;
		view->Unknown5C = NULL;
		view->Unknown60 = NULL;
		view->Unknown64 = NULL;
		view->Unknown68 = false;
	}
}

// 0x691830: SERVER_HANDOFF's loading screen: START's teardown of the previous one, Miles' mixing up for the load,
// the asset manager's 'map_load00' with its fonts (fc_map_load00.INI); with no names its two text lines hidden,
// else 'map_select' on the destination system's bone, 'map_from' on the source system's, the three texts and,
// between two systems, galaxy_map_c.ini's 'SectorLoadLine' from one to the other
void LoadingViewClass::Unknown_691830(const std::string &source_sector, const std::string &source_system, const std::string &name, const std::string &system)
{
	Release_Screen(this);
	AIL_set_preference(DIG_DS_MIX_FRAGMENT_CNT, 8);
	AIL_serve();
	WWAudioClass::Get_Instance()->m_Unknown19A5 = true;
	AIL_set_preference(DIG_DS_MIX_FRAGMENT_CNT, 16);
	AIL_serve();

	LocalInterfaceSort sort((InterfaceSceneClass *)Unknown_50(), false);
	if (source_sector.length() == 0 && source_system.length() == 0 && name.length() == 0 && system.length() == 0) {
		Unknown69 = true;
	} else {
		Unknown69 = false;
	}
	Unknown58 = NULL;
	Unknown5C = NULL;
	Unknown60 = NULL;
	Unknown64 = NULL;

	WW3DAssetManager::Get_Instance()->Set_WW3D_Load_On_Demand(true);
	WW3D::Enable_Overbright_Modify_On_Load(false);
	Unknown58 = Create_Get(WW3DAssetManager::Get_Instance()->Create_Render_Obj("map_load00"));
	Unknown_BE67BC->Load("fc_map_load00.INI");
	Unknown_BE67C4->Add(Unknown58);

	if (Unknown69) {
		RenderObjClass *text = Unknown58->Get_Sub_Object_By_Name("text00");
		text->Set_Hidden(true);
		text->Release_Ref();
		text = Unknown58->Get_Sub_Object_By_Name("text01");
		text->Set_Hidden(true);
		text->Release_Ref();
	} else {
		Unknown5C = Create_Get(WW3DAssetManager::Get_Instance()->Create_Render_Obj("map_select"));
		Unknown60 = Create_Get(WW3DAssetManager::Get_Instance()->Create_Render_Obj("map_from"));

		char bone[256];
		int from = 0;
		int to = 0;
		if (!system.empty()) {
			strcpy(bone, system.c_str());
			for (unsigned int i = 0; i < strlen(bone); i++) {
				if (bone[i] == ' ') {
					bone[i] = '_';
				}
			}
			to = Unknown58->Get_Bone_Index(bone);
			if (to != 0) {
				Unknown58->Add_Sub_Object_To_Bone(Unknown5C.Peek(), to);
			}
		}
		if (!source_system.empty()) {
			strcpy(bone, source_system.c_str());
			for (unsigned int i = 0; i < strlen(bone); i++) {
				if (bone[i] == ' ') {
					bone[i] = '_';
				}
			}
			from = Unknown58->Get_Bone_Index(bone);
			if (from != 0 && from != to) {
				Unknown58->Add_Sub_Object_To_Bone(Unknown60.Peek(), from);
			}
		}

		char buffer[1024];
		FontSys *text = Unknown_BE67C4->Get_Style("Source_Text_map_load00", Unknown58.Peek());
		if (source_sector.empty() && source_system.empty()) {
			buffer[0] = '\0';
		} else {
			_snprintf(buffer, sizeof(buffer), "Now leaving %s", source_sector.c_str());
		}
		text->Change_Text(buffer, 0);

		text = Unknown_BE67C4->Get_Style("Dest_Text_map_load00", Unknown58.Peek());
		if (name.empty() && system.empty()) {
			buffer[0] = '\0';
		} else if (system.empty()) {
			_snprintf(buffer, sizeof(buffer), "Docking with %s", name.c_str());
		} else {
			_snprintf(buffer, sizeof(buffer), "Entering %s (%s)", name.c_str(), system.c_str());
		}
		text->Change_Text(buffer, 0);

		Unknown_BE67C4->Get_Style("Loading_Text_map_load00", Unknown58.Peek())->Set_Sort_Level(1);

		if (from != to && from != 0 && to != 0) {
			Unknown64 = Create_NEW(new GalaxyMapLineRObj);
			INIClass ini("galaxy_map_c.ini");
			Unknown64->Unknown_7B3F00(ini, "SectorLoadLine", Vector3(1.0f, 0.0f, 0.0f), Unknown60, Unknown5C);
			Unknown64->Add(Unknown1C);
		}
	}
	Unknown58->Add(Unknown1C);
	Unknown68 = true;
}

// 0x692160: SERVER_HANDOFF's screen with no names (SpaceClientClass 0x74BA30, 0x7992F0)
void LoadingViewClass::Unknown_692160()
{
	std::string a, b, c, d;
	Unknown_691830(a, b, c, d);
}

// 0x692330: START 5: the loading screen's objects and fonts away, then Miles' mixing back
void LoadingViewClass::Unknown_692330()
{
	if (Unknown68) {
		if (Unknown64) {
			Unknown64->Remove();
		}
		Unknown58->Remove();
		Unknown_BE67C4->Unknown_6D2310(Unknown58);
		Unknown_BE67BC->Unload("fc_map_load00.ini");
		Unknown58 = NULL;
		Unknown5C = NULL;
		Unknown60 = NULL;
		Unknown64 = NULL;
		Unknown68 = false;
	}
	AIL_set_preference(DIG_DS_MIX_FRAGMENT_CNT, 8);
	AIL_serve();
}

// 0x692420: slot +0x0C: 'Loading' with one to five dots, once a second; the base's render; once the avatar is
// incapacitated, the loading screen goes as START's does
int LoadingViewClass::Unknown_0C(bool on)
{
	static unsigned int last;	// 0xBE65D0  // NAME 1: what it holds
	static int dots = 5;		// 0xB90830  // NAME 1: what it holds
	unsigned int now = Unknown_Get_Ticks();
	WW3D::Sync(now);
	if (now - last > 1000) {
		FontSys *text = Unknown_BE67C4->Get_Style("Loading_Text_map_load00", Unknown58.Peek());
		if (++dots > 5) {
			dots = 1;
		}
		char buffer[52];
		strcpy(buffer, "Loading");
		for (int i = 0; i < dots; i++) {
			strcat(buffer, ".");
		}
		text->Change_Text(buffer, 0);
		last = now;
	}
	int result = UIViewClass::Unknown_0C(on);
	((SpaceClientClass *)Client->As_Space_Client())->Text->Reset();
	if (((SpaceClientClass *)Client->As_Space_Client())->Unknown_750B60()) {
		if (Unknown68) {
			if (Unknown64) {
				Unknown64->Remove();
			}
			Unknown58->Remove();
			Unknown_BE67C4->Unknown_6D2310(Unknown58);
			Unknown_BE67BC->Unload("fc_map_load00.ini");
			Unknown58 = NULL;
			Unknown5C = NULL;
			Unknown60 = NULL;
			Unknown64 = NULL;
			Unknown68 = false;
		}
		AIL_set_preference(DIG_DS_MIX_FRAGMENT_CNT, 8);
		AIL_serve();
	}
	return(result);
}

/*
**	Notes (LD4, 2026-10-29)
**
**	loadingview.cpp is the loading view (LoadingViewClass, sclient.h: 0x6C bytes over uiview.cpp's second view,
**	table 0xAFCF7C), client 0x6912A0-0x6927C0 (appfile.py show app\client\loadingview.cpp; the constructor's
**	G_ASSERT 'ctask' on the client's line 52, __FILE__ e:\build\beta\G\app\Client\loadingview.cpp).
**	+0x58..+0x60 are the asset manager's render objects 'map_load00', 'map_select', 'map_from' (0x691830; slot
**	+0x38 RenderObjClass::Remove), +0x64 a 0xFC-byte render object of its own (constructor 0x7B3E30, its own
**	RefCountPtr instances 0x692680 / 0x6926B0); the font manager 0xBE67C4 keeps fonts per render object (0x6D2310
**	takes the RefCountPtr, 0x6D25D0 the pointer). START 0x692330 and the slot +0x0C 0x692420 rebuilt (UI1).
**	Not rebuilt: 0x6912A0 / 0x691320 before the constructor (no callers: a helper over a render object, a
**	second object and a Vector3, its class unknown).
**	SERVER_HANDOFF's 0x691830 rebuilt exact (L1, 2026-11-14): +0x64 is a GalaxyMapLineRObj (0x691830 news
**	one, constructor 0x7B3E30, and calls its 0x7B3F00 with galaxy_map_c.ini's 'SectorLoadLine'); the file's
**	member is RefCountPtr<GalaxyMapLineRObj> in sclient.h, galaxy_map_render.cpp's Line's type (the
**	instances 0x692680 Create_NEW, 0x6926B0 operator=(NULL) are this object's). Its first block is START's
**	teardown through one inline layer (Release_Screen): the layer leaves the four RefCountPtr clears out of
**	line, as the client calls them.
*/
