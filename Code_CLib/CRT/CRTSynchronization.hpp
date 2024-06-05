/*- CLib - CRT Synchronization --------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2024-06-04                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_CLib_CRT_CRTSynchronization_H__
#define __cnLibrary_CLib_CRT_CRTSynchronization_H__

/*-------------------------------------------------------------------------*/
#ifdef __cplusplus

//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
struct TKRuntime::ThreadNotification
{
	std::mutex SignalMutex;
	std::condition_variable Signal;

	void Start(void){
		Notified=false;
	}
	void Finish(void){
	}
	void Wait(void){
		std::unique_lock<std::mutex> lk(SignalMutex);
		while(Notified==false){
			Signal.wait(lk);
		}
	}
	void Notify(void){
		{
			std::unique_lock<std::mutex> lk(SignalMutex);
			Notified=true;
		}
		Signal.notify_one();
	}

	bool Notified;
};


//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif
