/*- cnWinUI - Win32 Style Control -----------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2009-03-22                                          */
/*-------------------------------------------------------------------------*/
#pragma once
/*-------------------------------------------------------------------------*/
#include <cnSystem\cnWindows.h>
#include <cnUI\WinUIBase.h>
#include <cnUI\WinUITheme.h>
#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
namespace cnUI{
//---------------------------------------------------------------------------
void SetUIDefaultVisualToWindowsStandardStyle(void);
//---------------------------------------------------------------------------
class vWinRectangle : public ViewControl, protected vWinDCPainter
{
public:
	vWinRectangle();
	~vWinRectangle();

	static rPtr<viControl> Create(uInt32 BorderColor,uInt32 BackgroundColor);

	uInt32 BorderColor=0xFF000000;
	uInt32 BackgroundColor=0;
	void Update(void);
	
	void SetView(iUIView *View);


protected:
	HBRUSH fBGBrush;
	HPEN fBorderPen;
	bool fObjectValid=false;
	void ClearObject(void);

	virtual void cnLib_FUNC Paint(HDC DC,HRGN ClipRegion)override;
	virtual cUIRectangle cnLib_FUNC UIMargin(const cUIRectangle &Margin)override;
};
//---------------------------------------------------------------------------
class vWinBorder : public ViewControl, protected vWinDCPainter
{
public:
	vWinBorder();
	~vWinBorder();

	static rPtr<viControl> Create(UINT edge,UINT grfFlags,uInt32 BkColor=0);

	uInt32 BackgroundColor;
	UINT EdgeFlag;
	UINT BorderFlag;
	void Update(void);

	void SetView(iUIView *View);

protected:
	virtual void cnLib_FUNC Paint(HDC DC,HRGN ClipRegion)override;
	virtual cUIRectangle cnLib_FUNC UIMargin(const cUIRectangle &Margin)override;
};
//---------------------------------------------------------------------------
#if 0
//---------------------------------------------------------------------------
class vWinGroupBox : public LayoutControl, protected vWinDCPainter
{
public:
	vWinGroupBox(viTextControlData *Data=nullptr,viSolidColorData *BackgroundColorData=nullptr);
	~vWinGroupBox();

	static rPtr<viControl> Create(viTextControlData *Data,viSolidColorData *BackgroundColorData,UINT edge,UINT grfFlags);

	viTextControlData* GetTextData(void)const;
	void SetTextData(viTextControlData *Data);
	viSolidColorData* GetColorData(void)const;
	void SetColorData(viSolidColorData *BackgroundColorData);
	void SetData(viTextControlData *Data,viSolidColorData *BackgroundColorData);

	UINT EdgeFlag;
	UINT BorderFlag;
	void Update(void);

	void SetView(iUIView *View);
	void SetContentZPosition(Float32 ZPosition);
protected:
	dPtr<viTextControlData> fData;
	dPtr<viSolidColorData> fBackgroundColorData;
	rPtr<iGDIFontHandle> fFont;

	cUIRectangle CalculateBoxMargin(Float32 TextHeight);
	virtual void cnLib_FUNC Paint(HDC DC)override;
	virtual cUIRectangle cnLib_FUNC UIMargin(const cUIRectangle &Margin)override;
private:

	void TextDataInsertCallback(void);
	void TextDataRemoveCallback(void);
	void ColorDataInsertCallback(void);
	void ColorDataRemoveCallback(void);
	class cSolidColorNotifyProcedure : public iProcedure
	{
		virtual void cnLib_FUNC Execute(void)override;
	}fSolidColorNotifyProcedure;
	class cTextControlNotifyProcedure : public iProcedure
	{
		virtual void cnLib_FUNC Execute(void)override;
	}fTextControlNotifyProcedure;

	void SetupFont(const cUITextStyle &TextStyle);
};
#endif // 0
//---------------------------------------------------------------------------
class cWinGroupBoxPainter_Normal : public cWinGroupBoxPainter
{
public:
	cWinGroupBoxPainter_Normal();
	virtual ~cWinGroupBoxPainter_Normal();

