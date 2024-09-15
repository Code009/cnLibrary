/*- cnWinCLI - Managed - WPF View -----------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2020-04-15                                          */
/*-------------------------------------------------------------------------*/
#pragma once
/*-------------------------------------------------------------------------*/
#if _MANAGED
#using <PresentationCore.dll>
#endif	/* _MANAGED */

#include <cnWinCLI\WinCLIM_WPFUICore.h>

#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
#if _MANAGED
//---------------------------------------------------------------------------
namespace DNet{
//---------------------------------------------------------------------------
value class WPFUIViewAdapter;
//---------------------------------------------------------------------------
public interface class IWPFUIViewTargetElement
{
	property WPFUIViewAdapter% Adapter{
		WPFUIViewAdapter% get(void);
	}
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
class mbcWPFUIView;
//---------------------------------------------------------------------------
#if _MANAGED
//---------------------------------------------------------------------------
private interface class riWPFUIViewRemoteRoot
{
	void RemoteChildAttach(DNet::IWPFUIViewTargetElement ^RemoteChild);
	void RemoteChildDetach(DNet::IWPFUIViewTargetElement ^RemoteChild);
};
//---------------------------------------------------------------------------
private ref class rcWPFExternalVisualRemoteRoot sealed : riWPFUIViewRemoteRoot
{
	static rcWPFExternalVisualRemoteRoot();
public:
	static initonly System::Windows::DependencyProperty ^VisualRemoteRootProperty;
	static rcWPFExternalVisualRemoteRoot^ Query(System::Windows::Media::Visual ^TargetVisual);

	rcWPFExternalVisualRemoteRoot(System::Windows::Media::Visual ^TargetVisual);


	// IWPFUIWindowElement

	virtual void RemoteChildAttach(DNet::IWPFUIViewTargetElement ^RemoteChild)=riWPFUIViewRemoteRoot::RemoteChildAttach;
	virtual void RemoteChildDetach(DNet::IWPFUIViewTargetElement ^RemoteChild)=riWPFUIViewRemoteRoot::RemoteChildDetach;


private:
	System::Windows::Media::Visual ^fTargetVisual;

	void *CPP;

	void CPPSetup(void);
	void CPPClear(void);

	int fChildCount;
};
//---------------------------------------------------------------------------
#endif // _MANAGED
//---------------------------------------------------------------------------
}	// namespace cnWin
//---------------------------------------------------------------------------
#if _MANAGED
//---------------------------------------------------------------------------
namespace DNet{
//---------------------------------------------------------------------------
public value class WPFUIViewAdapter
{
public:
	void CPPAttach(cnWin::mbcWPFUIView *CPP);
	void CPPDetach(cnWin::mbcWPFUIView *CPP);

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

internal:
	cnWin::mbcWPFUIView *CPP;
	array<System::Windows::Media::Visual^> ^ChildrenCache;
	System::Windows::Media::DrawingGroup ^ContentDrawingGroup;

	System::Windows::Rect WPFRect;

private:
	cnWin::riWPFUIViewRemoteRoot ^fRemoteRoot;

	static cnWin::mbcWPFUIView* CPPFromSender(System::Object ^sender);

	static void TargetOnPresentationSourceChanged(System::Object ^sender,System::Windows::SourceChangedEventArgs ^e);

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
}	// namespace DNet
//---------------------------------------------------------------------------
#endif // _MANAGED
//---------------------------------------------------------------------------
namespace cnWin{
//---------------------------------------------------------------------------
class mbcWPFUIView
{
public:
	cGCHandle& GetTargetElementHandle(void)noexcept(true);

	bool mInsertView(const cGCHandle &WPFTargetElement)noexcept(true);
	void mRemoveView(const cGCHandle &WPFTargetElement)noexcept(true);

	bool mGetVisible(void)noexcept(true);
	bool mSetVisible(bool Visible)noexcept(true);

	cUIPoint mGetSize(void)noexcept(true);
	bool mSetSize(cUIPoint Size)noexcept(true);
	bool mTranslatePointTo(const cGCHandle &UIElementHandle,cUIPoint &Position)noexcept(true);
	bool mMovePosition(const cGCHandle &UIElementHandle,cUIPoint Position)noexcept(true);
	bool mArrangeRectangle(const cGCHandle &UIElementHandle,cUIPoint Position,cUIPoint Size)noexcept(true);
	void mSetArrangement(void)noexcept(true);
	void mArrangeLayout(void)noexcept(true);

	void mRenderContent(cGCHandle const**Drawings,uIntn DrawingCount)noexcept(true);
	
	void mSetFocus(bool ActivateWindow)noexcept(true);

#if _MANAGED

