// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\eff_pass.cpp (the client's __FILE__)
**
**	The pass effect (client 0x6ED6C0-0x6EEE90): see eff_pass.h. Notes at the end.
*/
#include "eff_pass.h"
#include "co.h"
#include "rendobj.h"
#include "camera.h"
#include "matpass.h"
#include "vertmaterial.h"
#include "texture.h"
#include "matrixmapper.h"
#include "part_emt.h"
#include "wwmath.h"
#include "..\shared\g_assert.h"

// co.cpp's render hook (vtable 0xB007A4): declared as far as this file calls it
class UnknownB007A4Class : public RenderHookClass	// NAME 1: neutral (D10), named by its vtable: co.cpp's
{
public:
	void Unknown_6BB8E0(const RefCountPtr<MaterialPassClass> &pass, bool flag);	// 0x6BB8E0 (the pass and its flag onto the hook's list)  // NAME 1 [pass, flag]: what Unknown_18 passes
	void Unknown_6BBA20(const RefCountPtr<MaterialPassClass> &pass);			// 0x6BBA20 (the pass off it)  // NAME 1 [pass]: what Unknown_20 passes
};



// 0x6ED6C0
PassEffectClass::PassEffectClass(ClientClass *client, unsigned id, unsigned start_time, unsigned duration, unsigned cycle_time, const RefCountPtr<MaterialPassClass> &pass, bool projection0, bool projection1) :
	UnknownB00060EffectClass(client, id, start_time, duration, cycle_time),
	Pass(pass)
{
	for (int i = 0; i < 2; i++) {
		Scale[i].Set(0.0f, 0.0f);
		Projection[i].Make_Identity();
	}
	Set_Flag(0x80, true);
	if (projection0) {
		VertexMaterialClass *material = Pass->Peek_Material();
		Mapper[0] = Create_NEW(new CompositeMatrixMapperClass(material->Peek_Mapper(0), 0));
		Mapper[0]->Set_Type(MatrixMapperClass::ORTHO_PROJECTION);
		material->Set_Mapper(Mapper[0].Peek(), 0);
	}
	if (projection1) {
		VertexMaterialClass *material = Pass->Peek_Material();
		Mapper[1] = Create_NEW(new CompositeMatrixMapperClass(material->Peek_Mapper(1), 1));
		Mapper[1]->Set_Type(MatrixMapperClass::ORTHO_PROJECTION);
		material->Set_Mapper(Mapper[1].Peek(), 1);
	}
}






// 0x6EDB70
Vector3 PassEffectClass::Unknown_08(const Vector3 &)
{
	return(Transform.Get_Translation());
}


// 0x6EDBB0
void PassEffectClass::Unknown_6EDBB0(const Vector2 &scale, unsigned stage)
{
	FAIL_IF(stage > 1) return;
	Scale[stage] = scale;
}

// 0x6EDC00
void PassEffectClass::Unknown_18()
{
	UnknownB00060EffectClass::Unknown_18();
	if (Get_Flag(10)) {
		Pass->Peek_Material()->Reset_Mappers();
	}
	if (SourceObject) {
		RefCountPtr<RenderObjClass> robj = Create_Get(SourceObject->Unknown_6F3BC0());
		if (robj) {






			UnknownB007A4Class *rend_hook = (UnknownB007A4Class *)robj->Get_Render_Hook(); FAIL_IF(!rend_hook) return;
			rend_hook->Unknown_6BB8E0(Pass, Get_Flag(9));
			if (Mapper[0]) {
				float x;
				float y;
				if (Get_Flag(13)) {
					x = y = Scale[0].X * 0.5f;
				} else if (Get_Flag(14)) {
					const Vector3 &extent = robj->Get_Bounding_Box().Extent;
					if (Get_Flag(12)) {
						x = Scale[0].X * extent.Y;
						y = Scale[0].Y * extent.X;
					} else if (Get_Flag(11)) {
						x = Scale[0].X * extent.X;
						y = Scale[0].Y * extent.Z;
					} else {
						x = Scale[0].X * extent.Y;
						y = Scale[0].Y * extent.Z;
					}
				} else {
					const Vector3 &extent = robj->Get_Bounding_Box().Extent;
					float radius;
					if (Get_Flag(12)) {
						radius = WWMath::Sqrt(extent.Y * extent.Y + extent.X * extent.X);
					} else if (Get_Flag(11)) {
						radius = WWMath::Sqrt(extent.X * extent.X + extent.Z * extent.Z);
					} else {
						radius = WWMath::Sqrt(extent.Y * extent.Y + extent.Z * extent.Z);
					}
					x = y = radius * Scale[0].X;
				}
				Projection[0].Init_Ortho(-x, x, -y, y, 0.1f, 1.0f);
			}
			if (Mapper[1]) {
				float x;
				float y;
				if (Get_Flag(17)) {
					x = y = Scale[1].X * 0.5f;
				} else if (Get_Flag(18)) {
					const Vector3 &extent = robj->Get_Bounding_Box().Extent;
					if (Get_Flag(16)) {
						x = Scale[1].X * extent.Y;
						y = Scale[1].Y * extent.X;
					} else if (Get_Flag(15)) {
						x = Scale[1].X * extent.X;
						y = Scale[1].Y * extent.Z;
					} else {
						x = Scale[1].X * extent.Y;
						y = Scale[1].Y * extent.Z;
					}
				} else {
					const Vector3 &extent = robj->Get_Bounding_Box().Extent;
					float radius;
					if (Get_Flag(16)) {
						radius = WWMath::Sqrt(extent.Y * extent.Y + extent.X * extent.X);
					} else if (Get_Flag(15)) {
						radius = WWMath::Sqrt(extent.X * extent.X + extent.Z * extent.Z);
					} else {
						radius = WWMath::Sqrt(extent.Y * extent.Y + extent.Z * extent.Z);
					}
					x = y = radius * Scale[1].X;
				}
				Projection[1].Init_Ortho(-x, x, -y, y, 0.1f, 1.0f);
			}
		}
	}
}




































