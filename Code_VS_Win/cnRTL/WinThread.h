/*- cnRTL - Windows - Thread ----------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2021-07-29                                          */
/*-------------------------------------------------------------------------*/
#pragma once
/*-------------------------------------------------------------------------*/
#include <cnRTL\WinCommon.h>
#include <cnRTL\WinDateTime.h>

#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnRTL{
//---------------------------------------------------------------------------
namespace cnWinRTL{
//---------------------------------------------------------------------------
namespace Interlocked{
//---------------------------------------------------------------------------
template<uIntn Size>
struct _Func;
//---------------------------------------------------------------------------
template<>
struct _Func<1>
{
	typedef CHAR Type;
	static CHAR Inc(volatile CHAR &Dest);
	static CHAR Dec(volatile CHAR &Dest);
	static CHAR Xchg(volatile CHAR &Dest,CHAR Src);
	static CHAR CmpXchg(volatile CHAR &Dest,CHAR Src,CHAR Compare);
};
//---------------------------------------------------------------------------
template<>
struct _Func<2>
{
	typedef SHORT Type;
	static SHORT Inc(volatile SHORT &Dest);
	static SHORT Dec(volatile SHORT &Dest);
	static SHORT Xchg(volatile SHORT &Dest,SHORT Src);
	static SHORT CmpXchg(volatile SHORT &Dest,SHORT Src,SHORT Compare);
};
//---------------------------------------------------------------------------
template<>
struct _Func<4>
{
	typedef LONG Type;
	static LONG Inc(volatile LONG &Dest);
	static LONG Dec(volatile LONG &Dest);
	static LONG Xchg(volatile LONG &Dest,LONG Src);
	static LONG CmpXchg(volatile LONG &Dest,LONG Src,LONG Compare);
};
//---------------------------------------------------------------------------
template<>
struct _Func<8>
{
	typedef LONG64 Type;
	static LONG64 Inc(volatile LONG64 &Dest);
	static LONG64 Dec(volatile LONG64 &Dest);
	static LONG64 Xchg(volatile LONG64 &Dest,LONG64 Src);
	static LONG64 CmpXchg(volatile LONG64 &Dest,LONG64 Src,LONG64 Compare);
};
//---------------------------------------------------------------------------
// Inc
template<class TInt>
TInt Inc(volatile TInt &Dest){
	typedef _Func<sizeof(TInt)> FuncType;
	return static_cast<TInt>( FuncType::Inc(reinterpret_cast<typename FuncType::Type volatile&>(Dest)) );
}
// Dec
template<class TInt>
TInt Dec(volatile TInt &Dest){
	typedef _Func<sizeof(TInt)> FuncType;
	return static_cast<TInt>( FuncType::Dec(reinterpret_cast<typename FuncType::Type volatile&>(Dest)) );
}

// Xchg
template<class TInt>
TInt Xchg(volatile TInt &Dest,cnVar::TypeDef<TInt> Src){
	typedef _Func<sizeof(TInt)> FuncType;
	return static_cast<TInt>( FuncType::Xchg(reinterpret_cast<typename FuncType::Type volatile&>(Dest),reinterpret_cast<typename FuncType::Type const&>(Src)) );
}

// CmpXchg
template<class TInt>
TInt CmpXchg(volatile TInt &Dest,cnVar::TypeDef<TInt> Src,cnVar::TypeDef<TInt> Compare){
	typedef _Func<sizeof(TInt)> FuncType;
	return static_cast<TInt>( FuncType::CmpXchg(reinterpret_cast<typename FuncType::Type volatile&>(Dest),reinterpret_cast<typename FuncType::Type const&>(Src),reinterpret_cast<typename FuncType::Type const&>(Compare)) );
}
//---------------------------------------------------------------------------
}	// namespace Interlocked
//---------------------------------------------------------------------------
class cWinExclusiveFlag
{
public:
	cWinExclusiveFlag(bool InitalRun=false);

	// Acquire
	//	test if caller acquired the flag, if not ,request owner to continue
	// return true if the flag is acquired
	bool Acquire(void);
	// Release
	//	release the flag if no request pending
	//	if there was pending request, the thread should continue running
	//	this function must run called by owner
	// return true if the owner release the flag, false if there is pending request
	bool Release(void);

	// Continue
	//	continue execution and clear pending state
	void Continue(void);

	bool IsRunning(void)const;
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
	cWinTLS();
	~cWinTLS();

	void* Get(void);
	bool Set(void *Value);
private:
	DWORD fTLSIndex;
};
//---------------------------------------------------------------------------
class cWinWaitObject
{
public:
	cWinWaitObject();
	~cWinWaitObject();

	void Acquire(void);
	void Release(void);

	bool Check(void);
	void Wait(DWORD Millisecond);
private:
	cAtomicVar<uIntn> fRefCount;
	HANDLE fWaitThreadHandle=nullptr;
	bool fWaitFlag;

