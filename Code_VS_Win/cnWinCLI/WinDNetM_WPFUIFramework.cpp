#include "WinDNetM_WPFUIFramework.h"


using namespace cnLibrary;
using namespace cnWin;
using namespace DNet;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
WPFWindowAdapter::WPFWindowAdapter(IWPFWindowTarget ^Target)
	: fTarget(Target)
{
}
//---------------------------------------------------------------------------
IWPFWindowTarget^ WPFWindowAdapter::QueryTarget(void)
{
	auto CurTarget=static_cast<IWPFWindowTarget^>(fTarget.Target);
	if(CurTarget!=nullptr){
		return CurTarget;
	}
	return nullptr;
}
//---------------------------------------------------------------------------
void WPFWindowAdapter::CPPAttach(cnWin::mcWPFViewRoot *p)
{
	if(CPP!=nullptr)
		return;
	this->CPP=p;

	auto CurTarget=QueryTarget();
	if(CurTarget!=nullptr){
		CurTarget->WindowAdapter=this;
		auto Element=dynamic_cast<System::Windows::UIElement^>(CurTarget);
		if(Element!=nullptr){
			Element->IsVisibleChanged+=gcnew System::Windows::DependencyPropertyChangedEventHandler(OnIsVisibleChanged);
			fWindowVisible=Element->IsVisible;

			auto TargetWindow=dynamic_cast<System::Windows::Window^>(CurTarget);
			if(TargetWindow!=nullptr){
				TargetWindow->Activated+=gcnew System::EventHandler(OnActivated);
			
				fWindowActive=TargetWindow->IsActive;
			}
			else{
				fWindowActive=true;
			}
		}
		else{
			fWindowVisible=true;
			fWindowActive=true;
		}
	}
	NotifyStateChange();
}
//---------------------------------------------------------------------------
void WPFWindowAdapter::CPPDetach(cnWin::mcWPFViewRoot *p)
{
	if(CPP!=p)
		return;

	auto CurTarget=QueryTarget();
	if(CurTarget!=nullptr){
		auto Element=dynamic_cast<System::Windows::UIElement^>(CurTarget);
		if(Element!=nullptr){
			Element->IsVisibleChanged-=gcnew System::Windows::DependencyPropertyChangedEventHandler(OnIsVisibleChanged);
		}
		auto TargetWindow=dynamic_cast<System::Windows::Window^>(CurTarget);
		if(TargetWindow!=nullptr){
			TargetWindow->Activated-=gcnew System::EventHandler(OnActivated);
		}
		CurTarget->WindowAdapter=nullptr;
	}
	NotifyStateChange();
}
//---------------------------------------------------------------------------
cnWin::iWPFViewParent* WPFWindowAdapter::ParentInterface::get(void)
{
	return CPP;
}
//---------------------------------------------------------------------------
void WPFWindowAdapter::ParentRemoveViewChild(IWPFView ^Subview)
{
	return CPP->WPFUIWindowRemoveSubview(Subview);
}
//---------------------------------------------------------------------------
void WPFWindowAdapter::DispatcherFinishCleanup(void)
{
	auto CurTarget=QueryTarget();
	if(CurTarget==nullptr)
		return;
	auto CurClient=CurTarget->WindowClient;
	if(CurClient==nullptr)
		return;

	// clear child
	CurTarget->WindowClient=nullptr;

	// notify release old child
	auto CurClientViewChild=dynamic_cast<IWPFView^>(CurClient);
	CurClientViewChild->ParentRelease(this);
}
//---------------------------------------------------------------------------
System::Windows::UIElement^ WPFWindowAdapter::ClientElement::get()
{
	auto CurTarget=QueryTarget();
	if(CurTarget==nullptr)
		return nullptr;
	return CurTarget->WindowClient;
}
//---------------------------------------------------------------------------
void WPFWindowAdapter::ClientElement::set(System::Windows::UIElement ^Element)
{
	auto CurTarget=QueryTarget();
	if(CurTarget==nullptr)
		return;

	// set child
	CurTarget->WindowClient=Element;
}
//---------------------------------------------------------------------------
WPFWindowAdapter^ WPFWindowAdapter::ThisFromSender(System::Object ^sender)
{
	auto Element=dynamic_cast<IWPFWindowTarget^>(sender);
	if(Element!=nullptr){
		auto Adapter=Element->WindowAdapter;
		if(Adapter!=nullptr){
			return Adapter;
		}
	}
	return nullptr;
}
//---------------------------------------------------------------------------
void WPFWindowAdapter::OnIsVisibleChanged(System::Object ^sender,System::Windows::DependencyPropertyChangedEventArgs e)
{
	auto This=ThisFromSender(sender);
	if(This!=nullptr){
		This->UpdateVisible(static_cast<System::Boolean>(e.NewValue));
	}
}
//---------------------------------------------------------------------------
void WPFWindowAdapter::OnActivated(System::Object ^sender,System::EventArgs ^)
{
	auto This=ThisFromSender(sender);

	if(This!=nullptr){
		auto TargetWindow=static_cast<System::Windows::Window^>(sender);
		This->UpdateActive(TargetWindow->IsActive);
	}
}
//---------------------------------------------------------------------------
void WPFWindowAdapter::UpdateVisible(bool Visible)
{
	if(fWindowVisible==Visible)
		return;

	fWindowVisible=Visible;

	NotifyStateChange();
}
//---------------------------------------------------------------------------
void WPFWindowAdapter::UpdateActive(bool Active)
{
	if(fWindowActive==Active)
		return;

	fWindowActive=Active;

	NotifyStateChange();
}
//---------------------------------------------------------------------------
void WPFWindowAdapter::NotifyStateChange(void)
{
	if(CPP==nullptr)
		return;

	eUIState NewState;
	if(fTarget.IsAlive==false){
		NewState=UIState::Null;
	}
	else if(fWindowVisible==false){
		NewState=UIState::Background;
	}
	else if(fWindowActive==false){
		NewState=UIState::Inactive;
	}
	else{
		NewState=UIState::Active;
	}
	CPP->WPFWindowStateChanged(NewState);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
WPFWindowAdapter^ WPFWindow::WindowAdapter::get(void)
{
	return fWindowAdapter;
}
//---------------------------------------------------------------------------
void WPFWindow::WindowAdapter::set(WPFWindowAdapter ^value)
{
	fWindowAdapter=value;
}
//---------------------------------------------------------------------------
System::Windows::UIElement^ WPFWindow::WindowClient::get(void)
{
	return dynamic_cast<UIElement^>(Content);
}
//---------------------------------------------------------------------------
void WPFWindow::WindowClient::set(System::Windows::UIElement ^value)
{
	Content=value;
}
//---------------------------------------------------------------------------
iPtr<iUIWindow> cnWin::MakeUIWindowFromWindowTarget(IWPFWindowTarget ^Target)noexcept
{
	auto TargetObject=dynamic_cast<System::Windows::DependencyObject^>(Target);
	if(TargetObject==nullptr)
		return nullptr;

	if(TargetObject->CheckAccess()==false)
		return nullptr;

	auto Dispatcher=mcDNetUIThreadDispatcher::mCurrentUIDispatcher();
	if(Dispatcher==nullptr)
		return nullptr;

	//auto TargetWindow=dynamic_cast<System::Windows::Window^>(Target);
	//if(TargetWindow!=nullptr){
	//	auto Helper=gcnew System::Windows::Interop::WindowInteropHelper(TargetWindow);
	//	auto TargetWindowSource=System::Windows::Interop::HwndSource::FromHwnd(Helper->Handle);
	//	if(TargetWindowSource!=nullptr){
	//		DNetCreateWPFHwndSourceAsWindow(Dispatcher);
	//	}
	//}

	mcWPFViewRoot::mcConstructParameter Param={
		gcnew WPFWindowAdapter(Target)
	};

	return DNetCreateWPFWindowAsWindowClient(Dispatcher,Param);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
WPFViewAdapter::WPFViewAdapter(IWPFViewTarget ^Target)
	: fTarget(Target)
{
}
//---------------------------------------------------------------------------
IWPFViewTarget ^WPFViewAdapter::QueryTarget(void)
{
	auto CurTarget=fTarget.Target;
	if(CurTarget!=nullptr){
		return static_cast<IWPFViewTarget^>(CurTarget);
	}
	return nullptr;
}
//---------------------------------------------------------------------------
template<class T>
T ^WPFViewAdapter::QueryTarget(void)
{
	auto CurTarget=fTarget.Target;
	if(CurTarget!=nullptr){
		return dynamic_cast<T^>(CurTarget);
	}
	return nullptr;
}
//---------------------------------------------------------------------------
void WPFViewAdapter::CPPAttach(cnWin::mcWPFView *p)
{
	if(CPP!=nullptr)
		return;
	CPP=p;

	auto CurTarget=QueryTarget();
	if(CurTarget!=nullptr){
		CurTarget->ViewAdapter=this;
		auto TargetElement=dynamic_cast<System::Windows::UIElement^>(CurTarget);
		if(TargetElement!=nullptr){
			TargetElement->IsVisibleChanged+=gcnew System::Windows::DependencyPropertyChangedEventHandler(OnIsVisibleChanged);
			TargetElement->IsKeyboardFocusedChanged+=gcnew System::Windows::DependencyPropertyChangedEventHandler(OnIsKeyboardFocusedChanged);
			TargetElement->IsKeyboardFocusWithinChanged+=gcnew System::Windows::DependencyPropertyChangedEventHandler(OnIsKeyboardFocusWithinChanged);
			TargetElement->KeyDown+=gcnew System::Windows::Input::KeyEventHandler(OnKeyDown);
			TargetElement->KeyUp+=gcnew System::Windows::Input::KeyEventHandler(OnKeyUp);
			TargetElement->IsMouseDirectlyOverChanged+=gcnew System::Windows::DependencyPropertyChangedEventHandler(OnIsMouseDirectlyOverChanged);
			TargetElement->MouseEnter+=gcnew System::Windows::Input::MouseEventHandler(OnMouseEnter);
			TargetElement->MouseLeave+=gcnew System::Windows::Input::MouseEventHandler(OnMouseLeave);
			TargetElement->MouseMove+=gcnew System::Windows::Input::MouseEventHandler(OnMouseMove);
			TargetElement->MouseDown+=gcnew System::Windows::Input::MouseButtonEventHandler(OnMouseDown);
			TargetElement->MouseUp+=gcnew System::Windows::Input::MouseButtonEventHandler(OnMouseUp);
			TargetElement->MouseWheel+=gcnew System::Windows::Input::MouseWheelEventHandler(OnMouseWheel);
			TargetElement->TouchDown+=gcnew System::EventHandler<System::Windows::Input::TouchEventArgs^>(OnTouchDown);
			TargetElement->TouchUp+=gcnew System::EventHandler<System::Windows::Input::TouchEventArgs^>(OnTouchUp);
			TargetElement->TouchMove+=gcnew System::EventHandler<System::Windows::Input::TouchEventArgs^>(OnTouchMove);
		}
	}
}
//---------------------------------------------------------------------------
void WPFViewAdapter::CPPDetach(cnWin::mcWPFView *p)
{
	if(CPP!=p)
		return;
	CPP=nullptr;

	auto CurTarget=QueryTarget();
	if(CurTarget!=nullptr){
		auto TargetElement=dynamic_cast<System::Windows::UIElement^>(CurTarget);
		if(TargetElement!=nullptr){
			TargetElement->IsVisibleChanged-=gcnew System::Windows::DependencyPropertyChangedEventHandler(OnIsVisibleChanged);
			TargetElement->IsKeyboardFocusedChanged-=gcnew System::Windows::DependencyPropertyChangedEventHandler(OnIsKeyboardFocusedChanged);
			TargetElement->IsKeyboardFocusWithinChanged-=gcnew System::Windows::DependencyPropertyChangedEventHandler(OnIsKeyboardFocusWithinChanged);
			TargetElement->KeyDown-=gcnew System::Windows::Input::KeyEventHandler(OnKeyDown);
			TargetElement->KeyUp-=gcnew System::Windows::Input::KeyEventHandler(OnKeyUp);
			TargetElement->IsMouseDirectlyOverChanged-=gcnew System::Windows::DependencyPropertyChangedEventHandler(OnIsMouseDirectlyOverChanged);
			TargetElement->MouseEnter-=gcnew System::Windows::Input::MouseEventHandler(OnMouseEnter);
			TargetElement->MouseLeave-=gcnew System::Windows::Input::MouseEventHandler(OnMouseLeave);
			TargetElement->MouseMove-=gcnew System::Windows::Input::MouseEventHandler(OnMouseMove);
			TargetElement->MouseDown-=gcnew System::Windows::Input::MouseButtonEventHandler(OnMouseDown);
			TargetElement->MouseUp-=gcnew System::Windows::Input::MouseButtonEventHandler(OnMouseUp);
			TargetElement->MouseWheel-=gcnew System::Windows::Input::MouseWheelEventHandler(OnMouseWheel);
			TargetElement->TouchDown-=gcnew System::EventHandler<System::Windows::Input::TouchEventArgs^>(OnTouchDown);
			TargetElement->TouchUp-=gcnew System::EventHandler<System::Windows::Input::TouchEventArgs^>(OnTouchUp);
			TargetElement->TouchMove-=gcnew System::EventHandler<System::Windows::Input::TouchEventArgs^>(OnTouchMove);
		}
		CurTarget->ViewAdapter=nullptr;
	}
}
//---------------------------------------------------------------------------
int WPFViewAdapter::VisualChildrenCount::get(void)
{
	return CPP->WPFViewGetVisualChildrenCount();
}
//---------------------------------------------------------------------------
System::Windows::Media::Visual^ WPFViewAdapter::GetVisualChild(int index)
{
	return CPP->WPFViewGetVisualChild(index);
}
//---------------------------------------------------------------------------
void WPFViewAdapter::OnVisualParentChanged(System::Windows::DependencyObject ^NewParent)
{
	return CPP->WPFViewOnVisualParentChanged(NewParent);
}
//---------------------------------------------------------------------------
System::Windows::Size WPFViewAdapter::MeasureCore(System::Windows::Size availableSize)
{
	return CPP->WPFViewMeasureCore(availableSize);
}
//---------------------------------------------------------------------------
void WPFViewAdapter::ArrangeCore(System::Windows::Rect finalRect)
{
	return CPP->WPFViewArrangeCore(finalRect);
}
//---------------------------------------------------------------------------
System::Windows::Size WPFViewAdapter::MeasureOverride(System::Windows::Size availableSize)
{
	return CPP->WPFViewMeasureCore(availableSize);
}
//---------------------------------------------------------------------------
System::Windows::Size WPFViewAdapter::ArrangeOverride(System::Windows::Size finalSize)
{
	System::Windows::Rect finalRect;
	finalRect.X=0;
	finalRect.Y=0;
	finalRect.Size=finalSize;
	CPP->WPFViewArrangeCore(finalRect);
	return finalSize;
}
//---------------------------------------------------------------------------
void WPFViewAdapter::OnRender(System::Windows::Media::DrawingContext^ drawingContext)
{
	return CPP->WPFViewOnRender(drawingContext);
}
//---------------------------------------------------------------------------
System::Windows::Media::GeometryHitTestResult^ WPFViewAdapter::HitTestCore(System::Windows::Media::GeometryHitTestParameters ^ hitTestParameters)
{
	System::Windows::Media::IntersectionDetail HitDetail;
	if(CPP->WPFViewHitTestCore(HitDetail,hitTestParameters)){
		auto TargetVisual=QueryTarget<System::Windows::Media::Visual>();
		if(TargetVisual!=nullptr){
			auto HitTestResult=gcnew System::Windows::Media::GeometryHitTestResult(TargetVisual,HitDetail);
			return HitTestResult;
		}
	}
	return nullptr;
}
//---------------------------------------------------------------------------
System::Windows::Media::HitTestResult^ WPFViewAdapter::HitTestCore(System::Windows::Media::PointHitTestParameters ^ hitTestParameters)
{
	System::Windows::Point HitPoint=hitTestParameters->HitPoint;
	if(CPP->WPFViewHitTestCore(HitPoint)){
		auto TargetVisual=QueryTarget<System::Windows::Media::Visual>();
		if(TargetVisual!=nullptr){
			auto HitTestResult=gcnew System::Windows::Media::PointHitTestResult(TargetVisual,HitPoint);
			return HitTestResult;
		}
	}
	return nullptr;
}
//---------------------------------------------------------------------------
bool WPFViewAdapter::InsertChild(System::Windows::UIElement ^Child)
{
	auto CurTarget=QueryTarget();
	if(CurTarget==nullptr)
		return false;
	return CurTarget->ViewAddVisualChild(Child);
}
//---------------------------------------------------------------------------
bool WPFViewAdapter::RemoveChild(System::Windows::UIElement ^Child)
{
	auto CurTarget=QueryTarget();
	if(CurTarget==nullptr)
		return false;
	return CurTarget->ViewRemoveVisualChild(Child);
}
//---------------------------------------------------------------------------
System::Windows::DependencyObject^ WPFViewAdapter::GetParent(void)
{
	auto CurTarget=QueryTarget();
	if(CurTarget==nullptr)
		return nullptr;
	return CurTarget->ViewParent;
}
//---------------------------------------------------------------------------
void WPFViewAdapter::DispatcherFinishCleanup(void)
{
	fTarget.Target=nullptr;
}
//---------------------------------------------------------------------------
cnWin::iWPFViewParent* WPFViewAdapter::ParentInterface::get(void)
{
	return CPP;
}
//---------------------------------------------------------------------------
void WPFViewAdapter::ParentRemoveViewChild(IWPFView ^Subview)
{
	return CPP->WPFParentRemoveChild(Subview);
}
//---------------------------------------------------------------------------
System::Windows::Media::Visual^ WPFViewAdapter::Visual::get(void)
{
	return QueryTarget<System::Windows::Media::Visual>();
}
//---------------------------------------------------------------------------
float WPFViewAdapter::LayoutScale::get(void)
{
	return CPP->WPFChildGetLayoutScale();
}
//--------------------------------------------------------------------------
System::Windows::UIElement^ WPFViewAdapter::Element::get(void)
{
	return QueryTarget<System::Windows::UIElement>();
}
//---------------------------------------------------------------------------
cnWin::iWPFViewChild* WPFViewAdapter::ChildInterface::get(void)
{
	return CPP;
}
//---------------------------------------------------------------------------
bool WPFViewAdapter::ParentAcquire(IWPFViewParent ^Parent)
{
	return CPP->WPFViewParentAcquire(Parent);
}
//---------------------------------------------------------------------------
void WPFViewAdapter::ParentRelease(IWPFViewParent ^Parent)
{
	return CPP->WPFViewParentRelease(Parent);
}
//---------------------------------------------------------------------------
mcWPFView* WPFViewAdapter::CPPFromSender(System::Object ^sender)
{
	auto ViewTarget=dynamic_cast<IWPFViewTarget^>(sender);
	if(ViewTarget!=nullptr){
		return ViewTarget->ViewAdapter->CPP;
	}
	return nullptr;
}
//---------------------------------------------------------------------------
void WPFViewAdapter::OnIsVisibleChanged(System::Object ^sender,System::Windows::DependencyPropertyChangedEventArgs e)
{
	auto CPP=CPPFromSender(sender);
	if(CPP!=nullptr){
		CPP->WPFViewIsVisibleChanged(e);
	}
}
//---------------------------------------------------------------------------
void WPFViewAdapter::OnIsKeyboardFocusedChanged(System::Object ^sender,System::Windows::DependencyPropertyChangedEventArgs e)
{
	auto CPP=CPPFromSender(sender);
	if(CPP!=nullptr){
		CPP->WPFViewOnIsKeyboardFocusedChanged(e);
	}
}
//---------------------------------------------------------------------------
void WPFViewAdapter::OnIsKeyboardFocusWithinChanged(System::Object ^sender,System::Windows::DependencyPropertyChangedEventArgs e)
{
	auto CPP=CPPFromSender(sender);
	if(CPP!=nullptr){
		CPP->WPFViewOnIsKeyboardFocusWithinChanged(e);
	}
}
//---------------------------------------------------------------------------
void WPFViewAdapter::OnKeyDown(System::Object ^sender,System::Windows::Input::KeyEventArgs^ e)
{
	auto CPP=CPPFromSender(sender);
	if(CPP!=nullptr){
		return CPP->WPFViewOnKeyDown(e);
	}
}
//---------------------------------------------------------------------------
void WPFViewAdapter::OnKeyUp(System::Object ^sender,System::Windows::Input::KeyEventArgs^ e)
{
	auto CPP=CPPFromSender(sender);
	if(CPP!=nullptr){
		return CPP->WPFViewOnKeyUp(e);
	}
}
//---------------------------------------------------------------------------
void WPFViewAdapter::OnIsMouseDirectlyOverChanged(System::Object ^sender,System::Windows::DependencyPropertyChangedEventArgs e)
{
	auto CPP=CPPFromSender(sender);
	if(CPP!=nullptr){
		return CPP->WPFViewOnIsMouseDirectlyOverChanged(e);
	}
}
//---------------------------------------------------------------------------
void WPFViewAdapter::OnMouseEnter(System::Object ^sender,System::Windows::Input::MouseEventArgs^ e)
{
	auto CPP=CPPFromSender(sender);
	if(CPP!=nullptr){
		return CPP->WPFViewOnMouseEnter(e);
	}
}
//---------------------------------------------------------------------------
void WPFViewAdapter::OnMouseLeave(System::Object ^sender,System::Windows::Input::MouseEventArgs^ e)
{
	auto CPP=CPPFromSender(sender);
	if(CPP!=nullptr){
		return CPP->WPFViewOnMouseLeave(e);
	}
}
//---------------------------------------------------------------------------
void WPFViewAdapter::OnMouseMove(System::Object ^sender,System::Windows::Input::MouseEventArgs^ e)
{
	auto CPP=CPPFromSender(sender);
	if(CPP!=nullptr){
		return CPP->WPFViewOnMouseMove(e);
	}
}
//---------------------------------------------------------------------------
void WPFViewAdapter::OnMouseDown(System::Object ^sender,System::Windows::Input::MouseButtonEventArgs^ e)
{
	auto CPP=CPPFromSender(sender);
	if(CPP!=nullptr){
		return CPP->WPFViewOnMouseDown(e);
	}
}
//---------------------------------------------------------------------------
void WPFViewAdapter::OnMouseUp(System::Object ^sender,System::Windows::Input::MouseButtonEventArgs^ e)
{
	auto CPP=CPPFromSender(sender);
	if(CPP!=nullptr){
		return CPP->WPFViewOnMouseUp(e);
	}
}
//---------------------------------------------------------------------------
void WPFViewAdapter::OnMouseWheel(System::Object ^sender,System::Windows::Input::MouseWheelEventArgs^ e)
{
	auto CPP=CPPFromSender(sender);
	if(CPP!=nullptr){
		return CPP->WPFViewOnMouseWheel(e);
	}
}
//---------------------------------------------------------------------------
void WPFViewAdapter::OnTouchDown(System::Object ^sender,System::Windows::Input::TouchEventArgs^ e)
{
	auto CPP=CPPFromSender(sender);
	if(CPP!=nullptr){
		return CPP->WPFViewOnTouchDown(e);
	}
}
//---------------------------------------------------------------------------
void WPFViewAdapter::OnTouchUp(System::Object ^sender,System::Windows::Input::TouchEventArgs^ e)
{
	auto CPP=CPPFromSender(sender);
	if(CPP!=nullptr){
		return CPP->WPFViewOnTouchUp(e);
	}
}
//---------------------------------------------------------------------------
void WPFViewAdapter::OnTouchMove(System::Object ^sender,System::Windows::Input::TouchEventArgs^ e)
{
	auto CPP=CPPFromSender(sender);
	if(CPP!=nullptr){
		return CPP->WPFViewOnTouchMove(e);
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
iPtr<iUIView> cnWin::MakeUIViewFromViewTarget(IWPFViewTarget ^Target)noexcept
{
	auto TargetObject=dynamic_cast<System::Windows::DependencyObject^>(Target);
	if(TargetObject==nullptr)
		return nullptr;

	if(TargetObject->CheckAccess()==false)
		return nullptr;

	auto Dispatcher=mcDNetUIThreadDispatcher::mCurrentUIDispatcher();
	if(Dispatcher==nullptr)
		return nullptr;

	mcWPFView::mcConstructParameter Param={
		gcnew WPFViewAdapter(Target)
	};

	return DNetCreateUIView(Dispatcher,Param);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
WPFViewAdapter ^WPFNativeFrameworkElement::ViewAdapter::get(void)
{
	return fViewAdapter;
}
//---------------------------------------------------------------------------
void WPFNativeFrameworkElement::ViewAdapter::set(WPFViewAdapter^ value)
{
	fViewAdapter=value;
}
//---------------------------------------------------------------------------
System::Windows::DependencyObject ^WPFNativeFrameworkElement::ViewParent::get(void)
{
	return __super::VisualParent;
}
//---------------------------------------------------------------------------
bool WPFNativeFrameworkElement::ViewAddVisualChild(System::Windows::UIElement ^Child)
{
	try{
		this->AddVisualChild(Child);
		return true;
	}
	catch(System::Exception ^){
		return false;
	}
}
//---------------------------------------------------------------------------
bool WPFNativeFrameworkElement::ViewRemoveVisualChild(System::Windows::UIElement ^Child)
{
	try{
		this->RemoveVisualChild(Child);
		return true;
	}
	catch(System::Exception ^){
		return false;
	}
}
//---------------------------------------------------------------------------
System::Windows::Media::Visual^ WPFNativeFrameworkElement::GetVisualChild(int index)
{
	if(fViewAdapter==nullptr)
		return nullptr;
	return fViewAdapter->GetVisualChild(index);
}
//---------------------------------------------------------------------------
int WPFNativeFrameworkElement::VisualChildrenCount::get(void)
{
	if(fViewAdapter==nullptr)
		return 0;
	return fViewAdapter->VisualChildrenCount;
}
//---------------------------------------------------------------------------
void WPFNativeFrameworkElement::OnVisualParentChanged(System::Windows::DependencyObject ^oldParent)
{
	__super::OnVisualParentChanged(oldParent);
	if(fViewAdapter!=nullptr){
		fViewAdapter->OnVisualParentChanged(VisualParent);
	}
}
//---------------------------------------------------------------------------
System::Windows::Size WPFNativeFrameworkElement::MeasureOverride(System::Windows::Size availableSize)
{
	if(fViewAdapter==nullptr)
		return __super::MeasureOverride(availableSize);
	return fViewAdapter->MeasureOverride(availableSize);
}
//---------------------------------------------------------------------------
System::Windows::Size WPFNativeFrameworkElement::ArrangeOverride(System::Windows::Size finalSize)
{
	if(fViewAdapter==nullptr)
		return __super::ArrangeOverride(finalSize);
	return fViewAdapter->ArrangeOverride(finalSize);
}
//---------------------------------------------------------------------------
void WPFNativeFrameworkElement::OnRender(System::Windows::Media::DrawingContext^ drawingContext)
{
	if(fViewAdapter==nullptr)
		return;
	return fViewAdapter->OnRender(drawingContext);
}
//---------------------------------------------------------------------------
System::Windows::Media::GeometryHitTestResult^ WPFNativeFrameworkElement::HitTestCore(System::Windows::Media::GeometryHitTestParameters ^ hitTestParameters)
{
	if(fViewAdapter==nullptr)
		return __super::HitTestCore(hitTestParameters);
	return fViewAdapter->HitTestCore(hitTestParameters);
}
//---------------------------------------------------------------------------
System::Windows::Media::HitTestResult^ WPFNativeFrameworkElement::HitTestCore(System::Windows::Media::PointHitTestParameters ^ hitTestParameters)
{
	if(fViewAdapter==nullptr)
		return __super::HitTestCore(hitTestParameters);
	return fViewAdapter->HitTestCore(hitTestParameters);
}
//---------------------------------------------------------------------------

