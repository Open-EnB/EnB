// Earth & Beyond client (reconstruction): the scanner of the client's ui_scan.cpp (per its asserts,
// 'e:\build\beta\G\app\Client\ui_scan.cpp'; this header's name is provisional, NAME 1)
#ifndef UI_SCAN_H  // NAME 1 [ui_scan.h]: provisional header name for ui_scan.cpp
#define UI_SCAN_H

#include <string>
#include "ui_mfd.h"
#include "..\shared\duration.h"

class UIScanClass;
class LineDisplayClientClass;

#include "ui_interrupt.h"	// the threat's +0x2C: ui_interrupt.cpp's progress display (Unknown59E2C0Class, moved there: O15)
class UIVerbClass;
class UIMeshClass;
class GadgetClass;
class ClientObjectClass;
class TargetingClass;
class PlayerObjectRelationshipChangeEvent;

// What the scanner holds at +0x1A0 (0x38 bytes, made inline in 0x60D8D0, deleted inline in 0x60DF90): the target's threat
// (the main view's 0x513630 passes TargetThreat, TargetThreatSound, TargetThreatLevel through 0x60E540)
class UIScanThreatClass	// NAME 1: neutral (D10)
{
public:
	UIScanThreatClass(UIScanClass *owner) : Owner(owner), Unknown18(false), Unknown20(0), Unknown28(0), Unknown2C(NULL) {}	// NAME 2 [owner]: what it holds
	~UIScanThreatClass();
	UIScanClass *Get_Interface() { return(Owner); }		// (inline: 0x60FEE0's G_ASSERT 'Get_Interface()->Is_Active()', as UIGroupClass's members')  // NAME 3: the assert's
	void Unknown_60FEE0();								// 0x60FEE0 (the scanner's 0x60E580 when the target goes; the destructor between hide and show)
	void Unknown_Update() { Unknown2C->Unknown38 = (InterruptStateType)Unknown28; if (Unknown2C) { Unknown2C->Unknown_10(); } }	// (inline in 0x60F1A0: the progress display's state and redraw)  // NAME 1: neutral (D10)
	void Unknown_Clear()								// (inline in 0x60E6A0, and 0x60FEE0's after its assert: the progress display deleted, the state and name cleared)  // NAME 1: neutral (D10)
	{
		if (Unknown2C) {
			Unknown2C->Unknown_59E970();
			delete Unknown2C;
			Unknown2C = NULL;
		}
		Unknown28 = 0;
		AbilityName = "";
		Unknown18 = false;
	}
	void Unknown_Refresh();								// (inline in 0x60DF90 and 0x60F1A0: 0x60FEE0 between the owner's hide and show)  // NAME 1: neutral (D10)

	UIScanClass *Owner;									// +0x00  // NAME 2: what it holds
	std::string AbilityName;							// +0x04 (the constructor's ''; 0x60E6A0: the target's InterruptibleAbilityName or 'Unknown Skill', shown on +0x14)  // NAME 2: what it holds
	RefCountPtr<FontSys> Unknown14;						// +0x14 (the destructor's 0x51C630; 0x60F1A0's 'Interrupted!' and the rest, 0x60FEE0's hide)
	bool Unknown18;										// +0x18 (0x60F1A0: set with the time +0x20, cleared when the delay has passed)
	DurationClass Unknown20;							// +0x20 (0x60F1A0: the client clock +0x10BC when the state changed)
	int Unknown28;										// +0x28 (0x60F1A0: the interrupt state shown, 1 to 5)
	Unknown59E2C0Class *Unknown2C;						// +0x2C (0x60E6A0's)
	RefCountPtr<RenderObjClass> Unknown30;				// +0x30 (the destructor's 0x4740B0)
	int Unknown34;										// +0x34
};

// The scanner (0x1B0 bytes, tables 0xAF724C / 0xAF7240 / 0xAF7234 / 0xAF7220; constructor 0x60D800, destructor 0x60DF90,
// deleting 0x60D8A0): the cockpit's +0xD4, the display Unknown5BE980Class over the 'TSI08A' mesh, its two target buttons,
// the verb window and the con / faction labels
class UIScanClass : public Unknown5BE980Class	// NAME 3: class-from-file (D24) for ui_scan.cpp
{
public:
	UIScanClass(UIViewClass *view, LayerClass *layer);	// 0x60D800 (the cockpit's set-up passes the main view and its layer)  // NAME 1 [view, layer]: what 0x57DD20 passes
	virtual ~UIScanClass();								// 0x60DF90 (deleting 0x60D8A0)
	virtual void Unknown_54();							// 0x60D8D0
	virtual void Unknown_58();							// 0x60E1E0
	virtual void Unknown_5C();							// 0x60E2C0
	virtual void Unknown_60();							// 0x60E480
	virtual bool Unknown_68(int key);					// 0x60E300 (ids 6 / 7: the previous / next target)  // NAME 2 [key]: InterfaceClass's
	virtual void Unknown_88(ClientObjectClass *object);	// +0x88 0x60E580  // NAME 2 [object]: UIMFDClass's
	virtual void Unknown_Watch_0C() { Unknown_88(0); }	// second base's +0x0C

	void Unknown_60E540(ClientObjectClass *target, const std::string &assessment, const std::string &sound, int level);	// 0x60E540 (the main view's 0x513630)  // NAME 1 [target, assessment, sound, level]: what the main view's 0x512400 passes (TargetThreat, TargetThreatSound, TargetThreatLevel)
	void Unknown_60ED60(ClientObjectClass *target, const std::string &assessment, const std::string &sound, int level);	// 0x60ED60 (the threat onto +0x1A0)  // NAME 1 [target, assessment, sound, level]: 0x60E540's
	void Unknown_60E6A0(ClientObjectClass *object);		// 0x60E6A0 (0x60F1A0 when the target's InterruptState turns 1: the threat's +0x2C made)  // NAME 2 [object]: what 0x60F1A0 passes
	void Unknown_60ED10(PlayerObjectRelationshipChangeEvent *event, ClientObjectClass *object);	// 0x60ED10: the target's relationship changed  // NAME 2 [event, object]: the event's type
	void Unknown_60EFE0(ClientObjectClass *object);		// 0x60EFE0 (0x60E580's new target)  // NAME 2 [object]: what 0x60E580 passes
	void Unknown_60F1A0(bool arg);						// 0x60F1A0 (0x60E480 passes 0, 0x60E580 1)  // NAME 1 [arg]: neutral
	void Create_Mesh();									// (inline in 0x60D8D0: G_ASSERT '!UIMesh' l.270)  // NAME 2: what it does
	ClientObjectClass *Peek_Object() { return(Object); }	// (inline: 0x60ED10's FAIL_IF 'object != Peek_Object()')  // NAME 3: the FAIL_IF's
	LineDisplayClientClass *Get_Client() { return(Client); }	// (inline: 0x60F1A0's FAIL_IF '!Get_Client()', as UIFactionClass's)  // NAME 3: the FAIL_IF's
	TargetingClass *Get_Targeting();					// (inline in 0x60E300: FAIL_IFs '!Client', '!sclient', '!ts' l.733-739)  // NAME 2: what it returns

	UIMeshClass *UIMesh;								// +0x188  // NAME 5: the client's asserts 'UIMesh', '!UIMesh'
	GadgetClass *Buttons[3];							// +0x18C (the constructor's memset; [1] 'PREV_TARGET' id 6, [2] 'NEXT_TARGET' id 7)  // NAME 2: what it holds
	int Unknown198;										// +0x198 (the constructor: -1)
	UIVerbClass *Verb;									// +0x19C (0xE0 bytes, 0x6580B0)  // NAME 2: what it holds
	UIScanThreatClass *InterruptInfo;					// +0x1A0 (0x38 bytes)  // NAME 5: 0x60E6A0's G_ASSERT 'InterruptInfo' (l.571)
	int Unknown1A4;										// +0x1A4
	RefCountPtr<FontSys> ConFont;						// +0x1A8 ('MFD_CON')  // NAME 2: what it holds
	RefCountPtr<FontSys> FactionFont;					// +0x1AC ('MFD_FACTION')  // NAME 2: what it holds
};

inline void UIScanThreatClass::Unknown_Refresh()
{
	bool active = Owner->Is_Active();
	if (active) {
		Owner->Unknown_5C();
	}
	Unknown_60FEE0();
	if (active) {
		Owner->Unknown_58();
	}
}

#endif
