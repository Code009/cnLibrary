/*- cnSystem - Threading --------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2008-09-06                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnSystem_cnThread_H__
#define	__cnLibrary_cnSystem_cnThread_H__
/*-------------------------------------------------------------------------*/
#include <cnSystem/cnFundamental.h>
#include <cnSystem/cnData.h>
/*-------------------------------------------------------------------------*/
#ifdef __cplusplus
//---------------------------------------------------------------------------
#if cnLibrary_CPPFEATURE_CONSTEXPR >= 200704L
#define	cnLib_CONSTVAR	constexpr
// cnLibrary_CPPFEATURE_CONSTEXPR >= 200704L
#else
// cnLibrary_CPPFEATURE_CONSTEXPR < 200704L
#define	cnLib_CONSTVAR	const
#endif	// cnLibrary_CPPFEATURE_CONSTEXPR >= 200704L

#ifdef	cnLibrary_CPPEXCLUDE_VIRTUAL_OVERRIDE
#define	override
#endif
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
static cnLib_CONSTVAR uInt64 Time_1s	=1000000000;
static cnLib_CONSTVAR uInt64 Time_1ms	=1000000;
static cnLib_CONSTVAR uInt64 Time_1us	=1000;
static cnLib_CONSTVAR uInt64 Time_1ns	=1;
static cnLib_CONSTVAR uInt64 SystemTime_Never	=0xFFFFFFFFFFFFFFFF;
//---------------------------------------------------------------------------
struct cDateTime
{
	uInt32 NanoSecond;
	sInt16 Year;
	uInt8 Month;
	uInt8 Day;

