// Earth & Beyond client (reconstruction)
/*
**	PROVISIONAL file name (no assert or __FILE__ names it): AuthLoginResponseClass, the answer
**	of EA.Com's login component (unknown_authlogin.h), client 0x792040-0x792310 in the
**	unattributed run after nlp.cpp (docs/kernel-map-step9.md 4).
*/



#include "unknown_authlogin.h"
#include <stdio.h>

// The texts of the auth codes (0xB9DC90; Get_Error_Text 0x7922B0 searches it)
struct AuthLoginErrorStruct	// NAME 1: what it holds
{
	int Code;
	const char *Text;
};

static const char *AuthLoginUnknownError = "Unknown Error";	// 0xB9DC88  // NAME 1: its text

static AuthLoginErrorStruct AuthLoginErrors[] = {	// 0xB9DC90  // NAME 1: what it holds
	{ 12, "Internal failure (bad client build or null service ID provided)" },
	{ 20, "Please verify that your user name and password have been correctly entered" },
	{ 110, "Your password did not match the user name specified. Please try again" },
	{ 111, "Unknown Error" },
	{ 112, "Not Subscribed To Product" },
	{ 120, "Not Subscribed To Product" },
	{ 121, "Subscription Suspended or Canceled" },
	{ 122, "No Reg Code Entered" },
	{ 123, "Registration Code Disabled" },
	{ 150, "Account Suspended or Inactive" },
	{ 160, "Permanent internal failure (no such game service configured or service cannot be retrieved)" },
	{ 170, "Transient internal failure (ticket could not be created)" },
	{ 180, "Transient internal failure (customer could not be loaded)" },
	{ 200, "Internet game room IP address concurrency limit hit. The game room provider has exceeded the number of simultaneous players for which they have paid." },
	{ 201, "Internet game room time limit hit. The game room provider has exceeded the number of game time minutes for which they have paid." },
	{ 302, "Unspecified internal auth/auth/ident client error" },
	{ 300, "Cannot contact EA.com" },
	{ 301, "EA.com is temporarily unavailable. This may be due to routine server maintenance or other network issues. Please try again later." },
	{ 0, NULL }
};

// 0x792040
AuthLoginResponseClass::AuthLoginResponseClass(IAuthLoginResultClass *result) :
	Success(false)
{
	if (result != NULL) {
		Success = result->Is_Success() != 0;
		Message = result->Get_Message() ? result->Get_Message() : "";
		Unknown28 = result->Get_Unknown28() ? result->Get_Unknown28() : "";
		Ticket = result->Get_Ticket() ? result->Get_Ticket() : "";
		sscanf(result->Get_Code(), "%*4c%d", &Code);
	}
}

// 0x7922B0
const char *AuthLoginResponseClass::Get_Error_Text()
{
	AuthLoginErrorStruct *entry;
	for (entry = AuthLoginErrors; entry->Text != NULL; entry++) {
		if (entry->Code == Code) {
			break;
		}
	}
	const char *text = entry->Text;
	if (text == NULL) {
		text = AuthLoginUnknownError;
	}
	return text;
}

/*
**	Reconstruction notes
**
**	The constructor copies the result's strings through the inline std::string assignment; a
**	missing string becomes "" (the client's pooled empty string 0xBDA908). The destructor is
**	implicit (out of line at 0x76AC60 in tsk_login.cpp).
*/
