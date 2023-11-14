#include "Mach_Time.h"

using namespace cnLibrary;
using namespace siMach;

//---------------------------------------------------------------------------
//static const iTypeInfo IID_cMachTimespec={};
//const iTypeID cMachTimespec::cnInterfaceID=&IID_cMachTimespec;
//---------------------------------------------------------------------------
static const iTypeInfo IID_iMachTimespec={};
const iTypeID iMachTimespec::cnInterfaceID=&IID_iMachTimespec;
//---------------------------------------------------------------------------
//void cMachTimespec::FromAbsoluteTime(mach_timespec &tv,const cAbsoluteTime &at)
//{
//	tv.tv_sec=at.cAbsoluteTime::
//	tv.tv_sec=
	//auto SecondsSinceUnixEpoch=RefTime->SinceTime(cnSystem::RefTime_UnixEpoch);
	//SecondsToTimespec(tv,SecondsSinceUnixEpoch);
//}
//---------------------------------------------------------------------------
cTimespec::cTimespec()
{
}
//---------------------------------------------------------------------------
cTimespec::cTimespec(Float64 SecondsSinceUnixEPoch)
{
	SecondsToTimespec(fValue,SecondsSinceUnixEPoch);
}
//---------------------------------------------------------------------------
cTimespec::~cTimespec()
{
}
//---------------------------------------------------------------------------
void cTimespec::GetTimeNow(mach_timespec &tv)
{
	clock_serv_t cserv;
	host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cserv);
	clock_get_time(cserv,&tv);
}
//---------------------------------------------------------------------------
Float64 cTimespec::TimespecToSeconds(const mach_timespec &tv)
{
	Float64 Seconds;
	Seconds=tv.tv_nsec;
	Seconds/=NanoSeconds;
	Seconds+=tv.tv_sec;
	return Seconds;
}
//---------------------------------------------------------------------------
void cTimespec::SecondsToTimespec(mach_timespec &tv,Float64 Seconds)
{
	tv.tv_sec=Seconds;
	tv.tv_nsec=(Seconds-tv.tv_sec)*NanoSeconds;
}
//---------------------------------------------------------------------------
void cTimespec::TimespecFromTime(mach_timespec &tv,const iTimepoint *RefTime)
{
	auto MachTime=iCast<iMachTimespec>(RefTime);
	if(MachTime!=nullptr){
		tv=MachTime->MachTimespec();
		return;
	}
	//auto SecondsSinceUnixEpoch=RefTime->SinceTime(cnSystem::RefTime_UnixEpoch);
	int SecondsSinceUnixEpoch=0;
	SecondsToTimespec(tv,SecondsSinceUnixEpoch);
}
//---------------------------------------------------------------------------
Float64 cTimespec::SinceTime(const iTimepoint *Time)
{
	mach_timespec ref;
	if(Time!=nullptr){
		TimespecFromTime(ref,Time);
	}
	else{
		GetTimeNow(ref);
	}
	mach_timespec delta=fValue;
	SUB_MACH_TIMESPEC(&delta,&ref);

	return TimespecToSeconds(delta);
}
//---------------------------------------------------------------------------
void cTimespec::Set(const iTimepoint *RefTime,Float64 Difference)
{
	if(RefTime==nullptr){
		// for now
		GetTimeNow(fValue);
	}
	else{
		TimespecFromTime(fValue,RefTime);
	}
	if(Difference!=0){
		mach_timespec dt;
		SecondsToTimespec(dt,Difference);
		SUB_MACH_TIMESPEC(&fValue,&dt);
	}
}
//---------------------------------------------------------------------------
const mach_timespec& cTimespec::MachTimespec(void)
{
	return fValue;
}
//---------------------------------------------------------------------------

