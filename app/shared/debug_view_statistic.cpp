// Earth & Beyond client (reconstruction): app\shared\debug_view_statistic.cpp, the debug view's statistics
// (client 0x4E71E0-0x4E81AF, after combo_ffactory.cpp, before event.cpp). Notes at the end.
#include <algorithm>
#include "debug_view_statistic.h"
#include "nstrdup.h"
#include "message.h"

// 0x4E71E0
void DebugViewStatistic::Display_All()
{
	int row = 0;
	std::vector<DebugViewStatistic *> &list = Get_List();
	for (std::vector<DebugViewStatistic *>::iterator it = list.begin(); it != list.end(); ++it) {
		(*it)->Display(0, row++);
	}
}

// 0x4E7270
void DebugViewStatistic::Reset_All()
{
	std::vector<DebugViewStatistic *> &list = Get_List();
	for (std::vector<DebugViewStatistic *>::iterator it = list.begin(); it != list.end(); ++it) {
		(*it)->Reset();
	}
}

// 0x4E7300
void DebugViewStatistic::Register(DebugViewStatistic *stat)
{
	Get_List().push_back(stat);
}

// 0x4E7560
void DebugViewStatistic::Unregister(DebugViewStatistic *stat)
{
	std::vector<DebugViewStatistic *> &list = Get_List();
	std::vector<DebugViewStatistic *>::iterator it = std::find(list.begin(), list.end(), stat);
	if (it != list.end()) {
		list.erase(it);
	}
}

// 0x4E7620 (the list's guarded atexit destructor 0x4E7690)
std::vector<DebugViewStatistic *> &DebugViewStatistic::Get_List()
{
	static std::vector<DebugViewStatistic *> list;
	return(list);
}

// 0x4E76C0
DebugViewAverageStatistic::DebugViewAverageStatistic(const char *name) :
	Name(NULL),
	Total(0),
	Count(0)
{
	Name = nstrdup(name);
	Register(this);
}

// 0x4E7960
DebugViewAverageStatistic::~DebugViewAverageStatistic()
{
	Unregister(this);
	delete [] Name;
	Name = NULL;
}






























// 0x4E7A40
void DebugViewAverageStatistic::Display(int x, int y)
{
	Message_Stat("", "Statistic", __FILE__, __LINE__, x, y, "%s: %f (%d samples)", Name, (Count > 0) ? (Total / Count) : 0.0f, Count);
}

// 0x4E7AC0
void DebugViewAverageStatistic::Reset()
{
	Total = 0;
	Count = 0;
}

// 0x4E7AE0
DebugViewMedianStatistic::DebugViewMedianStatistic(const char *name) :
	Name(NULL)
{
	Name = nstrdup(name);
	Register(this);
}

// 0x4E7DD0
DebugViewMedianStatistic::~DebugViewMedianStatistic()
{
	Unregister(this);
	delete [] Name;
	Name = NULL;
}












// 0x4E7EE0
void DebugViewMedianStatistic::Display(int x, int y)
{
	float median = 0.0f;
	if (!Samples.empty()) {
		std::sort(Samples.begin(), Samples.end());
		median = Samples[Samples.size() / 2];
	}
	Message_Stat("", "Statistic", __FILE__, __LINE__, x, y, "%s: %f (%d samples)", Name, median, Samples.size());
}

// 0x4E8020
void DebugViewMedianStatistic::Reset()
{
	Samples.resize(0);
}

/*
**	Notes (reconstruction)
**
**	Placement: the client's __FILE__ 'e:\build\beta\G\app\shared\debug_view_statistic.cpp' (0xB74F2C), pushed by
**	the two Display functions on lines 100 and 145 (category "Statistic", format '%s: %f (%d samples)'). The
**	object runs from 0x4E71E0 (after combo_ffactory.cpp's last copy 0x4E7000) to the vector copies ending
**	0x4E81AF; event.cpp starts at 0x4E81B0 (its own __FILE__). Name order agrees (combo_ffactory <
**	debug_view_statistic < event). Then the template copies: ~vector<float> 0x4E8070 (the median
**	constructor's unwind), the list vector's _Destroy 0x4E80B0, size 0x4E80D0, _Ucopy 0x4E8100, _Ufill 0x4E8140
**	and std::_Construct 0x4E8180 (the last three COMDATs shared with other objects), std::_Sort /
**	_Insertion_sort_1 for float.
**
**	Nothing in the client calls any of these functions or names the two vtables (0xAEC618, 0xAEC624) outside
**	the constructors and destructors: the debug view's statistics are dead code in retail. The list is a
**	function-local static (0xBE55F8, guard 0xBE5608) inlined into every user; Register / Unregister end 'ret 4'
**	without using ecx, so they are thiscall members that never use this (written as members of the base).
**	The two vtables have two slots each and no destructor slot, and the constructors store one vtable each: the
**	base is an interface whose own vtable the optimizer drops. Neither destructor runs a base destructor (the
**	median constructor's only unwind state is its vector), so each class owns its name.
**
**	Forms the bytes asked for: Display_All declares its row counter before taking the list; the median Reset
**	is resize(0) (its 0.0f temporary is the store to the stack; clear() and erase(begin, end) skip the size
**	test). Every function is byte-exact (appfile.py match, 2026-11-13).
**
**	Names: DebugViewStatistic from the file's name; the rest are neutral (NAME tags in the header).
*/
