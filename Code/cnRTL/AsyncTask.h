/*- cnRTL - Async Task ----------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2020-09-14                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnRTL_AsyncTask_H__
#define __cnLibrary_cnRTL_AsyncTask_H__
/*-------------------------------------------------------------------------*/
#include <cnRTL/cnRTLHeader.h>
#include <cnRTL/RuntimeFunction.h>
#include <cnRTL/ThreadSynchronization.h>
#include <cnRTL/ThreadAsync.h>
#ifdef __cplusplus
//---------------------------------------------------------------------------
#if cnLibrary_CPPFEATURE_CONSTEXPR >= 200704L
#define	cnRTL_CONSTEXPR_FUNC	constexpr
#define	cnRTL_CONSTVAR	constexpr
// cnLibrary_CPPFEATURE_CONSTEXPR >= 200704L
#else
// cnLibrary_CPPFEATURE_CONSTEXPR < 200704L
#define	cnRTL_CONSTEXPR_FUNC
#define	cnRTL_CONSTVAR	const
#endif // cnLibrary_CPPFEATURE_CONSTEXPR < 200704L
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnRTL{
//---------------------------------------------------------------------------
//TAsyncTask
//{
//	bool IsDone(void)const;
//	bool SetNotify(iProcedure *Procedrue);
//};
//---------------------------------------------------------------------------
class cWaitForTaskProcedure : public cThreadOneTimeNotifier, public iProcedure
{
	virtual void cnLib_FUNC Execute(void)override;
};
//---------------------------------------------------------------------------
template<class TAsyncTaskPtr>
void WaitForTask(TAsyncTaskPtr&& Task)
{
	cWaitForTaskProcedure Notifier;
	if(Task->SetNotify(&Notifier)==false){
		// already done
		return;
	}
	Notifier.Wait();
}
//---------------------------------------------------------------------------
template<class TAsyncTaskPtr>
class cAsyncTaskAwaiter : private iProcedure
{
public:
	cAsyncTaskAwaiter(TAsyncTaskPtr &&Task)
		: fTask(static_cast<TAsyncTaskPtr&&>(Task)){}
	~cAsyncTaskAwaiter(){}

	bool await_ready(void)const noexcept(true){	return fTask->IsDone();		}
	template<class TCoHandle>
	bool await_suspend(TCoHandle&& CoHandle){
		fCoHandle=static_cast<TCoHandle&&>(CoHandle);
		return fTask->SetNotify(this);
	}
	void await_resume(void)noexcept(true){}
private:
	TAsyncTaskPtr fTask;
	vtCoHandle fCoHandle;
	virtual void cnLib_FUNC Execute(void)override{
		fCoHandle.resume();
	}
};
//---------------------------------------------------------------------------
template<class TAsyncTaskPtr>
cAsyncTaskAwaiter<TAsyncTaskPtr> TaskAwaiter(TAsyncTaskPtr&& Task)
{
	return cAsyncTaskAwaiter<TAsyncTaskPtr>(static_cast<TAsyncTaskPtr&&>(Task));
}
//---------------------------------------------------------------------------
template<class TAsyncTaskPtr>
class cAsyncTaskExecutionAwaiter
{
public:
	cAsyncTaskExecutionAwaiter(TAsyncTaskPtr &&Task,iPtr<iAsyncExecution> ResumeExecution)
		: fTask(static_cast<TAsyncTaskPtr&&>(Task))
		, fResumeExecution(cnVar::MoveCast(ResumeExecution)){}
	~cAsyncTaskExecutionAwaiter(){}

	bool await_ready(void)const noexcept(true){	return fTask->IsDone();		}
	template<class TCoHandle>
	bool await_suspend(TCoHandle&& CoHandle)noexcept(true){
		fCoHandle=static_cast<TCoHandle&&>(CoHandle);
		return fTask->SetNotify(&fTaskNotifyProcedure);
	}
	void await_resume(void)noexcept(true){}
private:
	TAsyncTaskPtr fTask;
	iPtr<iAsyncExecution> fResumeExecution;
	vtCoHandle fCoHandle;

	class cTaskNotifyProcedure : public iProcedure
	{
		virtual void cnLib_FUNC Execute(void)override{
			auto Host=cnMemory::GetObjectFromMemberPointer(this,&cAsyncTaskExecutionAwaiter::fTaskNotifyProcedure);
			if(Host->fResumeExecution!=nullptr){
				Host->fResumeExecution->Execute(nullptr,&Host->fDispatchResumeProcedure);
			}
			else{
				Host->fCoHandle.resume();
			}
		}
	}fTaskNotifyProcedure;

	class cDispatchResumeProcedure : public iProcedure
	{
		virtual void cnLib_FUNC Execute(void)override{
			auto Host=cnMemory::GetObjectFromMemberPointer(this,&cAsyncTaskExecutionAwaiter::fDispatchResumeProcedure);
			Host->fCoHandle.resume();
		}
	}fDispatchResumeProcedure;

};
//---------------------------------------------------------------------------
template<class TAsyncTaskPtr>
inline cAsyncTaskExecutionAwaiter<TAsyncTaskPtr> TaskAwaiter(TAsyncTaskPtr&& Task,iAsyncExecution *ResumeExecution)
{
	return cAsyncTaskExecutionAwaiter<TAsyncTaskPtr>(static_cast<TAsyncTaskPtr&&>(Task),ResumeExecution);
}
//---------------------------------------------------------------------------
template<class TAsyncTaskPtr>
inline cAsyncTaskExecutionAwaiter<TAsyncTaskPtr> TaskUIDispatchAwaiter(TAsyncTaskPtr&& Task,bool HighPriority=false)
{
	auto AwaitUIDispatch=cnSystem::CurrentUIThread::GetDispatch(HighPriority);
	return cAsyncTaskExecutionAwaiter<TAsyncTaskPtr>(static_cast<TAsyncTaskPtr&&>(Task),AwaitUIDispatch);
}
//---------------------------------------------------------------------------
class cAsyncTaskState
{
public:
	cAsyncTaskState();
	~cAsyncTaskState();

	void Reset(void);
	void SetDone(void);

	bool IsDone(void)const;
	bool SetNotify(iProcedure *NotifyProcedure);

protected:

	cAtomicVar<iProcedure*> fNotifyProcedure;
	cAtomicVar<bool> fNotified;
};
//---------------------------------------------------------------------------
class cAsyncTask : public iAsyncTask
{
public:
	cAsyncTask();
	~cAsyncTask();

	virtual bool cnLib_FUNC IsDone(void)override;
	virtual bool cnLib_FUNC SetNotify(iProcedure *NotifyProcedure)override;

protected:
	cAsyncTaskState fTaskState;
};
//---------------------------------------------------------------------------
class cManualAsyncTask : public cAsyncTask
{
public:
	cManualAsyncTask();
	~cManualAsyncTask();

	void Reset(void);
	void SetDone(void);
};
//---------------------------------------------------------------------------
class cDelayTask : public cAsyncTask, public iReference
{
public:
	cDelayTask();
	~cDelayTask();

	void SetTimer(uInt64 NS);
protected:
	void TimerHit(void);

	rPtr<iAsyncTimer> fTimer;
	class cTimerHitProcedure : public iProcedure
	{
	public:
		virtual void cnLib_FUNC Execute(void)override;
	}fTimerProc;

};
//---------------------------------------------------------------------------
iPtr<iAsyncTask> DelayTask(uInt64 NS);
//---------------------------------------------------------------------------
class cAsyncTaskCoroutine : public iPtr<iAsyncTask>
{
public:
	class impAsyncTask : public iAsyncTask
	{
	public:
		virtual bool cnLib_FUNC IsDone(void)override;
		virtual bool cnLib_FUNC SetNotify(iProcedure *NotifyProcedure)override;

		cAsyncTaskState State;
	};
	class promise_type : public cnAsync::cFreePromise
	{
	public:
		promise_type();
		~promise_type();

		iPtr<impAsyncTask> get_return_object();
		void return_void(void);
	private:
		iPtr<impAsyncTask> fTask;
	};

	cAsyncTaskCoroutine(iPtr<impAsyncTask> &&Task);
	cAsyncTaskCoroutine(const cAsyncTaskCoroutine&)=delete;
	cAsyncTaskCoroutine &operator=(const cAsyncTaskCoroutine&)=delete;
	cAsyncTaskCoroutine(cAsyncTaskCoroutine &&Src);
	cAsyncTaskCoroutine &operator=(cAsyncTaskCoroutine &&Src);
	~cAsyncTaskCoroutine();
};
//---------------------------------------------------------------------------
template<class TResult,class TResultInterface=TResult>
class cAsyncFunctionCoroutine : public iPtr< iAsyncFunction<TResultInterface> >
{
public:
	class impAsyncFunction : public iAsyncFunction<TResultInterface>
	{
	public:
		virtual bool cnLib_FUNC IsDone(void)override{	return State.IsDone();		}
		virtual bool cnLib_FUNC SetNotify(iProcedure *NotifyProcedure)override{	return State.SetNotify(NotifyProcedure);	}
		virtual void cnLib_FUNC Cancel(void)override{		}
		virtual TResultInterface cnLib_FUNC GetResult(void)override{	return Result;	}

		cAsyncTaskState State;
		TResult Result;
	};
	class promise_type : public cnAsync::cFreePromise
	{
	public:
		promise_type():fTask(iCreate<impAsyncFunction>()){}
		~promise_type(){}

		iPtr<impAsyncFunction> get_return_object(){	return fTask;	}
		template<class T>
		void return_value(T&& Value){
			fTask->Result=static_cast<T&&>(Value);
			fTask->State.SetDone();
		}
	private:
		iPtr<impAsyncFunction> fTask;
	};

	cAsyncFunctionCoroutine(iPtr<impAsyncFunction> &&Task):iPtr< iAsyncFunction<TResultInterface> >(cnVar::MoveCast(Task)){}
	cAsyncFunctionCoroutine(const cAsyncFunctionCoroutine&)=delete;
	cAsyncFunctionCoroutine &operator=(const cAsyncFunctionCoroutine&)=delete;
	cAsyncFunctionCoroutine(cAsyncFunctionCoroutine &&Src):iPtr< iAsyncFunction<TResultInterface> >(cnVar::MoveCast(Src)){}
	cAsyncFunctionCoroutine &operator=(cAsyncFunctionCoroutine &&Src){	*this=cnVar::MoveCast(Src);	return *this;	}
	~cAsyncFunctionCoroutine(){}
};
//---------------------------------------------------------------------------
class cTaskQueue
{
public:
	cTaskQueue();
	~cTaskQueue();

	class cTask : public iReference
	{
	public:
		cTask *Next;
		cTask();
		~cTask();

		bool IsCancelled(void)const;
	protected:
		cAsyncTaskState fTaskState;
	private:

		friend cTaskQueue;
		// CancelFlag
		//	cancel state flag
		cCancellationFlag fCancelFlag;
	};

	bool IsEmpty(void)const;
	void EnqueueTask(cTask *Task);
	rPtr<cTask> DequeueTask(void);
	void CancelTask(cTask *Task);
	void CompleteTask(rPtr<cTask> Task);

	void CreateTaskCompletionWork(iReference *ThreadReference,iThreadPool *ThreadPool);

private:
	// pending queue
	cAtomicQueueSO<cTask> fTaskQueue;

	
	// Completion queue

	cAtomicQueueSO<cTask> fTaskCompleteQueue;
	void UpdateCompleteTaskQueue(void);
	class cCompleteTaskProcedure : public bcAsyncExclusiveProcedure
	{
		virtual bool Procedure(void)override;
	}fCompleteTaskProcedure;
	rPtr<iAsyncProcedure> fCompleteTaskProcWork;
	bool CompleteTaskProcedure(void);

	void (cTaskQueue::*fCompleteTaskProc)(void);
	void CompleteTaskProc_Run(void);
	void CompleteTaskProc_Async(void);
};
//---------------------------------------------------------------------------
class bcNotificationCycle
{
public:
	bcNotificationCycle();
	~bcNotificationCycle();

	bool PrepareStartNotify(void);
	void CancelStartNotify(void);
	void ConfirmStartNotify(void);
	bool StopNotify(void);
	bool CloseNotify(void);

	bool IsNotificationClosed(void)const;
	bool IsNotificationStarted(void)const;
	bool IsNotificationEnded(void)const;

	bool RunNotifyCycle(void);

	enum class CycleState{
		Normal,Ended,Terminated
	};
protected:
	virtual iReference* NotificationInnerReference(void)=0;
	virtual void NotificationStarted(void)=0;
	virtual void NotificationStopped(void)=0;
	virtual void NotificationClosed(void)=0;
	virtual CycleState NotificationCheckState(void)=0;
	virtual void NotificationProcedure(void)=0;
private:

	cAtomicVar<ufInt8> fNotificationCloseState;
	cAtomicVar<ufInt8> fNotificationAction;	// 0==wait start, 1=start, 2=wait stop, 3=stop, 4=disabled
	bool fNotificationStarted		:1;
	bool fNotificationIsEnded		:1;

	bool RunNotifyCycle_Running(void);
	bool RunNotifyCycle_Idle(void);
};
//---------------------------------------------------------------------------
#if 0
class bcAsyncSignal
{
public:
	bcAsyncSignal();
	~bcAsyncSignal();

	void CloseSignal(void);

	class cSignalStartProc
	{
	public:
		cSignalStartProc(bcAsyncSignal *Owner);
#ifdef cnLib_DEBUG
		~cSignalStartProc(){
			cnLib_ASSERT(fOwner==nullptr);	}
#endif // cnLib_DEBUG
		cSignalStartProc(cSignalStartProc &&Src);

		operator bool();

		void Cancel(void);
		void Execute(void);
	private:
		bcAsyncSignal *fOwner;
	};
	cSignalStartProc StartSignal(void);
	void StopSignal(void);
	void NotifySignal(bool IdleNotify);

	bool SignalIsClosed(void)const;
	bool SignalIsStarted(void)const;
	bool SignalIsEnded(void)const;

	void CreateNotificationWork(iAsyncExecution *AsyncExecution,bool DisableUpdateThreadLoop);
	void UpdateSignalState(void);
	void UpdateSignalStateAsync(void);

	enum class Availability{
		Available,Empty,Ended,Terminated
	};
protected:
	virtual iReference* AsyncSignalInnerReference(void)=0;
	virtual void AsyncSignalStarted(void)=0;
	virtual void AsyncSignalStopped(void)=0;
	virtual void AsyncSignalClosed(void)=0;
	virtual Availability AsyncSignalAvailable(void)=0;
	virtual void AsyncSignalNotify(void)=0;
private:

	void StartSignal_Cancel(void);
	void StartSignal_Execute(void);

	cAtomicVar<ufInt8> fAsyncSignalCloseState;
	cAtomicVar<ufInt8> fAsyncSignalAction;	// 0==wait start, 1=start, 2=wait stop, 3=stop, 4=disabled
	ufInt8 fAsyncSignalNotifyType;	// 0==no, 1=when available, 2=always
	bool fAsyncSignalStarted		:1;
	bool fAsyncSignalIsEnded		:1;

	class cAsyncSignalProcedure : public bcAsyncExclusiveProcedure
	{
		virtual bool Procedure(void)override;
	}fAsyncSignalProcedure;
	rPtr<iAsyncProcedure> fAsyncSignalProcWork;
	bool AsyncSignalProcedure(void);
	bool AsyncSignalProcedure_Running(void);
	bool AsyncSignalProcedure_Idle(void);

	bool MarkClose(void);
	void NotifyClose(void);

	void (bcAsyncSignal::*fAsyncSignalUpdateStateProc)(bool DisableUpdateThreadLoop);
	void AsyncSignalUpdateStateProc_Run(bool DisableUpdateThreadLoop);
	void AsyncSignalUpdateStateProc_Async(bool DisableUpdateThreadLoop);
	void AsyncSignalUpdateStateProc_AsyncAfter(bool DisableUpdateThreadLoop);
};
#endif // 0
//---------------------------------------------------------------------------
class bcAsyncNotificationProcessor :  protected iAsyncNotificationCallback
{
public:
	bcAsyncNotificationProcessor(iAsyncNotification *Notification=nullptr);
	~bcAsyncNotificationProcessor();

	iAsyncNotification* GetAsyncNotification(void)const;
	bool SetAsyncNotification(iAsyncNotification *Queue);

	bool IsActive(void)const;

	bool Start(iReference *Reference=nullptr);
	void Stop(void);
protected:

	rPtr<iAsyncNotification> fAsyncNotification;
	bool fNotificationActive;
	cAtomicVar<bool> fActiveMutex;
	virtual void cnLib_FUNC AsyncStarted(void)override;
	virtual void cnLib_FUNC AsyncStopped(void)override;
};
//---------------------------------------------------------------------------
class bcAsyncQueue : public bcNotificationCycle
{
public:
	bcAsyncQueue();
	~bcAsyncQueue();

	void NotifyQueue(bool IdleNotify);
	void CloseQueue(void);

protected:

	void CreateQueueWork(iAsyncExecution *AsyncExecution,bool DisableUpdateThreadLoop);
	void UpdateQueueState(bool AsyncNotify);

	virtual void NotificationStarted(void)override;
	virtual void NotificationStopped(void)override;
	virtual void NotificationClosed(void)override;
	virtual CycleState NotificationCheckState(void)override;
	virtual void NotificationProcedure(void)override;

	virtual void AsyncQueueNotify(void)=0;

	void AsyncQueueSetAvailable(bool AsyncNotify);

private:

	ufInt8 fAsyncQueueNotifyType;	// 0==no, 1=when available, 2=always
	bool fAsyncQueueAvailable;

	class cAsyncQueueStateProcedure : public bcAsyncExclusiveProcedure
	{
		virtual bool Procedure(void)override;
	}fAsyncQueueStateProcedure;
	rPtr<iAsyncProcedure> fAsyncQueueProcWork;

	void (bcAsyncQueue::*fAsyncQueueUpdateStateProc)(bool AsyncNotify);
	void AsyncQueueUpdateStateProc_Run(bool AsyncNotify);
	void AsyncQueueUpdateStateProc_Async(bool AsyncNotify);
	void AsyncQueueUpdateStateProc_DisableUpdateThreadLoop(bool AsyncNotify);
};
//---------------------------------------------------------------------------
class bcAsyncNotification : public iAsyncNotification, protected bcAsyncQueue
{
public:
	bcAsyncNotification();
	~bcAsyncNotification();

	virtual bool cnLib_FUNC StartNotify(iReference *Reference,iAsyncNotificationCallback *Callback)override;
	virtual void cnLib_FUNC StopNotify(void)override;
	virtual void cnLib_FUNC NotifyCallback(bool IdleNotify)override;
	virtual bool cnLib_FUNC IsClosed(void)override;

protected:
	iAsyncNotificationCallback *fAsyncNotificationCallback;
	rPtr<iReference> fAsyncNotificationReference;

	virtual void NotificationStarted(void)override;
	virtual void NotificationStopped(void)override;
	virtual void AsyncQueueNotify(void)override;
};
//---------------------------------------------------------------------------
}	// namespace cnRTL
//---------------------------------------------------------------------------
}   // namespace cnLibrary
//---------------------------------------------------------------------------
#undef	cnRTL_CONSTEXPR_FUNC
#undef	cnRTL_CONSTVAR
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif
