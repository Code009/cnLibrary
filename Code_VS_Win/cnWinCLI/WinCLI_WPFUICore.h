/*- cnWinCLI - WPF - UI Core ----------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2023-10-07                                          */
/*-------------------------------------------------------------------------*/
#pragma once
/*-------------------------------------------------------------------------*/

#include <cnRTL\cnRTL.h>
#include <cnWinCLI\WinCLI_Common.h>
#include <cnWinCLI\WinCLIM_WPFUICore.h>

#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnWin{
//---------------------------------------------------------------------------
struct cWPFModule// : iDependentInfo
{
	cWPFModule()noexcept(true);
	~cWPFModule()noexcept(true);
	
	//virtual void cnLib_FUNC DependentShutdownNotification(void)noexcept(true)override;

	cnRTL::cWaitObjectRegistration ObjectRegistration;
};
extern cWPFModule gWPFModule;
//---------------------------------------------------------------------------
class cWPFUIThread;
//---------------------------------------------------------------------------
class cWPFKeyboard : public iUIKeyboard
{
public:
	virtual bool cnLib_FUNC EventIsKeyDown(eKeyCode KeyCode)noexcept(true)override;
	virtual bool cnLib_FUNC EventIsKeyToggled(eKeyCode KeyCode)noexcept(true)override;
	virtual bool cnLib_FUNC IsKeyDown(eKeyCode KeyCode)noexcept(true)override;
	virtual bool cnLib_FUNC IsKeyToggled(eKeyCode KeyCode)noexcept(true)override;

};
//---------------------------------------------------------------------------
class cWPFMouse : public iUIMouse
{
public:
	virtual bool cnLib_FUNC GetPosition(iInterface *Relative,cUIPoint &Position)noexcept(true)override;
	virtual bool cnLib_FUNC EventIsButtonDown(eMouseButton Button)noexcept(true)override;
	virtual bool cnLib_FUNC IsButtonDown(eMouseButton Button)noexcept(true)override;

};
//---------------------------------------------------------------------------
class cWPFUIThreadAsyncProcedure : public iAsyncProcedure
{
public:
	cWPFUIThreadAsyncProcedure(cWPFUIThread *UIThread,bool HighPriority,iProcedure *Procedure)noexcept(true);
	~cWPFUIThreadAsyncProcedure()noexcept(true);

	virtual void cnLib_FUNC Start(void)noexcept(true)override;

protected:
	iPtr<cWPFUIThread> fUIThread;
	mcNativeProcedureDelegate fExecutor;
	bool fHighPriority;
};
//---------------------------------------------------------------------------
class cWPFUIThreadAsyncProcedureRef : public iAsyncProcedure, protected mbcCLICallback
{
public:
	cWPFUIThreadAsyncProcedureRef(cWPFUIThread *UIThread,bool HighPriority,iReference *Reference,iProcedure *Procedure)noexcept(true);
	~cWPFUIThreadAsyncProcedureRef()noexcept(true);

	virtual void cnLib_FUNC Start(void)noexcept(true)override;

protected:
	iPtr<cWPFUIThread> fUIThread;
	iReference *fReference;
	iProcedure *fProcedure;
	bool fHighPriority;
	
	virtual void CLIExecute(void)noexcept(true)override;
};
//---------------------------------------------------------------------------
class cWPFUIThreadAsyncTimer : public iAsyncTimer, public cnRTL::cDualReference
{
public:
	cWPFUIThreadAsyncTimer(cWPFUIThread *UIDispatch,bool HighPriority,iReference *Reference,iProcedure *Procedure)noexcept(true);
	~cWPFUIThreadAsyncTimer()noexcept(true);

	virtual void cnLib_FUNC Start(uInt64 DueTime,uInt64 Period)noexcept(true)override;
	virtual void cnLib_FUNC Stop(void)noexcept(true)override;

protected:
	void VirtualStopped(void)noexcept(true);

	iPtr<cWPFUIThread> fUIDispatch;
	iReference *fReference;
	iProcedure *fProcedure;

