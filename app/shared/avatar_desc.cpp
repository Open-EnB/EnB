/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**	Original location: e:\build\beta\G\app\shared\avatar_desc.cpp (client 0x4BEB50-0x4C45B0)
**	AvatarDescClass (avatar_desc.h), partly rebuilt: the constructors, the wire and key/value slots,
**	Clear. The asserted loader 0x4C05D0 and the rest of the window are not yet (notes at the end).
*/
#include "avatar_desc.h"
#include "auxdata\linearbu.h"
#include "kv_encoder_base.h"
#include <stdio.h>
#include "unknown_debug.h"
#include "..\..\wwlib\ini.h"
#include "..\..\wwlib\point.h"

StringValueClass *Unknown_4C3110(float *values, int count, char separator);	// NAME 1 [Unknown_4C3110]: 0x4C3110, the values as one text value (body_weight, head_weight)

// 0x4BEB50
AvatarDescClass::AvatarDescClass()
{
	Clear(0);
	Load_Defaults(0, 0);
}

// 0x4BEBF0
AvatarDescClass::AvatarDescClass(int type, int unknown, int unknown2)
{
	Clear(0);
	Race = type;
	Profession = unknown;
	Gender = unknown2;
	Avatar_Version = 2;
	Load_Defaults(0, 0);
}

// 0x4BECC0
AvatarDescClass::AvatarDescClass(int type, const char *name)
{
	Clear(type);
	Avatar_Version = 2;
	Load_Defaults(type, name);
}

// 0x4BF260
bool AvatarDescClass::Write(LinearBufferClass *buffer)
{
	buffer->Write(First_Name, sizeof(First_Name));
	buffer->Write(Last_Name, sizeof(Last_Name));
	buffer->Write(&Avatar_Type, 4);
	buffer->Write(&Unknown0C, 1);
	buffer->Write(&Avatar_Version, 1);
	buffer->Write(&Race, 4);
	buffer->Write(&Profession, 4);
	buffer->Write(&Gender, 4);
	buffer->Write(&Mood_Type, 4);
	buffer->Write(&Personality, 1);
	buffer->Write(&NLP, 1);
	buffer->Write(&Body_Type, 1);
	buffer->Write(&Pants_Type, 1);
	buffer->Write(&Head_Type, 1);
	buffer->Write(&Hair_Num, 1);
	buffer->Write(&Ear_Num, 1);
	buffer->Write(&Goggle_Num, 1);
	buffer->Write(&Beard_Num, 1);
	buffer->Write(&Weapon_Hip_Num, 1);
	buffer->Write(&Weapon_Unique_Num, 1);
	buffer->Write(&Weapon_Back_Num, 1);
	buffer->Write(&Head_Texture_Num, 1);
	buffer->Write(&Tattoo_Texture_Num, 1);
	buffer->Write(&Tattoo_Offset, 12);
	buffer->Write(&Hair_Color, 12);
	buffer->Write(&Beard_Color, 12);
	buffer->Write(&Eye_Color, 12);
	buffer->Write(&Skin_Color, 12);
	buffer->Write(&Shirt_Primary_Color, 12);
	buffer->Write(&Shirt_Secondary_Color, 12);
	buffer->Write(&Pants_Primary_Color, 12);
	buffer->Write(&Pants_Secondary_Color, 12);
	buffer->Write(&Shirt_Primary_Metal, 4);
	buffer->Write(&Shirt_Secondary_Metal, 4);
	buffer->Write(&Pants_Primary_Metal, 4);
	buffer->Write(&Pants_Secondary_Metal, 4);
	buffer->Write_String(Skeleton.c_str());
	int i;
	for (i = 0; i < 5; i++) {
		buffer->Write(&Body_Weight[i], 4);
	}
	for (i = 0; i < 5; i++) {
		buffer->Write(&Head_Weight[i], 4);
	}
	return(true);
}

// NOT EXACT: inline budget (the client calls the string assignment's max_size, _Tidy and _Eos out of line, ours inlines them; six forms tried: dead size()/length() after it, assign(), assign(s, strlen), the Read_String call hoisted; attempts.py show 0x4BF550)
// 0x4BF550
bool AvatarDescClass::Read(LinearBufferClass *buffer)
{
	buffer->Read(First_Name, sizeof(First_Name));
	buffer->Read(Last_Name, sizeof(Last_Name));
	buffer->Read(&Avatar_Type, 4);
	buffer->Read(&Unknown0C, 1);
	buffer->Read(&Avatar_Version, 1);
	buffer->Read(&Race, 4);
	buffer->Read(&Profession, 4);
	buffer->Read(&Gender, 4);
	buffer->Read(&Mood_Type, 4);
	buffer->Read(&Personality, 1);
	buffer->Read(&NLP, 1);
	buffer->Read(&Body_Type, 1);
	buffer->Read(&Pants_Type, 1);
	buffer->Read(&Head_Type, 1);
	buffer->Read(&Hair_Num, 1);
	buffer->Read(&Ear_Num, 1);
	buffer->Read(&Goggle_Num, 1);
	buffer->Read(&Beard_Num, 1);
	buffer->Read(&Weapon_Hip_Num, 1);
	buffer->Read(&Weapon_Unique_Num, 1);
	buffer->Read(&Weapon_Back_Num, 1);
	buffer->Read(&Head_Texture_Num, 1);
	buffer->Read(&Tattoo_Texture_Num, 1);
	buffer->Read(&Tattoo_Offset, 12);
	buffer->Read(&Hair_Color, 12);
	buffer->Read(&Beard_Color, 12);
	buffer->Read(&Eye_Color, 12);
	buffer->Read(&Skin_Color, 12);
	buffer->Read(&Shirt_Primary_Color, 12);
	buffer->Read(&Shirt_Secondary_Color, 12);
	buffer->Read(&Pants_Primary_Color, 12);
	buffer->Read(&Pants_Secondary_Color, 12);
	buffer->Read(&Shirt_Primary_Metal, 4);
	buffer->Read(&Shirt_Secondary_Metal, 4);
	buffer->Read(&Pants_Primary_Metal, 4);
	buffer->Read(&Pants_Secondary_Metal, 4);
	char *text = buffer->Read_String();
	Skeleton = text;
	delete text;
	int i;
	for (i = 0; i < 5; i++) {
		buffer->Read(&Body_Weight[i], 4);
	}
	for (i = 0; i < 5; i++) {
		buffer->Read(&Head_Weight[i], 4);
	}
	return(true);
}

// 0x4BF910
int AvatarDescClass::Get_Size()
{
	return(Skeleton.length() + 0xF1);
}

// 0x4BF930
bool AvatarDescClass::Write_Short(LinearBufferClass *buffer)
{
	buffer->Write(First_Name, sizeof(First_Name));
	buffer->Write(Last_Name, sizeof(Last_Name));
	buffer->Write(&Avatar_Type, 4);
	buffer->Write(&Avatar_Version, 1);
	buffer->Write(&Race, 4);
	buffer->Write(&Profession, 4);
	buffer->Write(&Gender, 4);
	buffer->Write(&Mood_Type, 4);
	buffer->Write(&Personality, 1);
	buffer->Write(&NLP, 1);
	buffer->Write(&Body_Type, 1);
	buffer->Write(&Pants_Type, 1);
	buffer->Write(&Head_Type, 1);
	buffer->Write(&Hair_Num, 1);
	buffer->Write(&Ear_Num, 1);
	buffer->Write(&Goggle_Num, 1);
	buffer->Write(&Beard_Num, 1);
	buffer->Write(&Weapon_Hip_Num, 1);
	buffer->Write(&Weapon_Unique_Num, 1);
	buffer->Write(&Weapon_Back_Num, 1);
	buffer->Write(&Head_Texture_Num, 1);
	buffer->Write(&Tattoo_Texture_Num, 1);
	buffer->Write(&Tattoo_Offset, 12);
	buffer->Write(&Hair_Color, 12);
	buffer->Write(&Beard_Color, 12);
	buffer->Write(&Eye_Color, 12);
	buffer->Write(&Skin_Color, 12);
	buffer->Write(&Shirt_Primary_Color, 12);
	buffer->Write(&Shirt_Secondary_Color, 12);
	buffer->Write(&Pants_Primary_Color, 12);
	buffer->Write(&Pants_Secondary_Color, 12);
	buffer->Write(&Shirt_Primary_Metal, 4);
	buffer->Write(&Shirt_Secondary_Metal, 4);
	buffer->Write(&Pants_Primary_Metal, 4);
	buffer->Write(&Pants_Secondary_Metal, 4);
	int i;
	for (i = 0; i < 5; i++) {
		buffer->Write(&Body_Weight[i], 4);
	}
	for (i = 0; i < 5; i++) {
		buffer->Write(&Head_Weight[i], 4);
	}
	return(true);
}

// 0x4BFBF0
bool AvatarDescClass::Read_Short(LinearBufferClass *buffer)
{
	buffer->Read(First_Name, sizeof(First_Name));
	buffer->Read(Last_Name, sizeof(Last_Name));
	buffer->Read(&Avatar_Type, 4);
	buffer->Read(&Avatar_Version, 1);
	buffer->Read(&Race, 4);
	buffer->Read(&Profession, 4);
	buffer->Read(&Gender, 4);
	buffer->Read(&Mood_Type, 4);
	buffer->Read(&Personality, 1);
	buffer->Read(&NLP, 1);
	buffer->Read(&Body_Type, 1);
	buffer->Read(&Pants_Type, 1);
	buffer->Read(&Head_Type, 1);
	buffer->Read(&Hair_Num, 1);
	buffer->Read(&Ear_Num, 1);
	buffer->Read(&Goggle_Num, 1);
	buffer->Read(&Beard_Num, 1);
	buffer->Read(&Weapon_Hip_Num, 1);
	buffer->Read(&Weapon_Unique_Num, 1);
	buffer->Read(&Weapon_Back_Num, 1);
	buffer->Read(&Head_Texture_Num, 1);
	buffer->Read(&Tattoo_Texture_Num, 1);
	buffer->Read(&Tattoo_Offset, 12);
	buffer->Read(&Hair_Color, 12);
	buffer->Read(&Beard_Color, 12);
	buffer->Read(&Eye_Color, 12);
	buffer->Read(&Skin_Color, 12);
	buffer->Read(&Shirt_Primary_Color, 12);
	buffer->Read(&Shirt_Secondary_Color, 12);
	buffer->Read(&Pants_Primary_Color, 12);
	buffer->Read(&Pants_Secondary_Color, 12);
	buffer->Read(&Shirt_Primary_Metal, 4);
	buffer->Read(&Shirt_Secondary_Metal, 4);
	buffer->Read(&Pants_Primary_Metal, 4);
	buffer->Read(&Pants_Secondary_Metal, 4);
	int i;
	for (i = 0; i < 5; i++) {
		buffer->Read(&Body_Weight[i], 4);
	}
	for (i = 0; i < 5; i++) {
		buffer->Read(&Head_Weight[i], 4);
	}
	return(true);
}

