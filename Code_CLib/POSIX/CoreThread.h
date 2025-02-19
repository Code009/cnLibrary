/*- CLib - CRT Atomic -----------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2024-06-04                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_CLib_POSIX_CoreThread_H__
#define __cnLibrary_CLib_POSIX_CoreThread_H__

/*-------------------------------------------------------------------------*/
#ifdef __cplusplus

//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------

struct TKRuntime::Thread
{
	struct tThreadID
	{
		pthread_t IDValue;


		bool operator ==(const tThreadID &Src)noexcept(true);
		bool operator !=(const tThreadID &Src)noexcept(true);
	};
	
	static void Yield(void)noexcept(true);
	static void SpinWait(uIntn SpinCount)noexcept(true);
	static void Suspend(uInt64 Duration)noexcept(true);
	
	static tThreadID GetID(void)noexcept(true);
	
	struct tSingleNotification
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
		bool Wait(ufInt64 Duration)noexcept(true);
		void Notify(void)noexcept(true);
	};
};

//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif
