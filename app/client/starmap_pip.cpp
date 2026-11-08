// Earth & Beyond client (reconstruction): the PDA's star map picture-in-picture (notes at the end)
#include <float.h>
#include <string.h>
#include <string>
#include "starmap_pip.h"
#include "sclient.h"
#include "client.h"
#include "co.h"
#include "co_data.h"
#include "starmap_data.h"
#include "fontmanager.h"
#include "fontdatabase.h"
#include "option.h"
#include "local_interface_sort.h"
#include "intrface.h"
#include "starmap_pip_support.h"
#include "..\shared\g_assert.h"
#include "..\..\wwlib\ref_ptr.h"
#include "..\..\ww3d2\rendobj.h"
#include "..\..\ww3d2\scene.h"
#include "wwmath.h"

// NOT EXACT: written from the pseudocode (D64), functional
// @0x7BF930
StarmapPipClass::StarmapPipClass(SClient *client) :
	MapPipClass(client, true),
	Unknown50(false),
	Unknown51(false),
	Unknown54(1.0f),
	Mult(1.0f),
	CurMult(1.0f),
	Unknown60(0),
	Unknown68(0.0f, 0.0f),
	Unknown70(0.0f),
	Unknown74(0.0f, 0.0f),
	Unknown7C(0.0f, 0.0f),
	Unknown84(0),
	Unknown88(0),
	Unknown8C(NULL),
	Unknown90(NULL),
	Unknown94(-1),
	Unknown98(-1),
	Unknown9C(0),
	UnknownA0(NULL),
	UnknownA4(NULL),
	ShowCoords(0),
	UnknownB8(1.0f),
	UnknownBC(1.0f),
	Label(0)
{
	Unknown_BE67BC->Load("fc_radar.ini");
	Unknown8C = new Unknown_7B97F0Class(Client, 0.75f, 1, Vector3(0.0f, 0.8f, 0.0f), 0.5f);
	Unknown90 = new Unknown_7B97F0Class(Client, 0.75f, 2, Vector3(0.8f, 0.0f, 0.0f), 0.5f);
	UnknownA0 = new Unknown_7C3220Class(Scene, false, 4);
	UnknownA4 = new Unknown_7C3220Class(Scene, true, 4);
	XPos = Create_NEW(new FontSys("STARMAP_X_POS", 0, 0));
	YPos = Create_NEW(new FontSys("STARMAP_Y_POS", 0, 0));
	ZPos = Create_NEW(new FontSys("STARMAP_Z_POS", 0, 0));
	ShowCoords = OptionClass::Instance->Get_Int("Starmap_ShowCoords");
	Label = OptionClass::Instance->Get_Int("Starmap_Label");
}

// NOT EXACT: written from the pseudocode (D64), functional
// @0x7BFF20
StarmapPipClass::~StarmapPipClass()
{
	IntOptionStruct *option = OptionClass::Instance->Find_Int("Starmap_ShowCoords");
	if (option) {
		option->Set_Value(ShowCoords);
	}
	option = OptionClass::Instance->Find_Int("Starmap_Label");
	if (option) {
		option->Set_Value(Label);
	}
	OptionClass::Instance->Save();
	for (std::vector<Unknown_7C3220Class *>::iterator it = Labels.begin(); it != Labels.end(); it++) {
		delete *it;
	}
	Labels.clear();
	delete UnknownA0;
	UnknownA0 = NULL;
	delete UnknownA4;
	UnknownA4 = NULL;
	delete Unknown8C;
	Unknown8C = NULL;
	delete Unknown90;
	Unknown90 = NULL;
	Unknown_BE67BC->Unload("fc_radar.ini");
}

// @0x7C01C0
GadgetClass *StarmapPipClass::Unknown_18()
{
	return(new GadgetStarmapClass(this));
}

// @0x7C0240
void StarmapPipClass::Unknown_1C(int arg)
{
	Unknown_7C2060(ShowCoords + 1);
}

// NOT EXACT: written from the pseudocode (D64), functional
// @0x7C0260
void StarmapPipClass::Unknown_20(int arg)
{
	Unknown60 = Client->AvatarID;
	if (arg && Unknown50 && Mult != 0.5f) {
		Unknown_Set_Mult(0.5f);
	}
}

// NOT EXACT: written from the pseudocode (D64), functional
// @0x7C0330
void StarmapPipClass::Unknown_24(int arg)
{
	if (!Unknown50) {
		return;
	}
	switch (Label) {
		case 0:
			Label = 1;
			((SClient *)Client->Unknown_18())->Unknown_74D990(0, "Starmap PIP will now display labels of your own ship and other group member ships only.", 0);
			break;
		case 1:
			Label = 2;
			((SClient *)Client->Unknown_18())->Unknown_74D990(0, "Starmap PIP will now display labels of all visible ships.", 0);
			break;
		case 2:
			Label = 3;
			((SClient *)Client->Unknown_18())->Unknown_74D990(0, "Starmap PIP will now display all visible labels in the sector.", 0);
			break;
		case 3:
			Label = 0;
			((SClient *)Client->Unknown_18())->Unknown_74D990(0, "Starmap PIP will now not display any labels.", 0);
			break;
	}
	IntOptionStruct *option = OptionClass::Instance->Find_Int("Starmap_Label");
	if (option) {
		option->Set_Value(Label);
	}
	Unknown_7C2850(true);
}

// 0x7C0440 (FAIL_IF 'MouseTarget' on 283), 0x7C0620 ('!sdata' 361, '!co' 364) and 0x7C0910 are not rebuilt yet.

// NOT EXACT: written from the pseudocode (D64), functional
// @0x7C0AF0
bool StarmapPipClass::Unknown_0C(bool on)
{
	if (on) {
		Unknown60 = Client->AvatarID;
		return(MapPipClass::Unknown_0C(on));
	}
	for (std::vector<Unknown_7C3220Class *>::iterator it = Labels.begin(); it != Labels.end(); it++) {
		delete *it;
	}
	Labels.clear();
	return(MapPipClass::Unknown_0C(on));
}

// NOT EXACT: written from the pseudocode (D64), functional
// @0x7C0BC0
bool StarmapPipClass::Unknown_04(const RefCountPtr<InterfaceClass> &iface_ptr, const RefCountPtr<RenderObjClass> &vp, int arg)
{
	if (!MapPipClass::Unknown_04(iface_ptr, vp, arg)) {
		return(false);
	}
	XPos->Set_Sort_Level(Unknown48->Get_Sort_Level());
	YPos->Set_Sort_Level(Unknown48->Get_Sort_Level());
	ZPos->Set_Sort_Level(Unknown48->Get_Sort_Level());
	Unknown_7C2060(ShowCoords);
	return(true);
}

// NOT EXACT: written from the pseudocode (D64), functional
// @0x7C0C80
bool StarmapPipClass::Unknown_10(const RefCountPtr<RenderObjClass> &vp)
{
	if (!MapPipClass::Unknown_10(vp)) {
		return(false);
	}
	Unknown70 = (ViewportSize.Y < ViewportSize.X) ? ViewportSize.X : ViewportSize.Y;
	Unknown_7C1510(false);
	Vector3 position(Rect.Left, Rect.Bottom, 0.0f);
	XPos->Set_Position(position);
	YPos->Set_Position(position);
	ZPos->Set_Position(position);
	return(true);
}

// NOT EXACT: written from the pseudocode (D64), functional
// @0x7C0D40
bool StarmapPipClass::Unknown_08()
{
	if (!MapPipClass::Unknown_08()) {
		return(false);
	}
	Unknown_7C2060(ShowCoords);
	return(true);
}

// NOT EXACT: written from the pseudocode (D64), functional
// @0x7C0D70
void StarmapPipClass::Unknown_14()
{
	MapPipClass::Unknown_14();
	Unknown50 = false;
	Unknown51 = false;
	if (ViewportGadget) {
		ViewportGadget->Unknown_40(false);
	}
	Unknown48->Set_Hidden(true);
	Unknown4C->Set_Hidden(true);
	Unknown74.Set(FLT_MAX, FLT_MAX);
	Unknown7C.Set(-FLT_MAX, -FLT_MAX);
	UnknownA4->Unknown_7C3830(NULL);
	UnknownA0->Unknown_7C3830(NULL);
	for (std::vector<Unknown_7C3220Class *>::iterator it = Labels.begin(); it != Labels.end(); it++) {
		delete *it;
	}
	Labels.clear();
}

// 0x7C0EA0 and 0x7C0EE0 ('!WarpPath' 644) .. 0x7C1C50 ('CurMult < WWMATH_EPSILON' 1157) are not rebuilt yet,
// but for the inline below: 0x7C0260, 0x7C1120, 0x7C1510 and 0x7C1A90 expand it (FAIL_IF on 1039).









































































































































































































































































































































































































































































































































































































































































































































































































































// NOT EXACT: written from the pseudocode (D64), functional
inline void StarmapPipClass::Unknown_Set_Mult(float mult)
{
	FAIL_IF(CurMult < WWMATH_EPSILON) {
		CurMult = 1.0f;
	}
	float x = Unknown68.X / CurMult;
	float y = Unknown68.Y / CurMult;
	Mult = mult;
	CurMult = Unknown54 * mult;
	Unknown_7C1770(x, y);
}

// 0x7C1C50 ('CurMult < WWMATH_EPSILON' 1157) is not rebuilt yet.
























































































































