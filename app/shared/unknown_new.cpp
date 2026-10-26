// Earth & Beyond client (reconstruction): E&B's global operator new and delete. Notes at the end of the file.
#include <stddef.h>

// wwlib's FastAllocatorGeneral (FastAllocator.h), only the three members used here: the header
// brings wwlib's vector.h, whose placement array new VC6 resolves to operator new[] once this
// file declares one (C2660).
class FastAllocatorGeneral
{
public:
	static FastAllocatorGeneral *Get_Allocator();	// 0x9DAC10
	void *Alloc(unsigned int n);						// 0x9DAE10
	void Free(void *pAlloc);							// 0x9DAF60
};

void *operator new(size_t size)
{
	return FastAllocatorGeneral::Get_Allocator()->Alloc(size);
}

void *operator new[](size_t size)
{
	return FastAllocatorGeneral::Get_Allocator()->Alloc(size);
}

void operator delete(void *memory)
{
	if (memory) {
		FastAllocatorGeneral::Get_Allocator()->Free(memory);
	}
}


/*
**	Reconstruction notes
**
**	client.exe 0x4E9CB0 (new), 0x4E9CE0 (new[]) and 0x4E9D10 (delete): every allocation of the
**	client's own code and of the Westwood libraries linked with it goes through wwlib's
**	FastAllocatorGeneral (Get_Allocator 0x9DAC10, E&B's out-of-line Alloc 0x9DAE10 and Free
**	0x9DAF60). There is no operator delete[] in the object: delete[] falls back to the CRT's, which
**	calls this operator delete. The client has these in one object with the assert reporters
**	(unknown_debug.cpp, whose notes say why the tree keeps them apart). new and new[] are too short
**	for objmatch to search (fewer than 12 fixed bytes); delete is counted.
*/
