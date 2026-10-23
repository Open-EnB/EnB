/*
**	Command & Conquer Generals Zero Hour(tm)
**	Copyright 2025 Electronic Arts Inc.
**
**	This program is free software: you can redistribute it and/or modify
**	it under the terms of the GNU General Public License as published by
**	the Free Software Foundation, either version 3 of the License, or
**	(at your option) any later version.
**
**	This program is distributed in the hope that it will be useful,
**	but WITHOUT ANY WARRANTY; without even the implied warranty of
**	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**	GNU General Public License for more details.
**
**	You should have received a copy of the GNU General Public License
**	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

 /***********************************************************************************************
 ***              C O N F I D E N T I A L  ---  W E S T W O O D  S T U D I O S               ***
 ***********************************************************************************************
 *                                                                                             *
 *                 Project Name : WW3D                                                         *
 *                                                                                             *
 *                     $Archive:: /Commando/Code/ww3d2/shdrenderer.h                           $*
 *
 *                    Org Author:: Jani_p
 *                                                                                             *
 *                       $Author:: Kenny_m
 *																																	
 *								$Modtime:: 6/06/02 3:04p                                               $*
 *                                                                                             *
 *                    $Revision:: 3                                                          $*
 *                                                                                             *
 *---------------------------------------------------------------------------------------------*
 * Functions:                                                                                  *
 * 6/02/02 5:59p KM Added render info and light environment support                                              $*
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#ifndef SHDRENDERER_H
#define SHDRENDERER_H

#include "multilist.h"
#include "refcount.h"
#include "shdinterface.h"

class ShdSubMeshClass;
class ShdMeshClass;
class RenderInfoClass;

class ShdRendererNodeClass;

typedef MultiListClass<ShdRendererNodeClass> ShdRendererNodeList;
typedef MultiListIterator<ShdRendererNodeClass> ShdRendererNodeListIterator;

/**
** ShdRendererNode
** RendererNodeClass acts as a link between ShdMeshClass, ShdSubMeshClass and the
** rendering system. Rendering API specific implementations are expected.
**
** E&B: an earlier form, without Zero Hour's renderer list containers, reference count or
** sorting (vtable 0xB16AA8: destructor and three slots; the node constructor 0xA082D0 sets
** only the list node at +4).
*/
class ShdRendererNodeClass : public MultiListObjectClass
{
public:
	ShdRendererNodeClass()
	{
	}
	virtual ~ShdRendererNodeClass()
	{
	}

	virtual void Render(RenderInfoClass& rinfo)=0;
	virtual void Flush(int pass)=0;
	virtual void Apply_Shared_Shader_Settings(int pass)=0;
};


/**
** ShdRenderer
*/
class ShdRendererClass
{
	static ShdRendererClass* ShdRenderer;

protected:
	ShdRendererClass();
	virtual ~ShdRendererClass();
public:	

	/////////////////////////////////////////////////////////////////////////////////////
	// Init must be called before any objects can be rendered and Release() must be called
	// at the end to release all references to the rendering API. Working through a static
	// pointer instead of calling static functions will later allow us to switch to another
	// rendering API implementation of the renderer.
	/////////////////////////////////////////////////////////////////////////////////////
	static void Init();
	static void Shutdown();

	static void Init_Shaders();
	static void Shutdown_Shaders();

	static ShdRendererClass* Peek_Instance() { return ShdRenderer; }

	/////////////////////////////////////////////////////////////////////////////////////
	// Register initialized a mesh for rendering.
	/////////////////////////////////////////////////////////////////////////////////////
	virtual ShdRendererNodeClass* Register_Mesh(ShdMeshClass* mesh,ShdSubMeshClass* sub_mesh)=0;

	/////////////////////////////////////////////////////////////////////////////////////
	// Flush all linked meshes to the rendering system
	/////////////////////////////////////////////////////////////////////////////////////
	virtual void Flush()=0;
};


/**
** ShdDX8Renderer
*/
class ShdDX8RendererClass : public ShdRendererClass
{
public:
	ShdDX8RendererClass();
	virtual ~ShdDX8RendererClass();

	/////////////////////////////////////////////////////////////////////////////////////
	// Register initialized a mesh for rendering. Note, these will need to be virtual
	// if new types of renderers are created.
	/////////////////////////////////////////////////////////////////////////////////////
	virtual ShdRendererNodeClass* Register_Mesh(ShdMeshClass* mesh,ShdSubMeshClass* sub_mesh);

	/////////////////////////////////////////////////////////////////////////////////////
	// Flush all linked meshes to the rendering system
	/////////////////////////////////////////////////////////////////////////////////////
	virtual void Flush();

	class MeshContainerClass;
private:
	MeshContainerClass** MeshCategories;

};

#endif //SHDRENDERER_H
