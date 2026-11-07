// Earth & Beyond client (reconstruction): the verb manager (notes at the end)
#include <algorithm>
#include "verbmanager.h"
#include "sclient.h"
#include "ui_verb.h"
#include "..\packets\pkverbrequest.h"
#include "..\shared\g_assert.h"

// 0x81F100
VerbManagerClass::VerbManagerClass(SClient *client) :
	Client(client)
{
}

// 0x81F1E0
VerbManagerClass::~VerbManagerClass()
{
	for (WindowMapType::iterator it = Windows.begin(); it != Windows.end(); ++it) {
		delete it->second;
	}
	Windows.clear();
}

// 0x81F560
void VerbManagerClass::Unknown_81F560(int game_id, UIVerbClass *window)
{
	Unknown_81F890(game_id, window);
	Client->Send_Packet(&VerbRequestPacketClass(Client->Get_Avatar_ID(), game_id, 0));
}

// 0x81F610
void VerbManagerClass::Unknown_81F610(int game_id, UIVerbClass *window)
{
	Unknown_81F890(game_id, window);
	Client->Send_Packet(&VerbRequestPacketClass(Client->Get_Avatar_ID(), game_id, 1));
}

// 0x81F6A0
void VerbManagerClass::Unknown_81F6A0(int game_id, UIVerbClass *window)
{
	Unknown_81FA40(game_id, window);
	Client->Send_Packet(&VerbRequestPacketClass(Client->Get_Avatar_ID(), game_id, 2));
}

// 0x81F780
void VerbManagerClass::Unknown_81F780(int game_id, const std::set<VerbEnum> *verbs)
{
	WindowMapType::iterator found = Windows.find(game_id);
	if (found != Windows.end()) {
		WindowListType *windows = (*found).second;
		for (WindowListType::iterator it = windows->begin(); it != windows->end(); it++) {
			(*it)->Unknown_658F40(verbs);
			windows->erase(it);
		}
	}
}

// 0x81F810
void VerbManagerClass::Unknown_81F810(int game_id, const std::set<VerbEnum> *adds, const std::set<VerbEnum> *drops)
{
	WindowMapType::iterator found = Windows.find(game_id);
	if (found != Windows.end()) {
		WindowListType *windows = (*found).second;
		for (WindowListType::iterator it = windows->begin(); it != windows->end(); it++) {
			(*it)->Unknown_659010(adds, drops);
		}
		windows->size();
	}
}

























































// 0x81F890
bool VerbManagerClass::Unknown_81F890(int game_id, UIVerbClass *window)
{
	WindowMapType::iterator found;
	WindowListType *windows;
	if ((found = Windows.find(game_id)) == Windows.end()) {
		windows = new WindowListType;
		Windows[game_id] = windows;
	} else {
		windows = (*found).second;
	}

	if (std::find(windows->begin(), windows->end(), window) == windows->end()) {
		windows->push_back(window);
		return(true);
	}
	G_ASSERT(false);
	return(false);
}

// 0x81FA40
bool VerbManagerClass::Unknown_81FA40(int game_id, UIVerbClass *window)
{
	WindowMapType::iterator found = Windows.find(game_id);
	if (found != Windows.end()) {
		WindowListType *windows = (*found).second;
		WindowListType::iterator it = std::find(windows->begin(), windows->end(), window);
		if (it != windows->end()) {
			windows->erase(it);
			windows->size();
			return(true);
		}
	}
	return(false);
}

/*
**	Reconstruction notes (VB1).
**
**	The client's __FILE__ e:\build\beta\G\app\Client\verbmanager.cpp (0xBA8268) is 0x81F890's 'false' assert
**	(line 143). The object runs from the constructor 0x81F100 (0x14 bytes, SClient::Init) through 0x81FA40 and
**	its map / list instances up to 0x8207D0: 0x81F560 / 0x81F610 / 0x81F6A0 all call 0x81F890 or its partner, send
**	VerbRequestPacketClass (whose destructor 0x81F5F0 lands among them) and 0x81F6A0 inlines 0x81FA40.
**
**	0x81F780 walks with erase(it) then it++, the client's own use of the freed node (0x81F7DE reads [esi] after
**	the delete at 0x81F7CC). The dead windows->size() in 0x81F810 and 0x81FA40 emit nothing: they spend the inline
**	budget that leaves _Tree::find (0x81F810) and list::erase (0x81F6A0's inlined copy of 0x81FA40) out of line.
**	The assignment inside 0x81F890's if is what lets the new's temporary share found's slot (+0xC).
**
**	92's sets: 0x81F810 passes (adds, drops) as VERB_UPDATE gives them. 0x659010 takes the +0x10 set first: each verb
**	goes to its existing button (assert 0x1AC if none), a low word of 0 made 0xFFFF, so 0x659AC0 always greys it. The
**	+0x20 set then goes through 0x659240, as VERB_LIST's verbs do, which makes the button if it is missing. So the
**	names from Print hold: Drops (+0x10) greys buttons, Adds (+0x20) makes or sets them.
*/
