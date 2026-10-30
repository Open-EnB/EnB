// Earth & Beyond client (reconstruction): the key names (provisional file name, see inpkeys.h). Notes at the end of the file.
#include "inpkeys.h"
#include <hash_map.h>

static std::hash_map<std::string, unsigned int> NameToKey;		// 0xBDA940  // NAME 2: what it holds
static std::hash_map<int, std::string> KeyToName;		// 0xBDA928  // NAME 2: what it holds
static bool KeyNamesReady;											// 0xBDA958  // NAME 2: what it holds

unsigned int Unknown_42B610(const std::string &name)
{
	if (!KeyNamesReady) {
		Unknown_42BC90();
	}
	std::hash_map<std::string, unsigned int>::iterator it = NameToKey.find(name);
	if (it != NameToKey.end()) {
		return (*it).second;
	}
	return 0;
}

const std::string &Unknown_42B6A0(unsigned int key)
{
	if (!KeyNamesReady) {
		Unknown_42BC90();
	}
	std::hash_map<int, std::string>::iterator it = KeyToName.find(key);
	if (it != KeyToName.end()) {
		return (*it).second;
	}
	static std::string empty;
	return empty;
}

std::string &Unknown_42B7E0(const KernelEventClass &event, std::string &name)
{
	name.erase();
	if (event.Flags & 0x200) {
		name += "Ctrl-";
	}
	if (event.Flags & 0x400) {
		name += "Alt-";
	}
	if (event.Flags & 0x100) {
		name += "Shift-";
	}
	name += Unknown_42B6A0(event.Key);
	return name;
}

void Unknown_42BAE0(unsigned int key, const char *name)
{
	std::string text(name);
	NameToKey[text] = key;
	KeyToName[key] = text;
}