	virtual cUIRectangle GetContentMarginRect(void)override;
	virtual void PaintBox(HDC DC,const RECT &Rect)override;
private:
	HTHEME fTheme;
};
//---------------------------------------------------------------------------
class vWinGroupBox : public ViewControl, protected vWinDCThemePainter
{
public:
	vWinGroupBox(viTextControlData *Data=nullptr);
	~vWinGroupBox();

	static rPtr<viControl> Create(viTextControlData *Data);

	viTextControlData* GetTextData(void)const;
	void SetTextData(viTextControlData *Data);

	void Update(void);

	void SetView(iUIView *View);
protected:
	cWinGroupBoxPainter *fPainter=nullptr;

	dPtr<viTextControlData> fTextData;
	rPtr<iGDIFontHandle> fFont;

	cUIRectangle CalculateBoxMargin(Float32 TextHeight);

	virtual void ThemeSetup(HWND WindowHandle)override;
	virtual void ThemeClear(void)override;

	virtual void cnLib_FUNC Paint(HDC DC,HRGN ClipRegion)override;
	virtual cUIRectangle cnLib_FUNC UIMargin(const cUIRectangle &Margin)override;

private:



	void TextDataInsertCallback(void);
	void TextDataRemoveCallback(void);
	iFunctionToken *fTextControlNotifyToken;

	void SetupFont(const cUITextStyle &TextStyle);
};
//---------------------------------------------------------------------------
class cWinGroupBox : public cTitledFrame
{
public:
	cWinGroupBox();
	~cWinGroupBox();

protected:
	virtual void ControlBackgroundSetDefault(void)override;
};
//---------------------------------------------------------------------------
class cWinNCFramePainter_Normal : public cWinNCFramePainter
{
public:
	cWinNCFramePainter_Normal();
	virtual ~cWinNCFramePainter_Normal();

	virtual void PaintFrame(HDC DC,const RECT &Rect,const RECT &Border,Float32 CaptionHeight,bool Active)override;
	virtual void PaintSmallFrame(HDC DC,const RECT &Rect,const RECT &Border,Float32 CaptionHeight,bool Active)override;
private:
	void Paint(HDC DC,const RECT &Rect,const RECT &Border,Float32 CaptionHeight,bool Active);
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE viWinNCFrameData : public viData
{
public:
	cFunctionSet<void (void)> FrameNotifySet;
	virtual bool FrameIsSmallCaption(void)=0;
	virtual bool FrameIsActivate(void)=0;
	virtual RECT FrameGetBorderSize(void)=0;
	virtual Float32 FrameGetCaptionHeight(void)=0;
};
//---------------------------------------------------------------------------
class vWinNCFrame : public vWinDCThemePainter
{
public:
	vWinNCFrame(viWinNCFrameData *Data=nullptr);
	~vWinNCFrame();

	static rPtr<viControl> Create(viWinNCFrameData *Data);

	viWinNCFrameData* GetData(void)const;
	void SetData(viWinNCFrameData *Data);

protected:
	dPtr<viWinNCFrameData> fData;
	cWinNCFramePainter *fPainter=nullptr;

	virtual void ThemeSetup(HWND WindowHandle)override;
	virtual void ThemeClear(void)override;

	void Update(void);
	virtual void cnLib_FUNC Paint(HDC DC,HRGN ClipRegion)override;

	void DataInsertCallback(void);
	void DataRemoveCallback(void);

	iFunctionToken *fFrameNotifyToken;
};
//---------------------------------------------------------------------------
class cWinNCFrame : public VisualControl, public viWinNCFrameData, protected MouseHandler, protected iWindowMessageHandler
{
public:
	cWinNCFrame();
	~cWinNCFrame();

	Float32 SizeLeft;
	Float32 SizeRight;
	Float32 SizeTop;
	Float32 SizeBottom;
	Float32 CaptionHeight;
	Float32 SizeCorner=20;

	bool SmallCaption=false;

	void SetDefaultBorderSize(void);
	void SetBorderSizeForWindowHandle(HWND WindowHandle);
	void SetBorderSizeForWindowFrame(iWindowFrame *Frame);

	virtual bool FrameIsSmallCaption(void)override;
	virtual bool FrameIsActivate(void)override;
	virtual RECT FrameGetBorderSize(void)override;
	virtual Float32 FrameGetCaptionHeight(void)override;