// 0x4BFEB0
int AvatarDescClass::Get_Short_Size()
{
	return(0xEF);
}

// 0x4BFED0
void AvatarDescClass::Unknown_00(void *object)
{
	LoginKVVisitorClass *visitor = (LoginKVVisitorClass *)object;

	visitor->Add(0x3FF, "avatar_first_name", First_Name, sizeof(First_Name));
	visitor->Add(0x3FF, "avatar_last_name", Last_Name, sizeof(Last_Name));
	visitor->Add(0x3FF, "avatar_type", &Avatar_Type);
	visitor->Add(0x3FF, "avatar_version", &Avatar_Version);
	visitor->Add(0x3FF, "race", &Race);
	visitor->Add(0x3FF, "profession", &Profession);
	visitor->Add(0x3FF, "gender", &Gender);
	visitor->Add(0x3FF, "mood_type", &Mood_Type);
	visitor->Add(0x3FF, "personality", &Personality);
	visitor->Add(0x3FF, "nlp", &NLP);
	visitor->Add(0x3FF, "body_type", &Body_Type);
	visitor->Add(0x3FF, "pants_type", &Pants_Type);
	visitor->Add(0x3FF, "head_type", &Head_Type);
	visitor->Add(0x3FF, "hair_num", &Hair_Num);
	visitor->Add(0x3FF, "ear_num", &Ear_Num);
	visitor->Add(0x3FF, "goggle_num", &Goggle_Num);
	visitor->Add(0x3FF, "beard_num", &Beard_Num);
	visitor->Add(0x3FF, "weapon_hip_num", &Weapon_Hip_Num);
	visitor->Add(0x3FF, "weapon_unique_num", &Weapon_Unique_Num);
	visitor->Add(0x3FF, "weapon_back_num", &Weapon_Back_Num);
	visitor->Add(0x3FF, "head_texture_num", &Head_Texture_Num);
	visitor->Add(0x3FF, "tattoo_texture_num", &Tattoo_Texture_Num);
	visitor->Add(0x3FF, "tattoo_offset", &Tattoo_Offset);
	visitor->Add(0x3FF, "hair_color", &Hair_Color);
	visitor->Add(0x3FF, "beard_color", &Beard_Color);
	visitor->Add(0x3FF, "eye_color", &Eye_Color);
	visitor->Add(0x3FF, "skin_color", &Skin_Color);
	visitor->Add(0x3FF, "shirt_primary_color", &Shirt_Primary_Color);
	visitor->Add(0x3FF, "shirt_secondary_color", &Shirt_Secondary_Color);
	visitor->Add(0x3FF, "pants_primary_color", &Pants_Primary_Color);
	visitor->Add(0x3FF, "pants_secondary_color", &Pants_Secondary_Color);
	visitor->Add(0x3FF, "shirt_primary_metal", &Shirt_Primary_Metal);
	visitor->Add(0x3FF, "shirt_secondary_metal", &Shirt_Secondary_Metal);
	visitor->Add(0x3FF, "pants_primary_metal", &Pants_Primary_Metal);
	visitor->Add(0x3FF, "pants_secondary_metal", &Pants_Secondary_Metal);
	((KVEncoderBaseClass *)visitor)->Add_String(0x3FF, "body_weight", Unknown_4C3110(Body_Weight, 5, ','));
	((KVEncoderBaseClass *)visitor)->Add_String(0x3FF, "head_weight", Unknown_4C3110(Head_Weight, 5, ','));
}

// NOT EXACT: shape (the first sprintf's arguments pushed before the three stores, and Head_Weight cleared through a pointer in the client; five forms tried: memset / a loop for Head_Weight, the stores reordered; attempts.py show 0x4C0310)
// 0x4C0310
void AvatarDescClass::Clear(int type)
{
	Avatar_Type = type;
	Unknown0C = 0;
	Avatar_Version = 2;
	sprintf(First_Name, "player");
	sprintf(Last_Name, "");
	Race = 0;
	Profession = 0;
	Gender = 0;
	Mood_Type = 0;
	Personality = 0;
	NLP = 0;
	Body_Type = 0;
	Pants_Type = 0;
	Head_Type = 0;
	Hair_Num = 0;
	Ear_Num = 0;
	Goggle_Num = 0;
	Beard_Num = 0;
	Weapon_Hip_Num = 0;
	Weapon_Unique_Num = 0;
	Weapon_Back_Num = 0;
	Head_Texture_Num = 0;
	Tattoo_Texture_Num = 0;
	Tattoo_Offset.Set(0.0F, 0.0F, 1.0F);
	Hair_Color.Set(0.0F, 0.0F, 0.0F);
	Beard_Color.Set(0.0F, 0.0F, 0.0F);
	Eye_Color.Set(0.0F, 0.0F, 0.0F);
	Skin_Color.Set(0.0F, 0.0F, 0.0F);
	Shirt_Primary_Color.Set(0.0F, 0.0F, 0.0F);
	Shirt_Secondary_Color.Set(0.0F, 0.0F, 0.0F);
	Pants_Primary_Color.Set(0.0F, 0.0F, 0.0F);
	Pants_Secondary_Color.Set(0.0F, 0.0F, 0.0F);
	Shirt_Primary_Metal = -1;
	Shirt_Secondary_Metal = -1;
	Pants_Primary_Metal = -1;
	Pants_Secondary_Metal = -1;
	Skeleton = "";
	Body_Weight[0] = 1.0F;
	Body_Weight[1] = 0.0F;
	Body_Weight[2] = 0.0F;
	Body_Weight[3] = 0.0F;
	Body_Weight[4] = 0.0F;
	Head_Weight[0] = 1.0F;
	Head_Weight[1] = 0.0F;
	Head_Weight[2] = 0.0F;
	Head_Weight[3] = 0.0F;
	Head_Weight[4] = 0.0F;
}

// 0x4C0580
bool AvatarDescClass::Is_Valid()
{
	return(Avatar_Type != 0);
}

// 0x4C05A0
void AvatarDescClass::Reset()
{
	Avatar_Type = 0;
	Load_Defaults(0, 0);
}

void Unknown_4BC260(const char *sender, const char *category, const char *file, int line, const char *format, ...);	// NAME 1 [Unknown_4BC260]: 0x4BC260, E&B's message reporter (buff_database.cpp)

static INIClass AvatarDefinitions;	// 0xBE55D8  // NAME 2: avatardef.ini
static const char *AvatarGenderNames[] = { "male_", "female_" };	// 0xB73CC8  // NAME 2: what it holds
static const char *AvatarRaceNames[] = { "terran_", "jenquai_", "progen_" };	// 0xB73CD0  // NAME 2: what it holds
static const char *AvatarProfessionNames[] = { "warrior", "tradesman", "explorer", "default" };	// 0xB73CDC  // NAME 2: what it holds
static const char *AvatarOtherNames[] = { "narrator", "npc" };	// 0xB73CEC  // NAME 1: what it holds

















































































































































































































































// 0x4C05D0
void AvatarDescClass::Load_Defaults(int type, const char *name)
{
	char section[64];

	if (!AvatarDefinitions.Is_Loaded()) {
		AvatarDefinitions.Load("avatardef.ini");
		if (!AvatarDefinitions.Is_Loaded()) {
			Unknown_4BC260("Avatar Descriptions", "", __FILE__, __LINE__, "avatardef.ini file not found");
			return;
		}
	}
	if (type == 0) {
		_snprintf(section, sizeof(section), "%s%s%s", AvatarGenderNames[Gender], AvatarRaceNames[Race], AvatarProfessionNames[Profession]);
	} else if (type == 1) {
		_snprintf(section, sizeof(section), "%s", AvatarOtherNames[0]);
	} else if (type == 2 && name != NULL) {
		_snprintf(section, sizeof(section), "%s", name);
	} else {
		return;
	}
	if (AvatarDefinitions.Find_Section(section) == NULL) {
		return;
	}
	if (AvatarDefinitions.Find_Entry(section, "Gender") != NULL) {
		Gender = AvatarDefinitions.Get_Int(section, "Gender", 0);
	}
	if (AvatarDefinitions.Find_Entry(section, "BodyType") != NULL) {
		Body_Type = AvatarDefinitions.Get_Int(section, "BodyType", 0);
	}
	if (AvatarDefinitions.Find_Entry(section, "PantsType") != NULL) {
		Pants_Type = AvatarDefinitions.Get_Int(section, "PantsType", 0);
	}
	if (AvatarDefinitions.Find_Entry(section, "HeadType") != NULL) {
		Head_Type = AvatarDefinitions.Get_Int(section, "HeadType", 0);
	}
	if (AvatarDefinitions.Find_Entry(section, "HeadTextureNum") != NULL) {
		Head_Texture_Num = AvatarDefinitions.Get_Int(section, "HeadTextureNum", 0);
	}
	if (AvatarDefinitions.Find_Entry(section, "TattooTexture") != NULL) {
		Tattoo_Texture_Num = AvatarDefinitions.Get_Int(section, "TattooTexture", 0);
	}
	if (AvatarDefinitions.Find_Entry(section, "HairNum") != NULL) {
		Hair_Num = AvatarDefinitions.Get_Int(section, "HairNum", 0);
	}
	if (AvatarDefinitions.Find_Entry(section, "BeardNum") != NULL) {
		Beard_Num = AvatarDefinitions.Get_Int(section, "BeardNum", 0);
	}
	if (AvatarDefinitions.Find_Entry(section, "EarNum") != NULL) {
		Ear_Num = AvatarDefinitions.Get_Int(section, "EarNum", 0);
	}
	if (AvatarDefinitions.Find_Entry(section, "GoggleNum") != NULL) {
		Goggle_Num = AvatarDefinitions.Get_Int(section, "GoggleNum", 0);
	}
	if (AvatarDefinitions.Find_Entry(section, "BeardNum") != NULL) {
		Beard_Num = AvatarDefinitions.Get_Int(section, "BeardNum", 0);
	}
	if (AvatarDefinitions.Find_Entry(section, "Personality") != NULL) {
		Personality = AvatarDefinitions.Get_Int(section, "Personality", 0);
	}
	if (AvatarDefinitions.Find_Entry(section, "NLP") != NULL) {
		NLP = AvatarDefinitions.Get_Int(section, "NLP", 0);
	}
	TPoint3D<float> black(0.0F, 0.0F, 0.0F);
	if (AvatarDefinitions.Find_Entry(section, "HairColor") != NULL) {
		TPoint3D<float> color = AvatarDefinitions.Get_Point(section, "HairColor", black);
		Hair_Color.X = color.X;
		Hair_Color.Y = color.Y;
		Hair_Color.Z = color.Z;
		Beard_Color.X = color.X;
		Beard_Color.Y = color.Y;
		Beard_Color.Z = color.Z;
	}
	if (AvatarDefinitions.Find_Entry(section, "EyeColor") != NULL) {
		TPoint3D<float> color = AvatarDefinitions.Get_Point(section, "EyeColor", black);
		Eye_Color.X = color.X;
		Eye_Color.Y = color.Y;
		Eye_Color.Z = color.Z;
	}
	if (AvatarDefinitions.Find_Entry(section, "SkinColor") != NULL) {
		TPoint3D<float> color = AvatarDefinitions.Get_Point(section, "SkinColor", black);
		Skin_Color.X = color.X;
		Skin_Color.Y = color.Y;
		Skin_Color.Z = color.Z;
	}
	if (AvatarDefinitions.Find_Entry(section, "ShirtPrimaryColor") != NULL) {
		TPoint3D<float> color = AvatarDefinitions.Get_Point(section, "ShirtPrimaryColor", black);
		Shirt_Primary_Color.X = color.X;
		Shirt_Primary_Color.Y = color.Y;
		Shirt_Primary_Color.Z = color.Z;
	}
	if (AvatarDefinitions.Find_Entry(section, "ShirtSecondaryColor") != NULL) {
		TPoint3D<float> color = AvatarDefinitions.Get_Point(section, "ShirtSecondaryColor", black);
		Shirt_Secondary_Color.X = color.X;
		Shirt_Secondary_Color.Y = color.Y;
		Shirt_Secondary_Color.Z = color.Z;
	}
	if (AvatarDefinitions.Find_Entry(section, "PantsPrimaryColor") != NULL) {
		TPoint3D<float> color = AvatarDefinitions.Get_Point(section, "PantsPrimaryColor", black);
		Pants_Primary_Color.X = color.X;
		Pants_Primary_Color.Y = color.Y;
		Pants_Primary_Color.Z = color.Z;
	}
	if (AvatarDefinitions.Find_Entry(section, "PantsSecondaryColor") != NULL) {
		TPoint3D<float> color = AvatarDefinitions.Get_Point(section, "PantsSecondaryColor", black);
		Pants_Secondary_Color.X = color.X;
		Pants_Secondary_Color.Y = color.Y;
		Pants_Secondary_Color.Z = color.Z;
	}
	if (AvatarDefinitions.Find_Entry(section, "TattooOffset") != NULL) {
		TPoint3D<float> color = AvatarDefinitions.Get_Point(section, "TattooOffset", black);
		Tattoo_Offset.X = color.X;
		Tattoo_Offset.Y = color.Y;
		Tattoo_Offset.Z = color.Z;
	}
	if (AvatarDefinitions.Find_Entry(section, "ShirtPrimaryMetal") != NULL) {
		Shirt_Primary_Metal = AvatarDefinitions.Get_Int(section, "ShirtPrimaryMetal", -1);
	}
	if (AvatarDefinitions.Find_Entry(section, "ShirtSecondaryMetal") != NULL) {
		Shirt_Secondary_Metal = AvatarDefinitions.Get_Int(section, "ShirtSecondaryMetal", -1);
	}
	if (AvatarDefinitions.Find_Entry(section, "PantsPrimaryMetal") != NULL) {
		Pants_Primary_Metal = AvatarDefinitions.Get_Int(section, "PantsPrimaryMetal", -1);
	}
	if (AvatarDefinitions.Find_Entry(section, "PantsSecondaryMetal") != NULL) {
		Pants_Secondary_Metal = AvatarDefinitions.Get_Int(section, "PantsSecondaryMetal", -1);
	}
	if (AvatarDefinitions.Find_Entry(section, "Skeleton") == NULL) {
		return;
	}
	char skeleton[256];
	AvatarDefinitions.Get_String(section, "Skeleton", "", skeleton, 0xFF);
	Skeleton = skeleton;
}


std::string Unknown_4BE9D0(const char *name);	// NAME 1 [Unknown_4BE9D0]: 0x4BE9D0, a name turned into another (kv_util.cpp's run)
char *Unknown_4A1590(char *buffer, int size, const char *string);	// 0x4A1590: a bounded strcpy  // NAME 1 [Unknown_4A1590]: neutral placeholder (D10)

// 0x4C1020
std::string AvatarDescClass::Get_Full_Name()
{
	std::string name;
	Get_Full_Name(name);
	return(name);
}

// 0x4C1200
std::string &AvatarDescClass::Get_Full_Name(std::string &name)
{
	name = First_Name;
	if (Last_Name[0] != 0) {
		name += " ";
		name += Last_Name;
	}
	return(name);
}

// 0x4C1460
void AvatarDescClass::Unknown_4C1460(const char *name, bool convert)
{
	std::string text;
	if (convert) {
		text = Unknown_4BE9D0(name);
	} else {
		text = name;
	}
	Unknown_4A1590(First_Name, sizeof(First_Name), text.c_str());
}

// 0x4C15F0
void AvatarDescClass::Unknown_4C15F0(const char *name, bool convert)
{
	std::string text;
	if (convert) {
		text = Unknown_4BE9D0(name);
	} else {
		text = name;
	}
	Unknown_4A1590(Last_Name, sizeof(Last_Name), text.c_str());
}

