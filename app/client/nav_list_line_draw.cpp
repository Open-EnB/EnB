// Earth & Beyond client (reconstruction): the star map's navigation path lines (notes at the end)
#include "nav_list_line_draw.h"
#include "..\common\unknown_navlist.h"
#include "..\shared\g_assert.h"
#include "starmap_pip_support.h"
#include "nav_list_builder.h"
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


static const float NAV_LINE_Z = 0.05f;			// 0xB062E8 (read from memory by 0x7B99C0 and 0x7B9DC0: a const float, not a literal)  // NAME 1: what it is

// The path's line: starmap_pip_support.h's line with the path's sort level (table 0xB06320: only its deleting
// destructor 0x7B9C80 differs from 0xAE8754's; ~Line3DClass 0x7B9CB0 is emitted here for it)
class NavLineClass : public Unknown_466920Class  // NAME 1: neutral (D10): what it draws
{
public:
	NavLineClass(float width, const Vector3 &color, float opacity, int sort) :	// NAME 1 [width, color, opacity, sort]: the base's
		Unknown_466920Class(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, NAV_LINE_Z), width, color.X, color.Y, color.Z, opacity)
	{
		SortLevel = sort;
	}
};



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

// NOT EXACT: inline budget (0.69, attempts.py 0x7B99C0): the client calls Scene = NULL (0x7BA340) out of line inside the inlined 0x7B9CD0 and threads the size test's begin == 0 path; here Scene's release is inlined
// @0x7B99C0
void NavListLineDrawClass::Unknown_7B99C0(ClientObjectClass *avatar, NavListBuilderClass *path, const RefCountPtr<SceneClass> &scene, bool arg)
{
	Unknown_7B9CD0();
	Unknown04 = avatar;
	if (avatar) {
		Scene = scene;
		*List = *path;
		List->Set_Index(0);
		unsigned int count = List->List.size();
		if (count) {
			Unknown20 = 0.0f;
			CurLine = arg ? count : 0;
		}
		while (count--) {
			Lines.push_back(Create_NEW((Unknown_466920Class *)new NavLineClass(Width, Color, Opacity, Sort)));
		}
	}
}

#include "starmap_data.h"
#include "co_data.h"
#include "..\..\ww3d2\ww3d.h"
#include "sclient.h"
#include "..\audio\soundlib.h"











































// NOT EXACT: written from the pseudocode (D64), functional (0.72, attempts.py 0x7B9DC0): the client lays the object_list assert block before the shared clear tail and stores lines_drawn later
// @0x7B9DC0
void NavListLineDrawClass::Unknown_7B9DC0(int index)
{
	if (!Unknown04) {
		return;
	}
	FAIL_IF(!Client) return;
	std::vector<unsigned long> &object_list = List->List;
	int count = object_list.size();
	if (!count || (unsigned)index >= (unsigned)count) {
		Unknown_7B9CD0();
		return;
	}
	StarmapDataClass *sdata = Get_Starmap_Data(Unknown04, false);




	FAIL_IF(!sdata) {
		Unknown_7B9CD0();
		return;
	}
	Vector3 start = sdata->Get_Position();
	std::vector<RefCountPtr<Unknown_466920Class> >::iterator line = Lines.begin();
	int lines_drawn = 0;
	for (std::vector<unsigned long>::iterator id = object_list.begin(); id != object_list.end(); id++, line++) {
		bool draw = false;
		if (index != 0) {
			index--;
		} else if (lines_drawn <= CurLine) {
			draw = true;
		}









		FAIL_IF(line == Lines.end()) {
			Unknown_7B9CD0();
			return;
		}
		ClientObjectClass *co = Client->Find_Object(*id);
		if (!co) {
			Unknown_7B9CD0();
			return;
		}
		StarmapDataClass *data = Get_Starmap_Data(co, false);
		if (!data) {




			G_ASSERT(object_list.size() == 1);
			Unknown_7B9CD0();
			return;
		}
		Vector3 end = data->Get_Position();
		if (draw) {
			if (lines_drawn == CurLine) {
				Unknown20 += WW3D::Get_Frame_Time() * 0.002f;
				if (Unknown20 >= 1.0f) {
					CurLine++;
					Unknown20 = 0.0f;
					SoundLibraryClass::Play_2D_Sound(CurLine >= count ? "starmap_build_destination_1.wav" : "starmap_build_1.wav", 2);
				} else {
					end = (end - start) * Unknown20 + start;
				}
			} else {







				G_ASSERT(lines_drawn < CurLine);
			}
			start.Z = NAV_LINE_Z;
			(*line)->Reset(start, end);
			if (!(*line)->Unknown_In_Scene()) {
				(*line)->Add(Scene.Peek());
			}
			lines_drawn++;
			start = end;
		} else if ((*line)->Unknown_In_Scene()) {
			(*line)->Remove();
		}
	}
}

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
**	Exact: 0x7B97F0, 0x7B98E0, 0x7B9CD0, 0x7B9C80, 0x7B9CB0 (see the commits). NOT EXACT: 0x7B99C0 (it expands
**	0x7B9CD0, so that is defined first; the size test and the loop are separate statements: the client tests the
**	count twice; its line's constructor takes the width, the colour, the opacity and the sort, the two points its
**	own) and 0x7B9DC0 (it expands 0x7B9CD0 at each bail-out). Both read the line's height 0.05 from memory
**	(0xB062E8: mov / fld, never an immediate), so it is a const float, not a literal. Not rebuilt: what follows
**	(0x7BA2C0, 0x7BA340, 0x7BA380, ...: the POC and clienttest get them from generated stubs).
*/
