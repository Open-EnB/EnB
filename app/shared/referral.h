// Earth & Beyond client (reconstruction): app\shared\referral.h, the referral rewards (referral.cpp, client
// 0x49EF10-0x49FA7F). Notes at the end of referral.cpp.
#ifndef REFERRAL_H
#define REFERRAL_H

#include <vector>

class INIClass;

// one [RewardLevel<n>] section of referral.ini (12 bytes)
struct BuddyRewardStruct	// NAME 2: what SClient 0x751460 sums from it
{
	BuddyRewardStruct(int level, INIClass &ini);	// 0x49EF10
	~BuddyRewardStruct() {}							// 0x49F170 (the unwind's out-of-line copy)

	int RewardLevel;									// +0x00 (key 'RewardLevel')
	int SkillBoost;										// +0x04 (key 'SkillBoost')
	int CreditBoost;									// +0x08 (key 'CreditBoost')
};

// the rewards of referral.ini's [RewardLevel0] .. [RewardLevel9] (0x14 bytes, vtable 0xAEA9BC)
class Unknown49F200Class	// NAME 1: neutral (D10)
{
public:
	Unknown49F200Class();								// 0x49EFE0
	virtual ~Unknown49F200Class();						// 0x49F1C0

	const BuddyRewardStruct &Unknown_49F270(int level);	// 0x49F270 (inlined in 0x49F300)  // NAME 1 [level]: compared with RewardLevel
	std::vector<BuddyRewardStruct> Unknown_49F300(int points_spent, int points);	// 0x49F300 (SClient 0x751460)  // NAME 1 [points_spent, points]: what 0x751460 passes

protected:
	std::vector<BuddyRewardStruct> Rewards;				// +0x04  // NAME 1: neutral
};

Unknown49F200Class *Unknown_49F200();					// 0x49F200 (no this: the rewards' instance)

#endif
