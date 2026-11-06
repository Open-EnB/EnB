// Earth & Beyond client (reconstruction): ui_verb.cpp's verb window, as far as verbmanager.cpp calls it
#if defined(_MSC_VER)
#pragma once
#endif

#ifndef UI_VERB_H  // NAME 3: the header of ui_verb.cpp (the client's __FILE__ 0xB8C43C)
#define UI_VERB_H

#include <set>
#include "..\common\unknown_verb.h"	// VerbEnum

class UIVerbClass		// NAME 3: class-from-file (D24) for ui_verb.cpp (0x658F40's and 0x659010's asserts)
{
public:
	void Unknown_658F40(const std::set<VerbEnum> *verbs);	// 0x658F40 (for each verb 0x659240 makes or sets its button)  // NAME 1 [verbs]: VERB_LIST's field
	void Unknown_659010(const std::set<VerbEnum> *adds, const std::set<VerbEnum> *drops);	// 0x659010 (drops first: 0x659AC0 greys each one's button; then adds as 0x658F40)  // NAME 1 [adds, drops]: VERB_UPDATE's fields
};

#endif
