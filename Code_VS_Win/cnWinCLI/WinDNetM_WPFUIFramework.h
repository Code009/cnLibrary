/*- cnWin - WPF - UIElement -----------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2020-04-15                                          */
/*-------------------------------------------------------------------------*/
#pragma once
/*-------------------------------------------------------------------------*/
#if _MANAGED
#using <PresentationCore.dll>
#using <PresentationFramework.dll>
#using <System.Xaml.dll>
#endif	/* _MANAGED */

#include <cnWinCLI\WinDNetM_WPFUIView.h>
#include <cnWinCLI\WinDNetM_WPFUIViewWindow.h>
#include <cnWinCLI\WinDNetM_WPFComponent.h>

#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnWin{
//---------------------------------------------------------------------------
class mcWPFWindowAdapterAsUIWindow;
//---------------------------------------------------------------------------
}	// namespace cnWin
//---------------------------------------------------------------------------
#if _MANAGED
//---------------------------------------------------------------------------
namespace DNet{
//---------------------------------------------------------------------------
interface class IWPFWindowTarget;
//---------------------------------------------------------------------------
public ref class WPFWindowAdapter : IWPFViewRoot
{
internal:
	WPFWindowAdapter(IWPFWindowTarget ^Target);

	IWPFWindowTarget ^QueryTarget(void);

	// IWPFViewParent

	property cnWin::iWPFViewParent* ParentInterface{
		virtual cnWin::iWPFViewParent* get(void)=IWPFViewRoot::IWPFViewParent::Interface::get;
	};

	virtual void ParentRemoveViewChild(IWPFView ^Subview)=IWPFViewRoot::IWPFViewParent::RemoveViewChild;

	// IWPFViewRoot

	virtual void CPPAttach(cnWin::mcWPFViewRoot *CPP)=IWPFViewRoot::CPPAttach;
	virtual void CPPDetach(cnWin::mcWPFViewRoot *CPP)=IWPFViewRoot::CPPDetach;

	property System::Windows::UIElement ^ClientElement{
		virtual System::Windows::UIElement^ get()=IWPFViewRoot::ClientElement::get;
		virtual void set(System::Windows::UIElement ^Element)=IWPFViewRoot::ClientElement::set;
	};


	virtual void DispatcherFinishCleanup(void)=IWPFViewRoot::DispatcherFinishCleanup;

private:
	System::WeakReference fTarget;
	cnWin::mcWPFViewRoot *CPP;

	static WPFWindowAdapter^ ThisFromSender(System::Object ^sender);

	static void OnIsVisibleChanged(System::Object ^sender,System::Windows::DependencyPropertyChangedEventArgs e);
	static void OnActivated(System::Object ^sender,System::EventArgs ^e);

	bool fWindowVisible;
	bool fWindowActive;

	void UpdateVisible(bool Visible);
	void UpdateActive(bool Active);
	void NotifyStateChange(void);
};
//---------------------------------------------------------------------------
public interface class IWPFWindowTarget
{
	property WPFWindowAdapter^ WindowAdapter{
		WPFWindowAdapter^ get(void);
		void set(WPFWindowAdapter^ value);
	};

