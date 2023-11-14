/*- cnSystem - User Interface ---------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2008-09-21                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnSystem_cnUI_H__
#define	__cnLibrary_cnSystem_cnUI_H__
/*-------------------------------------------------------------------------*/
#include <cnSystem/cnFundamental.h>
#include <cnSystem/cnThread.h>
/*-------------------------------------------------------------------------*/
#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
cnLib_INTENUM_BEGIN(ufInt8,Direction)
{
	Left	=0,
	Right	=1,
	Up		=2,
	Down	=3,
}cnLib_INTENUM_END(Direction);
//---------------------------------------------------------------------------
cnLib_ENUM_BEGIN(ufInt8,MouseButton)
{
	None,
	Left,
	Right,
	Middle,
}cnLib_ENUM_END(MouseButton);
//---------------------------------------------------------------------------
cnLib_ENUM_BEGIN(ufInt16,KeyCode)
{
	Cancel		=0x03,
	// 05-07	undefinded
	Backspace	=0x08,
	Tab			=0x09,
	// 0A-0B	undefinded
	// 0C		Clear
	Enter		=0x0D,
	// 0E-0F	undefinded
	Shift		=0x10,
	Ctrl		=0x11,
	Alt			=0x12,
	Pause		=0x13,
	CapsLock	=0x14,
	// 15-19	resversed
	// 1A		undefinded
	Esc			=0x1B,
	// 1C-1F	resversed
	Space		=0x20,
	PageUp		=0x21,
	PageDown	=0x22,
	End			=0x23,
	Home		=0x24,
	Left		=0x25,
	Up			=0x26,
	Right		=0x27,
	Down		=0x28,
	// 29		Select
	// 2A		OEM
	// 2B		Execute
	PrintScreen	=0x2C,
	Insert		=0x2D,
	Delete		=0x2E,
	// 2F		Help
	k0			=0x30,
	k1			=0x31,
	k2			=0x32,
	k3			=0x33,
	k4			=0x34,
	k5			=0x35,
	k6			=0x36,
	k7			=0x37,
	k8			=0x38,
	k9			=0x39,
	// 3A-40	undefinded
	kA			=0x41,
	kB			=0x42,
	kC			=0x43,
	kD			=0x44,
	kE			=0x45,
	kF			=0x46,
	kG			=0x47,
	kH			=0x48,
	kI			=0x49,
	kJ			=0x4A,
	kK			=0x4B,
	kL			=0x4C,
	kM			=0x4D,
	kN			=0x4E,
	kO			=0x4F,
	kP			=0x50,
	kQ			=0x51,
	kR			=0x52,
	kS			=0x53,
	kT			=0x54,
	kU			=0x55,
	kV			=0x56,
	kW			=0x57,
	kX			=0x58,
	kY			=0x59,
	kZ			=0x5A,
	// 5B		windows left
	// 5C		windows right
	// 5D		windows application
	// 5E-5F	undefinded
	NP_0		=0x60,
	NP_1		=0x61,                                                                                                                                                                                                                                               
	NP_2		=0x62,
	NP_3		=0x63,
	NP_4		=0x64,
	NP_5		=0x65,
	NP_6		=0x66,
	NP_7		=0x67,
	NP_8		=0x68,
	NP_9		=0x69,
	Mul			=0x6A,	// *
	Add			=0x6B,	// +
	Separator	=0x6C,	//	.
	Sub			=0x6D,	// -
	// 6E	decimal
	Div			=0x6F,

	F1			=0x70,
	F2			=0x71,
	F3			=0x72,
	F4			=0x73,
	F5			=0x74,
	F6			=0x75,
	F7			=0x76,
	F8			=0x77,
	F9			=0x78,
	F10			=0x79,
	F11			=0x7A,
	F12			=0x7B,
	F13			=0x7C,
	F14			=0x7D,
	F15			=0x7E,
	F16			=0x7F,
	F17			=0x80,
	F18			=0x81,
	F19			=0x82,
	F20			=0x83,
	F21			=0x84,
	F22			=0x85,
	F23			=0x86,
	F24			=0x87,
	// 88-8F	undefinded
	NumLock		=0x90,
	ScrollLock	=0x91,
	// 92-9F	undefinded
	ShiftLeft	=0xA0,
	ShiftRight	=0xA1,
	CtrlLeft	=0xA2,
	CtrlRight	=0xA3,
	AltLeft		=0xA4,
	AltRight	=0xA5,
	// A6-B9	undefinded
	SemiColon	=0xBA,	// ;:
	Equal		=0xBB,	// =+
	Comma		=0xBC,	// ,<
	Minus		=0xBD,	// -_
	Period		=0xBE,	// .>
	Slash 		=0xBF,	// /?
	Tilde 		=0xC0,	// `~
	SqureLeft	=0xDB,	// [{
	BSlash		=0xDC,	// \|
	SqureRight	=0xDD,	// ]}
	Quotes		=0xDE,	// '"
}cnLib_ENUM_END(KeyCode);
//---------------------------------------------------------------------------
struct cUIPoint
{
	Float32 x;
	Float32 y;
};
//---------------------------------------------------------------------------
struct cUIRect
{
	cUIPoint Pos;
	cUIPoint Size;
};
//---------------------------------------------------------------------------
struct cUIRectangle
{
	Float32 Left;
	Float32 Top;
	Float32 Right;
	Float32 Bottom;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iUIMouse : public iInterface
{
public:
	virtual bool cnLib_FUNC GetPosition(iInterface *Relative,cUIPoint &Position)=0;
	virtual bool cnLib_FUNC EventIsButtonDown(eMouseButton Button)=0;
	virtual bool cnLib_FUNC IsButtonDown(eMouseButton Button)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iUIKeyboard : public iInterface
{
public:
	virtual bool cnLib_FUNC EventIsKeyDown(eKeyCode KeyCode)=0;
	virtual bool cnLib_FUNC EventIsKeyToggled(eKeyCode KeyCode)=0;
	virtual bool cnLib_FUNC IsKeyDown(eKeyCode KeyCode)=0;
	virtual bool cnLib_FUNC IsKeyToggled(eKeyCode KeyCode)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iUIThread : public iInterface
{
public:
	virtual bool cnLib_FUNC IsCurrentThread(void)=0;
	virtual iDispatch* cnLib_FUNC GetDispatch(bool HighPriority)=0;
	virtual iUIKeyboard* cnLib_FUNC GetDefaultKeyboard(void)=0;
	virtual iUIMouse* cnLib_FUNC GetDefaultMouse(void)=0;
};

//---------------------------------------------------------------------------
class cnLib_INTERFACE iUIArea : public iInterface
{
public:
	// GetUIThread
	//	get ui thread of the ui element
	virtual iUIThread* cnLib_FUNC GetUIThread(void)=0;

	virtual Float32 cnLib_FUNC GetContentScale(void)=0;
	virtual cUIPoint cnLib_FUNC GetSize(void)=0;
	virtual bool cnLib_FUNC SetSize(cUIPoint Size)=0;

	// TranslatePoint
	//	translate point from this area to relative area
	// [in]Relative		relative area. nullptr for relative to screen
	// [in]Position		poition relative to this area
	// [out]Position	poition relative to Relative
	virtual bool cnLib_FUNC TranslatePointTo(iInterface *Relative,cUIPoint &Position)=0;

	// MoveTo
	//	change position of the ui area
	// [in]Relative		relative area. nullptr for relative to screen
	// [in]Position		lefttop position of the area relative to lefttop of relative area
	virtual bool cnLib_FUNC MoveTo(iInterface *Relative,cUIPoint Position)=0;
	
	// ArrangeRectangle
	//	set rectangle of the ui area
	// [in]Relative		relative area. nullptr for relative to screen
	// [in]Position		lefttop position of the area relative to lefttop of relative area
	virtual bool cnLib_FUNC ArrangeRectangle(iInterface *Relative,cUIPoint Position,cUIPoint Size)=0;

	bool GetPosition(iInterface *Relative,cUIPoint &Position){
		Position.x=0;
		Position.y=0;
		return TranslatePointTo(Relative,Position);
	}
	bool SetPosition(iInterface *Relative,cUIPoint Position){
		return MoveTo(Relative,Position);
	}


	virtual Float32 cnLib_FUNC GetZPosition(void)=0;
	virtual bool cnLib_FUNC SetZPosition(Float32 Position)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iUIScreenHandler
{
public:
	virtual void cnLib_FUNC ContentScaleChanged(void)=0;
	virtual void cnLib_FUNC SizeChanged(void)=0;
	virtual void cnLib_FUNC VisibleChanged(void)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iUIScreen : public iUIArea
{
public:
	cnLib_INTERFACE_DEFINE(iUIScreen,iUIArea)
	
	virtual bool cnLib_FUNC InsertScreenHandler(iUIScreenHandler *Handler,sfInt16 Order=0)=0;
	virtual bool cnLib_FUNC RemoveScreenHandler(iUIScreenHandler *Handler)=0;

	virtual bool cnLib_FUNC IsVisible(void)=0;
};
//---------------------------------------------------------------------------
cnLib_ENUM_BEGIN(ufInt8,UIState)
{
	Null,			// ui not started
	Background,		// ui is in background. invisible or non operatable
	Inactive,		// ui is visible but inactive
	Active,			// ui is visible and active
}cnLib_ENUM_END(UIState);
//---------------------------------------------------------------------------
class cnLib_INTERFACE iUIStateHandler
{
public:
	// UIStarted
	//	NoHost => Background
	//	called when view has entered a host
	virtual void cnLib_FUNC UIStarted(void)=0;
	// UIShow
	//	Background => Inactive
	//	called when view become visible
	virtual void cnLib_FUNC UIShow(void)=0;
	// UIResume
	//	Inactive => Active
	//	called when view become active
	virtual void cnLib_FUNC UIResume(void)=0;
	// UIPaused
	//	Inactive <= Active
	//	called when layout is inactive
	virtual void cnLib_FUNC UIPaused(void)=0;
	// UIHide
	//	Background <= Inactive
	//	called when layout become invisible
	virtual void cnLib_FUNC UIHide(void)=0;
	// UIStopped
	//	NoHost <= Background
	//	called when layout has leaved a host
	virtual void cnLib_FUNC UIStopped(void)=0;

};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iUIViewHandler
{
public:
	virtual void cnLib_FUNC UILayout(void)=0;
	virtual cUIRectangle cnLib_FUNC UIMargin(const cUIRectangle &Margin)=0;
	virtual void cnLib_FUNC ContentScaleChanged(void)=0;
	virtual void cnLib_FUNC RectangleChanged(bool Moved,bool Sized)=0;
	virtual void cnLib_FUNC ZPositionChanged(void)=0;
	virtual void cnLib_FUNC VisibleChanged(void)=0;
	virtual void cnLib_FUNC EnableChanged(void)=0;
	virtual void cnLib_FUNC WindowChanged(void)=0;
	virtual void cnLib_FUNC ParentChanged(void)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iUIView : public iUIArea
{
public:
	cnLib_INTERFACE_DEFINE(iUIView,iUIArea)
	
	virtual eUIState cnLib_FUNC GetUIState(void)=0;
	virtual bool cnLib_FUNC InsertStateHandler(iUIStateHandler *Handler,sfInt16 Order=0)=0;
	virtual bool cnLib_FUNC RemoveStateHandler(iUIStateHandler *Handler)=0;

	virtual bool cnLib_FUNC GetVisible(void)=0;
	virtual bool cnLib_FUNC SetVisible(bool Visible)=0;

	virtual bool cnLib_FUNC IsEnabled(void)=0;
	virtual bool cnLib_FUNC GetEnable(void)=0;
	virtual bool cnLib_FUNC SetEnable(bool Enable)=0;

	virtual bool cnLib_FUNC InsertViewHandler(iUIViewHandler *Handler,sfInt16 Order=0)=0;
	virtual bool cnLib_FUNC RemoveViewHandler(iUIViewHandler *Handler)=0;

	virtual void cnLib_FUNC ArrangeLayout(void)=0;
	virtual void cnLib_FUNC SetArrangement(void)=0;

	virtual void cnLib_FUNC SetIncludeParentFrameMargin(bool Include)=0;
	virtual cUIRectangle cnLib_FUNC GetFrameMargin(void)=0;

	// GetWindow
	//	Get operating window for this view
	virtual iUIArea* cnLib_FUNC GetWindow(void)=0;

	// GetParent
	//	get parent of the view
	virtual iUIView* cnLib_FUNC GetParent(void)=0;


	virtual bool cnLib_FUNC InsertView(iUIView *View)=0;
	virtual bool cnLib_FUNC RemoveView(iUIView *View)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iUIWindowHandler
{
public:
	virtual void cnLib_FUNC ScreenChanged(void)=0;
	virtual void cnLib_FUNC ParentChanged(void)=0;
	virtual void cnLib_FUNC ContentScaleChanged(void)=0;
	virtual void cnLib_FUNC RectangleChanged(bool Moved,bool Sized)=0;
	virtual void cnLib_FUNC ZPositionChanged(void)=0;
	virtual void cnLib_FUNC VisibleChanged(void)=0;
	virtual void cnLib_FUNC EnableChanged(void)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iUIWindow : public iUIArea
{
public:
	cnLib_INTERFACE_DEFINE(iUIWindow,iUIArea)
		
	virtual eUIState cnLib_FUNC GetUIState(void)=0;
	virtual bool cnLib_FUNC InsertStateHandler(iUIStateHandler *Handler,sfInt16 Order=0)=0;
	virtual bool cnLib_FUNC RemoveStateHandler(iUIStateHandler *Handler)=0;

	virtual bool cnLib_FUNC GetVisible(void)=0;
	virtual bool cnLib_FUNC SetVisible(bool Visible)=0;

	virtual bool cnLib_FUNC IsEnabled(void)=0;
	virtual bool cnLib_FUNC GetEnable(void)=0;
	virtual bool cnLib_FUNC SetEnable(bool Enable)=0;

	virtual iUIScreen* cnLib_FUNC GetScreen(void)=0;

	// UI Tree

	virtual iUIWindow* cnLib_FUNC GetParent(void)=0;

	virtual bool cnLib_FUNC InsertWindowHandler(iUIWindowHandler *Handler,sfInt16 Order=0)=0;
	virtual bool cnLib_FUNC RemoveWindowHandler(iUIWindowHandler *Handler)=0;

	// Client

	virtual iUIView* cnLib_FUNC GetClient(void)=0;
	virtual bool cnLib_FUNC SetClient(iUIView *View)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iPrintPaperHandler
{
public:
	virtual void cnLib_FUNC ContentScaleChanged(void)=0;
	virtual void cnLib_FUNC RectangleChanged(bool Moved,bool Sized)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iPrintPaper : public iUIArea
{
public:
	cnLib_INTERFACE_DEFINE(iPrintPaper,iUIArea)
		
	virtual bool cnLib_FUNC InsertPrintPaperHandler(iPrintPaperHandler *Handler,sfInt16 Order=0)=0;
	virtual bool cnLib_FUNC RemovePrintPaperHandler(iPrintPaperHandler *Handler)=0;

	// Client

	virtual iUIView* cnLib_FUNC GetClient(void)=0;
	virtual bool cnLib_FUNC SetClient(iUIView *Area)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iUITextInputHandler
{
public:
	virtual void cnLib_FUNC TextInput(const uChar16 *Text,uIntn Length)=0;

	virtual void cnLib_FUNC TextCompositionStarted(void)=0;
	virtual void cnLib_FUNC TextCompositionFinished(void)=0;
	virtual void cnLib_FUNC TextCompositionUpdate(sfInt32 CaretPos,const uChar16 *Text,uIntn TextLength)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iUITextCompositionViewInfo
{
public:
	virtual void cnLib_FUNC TextCompositionViewRect(cUIPoint &Position,cUIPoint &Size,Float32 &LineHeight)=0;
	virtual cUIPoint cnLib_FUNC TextCompositionCaretPosition(uIntn CompositionTextIndex)=0;
};
//---------------------------------------------------------------------------
cnLib_ENUM_BEGIN(ufInt8,UIEventSourceType)
{
	Unknow,
	Keyboard,
	Mouse,
	Touch,
}cnLib_ENUM_END(UIEventSourceType);
//---------------------------------------------------------------------------
class cnLib_INTERFACE iUIInputEvent
{
public:
	virtual eUIEventSourceType cnLib_FUNC GetSourceType(void)=0;
	virtual iUIView* cnLib_FUNC GetSourceView(void)=0;
	virtual void cnLib_FUNC CancelEvent(void)=0;
	virtual bool cnLib_FUNC IsCancelled(void)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iUIKeyEvent : public iUIInputEvent
{
public:
	virtual iUIKeyboard* cnLib_FUNC GetKeyboard(void)=0;
	virtual bool cnLib_FUNC IsKeyDown(eKeyCode KeyCode)=0;
	virtual bool cnLib_FUNC IsKeyToggled(eKeyCode KeyCode)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iUIKeyHandler
{
public:
	virtual void cnLib_FUNC KeyFocusEnterRange(iUIKeyEvent *KeyEvent)=0;
	virtual void cnLib_FUNC KeyFocusLeaveRange(iUIKeyEvent *KeyEvent)=0;
	virtual void cnLib_FUNC KeyFocusEnter(iUIKeyEvent *KeyEvent)=0;
	virtual void cnLib_FUNC KeyFocusLeave(iUIKeyEvent *KeyEvent)=0;
	virtual void cnLib_FUNC KeyDown(iUIKeyEvent *KeyEvent,eKeyCode KeyCode)=0;
	virtual void cnLib_FUNC KeyUp(iUIKeyEvent *KeyEvent,eKeyCode KeyCode)=0;
	virtual void cnLib_FUNC KeyInput(iUIKeyEvent *KeyEvent,eKeyCode KeyCode)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iUIKeyControl : public iInterface
{
public:
	cnLib_INTERFACE_DEFINE(iUIKeyControl,iInterface)

	virtual iUIView* cnLib_FUNC GetFocus(void)=0;
	virtual void cnLib_FUNC SetFocus(bool ActivateWindow)=0;

	virtual bool cnLib_FUNC InsertKeyHandler(iUIKeyHandler *Handler,sfInt16 Order=0)=0;
	virtual bool cnLib_FUNC RemoveKeyHandler(iUIKeyHandler *Handler)=0;

	virtual bool cnLib_FUNC InsertKeyFilter(iUIKeyHandler *Filter,sfInt16 Order=0)=0;
	virtual bool cnLib_FUNC RemoveKeyFilter(iUIKeyHandler *Filter)=0;

	virtual bool cnLib_FUNC KeyAcquireExclusive(iUIKeyHandler *Handler)=0;
	virtual bool cnLib_FUNC KeyReleaseExclusive(iUIKeyHandler *Handler)=0;

	virtual bool cnLib_FUNC InsertTextInputHandler(iUITextInputHandler *Handler,sfInt16 Order=0)=0;
	virtual bool cnLib_FUNC RemoveTextInputHandler(iUITextInputHandler *Handler)=0;
	virtual bool cnLib_FUNC SetCompositionViewInfo(iUITextCompositionViewInfo *CompositionViewInfo)=0;

};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iUIMouseEvent : public iUIInputEvent
{
public:
	virtual iUIMouse* cnLib_FUNC GetMouse(void)=0;
	virtual bool cnLib_FUNC GetPosition(iInterface *Relative,cUIPoint &Position)=0;
	virtual bool cnLib_FUNC IsButtonDown(eMouseButton Button)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iUIMouseHandler
{
public:
	virtual void cnLib_FUNC MouseEnterRange(iUIMouseEvent *MouseEvent)=0;
	virtual void cnLib_FUNC MouseLeaveRange(iUIMouseEvent *MouseEvent)=0;
	virtual void cnLib_FUNC MouseEnter(iUIMouseEvent *MouseEvent)=0;
	virtual void cnLib_FUNC MouseLeave(iUIMouseEvent *MouseEvent)=0;
	virtual iInterface* cnLib_FUNC MouseGetCursor(iUIMouseEvent *MouseEvent)=0;
	virtual void cnLib_FUNC MouseMove(iUIMouseEvent *MouseEvent)=0;
	virtual void cnLib_FUNC MouseDown(iUIMouseEvent *MouseEvent,eMouseButton Button)=0;
	virtual void cnLib_FUNC MouseUp(iUIMouseEvent *MouseEvent,eMouseButton Button)=0;
	virtual void cnLib_FUNC MouseWheel(iUIMouseEvent *MouseEvent,Float32 ScrollX,Float32 ScrollY)=0;
};
//---------------------------------------------------------------------------
cnLib_ENUM_BEGIN(ufInt8,SysMouseCursor)
{
	Normal,
	Busy,
	BackgroundBusy,
	TextEdit,
	SizeA,
	SizeV,
	SizeH,
	SizeFS,
	SizeBS,
}cnLib_ENUM_END(SysMouseCursor);
//---------------------------------------------------------------------------
class cnLib_INTERFACE iUIMouseControl : public iInterface
{
public:
	cnLib_INTERFACE_DEFINE(iUIMouseControl,iInterface)

	virtual void cnLib_FUNC MouseUpdateCursor(void)=0;

	virtual bool cnLib_FUNC InsertMouseHandler(iUIMouseHandler *Handler,sfInt16 Order=0)=0;
	virtual bool cnLib_FUNC RemoveMouseHandler(iUIMouseHandler *Handler)=0;

	virtual bool cnLib_FUNC InsertMouseFilter(iUIMouseHandler *Filter,sfInt16 Order=0)=0;
	virtual bool cnLib_FUNC RemoveMouseFilter(iUIMouseHandler *Filter)=0;

	virtual bool cnLib_FUNC MouseSetCaptrue(void)=0;
	virtual void cnLib_FUNC MouseReleaseCaptrue(void)=0;
	virtual bool cnLib_FUNC MouseIsCaptured(void)=0;
	virtual bool cnLib_FUNC MouseAcquireExclusive(iUIMouseHandler *Handler)=0;
	virtual bool cnLib_FUNC MouseReleaseExclusive(iUIMouseHandler *Handler)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iUITouchEvent : public iUIInputEvent
{
public:
	virtual void* cnLib_FUNC GetTouchID(void)=0;
	virtual bool cnLib_FUNC GetPosition(iInterface *Relative,cUIPoint &Position)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iUITouchHandler
{
public:
	virtual void cnLib_FUNC TouchBegin(iUITouchEvent *TouchEvent)=0;
	virtual void cnLib_FUNC TouchEnd(iUITouchEvent *TouchEvent)=0;
	virtual void cnLib_FUNC TouchLost(iUITouchEvent *TouchEvent)=0;
	virtual void cnLib_FUNC TouchMove(iUITouchEvent *TouchEvent)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iUITouchControl : public iInterface
{
public:
	cnLib_INTERFACE_DEFINE(iUITouchControl,iInterface)

	virtual bool cnLib_FUNC InsertTouchHandler(iUITouchHandler *Handler,sfInt16 Order=0)=0;
	virtual bool cnLib_FUNC RemoveTouchHandler(iUITouchHandler *Handler)=0;

	virtual bool cnLib_FUNC InsertTouchFilter(iUITouchHandler *Filter,sfInt16 Order=0)=0;
	virtual bool cnLib_FUNC RemoveTouchFilter(iUITouchHandler *Filter)=0;

	virtual bool cnLib_FUNC TouchAcquireExclusive(iUITouchHandler *Handler)=0;
	virtual bool cnLib_FUNC TouchReleaseExclusive(iUITouchHandler *Handler)=0;
};
//---------------------------------------------------------------------------
class iPopupWindowControlCallback
{
public:
	virtual void cnLib_FUNC PopupOnShow(void)=0;
	virtual void cnLib_FUNC PopupOnHide(void)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iPopupWindowControl : public iReference
{
public:
	virtual void cnLib_FUNC SetCallback(iPopupWindowControlCallback *Callback)=0;
	virtual iUIWindow* cnLib_FUNC GetUIWindow(void)=0;
	virtual bool cnLib_FUNC SetupOwner(iUIView *View)=0;
	virtual void cnLib_FUNC ShowPopup(void)=0;
	virtual void cnLib_FUNC HidePopup(void)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iClipboardReader : public iReference
{
public:
	virtual iReadBuffer<uChar16>* cnLib_FUNC QueryText(void)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iClipboardWriter : public iReference
{
public:
	virtual bool cnLib_FUNC Submit(void)=0;
	virtual bool cnLib_FUNC SetText(const uChar16 *Text,uIntn TextLength)=0;
	virtual iWriteBuffer<uChar16>* cnLib_FUNC QueryText(void)=0;
};
//---------------------------------------------------------------------------
namespace cnSystem{
//---------------------------------------------------------------------------
namespace CurrentUIThread{
//---------------------------------------------------------------------------
iUIThread* cnLib_FUNC GetUIThread(void);
iDispatch* cnLib_FUNC GetDispatch(bool HighPriority);
iUIKeyboard* cnLib_FUNC GetDefaultKeyboard(void);
iUIMouse* cnLib_FUNC GetDefaultMouse(void);
//---------------------------------------------------------------------------
}	// namespace CurrentUIThread
//---------------------------------------------------------------------------
iPtr<iUIView>	cnLib_FUNC CreateUIView(void);

rPtr<iPopupWindowControl>	cnLib_FUNC CreatePopupWindowControl(void);

iInterface* cnLib_FUNC GetSysMouseCursor(eSysMouseCursor CursorID);
rPtr<iClipboardReader> cnLib_FUNC QueryClipboardReader(iUIView *View);
rPtr<iClipboardWriter> cnLib_FUNC QueryClipboardWriter(iUIView *View);
//---------------------------------------------------------------------------
}   // namespace cnSystem
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif
