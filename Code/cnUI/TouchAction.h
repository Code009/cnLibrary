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
	cUITouchFilter();
	~cUITouchFilter();

	iUIView* GetView(void);
	void SetView(iUIView *View);
};
//---------------------------------------------------------------------------
class cUITouchDistanceDetect
{
private:
	cnRTL::cSeqMap<void*,cUIPoint> fBeginMap;
public:
	bool Begin(sInt32 Distance,iUITouchEvent *TouchEvent,cUIPoint &Pos,iUIArea *Relative);
	bool Move(sInt32 Distance,iUITouchEvent *TouchEvent,cUIPoint &Pos,iUIArea *Relative);
	void End(iUITouchEvent *TouchEvent);
	void Clear(void);
};
//---------------------------------------------------------------------------
class cUITouchActionTap : public cUITouchFilter
{
private:
	void* fTouchID;
	cnRTL::cTime fTapTime;
public:
	cUITouchActionTap();
	~cUITouchActionTap();

	virtual void cnLib_FUNC TouchBegin(iUITouchEvent *Event)override;
	virtual void cnLib_FUNC TouchEnd(iUITouchEvent *Event)override;
	virtual void cnLib_FUNC TouchLost(iUITouchEvent *Event)override;

	cUIPoint TapPos;
	uIntn TapCount;
	cFunction<void (void)> OnTap;
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
		virtual void cnLib_FUNC Execute(void)override;
	}fTimerProcedure;
	void TimerHit(void);
public:
	cUITouchActionLongPress();
	~cUITouchActionLongPress();


	virtual void cnLib_FUNC TouchBegin(iUITouchEvent *Event)override;
	virtual void cnLib_FUNC TouchEnd(iUITouchEvent *Event)override;
	virtual void cnLib_FUNC TouchLost(iUITouchEvent *Event)override;
	virtual void cnLib_FUNC TouchMove(iUITouchEvent *Event)override;

	uIntn DurationNS;

	cUIPoint TapPos;
	cFunction<void (void)> OnPress;
};
//---------------------------------------------------------------------------
class cUITouchActionPan : public cUITouchFilter
{
private:
	void* fPanTouchID;
	cUITouchDistanceDetect fPointMoveDetect;

	void Start(iUITouchEvent *Event,const cUIPoint &Pos);
	void Stop(iUITouchEvent *Event);
protected:
	virtual void cnLib_FUNC TouchBegin(iUITouchEvent *Event)override;
	virtual void cnLib_FUNC TouchEnd(iUITouchEvent *Event)override;
	virtual void cnLib_FUNC TouchLost(iUITouchEvent *Event)override;
	virtual void cnLib_FUNC TouchMove(iUITouchEvent *Event)override;
public:
	cUITouchActionPan();
	~cUITouchActionPan();


	sInt32 DelayLength=0;

	cFunction<bool (const cUIPoint &Pos)> OnStart;
	cFunction<void (void)> OnPan;
	cFunction<void (void)> OnExit;

	void* GetTouchID(void);
};
//---------------------------------------------------------------------------
}	// namespace cnUI
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif
//---------------------------------------------------------------------------
