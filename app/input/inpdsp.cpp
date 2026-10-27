// Earth & Beyond client (reconstruction): app\input\inpdsp.cpp. Notes at the end of the file.
#include "inpdsp.h"
#include <windows.h>
#include "..\..\wwlib\ini.h"

LoginUnknown0CClass::LoginUnknown0CClass()
{
}


















LoginUnknown0CClass::~LoginUnknown0CClass()
{
	std::hash_map<unsigned int, InputRecordStruct *>::iterator i;
	for (i = Stacks.begin(); i != Stacks.end(); ++i) {
		InputRecordStruct *stack = i->second;
		G_ASSERT(stack);
		while (stack) {
			InputRecordStruct *next = stack->Next;
			delete stack;
			stack = next;
		}
	}
	Stacks.clear();
}

bool LoginUnknown0CClass::Unknown_429A20(KernelEventClass *event)
{
	unsigned int key = Unknown_42A170(event);
	std::hash_map<unsigned int, InputRecordStruct *>::iterator i = Stacks.find(key);
	if (i == Stacks.end()) {
		return false;
	}
	InputRecordStruct *rec = i->second;
	G_ASSERT(rec);
	G_ASSERT(rec->Action);
	return rec->Action->Unknown_08(rec->Data);
}










bool LoginUnknown0CClass::Unknown_429AF0(KernelEventClass *event, InputActionClass *action, int data, void *owner)
{
	unsigned int key = Unknown_42A170(event);
	InputRecordStruct *record = new InputRecordStruct(owner, data, action);
	std::hash_map<unsigned int, InputRecordStruct *>::iterator i = Stacks.find(key);
	if (i != Stacks.end()) {
		InputRecordStruct *rec = i->second;
		G_ASSERT(rec);
		while (rec) {
			if (rec->Owner == owner) { delete record; return false; }
			rec = rec->Next;
		}
		record->Next = i->second;
		Set_Stack(key, record);
		return true;
	}
	Stacks[key] = record;
	return true;
}

bool Unknown_429D00()
{
	HKEY key;
	DWORD type, value, size;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Westwood Studios\\Earth and Beyond", 0, KEY_READ, &key) == ERROR_SUCCESS) {
		value = 0; type = REG_DWORD; size = sizeof(value);
		LONG result = RegQueryValueEx(key, "DisableExceptions", NULL, &type, (LPBYTE)&value, &size);
		RegCloseKey(key);
		if (result == ERROR_SUCCESS && type == REG_DWORD) return value == 0;
	}
	return true;
}

bool LoginUnknown0CClass::Unknown_429D90(KernelEventClass *event, void *owner)
{
	unsigned int key = Unknown_42A170(event);
	std::hash_map<unsigned int, InputRecordStruct *>::iterator i = Stacks.find(key);
	if (i == Stacks.end()) {
		return false;
	}
	G_ASSERT(i->second);
	return Unknown_429F60(i->second, key, owner);
}







int LoginUnknown0CClass::Unknown_429E50(void *owner)
{
	int count = 0;
	std::hash_map<unsigned int, InputRecordStruct *>::iterator i;
	for (i = Stacks.begin(); i != Stacks.end(); ++i) {
		InputRecordStruct *stack = i->second;
		G_ASSERT(stack);
		if (Unknown_429F60(stack, i->first, owner)) {
			count++;
		}
	}
	return count;
}

// NOT EXACT: register-only, 1 row: the client reloads rec->Next through prev (edi) in the search loop (attempts.csv)
bool LoginUnknown0CClass::Unknown_429F60(InputRecordStruct *stack, unsigned int key, void *owner)
{
	if (stack->Owner == owner) {
		Stacks[key] = stack->Next;
		delete stack;
		if (Stacks[key] == NULL) {
			Stacks.erase(key);
		}
		return true;
	}
	InputRecordStruct *prev = stack;
	for (InputRecordStruct *rec = stack->Next; rec != NULL && rec->Owner != owner; rec = rec->Next) {
		prev = rec;
	}
	InputRecordStruct *found = prev->Next;
	if (found) {
		InputRecordStruct *next = found->Next;
		delete found;
		prev->Next = next;
		return true;
	}
	return false;
}

unsigned int LoginUnknown0CClass::Unknown_42A170(KernelEventClass *event) const
{
	return (event->Flags << 16) | event->Key;
}

KernelEventClass Unknown_42A1A0(INIClass &ini, const char *section, const char *entry)
{
	char buffer[0x31];
	unsigned short flags = 0;
	ini.Get_String(section, entry, "", buffer, sizeof(buffer));
	if (buffer[0] == '\0') {
		return KernelEventClass();
	}
	if (buffer[0] == '~' && buffer[1] == '\0') {
		buffer[0] = '`';
	}
	int i = 0;
	while (true) {
		switch (buffer[i++]) {
			case '\0':
				return KernelEventClass();
			case '^':
				flags |= 0x200;
				break;
			case '$':
				flags |= 0x100;
				break;
			case '@':
				flags |= 0x400;
				break;
			default: {
				unsigned short key = Unknown_42B610(std::string(&buffer[i - 1]));
				return KernelEventClass(key, flags);
			}
		}
	}
}

bool Unknown_42A410(INIClass &ini, const char *section, const char *entry, const KernelEventClass &event)
{
	if (event.Key == 0) {
		return false;
	}
	const InputFlagsStruct &flags = (const InputFlagsStruct &)event.Flags;
	std::string text;
	if (flags.Is_09()) {
		text += '^';
	}
	if (flags.Is_08()) {
		text += '$';
	}
	if (flags.Is_0A()) {
		text += '@';
	}
	std::string name;
	text += Unknown_42B7E0(KernelEventClass(event.Key), name);
	ini.Put_String(section, entry, text.c_str());
	return true;
}
/*
**	Reconstruction notes
**
**	client.exe: the window 0x429630-0x429F60 (G_ASSERT lines 32, 50, 51, 71, 104 and 120, D25),
**	then the table's instances to 0x42B320 (std::ctype's guarded static: the file's end). Not
**	attributed here: 0x4297A0 (an adjustor thunk, ecx -= 0x84), 0x4299B0 / 0x42A100 (vector
**	deleting destructors of 0xC- and 8-byte objects) and 0x42A170 ((Flags << 16) | Key of an event,
**	no caller): COMDATs of headers the file includes.
**	0x429D00 reads HKLM\SOFTWARE\Westwood Studios\Earth and Beyond, DisableExceptions (DWORD); it
**	only reads (KEY_READ), and nothing in the client calls it directly.
**	Forms that matched (builder H18): the early `return false` when find gives end() (the client's
**	separate failure tails in 0x429A20 / 0x429D90); the iterator declared before the loop and
**	assigned from begin() (0x429800, 0x429E50: begin's result is materialised); in 0x429AF0 the
**	push onto an existing stack through the one-line inline Set_Stack (the depth lever: the
**	client then calls the node's construct 0x42B2C0 out of line there). 0x429F60 is NOT EXACT
**	(register-only, parked after three forms of the search loop).
**	After 0x429F60: the event key 0x42A170 (a member defined after its users: /Ob2 inlines it
**	everywhere and still emits it), the ini reader 0x42A1A0 and writer 0x42A410 (the key names
**	themselves are the next file's, 0x42B610 / 0x42B7E0). The writer tests the flags through
**	bool accessors on bit fields (the client's word load, shr and test 1); the plain shifts or
**	bit fields compile to a byte test and move the std::string appends' inlining.
*/
