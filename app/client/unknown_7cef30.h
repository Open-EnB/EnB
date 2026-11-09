// Earth & Beyond client (reconstruction): app\client\unknown_7cef30.h. PROVISIONAL file name (D137): the account
// server's 'subs-interface' requests and replies (the object 0x7CECC0-0x7DFE70 has no __FILE__ or assert) (XR1, 2026-11-08).
#ifndef UNKNOWN_7CEF30_H  // NAME 1 [unknown_7cef30.h]: PROVISIONAL (D137)
#define UNKNOWN_7CEF30_H

#include <string>
#include <vector>

class SubsRequestClass;

class Sub14CClass {  // NAME 1 [Sub14CClass]: vtable 0xAF7130 (0x5F03B0, _purecall, the shared 0x5F0170 / 0x5F0280 / 0x5F02A0)
public:
	virtual ~Sub14CClass() {}
	virtual void Slot1() = 0;
	virtual void Slot2() = 0;
	virtual std::string Slot3(const char *path, int index, const char *name) = 0;  // NAME 1 [Slot3, path, index, name]: slot 0x0C, the reply value at path[index]/name (0x7CF3E0's 'user-status'; 0x7E01B0)
	virtual int Slot4(const char *path, int index, const char *name) = 0;  // NAME 1 [Slot4]: slot 0x10, the reply value at path[index]/name as a number (0x7CFEA0's 'pricing-selection-type'; 0x7E0960)
	virtual float Slot5(const char *path, int index, const char *name) = 0;  // NAME 1 [Slot5]: slot 0x14, the reply value at path[index]/name as a float (0x7D0BE0's 'pricing-option-price'; 0x7E09F0)
	virtual int Slot6(const char *path) = 0;  // NAME 1 [Slot6]: slot 0x18, how many nodes path has (0x7D3570; 0x7E0A80)
	virtual void Slot7() = 0;
	virtual int Slot8(const char *path, const char *name, int index, int unknown) = 0;  // NAME 1 [Slot8]: slot 0x20, an attribute of path[index] as a number (0x7D4530's 'type'; 0x7E1200)
	virtual void Slot9() = 0;
	virtual std::string Slot10() { return std::string(""); }  // NAME 1 [Slot10]: slot 0x28, 0x5F0170
	virtual int Slot11() = 0;  // NAME 1 [Slot11]: slot 0x2C (0x5FB950 hands its result to slot 0x30)
	virtual const char *Slot12(int arg) = 0;  // NAME 1 [Slot12]: slot 0x30
	virtual bool Slot13(SubsRequestClass *request, int callback) = 0;  // NAME 1 [Slot13, request, callback]: slot 0x34, sends the request (the request functions' last call; 0x7E1290)
	virtual void Slot14() {}  // NAME 1 [Slot14]: slot 0x38, 0x5F0280
	virtual std::string Slot15() { return std::string(""); }  // NAME 1 [Slot15]: slot 0x3C, 0x5F02A0
	virtual std::string Slot16() = 0;  // NAME 1 [Slot16]: slot 0x40, the reply's root element (0x7D1AC0: "" when there is none; 0x7E15B0)
};

// The second base at +0x84 (table 0xAF7184 while constructing: destructor 0x5F00A0 / 0x5F0120, three empty slots
// 0x5F00C0 / 0x5F00E0 / 0x5F0100; the interface's table 0xAF6FF4 overrides them with 0x600000 / 0x5FB950 / 0x5FB9F0)
class RegistrationCallbackClass  // NAME 1 [RegistrationCallbackClass]: neutral (D10): the server's replies reach the screen through it
{
public:
	virtual ~RegistrationCallbackClass() {}
	virtual void Unknown_5F00C0() {}
	virtual void Unknown_5F00E0() {}
	virtual void Unknown_5F0100() {}
};

// What the reply reader 0x7D1AC0 returns (0x5FB9F0 assigns it member by member to +0x390)
struct RegistrationErrorStruct  // NAME 1 [RegistrationErrorStruct]: neutral (D10)
{
	RegistrationErrorStruct() : Unknown10(0) {}
	std::string Unknown00;		// +0x00
	short Unknown10;			// +0x10: the error number ('%s\n(%d)'; -1: none)
	std::string Unknown14;		// +0x14: the error text
	std::string Unknown24;		// +0x24
};

// The 0x5C-byte entries of the interface's vector +0x578 (0x5F9C90 shows the ones whose id list holds +0x38C)
struct RegistrationUnknown578Struct  // NAME 1 [RegistrationUnknown578Struct]: neutral (D10)
{
	RegistrationUnknown578Struct() : Unknown00(""), Unknown10(""), Unknown20(0), Unknown24(""), Unknown34(""), Unknown44(0), Unknown48(0) {}
	std::string Unknown00;		// +0x00 (0x5FA900: 0x7D9790's third string)
	std::string Unknown10;		// +0x10
	int Unknown20;				// +0x20
	std::string Unknown24;		// +0x24: the text handed to the combo box
	std::string Unknown34;		// +0x34
	int Unknown44;				// +0x44
	int Unknown48;				// +0x48: the sort key (0x60BCF0)
	std::vector<int> Unknown4C;	// +0x4C: the ids it applies to

	// 0x60BCF0 (std::sort's in 0x5FA5F0, by value: it destroys its copy)
	bool operator<(RegistrationUnknown578Struct other) const { return Unknown48 < other.Unknown48; }
};

// The account details the interface holds at +0x3C4 (0xB4 bytes; the constructor 0x5EF0B0, 0x600000's copy from
// 0x7D21C0; 0x7D4E30 reads +0x58, +0x60, +0x70, +0x7C..+0x7E, +0x90, +0x91)
struct RegistrationUnknown3C4Struct  // NAME 1 [RegistrationUnknown3C4Struct]: neutral (D10)
{
	RegistrationUnknown3C4Struct() : Unknown50(0x41), Unknown54(0), Unknown58(0), Unknown7C(0), Unknown7D(0), Unknown7E(0), Unknown90(0), Unknown91(0) {}
	std::string Unknown00[5];	// +0x00
	int Unknown50;				// +0x50 (+0x414): 'A'
	int Unknown54;				// +0x54
	int Unknown58;				// +0x58
	std::string Unknown5C;		// +0x5C
	std::string Unknown6C;		// +0x6C
	char Unknown7C;				// +0x7C: the day of the month (the constructor)
	char Unknown7D;				// +0x7D: the month (from 1)
	short Unknown7E;			// +0x7E: the year
	std::string Unknown80;		// +0x80 (+0x444 in the interface): 'US' when empty (0x5FA900)
	bool Unknown90;				// +0x90
	bool Unknown91;				// +0x91
	std::string Unknown94;		// +0x94
	std::string UnknownA4;		// +0xA4
};

// A 'bundle' (0x38 bytes; constructor 0x7D4370, read by 0x7D0350)
struct SubsBundleStruct  // NAME 1 [SubsBundleStruct]: the reply's 'bundle' elements
{
	SubsBundleStruct() : BundleId(""), BundleDescription(""), BundleName(""), BundleType(0), BundleStatus(0) {}
	~SubsBundleStruct();	// 0x60A3E0 (byte-identical copies; defined between 0x7D3960 and 0x7D3CC0: see the .cpp)
	std::string BundleId;			// +0x00 'bundle-id'
	std::string BundleDescription;	// +0x10 'bundle-description'
	std::string BundleName;			// +0x20 'bundle-name'
	int BundleType;					// +0x30 'bundle-type'
	int BundleStatus;				// +0x34 'bundle-status'
};

// A 'pricing-option' (0x84 bytes; constructor 0x7D4010, destructor 0x42C330, read by 0x7D0BE0)
struct SubsPricingOptionStruct  // NAME 1 [SubsPricingOptionStruct]: the reply's 'pricing-option' elements
{
	SubsPricingOptionStruct() : PricingOptionId(""), LinkedId(""), Type(0), BundleId(""), Name(""), Description(""), ChainDesc(""),
		Value(0), Price(0), Units(0), ExpirationDate(""), RecurringCycles(0) {}
	std::string PricingOptionId;	// +0x00 'pricing-option-id'
	std::string LinkedId;			// +0x10 'pricing-option-linked-id'
	int Type;						// +0x20 'pricing-option-type'
	std::string BundleId;			// +0x24 'bundle-id'
	std::string Name;				// +0x34 'pricing-option-name'
	std::string Description;		// +0x44 'pricing-option-description'
	std::string ChainDesc;			// +0x54 'pricing-option-chain-desc'
	int Value;						// +0x64 'pricing-option-value'
	float Price;					// +0x68 'pricing-option-price' (the reader's slot 0x14)
	int Units;						// +0x6C 'pricing-option-units'
	std::string ExpirationDate;		// +0x70 'pricing-option-expiration-date'
	int RecurringCycles;			// +0x80 'pricing-option-recurring-cycles'
};

// A 'game-feature' (0x4C bytes; destructor 0x60B040, read by 0x7D0610)
struct SubsGameFeatureStruct  // NAME 1 [SubsGameFeatureStruct]: the reply's 'game-feature' elements
{
	SubsGameFeatureStruct() : GameFeatureId(0), Type(0), Requirements(0) {}
	int GameFeatureId;				// +0x00 'game-feature-id'
	std::string GameId;				// +0x04 'game-id'
	std::string Name;				// +0x14 'game-feature-name'
	std::string Description;		// +0x24 'game-feature-description'
	int Type;						// +0x34 'game-feature-type'
	int Requirements;				// +0x38 'game-feature-requirements'
	std::string Url;				// +0x3C 'game-feature-url'
};

// A 'coupon' (0x30 bytes; destructor 0x60B2B0, read by 0x7D0930)
struct SubsCouponStruct  // NAME 1 [SubsCouponStruct]: the reply's 'coupon' elements
{
	SubsCouponStruct() : CouponId(0), Source(0), Status(0), StatusReasonCode(0) {}
	int CouponId;					// +0x00 'coupon-id'
	std::string NextBillDate;		// +0x04 'coupon-next-bill-date'
	std::string RolloverDate;		// +0x14 'coupon-rollover-date'
	int Source;						// +0x24 'coupon-source'
	int Status;						// +0x28 'coupon-status'
	int StatusReasonCode;			// +0x2C 'coupon-status-reason-code'
};

// The 0xF4-byte entries of the interface's vector +0x568: one 'subscription-info' (0x7D3CC0's list, read by 0x7D1060;
// copy constructor 0x43AAB0, destructor 0x449CC0, both out of line in an earlier object)
struct RegistrationUnknown568Struct  // NAME 1 [RegistrationUnknown568Struct]: neutral (D10)
{
	RegistrationUnknown568Struct() : EntitlementStatus(0), EntitlementStatusReason(0) {}
	int EntitlementStatus;			// +0x00 'entitlement-status'
	int EntitlementStatusReason;	// +0x04 'entitlement-status-reason'
	std::string EntitlementSuspendDate;	// +0x08 'entitlement-suspend-date'
	SubsBundleStruct Bundle;		// +0x18 '/bundle'
	SubsPricingOptionStruct PricingOption;	// +0x50 '/pricing-option'
	std::vector<SubsGameFeatureStruct> GameFeatures;	// +0xD4 'game-feature-list/game-feature'
	std::vector<SubsCouponStruct> Coupons;	// +0xE4 'coupon-list/coupon'
};

// The 0x14-byte entries of the interface's vector +0x5A8 (0x5F9A90 hands their text to the combo box 0x22): the
// 'credit-card' types 0x7D4530 reads
struct RegistrationUnknown5A8Struct  // NAME 1 [RegistrationUnknown5A8Struct]: neutral (D10)
{
	int Unknown00;				// +0x00: the 'type' attribute
	std::string Unknown04;		// +0x04: the text
};

// The 'login-reply' (0x30 bytes; 0x7D1620 reads it, 0x7D3000 returns it)
struct SubsLoginStruct  // NAME 1 [SubsLoginStruct]: the reply's 'login-reply'
{
	SubsLoginStruct() : Lkey(""), ProfileId(""), ExpirationDate("") {}
	std::string Lkey;				// +0x00 'lkey'
	std::string ProfileId;			// +0x10 'profile-id'
	std::string ExpirationDate;		// +0x20 'entitlement-expiration-date'
};

// The billing record (0xF0 bytes: the interface's +0x478..+0x567; 0x7CF880 reads it, 0x7D28B0 returns it (copy
// constructor 0x7D2C70), 0x7D6180 sends it; destructor 0x5F0CC0)
struct RegistrationUnknown478Struct  // NAME 1 [RegistrationUnknown478Struct]: neutral (D10)
{
	RegistrationUnknown478Struct() : Unknown00(0), UnknownE8(0), UnknownEC(0) {}
	int Unknown00;				// +0x00 'billing-id'
	std::string Unknown04;		// +0x04 'first-name'
	std::string Unknown14;		// +0x14 'last-name'
	std::string Unknown24;		// +0x24 'address'
	std::string Unknown34;		// +0x34 'address2'
	std::string Unknown44;		// +0x44 'city'
	std::string Unknown54;		// +0x54 'state-province'
	std::string Unknown64;		// +0x64 'country'
	std::string Unknown74;		// +0x74 'postal-code'
	std::string Unknown84;		// +0x84 'phone'
	RegistrationUnknown5A8Struct Unknown94;	// +0x94 'cc-type'
	std::string UnknownA8;		// +0xA8 'cc-number'
	std::string UnknownB8;		// +0xB8 'issuer'
	std::string UnknownC8;		// +0xC8 'start-month'
	std::string UnknownD8;		// +0xD8 'start-year'
	int UnknownE8;				// +0xE8 'expire-month'
	int UnknownEC;				// +0xEC 'expire-year'
};

