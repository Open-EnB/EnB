// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\dynamic_window_toggle.h (name assumed from
**	dynamic_window_toggle.cpp, whose __FILE__ is 0xB76E1C; the client holds no string of this header)
**
**	A dynamic window's toggle button: one model per state, the next one on each click of its gadget
**	(table 0xAEDAE4, 0x74 bytes; constructor 0x51DEB0, destructor 0x51E0F0, deleting 0x51E0C0; made by the
**	two Create 0x51D790 / 0x51DB50). The item examination window's option rows show one.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef DYNAMIC_WINDOW_TOGGLE_H  // NAME 3 [dynamic_window_toggle.h]: header name assumed from the client's dynamic_window_toggle.cpp
#define DYNAMIC_WINDOW_TOGGLE_H

#include "dynamic_window_render_object.h"
#include "gadget_collection_interface.h"
#include <string>
#include <vector>

class TextureClass;
class GadgetClass;

// What the toggle emits when its state changes (on Unknown_4F7EF0's signal map, the toggle as sender)
class ToggleButtonStateEvent  // NAME 5: the client's RTTI .?AVToggleButtonStateEvent@@
{
public:
	ToggleButtonStateEvent(int state) : State(state) {}
	int State;																					// +0x00  // NAME 1: what it holds
};

class ToggleButtonDynamicWindow : public DynamicWindowRenderObjectClass  // NAME 5: the client's RTTI .?AVToggleButtonDynamicWindow@@
{
public:
	static RefCountPtr<ToggleButtonDynamicWindow> Create(const std::vector<std::string> &texture_names, const Vector2 &size, GadgetCollectionInterface *collection, unsigned int flags);	// 0x51D790 (FAIL_IF 'size.X <= 0 || size.Y <= 0', 'texture_name.empty()', '!render_object', 'render_objects.empty()')  // NAME 1 [Create, collection, flags]: as DynamicWindowTextClass's; NAME 2 [texture_names, size]: the FAIL_IFs'
	static RefCountPtr<ToggleButtonDynamicWindow> Create(const std::vector< RefCountPtr<TextureClass> > &textures, const Vector2 &size, GadgetCollectionInterface *collection, unsigned int flags);	// 0x51DB50 (FAIL_IF '!texture')  // NAME 1 [textures]: the FAIL_IF's 'texture'

	ToggleButtonDynamicWindow(const std::vector< RefCountPtr<RenderObjClass> > &render_objects, const Vector2 &size, GadgetCollectionInterface *collection, unsigned int flags);	// 0x51DEB0 (FAIL_IF 'StateRenderObjects.empty()')  // NAME 2 [render_objects]: Create's FAIL_IF
	virtual ~ToggleButtonDynamicWindow();														// +0x04 0x51E0F0 (deleting 0x51E0C0)

	void Set_State(int state);																	// 0x51E250 (FAIL_IF 'state >= StateRenderObjects.size()')  // NAME 1 [Set_State]; NAME 2 [state]: the FAIL_IF's
	int Get_State() const { return(State); }													// (inline)  // NAME 1 [Get_State]
	GadgetClass *Get_Gadget() const { return(Gadget); }											// (inline: the item examination rows)  // NAME 1 [Get_Gadget]

protected:
	static RefCountPtr<RenderObjClass> Create_Render_Object(const std::string &texture_name, const Vector2 &size);	// 0x51E2C0 (the texture through the asset manager)  // NAME 1 [Create_Render_Object]
	static RefCountPtr<RenderObjClass> Create_Render_Object(const RefCountPtr<TextureClass> &texture, const Vector2 &size);	// 0x51E3D0 (FAIL_IF '!texture': a two-triangle quad of size)  // NAME 1 [Create_Render_Object]
	void On_Gadget_Event(GadgetEvent *gadget_event, GadgetCollectionInterface *collection);	// 0x51E730 (FAIL_IF '!gadget_event': the next state, ToggleButtonStateEvent)  // NAME 1 [On_Gadget_Event, collection]; NAME 2 [gadget_event]: the FAIL_IF's

	Vector2 ButtonSize;																			// +0x4C  // NAME 1: what it holds
	GadgetCollectionInterface *Collection;														// +0x54  // NAME 1: what it holds
	int State;																					// +0x58 (Set_State's)  // NAME 2: the FAIL_IF's 'state'
	std::vector< RefCountPtr<RenderObjClass> > StateRenderObjects;								// +0x5C (FAIL_IF 'StateRenderObjects.empty()')  // NAME 3: the FAIL_IF's
	GadgetClass *Gadget;																		// +0x6C (gd_dynamicelement.h's, over the toggle)  // NAME 1: what it holds
	int GadgetId;																				// +0x70 (the collection's slot +0x10; the events' id)  // NAME 1: what it holds
};

#endif
