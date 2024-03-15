/*- cnWinWPF - Managed - WPF Provided Component ---------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2023-11-04                                          */
/*-------------------------------------------------------------------------*/
#pragma once
/*-------------------------------------------------------------------------*/
#if _MANAGED
#using <PresentationCore.dll>
#endif	/* _MANAGED */

#include <cnWinCLI\WinDNetM_WPFUICore.h>

#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
class iWindow;
//---------------------------------------------------------------------------
namespace cnWin{
//---------------------------------------------------------------------------
class mcWPFHwndSource;
//---------------------------------------------------------------------------
#if _MANAGED
//---------------------------------------------------------------------------
private ref class rcWPFHwndSource sealed
{
public:
	rcWPFHwndSource(mcWPFHwndSource *CPP);

	property System::Windows::Interop::HwndSource ^Source{
		System::Windows::Interop::HwndSource^get(void);
		void set(System::Windows::Interop::HwndSource ^Source);
	}

private:
	System::Windows::Interop::HwndSource ^fSource;
	System::Windows::Interop::HwndSourceHook ^fHook;
	mcWPFHwndSource *fCPP;

	System::IntPtr WindowMessageHook(System::IntPtr hwnd, int msg, System::IntPtr wParam, System::IntPtr lParam, bool %handled);

};
//---------------------------------------------------------------------------
#endif	// _MANAGED
//---------------------------------------------------------------------------
class mcWPFHwndSource
{
public:
	mcWPFHwndSource();
	~mcWPFHwndSource();
protected:
#if _MANAGED
	mcGCRefT<rcWPFHwndSource> fWPFHwnd;
#else
	cGCRef fWPFHwnd;
#endif	// _MANAGED

	HWND fWindowHandle;
	Float32 fWindowLayoutScale;
	Float32 fWindowDPIScale;

	virtual void WindowAttach(void)=0;
	virtual void WindowDetach(void)=0;
	virtual bool WindowMessage(LRESULT &Result,HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)=0;
	virtual void WindowShutdownFinished(bool Shutdown)=0;

private:
#if _MANAGED
	friend rcWPFHwndSource;

	void __clrcall WPFSourceAttach(rcWPFHwndSource ^WPFHwnd);
	void __clrcall WPFSourceDetach(void);
	System::IntPtr WPFSourceMessage(HWND hwnd, int msg, WPARAM wParam, LPARAM lParam, bool %handled);
	void __clrcall WPFDispatcherFinishNotify(bool Shutdown);


#endif // _MANAGED

};
//---------------------------------------------------------------------------
class cWPFHwndSourceAsWindow;
//---------------------------------------------------------------------------
class mcWPFHwndSourceAsWindow : public mcWPFHwndSource
{
	friend cWPFHwndSourceAsWindow;

	mcWPFHwndSourceAsWindow();
	~mcWPFHwndSourceAsWindow();

protected:


private:
#if _MANAGED

#endif // _MANAGED
};
//---------------------------------------------------------------------------
iPtr<iWindow> DNetCreateWPFHwndSourceAsWindow(mcDNetUIThreadDispatcher *Dispatcher);
//---------------------------------------------------------------------------
}	// namespace cnWin
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
