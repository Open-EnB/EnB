// Earth & Beyond client (reconstruction): app\client\slot_data_render.cpp, what draws a slot's item
// (the client's file per its asserts, 'e:\build\beta\G\app\Client\slot_data_render.cpp' at 0xB91684). Notes at the end.
#include "assetmgr.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include "..\shared\g_assert.h"
#include "slot_data_render.h"
#include "skill_render.h"
#include "rendobj.h"
#include "texture.h"
#include "vector3.h"

class SceneClass;

// gd_slot_data.cpp's: its RefCountClass sits at +0x18 (the `inc [edi+0x1C]`, `lea ecx,[eax+0x18]`)
class Unknown00Base { public: virtual void Unknown_00(); char Unknown04[0x14]; };	// NAME 1: neutral (D10)
class SlotDataClass : public Unknown00Base, public RefCountClass {};	// NAME 3: 0x6A7310's FAIL_IF
// What 0x6A90A0's callers (0x62DF90, 0x62EF80, 0x631000, 0x631270) pass: slots +0x90 and +0x94, +0x98 the data
class Unknown6A90A0Item { public: char Unknown00[0xBC]; int Unknown_BC; };	// NAME 1: neutral (D10); NAME 1 [Unknown_BC]: the item's kind (0, 10; 1, 11; 7; 12, 13)
class Unknown6A90A0Interface { public: virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C(); virtual void Unknown_10(); virtual void Unknown_14(); virtual void Unknown_18(); virtual void Unknown_1C(); virtual void Unknown_20(); virtual void Unknown_24(); virtual void Unknown_28(); virtual void Unknown_2C(); virtual void Unknown_30(); virtual void Unknown_34(); virtual void Unknown_38(); virtual void Unknown_3C(); virtual void Unknown_40(); virtual void Unknown_44(); virtual void Unknown_48(); virtual void Unknown_4C(); virtual void Unknown_50(); virtual void Unknown_54(); virtual void Unknown_58(); virtual void Unknown_5C(); virtual void Unknown_60(); virtual void Unknown_64(); virtual void Unknown_68(); virtual void Unknown_6C(); virtual void Unknown_70(); virtual void Unknown_74(); virtual void Unknown_78(); virtual void Unknown_7C(); virtual void Unknown_80(); virtual void Unknown_84(); virtual void Unknown_88(); virtual void Unknown_8C(); virtual Unknown6A90A0Item *Unknown_90(); virtual void *Unknown_94(); SlotDataClass *Get_Data() { return(Unknown98); } char Unknown04[0x94]; SlotDataClass *Unknown98; };	// NAME 1: neutral (D10); NAME 3 [iface]: its FAIL_IF
#include "inv_trade_render.h"


#include "inv_manu_render.h"


// ro_funcs.cpp's
void Unknown_496AE0(RenderObjClass *robj, int a);	TextureClass *Unknown_4960C0(RenderObjClass *robj, int a);	// NAME 1: neutral (D10); NAME 1 [robj, a]: neutral
void Unknown_495FD0(RenderObjClass *robj, TextureClass *texture, int a);	void Unknown_496730(RenderObjClass *robj, int a, bool b);	// NAME 1: neutral (D10); NAME 1 [robj, texture, a, b]: neutral
bool Unknown_496110(RenderObjClass *robj, float a, float b, float c);	// NAME 1: neutral (D10); NAME 1 [robj, a, b, c]: neutral

void Unknown_6A93E0(const char *value, std::string &string);	const char *Unknown_6A9660(const char *serial_string, std::string &string);	// NAME 1: neutral (D10)

// 0x6A7B60
SlotDataRenderClass::SlotDataRenderClass() :
	SlotData(0),
	Mesh(0),
	Container(0),
	Unknown1C(0),
	Unknown20(-1),
	Unknown24(0)
{
	AddedToScene = false;
	IsSubIcon = false;
}

