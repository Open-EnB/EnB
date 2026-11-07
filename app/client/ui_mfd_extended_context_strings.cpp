// Earth & Beyond client (reconstruction): app\client\ui_mfd_extended_context_strings.cpp (per the client's asserts)
#include <string>
#include "sclient.h"
#include "fontmanager.h"
#include "fontdatabase.h"
#include "rendobj.h"
#include "..\shared\g_assert.h"
#include "ui_mfd.h"
#include "ui_mfd_extended_context_strings.h"

// 0x5C1090
UIMFDExtendedContextStringsClass::UIMFDExtendedContextStringsClass(ClientObjectClass *co, UIMFDClass *owner) :
	UIMFDExtendedContextClass(owner),
	UnknownCount(0)
{
}

// 0x5C1110
UIMFDExtendedContextStringsClass::~UIMFDExtendedContextStringsClass()
{
	std::vector<FontSys *>::iterator it = Fonts.begin();
	while (it != Fonts.end()) {
		if (*it != NULL) {
			(*it)->Release_Ref();
		}
		it++;
	}
	Fonts.clear();
}

// 0x5C1220
void UIMFDExtendedContextStringsClass::Create_Fonts()
{
	int level = 15;
	UIMFDClass *owner = Owner;
	FontSys *fontbase = owner->Lines[0].Font;












	G_ASSERT(0 != fontbase);
	if (fontbase != NULL) {
		level = fontbase->Get_Sort_Level();
	}
	int index = 0;
	while (true) {
		std::string name = Get_Font_Name(index);
		if (name.length() == 0) {
			break;
		}
		if (Unknown_BE67B8->Find_String(name.c_str()) == NULL) {
			break;
		}
		FontSys *font = new FontSys(name.c_str(), 0, 0);
		font->Set_Sort_Level(level);
		Fonts.push_back(font);
		index++;
	}
	StringEmpty.resize(Fonts.size(), true);
}

// 0x5C1670
void UIMFDExtendedContextStringsClass::Set_Texts()
{
	UnknownCount = 0;
	unsigned int count = Fonts.size();
	for (unsigned int i = 0; i < count; i++) {






		FontSys *&font = Fonts[i];
		FAIL_IF(0 == font)
			continue;
		std::string text = Get_Line_Text(i);
		bool empty = (text.length() == 0);
		StringEmpty[i] = empty;
		font->Change_Text(text.c_str(), 2);
		if (!empty) {
			UnknownCount++;
		}
	}
}

// 0x5C17D0
void UIMFDExtendedContextStringsClass::Unknown_08()
{
	Vector3 position;
	FontSys *fontbase = Owner->Lines[0].Font;
	G_ASSERT(0 != fontbase);
	if (fontbase != NULL) {
		position = fontbase->Get_Position();
	}
	SceneClass *scene = Owner->Layer.Scene;

	G_ASSERT(0 != scene);
	unsigned int count = Fonts.size();
	for (unsigned int i = 0; i < count; i++) {
		FontSys *&font = Fonts[i];

		FAIL_IF(0 == font)
			continue;
		if (font->Peek_Scene() == NULL) {
			font->Add(scene);
		}
		font->Set_Position(position);
	}
}

// 0x5C18F0
void UIMFDExtendedContextStringsClass::Unknown_0C()
{
	unsigned int count = Fonts.size();
	for (unsigned int i = 0; i < count; i++) {
		FontSys *&font = Fonts[i];
		FAIL_IF(0 == font)
			continue;
		if (font->Peek_Scene() != NULL) {
			font->Remove();
		}
	}
}
// 0x5C1970
void UIMFDExtendedContextStringsClass::Unknown_10()
{
	G_ASSERT(Fonts.size() == StringEmpty.size());
	unsigned int count = Fonts.size();
	for (unsigned int i = 0; i < count; i++) {

		FontSys *&font = Fonts[i];
		FAIL_IF(0 == font)
			continue;
		if (Line_Changed(i)) {
			if (!StringEmpty[i]) {
				UnknownCount--;
			}
			std::string text = Get_Line_Text(i);
			bool empty = (text.length() == 0);
			StringEmpty[i] = empty;
			if (!empty) {
				UnknownCount++;
			}
			font->Change_Text(text.c_str(), 0);
		}
	}
}

// 0x5C1B30
unsigned int UIMFDExtendedContextStringsClass::Get_Line_Count()
{
	return(Fonts.size());
}
