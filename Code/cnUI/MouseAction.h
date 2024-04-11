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
	cFunction<void (void)noexcept(true)> OnClick;
protected:
	eMouseButton fClickButton;
	virtual void cnLib_FUNC MouseLeave(iUIMouseEvent *MouseEvent)noexcept(true)override;
	virtual void cnLib_FUNC MouseDown(iUIMouseEvent *MouseEvent,eMouseButton Button)noexcept(true)override;
	virtual void cnLib_FUNC MouseUp(iUIMouseEvent *MouseEvent,eMouseButton Button)noexcept(true)override;

};
//---------------------------------------------------------------------------
class aMouseTap : public MouseAction
{
public:
	eMouseButton ActionButton=MouseButton::Left;
	bool ActionWhenButtonUp=false;
	ufInt16 TapCount=0;
	ufInt16 TapIntervalMS=200;
	cUIPoint GetTapPos(void)const noexcept(true);

	sInt32 CancelDistance=5;
	cFunction<void (void)noexcept(true)> OnTap;
protected:
	eMouseButton fClickButton;
	cUIPoint fDownPos;
	sInt64 fDownTime;
	virtual void cnLib_FUNC MouseLeave(iUIMouseEvent *MouseEvent)noexcept(true)override;
	virtual void cnLib_FUNC MouseDown(iUIMouseEvent *MouseEvent,eMouseButton Button)noexcept(true)override;
	virtual void cnLib_FUNC MouseUp(iUIMouseEvent *MouseEvent,eMouseButton Button)noexcept(true)override;

	void ActionTap(void)noexcept(true);
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

	virtual void ViewSetup(void)noexcept(true)override;
	virtual void ViewClear(void)noexcept(true)override;

	//virtual void cnLib_FUNC MouseEnter(iUIMouseEvent *MouseEvent)noexcept(true)override;
	virtual void cnLib_FUNC MouseLeave(iUIMouseEvent *MouseEvent)noexcept(true)override;
	virtual void cnLib_FUNC MouseMove(iUIMouseEvent *MouseEvent)noexcept(true)override;

	class cMouseHoverTimerProc : public iProcedure
	{
		virtual void cnLib_FUNC Execute(void)noexcept(true)override;
	}fMouseHoverTimerProc;
	rPtr<iAsyncTimer> fHoverTimer;

	void ActionHover(void)noexcept(true);
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

	cFunction<bool (cUIPoint Pos)noexcept(true)> OnAllowScroll;
	
	cFunction<void (cUIPoint Pos)noexcept(true)> OnScrollStart;
	cFunction<void (cUIPoint Pos)noexcept(true)> OnScrollFinish;
	cFunction<void (cUIPoint Pos)noexcept(true)> OnScroll;
protected:
	//virtual void cnLib_FUNC MouseEnterRange(iUIMouseEvent *MouseEvent)noexcept(true)override;
	virtual void cnLib_FUNC MouseLeaveRange(iUIMouseEvent *MouseEvent)noexcept(true)override;
	virtual void cnLib_FUNC MouseMove(iUIMouseEvent *MouseEvent)noexcept(true)override;
	virtual void cnLib_FUNC MouseDown(iUIMouseEvent *MouseEvent,eMouseButton Button)noexcept(true)override;
	virtual void cnLib_FUNC MouseUp(iUIMouseEvent *MouseEvent,eMouseButton Button)noexcept(true)override;

	class cExclusiveHandler : public MouseHandler
	{
		aMousePan* GetHost(void)noexcept(true);
	public:
		//virtual void cnLib_FUNC MouseEnter(iUIMouseEvent *MouseEvent)noexcept(true)override;
		//virtual void cnLib_FUNC MouseLeave(iUIMouseEvent *MouseEvent)noexcept(true)override;
		//virtual iInterface* cnLib_FUNC MouseGetCursor(iUIMouseEvent *MouseEvent)noexcept(true)override;
		virtual void cnLib_FUNC MouseMove(iUIMouseEvent *MouseEvent)noexcept(true)override;
		//virtual void cnLib_FUNC MouseDown(iUIMouseEvent *MouseEvent,eMouseButton Button)noexcept(true)override;
		virtual void cnLib_FUNC MouseUp(iUIMouseEvent *MouseEvent,eMouseButton Button)noexcept(true)override;
		//virtual void cnLib_FUNC MouseWheel(iUIMouseEvent *MouseEvent,sfInt16 ScrollX,sfInt16 ScrollY)noexcept(true)override;
		//virtual void cnLib_FUNC MouseHover(iUIMouseEvent *MouseEvent)noexcept(true)override;

	}fExclusiveHandler;

	void ExclusiveMouseMove(iUIMouseEvent *MouseEvent)noexcept(true);
	void ExclusiveMouseUp(iUIMouseEvent *MouseEvent,eMouseButton Button)noexcept(true);
private:

	eMouseButton fDownButton=MouseButton::None;

	cUIPoint fDownPoint;
};
//---------------------------------------------------------------------------
class afMouseActiveContent : public ViewControl, protected MouseHandler, protected KeyHandler
{
public:
	afMouseActiveContent()noexcept(true);
	~afMouseActiveContent()noexcept(true);

	bool AutoFocus=false;

	void SetActiveView(iUIView *View)noexcept(true);
protected:
	virtual void ViewSetup(void)noexcept(true)override;
	virtual void ViewClear(void)noexcept(true)override;

	virtual void cnLib_FUNC MouseEnterRange(iUIMouseEvent *MouseEvent)noexcept(true)override;
	virtual void cnLib_FUNC MouseLeaveRange(iUIMouseEvent *MouseEvent)noexcept(true)override;
	virtual void cnLib_FUNC MouseDown(iUIMouseEvent *MouseEvent,eMouseButton Button)noexcept(true)override;

	virtual void cnLib_FUNC KeyFocusEnterRange(iUIKeyEvent *KeyEvent)noexcept(true)override;
	virtual void cnLib_FUNC KeyFocusLeaveRange(iUIKeyEvent *KeyEvent)noexcept(true)override;


private:
	bool fHot=false;
	bool fActiveFocus=false;
	iPtr<iUIView> fActiveView;
	void ApplyActiveViewVisible(void)noexcept(true);
};
//---------------------------------------------------------------------------
}	// namespace cnUI
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif
