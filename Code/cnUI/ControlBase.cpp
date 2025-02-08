#include "ControlBase.h"

//---------------------------------------------------------------------------
using namespace cnLibrary;
using namespace cnUI;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
Form::Form()noexcept(true)
{
	fView=cnSystem::CreateUIView();
	fView->InsertStateHandler(this);
	fView->InsertViewHandler(this);
}
//---------------------------------------------------------------------------
Form::~Form()noexcept(true)
{
	fView->RemoveStateHandler(this);
	fView->RemoveViewHandler(this);
}
//---------------------------------------------------------------------------
Form::operator iUIView *()const noexcept(true)
{
	return fView;
}
//---------------------------------------------------------------------------
iUIView* Form::GetView(void)const noexcept(true)
{
	return fView;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
ViewControl::ViewControl()noexcept(true)
{
}
//---------------------------------------------------------------------------
ViewControl::~ViewControl()noexcept(true)
{
	this->SetView(nullptr);
}
//---------------------------------------------------------------------------
iUIView* ViewControl::GetView(void)const noexcept(true)
{
	return fView;
}
//---------------------------------------------------------------------------
void ViewControl::SetView(iUIView *View)noexcept(true)
{
	if(fView!=nullptr){
		ViewClear();
	}
	fView=View;
	if(fView!=nullptr){
		ViewSetup();
	}
}
//---------------------------------------------------------------------------
iUIView* ViewControl::CreateView(void)noexcept(true)
{
	if(fView==nullptr){
		SetView(cnSystem::CreateUIView());
	}
	return fView;
}
//---------------------------------------------------------------------------
void ViewControl::ViewSetup(void)noexcept(true)
{
	fView->InsertViewHandler(this,LayoutOrder);
}
//---------------------------------------------------------------------------
void ViewControl::ViewClear(void)noexcept(true)
{
	fView->RemoveViewHandler(this);
}
//---------------------------------------------------------------------------
void ViewControl::UpdateLayoutOrder(void)noexcept(true)
{
	if(fView!=nullptr){
		fView->InsertViewHandler(this,LayoutOrder);
	}
}
//---------------------------------------------------------------------------
void ViewControl::SetLayoutOrder(sfInt16 Order)noexcept(true)
{
	if(LayoutOrder==Order)
		return;

	LayoutOrder=Order;
	UpdateLayoutOrder();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
Control::Control()noexcept(true)
{
}
//---------------------------------------------------------------------------
Control::~Control()noexcept(true)
{
	SetView(nullptr);
}
//---------------------------------------------------------------------------
void Control::ViewSetup(void)noexcept(true)
{
	ViewControl::ViewSetup();

	ViewInsertKeyHandler(fView,this);
	ViewInsertMouseHandler(fView,this);
	ViewInsertTouchHandler(fView,this);
}
//---------------------------------------------------------------------------
void Control::ViewClear(void)noexcept(true)
{
	ViewRemoveKeyHandler(fView,this);
	ViewRemoveMouseHandler(fView,this);
	ViewRemoveTouchHandler(fView,this);

	ViewControl::ViewClear();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
LayoutControl::LayoutControl()noexcept(true)
{
}
//---------------------------------------------------------------------------
LayoutControl::~LayoutControl()noexcept(true)
{
}
//---------------------------------------------------------------------------
void LayoutControl::UpdateControlZOffset(void)noexcept(true)
{
}
//---------------------------------------------------------------------------
void LayoutControl::SetControlZOffset(Float32 ZPosition)noexcept(true)
{
	ControlZOffset=ZPosition;
	UpdateControlZOffset();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
VisualControl::VisualControl()noexcept(true)
{
}
//---------------------------------------------------------------------------
VisualControl::~VisualControl()noexcept(true)
{
	SetView(nullptr);
}
//---------------------------------------------------------------------------
void VisualControl::UpdateControlZOffset(void)noexcept(true)
{
	LayoutControl::UpdateControlZOffset();
	if(fBackground!=nullptr){
		Float32 ZPosition=ControlBackgroundZPosition();
		fBackground->SetContentZPosition(ZPosition);
	}
}
//---------------------------------------------------------------------------
void VisualControl::SetContentVisible(bool Visible)noexcept(true)
{
	if(fBackground!=nullptr){
		fBackground->SetContentVisible(Visible);
	}
}
//---------------------------------------------------------------------------
Float32 VisualControl::ControlBackgroundZPosition(void)noexcept(true)
{
	return ZPosition_Background+ControlZOffset;
}
//---------------------------------------------------------------------------
void VisualControl::SetBackground(rPtr<viControl> Background)noexcept(true)
{
	if(fBackground==Background)
		return;

	if(fBackground!=nullptr){
		fBackground->SetView(nullptr);
	}
	fBackground=cnVar::MoveCast(Background);
	if(fBackground!=nullptr){
		Float32 ZPosition=ControlBackgroundZPosition();
		fBackground->SetContentZPosition(ZPosition);
		fBackground->SetView(fView);
	}

	ControlBackgroundChanged();
}
//---------------------------------------------------------------------------
void VisualControl::ViewSetup(void)noexcept(true)
{
	fView->InsertViewHandler(this,LayoutOrder);

	if(fBackground!=nullptr){
		fBackground->SetView(fView);
	}
	else{
		ControlBackgroundSetDefault();
	}
}
//---------------------------------------------------------------------------
void VisualControl::ViewClear(void)noexcept(true)
{
	if(fBackground!=nullptr){
		fBackground->SetView(nullptr);
	}

	fView->RemoveViewHandler(this);
}
//---------------------------------------------------------------------------
void VisualControl::ControlBackgroundSetDefault(void)noexcept(true)
{
}
//---------------------------------------------------------------------------
void VisualControl::ControlBackgroundChanged(void)noexcept(true)
{
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
vSimpleViewPainter::vSimpleViewPainter()noexcept(true)
{
	fViewContent=cnSystem::CreateUISimpleViewContent(this);
}
//---------------------------------------------------------------------------
vSimpleViewPainter::~vSimpleViewPainter()noexcept(true)
{
	fViewContent->SetView(nullptr);
}
//---------------------------------------------------------------------------
void vSimpleViewPainter::SetView(iUIView *View)noexcept(true)
{
	fViewContent->SetView(View);
}
//---------------------------------------------------------------------------
void vSimpleViewPainter::SetContentZPosition(Float32 ZPosition)noexcept(true)
{
	fViewContent->SetZPosition(ZPosition);
}
//---------------------------------------------------------------------------
void vSimpleViewPainter::SetContentVisible(bool Visible)noexcept(true)
{
	fViewContent->SetVisible(Visible);
}
//---------------------------------------------------------------------------
void vSimpleViewPainter::PaintStarted(void)noexcept(true){}
void vSimpleViewPainter::PaintShow(void)noexcept(true){}
void vSimpleViewPainter::PaintResume(void)noexcept(true){}
void vSimpleViewPainter::PaintPaused(void)noexcept(true){}
void vSimpleViewPainter::PaintHide(void)noexcept(true){}
void vSimpleViewPainter::PaintStopped(void)noexcept(true){}
//---------------------------------------------------------------------------
void vSimpleViewPainter::PaintRectChanged(void)noexcept(true)
{
	fPaintSize=fViewContent->GetPaintSize();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
KeyAction::KeyAction()noexcept(true)
{
}
//---------------------------------------------------------------------------
KeyAction::~KeyAction()noexcept(true)
{
	SetView(nullptr);
}
//---------------------------------------------------------------------------
void KeyAction::ViewSetup(void)noexcept(true)
{
	ViewControl::ViewSetup();

	fKeyRegisterAsFilter=KeyFilter;
	if(fKeyRegisterAsFilter){
		ViewInsertKeyHandlerFilter(fView,this);
	}
	else{
		ViewInsertKeyHandler(fView,this);
	}
}
//---------------------------------------------------------------------------
void KeyAction::ViewClear(void)noexcept(true)
{
	if(fKeyRegisterAsFilter){
		ViewRemoveKeyHandlerFilter(fView,this);
	}
	else{
		ViewRemoveKeyHandler(fView,this);
	}

	ViewControl::ViewClear();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
MouseAction::MouseAction()noexcept(true)
{
}
//---------------------------------------------------------------------------
MouseAction::~MouseAction()noexcept(true)
{
	SetView(nullptr);
}
//---------------------------------------------------------------------------
void MouseAction::ViewSetup(void)noexcept(true)
{
	ViewControl::ViewSetup();

	fMouseRegisterAsFilter=MouseFilter;
	if(fMouseRegisterAsFilter){
		ViewInsertMouseHandlerFilter(fView,this);
	}
	else{
		ViewInsertMouseHandler(fView,this);
	}
}
//---------------------------------------------------------------------------
void MouseAction::ViewClear(void)noexcept(true)
{
	if(fMouseRegisterAsFilter){
		ViewRemoveMouseHandlerFilter(fView,this);
	}
	else{
		ViewRemoveMouseHandler(fView,this);
	}

	ViewControl::ViewClear();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TouchAction::TouchAction()noexcept(true)
{
}
//---------------------------------------------------------------------------
TouchAction::~TouchAction()noexcept(true)
{
	SetView(nullptr);
}
//---------------------------------------------------------------------------
void TouchAction::ViewSetup(void)noexcept(true)
{
	ViewControl::ViewSetup();

	fTouchRegisterAsFilter=TouchFilter;
	if(fTouchRegisterAsFilter){
		ViewInsertTouchHandlerFilter(fView,this);
	}
	else{
		ViewInsertTouchHandler(fView,this);
	}
}
//---------------------------------------------------------------------------
void TouchAction::ViewClear(void)noexcept(true)
{
	if(fTouchRegisterAsFilter){
		ViewRemoveTouchHandlerFilter(fView,this);
	}
	else{
		ViewRemoveTouchHandler(fView,this);
	}

	ViewControl::ViewClear();
}
//---------------------------------------------------------------------------
void (*cnUI::gApplyDefaultTextStyle)(cUITextStyle &TextStyle)noexcept(true)=ApplyDefaultTextStyle;
//---------------------------------------------------------------------------
void cnUI::ApplyDefaultTextStyle(cUITextStyle &TextStyle)noexcept(true)
{
	TextStyle.FontSize=14;
	TextStyle.Strike=false;
	TextStyle.Underline=false;
	TextStyle.TextColor.Red=0;
	TextStyle.TextColor.Green=0;
	TextStyle.TextColor.Blue=0;
	TextStyle.TextColor.Alpha=1.;
	TextStyle.BackgroundColor.Red=0;
	TextStyle.BackgroundColor.Green=0;
	TextStyle.BackgroundColor.Blue=0;
	TextStyle.BackgroundColor.Alpha=0;
}
//---------------------------------------------------------------------------