	uInt8 Hour;
	uInt8 Minute;
	uInt8 Second;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iAsyncTask : public iInterface
{
public:
	// IsDone
	//	check if the operation is done
	// return true if operation is done, or false is in progress
	virtual bool cnLib_FUNC IsDone(void)noexcept(true)=0;

	// Await
	// return true if notification registered, or false if the task is already done
	virtual bool cnLib_FUNC Await(iProcedure *NotifyProcedure)noexcept(true)=0;

	void Result(void)noexcept(true){}
};
//---------------------------------------------------------------------------
template<class TResult>
class cnLib_INTERFACE iAsyncFunction : public iAsyncTask
{
public:
	virtual TResult cnLib_FUNC Result(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iMutex : public iReference
{
public:
	// Acquire
	//	Acquires the lock in exclusive mode
	virtual void cnLib_FUNC Acquire(void)noexcept(true)=0;
	// TryAcquire
	//	Attempts to acquires the lock in exclusive mode
	virtual bool cnLib_FUNC TryAcquire(void)noexcept(true)=0;
	// Release
	//	Release the lock in exclusive mode
	virtual void cnLib_FUNC Release(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iSharedMutex : public iMutex
{
public:
	// Acquire
	//	Acquires the lock in shared mode
	virtual void cnLib_FUNC AcquireShared(void)noexcept(true)=0;
	// TryAcquire
	//	Attempts to acquires the lock in shared mode
	virtual bool cnLib_FUNC TryAcquireShared(void)noexcept(true)=0;
	// Release
	//	Release the lock in shared mode
	virtual void cnLib_FUNC ReleaseShared(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
typedef iFunction<void (rPtr<iReference>,void*)noexcept(true)> iThreadExitNotifyProc;
class cnLib_INTERFACE iThreadLocalVariable : public iVariable
{
public:
	virtual void cnLib_FUNC SetThreadExitNotify(iThreadExitNotifyProc *NotifyProc)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iThread : public iInterface
{
public:
	struct tInterfaceID{	static iTypeID Value;	};
	virtual void* cnLib_FUNC CastInterface(iTypeID ID)noexcept(true) override{		return cnClass::ImplementCastInterface(this,ID);	}

	// SetPriority
	//	Sets the priority value for the thread
	// [in]Priority		priority value
	// return true if priority is set
	virtual bool cnLib_FUNC SetPriority(sInt8 Priority)noexcept(true)=0;
	// GetPriority
	//	Retrieves the priority value for the thread
	// [out]Priority	priority value
	// return true if priority is get
	virtual bool cnLib_FUNC GetPriority(sInt8 &Priority)noexcept(true)=0;

	// IsCurrentThread
	//	check if the calling thread is the thread
	virtual bool cnLib_FUNC IsCurrentThread(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iAsyncProcedure : public iReference
{
public:
	virtual void cnLib_FUNC Start(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iAsyncTimer : public iReference
{
public:
	// Start
	//	start timer
	// [in]DueTime		When the timer to be hit
	// [in]Period		The timer period, in nanoseconds. 0 if not to hit periodly
	virtual void cnLib_FUNC Start(uInt64 DueTime,uInt64 Period)noexcept(true)=0;
	// Stop
	//	stop timer
	virtual void cnLib_FUNC Stop(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iAsyncExecution : public iInterface
{
public:
	// Execute
	//	execute procedure in execution unit.
	// Reference		Reference to the procedure
	// Procedure		procedure to be called
	virtual void cnLib_FUNC Execute(iReference *Reference,iProcedure *Procedure)noexcept(true)=0;

	// CreateWork
	//	Creates a work execute in execution unit.
	// [in]ThreadProcedure	Work thread procedure, thread pool object will hold a weak reference to the procedure
	// return: new object
	virtual rPtr<iAsyncProcedure>	cnLib_FUNC CreateWork(iReference *Reference,iProcedure *ThreadProcedure)noexcept(true)=0;

	// CreateTimer
	//	Creates a timer execute in execution unit.
	// [in]ThreadProcedure	timer thread procedure, thread pool object will hold a weak reference to the procedure
	// return: new object
	virtual rPtr<iAsyncTimer>		cnLib_FUNC CreateTimer(iReference *Reference,iProcedure *ThreadProcedure)noexcept(true)=0;

};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iThreadPool : public iAsyncExecution
{
public:
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iDispatch : public iAsyncExecution
{
public:
	virtual bool cnLib_FUNC IsCurrentThread(void)noexcept(true)=0;

	// ExecuteSync
	//	execute procedure in dispatch thread and wait for its completion
	// Procedure		procedure to be called
	virtual void cnLib_FUNC ExecuteSync(iProcedure *Procedure)noexcept(true)=0;

	// ExecuteAsync
	//	execute procedure in dispatch thread
	// Reference		Reference to the procedure
	// Procedure		procedure to be called
	// return: task for the procedure
	virtual iPtr<iAsyncTask> cnLib_FUNC ExecuteAsync(iReference *Reference,iProcedure *Procedure)noexcept(true)=0;
};
//---------------------------------------------------------------------------
namespace cnSystem{
//---------------------------------------------------------------------------
extern const sInt64 SystemTimeEpochSecondsSinceUnixEpoch;
uInt64 cnLib_FUNC GetSystemTimeNow(void)noexcept(true);

bool cnLib_FUNC UTCGregorianDateFromSystemTime(cDateTime &DateTime,uInt64 SystemTime)noexcept(true);
bool cnLib_FUNC UTCGregorianDateToSystemTime(uInt64 &SystemTime,const cDateTime &DateTime)noexcept(true);

//---------------------------------------------------------------------------

// sync

rPtr<iMutex> cnLib_FUNC CreateMutexLock(void)noexcept(true);
rPtr<iMutex> cnLib_FUNC CreateRecursiveLock(void)noexcept(true);
rPtr<iSharedMutex> cnLib_FUNC CreateSharedLock(void)noexcept(true);

//---------------------------------------------------------------------------
namespace CurrentThread{
//---------------------------------------------------------------------------
// SwitchThread
//	yield execution to another thread that is ready to run
void cnLib_FUNC SwitchThread(void)noexcept(true);

// SuspendFor
//	Suspends the execution of the current thread until the time passed
// [in]Delay	nanoseconds to suspend
void cnLib_FUNC SuspendFor(uInt64 Delay)noexcept(true);

// GetThread
//	get thread interface for other thread use
// return thread object
iThread*	cnLib_FUNC GetThread(void)noexcept(true);

// SetPriority
//	Sets the priority value for the thread
// [in]Priority		priority value
bool cnLib_FUNC SetPriority(sInt8 Priority)noexcept(true);
// GetPriority
//	Retrieves the priority value for the thread
// return: priority value
sInt8 cnLib_FUNC GetPriority(void)noexcept(true);
//---------------------------------------------------------------------------
}	// namespace CurrentThread
//---------------------------------------------------------------------------
// thread
rPtr<iThreadLocalVariable>	cnLib_FUNC CreateThreadLocalVariable(void)noexcept(true);
iPtr<iThread>				cnLib_FUNC StartThread(iProcedure *ThreadProcedure)noexcept(true);
iPtr<iDispatch>				cnLib_FUNC CreateDispatchThread(void)noexcept(true);
//- Thread pool -------------------------------------------------------------
extern iThreadPool*const DefaultThreadPool;
//---------------------------------------------------------------------------
}   // namespace cnSystem
//---------------------------------------------------------------------------
//class cnLib_INTERFACE iDataCache : public iInterface
//{
//public:
//	struct tInterfaceID{	static iTypeID Value;	};
//	virtual void* cnLib_FUNC CastInterface(iTypeID ID)noexcept(true) override{		return cnVar::ImplementCastInterface(this,ID);	}
//
//	virtual bool cnLib_FUNC IsCached(void)noexcept(true)=0;
//	virtual void cnLib_FUNC Cache(void)noexcept(true)=0;
//	virtual iPtr<iAsyncTask> cnLib_FUNC CacheAsync(void)noexcept(true)=0;
//};
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#undef	cnLib_CONSTVAR
#ifdef	cnLibrary_CPPEXCLUDE_VIRTUAL_OVERRIDE
#undef	override
#endif
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif
