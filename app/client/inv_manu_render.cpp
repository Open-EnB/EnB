// Earth & Beyond client (reconstruction): app\client\inv_manu_render.cpp, an inventory slot on the manufacturing screen
// (the client's file per its asserts, 'e:\build\beta\G\app\Client\inv_manu_render.cpp' at 0xB900CC). Notes at the end.
#include <string>
#include <stdio.h>
#include "..\shared\g_assert.h"
#include "..\shared\auxdata\permstr.h"	// (its PermStringInitClass static: 0x68BEC0)
#include "inv_manu_render.h"
#include "rendobj.h"
#include "fontmanager.h"

#include "inv_data_peek.h"	// InvDataPeekClass (InvDataClass::Unknown_688830)
class Unknown831340Class { public: class TextureClass *Unknown_831840(); char Unknown00[0x14]; std::string Name; char Unknown24[0x48 - 0x24]; int TechLevel; };	// client.h's item template (item_template.cpp), as far as this file reads it
typedef InvDataPeekClass Unknown688830Class;	// inv_data_peek.h's view of an inventory list, under this file's old name  // NAME 1: neutral (D10)
class InvDataClass { public: Unknown688830Class *Unknown_688830(); Unknown688830Class *Unknown_688850(); };	// NAME 3: Peek_Inv_Data, inv_data's __FILE__ 0xB90004
class SlotDataClass { public: virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C(); virtual InvDataClass *Unknown_10(); };	// NAME 3: slot_data_render.h's
// ro_funcs.cpp's
void Unknown_496730(RenderObjClass *robj, int a, bool b);	void Unknown_496AE0(RenderObjClass *robj, bool a);	bool Unknown_496110(RenderObjClass *robj, float a, float b, float c);	// NAME 1: neutral (D10); NAME 1 [robj, a, b, c]: neutral
const char *Get_Roman_Numeral(int value);	// inv_render.cpp's

// 0x68B370
InvManuRenderClass::InvManuRenderClass() :
	LevelText(0)
{
	LevelText = new FontSys("INVENTORY_LEVEL", 0, 0);
}

// 0x68B540
InvManuRenderClass::~InvManuRenderClass()
{
	if (LevelText) {
		LevelText->Remove();
		REF_PTR_RELEASE(LevelText);
	}
}

// 0x68B5E0
void InvManuRenderClass::Unknown_2C(bool force)
{
	int slot = Unknown20;	// NAME 1 [slot, data, item, count]: neutral
	if (slot != -1) {
		Unknown688830Class *data = Peek_Inv_Data()->Unknown_688830();
		Unknown831340Class *item = (Unknown831340Class *)data->Get_Item(slot, false);
		if (!data->Status) {
			force = true;
		}
		if (item) {
			int count = data->Get_Stack_Count(slot);
			if (!data->Status) {
				force = true;
			}
			data = Peek_Inv_Data()->Unknown_688850();
			Unknown831340Class *other = (Unknown831340Class *)data->Get_Item(slot, false);	// NAME 1 [other]: neutral
			if (!data->Status) {
				force = true;
			}
			if (other) {
				count = data->Get_Stack_Count(slot);
				if (!data->Status) {
					force = true;
				}
			}
			if (force) {
				Unknown_68BDA0(item->TechLevel);
			}
			Unknown_496AE0(Mesh, 0);
			if (count) {
				Unknown_496110(Mesh, 1.0f, 1.0f, 1.0f);
			} else {
				Unknown_496110(Mesh, 0.5f, 0.5f, 0.5f);
			}
		} else if (force) {
			Unknown_6A8FE0(LevelText);
		}
	} else if (force) {
		Unknown_6A8FE0(LevelText);
	}
	SlotDataRenderClass::Unknown_2C(force);
}

// 0x68B860
TextureClass *InvManuRenderClass::Unknown_34()
{
	int slot = Unknown20;
	if (slot != -1) {
		Unknown688830Class *data = Peek_Inv_Data()->Unknown_688850();
		Unknown831340Class *item = (Unknown831340Class *)data->Get_Item(slot, false);
		if (item) {
			return(item->Unknown_831840());
		}
		data = Peek_Inv_Data()->Unknown_688830();
		item = (Unknown831340Class *)data->Get_Item(slot, false);
		if (item) {
			return(item->Unknown_831840());
		}
	}
	return(0);
}



















// 0x68B980
const char *InvManuRenderClass::Unknown_28(std::string *string)
{
	char buffer[32];	// NAME 1 [buffer, data, item, quality]: neutral
	FAIL_IF(!string) return(0);
	SlotDataRenderClass::Unknown_28(string);
	Unknown688830Class *data = Peek_Inv_Data()->Unknown_688830();
	int slot = Unknown20;
	Unknown831340Class *item = (Unknown831340Class *)data->Get_Item(slot, false);
	if (item) {
		*string += item->Name;
		*string += "\n";
		float structure = data->Get_Structure(slot);	// NAME 1 [structure]: neutral
		float quality = data->Get_Quality(slot);
		if (structure != -1.0f) {
			sprintf(buffer, "Quality %0.0f%%\n", quality * 100.0f);
			*string += buffer;
		}
	} else {
		*string += "Empty Slot\n";
	}
	return("NORMAL_SLOT");
}













// 0x68BCF0
InvDataClass *InvManuRenderClass::Peek_Inv_Data()
{
	SlotDataClass *sd = SlotData;
	FAIL_IF(!sd) return(0);

	InvDataClass *id = sd->Unknown_10();
	G_ASSERT(id);
	return(id);
}

// 0x68BD60
int InvManuRenderClass::Unknown_30(int level)
{
	int result = SlotDataRenderClass::Unknown_30(level);
	if (result) {
		result--;
	}
	Unknown_496730(LevelText, result, 0);
	return(result);
}

// 0x68BDA0
void InvManuRenderClass::Unknown_68BDA0(int value)
{
	if (value > 0) {
		LevelText->Unknown_6CE420(0, "%s", Get_Roman_Numeral(value));
		Unknown_6A8F00(LevelText);
	} else {
		Unknown_6A8FE0(LevelText);
	}
}

/*
**	Reconstruction notes. The client's file per its asserts ('e:\build\beta\G\app\Client\inv_manu_render.cpp' at 0xB900CC; lines
**	121-160), its object 0x68B370-0x68BEFA: permstr.h's initialiser 0x68BEC0 and its atexit 0x68BEF0 end it, inv_render.cpp's starts at 0x68BF10.
**	The class name is D24's, from the file. 0x68B370 is what slot_data_render.cpp called Unknown68B370Class.
*/
