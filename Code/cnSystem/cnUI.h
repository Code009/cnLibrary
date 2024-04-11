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
#ifdef	cnLibrary_CPPEXCLUDE_VIRTUAL_OVERRIDE
#define	override
#endif
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
	virtual bool cnLib_FUNC GetPosition(iInterface *Relative,cUIPoint &Position)noexcept(true)=0;
	virtual bool cnLib_FUNC EventIsButtonDown(eMouseButton Button)noexcept(true)=0;
	virtual bool cnLib_FUNC IsButtonDown(eMouseButton Button)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iUIKeyboard : public iInterface
{
public:
	virtual bool cnLib_FUNC EventIsKeyDown(eKeyCode KeyCode)noexcept(true)=0;
	virtual bool cnLib_FUNC EventIsKeyToggled(eKeyCode KeyCode)noexcept(true)=0;
	virtual bool cnLib_FUNC IsKeyDown(eKeyCode KeyCode)noexcept(true)=0;
	virtual bool cnLib_FUNC IsKeyToggled(eKeyCode KeyCode)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iUIThread : public iInterface
{
public:
	virtual bool cnLib_FUNC IsCurrentThread(void)noexcept(true)=0;
	virtual iDispatch* cnLib_FUNC GetDispatch(bool HighPriority)noexcept(true)=0;
	virtual iUIKeyboard* cnLib_FUNC GetDefaultKeyboard(void)noexcept(true)=0;
	virtual iUIMouse* cnLib_FUNC GetDefaultMouse(void)noexcept(true)=0;
};

//---------------------------------------------------------------------------
class cnLib_INTERFACE iUIArea : public iInterface
{
public:
	// GetUIThread
	//	get ui thread of the ui element
	virtual iUIThread* cnLib_FUNC GetUIThread(void)noexcept(true)=0;

	virtual Float32 cnLib_FUNC GetContentScale(void)noexcept(true)=0;
	virtual cUIPoint cnLib_FUNC GetSize(void)noexcept(true)=0;
	virtual bool cnLib_FUNC SetSize(cUIPoint Size)noexcept(true)=0;

	// TranslatePoint
	//	translate point from this area to relative area
	// [in]Relative		relative area. nullptr for relative to screen
	// [in]Position		poition relative to this area
	// [out]Position	poition relative to Relative
	virtual bool cnLib_FUNC TranslatePointTo(iInterface *Relative,cUIPoint &Position)noexcept(true)=0;

	// MoveTo
	//	change position of the ui area
	// [in]Relative		relative area. nullptr for relative to screen
	// [in]Position		lefttop position of the area relative to lefttop of relative area
	virtual bool cnLib_FUNC MoveTo(iInterface *Relative,cUIPoint Position)noexcept(true)=0;
	
	// ArrangeRectangle
	//	set rectangle of the ui area
	// [in]Relative		relative area. nullptr for relative to screen
	// [in]Position		lefttop position of the area relative to lefttop of relative area
	virtual bool cnLib_FUNC ArrangeRectangle(iInterface *Relative,cUIPoint Position,cUIPoint Size)noexcept(true)=0;

	bool GetPosition(iInterface *Relative,cUIPoint &Position)noexcept(true){
		Position.x=0;
		Position.y=0;
		return TranslatePointTo(Relative,Position);
	}
	bool SetPosition(iInterface *Relative,cUIPoint Position)noexcept(true){
		return MoveTo(Relative,Position);
	}


	virtual Float32 cnLib_FUNC GetZPosition(void)noexcept(true)=0;
	virtual bool cnLib_FUNC SetZPosition(Float32 Position)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iUIScreenHandler
{
public:
	virtual void cnLib_FUNC ContentScaleChanged(void)noexcept(true)=0;
	virtual void cnLib_FUNC SizeChanged(void)noexcept(true)=0;
	virtual void cnLib_FUNC VisibleChanged(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iUIScreen : public iUIArea
{
public:
	struct tInterfaceID{	static iTypeID Value;	};
	virtual void* cnLib_FUNC CastInterface(iTypeID ID)noexcept(true) override{		return cnVar::ImplementCastInterface(this,ID);	}
	
	virtual bool cnLib_FUNC InsertScreenHandler(iUIScreenHandler *Handler,sfInt16 Order=0)noexcept(true)=0;
	virtual bool cnLib_FUNC RemoveScreenHandler(iUIScreenHandler *Handler)noexcept(true)=0;

	virtual bool cnLib_FUNC IsVisible(void)noexcept(true)=0;
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
	virtual void cnLib_FUNC UIStarted(void)noexcept(true)=0;
	// UIShow
	//	Background => Inactive
	//	called when view become visible
	virtual void cnLib_FUNC UIShow(void)noexcept(true)=0;
	// UIResume
	//	Inactive => Active
	//	called when view become active
	virtual void cnLib_FUNC UIResume(void)noexcept(true)=0;
	// UIPaused
	//	Inactive <= Active
	//	called when layout is inactive
	virtual void cnLib_FUNC UIPaused(void)noexcept(true)=0;
	// UIHide
	//	Background <= Inactive
	//	called when layout become invisible
	virtual void cnLib_FUNC UIHide(void)noexcept(true)=0;
	// UIStopped
	//	NoHost <= Background
	//	called when layout has leaved a host
	virtual void cnLib_FUNC UIStopped(void)noexcept(true)=0;

};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iUIViewHandler
{
public:
	virtual void cnLib_FUNC UILayout(void)noexcept(true)=0;
	virtual cUIRectangle cnLib_FUNC UIMargin(const cUIRectangle &Margin)noexcept(true)=0;
	virtual void cnLib_FUNC ContentScaleChanged(void)noexcept(true)=0;
	virtual void cnLib_FUNC RectangleChanged(bool Moved,bool Sized)noexcept(true)=0;
	virtual void cnLib_FUNC ZPositionChanged(void)noexcept(true)=0;
	virtual void cnLib_FUNC VisibleChanged(void)noexcept(true)=0;
	virtual void cnLib_FUNC EnableChanged(void)noexcept(true)=0;
	virtual void cnLib_FUNC WindowChanged(void)noexcept(true)=0;
	virtual void cnLib_FUNC ParentChanged(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iUIView : public iUIArea
{
public:
	struct tInterfaceID{	static iTypeID Value;	};
	virtual void* cnLib_FUNC CastInterface(iTypeID ID)noexcept(true) override{		return cnVar::ImplementCastInterface(this,ID);	}
	
	virtual eUIState cnLib_FUNC GetUIState(void)noexcept(true)=0;
	virtual bool cnLib_FUNC InsertStateHandler(iUIStateHandler *Handler,sfInt16 Order=0)noexcept(true)=0;
	virtual bool cnLib_FUNC RemoveStateHandler(iUIStateHandler *Handler)noexcept(true)=0;

	virtual bool cnLib_FUNC GetVisible(void)noexcept(true)=0;
	virtual bool cnLib_FUNC SetVisible(bool Visible)noexcept(true)=0;

	virtual bool cnLib_FUNC IsEnabled(void)noexcept(true)=0;
	virtual bool cnLib_FUNC GetEnable(void)noexcept(true)=0;
	virtual bool cnLib_FUNC SetEnable(bool Enable)noexcept(true)=0;

	virtual bool cnLib_FUNC InsertViewHandler(iUIViewHandler *Handler,sfInt16 Order=0)noexcept(true)=0;
	virtual bool cnLib_FUNC RemoveViewHandler(iUIViewHandler *Handler)noexcept(true)=0;

	virtual void cnLib_FUNC ArrangeLayout(void)noexcept(true)=0;
	virtual void cnLib_FUNC SetArrangement(void)noexcept(true)=0;

	virtual void cnLib_FUNC SetIncludeParentFrameMargin(bool Include)noexcept(true)=0;
	virtual cUIRectangle cnLib_FUNC GetFrameMargin(void)noexcept(true)=0;

	// GetWindow
	//	Get operating window for this view
	virtual iUIArea* cnLib_FUNC GetWindow(void)noexcept(true)=0;

	// GetParent
	//	get parent of the view
	virtual iUIView* cnLib_FUNC GetParent(void)noexcept(true)=0;


	virtual bool cnLib_FUNC InsertView(iUIView *View)noexcept(true)=0;
	virtual bool cnLib_FUNC RemoveView(iUIView *View)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iUIWindowHandler
{
public:
	virtual void cnLib_FUNC ScreenChanged(void)noexcept(true)=0;
	virtual void cnLib_FUNC ParentChanged(void)noexcept(true)=0;
	virtual void cnLib_FUNC ContentScaleChanged(void)noexcept(true)=0;
	virtual void cnLib_FUNC RectangleChanged(bool Moved,bool Sized)noexcept(true)=0;
	virtual void cnLib_FUNC ZPositionChanged(void)noexcept(true)=0;
	virtual void cnLib_FUNC VisibleChanged(void)noexcept(true)=0;
	virtual void cnLib_FUNC EnableChanged(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iUIWindow : public iUIArea
{
public:
	struct tInterfaceID{	static iTypeID Value;	};
	virtual void* cnLib_FUNC CastInterface(iTypeID ID)noexcept(true) override{		return cnVar::ImplementCastInterface(this,ID);	}
		
	virtual eUIState cnLib_FUNC GetUIState(void)noexcept(true)=0;
	virtual bool cnLib_FUNC InsertStateHandler(iUIStateHandler *Handler,sfInt16 Order=0)noexcept(true)=0;
	virtual bool cnLib_FUNC RemoveStateHandler(iUIStateHandler *Handler)noexcept(true)=0;

	virtual bool cnLib_FUNC GetVisible(void)noexcept(true)=0;
	virtual bool cnLib_FUNC SetVisible(bool Visible)noexcept(true)=0;

	virtual bool cnLib_FUNC IsEnabled(void)noexcept(true)=0;
	virtual bool cnLib_FUNC GetEnable(void)noexcept(true)=0;
	virtual bool cnLib_FUNC SetEnable(bool Enable)noexcept(true)=0;

	virtual iUIScreen* cnLib_FUNC GetScreen(void)noexcept(true)=0;

	// UI Tree

	virtual iUIWindow* cnLib_FUNC GetParent(void)noexcept(true)=0;

	virtual bool cnLib_FUNC InsertWindowHandler(iUIWindowHandler *Handler,sfInt16 Order=0)noexcept(true)=0;
	virtual bool cnLib_FUNC RemoveWindowHandler(iUIWindowHandler *Handler)noexcept(true)=0;

	// Client

	virtual iUIView* cnLib_FUNC GetClient(void)noexcept(true)=0;
	virtual bool cnLib_FUNC SetClient(iUIView *View)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iPrintPaperHandler
{
public:
	virtual void cnLib_FUNC ContentScaleChanged(void)noexcept(true)=0;
	virtual void cnLib_FUNC RectangleChanged(bool Moved,bool Sized)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iPrintPaper : public iUIArea
{
public:
	struct tInterfaceID{	static iTypeID Value;	};
	virtual void* cnLib_FUNC CastInterface(iTypeID ID)noexcept(true) override{		return cnVar::ImplementCastInterface(this,ID);	}
		
	virtual bool cnLib_FUNC InsertPrintPaperHandler(iPrintPaperHandler *Handler,sfInt16 Order=0)noexcept(true)=0;
	virtual bool cnLib_FUNC RemovePrintPaperHandler(iPrintPaperHandler *Handler)noexcept(true)=0;

	// Client

	virtual iUIView* cnLib_FUNC GetClient(void)noexcept(true)=0;
	virtual bool cnLib_FUNC SetClient(iUIView *Area)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iUITextInputHandler
{
public:
	virtual void cnLib_FUNC TextInput(const uChar16 *Text,uIntn Length)noexcept(true)=0;

	virtual void cnLib_FUNC TextCompositionStarted(void)noexcept(true)=0;
	virtual void cnLib_FUNC TextCompositionFinished(void)noexcept(true)=0;
	virtual void cnLib_FUNC TextCompositionUpdate(sfInt32 CaretPos,const uChar16 *Text,uIntn TextLength)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iUITextCompositionViewInfo
{
public:
	virtual void cnLib_FUNC TextCompositionViewRect(cUIPoint &Position,cUIPoint &Size,Float32 &LineHeight)noexcept(true)=0;
	virtual cUIPoint cnLib_FUNC TextCompositionCaretPosition(uIntn CompositionTextIndex)noexcept(true)=0;
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
	virtual eUIEventSourceType cnLib_FUNC GetSourceType(void)noexcept(true)=0;
	virtual iUIView* cnLib_FUNC GetSourceView(void)noexcept(true)=0;
	virtual void cnLib_FUNC CancelEvent(void)noexcept(true)=0;
	virtual bool cnLib_FUNC IsCancelled(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iUIKeyEvent : public iUIInputEvent
{
public:
	virtual iUIKeyboard* cnLib_FUNC GetKeyboard(void)noexcept(true)=0;
	virtual bool cnLib_FUNC IsKeyDown(eKeyCode KeyCode)noexcept(true)=0;
	virtual bool cnLib_FUNC IsKeyToggled(eKeyCode KeyCode)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iUIKeyHandler
{
public:
	virtual void cnLib_FUNC KeyFocusEnterRange(iUIKeyEvent *KeyEvent)noexcept(true)=0;
	virtual void cnLib_FUNC KeyFocusLeaveRange(iUIKeyEvent *KeyEvent)noexcept(true)=0;
	virtual void cnLib_FUNC KeyFocusEnter(iUIKeyEvent *KeyEvent)noexcept(true)=0;
	virtual void cnLib_FUNC KeyFocusLeave(iUIKeyEvent *KeyEvent)noexcept(true)=0;
	virtual void cnLib_FUNC KeyDown(iUIKeyEvent *KeyEvent,eKeyCode KeyCode)noexcept(true)=0;
	virtual void cnLib_FUNC KeyUp(iUIKeyEvent *KeyEvent,eKeyCode KeyCode)noexcept(true)=0;
	virtual void cnLib_FUNC KeyInput(iUIKeyEvent *KeyEvent,eKeyCode KeyCode)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iUIKeyControl : public iInterface
{
public:
	struct tInterfaceID{	static iTypeID Value;	};
	virtual void* cnLib_FUNC CastInterface(iTypeID ID)noexcept(true) override{		return cnVar::ImplementCastInterface(this,ID);	}

	virtual iUIView* cnLib_FUNC GetFocus(void)noexcept(true)=0;
	virtual void cnLib_FUNC SetFocus(bool ActivateWindow)noexcept(true)=0;

	virtual bool cnLib_FUNC InsertKeyHandler(iUIKeyHandler *Handler,sfInt16 Order=0)noexcept(true)=0;
	virtual bool cnLib_FUNC RemoveKeyHandler(iUIKeyHandler *Handler)noexcept(true)=0;

	virtual bool cnLib_FUNC InsertKeyFilter(iUIKeyHandler *Filter,sfInt16 Order=0)noexcept(true)=0;
	virtual bool cnLib_FUNC RemoveKeyFilter(iUIKeyHandler *Filter)noexcept(true)=0;

	virtual bool cnLib_FUNC KeyAcquireExclusive(iUIKeyHandler *Handler)noexcept(true)=0;
	virtual bool cnLib_FUNC KeyReleaseExclusive(iUIKeyHandler *Handler)noexcept(true)=0;

	virtual bool cnLib_FUNC InsertTextInputHandler(iUITextInputHandler *Handler,sfInt16 Order=0)noexcept(true)=0;
	virtual bool cnLib_FUNC RemoveTextInputHandler(iUITextInputHandler *Handler)noexcept(true)=0;
	virtual bool cnLib_FUNC SetCompositionViewInfo(iUITextCompositionViewInfo *CompositionViewInfo)noexcept(true)=0;

};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iUIMouseEvent : public iUIInputEvent
{
public:
	virtual iUIMouse* cnLib_FUNC GetMouse(void)noexcept(true)=0;
	virtual bool cnLib_FUNC GetPosition(iInterface *Relative,cUIPoint &Position)noexcept(true)=0;
	virtual bool cnLib_FUNC IsButtonDown(eMouseButton Button)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iUIMouseHandler
{
public:
	virtual void cnLib_FUNC MouseEnterRange(iUIMouseEvent *MouseEvent)noexcept(true)=0;
	virtual void cnLib_FUNC MouseLeaveRange(iUIMouseEvent *MouseEvent)noexcept(true)=0;
	virtual void cnLib_FUNC MouseEnter(iUIMouseEvent *MouseEvent)noexcept(true)=0;
	virtual void cnLib_FUNC MouseLeave(iUIMouseEvent *MouseEvent)noexcept(true)=0;
	virtual iInterface* cnLib_FUNC MouseGetCursor(iUIMouseEvent *MouseEvent)noexcept(true)=0;
	virtual void cnLib_FUNC MouseMove(iUIMouseEvent *MouseEvent)noexcept(true)=0;
	virtual void cnLib_FUNC MouseDown(iUIMouseEvent *MouseEvent,eMouseButton Button)noexcept(true)=0;
	virtual void cnLib_FUNC MouseUp(iUIMouseEvent *MouseEvent,eMouseButton Button)noexcept(true)=0;
	virtual void cnLib_FUNC MouseWheel(iUIMouseEvent *MouseEvent,Float32 ScrollX,Float32 ScrollY)noexcept(true)=0;
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
	struct tInterfaceID{	static iTypeID Value;	};
	virtual void* cnLib_FUNC CastInterface(iTypeID ID)noexcept(true) override{		return cnVar::ImplementCastInterface(this,ID);	}

	virtual void cnLib_FUNC MouseUpdateCursor(void)noexcept(true)=0;

	virtual bool cnLib_FUNC InsertMouseHandler(iUIMouseHandler *Handler,sfInt16 Order=0)noexcept(true)=0;
	virtual bool cnLib_FUNC RemoveMouseHandler(iUIMouseHandler *Handler)noexcept(true)=0;

	virtual bool cnLib_FUNC InsertMouseFilter(iUIMouseHandler *Filter,sfInt16 Order=0)noexcept(true)=0;
	virtual bool cnLib_FUNC RemoveMouseFilter(iUIMouseHandler *Filter)noexcept(true)=0;

	virtual bool cnLib_FUNC MouseSetCaptrue(void)noexcept(true)=0;
	virtual void cnLib_FUNC MouseReleaseCaptrue(void)noexcept(true)=0;
	virtual bool cnLib_FUNC MouseIsCaptured(void)noexcept(true)=0;
	virtual bool cnLib_FUNC MouseAcquireExclusive(iUIMouseHandler *Handler)noexcept(true)=0;
	virtual bool cnLib_FUNC MouseReleaseExclusive(iUIMouseHandler *Handler)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iUITouchEvent : public iUIInputEvent
{
public:
	virtual void* cnLib_FUNC GetTouchID(void)noexcept(true)=0;
	virtual bool cnLib_FUNC GetPosition(iInterface *Relative,cUIPoint &Position)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iUITouchHandler
{
public:
	virtual void cnLib_FUNC TouchBegin(iUITouchEvent *TouchEvent)noexcept(true)=0;
	virtual void cnLib_FUNC TouchEnd(iUITouchEvent *TouchEvent)noexcept(true)=0;
	virtual void cnLib_FUNC TouchLost(iUITouchEvent *TouchEvent)noexcept(true)=0;
	virtual void cnLib_FUNC TouchMove(iUITouchEvent *TouchEvent)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iUITouchControl : public iInterface
{
public:
	struct tInterfaceID{	static iTypeID Value;	};
	virtual void* cnLib_FUNC CastInterface(iTypeID ID)noexcept(true) override{		return cnVar::ImplementCastInterface(this,ID);	}

	virtual bool cnLib_FUNC InsertTouchHandler(iUITouchHandler *Handler,sfInt16 Order=0)noexcept(true)=0;
	virtual bool cnLib_FUNC RemoveTouchHandler(iUITouchHandler *Handler)noexcept(true)=0;

	virtual bool cnLib_FUNC InsertTouchFilter(iUITouchHandler *Filter,sfInt16 Order=0)noexcept(true)=0;
	virtual bool cnLib_FUNC RemoveTouchFilter(iUITouchHandler *Filter)noexcept(true)=0;

	virtual bool cnLib_FUNC TouchAcquireExclusive(iUITouchHandler *Handler)noexcept(true)=0;
	virtual bool cnLib_FUNC TouchReleaseExclusive(iUITouchHandler *Handler)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class iPopupWindowControlCallback
{
public:
	virtual void cnLib_FUNC PopupOnShow(void)noexcept(true)=0;
	virtual void cnLib_FUNC PopupOnHide(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iPopupWindowControl : public iReference
{
public:
	virtual void cnLib_FUNC SetCallback(iPopupWindowControlCallback *Callback)noexcept(true)=0;
	virtual iUIWindow* cnLib_FUNC GetUIWindow(void)noexcept(true)=0;
	virtual bool cnLib_FUNC SetupOwner(iUIView *View)noexcept(true)=0;
	virtual void cnLib_FUNC ShowPopup(void)noexcept(true)=0;
	virtual void cnLib_FUNC HidePopup(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iClipboardReader : public iReference
{
public:
	virtual iReadBuffer<uChar16>* cnLib_FUNC QueryText(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iClipboardWriter : public iReference
{
public:
	virtual bool cnLib_FUNC Submit(void)noexcept(true)=0;
	virtual bool cnLib_FUNC SetText(const uChar16 *Text,uIntn TextLength)noexcept(true)=0;
	virtual iWriteBuffer<uChar16>* cnLib_FUNC QueryText(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
namespace cnSystem{
//---------------------------------------------------------------------------
namespace CurrentUIThread{
//---------------------------------------------------------------------------
iUIThread* cnLib_FUNC GetUIThread(void)noexcept(true);
iDispatch* cnLib_FUNC GetDispatch(bool HighPriority)noexcept(true);
iUIKeyboard* cnLib_FUNC GetDefaultKeyboard(void)noexcept(true);
iUIMouse* cnLib_FUNC GetDefaultMouse(void)noexcept(true);
//---------------------------------------------------------------------------
}	// namespace CurrentUIThread
//---------------------------------------------------------------------------
iPtr<iUIView>	cnLib_FUNC CreateUIView(void)noexcept(true);

rPtr<iPopupWindowControl>	cnLib_FUNC CreatePopupWindowControl(void)noexcept(true);

iInterface* cnLib_FUNC GetSysMouseCursor(eSysMouseCursor CursorID)noexcept(true);
rPtr<iClipboardReader> cnLib_FUNC QueryClipboardReader(iUIView *View)noexcept(true);
rPtr<iClipboardWriter> cnLib_FUNC QueryClipboardWriter(iUIView *View)noexcept(true);
//---------------------------------------------------------------------------
}   // namespace cnSystem
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#ifdef	cnLibrary_CPPEXCLUDE_VIRTUAL_OVERRIDE
#undef	override
#endif
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif
