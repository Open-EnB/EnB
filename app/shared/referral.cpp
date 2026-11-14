// Earth & Beyond client (reconstruction): app\shared\referral.cpp, the referral rewards (client 0x49EF10-0x49FA7F).
// Notes at the end.
#include "referral.h"
#include "..\..\wwlib\ini.h"
#include <stdio.h>

BuddyRewardStruct::BuddyRewardStruct(int level, INIClass &ini)
{
	RewardLevel = 0;
	SkillBoost = 0;
	CreditBoost = 0;
	char section[20];
	_snprintf(section, sizeof(section), "RewardLevel%d", level);
	if (ini.Find_Section(section)) {
		RewardLevel = ini.Get_Int(section, "RewardLevel", 0);
		SkillBoost = ini.Get_Int(section, "SkillBoost", 0);
		CreditBoost = ini.Get_Int(section, "CreditBoost", 0);
	}
}

Unknown49F200Class::Unknown49F200Class()
{
	INIClass ini("referral.ini");
	if (ini.Is_Loaded()) {
		for (int i = 0; i < 10; i++) {
			BuddyRewardStruct reward(i, ini);
			if (reward.RewardLevel) {
				Rewards.push_back(reward);
			}
		}
	}
}

Unknown49F200Class::~Unknown49F200Class()
{
}

Unknown49F200Class *Unknown_49F200()
{
	static Unknown49F200Class rewards;	// NAME 1 [rewards]: neutral
	return &rewards;
}

const BuddyRewardStruct &Unknown49F200Class::Unknown_49F270(int level)
{
	if (level == 0) {
		return Rewards[0];
	}
	for (unsigned int i = 0; i < Rewards.size(); i++) {
		if (Rewards[i].RewardLevel >= level) {
			return Rewards[i];
		}
	}
	return Rewards.back();
}

std::vector<BuddyRewardStruct> Unknown49F200Class::Unknown_49F300(int points_spent, int points)
{
	std::vector<BuddyRewardStruct> rewards;
	if (points == 0) {
		return rewards;
	}
	for (int i = 1; i <= points; i++) {
		BuddyRewardStruct reward = Unknown_49F270(points_spent + i);
		rewards.push_back(reward);
	}
	return rewards;
}

// Reconstruction notes (O25, 2026-11-14)
// - The object: no __FILE__ and no assert of its own. It opens at 0x49EF10, after observer.cpp's
//   DynamicVectorClass<ObserverClass *> members (to 0x49EEE0), and ends with the element's copy 0x49FA20 and
//   deleting destructor 0x49FA50, before selector.cpp's static initialiser 0x49FA80. The strings: 'referral.ini'
//   (0xB72CC4), the section 'RewardLevel%d' and the keys 'RewardLevel', 'SkillBoost', 'CreditBoost' (0xB72C84-0xB72CB4),
//   which name the element's fields. The file's name is not the client's: 'referral.cpp' (NAME 1, from referral.ini)
//   fits the link order between observer.cpp and selector.cpp.
// - The class: 0x14 bytes, vtable 0xAEA9BC (one slot, the deleting destructor 0x49F190), the vector at +0x04; its one
//   instance is 0x49F200's function-local static (0xBE3518, guard 0xBE3510, atexit stub 0x49F250). Its only caller is
//   SClient 0x751460 (the spend-buddy-points dialog), through 0x49F200 and 0x49F300.
// - Forms the bytes settle: the element declares an empty destructor (the unwinds call its out-of-line copy 0x49F170,
//   and the constructor's local has an EH state); the constructor's element is a named local pushed with push_back
//   (the 3-argument insert 0x49F610 stays out of line); 0x49F270 is a .cpp member with no caller, inlined in 0x49F300,
//   comparing 'RewardLevel >= level'; 0x49F300 returns early when points is 0 (its own copy of the empty vector).
// - Instances in the object: ~BuddyRewardStruct 0x49F170, vector<BuddyRewardStruct>'s destructor 0x49F5D0 and
//   insert 0x49F610, std::_Construct 0x49FA20, BuddyRewardStruct's deleting destructor 0x49FA50 (all exact).
