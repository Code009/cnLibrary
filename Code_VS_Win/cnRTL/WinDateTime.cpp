#include "WinDateTime.h"

using namespace cnLibrary;
using namespace cnRTL;
using namespace cnWinRTL;

static constexpr sInt64 FileTime_Second=1'000'000'0;
static constexpr sInt64 FileTime_ToNanoSecond=100;
//---------------------------------------------------------------------------
void cnWinRTL::NTFileTimeToDateTime(cDateTime &DateTime,const FILETIME &FileTime)
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
void cNTFileTime::SetNow(void)
{
	::GetSystemTimeAsFileTime(&FileTime);
}
//---------------------------------------------------------------------------
bool cNTFileTime::ToDateTime(cDateTime &DateTime)const
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
bool cNTFileTime::FromDateTime(const cDateTime &DateTime)
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
void cNTFileTime::FromTimepoint(iTimepoint *Time)
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
void cNTFileTime::FromTimepoint(iTimepoint *Time,sInt64 Delay)
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
void cNTFileTime::AdjustNanoSeconds(sInt64 Seconds)
{
	sInt64 FileTimeValue=Seconds/FileTime_ToNanoSecond;
	TimeValue.QuadPart+=FileTimeValue;
}
//---------------------------------------------------------------------------
void cNTFileTime::AdjustSeconds(sInt64 Seconds)
{
	sInt64 FileTimeValue=Seconds*FileTime_Second;
	TimeValue.QuadPart+=FileTimeValue;
}
//---------------------------------------------------------------------------
Float64 cNTFileTime::DifferenceSeconds(const cNTFileTime &Time)const
{
	sInt64 diff=TimeValue.QuadPart-Time.TimeValue.QuadPart;

	return diff/static_cast<Float64>(FileTime_Second);
}
//---------------------------------------------------------------------------
sInt64 cNTFileTime::DifferenceNanoseconds(const cNTFileTime &Time)const
{
	sInt64 diff=TimeValue.QuadPart-Time.TimeValue.QuadPart;

	return diff*FileTime_ToNanoSecond;
}
//---------------------------------------------------------------------------
sInt64 cNTFileTime::ToSystemTime(void)const
{
	sInt64 st=TimeValue.QuadPart-cnWindows::SystemTimeEpochAsFileTime.QuadPart;
	return st*FileTime_ToNanoSecond;
}
//---------------------------------------------------------------------------
void cNTFileTime::FromSystemTime(sInt64 st)
{
	TimeValue.QuadPart=st/FileTime_ToNanoSecond;
	TimeValue.QuadPart+=cnWindows::SystemTimeEpochAsFileTime.QuadPart;
}
//---------------------------------------------------------------------------
void cNTFileTime::FromTimeDue(iTimepoint *DueTime,sInt64 DueTimeDelay)
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
sInt64 cnWinRTL::TimepointSinceNTFileTimeNowNS(iTimepoint *Timepoint)
{
	cNTFileTime Now,Time;
	Now.SetNow();
	Time.FromTimepoint(Timepoint);
	sInt64 Nanoseconds=Time.DifferenceNanoseconds(Now);
	return Nanoseconds;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cNTFileTimePoint::cNTFileTimePoint()
{
}
//---------------------------------------------------------------------------
cNTFileTimePoint::~cNTFileTimePoint()
{
}
//---------------------------------------------------------------------------
iPtr<cNTFileTimePoint> cNTFileTimePoint::TimeNow(void)
{
	auto tp=iCreate<cNTFileTimePoint>();
	tp->SetNow();
	return tp;
}
//---------------------------------------------------------------------------
sInt64 cNTFileTimePoint::SystemTime(void)
{
	return ToSystemTime();
}
//---------------------------------------------------------------------------
sInt64 cNTFileTimePoint::SinceTime(iTimepoint *Time)
{
	cNTFileTime dt;
	dt.FromTimepoint(Time);
	
	return DifferenceNanoseconds(dt);
}
//---------------------------------------------------------------------------
FILETIME cNTFileTimePoint::GetFileTime(void)
{
	return FileTime;
}
//---------------------------------------------------------------------------
LARGE_INTEGER cNTFileTimePoint::GetTimeValue(void)
{
	return TimeValue;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
