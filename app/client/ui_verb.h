// Earth & Beyond client (reconstruction): app\client\ui_verb.h, the verb window (the client's ui_verb.cpp per its asserts;
// this header's name is provisional, NAME 3). Laid out from the constructor 0x6580B0 and the destructor 0x658280.
#if defined(_MSC_VER)
#pragma once
#endif

#ifndef UI_VERB_H  // NAME 3: the header of ui_verb.cpp (the client's __FILE__ 0xB8C43C)
#define UI_VERB_H

#include <set>
#include <vector>
#include "sclient.h"
#include "unknown_675120.h"
#include "co.h"
#include "..\common\unknown_verb.h"	// VerbEnum

class VerbManagerClass;
class UIMeshClass;
class ClientObjectClass;

// A verb's button (0x6C bytes, table 0xAF954C; constructor 0x659980): enabled while the verb's low word is 0; the
// Loot verb's flashes once
class UIVerbButtonClass : public ButtonGadgetClass	// NAME 1: neutral (D10)
{
public:
	UIVerbButtonClass(VerbEnum verb, RenderObjClass *robj, const LayerClass &layer);	// 0x659980  // NAME 1 [verb, robj, layer]: what it does with them
	virtual ~UIVerbButtonClass();						// +0x00 0x659AA0 (deleting 0x659A70)
	void Set_Verb(VerbEnum verb);						// 0x659AC0  // NAME 2: what it does
	VerbEnum Get_Verb();								// 0x659B30 (no callers)  // NAME 2: what it does
	void Start_Flash();									// 0x659B50 (Loot only, once)  // NAME 2: what it does
	void Stop_Flash();									// 0x659B90 (no callers)  // NAME 2: what it does

	VerbEnum Verb;										// +0x64  // NAME 5: the client's assert 'Verb_Type(verb)'
	bool Flashed;										// +0x68 (0x659B50)  // NAME 2: what it holds
};

// The verb window (0xE0 bytes, tables 0xAF94A4 / 0xAF9498 / 0xAF948C / 0xAF9478; constructor 0x6580B0, destructor 0x658280;
// 0x60D8D0 makes it): the buttons of the watched target's verbs, as the verb manager reports them
class UIVerbClass : public InterfaceClass, public UnknownObjectWatchClass		// NAME 3: class-from-file (D24) for ui_verb.cpp
{
public:
	UIVerbClass(UIViewClass *view, const LayerClass &layer, UIMeshClass *mesh, int arg);	// 0x6580B0  // NAME 1 [view, layer, mesh, arg]: what it does with them
	virtual ~UIVerbClass();								// 0x658280 (deleting 0x658250)
	virtual void Unknown_54();							// 0x6583B0
	virtual void Unknown_58();							// 0x6583D0 (only when not active)
	virtual void Unknown_5C();							// 0x6583F0 (only when active)
	virtual void Unknown_60();							// 0x658410
	virtual bool Unknown_68(int event);					// 0x658430 (a button's event: the verb's packet)  // NAME 5 [event]: the client's assert
	virtual void Unknown_88(ClientObjectClass *object);	// +0x88 0x658C90 (the target; slot +0x0C of the second base calls it with 0)  // NAME 2 [object]: what it holds
	virtual void Unknown_Watch_0C() { Unknown_88(NULL); }	// second base's +0x0C 0x658220

	void Unknown_658F40(const std::set<VerbEnum> *verbs);	// 0x658F40 (for each verb Add_Verb)  // NAME 1 [verbs]: VERB_LIST's field
	void Unknown_659010(const std::set<VerbEnum> *adds, const std::set<VerbEnum> *drops);	// 0x659010 (drops first: each one's button disabled; then adds as 0x658F40)  // NAME 1 [adds, drops]: VERB_UPDATE's fields
	ClientObjectClass *Get_Target() { return(Object); }	// NAME 2: what it does
	UIVerbButtonClass *Find_Button(VerbEnum verb);		// 0x6591F0  // NAME 2: what it does
	void Add_Verb(VerbEnum verb);						// 0x659240 (makes the button if there is none)  // NAME 2: what it does
	void Update_Verb(VerbEnum verb);					// 0x659710 (G_ASSERT 'vb')  // NAME 2: what it does
	void Clear_Buttons();								// 0x659870  // NAME 2: what it does

	UIMeshClass *InterfaceMesh;							// +0x9C (the 0x60D8D0's +0x188)  // NAME 2: ui_group.cpp's
	int UnknownA0;										// +0xA0 (0x60D8D0 passes 0)
	LayerClass Layer;									// +0xA4
	VerbManagerClass *VerbMangerPtr;					// +0xC8  // NAME 5: the client's assert (its spelling)
	int Count;											// +0xCC (buttons made)  // NAME 2: what it holds
	std::vector<UIVerbButtonClass *> Buttons;			// +0xD0 (its allocator; +0xD4 / +0xD8 / +0xDC the pointers)  // NAME 5: the client's assert 'Buttons.empty()'
};

#endif
