// Earth & Beyond client (reconstruction): app\packets\pkglobalerror.cpp. Notes at the end of the file.
#include "pkglobalerror.h"
#include <stdio.h>

// 0xBB25F0: the texts of the global server's errors, by Error  // NAME 1: what it holds
char *GlobalErrorStrings[] = {
	"Success",
	"Error: Customer name doesn't exist in the database.",
	"Error: Your password was incorrect.",
	"Error: This customer account is already in use.",
	"Error: Registration failed.",
	"Error: This galaxy is not available, please try again later.",
	"Error: This operation cannot be perfomed on this slot.",
	"Error: You have been temporarily banned.",
	"Sorry, that name has already been taken. Please try again.",
	"Sorry, your name can only contain the letters a-z. No spaces or other special characters are allowed.  Please try again.",
	"Sorry, this name is too short. It must contain at least 3 characters. Please try again.",
	"Sorry, this name needs enough vowels (a,e,i,o,u & y) to be pronouncable. Please try again.",
	"Sorry, there are too many repeating characters in this name. Please try again.",
	"Sorry, this name contains a reserved or illegal word. Please try again.",
	"Error: Ticket Validation Failed.",
	"Error: Authentication Server (AUTHD) is unavailable.  Try again in a few minutes.",
	"Error: Unknown error.",
	"Sorry, that ship name is not allowed. Please try again",
};

// Message is assigned in the body: the client stores the vtable before the string's _Grow
GlobalErrorPacketClass::GlobalErrorPacketClass(int unknown0c, int error, const char *message) :
	Unknown0C(unknown0c),
	Error(error)
{
	Message = "";
	if (message == NULL) {
		if (error < 18) {
			char *text = GlobalErrorStrings[error];
			if (text != NULL) {
				Message = text;
			}
		}
	} else {
		Message = message;
	}
}

GlobalErrorPacketClass::GlobalErrorPacketClass(RawPacketClass *raw)
{
	int value;
	raw->Get(value);
	Unknown0C = value;
	int error;
	raw->Get(error);
	Error = error;
	char *string = raw->Get_String();
	Message = string;
	delete [] string;
}

RawPacketClass *GlobalErrorPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(117, Get_Data_Size());
	int value = Unknown0C;
	raw->Add(value);
	int error = Error;
	raw->Add(error);
	raw->Add_String(Message.c_str());
	return(raw);
}

int GlobalErrorPacketClass::Get_Data_Size()
{
	return(Message.length() + 1 + sizeof(Unknown0C) + sizeof(Error));
}

int GlobalErrorPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s", "Global Error"));
}


/*
**	Reconstruction notes (pkglobalerror.cpp)
**
**	* Client 0x864A10-0x864FF0 (0x8649E0 is the previous object's guarded static, 0xC416FC; this one's is
**	  0x864FF0): opcode 117, written by hand (builder P5, 2026-10-27), the value constructor first. Its
**	  handler (Process_Packet 0x76B4A0) is tsk_login.cpp's, which used a local view of this class before.
**	* The file name is provisional (no assert names it).
*/
