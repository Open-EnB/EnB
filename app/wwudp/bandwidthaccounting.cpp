/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\wwudp\bandwidthaccounting.cpp (client 0x84C720-0x84DFB0)
**
**	The bandwidth accounting: a kernel task (vtable 0xB08CF0, every 2 s) over per-address byte and
**	datagram counts that wnet's UDP feeds through its accounting callback (UDP::Write and Read call
**	Record with every datagram, 28 bytes of IP and UDP header added), printed on two of E&B's debug
**	windows ('Bandwidth Accounting', 'Bandwidth Accounting Per IP'). The client never constructs it
**	(no caller of the constructor 0x84C760) and never sets the switch Enabled (0xBF40A8): it is
**	linked, but idle. The per-address table is WWLib's DynamicVectorClass of 0x3C-byte entries.
**	The debug window calls sit on the client's lines (281-282, 310-361: D25).
*/

#include "..\kernel\kernel.h"
#include "udp.h"
#include "vector.h"
#include <stdlib.h>
#include <string.h>
#include <winsock.h>

// E&B's debug window print (0x4BC460, cdecl): the window, E&B's log object (0xBDA908), the file
// and line, the column and row, then printf's. Not rebuilt: the program defines it.
class UnknownLogClass;		// NAME 1 [UnknownLogClass]: placeholder for E&B's log object (cordmgr.cpp)
extern UnknownLogClass UnknownLog;	// NAME 1 [UnknownLog]: 0xBDA908
void Unknown_4BC460(const char *window, UnknownLogClass *log, const char *file, int line, int x, int y,
	const char *format, ...);	// NAME 1 [Unknown_4BC460]: the debug window print

unsigned int Unknown_Get_Ticks();	// the aux-data clock 0x4A3B40 (unknown_root.h)

// One address's counts (0x3C bytes).
struct BandwidthEntryClass		// NAME 2: describes what it holds
{
	bool operator == (BandwidthEntryClass const &other) const { return memcmp(this, &other, sizeof(*this)) == 0; }	// 0x84D960
	bool operator != (BandwidthEntryClass const &other) const { return memcmp(this, &other, sizeof(*this)) != 0; }	// 0x84D990

	unsigned int Ip;			// NAME 2: +0x00 (host order)
	unsigned short Port;		// NAME 2: +0x04
	unsigned int BytesIn;		// NAME 2: +0x08 since the last Run
	unsigned int BytesOut;		// NAME 2: +0x0C
	unsigned int ExtraIn;		// NAME 1: +0x10 Record_In's third argument (0 from the callback)
	unsigned int ExtraOut;		// NAME 1: +0x14
	unsigned int BwIn;			// NAME 2: +0x18 bits per second
	unsigned int BwOut;			// NAME 2: +0x1C
	unsigned int ExtraBwIn;		// NAME 1: +0x20
	unsigned int ExtraBwOut;	// NAME 1: +0x24
	unsigned int PacketsIn;		// NAME 2: +0x28 since the last Run
	unsigned int PacketsOut;	// NAME 2: +0x2C
	unsigned int PpsIn;			// NAME 2: +0x30 datagrams per second
	unsigned int PpsOut;		// NAME 2: +0x34
	int Idle;					// NAME 2: +0x38 runs without a datagram (dropped after 10)
};

class BandwidthAccountingClass : public TaskClass	// NAME 2: after its windows' titles
{
public:
	BandwidthAccountingClass();							// 0x84C760
	virtual ~BandwidthAccountingClass();				// 0x84C830
	virtual void Run();									// 0x84C970
	virtual int Get_Interval() { return 2000; }			// 0x84C950
	virtual bool Delete_On_Remove() { return false; }	// 0x84C7E0

	static void Record(bool out, int len, unsigned int ip, unsigned short port);					// 0x84C8B0  // NAME 2: what it does
	static void Print_Address(int index, int row);												// 0x84CB80  // NAME 2: what it does
	static void Print();																		// 0x84CC50  // NAME 2: what it does
	static int Compare(void const *a, void const *b);											// 0x84D150  // NAME 2: what it does
	static void Sort();																			// 0x84D180  // NAME 2: what it does
	static void Reset();																		// 0x84D1C0  // NAME 2: what it does
	static int Find(unsigned int ip, unsigned short port, bool add);							// 0x84D240  // NAME 2: what it does
	static void Record_In(unsigned int ip, unsigned short port, int extra, unsigned int bytes);	// 0x84D380  // NAME 2: what it does
	static void Record_Out(unsigned int ip, unsigned short port, int extra, unsigned int bytes);	// 0x84D510  // NAME 2: what it does
	static void Compute_Rates(unsigned int msec);														// 0x84D730  // NAME 2: what it does

	static DynamicVectorClass<BandwidthEntryClass> Entries;	// NAME 2: 0xBF4048
	static unsigned int BytesOut;			// NAME 2: 0xBF4060
	static unsigned int BytesIn;			// NAME 2: 0xBF4064
	static unsigned int PacketsOut;			// NAME 2: 0xBF4068
	static unsigned int PacketsIn;			// NAME 2: 0xBF406C
	static unsigned int BwOut;				// NAME 2: 0xBF4070
	static unsigned int BwIn;				// NAME 2: 0xBF4074
	static unsigned int PpsOut;				// NAME 2: 0xBF4078
	static unsigned int PpsIn;				// NAME 2: 0xBF407C
	static unsigned int LastTime;			// NAME 2: 0xBF4080
	static int PeakOut;						// NAME 2: 0xBF4084
	static unsigned int PeakOutPerAddr;		// NAME 2: 0xBF4088
	static int PeakIn;						// NAME 2: 0xBF408C
	static unsigned int PeakInPerAddr;		// NAME 2: 0xBF4090
	static int Sizes[5];					// NAME 2: 0xBF4094 datagrams out by size: <= 50, < 100, < 250, < 576, larger
	static bool Enabled;					// NAME 2: 0xBF40A8 (never set in the client)
	static unsigned int LastInIp;			// NAME 2: 0xBF40AC
	static unsigned short LastInPort;		// NAME 2: 0xBF40B0
	static unsigned int LastOutIp;			// NAME 2: 0xBF40B4
	static unsigned short LastOutPort;		// NAME 2: 0xBF40B8
	static bool ForgetIn;					// NAME 2: 0xBB13E8 the cached index is stale
	static bool ForgetOut;					// NAME 2: 0xBB13E9
	static int SortCountdown;				// NAME 2: 0xBB13EC
	static int LastInIndex;					// NAME 2: 0xBB13F0
	static int LastOutIndex;				// NAME 2: 0xBB13F4
};

DynamicVectorClass<BandwidthEntryClass> BandwidthAccountingClass::Entries;
unsigned int BandwidthAccountingClass::BytesOut;
unsigned int BandwidthAccountingClass::BytesIn;
unsigned int BandwidthAccountingClass::PacketsOut;
unsigned int BandwidthAccountingClass::PacketsIn;
unsigned int BandwidthAccountingClass::BwOut;
unsigned int BandwidthAccountingClass::BwIn;
unsigned int BandwidthAccountingClass::PpsOut;
unsigned int BandwidthAccountingClass::PpsIn;
unsigned int BandwidthAccountingClass::LastTime;
int BandwidthAccountingClass::PeakOut;
unsigned int BandwidthAccountingClass::PeakOutPerAddr;
int BandwidthAccountingClass::PeakIn;
unsigned int BandwidthAccountingClass::PeakInPerAddr;
int BandwidthAccountingClass::Sizes[5];
bool BandwidthAccountingClass::Enabled;
unsigned int BandwidthAccountingClass::LastInIp;
unsigned short BandwidthAccountingClass::LastInPort;
unsigned int BandwidthAccountingClass::LastOutIp;
unsigned short BandwidthAccountingClass::LastOutPort;
bool BandwidthAccountingClass::ForgetIn = true;
bool BandwidthAccountingClass::ForgetOut = true;
int BandwidthAccountingClass::SortCountdown = 15;
int BandwidthAccountingClass::LastInIndex = -1;
int BandwidthAccountingClass::LastOutIndex = -1;

static const char WINDOW[] = "Bandwidth Accounting";
static const char WINDOW_IP[] = "Bandwidth Accounting Per IP";

// 0x84C760: counts from now on and runs as a background task.
BandwidthAccountingClass::BandwidthAccountingClass()
{
	UDP::Set_Accounting_Callback(Record);
	KernelClass::Get_Instance()->Add_Task(this);
}

// 0x84C830
BandwidthAccountingClass::~BandwidthAccountingClass()
{
	KernelClass::Get_Instance()->Remove_Task(this);
	UDP::Clear_Accounting_Callback();
}

