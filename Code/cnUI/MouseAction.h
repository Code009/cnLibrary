/*- cnUI - Mouse gesture --------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2015-01-21                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnUI_MouseAction_H__
#define __cnLibrary_cnUI_MouseAction_H__
/*-------------------------------------------------------------------------*/
#include <cnUI/ControlBase.h>

#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnUI{
//---------------------------------------------------------------------------
class aMouseClick : public MouseAction
{
public:
	eMouseButton ActionButton=MouseButton::Left;
	cFunction<void (void)> OnClick;
protected:
	eMouseButton fClickButton;
	virtual void cnLib_FUNC MouseLeave(iUIMouseEvent *MouseEvent)override;
	virtual void cnLib_FUNC MouseDown(iUIMouseEvent *MouseEvent,eMouseButton Button)override;
	virtual void cnLib_FUNC MouseUp(iUIMouseEvent *MouseEvent,eMouseButton Button)override;

};
//---------------------------------------------------------------------------
class aMouseTap : public MouseAction
{
public:
	eMouseButton ActionButton=MouseButton::Left;
	bool ActionWhenButtonUp=false;
	ufInt16 TapCount=0;
	ufInt16 TapIntervalMS=200;
	cUIPoint GetTapPos(void)const;

	sInt32 CancelDistance=5;
	cFunction<void (void)> OnTap;
protected:
	eMouseButton fClickButton;
	cUIPoint fDownPos;
	sInt64 fDownTime;
	virtual void cnLib_FUNC MouseLeave(iUIMouseEvent *MouseEvent)override;
	virtual void cnLib_FUNC MouseDown(iUIMouseEvent *MouseEvent,eMouseButton Button)override;
	virtual void cnLib_FUNC MouseUp(iUIMouseEvent *MouseEvent,eMouseButton Button)override;

	void ActionTap(void);
};
//---------------------------------------------------------------------------
class aMouseHover : public MouseAction
{
public:
	sfInt32 StayTime=1*Time_1s;
	sfInt32 CancelDistance=5;
	cFunction<void (void)> OnHover;
protected:
	cUIPoint fHoverPos;
	bool fHoverStarted=false;

	virtual void ViewSetup(void)override;
	virtual void ViewClear(void)override;

	//virtual void cnLib_FUNC MouseEnter(iUIMouseEvent *MouseEvent)override;
	virtual void cnLib_FUNC MouseLeave(iUIMouseEvent *MouseEvent)override;
	virtual void cnLib_FUNC MouseMove(iUIMouseEvent *MouseEvent)override;

	class cMouseHoverTimerProc : public iProcedure
	{
		virtual void cnLib_FUNC Execute(void)override;
	}fMouseHoverTimerProc;
	rPtr<iAsyncTimer> fHoverTimer;

	void ActionHover(void);
};
//---------------------------------------------------------------------------
class aMousePan : public MouseAction
{
public:
	sfInt16 DelayX=5;
	sfInt16 DelayY=5;
	eMouseButton ActionButton=MouseButton::Left;
	bool AllowX=true;
	bool AllowY=true;

	cFunction<bool (cUIPoint Pos)> OnAllowScroll;
	
	cFunction<void (cUIPoint Pos)> OnScrollStart;
	cFunction<void (cUIPoint Pos)> OnScrollFinish;
	cFunction<void (cUIPoint Pos)> OnScroll;
protected:
	//virtual void cnLib_FUNC MouseEnterRange(iUIMouseEvent *MouseEvent)override;
	virtual void cnLib_FUNC MouseLeaveRange(iUIMouseEvent *MouseEvent)override;
	virtual void cnLib_FUNC MouseMove(iUIMouseEvent *MouseEvent)override;
	virtual void cnLib_FUNC MouseDown(iUIMouseEvent *MouseEvent,eMouseButton Button)override;
	virtual void cnLib_FUNC MouseUp(iUIMouseEvent *MouseEvent,eMouseButton Button)override;

	class cExclusiveHandler : public MouseHandler
	{
		aMousePan* GetHost(void);
	public:
		//virtual void cnLib_FUNC MouseEnter(iUIMouseEvent *MouseEvent)override;
		//virtual void cnLib_FUNC MouseLeave(iUIMouseEvent *MouseEvent)override;
		//virtual iInterface* cnLib_FUNC MouseGetCursor(iUIMouseEvent *MouseEvent)override;
		virtual void cnLib_FUNC MouseMove(iUIMouseEvent *MouseEvent)override;
		//virtual void cnLib_FUNC MouseDown(iUIMouseEvent *MouseEvent,eMouseButton Button)override;
		virtual void cnLib_FUNC MouseUp(iUIMouseEvent *MouseEvent,eMouseButton Button)override;
		//virtual void cnLib_FUNC MouseWheel(iUIMouseEvent *MouseEvent,sfInt16 ScrollX,sfInt16 ScrollY)override;
		//virtual void cnLib_FUNC MouseHover(iUIMouseEvent *MouseEvent)override;

	}fExclusiveHandler;

	void ExclusiveMouseMove(iUIMouseEvent *MouseEvent);
	void ExclusiveMouseUp(iUIMouseEvent *MouseEvent,eMouseButton Button);
private:

	eMouseButton fDownButton=MouseButton::None;

	cUIPoint fDownPoint;
};
//---------------------------------------------------------------------------
class afMouseActiveContent : public ViewControl, protected MouseHandler, protected KeyHandler
{
public:
	afMouseActiveContent();
	~afMouseActiveContent();

	bool AutoFocus=false;

	void SetActiveView(iUIView *View);
protected:
	virtual void ViewSetup(void)override;
	virtual void ViewClear(void)override;

	virtual void cnLib_FUNC MouseEnterRange(iUIMouseEvent *MouseEvent)override;
	virtual void cnLib_FUNC MouseLeaveRange(iUIMouseEvent *MouseEvent)override;
	virtual void cnLib_FUNC MouseDown(iUIMouseEvent *MouseEvent,eMouseButton Button)override;

	virtual void cnLib_FUNC KeyFocusEnterRange(iUIKeyEvent *KeyEvent)override;
	virtual void cnLib_FUNC KeyFocusLeaveRange(iUIKeyEvent *KeyEvent)override;


private:
	bool fHot=false;
	bool fActiveFocus=false;
	iPtr<iUIView> fActiveView;
	void ApplyActiveViewVisible(void);
};
//---------------------------------------------------------------------------
}	// namespace cnUI
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif
