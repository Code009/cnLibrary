#include <cnRTL/cnRTLCore.h>

#include <pthread.h>


using namespace cnLibrary;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void TKRuntime::Thread::Yield(void)noexcept(true)
{
	::sched_yield();
}
//---------------------------------------------------------------------------
void TKRuntime::Thread::SpinWait(uIntn SpinCount)noexcept(true)
{
	::sched_yield();
}
//---------------------------------------------------------------------------
void TKRuntime::Thread::Suspend(uInt64 Duration)noexcept(true)
{
	timespec ts;
	ts.tv_sec=Duration/Time_1s;
	ts.tv_nsec=Duration%Time_1s;
	timespec trem;
	int ret=nanosleep(&ts,&trem);
	while(ret==EINTR){
		ret=nanosleep(&trem,&trem);
	}
}
//---------------------------------------------------------------------------
TKRuntime::Thread::tThreadID TKRuntime::Thread::GetID(void)noexcept(true)
{
	tThreadID RetValue={pthread_self()};
	return RetValue;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool TKRuntime::Thread::tThreadID::operator ==(const tThreadID &Src)noexcept(true)
{
	return 0!=pthread_equal(IDValue,Src.IDValue);
}
//---------------------------------------------------------------------------
bool TKRuntime::Thread::tThreadID::operator !=(const tThreadID &Src)noexcept(true)
{
	return 0==pthread_equal(IDValue,Src.IDValue);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void TKRuntime::Thread::tSingleNotification::Setup(void)noexcept(true)
{
	pthread_mutex_init(&Mutex,nullptr);
	pthread_cond_init(&Condition,nullptr);
	WaitFlag.Notified=false;
}
//---------------------------------------------------------------------------
void TKRuntime::Thread::tSingleNotification::Clear(void)noexcept(true)
{
	pthread_cond_destroy(&Condition);
	pthread_mutex_destroy(&Mutex);
}
//---------------------------------------------------------------------------
void TKRuntime::Thread::tSingleNotification::Wait(void)noexcept(true)
{
	pthread_mutex_lock(&Mutex);

	while(WaitFlag.Notified==false){
		pthread_cond_wait(&Condition,&Mutex);
	}

	pthread_mutex_unlock(&Mutex);
}
//---------------------------------------------------------------------------
bool TKRuntime::Thread::tSingleNotification::Wait(ufInt64 Duration)noexcept(true)
{
	timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);

	Duration+=ts.tv_nsec;
	ts.tv_nsec=Duration%Time_1s;
	ts.tv_sec+=Duration/Time_1s;
	pthread_mutex_lock(&Mutex);

	bool WaitSuccess=true;
	while(WaitFlag.Notified==false){
		int Ret=pthread_cond_timedwait(&Condition,&Mutex,&ts);
		if(Ret==ETIMEDOUT){
			WaitSuccess=false;
			break;
		}
	}

	pthread_mutex_unlock(&Mutex);
	return WaitSuccess;
}
//---------------------------------------------------------------------------
void TKRuntime::Thread::tSingleNotification::Notify(void)noexcept(true)
{
	pthread_mutex_lock(&Mutex);

	WaitFlag.Notified=true;

	pthread_mutex_unlock(&Mutex);

	pthread_cond_signal(&Condition);
}
//---------------------------------------------------------------------------
