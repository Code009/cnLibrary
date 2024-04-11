/*- cnRTL - Windows - DateTime --------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2008-06-05                                          */
/*-------------------------------------------------------------------------*/
#pragma once
/*-------------------------------------------------------------------------*/
#include <cnRTL\WinCommon.h>

#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnRTL{
//---------------------------------------------------------------------------
namespace cnWinRTL{
//---------------------------------------------------------------------------
void NTFileTimeToDateTime(cDateTime &DateTime,const FILETIME &FileTime)noexcept(true);
//---------------------------------------------------------------------------
struct cNTFileTime
{
	union{
		FILETIME FileTime;
		LARGE_INTEGER TimeValue;
	};

	void SetNow(void)noexcept(true);
	bool ToDateTime(cDateTime &DateTime)const noexcept(true);
	bool FromDateTime(const cDateTime &DateTime)noexcept(true);
	void FromTimepoint(iTimepoint *Time)noexcept(true);
	void FromTimepoint(iTimepoint *Time,sInt64 Delay)noexcept(true);
	void FromTimeDue(iTimepoint *DueTime,sInt64 DueTimeDelay)noexcept(true);

	void AdjustSeconds(sInt64 Seconds)noexcept(true);
	void AdjustNanoSeconds(sInt64 Seconds)noexcept(true);

	sInt64 DifferenceNanoseconds(const cNTFileTime &Time)const noexcept(true);
	Float64 DifferenceSeconds(const cNTFileTime &Time)const noexcept(true);
	//Float64 ReleativeToUNIXEpoch(void)const noexcept(true);

	uInt64 ToSystemTime(void)const noexcept(true);
	void FromSystemTime(uInt64 SystemTime)noexcept(true);


};
//---------------------------------------------------------------------------
class cNTFileTimePoint : public iFileTime, public cNTFileTime
{
public:
	cNTFileTimePoint()noexcept(true);
	~cNTFileTimePoint()noexcept(true);

	static iPtr<cNTFileTimePoint> TimeNow(void)noexcept(true);

	virtual sInt64 cnLib_FUNC SystemTime(void)noexcept(true)override;
	virtual sInt64 cnLib_FUNC SinceTime(iTimepoint *Time)noexcept(true)override;

	virtual FILETIME cnLib_FUNC GetFileTime(void)noexcept(true)override;
	virtual LARGE_INTEGER cnLib_FUNC GetTimeValue(void)noexcept(true)override;
};
//---------------------------------------------------------------------------
sInt64 SystemTimeSinceNTFileTimeNowNS(uInt64 SystemTime)noexcept(true);
//---------------------------------------------------------------------------
}	// namespace cnWinRTL
//---------------------------------------------------------------------------
}	// namespace cnRTL
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
