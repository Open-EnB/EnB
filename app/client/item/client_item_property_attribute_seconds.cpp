#include "client_item_property_attribute_seconds.h"
#include "..\..\shared\g_assert.h"
#include <stdio.h>
























RefCountPtr<ClientItemPropertyAttributeSecondsClass> ClientItemPropertyAttributeSecondsClass::Create(int sort, const std::string &label, const std::string &tooltip, int unknown, float base, float actual, int level)
{
	FAIL_IF(label.empty()) return(RefCountPtr<ClientItemPropertyAttributeSecondsClass>());
	FAIL_IF(tooltip.empty()) return(RefCountPtr<ClientItemPropertyAttributeSecondsClass>());
	return(Create_NEW(new ClientItemPropertyAttributeSecondsClass(sort, label, tooltip, unknown, base, actual, level)));
}

ClientItemPropertyAttributeSecondsClass::ClientItemPropertyAttributeSecondsClass(int sort, const std::string &label, const std::string &tooltip, int unknown, float base, float actual, int level) :
	ClientItemPropertyAttributeClass(sort, label, "seconds", tooltip, unknown, level),
	Base(base),
	Actual(actual)
{
}

ClientItemPropertyAttributeSecondsClass::~ClientItemPropertyAttributeSecondsClass()
{
}

std::string ClientItemPropertyAttributeSecondsClass::Unknown_14()
{
	char buffer[128];
	_snprintf(buffer, sizeof(buffer), "%2.1f", Actual);
	return(buffer);
}

/*
**	Reconstruction notes (RECONSTRUCTED): the client's 0x521050-0x52155F (Create 0x521050, its FAIL_IFs at
**	lines 30 and 31; the constructor 0x521230, the destructors 0x5213A0 / 0x521370, the table 0xAEDC98's slot
**	0x5213C0, Create_NEW<ClientItemPropertyAttributeSecondsClass> 0x521500, and <string>'s guarded static 0x521530).
*/
