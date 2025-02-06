/*- cnRTL - Windows - Window ----------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2018-06-28                                          */
/*-------------------------------------------------------------------------*/
#pragma once
/*-------------------------------------------------------------------------*/
#include <cnRTL\WinCommon.h>

#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnRTL{
//---------------------------------------------------------------------------
namespace cnWinRTL{
//---------------------------------------------------------------------------
class bcWindowClass
{
public:
	operator LPCWSTR ()noexcept(true);
protected:
	// Window Class atom
	ATOM fAtom;
};
//---------------------------------------------------------------------------
class cWindowClass : public bcWindowClass
{
public:

	cWindowClass(const WNDCLASSEXW *ClassInfo);
	cWindowClass(HINSTANCE Module,const wchar_t *ClassName,WNDPROC WndClassProc,UINT style=0);
	~cWindowClass();
private:
	HINSTANCE fModuleHandle;
};
//---------------------------------------------------------------------------
class bcWindowSubclass
{
public:
	virtual void SubclassAttached(HWND WindowHandle)noexcept(true)=0;
	virtual void SubclassDetached(void)noexcept(true)=0;
	virtual LRESULT CALLBACK SubclassMessage(UINT Code,WPARAM wParam,LPARAM lParam)noexcept(true)=0;

	class iProc{
	public:
		virtual LRESULT Execute(HWND Window,UINT Message,WPARAM wParam,LPARAM lParam)noexcept(true)=0;
	};

	cnVar::cPolymorphicInterface<iProc,sizeof(uIntn)*2,uIntn> DefaultWindowProcedure;

};
//---------------------------------------------------------------------------
#if _WIN32_WINNT >= _WIN32_WINNT_WINXP
//---------------------------------------------------------------------------
namespace NTXPWindowSubclass{
//---------------------------------------------------------------------------
class cDefaultProcedureCaller : public bcWindowSubclass::iProc
{
public:
	LRESULT Execute(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)noexcept(true)override;
};
//---------------------------------------------------------------------------
// LocalSubclass
//	subclass a window with the local class
bool SetLocal(HWND WindowHandle,bcWindowSubclass *Subclass)noexcept(true);
// Subclass
//	subclass a window
bool Set(HWND WindowHandle,bcWindowSubclass *Subclass)noexcept(true);
// SubclassRestore
//	restore subclassed window
void Restore(HWND WindowHandle,bcWindowSubclass *Subclass)noexcept(true);
//---------------------------------------------------------------------------
}	// namespace NTXPWindowSubclass
//---------------------------------------------------------------------------
#endif	// _WIN32_WINNT >= _WIN32_WINNT_WINXP
//---------------------------------------------------------------------------
class cWindowUIState
{
public:
	cWindowUIState()noexcept(true);
	~cWindowUIState()noexcept(true);

	eUIState GetUIState(void)const noexcept(true);

	// Window Handle Notification

	LONG WindowAttached(HWND WindowHandle)noexcept(true);
	void WindowDetached(void)noexcept(true);
	bool WindowMessageCheckState(const cWindowMessageParam &MsgParam)noexcept(true);
	void AfterChildWindowCreated(HWND WindowHandle)noexcept(true);

protected:

	bool fWindowValid		:1;
	bool fWindowVisible		:1;
	bool fWindowActive		:1;

};
//---------------------------------------------------------------------------
int GetWindowDPI(HWND hWnd)noexcept(true);
//---------------------------------------------------------------------------
HWND GetWindowHandleFromUIWindow(iUIArea *Area)noexcept(true);
HWND GetWindowHandleFromUIView(iUIView *View)noexcept(true);
iWindow* GetWindowFromUIWindow(iUIArea *Area)noexcept(true);
iWindow* GetWindowFromUIView(iUIView *View)noexcept(true);
//---------------------------------------------------------------------------
class cWindowMessageThread
{
public:
	cWindowMessageThread()noexcept(true);
	~cWindowMessageThread()noexcept(true);

	void Setup(HWND MessageWindow,DWORD MessageThreadID)noexcept(true);
	void Clear(void)noexcept(true);

	HWND GetMessageWindow(void)const noexcept(true);

	void PostQuitMessageLoop(int ExitCode)noexcept(true);
	static int MessageLoop(void)noexcept(true);

	void SendNoop(void)noexcept(true);
	void PostNoop(void)noexcept(true);
	void Execute(iReference *Reference,iProcedure *Procedure)noexcept(true);
	void ExecuteNoRef(iProcedure *Procedure)noexcept(true);
	void ExecuteSync(iProcedure *Procedure)noexcept(true);
	bool IsCurrentThread(void)const noexcept(true);

	bool SetTimer(iProcedure *Procedure,UINT Interval)noexcept(true);
	bool KillTimer(iProcedure *Procedure)noexcept(true);


	void SendFunction(void *Parameter,void (*Function)(void *Parameter)noexcept(true))noexcept(true);
	void PostFunction(void *Parameter,void (*Function)(void *Parameter)noexcept(true))noexcept(true);

	static bool MessageWindowProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)noexcept(true);
protected:

	DWORD fMessageThreadID;
	HWND fMessageWindow;

	static const UINT Message_Noop;
	static const UINT Message_Function;
	static const UINT Message_Execute;
	static const UINT Message_ExecuteRef;
	static const UINT Message_QuitLoop;
};
//---------------------------------------------------------------------------
class cWindowMessageQueueDispatch : public iDispatch
{
public:
	cWindowMessageQueueDispatch(aClsRef<cWindowMessageThread> MessageThread)noexcept(true);
	~cWindowMessageQueueDispatch()noexcept(true);

