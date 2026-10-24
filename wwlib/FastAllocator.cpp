/*
**	Command & Conquer Renegade(tm)
**	Copyright 2025 Electronic Arts Inc.
**
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

FastAllocatorGeneral::FastAllocatorGeneral() : m_UnknownE04(0), MemoryLeakLogEnabled(false), AllocatedWithMalloc(0), AllocatedWithMallocCount(0), ActualMemoryUsage(0), m_UnknownE18(0), m_UnknownE1C(0)
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

