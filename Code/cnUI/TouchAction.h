/*- cnUI - touch action ---------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2015-01-21                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLib_cnUI_TouchAction_H__
#define __cnLib_cnUI_TouchAction_H__
/*-------------------------------------------------------------------------*/
#include <cnUI/ControlBase.h>
#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
namespace cnUI{
//---------------------------------------------------------------------------
class cUITouchFilter : protected TouchHandler
{
protected:
	iPtr<iUIView> fView;
	iUITouchControl *fViewTouch;

public:
	cUITouchFilter()noexcept(true);
	~cUITouchFilter()noexcept(true);

	iUIView* GetView(void)noexcept(true);
	void SetView(iUIView *View)noexcept(true);
};
//---------------------------------------------------------------------------
class cUITouchDistanceDetect
{
private:
	cnRTL::cSeqMap<void*,cUIPoint> fBeginMap;
public:
	bool Begin(sInt32 Distance,iUITouchEvent *TouchEvent,cUIPoint &Pos,iUIArea *Relative)noexcept(true);
	bool Move(sInt32 Distance,iUITouchEvent *TouchEvent,cUIPoint &Pos,iUIArea *Relative)noexcept(true);
	void End(iUITouchEvent *TouchEvent)noexcept(true);
	void Clear(void)noexcept(true);
};
//---------------------------------------------------------------------------
class cUITouchActionTap : public cUITouchFilter
{
private:
	void* fTouchID;
	uInt64 fTapTime;
public:
	cUITouchActionTap()noexcept(true);
	~cUITouchActionTap()noexcept(true);

	virtual void cnLib_FUNC TouchDown(iUITouchEvent *Event)noexcept(true)override;
	virtual void cnLib_FUNC TouchUp(iUITouchEvent *Event)noexcept(true)override;
	virtual void cnLib_FUNC TouchLost(iUITouchEvent *Event)noexcept(true)override;

	cUIPoint TapPos;
	uIntn TapCount;
	cFunction<void (void)noexcept(true)> OnTap;
};
typedef cUITouchActionTap cTouchActionTap;
//---------------------------------------------------------------------------
class cUITouchActionLongPress : public cUITouchFilter
{
private:
	void* fTouchID;
	rPtr<iAsyncTimer> fPressTimer;

	class cTimerProcedure : public iProcedure
	{
		virtual void cnLib_FUNC Execute(void)noexcept(true)override;
	}fTimerProcedure;
	void TimerHit(void)noexcept(true);
public:
	cUITouchActionLongPress()noexcept(true);
	~cUITouchActionLongPress()noexcept(true);


	virtual void cnLib_FUNC TouchDown(iUITouchEvent *Event)noexcept(true)override;
	virtual void cnLib_FUNC TouchUp(iUITouchEvent *Event)noexcept(true)override;
	virtual void cnLib_FUNC TouchLost(iUITouchEvent *Event)noexcept(true)override;
	virtual void cnLib_FUNC TouchMove(iUITouchEvent *Event)noexcept(true)override;

	uIntn DurationNS;

	cUIPoint TapPos;
	cFunction<void (void)noexcept(true)> OnPress;
};
//---------------------------------------------------------------------------
class cUITouchActionPan : public cUITouchFilter
{
private:
	void* fPanTouchID;
	cUITouchDistanceDetect fPointMoveDetect;

	void Start(iUITouchEvent *Event,const cUIPoint &Pos)noexcept(true);
	void Stop(iUITouchEvent *Event)noexcept(true);
protected:
	virtual void cnLib_FUNC TouchDown(iUITouchEvent *Event)noexcept(true)override;
	virtual void cnLib_FUNC TouchUp(iUITouchEvent *Event)noexcept(true)override;
	virtual void cnLib_FUNC TouchLost(iUITouchEvent *Event)noexcept(true)override;
	virtual void cnLib_FUNC TouchMove(iUITouchEvent *Event)noexcept(true)override;
public:
	cUITouchActionPan()noexcept(true);
	~cUITouchActionPan()noexcept(true);


	sInt32 DelayLength=0;

	cFunction<bool (const cUIPoint &Pos)noexcept(true)> OnStart;
	cFunction<void (void)noexcept(true)> OnPan;
	cFunction<void (void)noexcept(true)> OnExit;

	void* GetTouchID(void)noexcept(true);
};
//---------------------------------------------------------------------------
}	// namespace cnUI
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif
//---------------------------------------------------------------------------
