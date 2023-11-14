#include "WinDNetM_WPFUIView.h"

using namespace cnLibrary;

using namespace cnRTL;
using namespace cnWin;
using namespace DNet;


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
iPtr<iUIView> cnWin::DNetCreateUIView(void)
{
	auto DispatchFrame=mcDNetUIThreadDispatcher::mCurrentUIDispatcher();
	if(DispatchFrame==nullptr)
		return nullptr;

	mcWPFView::mcConstructParameter Parameter={
		gcnew WPFView()
	};
	return DNetCreateUIView(DispatchFrame,Parameter);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
mcWPFKeyEventArgs::mcWPFKeyEventArgs(System::Windows::Input::KeyEventArgs^ e)
	: Handle(e)
{
}
//---------------------------------------------------------------------------
mcWPFKeyEventArgs::mcWPFKeyEventArgs(mcWPFKeyEventArgs &&Src)=default;
//---------------------------------------------------------------------------
mcWPFKeyEventArgs::~mcWPFKeyEventArgs()
{
}
//---------------------------------------------------------------------------
void mcWPFKeyEventArgs::mCancelEvent(void)
{
	auto Args=Handle.Get();
	Args->Handled=true;
}
//---------------------------------------------------------------------------
bool mcWPFKeyEventArgs::mIsCancelled(void)
{
	auto Args=Handle.Get();
	return Args->Handled;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
mcWPFMouseEventArgs::mcWPFMouseEventArgs(System::Windows::Input::MouseEventArgs^ e)
	: Handle(e)
{
}
//---------------------------------------------------------------------------
mcWPFMouseEventArgs::mcWPFMouseEventArgs(mcWPFMouseEventArgs &&Src)=default;
//---------------------------------------------------------------------------
mcWPFMouseEventArgs::~mcWPFMouseEventArgs()
{
}
//---------------------------------------------------------------------------
void mcWPFMouseEventArgs::mCancelEvent(void)
{
	auto Args=Handle.Get();
	Args->Handled=true;
}
//---------------------------------------------------------------------------
bool mcWPFMouseEventArgs::mIsCancelled(void)
{
	auto Args=Handle.Get();
	return Args->Handled;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
mcWPFTouchEventArgs::mcWPFTouchEventArgs(System::Windows::Input::TouchEventArgs^ e)
	: Handle(e)
{
}
//---------------------------------------------------------------------------
mcWPFTouchEventArgs::mcWPFTouchEventArgs(mcWPFTouchEventArgs &&Src)=default;
//---------------------------------------------------------------------------
mcWPFTouchEventArgs::~mcWPFTouchEventArgs()
{
}
//---------------------------------------------------------------------------
void* mcWPFTouchEventArgs::mGetTouchID(void)
{
	auto Args=Handle.Get();
	return reinterpret_cast<void*>(static_cast<uIntn>(Args->TouchDevice->Id));
}
//---------------------------------------------------------------------------
void mcWPFTouchEventArgs::mCancelEvent(void)
{
	auto Args=Handle.Get();
	Args->Handled=true;
}
//---------------------------------------------------------------------------
bool mcWPFTouchEventArgs::mIsCancelled(void)
{
	auto Args=Handle.Get();
	return Args->Handled;
}
//---------------------------------------------------------------------------
bool mcWPFTouchEventArgs::mGetPosition(cGCRef &UIElementHandle,cUIPoint &Position)
{
	System::Object^ RelativeObject=UIElementHandle;
	auto ViewVisual=dynamic_cast<IWPFUIViewVisual^>(RelativeObject);
	if(ViewVisual!=nullptr){
		return mGetPosition(ViewVisual,Position);
	}
	auto Visual=dynamic_cast<System::Windows::Media::Visual^>(RelativeObject);
	if(Visual!=nullptr){
		return mGetPosition(Visual,Position);
	}
	return false;
}
//---------------------------------------------------------------------------
bool mcWPFTouchEventArgs::mGetPosition(System::Windows::Media::Visual ^Visual,cUIPoint &Position)
{
	auto RelativeElement=dynamic_cast<System::Windows::IInputElement^>(Visual);
	if(RelativeElement==nullptr)
		return false;

	auto TouchEventArgs=Handle.Get();
	auto TouchPos=TouchEventArgs->TouchDevice->GetTouchPoint(RelativeElement);
	Position.x=static_cast<Float32>(TouchPos->Position.X);
	Position.y=static_cast<Float32>(TouchPos->Position.Y);
	return true;
}
//---------------------------------------------------------------------------
bool mcWPFTouchEventArgs::mGetPosition(IWPFUIViewVisual ^ViewVisual,cUIPoint &Position)
{
	auto RelativeElement=dynamic_cast<System::Windows::IInputElement^>(ViewVisual->Visual);
	if(RelativeElement==nullptr)
		return false;

	float LayoutScale=ViewVisual->LayoutScale;
	auto TouchEventArgs=Handle.Get();
	auto TouchPos=TouchEventArgs->TouchDevice->GetTouchPoint(RelativeElement);
	Position.x=static_cast<Float32>(TouchPos->Position.X*LayoutScale);
	Position.y=static_cast<Float32>(TouchPos->Position.Y*LayoutScale);
	return true;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
static rcWPFVisualRootAsWindow::rcWPFVisualRootAsWindow()
{
	VisualRootAsWindowProperty=System::Windows::DependencyProperty::Register("cnLibraryVisualRootAsWindow",
		rcWPFVisualRootAsWindow::typeid,System::Windows::Media::Visual::typeid);
}
//---------------------------------------------------------------------------
rcWPFVisualRootAsWindow^ rcWPFVisualRootAsWindow::Query(System::Windows::Media::Visual ^TargetVisual)
{
	auto Object=TargetVisual->GetValue(VisualRootAsWindowProperty);
	if(Object!=nullptr){
		return static_cast<rcWPFVisualRootAsWindow^>(Object);
	}

	return gcnew rcWPFVisualRootAsWindow(TargetVisual);
}
//---------------------------------------------------------------------------
rcWPFVisualRootAsWindow::rcWPFVisualRootAsWindow(System::Windows::Media::Visual ^TargetVisual)
	: fTargetVisual(TargetVisual)
	, CPP(nullptr)
{
}
//---------------------------------------------------------------------------
iUIWindow* rcWPFVisualRootAsWindow::UIWindowInterface::get(void)
{
	return rcWPFVisualRootAsWindow_GetUIWindow(CPP);
}
//---------------------------------------------------------------------------
void rcWPFVisualRootAsWindow::RemoteChildAttach(System::Object^)
{
	if(fChildCount==0){
		CPPSetup();
	}
	fChildCount++;
}
//---------------------------------------------------------------------------
void rcWPFVisualRootAsWindow::RemoteChildDetach(System::Object^)
{
	fChildCount--;

	if(fChildCount==0){
		CPPClear();
	}
}
//---------------------------------------------------------------------------
void rcWPFVisualRootAsWindow::CPPSetup(void)
{
	CPP=rcWPFVisualRootAsWindow_MakeWindowInterface(mcWPFVisual(fTargetVisual));

	fTargetVisual->SetValue(VisualRootAsWindowProperty,this);
}
//---------------------------------------------------------------------------
void rcWPFVisualRootAsWindow::CPPClear(void)
{
	fTargetVisual->SetValue(VisualRootAsWindowProperty,nullptr);

	rcWPFVisualRootAsWindow_DropWindowInterface(CPP);
	CPP=nullptr;
}
//---------------------------------------------------------------------------
mcWPFVisual::mcWPFVisual(System::Windows::Media::Visual^ v)
	: Handle(v)
{
}
//---------------------------------------------------------------------------
mcWPFVisual::mcWPFVisual(mcWPFVisual &&Src)=default;
//---------------------------------------------------------------------------
mcWPFVisual::~mcWPFVisual()
{
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
static rcUIElementAsWPFViewParent::rcUIElementAsWPFViewParent()
{
	UIElementAsWPFViewParentProperty=System::Windows::DependencyProperty::Register("cnLibraryUIElementAsWPFViewParent",
		rcUIElementAsWPFViewParent::typeid,System::Windows::UIElement::typeid);
}
//---------------------------------------------------------------------------
rcUIElementAsWPFViewParent^ rcUIElementAsWPFViewParent::Query(System::Windows::UIElement ^TargetElement)
{
	auto Object=TargetElement->GetValue(UIElementAsWPFViewParentProperty);
	if(Object!=nullptr){
		return static_cast<rcUIElementAsWPFViewParent^>(Object);
	}

	return gcnew rcUIElementAsWPFViewParent(TargetElement);
}
//---------------------------------------------------------------------------
rcUIElementAsWPFViewParent::rcUIElementAsWPFViewParent(System::Windows::UIElement ^TargetElement)
	: fTargetElement(TargetElement)
	, fViewRoot(nullptr)
	, CPP(nullptr)
{
}
//---------------------------------------------------------------------------
void rcUIElementAsWPFViewParent::CPPSetup(IWPFView ^InitalChild)
{
	CPP=new mcUIElementAsWPFViewParent(this,InitalChild);

	fTargetElement->IsVisibleChanged+=gcnew System::Windows::DependencyPropertyChangedEventHandler(this,&rcUIElementAsWPFViewParent::TargetOnIsVisibleChanged);
	System::Windows::PresentationSource::AddSourceChangedHandler(fTargetElement,
		gcnew System::Windows::SourceChangedEventHandler(this,&rcUIElementAsWPFViewParent::TargetOnSourceChanged));

	fTargetElement->SetValue(UIElementAsWPFViewParentProperty,this);

	CPP->WPFUpdateVisible(fTargetElement->IsVisible);
}
//---------------------------------------------------------------------------
void rcUIElementAsWPFViewParent::CPPClear(void)
{
	if(fViewRoot!=nullptr){
		fViewRoot->RemoteChildDetach(this);
		fViewRoot=nullptr;
	}

	fTargetElement->SetValue(UIElementAsWPFViewParentProperty,nullptr);

	fTargetElement->IsVisibleChanged-=gcnew System::Windows::DependencyPropertyChangedEventHandler(this,&rcUIElementAsWPFViewParent::TargetOnIsVisibleChanged);
	System::Windows::PresentationSource::RemoveSourceChangedHandler(fTargetElement,
		gcnew System::Windows::SourceChangedEventHandler(this,&rcUIElementAsWPFViewParent::TargetOnSourceChanged));
	
	delete CPP;
	CPP=nullptr;
}
//---------------------------------------------------------------------------
void rcUIElementAsWPFViewParent::ViewChildAttach(IWPFView ^Child)
{
	if(CPP==nullptr){
		CPPSetup(Child);
	}
	else{
		CPP->WPFChildAttach(Child->ChildInterface);
	}
}
//---------------------------------------------------------------------------
void rcUIElementAsWPFViewParent::ViewChildDetach(IWPFView ^Child)
{
	if(CPP!=nullptr){
		if(CPP->WPFChildDetach(Child->ChildInterface)){
			if(CPP->GetChildCount()==0){
				CPPClear();
			}
		}	
	}
}
//---------------------------------------------------------------------------
void rcUIElementAsWPFViewParent::TargetOnIsVisibleChanged(System::Object ^sender,System::Windows::DependencyPropertyChangedEventArgs e)
{
	if(CPP==nullptr)
		return;

	if(sender!=fTargetElement)
		return;

	auto NewVisibleValue=static_cast<bool>(e.NewValue);
	CPP->WPFUpdateVisible(NewVisibleValue);
}
//---------------------------------------------------------------------------
void rcUIElementAsWPFViewParent::TargetOnSourceChanged(System::Object ^sender,System::Windows::SourceChangedEventArgs ^e)
{sender;e;
	NotifyWindowChange();
}
//---------------------------------------------------------------------------
void rcUIElementAsWPFViewParent::NotifyWindowChange(void)
{
	if(fViewRoot!=nullptr){
		fViewRoot->RemoteChildDetach(this);
		fViewRoot=nullptr;
	}

	if(CPP==nullptr)
		return;

	auto NewSource=System::Windows::PresentationSource::FromVisual(fTargetElement);
	if(NewSource==nullptr){
		// no window
		CPP->WPFNotifyWindowChange(nullptr);
		return;
	}
	auto RootVisual=NewSource->RootVisual;

	auto VRoot=dynamic_cast<IWPFUIWindowElement^>(RootVisual);
	if(VRoot==nullptr){
		VRoot=rcWPFVisualRootAsWindow::Query(RootVisual);
	}
	if(VRoot!=nullptr){
		// cnLibrary window
		fViewRoot=VRoot;
		fViewRoot->RemoteChildAttach(this);

		CPP->WPFNotifyWindowChange(VRoot->UIWindowInterface);
	}
	else{
		// unknow window
		CPP->WPFNotifyWindowChange(nullptr);
	}
}
//---------------------------------------------------------------------------
cnWin::iWPFViewParent* rcUIElementAsWPFViewParent::ParentInterface::get(void)
{
	return CPP;
}
//---------------------------------------------------------------------------
void rcUIElementAsWPFViewParent::ParentRemoveViewChild(IWPFView^)
{
	throw gcnew System::NotImplementedException();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
mcUIElementAsWPFViewParent::mcUIElementAsWPFViewParent(rcUIElementAsWPFViewParent ^Parent,IWPFView ^Child)
	: fWPF(Parent)
	, fViewState(UIState::Null)
{
	fChildSet.Insert(Child->ChildInterface);
}
//---------------------------------------------------------------------------
mcUIElementAsWPFViewParent::~mcUIElementAsWPFViewParent()
{
}
//---------------------------------------------------------------------------
rcUIElementAsWPFViewParent^ mcUIElementAsWPFViewParent::GetWPF(void)const
{
	return fWPF;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
mcWPFView::mcWPFView(mcConstructParameter &Parameter)
	: fWPFContent(gcnew System::Windows::Media::DrawingGroup())
	, fWPFView(cnVar::MoveCast(Parameter.WPFView))
	, fRectWPF{0,0,0,0}
{
	fZPosition=0;

	fDispatcherFinishNotify.NotifyProcedure=DispatcherFinishNotify;

	auto WPF=fWPFView.Get();
	WPF->CPPAttach(this);
}
//---------------------------------------------------------------------------
mcWPFView::~mcWPFView()
{
	auto WPF=fWPFView.Get();
	WPF->CPPDetach(this);
}
//---------------------------------------------------------------------------
void mcWPFView::CleanupWPF(void)
{
	auto WPFView=fWPFView.Get();
	for(auto &SubviewPair : fSubviewMap){
		IWPFView^ Subview=SubviewPair.Value.WPFHandle;
		SubviewPair.Value.WPFHandle.Free();
		WPFView->RemoveChildElement(Subview->Element);

		Subview->ParentRelease(WPFView);
	}

	fSubviewZOrderSet.Clear();
	fSubviewMap.Clear();

	// remove from parent
	auto WPFViewParent=fWPFViewParent.Get();
	if(WPFViewParent!=nullptr){
		WPFViewParent->RemoveViewChild(WPFView);
	}

	WPFView->DispatcherFinishCleanup();
}
//---------------------------------------------------------------------------
void mcWPFView::DispatcherFinishNotify(mcDNetUIThreadDispatcher::cDispatcherFinishNotify *Notify,bool Shutdown)
{
	auto Host=cnMemory::GetObjectFromMemberPointer(Notify,&mcWPFView::fDispatcherFinishNotify);
	return Host->DispatcherFinishNotify(Shutdown);
}
//---------------------------------------------------------------------------
void mcWPFView::DispatcherFinishNotify(bool Shutdown)
{
	CleanupWPF();
	nDispatchFinishNotify(Shutdown);
}
//---------------------------------------------------------------------------
bool mcWPFView::mSetSize(cUIPoint Size)
{
	System::Windows::Rect NewRect;
	NewRect.X=fRectWPF.Pos.x;
	NewRect.Y=fRectWPF.Pos.y;
	NewRect.Width=Size.x;
	NewRect.Height=Size.y;
	return mArrange(NewRect);
}
//---------------------------------------------------------------------------
bool mcWPFView::mTranslatePointTo(cGCRef &UIElementHandle,cUIPoint &Position)
{
	System::Windows::Point TranslatePoint;
	TranslatePoint.X=Position.x/fLayoutScale;
	TranslatePoint.Y=Position.y/fLayoutScale;
	if(mTranslateWPFPointTo(UIElementHandle,TranslatePoint)==false)
		return false;

	Position.x=static_cast<Float32>(TranslatePoint.X*fLayoutScale);
	Position.y=static_cast<Float32>(TranslatePoint.Y*fLayoutScale);
	return true;
}
//---------------------------------------------------------------------------
bool mcWPFView::mTranslateWPFPointTo(System::Object ^Relative,System::Windows::Point %Point)
{
	auto WPFView=fWPFView.Get();
	auto WPFParent=WPFView->GetParent();
	if(WPFParent==Relative){
		Point.X+=fRectWPF.Pos.x;
		Point.Y+=fRectWPF.Pos.y;
		return true;
	}
	auto WPFVisual=dynamic_cast<System::Windows::Media::Visual^>(WPFView);
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
bool mcWPFView::mTranslateWPFPointFrom(System::Object ^Relative,System::Windows::Point %Point)
{
	auto WPFView=fWPFView.Get();
	auto WPFParent=WPFView->GetParent();
	if(WPFParent==Relative){
		//Point.X-=fRectWPF.Pos.x;
		//Point.Y-=fRectWPF.Pos.y;
		return true;
	}

	auto WPFVisual=dynamic_cast<System::Windows::Media::Visual^>(WPFView);
	if(WPFVisual==nullptr)
		return false;
	auto RelativeVisual=dynamic_cast<System::Windows::Media::Visual^>(Relative);
	if(RelativeVisual!=nullptr){
		auto Transform=RelativeVisual->TransformToVisual(WPFVisual);
		Transform->TryTransform(Point,Point);
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------
bool mcWPFView::mSetPosition(cGCRef &UIElementHandle,cUIPoint Position)
{
	Position.x=System::Math::Round(Position.x,System::MidpointRounding::AwayFromZero);
	Position.y=System::Math::Round(Position.y,System::MidpointRounding::AwayFromZero);
	System::Windows::Rect NewRect;

	System::Windows::Point WPFPosition;
	WPFPosition.X=Position.x/fLayoutScale;
	WPFPosition.Y=Position.y/fLayoutScale;
	if(mTranslateWPFPointFrom(UIElementHandle,WPFPosition)==false)
		return false;

	NewRect.Location=WPFPosition;
	NewRect.Width=fRectWPF.Size.x;
	NewRect.Height=fRectWPF.Size.y/fLayoutScale;
	return mArrange(NewRect);
}
//---------------------------------------------------------------------------
bool mcWPFView::mSetRectangle(cGCRef &UIElementHandle,cUIPoint Position,cUIPoint Size)
{
	System::Windows::Rect NewRect;

	Position.x=System::Math::Round(Position.x,System::MidpointRounding::AwayFromZero);
	Position.y=System::Math::Round(Position.y,System::MidpointRounding::AwayFromZero);
	Size.x=System::Math::Round(Size.x,System::MidpointRounding::AwayFromZero);
	Size.y=System::Math::Round(Size.y,System::MidpointRounding::AwayFromZero);

	System::Windows::Point WPFPosition;
	WPFPosition.X=Position.x/fLayoutScale;
	WPFPosition.Y=Position.y/fLayoutScale;
	if(mTranslateWPFPointFrom(UIElementHandle,WPFPosition)==false)
		return false;

	NewRect.Location=WPFPosition;
	if(Size.x<0){
		NewRect.Width=0;
	}
	else{
		NewRect.Width=Size.x/fLayoutScale;
	}
	if(Size.y<0){
		NewRect.Height=0;
	}
	else{
		NewRect.Height=Size.y/fLayoutScale;
	}
	return mArrange(NewRect);
}
//---------------------------------------------------------------------------
bool mcWPFView::mArrange(System::Windows::Rect Rect)
{
	auto WPFView=fWPFView.Get();
	auto WPFElement=WPFView->Element;
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
bool mcWPFView::mGetVisible(void)
{
	auto WPFView=fWPFView.Get();
	auto WPFElement=WPFView->Element;
	if(WPFElement==nullptr)
		return false;
	return WPFElement->Visibility==System::Windows::Visibility::Visible;
}
//---------------------------------------------------------------------------
bool mcWPFView::mSetVisible(bool Visible)
{
	auto WPFView=fWPFView.Get();
	auto WPFElement=WPFView->Element;
	if(WPFElement==nullptr)
		return false;
	WPFElement->Visibility=Visible?System::Windows::Visibility::Visible:System::Windows::Visibility::Hidden;
	return true;
}
//---------------------------------------------------------------------------
void mcWPFView::mSetArrangement(void)
{
	auto WPFView=fWPFView.Get();
	auto WPFElement=WPFView->Element;
	if(WPFElement==nullptr)
		return;
	WPFElement->InvalidateArrange();
}
//---------------------------------------------------------------------------
void mcWPFView::mArrangeLayout(void)
{
	auto WPFView=fWPFView.Get();
	auto WPFElement=WPFView->Element;
	if(WPFElement==nullptr)
		return;
	WPFElement->UpdateLayout();
}
//---------------------------------------------------------------------------
void mcWPFView::mRenderContent(cGCHandle const**Drawings,uIntn DrawingCount)
{
	//auto WPFView=fManaged->WPFView.Get();
	//WPFView->InvalidateVisual();
	auto WPFContent=fWPFContent.Get();
	auto Context=WPFContent->Open();

	for(uIntn i=0;i<DrawingCount;i++){
		Context->DrawDrawing(Drawings[i]->DynamicCast<System::Windows::Media::Drawing>());
	}
	
	Context->Close();
	delete Context;
}
//---------------------------------------------------------------------------
bool mcWPFView::mInsertView(iWPFViewChild *Subview,const cGCRef &WPFViewHandle,Float32 ZPosition)
{
	auto WPFSubView=WPFViewHandle.Cast<IWPFView>();
	if(WPFSubView==nullptr)
		return false;
	return mInsertView(Subview,WPFSubView,ZPosition);
}
//---------------------------------------------------------------------------
bool mcWPFView::mRemoveView(iWPFViewChild *Subview,const cGCRef &WPFViewHandle)
{
	auto WPFSubView=WPFViewHandle.Cast<IWPFView>();
	if(WPFSubView==nullptr)
		return false;
	return mRemoveView(Subview,WPFSubView);
}
//---------------------------------------------------------------------------
bool mcWPFView::mInsertView(iWPFViewChild *Subview,IWPFView ^WPFSubview,Float32 ZPosition)
{
	auto pSetItem=fSubviewMap.GetPair(Subview);
	if(pSetItem!=fSubviewMap.end()){
		// already exists
		return false;
	}

	auto SubviewElement=WPFSubview->Element;
	if(SubviewElement==nullptr)
		return false;
	if(SubviewElement->CheckAccess()==false)
		return false;	// different ui thread

	auto WPFView=fWPFView.Get();
	if(WPFSubview->ParentAcquire(WPFView)==false){
		return false;	// acquire failed
	}

	// add to set
	pSetItem=fSubviewMap.InsertPair(Subview);
	pSetItem->Value.ZPosition=ZPosition;
	pSetItem->Value.WPFHandle.Alloc(WPFSubview);
	
	// add to zorder set
	fSubviewZOrderSet.Insert(pSetItem);
	
	if(WPFView->InsertChildElement(SubviewElement)){
		return true;
	}

	// failed
	fSubviewZOrderSet.Remove(pSetItem);
	fSubviewMap.RemovePair(pSetItem);
	return false;
}
//---------------------------------------------------------------------------
bool mcWPFView::mRemoveView(iWPFViewChild *Subview,IWPFView ^WPFSubview)
{
	auto SubviewElement=WPFSubview->Element;
	if(SubviewElement==nullptr)
		return false;

	auto pSetItem=fSubviewMap.GetPair(Subview);
	if(pSetItem==fSubviewMap.end()){
		// not in the view
		return false;
	}
	pSetItem->Value.WPFHandle.Free();
	// remove from order list
	fSubviewZOrderSet.Remove(pSetItem);
	// remove from set
	fSubviewMap.RemovePair(pSetItem);

	auto WPFView=fWPFView.Get();
	WPFView->RemoveChildElement(SubviewElement);

	WPFSubview->ParentRelease(WPFView);
	
	return true;
}
//---------------------------------------------------------------------------
void mcWPFView::WPFParentRemoveChild(IWPFView ^Subview)
{
	mRemoveView(Subview->ChildInterface,Subview);
}
//---------------------------------------------------------------------------
void mcWPFView::mSetFocus(bool ActivateWindow)
{
	auto WPFView=fWPFView.Get();
	auto WPFElement=WPFView->Element;
	if(WPFElement==nullptr)
		return;
	WPFElement->Focusable=true;
	WPFElement->Focus();
}
//---------------------------------------------------------------------------
bool mcWPFView::WPFViewParentAcquire(IWPFViewParent ^Parent)
{
	if(fParent!=nullptr)
		return false;

	fWPFViewParent.Set(Parent);
	fParent=Parent->Interface;
	return true;
}
//---------------------------------------------------------------------------
void mcWPFView::WPFViewParentRelease(IWPFViewParent ^Parent)
{
	if(fParent!=Parent->Interface)
		return;

	fParent=nullptr;
	fWPFViewParent.Set(nullptr);
}
//---------------------------------------------------------------------------
void mcWPFView::WPFViewIsVisibleChanged(System::Windows::DependencyPropertyChangedEventArgs %e)
{
	WPFNotifyVisible(static_cast<System::Boolean>(e.NewValue));
}
//---------------------------------------------------------------------------
void mcWPFView::WPFViewOnVisualParentChanged(System::Windows::DependencyObject ^VisualParent)
{
	auto WPF=fWPFView.Get();
	auto ElementAsParent=fWPFElementParent.Get();
	if(ElementAsParent!=nullptr){
		fWPFElementParent.Set(nullptr);
		ElementAsParent->ViewChildDetach(WPF);
		fParent=nullptr;
	}


	mcWPFView *NewViewParentView=nullptr;
	if(VisualParent!=nullptr){
		auto ParentView=dynamic_cast<WPFView^>(VisualParent);
		if(ParentView!=nullptr){
			// direct parent
			NewViewParentView=ParentView->CPP;
		}
		else if(fParent==nullptr){
			// no direct parent, try use dummy parent
			auto ParentElement=dynamic_cast<System::Windows::UIElement^>(VisualParent);
			if(ParentElement!=nullptr){
				ElementAsParent=rcUIElementAsWPFViewParent::Query(ParentElement);
				ElementAsParent->ViewChildAttach(WPF);
				fWPFElementParent.Set(ElementAsParent);
				fParent=ElementAsParent->ParentInterface;
			}
		}
	}

	fParentView=NewViewParentView;

	WPFChildTreeNotifyState();
	WPFChildTreeNotifyScale();
}
//---------------------------------------------------------------------------
System::Windows::Media::Visual^ mcWPFView::WPFViewGetVisualChild(int index)
{
	auto &Item=fSubviewZOrderSet[index];
	IWPFView^ WPFSubView=Item->Value.WPFHandle;
	return WPFSubView->Element;
}
//---------------------------------------------------------------------------
int mcWPFView::WPFViewGetVisualChildrenCount(void)
{
	return static_cast<int>(fSubviewZOrderSet.GetCount());
}
//---------------------------------------------------------------------------
void mcWPFView::WPFViewArrangeCore(System::Windows::Rect finalRect)
{
	bool Moved=(fRectWPF.Pos.x!=static_cast<Float32>(finalRect.X)) || fRectWPF.Pos.y!=static_cast<Float32>(finalRect.Y);
	bool Sized=(fRectWPF.Size.x!=static_cast<Float32>(finalRect.Width)) || fRectWPF.Size.y!=static_cast<Float32>(finalRect.Height);
	fRectWPF.Pos.x=static_cast<Float32>(finalRect.X);
	fRectWPF.Pos.y=static_cast<Float32>(finalRect.Y);
	fRectWPF.Size.x=static_cast<Float32>(finalRect.Width);
	fRectWPF.Size.y=static_cast<Float32>(finalRect.Height);

	WPFUIViewUpdateContentSize(fLayoutScale,fRectWPF.Size.x,fRectWPF.Size.y);

	WPFChildTreeNotifyState();

	WPFUIViewNotifyArrange(Moved,Sized);
}
//---------------------------------------------------------------------------
System::Windows::Size mcWPFView::WPFViewMeasureCore(System::Windows::Size availableSize)
{
	return System::Windows::Size(fRectWPF.Size.x,fRectWPF.Size.y);
}
//---------------------------------------------------------------------------
bool mcWPFView::WPFViewHitTestCore(System::Windows::Media::IntersectionDetail %HitDetail,System::Windows::Media::GeometryHitTestParameters ^ hitTestParameters)
{
	System::Windows::Media::Geometry ^HitArea=hitTestParameters->HitGeometry;

	auto HitBound=HitArea->Bounds;
	
	if(HitBound.Right>=0 && HitBound.Left<=fRectWPF.Size.x){
		if(HitBound.Bottom>=0 && HitBound.Top<=fRectWPF.Size.y){
			if(
				HitBound.Left>0 && HitBound.Right<fRectWPF.Size.x &&
				HitBound.Top>0 && HitBound.Bottom<fRectWPF.Size.y
			){
				HitDetail=System::Windows::Media::IntersectionDetail::FullyContains;
			}
			else if(
				HitBound.Left<0 && HitBound.Right>fRectWPF.Size.x &&
				HitBound.Top<0 && HitBound.Bottom>fRectWPF.Size.y
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
bool mcWPFView::WPFViewHitTestCore(System::Windows::Point %HitPoint)
{
	if(HitPoint.X>=0 && HitPoint.X<=fRectWPF.Size.x){
		if(HitPoint.Y>=0 && HitPoint.Y<=fRectWPF.Size.y){
			return true;
		}
	}

	return false;
}
//---------------------------------------------------------------------------
void mcWPFView::WPFViewOnRender(System::Windows::Media::DrawingContext^ drawingContext)
{
	auto WPFContent=fWPFContent.Get();
	drawingContext->DrawDrawing(WPFContent);
}
//---------------------------------------------------------------------------
void mcWPFView::WPFViewOnIsKeyboardFocusedChanged(System::Windows::DependencyPropertyChangedEventArgs %e)
{
	bool Focused=*static_cast<System::Boolean^>(e.NewValue);

	return WPFUIViewOnIsKeyboardFocusedChanged(Focused);
}
//---------------------------------------------------------------------------
void mcWPFView::WPFViewOnIsKeyboardFocusWithinChanged(System::Windows::DependencyPropertyChangedEventArgs %e)
{
	bool Focused=*static_cast<System::Boolean^>(e.NewValue);

	return WPFUIViewOnIsKeyboardFocusWithinChanged(Focused);
}
//---------------------------------------------------------------------------
void mcWPFView::WPFViewOnKeyDown(System::Windows::Input::KeyEventArgs^ e)
{
	mcWPFKeyEventArgs EventArgs(e);

	bool Repeat=e->IsRepeat;
	auto Key=static_cast<eKeyCode>(System::Windows::Input::KeyInterop::VirtualKeyFromKey(e->Key));

	WPFUIViewOnKeyDownFilter(EventArgs,Key,Repeat);

	auto View=fWPFView.Get();
	if(e->OriginalSource==View){
		WPFUIViewOnKeyDown(EventArgs,Key,Repeat);
	}
}
//---------------------------------------------------------------------------
void mcWPFView::WPFViewOnKeyUp(System::Windows::Input::KeyEventArgs^ e)
{
	mcWPFKeyEventArgs EventArgs(e);

	auto Key=static_cast<eKeyCode>(System::Windows::Input::KeyInterop::VirtualKeyFromKey(e->Key));
	WPFUIViewOnKeyUpFilter(EventArgs,Key);

	auto View=fWPFView.Get();
	if(e->OriginalSource==View){
		WPFUIViewOnKeyUp(EventArgs,Key);
	}
}
//---------------------------------------------------------------------------
eMouseButton mcWPFView::ToMouseButton(System::Windows::Input::MouseButton Button)
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
void mcWPFView::WPFViewOnIsMouseDirectlyOverChanged(System::Windows::DependencyPropertyChangedEventArgs %e)
{
	bool InRange=*static_cast<System::Boolean^>(e.NewValue);
	
	WPFUIViewOnIsMouseDirectlyOverChanged(InRange);
}
//---------------------------------------------------------------------------
void mcWPFView::WPFViewOnMouseEnter(System::Windows::Input::MouseEventArgs^ e)
{
	mcWPFMouseEventArgs MouseEventArgs(e);

	WPFUIViewOnMouseEnter(MouseEventArgs);
}
//---------------------------------------------------------------------------
void mcWPFView::WPFViewOnMouseLeave(System::Windows::Input::MouseEventArgs^ e)
{
	mcWPFMouseEventArgs MouseEventArgs(e);

	WPFUIViewOnMouseLeave(MouseEventArgs);
}
//---------------------------------------------------------------------------
void mcWPFView::WPFViewOnMouseMove(System::Windows::Input::MouseEventArgs^ e)
{
	if(e->StylusDevice!=nullptr){
		// input from touch
		return;
	}


	auto View=fWPFView.Get();
	mcWPFMouseEventArgs MouseEventArgs(e);
	WPFUIViewOnMouseMoveFilter(MouseEventArgs);
	if(e->OriginalSource==View){
		WPFUIViewOnMouseMove(MouseEventArgs);
	}
}
//---------------------------------------------------------------------------
void mcWPFView::WPFViewOnMouseDown(System::Windows::Input::MouseButtonEventArgs^ e)
{
	if(e->StylusDevice!=nullptr){
		// input from touch
		return;
	}
	auto View=fWPFView.Get();
	if(e->OriginalSource==View){
		auto Element=dynamic_cast<System::Windows::UIElement^>(View);
		Element->CaptureMouse();
	}
	mcWPFMouseEventArgs MouseEventArgs(e);
	auto MouseButton=ToMouseButton(e->ChangedButton);
	WPFUIViewOnMouseDownFilter(MouseEventArgs,MouseButton);
	if(e->OriginalSource==View){
		WPFUIViewOnMouseDown(MouseEventArgs,MouseButton);
	}
}
//---------------------------------------------------------------------------
void mcWPFView::WPFViewOnMouseUp(System::Windows::Input::MouseButtonEventArgs^ e)
{
	if(e->StylusDevice!=nullptr){
		// input from touch
		return;
	}
	auto View=fWPFView.Get();
	if(e->OriginalSource==View){
		auto Element=dynamic_cast<System::Windows::UIElement^>(View);
		Element->ReleaseMouseCapture();
	}

	mcWPFMouseEventArgs MouseEventArgs(e);
	auto MouseButton=ToMouseButton(e->ChangedButton);
	WPFUIViewOnMouseUpFilter(MouseEventArgs,MouseButton);
	if(e->OriginalSource==View){
		WPFUIViewOnMouseUp(MouseEventArgs,ToMouseButton(e->ChangedButton));
	}
}
//---------------------------------------------------------------------------
void mcWPFView::WPFViewOnMouseWheel(System::Windows::Input::MouseWheelEventArgs^ e)
{
	if(e->StylusDevice!=nullptr){
		// input from touch
		return;
	}

	Float32 ScrollY;
	ScrollY=-static_cast<Float32>(e->Delta);
	mcWPFMouseEventArgs MouseEventArgs(e);
	WPFUIViewOnMouseWheelFilter(MouseEventArgs,0,ScrollY);
	auto View=fWPFView.Get();
	if(e->OriginalSource==View){
		WPFUIViewOnMouseWheel(MouseEventArgs,0,ScrollY);
	}
}
//---------------------------------------------------------------------------
void mcWPFView::WPFViewOnTouchDown(System::Windows::Input::TouchEventArgs^ e)
{
	mcWPFTouchEventArgs TouchEventArgs(e);
	WPFUIViewOnTouchDownFilter(TouchEventArgs);
	auto View=fWPFView.Get();
	if(e->OriginalSource==View){
		WPFUIViewOnTouchDown(TouchEventArgs);
	}
}
//---------------------------------------------------------------------------
void mcWPFView::WPFViewOnTouchUp(System::Windows::Input::TouchEventArgs^ e)
{
	mcWPFTouchEventArgs TouchEventArgs(e);
	WPFUIViewOnTouchUpFilter(TouchEventArgs);
	auto View=fWPFView.Get();
	if(e->OriginalSource==View){
		WPFUIViewOnTouchUp(TouchEventArgs);
	}
}
//---------------------------------------------------------------------------
void mcWPFView::WPFViewOnTouchMove(System::Windows::Input::TouchEventArgs^ e)
{
	mcWPFTouchEventArgs TouchEventArgs(e);
	WPFUIViewOnTouchMoveFilter(TouchEventArgs);
	auto View=fWPFView.Get();
	if(e->OriginalSource!=View){
		WPFUIViewOnTouchMove(TouchEventArgs);
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
WPFView::WPFView()
{
	IsVisibleChanged+=gcnew System::Windows::DependencyPropertyChangedEventHandler(&WPFView::OnIsVisibleChanged);
}
//---------------------------------------------------------------------------
void WPFView::CPPAttach(mcWPFView *p)
{
	if(CPP==nullptr){
		CPP=p;
	}
}
//---------------------------------------------------------------------------
void WPFView::CPPDetach(mcWPFView *p)
{
	if(CPP==p){
		CPP=nullptr;
	}
}
//---------------------------------------------------------------------------
System::Windows::UIElement^ WPFView::Element::get(void)
{
	return this;
}
//---------------------------------------------------------------------------
iWPFViewChild* WPFView::ChildInterface::get(void)
{
	return CPP;
}
//---------------------------------------------------------------------------
bool WPFView::ParentAcquire(IWPFViewParent ^Parent)
{
	return CPP->WPFViewParentAcquire(Parent);
}
//---------------------------------------------------------------------------
void WPFView::ParentRelease(IWPFViewParent ^Parent)
{
	return CPP->WPFViewParentRelease(Parent);
}
//---------------------------------------------------------------------------
iWPFViewParent* WPFView::ParentInterface::get(void)
{
	return CPP;
}
//---------------------------------------------------------------------------
void WPFView::ParentRemoveViewChild(IWPFView ^Subview)
{
	CPP->WPFParentRemoveChild(Subview);
}
//---------------------------------------------------------------------------
System::Windows::Media::Visual^ WPFView::Visual::get(void)
{
	return this;
}
//---------------------------------------------------------------------------
float WPFView::LayoutScale::get(void)
{
	return CPP->WPFChildGetLayoutScale();
}
//---------------------------------------------------------------------------
System::Windows::Media::Visual^ WPFView::GetVisualChild(int index)
{
	return CPP->WPFViewGetVisualChild(index);
}
//---------------------------------------------------------------------------
int WPFView::VisualChildrenCount::get(void)
{
	return CPP->WPFViewGetVisualChildrenCount();
}
//---------------------------------------------------------------------------
bool WPFView::InsertChildElement(System::Windows::UIElement ^Child)
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
bool WPFView::RemoveChildElement(System::Windows::UIElement ^Child)
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
System::Windows::DependencyObject^ WPFView::GetParent(void)
{
	return VisualParent;
}
//---------------------------------------------------------------------------
void WPFView::DispatcherFinishCleanup(void)
{
}
//---------------------------------------------------------------------------
void WPFView::OnVisualParentChanged(System::Windows::DependencyObject ^oldParent)
{
	__super::OnVisualParentChanged(oldParent);
	if(CPP!=nullptr){
		CPP->WPFViewOnVisualParentChanged(VisualParent);
	}
}
//---------------------------------------------------------------------------
System::Windows::Size WPFView::MeasureCore(System::Windows::Size availableSize)
{
	if(CPP==nullptr){
		return __super::MeasureCore(availableSize);
	}
	return CPP->WPFViewMeasureCore(availableSize);
}
//---------------------------------------------------------------------------
void WPFView::ArrangeCore(System::Windows::Rect finalRect)
{
	__super::ArrangeCore(finalRect);

	if(CPP==nullptr){
		return;
	}
	CPP->WPFViewArrangeCore(finalRect);
}
//---------------------------------------------------------------------------
void WPFView::OnRender(System::Windows::Media::DrawingContext^ drawingContext)
{
	__super::OnRender(drawingContext);
	
	if(CPP!=nullptr){
		CPP->WPFViewOnRender(drawingContext);
	}
}
//---------------------------------------------------------------------------
System::Windows::Media::GeometryHitTestResult^ WPFView::HitTestCore(System::Windows::Media::GeometryHitTestParameters ^ hitTestParameters)
{
	if(CPP==nullptr){
		return __super::HitTestCore(hitTestParameters);
	}
	System::Windows::Media::IntersectionDetail HitDetail;
	if(CPP->WPFViewHitTestCore(HitDetail,hitTestParameters)){
		auto HitTestResult=gcnew System::Windows::Media::GeometryHitTestResult(this,HitDetail);
		return HitTestResult;
	}
	return nullptr;
}
//---------------------------------------------------------------------------
System::Windows::Media::HitTestResult^ WPFView::HitTestCore(System::Windows::Media::PointHitTestParameters ^ hitTestParameters)
{
	if(CPP==nullptr){
		return __super::HitTestCore(hitTestParameters);
	}
	System::Windows::Point HitPoint=hitTestParameters->HitPoint;
	if(CPP->WPFViewHitTestCore(HitPoint)){
		auto HitTestResult=gcnew System::Windows::Media::PointHitTestResult(this,HitPoint);
		return HitTestResult;
	}
	return nullptr;
}
//---------------------------------------------------------------------------
void WPFView::OnIsVisibleChanged(System::Object ^sender,System::Windows::DependencyPropertyChangedEventArgs e)
{
	auto CPP=static_cast<WPFView^>(sender)->CPP;
	if(CPP==nullptr)
		return;

	CPP->WPFViewIsVisibleChanged(e);
}
//---------------------------------------------------------------------------
void WPFView::OnIsKeyboardFocusedChanged(System::Windows::DependencyPropertyChangedEventArgs e)
{
	__super::OnIsKeyboardFocusedChanged(e);

	if(CPP!=nullptr){
		CPP->WPFViewOnIsKeyboardFocusedChanged(e);
	}
}
//---------------------------------------------------------------------------
void WPFView::OnIsKeyboardFocusWithinChanged(System::Windows::DependencyPropertyChangedEventArgs e)
{
	__super::OnIsKeyboardFocusWithinChanged(e);

	if(CPP!=nullptr){
		CPP->WPFViewOnIsKeyboardFocusWithinChanged(e);
	}
}
//---------------------------------------------------------------------------
void WPFView::OnKeyDown(System::Windows::Input::KeyEventArgs^ e)
{
	__super::OnKeyDown(e);
	
	if(CPP!=nullptr){
		CPP->WPFViewOnKeyDown(e);
	}
}
//---------------------------------------------------------------------------
void WPFView::OnKeyUp(System::Windows::Input::KeyEventArgs^ e)
{
	__super::OnKeyUp(e);

	if(CPP!=nullptr){
		CPP->WPFViewOnKeyUp(e);
	}
}
//---------------------------------------------------------------------------
void WPFView::OnIsMouseDirectlyOverChanged(System::Windows::DependencyPropertyChangedEventArgs e)
{
	__super::OnIsMouseDirectlyOverChanged(e);

	if(CPP!=nullptr){
		CPP->WPFViewOnIsMouseDirectlyOverChanged(e);
	}
}
//---------------------------------------------------------------------------
void WPFView::OnMouseEnter(System::Windows::Input::MouseEventArgs^ e)
{
	__super::OnMouseEnter(e);

	if(CPP!=nullptr){
		CPP->WPFViewOnMouseEnter(e);
	}
}
//---------------------------------------------------------------------------
void WPFView::OnMouseLeave(System::Windows::Input::MouseEventArgs^ e)
{
	__super::OnMouseLeave(e);

	if(CPP!=nullptr){
		CPP->WPFViewOnMouseLeave(e);
	}
}
//---------------------------------------------------------------------------
void WPFView::OnMouseMove(System::Windows::Input::MouseEventArgs^ e)
{
	__super::OnMouseMove(e);
	
	if(CPP!=nullptr){
		CPP->WPFViewOnMouseMove(e);
	}
}
//---------------------------------------------------------------------------
void WPFView::OnMouseDown(System::Windows::Input::MouseButtonEventArgs^ e)
{
	__super::OnMouseDown(e);

	if(CPP!=nullptr){
		CPP->WPFViewOnMouseDown(e);
	}
}
//---------------------------------------------------------------------------
void WPFView::OnMouseUp(System::Windows::Input::MouseButtonEventArgs^ e)
{
	__super::OnMouseUp(e);

	if(CPP!=nullptr){
		CPP->WPFViewOnMouseUp(e);
	}
}
//---------------------------------------------------------------------------
void WPFView::OnMouseWheel(System::Windows::Input::MouseWheelEventArgs^ e)
{
	__super::OnMouseWheel(e);

	if(CPP!=nullptr){
		CPP->WPFViewOnMouseWheel(e);
	}
}
//---------------------------------------------------------------------------
void WPFView::OnTouchDown(System::Windows::Input::TouchEventArgs^ e)
{
	__super::OnTouchDown(e);

	if(CPP!=nullptr){
		CPP->WPFViewOnTouchDown(e);
	}
}
//---------------------------------------------------------------------------
void WPFView::OnTouchUp(System::Windows::Input::TouchEventArgs^ e)
{
	__super::OnTouchUp(e);

	if(CPP!=nullptr){
		CPP->WPFViewOnTouchUp(e);
	}
}
//---------------------------------------------------------------------------
void WPFView::OnTouchMove(System::Windows::Input::TouchEventArgs^ e)
{
	__super::OnTouchMove(e);

	if(CPP!=nullptr){
		CPP->WPFViewOnTouchMove(e);
	}
}
//---------------------------------------------------------------------------

#pragma message("-------- Reform --------------------")

#if 0

//---------------------------------------------------------------------------
#pragma managed
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
rcWPFUIElementAsUIView::rcWPFUIElementAsUIView(mcWPFUIElementAsUIView *CPP,System::Windows::UIElement ^Target)
	: CPP(CPP)
	, fTarget(Target)
{
}
//---------------------------------------------------------------------------
//IWPFViewElement ^rcWPFUIElementAsUIView::Target::get(void)
//{
//	return fTarget;
//}
////---------------------------------------------------------------------------
//void rcWPFUIElementAsUIView::Target::set(IWPFViewElement ^value)
//{
//	if(fTarget!=nullptr){
//		auto Element=dynamic_cast<System::Windows::UIElement^>(fTarget);
//		if(Element!=nullptr){
//			Element->IsVisibleChanged-=gcnew System::Windows::DependencyPropertyChangedEventHandler(OnIsVisibleChanged);
//			Element->IsKeyboardFocusedChanged-=gcnew System::Windows::DependencyPropertyChangedEventHandler(OnIsKeyboardFocusedChanged);
//			Element->IsKeyboardFocusWithinChanged-=gcnew System::Windows::DependencyPropertyChangedEventHandler(OnIsKeyboardFocusWithinChanged);
//			Element->KeyDown-=gcnew System::Windows::Input::KeyEventHandler(OnKeyDown);
//			Element->KeyUp-=gcnew System::Windows::Input::KeyEventHandler(OnKeyUp);
//			Element->IsMouseDirectlyOverChanged-=gcnew System::Windows::DependencyPropertyChangedEventHandler(OnIsMouseDirectlyOverChanged);
//			Element->MouseEnter-=gcnew System::Windows::Input::MouseEventHandler(OnMouseEnter);
//			Element->MouseLeave-=gcnew System::Windows::Input::MouseEventHandler(OnMouseLeave);
//			Element->MouseMove-=gcnew System::Windows::Input::MouseEventHandler(OnMouseMove);
//			Element->MouseDown-=gcnew System::Windows::Input::MouseButtonEventHandler(OnMouseDown);
//			Element->MouseUp-=gcnew System::Windows::Input::MouseButtonEventHandler(OnMouseUp);
//			Element->MouseWheel-=gcnew System::Windows::Input::MouseWheelEventHandler(OnMouseWheel);
//			Element->TouchDown-=gcnew System::EventHandler<System::Windows::Input::TouchEventArgs^>(OnTouchDown);
//			Element->TouchUp-=gcnew System::EventHandler<System::Windows::Input::TouchEventArgs^>(OnTouchUp);
//			Element->TouchMove-=gcnew System::EventHandler<System::Windows::Input::TouchEventArgs^>(OnTouchMove);
//		}
//	}
//	fTarget=value;
//	if(fTarget!=nullptr){
//		auto Element=dynamic_cast<System::Windows::UIElement^>(fTarget);
//		if(Element!=nullptr){
//			Element->IsVisibleChanged+=gcnew System::Windows::DependencyPropertyChangedEventHandler(OnIsVisibleChanged);
//			Element->IsKeyboardFocusedChanged+=gcnew System::Windows::DependencyPropertyChangedEventHandler(OnIsKeyboardFocusedChanged);
//			Element->IsKeyboardFocusWithinChanged+=gcnew System::Windows::DependencyPropertyChangedEventHandler(OnIsKeyboardFocusWithinChanged);
//			Element->KeyDown+=gcnew System::Windows::Input::KeyEventHandler(OnKeyDown);
//			Element->KeyUp+=gcnew System::Windows::Input::KeyEventHandler(OnKeyUp);
//			Element->IsMouseDirectlyOverChanged+=gcnew System::Windows::DependencyPropertyChangedEventHandler(OnIsMouseDirectlyOverChanged);
//			Element->MouseEnter+=gcnew System::Windows::Input::MouseEventHandler(OnMouseEnter);
//			Element->MouseLeave+=gcnew System::Windows::Input::MouseEventHandler(OnMouseLeave);
//			Element->MouseMove+=gcnew System::Windows::Input::MouseEventHandler(OnMouseMove);
//			Element->MouseDown+=gcnew System::Windows::Input::MouseButtonEventHandler(OnMouseDown);
//			Element->MouseUp+=gcnew System::Windows::Input::MouseButtonEventHandler(OnMouseUp);
//			Element->MouseWheel+=gcnew System::Windows::Input::MouseWheelEventHandler(OnMouseWheel);
//			Element->TouchDown+=gcnew System::EventHandler<System::Windows::Input::TouchEventArgs^>(OnTouchDown);
//			Element->TouchUp+=gcnew System::EventHandler<System::Windows::Input::TouchEventArgs^>(OnTouchUp);
//			Element->TouchMove+=gcnew System::EventHandler<System::Windows::Input::TouchEventArgs^>(OnTouchMove);
//		}
//	}
//}
//---------------------------------------------------------------------------
bool rcWPFUIElementAsUIView::InsertChild(System::Windows::UIElement ^Child)
{
	return false;
}
//---------------------------------------------------------------------------
bool rcWPFUIElementAsUIView::RemoveChild(System::Windows::UIElement ^Child)
{
	return false;
}
//---------------------------------------------------------------------------
System::Windows::DependencyObject^ rcWPFUIElementAsUIView::GetParent(void)
{
	return nullptr;
	//if(fTarget==nullptr)
	//	return nullptr;
	//return fTarget->ViewParent;
}
//---------------------------------------------------------------------------
void rcWPFUIElementAsUIView::DispatcherFinishCleanup(void)
{
	//Target=nullptr;
}
//---------------------------------------------------------------------------
cnWin::iWPFViewParent* rcWPFUIElementAsUIView::ParentInterface::get(void)
{
	return CPP;
}
//---------------------------------------------------------------------------
void rcWPFUIElementAsUIView::ParentRemoveViewChild(IWPFView ^Subview)
{
	//return CPP->WPFParentRemoveChild(Subview);
}
//---------------------------------------------------------------------------
System::Windows::Media::Visual^ rcWPFUIElementAsUIView::Visual::get(void)
{
	return dynamic_cast<System::Windows::Media::Visual^>(fTarget);
}
//---------------------------------------------------------------------------
float rcWPFUIElementAsUIView::LayoutScale::get(void)
{
	return CPP->WPFChildGetLayoutScale();
}
//---------------------------------------------------------------------------
System::Windows::UIElement^ rcWPFUIElementAsUIView::Element::get(void)
{
	return dynamic_cast<System::Windows::UIElement^>(fTarget);
}
//---------------------------------------------------------------------------
cnWin::iWPFViewChild* rcWPFUIElementAsUIView::ChildInterface::get(void)
{
	return CPP;
}
//---------------------------------------------------------------------------
bool rcWPFUIElementAsUIView::ParentAcquire(IWPFViewParent ^Parent)
{
	//return CPP->WPFViewParentAcquire(Parent);
	return false;
}
//---------------------------------------------------------------------------
void rcWPFUIElementAsUIView::ParentRelease(IWPFViewParent ^Parent)
{
	//return CPP->WPFViewParentRelease(Parent);
}
//---------------------------------------------------------------------------
mcWPFUIElementAsUIView* rcWPFUIElementAsUIView::CPPFromSender(System::Object ^sender)
{
	return nullptr;
}
//---------------------------------------------------------------------------
void rcWPFUIElementAsUIView::OnIsVisibleChanged(System::Object ^sender,System::Windows::DependencyPropertyChangedEventArgs e)
{
	auto CPP=CPPFromSender(sender);
	if(CPP!=nullptr){
//		CPP->WPFViewIsVisibleChanged(e);
	}
}
//---------------------------------------------------------------------------
void rcWPFUIElementAsUIView::OnIsKeyboardFocusedChanged(System::Object ^sender,System::Windows::DependencyPropertyChangedEventArgs e)
{
	auto CPP=CPPFromSender(sender);
	if(CPP!=nullptr){
//		CPP->WPFViewOnIsKeyboardFocusedChanged(e);
	}
}
//---------------------------------------------------------------------------
void rcWPFUIElementAsUIView::OnIsKeyboardFocusWithinChanged(System::Object ^sender,System::Windows::DependencyPropertyChangedEventArgs e)
{
	auto CPP=CPPFromSender(sender);
	if(CPP!=nullptr){
//		CPP->WPFViewOnIsKeyboardFocusWithinChanged(e);
	}
}
//---------------------------------------------------------------------------
void rcWPFUIElementAsUIView::OnKeyDown(System::Object ^sender,System::Windows::Input::KeyEventArgs^ e)
{
	auto CPP=CPPFromSender(sender);
	if(CPP!=nullptr){
//		return CPP->WPFViewOnKeyDown(e);
	}
}
//---------------------------------------------------------------------------
void rcWPFUIElementAsUIView::OnKeyUp(System::Object ^sender,System::Windows::Input::KeyEventArgs^ e)
{
	auto CPP=CPPFromSender(sender);
	if(CPP!=nullptr){
//		return CPP->WPFViewOnKeyUp(e);
	}
}
//---------------------------------------------------------------------------
void rcWPFUIElementAsUIView::OnIsMouseDirectlyOverChanged(System::Object ^sender,System::Windows::DependencyPropertyChangedEventArgs e)
{
	auto CPP=CPPFromSender(sender);
	if(CPP!=nullptr){
//		return CPP->WPFViewOnIsMouseDirectlyOverChanged(e);
	}
}
//---------------------------------------------------------------------------
void rcWPFUIElementAsUIView::OnMouseEnter(System::Object ^sender,System::Windows::Input::MouseEventArgs^ e)
{
	auto CPP=CPPFromSender(sender);
	if(CPP!=nullptr){
//		return CPP->WPFViewOnMouseEnter(e);
	}
}
//---------------------------------------------------------------------------
void rcWPFUIElementAsUIView::OnMouseLeave(System::Object ^sender,System::Windows::Input::MouseEventArgs^ e)
{
	auto CPP=CPPFromSender(sender);
	if(CPP!=nullptr){
//		return CPP->WPFViewOnMouseLeave(e);
	}
}
//---------------------------------------------------------------------------
void rcWPFUIElementAsUIView::OnMouseMove(System::Object ^sender,System::Windows::Input::MouseEventArgs^ e)
{
	auto CPP=CPPFromSender(sender);
	if(CPP!=nullptr){
//		return CPP->WPFViewOnMouseMove(e);
	}
}
//---------------------------------------------------------------------------
void rcWPFUIElementAsUIView::OnMouseDown(System::Object ^sender,System::Windows::Input::MouseButtonEventArgs^ e)
{
	auto CPP=CPPFromSender(sender);
	if(CPP!=nullptr){
//		return CPP->WPFViewOnMouseDown(e);
	}
}
//---------------------------------------------------------------------------
void rcWPFUIElementAsUIView::OnMouseUp(System::Object ^sender,System::Windows::Input::MouseButtonEventArgs^ e)
{
	auto CPP=CPPFromSender(sender);
	if(CPP!=nullptr){
//		return CPP->WPFViewOnMouseUp(e);
	}
}
//---------------------------------------------------------------------------
void rcWPFUIElementAsUIView::OnMouseWheel(System::Object ^sender,System::Windows::Input::MouseWheelEventArgs^ e)
{
	auto CPP=CPPFromSender(sender);
	if(CPP!=nullptr){
//		return CPP->WPFViewOnMouseWheel(e);
	}
}
//---------------------------------------------------------------------------
void rcWPFUIElementAsUIView::OnTouchDown(System::Object ^sender,System::Windows::Input::TouchEventArgs^ e)
{
	auto CPP=CPPFromSender(sender);
	if(CPP!=nullptr){
//		return CPP->WPFViewOnTouchDown(e);
	}
}
//---------------------------------------------------------------------------
void rcWPFUIElementAsUIView::OnTouchUp(System::Object ^sender,System::Windows::Input::TouchEventArgs^ e)
{
	auto CPP=CPPFromSender(sender);
	if(CPP!=nullptr){
//		return CPP->WPFViewOnTouchUp(e);
	}
}
//---------------------------------------------------------------------------
void rcWPFUIElementAsUIView::OnTouchMove(System::Object ^sender,System::Windows::Input::TouchEventArgs^ e)
{
	auto CPP=CPPFromSender(sender);
	if(CPP!=nullptr){
//		return CPP->WPFViewOnTouchMove(e);
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
mcWPFUIElementAsUIView::mcWPFUIElementAsUIView(System::Windows::UIElement ^TargetElement)
{
	auto WPF=gcnew rcWPFUIElementAsUIView(this,TargetElement);
	fWPF.Set(WPF);

	fRect=UIRectZero;
	fZPosition=0;

	//TargetElement->LayoutUpdated
}
//---------------------------------------------------------------------------
mcWPFUIElementAsUIView::~mcWPFUIElementAsUIView()
{
	fWPF.Set(nullptr);
}
//---------------------------------------------------------------------------
void mcWPFUIElementAsUIView::VirtualStarted(void)
{
	auto WPF=fWPF.Get();
	auto TargetElement=WPF->Element;
	auto Dispatcher=TargetElement->Dispatcher;
	fThreadMsg=0;
	if(Dispatcher->CheckAccess()){
		mSetupUIThread();
	}
	else{
		//cDNetUIThread::mDispatchExecuteSync(Dispatcher,System::Windows::Threading::DispatcherPriority::Normal,&fUIThreadSetupProc);
	}
}
//---------------------------------------------------------------------------
void mcWPFUIElementAsUIView::VirtualStopped(void)
{
	auto WPF=fWPF.Get();
	auto TargetElement=WPF->Element;
	auto Dispatcher=TargetElement->Dispatcher;
	fThreadMsg=1;
	if(fUIThread->mIsCurrent()){
		mThreadDelete();
	}
	else{
		//cDNetUIThread::mDispatchExecute(Dispatcher,System::Windows::Threading::DispatcherPriority::Normal,nullptr,&fUIThreadSetupProc);
	}
}
//---------------------------------------------------------------------------
void mcWPFUIElementAsUIView::mSetupUIThread(void)
{
	fUIThread=cDNetUIThread::CreateOnCurrentThread();
}
//---------------------------------------------------------------------------
//void mcWPFUIElementAsUIView::cUIThreadSetupProc::Execute(void)
//{
//	auto Host=cnMemory::GetObjectFromMemberPointer(this,&mcWPFUIElementAsUIView::fUIThreadSetupProc);
//	switch(Host->fThreadMsg){
//	case 0:
//		Host->mSetupUIThread();
//		break;
//	case 1:
//		Host->mThreadDelete();
//		break;
//	}
//}
//---------------------------------------------------------------------------
void mcWPFUIElementAsUIView::mThreadDelete(void)
{
	if(fParent!=nullptr){
		//fParent->WPFParentRemoveSubview(this);
	}
	VirtualDelete();
}
//---------------------------------------------------------------------------
bool mcWPFUIElementAsUIView::mCheckThread(void)
{
	return fUIThread->mIsCurrent();
}
//---------------------------------------------------------------------------
#pragma unmanaged
//---------------------------------------------------------------------------
cGCRef& mcWPFUIElementAsUIView::GetObjecHandle(void)
{
	return fWPF;
}
//---------------------------------------------------------------------------
cGCRef& mcWPFUIElementAsUIView::WPFChildGetElementHandle(void)
{
	return fWPF;
}
//---------------------------------------------------------------------------
#pragma managed
//---------------------------------------------------------------------------
bool mcWPFUIElementAsUIView::mTranslatePointTo(cGCRef &Handle,cUIPoint &Position)
{
	auto WPF=fWPF.Get();
	auto TargetElement=WPF->Element;
	System::Object^ RelativeObject=Handle;
	auto RelativeVisual=dynamic_cast<System::Windows::Media::Visual^>(RelativeObject);
	if(RelativeVisual!=nullptr){
		auto Transform=TargetElement->TransformToVisual(RelativeVisual);
		System::Windows::Point SrcPt;
		SrcPt.X=Position.x;
		SrcPt.Y=Position.y;
		System::Windows::Point ResultPoint;
		Transform->TryTransform(SrcPt,ResultPoint);
		Position.x=static_cast<Float32>(ResultPoint.X);
		Position.y=static_cast<Float32>(ResultPoint.Y);
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------
bool mcWPFUIElementAsUIView::mSetPosition(cGCRef &Handle,cUIPoint Position)
{
	if(mCheckThread()==false)
		return false;
	
	fRect.Pos=Position;

	mApplyRect();
	return true;
}
//---------------------------------------------------------------------------
bool mcWPFUIElementAsUIView::mSetRectangle(cGCRef &Handle,cUIPoint Position,cUIPoint Size)
{
	if(mCheckThread()==false)
		return false;

	if(Size.x<0)
		Size.x=0;
	if(Size.y<0)
		Size.y=0;

	fRect.Pos=Position;
	fRect.Size=Size;

	mApplyRect();
	return true;
}
//---------------------------------------------------------------------------
void mcWPFUIElementAsUIView::mApplyRect(void)
{
	try{

		System::Windows::Rect WPFRect;
		WPFRect.X=fRect.Pos.x;
		WPFRect.Y=fRect.Pos.y;
		WPFRect.Width=fRect.Size.x;
		WPFRect.Height=fRect.Size.y;
		auto WPF=fWPF.Get();
		auto TargetElement=WPF->Element;
		TargetElement->Arrange(WPFRect);
	}
	catch(System::Exception^){
	}
}
//---------------------------------------------------------------------------
bool mcWPFUIElementAsUIView::GetVisible(void)
{
	if(mCheckThread()==false)
		return false;
	auto WPF=fWPF.Get();
	auto TargetElement=WPF->Element;
	return TargetElement->Visibility==System::Windows::Visibility::Visible;
}
//---------------------------------------------------------------------------
bool mcWPFUIElementAsUIView::SetVisible(bool Visible)
{
	if(mCheckThread()==false)
		return false;

	auto WPF=fWPF.Get();
	auto TargetElement=WPF->Element;
	if(Visible){
		TargetElement->Visibility=System::Windows::Visibility::Visible;
	}
	else{
		TargetElement->Visibility=System::Windows::Visibility::Collapsed;
	}
	return true;
}
//---------------------------------------------------------------------------
bool mcWPFUIElementAsUIView::IsEnabled(void)
{
	if(mCheckThread()==false)
		return false;

	auto WPF=fWPF.Get();
	auto TargetElement=WPF->Element;
	return TargetElement->IsEnabled;
}
//---------------------------------------------------------------------------
bool mcWPFUIElementAsUIView::GetEnable(void)
{
	if(mCheckThread()==false)
		return false;

	auto WPF=fWPF.Get();
	auto TargetElement=WPF->Element;
	return TargetElement->IsEnabled;
}
//---------------------------------------------------------------------------
bool mcWPFUIElementAsUIView::SetEnable(bool Enable)
{
	if(mCheckThread()==false)
		return false;

	auto WPF=fWPF.Get();
	auto TargetElement=WPF->Element;
	TargetElement->IsEnabled=Enable;
	return true;
}
//---------------------------------------------------------------------------
#pragma managed
//---------------------------------------------------------------------------
#endif // 0
