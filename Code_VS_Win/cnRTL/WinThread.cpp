#include <intrin.h>
#include "WinThread.h"

using namespace cnLibrary;
using namespace cnRTL;
using namespace cnWinRTL;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CHAR Interlocked::_Func<1>::Inc(volatile CHAR &Dest)noexcept{	return _InterlockedExchangeAdd8(&Dest,1);	}
CHAR Interlocked::_Func<1>::Dec(volatile CHAR &Dest)noexcept{	return _InterlockedExchangeAdd8(&Dest,-1);	}
CHAR Interlocked::_Func<1>::Xchg(volatile CHAR &Dest,CHAR Src)noexcept{	return InterlockedExchange8(&Dest,Src);	}
CHAR Interlocked::_Func<1>::CmpXchg(volatile CHAR &Dest,CHAR Src,CHAR Compare)noexcept{	return _InterlockedCompareExchange8(&Dest,Src,Compare);	}
//---------------------------------------------------------------------------
SHORT Interlocked::_Func<2>::Inc(volatile SHORT &Dest)noexcept{	return InterlockedIncrement16(&Dest);	}
SHORT Interlocked::_Func<2>::Dec(volatile SHORT &Dest)noexcept{	return InterlockedDecrement16(&Dest);	}
SHORT Interlocked::_Func<2>::Xchg(volatile SHORT &Dest,SHORT Src)noexcept{	return InterlockedExchange16(&Dest,Src);	}
SHORT Interlocked::_Func<2>::CmpXchg(volatile SHORT &Dest,SHORT Src,SHORT Compare)noexcept{	return InterlockedCompareExchange16(&Dest,Src,Compare);	}
//---------------------------------------------------------------------------
LONG Interlocked::_Func<4>::Inc(volatile LONG &Dest)noexcept{	return InterlockedIncrement(&Dest);	}
LONG Interlocked::_Func<4>::Dec(volatile LONG &Dest)noexcept{	return InterlockedDecrement(&Dest);	}
LONG Interlocked::_Func<4>::Xchg(volatile LONG &Dest,LONG Src)noexcept{	return InterlockedExchange(&Dest,Src);	}
LONG Interlocked::_Func<4>::CmpXchg(volatile LONG &Dest,LONG Src,LONG Compare)noexcept{	return InterlockedCompareExchange(&Dest,Src,Compare);	}
//---------------------------------------------------------------------------
LONG64 Interlocked::_Func<8>::Inc(volatile LONG64 &Dest)noexcept{	return InterlockedIncrement64(&Dest);	}
LONG64 Interlocked::_Func<8>::Dec(volatile LONG64 &Dest)noexcept{	return InterlockedDecrement64(&Dest);	}
LONG64 Interlocked::_Func<8>::Xchg(volatile LONG64 &Dest,LONG64 Src)noexcept{	return InterlockedExchange64(&Dest,Src);	}
LONG64 Interlocked::_Func<8>::CmpXchg(volatile LONG64 &Dest,LONG64 Src,LONG64 Compare)noexcept{	return InterlockedCompareExchange64(&Dest,Src,Compare);	}
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
cCriticalSection::cCriticalSection()noexcept
{
	::InitializeCriticalSection(&fCriticalSection);
}
cCriticalSection::~cCriticalSection()noexcept
{
	::DeleteCriticalSection(&fCriticalSection);
}
//---------------------------------------------------------------------------
void cCriticalSection::Acquire(void)noexcept
{
	::EnterCriticalSection(&fCriticalSection);
}
//---------------------------------------------------------------------------
bool cCriticalSection::TryAcquire(void)noexcept
{
	return ::TryEnterCriticalSection(&fCriticalSection)!=FALSE;
}
//---------------------------------------------------------------------------
void cCriticalSection::Release(void)noexcept
{
	::LeaveCriticalSection(&fCriticalSection);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cDependentRegistration::cDependentRegistration()noexcept
{
}
//---------------------------------------------------------------------------
cDependentRegistration::~cDependentRegistration()noexcept
{
}
//---------------------------------------------------------------------------
void cDependentRegistration::Register(iDependentInfo *Dependent)noexcept
{
	return Register(reinterpret_cast<bcNotifyToken*>(Dependent));
}
//---------------------------------------------------------------------------
void cDependentRegistration::Unregister(iDependentInfo *Dependent)noexcept
{
	return Unregister(reinterpret_cast<bcNotifyToken*>(Dependent));
}
//---------------------------------------------------------------------------
void cDependentRegistration::Register(bcNotifyToken *NotifyToken)noexcept
{
	auto AutoLock=TakeLock(&fCS);

	if(fShutDown==false){
		if(fDependentSet.Insert(NotifyToken)){
			fWaitObject.Acquire();
		}
	}
}
//---------------------------------------------------------------------------
void cDependentRegistration::Unregister(bcNotifyToken *NotifyToken)noexcept
{
	auto AutoLock=TakeLock(&fCS);
	if(fDependentSet.Remove(NotifyToken)){
		fWaitObject.Release();


		if(fShutdowningItems!=nullptr){
			uIntn ItemIndex;
			if(cnMemory::ViewBinarySearch(ItemIndex,fShutdowningItems->Pointer,fShutdowningItems->Length,[NotifyToken](const cRemovingItem &Item)->sfInt8{
				if(Item.Token==NotifyToken){
					return 0;
				}
				if(Item.Token<NotifyToken){
					return -1;
				}
				return 1;
			})){
				fShutdowningItems->Pointer[ItemIndex].Removed=true;
			}
		}
	}
}
//---------------------------------------------------------------------------
void cDependentRegistration::Shutdown(void)noexcept
{
	if(fShutDown)
		return;
	

	fShutDown=true;
	{
		cArrayBlock<cRemovingItem> ShutdowningItems;

		auto AutoLock=TakeLock(&fCS);

		ShutdowningItems.SetLength(fDependentSet.GetCount());
		fShutdowningItems=&ShutdowningItems;
		uIntn ShutingDownItemIndex=0;
		for(auto Dependent : fDependentSet){
			auto &Item=ShutdowningItems.Pointer[ShutingDownItemIndex++];
			Item.Token=Dependent;
			Item.Removed=false;
		}

		for(ShutingDownItemIndex=0;ShutingDownItemIndex<ShutdowningItems.Length;ShutingDownItemIndex++){
			auto &Item=ShutdowningItems.Pointer[ShutingDownItemIndex];
			if(Item.Removed==false){
				Item.Token->DependentShutdownNotification();
			}
		}
		fShutdowningItems=nullptr;
	}
	fWaitObject.Wait(3000);

#if 1 && defined(cnLib_DEBUG)
	// report remaining dependent
	{
		auto AutoLock=TakeLock(&fCS);
		if(fDependentSet.GetCount()!=0){
			cStringBuffer<uChar16> ReportText;
			ReportText.Append(u"Remaining Dependent Object :\n");
			for(auto NotifyToken : fDependentSet){
				auto Dependent=reinterpret_cast<iDependentInfo*>(NotifyToken);
				auto Description=Dependent->DependentCreateDescription();
				uIntn DescLength;
				auto DescArray=Description->GetArray(DescLength);
				StringStream::WriteFormatString(ReportText.StreamWriteBuffer(),u"%.*s\n",DescLength,DescArray);
			}
			ReportText.Append(u"End of Dependent Object List\n");
			OutputDebugStringW(utow(ReportText.GetString()));
		}
	}
	fWaitObject.Wait(INFINITE);
#endif
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
	auto Registration=cnSystem::GetSystemDependentRegistration();
	Registration->Register(this);
}
//---------------------------------------------------------------------------
cThreadHandle::~cThreadHandle()noexcept
{
	auto Registration=cnSystem::GetSystemDependentRegistration();
	Registration->Unregister(this);
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
rPtr< iArrayReference<const uChar16> > cThreadHandle::DependentCreateDescription(void)noexcept
{
	cString<uChar16> Temp=cnRTL::CreateStringFormat(u"cThreadHandle - ThreadID = %d(0x%x), Handle=%x",fThreadID,fThreadID,(uIntn)fThreadHandle);
	return cnVar::MoveCast(Temp.Token());
	//return u"cThreadHandle"_cArray;
}
//---------------------------------------------------------------------------
void cThreadHandle::DependentShutdownNotification(void)noexcept
{
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
VOID NTAPI cNT6TLSStaticThreadData::WaitCallback(
	_Inout_     PTP_CALLBACK_INSTANCE Instance,
	_Inout_opt_ PVOID                 Context,
	_Inout_     PTP_WAIT              Wait,
	_In_        TP_WAIT_RESULT        WaitResult
)noexcept
{Instance;Wait;WaitResult;
	auto *Variable=static_cast<cNT6TLSStaticThreadData*>(Context);
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