// NOT EXACT: written from the pseudocode (D64), functional
// @0x7C2060
void StarmapPipClass::Unknown_7C2060(int mode)
{
	ShowCoords = mode;
	if (Interface && mode > 0 && mode < 3) {
		InterfaceSortClass sort(Interface.Peek());
		if (!XPos->Peek_Scene()) {
			XPos->Add((SceneClass *)Interface->Unknown44);
		}
		if (!YPos->Peek_Scene()) {
			YPos->Add((SceneClass *)Interface->Unknown44);
		}
		if (!ZPos->Peek_Scene()) {
			ZPos->Add((SceneClass *)Interface->Unknown44);
		}
		return;
	}
	if (XPos->Peek_Scene()) {
		XPos->Remove();
	}
	if (YPos->Peek_Scene()) {
		YPos->Remove();
	}
	if (ZPos->Peek_Scene()) {
		ZPos->Remove();
	}
	if (Interface) {
		ShowCoords = 0;
	}
}

// 0x7C2280, 0x7C2340 .. 0x7C2D30, 0x7C2DA0 (the labels' vector clear) .. 0x7C30E0 are not rebuilt yet.

// NOT EXACT: written from the pseudocode (D64), functional
// @0x7C3220
Unknown_7C3220Class::Unknown_7C3220Class(const RefCountPtr<SceneClass> &scene, bool arg, int type) :
	ObjectID(0),
	Scene(scene),
	Name("")
{
	Unknown_BE67BC->Load("fc_radar.ini");
	switch (type) {
		case 0:
			Text = Create_NEW(new FontSys("RADAR_GROUP", 0, 0));
			Text->Set_Sort_Level(4);
			break;
		case 1:
			Text = Create_NEW(new FontSys("RADAR_PLAYER", 0, 0));
			Text->Set_Sort_Level(5);
			break;
		case 2:
			Text = Create_NEW(new FontSys("RADAR_OBJECT", 0, 0));
			Text->Set_Sort_Level(6);
			break;
		case 3:
			Text = Create_NEW(new FontSys("RADAR_JUMPSTART", 0, 0));
			Text->Set_Sort_Level(3);
			Text->Unknown_6D8A80(true);
			break;
		case 4:
			Text = Create_NEW(new FontSys("RADAR_NAME", 0, 0));
			Text->Set_Sort_Level(arg ? 1 : 2);
			Text->Unknown_6CE910(!arg);
			break;
	}
}

// NOT EXACT: written from the pseudocode (D64), functional
// @0x7C3720
Unknown_7C3220Class::~Unknown_7C3220Class()
{
	Text->Remove();
	Text = NULL;
	Unknown_BE67BC->Unload("fc_radar.ini");
}

// NOT EXACT: written from the pseudocode (D64), functional
// @0x7C3830
void Unknown_7C3220Class::Unknown_7C3830(ClientObjectClass *co)
{
	if (!co) {
		if (ObjectID) {
			ObjectID = 0;
			Text->Remove();
		}
		return;
	}
	unsigned id = co->Get_Game_ID();
	std::string name("");
	co->Unknown_6FF7A0(name);
	if (ObjectID != id || strcmp(Text->Unknown_6D87D0(), name.c_str()) != 0) {
		ObjectID = id;
		if (name.compare("Unknown") == 0 && Name.length()) {
			name = Name;
		}
		Text->Change_Text(name.c_str(), 0);
		if (!Text->Peek_Scene()) {
			Text->Add(Scene.Peek());
		}
	}
	if (ObjectID) {
		StarmapDataClass *sdata = Get_Starmap_Data(co, false);
		if (!sdata) {
			ObjectID = 0;
			Text->Remove();
		} else {
			Text->Set_Position(sdata->Get_Position());
		}
	}
}

/*
**	Reconstruction notes
**
**	The object's boundaries in the client: __FILE__ 0xB9F4D8 ('e:\build\beta\G\app\client\starmap_pip.cpp'),
**	asserts on lines 283-1157 from 0x7C0260 to 0x7C1C50. The constructor 0x7BF930 sets the table 0xB06C8C
**	(slots 0x7BFEF0, 0x7C0BC0, 0x7C0D40, 0x7C0AF0, 0x7C0C80, 0x7C0D70, 0x7C01C0, 0x7C0240, 0x7C0260, 0x7C0330)
**	and its literals ('STARMAP_X_POS' 0xB9F4A8 .. 'Starmap_ShowCoords' 0xB9F470) sit just before the file's
**	first FAIL_IF strings (0xB9F4B8, 0xB9F4D8); the label class's 'RADAR_*' literals (0xB9F704-0xB9F734) follow
**	the slots' messages (0xB9F50C-0xB9F5D8). 'Starmap_Label' 0xB9F31C is pooled with an earlier object's.
**	0x7C3830 ends the claim; 0x7C3A90 (triage's hint for the end) is not settled.
**
**	The FAIL_IF on 1039 sits in an inline the client expands in 0x7C0260, 0x7C1120, 0x7C1510 and 0x7C1A90
**	(Unknown_Set_Mult here), so VC6 inlined it from a definition later in the file.
**
**	NOT EXACT (D64): every function marked, written from the pseudocode, functional. Not rebuilt: 0x7C0440,
**	0x7C0620, 0x7C0910, 0x7C0EA0-0x7C1C50, 0x7C2280-0x7C30E0 (the POC and clienttest get them from stubs).
*/
