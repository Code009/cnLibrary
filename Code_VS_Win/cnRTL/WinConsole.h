/*- cnRTL - Windows - Console ---------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2021-08-17                                          */
/*-------------------------------------------------------------------------*/
#pragma once
/*-------------------------------------------------------------------------*/
#include <cnRTL\Win32Thread.h>

//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnRTL{
//---------------------------------------------------------------------------
namespace cnWinRTL{
//---------------------------------------------------------------------------
class cWin32DebugTextOutput : public iTextStreamOutput
{
public:
	cWin32DebugTextOutput()noexcept(true);
	~cWin32DebugTextOutput()noexcept(true);

	virtual uIntn cnLib_FUNC Print(const uChar16 *Text,uIntn Length)noexcept(true)override;
protected:

	arSharedObjectRecycler<cWin32CodePageConvertBuffer> fBufferRecycler;
};
//---------------------------------------------------------------------------
class cWin32ConsoleOutput : public iConsoleOutput
{
public:
	cWin32ConsoleOutput(HANDLE OutputHandle)noexcept(true);
	~cWin32ConsoleOutput()noexcept(true);

	HANDLE GetOutputHandle(void)const noexcept(true);

	virtual cUIPoint cnLib_FUNC GetPageSize(void)noexcept(true)override;

	virtual uIntn cnLib_FUNC Print(const uChar16 *Text,uIntn Length)noexcept(true)override;

	virtual cUIPoint cnLib_FUNC GetCursorPos(void)noexcept(true)override;
	virtual bool cnLib_FUNC SetCursorPos(cUIPoint Pos)noexcept(true)override;

	virtual bool cnLib_FUNC GetCursorVisible(void)noexcept(true)override;
	virtual bool cnLib_FUNC SetCursorVisible(bool Visible)noexcept(true)override;

	virtual void cnLib_FUNC MoveCursor(eDirection Direction,ufInt16 Count)noexcept(true)override;
	virtual void cnLib_FUNC MoveCursorLineHead(bool Up,ufInt16 Count)noexcept(true)override;

	virtual void cnLib_FUNC EraseLine(void)noexcept(true)override;
	virtual void cnLib_FUNC EraseLinePart(bool AfterCursor)noexcept(true)override;

protected:

	HANDLE fOutputHandle;
};
//---------------------------------------------------------------------------
class cWin32ConsoleInput : public iConsoleInput, public cDualReference
{
public:
	cWin32ConsoleInput(HANDLE InputHandle,iDispatch *Dispatch)noexcept(true);
	~cWin32ConsoleInput()noexcept(true);

	HANDLE GetInputHandle(void)const noexcept(true);

	virtual iDispatch* cnLib_FUNC GetDispatch(void)noexcept(true)override;
	virtual bool cnLib_FUNC StartHandle(iReference *Reference,iConsoleInputHandler *Handler)noexcept(true)override;
	virtual void cnLib_FUNC StopHandle(void)noexcept(true)override;

protected:
	void VirtualStarted(void)noexcept(true);
	void VirtualStopped(void)noexcept(true);

	HANDLE fInputHandle;
	rPtr<iReference> fHandlerReference;
	iConsoleInputHandler *fHandler=nullptr;
	iPtr<iDispatch> fDispatch;

	cStringBuffer<uChar16> fInputTextBuffer;

private:
	rPtr<iThreadPoolHandleWaiter> fConsoleInputWaiter;
	rPtr<iAsyncProcedure> fConsoleProcessWork;

	class cConsoleHandleWaitProcedure : public iFunction<void (DWORD)noexcept(true)>
	{
		virtual void cnLib_FUNC Execute(DWORD WaitResult)noexcept(true)override;
	}fConsoleHandleWaitProcedure;

	class cConsoleInputProcessProcedure : public iProcedure
	{
		virtual void cnLib_FUNC Execute(void)noexcept(true)override;
	}fConsoleInputProcessProcedure;
	
	cAtomicVar<bool> fHandleStartFlag=false;
	cAtomicVar<bool> fHandleStopFlag=false;
	cExclusiveFlag fInputThreadExclusiveFlag;

	bool fHandlerActive;
	void NotifyConsoleInputProcess(void)noexcept(true);
	void ConsoleInputProcessThread(void)noexcept(true);
	bool ConsoleInputProcess(void)noexcept(true);
	bool HandleInput(void)noexcept(true);
	void HandleInputRecordKey(iConsoleInputHandler *Handler,KEY_EVENT_RECORD &Record)noexcept(true);
	void HandleInputRecordMouse(iConsoleInputHandler *Handler,MOUSE_EVENT_RECORD &Record)noexcept(true);
	void HandleInputRecordWindowBufferSize(iConsoleInputHandler *Handler,WINDOW_BUFFER_SIZE_RECORD &Record)noexcept(true);


};
//---------------------------------------------------------------------------
class cWindowsConsoleHost : public iWindowsConsoleHost
{
public:
	cWindowsConsoleHost(HANDLE OutputHandle,HANDLE InputHandle,iDispatch *Dispatch)noexcept(true);
	~cWindowsConsoleHost()noexcept(true);

	virtual iConsoleOutput* cnLib_FUNC GetOutput(void)noexcept(true)override;
	virtual iConsoleInput* cnLib_FUNC GetInput(void)noexcept(true)override;
protected:
	rPtr<cWin32ConsoleOutput> fOutput;
	rPtr<cWin32ConsoleInput> fInput;
};
//---------------------------------------------------------------------------
rPtr<iWindowsConsoleHost> CreateWindowsConsoleHost(iDispatch *Dispatch)noexcept(true);
//---------------------------------------------------------------------------
}	// namespace cnWinRTL
//---------------------------------------------------------------------------
}	// namespace cnRTL
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
