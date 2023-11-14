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
	mcDNetUIThreadAsyncProcedureExecutor(bool HighPriority,iProcedure *Procedure);
	~mcDNetUIThreadAsyncProcedureExecutor();
protected:

#if _MANAGED
	struct cManaged
	{
		System::Runtime::InteropServices::GCHandle Action;
		System::Windows::Threading::DispatcherPriority Priority;
	};
#endif
	cnCLI_MANAGED_VALUE_STRUCT(cManaged,1,4) fManaged;

	void mStart(mcDNetUIThreadDispatcher *UIDispatch);
};
//---------------------------------------------------------------------------
class mcDNetUIThreadAsyncProcedureRefExecutor
{
public:
	mcDNetUIThreadAsyncProcedureRefExecutor(bool HighPriority,iReference *Reference,iProcedure *Procedure);
	~mcDNetUIThreadAsyncProcedureRefExecutor();
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
	

	void mStart(mcDNetUIThreadDispatcher *UIDispatch);

	void CallProc(void);
};
//---------------------------------------------------------------------------
class mcDNetUIThreadAsyncTimerExecutor
{
public:
	mcDNetUIThreadAsyncTimerExecutor(mcDNetUIThreadDispatcher *UIDispatch,bool HighPriority,iReference *Reference,iProcedure *Procedure);
	~mcDNetUIThreadAsyncTimerExecutor();
		
protected:

	iReference *fReference;
	iProcedure *fProcedure;
	void TimerStateThreadProc(void);	
	
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

	void mStopTimer(void);
	void mHitAndSetTimer(mcDNetUIThreadDispatcher *UIDispatch);
	void mScheludeFirstHit(uInt64 Delay);
	void ExecuteAndState(void);

	void TimerStart(mcDNetUIThreadDispatcher *UIDispatch,iTimepoint *DueTime,sInt64 DueTimeDelay,uInt64 Period);
	void TimerStop(void);

#if _MANAGED
	friend rcWPFUICoreProcedureCaller;
	void __clrcall TimerTick(void);
	void __clrcall TimerFirstHit(void);

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
	mcDNetUIThreadDispatcher(iReference *InnerReference);
	~mcDNetUIThreadDispatcher();

#if _MANAGED
	static System::Windows::Threading::DispatcherPriority __clrcall GetDispatcherPriority(bool HighPriority);

	System::Windows::Threading::Dispatcher^ __clrcall GetDispatcher(void);

	void __clrcall SetThreadReleaseNotify(System::Action ^Action);
#endif
	bool mCreateOnCurrentThread(void);
	
	bool mIsCurrent(void);
	bool mIsShutdown(void);
	static mcDNetUIThreadDispatcher* mCurrentUIDispatcher(void);
	
	void mDispatchExecute(bool HighPriority,iProcedure *Procedure);
	void mDispatchExecuteSync(bool HighPriority,iProcedure *Procedure);
	void mDispatchExecuteReferenced(bool HighPriority,iReference *Reference,iProcedure *Procedure);

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
		virtual void ExecuteTask(void)=0;
	};

	void mDispatchExecuteAsync(mcDispatchExecuteAsyncTask *Task,bool HighPriority);
	
protected:

	void mShutdownFrame(void);

#if _MANAGED
	void __clrcall UIThreadRelease(void);

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
	void __clrcall SetupDispatcher(System::Windows::Threading::Dispatcher ^Dispatcher);
	void __clrcall ClearDispatcher(void);

#endif // _MANAGED

public:
	struct cDispatcherFinishNotify
	{
		cDispatcherFinishNotify *Parent;
		cDispatcherFinishNotify *Child[2];
		ufInt8 Branch;
		ufInt8 Color;

#if _MANAGED
		void (__clrcall *NotifyProcedure)(cDispatcherFinishNotify *Notify,bool Shutdown);
#else
		void (*NotifyProcedure)(void);
#endif
	};
	void mAttachDispatcher(cDispatcherFinishNotify *NotifyToken);
	bool mDetachDispatcher(cDispatcherFinishNotify *NotifyToken);

	bool fDispatcherShutdown=false;
private:
	bool fDispatchedRelease=false;
	
	iReference *fOwnerInnerReference;
	void ThreadExitProc(void);

	cnRTL::cLinkItemSet<
		cDispatcherFinishNotify,
		cnDataStruct::cItemAddressOrderOperator<cDispatcherFinishNotify>
	> fFinishNotifySet;
	cnRTL::cnWinRTL::cCriticalSection fShutdownLock;

#if _MANAGED
	friend rcWPFUICoreProcedureCaller;

	void __clrcall WPFOnShutdown(void);
	static void __clrcall FinishNotifyRelease(System::IntPtr This,System::IntPtr DispatcherFinishNoftify);
	void __clrcall NotifyFinish(cDispatcherFinishNotify *FinishNoftify);
#endif // _MANAGED


public:
	static bool mKeyboardEventIsKeyDown(eKeyCode KeyCode);
	static bool mKeyboardEventIsKeyToggled(eKeyCode KeyCode);

	static bool mMouseGetPosition(cGCRef &UIViewVisualHandle,cUIPoint &Position);
	static bool mMouseEventIsButtonDown(eMouseButton Button);
#if _MANAGED

	static bool __clrcall mMouseGetPosition(System::Windows::Media::Visual ^Visual,cUIPoint &Position);
	static bool __clrcall mMouseGetPosition(DNet::IWPFUIViewVisual ^ViewVisual,cUIPoint &Position);
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
void rcDNetUIThreadDispatchFrame_StartUIThread(iReference *DispatcherReference,mcDNetUIThreadDispatcher *Dispatcher);
//---------------------------------------------------------------------------
class mcDNetUIApplicationDispatchFrame
{
public:
	mcDNetUIApplicationDispatchFrame();
	~mcDNetUIApplicationDispatchFrame();
protected:
#if _MANAGED
	mcGCRefT<System::Windows::Threading::DispatcherFrame> fDispatchFrameRef;
#else
	cGCRef fDispatchFrameHandle;
#endif // _MANAGED

	void mNotifyStopRun(void);
	void mWPFUIMain(void);
};
//---------------------------------------------------------------------------
}	// namespace cnWin
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
