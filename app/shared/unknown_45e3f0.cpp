// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	PROVISIONAL file name (D10, interim placement D137): the object after rs_proj_sup.cpp's, client
**	0x45E3F0-0x45E536, has no __FILE__ and no assert; it follows rs_proj_sup.cpp's inline copies
**	(0x45E0D0-0x45E3E0), so it is a separate object linked between rs_proj_sup.cpp and rs_robj.cpp
**	(alphabetically an rs_p...cpp / rs_r...cpp, perhaps a render-state prototype file).
**
**	A named prototype (vtable 0xAE8124): a PrimitivePrototypeClass that keeps its own name and owns
**	an object it deletes with itself. Nothing in the client constructs it (no caller of 0x45E3F0).
*/
#include "proto.h"
#include "..\..\wwlib\nstrdup.h"

// the object the prototype owns: deleted through its first virtual (the deleting destructor)
class Unknown45E3F0OwnedClass	// NAME 1: neutral (D10)
{
public:
	virtual ~Unknown45E3F0OwnedClass();
};

class Unknown45E3F0Class : public PrimitivePrototypeClass	// NAME 1: neutral (D10)
{
public:
	Unknown45E3F0Class(const char *name, RenderObjClass *proto, Unknown45E3F0OwnedClass *owned);	// 0x45E3F0  // NAME 2 [name, proto]: the base's / Get_Name's; NAME 1 [owned]: neutral
	virtual ~Unknown45E3F0Class();									// 0x45E4A0 (deleting 0x45E470)
	virtual const char *Get_Name(void) const;						// 0x45E530

	Unknown45E3F0OwnedClass *Unknown0C;								// +0x0C  // NAME 1: neutral (D10)
	char *Name;														// +0x10  // NAME 2: Get_Name's
};

// @0x45E3F0
Unknown45E3F0Class::Unknown45E3F0Class(const char *name, RenderObjClass *proto, Unknown45E3F0OwnedClass *owned) :
	PrimitivePrototypeClass(proto),
	Unknown0C(owned)
{
	Name = nstrdup(name);
}

// @0x45E4A0
Unknown45E3F0Class::~Unknown45E3F0Class()
{
	delete Unknown0C;
	delete [] Name;
}

// @0x45E530
const char *Unknown45E3F0Class::Get_Name(void) const
{
	return(Name);
}
