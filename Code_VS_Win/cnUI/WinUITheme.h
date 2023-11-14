/*- cnWinUI - Themed Control Content --------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2009-03-22                                          */
/*-------------------------------------------------------------------------*/
#pragma once
/*-------------------------------------------------------------------------*/
#include <cnRTL\WinSDKHeader.h>
#include <Uxtheme.h>

#include <cnUI\WinUIGDI.h>
#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
namespace cnUI{
//---------------------------------------------------------------------------
class vWinDCThemePainter : public vWinDCPainter, protected iWindowMessageHandler
{
public:
	vWinDCThemePainter();
	~vWinDCThemePainter();

protected:
	iPtr<iWindow> fWindow;

	virtual void ThemeSetup(HWND WindowHandle){WindowHandle;}
	virtual void ThemeClear(void){}

	virtual void cnLib_FUNC PaintStarted(void)override;
	virtual void cnLib_FUNC PaintStopped(void)override;
	virtual void cnLib_FUNC PaintWindowChanged(void)override;

	virtual void cnLib_FUNC WindowAttached(void)override;
	virtual void cnLib_FUNC WindowDetached(void)override;
	virtual bool cnLib_FUNC WindowMessage(LRESULT &MsgResult,const cWindowMessageParam &Message)override;
	virtual void cnLib_FUNC WindowMessageProcessed(LRESULT MsgResult,const cWindowMessageParam &Message)override;

private:
	void SetupPaintWindow(void);
	void ClearPaintWindow(void);
};
//---------------------------------------------------------------------------
class cWinNCFramePainter
{
public:
	cWinNCFramePainter();
	virtual ~cWinNCFramePainter();

	virtual void PaintFrame(HDC DC,const RECT &Rect,const RECT &Border,Float32 CaptionHeight,bool Active)=0;
	virtual void PaintSmallFrame(HDC DC,const RECT &Rect,const RECT &Border,Float32 CaptionHeight,bool Active)=0;
};
//---------------------------------------------------------------------------
class cWinNCFramePainter_Theme : public cWinNCFramePainter
{
public:
	cWinNCFramePainter_Theme(HTHEME Theme);
	virtual ~cWinNCFramePainter_Theme();

	static HTHEME OpenTheme(HWND WindowHandle);

	virtual void PaintFrame(HDC DC,const RECT &Rect,const RECT &Border,Float32 CaptionHeight,bool Active)override;
	virtual void PaintSmallFrame(HDC DC,const RECT &Rect,const RECT &Border,Float32 CaptionHeight,bool Active)override;
private:
	HTHEME fTheme;
};
//---------------------------------------------------------------------------
class cWinNCButtonPainter
{
public:
	cWinNCButtonPainter();
	virtual ~cWinNCButtonPainter();

	virtual void PaintCloseButton(HDC DC,const RECT &Rect,eButtonState State)=0;
	virtual void PaintMinButton(HDC DC,const RECT &Rect,eButtonState State)=0;
	virtual void PaintMaxButton(HDC DC,const RECT &Rect,eButtonState State)=0;
	virtual void PaintRestoreButton(HDC DC,const RECT &Rect,eButtonState State)=0;
	virtual void PaintSmallCloseButton(HDC DC,const RECT &Rect,eButtonState State)=0;
	virtual void PaintHelpButton(HDC DC,const RECT &Rect,eButtonState State)=0;
};
//---------------------------------------------------------------------------
class cWinNCButtonPainter_Theme : public cWinNCButtonPainter
{
public:
	cWinNCButtonPainter_Theme(HTHEME Theme);
	virtual ~cWinNCButtonPainter_Theme();

	static HTHEME OpenTheme(HWND WindowHandle);

