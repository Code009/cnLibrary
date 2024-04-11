/*- cnWinUI - Themed Control Content --------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2009-03-22                                          */
/*-------------------------------------------------------------------------*/
#pragma once
/*-------------------------------------------------------------------------*/
#include <cnRTL\WinSDKHeader.h>
#include <Uxtheme.h>

#include <cnUI\BasicControl.h>
#include <cnUI\WinUIGDI.h>
#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
namespace cnUI{
//---------------------------------------------------------------------------
class vWinDCThemePainter : public vWinDCPainter, protected iWindowMessageHandler
{
public:
	vWinDCThemePainter()noexcept(true);
	~vWinDCThemePainter()noexcept(true);

protected:
	iPtr<iWindow> fWindow;

	virtual void ThemeSetup(HWND WindowHandle)noexcept(true){WindowHandle;}
	virtual void ThemeClear(void)noexcept(true){}

	virtual void cnLib_FUNC PaintStarted(void)noexcept(true)override;
	virtual void cnLib_FUNC PaintStopped(void)noexcept(true)override;
	virtual void cnLib_FUNC PaintWindowChanged(void)noexcept(true)override;

	virtual void cnLib_FUNC WindowAttached(void)noexcept(true)override;
	virtual void cnLib_FUNC WindowDetached(void)noexcept(true)override;
	virtual bool cnLib_FUNC WindowMessage(LRESULT &MsgResult,const cWindowMessageParam &Message)noexcept(true)override;
	virtual void cnLib_FUNC WindowMessageProcessed(LRESULT MsgResult,const cWindowMessageParam &Message)noexcept(true)override;

private:
	void SetupPaintWindow(void)noexcept(true);
	void ClearPaintWindow(void)noexcept(true);
};
//---------------------------------------------------------------------------
class cWinNCFramePainter
{
public:
	cWinNCFramePainter()noexcept(true);
	virtual ~cWinNCFramePainter()noexcept(true);

	virtual void PaintFrame(HDC DC,const RECT &Rect,const RECT &Border,Float32 CaptionHeight,bool Active)noexcept(true)=0;
	virtual void PaintSmallFrame(HDC DC,const RECT &Rect,const RECT &Border,Float32 CaptionHeight,bool Active)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cWinNCFramePainter_Theme : public cWinNCFramePainter
{
public:
	cWinNCFramePainter_Theme(HTHEME Theme)noexcept(true);
	virtual ~cWinNCFramePainter_Theme()noexcept(true);

	static HTHEME OpenTheme(HWND WindowHandle)noexcept(true);

	virtual void PaintFrame(HDC DC,const RECT &Rect,const RECT &Border,Float32 CaptionHeight,bool Active)noexcept(true)override;
	virtual void PaintSmallFrame(HDC DC,const RECT &Rect,const RECT &Border,Float32 CaptionHeight,bool Active)noexcept(true)override;
private:
	HTHEME fTheme;
};
//---------------------------------------------------------------------------
class cWinNCButtonPainter
{
public:
	cWinNCButtonPainter()noexcept(true);
	virtual ~cWinNCButtonPainter()noexcept(true);

	virtual void PaintCloseButton(HDC DC,const RECT &Rect,eButtonState State)noexcept(true)=0;
	virtual void PaintMinButton(HDC DC,const RECT &Rect,eButtonState State)noexcept(true)=0;
	virtual void PaintMaxButton(HDC DC,const RECT &Rect,eButtonState State)noexcept(true)=0;
	virtual void PaintRestoreButton(HDC DC,const RECT &Rect,eButtonState State)noexcept(true)=0;
	virtual void PaintSmallCloseButton(HDC DC,const RECT &Rect,eButtonState State)noexcept(true)=0;
	virtual void PaintHelpButton(HDC DC,const RECT &Rect,eButtonState State)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cWinNCButtonPainter_Theme : public cWinNCButtonPainter
{
public:
	cWinNCButtonPainter_Theme(HTHEME Theme)noexcept(true);
	virtual ~cWinNCButtonPainter_Theme()noexcept(true);

	static HTHEME OpenTheme(HWND WindowHandle)noexcept(true);

	virtual void PaintCloseButton(HDC DC,const RECT &Rect,eButtonState State)noexcept(true)override;
	virtual void PaintMinButton(HDC DC,const RECT &Rect,eButtonState State)noexcept(true)override;
	virtual void PaintMaxButton(HDC DC,const RECT &Rect,eButtonState State)noexcept(true)override;
	virtual void PaintRestoreButton(HDC DC,const RECT &Rect,eButtonState State)noexcept(true)override;
	virtual void PaintSmallCloseButton(HDC DC,const RECT &Rect,eButtonState State)noexcept(true)override;
	virtual void PaintHelpButton(HDC DC,const RECT &Rect,eButtonState State)noexcept(true)override;
private:
	HTHEME fTheme;
};
//---------------------------------------------------------------------------
class cWinGroupBoxPainter
{
public:
	cWinGroupBoxPainter()noexcept(true);
	virtual ~cWinGroupBoxPainter()noexcept(true);

