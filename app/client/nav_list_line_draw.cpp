// Earth & Beyond client (reconstruction): the star map's navigation path lines (notes at the end)
#include "nav_list_line_draw.h"
#include "..\common\unknown_navlist.h"
#include "..\shared\g_assert.h"
#include "starmap_pip_support.h"
#include "..\..\ww3d2\scene.h"

// @0x7B97F0
NavListLineDrawClass::NavListLineDrawClass(SClient *client, float width, int sort, const Vector3 &color, float opacity) :
	Client(client),
	Unknown04(NULL),
	List(NULL),
	CurLine(0),
	Unknown20(0.0f),
	Color(color),
	Opacity(opacity),
	Width(width),
	Sort(sort)
{
	List = new NavigationListClass;
}


// @0x7B98E0
NavListLineDrawClass::~NavListLineDrawClass()
{
	delete List;
}


// 0x7B99C0 is not rebuilt yet (the lines are a Line3DClass subclass of starmap_pip_support.h's, table 0xB06320).


// @0x7B9CD0
void NavListLineDrawClass::Unknown_7B9CD0()
{
	Unknown04 = NULL;
	for (std::vector<RefCountPtr<Unknown_466920Class> >::iterator it = Lines.begin(); it != Lines.end(); it++) {
		if ((*it)->Unknown_In_Scene()) {
			(*it)->Remove();
		}
	}
	Lines.clear();
	Scene = NULL;
}

// 0x7B9DC0 is not rebuilt yet (FAIL_IF '!Client' on 137).

/*
**	Reconstruction notes
**
**	The object's boundaries in the client: __FILE__ 0xB9EF98 ('e:\build\beta\G\app\client\nav_list_line_draw.cpp'),
**	asserts on lines 137-210 in 0x7B9DC0. It starts at the constructor 0x7B97F0, right after nav_list_builder.cpp's
**	template instances (0x7B92A0-0x7B9780: its vector<NavLinkStruct> insert / fill / copy and the NavNodeStruct
**	pool's free, all called only from nav_list_builder.cpp's code); 0x7B9C80 / 0x7B9CB0 are Line3DClass's
**	deleting destructor and destructor, emitted here for 0x7B99C0's new line.
**
**	+0x0C is the vector's allocator byte: the constructor copies a stack byte into it (VC6's vector(const A &)).
**
**	Exact: 0x7B97F0, 0x7B98E0, 0x7B9CD0 (see the commit). Not rebuilt: 0x7B99C0, 0x7B9DC0 and what follows
**	(0x7BA2C0, 0x7BA340, 0x7BA380, ...: the POC and clienttest get them from generated stubs).
*/
