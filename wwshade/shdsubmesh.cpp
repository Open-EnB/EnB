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
 *                 Project Name : WWShade                                                      *
 *                                                                                             *
 *                     $Archive:: wwshade/shdsubmesh.cpp													$*
 *                                                                                             *
 *                   Org Author:: Jani P                                               *
 *                                                                                             *
 *                       Author : Kenny Mitchell                                               * 
 *                                                                                             * 
 *                     $Modtime:: 07/12/02 10:31a                                              $*
 *                                                                                             *
 *                    $Revision:: 1                                                           $*
 *                                                                                             *
 * 07/12/02 KM Removed legacy mesh conversion                                                  *
 *---------------------------------------------------------------------------------------------*
 *---------------------------------------------------------------------------------------------*/

#include "shddefmanager.h"
#include "shdsubmesh.h"
#include "shdrenderer.h"
#include "shddef.h"
#include "shdclassids.h"

#include "chunkio.h"
#include "texture.h"
#include "w3d_file.h"
#include "ww3d.h"
#include "dx8fvf.h"
#include "dx8wrapper.h"
#include "meshmdl.h"
#include "texture.h"
#include "aabtree.h"
#include "sharebuf.h"

ShdSubMeshClass::ShdSubMeshClass(void)
	:
	Shader(NULL),
	S(NULL),
	T(NULL),
	SxT(NULL),
	Diffuse(NULL)
{
	for (int i=0;i<MAX_TEXTURE_STAGES;++i) 
	{
		UV[i]=NULL;
	}
}

ShdSubMeshClass::ShdSubMeshClass(const ShdSubMeshClass& that)
	:
	Shader(NULL),
	S(NULL),
	T(NULL),
	SxT(NULL),
	Diffuse(NULL)
{
	for (int i=0;i<MAX_TEXTURE_STAGES;++i) 
	{
		UV[i]=NULL;
	}
	*this = that;
}

ShdSubMeshClass & ShdSubMeshClass::operator = (const ShdSubMeshClass & that)
{
	if (this != &that) 
	{
		REF_PTR_SET(Shader,that.Shader);
		for (int i=0;i<MAX_TEXTURE_STAGES;++i) 
		{
			REF_PTR_SET(UV[i],that.UV[i]);
		}

		REF_PTR_SET(S,that.S);
		REF_PTR_SET(T,that.T);
		REF_PTR_SET(SxT,that.SxT);

		REF_PTR_SET(Diffuse,that.Diffuse);
		MeshGeometryClass::operator =(that);
	}
	return * this;
}


ShdSubMeshClass::~ShdSubMeshClass(void)
{
	REF_PTR_RELEASE(Shader);
	for (int i=0;i<MAX_TEXTURE_STAGES;++i) 
	{
		REF_PTR_RELEASE(UV[i]);
	}

	REF_PTR_RELEASE(S);
	REF_PTR_RELEASE(T);
	REF_PTR_RELEASE(SxT);

	REF_PTR_RELEASE(Diffuse);
}

WW3DErrorType ShdSubMeshClass::Load_W3D(ChunkLoadClass& cload)
{
	// read header
	W3dShdSubMeshHeaderStruct hdr;
	cload.Open_Chunk();
	if 
	(
		cload.Read
		(
			&hdr,
			sizeof(W3dShdSubMeshHeaderStruct)
		)!=sizeof(W3dShdSubMeshHeaderStruct)
	)
	{
		return WW3D_ERROR_LOAD_FAILED;
	}
	cload.Close_Chunk();

	// process header
	PolyCount=hdr.NumTris;
	VertexCount=hdr.NumVertices;

	if ((PolyCount!=0) && (VertexCount!=0)) 
	{
		Poly				= NEW_REF(ShareBufferClass<TriIndex>,(PolyCount,"ShdSubMesh::Poly"));
		PolySurfaceType		= NEW_REF(ShareBufferClass<uint8>,(PolyCount,"ShdSubMesh::PolySurfaceType"));
		Vertex				= NEW_REF(ShareBufferClass<Vector3>,(VertexCount,"ShdSubMesh::Vertex"));

		Poly->Clear();
		PolySurfaceType->Clear();
		Vertex->Clear();

#if (!OPTIMIZE_VNORM_RAM)
		VertexNorm		=NEW_REF(ShareBufferClass<Vector3>,(VertexCount,"ShdSubMesh::VertexNorm"));
		VertexNorm->Clear();
#endif
	}

	BoundBoxMin.Set(hdr.BoxMin.X,hdr.BoxMin.Y,hdr.BoxMin.Z);
	BoundBoxMax.Set(hdr.BoxMax.X,hdr.BoxMax.Y,hdr.BoxMax.Z);
	BoundSphereCenter.Set(hdr.SphCenter.X,hdr.SphCenter.Y,hdr.SphCenter.Z);
	BoundSphereRadius=hdr.SphRadius;

	read_chunks(cload);

	return WW3D_ERROR_OK;
}


