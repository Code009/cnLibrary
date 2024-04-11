#include "MouseAction.h"

using namespace cnLibrary;
using namespace cnUI;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void aMouseClick::MouseLeave(iUIMouseEvent*)noexcept
{
	fClickButton=MouseButton::None;
}
//---------------------------------------------------------------------------
void aMouseClick::MouseDown(iUIMouseEvent*,eMouseButton Button)noexcept
{
	if(ActionButton==Button){
		fClickButton=Button;
	}
}
//---------------------------------------------------------------------------
void aMouseClick::MouseUp(iUIMouseEvent *MouseEvent,eMouseButton Button)noexcept
{
	if(fClickButton==Button){
		// release
		fClickButton=MouseButton::None;
		// test if in click range
		if(OnClick!=nullptr){
			cUIPoint Pos;
			if(MouseEvent->GetPosition(fView,Pos)){
				if(ViewIsPointInside(fView,Pos.x,Pos.y)){
					OnClick();
				}
			}
		}
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void aMouseTap::MouseLeave(iUIMouseEvent*)noexcept
{
	TapCount=0;
	fClickButton=MouseButton::Left;
}
//---------------------------------------------------------------------------
void aMouseTap::ActionTap(void)noexcept
{
	if(OnTap!=nullptr){
		OnTap();
	}
}
//---------------------------------------------------------------------------
cUIPoint aMouseTap::GetTapPos(void)const noexcept
{
	return fDownPos;
}
//---------------------------------------------------------------------------
void aMouseTap::MouseDown(iUIMouseEvent *MouseEvent,eMouseButton Button)noexcept
{
	if(ActionButton==Button){
		fClickButton=Button;

		// get pos
		cUIPoint CurDownPos={0,0};
		MouseEvent->GetPosition(fView,CurDownPos);

		// get time
		auto CurTime=cnSystem::GetSystemTimeNow();
		if(TapCount!=0){
			if(CurTime-fDownTime<=TapIntervalMS*Time_1ms){
				// can be multiple click
				// test if in range
				Float32 dx=fDownPos.x-CurDownPos.x;
				Float32 dy=fDownPos.y-CurDownPos.y;
				if(dx<0)	dx=-dx;
				if(dy<0)	dy=-dy;
				if(dx>CancelDistance || dy>CancelDistance){
					// out of range, reset to first click
					TapCount=1;
				}
				else{
					// count as multiple click
					TapCount++;
				}
			}
			else{
				// time out , reset to first click
				TapCount=1;
			}
		}
		else{
			// first click
			TapCount=1;
		}
		fDownPos=CurDownPos;
		fDownTime=CurTime;
		if(ActionWhenButtonUp==false){
			ActionTap();
		}
	}
}
//---------------------------------------------------------------------------
void aMouseTap::MouseUp(iUIMouseEvent *MouseEvent,eMouseButton Button)noexcept
{
	if(fClickButton==Button){
		// release
		fClickButton=eMouseButton::None;
		// test if in click interval
		if(ActionWhenButtonUp){
			auto CurTime=cnSystem::GetSystemTimeNow();
			if(CurTime-fDownTime<TapIntervalMS*Time_1ms){
				// test if in click range
				cUIPoint CurDownPos;
				if(MouseEvent->GetPosition(fView,CurDownPos)){
					Float32 dx=fDownPos.x-CurDownPos.x;
					Float32 dy=fDownPos.y-CurDownPos.y;
					if(dx<0)	dx=-dx;
					if(dy<0)	dy=-dy;
					if(dx>CancelDistance || dy>CancelDistance){
						// out of range, reset click count
						TapCount=0;
					}
					else{
						// valid click
						ActionTap();
					}
				}
			}
			else{
				// timeout, reset click count
				TapCount=0;
			}
		}
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void aMouseHover::ViewSetup(void)noexcept
{
	MouseAction::ViewSetup();
	auto UIDispatch=cnSystem::CurrentUIThread::GetDispatch(false);
	fHoverTimer=UIDispatch->CreateTimer(nullptr,&fMouseHoverTimerProc);
}
//---------------------------------------------------------------------------
void aMouseHover::ViewClear(void)noexcept
{
	fHoverTimer->Stop();
	fHoverTimer=nullptr;
	MouseAction::ViewClear();
}
//---------------------------------------------------------------------------
void aMouseHover::MouseLeave(iUIMouseEvent*)noexcept
{
	fHoverTimer->Stop();
	fHoverStarted=false;
}
//---------------------------------------------------------------------------
void aMouseHover::MouseMove(iUIMouseEvent *MouseEvent)noexcept
{
	cUIPoint CurPos;
	if(MouseEvent->GetPosition(fView,CurPos)==false)
		return;

	if(fHoverStarted){
		Float32 dx=CurPos.x-fHoverPos.x;
		Float32 dy=CurPos.y-fHoverPos.y;
		dx=cnMath::Absolute(dx);
		dy=cnMath::Absolute(dy);
		if(dx>=CancelDistance || dy>=CancelDistance){
			uInt64 TimeNow=cnSystem::GetSystemTimeNow();
			fHoverTimer->Start(TimeNow+StayTime,0);
			fHoverPos=CurPos;
		}
	}
	else{
		uInt64 TimeNow=cnSystem::GetSystemTimeNow();
		fHoverTimer->Start(TimeNow+StayTime,0);
		fHoverPos=CurPos;
		fHoverStarted=true;
	}
}
//---------------------------------------------------------------------------
void aMouseHover::cMouseHoverTimerProc::Execute(void)noexcept
{
	auto Host=cnMemory::GetObjectFromMemberPointer(this,&aMouseHover::fMouseHoverTimerProc);
	Host->ActionHover();
}
//---------------------------------------------------------------------------
void aMouseHover::ActionHover(void)noexcept
{
	fHoverStarted=false;
	if(OnHover!=nullptr)
		OnHover();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void aMousePan::MouseLeaveRange(iUIMouseEvent*)noexcept
{
	fDownButton=MouseButton::None;
}
//---------------------------------------------------------------------------
void aMousePan::MouseMove(iUIMouseEvent *MouseEvent)noexcept
{
	auto MouseControl=iCast<iUIMouseControl>(fView);
	if(fDownButton!=MouseButton::None){
		cUIPoint Pos;
		MouseEvent->GetPosition(fView,Pos);

		Float32 dx=Pos.x-fDownPoint.x;
		Float32 dy=Pos.y-fDownPoint.y;
		bool ax=AllowX && (dx<-DelayX || dx>DelayX);
		bool ay=AllowY && (dy<-DelayY || dy>DelayY);
		if(ax || ay){
			if(MouseControl->MouseAcquireExclusive(&fExclusiveHandler)==false){
				fDownButton=MouseButton::None;
				return;
			}
			if(OnScrollStart!=nullptr){
				OnScrollStart(Pos);
			}
		}
	}
}
//---------------------------------------------------------------------------
void aMousePan::MouseDown(iUIMouseEvent *MouseEvent,eMouseButton Button)noexcept
{
	if(Button==ActionButton){
		cUIPoint Pos;
		MouseEvent->GetPosition(fView,Pos);

		bool AllowStart;
		if(OnAllowScroll!=nullptr){
			AllowStart=OnAllowScroll(Pos);
		}
		else{
			AllowStart=true;
		}
		if(AllowStart){
			fDownButton=Button;
			fDownPoint=Pos;
		}
		else{
			fDownButton=MouseButton::None;
		}
	}
}
//---------------------------------------------------------------------------
void aMousePan::MouseUp(iUIMouseEvent*,eMouseButton)noexcept
{
	fDownButton=MouseButton::None;
}
//---------------------------------------------------------------------------
aMousePan* aMousePan::cExclusiveHandler::GetHost(void)noexcept
{
	return cnMemory::GetObjectFromMemberPointer(this,&aMousePan::fExclusiveHandler);
}
//---------------------------------------------------------------------------
void aMousePan::cExclusiveHandler::MouseMove(iUIMouseEvent *MouseEvent)noexcept
{
	GetHost()->ExclusiveMouseMove(MouseEvent);
}
//---------------------------------------------------------------------------
void aMousePan::cExclusiveHandler::MouseUp(iUIMouseEvent *MouseEvent,eMouseButton Button)noexcept
{
	GetHost()->ExclusiveMouseUp(MouseEvent,Button);
}
//---------------------------------------------------------------------------
void aMousePan::ExclusiveMouseMove(iUIMouseEvent *MouseEvent)noexcept
{
	//auto MouseControl=iCast<iUIMouseControl>(fView);
	if(fDownButton!=MouseButton::None){
		cUIPoint Pos;
		MouseEvent->GetPosition(fView,Pos);

		if(OnScroll!=nullptr)
			OnScroll(Pos);
	}
}
//---------------------------------------------------------------------------
void aMousePan::ExclusiveMouseUp(iUIMouseEvent *MouseEvent,eMouseButton Button)noexcept
{
	if(Button==fDownButton){
		auto MouseControl=iCast<iUIMouseControl>(fView);
		fDownButton=MouseButton::None;
		MouseControl->MouseReleaseExclusive(this);
		//fStarted=false;

		if(OnScrollFinish!=nullptr){
			cUIPoint Pos;
			MouseEvent->GetPosition(fView,Pos);
			OnScrollFinish(Pos);
		}
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
afMouseActiveContent::afMouseActiveContent()noexcept
{
}
//---------------------------------------------------------------------------
afMouseActiveContent::~afMouseActiveContent()noexcept
{
	SetView(nullptr);
	if(fActiveView!=nullptr){
		ViewRemoveKeyHandler(fActiveView,this);
	}
}
//---------------------------------------------------------------------------
void afMouseActiveContent::SetActiveView(iUIView *View)noexcept
{
	if(fActiveView!=nullptr){
		ViewRemoveKeyHandler(fActiveView,this);
	}
	fActiveView=View;
	if(fActiveView!=nullptr){
		if(fView!=nullptr){
			ApplyActiveViewVisible();
		}
		ViewInsertKeyHandler(fActiveView,this);
	}
}
//---------------------------------------------------------------------------
void afMouseActiveContent::ViewSetup(void)noexcept
{
	ViewControl::ViewSetup();

	ViewInsertMouseHandlerFilter(fView,this);


	if(fActiveView!=nullptr){
		fActiveView->SetVisible(false);
	}
}
//---------------------------------------------------------------------------
void afMouseActiveContent::ViewClear(void)noexcept
{
	fHot=false;
	fActiveFocus=false;

	ViewRemoveMouseHandlerFilter(fView,this);

	ViewControl::ViewClear();
}
//---------------------------------------------------------------------------
void afMouseActiveContent::MouseEnterRange(iUIMouseEvent*)noexcept
{
	fHot=true;
	if(fActiveView!=nullptr){
		ApplyActiveViewVisible();
	}
}
//---------------------------------------------------------------------------
void afMouseActiveContent::MouseLeaveRange(iUIMouseEvent*)noexcept
{
	fHot=false;
	if(fActiveView!=nullptr){
		ApplyActiveViewVisible();
	}
}
//---------------------------------------------------------------------------
void afMouseActiveContent::MouseDown(iUIMouseEvent*,eMouseButton)noexcept
{
	if(AutoFocus && fActiveView!=nullptr){
		auto KeyControl=iCast<iUIKeyControl>(fActiveView);
		if(KeyControl!=nullptr){
			KeyControl->SetFocus(true);
		}
	}
}
//---------------------------------------------------------------------------
void afMouseActiveContent::KeyFocusEnterRange(iUIKeyEvent*)noexcept
{
	fActiveFocus=true;
}
//---------------------------------------------------------------------------
void afMouseActiveContent::KeyFocusLeaveRange(iUIKeyEvent*)noexcept
{
	fActiveFocus=false;
	ApplyActiveViewVisible();
}
//---------------------------------------------------------------------------
void afMouseActiveContent::ApplyActiveViewVisible(void)noexcept
{
	if(fActiveFocus || fHot){
		fActiveView->SetVisible(true);
	}
	else{
		fActiveView->SetVisible(false);
	}
}
//---------------------------------------------------------------------------
