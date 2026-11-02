// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\ (uiview.h: PROVISIONAL, after uiview.cpp's __FILE__)
**
**	What uiview.cpp's UIViewClass (sclient.h) makes and calls, as far as uiview.cpp and mainview.cpp use it.
*/



#if defined(_MSC_VER)
#pragma once
#endif

// NAME 3 [uiview.h]: the header of uiview.cpp (its __FILE__ 'e:\build\beta\G\app\Client\uiview.cpp')
#ifndef __UIVIEW_H
#define __UIVIEW_H

#include "..\..\ww3d2\scene.h"
#include "ref_ptr.h"

class CameraClass;
class GadgetClass;
class UIViewClass;

// the views' 2-D scene (UIViewClass +0x1C; 0xA0 bytes, constructor 0x6DA4D0)
#include "interface_scene.h"

// gadget_list_manager.cpp's manager (UIViewClass +0x2C; 0x4C bytes, constructor 0x6611B0, destructor 0x661430)
#include "gadget_list_manager.h"

// UIViewClass +0x30: the tooltip window (tooltip.cpp; 0x70 bytes, constructor 0x6AB320, destructor 0x6AB4D0)
#include "tooltip.h"

#endif //__UIVIEW_H
