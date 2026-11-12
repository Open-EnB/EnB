// Earth & Beyond client (reconstruction): the POC's forwards of ui_scut.cpp's and ui_manu.cpp's GadgetClass view (D179; poc_gadget_real.cpp). NOT THE CLIENT'S CODE.
extern "C" void **POC_Button_Table();

struct KernelEventClass;
class RenderObjClass;
class UIScutBarClass;

class GadgetClass
{
public:
	virtual void Unknown_00(); virtual void As_RObjGadget(); virtual GadgetClass *As_UIScutProxyGadget();
	virtual void As_RObjProxyGadget(); virtual void As_SlotDataGadget(); virtual void Unknown_18(); virtual void Unknown_1C();
	virtual void Unknown_20(); virtual void Unknown_24(); virtual void Unknown_28();
	virtual void Unknown_2C(KernelEventClass *event, float x, float y, unsigned short key);
	virtual void Unknown_30(); virtual void Unknown_34(); virtual void Unknown_38(); virtual void Unknown_3C();
	virtual void Unknown_44(); virtual void Unknown_48(); virtual void Unknown_4C();
	virtual void Unknown_54(); virtual void Unknown_58(); virtual void Unknown_5C(); virtual void Unknown_60();
	virtual void Unknown_64(); virtual void Unknown_68(); virtual void Unknown_74(); virtual void Unknown_78();
	virtual void Unknown_7C(UIScutBarClass *bar); virtual void Unknown_80(UIScutBarClass *bar);
	virtual void Unknown_84(); virtual void Unknown_88(); virtual void Unknown_8C(); virtual void Unknown_90();
	virtual void Unknown_94(RenderObjClass *robj); virtual void Unknown_98(); virtual void Unknown_9C();
	virtual void Unknown_A0(); virtual void Unknown_A4(GadgetClass *gadget);
};

#define FORWARD(off) { __asm push ecx __asm call POC_Button_Table __asm pop ecx __asm jmp dword ptr [eax + off] }

__declspec(naked) void GadgetClass::Unknown_00() FORWARD(0x00)
__declspec(naked) void GadgetClass::As_RObjGadget() FORWARD(0x08)
__declspec(naked) GadgetClass *GadgetClass::As_UIScutProxyGadget() FORWARD(0x14)
__declspec(naked) void GadgetClass::As_RObjProxyGadget() FORWARD(0x0C)
__declspec(naked) void GadgetClass::As_SlotDataGadget() FORWARD(0x10)
__declspec(naked) void GadgetClass::Unknown_18() FORWARD(0x18)
__declspec(naked) void GadgetClass::Unknown_1C() FORWARD(0x1C)
__declspec(naked) void GadgetClass::Unknown_20() FORWARD(0x20)
__declspec(naked) void GadgetClass::Unknown_24() FORWARD(0x24)
__declspec(naked) void GadgetClass::Unknown_28() FORWARD(0x28)
__declspec(naked) void GadgetClass::Unknown_2C(KernelEventClass *event, float x, float y, unsigned short key) FORWARD(0x2C)
__declspec(naked) void GadgetClass::Unknown_30() FORWARD(0x30)
__declspec(naked) void GadgetClass::Unknown_34() FORWARD(0x34)
__declspec(naked) void GadgetClass::Unknown_38() FORWARD(0x38)
__declspec(naked) void GadgetClass::Unknown_3C() FORWARD(0x3C)
__declspec(naked) void GadgetClass::Unknown_44() FORWARD(0x44)
__declspec(naked) void GadgetClass::Unknown_48() FORWARD(0x48)
__declspec(naked) void GadgetClass::Unknown_4C() FORWARD(0x4C)
__declspec(naked) void GadgetClass::Unknown_54() FORWARD(0x54)
__declspec(naked) void GadgetClass::Unknown_58() FORWARD(0x58)
__declspec(naked) void GadgetClass::Unknown_5C() FORWARD(0x5C)
__declspec(naked) void GadgetClass::Unknown_60() FORWARD(0x60)
__declspec(naked) void GadgetClass::Unknown_64() FORWARD(0x64)
__declspec(naked) void GadgetClass::Unknown_68() FORWARD(0x68)
__declspec(naked) void GadgetClass::Unknown_74() FORWARD(0x74)
__declspec(naked) void GadgetClass::Unknown_78() FORWARD(0x78)
__declspec(naked) void GadgetClass::Unknown_7C(UIScutBarClass *bar) FORWARD(0x7C)
__declspec(naked) void GadgetClass::Unknown_80(UIScutBarClass *bar) FORWARD(0x80)
__declspec(naked) void GadgetClass::Unknown_84() FORWARD(0x84)
__declspec(naked) void GadgetClass::Unknown_88() FORWARD(0x88)
__declspec(naked) void GadgetClass::Unknown_8C() FORWARD(0x8C)
__declspec(naked) void GadgetClass::Unknown_90() FORWARD(0x90)
__declspec(naked) void GadgetClass::Unknown_94(RenderObjClass *robj) FORWARD(0x94)
__declspec(naked) void GadgetClass::Unknown_98() FORWARD(0x98)
__declspec(naked) void GadgetClass::Unknown_9C() FORWARD(0x9C)
__declspec(naked) void GadgetClass::Unknown_A0() FORWARD(0xA0)
__declspec(naked) void GadgetClass::Unknown_A4(GadgetClass *gadget) FORWARD(0xA4)
