// Earth & Beyond client (reconstruction): app\packets\pk.cpp (PARTIAL). Notes at the end of the file.
#include "pk.h"
#include "..\shared\unknown_debug.h"
#include "..\..\wwlib\nstrdup.h"
#include <string.h>
#include <winsock.h>

unsigned __int64 Unknown_8B4510(unsigned __int64 value);	// NAME 1 [Unknown_8B4510]: wwonline's 64-bit htonl (a byte-order probe, then two htonl)
unsigned __int64 Unknown_8B4560(unsigned __int64 value);	// NAME 1 [Unknown_8B4560]: wwonline's 64-bit ntohl


int RawPacketClass::Add(const void *data, int len)
{
	return(Data.Write(data, len));
}


void RawPacketClass::Add(Unknown_PacketDataClass &object)
{
	object.Write(&Data);
}


void RawPacketClass::Add(const __int64 &value)
{
	__int64 net = Unknown_8B4510(value);
	Data.Write(&net, 8);
}


void RawPacketClass::Add(const int &value)
{
	int net = htonl(value);
	Data.Write(&net, 4);
}


void RawPacketClass::Add(const short &value)
{
	short net = htons(value);
	Data.Write(&net, 2);
}


void RawPacketClass::Add(const bool &value)
{
	bool net = htonl(value) != 0;
	Data.Write(&net, 1);
}


void RawPacketClass::Add(const float &value)
{
	unsigned long net = htonl(*(const unsigned long *)&value);
	Data.Write(&net, 4);
}


void RawPacketClass::Add_String(const char *string)
{
	Data.Write_String(string);
}

// (spacing kept: the next assert is on the client's line 1135)












































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































void RawPacketClass::Add(const char *str)
{
	G_ASSERT(str);
	int len = strlen(str) + 1;
	Data.Write(&len, 4);
	Data.Write(str, len);
}


void RawPacketClass::Add(const std::string &s)
{
	unsigned int slen = s.length();
	FAIL_IF(slen > 0xFFFF) slen = 0xFFFF;
	unsigned short length = (unsigned short)slen;
	Data.Write(&length, 2);
	Data.Write(s.c_str(), length);
}



void RawPacketClass::Get(std::string *s)
{
	FAIL_IF(!s) return;
	unsigned int len = 0;
	Data.Read(&len, 2);
	char *buffer = new char[(unsigned short)len + 1];
	Data.Read(buffer, (unsigned short)len);
	buffer[(unsigned short)len] = 0;
	*s = buffer;
	delete buffer;
}

int RawPacketClass::Get_Size(const std::string &s)
{
	unsigned int length = s.length();
	FAIL_IF(length > 0xFFFF) length = 0xFFFF;
	return(length + 2);
}

int RawPacketClass::Get(void *buffer, int len)
{
	return(Data.Read(buffer, len));
}

void RawPacketClass::Get(Unknown_PacketDataClass *object)
{
	if (object) {
		object->Read(&Data);
	}
}

void RawPacketClass::Get(Unknown_PacketDataClass &object)
{
	object.Read(&Data);
}

void RawPacketClass::Get(__int64 &value)
{
	Data.Read(&value, 8);
	value = Unknown_8B4560(value);
}

void RawPacketClass::Get(int &value)
{
	Data.Read(&value, 4);
	value = ntohl(value);
}

void RawPacketClass::Get(short &value)
{
	Data.Read(&value, 2);
	value = ntohs(value);
}

void RawPacketClass::Get(bool &value)
{
	Data.Read(&value, 1);
	value = ntohl(value) != 0;
}

void RawPacketClass::Get(float &value)
{
	unsigned long net;
	Data.Read(&net, 4);
	net = ntohl(net);
	value = *(float *)&net;
}

char *RawPacketClass::Get_String()
{
	return(Data.Read_String());
}
// (spacing kept: the next assert is on the client's line 1233)





void RawPacketClass::Get(char **str)
{
	FAIL_IF(!str) return;
	*str = 0;
	int len = 0;
	// (spacing kept: the next FAIL_IF is on the client's line 1245)







	if (Data.Read(&len, 4) == 4) {
		FAIL_IF(len <= 0) return;
		*str = new char[len];


		FAIL_IF(!*str) {
			*str = nstrdup("");
			return;
		}
		Data.Read(*str, len);
	}
}


int RawPacketClass::Get_Size(const char *str)
{
	return(strlen(str) + 1);
}

// (spacing kept: the next assert is on the client's line 1344)














































































int PacketClass::Unknown_14(int a, int b)
{
	G_ASSERT(0);
	return(0);
}


/*
**	Reconstruction notes (pk.cpp, PARTIAL)
**
**	* Rebuilt: the raw message's field helpers 0x86E870-0x86EFE0 (all exact) and the base's slot
**	  +0x14 default 0x86F010 (assert line 1344). The numbers go in network byte order (htonl / htons,
**	  wwonline's 64-bit pair 0x8B4510 / 0x8B4560); a bool through htonl and back (`!= 0`), a float
**	  as its bits.
**	* Get(char **) falls back to nstrdup of the global 0xBDA908 (here ""; the operand is a
**	  relocation) when new fails.
**	* Not rebuilt: the factory 0x8696C0 and its switch 0x869790 (lines to 1078: one case per opcode,
**	  259 packet classes), the small packet classes 0x86E5B0-0x86E850 ('FireEquipped',
**	  'PriorityMessage', 'QueueMessage', 'PushMessage'), and the static after 0x86F010.
*/
