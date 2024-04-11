/*- cnUI - UI common header -----------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2009-05-11                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnUI_UICommon_H__
#define __cnLibrary_cnUI_UICommon_H__

#include <cnSystem/cnSystem.h>
#include <cnRTL/ObjectReference.h>
#include <cnRTL/FunctionSet.h>
#include <cnRTL/UIGraph.h>

#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnUI{
//---------------------------------------------------------------------------
	using cnVar::cAnyPtr;

	using cnRTL::cStringBuffer;
	using cnRTL::cString;

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
bool ViewIsParentOf(iUIView *Parent,iUIView *Child)noexcept(true);
bool ViewSetParent(iUIView *View,iUIView *ParentView)noexcept(true);
bool ViewSetParent(iUIView *View,iUIView *ParentView,iUIView *CurrentParent)noexcept(true);
//---------------------------------------------------------------------------
bool ViewIsPointInside(iUIArea *Area,Float32 x,Float32 y)noexcept(true);
bool ViewGetPos(iUIArea *View,iUIArea *Relative,cUIPoint &Pos)noexcept(true);
bool ViewSetPos(iUIArea *View,iUIArea *Relative,const cUIPoint &Pos)noexcept(true);
cUIPoint ViewGetSize(iUIArea *View)noexcept(true);
bool ViewSetSize(iUIArea *View,const cUIPoint &Pos)noexcept(true);
bool ViewGetRect(iUIArea *View,iUIArea *Relative,cUIPoint &Pos,cUIPoint &Size)noexcept(true);
bool ViewSetRect(iUIArea *View,iUIArea *Relative,const cUIPoint &Pos,const cUIPoint &Size)noexcept(true);
bool ViewGetRect(iUIArea *View,iUIArea *Relative,cUIRect &Rect)noexcept(true);
bool ViewSetRect(iUIArea *View,iUIArea *Relative,const cUIRect &Rect)noexcept(true);
bool ViewGetVisible(iUIView *View)noexcept(true);
bool ViewSetVisible(iUIView *View,bool Visible)noexcept(true);
bool ViewGetEnable(iUIView *View)noexcept(true);
bool ViewSetEnable(iUIView *View,bool Enabled)noexcept(true);
Float32 ViewGetZPosition(iUIView *View)noexcept(true);
bool ViewSetZPosition(iUIView *View,Float32 ZPosition)noexcept(true);
//---------------------------------------------------------------------------
class UIStateHandler : public iUIStateHandler
{
public:
	virtual void cnLib_FUNC UIStarted(void)noexcept(true)override;
	virtual void cnLib_FUNC UIShow(void)noexcept(true)override;
	virtual void cnLib_FUNC UIResume(void)noexcept(true)override;
	virtual void cnLib_FUNC UIPaused(void)noexcept(true)override;
	virtual void cnLib_FUNC UIHide(void)noexcept(true)override;
	virtual void cnLib_FUNC UIStopped(void)noexcept(true)override;
};
//---------------------------------------------------------------------------
class UIViewHandler : public iUIViewHandler
{
public:
	virtual void cnLib_FUNC UILayout(void)noexcept(true)override;
	virtual cUIRectangle cnLib_FUNC UIMargin(const cUIRectangle &Margin)noexcept(true)override;
	virtual void cnLib_FUNC WindowChanged(void)noexcept(true)override;
	virtual void cnLib_FUNC ParentChanged(void)noexcept(true)override;
	virtual void cnLib_FUNC ContentScaleChanged(void)noexcept(true)override;
	virtual void cnLib_FUNC RectangleChanged(bool Moved,bool Sized)noexcept(true)override;
	virtual void cnLib_FUNC ZPositionChanged(void)noexcept(true)override;
	virtual void cnLib_FUNC VisibleChanged(void)noexcept(true)override;
	virtual void cnLib_FUNC EnableChanged(void)noexcept(true)override;
};
//---------------------------------------------------------------------------
class KeyHandler : protected iUIKeyHandler
{
protected:
	virtual void cnLib_FUNC KeyFocusEnterRange(iUIKeyEvent *KeyEvent)noexcept(true)override;
	virtual void cnLib_FUNC KeyFocusLeaveRange(iUIKeyEvent *KeyEvent)noexcept(true)override;
	virtual void cnLib_FUNC KeyFocusEnter(iUIKeyEvent *KeyEvent)noexcept(true)override;
	virtual void cnLib_FUNC KeyFocusLeave(iUIKeyEvent *KeyEvent)noexcept(true)override;
	virtual void cnLib_FUNC KeyDown(iUIKeyEvent *KeyEvent,eKeyCode KeyCode)noexcept(true)override;
	virtual void cnLib_FUNC KeyUp(iUIKeyEvent *KeyEvent,eKeyCode KeyCode)noexcept(true)override;
	virtual void cnLib_FUNC KeyInput(iUIKeyEvent *KeyEvent,eKeyCode KeyCode)noexcept(true)override;
};
//---------------------------------------------------------------------------
class TextInputHandler : protected iUITextInputHandler
{
protected:
	virtual void cnLib_FUNC TextInput(const uChar16 *Text,uIntn Length)noexcept(true)override;
	virtual void cnLib_FUNC TextCompositionStarted(void)noexcept(true)override;
	virtual void cnLib_FUNC TextCompositionFinished(void)noexcept(true)override;
	virtual void cnLib_FUNC TextCompositionUpdate(sfInt32 CaretPos,const uChar16 *Text,uIntn TextLength)noexcept(true)override;
};
//---------------------------------------------------------------------------
class MouseHandler : protected iUIMouseHandler
{
protected:
	virtual void cnLib_FUNC MouseEnterRange(iUIMouseEvent *MouseEvent)noexcept(true)override;
	virtual void cnLib_FUNC MouseLeaveRange(iUIMouseEvent *MouseEvent)noexcept(true)override;
	virtual void cnLib_FUNC MouseEnter(iUIMouseEvent *MouseEvent)noexcept(true)override;
	virtual void cnLib_FUNC MouseLeave(iUIMouseEvent *MouseEvent)noexcept(true)override;
	virtual iInterface* cnLib_FUNC MouseGetCursor(iUIMouseEvent *MouseEvent)noexcept(true)override;
	virtual void cnLib_FUNC MouseMove(iUIMouseEvent *MouseEvent)noexcept(true)override;
	virtual void cnLib_FUNC MouseDown(iUIMouseEvent *MouseEvent,eMouseButton Button)noexcept(true)override;
	virtual void cnLib_FUNC MouseUp(iUIMouseEvent *MouseEvent,eMouseButton Button)noexcept(true)override;
	virtual void cnLib_FUNC MouseWheel(iUIMouseEvent *MouseEvent,Float32 ScrollX,Float32 ScrollY)noexcept(true)override;
};
//---------------------------------------------------------------------------
class TouchHandler : protected iUITouchHandler
{
protected:
	virtual void cnLib_FUNC TouchBegin(iUITouchEvent *TouchEvent)noexcept(true)override;
	virtual void cnLib_FUNC TouchEnd(iUITouchEvent *TouchEvent)noexcept(true)override;
	virtual void cnLib_FUNC TouchLost(iUITouchEvent *TouchEvent)noexcept(true)override;
	virtual void cnLib_FUNC TouchMove(iUITouchEvent *TouchEvent)noexcept(true)override;
};
//---------------------------------------------------------------------------
iUIKeyControl* ViewInsertKeyHandler(iUIView *View,iUIKeyHandler *Handler,sfInt16 Order=0)noexcept(true);
iUIKeyControl* ViewRemoveKeyHandler(iUIView *View,iUIKeyHandler *Handler)noexcept(true);
iUIKeyControl* ViewInsertKeyHandlerFilter(iUIView *View,iUIKeyHandler *Handler,sfInt16 Order=0)noexcept(true);
iUIKeyControl* ViewRemoveKeyHandlerFilter(iUIView *View,iUIKeyHandler *Handler)noexcept(true);
//---------------------------------------------------------------------------
iUIKeyControl* ViewInsertTextInputHandler(iUIView *View,iUITextInputHandler *Handler,sfInt16 Order=0)noexcept(true);
iUIKeyControl* ViewRemoveTextInputHandler(iUIView *View,iUITextInputHandler *Handler)noexcept(true);
//---------------------------------------------------------------------------
iUIMouseControl* ViewInsertMouseHandler(iUIView *View,iUIMouseHandler *Handler,sfInt16 Order=0)noexcept(true);
iUIMouseControl* ViewRemoveMouseHandler(iUIView *View,iUIMouseHandler *Handler)noexcept(true);
iUIMouseControl* ViewInsertMouseHandlerFilter(iUIView *View,iUIMouseHandler *Handler,sfInt16 Order=0)noexcept(true);
iUIMouseControl* ViewRemoveMouseHandlerFilter(iUIView *View,iUIMouseHandler *Handler)noexcept(true);
//---------------------------------------------------------------------------
iUITouchControl* ViewInsertTouchHandler(iUIView *View,iUITouchHandler *Handler,sfInt16 Order=0)noexcept(true);
iUITouchControl* ViewRemoveTouchHandler(iUIView *View,iUITouchHandler *Handler)noexcept(true);
iUITouchControl* ViewInsertTouchHandlerFilter(iUIView *View,iUITouchHandler *Handler,sfInt16 Order=0)noexcept(true);
iUITouchControl* ViewRemoveTouchHandlerFilter(iUIView *View,iUITouchHandler *Handler)noexcept(true);
//---------------------------------------------------------------------------
template<uIntn Length>
inline iPtr<iUIFont> QueryFont(const uChar16 (&Name)[Length])noexcept(true){
	return cnSystem::QueryFont(Name,cnString::FindLength(Name,Length));
}
//---------------------------------------------------------------------------
template<class TPtr>
inline typename cnVar::TTypeConditional<iPtr<iUIFont>,
	cnVar::TIsPointerOf<uChar16,typename cnVar::TRemoveReference<TPtr>::Type>::Value
>::Type QueryFont(TPtr&& Name)noexcept(true){
	return cnSystem::QueryFont(Name,cnString::FindLength(Name));
}
//---------------------------------------------------------------------------
}//	namespace cnUI
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif


