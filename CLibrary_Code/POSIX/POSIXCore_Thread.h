/*--- POSIX - RTLCore - Synchronization -----------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2025-02-20                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLib_POSIX_POSIXCore_Thread_H__
#define __cnLib_POSIX_POSIXCore_Thread_H__

#include <POSIX/POSIXHeader.h>

#include <cnTK/Common.hpp>

/*-------------------------------------------------------------------------*/
#ifdef	__cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
namespace siPOSIX{
//---------------------------------------------------------------------------
#ifdef	siOS_POSIX_ENABLE_THREAD
//---------------------------------------------------------------------------
namespace RTLCore{
//---------------------------------------------------------------------------
struct cSingleNotification
{
	pthread_mutex_t Mutex;
	pthread_cond_t Condition;
	struct cWaitFlag
	{
		bool Notified;

		bool operator () ()noexcept(true){
			return Notified;
		}
	}WaitFlag;
	void Setup(void)noexcept(true);
	void Clear(void)noexcept(true);
	void Wait(void)noexcept(true);
	bool WaitFor(ufInt64 Duration)noexcept(true);
	void Notify(void)noexcept(true);
};
//---------------------------------------------------------------------------
}	// namespace RTLCore
//---------------------------------------------------------------------------
#endif	// siOS_POSIX_ENABLE_THREAD
//---------------------------------------------------------------------------
}	// namespace siPOSIX
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#endif	// __cplusplus
/*-------------------------------------------------------------------------*/
#endif
