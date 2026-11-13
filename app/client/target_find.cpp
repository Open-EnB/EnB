// target_find.cpp: the cycling target finder (0x7C3EC0-0x7C509F); notes at the end of the file.
#include "target_find.h"
#include "sclient.h"
#include "client.h"
#include "co.h"
#include "co_data.h"
#include "radar_data.h"
#include "..\shared\g_assert.h"
#include "..\shared\auxdata\auxdata.h"
#include "aux_roots.h"
#include <algorithm>

#include "distance_calc_client.h"	// 0x7E6060 / 0x7E6130
bool Unknown_70B3D0(ClientObjectClass *object);								// 0x70B3D0 (co_player_relationship.cpp)

// @0x7C3F10
TargetFindClass::TargetFindClass(SClient *client) :
	Client(client),
	Filter(0)
{
}

// @0x7C3F40
ClientObjectClass *TargetFindClass::Select(ClientObjectClass *co)
{
	return(co);
}

// @0x7C3F90
TargetFindClass::~TargetFindClass()
{
}

// @0x7C3FB0
ClientObjectClass *TargetFindClass::Select_Next(ClientObjectClass *co)
{
	Filter = 0;
	return(Select(Find_Next(co)));
}

// @0x7C3FF0
ClientObjectClass *TargetFindClass::Find_Next(ClientObjectClass *co)
{
	std::vector<TargetFindEntryStruct> list;
	Build_List(list);
	if (list.empty()) {
		return(co);
	}
	if (co) {
		std::vector<TargetFindEntryStruct>::iterator itor = std::find(list.begin(), list.end(), co);
		if (itor != list.end()) {
			itor++;
			if (itor != list.end()) {
				return(itor->Object);
			}
		}
	}
	return(list.begin()->Object);
}

// @0x7C40E0
ClientObjectClass *TargetFindClass::Select_Prev(ClientObjectClass *co)
{
	Filter = 0;
	return(Select(Find_Prev(co)));
}

// @0x7C4120
ClientObjectClass *TargetFindClass::Find_Prev(ClientObjectClass *co)
{
	std::vector<TargetFindEntryStruct> list;
	Build_List(list);
	if (list.empty()) {
		return(co);
	}
	std::vector<TargetFindEntryStruct>::iterator itor;
	if (co) {
		itor = std::find(list.begin(), list.end(), co);
	} else {
		itor = list.end();
	}
	if (itor == list.begin()) {
		itor = list.end();
	}
	itor--;
	return(itor->Object);
}

// @0x7C4210
ClientObjectClass *TargetFindClass::Select_Next_Filtered(ClientObjectClass *co)
{
	return(Select(Find_Next(co)));
}

// @0x7C4240
ClientObjectClass *TargetFindClass::Select_Prev_Filtered(ClientObjectClass *co)
{
	return(Select(Find_Prev(co)));
}

// @0x7C4270
ClientObjectClass *TargetFindClass::Select_Nearest_Avatar()
{
	Filter = TARGET_FIND_AVATAR;
	return(Select(Find_Next(Find_Next(NULL))));
}

// @0x7C42B0
ClientObjectClass *TargetFindClass::Select_Nearest_Attacking()
{
	Filter = TARGET_FIND_ATTACKING;
	return(Select(Find_Next(NULL)));
}

// @0x7C42E0
ClientObjectClass *TargetFindClass::Select_Nearest_100()
{
	Filter = TARGET_FIND_UNKNOWN_100;
	return(Select(Find_Next(NULL)));
}

// @0x7C4310
ClientObjectClass *TargetFindClass::Select_Nearest_200()
{
	Filter = TARGET_FIND_UNKNOWN_200;
	return(Select(Find_Next(NULL)));
}

// @0x7C4340
ClientObjectClass *TargetFindClass::Select_Nearest_Navigation()
{
	Filter = TARGET_FIND_NAVIGATION;
	return(Select(Find_Next(NULL)));
}

// @0x7C4370
ClientObjectClass *TargetFindClass::Select_Nearest_Neutral()
{
	Filter = TARGET_FIND_NEUTRAL;
	return(Select(Find_Next(NULL)));
}

// @0x7C43A0
ClientObjectClass *TargetFindClass::Select_Group_Member(unsigned idx)
{
	Filter = TARGET_FIND_GROUP;


































	FAIL_IF(idx >= MAX_VISIBLE_GROUP_MEMBERS) return(NULL);
	NameClass name("GroupInfo");
	DeltaGroupInfoClass *group = (DeltaGroupInfoClass *)Unknown_Find_Field<DeltaCompoundClass>(Client->AuxData, name);
	if (group && group->Unknown70) {
		int id = ((DeltaGroupMemberClass *)group->Members.Children[idx])->GameID.Value;
		if (id) {
			return(Select(Client->Find_Object(id)));
		}
	}
	return(NULL);
}

