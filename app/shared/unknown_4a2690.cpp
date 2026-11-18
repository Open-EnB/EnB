// Earth & Beyond client (reconstruction): app\shared\unknown_4a2690.cpp. Notes at the end of the file.
#include <stddef.h>
#include "..\..\wwlib\listnode.h"

typedef DoubleNode<int, int> GClassRelationNode;	// NAME 1: neutral (D10): 0x28 bytes, the other entry's number and the value

class GClassRelationEntryClass : public GenericNode	// NAME 1: neutral (D10): 0x30 bytes, table 0xAEAB80
{
public:
	GClassRelationEntryClass(int id) { Id = id; }	// (in the body: 0x4A2890 stores it after the table)  // NAME 1 [id]: neutral

	int Id;											// +0x0C  // NAME 1: neutral (D10)
	DataNode<GClassRelationNode *> Sources;			// +0x10 (the relations' Primary nodes)  // NAME 1: neutral (D10)
	DataNode<GClassRelationNode *> Targets;			// +0x20 (the relations' Secondary nodes)  // NAME 1: neutral (D10)
};

class GClassRelationTableClass	// NAME 1: neutral (D10): table 0xAEAB78
{
public:
	GClassRelationTableClass() { NextId = 1; }		// 0x4A26E0 (in the body: the client stores it after the table)
	virtual ~GClassRelationTableClass() {}			// 0x4A27B0

	int Add();										// 0x4A2890 (a new entry; its number)  // NAME 1: neutral (D10)
	void Remove(int id);							// 0x4A2A40 (the entry's relations deleted)  // NAME 1 [id]: neutral; NAME 1: neutral (D10)
	GClassRelationEntryClass *Find(int id);			// 0x4A2B90  // NAME 1: neutral (D10)
	void Set(int from, int to, int value);			// 0x4A2BD0 (0: the relation deleted)  // NAME 1 [from, to, value]: neutral; NAME 1: neutral (D10)
	int Get(int from, int to);						// 0x4A2EF0  // NAME 1 [from, to]: neutral; NAME 1: neutral (D10)

	List<GClassRelationEntryClass *> Entries;		// +0x04  // NAME 1: neutral (D10)
	int NextId;										// +0x20  // NAME 1: neutral (D10)
};

static GClassRelationTableClass _Relations;			// 0xBE3A40  // NAME 1: neutral (D10)
GClassRelationTableClass *Unknown_B72F54 = &_Relations;	// 0xB72F54  // NAME 1: neutral (D10): its address

int GClassRelationTableClass::Add()
{
	GClassRelationEntryClass *entry = new GClassRelationEntryClass(NextId);
	Entries.Add_Head(entry);
	return(NextId++);
}

void GClassRelationTableClass::Remove(int id)
{
	GClassRelationEntryClass *entry = Find(id);
	if (entry != NULL) {
		DataNode<GClassRelationNode *> *node = entry->Sources.Next();
		while (node != NULL) {
			GClassRelationNode *relation = node->Get();
			node = relation->Primary.Next();
			delete relation;
		}
		node = entry->Targets.Next();
		while (node != NULL) {
			GClassRelationNode *relation = node->Get();
			node = relation->Secondary.Next();
			delete relation;
		}
	}
}

GClassRelationEntryClass *GClassRelationTableClass::Find(int id)
{
	if (Entries.First()->Is_Valid()) {
		for (GClassRelationEntryClass *entry = Entries.First(); entry != NULL; entry = (GClassRelationEntryClass *)entry->Next()) {
			if (entry->Id == id) {
				return(entry);
			}
		}
	}
	return(NULL);
}

void GClassRelationTableClass::Set(int from, int to, int value)
{
	GClassRelationEntryClass *source = Find(from);
	if (source == NULL) {
		return;
	}
	GClassRelationEntryClass *target = Find(to);
	if (target == NULL) {
		return;
	}
	GClassRelationNode *relation = NULL;
	for (DataNode<GClassRelationNode *> *node = source->Targets.Next(); node != NULL; node = relation->Secondary.Next()) {
		relation = node->Get();
		if (relation->Get_Primary() == to) {
			if (value == 0) {
				delete relation;
			} else {
				relation->Set_Secondary(value);
			}
			return;
		}
	}
	if (value != 0) {
		relation = new GClassRelationNode(to, value);
		source->Targets.Link(&relation->Secondary);
		target->Sources.Link(&relation->Primary);
	}
}

int GClassRelationTableClass::Get(int from, int to)
{
	GClassRelationEntryClass *source = Find(from);
	if (source != NULL) {
		for (DataNode<GClassRelationNode *> *node = source->Targets.Next(); node != NULL; ) {
			GClassRelationNode *relation = node->Get();
			if (relation->Get_Primary() == to) {
				return(relation->Get_Secondary());
			}
			node = relation->Secondary.Next();
		}
	}
	return(0);
}

/*
**	Notes
**
**	client.exe 0x4A2690-0x4A3000: the relation table (nothing in the client calls it): a list of entries
**	numbered from 1, each with the relations to it (Sources) and from it (Targets), a DoubleNode<int, int>
**	per relation (the other entry's number, the value) in both lists. Its one instance is 0xBE3A40
**	(initialiser 0x4A2690, atexit 0x4A26C0), its pointer 0xB72F54.
**
**	An object of its own, not gclass.cpp's (XI, 2026-11-17): the CRT initialiser table lists gclass.cpp's
**	<string> guard 0x4A2650 (entry 46) before _Relations' initialiser 0x4A2690 (entry 47), and VC6 emits
**	that guarded initialiser as an object's last (all 571 app objects that have one); this object has
**	none (no <string>). GClass's adjustor thunk 0x4A2680 before it is gclass.obj's. No __FILE__ names
**	it: the file name is PROVISIONAL (D137).
*/
