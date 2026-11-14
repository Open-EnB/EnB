// Earth & Beyond client (reconstruction): app\shared\debug_view_statistic.h, the debug view's statistics
// (debug_view_statistic.cpp, client 0x4E71E0-0x4E81AF). The header's name follows the .cpp's __FILE__ (no string
// names it; interim placement, D137). Notes at the end of the .cpp.
#ifndef DEBUG_VIEW_STATISTIC_H	// NAME 1 [debug_view_statistic.h]: the .cpp's name (D137)
#define DEBUG_VIEW_STATISTIC_H

#include <vector>

// A statistic shown on the debug view's "Statistic" window (Message_Stat). The client has no vtable of its own for
// it (no RTTI, no destructor slot): two slots, Display and Reset, overridden by both kinds below. Every statistic
// adds itself to one list (a function-local static, 0xBE55F8, guard 0xBE5608) when made and leaves it when
// destroyed. The list's functions are thiscall members that never use this (Register / Unregister end 'ret 4').
class DebugViewStatistic	// NAME 5 [DebugViewStatistic]: the client's __FILE__ debug_view_statistic.cpp
{
public:
	virtual void Display(int x, int y) = 0;		// +0x00 one Message_Stat line at (x, y)  // NAME 2: what it does
	virtual void Reset() = 0;			// +0x04 forget the samples  // NAME 2: what it does

	void Display_All();				// 0x4E71E0 every statistic, one row each (x 0, y its index)  // NAME 2: what it does
	void Reset_All();				// 0x4E7270  // NAME 2: what it does
	void Register(DebugViewStatistic *stat);	// 0x4E7300 push_back  // NAME 2: what it does
	void Unregister(DebugViewStatistic *stat);	// 0x4E7560 find and erase  // NAME 2: what it does
	std::vector<DebugViewStatistic *> &Get_List();	// 0x4E7620 the list (0xBE55F8)  // NAME 2: what it returns
};

// The mean of its samples (vtable 0xAEC618; 0x10 bytes)
class DebugViewAverageStatistic : public DebugViewStatistic	// NAME 2: what it shows
{
public:
	DebugViewAverageStatistic(const char *name);	// 0x4E76C0
	~DebugViewAverageStatistic();			// 0x4E7960

	virtual void Display(int x, int y);		// 0x4E7A40 (line 100: 'Total / Count')
	virtual void Reset();				// 0x4E7AC0

	char *Name;					// +0x04 nstrdup  // NAME 2: what it holds ('%s: %f (%d samples)')
	float Total;					// +0x08  // NAME 2: divided by Count
	int Count;					// +0x0C  // NAME 2: '%d samples'
};

// The median of its samples (vtable 0xAEC624; 0x18 bytes)
class DebugViewMedianStatistic : public DebugViewStatistic	// NAME 2: what it shows
{
public:
	DebugViewMedianStatistic(const char *name);	// 0x4E7AE0
	~DebugViewMedianStatistic();			// 0x4E7DD0

	virtual void Display(int x, int y);		// 0x4E7EE0 (line 145: sorts, the middle sample)
	virtual void Reset();				// 0x4E8020

	char *Name;					// +0x04 nstrdup  // NAME 2: what it holds
	std::vector<float> Samples;			// +0x08 (its allocator byte; +0x0C..+0x14)  // NAME 2: what it holds
};

#endif // DEBUG_VIEW_STATISTIC_H
