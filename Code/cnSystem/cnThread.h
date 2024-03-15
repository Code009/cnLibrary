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
static cnLib_CONSTVAR sInt64 Time_1s	=1000000000;
static cnLib_CONSTVAR sInt64 Time_1ms	=1000000;
static cnLib_CONSTVAR sInt64 Time_1us	=1000;
static cnLib_CONSTVAR sInt64 Time_1ns	=1;
//- Timepoint ---------------------------------------------------------------
class cnLib_INTERFACE iTimepoint : public iInterface
{
public:
	//	nanoseconds since system epoch
	virtual sInt64 cnLib_FUNC SystemTime(void)=0;
	virtual sInt64 cnLib_FUNC SinceTime(iTimepoint *Time)=0;
};
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
	virtual bool cnLib_FUNC IsDone(void)=0;

	// SetNotify
	// return true if notification registered, or false if the task is already done
	virtual bool cnLib_FUNC SetNotify(iProcedure *NotifyProcedure)=0;
};
//---------------------------------------------------------------------------
template<class TResult>
class cnLib_INTERFACE iAsyncFunction : public iAsyncTask
{
public:
	// Cancel
	virtual void cnLib_FUNC Cancel(void)=0;

	virtual TResult cnLib_FUNC GetResult(void)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iMutex : public iReference
{
public:
	// Acquire
	//	Acquires the lock in exclusive mode
	virtual void cnLib_FUNC Acquire(void)=0;
	// TryAcquire
	//	Attempts to acquires the lock in exclusive mode
	virtual bool cnLib_FUNC TryAcquire(void)=0;
	// Release
	//	Release the lock in exclusive mode
	virtual void cnLib_FUNC Release(void)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iSharedMutex : public iMutex
{
public:
	// Acquire
	//	Acquires the lock in shared mode
	virtual void cnLib_FUNC AcquireShared(void)=0;
	// TryAcquire
	//	Attempts to acquires the lock in shared mode
	virtual bool cnLib_FUNC TryAcquireShared(void)=0;
	// Release
	//	Release the lock in shared mode
	virtual void cnLib_FUNC ReleaseShared(void)=0;
};
//---------------------------------------------------------------------------
typedef iFunction<void (rPtr<iReference>,void*)> iThreadExitNotifyProc;
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
	virtual void* cnLib_FUNC CastInterface(iTypeID ID)noexcept(true) override{		return cnVar::ImplementCastInterface(this,ID);	}

	// Wake
	//	wake thread on sleep
	// [in]ResetVal		pointer to bool value to reset zero before thread awakes
	virtual void cnLib_FUNC Wake(bool *ResetVal)=0;
	// SetPriority
	//	Sets the priority value for the thread
	// [in]Priority		priority value
	// return true if priority is set
	virtual bool cnLib_FUNC SetPriority(sInt8 Priority)=0;
	// GetPriority
	//	Retrieves the priority value for the thread
	// [out]Priority	priority value
	// return true if priority is get
	virtual bool cnLib_FUNC GetPriority(sInt8 &Priority)=0;

	// IsCurrentThread
	//	check if the calling thread is the thread
	virtual bool cnLib_FUNC IsCurrentThread(void)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iAsyncProcedure : public iReference
{
public:
	virtual void cnLib_FUNC Start(void)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iAsyncTimer : public iReference
{
public:
	// Start
	//	start timer
	// [in]DueTime		When the timer to be hit. should not be nullptr
	// [in]DueTimeDelay	The amount of time to delay the timer hitting, in nanoseconds.
	// [in]Period		The timer period, in nanoseconds. 0 if not to hit periodly
	virtual void cnLib_FUNC Start(iTimepoint *DueTime,sInt64 DueTimeDelay,uInt64 Period)=0;
	// Stop
	//	stop timer
	virtual void cnLib_FUNC Stop(void)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iAsyncExecution : public iInterface
{
public:
	// Execute
	//	execute procedure in execution unit.
	// Reference		Reference to the procedure
	// Procedure		procedure to be called
	virtual void cnLib_FUNC Execute(iReference *Reference,iProcedure *Procedure)=0;

	// CreateWork
	//	Creates a work execute in execution unit.
	// [in]ThreadProcedure	Work thread procedure, thread pool object will hold a weak reference to the procedure
	// return: new object
	virtual rPtr<iAsyncProcedure>	cnLib_FUNC CreateWork(iReference *Reference,iProcedure *ThreadProcedure)=0;

	// CreateTimer
	//	Creates a timer execute in execution unit.
	// [in]ThreadProcedure	timer thread procedure, thread pool object will hold a weak reference to the procedure
	// return: new object
	virtual rPtr<iAsyncTimer>		cnLib_FUNC CreateTimer(iReference *Reference,iProcedure *ThreadProcedure)=0;

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
	virtual bool cnLib_FUNC IsCurrentThread(void)=0;

	// ExecuteSync
	//	execute procedure in dispatch thread and wait for its completion
	// Procedure		procedure to be called
	virtual void cnLib_FUNC ExecuteSync(iProcedure *Procedure)=0;

	// ExecuteAsync
	//	execute procedure in dispatch thread
	// Reference		Reference to the procedure
	// Procedure		procedure to be called
	// return: task for the procedure
	virtual iPtr<iAsyncTask> cnLib_FUNC ExecuteAsync(iReference *Reference,iProcedure *Procedure)=0;
};
//---------------------------------------------------------------------------
namespace cnSystem{
//---------------------------------------------------------------------------
extern const sInt64 SystemTimeEpochSecondsSinceUnixEpoch;
sInt64 cnLib_FUNC GetSystemTimeNow(void);

bool cnLib_FUNC UTCGregorianDateFromSystemTime(cDateTime &DateTime,sInt64 SystemTime);
bool cnLib_FUNC UTCGregorianDateToSystemTime(sInt64 &SystemTime,const cDateTime &DateTime);

//---------------------------------------------------------------------------

// sync

rPtr<iMutex> cnLib_FUNC CreateMutexLock(void);
rPtr<iMutex> cnLib_FUNC CreateRecursiveLock(void);
rPtr<iSharedMutex> cnLib_FUNC CreateSharedLock(void);

//---------------------------------------------------------------------------
namespace CurrentThread{
//---------------------------------------------------------------------------
// SwitchThread
//	yield execution to another thread that is ready to run
void cnLib_FUNC SwitchThread(void);

// SuspendFor
//	Suspends the execution of the current thread until the time passed
// [in]Delay	nanoseconds to suspend
void cnLib_FUNC SuspendFor(uInt64 Delay);

// SleepUntil
//	Suspends the execution of the current thread until the time arraied or by other threads
// [in]Time		woke time,	nullptr means never wake by time
// [in]Delay	nanoseconds adds to Time
// return true if it is woke by other threads, or false if it is the time
bool cnLib_FUNC SleepUntil(iTimepoint *Time,sInt64 Delay);

// GetThread
//	get thread interface for other thread use
// return thread object
iThread*	cnLib_FUNC GetThread(void);

// SetPriority
//	Sets the priority value for the thread
// [in]Priority		priority value
bool cnLib_FUNC SetPriority(sInt8 Priority);
// GetPriority
//	Retrieves the priority value for the thread
// return: priority value
sInt8 cnLib_FUNC GetPriority(void);
//---------------------------------------------------------------------------
}	// namespace CurrentThread
//---------------------------------------------------------------------------
// thread
rPtr<iThreadLocalVariable>	cnLib_FUNC CreateThreadLocalVariable(void);
iPtr<iThread>				cnLib_FUNC StartThread(iProcedure *ThreadProcedure);
iPtr<iDispatch>				cnLib_FUNC CreateDispatchThread(void);
//- Thread pool -------------------------------------------------------------
extern iThreadPool*const DefaultThreadPool;
//---------------------------------------------------------------------------
}   // namespace cnSystem
//---------------------------------------------------------------------------
class cnLib_INTERFACE iDataCache : public iInterface
{
public:
	struct tInterfaceID{	static iTypeID Value;	};
	virtual void* cnLib_FUNC CastInterface(iTypeID ID)noexcept(true) override{		return cnVar::ImplementCastInterface(this,ID);	}

	virtual bool cnLib_FUNC IsCached(void)=0;
	virtual void cnLib_FUNC Cache(void)=0;
	virtual iPtr<iAsyncTask> cnLib_FUNC CacheAsync(void)=0;
};
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
