#include <cnRTL\WinSDKHeader.h>
#include <vsstyle.h>
#include <Vssym32.h>

#include "WinUITheme.h"
#pragma comment(lib,"UxTheme.lib")

using namespace cnLibrary;
using namespace cnRTL;
using namespace cnUI;
//---------------------------------------------------------------------------
//--------------------------------------------------------------------------
vWinDCThemePainter::vWinDCThemePainter()noexcept(true)
{
}
//---------------------------------------------------------------------------
vWinDCThemePainter::~vWinDCThemePainter()noexcept(true)
{
}
//---------------------------------------------------------------------------
void vWinDCThemePainter::PaintStarted(void)noexcept(true)
{
	vWinDCPainter::PaintStarted();
	
	fWindow=fDCViewContent->GetWindow();
	SetupPaintWindow();
}
//---------------------------------------------------------------------------
void vWinDCThemePainter::PaintStopped(void)noexcept(true)
{
	if(fWindow!=nullptr){
		ClearPaintWindow();
		fWindow=nullptr;
	}
	
	vWinDCPainter::PaintStopped();
}
//---------------------------------------------------------------------------
void vWinDCThemePainter::PaintWindowChanged(void)noexcept(true)
{
	if(fDCViewContent->GetPaintState()==UIState::Null){
		fWindow=fDCViewContent->GetWindow();
		return;
	}
	ClearPaintWindow();
	fWindow=fDCViewContent->GetWindow();
	SetupPaintWindow();
}
//---------------------------------------------------------------------------
void vWinDCThemePainter::SetupPaintWindow(void)noexcept(true)
{
	if(fWindow!=nullptr){
		fWindow->InsertMessageHandler(this);
		auto WindowHandle=fWindow->GetWindowHandle();
		ThemeSetup(WindowHandle);
	}
	else{
		ThemeSetup(nullptr);
	}
	fDCViewContent->InvalidateRect(&fPaintRC);
}
//---------------------------------------------------------------------------
void vWinDCThemePainter::ClearPaintWindow(void)noexcept(true)
{
	ThemeClear();
	if(fWindow!=nullptr){
		fWindow->RemoveMessageHandler(this);
	}
}
//---------------------------------------------------------------------------
void vWinDCThemePainter::WindowAttached(void)noexcept(true)
{
	auto WindowHandle=fWindow->GetWindowHandle();
	ThemeClear();
	ThemeSetup(WindowHandle);
}
//---------------------------------------------------------------------------
void vWinDCThemePainter::WindowDetached(void)noexcept(true)
{
	ThemeClear();
	ThemeSetup(nullptr);
}
//---------------------------------------------------------------------------
bool vWinDCThemePainter::WindowMessage(LRESULT &MsgResult,const cWindowMessageParam &Message)noexcept(true)
{MsgResult,Message;
	return false;
}
//---------------------------------------------------------------------------
void vWinDCThemePainter::WindowMessageProcessed(LRESULT,const cWindowMessageParam &Message)noexcept(true)
{
	switch(Message.Code){
	case WM_THEMECHANGED:
		auto WindowHandle=fWindow->GetWindowHandle();
		ThemeClear();
		ThemeSetup(WindowHandle);
		break;
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWinNCFramePainter::cWinNCFramePainter()noexcept(true)
{
}
cWinNCFramePainter::~cWinNCFramePainter()noexcept(true)
{
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWinNCFramePainter_Theme::cWinNCFramePainter_Theme(HTHEME Theme)noexcept(true)
{
	fTheme=Theme;
}
//---------------------------------------------------------------------------
cWinNCFramePainter_Theme::~cWinNCFramePainter_Theme()noexcept(true)
{
}
//---------------------------------------------------------------------------
HTHEME cWinNCFramePainter_Theme::OpenTheme(HWND WindowHandle)noexcept(true)
{
	return ::OpenThemeData(WindowHandle,VSCLASS_WINDOW);
}
//---------------------------------------------------------------------------
void cWinNCFramePainter_Theme::PaintFrame(HDC DC,const RECT &Rect,const RECT &Border,Float32 CaptionHeight,bool Active)noexcept(true)
{
	int StateID;
	RECT rc;

	if(Active){
		StateID=FS_ACTIVE;
	}
	else{
		StateID=FS_INACTIVE;
	}
	rc.left=Rect.left;
	rc.top=Rect.top;
	rc.bottom=Rect.bottom;
	rc.right=rc.left+Border.left;
	::DrawThemeBackground(fTheme,DC,WP_FRAMELEFT,StateID,&rc,nullptr);
	rc.right=Rect.right;
	rc.left=rc.right-Border.right;
	::DrawThemeBackground(fTheme,DC,WP_FRAMERIGHT,StateID,&rc,nullptr);

	rc.left=Rect.left;
	rc.right=Rect.right;
	rc.bottom=Rect.bottom;
	rc.top=rc.bottom-Border.bottom;
	::DrawThemeBackground(fTheme,DC,WP_FRAMEBOTTOM,StateID,&rc,nullptr);

	rc.top=Rect.top;
	rc.bottom=rc.top+Border.top+CaptionHeight;
	if(Active){
		StateID=CS_ACTIVE;
	}
	else{
		StateID=CS_INACTIVE;
	}
	::DrawThemeBackground(fTheme,DC,WP_CAPTION,StateID,&rc,nullptr);
}
//---------------------------------------------------------------------------
void cWinNCFramePainter_Theme::PaintSmallFrame(HDC DC,const RECT &Rect,const RECT &Border,Float32 CaptionHeight,bool Active)noexcept(true)
{
	int StateID;
	RECT rc;

	if(Active){
		StateID=FS_ACTIVE;
	}
	else{
		StateID=FS_INACTIVE;
	}
	rc.left=Rect.left;
	rc.top=Rect.top;
	rc.bottom=Rect.bottom;
	rc.right=rc.left+Border.left;
	::DrawThemeBackground(fTheme,DC,WP_SMALLFRAMELEFT,StateID,&rc,nullptr);
	rc.right=Rect.right;
	rc.left=rc.right-Border.right;
	::DrawThemeBackground(fTheme,DC,WP_SMALLFRAMERIGHT,StateID,&rc,nullptr);

	rc.left=Rect.left;
	rc.right=Rect.right;
	rc.bottom=Rect.bottom;
	rc.top=rc.bottom-Border.bottom;
	::DrawThemeBackground(fTheme,DC,WP_SMALLFRAMEBOTTOM,StateID,&rc,nullptr);

	rc.top=Rect.top;
	rc.bottom=rc.top+Border.top+CaptionHeight;
	if(Active){
		StateID=CS_ACTIVE;
	}
	else{
		StateID=CS_INACTIVE;
	}
	::DrawThemeBackground(fTheme,DC,WP_SMALLCAPTION,StateID,&rc,nullptr);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWinNCButtonPainter::cWinNCButtonPainter()noexcept(true)
{
}
cWinNCButtonPainter::~cWinNCButtonPainter()noexcept(true)
{
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWinNCButtonPainter_Theme::cWinNCButtonPainter_Theme(HTHEME Theme)noexcept(true)
{
	fTheme=Theme;
}
//---------------------------------------------------------------------------
cWinNCButtonPainter_Theme::~cWinNCButtonPainter_Theme()noexcept(true)
{
	::CloseThemeData(fTheme);
}
//---------------------------------------------------------------------------
HTHEME cWinNCButtonPainter_Theme::OpenTheme(HWND WindowHandle)noexcept(true)
{
	return ::OpenThemeData(WindowHandle,VSCLASS_WINDOW);
}
//---------------------------------------------------------------------------
void cWinNCButtonPainter_Theme::PaintCloseButton(HDC DC,const RECT &Rect,eButtonState State)noexcept(true)
{
	int StateID;
	switch(State){
	default:
	case ButtonState::Normal:
		StateID=CBS_NORMAL;
		break;
	case ButtonState::Hot:
		StateID=CBS_HOT;
		break;
	case ButtonState::Pressed:
		StateID=CBS_PUSHED;
		break;
	case ButtonState::Disabled:
		StateID=CBS_DISABLED;
		break;
	case ButtonState::Inactive:
		StateID=5;
		break;
	}
	::DrawThemeBackground(fTheme,DC,WP_CLOSEBUTTON,StateID,&Rect,nullptr);
}
//---------------------------------------------------------------------------
void cWinNCButtonPainter_Theme::PaintMinButton(HDC DC,const RECT &Rect,eButtonState State)noexcept(true)
{
	int StateID;
	switch(State){
	default:
	case ButtonState::Normal:
		StateID=MINBS_NORMAL;
		break;
	case ButtonState::Hot:
		StateID=MINBS_HOT;
		break;
	case ButtonState::Pressed:
		StateID=MINBS_PUSHED;
		break;
	case ButtonState::Disabled:
		StateID=MINBS_DISABLED;
		break;
	case ButtonState::Inactive:
		StateID=5;
		break;
	}
	::DrawThemeBackground(fTheme,DC,WP_MINBUTTON,StateID,&Rect,nullptr);
}
//---------------------------------------------------------------------------
void cWinNCButtonPainter_Theme::PaintMaxButton(HDC DC,const RECT &Rect,eButtonState State)noexcept(true)
{
	int StateID;
	switch(State){
	default:
	case ButtonState::Normal:
		StateID=MAXBS_NORMAL;
		break;
	case ButtonState::Hot:
		StateID=MAXBS_HOT;
		break;
	case ButtonState::Pressed:
		StateID=MAXBS_PUSHED;
		break;
	case ButtonState::Disabled:
		StateID=MAXBS_DISABLED;
		break;
	case ButtonState::Inactive:
		StateID=5;
		break;
	}
	::DrawThemeBackground(fTheme,DC,WP_MAXBUTTON,StateID,&Rect,nullptr);
}
//---------------------------------------------------------------------------
void cWinNCButtonPainter_Theme::PaintRestoreButton(HDC DC,const RECT &Rect,eButtonState State)noexcept(true)
{
	int StateID;
	switch(State){
	default:
	case ButtonState::Normal:
		StateID=RBS_NORMAL;
		break;
	case ButtonState::Hot:
		StateID=RBS_HOT;
		break;
	case ButtonState::Pressed:
		StateID=RBS_PUSHED;
		break;
	case ButtonState::Disabled:
		StateID=RBS_DISABLED;
		break;
	case ButtonState::Inactive:
		StateID=5;
		break;
	}
	::DrawThemeBackground(fTheme,DC,WP_RESTOREBUTTON,StateID,&Rect,nullptr);
}
//---------------------------------------------------------------------------
void cWinNCButtonPainter_Theme::PaintSmallCloseButton(HDC DC,const RECT &Rect,eButtonState State)noexcept(true)
{
	int StateID;
	switch(State){
	default:
	case ButtonState::Normal:
		StateID=CBS_NORMAL;
		break;
	case ButtonState::Hot:
		StateID=CBS_HOT;
		break;
	case ButtonState::Pressed:
		StateID=CBS_PUSHED;
		break;
	case ButtonState::Disabled:
		StateID=CBS_DISABLED;
		break;
	case ButtonState::Inactive:
		StateID=5;
		break;
	}
	::DrawThemeBackground(fTheme,DC,WP_SMALLCLOSEBUTTON,StateID,&Rect,nullptr);
}
//---------------------------------------------------------------------------
void cWinNCButtonPainter_Theme::PaintHelpButton(HDC DC,const RECT &Rect,eButtonState State)noexcept(true)
{
	int StateID;
	switch(State){
	default:
	case ButtonState::Normal:
		StateID=HBS_NORMAL;
		break;
	case ButtonState::Hot:
		StateID=HBS_HOT;
		break;
	case ButtonState::Pressed:
		StateID=HBS_PUSHED;
		break;
	case ButtonState::Disabled:
		StateID=HBS_DISABLED;
		break;
	case ButtonState::Inactive:
		StateID=5;
		break;
	}
	::DrawThemeBackground(fTheme,DC,WP_HELPBUTTON,StateID,&Rect,nullptr);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWinGroupBoxPainter::cWinGroupBoxPainter()noexcept(true)
{
}
cWinGroupBoxPainter::~cWinGroupBoxPainter()noexcept(true)
{
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWinGroupBoxPainter_Theme::cWinGroupBoxPainter_Theme(HTHEME Theme)noexcept(true)
{
	fTheme=Theme;
}
//---------------------------------------------------------------------------
cWinGroupBoxPainter_Theme::~cWinGroupBoxPainter_Theme()noexcept(true)
{
	::CloseThemeData(fTheme);
}
//---------------------------------------------------------------------------
HTHEME cWinGroupBoxPainter_Theme::OpenTheme(HWND WindowHandle)noexcept(true)
{
	return ::OpenThemeData(WindowHandle,VSCLASS_BUTTON);
}
//---------------------------------------------------------------------------
cUIRectangle cWinGroupBoxPainter_Theme::GetContentMarginRect(void)noexcept(true)
{
	HRESULT hr;
	MARGINS Margin={0,0,0,0};
	//hr=::GetThemeMargins(fTheme,nullptr,BP_GROUPBOX,GBS_NORMAL,TMT_CONTENTMARGINS,nullptr,&Margin);
	hr=::GetThemeMargins(fTheme,nullptr,BP_GROUPBOX,GBS_NORMAL,TMT_SIZINGMARGINS,nullptr,&Margin);
	
	return {
		static_cast<Float32>(Margin.cxLeftWidth),static_cast<Float32>(Margin.cyTopHeight),
		static_cast<Float32>(Margin.cxRightWidth),static_cast<Float32>(Margin.cyBottomHeight)
	};
}
//---------------------------------------------------------------------------
void cWinGroupBoxPainter_Theme::PaintBox(HDC DC,const RECT &Rect)noexcept(true)
{
	HRESULT hr;
	hr=::DrawThemeBackground(fTheme,DC,BP_GROUPBOX,GBS_NORMAL,&Rect,nullptr);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWinPushButtonPainter::cWinPushButtonPainter()noexcept(true)
{
}
cWinPushButtonPainter::~cWinPushButtonPainter()noexcept(true)
{
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWinPushButtonPainter_Theme::cWinPushButtonPainter_Theme(HTHEME Theme)noexcept(true)
{
	fTheme=Theme;
}
//---------------------------------------------------------------------------
cWinPushButtonPainter_Theme::~cWinPushButtonPainter_Theme()noexcept(true)
{
	::CloseThemeData(fTheme);
}
//---------------------------------------------------------------------------
HTHEME cWinPushButtonPainter_Theme::OpenTheme(HWND WindowHandle)noexcept(true)
{
	return ::OpenThemeData(WindowHandle,VSCLASS_BUTTON);
}
//---------------------------------------------------------------------------
int cWinPushButtonPainter_Theme::StateIDFromState(eButtonState State)noexcept(true)
{
	int StateID;
	switch(State){
	default:
	case ButtonState::Inactive:
	case ButtonState::Normal:
		StateID=PBS_NORMAL;
		break;
	case ButtonState::Hot:
		StateID=PBS_HOT;
		break;
	case ButtonState::Pressed:
		StateID=PBS_PRESSED;
		break;
	case ButtonState::Disabled:
		StateID=PBS_DISABLED;
		break;
	}

	return StateID;

}
//---------------------------------------------------------------------------
cUIRectangle cWinPushButtonPainter_Theme::GetContentMarginRect(void)noexcept(true)
{
	HRESULT hr;
	MARGINS Margin={0,0,0,0};
	hr=::GetThemeMargins(fTheme,nullptr,BP_PUSHBUTTON,PBS_NORMAL,TMT_CONTENTMARGINS,nullptr,&Margin);
	
	return {
		static_cast<Float32>(Margin.cxLeftWidth),static_cast<Float32>(Margin.cyTopHeight),
		static_cast<Float32>(Margin.cxRightWidth),static_cast<Float32>(Margin.cyBottomHeight)
	};
}
//---------------------------------------------------------------------------
void cWinPushButtonPainter_Theme::PaintButton(HDC DC,const RECT &Rect,eButtonState State)noexcept(true)
{
	auto StateID=StateIDFromState(State);

	HRESULT hr;
	hr=::DrawThemeBackground(fTheme,DC,BP_PUSHBUTTON,StateID,&Rect,nullptr);
}
//---------------------------------------------------------------------------
void cWinPushButtonPainter_Theme::PaintText(HDC DC,const RECT &TextRect,const wchar_t *Text,uIntn TextLength,HFONT TextFont,eButtonState State)noexcept(true)
{
	auto StateID=StateIDFromState(State);
	HGDIOBJ OldFont=nullptr;
	if(TextFont!=nullptr){
		OldFont=::SelectObject(DC,TextFont);
	}
	
	::DrawThemeText(fTheme,DC,BP_PUSHBUTTON,StateID,Text,static_cast<int>(TextLength),
		DT_SINGLELINE|DT_CENTER|DT_VCENTER,0,&TextRect);
	
	if(OldFont!=nullptr){
		::SelectObject(DC,OldFont);
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWinStateButtonPainter::cWinStateButtonPainter()noexcept(true)
{
}
cWinStateButtonPainter::~cWinStateButtonPainter()noexcept(true)
{
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWinCheckButtonPainter_Theme::cWinCheckButtonPainter_Theme(HTHEME Theme)noexcept(true)
{
	fTheme=Theme;
}
//---------------------------------------------------------------------------
cWinCheckButtonPainter_Theme::~cWinCheckButtonPainter_Theme()noexcept(true)
{
	::CloseThemeData(fTheme);
}
//---------------------------------------------------------------------------
HTHEME cWinCheckButtonPainter_Theme::OpenTheme(HWND WindowHandle)noexcept(true)
{
	return ::OpenThemeData(WindowHandle,VSCLASS_BUTTON);
}
//---------------------------------------------------------------------------
int cWinCheckButtonPainter_Theme::StateIDFromState(eButtonState State,uInt8 CheckState)noexcept(true)
{
	static constexpr int CheckBoxStateMap[3][4]={
		{	// Unchecked
			CBS_UNCHECKEDNORMAL,	// StateNormal
			CBS_UNCHECKEDHOT,		// StateHot
			CBS_UNCHECKEDPRESSED,	// StatePressed
			CBS_UNCHECKEDDISABLED,	// StateDisabled
		},
		{	// Checked
			CBS_CHECKEDNORMAL,		// StateNormal
			CBS_CHECKEDHOT,			// StateHot
			CBS_CHECKEDPRESSED,		// StatePressed
			CBS_CHECKEDDISABLED,	// StateDisabled
		},
		{	// Mixed
			CBS_MIXEDNORMAL,	// StateNormal
			CBS_MIXEDHOT,		// StateHot
			CBS_MIXEDPRESSED,	// StatePressed
			CBS_MIXEDDISABLED,	// StateDisabled
		},

	};

	const int *pStateMap;
	if(CheckState>=2){
		pStateMap=CheckBoxStateMap[2];
	}
	else{
		pStateMap=CheckBoxStateMap[CheckState];
	}

	int StateID;
	switch(State){
	default:
	case ButtonState::Inactive:
	case ButtonState::Normal:
		StateID=pStateMap[0];
		break;
	case ButtonState::Hot:
		StateID=pStateMap[1];
		break;
	case ButtonState::Pressed:
		StateID=pStateMap[2];
		break;
	case ButtonState::Disabled:
		StateID=pStateMap[3];
		break;
	}
	return StateID;
}
//---------------------------------------------------------------------------
cUIPoint cWinCheckButtonPainter_Theme::GetBoxSize(HDC DC)noexcept(true)
{
	SIZE BGSize;
	::GetThemePartSize(fTheme,DC,BP_RADIOBUTTON,RBS_UNCHECKEDNORMAL,nullptr,TS_TRUE,&BGSize);

	return {static_cast<Float32>(BGSize.cx),static_cast<Float32>(BGSize.cy)};
}
//---------------------------------------------------------------------------
void cWinCheckButtonPainter_Theme::PaintButton(HDC DC,const RECT &Rect,eButtonState State,ufInt8 CheckState)noexcept(true)
{
	auto StateID=StateIDFromState(State,CheckState);

	::DrawThemeBackground(fTheme,DC,BP_CHECKBOX,StateID,&Rect,nullptr);
}
//---------------------------------------------------------------------------
void cWinCheckButtonPainter_Theme::PaintText(HDC DC,const RECT &TextRect,const wchar_t *Text,uIntn TextLength,HFONT TextFont,eButtonState State,uInt8 CheckState)noexcept(true)
{	State,CheckState;
	HGDIOBJ OldFont=nullptr;
	if(TextFont!=nullptr){
		OldFont=::SelectObject(DC,TextFont);
	}

	::ExtTextOutW(DC,TextRect.left,TextRect.top,ETO_CLIPPED,&TextRect,Text,static_cast<UINT>(TextLength),nullptr);
	
	if(OldFont!=nullptr){
		::SelectObject(DC,OldFont);
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWinRadioButtonPainter_Theme::cWinRadioButtonPainter_Theme(HTHEME Theme)noexcept(true)
{
	fTheme=Theme;
}
//---------------------------------------------------------------------------
cWinRadioButtonPainter_Theme::~cWinRadioButtonPainter_Theme()noexcept(true)
{
	::CloseThemeData(fTheme);
}
//---------------------------------------------------------------------------
HTHEME cWinRadioButtonPainter_Theme::OpenTheme(HWND WindowHandle)noexcept(true)
{
	return ::OpenThemeData(WindowHandle,VSCLASS_BUTTON);
}
//---------------------------------------------------------------------------
int cWinRadioButtonPainter_Theme::StateIDFromState(eButtonState State,uInt8 CheckState)noexcept(true)
{
	static constexpr int RadioBoxStateMap[2][4]={
		{	// Unchecked
			RBS_UNCHECKEDNORMAL,	// StateNormal
			RBS_UNCHECKEDHOT,		// StateHot
			RBS_UNCHECKEDPRESSED,	// StatePressed
			RBS_UNCHECKEDDISABLED,	// StateDisabled
		},
		{	// Checked
			RBS_CHECKEDNORMAL,		// StateNormal
			RBS_CHECKEDHOT,			// StateHot
			RBS_CHECKEDPRESSED,		// StatePressed
			RBS_CHECKEDDISABLED,	// StateDisabled
		},
	};

	const int *pStateMap=RadioBoxStateMap[CheckState!=0];

	int StateID;
	switch(State){
	default:
	case ButtonState::Inactive:
	case ButtonState::Normal:
		StateID=pStateMap[0];
		break;
	case ButtonState::Hot:
		StateID=pStateMap[1];
		break;
	case ButtonState::Pressed:
		StateID=pStateMap[2];
		break;
	case ButtonState::Disabled:
		StateID=pStateMap[3];
		break;
	}
	return StateID;
}
//---------------------------------------------------------------------------
cUIPoint cWinRadioButtonPainter_Theme::GetBoxSize(HDC DC)noexcept(true)
{
	SIZE BGSize;
	::GetThemePartSize(fTheme,DC,BP_RADIOBUTTON,RBS_UNCHECKEDNORMAL,nullptr,TS_TRUE,&BGSize);

	return {static_cast<Float32>(BGSize.cx),static_cast<Float32>(BGSize.cy)};
}
//---------------------------------------------------------------------------
void cWinRadioButtonPainter_Theme::PaintButton(HDC DC,const RECT &Rect,eButtonState State,ufInt8 CheckState)noexcept(true)
{
	auto StateID=StateIDFromState(State,CheckState);

	::DrawThemeBackground(fTheme,DC,BP_RADIOBUTTON,StateID,&Rect,nullptr);
}
//---------------------------------------------------------------------------
void cWinRadioButtonPainter_Theme::PaintText(HDC DC,const RECT &TextRect,const wchar_t *Text,uIntn TextLength,HFONT TextFont,eButtonState State,uInt8 CheckState)noexcept(true)
{	State,CheckState;
	HGDIOBJ OldFont=nullptr;
	if(TextFont!=nullptr){
		OldFont=::SelectObject(DC,TextFont);
	}
	::ExtTextOutW(DC,TextRect.left,TextRect.top,ETO_CLIPPED,&TextRect,Text,static_cast<UINT>(TextLength),nullptr);
	
	if(OldFont!=nullptr){
		::SelectObject(DC,OldFont);
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWinSpinButtonPainter::cWinSpinButtonPainter()noexcept(true)
{
}
cWinSpinButtonPainter::~cWinSpinButtonPainter()noexcept(true)
{
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWinSpinButtonPainter_Theme::cWinSpinButtonPainter_Theme(HTHEME Theme)noexcept(true)
{
	fTheme=Theme;
}
//---------------------------------------------------------------------------
cWinSpinButtonPainter_Theme::~cWinSpinButtonPainter_Theme()noexcept(true)
{
	::CloseThemeData(fTheme);
}
//---------------------------------------------------------------------------
HTHEME cWinSpinButtonPainter_Theme::OpenTheme(HWND WindowHandle)noexcept(true)
{
	return ::OpenThemeData(WindowHandle,VSCLASS_SPIN);
}
//---------------------------------------------------------------------------
int cWinSpinButtonPainter_Theme::StateIDFromState(eButtonState State)noexcept(true)
{
	int StateID;
	switch(State){
	default:
	case ButtonState::Inactive:
	case ButtonState::Normal:
		StateID=DNS_NORMAL;
		break;
	case ButtonState::Hot:
		StateID=DNS_HOT;
		break;
	case ButtonState::Pressed:
		StateID=DNS_PRESSED;
		break;
	case ButtonState::Disabled:
		StateID=DNS_DISABLED;
		break;
	}

	return StateID;

}
//---------------------------------------------------------------------------
void cWinSpinButtonPainter_Theme::PaintUpButton(HDC DC,const RECT &Rect,eButtonState State)noexcept(true)
{
	auto StateID=StateIDFromState(State);

	HRESULT hr;
	hr=::DrawThemeBackground(fTheme,DC,SPNP_UP,StateID,&Rect,nullptr);
}
//---------------------------------------------------------------------------
void cWinSpinButtonPainter_Theme::PaintDownButton(HDC DC,const RECT &Rect,eButtonState State)noexcept(true)
{
	auto StateID=StateIDFromState(State);

	HRESULT hr;
	hr=::DrawThemeBackground(fTheme,DC,SPNP_DOWN,StateID,&Rect,nullptr);
}
//---------------------------------------------------------------------------
void cWinSpinButtonPainter_Theme::PaintLeftButton(HDC DC,const RECT &Rect,eButtonState State)noexcept(true)
{
	auto StateID=StateIDFromState(State);

	HRESULT hr;
	hr=::DrawThemeBackground(fTheme,DC,SPNP_UPHORZ,StateID,&Rect,nullptr);
}
//---------------------------------------------------------------------------
void cWinSpinButtonPainter_Theme::PaintRightButton(HDC DC,const RECT &Rect,eButtonState State)noexcept(true)
{
	auto StateID=StateIDFromState(State);

	HRESULT hr;
	hr=::DrawThemeBackground(fTheme,DC,SPNP_DOWNHORZ,StateID,&Rect,nullptr);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWinEditBackgroundPainter::cWinEditBackgroundPainter()noexcept(true)
{
}
cWinEditBackgroundPainter::~cWinEditBackgroundPainter()noexcept(true)
{
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWinEditBackgroundPainter_Theme::cWinEditBackgroundPainter_Theme(HTHEME Theme)noexcept(true)
{
	fTheme=Theme;
}
//---------------------------------------------------------------------------
cWinEditBackgroundPainter_Theme::~cWinEditBackgroundPainter_Theme()noexcept(true)
{
	::CloseThemeData(fTheme);
}
//---------------------------------------------------------------------------
HTHEME cWinEditBackgroundPainter_Theme::OpenTheme(HWND WindowHandle)noexcept(true)
{
	return ::OpenThemeData(WindowHandle,VSCLASS_EDIT);
}
//---------------------------------------------------------------------------
cUIRectangle cWinEditBackgroundPainter_Theme::GetContentMarginRect(void)noexcept(true)
{
	HRESULT hr;
	MARGINS Margin={0,0,0,0};
	//hr=::GetThemeMargins(fTheme,nullptr,EP_EDITBORDER_NOSCROLL,EPSN_NORMAL,TMT_CONTENTMARGINS,nullptr,&Margin);
	hr=::GetThemeMargins(fTheme,nullptr,EP_EDITBORDER_NOSCROLL,EPSN_NORMAL,TMT_SIZINGMARGINS,nullptr,&Margin);
	
	return {
		static_cast<Float32>(Margin.cxLeftWidth),static_cast<Float32>(Margin.cyTopHeight),
		static_cast<Float32>(Margin.cxRightWidth),static_cast<Float32>(Margin.cyBottomHeight)
	};
}
//---------------------------------------------------------------------------
void cWinEditBackgroundPainter_Theme::Paint(HDC DC,const RECT &Rect,ControlState State)noexcept(true)
{
	int StateID;
	switch(State){
	default:
	case ControlState::Normal:
		StateID=EPSN_NORMAL;
		break;
	case ControlState::Hot:
		StateID=EPSN_HOT;
		break;
	case ControlState::Focused:
		StateID=EPSN_FOCUSED;
		break;
	case ControlState::Disabled:
		StateID=EPSN_DISABLED;
		break;
	}

	::DrawThemeBackground(fTheme,DC,EP_EDITBORDER_NOSCROLL,StateID,&Rect,nullptr);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWinTabPainter::cWinTabPainter()noexcept(true)
{
}
cWinTabPainter::~cWinTabPainter()noexcept(true)
{
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWinTabPainter_Theme::cWinTabPainter_Theme(HTHEME Theme)noexcept(true)
{
	fTheme=Theme;
}
//---------------------------------------------------------------------------
cWinTabPainter_Theme::~cWinTabPainter_Theme()noexcept(true)
{
	::CloseThemeData(fTheme);
}
//---------------------------------------------------------------------------
HTHEME cWinTabPainter_Theme::OpenTheme(HWND WindowHandle)noexcept(true)
{
	return ::OpenThemeData(WindowHandle,VSCLASS_TAB);
}
//---------------------------------------------------------------------------
ufInt16 cWinTabPainter_Theme::TabBorderSize(void)noexcept(true)
{
	return 4;
}
//---------------------------------------------------------------------------
void cWinTabPainter_Theme::PaintPanelHeader(HDC DC,const RECT &PanelRect)noexcept(true)
{
	auto PenObject=fPanelHeaderPenRecycler.Query();

	auto OldPen=::SelectObject(DC,PenObject->Pen);
	::MoveToEx(DC,PanelRect.left,PanelRect.top,nullptr);
	::LineTo(DC,PanelRect.right,PanelRect.top);
	::SelectObject(DC,OldPen);
}
//---------------------------------------------------------------------------
void cWinTabPainter_Theme::TabToTextRect(RECT &Rect,TabState State)noexcept(true)
{
	if(State==TabState::Active){
		Rect.left+=4;
		Rect.right-=4;
		Rect.bottom-=2;
	}
	else{
		Rect.left+=4;
		Rect.top+=2;
		Rect.right-=4;
		Rect.bottom-=2;
	}
}
//---------------------------------------------------------------------------
RECT cWinTabPainter_Theme::PaintTabItem(HDC DC,const RECT &Rect,TabState State,uInt8 PartID)noexcept(true)
{
	RECT PaintRect=Rect;
	int StateID;
	switch(State){
	default:
	case TabState::Normal:
		PaintRect.top+=2;
		PaintRect.bottom-=1;
		StateID=TTIS_NORMAL;
		break;
	case TabState::Hot:
		PaintRect.top+=2;
		PaintRect.bottom-=1;
		StateID=TTIS_HOT;
		break;
	case TabState::Active:
		PaintRect.left-=2;
		PaintRect.right+=2;
		StateID=TTIS_SELECTED;
		break;
	}
	::DrawThemeBackground(fTheme,DC,PartID,StateID,&PaintRect,nullptr);


	TabToTextRect(PaintRect,State);
	return PaintRect;
}
//---------------------------------------------------------------------------
RECT cWinTabPainter_Theme::PaintItemLeft(HDC DC,const RECT &Rect,TabState State)noexcept(true)
{
	return PaintTabItem(DC,Rect,State,TABP_TABITEMLEFTEDGE);
}
//---------------------------------------------------------------------------
RECT cWinTabPainter_Theme::PaintItemMiddle(HDC DC,const RECT &Rect,TabState State)noexcept(true)
{
	return PaintTabItem(DC,Rect,State,TABP_TABITEM);
}
//---------------------------------------------------------------------------
RECT cWinTabPainter_Theme::PaintItemRight(HDC DC,const RECT &Rect,TabState State)noexcept(true)
{
	return PaintTabItem(DC,Rect,State,TABP_TABITEMRIGHTEDGE);
}
//---------------------------------------------------------------------------
RECT cWinTabPainter_Theme::PaintItemSingle(HDC DC,const RECT &Rect,TabState State)noexcept(true)
{
	return PaintTabItem(DC,Rect,State,TABP_TABITEMBOTHEDGE);
}
//---------------------------------------------------------------------------
void cWinTabPainter_Theme::PaintItemText(HDC DC,const RECT &TextRect,const wchar_t *Text,uIntn TextLength,HFONT TextFont,TabState State)noexcept(true)
{State;
	HGDIOBJ OldFont=nullptr;
	if(TextFont!=nullptr){
		OldFont=::SelectObject(DC,TextFont);
	}
	::SetTextColor(DC,0);
	::SetBkMode(DC,TRANSPARENT);

	DrawTextExW(DC,(LPWSTR)Text,static_cast<int>(TextLength),const_cast<LPRECT>(&TextRect),DT_SINGLELINE|DT_VCENTER|DT_CENTER,nullptr);

	if(OldFont!=nullptr){
		::SelectObject(DC,OldFont);
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWinScrollBarPainter::cWinScrollBarPainter()noexcept(true)
{
}
cWinScrollBarPainter::~cWinScrollBarPainter()noexcept(true)
{
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWinScrollBarPainter_Theme::cWinScrollBarPainter_Theme(HTHEME Theme)noexcept(true)
{
	fTheme=Theme;
}
//---------------------------------------------------------------------------
cWinScrollBarPainter_Theme::~cWinScrollBarPainter_Theme()noexcept(true)
{
	::CloseThemeData(fTheme);
}
//---------------------------------------------------------------------------
HTHEME cWinScrollBarPainter_Theme::OpenTheme(HWND WindowHandle)noexcept(true)
{
	return ::OpenThemeData(WindowHandle,VSCLASS_SCROLLBAR);
}
//---------------------------------------------------------------------------
uIntn cWinScrollBarPainter_Theme::BtnStateIndex(eButtonState State,bool IsActive)noexcept(true)
{
	switch(State){
	default:
	case ButtonState::Inactive:
	case ButtonState::Normal:
		return siNormal;
	case ButtonState::Hot:
		if(IsActive){
			return siHot;
		}
		else{
			return siHighlight;
		}
	case ButtonState::Pressed:
		if(IsActive){
			return siPressed;
		}
		else{
			return siHighlight;
		}
	case ButtonState::Disabled:
		return siDisabled;
	}
}
//---------------------------------------------------------------------------
int cWinScrollBarPainter_Theme::BtnStateFlag(eButtonState State,bool IsActive)noexcept(true)
{
	auto StateIndex=BtnStateIndex(State,IsActive);

	static const int StateMap[6]={
		SCRBS_NORMAL,	// siNormal
		SCRBS_HOT,		// siHot
		SCRBS_PRESSED,	// siPressed
		SCRBS_DISABLED,	// siDisabled
		SCRBS_HOVER,	// siHighlight
	};
	return StateMap[StateIndex];
}
//---------------------------------------------------------------------------
Float32 cWinScrollBarPainter_Theme::GetMinTrackButtonSize(void)noexcept(true)
{
	return 20;
}
//---------------------------------------------------------------------------
void cWinScrollBarPainter_Theme::PaintDec(const PaintParam &Param)noexcept(true)
{
	auto StateIndex=BtnStateIndex(Param.State,Param.ActiveButton==ScrollBarButton::Dec);

	static const int VerticalStateMap[6]={
		ABS_UPNORMAL,	// siNormal
		ABS_UPHOT,		// siHot
		ABS_UPPRESSED,	// siPressed
		ABS_UPDISABLED,	// siDisabled
		ABS_UPHOVER,	// siHighlight
	};
	static const int HorizontalStateMap[6]={
		ABS_LEFTNORMAL,		// siNormal
		ABS_LEFTHOT,		// siHot
		ABS_LEFTPRESSED,	// siPressed
		ABS_LEFTDISABLED,	// siDisabled
		ABS_LEFTHOVER,		// siHighlight
	};

	int StateID;
	if(Param.IsVertical){
		StateID=VerticalStateMap[StateIndex];
	}
	else{
		StateID=HorizontalStateMap[StateIndex];
	}
	::DrawThemeBackground(fTheme,Param.DC,SBP_ARROWBTN,StateID,&Param.Rect,nullptr);
}
//---------------------------------------------------------------------------
void cWinScrollBarPainter_Theme::PaintInc(const PaintParam &Param)noexcept(true)
{
	auto StateIndex=BtnStateIndex(Param.State,Param.ActiveButton==ScrollBarButton::Inc);

	static const int VerticalStateMap[6]={
		ABS_DOWNNORMAL,		// siNormal
		ABS_DOWNHOT,		// siHot
		ABS_DOWNPRESSED,	// siPressed
		ABS_DOWNDISABLED,	// siDisabled
		ABS_DOWNHOVER,		// siHighlight
	};
	static const int HorizontalStateMap[6]={
		ABS_RIGHTNORMAL,	// siNormal
		ABS_RIGHTHOT,		// siHot
		ABS_RIGHTPRESSED,	// siPressed
		ABS_RIGHTDISABLED,	// siDisabled
		ABS_RIGHTHOVER,		// siHighlight
	};

	int StateID;
	if(Param.IsVertical){
		StateID=VerticalStateMap[StateIndex];
	}
	else{
		StateID=HorizontalStateMap[StateIndex];
	}
	::DrawThemeBackground(fTheme,Param.DC,SBP_ARROWBTN,StateID,&Param.Rect,nullptr);
}
//---------------------------------------------------------------------------
void cWinScrollBarPainter_Theme::PaintDecBar(const PaintParam &Param)noexcept(true)
{
	int PartID;
	int StateID;
	if(Param.IsVertical){
		PartID=SBP_LOWERTRACKVERT;
	}
	else{
		PartID=SBP_LOWERTRACKHORZ;
	}
	StateID=BtnStateFlag(Param.State,Param.ActiveButton==ScrollBarButton::DecBar);
	::DrawThemeBackground(fTheme,Param.DC,PartID,StateID,&Param.Rect,nullptr);
}
//---------------------------------------------------------------------------
void cWinScrollBarPainter_Theme::PaintIncBar(const PaintParam &Param)noexcept(true)
{
	int PartID;
	int StateID;
	if(Param.IsVertical){
		PartID=SBP_UPPERTRACKVERT;
	}
	else{
		PartID=SBP_UPPERTRACKHORZ;
	}
	StateID=BtnStateFlag(Param.State,Param.ActiveButton==ScrollBarButton::IncBar);
	::DrawThemeBackground(fTheme,Param.DC,PartID,StateID,&Param.Rect,nullptr);
}
//---------------------------------------------------------------------------
void cWinScrollBarPainter_Theme::PaintTrack(const PaintParam &Param)noexcept(true)
{
	int PartID;
	int StateID;
	if(Param.IsVertical){
		PartID=SBP_THUMBBTNVERT;
	}
	else{
		PartID=SBP_THUMBBTNHORZ;
	}
	StateID=BtnStateFlag(Param.State,Param.ActiveButton==ScrollBarButton::Track);
	::DrawThemeBackground(fTheme,Param.DC,PartID,StateID,&Param.Rect,nullptr);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWinScrollBarGripPainter::cWinScrollBarGripPainter()noexcept(true)
{
}
cWinScrollBarGripPainter::~cWinScrollBarGripPainter()noexcept(true)
{
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWinScrollBarGripPainter_Theme::cWinScrollBarGripPainter_Theme(HTHEME Theme)noexcept(true)
{
	fTheme=Theme;
}
//---------------------------------------------------------------------------
cWinScrollBarGripPainter_Theme::~cWinScrollBarGripPainter_Theme()noexcept(true)
{
	::CloseThemeData(fTheme);
}
//---------------------------------------------------------------------------
HTHEME cWinScrollBarGripPainter_Theme::OpenTheme(HWND WindowHandle)noexcept(true)
{
	return ::OpenThemeData(WindowHandle,VSCLASS_SCROLLBAR);
}
//---------------------------------------------------------------------------
void cWinScrollBarGripPainter_Theme::Paint(HDC DC,const RECT &Rect,eAlignment BoxAlign)noexcept(true)
{
	::SetBkColor(DC,::GetSysColor(COLOR_BTNFACE));
	::ExtTextOutW(DC,0,0,ETO_OPAQUE,&Rect,nullptr,0,nullptr);
	ufInt8 AlignLeft,AlignTop;
	AlignLeft=(BoxAlign&Alignment::Left)!=Alignment::None;
	AlignTop=(BoxAlign&Alignment::Top)!=Alignment::None;
	int State;
	switch(AlignLeft | (AlignTop<<1)){
	default:
	case 0:	// BottomRight
		State=SZB_RIGHTALIGN;
		break;
	case 1:	// BottomLeft
		State=SZB_LEFTALIGN;
		break;
	case 2:	// TopRight
		State=SZB_TOPRIGHTALIGN;
		break;
	case 3:	// TopLeft
		State=SZB_TOPLEFTALIGN;
		break;
	}
	//HRESULT hr=
	::DrawThemeBackground(fTheme,DC,SBP_SIZEBOX,State,&Rect,nullptr);
}
//---------------------------------------------------------------------------