	property System::Windows::UIElement ^WindowClient{
		System::Windows::UIElement^ get();
		void set(System::Windows::UIElement ^Element);
	};
};
//---------------------------------------------------------------------------
public ref class WPFWindow : System::Windows::Window, IWPFWindowTarget
{
protected:
	property WPFWindowAdapter ^WindowAdapter{
		virtual WPFWindowAdapter^ get(void)=IWPFWindowTarget::WindowAdapter::get;
		virtual void set(WPFWindowAdapter^)=IWPFWindowTarget::WindowAdapter::set;
	}

	property System::Windows::UIElement ^WindowClient{
		virtual System::Windows::UIElement^ get(void)=IWPFWindowTarget::WindowClient::get;
		virtual void set(System::Windows::UIElement^)=IWPFWindowTarget::WindowClient::set;
	}
	WPFWindowAdapter ^fWindowAdapter;

};
//---------------------------------------------------------------------------
}	// namespace DNet
//---------------------------------------------------------------------------
#endif // _MANAGED
//---------------------------------------------------------------------------
namespace cnWin{
//---------------------------------------------------------------------------
#if _MANAGED
//---------------------------------------------------------------------------
iPtr<iUIWindow> MakeUIWindowFromWindowTarget(DNet::IWPFWindowTarget ^Target)noexcept(true);
//---------------------------------------------------------------------------
#endif // _MANAGED
//---------------------------------------------------------------------------
}	// namespace cnWin
//---------------------------------------------------------------------------
#if _MANAGED
//---------------------------------------------------------------------------
namespace DNet{
//---------------------------------------------------------------------------
interface class IWPFViewTarget;
//---------------------------------------------------------------------------
public ref class WPFViewAdapter : IWPFView
{
internal:
	WPFViewAdapter(IWPFViewTarget ^Target);

	IWPFViewTarget ^QueryTarget(void);
	template<class T> T ^QueryTarget(void);

	// IWPFViewParent

	property cnWin::iWPFViewParent* ParentInterface{
		virtual cnWin::iWPFViewParent* get(void)=IWPFView::IWPFViewParent::Interface::get;
	};

	virtual void ParentRemoveViewChild(IWPFView ^Subview)=IWPFView::IWPFViewParent::RemoveViewChild;

	// IWPFUIViewVisual

	property System::Windows::Media::Visual^ Visual{
		virtual System::Windows::Media::Visual^ get(void)=IWPFView::IWPFUIViewVisual::Visual::get;
	};
	property float LayoutScale{
		virtual float get(void)=IWPFView::IWPFUIViewVisual::LayoutScale::get;
	};

	// IWPFView

	virtual void CPPAttach(cnWin::mcWPFView *CPP)=IWPFView::CPPAttach;
	virtual void CPPDetach(cnWin::mcWPFView *CPP)=IWPFView::CPPDetach;

	property System::Windows::UIElement^ Element{
		virtual System::Windows::UIElement^ get(void)=IWPFView::Element::get;
	};

	property cnWin::iWPFViewChild* ChildInterface{
		virtual cnWin::iWPFViewChild* get(void)=IWPFView::ChildInterface::get;
	};

	virtual bool ParentAcquire(IWPFViewParent ^Parent)=IWPFView::ParentAcquire;
	virtual void ParentRelease(IWPFViewParent ^Parent)=IWPFView::ParentRelease;


	virtual bool InsertChild(System::Windows::UIElement ^Child)=IWPFView::InsertChildElement;
	virtual bool RemoveChild(System::Windows::UIElement ^Child)=IWPFView::RemoveChildElement;

	virtual System::Windows::DependencyObject^ GetParent(void)=IWPFView::GetParent;

	virtual void DispatcherFinishCleanup(void)=IWPFView::DispatcherFinishCleanup;
	
public:
	property int VisualChildrenCount{	int get(void);	};
	System::Windows::Media::Visual^ GetVisualChild(int index);

	void OnVisualParentChanged(System::Windows::DependencyObject ^NewParent);

	System::Windows::Size MeasureCore(System::Windows::Size availableSize);
	void ArrangeCore(System::Windows::Rect finalRect);

	System::Windows::Size MeasureOverride(System::Windows::Size availableSize);
	System::Windows::Size ArrangeOverride(System::Windows::Size finalSize);

	void OnRender(System::Windows::Media::DrawingContext^ drawingContext);
	System::Windows::Media::GeometryHitTestResult^ HitTestCore(System::Windows::Media::GeometryHitTestParameters ^ hitTestParameters);
	System::Windows::Media::HitTestResult^ HitTestCore(System::Windows::Media::PointHitTestParameters ^ hitTestParameters);


private:
	cnWin::mcWPFView *CPP;

	System::WeakReference fTarget;

	//static WPFViewAdapter^ ThisFromSender(System::Object ^sender);
	static cnWin::mcWPFView* CPPFromSender(System::Object ^sender);

	static void OnIsVisibleChanged(System::Object ^sender,System::Windows::DependencyPropertyChangedEventArgs e);

	// keyboard

	static void OnIsKeyboardFocusedChanged(System::Object ^sender,System::Windows::DependencyPropertyChangedEventArgs e);
	static void OnIsKeyboardFocusWithinChanged(System::Object ^sender,System::Windows::DependencyPropertyChangedEventArgs e);
	static void OnKeyDown(System::Object ^sender,System::Windows::Input::KeyEventArgs^ e);
	static void OnKeyUp(System::Object ^sender,System::Windows::Input::KeyEventArgs^ e);

	// mouse

	static void OnIsMouseDirectlyOverChanged(System::Object ^sender,System::Windows::DependencyPropertyChangedEventArgs e);
	static void OnMouseEnter(System::Object ^sender,System::Windows::Input::MouseEventArgs^ e);
	static void OnMouseLeave(System::Object ^sender,System::Windows::Input::MouseEventArgs^ e);
	static void OnMouseMove(System::Object ^sender,System::Windows::Input::MouseEventArgs^ e);
	static void OnMouseDown(System::Object ^sender,System::Windows::Input::MouseButtonEventArgs^ e);
	static void OnMouseUp(System::Object ^sender,System::Windows::Input::MouseButtonEventArgs^ e);
	static void OnMouseWheel(System::Object ^sender,System::Windows::Input::MouseWheelEventArgs^ e);

	// Touch

	static void OnTouchDown(System::Object ^sender,System::Windows::Input::TouchEventArgs^ e);
	static void OnTouchUp(System::Object ^sender,System::Windows::Input::TouchEventArgs^ e);
	static void OnTouchMove(System::Object ^sender,System::Windows::Input::TouchEventArgs^ e);
};
//---------------------------------------------------------------------------
public interface class IWPFViewTarget
{
	property WPFViewAdapter^ ViewAdapter{
		WPFViewAdapter ^get(void);
		void set(WPFViewAdapter^ value);
	};

