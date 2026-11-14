// Earth & Beyond client (reconstruction): app\client\gd_buffgadg.cpp, the buff gadget (the client's file per its
// asserts, 'e:\build\beta\G\app\Client\gd_buffgadg.cpp' at 0xB8CEF0). Notes at the end of the file.
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <string>
#include <vector>
#include <algorithm>
#include "gd_buffgadg.h"
#include "client.h"
#include "rendobj.h"
#include "fontmanager.h"
#include "buff_database.h"
#include "..\shared\g_assert.h"
#include "..\shared\duration.h"
#include "..\..\ww3d2\ww3d.h"
#include "..\..\wwmath\vector2.h"

static DurationClass PulseTime(30);	// 0xBE5FA0 (seconds; initialised by 0x665150): the last seconds of a buff pulse  // NAME 1: what 0x665F20 does with it

// 0x665180
BuffInfoStruct::BuffInfoStruct() : IsPermanent(false), BuffRemovalTime(0)
{
}

// 0x6651C0
void BuffInfoStruct::Set_Scrub_Type_Name(const std::string &name)
{
	ScrubTypeName = name;
}

// 0x665340
void BuffInfoStruct::Set_Is_Permanent(bool permanent)
{
	IsPermanent = permanent;
}

// 0x665360
void BuffInfoStruct::Set_Buff_Removal_Time(const DurationClass &time)
{
	BuffRemovalTime = time;
}

// 0x665390
void BuffInfoStruct::Add_Element(const BuffElementStruct &element)
{
	Elements.push_back(element);
}

// 0x665630
unsigned int BuffInfoStruct::Get_Element_Count() const
{
	return Elements.size();
}

// 0x665670
int BuffInfoStruct::Get_Active_Count() const
{
	int count = 0;
	for (std::vector<BuffElementStruct>::const_iterator it = Elements.begin(); it != Elements.end(); ++it) {
		if (it->IsActive) {
			count++;
		}
	}
	return count;
}























// (exact since 0x665940's expiry goes through DurationClass(__int64): the static element's zero expiry stores ebx twice)
// 0x6656A0
const BuffElementStruct &BuffInfoStruct::Get_Element(unsigned int index) const
{
	FAIL_IF(index >= Elements.size()) {
		static BuffElementStruct empty("", "", 0, 0, false);
		return empty;
	}
	return Elements[index];
}

// 0x665940 (one dead read in the body: the copies' assign out of line as the client's; the expiry through DurationClass(__int64) for its store order)
BuffElementStruct::BuffElementStruct(const std::string &source_entity, const std::string &source_object, int magnitude, const DurationClass &expiration_time, bool is_active) :
	SourceEntity(source_entity), SourceObject(source_object), ExpirationTime(expiration_time.Time), Magnitude(magnitude), IsActive(is_active)
{
	SourceEntity.size();
}

// The elements' order (0x667950): the active first, then the larger magnitude, the later expiry, the names
struct BuffElementSortClass	// NAME 1: neutral (D10)
{
	bool operator()(const BuffElementStruct &a, const BuffElementStruct &b) const
	{
		if (a.IsActive != b.IsActive) {
			return a.IsActive;
		}
		if (a.Magnitude != b.Magnitude) {
			return a.Magnitude > b.Magnitude;
		}
		if (a.ExpirationTime != b.ExpirationTime) {
			return a.ExpirationTime.Time > b.ExpirationTime.Time;
		}
		int result = stricmp(a.SourceEntity.c_str(), b.SourceEntity.c_str());
		if (result < 0) {
			return true;
		}
		if (result > 0) {
			return false;
		}
		return stricmp(a.SourceObject.c_str(), b.SourceObject.c_str()) < 0;
	}
};

// 0x665A10
void BuffInfoStruct::Sort_Elements()
{
	std::sort(Elements.begin(), Elements.end(), BuffElementSortClass());
}
















// 0x665A40
BuffGadgetClass::BuffGadgetClass(ClientClass *client, BuffDefStruct *buffinfo, const BuffInfoStruct &info, RenderObjClass *robj, int unknown, const LayerClass &layer) :
	Unknown_671300Class(robj, unknown, layer, 0, 0, 3),
	BuffInfo(buffinfo),
	Unknown84(false),
	Unknown_B8(0),
	Unknown_BC(0),
	Unknown_C4(""),
	Client(client)
{
	Unknown60 &= ~3;
	BuffCount = Create_NEW(new FontSys("BuffCount", 0, 0));
	G_ASSERT(BuffCount);
	G_ASSERT(robj);
	if (robj) {
		robj->Add_Sub_Object(BuffCount.Peek());
	}
	Unknown_6661F0(info, DurationClass((unsigned int)Client->Unknown10BC));
	Unknown_666570();
	Set_Unknown14(Unknown_C4.c_str());
}



// 0x665D60
BuffGadgetClass::~BuffGadgetClass()
{
	RenderObjClass *robj = Peek_Render_Obj();
	G_ASSERT(robj);
	if (robj) {
		robj->Remove_Sub_Object(BuffCount.Peek());
	}
}

// NOT EXACT: frame (the client keeps the time now on the stack, ours in registers; the body after it matches)
// 0x665F20
void BuffGadgetClass::Unknown_AC()
{
	float frame_time = WW3D::Get_Frame_Time() * 0.001f;
	DurationClass now((unsigned int)Client->Unknown10BC);
	DurationClass left(0);
	if (Info.IsPermanent || (Info.BuffRemovalTime != 0 && now.Time <= Info.BuffRemovalTime.Time)) {
		left = Info.BuffRemovalTime - now;
	}
	if (!Info.IsPermanent && Unknown_B8 > Unknown_BC) {
		Unknown_BC += frame_time;
		if (Unknown_BC > Unknown_B8) {
			Unknown_BC = 0;
			Unknown_B8 = 0;
			Unknown_66C2A0(1.0f);
		} else {
			Unknown_66C2A0(Unknown_BC / Unknown_B8);
		}
	} else if (!Info.IsPermanent && left < DurationClass(PulseTime.Time * 1000)) {
		float speed = (DurationClass(PulseTime.Time * 1000) - left).Get() * 0.0005f;
		if (speed < 3.0f) {
			speed = 3.0f;
		} else if (speed > 15.0f) {
			speed = 15.0f;
		}
		Unknown_BC += (int)speed * frame_time;
		Unknown_66C2A0((float)((cos(Unknown_BC) + 1.0) * 0.5));
	} else {
		Unknown_66C2A0(1.0f);
		Unknown_B8 = 0;
		Unknown_BC = 0;
	}
	if (Unknown68 != NULL && Unknown74) {
		Unknown6C += frame_time;
		Unknown68->Unknown_4A13A0(Vector2(Unknown6C, 0));
	}
}

// NOT EXACT: register (explain.py: identical once registers are normalised; ecx / edx swapped at the time compare)
// 0x6661F0
bool BuffGadgetClass::Unknown_6661F0(const BuffInfoStruct &info, const DurationClass &now)
{
	bool was_permanent = Info.IsPermanent;
	unsigned int old_count = Info.Elements.size();
	unsigned int count = info.Elements.size();
	bool grew = count > old_count;
	bool later = info.BuffRemovalTime.Time > now.Time && !was_permanent && info.BuffRemovalTime.Time > Info.BuffRemovalTime.Time;
	Info = info;
	Unknown_666570();
	if (!Unknown84) {
		Set_Unknown14(Unknown_C4.c_str());
	}
	if (!was_permanent && Info.BuffRemovalTime != info.BuffRemovalTime) {
		Unknown_B8 = 0;
		Unknown_BC = 0;
	}
	int active = 0;
	for (std::vector<BuffElementStruct>::const_iterator it = info.Elements.begin(); it != info.Elements.end(); ++it) {
		if (it->IsActive) {
			active++;
		}
	}
	char text[12];
	if (count <= 1) {
		strcpy(text, "");
	} else {
		_snprintf(text, sizeof(text), "%u", count);
	}
	BuffCount->Change_Text(text, 0);
	return grew || later;
}

// 0x666440
void BuffGadgetClass::Unknown_666440(bool unknown)
{
	Unknown_B8 = 1.0f;
	Unknown_BC = 0;
}

