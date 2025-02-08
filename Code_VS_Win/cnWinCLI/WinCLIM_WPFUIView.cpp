#include "WinCLIM_WPFUIView.h"
#include "WinCLIM_WPFUIWindow.h"

using namespace cnLibrary;

using namespace cnWin;
using namespace DNet;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void cnWin::mMakeWPFView(cGCRef &Ref)noexcept(true)
{
	Ref.Set(gcnew WPFView());
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
static rcWPFExternalVisualRemoteRoot::rcWPFExternalVisualRemoteRoot()
{
	VisualRemoteRootProperty=System::Windows::DependencyProperty::Register("cnLibraryExternalVisualRootManager",
		rcWPFExternalVisualRemoteRoot::typeid,System::Windows::Media::Visual::typeid);
}
//---------------------------------------------------------------------------
rcWPFExternalVisualRemoteRoot^ rcWPFExternalVisualRemoteRoot::Query(System::Windows::Media::Visual ^TargetVisual)
{
	auto Object=TargetVisual->GetValue(VisualRemoteRootProperty);
	if(Object!=nullptr){
		return static_cast<rcWPFExternalVisualRemoteRoot^>(Object);
	}

	return gcnew rcWPFExternalVisualRemoteRoot(TargetVisual);
}
//---------------------------------------------------------------------------
rcWPFExternalVisualRemoteRoot::rcWPFExternalVisualRemoteRoot(System::Windows::Media::Visual ^TargetVisual)
	: fTargetVisual(TargetVisual)
{
}
//---------------------------------------------------------------------------
void rcWPFExternalVisualRemoteRoot::RemoteChildAttach(DNet::IWPFUIViewTargetElement ^RemoteChild)
{
	if(fChildCount==0){
		CPPSetup();
	}
	fChildCount++;
}
//---------------------------------------------------------------------------
void rcWPFExternalVisualRemoteRoot::RemoteChildDetach(DNet::IWPFUIViewTargetElement ^RemoteChild)
{
	fChildCount--;

	if(fChildCount==0){
		CPPClear();
	}
}
//---------------------------------------------------------------------------
void rcWPFExternalVisualRemoteRoot::CPPSetup(void)
{
	fTargetVisual->SetValue(VisualRemoteRootProperty,this);
}
//---------------------------------------------------------------------------
void rcWPFExternalVisualRemoteRoot::CPPClear(void)
{
	fTargetVisual->SetValue(VisualRemoteRootProperty,nullptr);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void WPFUIViewAdapter::CPPAttach(cnWin::mbcWPFUIView *p)
{
	if(CPP!=nullptr)
		return;
	auto CurTarget=p->fTargetElement.Get();
	if(&CurTarget->Adapter!=this){
		return;
	}
	ContentDrawingGroup=gcnew System::Windows::Media::DrawingGroup();

	CPP=p;
	if(CurTarget!=nullptr){
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
void WPFUIViewAdapter::CPPDetach(cnWin::mbcWPFUIView *p)
{
	if(CPP!=p)
		return;

	auto CurTarget=CPP->fTargetElement.Get();
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
	}

	ContentDrawingGroup=nullptr;
	CPP=nullptr;
}
//---------------------------------------------------------------------------
int WPFUIViewAdapter::VisualChildrenCount::get(void)
{
	if(CPP==nullptr)
		return 0;
	if(ChildrenCache==nullptr){
		CPP->WPFSetupChildrenCache(*this);
	}
	return ChildrenCache->Length;
}
//---------------------------------------------------------------------------
System::Windows::Media::Visual^ WPFUIViewAdapter::GetVisualChild(int index)
{
	if(CPP==nullptr)
		return nullptr;
	return ChildrenCache[index];
}
//---------------------------------------------------------------------------
void WPFUIViewAdapter::OnVisualParentChanged(System::Windows::DependencyObject ^NewParent)
{
	if(fRemoteRoot!=nullptr){
		auto TargetElement=CPP->fTargetElement.Get();
		System::Windows::PresentationSource::RemoveSourceChangedHandler(dynamic_cast<System::Windows::IInputElement^>(TargetElement),gcnew System::Windows::SourceChangedEventHandler(TargetOnPresentationSourceChanged));

		CPP->WPFDetachExtParent();
		fRemoteRoot->RemoteChildDetach(TargetElement);
		fRemoteRoot=nullptr;
	}

	if(CPP==nullptr)
		return;
	if(CPP->WPFAttachExtParent()){
		// external wpf element as parent
		auto TargetElement=CPP->fTargetElement.Get();
		auto UIViewVisual=CPP->fTargetElement.DynamicCast<System::Windows::Media::Visual>();
		if(UIViewVisual==nullptr)
			return;

		
		auto TargetInputElement=dynamic_cast<System::Windows::IInputElement^>(UIViewVisual);
		if(TargetInputElement==nullptr)
			return;

		System::Windows::PresentationSource::AddSourceChangedHandler(TargetInputElement,gcnew System::Windows::SourceChangedEventHandler(TargetOnPresentationSourceChanged));
		
		auto CurrentPresentSource=System::Windows::PresentationSource::FromVisual(UIViewVisual);
		if(CurrentPresentSource!=nullptr){
			auto CurrentRootVisual=CurrentPresentSource->RootVisual;
			if(CurrentRootVisual==nullptr){
				// ???
				return;
			}
			
			fRemoteRoot=WPFUIViewQueryRemoteRootFromRootVisual(CurrentRootVisual);
			if(fRemoteRoot==nullptr){
				fRemoteRoot=rcWPFExternalVisualRemoteRoot::Query(CurrentRootVisual);
			}
			fRemoteRoot->RemoteChildAttach(TargetElement);
		}
	}
}
//---------------------------------------------------------------------------
System::Windows::Size WPFUIViewAdapter::MeasureCore(System::Windows::Size availableSize)
{
	if(CPP==nullptr)
		return System::Windows::Size(0,0);
	return WPFRect.Size;
}
//---------------------------------------------------------------------------
void WPFUIViewAdapter::ArrangeCore(System::Windows::Rect finalRect)
{
	if(CPP==nullptr)
		return;
	return CPP->WPFViewArrangeCore(*this,finalRect);
}
//---------------------------------------------------------------------------
System::Windows::Size WPFUIViewAdapter::MeasureOverride(System::Windows::Size availableSize)
{
	if(CPP==nullptr)
		return availableSize;
	return WPFRect.Size;
}
//---------------------------------------------------------------------------
System::Windows::Size WPFUIViewAdapter::ArrangeOverride(System::Windows::Size finalSize)
{
	if(CPP==nullptr)
		return finalSize;
	System::Windows::Rect finalRect;
	finalRect.X=0;
	finalRect.Y=0;
	finalRect.Size=finalSize;
	CPP->WPFViewArrangeCore(*this,finalRect);
	return finalSize;
}
//---------------------------------------------------------------------------
void WPFUIViewAdapter::OnRender(System::Windows::Media::DrawingContext^ drawingContext)
{
	if(CPP==nullptr)
		return;
	drawingContext->DrawDrawing(ContentDrawingGroup);
}
//---------------------------------------------------------------------------
System::Windows::Media::GeometryHitTestResult^ WPFUIViewAdapter::HitTestCore(System::Windows::Media::GeometryHitTestParameters ^ hitTestParameters)
{
	if(CPP!=nullptr){
		System::Windows::Media::IntersectionDetail HitDetail;
		if(CPP->WPFViewHitTestCore(*this,HitDetail,hitTestParameters)){
			auto TargetVisual=CPP->fTargetElement.DynamicCast<System::Windows::Media::Visual>();
			if(TargetVisual!=nullptr){
				auto HitTestResult=gcnew System::Windows::Media::GeometryHitTestResult(TargetVisual,HitDetail);
				return HitTestResult;
			}
		}
	}
	return nullptr;
}
//---------------------------------------------------------------------------
System::Windows::Media::HitTestResult^ WPFUIViewAdapter::HitTestCore(System::Windows::Media::PointHitTestParameters ^ hitTestParameters)
{
	if(CPP!=nullptr){
		System::Windows::Point HitPoint=hitTestParameters->HitPoint;
		if(CPP->WPFViewHitTestCore(*this,HitPoint)){
			auto TargetVisual=CPP->fTargetElement.DynamicCast<System::Windows::Media::Visual>();
			if(TargetVisual!=nullptr){
				auto HitTestResult=gcnew System::Windows::Media::PointHitTestResult(TargetVisual,HitPoint);
				return HitTestResult;
			}
		}
	}
	return nullptr;
}
//---------------------------------------------------------------------------
mbcWPFUIView* WPFUIViewAdapter::CPPFromSender(System::Object ^sender)
{
	auto ViewTarget=dynamic_cast<IWPFUIViewTargetElement^>(sender);
	if(ViewTarget!=nullptr){
		return ViewTarget->Adapter.CPP;
	}
	return nullptr;
}
//---------------------------------------------------------------------------
void WPFUIViewAdapter::TargetOnPresentationSourceChanged(System::Object ^sender,System::Windows::SourceChangedEventArgs ^e)
{
	auto CPP=CPPFromSender(sender);
	if(CPP!=nullptr){
		CPP->WPFViewOnPresentationSourceChanged(e);
	}
}
//---------------------------------------------------------------------------
void WPFUIViewAdapter::OnIsVisibleChanged(System::Object ^sender,System::Windows::DependencyPropertyChangedEventArgs e)
{
	auto CPP=CPPFromSender(sender);
	if(CPP!=nullptr){
		CPP->WPFNotifyVisible(static_cast<System::Boolean>(e.NewValue));
	}
}
//---------------------------------------------------------------------------
void WPFUIViewAdapter::OnIsKeyboardFocusedChanged(System::Object ^sender,System::Windows::DependencyPropertyChangedEventArgs e)
{
	auto CPP=CPPFromSender(sender);
	if(CPP!=nullptr){
		CPP->WPFViewOnIsKeyboardFocusedChanged(e);
	}
}
//---------------------------------------------------------------------------
void WPFUIViewAdapter::OnIsKeyboardFocusWithinChanged(System::Object ^sender,System::Windows::DependencyPropertyChangedEventArgs e)
{
	auto CPP=CPPFromSender(sender);
	if(CPP!=nullptr){
		CPP->WPFViewOnIsKeyboardFocusWithinChanged(e);
	}
}
//---------------------------------------------------------------------------
void WPFUIViewAdapter::OnKeyDown(System::Object ^sender,System::Windows::Input::KeyEventArgs^ e)
{
	auto CPP=CPPFromSender(sender);
	if(CPP!=nullptr){
		return CPP->WPFViewOnKeyDown(sender,e);
	}
}
//---------------------------------------------------------------------------
void WPFUIViewAdapter::OnKeyUp(System::Object ^sender,System::Windows::Input::KeyEventArgs^ e)
{
	auto CPP=CPPFromSender(sender);
	if(CPP!=nullptr){
		return CPP->WPFViewOnKeyUp(sender,e);
	}
}
//---------------------------------------------------------------------------
void WPFUIViewAdapter::OnIsMouseDirectlyOverChanged(System::Object ^sender,System::Windows::DependencyPropertyChangedEventArgs e)
{
	auto CPP=CPPFromSender(sender);
	if(CPP!=nullptr){
		return CPP->WPFViewOnIsMouseDirectlyOverChanged(e);
	}
}
//---------------------------------------------------------------------------
void WPFUIViewAdapter::OnMouseEnter(System::Object ^sender,System::Windows::Input::MouseEventArgs^ e)
{
	auto CPP=CPPFromSender(sender);
	if(CPP!=nullptr){
		return CPP->WPFViewOnMouseEnter(e);
	}
}
//---------------------------------------------------------------------------
void WPFUIViewAdapter::OnMouseLeave(System::Object ^sender,System::Windows::Input::MouseEventArgs^ e)
{
	auto CPP=CPPFromSender(sender);
	if(CPP!=nullptr){
		return CPP->WPFViewOnMouseLeave(e);
	}
}
//---------------------------------------------------------------------------
void WPFUIViewAdapter::OnMouseMove(System::Object ^sender,System::Windows::Input::MouseEventArgs^ e)
{
	auto CPP=CPPFromSender(sender);
	if(CPP!=nullptr){
		return CPP->WPFViewOnMouseMove(sender,e);
	}
}
//---------------------------------------------------------------------------
void WPFUIViewAdapter::OnMouseDown(System::Object ^sender,System::Windows::Input::MouseButtonEventArgs^ e)
{
	auto CPP=CPPFromSender(sender);
	if(CPP!=nullptr){
		return CPP->WPFViewOnMouseDown(sender,e);
	}
}
//---------------------------------------------------------------------------
void WPFUIViewAdapter::OnMouseUp(System::Object ^sender,System::Windows::Input::MouseButtonEventArgs^ e)
{
	auto CPP=CPPFromSender(sender);
	if(CPP!=nullptr){
		return CPP->WPFViewOnMouseUp(sender,e);
	}
}
//---------------------------------------------------------------------------
void WPFUIViewAdapter::OnMouseWheel(System::Object ^sender,System::Windows::Input::MouseWheelEventArgs^ e)
{
	auto CPP=CPPFromSender(sender);
	if(CPP!=nullptr){
		return CPP->WPFViewOnMouseWheel(sender,e);
	}
}
//---------------------------------------------------------------------------
void WPFUIViewAdapter::OnTouchDown(System::Object ^sender,System::Windows::Input::TouchEventArgs^ e)
{
	auto CPP=CPPFromSender(sender);
	if(CPP!=nullptr){
		return CPP->WPFViewOnTouchDown(sender,e);
	}
}
//---------------------------------------------------------------------------
void WPFUIViewAdapter::OnTouchUp(System::Object ^sender,System::Windows::Input::TouchEventArgs^ e)
{
	auto CPP=CPPFromSender(sender);
	if(CPP!=nullptr){
		return CPP->WPFViewOnTouchUp(sender,e);
	}
}
//---------------------------------------------------------------------------
void WPFUIViewAdapter::OnTouchMove(System::Object ^sender,System::Windows::Input::TouchEventArgs^ e)
{
	auto CPP=CPPFromSender(sender);
	if(CPP!=nullptr){
		return CPP->WPFViewOnTouchMove(sender,e);
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void mbcWPFUIView::mSetup(const cGCHandle &WPFTargetElement)noexcept(true)
{
	auto TargetElement=WPFTargetElement.DynamicCast<IWPFUIViewTargetElement>();
	if(TargetElement!=nullptr){
		fTargetElement.Alloc(TargetElement);
	}
	else{
		auto TargetElementMaker=WPFTargetElement.DynamicCast< System::Func<IWPFUIViewTargetElement^> >();
		if(TargetElementMaker!=nullptr){
			fTargetElement.Alloc(TargetElementMaker());
		}
	}
	TargetElement->Adapter.CPPAttach(this);
}
//---------------------------------------------------------------------------
void mbcWPFUIView::mClear(void)noexcept(true)
{
	auto TargetElement=fTargetElement.Get();
	TargetElement->Adapter.CPPDetach(this);
	fTargetElement.Free();
}
//---------------------------------------------------------------------------
#pragma unmanaged
cGCHandle& mbcWPFUIView::GetTargetElementHandle(void)noexcept(true)
{
	return fTargetElement;
}
#pragma managed
//---------------------------------------------------------------------------
bool mbcWPFUIView::mInsertView(const cGCHandle &WPFTargetElement)noexcept(true)
{
	auto WPFSubviewElement=WPFTargetElement.Cast<System::Windows::UIElement>();
	if(WPFSubviewElement==nullptr)
		return false;

	if(WPFSubviewElement->CheckAccess()==false)
		return false;	// different ui thread

	auto Target=fTargetElement.Get();
	if(Target==nullptr)
		return false;

	Target->Adapter.ChildrenCache=nullptr;
	return Target->ViewAddVisualChild(WPFSubviewElement);
}
//---------------------------------------------------------------------------
void mbcWPFUIView::mRemoveView(const cGCHandle &WPFTargetElement)noexcept(true)
{
	auto WPFSubviewElement=WPFTargetElement.Cast<System::Windows::UIElement>();
	if(WPFSubviewElement==nullptr)
		return;

	auto Target=fTargetElement.Get();
	if(Target==nullptr)
		return;

	Target->Adapter.ChildrenCache=nullptr;
	Target->ViewRemoveVisualChild(WPFSubviewElement);
}
//---------------------------------------------------------------------------
bool mbcWPFUIView::mGetVisible(void)noexcept(true)
{
	auto WPFElement=fTargetElement.DynamicCast<System::Windows::UIElement>();
	if(WPFElement==nullptr)
		return false;
	return WPFElement->Visibility==System::Windows::Visibility::Visible;
}
//---------------------------------------------------------------------------
bool mbcWPFUIView::mSetVisible(bool Visible)noexcept(true)
{
	auto WPFElement=fTargetElement.DynamicCast<System::Windows::UIElement>();
	if(WPFElement==nullptr)
		return false;
	WPFElement->Visibility=Visible?System::Windows::Visibility::Visible:System::Windows::Visibility::Hidden;
	return true;
}
//---------------------------------------------------------------------------
cUIPoint mbcWPFUIView::mGetSize(void)noexcept(true)
{
	auto TargetElement=fTargetElement.Get();
	auto %Adapter=TargetElement->Adapter;

	cUIPoint Size;
	Size.x=static_cast<Float32>(Adapter.WPFRect.Width)*fLayoutScale;
	Size.y=static_cast<Float32>(Adapter.WPFRect.Height)*fLayoutScale;
	return Size;
}
//---------------------------------------------------------------------------
bool mbcWPFUIView::mSetSize(cUIPoint Size)noexcept(true)
{
	auto TargetElement=fTargetElement.Get();
	auto %Adapter=TargetElement->Adapter;
	System::Windows::Rect NewRect;

	NewRect.Location=Adapter.WPFRect.Location;
	if(Size.x<=0){
		NewRect.Width=0;
	}
	else{
		NewRect.Width=Size.x/fLayoutScale;
	}
	if(Size.y<=0){
		NewRect.Height=0;
	}
	else{
		NewRect.Height=Size.y/fLayoutScale;
	}
	return mArrange(TargetElement,NewRect);
}
//---------------------------------------------------------------------------
bool mbcWPFUIView::mArrange(IWPFUIViewTargetElement ^TargetElement,System::Windows::Rect Rect)noexcept(true)
{
	auto WPFElement=dynamic_cast<System::Windows::UIElement^>(TargetElement);
	if(WPFElement==nullptr)
		return false;

	try{
		WPFElement->Arrange(Rect);
		return true;
	}
	catch(System::Exception ^){
		return false;
	}
}
//---------------------------------------------------------------------------
bool mbcWPFUIView::mTranslatePointTo(const cGCHandle &UIElementHandle,cUIPoint &Position)noexcept(true)
{
	auto TargetElement=fTargetElement.Get();

	System::Windows::Point TranslatePoint;
	TranslatePoint.X=Position.x/fLayoutScale;
	TranslatePoint.Y=Position.y/fLayoutScale;
	if(mTranslateWPFPointTo(TargetElement,UIElementHandle,TranslatePoint)==false)
		return false;

	Position.x=static_cast<Float32>(TranslatePoint.X*fLayoutScale);
	Position.y=static_cast<Float32>(TranslatePoint.Y*fLayoutScale);
	return true;
}
//---------------------------------------------------------------------------
bool mbcWPFUIView::mTranslateWPFPointTo(IWPFUIViewTargetElement ^TargetElement,System::Object ^Relative,System::Windows::Point %Point)noexcept(true)
{
	if(TargetElement->ViewParent==Relative){
		auto %Adapter=TargetElement->Adapter;
		Point.X+=Adapter.WPFRect.X;
		Point.Y+=Adapter.WPFRect.Y;
		return true;
	}
	auto WPFVisual=dynamic_cast<System::Windows::Media::Visual^>(TargetElement);
	if(WPFVisual==nullptr)
		return false;

	auto RelativeVisual=dynamic_cast<System::Windows::Media::Visual^>(Relative);
	if(RelativeVisual!=nullptr){
		auto Transform=WPFVisual->TransformToVisual((RelativeVisual));
		Transform->TryTransform(Point,Point);
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------
bool mbcWPFUIView::mTranslateWPFPointFrom(IWPFUIViewTargetElement ^TargetElement,System::Object ^Relative,System::Windows::Point %Point)noexcept(true)
{
	if(TargetElement->ViewParent==Relative){
		auto %Adapter=TargetElement->Adapter;
		Point.X-=Adapter.WPFRect.X;
		Point.Y-=Adapter.WPFRect.Y;
		return true;
	}

	auto WPFVisual=dynamic_cast<System::Windows::Media::Visual^>(TargetElement);
	if(WPFVisual==nullptr)
		return false;
	auto RelativeVisual=dynamic_cast<System::Windows::Media::Visual^>(Relative);
	if(RelativeVisual!=nullptr){
		try{
			auto Transform=RelativeVisual->TransformToVisual(WPFVisual);
			Transform->TryTransform(Point,Point);
			return true;
		}
		catch(System::Exception ^){
			return false;
		}
	}
	return false;
}
//---------------------------------------------------------------------------
bool mbcWPFUIView::mTranslateWPFParentPointFrom(IWPFUIViewTargetElement ^TargetElement,System::Object ^Relative,System::Windows::Point %Point)noexcept(true)
{
	auto Parent=TargetElement->ViewParent;
	if(Parent==Relative){
		return true;
	}

	auto ParentVisual=dynamic_cast<System::Windows::Media::Visual^>(Parent);
	if(ParentVisual==nullptr)
		return false;
	auto RelativeVisual=dynamic_cast<System::Windows::Media::Visual^>(Relative);
	if(RelativeVisual!=nullptr){
		try{
			auto Transform=RelativeVisual->TransformToVisual(ParentVisual);
			Transform->TryTransform(Point,Point);
			return true;
		}
		catch(System::Exception ^){
			return false;
		}
	}
	return false;
}
//---------------------------------------------------------------------------
bool mbcWPFUIView::mMovePosition(const cGCHandle &UIElementHandle,cUIPoint Position)noexcept(true)
{
	auto TargetElement=fTargetElement.Get();
	System::Windows::Rect NewRect;

	System::Windows::Point WPFPosition;
	WPFPosition.X=Position.x/fLayoutScale;
	WPFPosition.Y=Position.y/fLayoutScale;
	if(mTranslateWPFParentPointFrom(TargetElement,UIElementHandle,WPFPosition)==false)
		return false;

	NewRect.Location=WPFPosition;
	NewRect.Size=TargetElement->Adapter.WPFRect.Size;
	return mArrange(TargetElement,NewRect);
}
//---------------------------------------------------------------------------
bool mbcWPFUIView::mArrangeRectangle(const cGCHandle &UIElementHandle,cUIPoint Position,cUIPoint Size)noexcept(true)
{
	auto TargetElement=fTargetElement.Get();
	System::Windows::Rect NewRect;

	System::Windows::Point WPFPosition;
	WPFPosition.X=Position.x/fLayoutScale;
	WPFPosition.Y=Position.y/fLayoutScale;
	if(mTranslateWPFParentPointFrom(TargetElement,UIElementHandle,WPFPosition)==false)
		return false;

	NewRect.Location=WPFPosition;
	if(Size.x<=0){
		NewRect.Width=0;
	}
	else{
		NewRect.Width=Size.x/fLayoutScale;
	}
	if(Size.y<=0){
		NewRect.Height=0;
	}
	else{
		NewRect.Height=Size.y/fLayoutScale;
	}
	return mArrange(TargetElement,NewRect);
}
//---------------------------------------------------------------------------
void mbcWPFUIView::mSetArrangement(void)noexcept(true)
{
	auto WPFElement=fTargetElement.DynamicCast<System::Windows::UIElement>();
	if(WPFElement==nullptr)
		return;
	WPFElement->InvalidateArrange();
}
//---------------------------------------------------------------------------
void mbcWPFUIView::mArrangeLayout(void)noexcept(true)
{
	auto WPFElement=fTargetElement.DynamicCast<System::Windows::UIElement>();
	if(WPFElement==nullptr)
		return;
	WPFElement->UpdateLayout();
}
//---------------------------------------------------------------------------
void mbcWPFUIView::mRenderContent(cGCHandle const**Drawings,uIntn DrawingCount)noexcept(true)
{
	auto TargetElement=fTargetElement.Get();
	auto ContentDrawingGroup=TargetElement->Adapter.ContentDrawingGroup;
	auto Context=ContentDrawingGroup->Open();

	for(uIntn i=0;i<DrawingCount;i++){
		auto ddd=Drawings[i]->DynamicCast<System::Windows::Media::Drawing>();
		auto debug_b=ddd->Bounds;
		Context->DrawDrawing(Drawings[i]->DynamicCast<System::Windows::Media::Drawing>());
	}

	Context->Close();
	delete Context;
}
//---------------------------------------------------------------------------
void mbcWPFUIView::mSetFocus(bool ActivateWindow)noexcept(true)
{
	auto WPFElement=fTargetElement.DynamicCast<System::Windows::UIElement>();
	if(WPFElement==nullptr)
		return;
	WPFElement->Focusable=true;
	WPFElement->Focus();
}
//---------------------------------------------------------------------------
void mbcWPFUIView::WPFSetupChildrenCache(DNet::WPFUIViewAdapter %Adapter)noexcept(true)
{
	void *InfoHandle;
	auto Info=WPFSetupChildrenInfo(InfoHandle);

	auto ChildArray=gcnew array<System::Windows::Media::Visual^>(Info->ChildCount);
	
	for(uIntn i=0;i<Info->ChildCount;i++){
		auto &Handle=Info->Views[i]->GetTargetElementHandle();

		ChildArray->SetValue(Handle.DynamicCast<System::Windows::Media::Visual>(),static_cast<int>(i));
	}

	Adapter.ChildrenCache=ChildArray;

	WPFClearChildrenInfo(InfoHandle);
}
//---------------------------------------------------------------------------
void mbcWPFUIView::WPFViewOnPresentationSourceChanged(System::Windows::SourceChangedEventArgs ^e)noexcept(true)
{
}
//---------------------------------------------------------------------------
void mbcWPFUIView::WPFViewArrangeCore(DNet::WPFUIViewAdapter %Adapter,System::Windows::Rect finalRect)noexcept(true)
{
	bool Moved=(Adapter.WPFRect.X!=finalRect.X) || (Adapter.WPFRect.Y!=finalRect.Y);
	bool Sized=(Adapter.WPFRect.Width!=finalRect.Width) || (Adapter.WPFRect.Y!=finalRect.Height);
	Adapter.WPFRect=finalRect;

	cUIRect Rect;
	Rect.Pos.x=static_cast<Float32>(Adapter.WPFRect.X)*fLayoutScale;
	Rect.Pos.y=static_cast<Float32>(Adapter.WPFRect.Y)*fLayoutScale;
	Rect.Size.x=static_cast<Float32>(Adapter.WPFRect.Width)*fLayoutScale;
	Rect.Size.y=static_cast<Float32>(Adapter.WPFRect.Height)*fLayoutScale;

	WPFNotifyArrange(Moved,Sized,Rect);
}
//---------------------------------------------------------------------------
bool mbcWPFUIView::WPFViewHitTestCore(DNet::WPFUIViewAdapter %Adapter,System::Windows::Media::IntersectionDetail %HitDetail,System::Windows::Media::GeometryHitTestParameters ^ hitTestParameters)noexcept(true)
{
	System::Windows::Media::Geometry ^HitArea=hitTestParameters->HitGeometry;

	auto HitBound=HitArea->Bounds;

	if(HitBound.Right>=0 && HitBound.Left<=Adapter.WPFRect.Width){
		if(HitBound.Bottom>=0 && HitBound.Top<=Adapter.WPFRect.Height){
			if(
				HitBound.Left>0 && HitBound.Right<Adapter.WPFRect.Width &&
				HitBound.Top>0 && HitBound.Bottom<Adapter.WPFRect.Height
				){
				HitDetail=System::Windows::Media::IntersectionDetail::FullyContains;
			}
			else if(
				HitBound.Left<0 && HitBound.Right>Adapter.WPFRect.Width &&
				HitBound.Top<0 && HitBound.Bottom>Adapter.WPFRect.Height
				){
				HitDetail=System::Windows::Media::IntersectionDetail::FullyInside;
			}
			else{
				HitDetail=System::Windows::Media::IntersectionDetail::Intersects;
			}

			return true;
		}
	}

	return false;
}
//---------------------------------------------------------------------------
bool mbcWPFUIView::WPFViewHitTestCore(DNet::WPFUIViewAdapter %Adapter,System::Windows::Point %HitPoint)noexcept(true)
{
	if(HitPoint.X>=0 && HitPoint.X<=Adapter.WPFRect.Width){
		if(HitPoint.Y>=0 && HitPoint.Y<=Adapter.WPFRect.Height){
			return true;
		}
	}

	return false;
}
//---------------------------------------------------------------------------
void mbcWPFUIView::WPFViewOnIsKeyboardFocusedChanged(System::Windows::DependencyPropertyChangedEventArgs %e)noexcept(true)
{
	bool Focused=*static_cast<System::Boolean^>(e.NewValue);

	return WPFOnIsKeyboardFocusedChanged(Focused);
}
//---------------------------------------------------------------------------
void mbcWPFUIView::WPFViewOnIsKeyboardFocusWithinChanged(System::Windows::DependencyPropertyChangedEventArgs %e)noexcept(true)
{
	bool Focused=*static_cast<System::Boolean^>(e.NewValue);

	return WPFOnIsKeyboardFocusWithinChanged(Focused);
}
//---------------------------------------------------------------------------
void mbcWPFUIView::WPFViewOnKeyDown(System::Object ^sender,System::Windows::Input::KeyEventArgs^ e)noexcept(true)
{
	mcWPFKeyEventArgs EventArgs={e};

	bool Repeat=e->IsRepeat;
	auto Key=static_cast<eKeyCode>(System::Windows::Input::KeyInterop::VirtualKeyFromKey(e->Key));

	WPFOnKeyDownFilter(EventArgs,Key,Repeat);

	if(e->OriginalSource==sender){
		WPFOnKeyDown(EventArgs,Key,Repeat);
	}
}
//---------------------------------------------------------------------------
void mbcWPFUIView::WPFViewOnKeyUp(System::Object ^sender,System::Windows::Input::KeyEventArgs^ e)noexcept(true)
{
	mcWPFKeyEventArgs EventArgs={e};

	auto Key=static_cast<eKeyCode>(System::Windows::Input::KeyInterop::VirtualKeyFromKey(e->Key));
	WPFOnKeyUpFilter(EventArgs,Key);

	if(e->OriginalSource==sender){
		WPFOnKeyUp(EventArgs,Key);
	}
}
//---------------------------------------------------------------------------
eMouseButton mbcWPFUIView::ToMouseButton(System::Windows::Input::MouseButton Button)noexcept(true)
{
	switch(Button){
	case System::Windows::Input::MouseButton::Left:
		return MouseButton::Left;
	case System::Windows::Input::MouseButton::Right:
		return MouseButton::Right;
	case System::Windows::Input::MouseButton::Middle:
		return MouseButton::Middle;
	}
	return MouseButton::None;
}
//---------------------------------------------------------------------------
void mbcWPFUIView::WPFViewOnIsMouseDirectlyOverChanged(System::Windows::DependencyPropertyChangedEventArgs %e)noexcept(true)
{
	bool InRange=*static_cast<System::Boolean^>(e.NewValue);

	WPFOnIsMouseDirectlyOverChanged(InRange);
}
//---------------------------------------------------------------------------
void mbcWPFUIView::WPFViewOnMouseEnter(System::Windows::Input::MouseEventArgs^ e)noexcept(true)
{
	mcWPFMouseEventArgs MouseEventArgs={e};

	WPFOnMouseEnter(MouseEventArgs);
}
//---------------------------------------------------------------------------
void mbcWPFUIView::WPFViewOnMouseLeave(System::Windows::Input::MouseEventArgs^ e)noexcept(true)
{
	mcWPFMouseEventArgs MouseEventArgs={e};

	WPFOnMouseLeave(MouseEventArgs);
}
//---------------------------------------------------------------------------
void mbcWPFUIView::WPFViewOnMouseMove(System::Object ^sender,System::Windows::Input::MouseEventArgs^ e)noexcept(true)
{
	if(e->StylusDevice!=nullptr){
		// input from touch
		return;
	}


	mcWPFMouseEventArgs MouseEventArgs={e};
	WPFOnMouseMoveFilter(MouseEventArgs);
	if(e->OriginalSource==sender){
		WPFOnMouseMove(MouseEventArgs);
	}
}
//---------------------------------------------------------------------------
void mbcWPFUIView::WPFViewOnMouseDown(System::Object ^sender,System::Windows::Input::MouseButtonEventArgs^ e)noexcept(true)
{
	if(e->StylusDevice!=nullptr){
		// input from touch
		return;
	}
	if(e->OriginalSource==sender){
		auto Element=dynamic_cast<System::Windows::UIElement^>(sender);
		Element->CaptureMouse();
	}
	System::Windows::Input::MouseEventArgs ^eMouse=e;
	mcWPFMouseEventArgs MouseEventArgs={eMouse};
	auto MouseButton=ToMouseButton(e->ChangedButton);
	WPFOnMouseDownFilter(MouseEventArgs,MouseButton);
	if(e->OriginalSource==sender){
		WPFOnMouseDown(MouseEventArgs,MouseButton);
	}
}
//---------------------------------------------------------------------------
void mbcWPFUIView::WPFViewOnMouseUp(System::Object ^sender,System::Windows::Input::MouseButtonEventArgs^ e)noexcept(true)
{
	if(e->StylusDevice!=nullptr){
		// input from touch
		return;
	}
	if(e->OriginalSource==sender){
		auto Element=dynamic_cast<System::Windows::UIElement^>(sender);
		Element->ReleaseMouseCapture();
	}

	System::Windows::Input::MouseEventArgs ^eMouse=e;
	mcWPFMouseEventArgs MouseEventArgs={eMouse};
	auto MouseButton=ToMouseButton(e->ChangedButton);
	WPFOnMouseUpFilter(MouseEventArgs,MouseButton);
	if(e->OriginalSource==sender){
		WPFOnMouseUp(MouseEventArgs,ToMouseButton(e->ChangedButton));
	}
}
//---------------------------------------------------------------------------
void mbcWPFUIView::WPFViewOnMouseWheel(System::Object ^sender,System::Windows::Input::MouseWheelEventArgs^ e)noexcept(true)
{
	if(e->StylusDevice!=nullptr){
		// input from touch
		return;
	}

	Float32 ScrollY;
	ScrollY=-static_cast<Float32>(e->Delta);
	System::Windows::Input::MouseEventArgs ^eMouse=e;
	mcWPFMouseEventArgs MouseEventArgs={eMouse};
	WPFOnMouseWheelFilter(MouseEventArgs,0,ScrollY);
	if(e->OriginalSource==sender){
		WPFOnMouseWheel(MouseEventArgs,0,ScrollY);
	}
}
//---------------------------------------------------------------------------
void mbcWPFUIView::WPFViewOnTouchDown(System::Object ^sender,System::Windows::Input::TouchEventArgs^ e)noexcept(true)
{
	mcWPFTouchEventArgs TouchEventArgs={e};
	WPFOnTouchDownFilter(TouchEventArgs);
	if(e->OriginalSource==sender){
		WPFOnTouchDown(TouchEventArgs);
	}
}
//---------------------------------------------------------------------------
void mbcWPFUIView::WPFViewOnTouchUp(System::Object ^sender,System::Windows::Input::TouchEventArgs^ e)noexcept(true)
{
	mcWPFTouchEventArgs TouchEventArgs={e};
	WPFOnTouchUpFilter(TouchEventArgs);
	if(e->OriginalSource==sender){
		WPFOnTouchUp(TouchEventArgs);
	}
}
//---------------------------------------------------------------------------
void mbcWPFUIView::WPFViewOnTouchMove(System::Object ^sender,System::Windows::Input::TouchEventArgs^ e)noexcept(true)
{
	mcWPFTouchEventArgs TouchEventArgs={e};
	WPFOnTouchMoveFilter(TouchEventArgs);
	if(e->OriginalSource==sender){
		WPFOnTouchMove(TouchEventArgs);
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
WPFUIViewAdapter% WPFView::Adapter::get(void)
{
	return fViewAdapter;
}
//---------------------------------------------------------------------------
System::Windows::DependencyObject^ WPFView::ViewParent::get(void)
{
	return VisualParent;
}
//---------------------------------------------------------------------------
bool WPFView::ViewAddVisualChild(System::Windows::UIElement ^Child)
{
	try{
		AddVisualChild(Child);
		return true;
	}
	catch(System::Exception ^){
		return false;
	}
}
//---------------------------------------------------------------------------
bool WPFView::ViewRemoveVisualChild(System::Windows::UIElement ^Child)
{
	try{
		RemoveVisualChild(Child);
		return true;
	}
	catch(System::Exception ^){
		return false;
	}
}
//---------------------------------------------------------------------------
void WPFView::OnVisualParentChanged(System::Windows::DependencyObject ^oldParent)
{
	__super::OnVisualParentChanged(oldParent);
	fViewAdapter.OnVisualParentChanged(VisualParent);
}
//---------------------------------------------------------------------------
int WPFView::VisualChildrenCount::get(void)
{
	return fViewAdapter.VisualChildrenCount;
}
//---------------------------------------------------------------------------
System::Windows::Media::Visual^ WPFView::GetVisualChild(int index)
{
	return fViewAdapter.GetVisualChild(index);
}
//---------------------------------------------------------------------------
System::Windows::Size WPFView::MeasureCore(System::Windows::Size availableSize)
{
	return fViewAdapter.MeasureCore(availableSize);
}
//---------------------------------------------------------------------------
void WPFView::ArrangeCore(System::Windows::Rect finalRect)
{
	__super::ArrangeCore(finalRect);

	fViewAdapter.ArrangeCore(finalRect);
}
//---------------------------------------------------------------------------
void WPFView::OnRender(System::Windows::Media::DrawingContext^ drawingContext)
{
	__super::OnRender(drawingContext);

	fViewAdapter.OnRender(drawingContext);
}
//---------------------------------------------------------------------------
System::Windows::Media::GeometryHitTestResult^ WPFView::HitTestCore(System::Windows::Media::GeometryHitTestParameters ^ hitTestParameters)
{
	return fViewAdapter.HitTestCore(hitTestParameters);
}
//---------------------------------------------------------------------------
System::Windows::Media::HitTestResult^ WPFView::HitTestCore(System::Windows::Media::PointHitTestParameters ^ hitTestParameters)
{
	return fViewAdapter.HitTestCore(hitTestParameters);
}
//---------------------------------------------------------------------------

