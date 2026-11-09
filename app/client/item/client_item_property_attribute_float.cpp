#include "client_item_property_attribute_float.h"
#include "..\..\shared\g_assert.h"
#include <stdio.h>
























RefCountPtr<ClientItemPropertyAttributeFloatClass> ClientItemPropertyAttributeFloatClass::Create(int sort, const std::string &label, const std::string &units, const std::string &tooltip, int unknown, float base, float actual, int level)
{
	FAIL_IF(label.empty()) return(RefCountPtr<ClientItemPropertyAttributeFloatClass>());
	FAIL_IF(units.empty()) return(RefCountPtr<ClientItemPropertyAttributeFloatClass>());
	FAIL_IF(tooltip.empty()) return(RefCountPtr<ClientItemPropertyAttributeFloatClass>());
	return(Create_NEW(new ClientItemPropertyAttributeFloatClass(sort, label, units, tooltip, unknown, base, actual, level)));
}

ClientItemPropertyAttributeFloatClass::ClientItemPropertyAttributeFloatClass(int sort, const std::string &label, const std::string &units, const std::string &tooltip, int unknown, float base, float actual, int level) :
	ClientItemPropertyAttributeClass(sort, label, units, tooltip, unknown, level),
	Base(base),
	Actual(actual)
{
}

ClientItemPropertyAttributeFloatClass::~ClientItemPropertyAttributeFloatClass()
{
}

std::string ClientItemPropertyAttributeFloatClass::Unknown_14()
{
	char buffer[128];
	_snprintf(buffer, sizeof(buffer), "%0.1f", Actual);
	return(buffer);
}

/*
**	Reconstruction notes (RECONSTRUCTED): the client's 0x5203D0-0x52078F (Create 0x5203D0, its FAIL_IFs at
**	lines 30, 31 and 32; the constructor 0x520530, the destructors 0x5205D0 / 0x5205A0, the table 0xAEDC44's slot
**	0x5205F0, Create_NEW<ClientItemPropertyAttributeFloatClass> 0x520730, and <string>'s guarded static 0x520760).
*/
