#include "client_item_property_attribute_integer.h"
#include "..\..\shared\g_assert.h"
#include <stdio.h>

























RefCountPtr<ClientItemPropertyAttributeIntegerClass> ClientItemPropertyAttributeIntegerClass::Create(int sort, const std::string &label, const std::string &units, const std::string &tooltip, int unknown, int base, int actual, int level)
{
	FAIL_IF(label.empty()) return(RefCountPtr<ClientItemPropertyAttributeIntegerClass>());
	FAIL_IF(tooltip.empty()) return(RefCountPtr<ClientItemPropertyAttributeIntegerClass>());
	return(Create_NEW(new ClientItemPropertyAttributeIntegerClass(sort, label, units, tooltip, unknown, base, actual, level)));
}

ClientItemPropertyAttributeIntegerClass::ClientItemPropertyAttributeIntegerClass(int sort, const std::string &label, const std::string &units, const std::string &tooltip, int unknown, int base, int actual, int level) :
	ClientItemPropertyAttributeClass(sort, label, units, tooltip, unknown, level),
	Base(base),
	Actual(actual)
{
}

ClientItemPropertyAttributeIntegerClass::~ClientItemPropertyAttributeIntegerClass()
{
}

std::string ClientItemPropertyAttributeIntegerClass::Unknown_14()
{
	char buffer[128];
	_snprintf(buffer, sizeof(buffer), "%d", Actual);
	return(buffer);
}

/*
**	Reconstruction notes (RECONSTRUCTED): the client's 0x520790-0x520B1F (Create 0x520790, its FAIL_IFs at
**	lines 31 and 32; the constructor 0x5208C0, the destructors 0x520960 / 0x520930, the table 0xAEDC60's slot
**	0x520980, Create_NEW<ClientItemPropertyAttributeIntegerClass> 0x520AC0, and <string>'s guarded static 0x520AF0).
*/