	static bool __clrcall mArrange(DNet::IWPFUIViewTargetElement ^TargetElement,System::Windows::Rect Rect)noexcept(true);
	bool __clrcall mTranslateWPFPointTo(DNet::IWPFUIViewTargetElement ^TargetElement,System::Object ^Relative,System::Windows::Point %Point)noexcept(true);
	bool __clrcall mTranslateWPFPointFrom(DNet::IWPFUIViewTargetElement ^TargetElement,System::Object ^Relative,System::Windows::Point %Point)noexcept(true);
	bool __clrcall mTranslateWPFParentPointFrom(DNet::IWPFUIViewTargetElement ^TargetElement,System::Object ^Relative,System::Windows::Point %Point)noexcept(true);

#endif // _MANAGED

protected:
	void mSetup(const cGCHandle &WPFTargetElement)noexcept(true);
	void mClear(void)noexcept(true);

#if _MANAGED
	mcGCHandle<DNet::IWPFUIViewTargetElement,eGCHandleType::Weak> fTargetElement;
#else
	cGCHandle fTargetElement;
#endif // _MANAGED

	Float32 fLayoutScale;

	// layout events

	struct cChildrenInfo
	{
		uIntn ChildCount;
		mbcWPFUIView *const* Views;
	};

	virtual cChildrenInfo* WPFSetupChildrenInfo(void* &Handle)noexcept(true)=0;
	virtual void WPFClearChildrenInfo(void *Handle)noexcept(true)=0;
	virtual bool WPFAttachExtParent(void)noexcept(true)=0;
	virtual void WPFDetachExtParent(void)noexcept(true)=0;
	virtual void WPFNotifyVisible(bool Visible)noexcept(true)=0;
	virtual void WPFNotifyArrange(bool Moved,bool Sized,cUIRect Rect)noexcept(true)=0;

	// keyboard events

	virtual void WPFOnIsKeyboardFocusedChanged(bool Focused)noexcept(true)=0;
	virtual void WPFOnIsKeyboardFocusWithinChanged(bool Focused)noexcept(true)=0;
	virtual void WPFOnKeyDownFilter(mcWPFKeyEventArgs &KeyEventArgs,eKeyCode KeyCode,bool Repeat)noexcept(true)=0;
	virtual void WPFOnKeyDown(mcWPFKeyEventArgs &KeyEventArgs,eKeyCode KeyCode,bool Repeat)noexcept(true)=0;
	virtual void WPFOnKeyUp(mcWPFKeyEventArgs &KeyEventArgs,eKeyCode KeyCode)noexcept(true)=0;
	virtual void WPFOnKeyUpFilter(mcWPFKeyEventArgs &KeyEventArgs,eKeyCode KeyCode)noexcept(true)=0;


	// mouse events

	virtual void WPFOnIsMouseDirectlyOverChanged(bool InRange)noexcept(true)=0;
	virtual void WPFOnMouseEnter(mcWPFMouseEventArgs &MouseEventArgs)noexcept(true)=0;
	virtual void WPFOnMouseLeave(mcWPFMouseEventArgs &MouseEventArgs)noexcept(true)=0;
	virtual void WPFOnMouseMove(mcWPFMouseEventArgs &MouseEventArgs)noexcept(true)=0;
	virtual void WPFOnMouseMoveFilter(mcWPFMouseEventArgs &MouseEventArgs)noexcept(true)=0;
	virtual void WPFOnMouseDown(mcWPFMouseEventArgs &MouseEventArgs,eMouseButton Button)noexcept(true)=0;
	virtual void WPFOnMouseDownFilter(mcWPFMouseEventArgs &MouseEventArgs,eMouseButton Button)noexcept(true)=0;
	virtual void WPFOnMouseUp(mcWPFMouseEventArgs &MouseEventArgs,eMouseButton Button)noexcept(true)=0;
	virtual void WPFOnMouseUpFilter(mcWPFMouseEventArgs &MouseEventArgs,eMouseButton Button)noexcept(true)=0;
	virtual void WPFOnMouseWheel(mcWPFMouseEventArgs &MouseEventArgs,Float32 ScrollX,Float32 ScrollY)noexcept(true)=0;
	virtual void WPFOnMouseWheelFilter(mcWPFMouseEventArgs &MouseEventArgs,Float32 ScrollX,Float32 ScrollY)noexcept(true)=0;

	// touch events

	virtual void WPFOnTouchDown(mcWPFTouchEventArgs &TouchEventArgs)noexcept(true)=0;
	virtual void WPFOnTouchDownFilter(mcWPFTouchEventArgs &TouchEventArgs)noexcept(true)=0;
	virtual void WPFOnTouchUp(mcWPFTouchEventArgs &TouchEventArgs)noexcept(true)=0;
	virtual void WPFOnTouchUpFilter(mcWPFTouchEventArgs &TouchEventArgs)noexcept(true)=0;
	virtual void WPFOnTouchMove(mcWPFTouchEventArgs &TouchEventArgs)noexcept(true)=0;
	virtual void WPFOnTouchMoveFilter(mcWPFTouchEventArgs &TouchEventArgs)noexcept(true)=0;


private:
#if _MANAGED
	friend DNet::WPFUIViewAdapter;
	static eMouseButton __clrcall ToMouseButton(System::Windows::Input::MouseButton Button)noexcept(true);

	__forceinline void WPFSetupChildrenCache(DNet::WPFUIViewAdapter %Adapter)noexcept(true);

	__forceinline void WPFViewOnPresentationSourceChanged(System::Windows::SourceChangedEventArgs ^e)noexcept(true);

