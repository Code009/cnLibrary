/*- cnWin - WPF - Managed - UI Core ---------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2020-04-14                                          */
/*-------------------------------------------------------------------------*/
#pragma once
/*-------------------------------------------------------------------------*/
#if _MANAGED
#using <WindowsBase.dll>
#using <PresentationCore.dll>
#endif	/* _MANAGED */

#include <cnWinCLI\WinDNetM_Common.h>

#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
#if _MANAGED
//---------------------------------------------------------------------------
namespace DNet{
//---------------------------------------------------------------------------
private interface class IWPFUIViewVisual
{
	property System::Windows::Media::Visual^ Visual{
		System::Windows::Media::Visual^ get(void);
	};
	property float LayoutScale{
		float get(void);
	};
};
//---------------------------------------------------------------------------
}	//	namespace DNet
//---------------------------------------------------------------------------
#endif // _MANAGED
//---------------------------------------------------------------------------
namespace cnWin{
//---------------------------------------------------------------------------
#if _MANAGED
//---------------------------------------------------------------------------
private ref class rcWPFUICoreProcedureCaller
{
public:
	rcWPFUICoreProcedureCaller(void *CPP);
	
	void mcDNetUIThreadAsyncProcedureRefExecutor_Execute(void);

	void mcDNetUIThreadAsyncTimerExecutor_TimerTick(System::Object ^,System::EventArgs ^e);
	void mcDNetUIThreadAsyncTimerExecutor_FirstHit(void);

	void mcDispatchExecuteAsyncTask_Execute(void);

	void mcDNetUIThreadDispatchFrame_ThreadExitProcedure(void);
	void mcDNetUIThreadDispatchFrame_OnShutdownFinished(System::Object ^sender,System::EventArgs ^e);
private:
	void *CPP;
};
//---------------------------------------------------------------------------
#endif	// _MANAGED
//---------------------------------------------------------------------------
class mcDNetUIThreadDispatcher;
//---------------------------------------------------------------------------
class mcDNetUIThreadAsyncProcedureExecutor
{
public:
	mcDNetUIThreadAsyncProcedureExecutor(bool HighPriority,iProcedure *Procedure)noexcept(true);
	~mcDNetUIThreadAsyncProcedureExecutor()noexcept(true);
protected:

#if _MANAGED
	struct cManaged
	{
		System::Runtime::InteropServices::GCHandle Action;
		System::Windows::Threading::DispatcherPriority Priority;
	};
#endif
	cnCLI_MANAGED_VALUE_STRUCT(cManaged,1,4) fManaged;

	void mStart(mcDNetUIThreadDispatcher *UIDispatch)noexcept(true);
};
//---------------------------------------------------------------------------
class mcDNetUIThreadAsyncProcedureRefExecutor
{
public:
	mcDNetUIThreadAsyncProcedureRefExecutor(bool HighPriority,iReference *Reference,iProcedure *Procedure)noexcept(true);
	~mcDNetUIThreadAsyncProcedureRefExecutor()noexcept(true);
protected:
	iReference *fReference;
	iProcedure *fProcedure;

#if _MANAGED
	friend rcWPFUICoreProcedureCaller;
	struct cManaged
	{
		System::Runtime::InteropServices::GCHandle Action;
		System::Windows::Threading::DispatcherPriority Priority;
	};
#endif
	cnCLI_MANAGED_VALUE_STRUCT(cManaged,1,4) fManaged;
	

	void mStart(mcDNetUIThreadDispatcher *UIDispatch)noexcept(true);

	void CallProc(void)noexcept(true);
};
//---------------------------------------------------------------------------
class mcDNetUIThreadAsyncTimerExecutor
{
public:
	mcDNetUIThreadAsyncTimerExecutor(mcDNetUIThreadDispatcher *UIDispatch,bool HighPriority,iReference *Reference,iProcedure *Procedure)noexcept(true);
	~mcDNetUIThreadAsyncTimerExecutor()noexcept(true);
		
protected:

	iReference *fReference;
	iProcedure *fProcedure;
	void TimerStateThreadProc(void)noexcept(true);
	
#if _MANAGED

