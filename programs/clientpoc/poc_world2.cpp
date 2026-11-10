// Earth & Beyond client (reconstruction): the POC's stand-ins for the functions gen_stubs can't write (D179). NOT THE CLIENT'S CODE.
/*
**	gen_stubs cannot write a stand-in with a structure returned or passed by value or a vector of structures: the
**	calls go to the PE header (the crash at EIP 0x400003-5, with the return address popped). These are what the way
**	into the world reaches, written from the pseudocode where a real result matters, empty where it doesn't.
*/
#include <string.h>
#include <vector>
#include "..\..\stubs\stub_log.h"
#include "..\..\app\client\sclient.h"
#include "..\..\app\client\co.h"
#include "..\..\app\client\gadget.h"

// ---------------------------------------------------------------- 0x725E70: UnknownB02C90Class's buffer (client.cpp's Mount lines)
void UnknownB02C90Class::BufferStruct::Insert(const ElementStruct &element, int index)
{
	STUB_CALLED("POC BufferStruct::Insert (0x725E70)");
	if (Count >= Unknown04) {
		int size = Count * 2;
		if (size < 10) {
			size = 10;
		}
		ElementStruct *data = new ElementStruct[size];
		for (int i = 0; i < Count; i++) {
			data[i] = Data[i];
		}
		delete [] Data;
		Data = data;
		Unknown04 = size;
	}
	for (int i = Count; i > index; i--) {
		Data[i] = Data[i - 1];
	}
	Data[index] = element;
	Count++;
}

// 0x725AD0: the owner's thrust transform times Unknown1C: the transform alone here
Matrix3D UnknownB02C90Class::Unknown_725AD0()
{
	STUB_CALLED("POC UnknownB02C90Class::Unknown_725AD0");
	return(Unknown1C);
}

// ---------------------------------------------------------------- 0x80F220 (shiputils.cpp): the model's colours: left as loaded
struct ColorizationRegionStruct;
void Unknown_80F220(RenderObjClass *model, std::vector<ColorizationRegionStruct> &regions)
{
	STUB_CALLED("POC Unknown_80F220 (0x80F220, the ship's colours): nothing");
}

// ---------------------------------------------------------------- 0x65F780: sclient.cpp's Do_Frame declares the
// gadgets' input pass as a free function; it is gadget.cpp's GadgetClass::Process_Input (tsk_login.cpp calls it by
// that name), so the world's windows get the mouse as at login (was an empty event: nothing in the world took a click)
KernelEventClass Unknown_65F780(void *unknown)
{
	return(GadgetClass::Process_Input((GadgetListManagerClass *)unknown));
}

// ---------------------------------------------------------------- the client's view of keys no gadget took: nothing
void SClient::Unknown_744240(KernelEventClass event)
{
}

// ---------------------------------------------------------------- 0x73FDB0: the watcher's list node moved onto the object's list (from the pseudocode)
void UnknownAF0DD0Class::Unknown_73FDB0(DataNode<UnknownAF0DD0Class *> *node, ClientObjectClass *object)
{
	int *n = (int *)node;
	if (n[3] != 0) {
		if (n[1] != 0 && n[2] != 0) {
			((int *)n[2])[1] = n[1];
			((int *)n[1])[2] = n[2];
			n[2] = 0;
			n[1] = 0;
		}
		n[3] = 0;
	}
	if (object != NULL) {
		n[3] = (int)this;
		char *list = (char *)object->Unknown_6F5090();
		if (n[1] != 0 && n[2] != 0) {
			((int *)n[2])[1] = n[1];
			((int *)n[1])[2] = n[2];
			n[2] = 0;
			n[1] = 0;
		}
		int first = *(int *)(list + 8);
		n[2] = (int)(list + 4);
		n[1] = first;
		if (first != 0) {
			((int *)first)[2] = (int)n;
		}
		*(int *)(list + 8) = (int)n;
	}
}

// ---------------------------------------------------------------- 0x73FA60: the object's watcher list (a GenericList, 0x1C bytes): its two end nodes linked
Unknown8CClass::Unknown8CClass()
{
	STUB_CALLED("POC Unknown8CClass: the watchers' list");
	int *p = (int *)this;
	p[1] = p[2] = p[3] = p[4] = p[5] = p[6] = 0;
	p[6] = (int)(p + 1);
	p[2] = (int)(p + 4);
}

// ---------------------------------------------------------------- a watchdog (POC only): every 5 s the main thread's EIP and the code addresses on its stack, in the stub log
#include <windows.h>
#include <stdio.h>
static HANDLE Poc_Main_Thread;
static DWORD WINAPI Poc_Watchdog(void *)
{
	for (int n = 0; n < 60; n++) {
		Sleep(5000);
		SuspendThread(Poc_Main_Thread);
		CONTEXT c;
		c.ContextFlags = CONTEXT_FULL;
		char text[512];
		int len = 0;
		if (GetThreadContext(Poc_Main_Thread, &c)) {
			len += sprintf(text + len, "POC watchdog %d: %08X stack:", n, c.Eip);
			DWORD *sp = (DWORD *)c.Esp;
			int found = 0;
			for (int i = 0; i < 400 && found < 12; i++) {
				if (IsBadReadPtr(sp + i, 4)) {
					break;
				}
				if (sp[i] >= 0x401000 && sp[i] < 0x6A0000) {
					len += sprintf(text + len, " %08X", sp[i]);
					found++;
				}
			}
		}
		ResumeThread(Poc_Main_Thread);
		int seen = 0;
		Stub_Log_First_Call(text, &seen);
	}
	return(0);
}
void Poc_Start_Watchdog()
{
	DuplicateHandle(GetCurrentProcess(), GetCurrentThread(), GetCurrentProcess(), &Poc_Main_Thread, 0, FALSE, DUPLICATE_SAME_ACCESS);
	DWORD id;
	CreateThread(NULL, 0, Poc_Watchdog, NULL, 0, &id);
}

// co.cpp's render hook 0x6BB7E0 / 0x6BB8B0 is real app code since WE5 (app/client/unknown_6bb7e0.cpp).
