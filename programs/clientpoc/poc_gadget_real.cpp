// Earth & Beyond client (reconstruction): the POC's bridge from the app files' local views of GadgetClass to the real one (D179). NOT THE CLIENT'S CODE.
/*
**	ui_manu.cpp declares its own GadgetClass with neutral slots (ui_scut.cpp's view and its button and proxy button
**	stand-ins went with gd_scut_proxy.cpp, run 2026-11-11). Their derived classes' tables are
**	therefore built from ?Unknown_NN@GadgetClass@@... names no real function has, and gen_stubs' stand-ins of them
**	return with the wrong stack (the real slot +0x54 takes two arguments). poc_gadget_views*.cpp define those names
**	as jumps into ButtonGadgetClass's real table, which this file provides.
*/
#include <string.h>
#include "..\..\stubs\stub_log.h"
#define __INTRFACE_MEMBERS
#include "..\..\app\client\sclient.h"
#include "..\..\app\client\gadget.h"
#include "..\..\app\client\unknown_675120.h"
#include "layer.h"

extern "C" void **POC_Button_Table()
{
	static void **table = NULL;
	if (table == NULL) {
		STUB_CALLED("POC GadgetClass views: forwarding to ButtonGadgetClass's table");
		table = *(void ***)new ButtonGadgetClass((RenderObjClass *)NULL, LayerClass(), 0x55, 4);
	}
	return(table);
}