	DWORD HitTest(const cUIPoint &ViewPos);

	void Update(void);
protected:

	bool fActivate;

	virtual void ControlBackgroundSetDefault(void)override;

	virtual void ViewSetup(void)override;
	virtual void ViewClear(void)override;

	virtual iInterface* cnLib_FUNC MouseGetCursor(iUIMouseEvent *MouseEvent)override;
	virtual void cnLib_FUNC MouseDown(iUIMouseEvent *MouseEvent,eMouseButton Button)override;

	virtual void cnLib_FUNC WindowAttached(void)override;
	virtual void cnLib_FUNC WindowDetached(void)override;
	virtual bool cnLib_FUNC WindowMessage(LRESULT &MsgResult,const cWindowMessageParam &MsgParam)override;
	virtual void cnLib_FUNC WindowMessageProcessed(LRESULT MsgResult,const cWindowMessageParam &MsgParam)override;

};
//---------------------------------------------------------------------------
rPtr<iGDIFontHandle> vSetupTextControl(SIZE &TextSize,const wchar_t *Text,uIntn TextLength,const cUITextStyle &TextStyle);
//---------------------------------------------------------------------------
class bvWinButton : public vWinDCPainter
{
public:
	bvWinButton(viButtonData *ButtonData);
	~bvWinButton();

	viButtonData* GetButtonData(void)const;
	void SetButtonData(viButtonData *Data);
	
	void Update(void);

protected:
	dPtr<viButtonData> fButtonData;


private:
	void ButtonDataInsertCallback(void);
	void ButtonDataRemoveCallback(void);

	iFunctionToken *fButtonDataNotifyToken;
};
//---------------------------------------------------------------------------
class bvWinThemeButton : public vWinDCThemePainter
{
public:
	bvWinThemeButton(viButtonData *ButtonData);
	~bvWinThemeButton();

	viButtonData* GetButtonData(void)const;
	void SetButtonData(viButtonData *Data);
	
	void Update(void);
protected:
	dPtr<viButtonData> fButtonData;


private:
	void ButtonDataInsertCallback(void);
	void ButtonDataRemoveCallback(void);
	iFunctionToken *fButtonDataNotifyToken;
};
//---------------------------------------------------------------------------
class cWinNCButtonPainter_Normal : public cWinNCButtonPainter
{
public:
	cWinNCButtonPainter_Normal();
	virtual ~cWinNCButtonPainter_Normal();

	virtual void PaintCloseButton(HDC DC,const RECT &Rect,eButtonState State)override;
	virtual void PaintMinButton(HDC DC,const RECT &Rect,eButtonState State)override;
	virtual void PaintMaxButton(HDC DC,const RECT &Rect,eButtonState State)override;
	virtual void PaintRestoreButton(HDC DC,const RECT &Rect,eButtonState State)override;
	virtual void PaintSmallCloseButton(HDC DC,const RECT &Rect,eButtonState State)override;
	virtual void PaintHelpButton(HDC DC,const RECT &Rect,eButtonState State)override;
private:
	UINT StateIDFromState(eButtonState State);
};
//---------------------------------------------------------------------------
cnLib_ENUM_BEGIN(ufInt8,NCButtonType)
{
	Close,
	Min,
	Max,
	Restore,
	SmallClose,
	Help,
}cnLib_ENUM_END(NCButtonType);
//---------------------------------------------------------------------------
uIntn WinNCGetButtonList(HWND WindowHandle,uIntn ListCount,eNCButtonType *ListBuffer);
uIntn WinNCGetButtonList(iUIView *View,uIntn ListCount,eNCButtonType *ListBuffer);
void WinNCCommand(iUIView *View,const cUIPoint *Pos,eNCButtonType ButtonType);
//---------------------------------------------------------------------------
class vWinNCButton : public bvWinThemeButton
{
public:
	vWinNCButton(viButtonData *Data=nullptr);
	~vWinNCButton();

	static rPtr<viControl> Create(viButtonData *Data);

	eNCButtonType ButtonType;
protected:
	cWinNCButtonPainter *fPainter=nullptr;

	virtual void ThemeSetup(HWND WindowHandle)override;
	virtual void ThemeClear(void)override;

