// Earth & Beyond client (reconstruction): app\shared\px_process.cpp. Notes at the end of the file.
#include "px_process.h"
#include "..\shared\unknown_debug.h"

// 0x455880
PxProcessNode::PxProcessNode() :
	Next(NULL),
	Prev(NULL)
{
}

// 0x455940
PxProcessNode::~PxProcessNode()
{

	FAIL_IF(Next || Prev) {
		Prev->Next = Next; Next->Prev = Prev; Prev = NULL; Next = NULL;
	}
}

// 0x4559B0
void PxProcessNode::Remove()
{
	Prev->Next = Next; Next->Prev = Prev; Prev = NULL; Next = NULL;
}
std::vector<PxProcessClass *> PxProcessClass::Stack;
// 0x455A30
void PxProcessNode::Process()
{
}
// 0x455A90
PxProcessClass::PxProcessClass()
{
	Root.Prev = &Root; Root.Next = &Root;
}

// 0x455AB0
PxProcessClass::~PxProcessClass()
{
	FAIL_IF(Root.Next != &Root) {
		while (Root.Next != &Root) Root.Next->Remove();
	}
	Root.Prev = NULL; Root.Next = NULL;
}

// 0x455B70
void PxProcessClass::Add(PxProcessNode *node)
{
	FAIL_IF(!node) return;
	FAIL_IF(node->Next || node->Prev) return;
	node->Next = &Root;
	node->Prev = Root.Prev;
	Root.Prev->Next = node;
	Root.Prev = node;
}

// 0x455BF0
void PxProcessClass::Remove(PxProcessNode *node)
{

	FAIL_IF(!node) return;
	FAIL_IF(node->Next == 0 || node->Prev == 0) return;
	node->Remove();
}

// 0x455C80
void PxProcessClass::Remove_All()
{
	while (Root.Next != &Root) {
		Root.Next->Remove();
	}
}

// 0x455CC0
void PxProcessClass::Process()
{
	for (PxProcessNode *node = Root.Next; node != &Root; node = node->Next) {
		node->Process();
	}
}

// 0x455CF0
PxProcessClass *PxProcessClass::Get_Current()
{
	return(Stack.back());
}

// 0x455D10
// NOT EXACT: shape (the client's inlined insert knows _Last - _P is 0 (xor dl, dl) and computes max(size(), 1) with ja; ours reloads; attempts.py show 0x455D10)
void PxProcessClass::Push(PxProcessClass *process)
{
	Stack.push_back(process);
}

// 0x455F60
// NOT EXACT: shape (register: the client keeps the old end in esi and copies through edx; attempts.py show 0x455F60)
void PxProcessClass::Pop()
{
	Stack.pop_back();
}

/*
** Notes
**
** px_process.cpp is client 0x455880-0x4560B0 (its __FILE__ in the FAIL_IFs, lines 16-62), after PxObject's three
** functions 0x4557F0-0x455860 (no string places them) and before px_wrapper.cpp. The class name PxProcessClass is
** client.h's (D24); PxProcessNode, the function names and the header name are provisional; Next, Prev, Root and
** node are the asserts' names. The vector<PxProcessClass *> members 0x455A50 (destructor) and 0x455FB0-0x456080
** are this file's instances; 0x4559E0 / 0x455A10 construct Stack and register its destructor.
**
** Use in the retail client: ClientClass's constructor 0x726100 makes the list and its destructor deletes it;
** SClient::Init 0x741180, Shutdown 0x741BB0 and Do_Frame 0x742B60 push it at their start and pop it at their end
** (sclient.cpp still calls them as Unknown_455D10 / Unknown_455F60). px_callback_accurate.cpp's callbacks
** (0x44E470-0x44F6E0, 0x452AF0, 0x452B40) add and remove themselves on Get_Current(). Process 0x455CC0 and
** Remove_All 0x455C80 have no call, jump or pointer reference (their thunks 0x40B52D / 0x410EF1 included): the nodes
** are kept but never processed.
*/
