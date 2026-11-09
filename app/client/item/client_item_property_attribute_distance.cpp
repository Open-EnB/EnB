#include "client_item_property_attribute_distance.h"
#include "..\..\shared\g_assert.h"
#include <stdio.h>
























RefCountPtr<ClientItemPropertyAttributeDistanceClass> ClientItemPropertyAttributeDistanceClass::Create(int sort, const std::string &label, const std::string &tooltip, int unknown, int base, int actual, int level)
{
	FAIL_IF(label.empty()) return(RefCountPtr<ClientItemPropertyAttributeDistanceClass>());
	FAIL_IF(tooltip.empty()) return(RefCountPtr<ClientItemPropertyAttributeDistanceClass>());
	return(Create_NEW(new ClientItemPropertyAttributeDistanceClass(sort, label, tooltip, unknown, base, actual, level)));
}

ClientItemPropertyAttributeDistanceClass::ClientItemPropertyAttributeDistanceClass(int sort, const std::string &label, const std::string &tooltip, int unknown, int base, int actual, int level) :
	ClientItemPropertyAttributeClass(sort, label, "units", tooltip, unknown, level),
	Base(base),
	Actual(actual)
{
}

ClientItemPropertyAttributeDistanceClass::~ClientItemPropertyAttributeDistanceClass()
{
}

std::string ClientItemPropertyAttributeDistanceClass::Unknown_14()
{
	char buffer[128];
	_snprintf(buffer, sizeof(buffer), "%1.1fK", Actual * 0.001f);
	return(buffer);
}

/*
**	Reconstruction notes (RECONSTRUCTED): the client's 0x51FEA0-0x5203BF (Create 0x51FEA0, its FAIL_IFs at
**	lines 30 and 31; the constructor 0x520080, the destructors 0x5201F0 / 0x5201C0, the table 0xAEDC24's slot
**	0x520210 (its constant 0.001f at 0xAEDC40), Create_NEW<ClientItemPropertyAttributeDistanceClass> 0x520370,
**	and <string>'s guarded static 0x5203A0).
*/
