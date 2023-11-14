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
	virtual uIntn cnLib_FUNC Print(const uChar16 *Text,uIntn Length)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iConsoleOutput : public iTextStreamOutput
{
public:
	virtual cUIPoint cnLib_FUNC GetPageSize(void)=0;

	virtual cUIPoint cnLib_FUNC GetCursorPos(void)=0;
	virtual bool cnLib_FUNC SetCursorPos(cUIPoint Pos)=0;

	virtual bool cnLib_FUNC GetCursorVisible(void)=0;
	virtual bool cnLib_FUNC SetCursorVisible(bool Visible)=0;

	virtual void cnLib_FUNC MoveCursor(eDirection Direction,ufInt16 Count)=0;
	virtual void cnLib_FUNC MoveCursorLineHead(bool Up,ufInt16 Count)=0;

	virtual void cnLib_FUNC EraseLine(void)=0;
	virtual void cnLib_FUNC EraseLinePart(bool AfterCursor)=0;

	//virtual void cnLib_FUNC ErasePage(void)=0;
	//virtual void cnLib_FUNC ErasePageText(bool AfterCursor)=0;

};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iConsoleInputHandler
{
public:
	virtual void cnLib_FUNC OnClose(void)=0;
	virtual void cnLib_FUNC KeyInput(eKeyCode KeyCode)=0;
	virtual void cnLib_FUNC TextInput(const uChar16 *Text,uIntn Length)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iConsoleInput : public iReference
{
public:
	virtual iConsoleInputHandler* cnLib_FUNC GetHandler(void)=0;
	virtual void cnLib_FUNC SetHandler(iConsoleInputHandler *Handler)=0;

	virtual void cnLib_FUNC Exit(void)=0;
};
//---------------------------------------------------------------------------
namespace cnSystem{
//---------------------------------------------------------------------------
extern iTextStreamOutput*const DebugLogStream;
//---------------------------------------------------------------------------
}   // namespace cnSystem
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif
