#include "rs_track.h"
#include "matpass.h"
#include "vertmaterial.h"
#include "assetmgr.h"
#include "texture.h"
#include "shader.h"
#include "rinfo.h"
#include <math.h>
#include "wwmath.h"
#include "..\shared\g_assert.h"


// @0x458C10
Unknown458C10Class::Unknown458C10Class(RenderStateListClass *list, int id, int link_id, unsigned int flags, bool two_sided, bool hidden, const char *texture, bool a, bool b, ShaderClass shader) :
	RenderStateClass(list, id, link_id, flags),
	MaterialPass(new MaterialPassClass()),
	TwoSided(two_sided),
	Hidden(hidden)
{
	VertexMaterialClass *vmat = new VertexMaterialClass();
	MaterialPass->Set_Material(vmat);
	vmat->Release_Ref();

	TextureClass *tex = WW3DAssetManager::Get_Instance()->Get_Texture(texture);
	tex->Set_U_Addr_Mode((TextureClass::TxtAddrMode)(a != 0));
	tex->Set_V_Addr_Mode((TextureClass::TxtAddrMode)(b != 0));
	MaterialPass->Set_Texture(tex);
	if (tex != NULL) {
		tex->Release_Ref();
	}
	MaterialPass->Set_Shader(shader);
}

// @0x458DD0
Unknown458C10Class::Unknown458C10Class(RenderStateListClass *list, const Unknown458C10Class &source) :
	RenderStateClass(list, source),
	MaterialPass(new MaterialPassClass()),
	TwoSided(source.TwoSided),
	Hidden(source.Hidden)
{
	VertexMaterialClass *vmat = new VertexMaterialClass(*source.MaterialPass->Peek_Material());
	MaterialPass->Set_Material(vmat);
	vmat->Release_Ref();

	MaterialPass->Set_Texture(source.MaterialPass->Peek_Texture(), 0);
	MaterialPass->Set_Shader(source.MaterialPass->Peek_Shader());
}

// @0x458F10
Unknown458C10Class::~Unknown458C10Class()
{
	if (MaterialPass != NULL) {
		MaterialPass->Release_Ref();
	}
	MaterialPass = NULL;
}

// @0x458FA0
RenderStateClass *Unknown458C10Class::Unknown_04(RenderStateListClass *list)
{
	return new Unknown458C10Class(list, *this);
}

// @0x459030
void Unknown458C10Class::Unknown_14(RsTrackClass *owner)
{
	Unknown_457E10();
	Flags &= ~0x80;
}

// @0x459060
void Unknown458C10Class::Unknown_18(RsTrackClass *owner)
{
	Unknown_457E90();
}

// @0x459080
void Unknown458C10Class::Unknown_28(int a, int b)
{
	RenderInfoClass &rinfo = *(RenderInfoClass *)b;
	rinfo.Push_Material_Pass(MaterialPass);
	int two_sided = TwoSided ? 1 : 0;
	int hidden = Hidden ? 6 : 0;
	rinfo.Push_Override_Flags((RenderInfoClass::RINFO_OVERRIDE_FLAGS)(rinfo.Current_Override_Flags() | hidden | two_sided));
}

// @0x4590F0
void Unknown458C10Class::Unknown_2C(int a, int b)
{
	RenderInfoClass &rinfo = *(RenderInfoClass *)b;
	rinfo.Pop_Override_Flags();
	rinfo.Pop_Material_Pass();
}

// @0x459120
void Unknown458C10Class::Unknown_459120(TextureMapperClass *mapper)
{
	MaterialPass->Peek_Material()->Set_Mapper(mapper);
}

// @0x459170
TextureMapperClass *Unknown458C10Class::Unknown_459170()
{
	return MaterialPass->Peek_Material()->Get_Mapper();
}

// @0x4591A0
Unknown4591A0Class::Unknown4591A0Class(RenderStateListClass *list, int id, int link_id, unsigned int flags, int next_id, bool two_sided, bool hidden, const char *texture, int a, int b, ShaderClass shader, float cycle_time, float start_v, float end_v) :
	Unknown458C10Class(list, id, link_id, flags, two_sided, hidden, texture, 1, 1, shader),
	CurrentTime(0.0f),
	NextID(next_id),
	NextRenderState(NULL)
{
	if (cycle_time > 0.0f) {
		if (cycle_time < 0.001f) {
			cycle_time = 1.0f;
		}
		TimeRate = 1.0f / cycle_time;
	} else {
		TimeRate = 0.0f;
	}

	start_v = WWMath::Clamp(start_v, 0.0f, 0.9999f);
	end_v = WWMath::Clamp(end_v, 0.0f, 0.9999f);

	StartV = start_v;
	DeltaV = end_v - start_v;
}

// @0x459340
Unknown4591A0Class::~Unknown4591A0Class()
{
}

// @0x4593D0
Unknown4591A0Class::Unknown4591A0Class(RenderStateListClass *list, const Unknown4591A0Class &source) :
	Unknown458C10Class(list, source),
	CurrentTime(source.CurrentTime),
	TimeRate(source.TimeRate),
	StartV(source.StartV),
	DeltaV(source.DeltaV),
	NextID(source.NextID),
	NextRenderState(NULL)
{
}

// @0x459440
void Unknown4591A0Class::Unknown_18(RsTrackClass *owner)
{
	Unknown_457E90();
	CurrentTime = 0.0f;
}

// @0x459630


























































































// @0x459630
RenderStateClass *Unknown4591A0Class::Unknown_459630()
{
	if (NextID == -1) {
		return NULL;
	}
	if (NextRenderState == NULL) {
		NextRenderState = List->Unknown_45A7E0(NextID);
		G_ASSERT(NextRenderState);
	}
	return NextRenderState;
}

// @0x459470
void Unknown4591A0Class::Unknown_30(RsTrackClass *owner, float seconds)
{
	float new_time = CurrentTime + seconds * TimeRate;
	if (new_time >= 1.0f) {
		float remaining = seconds - (1.0f - CurrentTime) / TimeRate;
		Unknown_4584B0(owner, remaining, Unknown_459630());
		if (Flags & 0x10) {
			if (!(Flags & 0x44)) {
				new_time = (float)(new_time - floor(new_time));
			} else {
				new_time = 1.0f;
			}
		} else {
			new_time = 0.0f;
		}
	}
	CurrentTime = new_time;
	Unknown_4C(StartV + CurrentTime * DeltaV);
}

// @0x459590
void Unknown4591A0Class::Unknown_34(float seconds)
{
	CurrentTime = seconds * TimeRate;
}

// @0x4595B0
void Unknown4591A0Class::Unknown_3C()
{
	if (Flags & 1) {
		RenderStateClass *next = Unknown_459630();
		if (next != NULL) {
			next->Unknown_3C();
		}
	} else {
		Flags |= 0x20;
	}
}
