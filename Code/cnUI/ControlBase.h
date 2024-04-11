/*- cnUI - Control base declarations --------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2009-05-11                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnUI_ControlBase_H__
#define __cnLibrary_cnUI_ControlBase_H__

#include <cnUI/UICommon.h>
#include <cnUI/UILayout.h>
#include <cnUI/UIInterface.h>

#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnUI{
//---------------------------------------------------------------------------
static constexpr sfInt16 LayoutOrder_Background	=	-64;
//---------------------------------------------------------------------------
static constexpr Float32 ZPosition_Background		=	-64.f;
static constexpr Float32 ZPosition_Content			=	-32.f;
static constexpr Float32 ZPosition_ContentInset		=	32.f;
//---------------------------------------------------------------------------
class Form : protected UIStateHandler,protected UIViewHandler
{
public:
	Form()noexcept(true);
	~Form()noexcept(true);

	operator iUIView *()const noexcept(true);
	iUIView* GetView(void)const noexcept(true);
protected:
	iPtr<iUIView> fView;
};
//---------------------------------------------------------------------------
class ViewControl : protected UIViewHandler
{
public:
	ViewControl()noexcept(true);
	~ViewControl()noexcept(true);

	iUIView* GetView(void)const noexcept(true);
	void SetView(iUIView *View)noexcept(true);
	iUIView* CreateView(void)noexcept(true);

	
	sfInt16 LayoutOrder=0;
	virtual void UpdateLayoutOrder(void)noexcept(true);
	void SetLayoutOrder(sfInt16 Order)noexcept(true);

protected:
	iPtr<iUIView> fView;

	virtual void ViewSetup(void)noexcept(true);
	virtual void ViewClear(void)noexcept(true);
};
//---------------------------------------------------------------------------
template<class T>
inline iPtr<iUIView> ControlCreateView(T &Control)noexcept(true)
{
	return Control.CreateView();
}
//---------------------------------------------------------------------------
template<class T>
inline iPtr<iUIView> ControlCreateView(T &Control,iUIView *ParentView)noexcept(true)
{
	auto View=Control.CreateView();
	if(ParentView!=nullptr){
		ParentView->InsertView(View);
	}
	return View;
}
//---------------------------------------------------------------------------
template<class T>
inline void ControlDestroyView(T &Control)noexcept(true)
{
	iUIView *View=Control.GetView();
	if(View!=nullptr){
		iUIView *ParentView=View->GetParent();
		if(ParentView!=nullptr){
			ParentView->RemoveView(View);
		}
		Control.SetView(nullptr);
	}
}
//---------------------------------------------------------------------------
template<class T>
inline void ControlDestroyView(T &Control,iUIView *ParentView)noexcept(true)
{
	iUIView *View=Control.GetView();
	if(View!=nullptr){
		if(ParentView!=nullptr){
			ParentView->RemoveView(View);
		}
		Control.SetView(nullptr);
	}
}
//---------------------------------------------------------------------------
template<class T>
inline bool ControlSetParent(const T &Control,iUIView *ParentView)noexcept(true)
{
	return ViewSetParent(Control.GetView(),ParentView);
}
//---------------------------------------------------------------------------
template<class T>
inline bool ControlGetPos(T &Control,iUIArea *Relative,cUIPoint &Pos)noexcept(true)
{
	return ViewGetPos(Control.GetView(),Relative,Pos);
}
//---------------------------------------------------------------------------
template<class T>
inline bool ControlSetPos(T &Control,iUIArea *Relative,const cUIPoint &Pos)noexcept(true)
{
	return ViewSetPos(Control.GetView(),Relative,Pos);
}
//---------------------------------------------------------------------------
template<class T>
inline cUIPoint ControlGetSize(T &Control)noexcept(true)
{
	return ViewGetSize(Control.GetView());
}
//---------------------------------------------------------------------------
template<class T>
inline bool ControlSetSize(T &Control,const cUIPoint &Size)noexcept(true)
{
	return ViewSetSize(Control.GetView(),Size);
}
//---------------------------------------------------------------------------
template<class T>
inline bool ControlGetRect(T &Control,iUIArea *Relative,cUIPoint &Pos,cUIPoint &Size)noexcept(true)
{
	return ViewGetRect(Control.GetView(),Relative,Pos,Size);
}
//---------------------------------------------------------------------------
template<class T>
inline bool ControlSetRect(T &Control,iUIArea *Relative,const cUIPoint &Pos,const cUIPoint &Size)noexcept(true)
{
	return ViewSetRect(Control.GetView(),Relative,Pos,Size);
}
//---------------------------------------------------------------------------
template<class T>
inline bool ControlGetRect(T &Control,iUIArea *Relative,cUIRect &Rect)noexcept(true)
{
	return ViewGetRect(Control.GetView(),Relative,Rect);
}
//---------------------------------------------------------------------------
template<class T>
inline bool ControlSetRect(T &Control,iUIArea *Relative,const cUIRect &Rect)noexcept(true)
{
	return ViewSetRect(Control.GetView(),Relative,Rect);
}
//---------------------------------------------------------------------------
template<class T>
inline bool ControlGetVisible(T &Control)noexcept(true)
{
	return ViewGetVisible(Control.GetView());
}
//---------------------------------------------------------------------------
template<class T>
inline bool ControlSetVisible(T &Control,bool Visible)noexcept(true)
{
	return ViewSetVisible(Control.GetView(),Visible);
}
//---------------------------------------------------------------------------
template<class T>
inline bool ControlGetEnable(T &Control)noexcept(true)
{
	return ViewGetEnable(Control.GetView());
}
//---------------------------------------------------------------------------
template<class T>
inline bool ControlSetEnable(T &Control,bool Enable)noexcept(true)
{
	return ViewSetEnable(Control.GetView(),Enable);
}
//---------------------------------------------------------------------------
template<class T>
inline sfInt16 ControlGetZIndex(T &Control)noexcept(true)
{
	return ViewGetZIndex(Control.GetView());
}
//---------------------------------------------------------------------------
template<class T>
inline bool ControlSetZIndex(T &Control,sfInt16 ZIndex)noexcept(true)
{
	return ViewSetZIndex(Control.GetView(),ZIndex);
}
//---------------------------------------------------------------------------
// Control
//	response user input in a view
class Control : public ViewControl, protected KeyHandler , protected MouseHandler, protected TouchHandler
{
public:
	Control()noexcept(true);
	~Control()noexcept(true);

protected:

	virtual void ViewSetup(void)noexcept(true)override;
	virtual void ViewClear(void)noexcept(true)override;
};
//---------------------------------------------------------------------------
class LayoutControl : public ViewControl
{
public:
	LayoutControl()noexcept(true);
	~LayoutControl()noexcept(true);

	Float32 ControlZOffset=0;
	virtual void UpdateControlZOffset(void)noexcept(true);

	void SetControlZOffset(Float32 ZOffset)noexcept(true);

};
//---------------------------------------------------------------------------
class VisualControl : public LayoutControl
{
public:
	VisualControl()noexcept(true);
	~VisualControl()noexcept(true);

	virtual void UpdateControlZOffset(void)noexcept(true)override;

	void SetContentVisible(bool Visible)noexcept(true);

	void SetBackground(rPtr<viControl> Background)noexcept(true);
	template<class TVisual>
	void SetBackground(TVisual &Visual)noexcept(true){
		SetBackground(rCreate< bwvControl<TVisual&> >(Visual));
	}
protected:

	virtual void ViewSetup(void)noexcept(true)override;
	virtual void ViewClear(void)noexcept(true)override;

	virtual void ControlBackgroundSetDefault(void)noexcept(true);
	virtual Float32 ControlBackgroundZPosition(void)noexcept(true);
	virtual void ControlBackgroundChanged(void)noexcept(true);

	rPtr<viControl> fBackground;
};
//---------------------------------------------------------------------------
class vSimpleViewPainter : protected iUISimplePainter
{
public:
	vSimpleViewPainter()noexcept(true);
	~vSimpleViewPainter()noexcept(true);

	void SetView(iUIView *View)noexcept(true);
	void SetContentZPosition(Float32 ZPosition)noexcept(true);
	void SetContentVisible(bool Visible)noexcept(true);
protected:
	rPtr<iUISimpleViewContent> fViewContent;
	cUIPoint fPaintSize;

	virtual void cnLib_FUNC PaintStarted(void)noexcept(true)override;
	virtual void cnLib_FUNC PaintShow(void)noexcept(true)override;
	virtual void cnLib_FUNC PaintResume(void)noexcept(true)override;
	virtual void cnLib_FUNC PaintPaused(void)noexcept(true)override;
	virtual void cnLib_FUNC PaintHide(void)noexcept(true)override;
	virtual void cnLib_FUNC PaintStopped(void)noexcept(true)override;
	virtual void cnLib_FUNC PaintRectChanged(void)noexcept(true)override;
};
//---------------------------------------------------------------------------
class KeyAction : public ViewControl, protected KeyHandler
{
public:
	KeyAction()noexcept(true);
	~KeyAction()noexcept(true);

	bool KeyFilter=false;
protected:
	bool fKeyRegisterAsFilter;
	virtual void ViewSetup(void)noexcept(true)override;
	virtual void ViewClear(void)noexcept(true)override;
};
//---------------------------------------------------------------------------
class MouseAction : public ViewControl, protected MouseHandler
{
public:
	MouseAction()noexcept(true);
	~MouseAction()noexcept(true);

	bool MouseFilter=false;
protected:
	bool fMouseRegisterAsFilter;
	virtual void ViewSetup(void)noexcept(true)override;
	virtual void ViewClear(void)noexcept(true)override;
};
//---------------------------------------------------------------------------
class TouchAction : public ViewControl, protected TouchHandler
{
public:
	TouchAction()noexcept(true);
	~TouchAction()noexcept(true);

	bool TouchFilter=false;
protected:
	bool fTouchRegisterAsFilter;
	virtual void ViewSetup(void)noexcept(true)override;
	virtual void ViewClear(void)noexcept(true)override;
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
extern void (*gApplyDefaultTextStyle)(cUITextStyle &TextStyle)noexcept(true);
void ApplyDefaultTextStyle(cUITextStyle &TextStyle)noexcept(true);
//---------------------------------------------------------------------------
class cnLib_INTERFACE viControlStateData : public viData
{
public:
	cFunctionSet<void (void)noexcept(true)> ControlStateNotifySet;
	virtual bool ControlHot(void)noexcept(true)=0;
	virtual bool ControlFocused(void)noexcept(true)=0;
	virtual bool ControlDisabled(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
}//	namespace cnUI
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif
