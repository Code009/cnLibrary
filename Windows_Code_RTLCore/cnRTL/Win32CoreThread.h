/*- CLib - CRT Atomic -----------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2024-06-04                                          */
/*-------------------------------------------------------------------------*/
#pragma once

#include <cnRTL\Win32Interlocked.h>
#include <cnRTL\Win32Thread.h>

/*-------------------------------------------------------------------------*/
#ifdef __cplusplus


//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------

struct TKRuntime::SystemThread
{
	typedef DWORD tThreadID;
	//typedef tThreadReference;
	
	static void Yield(void)noexcept(true){	::SwitchToThread();	}
	static void SpinWait(uIntn)noexcept(true){	::SwitchToThread();	}
	static void Suspend(uInt64 Duration)noexcept(true){	::Sleep(static_cast<DWORD>(Duration/1000000));	}
	
	static tThreadID GetID(void)noexcept(true){	return ::GetCurrentThreadId();	}
	
	typedef cnRTL::Win32RTLCore::cSingleNotification tSingleNotification;

};

//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
