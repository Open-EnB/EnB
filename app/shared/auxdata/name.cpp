// Earth & Beyond client (reconstruction): app\shared\auxdata\name.cpp. Notes at the end of the file.
#include <string.h>
#include "name.h"
#include "..\unknown_debug.h"

NameClass::NameClass(const char *name) :
	Name(name),
	Count(-1),
	HasWildcard(false)
{
	HasWildcard = strchr(name, '*') != 0;
}

NameClass::NameClass(const NameClass &that) :
	Name(that.Name),
	Count(that.Count),
	HasWildcard(that.HasWildcard)
{
	for (int i = 0; i < Count; i++) {
		Parts[i] = that.Parts[i];
	}
}

NameClass &NameClass::operator=(const NameClass &that)
{
	Name = that.Name;
	HasWildcard = that.HasWildcard;
	Count = that.Count;
	for (int i = 0; i < Count; i++) {
		Parts[i] = that.Parts[i];
	}
	return *this;
}

NameClass &NameClass::operator=(const char *name)
{
	Name = name;
	HasWildcard = strchr(name, '*') != 0;
	Count = -1;
	return *this;
}

bool NameClass::operator==(const NameClass &that) const
{
	if (!HasWildcard && !that.HasWildcard) {
		return Name == that.Name;
	}
	if (Count == -1) {
		Split();
	}
	if (that.Count == -1) {
		that.Split();
	}
	if (Count != that.Count) {
		return false;
	}
	return Match_Parts(that);
}

bool NameClass::Is_Parent_Of(const NameClass &that) const
{
	if (Count == -1) {
		Split();
	}
	if (that.Count == -1) {
		that.Split();
	}
	if (Count >= that.Count) {
		return false;
	}
	return Match_Parts(that);
}

bool NameClass::Is_Parent_Or_Same(const NameClass &that) const
{
	if (Count == -1) {
		Split();
	}
	if (that.Count == -1) {
		that.Split();
	}
	if (Count > that.Count) {
		return false;
	}
	return Match_Parts(that);
}

bool NameClass::Match_Parts(const NameClass &that) const
{
	const PermString &wildcard = Wildcard();
	for (int i = 0; i < Count; i++) {
		if (Parts[i] != that.Parts[i] && wildcard != Parts[i] && wildcard != that.Parts[i]) {
			return false;
		}
	}
	return true;
}































void NameClass::Split() const
{
	char buffer[256];
	strcpy(buffer, Name.Get_String());
	Count = 0;
	char *name = buffer;
	char *cur = buffer;
	while (*cur != 0) {
		if (*cur == '.') {
			G_ASSERT(name != cur);

			G_ASSERT(Count+1 <= MAX_NAME_LENGTH);
			*cur = 0;
			Parts[Count++] = name;
			name = cur + 1;
		}
		cur++;
	}





	if (name != cur) {
		G_ASSERT(Count+1 <= MAX_NAME_LENGTH);
		Parts[Count++] = name;
	}


	G_ASSERT(Count);
}






NameClass &NameClass::Append(const NameClass &rhs)
{
	char buffer[256];
	strcpy(buffer, Name.Get_String());
	strcat(buffer, ".");
	strcat(buffer, rhs.Name.Get_String());
	Name = buffer;
	HasWildcard = (HasWildcard || rhs.HasWildcard);
	if (Count == -1 || rhs.Count == -1) {
		Count = -1;
	} else {
		G_ASSERT(Count + rhs.Count <= MAX_NAME_LENGTH);
		for (int i = 0; i < rhs.Count; i++) {
			Parts[Count + i] = rhs.Parts[i];
			Count += rhs.Count;
		}
	}
	return *this;
}

PermString NameClass::Get_First() const
{
	if (Count == -1) {
		Split();
	}
	return Parts[0];
}

PermString NameClass::Get_Rest() const
{
	char buffer[256];
	strcpy(buffer, Name.Get_String());
	char *dot = strchr(buffer, '.');
	if (dot == 0) {
		return PermString("");
	}
	*dot = 0;
	return PermString(dot + 1);
}

/*
**	Reconstruction notes
**
**	client.exe 0x4AD380-0x4ADB9B: NameClass (D24), a dotted field name split into PermString parts.
**	Rebuilt: the constructors 0x4AD380 and 0x4AD3F0, operator= 0x4AD460 and 0x4AD4B0, operator==
**	0x4AD500, Is_Parent_Of 0x4AD610 (fewer parts, all matching), Is_Parent_Or_Same 0x4AD6F0,
**	Match_Parts 0x4AD7D0 (the first Count parts; '*' matches any part; the three before inline it,
**	although it is defined after them), Split 0x4AD890 (asserts on the client's lines 138, 140, 153,
**	158), Append 0x4AD9F0 (line 177), Get_First 0x4ADB80, Get_Rest 0x4ADBB0. Wildcard's static is constructed on first
**	use (guard 0xBE3B28, the PermString at 0xBE3B24, its atexit 0x4AD870).
**	Append adds rhs.Count to Count inside the copy loop, as the client does (the bytes have the
**	update in the loop): with more than one part the later parts land past the copied ones.
**	Not rebuilt: this object's initialisers before 0x4AD380 (0x4AD240-0x4AD300: atexit, permstr).
*/
