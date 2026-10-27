/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**	Original location: e:\build\beta\G\app\shared\shipcust_desc.cpp (client 0x4F6160-0x4F9740)
**	ShipCustDescClass (shipcust_desc.h), partly rebuilt: the race and profession setters, the wire
**	and key/value slots of the description, its parts and their settings.
*/
#include "shipcust_desc.h"
#include "auxdata\linearbu.h"
#include <stdio.h>

extern ShipCustSettingsClass Unknown_BE56D8[][2];		// NAME 1 [Unknown_BE56D8]: 0xBE56D8, each race's two default settings (filled at run time)

static int Unknown_B75D28[3][3] = { { 1, 0, 2 }, { 1, 0, 2 }, { 1, 0, 2 } };	// NAME 1 [Unknown_B75D28]: 0xB75D28, the hull style per race and profession
static int Unknown_B75D4C[3][3] = { { 1, 0, 2 }, { 1, 0, 2 }, { 1, 0, 2 } };	// NAME 1 [Unknown_B75D4C]: 0xB75D4C, the wing style per race and profession

// 0x4F6210
void ShipCustDescClass::Set_Race(int race)
{
	Race = (ShipCustRaceType)race;
}

// 0x4F6230
void ShipCustDescClass::Reset_Parts()
{
	for (int i = 0; i < 4; i++) {
		Parts[i].Settings[0] = Unknown_BE56D8[Race][0];
		Parts[i].Settings[1] = Unknown_BE56D8[Race][1];
	}
}

// 0x4F62F0
void ShipCustDescClass::Set_Profession(int profession)
{
	Hull_Style = Unknown_B75D28[Race][profession];
	Wing_Style = Unknown_B75D4C[Race][profession];
	Profession = (ShipCustProfessionType)profession;
}

// 0x4F6330
bool ShipCustDescClass::Write(LinearBufferClass *buffer)
{
	buffer->Write(&Race, 4);
	buffer->Write(&Profession, 4);
	buffer->Write(&Hull_Style, 4);
	buffer->Write(&Wing_Style, 4);
	buffer->Write(&Selected_Decal, 4);
	buffer->Write(Ship_Name, 0x1A);
	buffer->Write(&Ship_Name_Color, 12);
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 2; j++) {
			buffer->Write(&Parts[i].Settings[j].Color, 12);
			buffer->Write(&Parts[i].Settings[j].Is_Matte, 1);
			buffer->Write(&Parts[i].Settings[j].Metal, 4);
		}
	}
	return(true);
}

// 0x4F6420
bool ShipCustDescClass::Read(LinearBufferClass *buffer)
{
	buffer->Read(&Race, 4);
	buffer->Read(&Profession, 4);
	buffer->Read(&Hull_Style, 4);
	buffer->Read(&Wing_Style, 4);
	buffer->Read(&Selected_Decal, 4);
	buffer->Read(Ship_Name, 0x1A);
	buffer->Read(&Ship_Name_Color, 12);
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 2; j++) {
			buffer->Read(&Parts[i].Settings[j].Color, 12);
			buffer->Read(&Parts[i].Settings[j].Is_Matte, 1);
			buffer->Read(&Parts[i].Settings[j].Metal, 4);
		}
	}
	return(true);
}

// 0x4F6510
int ShipCustDescClass::Get_Size()
{
	return(0xC2);
}

// 0x4F6530
bool ShipCustDescClass::Write_Short(LinearBufferClass *buffer)
{
	buffer->Write(&Race, 4);
	buffer->Write(&Profession, 4);
	buffer->Write(&Hull_Style, 4);
	buffer->Write(&Wing_Style, 4);
	buffer->Write(&Selected_Decal, 4);
	buffer->Write(Ship_Name, 0x19);
	buffer->Write(&Ship_Name_Color, 12);
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 2; j++) {
			buffer->Write(&Parts[i].Settings[j].Color, 12);
			buffer->Write(&Parts[i].Settings[j].Is_Matte, 1);
			buffer->Write(&Parts[i].Settings[j].Metal, 4);
		}
	}
	return(true);
}

// 0x4F6620
bool ShipCustDescClass::Read_Short(LinearBufferClass *buffer)
{
	buffer->Read(&Race, 4);
	buffer->Read(&Profession, 4);
	buffer->Read(&Hull_Style, 4);
	buffer->Read(&Wing_Style, 4);
	buffer->Read(&Selected_Decal, 4);
	buffer->Read(Ship_Name, 0x19);
	Ship_Name[0x19] = 0;
	buffer->Read(&Ship_Name_Color, 12);
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 2; j++) {
			buffer->Read(&Parts[i].Settings[j].Color, 12);
			buffer->Read(&Parts[i].Settings[j].Is_Matte, 1);
			buffer->Read(&Parts[i].Settings[j].Metal, 4);
		}
	}
	return(true);
}

// 0x4F6730
int ShipCustDescClass::Get_Short_Size()
{
	return(0xC1);
}

// 0x4F6750
void ShipCustDescClass::Unknown_00(void *object)
{
	LoginKVVisitorClass *visitor = (LoginKVVisitorClass *)object;
	char name[64];

	visitor->Add_Typed(0x3FF, "Profession", &Profession);
	visitor->Add_Typed(0x3FF, "Race", &Race);
	visitor->Add(0x3FF, "TechLevel", &Tech_Level);
	visitor->Add(0x3FF, "HullStyle", &Hull_Style);
	visitor->Add(0x3FF, "WingStyle", &Wing_Style);
	visitor->Add(0x3FF, "SelectedDecal", &Selected_Decal);
	visitor->Add(0x3FF, "ShipName", Ship_Name, 0x1A);
	visitor->Add_Color(0x3FF, "ShipNameColor", &Ship_Name_Color);
	for (int i = 0; i < 4; i++) {
		_snprintf(name, sizeof(name), "PartAttributes%d", i);
		visitor->Add_Child(0x3FF, name, &Parts[i]);
	}
}

// 0x4F6890
void ShipCustPartClass::Unknown_00(void *object)
{
	LoginKVVisitorClass *visitor = (LoginKVVisitorClass *)object;
	char name[64];

	for (int i = 0; i < 2; i++) {
		_snprintf(name, sizeof(name), "Settings%d", i);
		visitor->Add_Child(0x3FF, name, &Settings[i]);
	}
}

// 0x4F6900
void ShipCustSettingsClass::Unknown_00(void *object)
{
	LoginKVVisitorClass *visitor = (LoginKVVisitorClass *)object;

	visitor->Add_Color(0x3FF, "Color", &Color);
	visitor->Add_Typed(0x3FF, "IsMatte", &Is_Matte);
	visitor->Add_Typed(0x3FF, "Metal", &Metal);
}

/*
**	Reconstruction notes
**
**	Not rebuilt yet: the window's key/value templates (0x4F6970-0x4F7870, attributed here by their
**	instantiation: the race, profession, metal and bool adds), 0x4F6160-0x4F61C0 before the
**	setters, and 0x4F7930-0x4F9740 after them. The settings' constructor 0x618500 and the part's
**	out-of-line constructor 0x6191F0 lie in other files; the description's assignment 0x55D8A0 in
**	ui_avsel.cpp's window.
*/