	virtual void PaintCloseButton(HDC DC,const RECT &Rect,eButtonState State)override;
	virtual void PaintMinButton(HDC DC,const RECT &Rect,eButtonState State)override;
	virtual void PaintMaxButton(HDC DC,const RECT &Rect,eButtonState State)override;
	virtual void PaintRestoreButton(HDC DC,const RECT &Rect,eButtonState State)override;
	virtual void PaintSmallCloseButton(HDC DC,const RECT &Rect,eButtonState State)override;
	virtual void PaintHelpButton(HDC DC,const RECT &Rect,eButtonState State)override;
private:
	HTHEME fTheme;
};
//---------------------------------------------------------------------------
class cWinGroupBoxPainter
{
public:
	cWinGroupBoxPainter();
	virtual ~cWinGroupBoxPainter();

	virtual cUIRectangle GetContentMarginRect(void)=0;
	virtual void PaintBox(HDC DC,const RECT &Rect)=0;
};
//---------------------------------------------------------------------------
class cWinGroupBoxPainter_Theme : public cWinGroupBoxPainter
{
public:
	cWinGroupBoxPainter_Theme(HTHEME Theme);
	virtual ~cWinGroupBoxPainter_Theme();

	static HTHEME OpenTheme(HWND WindowHandle);

	virtual cUIRectangle GetContentMarginRect(void)override;
	virtual void PaintBox(HDC DC,const RECT &Rect)override;
private:
	HTHEME fTheme;
};
//---------------------------------------------------------------------------
class cWinPushButtonPainter
{
public:
	cWinPushButtonPainter();
	virtual ~cWinPushButtonPainter();

	virtual cUIRectangle GetContentMarginRect(void)=0;
	virtual void PaintButton(HDC DC,const RECT &Rect,eButtonState State)=0;
};
//---------------------------------------------------------------------------
class cWinPushButtonPainter_Theme : public cWinPushButtonPainter
{
public:
	cWinPushButtonPainter_Theme(HTHEME Theme);
	virtual ~cWinPushButtonPainter_Theme();

	static HTHEME OpenTheme(HWND WindowHandle);

	virtual cUIRectangle GetContentMarginRect(void)override;
	virtual void PaintButton(HDC DC,const RECT &Rect,eButtonState State)override;
private:
	HTHEME fTheme;

	int StateIDFromState(eButtonState State);
	void PaintText(HDC DC,const RECT &TextRect,const wchar_t *Text,uIntn TextLength,HFONT TextFont,eButtonState State);
};
//---------------------------------------------------------------------------
class cWinStateButtonPainter
{
public:
	cWinStateButtonPainter();
	virtual ~cWinStateButtonPainter();

	virtual cUIPoint GetBoxSize(HDC DC)=0;
	virtual void PaintButton(HDC DC,const RECT &Rect,eButtonState State,ufInt8 CheckState)=0;
	virtual void PaintText(HDC DC,const RECT &TextRect,const wchar_t *Text,uIntn TextLength,HFONT TextFont,eButtonState State,uInt8 CheckState)=0;
};
//---------------------------------------------------------------------------
class cWinCheckButtonPainter_Theme : public cWinStateButtonPainter
{
public:
	cWinCheckButtonPainter_Theme(HTHEME Theme);
	virtual ~cWinCheckButtonPainter_Theme();

	static HTHEME OpenTheme(HWND WindowHandle);

	virtual cUIPoint GetBoxSize(HDC DC)override;
	virtual void PaintButton(HDC DC,const RECT &Rect,eButtonState State,ufInt8 CheckState)override;
	virtual void PaintText(HDC DC,const RECT &TextRect,const wchar_t *Text,uIntn TextLength,HFONT TextFont,eButtonState State,uInt8 CheckState)override;
private:
	HTHEME fTheme;

	int StateIDFromState(eButtonState State,uInt8 CheckState);
};
//---------------------------------------------------------------------------
class cWinRadioButtonPainter_Theme : public cWinStateButtonPainter
{
public:
	cWinRadioButtonPainter_Theme(HTHEME Theme);
	virtual ~cWinRadioButtonPainter_Theme();

