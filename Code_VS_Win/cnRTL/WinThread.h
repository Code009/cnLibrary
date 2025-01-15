/*- cnRTL - Windows - Thread ----------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2021-07-29                                          */
/*-------------------------------------------------------------------------*/
#pragma once
/*-------------------------------------------------------------------------*/
#include <cnRTL\WinCommon.h>
#include <cnRTL\Win32Thread.h>
#include <cnRTL\WinDateTime.h>

#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnRTL{
//---------------------------------------------------------------------------
namespace cnWinRTL{
//---------------------------------------------------------------------------
class cWinExclusiveFlag
{
public:
	cWinExclusiveFlag(bool InitalRun=false)noexcept(true);

	// Acquire
	//	test if caller acquired the flag, if not ,request owner to continue
	// return true if the flag is acquired
	bool Acquire(void)noexcept(true);
	// Release
	//	release the flag if no request pending
	//	if there was pending request, the thread should continue running
	//	this function must run called by owner
	// return true if the owner release the flag, false if there is pending request
	bool Release(void)noexcept(true);

	// Continue
	//	continue execution and clear pending state
	void Continue(void)noexcept(true);

	bool IsRunning(void)const noexcept(true);
private:
	static constexpr char rfIdle=0;
	static constexpr char rfExecute=1;
	static constexpr char rfPending=2;
	volatile char fRunFlag=rfIdle;
};
//---------------------------------------------------------------------------
class cWinTLS
{
public:
	cWinTLS()noexcept(true);
	~cWinTLS()noexcept(true);

	void* Get(void)noexcept(true);
	bool Set(void *Value)noexcept(true);
private:
	DWORD fTLSIndex;
};
//---------------------------------------------------------------------------
class cWinWaitObject
{
public:
	cWinWaitObject()noexcept(true);
	~cWinWaitObject()noexcept(true);

	void Acquire(void)noexcept(true);
	void Release(void)noexcept(true);

	bool Check(void)noexcept(true);
	void Wait(DWORD Millisecond)noexcept(true);
private:
	cAtomicVar<uIntn> fRefCount;
	HANDLE fWaitThreadHandle=nullptr;
	bool fWaitFlag;

	static VOID CALLBACK WaitNotifyAPC(ULONG_PTR)noexcept(true);
};
//---------------------------------------------------------------------------
class cWinWaitReference : public iReference, public cWinWaitObject
{
public:
	cWinWaitReference()noexcept(true);
	~cWinWaitReference()noexcept(true);
	virtual void cnLib_FUNC IncreaseReference(void)noexcept(true) override;
	virtual void cnLib_FUNC DecreaseReference(void)noexcept(true) override;
};
//---------------------------------------------------------------------------
class bcModuleReference : public bcRegisteredReference
{
public:
	bcModuleReference()noexcept(true);
	~bcModuleReference()noexcept(true);

	rPtr<iLibraryReference> QueryReference(iLibraryReferrer *Referrer,bool NoLoad)noexcept(true);

protected:
	virtual void ReferenceUpdate(void)noexcept(true)override final;
	virtual void ReferenceShutdown(void)noexcept(true)override final;

	virtual void ModuleInitialize(void)noexcept(true)=0;
	virtual void ModuleFinialize(void)noexcept(true)=0;
private:

	cnWinRTL::cCriticalSection fCS;
	bool fModuleActive;
	bool fModuleShutdown;
	cExclusiveFlag fReferenceProcessFlag;

	void ReferenceProcess(void)noexcept(true);
};
//---------------------------------------------------------------------------
#if _WIN32_WINNT >= _WIN32_WINNT_WIN7
//---------------------------------------------------------------------------
class cSRWLock
{
public:
	cSRWLock()noexcept(true);
	~cSRWLock()noexcept(true);

	void Acquire(void)noexcept(true);
	bool TryAcquire(void)noexcept(true);
	void Release(void)noexcept(true);
	void AcquireShared(void)noexcept(true);
	bool TryAcquireShared(void)noexcept(true);
	void ReleaseShared(void)noexcept(true);
protected:
	SRWLOCK fLock;
};
//---------------------------------------------------------------------------
#endif	// _WIN32_WINNT >= _WIN32_WINNT_WIN7
//---------------------------------------------------------------------------
VOID CALLBACK EmptyAPCFunction(ULONG_PTR dwParam)noexcept(true);
//---------------------------------------------------------------------------
class cThreadHandle : public iThread
{
public:
	cThreadHandle()noexcept(true);
	~cThreadHandle()noexcept(true);

	static int ConvertThreadPriorityFrom(sInt8 Priority)noexcept(true);
	static sfInt8 ConvertThreadPriorityTo(int Priority)noexcept(true);

	static bool SetPriority(HANDLE ThreadHandle,sfInt8 Priority)noexcept(true);
	static bool GetPriority(HANDLE ThreadHandle,sfInt8 &Priority)noexcept(true);
	static sfInt8 GetPriority(HANDLE ThreadHandle)noexcept(true);

	HANDLE GetThreadHandle(void)noexcept(true);
	bool ThreadIsCurrent(void)noexcept(true);

	// iThread

	virtual void cnLib_FUNC Wake(bool *ResetVal)noexcept(true)override;
	virtual bool cnLib_FUNC SetPriority(sfInt8 Priority)noexcept(true)override;
	virtual bool cnLib_FUNC GetPriority(sfInt8 &Priority)noexcept(true)override;
	virtual bool cnLib_FUNC IsCurrentThread(void)noexcept(true)override;
protected:

	HANDLE fThreadHandle;
	DWORD fThreadID;

	static VOID CALLBACK WakeAPCFunction(ULONG_PTR dwParam)noexcept(true);
};
//---------------------------------------------------------------------------
bool CurrentThreadSleepUntil(uInt64 SystemTime)noexcept(true);
bool CurrentThreadSleep(DWORD Milliseconds)noexcept(true);
//---------------------------------------------------------------------------
#if _WIN32_WINNT >= _WIN32_WINNT_VISTA
//---------------------------------------------------------------------------
class cNT6ThreadLocalVariable : public iThreadLocalVariable
{
public:
	cNT6ThreadLocalVariable()noexcept(true);
	~cNT6ThreadLocalVariable()noexcept(true);

	struct cThreadData
	{
		PTP_WAIT Waiter;
		HANDLE ThreadHandle;
		void *Object;
		iReference *Reference;
		iThreadExitNotifyProc *NotifyProc;
	};


	static VOID NTAPI WaitCallback(
		_Inout_     PTP_CALLBACK_INSTANCE Instance,
		_Inout_opt_ PVOID                 Context,
		_Inout_     PTP_WAIT              Wait,
		_In_        TP_WAIT_RESULT        WaitResult
    )noexcept(true);

	virtual void* cnLib_FUNC Get(void)noexcept(true)override;
	virtual void cnLib_FUNC Set(iReference *Reference,void *Data)noexcept(true)override;
	virtual void cnLib_FUNC Clear(void)noexcept(true)override;
	virtual void cnLib_FUNC SetThreadExitNotify(iThreadExitNotifyProc *NotifyProc)noexcept(true)override;

private:
	DWORD fTLSIndex;

	cThreadData* CreateThreadData(void)noexcept(true);
};
//---------------------------------------------------------------------------
struct cNT6TLSStaticThreadData
{
	PTP_WAIT Waiter;
	HANDLE ThreadHandle;
	void *Object;
	iReference *Reference;
	iThreadExitNotifyProc *NotifyProc;

	void* operator new(tSize Size)noexcept(true);
	void operator delete(void*p)noexcept(true);

	void Setup(void)noexcept(true);
	void Cancel(void)noexcept(true);
	void Clear(void)noexcept(true);
	static VOID NTAPI WaitCallback(
		_Inout_     PTP_CALLBACK_INSTANCE Instance,
		_Inout_opt_ PVOID                 Context,
		_Inout_     PTP_WAIT              Wait,
		_In_        TP_WAIT_RESULT        WaitResult
    )noexcept(true);
};
//---------------------------------------------------------------------------
template<class T>
class cNT6TLSStaticPointer : public iThreadLocalVariable
{
public:
	~cNT6TLSStaticPointer()noexcept(true){
		// destroy data in current thread, since the thread will last after the library destruct
		cNT6TLSStaticThreadData *ThreadData=gDataTLS;
		if(ThreadData==nullptr){
			return;
		}

		ThreadData->Cancel();
		ThreadData->Clear();
		delete ThreadData;
		gDataTLS=nullptr;
	}

	virtual void cnLib_FUNC IncreaseReference(void)noexcept(true)override{}
	virtual void cnLib_FUNC DecreaseReference(void)noexcept(true)override{}

	virtual void* cnLib_FUNC Get(void)noexcept(true)override{
		cNT6TLSStaticThreadData *ThreadData=gDataTLS;
		if(ThreadData==nullptr)
			return nullptr;
		return ThreadData->Object;
	}

