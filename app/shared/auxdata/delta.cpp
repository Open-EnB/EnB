// Earth & Beyond client (reconstruction): app\shared\auxdata\delta.cpp. Notes at the end of the file.
#include "delta.h"
#include "lookup.h"
#include "..\unknown_debug.h"

DeltaClass::DeltaClass(const char *name) :
	Name(name),
	FullName(name),
	m_Unknown70(0),
	Parent(0),
	m_Unknown78(0),
	m_Unknown7C(0),
	m_Unknown80(0x3FF)
{
	m_Unknown04 = DeltaUnknownGlobal;
}


void DeltaClass::Unknown_18()
{
	m_Unknown70 = 2;
	Unknown_40(this);
}


void DeltaClass::Unknown_1C()
{
	m_Unknown70 = 0;
	Unknown_44(this);
}


void DeltaClass::Unknown_20()
{
	if (m_Unknown70 > 1) {
		m_Unknown70 = 1;
	}
}

void DeltaClass::Unknown_00(DeltaChildVisitorClass *)
{
}

DeltaCompoundClass *DeltaClass::As_Compound()
{
	return 0;
}

bool DeltaClass::Unknown_10(LinearBufferClass &buf, const DurationClass &base)
{
	if (!Read(buf, base)) {
		return false;
	}
	Unknown_3C(this);
	return true;
}


bool DeltaClass::Unknown_0C(LinearBufferClass &buf)
{
	Unknown_38(this);
	if (Write(buf)) {
		return true;
	}
	return false;
}


void DeltaClass::Unknown_34(DeltaClass *delta)
{
	m_Unknown70++;
	if (Parent) {
		Parent->Unknown_34(delta);
	}
}


void DeltaClass::Unknown_3C(DeltaClass *delta)
{
	m_Unknown70 = 2;
	if (Parent) {
		Parent->Unknown_3C(delta);
	}
}


void DeltaClass::Unknown_40(DeltaClass *delta)
{
	m_Unknown70 = 2;
	if (Parent) {
		Parent->Unknown_40(delta);
	}
}


void DeltaClass::Unknown_44(DeltaClass *delta)
{
	if (Parent) {
		Parent->Unknown_44(delta);
	}
}


void DeltaClass::Unknown_38(DeltaClass *delta)
{
	if (Parent) {
		Parent->Unknown_38(delta);
	}
}


void DeltaClass::Accept(DeltaVisitorClass &visitor)
{
	visitor.Visit(this);
}


void DeltaClass::Set_Parent(DeltaClass *parent, int slot, int &index)
{
	Parent = parent;
	m_Unknown78 = slot;
	m_Unknown7C = index++;
	if (parent) {
		FullName = parent->FullName;
		FullName.Append(Name);
	}
}


DeltaCompoundClass::~DeltaCompoundClass()
{
	if (Lookup != 0) {
		delete Lookup;
	}
}


void DeltaCompoundClass::Unknown_48()
{
	int index = 0;
	Set_Parent(0, 0, index);
}


void DeltaCompoundClass::Set_Parent(DeltaClass *parent, int slot, int &index)
{
	int start = index;
	DeltaClass::Set_Parent(parent, slot, index);
	int count = Children.size();
	for (int i = 0; i < count; i++) {
		Children[i]->Set_Parent(this, i, index);
	}
	m_Unknown98 = index - start;
}


bool DeltaCompoundClass::Unknown_0C(LinearBufferClass &buf)
{
	if (!DeltaClass::Unknown_0C(buf)) {
		return false;
	}
	int count = Children.size();
	for (int i = 0; i < count; i++) {
		if (!Children[i]->Unknown_0C(buf)) {
			return false;
		}
	}
	return true;
}


bool DeltaCompoundClass::Unknown_10(LinearBufferClass &buf, const DurationClass &base)
{
	if (!DeltaClass::Unknown_10(buf, base)) {
		return false;
	}
	int count = Children.size();
	for (int i = 0; i < count; i++) {
		if (!Children[i]->Unknown_10(buf, base)) {
			return false;
		}
	}
	return true;
}


void DeltaCompoundClass::Unknown_20()
{
	if (m_Unknown70 > 1) {
		int count = Children.size();
		for (int i = 0; i < count; i++) {
			Children[i]->Unknown_20();
		}
	}
	DeltaClass::Unknown_20();
}


void DeltaCompoundClass::Unknown_18()
{
	if (m_Unknown70 == 0) {
		int count = Children.size();
		for (int i = 0; i < count; i++) {
			Children[i]->Unknown_18();
		}
	}
	DeltaClass::Unknown_18();
}


void DeltaCompoundClass::Unknown_1C()
{
	if (m_Unknown70 != 0) {
		int count = Children.size();
		for (int i = 0; i < count; i++) {
			Children[i]->Unknown_1C();
		}
	}
	DeltaClass::Unknown_1C();
}


LookupClass *DeltaCompoundClass::Get_Lookup()
{
	if (Lookup == 0) {
		Lookup = new LookupClass(this);
		G_ASSERT(Lookup);
	}
	return Lookup;
}


void DeltaCompoundClass::Unknown_00(DeltaChildVisitorClass *visitor)
{
	int count = Children.size();
	for (int i = 0; i < count; i++) {
		visitor->Unknown_04(m_Unknown80, Name.Get_String(), Children[i]);
	}
}


void DeltaCompoundClass::Accept(DeltaVisitorClass &visitor)
{
	visitor.Visit(this);
}


void DeltaCompoundClass::Add(DeltaClass *child)
{
	int index = Children.size();
	Children.push_back(child);
}


/*
**	Reconstruction notes
**
**	client.exe 0x4A4D40-0x4A6040 (the object between bitset.cpp and delta_interpolator.cpp; its
**	only __FILE__ use is Get_Lookup's assert, on the client's line 227). The functions are in the
**	client's order; ~DeltaClass (0x4A5150) is inline in delta.h, so VC6 emits it after
**	~DeltaCompoundClass, its first user, as the client has it. 0x4A5880-0x4A6040 are the std::vector
**	and std::map instances these functions need (D11).
**
**	DeltaCompoundClass::Unknown_20 (0x4A53A0) calls DeltaClass::Unknown_20 after the if, not inside
**	it: inside, VC6 saves esi/edi in the branch where the client saves them first.
**
**	DeltaCompoundClass::Add (0x4A5670): the unused `index` is what makes it exact. Its inlined
**	size() is dead code, but it spends VC6's inline budget, so the last size() of the inlined
**	insert is called out of line (0x4A58C0) as in the client; without it VC6 inlines that one too
**	(D40; `Children.capacity();` or a dead `if` on size() do the same, `end()`/`begin()` don't).
*/
