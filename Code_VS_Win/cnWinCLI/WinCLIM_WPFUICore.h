/*- cnWinCLI - Managed - WPF UICore ---------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2020-04-14                                          */
/*-------------------------------------------------------------------------*/
#pragma once
/*-------------------------------------------------------------------------*/
#if _MANAGED
#using <WindowsBase.dll>
#using <PresentationCore.dll>
#endif	/* _MANAGED */

#include <cnSystem\cnUI.h>
#include <cnWinCLI\WinCLIM_Common.h>

#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnWin{
//---------------------------------------------------------------------------
#if _MANAGED
//---------------------------------------------------------------------------
private ref class rcNativeCaller_WPFUICore sealed
{
public:
	rcNativeCaller_WPFUICore(void *CPP);

	void iProcedure_ExecuteFallback(void);
	System::Object^ iProcedure_ExecuteSync(void);

	void mcWPFUIThreadAsyncTimerExecutor_TimerTick(System::Object ^,System::EventArgs ^e);
	void mcWPFUIThreadAsyncTimerExecutor_Schedule(void);
	void mcWPFUIThreadAsyncTimerExecutor_Hit(void);
	void mcWPFUIThreadAsyncTimerExecutor_Cleanup(void);
private:
	void *CPP;
};
//---------------------------------------------------------------------------
#endif	// _MANAGED

//---------------------------------------------------------------------------
struct mcWPFInput
{
	static bool mKeyboardEventIsKeyDown(eKeyCode KeyCode)noexcept(true);
	static bool mKeyboardEventIsKeyToggled(eKeyCode KeyCode)noexcept(true);

	static bool mMouseGetPosition(const cGCHandle &UIViewVisualHandle,cUIPoint &Position)noexcept(true);
	static bool mMouseEventIsButtonDown(eMouseButton Button)noexcept(true);

#if _MANAGED
	static bool __clrcall MouseGetPosition(System::Windows::Media::Visual ^Visual,cUIPoint &Position)noexcept(true);
#endif
};
//---------------------------------------------------------------------------
class mbcWPFUIThread;
//---------------------------------------------------------------------------
class mcWPFUIThreadAsyncTimerExecutor
{
public:
	static constexpr uIntn ManagedSize=sizeof(void*)+sizeof(cGCHandle)*5+sizeof(void*);

	void mSetup(mbcWPFUIThread *UIDispatch,bool HighPriority)noexcept(true);
	void mClear(void)noexcept(true);

	void mSchedule(void)noexcept(true);
	void mHit(void)noexcept(true);
	void mStart(uInt64 Interval)noexcept(true);
	void mStop(void)noexcept(true);
	void mStopCleanup(void)noexcept(true);
protected:

#if _MANAGED
	mcGCHandle<System::Windows::Threading::DispatcherTimer,eGCHandleType::Normal> fTimer;
	mcGCHandle<System::EventHandler,eGCHandleType::Normal> fTimerTickEventHandler;
	mcGCHandle<System::Action,eGCHandleType::Normal> fScheduleAction;
	mcGCHandle<System::Action,eGCHandleType::Normal> fHitAction;
	mcGCHandle<System::Action,eGCHandleType::Normal> fCleanupAction;
	System::Windows::Threading::DispatcherPriority fPriority;

	friend rcNativeCaller_WPFUICore;
#endif // _MANAGED

