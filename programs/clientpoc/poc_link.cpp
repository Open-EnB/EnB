// Earth & Beyond client (reconstruction): the proof-of-concept client's hand stand-ins for symbols gen_stubs.py
// --link cannot write (a nested class parameter, a struct returned by value, an array of objects) (D179).
// NOT THE CLIENT'S CODE. Delete one here when its real function lands in app/ (the linker reports the duplicate).



#include "..\..\stubs\stub_log.h"
#include "..\..\app\client\unknown_signalbase.h"
#include "..\..\app\client\control_keys_manager.h"
#include "..\..\app\shared\shipcust_desc.h"

class INIClass;

// ---------------------------------------------------------------- the UI kit's SignalMap (intrface.cpp's view)
// Replaced by real SignalMap in app/client/signal_map.cpp (0x4F7E00-0x4F9D00).

// ---------------------------------------------------------------- input.cpp

// 0x42A1A0: a key combination read from an ini entry: control_keys_manager.cpp's name for inpdsp.cpp's reader
// (it declares the 16-byte result as a ControlKeyComboStruct); the real event, through poc_standins.cpp
void Poc_Read_Ini_Key(INIClass &ini, const char *section, const char *entry, void *out);
ControlKeyComboStruct Unknown_42A1A0(INIClass &ini, const char *section, const char *entry)
{
	ControlKeyComboStruct combo(0, 0);
	Poc_Read_Ini_Key(ini, section, entry, &combo);
	return combo;
}

// ---------------------------------------------------------------- shipcust_desc.cpp's defaults

ShipCustSettingsClass Unknown_BE56D8[3][2];		// 0xBE56D8: each race's two default settings (three races assumed)
