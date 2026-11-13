// nav_list_builder.cpp: a navigation list the client builds itself, the path search (0x7B7B60-0x7B9800); notes at the end
#include <string>
#include "nav_list_builder.h"
#include "sclient.h"
#include "co.h"
#include "co_data.h"
#include "navigation_data.h"
#include "mempool.h"
#include "..\shared\g_assert.h"

#include "..\common\distance_calc.h"	// Unknown_832010 0x832010: bit 0 within radius1, bit 1 within radius2

// One link of a node of the search
struct NavLinkStruct	// NAME 1: neutral; the assert's 'link'
{
	NavLinkStruct(NavNodeStruct *node, float distance) : Node(node), Distance(distance) {}
	NavLinkStruct(const NavLinkStruct &src) : Node(src.Node), Distance(src.Distance) {}
	NavNodeStruct *Node;			// +0x00 (the FAIL_IF's name 'link.Node')
	float Distance;					// +0x04  // NAME 2: what it holds
};

// A node of the search (0x24 bytes, pooled: 0x7B96A0, 0xBE8738)
class NavNodeStruct : public AutoPoolClass<NavNodeStruct, 16>	// NAME 1: neutral; the asserts' 'nav_points'
{
public:
	NavNodeStruct(ClientObjectClass *co, float radius) : Object(co), Radius(radius), Distance(0), Previous(NULL) {}

	ClientObjectClass *Object;		// +0x00  // NAME 2: what it holds
	float Radius;					// +0x04  // NAME 2: what it holds
	std::vector<NavLinkStruct> Links;	// +0x08  // NAME 2: what it holds
	float Distance;					// +0x18  // NAME 2: what it holds
	int Unknown1C;					// +0x1C  // NAME 1: neutral
	NavNodeStruct *Previous;		// +0x20  // NAME 2: what it holds
};

DEFINE_AUTO_POOL(NavNodeStruct, 16);

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

// NOT EXACT: order only (the loop's exit reloads co_from before to; ours to first)
// @0x7B7EF0
bool NavListBuilderClass::Unknown_7B7EF0(float range, ClientObjectClass *co_from, ClientObjectClass *to)
{
	Clear();
	if (to) {

		FAIL_IF(!co_from) return(true);
		if (!Get_Navigation_Data(to, false)) {
			List.push_back(to->Get_Game_ID());
		} else {
			bool found;
			do {
				float search_range = range;
				std::vector<NavNodeStruct *> nav_points;
				Build_Nodes(nav_points, co_from, search_range, to);






















				FAIL_IF(nav_points.size() < 2) return(false);
				Link_Nodes(nav_points, co_from->Unknown_30(), search_range);
				std::vector<NavNodeStruct *> result;
				found = Find_Path(nav_points, result);
				if (found) {
					Add_Path(result);
				} else if (range > 0.0f) {
					range += range;
				} else {
					range = 1.0f;
				}
				Delete_Nodes(nav_points);
			} while (!found);
		}
	}
	return(true);
}

// @0x7B81E0
void NavListBuilderClass::Clear()
{
	NavigationListClass::Clear();
	Unknown1C++;
}

// @0x7B8210
void NavListBuilderClass::Build_Nodes(std::vector<NavNodeStruct *> &nav_points, ClientObjectClass *from, float range, ClientObjectClass *dest)
{




















































	FAIL_IF(!nav_points.empty()) return;
	FAIL_IF(!Client) return;
	FAIL_IF(!from) return;
	FAIL_IF(!dest) return;
	NavigationDataClass *nav = Get_Navigation_Data(dest, false);
	float radius;
	if (nav) {
		float extra = nav->Unknown08;
		radius = dest->Unknown_30() + extra;
	} else {
		radius = dest->Unknown_30();
	}
	Vector3 pos = dest->Unknown_24();
	nav_points.push_back(new NavNodeStruct(from, range));
	nav_points.push_back(new NavNodeStruct(dest, radius));
	HashNodeClass<ClientObjectClass *> *node = Client->ObjectList.First_Valid();
	while (node) {
		HashNodeClass<ClientObjectClass *> *cur = node;
		node = cur->Next_Valid();
		ClientObjectClass *co = cur->Get_Record();




		FAIL_IF(!co) continue;
		if (co != dest) {
			NavigationDataClass *co_nav = Get_Navigation_Data(co, false);
			if (co_nav && co_nav->Unknown10 == 1 && Get_Starmap_Data(co, false)) {
				nav_points.push_back(new NavNodeStruct(co, co_nav->Unknown08));
			}
		}
	}
}

// @0x7B8510
void NavListBuilderClass::Link_Nodes(std::vector<NavNodeStruct *> &nav_points, float range1, float range2)
{


























	FAIL_IF(nav_points.size() < 2) return;
	float distance = 0.0f;
	int index = 0;
	for (std::vector<NavNodeStruct *>::iterator it = nav_points.begin(); it != nav_points.end(); ++it) {
		std::vector<NavNodeStruct *>::iterator next = it + 1;
		NavNodeStruct *node1 = *it;
		for (std::vector<NavNodeStruct *>::iterator other = next; other != nav_points.end(); ++other) {
			NavNodeStruct *node2 = *other;
			int flags = Unknown_832010(node1->Object->Unknown_24(), node1->Radius, node2->Object->Unknown_24(), node2->Radius, range1, range2, &distance);
			if (index != 1 && (flags & 2) == 2) {
				node1->Links.push_back(NavLinkStruct(node2, distance));
			}
			if (index != 0 && (flags & 1) == 1) {
				node2->Links.push_back(NavLinkStruct(node1, distance));
			}
		}
		index++;
	}
}

// @0x7B8880
bool NavListBuilderClass::Find_Path(std::vector<NavNodeStruct *> &nav_points, std::vector<NavNodeStruct *> &result)
{




























































	FAIL_IF(!result.empty()) result.clear();
	FAIL_IF(nav_points.size() < 2) return(false);
	NavNodeStruct *from = nav_points[0];

	FAIL_IF(!from) return(false);
	NavNodeStruct *dest = nav_points[1];

	FAIL_IF(!dest) return(false);
	std::vector<NavNodeStruct *> next;
	std::vector<NavNodeStruct *> current;
	current.push_back(from);
	while (!current.empty()) {
		for (std::vector<NavNodeStruct *>::iterator it = current.begin(); it != current.end(); ++it) {
			NavNodeStruct *node = *it;
			for (std::vector<NavLinkStruct>::iterator l = node->Links.begin(); l != node->Links.end(); ++l) {
				NavLinkStruct &link = *l;



















				FAIL_IF(link.Node == from) continue;
				float distance = node->Distance + link.Distance;
				if (link.Node->Previous != NULL) {
					if (distance < link.Node->Distance) {
						link.Node->Previous = node;
						link.Node->Distance = distance;
					}
				} else {
					link.Node->Previous = node;
					link.Node->Distance = distance;
					next.push_back(link.Node);
				}
			}
		}
		std::swap(next, current);
		next.clear();
	}
	while (dest->Previous) {
		result.insert(result.begin(), dest);
		dest = dest->Previous;
	}
	return(result.size() != 0);
}

// @0x7B8BF0
void NavListBuilderClass::Add_Path(std::vector<NavNodeStruct *> &path)
{
	for (std::vector<NavNodeStruct *>::iterator it = path.begin(); it != path.end(); ++it) {
		NavNodeStruct *node = *it;
		List.push_back(node->Object->Get_Game_ID());
	}
}

// @0x7B8E40
void NavListBuilderClass::Delete_Nodes(std::vector<NavNodeStruct *> &nav_points)
{
	for (std::vector<NavNodeStruct *>::iterator it = nav_points.begin(); it != nav_points.end(); ++it) {
		delete *it;
	}
}

/*
**	Notes
**
**	The client's __FILE__ 0xB9EE4C. The object opens with the node pool's initialiser (0x7B7B60, atexit 0x7B7BA0);
**	mouse_targeting.cpp ends at 0x7B7B4F before it.
**	It ends with its template instances (0x7B92A0-0x7B9780); nav_list_line_draw.cpp starts at 0x7B97F0. NavLinkStruct's
**	copy constructor is declared: vector<NavLinkStruct>'s insert / _Ucopy / _Ufill (0x7B92A0, 0x7B95E0, 0x7B9630) copy
**	it member by member only with it (without it, the three are generic copies that match other files' instances).
*/
