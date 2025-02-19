#include "WinCLI_WPFUIView.h"

using namespace cnLibrary;

using namespace cnRTL;
using namespace cnWin;

//---------------------------------------------------------------------------
iPtr<iUIView> cnWin::WPFCreateUIView(void)noexcept(true)
{
	auto UIThread=cWPFUIThread::CurrentUIThread();
	if(UIThread==nullptr)
		return nullptr;
	
	cGCRef WPFViewHandle;
	mMakeWPFView(WPFViewHandle);
	return iCreate<cWPFUIView>(UIThread,WPFViewHandle);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
eUIEventSourceType bcWPFKeyEvent::GetSourceType(void)noexcept(true)
{
	return UIEventSourceType::Keyboard;
}
//---------------------------------------------------------------------------
iUIView* bcWPFKeyEvent::GetSourceView(void)noexcept(true)
{
	return nullptr;
}
//---------------------------------------------------------------------------
iUIKeyboard* bcWPFKeyEvent::GetKeyboard(void)noexcept(true)
{
	auto Thread=cWPFUIThread::CurrentUIThread();
	if(Thread==nullptr)
		return nullptr;
	return Thread->GetDefaultKeyboard();
}
//---------------------------------------------------------------------------
void bcWPFKeyEvent::CancelEvent(void)noexcept(true)
{
}
//---------------------------------------------------------------------------
bool bcWPFKeyEvent::IsCancelled(void)noexcept(true)
{
	return false;
}
//---------------------------------------------------------------------------
bool bcWPFKeyEvent::IsKeyDown(eKeyCode KeyCode)noexcept(true)
{
	return mcWPFInput::mKeyboardEventIsKeyDown(KeyCode);
}
//---------------------------------------------------------------------------
bool bcWPFKeyEvent::IsKeyToggled(eKeyCode KeyCode)noexcept(true)
{
	return mcWPFInput::mKeyboardEventIsKeyToggled(KeyCode);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
eUIEventSourceType bcWPFMouseEvent::GetSourceType(void)noexcept(true)
{
	return UIEventSourceType::Mouse;
}
//---------------------------------------------------------------------------
iUIMouse* bcWPFMouseEvent::GetMouse(void)noexcept(true)
{
	auto Thread=mbcWPFUIThread::mCurrentUIThread();
	if(Thread==nullptr)
		return nullptr;
	auto WPFUIThread=static_cast<cWPFUIThread*>(Thread);
	return WPFUIThread->GetDefaultMouse();
}
//---------------------------------------------------------------------------
void bcWPFMouseEvent::CancelEvent(void)noexcept(true)
{
}
//---------------------------------------------------------------------------
bool bcWPFMouseEvent::IsCancelled(void)noexcept(true)
{
	return false;
}
//---------------------------------------------------------------------------
bool bcWPFMouseEvent::GetPosition(iInterface *Relative,cUIPoint &Position)noexcept(true)
{
	auto RelativeObject=iCast<iCLIObject>(Relative);
	if(RelativeObject==nullptr)
		return false;

	auto &Handle=RelativeObject->GetObjecHandle();
	return mcWPFInput::mMouseGetPosition(Handle,Position);
}
//---------------------------------------------------------------------------
bool bcWPFMouseEvent::IsButtonDown(eMouseButton Button)noexcept(true)
{
	return mcWPFInput::mMouseEventIsButtonDown(Button);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWPFKeyEventByMessage::cWPFKeyEventByMessage(iUIView *SourceView)noexcept(true)
{
	fSourceView=SourceView;
}
//---------------------------------------------------------------------------
iUIView* cWPFKeyEventByMessage::GetSourceView(void)noexcept(true)
{
	return fSourceView;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWPFMouseEventByMessage::cWPFMouseEventByMessage(iUIView *SourceView)noexcept(true)
{
	fSourceView=SourceView;
}
//---------------------------------------------------------------------------
iUIView* cWPFMouseEventByMessage::GetSourceView(void)noexcept(true)
{
	return fSourceView;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWPFKeyEvent::cWPFKeyEvent(iUIView *SourceView,mcWPFKeyEventArgs &EventArgs)noexcept(true)
	: fEventArgs(EventArgs)
	, fSourceView(SourceView)
{
}
//---------------------------------------------------------------------------
cWPFKeyEvent::~cWPFKeyEvent()noexcept(true)
{
}
//---------------------------------------------------------------------------
iUIView* cWPFKeyEvent::GetSourceView(void)noexcept(true)
{
	return fSourceView;
}
//---------------------------------------------------------------------------
void cWPFKeyEvent::CancelEvent(void)noexcept(true)
{
	return fEventArgs.mCancelEvent();
}
//---------------------------------------------------------------------------
bool cWPFKeyEvent::IsCancelled(void)noexcept(true)
{
	return fEventArgs.mIsCancelled();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWPFMouseEvent::cWPFMouseEvent(iUIView *SourceView,mcWPFMouseEventArgs &EventArgs)noexcept(true)
	: fEventArgs(EventArgs)
	, fSourceView(SourceView)
{
}
//---------------------------------------------------------------------------
cWPFMouseEvent::~cWPFMouseEvent()noexcept(true)
{
}
//---------------------------------------------------------------------------
void cWPFMouseEvent::CancelEvent(void)noexcept(true)
{
	return fEventArgs.mCancelEvent();
}
//---------------------------------------------------------------------------
bool cWPFMouseEvent::IsCancelled(void)noexcept(true)
{
	return fEventArgs.mIsCancelled();
}
//---------------------------------------------------------------------------
eUIEventSourceType cWPFMouseEvent::GetSourceType(void)noexcept(true)
{
	return UIEventSourceType::Mouse;
}
//---------------------------------------------------------------------------
iUIView* cWPFMouseEvent::GetSourceView(void)noexcept(true)
{
	return fSourceView;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWPFTouchEvent::cWPFTouchEvent(iUIView *SourceView,mcWPFTouchEventArgs &EventArgs)noexcept(true)
	: fEventArgs(EventArgs)
	, fSourceView(SourceView)
{
	fTouchID=fEventArgs.mGetTouchID();
}
//---------------------------------------------------------------------------
cWPFTouchEvent::~cWPFTouchEvent()noexcept(true)
{
}
//---------------------------------------------------------------------------
void cWPFTouchEvent::CancelEvent(void)noexcept(true)
{
	return fEventArgs.mCancelEvent();
}
//---------------------------------------------------------------------------
bool cWPFTouchEvent::IsCancelled(void)noexcept(true)
{
	return fEventArgs.mIsCancelled();
}
//---------------------------------------------------------------------------
eUIEventSourceType cWPFTouchEvent::GetSourceType(void)noexcept(true)
{
	return UIEventSourceType::Touch;
}
//---------------------------------------------------------------------------
iUIView* cWPFTouchEvent::GetSourceView(void)noexcept(true)
{
	return fSourceView;
}
//---------------------------------------------------------------------------
void* cWPFTouchEvent::GetTouchID(void)noexcept(true)
{
	return fTouchID;
}
//---------------------------------------------------------------------------
bool cWPFTouchEvent::GetPosition(iInterface *Relative,cUIPoint &Position)noexcept(true)
{
	auto RelativeSubview=iCast<cWPFUIView>(Relative);
	if(RelativeSubview==nullptr)
		return false;

	auto &Handle=RelativeSubview->GetTargetElementHandle();
	return fEventArgs.mGetPosition(Handle,Position);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcWPFViewContentDrawing::bcWPFViewContentDrawing()noexcept(true)
{
	fView=nullptr;
	fZPosition=0;
	fContentMargin=UIRectangleZero;
}
//---------------------------------------------------------------------------
bcWPFViewContentDrawing::~bcWPFViewContentDrawing()noexcept(true)
{
}
//---------------------------------------------------------------------------
cWPFUIView* bcWPFViewContentDrawing::GetView(void)noexcept(true)
{
	return fView;
}
//---------------------------------------------------------------------------
bool bcWPFViewContentDrawing::SetView(iUIView *View)noexcept(true)
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
bool bcWPFViewContentDrawing::GetVisible(void)noexcept(true)
{
	return true;
}
//---------------------------------------------------------------------------
bool bcWPFViewContentDrawing::SetVisible(bool Visible)noexcept(true)
{
	return false;
}
//---------------------------------------------------------------------------
Float32 bcWPFViewContentDrawing::GetZPosition(void)noexcept(true)
{
	return fZPosition;
}
//---------------------------------------------------------------------------
bool bcWPFViewContentDrawing::SetZPosition(Float32 ZPosition)noexcept(true)
{
	fZPosition=ZPosition;
	if(fView!=nullptr){
		fView->UpdateViewContentZPosition();
	}
	return true;
}
//---------------------------------------------------------------------------
Float32 bcWPFViewContentDrawing::GetContentScale(void)noexcept(true)
{
	return fView->GetViewContentContentScale();
}
//---------------------------------------------------------------------------
cUIRectangle bcWPFViewContentDrawing::GetContentMargin(void)noexcept(true)
{
	return fContentMargin;
}
//---------------------------------------------------------------------------
void bcWPFViewContentDrawing::SetContentMargin(cUIRectangle Margin)noexcept(true)
{
	if(UIRectangleIsEqual(fContentMargin,Margin))
		return;
	fContentMargin=Margin;
	ViewContentUpdateRect();
}
//---------------------------------------------------------------------------
void bcWPFViewContentDrawing::UpdateDrawing(void)noexcept(true)
{
	if(fView!=nullptr){
		fView->UpdateViewContent();
	}
}
//---------------------------------------------------------------------------
eUIState bcWPFViewContentDrawing::CheckViewContentState(void)noexcept(true)
{
	if(fView!=nullptr){
		return fView->GetUIState();
	}
	return UIState::Null;
}
//---------------------------------------------------------------------------
void bcWPFViewContentDrawing::WPFArrange(Float32 LayoutScale,Float32 Width,Float32 Height)noexcept(true)
{
	fViewWidth=Width;
	fViewHeight=Height;
	fLayoutScale=LayoutScale;
	ViewContentUpdateRect();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
sfInt8 cWPFUIView::cSubviewItemZOrderOperator::Compare(const tSubviewNode *Item,const tSubviewNode *Value)noexcept(true)
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
cWPFUIView::cWPFUIView(cWPFUIThread *UIThread,const cGCHandle &WPFUIViewTargetElement)noexcept(true)
	: fUIThread(UIThread)
	, fViewState(UIState::Null)
	, fNeedLayout(false)
{
	mbcWPFUIView::mSetup(WPFUIViewTargetElement);
}
//---------------------------------------------------------------------------
cWPFUIView::~cWPFUIView()noexcept(true)
{
	mbcWPFUIView::mClear();
}
//---------------------------------------------------------------------------
void* cWPFUIView::CastInterface(iTypeID ID)noexcept(true)
{
	return cnClass::ImplementCastInterface<iCLIObject,iUIView,iUIKeyControl,iUIMouseControl,iUITouchControl>(this,ID);	
}
//---------------------------------------------------------------------------
void cWPFUIView::VirtualStarted(void)noexcept(true)
{
}
//---------------------------------------------------------------------------
void cWPFUIView::VirtualStopped(void)noexcept(true)
{
	if(fUIThread->IsCurrent()){
		Cleanup();
	}
	else{
		fUIThread->mDispatchExecute(true,&fExitParentProcedure);
	}
}
//---------------------------------------------------------------------------
void cWPFUIView::cExitParentProcedure::Execute(void)noexcept(true)
{
	auto Host=cnMemory::GetObjectFromMemberPointer(this,&cWPFUIView::fExitParentProcedure);
	return Host->Cleanup();
}
//---------------------------------------------------------------------------
void cWPFUIView::Cleanup(void)noexcept(true)
{
	if(fParent!=nullptr){
		fParent->RemoveSubview(this);
	}

	Dispose();
}
//---------------------------------------------------------------------------
const cGCHandle& cWPFUIView::WPFGetTargetElementHandle(void)noexcept(true)
{
	return GetTargetElementHandle();
}
//---------------------------------------------------------------------------
const cGCHandle& cWPFUIView::GetObjecHandle(void)noexcept(true)
{
	return GetTargetElementHandle();
}
//---------------------------------------------------------------------------
bool cWPFUIView::CheckThread(void)noexcept(true)
{
	return fUIThread->IsCurrent();
}
//---------------------------------------------------------------------------
iUIThread* cWPFUIView::GetUIThread(void)noexcept(true)
{
	return fUIThread;
}
//---------------------------------------------------------------------------
Float32 cWPFUIView::GetContentScale(void)noexcept(true)
{
	return fContentScale;
}
//---------------------------------------------------------------------------
cUIPoint cWPFUIView::GetSize(void)noexcept(true)
{
	if(CheckThread()==false)
		return UIPointZero;
	return mGetSize();
}
//---------------------------------------------------------------------------
bool cWPFUIView::SetSize(cUIPoint Size)noexcept(true)
{
	if(CheckThread()==false)
		return false;

	if(Size.x<0){
		Size.x=0;
	}
	if(Size.y<0){
		Size.y=0;
	}

	Size.x=cnMath::FloatRoundNearest(Size.x);
	Size.y=cnMath::FloatRoundNearest(Size.y);
	return mSetSize(Size);
}
//---------------------------------------------------------------------------
bool cWPFUIView::MoveTo(iInterface *Relative,cUIPoint Position)noexcept(true)
{
	if(CheckThread()==false)
		return false;

	auto RelativeObject=iCast<iCLIObject>(Relative);
	if(RelativeObject==nullptr)
		return false;

	auto &Handle=RelativeObject->GetObjecHandle();
	Position.x=cnMath::FloatRoundNearest(Position.x);
	Position.y=cnMath::FloatRoundNearest(Position.y);
	return mMovePosition(Handle,Position);
}
//---------------------------------------------------------------------------
bool cWPFUIView::TranslatePointTo(iInterface *Relative,cUIPoint &Position)noexcept(true)
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
bool cWPFUIView::ArrangeRectangle(iInterface *Relative,cUIPoint Position,cUIPoint Size)noexcept(true)
{
	if(CheckThread()==false)
		return false;

	auto RelativeObject=iCast<iCLIObject>(Relative);
	if(RelativeObject==nullptr)
		return false;

	Position.x=cnMath::FloatRoundNearest(Position.x);
	Position.y=cnMath::FloatRoundNearest(Position.y);
	if(Size.x<0)
		Size.x=0;
	else
		Size.x=cnMath::FloatRoundNearest(Size.x);
	if(Size.y<0)
		Size.y=0;
	else
		Size.y=cnMath::FloatRoundNearest(Size.y);

	auto &Handle=RelativeObject->GetObjecHandle();
	return mArrangeRectangle(Handle,Position,Size);
}
//---------------------------------------------------------------------------
Float32 cWPFUIView::GetZPosition(void)noexcept(true)
{
	return fZPosition;
}
//---------------------------------------------------------------------------
bool cWPFUIView::SetZPosition(Float32 ZPosition)noexcept(true)
{
	if(CheckThread()==false)
		return false;

	if(fZPosition==ZPosition)
		return true;


	fZPosition=ZPosition;
	if(fParent!=nullptr){
		fParent->UpdateSubviewZPosition(this);
	}
	return true;
}
//---------------------------------------------------------------------------
eUIState cWPFUIView::GetUIState(void)noexcept(true)
{
	return fViewState;
}
//---------------------------------------------------------------------------
bool cWPFUIView::InsertStateHandler(iUIStateHandler *Handler,sfInt16 Order)noexcept(true)
{
	if(CheckThread()==false)
		return false;
	fViewNotification.InsertStateHandler(Handler,Order);
	return true;
}
//---------------------------------------------------------------------------
bool cWPFUIView::RemoveStateHandler(iUIStateHandler *Handler)noexcept(true)
{
	if(CheckThread()==false)
		return false;
	fViewNotification.RemoveStateHandler(Handler);
	return true;
}
//---------------------------------------------------------------------------
bool cWPFUIView::GetVisible(void)noexcept(true)
{
	return mGetVisible();
}
//---------------------------------------------------------------------------
bool cWPFUIView::SetVisible(bool Visible)noexcept(true)
{
	return mSetVisible(Visible);
}
//---------------------------------------------------------------------------
bool cWPFUIView::IsEnabled(void)noexcept(true)
{
	return true;
}
//---------------------------------------------------------------------------
bool cWPFUIView::GetEnable(void)noexcept(true)
{
	return true;
}
//---------------------------------------------------------------------------
bool cWPFUIView::SetEnable(bool Enable)noexcept(true)
{
	return false;
}
//---------------------------------------------------------------------------
bool cWPFUIView::InsertViewHandler(iUIViewHandler *Handler,sfInt16 Order)noexcept(true)
{
	if(CheckThread()==false)
		return false;
	fViewNotification.InsertViewHandler(Handler,Order);
	return true;
}
//---------------------------------------------------------------------------
bool cWPFUIView::RemoveViewHandler(iUIViewHandler *Handler)noexcept(true)
{
	if(CheckThread()==false)
		return false;
	fViewNotification.RemoveViewHandler(Handler);
	return true;
}
//---------------------------------------------------------------------------
void cWPFUIView::SetArrangement(void)noexcept(true)
{
	if(CheckThread()==false)
		return;

	mSetArrangement();
}
//---------------------------------------------------------------------------
void cWPFUIView::ArrangeLayout(void)noexcept(true)
{
	if(CheckThread()==false)
		return;

	mArrangeLayout();
}
//---------------------------------------------------------------------------
void cWPFUIView::SetIncludeParentFrameMargin(bool Include)noexcept(true)
{
}
//---------------------------------------------------------------------------
cUIRectangle cWPFUIView::GetFrameMargin(void)noexcept(true)
{
	return UIRectangleZero;
}
//---------------------------------------------------------------------------
iUIWindow* cWPFUIView::GetWindow(void)noexcept(true)
{
	if(fWPFWindow==nullptr)
		return nullptr;
	return fWPFWindow->WPFWindowInterface();
}
//---------------------------------------------------------------------------
iUIView* cWPFUIView::GetParent(void)noexcept(true)
{
	if(fParent!=nullptr)
		return fParent;
	return nullptr;
}
//---------------------------------------------------------------------------
eUIState cWPFUIView::EvaluateViewState(void)noexcept(true)
{
	if(fWPFParent==nullptr){
		return UIState::Null;
	}
	if(fStateVisible==false){
		return UIState::Background;
	}
	if(fWPFWindow!=nullptr){
		bool IsActive=fWPFWindow->WPFWindowIsActive();
		if(IsActive){
			return UIState::Active;
		}
		else{
			return UIState::Inactive;
		}
	}
	else{
		// always active when outside uiview container
		return UIState::Active;
	}
}
//---------------------------------------------------------------------------
bool cWPFUIView::UpdateViewState(void)noexcept(true)
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
void cWPFUIView::UpdateWindow(iWPFViewWindow *WPFWindow)noexcept(true)
{
	if(fWPFWindow==WPFWindow){
		// update view state only
		UpdateViewState();
		return;
	}

	fWPFWindow=WPFWindow;

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
void cWPFUIView::UpdateSubviewZPosition(cWPFUIView *Subview)noexcept(true)
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
void cWPFUIView::NotifyWindowChanged(void)noexcept(true)
{
	fViewNotification.WindowChanged();

	iWindow *Window;
	if(fWPFWindow==nullptr)
		Window=nullptr;
	else
		Window=fWPFWindow->WPFWindowGetWindow();
	// notify view contents
	for(auto *Content : fContents){
		Content->ViewContentUpdateWindow(Window);
	}
}
//---------------------------------------------------------------------------
bool cWPFUIView::InsertView(iUIView *View)noexcept(true)
{
	if(CheckThread()==false)
		return false;
	if(View==nullptr)
		return false;
	auto Subview=iCast<cWPFUIView>(View);
	if(Subview==nullptr)
		return false;

	// add to set
	bool Exists;
	auto pSetItem=fSubviewMap.InsertPair(Subview,Exists);
	if(Exists){
		return false;
	}
	pSetItem->Value.Child=Subview;
	pSetItem->Value.ZPosition=Subview->WPFChildGetZPosition();
	// add to zorder set
	fSubviewZOrderSet.Insert(pSetItem);

	if(Subview->WPFChildParentAcquire(this)){
		auto &Handle=Subview->WPFGetTargetElementHandle();
		if(mInsertView(Handle)){
			Subview->WPFChildTreeNotifyWindow(fWPFWindow);
			return true;
		}
	}
	// failed
	fSubviewZOrderSet.Remove(pSetItem);
	fSubviewMap.RemovePair(pSetItem);
	return false;
}
//---------------------------------------------------------------------------
bool cWPFUIView::RemoveView(iUIView *View)noexcept(true)
{
	if(View==nullptr)
		return false;
	auto Subview=iCast<cWPFUIView>(View);
	if(Subview==nullptr)
		return false;

	return RemoveSubview(Subview);
}
//---------------------------------------------------------------------------
bool cWPFUIView::RemoveSubview(cWPFUIView *Subview)noexcept(true)
{
	auto pSetItem=fSubviewMap.GetPair(Subview);
	if(pSetItem==fSubviewMap.end()){
		// not in the view
		return false;
	}
	// remove from order list
	fSubviewZOrderSet.Remove(pSetItem);
	// remove from set
	fSubviewMap.RemovePair(pSetItem);

	auto &WPFChild=Subview->WPFGetTargetElementHandle();
	mRemoveView(WPFChild);

	Subview->WPFChildParentRelease(this);
	return true;
}
//---------------------------------------------------------------------------
cWPFUIView::cChildrenInfo* cWPFUIView::WPFSetupChildrenInfo(void* &Handle)noexcept(true)
{
	auto SubviewInfo=new cSubviewInfo;

	SubviewInfo->ChildCount=fSubviewZOrderSet.GetCount();
	SubviewInfo->ChildList.SetCount(SubviewInfo->ChildCount);
	
	for(uIntn i=0;i<SubviewInfo->ChildCount;i++){
		SubviewInfo->ChildList[i]=fSubviewZOrderSet[i]->Value.Child;
	}
	SubviewInfo->Views=SubviewInfo->ChildList;
	Handle=SubviewInfo;
	return SubviewInfo;
}
//---------------------------------------------------------------------------
void cWPFUIView::WPFClearChildrenInfo(void *Handle)noexcept(true)
{
	delete static_cast<cSubviewInfo*>(Handle);
}
//---------------------------------------------------------------------------
bool cWPFUIView::WPFAttachExtParent(void)noexcept(true)
{
	if(WPFChildParentAcquire(&fExternalWPFParent)){
		iIncReference(this,'pare');
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------
void cWPFUIView::WPFDetachExtParent(void)noexcept(true)
{
	WPFChildParentRelease(&fExternalWPFParent);	
	iDecReference(this,'pare');
}
//---------------------------------------------------------------------------
bool cWPFUIView::InsertViewContent(bcWPFViewContentDrawing *Content)noexcept(true)
{
	if(fContents.Insert(Content)){
		UpdateViewContentZPosition();
	}
	return true;
}
//---------------------------------------------------------------------------
bool cWPFUIView::RemoveViewContent(bcWPFViewContentDrawing *Content)noexcept(true)
{
	if(fContents.Remove(Content)){
		UpdateViewContentZPosition();
	}
	return true;
}
//---------------------------------------------------------------------------
void cWPFUIView::UpdateViewContentZPosition(void)noexcept(true)
{
	ClearContentOrderCache();
	SetupContentOrderCache();
	ApplyContentChanged();
}
//---------------------------------------------------------------------------
void cWPFUIView::UpdateViewContent(void)noexcept(true)
{
	ApplyContentChanged();
}
//---------------------------------------------------------------------------
Float32 cWPFUIView::GetViewContentContentScale(void)noexcept(true)
{
	return fContentScale;
}
//---------------------------------------------------------------------------
void cWPFUIView::SetupContentOrderCache(void)noexcept(true)
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
void cWPFUIView::ClearContentOrderCache(void)noexcept(true)
{
	fContentOrderCache.Clear();
	fContentOrderCacheUpdate=true;
}
//---------------------------------------------------------------------------
void cWPFUIView::ApplyContentChanged(void)noexcept(true)
{
	//if(fUIThread->IsShutdown())
	//	return;
	auto *Drawings=fContentOrderCache.GetItems();
	uIntn Count=fContentOrderCache.GetCount();

	mRenderContent(Drawings,Count);
}
//---------------------------------------------------------------------------
iUIView* cWPFUIView::GetFocus(void)noexcept(true)
{
	return nullptr;
}
//---------------------------------------------------------------------------
void cWPFUIView::SetFocus(bool ActivateWindow)noexcept(true)
{
	return mSetFocus(ActivateWindow);
}
//---------------------------------------------------------------------------
bool cWPFUIView::InsertKeyHandler(iUIKeyHandler *Handler,sfInt16 Order)noexcept(true)
{
	if(CheckThread()==false)
		return false;
	fKeyNotification.InsertKeyHandler(Handler,Order);
	return true;
}
//---------------------------------------------------------------------------
bool cWPFUIView::RemoveKeyHandler(iUIKeyHandler *Handler)noexcept(true)
{
	if(CheckThread()==false)
		return false;
	fKeyNotification.RemoveKeyHandler(Handler);
	return true;
}
//---------------------------------------------------------------------------
bool cWPFUIView::InsertKeyFilter(iUIKeyHandler *Filter,sfInt16 Order)noexcept(true)
{
	if(CheckThread()==false)
		return false;
	fKeyNotification.InsertKeyFilter(Filter,Order);
	return true;
}
//---------------------------------------------------------------------------
bool cWPFUIView::RemoveKeyFilter(iUIKeyHandler *Filter)noexcept(true)
{
	if(CheckThread()==false)
		return false;
	fKeyNotification.RemoveKeyFilter(Filter);
	return true;
}
//---------------------------------------------------------------------------
bool cWPFUIView::KeyAcquireExclusive(iUIKeyHandler *Handler)noexcept(true)
{
	if(CheckThread()==false)
		return false;
	return fKeyNotification.KeyAcquireExclusive(Handler);
}
//---------------------------------------------------------------------------
bool cWPFUIView::KeyReleaseExclusive(iUIKeyHandler *Handler)noexcept(true)
{
	if(CheckThread()==false)
		return false;
	fKeyNotification.KeyReleaseExclusive(Handler);
	return true;
}
//---------------------------------------------------------------------------
bool cWPFUIView::InsertTextInputHandler(iUITextInputHandler *Handler,sfInt16 Order)noexcept(true)
{
	if(CheckThread()==false)
		return false;
	fKeyNotification.InsertTextInputHandler(Handler,Order);
	return true;
}
//---------------------------------------------------------------------------
bool cWPFUIView::RemoveTextInputHandler(iUITextInputHandler *Handler)noexcept(true)
{
	if(CheckThread()==false)
		return false;
	fKeyNotification.RemoveTextInputHandler(Handler);
	return true;
}
//---------------------------------------------------------------------------
bool cWPFUIView::SetCompositionViewInfo(iUITextCompositionViewInfo *CompositionViewInfo)noexcept(true)
{
	return false;
}
//---------------------------------------------------------------------------
void cWPFUIView::MouseUpdateCursor(void)noexcept(true)
{
}
//---------------------------------------------------------------------------
bool cWPFUIView::InsertMouseHandler(iUIMouseHandler *Handler,sfInt16 Order)noexcept(true)
{
	if(CheckThread()==false)
		return false;
	fMouseNotification.InsertMouseHandler(Handler,Order);
	return true;
}
//---------------------------------------------------------------------------
bool cWPFUIView::RemoveMouseHandler(iUIMouseHandler *Handler)noexcept(true)
{
	if(CheckThread()==false)
		return false;
	fMouseNotification.RemoveMouseHandler(Handler);
	return true;
}
//---------------------------------------------------------------------------
bool cWPFUIView::InsertMouseFilter(iUIMouseHandler *Filter,sfInt16 Order)noexcept(true)
{
	if(CheckThread()==false)
		return false;
	fMouseNotification.InsertMouseFilter(Filter,Order);
	return true;
}
//---------------------------------------------------------------------------
bool cWPFUIView::RemoveMouseFilter(iUIMouseHandler *Filter)noexcept(true)
{
	if(CheckThread()==false)
		return false;
	fMouseNotification.RemoveMouseFilter(Filter);
	return true;
}
//---------------------------------------------------------------------------
bool cWPFUIView::MouseSetCaptrue(void)noexcept(true)
{
	return false;
}
//---------------------------------------------------------------------------
void cWPFUIView::MouseReleaseCaptrue(void)noexcept(true)
{
}
//---------------------------------------------------------------------------
bool cWPFUIView::MouseIsCaptured(void)noexcept(true)
{
	return false;
}
//---------------------------------------------------------------------------
bool cWPFUIView::MouseAcquireExclusive(iUIMouseHandler *Handler)noexcept(true)
{
	if(CheckThread()==false)
		return false;
	return fMouseNotification.MouseAcquireExclusive(Handler);
}
//---------------------------------------------------------------------------
bool cWPFUIView::MouseReleaseExclusive(iUIMouseHandler *Handler)noexcept(true)
{
	if(CheckThread()==false)
		return false;
	fMouseNotification.MouseReleaseExclusive(Handler);
	return true;
}
//---------------------------------------------------------------------------
bool cWPFUIView::InsertTouchHandler(iUITouchHandler *Handler,sfInt16 Order)noexcept(true)
{
	if(CheckThread()==false)
		return false;
	fTouchNotification.InsertTouchHandler(Handler,Order);
	return true;
}
//---------------------------------------------------------------------------
bool cWPFUIView::RemoveTouchHandler(iUITouchHandler *Handler)noexcept(true)
{
	if(CheckThread()==false)
		return false;
	fTouchNotification.RemoveTouchHandler(Handler);
	return true;
}
//---------------------------------------------------------------------------
bool cWPFUIView::InsertTouchFilter(iUITouchHandler *Filter,sfInt16 Order)noexcept(true)
{
	if(CheckThread()==false)
		return false;
	fTouchNotification.InsertTouchFilter(Filter,Order);
	return true;
}
//---------------------------------------------------------------------------
bool cWPFUIView::RemoveTouchFilter(iUITouchHandler *Filter)noexcept(true)
{
	if(CheckThread()==false)
		return false;
	fTouchNotification.RemoveTouchFilter(Filter);
	return true;
}
//---------------------------------------------------------------------------
bool cWPFUIView::TouchAcquireExclusive(iUITouchHandler *Handler)noexcept(true)
{
	if(CheckThread()==false)
		return false;
	return fTouchNotification.TouchAcquireExclusive(Handler);
}
//---------------------------------------------------------------------------
bool cWPFUIView::TouchReleaseExclusive(iUITouchHandler *Handler)noexcept(true)
{
	if(CheckThread()==false)
		return false;
	fTouchNotification.TouchReleaseExclusive(Handler);
	return true;
}
//---------------------------------------------------------------------------
bool cWPFUIView::WPFChildParentAcquire(cWPFUIView *Parent)noexcept(true)
{
	if(fWPFParent!=nullptr)
		return false;

	fParent=Parent;
	fWPFParent=Parent;
	return true;
}
//---------------------------------------------------------------------------
void cWPFUIView::WPFChildParentRelease(cWPFUIView *Parent)noexcept(true)
{
	if(fWPFParent!=Parent)
		return;

	fParent=nullptr;
	fWPFParent=nullptr;
}
//---------------------------------------------------------------------------
bool cWPFUIView::WPFChildParentAcquire(iWPFViewParent *Parent)noexcept(true)
{
	if(fWPFParent!=nullptr)
		return false;

	fParent=nullptr;
	fWPFParent=Parent;
	return true;
}
//---------------------------------------------------------------------------
void cWPFUIView::WPFChildParentRelease(iWPFViewParent *Parent)noexcept(true)
{
	if(fWPFParent!=Parent)
		return;

	fWPFParent=nullptr;
}
//---------------------------------------------------------------------------
Float32 cWPFUIView::WPFChildGetLayoutScale(void)noexcept(true)
{
	return fLayoutScale;
}
//---------------------------------------------------------------------------
Float32 cWPFUIView::WPFChildGetZPosition(void)noexcept(true)
{
	return fZPosition;
}
//---------------------------------------------------------------------------
void cWPFUIView::WPFChildTreeNotifyWindow(iWPFViewWindow *WPFWindow)noexcept(true)
{
	UpdateWindow(WPFWindow);
	for(auto &Item : fSubviewZOrderSet){
		Item->Key->WPFChildTreeNotifyWindow(WPFWindow);
	}
}
//---------------------------------------------------------------------------
void cWPFUIView::WPFChildTreeNotifyState(void)noexcept(true)
{
	if(UpdateViewState()){
		for(auto &Item : fSubviewZOrderSet){
			Item->Key->WPFChildTreeNotifyState();
		}
	}
}
//---------------------------------------------------------------------------
void cWPFUIView::WPFChildTreeNotifyScale(Float32 LayoutScale,Float32 ContentScale)noexcept(true)
{
	fLayoutScale=LayoutScale;
	fContentScale=ContentScale;

	fViewNotification.ContentScaleChanged();

	for(auto &Item : fSubviewZOrderSet){
		Item->Key->WPFChildTreeNotifyScale(LayoutScale,ContentScale);
	}

	auto ViewSize=mbcWPFUIView::mGetSize();
	for(auto *Content : fContents){
		Content->WPFArrange(fLayoutScale,ViewSize.x,ViewSize.y);
	}
}
//---------------------------------------------------------------------------
void cWPFUIView::ParentNotifyKeyboardFocusWithin(cWPFUIView *,iUIKeyEvent *KeyEvent)noexcept(true)
{
	WPFNotifyFocusEnterRange(KeyEvent);
}
//---------------------------------------------------------------------------
void cWPFUIView::ParentNotifyKeyboardFocuseEnter(iUIKeyEvent *KeyEvent)noexcept(true)
{
	if(fParent!=nullptr){
		fParent->ParentNotifyKeyboardFocuseEnter(KeyEvent);
	}

	fKeyNotification.FilterFocusEnter(KeyEvent);
}
//---------------------------------------------------------------------------
void cWPFUIView::ParentNotifyKeyboardFocuseLeave(iUIKeyEvent *KeyEvent)noexcept(true)
{
	if(fParent!=nullptr){
		fParent->ParentNotifyKeyboardFocuseLeave(KeyEvent);
	}

	fKeyNotification.FilterFocusLeave(KeyEvent);
}
//---------------------------------------------------------------------------
void cWPFUIView::ParentNotifyKeyboardKeyInput(iUIKeyEvent *KeyEvent,eKeyCode KeyCode)noexcept(true)
{
	if(fParent!=nullptr){
		fParent->ParentNotifyKeyboardKeyInput(KeyEvent,KeyCode);
	}

	fKeyNotification.FilterKeyInput(KeyEvent,KeyCode);
}
//---------------------------------------------------------------------------
void cWPFUIView::ParentNotifyMouseEnter(cWPFUIView *,iUIMouseEvent *MouseEvent)noexcept(true)
{
	WPFNotifyMouseEnterRange(MouseEvent);
}
//---------------------------------------------------------------------------
void cWPFUIView::ParentNotifyMouseIsDirectlyEnter(iUIMouseEvent *MouseEvent)noexcept(true)
{
	if(fParent!=nullptr){
		fParent->ParentNotifyMouseIsDirectlyEnter(MouseEvent);
	}
	fMouseNotification.FilterEnter(MouseEvent);
}
//---------------------------------------------------------------------------
void cWPFUIView::ParentNotifyMouseIsDirectlyLeave(iUIMouseEvent *MouseEvent)noexcept(true)
{
	if(fParent!=nullptr){
		fParent->ParentNotifyMouseIsDirectlyLeave(MouseEvent);
	}
	fMouseNotification.FilterLeave(MouseEvent);
}
//---------------------------------------------------------------------------
void cWPFUIView::WPFNotifyFocusEnterRange(iUIKeyEvent *KeyEvent)noexcept(true)
{
	if(fKeyFocusInRangeNotified==false){
		fKeyFocusInRangeNotified=true;

		// notify parent focus enter range
		if(fParent!=nullptr){
			fParent->ParentNotifyKeyboardFocusWithin(this,KeyEvent);
		}

		fKeyNotification.FocusEnterRange(KeyEvent);
	}
}
//---------------------------------------------------------------------------
void cWPFUIView::WPFNotifyFocusLeaveRange(iUIKeyEvent *KeyEvent)noexcept(true)
{
	fKeyFocusInRangeNotified=false;

	fKeyNotification.FocusLeaveRange(KeyEvent);
}
//---------------------------------------------------------------------------
void cWPFUIView::WPFNotifyFocusEnter(iUIKeyEvent *KeyEvent)noexcept(true)
{
	if(fKeyFocusedNotifyValue==false){
		fKeyFocusedNotifyValue=true;

		ParentNotifyKeyboardFocuseEnter(KeyEvent);

		fKeyNotification.HandleFocusEnter(KeyEvent);
	}
}
//---------------------------------------------------------------------------
void cWPFUIView::WPFNotifyFocusLeave(iUIKeyEvent *KeyEvent)noexcept(true)
{
	if(fKeyFocusedNotifyValue){
		fKeyFocusedNotifyValue=false;

		ParentNotifyKeyboardFocuseLeave(KeyEvent);

		fKeyNotification.HandleFocusLeave(KeyEvent);
	}
}
//---------------------------------------------------------------------------
void cWPFUIView::WPFNotifyMouseIsDirectlyEnter(iUIMouseEvent *MouseEvent)noexcept(true)
{
	if(fMouseIsOverNotifyValue==false){
		fMouseIsOverNotifyValue=true;

		ParentNotifyMouseIsDirectlyEnter(MouseEvent);

		fMouseNotification.HandleEnter(MouseEvent);
	}
}
//---------------------------------------------------------------------------
void cWPFUIView::WPFNotifyMouseIsDirectlyLeave(iUIMouseEvent *MouseEvent)noexcept(true)
{
	if(fMouseIsOverNotifyValue){
		fMouseIsOverNotifyValue=false;

		ParentNotifyMouseIsDirectlyLeave(MouseEvent);

		fMouseNotification.HandleLeave(MouseEvent);
	}
}
//---------------------------------------------------------------------------
void cWPFUIView::WPFNotifyMouseEnterRange(iUIMouseEvent *MouseEvent)noexcept(true)
{
	if(fMouseEnterNotified==false){
		fMouseEnterNotified=true;

		// notify parent mouse enter
		if(fParent!=nullptr){
			fParent->ParentNotifyMouseEnter(this,MouseEvent);
		}

		fMouseNotification.MouseEnterRange(MouseEvent);
	}
}
//---------------------------------------------------------------------------
void cWPFUIView::WPFNotifyMouseLeave(iUIMouseEvent *MouseEvent)noexcept(true)
{
	WPFNotifyMouseIsDirectlyLeave(MouseEvent);

	fMouseEnterNotified=false;
	fMouseNotification.MouseLeaveRange(MouseEvent);
}
//---------------------------------------------------------------------------
void cWPFUIView::WPFNotifyVisible(bool Visible)noexcept(true)
{
	if(fStateVisible==Visible)
		return;
	fStateVisible=Visible;

	WPFChildTreeNotifyState();
}
//---------------------------------------------------------------------------
void cWPFUIView::WPFNotifyArrange(bool Moved,bool Sized,cUIRect Rect)noexcept(true)
{
	for(auto *Content : fContents){
		Content->WPFArrange(fLayoutScale,Rect.Size.x,Rect.Size.y);
	}

	UpdateViewState();

	fViewNotification.RectangleChanged(Moved,Sized);

	if(fLayoutInProgress){
		return;
	}
	fLayoutInProgress=true;

	fViewNotification.Layout();

	fLayoutInProgress=false;

	ApplyContentChanged();
}
//---------------------------------------------------------------------------
void cWPFUIView::WPFOnIsKeyboardFocusedChanged(bool Focused)noexcept(true)
{
	cWPFKeyEventByMessage KeyEvent(this);

	if(Focused){
		WPFNotifyFocusEnter(&KeyEvent);
	}
	else{
		WPFNotifyFocusLeave(&KeyEvent);
	}
}
//---------------------------------------------------------------------------
void cWPFUIView::WPFOnIsKeyboardFocusWithinChanged(bool Focused)noexcept(true)
{
	cWPFKeyEventByMessage KeyEvent(this);

	//auto Msg=cnRTL::CreateStringFormat(L"%p WPFOnIsKeyboardFocusWithinChanged %d\n",KeyEvent->GetSourceView(),Focused);
	//OutputDebugStringW(Msg);
	if(Focused){
		WPFNotifyFocusEnterRange(&KeyEvent);
	}
	else{
		WPFNotifyFocusLeave(&KeyEvent);

		WPFNotifyFocusLeaveRange(&KeyEvent);
	}
}
//---------------------------------------------------------------------------
void cWPFUIView::WPFOnKeyDownFilter(mcWPFKeyEventArgs &KeyEventArgs,eKeyCode KeyCode,bool Repeat)noexcept(true)
{
	cWPFKeyEvent KeyEvent(this,KeyEventArgs);
	if(Repeat==false){
		fKeyNotification.FilterKeyDown(&KeyEvent,KeyCode);
	}
	else{
		fKeyNotification.FilterKeyInput(&KeyEvent,KeyCode);
	}
}
//---------------------------------------------------------------------------
void cWPFUIView::WPFOnKeyDown(mcWPFKeyEventArgs &KeyEventArgs,eKeyCode KeyCode,bool Repeat)noexcept(true)
{
	cWPFKeyEvent KeyEvent(this,KeyEventArgs);

	if(Repeat==false){

		fKeyNotification.HandleKeyDown(&KeyEvent,KeyCode);

		// trigger one extea Key Input route event
		ParentNotifyKeyboardKeyInput(&KeyEvent,KeyCode);
	}

	fKeyNotification.HandleKeyInput(&KeyEvent,KeyCode);
}
//---------------------------------------------------------------------------
void cWPFUIView::WPFOnKeyUp(mcWPFKeyEventArgs &KeyEventArgs,eKeyCode KeyCode)noexcept(true)
{
	cWPFKeyEvent KeyEvent(this,KeyEventArgs);

	fKeyNotification.HandleKeyUp(&KeyEvent,KeyCode);
}
//---------------------------------------------------------------------------
void cWPFUIView::WPFOnKeyUpFilter(mcWPFKeyEventArgs &KeyEventArgs,eKeyCode KeyCode)noexcept(true)
{
	cWPFKeyEvent KeyEvent(this,KeyEventArgs);

	fKeyNotification.FilterKeyUp(&KeyEvent,KeyCode);
}
//---------------------------------------------------------------------------
void cWPFUIView::WPFOnIsMouseDirectlyOverChanged(bool InRange)noexcept(true)
{
	cWPFMouseEventByMessage MouseEvent(this);

	if(InRange){
		WPFNotifyMouseIsDirectlyEnter(&MouseEvent);
	}
	else{
		WPFNotifyMouseIsDirectlyLeave(&MouseEvent);
	}
}
//---------------------------------------------------------------------------
void cWPFUIView::WPFOnMouseEnter(mcWPFMouseEventArgs &MouseEventArgs)noexcept(true)
{
	cWPFMouseEvent MouseEvent(this,MouseEventArgs);

	WPFNotifyMouseEnterRange(&MouseEvent);
}
//---------------------------------------------------------------------------
void cWPFUIView::WPFOnMouseLeave(mcWPFMouseEventArgs &MouseEventArgs)noexcept(true)
{
	cWPFMouseEvent MouseEvent(this,MouseEventArgs);

	WPFNotifyMouseLeave(&MouseEvent);
}
//---------------------------------------------------------------------------
void cWPFUIView::WPFOnMouseMove(mcWPFMouseEventArgs &MouseEventArgs)noexcept(true)
{
	cWPFMouseEvent MouseEvent(this,MouseEventArgs);

	fMouseNotification.HandleMove(&MouseEvent);
}
//---------------------------------------------------------------------------
void cWPFUIView::WPFOnMouseMoveFilter(mcWPFMouseEventArgs &MouseEventArgs)noexcept(true)
{
	cWPFMouseEvent MouseEvent(this,MouseEventArgs);

	fMouseNotification.FilterMove(&MouseEvent);
}
//---------------------------------------------------------------------------
void cWPFUIView::WPFOnMouseDown(mcWPFMouseEventArgs &MouseEventArgs,eMouseButton Button)noexcept(true)
{
	cWPFMouseEvent MouseEvent(this,MouseEventArgs);

	fMouseNotification.HandleDown(&MouseEvent,Button);
}
//---------------------------------------------------------------------------
void cWPFUIView::WPFOnMouseDownFilter(mcWPFMouseEventArgs &MouseEventArgs,eMouseButton Button)noexcept(true)
{
	cWPFMouseEvent MouseEvent(this,MouseEventArgs);

	fMouseNotification.FilterDown(&MouseEvent,Button);
}
//---------------------------------------------------------------------------
void cWPFUIView::WPFOnMouseUp(mcWPFMouseEventArgs &MouseEventArgs,eMouseButton Button)noexcept(true)
{
	cWPFMouseEvent MouseEvent(this,MouseEventArgs);

	fMouseNotification.HandleUp(&MouseEvent,Button);
}
//---------------------------------------------------------------------------
void cWPFUIView::WPFOnMouseUpFilter(mcWPFMouseEventArgs &MouseEventArgs,eMouseButton Button)noexcept(true)
{
	cWPFMouseEvent MouseEvent(this,MouseEventArgs);

	fMouseNotification.FilterUp(&MouseEvent,Button);
}
//---------------------------------------------------------------------------
void cWPFUIView::WPFOnMouseWheel(mcWPFMouseEventArgs &MouseEventArgs,Float32 ScrollX,Float32 ScrollY)noexcept(true)
{
	cWPFMouseEvent MouseEvent(this,MouseEventArgs);

	fMouseNotification.HandleWheel(&MouseEvent,ScrollX,ScrollY);
}
//---------------------------------------------------------------------------
void cWPFUIView::WPFOnMouseWheelFilter(mcWPFMouseEventArgs &MouseEventArgs,Float32 ScrollX,Float32 ScrollY)noexcept(true)
{
	cWPFMouseEvent MouseEvent(this,MouseEventArgs);

	fMouseNotification.FilterWheel(&MouseEvent,ScrollX,ScrollY);
}
//---------------------------------------------------------------------------
void cWPFUIView::WPFOnTouchDown(mcWPFTouchEventArgs &TouchEventArgs)noexcept(true)
{
	cWPFTouchEvent TouchEvent(this,TouchEventArgs);

	fTouchNotification.HandleDown(&TouchEvent);
}
//---------------------------------------------------------------------------
void cWPFUIView::WPFOnTouchDownFilter(mcWPFTouchEventArgs &TouchEventArgs)noexcept(true)
{
	cWPFTouchEvent TouchEvent(this,TouchEventArgs);

	fTouchNotification.FilterDown(&TouchEvent);
}
//---------------------------------------------------------------------------
void cWPFUIView::WPFOnTouchUp(mcWPFTouchEventArgs &TouchEventArgs)noexcept(true)
{
	cWPFTouchEvent TouchEvent(this,TouchEventArgs);

	fTouchNotification.HandleUp(&TouchEvent);
}
//---------------------------------------------------------------------------
void cWPFUIView::WPFOnTouchUpFilter(mcWPFTouchEventArgs &TouchEventArgs)noexcept(true)
{
	cWPFTouchEvent TouchEvent(this,TouchEventArgs);

	fTouchNotification.FilterUp(&TouchEvent);
}
//---------------------------------------------------------------------------
void cWPFUIView::WPFOnTouchMove(mcWPFTouchEventArgs &TouchEventArgs)noexcept(true)
{
	cWPFTouchEvent TouchEvent(this,TouchEventArgs);

	fTouchNotification.HandleMove(&TouchEvent);
}
//---------------------------------------------------------------------------
void cWPFUIView::WPFOnTouchMoveFilter(mcWPFTouchEventArgs &TouchEventArgs)noexcept(true)
{
	cWPFTouchEvent TouchEvent(this,TouchEventArgs);

	fTouchNotification.FilterMove(&TouchEvent);
}
//---------------------------------------------------------------------------
