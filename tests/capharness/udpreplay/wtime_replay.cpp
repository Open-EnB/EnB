// The harness's clock: wlib's Xtime (libs\wwonline\wlib\xtime.h, D73; the members CordCon and CordMgr
// call) over a time the harness sets (Replay_Time_Ms), so that a replay runs the same every time; the
// program doesn't link wlib's xtime.cpp. E&B's fields: the day since year 0 and the millisecond of the
// day. It starts at the real time.
#include "..\..\..\libs\wwonline\wlib\xtime.h"
#include <time.h>

static const sint32 DAY_MS = 86400000;
static const sint32 EPOCH_DAY = 719528;		// 1970-01-01 (0xAFAA8: Xtime::update 0x8BA6F0)

unsigned __int64 Replay_Time_Ms = (unsigned __int64)time(0) * 1000;

Xtime::Xtime()
{
	update();
}

Xtime::Xtime(const Xtime &other)
{
	day_ = other.day_;
	msec_ = other.msec_;
}

Xtime::Xtime(time_t other)
{
	day_ = (sint32)(other / 86400) + EPOCH_DAY;
	msec_ = (sint32)(other % 86400) * 1000;
}

Xtime::~Xtime()
{
}

void Xtime::update()
{
	day_ = (sint32)(Replay_Time_Ms / DAY_MS) + EPOCH_DAY;
	msec_ = (sint32)(Replay_Time_Ms % DAY_MS);
}

time_t Xtime::getTime_t(void) const
{
	return (day_ - EPOCH_DAY) * 86400 + msec_ / 1000;
}

sint32 Xtime::getDay(void) const { return day_; }
sint32 Xtime::getMsec(void) const { return msec_; }
void Xtime::setDay(sint32 day) { day_ = day; }
void Xtime::setMsec(sint32 msec) { msec_ = msec; }
void Xtime::set(sint32 newday, sint32 newmsec) { day_ = newday; msec_ = newmsec; }

void Xtime::normalize(void)
{
	day_ += msec_ / DAY_MS;
	msec_ %= DAY_MS;
	while (msec_ < 0) {
		msec_ += DAY_MS;
		day_--;
	}
}

void Xtime::addSeconds(sint32 seconds)
{
	day_ += seconds / 86400;
	msec_ += (seconds % 86400) * 1000;
	normalize();
}

Xtime &Xtime::operator = (const time_t other)
{
	day_ = (sint32)(other / 86400) + EPOCH_DAY;
	msec_ = (sint32)(other % 86400) * 1000;
	return *this;
}

Xtime &Xtime::operator = (const Xtime &other)
{
	day_ = other.day_;
	msec_ = other.msec_;
	return *this;
}

int Xtime::compare(const Xtime &other) const
{
	if (day_ != other.day_) {
		return day_ < other.day_ ? -1 : 1;
	}
	if (msec_ != other.msec_) {
		return msec_ < other.msec_ ? -1 : 1;
	}
	return 0;
}

bit8 Xtime::operator == (const Xtime &other) const { return compare(other) == 0; }
bit8 Xtime::operator != (const Xtime &other) const { return compare(other) != 0; }
bit8 Xtime::operator < (const Xtime &other) const { return compare(other) < 0; }
bit8 Xtime::operator > (const Xtime &other) const { return compare(other) > 0; }
bit8 Xtime::operator <= (const Xtime &other) const { return compare(other) <= 0; }
bit8 Xtime::operator >= (const Xtime &other) const { return compare(other) >= 0; }

Xtime &Xtime::operator += (const Xtime &other)
{
	day_ += other.day_;
	msec_ += other.msec_;
	normalize();
	return *this;
}

Xtime &Xtime::operator -= (const Xtime &other)
{
	day_ -= other.day_;
	msec_ -= other.msec_;
	normalize();
	return *this;
}

Xtime Xtime::operator - (Xtime &other)
{
	Xtime temp(*this);
	temp -= other;
	return temp;
}

Xtime Xtime::operator + (Xtime &other)
{
	Xtime temp(*this);
	temp += other;
	return temp;
}