	virtual cUIRectangle GetContentMarginRect(void)noexcept(true)=0;
	virtual void PaintBox(HDC DC,const RECT &Rect)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cWinGroupBoxPainter_Theme : public cWinGroupBoxPainter
{
public:
	cWinGroupBoxPainter_Theme(HTHEME Theme)noexcept(true);
	virtual ~cWinGroupBoxPainter_Theme()noexcept(true);

	static HTHEME OpenTheme(HWND WindowHandle)noexcept(true);

	virtual cUIRectangle GetContentMarginRect(void)noexcept(true)override;
	virtual void PaintBox(HDC DC,const RECT &Rect)noexcept(true)override;
private:
	HTHEME fTheme;
};
//---------------------------------------------------------------------------
class cWinPushButtonPainter
{
public:
	cWinPushButtonPainter()noexcept(true);
	virtual ~cWinPushButtonPainter()noexcept(true);

	virtual cUIRectangle GetContentMarginRect(void)noexcept(true)=0;
	virtual void PaintButton(HDC DC,const RECT &Rect,eButtonState State)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cWinPushButtonPainter_Theme : public cWinPushButtonPainter
{
public:
	cWinPushButtonPainter_Theme(HTHEME Theme)noexcept(true);
	virtual ~cWinPushButtonPainter_Theme()noexcept(true);

	static HTHEME OpenTheme(HWND WindowHandle)noexcept(true);

	virtual cUIRectangle GetContentMarginRect(void)noexcept(true)override;
	virtual void PaintButton(HDC DC,const RECT &Rect,eButtonState State)noexcept(true)override;
private:
	HTHEME fTheme;

	int StateIDFromState(eButtonState State)noexcept(true);
	void PaintText(HDC DC,const RECT &TextRect,const wchar_t *Text,uIntn TextLength,HFONT TextFont,eButtonState State)noexcept(true);
};
//---------------------------------------------------------------------------
class cWinStateButtonPainter
{
public:
	cWinStateButtonPainter()noexcept(true);
	virtual ~cWinStateButtonPainter()noexcept(true);

	virtual cUIPoint GetBoxSize(HDC DC)noexcept(true)=0;
	virtual void PaintButton(HDC DC,const RECT &Rect,eButtonState State,ufInt8 CheckState)noexcept(true)=0;
	virtual void PaintText(HDC DC,const RECT &TextRect,const wchar_t *Text,uIntn TextLength,HFONT TextFont,eButtonState State,uInt8 CheckState)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cWinCheckButtonPainter_Theme : public cWinStateButtonPainter
{
public:
	cWinCheckButtonPainter_Theme(HTHEME Theme)noexcept(true);
	virtual ~cWinCheckButtonPainter_Theme()noexcept(true);

	static HTHEME OpenTheme(HWND WindowHandle)noexcept(true);

	virtual cUIPoint GetBoxSize(HDC DC)noexcept(true)override;
	virtual void PaintButton(HDC DC,const RECT &Rect,eButtonState State,ufInt8 CheckState)noexcept(true)override;
	virtual void PaintText(HDC DC,const RECT &TextRect,const wchar_t *Text,uIntn TextLength,HFONT TextFont,eButtonState State,uInt8 CheckState)noexcept(true)override;
private:
	HTHEME fTheme;

	int StateIDFromState(eButtonState State,uInt8 CheckState)noexcept(true);
};
//---------------------------------------------------------------------------
class cWinRadioButtonPainter_Theme : public cWinStateButtonPainter
{
public:
	cWinRadioButtonPainter_Theme(HTHEME Theme)noexcept(true);
	virtual ~cWinRadioButtonPainter_Theme()noexcept(true);

