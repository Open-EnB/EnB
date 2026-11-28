// Earth & Beyond client (reconstruction): app\Client\gd_slot_data.cpp (__FILE__ 0xB8D684). Notes at the end of the file.
#include "gd_slot_data.h"
#include "ui_scut.h"
#include "..\shared\g_assert.h"
#include "..\shared\auth_product.h"
extern GadgetClass *Unknown_BE5F24;										// gadget.cpp's held gadgets
extern GadgetClass *Unknown_BE5F28;


























































SlotDataGadget::SlotDataGadget(SlotDataIFaceClass *slot_iface, SlotDataRenderClass *slot_render) :
	UIMeshGadgetClass(slot_render->Mesh, 0xE55), SlotIFace(slot_iface), SlotRender(slot_render), Unknown68(true)
{
	G_ASSERT(SlotIFace);
	FAIL_IF(!SlotRender) return;
	SlotRender->Add_Ref();
	Set_Tooltip(this, "GD_SLOT_DATA", "NORMAL_SLOT", "Unused Slot");
	Unknown2C_06 = false;
}

SlotDataGadget::~SlotDataGadget()
{
	Unknown_AC(-1);
	REF_PTR_RELEASE(SlotRender);
	SlotIFace = NULL;
}














void SlotDataGadget::Unknown_AC(int slot)
{
	FAIL_IF(!SlotRender) return;
	SlotRender->Unknown_40(slot);
	if (slot == -1) {
		Set_Tooltip(this, "GD_SLOT_DATA", "NORMAL_SLOT", "Unused Slot");
	} else {
		Set_Tooltip(this, "GD_SLOT_DATA", "NORMAL_SLOT", "Empty Slot");
	}
}


int SlotDataGadget::Unknown_670910()
{
	FAIL_IF(!SlotRender) return(-1);
	return(SlotRender->Unknown20);
}













int SlotDataGadget::Unknown_670950()
{
	FAIL_IF(!SlotRender) return(0);
	return((int)SlotRender->SlotData);
}





void SlotDataGadget::Unknown_20(int arg)
{
	FAIL_IF(!SlotRender) return;
	std::string text;
	const char *entry = SlotRender->Unknown_28(&text);
	if (entry) {
		Set_Tooltip(this, "GD_SLOT_DATA", entry, text.c_str());
	}
}











void SlotDataGadget::Unknown_40(bool on)
{
	if (on) {
		Unknown_66C360();
	} else {
		Unknown_66C2D0();
	}
	GadgetClass::Unknown_40(on);
}











bool SlotDataGadget::Unknown_30(float x, float y)
{
	return(RenderObjGadgetClass::Unknown_30(x, y));
}











bool SlotDataGadget::Unknown_2C(KernelEventClass *event, float x, float y, unsigned int mask)
{
	bool handled = false;
	if (Is_Enabled()) {
		if (mask & 0x11) {
			Unknown_66C2D0();
			GadgetClass::Unknown_2C(event, x, y, mask);
			Unknown_68();
			handled = true;
			if (SlotRender) {
				SlotRender->Unknown_3C();
			}
		}
		if (((mask & 4) && Unknown_BE5F24 == this) || ((mask & 0x40) && Unknown_BE5F28 == this)) {
			Unknown_66C360();
			GadgetClass::Unknown_2C(event, x, y, mask);
			Unknown_6C();
			handled = true;
		}
		if (((mask & 2) && Unknown_BE5F24 == this) || ((mask & 0x20) && Unknown_BE5F28 == this)) {
			GadgetClass::Unknown_2C(event, x, y, mask);
			handled = true;
		}
		if (handled && Unknown68) {










			FAIL_IF(!SlotIFace) return(true);
			SlotIFace->Unknown_98(this, event, mask);
		}
	}
	return(handled);
}

void SlotDataGadget::Unknown_670C90(bool on)
{
	Unknown68 = on;
}

void SlotDataGadget::Unknown_A8(bool arg)
{
	FAIL_IF(!SlotRender) return;
	SlotRender->Unknown_2C(arg);
}









RenderObjClass *SlotDataGadget::Unknown_24(Vector3 *position, float *frame, bool indicator)
{
	return(RenderObjGadgetClass::Unknown_24(position, frame, indicator));
}








void SlotDataGadget::Unknown_90(class TextureClass *texture)
{
	G_ASSERT(false);
}

Vector3 SlotDataGadget::Unknown_9C()
{
	Vector3 color = RenderObjGadgetClass::Unknown_9C();
	if (SlotRender && SlotRender->Unknown_38(color)) {
		color.X *= 0.35f;
		color.Y *= 0.35f;
		color.Z *= 0.35f;
	}
	return(color);
}

Vector3 SlotDataGadget::Unknown_A0()
{
	Vector3 color = RenderObjGadgetClass::Unknown_A0();
	if (SlotRender) {
		SlotRender->Unknown_38(color);
	}
	return(color);
}





SlotDataShortcutGadget::SlotDataShortcutGadget(SlotDataIFaceClass *slot_iface, SlotDataRenderClass *slot_render, LayerClass *layer) :
	SlotDataGadget(slot_iface, slot_render),
	ShortcutOwner(NULL)
{
	RenderObjGadgetClass::Unknown_28(layer);
	FAIL_IF(!SlotRender) return;
	SlotRender->Unknown_6A8EC0();
}








void SlotDataShortcutGadget::Unknown_04()
{
	if (ShortcutOwner) {
		ShortcutOwner->Unknown_611E20(this);
		G_ASSERT(!ShortcutOwner);
	}
}







void SlotDataShortcutGadget::Unknown_7C(int arg)
{
	G_ASSERT(!ShortcutOwner);
	ShortcutOwner = (UIScutBarClass *)arg;
	SlotDataGadget::Unknown_A8(1);
}









void SlotDataShortcutGadget::Unknown_80(int arg)
{
	G_ASSERT(ShortcutOwner);
	ShortcutOwner = NULL;
}






void SlotDataShortcutGadget::Unknown_84(int arg)
{
	GadgetClass *scut_gadg = (GadgetClass *)arg;
	FAIL_IF(!scut_gadg) return;
	FAIL_IF(!SlotRender) return;
	Unknown_A8(0);
	if (scut_gadg->Is_Unknown2C_08()) {
		std::string text;
		const char *entry = SlotRender->Unknown_28(&text);
		if (entry) {
			Set_Tooltip(scut_gadg, "GD_SLOT_DATA", entry, text.c_str());
		}
	}
}
/*
**	Reconstruction notes.
**
**	The object: __FILE__ 0xB8D684 from 0x670650 to 0x671150's end; the guarded initialiser of
**	std::ctype<unsigned short>::id that closes every object including <string> sits at 0x671150's tail, and 0x671300
**	starts the next object (a ButtonGadgetClass over a UV control, table 0xAFB278: no __FILE__ of its own, left out).
**	0x670600 and the initialiser before 0x670650 are gd_slider.cpp's.
*/
