/*- CLib - CRT Atomic -----------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2024-06-04                                          */
/*-------------------------------------------------------------------------*/
#pragma once

#include <cnRTL\Win32Interlocked.h>

/*-------------------------------------------------------------------------*/
#ifdef __cplusplus


//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------

struct TKRuntime::Thread
{
	typedef DWORD tThreadID;
	//typedef tThreadReference;
	
	static void Yield(void)noexcept(true){	::SwitchToThread();	}
	static void SpinWait(uIntn)noexcept(true){	::SwitchToThread();	}
	static void Suspend(uInt64 Duration)noexcept(true){	::Sleep(static_cast<DWORD>(Duration/1000000));	}
	
	static tThreadID GetID(void)noexcept(true){	return ::GetCurrentThreadId();	}
	
	class tSingleNotification
	{
	public:
		void Setup(void)noexcept(true){
			HANDLE ProcessHandle=::GetCurrentProcess();
			::DuplicateHandle(ProcessHandle,::GetCurrentThread(),ProcessHandle,&fNotifyThreadHandle,0,FALSE,DUPLICATE_SAME_ACCESS);
			fNotifyFlag=2;
			fAPCFlag=0;
		}
		void Clear(void)noexcept(true){
			if(fNotifyFlag==1){
				// clear pending APC
				WaitMS(INFINITE);
			}
			::CloseHandle(fNotifyThreadHandle);
		}
		void Wait(void)noexcept(true){
			WaitMS(INFINITE);
		}
		bool Wait(ufInt64 Duration)noexcept(true){
			return WaitMS(static_cast<DWORD>(Duration/1000000));
		}
		void Notify(void)noexcept(true){
			
			if(cnRTL::cnWinRTL::Interlocked::CmpXchg(fNotifyFlag,1,2)==2){
				::QueueUserAPC(WaitNotifyAPC,fNotifyThreadHandle,reinterpret_cast<ULONG_PTR>(this));
			}
		}
	
	private:
		HANDLE fNotifyThreadHandle;
		volatile CHAR fNotifyFlag;
		CHAR fAPCFlag;
		static VOID CALLBACK WaitNotifyAPC(ULONG_PTR Param)noexcept(true){
			auto This=reinterpret_cast<tSingleNotification*>(Param);
			This->fAPCFlag=1;
			This->fNotifyFlag=0;
		}

		bool WaitMS(DWORD Milliseconds)noexcept(true){
			while(fAPCFlag==0){
				DWORD WaitRet=SleepEx(Milliseconds,TRUE);
				if(WaitRet==0){
					// timeout
					return false;
				}
			}
			return true;
		}
	};
};

//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