// What the interface holds at +0x1AC (the function-static 0xBDA960: 0x5FB9F0 / 0x600000 ask it for the reply)
class RegistrationUnknown1ACClass  // NAME 1 [RegistrationUnknown1ACClass]: neutral (D10)
{
public:
	RegistrationUnknown1ACClass() : Unknown04(0) {}
	virtual ~RegistrationUnknown1ACClass() { Unknown_7CF190(); }	// 0x5F0680; the atexit 0x5F0660
	static void Unknown_7CF190();					// 0x7CF190 (clears its globals; the constructor's and 0x5F1660's)
	static void Unknown_7CEF30(Sub14CClass *a, const char *url, const char *page, const char *version, RegistrationCallbackClass *callback);	// 0x7CEF30 (the constructor; cdecl: static)  // NAME 1 [a, url, page, version, callback]: the [Connection] keys
	RegistrationErrorStruct Unknown_7D1AC0();	// 0x7D1AC0 (the reply's '/status', '/message', '/url')
	void Unknown_7CF3E0(RegistrationUnknown3C4Struct *info, const char *path, int index);	// 0x7CF3E0 (one 'account-info')
	RegistrationUnknown3C4Struct Unknown_7D21C0();	// 0x7D21C0 (the 'get-acctinfo' reply and its alt names; 0x600000; copy constructor 0x7D2690)
	void Unknown_7CF880(RegistrationUnknown478Struct *info, const char *path, int index);	// 0x7CF880 (one 'billing-info')
	RegistrationUnknown478Struct Unknown_7D28B0();	// 0x7D28B0 (the 'get-billing-info' reply; 0x600000)
	bool Unknown_7D6180(const char *a, const char *b, RegistrationUnknown478Struct *info, bool d, int callback);	// 0x7D6180 ('updateBillingInfo'; 0x600000)
	void Unknown_7CFEA0(RegistrationUnknown578Struct *entry, const char *path, int index);	// 0x7CFEA0 (one 'pricing-selection')  // NAME 1 [entry, path, index]: what they are
	void Unknown_7D0350(SubsBundleStruct *bundle, const char *path, int index);	// 0x7D0350 (one 'bundle')
	void Unknown_7D0610(SubsGameFeatureStruct *feature, const char *path, int index);	// 0x7D0610 (one 'game-feature')
	void Unknown_7D0930(SubsCouponStruct *coupon, const char *path, int index);	// 0x7D0930 (one 'coupon')
	void Unknown_7D0BE0(SubsPricingOptionStruct *option, const char *path, int index);	// 0x7D0BE0 (one 'pricing-option')
	void Unknown_7D18A0(RegistrationUnknown5A8Struct *entry, const char *path, int index);	// 0x7D18A0 (one 'credit-card': 'type' and the text)
	void Unknown_7D1620(SubsLoginStruct *login, const char *path, int index);	// 0x7D1620 (one 'login-reply'; no callers)
	SubsLoginStruct Unknown_7D3000();	// 0x7D3000 (the 'login-reply'; no callers)
	void Unknown_7D1060(RegistrationUnknown568Struct *info, const char *path, int index);	// 0x7D1060 (one 'subscription-info')  // NAME 1 [bundle, feature, coupon, option, info]: what they are
	std::vector<RegistrationUnknown578Struct> Unknown_7D3570();	// 0x7D3570 (0x5FA0B0, 0x5FA5F0, 0x600000)
	std::vector<SubsBundleStruct> Unknown_7D3960();	// 0x7D3960 (the 'bundle' lists; no callers)
	std::vector<RegistrationUnknown568Struct> Unknown_7D3CC0();	// 0x7D3CC0 (the 'subscription-info' list; 0x600000)
	std::vector<RegistrationUnknown5A8Struct> Unknown_7D4530();	// 0x7D4530 (the 'credit-card' types; 0x600000)
	bool Unknown_7D47F0(const char *a, const char *b, const char *c, int callback);	// 0x7D47F0 ('getAcctInfo')
	bool Unknown_7D5CE0(const char *a, const char *b, int callback);	// 0x7D5CE0 ('getBillingInfo')
	bool Unknown_7D6A60(const char *a, const char *b, const char *c, bool d, int callback);	// 0x7D6A60 ('changePassword')
	bool Unknown_7D6FE0(const char *a, int callback);	// 0x7D6FE0 ('sendPassword')
	bool Unknown_7D7480(const char *a, int callback);	// 0x7D7480 ('sendScreenname')
	bool Unknown_7D7920(const char *a, const char *b, int callback);	// 0x7D7920 ('authAOLScreenName')
	bool Unknown_7D7E10(const char *a, const char *b, const char *c, int callback);	// 0x7D7E10 ('login')
	bool Unknown_7D8380(const char *a, const char *b, const char *c, int callback);	// 0x7D8380 ('isPayingSubscriber')
	bool Unknown_7D88C0(const char *a, int callback);	// 0x7D88C0 ('getPricingSelection')
	bool Unknown_7D8D60(const char *a, const char *b, const char *c, int callback);	// 0x7D8D60 ('getPricingSelectionsByBundle')
	bool Unknown_7D92A0(const char *a, const char *b, int callback);	// 0x7D92A0 ('getPricingSelectionsByCode')
	bool Unknown_7D9E20(const char *a, int callback);	// 0x7D9E20 ('getActiveBundles')
	bool Unknown_7DA2C0(const char *a, int callback);	// 0x7DA2C0 ('getAllBundlesByGame')
	bool Unknown_7DA760(const char *a, const char *b, const char *c, const char *d, int callback);	// 0x7DA760 ('getSubscriptionInfo')
	bool Unknown_7DAD50(const char *a, const char *b, const char *c, bool d, int callback);	// 0x7DAD50 ('suspendEntitlement')
	bool Unknown_7DB2D0(const char *a, const char *b, const char *c, bool d, int callback);	// 0x7DB2D0 ('stopBilling')
	bool Unknown_7DB850(const char *a, const char *b, const char *c, int callback);	// 0x7DB850 ('getCreditCardTypeList')  // NAME 1 [a, b, c, d, callback]: neutral (the keys they fill)
	bool Unknown_7D4E30(const char *a, const char *b, RegistrationUnknown3C4Struct *info, const char *c, bool d, int e, int f);	// 0x7D4E30 ('addAcctInfo')  // NAME 1 [a, b, info, c, d, e, f]: neutral
	bool Unknown_7D5600(const char *a, const char *b, RegistrationUnknown3C4Struct *info, bool d, int e);	// 0x7D5600  // NAME 1 [a, b, info, d, e]: neutral
	bool Unknown_7D9790(const char *a, const char *b, const char *c, int payment_type, const char *code, const char *gt_pricing, const char *gt_code, bool send_email, int f);	// 0x7D9790 ('payment_type' itoa, 'gt_pricing_selection_id', 'gt_validation_code', 'send_email')  // NAME 1 [a, b, c, payment_type, code, gt_pricing, gt_code, send_email, f]: the request keys they fill

