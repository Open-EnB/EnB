// Earth & Beyond client (reconstruction): app\shared\selector.cpp, the selectors that pick entries from a list of
// chances (0x49FA80-0x4A0C40 with the vector templates it emits; the asserts sit on the client's lines). Notes at the end.
#include "selector.h"
#include "g_assert.h"
#include "..\..\wwlib\random.h"
#include <string.h>
#include <time.h>

static Random2Class Random(time(NULL));	// NAME 2: the generator every selector rolls (0xBE3538, its initialiser 0x49FA80)

// 'Dependent': one index, the chances weighted against each other (table 0xAEA9C0)
class DependentSelectorClass : public SelectorClass	// NAME 3: the client's 'Dependent' (0x49FAB0), Westwood style (D24)
{
	public:
		DependentSelectorClass(void);
		virtual ~DependentSelectorClass(void);
		virtual void Select(const std::vector<float> &chances, std::vector<unsigned long> &selected);
		virtual SelectorType Get_Type(void) { return DEPENDENT; }
};

// 'Independent': every index whose chance (0..1) comes up, each rolled on its own (table 0xAEA9D0)
class IndependentSelectorClass : public SelectorClass	// NAME 3: the client's 'Independent' (0x49FAB0), Westwood style (D24)
{
	public:
		IndependentSelectorClass(void);
		virtual ~IndependentSelectorClass(void);
		virtual void Select(const std::vector<float> &chances, std::vector<unsigned long> &selected);
		virtual SelectorType Get_Type(void) { return INDEPENDENT; }
};

// 'Counted': each index once in every chances[i] calls, at a random call of each round (table 0xAEA9E8, 0x24 bytes)
class CountedSelectorClass : public SelectorClass	// NAME 3: the client's 'Counted' (0x49FAB0), Westwood style (D24)
{
	public:
		CountedSelectorClass(const std::vector<float> &chances);
		virtual ~CountedSelectorClass(void);
		virtual void Select(const std::vector<float> &chances, std::vector<unsigned long> &selected);
		virtual SelectorType Get_Type(void) { return COUNTED; }

	protected:
		std::vector<unsigned long> TriggerCount;	// +0x04: calls so far in this round
		std::vector<unsigned long> Countdown;		// +0x14: calls left until the index is picked
};




















































































// @0x49FAB0
SelectorClass *SelectorClass::Create(const char *type, const std::vector<float> &chances)
{
	G_ASSERT(type);
	if (type == NULL) {
		return NULL;
	}
	if (stricmp(type, "Independent") == 0) {
		return Create(INDEPENDENT, chances);
	}
	if (stricmp(type, "Dependent") == 0) {
		return Create(DEPENDENT, chances);
	}
	if (stricmp(type, "Counted") == 0) {
		return Create(COUNTED, chances);
	}
	return NULL;
}

// @0x49FB70
SelectorClass *SelectorClass::Create(SelectorType type, const std::vector<float> &chances)
{
	switch (type) {
		case DEPENDENT:
			return new DependentSelectorClass;
		case INDEPENDENT:
			return new IndependentSelectorClass;
		case COUNTED:
			return new CountedSelectorClass(chances);
	}
	return NULL;
}

// @0x49FC90
DependentSelectorClass::DependentSelectorClass(void)
{
}

// @0x49FCE0
DependentSelectorClass::~DependentSelectorClass(void)
{
}

// @0x49FD00
void DependentSelectorClass::Select(const std::vector<float> &chances, std::vector<unsigned long> &selected)
{
	float total = 0.0f;
	for (int i = 0; i < chances.size(); i++) {
		total += chances[i];
	}
	if (total < 1.0f) {
		total = 1.0f;
	}
	float roll = Random(0, (int)(total * 100000.0f)) * 0.00001f;
	for (i = 0; i < chances.size(); i++) {
		if ((roll -= chances[i]) < 0.0f) {
			selected.push_back(i);
			return;
		}
	}
}

// @0x49FFC0
IndependentSelectorClass::IndependentSelectorClass(void)
{
}

// @0x4A0010
IndependentSelectorClass::~IndependentSelectorClass(void)
{
}








































// @0x4A0030
void IndependentSelectorClass::Select(const std::vector<float> &chances, std::vector<unsigned long> &selected)
{
	for (int i = 0; i < chances.size(); i++) {
		G_ASSERT(chances[i] >= 0.0f && chances[i] <= 1.0f);
		if (Random(0, 100000) * 0.00001f <= chances[i]) {
			selected.push_back(i);
		}
	}
}
















// @0x4A02F0
CountedSelectorClass::CountedSelectorClass(const std::vector<float> &chances)
{
	int count = chances.size();
	TriggerCount.reserve(count);
	Countdown.reserve(count);
	TriggerCount.assign(count, 0);
	for (int i = 0; i < count; i++) {
		Countdown.push_back(Random(1, (int)chances[i]));
	}
	G_ASSERT(TriggerCount.size() == Countdown.size());
	G_ASSERT(TriggerCount.size() == chances.size());
}

// @0x4A0640
CountedSelectorClass::~CountedSelectorClass(void)
{
	TriggerCount.clear();
	Countdown.clear();
}







// @0x4A0700
void CountedSelectorClass::Select(const std::vector<float> &chances, std::vector<unsigned long> &selected)
{
	for (int i = 0; i < chances.size(); i++) {
		G_ASSERT(chances[i] >= 1.0f);
		if (--Countdown[i] == 0) {
			selected.push_back(i);
		}
		if (++TriggerCount[i] == (int)chances[i]) {
			TriggerCount[i] = 0;
			Countdown[i] = Random(1, (int)chances[i]);
		}
	}
}

// Reconstruction notes (O8, 2026-11-12)
// - The object: __FILE__ e:\build\beta\G\app\shared\selector.cpp (0xB72D04) and its five asserts: 'type' l.131
//   (0x49FAB0), 'chances[i] >= 0.0f && chances[i] <= 1.0f' l.243 (0x4A0030), the two size checks l.275 / l.276
//   (0x4A02F0), 'chances[i] >= 1.0f' l.296 (0x4A0700). It opens with the static generator's initialiser 0x49FA80
//   (Random2Class at 0xBE3538 seeded with time(NULL)); 0x49FA20 / 0x49FA50 before it are the previous object's (the
//   referral.ini reader's 12-byte element copy and deleting destructor, called only from 0x49F300). It closes with
//   vector<unsigned long>'s members 0x4A0810-0x4A0C10 (destructor, size, insert(n), erase, _Destroy, _Ucopy, _Ufill,
//   _Construct), shared by later objects. Its literals: 1e-5f / 100000.0f at 0xAEA9E0 between the tables.
// - The class names are not the client's: only the kind strings 'Independent', 'Dependent', 'Counted' (0x49FAB0) and
//   the asserts' TriggerCount, Countdown, chances, type are. Nothing in the client calls either Create (no callers,
//   no references), so the classes are this file's alone; the derived ones are declared here (placement provisional).
// - Forms the bytes settle: the base declares no destructor (with an inline virtual one, Counted's destructor stores
//   the base's table again and its constructor's unwind state is 2, not 1); the loop counters are int (the pushed
//   index is a converted temporary: the counter stays in a register); Dependent's 'if ((roll -= chances[i]) < 0.0f)'
//   (fld st(0) / fcomp; a separate 'roll -= ...' statement compares with fcom); Counted's destructor clears both
//   vectors before they are destroyed.
