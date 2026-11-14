// Earth & Beyond client (reconstruction): app\Client\manufacturing_category_ui_util.cpp. Notes at the end of the file.
#include "manufacturing_category_ui_util.h"
#include "..\shared\g_assert.h"

























std::string Unknown_7E9B20(int primary)
{
	switch (primary) {
		case -1: return "im_default.tga";
		case 0: return "im_0.tga";
		case 1: return "im_1.tga";
		default:
			G_ASSERT(0);
			return "im_default.tga";
	}
}




std::string Unknown_7E9CB0(int category)
{
	switch (category) {
		case -1: return "im_default.tga";
		case 10: return "im_00.tga";
		case 11: return "im_01.tga";
		case 12: return "im_02.tga";
		case 13: return "im_03.tga";
		case 50: return "im_10.tga";
		case 51: return "im_11.tga";
		case 52: return "im_12.tga";
		case 53: return "im_13.tga";
		case 54: return "im_14.tga";
		case 80: return "im_default.tga";
		case 90: return "im_03.tga";
		default:
			G_ASSERT(0);
			return "im_default.tga";
	}
}




// NOT EXACT: shape (explain.py; an inline budget: the client expands std::string assign inline at case 153, ours at 160; every other site and the length agree) (0x7EA190)
std::string Unknown_7EA190(int id)
{
	switch (id) {
		case -1: return "im_default.tga";
		case 100: return "im_000.tga";
		case 101: return "im_001.tga";
		case 102: return "im_002.tga";
		case 103: return "im_003.tga";
		case 110: return "im_010.tga";
		case 111: return "im_011.tga";
		case 120: return "im_020.tga";
		case 121: return "im_021.tga";
		case 122: return "im_022.tga";
		case 130: return "im_03.tga";
		case 140: return "im_100.tga";
		case 141: return "im_101.tga";
		case 142: return "im_102.tga";
		case 150: return "im_110.tga";
		case 151: return "im_111.tga";
		case 152: return "im_112.tga";
		case 153: return "im_113.tga";
		case 160: return "im_120.tga";
		case 161: return "im_121.tga";
		case 162: return "im_122.tga";
		case 163: return "im_123.tga";
		case 170: return "im_130.tga";
		case 171: return "im_131.tga";
		case 172: return "im_132.tga";
		case 173: return "im_133.tga";
		case 180: return "im_140.tga";
		case 181: return "im_141.tga";
		case 182: return "im_142.tga";
		case 183: return "im_143.tga";
		case 200: return "im_03.tga";
		case 201: return "im_03.tga";
		case 202: return "im_03.tga";
		case 203: return "im_03.tga";
		case 204: return "im_03.tga";
		case 205: return "im_03.tga";
		case 206: return "im_03.tga";
		case 207: return "im_03.tga";
		case 208: return "im_03.tga";
		case 209: return "im_03.tga";
		case 210: return "im_03.tga";
		case 211: return "im_03.tga";
		default:
			G_ASSERT(0);
			return "im_default.tga";
	}
}




std::string Unknown_7EAE30(int primary)
{
	switch (primary) {
		case -1: return "";
		case 0: return "Item";
		case 1: return "Component";
		default:
			G_ASSERT(0);
			return "";
	}
}




std::string Unknown_7EAFE0(int category)
{
	switch (category) {
		case -1: return "";
		case 10: return "Weapon";
		case 11: return "Device";
		case 12: return "Core Item";
		case 13: return "Consumable";
		case 50: return "Electronic Item";
		case 51: return "Reactor";
		case 52: return "Fabricated Item";
		case 53: return "Weapon Component";
		case 54: return "Ammunition";
		case 80: return "Refined Resource";
		case 90: return "Trade Good";
		default:
			G_ASSERT(0);
			return "";
	}
}




// NOT EXACT: shape (explain.py; an inline budget: the client expands std::string assign inline at case 153, ours at 160, as 0x7EA190) (0x7EB4D0)
std::string Unknown_7EB4D0(int type)
{
	switch (type) {
		case -1: return "";
		case 100: return "Beam Weapon";
		case 101: return "Projectile Weapon";
		case 102: return "Missile Launcher";
		case 103: return "Ammunition";
		case 110: return "Device";
		case 111: return "Drone";
		case 120: return "Reactor";
		case 121: return "Engine";
		case 122: return "Shield";
		case 130: return "Consumable";
		case 140: return "Software";
		case 141: return "Electronic Item";
		case 142: return "Computer";
		case 150: return "Power Converter";
		case 151: return "Power Coupling";
		case 152: return "Power Core";
		case 153: return "Power Generator";
		case 160: return "Casing";
		case 161: return "Engine Frame";
		case 162: return "Drone Frame";
		case 163: return "Mount";
		case 170: return "Firing Mechanism";
		case 171: return "Ammunition Feeder";
		case 172: return "Beam Optic";
		case 173: return "Weapon Barrel";
		case 180: return "Ammunition Slug";
		case 181: return "Warhead";
		case 182: return "Propellant";
		case 183: return "Shell Casing";
		case 200: return "Optic Gem";
		case 201: return "Metal";
		case 202: return "Conductor";
		case 203: return "Alloy";
		case 204: return "Radioactive";
		case 205: return "Catalyst";
		case 206: return "Hydrocarbon";
		case 207: return "Silicate";
		case 208: return "Magnetic";
		case 209: return "Gemstone";
		case 210: return "Gas";
		case 211: return "Core Metal";
		default:
			G_ASSERT(0);
			return "";
	}
}

/*
**	Reconstruction notes (RECONSTRUCTED, 2026-11-13, O12).
**
**	The client's manufacturing_category_ui_util.cpp per the asserts' __FILE__ ('0' at lines 36, 60, 115, 130, 154, 209:
**	G_ASSERT(0) in each switch's default). Its object, 0x7E9B20-0x7EC3D0, lies between the previous object's guarded
**	<string> ids initialiser 0x7E9AF0 and its own at 0x7EC3B0: six free functions returning std::string by value, each
**	a switch over a manufacturing category id (-1 for none). The icons: 0x7E9B20 (first level, 0..1), 0x7E9CB0 (second
**	level, 10..13, 50..54, 80, 90), 0x7EA190 (item types, 100..211); the names: 0x7EAE30, 0x7EAFE0, 0x7EB4D0 over the
**	same ids. The ids and texts are the client's (its jump tables and strings 0xBA1630-0xBA1B64); the names of the
**	functions are neutral (D10) until a caller or symbol names them. Each case is one `return "..."` on its own line:
**	the client's per-case blocks (the shared im_03.tga tail is VC6's tail merging) and the assert lines fix that layout.
*/
