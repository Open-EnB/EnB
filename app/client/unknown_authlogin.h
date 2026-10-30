// Earth & Beyond client (reconstruction)
/*
**	PROVISIONAL file name (no assert or __FILE__ names it): EA.Com's login component as the client
**	uses it (docs/kernel-map-step9.md 4). authlogin.dll exports AuthLoginLib_GetAuthorize and
**	AuthLoginLib_GetComponentFactory; the client imports only the factory (tools/vc6/authlogin.def,
**	docs/toolchain.md 2). EA's published source has no kcom: the interfaces are declared as far as
**	LoginTaskClass::Fetch_EA_Ticket 0x76A7B0 and AuthLoginResponseClass 0x792040 call them, C++
**	virtuals (thiscall) with COM's Release at +0x08.
**
**	AuthLoginResponseClass is rebuilt in unknown_authlogin.cpp (client 0x792040-0x792310, in the
**	unattributed run after nlp.cpp).
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef UNKNOWN_AUTHLOGIN_H  // NAME 1 [unknown_authlogin.h]: neutral file name (D10)
#define UNKNOWN_AUTHLOGIN_H

#include <string>

namespace kcom	// NAME 5: the client's assert 'iauth && kcom::ResultSucceeded(kr)'
{
	typedef long Result;								// NAME 2: the assert's kr; tested >= 0
	inline bool ResultSucceeded(Result result) { return result >= 0; }	// NAME 5: the assert

	class IUnknownClass	// NAME 1: the COM-style base (+0x08 Release)
	{
	public:
		virtual void Unknown_00();
		virtual void Unknown_04();
		virtual void Release();							// +0x08 NAME 2: COM's slot
	};

	class IComponentFactoryClass : public IUnknownClass	// NAME 2: authFactory (the assert)
	{
	public:
		virtual void Unknown_0C();
		virtual void Unknown_10();
		virtual void Unknown_14();
		virtual void Unknown_18();
		virtual IUnknownClass *Create(int id, Result *result);	// +0x1C (id 0x67: the auth interface)  // NAME 1: what it does
	};
}

// What IAuthLoginClass::Login returns (AuthLoginResponseClass's constructor reads it)
class IAuthLoginResultClass : public kcom::IUnknownClass	// NAME 1: neutral (D10)
{
public:
	virtual void Unknown_0C();
	virtual void Unknown_10();
	virtual void Unknown_14();
	virtual void Unknown_18();
	virtual char Is_Success();							// +0x1C NAME 1: AuthLoginResponseClass's Success
	virtual const char *Get_Code();						// +0x20 NAME 1: scanned '%*4c%d' for the code
	virtual const char *Get_Message();					// +0x24 NAME 1: the response's Message
	virtual const char *Get_Unknown28();				// +0x28 NAME 1: neutral (D10)
	virtual const char *Get_Ticket();					// +0x2C NAME 1: the response's Ticket
};

class IAuthLoginClass : public kcom::IUnknownClass	// NAME 1: iauth (the assert)
{
public:
	virtual void Unknown_0C();
	virtual void Unknown_10();
	virtual void Unknown_14();
	virtual void Unknown_18();
	virtual void Unknown_1C();
	virtual void Unknown_20();
	virtual void Unknown_24();
	virtual IAuthLoginResultClass *Login(const char *name, const char *password, const char *product, const char *base, const char *url, int unknown);	// +0x28  // NAME 1: what it does
};

extern "C" __declspec(dllimport) kcom::IComponentFactoryClass *AuthLoginLib_GetComponentFactory();	// authlogin.dll (docs/binary.md)

// The login's answer (0x38 bytes; constructor 0x792040 from Login's result, destructor implicit: 0x76AC60)
class AuthLoginResponseClass	// NAME 1: the assert's 'response'
{
public:
	AuthLoginResponseClass(IAuthLoginResultClass *result);	// 0x792040
	const char *Get_Error_Text();						// 0x7922B0: the code as text  // NAME 1: what it does
	const char *Get_Ticket() { return Ticket.c_str(); }	// NAME 5: the assert 'strlen(response.Get_Ticket())'
	const char *Get_Message() { return Message.c_str(); }	// NAME 1: what it returns

	std::string Message;								// +0x00 NAME 1: shown when the login fails
	std::string Ticket;									// +0x10 NAME 2: Get_Ticket's
	int Code;											// +0x20 NAME 1: 0x89 the demo's end
	bool Success;										// +0x24 NAME 1: the ticket is taken when set
	std::string Unknown28;								// +0x28
};

#endif
