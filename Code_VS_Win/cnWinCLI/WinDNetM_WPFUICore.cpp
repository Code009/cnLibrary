#include "WinDNetM_WPFUICore.h"


using namespace cnLibrary;
using namespace cnRTL;
using namespace cnWinRTL;
using namespace cnWin;
using namespace DNet;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
rcWPFUICoreProcedureCaller::rcWPFUICoreProcedureCaller(void *CPP)
	: CPP(CPP)
{
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
mcDNetUIThreadAsyncProcedureExecutor::mcDNetUIThreadAsyncProcedureExecutor(bool HighPriority,iProcedure *Procedure)
{
	auto Caller=gcnew rcProcedureCaller(Procedure);
	auto CallerAction=gcnew System::Action(Caller,&rcProcedureCaller::Execute);
	fManaged->Action=System::Runtime::InteropServices::GCHandle::Alloc(CallerAction);
	fManaged->Priority=mcDNetUIThreadDispatcher::GetDispatcherPriority(HighPriority);
}
//---------------------------------------------------------------------------
mcDNetUIThreadAsyncProcedureExecutor::~mcDNetUIThreadAsyncProcedureExecutor(void)
{
	fManaged->Action.Free();
}
//---------------------------------------------------------------------------
void mcDNetUIThreadAsyncProcedureExecutor::mStart(mcDNetUIThreadDispatcher *UIDispatch)
{
	auto Dispatcher=UIDispatch->GetDispatcher();

	auto CallerAction=static_cast<System::Action^>(fManaged->Action.Target);
	Dispatcher->BeginInvoke(CallerAction);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
mcDNetUIThreadAsyncProcedureRefExecutor::mcDNetUIThreadAsyncProcedureRefExecutor(bool HighPriority,iReference *Reference,iProcedure *Procedure)
	: fReference(Reference)
	, fProcedure(Procedure)
{
	auto Caller=gcnew rcWPFUICoreProcedureCaller(this);
	auto CallerAction=gcnew System::Action(Caller,&rcWPFUICoreProcedureCaller::mcDNetUIThreadAsyncProcedureRefExecutor_Execute);
	fManaged->Action=System::Runtime::InteropServices::GCHandle::Alloc(CallerAction);
	fManaged->Priority=mcDNetUIThreadDispatcher::GetDispatcherPriority(HighPriority);
}
//---------------------------------------------------------------------------
mcDNetUIThreadAsyncProcedureRefExecutor::~mcDNetUIThreadAsyncProcedureRefExecutor(void)
{
	fManaged->Action.Free();
}
//---------------------------------------------------------------------------
void rcWPFUICoreProcedureCaller::mcDNetUIThreadAsyncProcedureRefExecutor_Execute(void)
{
	return static_cast<mcDNetUIThreadAsyncProcedureRefExecutor*>(CPP)->CallProc();
}
//---------------------------------------------------------------------------
void mcDNetUIThreadAsyncProcedureRefExecutor::mStart(mcDNetUIThreadDispatcher *Owner)
{
	auto Dispatcher=Owner->GetDispatcher();

	auto CallerAction=static_cast<System::Action^>(fManaged->Action.Target);
	Dispatcher->BeginInvoke(CallerAction);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
mcDNetUIThreadAsyncTimerExecutor::mcDNetUIThreadAsyncTimerExecutor(mcDNetUIThreadDispatcher *UIDispatch,bool HighPriority,iReference *Reference,iProcedure *Procedure)
	: fReference(Reference)
	, fProcedure(Procedure)
{
	auto Dispatcher=UIDispatch->GetDispatcher();
	fManaged->Priority=mcDNetUIThreadDispatcher::GetDispatcherPriority(HighPriority);

	auto Executor=gcnew rcWPFUICoreProcedureCaller(this);
	auto TimerTick=gcnew System::EventHandler(Executor,&rcWPFUICoreProcedureCaller::mcDNetUIThreadAsyncTimerExecutor_TimerTick);
	fTimerTick.Alloc(TimerTick);

	auto Timer=fTimer.Create(fManaged->Priority,Dispatcher);
	Timer->Tick+=TimerTick;

	fFirstHitAction.Alloc(gcnew System::Action(Executor,&rcWPFUICoreProcedureCaller::mcDNetUIThreadAsyncTimerExecutor_FirstHit));
}
//---------------------------------------------------------------------------
mcDNetUIThreadAsyncTimerExecutor::~mcDNetUIThreadAsyncTimerExecutor(void)
{
	auto TimerTick=fTimerTick.Discard();
	auto Timer=fTimer.Discard();
	Timer->Tick-=TimerTick;
	delete Timer;

	fFirstHitAction.Free();
}
//---------------------------------------------------------------------------
void mcDNetUIThreadAsyncTimerExecutor::mStopTimer(void)
{
	auto Timer=fTimer.Get();
	Timer->Stop();
}
//---------------------------------------------------------------------------
void mcDNetUIThreadAsyncTimerExecutor::mHitAndSetTimer(mcDNetUIThreadDispatcher *UIDispatch)
{
	auto Dispatcher=UIDispatch->GetDispatcher();
	Dispatcher->BeginInvoke(fManaged->Priority,fFirstHitAction);
}
//---------------------------------------------------------------------------
void mcDNetUIThreadAsyncTimerExecutor::mScheludeFirstHit(uInt64 Delay)
{
	if(fTimerStarted==false)
		return;

	auto Timer=fTimer.Get();
	Timer->Interval=System::TimeSpan(Delay/100);
	Timer->Start();
}
//---------------------------------------------------------------------------
void rcWPFUICoreProcedureCaller::mcDNetUIThreadAsyncTimerExecutor_TimerTick(System::Object ^,System::EventArgs^)
{
	static_cast<mcDNetUIThreadAsyncTimerExecutor*>(CPP)->TimerTick();
}
//---------------------------------------------------------------------------
void rcWPFUICoreProcedureCaller::mcDNetUIThreadAsyncTimerExecutor_FirstHit(void)
{
	static_cast<mcDNetUIThreadAsyncTimerExecutor*>(CPP)->TimerFirstHit();
}
//---------------------------------------------------------------------------
void mcDNetUIThreadAsyncTimerExecutor::TimerTick(void)
{
	auto Timer=fTimer.Get();
	if(fTimerStarted==false){
		Timer->Stop();
		return;
	}

	if(fTimerSwitchInterval){
		fTimerSwitchInterval=false;

		Timer->Interval=System::TimeSpan(fPeriod/100);

		ExecuteAndState();
	}
	else{
		fProcedure->Execute();
	}
}
//---------------------------------------------------------------------------
void mcDNetUIThreadAsyncTimerExecutor::TimerFirstHit(void)
{
	if(fTimerStarted==false)
		return;

	fTimerSwitchInterval=false;
	auto Timer=fTimer.Get();
	Timer->Interval=System::TimeSpan(fPeriod/100);

	ExecuteAndState();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
mcDNetUIThreadDispatcher::mcDNetUIThreadDispatcher(iReference *OwnerInnerReference)
	: fOnShutdownEvent(gcnew System::EventHandler(gcnew rcWPFUICoreProcedureCaller(this),&rcWPFUICoreProcedureCaller::mcDNetUIThreadDispatchFrame_OnShutdownFinished))
	, fOwnerInnerReference(OwnerInnerReference)
{
}
//---------------------------------------------------------------------------
mcDNetUIThreadDispatcher::~mcDNetUIThreadDispatcher()
{
}
//---------------------------------------------------------------------------
System::Windows::Threading::Dispatcher^ __clrcall mcDNetUIThreadDispatcher::GetDispatcher(void)
{
	return fDispatcher.Get();
}
//---------------------------------------------------------------------------
void mcDNetUIThreadDispatcher::SetupDispatcher(System::Windows::Threading::Dispatcher ^Dispatcher)
{
	rIncReference(fOwnerInnerReference,'disp');
	fDispatcher.Set(Dispatcher);
	Dispatcher->ShutdownFinished+=fOnShutdownEvent;
}
//---------------------------------------------------------------------------
void mcDNetUIThreadDispatcher::ClearDispatcher(void)
{
	auto Dispatcher=fDispatcher.Get();
	if(Dispatcher!=nullptr){
		fDispatcher.Set(nullptr);
		Dispatcher->ShutdownFinished-=fOnShutdownEvent;
	}
	rDecReference(fOwnerInnerReference,'disp');
}
//---------------------------------------------------------------------------
void rcWPFUICoreProcedureCaller::mcDNetUIThreadDispatchFrame_OnShutdownFinished(System::Object ^,System::EventArgs ^)
{
	auto CPPDispatcher=static_cast<mcDNetUIThreadDispatcher*>(CPP);
	if(CPPDispatcher==rcDNetUIThreadContext::gTLSContext.CPPDispatcher){
		rcDNetUIThreadContext::gTLSContext.CPPDispatcher=nullptr;
	}
	//dynamic_cast<System::Windows::Threading::Dispatcher^>(sender)
	CPPDispatcher->WPFOnShutdown();
}
//---------------------------------------------------------------------------
System::Windows::Threading::DispatcherPriority __clrcall mcDNetUIThreadDispatcher::GetDispatcherPriority(bool HighPriority)
{
	if(HighPriority){
		return System::Windows::Threading::DispatcherPriority::Normal;
	}
	else{
		return System::Windows::Threading::DispatcherPriority::Background;
	}
}
//---------------------------------------------------------------------------
bool mcDNetUIThreadDispatcher::mIsCurrent(void)
{
	if(fDispatcherShutdown)
		return true;
	auto Dispatcher=fDispatcher.Get();
	return Dispatcher->CheckAccess();
}
//---------------------------------------------------------------------------
bool mcDNetUIThreadDispatcher::mIsShutdown(void)
{
	return fDispatcherShutdown;
}
//---------------------------------------------------------------------------
mcDNetUIThreadDispatcher* mcDNetUIThreadDispatcher::mCurrentUIDispatcher(void)
{
	return rcDNetUIThreadContext::gTLSContext.CPPDispatcher;
}
//---------------------------------------------------------------------------
void mcDNetUIThreadDispatcher::SetThreadReleaseNotify(System::Action ^Action)
{
	fReleaseNotify.Set(Action);
}
//---------------------------------------------------------------------------
bool mcDNetUIThreadDispatcher::mCreateOnCurrentThread(void)
{
	auto Dispatcher=System::Windows::Threading::Dispatcher::CurrentDispatcher;
	if(Dispatcher==nullptr)
		return false;

	if(Dispatcher->HasShutdownStarted)
		return nullptr;

	SetupDispatcher(Dispatcher);

	rcDNetUIThreadContext::gTLSContext.CPPDispatcher=this;
	return true;
}
//---------------------------------------------------------------------------
void rcWPFUICoreProcedureCaller::mcDNetUIThreadDispatchFrame_ThreadExitProcedure(void)
{
	static_cast<mcDNetUIThreadDispatcher*>(CPP)->ThreadExitProc();
}
//---------------------------------------------------------------------------
void mcDNetUIThreadDispatcher::ThreadExitProc(void)
{
	fDispatchedRelease=false;
	UIThreadRelease();
	rDecReference(fOwnerInnerReference,'shut');
}
//---------------------------------------------------------------------------
void mcDNetUIThreadDispatcher::mShutdownFrame(void)
{
	auto Dispatcher=fDispatcher.Get();
	if(Dispatcher==nullptr){
		// not started
		return;
	}

	if(Dispatcher->CheckAccess()){
		// current thread
		return UIThreadRelease();
	}

	auto AutoLock=TakeLock(&fShutdownLock);
		
	if(fDispatcherShutdown==false){
		rIncReference(fOwnerInnerReference,'shut');

		Dispatcher->BeginInvoke(
			System::Windows::Threading::DispatcherPriority::Normal,
			gcnew System::Action(gcnew rcWPFUICoreProcedureCaller(this),&rcWPFUICoreProcedureCaller::mcDNetUIThreadDispatchFrame_ThreadExitProcedure)
		);
		fDispatchedRelease=true;
	}
}
//---------------------------------------------------------------------------
void mcDNetUIThreadDispatcher::WPFOnShutdown(void)
{
	auto AutoLock=TakeLock(&fShutdownLock);
	fDispatcherShutdown=true;

	for(auto NotifyToken : fFinishNotifySet){
		NotifyToken->NotifyProcedure(NotifyToken,true);
	}
	fFinishNotifySet.RemoveAll();

	ClearDispatcher();
	if(fDispatchedRelease){
		rDecReference(fOwnerInnerReference,'shut');
	}
}
//---------------------------------------------------------------------------
void mcDNetUIThreadDispatcher::UIThreadRelease(void)
{
	auto ReleaseNoitify=fReleaseNotify.Get();
	if(ReleaseNoitify!=nullptr){
		ReleaseNoitify->Invoke();
	}

	rcDNetUIThreadContext::gTLSContext.CPPDispatcher=nullptr;

	ClearDispatcher();
}
//---------------------------------------------------------------------------
void mcDNetUIThreadDispatcher::mAttachDispatcher(cDispatcherFinishNotify *NotifyToken)
{
	auto Dispatcher=fDispatcher.Get();
	auto AutoLock=TakeLock(&fShutdownLock);
	if(Dispatcher->HasShutdownFinished){
		NotifyToken->NotifyProcedure(NotifyToken,true);
		return;
	}
	fFinishNotifySet.Insert(NotifyToken);
}
//---------------------------------------------------------------------------
bool mcDNetUIThreadDispatcher::mDetachDispatcher(cDispatcherFinishNotify *NotifyToken)
{
	auto Dispatcher=fDispatcher.Get();
	if(Dispatcher->CheckAccess()){
		// this Thread
		fFinishNotifySet.Remove(NotifyToken);
		if(fDispatcherShutdown){
			return false;
		}
		NotifyToken->NotifyProcedure(NotifyToken,false);
		return true;
	}
	else{
		auto AutoLock=TakeLock(&fShutdownLock);
		
		if(fDispatcherShutdown==false){
			auto Params=gcnew array<System::IntPtr>(2);
			Params[0]=System::IntPtr(this);
			Params[1]=System::IntPtr(NotifyToken);
			Dispatcher->BeginInvoke(System::Windows::Threading::DispatcherPriority::Normal,gcnew System::Action<System::IntPtr,System::IntPtr>(FinishNotifyRelease),Params);
			return true;
		}
		else{
			fFinishNotifySet.Remove(NotifyToken);
			return false;
		}
	}
}
//---------------------------------------------------------------------------
void __clrcall mcDNetUIThreadDispatcher::FinishNotifyRelease(System::IntPtr This,System::IntPtr FinishNotify)
{
	static_cast<mcDNetUIThreadDispatcher*>(This.ToPointer())->NotifyFinish(static_cast<cDispatcherFinishNotify*>(FinishNotify.ToPointer()));
}
//---------------------------------------------------------------------------
void __clrcall mcDNetUIThreadDispatcher::NotifyFinish(cDispatcherFinishNotify *FinishNotify)
{
	fFinishNotifySet.Remove(FinishNotify);
	if(fDispatcherShutdown){
		// already shutdown
		return;
	}

	FinishNotify->NotifyProcedure(FinishNotify,false);
}
//---------------------------------------------------------------------------
void mcDNetUIThreadDispatcher::mDispatchExecute(bool HighPriority,iProcedure *Procedure)
{
	auto Dispatcher=fDispatcher.Get();
	auto dp=GetDispatcherPriority(HighPriority);
	auto ProcExecute=gcnew rcProcedureCaller(Procedure);
	Dispatcher->BeginInvoke(dp,gcnew System::Action(ProcExecute,&rcProcedureCaller::Execute));
}
//---------------------------------------------------------------------------
void mcDNetUIThreadDispatcher::mDispatchExecuteSync(bool HighPriority,iProcedure *Procedure)
{
	auto Dispatcher=fDispatcher.Get();
	auto dp=GetDispatcherPriority(HighPriority);
	auto ProcExecute=gcnew rcProcedureCaller(Procedure);
	Dispatcher->Invoke(dp,gcnew System::Action(ProcExecute,&rcProcedureCaller::Execute));
}
//---------------------------------------------------------------------------
void mcDNetUIThreadDispatcher::mDispatchExecuteReferenced(bool HighPriority,iReference *Reference,iProcedure *Procedure)
{
	auto Dispatcher=fDispatcher.Get();
	auto dp=GetDispatcherPriority(HighPriority);
	auto ProcExecute=gcnew rcRefProcedureCaller(Reference,Procedure);
	Dispatcher->BeginInvoke(dp,gcnew System::Action(ProcExecute,&rcRefProcedureCaller::Execute));
}
//---------------------------------------------------------------------------
void rcWPFUICoreProcedureCaller::mcDispatchExecuteAsyncTask_Execute(void)
{
	static_cast<mcDNetUIThreadDispatcher::mcDispatchExecuteAsyncTask*>(CPP)->ExecuteTask();
}
//---------------------------------------------------------------------------
void mcDNetUIThreadDispatcher::mDispatchExecuteAsync(mcDispatchExecuteAsyncTask *Task,bool HighPriority)
{
	auto Dispatcher=fDispatcher.Get();
	auto dp=mcDNetUIThreadDispatcher::GetDispatcherPriority(HighPriority);

	auto Executor=gcnew rcWPFUICoreProcedureCaller(Task);
	auto dproc=gcnew System::Action(Executor,&rcWPFUICoreProcedureCaller::mcDispatchExecuteAsyncTask_Execute);
	Dispatcher->BeginInvoke(dp,dproc);
}
//---------------------------------------------------------------------------
bool mcDNetUIThreadDispatcher::mKeyboardEventIsKeyDown(eKeyCode KeyCode)
{
	auto Key=System::Windows::Input::KeyInterop::KeyFromVirtualKey(static_cast<int>(KeyCode));
	return System::Windows::Input::Keyboard::IsKeyDown(Key);
}
//---------------------------------------------------------------------------
bool mcDNetUIThreadDispatcher::mKeyboardEventIsKeyToggled(eKeyCode KeyCode)
{
	auto Key=System::Windows::Input::KeyInterop::KeyFromVirtualKey(static_cast<int>(KeyCode));
	return System::Windows::Input::Keyboard::IsKeyToggled(Key);
}
//---------------------------------------------------------------------------
bool mcDNetUIThreadDispatcher::mMouseGetPosition(cGCRef &UIViewVisualHandle,cUIPoint &Position)
{
	System::Object^ RelativeObject=UIViewVisualHandle;
	auto ViewVisual=dynamic_cast<IWPFUIViewVisual^>(RelativeObject);
	if(ViewVisual!=nullptr){
		return mMouseGetPosition(ViewVisual,Position);
	}
	auto Visual=dynamic_cast<System::Windows::Media::Visual^>(RelativeObject);
	if(Visual!=nullptr){
		return mMouseGetPosition(Visual,Position);
	}
	return false;
}
//---------------------------------------------------------------------------
bool mcDNetUIThreadDispatcher::mMouseGetPosition(System::Windows::Media::Visual ^Visual,cUIPoint &Position)
{
	auto RelativeElement=dynamic_cast<System::Windows::IInputElement^>(Visual);
	if(RelativeElement==nullptr)
		return false;

	auto MousePos=System::Windows::Input::Mouse::GetPosition(RelativeElement);
	Position.x=static_cast<Float32>(MousePos.X);
	Position.y=static_cast<Float32>(MousePos.Y);
	return true;
}
//---------------------------------------------------------------------------
bool mcDNetUIThreadDispatcher::mMouseGetPosition(IWPFUIViewVisual ^ViewVisual,cUIPoint &Position)
{
	auto RelativeElement=dynamic_cast<System::Windows::IInputElement^>(ViewVisual->Visual);
	if(RelativeElement==nullptr)
		return false;
	Float32 LayoutScale=ViewVisual->LayoutScale;

	auto MousePos=System::Windows::Input::Mouse::GetPosition(RelativeElement);
	Position.x=static_cast<Float32>(MousePos.X*LayoutScale);
	Position.y=static_cast<Float32>(MousePos.Y*LayoutScale);
	return true;
}
//---------------------------------------------------------------------------
bool mcDNetUIThreadDispatcher::mMouseEventIsButtonDown(eMouseButton Button)
{
	System::Windows::Input::MouseButtonState BtnState;
	switch(Button){
	default:
		return false;
	case MouseButton::Left:
		BtnState=System::Windows::Input::Mouse::LeftButton;
		break;
	case MouseButton::Right:
		BtnState=System::Windows::Input::Mouse::RightButton;
		break;
	case MouseButton::Middle:
		BtnState=System::Windows::Input::Mouse::MiddleButton;
		break;
	}
	if(BtnState==System::Windows::Input::MouseButtonState::Pressed){
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
rcDNetUIThreadDispatchFrame::rcDNetUIThreadDispatchFrame(iReference *DispatcherReference,mcDNetUIThreadDispatcher *Dispatcher,System::Threading::ManualResetEvent ^DoneEvent)
	: fDispatcher(Dispatcher)
	, fDispatcherReference(DispatcherReference)
	, fDoneEvent(DoneEvent)
{
}
//---------------------------------------------------------------------------
rcDNetUIThreadDispatchFrame::~rcDNetUIThreadDispatchFrame()
{
}
//---------------------------------------------------------------------------
void rcDNetUIThreadDispatchFrame::ThreadProcedure(void)
{
	//rInnerPtr<mcDNetUIThread> UIThread;
	auto ReleaseNotification=gcnew System::Action(this,&rcDNetUIThreadDispatchFrame::UIThreadExit);
	fDispatcher->SetThreadReleaseNotify(ReleaseNotification);
	if(fDispatcher->mCreateOnCurrentThread()==false){
		fDoneEvent->Set();
		fDoneEvent=nullptr;
		fDispatcher->SetThreadReleaseNotify(nullptr);
		return;
	}

	fDispatcherReference->IncreaseReference();

	fDoneEvent->Set();
	fDoneEvent=nullptr;

	auto Dispatcher=fDispatcher->GetDispatcher();
	Dispatcher->PushFrame(%fDispatchFrame);

	fDispatcher->SetThreadReleaseNotify(nullptr);

	fDispatcherReference->DecreaseReference();
}
//---------------------------------------------------------------------------
void rcDNetUIThreadDispatchFrame::UIThreadExit(void)
{
	fDispatchFrame.Continue=false;
}
//---------------------------------------------------------------------------
void cnWin::rcDNetUIThreadDispatchFrame_StartUIThread(iReference *DispatcherReference,mcDNetUIThreadDispatcher *Dispatcher)
{
	System::Threading::ManualResetEvent DoneEvent(false);
	auto ThreadProcedure=gcnew rcDNetUIThreadDispatchFrame(DispatcherReference,Dispatcher,%DoneEvent);

	auto UIThread=gcnew System::Threading::Thread(gcnew System::Threading::ThreadStart(ThreadProcedure,&rcDNetUIThreadDispatchFrame::ThreadProcedure));
	UIThread->SetApartmentState(System::Threading::ApartmentState::STA);
	UIThread->Start();

	// wait for thread procedure
	DoneEvent.WaitOne();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
mcDNetUIApplicationDispatchFrame::mcDNetUIApplicationDispatchFrame()
	: fDispatchFrameRef(gcnew System::Windows::Threading::DispatcherFrame())
{
}
//---------------------------------------------------------------------------
mcDNetUIApplicationDispatchFrame::~mcDNetUIApplicationDispatchFrame(void)
{
}
//---------------------------------------------------------------------------
void mcDNetUIApplicationDispatchFrame::mNotifyStopRun(void)
{
	auto DispatchFrame=fDispatchFrameRef.Get();
	DispatchFrame->Continue=false;
}
//---------------------------------------------------------------------------
void mcDNetUIApplicationDispatchFrame::mWPFUIMain(void)
{
	auto DispatchFrame=fDispatchFrameRef.Get();
	DispatchFrame->Continue=true;
	System::Windows::Threading::Dispatcher::PushFrame(DispatchFrame);
}
//---------------------------------------------------------------------------
