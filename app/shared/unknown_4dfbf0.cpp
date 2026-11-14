// Earth & Beyond client (reconstruction): app\shared\unknown_4dfbf0.cpp. Notes at the end of the file.
#include "unknown_4dfbf0.h"

AttribLinkModClass *Link_Attribute(AttribDir *src, const std::string &src_name, AttribDir *dst, const std::string &dst_name, int type, void *owner)
{
	AttribLinkModClass *mod = new AttribLinkModClass(src, src_name, dst, dst_name, type, owner);
	dst->Attach_Mod(dst_name.c_str(), mod);
	src->Attach_Callback(src_name.c_str(), mod);
	return mod;
}

AttribLinkModClass::AttribLinkModClass(AttribDir *src, const std::string &src_name, AttribDir *dst, const std::string &dst_name, int type, void *owner) :
	Shadow(src->Get_Shadow(src_name.c_str())),
	Src(src),
	SrcName(src_name),
	Dst(dst),
	DstName(dst_name),
	Type(type),
	Owner(owner)
{
}

AttribLinkModClass::~AttribLinkModClass()
{
	Src->Detach_Callback(SrcName.c_str(), this);
}

int AttribLinkModClass::Get_Type()
{
	return Type;
}

void *AttribLinkModClass::Get_Owner()
{
	return Owner;
}

float AttribLinkModClass::Get_Value()
{
	return Shadow.Get_Value();
}

void AttribLinkModClass::Value_Changed(const char *name, float value)
{
	Dst->Recalculate(DstName.c_str());
}

AttribBinaryModClass::AttribBinaryModClass(int type, void *owner, AttribDir *dir, const std::string &name, const std::string &a, const std::string &b) :
	Type(type),
	Owner(owner),
	Dir(dir),
	Name(name),
	A(a),
	B(b),
	ShadowA(dir->Get_Shadow(a.c_str())),
	ShadowB(dir->Get_Shadow(b.c_str()))
{
	Dir->Attach_Update(A.c_str(), Name.c_str());
	Dir->Attach_Update(B.c_str(), Name.c_str());
}

int AttribBinaryModClass::Get_Type()
{
	return Type;
}

void *AttribBinaryModClass::Get_Owner()
{
	return Owner;
}

AttribBinaryModClass::~AttribBinaryModClass()
{
	Dir->Detach_Update(A.c_str(), Name.c_str());
	Dir->Detach_Update(B.c_str(), Name.c_str());
}

AttribSumModClass *Create_Sum_Mod(int type, void *owner, AttribDir *dir, const std::string &name, const std::string &a, const std::string &b)
{
	return new AttribSumModClass(type, owner, dir, name, a, b);
}

AttribSumModClass::AttribSumModClass(int type, void *owner, AttribDir *dir, const std::string &name, const std::string &a, const std::string &b) :
	AttribBinaryModClass(type, owner, dir, name, a, b)
{
}

AttribSumModClass::~AttribSumModClass()
{
}

float AttribSumModClass::Get_Value()
{
	return ShadowA.Get_Value() + ShadowB.Get_Value();
}


/*
**	Reconstruction notes
**
**	client.exe 0x4DFBF0-0x4E06EF, one object: it follows attrib_dir_collective.cpp's and ends with
**	<string>'s guarded ctype initialiser 0x4E06D0; its vtables 0xAEC3D0-0xAEC410 sit between the two
**	objects' __stl_prime_list copies (0xAEC348, 0xAEC414). No __FILE__, assert or RTTI: the file name
**	is provisional (D137). Link_Attribute (AttribDirCollective::Link's calls, bound types 4 / 5) makes
**	a link mod on dst's attribute whose value is src's (through a shadow) and registers it as src's
**	callback; the two-source mod attaches Name as an update of A and B in its directory, and the sum
**	(0xAEC400) is its one subclass in the client. Create_Sum_Mod 0x4E0430 and the sum's constructor
**	0x4E04C0 have no caller in client.exe. Every function exact (appfile.py match); the sum's
**	Get_Value 0x4E06A0 is PREFIX of a record that runs into the initialiser.
*/
