// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: unknown (PROVISIONAL, D137: a neutral name; notes in unknown_5dd960.cpp)
**
**	A display over Unknown5BE4F0Class (ui_mfd.h) with three buttons 'face_button', 'target_button' and
**	'orient_button' (ids 6 / 7 / 8), each of which only says it is not implemented. Tables 0xAF5E60 / 0xAF5E54 /
**	0xAF5E48 / 0xAF5E34; nothing in the client makes one.
*/
#ifndef UNKNOWN_5DD960_H  // NAME 1 [unknown_5dd960.h]: neutral (D137)
#define UNKNOWN_5DD960_H

#include "ui_mfd.h"

class UIViewClass;
class UIMeshClass;
class LayerClass;
class GadgetClass;

class Unknown5DD960Class : public Unknown5BE4F0Class	// NAME 1: neutral (D10)
{
public:
	Unknown5DD960Class(UIViewClass *view, UIMeshClass *mesh, const LayerClass &layer);	// 0x5DD960  // NAME 1 [view, mesh, layer]: what it does with them
	virtual ~Unknown5DD960Class();						// 0x5DDAE0 (deleting 0x5DDAB0)
	virtual void Unknown_58();							// 0x5DDB20
	virtual void Unknown_5C();							// 0x5DDB40
	virtual void Unknown_60();							// 0x5DDC20
	virtual bool Unknown_68(int key);					// 0x5DDB60 (ids 6 / 7 / 8: facing, tracking, orient)  // NAME 2 [key]: InterfaceClass's
	virtual int Unknown_8C() { return(10); }			// +0x8C 0x5DDA90

	GadgetClass *Buttons[4];							// +0x188 (the constructor's [1..3]: face, target, orient)  // NAME 2: what it holds
};

#endif