/***********************************************************************************************
 * ShdSubMeshClass::read_chunks -- read w3d chunks                                           *
 *                                                                                             *
 *                                                                                             *
 * INPUT:                                                                                      *
 *                                                                                             *
 * OUTPUT:                                                                                     *
 *                                                                                             *
 * WARNINGS:                                                                                   *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   5/21/2002  kjm : Created.                                                                 *
 *=============================================================================================*/
WW3DErrorType ShdSubMeshClass::read_chunks(ChunkLoadClass& cload) 
{
	// If there are no more chunks within the mesh chunk,
	// we are done.

	while (cload.Open_Chunk()) 
	{
		// Process the chunk
		WW3DErrorType error=WW3D_ERROR_OK;

		switch (cload.Cur_Chunk_ID()) 
		{
		case W3D_CHUNK_SHDSUBMESH_VERTICES					: error=read_vertices(cload); break;
		case W3D_CHUNK_SHDSUBMESH_VERTEX_NORMALS			: error=read_vertex_normals(cload); break;
		case W3D_CHUNK_SHDSUBMESH_UV0							: error=read_uv0(cload); break;
		case W3D_CHUNK_SHDSUBMESH_UV1							: error=read_uv1(cload); break;

		case W3D_CHUNK_SHDSUBMESH_TANGENT_BASIS_S			: error=read_tangent_basis_s(cload); break;
		case W3D_CHUNK_SHDSUBMESH_TANGENT_BASIS_T			: error=read_tangent_basis_t(cload); break;
		case W3D_CHUNK_SHDSUBMESH_TANGENT_BASIS_SxT		: error=read_tangent_basis_sxt(cload); break;

		case W3D_CHUNK_SHDSUBMESH_TRIANGLES					: error=read_triangles(cload); break;
		case W3D_CHUNK_SHDSUBMESH_VERTEX_SHADE_INDICES	: error=read_vertex_shade_indices(cload); break;
		case W3D_CHUNK_SHDSUBMESH_SHADER						: error=read_shader(cload); break;
		default: break;
		}	
		
		cload.Close_Chunk();

		if (error!=WW3D_ERROR_OK) 
		{
			return error;
		}
	}

	return WW3D_ERROR_OK;
}


/***********************************************************************************************
 * ShdSubMeshClass::read_vertices -- read the vertex chunk from a W3D file                   *
 *                                                                                             *
 * INPUT:                                                                                      *
 *                                                                                             *
 * OUTPUT:                                                                                     *
 *                                                                                             *
 * WARNINGS:                                                                                   *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   5/21/2002  kjm : Created.                                                                 *
 *=============================================================================================*/
WW3DErrorType ShdSubMeshClass::read_vertices(ChunkLoadClass& cload)
{
	W3dVectorStruct vert;
	Vector3* loc=Vertex->Get_Array();
	assert(loc);

	for (int i=0; i<Get_Vertex_Count(); i++) 
	{
		if (cload.Read(&vert,sizeof(W3dVectorStruct)) != sizeof(W3dVectorStruct)) 
		{
			return WW3D_ERROR_LOAD_FAILED;
		}
		
		loc[i].X=vert.X;
		loc[i].Y=vert.Y;
		loc[i].Z=vert.Z;
	}

	return WW3D_ERROR_OK;	
}


/***********************************************************************************************
 * ShdSubMeshClass::read_vertex_normals -- read the vertex normals chunk from a w3d file     *
 *                                                                                             *
 * INPUT:                                                                                      *
 *                                                                                             *
 * OUTPUT:                                                                                     *
 *                                                                                             *
 * WARNINGS:                                                                                   *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   5/21/2002  kjm : Created.                                                                 *
 *=============================================================================================*/
