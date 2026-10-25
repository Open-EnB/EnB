// The harness's stand-in for wlib's Wtime (app/wwudp/wtime.h): E&B's fields (the day since year 0
// and the millisecond of the day) over a clock the harness sets (Replay_Time_Ms), so that a replay
// runs the same every time. It starts at the real time.
#include "wtime.h"
#include <time.h>

static const unsigned int DAY_MS = 86400000;
static const unsigned int EPOCH_DAY = 0xAFAA8;		// 1970-01-01 (Update 0x8BA6F0)

unsigned __int64 Replay_Time_Ms = (unsigned __int64)time(0) * 1000;

Wtime::Wtime()
{
	Update();
}

Wtime::Wtime(const Wtime &other) : Day(other.Day), Msec(other.Msec)
{
}

Wtime::Wtime(unsigned int seconds)
{
	Day = seconds / 86400 + EPOCH_DAY;
	Msec = (seconds % 86400) * 1000;
}

Wtime::~Wtime()
{
}

void Wtime::Update()
{
	Day = (unsigned int)(Replay_Time_Ms / DAY_MS) + EPOCH_DAY;
	Msec = (unsigned int)(Replay_Time_Ms % DAY_MS);
}

unsigned int Wtime::Get_Time_T() const
{
	return (Day - EPOCH_DAY) * 86400 + Msec / 1000;
}

unsigned int Wtime::Get_Day() const { return Day; }
unsigned int Wtime::Get_Msec() const { return Msec; }
void Wtime::Set_Day(unsigned int day) { Day = day; }
void Wtime::Set_Msec(unsigned int msec) { Msec = msec; }
void Wtime::Set(unsigned int day, unsigned int msec) { Day = day; Msec = msec; }

void Wtime::Add_Seconds(int seconds)
{
	int day = seconds / 86400;
	int msec = (seconds % 86400) * 1000 + (int)Msec;
	while (msec < 0) {
		msec += DAY_MS;
		day--;
	}
	while (msec >= (int)DAY_MS) {
		msec -= DAY_MS;
		day++;
	}
	Day += day;
	Msec = (unsigned int)msec;
}

Wtime &Wtime::operator = (unsigned int seconds)
{
	Day = seconds / 86400 + EPOCH_DAY;
	Msec = (seconds % 86400) * 1000;
	return *this;
}

bool Wtime::operator < (const Wtime &other) const
{
	return Day < other.Day || (Day == other.Day && Msec < other.Msec);
}

bool Wtime::operator > (const Wtime &other) const
{
	return Day > other.Day || (Day == other.Day && Msec > other.Msec);
}

Wtime &Wtime::operator -= (const Wtime &other)
{
	if (Msec < other.Msec) {
		Msec += DAY_MS;
		Day--;
	}
	Msec -= other.Msec;
	Day -= other.Day;
	return *this;
}

Wtime Wtime::operator - (const Wtime &other) const
{
	Wtime t(*this);
	t -= other;
	return t;
}

Wtime Wtime::operator + (const Wtime &other) const
{
	Wtime t(*this);
	t.Msec += other.Msec;
	t.Day += other.Day;
	if (t.Msec >= DAY_MS) {
		t.Msec -= DAY_MS;
		t.Day++;
	}
	return t;
}

Wtime &Wtime::operator = (const Wtime &other)
{
	Day = other.Day;
	Msec = other.Msec;
	return *this;
}