	property System::Windows::DependencyObject ^ViewParent{
		System::Windows::DependencyObject ^get(void);
	}

	bool ViewAddVisualChild(System::Windows::UIElement ^Child);
	bool ViewRemoveVisualChild(System::Windows::UIElement ^Child);
};
//---------------------------------------------------------------------------
}	// namespace DNet
//---------------------------------------------------------------------------
#endif // _MANAGED
//---------------------------------------------------------------------------
namespace cnWin{
//---------------------------------------------------------------------------
#if _MANAGED
//---------------------------------------------------------------------------
iPtr<iUIView> MakeUIViewFromViewTarget(DNet::IWPFViewTarget ^Target)noexcept(true);
//---------------------------------------------------------------------------
#endif // _MANAGED
//---------------------------------------------------------------------------
}	// namespace cnWin
//---------------------------------------------------------------------------
#if _MANAGED
//---------------------------------------------------------------------------
namespace DNet{
//---------------------------------------------------------------------------
public ref class WPFNativeFrameworkElement : System::Windows::FrameworkElement, DNet::IWPFViewTarget
{
public:
	property cnLibrary::DNet::WPFViewAdapter^ ViewAdapter{
		virtual cnLibrary::DNet::WPFViewAdapter^get(void)=IWPFViewTarget::ViewAdapter::get;
		virtual void set(cnLibrary::DNet::WPFViewAdapter^ value)=IWPFViewTarget::ViewAdapter::set;
	};

	property System::Windows::DependencyObject ^ViewParent{
		virtual System::Windows::DependencyObject ^get(void)=IWPFViewTarget::ViewParent::get;
	}

	virtual bool ViewAddVisualChild(System::Windows::UIElement ^Child)=IWPFViewTarget::ViewAddVisualChild;
	virtual bool ViewRemoveVisualChild(System::Windows::UIElement ^Child)=IWPFViewTarget::ViewRemoveVisualChild;

public protected:
	virtual void OnVisualParentChanged(System::Windows::DependencyObject ^oldParent)override;
protected:

	cnLibrary::DNet::WPFViewAdapter ^fViewAdapter;

	// view children

	virtual System::Windows::Media::Visual^ GetVisualChild(int index)override;
	virtual property int VisualChildrenCount{
		virtual int get(void)override;
	};

	// layout
	
	//virtual void OnDpiChanged (System::Windows::DpiScale ^oldDpi,System::Windows::DpiScale ^newDpi)override;
	virtual System::Windows::Size MeasureOverride(System::Windows::Size availableSize)override;
	virtual System::Windows::Size ArrangeOverride(System::Windows::Size finalSize)override;
	virtual void OnRender(System::Windows::Media::DrawingContext^ drawingContext)override;
	virtual System::Windows::Media::GeometryHitTestResult^ HitTestCore(System::Windows::Media::GeometryHitTestParameters ^ hitTestParameters)override;
	virtual System::Windows::Media::HitTestResult^ HitTestCore(System::Windows::Media::PointHitTestParameters ^ hitTestParameters)override;


};
//---------------------------------------------------------------------------
}	// namespace DNet
//---------------------------------------------------------------------------
#endif // _MANAGED
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/