	static VOID CALLBACK WaitNotifyAPC(ULONG_PTR);
};
//---------------------------------------------------------------------------
class cWinWaitReference : public iReference, public cWinWaitObject
{
public:
	cWinWaitReference();
	~cWinWaitReference();
	virtual void cnLib_FUNC IncreaseReference(void)noexcept(true) override;
	virtual void cnLib_FUNC DecreaseReference(void)noexcept(true) override;
};
//---------------------------------------------------------------------------
class cCriticalSection 
{
public:
	cCriticalSection();
	~cCriticalSection();

	cCriticalSection(const cCriticalSection&)=delete;

	void Acquire(void);
	bool TryAcquire(void);
	void Release(void);

protected:
	CRITICAL_SECTION fCriticalSection;
};
//---------------------------------------------------------------------------
class cDependentRegistration : public iDependentRegistration
{
public:
	cDependentRegistration();
	~cDependentRegistration();

	void Shutdown(void);

	virtual void cnLib_FUNC Register(iDependentInfo *Dependent)override;
	virtual void cnLib_FUNC Unregister(iDependentInfo *Dependent)override;
protected:
	cCriticalSection fCS;
	cWinWaitObject fWaitObject;
	class cnLib_INTERFACE bcNotifyToken
	{
	public:
		virtual rPtr< iArrayReference<const uChar16> > cnLib_FUNC DependentCreateDescription(void)=0;
		virtual void cnLib_FUNC DependentShutdownNotification(void)=0;

		bcNotifyToken *Parent;
		bcNotifyToken *Child[2];
		ufInt8 Branch;
		ufInt8 Color;
	};
	static_assert(sizeof(bcNotifyToken)<=sizeof(iDependentInfo),"incompatiable iDependentInfo");

	void Register(bcNotifyToken *Token);
	void Unregister(bcNotifyToken *Token);

	struct cRemovingItem
	{
		bcNotifyToken *Token;
		bool Removed;
	};
	cLinkItemSet<bcNotifyToken,cnDataStruct::cItemAddressOrderOperator<bcNotifyToken> > fDependentSet;
	cArrayBlock<cRemovingItem> *fShutdowningItems=nullptr;
	bool fShutDown=false;
};
//---------------------------------------------------------------------------
#if _WIN32_WINNT >= _WIN32_WINNT_WIN7
//---------------------------------------------------------------------------
class cSRWLock
{
public:
	cSRWLock();
	~cSRWLock();

	void Acquire(void);
	bool TryAcquire(void);
	void Release(void);
	void AcquireShared(void);
	bool TryAcquireShared(void);
	void ReleaseShared(void);
protected:
	SRWLOCK fLock;
};
//---------------------------------------------------------------------------
#endif	// _WIN32_WINNT >= _WIN32_WINNT_WIN7
//---------------------------------------------------------------------------
VOID CALLBACK EmptyAPCFunction(ULONG_PTR dwParam);
//---------------------------------------------------------------------------
class cThreadHandle : public iThread, private iDependentInfo
{
public:
	cThreadHandle();
	~cThreadHandle();

	static int ConvertThreadPriorityFrom(sInt8 Priority);
	static sfInt8 ConvertThreadPriorityTo(int Priority);

	static bool SetPriority(HANDLE ThreadHandle,sfInt8 Priority);
	static bool GetPriority(HANDLE ThreadHandle,sfInt8 &Priority);
	static sfInt8 GetPriority(HANDLE ThreadHandle);

	HANDLE GetThreadHandle(void);
	bool ThreadIsCurrent(void);

	// iThread

	virtual void cnLib_FUNC Wake(bool *ResetVal)override;
	virtual bool cnLib_FUNC SetPriority(sfInt8 Priority)override;
	virtual bool cnLib_FUNC GetPriority(sfInt8 &Priority)override;
	virtual bool cnLib_FUNC IsCurrentThread(void)override;
protected:

	HANDLE fThreadHandle;
	DWORD fThreadID;

	static VOID CALLBACK WakeAPCFunction(ULONG_PTR dwParam);

	virtual rPtr< iArrayReference<const uChar16> > cnLib_FUNC DependentCreateDescription(void)override;
	virtual void cnLib_FUNC DependentShutdownNotification(void)override;
};
//---------------------------------------------------------------------------
bool CurrentThreadSleepUntil(iTimepoint *Time,uInt64 Delay);
bool CurrentThreadSleep(DWORD Milliseconds);
//---------------------------------------------------------------------------
#if _WIN32_WINNT >= _WIN32_WINNT_VISTA
//---------------------------------------------------------------------------
class cNT6ThreadLocalVariable : public iThreadLocalVariable
{
public:
	cNT6ThreadLocalVariable();
	~cNT6ThreadLocalVariable();

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
    );

	virtual void* cnLib_FUNC Get(void)noexcept(true)override;
	virtual void cnLib_FUNC Set(iReference *Reference,void *Data)noexcept(true)override;
	virtual void cnLib_FUNC Clear(void)noexcept(true)override;
	virtual void cnLib_FUNC SetThreadExitNotify(iThreadExitNotifyProc *NotifyProc)noexcept(true)override;

private:
	DWORD fTLSIndex;