// @0x7C44B0
ClientObjectClass *TargetFindClass::Select_Player()
{
	Filter = Client->Unknown_74D570() ? TARGET_FIND_GROUP : TARGET_FIND_AVATAR;
	return(Select(Client->Get_Avatar()));
}

// @0x7C44F0
ClientObjectClass *TargetFindClass::Select_And_Set_Filter(ClientObjectClass *co)
{
	if (co) {
		Filter = Get_Flags(co);
	}
	return(Select(co));
}

// @0x7C4530
void TargetFindClass::Build_List(std::vector<TargetFindEntryStruct> &list)
{
	ClientObjectClass *player = Client->Get_Avatar();



	FAIL_IF(!player) return;
	for (HashNodeClass<ClientObjectClass *> *node = Client->ObjectList.First_Valid(); node; node = node->Next_Valid()) {
		ClientObjectClass *co = node->Get_Record();
		if (co) {
			if (co->Unknown_6F5980()) {
				if (Filter ? Match_Filter(co) : true) {
					TargetFindEntryStruct entry;
					entry.Object = co;
					float distance = Unknown_7E6130(player, co);
					entry.Distance = distance;
					list.push_back(entry);
				}
			}
		} else {







			G_ASSERT(co);
		}
	}
	std::sort(list.begin(), list.end());
}

// @0x7C4940
unsigned TargetFindClass::Get_Flags(ClientObjectClass *co)
{



	FAIL_IF(!co) return(0);
	unsigned flags = 0;
	if (co->Relationship.IsAttacking) {
		flags = TARGET_FIND_ATTACKING;
	}
	if (co->Type == 1) {
		flags |= TARGET_FIND_AVATAR;
		if (co->GameID == Client->AvatarID) {
			flags |= TARGET_FIND_SELF;
			if (Client->Unknown_74D570()) {
				flags |= TARGET_FIND_GROUP;
			}
		}
		RadarDataClass *radar = Get_Radar_Data(co, true);
		if (radar && radar->Unknown78) {
			flags |= TARGET_FIND_GROUP;
		}
		if (Client->Unknown_74D780(co)) {
			flags |= TARGET_FIND_UNKNOWN_100;
		}
	} else {
		if (Get_Navigation_Data(co, false)) {
			flags |= TARGET_FIND_NAVIGATION;
		} else if (Unknown_70B3D0(co)) {
			flags |= TARGET_FIND_NEUTRAL;
		} else {
			flags |= TARGET_FIND_NOT_AVATAR;
			if (co->Relationship.Reaction >= 0 && co->Relationship.Reaction <= 1) {
				flags |= TARGET_FIND_UNKNOWN_100;
			} else {
				flags |= TARGET_FIND_UNKNOWN_200;
			}
		}
	}
	return(flags);
}

/*
**	Notes
**
**	The object is 0x7C3EC0-0x7C509F, placed by the __FILE__ 0xB9F7D4 pushed at 0x7C43B7, 0x7C4549,
**	0x7C45DE and 0x7C494C (asserts.txt lists 0xB9F7AC under starmap_pip_support.cpp, wrongly).
**	Its start: 0x7C3E50 is the previous object's closing <string> initialiser (guard 0xC416FC), and
**	0x7C3EC0 / 0x7C3EF0 are this object's two header statics (a PermStringInitClass at 0xBE87DC and
**	its atexit, $E70 / $E68 here, 22 and 10 bytes), too short for the matcher. The class's table is
**	0xB07008; 0x7C3F60 is the scalar deleting destructor (objmatch: 'multiple').
**	Its end: its own <string> initialiser is 0x7C4DE0 ($E136), then the COMDATs of std::sort:
**	0x7C4E30 _Sort, 0x7C4FF0 _Unguarded_insert, 0x7C5040 _Unguarded_partition. 0x7C50B0 / 0x7C50E0 (a PermStringInitClass at
**	0xBE87E0 and its atexit) open the next object, target_reticle.cpp. TargetingClass (targeting.cpp, 0x7C6120, table
**	0xB07134, SClient +0x12A4) derives from this class.
**
**	Forms the bytes decide (Build_List 0x7C4530): 'Filter ? Match_Filter(co) : true' keeps the
**	client's sete / test (|| folds them); a float local keeps the fstp after the pushes; the entry's
**	empty constructor gives std::_Sort 0x7C4E30's temporaries (without one it falls on 0x75EA60's
**	form); std::find with a pointer operator== is Find_Next's and Find_Prev's loop.
*/
