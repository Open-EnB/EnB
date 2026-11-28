// Earth & Beyond client (reconstruction): app\shared\px_process.cpp. Notes at the end of the file.
#include "px_process.h"
#include "..\shared\g_assert.h"

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
static struct PxProcessStack : public std::vector<PxProcessList *> { PxProcessStack(const std::allocator<PxProcessList *> &alloc = std::allocator<PxProcessList *>()) : std::vector<PxProcessList *>(alloc) {} } Stack;  // NAME 2: Push / Pop's, and the wrapper's name (0xBDC080; notes)
// 0x455A30
void PxProcessNode::Process()
{
}
// 0x455A90
PxProcessList::PxProcessList()
{
	Root.Prev = &Root; Root.Next = &Root;
}

// 0x455AB0
PxProcessList::~PxProcessList()
{
	FAIL_IF(Root.Next != &Root) {
		while (Root.Next != &Root) Root.Next->Remove();
	}
	Root.Prev = NULL; Root.Next = NULL;
}

// 0x455B70
void PxProcessList::Add(PxProcessNode *node)
{
	FAIL_IF(!node) return;
	FAIL_IF(node->Next || node->Prev) return;
	node->Next = &Root;
	node->Prev = Root.Prev;
	Root.Prev->Next = node;
	Root.Prev = node;
}

// 0x455BF0
void PxProcessList::Remove(PxProcessNode *node)
{

	FAIL_IF(!node) return;
	FAIL_IF(node->Next == 0 || node->Prev == 0) return;
	node->Remove();
}

// 0x455C80
void PxProcessList::Remove_All()
{
	while (Root.Next != &Root) {
		Root.Next->Remove();
	}
}

// 0x455CC0
void PxProcessList::Process()
{
	for (PxProcessNode *node = Root.Next; node != &Root; node = node->Next) {
		node->Process();
	}
}

// 0x455CF0
PxProcessList *PxProcessList::Get_Current()
{
	return(Stack.back());
}

// 0x455D10
void PxProcessList::Push(PxProcessList *process)
{
	Stack.push_back(process);
}

// 0x455F60
void PxProcessList::Pop()
{
	Stack.pop_back();
}

/*
** Notes
**
** px_process.cpp is client 0x455880-0x4560B0 (its __FILE__ in the FAIL_IFs, lines 16-62), after PxObject's three
** functions 0x4557F0-0x455860 (no string places them) and before px_wrapper.cpp. The class name PxProcessList is
** client.h's (D24); PxProcessNode, the function names and the header name are provisional; Next, Prev, Root and
** node are the asserts' names. 0x4559E0 / 0x455A10 construct Stack and register its destructor: the stub is
** unguarded, so Stack is a file static, not a class static member (that one's stub is guarded); its type is a
** wrapper (PxProcessStack) whose constructor 0x455FB0 and implicit destructor 0x455A50 the client
** calls out of line, where a bare vector's inline into the $E. The vector<PxProcessList *> members
** 0x455FE0-0x456080 are this file's instances.
**
** Use in the retail client: ClientClass's constructor 0x726100 makes the list and its destructor deletes it;
** SClient::Init 0x741180, Shutdown 0x741BB0 and Do_Frame 0x742B60 push it at their start and pop it at their end
** (sclient.cpp calls them as PxProcessList::Push / Pop). px_callback_accurate.cpp's callbacks
** (0x44E470-0x44F6E0, 0x452AF0, 0x452B40) add and remove themselves on Get_Current(). Process 0x455CC0 and
** Remove_All 0x455C80 have no call, jump or pointer reference (their thunks 0x40B52D / 0x410EF1 included): the nodes
** are kept but never processed.
*/
