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
	cWPFHwndSourceAsWindow(cDNetUIThread *UIThread);
	~cWPFHwndSourceAsWindow();

	HWND GetHandle(void);


	virtual void* cnLib_FUNC CastInterface(iTypeID IID)noexcept(true) override;

	// iWindow

	virtual iUIThread* cnLib_FUNC GetUIThread(void)override;
	virtual HWND cnLib_FUNC GetWindowHandle(void)override;
	virtual bool cnLib_FUNC InsertMessageHandler(iWindowMessageHandler *WindowHandler,sfInt8 Order)override;
	virtual bool cnLib_FUNC RemoveMessageHandler(iWindowMessageHandler *WindowHandler)override;
	virtual iWindowClient* GetClient(void)override;
	virtual iWindowFrame* GetFrame(void)override;
	virtual rPtr<iVariable> QueryAffixedVariable(const void *Token)override;

	cDNetUIThread* GetWPFUIThread(void);
	eUIState GetUIState(void)const;
	Float32 GetLayoutScale(void)const;
	Float32 GetContentScale(void)const;

protected:
	void VirtualStarted(void)noexcept(true);
	void VirtualStopped(void)noexcept(true);


	bool CheckThread(void);
	iPtr<cDNetUIThread> fUIThread;

	cnRTL::cnWinRTL::cWindowUIState fWindowState;
	WORD fWindowDPI;

	cnRTL::cOrderedObjectSet<iWindowMessageHandler*,sfInt8> fHandlers;

	virtual void WindowAttach(void)override;
	virtual void WindowDetach(void)override;
	virtual bool WindowMessage(LRESULT &Result,HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)override;
	virtual void WindowShutdownFinished(bool Shutdown)override;
private:
	

	virtual rPtr< iArrayReference<const uChar16> > cnLib_FUNC DependentCreateDescription(void)override;
	virtual void cnLib_FUNC DependentShutdownNotification(void)override;

};
//---------------------------------------------------------------------------
}	// namespace cnWin
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
