/*- cnWinWPF - Window and View --------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2020-04-15                                          */
/*-------------------------------------------------------------------------*/
#pragma once
/*-------------------------------------------------------------------------*/
#if _MANAGED
#using <PresentationCore.dll>
#endif	/* _MANAGED */


#include <cnWinCLI\WinDNetM_Common.h>
#include <cnWinCLI\WinDNetM_WPFUICore.h>
#include <cnWinCLI\WinDNetM_WPFUIView.h>


#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
class iWindowClient;
//---------------------------------------------------------------------------
namespace cnWin{
//---------------------------------------------------------------------------
iPtr<iWindowClient> DNetCreateWindowClient(void)noexcept(true);
//---------------------------------------------------------------------------
class iWPFWindowClient
{
public:
	virtual iWindowClient* WPFWindowGetInterface(void)noexcept(true)=0;
	virtual void WPFWindowStateChanged(eUIState NewState)noexcept(true)=0;
	virtual void WPFWindowDPIChanged(Float32 NewScale)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class mcWPFViewRoot;
class mcWPFWindowClient;
//---------------------------------------------------------------------------
}	// namespace cnWin
//---------------------------------------------------------------------------
#if _MANAGED
//---------------------------------------------------------------------------
namespace DNet{
//---------------------------------------------------------------------------

private interface class IWPFViewRoot : IWPFViewParent
{
	void CPPAttach(cnWin::mcWPFViewRoot *CPP);
	void CPPDetach(cnWin::mcWPFViewRoot *CPP);

	property System::Windows::UIElement ^ClientElement{
		System::Windows::UIElement^ get();
		void set(System::Windows::UIElement ^Element);
	};

	void DispatcherFinishCleanup(void);

};
//---------------------------------------------------------------------------
}	// namespace DNet
//---------------------------------------------------------------------------
#endif // _MANAGED
//---------------------------------------------------------------------------
namespace cnWin{
//---------------------------------------------------------------------------
class cWPFUIWindow;
//---------------------------------------------------------------------------
class mcWPFViewRoot : public iWPFViewParent//, public iWPFWindowClient
{
public:
	struct mcConstructParameter
	{
#if !_MANAGED
		mcConstructParameter()=delete;
		~mcConstructParameter()=delete;
#endif // !_MANAGED

#if _MANAGED
		mcGCRefT<DNet::IWPFViewRoot> WPFRoot;
#else
		cGCRef WPFRoot;
#endif
	};

	virtual void WPFWindowStateChanged(eUIState NewState)noexcept(true)=0;
private:
	friend cWPFUIWindow;

	mcWPFViewRoot(mcConstructParameter &Parameter)noexcept(true);
	~mcWPFViewRoot()noexcept(true);

protected:
#if _MANAGED
	mcGCRefT<DNet::IWPFViewRoot> fWPFRoot;
	mcGCRefT<DNet::IWPFView> fWPFClient;
#else
	cGCRef fWPFRoot;
	cGCRef fWPFClient;
#endif // _MANAGED
	
	iWPFViewChild *fClient=nullptr;
	iUIView *fClientView;

private:
	
	mcDNetUIThreadDispatcher::cDispatcherFinishNotify fDispatcherFinishNotify;
#if _MANAGED

	static void __clrcall DispatcherFinishNotify(mcDNetUIThreadDispatcher::cDispatcherFinishNotify *Notify,bool Shutdown)noexcept(true);
	void __clrcall DispatcherFinishNotify(bool Shutdown)noexcept(true);
	void __clrcall CleanupWPF(void)noexcept(true);
	
#endif // _MANAGED

	void nDispatcherFinishNotify(bool Shutdown)noexcept(true);

public:
#if _MANAGED
	System::Object^ mGetWPFRootObject(void)noexcept(true);
#endif // _MANAGED
	
	iUIWindow* nGetUIWindowInterface(void)noexcept(true);

	bool mSetClient(cGCRef &ObjectHandle)noexcept(true);
	void mResetClient(void)noexcept(true);

