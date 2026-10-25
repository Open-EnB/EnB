/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\auxdata\policy.h (name assumed from policy.cpp)
**
**	The aux-data send policy (policy.cpp): rule sets of rules per field path (EXCLUDE, INCLUDE,
**	REDUNDANT), each set switched on or off by a condition; compiled per root into a rule list per
**	field id. The update collector (encode.cpp) asks it what to do with each field.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __POLICY_H  // NAME 3 [policy.h]: header name assumed from the client's policy.cpp
#define __POLICY_H

#include <vector>
#include <utility>
#include "refcount.h"
#include "ref_ptr.h"
#include "name.h"

class DeltaCompoundClass;
class PolicyClass;
class PolicyRuleSetClass;
class PolicyStateClass;

// The actions (0x4AE490's assert names EXCLUDE, INCLUDE and REDUNDANT); 2 is the compile's own,
// for a field above fields whose rules differ: visit the children.
enum
{
	EXCLUDE = 1,
	DESCEND = 2,  // NAME 2: describes what it does
	INCLUDE = 3,
	REDUNDANT = 4
};

// What switches a rule set on (vtable 0xAEACD4; WWLib's RefCountClass): Evaluate gives the state,
// Attach the policy state that keeps it (index: its rule set's)
class PolicyConditionClass : public RefCountClass  // NAME 2: describes what it does
{
public:
	virtual bool Evaluate(int a, int b) = 0;										// +0x08  // NAME 2: describes what it does
	virtual void Attach(PolicyStateClass *state, int index) = 0;				// +0x0C  // NAME 2: describes what it does
};

// A condition set from outside (vtable 0xAEB324): Set pushes the value into the state it is attached to
class PolicySwitchClass : public PolicyConditionClass  // NAME 2: describes what it does
{
public:
	PolicySwitchClass();																// 0x4AF550
	~PolicySwitchClass();															// 0x4AF5D0 (line 466; deleting 0x4AF5A0)
	virtual bool Evaluate(int, int) { return Value; }						// 0x4AF580
	virtual void Attach(PolicyStateClass *state, int index);				// 0x4AF690
	void Set(bool value);															// 0x4AF650  // NAME 2: describes what it does
	void Update();																		// 0x4AF6E0  // NAME 2: describes what it does

	PolicyStateClass *Parent;			// +0x08 (the assert's name)
	int Index;								// +0x0C  // NAME 2: describes what it does
	bool Value;								// +0x10  // NAME 2: describes what it does
};

// One rule (0x44 bytes, made by PolicyRuleSetClass::Add_Rule 0x4AE490): a field path pattern, the
// action and its value (a time for INCLUDE / REDUNDANT; the collector's rate limit)
class PolicyRuleClass  // NAME 2: describes what it does
{
public:
	PolicyRuleClass(const NameClass &pattern, int action, unsigned int value, bool *enabled, PolicyRuleSetClass *rule_set) :
		Pattern(pattern),
		Action(action),
		Value(value),
		Enabled(enabled),
		RuleSet(rule_set)
	{
	}

	NameClass Pattern;					// +0x00  // NAME 2: describes what it does
	int Action;								// +0x34 (DESCEND: the rule belongs to its node)  // NAME 3: after the assert's parameter action (policy.cpp)
	unsigned int Value;					// +0x38 the value Get_Action hands back  // NAME 2: describes what it does
	bool *Enabled;							// +0x3C the rule applies while its set is enabled  // NAME 2: describes what it does
	PolicyRuleSetClass *RuleSet;		// +0x40 (the assert's name)
};

class PolicyNodeClass  // NAME 2: describes what it does
{
public:
	PolicyNodeClass(PolicyRuleClass *rule, PolicyNodeClass *next) : Rule(rule), Next(next) {}

	PolicyRuleClass *Rule;		// +0x00  // NAME 2: describes what it does
	PolicyNodeClass *Next;		// +0x04  // NAME 2: describes what it does
};

