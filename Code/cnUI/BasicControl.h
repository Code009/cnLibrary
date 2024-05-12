/*- cnUI - Basic Controls -------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2009-03-22                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnUI_BasicControl_H__
#define __cnLibrary_cnUI_BasicControl_H__
/*-------------------------------------------------------------------------*/
#include <cnUI/ControlBase.h>

#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnUI{
//---------------------------------------------------------------------------
class lFrame : public VisualControl
{
public:
	lFrame()noexcept(true);
	~lFrame()noexcept(true);

	iUIView* GetClientView(void)noexcept(true);
	void SetClientView(iUIView *View)noexcept(true);

	cUIRectangle ClientMargin;

protected:
	iPtr<iUIView> fClientView;

	virtual void ViewSetup(void)noexcept(true)override;
	virtual void ViewClear(void)noexcept(true)override;
	virtual void cnLib_FUNC UILayout(void)noexcept(true)override;
};
//---------------------------------------------------------------------------
class cContent : public Control
{
public:
	cContent()noexcept(true);
	~cContent()noexcept(true);

	Float32 ContentZPosition=ZPosition_Content;
	void UpdateContentZPosition(void)noexcept(true);

	void SetContent(rPtr<viControl> Content)noexcept(true);
	template<class TVisual>
	void SetContent(TVisual &Visual)noexcept(true){
		SetContent(rCreate< bwvControl<TVisual&> >(Visual));
	}

protected:
	rPtr<viControl> fContent;
	virtual void ControlContentSetDefault(void)noexcept(true);
	virtual void ControlContentChanged(void)noexcept(true);

	virtual void ViewSetup(void)noexcept(true)override;
	virtual void ViewClear(void)noexcept(true)override;
};
//---------------------------------------------------------------------------
class cVisual : public VisualControl
{ 
public:
	cVisual()noexcept(true);
	~cVisual()noexcept(true);

	void SetContent(rPtr<viControl> Content)noexcept(true);
	template<class TVisual>
	void SetContent(TVisual &Visual)noexcept(true){
		SetContent(rCreate< bwvControl<TVisual&> >(Visual));
	}

	virtual void UpdateControlZOffset(void)noexcept(true)override;

protected:
	rPtr<viControl> fContent;
	virtual Float32 ControlContentZPosition(void)noexcept(true);
	virtual void ControlContentSetDefault(void)noexcept(true);

