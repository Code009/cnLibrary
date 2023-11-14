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
	virtual void SubclassAttached(HWND WindowHandle)=0;
	virtual void SubclassDetached(void)=0;
	virtual LRESULT CALLBACK SubclassMessage(UINT Code,WPARAM wParam,LPARAM lParam)=0;

	class iProc{
	public:
		virtual LRESULT Execute(HWND Window,UINT Message,WPARAM wParam,LPARAM lParam)=0;
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
	LRESULT Execute(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)override;
};
//---------------------------------------------------------------------------
// LocalSubclass
//	subclass a window with the local class
bool SetLocal(HWND WindowHandle,bcWindowSubclass *Subclass);
// Subclass
//	subclass a window
bool Set(HWND WindowHandle,bcWindowSubclass *Subclass);
// SubclassRestore
//	restore subclassed window
void Restore(HWND WindowHandle,bcWindowSubclass *Subclass);
//---------------------------------------------------------------------------
}	// namespace NTXPWindowSubclass
//---------------------------------------------------------------------------
#endif	// _WIN32_WINNT >= _WIN32_WINNT_WINXP
//---------------------------------------------------------------------------
class cWindowUIState
{
public:
	cWindowUIState();
	~cWindowUIState();

	eUIState GetUIState(void)const;

	// Window Handle Notification

	LONG WindowAttached(HWND WindowHandle);
	void WindowDetached(void);
	bool WindowMessageCheckState(const cWindowMessageParam &MsgParam);
	void AfterChildWindowCreated(HWND WindowHandle);

protected:

	bool fWindowValid		:1;
	bool fWindowVisible		:1;
	bool fWindowActive		:1;

};
//---------------------------------------------------------------------------
int GetWindowDPI(HWND hWnd);
//---------------------------------------------------------------------------
HWND GetWindowHandleFromUIWindow(iUIArea *Area);
HWND GetWindowHandleFromUIView(iUIView *View);
iWindow* GetWindowFromUIWindow(iUIArea *Area);
iWindow* GetWindowFromUIView(iUIView *View);
//---------------------------------------------------------------------------
}	// namespace cnWinRTL
//---------------------------------------------------------------------------
}	// namespace cnRTL
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
