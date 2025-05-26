/*- cnWinUI - Win32 Style Control -----------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2009-03-22                                          */
/*-------------------------------------------------------------------------*/
#pragma once
/*-------------------------------------------------------------------------*/
#include <cnSystem\cnWindows.h>
#include <cnUI\BasicControl.h>
#include <cnUI\ImageControl.h>
#include <cnUI\TextEdit.h>
#include <cnUI\TreeContainer.h>
#include <cnUI\WinUIBase.h>
#include <cnUI\WinUITheme.h>
#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
namespace cnUI{
//---------------------------------------------------------------------------
void SetUIDefaultVisualToWindowsStandardStyle(void)noexcept(true);
//---------------------------------------------------------------------------
class vWinRectangle : public ViewControl, protected vWinDCPainter
{
public:
	vWinRectangle()noexcept(true);
	~vWinRectangle()noexcept(true);

	static rPtr<viControl> Create(uInt32 BorderColor,uInt32 BackgroundColor)noexcept(true);

	uInt32 BorderColor=0xFF000000;
	uInt32 BackgroundColor=0;
	void Update(void)noexcept(true);
	
	void SetView(iUIView *View)noexcept(true);


protected:
	HBRUSH fBGBrush;
	HPEN fBorderPen;
	bool fObjectValid=false;
	void ClearObject(void)noexcept(true);

	virtual void cnLib_FUNC Paint(HDC DC,HRGN ClipRegion)noexcept(true)override;
	virtual cUIRectangle cnLib_FUNC UIMargin(const cUIRectangle &Margin)noexcept(true)override;
};
//---------------------------------------------------------------------------
class vWinBorder : public ViewControl, protected vWinDCPainter
{
public:
	vWinBorder()noexcept(true);
	~vWinBorder()noexcept(true);

	static rPtr<viControl> Create(UINT edge,UINT grfFlags,uInt32 BkColor=0)noexcept(true);

	uInt32 BackgroundColor;
	UINT EdgeFlag;
	UINT BorderFlag;
	void Update(void)noexcept(true);

	void SetView(iUIView *View)noexcept(true);

protected:
	virtual void cnLib_FUNC Paint(HDC DC,HRGN ClipRegion)noexcept(true)override;
	virtual cUIRectangle cnLib_FUNC UIMargin(const cUIRectangle &Margin)noexcept(true)override;
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
	cWinGroupBoxPainter_Normal()noexcept(true);
	virtual ~cWinGroupBoxPainter_Normal()noexcept(true);

	virtual cUIRectangle GetContentMarginRect(void)noexcept(true)override;
	virtual void PaintBox(HDC DC,const RECT &Rect)noexcept(true)override;
private:
	HTHEME fTheme;
};
//---------------------------------------------------------------------------
class vWinGroupBox : public ViewControl, protected vWinDCThemePainter
{
public:
	vWinGroupBox(viTextControlData *Data=nullptr)noexcept(true);
	~vWinGroupBox()noexcept(true);

	static rPtr<viControl> Create(viTextControlData *Data)noexcept(true);

	viTextControlData* GetTextData(void)const noexcept(true);
	void SetTextData(viTextControlData *Data)noexcept(true);

	void Update(void)noexcept(true);

	void SetView(iUIView *View)noexcept(true);
protected:
	cWinGroupBoxPainter *fPainter=nullptr;

	dPtr<viTextControlData> fTextData;
	rPtr<iGDIFontHandle> fFont;

	cUIRectangle CalculateBoxMargin(Float32 TextHeight)noexcept(true);

	virtual void ThemeSetup(HWND WindowHandle)noexcept(true)override;
	virtual void ThemeClear(void)noexcept(true)override;

	virtual void cnLib_FUNC Paint(HDC DC,HRGN ClipRegion)noexcept(true)override;
	virtual cUIRectangle cnLib_FUNC UIMargin(const cUIRectangle &Margin)noexcept(true)override;

private:



	void TextDataInsertCallback(void)noexcept(true);
	void TextDataRemoveCallback(void)noexcept(true);
	iFunctionToken *fTextControlNotifyToken;

	void SetupFont(const cUITextStyle &TextStyle)noexcept(true);
};
//---------------------------------------------------------------------------
class cWinGroupBox : public cTitledFrame
{
public:
	cWinGroupBox()noexcept(true);
	~cWinGroupBox()noexcept(true);

protected:
	virtual void ControlBackgroundSetDefault(void)noexcept(true)override;
};
//---------------------------------------------------------------------------
class cWinNCFramePainter_Normal : public cWinNCFramePainter
{
public:
	cWinNCFramePainter_Normal()noexcept(true);
	virtual ~cWinNCFramePainter_Normal()noexcept(true);

	virtual void PaintFrame(HDC DC,const RECT &Rect,const RECT &Border,Float32 CaptionHeight,bool Active)noexcept(true)override;
	virtual void PaintSmallFrame(HDC DC,const RECT &Rect,const RECT &Border,Float32 CaptionHeight,bool Active)noexcept(true)override;
private:
	void Paint(HDC DC,const RECT &Rect,const RECT &Border,Float32 CaptionHeight,bool Active)noexcept(true);
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE viWinNCFrameData : public viData
{
public:
	cFunctionSet<void (void)noexcept(true)> FrameNotifySet;
	virtual bool FrameIsSmallCaption(void)noexcept(true)=0;
	virtual bool FrameIsActivate(void)noexcept(true)=0;
	virtual RECT FrameGetBorderSize(void)noexcept(true)=0;
	virtual Float32 FrameGetCaptionHeight(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class vWinNCFrame : public vWinDCThemePainter
{
public:
	vWinNCFrame(viWinNCFrameData *Data=nullptr)noexcept(true);
	~vWinNCFrame()noexcept(true);

	static rPtr<viControl> Create(viWinNCFrameData *Data)noexcept(true);

	viWinNCFrameData* GetData(void)const noexcept(true);
	void SetData(viWinNCFrameData *Data)noexcept(true);

protected:
	dPtr<viWinNCFrameData> fData;
	cWinNCFramePainter *fPainter=nullptr;

	virtual void ThemeSetup(HWND WindowHandle)noexcept(true)override;
	virtual void ThemeClear(void)noexcept(true)override;

	void Update(void)noexcept(true);
	virtual void cnLib_FUNC Paint(HDC DC,HRGN ClipRegion)noexcept(true)override;

	void DataInsertCallback(void)noexcept(true);
	void DataRemoveCallback(void)noexcept(true);

	iFunctionToken *fFrameNotifyToken;
};
//---------------------------------------------------------------------------
class cWinNCFrame : public VisualControl, public viWinNCFrameData, protected MouseHandler, protected iWindowMessageHandler
{
public:
	cWinNCFrame()noexcept(true);
	~cWinNCFrame()noexcept(true);

	Float32 SizeLeft;
	Float32 SizeRight;
	Float32 SizeTop;
	Float32 SizeBottom;
	Float32 CaptionHeight;
	Float32 SizeCorner=20;

	bool SmallCaption=false;

	void SetDefaultBorderSize(void)noexcept(true);
	void SetBorderSizeForWindowHandle(HWND WindowHandle)noexcept(true);
	void SetBorderSizeForWindowFrame(iWindowFrame *Frame)noexcept(true);

	virtual bool FrameIsSmallCaption(void)noexcept(true)override;
	virtual bool FrameIsActivate(void)noexcept(true)override;
	virtual RECT FrameGetBorderSize(void)noexcept(true)override;
	virtual Float32 FrameGetCaptionHeight(void)noexcept(true)override;

	DWORD HitTest(const cUIPoint &ViewPos)noexcept(true);

	void Update(void)noexcept(true);
protected:

	bool fActivate;

	virtual void ControlBackgroundSetDefault(void)noexcept(true)override;

	virtual void ViewSetup(void)noexcept(true)override;
	virtual void ViewClear(void)noexcept(true)override;

	virtual iInterface* cnLib_FUNC MouseGetCursor(iUIMouseEvent *MouseEvent)noexcept(true)override;
	virtual void cnLib_FUNC MouseDown(iUIMouseEvent *MouseEvent,eMouseButton Button)noexcept(true)override;

	virtual void cnLib_FUNC WindowAttached(void)noexcept(true)override;
	virtual void cnLib_FUNC WindowDetached(void)noexcept(true)override;
	virtual bool cnLib_FUNC WindowMessage(LRESULT &MsgResult,const cWindowMessageParam &MsgParam)noexcept(true)override;
	virtual void cnLib_FUNC WindowMessageProcessed(LRESULT MsgResult,const cWindowMessageParam &MsgParam)noexcept(true)override;

};
//---------------------------------------------------------------------------
rPtr<iGDIFontHandle> vSetupTextControl(SIZE &TextSize,const wchar_t *Text,uIntn TextLength,const cUITextStyle &TextStyle)noexcept(true);
//---------------------------------------------------------------------------
class bvWinButton : public vWinDCPainter
{
public:
	bvWinButton(viButtonData *ButtonData)noexcept(true);
	~bvWinButton()noexcept(true);

	viButtonData* GetButtonData(void)const noexcept(true);
	void SetButtonData(viButtonData *Data)noexcept(true);
	
	void Update(void)noexcept(true);

protected:
	dPtr<viButtonData> fButtonData;


private:
	void ButtonDataInsertCallback(void)noexcept(true);
	void ButtonDataRemoveCallback(void)noexcept(true);

	iFunctionToken *fButtonDataNotifyToken;
};
//---------------------------------------------------------------------------
class bvWinThemeButton : public vWinDCThemePainter
{
public:
	bvWinThemeButton(viButtonData *ButtonData)noexcept(true);
	~bvWinThemeButton()noexcept(true);

	viButtonData* GetButtonData(void)const noexcept(true);
	void SetButtonData(viButtonData *Data)noexcept(true);
	
	void Update(void)noexcept(true);
protected:
	dPtr<viButtonData> fButtonData;


private:
	void ButtonDataInsertCallback(void)noexcept(true);
	void ButtonDataRemoveCallback(void)noexcept(true);
	iFunctionToken *fButtonDataNotifyToken;
};
//---------------------------------------------------------------------------
class cWinNCButtonPainter_Normal : public cWinNCButtonPainter
{
public:
	cWinNCButtonPainter_Normal()noexcept(true);
	virtual ~cWinNCButtonPainter_Normal()noexcept(true);

	virtual void PaintCloseButton(HDC DC,const RECT &Rect,eButtonState State)noexcept(true)override;
	virtual void PaintMinButton(HDC DC,const RECT &Rect,eButtonState State)noexcept(true)override;
	virtual void PaintMaxButton(HDC DC,const RECT &Rect,eButtonState State)noexcept(true)override;
	virtual void PaintRestoreButton(HDC DC,const RECT &Rect,eButtonState State)noexcept(true)override;
	virtual void PaintSmallCloseButton(HDC DC,const RECT &Rect,eButtonState State)noexcept(true)override;
	virtual void PaintHelpButton(HDC DC,const RECT &Rect,eButtonState State)noexcept(true)override;
private:
	UINT StateIDFromState(eButtonState State)noexcept(true);
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
uIntn WinNCGetButtonList(HWND WindowHandle,uIntn ListCount,eNCButtonType *ListBuffer)noexcept(true);
uIntn WinNCGetButtonList(iUIView *View,uIntn ListCount,eNCButtonType *ListBuffer)noexcept(true);
void WinNCCommand(iUIView *View,const cUIPoint *Pos,eNCButtonType ButtonType)noexcept(true);
//---------------------------------------------------------------------------
class vWinNCButton : public bvWinThemeButton
{
public:
	vWinNCButton(viButtonData *Data=nullptr)noexcept(true);
	~vWinNCButton()noexcept(true);

	static rPtr<viControl> Create(viButtonData *Data)noexcept(true);

	eNCButtonType ButtonType;
protected:
	cWinNCButtonPainter *fPainter=nullptr;

	virtual void ThemeSetup(HWND WindowHandle)noexcept(true)override;
	virtual void ThemeClear(void)noexcept(true)override;

	virtual void cnLib_FUNC Paint(HDC DC,HRGN ClipRegion)noexcept(true)override;
};
//---------------------------------------------------------------------------
class cWinNCButton : public bcButton, protected UIStateHandler
{
public:
	cWinNCButton()noexcept(true);
	~cWinNCButton()noexcept(true);

	void SetButtonType(eNCButtonType ButtonType)noexcept(true);
	
	eButtonState ButtonState(void)noexcept(true)override;

protected:

	virtual void ViewSetup(void)noexcept(true)override;
	virtual void ViewClear(void)noexcept(true)override;

	virtual void cnLib_FUNC UIResume(void)noexcept(true)override;
	virtual void cnLib_FUNC UIPaused(void)noexcept(true)override;


	vWinNCButton fNCButtonContent;
	virtual void ButtonBackgroundSetDefault(void)noexcept(true)override;
	virtual bool ButtonMouseAllowButton(eMouseButton Button)noexcept(true)override;
	virtual void ButtonClick(const cUIPoint &Pos)noexcept(true)override;
};
//---------------------------------------------------------------------------
class cWinPushButtonPainter_Normal : public cWinPushButtonPainter
{
public:
	cWinPushButtonPainter_Normal()noexcept(true);
	virtual ~cWinPushButtonPainter_Normal()noexcept(true);

	virtual cUIRectangle GetContentMarginRect(void)noexcept(true)override;
	virtual void PaintButton(HDC DC,const RECT &Rect,eButtonState State)noexcept(true)override;
private:
	void PaintText(HDC DC,const RECT &TextRect,const wchar_t *Text,uIntn TextLength,HFONT TextFont,eButtonState State)noexcept(true);
};
//---------------------------------------------------------------------------
class vWinPushButton : public ViewControl, public bvWinThemeButton
{
public:
	vWinPushButton(viButtonData *ButtonData=nullptr)noexcept(true);
	~vWinPushButton()noexcept(true);
	
	static rPtr<viControl> Create(viButtonData *Data)noexcept(true);

	void SetView(iUIView *View)noexcept(true);

protected:
	cWinPushButtonPainter *fPainter=nullptr;

	virtual void ThemeSetup(HWND WindowHandle)noexcept(true)override;
	virtual void ThemeClear(void)noexcept(true)override;

	virtual void cnLib_FUNC Paint(HDC DC,HRGN ClipRegion)noexcept(true)override;
	virtual cUIRectangle cnLib_FUNC UIMargin(const cUIRectangle &Margin)noexcept(true)override;
};
//---------------------------------------------------------------------------
class cWinTextButton : public cTextButton
{
protected:
	virtual void ButtonBackgroundSetDefault(void)noexcept(true)override;
};
//---------------------------------------------------------------------------
class cWinImageTextButton : public cImageTextButton
{
protected:
	virtual void ButtonBackgroundSetDefault(void)noexcept(true)override;
};
//---------------------------------------------------------------------------
class bvWinThemeStateButton : public bvWinThemeButton
{
public:
	bvWinThemeStateButton(viButtonData *ButtonData,viImageTextControlData *TextData)noexcept(true);
	~bvWinThemeStateButton()noexcept(true);

	viImageTextControlData* GetImageTextData(void)const noexcept(true);
	void SetImageTextData(viImageTextControlData *Data)noexcept(true);

protected:
	dPtr<viImageTextControlData> fTextData;
	cWinStateButtonPainter *fPainter=nullptr;
	rPtr<iGDIFontHandle> fFont;
	SIZE fTextSize;

	void SetupText(void)noexcept(true);

	void Update(void)noexcept(true);
	void UpdateState(void)noexcept(true);
	void SetupButtonRects(RECT &BoxRect,RECT &TextRect,const cUIPoint &BoxSize)noexcept(true);
	
private:
	void ImageTextDataInsertCallback(void)noexcept(true);
	void ImageTextDataRemoveCallback(void)noexcept(true);
	iFunctionToken *fImageTextNotifyToken;
};
//---------------------------------------------------------------------------
class cWinStateButton : public bcImageTextButton
{
public:
	cWinStateButton()noexcept(true);
	~cWinStateButton()noexcept(true);

	cString<uChar16> Text;
	ufInt8 Check=0;

	virtual ufInt8 CheckState(void)noexcept(true)override;
	virtual const uChar16* ControlTextString(void)noexcept(true)override;
	virtual uIntn ControlTextLength(void)noexcept(true)override;
	
	cFunction<void (void)noexcept(true)> OnClick;

protected:
	virtual void ButtonBackgroundSetDefault(void)noexcept(true)override;
	virtual bool ButtonMouseAllowButton(eMouseButton Button)noexcept(true)override;
	virtual void ButtonClick(const cUIPoint &Pos)noexcept(true)override;
};
//---------------------------------------------------------------------------
class cWinCheckButtonPainter_Normal : public cWinStateButtonPainter
{
public:
	cWinCheckButtonPainter_Normal()noexcept(true);
	virtual ~cWinCheckButtonPainter_Normal()noexcept(true);

	virtual cUIPoint GetBoxSize(HDC DC)noexcept(true)override;
	virtual void PaintButton(HDC DC,const RECT &Rect,eButtonState State,ufInt8 CheckState)noexcept(true)override;
	virtual void PaintText(HDC DC,const RECT &TextRect,const wchar_t *Text,uIntn TextLength,HFONT TextFont,eButtonState State,uInt8 CheckState)noexcept(true)override;
private:
};
//---------------------------------------------------------------------------
class vWinCheckBox : public bvWinThemeStateButton
{
public:
	vWinCheckBox(viButtonData *ButtonData=nullptr,viImageTextControlData *TextData=nullptr)noexcept(true);
	~vWinCheckBox()noexcept(true);
	
	static rPtr<viControl> Create(viButtonData *ButtonData,viImageTextControlData *TextData)noexcept(true);

protected:
	virtual void ThemeSetup(HWND WindowHandle)noexcept(true)override;
	virtual void ThemeClear(void)noexcept(true)override;

	virtual void cnLib_FUNC Paint(HDC DC,HRGN ClipRegion)noexcept(true)override;
};
//---------------------------------------------------------------------------
class cWinCheckButton : public cWinStateButton
{
public:
	cWinCheckButton()noexcept(true);
protected:
	virtual void ControlContentSetDefault(void)noexcept(true)override;
};
//---------------------------------------------------------------------------
class cWinRadioButtonPainter_Normal : public cWinStateButtonPainter
{
public:
	cWinRadioButtonPainter_Normal()noexcept(true);
	virtual ~cWinRadioButtonPainter_Normal()noexcept(true);

	virtual cUIPoint GetBoxSize(HDC DC)noexcept(true)override;
	virtual void PaintButton(HDC DC,const RECT &Rect,eButtonState State,ufInt8 CheckState)noexcept(true)override;
	virtual void PaintText(HDC DC,const RECT &TextRect,const wchar_t *Text,uIntn TextLength,HFONT TextFont,eButtonState State,uInt8 CheckState)noexcept(true)override;
private:
};
//---------------------------------------------------------------------------
class vWinRadioButton : public bvWinThemeStateButton
{
public:
	vWinRadioButton(viButtonData *ButtonData=nullptr,viImageTextControlData *TextData=nullptr)noexcept(true);
	~vWinRadioButton()noexcept(true);
	
	static rPtr<viControl> Create(viButtonData *ButtonData,viImageTextControlData *TextData)noexcept(true);
protected:

	virtual void ThemeSetup(HWND WindowHandle)noexcept(true)override;
	virtual void ThemeClear(void)noexcept(true)override;

	virtual void cnLib_FUNC Paint(HDC DC,HRGN ClipRegion)noexcept(true)override;
};
//---------------------------------------------------------------------------
class cWinRadioButton : public cWinStateButton
{
public:
	cWinRadioButton()noexcept(true);
protected:
	virtual void ControlContentSetDefault(void)noexcept(true)override;
};
//---------------------------------------------------------------------------
class cWinSpinButtonPainter_Normal : public cWinSpinButtonPainter
{
public:
	cWinSpinButtonPainter_Normal()noexcept(true);
	virtual ~cWinSpinButtonPainter_Normal()noexcept(true);

	virtual void PaintUpButton(HDC DC,const RECT &Rect,eButtonState State)noexcept(true)override;
	virtual void PaintDownButton(HDC DC,const RECT &Rect,eButtonState State)noexcept(true)override;
	virtual void PaintLeftButton(HDC DC,const RECT &Rect,eButtonState State)noexcept(true)override;
	virtual void PaintRightButton(HDC DC,const RECT &Rect,eButtonState State)noexcept(true)override;
private:
	static UINT ButtonStateFlag(eButtonState State)noexcept(true);
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
	vWinDirectionButton(viButtonData *ButtonData=nullptr)noexcept(true);
	~vWinDirectionButton()noexcept(true);
	
	eDirection PointerDirection;
	void Update(void)noexcept(true);

	static rPtr<viControl> Create(viButtonData *Data,eDirection PointerDirection)noexcept(true);

protected:
	cWinSpinButtonPainter *fPainter=nullptr;

	virtual void ThemeSetup(HWND WindowHandle)noexcept(true)override;
	virtual void ThemeClear(void)noexcept(true)override;

	virtual void cnLib_FUNC Paint(HDC DC,HRGN ClipRegion)noexcept(true)override;
};
//---------------------------------------------------------------------------
class cWinEditBackgroundPainter_Normal : public cWinEditBackgroundPainter
{
public:
	cWinEditBackgroundPainter_Normal()noexcept(true);
	virtual ~cWinEditBackgroundPainter_Normal()noexcept(true);

	virtual cUIRectangle GetContentMarginRect(void)noexcept(true)override;
	virtual void Paint(HDC DC,const RECT &Rect,ControlState State)noexcept(true)override;
private:
};
//---------------------------------------------------------------------------
class vWinEditBackground : public ViewControl, public vWinDCThemePainter
{
public:
	vWinEditBackground(viControlStateData *Data=nullptr)noexcept(true);
	~vWinEditBackground()noexcept(true);

	void SetView(iUIView *View)noexcept(true);

	static rPtr<viControl> Create(viControlStateData *Data)noexcept(true);

	viControlStateData* GetData(void)const noexcept(true);
	void SetData(viControlStateData *Data)noexcept(true);

protected:
	dPtr<viControlStateData> fData;
	cWinEditBackgroundPainter *fPainter=nullptr;

	virtual cUIRectangle cnLib_FUNC UIMargin(const cUIRectangle &Margin)noexcept(true)override;

	HTHEME fTheme;
	virtual void ThemeSetup(HWND WindowHandle)noexcept(true)override;
	virtual void ThemeClear(void)noexcept(true)override;

	virtual void cnLib_FUNC Paint(HDC DC,HRGN ClipRegion)noexcept(true)override;
	
	void Update(void)noexcept(true);
private:

	void DataInsertCallback(void)noexcept(true);
	void DataRemoveCallback(void)noexcept(true);
	iFunctionToken *fControlStateDataNotifyToken;
};
//---------------------------------------------------------------------------
class lWinEditFrame : public lFrame
{
protected:
	virtual void ControlBackgroundSetDefault(void)noexcept(true)override;
};
//---------------------------------------------------------------------------
class vWinTextCaret : protected UIViewHandler, protected KeyHandler
{
public:
	vWinTextCaret(viTextCaretData *Data=nullptr)noexcept(true);
	~vWinTextCaret()noexcept(true);

	static rPtr<viControl> Create(viTextCaretData *Data)noexcept(true);

	void SetView(iUIView *View)noexcept(true);
	void SetContentZPosition(Float32 ZPosition)noexcept(true);
	void SetContentVisible(bool Visible)noexcept(true);


protected:
	dPtr<viTextCaretData> fData;
	iPtr<iUIView> fView;

	bool fFocused;
	bool fVisible;

	void StateFunction(void)noexcept(true);

	bool fCaretCreated;
	bool fCaretVisible;
	int fCaretWidth;
	int fCaretHeight;
	void CaretSetup(HWND WindowHandle,cUIPoint CaretSize)noexcept(true);
	void CaretClear(void)noexcept(true);

	virtual void cnLib_FUNC UILayout(void)noexcept(true)override;
	virtual void cnLib_FUNC KeyFocusEnter(iUIKeyEvent *KeyEvent)noexcept(true)override;
	virtual void cnLib_FUNC KeyFocusLeave(iUIKeyEvent *KeyEvent)noexcept(true)override;
private:

	void DataInsertCallback(void)noexcept(true);
	void DataRemoveCallback(void)noexcept(true);
	iFunctionToken *fTextCaretNotifyToken;
};
//---------------------------------------------------------------------------
class cWinTabPainter_Normal : public cWinTabPainter
{
public:
	cWinTabPainter_Normal()noexcept(true);
	virtual ~cWinTabPainter_Normal()noexcept(true);

	virtual ufInt16 TabBorderSize(void)noexcept(true)override;
	virtual void PaintPanelHeader(HDC DC,const RECT &PanelRect)noexcept(true)override;

	virtual RECT PaintItemLeft(HDC DC,const RECT &Rect,TabState State)noexcept(true)override;
	virtual RECT PaintItemMiddle(HDC DC,const RECT &Rect,TabState State)noexcept(true)override;
	virtual RECT PaintItemRight(HDC DC,const RECT &Rect,TabState State)noexcept(true)override;
	virtual RECT PaintItemSingle(HDC DC,const RECT &Rect,TabState State)noexcept(true)override;
	
	virtual void PaintItemText(HDC DC,const RECT &TextRect,const wchar_t *Text,uIntn TextLength,HFONT TextFont,TabState State)noexcept(true)override;

private:
	RECT PaintTabItem(HDC DC,const RECT &Rect,TabState State)noexcept(true);

	void TabToTextRect(RECT &Rect,TabState State)noexcept(true);

	cnRTL::arSharedObjectRecycler< cSolidPenObject<1,0xCCCCCC> > fPanelHeaderPenRecycler;
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
class vWinTextTab : public vWinDCThemePainter
{
public:
	vWinTextTab(iVisualData<cTextTabData> *Data=nullptr)noexcept(true);
	~vWinTextTab()noexcept(true);

	static rPtr<viControl> Create(iVisualData<cTextTabData> *Data)noexcept(true);

	iVisualData<cTextTabData>* GetData(void)const noexcept(true);
	void SetData(iVisualData<cTextTabData> *Data)noexcept(true);

	sfInt16 TabHitTest(Float32 x,Float32 y)noexcept(true);

protected:
	rPtr< iVisualData<cTextTabData> > fData;
	cWinTabPainter *fPainter=nullptr;

	virtual void ThemeSetup(HWND WindowHandle)noexcept(true)override;
	virtual void ThemeClear(void)noexcept(true)override;

	virtual void cnLib_FUNC Paint(HDC DC,HRGN ClipRegion)noexcept(true)override;

private:
	void DataInsertCallback(void)noexcept(true);
	void DataRemoveCallback(void)noexcept(true);
	
	class cDataNotification : public iProcedure
	{
		virtual void cnLib_FUNC Execute(void)noexcept(true)override;
	}fDataNotification;
	void DataUpdate(void)noexcept(true);

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
	void SetupTabCache(HDC DC)noexcept(true);
	typedef cWinTabPainter::TabState TabState;
	void DrawTabItem(HDC DC,const cTabCacheItem &Item,TabState State)noexcept(true);
};
//---------------------------------------------------------------------------
class cWinTextTab : public cTextTab
{
protected:
	virtual void ControlContentSetDefault(void)noexcept(true)override;
};
//---------------------------------------------------------------------------
class cWinScrollBarPainter_Normal : public cWinScrollBarPainter
{
public:
	cWinScrollBarPainter_Normal()noexcept(true);
	virtual ~cWinScrollBarPainter_Normal()noexcept(true);

	static UINT BtnStateFlag(eButtonState State,bool IsActive)noexcept(true);

	virtual Float32 GetMinTrackButtonSize(void)noexcept(true)override;
	virtual void PaintDec(const PaintParam &Param)noexcept(true)override;
	virtual void PaintInc(const PaintParam &Param)noexcept(true)override;
	virtual void PaintDecBar(const PaintParam &Param)noexcept(true)override;
	virtual void PaintIncBar(const PaintParam &Param)noexcept(true)override;
	virtual void PaintTrack(const PaintParam &Param)noexcept(true)override;

private:
	class cBrushBMP
	{
	public:
		cBrushBMP()noexcept(true);
		~cBrushBMP()noexcept(true);

		HBITMAP BrushPattern;
		HBRUSH Brush;
	protected:
		void CreateBmp32(HDC DC,int w,int h,const void *PixelData)noexcept(true);
		void Destroy(void)noexcept(true);

		void Setup(HDC DC,bool Pressed)noexcept(true);
	};

	void PaintBar(const PaintParam &Param,bool Active)noexcept(true);
	class cBrushBMPNormal : public cBrushBMP
	{
	public:
		void Setup(HDC DC)noexcept(true);
		static const uInt32 PixelData[];
	};
	cnRTL::arSharedObjectRecycler<cBrushBMPNormal> fNormalBMPRecycler;

	class cBrushBMPPressed : public cBrushBMP
	{
	public:
		void Setup(HDC DC)noexcept(true);
		static const uInt32 PixelData[];
	};
	cnRTL::arSharedObjectRecycler<cBrushBMPPressed> fPressedBMPRecycler;
};
//---------------------------------------------------------------------------
class vWinScrollBar : public vWinDCThemePainter
{
public:
	vWinScrollBar(viScrollBarData *Data)noexcept(true);
	~vWinScrollBar()noexcept(true);

	static rPtr<viScrollBar> Create(viScrollBarData *Data)noexcept(true);

	viScrollBarData* GetData(void)const noexcept(true);
	void SetData(viScrollBarData *Data)noexcept(true);

	eScrollBarButton ScrollHitTest(Float32 x,Float32 y)noexcept(true);
	Float32 ScrollOffsetToValue(Float32 OffsetX,Float32 OffsetY)noexcept(true);

protected:
	dPtr<viScrollBarData> fData;
	cWinScrollBarPainter *fPainter=nullptr;

	virtual void ThemeSetup(HWND WindowHandle)noexcept(true)override;
	virtual void ThemeClear(void)noexcept(true)override;

	virtual void cnLib_FUNC Paint(HDC DC,HRGN ClipRegion)noexcept(true)override;
	void Update(void)noexcept(true);

private:

	sfInt16 fBtnSize=24;
	bool fIsVertical;

	Float32 fTrackBtnPos;
	Float32 fTrackBtnSize;
	Float32 fTrackSize;
	Float32 fValueSize;
	void CalculateParts(Float32 TotalSize)noexcept(true);

	void DataInsertCallback(void)noexcept(true);
	void DataRemoveCallback(void)noexcept(true);

	iFunctionToken *fScrollBarNotifyToken;
};
//---------------------------------------------------------------------------
class cWinScrollBar : public cScrollBar
{
protected:
	virtual void ControlContentSetDefault(void)noexcept(true)override;
};
//---------------------------------------------------------------------------
class cWinControlScrollBar : public cControlScrollBar
{
protected:
	virtual void ControlContentSetDefault(void)noexcept(true)override;
};
//---------------------------------------------------------------------------
class cWinScrollBarGripPainter_Normal : public cWinScrollBarGripPainter
{
public:
	cWinScrollBarGripPainter_Normal()noexcept(true);
	virtual ~cWinScrollBarGripPainter_Normal()noexcept(true);

	virtual void Paint(HDC DC,const RECT &Rect,eAlignment BoxAlign)noexcept(true)override;
};
//---------------------------------------------------------------------------
class vWinScrollBarGrip : public vWinDCThemePainter
{
public:
	vWinScrollBarGrip(viScrollBarGripData *Data=nullptr)noexcept(true);
	~vWinScrollBarGrip()noexcept(true);

	void Update(void)noexcept(true);

	static rPtr<viControl> Create(viScrollBarGripData *Data)noexcept(true);

	viScrollBarGripData* GetData(void)const noexcept(true);
	void SetData(viScrollBarGripData *Data)noexcept(true);

protected:
	cWinScrollBarGripPainter *fPainter=nullptr;
	dPtr<viScrollBarGripData> fData;

	virtual void ThemeSetup(HWND WindowHandle)noexcept(true)override;
	virtual void ThemeClear(void)noexcept(true)override;

	virtual void cnLib_FUNC Paint(HDC DC,HRGN ClipRegion)noexcept(true)override;

	void DataInsertCallback(void)noexcept(true);
	void DataRemoveCallback(void)noexcept(true);

	iFunctionToken *fScrollBarGripNotifyToken;
};
//---------------------------------------------------------------------------
class kWinControlScrollBar : public kControlScrollBar
{
public:
	kWinControlScrollBar()noexcept(true);
	~kWinControlScrollBar()noexcept(true);

	static rPtr<kiScrollBar> Create(void)noexcept(true);
};
//---------------------------------------------------------------------------
class vWinTreeBranchGraph : public vWinDCPainter
{
public:
	vWinTreeBranchGraph(viTreeBranchData *BranchData=nullptr)noexcept(true);
	~vWinTreeBranchGraph()noexcept(true);

	static rPtr<viControl> Create(viTreeBranchData *Data)noexcept(true);

	viTreeBranchData* GetBranchData(void)const noexcept(true);
	void SetBranchData(viTreeBranchData *Data)noexcept(true);

protected:
	dPtr<viTreeBranchData> fBranchData;
	virtual void cnLib_FUNC Paint(HDC DC,HRGN ClipRegion)noexcept(true)override;
	void Update(void)noexcept(true);

	void DrawBranch(HDC DC,const cUIPoint &Offset,const viTreeBranchData::cTreeBranchItem &Branch)noexcept(true);
private:

	struct cTreeLinePen
	{
		cTreeLinePen()noexcept(true);
		~cTreeLinePen()noexcept(true);

		HPEN Pen;
	};
	cnRTL::arSharedObjectRecycler<cTreeLinePen> fTreeLinePenRecycler;

	void BranchDataInsertCallback(void)noexcept(true);
	void BranchDataRemoveCallback(void)noexcept(true);

	iFunctionToken *fTreeGraphNotifyToken;
};
//---------------------------------------------------------------------------
class vWinTreeExpandButton : public bvWinButton
{
public:
	vWinTreeExpandButton(viButtonData *Data)noexcept(true);
	~vWinTreeExpandButton()noexcept(true);
	
	static rPtr<viControl> Create(viButtonData *Data)noexcept(true);

protected:
	virtual void cnLib_FUNC Paint(HDC DC,HRGN ClipRegion)noexcept(true)override;
	void Update(void)noexcept(true);
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