	static HTHEME OpenTheme(HWND WindowHandle);

	virtual cUIPoint GetBoxSize(HDC DC)override;
	virtual void PaintButton(HDC DC,const RECT &Rect,eButtonState State,ufInt8 CheckState)override;
	virtual void PaintText(HDC DC,const RECT &TextRect,const wchar_t *Text,uIntn TextLength,HFONT TextFont,eButtonState State,uInt8 CheckState)override;
private:
	HTHEME fTheme;

	int StateIDFromState(eButtonState State,uInt8 CheckState);
};
//---------------------------------------------------------------------------
class cWinSpinButtonPainter
{
public:
	cWinSpinButtonPainter();
	virtual ~cWinSpinButtonPainter();

	virtual void PaintUpButton(HDC DC,const RECT &Rect,eButtonState State)=0;
	virtual void PaintDownButton(HDC DC,const RECT &Rect,eButtonState State)=0;
	virtual void PaintLeftButton(HDC DC,const RECT &Rect,eButtonState State)=0;
	virtual void PaintRightButton(HDC DC,const RECT &Rect,eButtonState State)=0;
};
//---------------------------------------------------------------------------
class cWinSpinButtonPainter_Theme : public cWinSpinButtonPainter
{
public:
	cWinSpinButtonPainter_Theme(HTHEME Theme);
	virtual ~cWinSpinButtonPainter_Theme();

	static HTHEME OpenTheme(HWND WindowHandle);

	virtual void PaintUpButton(HDC DC,const RECT &Rect,eButtonState State)override;
	virtual void PaintDownButton(HDC DC,const RECT &Rect,eButtonState State)override;
	virtual void PaintLeftButton(HDC DC,const RECT &Rect,eButtonState State)override;
	virtual void PaintRightButton(HDC DC,const RECT &Rect,eButtonState State)override;
private:
	HTHEME fTheme;

	int StateIDFromState(eButtonState State);
};
//---------------------------------------------------------------------------
class cWinEditBackgroundPainter
{
public:
	cWinEditBackgroundPainter();
	virtual ~cWinEditBackgroundPainter();


	virtual cUIRectangle GetContentMarginRect(void)=0;
	enum class ControlState : uInt8{
		Normal,
		Hot,
		Focused,
		Disabled,
	};
	virtual void Paint(HDC DC,const RECT &Rect,ControlState State)=0;
};
//---------------------------------------------------------------------------
class cWinEditBackgroundPainter_Theme : public cWinEditBackgroundPainter
{
public:
	cWinEditBackgroundPainter_Theme(HTHEME Theme);
	virtual ~cWinEditBackgroundPainter_Theme();

	static HTHEME OpenTheme(HWND WindowHandle);

	virtual cUIRectangle GetContentMarginRect(void)override;
	virtual void Paint(HDC DC,const RECT &Rect,ControlState State)override;
private:
	HTHEME fTheme;
};
//---------------------------------------------------------------------------
class cWinTabPainter
{
public:
	cWinTabPainter();
	virtual ~cWinTabPainter();

	virtual ufInt16n TabBorderSize(void)=0;
	virtual void PaintPanelHeader(HDC DC,const RECT &PanelRect)=0;

	enum class TabState : uInt8{
		Normal,
		Active,
		Hot
	};

	virtual RECT PaintItemLeft(HDC DC,const RECT &Rect,TabState State)=0;
	virtual RECT PaintItemMiddle(HDC DC,const RECT &Rect,TabState State)=0;
	virtual RECT PaintItemRight(HDC DC,const RECT &Rect,TabState State)=0;
	virtual RECT PaintItemSingle(HDC DC,const RECT &Rect,TabState State)=0;

