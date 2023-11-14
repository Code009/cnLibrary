
#include <mach/mach.h>
#include <mach/mach_time.h>
#include <mach/clock.h>
#include "Mac_Thread.h"

using namespace cnLibrary;
//using namespace cnRTL;
using namespace cnMac;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cClockService::cClockService()
{
	host_get_clock_service(mach_host_self(),CALENDAR_CLOCK, &fClockService);
}
//---------------------------------------------------------------------------
cClockService::~cClockService()
{
}
//---------------------------------------------------------------------------
sInt64 cClockService::GetSystemTimeNow(void)const
{
	mach_timespec_t TimeNow;
	clock_get_time(fClockService, &TimeNow);

	sInt64 st;
	st=TimeNow.tv_sec-cnSystem::SystemTimeEpochSecondsSinceUnixEpoch;
	st*=Time_1s;
	st+=TimeNow.tv_nsec;
	return st;
}
//---------------------------------------------------------------------------
