// Earth & Beyond client (reconstruction): app\client\enbscene.cpp (the client's __FILE__), so far 0x75F690-0x75F852 only (notes at the end)
#include "..\..\wwlib\multilist.h"
#include "..\..\wwmath\vector3.h"
#include "..\..\ww3d2\scene.h"
#include "..\..\ww3d2\rendobj.h"
#include "..\..\ww3d2\part_emt.h"
#include "unknown_75b7b0.h"

class MainViewSceneUnknown98Class;

// the main view's scene (0x75ECF0, 0xF4 bytes), as far as these functions use it
class MainViewSceneClass : public SimpleSceneClass	// NAME 1: neutral (D10): mainview.cpp's
{
public:
	void Unknown_75F690(const Vector3 &sun, const Vector3 &camera);	// 0x75F690 (mainview.cpp's Update, space mode)  // NAME 1 [sun, camera]: what Update passes
	void Unknown_75F700(const Vector3 &color);	// 0x75F700 (mainview.cpp's Update, planet mode)  // NAME 1 [color]: what Update passes
	MainViewSceneUnknown98Class *Unknown98;			// +0x98
	char Unknown9C[0xA8 - 0x9C];
	RefMultiListClass<RenderObjClass> UnknownA8;	// +0xA8 the 'plt' models (Add_Render_Object 0x75F180)  // NAME 1: neutral (D10)
	RefMultiListClass<RenderObjClass> UnknownC0;	// +0xC0  // NAME 1: neutral (D10)
	char UnknownD8[0xF4 - 0xD8];
};

static void Unknown_75F750(RenderObjClass *obj, const Vector3 &color);	// NAME 1 [obj, color]: neutral (D10)

// @0x75F690
void MainViewSceneClass::Unknown_75F690(const Vector3 &sun, const Vector3 &camera)
{
	RefMultiListIterator<RenderObjClass> it(&UnknownA8);
	for (it.First(); !it.Is_Done(); it.Next()) {
		Unknown_75B8F0(it.Peek_Obj(), sun, camera);
	}
}

// @0x75F700
void MainViewSceneClass::Unknown_75F700(const Vector3 &color)
{
	RefMultiListIterator<RenderObjClass> it(&UnknownC0);
	for (it.First(); !it.Is_Done(); it.Next()) {
		Unknown_75F750(it.Peek_Obj(), color);
	}
}

// @0x75F750
static void Unknown_75F750(RenderObjClass *obj, const Vector3 &color)
{
	if (obj->Class_ID() == RenderObjClass::CLASSID_PARTICLEEMITTER) {
		ParticleEmitterClass *emitter = (ParticleEmitterClass *)obj;
		ParticlePropertyStruct<Vector3> colors;
		emitter->Get_Color_Key_Frames(colors);
		for (unsigned int i = 0; i < colors.NumKeyFrames - 1; i++) {
			colors.Values[i] = color;
		}
		colors.Values[colors.NumKeyFrames - 1].Set(0.0f, 0.0f, 0.0f);
		emitter->Reset_Colors(colors);
		if (colors.KeyTimes) {
			delete [] colors.KeyTimes;
		}
		if (colors.Values) {
			delete [] colors.Values;
		}
	} else {
		int count = obj->Get_Num_Sub_Objects();
		for (int i = 0; i < count; i++) {
			RenderObjClass *sub = obj->Get_Sub_Object(i);
			Unknown_75F750(sub, color);
			if (sub) {
				sub->Release_Ref();
			}
		}
	}
}

/*
** Reconstruction notes (WE5, 2026-11-07)
**
** Only 0x75F690-0x75F852 so far: the constructor 0x75ECF0, Add / Remove_Render_Object (0x75F180, 0x75F440 with the
** file's one assert, line 106) and the rest of the file are not built yet, so this file has no assert to place and the
** functions sit at no particular line. 0x75F690 gives each 'plt' model in the +0xA8 list its day / night phase
** (unknown_75b7b0.cpp's 0x75B8F0) from the sun direction and the camera position; 0x75F700 sets every colour key but
** the last of each particle emitter in the +0xC0 list (the planet mode's clouds?) to the given colour, the last to black.
*/
