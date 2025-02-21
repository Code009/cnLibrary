#include "POSIXCore_Thread.h"

#include <cnSystem/cnThread.h>

using namespace cnLibrary;
using namespace siPOSIX;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void RTLCore::cSingleNotification::Setup(void)noexcept(true)
{
	pthread_mutex_init(&Mutex,nullptr);
	pthread_cond_init(&Condition,nullptr);
	WaitFlag.Notified=false;
}
//---------------------------------------------------------------------------
void RTLCore::cSingleNotification::Clear(void)noexcept(true)
{
	pthread_cond_destroy(&Condition);
	pthread_mutex_destroy(&Mutex);
}
//---------------------------------------------------------------------------
void RTLCore::cSingleNotification::Wait(void)noexcept(true)
{
	pthread_mutex_lock(&Mutex);

	while(WaitFlag.Notified==false){
		pthread_cond_wait(&Condition,&Mutex);
	}

	pthread_mutex_unlock(&Mutex);
}
//---------------------------------------------------------------------------
bool RTLCore::cSingleNotification::WaitFor(ufInt64 Duration)noexcept(true)
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
void RTLCore::cSingleNotification::Notify(void)noexcept(true)
{
	pthread_mutex_lock(&Mutex);

	WaitFlag.Notified=true;

	pthread_mutex_unlock(&Mutex);

	pthread_cond_signal(&Condition);
}
//---------------------------------------------------------------------------