	// WPF Notification

#if _MANAGED
	void __clrcall WPFUIWindowRemoveSubview(DNet::IWPFView ^Subview)noexcept(true);
#endif

};
//---------------------------------------------------------------------------
iPtr<iUIWindow> DNetCreateWPFWindowAsWindowClient(mcDNetUIThreadDispatcher *Dispatcher,mcWPFViewRoot::mcConstructParameter &Parameter)noexcept(true);
//---------------------------------------------------------------------------
}	// namespace cnWin
//---------------------------------------------------------------------------
#if _MANAGED
//---------------------------------------------------------------------------
namespace DNet{
//---------------------------------------------------------------------------
private ref class WPFViewRoot : public System::Windows::UIElement
	, public IWPFViewRoot, public IWPFUIWindowElement
{
public:
	WPFViewRoot();


	property System::Windows::Media::Color BackgroundColor{
		System::Windows::Media::Color get();
		void set(System::Windows::Media::Color c);
	};

internal:

	// IWPFViewParent

	virtual property cnWin::iWPFViewParent* ParentInterface{
		virtual cnWin::iWPFViewParent* get(void)=IWPFViewRoot::IWPFViewParent::Interface::get;
	};

	virtual void ParentRemoveViewChild(IWPFView ^Subview)=IWPFViewRoot::IWPFViewParent::RemoveViewChild;

	// IWPFUIWindowElement

	virtual property iUIWindow* UIWindowInterface{
		virtual iUIWindow* get(void)=IWPFUIWindowElement::UIWindowInterface::get;
	};
	virtual void RemoteChildAttach(System::Object^)=IWPFUIWindowElement::RemoteChildAttach;
	virtual void RemoteChildDetach(System::Object^)=IWPFUIWindowElement::RemoteChildDetach;

	// IWPFViewRoot

	virtual void CPPAttach(cnWin::mcWPFViewRoot *CPP)sealed=IWPFViewRoot::CPPAttach;
	virtual void CPPDetach(cnWin::mcWPFViewRoot *CPP)sealed=IWPFViewRoot::CPPDetach;

	property System::Windows::UIElement ^ClientElement{
		virtual System::Windows::UIElement^ get()=IWPFViewRoot::ClientElement::get;
		virtual void set(System::Windows::UIElement ^Element)=IWPFViewRoot::ClientElement::set;
	};

	virtual void DispatcherFinishCleanup(void)=IWPFViewRoot::DispatcherFinishCleanup;

protected:
	cnWin::mcWPFViewRoot *fCPP;

	System::Windows::Media::GeometryDrawing fBackgroundDrawing;
	System::Windows::Media::SolidColorBrush ^fBGBrush;
	System::Windows::UIElement ^fClientElement;
	System::Windows::Size fSize;

	virtual System::Windows::Media::Visual^ GetVisualChild(int index)override;
	virtual property int VisualChildrenCount{
		virtual int get(void)override;
	};
	virtual void OnRender(System::Windows::Media::DrawingContext^ drawingContext)override;
	virtual void ArrangeCore(System::Windows::Rect finalRect)override;

};
//---------------------------------------------------------------------------
}	// namespace DNet
//---------------------------------------------------------------------------
#endif // _MANAGED
//---------------------------------------------------------------------------
namespace cnWin{
//---------------------------------------------------------------------------
void mWPFViewRoot_SetBackgroundColor(cGCHandle &ViewRoot,uInt8 r,uInt8 g,uInt8 b)noexcept(true);
//---------------------------------------------------------------------------
class cWPFWindow;
class mcWPFWindow;

class mcWPFHwndSource;
//---------------------------------------------------------------------------
#if _MANAGED
//---------------------------------------------------------------------------
private ref class rcWPFWindow
{
public:
	rcWPFWindow(mcWPFWindow *CPP);

	property System::Windows::Interop::HwndSource ^Source{
		System::Windows::Interop::HwndSource^get(void){	return fSource;	}
	}
	
internal:
	mcWPFWindow *CPP;
	void Create(System::Windows::Interop::HwndSourceParameters %Parameter);
	void Destroy(void);

protected:
	System::Windows::Interop::HwndSource ^fSource;
	initonly System::Windows::Interop::HwndSourceHook ^fHook;


	System::IntPtr WindowMessageHook(System::IntPtr hwnd, int msg, System::IntPtr wParam, System::IntPtr lParam, bool %handled);
};
//---------------------------------------------------------------------------
#endif	// _MANAGED
//---------------------------------------------------------------------------
class mcWPFWindow
{
	friend cWPFWindow;
	mcWPFWindow()noexcept(true);
	~mcWPFWindow()noexcept(true);
public:
#if _MANAGED
	HWND __clrcall mGetHandle(void)noexcept(true);

#endif // _MANAGED

	bool Create(HWND Parent,const wchar_t *WindowText,DWORD Style,DWORD ExStyle,LONG X,LONG Y,LONG Width,LONG Height)noexcept(true);

	SIZE GetClientSize(void)noexcept(true);


	static mcWPFWindow* mWindowAttachClient(cGCHandle &WindowCLIHandle,mcWPFViewRoot *ViewRoot,iWPFWindowClient *WindowClient)noexcept(true);
	bool mWindowAttachClient(mcWPFViewRoot *ViewRoot,iWPFWindowClient *WindowClient)noexcept(true);
	bool mWindowDetachClient(mcWPFViewRoot *ViewRoot)noexcept(true);

protected:
#if _MANAGED
	mcGCRefT<rcWPFWindow> fWPFWindow;
	mcGCRefT<DNet::WPFViewRoot> fWPFViewRoot;
#else
	cGCRef fWPFWindow;
	cGCRef fWPFViewRoot;
#endif	// _MANAGED

	HWND fWindowHandle;
	Float32 fWindowLayoutScale;
	Float32 fWindowDPIScale;
	iWPFWindowClient *fWindowClient;

private:
	mcDNetUIThreadDispatcher::cDispatcherFinishNotify fDispatcherFinishNotify;
#if _MANAGED
	friend rcWPFWindow;

	void __clrcall WPFSourceAttach(rcWPFWindow ^Window)noexcept(true);
	void __clrcall WPFSourceDetach(void)noexcept(true);
	System::IntPtr WPFSourceMessage(HWND hwnd, int msg, WPARAM wParam, LPARAM lParam, bool %handled)noexcept(true);

	bool __clrcall ClientAttach(DNet::WPFViewRoot ^Client,iWPFWindowClient *WindowClient)noexcept(true);
	void __clrcall ClearClient(void)noexcept(true);

	static void __clrcall DispatcherFinishNotify(mcDNetUIThreadDispatcher::cDispatcherFinishNotify *Notify,bool Shutdown)noexcept(true);
	void __clrcall DispatcherFinishNotify(bool Shutdown)noexcept(true);
	void CleanupWPF(void)noexcept(true);
	
#endif // _MANAGED

	void nWPFSourceAttach(void)noexcept(true);
	void nWPFSourceDetach(void)noexcept(true);
	bool nWPFMessage(LRESULT &Result,HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)noexcept(true);
	void nDispatcherFinishNotify(bool Shutdown)noexcept(true);
};
//---------------------------------------------------------------------------
 

//---------------------------------------------------------------------------
iPtr<iWindowClient> DNetCreateWindowClient(mcDNetUIThreadDispatcher *Dispatcher,mcWPFViewRoot::mcConstructParameter &Parameter)noexcept(true);

//---------------------------------------------------------------------------
}	// namespace cnWin
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