	virtual void cnLib_FUNC Set(iReference *Reference,void *Data)noexcept(true)override{
		cNT6TLSStaticThreadData *ThreadData=gDataTLS;
		if(ThreadData==nullptr){
			if(Reference==nullptr && Data==nullptr){
				return;
			}
			ThreadData=CreateThreadData();
		}
		else{
			if(ThreadData->Reference!=nullptr){
				ThreadData->Reference->DecreaseReference();
			}
		}
		ThreadData->Object=Data;
		ThreadData->Reference=Reference;
		if(Reference!=nullptr){
			Reference->IncreaseReference();
		}
	}
	virtual void cnLib_FUNC Clear(void)noexcept(true)override{
		cNT6TLSStaticThreadData *ThreadData=gDataTLS;
		if(ThreadData==nullptr){
			return;
		}
		if(ThreadData->Reference!=nullptr){
			ThreadData->Reference->DecreaseReference();
		}
		ThreadData->Object=nullptr;
		ThreadData->Reference=nullptr;
		ThreadData->NotifyProc=nullptr;
	}
	virtual void cnLib_FUNC SetThreadExitNotify(iThreadExitNotifyProc *NotifyProc)noexcept(true)override{
		cNT6TLSStaticThreadData *ThreadData=gDataTLS;
		if(ThreadData==nullptr){
			if(NotifyProc==nullptr){
				return;
			}
			ThreadData=CreateThreadData();
		}
		ThreadData->NotifyProc=NotifyProc;
	}

private:
	static __declspec(thread) cNT6TLSStaticThreadData *gDataTLS;

	static cNT6TLSStaticThreadData* CreateThreadData(void)noexcept(true){
		auto ThreadData=new cNT6TLSStaticThreadData;
		ThreadData->Setup();
		gDataTLS=ThreadData;
		return ThreadData;
	}

};
//---------------------------------------------------------------------------
template<class T> __declspec(thread) cNT6TLSStaticThreadData* cNT6TLSStaticPointer<T>::gDataTLS=nullptr;
//---------------------------------------------------------------------------
class cNT6ThreadPoolHandleWaiter : public iThreadPoolHandleWaiter, public cnRTL::cDualReference
{
public:
	cNT6ThreadPoolHandleWaiter(PTP_CALLBACK_ENVIRON env,iReference *Reference,iFunction<void (DWORD WaitResult)noexcept(true)> *Callback)noexcept(true);
	~cNT6ThreadPoolHandleWaiter()noexcept(true);

	virtual void SetWait(HANDLE WaitHandle,PFILETIME Timeout)noexcept(true)override;

protected:
	void VirtualStopped(void)noexcept(true);
	PTP_CALLBACK_ENVIRON fTPEnv;
	PTP_WAIT fWaiter;
	static VOID CALLBACK WorkProc(
	  _Inout_     PTP_CALLBACK_INSTANCE Instance,
	  _Inout_opt_ PVOID                 Context,
	  _Inout_     PTP_WAIT              Wait,
	  _In_        TP_WAIT_RESULT        WaitResult
	)noexcept(true);
	iReference *fProcedureReference;
	iFunction<void (DWORD)noexcept(true)> *fProcedure;

	cnRTL::cAtomicVar<bool> fSetting;
	bool fWaiterIsSet;

	void CleanupCallback(void)noexcept(true);
	void WaitForCallback(void)noexcept(true);
	static VOID CALLBACK WaitCallbackProc(
		_Inout_     PTP_CALLBACK_INSTANCE Instance,
		_Inout_opt_ PVOID                 Context
	)noexcept(true);
};
//---------------------------------------------------------------------------
//class cNT6ThreadPoolHandleRepeatWaiter : public iThreadPoolHandleRepeatWaiter
//{
//public:
//
//	cNT6ThreadPoolHandleRepeatWaiter(PTP_CALLBACK_ENVIRON env,iReference *Reference,iProcedure *Callback);
//	~cNT6ThreadPoolHandleRepeatWaiter(void);
//
//	virtual void SetWaitHandle(HANDLE WaitHandle)override;
//
//protected:
//	PTP_WAIT fWaiter;
//	HANDLE fWaitHandle;
//	static VOID CALLBACK WorkProc(
//	  _Inout_     PTP_CALLBACK_INSTANCE Instance,
//	  _Inout_opt_ PVOID                 Context,
//	  _Inout_     PTP_WAIT              Wait,
//	  _In_        TP_WAIT_RESULT        WaitResult
//	);
//	void WorkProc(void);
//	iReference *fProcedureReference;
//	iProcedure *fProcedure;
//};
//---------------------------------------------------------------------------
class cNT6ThreadPoolWorkAsyncProcedure : public iAsyncProcedure
{
public:
	cNT6ThreadPoolWorkAsyncProcedure(PTP_CALLBACK_ENVIRON env,iReference *Reference,iProcedure *Procedure)noexcept(true);
	~cNT6ThreadPoolWorkAsyncProcedure()noexcept(true);

