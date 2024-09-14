/*- cnWinWPF - Managed - View ---------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2020-04-15                                          */
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
namespace cnWin{
//---------------------------------------------------------------------------
iPtr<iUIView> DNetCreateUIView(void)noexcept(true);
//---------------------------------------------------------------------------
class cnLib_INTERFACE iWPFViewChild : public iInterface
{
public:

	struct tInterfaceID{	static iTypeID Value;	};
	virtual void* cnLib_FUNC CastInterface(iTypeID ID)noexcept(true) override;

	virtual cGCRef& WPFChildGetElementHandle(void)noexcept(true)=0;

	virtual Float32 WPFChildGetLayoutScale(void)noexcept(true)=0;
	virtual Float32 WPFChildGetZPosition(void)noexcept(true)=0;
	virtual void WPFChildTreeNotifyWindow(iUIWindow *Window)noexcept(true)=0;
	virtual void WPFChildTreeNotifyState(void)noexcept(true)=0;
	virtual void WPFChildTreeNotifyScale(void)noexcept(true)=0;

};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iWPFViewParent
{
public:
	virtual eUIState WPFParentGetState(void)noexcept(true)=0;
	virtual Float32 WPFParentGetContentScale(void)noexcept(true)=0;
	virtual Float32 WPFParentGetLayoutScale(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class mcWPFView;
//---------------------------------------------------------------------------
}	// namespace cnWin
//---------------------------------------------------------------------------
#if _MANAGED
//---------------------------------------------------------------------------
namespace DNet{
//---------------------------------------------------------------------------
private interface class IWPFUIWindowElement
{
	property iUIWindow* UIWindowInterface{
		iUIWindow* get(void);
	};

	void RemoteChildAttach(System::Object^);
	void RemoteChildDetach(System::Object^);
};
//---------------------------------------------------------------------------
interface class IWPFView;
//---------------------------------------------------------------------------
private interface class IWPFViewParent
{
	property cnWin::iWPFViewParent* Interface{
		cnWin::iWPFViewParent* get(void);
	};

	void RemoveViewChild(IWPFView ^Subview);
};
//---------------------------------------------------------------------------
private interface class IWPFView : IWPFViewParent, IWPFUIViewVisual
{
	void CPPAttach(cnWin::mcWPFView *CPP);
	void CPPDetach(cnWin::mcWPFView *CPP);

	property System::Windows::UIElement^ Element{
		System::Windows::UIElement^ get(void);
	};

	property cnWin::iWPFViewChild* ChildInterface{
		cnWin::iWPFViewChild* get(void);
	};

	bool ParentAcquire(IWPFViewParent ^Parent);
	void ParentRelease(IWPFViewParent ^Parent);

	bool InsertChildElement(System::Windows::UIElement ^Child);
	bool RemoveChildElement(System::Windows::UIElement ^Child);

	System::Windows::DependencyObject^ GetParent(void);

	void DispatcherFinishCleanup(void);

};
//---------------------------------------------------------------------------
}	// namespace DNet
//---------------------------------------------------------------------------
#endif // _MANAGED
//---------------------------------------------------------------------------
namespace cnWin{
//---------------------------------------------------------------------------
struct mcWPFKeyEventArgs
{
#if _MANAGED
	mcWPFKeyEventArgs(System::Windows::Input::KeyEventArgs^ e)noexcept(true);
#endif // _MANAGED
	mcWPFKeyEventArgs(mcWPFKeyEventArgs &&Src)noexcept(true);
	~mcWPFKeyEventArgs()noexcept(true);

#if _MANAGED
	mcGCRefT<System::Windows::Input::KeyEventArgs> Handle;
#else
	cGCRef Handle;
#endif

	void mCancelEvent(void)noexcept(true);
	bool mIsCancelled(void)noexcept(true);
};
//---------------------------------------------------------------------------
struct mcWPFMouseEventArgs
{
#if _MANAGED
	mcWPFMouseEventArgs(System::Windows::Input::MouseEventArgs^ e)noexcept(true);
#endif // _MANAGED
	mcWPFMouseEventArgs(mcWPFMouseEventArgs &&Src)noexcept(true);
	~mcWPFMouseEventArgs()noexcept(true);

#if _MANAGED
	mcGCRefT<System::Windows::Input::MouseEventArgs> Handle;
#else
	cGCRef Handle;
#endif

	void mCancelEvent(void)noexcept(true);
	bool mIsCancelled(void)noexcept(true);
};
//---------------------------------------------------------------------------
struct mcWPFTouchEventArgs
{
#if _MANAGED
	mcWPFTouchEventArgs(System::Windows::Input::TouchEventArgs^ e)noexcept(true);
#endif // _MANAGED
	mcWPFTouchEventArgs(mcWPFTouchEventArgs &&Src)noexcept(true);
	~mcWPFTouchEventArgs()noexcept(true);

#if _MANAGED
	mcGCRefT<System::Windows::Input::TouchEventArgs> Handle;
#else
	cGCRef Handle;
#endif

	void* mGetTouchID(void)noexcept(true);
	void mCancelEvent(void)noexcept(true);
	bool mIsCancelled(void)noexcept(true);

	bool mGetPosition(cGCRef &UIElementHandle,cUIPoint &Position)noexcept(true);

#if _MANAGED
	bool __clrcall mGetPosition(System::Windows::Media::Visual ^Visual,cUIPoint &Position)noexcept(true);
	bool __clrcall mGetPosition(DNet::IWPFUIViewVisual ^ViewVisual,cUIPoint &Position)noexcept(true);
#endif
};
//---------------------------------------------------------------------------
#if _MANAGED
//---------------------------------------------------------------------------
private ref class rcWPFVisualRootAsWindow sealed : public DNet::IWPFUIWindowElement
{
	static rcWPFVisualRootAsWindow();
public:
	static initonly System::Windows::DependencyProperty ^VisualRootAsWindowProperty;
	static rcWPFVisualRootAsWindow^ Query(System::Windows::Media::Visual ^TargetVisual);
	
	rcWPFVisualRootAsWindow(System::Windows::Media::Visual ^TargetVisual);


	// IWPFUIWindowElement

	virtual property iUIWindow* UIWindowInterface{
		virtual iUIWindow* get(void);
	};

	virtual void RemoteChildAttach(System::Object^);
	virtual void RemoteChildDetach(System::Object^);


private:
	initonly System::Windows::Media::Visual ^fTargetVisual;

	void *CPP;

	void CPPSetup(void);
	void CPPClear(void);

	int fChildCount;
};
#endif	// _MANAGED
//---------------------------------------------------------------------------
struct mcWPFVisual
{
#if _MANAGED
	mcWPFVisual(System::Windows::Media::Visual^ v)noexcept(true);
#endif // _MANAGED
	mcWPFVisual(mcWPFVisual &&Src)noexcept(true);
	~mcWPFVisual()noexcept(true);

#if _MANAGED
	mcGCRefT<System::Windows::Media::Visual> Handle;
#else
	cGCRef Handle;
#endif
};
//---------------------------------------------------------------------------
void* rcWPFVisualRootAsWindow_MakeWindowInterface(mcWPFVisual &&VisualHandle)noexcept(true);
void rcWPFVisualRootAsWindow_DropWindowInterface(void *Window)noexcept(true);
iUIWindow* rcWPFVisualRootAsWindow_GetUIWindow(void *Window)noexcept(true);
//---------------------------------------------------------------------------
#if _MANAGED
//---------------------------------------------------------------------------
class mcUIElementAsWPFViewParent;
private ref class rcUIElementAsWPFViewParent sealed : DNet::IWPFViewParent
{
	static rcUIElementAsWPFViewParent();
public:
	static initonly System::Windows::DependencyProperty ^UIElementAsWPFViewParentProperty;
	static rcUIElementAsWPFViewParent^ Query(System::Windows::UIElement ^TargetElement);

	rcUIElementAsWPFViewParent(System::Windows::UIElement ^TargetElement);

	// IWPFViewParent

	property iWPFViewParent* ParentInterface{
		virtual iWPFViewParent* get(void)=IWPFViewParent::Interface::get;
	};

	virtual void ParentRemoveViewChild(DNet::IWPFView ^Subview)=IWPFViewParent::RemoveViewChild;
	
	void ViewChildAttach(DNet::IWPFView ^Child);
	void ViewChildDetach(DNet::IWPFView ^Child);

protected:
	System::Windows::UIElement ^fTargetElement;
	DNet::IWPFUIWindowElement ^fViewRoot;

private:
	mcUIElementAsWPFViewParent *CPP;
	void CPPSetup(DNet::IWPFView ^InitalChild);
	void CPPClear(void);

	void TargetOnIsVisibleChanged(System::Object ^sender,System::Windows::DependencyPropertyChangedEventArgs e);
	void TargetOnSourceChanged(System::Object ^sender,System::Windows::SourceChangedEventArgs ^e);

	void NotifyWindowChange(void);
};
//---------------------------------------------------------------------------
#endif	// _MANAGED
//---------------------------------------------------------------------------
class mcUIElementAsWPFViewParent final : public iWPFViewParent
{
public:
#if _MANAGED
	friend rcUIElementAsWPFViewParent;
	mcUIElementAsWPFViewParent(rcUIElementAsWPFViewParent ^Parent,DNet::IWPFView ^Child)noexcept(true);
	rcUIElementAsWPFViewParent^ GetWPF(void)const noexcept(true);
#endif // _MANAGED

	mcUIElementAsWPFViewParent()=delete;
	~mcUIElementAsWPFViewParent()noexcept(true);


	// iWPFViewParent

	virtual eUIState WPFParentGetState(void)noexcept(true)override;
	virtual Float32 WPFParentGetContentScale(void)noexcept(true)override;
	virtual Float32 WPFParentGetLayoutScale(void)noexcept(true)override;

	uIntn GetChildCount(void)const noexcept(true);

private:
	// state
	cnRTL::cSeqSet< iPtr<iWPFViewChild> > fChildSet;
	eUIState fViewState;

	void WPFNotifyWindowChange(iUIWindow *Window)noexcept(true);
	void WPFUpdateVisible(bool IsVisible)noexcept(true);

#if _MANAGED
	mcGCRefT<rcUIElementAsWPFViewParent> fWPF;
#else
	cGCHandle fWPF;
#endif // _MANAGED


	bool WPFChildAttach(iWPFViewChild *Child)noexcept(true);
	bool WPFChildDetach(iWPFViewChild *Child)noexcept(true);

};
//---------------------------------------------------------------------------
class cWPFUIView;
//---------------------------------------------------------------------------
class mcWPFView : public iWPFViewChild, public iWPFViewParent
{
public:
	struct mcConstructParameter
	{
#if !_MANAGED
		mcConstructParameter()=delete;
		~mcConstructParameter()=delete;
#endif // !_MANAGED

#if _MANAGED
		mcGCRefT<DNet::IWPFView> WPFView;
#else
		cGCRef WPFView;
#endif
	};

private:
	mcWPFView(mcConstructParameter &Parameter)noexcept(true);
	~mcWPFView()noexcept(true);

	friend cWPFUIView;
	
protected:
	

	
	iWPFViewParent *fParent=nullptr;
	mcWPFView *fParentView=nullptr;
	iUIWindow *fWindow=nullptr;
#if _MANAGED
	mcGCRefT<DNet::IWPFView> fWPFView;
	mcGCRefT<DNet::IWPFViewParent> fWPFViewParent;
	mcGCRefT<rcUIElementAsWPFViewParent> fWPFElementParent;
	mcGCRefT<System::Windows::Media::DrawingGroup> fWPFContent;
#else
	cGCRef fWPFView;
	cGCRef fWPFViewParent;
	cGCRef fWPFElementParent;
	cGCRef fWPFContent;
#endif // _MANAGED



	// layout

	cUIRect fRectWPF;
	Float32 fZPosition;
	Float32 fLayoutScale;
	Float32 fContentScale;

	bool mGetVisible(void)noexcept(true);
	bool mSetVisible(bool Visible)noexcept(true);

	bool mTranslatePointTo(cGCRef &UIElementHandle,cUIPoint &Position)noexcept(true);
	bool mSetSize(cUIPoint Size)noexcept(true);
	bool mSetPosition(cGCRef &UIElementHandle,cUIPoint Position)noexcept(true);
	bool mSetRectangle(cGCRef &UIElementHandle,cUIPoint Position,cUIPoint Size)noexcept(true);

	void mSetArrangement(void)noexcept(true);
	void mArrangeLayout(void)noexcept(true);

#if _MANAGED
	bool __clrcall mTranslateWPFPointTo(System::Object ^Relative,System::Windows::Point %Point)noexcept(true);
	bool __clrcall mTranslateWPFPointFrom(System::Object ^Relative,System::Windows::Point %Point)noexcept(true);
	bool __clrcall mArrange(System::Windows::Rect Rect)noexcept(true);

#endif // _MANAGED

	
	// children

	struct cSubviewSetItem
	{
		Float32 ZPosition;
#if _MANAGED
		mcGCHandleT<DNet::IWPFView> WPFHandle;
#else
		cGCRef WPFHandle;

		cSubviewSetItem()=delete;
		~cSubviewSetItem()=delete;
#endif	// _MANAGED
	};
	cnRTL::cLinkMap<iWPFViewChild*,cSubviewSetItem> fSubviewMap;
	typedef cnRTL::cLinkMap<iWPFViewChild*,cSubviewSetItem>::tContent tSubviewNode;
	
	// content
	
	struct cSubviewItemZOrderOperator
	{
		static sfInt8 Compare(const tSubviewNode *Item,const tSubviewNode *Value)noexcept(true);
	};
	cnRTL::cSeqSet<tSubviewNode*,cSubviewItemZOrderOperator> fSubviewZOrderSet;


	void UpdateSubviewZPosition(iWPFViewChild *Subview)noexcept(true);

#if _MANAGED

	bool mInsertView(iWPFViewChild *Subview,DNet::IWPFView ^WPFSubview,Float32 ZPosition)noexcept(true);
	bool mRemoveView(iWPFViewChild *Subview,DNet::IWPFView ^WPFSubview)noexcept(true);
#endif // _MANAGED


	bool mInsertView(iWPFViewChild *Subview,const cGCRef &WPFViewHandle,Float32 ZPosition)noexcept(true);
	bool mRemoveView(iWPFViewChild *Subview,const cGCRef &WPFViewHandle)noexcept(true);

	
	void mRenderContent(cGCHandle const**Drawings,uIntn DrawingCount)noexcept(true);



	void mSetFocus(bool ActivateWindow)noexcept(true);

	// state

	bool fStateVisible=false;
	bool fLayoutInProgress=false;


private:
	mcDNetUIThreadDispatcher::cDispatcherFinishNotify fDispatcherFinishNotify;
	
#if _MANAGED

	static void __clrcall DispatcherFinishNotify(mcDNetUIThreadDispatcher::cDispatcherFinishNotify *Notify,bool Shutdown)noexcept(true);
	void __clrcall DispatcherFinishNotify(bool Shutdown)noexcept(true);
	void __clrcall CleanupWPF(void)noexcept(true);
	
#endif // _MANAGED

	void nDispatchFinishNotify(bool Shutdown)noexcept(true);

	// layout events from  from IWPFView

	void WPFUIViewUpdateContentSize(Float32 Scale,Float32 Width,Float32 Height)noexcept(true);
	void WPFUIViewNotifyArrange(bool Moved,bool Sized)noexcept(true);

	// keyboard events from IWPFView

	void WPFUIViewOnIsKeyboardFocusedChanged(bool Focused)noexcept(true);
	void WPFUIViewOnIsKeyboardFocusWithinChanged(bool Focused)noexcept(true);
	void WPFUIViewOnKeyDownFilter(mcWPFKeyEventArgs &KeyEventArgs,eKeyCode KeyCode,bool Repeat)noexcept(true);
	void WPFUIViewOnKeyDown(mcWPFKeyEventArgs &KeyEventArgs,eKeyCode Key,bool Repeat)noexcept(true);
	void WPFUIViewOnKeyUp(mcWPFKeyEventArgs &KeyEventArgs,eKeyCode KeyCode)noexcept(true);
	void WPFUIViewOnKeyUpFilter(mcWPFKeyEventArgs &KeyEventArgs,eKeyCode KeyCode)noexcept(true);

	// mouse events from IWPFView

	void WPFUIViewOnIsMouseDirectlyOverChanged(bool InRange)noexcept(true);
	void WPFUIViewOnMouseEnter(mcWPFMouseEventArgs &MouseEventArgs)noexcept(true);
	void WPFUIViewOnMouseLeave(mcWPFMouseEventArgs &MouseEventArgs)noexcept(true);
	void WPFUIViewOnMouseMove(mcWPFMouseEventArgs &MouseEventArgs)noexcept(true);
	void WPFUIViewOnMouseMoveFilter(mcWPFMouseEventArgs &MouseEventArgs)noexcept(true);
	void WPFUIViewOnMouseDown(mcWPFMouseEventArgs &MouseEventArgs,eMouseButton Button)noexcept(true);
	void WPFUIViewOnMouseDownFilter(mcWPFMouseEventArgs &MouseEventArgs,eMouseButton Button)noexcept(true);
	void WPFUIViewOnMouseUp(mcWPFMouseEventArgs &MouseEventArgs,eMouseButton Button)noexcept(true);
	void WPFUIViewOnMouseUpFilter(mcWPFMouseEventArgs &MouseEventArgs,eMouseButton Button)noexcept(true);
	void WPFUIViewOnMouseWheel(mcWPFMouseEventArgs &MouseEventArgs,Float32 ScrollX,Float32 ScrollY)noexcept(true);
	void WPFUIViewOnMouseWheelFilter(mcWPFMouseEventArgs &MouseEventArgs,Float32 ScrollX,Float32 ScrollY)noexcept(true);

	// touch events from IWPFView

	void WPFUIViewOnTouchDown(mcWPFTouchEventArgs &TouchEventArgs)noexcept(true);
	void WPFUIViewOnTouchDownFilter(mcWPFTouchEventArgs &TouchEventArgs)noexcept(true);
	void WPFUIViewOnTouchUp(mcWPFTouchEventArgs &TouchEventArgs)noexcept(true);
	void WPFUIViewOnTouchUpFilter(mcWPFTouchEventArgs &TouchEventArgs)noexcept(true);
	void WPFUIViewOnTouchMove(mcWPFTouchEventArgs &TouchEventArgs)noexcept(true);
	void WPFUIViewOnTouchMoveFilter(mcWPFTouchEventArgs &TouchEventArgs)noexcept(true);

public:

	// WPF notification

	void WPFNotifyVisible(bool Visible)noexcept(true);

#if _MANAGED
	static eMouseButton __clrcall ToMouseButton(System::Windows::Input::MouseButton Button)noexcept(true);
	
	// Methods for IWPFParent

	void __clrcall WPFParentRemoveChild(DNet::IWPFView ^WPFSubview)noexcept(true);

	// Methods for WPF Element

	bool __clrcall WPFViewParentAcquire(DNet::IWPFViewParent ^Parent)noexcept(true);
	void __clrcall WPFViewParentRelease(DNet::IWPFViewParent ^Parent)noexcept(true);
	void __clrcall WPFViewIsVisibleChanged(System::Windows::DependencyPropertyChangedEventArgs %e)noexcept(true);
	void __clrcall WPFViewOnVisualParentChanged(System::Windows::DependencyObject ^VisualParent)noexcept(true);
	System::Windows::Media::Visual^ __clrcall WPFViewGetVisualChild(int index)noexcept(true);
	int __clrcall WPFViewGetVisualChildrenCount(void)noexcept(true);
	void __clrcall WPFViewArrangeCore(System::Windows::Rect finalRect)noexcept(true);
	System::Windows::Size __clrcall WPFViewMeasureCore(System::Windows::Size availableSize)noexcept(true);
	bool __clrcall WPFViewHitTestCore(System::Windows::Media::IntersectionDetail %HitDetail,System::Windows::Media::GeometryHitTestParameters ^ hitTestParameters)noexcept(true);
	bool __clrcall WPFViewHitTestCore(System::Windows::Point %HitPoint)noexcept(true);
	void __clrcall WPFViewOnRender(System::Windows::Media::DrawingContext^ drawingContext)noexcept(true);

	void __clrcall WPFViewOnIsKeyboardFocusedChanged(System::Windows::DependencyPropertyChangedEventArgs %e)noexcept(true);
	void __clrcall WPFViewOnIsKeyboardFocusWithinChanged(System::Windows::DependencyPropertyChangedEventArgs %e)noexcept(true);
	void __clrcall WPFViewOnKeyDown(System::Windows::Input::KeyEventArgs^ e)noexcept(true);
	void __clrcall WPFViewOnKeyUp(System::Windows::Input::KeyEventArgs^ e)noexcept(true);
	void __clrcall WPFViewOnIsMouseDirectlyOverChanged(System::Windows::DependencyPropertyChangedEventArgs %e)noexcept(true);
	void __clrcall WPFViewOnMouseEnter(System::Windows::Input::MouseEventArgs^ e)noexcept(true);
	void __clrcall WPFViewOnMouseLeave(System::Windows::Input::MouseEventArgs^ e)noexcept(true);
	void __clrcall WPFViewOnMouseMove(System::Windows::Input::MouseEventArgs^ e)noexcept(true);
	void __clrcall WPFViewOnMouseDown(System::Windows::Input::MouseButtonEventArgs^ e)noexcept(true);
	void __clrcall WPFViewOnMouseUp(System::Windows::Input::MouseButtonEventArgs^ e)noexcept(true);
	void __clrcall WPFViewOnMouseWheel(System::Windows::Input::MouseWheelEventArgs^ e)noexcept(true);
	void __clrcall WPFViewOnTouchDown(System::Windows::Input::TouchEventArgs^ e)noexcept(true);
	void __clrcall WPFViewOnTouchUp(System::Windows::Input::TouchEventArgs^ e)noexcept(true);
	void __clrcall WPFViewOnTouchMove(System::Windows::Input::TouchEventArgs^ e)noexcept(true);

#endif // _MANAGED

};
//---------------------------------------------------------------------------
}	// namespace cnWin
//---------------------------------------------------------------------------
#if _MANAGED
//---------------------------------------------------------------------------
namespace DNet{
//---------------------------------------------------------------------------
private ref class WPFView : public System::Windows::UIElement, public IWPFView
{
public:
	WPFView();

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
		virtual System::Windows::UIElement^ get(void)=IWPFView::IWPFView::Element::get;
	};

	property cnWin::iWPFViewChild* ChildInterface{
		virtual cnWin::iWPFViewChild* get(void)=IWPFView::IWPFView::ChildInterface::get;
	};

	virtual bool ParentAcquire(IWPFViewParent ^Parent)=IWPFView::IWPFView::ParentAcquire;
	virtual void ParentRelease(IWPFViewParent ^Parent)=IWPFView::IWPFView::ParentRelease;

	virtual bool InsertChildElement(System::Windows::UIElement ^Child)=IWPFView::InsertChildElement;
	virtual bool RemoveChildElement(System::Windows::UIElement ^Child)=IWPFView::RemoveChildElement;

	virtual System::Windows::DependencyObject^ GetParent(void)=IWPFView::GetParent;

	virtual void DispatcherFinishCleanup(void)=IWPFView::DispatcherFinishCleanup;

internal:
	cnWin::mcWPFView *CPP;
public protected:
	virtual void OnVisualParentChanged(System::Windows::DependencyObject ^oldParent)override;
protected:
	// view children

	virtual System::Windows::Media::Visual^ GetVisualChild(int index)override;
	virtual property int VisualChildrenCount{
		virtual int get(void)override;
	};

	// layout
	
	//virtual void OnDpiChanged (System::Windows::DpiScale ^oldDpi,System::Windows::DpiScale ^newDpi)override;
	virtual System::Windows::Size MeasureCore(System::Windows::Size availableSize)override;
	virtual void ArrangeCore(System::Windows::Rect finalRect)override;
	virtual void OnRender(System::Windows::Media::DrawingContext^ drawingContext)override;
	virtual System::Windows::Media::GeometryHitTestResult^ HitTestCore(System::Windows::Media::GeometryHitTestParameters ^ hitTestParameters)override;
	virtual System::Windows::Media::HitTestResult^ HitTestCore(System::Windows::Media::PointHitTestParameters ^ hitTestParameters)override;

	static void OnIsVisibleChanged(System::Object ^sender,System::Windows::DependencyPropertyChangedEventArgs e);

	// keyboard

	virtual void OnIsKeyboardFocusedChanged(System::Windows::DependencyPropertyChangedEventArgs e)override;
	virtual void OnIsKeyboardFocusWithinChanged(System::Windows::DependencyPropertyChangedEventArgs e)override;
	virtual void OnKeyDown(System::Windows::Input::KeyEventArgs^ e)override;
	virtual void OnKeyUp(System::Windows::Input::KeyEventArgs^ e)override;


	// mouse

	virtual void OnIsMouseDirectlyOverChanged(System::Windows::DependencyPropertyChangedEventArgs e)override;
	virtual void OnMouseEnter(System::Windows::Input::MouseEventArgs^ e)override;
	virtual void OnMouseLeave(System::Windows::Input::MouseEventArgs^ e)override;
	virtual void OnMouseMove(System::Windows::Input::MouseEventArgs^ e)override;
	virtual void OnMouseDown(System::Windows::Input::MouseButtonEventArgs^ e)override;
	virtual void OnMouseUp(System::Windows::Input::MouseButtonEventArgs^ e)override;
	virtual void OnMouseWheel(System::Windows::Input::MouseWheelEventArgs^ e)override;

	// Touch

	virtual void OnTouchDown(System::Windows::Input::TouchEventArgs^ e)override;
	virtual void OnTouchUp(System::Windows::Input::TouchEventArgs^ e)override;
	virtual void OnTouchMove(System::Windows::Input::TouchEventArgs^ e)override;

};

//---------------------------------------------------------------------------
}	// namespace DNet
//---------------------------------------------------------------------------
#endif	// _MANAGED
//---------------------------------------------------------------------------
namespace cnWin{
//---------------------------------------------------------------------------
iPtr<iUIView> DNetCreateUIView(mcDNetUIThreadDispatcher *Dispatcher,mcWPFView::mcConstructParameter &Parameter)noexcept(true);
//---------------------------------------------------------------------------
#if 0

//---------------------------------------------------------------------------
class mcWPFUIElementAsUIView;
#if _MANAGED
public ref class rcWPFUIElementAsUIView : DNet::IWPFView
{
internal:
	rcWPFUIElementAsUIView(mcWPFUIElementAsUIView *CPP,System::Windows::UIElement ^Target);

	// IWPFViewParent

	property cnWin::iWPFViewParent* ParentInterface{
		virtual cnWin::iWPFViewParent* get(void)=IWPFView::IWPFViewParent::Interface::get;
	};

	virtual void ParentRemoveViewChild(DNet::IWPFView ^Subview)=IWPFView::IWPFViewParent::RemoveViewChild;

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
		virtual System::Windows::UIElement^ get(void)=IWPFView::IWPFView::Element::get;
	};

	property cnWin::iWPFViewChild* ChildInterface{
		virtual cnWin::iWPFViewChild* get(void)=IWPFView::ChildInterface::get;
	};

	virtual bool ParentAcquire(DNet::IWPFViewParent ^Parent)=IWPFView::ParentAcquire;
	virtual void ParentRelease(DNet::IWPFViewParent ^Parent)=IWPFView::ParentRelease;


	virtual bool InsertChild(System::Windows::UIElement ^Child)=IWPFView::InsertChildElement;
	virtual bool RemoveChild(System::Windows::UIElement ^Child)=IWPFView::RemoveChildElement;

	virtual System::Windows::DependencyObject^ GetParent(void)=IWPFView::GetParent;

	virtual void DispatcherFinishCleanup(void)=IWPFView::DispatcherFinishCleanup;
	
internal:

	mcWPFUIElementAsUIView *CPP;


private:
	DNet::IWPFUIWindowElement ^fViewRoot;
	System::Windows::UIElement ^fTarget;

	static mcWPFUIElementAsUIView* CPPFromSender(System::Object ^sender);

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
#endif // _MANAGED
//---------------------------------------------------------------------------
class mcWPFUIElementAsUIView : public iUIView, public iCLIObject
	, public iWPFViewChild, public iWPFViewParent
	, public cnRTL::bcVirtualLifeCycle
{
public:
#if _MANAGED
	mcWPFUIElementAsUIView(System::Windows::UIElement ^TargetElement);
	~mcWPFUIElementAsUIView();
#endif // _MANAGED

	virtual void* cnLib_FUNC CastInterface(iTypeID ID)noexcept(true) override;

	void UIViewNotifyParent(iWPFViewParent *Parent);

	virtual cGCRef& cnLib_FUNC GetObjecHandle(void)override;

	// iUIArea

	virtual iUIThread* cnLib_FUNC GetUIThread(void)override;
	virtual eUIState cnLib_FUNC GetUIState(void)override;
	virtual bool cnLib_FUNC InsertStateHandler(iUIStateHandler *Handler,sfInt16 Order)override;
	virtual bool cnLib_FUNC RemoveStateHandler(iUIStateHandler *Handler)override;
	virtual bool cnLib_FUNC GetVisible(void)override;
	virtual bool cnLib_FUNC SetVisible(bool Visible)override;
	virtual bool cnLib_FUNC IsEnabled(void)override;
	virtual bool cnLib_FUNC GetEnable(void)override;
	virtual bool cnLib_FUNC SetEnable(bool Enable)override;
	virtual Float32 cnLib_FUNC GetContentScale(void)override;
	virtual cUIPoint cnLib_FUNC GetSize(void)override;
	virtual bool cnLib_FUNC SetSize(cUIPoint Size)override;
	virtual bool cnLib_FUNC TranslatePointTo(iInterface *Relative,cUIPoint &Position)override;
	virtual bool cnLib_FUNC MoveTo(iInterface *Relative,cUIPoint Position)override;
	virtual bool cnLib_FUNC ArrangeRectangle(iInterface *Relative,cUIPoint Position,cUIPoint Size)override;
	virtual Float32 cnLib_FUNC GetZPosition(void)override;
	virtual bool cnLib_FUNC SetZPosition(Float32 ZPosition)override;

	// iUIView

	virtual bool cnLib_FUNC InsertViewHandler(iUIViewHandler *Handler,sfInt16 Order)override;
	virtual bool cnLib_FUNC RemoveViewHandler(iUIViewHandler *Handler)override;

	virtual void cnLib_FUNC SetArrangement(void)override;
	virtual void cnLib_FUNC ArrangeLayout(void)override;

	virtual void cnLib_FUNC SetIncludeParentFrameMargin(bool Include)override;
	virtual cUIRectangle cnLib_FUNC GetFrameMargin(void)override;

	virtual iUIWindow* cnLib_FUNC GetWindow(void)override;
	virtual iUIView* cnLib_FUNC GetParent(void)override;


	virtual bool cnLib_FUNC InsertView(iUIView *View)override;
	virtual bool cnLib_FUNC RemoveView(iUIView *View)override;

	// iWPFViewParent

	virtual eUIState WPFParentGetState(void)override;
	virtual Float32 WPFParentGetContentScale(void)override;
	virtual Float32 WPFParentGetLayoutScale(void)override;

	// iWPFViewChild

	virtual cGCRef& WPFChildGetElementHandle(void)override;
	
	virtual Float32 WPFChildGetLayoutScale(void)override;
	virtual Float32 WPFChildGetZPosition(void)override;
	virtual void WPFChildTreeNotifyWindow(iUIWindow *Window)override;
	virtual void WPFChildTreeNotifyState(void)override;
	virtual void WPFChildTreeNotifyScale(void)override;


protected:
	void VirtualStarted(void);
	void VirtualStopped(void);
	iWPFViewParent *fParent=nullptr;
	iUIWindow *fWindow=nullptr;
	iPtr<cDNetUIThread> fUIThread;

	bool CheckThread(void);
	bool mCheckThread(void);

	// layout

	ufInt8 fThreadMsg;
	Float32 fZPosition;
	cUIRect fRect;

	
	bool mTranslatePointTo(cGCRef &Handle,cUIPoint &Position);
	bool mSetPosition(cGCRef &Handle,cUIPoint Position);
	bool mSetRectangle(cGCRef &Handle,cUIPoint Position,cUIPoint Size);
	void mApplyRect(void);

private:
	void mSetupUIThread(void);
	void mThreadDelete(void);

	//class cUIThreadSetupProc : public miProcedure
	//{
	//	virtual void __clrcall Execute(void)override;
	//}fUIThreadSetupProc;

	
#if _MANAGED
	mcGCRefT<rcWPFUIElementAsUIView> fWPF;
#else
	cGCRef fWPF;
#endif // _MANAGED

};
#endif
//---------------------------------------------------------------------------
}	// namespace cnWin
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
