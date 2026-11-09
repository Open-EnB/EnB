// Earth & Beyond client (reconstruction): app\client\unknown_7cef30.cpp, PROVISIONAL file name (D137; notes at the end)
#include "unknown_7cef30.h"
#include <stdlib.h>
#include <stdio.h>

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

void RegistrationUnknown1ACClass::Unknown_7CFEA0(RegistrationUnknown578Struct *entry, const char *path, int index)
{
	if (Unknown_BE8AD8 == NULL) {
		return;
	}
	entry->Unknown00 = Unknown_BE8AD8->Slot3(path, index, "pricing-selection-id");
	entry->Unknown10 = Unknown_BE8AD8->Slot3(path, index, "pricing-selection-primary-bundle-id");
	entry->Unknown20 = Unknown_BE8AD8->Slot4(path, index, "pricing-selection-type");
	entry->Unknown24 = Unknown_BE8AD8->Slot3(path, index, "pricing-selection-name");
	entry->Unknown34 = Unknown_BE8AD8->Slot3(path, index, "pricing-selection-desc");
	entry->Unknown44 = Unknown_BE8AD8->Slot4(path, index, "pricing-selection-access-level");
	entry->Unknown48 = Unknown_BE8AD8->Slot4(path, index, "pricing-selection-display-order");
	char list[1024];
	_snprintf(list, sizeof(list), "%s[%d]/payment-type-list", path, index);
	int count = Unknown_BE8AD8->Slot6(list);
	for (int i = 0; i < count; i++) {
		int type = Unknown_BE8AD8->Slot4(list, i, "payment-type");
		entry->Unknown4C.push_back(type);
	}
}

void RegistrationUnknown1ACClass::Unknown_7D0350(SubsBundleStruct *bundle, const char *path, int index)
{
	if (Unknown_BE8AD8 == NULL) {
		return;
	}
	bundle->BundleId = Unknown_BE8AD8->Slot3(path, index, "bundle-id");
	bundle->BundleType = Unknown_BE8AD8->Slot4(path, index, "bundle-type");
	bundle->BundleStatus = Unknown_BE8AD8->Slot4(path, index, "bundle-status");
	bundle->BundleName = Unknown_BE8AD8->Slot3(path, index, "bundle-name");
	bundle->BundleDescription = Unknown_BE8AD8->Slot3(path, index, "bundle-description");
}

void RegistrationUnknown1ACClass::Unknown_7D0610(SubsGameFeatureStruct *feature, const char *path, int index)
{
	if (Unknown_BE8AD8 == NULL) {
		return;
	}
	feature->GameFeatureId = Unknown_BE8AD8->Slot4(path, index, "game-feature-id");
	feature->GameId = Unknown_BE8AD8->Slot3(path, index, "game-id");
	feature->Name = Unknown_BE8AD8->Slot3(path, index, "game-feature-name");
	feature->Description = Unknown_BE8AD8->Slot3(path, index, "game-feature-description");
	feature->Type = Unknown_BE8AD8->Slot4(path, index, "game-feature-type");
	feature->Requirements = Unknown_BE8AD8->Slot4(path, index, "game-feature-requirements");
	feature->Url = Unknown_BE8AD8->Slot3(path, index, "game-feature-url");
}

void RegistrationUnknown1ACClass::Unknown_7D0930(SubsCouponStruct *coupon, const char *path, int index)
{
	if (Unknown_BE8AD8 == NULL) {
		return;
	}
	coupon->CouponId = Unknown_BE8AD8->Slot4(path, index, "coupon-id");
	coupon->NextBillDate = Unknown_BE8AD8->Slot3(path, index, "coupon-next-bill-date");
	coupon->RolloverDate = Unknown_BE8AD8->Slot3(path, index, "coupon-rollover-date");
	coupon->Source = Unknown_BE8AD8->Slot4(path, index, "coupon-source");
	coupon->Status = Unknown_BE8AD8->Slot4(path, index, "coupon-status");
	coupon->StatusReasonCode = Unknown_BE8AD8->Slot4(path, index, "coupon-status-reason-code");
}

