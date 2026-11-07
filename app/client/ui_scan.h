// Earth & Beyond client (reconstruction): the scanner of the client's ui_scan.cpp (per its asserts,
// 'e:\build\beta\G\app\Client\ui_scan.cpp'; this header's name is provisional, NAME 1)
#ifndef UI_SCAN_H  // NAME 1 [ui_scan.h]: provisional header name for ui_scan.cpp
#define UI_SCAN_H

#include <string>
#include "ui_mfd.h"

class UIScanClass;
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
	UIScanThreatClass(UIScanClass *owner) : Owner(owner), Unknown14(0), Unknown18(false), Unknown20(NULL), Unknown24(NULL), Unknown28(0), Unknown2C(0), Unknown30(0) {}	// NAME 2 [owner]: what it holds
	~UIScanThreatClass();
	void Unknown_60FEE0();								// 0x60FEE0 (the scanner's 0x60E580 when the target goes; the destructor between hide and show)

	UIScanClass *Owner;									// +0x00  // NAME 2: what it holds
	std::string Assessment;								// +0x04 (the constructor's '')  // NAME 1: neutral
	int Unknown14;										// +0x14
	bool Unknown18;										// +0x18
	int Unknown1C;										// +0x1C
	FontSys *Unknown20;									// +0x20 (the destructor's 0x51C630: a RefCountPtr<FontSys>)
	RenderObjClass *Unknown24;							// +0x24 (the destructor's 0x4740B0: a RefCountPtr<RenderObjClass>)
	int Unknown28;										// +0x28
	int Unknown2C;										// +0x2C
	int Unknown30;										// +0x30
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
	void Unknown_60ED10(PlayerObjectRelationshipChangeEvent *event, ClientObjectClass *object);	// 0x60ED10: the target's relationship changed  // NAME 2 [event, object]: the event's type
	void Unknown_60EFE0(ClientObjectClass *object);		// 0x60EFE0 (0x60E580's new target)  // NAME 2 [object]: what 0x60E580 passes
	void Unknown_60F1A0(bool arg);						// 0x60F1A0 (0x60E480 passes 0, 0x60E580 1)  // NAME 1 [arg]: neutral
	void Create_Mesh();									// (inline in 0x60D8D0: G_ASSERT '!UIMesh' l.270)  // NAME 2: what it does
	TargetingClass *Get_Targeting();					// (inline in 0x60E300: FAIL_IFs '!Client', '!sclient', '!ts' l.733-739)  // NAME 2: what it returns

	UIMeshClass *UIMesh;								// +0x188  // NAME 5: the client's asserts 'UIMesh', '!UIMesh'
	GadgetClass *Buttons[3];							// +0x18C (the constructor's memset; [1] 'PREV_TARGET' id 6, [2] 'NEXT_TARGET' id 7)  // NAME 2: what it holds
	int Unknown198;										// +0x198 (the constructor: -1)
	UIVerbClass *Verb;									// +0x19C (0xE0 bytes, 0x6580B0)  // NAME 2: what it holds
	UIScanThreatClass *Threat;							// +0x1A0 (0x38 bytes)  // NAME 1: neutral
	int Unknown1A4;										// +0x1A4
	RefCountPtr<FontSys> ConFont;						// +0x1A8 ('MFD_CON')  // NAME 2: what it holds
	RefCountPtr<FontSys> FactionFont;					// +0x1AC ('MFD_FACTION')  // NAME 2: what it holds
};

#endif
