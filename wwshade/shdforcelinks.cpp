/*
**	Command & Conquer Generals Zero Hour(tm)
**	Copyright 2025 Electronic Arts Inc.
**	Modified for the Earth & Beyond client reconstruction: each change is recorded at the end of this file.
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
 *                 Project Name : wwshade                                                        *
 *                                                                                             *
 *                     $Archive:: \code\wwshade\shdforcelinks.cpp											 $*
 *                                                                                             *
 *                   Org Author:: Patrick Smith                                                *
 *                                                                                             *
 *                       Author:: Kenny Mitchell                                                *
 *                                                                                             *
 *                     $Modtime:: 6/27/2002 4:28p                                            $*
 *                                                                                             *
 *                    $Revision:: 3                                                           $*
 *                                                                                             *
 *---------------------------------------------------------------------------------------------*
 * Functions:                                                                                  *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#include "shdforcelinks.h"
#include "wwhack.h"


/////////////////////////////////////////////////////////////////////////////
//
//	SHD_Force_Links
//
/////////////////////////////////////////////////////////////////////////////
void SHD_Force_Links ()
{
	//
	//	Force-link those modules that would be "linked-out" of the static lib
	//	because they are not directly referenced.
	//
	FORCE_LINK (SimpleShader);
	FORCE_LINK (GlossMaskShader);
	FORCE_LINK (BumpSpecShader);
	FORCE_LINK (BumpDiffShader);
	FORCE_LINK (CubeMapShader);

	return ;
}

// Earth & Beyond client (reconstruction): changes to Zero Hour's shdforcelinks.cpp (frozen 2026-11-07 from importer.py)
// EnB: imported from Zero Hour (not by byte-match evidence; EXTRA_FILES): SHD_Force_Links (client 0xA08C70) for
//       shdrenderer.cpp; Z only
// EnB: SHD_Force_Links (line 59 at the freeze): no LegacyW3D shader in the client (shdlegacyw3d.cpp is NOT_LINKED):
//       SHD_Force_Links 0xA08C70 calls four force-link functions and tail-jumps to a fifth (0xA0D240, 0xA0C630,
//       0xA0BC80, 0xA0B300, 0xA0A7F0)