	cThreadData* CreateThreadData(void);
};
//---------------------------------------------------------------------------
struct cNT6TLSStaticThreadData
{
	PTP_WAIT Waiter;
	HANDLE ThreadHandle;
	void *Object;
	iReference *Reference;
	iThreadExitNotifyProc *NotifyProc;

	void* operator new(tSize Size);
	void operator delete(void*p);

	void Setup(void);
	static VOID NTAPI WaitCallback(
		_Inout_     PTP_CALLBACK_INSTANCE Instance,
		_Inout_opt_ PVOID                 Context,
		_Inout_     PTP_WAIT              Wait,
		_In_        TP_WAIT_RESULT        WaitResult
    );
};
//---------------------------------------------------------------------------
template<class T>
class cNT6TLSStaticPointer : public iThreadLocalVariable
{
public:

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
	cNT6ThreadPoolHandleWaiter(PTP_CALLBACK_ENVIRON env,iReference *Reference,iFunction<void (DWORD WaitResult)> *Callback);
	~cNT6ThreadPoolHandleWaiter();

	virtual void SetWait(HANDLE WaitHandle,PFILETIME Timeout)override;

protected:
	void VirtualStopped(void);
	PTP_CALLBACK_ENVIRON fTPEnv;
	PTP_WAIT fWaiter;
	static VOID CALLBACK WorkProc(
	  _Inout_     PTP_CALLBACK_INSTANCE Instance,
	  _Inout_opt_ PVOID                 Context,
	  _Inout_     PTP_WAIT              Wait,
	  _In_        TP_WAIT_RESULT        WaitResult
	);
	iReference *fProcedureReference;
	iFunction<void (DWORD)> *fProcedure;

	cnRTL::cAtomicVar<bool> fSetting;
	bool fWaiterIsSet;

	void CleanupCallback(void);
	void WaitForCallback(void);
	static VOID CALLBACK WaitCallbackProc(
		_Inout_     PTP_CALLBACK_INSTANCE Instance,
		_Inout_opt_ PVOID                 Context
	);
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
	cNT6ThreadPoolWorkAsyncProcedure(PTP_CALLBACK_ENVIRON env,iReference *Reference,iProcedure *Procedure);
	~cNT6ThreadPoolWorkAsyncProcedure();

	virtual void cnLib_FUNC Start(void)override;
protected:
	PTP_WORK fWork;
	iReference *fProcedureReference;
	iProcedure *fProcedure;

	static VOID CALLBACK WorkProc(
		_Inout_     PTP_CALLBACK_INSTANCE Instance,
		_Inout_opt_ PVOID                 Context,
		_Inout_     PTP_WORK              Work
	);

};
//---------------------------------------------------------------------------
class cNT6ThreadPoolTimerAsyncTimer : public iAsyncTimer, public cnRTL::cDualReference
{
public:
	cNT6ThreadPoolTimerAsyncTimer(PTP_CALLBACK_ENVIRON env,iReference *Reference,iProcedure *Procedure);
	~cNT6ThreadPoolTimerAsyncTimer(void);

	virtual void cnLib_FUNC Start(iTimepoint *DueTime,sInt64 DueTimeDelay,uInt64 Period)override;
	virtual void cnLib_FUNC Stop(void)override;

protected:
	void VirtualStopped(void);

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
	);

	void CleanupTimerCallback(void);
	void WaitForTimer(void);
	static VOID CALLBACK WaitTimerProc(
		_Inout_     PTP_CALLBACK_INSTANCE Instance,
		_Inout_opt_ PVOID                 Context
	);

};
//---------------------------------------------------------------------------
class bcNT6ThreadPoolEnvironment : public iThreadPool
{
public:
	bcNT6ThreadPoolEnvironment(PTP_CALLBACK_ENVIRON Env);
	~bcNT6ThreadPoolEnvironment();

	typedef cNT6ThreadPoolHandleWaiter cHandleWaiter;
	//typedef cNT6ThreadPoolHandleRepeatWaiter cHandleRepeatWaiter;

	static void StartWork(PTP_CALLBACK_ENVIRON Env,iReference *Reference,iProcedure *Procedure);
	
	virtual void cnLib_FUNC Execute(iReference *Reference,iProcedure *Procedure)override;
	virtual rPtr<iAsyncProcedure>	cnLib_FUNC CreateWork(iReference *Reference,iProcedure *Procedure)override;
	virtual rPtr<iAsyncTimer>		cnLib_FUNC CreateTimer(iReference *Reference,iProcedure *Procedure)override;

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
	);

};
//---------------------------------------------------------------------------
class cNT6ThreadPoolEnvironment : public bcNT6ThreadPoolEnvironment
{
public:
	cNT6ThreadPoolEnvironment();
	~cNT6ThreadPoolEnvironment();

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
