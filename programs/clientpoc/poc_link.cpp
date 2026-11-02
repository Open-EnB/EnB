// Earth & Beyond client (reconstruction): the proof-of-concept client's hand stand-ins for symbols gen_stubs.py
// --link cannot write (a nested class parameter, a struct returned by value, an array of objects) (D179).
// NOT THE CLIENT'S CODE. Delete one here when its real function lands in app/ (the linker reports the duplicate).



#include "..\..\stubs\stub_log.h"
#include "..\..\app\client\unknown_signalbase.h"
#include "..\..\app\client\control_keys_manager.h"
#include "..\..\app\shared\shipcust_desc.h"

class INIClass;

// ---------------------------------------------------------------- the UI kit's SignalMap (intrface.cpp's view)

class InterfaceSignalMapClass
{
public:
	void Add(void *sender, SignalBaseClass::CallbackClass *callback);
	void Remove(void *sender, const SignalBaseClass::CallbackClass &callback);
};

// 0x4F9740 'SignalMap::connect': not kept (no gadget event reaches its interface yet)
void InterfaceSignalMapClass::Add(void *sender, SignalBaseClass::CallbackClass *callback)
{
	STUB_CALLED("POC InterfaceSignalMapClass::Add (0x4F9740, SignalMap::connect): not connected");
}

// 0x4F98D0 'SignalMap::disconnect'
void InterfaceSignalMapClass::Remove(void *sender, const SignalBaseClass::CallbackClass &callback)
{
	STUB_CALLED("POC InterfaceSignalMapClass::Remove (0x4F98D0, SignalMap::disconnect)");
}

// ---------------------------------------------------------------- input.cpp

// 0x42A1A0: a key combination read from an ini entry; here none
ControlKeyComboStruct Unknown_42A1A0(INIClass &ini, const char *section, const char *entry)
{
	STUB_CALLED("POC Unknown_42A1A0 (0x42A1A0, a key from an ini entry): none");
	return ControlKeyComboStruct(0, 0);
}

// ---------------------------------------------------------------- shipcust_desc.cpp's defaults

ShipCustSettingsClass Unknown_BE56D8[3][2];		// 0xBE56D8: each race's two default settings (three races assumed)
