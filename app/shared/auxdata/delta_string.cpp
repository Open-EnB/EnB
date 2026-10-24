// Earth & Beyond client (reconstruction): app\shared\auxdata\delta_string.cpp. Notes at the end of the file.
#include <stdlib.h>
#include <string.h>
#include "delta_string.h"
#include "linearbu.h"
#include "name.h"
#include "..\unknown_debug.h"











DeltaStringClass::DeltaStringClass(const char *name, const char *string) :
	DeltaClass(name),
	String(0)
{
	m_Unknown04 = UnknownGlobal;
	if (string == 0) {
		string = "";
	}
	String = strdup(string);
	G_ASSERT(String);
}

DeltaStringClass::~DeltaStringClass()
{
	free(String);
}




void DeltaStringClass::Assign(const char *s)
{
	if (s == 0) {
		G_ASSERT(s);
		s = "";
	}
	char *copy = strdup(s);




	if (copy == 0) {
		G_ASSERT(copy);
		return;
	}
	free(String);
	String = copy;
	Unknown_34(this);
}




void DeltaStringClass::Set(const char *s)
{
	if (s == 0) {
		G_ASSERT(s);
		s = "";
	}
	if (strcmp(String, s) != 0) {
		Assign(s);
	}
}










bool DeltaStringClass::Write(LinearBufferClass &buf)
{
	G_ASSERT(String);
	const char *s = String;
	if (s == 0) {
		s = "";
	}
	unsigned short length = strlen(s);



	if (buf.Write(&length, sizeof(length)) != sizeof(length)) {
		G_ASSERT(0);
		return false;
	}


	if (buf.Write(s, length) != length) {
		G_ASSERT(0);
		return false;
	}
	return true;
}









bool DeltaStringClass::Read(LinearBufferClass &buf, const DurationClass &)
{
	unsigned short length;
	if (buf.Read(&length, sizeof(length)) != sizeof(length)) {
		G_ASSERT(0);
		return false;
	}
	char *tmp = (char *)malloc(length + 1);

	if (tmp == 0) {
		G_ASSERT(tmp);
		return false;
	}


	if (buf.Read(tmp, length) != length) {
		G_ASSERT(0);
		free(tmp);
		return false;
	}
	tmp[length] = 0;
	if (String != 0) {
		free(String);
	}
	String = tmp;
	return true;
}

DeltaStringClass &DeltaStringClass::operator=(const DeltaStringClass &that)
{
	Assign(that.String);
	return *this;
}

// The value Unknown_00 hands the encoder: vtable 0xAEB13C
class DeltaStringValueClass : public StringValueClass
{
public:
	DeltaStringValueClass(DeltaStringClass *owner) : Owner(owner) {}
	virtual const char *Get() const;
	virtual void Set(const char *value);
	virtual StringValueClass *Clone() const;

	DeltaStringClass *Owner;		// +0x04
};

void DeltaStringClass::Unknown_00(DeltaChildVisitorClass *visitor)
{
	visitor->Add_String(m_Unknown80, Name.Get_String(), new DeltaStringValueClass(this));
}

const char *DeltaStringValueClass::Get() const
{
	return Owner->Get();
}

void DeltaStringValueClass::Set(const char *value)
{
	Owner->Set(value);
}

StringValueClass *DeltaStringValueClass::Clone() const
{
	return new DeltaStringValueClass(Owner);
}

/*
**	Reconstruction notes
**
**	client.exe 0x4A78F0-0x4A80A0: DeltaStringClass (D24; the asserts name String), vtable 0xAEB0E4,
**	size 0x88, its per-class global 0xBE3A9C. Rebuilt: the constructor 0x4A78F0 (line 28), the
**	destructor 0x4A79E0, Assign 0x4A7A10 (lines 42 and 51), Set 0x4A7AA0 (line 65, Assign inlined),
**	Write 0x4A7BB0 (lines 84, 94, 100: a 2-byte length, then the characters), Read 0x4A7CA0 (lines
**	118, 124, 130), operator= 0x4A7DC0 (Assign inlined), Unknown_00 0x4A7E60 (kv_encoder_base.cpp's
**	0x4B48A0 with a DeltaStringValueClass), and DeltaStringValueClass (vtable 0xAEB13C): Get
**	0x4A7F30, Set 0x4A7F50 (DeltaStringClass::Set inlined), Clone 0x4A8060 (`new ...(Owner)`: the implicit
**	copy constructor would store the base's vtable too); StringValueClass's
**	scalar deleting destructor 0x4A7F00 and DeltaStringValueClass's 0x4A7EB0/0x4A7EE0 are the
**	compiler's. The failure paths are `if (!copy) { G_ASSERT(copy); return; }` (one test, the
**	assert's expression the pointer). Not rebuilt: this object's initialisers 0x4A77A0 and those
**	after 0x4A8060 (atexit, a permstr.cpp object, 0x4AE1E0).
*/