	struct cManaged
	{
		System::Windows::Threading::DispatcherPriority Priority;
	};
#endif
	cnCLI_MANAGED_VALUE_STRUCT(cManaged,0,4) fManaged;
	
#if _MANAGED
	mcGCHandleT<System::Windows::Threading::DispatcherTimer> fTimer;
	mcGCHandleT<System::Action> fFirstHitAction;
	mcGCHandleT<System::EventHandler> fTimerTick;
#else
	cGCHandle fTimer;
	cGCHandle fFirstHitAction;
	cGCHandle fTimerTick;
#endif // _MANAGED

	cnRTL::cnWinRTL::cWinExclusiveFlag fTimerStateFlag;
	bool fTimerStop;
	bool fTimerStarted;
	bool fTimerSwitchInterval;
	
	uInt64 fPeriod;

	void mStopTimer(void)noexcept(true);
	void mHitAndSetTimer(mcDNetUIThreadDispatcher *UIDispatch)noexcept(true);
	void mScheludeFirstHit(uInt64 Delay)noexcept(true);
	void ExecuteAndState(void)noexcept(true);

	void TimerStart(mcDNetUIThreadDispatcher *UIDispatch,uInt64 DueTime,uInt64 Period)noexcept(true);
	void TimerStop(void)noexcept(true);

#if _MANAGED
	friend rcWPFUICoreProcedureCaller;
	void __clrcall TimerTick(void)noexcept(true);
	void __clrcall TimerFirstHit(void)noexcept(true);

#endif // _MANAGED


};
//---------------------------------------------------------------------------
#if _MANAGED
class mcDNetUIThreadDispatcher;
private value struct rcDNetUIThreadContext
{
	mcDNetUIThreadDispatcher *CPPDispatcher;

	[System::ThreadStatic]
	static rcDNetUIThreadContext gTLSContext;
};
//---------------------------------------------------------------------------
#endif	// _MANAGED
//---------------------------------------------------------------------------
class mcDNetUIThreadDispatcher
{
public:
	mcDNetUIThreadDispatcher(iReference *InnerReference)noexcept(true);
	~mcDNetUIThreadDispatcher()noexcept(true);

#if _MANAGED
	static System::Windows::Threading::DispatcherPriority __clrcall GetDispatcherPriority(bool HighPriority)noexcept(true);

	System::Windows::Threading::Dispatcher^ __clrcall GetDispatcher(void)noexcept(true);

	void __clrcall SetThreadReleaseNotify(System::Action ^Action)noexcept(true);
#endif
	bool mCreateOnCurrentThread(void)noexcept(true);
	
	bool mIsCurrent(void)noexcept(true);
	bool mIsShutdown(void)noexcept(true);
	static mcDNetUIThreadDispatcher* mCurrentUIDispatcher(void)noexcept(true);
	
	void mDispatchExecute(bool HighPriority,iProcedure *Procedure)noexcept(true);
	void mDispatchExecuteSync(bool HighPriority,iProcedure *Procedure)noexcept(true);
	void mDispatchExecuteReferenced(bool HighPriority,iReference *Reference,iProcedure *Procedure)noexcept(true);

	class mcDispatchExecuteAsyncTask
	{
	public:

		rPtr<iReference> Reference;
		iProcedure *Procedure;
		cnRTL::cManualAsyncTask TaskState;

#if _MANAGED
		friend rcWPFUICoreProcedureCaller;

#endif	// _MANAGED
	protected:
		virtual void ExecuteTask(void)noexcept(true)=0;
	};

	void mDispatchExecuteAsync(mcDispatchExecuteAsyncTask *Task,bool HighPriority)noexcept(true);
	
protected:

