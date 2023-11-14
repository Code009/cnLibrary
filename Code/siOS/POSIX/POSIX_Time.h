/*--- POSIX - Time --------------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2016-04-15                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLib_siOS_POSIX_Time_H__
#define __cnLib_siOS_POSIX_Time_H__

#include <time.h>

#include <cnSystem/cnData.h>
#include <cnRTL/Interface.h>
/*-------------------------------------------------------------------------*/
#ifdef	__cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
namespace siPOSIX{
//---------------------------------------------------------------------------
#if _POSIX_C_SOURCE >=200112L
//---------------------------------------------------------------------------
class cTimepoint : public iTimepoint
{
protected:
	timespec fValue;

	~cTimepoint();
public:
	cTimepoint();
	cTimepoint(sint64 NanoSecondsSinceUnixEPoch);

	virtual sint64 cnLib_FUNC SystemTime(void)const override;
	virtual sint64 cnLib_FUNC SinceTime(const iTimepoint *Time)const override;

	static sint64 timespecToNanoSeconds(const timespec &tv);
	static void NanoSecondsTotimespec(timespec &tv,sint64 Seconds);
	static void timespecFromTime(timespec &tv,const iTimepoint *RefTime);
};
#endif	//_POSIX_C_SOURCE >=200112L
//---------------------------------------------------------------------------
class cTimeNow : public iTimepoint
{
public:
	cTimeNow();
	~cTimeNow();

	virtual sint64 cnLib_FUNC SystemTime(void)const override;
	virtual sint64 cnLib_FUNC SinceTime(const iTimepoint *Time)const override;

};
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