	static HTHEME OpenTheme(HWND WindowHandle)noexcept(true);

	virtual cUIPoint GetBoxSize(HDC DC)noexcept(true)override;
	virtual void PaintButton(HDC DC,const RECT &Rect,eButtonState State,ufInt8 CheckState)noexcept(true)override;
	virtual void PaintText(HDC DC,const RECT &TextRect,const wchar_t *Text,uIntn TextLength,HFONT TextFont,eButtonState State,uInt8 CheckState)noexcept(true)override;
private:
	HTHEME fTheme;

	int StateIDFromState(eButtonState State,uInt8 CheckState)noexcept(true);
};
//---------------------------------------------------------------------------
class cWinSpinButtonPainter
{
public:
	cWinSpinButtonPainter()noexcept(true);
	virtual ~cWinSpinButtonPainter()noexcept(true);

	virtual void PaintUpButton(HDC DC,const RECT &Rect,eButtonState State)noexcept(true)=0;
	virtual void PaintDownButton(HDC DC,const RECT &Rect,eButtonState State)noexcept(true)=0;
	virtual void PaintLeftButton(HDC DC,const RECT &Rect,eButtonState State)noexcept(true)=0;
	virtual void PaintRightButton(HDC DC,const RECT &Rect,eButtonState State)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cWinSpinButtonPainter_Theme : public cWinSpinButtonPainter
{
public:
	cWinSpinButtonPainter_Theme(HTHEME Theme)noexcept(true);
	virtual ~cWinSpinButtonPainter_Theme()noexcept(true);

	static HTHEME OpenTheme(HWND WindowHandle)noexcept(true);

	virtual void PaintUpButton(HDC DC,const RECT &Rect,eButtonState State)noexcept(true)override;
	virtual void PaintDownButton(HDC DC,const RECT &Rect,eButtonState State)noexcept(true)override;
	virtual void PaintLeftButton(HDC DC,const RECT &Rect,eButtonState State)noexcept(true)override;
	virtual void PaintRightButton(HDC DC,const RECT &Rect,eButtonState State)noexcept(true)override;
private:
	HTHEME fTheme;

	int StateIDFromState(eButtonState State)noexcept(true);
};
//---------------------------------------------------------------------------
class cWinEditBackgroundPainter
{
public:
	cWinEditBackgroundPainter()noexcept(true);
	virtual ~cWinEditBackgroundPainter()noexcept(true);


	virtual cUIRectangle GetContentMarginRect(void)noexcept(true)=0;
	enum class ControlState : uInt8{
		Normal,
		Hot,
		Focused,
		Disabled,
	};
	virtual void Paint(HDC DC,const RECT &Rect,ControlState State)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cWinEditBackgroundPainter_Theme : public cWinEditBackgroundPainter
{
public:
	cWinEditBackgroundPainter_Theme(HTHEME Theme)noexcept(true);
	virtual ~cWinEditBackgroundPainter_Theme()noexcept(true);

	static HTHEME OpenTheme(HWND WindowHandle)noexcept(true);

	virtual cUIRectangle GetContentMarginRect(void)noexcept(true)override;
	virtual void Paint(HDC DC,const RECT &Rect,ControlState State)noexcept(true)override;
private:
	HTHEME fTheme;
};
//---------------------------------------------------------------------------
class cWinTabPainter
{
public:
	cWinTabPainter()noexcept(true);
	virtual ~cWinTabPainter()noexcept(true);

	virtual ufInt16n TabBorderSize(void)noexcept(true)=0;
	virtual void PaintPanelHeader(HDC DC,const RECT &PanelRect)noexcept(true)=0;

	enum class TabState : uInt8{
		Normal,
		Active,
		Hot
	};

	virtual RECT PaintItemLeft(HDC DC,const RECT &Rect,TabState State)noexcept(true)=0;
	virtual RECT PaintItemMiddle(HDC DC,const RECT &Rect,TabState State)noexcept(true)=0;
	virtual RECT PaintItemRight(HDC DC,const RECT &Rect,TabState State)noexcept(true)=0;
	virtual RECT PaintItemSingle(HDC DC,const RECT &Rect,TabState State)noexcept(true)=0;

