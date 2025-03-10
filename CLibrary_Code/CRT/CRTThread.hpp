/*- CLib - CRT Atomic -----------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2024-06-04                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_CLib_CRT_CRTThread_H__
#define __cnLibrary_CLib_CRT_CRTThread_H__

/*-------------------------------------------------------------------------*/
#ifdef __cplusplus

//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------

struct TKRuntime::SystemThread
{
	typedef std::thread::id tThreadID;
	//typedef tThreadReference;
	
	static void Yield(void)noexcept(true){	std::this_thread::yield();	}
	static void SpinWait(uIntn)noexcept(true){	std::this_thread::yield();	}
	static void Suspend(uInt64 Duration)noexcept(true){	std::this_thread::sleep_for(std::chrono::nanoseconds(Duration));	}
	
	static tThreadID GetID(void)noexcept(true){	return std::this_thread::get_id();	}
	
	struct tSingleNotification
	{
		struct cContext
		{
			std::mutex Mutex;
			std::condition_variable CV;
		};
		cnVar::cStaticVariable<cContext> Context;
		struct cWaitFlag
		{
			bool Notified;

			bool operator () ()noexcept(true){
				return Notified;
			}
		}WaitFlag;
		void Setup(void)noexcept(true){		Context.Construct();	WaitFlag.Notified=false;	}
		void Clear(void)noexcept(true){		Context.Destruct();	}
		void Wait(void)noexcept(true){		std::unique_lock Lock(Context->Mutex);	Context->CV.wait(Lock,WaitFlag);	}
		bool WaitFor(ufInt64 Duration)noexcept(true){		std::unique_lock Lock(Context->Mutex);	return Context->CV.wait_for(Lock,std::chrono::nanoseconds(Duration),WaitFlag);		}
		void Notify(void)noexcept(true){	{std::unique_lock lk(Context->Mutex);	WaitFlag.Notified=true;}	Context->CV.notify_one();		}
	};
};

//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif
