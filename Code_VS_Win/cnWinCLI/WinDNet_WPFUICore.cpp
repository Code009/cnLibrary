
#include "WinDNet_WPFUICore.h"


using namespace cnLibrary;
using namespace cnRTL;
using namespace cnWin;

//---------------------------------------------------------------------------
iUIThread* cnWin::DNetCurrentUIThread(void)
{
	return cDNetUIThread::CurrentUIThread();
}
//---------------------------------------------------------------------------
iPtr<iUIThread> cnWin::DNetCreateOnCurrentThread(void)
{
	return cDNetUIThread::CreateOnCurrentThread();
}
//---------------------------------------------------------------------------
iPtr<iUIThread> cnWin::DNetStartUIThread(void)
{
	return cDNetUIThread::StartUIThread();
}
//---------------------------------------------------------------------------
rPtr<iWindowsUIApplication> cnWin::DNetCreateWindowsUIApplication(void)
{
	auto UIThread=cDNetUIThread::CreateOnCurrentThread();
	return rCreate<cDNetUIApplication>(cnVar::MoveCast(UIThread));
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool cWPFKeyboard::EventIsKeyDown(eKeyCode KeyCode)
{
	return mcDNetUIThreadDispatcher::mKeyboardEventIsKeyDown(KeyCode);
}
//---------------------------------------------------------------------------
bool cWPFKeyboard::EventIsKeyToggled(eKeyCode KeyCode)
{
	return mcDNetUIThreadDispatcher::mKeyboardEventIsKeyToggled(KeyCode);
}
//---------------------------------------------------------------------------
bool cWPFKeyboard::IsKeyDown(eKeyCode KeyCode)
{
	return mcDNetUIThreadDispatcher::mKeyboardEventIsKeyDown(KeyCode);
}
//---------------------------------------------------------------------------
bool cWPFKeyboard::IsKeyToggled(eKeyCode KeyCode)
{
	return mcDNetUIThreadDispatcher::mKeyboardEventIsKeyToggled(KeyCode);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool cWPFMouse::GetPosition(iInterface *Relative,cUIPoint &Position)
{
	auto RelativeObject=iCast<iCLIObject>(Relative);
	auto &Handle=RelativeObject->GetObjecHandle();
	return mcDNetUIThreadDispatcher::mMouseGetPosition(Handle,Position);
}
//---------------------------------------------------------------------------
bool cWPFMouse::EventIsButtonDown(eMouseButton Button)
{
	return mcDNetUIThreadDispatcher::mMouseEventIsButtonDown(Button);
}
//---------------------------------------------------------------------------
bool cWPFMouse::IsButtonDown(eMouseButton Button)
{
	return mcDNetUIThreadDispatcher::mMouseEventIsButtonDown(Button);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cDNetUIThreadAsyncProcedure::cDNetUIThreadAsyncProcedure(cDNetUIThread *UIThread,bool HighPriority,iProcedure *Procedure)
	: fUIThread(UIThread)
	, mcDNetUIThreadAsyncProcedureExecutor(HighPriority,Procedure)
{
}
//---------------------------------------------------------------------------
cDNetUIThreadAsyncProcedure::~cDNetUIThreadAsyncProcedure()
{
}
//---------------------------------------------------------------------------
void cDNetUIThreadAsyncProcedure::Start(void)
{
	return mStart(fUIThread);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cDNetUIThreadAsyncProcedureRef::cDNetUIThreadAsyncProcedureRef(cDNetUIThread *UIThread,bool HighPriority,iReference *Reference,iProcedure *Procedure)
	: fUIThread(UIThread)
	, mcDNetUIThreadAsyncProcedureRefExecutor(HighPriority,Reference,Procedure)
{
}
//---------------------------------------------------------------------------
cDNetUIThreadAsyncProcedureRef::~cDNetUIThreadAsyncProcedureRef()
{
}
//---------------------------------------------------------------------------
void mcDNetUIThreadAsyncProcedureRefExecutor::CallProc(void)
{
	fProcedure->Execute();

	rDecReference(fReference,'exec');
}
//---------------------------------------------------------------------------
void cDNetUIThreadAsyncProcedureRef::Start(void)
{
	rIncReference(fReference,'exec');
	return mStart(fUIThread);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void mcDNetUIThreadAsyncTimerExecutor::TimerStart(mcDNetUIThreadDispatcher *UIDispatch,iTimepoint *DueTime,sInt64 DueTimeDelay,uInt64 Period)
{
	if(DueTime==nullptr)
		return;

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
	sInt64 DueST=DueTime->SystemTime();
	DueST+=DueTimeDelay;
	
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
void mcDNetUIThreadAsyncTimerExecutor::TimerStop(void)
{
	fTimerStop=true;
	if(fTimerStateFlag.Acquire()==false)
		return;

	TimerStateThreadProc();
}
//---------------------------------------------------------------------------
void mcDNetUIThreadAsyncTimerExecutor::ExecuteAndState(void)
{
	fProcedure->Execute();

	TimerStateThreadProc();
}
//---------------------------------------------------------------------------
void mcDNetUIThreadAsyncTimerExecutor::TimerStateThreadProc(void)
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
cDNetUIThreadAsyncTimer::cDNetUIThreadAsyncTimer(cDNetUIThread *UIDispatch,bool HighPriority,iReference *Reference,iProcedure *Procedure)
	: fUIDispatch(UIDispatch)
	, mcDNetUIThreadAsyncTimerExecutor(UIDispatch,HighPriority,Reference,Procedure)
{
}
//---------------------------------------------------------------------------
cDNetUIThreadAsyncTimer::~cDNetUIThreadAsyncTimer()
{
}
//---------------------------------------------------------------------------
void cDNetUIThreadAsyncTimer::Start(iTimepoint *DueTime,sInt64 DueTimeDelay,uInt64 Period)
{
	return TimerStart(fUIDispatch,DueTime,DueTimeDelay,Period);
}
//---------------------------------------------------------------------------
void cDNetUIThreadAsyncTimer::Stop(void)
{
	return TimerStop();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cDNetUIThread* cDNetUIThread::mcWPFDispatchHigh::GetHost(void)
{
	return cnMemory::GetObjectFromMemberPointer(this,&cDNetUIThread::fDispatchHigh);
}
//---------------------------------------------------------------------------
void* cDNetUIThread::mcWPFDispatchHigh::CastInterface(iTypeID IID)noexcept(true)
{
	if(IID==TInterfaceID<iReference>::Value){
		auto Host=GetHost();
		return Host->CastInterface(IID);
	}
	return iDispatch::CastInterface(IID);
}
//---------------------------------------------------------------------------
rPtr<iAsyncProcedure> cDNetUIThread::mcWPFDispatchHigh::CreateWork(iReference *Reference,iProcedure *ThreadProcedure)
{
	auto Host=GetHost();
	return Host->DispatchCreateWork(true,Reference,ThreadProcedure);
}
//---------------------------------------------------------------------------
rPtr<iAsyncTimer> cDNetUIThread::mcWPFDispatchHigh::CreateTimer(iReference *Reference,iProcedure *ThreadProcedure)
{
	auto Host=GetHost();
	return Host->DispatchCreateTimer(true,Reference,ThreadProcedure);
}
//---------------------------------------------------------------------------
bool cDNetUIThread::mcWPFDispatchHigh::IsCurrentThread(void)
{
	auto Host=GetHost();
	return Host->mIsCurrent();
}
//---------------------------------------------------------------------------
void cDNetUIThread::mcWPFDispatchHigh::Execute(iReference *Reference,iProcedure *Procedure)
{
	auto Host=GetHost();
	return Host->DispatchExecute(true,Reference,Procedure);
}
//---------------------------------------------------------------------------
void cDNetUIThread::mcWPFDispatchHigh::ExecuteSync(iProcedure *Procedure)
{
	auto Host=GetHost();
	return Host->mDispatchExecuteSync(true,Procedure);
}
//---------------------------------------------------------------------------
iPtr<iAsyncTask> cDNetUIThread::mcWPFDispatchHigh::ExecuteAsync(iReference *Reference,iProcedure *Procedure)
{
	auto Host=GetHost();
	return Host->DispatchExecuteAsync(true,Reference,Procedure);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cDNetUIThread* cDNetUIThread::mcWPFDispatchLow::GetHost(void)
{
	return cnMemory::GetObjectFromMemberPointer(this,&cDNetUIThread::fDispatchLow);
}
//---------------------------------------------------------------------------
void* cDNetUIThread::mcWPFDispatchLow::CastInterface(iTypeID IID)noexcept(true)
{
	if(IID==TInterfaceID<iReference>::Value){
		auto Host=GetHost();
		return Host->CastInterface(IID);
	}
	return iDispatch::CastInterface(IID);
}
//---------------------------------------------------------------------------
rPtr<iAsyncProcedure> cDNetUIThread::mcWPFDispatchLow::CreateWork(iReference *Reference,iProcedure *ThreadProcedure)
{
	auto Host=GetHost();
	return Host->DispatchCreateWork(false,Reference,ThreadProcedure);
}
//------------------------------------------------------------------s---------
rPtr<iAsyncTimer> cDNetUIThread::mcWPFDispatchLow::CreateTimer(iReference *Reference,iProcedure *ThreadProcedure)
{
	auto Host=GetHost();
	return Host->DispatchCreateTimer(false,Reference,ThreadProcedure);
}
//---------------------------------------------------------------------------
bool cDNetUIThread::mcWPFDispatchLow::IsCurrentThread(void)
{
	auto Host=GetHost();
	return Host->mIsCurrent();
}
//---------------------------------------------------------------------------
void cDNetUIThread::mcWPFDispatchLow::Execute(iReference *Reference,iProcedure *Procedure)
{
	auto Host=GetHost();
	return Host->DispatchExecute(false,Reference,Procedure);
}
//---------------------------------------------------------------------------
void cDNetUIThread::mcWPFDispatchLow::ExecuteSync(iProcedure *Procedure)
{
	auto Host=GetHost();
	return Host->mDispatchExecuteSync(false,Procedure);
}
//---------------------------------------------------------------------------
iPtr<iAsyncTask> cDNetUIThread::mcWPFDispatchLow::ExecuteAsync(iReference *Reference,iProcedure *Procedure)
{
	auto Host=GetHost();
	return Host->DispatchExecuteAsync(false,Reference,Procedure);
}
//---------------------------------------------------------------------------
bool cDNetUIThread::cExecuteAsyncTask::IsDone(void)
{
	return TaskState.IsDone();
}
//---------------------------------------------------------------------------
bool cDNetUIThread::cExecuteAsyncTask::SetNotify(iProcedure *NotifyProcedure)
{
	return TaskState.SetNotify(NotifyProcedure);
}
//---------------------------------------------------------------------------
void cDNetUIThread::cExecuteAsyncTask::ExecuteTask(void)
{
	Procedure->Execute();

	TaskState.SetDone();

	iDecReference(this,'exec');
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cDNetUIThread::cDNetUIThread()
	: mcDNetUIThreadDispatcher(&fInnerReference)
{
	fWPFKeyboard=iCreate<cWPFKeyboard>();
	fWPFMouse=iCreate<cWPFMouse>();
}
//---------------------------------------------------------------------------
cDNetUIThread::~cDNetUIThread(void)
{
}
//---------------------------------------------------------------------------
void cDNetUIThread::VirtualStopped(void)
{
	mShutdownFrame();
	InnerDecReference('self');
}
//---------------------------------------------------------------------------
bool cDNetUIThread::IsCurrent(void)
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
bool cDNetUIThread::IsShutdown(void)
{
	return fDispatcherShutdown;
}
//---------------------------------------------------------------------------
bool cDNetUIThread::IsCurrentThread(void)
{
	return IsCurrent();
}
//---------------------------------------------------------------------------
cDNetUIThread* cDNetUIThread::CurrentUIThread(void)
{
	return static_cast<cDNetUIThread*>(mCurrentUIDispatcher());
}
//---------------------------------------------------------------------------
iPtr<cDNetUIThread> cDNetUIThread::CreateOnCurrentThread(void)
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
iPtr<cDNetUIThread> cDNetUIThread::StartUIThread(void)
{
	auto Thread=iCreate<cDNetUIThread>();
	rcDNetUIThreadDispatchFrame_StartUIThread(&Thread->fInnerReference,Thread);
	return Thread;
}
//---------------------------------------------------------------------------
iDispatch* cDNetUIThread::GetDispatch(bool HighPriority)
{
	if(HighPriority){
		return &fDispatchHigh;
	}
	else{
		return &fDispatchLow;
	}
}
//---------------------------------------------------------------------------
iUIKeyboard* cDNetUIThread::GetDefaultKeyboard(void)
{
	return fWPFKeyboard;
}
//---------------------------------------------------------------------------
iUIMouse* cDNetUIThread::GetDefaultMouse(void)
{
	return fWPFMouse;
}
//---------------------------------------------------------------------------
rPtr<iAsyncProcedure> cDNetUIThread::DispatchCreateWork(bool HighPriority,iReference *Reference,iProcedure *ThreadProcedure)
{
	if(ThreadProcedure==nullptr)
		return nullptr;
	if(Reference!=nullptr)
		return rCreate<cDNetUIThreadAsyncProcedureRef>(this,HighPriority,Reference,ThreadProcedure);
	else
		return rCreate<cDNetUIThreadAsyncProcedure>(this,HighPriority,ThreadProcedure);
}
//---------------------------------------------------------------------------
rPtr<iAsyncTimer> cDNetUIThread::DispatchCreateTimer(bool HighPriority,iReference *Reference,iProcedure *ThreadProcedure)
{
	auto AsyncTimer=rCreate<cDNetUIThreadAsyncTimer>(this,HighPriority,Reference,ThreadProcedure);
	return AsyncTimer;
}
//---------------------------------------------------------------------------
void cDNetUIThread::DispatchExecute(bool HighPriority,iReference *Reference,iProcedure *Procedure)
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
iPtr<iAsyncTask> cDNetUIThread::DispatchExecuteAsync(bool HighPriority,iReference *Reference,iProcedure *Procedure)
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
cDNetUIApplication::cDNetUIApplication(iPtr<cDNetUIThread> UIThread)
	: fUIThread(cnVar::MoveCast(UIThread))
{
}
//---------------------------------------------------------------------------
cDNetUIApplication::~cDNetUIApplication()
{
}
//---------------------------------------------------------------------------
iUIThread* cDNetUIApplication::GetMainUIThread(void)
{
	return fUIThread;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void cDNetUIApplication::cMainWindowMessageHandler::WindowAttached(void)
{
}
//---------------------------------------------------------------------------
void cDNetUIApplication::cMainWindowMessageHandler::WindowDetached(void)
{
	auto Host=cnMemory::GetObjectFromMemberPointer(this,&cDNetUIApplication::fMainWindowMessageHandler);
	Host->mNotifyStopRun();
}
//---------------------------------------------------------------------------
bool cDNetUIApplication::cMainWindowMessageHandler::WindowMessage(LRESULT &Result,const cWindowMessageParam &MsgParam)
{Result,MsgParam;
	return false;
}
//---------------------------------------------------------------------------
void cDNetUIApplication::cMainWindowMessageHandler::WindowMessageProcessed(LRESULT Result,const cWindowMessageParam &MsgParam)
{Result,MsgParam;
}
//---------------------------------------------------------------------------
void cDNetUIApplication::UIMain(iWindow *MainWindow,int CmdShow)
{
	MainWindow->InsertMessageHandler(&fMainWindowMessageHandler);


	HWND MainWindowHandle=MainWindow->GetWindowHandle();
	::ShowWindow(MainWindowHandle,CmdShow);

	// message loop
	mWPFUIMain();

	MainWindow->RemoveMessageHandler(&fMainWindowMessageHandler);
}
//---------------------------------------------------------------------------
