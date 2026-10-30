// Earth & Beyond client (reconstruction): the aux-data change table's object, PROVISIONAL file name
// (M1.3). Notes at the end of the file.
// NAME 1 [unknown_changes.cpp]: provisional file name: the object 0x4AB580-0x4AC250 has no assert and no __FILE__
#include "unknown_changes.h"
#include "delta.h"

// ------------------------------------------------------------------------- the visitors

// Set_Sent(DeltaClass *)'s (vtable 0xAEB2F4): every field below as sent now
class ChangeSentVisitorClass : public DeltaVisitorClass  // NAME 2: describes what it does
{
public:
	ChangeSentVisitorClass(ChangeTableClass *table) : Table(table) {}
	virtual void Visit(DeltaCompoundClass *delta);			// 0x4AB7D0
	virtual void Visit(DeltaClass *delta);						// 0x4AB780

	ChangeTableClass *Table;			// +0x04  // NAME 2: describes what it does
};

// the constructor's (vtable 0xAEB300): links the entries as the tree is and counts the leaves
class ChangeBuildVisitorClass : public DeltaVisitorClass  // NAME 2: describes what it does
{
public:
	ChangeBuildVisitorClass(ChangeTableClass *table) : Parent(0), Link(&Head), Count(0), Head(0), Table(table) {}
	virtual void Visit(DeltaCompoundClass *delta);			// 0x4ABA70
	virtual void Visit(DeltaClass *delta);						// 0x4ABA30

	ChangeEntryClass *Parent;			// +0x04  // NAME 2: describes what it does
	ChangeEntryClass **Link;			// +0x08 where the next entry is linked  // NAME 2: describes what it does
	unsigned int Count;					// +0x0C the leaves so far  // NAME 2: describes what it does
	ChangeEntryClass *Head;				// +0x10 the root's entry  // NAME 2: describes what it does
	ChangeTableClass *Table;			// +0x14  // NAME 2: describes what it does
};

// Scan's (vtable 0xAEB30C): walks the entries along with the tree
class ChangeScanVisitorClass : public DeltaVisitorClass  // NAME 2: describes what it does
{
public:
	ChangeScanVisitorClass(ChangeEntryClass *entry) : Entry(entry) {}
	virtual void Visit(DeltaCompoundClass *delta);			// 0x4ABB80
	virtual void Visit(DeltaClass *delta);						// 0x4ABCA0

	ChangeEntryClass *Entry;			// +0x04 the visited field's  // NAME 2: describes what it does
};

// ------------------------------------------------------------------------- the table

void ChangeTableClass::Unsent_Parents(ChangeEntryClass *entry)
{
	for (ChangeEntryClass *parent = entry->Parent; parent != 0; parent = parent->Parent) {
		if (entry->Changed) {
			parent->Changed -= entry->Changed;
			if (parent->Changed) {
				parent->Dirty = true;
			} else {
				parent->Dirty = false;
			}
		}
		if (!entry->Sent) {
			parent->Sent += entry->Size;
		}
	}
}

void ChangeTableClass::Set_Sent(int id, unsigned int stamp)
{
	ChangeEntryClass *entry = &Entries[id];
	Unsent_Parents(entry);
	entry->Changed = 0;
	entry->Sent = entry->Size;
	entry->First = Time;
	entry->Last = Time;
	entry->Stamp = stamp;
}

void ChangeTableClass::Set_Sent(DeltaClass *delta)
{
	Unsent_Parents(&Entries[delta->Unknown7C]);
	ChangeSentVisitorClass visitor(this);
	delta->Accept(visitor);
}

void ChangeSentVisitorClass::Visit(DeltaClass *delta)
{
	Table->Set_Entry_Sent(delta->Unknown7C, delta->Unknown70);
}

void ChangeSentVisitorClass::Visit(DeltaCompoundClass *delta)
{
	Table->Set_Entry_Sent(delta->Unknown7C, delta->Unknown70)->Dirty = false;
	Visit_Children(delta);
}

inline ChangeEntryClass *ChangeTableClass::Set_Entry_Sent(int id, unsigned int stamp)
{
	ChangeEntryClass *entry = &Entries[id];
	entry->Changed = 0;
	entry->Sent = entry->Size;
	entry->First = Time;
	entry->Last = Time;
	entry->Stamp = stamp;
	return entry;
}

void ChangeTableClass::Set_Unsent(int id)
{
	ChangeEntryClass *entry = &Entries[id];
	unsigned int sent = entry->Sent;
	Set_Unsent(entry);
	for (ChangeEntryClass *parent = entry->Parent; parent != 0; parent = parent->Parent) {
		parent->Sent -= sent;
	}
}

