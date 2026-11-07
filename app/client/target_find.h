/*
**	target_find.h: the cycling target finder (app\client\target_find.cpp).
**
**	The client has no name for the class. Its one known derived class is the object at
**	SClient +0x12A4 (constructor 0x7C6120, vtable 0xB07134; ui_group.cpp's FAIL_IF calls that
**	pointer 'targeting'), whose slot +0x04 receives every object found here.
*/

#ifndef __TARGET_FIND_H  // NAME 3 [target_find.h]: header name assumed from the client's target_find.cpp
#define __TARGET_FIND_H

#include <vector>

class SClient;
class ClientObjectClass;

#ifndef MAX_VISIBLE_GROUP_MEMBERS
#define MAX_VISIBLE_GROUP_MEMBERS	5	// as ui_group.h defines it (the client's FAIL_IF 'idx >= MAX_VISIBLE_GROUP_MEMBERS' compares with 5, 0x7C43AB)
#endif

// What 0x7C4940 says about an object; a filter asks for all of its bits
enum TargetFindFlagEnum	// NAME 1: neutral (D10): the bits' names are what 0x7C4940 tests
{
	TARGET_FIND_AVATAR = 0x02,			// NAME 2: Type == 1
	TARGET_FIND_NOT_AVATAR = 0x04,		// NAME 2: Type != 1, no navigation data, not Neutral's type
	TARGET_FIND_NAVIGATION = 0x08,		// NAME 2: has navigation data (Get_Navigation_Data)
	TARGET_FIND_NEUTRAL = 0x10,			// NAME 2: Unknown_70B3D0
	TARGET_FIND_SELF = 0x20,			// NAME 2: GameID == the client's AvatarID
	TARGET_FIND_GROUP = 0x40,			// NAME 1: neutral: self in a group, or radar data +0x78; what 0x7C43A0 sets
	TARGET_FIND_ATTACKING = 0x80,		// NAME 2: Relationship.IsAttacking
	TARGET_FIND_UNKNOWN_100 = 0x100,	// NAME 1: neutral (D10): Unknown_74D780, or a Reaction of 0 or 1
	TARGET_FIND_UNKNOWN_200 = 0x200,	// NAME 1: neutral (D10): a non-avatar with any other Reaction
};

// One entry of the list 0x7C4530 sorts: the object and how far it is from the player
struct TargetFindEntryStruct	// NAME 1: neutral (D10)
{
	ClientObjectClass *Object;			// +0x00  // NAME 2: what it holds
	float Distance;						// +0x04 (0x7E6130, distance_calc_client.cpp)  // NAME 2: what it holds

	TargetFindEntryStruct() {}				// (a constructor decides 0x7C4E30's temporaries: without one std::_Sort falls on 0x75EA60's form)

	bool operator<(const TargetFindEntryStruct &that) const { return(Distance < that.Distance); }
	bool operator==(ClientObjectClass *co) const { return(co == Object); }
};

class TargetFindClass	// NAME 3: class-from-file (D24) for target_find.cpp
{
public:
	TargetFindClass(SClient *client);								// 0x7C3F10
	virtual ~TargetFindClass();										// +0x00 0x7C3F60 (deleting), 0x7C3F90
	virtual ClientObjectClass *Select(ClientObjectClass *co);		// +0x04 0x7C3F40: returns co; the derived class overrides it  // NAME 1: neutral by what callers hand it

	ClientObjectClass *Select_Next(ClientObjectClass *co);			// 0x7C3FB0 (no filter)  // NAME 2: what it does
	ClientObjectClass *Find_Next(ClientObjectClass *co);			// 0x7C3FF0: the object after co, or the nearest  // NAME 2: what it does
	ClientObjectClass *Select_Prev(ClientObjectClass *co);			// 0x7C40E0 (no filter)  // NAME 2: what it does
	ClientObjectClass *Find_Prev(ClientObjectClass *co);			// 0x7C4120: the object before co, or the farthest  // NAME 2: what it does
	ClientObjectClass *Select_Next_Filtered(ClientObjectClass *co);	// 0x7C4210 (the filter as it stands)  // NAME 2: what it does
	ClientObjectClass *Select_Prev_Filtered(ClientObjectClass *co);	// 0x7C4240 (the filter as it stands)  // NAME 2: what it does
	ClientObjectClass *Select_Nearest_Avatar();						// 0x7C4270 (the second nearest avatar: the nearest is the player)  // NAME 2: what it does
	ClientObjectClass *Select_Nearest_Attacking();					// 0x7C42B0  // NAME 2: what it does
	ClientObjectClass *Select_Nearest_100();						// 0x7C42E0  // NAME 1: neutral (D10)
	ClientObjectClass *Select_Nearest_200();						// 0x7C4310  // NAME 1: neutral (D10)
	ClientObjectClass *Select_Nearest_Navigation();					// 0x7C4340  // NAME 2: what it does
	ClientObjectClass *Select_Nearest_Neutral();					// 0x7C4370  // NAME 2: what it does
	ClientObjectClass *Select_Group_Member(unsigned idx);			// 0x7C43A0 (0x5946D0, 0x59DB10)  // NAME 3 [idx]: the FAIL_IF's  // NAME 2: what it does
	ClientObjectClass *Select_Player();								// 0x7C44B0: the player; the group's, or the avatars', filter  // NAME 2: what it does
	ClientObjectClass *Select_And_Set_Filter(ClientObjectClass *co);	// 0x7C44F0: co; further cycling stays among its kind  // NAME 2: what it does

protected:
	bool Match_Filter(ClientObjectClass *co) { unsigned flags = Get_Flags(co); return((Filter & flags) == Filter); }	// NAME 2: what it does
	void Build_List(std::vector<TargetFindEntryStruct> &list);		// 0x7C4530: the filter's objects, sorted by distance  // NAME 2: what it does
	unsigned Get_Flags(ClientObjectClass *co);						// 0x7C4940 (FAIL_IF '!co')  // NAME 2: what it does

	SClient *Client;												// +0x04  // NAME 2: what it holds
	unsigned Filter;												// +0x08 TargetFindFlagEnum bits, 0 for all  // NAME 2: what it holds
};

#endif //__TARGET_FIND_H
