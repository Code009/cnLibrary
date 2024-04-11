#include "UICommon.h"

//---------------------------------------------------------------------------
using namespace cnLibrary;
using namespace cnUI;

//---------------------------------------------------------------------------
const cUIRange cnUI::UIRangeZero={0,0};
//---------------------------------------------------------------------------
bool cnUI::ViewIsParentOf(iUIView *Parent,iUIView *Child)noexcept
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
bool cnUI::ViewSetParent(iUIView *View,iUIView *ParentView)noexcept
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
bool cnUI::ViewSetParent(iUIView *View,iUIView *ParentView,iUIView *CurrentParent)noexcept
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
bool cnUI::ViewIsPointInside(iUIArea *Area,Float32 x,Float32 y)noexcept
{
	cUIPoint Size=Area->GetSize();
	if(x<0 || x>=Size.x)
		return false;
	if(y<0 || y>=Size.y)
		return false;
	return true;
}
//---------------------------------------------------------------------------
bool cnUI::ViewGetPos(iUIArea *View,iUIArea *Relative,cUIPoint &Pos)noexcept
{
	if(View==nullptr)
		return false;
	
	return View->GetPosition(Relative,Pos);
}
//---------------------------------------------------------------------------
bool cnUI::ViewSetPos(iUIArea *View,iUIArea *Relative,const cUIPoint &Pos)noexcept
{
	if(View==nullptr)
		return false;
	
	return View->SetPosition(Relative,Pos);
}
//---------------------------------------------------------------------------
cUIPoint cnUI::ViewGetSize(iUIArea *View)noexcept
{
	if(View==nullptr)
		return UIPointZero;
	
	return View->GetSize();
}
//---------------------------------------------------------------------------
bool cnUI::ViewSetSize(iUIArea *View,const cUIPoint &Size)noexcept
{
	if(View==nullptr)
		return false;
	
	return View->SetSize(Size);
}
//---------------------------------------------------------------------------
bool cnUI::ViewGetRect(iUIArea *View,iUIArea *Relative,cUIPoint &Pos,cUIPoint &Size)noexcept
{
	if(View==nullptr)
		return false;
	
	if(View->GetPosition(Relative,Pos)==false)
		return false;

	Size=View->GetSize();
	return true;
}
//---------------------------------------------------------------------------
bool cnUI::ViewSetRect(iUIArea *View,iUIArea *Relative,const cUIPoint &Pos,const cUIPoint &Size)noexcept
{
	if(View==nullptr)
		return false;
	
	return View->ArrangeRectangle(Relative,Pos,Size);
}
//---------------------------------------------------------------------------
bool cnUI::ViewGetRect(iUIArea *View,iUIArea *Relative,cUIRect &Rect)noexcept
{
	if(View==nullptr)
		return false;
	
	if(View->GetPosition(Relative,Rect.Pos)==false)
		return false;

	Rect.Size=View->GetSize();
	return true;
}
//---------------------------------------------------------------------------
bool cnUI::ViewSetRect(iUIArea *View,iUIArea *Relative,const cUIRect &Rect)noexcept
{
	if(View==nullptr)
		return false;
	
	return View->ArrangeRectangle(Relative,Rect.Pos,Rect.Size);
}
//---------------------------------------------------------------------------
bool cnUI::ViewGetVisible(iUIView *View)noexcept
{
	if(View==nullptr)
		return false;
	return View->GetVisible();
}
//---------------------------------------------------------------------------
bool cnUI::ViewSetVisible(iUIView *View,bool Value)noexcept
{
	if(View==nullptr)
		return false;
	return View->SetVisible(Value);
}
//---------------------------------------------------------------------------
bool cnUI::ViewGetEnable(iUIView *View)noexcept
{
	if(View==nullptr)
		return false;
	return View->GetEnable();
}
//---------------------------------------------------------------------------
bool cnUI::ViewSetEnable(iUIView *View,bool Value)noexcept
{
	if(View==nullptr)
		return false;
	return View->SetEnable(Value);
}
//---------------------------------------------------------------------------
Float32 cnUI::ViewGetZPosition(iUIView *View)noexcept
{
	if(View==nullptr)
		return 0;
	return View->GetZPosition();
}
//---------------------------------------------------------------------------
bool cnUI::ViewSetZPosition(iUIView *View,Float32 ZPosition)noexcept
{
	if(View==nullptr)
		return false;
	return View->SetZPosition(ZPosition);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void UIStateHandler::UIStarted(void)noexcept{}
void UIStateHandler::UIShow(void)noexcept{}
void UIStateHandler::UIResume(void)noexcept{}
void UIStateHandler::UIPaused(void)noexcept{}
void UIStateHandler::UIHide(void)noexcept{}
void UIStateHandler::UIStopped(void)noexcept{}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void UIViewHandler::UILayout(void)noexcept{}
cUIRectangle UIViewHandler::UIMargin(const cUIRectangle&)noexcept{	return UIRectangleZero;	}
void UIViewHandler::WindowChanged(void)noexcept{}
void UIViewHandler::ParentChanged(void)noexcept{}
void UIViewHandler::ContentScaleChanged(void)noexcept{}
void UIViewHandler::RectangleChanged(bool,bool)noexcept{}
void UIViewHandler::ZPositionChanged(void)noexcept{}
void UIViewHandler::VisibleChanged(void)noexcept{}
void UIViewHandler::EnableChanged(void)noexcept{}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void KeyHandler::KeyFocusEnterRange(iUIKeyEvent*)noexcept{}
void KeyHandler::KeyFocusLeaveRange(iUIKeyEvent*)noexcept{}
void KeyHandler::KeyFocusEnter(iUIKeyEvent*)noexcept{}
void KeyHandler::KeyFocusLeave(iUIKeyEvent*)noexcept{}
void KeyHandler::KeyDown(iUIKeyEvent*,eKeyCode)noexcept{}
void KeyHandler::KeyUp(iUIKeyEvent*,eKeyCode)noexcept{}
void KeyHandler::KeyInput(iUIKeyEvent*,eKeyCode)noexcept{}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void TextInputHandler::TextInput(const uChar16*,uIntn)noexcept{}
void TextInputHandler::TextCompositionStarted(void)noexcept{}
void TextInputHandler::TextCompositionFinished(void)noexcept{}
void TextInputHandler::TextCompositionUpdate(sfInt32,const uChar16*,uIntn)noexcept{}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void MouseHandler::MouseEnterRange(iUIMouseEvent*)noexcept{}
void MouseHandler::MouseLeaveRange(iUIMouseEvent*)noexcept{}
void MouseHandler::MouseEnter(iUIMouseEvent*)noexcept{}
void MouseHandler::MouseLeave(iUIMouseEvent*)noexcept{}
iInterface* MouseHandler::MouseGetCursor(iUIMouseEvent*)noexcept{	return nullptr;	}
void MouseHandler::MouseMove(iUIMouseEvent*)noexcept{}
void MouseHandler::MouseDown(iUIMouseEvent*,eMouseButton)noexcept{}
void MouseHandler::MouseUp(iUIMouseEvent*,eMouseButton)noexcept{}
void MouseHandler::MouseWheel(iUIMouseEvent*,Float32,Float32)noexcept{}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void TouchHandler::TouchBegin(iUITouchEvent*)noexcept{}
void TouchHandler::TouchEnd(iUITouchEvent*)noexcept{}
void TouchHandler::TouchLost(iUITouchEvent*)noexcept{}
void TouchHandler::TouchMove(iUITouchEvent*)noexcept{}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
iUIKeyControl* cnUI::ViewInsertKeyHandler(iUIView *View,iUIKeyHandler *Handler,sfInt16 Order)noexcept
{
	auto KeyControl=iCast<iUIKeyControl>(View);
	if(KeyControl!=nullptr){
		KeyControl->InsertKeyHandler(Handler,Order);
	}
	return KeyControl;
}
iUIKeyControl* cnUI::ViewRemoveKeyHandler(iUIView *View,iUIKeyHandler *Handler)noexcept
{
	auto KeyControl=iCast<iUIKeyControl>(View);
	if(KeyControl!=nullptr){
		KeyControl->RemoveKeyHandler(Handler);
	}
	return KeyControl;
}
//---------------------------------------------------------------------------
iUIKeyControl* cnUI::ViewInsertKeyHandlerFilter(iUIView *View,iUIKeyHandler *Handler,sfInt16 Order)noexcept
{
	auto KeyControl=iCast<iUIKeyControl>(View);
	if(KeyControl!=nullptr){
		KeyControl->InsertKeyFilter(Handler,Order);
	}
	return KeyControl;
}
iUIKeyControl* cnUI::ViewRemoveKeyHandlerFilter(iUIView *View,iUIKeyHandler *Handler)noexcept
{
	auto KeyControl=iCast<iUIKeyControl>(View);
	if(KeyControl!=nullptr){
		KeyControl->RemoveKeyFilter(Handler);
	}
	return KeyControl;
}
//---------------------------------------------------------------------------
iUIKeyControl* cnUI::ViewInsertTextInputHandler(iUIView *View,iUITextInputHandler *Handler,sfInt16 Order)noexcept
{
	auto KeyControl=iCast<iUIKeyControl>(View);
	if(KeyControl!=nullptr){
		KeyControl->InsertTextInputHandler(Handler,Order);
	}
	return KeyControl;
}
iUIKeyControl* cnUI::ViewRemoveTextInputHandler(iUIView *View,iUITextInputHandler *Handler)noexcept
{
	auto KeyControl=iCast<iUIKeyControl>(View);
	if(KeyControl!=nullptr){
		KeyControl->RemoveTextInputHandler(Handler);
	}
	return KeyControl;
}
//---------------------------------------------------------------------------
iUIMouseControl* cnUI::ViewInsertMouseHandler(iUIView *View,iUIMouseHandler *Handler,sfInt16 Order)noexcept
{
	auto MouseControl=iCast<iUIMouseControl>(View);
	if(MouseControl!=nullptr){
		MouseControl->InsertMouseHandler(Handler,Order);
	}
	return MouseControl;
}
iUIMouseControl* cnUI::ViewRemoveMouseHandler(iUIView *View,iUIMouseHandler *Handler)noexcept
{
	auto MouseControl=iCast<iUIMouseControl>(View);
	if(MouseControl!=nullptr){
		MouseControl->RemoveMouseHandler(Handler);
	}
	return MouseControl;
}
//---------------------------------------------------------------------------
iUIMouseControl* cnUI::ViewInsertMouseHandlerFilter(iUIView *View,iUIMouseHandler *Handler,sfInt16 Order)noexcept
{
	auto MouseControl=iCast<iUIMouseControl>(View);
	if(MouseControl!=nullptr){
		MouseControl->InsertMouseFilter(Handler,Order);
	}
	return MouseControl;
}
iUIMouseControl* cnUI::ViewRemoveMouseHandlerFilter(iUIView *View,iUIMouseHandler *Handler)noexcept
{
	auto MouseControl=iCast<iUIMouseControl>(View);
	if(MouseControl!=nullptr){
		MouseControl->RemoveMouseFilter(Handler);
	}
	return MouseControl;
}
//---------------------------------------------------------------------------
iUITouchControl* cnUI::ViewInsertTouchHandler(iUIView *View,iUITouchHandler *Handler,sfInt16 Order)noexcept
{
	auto TouchControl=iCast<iUITouchControl>(View);
	if(TouchControl!=nullptr){
		TouchControl->InsertTouchHandler(Handler,Order);
	}
	return TouchControl;
}
iUITouchControl* cnUI::ViewRemoveTouchHandler(iUIView *View,iUITouchHandler *Handler)noexcept
{
	auto TouchControl=iCast<iUITouchControl>(View);
	if(TouchControl!=nullptr){
		TouchControl->RemoveTouchHandler(Handler);
	}
	return TouchControl;
}
//---------------------------------------------------------------------------
iUITouchControl* cnUI::ViewInsertTouchHandlerFilter(iUIView *View,iUITouchHandler *Handler,sfInt16 Order)noexcept
{
	auto TouchControl=iCast<iUITouchControl>(View);
	if(TouchControl!=nullptr){
		TouchControl->InsertTouchFilter(Handler,Order);
	}
	return TouchControl;
}
iUITouchControl* cnUI::ViewRemoveTouchHandlerFilter(iUIView *View,iUITouchHandler *Handler)noexcept
{
	auto TouchControl=iCast<iUITouchControl>(View);
	if(TouchControl!=nullptr){
		TouchControl->RemoveTouchFilter(Handler);
	}
	return TouchControl;
}
//---------------------------------------------------------------------------
