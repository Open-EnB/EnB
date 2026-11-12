// Earth & Beyond client (reconstruction): app\shared\unknown_4fac20.cpp. Notes at the end of the file.
#include "unknown_4fac20.h"
#include <algorithm>
#include <ctype.h>
#include <string.h>

// 0x4FAC20: the text without its leading and trailing white space
std::string String_Trim(const char *text)
{
	const char *first = text;
	const char *last = text + strlen(text) - 1;
	while (isspace(*first)) {
		first++;
	}
	while (last > first && isspace(*last)) {
		last--;
	}
	return std::string(first, last + 1);
}

// 0x4FAD80: the text without the leading and trailing characters that characters holds
std::string String_Trim(const char *text, const char *characters)
{
	const char *first = text;
	const char *last = text + strlen(text) - 1;
	while (*first && strchr(characters, *first)) {
		first++;
	}
	while (last > first && *last && strchr(characters, *last)) {
		last--;
	}
	return std::string(first, last + 1);
}

// 0x4FAEE0: a ',' between every three digits from the right; false (number untouched) unless it is all digits
bool String_Insert_Commas(std::string &number)
{
	if (number.size() == 0) {
		return false;
	}
	std::string::iterator it;
	for (it = number.begin(); it != number.end(); ++it) {
		if (!isdigit(*it)) {
			return false;
		}
	}
	int count = 3 - (int)number.size() % 3;
	if (count == 3) {
		count = 0;
	}
	it = number.begin();
	while (it != number.end()) {
		count++;
		if (count == 3) {
			count = 0;
			++it;
			if (it != number.end()) {
				it = number.insert(it, ',');
				++it;
			}
		} else {
			++it;
		}
	}
	return true;
}

// 0x4FB150: a lower-case copy
std::string String_To_Lower(const std::string &string)
{
	std::string result(string);
	String_Make_Lower(result);
	return result;
}

// 0x4FB290: lower case in place
void String_Make_Lower(std::string &string)
{
	std::transform(string.begin(), string.end(), string.begin(), tolower);
}

// 0x4FB380: an upper-case copy
std::string String_To_Upper(const std::string &string)
{
	std::string result(string);
	String_Make_Upper(result);
	return result;
}

// 0x4FB4C0: upper case in place
void String_Make_Upper(std::string &string)
{
	std::transform(string.begin(), string.end(), string.begin(), toupper);
}

// 0x4FB5B0: the list's strings in one, separator between each two
std::string String_Join(const std::vector<std::string> &list, const std::string &separator)
{
	std::string result;
	if (list.size() == 0) {
		return result;
	}
	std::vector<std::string>::const_iterator it = list.begin();
	while (it != list.end()) {
		result += *it;
		++it;
		if (it != list.end()) {
			result += separator;
		}
	}
	return result;
}

/*
**	Reconstruction notes
**
**	client.exe 0x4FAC20-0x4FB800: the object after unknown_font_split_string.cpp's (whose guarded <string> static
**	initialiser is 0x4FABF0) and before the tokenizer's (unknown_4fb810.cpp, from 0x4FB810). Its own guarded <string>
**	initialiser 0x4FB7E0 closes it (the CRT initialiser table lists 0x4FABF0, 0x4FB7E0, 0x4FCBA0 in a row). No __FILE__
**	and no string names the file: a neutral name (D10), the placement provisional (D137).
**
**	Eight free functions, every one byte-exact. What the bytes say about the source: both trims build the result
**	with string(first, last + 1) and compare "last > first"; String_Insert_Commas counts with a signed remainder
**	(idiv) and walks iterators, inserting through insert(iterator, char); the two copies (String_To_Lower / _Upper)
**	call the in-place functions inline (their _Freeze calls stay out of line, one inline layer down), which are
**	std::transform with tolower / toupper; String_Join tests size() == 0 first, appends with += (the separator's
**	append is inline, the element's is not) and adds the separator under "if (it != list.end())" (with a break in
**	its place the jump at 0x4FB676 lands two bytes later). String_Join's bytes end at 0x4FB769; appfile.py calls
**	it PREFIX because the initialiser 0x4FB7E0 behind it is not a verified function start.
*/
