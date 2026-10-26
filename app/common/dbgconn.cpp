// Earth & Beyond client (reconstruction): app\common\dbgconn.cpp (PARTIAL). Notes at the end of the file.
#include "dbgconn.h"
#include "..\packets\pk.h"
#include "..\shared\unknown_debug.h"
#include "..\shared\auxdata\unknown_root.h"
#include "..\..\wwlib\ini.h"
#include <string.h>
#include <direct.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

INIClass &Get_Network_INI();	// connect.cpp 0x821B40

static char Log_Directory[256];	// 0xBE9024  // NAME 2: what it holds

// LogConnectionClass (vtable 0xB086C4)
LogConnectionClass::LogConnectionClass(ConnectionClass *connection, const char *name) :
	ForwardingConnectionClass(connection),
	File(0),
	Buffer(0)
{
	Buffer = new char[0x800];
	if (name == 0) {
		name = "netlog";
	}
	const char *directory = Get_Log_Directory();
	char path[256];
	struct stat info;
	for (int i = 0; i < 1000; i++) {
		sprintf(path, "%s\\%s-%03d.%s", directory, name, i, "log");
		if (stat(path, &info) == -1) {
			break;
		}
	}
	File = fopen(path, "w");
}

LogConnectionClass::~LogConnectionClass()
{
	if (File != 0) {
		fclose(File);
	}
	if (Buffer != 0) {
		delete [] Buffer;
	}
}

bool LogConnectionClass::Send_Message(RawPacketClass *packet)
{
	if (File != 0) {
		Describe(packet, "Sent");
		fprintf(File, "%s\n", Buffer);
		fflush(File);
	}
	return(Connection->Send_Message(packet));
}

bool LogConnectionClass::Send_Packet(PacketClass *packet)
{
	if (File != 0) {
		Describe(packet, "Sent");
		fprintf(File, "%s\n", Buffer);
		fflush(File);
	}
	return(Connection->Send_Packet(packet));
}

PacketClass *LogConnectionClass::Receive()
{
	PacketClass *packet = ForwardingConnectionClass::Receive();
	if (packet != 0 && File != 0) {
		Describe(packet, "Received");
		fprintf(File, "%s\n", Buffer);
		fflush(File);
	}
	return(packet);
}

int LogConnectionClass::Describe(PacketClass *packet, const char *label)
{
	int len = sprintf(Buffer, "%8s\t", label);
	return(len + packet->Print(Buffer + len, 0x800 - len));
}

int LogConnectionClass::Describe(RawPacketClass *packet, const char *label)
{
	return(sprintf(Buffer, "%8s\t%s", label, packet->Unknown2A));
}

const char *Get_Log_Directory()
{
	if (Log_Directory[0] == 0) {
		char path[256];
		Get_Network_INI().Get_String("Network", "LogPath", "c:\\temp\\glog", path, 255);
		for (int i = 0; i < 1000; i++) {
			sprintf(Log_Directory, "%s%03d", path, i);
			if (_mkdir(Log_Directory) == 0) goto done;
			if (errno == ENOENT) break;
		}
		strcpy(Log_Directory, "c:\\temp");
	}
done:
	return(Log_Directory);
}




// TimedLogConnectionClass (vtable 0xB0870C)
// NOT EXACT: the client inlines LogConnectionClass's constructor here and keeps it out of line (0x823F70,
// no caller); `inline` on it matches this one and loses that one.
TimedLogConnectionClass::TimedLogConnectionClass(ConnectionClass *connection, const char *name) :
	LogConnectionClass(connection, name)
{
	Start = Unknown_Get_Ticks();
}

// mm:ss.mmm (0x824A20)
int TimedLogConnectionClass::Format_Time(char *buffer, unsigned int ms)
{
	unsigned int minutes = ms / 60000; ms %= 60000; return(sprintf(buffer, "%02u:%02u.%03u", minutes, ms / 1000, ms % 1000));
}

int TimedLogConnectionClass::Describe(PacketClass *packet, const char *label)
{
	unsigned int now = Unknown_Get_Ticks();
	int len = Format_Time(Buffer, now - Start);
	len += sprintf(Buffer + len, "\t%8s\t%8u\t%4d\t", label, packet->Serial, packet->Get_Data_Size() + 4);
	len += packet->Print(Buffer + len, 0x800 - len);
	return(len + sprintf(Buffer + len, "\t time=%u", now));
}

