// Earth & Beyond client (reconstruction): frameint.cpp, the frame rate's interface (notes at the end)
#define __INTRFACE_MEMBERS	// intrface.h's members (+0x4C's key list)
#include "frameint.h"
#include "sclient.h"
#include "control_keys_manager.h"
#include "ww3d.h"
#include "render2d.h"
#include "texture.h"
#include "dx8wrapper.h"
#include "dx8vertexbuffer.h"
#include "dx8indexbuffer.h"
#include "wwstring.h"
#include "wwmemlog.h"
#include "fastallocator.h"
#include "..\shared\g_assert.h"
#include "..\shared\message.h"

int Unknown_94B910();									// 0x94B910 (the textures' 'Frame: %d')

// 0x53E970
FrameInterfaceClass::FrameInterfaceClass(UIViewClass *view, SimpleSceneClass *scene) :
	InterfaceClass(view, (int)scene),
	FrameRate(0.0f),
	MinFrameRate(99.4f),
	MaxFrameRate(0.0f)
{
	Unknown4C.Add(KernelEventClass(0x54, 0x400), NULL);
}

// 0x53EA70
FrameInterfaceClass::~FrameInterfaceClass()
{
}

// 0x53EAA0
void FrameInterfaceClass::Unknown_54()
{
}

// 0x53EAC0
void FrameInterfaceClass::Unknown_58()
{
	InterfaceClass::Unknown_58();
}

// 0x53EAE0
void FrameInterfaceClass::Unknown_5C()
{
	InterfaceClass::Unknown_5C();
}
















































































































































// NOT EXACT: functional (D64), register-only and order-only (the three subtractions call their right operand first in the client)
// 0x53EB00
void FrameInterfaceClass::Unknown_60()
{
	unsigned frame_time = WW3D::Get_Frame_Time();
	if (frame_time <= 1) {
		frame_time = 1;
	}
	FrameRate = FrameRate * 0.9f + (1.0f / ((float)frame_time * 0.001f)) * 0.1f;

	if (((SClient *)Client)->Unknown14EC == 4) {
		StringClass text;
		unsigned frame_allocs = FastAllocatorGeneral::Get_Allocator()->UnknownE20[2];
		unsigned frame_count = FastAllocatorGeneral::Get_Allocator()->UnknownE20[0];
		unsigned malloc_size = FastAllocatorGeneral::Get_Allocator()->AllocatedWithMalloc;
		unsigned malloc_count = FastAllocatorGeneral::Get_Allocator()->AllocatedWithMallocCount;
		text.Format(
			"Total: %3.3fMb\n\n"
			"Count: %d\n"
			"Size: %3.3fMb\n"
			"Heap: %3.3fMb\n\n"
			"Mallocs: %d (%3.3fMb)\n"
			"FastAllocs: %d (%3.3fMb)\n\n"
			"Frame: %d (%d)\n"
			"Bal. %d\n\n"
			"Textures: %d (%3.3fMb)\n"
			"Thumbs: %d (%3.3fMb)\n"
			"Frame: %d\n\n"
			"VBs: %d (%3.3fMb)\n"
			"IBs: %d (%3.3fMb)\n",
			(float)(FastAllocatorGeneral::Get_Allocator()->Get_Total_Allocated_Size() + VertexBufferClass::Get_Total_Allocated_Memory() + IndexBufferClass::Get_Total_Allocated_Memory() + TextureClass::_Get_Total_Texture_Size()) / (1024.0f * 1024.0f),
			FastAllocatorGeneral::Get_Allocator()->Get_Total_Allocation_Count(),
			(float)FastAllocatorGeneral::Get_Allocator()->Get_Total_Allocated_Size() / (1024.0f * 1024.0f),
			(float)FastAllocatorGeneral::Get_Allocator()->Get_Total_Heap_Size() / (1024.0f * 1024.0f),
			malloc_count,
			(float)malloc_size / (1024.0f * 1024.0f),
			FastAllocatorGeneral::Get_Allocator()->Get_Fast_Allocation_Count(),
			(float)FastAllocatorGeneral::Get_Allocator()->Get_Fast_Allocated_Size() / (1024.0f * 1024.0f),
			frame_count,
			frame_allocs,
			WWMemoryLogClass::Get_Allocate_Count() - WWMemoryLogClass::Get_Free_Count(),
			TextureClass::_Get_Total_Texture_Count() - TextureClass::_Get_Total_Locked_Surface_Count(),
			(float)(TextureClass::_Get_Total_Texture_Size() - TextureClass::_Get_Total_Locked_Surface_Size()) / (1024.0f * 1024.0f),
			TextureClass::_Get_Total_Locked_Surface_Count(),
			(float)TextureClass::_Get_Total_Locked_Surface_Size() / (1024.0f * 1024.0f),
			Unknown_94B910(),
			IndexBufferClass::Get_Total_Buffer_Count(),
			(float)IndexBufferClass::Get_Total_Allocated_Memory() / (1024.0f * 1024.0f),
			VertexBufferClass::Get_Total_Buffer_Count(),
			(float)VertexBufferClass::Get_Total_Allocated_Memory() / (1024.0f * 1024.0f));
		int count = WWMemoryLogClass::Get_Category_Count();
		for (int i = 0; i < count; i++) {
			unsigned memory = WWMemoryLogClass::Get_Current_Allocated_Memory(i);
			if (memory != 0) {
				StringClass line;
				line.Format("%s : %2.2fMb\n", WWMemoryLogClass::Get_Category_Name(i), (float)memory / (1024.0f * 1024.0f));
				text += line;
			}
		}
		((SClient *)Client)->Text->Draw_Text(text);
		WWMemoryLogClass::Reset_Counters();
		FastAllocatorGeneral::Get_Allocator()->Unknown_Reset_Statistics();
	}

	if (((SClient *)Client)->Unknown14EC == 1) {
		StringClass line(255, true);
		StringClass text(1024, true);
		line.Format("FPS = %4.0f\n", FrameRate);
		text += line;
		line.Format("%d bytes free (AGP + Video)\n", DX8Wrapper::Get_Free_Texture_RAM());
		text += line;
		((SClient *)Client)->Text->Draw_Text(text);
	}

	Message_Stat("Render", "", __FILE__, __LINE__, 0, 0, "Frame Rate = %4.0f", FrameRate);
	if (FrameRate < MinFrameRate) {
		MinFrameRate = FrameRate;
	}
	if (FrameRate > MaxFrameRate) {
		MaxFrameRate = FrameRate;
	}
}
#pragma auto_inline(off)

// 0x53F200
unsigned FastAllocatorGeneral::Get_Fast_Allocated_Size()
{
	int size=0; for (int i=0;i<MAX_ALLOC_SIZE/ALLOC_STEP;++i) { FastCriticalSectionClass::LockClass lock(CriticalSections[i]); size+=allocators[i].Get_Allocated_Size(); } return size;
}

// 0x53F250
unsigned FastAllocatorGeneral::Get_Fast_Allocation_Count()
{	int count=0; for (int i=0;i<MAX_ALLOC_SIZE/ALLOC_STEP;++i) { FastCriticalSectionClass::LockClass lock(CriticalSections[i]); count+=allocators[i].Get_Allocation_Count(); } return count; }
#pragma auto_inline(on)

// 0x53F2A0
bool FrameInterfaceClass::Unknown_68(int key)
{
	switch (key) {
		case 0:
			MinFrameRate = 99.4f;
			MaxFrameRate = 0.0f;
			break;
		default:
			G_ASSERT(0);
			return(false);
	}
	return(true);
}

/*
**	Reconstruction notes (from the retail client.exe, SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\client\frameint.cpp (its __FILE__ 0xB790A4: Message_Stat 'Render' on line 269,
**	G_ASSERT '0' on line 299). Client 0x53E970-0x53F338: the constructor (its key 0x54 with modifier 0x400 reaches slot
**	+0x68 as key 0), the deleting destructor 0x53EA40 and the destructor, slots +0x54 (empty) / +0x58 / +0x5C / +0x60 /
**	+0x68, StringClass(int, bool)'s out-of-line copy 0x53F1C0, FastAllocatorGeneral's two sums 0x53F200 / 0x53F250
**	(defined here after slot +0x60, not inlined into it: auto_inline off), <string>'s guarded initialiser 0x53F2F0 and the closing thunks 0x53F320 / 0x53F330. The class name follows the file (D24).
**	The client's display mode (SClient +0x14EC) picks what slot +0x60 draws with the client's text (+0x13C4): 4 the
**	memory statistics, 1 the frame rate and the free texture memory.
*/
