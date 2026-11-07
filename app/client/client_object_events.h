// Earth & Beyond client (reconstruction): the events emitted on a client object through the signal map 0x4F7EF0.
// The header's name is neutral (no __FILE__ names it; interim placement, D137).
#ifndef CLIENT_OBJECT_EVENTS_H  // NAME 1 [client_object_events.h]: neutral header name (D10)
#define CLIENT_OBJECT_EVENTS_H

#include "..\packets\pk_player_object_relationship.h"

class ClientObjectClass;

// The event 0x6FEEE0 emits (the client's RTTI .?AVPlayerObjectRelationshipChangeEvent@@, 0x14 bytes)
class PlayerObjectRelationshipChangeEvent	// NAME 5: the client's RTTI name
{
public:
	PlayerObjectRelationshipChangeEvent() : Object(0) {}	// (radar_data.cpp's 0x7BB1D0 / 0x7BB450)
	PlayerObjectRelationshipChangeEvent(ClientObjectClass *object, const RelationshipClass &old_relationship, const RelationshipClass &new_relationship) :
		Object(object), Old(old_relationship), New(new_relationship) {}

	ClientObjectClass *Object;		// +0x00  // NAME 2: what it holds
	RelationshipClass Old;			// +0x04  // NAME 2: what it holds
	RelationshipClass New;			// +0x0C  // NAME 2: what it holds
};

// ui_group.cpp's event for an object
class ClientGroupEvent	// NAME 5: the client's RTTI .?AVClientGroupEvent@@
{
public:
	ClientGroupEvent() {}									// (radar_data.cpp's 0x7BB1D0 / 0x7BB450: no store)
	ClientGroupEvent(bool value) : Value(value) {}
	bool Value;											// +0x00  // NAME 1: neutral
};

#endif
