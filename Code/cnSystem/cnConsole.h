/*- cnSystem - Console Interface ------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2021-08-17                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnSystem_cnConsole_H__
#define	__cnLibrary_cnSystem_cnConsole_H__
/*-------------------------------------------------------------------------*/
#include <cnSystem/cnFundamental.h>
#include <cnSystem/cnThread.h>
#include <cnSystem/cnUI.h>
/*-------------------------------------------------------------------------*/
#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
class cnLib_INTERFACE iTextStreamOutput : public iReference
{
public:
	virtual uIntn cnLib_FUNC Print(const uChar16 *Text,uIntn Length)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iConsoleOutput : public iTextStreamOutput
{
public:
	virtual cUIPoint cnLib_FUNC GetPageSize(void)noexcept(true)=0;

	virtual cUIPoint cnLib_FUNC GetCursorPos(void)noexcept(true)=0;
	virtual bool cnLib_FUNC SetCursorPos(cUIPoint Pos)noexcept(true)=0;

	virtual bool cnLib_FUNC GetCursorVisible(void)noexcept(true)=0;
	virtual bool cnLib_FUNC SetCursorVisible(bool Visible)noexcept(true)=0;

	virtual void cnLib_FUNC MoveCursor(eDirection Direction,ufInt16 Count)noexcept(true)=0;
	virtual void cnLib_FUNC MoveCursorLineHead(bool Up,ufInt16 Count)noexcept(true)=0;

	virtual void cnLib_FUNC EraseLine(void)noexcept(true)=0;
	virtual void cnLib_FUNC EraseLinePart(bool AfterCursor)noexcept(true)=0;

	//virtual void cnLib_FUNC ErasePage(void)noexcept(true)=0;
	//virtual void cnLib_FUNC ErasePageText(bool AfterCursor)noexcept(true)=0;

};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iConsoleInputHandler
{
public:
	virtual void cnLib_FUNC ConsoleStarted(void)noexcept(true)=0;
	virtual void cnLib_FUNC ConsoleStopped(void)noexcept(true)=0;
	virtual void cnLib_FUNC KeyInput(eKeyCode KeyCode)noexcept(true)=0;
	virtual void cnLib_FUNC TextInput(const uChar16 *Text,uIntn Length)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iConsoleInput : public iReference
{
public:
	virtual iDispatch* cnLib_FUNC GetDispatch(void)noexcept(true)=0;
	// StartHandle
	//	start handle console input and calls ConsoleStarted in dispatch
	// Callback		callback
	// return true if successful registered callback
	virtual bool cnLib_FUNC StartHandle(iReference *Reference,iConsoleInputHandler *Handler)noexcept(true)=0;
	// StopHandle
	//	stop handle console iput, and post ConsoleStopped at the end of dispatch
	virtual void cnLib_FUNC StopHandle(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif
