/*- CLib - CRT Synchronization --------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2024-06-04                                          */
/*-------------------------------------------------------------------------*/
#pragma once

/*-------------------------------------------------------------------------*/
#ifdef __cplusplus

//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
struct TKRuntime::ThreadNotification
{
	CRITICAL_SECTION SignalMutex;
	CONDITION_VARIABLE Signal;

	ThreadNotification()noexcept
		: Signal(CONDITION_VARIABLE_INIT)
	{
		::InitializeCriticalSection(&SignalMutex);
	}
	~ThreadNotification()noexcept{
		::DeleteCriticalSection(&SignalMutex);
	}
	void Start(void)noexcept{
		Notified=false;
	}
	void Finish(void)noexcept{
	}
	void Wait(void)noexcept{
	
		::EnterCriticalSection(&SignalMutex);
		while(Notified==false){
			::SleepConditionVariableCS(&Signal,&SignalMutex,INFINITE);
		}
		::LeaveCriticalSection(&SignalMutex);
	}
	void Notify(void){
		::EnterCriticalSection(&SignalMutex);
		Notified=true;
		::WakeConditionVariable(&Signal);
		::LeaveCriticalSection(&SignalMutex);
	}

	bool Notified;
};


//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
