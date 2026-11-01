/*
**	Command & Conquer Generals Zero Hour(tm)
**	Copyright 2025 Electronic Arts Inc.
**	Modified for the Earth & Beyond client reconstruction: tools/vc6/importer.py records each change.
**	This program is free software: you can redistribute it and/or modify
**	it under the terms of the GNU General Public License as published by
**	the Free Software Foundation, either version 3 of the License, or
**	(at your option) any later version.
**
**	This program is distributed in the hope that it will be useful,
**	but WITHOUT ANY WARRANTY; without even the implied warranty of
**	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**	GNU General Public License for more details.
**
**	You should have received a copy of the GNU General Public License
**	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/****************************************************************************\

xtime                      Neal Kettler

Improved version of the Wtime library (V->W->X...)

Handles signed times better and dates long
long long after you'll be dead.

\****************************************************************************/

#include <ctype.h>
#include <time.h>
#ifndef _WINDOWS
#include <sys/time.h>
#endif
#include "xtime.h"

static char *DAYS[]={"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};

static char *FULLDAYS[]={"Sunday","Monday","Tuesday","Wednesday","Thursday",
               "Friday","Saturday"};

static char *MONTHS[]={"Jan","Feb","Mar","Apr","May","Jun","Jul",
               "Aug","Sep","Oct","Nov","Dec"};

static char *FULLMONTHS[]={"January","February","March","April","May","June",
               "July","August","September","October","November","December"};


#define IS_LEAP(y) ((y) % 4) == 0 && (! ((y) % 100) == 0 || ((y) % 400) == 0)
#define LEAPS_THRU_END_OF(y) ((y) / 4 - (y) / 100 + (y) / 400)


/////////////// Utility functions ///////////////////

//
// Return the daycount since year 0 for the specified date.
// month = 1-12, day = 1-31 year = 0...
//
static sint32 Get_Day(int month, int day, int year)
{
  time_t days;

  static int DaysAtMonth[] =
      {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};

  /* Add up number of straight days + number of leap days + days */
  /* up to the month + the day of month. */

  days = (year * 365) + (year / 4) - (year / 100) + (year / 400) +
      DaysAtMonth[month-1] + day;

  /* If we haven't hit Feb 29, and this is a leap year, we need to */
  /* subtract out the leap day that was added above for this year */

  if (month < 3 && IS_LEAP(year))
      --days;
    return(days);
}



//
// Get the year from a daycount since year 0
// Also get the daycount since the start of the year
//
// Ayecarumba what a pain in the ass!
//
static bit8 Get_Date_From_Day(sint32 days, OUT int &year, OUT sint32 &yday)	// E&B: int year (wstypes.h's sint32 is long)
{
  //register long int rem;
  register long int y;
  //register const unsigned short int *ip;
 
  if (days <= 365)
  {
    year=0;
    yday=days+1;  // 1 based
    return(TRUE);
  }

  y = 1;
  days-=365;

  days--;  // zero based
 
  //
  // As far as I can tell there's no non-iteritive way to
  //   do this...
  //
  while (days < 0 || days >= (IS_LEAP (y) ? 366 : 365))
  {
    /* Guess a corrected year, assuming 365 days per year.  */
    long int yg = y + days / 365 - (days % 365 < 0);
 
    /* Adjust DAYS and Y to match the guessed year.  */
    days -= ((yg - y) * 365
        + LEAPS_THRU_END_OF (yg - 1)
        - LEAPS_THRU_END_OF (y - 1));
      y = yg;
  }
  year=y;
  yday=days+1;  // 1 based
  return(TRUE);
}


//
// Get the max day of a given month in a given year
//
int Max_Day(int month, int year)
{
  static char dayTable[2][13] = {
    {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
    {0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
  };
  bit8 isleap=IS_LEAP(year);
  return(dayTable[isleap][month]);
}



/**********************************************************
int main(int argc, char *argv[])
{
  Xtime wtime;
  int month,mday,year,hour,minute,second,day;
  int i;
  int dayoffset;
  int yr;
  int yday;
  int ydaycount;

  wtime.set(719528,0);

***********************************************
  for (year=0; year<10000; year++)
  {
    ydaycount=1;
    for (month=1; month<=12; month++)
    {
      for (day=1; day<=Max_Day(month,year); day++)
      {
        dayoffset=Get_Day(month,day,year);
        assert (GetDateFromDay(dayoffset,yr,yday)==TRUE);

        //printf("Yday=%d YdayCount=%d\n",yday,ydaycount);

        if (yr!=year)
        {
          printf("MO=%d  DAY=%d  YEAR=%d  YR=%d\n",month,day,year,yr);
          assert(0);
        }
        if (yday != ydaycount)
        {
          printf("MO=%d  DAY=%d  YEAR=%d  YR=%d\n",month,day,year,yr);
          printf("Yday=%d YdayCount=%d\n",yday,ydaycount);
          assert(0);
        }
        ydaycount++;
      }
    } 
    printf("(%d) ",year);
  }
***************************************

  ///////wtime.addSeconds((60*60*24)-(60*60*8));
  ////////wtime.addSeconds(-(60*60*8));  // timezone delta

  dayoffset=Get_Day(1,1,1970);
  printf("DAYOFFSET = %d\n",dayoffset);


  wtime.getTime(month, mday, year, hour, minute, second);
  printf("\n%s %d  %d  %d:%02d:%02d\n\n",
    MONTHS[month-1],mday,year,hour,minute,second);

  struct  timeval   unixtime;
  struct  timezone  unixtzone;
  time_t    ttime;
  tm        tmtime;

  memset(&tmtime,0,sizeof(tmtime));
  ttime=0;
  unixtime.tv_sec=0;
  unixtime.tv_usec=0;

  //gettimeofday(&unixtime,&unixtzone);
  //ttime=time(NULL);
  tmtime=*gmtime(&ttime);

  printf("TIME->CTIME = %s\n",ctime(&ttime));
  printf("GTOD->CTIE = %s\n",ctime(&(unixtime.tv_sec)));
  printf("TIME->GMT->ASCTIME = %s\n",asctime(&tmtime));
}
***************************************************************/



//
// Construct with current clock time
//
Xtime::Xtime(void)
{
  update();
}

//
// Copy constructor
//
Xtime::Xtime( const Xtime &other )
{
  day_=other.day_;
  msec_=other.msec_;
}

//
// Set to a time_t (1970-2038)
//
Xtime::Xtime( time_t other )
{
  day_=719528;  // days from year 0 to Jan1, 1970
  // Add seconds from time_t
  addSeconds(other);
  msec_=0;
}

Xtime::~Xtime()
{
}

//
// E&B: the time as a time_t (1970-2038), 0x8BA510
//
time_t Xtime::getTime_t(void) const
{
  return((day_-719528)*(60*60*24) + msec_/1000);
}

//
// Add some number of seconds to the time (seconds can be negative)
//
void Xtime::addSeconds(sint32 seconds)
{
  // Add to day counter first
  day_+=(seconds/86400);
  msec_+=(seconds % 86400)*1000;

  // Now normalize in case msec is > 1 days worth
  normalize();
}

//
// E&B: add some number of milliseconds to the time (can be negative), 0x8BA5F0
//
void Xtime::addMsec(sint32 msec)
{
  day_+=(msec/86400000);
  msec_+=(msec % 86400000);

  normalize();
}

//
// If msec is > 1 days worth, adjust the day count
//   & decrement the msec counter until OK.
//
void Xtime::normalize(void)
{
  day_+=(msec_/86400000);
  msec_%=86400000;
 
  while (msec_ < 0)
  {
    day_--;
    msec_+=86400000;
  }
}

//
// Update time to hold the current clock time
// (breaks in 2038 :-)
//
void Xtime::update(void)
{
  day_=719528;  // day_s from year 0 to Jan1, 1970
  msec_=0;

 #ifdef _WINDOWS
  struct _timeb    wintime;
  _ftime(&wintime);
  addSeconds(wintime.time);
  msec_+=wintime.millitm;
 #endif
 #ifndef _WINDOWS
  struct timeval   unixtime;
  struct timezone  unixtzone;
  gettimeofday(&unixtime,&unixtzone);
  addSeconds(unixtime.tv_sec);
  msec_+=(unixtime.tv_usec/1000);
 #endif

  // Now normalize in case msec is > 1 days worth
  normalize();
}


// Get day_s since year 0
sint32 Xtime::getDay(void) const
{
  return(day_);
}

// Get msecs since start of day
sint32 Xtime::getMsec(void) const
{
  return(msec_);
}

// Set days since year 0
void Xtime::setDay(sint32 newday)
{
  day_=newday;
}

// Set msec since start of this day
void Xtime::setMsec(sint32 newmsec)
{
  msec_=newmsec;
}

// Set both
void Xtime::set(sint32 newday, sint32 newmsec)
{
  day_=newday;
  msec_=newmsec;
}


//
// Get a timeval ptr from a Xtime class
// May fail if timeval can't hold a year this big or small
//
bit8 Xtime::getTimeval(struct timeval &tv)
{
  // A timeval can only hold dates from 1970-2038
  if ((day_ < 719528) || (day_ >= 719528+24855))
    return(FALSE);

  // Compute seconds since Jan 1, 1970
  uint32 seconds=day_-719528;
  seconds*=(60*60*24);
  seconds+=(msec_/1000);

  tv.tv_sec=seconds;
  tv.tv_usec=(msec_%1000)*1000;
  return(TRUE);
}

//
// Set the time
//
bit8 Xtime::setTime(int month, int mday, int year, int hour, int minute, int second)
{
  day_=Get_Day(month,mday,year);
  msec_=(hour*1000*60*60)+(minute*1000*60)+(second*1000);
  return(TRUE);
}



//
// Get all the components of the time in the usual normalized format.
//
// Most of the uglyness is in Get_Date_From_Day() 
//
bit8 Xtime::getTime(int &month, int &mday, int &year, int &hour, int &minute, int &second) const
{
  int i;
  sint32 dayofyear;
  if (Get_Date_From_Day(day_,year,dayofyear)==FALSE)
    return(FALSE);

  static int DaysAtMonth[2][12] = {
        {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334},   // normal
        {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335}    // leap year
  };

  month=0;

  bit8 isleap=IS_LEAP(year);
  for (i=0; i<12; i++)
  {
    if (DaysAtMonth[isleap][i] > dayofyear)  // E&B: > (0x8BAA50 `jg`)
      break;
    month=i;
  }
  month++;  // 1 based

  mday=dayofyear-DaysAtMonth[isleap][month-1];

  // Whew! Now all we have to do is figure out H/M/S from the msec!
  hour=(msec_/3600000)%24;  // 1000*60*60
  minute=(msec_/60000)%60;  // 1000*60 
  second=(msec_/ 1000)%60;  // 1000

  return(TRUE);
}

//
// These are for getting components of the time in the
//   standard ranges.  Like Day 1-31, Second 0-59, etc...
//
int Xtime::getSecond(void) const
{
  int month,mday,year,hour,minute,second;
  getTime(month, mday, year, hour, minute, second);
  return(second);
}
int Xtime::getMinute(void) const
{
  int month,mday,year,hour,minute,second;
  getTime(month, mday, year, hour, minute, second);
  return(minute);
}
int Xtime::getHour(void) const
{
  int month,mday,year,hour,minute,second;
  getTime(month, mday, year, hour, minute, second);
  return(hour);
}
int Xtime::getMDay(void) const
{
  int month,mday,year,hour,minute,second;
  getTime(month, mday, year, hour, minute, second);
  return(mday);
}

int Xtime::getMonth(void) const
{
  int month,mday,year,hour,minute,second;
  getTime(month, mday, year, hour, minute, second);
  return(month);
}

// based at year 0 (real 0, not 1970)
int Xtime::getYear(void) const
{
  int month,mday,year,hour,minute,second;
  getTime(month, mday, year, hour, minute, second);
  return(year);
}


//
// Set the seconds value (0-59)
//
bit8 Xtime::setSecond(sint32 sec)
{
  sint32 second=(msec_/ 1000)%60;
  msec_-=(second*1000);
  msec_+=(sec*1000);
  return(TRUE);
}

//
// Set the minutes value (0-59)
//
bit8 Xtime::setMinute(sint32 min)
{
  sint32 minute=(msec_/60000)%60;  // 1000*60 
  msec_-=(minute*60000);
  msec_+=(min*60000);
  return(TRUE);
}

//
// Set the minutes value (0-23)
//
bit8 Xtime::setHour(sint32 hour)
{
  hour=(msec_/3600000)%24;  // 1000*60*60
  msec_-=(hour*3600000);
  msec_+=(hour*3600000);
  return(TRUE);
}

//
// Set the year value
// Results are undefined if you're moving from Feb 29, to a non leap year
//
bit8 Xtime::setYear(sint32 _year)
{
  // extract the date
  int month,mday,year,hour,min,sec;
  getTime(month,mday,year,hour,min,sec);

  // modify & rebuild
  year=_year;
  day_=Get_Day(month,mday,year);
  return(TRUE);
}

//
// Modify the month
//
bit8 Xtime::setMonth(sint32 _month)
{
  // extract the date
  int month,mday,year,hour,min,sec;
  getTime(month,mday,year,hour,min,sec);

  // modify & rebuild
  month=_month;
  day_=Get_Day(month,mday,year);
  return(TRUE);
}


//
// Modify the day of the month 
//
bit8 Xtime::setMDay(sint32 _mday)
{
  // extract the date
  int month,mday,year,hour,min,sec;
  getTime(month,mday,year,hour,min,sec);

  // modify & rebuild
  mday=_mday;
  day_=Get_Day(month,mday,year);
  return(TRUE);
}


//
// Compare two times.  The time better be normalized
//   which it would be unless you've put bad stuff in it!
//
// 1 = *this > other
//-1 = *this < other
// 0 = *this == other
int   Xtime::compare(const Xtime &other) const
{
  if ((day_==other.day_)&&(msec_==other.msec_))
    return(0);        // equal

  else if (day_>other.day_)
    return(1);
  else if (day_<other.day_)
    return(-1);
  else if (msec_>other.msec_)
    return(1);
  else
    return(-1);
}
  

bit8 Xtime::operator == ( const Xtime &other ) const
{
  bit8 retval=compare(other);
  if (retval==0)
    return(TRUE);
  else
    return(FALSE);
}

bit8 Xtime::operator != ( const Xtime &other ) const
{
  bit8 retval=compare(other);
  if (retval==0)
    return(FALSE);
  else
    return(TRUE);
}

bit8 Xtime::operator < ( const Xtime &other ) const
{
  int retval=compare(other);
  if (retval==-1)
    return(TRUE);
  else
    return(FALSE);
}

bit8 Xtime::operator > ( const Xtime &other ) const
{
  int retval=compare(other);
  if (retval==1)
    return(TRUE);
  else
    return(FALSE);
}

bit8 Xtime::operator <= ( const Xtime &other ) const
{
  int retval=compare(other);
  if ((retval==-1)||(retval==0))
    return(TRUE);
  else
    return(FALSE);
}

bit8 Xtime::operator >= ( const Xtime &other ) const
{
  int retval=compare(other);
  if ((retval==1)||(retval==0))
    return(TRUE);
  else
    return(FALSE);
}


Xtime &Xtime::operator += (const Xtime &other)
{
  day_+=other.day_;
  msec_+=other.msec_;
  normalize();
  return *this;
}

Xtime &Xtime::operator -= (const Xtime &other)
{
  day_-=other.day_;
  msec_-=other.msec_;
  normalize();
  return *this;
}

Xtime Xtime::operator - (Xtime &other)
{
  Xtime temp(*this);
  temp-=other;
  return(temp);
}

Xtime Xtime::operator + (Xtime &other)
{
  Xtime temp(*this);
  temp+=other;
  return(temp);
}


Xtime   &Xtime::operator = (const Xtime &other)
{
  day_=other.day_;
  msec_=other.msec_;
  return *this;
}


Xtime &Xtime::operator += (const time_t other)
{
  addSeconds(other);
  return *this;
}


Xtime &Xtime::operator -= (const time_t other)
{
  addSeconds(-((sint32)other));
  return *this;
}


Xtime Xtime::operator - (time_t other)
{
  Xtime temp(*this);
  temp-=other;
  return(temp);
}


Xtime Xtime::operator + (time_t other)
{
  Xtime temp(*this);
  temp+=other;
  return(temp);
}

Xtime &Xtime::operator = (const time_t other)
{
  msec_=0;
  day_=719528;  // Jan 1, 1970
  addSeconds(other);
  return *this;
}