	virtual void cnLib_FUNC Start(void)noexcept(true)override;
protected:
	PTP_WORK fWork;
	iReference *fProcedureReference;
	iProcedure *fProcedure;

	static VOID CALLBACK WorkProc(
		_Inout_     PTP_CALLBACK_INSTANCE Instance,
		_Inout_opt_ PVOID                 Context,
		_Inout_     PTP_WORK              Work
	)noexcept(true);

};
//---------------------------------------------------------------------------
class cNT6ThreadPoolTimerAsyncTimer : public iAsyncTimer, public cnRTL::cDualReference
{
public:
	cNT6ThreadPoolTimerAsyncTimer(PTP_CALLBACK_ENVIRON env,iReference *Reference,iProcedure *Procedure)noexcept(true);
	~cNT6ThreadPoolTimerAsyncTimer(void)noexcept(true);

	virtual void cnLib_FUNC Start(uInt64 DueTime,uInt64 Period)noexcept(true)override;
	virtual void cnLib_FUNC Stop(void)noexcept(true)override;

protected:
	void VirtualStopped(void)noexcept(true);

	PTP_CALLBACK_ENVIRON fTPEnv;
	PTP_TIMER fTimer;
	iReference *fProcedureReference;
	iProcedure *fProcedure;

	cnRTL::cAtomicVar<bool> fSetting;
	bool fTimerIsSet;
	static VOID CALLBACK WorkProc(
		_Inout_     PTP_CALLBACK_INSTANCE Instance,
		_Inout_opt_ PVOID                 Context,
		_Inout_     PTP_TIMER             Timer
	)noexcept(true);

	void CleanupTimerCallback(void)noexcept(true);
	void WaitForTimer(void)noexcept(true);
	static VOID CALLBACK WaitTimerProc(
		_Inout_     PTP_CALLBACK_INSTANCE Instance,
		_Inout_opt_ PVOID                 Context
	)noexcept(true);

};
//---------------------------------------------------------------------------
class bcNT6ThreadPoolEnvironment : public iThreadPool
{
public:
	bcNT6ThreadPoolEnvironment(PTP_CALLBACK_ENVIRON Env)noexcept(true);
	~bcNT6ThreadPoolEnvironment()noexcept(true);

	typedef cNT6ThreadPoolHandleWaiter cHandleWaiter;
	//typedef cNT6ThreadPoolHandleRepeatWaiter cHandleRepeatWaiter;

	static void StartWork(PTP_CALLBACK_ENVIRON Env,iReference *Reference,iProcedure *Procedure)noexcept(true);
	
	virtual void cnLib_FUNC Execute(iReference *Reference,iProcedure *Procedure)noexcept(true)override;
	virtual rPtr<iAsyncProcedure>	cnLib_FUNC CreateWork(iReference *Reference,iProcedure *Procedure)noexcept(true)override;
	virtual rPtr<iAsyncTimer>		cnLib_FUNC CreateTimer(iReference *Reference,iProcedure *Procedure)noexcept(true)override;

protected:

	PTP_CALLBACK_ENVIRON fEnv;

	struct SimpleWorkData : cRTLAllocator
	{
		rPtr<iReference> Reference;
		iProcedure *Procedure;
	};

	static VOID CALLBACK SimpleWork(
		_Inout_     PTP_CALLBACK_INSTANCE Instance,
		_Inout_opt_ PVOID                 Context
	)noexcept(true);

};
//---------------------------------------------------------------------------
class cNT6ThreadPoolEnvironment : public bcNT6ThreadPoolEnvironment
{
public:
	cNT6ThreadPoolEnvironment()noexcept(true);
	~cNT6ThreadPoolEnvironment()noexcept(true);

protected:
	TP_CALLBACK_ENVIRON fEnvironment;
	PTP_POOL fPool;
};
//---------------------------------------------------------------------------
#endif	// _WIN32_WINNT >= _WIN32_WINNT_VISTA
//---------------------------------------------------------------------------
}	// namespace cnWinRTL
//---------------------------------------------------------------------------
}	// namespace cnRTL
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
