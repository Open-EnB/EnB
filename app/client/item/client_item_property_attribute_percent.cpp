#include "client_item_property_attribute_percent.h"
#include "..\..\shared\g_assert.h"
#include <stdio.h>
























RefCountPtr<ClientItemPropertyAttributePercentClass> ClientItemPropertyAttributePercentClass::Create(int sort, const std::string &label, const std::string &tooltip, int unknown, float base, float actual, int level)
{
	FAIL_IF(label.empty()) return(RefCountPtr<ClientItemPropertyAttributePercentClass>());
	FAIL_IF(tooltip.empty()) return(RefCountPtr<ClientItemPropertyAttributePercentClass>());
	return(Create_NEW(new ClientItemPropertyAttributePercentClass(sort, label, tooltip, unknown, base, actual, level)));
}

ClientItemPropertyAttributePercentClass::ClientItemPropertyAttributePercentClass(int sort, const std::string &label, const std::string &tooltip, int unknown, float base, float actual, int level) :
	ClientItemPropertyAttributeClass(sort, label, "", tooltip, unknown, level),
	Base(base),
	Actual(actual)
{
}

ClientItemPropertyAttributePercentClass::~ClientItemPropertyAttributePercentClass()
{
}

std::string ClientItemPropertyAttributePercentClass::Unknown_14()
{
	char buffer[128];
	_snprintf(buffer, sizeof(buffer), "%0.0f%%", Actual * 100.0f);
	return(buffer);
}

/*
**	Reconstruction notes (RECONSTRUCTED): the client's 0x520B20-0x52104F (Create 0x520B20, its FAIL_IFs at
**	lines 30 and 31; the constructor 0x520D00, the destructors 0x520E70 / 0x520E40, the table 0xAEDC7C's slot
**	0x520E90, Create_NEW<ClientItemPropertyAttributePercentClass> 0x520FF0, and <string>'s guarded static 0x521020).
*/
