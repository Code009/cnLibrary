﻿#include <intrin.h>
#include "WinThread.h"

using namespace cnLibrary;
using namespace cnRTL;
using namespace cnWinRTL;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
constexpr char cWinExclusiveFlag::rfIdle;
constexpr char cWinExclusiveFlag::rfExecute;
constexpr char cWinExclusiveFlag::rfPending;
//---------------------------------------------------------------------------
cWinExclusiveFlag::cWinExclusiveFlag(bool InitalRun)noexcept
{
	if(InitalRun)
		fRunFlag=rfPending;
	else
		fRunFlag=rfIdle;
}
//---------------------------------------------------------------------------
bool cWinExclusiveFlag::IsRunning(void)const noexcept
{
	return fRunFlag!=rfIdle;
}
//---------------------------------------------------------------------------
bool cWinExclusiveFlag::Acquire(void)noexcept
{
	auto PrevRunFlag=::_InterlockedExchange8(&fRunFlag,rfPending);
	switch(PrevRunFlag){
	case rfIdle:
		// new start
		return true;
	case rfExecute:
		// is running
		return false;
	default:
		cnLib_ASSERT(0);
	case rfPending:
		// already requested to run
		return false;
	}
}
//---------------------------------------------------------------------------
bool cWinExclusiveFlag::Release(void)noexcept
{
	// decrease state
	auto PrevRunFlag=::_InterlockedExchangeAdd8(&fRunFlag,-1);
	switch(PrevRunFlag){
	default:
		cnLib_ASSERT(0);
	case rfPending:
		// requested to run, continue running
		return false;
	case rfExecute:
		// successfully stoped
		return true;
	}
}
//---------------------------------------------------------------------------
void cWinExclusiveFlag::Continue(void)noexcept
{
	fRunFlag=rfExecute;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWinTLS::cWinTLS()noexcept
{
	fTLSIndex=::TlsAlloc();
}
//---------------------------------------------------------------------------
cWinTLS::~cWinTLS()noexcept
{
	::TlsFree(fTLSIndex);
}
//---------------------------------------------------------------------------
void* cWinTLS::Get(void)noexcept
{
	return ::TlsGetValue(fTLSIndex);
}
//---------------------------------------------------------------------------
bool cWinTLS::Set(void *Value)noexcept
{
	return ::TlsSetValue(fTLSIndex,Value)!=FALSE;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWinWaitObject::cWinWaitObject()noexcept
{
	fRefCount=0;
}
//---------------------------------------------------------------------------
cWinWaitObject::~cWinWaitObject()noexcept
{
	cnLib_ASSERT(fRefCount==0);
}
//---------------------------------------------------------------------------
void cWinWaitObject::Acquire(void)noexcept
{
	fRefCount.Free++;
}
//---------------------------------------------------------------------------
void cWinWaitObject::Release(void)noexcept
{
	if(fRefCount.Free--==0){
		if(fWaitThreadHandle!=nullptr){
			::QueueUserAPC(WaitNotifyAPC,fWaitThreadHandle,reinterpret_cast<ULONG_PTR>(this));
		}
	}
}
//---------------------------------------------------------------------------
bool cWinWaitObject::Check(void)noexcept
{
	return fRefCount==0;
}
//---------------------------------------------------------------------------
VOID CALLBACK cWinWaitObject::WaitNotifyAPC(ULONG_PTR Param)noexcept
{
	auto This=reinterpret_cast<cWinWaitObject*>(Param);
	This->fWaitFlag=false;
}
//---------------------------------------------------------------------------
void cWinWaitObject::Wait(DWORD Millisecond)noexcept
{
	if(fRefCount==0)
		return;

	HANDLE CurProc=::GetCurrentProcess();
	if(DuplicateHandle(CurProc,::GetCurrentThread(),CurProc,&fWaitThreadHandle,THREAD_SET_CONTEXT,false,DUPLICATE_SAME_ACCESS)==FALSE){
		return;
	}
	fWaitFlag=true;

	if(fRefCount.Free--!=0){
		while(fWaitFlag){
			DWORD WaitRet=SleepEx(Millisecond,TRUE);
			if(WaitRet==0){
				break;
			}
		}
	}
	fRefCount.Free++;
	::CloseHandle(fWaitThreadHandle);
	fWaitThreadHandle=nullptr;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWinWaitReference::cWinWaitReference()noexcept
{
}
//---------------------------------------------------------------------------
cWinWaitReference::~cWinWaitReference()noexcept
{
}
//---------------------------------------------------------------------------
void cWinWaitReference::IncreaseReference(void)noexcept
{
	Acquire();
}
//---------------------------------------------------------------------------
void cWinWaitReference::DecreaseReference(void)noexcept
{
	Release();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcModuleReference::bcModuleReference()noexcept(true)
	: bcRegisteredReference(false)
	, fModuleActive(false)
{
}
//---------------------------------------------------------------------------
bcModuleReference::~bcModuleReference()noexcept(true)
{
}
//---------------------------------------------------------------------------
rPtr<iLibraryReference> bcModuleReference::QueryReference(iLibraryReferrer *Referrer,bool NoLoad)noexcept(true)
{
	bool Referenced=MakeStrongReference();

	auto AutoLock=TakeLock(&fCS);

	if(Referenced==false){
		// RefCount==0

		if(fModuleActive || NoLoad){
			// is shutting down, or did not load
			return nullptr;
		}
		fModuleShutdown=false;
		fModuleActive=true;
		ModuleInitialize();
		IncRef();
	}

	return CreateReference(Referrer);
}
//---------------------------------------------------------------------------
void bcModuleReference::ReferenceUpdate(void)noexcept(true)
{
	ReferenceProcess();
}
//---------------------------------------------------------------------------
void bcModuleReference::ReferenceShutdown(void)noexcept(true)
{
	fModuleShutdown=true;
	ReferenceProcess();
}
//---------------------------------------------------------------------------
void bcModuleReference::ReferenceProcess(void)noexcept(true)
{
	if(fReferenceProcessFlag.Acquire()==false)
		return;


	do{
		fReferenceProcessFlag.Continue();

		auto AutoLock=TakeLock(&fCS);

		auto IsEmptySet=UpdateReferenceSet();
		if(fModuleShutdown){

			if(IsEmptySet){
				// all reference deleted
				ModuleFinialize();

				fModuleActive=false;
			}
		}

	}while(fReferenceProcessFlag.Release()==false);
}
//---------------------------------------------------------------------------
#if _WIN32_WINNT >= _WIN32_WINNT_WIN7
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cSRWLock::cSRWLock()noexcept
{
	::InitializeSRWLock(&fLock);
}
//---------------------------------------------------------------------------
cSRWLock::~cSRWLock()noexcept
{
}
//---------------------------------------------------------------------------
void cSRWLock::Acquire(void)noexcept
{
	::AcquireSRWLockExclusive(&fLock);
}
//---------------------------------------------------------------------------
bool cSRWLock::TryAcquire(void)noexcept
{
	return FALSE!=::TryAcquireSRWLockExclusive(&fLock);
}
//---------------------------------------------------------------------------
void cSRWLock::Release(void)noexcept
{
	::ReleaseSRWLockExclusive(&fLock);
}
//---------------------------------------------------------------------------
void cSRWLock::AcquireShared(void)noexcept
{
	::AcquireSRWLockShared(&fLock);
}
//---------------------------------------------------------------------------
bool cSRWLock::TryAcquireShared(void)noexcept
{
	return FALSE!=::TryAcquireSRWLockShared(&fLock);
}
//---------------------------------------------------------------------------
void cSRWLock::ReleaseShared(void)noexcept
{
	::ReleaseSRWLockShared(&fLock);
}
//---------------------------------------------------------------------------
#endif	// _WIN32_WINNT >= _WIN32_WINNT_WIN7
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
VOID CALLBACK cnWinRTL::EmptyAPCFunction(ULONG_PTR)noexcept
{
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cThreadHandle::cThreadHandle()noexcept
{
}
//---------------------------------------------------------------------------
cThreadHandle::~cThreadHandle()noexcept
{
}
//---------------------------------------------------------------------------
HANDLE cThreadHandle::GetThreadHandle(void)noexcept
{
	return fThreadHandle;
}
//---------------------------------------------------------------------------
bool cThreadHandle::ThreadIsCurrent(void)noexcept
{
	return fThreadID==::GetCurrentThreadId();
}
//---------------------------------------------------------------------------
bool cThreadHandle::IsCurrentThread(void)noexcept
{
	return ThreadIsCurrent();
}
//---------------------------------------------------------------------------
int cThreadHandle::ConvertThreadPriorityFrom(sInt8 Priority)noexcept
{
	static_assert(THREAD_PRIORITY_HIGHEST<=15,"");
	static_assert(THREAD_PRIORITY_NORMAL==0,"");
	static_assert(THREAD_PRIORITY_IDLE>=-15,"");
	
	if(Priority==0)
		return THREAD_PRIORITY_NORMAL;

	int wtProiority=Priority/8;
	
	if(wtProiority<THREAD_PRIORITY_IDLE){
		wtProiority=THREAD_PRIORITY_IDLE;
	}
	else if(wtProiority>THREAD_PRIORITY_HIGHEST){
		wtProiority=THREAD_PRIORITY_HIGHEST;
	}
	return wtProiority;
}
//---------------------------------------------------------------------------
sInt8 cThreadHandle::ConvertThreadPriorityTo(int Priority)noexcept
{
	static_assert(THREAD_PRIORITY_NORMAL==0,"");

	if(Priority==THREAD_PRIORITY_NORMAL)
		return 0;

	if(Priority<THREAD_PRIORITY_IDLE)
		return -128;
	if(Priority>THREAD_PRIORITY_HIGHEST)
		return 127;

	sInt8 libProiority=static_cast<sInt8>(Priority*8);
	return libProiority;
}
//---------------------------------------------------------------------------
VOID CALLBACK cThreadHandle::WakeAPCFunction(ULONG_PTR dwParam)noexcept
{
	auto pVal=reinterpret_cast<bool*>(dwParam);
	*pVal=false;
}
//---------------------------------------------------------------------------
void cThreadHandle::Wake(bool *ResetVal)noexcept
{
	// Queue APC
	DWORD r;
	if(ResetVal!=nullptr){
		r=::QueueUserAPC(WakeAPCFunction,fThreadHandle,reinterpret_cast<ULONG_PTR>(ResetVal));
	}
	else{
		r=::QueueUserAPC(EmptyAPCFunction,fThreadHandle,0);
	}
	if(r==FALSE){
		DWORD ErrorCode=::GetLastError();
		cnLib_ASSERT(!ErrorCode);
	}
}
//---------------------------------------------------------------------------
bool cThreadHandle::SetPriority(HANDLE ThreadHandle,sfInt8 Priority)noexcept
{
	return ::SetThreadPriority(ThreadHandle,ConvertThreadPriorityFrom(Priority))!=FALSE;
}
//---------------------------------------------------------------------------
bool cThreadHandle::GetPriority(HANDLE ThreadHandle,sfInt8 &Priority)noexcept
{
	int r=::GetThreadPriority(ThreadHandle);
	if(r==THREAD_PRIORITY_ERROR_RETURN)
		return false;
	Priority=ConvertThreadPriorityTo(r);
	return true;
}
//---------------------------------------------------------------------------
sfInt8 cThreadHandle::GetPriority(HANDLE ThreadHandle)noexcept
{
	int r=::GetThreadPriority(ThreadHandle);
	if(r==THREAD_PRIORITY_ERROR_RETURN)
		return 0;
	return ConvertThreadPriorityTo(r);
}
//---------------------------------------------------------------------------
bool cThreadHandle::SetPriority(sfInt8 Priority)noexcept
{
	return SetPriority(fThreadHandle,Priority);
}
//---------------------------------------------------------------------------
bool cThreadHandle::GetPriority(sfInt8 &Priority)noexcept
{
	return GetPriority(fThreadHandle,Priority);
}
//---------------------------------------------------------------------------
bool cnWinRTL::CurrentThreadSleepUntil(uInt64 SystemTime)noexcept
{
	if(SystemTime==SystemTime_Never){
		return CurrentThreadSleep(INFINITE);
	}
	sInt64 SleepNanoseconds=SystemTimeSinceNTFileTimeNowNS(SystemTime);
	if(SleepNanoseconds<0){
		// already time out
		return false;
	}
	DWORD SleepMS=static_cast<DWORD>(SleepNanoseconds/Time_1ms);

	return CurrentThreadSleep(SleepMS);
}
//---------------------------------------------------------------------------
bool cnWinRTL::CurrentThreadSleep(DWORD Milliseconds)noexcept
{
	auto r=SleepEx(Milliseconds,TRUE);
	if(r==WAIT_TIMEOUT){
		return false;
	}
	//WAIT_IO_COMPLETION
	return true;
}
//---------------------------------------------------------------------------
#if _WIN32_WINNT >= _WIN32_WINNT_VISTA
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cNT6ThreadLocalVariable::cNT6ThreadLocalVariable()noexcept
{
	fTLSIndex=::TlsAlloc();
}
//---------------------------------------------------------------------------
cNT6ThreadLocalVariable::~cNT6ThreadLocalVariable()noexcept
{
	::TlsFree(fTLSIndex);
}
//---------------------------------------------------------------------------
VOID NTAPI cNT6ThreadLocalVariable::WaitCallback(
	_Inout_     PTP_CALLBACK_INSTANCE Instance,
	_Inout_opt_ PVOID                 Context,
	_Inout_     PTP_WAIT              Wait,
	_In_        TP_WAIT_RESULT        WaitResult
)noexcept
{Instance;Wait;WaitResult;
	auto *Variable=static_cast<cThreadData*>(Context);
	::CloseThreadpoolWait(Variable->Waiter);
	::CloseHandle(Variable->ThreadHandle);
	if(Variable->NotifyProc!=nullptr){
		Variable->NotifyProc->Execute(rPtr<iReference>::TakeFromManual(Variable->Reference),Variable->Object);
	}
	else{
		if(Variable->Reference!=nullptr){
			Variable->Reference->DecreaseReference();
		}
	}
	delete Variable;
}
//---------------------------------------------------------------------------
cNT6ThreadLocalVariable::cThreadData* cNT6ThreadLocalVariable::CreateThreadData(void)noexcept
{
	auto ThreadData=new cThreadData;
	HANDLE Process=GetCurrentProcess();
	::DuplicateHandle(Process,GetCurrentThread(),Process,&ThreadData->ThreadHandle,SYNCHRONIZE,FALSE,0);
	ThreadData->Waiter=::CreateThreadpoolWait(WaitCallback,ThreadData,nullptr);
	::SetThreadpoolWait(ThreadData->Waiter,ThreadData->ThreadHandle,nullptr);
	::TlsSetValue(fTLSIndex,ThreadData);
	return ThreadData;
}
//---------------------------------------------------------------------------
void* cNT6ThreadLocalVariable::Get(void)noexcept
{
	void *ThreadData=::TlsGetValue(fTLSIndex);
	if(ThreadData==nullptr)
		return nullptr;
	return static_cast<cThreadData*>(ThreadData)->Object;
}
//---------------------------------------------------------------------------
void cNT6ThreadLocalVariable::Set(iReference *Reference,void *Data)noexcept
{
	void *pData=::TlsGetValue(fTLSIndex);
	cThreadData *ThreadData;
	if(pData==nullptr){
		if(Reference==nullptr && Data==nullptr){
			return;
		}
		ThreadData=CreateThreadData();
	}
	else{
		ThreadData=static_cast<cThreadData*>(pData);
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
//---------------------------------------------------------------------------
void cNT6ThreadLocalVariable::Clear(void)noexcept
{
	void *pData=::TlsGetValue(fTLSIndex);
	if(pData==nullptr){
		return;
	}
	cThreadData *ThreadData=static_cast<cThreadData*>(pData);
	if(ThreadData->Reference!=nullptr){
		ThreadData->Reference->DecreaseReference();
	}
	ThreadData->Object=nullptr;
	ThreadData->Reference=nullptr;
	ThreadData->NotifyProc=nullptr;
}
//---------------------------------------------------------------------------
void cNT6ThreadLocalVariable::SetThreadExitNotify(iThreadExitNotifyProc *NotifyProc)noexcept
{
	void *pData=::TlsGetValue(fTLSIndex);
	cThreadData *ThreadData;
	if(pData==nullptr){
		if(NotifyProc==nullptr){
			return;
		}
		ThreadData=CreateThreadData();
	}
	else{
		ThreadData=static_cast<cThreadData*>(pData);
	}
	ThreadData->NotifyProc=NotifyProc;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void* cNT6TLSStaticThreadData::operator new(tSize Size)noexcept
{
	return ::HeapAlloc(::GetProcessHeap(),0,Size);
}
//---------------------------------------------------------------------------
void cNT6TLSStaticThreadData::operator delete(void*p)noexcept{
	::HeapFree(::GetProcessHeap(),0,p);
}
//---------------------------------------------------------------------------
void cNT6TLSStaticThreadData::Setup(void)noexcept
{
	HANDLE Process=GetCurrentProcess();
	::DuplicateHandle(Process,GetCurrentThread(),Process,&ThreadHandle,SYNCHRONIZE,FALSE,0);
	Waiter=::CreateThreadpoolWait(WaitCallback,this,nullptr);
	::SetThreadpoolWait(Waiter,ThreadHandle,nullptr);
}
//---------------------------------------------------------------------------
void cNT6TLSStaticThreadData::Cancel(void)noexcept
{
	::SetThreadpoolWait(Waiter,nullptr,nullptr);
	::WaitForThreadpoolWaitCallbacks(Waiter,TRUE);
}
//---------------------------------------------------------------------------
void cNT6TLSStaticThreadData::Clear(void)noexcept
{
	::CloseThreadpoolWait(Waiter);
	::CloseHandle(ThreadHandle);
	if(NotifyProc!=nullptr){
		NotifyProc->Execute(rPtr<iReference>::TakeFromManual(Reference),Object);
	}
	else{
		if(Reference!=nullptr){
			Reference->DecreaseReference();
		}
	}
}
//---------------------------------------------------------------------------
VOID NTAPI cNT6TLSStaticThreadData::WaitCallback(
	_Inout_     PTP_CALLBACK_INSTANCE Instance,
	_Inout_opt_ PVOID                 Context,
	_Inout_     PTP_WAIT              Wait,
	_In_        TP_WAIT_RESULT        WaitResult
)noexcept
{Instance;Wait;WaitResult;
	auto *Variable=static_cast<cNT6TLSStaticThreadData*>(Context);
	Variable->Clear();
	delete Variable;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cNT6ThreadPoolHandleWaiter::cNT6ThreadPoolHandleWaiter(PTP_CALLBACK_ENVIRON env,iReference *Reference,iFunction<void (DWORD WaitResult)noexcept(true)> *Callback)noexcept
{
	fTPEnv=env;
	fSetting=false;
	fWaiterIsSet=false;
	fWaiter=::CreateThreadpoolWait(WorkProc,this,fTPEnv);
	fProcedureReference=Reference;
	fProcedure=Callback;
}
//---------------------------------------------------------------------------
cNT6ThreadPoolHandleWaiter::~cNT6ThreadPoolHandleWaiter(void)noexcept
{
	::SetThreadpoolWait(fWaiter,nullptr,nullptr);
	::CloseThreadpoolWait(fWaiter);
}
//---------------------------------------------------------------------------
void cNT6ThreadPoolHandleWaiter::VirtualStopped(void)noexcept
{
	cnLib_ASSERT(fSetting==false);
	while(fSetting){
		// wait for other operation
		::SwitchToThread();
	}

	if(fWaiterIsSet){
		CleanupCallback();
	}
	InnerDecReference('self');
}
//---------------------------------------------------------------------------
void cNT6ThreadPoolHandleWaiter::CleanupCallback(void)noexcept
{
	// wait for timer callbacks in thread pool
	if(::TrySubmitThreadpoolCallback(WaitCallbackProc,this,fTPEnv)==FALSE){
		// cannot submit a new work item, wait in this thread
		WaitForCallback();
	}
}
//---------------------------------------------------------------------------
void cNT6ThreadPoolHandleWaiter::WaitForCallback(void)noexcept
{
	// wait for timer ends
	::WaitForThreadpoolWaitCallbacks(fWaiter,FALSE);

	if(fProcedureReference!=nullptr)
		rDecReference(fProcedureReference,'thpt');
	
	InnerDecReference('tphw');
}
//---------------------------------------------------------------------------
VOID CALLBACK cNT6ThreadPoolHandleWaiter::WaitCallbackProc(
		_Inout_     PTP_CALLBACK_INSTANCE Instance,
		_Inout_opt_ PVOID                 Context
	)noexcept
{Instance;
	auto This=static_cast<cNT6ThreadPoolHandleWaiter*>(Context);
	This->WaitForCallback();
}
//---------------------------------------------------------------------------
VOID CALLBACK cNT6ThreadPoolHandleWaiter::WorkProc(
		_Inout_     PTP_CALLBACK_INSTANCE Instance,
		_Inout_opt_ PVOID                 Context,
		_Inout_     PTP_WAIT              Wait,
		_In_        TP_WAIT_RESULT        WaitResult
	)noexcept
{Instance,Wait;
	auto This=static_cast<cNT6ThreadPoolHandleWaiter*>(Context);
	This->fProcedure->Execute(WaitResult);
}
//---------------------------------------------------------------------------
void cNT6ThreadPoolHandleWaiter::SetWait(HANDLE WaitHandle,PFILETIME Timeout)noexcept
{
	if(fSetting.Acquire.Xchg(true)){
		// another configuration in progress
		return;
	}

	if(WaitHandle!=nullptr){
		// set wait
		if(fWaiterIsSet==false){
			fWaiterIsSet=true;
			InnerIncReference('tphw');

			if(fProcedureReference!=nullptr)
				rIncReference(fProcedureReference,'thpt');
		}

		::SetThreadpoolWait(fWaiter,WaitHandle,Timeout);
	}
	else{
		// reset wait
		::SetThreadpoolWait(fWaiter,nullptr,nullptr);

		if(fWaiterIsSet){
			fWaiterIsSet=false;
			CleanupCallback();
		}
	}

	fSetting.Release.Store(false);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//cNT6ThreadPoolHandleRepeatWaiter::cNT6ThreadPoolHandleRepeatWaiter(PTP_CALLBACK_ENVIRON env,iReference *Reference,iProcedure *Procedure)
//{
//	fWaiter=::CreateThreadpoolWait(WorkProc,this,env);
//	fProcedureReference=Reference;
//	fProcedure=Procedure;
//	fWaitHandle=nullptr;
//#pragma message (cnLib_FILE_LINE ": TODO - remove repeat waiter")
//}
////---------------------------------------------------------------------------
//cNT6ThreadPoolHandleRepeatWaiter::~cNT6ThreadPoolHandleRepeatWaiter(void)
//{
//	::SetThreadpoolWait(fWaiter,nullptr,nullptr);
//	::CloseThreadpoolWait(fWaiter);
//}
////---------------------------------------------------------------------------
//VOID CALLBACK cNT6ThreadPoolHandleRepeatWaiter::WorkProc(
//		_Inout_     PTP_CALLBACK_INSTANCE Instance,
//		_Inout_opt_ PVOID                 Context,
//		_Inout_     PTP_WAIT              Wait,
//		_In_        TP_WAIT_RESULT        WaitResult
//	)
//{Instance,Wait,WaitResult;
//	static_cast<cNT6ThreadPoolHandleRepeatWaiter*>(Context)->WorkProc();
//}
////---------------------------------------------------------------------------
//void cNT6ThreadPoolHandleRepeatWaiter::WorkProc(void)
//{
//	fProcedure->Execute();
//	::SetThreadpoolWait(fWaiter,fWaitHandle,nullptr);
//}
////---------------------------------------------------------------------------
//void cNT6ThreadPoolHandleRepeatWaiter::SetWaitHandle(HANDLE WaitHandle)
//{
//	fWaitHandle=WaitHandle;
//	::SetThreadpoolWait(fWaiter,fWaitHandle,nullptr);
//}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cNT6ThreadPoolWorkAsyncProcedure::cNT6ThreadPoolWorkAsyncProcedure(PTP_CALLBACK_ENVIRON env,iReference *Reference,iProcedure *Procedure)noexcept
	: fProcedureReference(Reference)
	, fProcedure(Procedure)
{
	fWork=::CreateThreadpoolWork(WorkProc,this,env);
}
//---------------------------------------------------------------------------
cNT6ThreadPoolWorkAsyncProcedure::~cNT6ThreadPoolWorkAsyncProcedure()noexcept
{
	::CloseThreadpoolWork(fWork);
}
//---------------------------------------------------------------------------
VOID CALLBACK cNT6ThreadPoolWorkAsyncProcedure::WorkProc(
		_Inout_     PTP_CALLBACK_INSTANCE Instance,
		_Inout_opt_ PVOID                 Context,
		_Inout_     PTP_WORK              Work
	)noexcept
{	Work;Instance;
	auto This=static_cast<cNT6ThreadPoolWorkAsyncProcedure*>(Context);
	auto Reference=This->fProcedureReference;

	This->fProcedure->Execute();

	// release this from work
	rDecReference(This,'work');

	// release thread procedure
	if(Reference!=nullptr)
		rDecReference(Reference,'thpw');

}
//---------------------------------------------------------------------------
void cNT6ThreadPoolWorkAsyncProcedure::Start(void)noexcept
{
	cnLib_ASSERT(this!=nullptr);

	// reference this for work
	rIncReference(this,'work');

	// reference for thread procedure
	if(fProcedureReference!=nullptr)
		rIncReference(fProcedureReference,'thpw');

	SubmitThreadpoolWork(fWork);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cNT6ThreadPoolTimerAsyncTimer::cNT6ThreadPoolTimerAsyncTimer(PTP_CALLBACK_ENVIRON env,iReference *Reference,iProcedure *Procedure)noexcept
	: fProcedureReference(Reference),fProcedure(Procedure)
{
	fSetting=false;
	fTimerIsSet=false;
	fTPEnv=env;
	fTimer=::CreateThreadpoolTimer(WorkProc,this,fTPEnv);
}
//---------------------------------------------------------------------------
cNT6ThreadPoolTimerAsyncTimer::~cNT6ThreadPoolTimerAsyncTimer(void)noexcept
{
	::CloseThreadpoolTimer(fTimer);
}
//---------------------------------------------------------------------------
void cNT6ThreadPoolTimerAsyncTimer::VirtualStopped(void)noexcept
{
	cnLib_ASSERT(fSetting==false);
	while(fSetting){
		// wait for other operation
		::SwitchToThread();
	}

	if(fTimerIsSet){
		::SetThreadpoolTimer(fTimer,NULL,0,0);
		CleanupTimerCallback();
	}
	InnerDecReference('self');
}
//---------------------------------------------------------------------------
void cNT6ThreadPoolTimerAsyncTimer::CleanupTimerCallback(void)noexcept
{
	// wait for timer callbacks in thread pool
	if(::TrySubmitThreadpoolCallback(WaitTimerProc,this,fTPEnv)==FALSE){
		// cannot submit a new work item, wait in this thread
		WaitForTimer();
	}
}
//---------------------------------------------------------------------------
void cNT6ThreadPoolTimerAsyncTimer::WaitForTimer(void)noexcept
{
	// wait for timer ends
	::WaitForThreadpoolTimerCallbacks(fTimer,FALSE);

	if(fProcedureReference!=nullptr)
		rDecReference(fProcedureReference,'thpt');
	
	InnerDecReference('tptm');
}
//---------------------------------------------------------------------------
VOID CALLBACK cNT6ThreadPoolTimerAsyncTimer::WaitTimerProc(
		_Inout_     PTP_CALLBACK_INSTANCE Instance,
		_Inout_opt_ PVOID                 Context
	)noexcept
{Instance;
	auto This=static_cast<cNT6ThreadPoolTimerAsyncTimer*>(Context);
	This->WaitForTimer();
}
//---------------------------------------------------------------------------
VOID CALLBACK cNT6ThreadPoolTimerAsyncTimer::WorkProc(
		_Inout_     PTP_CALLBACK_INSTANCE Instance,
		_Inout_opt_ PVOID                 Context,
		_Inout_     PTP_TIMER             Timer
	)noexcept
{Instance;Timer;
	auto This=static_cast<cNT6ThreadPoolTimerAsyncTimer*>(Context);
	This->fProcedure->Execute();
}
//---------------------------------------------------------------------------
void cNT6ThreadPoolTimerAsyncTimer::Start(uInt64 DueTime,uInt64 Period)noexcept
{
	if(fSetting.Acquire.Xchg(true)){
		// another configuration in progress
		return;
	}


	if(fTimerIsSet==false){
		fTimerIsSet=true;
		InnerIncReference('tptm');

		if(fProcedureReference!=nullptr)
			rIncReference(fProcedureReference,'thpt');
	}

	cNTFileTime DueFileTime;
	DueFileTime.FromSystemTime(DueTime);
	DWORD PeriodMS=static_cast<DWORD>(Period/Time_1ms);
	::SetThreadpoolTimer(fTimer,&DueFileTime.FileTime,PeriodMS,1000);


	fSetting.Release.Store(false);
}
//---------------------------------------------------------------------------
void cNT6ThreadPoolTimerAsyncTimer::Stop(void)noexcept
{
	if(fSetting.Acquire.Xchg(true)){
		// another configuration in progress
		return;
	}

	::SetThreadpoolTimer(fTimer,NULL,0,0);

	if(fTimerIsSet){
		fTimerIsSet=false;
		CleanupTimerCallback();
	}

	fSetting.Release.Store(false);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcNT6ThreadPoolEnvironment::bcNT6ThreadPoolEnvironment(PTP_CALLBACK_ENVIRON Env)noexcept
	: fEnv(Env)
{
}
//---------------------------------------------------------------------------
bcNT6ThreadPoolEnvironment::~bcNT6ThreadPoolEnvironment()noexcept
{
}
//---------------------------------------------------------------------------
VOID CALLBACK bcNT6ThreadPoolEnvironment::SimpleWork(
	_Inout_     PTP_CALLBACK_INSTANCE Instance,
	_Inout_opt_ PVOID                 Context
)noexcept
{	Instance;
	auto Data=static_cast<SimpleWorkData*>(Context);

	Data->Procedure->Execute();
	delete Data;
}
//---------------------------------------------------------------------------
void bcNT6ThreadPoolEnvironment::StartWork(PTP_CALLBACK_ENVIRON Env,iReference *Reference,iProcedure *Procedure)noexcept
{
	auto Data=new SimpleWorkData;
	Data->Reference=Reference;
	Data->Procedure=Procedure;
	if(TrySubmitThreadpoolCallback(SimpleWork,Data,Env)){
		// successful submited
		return;
	}

	delete Data;
}
//---------------------------------------------------------------------------
void bcNT6ThreadPoolEnvironment::Execute(iReference *Reference,iProcedure *Procedure)noexcept
{
	return StartWork(fEnv,Reference,Procedure);
}
//---------------------------------------------------------------------------
rPtr<iAsyncProcedure> bcNT6ThreadPoolEnvironment::CreateWork(iReference *Reference,iProcedure *Procedure)noexcept
{
	return rCreate<cNT6ThreadPoolWorkAsyncProcedure>(fEnv,Reference,Procedure);
}
//---------------------------------------------------------------------------
rPtr<iAsyncTimer> bcNT6ThreadPoolEnvironment::CreateTimer(iReference *Reference,iProcedure *Procedure)noexcept
{
	return rCreate<cNT6ThreadPoolTimerAsyncTimer>(fEnv,Reference,Procedure);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cNT6ThreadPoolEnvironment::cNT6ThreadPoolEnvironment()noexcept
	: bcNT6ThreadPoolEnvironment(&fEnvironment)
{
	fPool=::CreateThreadpool(nullptr);

	::SetThreadpoolThreadMinimum(fPool,0);
	// default thread maximum to number of processors
	//::SetThreadpoolThreadMaximum(fPool,gSysInfo.dwNumberOfProcessors*2);

	::InitializeThreadpoolEnvironment(fEnv);
	::SetThreadpoolCallbackPool(fEnv,fPool);
}
//---------------------------------------------------------------------------
cNT6ThreadPoolEnvironment::~cNT6ThreadPoolEnvironment()noexcept
{
	::CloseThreadpool(fPool);
	::DestroyThreadpoolEnvironment(fEnv);
}
//---------------------------------------------------------------------------
#endif	// _WIN32_WINNT >= _WIN32_WINNT_VISTA
//---------------------------------------------------------------------------