// 0x4C1780
void AvatarDescClass::Get_Body_Morph(Vector3 &scale, int morph)
{
	const char *names[5] = { "normal", "short_skinny", "short_fat", "tall_fat", "tall_skinny" };
	Vector3 result(1.0F, 1.0F, 1.0F);
	char section[64];

	if (!AvatarDefinitions.Is_Loaded() || morph < 1 || morph > 4) {
		scale.Set(1.0F, 1.0F, 1.0F);
		return;
	}
	_snprintf(section, sizeof(section), "%s%smorph", AvatarGenderNames[Gender], AvatarRaceNames[Race]);
	if (AvatarDefinitions.Find_Section(section) != NULL) {
		const char *entry = names[morph];
		TPoint3D<float> one(1.0F, 1.0F, 1.0F);
		if (AvatarDefinitions.Find_Entry(section, entry) != NULL) {
			TPoint3D<float> point = AvatarDefinitions.Get_Point(section, entry, one);
			result.X = point.X;
			result.Y = point.Y;
			result.Z = point.Z;
		}
	}
	scale = result;
}

// 0x4C1930
void AvatarDescClass::Get_Head_Morph(Vector3 &scale, int morph)
{
	const char *names[5] = { "normal", "short_narrow", "short_wide", "long_wide", "long_narrow" };
	Vector3 result(1.0F, 1.0F, 1.0F);
	char section[64];

	if (!AvatarDefinitions.Is_Loaded() || morph < 1 || morph > 4) {
		scale.Set(1.0F, 1.0F, 1.0F);
		return;
	}
	_snprintf(section, sizeof(section), "%s%smorph", AvatarGenderNames[Gender], AvatarRaceNames[Race]);
	if (AvatarDefinitions.Find_Section(section) != NULL) {
		const char *entry = names[morph];
		TPoint3D<float> one(1.0F, 1.0F, 1.0F);
		if (AvatarDefinitions.Find_Entry(section, entry) != NULL) {
			TPoint3D<float> point = AvatarDefinitions.Get_Point(section, entry, one);
			result.Y = point.X;
			result.Z = point.Y;
		}
	}
	scale = result;
}

// 0x4C1AC0
void AvatarDescClass::Get_Facial_Feature(bool &movable, int &scale)
{
	char section[64];

	movable = false;
	scale = 1;
	if (AvatarDefinitions.Is_Loaded()) {
		_snprintf(section, sizeof(section), "FacialFeature%02d", Tattoo_Texture_Num);
		if (AvatarDefinitions.Find_Section(section) != NULL) {
			movable = AvatarDefinitions.Get_Bool(section, "Movable", false);
			scale = AvatarDefinitions.Get_Int(section, "Scale", 1);
		}
	}
}

// NOT EXACT: frame (the client keeps the 0.5, 0.5 result in an 8-byte local, ours folds it; attempts.py show 0x4C1B80)
// 0x4C1B80
void AvatarDescClass::Get_Hair_Offset(Vector2 &offset)
{
	Vector2 result(0.5F, 0.5F);
	char section[64];

	if (AvatarDefinitions.Is_Loaded()) {
		_snprintf(section, sizeof(section), "%s%s%s", AvatarGenderNames[Gender], AvatarRaceNames[Race], AvatarProfessionNames[Profession]);
		TPoint2D<float> half(0.5F, 0.5F);
		if (AvatarDefinitions.Find_Entry(section, "HairOffset") != NULL) {
			TPoint2D<float> point = AvatarDefinitions.Get_Point(section, "HairOffset", half);
			result.X = point.X;
			result.Y = point.Y;
			offset = result;
			return;
		}
	}
	offset = result;
}

// NOT EXACT: frame (the client keeps the 0.5, 0.5 result in an 8-byte local, ours folds it; attempts.py show 0x4C1B80)
// 0x4C1CA0
void AvatarDescClass::Get_Skin_Offset(Vector2 &offset)
{
	Vector2 result(0.5F, 0.5F);
	char section[64];

	if (AvatarDefinitions.Is_Loaded()) {
		_snprintf(section, sizeof(section), "%s%s%s", AvatarGenderNames[Gender], AvatarRaceNames[Race], AvatarProfessionNames[Profession]);
		TPoint2D<float> half(0.5F, 0.5F);
		if (AvatarDefinitions.Find_Entry(section, "SkinOffset") != NULL) {
			TPoint2D<float> point = AvatarDefinitions.Get_Point(section, "SkinOffset", half);
			result.X = point.X;
			result.Y = point.Y;
			offset = result;
			return;
		}
	}
	offset = result;
}

// NOT EXACT: frame (the client keeps the 0.5, 0.5 result in an 8-byte local, ours folds it; attempts.py show 0x4C1B80)
// 0x4C1DC0
void AvatarDescClass::Get_Eye_Offset(Vector2 &offset)
{
	Vector2 result(0.5F, 0.5F);
	char section[64];

	if (AvatarDefinitions.Is_Loaded()) {
		_snprintf(section, sizeof(section), "%s%s%s", AvatarGenderNames[Gender], AvatarRaceNames[Race], AvatarProfessionNames[Profession]);
		TPoint2D<float> half(0.5F, 0.5F);
		if (AvatarDefinitions.Find_Entry(section, "EyeOffset") != NULL) {
			TPoint2D<float> point = AvatarDefinitions.Get_Point(section, "EyeOffset", half);
			result.X = point.X;
			result.Y = point.Y;
			offset = result;
			return;
		}
	}
	offset = result;
}

// NOT EXACT: frame (the client keeps the 0.5, 0.5 result in an 8-byte local, ours folds it; attempts.py show 0x4C1B80)
// 0x4C1EE0
void AvatarDescClass::Get_Shirt_Primary_Offset(Vector2 &offset)
{
	Vector2 result(0.5F, 0.5F);
	char section[64];

	if (AvatarDefinitions.Is_Loaded()) {
		_snprintf(section, sizeof(section), "%s%s%s", AvatarGenderNames[Gender], AvatarRaceNames[Race], AvatarProfessionNames[Profession]);
		TPoint2D<float> half(0.5F, 0.5F);
		if (AvatarDefinitions.Find_Entry(section, "ShirtPrimaryOffset") != NULL) {
			TPoint2D<float> point = AvatarDefinitions.Get_Point(section, "ShirtPrimaryOffset", half);
			result.X = point.X;
			result.Y = point.Y;
			offset = result;
			return;
		}
	}
	offset = result;
}

// NOT EXACT: frame (the client keeps the 0.5, 0.5 result in an 8-byte local, ours folds it; attempts.py show 0x4C1B80)
// 0x4C2000
void AvatarDescClass::Get_Shirt_Secondary_Offset(Vector2 &offset)
{
	Vector2 result(0.5F, 0.5F);
	char section[64];

	if (AvatarDefinitions.Is_Loaded()) {
		_snprintf(section, sizeof(section), "%s%s%s", AvatarGenderNames[Gender], AvatarRaceNames[Race], AvatarProfessionNames[Profession]);
		TPoint2D<float> half(0.5F, 0.5F);
		if (AvatarDefinitions.Find_Entry(section, "ShirtSecondaryOffset") != NULL) {
			TPoint2D<float> point = AvatarDefinitions.Get_Point(section, "ShirtSecondaryOffset", half);
			result.X = point.X;
			result.Y = point.Y;
			offset = result;
			return;
		}
	}
	offset = result;
}

// NOT EXACT: frame (the client keeps the 0.5, 0.5 result in an 8-byte local, ours folds it; attempts.py show 0x4C1B80)
// 0x4C2120
void AvatarDescClass::Get_Pants_Primary_Offset(Vector2 &offset)
{
	Vector2 result(0.5F, 0.5F);
	char section[64];

	if (AvatarDefinitions.Is_Loaded()) {
		_snprintf(section, sizeof(section), "%s%s%s", AvatarGenderNames[Gender], AvatarRaceNames[Race], AvatarProfessionNames[Profession]);
		TPoint2D<float> half(0.5F, 0.5F);
		if (AvatarDefinitions.Find_Entry(section, "PantsPrimaryOffset") != NULL) {
			TPoint2D<float> point = AvatarDefinitions.Get_Point(section, "PantsPrimaryOffset", half);
			result.X = point.X;
			result.Y = point.Y;
			offset = result;
			return;
		}
	}
	offset = result;
}

// NOT EXACT: frame (the client keeps the 0.5, 0.5 result in an 8-byte local, ours folds it; attempts.py show 0x4C1B80)
// 0x4C2240
void AvatarDescClass::Get_Pants_Secondary_Offset(Vector2 &offset)
{
	Vector2 result(0.5F, 0.5F);
	char section[64];

	if (AvatarDefinitions.Is_Loaded()) {
		_snprintf(section, sizeof(section), "%s%s%s", AvatarGenderNames[Gender], AvatarRaceNames[Race], AvatarProfessionNames[Profession]);
		TPoint2D<float> half(0.5F, 0.5F);
		if (AvatarDefinitions.Find_Entry(section, "PantsSecondaryOffset") != NULL) {
			TPoint2D<float> point = AvatarDefinitions.Get_Point(section, "PantsSecondaryOffset", half);
			result.X = point.X;
			result.Y = point.Y;
			offset = result;
			return;
		}
	}
	offset = result;
}