void ChangeTableClass::Set_Unsent(ChangeEntryClass *entry)
{
	for (ChangeEntryClass *child = entry->Child; child != 0; child = child->Next) {
		if (child->Sent) {
			Set_Unsent(child);
		}
	}
	entry->Sent = 0;
}

ChangeTableClass::ChangeTableClass(DeltaCompoundClass *root) : Root(0), Time(0), Delta(root)
{
	Entries.resize(root->Unknown98, ChangeEntryClass());
	ChangeBuildVisitorClass visitor(this);
	root->Accept(visitor);
	Root = visitor.Head;
}

void ChangeBuildVisitorClass::Visit(DeltaClass *delta)
{
	ChangeEntryClass *entry = &Table->Entries[delta->Unknown7C];
	entry->Parent = Parent;
	*Link = entry;
	Link = &entry->Next;
	Count++;
}

void ChangeBuildVisitorClass::Visit(DeltaCompoundClass *delta)
{
	ChangeEntryClass *entry = &Table->Entries[delta->Unknown7C];
	entry->Parent = Parent;
	*Link = entry;
	Link = &entry->Next;
	entry->Is_Leaf = false;
	ChangeEntryClass *parent = Parent;
	ChangeEntryClass **link = Link;
	unsigned int count = Count;
	Parent = entry;
	Link = &entry->Child;
	Visit_Children(delta);
	entry->Size = Count - count;
	Parent = parent;
	Link = link;
}

ChangeTableClass::~ChangeTableClass()
{
}

void ChangeTableClass::Scan()
{
	ChangeScanVisitorClass visitor(Root);
	Delta->Accept(visitor);
}

void ChangeScanVisitorClass::Visit(DeltaCompoundClass *delta)
{
	ChangeEntryClass *entry = Entry;
	if (entry->Stamp != delta->Unknown70) {
		Entry = entry->Child;
		int count = delta->Children.size();
		for (int i = 0; i < count; i++) {
			if (Entry->Included) {
				delta->Children[i]->Accept(*this);
				entry->Dirty = true;
			}
			Entry = Entry->Next;
		}
		Entry = entry;
		entry->Stamp = delta->Unknown70;
	}
	ChangeEntryClass *current = Entry;
	if (current->Dirty) {
		unsigned int changed = 0;
		unsigned int first = 0xFFFFFFFF;
		unsigned int last = 0;
		for (ChangeEntryClass *child = current->Child; child != 0; child = child->Next) {
			if (child->Included && child->Changed) {
				changed += child->Changed;
				if (child->First < first) {
					first = child->First;
				}
				if (child->Last > last) {
					last = child->Last;
				}
			}
		}
		current->Changed = changed;
		Entry->First = first;
		Entry->Last = last;
		Entry->Dirty = false;
	}
}

void ChangeScanVisitorClass::Visit(DeltaClass *delta)
{
	if (Entry->Stamp != delta->Unknown70) {
		Entry->Changed = 1;
	}
}

void ChangeTableClass::Exclude(int id)
{
	ChangeEntryClass *entry = &Entries[id];
	if (entry->Included) {
		for (ChangeEntryClass *parent = entry->Parent; parent != 0; parent = parent->Parent) {
			parent->Changed -= entry->Changed;
			if (parent->Changed) {
				parent->Dirty = true;
			} else {
				parent->Dirty = false;
			}
		}
		entry->Included = false;
	}
}

void ChangeTableClass::Include_All()
{
	Include(Root);
}

void ChangeTableClass::Include(ChangeEntryClass *entry)
{
	if (entry == 0) {
		return;
	}
	if (!entry->Included) {
		entry->Included = true;
		for (ChangeEntryClass *parent = entry->Parent; parent != 0; parent = parent->Parent) {
			parent->Stamp = 0;
			parent->Dirty = true;
		}
	} else {
		Include(entry->Child);
	}
	Include(entry->Next);
}

/*
**	Reconstruction notes
**
**	client.exe 0x4AB580-0x4AC250 (no assert, no __FILE__: the file name is provisional, as
**	unknown_root.cpp; the user decides the name). The order of the functions is the client's. The
**	entries are kept by field id in a vector (its insert 0x4ABE40 is the STL instance, its
**	destructor's copy 0x4ABE00); the three visitors are local classes with the two Visit slots only.
**	How a field counts as changed: Scan compares each field's Unknown70 with the entry's Stamp
**	(DeltaClass::Unknown_34 counts it up on every change; a compound's moves with any field below,
**	so Scan skips a compound whose Stamp is current), and sums Changed, First and Last up the
**	compounds marked Dirty. Not rebuilt: 0x4AB580 (returns +0x0C less +0x08 of its object; no
**	caller found) and the file's PermStringInitClass static 0x4AB5A0 (permstr.h's, compiled with
**	the include).
*/
