#include "TouchAction.h"

using namespace cnLibrary;
using namespace cnUI;
using namespace cnRTL;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cUITouchFilter::cUITouchFilter()
{
	fViewTouch=nullptr;
}
//---------------------------------------------------------------------------
cUITouchFilter::~cUITouchFilter()
{
	if(fViewTouch!=nullptr){
		fViewTouch->RemoveTouchFilter(this);
	}
}
//---------------------------------------------------------------------------
iUIView* cUITouchFilter::GetView(void)
{
	return fView;
}
//---------------------------------------------------------------------------
void cUITouchFilter::SetView(iUIView *View)
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
bool cUITouchDistanceDetect::Begin(sInt32 Distance,iUITouchEvent *TouchEvent,cUIPoint &Pos,iUIArea *Relative)
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
bool cUITouchDistanceDetect::Move(sInt32 Distance,iUITouchEvent *TouchEvent,cUIPoint &Pos,iUIArea *Relative)
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
void cUITouchDistanceDetect::End(iUITouchEvent *TouchEvent)
{
	fBeginMap.Remove(TouchEvent->GetTouchID());
}
//---------------------------------------------------------------------------
void cUITouchDistanceDetect::Clear(void)
{
	fBeginMap.Clear();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cUITouchActionTap::cUITouchActionTap()
	: fTapTime(0*Time_1ns)
	, fTouchID(nullptr)
{
}
//---------------------------------------------------------------------------
cUITouchActionTap::~cUITouchActionTap(void)
{
}
//---------------------------------------------------------------------------
void cnLib_FUNC cUITouchActionTap::TouchBegin(iUITouchEvent *TouchEvent)
{
	if(fTouchID!=0)
		return;

	fTouchID=TouchEvent->GetTouchID();
	TouchEvent->GetPosition(fView,TapPos);
}
//---------------------------------------------------------------------------
void cnLib_FUNC cUITouchActionTap::TouchEnd(iUITouchEvent *TouchEvent)
{
	if(TouchEvent->GetTouchID()!=fTouchID){
		return;
	}
	cUIPoint upPos;
	TouchEvent->GetPosition(fView,upPos);
	sIntn dx=static_cast<sIntn>(upPos.x-TapPos.x);
	sIntn dy=static_cast<sIntn>(upPos.y-TapPos.y);
	static sIntn TapSize=10;
	cTime Now;
	Now.SetTimeNow();
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
void cnLib_FUNC cUITouchActionTap::TouchLost(iUITouchEvent *TouchEvent)
{
	if(TouchEvent->GetTouchID()!=fTouchID){
		return;
	}
	fTouchID=nullptr;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cUITouchActionLongPress::cUITouchActionLongPress()
{
	//fPressTimer=cnSystem::UIThread->CreateTimer(nullptr,&fTimerProcedure);
	DurationNS=Time_1s;
}
//---------------------------------------------------------------------------
cUITouchActionLongPress::~cUITouchActionLongPress(void)
{
}
//---------------------------------------------------------------------------
void cnLib_FUNC cUITouchActionLongPress::cTimerProcedure::Execute(void)
{
	auto Host=cnMemory::GetObjectFromMemberPointer(this,&cUITouchActionLongPress::fTimerProcedure);
	Host->TimerHit();
}
//---------------------------------------------------------------------------
void cUITouchActionLongPress::TimerHit(void)
{
	if(fTouchID==nullptr)
		return;

//	fViewTouch->TouchFilterExclusive(this);
	if(OnPress!=nullptr)
		OnPress();
}
//---------------------------------------------------------------------------
void cnLib_FUNC cUITouchActionLongPress::TouchBegin(iUITouchEvent *TouchEvent)
{
	if(fTouchID!=nullptr)
		return;

	fTouchID=TouchEvent->GetTouchID();
	TouchEvent->GetPosition(fView,TapPos);
	cTime TimeNow;
	TimeNow.SetTimeNow();
	fPressTimer->Start(&TimeNow,DurationNS,0);
}
//---------------------------------------------------------------------------
void cnLib_FUNC cUITouchActionLongPress::TouchEnd(iUITouchEvent *TouchEvent)
{
	if(TouchEvent->GetTouchID()!=fTouchID){
		return;
	}
	fPressTimer->Stop();
	fTouchID=nullptr;
}
//---------------------------------------------------------------------------
void cnLib_FUNC cUITouchActionLongPress::TouchLost(iUITouchEvent *TouchEvent)
{
	if(TouchEvent->GetTouchID()!=fTouchID){
		return;
	}
	fPressTimer->Stop();
	fTouchID=nullptr;
}
//---------------------------------------------------------------------------
void cnLib_FUNC cUITouchActionLongPress::TouchMove(iUITouchEvent *TouchEvent)
{
	if(TouchEvent->GetTouchID()!=fTouchID){
		return;
	}
	fPressTimer->Stop();
	fTouchID=nullptr;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cUITouchActionPan::cUITouchActionPan()
{
}
//---------------------------------------------------------------------------
cUITouchActionPan::~cUITouchActionPan()
{
}
//---------------------------------------------------------------------------
void cUITouchActionPan::Start(iUITouchEvent *TouchEvent,const cUIPoint &Pos)
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
	//fViewTouch->TouchFilterExclusive(this);
}
//---------------------------------------------------------------------------
void cUITouchActionPan::Stop(iUITouchEvent *TouchEvent)
{
	fPointMoveDetect.End(TouchEvent);
	if(fPanTouchID==TouchEvent->GetTouchID()){
		if(OnExit!=nullptr)
			OnExit();
		fPanTouchID=nullptr;
	}
}
//---------------------------------------------------------------------------
void cnLib_FUNC cUITouchActionPan::TouchBegin(iUITouchEvent *TouchEvent)
{
	if(fPanTouchID!=nullptr)
		return;

	cUIPoint Pos;
	if(fPointMoveDetect.Begin(DelayLength,TouchEvent,Pos,fView)){
		Start(TouchEvent,Pos);
	}
}
//---------------------------------------------------------------------------
void cnLib_FUNC cUITouchActionPan::TouchEnd(iUITouchEvent *TouchEvent)
{
	Stop(TouchEvent);
}
//---------------------------------------------------------------------------
void cnLib_FUNC cUITouchActionPan::TouchLost(iUITouchEvent *TouchEvent)
{
	Stop(TouchEvent);
}
//---------------------------------------------------------------------------
void cnLib_FUNC cUITouchActionPan::TouchMove(iUITouchEvent *TouchEvent)
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
void* cUITouchActionPan::GetTouchID(void)
{
	return fPanTouchID;
}
//---------------------------------------------------------------------------