	virtual void PaintItemText(HDC DC,const RECT &TextRect,const wchar_t *Text,uIntn TextLength,HFONT TextFont,TabState State)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cWinTabPainter_Theme : public cWinTabPainter
{
public:
	cWinTabPainter_Theme(HTHEME Theme)noexcept(true);
	virtual ~cWinTabPainter_Theme()noexcept(true);

	static HTHEME OpenTheme(HWND WindowHandle)noexcept(true);

	virtual ufInt16n TabBorderSize(void)noexcept(true)override;
	virtual void PaintPanelHeader(HDC DC,const RECT &PanelRect)noexcept(true)override;

	virtual RECT PaintItemLeft(HDC DC,const RECT &Rect,TabState State)noexcept(true)override;
	virtual RECT PaintItemMiddle(HDC DC,const RECT &Rect,TabState State)noexcept(true)override;
	virtual RECT PaintItemRight(HDC DC,const RECT &Rect,TabState State)noexcept(true)override;
	virtual RECT PaintItemSingle(HDC DC,const RECT &Rect,TabState State)noexcept(true)override;

	virtual void PaintItemText(HDC DC,const RECT &TextRect,const wchar_t *Text,uIntn TextLength,HFONT TextFont,TabState State)noexcept(true)override;
private:
	HTHEME fTheme;

	RECT PaintTabItem(HDC DC,const RECT &Rect,TabState State,uInt8 PartID)noexcept(true);

	void TabToTextRect(RECT &Rect,TabState State)noexcept(true);

	cnRTL::arSharedObjectRecycler< cSolidPenObject<1,0xD9D9D9> > fPanelHeaderPenRecycler;


};
//---------------------------------------------------------------------------
class cWinScrollBarPainter
{
public:
	cWinScrollBarPainter()noexcept(true);
	virtual ~cWinScrollBarPainter()noexcept(true);

	virtual Float32 GetMinTrackButtonSize(void)noexcept(true)=0;
	struct PaintParam
	{
		HDC DC;
		RECT Rect;
		eScrollBarButton ActiveButton;
		eButtonState State;
		bool IsVertical;
	};
	virtual void PaintDec(const PaintParam &Param)noexcept(true)=0;
	virtual void PaintInc(const PaintParam &Param)noexcept(true)=0;
	virtual void PaintDecBar(const PaintParam &Param)noexcept(true)=0;
	virtual void PaintIncBar(const PaintParam &Param)noexcept(true)=0;
	virtual void PaintTrack(const PaintParam &Param)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cWinScrollBarPainter_Theme : public cWinScrollBarPainter
{
public:
	cWinScrollBarPainter_Theme(HTHEME Theme)noexcept(true);
	virtual ~cWinScrollBarPainter_Theme()noexcept(true);

	static HTHEME OpenTheme(HWND WindowHandle)noexcept(true);

	enum{
		siNormal,
		siHot,
		siPressed,
		siDisabled,
		siHighlight
	};
	static uIntn BtnStateIndex(eButtonState State,bool IsActive)noexcept(true);
	static int BtnStateFlag(eButtonState State,bool IsActive)noexcept(true);

	virtual Float32 GetMinTrackButtonSize(void)noexcept(true)override;
	virtual void PaintDec(const PaintParam &Param)noexcept(true)override;
	virtual void PaintInc(const PaintParam &Param)noexcept(true)override;
	virtual void PaintDecBar(const PaintParam &Param)noexcept(true)override;
	virtual void PaintIncBar(const PaintParam &Param)noexcept(true)override;
	virtual void PaintTrack(const PaintParam &Param)noexcept(true)override;

private:
	HTHEME fTheme;
	
};
//---------------------------------------------------------------------------
class cWinScrollBarGripPainter
{
public:
	cWinScrollBarGripPainter()noexcept(true);
	virtual ~cWinScrollBarGripPainter()noexcept(true);

	virtual void Paint(HDC DC,const RECT &Rect,eAlignment BoxAlign)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cWinScrollBarGripPainter_Theme : public cWinScrollBarGripPainter
{
public:
	cWinScrollBarGripPainter_Theme(HTHEME Theme)noexcept(true);
	virtual ~cWinScrollBarGripPainter_Theme()noexcept(true);

	static HTHEME OpenTheme(HWND WindowHandle)noexcept(true);

	virtual void Paint(HDC DC,const RECT &Rect,eAlignment BoxAlign)noexcept(true)override;
private:
	HTHEME fTheme;
};
//---------------------------------------------------------------------------
}	// namespace UI
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