// 0x84C8B0: UDP::Write (out) and Read call it with every datagram.
void BandwidthAccountingClass::Record(bool out, int len, unsigned int ip, unsigned short port)
{
	if (len == 0 || !Enabled) {
		return;
	}
	len += 28;
	if (out) {
		BytesOut += len;
		PacketsOut++;
		Record_Out(ip, port, 0, len);
	} else {
		BytesIn += len;
		PacketsIn++;
		Record_In(ip, port, 0, len);
	}
}

// 0x84C970: the rates since the last run, the windows; addresses idle for 10 runs are dropped.
void BandwidthAccountingClass::Run()
{
	unsigned int now = Unknown_Get_Ticks();
	if (LastTime != 0) {
		unsigned int msec = now - LastTime;
		BwOut = BytesOut * 1000 / msec;
		BwIn = BytesIn * 1000 / msec;
		if (BwOut > (unsigned int)PeakOut) {
			PeakOut = BwOut;
		}
		if (BwIn > (unsigned int)PeakIn) {
			PeakIn = BwIn;
		}
		PpsOut = PacketsOut * 1000 / msec;
		PpsIn = PacketsIn * 1000 / msec;
		Compute_Rates(msec);
		Print();
	} else {
		Compute_Rates(2000);
		PeakOutPerAddr = 0;
		PeakInPerAddr = 0;
	}
	LastTime = now;
	PacketsOut = 0;
	PacketsIn = 0;
	BytesOut = 0;
	BytesIn = 0;
	Sizes[0] = 0;
	Sizes[1] = 0;
	Sizes[2] = 0;
	Sizes[3] = 0;
	Sizes[4] = 0;
	for (int i = Entries.Count() - 1; i >= 0; i--) {
		Entries[i].Idle++;
		if (Entries[i].Idle > 10) {
			Entries.Delete(i);
			ForgetIn = true;
			ForgetOut = true;
		}
	}
}

































































// 0x84CB80: one address's line on the per-address window.
void BandwidthAccountingClass::Print_Address(int index, int row)
{
	in_addr ip;
	ip.s_addr = htonl(Entries[index].Ip);
	int port = Entries[index].Port;
	unsigned int bw_out = Entries[index].BwOut;
	unsigned int bw_in = Entries[index].BwIn;
	unsigned int pps_out = Entries[index].PpsOut;
	unsigned int pps_in = Entries[index].PpsIn;
	Unknown_4BC460(WINDOW_IP, &UnknownLog, __FILE__, __LINE__, 0, row, "%s : %d      ", inet_ntoa(ip), port);
	Unknown_4BC460(WINDOW_IP, &UnknownLog, __FILE__, __LINE__, 26, row, "%05d     %05d    %03d        %03d", bw_out, bw_in, pps_out, pps_in);
}





















