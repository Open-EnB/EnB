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
 *                 Project Name : WWSHADE                                                         *
 *                                                                                             *
 *                     $Archive:: wwshade/shdinterface.h                           $*
 *                                                                                             *
 *                  $Org Author:: Jani_p
 *																																	
 *                      $Author:: Kenny_m
 *																																	
 *							  $Modtime:: 6/05/02 3:12p                                               $*
 *                                                                                             *
 *                    $Revision:: 2                                                          $*
 *                                                                                             *
 *   6/05/02	 KJM : Added texture info functions
 *---------------------------------------------------------------------------------------------*
 * Functions:                                                                                  *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#ifndef SHDINTERFACE_H
#define SHDINTERFACE_H

#include "always.h"
#include "refcount.h"
#include "dx8wrapper.h"

class ShdDefClass;
class ShdMeshClass;
class RenderInfoClass;
class TextureClass;

/*
** Maximum passes allowed for any shader implementation!
*/
const int SHD_MAX_PASSES = 4;

/**
** Utility structure for passing in streams to the shader. Those streams that are not available will be NULL,
** and the shader should ASSERT that it is passed everything it needs in its Copy_Vertex_Stream() function.
** The caller of the Copy_Vertex_Stream() should use query functions (such as Requires_Normals()) in the definition
** to determine which streams the shader needs.
*/
struct VertexStreamStruct
{
	VertexStreamStruct()
	:	Locations(0),
		Normals(0),
		DiffuseInt(0),
		DiffuseFloat(0)
	{
		for (int i=0;i<MAX_TEXTURE_STAGES;++i) 
		{
			UV[i]=0;
		}
	}
		
	const Vector3* Locations;
	const Vector3* Normals;
	const Vector2* UV[MAX_TEXTURE_STAGES];
	const unsigned* DiffuseInt;
	const Vector4* DiffuseFloat;
	const Vector3*	S;
	const Vector3* T;
	const Vector3* SxT;
};


/**
** ShdInterfaceClass - This class is the virtual interface for all shaders.  A derived shader's job is to 
** set up the D3D render states for a particular rendering operation.  Instances of shaders are 
** are created by an associated ShdDefClass.
*/
class ShdInterfaceClass : public RefCountClass
{
public:
	// E&B: no class id; the shader's type is its definition's (ShdInterfaceClass ctor 0xA08E70
	// takes only def, ret 4; Shd6CubeMapClass's Material at +0xC follows Definition at +8)
	ShdInterfaceClass(const ShdDefClass * def);
	virtual ~ShdInterfaceClass(void);

	const ShdDefClass *		Peek_Definition(void);

	// E&B: no Greater_Than/Similar_Enough (Shd6SimpleClass vtable 0xB16F0C: Get_Pass_Count in slot 2)

	virtual int					Get_Pass_Count(void) = 0;
	virtual bool				Pass_Selection(ShdMeshClass*, RenderInfoClass*,int) { return true; }

	virtual void				Apply_Shared(int cur_pass, RenderInfoClass& rinfo) = 0;
	virtual void				Apply_Instance(int cur_pass, RenderInfoClass& rinfo) = 0;

	// The shader needs to tell how many vertex streams it needs and what are the sizes of vertices
	// in each stream.
	virtual unsigned			Get_Vertex_Stream_Count() const = 0;
	virtual unsigned			Get_Vertex_Size(unsigned stream) const = 0;

	virtual bool				Use_HW_Vertex_Processing() const = 0;

	virtual int					Get_Texture_Count() const = 0;
	virtual TextureClass*	Peek_Texture(int idx) const = 0;

	// The shader needs to construct each vertex stream as requested. The data comes in a standard
	// VertexStreamStruct, and the shader can format it as it wishes (nothing else will be using
	// the buffer than the shader itself.
	virtual void				Copy_Vertex_Stream(unsigned stream, void* dest_buffer, const VertexStreamStruct& vss, unsigned vertex_count) = 0;

	// E&B: no Is_Opaque (Shd6SimpleClass's table ends with Copy_Vertex_Stream, slot 11)

protected:

	const ShdDefClass *		Definition;

};


#endif //SHDINTERFACE_H