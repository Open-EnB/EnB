// Earth & Beyond client (reconstruction): app\common\item_info.cpp
// Client 0x83E7C0-0x842930: an item's info (its typed fields and two effect descriptions). Notes at the end of the file.
#include "item_info.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "..\packets\pk.h"
#include "..\shared\g_assert.h"

void Unknown_4FC2F0(std::vector<std::string> &list, const std::string &string, const std::string &separator);	// 0x4FC2F0: splits string at separator  // NAME 1: neutral placeholder (D10)

// What a field's value is
enum FieldTypeEnum  // NAME 2: the client's FT_NONE
{
	FT_NONE = 0,	// the client's G_ASSERT 'FieldTypes[lp] != FT_NONE'
	FT_INT = 1,		// NAME 2: '_ItemField.Is_Int'
	FT_FLOAT = 2,	// NAME 2: '_ItemField.Is_Float'
	FT_STRING = 4	// NAME 2: '_ItemField.Is_String'
};

// Each field's type, by its ItemInfoEnum value (0xBF34E0; the static initialiser 0x83F320 fills it)
class ItemFieldClass  // NAME 2: the client's '_ItemField'
{
public:
	ItemFieldClass();

	bool Is_Int(int iteminfo) const { return(FieldTypes[iteminfo] == FT_INT); }
	bool Is_Float(int iteminfo) const { return(FieldTypes[iteminfo] == FT_FLOAT); }
	bool Is_String(int iteminfo) const { return(FieldTypes[iteminfo] == FT_STRING); }

	FieldTypeEnum FieldTypes[NUM_ITEM_INFO];	// the client's 'FieldTypes[lp]'
};

inline ItemFieldClass::ItemFieldClass()
{
	memset(FieldTypes, 0, sizeof(FieldTypes));
	FieldTypes[0] = FT_FLOAT;
	FieldTypes[1] = FT_STRING;
	FieldTypes[2] = FT_INT;
	FieldTypes[3] = FT_INT;
	FieldTypes[4] = FT_INT;
	FieldTypes[5] = FT_INT;
	FieldTypes[6] = FT_INT;
	FieldTypes[7] = FT_INT;
	FieldTypes[8] = FT_INT;
	FieldTypes[9] = FT_FLOAT;
	FieldTypes[10] = FT_FLOAT;
	FieldTypes[11] = FT_STRING;
	FieldTypes[12] = FT_INT;
	FieldTypes[13] = FT_STRING;
	FieldTypes[14] = FT_INT;
	FieldTypes[15] = FT_INT;
	FieldTypes[16] = FT_INT;
	FieldTypes[17] = FT_INT;
	FieldTypes[18] = FT_INT;
	FieldTypes[19] = FT_INT;
	FieldTypes[20] = FT_FLOAT;
	FieldTypes[21] = FT_FLOAT;
	FieldTypes[22] = FT_INT;
	FieldTypes[23] = FT_FLOAT;
	FieldTypes[24] = FT_INT;
	FieldTypes[25] = FT_FLOAT;
	FieldTypes[26] = FT_FLOAT;
	FieldTypes[27] = FT_STRING;
	FieldTypes[28] = FT_INT;
	FieldTypes[29] = FT_INT;
	FieldTypes[30] = FT_INT;
	FieldTypes[31] = FT_INT;
	FieldTypes[32] = FT_INT;
	FieldTypes[33] = FT_INT;
	FieldTypes[34] = FT_FLOAT;
	FieldTypes[35] = FT_INT;
	FieldTypes[36] = FT_FLOAT;
	FieldTypes[37] = FT_FLOAT;

	for (int lp = 0; lp < NUM_ITEM_INFO; lp++) {


























		G_ASSERT(FieldTypes[lp] != FT_NONE);
	}
}

static ItemFieldClass _ItemField;	// 0xBF34E0

static const char *DeltaSeparator = "^";	// 0xBAE1FC (between the fields of a delta string)  // NAME 1: what it is

// NOT EXACT: 0x83F490 (0.86, order-only: the same 39 instructions; the client takes EnumMapping's address before the count and fill of the memset, ours after)
ItemBaseInfoClass::ItemBaseInfoClass()
{
	memset(EnumMapping, 0xFF, sizeof(EnumMapping));
}

// 0x83F580 (the scalar deleting destructor 0x83F550)
ItemBaseInfoClass::~ItemBaseInfoClass()
{
	Reset();
}

