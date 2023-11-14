#include "ControlBase.h"

//---------------------------------------------------------------------------
using namespace cnLibrary;
using namespace cnUI;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
Form::Form()
{
	fView=cnSystem::CreateUIView();
	fView->InsertStateHandler(this);
	fView->InsertViewHandler(this);
}
//---------------------------------------------------------------------------
Form::~Form()
{
	fView->RemoveStateHandler(this);
	fView->RemoveViewHandler(this);
}
//---------------------------------------------------------------------------
Form::operator iUIView *()const
{
	return fView;
}
//---------------------------------------------------------------------------
iUIView* Form::GetView(void)const
{
	return fView;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
ViewControl::ViewControl()
{
}
//---------------------------------------------------------------------------
ViewControl::~ViewControl()
{
	this->SetView(nullptr);
}
//---------------------------------------------------------------------------
iUIView* ViewControl::GetView(void)const
{
	return fView;
}
//---------------------------------------------------------------------------
void ViewControl::SetView(iUIView *View)
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
iUIView* ViewControl::CreateView(void)
{
	if(fView==nullptr){
		SetView(cnSystem::CreateUIView());
	}
	return fView;
}
//---------------------------------------------------------------------------
void ViewControl::ViewSetup(void)
{
	fView->InsertViewHandler(this,LayoutOrder);
}
//---------------------------------------------------------------------------
void ViewControl::ViewClear(void)
{
	fView->RemoveViewHandler(this);
}
//---------------------------------------------------------------------------
void ViewControl::UpdateLayoutOrder(void)
{
	if(fView!=nullptr){
		fView->InsertViewHandler(this,LayoutOrder);
	}
}
//---------------------------------------------------------------------------
void ViewControl::SetLayoutOrder(sfInt16 Order)
{
	if(LayoutOrder==Order)
		return;

	LayoutOrder=Order;
	UpdateLayoutOrder();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
Control::Control()
{
}
//---------------------------------------------------------------------------
Control::~Control()
{
	SetView(nullptr);
}
//---------------------------------------------------------------------------
void Control::ViewSetup(void)
{
	ViewControl::ViewSetup();

	ViewInsertKeyHandler(fView,this);
	ViewInsertMouseHandler(fView,this);
	ViewInsertTouchHandler(fView,this);
}
//---------------------------------------------------------------------------
void Control::ViewClear(void)
{
	ViewRemoveKeyHandler(fView,this);
	ViewRemoveMouseHandler(fView,this);
	ViewRemoveTouchHandler(fView,this);

	ViewControl::ViewClear();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
LayoutControl::LayoutControl()
{
}
//---------------------------------------------------------------------------
LayoutControl::~LayoutControl()
{
}
//---------------------------------------------------------------------------
void LayoutControl::UpdateControlZOffset(void)
{
}
//---------------------------------------------------------------------------
void LayoutControl::SetControlZOffset(Float32 ZPosition)
{
	ControlZOffset=ZPosition;
	UpdateControlZOffset();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
VisualControl::VisualControl()
{
}
//---------------------------------------------------------------------------
VisualControl::~VisualControl()
{
	SetView(nullptr);
}
//---------------------------------------------------------------------------
void VisualControl::UpdateControlZOffset(void)
{
	LayoutControl::UpdateControlZOffset();
	if(fBackground!=nullptr){
		Float32 ZPosition=ControlBackgroundZPosition();
		fBackground->SetContentZPosition(ZPosition);
	}
}
//---------------------------------------------------------------------------
void VisualControl::SetContentVisible(bool Visible)
{
	if(fBackground!=nullptr){
		fBackground->SetContentVisible(Visible);
	}
}
//---------------------------------------------------------------------------
Float32 VisualControl::ControlBackgroundZPosition(void)
{
	return ZPosition_Background+ControlZOffset;
}
//---------------------------------------------------------------------------
void VisualControl::SetBackground(rPtr<viControl> Background)
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
void VisualControl::ViewSetup(void)
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
void VisualControl::ViewClear(void)
{
	if(fBackground!=nullptr){
		fBackground->SetView(nullptr);
	}

	fView->RemoveViewHandler(this);
}
//---------------------------------------------------------------------------
void VisualControl::ControlBackgroundSetDefault(void)
{
}
//---------------------------------------------------------------------------
void VisualControl::ControlBackgroundChanged(void)
{
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
vSimpleViewPainter::vSimpleViewPainter()
{
	fViewContent=cnSystem::CreateUISimpleViewContent(this);
}
//---------------------------------------------------------------------------
vSimpleViewPainter::~vSimpleViewPainter()
{
	fViewContent->SetView(nullptr);
}
//---------------------------------------------------------------------------
void vSimpleViewPainter::SetView(iUIView *View)
{
	fViewContent->SetView(View);
}
//---------------------------------------------------------------------------
void vSimpleViewPainter::SetContentZPosition(Float32 ZPosition)
{
	fViewContent->SetZPosition(ZPosition);
}
//---------------------------------------------------------------------------
void vSimpleViewPainter::SetContentVisible(bool Visible)
{
	fViewContent->SetVisible(Visible);
}
//---------------------------------------------------------------------------
void vSimpleViewPainter::PaintStarted(void){}
void vSimpleViewPainter::PaintShow(void){}
void vSimpleViewPainter::PaintResume(void){}
void vSimpleViewPainter::PaintPaused(void){}
void vSimpleViewPainter::PaintHide(void){}
void vSimpleViewPainter::PaintStopped(void){}
//---------------------------------------------------------------------------
void vSimpleViewPainter::PaintRectChanged(void)
{
	fPaintSize=fViewContent->GetPaintSize();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
KeyAction::KeyAction()
{
}
//---------------------------------------------------------------------------
KeyAction::~KeyAction()
{
	SetView(nullptr);
}
//---------------------------------------------------------------------------
void KeyAction::ViewSetup(void)
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
void KeyAction::ViewClear(void)
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
MouseAction::MouseAction()
{
}
//---------------------------------------------------------------------------
MouseAction::~MouseAction()
{
	SetView(nullptr);
}
//---------------------------------------------------------------------------
void MouseAction::ViewSetup(void)
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
void MouseAction::ViewClear(void)
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
TouchAction::TouchAction()
{
}
//---------------------------------------------------------------------------
TouchAction::~TouchAction()
{
	SetView(nullptr);
}
//---------------------------------------------------------------------------
void TouchAction::ViewSetup(void)
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
void TouchAction::ViewClear(void)
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
void (*cnUI::gApplyDefaultTextStyle)(cUITextStyle &TextStyle)=ApplyDefaultTextStyle;
//---------------------------------------------------------------------------
void cnUI::ApplyDefaultTextStyle(cUITextStyle &TextStyle)
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
