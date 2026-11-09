// Earth & Beyond client (reconstruction): app\client\unknown_7cef30.cpp, PROVISIONAL file name (D137; notes at the end)
#include "unknown_7cef30.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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

void RegistrationUnknown1ACClass::Unknown_7CF3E0(RegistrationUnknown3C4Struct *info, const char *path, int index)
{
	if (Unknown_BE8AD8 == NULL) {
		return;
	}
	info->Unknown50 = Unknown_BE8AD8->Slot3(path, index, "user-status").c_str()[0];
	info->Unknown54 = Unknown_BE8AD8->Slot4(path, index, "status-reason-code");
	int length = Unknown_BE8AD8->Slot3(path, index, "gender").length();
	if (length != 0) {
		info->Unknown58 = Unknown_BE8AD8->Slot4(path, index, "gender");
	} else {
		info->Unknown58 = 2;
	}
	info->Unknown5C = Unknown_BE8AD8->Slot3(path, index, "email");
	info->Unknown6C = Unknown_BE8AD8->Slot3(path, index, "parental-email");
	info->Unknown7C = (char)Unknown_BE8AD8->Slot4(path, index, "bdateday");
	info->Unknown7D = (char)Unknown_BE8AD8->Slot4(path, index, "bdatemonth");
	info->Unknown7E = (short)Unknown_BE8AD8->Slot4(path, index, "bdateyear");
	info->Unknown80 = Unknown_BE8AD8->Slot3(path, index, "country");
	info->Unknown90 = Unknown_BE8AD8->Slot4(path, index, "ea-email-flag") > 0;
	info->Unknown91 = Unknown_BE8AD8->Slot4(path, index, "third-party-share-flag") > 0;
	info->Unknown94 = Unknown_BE8AD8->Slot3(path, index, "profile-id");
	info->UnknownA4 = Unknown_BE8AD8->Slot3(path, index, "long-screenname");
}

void RegistrationUnknown1ACClass::Unknown_7CF880(RegistrationUnknown478Struct *info, const char *path, int index)
{
	if (Unknown_BE8AD8 == NULL) {
		return;
	}
	info->Unknown00 = Unknown_BE8AD8->Slot4(path, index, "billing-id");
	info->Unknown04 = Unknown_BE8AD8->Slot3(path, index, "first-name");
	info->Unknown14 = Unknown_BE8AD8->Slot3(path, index, "last-name");
	info->Unknown24 = Unknown_BE8AD8->Slot3(path, index, "address");
	info->Unknown34 = Unknown_BE8AD8->Slot3(path, index, "address2");
	info->Unknown44 = Unknown_BE8AD8->Slot3(path, index, "city");
	info->Unknown54 = Unknown_BE8AD8->Slot3(path, index, "state-province");
	info->Unknown64 = Unknown_BE8AD8->Slot3(path, index, "country");
	info->Unknown74 = Unknown_BE8AD8->Slot3(path, index, "postal-code");
	info->Unknown84 = Unknown_BE8AD8->Slot3(path, index, "phone");
	info->Unknown94.Unknown00 = Unknown_BE8AD8->Slot4(path, index, "cc-type");
	info->UnknownA8 = Unknown_BE8AD8->Slot3(path, index, "cc-number");
	info->UnknownE8 = Unknown_BE8AD8->Slot4(path, index, "expire-month");
	info->UnknownEC = Unknown_BE8AD8->Slot4(path, index, "expire-year");
	info->UnknownC8 = (char)Unknown_BE8AD8->Slot4(path, index, "start-month");
	info->UnknownD8 = (char)Unknown_BE8AD8->Slot4(path, index, "start-year");
	info->UnknownB8 = (char)Unknown_BE8AD8->Slot4(path, index, "issuer");
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

void RegistrationUnknown1ACClass::Unknown_7D1620(SubsLoginStruct *login, const char *path, int index)
{
	if (Unknown_BE8AD8 == NULL) {
		return;
	}
	login->Lkey = Unknown_BE8AD8->Slot3(path, index, "lkey");
	login->ProfileId = Unknown_BE8AD8->Slot3(path, index, "profile-id");
	login->ExpirationDate = Unknown_BE8AD8->Slot3(path, index, "entitlement-expiration-date");
}

void RegistrationUnknown1ACClass::Unknown_7D18A0(RegistrationUnknown5A8Struct *entry, const char *path, int index)
{
	if (Unknown_BE8AD8 == NULL) {
		return;
	}
	entry->Unknown00 = Unknown_BE8AD8->Slot8(path, "type", index, 0);
	entry->Unknown04 = Unknown_BE8AD8->Slot3(path, index, NULL);
}

RegistrationErrorStruct RegistrationUnknown1ACClass::Unknown_7D1AC0()
{
	RegistrationErrorStruct error;
	if (Unknown_BE8AD8 != NULL && Unknown_BE8AD8->Slot16() != "") {
		std::string name = Unknown_BE8AD8->Slot15();
		std::string root = Unknown_BE8AD8->Slot16();
		std::string path;
		path = "/" + root + "/status";
		error.Unknown10 = Unknown_BE8AD8->Slot4(path.c_str(), 0, NULL);
		path = "/" + root + "/message";
		error.Unknown14 = Unknown_BE8AD8->Slot3(path.c_str(), 0, NULL);
		path = "/" + root + "/url";
		error.Unknown24 = Unknown_BE8AD8->Slot3(path.c_str(), 0, NULL);
		path = "/" + root + "/@dtdversion";
		error.Unknown00 = Unknown_BE8AD8->Slot3(path.c_str(), 0, NULL);
	} else {
		error.Unknown14 = "NO DOCTYPE Specification.";
		error.Unknown24 = "";
		error.Unknown10 = -1;
		error.Unknown00 = "";
	}
	return error;
}

RegistrationUnknown3C4Struct RegistrationUnknown1ACClass::Unknown_7D21C0()
{
	RegistrationUnknown3C4Struct info;
	if (Unknown_BE8AD8 == NULL) {
		return info;
	}
	Unknown_7CF3E0(&info, "/subs-interface-reply/get-acctinfo-reply/account-info", 0);
	for (int i = 0; i < 5; i++) {
		info.Unknown00[i] = Unknown_BE8AD8->Slot3("/subs-interface-reply/add-acctinfo-reply/alt-name-list/alt-name", i, NULL);
	}
	return info;
}

RegistrationUnknown478Struct RegistrationUnknown1ACClass::Unknown_7D28B0()
{
	RegistrationUnknown478Struct info;
	Unknown_7CF880(&info, "/subs-interface-reply/get-billing-info-reply/billing-info", 0);
	return info;
}

SubsLoginStruct RegistrationUnknown1ACClass::Unknown_7D3000()
{
	SubsLoginStruct login;
	Unknown_7D1620(&login, "/subs-interface-reply/login-reply", 0);
	return login;
}

SubsPayingStruct RegistrationUnknown1ACClass::Unknown_7D33D0()
{
	SubsPayingStruct paying;
	std::string text = Unknown_BE8AD8->Slot3("/subs-interface-reply/is-paying-subscriber-reply", 0, "paying-subscriber");
	paying.PayingSubscriber = (text[0] == 'T' || text[0] == 't');
	return paying;
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

std::vector<SubsBundleStruct> RegistrationUnknown1ACClass::Unknown_7D3960()
{
	std::vector<SubsBundleStruct> list;
	if (Unknown_BE8AD8 == NULL) {
		return list;
	}
	const char *path = "/subs-interface-reply/get-active-bundles-reply/bundle-list/bundle";
	int count = Unknown_BE8AD8->Slot6(path);
	if (count == 0) {
		path = "/subs-interface-reply/get-all-bundles-by-game-reply/bundle-list/bundle";
		count = Unknown_BE8AD8->Slot6(path);
		if (count == 0) {
			return list;
		}
	}
	for (int i = 0; i < count; i++) {
		SubsBundleStruct bundle;
		Unknown_7D0350(&bundle, path, i);
		list.push_back(bundle);
	}
	return list;
}

// Defined here, after 0x7D3960 (which expands the implicit form) and before 0x7D3CC0 (which expands this one
// under one EH state); the deleting destructor 0x7DF400 calls the byte-identical copy 0x60A3E0
SubsBundleStruct::~SubsBundleStruct()
{
}

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

std::vector<RegistrationUnknown5A8Struct> RegistrationUnknown1ACClass::Unknown_7D4530()
{
	std::vector<RegistrationUnknown5A8Struct> list;
	if (Unknown_BE8AD8 == NULL) {
		return list;
	}
	const char *path = "/subs-interface-reply/get-credit-card-type-list-reply/credit-card-type-list/credit-card";
	int count = Unknown_BE8AD8->Slot6(path);
	for (int i = 0; i < count; i++) {
		RegistrationUnknown5A8Struct entry;
		Unknown_7D18A0(&entry, path, i);
		list.push_back(entry);
	}
	return list;
}

inline SubsRequestClass::SubsRequestClass(const std::string &url, const std::string &page, const std::string &version, bool a, bool b) :
	Unknown04(a),
	Unknown05(b),
	Query("")
{
	Url = url;
	Url += "/";
	Url += page;
	Add_Param("version", version.c_str());
}


bool RegistrationUnknown1ACClass::Unknown_7D47F0(const char *a, const char *b, const char *c, int callback)
{
	if (Unknown_BE8AD8 == NULL) {
		return false;
	}
	SubsRequestClass request(Unknown_BE8AC0, Unknown_BE8AB0, Unknown_BE8AA0);
	request.Add_Param("methodCall", "getAcctInfo");
	request.Add_Param("screenname", a);
	request.Add_Param("password", b);
	if (c != NULL && strlen(c) != 0) {
		request.Add_Param("lkey", c);
	}
	request.Add_Param("skin", "eb");
	return Unknown_BE8AD8->Slot13(&request, callback);
}

// NOT EXACT: inline budget (0.56: the client calls more of the request's string helpers out of line)
bool RegistrationUnknown1ACClass::Unknown_7D4E30(const char *a, const char *b, RegistrationUnknown3C4Struct *info, const char *c, bool d, int e, int f)
{
	if (Unknown_BE8AD8 == NULL) {
		return false;
	}
	SubsRequestClass request(Unknown_BE8AC0, Unknown_BE8AB0, Unknown_BE8AA0);
	request.Add_Param("methodCall", "addAcctInfo");
	request.Add_Param("screenname", a);
	request.Add_Param("password", b);
	request.Add_Param("send-email", d);
	if (c != NULL && strlen(c) != 0) {
		request.Add_Param("aolpassword", c);
	}
	request.Add_Param("gender", info->Unknown58);
	request.Add_Param("email", info->Unknown5C.c_str());
	request.Add_Param("parental-email", info->Unknown6C.c_str());
	request.Add_Param("bdatemonth", (int)info->Unknown7D);
	request.Add_Param("bdateday", (int)info->Unknown7C);
	request.Add_Param("bdateyear", (int)info->Unknown7E);
	request.Add_Param("country", info->Unknown80.substr(0, 2).c_str());
	request.Add_Param("ea-email-flag", info->Unknown90);
	request.Add_Param("third-party-share-flag", info->Unknown91);
	request.Add_Param("member-source", e);
	request.Add_Param("suggest-names", 1);
	request.Add_Param("skin", "eb");
	return Unknown_BE8AD8->Slot13(&request, f);
}

// NOT EXACT: inline budget (0.50: as 0x7D4E30)
bool RegistrationUnknown1ACClass::Unknown_7D5600(const char *a, const char *b, RegistrationUnknown3C4Struct *info, bool d, int e)
{
	if (Unknown_BE8AD8 == NULL) {
		return false;
	}
	SubsRequestClass request(Unknown_BE8AC0, Unknown_BE8AB0, Unknown_BE8AA0);
	request.Add_Param("methodCall", "updateAcctInfo");
	request.Add_Param("screenname", a);
	request.Add_Param("password", b);
	request.Add_Param("send-email", d);
	request.Add_Param("gender", info->Unknown58);
	request.Add_Param("email", info->Unknown5C.c_str());
	request.Add_Param("parental-email", info->Unknown6C.c_str());
	request.Add_Param("bdatemonth", (int)info->Unknown7D);
	request.Add_Param("bdateday", (int)info->Unknown7C);
	request.Add_Param("bdateyear", (int)info->Unknown7E);
	request.Add_Param("country", info->Unknown80.substr(0, 2).c_str());
	request.Add_Param("ea-email-flag", info->Unknown90);
	request.Add_Param("third-party-share-flag", info->Unknown91);
	request.Add_Param("skin", "eb");
	return Unknown_BE8AD8->Slot13(&request, e);
}

bool RegistrationUnknown1ACClass::Unknown_7D5CE0(const char *a, const char *b, int callback)
{
	if (Unknown_BE8AD8 == NULL) {
		return false;
	}
	SubsRequestClass request(Unknown_BE8AC0, Unknown_BE8AB0, Unknown_BE8AA0);
	request.Add_Param("methodCall", "getBillingInfo");
	request.Add_Param("screenname", a);
	request.Add_Param("password", b);
	return Unknown_BE8AD8->Slot13(&request, callback);
}

// NOT EXACT: shape (0.26: functional from the disassembly; the client inlines other string helpers)
bool RegistrationUnknown1ACClass::Unknown_7D6180(const char *a, const char *b, RegistrationUnknown478Struct *info, bool d, int callback)
{
	if (Unknown_BE8AD8 == NULL) {
		return false;
	}
	SubsRequestClass request(Unknown_BE8AC0, Unknown_BE8AB0, Unknown_BE8AA0);
	request.Add_Param("methodCall", "updateBillingInfo");
	request.Add_Param("screenname", a);
	request.Add_Param("password", b);
	request.Add_Param("send-email", d);
	request.Add_Param("first-name", info->Unknown04.c_str());
	request.Add_Param("last-name", info->Unknown14.c_str());
	request.Add_Param("address", info->Unknown24.c_str());
	request.Add_Param("address2", info->Unknown34.c_str());
	request.Add_Param("city", info->Unknown44.c_str());
	request.Add_Param("state-province", info->Unknown54.c_str());
	request.Add_Param("country", info->Unknown64.substr(0, 2).c_str());
	request.Add_Param("phone", info->Unknown84.c_str());
	request.Add_Param("postal-code", info->Unknown74.c_str());
	request.Add_Param("cc-type", info->Unknown94.Unknown00);
	request.Add_Param("cc-number", info->UnknownA8.c_str());
	request.Add_Param("expire-month", info->UnknownE8);
	request.Add_Param("expire-year", info->UnknownEC);
	if (info->UnknownB8 != "") {
		request.Add_Param("issuer", info->UnknownB8.c_str());
	}
	if (info->UnknownD8 != "" && info->UnknownC8 != "") {
		request.Add_Param("start-year", info->UnknownD8.c_str());
		request.Add_Param("start-month", info->UnknownC8.c_str());
	}
	request.Add_Param("skin", "eb");
	return Unknown_BE8AD8->Slot13(&request, callback);
}

// NOT EXACT: inline budget (0.71: the client calls the request's destructor 0x7D4D60 out of line)
bool RegistrationUnknown1ACClass::Unknown_7D6A60(const char *a, const char *b, const char *c, bool d, int callback)
{
	if (Unknown_BE8AD8 == NULL) {
		return false;
	}
	SubsRequestClass request(Unknown_BE8AC0, Unknown_BE8AB0, Unknown_BE8AA0);
	request.Add_Param("methodCall", "changePassword");
	request.Add_Param("screenname", a);
	request.Add_Param("password", b);
	request.Add_Param("new-password", c);
	request.Add_Param("send-email", d);
	request.Add_Param("skin", "eb");
	return Unknown_BE8AD8->Slot13(&request, callback);
}

bool RegistrationUnknown1ACClass::Unknown_7D6FE0(const char *a, int callback)
{
	if (Unknown_BE8AD8 == NULL) {
		return false;
	}
	SubsRequestClass request(Unknown_BE8AC0, Unknown_BE8AB0, Unknown_BE8AA0);
	request.Add_Param("methodCall", "sendPassword");
	request.Add_Param("screenname", a);
	request.Add_Param("skin", "eb");
	return Unknown_BE8AD8->Slot13(&request, callback);
}

bool RegistrationUnknown1ACClass::Unknown_7D7480(const char *a, int callback)
{
	if (Unknown_BE8AD8 == NULL) {
		return false;
	}
	SubsRequestClass request(Unknown_BE8AC0, Unknown_BE8AB0, Unknown_BE8AA0);
	request.Add_Param("methodCall", "sendScreenname");
	request.Add_Param("email", a);
	request.Add_Param("skin", "eb");
	return Unknown_BE8AD8->Slot13(&request, callback);
}

bool RegistrationUnknown1ACClass::Unknown_7D7920(const char *a, const char *b, int callback)
{
	if (Unknown_BE8AD8 == NULL) {
		return false;
	}
	SubsRequestClass request(Unknown_BE8AC0, Unknown_BE8AB0, Unknown_BE8AA0);
	request.Add_Param("methodCall", "authAOLScreenName");
	request.Add_Param("screenname", a);
	request.Add_Param("aolpassword", b);
	request.Add_Param("skin", "eb");
	return Unknown_BE8AD8->Slot13(&request, callback);
}

bool RegistrationUnknown1ACClass::Unknown_7D7E10(const char *a, const char *b, const char *c, int callback)
{
	if (Unknown_BE8AD8 == NULL) {
		return false;
	}
	SubsRequestClass request(Unknown_BE8AC0, Unknown_BE8AB0, Unknown_BE8AA0);
	request.Add_Param("methodCall", "login");
	request.Add_Param("screenname", a);
	request.Add_Param("password", b);
	if (c != NULL && strlen(c) != 0) {
		request.Add_Param("game-feature-id", c);
	}
	request.Add_Param("skin", "eb");
	return Unknown_BE8AD8->Slot13(&request, callback);
}

bool RegistrationUnknown1ACClass::Unknown_7D8380(const char *a, const char *b, const char *c, int callback)
{
	if (Unknown_BE8AD8 == NULL) {
		return false;
	}
	SubsRequestClass request(Unknown_BE8AC0, Unknown_BE8AB0, Unknown_BE8AA0);
	request.Add_Param("methodCall", "isPayingSubscriber");
	request.Add_Param("screenname", a);
	request.Add_Param("password", b);
	request.Add_Param("game-feature-id", c);
	request.Add_Param("skin", "eb");
	return Unknown_BE8AD8->Slot13(&request, callback);
}

bool RegistrationUnknown1ACClass::Unknown_7D88C0(const char *a, int callback)
{
	if (Unknown_BE8AD8 == NULL) {
		return false;
	}
	SubsRequestClass request(Unknown_BE8AC0, Unknown_BE8AB0, Unknown_BE8AA0);
	request.Add_Param("methodCall", "getPricingSelection");
	request.Add_Param("pricing-selection-id", a);
	request.Add_Param("skin", "eb");
	return Unknown_BE8AD8->Slot13(&request, callback);
}

bool RegistrationUnknown1ACClass::Unknown_7D8D60(const char *a, const char *b, const char *c, int callback)
{
	if (Unknown_BE8AD8 == NULL) {
		return false;
	}
	SubsRequestClass request(Unknown_BE8AC0, Unknown_BE8AB0, Unknown_BE8AA0);
	request.Add_Param("methodCall", "getPricingSelectionsByBundle");
	request.Add_Param("screenname", a);
	request.Add_Param("password", b);
	request.Add_Param("bundle-id", c);
	request.Add_Param("skin", "eb");
	return Unknown_BE8AD8->Slot13(&request, callback);
}

bool RegistrationUnknown1ACClass::Unknown_7D92A0(const char *a, const char *b, int callback)
{
	if (Unknown_BE8AD8 == NULL) {
		return false;
	}
	SubsRequestClass request(Unknown_BE8AC0, Unknown_BE8AB0, Unknown_BE8AA0);
	request.Add_Param("methodCall", "getPricingSelectionsByCode");
	request.Add_Param("code", a);
	request.Add_Param("country", b);
	request.Add_Param("skin", "eb");
	return Unknown_BE8AD8->Slot13(&request, callback);
}

// NOT EXACT: frame (0.60: the request's allocator bytes come from other argument slots; the optional keys tested with strlen)
bool RegistrationUnknown1ACClass::Unknown_7D9790(const char *a, const char *b, const char *c, int payment_type, const char *code, const char *gt_pricing, const char *gt_code, bool send_email, int f)
{
	if (Unknown_BE8AD8 == NULL) {
		return false;
	}
	SubsRequestClass request(Unknown_BE8AC0, Unknown_BE8AB0, Unknown_BE8AA0);
	request.Add_Param("methodCall", "applyPricingSelection");
	request.Add_Param("screenname", a);
	request.Add_Param("password", b);
	request.Add_Param("reg-pricing-selection-id", c);
	request.Add_Param("payment-type", payment_type);
	if (code != NULL && strlen(code) != 0) {
		request.Add_Param("reg-validation-code", code);
	}
	if (gt_pricing != NULL && strlen(gt_pricing) != 0) {
		request.Add_Param("gt-pricing-selection-id", gt_pricing);
	}
	if (gt_code != NULL && strlen(gt_code) != 0) {
		request.Add_Param("gt-validation-code", gt_code);
	}
	request.Add_Param("send-email", send_email);
	request.Add_Param("skin", "eb");
	return Unknown_BE8AD8->Slot13(&request, f);
}

bool RegistrationUnknown1ACClass::Unknown_7D9E20(const char *a, int callback)
{
	if (Unknown_BE8AD8 == NULL) {
		return false;
	}
	SubsRequestClass request(Unknown_BE8AC0, Unknown_BE8AB0, Unknown_BE8AA0);
	request.Add_Param("methodCall", "getActiveBundles");
	request.Add_Param("game-id", a);
	request.Add_Param("skin", "eb");
	return Unknown_BE8AD8->Slot13(&request, callback);
}

bool RegistrationUnknown1ACClass::Unknown_7DA2C0(const char *a, int callback)
{
	if (Unknown_BE8AD8 == NULL) {
		return false;
	}
	SubsRequestClass request(Unknown_BE8AC0, Unknown_BE8AB0, Unknown_BE8AA0);
	request.Add_Param("methodCall", "getAllBundlesByGame");
	request.Add_Param("game-id", a);
	request.Add_Param("skin", "eb");
	return Unknown_BE8AD8->Slot13(&request, callback);
}

// NOT EXACT: register-only (0.65: the client keeps '&' in bl, ours 0 in ebx)
bool RegistrationUnknown1ACClass::Unknown_7DA760(const char *a, const char *b, const char *c, const char *d, int callback)
{
	if (Unknown_BE8AD8 == NULL) {
		return false;
	}
	SubsRequestClass request(Unknown_BE8AC0, Unknown_BE8AB0, Unknown_BE8AA0);
	request.Add_Param("methodCall", "getSubscriptionInfo");
	request.Add_Param("screenname", a);
	request.Add_Param("password", b);
	if (c != NULL && strlen(c) != 0) {
		request.Add_Param("bundle-id", c);
	}
	if (d != NULL && strlen(d) != 0) {
		request.Add_Param("game-id", d);
	}
	request.Add_Param("skin", "eb");
	return Unknown_BE8AD8->Slot13(&request, callback);
}

// NOT EXACT: inline budget (0.71: the client calls the request's destructor 0x7D4D60 out of line)
bool RegistrationUnknown1ACClass::Unknown_7DAD50(const char *a, const char *b, const char *c, bool d, int callback)
{
	if (Unknown_BE8AD8 == NULL) {
		return false;
	}
	SubsRequestClass request(Unknown_BE8AC0, Unknown_BE8AB0, Unknown_BE8AA0);
	request.Add_Param("methodCall", "suspendEntitlement");
	request.Add_Param("screenname", a);
	request.Add_Param("password", b);
	request.Add_Param("bundle-id", c);
	request.Add_Param("send-email", d);
	request.Add_Param("skin", "eb");
	return Unknown_BE8AD8->Slot13(&request, callback);
}

// NOT EXACT: inline budget (0.71: the client calls the request's destructor 0x7D4D60 out of line)
bool RegistrationUnknown1ACClass::Unknown_7DB2D0(const char *a, const char *b, const char *c, bool d, int callback)
{
	if (Unknown_BE8AD8 == NULL) {
		return false;
	}
	SubsRequestClass request(Unknown_BE8AC0, Unknown_BE8AB0, Unknown_BE8AA0);
	request.Add_Param("methodCall", "stopBilling");
	request.Add_Param("screenname", a);
	request.Add_Param("password", b);
	request.Add_Param("bundle-id", c);
	request.Add_Param("send-email", d);
	request.Add_Param("skin", "eb");
	return Unknown_BE8AD8->Slot13(&request, callback);
}

bool RegistrationUnknown1ACClass::Unknown_7DB850(const char *a, const char *b, const char *c, int callback)
{
	if (Unknown_BE8AD8 == NULL) {
		return false;
	}
	SubsRequestClass request(Unknown_BE8AC0, Unknown_BE8AB0, Unknown_BE8AA0);
	request.Add_Param("methodCall", "getCreditCardTypeList");
	if (a != NULL) {
		request.Add_Param("screenname", a);
	}
	if (b != NULL) {
		request.Add_Param("password", b);
	}
	if (c != NULL) {
		request.Add_Param("country", c);
	}
	request.Add_Param("skin", "eb");
	return Unknown_BE8AD8->Slot13(&request, callback);
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

void SubsRequestClass::Add_Param(const char *name, const char *value)
{
	if (Query.length() && *Query.rend() != '&') {
		Query += "&";
	}
	Query += name;
	Query += "=";
	Query += value;
}

void SubsRequestClass::Add_Param(const char *name, int value)
{
	char buffer[52];
	Add_Param(name, std::string(itoa(value, buffer, 10)));
}

void SubsRequestClass::Add_Param(const char *name, bool value)
{
	Add_Param(name, std::string(value ? "1" : "0"));
}

/*
** Reconstruction notes (XR1, 2026-11-08)
**
** The object 0x7CECC0-0x7DFE70, between settings_store.cpp's (its map / string helpers end at 0x7CEC90) and the
** registration reader's (Sub14CImplClass, slots 0x7DFED0-0x7E15B0). No __FILE__ or assert: the name is PROVISIONAL
** (D137), after its first function. It opens with the globals' initialisers (0x7CECC0-0x7CEE70: the four $E pairs),
** then:
** - 0x7CEF30 (cdecl, so static) / 0x7CF190: set / clear the globals (url, page, version, the reader, the callback).
** - 0x7CF3E0-0x7D18A0: reply readers, each filling one record from the reader (slot 0x0C a string, 0x10 an int, 0x14 a
**   float at path[index]/name; 0x18 counts path's nodes): account info 0x7CF3E0 ('user-status' ... into
**   RegistrationUnknown3C4Struct), billing 0x7CF880, pricing selection 0x7CFEA0, bundle
**   0x7D0350, game feature 0x7D0610, coupon 0x7D0930, pricing option 0x7D0BE0, subscription 0x7D1060, login 0x7D1620
**   ('lkey'), credit card type 0x7D18A0 ('type').
** - 0x7D1AC0: the reply's '/status', '/message', '/url', '/@dtdversion' under the root element (slot 0x40), else
**   'NO DOCTYPE Specification.' and -1.
** - 0x7D21C0-0x7D4530: the '/subs-interface-reply/...' readers: get-acctinfo 0x7D21C0 (with the alt-name list 0x7D2690),
**   billing 0x7D28B0 (0x7D2C70), login 0x7D3000, is-paying-subscriber 0x7D33D0 (no callers: 'T' / 't'; the
**   empty ~SubsPayingStruct 0x7D3550 is its unwind's), pricing
**   selections 0x7D3570, bundles 0x7D3960, subscription info 0x7D3CC0, credit card types 0x7D4530; between them
**   ~SubsLoginStruct 0x7D3300 (0x7D3000's unwind calls it) and the records' constructors 0x7D4010 / 0x7D4370.
** - 0x7D47F0-0x7DB850: one request function per 'methodCall': a SubsRequestClass on the stack from the globals (the
**   inline std::string constructor), Add_Param per key ('skin' 'eb' last but in getBillingInfo), then the reader's slot
**   0x34 (request, callback). Optional keys: 'if (p != NULL && strlen(p) != 0)', plain NULL tests in
**   getCreditCardTypeList. The destructor's out-of-line copies are 0x7D4D60 / 0x7D4E00.
** - 0x7DBDA0 / 0x7DC0B0: SubsRequestClass's constructors (no callers); 0x7DC420 / 0x7DC6D0 / 0x7DC8F0: Add_Param,
**   called out of line where the callers' inline budget runs out (the constructor's 'version' always).
** - 0x7DCB10-0x7DFE70: the vector helpers of the readers' records.
** Add_Param's '&' test reads the byte before the string (*rend()): the client's, a bug (the reference count byte).
** The billing record (0x7CF880, 0x7D6180) is 0xF0 bytes: ui_registration.h's RegistrationUnknown478Struct (0xE8) plus
** the interface's +0x560 / +0x564 ('expire-month' / 'expire-year' at +0xE8 / +0xEC; 'start-month' / 'start-year' are
** strings at +0xC8 / +0xD8).
*/