// 0x6A7C00
bool SlotDataRenderClass::Unknown_08(SlotDataClass *sd, RenderObjClass *robj)
{
	REF_PTR_SET(SlotData, sd);




































	FAIL_IF(!SlotData) return(false);

	RenderObjClass *loaded = NULL;
	if (!robj) {
		loaded = WW3DAssetManager::Get_Instance()->Create_Render_Obj(Unknown_48());
		if (loaded) {
			robj = loaded->Get_Sub_Object(0);
			if (robj) Unknown_496AE0(robj, 0);
		}
	} else {
		robj->Add_Ref();
	}
	REF_PTR_SET(Mesh, robj);








	FAIL_IF(!Mesh) {
		if (loaded) loaded->Release_Ref();
		return(false);
	}

	Unknown24 = Unknown_4960C0(Mesh, 0);
	if (Unknown24) {
		Unknown24->Add_Ref();
	} else {
		Unknown24 = WW3DAssetManager::Get_Instance()->Get_Texture("BLANK4.TGA", MIP_LEVELS_1, WW3D_FORMAT_UNKNOWN, true);
	}

	Container = Mesh->Get_Container();
	if (Container) {

		G_ASSERT(!loaded || (Container == loaded));
		Container->Add_Ref();
		Unknown1C = Container->Get_Sub_Object_Bone_Index(Mesh);
	} else {


		G_ASSERT(false);
		Unknown1C = 0;
	}

	Unknown10 = Create_NEW(WW3DAssetManager::Get_Instance()->Create_Render_Obj("inv_blank01"));
	if (Unknown10) Unknown_496AE0(Unknown10.Peek(), 0);
	Mask = Create_NEW(WW3DAssetManager::Get_Instance()->Create_Render_Obj("inv_blank01"));
	if (Mask) {
		Unknown_496AE0(Mask.Peek(), 0);
		RefCountPtr<TextureClass> texture = Create_NEW(WW3DAssetManager::Get_Instance()->Get_Texture("icon_mask.tga", MIP_LEVELS_1, WW3D_FORMAT_UNKNOWN, true));
		if (texture) Unknown_495FD0(Mask.Peek(), texture.Peek(), 0);
	}

	if (Mesh) Mesh->Set_Sort_Level(Mesh->Get_Sort_Level() - 1);
	if (Unknown10) Unknown10->Set_Sort_Level(Mesh->Get_Sort_Level() + 1);
	if (Mask) Mask->Set_Sort_Level(Mesh->Get_Sort_Level() - 2);

	if (loaded) loaded->Release_Ref();
	robj->Release_Ref();
	return(Container != NULL);
}

// 0x6A80D0
SlotDataRenderClass::~SlotDataRenderClass()
{








	FAIL_IF(AddedToScene) {
		Unknown_6A8C40();
	}
	Unknown_495FD0(Mesh, Unknown24, 0);
	Unknown_6A8A40(0);
	if (Unknown10) Container->Remove_Sub_Object(Unknown10.Peek());
	Unknown10 = NULL;
	Mask = NULL;
	REF_PTR_RELEASE(Unknown24);
	REF_PTR_RELEASE(Mesh);
	REF_PTR_RELEASE(Container);
	REF_PTR_RELEASE(SlotData);
	Unknown1C = 0;
}

// 0x6A82F0
void SlotDataRenderClass::Unknown_0C(std::string &string)
{
	string.erase();
	char buffer[12];
	sprintf(buffer, "%d", Unknown20);
	Unknown_6A93E0(buffer, string);
}

// 0x6A8430
const char *SlotDataRenderClass::Unknown_10(const char *state)
{





	FAIL_IF(!state) return(0);
	std::string string;
	state = Unknown_6A9660(state, string);
	Unknown_40(atoi(string.c_str()));
	return(state);
}
// 0x6A8610
const char *SlotDataRenderClass::Unknown_28(std::string *string)
{
	string->erase();
	return(0);
}
// 0x6A8730
void SlotDataRenderClass::Unknown_40(int slot)
{
	Unknown20 = slot;
	Unknown_2C(true);
}

// 0x6A8760
void SlotDataRenderClass::Unknown_44(SlotDataRenderClass *render)
{

	FAIL_IF(!render) return;
	Unknown_40(render->Unknown20);
}

// 0x6A87A0
void SlotDataRenderClass::Unknown_2C(bool force)
{
	if (!force) return;

	int slot = Get_Slot();
	RefCountPtr<TextureClass> texture;
	if (slot == -1) {
		const char *name = Unknown_4C();
		if (name && *name) texture = Create_NEW(WW3DAssetManager::Get_Instance()->Get_Texture(name, MIP_LEVELS_1, WW3D_FORMAT_UNKNOWN, true));
	} else {
		texture = Create_NEW(Unknown_34());
		Vector3 color;
		if (Unknown_38(color)) Unknown_496110(Mesh, color.X, color.Y, color.Z);
	}
	if (texture) {
		Unknown_495FD0(Mesh, texture.Peek(), 0);
	} else {
		Unknown_495FD0(Mesh, Unknown24, 0);
		if (Unknown10) Container->Remove_Sub_Object(Unknown10.Peek());
	}
}

// 0x6A8980
const char *SlotDataRenderClass::Unknown_4C()
{
	return("item_dark.TGA");
}

// 0x6A89A0
int SlotDataRenderClass::Unknown_30(int level)
{
	Unknown_496730(Mesh, level - 1, false);
	return(level);
}

// 0x6A89D0
void SlotDataRenderClass::Unknown_6A89D0(TextureClass *texture)
{
	if (texture) {
		Unknown_495FD0(Mesh, texture, 0);
	} else {
		Unknown_495FD0(Mesh, Unknown24, 0);
		if (Unknown10) Container->Remove_Sub_Object(Unknown10.Peek());
	}
}

// 0x6A8A40
void SlotDataRenderClass::Unknown_6A8A40(TextureClass *texture)
{
	Get_Slot();	Get_Slot();	Get_Slot();	Get_Slot();	Get_Slot();	Get_Slot();	Get_Slot();	// (see the notes: seven dead reads keep this out of line in the destructor)
	if (Unknown10) {
		Container->Remove_Sub_Object(Unknown10.Peek());
		if (texture && Mesh) {
			Unknown_495FD0(Unknown10.Peek(), texture, 0);
			Container->Add_Sub_Object_To_Bone(Unknown10.Peek(), Unknown1C);
		}
	}
}

// 0x6A8AB0
void SlotDataRenderClass::Unknown_6A8AB0(bool on)
{
	if (Mask) {
		if (on) {



			FAIL_IF(Mask->Get_Container() != 0) return;
			Container->Add_Sub_Object_To_Bone(Mask.Peek(), Unknown1C);
		} else {
			Container->Remove_Sub_Object(Mask.Peek());
		}
	}
}

// 0x6A8B30
int SlotDataRenderClass::Unknown_6A8B30()
{
	if (Mesh) return(Mesh->Get_Sort_Level());
	return(8);
}
// 0x6A8B60
void SlotDataRenderClass::Unknown_6A8B60(SceneClass *scene)
{
	FAIL_IF(!scene) return;

	RenderObjClass *robj = Container;
	FAIL_IF(!robj) return;

	G_ASSERT(!AddedToScene);
	G_ASSERT(!robj->Peek_Scene());
	robj->Add(scene);
	AddedToScene = true;
	Unknown_2C(true);
}
// 0x6A8C40
void SlotDataRenderClass::Unknown_6A8C40()
{
	if (AddedToScene) {
		RenderObjClass *robj = Container;

		FAIL_IF(!robj) return;

		G_ASSERT(robj->Peek_Scene());
		robj->Remove();
		AddedToScene = false;
	}
}

// 0x6A8CC0
void SlotDataRenderClass::Unknown_6A8CC0(const Vector3 &position)
{

	FAIL_IF(!Container) return;
	Container->Set_Position(position);
}

// 0x6A8D00
void SlotDataRenderClass::Unknown_6A8D00(SlotDataRenderClass *icon)
{
	FAIL_IF(!icon) return;

	G_ASSERT(!icon->IsSubIcon);

	RenderObjClass *mine = Container;
	FAIL_IF(!mine) return;

	RenderObjClass *his = icon->Container;
	FAIL_IF(!his) return;

	mine->Add_Sub_Object_To_Bone(his, Unknown1C);
	icon->Unknown_30(Mesh->Get_Sort_Level() - 1);
	icon->IsSubIcon = true;
}

// 0x6A8DF0
void SlotDataRenderClass::Unknown_6A8DF0(SlotDataRenderClass *icon)
{
	FAIL_IF(!icon) return;

	FAIL_IF(!icon->IsSubIcon) return;

	RenderObjClass *mine = Container;
	FAIL_IF(!mine) return;

	RenderObjClass *his = icon->Container;
	FAIL_IF(!his) return;

	mine->Remove_Sub_Object(his);
	icon->IsSubIcon = false;
}

// 0x6A8EC0
void SlotDataRenderClass::Unknown_6A8EC0()
{
	if (Unknown10) {
		Unknown10->Remove();
		Unknown10 = NULL;
	}
}

