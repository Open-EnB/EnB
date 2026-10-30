// Earth & Beyond client (reconstruction)
/*
**	Original location: e:\build\beta\G\app\shared\. px_process.cpp is the client's (its __FILE__, client
**	0x455880-0x4560B0); this header's name is provisional: no string names it.
**
**	A list of nodes to process (PxProcessClass, ClientClass's PxProcess at +0x0C) and a static stack of the current
**	list: SClient::Init, Shutdown and Do_Frame push their client's list around their work, and px_callback_accurate.cpp's
**	callbacks add themselves to the current one (Get_Current, Add). Nothing in the client calls Process 0x455CC0 or
**	Remove_All 0x455C80.
*/
#if defined(_MSC_VER)
#pragma once
#endif
#ifndef PX_PROCESS_H  // NAME 1 [px_process.h]: provisional header name (px_process.cpp's)
#define PX_PROCESS_H

#include <vector>

/*
** One entry of a PxProcessClass (0x0C bytes, vtable 0xAE7CE8: the deleting destructor and Process, empty here); a
** base of px_callback_accurate.cpp's and px_callback_poll.cpp's classes and of the loose box (its +0x30).
*/
class PxProcessNode  // NAME 2: px_process.cpp's list entries (the asserts' 'node')
{
public:
	PxProcessNode();						// 0x455880
	virtual ~PxProcessNode();				// 0x455940 (FAIL_IF 'Next || Prev' l.16; deleting 0x4558B0)

	virtual void Process();					// 0x455A30 (empty)  // NAME 2: what PxProcessClass::Process calls
	void Remove();							// 0x4559B0 (unlinks it; inlined at every use)  // NAME 2: what it does

	PxProcessNode *Next;					// +0x04 (the client's name, the FAIL_IFs 'Next || Prev', 'node->Next')
	PxProcessNode *Prev;					// +0x08 (as above)
};

/*
** The list (0x0C bytes: the sentinel Root, its own Next and Prev while empty).
*/
class PxProcessClass  // NAME 3: class-from-file (D24) for px_process.cpp
{
public:
	PxProcessClass();						// 0x455A90
	~PxProcessClass();						// 0x455AB0 (FAIL_IF 'Root.Next != &Root' l.40)

	void Add(PxProcessNode *node);			// 0x455B70 (FAIL_IFs l.49 / 50: at the end)  // NAME 2: what it does
	void Remove(PxProcessNode *node);		// 0x455BF0 (FAIL_IFs l.61 / 62)  // NAME 2: what it does
	void Remove_All();						// 0x455C80 (no caller)  // NAME 2: what it does
	void Process();							// 0x455CC0 (every node's Process; no caller)  // NAME 2: what it does

	static PxProcessClass *Get_Current();				// 0x455CF0 (the stack's top)  // NAME 2: what it returns
	static void Push(PxProcessClass *process);			// 0x455D10 (SClient::Init / Shutdown / Do_Frame)  // NAME 2 [process]: what it does
	static void Pop();									// 0x455F60 (as above, at their ends)  // NAME 2: what it does

	PxProcessNode Root;						// +0x00 (the client's name, the FAIL_IF 'Root.Next != &Root')

	static std::vector<PxProcessClass *> Stack;	// 0xBDC080 (constructed by 0x4559E0)  // NAME 2: Push / Pop's
};

#endif
