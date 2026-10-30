/*
**	Command & Conquer Generals Zero Hour(tm)
**	Copyright 2025 Electronic Arts Inc.
**	Modified for the Earth & Beyond client reconstruction: tools/vc6/importer.py records each change.
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
 *                     $Archive:: /Commando/Code/ww3d2/shdrenderer.cpp                           $*
 *
 *                    Org Author:: Jani_p
 *                                                                                             *
 *                       $Author:: Kenny_m
 *																																	
 *								$Modtime:: 7/29/02 1:50p                                               $*
 *                                                                                             *
 *                    $Revision:: 3                                                          $*
 *                                                                                             *
 *---------------------------------------------------------------------------------------------*
 * Functions:                                                                                  *
 * 6/02/02 5:59p KM Added render info and light environment support                                              $*
 * 7/29/02 1:50p KM Added VB and IB usage flags for software vertex shaders
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#include "shdrenderer.h"
#include "shdforcelinks.h"
#include "shdmesh.h"
#include "shdsubmesh.h"
#include "shddef.h"
#include "shddefmanager.h"
#include "shdclassids.h"
#include "wwdebug.h"
#include "dx8vertexbuffer.h"
#include "dx8indexbuffer.h"
#include "dx8wrapper.h"
#include "rinfo.h"
#include "camera.h"
#include "texture.h"
#include "ww3dformat.h"
#include "wwprofile.h"
#include "sortingrenderer.h"
#include "meshmatdesc.h"

ShdRendererClass* ShdRendererClass::ShdRenderer=NULL;

ShdRendererClass::ShdRendererClass()
{
}

ShdRendererClass::~ShdRendererClass()
{
}

/////////////////////////////////////////////////////////////////////////////////////
// Initialize the rendering system. This is the place to create different versions
// (such as DX8, DX9, OGL) of the rendering system, if needed.
/////////////////////////////////////////////////////////////////////////////////////
void ShdRendererClass::Init()
{
	SHD_Force_Links();

	WWASSERT(!ShdRenderer);

	ShdRenderer=new ShdDX8RendererClass();

	Init_Shaders();
}

void ShdRendererClass::Init_Shaders()
{
	ShdDefClass* def;
	for (int i=SHDDEF_CLASSID_DUMMY+1; i<SHDDEF_CLASSID_LAST; i++)
	{
		def=ShdDefManagerClass::Create_ShdDefClass_Instance(i);
		def->Init();
		REF_PTR_RELEASE(def);
	}
}

/////////////////////////////////////////////////////////////////////////////////////
// Release the renderer system. At this point, all the meshes must have been
// unregistered.
/////////////////////////////////////////////////////////////////////////////////////
void ShdRendererClass::Shutdown()
{
	Shutdown_Shaders();

	WWASSERT(ShdRenderer);

	delete ShdRenderer;
	ShdRenderer=0;
}

void ShdRendererClass::Shutdown_Shaders()
{
	ShdDefClass* def;
	for (int i=SHDDEF_CLASSID_DUMMY+1; i<SHDDEF_CLASSID_LAST; i++)
	{
		def=ShdDefManagerClass::Create_ShdDefClass_Instance(i);
		def->Shutdown();
		REF_PTR_RELEASE(def);
	}
}

class ShdDX8RendererClass::MeshContainerClass : public RefCountClass
{
public:
	MeshContainerClass();
	virtual ~MeshContainerClass();

	ShdRendererNodeClass* Register_Mesh(ShdMeshClass* mesh,ShdSubMeshClass* sub_mesh);
	void Add_Visible_Node(ShdRendererNodeClass* node,int pass_count);
	void Flush();

private:

	// E&B: one visible list per pass (MeshContainerClass is 0x68 bytes, 0xA08005; the lists
	// at +8, 0x18 bytes each, 0xA080D9)
	ShdRendererNodeList VisibleNodeList[SHD_MAX_PASSES];
};

/**
** ShdDX8RendererNode
*/
class ShdDX8RendererNodeClass : public ShdRendererNodeClass
{
public:
	ShdDX8RendererNodeClass(ShdDX8RendererClass::MeshContainerClass* container, ShdMeshClass* mesh,ShdSubMeshClass* sub_mesh);
	virtual ~ShdDX8RendererNodeClass();

	virtual void Render(RenderInfoClass& rinfo);
	virtual void Flush(int pass);
	virtual void Apply_Shared_Shader_Settings(int pass);

private:

	ShdDX8RendererClass::MeshContainerClass*	Container;
	ShdMeshClass*										Mesh;
	ShdSubMeshClass*									SubMesh;
	VertexBufferClass**							VertexBuffers;
	IndexBufferClass*								IndexBuffer;
	LightEnvironmentClass							LightEnvironment;	// todo KJM optimize for output lights
	RenderInfoClass*									RenderInfo;
};

/////////////////////////////////////////////////////////////////////////////////////
// DirectX8 renderer implementation
/////////////////////////////////////////////////////////////////////////////////////
ShdDX8RendererClass::ShdDX8RendererClass()
{
	MeshCategories=new MeshContainerClass*[SHDDEF_CLASSID_LAST];
	for (int i=0;i<SHDDEF_CLASSID_LAST;++i) 
	{
		MeshCategories[i]=0;
	}
}

ShdDX8RendererClass::~ShdDX8RendererClass()
{
	for (int i=0;i<SHDDEF_CLASSID_LAST;++i) 
	{
		if (MeshCategories[i]) 
		{
			REF_PTR_RELEASE(MeshCategories[i]);
			delete MeshCategories[i];
			MeshCategories[i]=0;
		}
	}
	delete[] MeshCategories;
	MeshCategories=0;
}


/////////////////////////////////////////////////////////////////////////////////////
// Register mesh to the rendering system
/////////////////////////////////////////////////////////////////////////////////////
ShdRendererNodeClass* ShdDX8RendererClass::Register_Mesh
(
	ShdMeshClass* mesh,
	ShdSubMeshClass* sub_mesh
)
{
	ShdInterfaceClass* shd=sub_mesh->Peek_Shader();
	const ShdDefClass* def=shd->Peek_Definition();
	uint32 class_id=def->Get_Class_ID();
	WWASSERT(class_id<SHDDEF_CLASSID_LAST);
	if (!MeshCategories[class_id]) 
	{
		MeshCategories[class_id]=new MeshContainerClass();
	}
	return MeshCategories[class_id]->Register_Mesh(mesh,sub_mesh);
}

/////////////////////////////////////////////////////////////////////////////////////
// Flush the list of visible meshes
/////////////////////////////////////////////////////////////////////////////////////
void ShdDX8RendererClass::Flush()
{
	DX8Wrapper::Invalidate_Cached_Render_States();
	DX8Wrapper::Apply_Default_State();
	DX8Wrapper::Apply_Render_State_Changes();

	SNAPSHOT_SAY(("ShdDX8RendererClass::Flush()\n"));
	for (int i=0;i<SHDDEF_CLASSID_LAST;++i) 
	{
		if (MeshCategories[i]) 
		{
			MeshCategories[i]->Flush();
		}
	}
	DX8Wrapper::Invalidate_Cached_Render_States();
	DX8Wrapper::Apply_Default_State();
}



/////////////////////////////////////////////////////////////////////////////////////
// MeshContiner is used for each shader type. Meshes are assigned to a container
// when they're first rendered and they link themselves to container's visible list
// each frame they're visible.
/////////////////////////////////////////////////////////////////////////////////////
ShdDX8RendererClass::MeshContainerClass::MeshContainerClass()
{
}