	virtual void cnLib_FUNC Paint(HDC DC,HRGN ClipRegion)override;
};
//---------------------------------------------------------------------------
class cWinNCButton : public bcButton, protected UIStateHandler
{
public:
	cWinNCButton();
	~cWinNCButton();

	void SetButtonType(eNCButtonType ButtonType);
	
	eButtonState ButtonState(void)override;

protected:

	virtual void ViewSetup(void)override;
	virtual void ViewClear(void)override;

	virtual void cnLib_FUNC UIResume(void)override;
	virtual void cnLib_FUNC UIPaused(void)override;


	vWinNCButton fNCButtonContent;
	virtual void ButtonBackgroundSetDefault(void)override;
	virtual bool ButtonMouseAllowButton(eMouseButton Button)override;
	virtual void ButtonClick(const cUIPoint &Pos)override;
};
//---------------------------------------------------------------------------
class cWinPushButtonPainter_Normal : public cWinPushButtonPainter
{
public:
	cWinPushButtonPainter_Normal();
	virtual ~cWinPushButtonPainter_Normal();

	virtual cUIRectangle GetContentMarginRect(void)override;
	virtual void PaintButton(HDC DC,const RECT &Rect,eButtonState State)override;
private:
	void PaintText(HDC DC,const RECT &TextRect,const wchar_t *Text,uIntn TextLength,HFONT TextFont,eButtonState State);
};
//---------------------------------------------------------------------------
class vWinPushButton : public ViewControl, public bvWinThemeButton
{
public:
	vWinPushButton(viButtonData *ButtonData=nullptr);
	~vWinPushButton();
	
	static rPtr<viControl> Create(viButtonData *Data);

	void SetView(iUIView *View);

protected:
	cWinPushButtonPainter *fPainter=nullptr;

	virtual void ThemeSetup(HWND WindowHandle)override;
	virtual void ThemeClear(void)override;

	virtual void cnLib_FUNC Paint(HDC DC,HRGN ClipRegion)override;
	virtual cUIRectangle cnLib_FUNC UIMargin(const cUIRectangle &Margin)override;
};
//---------------------------------------------------------------------------
class cWinTextButton : public cTextButton
{
protected:
	virtual void ButtonBackgroundSetDefault(void)override;
};
//---------------------------------------------------------------------------
class cWinImageTextButton : public cImageTextButton
{
protected:
	virtual void ButtonBackgroundSetDefault(void)override;
};
//---------------------------------------------------------------------------
class bvWinThemeStateButton : public bvWinThemeButton
{
public:
	bvWinThemeStateButton(viButtonData *ButtonData,viImageTextControlData *TextData);
	~bvWinThemeStateButton();

	viImageTextControlData* GetImageTextData(void)const;
	void SetImageTextData(viImageTextControlData *Data);

protected:
	dPtr<viImageTextControlData> fTextData;
	cWinStateButtonPainter *fPainter=nullptr;
	rPtr<iGDIFontHandle> fFont;
	SIZE fTextSize;

	void SetupText(void);

	void Update(void);
	void UpdateState(void);
	void SetupButtonRects(RECT &BoxRect,RECT &TextRect,const cUIPoint &BoxSize);
	
private:
	void ImageTextDataInsertCallback(void);
	void ImageTextDataRemoveCallback(void);
	iFunctionToken *fImageTextNotifyToken;
};
//---------------------------------------------------------------------------
class cWinStateButton : public bcImageTextButton
{
public:
	cWinStateButton();
	~cWinStateButton();

	cString<uChar16> Text;
	ufInt8 Check=0;

	virtual ufInt8 CheckState(void)override;
	virtual const uChar16* ControlTextString(void)override;
	virtual uIntn ControlTextLength(void)override;
	
	cFunction<void (void)> OnClick;

protected:
	virtual void ButtonBackgroundSetDefault(void)override;
	virtual bool ButtonMouseAllowButton(eMouseButton Button)override;
	virtual void ButtonClick(const cUIPoint &Pos)override;
};
//---------------------------------------------------------------------------
class cWinCheckButtonPainter_Normal : public cWinStateButtonPainter
{
public:
	cWinCheckButtonPainter_Normal();
	virtual ~cWinCheckButtonPainter_Normal();

