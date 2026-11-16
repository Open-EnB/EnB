// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: unknown (the object 0x466920-0x466BC0 between heightfield.cpp and landdmap.cpp; the client
**	holds no string of it). Named after its constructor (D10).
**
**	A Line3DClass with a sort level: table 0xAE8754, 0x10C bytes.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __UNKNOWN_466920_H  // NAME 1 [unknown_466920.h]: neutral (D10): the client's file is not known
#define __UNKNOWN_466920_H

#include "always.h"
#include "..\..\ww3d2\line3d.h"

class Unknown_466920Class : public Line3DClass  // NAME 1: neutral (D10): its file is not known
{
public:
	Unknown_466920Class(const Vector3 &start, const Vector3 &end, float width, float r, float g, float b, float opacity);	// 0x466920
	Unknown_466920Class(const Unknown_466920Class &src);	// 0x4669F0
	Unknown_466920Class &operator=(const Unknown_466920Class &that);	// 0x466A40
	virtual RenderObjClass *Clone() const;				// 0x466A80
	virtual void Render(RenderInfoClass &rinfo);		// 0x466B20
	virtual int Get_Sort_Level() const { return SortLevel; }	// 0x466BA0
	virtual void Set_Sort_Level(int level) { SortLevel = level; }	// 0x466BC0
	bool Unknown_In_Scene() { return(Scene != NULL); }	// (inline: nav_list_line_draw.cpp's 0x7B9CD0 reads +0x7C)  // NAME 1: what it returns

	int SortLevel;										// +0x108 (0x466B20: the static sort list's level)  // NAME 2: Get_Sort_Level's
};

#endif //__UNKNOWN_466920_H
