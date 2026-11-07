// Earth & Beyond client (reconstruction): the verb manager (verbmanager.cpp)
#if defined(_MSC_VER)
#pragma once
#endif

#ifndef VERBMANAGER_H  // NAME 3: the header of verbmanager.cpp (the client's __FILE__ 0xBA8268)
#define VERBMANAGER_H

#include <list>
#include <map>
#include <set>
#include "..\common\unknown_verb.h"	// VerbEnum

class SClient;
class UIVerbClass;					// ui_verb.h

// 0x14 bytes: SClient::Init creates it (+0x1294), the verb packets update it. For each object's
// game id, the verb windows that asked for its verbs; each is told what the server answers.
class VerbManagerClass		// NAME 3: class-from-file (D24) for verbmanager.cpp (constructor 0x81F100, 0x14 bytes)
{
public:
	VerbManagerClass(SClient *client);					// 0x81F100
	~VerbManagerClass();								// 0x81F1E0
	void Unknown_81F560(int game_id, UIVerbClass *window);	// 0x81F560 (no callers; sends VerbRequestPacketClass action 0)  // NAME 1 [game_id, window]: what 0x81F890 and the packet take
	void Unknown_81F610(int game_id, UIVerbClass *window);	// 0x81F610 (ui_verb.cpp 0x658C90 passes its target's +0x90; action 1)  // NAME 1 [game_id, window]: as 0x81F560
	void Unknown_81F6A0(int game_id, UIVerbClass *window);	// 0x81F6A0 (ui_verb.cpp 0x658C90 for the old target; action 2)  // NAME 1 [game_id, window]: as 0x81F560
	void Unknown_81F780(int game_id, const std::set<VerbEnum> *verbs);	// 0x81F780 (cpacket.cpp: VERB_LIST 91 passes its GameID, &Verbs)  // NAME 1 [game_id, verbs]: the packet's fields
	void Unknown_81F810(int game_id, const std::set<VerbEnum> *adds, const std::set<VerbEnum> *drops);	// 0x81F810 (cpacket.cpp: VERB_UPDATE 92 passes its GameID, &Adds, &Drops: +0x20 then +0x10)  // NAME 1 [game_id, adds, drops]: the packet's fields
	bool Unknown_81F890(int game_id, UIVerbClass *window);	// 0x81F890 (adds the window to the game id's list; 'false' assert if it is there)  // NAME 1 [game_id, window]: as 0x81F560
	bool Unknown_81FA40(int game_id, UIVerbClass *window);	// 0x81FA40 (no callers; takes the window off the list)  // NAME 1 [game_id, window]: as 0x81F560

	typedef std::list<UIVerbClass *> WindowListType;				// NAME 1: neutral (D10); list nodes hold the window at +8
	typedef std::map<int, WindowListType *> WindowMapType;			// NAME 1: neutral (D10); 0x81F100's map, nodes 0x18 bytes

	SClient *Client;				// +0x00 the constructor's argument  // NAME 2: what it holds
	WindowMapType Windows;			// +0x04 (0x10 bytes)  // NAME 1: neutral (D10), what the lists hold
};

#endif