	__forceinline void WPFViewArrangeCore(DNet::WPFUIViewAdapter %Adapter,System::Windows::Rect finalRect)noexcept(true);
	__forceinline bool WPFViewHitTestCore(DNet::WPFUIViewAdapter %Adapter,System::Windows::Media::IntersectionDetail %HitDetail,System::Windows::Media::GeometryHitTestParameters ^ hitTestParameters)noexcept(true);
	__forceinline bool WPFViewHitTestCore(DNet::WPFUIViewAdapter %Adapter,System::Windows::Point %HitPoint)noexcept(true);

	__forceinline void WPFViewOnIsKeyboardFocusedChanged(System::Windows::DependencyPropertyChangedEventArgs %e)noexcept(true);
	__forceinline void WPFViewOnIsKeyboardFocusWithinChanged(System::Windows::DependencyPropertyChangedEventArgs %e)noexcept(true);
	__forceinline void WPFViewOnKeyDown(System::Object ^sender,System::Windows::Input::KeyEventArgs^ e)noexcept(true);
	__forceinline void WPFViewOnKeyUp(System::Object ^sender,System::Windows::Input::KeyEventArgs^ e)noexcept(true);
	__forceinline void WPFViewOnIsMouseDirectlyOverChanged(System::Windows::DependencyPropertyChangedEventArgs %e)noexcept(true);
	__forceinline void WPFViewOnMouseEnter(System::Windows::Input::MouseEventArgs^ e)noexcept(true);
	__forceinline void WPFViewOnMouseLeave(System::Windows::Input::MouseEventArgs^ e)noexcept(true);
	__forceinline void WPFViewOnMouseMove(System::Object ^sender,System::Windows::Input::MouseEventArgs^ e)noexcept(true);
	__forceinline void WPFViewOnMouseDown(System::Object ^sender,System::Windows::Input::MouseButtonEventArgs^ e)noexcept(true);
	__forceinline void WPFViewOnMouseUp(System::Object ^sender,System::Windows::Input::MouseButtonEventArgs^ e)noexcept(true);
	__forceinline void WPFViewOnMouseWheel(System::Object ^sender,System::Windows::Input::MouseWheelEventArgs^ e)noexcept(true);
	__forceinline void WPFViewOnTouchDown(System::Object ^sender,System::Windows::Input::TouchEventArgs^ e)noexcept(true);
	__forceinline void WPFViewOnTouchUp(System::Object ^sender,System::Windows::Input::TouchEventArgs^ e)noexcept(true);
	__forceinline void WPFViewOnTouchMove(System::Object ^sender,System::Windows::Input::TouchEventArgs^ e)noexcept(true);

#endif // _MANAGED


};
//---------------------------------------------------------------------------
}	// namespace cnWin
//---------------------------------------------------------------------------
#if _MANAGED
//---------------------------------------------------------------------------
namespace DNet{
//---------------------------------------------------------------------------
private ref class WPFView sealed : System::Windows::UIElement, IWPFUIViewTargetElement
{
public:
	// IWPFUIViewTargetElement

	property WPFUIViewAdapter %Adapter{
		virtual WPFUIViewAdapter% get(void)=IWPFUIViewTargetElement::Adapter::get;
	}
	property System::Windows::DependencyObject ^ViewParent{
		virtual System::Windows::DependencyObject^ get(void)=IWPFUIViewTargetElement::ViewParent::get;
	}

	virtual bool ViewAddVisualChild(System::Windows::UIElement ^Child)=IWPFUIViewTargetElement::ViewAddVisualChild;
	virtual bool ViewRemoveVisualChild(System::Windows::UIElement ^Child)=IWPFUIViewTargetElement::ViewRemoveVisualChild;

public protected:
	virtual void OnVisualParentChanged(System::Windows::DependencyObject ^oldParent)override;
protected:
	WPFUIViewAdapter fViewAdapter;

	// view children

	virtual property int VisualChildrenCount{
		virtual int get(void)override;
	};
	virtual System::Windows::Media::Visual^ GetVisualChild(int index)override;

	// layout
	
	//virtual void OnDpiChanged (System::Windows::DpiScale ^oldDpi,System::Windows::DpiScale ^newDpi)override;
	virtual System::Windows::Size MeasureCore(System::Windows::Size availableSize)override;
	virtual void ArrangeCore(System::Windows::Rect finalRect)override;
	virtual void OnRender(System::Windows::Media::DrawingContext^ drawingContext)override;
	virtual System::Windows::Media::GeometryHitTestResult^ HitTestCore(System::Windows::Media::GeometryHitTestParameters ^ hitTestParameters)override;
	virtual System::Windows::Media::HitTestResult^ HitTestCore(System::Windows::Media::PointHitTestParameters ^ hitTestParameters)override;

};

//---------------------------------------------------------------------------
}	// namespace DNet
//---------------------------------------------------------------------------
#endif	// _MANAGED
//---------------------------------------------------------------------------
namespace cnWin{
//---------------------------------------------------------------------------
void mMakeWPFView(cGCRef &Ref)noexcept(true);
//---------------------------------------------------------------------------
}	// namespace cnWin
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
