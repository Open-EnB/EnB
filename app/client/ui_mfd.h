// Earth & Beyond client (reconstruction): app\client\ui_mfd.h, the multi-function display's interface (the client's
// ui_mfd.cpp per its asserts, 'e:\build\beta\G\app\Client\ui_mfd.cpp'; this header's name is provisional, NAME 1). Laid
// out from the constructor 0x5BB720 and the destructor 0x5BC2E0.
#ifndef UI_MFD_H  // NAME 1 [ui_mfd.h]: provisional header name for ui_mfd.cpp
#define UI_MFD_H

#include <vector>
#include "sclient.h"
#include "fontmanager.h"

class UIMFDClass;
class ClientObjectClass;
class GadgetClass;

// What the display's vector holds (vtable 0xAF34DC, deleting destructor 0x5BBC90, 12 slots; the constructor 0x5BB720 makes
// one directly with this as Owner, the subclasses' Unknown_54 add theirs)
class UIMFDItemClass	// NAME 1: neutral (D10)
{
public:
	UIMFDItemClass(UIMFDClass *owner) : Owner(owner) {}	// NAME 2 [owner]: what it holds
	virtual ~UIMFDItemClass() {}						// 0x5BBC90
	virtual void Unknown_04(ClientObjectClass *object) {}	// +0x04 0x5BBB50 (0x5BC9E0: the display's new object)  // NAME 2 [object]: what 0x5BC9E0 passes
	virtual void Unknown_08() {}						// +0x08 0x5BBB70 (0x5BC940, 0x5BCB30: shown)
	virtual void Unknown_0C() {}						// +0x0C 0x5BBB90 (0x5BC5A0, 0x5BC940, 0x5BC9E0: hidden)
	virtual void Unknown_10() {}						// +0x10 0x5BBBB0 (0x5BC940: every frame)
	virtual bool Unknown_14();							// +0x14 0x5C1FE0 (0x5BCB30: can it be shown; emitted after ui_mfd.cpp's object)
	virtual int Unknown_18() { return(0); }				// +0x18 0x5BBBD0 (0x5BCB00)
	virtual int Unknown_1C() { return(0); }				// +0x1C 0x5BBBF0
	virtual int Unknown_20() { return(0); }				// +0x20 0x5BBC10
	virtual int Unknown_24() { return(0); }				// +0x24 0x5BBC30
	virtual int Unknown_28() { return(0); }				// +0x28 0x5BBC50
	virtual int Unknown_2C() { return(0); }				// +0x2C 0x5BBC70

	UIMFDClass *Owner;									// +0x04  // NAME 2: what it holds
};

// The items 0x5BE570 and 0x5BEA00 add (their sizes 0xC / 0x44 and 0x58 / 0xC; constructors after ui_mfd.cpp's object)
class Unknown5C3030Class : public UIMFDItemClass { public: Unknown5C3030Class(UIMFDClass *owner); int Unknown08; };	// NAME 1: neutral (D10)
class Unknown5C3920Class : public UIMFDItemClass { public: Unknown5C3920Class(UIMFDClass *owner); char Unknown08[0x3C]; };	// NAME 1: neutral (D10)
class Unknown5C2CD0Class : public UIMFDItemClass { public: Unknown5C2CD0Class(UIMFDClass *owner); char Unknown08[0x50]; };	// NAME 1: neutral (D10)
class Unknown5C3650Class : public UIMFDItemClass { public: Unknown5C3650Class(UIMFDClass *owner); int Unknown08; };	// NAME 1: neutral (D10)

// A value line's value (0x10 bytes), as 0x5BD540 fills it: the asserts' 'max.IsInt' / 'per.IsFloat' are its flags
struct UIMFDValueStruct	// NAME 1: neutral (D10)
{
	ClientObjectClass *Self;							// +0x00 (the client's slot +0x28)  // NAME 2: what it holds
	ClientObjectClass *Target;							// +0x04 (the display's object)  // NAME 2: what it holds
	union {
		int Int;										// NAME 2: flag 4, 0x10, 0x20, 0x40
		float Float;									// NAME 2: flag 8
		const char *String;								// NAME 2: flags 1, 2
	};													// +0x08
	union {
		unsigned int Flags;								// +0x0C (1 string to free, 2 string, 0x10 kilo, 0x20 time, 0x40 left or right)  // NAME 2: what it holds
		struct {
			unsigned int Unknown0 : 2;					// 1, 2
			unsigned int IsInt : 1;						// 4  // NAME 5: the client's assert 'max.IsInt'
			unsigned int IsFloat : 1;					// 8  // NAME 5: the client's assert 'per.IsFloat'
		};
	};
};

// The multi-function display (0x188 bytes, tables 0xAF3544 / 0xAF3538 / 0xAF352C / 0xAF3518; constructor 0x5BB720, destructor
// 0x5BC2E0; 'fc_mfd.ini'): watches the target object through its second base; slot +0x90 is pure (0xAF35D4 _purecall)
class UIMFDClass : public InterfaceClass, public UnknownObjectWatchClass	// NAME 3: D24 class-from-file for ui_mfd.cpp
{
public:
	// One of the two value lines (0x48 bytes, the constructor's loop)
	struct InfoLineStruct	// NAME 1: neutral (D10)
	{
		InfoLineStruct() : Font(NULL), Type(0) { memset(Text, 0, sizeof(Text)); }
		char Text[0x40];								// +0x00 (what was shown last; its first dword the last int or float, -1 when the type changes)  // NAME 2: what it holds
		FontSys *Font;									// +0x40 ('MFD_INFO_VALUE_%02d')  // NAME 2: what it holds
		int Type;										// +0x44 (0x5BCC60 steps it; 0x5BD540 turns it into a value)  // NAME 2: what it holds
	};

	UIMFDClass(UIViewClass *view, RenderObjClass *robj, const LayerClass &layer, int flag);	// 0x5BB720  // NAME 1 [view, robj, layer, flag]: what it does with them
	virtual ~UIMFDClass();								// 0x5BC2E0 (deleting 0x5BBCC0)
	virtual void Unknown_54();							// 0x5BBF70
	virtual void Unknown_58();							// 0x5BC510
	virtual void Unknown_5C();							// 0x5BC5A0
	virtual void Unknown_60();							// 0x5BC940
	virtual bool Unknown_68(int key);					// 0x5BC5E0  // NAME 2 [key]: InterfaceClass's
	virtual void Unknown_88(ClientObjectClass *object);	// +0x88 0x5BC9E0 ('0 == monitor'; slot +0x0C of the second base calls it with 0)  // NAME 2 [object]: what it holds
	virtual int Unknown_8C() { return(5); }				// +0x8C 0x5BBB30
	virtual void Unknown_90() = 0;						// +0x90 (0x5BE860 / 0x5BECF0: the two lines' types for the object)
	virtual void Unknown_Watch_0C() { Unknown_88(0); }	// second base's +0x0C 0x5BBB00

