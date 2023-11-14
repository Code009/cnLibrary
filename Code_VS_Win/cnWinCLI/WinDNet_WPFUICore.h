/*- cnWin - WPF - UI Core -------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2023-10-07                                          */
/*-------------------------------------------------------------------------*/
#pragma once
/*-------------------------------------------------------------------------*/

#include <cnWinCLI\WinDNet_Common.h>
#include <cnWinCLI\WinDNetM_WPFUICore.h>

#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnWin{
//---------------------------------------------------------------------------
iUIThread* DNetCurrentUIThread(void);
iPtr<iUIThread> DNetCreateOnCurrentThread(void);
iPtr<iUIThread> DNetStartUIThread(void);
rPtr<iWindowsUIApplication> DNetCreateWindowsUIApplication(void);
//---------------------------------------------------------------------------
class cWPFKeyboard : public iUIKeyboard
{
public:
	virtual bool cnLib_FUNC EventIsKeyDown(eKeyCode KeyCode)override;
	virtual bool cnLib_FUNC EventIsKeyToggled(eKeyCode KeyCode)override;
	virtual bool cnLib_FUNC IsKeyDown(eKeyCode KeyCode)override;
	virtual bool cnLib_FUNC IsKeyToggled(eKeyCode KeyCode)override;

};
//---------------------------------------------------------------------------
class cWPFMouse : public iUIMouse
{
public:
	virtual bool cnLib_FUNC GetPosition(iInterface *Relative,cUIPoint &Position)override;
	virtual bool cnLib_FUNC EventIsButtonDown(eMouseButton Button)override;
	virtual bool cnLib_FUNC IsButtonDown(eMouseButton Button)override;

};
//---------------------------------------------------------------------------
class cDNetUIThread;
//---------------------------------------------------------------------------
class cDNetUIThreadAsyncProcedure : public iAsyncProcedure, private mcDNetUIThreadAsyncProcedureExecutor
{
public:
	cDNetUIThreadAsyncProcedure(cDNetUIThread *UIThread,bool HighPriority,iProcedure *Procedure);
	~cDNetUIThreadAsyncProcedure();

	virtual void cnLib_FUNC Start(void)override;

protected:
	iPtr<cDNetUIThread> fUIThread;
};
//---------------------------------------------------------------------------
class cDNetUIThreadAsyncProcedureRef : public iAsyncProcedure, public mcDNetUIThreadAsyncProcedureRefExecutor
{
public:
	cDNetUIThreadAsyncProcedureRef(cDNetUIThread *UIThread,bool HighPriority,iReference *Reference,iProcedure *Procedure);
	~cDNetUIThreadAsyncProcedureRef();

	virtual void cnLib_FUNC Start(void)override;

protected:
	iPtr<cDNetUIThread> fUIThread;
};
//---------------------------------------------------------------------------
class cDNetUIThreadAsyncTimer : public iAsyncTimer, private mcDNetUIThreadAsyncTimerExecutor
{
public:
	cDNetUIThreadAsyncTimer(cDNetUIThread *UIDispatch,bool HighPriority,iReference *Reference,iProcedure *Procedure);
	~cDNetUIThreadAsyncTimer();

	virtual void cnLib_FUNC Start(iTimepoint *DueTime,sInt64 DueTimeDelay,uInt64 Period)override;
	virtual void cnLib_FUNC Stop(void)override;
		
protected:
	iPtr<cDNetUIThread> fUIDispatch;
};
//---------------------------------------------------------------------------
class cDNetUIThread : public iUIThread, public cnRTL::cDualReference, public mcDNetUIThreadDispatcher
{
protected:
	cDNetUIThread();
	~cDNetUIThread();

	void VirtualStopped(void);
	
public:

	bool IsCurrent(void);
	bool IsShutdown(void);

	// iUIThread

	virtual bool cnLib_FUNC IsCurrentThread(void)override;
	virtual iDispatch* cnLib_FUNC GetDispatch(bool HighPriority)override;
	virtual iUIKeyboard* cnLib_FUNC GetDefaultKeyboard(void)override;
	virtual iUIMouse* cnLib_FUNC GetDefaultMouse(void)override;