// 0x666470
bool BuffGadgetClass::Unknown_666470()
{
	if (Info.IsPermanent) {
		return false;
	}
	if (Info.BuffRemovalTime == 0) {
		return true;
	}
	return DurationClass((unsigned int)Client->Unknown10BC).Time > Info.BuffRemovalTime.Time;
}

// 0x6664E0
void BuffGadgetClass::Unknown_6664E0(bool on)
{
	if (on != Unknown84) {
		Unknown84 = on;
		if (on) {
			Unknown2C_06 = true;
			if (BuffInfo != NULL) {
				Set_Unknown14(BuffInfo->AltToolTip);
			}
		} else {
			Unknown2C_06 = false;
			if (BuffInfo != NULL) {
				Set_Unknown14(Unknown_C4.c_str());
			}
		}
	}
}

// 0x666570
void BuffGadgetClass::Unknown_666570()
{
	Unknown_C4 = "";
	if (BuffInfo != NULL) {
		Unknown_C4 += BuffInfo->ToolTip;
	}
	std::string type = Info.Get_Scrub_Type_Name();
	if (type.length() != 0) {
		if (Unknown_C4.length() != 0) {
			Unknown_C4 += "\n";
		}
		Unknown_C4 += std::string("(") + type + ")";
	}
	unsigned int count = Info.Get_Element_Count();
	for (unsigned int i = 0; i < count; i++) {
		const BuffElementStruct &element = Info.Get_Element(i);
		Unknown_C4 += "\n ";
		if (element.IsActive) {
			Unknown_C4 += '+';
		} else {
			Unknown_C4 += ' ';
		}
		if (element.SourceEntity.length() == 0) {
			Unknown_C4 += "(unknown)";
		} else {
			Unknown_C4 += element.SourceEntity;
		}
		Unknown_C4 += ": ";
		Unknown_C4 += element.SourceObject;
	}
}

/*
**	Notes (GADG, 2026-11-04)
**
**	The client's object 0x665150-0x666441 and on (gd_buffgadg.obj: its __FILE__ 'e:\build\beta\G\app\Client\gd_buffgadg.cpp'
**	at 0xB8CEF0; FAIL_IF 'index >= Elements.size()' on line 94, G_ASSERT 'BuffCount' / 'robj' on 166 / 167, 'robj' on
**	182), linked after gd_attrb_skills.cpp (whose static's initialiser is 0x665120). First PulseTime's initialiser
**	0x665150 (too short to count), then BuffInfoStruct's members 0x665180-0x665A10 (the element constructor 0x665940
**	between them, inlined into 0x6656A0's static), the gadget's constructor 0x665A40, its deleting destructor 0x665D30
**	and destructor 0x665D60, slot +0xAC 0x665F20, 0x6661F0, 0x666440, 0x666470 (the buff is over), 0x6664E0 (the
**	alternative tooltip) and 0x666570 (the tooltip text: the BuffDefStruct's tool tip, '(type)', then a line per element),
**	then the template instances. The COMDATs this file brings that match: Get_Scrub_Type_Name 0x6669D0, the info's vector
**	constructor / operator= 0x666B50 / 0x666B80, the element vector's _Ufill / _Ucopy / fill / copy_backward / _Destroy
**	(0x666EE0 / 0x666E90 / 0x666F20 / 0x6670A0 / 0x561BD0), BuffElementStruct::operator= 0x667300, std::sort's _Sort_0 /
**	_Sort / _Unguarded_insert 0x667540 / 0x667B60 / 0x667D20 and the order BuffElementSortClass 0x667950.
**
**	Forms the client needs: the order's expiry test on Time ('>', duration.h has no operator>); 0x6661F0 computes 'grew'
**	before the expiry test and compares the old and new BuffRemovalTime after the assignment (as the client does).
**	RObjGadgetClass::Unknown_66C2A0 takes a float (0x665F20 passes the pulse 0..1; ro_funcs.cpp's 0x496240 'opacity').
**	0x666570 appends '+' or ' ' through an if / else (the client's two pushes before one call: the tails merged; a ?:
**	gives neg / sbb). Not matching, the same residue each time: the element's copies 0x665940, its implicit copy
**	constructor 0x667A60 and _Construct 0x667230 call basic_string::assign where ours inlines it.
*/