	void mShutdownFrame(void)noexcept(true);

#if _MANAGED
	void __clrcall UIThreadRelease(void)noexcept(true);

#endif
	
#if _MANAGED
	mcGCRefT<System::Windows::Threading::Dispatcher> fDispatcher;
	mcGCRefT<System::EventHandler> fOnShutdownEvent;
	mcGCRefT<System::Action> fReleaseNotify;
#else
	cGCRef fDispatcher;
	cGCRef fOnShutdownEvent;
	cGCRef fReleaseNotify;
#endif // _MANAGED

#if _MANAGED
	void __clrcall SetupDispatcher(System::Windows::Threading::Dispatcher ^Dispatcher)noexcept(true);
	void __clrcall ClearDispatcher(void)noexcept(true);

#endif // _MANAGED

public:
	struct cDispatcherFinishNotify
	{
		cDispatcherFinishNotify *Parent;
		cDispatcherFinishNotify *Child[2];
		ufInt8 Branch;
		ufInt8 Color;

#if _MANAGED
		void (__clrcall *NotifyProcedure)(cDispatcherFinishNotify *Notify,bool Shutdown)noexcept(true);
#else
		void (*NotifyProcedure)(void);
#endif
	};
	void mAttachDispatcher(cDispatcherFinishNotify *NotifyToken)noexcept(true);
	bool mDetachDispatcher(cDispatcherFinishNotify *NotifyToken)noexcept(true);

	bool fDispatcherShutdown=false;
private:
	bool fDispatchedRelease=false;
	
	iReference *fOwnerInnerReference;
	void ThreadExitProc(void)noexcept(true);

	cnRTL::cLinkItemSet<
		cDispatcherFinishNotify,
		cnDataStruct::cItemAddressOrderOperator<cDispatcherFinishNotify>
	> fFinishNotifySet;
	cnRTL::cnWinRTL::cCriticalSection fShutdownLock;

#if _MANAGED
	friend rcWPFUICoreProcedureCaller;

	void __clrcall WPFOnShutdown(void)noexcept(true);
	static void __clrcall FinishNotifyRelease(System::IntPtr This,System::IntPtr DispatcherFinishNoftify)noexcept(true);
	void __clrcall NotifyFinish(cDispatcherFinishNotify *FinishNoftify)noexcept(true);
#endif // _MANAGED


public:
	static bool mKeyboardEventIsKeyDown(eKeyCode KeyCode)noexcept(true);
	static bool mKeyboardEventIsKeyToggled(eKeyCode KeyCode)noexcept(true);

	static bool mMouseGetPosition(cGCRef &UIViewVisualHandle,cUIPoint &Position)noexcept(true);
	static bool mMouseEventIsButtonDown(eMouseButton Button)noexcept(true);
#if _MANAGED

	static bool __clrcall mMouseGetPosition(System::Windows::Media::Visual ^Visual,cUIPoint &Position)noexcept(true);
	static bool __clrcall mMouseGetPosition(DNet::IWPFUIViewVisual ^ViewVisual,cUIPoint &Position)noexcept(true);
#endif // _MANAGED

};
//---------------------------------------------------------------------------
#if _MANAGED
//---------------------------------------------------------------------------
private ref class rcDNetUIThreadDispatchFrame
{
internal:
	rcDNetUIThreadDispatchFrame(iReference *DispatcherReference,mcDNetUIThreadDispatcher *Dispatcher,System::Threading::ManualResetEvent ^DoneEvent);
	~rcDNetUIThreadDispatchFrame();

	void ThreadProcedure(void);
private:
	System::Threading::ManualResetEvent ^fDoneEvent;
	mcDNetUIThreadDispatcher *fDispatcher;
	iReference *fDispatcherReference;
	
	System::Windows::Threading::DispatcherFrame fDispatchFrame;

	void UIThreadExit(void);
};
//---------------------------------------------------------------------------
#endif	// _MANAGED
//---------------------------------------------------------------------------
void rcDNetUIThreadDispatchFrame_StartUIThread(iReference *DispatcherReference,mcDNetUIThreadDispatcher *Dispatcher)noexcept(true);
//---------------------------------------------------------------------------
class mcDNetUIApplicationDispatchFrame
{
public:
	mcDNetUIApplicationDispatchFrame()noexcept(true);
	~mcDNetUIApplicationDispatchFrame()noexcept(true);
protected:
#if _MANAGED
	mcGCRefT<System::Windows::Threading::DispatcherFrame> fDispatchFrameRef;
#else
	cGCRef fDispatchFrameHandle;
#endif // _MANAGED

	void mNotifyStopRun(void)noexcept(true);
	void mWPFUIMain(void)noexcept(true);
};
//---------------------------------------------------------------------------
}	// namespace cnWin
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
