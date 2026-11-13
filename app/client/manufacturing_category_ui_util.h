// Earth & Beyond client (reconstruction): app\client\manufacturing_category_ui_util.h, the header of
// manufacturing_category_ui_util.cpp (the client's file per its asserts' __FILE__
// 'e:\build\beta\G\app\Client\manufacturing_category_ui_util.cpp'; this header's name is provisional, NAME 1).
// The manufacturing categories' icon textures and display names: switches over the category ids, -1 for none,
// the default case asserting.
#ifndef MANUFACTURING_CATEGORY_UI_UTIL_H  // NAME 1 [manufacturing_category_ui_util.h]: provisional header name (D137)
#define MANUFACTURING_CATEGORY_UI_UTIL_H

#include <string>

std::string Unknown_7E9B20(int primary);		// 0x7E9B20: a first-level category's icon ('im_0.tga', 'im_1.tga'; no callers)  // NAME 1: neutral (D10); NAME 1 [primary]: neutral
std::string Unknown_7E9CB0(int category);		// 0x7E9CB0: a second-level category's icon ('im_00.tga' .. 'im_14.tga'; no callers)  // NAME 1: neutral (D10); NAME 1 [category]: neutral
std::string Unknown_7EA190(int id);				// 0x7EA190: an item type's icon ('im_000.tga' .. 'im_143.tga'; ui_item_examination_title.cpp asks for 100)  // NAME 1: neutral (D10); NAME 1 [id]: neutral
std::string Unknown_7EAE30(int primary);		// 0x7EAE30: a first-level category's name ('Item', 'Component'; no callers)  // NAME 1: neutral (D10); NAME 1 [primary]: neutral
std::string Unknown_7EAFE0(int category);		// 0x7EAFE0: a second-level category's name ('Weapon' .. 'Trade Good'; no callers)  // NAME 1: neutral (D10); NAME 1 [category]: neutral
std::string Unknown_7EB4D0(int type);			// 0x7EB4D0: an item type's name ('Beam Weapon' .. 'Core Metal'; ui_item_examination_title.cpp)  // NAME 1: neutral (D10); NAME 1 [type]: neutral

#endif
