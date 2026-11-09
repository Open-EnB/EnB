#include "client_item_property_attribute_string.h"
#include "..\..\shared\g_assert.h"
























RefCountPtr<ClientItemPropertyAttributeStringClass> ClientItemPropertyAttributeStringClass::Create(int sort, const std::string &label, const std::string &units, const std::string &tooltip, int unknown, const std::string &value)
{
	FAIL_IF(label.empty()) return(RefCountPtr<ClientItemPropertyAttributeStringClass>());
	FAIL_IF(tooltip.empty()) return(RefCountPtr<ClientItemPropertyAttributeStringClass>());
	return(Create_NEW(new ClientItemPropertyAttributeStringClass(sort, label, units, tooltip, unknown, value)));
}

ClientItemPropertyAttributeStringClass::ClientItemPropertyAttributeStringClass(int sort, const std::string &label, const std::string &units, const std::string &tooltip, int unknown, const std::string &value) :
	ClientItemPropertyAttributeClass(sort, label, units, tooltip, unknown, 1),
	Value(value)
{
}

ClientItemPropertyAttributeStringClass::~ClientItemPropertyAttributeStringClass()
{
}

std::string ClientItemPropertyAttributeStringClass::Unknown_14()
{
	return(Value);
}

/*
**	Reconstruction notes (RECONSTRUCTED): the client's 0x521560-0x521B1F (Create 0x521560, its FAIL_IFs at
**	lines 29 and 30; the constructor 0x5216D0, the destructors 0x5218E0 / 0x5218B0, the table 0xAEDCB4's slot
**	0x521950, Create_NEW<ClientItemPropertyAttributeStringClass> 0x521AD0, and <string>'s guarded static
**	0x521B00).
*/
