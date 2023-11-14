#include <time.h>
#include "POSIX_Time.h"

using namespace cnLibrary;
using namespace siPOSIX;
//---------------------------------------------------------------------------
#if	_POSIX_C_SOURCE >= 200112L

//---------------------------------------------------------------------------
cTimepoint::cTimepoint()
{
}
//---------------------------------------------------------------------------
cTimepoint::cTimepoint(sint64 NanoSecondsSinceUnixEPoch)
{
	NanoSecondsTotimespec(fValue,NanoSecondsSinceUnixEPoch);
}
//---------------------------------------------------------------------------
cTimepoint::~cTimepoint()
{
}
//---------------------------------------------------------------------------
sint64 cTimepoint::timespecToNanoSeconds(const timespec &tv)
{
	sint64 NanoSeconds;
	NanoSeconds=tv.tv_nsec;
	NanoSeconds+=tv.tv_sec*Time_1s;
	return NanoSeconds;
}
//---------------------------------------------------------------------------
void cTimepoint::NanoSecondsTotimespec(timespec &tv,sint64 NanoSeconds)
{
	tv.tv_sec=NanoSeconds/Time_1s;
	tv.tv_nsec=NanoSeconds%Time_1s;
}
//---------------------------------------------------------------------------
void cTimepoint::timespecFromTime(timespec &tv,const iTimepoint *RefTime)
{
	auto NanoSeconds=RefTime->SystemTime();

	NanoSecondsTotimespec(tv,NanoSeconds);
	tv.tv_sec+=cnSystem::EpochSecondsSinceUnix;
}
//---------------------------------------------------------------------------
sint64 cnLib_FUNC cTimepoint::SystemTime(void)const
{
	return timespecToNanoSeconds(fValue);
}
//---------------------------------------------------------------------------
sint64 cnLib_FUNC cTimepoint::SinceTime(const iTimepoint *Time)const
{
	timespec ref;
	timespecFromTime(ref,Time);
	sint64 dsec=fValue.tv_sec-ref.tv_sec;
	sint64 dns=fValue.tv_nsec-ref.tv_nsec;
	sint64 d=dsec*Time_1s+dns;
	return d;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cTimeNow::cTimeNow()
{
}
//---------------------------------------------------------------------------
cTimeNow::~cTimeNow()
{
}
//---------------------------------------------------------------------------
sint64 cnLib_FUNC cTimeNow::SystemTime(void)const
{
	timespec now;
	clock_gettime(CLOCK_REALTIME,&now);
	return cTimepoint::timespecToNanoSeconds(now);
}
//---------------------------------------------------------------------------
sint64 cnLib_FUNC cTimeNow::SinceTime(const iTimepoint *Time)const
{
	timespec now,ref;
	clock_gettime(CLOCK_REALTIME,&now);
	cTimepoint::timespecFromTime(ref,Time);

	sint64 dsec=now.tv_sec-ref.tv_sec;
	sint64 dns=now.tv_nsec-ref.tv_nsec;
	sint64 d=dsec*Time_1s+dns;
	return d;
}
//---------------------------------------------------------------------------
#endif	// _POSIX_C_SOURCE >= 200112L
//---------------------------------------------------------------------------

