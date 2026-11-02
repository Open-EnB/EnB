// Earth & Beyond client (reconstruction): app\shared\gclass.cpp. Notes at the end of the file.
#include <string.h>
#include "gclass.h"
#include "..\..\wwlib\vector.h"
#include "unknown_debug.h"

// The list +0x10 puts the first base's nodes in (0x4A17B0 walks it) and the dirty list +0x18 fills (0x4A20A0
// empties it). The client's destroy them through guarded atexit functions (0x4A1610, 0x4A1740: template statics?)
static List<DataNode<UnknownGadgetBase00Class *> *> _Nodes;	// 0xBE39F8 (table 0xAEAB58)  // NAME 1: neutral (D10)
static DynamicVectorClass<GClass *> _DirtyList;				// 0xBE3A18 (table 0xAEAB5C)  // NAME 1: neutral (D10)

// NOT EXACT: order-only (0.65; the client saves esi before the first test, VC6 after the Is_Valid tests; fifteen forms, attempts.py show 0x4A17B0)
void GClass::Unknown_4A17B0()
{
	if (_Nodes.First()->Is_Valid()) {
		DataNode<UnknownGadgetBase00Class *> *node = _Nodes.First();
		while (node != NULL) {
			DataNode<UnknownGadgetBase00Class *> *next = node->Next();
			if (next != NULL) {
				node->Get()->Unknown_08();
			}
			node = next;
		}
	}
}

void GClass::Unknown_10()
{
	GenericNode *node = Unknown_04();
	if (node != NULL) {
		_Nodes.Add_Head(node);
	}
}

GClass::GClass() :
	Flags(0),
	Unknown0C(0)
{
}

GClass::GClass(const GClass &that) :
	Flags(that.Flags),
	Unknown0C(that.Unknown0C)
{
}

GClass::~GClass()
{
}

void GClass::Unknown_34()
{
	Unknown_38(Get_Target_Node());
}

bool GClass::Unknown_30(GClass *target, GClassTargetNode *node)
{
	if (node == NULL) {
		node = Get_Target_Node();
	}
	bool is_target_node = (node == Get_Target_Node());
	if (target != NULL) {
		GClass *old_target = node->Get();
		node->Unlink();
		node->Set(NULL);
		target->Unknown_44(node);
		node->Set(target);
		if (is_target_node && old_target != target) {
			Signals.emit(this, TargetChangeEvent(this, old_target, target));
		}
	} else {
		Unknown_38(node);
	}
	return(true);
}

bool GClass::Unknown_14(int unknown1, int unknown2, char *buffer, int size)
{
	if (Unknown_0C() == NULL) {
		return(false);
	}
	strncpy(buffer, Unknown_0C(), size);
	bool result = (unknown2 < 1);
	return(result);
}

void GClass::Unknown_3C(GClass *target)
{
	if (target != NULL && Get_Target_Node() != NULL && target == Get_Target_Node()->Get()) {
		Unknown_38(Get_Target_Node());
	}
}

void GClass::Unknown_38(GClassTargetNode *node)
{
	GClass *old_target = node->Get();
	node->Unlink();
	node->Set(NULL);
	if (node == Get_Target_Node() && old_target != NULL) {
		Signals.emit(this, TargetChangeEvent(this, old_target, NULL));
	}
}

void GClass::Unknown_40()
{
	GClassTargetingList *list = Get_Targeting_List();
	if (list != NULL) {
		GClassTargetNode *node = list->First_Valid();
		while (node != NULL) {
			GClassTargetNode *next = (GClassTargetNode *)node->Next_Valid();
			if (node->Get_Context() != NULL) {
				node->Get_Context()->Unknown_38(node);
			}
			node = next;
		}
	}
}


























































































































































































/*
**	Reconstruction notes (app\shared\gclass.cpp).
**
**	The client's object, 0x4A15E0-0x4A3000 (strconv_lib.cpp's value conversions follow at 0x4A30A0):
**	the dynamic initialisers of the two lists above (0x4A15E0 / 0x4A1610 with List's destructor 0x4A1640,
**	0x4A1710 / 0x4A1740 with DynamicVectorClass's 0x4A1770; the client guards the atexit functions with a
**	bit each, 0xBE3A31 / 0xBE3A30), then 0x4A17B0, GClass's +0x10 0x4A1800, the constructor 0x4A1880 with
**	the bases' inline slots (0x4A1910-0x4A19E0), GClass's inline slots 0x4A1A10-0x4A1AF0, the deleting
**	destructor 0x4A1B10, the copy constructor 0x4A1B40 (unreferenced), the destructor 0x4A1BD0 and the
**	slots 0x4A1C60-0x4A2010 in this file's order, 0x4A20A0 (unreferenced), the template instances
**	0x4A20F0-0x4A2650 (List's constructor and deleting destructor, VectorClass / DynamicVectorClass<GClass *>'s
**	slots) and the +0x04 adjustor thunk 0x4A2680; the relation table at the end of the file.
**
**	The asserts' lines (325 'Get_Targeting_List() != NULL', 344 'Get_Target_Node()') place Unknown_44 and
**	Unknown_48; the blank lines above keep them there (D25).
**
**	Names: GClass and TargetChangeEvent are the client's RTTI (SignalMap::emit's type_info arguments in
**	Unknown_30 / Unknown_38); Get_Target_Node and Get_Targeting_List the asserts'. The rest is neutral (D10).
*/
void GClass::Unknown_44(GClassTargetNode *node)
{
	G_ASSERT(Get_Targeting_List() != NULL);
	node->Set(this);
	Get_Targeting_List()->Add_Head(node);
}













GClass *GClass::Unknown_48()
{
	G_ASSERT(Get_Target_Node());
	return(Get_Target_Node()->Get());
}

void GClass::Unknown_18()
{
	if (Flags & 1) {
		return;
	}
	Flags |= 1;
	DynamicVectorClass<GClass *> &list = _DirtyList;	// (a reference: the client computes Resize's size in ecx)
	list.Add(this);
}

void GClass::Unknown_4A20A0()
{
	_DirtyList.Clear();
}


// The relation table (0x4A2690-0x4A3000; nothing in the client calls it): a list of entries numbered from 1,
// each with the relations to it (Sources) and from it (Targets), a DoubleNode<int, int> per relation
// (the other entry's number, the value) in both lists. Its one instance is 0xBE3A40, its pointer 0xB72F54.
// Interim placement (D137): <string>'s guarded static (0x4A2650) and GClass's adjustor thunk (0x4A2680) come
// before it, which may make it an object of its own (no __FILE__ names one)
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