	virtual void Schedule(void)noexcept(true)=0;
	virtual void TimerTick(void)noexcept(true)=0;
	virtual void TimerCleanup(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
#if _MANAGED
//---------------------------------------------------------------------------
struct mcWPFDispatcherReference;
//---------------------------------------------------------------------------
private ref class rcWPFUIThreadDispatcherProcedure sealed
{
public:
	rcWPFUIThreadDispatcherProcedure(iFunction<void (mcWPFDispatcherReference*)noexcept(true)> *Callback);

	void ThreadProcedure(void);
	void NotifyExit(void);
private:
	iFunction<void (mcWPFDispatcherReference*)noexcept> *fCallback;

	System::Windows::Threading::DispatcherFrame fDispatchFrame;

};
//---------------------------------------------------------------------------
private ref class rcWPFUIThread sealed
{
public:
	static System::Threading::ThreadLocal<rcWPFUIThread^> ^gTLS=gcnew System::Threading::ThreadLocal<rcWPFUIThread^>();

	static System::Windows::Threading::DispatcherPriority ToDispatcherPriority(bool HighPriority);

	rcWPFUIThread(System::Windows::Threading::Dispatcher ^Dispatcher,rcWPFUIThreadDispatcherProcedure ^Procedure,mbcWPFUIThread *CPP);

	void CPPDispose(void);

	property System::Windows::Threading::Dispatcher ^Dispatcher{
		System::Windows::Threading::Dispatcher ^get();
	}
	
	System::Windows::Threading::Dispatcher^ QueryFallbackDispatcher(void);
protected:
	initonly System::Windows::Threading::Dispatcher ^fDispatcher;
	System::EventHandler ^fOnShutdownEvent;
	

	void OnShutdownFinished(System::Object ^sender,System::EventArgs ^e);
internal:
	mbcWPFUIThread *fCPP;
	rcWPFUIThreadDispatcherProcedure ^fProcedure;

	void CPPCallClose(void);

	ref class rcDelegateExecutor sealed
	{
	public:
		rcWPFUIThread ^Thread;
		System::Action ^Procedure;
		System::Windows::Threading::DispatcherPriority Priority;
		int AbortProcessFlag;

		__forceinline void Aborted(void);
		void OnAborted(System::Object ^sender,System::EventArgs ^e);
	};
	ref class rcReferencedProcedureExecutor sealed
	{
	public:
		mbcWPFUIThread *CPP;
		iReference *Reference;
		iProcedure *Procedure;

		void Execute(void);
	};
private:
	static int gFallbackDispatcherRefCount=0;
	static System::Threading::ManualResetEvent ^gFallbackCreateCompleteEvent=gcnew System::Threading::ManualResetEvent(false);
	static System::Windows::Threading::Dispatcher^ gFallbackDispatcher=nullptr;
	static System::Threading::SpinLock gFallbackLock;
	
	System::Windows::Threading::Dispatcher^ fFallbackDispatcher=nullptr;
	static void FallbackDispatcherProcedure(void);

	static System::Windows::Threading::Dispatcher^ SetupFallbackDispatcher(void);
	static void ClearFallbackDispatcher(void);
};
//---------------------------------------------------------------------------
#endif	// _MANAGED
//---------------------------------------------------------------------------
struct mcWPFDispatcherReference
{
#if _MANAGED
	System::Windows::Threading::Dispatcher^ &Dispatcher;
	rcWPFUIThreadDispatcherProcedure^ const&Procedure;
#else
	mcWPFDispatcherReference()=delete;
	~mcWPFDispatcherReference()=delete;
#endif
};
//---------------------------------------------------------------------------
class mbcWPFUIThread
{
public:
	bool IsShutdown(void)const noexcept(true);
	bool mIsCurrent(void)const noexcept(true);
	static mbcWPFUIThread* mCurrentUIThread(void)noexcept(true);
	static mbcWPFUIThread* mCurrentUIThread(iFunction<void (mcWPFDispatcherReference&)noexcept(true)> *MakeObject)noexcept(true);

	void mExecuteDelegate(bool HighPriority,mbcActionDelegate &Delegate)noexcept(true);

	void mDispatchExecute(bool HighPriority,iProcedure *Procedure)noexcept(true);
	void mDispatchExecuteSync(bool HighPriority,iProcedure *Procedure)noexcept(true);
	void mDispatchExecuteReferenced(bool HighPriority,iReference *Reference,iProcedure *Procedure)noexcept(true);

#if _MANAGED
	friend rcWPFUIThread;

	__forceinline rcWPFUIThread^ __clrcall GetWPFThread(void)const noexcept(true);
	__forceinline void __clrcall ExecuteDelegate(System::Windows::Threading::DispatcherPriority Priority,System::Action ^Procedure)noexcept(true);
#endif // _MANAGED

protected:
	void mSetup(mcWPFDispatcherReference &DispatcherReference)noexcept(true);
	void mClear(void)noexcept(true);

	void mCPPClose(void)noexcept(true);

#if _MANAGED
	mcGCHandle<rcWPFUIThread,eGCHandleType::Normal> fUIThreadHandle;
#else
	cGCHandle fUIThreadHandle;
#endif
	
	virtual void WPFOnShutdown(void)noexcept(true)=0;
	virtual void WPFOnCPPClose(void)noexcept(true)=0;
private:
	void nExecuteProcedureReference(iReference *Reference,iProcedure *Procedure)noexcept(true);

};
//---------------------------------------------------------------------------
void WPFUIThreadNewThread(iFunction<void (mcWPFDispatcherReference*)noexcept(true)> *Callback)noexcept(true);
//---------------------------------------------------------------------------
class mcWPFDispatchFrame
{
public:
	void mSetup(void)noexcept(true);
	void mClear(void)noexcept(true);

	void mNotifyStopRun(void)noexcept(true);
	void mUIMain(void)noexcept(true);
protected:
#if _MANAGED
	mcGCHandle<System::Windows::Threading::DispatcherFrame,eGCHandleType::Normal> fDispatchFrameRef;
#else
	cGCHandle fDispatchFrameRef;
#endif // _MANAGED
};
//---------------------------------------------------------------------------
struct mcWPFKeyEventArgs
{
#if _MANAGED
	System::Windows::Input::KeyEventArgs^ &EventArgs;
#else
	mcWPFKeyEventArgs()=delete;
	~mcWPFKeyEventArgs()=delete;
#endif

	void mCancelEvent(void)noexcept(true);
	bool mIsCancelled(void)noexcept(true);
};
//---------------------------------------------------------------------------
struct mcWPFMouseEventArgs
{
#if _MANAGED
	System::Windows::Input::MouseEventArgs^ &EventArgs;
#else
	mcWPFMouseEventArgs()=delete;
	~mcWPFMouseEventArgs()=delete;
#endif

	void mCancelEvent(void)noexcept(true);
	bool mIsCancelled(void)noexcept(true);
};
//---------------------------------------------------------------------------
struct mcWPFTouchEventArgs
{
#if _MANAGED
	System::Windows::Input::TouchEventArgs^ &EventArgs;
#else
	mcWPFTouchEventArgs()=delete;
	~mcWPFTouchEventArgs()=delete;
#endif

	void* mGetTouchID(void)noexcept(true);
	void mCancelEvent(void)noexcept(true);
	bool mIsCancelled(void)noexcept(true);

	bool mGetPosition(const cGCHandle &UIElementHandle,cUIPoint &Position)noexcept(true);

#if _MANAGED
	bool __clrcall mGetPosition(System::Windows::Media::Visual ^Visual,cUIPoint &Position)noexcept(true);
#endif
};
//---------------------------------------------------------------------------
}	// namespace cnWin
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
