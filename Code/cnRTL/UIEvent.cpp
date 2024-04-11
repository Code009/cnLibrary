#include "UIEvent.h"

using namespace cnLibrary;
using namespace cnRTL;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cUIStateHandlerSetNotifier::cUIStateHandlerSetNotifier(const cUIOrderedObjectSet<iUIStateHandler*,sfInt16> &Set)noexcept
	: fSet(Set)
{
}
//---------------------------------------------------------------------------
void cUIStateHandlerSetNotifier::UIStarted(void)noexcept
{
	auto Enum=fSet.Enumerator();
	if(Enum.Enum()) do{
		Enum.Get()->UIStarted();
	}while(Enum.Next());
}
//---------------------------------------------------------------------------
void cUIStateHandlerSetNotifier::UIShow(void)noexcept
{
	auto Enum=fSet.Enumerator();
	if(Enum.Enum()) do{
		Enum.Get()->UIShow();
	}while(Enum.Next());
}
//---------------------------------------------------------------------------
void cUIStateHandlerSetNotifier::UIResume(void)noexcept
{
	auto Enum=fSet.Enumerator();
	if(Enum.Enum()) do{
		Enum.Get()->UIResume();
	}while(Enum.Next());
}
//---------------------------------------------------------------------------
void cUIStateHandlerSetNotifier::UIPaused(void)noexcept
{
	auto Enum=fSet.Enumerator();
	if(Enum.Enum()) do{
		Enum.Get()->UIPaused();
	}while(Enum.Next());
}
//---------------------------------------------------------------------------
void cUIStateHandlerSetNotifier::UIHide(void)noexcept
{
	auto Enum=fSet.Enumerator();
	if(Enum.Enum()) do{
		Enum.Get()->UIHide();
	}while(Enum.Next());
}
//---------------------------------------------------------------------------
void cUIStateHandlerSetNotifier::UIStopped(void)noexcept
{
	auto Enum=fSet.Enumerator();
	if(Enum.Enum()) do{
		Enum.Get()->UIStopped();
	}while(Enum.Next());
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cUIWindowHandlerSetNotifier::cUIWindowHandlerSetNotifier(const cUIOrderedObjectSet<iUIWindowHandler*,sfInt16> &Set)noexcept
	: fSet(Set)
{
}
//---------------------------------------------------------------------------
void cUIWindowHandlerSetNotifier::ScreenChanged(void)noexcept
{
	auto Enum=fSet.Enumerator();
	if(Enum.Enum()) do{
		Enum.Get()->ScreenChanged();
	}while(Enum.Next());
}
//---------------------------------------------------------------------------
void cUIWindowHandlerSetNotifier::ParentChanged(void)noexcept
{
	auto Enum=fSet.Enumerator();
	if(Enum.Enum()) do{
		Enum.Get()->ParentChanged();
	}while(Enum.Next());
}
//---------------------------------------------------------------------------
void cUIWindowHandlerSetNotifier::ContentScaleChanged(void)noexcept
{
	auto Enum=fSet.Enumerator();
	if(Enum.Enum()) do{
		Enum.Get()->ContentScaleChanged();
	}while(Enum.Next());
}
//---------------------------------------------------------------------------
void cUIWindowHandlerSetNotifier::RectangleChanged(bool Moved,bool Sized)noexcept
{
	auto Enum=fSet.Enumerator();
	if(Enum.Enum()) do{
		Enum.Get()->RectangleChanged(Moved,Sized);
	}while(Enum.Next());
}
//---------------------------------------------------------------------------
void cUIWindowHandlerSetNotifier::ZPositionChanged(void)noexcept
{
	auto Enum=fSet.Enumerator();
	if(Enum.Enum()) do{
		Enum.Get()->ZPositionChanged();
	}while(Enum.Next());
}
//---------------------------------------------------------------------------
void cUIWindowHandlerSetNotifier::VisibleChanged(void)noexcept
{
	auto Enum=fSet.Enumerator();
	if(Enum.Enum()) do{
		Enum.Get()->VisibleChanged();
	}while(Enum.Next());
}
//---------------------------------------------------------------------------
void cUIWindowHandlerSetNotifier::EnableChanged(void)noexcept
{
	auto Enum=fSet.Enumerator();
	if(Enum.Enum()) do{
		Enum.Get()->EnableChanged();
	}while(Enum.Next());
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cUIViewHandlerSetNotifier::cUIViewHandlerSetNotifier(const cUIOrderedObjectSet<iUIViewHandler*,sfInt16> &Set)noexcept
	: fSet(Set)
{
}
//---------------------------------------------------------------------------
void cUIViewHandlerSetNotifier::UILayout(void)noexcept
{
	auto Enum=fSet.Enumerator();
	if(Enum.Enum()) do{
		Enum.Get()->UILayout();
	}while(Enum.Next());
}
//---------------------------------------------------------------------------
void cUIViewHandlerSetNotifier::UIMargin(cUIRectangle &Margin)noexcept
{
	auto Enum=fSet.Enumerator();
	if(Enum.Enum()) do{
		Enum.Get()->UIMargin(Margin);
	}while(Enum.Next());
}
//---------------------------------------------------------------------------
void cUIViewHandlerSetNotifier::WindowChanged(void)noexcept
{
	auto Enum=fSet.Enumerator();
	if(Enum.Enum()) do{
		Enum.Get()->WindowChanged();
	}while(Enum.Next());
}
//---------------------------------------------------------------------------
void cUIViewHandlerSetNotifier::ParentChanged(void)noexcept
{
	auto Enum=fSet.Enumerator();
	if(Enum.Enum()) do{
		Enum.Get()->ParentChanged();
	}while(Enum.Next());
}
//---------------------------------------------------------------------------
void cUIViewHandlerSetNotifier::ContentScaleChanged(void)noexcept
{
	auto Enum=fSet.Enumerator();
	if(Enum.Enum()) do{
		Enum.Get()->ContentScaleChanged();
	}while(Enum.Next());
}
//---------------------------------------------------------------------------
void cUIViewHandlerSetNotifier::RectangleChanged(bool Moved,bool Sized)noexcept
{
	auto Enum=fSet.Enumerator();
	if(Enum.Enum()) do{
		Enum.Get()->RectangleChanged(Moved,Sized);
	}while(Enum.Next());
}
//---------------------------------------------------------------------------
void cUIViewHandlerSetNotifier::ZPositionChanged(void)noexcept
{
	auto Enum=fSet.Enumerator();
	if(Enum.Enum()) do{
		Enum.Get()->ZPositionChanged();
	}while(Enum.Next());
}
//---------------------------------------------------------------------------
void cUIViewHandlerSetNotifier::VisibleChanged(void)noexcept
{
	auto Enum=fSet.Enumerator();
	if(Enum.Enum()) do{
		Enum.Get()->VisibleChanged();
	}while(Enum.Next());
}
//---------------------------------------------------------------------------
void cUIViewHandlerSetNotifier::EnableChanged(void)noexcept
{
	auto Enum=fSet.Enumerator();
	if(Enum.Enum()) do{
		Enum.Get()->EnableChanged();
	}while(Enum.Next());
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool cUIKeyEventNotification::InsertKeyHandler(iUIKeyHandler *Handler,sfInt16 Order)noexcept
{
	return fKeyHandlers.Insert(Handler,Order);
}
//---------------------------------------------------------------------------
bool cUIKeyEventNotification::RemoveKeyHandler(iUIKeyHandler *Handler)noexcept
{
	return fKeyHandlers.Remove(Handler);
}
//---------------------------------------------------------------------------
bool cUIKeyEventNotification::InsertKeyFilter(iUIKeyHandler *Filter,sfInt16 Order)noexcept
{
	return fKeyFilters.Insert(Filter,Order);
}
//---------------------------------------------------------------------------
bool cUIKeyEventNotification::RemoveKeyFilter(iUIKeyHandler *Filter)noexcept
{
	return fKeyFilters.Remove(Filter);
}
//---------------------------------------------------------------------------
bool cUIKeyEventNotification::KeyAcquireExclusive(iUIKeyHandler *Handler)noexcept
{
	return false;
}
//---------------------------------------------------------------------------
void cUIKeyEventNotification::KeyReleaseExclusive(iUIKeyHandler *Handler)noexcept
{
}
//---------------------------------------------------------------------------
bool cUIKeyEventNotification::InsertTextInputHandler(iUITextInputHandler *Handler,sfInt16 Order)noexcept
{
	return fTextInputHandlers.Insert(Handler,Order);
}
//---------------------------------------------------------------------------
bool cUIKeyEventNotification::RemoveTextInputHandler(iUITextInputHandler *Handler)noexcept
{
	return fTextInputHandlers.Remove(Handler);
}
//---------------------------------------------------------------------------
void cUIKeyEventNotification::FocusEnterRange(iUIKeyEvent *KeyEvent)noexcept
{
	for(auto *Handler : fKeyFilters){
		Handler->KeyFocusEnterRange(KeyEvent);
	}
	for(auto *Handler : fKeyHandlers){
		Handler->KeyFocusEnterRange(KeyEvent);
	}
}
//---------------------------------------------------------------------------
void cUIKeyEventNotification::FocusLeaveRange(iUIKeyEvent *KeyEvent)noexcept
{
	for(auto *Handler : fKeyFilters){
		Handler->KeyFocusLeaveRange(KeyEvent);
	}
	for(auto *Handler : fKeyHandlers){
		Handler->KeyFocusLeaveRange(KeyEvent);
	}
}
//---------------------------------------------------------------------------
void cUIKeyEventNotification::HandleFocusEnter(iUIKeyEvent *KeyEvent)noexcept
{
	for(auto *Handler : fKeyHandlers){
		Handler->KeyFocusEnter(KeyEvent);
	}
}
//---------------------------------------------------------------------------
void cUIKeyEventNotification::HandleFocusLeave(iUIKeyEvent *KeyEvent)noexcept
{
	for(auto *Handler : fKeyHandlers){
		Handler->KeyFocusLeave(KeyEvent);
	}
}
//---------------------------------------------------------------------------
void cUIKeyEventNotification::HandleKeyDown(iUIKeyEvent *KeyEvent,eKeyCode Key)noexcept
{
	for(auto *Handler : fKeyHandlers){
		Handler->KeyDown(KeyEvent,Key);
		if(KeyEvent->IsCancelled())
			break;
	}
}
//---------------------------------------------------------------------------
void cUIKeyEventNotification::HandleKeyUp(iUIKeyEvent *KeyEvent,eKeyCode Key)noexcept
{
	for(auto *Handler : fKeyHandlers){
		Handler->KeyUp(KeyEvent,Key);
		if(KeyEvent->IsCancelled())
			break;
	}
}
//---------------------------------------------------------------------------
void cUIKeyEventNotification::HandleKeyInput(iUIKeyEvent *KeyEvent,eKeyCode Key)noexcept
{
	for(auto *Handler : fKeyHandlers){
		Handler->KeyInput(KeyEvent,Key);
		if(KeyEvent->IsCancelled())
			break;
	}
}
//---------------------------------------------------------------------------
void cUIKeyEventNotification::FilterFocusEnter(iUIKeyEvent *KeyEvent)noexcept
{
	for(auto *Handler : fKeyFilters){
		Handler->KeyFocusEnter(KeyEvent);
	}
}
//---------------------------------------------------------------------------
void cUIKeyEventNotification::FilterFocusLeave(iUIKeyEvent *KeyEvent)noexcept
{
	for(auto *Handler : fKeyFilters){
		Handler->KeyFocusLeave(KeyEvent);
	}
}
//---------------------------------------------------------------------------
void cUIKeyEventNotification::FilterKeyDown(iUIKeyEvent *KeyEvent,eKeyCode Key)noexcept
{
	for(auto *Handler : fKeyFilters){
		Handler->KeyDown(KeyEvent,Key);
		if(KeyEvent->IsCancelled())
			break;
	}
}
//---------------------------------------------------------------------------
void cUIKeyEventNotification::FilterKeyUp(iUIKeyEvent *KeyEvent,eKeyCode Key)noexcept
{
	for(auto *Handler : fKeyFilters){
		Handler->KeyUp(KeyEvent,Key);
		if(KeyEvent->IsCancelled())
			break;
	}
}
//---------------------------------------------------------------------------
void cUIKeyEventNotification::FilterKeyInput(iUIKeyEvent *KeyEvent,eKeyCode Key)noexcept
{
	for(auto *Handler : fKeyFilters){
		Handler->KeyInput(KeyEvent,Key);
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool cUIMouseEventNotification::InsertMouseHandler(iUIMouseHandler *Handler,sfInt16 Order)noexcept
{
	return fMouseHandlers.Insert(Handler,Order);
}
//---------------------------------------------------------------------------
bool cUIMouseEventNotification::RemoveMouseHandler(iUIMouseHandler *Handler)noexcept
{
	return fMouseHandlers.Remove(Handler);
}
//---------------------------------------------------------------------------
bool cUIMouseEventNotification::InsertMouseFilter(iUIMouseHandler *Filter,sfInt16 Order)noexcept
{
	return fMouseFilters.Insert(Filter,Order);
}
//---------------------------------------------------------------------------
bool cUIMouseEventNotification::RemoveMouseFilter(iUIMouseHandler *Filter)noexcept
{
	return fMouseFilters.Remove(Filter);
}
//---------------------------------------------------------------------------
bool cUIMouseEventNotification::MouseAcquireExclusive(iUIMouseHandler *Handler)noexcept
{
	return false;
}
//---------------------------------------------------------------------------
void cUIMouseEventNotification::MouseReleaseExclusive(iUIMouseHandler *Handler)noexcept
{
}
//---------------------------------------------------------------------------
void cUIMouseEventNotification::MouseEnterRange(iUIMouseEvent *MouseEvent)noexcept
{
	for(auto *Handler : fMouseFilters){
		Handler->MouseEnterRange(MouseEvent);
	}
	for(auto *Handler : fMouseHandlers){
		Handler->MouseEnterRange(MouseEvent);
	}
}
//---------------------------------------------------------------------------
void cUIMouseEventNotification::MouseLeaveRange(iUIMouseEvent *MouseEvent)noexcept
{
	for(auto *Handler : fMouseFilters){
		Handler->MouseLeaveRange(MouseEvent);
	}
	for(auto *Handler : fMouseHandlers){
		Handler->MouseLeaveRange(MouseEvent);
	}
}
//---------------------------------------------------------------------------
void cUIMouseEventNotification::HandleEnter(iUIMouseEvent *MouseEvent)noexcept
{
	for(auto *Handler : fMouseHandlers){
		Handler->MouseEnter(MouseEvent);
	}
}
//---------------------------------------------------------------------------
void cUIMouseEventNotification::HandleLeave(iUIMouseEvent *MouseEvent)noexcept
{
	for(auto *Handler : fMouseHandlers){
		Handler->MouseLeave(MouseEvent);
	}
}
//---------------------------------------------------------------------------
void cUIMouseEventNotification::HandleMove(iUIMouseEvent *MouseEvent)noexcept
{
	for(auto *Handler : fMouseHandlers){
		Handler->MouseMove(MouseEvent);
		if(MouseEvent->IsCancelled())
			break;
	}
}
//---------------------------------------------------------------------------
void cUIMouseEventNotification::HandleDown(iUIMouseEvent *MouseEvent,eMouseButton Button)noexcept
{
	for(auto *Handler : fMouseHandlers){
		Handler->MouseDown(MouseEvent,Button);
		if(MouseEvent->IsCancelled())
			break;
	}
}
//---------------------------------------------------------------------------
void cUIMouseEventNotification::HandleUp(iUIMouseEvent *MouseEvent,eMouseButton Button)noexcept
{
	for(auto *Handler : fMouseHandlers){
		Handler->MouseUp(MouseEvent,Button);
		if(MouseEvent->IsCancelled())
			break;
	}
}
//---------------------------------------------------------------------------
void cUIMouseEventNotification::HandleWheel(iUIMouseEvent *MouseEvent,Float32 ScrollX,Float32 ScrollY)noexcept
{
	for(auto *Handler : fMouseHandlers){
		Handler->MouseWheel(MouseEvent,ScrollX,ScrollY);
		if(MouseEvent->IsCancelled())
			break;
	}
}
//---------------------------------------------------------------------------
void cUIMouseEventNotification::FilterEnter(iUIMouseEvent *MouseEvent)noexcept
{
	for(auto *Handler : fMouseFilters){
		Handler->MouseEnter(MouseEvent);
	}
}
//---------------------------------------------------------------------------
void cUIMouseEventNotification::FilterLeave(iUIMouseEvent *MouseEvent)noexcept
{
	for(auto *Handler : fMouseFilters){
		Handler->MouseLeave(MouseEvent);
	}
}
//---------------------------------------------------------------------------
void cUIMouseEventNotification::FilterMove(iUIMouseEvent *MouseEvent)noexcept
{
	for(auto *Handler : fMouseFilters){
		Handler->MouseMove(MouseEvent);
		if(MouseEvent->IsCancelled())
			break;
	}
}
//---------------------------------------------------------------------------
void cUIMouseEventNotification::FilterDown(iUIMouseEvent *MouseEvent,eMouseButton Button)noexcept
{
	for(auto *Handler : fMouseFilters){
		Handler->MouseDown(MouseEvent,Button);
		if(MouseEvent->IsCancelled())
			break;
	}
}
//---------------------------------------------------------------------------
void cUIMouseEventNotification::FilterUp(iUIMouseEvent *MouseEvent,eMouseButton Button)noexcept
{
	for(auto *Handler : fMouseFilters){
		Handler->MouseUp(MouseEvent,Button);
		if(MouseEvent->IsCancelled())
			break;
	}
}
//---------------------------------------------------------------------------
void cUIMouseEventNotification::FilterWheel(iUIMouseEvent *MouseEvent,Float32 ScrollX,Float32 ScrollY)noexcept
{
	for(auto *Handler : fMouseFilters){
		Handler->MouseWheel(MouseEvent,ScrollX,ScrollY);
		if(MouseEvent->IsCancelled())
			break;
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool cUITouchEventNotification::InsertTouchHandler(iUITouchHandler *Handler,sfInt16 Order)noexcept
{
	return fTouchHandlers.Insert(Handler,Order);
}
//---------------------------------------------------------------------------
bool cUITouchEventNotification::RemoveTouchHandler(iUITouchHandler *Handler)noexcept
{
	return fTouchHandlers.Remove(Handler);
}
//---------------------------------------------------------------------------
bool cUITouchEventNotification::InsertTouchFilter(iUITouchHandler *Filter,sfInt16 Order)noexcept
{
	return fTouchFilters.Insert(Filter,Order);
}
//---------------------------------------------------------------------------
bool cUITouchEventNotification::RemoveTouchFilter(iUITouchHandler *Filter)noexcept
{
	return fTouchFilters.Remove(Filter);
}
//---------------------------------------------------------------------------
bool cUITouchEventNotification::TouchAcquireExclusive(iUITouchHandler *Handler)noexcept
{
	return false;
}
//---------------------------------------------------------------------------
void cUITouchEventNotification::TouchReleaseExclusive(iUITouchHandler *Handler)noexcept
{
}
//---------------------------------------------------------------------------
void cUITouchEventNotification::HandleBegin(iUITouchEvent *TouchEvent)noexcept
{
	for(auto *Handler : fTouchHandlers){
		Handler->TouchBegin(TouchEvent);
	}
}
//---------------------------------------------------------------------------
void cUITouchEventNotification::HandleEnd(iUITouchEvent *TouchEvent)noexcept
{
	for(auto *Handler : fTouchHandlers){
		Handler->TouchEnd(TouchEvent);
	}
}
//---------------------------------------------------------------------------
void cUITouchEventNotification::HandleLost(iUITouchEvent *TouchEvent)noexcept
{
	for(auto *Handler : fTouchHandlers){
		Handler->TouchLost(TouchEvent);
	}
}
//---------------------------------------------------------------------------
void cUITouchEventNotification::HandleMove(iUITouchEvent *TouchEvent)noexcept
{
	for(auto *Handler : fTouchHandlers){
		Handler->TouchMove(TouchEvent);
		if(TouchEvent->IsCancelled())
			break;
	}
}
//---------------------------------------------------------------------------
void cUITouchEventNotification::FilterBegin(iUITouchEvent *TouchEvent)noexcept
{
	for(auto *Handler : fTouchFilters){
		Handler->TouchBegin(TouchEvent);
	}
}
//---------------------------------------------------------------------------
void cUITouchEventNotification::FilterEnd(iUITouchEvent *TouchEvent)noexcept
{
	for(auto *Handler : fTouchFilters){
		Handler->TouchEnd(TouchEvent);
	}
}
//---------------------------------------------------------------------------
void cUITouchEventNotification::FilterLost(iUITouchEvent *TouchEvent)noexcept
{
	for(auto *Handler : fTouchFilters){
		Handler->TouchLost(TouchEvent);
	}
}
//---------------------------------------------------------------------------
void cUITouchEventNotification::FilterMove(iUITouchEvent *TouchEvent)noexcept
{
	for(auto *Handler : fTouchFilters){
		Handler->TouchMove(TouchEvent);
		if(TouchEvent->IsCancelled())
			break;
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool cUIViewEventNotification::InsertStateHandler(iUIStateHandler *Handler,sfInt16 Order)noexcept
{
	return fStateHandlers.Insert(Handler,Order);
}
//---------------------------------------------------------------------------
bool cUIViewEventNotification::RemoveStateHandler(iUIStateHandler *Handler)noexcept
{
	return fStateHandlers.Remove(Handler);
}
//---------------------------------------------------------------------------
bool cUIViewEventNotification::InsertViewHandler(iUIViewHandler *Handler,sfInt16 Order)noexcept
{
	return fViewHandlers.Insert(Handler,Order);
}
//---------------------------------------------------------------------------
bool cUIViewEventNotification::RemoveViewHandler(iUIViewHandler *Handler)noexcept
{
	return fViewHandlers.Remove(Handler);
}
//---------------------------------------------------------------------------
void cUIViewEventNotification::StateChange(eUIState PrevState,eUIState NextState)noexcept
{
	cUIStateHandlerSetNotifier Notifier(fStateHandlers);

	UIStateNotifyChange(PrevState,NextState,Notifier);
}
//---------------------------------------------------------------------------
void cUIViewEventNotification::ContentScaleChanged(void)noexcept
{
	for(auto *Handler : fViewHandlers){
		Handler->ContentScaleChanged();
	}
}
//---------------------------------------------------------------------------
void cUIViewEventNotification::RectangleChanged(bool Moved,bool Sized)noexcept
{
	for(auto *Handler : fViewHandlers){
		Handler->RectangleChanged(Moved,Sized);
	}
}
//---------------------------------------------------------------------------
void cUIViewEventNotification::VisibleChanged(void)noexcept
{
	for(auto *Handler : fViewHandlers){
		Handler->VisibleChanged();
	}
}
//---------------------------------------------------------------------------
void cUIViewEventNotification::EnableChanged(void)noexcept
{
	for(auto *Handler : fViewHandlers){
		Handler->EnableChanged();
	}
}
//---------------------------------------------------------------------------
void cUIViewEventNotification::Layout(void)noexcept
{
	for(auto *Handler : fViewHandlers){
		Handler->UILayout();
	}
}
//---------------------------------------------------------------------------
cUIRectangle cUIViewEventNotification::UIMargin(const cUIRectangle &Margin)noexcept
{
	cUIRectangle RetMargin=Margin;
	cVector<Float32,4> RetVector;
	cVector<Float32,4> HandlerVector;
	for(auto *Handler : fViewHandlers){
		auto HandlerMargin=Handler->UIMargin(RetMargin);
		HandlerVector.Load(reinterpret_cast<Float32*>(&HandlerMargin));
		RetVector.Load(reinterpret_cast<Float32*>(&RetMargin));
		RetVector+=HandlerVector;
		RetVector.Store(reinterpret_cast<Float32*>(&RetMargin));
	}
	return RetMargin;
}
//---------------------------------------------------------------------------
void cUIViewEventNotification::WindowChanged(void)noexcept
{
	for(auto *Handler : fViewHandlers){
		Handler->WindowChanged();
	}
}
//---------------------------------------------------------------------------
void cUIViewEventNotification::ParentChanged(void)noexcept
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
