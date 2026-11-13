// Earth & Beyond client (reconstruction): the check box gadget (PROVISIONAL file, D137). Notes at the end of the file.
#include "unknown_7e7e00.h"
#include "assetmgr.h"
#include "texture.h"
#include "ref_ptr.h"

bool Unknown_495FD0(RenderObjClass *robj, TextureClass *texture, int index);	// 0x495FD0 (ro_funcs.cpp, cdecl): the texture onto the model  // NAME 1 [robj, texture, index]: gd_robj.cpp's

CheckBoxGadgetClass::CheckBoxGadgetClass(RenderObjClass *robj, const LayerClass &layer, unsigned int flags) :
	ButtonGadgetClass(robj, layer, 0x11, flags),
	Checked(false)
{
	Unknown_7E8050();
}

CheckBoxGadgetClass::CheckBoxGadgetClass(RenderObjClass *robj, unsigned int flags) :
	ButtonGadgetClass(robj, 0x11, flags),
	Checked(false)
{
	Unknown_7E8050();
}

CheckBoxGadgetClass::~CheckBoxGadgetClass()
{
}

bool CheckBoxGadgetClass::Unknown_2C(KernelEventClass *event, float x, float y, unsigned int mask)
{
	if (Unknown_60() && (mask & 0x11) != 0) {
		ButtonGadgetClass::Unknown_2C(event, x, y, mask);
		Checked = !Checked;
		Unknown_7E8050();
		return(true);
	}
	return(false);
}

void CheckBoxGadgetClass::Unknown_7E8030(bool on)
{
	Checked = on;
	Unknown_7E8050();
}

// NOT EXACT: register residue only (the last Release_Ref's vtable in edx, the client's in eax); D89
void CheckBoxGadgetClass::Unknown_7E8050()
{
	RefCountPtr<TextureClass> texture;
	if (Checked) {
		texture = Create_Get(WW3DAssetManager::Get_Instance()->Get_Texture("if_check_on.tga", MIP_LEVELS_ALL, WW3D_FORMAT_UNKNOWN, true));
	} else {
		texture = Create_Get(WW3DAssetManager::Get_Instance()->Get_Texture("if_check_off.tga", MIP_LEVELS_ALL, WW3D_FORMAT_UNKNOWN, true));
	}
	Unknown_495FD0(RObj, texture.Peek(), 0);
}

/*
**	Notes.
**	Object 0x7E7E70-0x7E81AF: no __FILE__ and no assert, so the name is provisional (D137). It opens after emote_data's
**	object, whose last pieces are the deque iterator constructor 0x7E7E00 (called only by emote_data's 0x7E6650) and the
**	guarded std::ctype<unsigned short>::id initialiser 0x7E7E40 (guard 0xC416FC) that ends every object including
**	<string>; 0x7E81B0 (tables 0xB07744 / 0xB07730) opens the next object. The scalar deleting destructor 0x7E7EF0 sits
**	between the two constructors. Table 0xB075E4 (41 slots): ButtonGadgetClass's with +0x00 and +0x2C its own.
*/
