#include <time.h>
#include "POSIX_Time.h"

using namespace cnLibrary;
using namespace siPOSIX;
//---------------------------------------------------------------------------
#ifdef siOS_POSIX_ENABLE_TIME
//---------------------------------------------------------------------------
uInt64 siPOSIX::timespecToNanoSeconds(const timespec &tv)noexcept
{
	uInt64 NanoSeconds;
	NanoSeconds=tv.tv_nsec;
	NanoSeconds+=tv.tv_sec*Time_1s;
	return NanoSeconds;
}
//---------------------------------------------------------------------------
timespec siPOSIX::timespecFromNanoSeconds(uInt64 NanoSeconds)noexcept
{
	timespec ts;
	ts.tv_sec=NanoSeconds/Time_1s;
	ts.tv_nsec=NanoSeconds%Time_1s;
	return ts;
}
//---------------------------------------------------------------------------
uInt64 siPOSIX::GetSystemTimeNow(void)noexcept
{
	timespec now;
	clock_gettime(CLOCK_REALTIME,&now);
	return timespecToNanoSeconds(now);
}
//---------------------------------------------------------------------------
cTimesepcTimepoint::cTimesepcTimepoint()noexcept
{
}
//---------------------------------------------------------------------------
cTimesepcTimepoint::cTimesepcTimepoint(sInt64 NanoSecondsSinceUnixEPoch)noexcept
{
	fValue=timespecFromNanoSeconds(NanoSecondsSinceUnixEPoch);
}
//---------------------------------------------------------------------------
cTimesepcTimepoint::~cTimesepcTimepoint()noexcept
{
}
//---------------------------------------------------------------------------
void cTimesepcTimepoint::timespecFromTime(timespec &tv,iTimepoint *RefTime)noexcept
{
	auto NanoSeconds=RefTime->SystemTime();

	tv=timespecFromNanoSeconds(NanoSeconds);
	tv.tv_sec+=cnSystem::SystemTimeEpochSecondsSinceUnixEpoch;
}
//---------------------------------------------------------------------------
sInt64 cTimesepcTimepoint::SystemTime(void)noexcept
{
	return timespecToNanoSeconds(fValue);
}
//---------------------------------------------------------------------------
sInt64 cTimesepcTimepoint::SinceTime(iTimepoint *Time)noexcept
{
	timespec ref;
	timespecFromTime(ref,Time);
	sInt64 dsec=fValue.tv_sec-ref.tv_sec;
	sInt64 dns=fValue.tv_nsec-ref.tv_nsec;
	sInt64 d=dsec*Time_1s+dns;
	return d;
}
//---------------------------------------------------------------------------
#endif	// siOS_POSIX_ENABLE_TIME
//---------------------------------------------------------------------------

