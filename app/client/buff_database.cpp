// Earth & Beyond client (reconstruction): app\Client\buff_database.cpp. Notes at the end of the file.
#include <stdio.h>
#include "buff_database.h"
#include "..\shared\unknown_debug.h"
#include "..\..\wwlib\ini.h"
#include "..\..\wwlib\wwstring.h"
#include "..\..\wwlib\nstrdup.h"

void Unknown_4BC000(const char *sender, const char *category, const char *file, int line, const char *format, ...);	// NAME 1 [Unknown_4BC000]: 0x4BC000, E&B's message reporter (tcpconn.cpp)
void Unknown_4BC260(const char *sender, const char *category, const char *file, int line, const char *format, ...);	// NAME 1 [Unknown_4BC260]: 0x4BC260, 0x4BC000's sibling

BuffDatabaseClass *BuffDatabaseClass::Instance;

BuffDefStruct::BuffDefStruct(const char *type, const char *asset, const char *tool_tip, const char *alt_tool_tip, bool is_good)
{
	Type = nstrdup(type);
	Asset = nstrdup(asset);
	ToolTip = nstrdup(tool_tip);
	AltToolTip = nstrdup(alt_tool_tip);
	IsGoodBuff = is_good;
}

BuffDefStruct::~BuffDefStruct()
{
	delete [] Type;
	Type = 0;
	delete [] Asset;
	Asset = 0;
	delete [] ToolTip;
	ToolTip = 0;
	delete [] AltToolTip;
	AltToolTip = 0;
}









BuffDatabaseClass::BuffDatabaseClass() : Buffs(0)
{
	INIClass ini("buffdef.ini");
	if (!ini.Is_Loaded()) {
		Unknown_4BC260("Buff Database", "", __FILE__, __LINE__, "buffdef.ini file not found");
		return;
	}







	int index = 0;
	while (true) {
		{
			char section[64];
			sprintf(section, "BUFF-%d", index);
			if (ini.Find_Section(section) == 0) {
				break;
			}

			StringClass type;
			ini.Get_String(type, section, "BuffType", "");
			StringClass asset;
			ini.Get_String(asset, section, "BuffAsset", "");
			StringClass tool_tip;
			ini.Get_String(tool_tip, section, "BuffToolTip", "");
			StringClass alt_tool_tip;
			ini.Get_String(alt_tool_tip, section, "BuffAltToolTip", "");
			bool is_good = ini.Get_Bool(section, "IsGoodBuff", false);
			BuffDefStruct *def = new BuffDefStruct(type, asset, tool_tip, alt_tool_tip, is_good);
			if (Find(def->Type) != 0) {
				delete def;
				Unknown_4BC000("Buff Database", "", __FILE__, __LINE__, "Duplicate buff type definition encountered");
			} else {
				Buffs[std::string(def->Type)] = def;
			}
		}
		index++;
	}
}
// NOT EXACT: inline budget (explain.py: frame 8 vs 4), the client inlines the iterator's ++ but calls the bucket size 0x677AB0 and _M_bkt_num 0x677AE0 out of line; ours keeps the iterator in the frame
BuffDatabaseClass::~BuffDatabaseClass()
{
	for (std::hash_map<std::string, BuffDefStruct *>::iterator i = Buffs.begin(); i != Buffs.end(); ++i) {
		delete (*i).second;
	}
	Buffs.clear();
}

void BuffDatabaseClass::Set_Instance(BuffDatabaseClass *database)
{
	Instance = database;
}

BuffDefStruct *BuffDatabaseClass::Find(const char *type)
{
	std::hash_map<std::string, BuffDefStruct *>::iterator i = Buffs.find(type);
	if (i == Buffs.end()) return 0;
	return (*i).second;
}

/*
**	Reconstruction notes
**
**	client.exe 0x676CD0-0x677660 (then the table's instances to 0x678420): BuffDefStruct's constructor
**	0x676CD0 (nstrdup of the four strings) and destructor 0x676D40, the database's constructor 0x676DB0
**	(the table built as Buffs(0): the client pushes 0 to the out-of-line three-argument table
**	constructor, the identical copies 0x450DB0 / 0x4E5B90 / 0x502200 / 0x6778A0), the destructor 0x677410,
**	Set_Instance 0x677640 (0xBE6014) and Find 0x677660 (identical bytes to tutorial_manager.cpp's
**	0x6AD230). The constructor reads sections BUFF-0, BUFF-1, ... until one is missing; the reports sit
**	on the client's lines 47 ('buffdef.ini file not found', 0x4BC260) and 78 ('Duplicate buff type
**	definition encountered', 0x4BC000) (D25). Forms: the index outside the loop and incremented after an
**	inner block (so the StringClass destructors run first); the duplicate test first (`!= 0`).
**	Callers: 0x526DE0 (new, Set_Instance, delete), 0x560D20 (Find).
*/
