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
template<class TAsyncTaskPtr>
class cAsyncTaskAwaiter : private iProcedure
{
public:
	cAsyncTaskAwaiter(TAsyncTaskPtr&& Task)noexcept(true):fTask(cnVar::MoveCast(Task)){}
	cAsyncTaskAwaiter(cAsyncTaskAwaiter &&Src)noexcept(true):fTask(cnVar::MoveCast(Src.fTask)){}

	bool await_ready(void)const noexcept(true){
		if(fTask==nullptr)
			return true;
		return fTask->IsDone();
	}
	template<class TAwaitCoHandle>
	bool await_suspend(TAwaitCoHandle && CoHandle){
		cCoroutineHandleOperator::Assign(fCoHandle,cnVar::MoveCast(CoHandle));
		return fTask->Await(this);
	}
	decltype(cnVar::DeclVal<TAsyncTaskPtr>()->Result()) await_resume(void)noexcept(true){
		return fTask->Result();
	}


protected:
	virtual void cnLib_FUNC Execute(void)noexcept(true)override{
		cCoroutineHandleOperator::Resume(fCoHandle);
	}

	cCoroutineHandleOperator::tHandle fCoHandle;
	TAsyncTaskPtr fTask;
};
//---------------------------------------------------------------------------
class cWaitForTaskProcedure : public cLocalSingleThreadNotification, public iProcedure
{
	virtual void cnLib_FUNC Execute(void)noexcept(true)override;
};
//---------------------------------------------------------------------------
template<class TAsyncTask>
inline decltype(cnVar::DeclVal<TAsyncTask>().Result()) WaitForTask(TAsyncTask&& Task)noexcept(true)
{
	if(Task.IsDone())
		return;
	cWaitForTaskProcedure Notifier;
	if(Task.Await(&Notifier)){
		Notifier.Wait();
	}
	return Task.Result();
}
//---------------------------------------------------------------------------
template<class TAsyncTaskPtr>
inline cAsyncTaskAwaiter<TAsyncTaskPtr> AsyncTaskAwaiter(TAsyncTaskPtr&& Task)noexcept(true)
{
	return cAsyncTaskAwaiter<TAsyncTaskPtr>(static_cast<TAsyncTaskPtr&&>(Task));
}
//---------------------------------------------------------------------------
template<class TAsyncTaskPtr>
class cAsyncTaskExecutionAwaiter : public cAsyncTaskAwaiter<TAsyncTaskPtr>
{
public:
	cAsyncTaskExecutionAwaiter(TAsyncTaskPtr &&Task,iPtr<iAsyncExecution> ResumeExecution)noexcept(true)
		: cAsyncTaskAwaiter<TAsyncTaskPtr>(static_cast<TAsyncTaskPtr&&>(Task))
		, fResumeExecution(cnVar::MoveCast(ResumeExecution)){}
	~cAsyncTaskExecutionAwaiter(){}

private:
	iPtr<iAsyncExecution> fResumeExecution;

	virtual void cnLib_FUNC Execute(void)noexcept(true)override{
		if(fResumeExecution!=nullptr){
			fResumeExecution->Execute(nullptr,&fDispatchResumeProcedure);
		}
		else{
			cAsyncTaskAwaiter<TAsyncTaskPtr>::Execute();
		}
	}

	class cDispatchResumeProcedure : public iProcedure
	{
		virtual void cnLib_FUNC Execute(void)noexcept(true)override{
			auto Host=cnMemory::GetObjectFromMemberPointer(this,&cAsyncTaskExecutionAwaiter::fDispatchResumeProcedure);
			Host->cAsyncTaskAwaiter<TAsyncTaskPtr>::Execute();
		}
	}fDispatchResumeProcedure;

};
//---------------------------------------------------------------------------
template<class TAsyncTaskPtr>
inline cAsyncTaskExecutionAwaiter<TAsyncTaskPtr> TaskAwaiter(TAsyncTaskPtr&& Task,iAsyncExecution *ResumeExecution)noexcept(true)
{
	return cAsyncTaskExecutionAwaiter<TAsyncTaskPtr>(static_cast<TAsyncTaskPtr&&>(Task),ResumeExecution);
}
//---------------------------------------------------------------------------
template<class TAsyncTaskPtr>
inline cAsyncTaskExecutionAwaiter<TAsyncTaskPtr> TaskUIDispatchAwaiter(TAsyncTaskPtr&& Task,bool HighPriority=false)noexcept(true)
{
	auto AwaitUIDispatch=cnSystem::CurrentUIThread::GetDispatch(HighPriority);
	return cAsyncTaskExecutionAwaiter<TAsyncTaskPtr>(static_cast<TAsyncTaskPtr&&>(Task),AwaitUIDispatch);
}
//---------------------------------------------------------------------------
class cAsyncTaskState
{
public:
	cAsyncTaskState()noexcept(true);
	~cAsyncTaskState()noexcept(true);

	void Reset(void)noexcept(true);
	void SetDone(void)noexcept(true);

	bool IsDone(void)const noexcept(true);
	bool Await(iProcedure *NotifyProcedure)noexcept(true);
	void Result(void)noexcept(true){}

protected:

	cAtomicVar<iProcedure*> fNotifyProcedure;
	cAtomicVar<bool> fNotified;
};
//---------------------------------------------------------------------------
class cAsyncTask : public iAsyncTask
{
public:
	cAsyncTask()noexcept(true);
	~cAsyncTask()noexcept(true);

	virtual bool cnLib_FUNC IsDone(void)noexcept(true)override;
	virtual bool cnLib_FUNC Await(iProcedure *NotifyProcedure)noexcept(true)override;

protected:
	cAsyncTaskState fTaskState;
};
//---------------------------------------------------------------------------
class cManualAsyncTask : public cAsyncTask
{
public:
	cManualAsyncTask()noexcept(true);
	~cManualAsyncTask()noexcept(true);

	void Reset(void)noexcept(true);
	void SetDone(void)noexcept(true);
};
//---------------------------------------------------------------------------
class cDelayTask : public cAsyncTask, public iReference
{
public:
	cDelayTask()noexcept(true);
	~cDelayTask()noexcept(true);

	void SetTimer(uInt64 NS)noexcept(true);
protected:
	void TimerHit(void)noexcept(true);

	rPtr<iAsyncTimer> fTimer;
	class cTimerHitProcedure : public iProcedure
	{
	public:
		virtual void cnLib_FUNC Execute(void)noexcept(true)override;
	}fTimerProc;

};
//---------------------------------------------------------------------------
iPtr<iAsyncTask> DelayTask(uInt64 NS)noexcept(true);
//---------------------------------------------------------------------------
#if 0

class cAsyncTaskCoroutine : public iPtr<iAsyncTask>
{
public:
	class impAsyncTask : public iAsyncTask, public cnAsync::iCoroutinePromiseAwaiter
	{
	public:
		virtual bool cnLib_FUNC IsDone(void)noexcept(true)override;
		virtual bool cnLib_FUNC SetNotify(iProcedure *NotifyProcedure)noexcept(true)override;

		cAsyncTaskState State;

		virtual void NotifyCompletion(void)noexcept(true)override;

		cCoroutine<void>::pPtr Promise;

		impAsyncTask(cCoroutine<void>::pPtr &&Promise)noexcept(true);
	};

	cAsyncTaskCoroutine(const cAsyncTaskCoroutine&)=delete;
	cAsyncTaskCoroutine &operator=(const cAsyncTaskCoroutine&)=delete;
	cAsyncTaskCoroutine(cAsyncTaskCoroutine &&Src)noexcept(true);
	cAsyncTaskCoroutine &operator=(cAsyncTaskCoroutine &&Src)noexcept(true);
	~cAsyncTaskCoroutine()noexcept(true);


	// construct by promise
	cAsyncTaskCoroutine(cCoroutine<void> &&Src)noexcept(true);


};
//---------------------------------------------------------------------------
template<class TResult,class TResultInterface=TResult>
class cAsyncFunctionCoroutine : public iPtr< iAsyncFunction<TResultInterface> >
{
public:
	class impAsyncFunction : public iAsyncFunction<TResultInterface>, public cnAsync::iCoroutinePromiseAwaiter
	{
	public:
		virtual bool cnLib_FUNC IsDone(void)noexcept(true)override{	return State.IsDone();		}
		virtual bool cnLib_FUNC SetNotify(iProcedure *NotifyProcedure)noexcept(true)override{	return State.SetNotify(NotifyProcedure);	}
		virtual void cnLib_FUNC Cancel(void)noexcept(true)override{		}
		virtual TResultInterface cnLib_FUNC GetResult(void)noexcept(true)override{	return Promise->fReturnValue;	}