	class cExecutor : public mcWPFUIThreadAsyncTimerExecutor
	{
		virtual void Schedule(void)noexcept(true)override;
		virtual void TimerTick(void)noexcept(true)override;
		virtual void TimerCleanup(void)noexcept(true)override;
	};
	cManagedStruct<cExecutor> fExecutor;

	uInt64 fDueTime;
	uInt64 fPeriod;
	bool fActive=false;
	bool fUpdate=false;
	bool fClose=false;

	void TimerTick(void)noexcept(true);
private:

	cnRTL::cExclusiveFlag fTimerStateFlag;
	bool fTimerActive=false;
	bool fTimerDue;

	cnRTL::rAtomicPtr<iReference> fStateProcRefrence;

	void NotifyStateProcedure(void)noexcept(true);
	void StateProcedureThread(void)noexcept(true);
	void StateProcedure(void)noexcept(true);

	void State_UpdateTimer(void)noexcept(true);
};
//---------------------------------------------------------------------------
class cWPFUIThread : public iUIThread, public cnRTL::cDualReference
	, public mbcWPFUIThread
{
public:
	cWPFUIThread(mcWPFDispatcherReference &DispatcherReference)noexcept(true);
	~cWPFUIThread()noexcept(true);

	bool IsCurrent(void)const noexcept(true);

	// iUIThread

	virtual bool cnLib_FUNC IsCurrentThread(void)noexcept(true)override;
	virtual iDispatch* cnLib_FUNC GetDispatch(bool HighPriority)noexcept(true)override;
	virtual iUIKeyboard* cnLib_FUNC GetDefaultKeyboard(void)noexcept(true)override;
	virtual iUIMouse* cnLib_FUNC GetDefaultMouse(void)noexcept(true)override;


	static cWPFUIThread* CurrentUIThread(void)noexcept(true);
	static iPtr<cWPFUIThread> CreateOnCurrentThread(void)noexcept(true);
	static iPtr<cWPFUIThread> StartUIThread(void)noexcept(true);

	void DispatchExecute(bool HighPriority,iReference *Reference,iProcedure *Procedure)noexcept(true);
	iPtr<iAsyncTask> DispatchExecuteAsync(bool HighPriority,iReference *Reference,iProcedure *Procedure)noexcept(true);


protected:
	void VirtualStarted(void)noexcept(true);
	void VirtualStopped(void)noexcept(true);

	virtual void WPFOnShutdown(void)noexcept(true)override;
	virtual void WPFOnCPPClose(void)noexcept(true)override;

	class mcWPFDispatchHigh : public iDispatch
	{
	public:
		cWPFUIThread* GetHost(void)noexcept(true);
		virtual void* cnLib_FUNC CastInterface(iTypeID IID)noexcept(true) override;

		virtual rPtr<iAsyncProcedure>	cnLib_FUNC CreateWork(iReference *Reference,iProcedure *ThreadProcedure)noexcept(true)override;
		virtual rPtr<iAsyncTimer>		cnLib_FUNC CreateTimer(iReference *Reference,iProcedure *ThreadProcedure)noexcept(true)override;
		virtual bool cnLib_FUNC IsCurrentThread(void)noexcept(true)override;
		virtual void cnLib_FUNC Execute(iReference *Reference,iProcedure *Procedure)noexcept(true)override;
		virtual void cnLib_FUNC ExecuteSync(iProcedure *Procedure)noexcept(true)override;
		virtual iPtr<iAsyncTask> cnLib_FUNC ExecuteAsync(iReference *Reference,iProcedure *Procedure)noexcept(true)override;

	}fDispatchHigh;
	class mcWPFDispatchLow : public iDispatch
	{
	public:
		cWPFUIThread* GetHost(void)noexcept(true);
		virtual void* cnLib_FUNC CastInterface(iTypeID IID)noexcept(true) override;

		virtual rPtr<iAsyncProcedure>	cnLib_FUNC CreateWork(iReference *Reference,iProcedure *ThreadProcedure)noexcept(true)override;
		virtual rPtr<iAsyncTimer>		cnLib_FUNC CreateTimer(iReference *Reference,iProcedure *ThreadProcedure)noexcept(true)override;
		virtual bool cnLib_FUNC IsCurrentThread(void)noexcept(true)override;
		virtual void cnLib_FUNC Execute(iReference *Reference,iProcedure *Procedure)noexcept(true)override;
		virtual void cnLib_FUNC ExecuteSync(iProcedure *Procedure)noexcept(true)override;
		virtual iPtr<iAsyncTask> cnLib_FUNC ExecuteAsync(iReference *Reference,iProcedure *Procedure)noexcept(true)override;

	}fDispatchLow;

	rPtr<iAsyncProcedure> DispatchCreateWork(bool HighPriority,iReference *Reference,iProcedure *ThreadProcedure)noexcept(true);
	rPtr<iAsyncTimer> DispatchCreateTimer(bool HighPriority,iReference *Reference,iProcedure *ThreadProcedure)noexcept(true);

	class cExecuteAsyncTask : public iAsyncTask, private iProcedure
	{
	public:
		cExecuteAsyncTask(iReference *Reference,iProcedure *Procedure)noexcept;
		virtual bool cnLib_FUNC IsDone(void)noexcept(true)override;
		virtual bool cnLib_FUNC Await(iProcedure *NotifyProcedure)noexcept(true)override;

		void Queue(mbcWPFUIThread *Thread,bool HighPriority)noexcept(true);

	private:
		rPtr<iReference> Reference;
		iProcedure *Procedure;
		cnRTL::cManualAsyncTask TaskState;
		virtual void cnLib_FUNC Execute(void)noexcept(true)override;
	};

private:
	mutable DWORD fNativeThreadID;

	bool fDispatcherShutdown=false;

	class cCreateOnCurrentProcedure : public iFunction<void(mcWPFDispatcherReference&)noexcept(true)>
	{
	public:
		iPtr<cWPFUIThread> Thread;
		virtual void cnLib_FUNC Execute(mcWPFDispatcherReference &DispatcherReference)noexcept(true)override;
	};

	class cThreadSetupProcedure : public iFunction<void(mcWPFDispatcherReference*)noexcept(true)>
	{
	public:
		iPtr<cWPFUIThread> Thread;

		void Wait(void)noexcept(true);
		virtual void cnLib_FUNC Execute(mcWPFDispatcherReference *DispatcherReference)noexcept(true)override;
	private:
		cnRTL::cLocalSingleThreadNotification fNotifier;
	};

	iPtr<cWPFKeyboard> fWPFKeyboard;
	iPtr<cWPFMouse> fWPFMouse;
};
//---------------------------------------------------------------------------
class cWPFUIApplication : public iWindowsUIApplication
{
public:
	static rPtr<iWindowsUIApplication> CreateWindowsUIApplication(void)noexcept(true);


	cWPFUIApplication(iPtr<cWPFUIThread> Thread)noexcept(true);
	~cWPFUIApplication()noexcept(true);

	virtual iUIThread* cnLib_FUNC GetMainUIThread(void)noexcept(true)override;
	virtual bool cnLib_FUNC InsertHandler(iWindowsUISessionHandler *SessionHandler)noexcept(true)override;
	virtual bool cnLib_FUNC RemoveHandler(iWindowsUISessionHandler *SessionHandler)noexcept(true)override;
	virtual void cnLib_FUNC UIMain(void)noexcept(true)override;
	virtual void cnLib_FUNC CloseUISession(void)noexcept(true)override;

private:
	iPtr<cWPFUIThread> fUIThread;
	cnRTL::cSeqSet<iWindowsUISessionHandler*> fHandlers;
	bool fUISession=false;

	mcWPFDispatchFrame fDispatchFrame;
};
//---------------------------------------------------------------------------
}	// namespace cnWin
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
