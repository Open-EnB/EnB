// Earth & Beyond client (reconstruction): app\shared\auxdata\policy.cpp. Notes at the end of the file.
#include "..\unknown_debug.h"
#include "policy.h"
#include "delta.h"
#include "lookup.h"
















PolicyRuleSetClass::~PolicyRuleSetClass()
{
	for (PolicyRuleClass **rule = Rules.begin(); rule != Rules.end(); rule++) {
		delete *rule;
	}
	Rules.clear();
}

void PolicyRuleSetClass::Add_Rule(const NameClass &pattern, int action, int value)
{
	G_ASSERT(action == EXCLUDE || action == INCLUDE || action == REDUNDANT);


	if (value == -1) {
		switch (action) {
			case EXCLUDE:
				value = -1;
				break;
			case INCLUDE:
			case REDUNDANT:
				value = 0;
				break;
			default:
				G_ASSERT(0);



















































				value = 0;
				break;
		}
	}
	Rules.push_back(new PolicyRuleClass(pattern, action, value, &Enabled, this));
}

CompiledPolicyClass::CompiledPolicyClass(PolicyClass *policy, DeltaCompoundClass *c, CompiledPolicyClass *next) :
	Root(0),
	Next(next),
	Size(0),
	Table(0)
{
	G_ASSERT(c);











































































	Root = c->Name.Get_String();
	Size = c->m_Unknown98;
	Table = new PolicyNodeClass *[Size];
	for (int i = 0; i < Size; i++) {
		Table[i] = 0;
	}
	for (PolicyRuleSetClass **rule_set = policy->Sets.begin(); rule_set != policy->Sets.end(); rule_set++) {
		PolicyRuleClass **rules = Compile_Rules((*rule_set)->Rules, c);
		Insert(rules);
		delete [] rules;
	}
}

CompiledPolicyClass::~CompiledPolicyClass()
{
	for (int i = 0; i < Size; i++) {
		PolicyNodeClass *node = Table[i];
		while (node != 0) {
			PolicyNodeClass *next = node->Next;
			if (node->Rule->Action == DESCEND) {
				delete node->Rule;
			}
			delete node;
			node = next;
		}
	}
	delete [] Table;
}

PolicyRuleClass **CompiledPolicyClass::Compile_Rules(const std::vector<PolicyRuleClass *> &rules, DeltaCompoundClass *c)
{
	int count = c->m_Unknown98;
	PolicyRuleClass **result = new PolicyRuleClass *[count];
	for (int i = 0; i < count; i++) {
		result[i] = 0;
	}
	LookupClass *lookup = c->Get_Lookup();
	for (int id = count - 1; id >= 0; id--) {
		DeltaClass *delta = lookup->Get_Delta(id);
		result[id] = Merge_Rules(result[id], Find_Rule(rules, delta->FullName));
		if (delta->Parent != 0) {
			int parent = delta->Parent->m_Unknown7C;
			result[parent] = Merge_Rules(result[parent], result[id]);
		}
	}
	return result;
}

PolicyRuleClass *CompiledPolicyClass::Find_Rule(const std::vector<PolicyRuleClass *> &rules, const NameClass &name)
{
	for (PolicyRuleClass * const *rule = rules.begin(); rule != rules.end(); rule++) {
		if ((*rule)->Pattern.Is_Parent_Or_Same(name)) {
			return *rule;
		}
	}
	return 0;
}

PolicyRuleClass *CompiledPolicyClass::Merge_Rules(PolicyRuleClass *current, PolicyRuleClass *mod)
{
	if (current == 0) {
		return mod;
	}
	if (mod == 0) {
		if (current->Action != EXCLUDE && current->Action != DESCEND) {
			PolicyRuleClass *rule = new PolicyRuleClass(*current);
			rule->Action = DESCEND;
			return rule;
		}
	} else {
		G_ASSERT(current->RuleSet == mod->RuleSet);




































		if (current->Action == DESCEND) {
			if (mod->Value < current->Value) {
				current->Value = mod->Value;
			}
			return current;
		}
		if (current->Action != mod->Action || current->Value != mod->Value) {
			PolicyRuleClass *rule = new PolicyRuleClass(*current);
			rule->Action = DESCEND;
			rule->Value = current->Value < mod->Value ? current->Value : mod->Value;
			return rule;
		}
	}
	return current;
}

void CompiledPolicyClass::Insert(PolicyRuleClass **rules)
{
	for (int i = 0; i < Size; i++) {
		PolicyRuleClass *rule = rules[i];
		if (rule != 0) {
			if (Table[i] == 0) {
				Table[i] = new PolicyNodeClass(rule, 0);
			} else {
				PolicyNodeClass **link = &Table[i];
				PolicyNodeClass *node = Table[i];
				while (node != 0) {
					if (node->Rule->Action < rule->Action || (node->Rule->Action == rule->Action && rule->Value < node->Rule->Value)) {
						break;
					}
					link = &node->Next;
					node = node->Next;
				}
				*link = new PolicyNodeClass(rule, node);
			}
		}
	}
}

