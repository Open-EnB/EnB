// Earth & Beyond client (reconstruction): app\client\unknown_7dfed0.h. PROVISIONAL file name (D137): the registration
// screen's XML reader over MSXML's DOMDocument (the object from 0x7DFED0 has no __FILE__ or assert) (XR2, 2026-11-09).
#ifndef UNKNOWN_7DFED0_H  // NAME 1 [unknown_7dfed0.h]: PROVISIONAL (D137)
#define UNKNOWN_7DFED0_H

#include "msxml3.tlh"
#include "unknown_7cef30.h"
#include "unknown_7e1e40.h"

// The implementation at +0x14C (0x60 bytes; tables 0xAF70DC / 0xAF70C8, destructor 0x5F0590, deleting 0x5F0630).
// Slots 0x7E0070-0x7E15B0, 0x7DFED0-0x7E0050 live in unknown_7dfed0.cpp.
class Sub14CImplClass : public Sub14CClass, public RegistrationCallbackClass  // NAME 1 [Sub14CImplClass]: neutral (D10)
{
public:
	Sub14CImplClass() : Unknown08(0), Unknown14(0) { Slot1(); }
	virtual ~Sub14CImplClass() { Slot2(); }
	virtual void Slot1();	// 0x7E0070
	virtual void Slot2();	// 0x7E0180
	virtual std::string Slot3(const char *path, int index, const char *name);	// 0x7E01B0
	virtual int Slot4(const char *path, int index, const char *name);	// 0x7E0960
	virtual float Slot5(const char *path, int index, const char *name);	// 0x7E09F0
	virtual int Slot6(const char *path);	// 0x7E0A80
	virtual std::string Slot7(const char *path, const char *name, int index, const char *element);	// 0x7E0BC0
	virtual int Slot8(const char *path, const char *name, int index, const char *element);	// 0x7E1200
	virtual int Slot9();	// 0x7E1940
	virtual int Slot11();	// 0x7E1C60
	virtual const char *Slot12(int arg);	// 0x7E1C80
	virtual bool Slot13(SubsRequestClass *request, int callback);	// 0x7E1290
	virtual void Slot14();	// 0x7E1A20
	virtual std::string Slot15();	// 0x7E1310
	virtual std::string Slot16();	// 0x7E15B0
	virtual void Unknown_5F00C0();	// 0x7DFED0
	virtual void Unknown_5F00E0();	// 0x7E0030
	virtual void Unknown_5F0100();	// 0x7E0050

	RegistrationCallbackClass *Unknown08;	// +0x08: slot 0x34's callback (the reply's notifications go on to it)
	HRESULT Unknown0C;						// +0x0C: CreateInstance's result
	MSXML2::IXMLDOMDocumentPtr Unknown10;	// +0x10: the reply document (DOMDocument)
	int Unknown14;							// +0x14
	Sub14CUnknown18Class Unknown18;			// +0x18
};

#endif // UNKNOWN_7DFED0_H
