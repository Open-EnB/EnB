// Earth & Beyond client (reconstruction): app\shared\selector.h, the selectors that pick entries from a list of
// chances (the client's app\shared\selector.cpp per its asserts; this header's name is provisional, NAME 3). Laid out
// from the factory 0x49FB70 and the three tables 0xAEA9C0 / 0xAEA9D0 / 0xAEA9E8. Notes at the end of selector.cpp.
#ifndef SELECTOR_H  // NAME 3 [selector.h]: provisional header name for selector.cpp
#define SELECTOR_H

#include <vector>

// What every selector does (4 bytes: the table only). It declares no destructor: each kind's table holds its own
// in slot +0x08, and the kinds' constructors and destructors store only their own table (0x49FC90, 0x4A0640).
class SelectorClass	// NAME 3: Westwood style (D24) after the client's selector.cpp
{
	public:
		enum SelectorType {	// NAME 3: the numbers 0x49FAB0 passes to 0x49FB70 for 'Dependent', 'Independent', 'Counted'
			DEPENDENT = 0,
			INDEPENDENT,
			COUNTED
		};

		// +0x00: add the indices picked from chances to selected (Dependent 0x49FD00, Independent 0x4A0030, Counted 0x4A0700)
		virtual void Select(const std::vector<float> &chances, std::vector<unsigned long> &selected) = 0;	// NAME 2: what it does
		// +0x04: the kind (0x49FC50, 0x49FC70, 0x4A05F0)
		virtual SelectorType Get_Type(void) = 0;	// NAME 2: what it does

		// 0x49FAB0 (by the client's names 'Independent', 'Dependent', 'Counted'), 0x49FB70 (by number); no callers
		static SelectorClass *Create(const char *type, const std::vector<float> &chances);	// NAME 2: what it does
		static SelectorClass *Create(SelectorType type, const std::vector<float> &chances);	// NAME 2: what it does
};

#endif