// NOT EXACT: 0x83F710 (0.88, order-only: the same 44 instructions, as 0x83F490)
ItemBaseInfoClass::ItemBaseInfoClass(const ItemBaseInfoClass &that)
{
	memset(EnumMapping, 0xFF, sizeof(EnumMapping));
	Merge(that);
}

// 0x83F7E0
ItemBaseInfoClass &ItemBaseInfoClass::operator=(const ItemBaseInfoClass &that)
{
	Reset();
	Merge(that);
	return(*this);
}

// 0x83F8A0
void ItemBaseInfoClass::Reset()
{
	Reset_Fields();
	Reset_Effects();
}

// 0x83F940: the strings are the info's own
void ItemBaseInfoClass::Reset_Fields()
{
	for (std::vector<ItemFieldStruct>::iterator field = Fields.begin(); field != Fields.end(); ++field) {
		if (_ItemField.Is_String(field->ItemInfo)) {
			delete [] field->String;
			field->String = NULL;
		}
	}
	Fields.clear();
}

// 0x83F9E0
void ItemBaseInfoClass::Reset_Effects()
{
	ActivatedEffects.Clear_Effects();
	EquipEffects.Clear_Effects();
}

// 0x83FA10: every field of that which has a value replaces ours; the effects are copied
void ItemBaseInfoClass::Merge(const ItemBaseInfoClass &that)
{
	for (std::vector<ItemFieldStruct>::const_iterator field = that.Fields.begin(); field != that.Fields.end(); ++field) {
		if (_ItemField.Is_String(field->ItemInfo)) {
			Set_String(field->ItemInfo, field->String);
		} else if (_ItemField.Is_Float(field->ItemInfo)) {
			Set_Float(field->ItemInfo, field->Float, false);
		} else if (_ItemField.Is_Int(field->ItemInfo)) {
			Set_Int(field->ItemInfo, field->Int, false);
		} else {
































			G_ASSERT(0);
		}
	}
	ActivatedEffects = that.ActivatedEffects;
	EquipEffects = that.EquipEffects;
}

// 0x83FD60
bool ItemBaseInfoClass::Has_Field(int iteminfo)
{
	if (EnumMapping[iteminfo] == 0xFF) {
		return(false);
	}



	FAIL_IF(EnumMapping[iteminfo] >= Fields.size()) return(false);
	return(true);
}

// 0x83FDE0
int ItemBaseInfoClass::Get_Enum_Mapping(int iteminfo)
{
	return(EnumMapping[iteminfo]);
}

// 0x83FE00
unsigned int ItemBaseInfoClass::Get_Field_Count()
{
	return(Fields.size());
}

// 0x83FE30: the count byte, then per field its id and value (a string: a 2-byte length and the bytes with the 0)
int ItemBaseInfoClass::Get_Size()
{
	int size = sizeof(unsigned char);
	for (std::vector<ItemFieldStruct>::iterator field = Fields.begin(); field != Fields.end(); ++field) {
		size += sizeof(int);
		if (_ItemField.Is_Int(field->ItemInfo)) {
			size += sizeof(int);
		} else if (_ItemField.Is_Float(field->ItemInfo)) {
			size += sizeof(float);
		} else if (_ItemField.Is_String(field->ItemInfo)) {
			size += sizeof(short) + strlen(field->String) + 1;
		} else {








			G_ASSERT(false);
		}
	}
	size += ActivatedEffects.Get_Size();
	size += EquipEffects.Get_Size();
	return(size);
}

// 0x83FEF0
void ItemBaseInfoClass::Write(RawPacketClass *raw)
{
	unsigned char count = Fields.size();
	raw->Add(&count, sizeof(count));
	for (std::vector<ItemFieldStruct>::iterator field = Fields.begin(); field != Fields.end(); ++field) {
		raw->Add(&field->ItemInfo, sizeof(field->ItemInfo));
		if (_ItemField.Is_Int(field->ItemInfo)) {
			raw->Add(&field->Int, sizeof(field->Int));
		} else if (_ItemField.Is_Float(field->ItemInfo)) {
			raw->Add(&field->Float, sizeof(field->Float));
		} else if (_ItemField.Is_String(field->ItemInfo)) {
			short length = strlen(field->String) + 1;
			raw->Add(&length, sizeof(length));
			raw->Add(field->String, length);
		} else {







			G_ASSERT(false);
		}
	}
	ActivatedEffects.Write(raw);
	EquipEffects.Write(raw);
}