	virtual void PaintItemText(HDC DC,const RECT &TextRect,const wchar_t *Text,uIntn TextLength,HFONT TextFont,TabState State)=0;
};
//---------------------------------------------------------------------------
class cWinTabPainter_Theme : public cWinTabPainter
{
public:
	cWinTabPainter_Theme(HTHEME Theme);
	virtual ~cWinTabPainter_Theme();

	static HTHEME OpenTheme(HWND WindowHandle);

	virtual ufInt16n TabBorderSize(void)override;
	virtual void PaintPanelHeader(HDC DC,const RECT &PanelRect)override;

	virtual RECT PaintItemLeft(HDC DC,const RECT &Rect,TabState State)override;
	virtual RECT PaintItemMiddle(HDC DC,const RECT &Rect,TabState State)override;
	virtual RECT PaintItemRight(HDC DC,const RECT &Rect,TabState State)override;
	virtual RECT PaintItemSingle(HDC DC,const RECT &Rect,TabState State)override;

	virtual void PaintItemText(HDC DC,const RECT &TextRect,const wchar_t *Text,uIntn TextLength,HFONT TextFont,TabState State)override;
private:
	HTHEME fTheme;

	RECT PaintTabItem(HDC DC,const RECT &Rect,TabState State,uInt8 PartID);

	void TabToTextRect(RECT &Rect,TabState State);

	cnRTL::arSharedObjectRecycler< cSolidPenObject<1,0xD9D9D9> > fPanelHeaderPenRecycler;


};
//---------------------------------------------------------------------------
class cWinScrollBarPainter
{
public:
	cWinScrollBarPainter();
	virtual ~cWinScrollBarPainter();

	virtual Float32 GetMinTrackButtonSize(void)=0;
	struct PaintParam
	{
		HDC DC;
		RECT Rect;
		eScrollBarButton ActiveButton;
		eButtonState State;
		bool IsVertical;
	};
	virtual void PaintDec(const PaintParam &Param)=0;
	virtual void PaintInc(const PaintParam &Param)=0;
	virtual void PaintDecBar(const PaintParam &Param)=0;
	virtual void PaintIncBar(const PaintParam &Param)=0;
	virtual void PaintTrack(const PaintParam &Param)=0;
};
//---------------------------------------------------------------------------
class cWinScrollBarPainter_Theme : public cWinScrollBarPainter
{
public:
	cWinScrollBarPainter_Theme(HTHEME Theme);
	virtual ~cWinScrollBarPainter_Theme();

	static HTHEME OpenTheme(HWND WindowHandle);

	enum{
		siNormal,
		siHot,
		siPressed,
		siDisabled,
		siHighlight
	};
	static uIntn BtnStateIndex(eButtonState State,bool IsActive);
	static int BtnStateFlag(eButtonState State,bool IsActive);

	virtual Float32 GetMinTrackButtonSize(void)override;
	virtual void PaintDec(const PaintParam &Param)override;
	virtual void PaintInc(const PaintParam &Param)override;
	virtual void PaintDecBar(const PaintParam &Param)override;
	virtual void PaintIncBar(const PaintParam &Param)override;
	virtual void PaintTrack(const PaintParam &Param)override;

private:
	HTHEME fTheme;
	
};
//---------------------------------------------------------------------------
class cWinScrollBarGripPainter
{
public:
	cWinScrollBarGripPainter();
	virtual ~cWinScrollBarGripPainter();

	virtual void Paint(HDC DC,const RECT &Rect,eAlignment BoxAlign)=0;
};
//---------------------------------------------------------------------------
class cWinScrollBarGripPainter_Theme : public cWinScrollBarGripPainter
{
public:
	cWinScrollBarGripPainter_Theme(HTHEME Theme);
	virtual ~cWinScrollBarGripPainter_Theme();

	static HTHEME OpenTheme(HWND WindowHandle);

	virtual void Paint(HDC DC,const RECT &Rect,eAlignment BoxAlign)override;
private:
	HTHEME fTheme;
};
//---------------------------------------------------------------------------
}	// namespace UI
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
