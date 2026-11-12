// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\client\nav_list_line_draw.h (name assumed from nav_list_line_draw.cpp,
**	whose __FILE__ is 'e:\build\beta\G\app\client\nav_list_line_draw.cpp' 0xB9EF98; the client holds no string
**	of this header)
**
**	A navigation path drawn as lines on the star map: the list a NavListBuilderClass built, one Line3D per leg,
**	drawn one more leg at a time (0x7B9DC0's 'lines_drawn < CurLine'). 0x40 bytes, no table; StarmapPipClass's
**	constructor 0x7BF930 news two (TargetPath +0x8C, WarpPath +0x90).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __NAV_LIST_LINE_DRAW_H  // NAME 3 [nav_list_line_draw.h]: header name assumed from the client's nav_list_line_draw.cpp
#define __NAV_LIST_LINE_DRAW_H

#include <vector>
#include "always.h"
#include "ref_ptr.h"
#include "vector3.h"

class SClient;
class SceneClass;
class Unknown_466920Class;
class ClientObjectClass;
class NavigationListClass;
class NavListBuilderClass;

class NavListLineDrawClass  // NAME 3: class-from-file (D24) for app\client\nav_list_line_draw.cpp
{
public:
	NavListLineDrawClass(SClient *client, float width, int sort, const Vector3 &color, float opacity);	// 0x7B97F0  // NAME 1 [width, sort, color, opacity]: what 0x7B99C0 hands the lines (0x466920's arguments, +0x108)
	~NavListLineDrawClass();							// 0x7B98E0
	void Unknown_7B99C0(ClientObjectClass *avatar, NavListBuilderClass *path, const RefCountPtr<SceneClass> &scene, bool arg);	// 0x7B99C0 (the path's lines made anew; StarmapPipClass 0x7C0910)  // NAME 1 [avatar, path, scene, arg]: what 0x7C0910 passes
	void Unknown_7B9CD0();								// 0x7B9CD0 (the lines out of the scene and released; StarmapPipClass 0x7C2D30)
	void Unknown_7B9DC0(int index);						// 0x7B9DC0 (FAIL_IF '!Client' l.137, '!sdata' l.149; G_ASSERTs l.172-210; StarmapPipClass 0x7C0910)  // NAME 1 [index]: the list's Index that 0x7C0910 passes

	SClient *Client;									// +0x00  // NAME 3: 0x7B9DC0's FAIL_IF '!Client'
	ClientObjectClass *Unknown04;						// +0x04 (0x7B99C0's avatar; 0x7B9CD0 clears it; 0x7B9DC0 draws only with it set)
	NavigationListClass *List;							// +0x08 (the constructor news it; 0x7B99C0 copies the path into it)  // NAME 2: what it holds
	std::vector<RefCountPtr<Unknown_466920Class> > Lines;	// +0x0C (the lines: 0x7B9CD0 reads their +0x7C, the scene, directly)  // NAME 3: 0x7B9DC0's G_ASSERT 'line == Lines.end()'
	int CurLine;										// +0x1C (0x7B99C0: the list's size, or 0)  // NAME 3: 0x7B9DC0's G_ASSERT 'lines_drawn < CurLine'
	float Unknown20;									// +0x20 (0x7B9DC0 adds the frame time to it)
	RefCountPtr<SceneClass> Scene;						// +0x24 (0x7B99C0's scene)  // NAME 2: what it holds
	Vector3 Color;										// +0x28  // NAME 2: what the constructor's callers pass (green, red)
	float Opacity;										// +0x34 (0.5)  // NAME 1: 0x466920's last argument
	float Width;										// +0x38 (0.75)  // NAME 1: 0x466920's first argument
	int Sort;											// +0x3C (1, 2: the line's +0x108)  // NAME 1: what the line keeps
};

#endif // __NAV_LIST_LINE_DRAW_H
