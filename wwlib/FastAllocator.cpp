/*
**	Command & Conquer Renegade(tm)
**	Copyright 2025 Electronic Arts Inc.
**	Modified for the Earth & Beyond client reconstruction: tools/vc6/importer.py records each change.
**	This program is free software: you can redistribute it and/or modify
**	it under the terms of the GNU General Public License as published by
**	the Free Software Foundation, either version 3 of the License, or
**	(at your option) any later version.
**
**	This program is distributed in the hope that it will be useful,
**	but WITHOUT ANY WARRANTY; without even the implied warranty of
**	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**	GNU General Public License for more details.
**
**	You should have received a copy of the GNU General Public License
**	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "FastAllocator.h"

static FastAllocatorGeneral* generalAllocator; //This general allocator will do all allocations for us.

FastAllocatorGeneral* FastAllocatorGeneral::Get_Allocator()
{
	if (!generalAllocator) {
		generalAllocator=reinterpret_cast<FastAllocatorGeneral*>(::malloc(sizeof(FastAllocatorGeneral)));

		new (generalAllocator) FastAllocatorGeneral();
	}
	return generalAllocator;
}

FastAllocatorGeneral::FastAllocatorGeneral() : MemoryLeakLogEnabled(false), AllocatedWithMalloc(0), AllocatedWithMallocCount(0), ActualMemoryUsage(0), m_UnknownE18(0), m_UnknownE1C(0)
{
	int alloc_size=ALLOC_STEP;
	for (int i=0;i<MAX_ALLOC_SIZE/ALLOC_STEP;++i) {
	   allocators[i].Init(alloc_size);
		alloc_size+=ALLOC_STEP;
	}
}

unsigned FastAllocatorGeneral::Get_Total_Heap_Size()
{
	int size=AllocatedWithMalloc;
	for (int i=0;i<MAX_ALLOC_SIZE/ALLOC_STEP;++i) {
		FastCriticalSectionClass::LockClass lock(CriticalSections[i]);
		size+=allocators[i].Get_Heap_Size();
	}
	return size;
}

unsigned FastAllocatorGeneral::Get_Total_Allocated_Size()
{
	int size=AllocatedWithMalloc;
	for (int i=0;i<MAX_ALLOC_SIZE/ALLOC_STEP;++i) {
		FastCriticalSectionClass::LockClass lock(CriticalSections[i]);
		size+=allocators[i].Get_Allocated_Size();
	}
	return size;
}

unsigned FastAllocatorGeneral::Get_Total_Allocation_Count()
{
	int count=AllocatedWithMallocCount;
	for (int i=0;i<MAX_ALLOC_SIZE/ALLOC_STEP;++i) {
		FastCriticalSectionClass::LockClass lock(CriticalSections[i]);
		count+=allocators[i].Get_Allocation_Count();
	}
	return count;
}

// E&B (name not known): latches and clears the usage counters (0x9DADA0; called from the app at 0x53EE47
// and 0x6A1599)
void FastAllocatorGeneral::Unknown_Reset_Statistics()
{
	{
		FastCriticalSectionClass::LockClass lock(m_UnknownE00);
		m_UnknownE20[2]=m_UnknownE20[3];
		m_UnknownE20[3]=0;
	}
	{
		FastCriticalSectionClass::LockClass lock(m_UnknownE04);
		m_UnknownE18=m_UnknownE1C;
		m_UnknownE20[0]=m_UnknownE20[1];
		m_UnknownE1C=0;
		m_UnknownE20[1]=0;
	}
}

// E&B's Alloc (0x9DAE10, out of line): no re-entrancy counter; usage statistics under their own lock,
// the malloc path under another (both locks and the extra counters are E&B additions)
void* FastAllocatorGeneral::Alloc(unsigned int n)
{
	void* pMemory;

	//We actually allocate n+4 bytes. We store the # allocated
	//in the first 4 bytes, and return the ptr to the rest back
	//to the user.
	n += sizeof(unsigned int);

	{
		FastCriticalSectionClass::LockClass lock(m_UnknownE04);
		ActualMemoryUsage+=n;
		m_UnknownE1C+=n;
		m_UnknownE20[1]++;
	}
	if (n<MAX_ALLOC_SIZE) {
		unsigned index=n/ALLOC_STEP;
		FastCriticalSectionClass::LockClass lock(CriticalSections[index]);
		m_UnknownE1C+=n;
		pMemory = allocators[index].Alloc();
	}
	else {
		FastCriticalSectionClass::LockClass lock(m_UnknownE00);
		AllocatedWithMalloc+=n;
		AllocatedWithMallocCount++;
		pMemory = ::malloc(n);
	}

	*((unsigned int*)pMemory) = n;     //Write modified (augmented by 4) count into first four bytes.
	return ((unsigned int*)pMemory)+1; //return ptr to bytes after it back to user.
}

// E&B's Free (0x9DAF60, out of line): the malloc path under the 0xE00 lock, the usage counter under the
// 0xE04 lock afterwards (see Alloc)
void FastAllocatorGeneral::Free(void* pAlloc)
{
	if (pAlloc) {
		unsigned int* n = ((unsigned int*)pAlloc)-1; //Subtract four bytes and the count is stored there.

		unsigned size=*n;

		if (size<MAX_ALLOC_SIZE) {
			unsigned index=size/ALLOC_STEP;
			FastCriticalSectionClass::LockClass lock(CriticalSections[index]);
			allocators[index].Free(n);
		}
		else {
			FastCriticalSectionClass::LockClass lock(m_UnknownE00);
			AllocatedWithMallocCount--;
			AllocatedWithMalloc-=size;
			::free(n);
		}
		{
			FastCriticalSectionClass::LockClass lock(m_UnknownE04);
			ActualMemoryUsage-=size;
		}
	}
}

