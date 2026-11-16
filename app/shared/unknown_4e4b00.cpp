// Earth & Beyond client (reconstruction): app\shared\unknown_4e4b00.cpp (PROVISIONAL file name, D137). Notes at the end.
#include <string>
#include <algorithm>
#include <ctype.h>
#include <string.h>
#include <hash_map.h>

struct Unknown4E4B00Equal								// NAME 1: neutral (D10), case-insensitive string equality functor
{
	bool operator()(const std::string &a, const std::string &b) const;	// 0x4E4B00  // NAME 1 [a, b]: neutral
};

struct Unknown4E4B50Hash									// NAME 1: neutral (D10), case-insensitive std::string hash functor
{
	size_t operator()(std::string s) const;				// 0x4E4B50  // NAME 1 [s]: neutral
};

bool Unknown4E4B00Equal::operator()(const std::string &a, const std::string &b) const
{
	if (stricmp(a.c_str(), b.c_str()) == 0) {
		return true;
	}
	return false;
}

size_t Unknown4E4B50Hash::operator()(std::string s) const
{
	std::transform(s.begin(), s.end(), s.begin(), tolower);
	return std::__stl_hash_string(s.c_str());
}

/*
**	Reconstruction notes
**
**	client.exe 0x4E4B00-0x4E4C85 (followed by <string>'s guarded initialiser 0x4E4CE0, between
**	chat_filter_util.cpp's 0x4E4AD0 and combo_ffactory.cpp's 0x4E4D10): case-insensitive
**	string equality (0x4E4B00, stricmp) and lowercase SGI __stl_hash_string (0x4E4B50)
**	functors. No callers in client.exe; no __FILE__ string; provisional name (D137).
*/
