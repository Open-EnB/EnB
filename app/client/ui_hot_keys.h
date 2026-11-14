// Earth & Beyond client (reconstruction): app\client\ui_hot_keys.h, the space client's hot keys window (the client's
// ui_hot_keys.cpp per its FAIL_IFs, 'e:\build\beta\G\app\Client\ui_hot_keys.cpp' at 0xB80268; this header's name is
// provisional, NAME 1, D137). Laid out from the constructor 0x59C6F0 and the tables (O15, 2026-11-13).
#ifndef UI_HOT_KEYS_H  // NAME 1 [ui_hot_keys.h]: provisional header name for ui_hot_keys.cpp
#define UI_HOT_KEYS_H

#include "intrface.h"

class SClient;
class ControlKeysManager;

// The hot keys (0x84 bytes, tables 0xAF20D0 / 0xAF20C4 / 0xAF20B8; SClient::Init news it): an interface window with no
// members of its own whose keys 0..17 ('Autofollow' .. 'Target Self') move, fire and target in the main view
class UIHotKeysClass : public InterfaceClass	// NAME 3: class-from-file (D24) for app\Client\ui_hot_keys.cpp
{
public:
	UIHotKeysClass(SClient *client);					// 0x59C6F0  // NAME 1 [client]: what Init passes
	virtual ~UIHotKeysClass();							// 0x59C760 (deleting 0x59C730)
	virtual bool Unknown_68(int key);					// +0x68 0x59DB10 (FAIL_IFs '!Client' l.106, '!sclient' l.108, '!targeting' l.111)  // NAME 1 [key]: InterfaceClass's
	virtual void Unknown_70(ControlKeysManager *manager);	// +0x70 0x59C790 (the 18 keys added)  // NAME 1 [manager]: InterfaceClass's
	virtual void Unknown_74(ControlKeysManager *manager);	// +0x74 0x59D180 (the 18 keys removed)  // NAME 1 [manager]: InterfaceClass's
};

#endif
