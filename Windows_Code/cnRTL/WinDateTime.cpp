﻿#include "WinDateTime.h"

using namespace cnLibrary;
using namespace cnRTL;
using namespace cnWinRTL;

static constexpr uInt64 FileTime_Second=1'000'000'0;
static constexpr uInt64 FileTime_ToNanoSecond=100;
//---------------------------------------------------------------------------
void cnWinRTL::NTFileTimeToDateTime(cDateTime &DateTime,const FILETIME &FileTime)noexcept(true)
{
	SYSTEMTIME st;
	::FileTimeToSystemTime(&FileTime,&st);
	DateTime.Year=st.wYear;
	DateTime.Month=static_cast<uInt8>(st.wMonth);
	DateTime.Day=static_cast<uInt8>(st.wDay);

	DateTime.Hour=static_cast<uInt8>(st.wHour);
	DateTime.Minute=static_cast<uInt8>(st.wMinute);
	DateTime.Second=static_cast<uInt8>(st.wSecond);
	DateTime.NanoSecond=st.wMilliseconds*1'000'000;

}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void cNTFileTime::SetNow(void)noexcept(true)
{
	::GetSystemTimeAsFileTime(&FileTime);
}
//---------------------------------------------------------------------------
bool cNTFileTime::ToDateTime(cDateTime &DateTime)const noexcept(true)
{
	SYSTEMTIME st;
	if(::FileTimeToSystemTime(&FileTime,&st)==FALSE){
		return false;
	}
	DateTime.Year=st.wYear;
	DateTime.Month=static_cast<uInt8>(st.wMonth);
	DateTime.Day=static_cast<uInt8>(st.wDay);

	DateTime.Hour=static_cast<uInt8>(st.wHour);
	DateTime.Minute=static_cast<uInt8>(st.wMinute);
	DateTime.Second=static_cast<uInt8>(st.wSecond);
	DateTime.NanoSecond=st.wMilliseconds*1'000'000;
	return true;
}
//---------------------------------------------------------------------------
bool cNTFileTime::FromDateTime(const cDateTime &DateTime)noexcept(true)
{
	SYSTEMTIME st;
	st.wYear=DateTime.Year;
	st.wMonth=DateTime.Month;
	st.wDay=DateTime.Day;
	st.wDayOfWeek=0;

	st.wHour=DateTime.Hour;
	st.wMinute=DateTime.Minute;
	st.wSecond=DateTime.Second;
	st.wMilliseconds=static_cast<WORD>(DateTime.NanoSecond/1'000'000);

	return ::SystemTimeToFileTime(&st,&FileTime)!=FALSE;
}
//---------------------------------------------------------------------------
void cNTFileTime::FromTimepoint(iTimepoint *Time)noexcept(true)
{
	auto RefFileTime=iCast<iFileTime>(Time);
	if(RefFileTime!=nullptr){
		TimeValue=RefFileTime->GetTimeValue();
	}
	else{
		// convert
		auto st=Time->SystemTime();
		FromSystemTime(st);
	}
}
//---------------------------------------------------------------------------
void cNTFileTime::FromTimepoint(iTimepoint *Time,sInt64 Delay)noexcept(true)
{
	auto RefFileTime=iCast<iFileTime>(Time);
	if(RefFileTime!=nullptr){
		TimeValue=RefFileTime->GetTimeValue();
		TimeValue.QuadPart+=Delay/FileTime_ToNanoSecond;
	}
	else{
		// convert
		auto st=Time->SystemTime();
		st+=Delay;
		FromSystemTime(st);
	}
}
//---------------------------------------------------------------------------
void cNTFileTime::AdjustNanoSeconds(sInt64 Seconds)noexcept(true)
{
	sInt64 FileTimeValue=Seconds/FileTime_ToNanoSecond;
	TimeValue.QuadPart+=FileTimeValue;
}
//---------------------------------------------------------------------------
void cNTFileTime::AdjustSeconds(sInt64 Seconds)noexcept(true)
{
	sInt64 FileTimeValue=Seconds*FileTime_Second;
	TimeValue.QuadPart+=FileTimeValue;
}
//---------------------------------------------------------------------------
Float64 cNTFileTime::DifferenceSeconds(const cNTFileTime &Time)const noexcept(true)
{
	sInt64 diff=TimeValue.QuadPart-Time.TimeValue.QuadPart;

	return diff/static_cast<Float64>(FileTime_Second);
}
//---------------------------------------------------------------------------
sInt64 cNTFileTime::DifferenceNanoseconds(const cNTFileTime &Time)const noexcept(true)
{
	sInt64 diff=TimeValue.QuadPart-Time.TimeValue.QuadPart;

	return diff*FileTime_ToNanoSecond;
}
//---------------------------------------------------------------------------
uInt64 cNTFileTime::ToSystemTime(void)const noexcept(true)
{
	uInt64 st=TimeValue.QuadPart-cnWindows::SystemTimeEpochAsFileTime.QuadPart;
	return st*FileTime_ToNanoSecond;
}
//---------------------------------------------------------------------------
void cNTFileTime::FromSystemTime(uInt64 st)noexcept(true)
{
	TimeValue.QuadPart=st/FileTime_ToNanoSecond;
	TimeValue.QuadPart+=cnWindows::SystemTimeEpochAsFileTime.QuadPart;
}
//---------------------------------------------------------------------------
void cNTFileTime::FromTimeDue(iTimepoint *DueTime,sInt64 DueTimeDelay)noexcept(true)
{
	if(DueTime==nullptr){
		if(DueTimeDelay<=0){
			// due now
			TimeValue.QuadPart=0;
		}
		else{
			// delay from now
			TimeValue.QuadPart=-DueTimeDelay;
		}
	}
	else{
		FromTimepoint(DueTime);
		AdjustNanoSeconds(DueTimeDelay);
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
sInt64 cnWinRTL::SystemTimeSinceNTFileTimeNowNS(uInt64 SystemTime)noexcept(true)
{
	cNTFileTime Now,Time;
	Now.SetNow();
	Time.FromSystemTime(SystemTime);
	sInt64 Nanoseconds=Time.DifferenceNanoseconds(Now);
	return Nanoseconds;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cNTFileTimePoint::cNTFileTimePoint()noexcept(true)
{
}
//---------------------------------------------------------------------------
cNTFileTimePoint::~cNTFileTimePoint()noexcept(true)
{
}
//---------------------------------------------------------------------------
iPtr<cNTFileTimePoint> cNTFileTimePoint::TimeNow(void)noexcept(true)
{
	auto tp=iCreate<cNTFileTimePoint>();
	tp->SetNow();
	return tp;
}
//---------------------------------------------------------------------------
sInt64 cNTFileTimePoint::SystemTime(void)noexcept(true)
{
	return ToSystemTime();
}
//---------------------------------------------------------------------------
sInt64 cNTFileTimePoint::SinceTime(iTimepoint *Time)noexcept(true)
{
	cNTFileTime dt;
	dt.FromTimepoint(Time);
	
	return DifferenceNanoseconds(dt);
}
//---------------------------------------------------------------------------
FILETIME cNTFileTimePoint::GetFileTime(void)noexcept(true)
{
	return FileTime;
}
//---------------------------------------------------------------------------
LARGE_INTEGER cNTFileTimePoint::GetTimeValue(void)noexcept(true)
{
	return TimeValue;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
