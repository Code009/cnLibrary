/*--- Mach - Time ---------------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2016-04-29                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLib_siOS_Mach_Time_H__
#define __cnLib_siOS_Mach_Time_H__

#include <mach/mach.h>

#include <cnSystem/cnSystem.h>
/*-------------------------------------------------------------------------*/
#ifdef	__cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
namespace siMach{
//---------------------------------------------------------------------------
class iMachTimespec : public iTimepoint
{
public:
	cnLib_INTERFACE_DEFINE(iMachTimespec,iTimepoint);

	virtual const mach_timespec& MachTimespec(void)=0;
};
//---------------------------------------------------------------------------
class cTimespec : public iMachTimespec
{
public:
	cTimespec();
	cTimespec(Float64 SecondsSinceUnixEPoch);
	~cTimespec();

	virtual sInt64 cnLib_FUNC SystemTime(void)const override;
	virtual sInt64 cnLib_FUNC SinceTime(const iTimepoint *Time)const override;

	// iMachTimespec

	virtual const mach_timespec& MachTimespec(void)override;

	void Set(const iTimepoint *RefTime,Float64 Difference);
protected:
	mach_timespec fValue;
	static constexpr uIntn NanoSeconds=1'000'000'000;

	static void GetTimeNow(mach_timespec &tv);
	static Float64 TimespecToSeconds(const mach_timespec &tv);
	static void SecondsToTimespec(mach_timespec &tv,Float64 Seconds);
	static void TimespecFromTime(mach_timespec &tv,const iTimepoint *RefTime);

};
//---------------------------------------------------------------------------
}	// namespace siMach
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#endif	// __cplusplus
/*-------------------------------------------------------------------------*/
#endif
