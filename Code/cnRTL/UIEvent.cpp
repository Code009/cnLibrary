#include "UIEvent.h"

using namespace cnLibrary;
using namespace cnRTL;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cUIStateHandlerSetNotifier::cUIStateHandlerSetNotifier(const cUIOrderedObjectSet<iUIStateHandler*,sfInt16> &Set)
	: fSet(Set)
{
}
//---------------------------------------------------------------------------
void cUIStateHandlerSetNotifier::UIStarted(void)
{
	auto Enum=fSet.Enumerator();
	if(Enum.Enum()) do{
		Enum.Get()->UIStarted();
	}while(Enum.Next());
}
//---------------------------------------------------------------------------
void cUIStateHandlerSetNotifier::UIShow(void)
{
	auto Enum=fSet.Enumerator();
	if(Enum.Enum()) do{
		Enum.Get()->UIShow();
	}while(Enum.Next());
}
//---------------------------------------------------------------------------
void cUIStateHandlerSetNotifier::UIResume(void)
{
	auto Enum=fSet.Enumerator();
	if(Enum.Enum()) do{
		Enum.Get()->UIResume();
	}while(Enum.Next());
}
//---------------------------------------------------------------------------
void cUIStateHandlerSetNotifier::UIPaused(void)
{
	auto Enum=fSet.Enumerator();
	if(Enum.Enum()) do{
		Enum.Get()->UIPaused();
	}while(Enum.Next());
}
//---------------------------------------------------------------------------
void cUIStateHandlerSetNotifier::UIHide(void)
{
	auto Enum=fSet.Enumerator();
	if(Enum.Enum()) do{
		Enum.Get()->UIHide();
	}while(Enum.Next());
}
//---------------------------------------------------------------------------
void cUIStateHandlerSetNotifier::UIStopped(void)
{
	auto Enum=fSet.Enumerator();
	if(Enum.Enum()) do{
		Enum.Get()->UIStopped();
	}while(Enum.Next());
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cUIWindowHandlerSetNotifier::cUIWindowHandlerSetNotifier(const cUIOrderedObjectSet<iUIWindowHandler*,sfInt16> &Set)
	: fSet(Set)
{
}
//---------------------------------------------------------------------------
void cUIWindowHandlerSetNotifier::ScreenChanged(void)
{
	auto Enum=fSet.Enumerator();
	if(Enum.Enum()) do{
		Enum.Get()->ScreenChanged();
	}while(Enum.Next());
}
//---------------------------------------------------------------------------
void cUIWindowHandlerSetNotifier::ParentChanged(void)
{
	auto Enum=fSet.Enumerator();
	if(Enum.Enum()) do{
		Enum.Get()->ParentChanged();
	}while(Enum.Next());
}
//---------------------------------------------------------------------------
void cUIWindowHandlerSetNotifier::ContentScaleChanged(void)
{
	auto Enum=fSet.Enumerator();
	if(Enum.Enum()) do{
		Enum.Get()->ContentScaleChanged();
	}while(Enum.Next());
}
//---------------------------------------------------------------------------
void cUIWindowHandlerSetNotifier::RectangleChanged(bool Moved,bool Sized)
{
	auto Enum=fSet.Enumerator();
	if(Enum.Enum()) do{
		Enum.Get()->RectangleChanged(Moved,Sized);
	}while(Enum.Next());
}
//---------------------------------------------------------------------------
void cUIWindowHandlerSetNotifier::ZPositionChanged(void)
{
	auto Enum=fSet.Enumerator();
	if(Enum.Enum()) do{
		Enum.Get()->ZPositionChanged();
	}while(Enum.Next());
}
//---------------------------------------------------------------------------
void cUIWindowHandlerSetNotifier::VisibleChanged(void)
{
	auto Enum=fSet.Enumerator();
	if(Enum.Enum()) do{
		Enum.Get()->VisibleChanged();
	}while(Enum.Next());
}
//---------------------------------------------------------------------------
void cUIWindowHandlerSetNotifier::EnableChanged(void)
{
	auto Enum=fSet.Enumerator();
	if(Enum.Enum()) do{
		Enum.Get()->EnableChanged();
	}while(Enum.Next());
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cUIViewHandlerSetNotifier::cUIViewHandlerSetNotifier(const cUIOrderedObjectSet<iUIViewHandler*,sfInt16> &Set)
	: fSet(Set)
{
}
//---------------------------------------------------------------------------
void cUIViewHandlerSetNotifier::UILayout(void)
{
	auto Enum=fSet.Enumerator();
	if(Enum.Enum()) do{
		Enum.Get()->UILayout();
	}while(Enum.Next());
}
//---------------------------------------------------------------------------
void cUIViewHandlerSetNotifier::UIMargin(cUIRectangle &Margin)
{
	auto Enum=fSet.Enumerator();
	if(Enum.Enum()) do{
		Enum.Get()->UIMargin(Margin);
	}while(Enum.Next());
}
//---------------------------------------------------------------------------
void cUIViewHandlerSetNotifier::WindowChanged(void)
{
	auto Enum=fSet.Enumerator();
	if(Enum.Enum()) do{
		Enum.Get()->WindowChanged();
	}while(Enum.Next());
}
//---------------------------------------------------------------------------
void cUIViewHandlerSetNotifier::ParentChanged(void)
{
	auto Enum=fSet.Enumerator();
	if(Enum.Enum()) do{
		Enum.Get()->ParentChanged();
	}while(Enum.Next());
}
//---------------------------------------------------------------------------
void cUIViewHandlerSetNotifier::ContentScaleChanged(void)
{
	auto Enum=fSet.Enumerator();
	if(Enum.Enum()) do{
		Enum.Get()->ContentScaleChanged();
	}while(Enum.Next());
}
//---------------------------------------------------------------------------
void cUIViewHandlerSetNotifier::RectangleChanged(bool Moved,bool Sized)
{
	auto Enum=fSet.Enumerator();
	if(Enum.Enum()) do{
		Enum.Get()->RectangleChanged(Moved,Sized);
	}while(Enum.Next());
}
//---------------------------------------------------------------------------
void cUIViewHandlerSetNotifier::ZPositionChanged(void)
{
	auto Enum=fSet.Enumerator();
	if(Enum.Enum()) do{
		Enum.Get()->ZPositionChanged();
	}while(Enum.Next());
}
//---------------------------------------------------------------------------
void cUIViewHandlerSetNotifier::VisibleChanged(void)
{
	auto Enum=fSet.Enumerator();
	if(Enum.Enum()) do{
		Enum.Get()->VisibleChanged();
	}while(Enum.Next());
}
//---------------------------------------------------------------------------
void cUIViewHandlerSetNotifier::EnableChanged(void)
{
	auto Enum=fSet.Enumerator();
	if(Enum.Enum()) do{
		Enum.Get()->EnableChanged();
	}while(Enum.Next());
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool cUIKeyEventNotification::InsertKeyHandler(iUIKeyHandler *Handler,sfInt16 Order)
{
	return fKeyHandlers.Insert(Handler,Order);
}
//---------------------------------------------------------------------------
bool cUIKeyEventNotification::RemoveKeyHandler(iUIKeyHandler *Handler)
{
	return fKeyHandlers.Remove(Handler);
}
//---------------------------------------------------------------------------
bool cUIKeyEventNotification::InsertKeyFilter(iUIKeyHandler *Filter,sfInt16 Order)
{
	return fKeyFilters.Insert(Filter,Order);
}
//---------------------------------------------------------------------------
bool cUIKeyEventNotification::RemoveKeyFilter(iUIKeyHandler *Filter)
{
	return fKeyFilters.Remove(Filter);
}
//---------------------------------------------------------------------------
bool cUIKeyEventNotification::KeyAcquireExclusive(iUIKeyHandler *Handler)
{
	return false;
}
//---------------------------------------------------------------------------
void cUIKeyEventNotification::KeyReleaseExclusive(iUIKeyHandler *Handler)
{
}
//---------------------------------------------------------------------------
bool cUIKeyEventNotification::InsertTextInputHandler(iUITextInputHandler *Handler,sfInt16 Order)
{
	return fTextInputHandlers.Insert(Handler,Order);
}
//---------------------------------------------------------------------------
bool cUIKeyEventNotification::RemoveTextInputHandler(iUITextInputHandler *Handler)
{
	return fTextInputHandlers.Remove(Handler);
}
//---------------------------------------------------------------------------
void cUIKeyEventNotification::FocusEnterRange(iUIKeyEvent *KeyEvent)
{
	for(auto *Handler : fKeyFilters){
		Handler->KeyFocusEnterRange(KeyEvent);
	}
	for(auto *Handler : fKeyHandlers){
		Handler->KeyFocusEnterRange(KeyEvent);
	}
}
//---------------------------------------------------------------------------
void cUIKeyEventNotification::FocusLeaveRange(iUIKeyEvent *KeyEvent)
{
	for(auto *Handler : fKeyFilters){
		Handler->KeyFocusLeaveRange(KeyEvent);
	}
	for(auto *Handler : fKeyHandlers){
		Handler->KeyFocusLeaveRange(KeyEvent);
	}
}
//---------------------------------------------------------------------------
void cUIKeyEventNotification::HandleFocusEnter(iUIKeyEvent *KeyEvent)
{
	for(auto *Handler : fKeyHandlers){
		Handler->KeyFocusEnter(KeyEvent);
	}
}
//---------------------------------------------------------------------------
void cUIKeyEventNotification::HandleFocusLeave(iUIKeyEvent *KeyEvent)
{
	for(auto *Handler : fKeyHandlers){
		Handler->KeyFocusLeave(KeyEvent);
	}
}
//---------------------------------------------------------------------------
void cUIKeyEventNotification::HandleKeyDown(iUIKeyEvent *KeyEvent,eKeyCode Key)
{
	for(auto *Handler : fKeyHandlers){
		Handler->KeyDown(KeyEvent,Key);
		if(KeyEvent->IsCancelled())
			break;
	}
}
//---------------------------------------------------------------------------
void cUIKeyEventNotification::HandleKeyUp(iUIKeyEvent *KeyEvent,eKeyCode Key)
{
	for(auto *Handler : fKeyHandlers){
		Handler->KeyUp(KeyEvent,Key);
		if(KeyEvent->IsCancelled())
			break;
	}
}
//---------------------------------------------------------------------------
void cUIKeyEventNotification::HandleKeyInput(iUIKeyEvent *KeyEvent,eKeyCode Key)
{
	for(auto *Handler : fKeyHandlers){
		Handler->KeyInput(KeyEvent,Key);
		if(KeyEvent->IsCancelled())
			break;
	}
}
//---------------------------------------------------------------------------
void cUIKeyEventNotification::FilterFocusEnter(iUIKeyEvent *KeyEvent)
{
	for(auto *Handler : fKeyFilters){
		Handler->KeyFocusEnter(KeyEvent);
	}
}
//---------------------------------------------------------------------------
void cUIKeyEventNotification::FilterFocusLeave(iUIKeyEvent *KeyEvent)
{
	for(auto *Handler : fKeyFilters){
		Handler->KeyFocusLeave(KeyEvent);
	}
}
//---------------------------------------------------------------------------
void cUIKeyEventNotification::FilterKeyDown(iUIKeyEvent *KeyEvent,eKeyCode Key)
{
	for(auto *Handler : fKeyFilters){
		Handler->KeyDown(KeyEvent,Key);
		if(KeyEvent->IsCancelled())
			break;
	}
}
//---------------------------------------------------------------------------
void cUIKeyEventNotification::FilterKeyUp(iUIKeyEvent *KeyEvent,eKeyCode Key)
{
	for(auto *Handler : fKeyFilters){
		Handler->KeyUp(KeyEvent,Key);
		if(KeyEvent->IsCancelled())
			break;
	}
}
//---------------------------------------------------------------------------
void cUIKeyEventNotification::FilterKeyInput(iUIKeyEvent *KeyEvent,eKeyCode Key)
{
	for(auto *Handler : fKeyFilters){
		Handler->KeyInput(KeyEvent,Key);
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool cUIMouseEventNotification::InsertMouseHandler(iUIMouseHandler *Handler,sfInt16 Order)
{
	return fMouseHandlers.Insert(Handler,Order);
}
//---------------------------------------------------------------------------
bool cUIMouseEventNotification::RemoveMouseHandler(iUIMouseHandler *Handler)
{
	return fMouseHandlers.Remove(Handler);
}
//---------------------------------------------------------------------------
bool cUIMouseEventNotification::InsertMouseFilter(iUIMouseHandler *Filter,sfInt16 Order)
{
	return fMouseFilters.Insert(Filter,Order);
}
//---------------------------------------------------------------------------
bool cUIMouseEventNotification::RemoveMouseFilter(iUIMouseHandler *Filter)
{
	return fMouseFilters.Remove(Filter);
}
//---------------------------------------------------------------------------
bool cUIMouseEventNotification::MouseAcquireExclusive(iUIMouseHandler *Handler)
{
	return false;
}
//---------------------------------------------------------------------------
void cUIMouseEventNotification::MouseReleaseExclusive(iUIMouseHandler *Handler)
{
}
//---------------------------------------------------------------------------
void cUIMouseEventNotification::MouseEnterRange(iUIMouseEvent *MouseEvent)
{
	for(auto *Handler : fMouseFilters){
		Handler->MouseEnterRange(MouseEvent);
	}
	for(auto *Handler : fMouseHandlers){
		Handler->MouseEnterRange(MouseEvent);
	}
}
//---------------------------------------------------------------------------
void cUIMouseEventNotification::MouseLeaveRange(iUIMouseEvent *MouseEvent)
{
	for(auto *Handler : fMouseFilters){
		Handler->MouseLeaveRange(MouseEvent);
	}
	for(auto *Handler : fMouseHandlers){
		Handler->MouseLeaveRange(MouseEvent);
	}
}
//---------------------------------------------------------------------------
void cUIMouseEventNotification::HandleEnter(iUIMouseEvent *MouseEvent)
{
	for(auto *Handler : fMouseHandlers){
		Handler->MouseEnter(MouseEvent);
	}
}
//---------------------------------------------------------------------------
void cUIMouseEventNotification::HandleLeave(iUIMouseEvent *MouseEvent)
{
	for(auto *Handler : fMouseHandlers){
		Handler->MouseLeave(MouseEvent);
	}
}
//---------------------------------------------------------------------------
void cUIMouseEventNotification::HandleMove(iUIMouseEvent *MouseEvent)
{
	for(auto *Handler : fMouseHandlers){
		Handler->MouseMove(MouseEvent);
		if(MouseEvent->IsCancelled())
			break;
	}
}
//---------------------------------------------------------------------------
void cUIMouseEventNotification::HandleDown(iUIMouseEvent *MouseEvent,eMouseButton Button)
{
	for(auto *Handler : fMouseHandlers){
		Handler->MouseDown(MouseEvent,Button);
		if(MouseEvent->IsCancelled())
			break;
	}
}
//---------------------------------------------------------------------------
void cUIMouseEventNotification::HandleUp(iUIMouseEvent *MouseEvent,eMouseButton Button)
{
	for(auto *Handler : fMouseHandlers){
		Handler->MouseUp(MouseEvent,Button);
		if(MouseEvent->IsCancelled())
			break;
	}
}
//---------------------------------------------------------------------------
void cUIMouseEventNotification::HandleWheel(iUIMouseEvent *MouseEvent,Float32 ScrollX,Float32 ScrollY)
{
	for(auto *Handler : fMouseHandlers){
		Handler->MouseWheel(MouseEvent,ScrollX,ScrollY);
		if(MouseEvent->IsCancelled())
			break;
	}
}
//---------------------------------------------------------------------------
void cUIMouseEventNotification::FilterEnter(iUIMouseEvent *MouseEvent)
{
	for(auto *Handler : fMouseFilters){
		Handler->MouseEnter(MouseEvent);
	}
}
//---------------------------------------------------------------------------
void cUIMouseEventNotification::FilterLeave(iUIMouseEvent *MouseEvent)
{
	for(auto *Handler : fMouseFilters){
		Handler->MouseLeave(MouseEvent);
	}
}
//---------------------------------------------------------------------------
void cUIMouseEventNotification::FilterMove(iUIMouseEvent *MouseEvent)
{
	for(auto *Handler : fMouseFilters){
		Handler->MouseMove(MouseEvent);
		if(MouseEvent->IsCancelled())
			break;
	}
}
//---------------------------------------------------------------------------
void cUIMouseEventNotification::FilterDown(iUIMouseEvent *MouseEvent,eMouseButton Button)
{
	for(auto *Handler : fMouseFilters){
		Handler->MouseDown(MouseEvent,Button);
		if(MouseEvent->IsCancelled())
			break;
	}
}
//---------------------------------------------------------------------------
void cUIMouseEventNotification::FilterUp(iUIMouseEvent *MouseEvent,eMouseButton Button)
{
	for(auto *Handler : fMouseFilters){
		Handler->MouseUp(MouseEvent,Button);
		if(MouseEvent->IsCancelled())
			break;
	}
}
//---------------------------------------------------------------------------
void cUIMouseEventNotification::FilterWheel(iUIMouseEvent *MouseEvent,Float32 ScrollX,Float32 ScrollY)
{
	for(auto *Handler : fMouseFilters){
		Handler->MouseWheel(MouseEvent,ScrollX,ScrollY);
		if(MouseEvent->IsCancelled())
			break;
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool cUITouchEventNotification::InsertTouchHandler(iUITouchHandler *Handler,sfInt16 Order)
{
	return fTouchHandlers.Insert(Handler,Order);
}
//---------------------------------------------------------------------------
bool cUITouchEventNotification::RemoveTouchHandler(iUITouchHandler *Handler)
{
	return fTouchHandlers.Remove(Handler);
}
//---------------------------------------------------------------------------
bool cUITouchEventNotification::InsertTouchFilter(iUITouchHandler *Filter,sfInt16 Order)
{
	return fTouchFilters.Insert(Filter,Order);
}
//---------------------------------------------------------------------------
bool cUITouchEventNotification::RemoveTouchFilter(iUITouchHandler *Filter)
{
	return fTouchFilters.Remove(Filter);
}
//---------------------------------------------------------------------------
bool cUITouchEventNotification::TouchAcquireExclusive(iUITouchHandler *Handler)
{
	return false;
}
//---------------------------------------------------------------------------
void cUITouchEventNotification::TouchReleaseExclusive(iUITouchHandler *Handler)
{
}
//---------------------------------------------------------------------------
void cUITouchEventNotification::HandleBegin(iUITouchEvent *TouchEvent)
{
	for(auto *Handler : fTouchHandlers){
		Handler->TouchBegin(TouchEvent);
	}
}
//---------------------------------------------------------------------------
void cUITouchEventNotification::HandleEnd(iUITouchEvent *TouchEvent)
{
	for(auto *Handler : fTouchHandlers){
		Handler->TouchEnd(TouchEvent);
	}
}
//---------------------------------------------------------------------------
void cUITouchEventNotification::HandleLost(iUITouchEvent *TouchEvent)
{
	for(auto *Handler : fTouchHandlers){
		Handler->TouchLost(TouchEvent);
	}
}
//---------------------------------------------------------------------------
void cUITouchEventNotification::HandleMove(iUITouchEvent *TouchEvent)
{
	for(auto *Handler : fTouchHandlers){
		Handler->TouchMove(TouchEvent);
		if(TouchEvent->IsCancelled())
			break;
	}
}
//---------------------------------------------------------------------------
void cUITouchEventNotification::FilterBegin(iUITouchEvent *TouchEvent)
{
	for(auto *Handler : fTouchFilters){
		Handler->TouchBegin(TouchEvent);
	}
}
//---------------------------------------------------------------------------
void cUITouchEventNotification::FilterEnd(iUITouchEvent *TouchEvent)
{
	for(auto *Handler : fTouchFilters){
		Handler->TouchEnd(TouchEvent);
	}
}
//---------------------------------------------------------------------------
void cUITouchEventNotification::FilterLost(iUITouchEvent *TouchEvent)
{
	for(auto *Handler : fTouchFilters){
		Handler->TouchLost(TouchEvent);
	}
}
//---------------------------------------------------------------------------
void cUITouchEventNotification::FilterMove(iUITouchEvent *TouchEvent)
{
	for(auto *Handler : fTouchFilters){
		Handler->TouchMove(TouchEvent);
		if(TouchEvent->IsCancelled())
			break;
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool cUIViewEventNotification::InsertStateHandler(iUIStateHandler *Handler,sfInt16 Order)
{
	return fStateHandlers.Insert(Handler,Order);
}
//---------------------------------------------------------------------------
bool cUIViewEventNotification::RemoveStateHandler(iUIStateHandler *Handler)
{
	return fStateHandlers.Remove(Handler);
}
//---------------------------------------------------------------------------
bool cUIViewEventNotification::InsertViewHandler(iUIViewHandler *Handler,sfInt16 Order)
{
	return fViewHandlers.Insert(Handler,Order);
}
//---------------------------------------------------------------------------
bool cUIViewEventNotification::RemoveViewHandler(iUIViewHandler *Handler)
{
	return fViewHandlers.Remove(Handler);
}
//---------------------------------------------------------------------------
void cUIViewEventNotification::StateChange(eUIState PrevState,eUIState NextState)
{
	cUIStateHandlerSetNotifier Notifier(fStateHandlers);

	UIStateNotifyChange(PrevState,NextState,Notifier);
}
//---------------------------------------------------------------------------
void cUIViewEventNotification::ContentScaleChanged(void)
{
	for(auto *Handler : fViewHandlers){
		Handler->ContentScaleChanged();
	}
}
//---------------------------------------------------------------------------
void cUIViewEventNotification::RectangleChanged(bool Moved,bool Sized)
{
	for(auto *Handler : fViewHandlers){
		Handler->RectangleChanged(Moved,Sized);
	}
}
//---------------------------------------------------------------------------
void cUIViewEventNotification::VisibleChanged(void)
{
	for(auto *Handler : fViewHandlers){
		Handler->VisibleChanged();
	}
}
//---------------------------------------------------------------------------
void cUIViewEventNotification::EnableChanged(void)
{
	for(auto *Handler : fViewHandlers){
		Handler->EnableChanged();
	}
}
//---------------------------------------------------------------------------
void cUIViewEventNotification::Layout(void)
{
	for(auto *Handler : fViewHandlers){
		Handler->UILayout();
	}
}
//---------------------------------------------------------------------------
cUIRectangle cUIViewEventNotification::UIMargin(const cUIRectangle &Margin)
{
	union{
		cVector<Float32,4> RetVector;
		cUIRectangle RetMargin;
	};
	union{
		cVector<Float32,4> HandlerVector;
		cUIRectangle HandlerMargin;
	};
	RetMargin=Margin;
	for(auto *Handler : fViewHandlers){
		HandlerMargin=Handler->UIMargin(RetMargin);
		RetVector+=HandlerVector;
	}
	return RetMargin;
}
//---------------------------------------------------------------------------
void cUIViewEventNotification::WindowChanged(void)
{
	for(auto *Handler : fViewHandlers){
		Handler->WindowChanged();
	}
}
//---------------------------------------------------------------------------
void cUIViewEventNotification::ParentChanged(void)
{
	for(auto *Handler : fViewHandlers){
		Handler->ParentChanged();
	}
}
//---------------------------------------------------------------------------
#if 0
//---------------------------------------------------------------------------
bcOrderedReferenceSet::bcOrderedReferenceSet()
{
}
//---------------------------------------------------------------------------
bcOrderedReferenceSet::~bcOrderedReferenceSet()
{
}
//---------------------------------------------------------------------------
bool bcOrderedReferenceSet::Exists(iReference *Object)
{
	auto Pair=fPtrOrderMap.GetPair(Object);
	return Pair!=nullptr;
}
//---------------------------------------------------------------------------
bool bcOrderedReferenceSet::Insert(iReference *Object,sIntn Order)
{
	bool Exists;
	auto Pair=fPtrOrderMap.InsertPair(Object,Exists);
	cOrderItem OrderItem;
	if(Exists){
		if(Pair->Value==Order){
			// already exists
			return false;
		}
		// reorder
		OrderItem.Pointer=Object;

		// remove previous order
		OrderItem.Order=Pair->Value;
		fPtrSet.Remove(OrderItem);
	}
	else{
		OrderItem.Pointer=Object;
	}
	// insert with new order
	Pair->Value=Order;
	OrderItem.Order=Order;
	fPtrSet.Insert(OrderItem);
	return true;
}
//---------------------------------------------------------------------------
bool bcOrderedReferenceSet::Remove(iReference *Object)
{
	auto Pair=fPtrOrderMap.GetPair(Object);
	if(Pair==nullptr)
		return false;

	cOrderItem OrderItem;
	OrderItem.Pointer=Object;
	OrderItem.Order=Pair->Value;
	fPtrSet.Remove(OrderItem);

	return true;
}
//---------------------------------------------------------------------------
bool bcOrderedReferenceSet::cOrderItem::operator ==(tNullptr)const{	return Pointer==nullptr;	}
bool bcOrderedReferenceSet::cOrderItem::operator !=(tNullptr)const{	return Pointer!=nullptr;	}
bool bcOrderedReferenceSet::cOrderItem::operator ==(const cOrderItem &Src)const{	return Order==Src.Order && Pointer==Src.Pointer;	}
bool bcOrderedReferenceSet::cOrderItem::operator !=(const cOrderItem &Src)const{	return Order!=Src.Order || Pointer!=Src.Pointer;	}
bool bcOrderedReferenceSet::cOrderItem::operator <(const cOrderItem &Src)const{	return Order!=Src.Order	? Order<Src.Order : Pointer<Src.Pointer;	}
bool bcOrderedReferenceSet::cOrderItem::operator <=(const cOrderItem &Src)const{	return Order!=Src.Order	? Order<Src.Order : Pointer<=Src.Pointer;	}
bool bcOrderedReferenceSet::cOrderItem::operator >(const cOrderItem &Src)const{	return Order!=Src.Order	? Order>Src.Order : Pointer>Src.Pointer;	}
bool bcOrderedReferenceSet::cOrderItem::operator >=(const cOrderItem &Src)const{	return Order!=Src.Order	? Order>Src.Order : Pointer>=Src.Pointer;	}
//---------------------------------------------------------------------------
#endif // 0
