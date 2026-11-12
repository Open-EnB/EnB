// Earth & Beyond client (reconstruction): the forwards that differ from poc_gadget_views.cpp only in the return type (D179). NOT THE CLIENT'S CODE.
extern "C" void **POC_Button_Table();
class GadgetClass
{
public:
	virtual GadgetClass *As_RObjGadget(); virtual void As_UIScutProxyGadget();
};
#define FORWARD(off) { __asm push ecx __asm call POC_Button_Table __asm pop ecx __asm jmp dword ptr [eax + off] }
__declspec(naked) GadgetClass *GadgetClass::As_RObjGadget() FORWARD(0x08)
__declspec(naked) void GadgetClass::As_UIScutProxyGadget() FORWARD(0x14)
