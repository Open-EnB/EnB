// Earth & Beyond client (reconstruction): app\client\unknown_73fa60.cpp (PROVISIONAL file name, D137). Notes at the end.
#include <stdlib.h>
#include "..\..\wwlib\listnode.h"
#include <string>	// the client object ends with <string>'s guard 0x73FE70 (CRT entry 588)
class ClientObjectClass;

class UnknownAF0DD0Class								// NAME 1: neutral (D10), vtable 0xAF0DD0
{
public:
	virtual ~UnknownAF0DD0Class() {}
	virtual void Unknown_04(int arg) = 0;				// +0x04 (_purecall in 0xAF0DD0)  // NAME 1 [arg]: neutral
	void Unknown_73FDB0(DataNode<UnknownAF0DD0Class *> *node, ClientObjectClass *object);	// 0x73FDB0  // NAME 1 [node, object]: neutral
};

class Unknown8CClass : public GenericList				// NAME 1: neutral (D10), co.h +0x8C list (vtable 0xB03194)
{
public:
	Unknown8CClass();									// 0x73FA60
	virtual ~Unknown8CClass();							// 0x73FB20 (scalar deleting dtor 0x73FAF0)
};

class ClientObjectClass
{
public:
	Unknown8CClass *Unknown_6F5090();					// co.cpp's +0x8C accessor (0x6F5090)
};

Unknown8CClass::Unknown8CClass()
{
}

Unknown8CClass::~Unknown8CClass()
{
	DataNode<UnknownAF0DD0Class *> *node = (DataNode<UnknownAF0DD0Class *> *)First_Valid();	// NAME 1 [node, next]: neutral
	while (node != NULL) {
		DataNode<UnknownAF0DD0Class *> *next = node->Next_Valid();
		if (node->Get() != NULL) {
			node->Get()->Unknown_04((int)node);
			node->Unlink();
		}
		node->Unlink();
		node = next;
	}
}

void UnknownAF0DD0Class::Unknown_73FDB0(DataNode<UnknownAF0DD0Class *> *node, ClientObjectClass *object)
{
	if (node->Get() != NULL) {
		node->Unlink();
		node->Set(NULL);
	}
	if (object != NULL) {
		node->Set(this);
		object->Unknown_6F5090()->Add_Head(node);
	}
}

/*
**	Reconstruction notes
**
**	client.exe 0x73FA60-0x73FE38 (followed by <string>'s guarded initialiser 0x73FE70, after
**	cpacket.cpp's 0x73FA30 and before Unknown_73FEF0): Unknown8CClass (a GenericList subclass at
**	ClientObjectClass+0x8C, vtable 0xB03194: constructor 0x73FA60, scalar deleting destructor
**	0x73FAF0, destructor 0x73FB20, plus GenericList::~GenericList 0x73FCE0 for the EH funclet
**	0xACAB13) and UnknownAF0DD0Class::Unknown_73FDB0 (0x73FDB0: detaches a watch node from its
**	previous object list and links it onto object->Unknown_6F5090()). No __FILE__ string; the
**	file name is provisional (D137).
*/
