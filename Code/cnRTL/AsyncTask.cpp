//---------------------------------------------------------------------------
#include "AsyncTask.h"

using namespace cnLibrary;
using namespace cnRTL;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void cWaitForTaskProcedure::Execute(void)noexcept(true)
{
	Notify();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cAsyncTaskState::cAsyncTaskState()noexcept(true)
{
	Reset();
}
//---------------------------------------------------------------------------
cAsyncTaskState::~cAsyncTaskState()noexcept(true)
{
}
//---------------------------------------------------------------------------
void cAsyncTaskState::Reset(void)noexcept(true)
{
	fNotified=false;
	fNotifyProcedure=nullptr;
}
//---------------------------------------------------------------------------
void cAsyncTaskState::SetDone(void)noexcept(true)
{
	if(fNotified.Acquire.Xchg(true)==false){
		auto NotifyProc=fNotifyProcedure.Acquire.Xchg(nullptr);
		if(NotifyProc!=nullptr){
			NotifyProc->Execute();
		}
	}
}
//---------------------------------------------------------------------------
bool cAsyncTaskState::IsDone(void)const noexcept(true)
{
	return fNotified;
}
//---------------------------------------------------------------------------
bool cAsyncTaskState::Await(iProcedure *NotifyProcedure)noexcept(true)
{
	if(NotifyProcedure==nullptr)
		return false;

	fNotifyProcedure.Release.Store(NotifyProcedure);

	if(fNotified.Acquire.Load()){
		if((fNotifyProcedure.Acquire.Xchg(nullptr))!=nullptr){
			// already done
			return false;
		}
		// done but notified procedure
	}
	return true;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cAsyncTask::cAsyncTask()noexcept(true)
{
}
//---------------------------------------------------------------------------
cAsyncTask::~cAsyncTask()noexcept(true)
{
}
//---------------------------------------------------------------------------
bool cAsyncTask::IsDone(void)noexcept(true)
{
	return fTaskState.IsDone();
}
//---------------------------------------------------------------------------
bool cAsyncTask::Await(iProcedure *NotifyProcedure)noexcept(true)
{
	return fTaskState.Await(NotifyProcedure);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cManualAsyncTask::cManualAsyncTask()noexcept(true)
{
}
//---------------------------------------------------------------------------
cManualAsyncTask::~cManualAsyncTask()noexcept(true)
{
}
void cManualAsyncTask::Reset(void)noexcept(true)
{
	return fTaskState.Reset();
}
void cManualAsyncTask::SetDone(void)noexcept(true)
{
	return fTaskState.SetDone();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cDelayTask::cDelayTask()noexcept(true)
{
	fTimer=cnSystem::DefaultThreadPool->CreateTimer(this,&fTimerProc);
}
//---------------------------------------------------------------------------
cDelayTask::~cDelayTask()noexcept(true)
{
}
//---------------------------------------------------------------------------
void cDelayTask::cTimerHitProcedure::Execute(void)noexcept(true)
{
	auto Host=cnMemory::GetObjectFromMemberPointer(this,&cDelayTask::fTimerProc);
	Host->TimerHit();
}
//---------------------------------------------------------------------------
void cDelayTask::SetTimer(uInt64 NS)noexcept(true)
{
	uInt64 TimeNow=cnSystem::GetSystemTimeNow();
	TimeNow+=NS;
	fTimer->Start(TimeNow,0);
}
//---------------------------------------------------------------------------
void cDelayTask::TimerHit(void)noexcept(true)
{
	fTaskState.SetDone();
	fTimer->Stop();
}
//---------------------------------------------------------------------------
iPtr<iAsyncTask> cnRTL::DelayTask(uInt64 NS)noexcept(true)
{
	auto Task=cnRTL::iCreate<cDelayTask>();
	Task->SetTimer(NS);
	return Task;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#if 0

cAsyncTaskCoroutine::impAsyncTask::impAsyncTask(cCoroutine<void>::pPtr &&Promise)noexcept(true)
	: Promise(cnVar::MoveCast(Promise))
{
	if(Promise->SetupAwait(this)==false){
		State.SetDone();
	}
}
//---------------------------------------------------------------------------
bool cAsyncTaskCoroutine::impAsyncTask::IsDone(void)noexcept(true)
{
	return State.IsDone();
}
//---------------------------------------------------------------------------
bool cAsyncTaskCoroutine::impAsyncTask::SetNotify(iProcedure *NotifyProcedure)noexcept(true)
{
	return State.SetNotify(NotifyProcedure);
}
//---------------------------------------------------------------------------
void cAsyncTaskCoroutine::impAsyncTask::NotifyCompletion(void)noexcept(true)
{
	State.SetDone();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cAsyncTaskCoroutine::cAsyncTaskCoroutine(cCoroutine<void> &&Src)noexcept(true)
	: iPtr<iAsyncTask>(iCreate<impAsyncTask>(Src.TakePromise()))
{
}
//---------------------------------------------------------------------------
cAsyncTaskCoroutine::cAsyncTaskCoroutine(cAsyncTaskCoroutine &&Src)noexcept(true)
	: iPtr<iAsyncTask>(cnVar::MoveCast(Src))
{
}
//---------------------------------------------------------------------------
cAsyncTaskCoroutine& cAsyncTaskCoroutine::operator=(cAsyncTaskCoroutine &&Src)noexcept(true)
{
	this->iPtr<iAsyncTask>::operator=(cnVar::MoveCast(Src));
	return *this;
}
//---------------------------------------------------------------------------
cAsyncTaskCoroutine::~cAsyncTaskCoroutine()noexcept(true)
{
}
#endif // 0
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cTaskQueue::cTask::cTask()noexcept(true)
{
	fCancelFlag.Reset();
}
//---------------------------------------------------------------------------
cTaskQueue::cTask::~cTask()noexcept(true)
{
}
//---------------------------------------------------------------------------
bool cTaskQueue::cTask::IsCancelled(void)const noexcept(true)
{
	return fCancelFlag.IsCancelled();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cTaskQueue::cTaskQueue()noexcept(true)
{
	fCompleteTaskProc=&cTaskQueue::CompleteTaskProc_Run;
}
//---------------------------------------------------------------------------
cTaskQueue::~cTaskQueue()noexcept(true)
{
}
//---------------------------------------------------------------------------
bool cTaskQueue::IsEmpty(void)const noexcept(true)
{
	return fTaskQueue.IsEmpty();
}
//---------------------------------------------------------------------------
void cTaskQueue::EnqueueTask(cTask *Task)noexcept(true)
{
	rIncReference(Task,'queu');
	fTaskQueue.Enqueue(Task);
}
//---------------------------------------------------------------------------
rPtr<cTaskQueue::cTask> cTaskQueue::DequeueTask(void)noexcept(true)
{
	for(auto pTask=fTaskQueue.Dequeue();pTask!=nullptr;pTask=fTaskQueue.Dequeue()){
		// mark the item as processing
		if(pTask->fCancelFlag.Complete()==false){
			// item cancelled
			pTask->fTaskState.SetDone();

			rDecReference(pTask,'queu');
			continue;
		}

		return rTake(pTask,'queu');
	}
	// queue empty
	return nullptr;
}
//---------------------------------------------------------------------------
void cTaskQueue::CreateTaskCompletionWork(iReference *ThreadReference,iThreadPool *ThreadPool)noexcept(true)
{
	if(fCompleteTaskProcWork!=nullptr)
		return;
	fCompleteTaskProcWork=ThreadPool->CreateWork(ThreadReference,fCompleteTaskProcedure);
	fCompleteTaskProc=&cTaskQueue::CompleteTaskProc_Async;
}
//---------------------------------------------------------------------------
void cTaskQueue::CompleteTaskProc_Run(void)noexcept(true)
{
	fCompleteTaskProcedure.Run();
}
//---------------------------------------------------------------------------
void cTaskQueue::CompleteTaskProc_Async(void)noexcept(true)
{
	fCompleteTaskProcedure.RunAsyncAfter(fCompleteTaskProcWork,1);
}
//---------------------------------------------------------------------------
void cTaskQueue::UpdateCompleteTaskQueue(void)noexcept(true)
{
	(this->*fCompleteTaskProc)();
}
//---------------------------------------------------------------------------
bool cTaskQueue::cCompleteTaskProcedure::Procedure(void)noexcept(true)
{
	auto Host=cnMemory::GetObjectFromMemberPointer(this,&cTaskQueue::fCompleteTaskProcedure);
	return Host->CompleteTaskProcedure();
}
//---------------------------------------------------------------------------
bool cTaskQueue::CompleteTaskProcedure(void)noexcept(true)
{
	auto CompletedTask=fTaskCompleteQueue.DequeueAll();
	while(CompletedTask!=nullptr){
		auto CurTask=rTake(CompletedTask,'comp');
		CurTask->fTaskState.SetDone();
		CompletedTask=CompletedTask->Next;
	}
	return false;
}
//---------------------------------------------------------------------------
void cTaskQueue::CompleteTask(rPtr<cTask> Task)noexcept(true)
{
	auto pTask=rExtract(Task,'comp');
	fTaskCompleteQueue.Enqueue(pTask);
	UpdateCompleteTaskQueue();
}
//---------------------------------------------------------------------------
void cTaskQueue::CancelTask(cTask *Task)noexcept(true)
{
	Task->fCancelFlag.Cancel();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcNotificationCycle::bcNotificationCycle()noexcept(true)
	: fNotificationCloseState(0)
	, fNotificationState(nsIdle)
	, fNotificationStarted(false)
	, fNotificationIsEnded(false)
{
}
//---------------------------------------------------------------------------
bcNotificationCycle::~bcNotificationCycle()noexcept(true)
{
}
//---------------------------------------------------------------------------
bool bcNotificationCycle::RunNotifyCycle(void)noexcept(true)
{
	if(fNotificationStarted){
		return RunNotifyCycle_Running();
	}
	else{
		return RunNotifyCycle_Idle();
	}
}
//---------------------------------------------------------------------------
bool bcNotificationCycle::RunNotifyCycle_Running(void)noexcept(true)
{
	if(fNotificationState.Acquire.CmpStore(nsStop,nsStopping)){
		iReference *Reference=NotificationInnerReference();
		rDecReference(Reference,'stop');

		// stop
		fNotificationStarted=false;
		NotificationStopped();

		fNotificationState.Release.Store(nsIdle);
		return true;	// continue with idle state
	}

	if(fNotificationCloseState){
		// notify closed
		NotificationProcedure();
		return false;
	}

	auto State=NotificationCheckState();

	switch(State){
	case CycleState::Ended:
		fNotificationIsEnded=true;
		cnLib_SWITCH_FALLTHROUGH;
	default:
	case CycleState::Terminated:
		CloseNotify();
		// notify closed
		cnLib_SWITCH_FALLTHROUGH;
	case CycleState::Normal:
		NotificationProcedure();
		break;
	}
	return false;
}
//---------------------------------------------------------------------------
bool bcNotificationCycle::RunNotifyCycle_Idle(void)noexcept(true)
{
	if(fNotificationState.Acquire.CmpStore(nsWaitStart,nsStarting)){
		iReference *Reference=NotificationInnerReference();
		rDecReference(Reference,'star');

		if(fNotificationCloseState==0){
			// start
			fNotificationStarted=true;
			NotificationStarted();

			fNotificationState.Release.Store(nsActive);

			return true;	// to running state
		}

	}
	if(fNotificationCloseState==0){
		// check
		cnLib_ASSERT(fNotificationIsEnded==false);
		auto State=NotificationCheckState();
		switch(State){
		default:
			break;
		case CycleState::Ended:
			fNotificationIsEnded=true;
			cnLib_SWITCH_FALLTHROUGH;
		case CycleState::Terminated:
			return CloseNotify();
		}
	}
	else{
		// closed
		if(fNotificationCloseState==1){
			fNotificationCloseState=2;
			NotificationClosed();

			auto Reference=NotificationInnerReference();
			if(Reference!=nullptr){
				rDecReference(Reference,'clos');
			}
		}
	}
	return false;
}
//---------------------------------------------------------------------------
bool bcNotificationCycle::IsNotificationClosed(void)const noexcept(true)
{
	return fNotificationCloseState!=0;
}
//---------------------------------------------------------------------------
bool bcNotificationCycle::IsNotificationStarted(void)const noexcept(true)
{
	return fNotificationStarted;
}
//---------------------------------------------------------------------------
bool bcNotificationCycle::IsNotificationEnded(void)const noexcept(true)
{
	return fNotificationIsEnded;
}
//---------------------------------------------------------------------------
bool bcNotificationCycle::PrepareStartNotify(void)noexcept(true)
{
	return fNotificationState.Free.CmpStore(nsIdle,nsSetup);
}
//---------------------------------------------------------------------------
void bcNotificationCycle::CancelStartNotify(void)noexcept(true)
{
	fNotificationState.Free.CmpStore(nsSetup,nsIdle);
}
//---------------------------------------------------------------------------
void bcNotificationCycle::ConfirmStartNotify(void)noexcept(true)
{
	if(fNotificationState.Free.CmpStore(nsSetup,nsWaitStart)){
		iReference *Reference=NotificationInnerReference();
		rIncReference(Reference,'star');
	}
}
//---------------------------------------------------------------------------
bool bcNotificationCycle::StopNotify(void)noexcept(true)
{
	if(fNotificationState.Free.CmpStore(nsActive,nsStop)){
		iReference *Reference=NotificationInnerReference();
		rIncReference(Reference,'stop');
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------
bool bcNotificationCycle::CloseNotify(void)noexcept(true)
{
	if(fNotificationCloseState.Free.CmpStore(0,1)==false){
		return false;
	}
	auto Reference=NotificationInnerReference();
	if(Reference!=nullptr){
		rIncReference(Reference,'clos');
	}
	return true;
}
//---------------------------------------------------------------------------
#if	0
//---------------------------------------------------------------------------
bcAsyncSignal::bcAsyncSignal()
	: fAsyncSignalCloseState(0)
	, fAsyncSignalAction(0)
	, fAsyncSignalNotifyType(0)
	, fAsyncSignalStarted(false)
	, fAsyncSignalIsEnded(false)
{
	fAsyncSignalUpdateStateProc=&bcAsyncSignal::AsyncSignalUpdateStateProc_Run;
}
//---------------------------------------------------------------------------
bcAsyncSignal::~bcAsyncSignal()
{
}
//---------------------------------------------------------------------------
void bcAsyncSignal::CreateNotificationWork(iAsyncExecution *AsyncExecution,bool DisableUpdateThreadLoop)
{
	if(fAsyncSignalProcWork!=nullptr)
		return;

	fAsyncSignalProcWork=AsyncExecution->CreateWork(AsyncSignalInnerReference(),fAsyncSignalProcedure);
	if(DisableUpdateThreadLoop){
		fAsyncSignalUpdateStateProc=&bcAsyncSignal::AsyncSignalUpdateStateProc_Async;
	}
	else{
		fAsyncSignalUpdateStateProc=&bcAsyncSignal::AsyncSignalUpdateStateProc_AsyncAfter;
	}
}
//---------------------------------------------------------------------------
void bcAsyncSignal::AsyncSignalUpdateStateProc_Run(bool)
{
	fAsyncSignalProcedure.Run();
}
//---------------------------------------------------------------------------
void bcAsyncSignal::AsyncSignalUpdateStateProc_Async(bool)
{
	fAsyncSignalProcedure.RunAsync(fAsyncSignalProcWork);
}
//---------------------------------------------------------------------------
void bcAsyncSignal::AsyncSignalUpdateStateProc_AsyncAfter(bool DisableUpdateThreadLoop)
{
	if(DisableUpdateThreadLoop){
		fAsyncSignalProcedure.RunAsync(fAsyncSignalProcWork);
	}
	else{
		fAsyncSignalProcedure.RunAsyncAfter(fAsyncSignalProcWork,1);
	}
}
//---------------------------------------------------------------------------
void bcAsyncSignal::UpdateSignalState(void)
{
	(this->*fAsyncSignalUpdateStateProc)(false);
}
//---------------------------------------------------------------------------
void bcAsyncSignal::UpdateSignalStateAsync(void)
{
	(this->*fAsyncSignalUpdateStateProc)(true);
}
//---------------------------------------------------------------------------
bool bcAsyncSignal::cAsyncSignalProcedure::Procedure(void)
{
	auto Host=cnMemory::GetObjectFromMemberPointer(this,&bcAsyncSignal::fAsyncSignalProcedure);
	return Host->AsyncSignalProcedure();
}
//---------------------------------------------------------------------------
bool bcAsyncSignal::AsyncSignalProcedure(void)
{
	if(fAsyncSignalStarted){
		return AsyncSignalProcedure_Running();
	}
	else{
		return AsyncSignalProcedure_Idle();
	}
}
//---------------------------------------------------------------------------
bool bcAsyncSignal::AsyncSignalProcedure_Running(void)
{
	if(fAsyncSignalAction.Acquire.CmpStore(3,4)){
		iReference *Reference=AsyncSignalInnerReference();
		rDecReference(Reference,'stop');

		// stop
		fAsyncSignalStarted=false;
		AsyncSignalStopped();

		fAsyncSignalAction.Release.Store(0);
		return true;	// continue with idle state
	}

	if(fAsyncSignalCloseState){
		// notify closed
		if(fAsyncSignalNotifyType){
			fAsyncSignalNotifyType=0;
			AsyncSignalNotify();
		}
		return false;
	}

	auto SignalAvailable=AsyncSignalAvailable();

	switch(SignalAvailable){
	case Availability::Empty:
		if(fAsyncSignalNotifyType>1){
			fAsyncSignalNotifyType=0;
			AsyncSignalNotify();
		}
		break;
	case Availability::Ended:
		fAsyncSignalIsEnded=true;
		cnLib_SWITCH_FALLTHROUGH;
	case Availability::Terminated:
		MarkClose();
		// notify closed
		cnLib_SWITCH_FALLTHROUGH;
	case Availability::Available:
		if(fAsyncSignalNotifyType){
			fAsyncSignalNotifyType=0;
			AsyncSignalNotify();
		}
		break;
	}
	return false;
}
//---------------------------------------------------------------------------
bool bcAsyncSignal::AsyncSignalProcedure_Idle(void)
{
	if(fAsyncSignalAction.Acquire.CmpStore(1,4)){
		iReference *Reference=AsyncSignalInnerReference();
		rDecReference(Reference,'star');

		if(fAsyncSignalCloseState==0){
			// start
			fAsyncSignalStarted=true;
			fAsyncSignalNotifyType=1;	// initially notify when available
			AsyncSignalStarted();

			fAsyncSignalAction.Release.Store(2);

			return true;	// to running state
		}

	}
	if(fAsyncSignalCloseState==0){
		// check
		cnLib_ASSERT(fAsyncSignalIsEnded==false);
		auto SignalAvailable=AsyncSignalAvailable();
		switch(SignalAvailable){
		default:
			break;
		case Availability::Ended:
			fAsyncSignalIsEnded=true;
			cnLib_SWITCH_FALLTHROUGH;
		case Availability::Terminated:
			return MarkClose();
		}
	}
	else{
		// closed
		NotifyClose();
	}
	return false;
}
//---------------------------------------------------------------------------
bool bcAsyncSignal::SignalIsClosed(void)const
{
	return fAsyncSignalCloseState!=0;
}
//---------------------------------------------------------------------------
bool bcAsyncSignal::SignalIsStarted(void)const
{
	return fAsyncSignalStarted;
}
//---------------------------------------------------------------------------
bool bcAsyncSignal::SignalIsEnded(void)const
{
	return fAsyncSignalIsEnded;
}
//---------------------------------------------------------------------------
bcAsyncSignal::cSignalStartProc::cSignalStartProc(bcAsyncSignal *Owner)
	: fOwner(Owner)
{
}
//---------------------------------------------------------------------------
bcAsyncSignal::cSignalStartProc::cSignalStartProc(cSignalStartProc &&Src)
	: fOwner(Src.fOwner)
{
	Src.fOwner=nullptr;
}
//---------------------------------------------------------------------------
bcAsyncSignal::cSignalStartProc::operator bool()
{
	return fOwner!=nullptr;
}
//---------------------------------------------------------------------------
void bcAsyncSignal::cSignalStartProc::Cancel(void)
{
	fOwner->StartSignal_Cancel();
	fOwner=nullptr;
}
//---------------------------------------------------------------------------
void bcAsyncSignal::cSignalStartProc::Execute(void)
{
	fOwner->StartSignal_Execute();
	fOwner=nullptr;
}
//---------------------------------------------------------------------------
bcAsyncSignal::cSignalStartProc bcAsyncSignal::StartSignal(void)
{
	if(fAsyncSignalAction.Free.CmpStore(0,1)){
		return this;
	}
	else{
		return nullptr;
	}
}
//---------------------------------------------------------------------------
void bcAsyncSignal::StartSignal_Cancel(void)
{
	cnLib_ASSERT(fAsyncSignalAction==1);
	fAsyncSignalAction.Release.Store(0);
}
//---------------------------------------------------------------------------
void bcAsyncSignal::StartSignal_Execute(void)
{
	cnLib_ASSERT(fAsyncSignalAction==1);

	iReference *Reference=AsyncSignalInnerReference();
	rIncReference(Reference,'star');

	UpdateSignalState();
}
//---------------------------------------------------------------------------
void bcAsyncSignal::StopSignal(void)
{
	if(fAsyncSignalAction.Free.CmpStore(2,3)){
		iReference *Reference=AsyncSignalInnerReference();
		rIncReference(Reference,'stop');
	
		UpdateSignalState();
	}
}
//---------------------------------------------------------------------------
void bcAsyncSignal::NotifySignal(bool IdleNotify)
{
	if(IdleNotify){
		fAsyncSignalNotifyType=2;
		UpdateSignalStateAsync();
	}
	else{
		if(fAsyncSignalNotifyType==0){
			fAsyncSignalNotifyType=1;
		}
		UpdateSignalState();
	}
}
//---------------------------------------------------------------------------
void bcAsyncSignal::CloseSignal(void)
{
	if(MarkClose()){
		UpdateSignalState();
	}
}
//---------------------------------------------------------------------------
bool bcAsyncSignal::MarkClose(void)
{
	if(fAsyncSignalCloseState.Free.CmpStore(0,1)==false){
		return false;
	}
	auto Reference=AsyncSignalInnerReference();
	if(Reference!=nullptr){
		rIncReference(Reference,'clos');
	}
	return true;
}
//---------------------------------------------------------------------------
void bcAsyncSignal::NotifyClose(void)
{
	if(fAsyncSignalCloseState==1){
		fAsyncSignalCloseState=2;
		AsyncSignalClosed();

		auto Reference=AsyncSignalInnerReference();
		if(Reference!=nullptr){
			rDecReference(Reference,'clos');
		}
	}
}
#endif
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcAsyncNotificationProcessor::bcAsyncNotificationProcessor(iAsyncNotification *Queue)noexcept(true)
	: fAsyncNotification(Queue)
	, fNotificationActive(false)
	, fActiveMutex(false)
{
}
//---------------------------------------------------------------------------
bcAsyncNotificationProcessor::~bcAsyncNotificationProcessor()noexcept(true)
{
}
//---------------------------------------------------------------------------
iAsyncNotification* bcAsyncNotificationProcessor::GetAsyncNotification(void)const noexcept(true)
{
	return fAsyncNotification;
}
//---------------------------------------------------------------------------
bool bcAsyncNotificationProcessor::SetAsyncNotification(iAsyncNotification *Notification)noexcept(true)
{
	if(fActiveMutex.Acquire.Xchg(true)==true){
		return false;
	}
	if(fNotificationActive==false){
		fAsyncNotification=Notification;
	}
	
	fActiveMutex.Release.Store(false);
	return true;
}
//---------------------------------------------------------------------------
bool bcAsyncNotificationProcessor::IsActive(void)const noexcept(true)
{
	return fNotificationActive;
}
//---------------------------------------------------------------------------
bool bcAsyncNotificationProcessor::Start(iReference *Reference)noexcept(true)
{
	if(fAsyncNotification==nullptr){
		return false;
	}
	if(fNotificationActive)
		return false;
	if(fActiveMutex.Acquire.Xchg(true)==true){
		return false;
	}

	if(fAsyncNotification->StartNotify(Reference,this)==false){
		fActiveMutex.Release.Store(false);
		return false;
	}

	return true;
}
//---------------------------------------------------------------------------
void bcAsyncNotificationProcessor::Stop(void)noexcept(true)
{
	if(fAsyncNotification!=nullptr){
		if(fNotificationActive){
			fAsyncNotification->StopNotify();
		}
	}
}
//---------------------------------------------------------------------------
void bcAsyncNotificationProcessor::AsyncStarted(void)noexcept(true)
{
	fNotificationActive=true;
	fActiveMutex.Release.Store(false);
}
//---------------------------------------------------------------------------
void bcAsyncNotificationProcessor::AsyncStopped(void)noexcept(true)
{
	fNotificationActive=false;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcAsyncQueue::bcAsyncQueue()noexcept(true)
	: fAsyncQueueNotifyType(0)
	, fAsyncQueueAvailable(false)
{
	fAsyncQueueUpdateStateProc=&bcAsyncQueue::AsyncQueueUpdateStateProc_Run;
}
//---------------------------------------------------------------------------
bcAsyncQueue::~bcAsyncQueue()noexcept(true)
{
}
//---------------------------------------------------------------------------
void bcAsyncQueue::CreateQueueWork(iAsyncExecution *AsyncExecution,bool RestrictInAsyncExecution)noexcept(true)
{
	if(fAsyncQueueProcWork!=nullptr)
		return;

	fAsyncQueueProcWork=AsyncExecution->CreateWork(NotificationInnerReference(),fAsyncQueueStateProcedure);
	if(RestrictInAsyncExecution){
		fAsyncQueueUpdateStateProc=&bcAsyncQueue::AsyncQueueUpdateStateProc_RestrictInAsyncExecution;
	}
	else{
		fAsyncQueueUpdateStateProc=&bcAsyncQueue::AsyncQueueUpdateStateProc_Async;
	}
}
//---------------------------------------------------------------------------
void bcAsyncQueue::AsyncQueueUpdateStateProc_Run(bool)noexcept(true)
{
	fAsyncQueueStateProcedure.Run();
}
//---------------------------------------------------------------------------
void bcAsyncQueue::AsyncQueueUpdateStateProc_RestrictInAsyncExecution(bool)noexcept(true)
{
	fAsyncQueueStateProcedure.RunAsync(fAsyncQueueProcWork);
}
//---------------------------------------------------------------------------
void bcAsyncQueue::AsyncQueueUpdateStateProc_Async(bool AsyncNotify)noexcept(true)
{
	if(AsyncNotify){
		fAsyncQueueStateProcedure.RunAsync(fAsyncQueueProcWork);
	}
	else{
		fAsyncQueueStateProcedure.RunAsyncAfter(fAsyncQueueProcWork,1);
	}
}
//---------------------------------------------------------------------------
void bcAsyncQueue::UpdateQueueState(bool AsyncNotify)noexcept(true)
{
	(this->*fAsyncQueueUpdateStateProc)(AsyncNotify);
}
//---------------------------------------------------------------------------
bool bcAsyncQueue::cAsyncQueueStateProcedure::Procedure(void)noexcept(true)
{
	auto Host=cnMemory::GetObjectFromMemberPointer(this,&bcAsyncQueue::fAsyncQueueStateProcedure);
	return Host->RunNotifyCycle();
}
//---------------------------------------------------------------------------
void bcAsyncQueue::NotificationStarted(void)noexcept(true)
{
}
//---------------------------------------------------------------------------
void bcAsyncQueue::NotificationStopped(void)noexcept(true)
{
}
//---------------------------------------------------------------------------
void bcAsyncQueue::NotificationClosed(void)noexcept(true)
{
}
//---------------------------------------------------------------------------
bcAsyncQueue::CycleState bcAsyncQueue::NotificationCheckState(void)noexcept(true)
{
	return CycleState::Normal;
}
//---------------------------------------------------------------------------
void bcAsyncQueue::NotificationProcedure(void)noexcept(true)
{
	if(IsNotificationClosed()){
		AsyncQueueNotify();
		return;
	}
	switch(fAsyncQueueNotifyType){
	case 0:
		break;
	case 1:	// notify when available
		if(fAsyncQueueAvailable==false){
			break;
		}
		// notify
		cnLib_SWITCH_FALLTHROUGH;
	case 2:	// always notify
		fAsyncQueueNotifyType=0;
		fAsyncQueueAvailable=false;
		AsyncQueueNotify();
		break;
	}
}
//---------------------------------------------------------------------------
void bcAsyncQueue::NotifyQueue(bool IdleNotify)noexcept(true)
{
	if(IdleNotify){
		if(fAsyncQueueNotifyType<2){
			fAsyncQueueNotifyType=2;
			UpdateQueueState(true);
		}
	}
	else{
		if(fAsyncQueueNotifyType==0){
			fAsyncQueueNotifyType=1;
			UpdateQueueState(false);
		}
	}
}
//---------------------------------------------------------------------------
void bcAsyncQueue::CloseQueue(void)noexcept(true)
{
	if(CloseNotify()){
		UpdateQueueState(false);
	}
}
//---------------------------------------------------------------------------
void bcAsyncQueue::AsyncQueueSetAvailable(bool AsyncNotify)noexcept(true)
{
	if(fAsyncQueueAvailable==false){
		fAsyncQueueAvailable=true;
		UpdateQueueState(AsyncNotify);
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcAsyncNotification::bcAsyncNotification()noexcept(true)
{
	fAsyncNotificationCallback=nullptr;
}
//---------------------------------------------------------------------------
bcAsyncNotification::~bcAsyncNotification()noexcept(true)
{
}
//---------------------------------------------------------------------------
bool bcAsyncNotification::StartNotify(iReference *Reference,iAsyncNotificationCallback *Callback)noexcept(true)
{
	if(PrepareStartNotify()==false)
		return false;

	fAsyncNotificationCallback=Callback;
	fAsyncNotificationReference=Reference;

	ConfirmStartNotify();
	return true;
}
//---------------------------------------------------------------------------
void bcAsyncNotification::StopNotify(void)noexcept(true)
{
	if(bcAsyncQueue::StopNotify()){
		UpdateQueueState(false);
	}
}
//---------------------------------------------------------------------------
void bcAsyncNotification::NotifyCallback(bool IdleNotify)noexcept(true)
{
	return NotifyQueue(IdleNotify);
}
//---------------------------------------------------------------------------
bool bcAsyncNotification::IsClosed(void)noexcept(true)
{
	return IsNotificationClosed();
}
//---------------------------------------------------------------------------
void bcAsyncNotification::NotificationStarted(void)noexcept(true)
{
	rIncReference(this,'asnt');
	fAsyncNotificationCallback->AsyncStarted();
}
//---------------------------------------------------------------------------
void bcAsyncNotification::NotificationStopped(void)noexcept(true)
{
	auto CallbackReference=cnVar::MoveCast(fAsyncNotificationReference);
	auto Callback=fAsyncNotificationCallback;
	fAsyncNotificationCallback=nullptr;
	Callback->AsyncStopped();
	rDecReference(this,'asnt');
}
//---------------------------------------------------------------------------
void bcAsyncNotification::AsyncQueueNotify(void)noexcept(true)
{
	fAsyncNotificationCallback->AsyncNotify();
}
//---------------------------------------------------------------------------
