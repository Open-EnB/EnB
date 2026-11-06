// Earth & Beyond client (reconstruction): app\client\ui_ping.cpp (the client's __FILE__
// 'e:\build\beta\G\app\Client\ui_ping.cpp', 0x5D7630-0x5D8C3F by its asserts, its 'fc_ping.ini' and its table 0xAF5768).
// Notes at the end.
#define __INTRFACE_MEMBERS	// intrface.h's members (the constructor 0x540D30, +0x58's list)
#include <stdio.h>
#include "ui_ping.h"
#include "gadget.h"
#include "unknown_675120.h"
#include "unknown_674260.h"
#include "unknown_51c800.h"
#include "dynamic_window_class.h"
#include "fontmanager.h"
#include "sclient.h"
#include "local_interface_sort.h"
#include "client.h"
#include "rendobj.h"
#include "assetmgr.h"
#include "vector2.h"
#include "..\..\ww3d2\ww3d.h"
#include "..\shared\unknown_debug.h"
#include "..\shared\unknown_message.h"

void Unknown_496AE0(RenderObjClass *robj, bool make_geometry_unique);	// 0x496AE0 (ro_funcs.cpp, cdecl)  // NAME 1 [robj, make_geometry_unique]: as eff_o.cpp's
bool Unknown_496110(RenderObjClass *robj, float a, float b, float c);	// 0x496110 (ro_funcs.cpp, cdecl): a model's highlight  // NAME 1 [robj, a, b, c]: neutral
RefCountPtr<FontSys> Unknown_589CA0(const RefCountPtr<RenderObjClass> &robj);	// 0x589CA0 (unknown_msgbox.cpp: the text of an element)

inline RenderObjClass *UIPing_Peek_Model(ButtonGadgetClass *button) { return(button->RObj); }	// (an inline accessor: 0x5D85B0 loads the model in each branch)  // NAME 1: neutral
// 0xB84410: the rows' labels
static const char *UIPingLabels[3] = {	// NAME 1: neutral (D10)
	"Latency : ",
	"Data Vol : ",
	"Efficiency : "
};

// 0x5D7630
UIPingClass::UIPingClass(LineDisplayClientClass *client) :
	InterfaceClass(client),
	Button(NULL),
	Unknown0AC(0),
	Unknown0B0(0),
	Unknown0B4(0),
	Timer(0),
	Shown(false)
{
	for (int i = 0; i < 3; i++) {
		Rows[i] = NULL;
	}
	Unknown_BE67BC->Load("fc_ping.ini");
	Window = Create_NEW(new DynamicWindowClass("DIALOG_BOX_WINDOW", 3));
	Window->Set_Level(3);
	Info = Create_NEW(new DynamicWindowClass("DIALOG_BOX_INFO", 1));
	Window->Add_Object(Info.Peek(), 0);
	Unknown098 = Create_NEW(new DynamicWindowClass("DIALOG_BOX_INFO", 4));
	Window->Add_Object(Unknown098.Peek(), 1);
}

// 0x5D7A00
UIPingClass::~UIPingClass()
{
	Unknown_5C();
	if (Efficiency) {
		Efficiency->Remove();
		Efficiency = NULL;
	}
	if (Mesh) {
		Unknown58.Unknown_686DD0(Mesh.Peek());
		Mesh = NULL;
	}
	for (int i = 0; i < 3; i++) {
		if (Rows[i]) {
			Rows[i] = NULL;
		}
	}
	if (Header) {
		Header = NULL;
	}
	if (Info) {
		Info->Remove_All(false);
		Info = NULL;
	}
	if (Unknown098) {
		Unknown098->Remove_All(false);
		Unknown098 = NULL;
	}
	if (Window) {
		Window->Remove_All(false);
		Window = NULL;
	}
	Unknown_BE67BC->Unload("fc_ping.ini");
}

// 0x5D7D60
void UIPingClass::Unknown_54()
{
	Mesh = Create_Get(WW3DAssetManager::Get_Instance()->Create_Render_Obj("TSI05C"));
	Unknown58.Unknown_686CC0(Mesh.Peek(), NULL);

	RefCountPtr<RenderObjClass> color = Create_Get(Mesh->Get_Sub_Object_By_Name("color"));
	if (color) {
		Unknown_496AE0(color.Peek(), 0);
		Button = new ButtonGadgetClass(color.Peek(), 0x55, 3);
		Unknown64.Unknown_6870E0(Button, 1, View, 1, 0);
		Button->Unknown2C_0A = true;
		GadgetClass::Set_Tooltip(Button, "UI_PING", "PING");
	}

	GadgetClass *gadget = new Unknown_674260Class(Mesh);
	Unknown64.Unknown_6870E0(gadget, 3, View, 1, 0);
	GadgetClass::Set_Tooltip(gadget, "UI_PING", "PING");

	Efficiency = Create_NEW(new FontSys("PING_EFFICIENCY", 0, 0));
	Mesh->Add_Sub_Object_To_Bone(Efficiency.Peek(), "value");

	RefCountPtr<FontSys> text = Create_NEW(new FontSys("HEADER_INFO", 0, 0));
	Header = Create_NEW(new Unknown51C800Class(text.Peek(), 6));
	Info->Add_Object(Header.Peek(), 0);

	char name[32];
	for (int i = 0; i < 3; i++) {
		_snprintf(name, sizeof(name), "PING_INFO_%d", i);
		text = Create_NEW(new FontSys(name, 0, 0));
		_snprintf(name, sizeof(name), "%s000", UIPingLabels[i]);
		text->Change_Text(name, 0);
		Rows[i] = Create_NEW(new Unknown51C800Class(text.Peek(), 6));
		Unknown098->Add_Object(Rows[i].Peek(), i);
	}
}

// 0x5D83D0
void UIPingClass::Unknown_58()
{
	if (!Active) {
		InterfaceSortScopeClass scope(this);
		InterfaceClass::Unknown_58();
		Unknown_5D85B0();
	}
}

// 0x5D8460
void UIPingClass::Unknown_5C()
{
	if (Active) {
		if (Shown) {
			Window->Remove_From_Scene();
			Shown = false;
		}
		InterfaceClass::Unknown_5C();
	}
}

// 0x5D84A0
void UIPingClass::Unknown_60()
{
	if (Active) {
		Timer += WW3D::Get_Frame_Time() * 0.001f;
		if (Timer > 1.0f) {
			Unknown_5D85B0();
		}
	}
}

// 0x5D8510
bool UIPingClass::Unknown_68(int key)
{
	switch (key) {
		case 1:
			if (Shown) {
				Shown = false;
				Window->Remove_From_Scene();
				Unknown_5D85B0();
				return(false);
			}
			Shown = true;
			Window->Set_Center(Vector2(-320.0f, 180.0f));
			Window->Add_To_Scene((SceneClass *)Unknown44);
			Unknown_5D85B0();
			break;
	}
	return(false);
}






































































// 0x5D85B0
void UIPingClass::Unknown_5D85B0()
{
	Timer = 0;
	ClientClass *client = (ClientClass *)Client;
	FAIL_IF(!client) return;

	float send, recv, rtt, win_size, win_used, pct_loss;
	int no_recv;
	if (!client->Get_Stats(&send, &recv, &rtt, &win_size, &win_used, &pct_loss, &no_recv)) {
		return;
	}


	Unknown_4BC460("", "NET_STATS", __FILE__, __LINE__, 1, 1, "Send:     %.2f", send);
	Unknown_4BC460("", "NET_STATS", __FILE__, __LINE__, 1, 2, "Recv:     %.2f", recv);
	Unknown_4BC460("", "NET_STATS", __FILE__, __LINE__, 1, 3, "Rtt:      %.2f", rtt);
	Unknown_4BC460("", "NET_STATS", __FILE__, __LINE__, 1, 4, "WinSize:  %.2f", win_size);
	Unknown_4BC460("", "NET_STATS", __FILE__, __LINE__, 1, 5, "WinUsed:  %.2f", win_used);
	Unknown_4BC460("", "NET_STATS", __FILE__, __LINE__, 1, 6, "PctLoss:  %.2f", pct_loss);
	Unknown_4BC460("", "NET_STATS", __FILE__, __LINE__, 1, 6, "NoRecvT:  %d   ", no_recv);

	RefCountPtr<FontSys> text;
	if (Button) {
		if (rtt > 500.0f) {
			Unknown_496110(UIPing_Peek_Model(Button), 1.0f, 0.0f, 0.0f);
		} else if (rtt < 200.0f) {
			Unknown_496110(UIPing_Peek_Model(Button), 0.0f, 1.0f, 0.0f);
		} else {
			Unknown_496110(UIPing_Peek_Model(Button), 1.0f, 1.0f, 0.0f);
		}
	}

	char buffer[32];
	if (Efficiency) {
		int efficiency = 100 - (int)pct_loss;
		efficiency = (efficiency < 0) ? 0 : efficiency;
		_snprintf(buffer, sizeof(buffer), "%d", efficiency);
		Efficiency->Change_Text(buffer, 0);
		if (efficiency < 75) {
			Efficiency->Unknown_6CE910(true);
		} else {
			Efficiency->Unknown_6CE910(false);
		}
	}

	if (Shown) {
		text = Unknown_589CA0(Rows[0]->Get_Render_Object());
		_snprintf(buffer, sizeof(buffer), "%s%d", UIPingLabels[0], (int)rtt);
		text->Change_Text(buffer, 0);
		if (rtt > 500.0f) {
			text->Unknown_6CE910(true);
		} else {
			text->Unknown_6CE910(false);
		}
		Rows[0]->Invalidate();

		int volume = (int)(send + recv);
		text = Unknown_589CA0(Rows[1]->Get_Render_Object());
		_snprintf(buffer, sizeof(buffer), "%s%d", UIPingLabels[1], volume);
		text->Change_Text(buffer, 0);
		if (volume > 1000) {
			text->Unknown_6CE910(true);
		} else {
			text->Unknown_6CE910(false);
		}
		Rows[1]->Invalidate();

		int efficiency = 100 - (int)pct_loss;
		text = Unknown_589CA0(Rows[2]->Get_Render_Object());
		_snprintf(buffer, sizeof(buffer), "%s%d", UIPingLabels[2], efficiency);
		text->Change_Text(buffer, 0);
		if (efficiency < 75) {
			text->Unknown_6CE910(true);
		} else {
			text->Unknown_6CE910(false);
		}
		Rows[2]->Invalidate();
	}
}

/*
**	Notes (PG1, 2026-11-06)
**
**	The object: the asserts' __FILE__ (0xB84560) in 0x5D85B0; the constructor 0x5D7630 loads 'fc_ping.ini' and the
**	destructor 0x5D7A00 unloads it; the next object starts at 0x5D8C40 (a UIChatPanelBaseClass constructor). Compiler
**	output in the range: the deleting destructor 0x5D79D0, RefCountPtr's out-of-line default constructor 0x5D8BD0
**	(the eh vector constructor's element) and the table thunks 0x5D8C20 / 0x5D8C30.
**
**	The NoRecvT line repeats the PctLoss row (6).
*/
