#include "TouchAction.h"

using namespace cnLibrary;
using namespace cnUI;
using namespace cnRTL;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cUITouchFilter::cUITouchFilter()noexcept
{
	fViewTouch=nullptr;
}
//---------------------------------------------------------------------------
cUITouchFilter::~cUITouchFilter()noexcept
{
	if(fViewTouch!=nullptr){
		fViewTouch->RemoveTouchFilter(this);
	}
}
//---------------------------------------------------------------------------
iUIView* cUITouchFilter::GetView(void)noexcept
{
	return fView;
}
//---------------------------------------------------------------------------
void cUITouchFilter::SetView(iUIView *View)noexcept
{
	if(fViewTouch!=nullptr){
		fViewTouch->RemoveTouchFilter(this);
	}
	fViewTouch=iCast<iUITouchControl>(View);
	if(fViewTouch==nullptr)
		return;

	fView=View;
	fViewTouch->InsertTouchFilter(this);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool cUITouchDistanceDetect::Begin(sInt32 Distance,iUITouchEvent *TouchEvent,cUIPoint &Pos,iUIArea *Relative)noexcept
{
	if(TouchEvent->GetPosition(Relative,Pos)==false)
		return false;
	if(Distance==0){
		return true;
	}
	else{
		fBeginMap[TouchEvent->GetTouchID()]=Pos;
		return false;
	}
}
//---------------------------------------------------------------------------
bool cUITouchDistanceDetect::Move(sInt32 Distance,iUITouchEvent *TouchEvent,cUIPoint &Pos,iUIArea *Relative)noexcept
{
	if(Distance==0){
		return true;
	}
	TouchEvent->GetPosition(Relative,Pos);
	auto pBeginPoint=fBeginMap.Get(TouchEvent->GetTouchID());
	if(pBeginPoint==nullptr)
		return false;
	auto dx=pBeginPoint->x-Pos.x;
	auto dy=pBeginPoint->y-Pos.y;
	if(dx>=Distance || dy>=Distance){
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------
void cUITouchDistanceDetect::End(iUITouchEvent *TouchEvent)noexcept
{
	fBeginMap.Remove(TouchEvent->GetTouchID());
}
//---------------------------------------------------------------------------
void cUITouchDistanceDetect::Clear(void)noexcept
{
	fBeginMap.Clear();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cUITouchActionTap::cUITouchActionTap()noexcept
	: fTapTime(0)
	, fTouchID(nullptr)
{
}
//---------------------------------------------------------------------------
cUITouchActionTap::~cUITouchActionTap(void)noexcept
{
}
//---------------------------------------------------------------------------
void cUITouchActionTap::TouchDown(iUITouchEvent *TouchEvent)noexcept
{
	if(fTouchID!=0)
		return;

	fTouchID=TouchEvent->GetTouchID();
	TouchEvent->GetPosition(fView,TapPos);
}
//---------------------------------------------------------------------------
void cUITouchActionTap::TouchUp(iUITouchEvent *TouchEvent)noexcept
{
	if(TouchEvent->GetTouchID()!=fTouchID){
		return;
	}
	cUIPoint upPos;
	TouchEvent->GetPosition(fView,upPos);
	sIntn dx=static_cast<sIntn>(upPos.x-TapPos.x);
	sIntn dy=static_cast<sIntn>(upPos.y-TapPos.y);
	static sIntn TapSize=10;
	uInt64 Now=cnSystem::GetSystemTimeNow();
	if(dx>-TapSize && dx<TapSize && dy>-TapSize && dy<TapSize){
		if(Now-fTapTime>300*Time_1ms){
			TapCount=0;
		}
		else{
			TapCount++;
		}
		TapPos=upPos;
		if(OnTap!=nullptr){
			OnTap();
		}
	}
	else{
		TapCount=0;
	}

	fTapTime=Now;
	fTouchID=nullptr;
}
//---------------------------------------------------------------------------
void cUITouchActionTap::TouchLost(iUITouchEvent *TouchEvent)noexcept
{
	if(TouchEvent->GetTouchID()!=fTouchID){
		return;
	}
	fTouchID=nullptr;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cUITouchActionLongPress::cUITouchActionLongPress()noexcept
{
	auto UIDispatch=cnSystem::CurrentUIThread::GetDispatch(false);
	fPressTimer=UIDispatch->CreateTimer(nullptr,&fTimerProcedure);
	DurationNS=Time_1s;
}
//---------------------------------------------------------------------------
cUITouchActionLongPress::~cUITouchActionLongPress(void)noexcept
{
}
//---------------------------------------------------------------------------
void cnLib_FUNC cUITouchActionLongPress::cTimerProcedure::Execute(void)noexcept
{
	auto Host=cnMemory::GetObjectFromMemberPointer(this,&cUITouchActionLongPress::fTimerProcedure);
	Host->TimerHit();
}
//---------------------------------------------------------------------------
void cUITouchActionLongPress::TimerHit(void)noexcept
{
	if(fTouchID==nullptr)
		return;

	if(OnPress!=nullptr)
		OnPress();
}
//---------------------------------------------------------------------------
void cUITouchActionLongPress::TouchDown(iUITouchEvent *TouchEvent)noexcept
{
	if(fTouchID!=nullptr)
		return;

	fTouchID=TouchEvent->GetTouchID();
	TouchEvent->GetPosition(fView,TapPos);
	uInt64 TimeNow=cnSystem::GetSystemTimeNow();
	fPressTimer->Start(TimeNow+DurationNS,0);
}
//---------------------------------------------------------------------------
void cUITouchActionLongPress::TouchUp(iUITouchEvent *TouchEvent)noexcept
{
	if(TouchEvent->GetTouchID()!=fTouchID){
		return;
	}
	fPressTimer->Stop();
	fTouchID=nullptr;
}
//---------------------------------------------------------------------------
void cUITouchActionLongPress::TouchLost(iUITouchEvent *TouchEvent)noexcept
{
	if(TouchEvent->GetTouchID()!=fTouchID){
		return;
	}
	fPressTimer->Stop();
	fTouchID=nullptr;
}
//---------------------------------------------------------------------------
void cUITouchActionLongPress::TouchMove(iUITouchEvent *TouchEvent)noexcept
{
	if(TouchEvent->GetTouchID()!=fTouchID){
		return;
	}
	fPressTimer->Stop();
	fTouchID=nullptr;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cUITouchActionPan::cUITouchActionPan()noexcept
{
}
//---------------------------------------------------------------------------
cUITouchActionPan::~cUITouchActionPan()noexcept
{
}
//---------------------------------------------------------------------------
void cUITouchActionPan::Start(iUITouchEvent *TouchEvent,const cUIPoint &Pos)noexcept
{
	if(OnStart!=nullptr){
		if(OnStart(Pos)==false){
			fPointMoveDetect.End(TouchEvent);
			return;
		}
	}
	fPointMoveDetect.Clear();
	// begin
	fPanTouchID=TouchEvent->GetTouchID();
	fViewTouch->TouchAcquireExclusive(this);
}
//---------------------------------------------------------------------------
void cUITouchActionPan::Stop(iUITouchEvent *TouchEvent)noexcept
{
	fViewTouch->TouchReleaseExclusive(this);
	fPointMoveDetect.End(TouchEvent);
	if(fPanTouchID==TouchEvent->GetTouchID()){
		if(OnExit!=nullptr)
			OnExit();
		fPanTouchID=nullptr;
	}
}
//---------------------------------------------------------------------------
void cUITouchActionPan::TouchDown(iUITouchEvent *TouchEvent)noexcept
{
	if(fPanTouchID!=nullptr)
		return;

	cUIPoint Pos;
	if(fPointMoveDetect.Begin(DelayLength,TouchEvent,Pos,fView)){
		Start(TouchEvent,Pos);
	}
}
//---------------------------------------------------------------------------
void cUITouchActionPan::TouchUp(iUITouchEvent *TouchEvent)noexcept
{
	Stop(TouchEvent);
}
//---------------------------------------------------------------------------
void cUITouchActionPan::TouchLost(iUITouchEvent *TouchEvent)noexcept
{
	Stop(TouchEvent);
}
//---------------------------------------------------------------------------
void cUITouchActionPan::TouchMove(iUITouchEvent *TouchEvent)noexcept
{
	if(fPanTouchID==nullptr){
		cUIPoint Pos;
		if(fPointMoveDetect.Move(DelayLength,TouchEvent,Pos,fView)){
			Start(TouchEvent,Pos);
		}
		return;
	}
	if(fPanTouchID!=TouchEvent->GetTouchID()){
		return;
	}
	if(OnPan!=nullptr)
		OnPan();
}
//---------------------------------------------------------------------------
void* cUITouchActionPan::GetTouchID(void)noexcept
{
	return fPanTouchID;
}
//---------------------------------------------------------------------------
