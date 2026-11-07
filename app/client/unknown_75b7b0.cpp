// Earth & Beyond client (reconstruction): app\client\unknown_75b7b0.cpp, PROVISIONAL file name (D137; notes at the end)
#include <string.h>
#include <math.h>
#include "unknown_75b7b0.h"
#include "..\..\wwlib\wwstring.h"
#include "..\..\wwmath\vector2.h"
#include "..\..\wwmath\vector3.h"
#include "..\..\wwmath\wwmath.h"
#include "..\..\ww3d2\rendobj.h"
#include "..\..\ww3d2\mesh.h"
#include "..\..\ww3d2\meshmdl.h"
#include "..\..\ww3d2\meshmatdesc.h"
#include "..\..\ww3d2\texture.h"
#include "..\..\ww3d2\vertmaterial.h"
#include "..\..\ww3d2\mapper.h"

// @0x75B7B0
void Unknown_75B7B0(RenderObjClass *obj, float phase)
{
	if (obj) {
		StringClass name(obj->Get_Name(), true);
		strlwr(name.Peek_Buffer());
		if (strncmp(name, "plt", 3) == 0) {
			Unknown_75BB50(obj, phase);
		}
	}
}

// NOT EXACT: functional (D64), 0.83: the x87 order of the cross product's length and the dot product (WE5)
// @0x75B8F0
void Unknown_75B8F0(RenderObjClass *obj, const Vector3 &sun, const Vector3 &camera)
{
	if (obj) {
		StringClass name(obj->Get_Name(), true);
		strlwr(name.Peek_Buffer());
		if (strncmp(name, "plt", 3) == 0) {
			Vector3 dir = camera - obj->Get_Position();
			dir.Normalize();
			Vector3 cross;
			Vector3::Cross_Product(dir, sun, &cross);
			float sine = cross.Length();
			float cosine = Vector3::Dot_Product(dir, sun);
			if (cross.Z < 0.0f) {
				sine = -sine;
			}
			float angle = WWMath::Atan2(sine, cosine);
			if (angle < 0.0f) {
				angle += (2.0f * WWMATH_PI);
			}
			float phase = 1.0f - angle * (1.0f / (2.0f * WWMATH_PI));
			cross.Length2();	// a dead read: the client's inline budget (operator= out of line)
			Unknown_75BB50(obj, phase);
		}
	}
}

// NOT EXACT: functional (D64), 0.37: ours calls StringClass::operator= where the client inlines it, so the registers differ (WE5)
// @0x75BB50
void Unknown_75BB50(RenderObjClass *obj, float phase)
{
	if (obj->Class_ID() == RenderObjClass::CLASSID_MESH) {
		MeshModelClass *model = ((MeshClass *)obj)->Get_Model();
		for (int pass = 0; pass < model->Get_Pass_Count(); pass++) {
			for (int stage = 0; stage < MeshMatDescClass::MAX_TEX_STAGES; stage++) {
				TextureClass *texture = model->Peek_Single_Texture(pass, stage);
				if (texture) {
					StringClass name((const char *)texture->Get_Texture_Name());
					strlwr(name.Peek_Buffer());
					if (strncmp(name, "plt_dn", 6) == 0) {
						TextureMapperClass *mapper = model->Peek_Single_Material(pass)->Peek_Mapper(stage);
						if (mapper && mapper->Mapper_ID() == TextureMapperClass::MAPPER_ID_LINEAR_OFFSET) {
							LinearOffsetTextureMapperClass *offset = (LinearOffsetTextureMapperClass *)mapper;
							offset->Set_UV_Offset_Delta(Vector2(0.0f, 0.0f));
							offset->Set_Current_UV_Offset(Vector2(phase, 0.0f));
						}
					}
				}
			}
		}
		if (model) {
			model->Release_Ref();
		}
	} else {
		int count = obj->Get_Num_Sub_Objects();
		for (int i = 0; i < count; i++) {
			RenderObjClass *sub = obj->Get_Sub_Object(i);
			Unknown_75BB50(sub, phase);
			if (sub) {
				sub->Release_Ref();
			}
		}
	}
}

/*
** Reconstruction notes (WE5, 2026-11-07)
**
** The object: no __FILE__ or assert; client 0x75B7B0-0x75BD68, after sound_manager.cpp's deque helpers (0x75B740) and
** before decal.cpp (0x75BE00). The planets' day / night: a 'plt' model's 'plt_dn' textures take a linear-offset
** mapper whose U offset is the planet's phase (0..1). 0x75B8F0 computes the phase from the angle, about the world Z
** axis, between the sun direction and the planet-to-camera direction; 0x75B7B0 sets a given phase. OrbitalManager
** 0x4F45A0 calls both; enbscene.cpp 0x75F690 calls 0x75B8F0 for each planet in its scene list.
*/
