// Earth & Beyond client (reconstruction): gd_uimesh.cpp (PROVISIONAL, D137). Notes at the end of the file.
#include "gd_uimesh.h"
#include "ui_mesh.h"
#include "rendobj.h"

UIMeshGadgetClass::UIMeshGadgetClass(UIMeshClass *mesh, const char *name, LayerClass &layer, unsigned int mask) :
	RenderObjGadgetClass(mesh->Get_Sub_Object_By_Name(name), layer, mask)
{
	Peek_Render_Obj()->Release_Ref();
}

UIMeshGadgetClass::UIMeshGadgetClass(RenderObjClass *robj, LayerClass &layer, unsigned int mask) :
	RenderObjGadgetClass(robj, layer, mask)
{
}

UIMeshGadgetClass::UIMeshGadgetClass(UIMeshClass *mesh, const char *name, unsigned int mask) :
	RenderObjGadgetClass(mesh->Get_Sub_Object_By_Name(name), mask)
{
	Peek_Render_Obj()->Release_Ref();
}

UIMeshGadgetClass::UIMeshGadgetClass(RenderObjClass *robj, unsigned int mask) :
	RenderObjGadgetClass(robj, mask)
{
}

UIMeshGadgetClass::~UIMeshGadgetClass()
{
}

ButtonGadgetClass::ButtonGadgetClass(UIMeshClass *mesh, const char *name, const LayerClass &layer, unsigned int mask, unsigned int flags) :
	Unknown_66B770Class(mesh->Get_Sub_Object_By_Name(name), layer, mask, flags)
{
	Peek_Render_Obj()->Release_Ref();
}

ButtonGadgetClass::ButtonGadgetClass(RenderObjClass *robj, const LayerClass &layer, unsigned int mask, unsigned int flags) :
	Unknown_66B770Class(robj, layer, mask, flags)
{
}

ButtonGadgetClass::ButtonGadgetClass(UIMeshClass *mesh, const char *name, unsigned int mask, unsigned int flags) :
	Unknown_66B770Class(mesh->Get_Sub_Object_By_Name(name), mask, flags)
{
	Peek_Render_Obj()->Release_Ref();
}

ButtonGadgetClass::ButtonGadgetClass(RenderObjClass *robj, unsigned int mask, unsigned int flags) :
	Unknown_66B770Class(robj, mask, flags)
{
}

ButtonGadgetClass::~ButtonGadgetClass()
{
}

/*
**	Reconstruction notes (client 0x674F90-0x675370; PROVISIONAL file, D137).
**
**	The file between gd_uiback.cpp (its last assert at 0x674DE0) and the class at 0x675370 (its
**	constructor calls 0x675050): no string or assert names it, so the name is neutral and the placement
**	interim. The constructors over a UIMeshClass take its sub-object by name (0x5B8B40, a new
**	reference) and drop that reference once the base holds the model. The slots +0x74 / +0x78 are
**	inline: their copies (0x51ADC0 / 0x51ADE0, 0x58E5B0 / 0x58E5D0) are the ones other objects emitted
**	first. 0x674F00 / 0x674F30 (RefCountPtr's Create_NEW and destructor) before 0x674F90 may be this
**	object's or gd_uiback.cpp's.
*/
