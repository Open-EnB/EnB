// Earth & Beyond client (reconstruction): app\client\unknown_7cef30.cpp, PROVISIONAL file name (D137; notes at the end)
#include "unknown_7cef30.h"
#include <stdlib.h>

std::string Unknown_BE8AA0;
std::string Unknown_BE8AB0;
std::string Unknown_BE8AC0;
int Unknown_BE8AD4;
Sub14CClass *Unknown_BE8AD8;

void RegistrationUnknown1ACClass::Unknown_7CEF30(Sub14CClass *a, const char *url, const char *page, const char *version, RegistrationCallbackClass *callback)
{
	Unknown_BE8AC0 = url;
	Unknown_BE8AB0 = page;
	Unknown_BE8AA0 = version;
	Unknown_BE8AD8 = a;
	Unknown_BE8AD4 = (int)callback;
}

void RegistrationUnknown1ACClass::Unknown_7CF190()
{
	Unknown_BE8AD4 = 0;
	Unknown_BE8AC0 = "";
	Unknown_BE8AB0 = "";
	Unknown_BE8AA0 = "";
}

SubsRequestClass::SubsRequestClass(const char *url, const char *page, const char *version, bool a, bool b) :
	Unknown04(a),
	Unknown05(b),
	Query("")
{
	Url = url;
	Url += "/";
	Url += page;
	Add_Param("version", version);
}

// NOT EXACT: inline budget (0.48: the client inlines more of the strings' assigns)
SubsRequestClass::SubsRequestClass(const std::string &url, const std::string &page, const std::string &version, bool a, bool b) :
	Unknown04(a),
	Unknown05(b),
	Query("")
{
	Url = url;
	Url += "/";
	Url += page;
	Add_Param("version", version.c_str());
}

void SubsRequestClass::Add_Param(const char *name, const char *value)
{
	if (Query.length() && *Query.rend() != '&') {
		Query += "&";
	}
	Query += name;
	Query += "=";
	Query += value;
}

// NOT EXACT: inline budget (0.40: the client inlines the string's _Eos, and the buffer is larger)
void SubsRequestClass::Add_Param(const char *name, int value)
{
	char buffer[52];
	std::string text(itoa(value, buffer, 10));
	Add_Param(name, text.c_str());
}

// NOT EXACT: inline budget (0.35: the client inlines the string's _Eos and the last append)
void SubsRequestClass::Add_Param(const char *name, bool value)
{
	std::string text(value ? "1" : "0");
	Add_Param(name, text.c_str());
}

/*
** Reconstruction notes (XR1, 2026-11-08)
**
** The object 0x7CECC0-0x7DFE70, between settings_store.cpp's (its map / string helpers end at 0x7CEC90) and the
** registration reader's (Sub14CImplClass, slots 0x7DFED0-0x7E15B0). No __FILE__ or assert: the name is PROVISIONAL
** (D137), after its first function. It opens with the globals' initialisers (0x7CECC0-0x7CEE70: the four $E pairs),
** then:
** - 0x7CEF30 (cdecl, so static) / 0x7CF190: set / clear the globals (url, page, version, the reader, the callback).
** - 0x7CF3E0-0x7D18A0: reply readers, each filling one record from the reader's slot 0x0C (path, index, name):
**   account info 0x7CF3E0 ('user-status' ...), billing 0x7CF880, pricing selection 0x7CFEA0, bundle 0x7D0350,
**   game feature 0x7D0610, coupon 0x7D0930, pricing option 0x7D0BE0, subscription 0x7D1060 (lists through
**   '%s[%d]/game-feature-list/game-feature', '%s[%d]/coupon-list/coupon'), login 0x7D1620 ('lkey'), 0x7D18A0 ('type').
** - 0x7D1AC0: the reply's '/status', '/message', '/url' ('/@dtdversion', 'NO DOCTYPE Specification.').
** - 0x7D21C0-0x7D4530: the '/subs-interface-reply/...' readers (get-acctinfo 0x7D21C0, billing 0x7D28B0, login
**   0x7D3000, is-paying-subscriber 0x7D33D0, pricing selections 0x7D3570, bundles 0x7D3960, subscription info
**   0x7D3CC0, credit card types 0x7D4530).
** - 0x7D47F0-0x7DB850: one request function per 'methodCall' (getAcctInfo 0x7D47F0, addAcctInfo 0x7D4E30,
**   updateAcctInfo 0x7D5600, getBillingInfo 0x7D5CE0, updateBillingInfo 0x7D6180, changePassword 0x7D6A60,
**   sendPassword 0x7D6FE0, sendScreenname 0x7D7480, authAOLScreenName 0x7D7920, login 0x7D7E10, isPayingSubscriber
**   0x7D8380, getPricingSelection 0x7D88C0, ...ByBundle 0x7D8D60, ...ByCode 0x7D92A0, applyPricingSelection
**   0x7D9790, getActiveBundles 0x7D9E20, getAllBundlesByGame 0x7DA2C0, getSubscriptionInfo 0x7DA760,
**   suspendEntitlement 0x7DAD50, stopBilling 0x7DB2D0, getCreditCardTypeList 0x7DB850): a SubsRequestClass on the
**   stack from the globals (the std::string constructor 0x7DC0B0 inlined), Add_Param per key ('skin' 'eb' last), then
**   the reader's slot 0x34 (request, callback). Their destructor's out-of-line copies are 0x7D4D60 / 0x7D4E00.
** - 0x7DBDA0 / 0x7DC0B0: SubsRequestClass's constructors (no callers: the request functions inline the second);
**   0x7DC420 / 0x7DC6D0 / 0x7DC8F0: Add_Param, called out of line where the callers' inline budget runs out (the
**   constructor's 'version' always).
** - 0x7DCB10-0x7DFE70: the vector / list helpers of the readers' records.
** Add_Param's '&' test reads the byte before the string (*rend()): the client's, a bug (the reference count byte).
*/
