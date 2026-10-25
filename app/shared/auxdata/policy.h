/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\auxdata\policy.h (name assumed from policy.cpp)
**
**	The aux-data send policy (policy.cpp): rules per field path (EXCLUDE, INCLUDE, REDUNDANT),
**	compiled per root into a rule list per field id. The update collector (encode.cpp) asks it what
**	to do with each field.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __POLICY_H  // NAME 3 [policy.h]: header name assumed from the client's policy.cpp
#define __POLICY_H

// one rule of a policy (made by 0x4AE490, whose assert names the actions EXCLUDE, INCLUDE and
// REDUNDANT: 1, 3 and 4 there)
class PolicyRuleClass  // NAME 2: describes what it does
{
public:
	char m_Unknown00[0x34];		// not known yet (a NameClass: 0x4AE490 copies its name argument here)
	int Action;						// +0x34 (2: the rule belongs to its node)  // NAME 3: after the assert's parameter action (policy.cpp)
	int m_Unknown38;				// +0x38 the value Get_Action hands back
	bool *m_Unknown3C;			// +0x3C the rule applies while this is set
};

class PolicyNodeClass  // NAME 2: describes what it does
{
public:
	PolicyRuleClass *Rule;		// +0x00  // NAME 2: describes what it does
	PolicyNodeClass *Next;		// +0x04  // NAME 2: describes what it does
};

// the rules per field id (the asserts' Compiled, Size); made by 0x4AE760 for one root
class CompiledPolicyClass  // NAME 2: after the asserts' Compiled
{
public:
	~CompiledPolicyClass();											// 0x4AE850
	int Get_Action(int id, int *value) const;					// 0x4AED50 (the first rule that applies; 1 if none)  // NAME 2: describes what it does

	int m_Unknown00;					// +0x00 the root's name (its interned string; 0x4AF3A0 looks it up)
	CompiledPolicyClass *m_Unknown04;	// +0x04 the next root's (the list at PolicyClass +0x10)
	int Size;							// +0x08
	PolicyNodeClass **Table;		// +0x0C  // NAME 2: describes what it does
};

// size 0x18 (UnknownSendRootClass's +0xA0)
class PolicyClass  // NAME 3: by convention (D24) from the client's file policy.cpp
{
public:
	~PolicyClass();														// 0x4AEDD0 (functional here)
	int Get_Action(int id, int *value) const;					// 0x4AF490  // NAME 2: describes what it does

	char m_Unknown00[0x10];				// +0x00 a vector of the rule sets (0x4AEF40 adds one), not known yet
	CompiledPolicyClass *m_Unknown10;	// +0x10 the compiled policies, one per root ("PolicyHead == 0" at line 378)
	CompiledPolicyClass *Compiled;		// +0x14
};

// what 0x4AF190 makes from a policy's rule sets (0x14 bytes; UnknownSendRootClass +0xA4): a flag
// per rule set, which 0x4AF3A0 copies into them
class PolicyStateClass  // NAME 2: describes what it does
{
public:
	~PolicyStateClass();													// 0x4AF890 (functional here)

	char m_Unknown00[0x14];		// not known yet
};

// NOT EXACT (D42): a functional stand-in for what UnknownSendRootClass::Unknown_48 (0x4A40B0)
// builds through the rule set ("*" INCLUDE, 0x4AE490), PolicyClass's 0x4AEF40 / 0x4AF190 /
// 0x4AF3A0: a policy whose every field of a root (name: its interned name, size: its field count)
// has the action INCLUDE (3) with the value 0, the root itself 2 (visit the children).
PolicyClass *Unknown_Make_Include_Policy(const char *name, int size);  // NAME 2: describes what it does

#endif //__POLICY_H