// 0x6EE260
void PassEffectClass::Unknown_1C()
{
	UnknownB00060EffectClass::Unknown_1C();
	FAIL_IF(!Camera) return;
	if (Get_Flag(9) && SourceObject) {
		RefCountPtr<RenderObjClass> robj = Create_Get(SourceObject->Unknown_6F3BC0());
		if (robj) {
			Unknown_6EECD0(robj, true);
		}
	}
	if (Mapper[0]) {






		TextureClass *texture = Pass->Peek_Texture(0); FAIL_IF(!texture) return;
		int texsize = texture->Get_Width(); Matrix3D camera = Camera->Get_Transform();
		Matrix3D inverse;
		Matrix3D view;
		Matrix4 texture_tm;
		Matrix3D tm = Transform;
		if (Get_Flag(12)) {
			tm.Rotate_Z(-1.0f, 0.0f);
		} else if (Get_Flag(11)) {
			tm.Rotate_X(1.0f, 0.0f);
		} else {
			tm.Rotate_X(1.0f, 0.0f);
			tm.Rotate_Y(-1.0f, 0.0f);
		}


		tm.Get_Orthogonal_Inverse(inverse);
		Matrix3D::Multiply(inverse, camera, &view);
		Matrix4::Multiply(Projection[0], view, &texture_tm);
		Mapper[0]->Set_Texture_Transform(texture_tm, (float)texsize);
	}
	if (Mapper[1]) {






		TextureClass *texture = Pass->Peek_Texture(1); FAIL_IF(!texture) return;
		int texsize = texture->Get_Width(); Matrix3D camera = Camera->Get_Transform();
		Matrix3D inverse;
		Matrix3D view;
		Matrix4 texture_tm;
		Matrix3D tm = Transform;
		if (Get_Flag(16)) {
			tm.Rotate_Z(-1.0f, 0.0f);
		} else if (Get_Flag(15)) {
			tm.Rotate_X(1.0f, 0.0f);
		} else {
			tm.Rotate_X(1.0f, 0.0f);
			tm.Rotate_Y(-1.0f, 0.0f);
		}


		tm.Get_Orthogonal_Inverse(inverse);
		Matrix3D::Multiply(inverse, camera, &view);
		Matrix4::Multiply(Projection[1], view, &texture_tm);
		Mapper[1]->Set_Texture_Transform(texture_tm, (float)texsize);
	}
}










// 0x6EEB90
void PassEffectClass::Unknown_20()
{
	if (SourceObject) {
		RefCountPtr<RenderObjClass> robj = Create_Get(SourceObject->Unknown_6F3BC0());
		if (robj) {
			Unknown_6EECD0(robj, false);
			UnknownB007A4Class *rend_hook = (UnknownB007A4Class *)robj->Get_Render_Hook(); FAIL_IF(!rend_hook) return;
			rend_hook->Unknown_6BBA20(Pass);
		}
	}
}



// 0x6EECD0: the particle emitters in a model and its sub-objects shown or hidden
void PassEffectClass::Unknown_6EECD0(const RefCountPtr<RenderObjClass> &robj, bool invisible)
{
	FAIL_IF(!robj) return;
	if (robj->Class_ID() == RenderObjClass::CLASSID_PARTICLEEMITTER) {
		((ParticleEmitterClass *)robj.Peek())->Set_Invisible(invisible);
	} else {
		int count = robj->Get_Num_Sub_Objects();
		for (int i = 0; i < count; i++) {
			RefCountPtr<RenderObjClass> sub = Create_Get(robj->Get_Sub_Object(i));
			Unknown_6EECD0(sub, invisible);
		}
	}
}

/*
**	Reconstruction notes (O15, 2026-11-13)
**
**	The window: eff_particlejet.cpp's last function 0x6ED650 (FAIL_IF '!Emitter' l.161, its atexit static) ends
**	the object before; the guarded <string> static 0x6EEE70 ends this one, and eff_robj.cpp's constructor
**	0x6EEEA0 opens the next. Between the functions lie the out-of-line Matrix3D::Rotate_X / _Y / _Z
**	(0x6EE9B0 / 0x6EEA50 / 0x6EEAF0, called by Unknown_1C's second projection) and the reference pointer's
**	Create_NEW, constructor and destructor (0x6EEDF0 / 0x6EEE20 / 0x6EEE40).
**	The destructor is the implicit one (0x6EDA60 stores no vtable). 0x6EECD0 is a member (thiscall, this
**	unused): class 15 is CLASSID_PARTICLEEMITTER, its byte +0xFC IsInvisible. The axis tests run Z, then Y,
**	then X, as the factory's flags; the box sizes read the box's Extent through a reference; Unknown_1C takes
**	the texture's width into an int before the camera's transform. The render hook's two calls (0x6BB8E0,
**	0x6BBA20) take the pass by reference: co.cpp's UnknownB007A4PassStruct holds a reference pointer there
**	(0x6BB8E0 adds the reference), not the plain pointer co.cpp and unknown_6bb7e0.cpp declare.
*/
