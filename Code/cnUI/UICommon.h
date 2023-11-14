/*- cnUI - UI common header -----------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2009-05-11                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnUI_UICommon_H__
#define __cnLibrary_cnUI_UICommon_H__

#include <cnSystem/cnSystem.h>
#include <cnRTL/cnRTL.h>

#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnUI{
//---------------------------------------------------------------------------
	using cnVar::cAnyPtr;

	using cnRTL::cStringBuffer;
	using cnRTL::cString;

	using cnRTL::cTime;

	using cnRTL::iCreate;
	using cnRTL::rCreate;
	
	using cnRTL::aCls;
	using cnRTL::aClsRef;
	using cnRTL::aClsConstRef;
	using cnRTL::aClsWeakRef;
	using cnRTL::aClsCreate;
	
	using cnRTL::cFunction;
	using cnRTL::iFunctionToken;
	using cnRTL::cCallbackSet;
	using cnRTL::cFunctionSet;
	using cnRTL::cFunctionSetReference;

	using cnRTL::UIPointZero;
	using cnRTL::UIRectZero;
	using cnRTL::UIRectangleZero;

	using cnRTL::UIColorFromUInt32;

	using cnRTL::cVector;
//---------------------------------------------------------------------------
struct cUIRange
{
	Float32 Begin;
	Float32 End;
};
extern const cUIRange UIRangeZero;
//---------------------------------------------------------------------------
bool ViewIsParentOf(iUIView *Parent,iUIView *Child);
bool ViewSetParent(iUIView *View,iUIView *ParentView);
bool ViewSetParent(iUIView *View,iUIView *ParentView,iUIView *CurrentParent);
//---------------------------------------------------------------------------
bool ViewIsPointInside(iUIArea *Area,Float32 x,Float32 y);
bool ViewGetPos(iUIArea *View,iUIArea *Relative,cUIPoint &Pos);
bool ViewSetPos(iUIArea *View,iUIArea *Relative,const cUIPoint &Pos);
cUIPoint ViewGetSize(iUIArea *View);
bool ViewSetSize(iUIArea *View,const cUIPoint &Pos);
bool ViewGetRect(iUIArea *View,iUIArea *Relative,cUIPoint &Pos,cUIPoint &Size);
bool ViewSetRect(iUIArea *View,iUIArea *Relative,const cUIPoint &Pos,const cUIPoint &Size);
bool ViewGetRect(iUIArea *View,iUIArea *Relative,cUIRect &Rect);
bool ViewSetRect(iUIArea *View,iUIArea *Relative,const cUIRect &Rect);
bool ViewGetVisible(iUIView *View);
bool ViewSetVisible(iUIView *View,bool Visible);
bool ViewGetEnable(iUIView *View);
bool ViewSetEnable(iUIView *View,bool Enabled);
Float32 ViewGetZPosition(iUIView *View);
bool ViewSetZPosition(iUIView *View,Float32 ZPosition);
//---------------------------------------------------------------------------
class UIStateHandler : public iUIStateHandler
{
public:
	virtual void cnLib_FUNC UIStarted(void)override;
	virtual void cnLib_FUNC UIShow(void)override;
	virtual void cnLib_FUNC UIResume(void)override;
	virtual void cnLib_FUNC UIPaused(void)override;
	virtual void cnLib_FUNC UIHide(void)override;
	virtual void cnLib_FUNC UIStopped(void)override;
};
//---------------------------------------------------------------------------
class UIViewHandler : public iUIViewHandler
{
public:
	virtual void cnLib_FUNC UILayout(void)override;
	virtual cUIRectangle cnLib_FUNC UIMargin(const cUIRectangle &Margin)override;
	virtual void cnLib_FUNC WindowChanged(void)override;
	virtual void cnLib_FUNC ParentChanged(void)override;
	virtual void cnLib_FUNC ContentScaleChanged(void)override;
	virtual void cnLib_FUNC RectangleChanged(bool Moved,bool Sized)override;
	virtual void cnLib_FUNC ZPositionChanged(void)override;
	virtual void cnLib_FUNC VisibleChanged(void)override;
	virtual void cnLib_FUNC EnableChanged(void)override;
};
//---------------------------------------------------------------------------
class KeyHandler : protected iUIKeyHandler
{
protected:
	virtual void cnLib_FUNC KeyFocusEnterRange(iUIKeyEvent *KeyEvent)override;
	virtual void cnLib_FUNC KeyFocusLeaveRange(iUIKeyEvent *KeyEvent)override;
	virtual void cnLib_FUNC KeyFocusEnter(iUIKeyEvent *KeyEvent)override;
	virtual void cnLib_FUNC KeyFocusLeave(iUIKeyEvent *KeyEvent)override;
	virtual void cnLib_FUNC KeyDown(iUIKeyEvent *KeyEvent,eKeyCode KeyCode)override;
	virtual void cnLib_FUNC KeyUp(iUIKeyEvent *KeyEvent,eKeyCode KeyCode)override;
	virtual void cnLib_FUNC KeyInput(iUIKeyEvent *KeyEvent,eKeyCode KeyCode)override;
};
//---------------------------------------------------------------------------
class TextInputHandler : protected iUITextInputHandler
{
protected:
	virtual void cnLib_FUNC TextInput(const uChar16 *Text,uIntn Length)override;
	virtual void cnLib_FUNC TextCompositionStarted(void)override;
	virtual void cnLib_FUNC TextCompositionFinished(void)override;
	virtual void cnLib_FUNC TextCompositionUpdate(sfInt32 CaretPos,const uChar16 *Text,uIntn TextLength)override;
};
//---------------------------------------------------------------------------
class MouseHandler : protected iUIMouseHandler
{
protected:
	virtual void cnLib_FUNC MouseEnterRange(iUIMouseEvent *MouseEvent)override;
	virtual void cnLib_FUNC MouseLeaveRange(iUIMouseEvent *MouseEvent)override;
	virtual void cnLib_FUNC MouseEnter(iUIMouseEvent *MouseEvent)override;
	virtual void cnLib_FUNC MouseLeave(iUIMouseEvent *MouseEvent)override;
	virtual iInterface* cnLib_FUNC MouseGetCursor(iUIMouseEvent *MouseEvent)override;
	virtual void cnLib_FUNC MouseMove(iUIMouseEvent *MouseEvent)override;
	virtual void cnLib_FUNC MouseDown(iUIMouseEvent *MouseEvent,eMouseButton Button)override;
	virtual void cnLib_FUNC MouseUp(iUIMouseEvent *MouseEvent,eMouseButton Button)override;
	virtual void cnLib_FUNC MouseWheel(iUIMouseEvent *MouseEvent,Float32 ScrollX,Float32 ScrollY)override;
};
//---------------------------------------------------------------------------
class TouchHandler : protected iUITouchHandler
{
protected:
	virtual void cnLib_FUNC TouchBegin(iUITouchEvent *TouchEvent)override;
	virtual void cnLib_FUNC TouchEnd(iUITouchEvent *TouchEvent)override;
	virtual void cnLib_FUNC TouchLost(iUITouchEvent *TouchEvent)override;
	virtual void cnLib_FUNC TouchMove(iUITouchEvent *TouchEvent)override;
};
//---------------------------------------------------------------------------
iUIKeyControl* ViewInsertKeyHandler(iUIView *View,iUIKeyHandler *Handler,sfInt16 Order=0);
iUIKeyControl* ViewRemoveKeyHandler(iUIView *View,iUIKeyHandler *Handler);
iUIKeyControl* ViewInsertKeyHandlerFilter(iUIView *View,iUIKeyHandler *Handler,sfInt16 Order=0);
iUIKeyControl* ViewRemoveKeyHandlerFilter(iUIView *View,iUIKeyHandler *Handler);
//---------------------------------------------------------------------------
iUIKeyControl* ViewInsertTextInputHandler(iUIView *View,iUITextInputHandler *Handler,sfInt16 Order=0);
iUIKeyControl* ViewRemoveTextInputHandler(iUIView *View,iUITextInputHandler *Handler);
//---------------------------------------------------------------------------
iUIMouseControl* ViewInsertMouseHandler(iUIView *View,iUIMouseHandler *Handler,sfInt16 Order=0);
iUIMouseControl* ViewRemoveMouseHandler(iUIView *View,iUIMouseHandler *Handler);
iUIMouseControl* ViewInsertMouseHandlerFilter(iUIView *View,iUIMouseHandler *Handler,sfInt16 Order=0);
iUIMouseControl* ViewRemoveMouseHandlerFilter(iUIView *View,iUIMouseHandler *Handler);
//---------------------------------------------------------------------------
iUITouchControl* ViewInsertTouchHandler(iUIView *View,iUITouchHandler *Handler,sfInt16 Order=0);
iUITouchControl* ViewRemoveTouchHandler(iUIView *View,iUITouchHandler *Handler);
iUITouchControl* ViewInsertTouchHandlerFilter(iUIView *View,iUITouchHandler *Handler,sfInt16 Order=0);
iUITouchControl* ViewRemoveTouchHandlerFilter(iUIView *View,iUITouchHandler *Handler);
//---------------------------------------------------------------------------
template<uIntn Length>
inline iPtr<iUIFont> QueryFont(const uChar16 (&Name)[Length]){
	return cnSystem::QueryFont(Name,cnString::GetLength(Name,Length));
}
//---------------------------------------------------------------------------
template<class TPtr>
inline typename cnVar::TTypeConditional<iPtr<iUIFont>,
	cnVar::TIsPointerOf<uChar16,typename cnVar::TRemoveReference<TPtr>::Type>::Value
>::Type QueryFont(TPtr&& Name){
	return cnSystem::QueryFont(Name,cnString::GetLength(Name));
}
//---------------------------------------------------------------------------
}//	namespace cnUI
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif


