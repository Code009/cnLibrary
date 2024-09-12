/*- cnWin - WPF - WPF Provided Component ----------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2023-10-07                                          */
/*-------------------------------------------------------------------------*/
#pragma once
/*-------------------------------------------------------------------------*/

#include <cnWinCLI\WinDNet_Common.h>
#include <cnWinCLI\WinDNet_WPFUICore.h>
#include <cnWinCLI\WinDNetM_WPFComponent.h>

#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnWin{
//---------------------------------------------------------------------------

// cWPFHwndSourceAsWindow
//	fixed iWindowClient by RootVisual
class cWPFHwndSourceAsWindow
	: public iWindow
	, private iDependentInfo, public cnRTL::bcVirtualLifeCycle
	, public mcWPFHwndSourceAsWindow
{
	friend mcWPFHwndSourceAsWindow;
public:
	cWPFHwndSourceAsWindow(cDNetUIThread *UIThread)noexcept(true);
	~cWPFHwndSourceAsWindow()noexcept(true);

	HWND GetHandle(void)noexcept(true);


	virtual void* cnLib_FUNC CastInterface(iTypeID IID)noexcept(true) override;

	// iWindow

	virtual iUIThread* cnLib_FUNC GetUIThread(void)noexcept(true)override;
	virtual HWND cnLib_FUNC GetWindowHandle(void)noexcept(true)override;
	virtual bool cnLib_FUNC InsertMessageHandler(iWindowMessageHandler *WindowHandler,sfInt8 Order)noexcept(true)override;
	virtual bool cnLib_FUNC RemoveMessageHandler(iWindowMessageHandler *WindowHandler)noexcept(true)override;
	virtual iWindowClient* GetClient(void)noexcept(true)override;
	virtual bool SetClient(iWindowClient*)noexcept(true)override{return false;}
	virtual iWindowFrame* GetFrame(void)noexcept(true)override;
	virtual bool SetFrame(iWindowFrame*)noexcept(true)override{return false;}
	virtual rPtr<iVariable> QueryAffixedVariable(const void *Token)noexcept(true)override;
	virtual bool cnLib_FUNC GetMouseAutoCapture(void)noexcept(true)override{return false;}
	virtual bool cnLib_FUNC SetMouseAutoCapture(bool Enable)noexcept(true)override{return false;}

	cDNetUIThread* GetWPFUIThread(void)noexcept(true);
	eUIState GetUIState(void)const noexcept(true);
	Float32 GetLayoutScale(void)const noexcept(true);
	Float32 GetContentScale(void)const noexcept(true);

protected:
	void VirtualStarted(void)noexcept(true);
	void VirtualStopped(void)noexcept(true);


	bool CheckThread(void)noexcept(true);
	iPtr<cDNetUIThread> fUIThread;

	cnRTL::cnWinRTL::cWindowUIState fWindowState;
	WORD fWindowDPI;

	cnRTL::cOrderedObjectSet<iWindowMessageHandler*,sfInt8> fHandlers;

	virtual void WindowAttach(void)noexcept(true)override;
	virtual void WindowDetach(void)noexcept(true)override;
	virtual bool WindowMessage(LRESULT &Result,HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)noexcept(true)override;
	virtual void WindowShutdownFinished(bool Shutdown)noexcept(true)override;
private:
	

	virtual rPtr<iStringReference> cnLib_FUNC DependentCreateDescription(void)noexcept(true)override;
	virtual void cnLib_FUNC DependentShutdownNotification(void)noexcept(true)override;

};
//---------------------------------------------------------------------------
}	// namespace cnWin
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
