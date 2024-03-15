/*--- POSIX - Thread ------------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2012-05-28                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLib_siOS_POSIX_POSIX_Thread_H__
#define __cnLib_siOS_POSIX_POSIX_Thread_H__

#include <siOS/POSIX/POSIXHeader.h>

/*-------------------------------------------------------------------------*/
#ifdef	__cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
namespace siPOSIX{
//---------------------------------------------------------------------------
void iTimepoint2timespec(timespec &ts,iTimepoint *Time,uInt64 Delay);
//---------------------------------------------------------------------------
#ifdef	siOS_POSIX_ENABLE_THREAD
//---------------------------------------------------------------------------
class c_pthread_mutex
{
	// _POSIX_THREADS
public:
	c_pthread_mutex(pthread_mutexattr_t *attr=nullptr);
	~c_pthread_mutex();

	operator pthread_mutex_t* ();
	
	void lock(void);
	bool trylock(void);
	void unlock(void);

	void Acquire(void);
	bool TryAcquire(void);
	void Release(void);

protected:
	pthread_mutex_t f_mutex;
};
//---------------------------------------------------------------------------
class c_pthread_rwlock
{
	// _POSIX_THREADS
public:
	c_pthread_rwlock(pthread_rwlockattr_t *attr=nullptr);
	~c_pthread_rwlock();

	operator pthread_rwlock_t* ();

	void rdlock(void);
	bool tryrdlock(void);
	void wrlock(void);
	bool trywrlock(void);
	void unlock(void);

	void Acquire(void);
	bool TryAcquire(void);
	void Release(void);

	void AcquireShared(void);
	bool TryAcquireShared(void);
	void ReleaseShared(void);
protected:
	pthread_rwlock_t f_lock;
};
//---------------------------------------------------------------------------
class c_pthread_cond
{
	// _POSIX_THREADS
public:
	c_pthread_cond(pthread_condattr_t *attr=nullptr);
	~c_pthread_cond();

	void signal(void);
	void broadcast(void);
	void wait(pthread_mutex_t *mutex);
	int timedwait(pthread_mutex_t *mutex,const struct timespec &tv);
protected:
	pthread_cond_t f_cond;
};
//---------------------------------------------------------------------------
class c_pthread_key
{
	// _POSIX_THREADS
public:
	c_pthread_key(void (*destructor)(void*));
	~c_pthread_key();

	void* getspecific(void);
	bool setspecific(void *v);
protected:
	pthread_key_t f_key;
};
//- Thread variable ---------------------------------------------------------
class cThreadLocalVariable : public iThreadLocalVariable
{
public:
	cThreadLocalVariable();
	~cThreadLocalVariable();

	virtual void cnLib_FUNC Clear(void)noexcept(true)override;
	virtual void* cnLib_FUNC Get(void)noexcept(true)override;
	virtual void cnLib_FUNC Set(iReference *Reference,void *Value)noexcept(true)override;
	virtual void cnLib_FUNC SetThreadExitNotify(iThreadExitNotifyProc *NotifyProc)noexcept(true)override;
protected:
	c_pthread_key fTS;

	struct cThreadData
	{
		void *Object;
		iReference *Reference;
		iThreadExitNotifyProc *NotifyProc;
	};

	static void PThreadKeyDestructor(void *pThreadData);
};
//---------------------------------------------------------------------------
#ifdef	siOS_POSIX_ENABLE_THREAD_PRIORITY
//---------------------------------------------------------------------------
struct cPosixThreadPriorityConverter
{
	// _POSIX_THREAD_PRIORITY_SCHEDULING
public:
	int cnLib2POSIX(sInt8 Priority);
	sInt8 POSIX2cnLib(int POSIXPriority);
private:
	int fPriorityMin;
	int fPriorityMax;
	int fPriorityRange=0;
	void Setup(void);
};
extern cPosixThreadPriorityConverter gPosixThreadPriorityConverter;
//---------------------------------------------------------------------------
#endif	// siOS_POSIX_ENABLE_THREAD_PRIORITY
//---------------------------------------------------------------------------
class cPosixThread : public iThread
{
public:
	bool SleepUntil(const timespec *time);

	static iPtr<cPosixThread> StartThread(iProcedure *Procedure);
	static cPosixThread* CurrentThread(void);
	static cPosixThread* GetCurrentThread(void);

	// iThread

	virtual void cnLib_FUNC Wake(bool *ResetVal)override;
	virtual bool cnLib_FUNC SetPriority(sInt8 Priority)override;
	virtual bool cnLib_FUNC GetPriority(sInt8 &Priority)override;
	virtual bool cnLib_FUNC IsCurrentThread(void)override;

	static bool SetPriority(pthread_t Thread,sInt8 Priority);
	static bool GetPriority(pthread_t Thread,sInt8 &Priority);
protected:
	cPosixThread();
	~cPosixThread();
private:
	pthread_t fThread;

	c_pthread_cond fSleepCond;
	c_pthread_mutex fSleepMutex;
	bool fPendingWake=false;

	void OnExit(void);

	static void ThreadObjectDestructor(void *pThreadObject);
	class cModule
	{
	public:
		cModule();
		~cModule();
		c_pthread_key ThreadObject;
	};
	static cModule gModule;

	struct pthread_param : cnRTL::cRTLAllocator{
		iPtr<cPosixThread> Thread;
		iProcedure *Procedure;
	};
	static void* PosixThreadMainRoutine(void* data);

};
//---------------------------------------------------------------------------
namespace CurrentPosixThread{
//---------------------------------------------------------------------------
	bool SleepUntil(const timespec *time);
	void SwitchThread(void);
	bool SetPriority(sInt8 Priority);
	sInt8 GetPriority(void);
//---------------------------------------------------------------------------
}	// namespace CurrentPosixThread
//---------------------------------------------------------------------------
#endif	// siOS_POSIX_ENABLE_THREAD
//---------------------------------------------------------------------------
}	// namespace siPOSIX
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#endif	// __cplusplus
/*-------------------------------------------------------------------------*/
#endif
