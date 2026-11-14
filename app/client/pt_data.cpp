// Earth & Beyond client (reconstruction): app\client\pt_data.cpp. Notes at the end of the file.
#include <stdio.h>
#include "ini.h"
#include "nstrdup.h"
#include "..\shared\g_assert.h"
#include "pt_data.h"

// 0x696320
Unknown696320Class::Unknown696320Class()
{
	INIClass ini("pt_missions.ini");
	for (int i = 0; ; i++) {
		char section[12];
		sprintf(section, "%d", i);
		if (!ini.Is_Present(section)) break;
		Unknown696320RecordStruct *record = new Unknown696320RecordStruct;
		record->Type = ini.Get_Alloc_String(section, "Type", "Not Set");
		record->Sponsor = ini.Get_Alloc_String(section, "Sponsor", "Not Set");
		record->Short = ini.Get_Alloc_String(section, "Short", "Not Set");
		record->Payout = ini.Get_Alloc_String(section, "Payout", "Not Set");
		sprintf(section, "%d\\Desc", i);
		char text[1025];
		ini.Get_TextBlock(section, text, sizeof(text) - 1);
		record->Desc = nstrdup(text[0] != '\0' ? text : "Not Set");
		List.push_back(record);
	}
}

// 0x696700
Unknown696320Class::~Unknown696320Class()
{
	for (std::vector<Unknown696320RecordStruct *>::iterator it = List.begin(); it != List.end(); it++) {
		delete *it;
	}
}















































// 0x6967A0
Unknown696320RecordStruct *Unknown696320Class::Unknown_6967A0(int idx)
{
	G_ASSERT(idx >= 0);
	if (idx >= Count()) return(NULL);
	return(List[idx]);
}

// 0x696810
Unknown696810Class::Unknown696810Class()
{
	INIClass ini("pt_news.ini");
	for (int i = 0; ; i++) {
		char section[12];
		sprintf(section, "%d", i);
		if (!ini.Is_Present(section)) break;
		Unknown696810RecordStruct *record = new Unknown696810RecordStruct;
		record->Date = ini.Get_Alloc_String(section, "Date", "Not Set");
		record->Location = ini.Get_Alloc_String(section, "Location", "Not Set");
		record->Headline = ini.Get_Alloc_String(section, "Headline", "Not Set");
		sprintf(section, "%d\\Story", i);
		char text[1025];
		ini.Get_TextBlock(section, text, sizeof(text) - 1);
		record->Story = nstrdup(text[0] != '\0' ? text : "Not Set");
		List.push_back(record);
	}
}

// 0x696BC0
Unknown696810Class::~Unknown696810Class()
{
	for (int i = 0; i < Count(); i++) {
		delete List[i];
	}
}


















// 0x696C60
Unknown696810RecordStruct *Unknown696810Class::Unknown_696C60(int idx)
{
	G_ASSERT(idx >= 0);
	if (idx >= Count()) return(NULL);
	return(List[idx]);
}

// 0x696CD0
Unknown696CD0Class::Unknown696CD0Class()
{
	INIClass ini("pt_people.ini");
	for (int i = 0; ; i++) {
		char section[32];
		sprintf(section, "%d", i);
		if (!ini.Is_Present(section)) break;
		Unknown696CD0RecordStruct *record = new Unknown696CD0RecordStruct;
		record->Name = ini.Get_Alloc_String(section, "Name", "Not Set");
		record->Profession = ini.Get_Alloc_String(section, "Profession", "Not Set");
		record->Location = ini.Get_Alloc_String(section, "Location", "Not Set");
		record->Level = ini.Get_Int(section, "Level", 0);
		record->Combat = ini.Get_Int(section, "Combat", 0);
		record->Explore = ini.Get_Int(section, "Explore", 0);
		record->Trade = ini.Get_Int(section, "Trade", 0);
		sprintf(section, "%d\\Background", i);
		char text[1025];
		ini.Get_TextBlock(section, text, sizeof(text) - 1);
		record->Background = nstrdup(text[0] != '\0' ? text : "Not Set");
		List.push_back(record);
	}
}

// 0x697110
Unknown696CD0Class::~Unknown696CD0Class()
{
	for (int i = 0; i < Count(); i++) {
		delete List[i];
	}
}



















// 0x6971B0
Unknown696CD0RecordStruct *Unknown696CD0Class::Unknown_6971B0(int idx)
{
	G_ASSERT(idx >= 0);
	if (idx >= Count()) return(NULL);
	return(List[idx]);
}

/*
**	Reconstruction notes (app\client\pt_data.cpp, the petition terminal's three lists).
**
**	The client's __FILE__ (the G_ASSERTs 'idx >= 0' on lines 86 / 139 / 197) names the file; the ini files
**	pt_missions.ini, pt_news.ini and pt_people.ini and their keys name the records' fields. The object runs from
**	the missions list's constructor 0x696320 (after pdaint.cpp's adjustor thunks 0x696300 / 0x696310) to the last
**	vector instance 0x6974B0 (0x6974D5 padding to sbase_info.cpp at 0x6974F0); it has no <string> initialiser.
**	Template copies, three per list: ~vector (the constructors' unwind) 0x697220 / 0x697290 / 0x697300, size()
**	0x697260 / 0x6972D0 / 0x697340, _Ucopy and _Ufill 0x697370 / 0x6973B0, 0x6973F0 / 0x697430, 0x697470 /
**	0x6974B0 (push_back inlined into the constructors).
**
**	Forms: a record's constructor is memset(this, 0, sizeof(*this)) (5 stores for 0x14 / 0x10 bytes, rep stosd
**	for 0x20); the section test is Is_Present (its bool gives the setne); the text buffer is 1025 bytes with 1024
**	passed (the frame's extra dword); the section buffer is 12 bytes for missions / news and 32 for people (the
**	stack layout); the news / people destructors index the list (size() each pass), the missions one iterates.
**	The classes keep neutral names (no string names them, D10); pt_data.h is a PROVISIONAL header (D137).
*/
