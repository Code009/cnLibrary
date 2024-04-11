#include "ControlBase.h"

//---------------------------------------------------------------------------
using namespace cnLibrary;
using namespace cnUI;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
Form::Form()noexcept
{
	fView=cnSystem::CreateUIView();
	fView->InsertStateHandler(this);
	fView->InsertViewHandler(this);
}
//---------------------------------------------------------------------------
Form::~Form()noexcept
{
	fView->RemoveStateHandler(this);
	fView->RemoveViewHandler(this);
}
//---------------------------------------------------------------------------
Form::operator iUIView *()const noexcept
{
	return fView;
}
//---------------------------------------------------------------------------
iUIView* Form::GetView(void)const noexcept
{
	return fView;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
ViewControl::ViewControl()noexcept
{
}
//---------------------------------------------------------------------------
ViewControl::~ViewControl()noexcept
{
	this->SetView(nullptr);
}
//---------------------------------------------------------------------------
iUIView* ViewControl::GetView(void)const noexcept
{
	return fView;
}
//---------------------------------------------------------------------------
void ViewControl::SetView(iUIView *View)noexcept
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
iUIView* ViewControl::CreateView(void)noexcept
{
	if(fView==nullptr){
		SetView(cnSystem::CreateUIView());
	}
	return fView;
}
//---------------------------------------------------------------------------
void ViewControl::ViewSetup(void)noexcept
{
	fView->InsertViewHandler(this,LayoutOrder);
}
//---------------------------------------------------------------------------
void ViewControl::ViewClear(void)noexcept
{
	fView->RemoveViewHandler(this);
}
//---------------------------------------------------------------------------
void ViewControl::UpdateLayoutOrder(void)noexcept
{
	if(fView!=nullptr){
		fView->InsertViewHandler(this,LayoutOrder);
	}
}
//---------------------------------------------------------------------------
void ViewControl::SetLayoutOrder(sfInt16 Order)noexcept
{
	if(LayoutOrder==Order)
		return;

	LayoutOrder=Order;
	UpdateLayoutOrder();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
Control::Control()noexcept
{
}
//---------------------------------------------------------------------------
Control::~Control()noexcept
{
	SetView(nullptr);
}
//---------------------------------------------------------------------------
void Control::ViewSetup(void)noexcept
{
	ViewControl::ViewSetup();

	ViewInsertKeyHandler(fView,this);
	ViewInsertMouseHandler(fView,this);
	ViewInsertTouchHandler(fView,this);
}
//---------------------------------------------------------------------------
void Control::ViewClear(void)noexcept
{
	ViewRemoveKeyHandler(fView,this);
	ViewRemoveMouseHandler(fView,this);
	ViewRemoveTouchHandler(fView,this);

	ViewControl::ViewClear();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
LayoutControl::LayoutControl()noexcept
{
}
//---------------------------------------------------------------------------
LayoutControl::~LayoutControl()noexcept
{
}
//---------------------------------------------------------------------------
void LayoutControl::UpdateControlZOffset(void)noexcept
{
}
//---------------------------------------------------------------------------
void LayoutControl::SetControlZOffset(Float32 ZPosition)noexcept
{
	ControlZOffset=ZPosition;
	UpdateControlZOffset();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
VisualControl::VisualControl()noexcept
{
}
//---------------------------------------------------------------------------
VisualControl::~VisualControl()noexcept
{
	SetView(nullptr);
}
//---------------------------------------------------------------------------
void VisualControl::UpdateControlZOffset(void)noexcept
{
	LayoutControl::UpdateControlZOffset();
	if(fBackground!=nullptr){
		Float32 ZPosition=ControlBackgroundZPosition();
		fBackground->SetContentZPosition(ZPosition);
	}
}
//---------------------------------------------------------------------------
void VisualControl::SetContentVisible(bool Visible)noexcept
{
	if(fBackground!=nullptr){
		fBackground->SetContentVisible(Visible);
	}
}
//---------------------------------------------------------------------------
Float32 VisualControl::ControlBackgroundZPosition(void)noexcept
{
	return ZPosition_Background+ControlZOffset;
}
//---------------------------------------------------------------------------
void VisualControl::SetBackground(rPtr<viControl> Background)noexcept
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
void VisualControl::ViewSetup(void)noexcept
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
void VisualControl::ViewClear(void)noexcept
{
	if(fBackground!=nullptr){
		fBackground->SetView(nullptr);
	}

	fView->RemoveViewHandler(this);
}
//---------------------------------------------------------------------------
void VisualControl::ControlBackgroundSetDefault(void)noexcept
{
}
//---------------------------------------------------------------------------
void VisualControl::ControlBackgroundChanged(void)noexcept
{
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
vSimpleViewPainter::vSimpleViewPainter()noexcept
{
	fViewContent=cnSystem::CreateUISimpleViewContent(this);
}
//---------------------------------------------------------------------------
vSimpleViewPainter::~vSimpleViewPainter()noexcept
{
	fViewContent->SetView(nullptr);
}
//---------------------------------------------------------------------------
void vSimpleViewPainter::SetView(iUIView *View)noexcept
{
	fViewContent->SetView(View);
}
//---------------------------------------------------------------------------
void vSimpleViewPainter::SetContentZPosition(Float32 ZPosition)noexcept
{
	fViewContent->SetZPosition(ZPosition);
}
//---------------------------------------------------------------------------
void vSimpleViewPainter::SetContentVisible(bool Visible)noexcept
{
	fViewContent->SetVisible(Visible);
}
//---------------------------------------------------------------------------
void vSimpleViewPainter::PaintStarted(void)noexcept{}
void vSimpleViewPainter::PaintShow(void)noexcept{}
void vSimpleViewPainter::PaintResume(void)noexcept{}
void vSimpleViewPainter::PaintPaused(void)noexcept{}
void vSimpleViewPainter::PaintHide(void)noexcept{}
void vSimpleViewPainter::PaintStopped(void)noexcept{}
//---------------------------------------------------------------------------
void vSimpleViewPainter::PaintRectChanged(void)noexcept
{
	fPaintSize=fViewContent->GetPaintSize();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
KeyAction::KeyAction()noexcept
{
}
//---------------------------------------------------------------------------
KeyAction::~KeyAction()noexcept
{
	SetView(nullptr);
}
//---------------------------------------------------------------------------
void KeyAction::ViewSetup(void)noexcept
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
void KeyAction::ViewClear(void)noexcept
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
MouseAction::MouseAction()noexcept
{
}
//---------------------------------------------------------------------------
MouseAction::~MouseAction()noexcept
{
	SetView(nullptr);
}
//---------------------------------------------------------------------------
void MouseAction::ViewSetup(void)noexcept
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
void MouseAction::ViewClear(void)noexcept
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
TouchAction::TouchAction()noexcept
{
}
//---------------------------------------------------------------------------
TouchAction::~TouchAction()noexcept
{
	SetView(nullptr);
}
//---------------------------------------------------------------------------
void TouchAction::ViewSetup(void)noexcept
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
void TouchAction::ViewClear(void)noexcept
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
void (*cnUI::gApplyDefaultTextStyle)(cUITextStyle &TextStyle)noexcept=ApplyDefaultTextStyle;
//---------------------------------------------------------------------------
void cnUI::ApplyDefaultTextStyle(cUITextStyle &TextStyle)noexcept
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
