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
iPtr<iWindowClient> DNetCreateWindowClient(void);
//---------------------------------------------------------------------------
class iWPFWindowClient
{
public:
	virtual iWindowClient* WPFWindowGetInterface(void)=0;
	virtual void WPFWindowStateChanged(eUIState NewState)=0;
	virtual void WPFWindowDPIChanged(Float32 NewScale)=0;
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

	virtual void WPFWindowStateChanged(eUIState NewState)=0;
private:
	friend cWPFUIWindow;

	mcWPFViewRoot(mcConstructParameter &Parameter);
	~mcWPFViewRoot();

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

	static void __clrcall DispatcherFinishNotify(mcDNetUIThreadDispatcher::cDispatcherFinishNotify *Notify,bool Shutdown);
	void __clrcall DispatcherFinishNotify(bool Shutdown);
	void __clrcall CleanupWPF(void);
	
#endif // _MANAGED

	void nDispatcherFinishNotify(bool Shutdown);

public:
#if _MANAGED
	System::Object^ mGetWPFRootObject(void);
#endif // _MANAGED
	
	iUIWindow* nGetUIWindowInterface(void);

	bool mSetClient(cGCRef &ObjectHandle);
	void mResetClient(void);

	// WPF Notification

#if _MANAGED
	void __clrcall WPFUIWindowRemoveSubview(DNet::IWPFView ^Subview);
#endif

};
//---------------------------------------------------------------------------
iPtr<iUIWindow> DNetCreateWPFWindowAsWindowClient(mcDNetUIThreadDispatcher *Dispatcher,mcWPFViewRoot::mcConstructParameter &Parameter);
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
void mWPFViewRoot_SetBackgroundColor(cGCHandle &ViewRoot,uInt8 r,uInt8 g,uInt8 b);
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
	mcWPFWindow();
	~mcWPFWindow();
public:
#if _MANAGED
	HWND __clrcall mGetHandle(void);

#endif // _MANAGED

	bool Create(HWND Parent,const wchar_t *WindowText,DWORD Style,DWORD ExStyle,LONG X,LONG Y,LONG Width,LONG Height);

	SIZE GetClientSize(void);


	static mcWPFWindow* mWindowAttachClient(cGCHandle &WindowCLIHandle,mcWPFViewRoot *ViewRoot,iWPFWindowClient *WindowClient);
	bool mWindowAttachClient(mcWPFViewRoot *ViewRoot,iWPFWindowClient *WindowClient);
	bool mWindowDetachClient(mcWPFViewRoot *ViewRoot);

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

	void __clrcall WPFSourceAttach(rcWPFWindow ^Window);
	void __clrcall WPFSourceDetach(void);
	System::IntPtr WPFSourceMessage(HWND hwnd, int msg, WPARAM wParam, LPARAM lParam, bool %handled);

	bool __clrcall ClientAttach(DNet::WPFViewRoot ^Client,iWPFWindowClient *WindowClient);
	void __clrcall ClearClient(void);

	static void __clrcall DispatcherFinishNotify(mcDNetUIThreadDispatcher::cDispatcherFinishNotify *Notify,bool Shutdown);
	void __clrcall DispatcherFinishNotify(bool Shutdown);
	void CleanupWPF(void);
	
#endif // _MANAGED

	void nWPFSourceAttach(void);
	void nWPFSourceDetach(void);
	bool nWPFMessage(LRESULT &Result,HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void nDispatcherFinishNotify(bool Shutdown);
};
//---------------------------------------------------------------------------
 

//---------------------------------------------------------------------------
iPtr<iWindowClient> DNetCreateWindowClient(mcDNetUIThreadDispatcher *Dispatcher,mcWPFViewRoot::mcConstructParameter &Parameter);

//---------------------------------------------------------------------------
}	// namespace cnWin
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