// A set of rules (vtable 0xAEACB4, 0x18 bytes): enabled as its condition says (Get_Condition)
class PolicyRuleSetClass  // NAME 3: after the assert's RuleSet (policy.cpp)
{
public:
	PolicyRuleSetClass() : Enabled(true) {}
	virtual ~PolicyRuleSetClass();																	// 0x4AE400
	virtual RefCountPtr<PolicyConditionClass> Get_Condition(int a, int b) = 0;		// +0x04  // NAME 2: describes what it does
	void Add_Rule(const NameClass &pattern, int action, int value);						// 0x4AE490 (lines 32, 45)  // NAME 2: describes what it does

	bool Enabled;									// +0x04  // NAME 2: describes what it does
	std::vector<PolicyRuleClass *> Rules;	// +0x08  // NAME 2: describes what it does
};

// The rules per field id of one root (the asserts' Compiled, Size); made by 0x4AE760
class CompiledPolicyClass  // NAME 2: after the asserts' Compiled
{
public:
	CompiledPolicyClass(PolicyClass *policy, DeltaCompoundClass *c, CompiledPolicyClass *next);	// 0x4AE760 (line 110)
	~CompiledPolicyClass();											// 0x4AE850
	// the compile (members that use nothing of the object): per field id the rule of one set
	PolicyRuleClass **Compile_Rules(const std::vector<PolicyRuleClass *> &rules, DeltaCompoundClass *c);	// 0x4AE8D0  // NAME 2: describes what it does
	PolicyRuleClass *Find_Rule(const std::vector<PolicyRuleClass *> &rules, const NameClass &name);	// 0x4AEA00 (inlined in Compile_Rules)  // NAME 2: describes what it does
	PolicyRuleClass *Merge_Rules(PolicyRuleClass *current, PolicyRuleClass *mod);	// 0x4AEA50 (line 256)  // NAME 2: describes what it does
	void Insert(PolicyRuleClass **rules);							// 0x4AEC70 (each field's rule into its list, in order)  // NAME 2: describes what it does
	int Get_Action(int id, int *value) const;					// 0x4AED50 (the first rule that applies; 1 if none)  // NAME 2: describes what it does

	const char *Root;					// +0x00 the root's name (interned)  // NAME 2: describes what it does
	CompiledPolicyClass *Next;		// +0x04 (the list at PolicyClass::PolicyHead)  // NAME 2: describes what it does
	int Size;							// +0x08
	PolicyNodeClass **Table;		// +0x0C  // NAME 2: describes what it does
};

// What Make_State 0x4AF190 makes of a policy's rule sets (0x14 bytes; UnknownSendRootClass +0xA4):
// each set's condition and its value; Bind copies the values into the sets
class PolicyStateClass  // NAME 2: describes what it does
{
public:
	PolicyStateClass(int count) : Dirty(true) { Entries.reserve(count); }
	~PolicyStateClass();																					// 0x4AF890

	std::vector<std::pair<RefCountPtr<PolicyConditionClass>, bool> > Entries;	// +0x00  // NAME 2: describes what it does
	bool Dirty;																							// +0x10  // NAME 2: describes what it does
};

// size 0x18 (UnknownSendRootClass's +0xA0)
class PolicyClass  // NAME 3: by convention (D24) from the client's file policy.cpp
{
public:
	PolicyClass() : PolicyHead(0), Compiled(0) {}
	~PolicyClass();														// 0x4AEDD0
	void Add(PolicyRuleSetClass *rule_set);							// 0x4AEF40 (line 378)  // NAME 2: describes what it does
	PolicyStateClass *Make_State(int a, int b);					// 0x4AF190  // NAME 2: describes what it does
	void Bind(PolicyStateClass *state, DeltaCompoundClass *root);	// 0x4AF3A0 (the sets' state, the root's compiled rules)  // NAME 2: describes what it does
	int Get_Action(int id, int *value) const;					// 0x4AF490  // NAME 2: describes what it does

	std::vector<PolicyRuleSetClass *> Sets;		// +0x00  // NAME 2: describes what it does
	CompiledPolicyClass *PolicyHead;				// +0x10 the compiled policies, one per root (the assert's name)
	CompiledPolicyClass *Compiled;				// +0x14 the bound root's (the assert's name)
};

#endif //__POLICY_H
