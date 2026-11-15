// Earth & Beyond client (reconstruction): app\client\ui_hdamg.cpp. Notes at the end of the file.
#define __DELTA_ARRAY_MEMBERS	// delta_array.h's Get_Value
#include "ui_hdamg.h"
#include "co.h"
#include "rendobj.h"
#include "matinfo.h"
#include "vertmaterial.h"
#include "mapper.h"
#include "vector2.h"
#include "wwmath.h"
#include "ww3d.h"
#include "..\shared\auxdata\auxdata.h"
#include "..\shared\auxdata\delta_array.h"
#include "..\shared\message.h"
#include "..\shared\g_assert.h"

// 0x59BF30
UIHullDamageClass::UIHullDamageClass(RenderObjClass *model) :
	Object(NULL)
{
	Quadrant[0].Init(model, "rt", "rb");
	Quadrant[1].Init(model, "br", "bl");
	Quadrant[2].Init(model, "lb", "lt");
	Quadrant[3].Init(model, "tl", "tr");
	for (int i = 0; i < 4; i++) {
		Quadrant[i].Set(0.0f, true);
	}
}

// 0x59C0B0
UIHullDamageClass::~UIHullDamageClass()
{
	Set_Object(NULL);
}

// 0x59C150
void UIHullDamageClass::Set_Object(ClientObjectClass *object)
{
	if (object != Object) {
		Object = object;
		for (int i = 0; i < 4; i++) {
			Quadrant[i].Set(0.0f, true);
		}
	}
}

















































template<class C> inline C *Unknown_Find_Valid(DeltaCompoundClass *root, const NameClass &name) { C *t = Unknown_Find_Field<C>(root, name); if (t && t->Unknown70) return(t); return(NULL); }	// NAME 1: neutral (D10), as co.cpp's

// 0x59C190
void UIHullDamageClass::Update()
{
	static int frame = 0;	// 0xBE5C00  // NAME 1: neutral (D10)

	frame++;
	if (Object) {
		DeltaCompoundClass *aux = (DeltaCompoundClass *)Object->AuxData;
		NameClass name("VisibleQuadrantDamagePercent");
		DeltaArrayClass<float> *damage = Unknown_Find_Valid<DeltaArrayClass<float> >(aux, name);
		if (damage) {
			for (int i = 0; i < 4; i++) {
				float value = WWMath::Clamp(damage->Get_Value(i), 0.0f, 1.0f);
				Quadrant[i].Set(value, false);
				Message_Stat("", "HullDamage", __FILE__, __LINE__, (frame & 1) ? 0 : 30, i, "%2.2f -- %2.2f", value, damage->Get_Value(i));
			}
		}
	} return; Unknown_Find_Field<DeltaArrayClass<float> >(NULL, *(NameClass *)this); Unknown_Find_Field<DeltaArrayClass<float> >(NULL, *(NameClass *)this); Unknown_Find_Field<DeltaArrayClass<float> >(NULL, *(NameClass *)this);	// D130: dead spenders after the find send rtti.h's Cast (0x59C670) out of line
}

// 0x59C3B0
void UIHullDamageQuadrantClass::Init(RenderObjClass *model, const char *name0, const char *name1)
{
	Mapper0 = Find_Mapper(model, name0);
	Mapper1 = Find_Mapper(model, name1);
}

// 0x59C3F0
UIHullDamageQuadrantClass::~UIHullDamageQuadrantClass()
{
	if (Mapper0) Mapper0->Release_Ref();
	if (Mapper1) Mapper1->Release_Ref();
}












#pragma auto_inline(off)	// the client calls Find_Mapper and Set out of line (no client caller inlines them)
// 0x59C430
LinearOffsetTextureMapperClass *UIHullDamageQuadrantClass::Find_Mapper(RenderObjClass *model, const char *name)
{
	RenderObjClass *robj = model->Get_Sub_Object_By_Name(name);
	MaterialInfoClass *info = robj->Get_Material_Info();
	robj->Release_Ref();
	VertexMaterialClass *vmat = info->Get_Vertex_Material(0);
	info->Release_Ref();
	TextureMapperClass *mapper0 = vmat->Get_Mapper(0);
	TextureMapperClass *mapper1 = vmat->Get_Mapper(1);
	vmat->Release_Ref();

	LinearOffsetTextureMapperClass *mapper;
	if (mapper0 && mapper0->Mapper_ID() == TextureMapperClass::MAPPER_ID_LINEAR_OFFSET) {
		mapper = (LinearOffsetTextureMapperClass *)mapper0;
	} else if (mapper1 && mapper1->Mapper_ID() == TextureMapperClass::MAPPER_ID_LINEAR_OFFSET) {
		mapper = (LinearOffsetTextureMapperClass *)mapper1;
	} else {
		G_ASSERT(false);
		return(NULL);
	}
	mapper->Set_UV_Offset_Delta(Vector2(0.0f, 0.0f));
	return(mapper);
}

// 0x59C560
void UIHullDamageQuadrantClass::Set(float damage, bool immediate)
{
	damage = damage * 0.4f - 0.55f;
	if (immediate) {
		Offset.Set_Value(damage, true);
	} else if (Last != damage) {
		Offset.Set_Target(damage, 0.0001f);
	}
	Last = damage;
	Offset.Update_Changed(WW3D::Get_Frame_Time());
	Vector2 uv;
	uv.Set(Offset.Current, 0.0f);
	Mapper0->Set_Current_UV_Offset(uv);
	Mapper1->Set_Current_UV_Offset(uv);
}
#pragma auto_inline(on)

/*
**	Reconstruction notes (ui_hdamg.cpp, client 0x59BF30-0x59C660; __FILE__ 0xB80034, Message_Stat 'HullDamage' on the
**	client's line 111 in 0x59C190, G_ASSERT 'false' on line 161 in 0x59C430).
**
**	- The object: the constructor 0x59BF30 opens it (0x59BF20 is int3 padding after the previous object), the out-of-line
**	  UnknownRTTIClass<DeltaArrayClass<float> >::Cast 0x59C670 (rtti.h's COMDAT) and the guarded ctype initialiser
**	  0x59C640 close it; 0x59C6F0 (an InterfaceClass constructor's caller, tables 0xAF20B8..0xAF20D0) is the next object.
**	- No client function calls the constructor, the destructor, Set_Object or the table 0xAF2028 (one slot, 0x59C190):
**	  the class is compiled but unused in the retail client. The names are from what the code does (NAME 2).
**	- Each quadrant finds two sub objects of the model by name ('rt' / 'rb', 'br' / 'bl', 'lb' / 'lt', 'tl' / 'tr'), the
**	  linear-offset mapper (MAPPER_ID_LINEAR_OFFSET, Mapper_ID 1) on stage 0 or 1 of each one's first vertex material,
**	  stops its scroll (Set_UV_Offset_Delta(0, 0)), and Set slides both mappers' U offset to damage * 0.4 - 0.55 through
**	  an AccelClass (target accel 0.0001, WW3D::Get_Frame_Time()).
**	- Find_Mapper and Set are bracketed by auto_inline(off): the client calls them from every site (workflow.md 3.1.2);
**	  Init has no caller and is inlined into the constructor.
**	- Update's 'VisibleQuadrantDamagePercent' lookup is co.cpp's Unknown_Find_Valid layer; 3 dead finds (root NULL: no code,
**	  D130) after the find spend the nested inline budget so rtti.h's Cast goes out of line (0x59C670), matching Update
**	  0x59C190 byte for byte.
*/
