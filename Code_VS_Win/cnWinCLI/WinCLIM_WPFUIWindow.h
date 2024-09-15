/*- cnWinCLI - Managed - WPF Window ---------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2020-04-15                                          */
/*-------------------------------------------------------------------------*/
#pragma once
/*-------------------------------------------------------------------------*/
#if _MANAGED
#using <PresentationCore.dll>
#endif	/* _MANAGED */

#include <cnWinCLI\WinCLIM_WPFUIView.h>

#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
class iWindowClient;
//---------------------------------------------------------------------------
namespace cnWin{
//---------------------------------------------------------------------------
#if _MANAGED
//---------------------------------------------------------------------------
private ref class rcNativeCaller_WPFUIWindow sealed
{
public:
	rcNativeCaller_WPFUIWindow(void *CPP);
	
	void mbcWPFUIViewRoot_ShutdownStarted(System::Object ^,System::EventArgs ^e);
private:
	void *CPP;
};
//---------------------------------------------------------------------------
#endif	// _MANAGED
//---------------------------------------------------------------------------
class mbcWPFUIViewRoot;
class mcWPFWindowClient;
//---------------------------------------------------------------------------
}	// namespace cnWin
//---------------------------------------------------------------------------
#if _MANAGED
//---------------------------------------------------------------------------
namespace DNet{
//---------------------------------------------------------------------------
value class WPFUIViewRootAdapter;
//---------------------------------------------------------------------------
public interface class IWPFUIViewRootTargetElement
{
	property WPFUIViewRootAdapter %Adapter{
		WPFUIViewRootAdapter% get(void);
	}

	property System::Windows::UIElement ^ClientElement{
		System::Windows::UIElement^ get();
		void set(System::Windows::UIElement ^Element);
	};

	property IWPFUIViewRootTargetElement ^OwnerTargetElement{
		IWPFUIViewRootTargetElement^ get();
		void set(IWPFUIViewRootTargetElement ^Owner);
	}

	property bool IsActive{
		bool get();
	}

	bool SetBackgroundColor(System::Windows::Media::Color Color);
};
//---------------------------------------------------------------------------
public value class WPFUIViewRootAdapter
{
public:
	void CPPAttach(cnWin::mbcWPFUIViewRoot *CPP);
	void CPPDetach(cnWin::mbcWPFUIViewRoot *CPP);

	void RemoteChildAttach(IWPFUIViewTargetElement ^RemoteChild);
	void RemoteChildDetach(IWPFUIViewTargetElement ^RemoteChild);

	void OnActiveChanged(void);

internal:
	cnWin::mbcWPFUIViewRoot *CPP;

private:
	cnWin::rcNativeCaller_WPFUIWindow ^NativeCaller;

	static cnWin::mbcWPFUIViewRoot* CPPFromSender(System::Object ^sender);
	static void OnIsVisibleChanged(System::Object ^sender,System::Windows::DependencyPropertyChangedEventArgs e);
};
//---------------------------------------------------------------------------
}	// namespace DNet
//---------------------------------------------------------------------------
#endif // _MANAGED
//---------------------------------------------------------------------------
namespace cnWin{
//---------------------------------------------------------------------------
#if _MANAGED
private ref class rcWPFUIViewRootTargetElementAsRemoteRoot sealed : riWPFUIViewRemoteRoot
{
public:
	rcWPFUIViewRootTargetElementAsRemoteRoot(DNet::IWPFUIViewRootTargetElement ^Target);

private:
	DNet::IWPFUIViewRootTargetElement ^fTarget;
	virtual void RemoteChildAttach(DNet::IWPFUIViewTargetElement ^RemoteChild)sealed=riWPFUIViewRemoteRoot::RemoteChildAttach;
	virtual void RemoteChildDetach(DNet::IWPFUIViewTargetElement ^RemoteChild)sealed=riWPFUIViewRemoteRoot::RemoteChildDetach;
};
#endif // _MANAGED
//---------------------------------------------------------------------------
class mbcWPFUIViewRoot
{
public:
	const cGCHandle& GetTargetElementHandle(void)const noexcept(true);


#if _MANAGED
	DNet::IWPFUIViewRootTargetElement^ GetTargetElement(void)const noexcept(true);
#endif // _MANAGED

	bool mSetClient(const cGCHandle &ObjectHandle)noexcept(true);
	void mResetClient(void)noexcept(true);


	bool mGetVisible(void)noexcept(true);
	bool mSetVisible(bool Visible)noexcept(true);

	bool mSetBackgroundColor(uInt8 r,uInt8 g,uInt8 b)noexcept(true);

	bool mSetWindowOwner(const cGCHandle &WPFRootRef)noexcept(true);
	void mClearWindowOwner(void)noexcept(true);


protected:
	void mSetup(const cGCHandle &WPFUIViewRootTargetElement)noexcept(true);
	void mClear(void)noexcept(true);
#if _MANAGED
	mcGCHandle<DNet::IWPFUIViewRootTargetElement,eGCHandleType::Weak> fTargetRootElement;
#else
	cGCHandle fTargetRootElement;
#endif // _MANAGED

	bool fWindowVisible;
	bool fWindowActive;

	virtual void WPFWindowStateChanged(void)noexcept(true)=0;
	virtual void WPFWindowShutdownStarted(void)noexcept(true)=0;
private:

#if _MANAGED
	friend DNet::WPFUIViewRootAdapter;
	friend rcNativeCaller_WPFUIWindow;

	__forceinline void __clrcall WPFDispatcherShutdownStarted(void)noexcept(true);

#endif // _MANAGED

};
//---------------------------------------------------------------------------
}	// namespace cnWin
//---------------------------------------------------------------------------
#if _MANAGED
//---------------------------------------------------------------------------
namespace DNet{
//---------------------------------------------------------------------------
private ref class WPFViewRoot sealed : System::Windows::UIElement
	, IWPFUIViewRootTargetElement
{
public:
	WPFViewRoot();


	property System::Windows::Media::Color BackgroundColor{
		System::Windows::Media::Color get();
		void set(System::Windows::Media::Color c);
	};
internal:

	// IWPFUIViewRootTargetElement

	property WPFUIViewRootAdapter% Adapter{
		virtual WPFUIViewRootAdapter% get(void)=IWPFUIViewRootTargetElement::Adapter::get;
	}
	
	property System::Windows::UIElement ^ClientElement{
		virtual System::Windows::UIElement^ get()=IWPFUIViewRootTargetElement::ClientElement::get;
		virtual void set(System::Windows::UIElement ^Element)=IWPFUIViewRootTargetElement::ClientElement::set;
	};

	property IWPFUIViewRootTargetElement^ OwnerTargetElement{
		virtual IWPFUIViewRootTargetElement^ get()=IWPFUIViewRootTargetElement::OwnerTargetElement::get;
		virtual void set(IWPFUIViewRootTargetElement ^Owner)=IWPFUIViewRootTargetElement::OwnerTargetElement::set;
	}

	property bool IsActive{
		virtual bool get()=IWPFUIViewRootTargetElement::IsActive::get;
	}
	virtual bool SetBackgroundColor(System::Windows::Media::Color Color)=IWPFUIViewRootTargetElement::SetBackgroundColor;
protected:
	WPFUIViewRootAdapter fViewRootAdapter;
	
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
class mbcWPFWindow;
//---------------------------------------------------------------------------
#if _MANAGED
//---------------------------------------------------------------------------
private ref class rcWPFHwndHandler sealed
{
public:
	rcWPFHwndHandler(mbcWPFWindow *CPP);

	property System::Windows::Interop::HwndSource ^Source{
		System::Windows::Interop::HwndSource^ get(void);
		void set(System::Windows::Interop::HwndSource ^value);
	}
	
internal:
	mbcWPFWindow *CPP;
protected:
	System::Windows::Interop::HwndSource ^fSource;
	System::Windows::Interop::HwndSourceHook ^fHook;


	void HwndSourceAttach(void);
	void HwndSourceDetach(void);
	System::IntPtr WindowMessageHook(System::IntPtr hwnd, int msg, System::IntPtr wParam, System::IntPtr lParam, bool %handled);
};
//---------------------------------------------------------------------------
#endif	// _MANAGED
//---------------------------------------------------------------------------
class mbcWPFWindow
{
public:
	bool mCreate(void* Parent,const wchar_t *WindowText,uInt32 Style,uInt32 ExStyle,sInt32 X,sInt32 Y,sInt32 Width,sInt32 Height)noexcept(true);
	bool mDestroy(void)noexcept(true);

	Float32 mQueryLayoutScale(void)const noexcept(true);

	bool mGetVisualRoot(cGCRef &VisualHandle)noexcept(true);
	bool mSetVisualRoot(const cGCHandle &VisualHandle)noexcept(true);
	bool mClearVisualRoot(void)noexcept(true);

#if _MANAGED
	friend rcWPFHwndHandler;

#endif // _MANAGED

protected:
	void mSetup(void)noexcept(true);
	void mClear(void)noexcept(true);
#if _MANAGED
	mcGCHandle<rcWPFHwndHandler> fWPFHwndHandler;
#else
	cGCHandle fWPFHwndHandler;
#endif	// _MANAGED

	virtual void WPFSourceAttach(void* WindowHandle)noexcept(true)=0;
	virtual void WPFSourceDetach(void)noexcept(true)=0;
	virtual bool WPFSourceMessage(uIntn &Result,void* hwnd, uInt32 msg, uIntn wParam, uIntn lParam)noexcept(true)=0;
private:
};
//---------------------------------------------------------------------------
void mMakeWPFViewRoot(cGCRef &Ref)noexcept(true);
//---------------------------------------------------------------------------
#if _MANAGED
//---------------------------------------------------------------------------
riWPFUIViewRemoteRoot^ WPFUIViewQueryRemoteRootFromRootVisual(System::Windows::Media::Visual ^RootVisual)noexcept(true);
//---------------------------------------------------------------------------
#endif // _MANAGED
//---------------------------------------------------------------------------
}	// namespace cnWin
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