// 0x84CC50: both windows; the table sorted by bandwidth every 4th time, the 8 busiest and the 8 quietest addresses.
void BandwidthAccountingClass::Print()
{
	int i;
	for (i = 0; i < 16; i++) {
		Unknown_4BC460(WINDOW, &UnknownLog, __FILE__, __LINE__, 0, i, "                                                                     ");
	}




	Unknown_4BC460(WINDOW, &UnknownLog, __FILE__, __LINE__, 0, 0, "BW Out  :  %d kbps      ", BwOut >> 7);
	Unknown_4BC460(WINDOW, &UnknownLog, __FILE__, __LINE__, 0, 1, "BW In   :  %d kbps      ", BwIn >> 7);
	Unknown_4BC460(WINDOW, &UnknownLog, __FILE__, __LINE__, 0, 2, "Pkts Out:  %d ps     ", PpsOut);
	Unknown_4BC460(WINDOW, &UnknownLog, __FILE__, __LINE__, 0, 3, "Pkts In :  %d ps     ", PpsIn);
	Unknown_4BC460(WINDOW, &UnknownLog, __FILE__, __LINE__, 0, 4, "Peak Out:  %d kbps     ", PeakOut >> 7);
	Unknown_4BC460(WINDOW, &UnknownLog, __FILE__, __LINE__, 0, 5, "Peak In :  %d kbps     ", PeakIn >> 7);
	Unknown_4BC460(WINDOW, &UnknownLog, __FILE__, __LINE__, 30, 4, "Peak Out Per Addr : %d bps      ", PeakOutPerAddr);
	Unknown_4BC460(WINDOW, &UnknownLog, __FILE__, __LINE__, 30, 5, " Peak In Per Addr : %d bps      ", PeakInPerAddr);

	Unknown_4BC460(WINDOW, &UnknownLog, __FILE__, __LINE__, 0, 6, "Total UDP addresses tracked : %d     ", Entries.Count());
	if (PacketsOut != 0) {


		Unknown_4BC460(WINDOW, &UnknownLog, __FILE__, __LINE__, 0, 8, "Packets 1 - 50  bytes    :  %d %%      ", Sizes[0] * 100 / (int)PacketsOut);
		Unknown_4BC460(WINDOW, &UnknownLog, __FILE__, __LINE__, 0, 9, "Packets 51 - 100 bytes   :  %d %%      ", Sizes[1] * 100 / (int)PacketsOut);
		Unknown_4BC460(WINDOW, &UnknownLog, __FILE__, __LINE__, 0, 10, "Packets 101 - 250 bytes  :  %d %%      ", Sizes[2] * 100 / (int)PacketsOut);
		Unknown_4BC460(WINDOW, &UnknownLog, __FILE__, __LINE__, 0, 11, "Packets 251 - 576 bytes  :  %d %%      ", Sizes[3] * 100 / (int)PacketsOut);
		Unknown_4BC460(WINDOW, &UnknownLog, __FILE__, __LINE__, 0, 12, "Packets > 576 bytes      :  %d %%      ", Sizes[4] * 100 / (int)PacketsOut);
	}
	if (SortCountdown-- <= 0) {
		Sort();
		SortCountdown = 3;
	}
	int rows = 8;
	if (Entries.Count() < 16) {
		rows = Entries.Count() / 2;
		if (Entries.Count() == 1) {
			rows = 1;
		}
	}












	Unknown_4BC460(WINDOW_IP, &UnknownLog, __FILE__, __LINE__, 0, 0, "Address                   Bw Out    Bw In    Pkts Out   Pkts In");
	for (i = 1; i < 17; i++) {

		Unknown_4BC460(WINDOW_IP, &UnknownLog, __FILE__, __LINE__, 0, i, "                                                                   ");
	}
	for (i = 0; i < rows; i++) {
		Print_Address(i, i + 1);
	}
	if (rows > 1) {
		for (i = 0; i < rows; i++) {
			Print_Address(Entries.Count() - i - 1, rows + 8 + i);
		}
	}
}

// 0x84D150: qsort's, the highest outgoing bandwidth first.
int BandwidthAccountingClass::Compare(void const *a, void const *b)
{
	unsigned int x = ((BandwidthEntryClass const *)a)->BwOut;
	unsigned int y = ((BandwidthEntryClass const *)b)->BwOut;
	if (x == y) {
		return 0;
	}
	return x < y ? 1 : -1;
}

// 0x84D180
void BandwidthAccountingClass::Sort()
{
	if (Entries.Count() > 1) {
		qsort(&Entries[0], Entries.Count(), sizeof(BandwidthEntryClass), Compare);
		ForgetIn = true;
		ForgetOut = true;
	}
}

// 0x84D1C0
void BandwidthAccountingClass::Reset()
{
	int size = Entries.Length();
	((VectorClass<BandwidthEntryClass> &)Entries).Clear();		// virtual calls in the client
	((VectorClass<BandwidthEntryClass> &)Entries).Resize(size);
	ForgetIn = true;
	ForgetOut = true;
	LastTime = Unknown_Get_Ticks();
	PacketsOut = 0;
	PacketsIn = 0;
	BytesOut = 0;
	BytesIn = 0;
}

// 0x84D240: the address's index; with add, a new entry when it has none (-1 without).
int BandwidthAccountingClass::Find(unsigned int ip, unsigned short port, bool add)
{
	for (int i = 0; i < Entries.Count(); i++) {
		if (ip == Entries[i].Ip && port == Entries[i].Port) {
			return i;
		}
	}
	if (add) {
		BandwidthEntryClass e;
		e.Ip = ip;
		e.Port = port;
		e.BytesIn = 0;
		e.BytesOut = 0;
		e.ExtraIn = 0;
		e.ExtraOut = 0;
		e.BwIn = 0;
		e.BwOut = 0;
		e.ExtraBwIn = 0;
		e.ExtraBwOut = 0;
		e.PacketsIn = 0;
		e.PacketsOut = 0;
		e.PpsIn = 0;
		e.PpsOut = 0;
		e.Idle = 0;
		Entries.Add(e);
		return Entries.Count() - 1;
	}
	return -1;
}

// 0x84D380: the last address's index is kept for the next datagram.
void BandwidthAccountingClass::Record_In(unsigned int ip, unsigned short port, int extra, unsigned int bytes)
{
	if (ForgetIn) {
		LastInIp = 0;
		LastInPort = 0;
		LastInIndex = -1;
		ForgetIn = false;
	} else if (LastInIndex != -1 && LastInIp == ip && LastInPort == port && Entries.Count() != 0) {
		Entries[LastInIndex].BytesIn += bytes;
		Entries[LastInIndex].ExtraIn += extra;
		Entries[LastInIndex].Idle = 0;
		Entries[LastInIndex].PacketsIn++;
		return;
	}
	int i = Find(ip, port, true);
	if (i != -1) {
		Entries[i].BytesIn += bytes;
		Entries[i].ExtraIn += extra;
		Entries[i].Idle = 0;
		Entries[i].PacketsIn++;
		LastInIndex = i;
		LastInIp = ip;
		LastInPort = port;
	}
}

// 0x84D510: also counts the datagram's size.
void BandwidthAccountingClass::Record_Out(unsigned int ip, unsigned short port, int extra, unsigned int bytes)
{
	if (ForgetOut) {
		LastOutIp = 0;
		LastOutPort = 0;
		LastOutIndex = -1;
		ForgetOut = false;
	} else if (LastOutIndex != -1 && LastOutIp == ip && LastOutPort == port && Entries.Count() != 0) {
		Entries[LastOutIndex].BytesOut += bytes;
		Entries[LastOutIndex].ExtraOut += extra;
		Entries[LastOutIndex].Idle = 0;
		Entries[LastOutIndex].PacketsOut++;
		goto sizes;
	}
	{
		int i = Find(ip, port, true);
		if (i != -1) {
			Entries[i].BytesOut += bytes;
			Entries[i].ExtraOut += extra;
			Entries[i].Idle = 0;
			Entries[i].PacketsOut++;
			LastOutIndex = i;
			LastOutIp = ip;
			LastOutPort = port;
		}
	}
sizes:
	if (bytes <= 50) {
		Sizes[0]++;
	} else if (bytes < 100) {
		Sizes[1]++;
	} else if (bytes < 250) {
		Sizes[2]++;
	} else if (bytes < 576) {
		Sizes[3]++;
	} else {
		Sizes[4]++;
	}
}

// 0x84D730: each address's bits and datagrams per second over msec.
void BandwidthAccountingClass::Compute_Rates(unsigned int msec)
{
	if (msec != 0) {
		for (int i = 0; i < Entries.Count(); i++) {
			BandwidthEntryClass &e = Entries[i];
			if (e.ExtraOut != 0) {
				e.ExtraBwOut = (unsigned int)((float)(e.ExtraOut * 8000) / (float)msec);
			} else {
				e.ExtraBwOut = 0;
			}
			e.ExtraOut = 0;
			if (e.ExtraIn != 0) {
				e.ExtraBwIn = (unsigned int)((float)(e.ExtraIn * 8000) / (float)msec);
			} else {
				e.ExtraBwIn = 0;
			}
			e.ExtraIn = 0;
			if (e.BytesOut != 0) {
				e.BwOut = (unsigned int)((float)(e.BytesOut * 8000) / (float)msec);
			} else {
				e.BwOut = 0;
			}
			e.BytesOut = 0;
			if (e.BytesIn != 0) {
				e.BwIn = (unsigned int)((float)(e.BytesIn * 8000) / (float)msec);
			} else {
				e.BwIn = 0;
			}
			e.BytesIn = 0;
			if (e.PacketsOut != 0) {
				e.PpsOut = e.PacketsOut * 1000 / msec;
			} else {
				e.PpsOut = 0;
			}
			e.PacketsOut = 0;
			if (e.PacketsIn != 0) {
				e.PpsIn = e.PacketsIn * 1000 / msec;
			} else {
				e.PpsIn = 0;
			}
			e.PacketsIn = 0;
			if (e.BwOut + e.ExtraBwOut > PeakOutPerAddr) {
				PeakOutPerAddr = e.BwOut + e.ExtraBwOut;
			}
			if (e.ExtraBwIn + e.BwIn > PeakInPerAddr) {
				PeakInPerAddr = e.ExtraBwIn + e.BwIn;
			}
		}
	}
}
