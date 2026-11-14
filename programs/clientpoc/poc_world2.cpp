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

// ---------------------------------------------------------------- 0x6B6050-0x6B6170: the starbase interface's +0xB8 gadget (declared in ui_starbase.cpp)
// The client's five functions as they are (table 0xAFE174); the generated constructor left the object without its table, and
// intrlist.cpp's 0x6871D0 called through it from UIStarbaseClass's constructor. Its own app file is still to make (the client's
// object sits between viewbttn.cpp and wnd_file.cpp: defining it in ui_starbase.cpp widened that file's range and lost two pairs)
#include "..\..\app\client\gadget.h"
class SBaseViewClass;
class UIStarbaseGadget6B6050Class : public RectangleGadgetClass
{	public:
	UIStarbaseGadget6B6050Class(int x, int y, int width, int height, SBaseViewClass *view);
	virtual ~UIStarbaseGadget6B6050Class(); virtual GadgetClass *Unknown_1C(); virtual bool Unknown_2C(KernelEventClass *event, float x, float y, unsigned int mask); virtual bool Unknown_34(KernelEventClass *event, float x, float y, unsigned int mask);
	SBaseViewClass *Unknown40;
};
// 0x6B6050: RectangleGadgetClass(x, y, width, height, 0xFF), then the view
UIStarbaseGadget6B6050Class::UIStarbaseGadget6B6050Class(int x, int y, int width, int height, SBaseViewClass *view) : RectangleGadgetClass(x, y, width, height, 0xFF), Unknown40(view)
{
}
// 0x6B60A0: slot +0x1C answers with the gadget itself
GadgetClass *UIStarbaseGadget6B6050Class::Unknown_1C()
{
	return(this);
}
// 0x6B60F0 (deleting 0x6B60C0): the table, then GadgetClass's destructor
UIStarbaseGadget6B6050Class::~UIStarbaseGadget6B6050Class()
{
}
// 0x6B6110: no event; true when the mask holds a bit of its own (+0x10)
bool UIStarbaseGadget6B6050Class::Unknown_2C(KernelEventClass *event, float x, float y, unsigned int mask)
{
	return((mask & Mask) != 0);
}
// 0x6B6130: the base's; when it declined, bits 0x44 go to slot +0x2C and the answer is false
bool UIStarbaseGadget6B6050Class::Unknown_34(KernelEventClass *event, float x, float y, unsigned int mask)
{
	if (GadgetClass::Unknown_34(event, x, y, mask)) {
		return(true);
	}
	if (mask & 0x44) {
		Unknown_2C(event, x, y, mask & 0x44);
	}
	return(false);
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
