// Earth & Beyond client (reconstruction): app\client\ui_mfd.cpp, the multi-function display (per the client's asserts)
#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
#include "sclient.h"
#include "client.h"
#include "rendobj.h"
#include "co.h"
#include "gadget.h"
#include "unknown_675120.h"
#include "local_interface_sort.h"
#include "fontdatabase.h"
#include "..\shared\g_assert.h"
#include "..\audio\soundlib.h"
#include "..\shared\auxdata\auxdata.h"
#include "..\shared\auxdata\delta_interpolator.h"
#include <math.h>
#include "nstrdup.h"
#include "..\shared\auxdata\permstr.h"	// (its PermStringInitClass static 0xBE5C8C, initialised by 0x5BB6D0)
#include "ui_mfd.h"
#include "ui_power.h"		// UIPowerClass (+0xC8)
extern unsigned int Unknown_BE5F34;
template<class C> inline C *Unknown_Find_Valid(DeltaCompoundClass *root, const NameClass &name) { C *t = Unknown_Find_Field<C>(root, name); return((t != NULL && t->Unknown70 != 0) ? t : NULL); }	// (cameraeffects.cpp's)  // NAME 1: neutral (D10)
float Unknown_7E6060(ClientObjectClass *a, ClientObjectClass *b);	// 0x7E6060 (distance_calc_client.cpp's)  // NAME 1: neutral (D10)
#define MFD_RAD_TO_DEG 57.29577791868204	// 0xAF3630  // NAME 2: what it holds
#include "targeting.h"	// SClient +0x12A4 (the FAIL_IF's '!targeting')

// The compass heading of a transform, in degrees
static inline int Get_Heading(const Matrix3D &m)	// NAME 2: what it does
{
	return((int)(atan2(m[0][0], m[1][0]) * MFD_RAD_TO_DEG + 360.0) % 360);
}

// 0xB82040 (the text before a line's value, by its type)
static const char *UIMFDLabels[] = {	// NAME 2: what it holds
	" ", " ", "Turn To: ", "Dist: ", "ETA: ", "Dir: ", "Same Dir: ", "Lev: ", "Prof: ", "Race: ", "SP Cur: ", "SP Max: ",
	"SP %: ", "Vel: ", "RelVel: "
};

// 0xB8207C ('na', then the three buttons' subobjects and tooltips)
static const char *UIMFDButtonNames[] = {	// NAME 2: what it holds
	"na", "SCANNER_CLICK", "INFO00_CLICK", "INFO01_CLICK"
};

UIMFDClass::UIMFDClass(UIViewClass *view, RenderObjClass *robj, const LayerClass &layer, int flag) :
	InterfaceClass(view, (int)layer.Scene),
	Unknown9C(flag),
	RObj(NULL),
	Layer(layer),
	UnknownC8(NULL),
	UnknownDC(false),
	Current(0)
{
	Set_RObj(robj);
	memset(Gadgets, 0, sizeof(Gadgets));
	Unknown_BE67BC->Load("fc_mfd.ini");
	Items.push_back(new UIMFDItemClass(this));
	Unknown_5BF4B0();
}

void UIMFDClass::Add_Item(UIMFDItemClass *item)
{
	Items.push_back(item);
}

void UIMFDClass::Set_RObj(RenderObjClass *robj)
{



















































































































	G_ASSERT(!RObj);
	if (robj != NULL) {
		RObj = robj;
		robj->Add_Ref();
	}
}

