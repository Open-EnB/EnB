// Earth & Beyond client (reconstruction): app\Client\dynamic_window_toggle.cpp (__FILE__ 0xB76E1C). Notes at the end of the file.
#include "dynamic_window_toggle.h"
#include "gd_dynamicelement.h"
#include "unknown_signalbase.h"
#include "..\..\ww3d2\assetmgr.h"
#include "..\..\ww3d2\texture.h"
#include "..\..\ww3d2\textureloader.h"
#include "..\..\ww3d2\dynamesh.h"
#include "..\..\ww3d2\shader.h"
#include "..\shared\g_assert.h"





















RefCountPtr<ToggleButtonDynamicWindow> ToggleButtonDynamicWindow::Create(const std::vector<std::string> &texture_names, const Vector2 &size, GadgetCollectionInterface *collection, unsigned int flags)
{
	FAIL_IF(size.X <= 0 || size.Y <= 0) return NULL;

	std::vector< RefCountPtr<RenderObjClass> > render_objects;
	for (std::vector<std::string>::const_iterator it = texture_names.begin(); it != texture_names.end(); ++it) {
		const std::string &texture_name = *it;

		FAIL_IF(texture_name.empty()) return NULL;

		RefCountPtr<RenderObjClass> render_object = Create_Render_Object(texture_name, size);
		FAIL_IF(!render_object) return NULL;

		render_objects.push_back(render_object);
	}

	FAIL_IF(render_objects.empty()) return NULL;

	return Create_NEW(new ToggleButtonDynamicWindow(render_objects, size, collection, flags));
}

RefCountPtr<ToggleButtonDynamicWindow> ToggleButtonDynamicWindow::Create(const std::vector< RefCountPtr<TextureClass> > &textures, const Vector2 &size, GadgetCollectionInterface *collection, unsigned int flags)
{
	FAIL_IF(size.X <= 0 || size.Y <= 0) return NULL;

	std::vector< RefCountPtr<RenderObjClass> > render_objects;
	for (std::vector< RefCountPtr<TextureClass> >::const_iterator it = textures.begin(); it != textures.end(); ++it) {
		const RefCountPtr<TextureClass> &texture = *it;

		FAIL_IF(!texture) return NULL;

		RefCountPtr<RenderObjClass> render_object = Create_Render_Object(texture, size);
		FAIL_IF(!render_object) return NULL;

		render_objects.push_back(render_object);
	}

	FAIL_IF(render_objects.empty()) return NULL;

	return Create_NEW(new ToggleButtonDynamicWindow(render_objects, size, collection, flags));
}
// NOT EXACT: 0.47, inline budget: the client calls the elements' RefCountPtr copy constructor out of line (0x51E950) in StateRenderObjects' copy, under its own EH state; ours inlines it
ToggleButtonDynamicWindow::ToggleButtonDynamicWindow(const std::vector< RefCountPtr<RenderObjClass> > &render_objects, const Vector2 &size, GadgetCollectionInterface *collection, unsigned int flags) :
	DynamicWindowRenderObjectClass(flags),
	ButtonSize(size),
	Collection(collection),
	State(0),
	StateRenderObjects(render_objects),
	Gadget(NULL),
	GadgetId(-1)
{
	FAIL_IF(StateRenderObjects.empty()) return;

	Set_Render_Object(StateRenderObjects[0]);
	GadgetId = Collection->Unknown84_10();
	Gadget = new DynamicElementGadgetClass(this);
	Collection->Unknown84_08(Gadget, GadgetId, -2);
	Unknown_4F7EF0().connect(Collection, this, &ToggleButtonDynamicWindow::On_Gadget_Event);
}

ToggleButtonDynamicWindow::~ToggleButtonDynamicWindow()
{
	Collection->Unknown84_0C(Gadget);
	GadgetClass::Destroy(Gadget);
	Gadget = NULL;
	Unknown_4F7EF0().disconnect(Collection, this, &ToggleButtonDynamicWindow::On_Gadget_Event, GadgetEvent());
}




void ToggleButtonDynamicWindow::Set_State(int state)
{
	FAIL_IF(state >= StateRenderObjects.size()) return;

	Replace_Render_Object(StateRenderObjects[state]);
	State = state;
}

RefCountPtr<RenderObjClass> ToggleButtonDynamicWindow::Create_Render_Object(const std::string &texture_name, const Vector2 &size)
{
	RefCountPtr<TextureClass> texture = Create_Get(WW3DAssetManager::Get_Instance()->Get_Texture(texture_name.c_str(), MIP_LEVELS_ALL, WW3D_FORMAT_UNKNOWN, true));
	if (!texture) {
		return NULL;
	}
	return Create_Render_Object(texture, size);
}

RefCountPtr<RenderObjClass> ToggleButtonDynamicWindow::Create_Render_Object(const RefCountPtr<TextureClass> &texture, const Vector2 &size)
{


	FAIL_IF(!texture) return NULL;

	if (!texture->Is_Initialized()) {
		TextureLoader::Request_Foreground_Loading(texture.Peek());
	}
	RefCountPtr<DynamicMeshClass> mesh = Create_NEW(new DynamicMeshClass(2, 4));
	mesh->Set_Texture(texture.Peek());
	mesh->Set_Shader(ShaderClass::_PresetAlpha2DShader);

	int width = size.X * 0.5f;
	int height = size.Y * 0.5f;
	mesh->Begin_Tri_Strip();
	mesh->Vertex(-width, -height, 0, 0, 1);
	mesh->Vertex(width, -height, 0, 1, 1);
	mesh->Vertex(-width, height, 0, 0, 0);
	mesh->Vertex(width, height, 0, 1, 0);
	mesh->End_Tri_Strip();
	return mesh;
}






void ToggleButtonDynamicWindow::On_Gadget_Event(GadgetEvent *gadget_event, GadgetCollectionInterface *collection)
{
	FAIL_IF(!gadget_event) return;

	if (gadget_event->Id == GadgetId) {
		if (++State == StateRenderObjects.size()) {
			State = 0;
		}
		Replace_Render_Object(StateRenderObjects[State]);
		Unknown_4F7EF0().emit(this, ToggleButtonStateEvent(State));
	}
}

/*
**	Reconstruction notes (DW1, 2026-11-10)
**
**	Client 0x51D790-0x51EADF (__FILE__ 0xB76E1C; FAIL_IFs on the client's lines 34-151, D25): the two Create
**	0x51D790 (texture names) / 0x51DB50 (textures), the constructor 0x51DEB0, the deleting destructor 0x51E0C0, the
**	destructor 0x51E0F0, the callbacks' base destructor 0x51E220 (inline), Set_State 0x51E250, the two
**	Create_Render_Object 0x51E2C0 / 0x51E3D0 (a two-triangle strip of the texture, size wide and high, centred), the
**	gadget event 0x51E730 and the template helpers after it (RefCountPtr, the vector's, the MemberCallback's slots
**	0x51E980-0x51EAB0). The class name is the client's RTTI (ToggleButtonDynamicWindow, ToggleButtonStateEvent).
*/
