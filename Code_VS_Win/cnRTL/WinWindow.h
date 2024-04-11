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
}	// namespace cnWinRTL
//---------------------------------------------------------------------------
}	// namespace cnRTL
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
