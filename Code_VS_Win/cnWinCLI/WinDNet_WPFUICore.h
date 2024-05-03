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
iUIThread* DNetCurrentUIThread(void)noexcept(true);
iPtr<iUIThread> DNetCreateOnCurrentThread(void)noexcept(true);
iPtr<iUIThread> DNetStartUIThread(void)noexcept(true);
rPtr<iWindowsUIApplication> DNetCreateWindowsUIApplication(void)noexcept(true);
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
class cDNetUIThread;
//---------------------------------------------------------------------------
class cDNetUIThreadAsyncProcedure : public iAsyncProcedure, private mcDNetUIThreadAsyncProcedureExecutor
{
public:
	cDNetUIThreadAsyncProcedure(cDNetUIThread *UIThread,bool HighPriority,iProcedure *Procedure)noexcept(true);
	~cDNetUIThreadAsyncProcedure()noexcept(true);

	virtual void cnLib_FUNC Start(void)noexcept(true)override;

protected:
	iPtr<cDNetUIThread> fUIThread;
};
//---------------------------------------------------------------------------
class cDNetUIThreadAsyncProcedureRef : public iAsyncProcedure, public mcDNetUIThreadAsyncProcedureRefExecutor
{
public:
	cDNetUIThreadAsyncProcedureRef(cDNetUIThread *UIThread,bool HighPriority,iReference *Reference,iProcedure *Procedure)noexcept(true);
	~cDNetUIThreadAsyncProcedureRef()noexcept(true);

	virtual void cnLib_FUNC Start(void)noexcept(true)override;

protected:
	iPtr<cDNetUIThread> fUIThread;
};
//---------------------------------------------------------------------------
class cDNetUIThreadAsyncTimer : public iAsyncTimer, private mcDNetUIThreadAsyncTimerExecutor
{
public:
	cDNetUIThreadAsyncTimer(cDNetUIThread *UIDispatch,bool HighPriority,iReference *Reference,iProcedure *Procedure)noexcept(true);
	~cDNetUIThreadAsyncTimer()noexcept(true);

	virtual void cnLib_FUNC Start(uInt64 DueTime,uInt64 Period)noexcept(true)override;
	virtual void cnLib_FUNC Stop(void)noexcept(true)override;
		
protected:
	iPtr<cDNetUIThread> fUIDispatch;
};
//---------------------------------------------------------------------------
class cDNetUIThread : public iUIThread, public cnRTL::cDualReference, public mcDNetUIThreadDispatcher
{
protected:
	cDNetUIThread()noexcept(true);
	~cDNetUIThread()noexcept(true);

	void VirtualStopped(void)noexcept(true);
	
public:

	bool IsCurrent(void)noexcept(true);
	bool IsShutdown(void)noexcept(true);

	// iUIThread

	virtual bool cnLib_FUNC IsCurrentThread(void)noexcept(true)override;
	virtual iDispatch* cnLib_FUNC GetDispatch(bool HighPriority)noexcept(true)override;
	virtual iUIKeyboard* cnLib_FUNC GetDefaultKeyboard(void)noexcept(true)override;
	virtual iUIMouse* cnLib_FUNC GetDefaultMouse(void)noexcept(true)override;


	static cDNetUIThread* CurrentUIThread(void)noexcept(true);
	static iPtr<cDNetUIThread> CreateOnCurrentThread(void)noexcept(true);
	static iPtr<cDNetUIThread> StartUIThread(void)noexcept(true);


	void DispatchExecute(bool HighPriority,iReference *Reference,iProcedure *Procedure)noexcept(true);
	iPtr<iAsyncTask> DispatchExecuteAsync(bool HighPriority,iReference *Reference,iProcedure *Procedure)noexcept(true);


protected:

	class mcWPFDispatchHigh : public iDispatch
	{
	public:
		cDNetUIThread* GetHost(void)noexcept(true);
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
		cDNetUIThread* GetHost(void)noexcept(true);
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

	class cExecuteAsyncTask : public iAsyncTask, public mcDispatchExecuteAsyncTask
	{
	public:
		virtual bool cnLib_FUNC IsDone(void)noexcept(true)override;
		virtual bool cnLib_FUNC SetNotify(iProcedure *NotifyProcedure)noexcept(true)override;
		virtual void ExecuteTask(void)noexcept(true)override;
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
	cDNetUIApplication(iPtr<cDNetUIThread> Thread)noexcept(true);
	~cDNetUIApplication()noexcept(true);


	virtual iUIThread* cnLib_FUNC GetMainUIThread(void)noexcept(true)override;
	virtual void cnLib_FUNC UIMain(iFunction<void (iWindowsUISession*)noexcept(true)> *SessionHandler)noexcept(true)override;

private:
	iPtr<cDNetUIThread> fUIThread;

	class cWindowsUISession : public iWindowsUISession
	{
	public:
		cDNetUIApplication *Owner;
		cWindowsUISession()noexcept(true);
		~cWindowsUISession()noexcept(true);

		virtual void cnLib_FUNC Terminate(void)noexcept(true)override;
	};

};
//---------------------------------------------------------------------------
}	// namespace cnWin
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
