#include "WinCLIM_WPFUICore.h"

using namespace cnLibrary;
//using namespace cnRTL;
//using namespace cnWinRTL;
using namespace cnWin;
//using namespace DNet;

static_assert(TManagedStructCheck<mcWPFUIThreadAsyncTimerExecutor>::Value,"managed class has an incorrect ManagedSize");

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
rcNativeCaller_WPFUICore::rcNativeCaller_WPFUICore(void *CPP)
	: CPP(CPP)
{
}
//---------------------------------------------------------------------------
bool mcWPFInput::mKeyboardEventIsKeyDown(eKeyCode KeyCode)noexcept(true)
{
	auto Key=System::Windows::Input::KeyInterop::KeyFromVirtualKey(static_cast<int>(KeyCode));
	return System::Windows::Input::Keyboard::IsKeyDown(Key);
}
//---------------------------------------------------------------------------
bool mcWPFInput::mKeyboardEventIsKeyToggled(eKeyCode KeyCode)noexcept(true)
{
	auto Key=System::Windows::Input::KeyInterop::KeyFromVirtualKey(static_cast<int>(KeyCode));
	return System::Windows::Input::Keyboard::IsKeyToggled(Key);
}
//---------------------------------------------------------------------------
bool mcWPFInput::mMouseGetPosition(const cGCHandle &UIViewVisualHandle,cUIPoint &Position)noexcept(true)
{
	auto Visual=UIViewVisualHandle.DynamicCast<System::Windows::Media::Visual>();
	if(Visual!=nullptr){
		return MouseGetPosition(Visual,Position);
	}
	return false;
}
//---------------------------------------------------------------------------
bool mcWPFInput::MouseGetPosition(System::Windows::Media::Visual ^Visual,cUIPoint &Position)noexcept(true)
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
bool mcWPFInput::mMouseEventIsButtonDown(eMouseButton Button)noexcept(true)
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
void mcWPFUIThreadAsyncTimerExecutor::mSetup(mbcWPFUIThread *UIDispatch,bool HighPriority)noexcept(true)
{
	fPriority=rcWPFUIThread::ToDispatcherPriority(HighPriority);

	auto Caller=gcnew rcNativeCaller_WPFUICore(this);
	auto TimerTick=gcnew System::EventHandler(Caller,&rcNativeCaller_WPFUICore::mcWPFUIThreadAsyncTimerExecutor_TimerTick);
	fTimerTickEventHandler.Alloc(TimerTick);

	fScheduleAction.Alloc(gcnew System::Action(Caller,&rcNativeCaller_WPFUICore::mcWPFUIThreadAsyncTimerExecutor_Schedule));
	fHitAction.Alloc(gcnew System::Action(Caller,&rcNativeCaller_WPFUICore::mcWPFUIThreadAsyncTimerExecutor_Hit));
	fCleanupAction.Alloc(gcnew System::Action(Caller,&rcNativeCaller_WPFUICore::mcWPFUIThreadAsyncTimerExecutor_Cleanup));

	auto WPFThread=UIDispatch->GetWPFThread();
	auto Timer=gcnew System::Windows::Threading::DispatcherTimer(fPriority,WPFThread->Dispatcher);
	fTimer.Alloc(Timer);
	Timer->Tick+=TimerTick;

}
//---------------------------------------------------------------------------
void mcWPFUIThreadAsyncTimerExecutor::mClear(void)noexcept(true)
{
	fScheduleAction.Free();
	fHitAction.Free();
	fCleanupAction.Free();

	auto TimerTick=fTimerTickEventHandler.Get();
	fTimerTickEventHandler.Free();
	auto Timer=fTimer.Get();
	fTimer.Free();
	Timer->Tick-=TimerTick;
	delete Timer;

}
//---------------------------------------------------------------------------
void rcNativeCaller_WPFUICore::mcWPFUIThreadAsyncTimerExecutor_TimerTick(System::Object ^,System::EventArgs^)
{
	static_cast<mcWPFUIThreadAsyncTimerExecutor*>(CPP)->TimerTick();
}
//---------------------------------------------------------------------------
void rcNativeCaller_WPFUICore::mcWPFUIThreadAsyncTimerExecutor_Schedule(void)
{
	static_cast<mcWPFUIThreadAsyncTimerExecutor*>(CPP)->Schedule();
}
//---------------------------------------------------------------------------
void rcNativeCaller_WPFUICore::mcWPFUIThreadAsyncTimerExecutor_Hit(void)
{
	static_cast<mcWPFUIThreadAsyncTimerExecutor*>(CPP)->TimerTick();
}
//---------------------------------------------------------------------------
void rcNativeCaller_WPFUICore::mcWPFUIThreadAsyncTimerExecutor_Cleanup(void)
{
	static_cast<mcWPFUIThreadAsyncTimerExecutor*>(CPP)->TimerCleanup();
}
//---------------------------------------------------------------------------
void mcWPFUIThreadAsyncTimerExecutor::mSchedule(void)noexcept(true)
{
	auto Timer=fTimer.Get();
	Timer->Dispatcher->BeginInvoke(fScheduleAction.Get());
}
//---------------------------------------------------------------------------
void mcWPFUIThreadAsyncTimerExecutor::mHit(void)noexcept(true)
{
	auto Timer=fTimer.Get();
	Timer->Dispatcher->BeginInvoke(fHitAction.Get());
}
//---------------------------------------------------------------------------
void mcWPFUIThreadAsyncTimerExecutor::mStart(uInt64 Interval)noexcept(true)
{
	auto Timer=fTimer.Get();
	Timer->Interval=System::TimeSpan(static_cast<long long>(Interval/100));
	Timer->Start();
}
//---------------------------------------------------------------------------
void mcWPFUIThreadAsyncTimerExecutor::mStop(void)noexcept(true)
{
	auto Timer=fTimer.Get();
	Timer->Stop();
}
//---------------------------------------------------------------------------
void mcWPFUIThreadAsyncTimerExecutor::mStopCleanup(void)noexcept(true)
{
	auto Timer=fTimer.Get();
	Timer->Stop();
	Timer->Dispatcher->BeginInvoke(fCleanupAction.Get());
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
rcWPFUIThread::rcWPFUIThread(System::Windows::Threading::Dispatcher ^Dispatcher,rcWPFUIThreadDispatcherProcedure ^Procedure,mbcWPFUIThread *CPP)
	: fDispatcher(Dispatcher)
	, fProcedure(Procedure)
	, fCPP(CPP)
{
	fOnShutdownEvent=gcnew System::EventHandler(this,&rcWPFUIThread::OnShutdownFinished);
	fDispatcher->ShutdownFinished+=fOnShutdownEvent;

	gTLS->Value=this;
}
//---------------------------------------------------------------------------
void rcWPFUIThread::CPPDispose(void)
{
	fDispatcher->ShutdownFinished-=fOnShutdownEvent;
	fOnShutdownEvent=nullptr;

	if(gTLS->IsValueCreated){
		if(gTLS->Value==this)
			gTLS->Value=nullptr;
	}

	if(fFallbackDispatcher!=nullptr){
		fFallbackDispatcher=nullptr;
		// close fallback dispatch
		ClearFallbackDispatcher();
	}

	fCPP=nullptr;
}
//---------------------------------------------------------------------------
System::Windows::Threading::DispatcherPriority rcWPFUIThread::ToDispatcherPriority(bool HighPriority)
{
	if(HighPriority){
		return System::Windows::Threading::DispatcherPriority::Normal;
	}
	else{
		return System::Windows::Threading::DispatcherPriority::Background;
	}
}
//---------------------------------------------------------------------------
System::Windows::Threading::Dispatcher ^rcWPFUIThread::Dispatcher::get()
{
	return fDispatcher;
}
//---------------------------------------------------------------------------
void rcWPFUIThread::OnShutdownFinished(System::Object ^,System::EventArgs ^)
{
	if(fCPP!=nullptr)
		fCPP->WPFOnShutdown();
}
//---------------------------------------------------------------------------
void rcWPFUIThread::CPPCallClose(void)
{
	fCPP->WPFOnCPPClose();
}
//---------------------------------------------------------------------------
System::Windows::Threading::Dispatcher^ rcWPFUIThread::QueryFallbackDispatcher(void)
{
	if(fFallbackDispatcher==nullptr)
		fFallbackDispatcher=SetupFallbackDispatcher();

	return fFallbackDispatcher;
}
//---------------------------------------------------------------------------
void rcWPFUIThread::FallbackDispatcherProcedure(void)
{
	System::Windows::Threading::DispatcherFrame FallbackDispatchFrame;
	auto FallbackDispatcher=FallbackDispatchFrame.Dispatcher;
	bool LockTaken=false;
	try{
		gFallbackLock.Enter(LockTaken);

		gFallbackDispatcher=FallbackDispatcher;

	}
	finally{
		if(LockTaken)
			gFallbackLock.Exit();
	}
	gFallbackCreateCompleteEvent->Set();
	FallbackDispatcher->PushFrame(%FallbackDispatchFrame);
}
//---------------------------------------------------------------------------
System::Windows::Threading::Dispatcher^ rcWPFUIThread::SetupFallbackDispatcher(void)
{
	if(System::Threading::Interlocked::Increment(gFallbackDispatcherRefCount)==1){
		auto Thread=gcnew System::Threading::Thread(gcnew System::Threading::ThreadStart(&FallbackDispatcherProcedure));
		Thread->SetApartmentState(System::Threading::ApartmentState::STA);
		Thread->Start();
	}
	while(gFallbackDispatcher==nullptr){
		gFallbackCreateCompleteEvent->WaitOne();
	}
	gFallbackCreateCompleteEvent->Reset();
	return gFallbackDispatcher;
}
//---------------------------------------------------------------------------
void rcWPFUIThread::ClearFallbackDispatcher(void)
{
	if(System::Threading::Interlocked::Decrement(gFallbackDispatcherRefCount)==0){
		System::Windows::Threading::Dispatcher ^RemovedDispatcher;
		bool LockTaken=false;
		try{
			gFallbackLock.Enter(LockTaken);

			RemovedDispatcher=gFallbackDispatcher;
			gFallbackDispatcher=nullptr;

		}
		finally{
			if(LockTaken)
				gFallbackLock.Exit();
		}

		RemovedDispatcher->BeginInvokeShutdown(System::Windows::Threading::DispatcherPriority::Normal);
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void mbcWPFUIThread::mSetup(mcWPFDispatcherReference &DispatcherReference)noexcept(true)
{
	auto WPFThread=gcnew rcWPFUIThread(DispatcherReference.Dispatcher,DispatcherReference.Procedure,this);
	fUIThreadHandle.Alloc(WPFThread);
}
//---------------------------------------------------------------------------
void mbcWPFUIThread::mClear(void)noexcept(true)
{
	auto WPFThread=fUIThreadHandle.Get();
	WPFThread->CPPDispose();
	fUIThreadHandle.Free();
}
//---------------------------------------------------------------------------
void mbcWPFUIThread::mCPPClose(void)noexcept(true)
{
	auto WPFThread=fUIThreadHandle.Get();
	auto Dispatcher=WPFThread->Dispatcher;

	if(WPFThread->fProcedure!=nullptr){
		// exit ui thread if backed
		WPFThread->fProcedure->NotifyExit();
	}

	if(Dispatcher->CheckAccess()){
		WPFOnCPPClose();
	}
	else{
		ExecuteDelegate(System::Windows::Threading::DispatcherPriority::Normal,
			gcnew System::Action(WPFThread,&rcWPFUIThread::CPPCallClose)
		);
	}
}
//---------------------------------------------------------------------------
rcWPFUIThread^ mbcWPFUIThread::GetWPFThread(void)const noexcept(true)
{
	return fUIThreadHandle.Get();
}
//---------------------------------------------------------------------------
bool mbcWPFUIThread::IsShutdown(void)const noexcept(true)
{
	auto WPFThread=fUIThreadHandle.Get();
	return WPFThread->Dispatcher->HasShutdownStarted;
}
//---------------------------------------------------------------------------
bool mbcWPFUIThread::mIsCurrent(void)const noexcept(true)
{
	auto WPFThread=fUIThreadHandle.Get();
	if(WPFThread->Dispatcher==nullptr)
		return false;
	return WPFThread->Dispatcher->CheckAccess();
}
//---------------------------------------------------------------------------
mbcWPFUIThread* mbcWPFUIThread::mCurrentUIThread(void)noexcept(true)
{
	if(rcWPFUIThread::gTLS->IsValueCreated){
		auto WPFUIThread=rcWPFUIThread::gTLS->Value;
		if(WPFUIThread!=nullptr)
			return WPFUIThread->fCPP;
	}
	return nullptr;
}
//---------------------------------------------------------------------------
mbcWPFUIThread* mbcWPFUIThread::mCurrentUIThread(iFunction<void (mcWPFDispatcherReference&)noexcept(true)> *MakeObject)noexcept(true)
{
	if(rcWPFUIThread::gTLS->IsValueCreated){
		auto WPFUIThread=rcWPFUIThread::gTLS->Value;
		if(WPFUIThread!=nullptr)
			return WPFUIThread->fCPP;
	}

	auto Dispatcher=System::Windows::Threading::Dispatcher::CurrentDispatcher;
	if(Dispatcher==nullptr)
		return nullptr;

	if(Dispatcher->HasShutdownStarted)
		return nullptr;

	rcWPFUIThreadDispatcherProcedure ^NullProcedure=nullptr;
	mcWPFDispatcherReference DispatcherReference={Dispatcher,NullProcedure};
	MakeObject->Execute(DispatcherReference);
	return nullptr;
}
//---------------------------------------------------------------------------
void rcWPFUIThread::rcDelegateExecutor::Aborted(void)
{
	if(System::Threading::Interlocked::Exchange(AbortProcessFlag,1)!=0)
		return;	// other thread is processing

	// reroute execution to backup thread
	auto FallbackDispatcher=Thread->QueryFallbackDispatcher();
	FallbackDispatcher->BeginInvoke(Priority,Procedure);
}
//---------------------------------------------------------------------------
void rcWPFUIThread::rcDelegateExecutor::OnAborted(System::Object^,System::EventArgs^)
{
	Aborted();
}
//---------------------------------------------------------------------------
void mbcWPFUIThread::ExecuteDelegate(System::Windows::Threading::DispatcherPriority Priority,System::Action ^Procedure)noexcept(true)
{
	auto WPFThread=fUIThreadHandle.Get();
	auto Executor=gcnew rcWPFUIThread::rcDelegateExecutor();
	Executor->Thread=WPFThread;
	Executor->Procedure=Procedure;
	Executor->Priority=Priority;
	auto Operation=WPFThread->Dispatcher->BeginInvoke(Priority,Procedure);
	Operation->Aborted+=gcnew System::EventHandler(Executor,&rcWPFUIThread::rcDelegateExecutor::OnAborted);

	auto CurrentStatus=Operation->Wait(System::TimeSpan(0));
	if(CurrentStatus==System::Windows::Threading::DispatcherOperationStatus::Aborted){
		// already aborted
		Executor->Aborted();
	}
}
//---------------------------------------------------------------------------
void mbcWPFUIThread::mExecuteDelegate(bool HighPriority,mbcActionDelegate &Delegate)noexcept(true)
{
	auto dp=rcWPFUIThread::ToDispatcherPriority(HighPriority);
	return ExecuteDelegate(dp,Delegate.GetAction());
}
//---------------------------------------------------------------------------
void mbcWPFUIThread::mDispatchExecute(bool HighPriority,iProcedure *Procedure)noexcept(true)
{
	//auto WPFThread=fUIThreadHandle.Get();
	auto dp=rcWPFUIThread::ToDispatcherPriority(HighPriority);
	auto Caller=gcnew rcNativeCaller_CLICommon(Procedure);
	return ExecuteDelegate(dp,gcnew System::Action(Caller,&rcNativeCaller_CLICommon::iProcedure_Execute));
}
//---------------------------------------------------------------------------
System::Object^ rcNativeCaller_WPFUICore::iProcedure_ExecuteSync(void)
{
	static_cast<iProcedure*>(CPP)->Execute();
	return rcWPFUIThread::gTLS;
}
//---------------------------------------------------------------------------
void rcNativeCaller_WPFUICore::iProcedure_ExecuteFallback(void)
{
	static_cast<iProcedure*>(CPP)->Execute();
}
//---------------------------------------------------------------------------
void mbcWPFUIThread::mDispatchExecuteSync(bool HighPriority,iProcedure *Procedure)noexcept(true)
{
	auto WPFThread=fUIThreadHandle.Get();
	auto dp=rcWPFUIThread::ToDispatcherPriority(HighPriority);
	auto Caller=gcnew rcNativeCaller_WPFUICore(Procedure);
	auto RetObject=WPFThread->Dispatcher->Invoke(dp,gcnew System::Func<System::Object^>(Caller,&rcNativeCaller_WPFUICore::iProcedure_ExecuteSync));
	if(RetObject==nullptr){
		// the procedure was cancelled and not executed, reroute to fallback thread
		auto FallbackDispatcher=WPFThread->QueryFallbackDispatcher();
		FallbackDispatcher->Invoke(dp,gcnew System::Action(Caller,&rcNativeCaller_WPFUICore::iProcedure_ExecuteFallback));
	}
}
//---------------------------------------------------------------------------
void rcWPFUIThread::rcReferencedProcedureExecutor::Execute(void)
{
	CPP->nExecuteProcedureReference(Reference,Procedure);
}
//---------------------------------------------------------------------------
void mbcWPFUIThread::mDispatchExecuteReferenced(bool HighPriority,iReference *Reference,iProcedure *Procedure)noexcept(true)
{
	//auto WPFThread=fUIThreadHandle.Get();
	auto dp=rcWPFUIThread::ToDispatcherPriority(HighPriority);
	auto Executor=gcnew rcWPFUIThread::rcReferencedProcedureExecutor();
	Executor->CPP=this;
	Executor->Reference=Reference;
	Executor->Procedure=Procedure;
	ExecuteDelegate(dp,gcnew System::Action(Executor,&rcWPFUIThread::rcReferencedProcedureExecutor::Execute));
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
rcWPFUIThreadDispatcherProcedure::rcWPFUIThreadDispatcherProcedure(iFunction<void (mcWPFDispatcherReference*)noexcept(true)> *Callback)
	: fCallback(Callback)
{
}
//---------------------------------------------------------------------------
void rcWPFUIThreadDispatcherProcedure::ThreadProcedure(void)
{
	auto Dispatcher=System::Windows::Threading::Dispatcher::CurrentDispatcher;
	if(Dispatcher==nullptr || Dispatcher->HasShutdownStarted){
		fCallback->Execute(nullptr);
		return;
	}
	{
		mcWPFDispatcherReference DispatcherReference={Dispatcher,this};
		fCallback->Execute(&DispatcherReference);
	}

	Dispatcher->PushFrame(%fDispatchFrame);
}
//---------------------------------------------------------------------------
void rcWPFUIThreadDispatcherProcedure::NotifyExit(void)
{
	fDispatchFrame.Continue=false;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void cnWin::WPFUIThreadNewThread(iFunction<void (mcWPFDispatcherReference*)noexcept(true)> *Callback)noexcept(true)
{
	auto ThreadProcedure=gcnew rcWPFUIThreadDispatcherProcedure(Callback);
	auto Thread=gcnew System::Threading::Thread(gcnew System::Threading::ThreadStart(ThreadProcedure,&rcWPFUIThreadDispatcherProcedure::ThreadProcedure));
	Thread->SetApartmentState(System::Threading::ApartmentState::STA);
	Thread->Start();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void mcWPFDispatchFrame::mSetup(void)noexcept(true)
{
	fDispatchFrameRef.Alloc(gcnew System::Windows::Threading::DispatcherFrame());
}
//---------------------------------------------------------------------------
void mcWPFDispatchFrame::mClear(void)noexcept(true)
{
	fDispatchFrameRef.Free();
}
//---------------------------------------------------------------------------
void mcWPFDispatchFrame::mNotifyStopRun(void)noexcept(true)
{
	auto DispatchFrame=fDispatchFrameRef.Get();
	DispatchFrame->Continue=false;
}
//---------------------------------------------------------------------------
void mcWPFDispatchFrame::mUIMain(void)noexcept(true)
{
	auto DispatchFrame=fDispatchFrameRef.Get();
	DispatchFrame->Continue=true;
	System::Windows::Threading::Dispatcher::PushFrame(DispatchFrame);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void mcWPFKeyEventArgs::mCancelEvent(void)noexcept(true)
{
	EventArgs->Handled=true;
}
//---------------------------------------------------------------------------
bool mcWPFKeyEventArgs::mIsCancelled(void)noexcept(true)
{
	return EventArgs->Handled;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void mcWPFMouseEventArgs::mCancelEvent(void)noexcept(true)
{
	EventArgs->Handled=true;
}
//---------------------------------------------------------------------------
bool mcWPFMouseEventArgs::mIsCancelled(void)noexcept(true)
{
	return EventArgs->Handled;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void* mcWPFTouchEventArgs::mGetTouchID(void)noexcept(true)
{
	return reinterpret_cast<void*>(static_cast<uIntn>(EventArgs->TouchDevice->Id));
}
//---------------------------------------------------------------------------
void mcWPFTouchEventArgs::mCancelEvent(void)noexcept(true)
{
	EventArgs->Handled=true;
}
//---------------------------------------------------------------------------
bool mcWPFTouchEventArgs::mIsCancelled(void)noexcept(true)
{
	return EventArgs->Handled;
}
//---------------------------------------------------------------------------
bool mcWPFTouchEventArgs::mGetPosition(const cGCHandle &UIElementHandle,cUIPoint &Position)noexcept(true)
{
	System::Object^ RelativeObject=UIElementHandle;
	auto Visual=dynamic_cast<System::Windows::Media::Visual^>(RelativeObject);
	if(Visual!=nullptr){
		return mGetPosition(Visual,Position);
	}
	return false;
}
//---------------------------------------------------------------------------
bool mcWPFTouchEventArgs::mGetPosition(System::Windows::Media::Visual ^Visual,cUIPoint &Position)noexcept(true)
{
	auto RelativeElement=dynamic_cast<System::Windows::IInputElement^>(Visual);
	if(RelativeElement==nullptr)
		return false;

	auto TouchPos=EventArgs->TouchDevice->GetTouchPoint(RelativeElement);
	Position.x=static_cast<Float32>(TouchPos->Position.X);
	Position.y=static_cast<Float32>(TouchPos->Position.Y);
	return true;
}
//---------------------------------------------------------------------------
