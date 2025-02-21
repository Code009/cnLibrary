//---------------------------------------------------------------------------
#include "CommandLineConsole.h"
#include "InterfaceImplementation.h"

using namespace cnLibrary;
using namespace cnRTL;


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
static cStringBuffer<uChar16> ToUpperCase(const uChar16 *String)noexcept(true)
{
	cStringBuffer<uChar16> Result;
	while(*String!=0){
		uChar16 c=*String++;
		if(c>=u'a' && c<=u'z')
			c+=static_cast<uChar16>(u'A'-u'a');
		
		Result.AppendChar(c);
	}
	return Result;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cCommandDispatcher::cCommandDispatcher()noexcept(true)
{
}
//---------------------------------------------------------------------------
cCommandDispatcher::~cCommandDispatcher()noexcept(true)
{
}
//---------------------------------------------------------------------------
void cCommandDispatcher::Execute(const uChar16 *Text,uIntn Length)noexcept(true)
{
	if(Length==0)
		return;
	cCmdTextParser<uChar16> Parser;
	Parser.Reset(Text,Length);

	return Execute(Parser);
}
//---------------------------------------------------------------------------
void cCommandDispatcher::Execute(cCmdTextParser<uChar16> &Parser)noexcept(true)
{
	if(Parser.Run()==false){
		//Log("Unknow command\n");
		return;
	}

	auto CmdPart=Parser.Value();
	auto CmdProc=fCmdMap.GetPair(CmdPart);
	if(CmdProc==nullptr){
		//Log("Unknow command\n");
		return;
	}
	// execute
	CmdProc->Value(Parser);
}
//---------------------------------------------------------------------------
void cCommandDispatcher::ClearCommand(void)noexcept(true)
{
	fCmdMap.Clear();
}
//---------------------------------------------------------------------------
void cCommandDispatcher::ResetCommand(const uChar16 *Name)noexcept(true)
{
	cString<uChar16> NameU=ToUpperCase(Name);
	fCmdMap.Remove(NameU);
}
//---------------------------------------------------------------------------
cFunction<void (cCmdTextParser<uChar16>&)>& cCommandDispatcher::SetCommand(const uChar16 *Name)noexcept(true)
{
	cString<uChar16> NameU=ToUpperCase(Name);
	return fCmdMap[NameU];
}
//---------------------------------------------------------------------------
cFunction<void (cCmdTextParser<uChar16>&)>& cCommandDispatcher::operator [](const uChar16 *Name)noexcept(true)
{
	return SetCommand(Name);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cConsoleOutputWithPrompt::cConsoleOutputWithPrompt(rPtr<iConsoleOutput> ConsoleOutput)noexcept(true)
	: fConsoleOutput(cnVar::MoveCast(ConsoleOutput))
{
}
//---------------------------------------------------------------------------
cConsoleOutputWithPrompt::~cConsoleOutputWithPrompt()noexcept(true)
{
}
//---------------------------------------------------------------------------
cConsoleOutputWithPrompt::cPromptSaveState::cPromptSaveState(cConsoleOutputWithPrompt *p)noexcept(true)
	: Owner(p)
{
	if(Owner->fLineText==nullptr){
		Cleared=false;
		return;
	}
	LineText=Owner->fLineText->Get();
	if(LineText.Pointer==nullptr){
		Cleared=false;
		return;
	}
	// cleanup prompt
	Owner->ClearPrompt();
	Cleared=true;
}
//---------------------------------------------------------------------------
cConsoleOutputWithPrompt::cPromptSaveState::~cPromptSaveState()noexcept(true)
{
	if(Cleared){
		Owner->RestorePrompt(LineText.Pointer,LineText.Length);
	}
}
//---------------------------------------------------------------------------
cConsoleOutputWithPrompt::cPromptSaveState::operator bool()noexcept(true)
{
	return Cleared;
}
//---------------------------------------------------------------------------
void cConsoleOutputWithPrompt::ClearPrompt(void)noexcept(true)
{
	fConsoleOutput->SetCursorPos(fPromptPos);
	fConsoleOutput->EraseLinePart(true);
}
//---------------------------------------------------------------------------
void cConsoleOutputWithPrompt::RestorePrompt(const uChar16 *Text,uIntn Length)noexcept(true)
{
	fConsoleOutput->Print(Text,Length);
	
	if(fPromptCursorPos!=0){
		ApplyPromptCursorPos();
	}
}
//---------------------------------------------------------------------------
uIntn cConsoleOutputWithPrompt::Print(const uChar16 *Text,uIntn Length)noexcept(true)
{
	cPromptSaveState SaveState(this);
	return fConsoleOutput->Print(Text,Length);
}
//---------------------------------------------------------------------------
cUIPoint cConsoleOutputWithPrompt::GetCursorPos(void)noexcept(true)
{
	return fConsoleOutput->GetCursorPos();
}
//---------------------------------------------------------------------------
bool cConsoleOutputWithPrompt::SetCursorPos(cUIPoint Pos)noexcept(true)
{
	cPromptSaveState SaveState(this);
	// print
	if(fConsoleOutput->SetCursorPos(Pos)==false){
		return false;
	}
	fPromptPos=Pos;
	return true;
}
//---------------------------------------------------------------------------
bool cConsoleOutputWithPrompt::GetCursorVisible(void)noexcept(true)
{
	return fConsoleOutput->GetCursorVisible();
}
//---------------------------------------------------------------------------
bool cConsoleOutputWithPrompt::SetCursorVisible(bool Visible)noexcept(true)
{
	return fConsoleOutput->SetCursorVisible(Visible);
}
//---------------------------------------------------------------------------
void cConsoleOutputWithPrompt::MoveCursor(eDirection Direction,ufInt16 Count)noexcept(true)
{
	cPromptSaveState SaveState(this);
	fConsoleOutput->MoveCursor(Direction,Count);
	if(SaveState){
		fPromptPos=fConsoleOutput->GetCursorPos();
	}
}
//---------------------------------------------------------------------------
void cConsoleOutputWithPrompt::MoveCursorLineHead(bool Up,ufInt16 Count)noexcept(true)
{
	cPromptSaveState SaveState(this);
	fConsoleOutput->MoveCursorLineHead(Up,Count);
	if(SaveState){
		fPromptPos=fConsoleOutput->GetCursorPos();
	}
}
//---------------------------------------------------------------------------
cUIPoint cConsoleOutputWithPrompt::GetPageSize(void)noexcept(true)
{
	return fConsoleOutput->GetPageSize();
}
//---------------------------------------------------------------------------
void cConsoleOutputWithPrompt::EraseLine(void)noexcept(true)
{
	return fConsoleOutput->EraseLine();
}
//---------------------------------------------------------------------------
void cConsoleOutputWithPrompt::EraseLinePart(bool AfterCursor)noexcept(true)
{
	return fConsoleOutput->EraseLinePart(AfterCursor);
}
//---------------------------------------------------------------------------
void cConsoleOutputWithPrompt::StartPrompt(iStringReference *Reference)noexcept(true)
{
	fLineText=Reference;
	// record echo pos
	fPromptPos=fConsoleOutput->GetCursorPos();
	// print new text
	auto TextArray=fLineText->Get();
	fConsoleOutput->Print(TextArray.Pointer,TextArray.Length);
	fPromptCursorPos=TextArray.Length;
	fPromptLength=TextArray.Length;
}
//---------------------------------------------------------------------------
void cConsoleOutputWithPrompt::FinishPrompt(void)noexcept(true)
{
	if(fLineText==nullptr)
		return;

	auto TextArray=fLineText->Get();
	auto LinePos=LinePosToConsolePos(TextArray.Length);
	fConsoleOutput->SetCursorPos(LinePos);

	uChar16 LineFeed='\n';
	fConsoleOutput->Print(&LineFeed,1);
	fLineText=nullptr;
}
//---------------------------------------------------------------------------
void cConsoleOutputWithPrompt::UpdatePrompt(uIntn ChangedOffset)noexcept(true)
{
	if(fLineText==nullptr)
		return;

	auto PrintPos=LinePosToConsolePos(ChangedOffset);

	//if(ChangedOffset>=fPromptLength)
	//	return;

	auto TextArray=fLineText->Get();
	fConsoleOutput->SetCursorPos(PrintPos);
	fConsoleOutput->Print(TextArray.Pointer+ChangedOffset,TextArray.Length-ChangedOffset);
	fPromptLength=TextArray.Length;
	fConsoleOutput->EraseLinePart(true);
}
//---------------------------------------------------------------------------
uIntn cConsoleOutputWithPrompt::GetPromptCursorPos(void)noexcept(true)
{
	if(fLineText==nullptr)
		return IndexNotFound;

	return fPromptCursorPos;
}
//---------------------------------------------------------------------------
bool cConsoleOutputWithPrompt::SetPromptCursorPos(uIntn Pos)noexcept(true)
{
	if(fLineText==nullptr)
		return false;

	auto TextArray=fLineText->Get();
	if(Pos>TextArray.Length){
		return false;
	}
	fPromptCursorPos=Pos;
	ApplyPromptCursorPos();
	return true;
}
//---------------------------------------------------------------------------
void cConsoleOutputWithPrompt::ApplyPromptCursorPos(void)noexcept(true)
{
	auto CaretPos=LinePosToConsolePos(fPromptCursorPos);
	fConsoleOutput->SetCursorPos(CaretPos);
}
//---------------------------------------------------------------------------
cUIPoint cConsoleOutputWithPrompt::OffsetConsolePos(cUIPoint Pos,sIntn Offset)noexcept(true)
{
	Pos.x+=Offset;
	
	cUIPoint PageSize=fConsoleOutput->GetPageSize();
	while(Pos.x<0){
		Pos.x+=PageSize.x;
		Pos.y--;
	}
	while(Pos.x>PageSize.x){
		Pos.x-=PageSize.x;
		Pos.y++;
	}
	return Pos;
}
//---------------------------------------------------------------------------
cUIPoint cConsoleOutputWithPrompt::LinePosToConsolePos(uIntn Offset)noexcept(true)
{
	cUIPoint Pos=fPromptPos;
	Pos.x+=Offset;
	
	cUIPoint PageSize=fConsoleOutput->GetPageSize();
	//while(Pos.x<0){
	//	Pos.x+=PageSize.x;
	//	Pos.y--;
	//}
	while(Pos.x>PageSize.x){
		Pos.x-=PageSize.x;
		Pos.y++;
	}
	return Pos;
}
//---------------------------------------------------------------------------
uIntn cConsoleOutputWithPrompt::ConsolePosToLinePos(cUIPoint Pos)noexcept(true)
{
	cnLib_ASSERT(fLineText!=nullptr);
	if(fPromptPos.y>Pos.y){
		return IndexNotFound;
	}
	if(fPromptPos.y==Pos.y){
		if(fPromptPos.x>Pos.x){
			return IndexNotFound;
		}
		return Pos.x-fPromptPos.x;
	}
	cUIPoint PageSize=fConsoleOutput->GetPageSize();
	uIntn Offset=PageSize.x-fPromptPos.x;
	sfInt32 CurY=fPromptPos.y+1;
	while(CurY<Pos.y){
		Offset+=PageSize.x;
	}
	Offset+=Pos.x;
	return Offset;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcConsoleLineInputHandler::bcConsoleLineInputHandler(iConsolePrompt *ConsolePrompt)noexcept(true)
	: fConsolePrompt(ConsolePrompt)
{
	fLineBuffer=rCreate<cStringBufferRef>();
}
//---------------------------------------------------------------------------
bcConsoleLineInputHandler::~bcConsoleLineInputHandler()noexcept(true)
{
}
//---------------------------------------------------------------------------
void bcConsoleLineInputHandler::KeyInput(eKeyCode KeyCode)noexcept(true)
{
	switch(KeyCode){
	default:
		break;
	case KeyCode::Backspace:
		{
			uIntn BufferLength=fLineBuffer->GetLength();
			if(BufferLength!=0){
				BufferLength--;
				fLineBuffer->SetLength(BufferLength);
					
				fConsolePrompt->UpdatePrompt(BufferLength);
				fConsolePrompt->SetPromptCursorPos(BufferLength);
			}
		}
		break;
	case KeyCode::Enter:
		{
			fConsolePrompt->FinishPrompt();

			// enter line
			LineInput(fLineBuffer->GetString(),fLineBuffer->GetLength());
			fLineBuffer->SetLength(0);
		}
		break;
	case KeyCode::Esc:
		{
			fLineBuffer->Clear();
			fConsolePrompt->UpdatePrompt(0);
		}
		break;
	case KeyCode::Left:
		{
			auto CurPos=fConsolePrompt->GetPromptCursorPos();
			if(CurPos>0){
				CurPos--;
				fConsolePrompt->SetPromptCursorPos(CurPos);
			}
		}
		break;
	case KeyCode::Right:
		{
			auto CurPos=fConsolePrompt->GetPromptCursorPos();
			uIntn CurLineLength=fLineBuffer->GetLength();
			if(CurPos<CurLineLength){
				CurPos++;
				fConsolePrompt->SetPromptCursorPos(CurPos);
			}
		}
		break;
	//case KeyCode::Up:
	//case KeyCode::Down:
	case KeyCode::Delete:
		break;
	}
}
//---------------------------------------------------------------------------
void bcConsoleLineInputHandler::TextInput(const uChar16 *Text,uIntn Length)noexcept(true)
{
	uIntn AppendIndex=fConsolePrompt->GetPromptCursorPos();
	uIntn CurLineLength=fLineBuffer->GetLength();
	if(AppendIndex>=CurLineLength){
		AppendIndex=CurLineLength;
	}
	fLineBuffer->Insert(AppendIndex,Text,Length);
	//fEditPos+=Length;
	if(AppendIndex==0){
		fConsolePrompt->StartPrompt(fLineBuffer);
	}
	else{
		fConsolePrompt->UpdatePrompt(AppendIndex);
	}
	fConsolePrompt->SetPromptCursorPos(AppendIndex+Length);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#if 0

void bcConsoleLineInputHandler::KeyInput(eKeyCode KeyCode)
{
	switch(KeyCode){
	case KeyCode::Backspace:
		if(fLineBuffer->Length!=0){
			fLineBuffer.SetLength(fLineBuffer->Length-1);

			UpdateLineEcho();
		}
		
		break;
	case KeyCode::Enter:
		{
			auto ConsolePrompt=LineConsolePrompt();
			ConsolePrompt->FinishLine();

			//auto ConsoleOutput=LineEchoOutput();
			//UpdateCaretPos(fLineBuffer->Length);
			//ConsoleOutput->Print("\n",1);

			auto LineBuffer=cnVar::MoveCast(fLineBuffer);
			// enter line
			LineInput(LineBuffer,LineBuffer.GetLength());
		}
		break;
	case KeyCode::Esc:
		{
			auto ConsolePrompt=LineConsolePrompt();
			ConsolePrompt->SetTextLength(0);
			fLineBuffer.Clear();

			//auto ConsoleOutput=LineEchoOutput();
			//ConsoleOutput->SetCursorPos(fLineEchoPos);
			//ConsoleOutput->EraseLinePart(true);
			//fLineBuffer.Clear();
		}
		break;
	//case KeyCode::Left:
	//	if(fEditPos>0){
	//		fEditPos--;
	//		UpdateCaretPos(fEditPos);
	//	}
	//	break;
	//case KeyCode::Right:
	//	if(fEditPos<fLineBuffer->Length){
	//		fEditPos++;
	//		UpdateCaretPos(fEditPos);
	//	}
	//	break;
	//case KeyCode::Up:
	//case KeyCode::Down:
	case KeyCode::Delete:
		break;
	}
}
//---------------------------------------------------------------------------
cUIPoint bcConsoleLineInputHandler::CalculateOffsetPos(cUIPoint Pos,cUIPoint Size,sIntn Offset)
{
	Pos.x+=Offset;
	while(Pos.x<0){
		Pos.x+=Size.x;
		Pos.y--;
	}
	while(Pos.x>Size.x){
		Pos.x-=Size.x;
		Pos.y++;
	}
	return Pos;
}
//---------------------------------------------------------------------------
void bcConsoleLineInputHandler::UpdateCaretPos(sIntn Offset)
{
	auto ConsoleOutput=LineEchoOutput();
	cUIPoint PageSize=ConsoleOutput->GetPageSize();
	auto CaretPos=CalculateOffsetPos(fLineEchoPos,PageSize,Offset);
	ConsoleOutput->SetCursorPos(CaretPos);
}
//---------------------------------------------------------------------------
void bcConsoleLineInputHandler::ClearLineEcho(void)
{
	if(fLineBuffer->Length!=0){
		auto ConsoleOutput=LineEchoOutput();
		ConsoleOutput->SetCursorPos(fLineEchoPos);
		ConsoleOutput->EraseLinePart(true);
	}
}
//---------------------------------------------------------------------------
void bcConsoleLineInputHandler::RestoreLineEcho(void)
{
	if(fLineBuffer->Length!=0){
		auto ConsoleOutput=LineEchoOutput();
		// update echo position
		fLineEchoPos=ConsoleOutput->GetCursorPos();
		ConsoleOutput->Print(fLineBuffer->Pointer,fLineBuffer->Length);
	}
}
//---------------------------------------------------------------------------
void bcConsoleLineInputHandler::UpdateLineEcho(void)
{
	auto ConsoleOutput=LineEchoOutput();
	ConsoleOutput->SetCursorPos(fLineEchoPos);
	ConsoleOutput->EraseLinePart(true);
	ConsoleOutput->Print(fLineBuffer->Pointer,fLineBuffer->Length);
}
//---------------------------------------------------------------------------
void bcConsoleLineInputHandler::TextInput(const uChar16 *Text,uIntn Length)
{
	auto ConsoleOutput=LineEchoOutput();
	bool InitalInput=fLineBuffer->Length==0;
	fLineBuffer.Append(Text,Length);
	fEditPos+=Length;
	if(InitalInput){
		// record echo pos
		fLineEchoPos=ConsoleOutput->GetCursorPos();
	}
	// print new text
	ConsoleOutput->Print(Text,Length);
}
#endif // 0
//---------------------------------------------------------------------------