int TimedLogConnectionClass::Describe(RawPacketClass *packet, const char *label)
{
	unsigned int now = Unknown_Get_Ticks();
	int size = packet->Header.Size; int len = Format_Time(Buffer, now - Start);
	return(len + sprintf(Buffer + len, "\t%8s\t%8u\t%4d\t%s\t time=%u", label, 0, size, packet->Unknown2A, now));
}

// RateConnectionClass (vtable 0xB08754)
RateConnectionClass::RateConnectionClass(ConnectionClass *connection) :
	ForwardingConnectionClass(connection),
	In(4),
	Out(4)
{
	unsigned int now = Unknown_Get_Ticks();
	In.Time = now;
	In.Current = 0;
	Out.Time = now;
	Out.Current = 0;
}

// NOT EXACT: the client inlines ~ForwardingConnectionClass here (inline in its connect.h; ours is in
// connect.cpp, see the notes).
RateConnectionClass::~RateConnectionClass()
{
}

bool RateConnectionClass::Send_Message(RawPacketClass *packet)
{
	Count(packet->Header.Size, Out);
	return(Connection->Send_Message(packet));
}

bool RateConnectionClass::Send_Packet(PacketClass *packet)
{
	Count(packet->Get_Data_Size() + 4, Out);
	return(Connection->Send_Packet(packet));
}

PacketClass *RateConnectionClass::Receive()
{
	PacketClass *packet = ForwardingConnectionClass::Receive();
	if (packet == 0) {
		return(0);
	}
	Count(packet->Get_Data_Size() + 4, In);
	return(packet);
}

// bytes into this second's total, after rolling each finished second into the samples
// NOT EXACT: the client stores Index before it loads Time (ours loads Time before the idiv); the slots
// that inline it (0x824CF0, 0x824D90, 0x824E20, 0x824F50) differ only there.
void RateConnectionClass::Count(int bytes, AverageClass &average)
{
	unsigned int now = Unknown_Get_Ticks();
	while (average.Time < now - 1000) {
		average.Sample[average.Index] = average.Current;
		average.Index = (average.Index + 1) % average.Count;
		average.Current = 0;
		average.Time += 1000;
	}
	average.Current += bytes;
}

bool RateConnectionClass::Get_Stats(float *out_rate, float *in_rate, float *c, float *d, float *e, float *f, int *g)
{
	if (ForwardingConnectionClass::Get_Stats(out_rate, in_rate, c, d, e, f, g)) {
		return(true);
	}
	Count(0, Out);
	Count(0, In);
	*out_rate = (float)Out.Get_Average();
	*in_rate = (float)In.Get_Average();
	return(true);
}

/*
**	Reconstruction notes (dbgconn.cpp, PARTIAL; docs/kernel-map.md 4, step 8)
**
**	* The client's functions, in order: the packet log 0x823F70-0x824450 (constructor, destructors,
**	  slots +0x04 / +0x08 / +0x0C, the two Describe overloads at +0x38 / +0x34, Get_Log_Directory),
**	  the timed packet log 0x824540-0x824A20 (Format_Time last, a member: `ret 8`), the rate wrapper
**	  0x824A90-0x825050 (average.h's two AverageClass: the constructor exact, the rest functional);
**	  not rebuilt: the statistics wrapper 0x8250B0-0x826470 (a std::map of per-packet-name counts
**	  through strtok and std::string; the constructor asserts on line 298).
**	* Forms: Get_Log_Directory's success leaves through a goto to the one return; Format_Time
**	  takes `ms %= 60000` after the minutes; the timed Describe adds the text's length to len
**	  before the last sprintf; the raw one reads Header.Size first.
**	* The destructors 0x8241B0 / 0x8247A0 have the same bytes (the client inlines
**	  ~ForwardingConnectionClass in both: it is inline in the client's connect.h, ours is in
**	  connect.cpp: moving it there makes dbgconn.cpp's copy take 0x8251A0's twin and costs connect's
**	  0x821E40), so they count as ambiguous either way.
*/