WW3DErrorType ShdSubMeshClass::read_vertex_normals(ChunkLoadClass& cload)
{
	W3dVectorStruct norm;
	Vector3 * mdlnorms=get_vert_normals();
	WWASSERT(mdlnorms);

	for (int i=0; i<VertexCount; i++) 
	{
		if (cload.Read(&norm,sizeof(W3dVectorStruct)) != sizeof(W3dVectorStruct)) 
		{
			return WW3D_ERROR_LOAD_FAILED;
		}

		mdlnorms[i].Set(norm.X,norm.Y,norm.Z);
	}

	return WW3D_ERROR_OK;	
}

/***********************************************************************************************
 * ShdSubMeshClass::read_uv0 -- read the texture coords chunk from a w3d file     *
 *                                                                                             *
 * INPUT:                                                                                      *
 *                                                                                             *
 * OUTPUT:                                                                                     *
 *                                                                                             *
 * WARNINGS:                                                                                   *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   5/26/2002  kjm : Created.                                                                 *
 *=============================================================================================*/
WW3DErrorType ShdSubMeshClass::read_uv0(ChunkLoadClass& cload)
{
	UV[0]=NEW_REF(ShareBufferClass<Vector2>,(VertexCount,"ShdSubMeshClass::UV"));
	UV[0]->Clear();

	Vector2* uv=UV[0]->Get_Array();

	for (int i=0; i<VertexCount; i++) 
	{
		if 
		(
			cload.Read
			(
				uv++,
				sizeof(Vector2)
			)!=sizeof(Vector2)
		) 
		{
			return WW3D_ERROR_LOAD_FAILED;
		}
	}

	return WW3D_ERROR_OK;	

}

/***********************************************************************************************
 * ShdSubMeshClass::read_uv1 -- read the texture coords chunk from a w3d file     *
 *                                                                                             *
 * INPUT:                                                                                      *
 *                                                                                             *
 * OUTPUT:                                                                                     *
 *                                                                                             *
 * WARNINGS:                                                                                   *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   5/26/2002  kjm : Created.                                                                 *
 *=============================================================================================*/
WW3DErrorType ShdSubMeshClass::read_uv1(ChunkLoadClass& cload)
{
	UV[1]=NEW_REF(ShareBufferClass<Vector2>,(VertexCount,"ShdSubMeshClass::UV"));
	UV[1]->Clear();

	Vector2* uv=UV[1]->Get_Array();

	for (int i=0; i<VertexCount; i++) 
	{
		if 
		(
			cload.Read
			(
				uv++,
				sizeof(Vector2)
			)!=sizeof(Vector2)
		) 
		{
			return WW3D_ERROR_LOAD_FAILED;
		}
	}

	return WW3D_ERROR_OK;	

}

/***********************************************************************************************
 * ShdSubMeshClass::read_tangent_basis_s -- read the tangent basis chunk from a w3d file     *
 *                                                                                             *
 * INPUT:                                                                                      *
 *                                                                                             *
 * OUTPUT:                                                                                     *
 *                                                                                             *
 * WARNINGS:                                                                                   *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   5/26/2002  kjm : Created.                                                                 *
 *=============================================================================================*/
WW3DErrorType ShdSubMeshClass::read_tangent_basis_s(ChunkLoadClass& cload)
{
	S=NEW_REF(ShareBufferClass<Vector3>,(VertexCount,"ShdSubMeshClass::S"));
	S->Clear();

	Vector3* t=S->Get_Array();

	for (int i=0; i<VertexCount; i++) 
	{
		if 
		(
			cload.Read
			(
				t++,
				sizeof(Vector3)
			)!=sizeof(Vector3)
		) 
		{
			return WW3D_ERROR_LOAD_FAILED;
		}
	}

	return WW3D_ERROR_OK;	

}

/***********************************************************************************************
 * ShdSubMeshClass::read_tangent_basis_t -- read the tangent basis chunk from a w3d file     *
 *                                                                                             *
 * INPUT:                                                                                      *
 *                                                                                             *
 * OUTPUT:                                                                                     *
 *                                                                                             *
 * WARNINGS:                                                                                   *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   5/26/2002  kjm : Created.                                                                 *
 *=============================================================================================*/
WW3DErrorType ShdSubMeshClass::read_tangent_basis_t(ChunkLoadClass& cload)
{
	T=NEW_REF(ShareBufferClass<Vector3>,(VertexCount,"ShdSubMeshClass::T"));
	T->Clear();

	Vector3* t=T->Get_Array();

	for (int i=0; i<VertexCount; i++) 
	{
		if 
		(
			cload.Read
			(
				t++,
				sizeof(Vector3)
			)!=sizeof(Vector3)
		) 
		{
			return WW3D_ERROR_LOAD_FAILED;
		}
	}

	return WW3D_ERROR_OK;	

}

/***********************************************************************************************
 * ShdSubMeshClass::read_tangent_basis_sxt -- read the tangent basis chunk from a w3d file     *
 *                                                                                             *
 * INPUT:                                                                                      *
 *                                                                                             *
 * OUTPUT:                                                                                     *
 *                                                                                             *
 * WARNINGS:                                                                                   *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   5/26/2002  kjm : Created.                                                                 *
 *=============================================================================================*/
WW3DErrorType ShdSubMeshClass::read_tangent_basis_sxt(ChunkLoadClass& cload)
{
	SxT=NEW_REF(ShareBufferClass<Vector3>,(VertexCount,"ShdSubMeshClass::SxT"));
	SxT->Clear();

	Vector3* t=SxT->Get_Array();

	for (int i=0; i<VertexCount; i++) 
	{
		if 
		(
			cload.Read
			(
				t++,
				sizeof(Vector3)
			)!=sizeof(Vector3)
		) 
		{
			return WW3D_ERROR_LOAD_FAILED;
		}
	}

	return WW3D_ERROR_OK;	

}

/***********************************************************************************************
 * ShdSubMeshClass::read_triangles -- read the triangles chunk from a w3d file               *
 *                                                                                             *
 * INPUT:                                                                                      *
 *                                                                                             *
 * OUTPUT:                                                                                     *
 *                                                                                             *
 * WARNINGS:                                                                                   *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   5/21/2002  kjm : Created.                                                                 *
 *=============================================================================================*/
WW3DErrorType ShdSubMeshClass::read_triangles(ChunkLoadClass & cload)
{
	unsigned short tri[3];

	// cache pointers to various arrays in the surrender mesh
	TriIndex * vi = get_polys();
	Set_Flag(DIRTY_PLANES,false);
//	Vector4 * peq = get_planes();
//	uint8 * surface_types = Get_Poly_Surface_Type_Array();

	// read in each polygon one by one
	for (int i=0; i<Get_Polygon_Count(); i++) 
	{
		if 
		(
			cload.Read
			(
				tri,
				sizeof(unsigned short)*3
			)!=sizeof(unsigned short)*3
		)
		{
			return WW3D_ERROR_LOAD_FAILED;
		}

		// set the vertex indices
		vi[i].I = tri[0];
		vi[i].J = tri[1];
		vi[i].K = tri[2];

		// set the normal
		/*peq[i].X = tri.Normal.X;
		peq[i].Y = tri.Normal.Y;
		peq[i].Z = tri.Normal.Z;
		peq[i].W = -tri.Dist;

		// set the surface type
		WWASSERT(tri.Attributes < 256);
		surface_types[i] = (uint8)(tri.Attributes);*/
	}

	return WW3D_ERROR_OK;	
}

/***********************************************************************************************
 * ShdSubMeshClass::read_vertex_shade_indices -- read the vertex shade indices chunk         *
 *                                                                                             *
 * INPUT:                                                                                      *
 *                                                                                             *
 * OUTPUT:                                                                                     *
 *                                                                                             *
 * WARNINGS:                                                                                   *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   5/21/2002  kjm : Created.                                                                 *
 *=============================================================================================*/
WW3DErrorType ShdSubMeshClass::read_vertex_shade_indices(ChunkLoadClass & cload)
{
	uint32 * shade_index = get_shade_indices(true);
	uint32 si;

	for (int i=0; i<Get_Vertex_Count(); i++) 
	{
		if (cload.Read(&si,sizeof(uint32)) != sizeof(uint32)) 
		{
			return WW3D_ERROR_LOAD_FAILED;
		}
		shade_index[i] = si;
	}
	return WW3D_ERROR_OK;
}


/***********************************************************************************************
 * ShdSubMeshClass::read_shader -- read the shader chunk													 *
 *                                                                                             *
 * INPUT:                                                                                      *
 *                                                                                             *
 * OUTPUT:                                                                                     *
 *                                                                                             *
 * WARNINGS:                                                                                   *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   5/21/2002  kjm : Created.                                                                 *
 *=============================================================================================*/
WW3DErrorType ShdSubMeshClass::read_shader(ChunkLoadClass& cload)
{
	// load the shader definition
	ShdDefClass* shddef=NULL;

	ShdDefManagerClass::Load_Shader(cload, &shddef);

	// create the shader interface
	Shader=shddef->Create();

	return WW3D_ERROR_OK;
}