void RegistrationUnknown1ACClass::Unknown_7D0BE0(SubsPricingOptionStruct *option, const char *path, int index)
{
	if (Unknown_BE8AD8 == NULL) {
		return;
	}
	option->BundleId = Unknown_BE8AD8->Slot3(path, index, "bundle-id");
	option->PricingOptionId = Unknown_BE8AD8->Slot3(path, index, "pricing-option-id");
	option->LinkedId = Unknown_BE8AD8->Slot3(path, index, "pricing-option-linked-id");
	option->Type = Unknown_BE8AD8->Slot4(path, index, "pricing-option-type");
	option->Name = Unknown_BE8AD8->Slot3(path, index, "pricing-option-name");
	option->Description = Unknown_BE8AD8->Slot3(path, index, "pricing-option-description");
	option->ChainDesc = Unknown_BE8AD8->Slot3(path, index, "pricing-option-chain-desc");
	option->Value = Unknown_BE8AD8->Slot4(path, index, "pricing-option-value");
	option->Price = Unknown_BE8AD8->Slot5(path, index, "pricing-option-price");
	option->Units = Unknown_BE8AD8->Slot4(path, index, "pricing-option-units");
	option->ExpirationDate = Unknown_BE8AD8->Slot3(path, index, "pricing-option-expiration-date");
	option->RecurringCycles = Unknown_BE8AD8->Slot4(path, index, "pricing-option-recurring-cycles");
}

void RegistrationUnknown1ACClass::Unknown_7D1060(RegistrationUnknown568Struct *info, const char *path, int index)
{
	if (Unknown_BE8AD8 == NULL) {
		return;
	}
	info->EntitlementStatus = Unknown_BE8AD8->Slot4(path, index, "entitlement-status");
	info->EntitlementStatusReason = Unknown_BE8AD8->Slot4(path, index, "entitlement-status-reason");
	info->EntitlementSuspendDate = Unknown_BE8AD8->Slot3(path, index, "entitlement-suspend-date");
	Unknown_7D0350(&info->Bundle, (std::string(path) + "/bundle").c_str(), index);
	Unknown_7D0BE0(&info->PricingOption, (std::string(path) + "/pricing-option").c_str(), index);
	char list[1024];
	_snprintf(list, sizeof(list), "%s[%d]/game-feature-list/game-feature", path, index);
	int count = Unknown_BE8AD8->Slot6(list);
	for (int i = 0; i < count; i++) {
		SubsGameFeatureStruct feature;
		Unknown_7D0610(&feature, list, i);
		info->GameFeatures.push_back(feature);
	}
	_snprintf(list, sizeof(list), "%s[%d]/coupon-list/coupon", path, index);
	count = Unknown_BE8AD8->Slot6(list);
	for (int j = 0; j < count; j++) {
		SubsCouponStruct coupon;
		Unknown_7D0930(&coupon, list, j);
		info->Coupons.push_back(coupon);
	}
}

std::vector<RegistrationUnknown578Struct> RegistrationUnknown1ACClass::Unknown_7D3570()
{
	std::vector<RegistrationUnknown578Struct> list;
	if (Unknown_BE8AD8 == NULL) {
		return list;
	}
	const char *path = "/subs-interface-reply/get-pricing-selection-reply/pricing-selection";
	int count = Unknown_BE8AD8->Slot6(path);
	if (count == 0) {
		path = "/subs-interface-reply/get-pricing-selections-by-bundle-reply/pricing-selection-list/pricing-selection";
		count = Unknown_BE8AD8->Slot6(path);
		if (count == 0) {
			path = "/subs-interface-reply/get-pricing-selections-by-code-reply/pricing-selection-list/pricing-selection";
			count = Unknown_BE8AD8->Slot6(path);
			if (count == 0) {
				return list;
			}
		}
	}
	for (int i = 0; i < count; i++) {
		RegistrationUnknown578Struct entry;
		Unknown_7CFEA0(&entry, path, i);
		list.push_back(entry);
	}
	return list;
}

// NOT EXACT: EH states (0.73: the client destroys the record's bundle strings under one state, ours one each)
std::vector<RegistrationUnknown568Struct> RegistrationUnknown1ACClass::Unknown_7D3CC0()
{
	std::vector<RegistrationUnknown568Struct> list;
	if (Unknown_BE8AD8 == NULL) {
		return list;
	}
	const char *path = "/subs-interface-reply/get-subscription-info-reply/subscription-info-list/subscription-info";
	int count = Unknown_BE8AD8->Slot6(path);
	for (int i = 0; i < count; i++) {
		RegistrationUnknown568Struct info;
		Unknown_7D1060(&info, path, i);
		list.push_back(info);
	}
	return list;
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
