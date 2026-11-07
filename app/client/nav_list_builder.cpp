// nav_list_builder.cpp: a navigation list the client builds itself (0x7B7BF0-); the rest of the object is not rebuilt; notes at the end
#include <string>
#include "nav_list_builder.h"
#include "sclient.h"
#include "co.h"
#include "..\shared\g_assert.h"

// @0x7B7BF0
NavListBuilderClass::NavListBuilderClass(SClient *client) :
	Client(client),
	Unknown1C(0)
{
}

// @0x7B7C60
NavListBuilderClass::~NavListBuilderClass()
{
}

// @0x7B7C80
bool NavListBuilderClass::Unknown_7B7C80(ClientObjectClass *from, ClientObjectClass *to)
{
	Clear();
	if (to) {





























































































































		FAIL_IF(!from) return(true);
		List.push_back(from->Get_Game_ID());
		List.push_back(to->Get_Game_ID());
	}
	return(true);
}

// @0x7B81E0
void NavListBuilderClass::Clear()
{
	NavigationListClass::Clear();
	Unknown1C++;
}

/*
**	Notes
**
**	The client's __FILE__ 0xB9EE4C; '!from' at line 150. The object runs to about 0x7B9300 (the path search, 0x7B7EF0
**	and 0x7B8210 on, is not rebuilt: gen_stubs stands in). mouse_targeting.cpp ends at 0x7B7B4F before it.
*/