	virtual void ViewSetup(void)noexcept(true)override;
	virtual void ViewClear(void)noexcept(true)override;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE viTextControlData : public viData
{
public:
	cFunctionSet<void (void)noexcept(true)> ControlTextNotifySet;
	virtual const uChar16* ControlTextString(void)noexcept(true)=0;
	virtual uIntn ControlTextLength(void)noexcept(true)=0;
	virtual const cUITextStyle& ControlTextStyle(void)noexcept(true)=0;
	virtual cUIRectangle ControlTextMargin(void)noexcept(true)=0;
	virtual eAlignment ControlTextAlignment(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class vLabel : public vSimpleViewPainter
{
public:
	vLabel(viTextControlData *Data=nullptr)noexcept(true);
	~vLabel()noexcept(true);

	static rPtr<viControl> Create(viTextControlData *Data)noexcept(true);

	viTextControlData* GetData(void)const noexcept(true);
	void SetData(viTextControlData *Data)noexcept(true);

protected:

	dPtr<viTextControlData> fData;

	virtual void cnLib_FUNC Paint(iUISimplePaintContext *Context)noexcept(true)override;
private:
	void DataInsertCallback(void)noexcept(true);
	void DataRemoveCallback(void)noexcept(true);

	iPtr<iUITextGraph> fCacheTextGraph;
	cUIPoint fTextGraphSize;
	bool fUpdateText;

	void SetupTextCache(void)noexcept(true);

	iFunctionToken *fTextControlNotifyToken;
	void Update(void)noexcept(true);

};
//---------------------------------------------------------------------------
class cLabel : public cVisual, public viTextControlData
{ 
public:
	cLabel()noexcept(true);
	~cLabel()noexcept(true);

	cString<uChar16> Text;
	eAlignment TextAlign=Alignment::Center;

	void Update(void)noexcept(true);
	cUITextStyle TextStyle;

	// viTextControlData

	virtual const uChar16* ControlTextString(void)noexcept(true)override;
	virtual uIntn ControlTextLength(void)noexcept(true)override;
	virtual const cUITextStyle& ControlTextStyle(void)noexcept(true)override;
	virtual cUIRectangle ControlTextMargin(void)noexcept(true)override;
	virtual eAlignment ControlTextAlignment(void)noexcept(true)override;

protected:

	virtual void ControlContentSetDefault(void)noexcept(true)override;
};
//---------------------------------------------------------------------------
cnLib_ENUM_BEGIN(ufInt8,ButtonState)
{
	Normal,
	Hot,
	Pressed,
	Disabled,
	Inactive,
}cnLib_ENUM_END(ButtonState);
//---------------------------------------------------------------------------
class cnLib_INTERFACE viButtonData : public viData
{
public:
	cFunctionSet<void (void)noexcept(true)> ButtonNotifySet;
	virtual eButtonState ButtonState(void)noexcept(true)=0;
	virtual ufInt8 CheckState(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class vButton : public ViewControl, public vSimpleViewPainter
{
public:
	vButton(viButtonData *Data=nullptr)noexcept(true);
	~vButton()noexcept(true);

	static rPtr<viControl> Create(viButtonData *Data)noexcept(true);
	void SetView(iUIView *View)noexcept(true);
	void SetContentZPosition(Float32 ZPosition)noexcept(true);

	viButtonData* GetData(void)const noexcept(true);
	void SetData(viButtonData *Data)noexcept(true);

protected:
	dPtr<viButtonData> fData;

	virtual void cnLib_FUNC Paint(iUISimplePaintContext *Context)noexcept(true)override;
	void Update(void)noexcept(true);
	
	virtual cUIRectangle cnLib_FUNC UIMargin(const cUIRectangle &Margin)noexcept(true)override;

	static const cUIColor StateColorNormal;
	static const cUIColor StateColorHot;
	static const cUIColor StateColorPressed;
	static const cUIColor StateColorDisabled;
private:
	void DataInsertCallback(void)noexcept(true);
	void DataRemoveCallback(void)noexcept(true);

	iFunctionToken *fButtonNotifyToken;
};
//---------------------------------------------------------------------------
extern rPtr<viControl> (*gCreateDefaultButtonVisual)(viButtonData *Data)noexcept(true);
//---------------------------------------------------------------------------
class bcButton : public Control, public viButtonData
{
public:
	bcButton()noexcept(true);
	~bcButton()noexcept(true);

	Float32 BackgroundZPosition=ZPosition_Background;
	void UpdateZPosition(void)noexcept(true);

	void SetBackground(rPtr<viControl> Background)noexcept(true);
	template<class TVisual>
	void SetBackground(TVisual &Visual)noexcept(true){
		SetBackground(rCreate< bwvControl<TVisual&> >(Visual));
	}

	virtual eButtonState ButtonState(void)noexcept(true)override;
	virtual ufInt8 CheckState(void)noexcept(true)override;

protected:
	bool fHot		:1;
	bool fMouseDown	:1;
	bool fTouchDown	:1;
	eMouseButton fMouseDownBtn;

	eButtonState GetButtonState(void)const noexcept(true);

	rPtr<viControl> fBackground;


	void* fTouchDownPointID;

	virtual void ViewSetup(void)noexcept(true)override;
	virtual void ViewClear(void)noexcept(true)override;

	virtual void cnLib_FUNC MouseEnter(iUIMouseEvent *MouseEvent)noexcept(true)override;
	virtual void cnLib_FUNC MouseLeave(iUIMouseEvent *MouseEvent)noexcept(true)override;
	virtual void cnLib_FUNC MouseMove(iUIMouseEvent *MouseEvent)noexcept(true)override;
	virtual void cnLib_FUNC MouseDown(iUIMouseEvent *MouseEvent,eMouseButton Button)noexcept(true)override;
	virtual void cnLib_FUNC MouseUp(iUIMouseEvent *MouseEvent,eMouseButton Button)noexcept(true)override;

	virtual void cnLib_FUNC TouchDown(iUITouchEvent *TouchEvent)noexcept(true)override;
	virtual void cnLib_FUNC TouchUp(iUITouchEvent *TouchEvent)noexcept(true)override;
	virtual void cnLib_FUNC TouchLost(iUITouchEvent *TouchEvent)noexcept(true)override;
	virtual void cnLib_FUNC TouchMove(iUITouchEvent *TouchEvent)noexcept(true)override;

	virtual void ButtonBackgroundSetDefault(void)noexcept(true);
	virtual void ButtonBackgroundChanged(void)noexcept(true);

	virtual bool ButtonMouseAllowButton(eMouseButton Button)noexcept(true){	UnusedParameter(Button);	return true;	}
	virtual void ButtonClick(const cUIPoint &Pos)noexcept(true){UnusedParameter(Pos);	}
	virtual void ButtonStateChanged(void)noexcept(true);

};
//---------------------------------------------------------------------------
class cButton : public bcButton
{
public:
	cButton()noexcept(true);
	~cButton()noexcept(true);

	cFunction<void (void)> OnClick;

protected:
	virtual void ButtonClick(const cUIPoint &Pos)noexcept(true)override;
};
//---------------------------------------------------------------------------
class bcTextButton : public bcButton, public viTextControlData
{
public:
	bcTextButton()noexcept(true);
	~bcTextButton()noexcept(true);

	cUITextStyle TextStyle;
	eAlignment TextAlign=Alignment::Center;
	sfInt8 ContentMarginLeft=0;
	sfInt8 ContentMarginTop=0;
	sfInt8 ContentMarginRight=0;
	sfInt8 ContentMarginBottom=0;

	Float32 ContentZPosition=ZPosition_Content;
	void UpdateZPosition(void)noexcept(true);

	void SetContent(rPtr<viControl> Content)noexcept(true);
	template<class TVisual>
	void SetContent(TVisual &Visual)noexcept(true){
		SetContent(rCreate< bwvControl<TVisual&> >(Visual));
	}


	// viTextControlData
	
	virtual const uChar16* ControlTextString(void)noexcept(true)override;
	virtual uIntn ControlTextLength(void)noexcept(true)override;
	virtual const cUITextStyle& ControlTextStyle(void)noexcept(true)override;
	virtual cUIRectangle ControlTextMargin(void)noexcept(true)override;
	virtual eAlignment ControlTextAlignment(void)noexcept(true)override;

protected:

	virtual void ViewSetup(void)noexcept(true)override;
	virtual void ViewClear(void)noexcept(true)override;

	rPtr<viControl> fContent;
	virtual void ButtonContentSetDefault(void)noexcept(true);
};
//---------------------------------------------------------------------------
class cTextButton : public bcTextButton
{
public:
	cTextButton()noexcept(true);
	~cTextButton()noexcept(true);

	cString<uChar16> Text;
	ufInt8 Check=0;

	void Update(void)noexcept(true);
	void UpdateState(void)noexcept(true);

	// viButtonData

	virtual ufInt8 CheckState(void)noexcept(true)override;

	// viTextControlData

	virtual const uChar16* ControlTextString(void)noexcept(true)override;
	virtual uIntn ControlTextLength(void)noexcept(true)override;
	
	cFunction<void (void)> OnClick;

protected:
	virtual bool ButtonMouseAllowButton(eMouseButton Button)noexcept(true)override;
	virtual void ButtonClick(const cUIPoint &Pos)noexcept(true)override;
};
//---------------------------------------------------------------------------
cnLib_ENUM_BEGIN(ufInt8,ScrollBarButton)
{
	None,
	Dec,
	DecBar,
	Track,
	IncBar,
	Inc,
}cnLib_ENUM_END(ScrollBarButton);
//---------------------------------------------------------------------------
class cnLib_INTERFACE viScrollBarData : public viData
{
public:
	cFunctionSet<void (void)noexcept(true)> ScrollBarNotifySet;
	virtual eScrollBarButton ScrollBarActiveButton(void)noexcept(true)=0;
	virtual eButtonState ScrollBarButtonState(void)noexcept(true)=0;

	struct cScrollPosition
	{
		cUIRange View;
		cUIRange Total;
	};
	virtual cScrollPosition ScrollBarPosition(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE viScrollBar : public viControl
{
public:
	virtual eScrollBarButton ScrollHitTest(Float32 x,Float32 y)noexcept(true)=0;
	virtual Float32 ScrollOffsetToValue(Float32 OffsetX,Float32 OffsetY)noexcept(true)=0;
};
//---------------------------------------------------------------------------
template<class TClass,class TInterface=viScrollBar>
class bwvScrollBar : public bwvControl<TClass,TInterface>
{
public:
	using bwvControl<TClass,TInterface>::bwvControl;

	virtual eScrollBarButton ScrollHitTest(Float32 x,Float32 y)noexcept(true)override{
		return (*this)->ScrollHitTest(x,y);
	}
	virtual Float32 ScrollOffsetToValue(Float32 OffsetX,Float32 OffsetY)noexcept(true)override{
		return (*this)->ScrollOffsetToValue(OffsetX,OffsetY);
	}
};
//---------------------------------------------------------------------------
extern rPtr<viScrollBar> (*gCreateDefaultScrollBarVisual)(viScrollBarData *Data)noexcept(true);
//---------------------------------------------------------------------------
class bcScrollBar : public Control, public viScrollBarData
{
public:
	bcScrollBar()noexcept(true);
	~bcScrollBar()noexcept(true);

	Float32 ContentZPosition=ZPosition_Content;
	void UpdateZPosition(void)noexcept(true);

	void SetContent(rPtr<viScrollBar> Content)noexcept(true);
	template<class TVisual>
	void SetContent(TVisual &Visual)noexcept(true){
		SetContent(rCreate< bwvScrollBar<TVisual&> >(Visual));
	}

	virtual eScrollBarButton ScrollBarActiveButton(void)noexcept(true)override;
	virtual eButtonState ScrollBarButtonState(void)noexcept(true)override;
	virtual cScrollPosition ScrollBarPosition(void)noexcept(true)override;

protected:
	eScrollBarButton fActiveButton=ScrollBarButton::None;
	bool fHot			:1;
	bool fButtonDown	:1;

	rPtr<viScrollBar> fContent;
	virtual void ControlContentSetDefault(void)noexcept(true);


	virtual void ViewSetup(void)noexcept(true)override;
	virtual void ViewClear(void)noexcept(true)override;

	virtual void cnLib_FUNC MouseEnter(iUIMouseEvent *MouseEvent)noexcept(true)override;
	virtual void cnLib_FUNC MouseLeave(iUIMouseEvent *MouseEvent)noexcept(true)override;
	virtual void cnLib_FUNC MouseMove(iUIMouseEvent *MouseEvent)noexcept(true)override;
	virtual void cnLib_FUNC MouseDown(iUIMouseEvent *MouseEvent,eMouseButton Button)noexcept(true)override;
	virtual void cnLib_FUNC MouseUp(iUIMouseEvent *MouseEvent,eMouseButton Button)noexcept(true)override;
	virtual void cnLib_FUNC MouseWheel(iUIMouseEvent *MouseEvent,Float32 ScrollX,Float32 ScrollY)noexcept(true)override;

	void ScrollBarNotify(void)noexcept(true);

	virtual void ScrollBarMove(Float32 Offset)noexcept(true);
	enum eScrollBarMoveDistance{
		sbMovePage,
		sbMoveStep,
	};
	virtual void ScrollBarMoveAction(bool Negative,eScrollBarMoveDistance Distance)noexcept(true);
private:

	class cMousePressTimerProc : public iProcedure
	{
		virtual void cnLib_FUNC Execute(void)noexcept(true)override;
	}fMousePressTimerProc;
	rPtr<iAsyncTimer> fMousePressTimer;

	iPtr<iUIMouse> fActiveMouse;
	cUIPoint fTrackPos;
	void MousePress(void)noexcept(true);
};
//---------------------------------------------------------------------------
class cScrollBar : public bcScrollBar
{
public:
	cScrollBar()noexcept(true);
	~cScrollBar()noexcept(true);

	Float32 ScrollMin=0;
	Float32 ScrollMax=0;
	Float32 ScrollCurrent=0;
	Float32 ScrollViewSize=1;
	Float32 ScrollPageSize=1;
	Float32 ScrollStepSize=1;

	void Update(void)noexcept(true);

	virtual cScrollPosition ScrollBarPosition(void)noexcept(true)override;

	cFunction<void (void)> OnScroll;

protected:
	virtual void ScrollBarMove(Float32 Offset)noexcept(true)override;
	virtual void ScrollBarMoveAction(bool Negative,eScrollBarMoveDistance Distance)noexcept(true)override;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iScrollOffset
{
public:
	cFunctionSet<void (void)noexcept(true)> ScrollOffsetNotifySet;

	virtual Float32 ScrollGetOffset(void)const noexcept(true)=0;
	virtual bool ScrollSetOffset(Float32 Offset)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iControlScrollInfo : public iScrollOffset
{
public:
	cFunctionSet<void (void)noexcept(true)> ScrollInfoNotifySet;

	struct cScrollRange
	{
		Float32 ViewportSize;
		Float32 LowwerLimit;
		Float32 UpperLimit;
		bool NoLowwerLimit;
		bool NoUpperLimit;
	};

	virtual cScrollRange ScrollGetScrollRange(void)const noexcept(true)=0;
	virtual Float32 ScrollGetStepSize(void)const noexcept(true)=0;
	virtual Float32 ScrollGetPageSize(void)const noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cControlScrollBar : public bcScrollBar
{
public:
	cControlScrollBar()noexcept(true);
	~cControlScrollBar()noexcept(true);

	iControlScrollInfo* GetScrollInfo(void)const noexcept(true);
	void SetScrollInfo(iControlScrollInfo *ScrollInfo)noexcept(true);

	virtual cScrollPosition ScrollBarPosition(void)noexcept(true)override;

	bool NeedScrollBar(void)const noexcept(true);

protected:
	iControlScrollInfo *fScrollInfo=nullptr;

	virtual void ScrollBarMove(Float32 Offset)noexcept(true)override;
	virtual void ScrollBarMoveAction(bool Negative,eScrollBarMoveDistance Distance)noexcept(true)override;

	virtual void ScrollInfoChanged(void)noexcept(true);

private:
	iFunctionToken *fScrollInfoNotifyToken;
	iFunctionToken *fScrollOffsetNotifyToken;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE viTabData : public viData
{
public:
	cFunctionSet<void (void)noexcept(true)> TabNotifySet;
	virtual sfInt16 TabActiveIndex(void)noexcept(true)=0;
	virtual sfInt16 TabHotIndex(void)noexcept(true)=0;
	
	virtual uIntn TabCount(void)noexcept(true)=0;
	struct cTabItem{
		const uChar16 *Text;
		uIntn TextLength;
		cUITextStyle TextStyle;
		sfInt16 TabSize;
		sfInt16 TextMargin;
	};
	virtual cTabItem TabGet(uIntn Index)noexcept(true)=0;
};
//---------------------------------------------------------------------------
extern rPtr<viControl> (*gCreateDefaultTabVisual)(viTabData *Data)noexcept(true);
//---------------------------------------------------------------------------
class bcTab : public cContent, public viTabData
{
public:
	bcTab()noexcept(true);
	~bcTab()noexcept(true);

	virtual sfInt16 TabActiveIndex(void)noexcept(true)override;
	virtual sfInt16 TabHotIndex(void)noexcept(true)override;

	virtual uIntn TabCount(void)noexcept(true)override;
	virtual cTabItem TabGet(uIntn Index)noexcept(true)override;

protected:
	void TabNotify(void)noexcept(true);

	virtual void ControlContentSetDefault(void)noexcept(true)override;


	virtual void cnLib_FUNC MouseLeave(iUIMouseEvent *MouseEvent)noexcept(true)override;
	virtual void cnLib_FUNC MouseMove(iUIMouseEvent *MouseEvent)noexcept(true)override;
	virtual void cnLib_FUNC MouseDown(iUIMouseEvent *MouseEvent,eMouseButton Button)noexcept(true)override;
	virtual void cnLib_FUNC MouseUp(iUIMouseEvent *MouseEvent,eMouseButton Button)noexcept(true)override;


	sfInt16 TabHitTest(Float32 x,Float32 y)noexcept(true);

	virtual void TabClick(sfInt16 Index)noexcept(true);

private:
	sfInt16 fHotIndex;
	bool fMouseBtnLeftDown=false;
};
//---------------------------------------------------------------------------
class cTab : public bcTab
{
public:
	cTab()noexcept(true);
	~cTab()noexcept(true);

	virtual sfInt16 TabActiveIndex(void)noexcept(true)override;
	virtual uIntn TabCount(void)noexcept(true)override;
	virtual cTabItem TabGet(uIntn Index)noexcept(true)override;

	struct cItem{
		cnRTL::cString<uChar16> Text;
		sfInt16 ItemWidth;
	};
	cnRTL::cSeqList<cItem> TabList;


	sfInt16 GetActiveIndex(void)const noexcept(true);
	void SetActiveIndex(sfInt16 Index)noexcept(true);
	

	sfInt16 TabTextMargin=0;
	cUITextStyle TextStyle;
	void Update(void)noexcept(true);

	cFunction<void (void)noexcept(true)> OnTabChanged;
	cFunction<void (sfInt16 TabIndex)noexcept(true)> OnClickTab;

protected:
	virtual void TabClick(sfInt16 Index)noexcept(true)override;

	sfInt16 fActiveIndex=-1;
};
//---------------------------------------------------------------------------
extern rPtr<viControl> (*gCreateDefaultSplitterBarVisual)(void)noexcept(true);
//---------------------------------------------------------------------------
class cSplitterBar : public cContent
{
public:
	cSplitterBar()noexcept(true);
	~cSplitterBar()noexcept(true);

	eMouseButton MouseActionButton=MouseButton::Left;

	cFunction<void (Float32 Delta)noexcept(true)> OnDrag;

	iPtr<iInterface> MouseCursorH;
	iPtr<iInterface> MouseCursorV;

protected:

	bool fBtnDown=false;
	bool fHorization;
	cUIPoint fMoveRefPoint;

	virtual void ControlContentSetDefault(void)noexcept(true)override;

	virtual void cnLib_FUNC RectangleChanged(bool Moved,bool Sized)noexcept(true)override;

	virtual void cnLib_FUNC MouseLeave(iUIMouseEvent *MouseEvent)noexcept(true)override;
	virtual iInterface* cnLib_FUNC MouseGetCursor(iUIMouseEvent *MouseEvent)noexcept(true)override;
	virtual void cnLib_FUNC MouseMove(iUIMouseEvent *MouseEvent)noexcept(true)override;
	virtual void cnLib_FUNC MouseDown(iUIMouseEvent *MouseEvent,eMouseButton Button)noexcept(true)override;
	virtual void cnLib_FUNC MouseUp(iUIMouseEvent *MouseEvent,eMouseButton Button)noexcept(true)override;

	void SizeChanged(void)noexcept(true);
};
//---------------------------------------------------------------------------
class lEdgeInsets : public LayoutControl
{
public:
	lEdgeInsets()noexcept(true);
	~lEdgeInsets()noexcept(true);

	iControlComponent* GetInset(eDirection Direction)const noexcept(true);
	void SetInset(rPtr<iControlComponent> Inset,eDirection Direction)noexcept(true);
	template<class TControl>
	void SetInset(TControl &Inset,eDirection Direction)noexcept(true){
		SetInset(rCreate< bwControlComponent<TControl&> >(Inset),Direction);
	}

	cUIRectangle InsetSizes;

protected:

	virtual void ViewSetup(void)noexcept(true)override;
	virtual void ViewClear(void)noexcept(true)override;

	virtual cUIRectangle cnLib_FUNC UIMargin(const cUIRectangle &Margin)noexcept(true)override;
	virtual void cnLib_FUNC UILayout(void)noexcept(true)override;

	struct cInsetItem
	{
		iPtr<iUIView> View;
		rPtr<iControlComponent> Content;
	}fInsets[4];


	cUIRectangle fInsetPos;
};
//---------------------------------------------------------------------------
class cTitledFrame : public VisualControl, public viTextControlData
{
public:
	cTitledFrame()noexcept(true);
	~cTitledFrame()noexcept(true);

	cUIRectangle TextMargin;
	cString<uChar16> Text;
	cUITextStyle TextStyle;
	uInt32 BackgroundColor=0;

	void Update(void)noexcept(true);

	virtual const uChar16* ControlTextString(void)noexcept(true)override;
	virtual uIntn ControlTextLength(void)noexcept(true)override;
	virtual const cUITextStyle& ControlTextStyle(void)noexcept(true)override;
	virtual cUIRectangle ControlTextMargin(void)noexcept(true)override;
	virtual eAlignment ControlTextAlignment(void)noexcept(true)override;

protected:
	//virtual void ControlBackgroundSetDefault(void)noexcept(true)override;
};
//---------------------------------------------------------------------------
}	// namespace cnUI
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif
