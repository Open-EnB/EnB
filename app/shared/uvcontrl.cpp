// Earth & Beyond client (reconstruction): app\shared\uvcontrl.cpp. Notes at the end of the file.
#include "uvcontrl.h"
#include "g_assert.h"
#include "..\..\ww3d2\rendobj.h"
#include "..\..\ww3d2\matinfo.h"
#include "..\..\ww3d2\vertmaterial.h"
#include "..\..\ww3d2\mapper.h"
#include "..\..\wwmath\vector2.h"


Unknown4A0FB0Class::Unknown4A0FB0Class(RenderObjClass *robj, int index, int stage, int mapper_id) :
	RObj(NULL),
	Mapper(NULL)
{
	Unknown_4A10D0(robj, index, stage, mapper_id);
}


Unknown4A0FB0Class::~Unknown4A0FB0Class()
{
	REF_PTR_RELEASE(Mapper);
	REF_PTR_RELEASE(RObj);
}







































































void Unknown4A0FB0Class::Unknown_4A10D0(RenderObjClass *robj, int index, int stage, int mapper_id)
{
	REF_PTR_SET(RObj, robj);
	REF_PTR_RELEASE(Mapper);
	FAIL_IF(!RObj) return;

	MaterialInfoClass *matinfo = robj->Get_Material_Info();
	VertexMaterialClass *vmat = matinfo->Get_Vertex_Material(index);
	matinfo->Release_Ref();

	TextureMapperClass *mapper = vmat->Get_Mapper(stage);
	vmat->Release_Ref();





	FAIL_IF(!mapper) return;
	FAIL_IF(mapper->Mapper_ID() != mapper_id) {
		mapper->Release_Ref();
		return;
	}
	Mapper = mapper;
}






UVControllerClass::UVControllerClass(RenderObjClass *robj, int index, int stage) :
	Unknown4A0FB0Class(robj, index, stage, TextureMapperClass::MAPPER_ID_LINEAR_OFFSET)
{
	FAIL_IF(!Mapper) return;
	((LinearOffsetTextureMapperClass *)Mapper)->Set_UV_Offset_Delta(Vector2(0.0f, 0.0f));
}


UVControllerClass::~UVControllerClass()
{
}










void UVControllerClass::Unknown_4A13A0(const Vector2 &offset)
{
	FAIL_IF(!Mapper) return;
	((LinearOffsetTextureMapperClass *)Mapper)->Set_Current_UV_Offset(offset);
}





Unknown4A13F0Class::Unknown4A13F0Class(RenderObjClass *robj, int index, int stage) :
	Unknown4A0FB0Class(robj, index, stage, TextureMapperClass::MAPPER_ID_GRID)
{
	FAIL_IF(!Mapper) return;
	((GridTextureMapperClass *)Mapper)->Set_Frame_Per_Second(0.0f);
}
Unknown4A13F0Class::~Unknown4A13F0Class() {}
void Unknown4A13F0Class::Unknown_4A1550(unsigned int frame)
{
	FAIL_IF(!Mapper) return;
	((GridTextureMapperClass *)Mapper)->Set_Frame(frame);
}

/*
**	Reconstruction notes
**
**	client.exe 0x4A0FB0-0x4A157B, every function byte-exact (the FAIL_IFs on the client's lines 99, 112, 113, 128, 148, 159, 165, D25): the base's
**	constructor 0x4A0FB0 (inlined into both derived constructors), its destructor 0x4A1080 and scalar deleting destructor
**	0x4A1000 (table 0xAEAA18), Unknown_4A10D0 (the render object's material info -> vertex material index -> its mapper
**	of stage stage, kept when its Mapper_ID is the one asked for), UVControllerClass's constructor 0x4A1240
**	(MAPPER_ID_LINEAR_OFFSET, the offset rate zeroed), destructor 0x4A1350 / 0x4A1320 (table 0xAEAA20), Unknown_4A13A0
**	(Set_Current_UV_Offset), Unknown4A13F0Class's constructor 0x4A13F0 (MAPPER_ID_GRID, Set_Frame_Per_Second(0)),
**	destructor 0x4A1500 / 0x4A14D0 (table 0xAEAA24) and Unknown_4A1550 (Set_Frame). Caller: Unknown_671300Class
**	(gd_text_editor.cpp 0x671300 / 0x671410: the gadget's second render object, vertex material 0, stage 0).
*/