		cAsyncTaskState State;

		virtual void NotifyCompletion(void)noexcept(true)override;

		typename cCoroutine<TResult>::pPtr Promise;

		impAsyncFunction(cCoroutine<void>::pPtr &&Promise)noexcept(true);
	};

	cAsyncFunctionCoroutine(cCoroutine<void> &&Src)noexcept(true):iPtr< iAsyncFunction<TResultInterface> >(iCreate<impAsyncFunction>(static_cast<typename cCoroutine<TResult>::pPtr&&>(Src))){}

	cAsyncFunctionCoroutine(const cAsyncFunctionCoroutine&)=delete;
	cAsyncFunctionCoroutine &operator=(const cAsyncFunctionCoroutine&)=delete;
	cAsyncFunctionCoroutine(cAsyncFunctionCoroutine &&Src)noexcept(true):iPtr< iAsyncFunction<TResultInterface> >(cnVar::MoveCast(Src)){}
	cAsyncFunctionCoroutine &operator=(cAsyncFunctionCoroutine &&Src)noexcept(true){	*this=cnVar::MoveCast(Src);	return *this;	}
	~cAsyncFunctionCoroutine()noexcept(true){}
};
#endif // 0
//---------------------------------------------------------------------------
class cTaskQueue
{
public:
	cTaskQueue()noexcept(true);
	~cTaskQueue()noexcept(true);

	class cTask : public iReference
	{
	public:
		cTask *Next;
		cTask()noexcept(true);
		~cTask()noexcept(true);

		bool IsCancelled(void)const noexcept(true);
	protected:
		cAsyncTaskState fTaskState;
	private:

		friend cTaskQueue;
		// CancelFlag
		//	cancel state flag
		cCancellationFlag fCancelFlag;
	};

	bool IsEmpty(void)const noexcept(true);
	void EnqueueTask(cTask *Task)noexcept(true);
	rPtr<cTask> DequeueTask(void)noexcept(true);
	void CancelTask(cTask *Task)noexcept(true);
	void CompleteTask(rPtr<cTask> Task)noexcept(true);

	void CreateTaskCompletionWork(iReference *ThreadReference,iThreadPool *ThreadPool)noexcept(true);

private:
	// pending queue
	cAtomicQueueSO<cTask> fTaskQueue;

	
	// Completion queue

	cAtomicQueueSO<cTask> fTaskCompleteQueue;
	void UpdateCompleteTaskQueue(void)noexcept(true);
	class cCompleteTaskProcedure : public bcAsyncExclusiveProcedure
	{
		virtual bool Procedure(void)noexcept(true)override;
	}fCompleteTaskProcedure;
	rPtr<iAsyncProcedure> fCompleteTaskProcWork;
	bool CompleteTaskProcedure(void)noexcept(true);

	void (cTaskQueue::*fCompleteTaskProc)(void)noexcept(true);
	void CompleteTaskProc_Run(void)noexcept(true);
	void CompleteTaskProc_Async(void)noexcept(true);
};
//---------------------------------------------------------------------------
class bcNotificationCycle
{
public:
	bcNotificationCycle()noexcept(true);
	~bcNotificationCycle()noexcept(true);

	bool PrepareStartNotify(void)noexcept(true);
	void CancelStartNotify(void)noexcept(true);
	void ConfirmStartNotify(void)noexcept(true);
	bool StopNotify(void)noexcept(true);
	bool CloseNotify(void)noexcept(true);

	bool IsNotificationClosed(void)const noexcept(true);
	bool IsNotificationStarted(void)const noexcept(true);
	bool IsNotificationEnded(void)const noexcept(true);

	bool RunNotifyCycle(void)noexcept(true);

	enum class CycleState{
		Normal,Ended,Terminated
	};
protected:
	virtual iReference* NotificationInnerReference(void)noexcept(true)=0;
	virtual void NotificationStarted(void)noexcept(true)=0;
	virtual void NotificationStopped(void)noexcept(true)=0;
	virtual void NotificationClosed(void)noexcept(true)=0;
	virtual CycleState NotificationCheckState(void)noexcept(true)=0;
	virtual void NotificationProcedure(void)noexcept(true)=0;
private:

	cAtomicVar<ufInt8> fNotificationCloseState;
	cAtomicVar<ufInt8> fNotificationAction;	// 0==wait start, 1=start, 2=wait stop, 3=stop, 4=disabled
	bool fNotificationStarted		:1;
	bool fNotificationIsEnded		:1;

	bool RunNotifyCycle_Running(void)noexcept(true);
	bool RunNotifyCycle_Idle(void)noexcept(true);
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
	bcAsyncNotificationProcessor(iAsyncNotification *Notification=nullptr)noexcept(true);
	~bcAsyncNotificationProcessor()noexcept(true);

	iAsyncNotification* GetAsyncNotification(void)const noexcept(true);
	bool SetAsyncNotification(iAsyncNotification *Queue)noexcept(true);

	bool IsActive(void)const noexcept(true);

	bool Start(iReference *Reference=nullptr)noexcept(true);
	void Stop(void)noexcept(true);
protected:

	rPtr<iAsyncNotification> fAsyncNotification;
	bool fNotificationActive;
	cAtomicVar<bool> fActiveMutex;
	virtual void cnLib_FUNC AsyncStarted(void)noexcept(true)override;
	virtual void cnLib_FUNC AsyncStopped(void)noexcept(true)override;
};
//---------------------------------------------------------------------------
class bcAsyncQueue : public bcNotificationCycle
{
public:
	bcAsyncQueue()noexcept(true);
	~bcAsyncQueue()noexcept(true);

	void NotifyQueue(bool IdleNotify)noexcept(true);
	void CloseQueue(void)noexcept(true);

protected:

	void CreateQueueWork(iAsyncExecution *AsyncExecution,bool RestrictInExecution)noexcept(true);
	void UpdateQueueState(bool AsyncNotify)noexcept(true);

	virtual void NotificationStarted(void)noexcept(true)override;
	virtual void NotificationStopped(void)noexcept(true)override;
	virtual void NotificationClosed(void)noexcept(true)override;
	virtual CycleState NotificationCheckState(void)noexcept(true)override;
	virtual void NotificationProcedure(void)noexcept(true)override;

	virtual void AsyncQueueNotify(void)noexcept(true)=0;

	void AsyncQueueSetAvailable(bool AsyncNotify)noexcept(true);

private:

	ufInt8 fAsyncQueueNotifyType;	// 0==no, 1=when available, 2=always
	bool fAsyncQueueAvailable;

	class cAsyncQueueStateProcedure : public bcAsyncExclusiveProcedure
	{
		virtual bool Procedure(void)noexcept(true)override;
	}fAsyncQueueStateProcedure;
	rPtr<iAsyncProcedure> fAsyncQueueProcWork;

	void (bcAsyncQueue::*fAsyncQueueUpdateStateProc)(bool AsyncNotify)noexcept(true);
	void AsyncQueueUpdateStateProc_Run(bool AsyncNotify)noexcept(true);
	void AsyncQueueUpdateStateProc_Async(bool AsyncNotify)noexcept(true);
	void AsyncQueueUpdateStateProc_RestrictInAsyncExecution(bool AsyncNotify)noexcept(true);
};
//---------------------------------------------------------------------------
class bcAsyncNotification : public iAsyncNotification, protected bcAsyncQueue
{
public:
	bcAsyncNotification()noexcept(true);
	~bcAsyncNotification()noexcept(true);

	virtual bool cnLib_FUNC StartNotify(iReference *Reference,iAsyncNotificationCallback *Callback)noexcept(true)override;
	virtual void cnLib_FUNC StopNotify(void)noexcept(true)override;
	virtual void cnLib_FUNC NotifyCallback(bool IdleNotify)noexcept(true)override;
	virtual bool cnLib_FUNC IsClosed(void)noexcept(true)override;

protected:
	iAsyncNotificationCallback *fAsyncNotificationCallback;
	rPtr<iReference> fAsyncNotificationReference;

	virtual void NotificationStarted(void)noexcept(true)override;
	virtual void NotificationStopped(void)noexcept(true)override;
	virtual void AsyncQueueNotify(void)noexcept(true)override;
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