ShdDX8RendererClass::MeshContainerClass::~MeshContainerClass()
{
	for (int pass=0; pass<SHD_MAX_PASSES; pass++) 
	{
		while (!VisibleNodeList[pass].Is_Empty()) {
			ShdRendererNodeClass* node=VisibleNodeList[pass].Remove_Head();
			delete node;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////
// RendererNodeClass acts as a link between ShdMeshClass, ShdSubMeshClass and the
// rendering system.
/////////////////////////////////////////////////////////////////////////////////////
ShdRendererNodeClass* ShdDX8RendererClass::MeshContainerClass::Register_Mesh
(
	ShdMeshClass* mesh,
	ShdSubMeshClass* sub_mesh
)
{
	return new ShdDX8RendererNodeClass(this,mesh,sub_mesh);
}

/////////////////////////////////////////////////////////////////////////////////////
// Link the node to the visible list of each of its passes.
/////////////////////////////////////////////////////////////////////////////////////
void ShdDX8RendererClass::MeshContainerClass::Add_Visible_Node(ShdRendererNodeClass* node,int pass_count)
{
	for (int pass=0; pass<pass_count; pass++) 
	{
		VisibleNodeList[pass].Add_Tail(node);
	}
}

/////////////////////////////////////////////////////////////////////////////////////
// Render all visible nodes and clear the visible list.
/////////////////////////////////////////////////////////////////////////////////////
void ShdDX8RendererClass::MeshContainerClass::Flush()
{
	SNAPSHOT_SAY(("ShdDX8RendererClass::MeshContainerClass::Flush()\n"));
	
	for (int pass=0; pass<SHD_MAX_PASSES; pass++) 
	{
		if (!VisibleNodeList[pass].Is_Empty()) {
			// the first node sets the states the pass shares
			ShdRendererNodeClass* node=VisibleNodeList[pass].Remove_Head();
			node->Apply_Shared_Shader_Settings(pass);
			node->Flush(pass);
			while (!VisibleNodeList[pass].Is_Empty()) {
				node=VisibleNodeList[pass].Remove_Head();
				node->Flush(pass);
			}
		}
	}
}


/***********************************************************************************************
 * ShdDX8RendererNodeClass::ShdDX8RendererNodeClass -- 
 * Init the mesh for rendering... this node is used for all subsequent rendering of
 * the particular mesh.
 * INPUT:                                                                                      *
 *                                                                                             *
 * OUTPUT:                                                                                     *
 *                                                                                             *
 * WARNINGS:                                                                                   *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *	  4/20/2002  jp  : Created
 *   5/26/2002  kjm : Updated stream handling for per-pixel shaders                                                                 *
 *   6/02/2002  kjm : Added light environment handling
 *   7/29/2002  kjm : Added VB and IB usage flags for software vertex shaders
 *=============================================================================================*/
ShdDX8RendererNodeClass::ShdDX8RendererNodeClass
(
	ShdDX8RendererClass::MeshContainerClass* container,
	ShdMeshClass* mesh,
	ShdSubMeshClass* sub_mesh
)
:	Container(container),
	Mesh(NULL),
	SubMesh(NULL),
	VertexBuffers(NULL),
	IndexBuffer(NULL)
{
	REF_PTR_SET(Container,container);
	REF_PTR_SET(Mesh,mesh);
	REF_PTR_SET(SubMesh,sub_mesh);

	// Compose the vertex and index buffers
	VertexStreamStruct vss;
	vss.Locations=SubMesh->Get_Vertex_Array();
	vss.Normals=SubMesh->Get_Vertex_Normal_Array();
	for (unsigned stage=0;stage<MAX_TEXTURE_STAGES;++stage) 
	{
		vss.UV[stage]=SubMesh->Get_UV_Array(stage);
	}
	vss.DiffuseInt=SubMesh->Get_Diffuse_Array();
	vss.S=SubMesh->Get_Tangent_Basis_S_Array();
	vss.T=SubMesh->Get_Tangent_Basis_T_Array();
	vss.SxT=SubMesh->Get_Tangent_Basis_SxT_Array();

	unsigned vertex_count=SubMesh->Get_Vertex_Count();
	unsigned stream_count=SubMesh->Peek_Shader()->Get_Vertex_Stream_Count();
	WWASSERT(stream_count>0);

	// create usage depending on associated shader's processing behaviour KM
	DX8VertexBufferClass::UsageType vb_usage=DX8VertexBufferClass::USAGE_DEFAULT;

	if (!SubMesh->Peek_Shader()->Use_HW_Vertex_Processing())
	{
		vb_usage=DX8VertexBufferClass::USAGE_SOFTWAREPROCESSING;
	}

	VertexBuffers=new VertexBufferClass*[stream_count];
	unsigned n;
	for (n=0;n<stream_count;++n) 
	{
		unsigned vertex_size=SubMesh->Peek_Shader()->Get_Vertex_Size(n);

		VertexBuffers[n]=
			new DX8VertexBufferClass
			(
				0,
				vertex_count,
				vb_usage,
				vertex_size
			);

		VertexBufferClass::WriteLockClass vlock(VertexBuffers[n]);
		SubMesh->Peek_Shader()->Copy_Vertex_Stream
		(
			n,
			vlock.Get_Vertex_Array(),
			vss,
			SubMesh->Get_Vertex_Count()
		);
	}

	// create usage depending on associated shader's processing behaviour KM
	DX8IndexBufferClass::UsageType ib_usage=DX8IndexBufferClass::USAGE_DEFAULT;

	if (!SubMesh->Peek_Shader()->Use_HW_Vertex_Processing())
	{
		ib_usage=DX8IndexBufferClass::USAGE_SOFTWAREPROCESSING;
	}

	unsigned index_count=SubMesh->Get_Polygon_Count()*3;
	IndexBuffer=new DX8IndexBufferClass(index_count,ib_usage);
	IndexBufferClass::WriteLockClass ilock(IndexBuffer);
	const TriIndex* indices=SubMesh->Get_Polygon_Array();
	int i;
	for (i=0;i<SubMesh->Get_Polygon_Count();++i) 
	{
		ilock.Get_Index_Array()[i*3]=indices[i][0];
		ilock.Get_Index_Array()[i*3+1]=indices[i][1];
		ilock.Get_Index_Array()[i*3+2]=indices[i][2];
	}
}

ShdDX8RendererNodeClass::~ShdDX8RendererNodeClass()
{
	unsigned stream_count=SubMesh->Peek_Shader()->Get_Vertex_Stream_Count();
	unsigned n;
	for (n=0;n<stream_count;++n) 
	{
		REF_PTR_RELEASE(VertexBuffers[n]);
	}
	delete[] VertexBuffers;
	VertexBuffers=NULL;

	REF_PTR_RELEASE(IndexBuffer);
	REF_PTR_RELEASE(Mesh);
	REF_PTR_RELEASE(SubMesh);
	REF_PTR_RELEASE(Container);
}

/***********************************************************************************************
 * ShdDX8RendererNodeClass::Render -- Render node
 * 
 * INPUT:                                                                                      *
 *                                                                                             *
 * OUTPUT:                                                                                     *
 *                                                                                             *
 * WARNINGS:                                                                                   *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *	  4/20/2002  jp  : Created
 *   6/02/2002  kjm : Added light environment handling
 *=============================================================================================*/
void ShdDX8RendererNodeClass::Render(RenderInfoClass& rinfo)
{
	RenderInfo=&rinfo;
	if (rinfo.light_environment)
	{
		memcpy(&LightEnvironment,rinfo.light_environment,sizeof(LightEnvironmentClass));
	}

	Container->Add_Visible_Node(this,SubMesh->Peek_Shader()->Get_Pass_Count());
}

void ShdDX8RendererNodeClass::Flush(int cur_pass)
{
	WWASSERT(RenderInfo);

	// pass selection for this instance
	if (!SubMesh->Peek_Shader()->Pass_Selection(Mesh,RenderInfo,cur_pass))
	{
		return;
	}

	SNAPSHOT_SAY(("ShdDX8RendererNodeClass::Flush(%d) - %s\n",cur_pass,SubMesh->Get_Name()));

	for (unsigned n=0;n<SubMesh->Peek_Shader()->Get_Vertex_Stream_Count();++n) 
	{
		DX8Wrapper::Set_Vertex_Buffer(VertexBuffers[n],n);
	}

	DX8Wrapper::Set_Index_Buffer(IndexBuffer,0);
	DX8Wrapper::Set_Transform(D3DTS_WORLD,Mesh->Get_Transform());

	// E&B: the environment is only recorded (the flag), and the render info is handed to
	// DX8Wrapper with it (0xA089C2-0xA089D9)
	DX8Wrapper::Set_Light_Environment(&LightEnvironment,false);
	RenderInfo->light_environment=&LightEnvironment;
	DX8Wrapper::Unknown_Set_Render_Info(RenderInfo);
	SubMesh->Peek_Shader()->Apply_Instance(cur_pass, *RenderInfo);

	DX8Wrapper::Draw_Triangles
	(
		0,
		SubMesh->Get_Polygon_Count(),
		0,
		SubMesh->Get_Vertex_Count()
	);
}

void ShdDX8RendererNodeClass::Apply_Shared_Shader_Settings(int pass)
{
	// E&B: always from the default state (0xA08A20)
	DX8Wrapper::Apply_Default_State();
	SubMesh->Peek_Shader()->Apply_Shared(pass, *RenderInfo);
}

