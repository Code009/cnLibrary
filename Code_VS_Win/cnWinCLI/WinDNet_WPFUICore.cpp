
#include "WinDNet_WPFUICore.h"


using namespace cnLibrary;
using namespace cnRTL;
using namespace cnWin;

//---------------------------------------------------------------------------
iUIThread* cnWin::DNetCurrentUIThread(void)noexcept
{
	return cDNetUIThread::CurrentUIThread();
}
//---------------------------------------------------------------------------
iPtr<iUIThread> cnWin::DNetCreateOnCurrentThread(void)noexcept
{
	return cDNetUIThread::CreateOnCurrentThread();
}
//---------------------------------------------------------------------------
iPtr<iUIThread> cnWin::DNetStartUIThread(void)noexcept
{
	return cDNetUIThread::StartUIThread();
}
//---------------------------------------------------------------------------
rPtr<iWindowsUIApplication> cnWin::DNetCreateWindowsUIApplication(void)noexcept
{
	auto UIThread=cDNetUIThread::CreateOnCurrentThread();
	return rCreate<cDNetUIApplication>(cnVar::MoveCast(UIThread));
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool cWPFKeyboard::EventIsKeyDown(eKeyCode KeyCode)noexcept
{
	return mcDNetUIThreadDispatcher::mKeyboardEventIsKeyDown(KeyCode);
}
//---------------------------------------------------------------------------
bool cWPFKeyboard::EventIsKeyToggled(eKeyCode KeyCode)noexcept
{
	return mcDNetUIThreadDispatcher::mKeyboardEventIsKeyToggled(KeyCode);
}
//---------------------------------------------------------------------------
bool cWPFKeyboard::IsKeyDown(eKeyCode KeyCode)noexcept
{
	return mcDNetUIThreadDispatcher::mKeyboardEventIsKeyDown(KeyCode);
}
//---------------------------------------------------------------------------
bool cWPFKeyboard::IsKeyToggled(eKeyCode KeyCode)noexcept
{
	return mcDNetUIThreadDispatcher::mKeyboardEventIsKeyToggled(KeyCode);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool cWPFMouse::GetPosition(iInterface *Relative,cUIPoint &Position)noexcept
{
	auto RelativeObject=iCast<iCLIObject>(Relative);
	auto &Handle=RelativeObject->GetObjecHandle();
	return mcDNetUIThreadDispatcher::mMouseGetPosition(Handle,Position);
}
//---------------------------------------------------------------------------
bool cWPFMouse::EventIsButtonDown(eMouseButton Button)noexcept
{
	return mcDNetUIThreadDispatcher::mMouseEventIsButtonDown(Button);
}
//---------------------------------------------------------------------------
bool cWPFMouse::IsButtonDown(eMouseButton Button)noexcept
{
	return mcDNetUIThreadDispatcher::mMouseEventIsButtonDown(Button);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cDNetUIThreadAsyncProcedure::cDNetUIThreadAsyncProcedure(cDNetUIThread *UIThread,bool HighPriority,iProcedure *Procedure)noexcept
	: fUIThread(UIThread)
	, mcDNetUIThreadAsyncProcedureExecutor(HighPriority,Procedure)
{
}
//---------------------------------------------------------------------------
cDNetUIThreadAsyncProcedure::~cDNetUIThreadAsyncProcedure()noexcept
{
}
//---------------------------------------------------------------------------
void cDNetUIThreadAsyncProcedure::Start(void)noexcept
{
	return mStart(fUIThread);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cDNetUIThreadAsyncProcedureRef::cDNetUIThreadAsyncProcedureRef(cDNetUIThread *UIThread,bool HighPriority,iReference *Reference,iProcedure *Procedure)noexcept
	: fUIThread(UIThread)
	, mcDNetUIThreadAsyncProcedureRefExecutor(HighPriority,Reference,Procedure)
{
}
//---------------------------------------------------------------------------
cDNetUIThreadAsyncProcedureRef::~cDNetUIThreadAsyncProcedureRef()noexcept
{
}
//---------------------------------------------------------------------------
void mcDNetUIThreadAsyncProcedureRefExecutor::CallProc(void)noexcept
{
	fProcedure->Execute();

	rDecReference(fReference,'exec');
}
//---------------------------------------------------------------------------
void cDNetUIThreadAsyncProcedureRef::Start(void)noexcept
{
	rIncReference(fReference,'exec');
	return mStart(fUIThread);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void mcDNetUIThreadAsyncTimerExecutor::TimerStart(mcDNetUIThreadDispatcher *UIDispatch,uInt64 DueTime,uInt64 Period)noexcept
{
	if(fTimerStateFlag.Acquire()==false)
		return;

	fTimerStateFlag.Continue();

	mStopTimer();
	fTimerStop=false;
	if(fTimerStarted==false){
		fTimerStarted=true;
		if(fReference!=nullptr)
			rIncReference(fReference,'timr');
	}
	fPeriod=Period;
	sInt64 NowST=cnSystem::GetSystemTimeNow();
	sInt64 DueST=DueTime;
	
	if(DueST<NowST){
		// hit the timer now and continue period
		mHitAndSetTimer(UIDispatch);
	}
	else{
		// schedule first hit
		mScheludeFirstHit(DueST-NowST);
	}

}
//---------------------------------------------------------------------------
void mcDNetUIThreadAsyncTimerExecutor::TimerStop(void)noexcept
{
	fTimerStop=true;
	if(fTimerStateFlag.Acquire()==false)
		return;

	TimerStateThreadProc();
}
//---------------------------------------------------------------------------
void mcDNetUIThreadAsyncTimerExecutor::ExecuteAndState(void)noexcept
{
	fProcedure->Execute();

	TimerStateThreadProc();
}
//---------------------------------------------------------------------------
void mcDNetUIThreadAsyncTimerExecutor::TimerStateThreadProc(void)noexcept
{
	do{
		fTimerStateFlag.Continue();
		if(fTimerStop){
			mStopTimer();
			if(fTimerStarted){
				if(fReference!=nullptr)
					rDecReference(fReference,'timr');
				fTimerStarted=false;
			}
			fTimerStop=false;
		}

	}while(fTimerStateFlag.Release()==false);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cDNetUIThreadAsyncTimer::cDNetUIThreadAsyncTimer(cDNetUIThread *UIDispatch,bool HighPriority,iReference *Reference,iProcedure *Procedure)noexcept
	: fUIDispatch(UIDispatch)
	, mcDNetUIThreadAsyncTimerExecutor(UIDispatch,HighPriority,Reference,Procedure)
{
}
//---------------------------------------------------------------------------
cDNetUIThreadAsyncTimer::~cDNetUIThreadAsyncTimer()noexcept
{
}
//---------------------------------------------------------------------------
void cDNetUIThreadAsyncTimer::Start(uInt64 DueTime,uInt64 Period)noexcept
{
	return TimerStart(fUIDispatch,DueTime,Period);
}
//---------------------------------------------------------------------------
void cDNetUIThreadAsyncTimer::Stop(void)noexcept
{
	return TimerStop();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cDNetUIThread* cDNetUIThread::mcWPFDispatchHigh::GetHost(void)noexcept
{
	return cnMemory::GetObjectFromMemberPointer(this,&cDNetUIThread::fDispatchHigh);
}
//---------------------------------------------------------------------------
void* cDNetUIThread::mcWPFDispatchHigh::CastInterface(iTypeID IID)noexcept
{
	if(IID==TInterfaceID<iReference>::Value){
		auto Host=GetHost();
		return Host->CastInterface(IID);
	}
	return iDispatch::CastInterface(IID);
}
//---------------------------------------------------------------------------
rPtr<iAsyncProcedure> cDNetUIThread::mcWPFDispatchHigh::CreateWork(iReference *Reference,iProcedure *ThreadProcedure)noexcept
{
	auto Host=GetHost();
	return Host->DispatchCreateWork(true,Reference,ThreadProcedure);
}
//---------------------------------------------------------------------------
rPtr<iAsyncTimer> cDNetUIThread::mcWPFDispatchHigh::CreateTimer(iReference *Reference,iProcedure *ThreadProcedure)noexcept
{
	auto Host=GetHost();
	return Host->DispatchCreateTimer(true,Reference,ThreadProcedure);
}
//---------------------------------------------------------------------------
bool cDNetUIThread::mcWPFDispatchHigh::IsCurrentThread(void)noexcept
{
	auto Host=GetHost();
	return Host->mIsCurrent();
}
//---------------------------------------------------------------------------
void cDNetUIThread::mcWPFDispatchHigh::Execute(iReference *Reference,iProcedure *Procedure)noexcept
{
	auto Host=GetHost();
	return Host->DispatchExecute(true,Reference,Procedure);
}
//---------------------------------------------------------------------------
void cDNetUIThread::mcWPFDispatchHigh::ExecuteSync(iProcedure *Procedure)noexcept
{
	auto Host=GetHost();
	return Host->mDispatchExecuteSync(true,Procedure);
}
//---------------------------------------------------------------------------
iPtr<iAsyncTask> cDNetUIThread::mcWPFDispatchHigh::ExecuteAsync(iReference *Reference,iProcedure *Procedure)noexcept
{
	auto Host=GetHost();
	return Host->DispatchExecuteAsync(true,Reference,Procedure);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cDNetUIThread* cDNetUIThread::mcWPFDispatchLow::GetHost(void)noexcept
{
	return cnMemory::GetObjectFromMemberPointer(this,&cDNetUIThread::fDispatchLow);
}
//---------------------------------------------------------------------------
void* cDNetUIThread::mcWPFDispatchLow::CastInterface(iTypeID IID)noexcept
{
	if(IID==TInterfaceID<iReference>::Value){
		auto Host=GetHost();
		return Host->CastInterface(IID);
	}
	return iDispatch::CastInterface(IID);
}
//---------------------------------------------------------------------------
rPtr<iAsyncProcedure> cDNetUIThread::mcWPFDispatchLow::CreateWork(iReference *Reference,iProcedure *ThreadProcedure)noexcept
{
	auto Host=GetHost();
	return Host->DispatchCreateWork(false,Reference,ThreadProcedure);
}
//------------------------------------------------------------------s---------
rPtr<iAsyncTimer> cDNetUIThread::mcWPFDispatchLow::CreateTimer(iReference *Reference,iProcedure *ThreadProcedure)noexcept
{
	auto Host=GetHost();
	return Host->DispatchCreateTimer(false,Reference,ThreadProcedure);
}
//---------------------------------------------------------------------------
bool cDNetUIThread::mcWPFDispatchLow::IsCurrentThread(void)noexcept
{
	auto Host=GetHost();
	return Host->mIsCurrent();
}
//---------------------------------------------------------------------------
void cDNetUIThread::mcWPFDispatchLow::Execute(iReference *Reference,iProcedure *Procedure)noexcept
{
	auto Host=GetHost();
	return Host->DispatchExecute(false,Reference,Procedure);
}
//---------------------------------------------------------------------------
void cDNetUIThread::mcWPFDispatchLow::ExecuteSync(iProcedure *Procedure)noexcept
{
	auto Host=GetHost();
	return Host->mDispatchExecuteSync(false,Procedure);
}
//---------------------------------------------------------------------------
iPtr<iAsyncTask> cDNetUIThread::mcWPFDispatchLow::ExecuteAsync(iReference *Reference,iProcedure *Procedure)noexcept
{
	auto Host=GetHost();
	return Host->DispatchExecuteAsync(false,Reference,Procedure);
}
//---------------------------------------------------------------------------
bool cDNetUIThread::cExecuteAsyncTask::IsDone(void)noexcept
{
	return TaskState.IsDone();
}
//---------------------------------------------------------------------------
bool cDNetUIThread::cExecuteAsyncTask::SetNotify(iProcedure *NotifyProcedure)noexcept
{
	return TaskState.SetNotify(NotifyProcedure);
}
//---------------------------------------------------------------------------
void cDNetUIThread::cExecuteAsyncTask::ExecuteTask(void)noexcept
{
	Procedure->Execute();

	TaskState.SetDone();

	iDecReference(this,'exec');
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cDNetUIThread::cDNetUIThread()noexcept
	: mcDNetUIThreadDispatcher(&fInnerReference)
{
	fWPFKeyboard=iCreate<cWPFKeyboard>();
	fWPFMouse=iCreate<cWPFMouse>();
}
//---------------------------------------------------------------------------
cDNetUIThread::~cDNetUIThread(void)noexcept
{
}
//---------------------------------------------------------------------------
void cDNetUIThread::VirtualStopped(void)noexcept
{
	mShutdownFrame();
	InnerDecReference('self');
}
//---------------------------------------------------------------------------
bool cDNetUIThread::IsCurrent(void)noexcept
{
	if(fDispatcherShutdown)
		return true;

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
bool cDNetUIThread::IsShutdown(void)noexcept
{
	return fDispatcherShutdown;
}
//---------------------------------------------------------------------------
bool cDNetUIThread::IsCurrentThread(void)noexcept
{
	return IsCurrent();
}
//---------------------------------------------------------------------------
cDNetUIThread* cDNetUIThread::CurrentUIThread(void)noexcept
{
	return static_cast<cDNetUIThread*>(mCurrentUIDispatcher());
}
//---------------------------------------------------------------------------
iPtr<cDNetUIThread> cDNetUIThread::CreateOnCurrentThread(void)noexcept
{
	auto DispatchFrame=mCurrentUIDispatcher();
	if(DispatchFrame!=nullptr)
		return static_cast<cDNetUIThread*>(DispatchFrame);

	// create ui thread in current thread
	auto Thread=iCreate<cDNetUIThread>();
	Thread->fNativeThreadID=::GetCurrentThreadId();

	if(Thread->mCreateOnCurrentThread()==false)
		return nullptr;

	return Thread;
}
//---------------------------------------------------------------------------
iPtr<cDNetUIThread> cDNetUIThread::StartUIThread(void)noexcept
{
	auto Thread=iCreate<cDNetUIThread>();
	rcDNetUIThreadDispatchFrame_StartUIThread(&Thread->fInnerReference,Thread);
	return Thread;
}
//---------------------------------------------------------------------------
iDispatch* cDNetUIThread::GetDispatch(bool HighPriority)noexcept
{
	if(HighPriority){
		return &fDispatchHigh;
	}
	else{
		return &fDispatchLow;
	}
}
//---------------------------------------------------------------------------
iUIKeyboard* cDNetUIThread::GetDefaultKeyboard(void)noexcept
{
	return fWPFKeyboard;
}
//---------------------------------------------------------------------------
iUIMouse* cDNetUIThread::GetDefaultMouse(void)noexcept
{
	return fWPFMouse;
}
//---------------------------------------------------------------------------
rPtr<iAsyncProcedure> cDNetUIThread::DispatchCreateWork(bool HighPriority,iReference *Reference,iProcedure *ThreadProcedure)noexcept
{
	if(ThreadProcedure==nullptr)
		return nullptr;
	if(Reference!=nullptr)
		return rCreate<cDNetUIThreadAsyncProcedureRef>(this,HighPriority,Reference,ThreadProcedure);
	else
		return rCreate<cDNetUIThreadAsyncProcedure>(this,HighPriority,ThreadProcedure);
}
//---------------------------------------------------------------------------
rPtr<iAsyncTimer> cDNetUIThread::DispatchCreateTimer(bool HighPriority,iReference *Reference,iProcedure *ThreadProcedure)noexcept
{
	auto AsyncTimer=rCreate<cDNetUIThreadAsyncTimer>(this,HighPriority,Reference,ThreadProcedure);
	return AsyncTimer;
}
//---------------------------------------------------------------------------
void cDNetUIThread::DispatchExecute(bool HighPriority,iReference *Reference,iProcedure *Procedure)noexcept
{
	if(Reference!=nullptr){
		rcRefProcedureCaller_RefProc(Reference);
		mDispatchExecuteReferenced(HighPriority,Reference,Procedure);
	}
	else{
		mDispatchExecute(HighPriority,Procedure);
	}
}
//---------------------------------------------------------------------------
iPtr<iAsyncTask> cDNetUIThread::DispatchExecuteAsync(bool HighPriority,iReference *Reference,iProcedure *Procedure)noexcept
{
	if(Reference!=nullptr)
		rIncReference(Reference,'exec');

	auto Task=iCreate<cExecuteAsyncTask>();
	Task->Reference=Reference;
	Task->Procedure=Procedure;

	iIncReference(Task,'exec');
	mDispatchExecuteAsync(Task,HighPriority);

	return Task;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cDNetUIApplication::cDNetUIApplication(iPtr<cDNetUIThread> UIThread)noexcept
	: fUIThread(cnVar::MoveCast(UIThread))
{
}
//---------------------------------------------------------------------------
cDNetUIApplication::~cDNetUIApplication()noexcept
{
}
//---------------------------------------------------------------------------
iUIThread* cDNetUIApplication::GetMainUIThread(void)noexcept
{
	return fUIThread;
}
//---------------------------------------------------------------------------
bool cDNetUIApplication::InsertHandler(iWindowsUISessionHandler *SessionHandler)noexcept
{
	if(fUIThread->IsCurrent()==false)
		return false;

	fHandlers.Insert(SessionHandler);
	return true;
}
//---------------------------------------------------------------------------
bool cDNetUIApplication::RemoveHandler(iWindowsUISessionHandler *SessionHandler)noexcept
{
	if(fUIThread->IsCurrent()==false)
		return false;

	fHandlers.Remove(SessionHandler);
	return true;
}
//---------------------------------------------------------------------------
void cDNetUIApplication::UIMain(void)noexcept
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
	mWPFUIMain();

	fUISession=false;

	Handlers=fHandlers.Storage();
	for(auto Handler : Handlers){
		Handler->UISessionExit();
	}
}
//---------------------------------------------------------------------------
void cDNetUIApplication::CloseUISession(void)noexcept
{
	if(fUISession)
		mNotifyStopRun();
}
//---------------------------------------------------------------------------
