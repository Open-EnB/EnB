#include "unknown_67d430.h"
#include "option.h"
#include <string.h>

Unknown67D430Class::Unknown67D430Class(unsigned int capacity)
{
	Level = 0;
	Capacity = capacity < 1 ? 1 : capacity;
	Count = 0;
	Entries = NULL;
	memset(Colors, -1, sizeof(Colors));
	Entries = new Unknown67D430EntryStruct[capacity];
}

Unknown67D430Class::~Unknown67D430Class()
{
	delete [] Entries;
	Entries = NULL;
}

void Unknown67D430Class::Unknown_67D660(int level)
{
	Level = level;
	int max = level - 1;
	Colors[0] = max > 1 ? 1 : max;
	Colors[1] = max > 1 ? 1 : max;
	Colors[2] = max > 0 ? 0 : max;
	Colors[3] = max > 0 ? 0 : max;
	Colors[4] = max > 0 ? 0 : max;
	Colors[5] = max > 1 ? 1 : max;
	Colors[6] = max > 1 ? 1 : max;
	Colors[7] = max > 0 ? 0 : max;
	Colors[8] = max > 0 ? 0 : max;
	Colors[9] = max > 0 ? 0 : max;
	Colors[10] = max > 0 ? 0 : max;
	Colors[11] = max > 0 ? 0 : max;
	Colors[12] = max > 0 ? 0 : max;
	Colors[14] = max > 0 ? 0 : max;
	Colors[15] = max > 0 ? 0 : max;
	Colors[13] = max > 0 ? 0 : max;
	Colors[16] = max > 0 ? 0 : max;
	Colors[17] = max > 2 ? 2 : max;
	Colors[18] = max > 2 ? 2 : max;
	Colors[19] = max > 2 ? 2 : max;
	Colors[20] = max > 1 ? 1 : max;
	Colors[21] = max > 2 ? 2 : max;
	Colors[22] = max > 1 ? 1 : max;
	Colors[23] = max > 3 ? 3 : max;
}

Unknown67D430EntryStruct &Unknown67D430Class::Unknown_67D780(int type, const std::string &text)
{
	unsigned int index = Count++ % Capacity;
	Unknown67D430EntryStruct &entry = Entries[index];
	entry.Type = type;
	entry.Text = text;
	return Entries[index];
}

const Unknown67D430EntryStruct &Unknown67D430Class::Unknown_67D980(unsigned int index, int *color)
{
	static Unknown67D430EntryStruct null_entry;
	int &out = color ? *color : *(int *)&color;
	unsigned int count = Capacity < Count ? Capacity : Count;
	if (index >= count) {
		out = -1;
		return null_entry;
	}
	if (Count >= Capacity) {
		index += Count - Capacity;
	}
	const Unknown67D430EntryStruct *entry = &Entries[index % Capacity];
	out = Unknown_67DB40(entry->Get_Type());
	return *entry;
}

const Unknown67D430EntryStruct &Unknown67D430Class::Unknown_67DA60(unsigned int index, int *color)
{
	static Unknown67D430EntryStruct null_entry;
	int &out = color ? *color : *(int *)&color;
	unsigned int count = Capacity < Count ? Capacity : Count;
	if (index >= count) {
		out = -1;
		return null_entry;
	}
	const Unknown67D430EntryStruct *entry = &Entries[(Count - index - 1) % Capacity];
	out = Unknown_67DB40(entry->Get_Type());
	return *entry;
}

int Unknown67D430Class::Unknown_67DB40(int type)
{
	switch (type) {
		case 2:		if (!OptionClass::Instance->Get_Bool("Channel_Broadcast")) return -1;	break;
		case 7:		if (!OptionClass::Instance->Get_Bool("Channel_Local")) return -1;		break;
		case 4:		if (!OptionClass::Instance->Get_Bool("Channel_Group")) return -1;		break;
		case 8:		if (!OptionClass::Instance->Get_Bool("Channel_Guild")) return -1;		break;
		case 3:		if (!OptionClass::Instance->Get_Bool("Chat_Tell")) return -1;			break;
		case 6:		if (!OptionClass::Instance->Get_Bool("Chat_Tips")) return -1;			break;
		case 5:		if (!OptionClass::Instance->Get_Bool("Chat_NPC")) return -1;			break;
		case 21:	if (!OptionClass::Instance->Get_Bool("Chat_Warnings")) return -1;		break;
		case 22:	if (!OptionClass::Instance->Get_Bool("Chat_Attention")) return -1;		break;
		case 17:	if (!OptionClass::Instance->Get_Bool("Chat_Errors_IC")) return -1;		break;
		case 0:		if (!OptionClass::Instance->Get_Bool("Chat_Status_IC")) return -1;		break;
	}
	return Colors[type];
}

/*
**	Reconstruction notes (client.exe, the object 0x67D430-0x67DC6F).
**	PLACEMENT: provisional (D137): a string, assert or link map naming the object. Its bounds are firm: every object that
**	includes <string> ends its code with the same 32-byte guarded initialiser of std::ctype<unsigned short>::id ($E<n>,
**	guard 0xC416FC), and those stand at 0x67D400 (charview.cpp's last), 0x67DC50 (this object's last), 0x681270
**	(chat_window.cpp's last) and 0x6831B0 (chat_window_history.cpp's last). It links between charview.obj and
**	chat_window.obj (app\client, in name order), so its real name sorts between those two.
**
**	Only ui_chat_window.cpp's functions call it: 0x5629E0 (constructor), 0x562440 (destructor), 0x56FD30 (0x67D660),
**	0x563760 (0x67D780).
**
**	The 0x67D980, 0x67DA60 and 0x67DB40 markers report PREFIX: function_starts lacks the atexit thunks 0x67DA40 and 0x67DB20
**	and the initialiser 0x67DC50, which follow them. Their own bytes are the client's.
*/