void Unknown_42BC90()
{
	NameToKey.clear();
	KeyToName.clear();
	Unknown_42BAE0(0x00, " ");
	Unknown_42BAE0(0x30, "0");
	Unknown_42BAE0(0x31, "1");
	Unknown_42BAE0(0x32, "2");
	Unknown_42BAE0(0x33, "3");
	Unknown_42BAE0(0x34, "4");
	Unknown_42BAE0(0x35, "5");
	Unknown_42BAE0(0x36, "6");
	Unknown_42BAE0(0x37, "7");
	Unknown_42BAE0(0x38, "8");
	Unknown_42BAE0(0x39, "9");
	Unknown_42BAE0(0x41, "A");
	Unknown_42BAE0(0x42, "B");
	Unknown_42BAE0(0xDC, "\\");
	Unknown_42BAE0(0x08, "Backspace");
	Unknown_42BAE0(0x43, "C");
	Unknown_42BAE0(0x14, "Caps");
	Unknown_42BAE0(0xBC, ",");
	Unknown_42BAE0(0x44, "D");
	Unknown_42BAE0(0x2E, "Del");
	Unknown_42BAE0(0x28, "Down");
	Unknown_42BAE0(0x45, "E");
	Unknown_42BAE0(0x2E, "Ext Del");
	Unknown_42BAE0(0x62, "Ext Down");
	Unknown_42BAE0(0x61, "Ext End");
	Unknown_42BAE0(0x67, "Ext Home");
	Unknown_42BAE0(0x2D, "Ext Ins");
	Unknown_42BAE0(0x64, "Ext Left");
	Unknown_42BAE0(0x63, "Ext PgDn");
	Unknown_42BAE0(0x69, "Ext PgUp");
	Unknown_42BAE0(0x66, "Ext Right");
	Unknown_42BAE0(0x68, "Ext Up");
	Unknown_42BAE0(0x23, "End");
	Unknown_42BAE0(0xBB, "=");
	Unknown_42BAE0(0x1B, "Esc");
	Unknown_42BAE0(0x46, "F");
	Unknown_42BAE0(0x70, "F1");
	Unknown_42BAE0(0x79, "F10");
	Unknown_42BAE0(0x7A, "F11");
	Unknown_42BAE0(0x7B, "F12");
	Unknown_42BAE0(0x71, "F2");
	Unknown_42BAE0(0x72, "F3");
	Unknown_42BAE0(0x73, "F4");
	Unknown_42BAE0(0x74, "F5");
	Unknown_42BAE0(0x75, "F6");
	Unknown_42BAE0(0x76, "F7");
	Unknown_42BAE0(0x77, "F8");
	Unknown_42BAE0(0x78, "F9");
	Unknown_42BAE0(0x47, "G");
	Unknown_42BAE0(0xC0, "`");
	Unknown_42BAE0(0x48, "H");
	Unknown_42BAE0(0x24, "Home");
	Unknown_42BAE0(0x49, "I");
	Unknown_42BAE0(0x2D, "Ins");
	Unknown_42BAE0(0x4A, "J");
	Unknown_42BAE0(0x4B, "K");
	Unknown_42BAE0(0x60, "Keypad 0");
	Unknown_42BAE0(0x61, "Keypad 1");
	Unknown_42BAE0(0x62, "Keypad 2");
	Unknown_42BAE0(0x63, "Keypad 3");
	Unknown_42BAE0(0x64, "Keypad 4");
	Unknown_42BAE0(0x65, "Keypad 5");
	Unknown_42BAE0(0x66, "Keypad 6");
	Unknown_42BAE0(0x67, "Keypad 7");
	Unknown_42BAE0(0x68, "Keypad 8");
	Unknown_42BAE0(0x69, "Keypad 9");
	Unknown_42BAE0(0x6A, "Keypad *");
	Unknown_42BAE0(0x100, "Keypad Enter");
	Unknown_42BAE0(0x6D, "Keypad -");
	Unknown_42BAE0(0x6E, "Keypad .");
	Unknown_42BAE0(0x6B, "Keypad +");
	Unknown_42BAE0(0x6B, "Keypad +");
	Unknown_42BAE0(0x6F, "Keypad /");
	Unknown_42BAE0(0x4C, "L");
	Unknown_42BAE0(0x12, "L Alt");
	Unknown_42BAE0(0xDB, "[");
	Unknown_42BAE0(0x11, "L Ctrl");
	Unknown_42BAE0(0x25, "Left");
	Unknown_42BAE0(0x01, "L Mouse");
	Unknown_42BAE0(0x10, "L Shift");
	Unknown_42BAE0(0x4D, "M");
	Unknown_42BAE0(0xBD, "-");
	Unknown_42BAE0(0x04, "M Mouse");
	Unknown_42BAE0(0x4E, "N");
	Unknown_42BAE0(0x90, "Num Lock");
	Unknown_42BAE0(0x4F, "O");
	Unknown_42BAE0(0x50, "P");
	Unknown_42BAE0(0x13, "Pause");
	Unknown_42BAE0(0xBE, ".");
	Unknown_42BAE0(0x22, "PgDn");
	Unknown_42BAE0(0x21, "PgUp");
	Unknown_42BAE0(0x2C, "Prnt Scrn");
	Unknown_42BAE0(0x51, "Q");
	Unknown_42BAE0(0x52, "R");
	Unknown_42BAE0(0x12, "R Alt");
	Unknown_42BAE0(0xDD, "]");
	Unknown_42BAE0(0x11, "R Ctrl");
	Unknown_42BAE0(0x0D, "Return");
	Unknown_42BAE0(0x27, "Right");
	Unknown_42BAE0(0x02, "R Mouse");
	Unknown_42BAE0(0x10, "R Shift");
	Unknown_42BAE0(0x53, "S");
	Unknown_42BAE0(0x91, "Scroll Lock");
	Unknown_42BAE0(0xBA, "Semicolon");
	Unknown_42BAE0(0xBF, "/");
	Unknown_42BAE0(0x20, "Space");
	Unknown_42BAE0(0xDE, "'");
	Unknown_42BAE0(0x54, "T");
	Unknown_42BAE0(0x09, "Tab");
	Unknown_42BAE0(0x55, "U");
	Unknown_42BAE0(0x26, "Up");
	Unknown_42BAE0(0x56, "V");
	Unknown_42BAE0(0x57, "W");
	Unknown_42BAE0(0x58, "X");
	Unknown_42BAE0(0x59, "Y");
	Unknown_42BAE0(0x5A, "Z");
	Unknown_42BAE0(0x5B, "L Win");
	Unknown_42BAE0(0x5C, "R Win");
	Unknown_42BAE0(0x5D, "Menu");
	KeyNamesReady = true;
}
/*
**	Reconstruction notes
**
**	client.exe: the object between inpdsp.cpp's end (std::ctype's guarded static 0x42B320) and
**	input.cpp (its first function after the guarded static 0x42E350): the tables' static
**	constructors and destructors 0x42B3D0-0x42B540, the five functions 0x42B610-0x42BC90 and the
**	tables' instances 0x42C330-0x42E190. No __FILE__ and no assert; its strings (0xB6E5F4-0xB6E948)
**	lie between inpdsp.cpp's and input.cpp's.
**	The key codes are Windows virtual keys plus E&B's own (0x60-0x69: the numeric keypad and the
**	extended keys share codes, so the later entry wins in the code -> name table); 'Keypad +' is
**	entered twice in the client.
**	Forms that matched (builder H19): the code -> name table keyed by int while the lookup takes an
**	unsigned int (0x42B7E0 calls the find 0x42CE80 out of line with a converted temporary of the
**	key, stored after the Init test); 0x42B610 returns the table's 32-bit value (an unsigned short
**	return loads a word). 0x42C330, between Init and the tables' instances, is the destructor of a
**	struct of std::strings at +0x00..+0x58 (0x7D3CC0 calls it): nothing in this file uses one, so it is not
**	reconstructed (a function the linker dropped may have; COMDAT placement).
*/