int CompiledPolicyClass::Get_Action(int id, int *value) const
{
	G_ASSERT(id >= 0 && id < Size);



















	for (PolicyNodeClass *node = Table[id]; node != 0; node = node->Next) {
		if (*node->Rule->Enabled) {
			*value = node->Rule->Value;
			return node->Rule->Action;
		}
	}
	return 1;
}

PolicyClass::~PolicyClass()
{
	while (PolicyHead != 0) {
		CompiledPolicyClass *next = PolicyHead->Next;
		delete PolicyHead;
		PolicyHead = next;
	}
	for (PolicyRuleSetClass **rule_set = Sets.begin(); rule_set != Sets.end(); rule_set++) {
		delete *rule_set;
	}
	Sets.clear();
}

void PolicyClass::Add(PolicyRuleSetClass *rule_set)
{
	G_ASSERT(PolicyHead == 0);




















	Sets.push_back(rule_set);
}

PolicyStateClass *PolicyClass::Make_State(int a, int b)
{
	PolicyStateClass *state = new PolicyStateClass(Sets.size());
	for (PolicyRuleSetClass **rule_set = Sets.begin(); rule_set != Sets.end(); rule_set++) {
		RefCountPtr<PolicyConditionClass> condition = (*rule_set)->Get_Condition(a, b);
		int index = state->Entries.size();
		state->Entries.push_back(std::pair<RefCountPtr<PolicyConditionClass>, bool>(condition, false));
		condition->Attach(state, index);
	}
	for (std::pair<RefCountPtr<PolicyConditionClass>, bool> *entry = state->Entries.begin(); entry != state->Entries.end(); entry++) {
		bool value = entry->first->Evaluate(a, b);
		if (value != entry->second) {
			entry->second = value;
			state->Dirty = true;
		}
	}
	return state;
}

void PolicyClass::Bind(PolicyStateClass *state, DeltaCompoundClass *root)
{
	unsigned int count = Sets.size();
	for (unsigned int i = 0; i < count; i++) {
		Sets[i]->Enabled = state->Entries[i].second;
	}
	if (Compiled == 0 || Compiled->Root != root->Name.Get_String()) {
		CompiledPolicyClass *compiled;
		for (compiled = PolicyHead; compiled != 0; compiled = compiled->Next) {
			if (compiled->Root == root->Name.Get_String()) {
				break;
			}
		}
		if (compiled == 0) {
			compiled = new CompiledPolicyClass(this, root, PolicyHead);
			PolicyHead = compiled;
		}
		Compiled = compiled;
	}
}

int PolicyClass::Get_Action(int id, int *value) const
{
	G_ASSERT(Compiled);












	return Compiled->Get_Action(id, value);
}

PolicySwitchClass::PolicySwitchClass() : Parent(0), Index(-1), Value(false)
{
}

PolicySwitchClass::~PolicySwitchClass()
{
	G_ASSERT(Parent == 0);
}

void PolicySwitchClass::Set(bool value)
{
	Value = value;
	Update();
}

void PolicySwitchClass::Attach(PolicyStateClass *state, int index)
{
	Parent = state;
	Index = index;
	Update();
}

void PolicySwitchClass::Update()
{
	PolicyStateClass *state = Parent;
	if (state != 0) {
		std::pair<RefCountPtr<PolicyConditionClass>, bool> &entry = state->Entries[Index];
		if (entry.second != Value) {
			entry.second = Value;
			state->Dirty = true;
		}
	}
}

PolicyStateClass::~PolicyStateClass()
{
	for (std::pair<RefCountPtr<PolicyConditionClass>, bool> *entry = Entries.begin(); entry != Entries.end(); entry++) {
		entry->first->Attach(0, 0);
	}
}

/*
**	Reconstruction notes
**
**	client.exe 0x4AE400-0x4AF930 (policy.cpp; earlier research notes: a rule set, INCLUDE/EXCLUDE per
**	field path), in the client's order: the rule set's destructor 0x4AE400 and Add_Rule 0x4AE490
**	(lines 32, 45), CompiledPolicyClass (constructor 0x4AE760 line 110, destructor 0x4AE850, Insert
**	0x4AEC70, Get_Action 0x4AED50 line 334) with the compile's statics 0x4AE8D0 / 0x4AEA00 /
**	0x4AEA50 (line 256), PolicyClass (destructor 0x4AEDD0, Add 0x4AEF40 line 378, Make_State
**	0x4AF190, Bind 0x4AF3A0, Get_Action 0x4AF490 line 444), PolicySwitchClass (vtable 0xAEB324:
**	0x4AF550-0x4AF6E0, line 466) and PolicyStateClass's destructor 0x4AF890.
**	How a policy works: each rule set holds rules (a field path pattern, an action, a value) and is
**	enabled while its condition evaluates true; Make_State records each set's condition and value,
**	Bind copies the values into the sets and compiles the policy for the root on first use: per field
**	id the first rule of each set whose pattern is the field's path or a parent of it, merged with
**	what the fields below gave (a parent whose children's rules differ gets a rule of its own,
**	DESCEND, with the smallest value), the sets' rules in one list per field ordered by action.
**	Get_Action gives the first enabled rule's action and value (EXCLUDE when none).
*/
