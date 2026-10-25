// Earth & Beyond client (reconstruction): app\shared\auxdata\policy.cpp. Notes at the end of the file.
#include "..\unknown_debug.h"

#include <string.h>
#include "policy.h"








































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

// ------------------------------------------------------------------------- functional (NOT EXACT)

// NOT EXACT (D42): the client's destructor 0x4AEDD0 also releases the rule sets (the vector at
// +0x00, not rebuilt); this one deletes the compiled policies.
PolicyClass::~PolicyClass()
{
	CompiledPolicyClass *compiled = m_Unknown10;
	while (compiled != 0) {
		CompiledPolicyClass *next = compiled->m_Unknown04;
		delete compiled;
		compiled = next;
	}
}

// NOT EXACT (D42): the client's 0x4AF890 releases what 0x4AF190 made (not rebuilt)
PolicyStateClass::~PolicyStateClass()
{
}

static bool IncludeEnabled = true;  // NAME 2: describes what it does
static PolicyRuleClass IncludeRule = {{0}, 3, 0, &IncludeEnabled};  // NAME 2: describes what it does

// NOT EXACT (D42): see policy.h. size is the root's field count (m_Unknown98), name its interned name.
// The root's own action is 2 (what the client's compile gives a node above a rule's fields: the
// captures show no update with the whole root), every other field's INCLUDE.
PolicyClass *Unknown_Make_Include_Policy(const char *name, int size)
{
	CompiledPolicyClass *compiled = new CompiledPolicyClass;
	compiled->m_Unknown00 = (int)name;
	compiled->m_Unknown04 = 0;
	compiled->Size = size;
	compiled->Table = new PolicyNodeClass *[size > 0 ? size : 1];
	for (int i = 0; i < size; i++) {
		PolicyNodeClass *node = new PolicyNodeClass;
		node->Rule = &IncludeRule;
		node->Next = 0;
		if (i == 0) {
			// the root: above the rule's fields, so it gets a rule of its own (action 2: visit
			// the children; the destructor deletes it with its node)
			PolicyRuleClass *rule = new PolicyRuleClass;
			memset(rule->m_Unknown00, 0, sizeof(rule->m_Unknown00));
			rule->Action = 2;
			rule->m_Unknown38 = 0;
			rule->m_Unknown3C = &IncludeEnabled;
			node->Rule = rule;
		}
		compiled->Table[i] = node;
	}
	PolicyClass *policy = new PolicyClass;
	memset(policy->m_Unknown00, 0, sizeof(policy->m_Unknown00));
	policy->m_Unknown10 = compiled;
	policy->Compiled = compiled;
	return policy;
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
