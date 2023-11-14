#include "UICommon.h"

//---------------------------------------------------------------------------
using namespace cnLibrary;
using namespace cnUI;

//---------------------------------------------------------------------------
const cUIRange cnUI::UIRangeZero={0,0};
//---------------------------------------------------------------------------
bool cnUI::ViewIsParentOf(iUIView *Parent,iUIView *Child)
{
	if(Child==nullptr)
		return false;
	if(Parent==nullptr)
		return false;
	auto CheckParent=Child->GetParent();
	while(CheckParent!=nullptr){
		if(CheckParent==Parent)
			return true;
		CheckParent=CheckParent->GetParent();
	}
	return false;
}
//---------------------------------------------------------------------------
bool cnUI::ViewSetParent(iUIView *View,iUIView *ParentView)
{
	if(View==nullptr)
		return false;
	auto CurParent=View->GetParent();
	if(CurParent==ParentView)
		return true;

	if(CurParent!=nullptr){
		if(CurParent->RemoveView(View)==false)
			return false;
	}
	if(ParentView==nullptr){
		return true;
	}
	return ParentView->InsertView(View);
}
//---------------------------------------------------------------------------
bool cnUI::ViewSetParent(iUIView *View,iUIView *ParentView,iUIView *CurrentParent)
{
	if(View==nullptr)
		return false;
	if(CurrentParent==ParentView)
		return false;	// parent not changed
	if(CurrentParent!=nullptr){
		if(CurrentParent->RemoveView(View)==false){
			return false;
		}
	}
	if(ParentView==nullptr)
		return true;	// no parent
	return ParentView->InsertView(View);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool cnUI::ViewIsPointInside(iUIArea *Area,Float32 x,Float32 y)
{
	cUIPoint Size=Area->GetSize();
	if(x<0 || x>=Size.x)
		return false;
	if(y<0 || y>=Size.y)
		return false;
	return true;
}
//---------------------------------------------------------------------------
bool cnUI::ViewGetPos(iUIArea *View,iUIArea *Relative,cUIPoint &Pos)
{
	if(View==nullptr)
		return false;
	
	return View->GetPosition(Relative,Pos);
}
//---------------------------------------------------------------------------
bool cnUI::ViewSetPos(iUIArea *View,iUIArea *Relative,const cUIPoint &Pos)
{
	if(View==nullptr)
		return false;
	
	return View->SetPosition(Relative,Pos);
}
//---------------------------------------------------------------------------
cUIPoint cnUI::ViewGetSize(iUIArea *View)
{
	if(View==nullptr)
		return UIPointZero;
	
	return View->GetSize();
}
//---------------------------------------------------------------------------
bool cnUI::ViewSetSize(iUIArea *View,const cUIPoint &Size)
{
	if(View==nullptr)
		return false;
	
	return View->SetSize(Size);
}
//---------------------------------------------------------------------------
bool cnUI::ViewGetRect(iUIArea *View,iUIArea *Relative,cUIPoint &Pos,cUIPoint &Size)
{
	if(View==nullptr)
		return false;
	
	if(View->GetPosition(Relative,Pos)==false)
		return false;

	Size=View->GetSize();
	return true;
}
//---------------------------------------------------------------------------
bool cnUI::ViewSetRect(iUIArea *View,iUIArea *Relative,const cUIPoint &Pos,const cUIPoint &Size)
{
	if(View==nullptr)
		return false;
	
	return View->ArrangeRectangle(Relative,Pos,Size);
}
//---------------------------------------------------------------------------
bool cnUI::ViewGetRect(iUIArea *View,iUIArea *Relative,cUIRect &Rect)
{
	if(View==nullptr)
		return false;
	
	if(View->GetPosition(Relative,Rect.Pos)==false)
		return false;

	Rect.Size=View->GetSize();
	return true;
}
//---------------------------------------------------------------------------
bool cnUI::ViewSetRect(iUIArea *View,iUIArea *Relative,const cUIRect &Rect)
{
	if(View==nullptr)
		return false;
	
	return View->ArrangeRectangle(Relative,Rect.Pos,Rect.Size);
}
//---------------------------------------------------------------------------
bool cnUI::ViewGetVisible(iUIView *View)
{
	if(View==nullptr)
		return false;
	return View->GetVisible();
}
//---------------------------------------------------------------------------
bool cnUI::ViewSetVisible(iUIView *View,bool Value)
{
	if(View==nullptr)
		return false;
	return View->SetVisible(Value);
}
//---------------------------------------------------------------------------
bool cnUI::ViewGetEnable(iUIView *View)
{
	if(View==nullptr)
		return false;
	return View->GetEnable();
}
//---------------------------------------------------------------------------
bool cnUI::ViewSetEnable(iUIView *View,bool Value)
{
	if(View==nullptr)
		return false;
	return View->SetEnable(Value);
}
//---------------------------------------------------------------------------
Float32 cnUI::ViewGetZPosition(iUIView *View)
{
	if(View==nullptr)
		return 0;
	return View->GetZPosition();
}
//---------------------------------------------------------------------------
bool cnUI::ViewSetZPosition(iUIView *View,Float32 ZPosition)
{
	if(View==nullptr)
		return false;
	return View->SetZPosition(ZPosition);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void UIStateHandler::UIStarted(void){}
void UIStateHandler::UIShow(void){}
void UIStateHandler::UIResume(void){}
void UIStateHandler::UIPaused(void){}
void UIStateHandler::UIHide(void){}
void UIStateHandler::UIStopped(void){}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void UIViewHandler::UILayout(void){}
cUIRectangle UIViewHandler::UIMargin(const cUIRectangle&){	return UIRectangleZero;	}
void UIViewHandler::WindowChanged(void){}
void UIViewHandler::ParentChanged(void){}
void UIViewHandler::ContentScaleChanged(void){}
void UIViewHandler::RectangleChanged(bool,bool){}
void UIViewHandler::ZPositionChanged(void){}
void UIViewHandler::VisibleChanged(void){}
void UIViewHandler::EnableChanged(void){}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void KeyHandler::KeyFocusEnterRange(iUIKeyEvent*){}
void KeyHandler::KeyFocusLeaveRange(iUIKeyEvent*){}
void KeyHandler::KeyFocusEnter(iUIKeyEvent*){}
void KeyHandler::KeyFocusLeave(iUIKeyEvent*){}
void KeyHandler::KeyDown(iUIKeyEvent*,eKeyCode){}
void KeyHandler::KeyUp(iUIKeyEvent*,eKeyCode){}
void KeyHandler::KeyInput(iUIKeyEvent*,eKeyCode){}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void TextInputHandler::TextInput(const uChar16*,uIntn){}
void TextInputHandler::TextCompositionStarted(void){}
void TextInputHandler::TextCompositionFinished(void){}
void TextInputHandler::TextCompositionUpdate(sfInt32,const uChar16*,uIntn){}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void MouseHandler::MouseEnterRange(iUIMouseEvent*){}
void MouseHandler::MouseLeaveRange(iUIMouseEvent*){}
void MouseHandler::MouseEnter(iUIMouseEvent*){}
void MouseHandler::MouseLeave(iUIMouseEvent*){}
iInterface* MouseHandler::MouseGetCursor(iUIMouseEvent*){	return nullptr;	}
void MouseHandler::MouseMove(iUIMouseEvent*){}
void MouseHandler::MouseDown(iUIMouseEvent*,eMouseButton){}
void MouseHandler::MouseUp(iUIMouseEvent*,eMouseButton){}
void MouseHandler::MouseWheel(iUIMouseEvent*,Float32,Float32){}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void TouchHandler::TouchBegin(iUITouchEvent*){}
void TouchHandler::TouchEnd(iUITouchEvent*){}
void TouchHandler::TouchLost(iUITouchEvent*){}
void TouchHandler::TouchMove(iUITouchEvent*){}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
iUIKeyControl* cnUI::ViewInsertKeyHandler(iUIView *View,iUIKeyHandler *Handler,sfInt16 Order)
{
	auto KeyControl=iCast<iUIKeyControl>(View);
	if(KeyControl!=nullptr){
		KeyControl->InsertKeyHandler(Handler,Order);
	}
	return KeyControl;
}
iUIKeyControl* cnUI::ViewRemoveKeyHandler(iUIView *View,iUIKeyHandler *Handler)
{
	auto KeyControl=iCast<iUIKeyControl>(View);
	if(KeyControl!=nullptr){
		KeyControl->RemoveKeyHandler(Handler);
	}
	return KeyControl;
}
//---------------------------------------------------------------------------
iUIKeyControl* cnUI::ViewInsertKeyHandlerFilter(iUIView *View,iUIKeyHandler *Handler,sfInt16 Order)
{
	auto KeyControl=iCast<iUIKeyControl>(View);
	if(KeyControl!=nullptr){
		KeyControl->InsertKeyFilter(Handler,Order);
	}
	return KeyControl;
}
iUIKeyControl* cnUI::ViewRemoveKeyHandlerFilter(iUIView *View,iUIKeyHandler *Handler)
{
	auto KeyControl=iCast<iUIKeyControl>(View);
	if(KeyControl!=nullptr){
		KeyControl->RemoveKeyFilter(Handler);
	}
	return KeyControl;
}
//---------------------------------------------------------------------------
iUIKeyControl* cnUI::ViewInsertTextInputHandler(iUIView *View,iUITextInputHandler *Handler,sfInt16 Order)
{
	auto KeyControl=iCast<iUIKeyControl>(View);
	if(KeyControl!=nullptr){
		KeyControl->InsertTextInputHandler(Handler,Order);
	}
	return KeyControl;
}
iUIKeyControl* cnUI::ViewRemoveTextInputHandler(iUIView *View,iUITextInputHandler *Handler)
{
	auto KeyControl=iCast<iUIKeyControl>(View);
	if(KeyControl!=nullptr){
		KeyControl->RemoveTextInputHandler(Handler);
	}
	return KeyControl;
}
//---------------------------------------------------------------------------
iUIMouseControl* cnUI::ViewInsertMouseHandler(iUIView *View,iUIMouseHandler *Handler,sfInt16 Order)
{
	auto MouseControl=iCast<iUIMouseControl>(View);
	if(MouseControl!=nullptr){
		MouseControl->InsertMouseHandler(Handler,Order);
	}
	return MouseControl;
}
iUIMouseControl* cnUI::ViewRemoveMouseHandler(iUIView *View,iUIMouseHandler *Handler)
{
	auto MouseControl=iCast<iUIMouseControl>(View);
	if(MouseControl!=nullptr){
		MouseControl->RemoveMouseHandler(Handler);
	}
	return MouseControl;
}
//---------------------------------------------------------------------------
iUIMouseControl* cnUI::ViewInsertMouseHandlerFilter(iUIView *View,iUIMouseHandler *Handler,sfInt16 Order)
{
	auto MouseControl=iCast<iUIMouseControl>(View);
	if(MouseControl!=nullptr){
		MouseControl->InsertMouseFilter(Handler,Order);
	}
	return MouseControl;
}
iUIMouseControl* cnUI::ViewRemoveMouseHandlerFilter(iUIView *View,iUIMouseHandler *Handler)
{
	auto MouseControl=iCast<iUIMouseControl>(View);
	if(MouseControl!=nullptr){
		MouseControl->RemoveMouseFilter(Handler);
	}
	return MouseControl;
}
//---------------------------------------------------------------------------
iUITouchControl* cnUI::ViewInsertTouchHandler(iUIView *View,iUITouchHandler *Handler,sfInt16 Order)
{
	auto TouchControl=iCast<iUITouchControl>(View);
	if(TouchControl!=nullptr){
		TouchControl->InsertTouchHandler(Handler,Order);
	}
	return TouchControl;
}
iUITouchControl* cnUI::ViewRemoveTouchHandler(iUIView *View,iUITouchHandler *Handler)
{
	auto TouchControl=iCast<iUITouchControl>(View);
	if(TouchControl!=nullptr){
		TouchControl->RemoveTouchHandler(Handler);
	}
	return TouchControl;
}
//---------------------------------------------------------------------------
iUITouchControl* cnUI::ViewInsertTouchHandlerFilter(iUIView *View,iUITouchHandler *Handler,sfInt16 Order)
{
	auto TouchControl=iCast<iUITouchControl>(View);
	if(TouchControl!=nullptr){
		TouchControl->InsertTouchFilter(Handler,Order);
	}
	return TouchControl;
}
iUITouchControl* cnUI::ViewRemoveTouchHandlerFilter(iUIView *View,iUITouchHandler *Handler)
{
	auto TouchControl=iCast<iUITouchControl>(View);
	if(TouchControl!=nullptr){
		TouchControl->RemoveTouchFilter(Handler);
	}
	return TouchControl;
}
//---------------------------------------------------------------------------
