/*--- POSIX - Thread ------------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2012-05-28                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLib_POSIX_POSIX_Thread_H__
#define __cnLib_POSIX_POSIX_Thread_H__

#include <POSIX/POSIXCommon.h>

/*-------------------------------------------------------------------------*/
#ifdef	__cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
namespace siPOSIX{
//---------------------------------------------------------------------------
#ifdef	siOS_POSIX_ENABLE_THREAD
//---------------------------------------------------------------------------
class c_pthread_mutex
{
	// _POSIX_THREADS
public:
	c_pthread_mutex(pthread_mutexattr_t *attr=nullptr)noexcept(true);
	~c_pthread_mutex();

	operator pthread_mutex_t* ()noexcept(true);
	
	void lock(void)noexcept(true);
	bool trylock(void)noexcept(true);
	void unlock(void)noexcept(true);

	void Acquire(void)noexcept(true);
	bool TryAcquire(void)noexcept(true);
	void Release(void)noexcept(true);

protected:
	pthread_mutex_t f_mutex;
};
//---------------------------------------------------------------------------
class c_pthread_rwlock
{
	// _POSIX_THREADS
public:
	c_pthread_rwlock(pthread_rwlockattr_t *attr=nullptr)noexcept(true);
	~c_pthread_rwlock()noexcept(true);

	operator pthread_rwlock_t* ()noexcept(true);

	void rdlock(void)noexcept(true);
	bool tryrdlock(void)noexcept(true);
	void wrlock(void)noexcept(true);
	bool trywrlock(void)noexcept(true);
	void unlock(void)noexcept(true);

	void Acquire(void)noexcept(true);
	bool TryAcquire(void)noexcept(true);
	void Release(void)noexcept(true);

	void AcquireShared(void)noexcept(true);
	bool TryAcquireShared(void)noexcept(true);
	void ReleaseShared(void)noexcept(true);
protected:
	pthread_rwlock_t f_lock;
};
//---------------------------------------------------------------------------
class c_pthread_cond
{
	// _POSIX_THREADS
public:
	c_pthread_cond(pthread_condattr_t *attr=nullptr)noexcept(true);
	~c_pthread_cond()noexcept(true);

	void signal(void)noexcept(true);
	void broadcast(void)noexcept(true);
	int wait(pthread_mutex_t *mutex)noexcept(true);
	int timedwait(pthread_mutex_t *mutex,const struct timespec &tv)noexcept(true);
protected:
	pthread_cond_t f_cond;
};
//---------------------------------------------------------------------------
class c_pthread_key
{
	// _POSIX_THREADS
public:
	c_pthread_key(void (*destructor)(void*)noexcept(true))noexcept(true);
	~c_pthread_key()noexcept(true);

	void* getspecific(void)noexcept(true);
	bool setspecific(void *v)noexcept(true);
protected:
	pthread_key_t f_key;
};
//- Thread variable ---------------------------------------------------------
class cThreadLocalVariable : public iThreadLocalVariable
{
public:
	cThreadLocalVariable()noexcept(true);
	~cThreadLocalVariable()noexcept(true);

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

	static void PThreadKeyDestructor(void *pThreadData)noexcept(true);
};
//---------------------------------------------------------------------------
#ifdef	siOS_POSIX_ENABLE_THREAD_PRIORITY
//---------------------------------------------------------------------------
struct cPOSIXThreadPriorityConverter
{
	// _POSIX_THREAD_PRIORITY_SCHEDULING
public:
	int cnLib2POSIX(sInt8 Priority)noexcept(true);
	sInt8 POSIX2cnLib(int POSIXPriority)noexcept(true);
private:
	int fPriorityMin;
	int fPriorityMax;
	int fPriorityRange=0;
	void Setup(void)noexcept(true);
};
extern cPOSIXThreadPriorityConverter gPOSIXThreadPriorityConverter;
//---------------------------------------------------------------------------
#endif	// siOS_POSIX_ENABLE_THREAD_PRIORITY
//---------------------------------------------------------------------------
class cPOSIXThread : public iThread
{
public:
	static iPtr<cPOSIXThread> StartThread(iProcedure *Procedure)noexcept(true);
	static cPOSIXThread* CurrentThread(void)noexcept(true);
	static cPOSIXThread* GetCurrentThread(void)noexcept(true);

	// iThread

	virtual bool cnLib_FUNC SetPriority(sInt8 Priority)noexcept(true)override;
	virtual bool cnLib_FUNC GetPriority(sInt8 &Priority)noexcept(true)override;
	virtual bool cnLib_FUNC IsCurrentThread(void)noexcept(true)override;

	static bool SetPriority(pthread_t Thread,sInt8 Priority)noexcept(true);
	static bool GetPriority(pthread_t Thread,sInt8 &Priority)noexcept(true);
protected:
	cPOSIXThread()noexcept(true);
	~cPOSIXThread()noexcept(true);
private:
	pthread_t fThread;

	void OnExit(void)noexcept(true);

	static void ThreadObjectDestructor(void *pThreadObject)noexcept(true);
	class cModule
	{
	public:
		cModule()noexcept(true);
		~cModule()noexcept(true);
		c_pthread_key ThreadObject;
	};
	static cModule gModule;

	struct pthread_param : cnRTL::cRTLAllocator{
		iPtr<cPOSIXThread> Thread;
		iProcedure *Procedure;
	};
	static void* POSIXThreadMainRoutine(void* data)noexcept(true);

};
//---------------------------------------------------------------------------
namespace CurrentPOSIXThread{
//---------------------------------------------------------------------------
	void SuspendFor(uInt64 DelayNS)noexcept(true);
	void SwitchThread(void)noexcept(true);
	bool SetPriority(sInt8 Priority)noexcept(true);
	sInt8 GetPriority(void)noexcept(true);
//---------------------------------------------------------------------------
}	// namespace CurrentPOSIXThread
//---------------------------------------------------------------------------
class cPOSIXThreadExecutionPool : public iReference, public cnRTL::cDualReference
{
public:
    cPOSIXThreadExecutionPool()noexcept(true);
    ~cPOSIXThreadExecutionPool()noexcept(true);

    void Close(void)noexcept(true);

	void Post(iProcedure *Procedure)noexcept(true);

protected:
	void VirtualStarted(void)noexcept(true);
	void VirtualStopped(void)noexcept(true);

	
    void MakeThread(void)noexcept(true);

	class cWorkThreadProcedure : public iProcedure
	{
		virtual void cnLib_FUNC Execute(void)noexcept(true)override;
	}fWorkThreadProcedure;


	bool fClose=false;
	bool fProcQueueAvailable=false;
	c_pthread_mutex fThreadWaitMutex;
	c_pthread_cond fThreadWaitCond;
	TKRuntime::Thread::tSingleNotification fCloseNotification;

    cnRTL::cAtomicVar<ufInt32> fWaitingThreads;
    cnRTL::cAtomicVar<ufInt32> fWorkingThreads;
    void WorkThreadProcedure(void)noexcept(true);

	struct cProcItem : cnRTL::cRTLAllocator
	{
		cProcItem *Next;
		iProcedure *Procedure;
	};
	cnRTL::cAtomicQueue<cProcItem> fProcQueue;
	cnRTL::cAtomicStack<cProcItem> fProcItemRecycler;

	static void DeleteProcItems(cProcItem *Items)noexcept(true);
	void CompactRecycler(uIntn LeastCount)noexcept(true);
};
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