	virtual void cnLib_FUNC Execute(iReference *Reference,iProcedure *Procedure)noexcept(true)override;
	virtual rPtr<iAsyncProcedure>	cnLib_FUNC CreateWork(iReference *Reference,iProcedure *ThreadProcedure)noexcept(true)override;
	virtual rPtr<iAsyncTimer>		cnLib_FUNC CreateTimer(iReference *Reference,iProcedure *ThreadProcedure)noexcept(true)override;
	virtual bool cnLib_FUNC IsCurrentThread(void)noexcept(true)override;
	virtual void cnLib_FUNC ExecuteSync(iProcedure *Procedure)noexcept(true)override;
	virtual iPtr<iAsyncTask> cnLib_FUNC ExecuteAsync(iReference *Reference,iProcedure *Procedure)noexcept(true)override;

protected:
	aClsRef<cWindowMessageThread> fMessageThread;

	class cAsyncProcedure : public iAsyncProcedure
	{
	public:
		cAsyncProcedure(aClsRef<cWindowMessageThread> MessageThread,iReference *Reference,iProcedure *Procedure)noexcept(true);

		virtual void cnLib_FUNC Start(void)noexcept(true)override;

	protected:
		aClsRef<cWindowMessageThread> fMessageThread;

		iReference *fReference;
		iProcedure *fProcedure;
	};

	class cAsyncProcedureNoRef : public iAsyncProcedure
	{
	public:
		cAsyncProcedureNoRef(aClsRef<cWindowMessageThread> MessageThread,iProcedure *Procedure)noexcept(true);

		virtual void cnLib_FUNC Start(void)noexcept(true)override;

	protected:
		aClsRef<cWindowMessageThread> fMessageThread;
		iProcedure *fProcedure;
	};

	
	class cAsyncTimer : public iAsyncTimer, public cDualReference
	{
	public:
		cAsyncTimer(aClsRef<cWindowMessageThread> MessageThread,iReference *Reference,iProcedure *Procedure)noexcept(true);
		~cAsyncTimer()noexcept(true);


		virtual void cnLib_FUNC Start(uInt64 DueTime,uInt64 Period)noexcept(true)override;
		virtual void cnLib_FUNC Stop(void)noexcept(true)override;


	protected:
		void VirtualStarted(void)noexcept(true);
		void VirtualStopped(void)noexcept(true);

		aClsRef<cWindowMessageThread> fMessageThread;

		iReference *fReference;
		iProcedure *fProcedure;

	private:
		cAtomicVar<ufInt8> fTimerState;
		rPtr<iAsyncTimer> fThreadPoolTimer;
		uInt64 fDueTime;
		uInt64 fPeriod;
		void StartTimer(uInt64 DueTime,uInt64 Period)noexcept(true);
		void ClearTimer(void)noexcept(true);
		void TimerHit(void)noexcept(true);
		void ThreadPoolTimerHit(void)noexcept(true);
		class cThreadPoolTimerProcedure : public iProcedure
		{
			virtual void cnLib_FUNC Execute(void)noexcept(true)override;
		}fThreadPoolTimerProcedure;
		class cTimerDispatchProcedure : public iProcedure
		{
			virtual void cnLib_FUNC Execute(void)noexcept(true)override;
		}fTimerDispatchProcedure;
	};

};
//---------------------------------------------------------------------------
class cWindowMessageUIThread : public iUIThread
{
public:
	cWindowMessageUIThread(aClsRef<cWindowMessageThread> MessageThread)noexcept(true);
	~cWindowMessageUIThread()noexcept(true);

	virtual bool cnLib_FUNC IsCurrentThread(void)noexcept(true)override;
	virtual iDispatch* cnLib_FUNC GetDispatch(bool HighPriority)noexcept(true)override;
	virtual iUIKeyboard* cnLib_FUNC GetDefaultKeyboard(void)noexcept(true)override;
	virtual iUIMouse* cnLib_FUNC GetDefaultMouse(void)noexcept(true)override;

	aCls<cWindowMessageThread>* GetMessageThread(void)const noexcept(true);
protected:
	aClsRef<cWindowMessageThread> fMessageThread;
	iPtr<cWindowMessageQueueDispatch> fDispatch;
};
//---------------------------------------------------------------------------
class cWindowMessageUIApplication : public iWindowsUIApplication
{
public:
	cWindowMessageUIApplication(iPtr<cWindowMessageUIThread> Thread)noexcept(true);
	~cWindowMessageUIApplication()noexcept(true);

	virtual iUIThread* cnLib_FUNC GetMainUIThread(void)noexcept(true)override;
	virtual bool cnLib_FUNC InsertHandler(iWindowsUISessionHandler *SessionHandler)noexcept(true)override;
	virtual bool cnLib_FUNC RemoveHandler(iWindowsUISessionHandler *SessionHandler)noexcept(true)override;
	virtual void cnLib_FUNC UIMain(void)noexcept(true)override;
	virtual void cnLib_FUNC CloseUISession(void)noexcept(true)override;

private:
	iPtr<cWindowMessageUIThread> fUIThread;
	cnRTL::cSeqSet<iWindowsUISessionHandler*> fHandlers;
	bool fUISession=false;
	bool fUITerminate=false;
};
//---------------------------------------------------------------------------
}	// namespace cnWinRTL
//---------------------------------------------------------------------------
}	// namespace cnRTL
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
