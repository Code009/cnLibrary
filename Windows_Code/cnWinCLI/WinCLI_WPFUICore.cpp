#include "WinCLI_WPFUICore.h"


using namespace cnLibrary;
using namespace cnRTL;
using namespace cnWin;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWPFModule::cWPFModule()noexcept(true)
{
	//cnSystem::SystemDependentRegistration->Register(this);
}
//---------------------------------------------------------------------------
cWPFModule::~cWPFModule()noexcept(true)
{
	//cnSystem::SystemDependentRegistration->Unregister(this);
}
//---------------------------------------------------------------------------
//void cWPFModule::DependentShutdownNotification(void)noexcept(true)
//{
//	ObjectRegistration.Wait();
//	cnSystem::SystemDependentRegistration->Unregister(this);
//}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool cWPFKeyboard::EventIsKeyDown(eKeyCode KeyCode)noexcept(true)
{
	return mcWPFInput::mKeyboardEventIsKeyDown(KeyCode);
}
//---------------------------------------------------------------------------
bool cWPFKeyboard::EventIsKeyToggled(eKeyCode KeyCode)noexcept(true)
{
	return mcWPFInput::mKeyboardEventIsKeyToggled(KeyCode);
}
//---------------------------------------------------------------------------
bool cWPFKeyboard::IsKeyDown(eKeyCode KeyCode)noexcept(true)
{
	return mcWPFInput::mKeyboardEventIsKeyDown(KeyCode);
}
//---------------------------------------------------------------------------
bool cWPFKeyboard::IsKeyToggled(eKeyCode KeyCode)noexcept(true)
{
	return mcWPFInput::mKeyboardEventIsKeyToggled(KeyCode);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool cWPFMouse::GetPosition(iInterface *Relative,cUIPoint &Position)noexcept(true)
{
	auto RelativeObject=iCast<iCLIObject>(Relative);
	auto &Handle=RelativeObject->GetObjecHandle();
	return mcWPFInput::mMouseGetPosition(Handle,Position);
}
//---------------------------------------------------------------------------
bool cWPFMouse::EventIsButtonDown(eMouseButton Button)noexcept(true)
{
	return mcWPFInput::mMouseEventIsButtonDown(Button);
}
//---------------------------------------------------------------------------
bool cWPFMouse::IsButtonDown(eMouseButton Button)noexcept(true)
{
	return mcWPFInput::mMouseEventIsButtonDown(Button);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWPFUIThreadAsyncProcedure::cWPFUIThreadAsyncProcedure(cWPFUIThread *UIThread,bool HighPriority,iProcedure *Procedure)noexcept(true)
	: fUIThread(UIThread)
	, fHighPriority(HighPriority)
{
	fExecutor.mSetup(Procedure);
}
//---------------------------------------------------------------------------
cWPFUIThreadAsyncProcedure::~cWPFUIThreadAsyncProcedure()noexcept(true)
{
	fExecutor.mClear();
}
//---------------------------------------------------------------------------
void cWPFUIThreadAsyncProcedure::Start(void)noexcept(true)
{
	fUIThread->mExecuteDelegate(fHighPriority,fExecutor);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWPFUIThreadAsyncProcedureRef::cWPFUIThreadAsyncProcedureRef(cWPFUIThread *UIThread,bool HighPriority,iReference *Reference,iProcedure *Procedure)noexcept(true)
	: fUIThread(UIThread)
	, fReference(Reference)
	, fProcedure(Procedure)
	, fHighPriority(HighPriority)
{
	mbcCLICallback::mSetup();
}
//---------------------------------------------------------------------------
cWPFUIThreadAsyncProcedureRef::~cWPFUIThreadAsyncProcedureRef()noexcept(true)
{
	mbcCLICallback::mClear();
}
//---------------------------------------------------------------------------
void cWPFUIThreadAsyncProcedureRef::Start(void)noexcept(true)
{
	rIncReference(fReference,'exec');
	fUIThread->mExecuteDelegate(fHighPriority,*this);
}
//---------------------------------------------------------------------------
void cWPFUIThreadAsyncProcedureRef::CLIExecute(void)noexcept(true)
{
	fProcedure->Execute();
	rDecReference(fReference,'exec');
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWPFUIThreadAsyncTimer::cWPFUIThreadAsyncTimer(cWPFUIThread *UIDispatch,bool HighPriority,iReference *Reference,iProcedure *Procedure)noexcept(true)
	: fUIDispatch(UIDispatch)
	, fReference(Reference)
	, fProcedure(Procedure)
{
	fExecutor.mSetup(UIDispatch,HighPriority);
}
//---------------------------------------------------------------------------
cWPFUIThreadAsyncTimer::~cWPFUIThreadAsyncTimer()noexcept(true)
{
	fExecutor.mClear();
}
//---------------------------------------------------------------------------
void cWPFUIThreadAsyncTimer::VirtualStopped(void)noexcept(true)
{
	fClose=true;
	NotifyStateProcedure();

	cDualReference::VirtualStopped();
}
//---------------------------------------------------------------------------
void cWPFUIThreadAsyncTimer::Start(uInt64 DueTime,uInt64 Period)noexcept(true)
{
	fDueTime=DueTime;
	fPeriod=Period;
	fActive=true;
	fUpdate=true;

	if(fReference!=nullptr)
		fStateProcRefrence<<=fReference;
	NotifyStateProcedure();
}
//---------------------------------------------------------------------------
void cWPFUIThreadAsyncTimer::Stop(void)noexcept(true)
{
	fActive=false;
	NotifyStateProcedure();
}
//---------------------------------------------------------------------------
void cWPFUIThreadAsyncTimer::cExecutor::Schedule(void)noexcept(true)
{
	auto Host=cnMemory::GetObjectFromMemberPointer(this,&cWPFUIThreadAsyncTimer::fExecutor);
	return Host->StateProcedureThread();
}
//---------------------------------------------------------------------------
void cWPFUIThreadAsyncTimer::cExecutor::TimerTick(void)noexcept(true)
{
	auto Host=cnMemory::GetObjectFromMemberPointer(this,&cWPFUIThreadAsyncTimer::fExecutor);
	return Host->TimerTick();
}
//---------------------------------------------------------------------------
void cWPFUIThreadAsyncTimer::cExecutor::TimerCleanup(void)noexcept(true)
{
	auto Host=cnMemory::GetObjectFromMemberPointer(this,&cWPFUIThreadAsyncTimer::fExecutor);
	if(Host->fReference!=nullptr)
		rDecReference(Host->fReference,'timr');
	Host->InnerDecReference('timr');

}
//---------------------------------------------------------------------------
void cWPFUIThreadAsyncTimer::TimerTick(void)noexcept(true)
{
	fProcedure->Execute();
	if(fTimerDue){
		// first hit
		fTimerDue=false;

		if(fPeriod!=0){
			// continue time with period interval
			fExecutor.mStart(fPeriod);
		}
		else{
			fExecutor.mStop();
		}
	}
}
//---------------------------------------------------------------------------
void cWPFUIThreadAsyncTimer::NotifyStateProcedure(void)noexcept(true)
{
	if(fTimerStateFlag.Acquire()==false)
		return;

	InnerIncReference('stat');

	if(fUIDispatch->IsCurrent()){
		StateProcedureThread();
	}
	else{
		fExecutor.mSchedule();
	}
}
//---------------------------------------------------------------------------
void cWPFUIThreadAsyncTimer::StateProcedureThread(void)noexcept(true)
{
	do{
		fTimerStateFlag.Continue();

		StateProcedure();
		fStateProcRefrence<<=nullptr;

	}while(fTimerStateFlag.Release()==false);
	InnerDecReference('stat');
}
//---------------------------------------------------------------------------
void cWPFUIThreadAsyncTimer::StateProcedure(void)noexcept(true)
{
	if(fTimerActive){
		if(fActive){
			if(fUpdate){
				fUpdate=false;
				State_UpdateTimer();
			}
		}
		else{
			fTimerActive=false;

			// stop timer
			fExecutor.mStopCleanup();
		}
	}
	else{
		if(fActive){
			fTimerActive=true;
			InnerIncReference('timr');
			if(fReference!=nullptr)
				rIncReference(fReference,'timr');

			fUpdate=false;
			State_UpdateTimer();
		}
	}
}
//---------------------------------------------------------------------------
void cWPFUIThreadAsyncTimer::State_UpdateTimer(void)noexcept(true)
{
	fTimerDue=true;
	// start timer
	uInt64 Now=cnSystem::GetSystemTimeNow();
	if(fDueTime<Now){
		// hit the timer now and continue period
		fExecutor.mHit();
	}
	else{
		// schedule first hit
		fExecutor.mStart(Now-fDueTime);
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWPFUIThread* cWPFUIThread::mcWPFDispatchHigh::GetHost(void)noexcept(true)
{
	return cnMemory::GetObjectFromMemberPointer(this,&cWPFUIThread::fDispatchHigh);
}
//---------------------------------------------------------------------------
void* cWPFUIThread::mcWPFDispatchHigh::CastInterface(iTypeID IID)noexcept(true)
{
	if(IID==TInterfaceID<iReference>::Value){
		auto Host=GetHost();
		return Host->CastInterface(IID);
	}
	return iDispatch::CastInterface(IID);
}
//---------------------------------------------------------------------------
rPtr<iAsyncProcedure> cWPFUIThread::mcWPFDispatchHigh::CreateWork(iReference *Reference,iProcedure *ThreadProcedure)noexcept(true)
{
	auto Host=GetHost();
	return Host->DispatchCreateWork(true,Reference,ThreadProcedure);
}
//---------------------------------------------------------------------------
rPtr<iAsyncTimer> cWPFUIThread::mcWPFDispatchHigh::CreateTimer(iReference *Reference,iProcedure *ThreadProcedure)noexcept(true)
{
	auto Host=GetHost();
	return Host->DispatchCreateTimer(true,Reference,ThreadProcedure);
}
//---------------------------------------------------------------------------
bool cWPFUIThread::mcWPFDispatchHigh::IsCurrentThread(void)noexcept(true)
{
	auto Host=GetHost();
	return Host->IsCurrent();
}
//---------------------------------------------------------------------------
void cWPFUIThread::mcWPFDispatchHigh::Execute(iReference *Reference,iProcedure *Procedure)noexcept(true)
{
	auto Host=GetHost();
	return Host->DispatchExecute(true,Reference,Procedure);
}
//---------------------------------------------------------------------------
void cWPFUIThread::mcWPFDispatchHigh::ExecuteSync(iProcedure *Procedure)noexcept(true)
{
	auto Host=GetHost();
	return Host->mDispatchExecuteSync(true,Procedure);
}
//---------------------------------------------------------------------------
iPtr<iAsyncTask> cWPFUIThread::mcWPFDispatchHigh::ExecuteAsync(iReference *Reference,iProcedure *Procedure)noexcept(true)
{
	auto Host=GetHost();
	return Host->DispatchExecuteAsync(true,Reference,Procedure);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWPFUIThread* cWPFUIThread::mcWPFDispatchLow::GetHost(void)noexcept(true)
{
	return cnMemory::GetObjectFromMemberPointer(this,&cWPFUIThread::fDispatchLow);
}
//---------------------------------------------------------------------------
void* cWPFUIThread::mcWPFDispatchLow::CastInterface(iTypeID IID)noexcept(true)
{
	if(IID==TInterfaceID<iReference>::Value){
		auto Host=GetHost();
		return Host->CastInterface(IID);
	}
	return iDispatch::CastInterface(IID);
}
//---------------------------------------------------------------------------
rPtr<iAsyncProcedure> cWPFUIThread::mcWPFDispatchLow::CreateWork(iReference *Reference,iProcedure *ThreadProcedure)noexcept(true)
{
	auto Host=GetHost();
	return Host->DispatchCreateWork(false,Reference,ThreadProcedure);
}
//------------------------------------------------------------------s---------
rPtr<iAsyncTimer> cWPFUIThread::mcWPFDispatchLow::CreateTimer(iReference *Reference,iProcedure *ThreadProcedure)noexcept(true)
{
	auto Host=GetHost();
	return Host->DispatchCreateTimer(false,Reference,ThreadProcedure);
}
//---------------------------------------------------------------------------
bool cWPFUIThread::mcWPFDispatchLow::IsCurrentThread(void)noexcept(true)
{
	auto Host=GetHost();
	return Host->IsCurrent();
}
//---------------------------------------------------------------------------
void cWPFUIThread::mcWPFDispatchLow::Execute(iReference *Reference,iProcedure *Procedure)noexcept(true)
{
	auto Host=GetHost();
	return Host->DispatchExecute(false,Reference,Procedure);
}
//---------------------------------------------------------------------------
void cWPFUIThread::mcWPFDispatchLow::ExecuteSync(iProcedure *Procedure)noexcept(true)
{
	auto Host=GetHost();
	return Host->mDispatchExecuteSync(false,Procedure);
}
//---------------------------------------------------------------------------
iPtr<iAsyncTask> cWPFUIThread::mcWPFDispatchLow::ExecuteAsync(iReference *Reference,iProcedure *Procedure)noexcept(true)
{
	auto Host=GetHost();
	return Host->DispatchExecuteAsync(false,Reference,Procedure);
}
//---------------------------------------------------------------------------
cWPFUIThread::cExecuteAsyncTask::cExecuteAsyncTask(iReference *Reference,iProcedure *Procedure)noexcept(true)
	: Reference(Reference)
	, Procedure(Procedure)
{
}
//---------------------------------------------------------------------------
bool cWPFUIThread::cExecuteAsyncTask::IsDone(void)noexcept(true)
{
	return TaskState.IsDone();
}
//---------------------------------------------------------------------------
bool cWPFUIThread::cExecuteAsyncTask::Await(iProcedure *NotifyProcedure)noexcept(true)
{
	return TaskState.Await(NotifyProcedure);
}
//---------------------------------------------------------------------------
void cWPFUIThread::cExecuteAsyncTask::Queue(mbcWPFUIThread *WPFThread,bool HighPriority)noexcept(true)
{
	iIncReference(this,'exec');
	WPFThread->mDispatchExecute(HighPriority,this);
}
//---------------------------------------------------------------------------
void cWPFUIThread::cExecuteAsyncTask::Execute(void)noexcept(true)
{
	Procedure->Execute();

	TaskState.SetDone();

	iDecReference(this,'exec');
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void mbcWPFUIThread::nExecuteProcedureReference(iReference *Reference,iProcedure *Procedure)noexcept(true)
{
	Procedure->Execute();
	rDecReference(Reference,'disp');
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void cWPFUIThread::cCreateOnCurrentProcedure::Execute(mcWPFDispatcherReference &DispatcherReference)noexcept(true)
{
	Thread=iCreate<cWPFUIThread>(DispatcherReference);
}
//---------------------------------------------------------------------------
iPtr<cWPFUIThread> cWPFUIThread::CreateOnCurrentThread(void)noexcept(true)
{
	cCreateOnCurrentProcedure ThreadCallback;
	auto WPFThread=mbcWPFUIThread::mCurrentUIThread(&ThreadCallback);
	if(WPFThread!=nullptr)
		return static_cast<cWPFUIThread*>(WPFThread);

	return cnVar::MoveCast(ThreadCallback.Thread);
}
//---------------------------------------------------------------------------
void cWPFUIThread::cThreadSetupProcedure::Wait(void)noexcept(true)
{
	fNotifier.Wait();
}
//---------------------------------------------------------------------------
void cWPFUIThread::cThreadSetupProcedure::Execute(mcWPFDispatcherReference *DispatcherRefrence)noexcept(true)
{
	if(DispatcherRefrence!=nullptr){
		Thread=iCreate<cWPFUIThread>(*DispatcherRefrence);
	}
	fNotifier.Notify();
}
//---------------------------------------------------------------------------
iPtr<cWPFUIThread> cWPFUIThread::StartUIThread(void)noexcept(true)
{
	cThreadSetupProcedure ThreadCallback;
	WPFUIThreadNewThread(&ThreadCallback);

	ThreadCallback.Wait();
	return cnVar::MoveCast(ThreadCallback.Thread);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWPFUIThread::cWPFUIThread(mcWPFDispatcherReference &DispatcherReference)noexcept(true)
{
	mbcWPFUIThread::mSetup(DispatcherReference);
	fWPFKeyboard=iCreate<cWPFKeyboard>();
	fWPFMouse=iCreate<cWPFMouse>();
	fNativeThreadID=::GetCurrentThreadId();
}
//---------------------------------------------------------------------------
cWPFUIThread::~cWPFUIThread(void)noexcept(true)
{
	mbcWPFUIThread::mClear();
}
//---------------------------------------------------------------------------
void cWPFUIThread::VirtualStarted(void)noexcept(true)
{
	cDualReference::VirtualStarted();
}
//---------------------------------------------------------------------------
void cWPFUIThread::VirtualStopped(void)noexcept(true)
{
	InnerIncReference('clea');
	mbcWPFUIThread::mCPPClose();
	cDualReference::VirtualStopped();
}
//---------------------------------------------------------------------------
void cWPFUIThread::WPFOnCPPClose(void)noexcept(true)
{
	InnerDecReference('clea');
}
//---------------------------------------------------------------------------
void cWPFUIThread::WPFOnShutdown(void)noexcept(true)
{
	fDispatcherShutdown=true;
}
//---------------------------------------------------------------------------
bool cWPFUIThread::IsCurrent(void)const noexcept(true)
{
	if(fDispatcherShutdown)
		return false;

	DWORD CurrentID=::GetCurrentThreadId();
	if(fNativeThreadID==CurrentID){
		return true;
	}
	// in case the managed thread shifted to another native thread
	if(mIsCurrent()==false){
		return false;
	}
	fNativeThreadID=CurrentID;
	return true;
}
//---------------------------------------------------------------------------
bool cWPFUIThread::IsCurrentThread(void)noexcept(true)
{
	return IsCurrent();
}
//---------------------------------------------------------------------------
cWPFUIThread* cWPFUIThread::CurrentUIThread(void)noexcept(true)
{
	auto WPFThread=mbcWPFUIThread::mCurrentUIThread();
	if(WPFThread==nullptr)
		return nullptr;
	return static_cast<cWPFUIThread*>(WPFThread);
}
//---------------------------------------------------------------------------
iDispatch* cWPFUIThread::GetDispatch(bool HighPriority)noexcept(true)
{
	if(HighPriority){
		return &fDispatchHigh;
	}
	else{
		return &fDispatchLow;
	}
}
//---------------------------------------------------------------------------
iUIKeyboard* cWPFUIThread::GetDefaultKeyboard(void)noexcept(true)
{
	return fWPFKeyboard;
}
//---------------------------------------------------------------------------
iUIMouse* cWPFUIThread::GetDefaultMouse(void)noexcept(true)
{
	return fWPFMouse;
}
//---------------------------------------------------------------------------
rPtr<iAsyncProcedure> cWPFUIThread::DispatchCreateWork(bool HighPriority,iReference *Reference,iProcedure *ThreadProcedure)noexcept(true)
{
	if(ThreadProcedure==nullptr)
		return nullptr;
	if(Reference!=nullptr)
		return rCreate<cWPFUIThreadAsyncProcedureRef>(this,HighPriority,Reference,ThreadProcedure);
	else
		return rCreate<cWPFUIThreadAsyncProcedure>(this,HighPriority,ThreadProcedure);
}
//---------------------------------------------------------------------------
rPtr<iAsyncTimer> cWPFUIThread::DispatchCreateTimer(bool HighPriority,iReference *Reference,iProcedure *ThreadProcedure)noexcept(true)
{
	auto AsyncTimer=rCreate<cWPFUIThreadAsyncTimer>(this,HighPriority,Reference,ThreadProcedure);
	return AsyncTimer;
}
//---------------------------------------------------------------------------
void cWPFUIThread::DispatchExecute(bool HighPriority,iReference *Reference,iProcedure *Procedure)noexcept(true)
{
	if(Reference!=nullptr){
		rIncReference(Reference,'disp');
		mbcWPFUIThread::mDispatchExecuteReferenced(HighPriority,Reference,Procedure);
	}
	else{
		mbcWPFUIThread::mDispatchExecute(HighPriority,Procedure);
	}
}
//---------------------------------------------------------------------------
iPtr<iAsyncTask> cWPFUIThread::DispatchExecuteAsync(bool HighPriority,iReference *Reference,iProcedure *Procedure)noexcept(true)
{
	auto Task=iCreate<cExecuteAsyncTask>(Reference,Procedure);

	Task->Queue(this,HighPriority);

	return Task;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWPFUIApplication::cWPFUIApplication(iPtr<cWPFUIThread> UIThread)noexcept(true)
	: fUIThread(cnVar::MoveCast(UIThread))
{
	fDispatchFrame.mSetup();
}
//---------------------------------------------------------------------------
cWPFUIApplication::~cWPFUIApplication()noexcept(true)
{
	fDispatchFrame.mClear();
}
//---------------------------------------------------------------------------
iUIThread* cWPFUIApplication::GetMainUIThread(void)noexcept(true)
{
	return fUIThread;
}
//---------------------------------------------------------------------------
bool cWPFUIApplication::InsertHandler(iWindowsUISessionHandler *SessionHandler)noexcept(true)
{
	if(fUIThread->IsCurrent()==false)
		return false;

	fHandlers.Insert(SessionHandler);
	return true;
}
//---------------------------------------------------------------------------
bool cWPFUIApplication::RemoveHandler(iWindowsUISessionHandler *SessionHandler)noexcept(true)
{
	if(fUIThread->IsCurrent()==false)
		return false;

	fHandlers.Remove(SessionHandler);
	return true;
}
//---------------------------------------------------------------------------
void cWPFUIApplication::UIMain(void)noexcept(true)
{
	if(fUIThread->IsCurrent()==false)
		return;

	if(fUISession)
		return;

	fUISession=true;

	cSeqList<iWindowsUISessionHandler*> Handlers;
	Handlers=fHandlers.Storage();
	for(auto Handler : Handlers){
		Handler->UISessionStart();
	}
	// message loop
	fDispatchFrame.mUIMain();

	fUISession=false;

	Handlers=fHandlers.Storage();
	for(auto Handler : Handlers){
		Handler->UISessionExit();
	}
}
//---------------------------------------------------------------------------
void cWPFUIApplication::CloseUISession(void)noexcept(true)
{
	if(fUISession)
		fDispatchFrame.mNotifyStopRun();
}
//---------------------------------------------------------------------------
rPtr<iWindowsUIApplication> cWPFUIApplication::CreateWindowsUIApplication(void)noexcept(true)
{
	auto UIThread=cWPFUIThread::CreateOnCurrentThread();
	return rCreate<cWPFUIApplication>(cnVar::MoveCast(UIThread));
}
//---------------------------------------------------------------------------
