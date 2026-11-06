// Earth & Beyond client (reconstruction): app\client\ui_mfd.h, the multi-function display's interface (the client's
// ui_mfd.cpp per its asserts, 'e:\build\beta\G\app\Client\ui_mfd.cpp'; this header's name is provisional, NAME 1). Laid
// out from the constructor 0x5BB720 and the destructor 0x5BC2E0.
#ifndef UI_MFD_H  // NAME 1 [ui_mfd.h]: provisional header name for ui_mfd.cpp
#define UI_MFD_H

#include <vector>
#include "sclient.h"
#include "fontmanager.h"

class UIMFDClass;
class UnknownC8Class;		// 0x5DBFC0's object  // NAME 1: neutral (D10)

// What the display's vector holds (vtable 0xAF34DC, deleting destructor 0x5BBC90, 12 slots; the constructor 0x5BB720 makes
// one directly with this as Owner, the subclasses' Unknown_54 add theirs)
class UIMFDItemClass	// NAME 1: neutral (D10)
{
public:
	UIMFDItemClass(UIMFDClass *owner) : Owner(owner) {}	// NAME 2 [owner]: what it holds
	virtual ~UIMFDItemClass() {}						// 0x5BBC90

	UIMFDClass *Owner;									// +0x04  // NAME 2: what it holds
};

// The items 0x5BE570 and 0x5BEA00 add (their sizes 0xC / 0x44 and 0x58 / 0xC; constructors after ui_mfd.cpp, not rebuilt yet)
class Unknown5C3030Class : public UIMFDItemClass { public: Unknown5C3030Class(UIMFDClass *owner); int Unknown08; };	// NAME 1: neutral (D10)
class Unknown5C3920Class : public UIMFDItemClass { public: Unknown5C3920Class(UIMFDClass *owner); char Unknown08[0x3C]; };	// NAME 1: neutral (D10)
class Unknown5C2CD0Class : public UIMFDItemClass { public: Unknown5C2CD0Class(UIMFDClass *owner); char Unknown08[0x50]; };	// NAME 1: neutral (D10)
class Unknown5C3650Class : public UIMFDItemClass { public: Unknown5C3650Class(UIMFDClass *owner); int Unknown08; };	// NAME 1: neutral (D10)

// The multi-function display (0x188 bytes, tables 0xAF3544 / 0xAF3538 / 0xAF352C / 0xAF3518; constructor 0x5BB720, destructor
// 0x5BC2E0; 'fc_mfd.ini'): watches the target object through its second base; slot +0x90 is pure (0xAF35D4 _purecall)
class UIMFDClass : public InterfaceClass, public UnknownObjectWatchClass	// NAME 3: D24 class-from-file for ui_mfd.cpp
{
public:
	// One of the two value lines (0x48 bytes, the constructor's loop)
	struct InfoLineStruct	// NAME 1: neutral (D10)
	{
		int Value;										// +0x00 (-1 when the type changes; 0x5BCE50 reads it)  // NAME 2: what it holds
		char Unknown04[0x3C];							// +0x04
		RefCountPtr<FontSys> Font;						// +0x40 ('MFD_INFO_VALUE_%02d')  // NAME 2: what it holds
		int Type;										// +0x44 (0x5BCC60 steps it; 0x5BD540 turns it into a value)  // NAME 2: what it holds
	};

	UIMFDClass(UIViewClass *view, RenderObjClass *robj, const LayerClass &layer, int flag);	// 0x5BB720  // NAME 1 [view, robj, layer, flag]: what it does with them
	virtual ~UIMFDClass();								// 0x5BC2E0 (deleting 0x5BBCC0)
	virtual void Unknown_54();							// 0x5BBF70
	virtual void Unknown_58();							// 0x5BC510
	virtual void Unknown_5C();							// 0x5BC5A0
	virtual void Unknown_60();							// 0x5BC940
	virtual bool Unknown_68(int key);					// 0x5BC5E0
	virtual void Unknown_88(int arg);					// +0x88 0x5BC9E0 ('0 == monitor'; slot +0x0C of the second base calls it with 0)  // NAME 1 [arg]: neutral (D10)
	virtual int Unknown_8C() { return(5); }				// +0x8C 0x5BBB30
	virtual void Unknown_90() = 0;						// +0x90 (0x5BE860 / 0x5BECF0: the two lines' types for the object)
	virtual void Unknown_Watch_0C() { Unknown_88(0); }		// second base's +0x0C 0x5BBB00

	void Unknown_5BCE50(InfoLineStruct &line, bool arg);	// 0x5BCE50 (the line's text)  // NAME 1 [line, arg]: neutral (D10)

	// 0x5BCDE0 (inlined into the subclasses): the line's type, its text cleared and shown again
	void Unknown_5BCDE0(InfoLineStruct &line, int type)	// NAME 1 [line, type]: what it does
	{
		line.Type = type;
		line.Value = -1;
		if (type == 0) {
			line.Font->Set_Hidden(true);
		} else {
			line.Font->Set_Hidden(false);
		}
		line.Font->Change_Text("", 0);
		Unknown_5BCE50(line, true);
	}

	int Unknown9C;										// +0x9C (the constructor's last argument)
	RefCountPtr<RenderObjClass> RObj;					// +0xA0 (G_ASSERT '!RObj', 'RObj')  // NAME 5: the client's assert
	LayerClass Layer;									// +0xA4
	UnknownC8Class *UnknownC8;							// +0xC8 (0x28 bytes: 0x5DBFC0 in 0x5BBF70; the destructor deletes it)
	std::vector<UIMFDItemClass *> Items;				// +0xCC (its allocator; +0xD0 / +0xD4 / +0xD8 the pointers)  // NAME 2: what it holds
	bool UnknownDC;										// +0xDC (the constructor's 0)
	int UnknownE0;										// +0xE0 (the constructor's 0)
	int UnknownE4;										// +0xE4
	InfoLineStruct Lines[2];							// +0xE8, +0x130  // NAME 2: 'MFD_INFO_VALUE_%02d'
	void *Unknown178[4];								// +0x178 (the gadgets of 0x5BBF70's table 0xB8207C; [1] a ButtonGadgetClass)
};

// The display with slots +0x54 / +0x90 for what 0x5DD960's class builds on (constructor 0x5BE4F0: flag 0; tables 0xAF3678 /
// 0xAF366C / 0xAF3660 / 0xAF364C)
class Unknown5BE4F0Class : public UIMFDClass	// NAME 1: neutral (D10)
{
public:
	Unknown5BE4F0Class(UIViewClass *view, RenderObjClass *robj, const LayerClass &layer);	// 0x5BE4F0  // NAME 1 [view, robj, layer]: the base's
	virtual ~Unknown5BE4F0Class();						// 0x5BE820 (deleting 0x5BE540)
	virtual void Unknown_54();							// 0x5BE570
	virtual void Unknown_90();							// 0x5BE860
};

// The display UIScanClass builds on (constructor 0x5BE980: flag 1; tables 0xAF3758 / 0xAF374C / 0xAF3740 / 0xAF372C)
class Unknown5BE980Class : public UIMFDClass	// NAME 1: neutral (D10)
{
public:
	Unknown5BE980Class(UIViewClass *view, RenderObjClass *robj, const LayerClass &layer);	// 0x5BE980  // NAME 1 [view, robj, layer]: the base's
	virtual ~Unknown5BE980Class();						// 0x5BECB0 (deleting 0x5BE9D0)
	virtual void Unknown_54();							// 0x5BEA00
	virtual void Unknown_90();							// 0x5BECF0
};

#endif
