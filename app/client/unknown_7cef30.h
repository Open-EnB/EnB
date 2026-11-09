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
	virtual void Slot4() = 0;
	virtual void Slot5() = 0;
	virtual void Slot6() = 0;
	virtual void Slot7() = 0;
	virtual void Slot8() = 0;
	virtual void Slot9() = 0;
	virtual std::string Slot10() { return std::string(""); }  // NAME 1 [Slot10]: slot 0x28, 0x5F0170
	virtual int Slot11() = 0;  // NAME 1 [Slot11]: slot 0x2C (0x5FB950 hands its result to slot 0x30)
	virtual const char *Slot12(int arg) = 0;  // NAME 1 [Slot12]: slot 0x30
	virtual bool Slot13(SubsRequestClass *request, int callback) = 0;  // NAME 1 [Slot13, request, callback]: slot 0x34, sends the request (the request functions' last call; 0x7E1290)
	virtual void Slot14() {}  // NAME 1 [Slot14]: slot 0x38, 0x5F0280
	virtual std::string Slot15() { return std::string(""); }  // NAME 1 [Slot15]: slot 0x3C, 0x5F02A0
	virtual void Slot16() = 0;
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

// What the interface holds at +0x1AC (the function-static 0xBDA960: 0x5FB9F0 / 0x600000 ask it for the reply)
class RegistrationUnknown1ACClass  // NAME 1 [RegistrationUnknown1ACClass]: neutral (D10)
{
public:
	RegistrationUnknown1ACClass() : Unknown04(0) {}
	virtual ~RegistrationUnknown1ACClass() { Unknown_7CF190(); }	// 0x5F0680; the atexit 0x5F0660
	static void Unknown_7CF190();					// 0x7CF190 (clears its globals; the constructor's and 0x5F1660's)
	static void Unknown_7CEF30(Sub14CClass *a, const char *url, const char *page, const char *version, RegistrationCallbackClass *callback);	// 0x7CEF30 (the constructor; cdecl: static)  // NAME 1 [a, url, page, version, callback]: the [Connection] keys
	RegistrationErrorStruct Unknown_7D1AC0();	// 0x7D1AC0 (the reply's '/status', '/message', '/url')
	std::vector<RegistrationUnknown578Struct> Unknown_7D3570();	// 0x7D3570 (0x5FA0B0, 0x5FA5F0, 0x600000)
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
	SubsRequestClass(const std::string &url, const std::string &page, const std::string &version, bool a = true, bool b = false);	// 0x7DC0B0
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
