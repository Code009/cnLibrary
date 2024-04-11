#include "WinDNet_WPFUIView.h"

using namespace cnLibrary;

using namespace cnRTL;
using namespace cnWin;


//---------------------------------------------------------------------------
void* iWPFViewChild::CastInterface(iTypeID ID)noexcept
{
	if(ID==TInterfaceID<iWPFViewChild>::Value){
		return this;
	}
	return iInterface::CastInterface(ID);
}
//---------------------------------------------------------------------------
iPtr<iUIView> cnWin::DNetCreateUIView(mcDNetUIThreadDispatcher *Dispatcher,mcWPFView::mcConstructParameter &Parameter)noexcept
{
	return iCreate<cWPFUIView>(static_cast<cDNetUIThread*>(Dispatcher),Parameter);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
eUIEventSourceType bcWPFKeyEvent::GetSourceType(void)noexcept
{
	return UIEventSourceType::Keyboard;
}
//---------------------------------------------------------------------------
iUIView* bcWPFKeyEvent::GetSourceView(void)noexcept
{
	return nullptr;
}
//---------------------------------------------------------------------------
iUIKeyboard* bcWPFKeyEvent::GetKeyboard(void)noexcept
{
	auto Thread=cDNetUIThread::CurrentUIThread();
	if(Thread==nullptr)
		return nullptr;
	return Thread->GetDefaultKeyboard();
}
//---------------------------------------------------------------------------
void bcWPFKeyEvent::CancelEvent(void)noexcept
{
}
//---------------------------------------------------------------------------
bool bcWPFKeyEvent::IsCancelled(void)noexcept
{
	return false;
}
//---------------------------------------------------------------------------
bool bcWPFKeyEvent::IsKeyDown(eKeyCode KeyCode)noexcept
{
	return mcDNetUIThreadDispatcher::mKeyboardEventIsKeyDown(KeyCode);
}
//---------------------------------------------------------------------------
bool bcWPFKeyEvent::IsKeyToggled(eKeyCode KeyCode)noexcept
{
	return mcDNetUIThreadDispatcher::mKeyboardEventIsKeyToggled(KeyCode);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
eUIEventSourceType bcWPFMouseEvent::GetSourceType(void)noexcept
{
	return UIEventSourceType::Mouse;
}
//---------------------------------------------------------------------------
iUIMouse* bcWPFMouseEvent::GetMouse(void)noexcept
{
	auto Thread=cDNetUIThread::CurrentUIThread();
	if(Thread==nullptr)
		return nullptr;
	return Thread->GetDefaultMouse();
}
//---------------------------------------------------------------------------
void bcWPFMouseEvent::CancelEvent(void)noexcept
{
}
//---------------------------------------------------------------------------
bool bcWPFMouseEvent::IsCancelled(void)noexcept
{
	return false;
}
//---------------------------------------------------------------------------
bool bcWPFMouseEvent::GetPosition(iInterface *Relative,cUIPoint &Position)noexcept
{
	auto RelativeObject=iCast<iCLIObject>(Relative);
	if(RelativeObject==nullptr)
		return false;

	auto &Handle=RelativeObject->GetObjecHandle();
	return mcDNetUIThreadDispatcher::mMouseGetPosition(Handle,Position);
}
//---------------------------------------------------------------------------
bool bcWPFMouseEvent::IsButtonDown(eMouseButton Button)noexcept
{
	return mcDNetUIThreadDispatcher::mMouseEventIsButtonDown(Button);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWPFKeyEventByMessage::cWPFKeyEventByMessage(iUIView *SourceView)noexcept
{
	fSourceView=SourceView;
}
//---------------------------------------------------------------------------
iUIView* cWPFKeyEventByMessage::GetSourceView(void)noexcept
{
	return fSourceView;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWPFMouseEventByMessage::cWPFMouseEventByMessage(iUIView *SourceView)noexcept
{
	fSourceView=SourceView;
}
//---------------------------------------------------------------------------
iUIView* cWPFMouseEventByMessage::GetSourceView(void)noexcept
{
	return fSourceView;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWPFKeyEvent::cWPFKeyEvent(iUIView *SourceView,mcWPFKeyEventArgs &EventArgs)noexcept
	: fEventArgs(EventArgs)
	, fSourceView(SourceView)
{
}
//---------------------------------------------------------------------------
cWPFKeyEvent::~cWPFKeyEvent()noexcept
{
}
//---------------------------------------------------------------------------
iUIView* cWPFKeyEvent::GetSourceView(void)noexcept
{
	return fSourceView;
}
//---------------------------------------------------------------------------
void cWPFKeyEvent::CancelEvent(void)noexcept
{
	return fEventArgs.mCancelEvent();
}
//---------------------------------------------------------------------------
bool cWPFKeyEvent::IsCancelled(void)noexcept
{
	return fEventArgs.mIsCancelled();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWPFMouseEvent::cWPFMouseEvent(iUIView *SourceView,mcWPFMouseEventArgs &EventArgs)noexcept
	: fEventArgs(EventArgs)
	, fSourceView(SourceView)
{
}
//---------------------------------------------------------------------------
cWPFMouseEvent::~cWPFMouseEvent()noexcept
{
}
//---------------------------------------------------------------------------
void cWPFMouseEvent::CancelEvent(void)noexcept
{
	return fEventArgs.mCancelEvent();
}
//---------------------------------------------------------------------------
bool cWPFMouseEvent::IsCancelled(void)noexcept
{
	return fEventArgs.mIsCancelled();
}
//---------------------------------------------------------------------------
eUIEventSourceType cWPFMouseEvent::GetSourceType(void)noexcept
{
	return UIEventSourceType::Mouse;
}
//---------------------------------------------------------------------------
iUIView* cWPFMouseEvent::GetSourceView(void)noexcept
{
	return fSourceView;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWPFTouchEvent::cWPFTouchEvent(iUIView *SourceView,mcWPFTouchEventArgs &EventArgs)noexcept
	: fEventArgs(EventArgs)
	, fSourceView(SourceView)
{
	fTouchID=fEventArgs.mGetTouchID();
}
//---------------------------------------------------------------------------
cWPFTouchEvent::~cWPFTouchEvent()noexcept
{
}
//---------------------------------------------------------------------------
void cWPFTouchEvent::CancelEvent(void)noexcept
{
	return fEventArgs.mCancelEvent();
}
//---------------------------------------------------------------------------
bool cWPFTouchEvent::IsCancelled(void)noexcept
{
	return fEventArgs.mIsCancelled();
}
//---------------------------------------------------------------------------
eUIEventSourceType cWPFTouchEvent::GetSourceType(void)noexcept
{
	return UIEventSourceType::Touch;
}
//---------------------------------------------------------------------------
iUIView* cWPFTouchEvent::GetSourceView(void)noexcept
{
	return fSourceView;
}
//---------------------------------------------------------------------------
void* cWPFTouchEvent::GetTouchID(void)noexcept
{
	return fTouchID;
}
//---------------------------------------------------------------------------
bool cWPFTouchEvent::GetPosition(iInterface *Relative,cUIPoint &Position)noexcept
{
	auto RelativeSubview=iCast<iWPFViewChild>(Relative);
	if(RelativeSubview==nullptr)
		return false;

	auto &Handle=RelativeSubview->WPFChildGetElementHandle();
	return fEventArgs.mGetPosition(Handle,Position);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWPFVisualAsWindow::cWPFVisualAsWindow(mcWPFVisual &&Visual)noexcept
	: mcWPFVisual(cnVar::MoveCast(Visual))
{
}
//---------------------------------------------------------------------------
cWPFVisualAsWindow::~cWPFVisualAsWindow()noexcept
{
}
//---------------------------------------------------------------------------
iUIThread* cWPFVisualAsWindow::GetUIThread(void)noexcept
{
	return nullptr;
}
//---------------------------------------------------------------------------
eUIState cWPFVisualAsWindow::GetUIState(void)noexcept
{
	return UIState::Active;
}
//---------------------------------------------------------------------------
bool cWPFVisualAsWindow::InsertStateHandler(iUIStateHandler *Handler,sfInt16 Order)noexcept
{
	return false;
}
//---------------------------------------------------------------------------
bool cWPFVisualAsWindow::RemoveStateHandler(iUIStateHandler *Handler)noexcept
{
	return false;
}
//---------------------------------------------------------------------------
bool cWPFVisualAsWindow::GetVisible(void)noexcept
{
	return false;
}
//---------------------------------------------------------------------------
bool cWPFVisualAsWindow::SetVisible(bool Visible)noexcept
{
	return false;
}
//---------------------------------------------------------------------------
bool cWPFVisualAsWindow::IsEnabled(void)noexcept
{
	return false;
}
//---------------------------------------------------------------------------
bool cWPFVisualAsWindow::GetEnable(void)noexcept
{
	return false;
}
//---------------------------------------------------------------------------
bool cWPFVisualAsWindow::SetEnable(bool Enable)noexcept
{
	return false;
}
//---------------------------------------------------------------------------
cUIPoint cWPFVisualAsWindow::GetSize(void)noexcept
{
	return {0,0};
}
//---------------------------------------------------------------------------
bool cWPFVisualAsWindow::SetSize(cUIPoint Size)noexcept
{
	return false;
}
//---------------------------------------------------------------------------
bool cWPFVisualAsWindow::TranslatePointTo(iInterface *Relative,cUIPoint &Position)noexcept
{
	return false;
}
//---------------------------------------------------------------------------
bool cWPFVisualAsWindow::MoveTo(iInterface *Relative,cUIPoint Position)noexcept
{
	return false;
}
//---------------------------------------------------------------------------
bool cWPFVisualAsWindow::ArrangeRectangle(iInterface *Relative,cUIPoint Position,cUIPoint Size)noexcept
{
	return false;
}
//---------------------------------------------------------------------------
Float32 cWPFVisualAsWindow::GetZPosition(void)noexcept
{
	return 0.f;
}
//---------------------------------------------------------------------------
bool cWPFVisualAsWindow::SetZPosition(Float32)noexcept
{
	return false;
}
//---------------------------------------------------------------------------
Float32 cWPFVisualAsWindow::GetContentScale(void)noexcept
{
	return 1.f;
}
//---------------------------------------------------------------------------
iUIScreen* cWPFVisualAsWindow::GetScreen(void)noexcept
{
	return nullptr;
}
//---------------------------------------------------------------------------
iUIWindow* cWPFVisualAsWindow::GetParent(void)noexcept
{
	return nullptr;
}
//---------------------------------------------------------------------------
bool cWPFVisualAsWindow::InsertWindowHandler(iUIWindowHandler *Handler,sfInt16 Order)noexcept
{
	return false;
}
//---------------------------------------------------------------------------
bool cWPFVisualAsWindow::RemoveWindowHandler(iUIWindowHandler *Handler)noexcept
{
	return false;
}
//---------------------------------------------------------------------------
iUIView* cWPFVisualAsWindow::GetClient(void)noexcept
{
	return nullptr;
}
//---------------------------------------------------------------------------
bool cWPFVisualAsWindow::SetClient(iUIView*)noexcept
{
	return false;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void* cnWin::rcWPFVisualRootAsWindow_MakeWindowInterface(mcWPFVisual &&TargetVisual)noexcept
{
	auto Window=iCreate<cWPFVisualAsWindow>(cnVar::MoveCast(TargetVisual));
	return iExtract(Window,'wpfr');
}
//---------------------------------------------------------------------------
void cnWin::rcWPFVisualRootAsWindow_DropWindowInterface(void *Window)noexcept
{
	iTake(static_cast<cWPFVisualAsWindow*>(Window),'wpfr');
}
//---------------------------------------------------------------------------
iUIWindow* cnWin::rcWPFVisualRootAsWindow_GetUIWindow(void *Window)noexcept
{
	return static_cast<cWPFVisualAsWindow*>(Window);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void mcUIElementAsWPFViewParent::WPFUpdateVisible(bool IsVisible)noexcept
{
	bool Changed=false;
	if(IsVisible){
		if(fViewState!=UIState::Active){
			Changed=true;
			fViewState=UIState::Active;
		}
	}
	else{
		if(fViewState!=UIState::Null){
			Changed=true;
			fViewState=UIState::Null;
		}
	}
	if(Changed){
		for(auto c : fChildSet){
			c->WPFChildTreeNotifyState();
		}
	}
}
//---------------------------------------------------------------------------
uIntn mcUIElementAsWPFViewParent::GetChildCount(void)const noexcept
{
	return fChildSet.GetCount();
}
//---------------------------------------------------------------------------
bool mcUIElementAsWPFViewParent::WPFChildAttach(iWPFViewChild *Child)noexcept
{
	return fChildSet.Insert(Child);
}
//---------------------------------------------------------------------------
bool mcUIElementAsWPFViewParent::WPFChildDetach(iWPFViewChild *Child)noexcept
{
	return fChildSet.Remove(Child);
}
//---------------------------------------------------------------------------
void mcUIElementAsWPFViewParent::WPFNotifyWindowChange(iUIWindow *Window)noexcept
{
}
//---------------------------------------------------------------------------
eUIState mcUIElementAsWPFViewParent::WPFParentGetState(void)noexcept
{
	return fViewState;
}
//---------------------------------------------------------------------------
Float32 mcUIElementAsWPFViewParent::WPFParentGetContentScale(void)noexcept
{
	return 1.f;
}
//---------------------------------------------------------------------------
Float32 mcUIElementAsWPFViewParent::WPFParentGetLayoutScale(void)noexcept
{
	return 1.f;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void mcWPFView::UpdateSubviewZPosition(iWPFViewChild *Subview)noexcept
{
	auto SubviewPair=fSubviewMap.GetPair(Subview);
	if(SubviewPair==fSubviewMap.end())
		return;

	// update item
	fSubviewZOrderSet.Remove(SubviewPair);

	SubviewPair->Value.ZPosition=Subview->WPFChildGetZPosition();
	fSubviewZOrderSet.Insert(SubviewPair);
}
//---------------------------------------------------------------------------
void mcWPFView::WPFNotifyVisible(bool Visible)noexcept
{
	if(fStateVisible==Visible)
		return;
	fStateVisible=Visible;
	
	WPFChildTreeNotifyState();
}
//---------------------------------------------------------------------------
void mcWPFView::WPFUIViewUpdateContentSize(Float32 Scale,Float32 Width,Float32 Height)noexcept
{
	auto UIView=static_cast<cWPFUIView*>(this);

	for(auto *Content : UIView->fContents){
		Content->WPFSetSize(fLayoutScale,Width,Height);
	}
}
//---------------------------------------------------------------------------
void mcWPFView::WPFUIViewNotifyArrange(bool Moved,bool Sized)noexcept
{
	auto UIView=static_cast<cWPFUIView*>(this);
	
	UIView->fViewNotification.RectangleChanged(Moved,Sized);

	if(fLayoutInProgress){
		return;
	}
	fLayoutInProgress=true;

	UIView->fViewNotification.Layout();

	fLayoutInProgress=false;
}
//---------------------------------------------------------------------------
void mcWPFView::WPFUIViewOnIsKeyboardFocusedChanged(bool Focused)noexcept
{
	auto UIView=static_cast<cWPFUIView*>(this);
	cWPFKeyEventByMessage KeyEvent(UIView);

	if(Focused){
		UIView->WPFNotifyFocusEnter(&KeyEvent);
	}
	else{
		UIView->WPFNotifyFocusLeave(&KeyEvent);
	}
}
//---------------------------------------------------------------------------
void mcWPFView::WPFUIViewOnIsKeyboardFocusWithinChanged(bool Focused)noexcept
{
	auto UIView=static_cast<cWPFUIView*>(this);
	cWPFKeyEventByMessage KeyEvent(UIView);

	//auto Msg=cnRTL::CreateStringFormat(L"%p WPFOnIsKeyboardFocusWithinChanged %d\n",KeyEvent->GetSourceView(),Focused);
	//OutputDebugStringW(Msg);
	if(Focused){
		UIView->WPFNotifyFocusEnterRange(&KeyEvent);
	}
	else{
		UIView->WPFNotifyFocusLeave(&KeyEvent);

		UIView->WPFNotifyFocusLeaveRange(&KeyEvent);
	}
}
//---------------------------------------------------------------------------
void mcWPFView::WPFUIViewOnKeyDownFilter(mcWPFKeyEventArgs &KeyEventArgs,eKeyCode Key,bool Repeat)noexcept
{
	auto UIView=static_cast<cWPFUIView*>(this);
	cWPFKeyEvent KeyEvent(UIView,KeyEventArgs);
	if(Repeat==false){
		UIView->fKeyNotification.FilterKeyDown(&KeyEvent,Key);
	}
	else{
		UIView->fKeyNotification.FilterKeyInput(&KeyEvent,Key);
	}
}
//---------------------------------------------------------------------------
void mcWPFView::WPFUIViewOnKeyDown(mcWPFKeyEventArgs &KeyEventArgs,eKeyCode Key,bool Repeat)noexcept
{
	auto UIView=static_cast<cWPFUIView*>(this);
	cWPFKeyEvent KeyEvent(UIView,KeyEventArgs);
	
	if(Repeat==false){
		
		UIView->fKeyNotification.HandleKeyDown(&KeyEvent,Key);

		// trigger one extea Key Input route event
		UIView->ParentNotifyKeyboardKeyInput(&KeyEvent,Key);
	}
	
	UIView->fKeyNotification.HandleKeyInput(&KeyEvent,Key);
}
//---------------------------------------------------------------------------
void mcWPFView::WPFUIViewOnKeyUp(mcWPFKeyEventArgs &KeyEventArgs,eKeyCode Key)noexcept
{
	auto UIView=static_cast<cWPFUIView*>(this);
	cWPFKeyEvent KeyEvent(UIView,KeyEventArgs);

	UIView->fKeyNotification.HandleKeyUp(&KeyEvent,Key);
}
//---------------------------------------------------------------------------
void mcWPFView::WPFUIViewOnKeyUpFilter(mcWPFKeyEventArgs &KeyEventArgs,eKeyCode Key)noexcept
{
	auto UIView=static_cast<cWPFUIView*>(this);
	cWPFKeyEvent KeyEvent(UIView,KeyEventArgs);

	UIView->fKeyNotification.FilterKeyUp(&KeyEvent,Key);
}
//---------------------------------------------------------------------------
void mcWPFView::WPFUIViewOnIsMouseDirectlyOverChanged(bool InRange)noexcept
{
	auto UIView=static_cast<cWPFUIView*>(this);
	cWPFMouseEventByMessage MouseEvent(UIView);

	if(InRange){
		UIView->WPFNotifyMouseIsDirectlyEnter(&MouseEvent);
	}
	else{
		UIView->WPFNotifyMouseIsDirectlyLeave(&MouseEvent);
	}
}
//---------------------------------------------------------------------------
void mcWPFView::WPFUIViewOnMouseEnter(mcWPFMouseEventArgs &MouseEventArgs)noexcept
{
	auto UIView=static_cast<cWPFUIView*>(this);
	cWPFMouseEvent MouseEvent(UIView,MouseEventArgs);

	UIView->WPFNotifyMouseEnterRange(&MouseEvent);

}
//---------------------------------------------------------------------------
void mcWPFView::WPFUIViewOnMouseLeave(mcWPFMouseEventArgs &MouseEventArgs)noexcept
{
	auto UIView=static_cast<cWPFUIView*>(this);
	cWPFMouseEvent MouseEvent(UIView,MouseEventArgs);

	UIView->WPFNotifyMouseLeave(&MouseEvent);
}
//---------------------------------------------------------------------------
void mcWPFView::WPFUIViewOnMouseMove(mcWPFMouseEventArgs &MouseEventArgs)noexcept
{
	auto UIView=static_cast<cWPFUIView*>(this);
	cWPFMouseEvent MouseEvent(UIView,MouseEventArgs);

	UIView->fMouseNotification.HandleMove(&MouseEvent);
}
//---------------------------------------------------------------------------
void mcWPFView::WPFUIViewOnMouseMoveFilter(mcWPFMouseEventArgs &MouseEventArgs)noexcept
{
	auto UIView=static_cast<cWPFUIView*>(this);
	cWPFMouseEvent MouseEvent(UIView,MouseEventArgs);

	UIView->fMouseNotification.FilterMove(&MouseEvent);
}
//---------------------------------------------------------------------------
void mcWPFView::WPFUIViewOnMouseDown(mcWPFMouseEventArgs &MouseEventArgs,eMouseButton Button)noexcept
{
	auto UIView=static_cast<cWPFUIView*>(this);
	cWPFMouseEvent MouseEvent(UIView,MouseEventArgs);

	UIView->fMouseNotification.HandleDown(&MouseEvent,Button);
}
//---------------------------------------------------------------------------
void mcWPFView::WPFUIViewOnMouseDownFilter(mcWPFMouseEventArgs &MouseEventArgs,eMouseButton Button)noexcept
{
	auto UIView=static_cast<cWPFUIView*>(this);
	cWPFMouseEvent MouseEvent(UIView,MouseEventArgs);

	UIView->fMouseNotification.FilterDown(&MouseEvent,Button);
}
//---------------------------------------------------------------------------
void mcWPFView::WPFUIViewOnMouseUp(mcWPFMouseEventArgs &MouseEventArgs,eMouseButton Button)noexcept
{
	auto UIView=static_cast<cWPFUIView*>(this);
	cWPFMouseEvent MouseEvent(UIView,MouseEventArgs);

	UIView->fMouseNotification.HandleUp(&MouseEvent,Button);
}
//---------------------------------------------------------------------------
void mcWPFView::WPFUIViewOnMouseUpFilter(mcWPFMouseEventArgs &MouseEventArgs,eMouseButton Button)noexcept
{
	auto UIView=static_cast<cWPFUIView*>(this);
	cWPFMouseEvent MouseEvent(UIView,MouseEventArgs);

	UIView->fMouseNotification.FilterUp(&MouseEvent,Button);
}
//---------------------------------------------------------------------------
void mcWPFView::WPFUIViewOnMouseWheel(mcWPFMouseEventArgs &MouseEventArgs,Float32 ScrollX,Float32 ScrollY)noexcept
{
	auto UIView=static_cast<cWPFUIView*>(this);
	cWPFMouseEvent MouseEvent(UIView,MouseEventArgs);

	UIView->fMouseNotification.HandleWheel(&MouseEvent,ScrollX,ScrollY);
}
//---------------------------------------------------------------------------
void mcWPFView::WPFUIViewOnMouseWheelFilter(mcWPFMouseEventArgs &MouseEventArgs,Float32 ScrollX,Float32 ScrollY)noexcept
{
	auto UIView=static_cast<cWPFUIView*>(this);
	cWPFMouseEvent MouseEvent(UIView,MouseEventArgs);

	UIView->fMouseNotification.FilterWheel(&MouseEvent,ScrollX,ScrollY);
}
//---------------------------------------------------------------------------
void mcWPFView::WPFUIViewOnTouchDown(mcWPFTouchEventArgs &TouchEventArgs)noexcept
{
	auto UIView=static_cast<cWPFUIView*>(this);
	cWPFTouchEvent TouchEvent(UIView,TouchEventArgs);

	UIView->fTouchNotification.HandleBegin(&TouchEvent);
}
//---------------------------------------------------------------------------
void mcWPFView::WPFUIViewOnTouchDownFilter(mcWPFTouchEventArgs &TouchEventArgs)noexcept
{
	auto UIView=static_cast<cWPFUIView*>(this);
	cWPFTouchEvent TouchEvent(UIView,TouchEventArgs);

	UIView->fTouchNotification.FilterBegin(&TouchEvent);
}
//---------------------------------------------------------------------------
void mcWPFView::WPFUIViewOnTouchUp(mcWPFTouchEventArgs &TouchEventArgs)noexcept
{
	auto UIView=static_cast<cWPFUIView*>(this);
	cWPFTouchEvent TouchEvent(UIView,TouchEventArgs);

	UIView->fTouchNotification.HandleEnd(&TouchEvent);
}
//---------------------------------------------------------------------------
void mcWPFView::WPFUIViewOnTouchUpFilter(mcWPFTouchEventArgs &TouchEventArgs)noexcept
{
	auto UIView=static_cast<cWPFUIView*>(this);
	cWPFTouchEvent TouchEvent(UIView,TouchEventArgs);

	UIView->fTouchNotification.FilterEnd(&TouchEvent);
}
//---------------------------------------------------------------------------
void mcWPFView::WPFUIViewOnTouchMove(mcWPFTouchEventArgs &TouchEventArgs)noexcept
{
	auto UIView=static_cast<cWPFUIView*>(this);
	cWPFTouchEvent TouchEvent(UIView,TouchEventArgs);

	UIView->fTouchNotification.HandleMove(&TouchEvent);
}
//---------------------------------------------------------------------------
void mcWPFView::WPFUIViewOnTouchMoveFilter(mcWPFTouchEventArgs &TouchEventArgs)noexcept
{
	auto UIView=static_cast<cWPFUIView*>(this);
	cWPFTouchEvent TouchEvent(UIView,TouchEventArgs);

	UIView->fTouchNotification.FilterMove(&TouchEvent);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
sfInt8 cWPFUIView::cSubviewItemZOrderOperator::Compare(const tSubviewNode *Item,const tSubviewNode *Value)noexcept
{
	if(Item->Value.ZPosition<Value->Value.ZPosition)
		return -1;
	if(Item->Value.ZPosition>Value->Value.ZPosition)
		return 1;
	if(Item->Key<Value->Key)
		return -1;
	if(Item->Key>Value->Key)
		return 1;
	return 0;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcWPFViewContentDrawing::bcWPFViewContentDrawing()noexcept
{
	fView=nullptr;
	fZPosition=0;
}
//---------------------------------------------------------------------------
bcWPFViewContentDrawing::~bcWPFViewContentDrawing()noexcept
{
}
//---------------------------------------------------------------------------
cWPFUIView* bcWPFViewContentDrawing::GetView(void)noexcept
{
	return fView;
}
//---------------------------------------------------------------------------
bool bcWPFViewContentDrawing::SetView(iUIView *View)noexcept
{
	cWPFUIView *wpfView;
	if(View==nullptr){
		wpfView=nullptr;
	}
	else{
		wpfView=iCast<cWPFUIView>(View);
		if(wpfView==nullptr){
			return false;
		}
	}

	if(fView!=nullptr){
		fView->RemoveViewContent(this);
	}
	fView=wpfView;
	if(fView!=nullptr){
		fView->InsertViewContent(this);
	}
	
	ViewContentUpdateState();
	return true;
}
//---------------------------------------------------------------------------
bool bcWPFViewContentDrawing::GetVisible(void)noexcept
{
	return true;
}
//---------------------------------------------------------------------------
bool bcWPFViewContentDrawing::SetVisible(bool Visible)noexcept
{
	return false;
}
//---------------------------------------------------------------------------
Float32 bcWPFViewContentDrawing::GetZPosition(void)noexcept
{
	return fZPosition;
}
//---------------------------------------------------------------------------
bool bcWPFViewContentDrawing::SetZPosition(Float32 ZPosition)noexcept
{
	fZPosition=ZPosition;
	if(fView!=nullptr){
		fView->UpdateViewContentZPosition();
	}
	return true;
}
//---------------------------------------------------------------------------
Float32 bcWPFViewContentDrawing::GetContentScale(void)noexcept
{
	return fView->GetViewContentContentScale();
}
//---------------------------------------------------------------------------
void bcWPFViewContentDrawing::UpdateDrawing(void)noexcept
{
	if(fView!=nullptr){
		fView->UpdateViewContent();
	}
}
//---------------------------------------------------------------------------
eUIState bcWPFViewContentDrawing::CheckViewContentState(void)noexcept
{
	if(fView!=nullptr){
		return fView->GetUIState();
	}
	return UIState::Null;
}
//---------------------------------------------------------------------------
void bcWPFViewContentDrawing::WPFSetSize(Float32 LayoutScale,Float32 Width,Float32 Height)noexcept
{
	fDrawingWidth=Width;
	fDrawingHeight=Height;
	fLayoutScale=LayoutScale;
	ViewContentUpdateRect();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWPFUIView::cWPFUIView(cDNetUIThread *UIThread,mcConstructParameter &Parameter)noexcept
	: mcWPFView(Parameter)
	, fUIThread(UIThread)
	, fViewState(UIState::Null)
	, fNeedLayout(false)
{
}
//---------------------------------------------------------------------------
cWPFUIView::~cWPFUIView()noexcept
{
}
//---------------------------------------------------------------------------
void* cWPFUIView::CastInterface(iTypeID ID)noexcept
{
	if(ID==TInterfaceID<cWPFUIView>::Value){
		return this;
	}
	return ImpCastInterface<iCLIObject,iUIView,iWPFViewChild,iUIKeyControl,iUIMouseControl,iUITouchControl>(this,ID);
}
//---------------------------------------------------------------------------
void cWPFUIView::VirtualStarted(void)noexcept
{
	fUIThread->mAttachDispatcher(&fDispatcherFinishNotify);
}
//---------------------------------------------------------------------------
void cWPFUIView::VirtualStopped(void)noexcept
{
	if(fUIThread->mDetachDispatcher(&fDispatcherFinishNotify)){
		// wait for notification
		return;
	}
	// dispatcher already shutdown
	VirtualDelete();
}
//---------------------------------------------------------------------------
void mcWPFView::nDispatchFinishNotify(bool Shutdown)noexcept
{
	if(Shutdown==false){
		// called by VirtualStopped
		auto UIView=static_cast<cWPFUIView*>(this);
		UIView->VirtualDelete();
	}
}
//---------------------------------------------------------------------------
cGCRef& cWPFUIView::GetObjecHandle(void)noexcept
{
	return fWPFView;
}
//---------------------------------------------------------------------------
bool cWPFUIView::CheckThread(void)noexcept
{
	return fUIThread->IsCurrent();
}
//---------------------------------------------------------------------------
iUIThread* cWPFUIView::GetUIThread(void)noexcept
{
	return fUIThread;
}
//---------------------------------------------------------------------------
Float32 cWPFUIView::GetContentScale(void)noexcept
{
	return fContentScale;
}
//---------------------------------------------------------------------------
cUIPoint cWPFUIView::GetSize(void)noexcept
{
	cUIPoint Size;
	Size.x=fRectWPF.Size.x*fLayoutScale;
	Size.y=fRectWPF.Size.y*fLayoutScale;
	return Size;
}
//---------------------------------------------------------------------------
bool cWPFUIView::SetSize(cUIPoint Size)noexcept
{
	if(CheckThread()==false)
		return false;

	if(Size.x<0){
		Size.x=0;
	}
	else{
		Size.x=Size.x/fLayoutScale;
	}
	if(Size.y<0){
		Size.y=0;
	}
	else{
		Size.y=Size.y/fLayoutScale;
	}

	Size.x=cnMath::FloatRoundNearest(Size.x);
	Size.y=cnMath::FloatRoundNearest(Size.y);
	return mSetSize(Size);
}
//---------------------------------------------------------------------------
bool cWPFUIView::MoveTo(iInterface *Relative,cUIPoint Position)noexcept
{
	if(CheckThread()==false)
		return false;

	auto RelativeObject=iCast<iCLIObject>(Relative);
	if(RelativeObject==nullptr)
		return false;

	auto &Handle=RelativeObject->GetObjecHandle();
	return mSetPosition(Handle,Position);
}
//---------------------------------------------------------------------------
bool cWPFUIView::TranslatePointTo(iInterface *Relative,cUIPoint &Position)noexcept
{
	if(CheckThread()==false)
		return false;

	auto RelativeObject=iCast<iCLIObject>(Relative);
	if(RelativeObject==nullptr)
		return false;

	auto &Handle=RelativeObject->GetObjecHandle();
	return mTranslatePointTo(Handle,Position);
}
//---------------------------------------------------------------------------
bool cWPFUIView::ArrangeRectangle(iInterface *Relative,cUIPoint Position,cUIPoint Size)noexcept
{
	if(CheckThread()==false)
		return false;

	auto RelativeObject=iCast<iCLIObject>(Relative);
	if(RelativeObject==nullptr)
		return false;

	auto &Handle=RelativeObject->GetObjecHandle();
	return mSetRectangle(Handle,Position,Size);
}
//---------------------------------------------------------------------------
Float32 cWPFUIView::GetZPosition(void)noexcept
{
	return fZPosition;
}
//---------------------------------------------------------------------------
bool cWPFUIView::SetZPosition(Float32 ZPosition)noexcept
{
	if(CheckThread()==false)
		return false;

	if(fZPosition==ZPosition)
		return true;


	fZPosition=ZPosition;
	if(fParentView!=nullptr){
		fParentView->UpdateSubviewZPosition(this);
	}
	return true;
}
//---------------------------------------------------------------------------
eUIState cWPFUIView::GetUIState(void)noexcept
{
	return fViewState;
}
//---------------------------------------------------------------------------
bool cWPFUIView::InsertStateHandler(iUIStateHandler *Handler,sfInt16 Order)noexcept
{
	if(CheckThread()==false)
		return false;
	fViewNotification.InsertStateHandler(Handler,Order);
	return true;
}
//---------------------------------------------------------------------------
bool cWPFUIView::RemoveStateHandler(iUIStateHandler *Handler)noexcept
{
	if(CheckThread()==false)
		return false;
	fViewNotification.RemoveStateHandler(Handler);
	return true;
}
//---------------------------------------------------------------------------
bool cWPFUIView::GetVisible(void)noexcept
{
	return mGetVisible();
}
//---------------------------------------------------------------------------
bool cWPFUIView::SetVisible(bool Visible)noexcept
{
	return mSetVisible(Visible);
}
//---------------------------------------------------------------------------
bool cWPFUIView::IsEnabled(void)noexcept
{
	return true;
}
//---------------------------------------------------------------------------
bool cWPFUIView::GetEnable(void)noexcept
{
	return true;
}
//---------------------------------------------------------------------------
bool cWPFUIView::SetEnable(bool Enable)noexcept
{
	return false;
}
//---------------------------------------------------------------------------
bool cWPFUIView::InsertViewHandler(iUIViewHandler *Handler,sfInt16 Order)noexcept
{
	if(CheckThread()==false)
		return false;
	fViewNotification.InsertViewHandler(Handler,Order);
	return true;
}
//---------------------------------------------------------------------------
bool cWPFUIView::RemoveViewHandler(iUIViewHandler *Handler)noexcept
{
	if(CheckThread()==false)
		return false;
	fViewNotification.RemoveViewHandler(Handler);
	return true;
}
//---------------------------------------------------------------------------
void cWPFUIView::SetArrangement(void)noexcept
{
	if(CheckThread()==false)
		return;

	mSetArrangement();
}
//---------------------------------------------------------------------------
void cWPFUIView::ArrangeLayout(void)noexcept
{
	if(CheckThread()==false)
		return;

	mArrangeLayout();
}
//---------------------------------------------------------------------------
void cWPFUIView::SetIncludeParentFrameMargin(bool Include)noexcept
{
}
//---------------------------------------------------------------------------
cUIRectangle cWPFUIView::GetFrameMargin(void)noexcept
{
	return UIRectangleZero;
}
//---------------------------------------------------------------------------
iUIWindow* cWPFUIView::GetWindow(void)noexcept
{
	return fWindow;
}
//---------------------------------------------------------------------------
iUIView* cWPFUIView::GetParent(void)noexcept
{
	return static_cast<cWPFUIView*>(fParentView);
}
//---------------------------------------------------------------------------
eUIState cWPFUIView::EvaluateViewState(void)noexcept
{
	if(fParent==nullptr){
		return UIState::Null;
	}
	eUIState State=fParent->WPFParentGetState();
	if(fStateVisible==false || fRectWPF.Size.x==0 || fRectWPF.Size.y==0){
		if(State>UIState::Background)
			State=UIState::Background;
	}
	return State;
}
//---------------------------------------------------------------------------
bool cWPFUIView::UpdateViewState(void)noexcept
{
	eUIState NewState=EvaluateViewState();
	if(fViewState==NewState)
		return false;

	eUIState OldState=fViewState;
	fViewState=NewState;
	fViewNotification.StateChange(OldState,NewState);

	// notify view contents
	for(auto *Content : fContents){
		Content->ViewContentUpdateState();
	}

	return true;
}
//---------------------------------------------------------------------------
void cWPFUIView::UpdateWindow(iUIWindow *Window)noexcept
{
	if(fWindow==Window){
		// update view state only
		UpdateViewState();
		return;
	}

	fWindow=Window;

	eUIState NewState=EvaluateViewState();

	if(NewState>fViewState){
		NotifyWindowChanged();
		eUIState OldState=fViewState;
		fViewState=NewState;
		fViewNotification.StateChange(OldState,NewState);
	}
	else if(NewState<fViewState){
		eUIState OldState=fViewState;
		fViewState=NewState;
		fViewNotification.StateChange(OldState,NewState);
		NotifyWindowChanged();
	}
	else{
		NotifyWindowChanged();
	}

}
//---------------------------------------------------------------------------
void cWPFUIView::NotifyWindowChanged(void)noexcept
{
	fViewNotification.WindowChanged();

	// notify view contents
	for(auto *Content : fContents){
		iWindow *Window=nullptr;
		auto Component=iCast<iWindowClient>(fWindow);
		if(Component!=nullptr){
			Window=Component->GetWindow();
		}
		Content->ViewContentUpdateWindow(Window);
	}
}
//---------------------------------------------------------------------------
bool cWPFUIView::InsertView(iUIView *View)noexcept
{
	if(CheckThread()==false)
		return false;
	if(View==nullptr)
		return false;
	auto Subview=iCast<iWPFViewChild>(View);
	if(Subview==nullptr)
		return false;

	auto &Handle=Subview->WPFChildGetElementHandle();
	auto ZPosition=Subview->WPFChildGetZPosition();
	if(mInsertView(Subview,Handle,ZPosition)==false){
		return false;
	}
	
	Subview->WPFChildTreeNotifyWindow(fWindow);

	return true;
}
//---------------------------------------------------------------------------
bool cWPFUIView::RemoveView(iUIView *View)noexcept
{
	if(CheckThread()==false)
		return false;
	if(View==nullptr)
		return false;
	auto Subview=iCast<iWPFViewChild>(View);
	if(Subview==nullptr)
		return false;

	auto &Handle=Subview->WPFChildGetElementHandle();
	return mRemoveView(Subview,Handle);
}
//---------------------------------------------------------------------------
bool cWPFUIView::InsertViewContent(bcWPFViewContentDrawing *Content)noexcept
{
	if(fContents.Insert(Content)){
		UpdateViewContentZPosition();
	}
	return true;
}
//---------------------------------------------------------------------------
bool cWPFUIView::RemoveViewContent(bcWPFViewContentDrawing *Content)noexcept
{
	if(fContents.Remove(Content)){
		UpdateViewContentZPosition();
	}
	return true;
}
//---------------------------------------------------------------------------
void cWPFUIView::UpdateViewContentZPosition(void)noexcept
{
	ClearContentOrderCache();
	SetupContentOrderCache();
	ApplyContentChanged();
}
//---------------------------------------------------------------------------
void cWPFUIView::UpdateViewContent(void)noexcept
{
	ApplyContentChanged();
}
//---------------------------------------------------------------------------
Float32 cWPFUIView::GetViewContentContentScale(void)noexcept
{
	return fContentScale;
}
//---------------------------------------------------------------------------
void cWPFUIView::SetupContentOrderCache(void)noexcept
{
	if(fContentOrderCacheUpdate==false){
		return;
	}
	
	cSeqMap<Float32,cSeqList<bcWPFViewContentDrawing*> > ZOrderMap;

	fContentOrderCache.Clear();
	for(auto *Content : fContents){
		Float32 ContentZPosition=Content->GetZPosition();
		auto Pair=ZOrderMap.InsertPair(ContentZPosition);
		Pair->Value.AppendMake(Content);
	}
	for(auto Pair : ZOrderMap){
		for(auto Content : Pair.Value){
			fContentOrderCache.AppendMake(Content->GetDrawingPointer());
		}
	}

	fContentOrderCacheUpdate=false;
}
//---------------------------------------------------------------------------
void cWPFUIView::ClearContentOrderCache(void)noexcept
{
	fContentOrderCache.Clear();
	fContentOrderCacheUpdate=true;
}
//---------------------------------------------------------------------------
void cWPFUIView::ApplyContentChanged(void)noexcept
{
	if(fUIThread->IsShutdown())
		return;
	auto *Drawings=fContentOrderCache.GetItems();
	uIntn Count=fContentOrderCache.GetCount();

	mRenderContent(Drawings,Count);
}
//---------------------------------------------------------------------------
iUIView* cWPFUIView::GetFocus(void)noexcept
{
	return nullptr;
}
//---------------------------------------------------------------------------
void cWPFUIView::SetFocus(bool ActivateWindow)noexcept
{
	return mSetFocus(ActivateWindow);
}
//---------------------------------------------------------------------------
bool cWPFUIView::InsertKeyHandler(iUIKeyHandler *Handler,sfInt16 Order)noexcept
{
	if(CheckThread()==false)
		return false;
	fKeyNotification.InsertKeyHandler(Handler,Order);
	return true;
}
//---------------------------------------------------------------------------
bool cWPFUIView::RemoveKeyHandler(iUIKeyHandler *Handler)noexcept
{
	if(CheckThread()==false)
		return false;
	fKeyNotification.RemoveKeyHandler(Handler);
	return true;
}
//---------------------------------------------------------------------------
bool cWPFUIView::InsertKeyFilter(iUIKeyHandler *Filter,sfInt16 Order)noexcept
{
	if(CheckThread()==false)
		return false;
	fKeyNotification.InsertKeyFilter(Filter,Order);
	return true;
}
//---------------------------------------------------------------------------
bool cWPFUIView::RemoveKeyFilter(iUIKeyHandler *Filter)noexcept
{
	if(CheckThread()==false)
		return false;
	fKeyNotification.RemoveKeyFilter(Filter);
	return true;
}
//---------------------------------------------------------------------------
bool cWPFUIView::KeyAcquireExclusive(iUIKeyHandler *Handler)noexcept
{
	if(CheckThread()==false)
		return false;
	return fKeyNotification.KeyAcquireExclusive(Handler);
}
//---------------------------------------------------------------------------
bool cWPFUIView::KeyReleaseExclusive(iUIKeyHandler *Handler)noexcept
{
	if(CheckThread()==false)
		return false;
	fKeyNotification.KeyReleaseExclusive(Handler);
	return true;
}
//---------------------------------------------------------------------------
bool cWPFUIView::InsertTextInputHandler(iUITextInputHandler *Handler,sfInt16 Order)noexcept
{
	if(CheckThread()==false)
		return false;
	fKeyNotification.InsertTextInputHandler(Handler,Order);
	return true;
}
//---------------------------------------------------------------------------
bool cWPFUIView::RemoveTextInputHandler(iUITextInputHandler *Handler)noexcept
{
	if(CheckThread()==false)
		return false;
	fKeyNotification.RemoveTextInputHandler(Handler);
	return true;
}
//---------------------------------------------------------------------------
bool cWPFUIView::SetCompositionViewInfo(iUITextCompositionViewInfo *CompositionViewInfo)noexcept
{
	return false;
}
//---------------------------------------------------------------------------
void cWPFUIView::MouseUpdateCursor(void)noexcept
{
}
//---------------------------------------------------------------------------
bool cWPFUIView::InsertMouseHandler(iUIMouseHandler *Handler,sfInt16 Order)noexcept
{
	if(CheckThread()==false)
		return false;
	fMouseNotification.InsertMouseHandler(Handler,Order);
	return true;
}
//---------------------------------------------------------------------------
bool cWPFUIView::RemoveMouseHandler(iUIMouseHandler *Handler)noexcept
{
	if(CheckThread()==false)
		return false;
	fMouseNotification.RemoveMouseHandler(Handler);
	return true;
}
//---------------------------------------------------------------------------
bool cWPFUIView::InsertMouseFilter(iUIMouseHandler *Filter,sfInt16 Order)noexcept
{
	if(CheckThread()==false)
		return false;
	fMouseNotification.InsertMouseFilter(Filter,Order);
	return true;
}
//---------------------------------------------------------------------------
bool cWPFUIView::RemoveMouseFilter(iUIMouseHandler *Filter)noexcept
{
	if(CheckThread()==false)
		return false;
	fMouseNotification.RemoveMouseFilter(Filter);
	return true;
}
//---------------------------------------------------------------------------
bool cWPFUIView::MouseSetCaptrue(void)noexcept
{
	return false;
}
//---------------------------------------------------------------------------
void cWPFUIView::MouseReleaseCaptrue(void)noexcept
{
}
//---------------------------------------------------------------------------
bool cWPFUIView::MouseIsCaptured(void)noexcept
{
	return false;
}
//---------------------------------------------------------------------------
bool cWPFUIView::MouseAcquireExclusive(iUIMouseHandler *Handler)noexcept
{
	if(CheckThread()==false)
		return false;
	return fMouseNotification.MouseAcquireExclusive(Handler);
}
//---------------------------------------------------------------------------
bool cWPFUIView::MouseReleaseExclusive(iUIMouseHandler *Handler)noexcept
{
	if(CheckThread()==false)
		return false;
	fMouseNotification.MouseReleaseExclusive(Handler);
	return true;
}
//---------------------------------------------------------------------------
bool cWPFUIView::InsertTouchHandler(iUITouchHandler *Handler,sfInt16 Order)noexcept
{
	if(CheckThread()==false)
		return false;
	fTouchNotification.InsertTouchHandler(Handler,Order);
	return true;
}
//---------------------------------------------------------------------------
bool cWPFUIView::RemoveTouchHandler(iUITouchHandler *Handler)noexcept
{
	if(CheckThread()==false)
		return false;
	fTouchNotification.RemoveTouchHandler(Handler);
	return true;
}
//---------------------------------------------------------------------------
bool cWPFUIView::InsertTouchFilter(iUITouchHandler *Filter,sfInt16 Order)noexcept
{
	if(CheckThread()==false)
		return false;
	fTouchNotification.InsertTouchFilter(Filter,Order);
	return true;
}
//---------------------------------------------------------------------------
bool cWPFUIView::RemoveTouchFilter(iUITouchHandler *Filter)noexcept
{
	if(CheckThread()==false)
		return false;
	fTouchNotification.RemoveTouchFilter(Filter);
	return true;
}
//---------------------------------------------------------------------------
bool cWPFUIView::TouchAcquireExclusive(iUITouchHandler *Handler)noexcept
{
	if(CheckThread()==false)
		return false;
	return fTouchNotification.TouchAcquireExclusive(Handler);
}
//---------------------------------------------------------------------------
bool cWPFUIView::TouchReleaseExclusive(iUITouchHandler *Handler)noexcept
{
	if(CheckThread()==false)
		return false;
	fTouchNotification.TouchReleaseExclusive(Handler);
	return true;
}
//---------------------------------------------------------------------------
cGCRef& cWPFUIView::WPFChildGetElementHandle(void)noexcept
{
	return fWPFView;
}
//---------------------------------------------------------------------------
eUIState cWPFUIView::WPFParentGetState(void)noexcept
{
	return fViewState;
}
//---------------------------------------------------------------------------
Float32 cWPFUIView::WPFParentGetContentScale(void)noexcept
{
	return fContentScale;
}
//---------------------------------------------------------------------------
Float32 cWPFUIView::WPFParentGetLayoutScale(void)noexcept
{
	return fLayoutScale;
}
//---------------------------------------------------------------------------
Float32 cWPFUIView::WPFChildGetLayoutScale(void)noexcept
{
	return fLayoutScale;
}
//---------------------------------------------------------------------------
Float32 cWPFUIView::WPFChildGetZPosition(void)noexcept
{
	return fZPosition;
}
//---------------------------------------------------------------------------
void cWPFUIView::WPFChildTreeNotifyWindow(iUIWindow *Window)noexcept
{
	UpdateWindow(Window);
	for(auto &Item : fSubviewZOrderSet){
		Item->Key->WPFChildTreeNotifyWindow(Window);
	}
}
//---------------------------------------------------------------------------
void cWPFUIView::WPFChildTreeNotifyState(void)noexcept
{
	if(UpdateViewState()){
		for(auto &Item : fSubviewZOrderSet){
			Item->Key->WPFChildTreeNotifyState();
		}
	}
}
//---------------------------------------------------------------------------
void cWPFUIView::WPFChildTreeNotifyScale(void)noexcept
{
	if(fParent!=nullptr){
		fLayoutScale=fParent->WPFParentGetLayoutScale();
		fContentScale=fParent->WPFParentGetContentScale();
	}
	else{
		fLayoutScale=1.f;
		fContentScale=1.f;
	}

	fViewNotification.ContentScaleChanged();

	for(auto &Item : fSubviewZOrderSet){
		Item->Key->WPFChildTreeNotifyScale();
	}
}
//---------------------------------------------------------------------------
void cWPFUIView::ParentNotifyKeyboardFocusWithin(iWPFViewChild *Subview,iUIKeyEvent *KeyEvent)noexcept
{
	WPFNotifyFocusEnterRange(KeyEvent);
}
//---------------------------------------------------------------------------
void cWPFUIView::ParentNotifyKeyboardFocuseEnter(iUIKeyEvent *KeyEvent)noexcept
{
	if(fParentView!=nullptr){
		static_cast<cWPFUIView*>(fParentView)->ParentNotifyKeyboardFocuseEnter(KeyEvent);
	}

	fKeyNotification.FilterFocusEnter(KeyEvent);
}
//---------------------------------------------------------------------------
void cWPFUIView::ParentNotifyKeyboardFocuseLeave(iUIKeyEvent *KeyEvent)noexcept
{
	if(fParentView!=nullptr){
		static_cast<cWPFUIView*>(fParentView)->ParentNotifyKeyboardFocuseLeave(KeyEvent);
	}

	fKeyNotification.FilterFocusLeave(KeyEvent);
}
//---------------------------------------------------------------------------
void cWPFUIView::ParentNotifyKeyboardKeyInput(iUIKeyEvent *KeyEvent,eKeyCode KeyCode)noexcept
{
	if(fParentView!=nullptr){
		static_cast<cWPFUIView*>(fParentView)->ParentNotifyKeyboardKeyInput(KeyEvent,KeyCode);
	}

	fKeyNotification.FilterKeyInput(KeyEvent,KeyCode);
}
//---------------------------------------------------------------------------
void cWPFUIView::ParentNotifyMouseEnter(iWPFViewChild *Subview,iUIMouseEvent *MouseEvent)noexcept
{
	WPFNotifyMouseEnterRange(MouseEvent);
}
//---------------------------------------------------------------------------
void cWPFUIView::ParentNotifyMouseIsDirectlyEnter(iUIMouseEvent *MouseEvent)noexcept
{
	if(fParentView!=nullptr){
		static_cast<cWPFUIView*>(fParentView)->ParentNotifyMouseIsDirectlyEnter(MouseEvent);
	}
	fMouseNotification.FilterEnter(MouseEvent);
}
//---------------------------------------------------------------------------
void cWPFUIView::ParentNotifyMouseIsDirectlyLeave(iUIMouseEvent *MouseEvent)noexcept
{
	if(fParentView!=nullptr){
		static_cast<cWPFUIView*>(fParentView)->ParentNotifyMouseIsDirectlyLeave(MouseEvent);
	}
	fMouseNotification.FilterLeave(MouseEvent);
}
//---------------------------------------------------------------------------
void cWPFUIView::WPFNotifyFocusEnterRange(iUIKeyEvent *KeyEvent)noexcept
{
	if(fKeyFocusInRangeNotified==false){
		fKeyFocusInRangeNotified=true;

		// notify parent focus enter range
		if(fParentView!=nullptr){
			static_cast<cWPFUIView*>(fParentView)->ParentNotifyKeyboardFocusWithin(this,KeyEvent);
		}

		fKeyNotification.FocusEnterRange(KeyEvent);
	}
}
//---------------------------------------------------------------------------
void cWPFUIView::WPFNotifyFocusLeaveRange(iUIKeyEvent *KeyEvent)noexcept
{
	fKeyFocusInRangeNotified=false;

	fKeyNotification.FocusLeaveRange(KeyEvent);
}
//---------------------------------------------------------------------------
void cWPFUIView::WPFNotifyFocusEnter(iUIKeyEvent *KeyEvent)noexcept
{
	if(fKeyFocusedNotifyValue==false){
		fKeyFocusedNotifyValue=true;

		ParentNotifyKeyboardFocuseEnter(KeyEvent);

		fKeyNotification.HandleFocusEnter(KeyEvent);
	}
}
//---------------------------------------------------------------------------
void cWPFUIView::WPFNotifyFocusLeave(iUIKeyEvent *KeyEvent)noexcept
{
	if(fKeyFocusedNotifyValue){
		fKeyFocusedNotifyValue=false;
			
		ParentNotifyKeyboardFocuseLeave(KeyEvent);

		fKeyNotification.HandleFocusLeave(KeyEvent);
	}
}
//---------------------------------------------------------------------------
void cWPFUIView::WPFNotifyMouseIsDirectlyEnter(iUIMouseEvent *MouseEvent)noexcept
{
	if(fMouseIsOverNotifyValue==false){
		fMouseIsOverNotifyValue=true;

		ParentNotifyMouseIsDirectlyEnter(MouseEvent);

		fMouseNotification.HandleEnter(MouseEvent);
	}
}
//---------------------------------------------------------------------------
void cWPFUIView::WPFNotifyMouseIsDirectlyLeave(iUIMouseEvent *MouseEvent)noexcept
{
	if(fMouseIsOverNotifyValue){
		fMouseIsOverNotifyValue=false;
			
		ParentNotifyMouseIsDirectlyLeave(MouseEvent);

		fMouseNotification.HandleLeave(MouseEvent);
	}
}
//---------------------------------------------------------------------------
void cWPFUIView::WPFNotifyMouseEnterRange(iUIMouseEvent *MouseEvent)noexcept
{
	if(fMouseEnterNotified==false){
		fMouseEnterNotified=true;

		// notify parent mouse enter
		if(fParentView!=nullptr){
			static_cast<cWPFUIView*>(fParentView)->ParentNotifyMouseEnter(this,MouseEvent);
		}

		fMouseNotification.MouseEnterRange(MouseEvent);
	}
}
//---------------------------------------------------------------------------
void cWPFUIView::WPFNotifyMouseLeave(iUIMouseEvent *MouseEvent)noexcept
{
	WPFNotifyMouseIsDirectlyLeave(MouseEvent);

	fMouseEnterNotified=false;
	fMouseNotification.MouseLeaveRange(MouseEvent);
}
//---------------------------------------------------------------------------

#if 0


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void* mcWPFUIElementAsUIView::CastInterface(iTypeID ID)noexcept(true)
{
	return ImpCastInterface<iCLIObject,iUIView,iWPFViewChild>(this,ID);
}
//---------------------------------------------------------------------------
bool mcWPFUIElementAsUIView::CheckThread(void)
{
	return fUIThread->IsCurrent();
}
//---------------------------------------------------------------------------
void mcWPFUIElementAsUIView::UIViewNotifyParent(iWPFViewParent *Parent)
{
	fParent=Parent;
}
//---------------------------------------------------------------------------
iUIThread* mcWPFUIElementAsUIView::GetUIThread(void)
{
	return fUIThread;
}
//---------------------------------------------------------------------------
Float32 mcWPFUIElementAsUIView::GetZPosition(void)
{
	return fZPosition;
}
//---------------------------------------------------------------------------
bool mcWPFUIElementAsUIView::SetZPosition(Float32 ZPosition)
{
	if(fZPosition==ZPosition)
		return true;


	fZPosition=ZPosition;
	if(fParent!=nullptr){
		//fParent->WPFParentUpdateSubviewZPosition(this);
	}
	return true;
}
//---------------------------------------------------------------------------
cUIPoint mcWPFUIElementAsUIView::GetSize(void)
{
	return fRect.Size;
}
//---------------------------------------------------------------------------
bool mcWPFUIElementAsUIView::SetSize(cUIPoint Size)
{
	if(CheckThread()==false)
		return false;

	if(Size.x<0)
		Size.x=0;
	if(Size.y<0)
		Size.y=0;
	fRect.Size=Size;
	mApplyRect();
	return true;
}
//---------------------------------------------------------------------------
bool mcWPFUIElementAsUIView::TranslatePointTo(iInterface *Relative,cUIPoint &Position)
{
	if(CheckThread()==false)
		return false;

	auto RelativeObject=iCast<iCLIObject>(Relative);
	if(RelativeObject==nullptr)
		return false;

	auto &Handle=RelativeObject->GetObjecHandle();
	return mTranslatePointTo(Handle,Position);
}
//---------------------------------------------------------------------------
bool mcWPFUIElementAsUIView::MoveTo(iInterface *Relative,cUIPoint Position)
{
	if(CheckThread()==false)
		return false;

	auto RelativeObject=iCast<iCLIObject>(Relative);
	if(RelativeObject==nullptr)
		return false;

	auto &Handle=RelativeObject->GetObjecHandle();
	return mSetPosition(Handle,Position);
}
//---------------------------------------------------------------------------
bool mcWPFUIElementAsUIView::ArrangeRectangle(iInterface *Relative,cUIPoint Position,cUIPoint Size)
{
	if(CheckThread()==false)
		return false;

	auto RelativeObject=iCast<iCLIObject>(Relative);
	if(RelativeObject==nullptr)
		return false;

	auto &Handle=RelativeObject->GetObjecHandle();
	return mSetRectangle(Handle,Position,Size);
}
//---------------------------------------------------------------------------
Float32 mcWPFUIElementAsUIView::GetContentScale(void)
{
	return 1.f;
}
//---------------------------------------------------------------------------
eUIState mcWPFUIElementAsUIView::WPFParentGetState(void)
{
	return UIState::Active;
}
//---------------------------------------------------------------------------
Float32 mcWPFUIElementAsUIView::WPFParentGetContentScale(void)
{
	return 1.f;
}
//---------------------------------------------------------------------------
Float32 mcWPFUIElementAsUIView::WPFParentGetLayoutScale(void)
{
	return 1.f;
}
//---------------------------------------------------------------------------
Float32 mcWPFUIElementAsUIView::WPFChildGetLayoutScale(void)
{
	return 1.f;
}
//---------------------------------------------------------------------------
Float32 mcWPFUIElementAsUIView::WPFChildGetZPosition(void)
{
	return fZPosition;
}
//---------------------------------------------------------------------------
void mcWPFUIElementAsUIView::WPFChildTreeNotifyWindow(iUIWindow *Window)
{
	fWindow=Window;
}
//---------------------------------------------------------------------------
void mcWPFUIElementAsUIView::WPFChildTreeNotifyState(void)
{
}
//---------------------------------------------------------------------------
void mcWPFUIElementAsUIView::WPFChildTreeNotifyScale(void)
{
}
//---------------------------------------------------------------------------
eUIState mcWPFUIElementAsUIView::GetUIState(void)
{
	if(fParent==nullptr)
		return UIState::Null;
	return fParent->WPFParentGetState();
}
//---------------------------------------------------------------------------
bool mcWPFUIElementAsUIView::InsertStateHandler(iUIStateHandler *Handler,sfInt16 Order)
{
	return false;
}
//---------------------------------------------------------------------------
bool mcWPFUIElementAsUIView::RemoveStateHandler(iUIStateHandler *Handler)
{
	return false;
}
//---------------------------------------------------------------------------
bool mcWPFUIElementAsUIView::InsertViewHandler(iUIViewHandler *Handler,sfInt16 Order)
{
	return false;
}
//---------------------------------------------------------------------------
bool mcWPFUIElementAsUIView::RemoveViewHandler(iUIViewHandler *Handler)
{
	return false;
}
//---------------------------------------------------------------------------
void mcWPFUIElementAsUIView::SetArrangement(void)
{
}
//---------------------------------------------------------------------------
void mcWPFUIElementAsUIView::ArrangeLayout(void)
{
}
//---------------------------------------------------------------------------
void mcWPFUIElementAsUIView::SetIncludeParentFrameMargin(bool Include)
{
}
//---------------------------------------------------------------------------
cUIRectangle mcWPFUIElementAsUIView::GetFrameMargin(void)
{
	return UIRectangleZero;
}
//---------------------------------------------------------------------------
iUIWindow* mcWPFUIElementAsUIView::GetWindow(void)
{
	if(fParent==nullptr)
		return nullptr;
	return fWindow;
}
//---------------------------------------------------------------------------
iUIView* mcWPFUIElementAsUIView::GetParent(void)
{
	if(fParent==nullptr)
		return nullptr;
	//return fParent->WPFParentGetUIView();
	return nullptr;
}
//---------------------------------------------------------------------------
bool mcWPFUIElementAsUIView::InsertView(iUIView*)
{
	return false;
}
//---------------------------------------------------------------------------
bool mcWPFUIElementAsUIView::RemoveView(iUIView*)
{
	return false;
}
//---------------------------------------------------------------------------
#endif // 0
