// Earth & Beyond client (reconstruction): the forwards that differ from poc_gadget_views.cpp only in the return type (D179). NOT THE CLIENT'S CODE.
extern "C" void **POC_Button_Table();
class GadgetClass
{
public:
	virtual GadgetClass *Unknown_08(); virtual void Unknown_14();
};
#define FORWARD(off) { __asm push ecx __asm call POC_Button_Table __asm pop ecx __asm jmp dword ptr [eax + off] }
__declspec(naked) GadgetClass *GadgetClass::Unknown_08() FORWARD(0x08)
__declspec(naked) void GadgetClass::Unknown_14() FORWARD(0x14)