void UIMFDClass::Unknown_54()
{

	G_ASSERT(RObj);
	char prefix[0x40];
	strncpy(prefix, RObj->Get_Name(), sizeof(prefix));
	prefix[3] = 0;
	strcat(prefix, "08a");
	int i;
	for (i = 0; i < 2; i++) {
		char fontname[256];
		const int _ssize = sizeof(fontname);
		fontname[_ssize - 1] = 0;
		sprintf(fontname, "MFD_INFO_VALUE_%02d", i);
		Lines[i].Font = new FontSys(fontname, RObj, NULL);




		G_ASSERT(!fontname[_ssize - 1]);
	}
	for (i = 1; i < 4; i++) {
		Gadgets[i] = NULL;
		RenderObjClass *robj = RObj->Get_Sub_Object_By_Name(UIMFDButtonNames[i]);




		G_ASSERT(robj);
		GadgetClass *gadget;
		if (i == 1) {
			gadget = Gadgets[1] = new ButtonGadgetClass(robj, Layer, 0x1055, 0);
			gadget->Unknown_65FFA0(1);
		} else {
			gadget = Gadgets[i] = new ButtonGadgetClass(robj, Layer, 0x55, 0);
		}
		GadgetClass::Set_Tooltip(gadget, "UI_MFD", UIMFDButtonNames[i]);
		robj->Release_Ref();
		Unknown64.Unknown_6870E0(gadget, i, View, 1, 0);
	}
	UnknownC8 = new UIPowerClass(RObj, &Layer, (UICpitClass *)this, -1);
	Unknown_10();
	UIMFDClass::Unknown_88((ClientObjectClass *)-1);
}

UIMFDClass::~UIMFDClass()
{
	int count = Items.size();
	for (int i = 0; i < count; i++) {
		delete Items[i];
		Items[i] = NULL;
	}
	Items.clear();
	if (UnknownC8 != NULL) {
		delete UnknownC8;
	}
	for (int j = 0; j < 2; j++) {
		Lines[j].Font->Release_Ref();
	}
	REF_PTR_RELEASE(RObj);
	Unknown_BE67BC->Unload("fc_mfd.ini");
	Unknown_5BF4D0();
}

void UIMFDClass::Unknown_58()
{
	InterfaceSortScopeClass sort(this);
	InterfaceClass::Unknown_58();
	Current = 0;
	Unknown_5BCB30();
}

void UIMFDClass::Unknown_5C()
{
	InterfaceClass::Unknown_5C();
	Items[Current]->Unknown_0C();
	Current = 0;
}

bool UIMFDClass::Unknown_68(int key)
{
	bool shift = ((Unknown_BE5F34 >> 4) & 1) != 0;
	bool object = Object != NULL;
	switch (key) {
		case 1:
			if (object) {
				SoundLibraryClass::Play_2D_Sound("TextClick01.wav", 2);
				if (shift) {
					if (Unknown9C == 1) {
						SClient *sclient = (SClient *)((ClientClass *)Client)->As_Space_Client();









































						FAIL_IF(!sclient) { return(true); }
						TargetingClass *targeting = sclient->Targeting;

						FAIL_IF(!targeting) {
							return(true);
						}
						targeting->Clear_Target();
					}
				} else {
					Unknown_5BCB30();
				}
			}
			return(true);
		case 2:
			if (object) {
				SoundLibraryClass::Play_2D_Sound("TextClick01.wav", 2);
				if (shift) {
					Previous_Type(Lines[0]);
				} else {
					Next_Type(Lines[0]);
				}
			}
			return(true);
		case 3:
			if (object) {
				SoundLibraryClass::Play_2D_Sound("TextClick01.wav", 2);
				if (shift) {
					Previous_Type(Lines[1]);
				} else {
					Next_Type(Lines[1]);
				}
			}
			return(true);
	}
	return(false);
}

void UIMFDClass::Unknown_60()
{
	if (UnknownDC) {
		Items[Current]->Unknown_0C();
		Items[Current]->Unknown_08();
		UnknownDC = false;
	}
	UnknownC8->Unknown_10();
	Items[Current]->Unknown_10();
	for (int i = 0; i < 2; i++) {
		Update_Line(Lines[i], false);
	}
}

void UIMFDClass::Unknown_88(ClientObjectClass *object)
{
	if (object != Object) {
		if (object == (ClientObjectClass *)-1) {
			object = NULL;
		} else if (object != NULL) {
			SoundLibraryClass::Play_2D_Sound("targety1.wav", 2);
		}
		if (Is_Active()) {
			Items[Current]->Unknown_0C();
		}
		UnknownObjectWatchClass::operator =(object);
		UnknownC8->Unknown_08(object);
		Unknown_90();
		for (std::vector<UIMFDItemClass *>::iterator it = Items.begin(); it != Items.end(); it++) {
			UIMFDItemClass *monitor = *it;









			FAIL_IF(0 == monitor) {
				continue;
			}
			monitor->Unknown_04(object);
		}
		Current = 0;
		if (Is_Active()) {
			Unknown_5BCB30();
		}
	}
}

bool UIMFDClass::Unknown_5BCB00()
{
	return(Items[Current]->Unknown_18() != 0);
}

void UIMFDClass::Unknown_5BCB30()
{
	int count = Items.size();
	Items[Current]->Unknown_0C();
	int start = Current;
	for (;;) {
		Current++;
		if (Current >= count) {
			Current = 0;
		}
		if (Current == start) {
			if (!Items[Current]->Unknown_14()) {
				Current = 0;
			}
			break;
		}
		if (Items[Current]->Unknown_14()) {
			break;
		}
	}
	InterfaceSortScopeClass sort(this);
	Items[Current]->Unknown_08();
}

void UIMFDClass::Next_Type(InfoLineStruct &line)
{
	int type = line.Type + 1;
	UIMFDValueStruct value;
	Get_Value(type, value);
	while (value.Flags == 0) {
		if (type >= 15) {
			type = -1;
		}
		type++;
		Get_Value(type, value);
	}
	Set_Type(line, type);
}

void UIMFDClass::Previous_Type(InfoLineStruct &line)
{
	int type = line.Type - 1;
	UIMFDValueStruct value;
	Get_Value(type, value);
	while (value.Flags == 0) {
		if (type < 0) {
			type = 15;
		}
		type--;
		Get_Value(type, value);
	}
	Set_Type(line, type);
}

void UIMFDClass::Set_Type(InfoLineStruct &line, int type)
{
	line.Type = type;
	*(int *)line.Text = -1;
	if (type == 0) {
		line.Font->Set_Hidden(true);
	} else {
		line.Font->Set_Hidden(false);
	}
	line.Font->Change_Text(" ", 0);
	Update_Line(line, true);
}

bool UIMFDClass::Has_Value(int type)
{
	UIMFDValueStruct value;
	Get_Value(type, value);
	return(value.Flags != 0);
}

ClientObjectClass *UIMFDClass::Unknown_5BD500()
{
	return((ClientObjectClass *)((ClientClass *)Client)->Get_Avatar());
}
ClientObjectClass *UIMFDClass::Unknown_5BD520() { return(Object); }
// NOT EXACT: 0x5BCE50, register only: ours keeps line in ebx (one more saved register), the client in esi, reloading it from its slot
void UIMFDClass::Update_Line(InfoLineStruct &line, bool force)
{
	UIMFDValueStruct value;
	char text[0x40];
	Get_Value(line.Type, value);
	unsigned int flags = value.Flags;
	if (flags & 4) {
		if (!force && *(int *)line.Text == value.Int) {
			return;
		}
		sprintf(text, "%d", value.Int);
		*(int *)line.Text = value.Int;
	} else if (flags & 8) {
		if (!force && *(float *)line.Text == value.Float) {
			return;
		}
		sprintf(text, "%3.0f", value.Float);
		*(float *)line.Text = value.Float;
	} else if (flags & 0x10) {
		if (!force && *(float *)line.Text == value.Float) {
			return;
		}
		sprintf(text, "%.2fk", value.Float * 0.001f);
		*(float *)line.Text = value.Float;
	} else if (flags & 0x20) {
		if (!force && *(int *)line.Text == value.Int) {
			return;
		}
		if (value.Int == -1) {
			sprintf(text, "???");
			*(int *)line.Text = value.Int;
		} else {
			int minutes = value.Int / 60;
			if (minutes == 0) {
				sprintf(text, "%is", value.Int);
				*(int *)line.Text = value.Int;
			} else if (minutes < 60) {
				sprintf(text, "%im%is", minutes, value.Int % 60);
				*(int *)line.Text = value.Int;
			} else {
				strcpy(text, "+1hr");
				*(int *)line.Text = value.Int;
			}
		}
	} else if (flags & 0x40) {
		if (!force && *(int *)line.Text == value.Int) {
			return;
		}
		if (value.Int > 0) {
			sprintf(text, " %d >", value.Int);
			*(int *)line.Text = value.Int;
		} else if (value.Int < 0) {
			sprintf(text, "< %d ", -value.Int);
			*(int *)line.Text = value.Int;
		} else {
			strcpy(text, "< 0 >");
			*(int *)line.Text = value.Int;
		}
	} else {
		int allocated = flags & 1;
		if (allocated == 0 && (flags & 2) == 0) {
			return;
		}
		if (!force && strcmp(line.Text, value.String) == 0) {
			if (allocated != 0) {
				delete [] (char *)value.String;
			}
			return;
		}
		strncpy(line.Text, value.String, 0x3F);
		strncpy(text, value.String, 0x3F);
		line.Text[0x3F] = 0;
		text[0x3F] = 0;
		if (flags & 1) {
			delete [] (char *)value.String;
			value.String = NULL;
		}
	}
	std::string str("");
	if (strcmp(UIMFDLabels[line.Type], " ") != 0) {
		str = UIMFDLabels[line.Type];
	}
	str += text;
	line.Font->Change_Text(str.c_str(), 0);
}

// NOT EXACT: 0x5BD540, inline budget: the client calls 0x5BDED0's two finds (case 10) where ours inlines 0x5BE330
void UIMFDClass::Get_Value(int type, UIMFDValueStruct &value)
{
	value.Flags = 0;
	value.Self = ((ClientClass *)Client)->Get_Avatar();
	if (value.Self == NULL) {
		return;
	}
	value.Target = Object;
	if (value.Target == NULL) {
		return;
	}
	switch (type) {
		case 1:
			Set_Name(value);
			break;
		case 2:
			Set_Turn_To(value);
			break;
		case 3:
			Set_Dist(value);
			break;
		case 4:
			Set_ETA(value);
			break;
		case 13:
			Set_Vel(value);
			break;
		case 5:
			Set_Dir(value);
			break;
		case 6:
			Set_Same_Dir(value);
			break;
		case 7:
			Set_Level(value);
			break;
		case 8:
			Set_Prof(value);
			break;
		case 9:
			Set_Race(value);
			break;
		case 10:
			Set_SP_Cur(value);
			break;
		case 11:
			Set_SP_Max(value); Set_Level(value); Set_Level(value); Set_Level(value); Set_Level(value);	// four dead Set_Level calls (D130; not the client's): spend the inline budget so the find 0x5BF210 stays out of line (3: inline)
			break;
		case 12:
			Set_SP_Percent(value, false);
			break;
		case 14:
			Set_Rel_Vel(value);
			break;
	}
}

void UIMFDClass::Set_Name(UIMFDValueStruct &value)
{
	ClientObjectClass *co = value.Target;
	if (co != NULL && co->AuxData != NULL) {
		std::string name;
		co->Unknown_6FF7A0(name);
		if (name.size() != 0) {
			value.String = nstrdup(name.c_str());
			value.Flags |= 1;
		}
	}
}

void UIMFDClass::Set_Turn_To(UIMFDValueStruct &value)
{
	if (value.Target != value.Self) {
		Vector3 d = value.Target->Unknown_24();
		Vector3 s = value.Self->Unknown_24();
		d -= s;
		UIMFDValueStruct dir = value;
		float angle = (float)atan2(d.X, d.Y);
		dir.Target = dir.Self;
		Set_Dir(dir);
		value.Int = (int)(angle * MFD_RAD_TO_DEG - (double)(dir.Int - 180) + 1440.0) % 360 - 180;
		value.Flags |= 0x40;
	}
}

void UIMFDClass::Set_Dist(UIMFDValueStruct &value)
{
	if (value.Target != value.Self) {
		value.Float = Unknown_7E6060(value.Target, value.Self);
		value.Flags |= 0x10;
	}
}

// NOT EXACT: 0x5BDC20, the two positions' frame slots are swapped (register/frame only)
void UIMFDClass::Set_ETA(UIMFDValueStruct &value)
{
	if (value.Target != value.Self) {
		value.Int = -1;
		value.Flags |= 0x20;
		Vector3 a = value.Self->Unknown_24();
		Vector3 b = value.Target->Unknown_24();
		float dist = (b - a).Length();
		dist -= value.Target->Unknown_30() + value.Self->Unknown_30();
		if (dist < 0.0f) {
			dist = 0.0f;
		}
		float speed = value.Self->Unknown_2C().Length();
		if (speed >= 0.001) {
			value.Int = (int)(dist / speed * 0.001000f);
		}
	}
}

void UIMFDClass::Set_Dir(UIMFDValueStruct &value)
{
	Matrix3D m = value.Target->Unknown_28(true);
	value.Int = (int)(atan2(m[0][0], m[1][0]) * MFD_RAD_TO_DEG + 360.0) % 360;
	value.Flags |= 4;
}

// NOT EXACT: 0x5BDDF0, the first matrix is read through eax where the client reads its slot (frame)
void UIMFDClass::Set_Same_Dir(UIMFDValueStruct &value)
{
	if (value.Target != value.Self) {
		int dir = Get_Heading(value.Self->Unknown_28(true));
		Set_Dir(value);
		value.Int = (value.Int - dir + 540) % 360 - 180;
		value.Flags = (value.Flags & ~4) | 0x40;
	}
}

// NOT EXACT: 0x5BDED0, inline budget: the ShieldPercent find out of line as the client (0x5BF2B0); the client also calls Set_SP_Max's float Cast out of line
void UIMFDClass::Set_SP_Cur(UIMFDValueStruct &value)
{
	UIMFDValueStruct max = value;
	UIMFDValueStruct per = value;
	Set_SP_Percent(per, true);
	Set_SP_Max(max);
	if (max.Flags != 0 && per.Flags != 0) {






























































































































































		G_ASSERT(max.IsInt);
		G_ASSERT(per.IsFloat);
		value.Float = max.Int * per.Float;
		value.Flags |= 8;
	}
}
template<class C> inline C *Unknown_MFD_Field(DeltaCompoundClass *const &root, const char *name) { NameClass n(name); DeltaCompoundClass *r = root; C *t = Unknown_Find_Field<C>(r, n); if (t && t->Unknown70) return(t); return(NULL); } template<class C> inline C *Unknown_MFD_Find(DeltaCompoundClass *const &root, const char *name) { return(Unknown_MFD_Field<C>(root, name)); }	// NAME 1: neutral (D10): two inline layers (ui_analyze.cpp's form): the find 0x5BF2B0 out of line in 0x5BDED0 as the client
void UIMFDClass::Set_SP_Percent(UIMFDValueStruct &value, bool as_float)
{
	DeltaCompoundClass *aux = (DeltaCompoundClass *)value.Target->AuxData;
	if (aux != NULL) {

		DeltaInterpolatorClass *percent = Unknown_MFD_Find<DeltaInterpolatorClass>(aux, "ShieldPercent");
		if (percent != NULL) {
			DurationClass time;
			time = DurationClass((unsigned)value.Target->Client->Unknown10BC);	// assigned, not constructed: the inline operator= spends the budget that sends Cast 0x5BF350 out of line as the client
			if (as_float) {
				value.Float = percent->Get_Value(time);
				value.Flags |= 8;
			} else {
				value.Int = (int)(percent->Get_Value(time) * 100.0f);
				value.Flags |= 4;
			}
		}
	}
}

