#include "WinConsole.h"
#include <cnRTL\UIGraph.h>

using namespace cnLibrary;
using namespace cnRTL;
using namespace cnWinRTL;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWin32DebugTextOutput::cWin32DebugTextOutput()noexcept
{
}
//---------------------------------------------------------------------------
cWin32DebugTextOutput::~cWin32DebugTextOutput()noexcept
{
}
//---------------------------------------------------------------------------
uIntn cWin32DebugTextOutput::Print(const uChar16 *Text,uIntn Length)noexcept
{
	auto CurrentCodePage=GetACP();
	arPtr<cWin32CodePageConvertBuffer> Converter;
	Converter=fBufferRecycler.Query();
	Converter->TranslateUTF16(Text,Length);
	::OutputDebugStringA(Converter->Result->Pointer);

	return Converter->Result->Length;
}
//---------------------------------------------------------------------------


//	If a console application loads the gdi32.dll or user32.dll library, 
//	the HandlerRoutine function that you specify when you call SetConsoleCtrlHandler does not get called for the CTRL_LOGOFF_EVENT and CTRL_SHUTDOWN_EVENT events. 
//	The operating system recognizes processes that load gdi32.dll or user32.dll as Windows applications rather than console applications.
//	This behavior also occurs for console applications that do not call functions in gdi32.dll or user32.dll directly, 
//	but do call functions such as Shell functions that do in turn call functions in gdi32.dll or user32.dll.

#if 0
       ESC (0x1B, ^[) starts an escape sequence;
       CSI (0x9B) is equivalent to ESC [.
--
       ECMA-48 CSI sequences

       CSI (or ESC [) is followed by a sequence of parameters, at most
       NPAR (16), that are decimal numbers separated by semicolons.  An
       empty or absent parameter is taken to be 0.  The sequence of
       parameters may be preceded by a single question mark.

       However, after CSI [ (or ESC [ [) a single character is read and
       this entire sequence is ignored.  (The idea is to ignore an
       echoed function key.)

       The action of a CSI sequence is determined by its final
       character.

       @   ICH       Insert the indicated # of blank characters.
       A   CUU       Move cursor up the indicated # of rows.
       B   CUD       Move cursor down the indicated # of rows.
       C   CUF       Move cursor right the indicated # of columns.
       D   CUB       Move cursor left the indicated # of columns.
       E   CNL       Move cursor down the indicated # of rows, to column
                     1.
       F   CPL       Move cursor up the indicated # of rows, to column
                     1.
       G   CHA       Move cursor to indicated column in current row.
       H   CUP       Move cursor to the indicated row, column (origin at
                     1,1).
       J   ED        Erase display (default: from cursor to end of
                     display).
                     ESC [ 1 J: erase from start to cursor.
                     ESC [ 2 J: erase whole display.
                     ESC [ 3 J: erase whole display including scroll-
                     back buffer (since Linux 3.0).
       L   IL        Insert the indicated # of blank lines.
       M   DL        Delete the indicated # of lines.
       P   DCH       Delete the indicated # of characters on current
                     line.
       X   ECH       Erase the indicated # of characters on current
                     line.
       a   HPR       Move cursor right the indicated # of columns.
       c   DA        Answer ESC [ ? 6 c: "I am a VT102".
       d   VPA       Move cursor to the indicated row, current column.
       e   VPR       Move cursor down the indicated # of rows.
       f   HVP       Move cursor to the indicated row, column.
       g   TBC       Without parameter: clear tab stop at current
                     position.
                     ESC [ 3 g: delete all tab stops.
       h   SM        Set Mode (see below).
       l   RM        Reset Mode (see below).
       m   SGR       Set attributes (see below).
       n   DSR       Status report (see below).
       q   DECLL     Set keyboard LEDs.
                     ESC [ 0 q: clear all LEDs
                     ESC [ 1 q: set Scroll Lock LED
                     ESC [ 2 q: set Num Lock LED
                     ESC [ 3 q: set Caps Lock LED

       r   DECSTBM   Set scrolling region; parameters are top and bottom
                     row.
       s   ?         Save cursor location.
       u   ?         Restore cursor location.
       `   HPA       Move cursor to indicated column in current row.

       ECMA-48 Select Graphic Rendition

       The ECMA-48 SGR sequence ESC [ parameters m sets display
       attributes.  Several attributes can be set in the same sequence,
       separated by semicolons.  An empty parameter (between semicolons
       or string initiator or terminator) is interpreted as a zero.

       param     result
       0         reset all attributes to their defaults
       1         set bold
       2         set half-bright (simulated with color on a color
                 display)
       4         set underscore (simulated with color on a color
                 display) (the colors used to simulate dim or underline
                 are set using ESC ] ...)
       5         set blink
       7         set reverse video
       10        reset selected mapping, display control flag, and
                 toggle meta flag (ECMA-48 says "primary font").
       11        select null mapping, set display control flag, reset
                 toggle meta flag (ECMA-48 says "first alternate font").
       12        select null mapping, set display control flag, set
                 toggle meta flag (ECMA-48 says "second alternate
                 font").  The toggle meta flag causes the high bit of a
                 byte to be toggled before the mapping table translation
                 is done.
       21        set underline; before Linux 4.17, this value set normal
                 intensity (as is done in many other terminals)
       22        set normal intensity
       24        underline off
       25        blink off
       27        reverse video off
       30        set black foreground
       31        set red foreground
       32        set green foreground
       33        set brown foreground
       34        set blue foreground
       35        set magenta foreground
       36        set cyan foreground
       37        set white foreground
       38        256/24-bit foreground color follows, shoehorned into 16
                 basic colors (before Linux 3.16: set underscore on, set
                 default foreground color)
       39        set default foreground color (before Linux 3.16: set
                 underscore off, set default foreground color)
       40        set black background
       41        set red background
       42        set green background
       43        set brown background
       44        set blue background
       45        set magenta background
       46        set cyan background
       47        set white background
       48        256/24-bit background color follows, shoehorned into 8
                 basic colors
       49        set default background color
       90..97    set foreground to bright versions of 30..37
       100.107   set background, same as 40..47 (bright not supported)

       Commands 38 and 48 require further arguments:

       ;5;x       256 color: values 0..15 are IBGR (black, red, green,
                  ... white), 16..231 a 6x6x6 color cube, 232..255 a
                  grayscale ramp
       ;2;r;g;b   24-bit color, r/g/b components are in the range 0..255

       ECMA-48 Mode Switches

       ESC [ 3 h
              DECCRM (default off): Display control chars.

       ESC [ 4 h
              DECIM (default off): Set insert mode.

       ESC [ 20 h
              LF/NL (default off): Automatically follow echo of LF, VT,
              or FF with CR.

       ECMA-48 Status Report Commands

       ESC [ 5 n
              Device status report (DSR): Answer is ESC [ 0 n (Terminal
              OK).

       ESC [ 6 n
              Cursor position report (CPR): Answer is ESC [ y ; x R,
              where x,y is the cursor location.

       DEC Private Mode (DECSET/DECRST) sequences

       These are not described in ECMA-48.  We list the Set Mode
       sequences; the Reset Mode sequences are obtained by replacing the
       final 'h' by 'l'.

       ESC [ ? 1 h
              DECCKM (default off): When set, the cursor keys send an
              ESC O prefix, rather than ESC [.

       ESC [ ? 3 h
              DECCOLM (default off = 80 columns): 80/132 col mode
              switch.  The driver sources note that this alone does not
              suffice; some user-mode utility such as resizecons(8) has
              to change the hardware registers on the console video
              card.

       ESC [ ? 5 h
              DECSCNM (default off): Set reverse-video mode.

       ESC [ ? 6 h
              DECOM (default off): When set, cursor addressing is
              relative to the upper left corner of the scrolling region.

       ESC [ ? 7 h
              DECAWM (default on): Set autowrap on.  In this mode, a
              graphic character emitted after column 80 (or column 132
              of DECCOLM is on) forces a wrap to the beginning of the
              following line first.

       ESC [ ? 8 h
              DECARM (default on): Set keyboard autorepeat on.

       ESC [ ? 9 h
              X10 Mouse Reporting (default off): Set reporting mode to 1
              (or reset to 0)¡ªsee below.

       ESC [ ? 25 h
              DECTECM (default on): Make cursor visible.

       ESC [ ? 1000 h
              X11 Mouse Reporting (default off): Set reporting mode to 2
              (or reset to 0)¡ªsee below.

-- linux
       ESC- but not CSI-sequences

       ESC c     RIS      Reset.
       ESC D     IND      Linefeed.
       ESC E     NEL      Newline.
       ESC H     HTS      Set tab stop at current column.
       ESC M     RI       Reverse linefeed.
       ESC Z     DECID    DEC private identification. The kernel returns
                          the string  ESC [ ? 6 c, claiming that it is a
                          VT102.
       ESC 7     DECSC    Save current state (cursor coordinates,
                          attributes, character sets pointed at by G0,
                          G1).
       ESC 8     DECRC    Restore state most recently saved by ESC 7.
       ESC [     CSI      Control sequence introducer
       ESC %              Start sequence selecting character set
       ESC % @               Select default (ISO 646 / ISO 8859-1)
       ESC % G               Select UTF-8
       ESC % 8               Select UTF-8 (obsolete)
       ESC # 8   DECALN   DEC screen alignment test - fill screen with
                          E's
       ESC (              Start sequence defining G0 character set
                          (followed by one of B, 0, U, K, as below)
       ESC ( B            Select default (ISO 8859-1 mapping)
       ESC ( 0            Select VT100 graphics mapping
       ESC ( U            Select null mapping - straight to character
                          ROM
       ESC ( K            Select user mapping - the map that is loaded
                          by the utility mapscrn(8)
       ESC )              Start sequence defining G1 (followed by one of
                          B, 0, U, K, as above).
       ESC >     DECPNM   Set numeric keypad mode
       ESC =     DECPAM   Set application keypad mode

       ESC ]     OSC      (Should be: Operating system command) ESC ] P
                          nrrggbb: set palette, with parameter given in
                          7 hexadecimal digits after the final P :-(.
                          Here n is the color (0¨C15), and rrggbb
                          indicates the red/green/blue values (0¨C255).
                          ESC ] R: reset palette

#endif // 0
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWin32ConsoleOutput::cWin32ConsoleOutput(HANDLE OutputHandle)noexcept
	: fOutputHandle(OutputHandle)
{
}
//---------------------------------------------------------------------------
cWin32ConsoleOutput::~cWin32ConsoleOutput()noexcept
{
}
//---------------------------------------------------------------------------
HANDLE cWin32ConsoleOutput::GetOutputHandle(void)const noexcept
{
	return fOutputHandle;
}
//---------------------------------------------------------------------------
cUIPoint cWin32ConsoleOutput::GetPageSize(void)noexcept
{
	CONSOLE_SCREEN_BUFFER_INFO Info;
	if(::GetConsoleScreenBufferInfo(fOutputHandle,&Info)){
		cUIPoint p;
		p.x=Info.dwSize.X;
		p.y=Info.dwSize.Y;
		return p;
	}
	return UIPointZero;
}
//---------------------------------------------------------------------------
uIntn cWin32ConsoleOutput::Print(const uChar16 *Text,uIntn Length)noexcept
{
	DWORD LengthWritten=0;
	::WriteConsoleW(fOutputHandle,Text,Length,&LengthWritten,nullptr);
	return LengthWritten;
}
//---------------------------------------------------------------------------
cUIPoint cWin32ConsoleOutput::GetCursorPos(void)noexcept
{
	CONSOLE_SCREEN_BUFFER_INFO Info;
	if(::GetConsoleScreenBufferInfo(fOutputHandle,&Info)){
		cUIPoint p;
		p.x=Info.dwCursorPosition.X;
		p.y=Info.dwCursorPosition.Y;
		return p;
	}
	return UIPointZero;
}
//---------------------------------------------------------------------------
bool cWin32ConsoleOutput::SetCursorPos(cUIPoint Pos)noexcept
{
	COORD CurPos;
	CurPos.X=static_cast<SHORT>(Pos.x);
	CurPos.Y=static_cast<SHORT>(Pos.y);
	if(::SetConsoleCursorPosition(fOutputHandle,CurPos))
		return true;
	return false;
}
//---------------------------------------------------------------------------
bool cWin32ConsoleOutput::GetCursorVisible(void)noexcept
{
	CONSOLE_CURSOR_INFO CursorInfo;
	CursorInfo.dwSize=sizeof(CONSOLE_CURSOR_INFO);
	if(::GetConsoleCursorInfo(fOutputHandle,&CursorInfo)){
		return CursorInfo.bVisible!=FALSE;
	}
	return false;
}
//---------------------------------------------------------------------------
bool cWin32ConsoleOutput::SetCursorVisible(bool Visible)noexcept
{
	CONSOLE_CURSOR_INFO CursorInfo;
	CursorInfo.dwSize=sizeof(CONSOLE_CURSOR_INFO);
	CursorInfo.bVisible=Visible;
	if(::SetConsoleCursorInfo(fOutputHandle,&CursorInfo)){
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------
void cWin32ConsoleOutput::MoveCursor(eDirection Direction,ufInt16 Count)noexcept
{

}
//---------------------------------------------------------------------------
void cWin32ConsoleOutput::MoveCursorLineHead(bool Up,ufInt16 Count)noexcept
{
}
//---------------------------------------------------------------------------
void cWin32ConsoleOutput::EraseLine(void)noexcept
{
	CONSOLE_SCREEN_BUFFER_INFO Info;
	if(::GetConsoleScreenBufferInfo(fOutputHandle,&Info)==FALSE){
		return;
	}
	DWORD CharWritten;
	DWORD WriteLength;
	COORD WriteCoord;
	WriteCoord.Y=Info.dwCursorPosition.Y;
	WriteLength=Info.dwSize.X;
	WriteCoord.X=0;
	::FillConsoleOutputCharacterW(fOutputHandle,L' ',WriteLength,WriteCoord,&CharWritten);
}
//---------------------------------------------------------------------------
void cWin32ConsoleOutput::EraseLinePart(bool AfterCursor)noexcept
{
	CONSOLE_SCREEN_BUFFER_INFO Info;
	if(::GetConsoleScreenBufferInfo(fOutputHandle,&Info)==FALSE){
		return;
	}
	DWORD CharWritten;
	DWORD WriteLength;
	COORD WriteCoord;
	WriteCoord.Y=Info.dwCursorPosition.Y;
	if(AfterCursor){
		WriteLength=Info.dwSize.X-Info.dwCursorPosition.X;
		WriteCoord.X=Info.dwCursorPosition.X;
	}
	else{
		WriteLength=Info.dwCursorPosition.X;
		WriteCoord.X=0;
	}
	::FillConsoleOutputCharacterW(fOutputHandle,L' ',WriteLength,WriteCoord,&CharWritten);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWin32ConsoleInput::cWin32ConsoleInput(HANDLE InputHandle,iDispatch *Dispatch)noexcept
	: fInputHandle(InputHandle)
	, fDispatch(Dispatch)
	, fConsoleInputWaiter(cnWindows::DefaultThreadPool->CreateHandleWaiter(&fInnerReference,&fConsoleHandleWaitProcedure))
	, fConsoleProcessWork(fDispatch->CreateWork(&fInnerReference,&fConsoleInputProcessProcedure))
{
}
//---------------------------------------------------------------------------
cWin32ConsoleInput::~cWin32ConsoleInput()noexcept
{
}
//---------------------------------------------------------------------------
void cWin32ConsoleInput::VirtualStarted(void)noexcept
{
	cDualReference::VirtualStarted();
}
//---------------------------------------------------------------------------
void cWin32ConsoleInput::VirtualStopped(void)noexcept
{
	cDualReference::VirtualStopped();
}
//---------------------------------------------------------------------------
HANDLE cWin32ConsoleInput::GetInputHandle(void)const noexcept
{
	return fInputHandle;
}
//---------------------------------------------------------------------------
iDispatch* cWin32ConsoleInput::GetDispatch(void)noexcept(true)
{
	return fDispatch;
}
//---------------------------------------------------------------------------
bool cWin32ConsoleInput::StartHandle(iReference *Reference,iConsoleInputHandler *Handler)noexcept(true)
{
	if(fHandleStartFlag.Acquire.Xchg(true)){
		return false;
	}

	if(fHandler!=nullptr){
		return false;
	}

	fHandlerReference=Reference;
	fHandler=Handler;
	fHandleStopFlag=false;
	fHandlerActive=false;

	NotifyConsoleInputProcess();

	fHandleStartFlag.Release.Store(false);
	return true;

	fConsoleInputWaiter->SetWait(fInputHandle,nullptr);
}
//---------------------------------------------------------------------------
void cWin32ConsoleInput::StopHandle(void)noexcept(true)
{
	if(fHandleStopFlag.Free.Xchg(true))
		return;

	NotifyConsoleInputProcess();
}
//---------------------------------------------------------------------------
void cWin32ConsoleInput::cConsoleHandleWaitProcedure::Execute(DWORD WaitResult)noexcept
{
	auto Host=cnMemory::GetObjectFromMemberPointer(this,&cWin32ConsoleInput::fConsoleHandleWaitProcedure);
	return Host->NotifyConsoleInputProcess();
}
//---------------------------------------------------------------------------
void cWin32ConsoleInput::cConsoleInputProcessProcedure::Execute(void)noexcept
{
	auto Host=cnMemory::GetObjectFromMemberPointer(this,&cWin32ConsoleInput::fConsoleInputProcessProcedure);
	return Host->ConsoleInputProcessThread();
}
//---------------------------------------------------------------------------
void cWin32ConsoleInput::NotifyConsoleInputProcess(void)noexcept
{
	if(fInputThreadExclusiveFlag.Acquire()==false)
		return;

	fConsoleProcessWork->Start();
}
//---------------------------------------------------------------------------
void cWin32ConsoleInput::ConsoleInputProcessThread(void)noexcept
{
	do{
		while(fHandler!=nullptr){
			fInputThreadExclusiveFlag.Continue();

			if(ConsoleInputProcess()==false)
				break;
		}
	}while(fInputThreadExclusiveFlag.Release()==false);
}
//---------------------------------------------------------------------------
bool cWin32ConsoleInput::ConsoleInputProcess(void)noexcept
{
	if(fHandler==nullptr){
		return false;
	}

	if(fHandlerActive==false){
		fHandlerActive=true;
		fHandler->ConsoleStarted();
	}

	if(fHandleStopFlag.Get()){
		fHandlerActive=false;
			
		auto HandlerReference=cnVar::MoveCast(fHandlerReference);
		auto Handler=fHandler;
		fHandler=nullptr;

		Handler->ConsoleStopped();
		return false;
	}
	
	if(HandleInput()){
		return true;
	}
	fConsoleInputWaiter->SetWait(fInputHandle,nullptr);
	return false;
}
//---------------------------------------------------------------------------
bool cWin32ConsoleInput::HandleInput(void)noexcept
{
	DWORD TotalEvent=0;
	if(::GetNumberOfConsoleInputEvents(fInputHandle,&TotalEvent)==false){
		return false;
	}
	if(TotalEvent==0)
		return false;
	iConsoleInputHandler *Handler;
	do{
		// has input records
		constexpr uIntn RecordBufferCount=4;
		INPUT_RECORD RecordBuffer[RecordBufferCount];
		DWORD RecordsRead=0;
		if(::ReadConsoleInputW(fInputHandle,RecordBuffer,RecordBufferCount,&RecordsRead)==false){
			return false;
		}
		for(DWORD i=0;i<RecordsRead;i++){
			Handler=fHandler;
			if(Handler==nullptr){
				break;
			}
			auto &Record=RecordBuffer[i];
			switch(Record.EventType){
			case KEY_EVENT:			// 0x0001	The Event member contains a KEY_EVENT_RECORD structure with information about a keyboard event.
				HandleInputRecordKey(Handler,Record.Event.KeyEvent);
				break;
			case MOUSE_EVENT:		// 0x0002	The Event member contains a MOUSE_EVENT_RECORD structure with information about a mouse movement or button press event.
				HandleInputRecordMouse(Handler,Record.Event.MouseEvent);
				break;
			case WINDOW_BUFFER_SIZE_EVENT:		// 0x0004	The Event member contains a WINDOW_BUFFER_SIZE_RECORD structure with information about the new size of the console screen buffer.
				HandleInputRecordWindowBufferSize(Handler,Record.Event.WindowBufferSizeEvent);
				break;
			}
		}
		if(::GetNumberOfConsoleInputEvents(fInputHandle,&TotalEvent)==false){
			return false;
		}
	}while(TotalEvent!=0);
	Handler=fHandler;
	if(Handler==nullptr){
		return false;
	}
	uIntn DismissLength=0;
	if(fInputTextBuffer->Length!=0){
		Handler->TextInput(fInputTextBuffer->Pointer,fInputTextBuffer->Length);
		fInputTextBuffer.Clear();
	}
	return true;
}
//---------------------------------------------------------------------------
void cWin32ConsoleInput::HandleInputRecordKey(iConsoleInputHandler *Handler,KEY_EVENT_RECORD &Record)noexcept
{
	if(Record.bKeyDown){
		for(uIntn i=0;i<Record.wRepeatCount;i++){
			if(Record.wVirtualKeyCode!=0){
				Handler->KeyInput(static_cast<KeyCode>(Record.wVirtualKeyCode));
			}
			if(Record.uChar.UnicodeChar>0x1F && Record.uChar.UnicodeChar!=0x7F){
				// treat as text character
				fInputTextBuffer.AppendChar(static_cast<uChar16>(Record.uChar.UnicodeChar));
			}
			else{
				// filter out non-display characters
			}
		}
	}
}
//---------------------------------------------------------------------------
void cWin32ConsoleInput::HandleInputRecordMouse(iConsoleInputHandler *Handler,MOUSE_EVENT_RECORD &Record)noexcept
{
}
//---------------------------------------------------------------------------
void cWin32ConsoleInput::HandleInputRecordWindowBufferSize(iConsoleInputHandler *Handler,WINDOW_BUFFER_SIZE_RECORD &Record)noexcept
{
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWindowsConsoleHost::cWindowsConsoleHost(HANDLE OutputHandle,HANDLE InputHandle,iDispatch *Dispatch)noexcept
	: fOutput(rCreate<cWin32ConsoleOutput>(OutputHandle))
	, fInput(rCreate<cWin32ConsoleInput>(InputHandle,Dispatch))
{
}
//---------------------------------------------------------------------------
cWindowsConsoleHost::~cWindowsConsoleHost()noexcept
{
}
//---------------------------------------------------------------------------
iConsoleOutput* cWindowsConsoleHost::GetOutput(void)noexcept
{
	return fOutput;
}
//---------------------------------------------------------------------------
iConsoleInput* cWindowsConsoleHost::GetInput(void)noexcept
{
	return fInput;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
rPtr<iWindowsConsoleHost> cnWinRTL::CreateWindowsConsoleHost(iDispatch *Dispatch)noexcept
{
	auto InputHandle=::GetStdHandle(STD_INPUT_HANDLE);
	auto OutputHandle=::GetStdHandle(STD_OUTPUT_HANDLE);

	return rCreate<cWindowsConsoleHost>(OutputHandle,InputHandle,Dispatch);
}
//---------------------------------------------------------------------------
