// Earth & Beyond client (reconstruction): app\input\winmsg.cpp (PROVISIONAL name). Notes at the end of the file.
#include "winmsg.h"
#include "..\..\wwlib\slist.h"

// One handler of a message (8 bytes)
struct MessageHandlerStruct	// NAME 1 [MessageHandlerStruct]: neutral (D10)
{
	MessageHandlerStruct(int message, int (*handler)(int, int)) : Message(message), Handler(handler) {}

	int Message;					// +0x00  // NAME 2: what it holds
	int (*Handler)(int, int);		// +0x04: called with (message, wParam, lParam)  // NAME 2: what it holds
};

static SList<MessageHandlerStruct> Unknown_BDAAD8;	// NAME 1 [Unknown_BDAAD8]: neutral (D10)

void Unknown_42F940(int message, int (*handler)(int, int))
{
	Unknown_BDAAD8.Add_Head(new MessageHandlerStruct(message, handler));
}

// NOT EXACT: shape: the client keeps Remove's HeadNode test after the search, ours drops it (attempts.csv)
void Unknown_42FAC0(int message, int (*handler)(int, int))
{
	for (SLNode<MessageHandlerStruct> *node = Unknown_BDAAD8.Head(); node != NULL; node = node->Next()) {
		MessageHandlerStruct *entry = node->Data();
		if (entry->Message == message && entry->Handler == handler) {
			Unknown_BDAAD8.Remove(entry);
			delete entry;
			return;
		}
	}
}

int Unknown_42FBA0(int message, int wparam, int lparam)
{
	int result = -1;
	for (SLNode<MessageHandlerStruct> *node = Unknown_BDAAD8.Head(); node != NULL; node = node->Next()) {
		if (node->Data()->Message == message) {
			result = ((int (*)(int, int, int))node->Data()->Handler)(message, wparam, lparam);
		}
	}
	return result;
}
/*
**	Reconstruction notes
**
**	client.exe: the object after input.cpp's, 0x42F880-0x430450 (no __FILE__, no assert: the file's
**	name and its place beside w32kbd.cpp are provisional, D137): the list's dynamic initializer and
**	its atexit destructor 0x42F880 / 0x42F8C0, the add 0x42F940 and dispatch 0x42FBA0 exact, the
**	remove 0x42FAC0 NOT EXACT (shape), then wwlib's SList<T> instances 0x42FBF0-0x430340 and the node
**	pool's Allocate_Object_Memory / Free_Object_Memory 0x430370 / 0x430450. Its statics are the
**	list 0xBDAAD8 (vtable, head 0xBDAADC, tail 0xBDAAE0). The handlers are called with (message,
**	wParam, lParam) through the two-argument type the callers declare.
**	Forms that matched: 0x42F940 with the entry's constructor in the Add_Head call (new's null test
**	and Add_Head's merge; assigning the members after new keeps Add_Head out of line).
*/