	virtual cUIPoint GetBoxSize(HDC DC)override;
	virtual void PaintButton(HDC DC,const RECT &Rect,eButtonState State,ufInt8 CheckState)override;
	virtual void PaintText(HDC DC,const RECT &TextRect,const wchar_t *Text,uIntn TextLength,HFONT TextFont,eButtonState State,uInt8 CheckState)override;
private:
};
//---------------------------------------------------------------------------
class vWinCheckBox : public bvWinThemeStateButton
{
public:
	vWinCheckBox(viButtonData *ButtonData=nullptr,viImageTextControlData *TextData=nullptr);
	~vWinCheckBox();
	
	static rPtr<viControl> Create(viButtonData *ButtonData,viImageTextControlData *TextData);

protected:
	virtual void ThemeSetup(HWND WindowHandle)override;
	virtual void ThemeClear(void)override;

	virtual void cnLib_FUNC Paint(HDC DC,HRGN ClipRegion)override;
};
//---------------------------------------------------------------------------
class cWinCheckButton : public cWinStateButton
{
public:
	cWinCheckButton();
protected:
	virtual void ControlContentSetDefault(void)override;
};
//---------------------------------------------------------------------------
class cWinRadioButtonPainter_Normal : public cWinStateButtonPainter
{
public:
	cWinRadioButtonPainter_Normal();
	virtual ~cWinRadioButtonPainter_Normal();

	virtual cUIPoint GetBoxSize(HDC DC)override;
	virtual void PaintButton(HDC DC,const RECT &Rect,eButtonState State,ufInt8 CheckState)override;
	virtual void PaintText(HDC DC,const RECT &TextRect,const wchar_t *Text,uIntn TextLength,HFONT TextFont,eButtonState State,uInt8 CheckState)override;
private:
};
//---------------------------------------------------------------------------
class vWinRadioButton : public bvWinThemeStateButton
{
public:
	vWinRadioButton(viButtonData *ButtonData=nullptr,viImageTextControlData *TextData=nullptr);
	~vWinRadioButton();
	
	static rPtr<viControl> Create(viButtonData *ButtonData,viImageTextControlData *TextData);
protected:

	virtual void ThemeSetup(HWND WindowHandle)override;
	virtual void ThemeClear(void)override;

	virtual void cnLib_FUNC Paint(HDC DC,HRGN ClipRegion)override;
};
//---------------------------------------------------------------------------
class cWinRadioButton : public cWinStateButton
{
public:
	cWinRadioButton();
protected:
	virtual void ControlContentSetDefault(void)override;
};
//---------------------------------------------------------------------------
class cWinSpinButtonPainter_Normal : public cWinSpinButtonPainter
{
public:
	cWinSpinButtonPainter_Normal();
	virtual ~cWinSpinButtonPainter_Normal();

	virtual void PaintUpButton(HDC DC,const RECT &Rect,eButtonState State)override;
	virtual void PaintDownButton(HDC DC,const RECT &Rect,eButtonState State)override;
	virtual void PaintLeftButton(HDC DC,const RECT &Rect,eButtonState State)override;
	virtual void PaintRightButton(HDC DC,const RECT &Rect,eButtonState State)override;
private:
	static UINT ButtonStateFlag(eButtonState State); 
};
//---------------------------------------------------------------------------
#if 0
class cnLib_INTERFACE viSpinData
{
public:
	virtual eButtonState ButtonState(void)=0;
};
//---------------------------------------------------------------------------
class vWinSpinButton : public viControl, protected vWinDCThemePainter
{
public:
	vWinSpinButton(viSpinData *Data);
	~vWinSpinButton();
	
	static rPtr<viControl> Create(viSpinData *Data);

	virtual void SetView(iUIView *View)override;
	virtual void SetZPosition(Float32 ZPosition)override;
	virtual void SetVisible(bool Visible)override;
	virtual void Update(void)override;
protected:
	cWinSpinButtonPainter *fPainter=nullptr;

	virtual void ThemeSetup(HWND WindowHandle)override;
	virtual void ThemeClear(void)override;

	virtual void cnLib_FUNC Paint(HDC DC)override;

	viSpinData *fData;
};
#endif // 0
//---------------------------------------------------------------------------
class vWinDirectionButton :  protected bvWinThemeButton
{
public:
	vWinDirectionButton(viButtonData *ButtonData=nullptr);
	~vWinDirectionButton();
	
	eDirection PointerDirection;
	void Update(void);

	static rPtr<viControl> Create(viButtonData *Data,eDirection PointerDirection);

protected:
	cWinSpinButtonPainter *fPainter=nullptr;

	virtual void ThemeSetup(HWND WindowHandle)override;
	virtual void ThemeClear(void)override;

	virtual void cnLib_FUNC Paint(HDC DC,HRGN ClipRegion)override;
};
//---------------------------------------------------------------------------
class cWinEditBackgroundPainter_Normal : public cWinEditBackgroundPainter
{
public:
	cWinEditBackgroundPainter_Normal();
	virtual ~cWinEditBackgroundPainter_Normal();

	virtual cUIRectangle GetContentMarginRect(void)override;
	virtual void Paint(HDC DC,const RECT &Rect,ControlState State)override;
private:
};
//---------------------------------------------------------------------------
class vWinEditBackground : public ViewControl, public vWinDCThemePainter
{
public:
	vWinEditBackground(viControlStateData *Data=nullptr);
	~vWinEditBackground();

	void SetView(iUIView *View);

	static rPtr<viControl> Create(viControlStateData *Data);

	viControlStateData* GetData(void)const;
	void SetData(viControlStateData *Data);

protected:
	dPtr<viControlStateData> fData;
	cWinEditBackgroundPainter *fPainter=nullptr;

	virtual cUIRectangle cnLib_FUNC UIMargin(const cUIRectangle &Margin)override;

	HTHEME fTheme;
	virtual void ThemeSetup(HWND WindowHandle)override;
	virtual void ThemeClear(void)override;

	virtual void cnLib_FUNC Paint(HDC DC,HRGN ClipRegion)override;
	
	void Update(void);
private:

	void DataInsertCallback(void);
	void DataRemoveCallback(void);
	iFunctionToken *fControlStateDataNotifyToken;
};
//---------------------------------------------------------------------------
class lWinEditFrame : public lFrame
{
protected:
	virtual void ControlBackgroundSetDefault(void)override;
};
//---------------------------------------------------------------------------
class vWinTextCaret : protected UIViewHandler, protected KeyHandler
{
public:
	vWinTextCaret(viTextCaretData *Data=nullptr);
	~vWinTextCaret();

	static rPtr<viControl> Create(viTextCaretData *Data);

	void SetView(iUIView *View);
	void SetContentZPosition(Float32 ZPosition);
	void SetContentVisible(bool Visible);


protected:
	dPtr<viTextCaretData> fData;
	iPtr<iUIView> fView;

	bool fFocused;
	bool fVisible;

	void StateFunction(void);

	bool fCaretCreated;
	bool fCaretVisible;
	int fCaretWidth;
	int fCaretHeight;
	void CaretSetup(HWND WindowHandle,cUIPoint CaretSize);
	void CaretClear(void);

	virtual void cnLib_FUNC UILayout(void)override;
	virtual void cnLib_FUNC KeyFocusEnter(iUIKeyEvent *KeyEvent)override;
	virtual void cnLib_FUNC KeyFocusLeave(iUIKeyEvent *KeyEvent)override;
private:

	void DataInsertCallback(void);
	void DataRemoveCallback(void);
	iFunctionToken *fTextCaretNotifyToken;
};
//---------------------------------------------------------------------------
class cWinTabPainter_Normal : public cWinTabPainter
{
public:
	cWinTabPainter_Normal();
	virtual ~cWinTabPainter_Normal();

	virtual ufInt16n TabBorderSize(void)override;
	virtual void PaintPanelHeader(HDC DC,const RECT &PanelRect)override;

	virtual RECT PaintItemLeft(HDC DC,const RECT &Rect,TabState State)override;
	virtual RECT PaintItemMiddle(HDC DC,const RECT &Rect,TabState State)override;
	virtual RECT PaintItemRight(HDC DC,const RECT &Rect,TabState State)override;
	virtual RECT PaintItemSingle(HDC DC,const RECT &Rect,TabState State)override;
	
	virtual void PaintItemText(HDC DC,const RECT &TextRect,const wchar_t *Text,uIntn TextLength,HFONT TextFont,TabState State)override;

private:
	RECT PaintTabItem(HDC DC,const RECT &Rect,TabState State);

	void TabToTextRect(RECT &Rect,TabState State);

	cnRTL::arSharedObjectRecycler< cSolidPenObject<1,0xCCCCCC> > fPanelHeaderPenRecycler;
};
//---------------------------------------------------------------------------
class vWinTab : public vWinDCThemePainter
{
public:
	vWinTab(viTabData *Data=nullptr);
	~vWinTab();

	static rPtr<viControl> Create(viTabData *Data);

	viTabData* GetData(void)const;
	void SetData(viTabData *Data);

	sfInt16 TabHitTest(Float32 x,Float32 y);

protected:
	dPtr<viTabData> fData;
	cWinTabPainter *fPainter=nullptr;

	virtual void ThemeSetup(HWND WindowHandle)override;
	virtual void ThemeClear(void)override;

	virtual void cnLib_FUNC Paint(HDC DC,HRGN ClipRegion)override;
	void Update(void);
	void UpdateState(void);

private:
	void DataInsertCallback(void);
	void DataRemoveCallback(void);

	iFunctionToken *fTabNotifyToken;

	enum class TabPart : uInt8{
		Middle,
		Left,
		Right,
		Single,
	};
	struct cTabCacheItem
	{
		cnRTL::cString<wchar_t> Text;
		rPtr<iGDIFontHandle> Font;
		Float32 TabPos;
		Float32 TabSize;
		SIZE TextSize;
		sfInt16 TextPos;
		TabPart Part;
	};
	cnRTL::cSeqList<cTabCacheItem> fTabCacheList;
	bool fUpdateTab=false;
	void SetupTabCache(HDC DC);
	typedef cWinTabPainter::TabState TabState;
	void DrawTabItem(HDC DC,const cTabCacheItem &Item,TabState State);
};
//---------------------------------------------------------------------------
class cWinTab : public cTab
{
protected:
	virtual void ControlContentSetDefault(void)override;
};
//---------------------------------------------------------------------------
class cWinScrollBarPainter_Normal : public cWinScrollBarPainter
{
public:
	cWinScrollBarPainter_Normal();
	virtual ~cWinScrollBarPainter_Normal();

	static UINT BtnStateFlag(eButtonState State,bool IsActive);

	virtual Float32 GetMinTrackButtonSize(void)override;
	virtual void PaintDec(const PaintParam &Param)override;
	virtual void PaintInc(const PaintParam &Param)override;
	virtual void PaintDecBar(const PaintParam &Param)override;
	virtual void PaintIncBar(const PaintParam &Param)override;
	virtual void PaintTrack(const PaintParam &Param)override;

private:
	class cBrushBMP
	{
	public:
		cBrushBMP();
		~cBrushBMP();

		HBITMAP BrushPattern;
		HBRUSH Brush;
	protected:
		void CreateBmp32(HDC DC,int w,int h,const void *PixelData);
		void Destroy(void);

		void Setup(HDC DC,bool Pressed);
	};

	void PaintBar(const PaintParam &Param,bool Active);
	class cBrushBMPNormal : public cBrushBMP
	{
	public:
		void Setup(HDC DC);
		static const uInt32 PixelData[];
	};
	cnRTL::arSharedObjectRecycler<cBrushBMPNormal> fNormalBMPRecycler;

	class cBrushBMPPressed : public cBrushBMP
	{
	public:
		void Setup(HDC DC);
		static const uInt32 PixelData[];
	};
	cnRTL::arSharedObjectRecycler<cBrushBMPPressed> fPressedBMPRecycler;
};
//---------------------------------------------------------------------------
class vWinScrollBar : public vWinDCThemePainter
{
public:
	vWinScrollBar(viScrollBarData *Data);
	~vWinScrollBar();

	static rPtr<viScrollBar> Create(viScrollBarData *Data);

	viScrollBarData* GetData(void)const;
	void SetData(viScrollBarData *Data);

	eScrollBarButton ScrollHitTest(Float32 x,Float32 y);
	Float32 ScrollOffsetToValue(Float32 OffsetX,Float32 OffsetY);

protected:
	dPtr<viScrollBarData> fData;
	cWinScrollBarPainter *fPainter=nullptr;

	virtual void ThemeSetup(HWND WindowHandle)override;
	virtual void ThemeClear(void)override;

	virtual void cnLib_FUNC Paint(HDC DC,HRGN ClipRegion)override;
	void Update(void);

private:

	sfInt16 fBtnSize=24;
	bool fIsVertical;

	Float32 fTrackBtnPos;
	Float32 fTrackBtnSize;
	Float32 fTrackSize;
	Float32 fValueSize;
	void CalculateParts(Float32 TotalSize);

	void DataInsertCallback(void);
	void DataRemoveCallback(void);

	iFunctionToken *fScrollBarNotifyToken;
};
//---------------------------------------------------------------------------
class cWinScrollBar : public cScrollBar
{
protected:
	virtual void ControlContentSetDefault(void)override;
};
//---------------------------------------------------------------------------
class cWinControlScrollBar : public cControlScrollBar
{
protected:
	virtual void ControlContentSetDefault(void)override;
};
//---------------------------------------------------------------------------
class cWinScrollBarGripPainter_Normal : public cWinScrollBarGripPainter
{
public:
	cWinScrollBarGripPainter_Normal();
	virtual ~cWinScrollBarGripPainter_Normal();

	virtual void Paint(HDC DC,const RECT &Rect,eAlignment BoxAlign)override;
};
//---------------------------------------------------------------------------
class vWinScrollBarGrip : public vWinDCThemePainter
{
public:
	vWinScrollBarGrip(viScrollBarGripData *Data=nullptr);
	~vWinScrollBarGrip();

	void Update(void);

	static rPtr<viControl> Create(viScrollBarGripData *Data);

	viScrollBarGripData* GetData(void)const;
	void SetData(viScrollBarGripData *Data);

protected:
	cWinScrollBarGripPainter *fPainter=nullptr;
	dPtr<viScrollBarGripData> fData;

	virtual void ThemeSetup(HWND WindowHandle)override;
	virtual void ThemeClear(void)override;

	virtual void cnLib_FUNC Paint(HDC DC,HRGN ClipRegion)override;

	void DataInsertCallback(void);
	void DataRemoveCallback(void);

	iFunctionToken *fScrollBarGripNotifyToken;
};
//---------------------------------------------------------------------------
class kWinControlScrollBar : public kControlScrollBar
{
public:
	kWinControlScrollBar();
	~kWinControlScrollBar();

	static rPtr<kiScrollBar> Create(void);
};
//---------------------------------------------------------------------------
class vWinTreeBranchGraph : public vWinDCPainter
{
public:
	vWinTreeBranchGraph(viTreeBranchData *BranchData=nullptr);
	~vWinTreeBranchGraph();

	static rPtr<viControl> Create(viTreeBranchData *Data);

	viTreeBranchData* GetBranchData(void)const;
	void SetBranchData(viTreeBranchData *Data);

protected:
	dPtr<viTreeBranchData> fBranchData;
	virtual void cnLib_FUNC Paint(HDC DC,HRGN ClipRegion)override;
	void Update(void);

	void DrawBranch(HDC DC,const cUIPoint &Offset,const viTreeBranchData::cTreeBranchItem &Branch);
private:

	struct cTreeLinePen
	{
		cTreeLinePen();
		~cTreeLinePen();

		HPEN Pen;
	};
	cnRTL::arSharedObjectRecycler<cTreeLinePen> fTreeLinePenRecycler;

	void BranchDataInsertCallback(void);
	void BranchDataRemoveCallback(void);

	iFunctionToken *fTreeGraphNotifyToken;
};
//---------------------------------------------------------------------------
class vWinTreeExpandButton : public bvWinButton
{
public:
	vWinTreeExpandButton(viButtonData *Data);
	~vWinTreeExpandButton();
	
	static rPtr<viControl> Create(viButtonData *Data);

protected:
	virtual void cnLib_FUNC Paint(HDC DC,HRGN ClipRegion)override;
	void Update(void);
};
//---------------------------------------------------------------------------
//ComboBox
//ProgressBar
//ToolbAr
//TrackBar
//---------------------------------------------------------------------------
}	// namespace UI
}	// namespace cnLibrary 
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
