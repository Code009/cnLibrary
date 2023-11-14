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
	cWin32DebugTextOutput();
	~cWin32DebugTextOutput();

	virtual uIntn cnLib_FUNC Print(const uChar16 *Text,uIntn Length)override;
protected:

	arSharedObjectRecycler<cWin32CodePageConvertBuffer> fBufferRecycler;
};
//---------------------------------------------------------------------------
class cWin32ConsoleOutput : public iConsoleOutput
{
public:
	cWin32ConsoleOutput();
	cWin32ConsoleOutput(HANDLE OutputHandle);
	~cWin32ConsoleOutput();

	HANDLE GetOutputHandle(void)const;

	virtual cUIPoint cnLib_FUNC GetPageSize(void)override;

	virtual uIntn cnLib_FUNC Print(const uChar16 *Text,uIntn Length)override;

	virtual cUIPoint cnLib_FUNC GetCursorPos(void)override;
	virtual bool cnLib_FUNC SetCursorPos(cUIPoint Pos)override;

	virtual bool cnLib_FUNC GetCursorVisible(void)override;
	virtual bool cnLib_FUNC SetCursorVisible(bool Visible)override;

	virtual void cnLib_FUNC MoveCursor(eDirection Direction,ufInt16 Count)override;
	virtual void cnLib_FUNC MoveCursorLineHead(bool Up,ufInt16 Count)override;

	virtual void cnLib_FUNC EraseLine(void)override;
	virtual void cnLib_FUNC EraseLinePart(bool AfterCursor)override;

protected:

	HANDLE fOutputHandle;
};
//---------------------------------------------------------------------------
class cWin32ConsoleInput : public iConsoleInput
{
public:
	cWin32ConsoleInput();
	cWin32ConsoleInput(HANDLE InputHandle);
	~cWin32ConsoleInput();

	HANDLE GetInputHandle(void)const;

	// NotifyClose
	//	notify input needs to close.
	//	usually called when user press CTRL+C/CTRL+Break, or closing application
	void NotifyClose(void);

	virtual iConsoleInputHandler* cnLib_FUNC GetHandler(void)override;
	virtual void cnLib_FUNC SetHandler(iConsoleInputHandler *Handler)override;

	virtual void cnLib_FUNC Exit(void)override;

	bool ConsoleInputProcess(void);

protected:

	HANDLE fInputHandle;
	HANDLE fExitEvent;
	iConsoleInputHandler *fHandler=nullptr;

	cStringBuffer<uChar16> fInputTextBuffer;

	//cExclusiveFlag fInputThreadExclusiveFlag;
	//void ActivateInputThread(void);
	//void ConsoleInputThread(void);
	//static DWORD WINAPI ConsoleInputThreadProc(LPVOID Parameter);

	void HandleInputRecordKey(iConsoleInputHandler *Handler,KEY_EVENT_RECORD &Record);
	void HandleInputRecordMouse(iConsoleInputHandler *Handler,MOUSE_EVENT_RECORD &Record);
	void HandleInputRecordWindowBufferSize(iConsoleInputHandler *Handler,WINDOW_BUFFER_SIZE_RECORD &Record);
};
//---------------------------------------------------------------------------
}	// namespace cnWinRTL
//---------------------------------------------------------------------------
}	// namespace cnRTL
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