// 0x6A8F00
void SlotDataRenderClass::Unknown_6A8F00(RenderObjClass *robj)
{














	FAIL_IF(!robj) return;

	RenderObjClass *container = Container;
	FAIL_IF(!container) return;

	if (!robj->Get_Container()) {
		container->Add_Sub_Object_To_Bone(robj, Unknown1C);
		robj->Set_Sort_Level(Mesh->Get_Sort_Level() - 1);
	} else {
		G_ASSERT(robj->Get_Container() == container);
	}
}

// 0x6A8FE0
void SlotDataRenderClass::Unknown_6A8FE0(RenderObjClass *robj)
{











	FAIL_IF(!robj) return;

	RenderObjClass *his = robj->Get_Container();
	if (his) {
		RenderObjClass *mine = Container;


		FAIL_IF(!mine) return;

		FAIL_IF(his != mine) return;

		mine->Remove_Sub_Object(robj);
	}
}

// 0x6A90A0
SlotDataRenderClass *Unknown_6A90A0(int kind, Unknown6A90A0Interface *iface, RenderObjClass *robj)
{

	FAIL_IF(!iface) return(0);

	Unknown6A90A0Item *item = iface->Unknown_90();
	void *skill = iface->Unknown_94();
	SlotDataRenderClass *render = NULL;

	switch (kind) {
		case 1:
			if (item) {
				switch (item->Unknown_BC) {
					case 0:
					case 10:
						render = new Unknown68D870Class;
						break;
					case 1:
					case 11:
						render = new Unknown68DC20Class;
						break;
				}
			} else if (skill) {
				render = new SkillRenderClass;
			}
			break;

		case 0:
		case 2:
			if (item) {
				switch (item->Unknown_BC) {
					case 12:
					case 13:
						render = new InvManuRenderClass;
						break;
					case 7:
						if (kind == 0) {
							render = new InvTradeRenderClass;
						} else {
							render = new InvRenderClass(true);
						}
						break;
					default:
						render = new InvRenderClass(true);
						break;
				}
			} else if (skill) {
				render = new SkillRenderClass;
			}
			break;
	}












	FAIL_IF(!render) return(0);

	if (!render->Unknown_08(iface->Get_Data(), robj)) {
		render->Release_Ref();
		render = NULL;
	}
	return(render);
}
// 0x6A93E0
void Unknown_6A93E0(const char *value, std::string &string)
{
	if (value && *value) {
		string += std::string("<") + value + "/>";
	} else {
		string += "</>";
	}
}

// 0x6A9660
const char *Unknown_6A9660(const char *serial_string, std::string &string)
{
	string.erase();
	FAIL_IF(!serial_string) return(0);
	FAIL_IF(*serial_string != '<') return(0);

	serial_string++;
	const char *end = strstr(serial_string, "/>");
	string = serial_string;
	if (!end) return(0);
	string.resize(end - serial_string);
	return(end + 2);
}
/*
**	Reconstruction notes.
**
**	Object 0x6A7B60-0x6A9B50: <string>'s initialisers 0x6A7B20 and 0x6A9B30 bound it; asserts on lines 89-531 push the
**	__FILE__ 0xB91684. 0x6A7BB0 (+0x34), 0x6A7BD0 (the deleting destructor) follow the constructor, which references the
**	table; 0x6A9920 is std::string::resize, called from the inlined 0x6A9660 in 0x6A8430.
**	The client's destructor calls 0x6A8A40 out of line and inlines 0x6A8C40. Six dead reads at the top of 0x6A8A40 still
**	inline it (433 bytes against the client's 425), seven do not; accessors instead of the reads kept it inlined or moved 0x6A7C00's
**	registers. The client's form is not known; the reads emit nothing.
**	0x6A7C00 is a tie: the two reloads at 0x6A7CD3 (edi from robj's slot, then ebx from loaded's) swap with no change to
**	the function. What is included decides it: verify's include list (the DirectX SDK's folder) gives ebx first, appfile.py's
**	automatic list edi first; assetmgr.h first gives the client's order under verify's. Also right there: adding ww3d.h, mesh.h, dx8wrapper.h, d3d8.h, gadget.h,
**	camera.h, light.h or list.h after vector3.h; not 20 other headers tried. So a changed include in this file can bring it back.
**	0x6A7420-0x6A7B20 before it is a separate object.
*/