// NOT EXACT: order (the client's offsets and weights interleave differently; nine forms tried: attempts.py show 0x4C2360)
// 0x4C2360
void AvatarDescClass::Save_Defaults(const Vector2 *offsets)
{
	char section[64];
	TPoint3D<float> color;
	TPoint3D<float> offset;
	Vector2 uv;

	_snprintf(section, sizeof(section), "%s%s%s", AvatarGenderNames[Gender], AvatarRaceNames[Race], AvatarProfessionNames[Profession]);
	if (AvatarDefinitions.Find_Section(section) != NULL) {
		AvatarDefinitions.Put_Int(section, "Gender", Gender);
		AvatarDefinitions.Put_Int(section, "BodyType", Body_Type);
		AvatarDefinitions.Put_Int(section, "PantsType", Pants_Type);
		AvatarDefinitions.Put_Int(section, "HeadType", Head_Type);
		AvatarDefinitions.Put_Int(section, "HeadTextureNum", Head_Texture_Num);
		AvatarDefinitions.Put_Int(section, "TattooTextureNum", Tattoo_Texture_Num);
		AvatarDefinitions.Put_Int(section, "HairNum", Hair_Num);
		AvatarDefinitions.Put_Int(section, "BeardNum", Beard_Num);
		AvatarDefinitions.Put_Int(section, "EarNum", Ear_Num);
		AvatarDefinitions.Put_Int(section, "GoggleNum", Goggle_Num);
		AvatarDefinitions.Put_Int(section, "BeardNum", Beard_Num);
		color.X = Hair_Color.X;
		color.Y = Hair_Color.Y;
		color.Z = Hair_Color.Z;
		AvatarDefinitions.Put_Point(section, "HairColor", color);
		color.X = Eye_Color.X;
		color.Y = Eye_Color.Y;
		color.Z = Eye_Color.Z;
		AvatarDefinitions.Put_Point(section, "EyeColor", color);
		color.X = Skin_Color.X;
		color.Y = Skin_Color.Y;
		color.Z = Skin_Color.Z;
		AvatarDefinitions.Put_Point(section, "SkinColor", color);
		color.X = Shirt_Primary_Color.X;
		color.Y = Shirt_Primary_Color.Y;
		color.Z = Shirt_Primary_Color.Z;
		AvatarDefinitions.Put_Point(section, "ShirtPrimaryColor", color);
		color.X = Shirt_Secondary_Color.X;
		color.Y = Shirt_Secondary_Color.Y;
		color.Z = Shirt_Secondary_Color.Z;
		AvatarDefinitions.Put_Point(section, "ShirtSecondaryColor", color);
		color.X = Pants_Primary_Color.X;
		color.Y = Pants_Primary_Color.Y;
		color.Z = Pants_Primary_Color.Z;
		AvatarDefinitions.Put_Point(section, "PantsPrimaryColor", color);
		color.X = Pants_Secondary_Color.X;
		color.Y = Pants_Secondary_Color.Y;
		color.Z = Pants_Secondary_Color.Z;
		AvatarDefinitions.Put_Point(section, "PantsSecondaryColor", color);
		color.X = Tattoo_Offset.X;
		color.Y = Tattoo_Offset.Y;
		color.Z = Tattoo_Offset.Z;
		AvatarDefinitions.Put_Point(section, "TattooOffset", color);
		AvatarDefinitions.Put_Int(section, "ShirtPrimaryMetal", Shirt_Primary_Metal);
		AvatarDefinitions.Put_Int(section, "ShirtSecondaryMetal", Shirt_Secondary_Metal);
		AvatarDefinitions.Put_Int(section, "PantsPrimaryMetal", Pants_Primary_Metal);
		AvatarDefinitions.Put_Int(section, "PantsSecondaryMetal", Pants_Secondary_Metal);
		uv.Set(offsets[0].X, 1.0 - offsets[0].Y);
		offset.X = uv.X;
		offset.Y = uv.Y;
		offset.Z = 0.0F;
		AvatarDefinitions.Put_Point(section, "HairOffset", offset);
		uv.Set(offsets[1].X, 1.0 - offsets[1].Y);
		offset.X = uv.X;
		offset.Y = uv.Y;
		offset.Z = 0.0F;
		AvatarDefinitions.Put_Point(section, "EyeOffset", offset);
		uv.Set(offsets[2].X, 1.0 - offsets[2].Y);
		offset.X = uv.X;
		offset.Y = uv.Y;
		offset.Z = 0.0F;
		AvatarDefinitions.Put_Point(section, "SkinOffset", offset);
		uv.Set(offsets[3].X, 1.0 - offsets[3].Y);
		offset.X = uv.X;
		offset.Y = uv.Y;
		offset.Z = 0.0F;
		AvatarDefinitions.Put_Point(section, "ShirtPrimaryOffset", offset);
		uv.Set(offsets[4].X, 1.0 - offsets[4].Y);
		offset.X = uv.X;
		offset.Y = uv.Y;
		offset.Z = 0.0F;
		AvatarDefinitions.Put_Point(section, "ShirtSecondaryOffset", offset);
		uv.Set(offsets[5].X, 1.0 - offsets[5].Y);
		offset.X = uv.X;
		offset.Y = uv.Y;
		offset.Z = 0.0F;
		AvatarDefinitions.Put_Point(section, "PantsPrimaryOffset", offset);
		uv.Set(offsets[6].X, 1.0 - offsets[6].Y);
		offset.X = uv.X;
		offset.Y = uv.Y;
		offset.Z = 0.0F;
		AvatarDefinitions.Put_Point(section, "PantsSecondaryOffset", offset);
		AvatarDefinitions.Put_String(section, "Skeleton", Skeleton.c_str());
		AvatarDefinitions.Put_Float(section, "BodyWeight0", Body_Weight[0]);
		AvatarDefinitions.Put_Float(section, "BodyWeight1", Body_Weight[1]);
		AvatarDefinitions.Put_Float(section, "BodyWeight2", Body_Weight[2]);
		AvatarDefinitions.Put_Float(section, "BodyWeight3", Body_Weight[3]);
		AvatarDefinitions.Put_Float(section, "BodyWeight4", Body_Weight[4]);
		AvatarDefinitions.Put_Float(section, "HeadWeight0", Head_Weight[0]);
		AvatarDefinitions.Put_Float(section, "HeadWeight1", Head_Weight[1]);
		AvatarDefinitions.Put_Float(section, "HeadWeight2", Head_Weight[2]);
		AvatarDefinitions.Put_Float(section, "HeadWeight3", Head_Weight[3]);
		AvatarDefinitions.Put_Float(section, "HeadWeight4", Head_Weight[4]);
	}
	AvatarDefinitions.Save("avatardef.ini");
}

/*
**	Reconstruction notes
**
**	Not rebuilt yet (the assert 'strchr(buffer, Separator) == 0' is in the window's parser): the
**	window's other functions 0x4C1020-0x4C45B0 (the INI sections' parser, '[%s]%s' / 'Name = %s%s',
**	and the key/value templates the visitor instantiates: the int add 0x4C2EB0, the char add 0x4C2FE0,
**	the array text 0x4C3110).
*/
