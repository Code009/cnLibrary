#include "POSIX_Thread.h"
#include "POSIX_Time.h"

using namespace cnLibrary;
using namespace cnRTL;
using namespace siPOSIX;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#ifdef	siOS_POSIX_ENABLE_THREAD
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
c_pthread_mutex::c_pthread_mutex(pthread_mutexattr_t *attr)noexcept(true)
{
	pthread_mutex_init(&f_mutex,attr);
}
//---------------------------------------------------------------------------
c_pthread_mutex::~c_pthread_mutex()noexcept(true)
{
	pthread_mutex_destroy(&f_mutex);
}
//---------------------------------------------------------------------------
c_pthread_mutex::operator pthread_mutex_t* ()noexcept(true)
{
	return &f_mutex;
}
//---------------------------------------------------------------------------
void c_pthread_mutex::lock(void)noexcept(true)
{
	pthread_mutex_lock(&f_mutex);
}
//---------------------------------------------------------------------------
bool c_pthread_mutex::trylock(void)noexcept(true)
{
	return pthread_mutex_trylock(&f_mutex)==0;
}
//---------------------------------------------------------------------------
void c_pthread_mutex::unlock(void)noexcept(true)
{
	pthread_mutex_unlock(&f_mutex);
}
//---------------------------------------------------------------------------
void c_pthread_mutex::Acquire(void)noexcept(true){	return lock();	}
bool c_pthread_mutex::TryAcquire(void)noexcept(true){	return trylock();	}
void c_pthread_mutex::Release(void)noexcept(true){	return unlock();	}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
c_pthread_rwlock::c_pthread_rwlock(pthread_rwlockattr_t *attr)noexcept(true)
{
	pthread_rwlock_init(&f_lock,attr);
}
//---------------------------------------------------------------------------
c_pthread_rwlock::~c_pthread_rwlock()noexcept(true)
{
	pthread_rwlock_destroy(&f_lock);
}
//---------------------------------------------------------------------------
c_pthread_rwlock::operator pthread_rwlock_t* ()noexcept(true)
{
	return &f_lock;
}
//---------------------------------------------------------------------------
void c_pthread_rwlock::rdlock(void)noexcept(true)
{
	pthread_rwlock_rdlock(&f_lock);
}
//---------------------------------------------------------------------------
bool c_pthread_rwlock::tryrdlock(void)noexcept(true)
{
	return pthread_rwlock_tryrdlock(&f_lock)==0;
}
//---------------------------------------------------------------------------
void c_pthread_rwlock::wrlock(void)noexcept(true)
{
	pthread_rwlock_wrlock(&f_lock);
}
//---------------------------------------------------------------------------
bool c_pthread_rwlock::trywrlock(void)noexcept(true)
{
	return pthread_rwlock_trywrlock(&f_lock)==0;
}
//---------------------------------------------------------------------------
void c_pthread_rwlock::unlock(void)noexcept(true)
{
	pthread_rwlock_unlock(&f_lock);
}
//---------------------------------------------------------------------------
void c_pthread_rwlock::Acquire(void)noexcept(true){	return wrlock();	}
bool c_pthread_rwlock::TryAcquire(void)noexcept(true){	return trywrlock();	}
void c_pthread_rwlock::Release(void)noexcept(true){	return unlock();	}

void c_pthread_rwlock::AcquireShared(void)noexcept(true){	return rdlock();	}
bool c_pthread_rwlock::TryAcquireShared(void)noexcept(true){	return tryrdlock();	}
void c_pthread_rwlock::ReleaseShared(void)noexcept(true){	return unlock();	}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
c_pthread_cond::c_pthread_cond(pthread_condattr_t *attr)noexcept(true)
{
	pthread_cond_init(&f_cond,attr);
}
//---------------------------------------------------------------------------
c_pthread_cond::~c_pthread_cond()noexcept(true)
{
	pthread_cond_destroy(&f_cond);
}
//---------------------------------------------------------------------------
void c_pthread_cond::signal(void)noexcept(true)
{
	pthread_cond_signal(&f_cond);
}
//---------------------------------------------------------------------------
void c_pthread_cond::broadcast(void)noexcept(true)
{
	pthread_cond_broadcast(&f_cond);
}
//---------------------------------------------------------------------------
int c_pthread_cond::wait(pthread_mutex_t *mutex)noexcept(true)
{
	return pthread_cond_wait(&f_cond,mutex);
}
//---------------------------------------------------------------------------
int c_pthread_cond::timedwait(pthread_mutex_t *mutex,const struct timespec &tv)noexcept(true)
{
	return pthread_cond_timedwait(&f_cond,mutex,&tv);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
c_pthread_key::c_pthread_key(void (*destructor)(void*)noexcept(true))noexcept(true)
{
	pthread_key_create(&f_key,destructor);
}
//---------------------------------------------------------------------------
c_pthread_key::~c_pthread_key()noexcept(true)
{
	pthread_key_delete(f_key);
}
//---------------------------------------------------------------------------
void* c_pthread_key::getspecific(void)noexcept(true)
{
	return pthread_getspecific(f_key);
}
//---------------------------------------------------------------------------
bool c_pthread_key::setspecific(void *v)noexcept(true)
{
	return pthread_setspecific(f_key,v)==0;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cThreadLocalVariable::cThreadLocalVariable()noexcept(true)
	: fTS(PThreadKeyDestructor)
{
}
//---------------------------------------------------------------------------
cThreadLocalVariable::~cThreadLocalVariable()noexcept(true)
{
}
//---------------------------------------------------------------------------
void cThreadLocalVariable::Clear(void)noexcept(true)
{
	void *pData=fTS.getspecific();
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
void* cThreadLocalVariable::Get(void)noexcept(true)
{
	void *ThreadData=fTS.getspecific();
	if(ThreadData==nullptr)
		return nullptr;
	return static_cast<cThreadData*>(ThreadData)->Object;
}
//---------------------------------------------------------------------------
void cThreadLocalVariable::Set(iReference *Reference,void *Value)noexcept(true)
{
	void *pData=fTS.getspecific();
	cThreadData *ThreadData;
	if(pData==nullptr){
		if(Reference==nullptr && Value==nullptr){
			return;
		}
		ThreadData=new cThreadData;
		ThreadData->Reference=nullptr;
		ThreadData->Object=nullptr;
		ThreadData->NotifyProc=nullptr;
		fTS.setspecific(ThreadData);
	}
	else{
		ThreadData=static_cast<cThreadData*>(pData);
		if(ThreadData->Reference!=nullptr){
			ThreadData->Reference->DecreaseReference();
		}
	}
	ThreadData->Object=Value;
	ThreadData->Reference=Reference;
	if(Reference!=nullptr){
		Reference->IncreaseReference();
	}
}
//---------------------------------------------------------------------------
void cThreadLocalVariable::SetThreadExitNotify(iThreadExitNotifyProc *NotifyProc)noexcept(true)
{
	void *pData=fTS.getspecific();
	cThreadData *ThreadData;
	if(pData==nullptr){
		if(NotifyProc==nullptr){
			return;
		}
		ThreadData=new cThreadData;
		ThreadData->Reference=nullptr;
		ThreadData->Object=nullptr;
		ThreadData->NotifyProc=nullptr;
		fTS.setspecific(ThreadData);
	}
	else{
		ThreadData=static_cast<cThreadData*>(pData);
	}
	ThreadData->NotifyProc=NotifyProc;
}
//---------------------------------------------------------------------------
void cThreadLocalVariable::PThreadKeyDestructor(void *pThreadData)noexcept(true)
{
	if(pThreadData==nullptr)
		return;
	auto ThreadData=static_cast<cThreadData*>(pThreadData);
	if(ThreadData->NotifyProc!=nullptr){
		ThreadData->NotifyProc->Execute(rPtr<iReference>::TakeFromManual(ThreadData->Reference),ThreadData->Object);
	}
	else{
		if(ThreadData->Reference!=nullptr){
			ThreadData->Reference->DecreaseReference();
		}
	}
	delete ThreadData;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#ifdef	siOS_POSIX_ENABLE_THREAD_PRIORITY
//---------------------------------------------------------------------------
void cPOSIXThreadPriorityConverter::Setup(void)noexcept(true)
{
	if(fPriorityRange==0){
		fPriorityMin=sched_get_priority_min(SCHED_OTHER);
		fPriorityMax=sched_get_priority_max(SCHED_OTHER);
		fPriorityRange=fPriorityMax-fPriorityMin;
	}
}
sInt8 cPOSIXThreadPriorityConverter::POSIX2cnLib(int POSIXPriority)noexcept(true)
{
	Setup();
	if(POSIXPriority>=fPriorityMax)
		return 127;
	if(POSIXPriority<=fPriorityMin)
		return -128;
	int pRange=POSIXPriority-fPriorityMin;
	pRange=pRange*256/fPriorityRange;
	sInt8 Priority=static_cast<sInt8>(pRange)-128;
	return Priority;
}
int cPOSIXThreadPriorityConverter::cnLib2POSIX(sInt8 Priority)noexcept(true)
{
	Setup();
	int POSIXPriority=Priority;
	POSIXPriority+=128;
	POSIXPriority=POSIXPriority*fPriorityRange/256;
	POSIXPriority+=fPriorityMin;
	return POSIXPriority;
}
cPOSIXThreadPriorityConverter siPOSIX::gPOSIXThreadPriorityConverter;
//---------------------------------------------------------------------------
#endif	// siOS_POSIX_ENABLE_THREAD_PRIORITY
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cPOSIXThread::cModule::cModule()noexcept(true)
	: ThreadObject(ThreadObjectDestructor)
{
}
//---------------------------------------------------------------------------
cPOSIXThread::cModule::~cModule()noexcept(true)
{
}
//---------------------------------------------------------------------------
void cPOSIXThread::ThreadObjectDestructor(void *data)noexcept(true)
{
	auto Thread=iTake(static_cast<cPOSIXThread*>(data),'thsp');
	Thread->OnExit();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cPOSIXThread::cPOSIXThread()noexcept(true)
{
}
//---------------------------------------------------------------------------
cPOSIXThread::~cPOSIXThread()noexcept(true)
{
}
//---------------------------------------------------------------------------
iPtr<cPOSIXThread> cPOSIXThread::StartThread(iProcedure *Procedure)noexcept(true)
{
	auto NewThread=iCreate<cPOSIXThread>();
	auto *CreateParam=new pthread_param;
	CreateParam->Thread=NewThread;
	CreateParam->Procedure=Procedure;

	int threadError=pthread_create(&NewThread->fThread,nullptr,POSIXThreadMainRoutine,CreateParam);
    //cnLib_ASSERT(!returnVal);

    if(threadError!=0){
         // Report an error.
		 delete CreateParam;
		 return nullptr;
    }
	return NewThread;
}
//---------------------------------------------------------------------------
void* cPOSIXThread::POSIXThreadMainRoutine(void* data)noexcept(true)
{
	auto CreateParam=static_cast<pthread_param*>(data);
	auto ThreadObject=iExtract(CreateParam->Thread,'thsp');
	gModule.ThreadObject.setspecific(ThreadObject);

	auto *Proc=CreateParam->Procedure;
	delete CreateParam;

	Proc->Execute();
	return nullptr;
}
//---------------------------------------------------------------------------
void cPOSIXThread::OnExit(void)noexcept(true)
{
}
//---------------------------------------------------------------------------
cPOSIXThread* cPOSIXThread::CurrentThread(void)noexcept(true)
{
	auto pThreadObject=gModule.ThreadObject.getspecific();
	if(pThreadObject!=nullptr){
		return static_cast<cPOSIXThread*>(pThreadObject);
	}
	// attach to thread
	auto NewThread=iCreate<cPOSIXThread>();

	NewThread->fThread=pthread_self();

	auto pThread=iExtract(NewThread,'thsp');
	gModule.ThreadObject.setspecific(pThread);
	return pThread;

}
//---------------------------------------------------------------------------
cPOSIXThread* cPOSIXThread::GetCurrentThread(void)noexcept(true)
{
	auto pThread=gModule.ThreadObject.getspecific();
	return static_cast<cPOSIXThread*>(pThread);
}
//---------------------------------------------------------------------------
//bool cPOSIXThread::SleepUntilWake(void)noexcept(true)
//{
//	fSleepMutex.lock();
//
//	bool ByAlarm=false;
//	if(fPendingWake==false){
//		int ret=fSleepCond.wait(fSleepMutex);
//		ByAlarm=ret==0;
//	}
//	fPendingWake=false;
//
//	fSleepMutex.unlock();
//	return ByAlarm;
//}
//---------------------------------------------------------------------------
//bool cPOSIXThread::SleepUntil(const timespec &time)noexcept(true)
//{
//	fSleepMutex.lock();
//
//	bool ByAlarm=false;
//	while(fPendingWake==false){
//		int ret=fSleepCond.timedwait(fSleepMutex,time);
//		if(ret==0){
//			ByAlarm=true;
//		}
//		else{
//			// time out or fail
//			break;
//		}
//	}
//	fPendingWake=false;
//
//	fSleepMutex.unlock();
//	return ByAlarm;
//}
//---------------------------------------------------------------------------
//void cPOSIXThread::Wake(bool *ResetVal)noexcept(true)
//{
//	fSleepMutex.lock();
//	fPendingWake=true;
//	fSleepCond.signal();
//	if(ResetVal!=nullptr){
//		*ResetVal=false;
//	}
//	fSleepMutex.unlock();
//}
//---------------------------------------------------------------------------
bool cPOSIXThread::IsCurrentThread(void)noexcept(true)
{
	return pthread_self()==fThread;
}
//---------------------------------------------------------------------------
#ifdef	siOS_POSIX_ENABLE_THREAD_PRIORITY
//---------------------------------------------------------------------------
bool cPOSIXThread::SetPriority(pthread_t Thread,sInt8 Priority)noexcept(true)
{
	sched_param TS_param={0};

	TS_param.sched_priority=gPOSIXThreadPriorityConverter.cnLib2POSIX(Priority);
	return pthread_setschedparam(Thread,SCHED_RR,&TS_param)==0;
}
//---------------------------------------------------------------------------
bool cPOSIXThread::GetPriority(pthread_t Thread,sInt8 &Priority)noexcept(true)
{
	int TS_Policy;
	sched_param TS_param;

	if(pthread_getschedparam(Thread,&TS_Policy,&TS_param)!=0)
		return false;
	Priority=gPOSIXThreadPriorityConverter.POSIX2cnLib(TS_param.sched_priority);
	return true;
}
//---------------------------------------------------------------------------
// siOS_POSIX_ENABLE_THREAD_PRIORITY
#else
// !siOS_POSIX_ENABLE_THREAD_PRIORITY
//---------------------------------------------------------------------------
bool cPOSIXThread::SetPriority(pthread_t Thread,sInt8 Priority)noexcept(true)
{
	return false;
}
//---------------------------------------------------------------------------
bool cPOSIXThread::GetPriority(pthread_t Thread,sInt8 &Priority)noexcept(true)
{
	return false;
}
//---------------------------------------------------------------------------
#endif	// !siOS_POSIX_ENABLE_THREAD_PRIORITY
//---------------------------------------------------------------------------
bool cPOSIXThread::SetPriority(sInt8 Priority)noexcept(true)
{
	return SetPriority(fThread,Priority);
}
//---------------------------------------------------------------------------
bool cPOSIXThread::GetPriority(sInt8 &Priority)noexcept(true)
{
	return GetPriority(fThread,Priority);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void CurrentPOSIXThread::SuspendFor(uInt64 DelayNS)noexcept(true)
{
	timespec ts;
	ts.tv_sec=DelayNS/Time_1s;
	ts.tv_nsec=DelayNS%Time_1s;
	timespec trem;
	int ret=nanosleep(&ts,&trem);
	while(ret==EINTR){
		ret=nanosleep(&trem,&trem);
	}
}
////---------------------------------------------------------------------------
//bool CurrentPOSIXThread::SleepUntil(uInt64 SystemTime)noexcept(true)
//{
//	auto ThreadObject=cPOSIXThread::CurrentThread();
//	if(SystemTime==SystemTime_Never){
//		return ThreadObject->SleepUntilWake();
//	}
//	else{
//		auto tv=timespecFromNanoSeconds(SystemTime);
//		return ThreadObject->SleepUntil(tv);
//	}
//}
//---------------------------------------------------------------------------
void CurrentPOSIXThread::SwitchThread(void)noexcept(true)
{
	sched_yield();
}
//---------------------------------------------------------------------------
bool CurrentPOSIXThread::SetPriority(sInt8 Priority)noexcept(true)
{
	return cPOSIXThread::SetPriority(pthread_self(),Priority);
}
//---------------------------------------------------------------------------
sInt8 CurrentPOSIXThread::GetPriority(void)noexcept(true)
{
	sInt8 Priority;
	if(cPOSIXThread::GetPriority(pthread_self(),Priority))
		return Priority;
	return 0;
}
//---------------------------------------------------------------------------
cPOSIXThreadExecutionPool::cPOSIXThreadExecutionPool()noexcept(true)
    : fWaitingThreads(0)
	, fWorkingThreads(0)
{
}
//---------------------------------------------------------------------------
cPOSIXThreadExecutionPool::~cPOSIXThreadExecutionPool()noexcept(true)
{
	cnLib_ASSERT(fProcQueue.IsEmpty());
	auto Items=fProcItemRecycler.Swap(nullptr);
	DeleteProcItems(Items);
}
//---------------------------------------------------------------------------
void cPOSIXThreadExecutionPool::VirtualStarted(void)noexcept(true)
{
	InnerActivate('self');
	MakeThread();
}
//---------------------------------------------------------------------------
void cPOSIXThreadExecutionPool::VirtualStopped(void)noexcept(true)
{
	Close();
	InnerDecReference('self');
}
//---------------------------------------------------------------------------
void cPOSIXThreadExecutionPool::Close(void)noexcept(true)
{
	if(fClose)
		return;

	fClose=true;
	fCloseNotification.Setup();
	while(fWorkingThreads.Free.Load()!=0){
		fThreadWaitMutex.lock();
		fThreadWaitCond.broadcast();
		fThreadWaitMutex.unlock();
		fCloseNotification.Wait();
	};
	fCloseNotification.Clear();
}
//---------------------------------------------------------------------------
void cPOSIXThreadExecutionPool::MakeThread(void)noexcept(true)
{
	InnerIncReference('wktd');

	auto Thread=cPOSIXThread::StartThread(&fWorkThreadProcedure);
	if(Thread!=nullptr){
		++fWorkingThreads.Release;
	}
	else{
		// failed to start thread
		InnerDecReference('wktd');
	}
}
//---------------------------------------------------------------------------
void cPOSIXThreadExecutionPool::Post(iProcedure *Procedure)noexcept(true)
{
	if(fClose)
		return;

	auto Item=fProcItemRecycler.Pop();
	if(Item==nullptr){
		Item=new cProcItem;
	}
	Item->Procedure=Procedure;
	fProcQueue.Enqueue(Item);

	fProcQueueAvailable=true;
	fThreadWaitMutex.lock();
	fThreadWaitCond.signal();
	fThreadWaitMutex.unlock();
}
//---------------------------------------------------------------------------
void cPOSIXThreadExecutionPool::cWorkThreadProcedure::Execute(void)noexcept(true)
{
	auto Host=cnMemory::GetObjectFromMemberPointer(this,&cPOSIXThreadExecutionPool::fWorkThreadProcedure);
    Host->WorkThreadProcedure();
}
//---------------------------------------------------------------------------
void cPOSIXThreadExecutionPool::WorkThreadProcedure(void)noexcept(true)
{
	ufInt32 WaitingThreadCount=fWaitingThreads.Acquire.Load();
	for(;;){
		auto ProcItem=fProcQueue.Dequeue();
		if(ProcItem!=nullptr){
			if(fClose==false && WaitingThreadCount==0){
				// create extra thread to monitor queue
				MakeThread();
			}
			do{
				ProcItem->Procedure->Execute();
				fProcItemRecycler.Push(ProcItem);
			
				ProcItem=fProcQueue.Dequeue();
			}while(ProcItem!=nullptr);

		}
		
		ufInt32 WaitingThreadCount=++fWaitingThreads.Acquire;

		bool WaitInfinite=WaitingThreadCount==1;

		int WaitRet;
		fThreadWaitMutex.lock();
		if(fProcQueueAvailable){
			fProcQueueAvailable=false;
			WaitRet=0;
		}
		else{
			if(WaitInfinite){
				WaitRet=fThreadWaitCond.wait(fThreadWaitMutex);
			}
			else{
				timespec WaitTimeOut;
				clock_gettime(CLOCK_REALTIME, &WaitTimeOut);
				WaitTimeOut.tv_nsec+=30;
				WaitRet=fThreadWaitCond.timedwait(fThreadWaitMutex,WaitTimeOut);
			}
		}
		fThreadWaitMutex.unlock();

		WaitingThreadCount=--fWaitingThreads.Barrier;

		if(WaitRet==ETIMEDOUT){
			CompactRecycler(8);
			// time out
			if(WaitingThreadCount!=0){
				// exit thread
				break;
			}
		}
	}
	--fWorkingThreads.Release;
	fCloseNotification.Notify();
	InnerDecReference('wktd');
}
//---------------------------------------------------------------------------
void cPOSIXThreadExecutionPool::DeleteProcItems(cProcItem *Items)noexcept(true)
{
	while(Items==nullptr){
		auto DeleteItem=Items;
		Items=Items->Next;

		delete DeleteItem;
	}
}
//---------------------------------------------------------------------------
void cPOSIXThreadExecutionPool::CompactRecycler(uIntn LeastCount)noexcept(true)
{
	auto SaveItems=fProcItemRecycler.Swap(nullptr);
	if(SaveItems==nullptr)
		return;
	auto SaveLastItems=SaveItems;
	while(SaveLastItems->Next!=nullptr){
		if(LeastCount==0){
			DeleteProcItems(SaveLastItems->Next);
			break;
		}
		LeastCount--;
	}
	fProcItemRecycler.PushChain(SaveItems,SaveLastItems);
}
//---------------------------------------------------------------------------
#endif	// siOS_POSIX_ENABLE_THREAD
//---------------------------------------------------------------------------