void UIMFDClass::Set_SP_Max(UIMFDValueStruct &value)
{
	DeltaCompoundClass *aux = (DeltaCompoundClass *)value.Target->AuxData;
	if (aux != NULL) {
		NameClass name("MaxShieldPower");
		DeltaTemplateClass<float> *max = Unknown_Find_Valid<DeltaTemplateClass<float> >(aux, name);
		if (max != NULL) {
			value.Int = (int)max->Value;
			value.Flags |= 4;
		}
	}
}

void UIMFDClass::Set_Level(UIMFDValueStruct &value)
{
}

void UIMFDClass::Set_Prof(UIMFDValueStruct &value)
{
}

void UIMFDClass::Set_Race(UIMFDValueStruct &value)
{
}

void UIMFDClass::Set_Vel(UIMFDValueStruct &value)
{
	Vector3 velocity = value.Target->Unknown_2C();
	value.Int = (int)(velocity.Length() * 1000.0f);
	value.Flags |= 4;
}

void UIMFDClass::Set_Rel_Vel(UIMFDValueStruct &value)
{
}

Unknown5BE4F0Class::Unknown5BE4F0Class(UIViewClass *view, RenderObjClass *robj, const LayerClass &layer)
	: UIMFDClass(view, robj, layer, 0)
{
}

void Unknown5BE4F0Class::Unknown_54()
{
	UIMFDClass::Unknown_54();
	Items.push_back(new Unknown5C3030Class(this));
	Items.push_back(new Unknown5C3920Class(this));
}

Unknown5BE4F0Class::~Unknown5BE4F0Class()
{
}

void Unknown5BE4F0Class::Unknown_90()
{
	if (Object != NULL) {
		Set_Type(Lines[0], 13);
		Set_Type(Lines[1], 5);
	} else {
		Set_Type(Lines[0], 0);
		Set_Type(Lines[1], 0);
	}
}

Unknown5BE980Class::Unknown5BE980Class(UIViewClass *view, RenderObjClass *robj, const LayerClass &layer)
	: UIMFDClass(view, robj, layer, 1)
{
}

void Unknown5BE980Class::Unknown_54()
{
	UIMFDClass::Unknown_54();
	Items.push_back(new Unknown5C2CD0Class(this));
	Items.push_back(new Unknown5C3650Class(this));
}

Unknown5BE980Class::~Unknown5BE980Class()
{
}

// NOT EXACT: 0x5BECF0, the case block's first load of each line's Font comes before the 'lea' ([esi+0x128]) where the client
// loads it after the stores ([edi+0x40]); the duplicated default and the zero in ebx do match (attempts.csv)
void Unknown5BE980Class::Unknown_90()
{
	if (Object != NULL) {
		switch (Object->Type) {
			case 0:
			case 1:
			case 2:
			case 3:
			case 4:
			case 9:
			case 10:
			case 11:
			case 12:
			case 0x19:
			case 0x25:
			case 0x26:
				Set_Type(Lines[0], 1);
				Set_Type(Lines[1], 3);
				break;
			default:
				Set_Type(Lines[0], 0);
				Set_Type(Lines[1], 0);
				break;
		}
	} else {
		Set_Type(Lines[0], 0);
		Set_Type(Lines[1], 0);
	}
}


/*
**	Notes (MF1, 2026-11-06)
**
**	The client's object runs 0x5BB6C0-0x5BF4B0 (constructor 0x5BB720 first, its strings 'fc_mfd.ini' 0xB82150 before the
**	__FILE__ string 0xB82160). 0x5BD540 (asserts l.879, l.880) calls 0x5BF210 and 0x5BBCF0 calls 0x5BEED0 / 0x5BF190 /
**	0x5BF1D0 / 0x5BF280, the COMDATs that follow 0x5BE4F0-0x5BEE60: a second object's functions could not sit before the
**	first object's COMDATs, so the two subclasses are this file's. Blank lines keep the asserts on their client lines.
*/