	static cDNetUIThread* CurrentUIThread(void);
	static iPtr<cDNetUIThread> CreateOnCurrentThread(void);
	static iPtr<cDNetUIThread> StartUIThread(void);


	void DispatchExecute(bool HighPriority,iReference *Reference,iProcedure *Procedure);
	iPtr<iAsyncTask> DispatchExecuteAsync(bool HighPriority,iReference *Reference,iProcedure *Procedure);


protected:

	class mcWPFDispatchHigh : public iDispatch
	{
	public:
		cDNetUIThread* GetHost(void);
		virtual void* cnLib_FUNC CastInterface(iTypeID IID)noexcept(true) override;

		virtual rPtr<iAsyncProcedure>	cnLib_FUNC CreateWork(iReference *Reference,iProcedure *ThreadProcedure)override;
		virtual rPtr<iAsyncTimer>		cnLib_FUNC CreateTimer(iReference *Reference,iProcedure *ThreadProcedure)override;
		virtual bool cnLib_FUNC IsCurrentThread(void)override;
		virtual void cnLib_FUNC Execute(iReference *Reference,iProcedure *Procedure)override;
		virtual void cnLib_FUNC ExecuteSync(iProcedure *Procedure)override;
		virtual iPtr<iAsyncTask> cnLib_FUNC ExecuteAsync(iReference *Reference,iProcedure *Procedure)override;

	}fDispatchHigh;
	class mcWPFDispatchLow : public iDispatch
	{
	public:
		cDNetUIThread* GetHost(void);
		virtual void* cnLib_FUNC CastInterface(iTypeID IID)noexcept(true) override;

		virtual rPtr<iAsyncProcedure>	cnLib_FUNC CreateWork(iReference *Reference,iProcedure *ThreadProcedure)override;
		virtual rPtr<iAsyncTimer>		cnLib_FUNC CreateTimer(iReference *Reference,iProcedure *ThreadProcedure)override;
		virtual bool cnLib_FUNC IsCurrentThread(void)override;
		virtual void cnLib_FUNC Execute(iReference *Reference,iProcedure *Procedure)override;
		virtual void cnLib_FUNC ExecuteSync(iProcedure *Procedure)override;
		virtual iPtr<iAsyncTask> cnLib_FUNC ExecuteAsync(iReference *Reference,iProcedure *Procedure)override;

	}fDispatchLow;

	rPtr<iAsyncProcedure> DispatchCreateWork(bool HighPriority,iReference *Reference,iProcedure *ThreadProcedure);
	rPtr<iAsyncTimer> DispatchCreateTimer(bool HighPriority,iReference *Reference,iProcedure *ThreadProcedure);

	class cExecuteAsyncTask : public iAsyncTask, public mcDispatchExecuteAsyncTask
	{
	public:
		virtual bool cnLib_FUNC IsDone(void)override;
		virtual bool cnLib_FUNC SetNotify(iProcedure *NotifyProcedure)override;
		virtual void ExecuteTask(void)override;
	};

private:

	DWORD fNativeThreadID;

	iPtr<cWPFKeyboard> fWPFKeyboard;
	iPtr<cWPFMouse> fWPFMouse;
};
//---------------------------------------------------------------------------
class cDNetUIApplication : public iWindowsUIApplication, private mcDNetUIApplicationDispatchFrame
{
public:
	cDNetUIApplication(iPtr<cDNetUIThread> Thread);
	~cDNetUIApplication();


	virtual iUIThread* cnLib_FUNC GetMainUIThread(void)override;
	virtual void cnLib_FUNC UIMain(iWindow *MainWindow,int CmdShow)override;

private:
	iPtr<cDNetUIThread> fUIThread;

	class cMainWindowMessageHandler : public iWindowMessageHandler
	{
	public:
		virtual void cnLib_FUNC WindowAttached(void)override;
		virtual void cnLib_FUNC WindowDetached(void)override;
		virtual bool cnLib_FUNC WindowMessage(LRESULT &Result,const cWindowMessageParam &MsgParam)override;
		virtual void cnLib_FUNC WindowMessageProcessed(LRESULT Result,const cWindowMessageParam &MsgParam)override;
	private:
	}fMainWindowMessageHandler;
};
//---------------------------------------------------------------------------
}	// namespace cnWin
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
