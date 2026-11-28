// Earth & Beyond client (reconstruction): app\wwudp\unknown_85bf30.cpp, PROVISIONAL file name (D137). Notes at the end of the file.
#include "cordcon.h"
#include <time.h>
#include <string>	// the client object ends with <string>'s guard 0x85C5C0 (CRT entry 794)

// ------------------------------------------------------------------------ CordStatsClass

// 0x85BF30
CordStatsClass::CordStatsClass(int max_window)
{
	RateLimit = 0xFFFFFFF;
	PeerWindow = 0xFFFFFFF;
	RoundTrip = 300;
	Window = 200;
	Threshold = 0x7FFFFFFF;
	MaxWindow = max_window;
	InFlight = 0;
	RoundStart.setDay(0);
	RoundStart.setMsec(0);
	Round = 0;
	LastPeak = 0;
	Peak = 0;
	LastSecondBytes = 0;
	SecondBytes = 0;
}

// 0x85C010
CordStatsClass::~CordStatsClass()
{
}

// 0x85C090
bool CordStatsClass::Can_Send() const
{
	if (Window - InFlight >= 200 && PeerWindow - InFlight >= 200) {
		return true;
	}
	return false;
}

// 0x85C0D0
bool CordStatsClass::Update(const Xtime &now)
{
	Xtime rtt(now);
	rtt.setDay(0);
	rtt.setMsec(RoundTrip);
	if (RoundStart + rtt < now) {
		RoundStart.update();
		Round++;
	}
	return true;
}

// 0x85C1C0
bool CordStatsClass::Set_Rate_Limit(int bytes)
{
	RateLimit = bytes;
	return true;
}

// 0x85C1E0
void CordStatsClass::Get_Resend_Timeout(Xtime &out) const
{
	out.setDay(0);
	out.setMsec(RoundTrip * 2 + 50);
}

// 0x85C220
bool CordStatsClass::Add_In_Flight(int bytes)
{
	if (bytes != 0) {
		InFlight += bytes;
	}
	if (InFlight > Peak) {
		Peak = InFlight;
	}
	return true;
}

static int AckRound = -1;		// NAME 2: 0xBB1DB4
static int AckGrowth;			// NAME 2: 0xBF411C the window growth this round, all connections

// 0x85C250
bool CordStatsClass::On_Ack(int bytes)
{
	if (AckRound != Round) {
		AckGrowth = 0;
		AckRound = Round;
	}
	InFlight -= bytes;
	if (Window < Threshold) {
		Window += 200;
	} else {
		int grow = 40000 / Window;
		if (grow <= 1) {
			grow = 1;
		}
		if (AckGrowth + grow > 200) {
			grow = 200 - AckGrowth;
		}
		Window += grow;
		AckGrowth += grow;
	}
	if (Window > MaxWindow) {
		Window = MaxWindow;
	}
	return true;
}

static int RoundTripRound = -1;	// NAME 2: 0xBB1DB8

// 0x85C310
bool CordStatsClass::On_Round_Trip(int msec)
{
	if (InFlight > Peak) {
		Peak = InFlight;
	}
	if (RoundTripRound == Round) {
		return false;
	}
	RoundTripRound = Round;
	LastPeak = Peak;
	Peak = 0;
	if (msec > 3000) {
		msec = 3000;
	} else if (msec < 10) {
		msec = 10;
	}
	RoundTrip = (msec + RoundTrip * 4) / 5;
	return true;
}

static int LossRound = -1;		// NAME 2: 0xBB1DBC

// 0x85C3A0
bool CordStatsClass::On_Loss(int bytes, bool timeout)
{
	if (LossRound == Round) {
		if (timeout) {
			int w = Threshold / 2;
			if (w <= 400) {
				w = 400;
			}
			Window = w;
		}
		return false;
	}
	LossRound = Round;
	Threshold = Window * 3 / 4;
	if (timeout) {
		int w = Threshold / 2;
		if (w <= 400) {
			w = 400;
		}
		Window = w;
	}
	return true;
}

static unsigned int LimitedAt;	// NAME 2: 0xBF4120 when the rate limit last held a send back

// 0x85C430
bool CordStatsClass::Allow(int channel, unsigned int len, int send_count, const Xtime &now)
{
	Xtime elapsed(now);
	elapsed -= SecondStart;
	if ((int)elapsed.getMsec() >= 1000) {
		SecondBytes = 0;
		SecondStart.update();
	}
	LastSecondBytes = SecondBytes;
	if (send_count == 0 && len != 0 && channel != 0) {
		if (SecondBytes > RateLimit) {
			if (now.getTime_t() != LimitedAt) {
				LimitedAt = now.getTime_t();
			}
			return false;
		}
		if ((unsigned int)(Window - InFlight) < len || (unsigned int)(PeerWindow - InFlight) < len) {
			return false;
		}
	}
	return true;
}

/*
**	Client 0x85BF30-0x85C5E0, an object of its own by the CRT initialiser table (SG, run 2026-11-18): the
**	connection's send window and statistics CordCon keeps at +0x68 (CordStatsClass, 0x4C bytes), then <string>'s
**	guarded initialiser 0x85C5C0 (entry 794); unknown_cordmsg.cpp's object (CordMessage) ends with its own
**	guard 0x85BF00 (entry 793). No assert or string names the file: PROVISIONAL (D137).
*/