// 0x840030
void ItemBaseInfoClass::Read(RawPacketClass *raw)
{
	unsigned char count = 0;
	raw->Get(&count, sizeof(count));
	Fields.reserve(count);
	for (int i = 0; i < count; i++) {
		ItemFieldStruct field;
		raw->Get(&field.ItemInfo, sizeof(field.ItemInfo));
		if (_ItemField.Is_Int(field.ItemInfo)) {
			raw->Get(&field.Int, sizeof(field.Int));
		} else if (_ItemField.Is_Float(field.ItemInfo)) {
			raw->Get(&field.Float, sizeof(field.Float));
		} else if (_ItemField.Is_String(field.ItemInfo)) {
			short length;
			raw->Get(&length, sizeof(length));
			field.String = new char[length];
			raw->Get(field.String, length);
		} else {






			G_ASSERT(false);
		}
		Fields.push_back(field);
		EnumMapping[field.ItemInfo] = i;
	}
	ActivatedEffects.Read(raw);
	EquipEffects.Read(raw);
}

void ItemBaseInfoClass::Set_Int(int iteminfo, int val, bool force)	// 0x840420: a zero is not stored unless forced
{
	if (val == 0 && !force) {
		return;
	}

	FAIL_IF(!_ItemField.Is_Int(iteminfo)) return;
	Add_Field(iteminfo)->Int = val;
}

// 0x840540
void ItemBaseInfoClass::Set_Float(int iteminfo, float val, bool force)
{
	if (val > -0.0001f && val < 0.0001f && !force) {
		return;
	}
	FAIL_IF(!_ItemField.Is_Float(iteminfo)) return;
	Add_Field(iteminfo)->Float = val;
}

// 0x840670: an empty string is not stored; the field gets its own copy
void ItemBaseInfoClass::Set_String(int iteminfo, const char *val)
{
	if (val == NULL || *val == '\0') {
		return;
	}
	FAIL_IF(!_ItemField.Is_String(iteminfo)) return;
	ItemFieldStruct *field = Add_Field(iteminfo);
	field->String = new char[strlen(val) + 1];
	strcpy(field->String, val);
}

// 0x8407D0
void ItemBaseInfoClass::Set_String(int iteminfo, const std::string &val)
{
	Set_String(iteminfo, val.c_str());
}

int ItemBaseInfoClass::Get_Field_Info(unsigned int index)	// 0x840940
{
	if (index >= Fields.size()) {
		return(ITEM_INFO_NONE);
	}
	return(Fields[index].ItemInfo);
}

bool ItemBaseInfoClass::Get_String(std::string &val, unsigned int index)	// 0x840980
{
	if (index >= Fields.size()) {
		return(false);
	}
	return(Field_To_String(&Fields[index], val));
}

bool ItemBaseInfoClass::Get_String(int iteminfo, std::string &val)	// 0x8409D0
{
	return(Field_To_String(Find_Field(iteminfo), val));
}

bool ItemBaseInfoClass::Get_Int(int iteminfo, int *val)	// 0x840A20
{
	FAIL_IF(!val) return(false);
	const ItemFieldStruct *field = Find_Field(iteminfo);
	if (field == NULL) {
		return(false);
	}
	FAIL_IF(!_ItemField.Is_Int(field->ItemInfo)) return(false);
	*val = field->Int;
	return(true);
}

// 0x840AC0
bool ItemBaseInfoClass::Get_Float(int iteminfo, float *val)
{
	FAIL_IF(!val) return(false);
	const ItemFieldStruct *field = Find_Field(iteminfo);
	if (field == NULL) {
		return(false);
	}
	FAIL_IF(!_ItemField.Is_Float(field->ItemInfo)) return(false);
	*val = field->Float;
	return(true);
}

// 0x840B60
void ItemBaseInfoClass::Set_Activated_Effects(const ItemEffectDescriptionClass &effects)
{
	ActivatedEffects = effects;
}

// 0x840DB0
void ItemBaseInfoClass::Set_Equip_Effects(const ItemEffectDescriptionClass &effects)
{
	EquipEffects = effects;
}

// 0x841000: the field, added with a zero value when the info has none
ItemFieldStruct *ItemBaseInfoClass::Add_Field(int iteminfo)
{
	if (!Has_Field(iteminfo)) {
		ItemFieldStruct field;
		field.ItemInfo = iteminfo;
		field.Int = 0;
		EnumMapping[iteminfo] = Fields.size();
		Fields.push_back(field);
	}
	return(&Fields[EnumMapping[iteminfo]]);
}

// 0x8410D0
ItemFieldStruct *ItemBaseInfoClass::Find_Field(int iteminfo)
{
	int index = Get_Enum_Mapping(iteminfo);
	if (index == 0xFF) {
		return(NULL);
	}
	return(&Fields[index]);
}

// 0x841100: the field's value as text (field 19, a float, in thousands: '%.2fk'); false and an empty val without a field
bool ItemBaseInfoClass::Field_To_String(const ItemFieldStruct *field, std::string &val)
{
	if (field == NULL) {
		val.erase();
		return(false);
	}
	char buffer[32];
	if (_ItemField.Is_Int(field->ItemInfo)) {
		sprintf(buffer, "%d", field->Int);
		val = buffer;
	} else if (_ItemField.Is_Float(field->ItemInfo)) {
		if (field->ItemInfo != 19) {
			sprintf(buffer, "%.2f", field->Float);
		} else {
			sprintf(buffer, "%.2fk", field->Float * 0.001f);
		}
		val = buffer;
	} else if (_ItemField.Is_String(field->ItemInfo)) {
		val = field->String;
	} else {








		G_ASSERT(false);
		val.erase();
		return(false);
	}
	return(true);
}

// 0x841410: 'id:value' for every field, each followed by DeltaSeparator (what Apply_Delta reads)
std::string ItemBaseInfoClass::Get_Delta()
{
	std::string delta;
	for (std::vector<ItemFieldStruct>::iterator field = Fields.begin(); field != Fields.end(); ++field) {
		if (field->ItemInfo != ITEM_INFO_NONE) {
			char id[64];
			_snprintf(id, sizeof(id), "%d:", field->ItemInfo);
			std::string text = id;
			char value[64];
			if (_ItemField.Is_String(field->ItemInfo)) {
				text += field->String;
			} else if (_ItemField.Is_Float(field->ItemInfo)) {
				_snprintf(value, sizeof(value), "%1.2f", field->Float);
				text += value;
			} else if (_ItemField.Is_Int(field->ItemInfo)) {
				_snprintf(value, sizeof(value), "%d", field->Int);
				text += value;
			} else {
















				G_ASSERT(0);
			}
			delta += text + DeltaSeparator;
		}
	}
	return(delta);
}

// NOT EXACT: 0x8417B0 (0.78, the same 287 instructions but 4: the client keeps field_separator and takes field_separator + 1 into another register for the second substr)
// Replaces the fields by those of a delta string ('id:value' pieces between DeltaSeparator)
void ItemBaseInfoClass::Apply_Delta(const std::string &delta)
{
	Reset_Fields();
	std::vector<std::string> fields;
	Unknown_4FC2F0(fields, delta, std::string(DeltaSeparator));
	for (std::vector<std::string>::iterator it = fields.begin(); it != fields.end(); ++it) {
		int field_separator = it->find(':');




		FAIL_IF(field_separator == std::string::npos) continue;
		std::string id = it->substr(0, field_separator);
		int item_info = atoi(id.c_str());









		FAIL_IF(item_info <= ITEM_INFO_NONE) {
			continue;
		}

		FAIL_IF(item_info >= NUM_ITEM_INFO) {
			continue;
		}
		std::string value = it->substr(field_separator + 1);
		if (_ItemField.Is_String(item_info)) {
			Set_String(item_info, value.c_str());
			continue;
		}
		if (_ItemField.Is_Float(item_info)) {
			Set_Float(item_info, atof(value.c_str()), false);
			continue;
		}
		if (_ItemField.Is_Int(item_info)) {
			Set_Int(item_info, atoi(value.c_str()), false);
		}
	}
}

/*
**	Reconstruction notes
**
**	The object: std::vector<ItemEffectClass>'s and std::vector<ItemFieldStruct>'s instances (0x83E7C0-0x83F2B0 and
**	0x841C50-0x8428D0), <string>'s guarded static 0x83F2F0, _ItemField's static initialiser 0x83F320 (its G_ASSERT on
**	line 103), then ItemBaseInfoClass in source order 0x83F490-0x8417B0. The names Fields, EnumMapping, ItemInfo,
**	_ItemField, FieldTypes, FT_NONE, lp, iteminfo, val, field, field_separator, item_info, ITEM_INFO_NONE and
**	NUM_ITEM_INFO are the asserts'. The class name is the tree's (D219).
*/
