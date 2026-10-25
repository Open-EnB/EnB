// Earth & Beyond client (reconstruction): app\shared\auxdata\policy.cpp. Notes at the end of the file.
#include "..\unknown_debug.h"

// The classes are declared here until the rest of the file is rebuilt (the header is not known).

// one rule of a policy (made by 0x4AE490, whose assert names the actions EXCLUDE, INCLUDE and
// REDUNDANT: 1, 3 and 4 there)
class PolicyRuleClass  // NAME 2: describes what it does
{
public:
	char m_Unknown00[0x34];		// not known yet
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

// the rules per field id (the asserts' Compiled, Size)
class CompiledPolicyClass  // NAME 2: after the asserts' Compiled
{
public:
	~CompiledPolicyClass();											// 0x4AE850
	int Get_Action(int id, int *value) const;					// 0x4AED50 (the first rule that applies; 1 if none)  // NAME 2: describes what it does

	int m_Unknown00;
	int m_Unknown04;
	int Size;							// +0x08
	PolicyNodeClass **Table;		// +0x0C  // NAME 2: describes what it does
};

class PolicyClass  // NAME 3: by convention (D24) from the client's file policy.cpp
{
public:
	int Get_Action(int id, int *value) const;					// 0x4AF490  // NAME 2: describes what it does

	char m_Unknown00[0x14];				// not known yet
	CompiledPolicyClass *Compiled;		// +0x14
};

// (the lines down to Get_Action hold code of the client's file not rebuilt yet: its assert is on
// line 334)

CompiledPolicyClass::~CompiledPolicyClass()
{
	for (int i = 0; i < Size; i++) {
		PolicyNodeClass *node = Table[i];
		while (node != 0) {
			PolicyNodeClass *next = node->Next;
			if (node->Rule->Action == 2) {
				delete node->Rule;
			}
			delete node;
			node = next;
		}
	}
	delete [] Table;
}












































































































































































































































































int CompiledPolicyClass::Get_Action(int id, int *value) const
{
	G_ASSERT(id >= 0 && id < Size);
	for (PolicyNodeClass *node = Table[id]; node != 0; node = node->Next) {
		if (*node->Rule->m_Unknown3C) {
			*value = node->Rule->m_Unknown38;
			return node->Rule->Action;
		}
	}
	return 1;
}



































































































int PolicyClass::Get_Action(int id, int *value) const
{
	G_ASSERT(Compiled);
	return Compiled->Get_Action(id, value);
}

/*
**	Reconstruction notes
**
**	client.exe 0x4AE400-0x4AF6E0 (policy.cpp; the reference decoder's notes: a rule set, INCLUDE/EXCLUDE per
**	field path). Rebuilt: CompiledPolicyClass's destructor 0x4AE850 (the nodes, and the rules of
**	action 2), Get_Action 0x4AED50 (line 334) and PolicyClass::Get_Action 0x4AF490 (line 444,
**	the compiled one inlined; encode.cpp's 0x4A8CA0/0x4A8E60 call it). Not rebuilt yet: the rule's
**	constructor 0x4AE490 (lines 32, 45), 0x4AE760 (110), 0x4AE8D0-0x4AEC70 (256), 0x4AEDD0,
**	0x4AEF40 (378), 0x4AF190, 0x4AF3A0, and the class of vtable 0xAEB324 (constructor 0x4AF550,
**	destructor 0x4AF5D0 with "Parent == 0" on line 466, base vtable 0xAE7FB0).
*/
