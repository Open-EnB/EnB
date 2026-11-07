// Earth & Beyond client (reconstruction): notes at the end of the file
#include "delta.h"
#include <queue>

// PLACEMENT: provisional (D137): no vtable, caller or string names it; a client string or a caller would place it.
class DeltaQueueVisitorClass : public DeltaVisitorClass	// NAME 1: neutral, what it does (D10)
{
public:
	void Visit_Breadth_First(DeltaCompoundClass *delta);	// 0x4B0680  // NAME 2: what it does

	std::queue<DeltaClass *> Queue;							// +0x04  // NAME 2: what it holds
};

// @0x4B0680
void DeltaQueueVisitorClass::Visit_Breadth_First(DeltaCompoundClass *delta)
{
	int count = delta->Children.size();
	for (int i = 0; i < count; i++) {
		DeltaClass *child = delta->Children[i];
		Queue.push(child);
	}
	while (!Queue.empty()) {
		DeltaClass *child = Queue.front();
		Queue.pop();
		child->Accept(*this);
	}
}

/*
**	Reconstruction notes
**
**	client.exe 0x4B0680-0x4B0A29 (no assert, no __FILE__, no string, no <string> initialiser: the file name is
**	provisional, D137). policy.cpp's object ends with its static initialiser at 0x4B0660; kv_acc.cpp follows at
**	0x4B0A90. In order:
**	* 0x4B0680, a breadth-first walk: the compound's children go into a std::queue (the deque at +0x04, its size at
**	  +0x30), then each is popped and visited. No caller and no vtable refers to it. std::queue, not a bare deque:
**	  its extra inline layer is what leaves two of push_back's three _Construct calls out of line (0x4A6040).
**	* DeltaVisitorClass::Visit_Children 0x4B0860 belongs here by its address; it stays in unknown_root.cpp for now:
**	  moved here, unknown_root.obj lost its Create_NEW pair 0x4A46F0 (a 21-byte body with several client copies,
**	  placed by the object's other pairs), an outright loss.
**	* deque<DeltaClass *>::_Buyback 0x4B08D0, the queue's instance.
*/
