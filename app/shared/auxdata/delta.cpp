// Earth & Beyond client (reconstruction): app\shared\auxdata\delta.cpp. Notes at the end of the file.
#include "..\kv_encoder.h"	// delta.h and KVEncoderClass
#include "lookup.h"
#include "..\g_assert.h"
int DeltaUnknownGlobal = DeltaUnknownCounter++; int DeltaContainer::UnknownGlobal = DeltaUnknownCounter++;
DeltaClass::DeltaClass(const char *name) :
	Name(name),
	FullName(name),
	Unknown70(0),
	Parent(0),
	Unknown78(0),
	Unknown7C(0),
	Unknown80(0x3FF)
{
	Unknown04 = DeltaUnknownGlobal;
}


void DeltaClass::Unknown_18()
{
	Unknown70 = 2;
	Unknown_40(this);
}


void DeltaClass::Unknown_1C()
{
	Unknown70 = 0;
	Unknown_44(this);
}


void DeltaClass::Unknown_20()
{
	if (Unknown70 > 1) {
		Unknown70 = 1;
	}
}

void DeltaClass::Unknown_00(KVEncoderClass *)
{
}

DeltaContainer *DeltaClass::As_Compound()
{
	return 0;
}

bool DeltaClass::Unknown_10(LinearBuffer &buf, const DurationClass &base)
{
	if (!Read(buf, base)) {
		return false;
	}
	Unknown_3C(this);
	return true;
}


bool DeltaClass::Unknown_0C(LinearBuffer &buf)
{
	Unknown_38(this);
	if (Write(buf)) {
		return true;
	}
	return false;
}


void DeltaClass::Unknown_34(DeltaClass *delta)
{
	Unknown70++;
	if (Parent) {
		Parent->Unknown_34(delta);
	}
}


void DeltaClass::Unknown_3C(DeltaClass *delta)
{
	Unknown70 = 2;
	if (Parent) {
		Parent->Unknown_3C(delta);
	}
}


void DeltaClass::Unknown_40(DeltaClass *delta)
{
	Unknown70 = 2;
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
	Unknown78 = slot;
	Unknown7C = index++;
	if (parent) {
		FullName = parent->FullName;
		FullName.Append(Name);
	}
}


DeltaContainer::~DeltaContainer()
{
	if (Lookup != 0) {
		delete Lookup;
	}
}


void DeltaContainer::Unknown_48()
{
	int index = 0;
	Set_Parent(0, 0, index);
}


void DeltaContainer::Set_Parent(DeltaClass *parent, int slot, int &index)
{
	int start = index;
	DeltaClass::Set_Parent(parent, slot, index);
	int count = Children.size();
	for (int i = 0; i < count; i++) {
		Children[i]->Set_Parent(this, i, index);
	}
	Unknown98 = index - start;
}


bool DeltaContainer::Unknown_0C(LinearBuffer &buf)
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


bool DeltaContainer::Unknown_10(LinearBuffer &buf, const DurationClass &base)
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


void DeltaContainer::Unknown_20()
{
	if (Unknown70 > 1) {
		int count = Children.size();
		for (int i = 0; i < count; i++) {
			Children[i]->Unknown_20();
		}
	}
	DeltaClass::Unknown_20();
}


void DeltaContainer::Unknown_18()
{
	if (Unknown70 == 0) {
		int count = Children.size();
		for (int i = 0; i < count; i++) {
			Children[i]->Unknown_18();
		}
	}
	DeltaClass::Unknown_18();
}


void DeltaContainer::Unknown_1C()
{
	if (Unknown70 != 0) {
		int count = Children.size();
		for (int i = 0; i < count; i++) {
			Children[i]->Unknown_1C();
		}
	}
	DeltaClass::Unknown_1C();
}


LookupClass *DeltaContainer::Get_Lookup()
{
	if (Lookup == 0) {
		Lookup = new LookupClass(this);
		G_ASSERT(Lookup);
	}
	return Lookup;
}


void DeltaContainer::Unknown_00(KVEncoderClass *visitor)
{
	int count = Children.size();
	for (int i = 0; i < count; i++) {
		visitor->Add_Child(Unknown80, Name.Get_String(), Children[i]);
	}
}


void DeltaContainer::Accept(DeltaVisitorClass &visitor)
{
	visitor.Visit(this);
}


void DeltaContainer::Add(DeltaClass *child)
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
**	~DeltaContainer, its first user, as the client has it. 0x4A5880-0x4A6040 are the std::vector
**	and std::map instances these functions need (D11).
**
**	DeltaContainer::Unknown_20 (0x4A53A0) calls DeltaClass::Unknown_20 after the if, not inside
**	it: inside, VC6 saves esi/edi in the branch where the client saves them first.
**
**	DeltaContainer::Add (0x4A5670): the unused `index` is what makes it exact. Its inlined
**	size() is dead code, but it spends VC6's inline budget, so the last size() of the inlined
**	insert is called out of line (0x4A58C0) as in the client; without it VC6 inlines that one too
**	(D40; `Children.capacity();` or a dead `if` on size() do the same, `end()`/`begin()` don't).
**
**	The per-class globals are numbered at start-up from the counter DeltaUnknownCounter (0xBE3B44): the
**	client's CRT table lists this object's initialisers 0x4A4CE0 (DeltaUnknownGlobal 0xBE3A78) and 0x4A4D10
**	(DeltaContainer::UnknownGlobal 0xBE3A74), `mov eax,[0xBE3B44]; mov [g],eax; inc eax; mov [0xBE3B44],eax`,
**	after the header's _PermStringInit pair and before the constructor 0x4A4D40. delta_money.cpp (0x4A7050),
**	delta_string.cpp (0x4A78C0) and delta_time.cpp (0x4A8120) number theirs the same way. The template
**	classes' globals (DeltaTemplateClass<T>, DeltaArrayClass<T>, DeltaItemArrayClass<T>) are numbered the same
**	way in the client, one initialiser per object at its end (0x529580 numbers six), but here every
**	aux_roots.h includer would number all thirty of them: they stay zero-initialised until the headers are
**	split as the client's objects see them (XI, 2026-11-17).
*/
