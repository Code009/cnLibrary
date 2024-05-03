/*--- POSIX - Time --------------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2016-04-15                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLib_siOS_POSIX_Time_H__
#define __cnLib_siOS_POSIX_Time_H__

#include <time.h>
#include <siOS/POSIX/POSIXHeader.h>

#include <cnRTL/cnRTLCore.h>
#include <cnSystem/cnData.h>
/*-------------------------------------------------------------------------*/
#ifdef	__cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
namespace siPOSIX{
//---------------------------------------------------------------------------
#ifdef siOS_POSIX_ENABLE_TIME
//---------------------------------------------------------------------------
uInt64 timespecToNanoSeconds(const timespec &tv)noexcept(true);
timespec timespecFromNanoSeconds(sInt64 Seconds)noexcept(true);
uInt64 GetSystemTimeNow(void)noexcept(true);
//---------------------------------------------------------------------------
class cTimesepcTimepoint : public iTimepoint
{
public:
	cTimesepcTimepoint()noexcept(true);
	cTimesepcTimepoint(sInt64 NanoSecondsSinceUnixEPoch)noexcept(true);
	~cTimesepcTimepoint()noexcept(true);

	virtual sInt64 cnLib_FUNC SystemTime(void)noexcept(true)override;
	virtual sInt64 cnLib_FUNC SinceTime(iTimepoint *Time)noexcept(true)override;

	static void timespecFromTime(timespec &tv,iTimepoint *RefTime)noexcept(true);
protected:
	timespec fValue;
};
//---------------------------------------------------------------------------
#endif	// siOS_POSIX_ENABLE_TIME
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
}	// namespace siPOSIX
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#endif	// __cplusplus
/*-------------------------------------------------------------------------*/
#endif