	void Add_Item(UIMFDItemClass *item);				// 0x5BBCF0  // NAME 2: what it does
	void Set_RObj(RenderObjClass *robj);				// 0x5BBF20 (inlined into the constructor; G_ASSERT '!RObj')  // NAME 2 [Set_RObj]: what it does
	bool Unknown_5BCB00();								// 0x5BCB00 (the current item's slot +0x18)
	void Unknown_5BCB30();								// 0x5BCB30 (on to the next item that can be shown)
	void Next_Type(InfoLineStruct &line);				// 0x5BCC60  // NAME 2: what it does
	void Previous_Type(InfoLineStruct &line);			// 0x5BCD20  // NAME 2: what it does
	void Set_Type(InfoLineStruct &line, int type);		// 0x5BCDE0 (the line's type, its text cleared and shown again)  // NAME 2: what it does
	void Update_Line(InfoLineStruct &line, bool force);	// 0x5BCE50 (the line's text)  // NAME 2: what it does
	bool Has_Value(int type);							// 0x5BD4C0  // NAME 2: what it does
	ClientObjectClass *Unknown_5BD500();				// 0x5BD500 (the client's slot +0x28)
	ClientObjectClass *Unknown_5BD520() { return(Object); }	// 0x5BD520
	void Get_Value(int type, UIMFDValueStruct &value);	// 0x5BD540  // NAME 2: what it does
	void Set_Name(UIMFDValueStruct &value);				// 0x5BD9A0 (line label: none)  // NAME 2: what it does
	void Set_Turn_To(UIMFDValueStruct &value);			// 0x5BDAB0 ('Turn To: ')  // NAME 3: the client's label
	void Set_Dist(UIMFDValueStruct &value);				// 0x5BDBE0 ('Dist: ')  // NAME 3: the client's label
	void Set_ETA(UIMFDValueStruct &value);				// 0x5BDC20 ('ETA: ')  // NAME 3: the client's label
	void Set_Dir(UIMFDValueStruct &value);				// 0x5BDD80 ('Dir: ')  // NAME 3: the client's label
	void Set_Same_Dir(UIMFDValueStruct &value);			// 0x5BDDF0 ('Same Dir: ')  // NAME 3: the client's label
	void Set_SP_Cur(UIMFDValueStruct &value);			// 0x5BDED0 ('SP Cur: ')  // NAME 3: the client's label
	void Set_SP_Percent(UIMFDValueStruct &value, bool as_float);	// 0x5BE120 ('SP %: ')  // NAME 3: the client's label; NAME 2 [as_float]: what it does
	void Set_SP_Max(UIMFDValueStruct &value);			// 0x5BE330 ('SP Max: ')  // NAME 3: the client's label
	void Set_Level(UIMFDValueStruct &value);			// 0x5BE3F0 ('Lev: ', empty)  // NAME 3: the client's label
	void Set_Prof(UIMFDValueStruct &value);				// 0x5BE410 ('Prof: ', empty)  // NAME 3: the client's label
	void Set_Race(UIMFDValueStruct &value);				// 0x5BE430 ('Race: ', empty)  // NAME 3: the client's label
	void Set_Vel(UIMFDValueStruct &value);				// 0x5BE450 ('Vel: ')  // NAME 3: the client's label
	void Set_Rel_Vel(UIMFDValueStruct &value);			// 0x5BE4D0 ('RelVel: ', empty)  // NAME 3: the client's label

	int Unknown9C;										// +0x9C (the constructor's last argument: 1 lets 0x5BC5E0's shifted key reach the starmap)
	RenderObjClass *RObj;								// +0xA0  // NAME 5: the client's assert
	LayerClass Layer;									// +0xA4
	UICpitUnknownD0Class *UnknownC8;					// +0xC8 (0x28 bytes: 0x5DBFC0 in 0x5BBF70; the destructor deletes it)
	std::vector<UIMFDItemClass *> Items;				// +0xCC (its allocator; +0xD0 / +0xD4 / +0xD8 the pointers)  // NAME 2: what it holds
	bool UnknownDC;										// +0xDC (0x5BC940: the current item shown once again)
	int Current;										// +0xE0 (the index into Items)  // NAME 2: what it holds
	int UnknownE4;										// +0xE4
	InfoLineStruct Lines[2];							// +0xE8, +0x130  // NAME 2: 'MFD_INFO_VALUE_%02d'
	GadgetClass *Gadgets[4];							// +0x178 (of 0x5BBF70's table 0xB8207C, 1..3; [1] with the mask 0x1055)  // NAME 2: what it holds
};

void Unknown_5BF4B0();									// 0x5BF4B0 (the constructor's last: ui_mfd_extended_context.cpp's; 'fa_harvesting.ini' loaded)
void Unknown_5BF4D0();									// 0x5BF4D0 (the destructor's: the same file unloaded)

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