	char Unknown04;
};

// One request to the account server (0x28 bytes, table 0xB073AC: the destructor 0x7D4D60 / 0x7D4E00; built on the
// stack by each request function, handed to the reader's slot 0x34)
class SubsRequestClass  // NAME 1 [SubsRequestClass]: the 'subs-interface' replies and 'methodCall' keys
{
public:
	SubsRequestClass(const char *url, const char *page, const char *version, bool a = true, bool b = false);	// 0x7DBDA0  // NAME 1 [url, page, version, a, b]: 0x7CEF30's arguments
	SubsRequestClass(const std::string &url, const std::string &page, const std::string &version, bool a = true, bool b = false);	// 0x7DC0B0 (inline: the request functions expand it)
	virtual ~SubsRequestClass() {}	// 0x7D4D60

	void Add_Param(const char *name, const char *value);	// 0x7DC420 ('&' between, '=')  // NAME 1 [Add_Param, name, value]: what it does
	void Add_Param(const char *name, int value);			// 0x7DC6D0 (itoa)
	void Add_Param(const char *name, bool value);			// 0x7DC8F0 ('1' / '0')

	bool Unknown04;			// +0x04
	bool Unknown05;			// +0x05
	std::string Url;		// +0x08: url '/' page  // NAME 1 [Url]: what it holds
	std::string Query;		// +0x18: name=value&...  // NAME 1 [Query]: what it holds
};

// The interface's globals (0x7CEF30 sets them, 0x7CF190 clears them)
extern std::string Unknown_BE8AA0;	// NAME 1: neutral (D10): the version
extern std::string Unknown_BE8AB0;	// NAME 1: neutral (D10): the page
extern std::string Unknown_BE8AC0;	// NAME 1: neutral (D10): the url
extern int Unknown_BE8AD4;			// NAME 1: neutral (D10): the callback (the request functions' last argument)
extern Sub14CClass *Unknown_BE8AD8;	// NAME 1: neutral (D10): the reader (slot 0x0C reads the reply, slot 0x34 sends)

#endif // UNKNOWN_7CEF30_H
