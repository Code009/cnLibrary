#include "POSIX_Thread.h"

using namespace cnLibrary;
using namespace cnRTL;
using namespace siPOSIX;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/*
static void timespec_addns(timespec &ts,uint64 Nanoseconds)
{
	Nanoseconds+=ts.tv_nsec;
	ts.tv_sec+=Nanoseconds/Time_1s;
	ts.tv_nsec=Nanoseconds%Time_1s;
}*/
//---------------------------------------------------------------------------
void siPOSIX::iTimepoint2timespec(timespec &ts,iTimepoint *Time,uInt64 Delay)
{
	// absolute time
	sInt64 UnixTimeNS=Time->SystemTime();
	ts.tv_sec=static_cast<sInt32>(UnixTimeNS/Time_1s);
	ts.tv_nsec=UnixTimeNS%Time_1s;
	ts.tv_sec+=cnSystem::SystemTimeEpochSecondsSinceUnixEpoch;
}
//---------------------------------------------------------------------------
#ifdef	siOS_POSIX_ENABLE_THREAD
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
c_pthread_mutex::c_pthread_mutex(pthread_mutexattr_t *attr)
{
	pthread_mutex_init(&f_mutex,attr);
}
//---------------------------------------------------------------------------
c_pthread_mutex::~c_pthread_mutex()
{
	pthread_mutex_destroy(&f_mutex);
}
//---------------------------------------------------------------------------
c_pthread_mutex::operator pthread_mutex_t* ()
{
	return &f_mutex;
}
//---------------------------------------------------------------------------
void c_pthread_mutex::lock(void)
{
	pthread_mutex_lock(&f_mutex);
}
//---------------------------------------------------------------------------
bool c_pthread_mutex::trylock(void)
{
	return pthread_mutex_trylock(&f_mutex)==0;
}
//---------------------------------------------------------------------------
void c_pthread_mutex::unlock(void)
{
	pthread_mutex_unlock(&f_mutex);
}
//---------------------------------------------------------------------------
void c_pthread_mutex::Acquire(void){	return lock();	}
bool c_pthread_mutex::TryAcquire(void){	return trylock();	}
void c_pthread_mutex::Release(void){	return unlock();	}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
c_pthread_rwlock::c_pthread_rwlock(pthread_rwlockattr_t *attr)
{
	pthread_rwlock_init(&f_lock,attr);
}
//---------------------------------------------------------------------------
c_pthread_rwlock::~c_pthread_rwlock()
{
	pthread_rwlock_destroy(&f_lock);
}
//---------------------------------------------------------------------------
c_pthread_rwlock::operator pthread_rwlock_t* ()
{
	return &f_lock;
}
//---------------------------------------------------------------------------
void c_pthread_rwlock::rdlock(void)
{
	pthread_rwlock_rdlock(&f_lock);
}
//---------------------------------------------------------------------------
bool c_pthread_rwlock::tryrdlock(void)
{
	return pthread_rwlock_tryrdlock(&f_lock)==0;
}
//---------------------------------------------------------------------------
void c_pthread_rwlock::wrlock(void)
{
	pthread_rwlock_wrlock(&f_lock);
}
//---------------------------------------------------------------------------
bool c_pthread_rwlock::trywrlock(void)
{
	return pthread_rwlock_trywrlock(&f_lock)==0;
}
//---------------------------------------------------------------------------
void c_pthread_rwlock::unlock(void)
{
	pthread_rwlock_unlock(&f_lock);
}
//---------------------------------------------------------------------------
void c_pthread_rwlock::Acquire(void){	return wrlock();	}
bool c_pthread_rwlock::TryAcquire(void){	return trywrlock();	}
void c_pthread_rwlock::Release(void){	return unlock();	}

void c_pthread_rwlock::AcquireShared(void){	return rdlock();	}
bool c_pthread_rwlock::TryAcquireShared(void){	return tryrdlock();	}
void c_pthread_rwlock::ReleaseShared(void){	return unlock();	}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
c_pthread_cond::c_pthread_cond(pthread_condattr_t *attr)
{
	pthread_cond_init(&f_cond,attr);
}
//---------------------------------------------------------------------------
c_pthread_cond::~c_pthread_cond()
{
	pthread_cond_destroy(&f_cond);
}
//---------------------------------------------------------------------------
void c_pthread_cond::signal(void)
{
	pthread_cond_signal(&f_cond);
}
//---------------------------------------------------------------------------
void c_pthread_cond::broadcast(void)
{
	pthread_cond_broadcast(&f_cond);
}
//---------------------------------------------------------------------------
void c_pthread_cond::wait(pthread_mutex_t *mutex)
{
	pthread_cond_wait(&f_cond,mutex);
}
//---------------------------------------------------------------------------
int c_pthread_cond::timedwait(pthread_mutex_t *mutex,const struct timespec &tv)
{
	return pthread_cond_timedwait(&f_cond,mutex,&tv);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
c_pthread_key::c_pthread_key(void (*destructor)(void*))
{
	pthread_key_create(&f_key,destructor);
}
//---------------------------------------------------------------------------
c_pthread_key::~c_pthread_key()
{
	pthread_key_delete(f_key);
}
//---------------------------------------------------------------------------
void* c_pthread_key::getspecific(void)
{
	return pthread_getspecific(f_key);
}
//---------------------------------------------------------------------------
bool c_pthread_key::setspecific(void *v)
{
	return pthread_setspecific(f_key,v)==0;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cThreadLocalVariable::cThreadLocalVariable()
	: fTS(PThreadKeyDestructor)
{
}
//---------------------------------------------------------------------------
cThreadLocalVariable::~cThreadLocalVariable()
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
void cThreadLocalVariable::PThreadKeyDestructor(void *pThreadData)
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
void cPosixThreadPriorityConverter::Setup(void)
{
	if(fPriorityRange==0){
		fPriorityMin=sched_get_priority_min(SCHED_OTHER);
		fPriorityMax=sched_get_priority_max(SCHED_OTHER);
		fPriorityRange=fPriorityMax-fPriorityMin;
	}
}
sInt8 cPosixThreadPriorityConverter::POSIX2cnLib(int POSIXPriority)
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
int cPosixThreadPriorityConverter::cnLib2POSIX(sInt8 Priority)
{
	Setup();
	int POSIXPriority=Priority;
	POSIXPriority+=128;
	POSIXPriority=POSIXPriority*fPriorityRange/256;
	POSIXPriority+=fPriorityMin;
	return POSIXPriority;
}
cPosixThreadPriorityConverter siPOSIX::gPosixThreadPriorityConverter;
//---------------------------------------------------------------------------
#endif	// siOS_POSIX_ENABLE_THREAD_PRIORITY
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cPosixThread::cModule::cModule()
	: ThreadObject(ThreadObjectDestructor)
{
}
//---------------------------------------------------------------------------
cPosixThread::cModule::~cModule()
{
}
//---------------------------------------------------------------------------
void cPosixThread::ThreadObjectDestructor(void *data)
{
	auto Thread=iTake(static_cast<cPosixThread*>(data),'thsp');
	Thread->OnExit();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cPosixThread::cPosixThread()
{
}
//---------------------------------------------------------------------------
cPosixThread::~cPosixThread()
{
}
//---------------------------------------------------------------------------
iPtr<cPosixThread> cPosixThread::StartThread(iProcedure *Procedure)
{
	auto NewThread=iCreate<cPosixThread>();
	auto *CreateParam=new pthread_param;
	CreateParam->Thread=NewThread;
	CreateParam->Procedure=Procedure;

	int threadError=pthread_create(&NewThread->fThread,nullptr,PosixThreadMainRoutine,CreateParam);
    //cnLib_ASSERT(!returnVal);

    if(threadError!=0){
         // Report an error.
		 delete CreateParam;
		 return nullptr;
    }
	return NewThread;
}
//---------------------------------------------------------------------------
void* cPosixThread::PosixThreadMainRoutine(void* data)
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
void cPosixThread::OnExit(void)
{
}
//---------------------------------------------------------------------------
cPosixThread* cPosixThread::CurrentThread(void)
{
	auto pThreadObject=gModule.ThreadObject.getspecific();
	if(pThreadObject!=nullptr){
		return static_cast<cPosixThread*>(pThreadObject);
	}
	// attach to thread
	auto NewThread=iCreate<cPosixThread>();

	NewThread->fThread=pthread_self();

	auto pThread=iExtract(NewThread,'thsp');
	gModule.ThreadObject.setspecific(pThread);
	return NewThread;

}
//---------------------------------------------------------------------------
cPosixThread* cPosixThread::GetCurrentThread(void)
{
	auto pThread=gModule.ThreadObject.getspecific();
	return static_cast<cPosixThread*>(pThread);
}
//---------------------------------------------------------------------------
bool cPosixThread::SleepUntil(const timespec *time)
{
	fSleepMutex.lock();

	bool ByAlarm=false;
	while(fPendingWake==false){
		if(time!=nullptr){
			int ret=fSleepCond.timedwait(fSleepMutex,*time);
			if(ret==ETIMEDOUT){
				// time out
				break;
			}
			else{
				ByAlarm=true;
			}
		}
		else{
			fSleepCond.wait(fSleepMutex);
			ByAlarm=true;
		}
	}
	fPendingWake=false;

	fSleepMutex.unlock();
	return ByAlarm;
}
//---------------------------------------------------------------------------
void cPosixThread::Wake(bool *ResetVal)
{
	fSleepMutex.lock();
	fPendingWake=true;
	fSleepCond.signal();
	if(ResetVal!=nullptr){
		*ResetVal=false;
	}
	fSleepMutex.unlock();
}
//---------------------------------------------------------------------------
bool cPosixThread::IsCurrentThread(void)
{
	return pthread_self()==fThread;
}
//---------------------------------------------------------------------------
#ifdef	siOS_POSIX_ENABLE_THREAD_PRIORITY
//---------------------------------------------------------------------------
bool cPosixThread::SetPriority(pthread_t Thread,sInt8 Priority)
{
	sched_param TS_param={0};

	TS_param.sched_priority=gPosixThreadPriorityConverter.cnLib2POSIX(Priority);
	return pthread_setschedparam(Thread,SCHED_RR,&TS_param)==0;
}
//---------------------------------------------------------------------------
bool cPosixThread::GetPriority(pthread_t Thread,sInt8 &Priority)
{
	int TS_Policy;
	sched_param TS_param;

	if(pthread_getschedparam(Thread,&TS_Policy,&TS_param)!=0)
		return false;
	Priority=gPosixThreadPriorityConverter.POSIX2cnLib(TS_param.sched_priority);
	return true;
}
//---------------------------------------------------------------------------
// siOS_POSIX_ENABLE_THREAD_PRIORITY
#else
// !siOS_POSIX_ENABLE_THREAD_PRIORITY
//---------------------------------------------------------------------------
bool cPosixThread::SetPriority(pthread_t Thread,sInt8 Priority)
{
	return false;
}
//---------------------------------------------------------------------------
bool cPosixThread::GetPriority(pthread_t Thread,sInt8 &Priority)
{
	return false;
}
//---------------------------------------------------------------------------
#endif	// !siOS_POSIX_ENABLE_THREAD_PRIORITY
//---------------------------------------------------------------------------
bool cPosixThread::SetPriority(sInt8 Priority)
{
	return SetPriority(fThread,Priority);
}
//---------------------------------------------------------------------------
bool cPosixThread::GetPriority(sInt8 &Priority)
{
	return GetPriority(fThread,Priority);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool CurrentPosixThread::SleepUntil(const timespec *time)
{
	auto ThreadObject=cPosixThread::CurrentThread();
	return ThreadObject->SleepUntil(time);
}
//---------------------------------------------------------------------------
void CurrentPosixThread::SwitchThread(void)
{
	sched_yield();
}
//---------------------------------------------------------------------------
bool CurrentPosixThread::SetPriority(sInt8 Priority)
{
	return cPosixThread::SetPriority(pthread_self(),Priority);
}
//---------------------------------------------------------------------------
sInt8 CurrentPosixThread::GetPriority(void)
{
	sInt8 Priority;
	if(cPosixThread::GetPriority(pthread_self(),Priority))
		return Priority;
	return 0;
}
//---------------------------------------------------------------------------
#endif	// siOS_POSIX_ENABLE_THREAD
//---------------------------------------------------------------------------
