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
void NTFileTimeToDateTime(cDateTime &DateTime,const FILETIME &FileTime);
//---------------------------------------------------------------------------
struct cNTFileTime
{
	union{
		FILETIME FileTime;
		LARGE_INTEGER TimeValue;
	};

	void SetNow(void);
	bool ToDateTime(cDateTime &DateTime)const;
	bool FromDateTime(const cDateTime &DateTime);
	void FromTimepoint(iTimepoint *Time);
	void FromTimepoint(iTimepoint *Time,sInt64 Delay);
	void FromTimeDue(iTimepoint *DueTime,sInt64 DueTimeDelay);

	void AdjustSeconds(sInt64 Seconds);
	void AdjustNanoSeconds(sInt64 Seconds);

	sInt64 DifferenceNanoseconds(const cNTFileTime &Time)const;
	Float64 DifferenceSeconds(const cNTFileTime &Time)const;
	//Float64 ReleativeToUNIXEpoch(void)const;

	sInt64 ToSystemTime(void)const;
	void FromSystemTime(sInt64 SystemTime);


};
//---------------------------------------------------------------------------
class cNTFileTimePoint : public iFileTime, public cNTFileTime
{
public:
	cNTFileTimePoint();
	~cNTFileTimePoint();

	static iPtr<cNTFileTimePoint> TimeNow(void);

	virtual sInt64 cnLib_FUNC SystemTime(void)override;
	virtual sInt64 cnLib_FUNC SinceTime(iTimepoint *Time)override;

	virtual FILETIME cnLib_FUNC GetFileTime(void)override;
	virtual LARGE_INTEGER cnLib_FUNC GetTimeValue(void)override;
};
//---------------------------------------------------------------------------
sInt64 TimepointSinceNTFileTimeNowNS(iTimepoint *Timepoint);
//---------------------------------------------------------------------------
}	// namespace cnWinRTL
//---------------------------------------------------------------------------
}	// namespace cnRTL
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
