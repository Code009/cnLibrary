#include "TextEdit.h"

using namespace cnLibrary;
using namespace cnUI;

rPtr<viControl> (*cnUI::gCreateDefaultTextCaretVisual)(viTextCaretData *Data)noexcept(true)=vTextCaret::Create;
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
vTextCaret::vTextCaret(viTextCaretData *Data)noexcept(true)
	: fData(Data)
{
	DataInsertCallback();
}
//---------------------------------------------------------------------------
vTextCaret::~vTextCaret()noexcept(true)
{
	DataRemoveCallback();
}
//---------------------------------------------------------------------------
rPtr<viControl> vTextCaret::Create(viTextCaretData *Data)noexcept(true)
{
	return rCreate< bwvControl<vTextCaret> >(Data);
}
//---------------------------------------------------------------------------
viTextCaretData* vTextCaret::GetData(void)const noexcept(true)
{
	return fData;
}
//---------------------------------------------------------------------------
void vTextCaret::SetData(viTextCaretData *Data)noexcept(true)
{
	if(fData==Data)
		return;

	DataRemoveCallback();
	fData=Data;
	DataInsertCallback();
}
//---------------------------------------------------------------------------
void vTextCaret::DataInsertCallback(void)noexcept(true)
{
	if(fData!=nullptr){
		fTextCaretNotifyToken=fData->TextCaretNotifySet.Insert([this]{
			Update();
		});
	}
}
//---------------------------------------------------------------------------
void vTextCaret::DataRemoveCallback(void)noexcept(true)
{
	if(fData!=nullptr){
		fData->TextCaretNotifySet.Remove(fTextCaretNotifyToken);
	}
}
//---------------------------------------------------------------------------
void vTextCaret::Update(void)noexcept(true)
{
	if(fViewContent!=nullptr)
		fViewContent->QueryUpdate();
}
//---------------------------------------------------------------------------
void vTextCaret::Paint(iUISimplePaintContext *Context)noexcept(true)
{
	if(fData==nullptr)
		return;
	cUIPoint CaretPos;
	cUIPoint CaretSize;
	auto CaretVisible=fData->TextCaretVisualPosition(CaretPos,CaretSize);

	if(CaretVisible){
		Context->Fill(CaretPos,CaretSize,UIColorFromUInt32(0x7F000000));
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcTextDocumentEdit::bcTextDocumentEdit()noexcept(true)
{
	fControlFocused=false;
	fControlHot=false;
	cnMemory::ZeroFill(fKeyDown);

	MouseCursor=cnSystem::GetSysMouseCursor(SysMouseCursor::TextEdit);

	fCaretLineIndex=0;
	fCaretTextIndex=0;
	fCompositionCaretPos=0;
	
	InsertAnnotator(&fCompositionAnnotator);
}
//---------------------------------------------------------------------------
bcTextDocumentEdit::~bcTextDocumentEdit()noexcept(true)
{
	SetView(nullptr);
	viTextCaretData::InvalidateData();
	viControlStateData::InvalidateData();
}
//---------------------------------------------------------------------------
void bcTextDocumentEdit::EnableCompositionAnnotator(bool Enable)noexcept(true)
{
	if(Enable){
		InsertAnnotator(&fCompositionAnnotator);
	}
	else{
		RemoveAnnotator(&fCompositionAnnotator);
	}
}
//---------------------------------------------------------------------------
void bcTextDocumentEdit::UpdateZIndex(void)noexcept(true)
{
	bcTextDocument::UpdateZIndex();
	if(fCaretContent!=nullptr){
		fCaretContent->SetContentZPosition(CaretContentZPosition);
	}
}
//---------------------------------------------------------------------------
void bcTextDocumentEdit::ControlCaretContentSetDefault(void)noexcept(true)
{
	if(gCreateDefaultTextCaretVisual!=nullptr){
		SetCaretContent(gCreateDefaultTextCaretVisual(this));
	}
}
//---------------------------------------------------------------------------
void bcTextDocumentEdit::SetCaretContent(rPtr<viControl> Content)noexcept(true)
{
	if(fCaretContent!=nullptr){
		fCaretContent->SetView(nullptr);
	}
	fCaretContent=cnVar::MoveCast(Content);
	if(fCaretContent!=nullptr){
		fCaretContent->SetContentZPosition(CaretContentZPosition);
		fCaretContent->SetView(fView);
	}
}
//---------------------------------------------------------------------------
void bcTextDocumentEdit::ViewSetup(void)noexcept(true)
{
	bcTextDocument::ViewSetup();

	ViewInsertKeyHandler(fView,this);
	ViewInsertMouseHandler(fView,this);
	ViewInsertTouchHandler(fView,this);

	auto ViewKey=iCast<iUIKeyControl>(fView);
	if(ViewKey!=nullptr){
		ViewKey->InsertTextInputHandler(this);
		ViewKey->SetCompositionViewInfo(this);
	}

	if(fCaretContent!=nullptr){
		fCaretContent->SetView(fView);
	}
	else{
		ControlCaretContentSetDefault();
	}
}
//---------------------------------------------------------------------------
void bcTextDocumentEdit::ViewClear(void)noexcept(true)
{
	auto ViewKey=iCast<iUIKeyControl>(fView);
	if(ViewKey!=nullptr){
		ViewKey->RemoveTextInputHandler(this);
		ViewKey->SetCompositionViewInfo(nullptr);
	}

	if(fCaretContent!=nullptr){
		fCaretContent->SetView(nullptr);
	}

	ViewRemoveKeyHandler(fView,this);
	ViewRemoveMouseHandler(fView,this);
	ViewRemoveTouchHandler(fView,this);

	bcTextDocument::ViewClear();
}
//---------------------------------------------------------------------------
bool bcTextDocumentEdit::ControlHot(void)noexcept(true)
{
	return fControlHot;
}
//---------------------------------------------------------------------------
bool bcTextDocumentEdit::ControlFocused(void)noexcept(true)
{
	return fControlFocused;
}
//---------------------------------------------------------------------------
bool bcTextDocumentEdit::ControlDisabled(void)noexcept(true)
{
	if(fView!=nullptr){
		return !fView->IsEnabled();
	}
	return false;
}
//---------------------------------------------------------------------------
void bcTextDocumentEdit::ControlStateChanged(void)noexcept(true)
{
	ControlStateNotifySet();
}
//---------------------------------------------------------------------------
void bcTextDocumentEdit::MouseEnter(iUIMouseEvent*)noexcept(true)
{
	fControlHot=true;

	ControlStateChanged();
}
//---------------------------------------------------------------------------
void bcTextDocumentEdit::MouseLeave(iUIMouseEvent*)noexcept(true)
{
	fControlHot=false;
	fLBtnDown=false;
	ControlStateChanged();
}
//---------------------------------------------------------------------------
iInterface* bcTextDocumentEdit::MouseGetCursor(iUIMouseEvent*)noexcept(true)
{
	return MouseCursor;
}
//---------------------------------------------------------------------------
void bcTextDocumentEdit::MouseMove(iUIMouseEvent *MouseEvent)noexcept(true)
{
	cUIPoint Pos;
	MouseEvent->GetPosition(fView,Pos);

	if(ViewIsPointInside(fView,Pos.x,Pos.y))
		fControlHot=true;
	if(fLBtnDown){
		PositCaret(Pos,true);
	}
}
//---------------------------------------------------------------------------
void bcTextDocumentEdit::MouseDown(iUIMouseEvent *MouseEvent,eMouseButton Button)noexcept(true)
{
	cUIPoint Pos;
	MouseEvent->GetPosition(fView,Pos);

	auto ViewKey=iCast<iUIKeyControl>(fView);
	ViewKey->SetFocus(true);
	if(Button==MouseButton::Left){
		fLBtnDown=true;
		
		PositCaret(Pos,fKeyDown.Shift);
	}
}
//---------------------------------------------------------------------------
void bcTextDocumentEdit::MouseUp(iUIMouseEvent *MouseEvent,eMouseButton Button)noexcept(true)
{
	cUIPoint Pos;
	MouseEvent->GetPosition(fView,Pos);

	if(Button==MouseButton::Left){
		fLBtnDown=false;
	}
	fLBtnDown=false;
}
//---------------------------------------------------------------------------
void bcTextDocumentEdit::KeyFocusEnter(iUIKeyEvent *KeyEvent)noexcept(true)
{UnusedParameter(KeyEvent);
	fControlFocused=true;
	
	ControlStateChanged();

	// caret show
	CaretChanged();
}
//---------------------------------------------------------------------------
void bcTextDocumentEdit::KeyFocusLeave(iUIKeyEvent *KeyEvent)noexcept(true)
{UnusedParameter(KeyEvent);
	fControlFocused=false;

	cnMemory::ZeroFill(fKeyDown);
	
	// caret hide
	CaretChanged();

	ControlStateChanged();
}
//---------------------------------------------------------------------------
void bcTextDocumentEdit::KeyDown(iUIKeyEvent *KeyEvent,eKeyCode KeyCode)noexcept(true)
{UnusedParameter(KeyEvent,KeyCode);
	switch(KeyCode){
	default:
		break;
	case KeyCode::Shift:
		fKeyDown.Shift=true;
		break;
	}
}
//---------------------------------------------------------------------------
void bcTextDocumentEdit::KeyUp(iUIKeyEvent *KeyEvent,eKeyCode KeyCode)noexcept(true)
{UnusedParameter(KeyEvent,KeyCode);
	switch(KeyCode){
	default:
		break;
	case KeyCode::Shift:
		fKeyDown.Shift=false;
		break;
	}
}
//---------------------------------------------------------------------------
void bcTextDocumentEdit::KeyInput(iUIKeyEvent *KeyEvent,eKeyCode KeyCode)noexcept(true)
{
	bool ShiftPressed=KeyEvent->IsKeyDown(KeyCode::Shift);
	bool CtrlPressed=KeyEvent->IsKeyDown(KeyCode::Ctrl);
	switch(KeyCode){
	default:
		break;
	// movement
	case KeyCode::Left:
		CaretMoveText(true,ShiftPressed);
		break;
	case KeyCode::Right:
		CaretMoveText(false,ShiftPressed);
		break;
	case KeyCode::Up:
		CaretMoveLine(true,ShiftPressed);
		break;
	case KeyCode::Down:
		CaretMoveLine(false,ShiftPressed);
		break;
	case KeyCode::Home:
		if(CtrlPressed){
			CaretMoveLineEnd(true,ShiftPressed);
		}
		else{
			CaretMoveTextEnd(true,ShiftPressed);
		}
		break;
	case KeyCode::End:
		if(CtrlPressed){
			CaretMoveLineEnd(false,ShiftPressed);
		}
		else{
			CaretMoveTextEnd(false,ShiftPressed);
		}
		break;

	// Edit

	case KeyCode::Enter:
		// insert line
		if(ActionNewLine()){
		}
		break;

	case KeyCode::Backspace:
		ActionDelete(true);
		break;
	case KeyCode::Delete:
		ActionDelete(false);
		break;

//	case KeyCode::kZ:
//		if(CtrlPressed){
//			// Undo
//		}
//		break;

	case KeyCode::kA:
		if(CtrlPressed){
			// Select all
			SelectAll();
		}
		break;

	// clipboard

	case KeyCode::kX:
		if(CtrlPressed){
			CutToClipboard();
		}
		break;
	case KeyCode::kC:
		if(CtrlPressed){
			CopyToClipboard();
		}
		break;
	case KeyCode::kV:
		if(CtrlPressed){
			PasteFromClipboard();
		}
		break;
	}
}
//---------------------------------------------------------------------------
void bcTextDocumentEdit::TextInput(const uChar16 *Text,uIntn Length)noexcept(true)
{
	ActionInputText(Text,Length);
}
//---------------------------------------------------------------------------
void bcTextDocumentEdit::TextCompositionStarted(void)noexcept(true)
{
	fCompositionState=true;
}
//---------------------------------------------------------------------------
void bcTextDocumentEdit::TextCompositionFinished(void)noexcept(true)
{
	fCompositionState=false;
	fCompositionString.Clear();
	fCompositionTextLayout=nullptr;
	ClearCache();
	SetNeedUpdateTextContent();
}
//---------------------------------------------------------------------------
void bcTextDocumentEdit::TextCompositionUpdate(sfInt32 CaretPos,const uChar16 *Text,uIntn TextLength)noexcept(true)
{
	fCompositionCaretPos=CaretPos;

	if(fCompositionString->Length==TextLength && cnMemory::IsEqual(fCompositionString->Pointer,Text,TextLength*2)){
		CaretChanged();
		return;
	}
	// change text
	fCompositionString.SetString(Text,TextLength);
	fCompositionTextLayout=nullptr;

	ClearCache();
	SetNeedUpdateTextContent();
}
//---------------------------------------------------------------------------
sfInt32 bcTextDocumentEdit::GetCompositionCaretPos(void)noexcept(true)
{
	return fCompositionCaretPos;
}
//---------------------------------------------------------------------------
void bcTextDocumentEdit::TextCompositionViewRect(cUIPoint &Position,cUIPoint &Size,Float32 &LineHeight)noexcept(true)
{
	Position={0,0};
	Size=fView->GetSize();
	LineHeight=GetLineHeight(fCaretLineIndex);
}
//---------------------------------------------------------------------------
Float32 bcTextDocumentEdit::CalculateCompositionCaretTextContentX(uIntn LineIndex,uIntn CompositionTextIndex)noexcept(true)
{
	auto LineItem=QueryCacheLine(LineIndex);
	if(LineItem==nullptr)
		return 0;

	Float32 LineLeft=AlignLineHorizontal(LineItem->Width);
	
	uIntn CurPartCompositionIndex=0;
	Float32 CurCompositionPartPos=LineLeft;

	for(uIntn PartIndex=0,PartCount=LineItem->LineParts.GetCount();PartIndex<PartCount;PartIndex++){
		auto &TextPart=LineItem->LineParts[PartIndex];
		uIntn TextLength=TextPart.Text->Length;
		if(TextPart.Annotator==&fCompositionAnnotator){
			// composition part
			if(TextLength!=0){
				if(CurPartCompositionIndex+TextLength>CompositionTextIndex){
					// found part
					uIntn TextIndexInPart=CompositionTextIndex-CurPartCompositionIndex;
					// beginning of the part
					Float32 PartTextPos=LineItem->TextPosition[TextPart.TotalTextIndex+TextIndexInPart];
					return LineLeft+PartTextPos;
				}
				// next part
				CurPartCompositionIndex+=TextLength;
			}
		
			CurCompositionPartPos=LineLeft+LineItem->TextPosition[TextPart.TotalTextIndex+TextLength];
		}
	}
	// TextIndex out of range, return end of text
	return CurCompositionPartPos;
}
//---------------------------------------------------------------------------
cUIPoint bcTextDocumentEdit::TextCompositionCaretPosition(uIntn CompositionTextIndex)noexcept(true)
{
	cUIPoint CaretPos;
	// caret posit at text
	CaretPos.y=GetLineContentY(fCaretLineIndex);
	CaretPos.x=CalculateCompositionCaretTextContentX(fCaretLineIndex,CompositionTextIndex);

	CaretPos.x+=GetDocumentContentX();
	CaretPos.y+=GetDocumentContentY();
	return CaretPos;
}
//---------------------------------------------------------------------------
cIndexPoint bcTextDocumentEdit::GetCaretPos(void)noexcept(true)
{
	cIndexPoint p;
	p.y=fCaretLineIndex;
	p.x=fCaretTextIndex;
	return p;
}
//---------------------------------------------------------------------------
void bcTextDocumentEdit::SetCaretPos(cIndexPoint CaretPos,bool KeepSelection)noexcept(true)
{
	SetCaretCorrectedPosition(ClipCaretPosition(CaretPos),KeepSelection);
}
//---------------------------------------------------------------------------
void bcTextDocumentEdit::PositCaret(cUIPoint Pos,bool KeepSelection)noexcept(true)
{
	if(fContent!=nullptr){
		auto CaretPos=HitTest(Pos.x,Pos.y);
		SetCaretCorrectedPosition(CaretPos,KeepSelection);
	}
}
//---------------------------------------------------------------------------
bool bcTextDocumentEdit::GetSelection(cIndexPoint &Begin,cIndexPoint &End)const noexcept(true)
{
	auto SelectionPoint=TextEditGetSelectionPoint();
	uIntn SelectionLineIndex=SelectionPoint.y;
	uIntn SelectionTextIndex=SelectionPoint.x;

	bool SelectPosStart;
	if(SelectionLineIndex==fCaretLineIndex){
		if(SelectionTextIndex==fCaretTextIndex){
			// no selection
			Begin.y=End.y=fCaretLineIndex;
			Begin.x=End.x=fCaretTextIndex;
			return false;
		}
		SelectPosStart=SelectionTextIndex<fCaretTextIndex;
	}
	else{
		SelectPosStart=SelectionLineIndex<fCaretLineIndex;
	}

	if(SelectPosStart){
		Begin.y=SelectionLineIndex;
		Begin.x=SelectionTextIndex;

		End.y=fCaretLineIndex;
		End.x=fCaretTextIndex;
	}
	else{
		Begin.y=fCaretLineIndex;
		Begin.x=fCaretTextIndex;

		End.y=SelectionLineIndex;
		End.x=SelectionTextIndex;

	}
	return true;
}
//---------------------------------------------------------------------------
void bcTextDocumentEdit::GetLineSelection(uIntn LineIndex,uIntn &Left,uIntn &Right)noexcept(true)
{
	auto SelectionPoint=TextEditGetSelectionPoint();
	uIntn SelectionLineIndex=SelectionPoint.y;
	uIntn SelectionTextIndex=SelectionPoint.x;

	if(LineIndex==SelectionLineIndex){
		if(LineIndex==fCaretLineIndex){
			if(SelectionTextIndex==fCaretTextIndex){
				Left=0;
				Right=0;
			}
			else if(SelectionTextIndex<fCaretTextIndex){
				Left=SelectionTextIndex;
				Right=fCaretTextIndex;
			}
			else{	// SelectionTextIndex>fCaretTextIndex
				Left=fCaretTextIndex;
				Right=SelectionTextIndex;
			}
			return;
		}
		else{	// LineIndex!=fCaretLineIndex
			if(SelectionLineIndex<fCaretLineIndex){
				// start with selection pos
				auto LineLength=TextEditGetLineLength(LineIndex);
				Left=SelectionTextIndex;
				Right=LineLength;
			}
			else{
				// start with caret pos
				Left=0;
				Right=SelectionTextIndex;
			}
		}
	}
	else{	// LineIndex!=SelectionLineIndex
		if(LineIndex==fCaretLineIndex){
			if(SelectionLineIndex<fCaretLineIndex){
				// start with selection pos
				Left=0;
				Right=fCaretTextIndex;
			}
			else{
				// start with caret pos
				auto LineLength=TextEditGetLineLength(LineIndex);
				Left=fCaretTextIndex;
				Right=LineLength;
			}
		}
		else{	// LineIndex!=fCaretLineIndex
			uIntn SelStartLine;
			uIntn SelEndLine;
			if(SelectionLineIndex<fCaretLineIndex){
				SelStartLine=SelectionLineIndex;
				SelEndLine=fCaretLineIndex;
			}
			else{
				SelStartLine=fCaretLineIndex;
				SelEndLine=SelectionLineIndex;
			}
			if(LineIndex>SelStartLine &&LineIndex<SelEndLine){
				Left=0;
				Right=TextEditGetLineLength(LineIndex);
			}
			else{
				Left=0;
				Right=0;
			}
		}
	}
}
//---------------------------------------------------------------------------
void bcTextDocumentEdit::SelectAll(void)noexcept(true)
{
	uIntn LastLineIndex=TextEditGetLineCount();
	if(LastLineIndex==0)
		return;
	LastLineIndex--;

	auto LastLineLength=TextEditGetLineLength(LastLineIndex);

	TextEditMoveCaretAndSelectionPoint(LastLineIndex,LastLineLength,0,0);
}
//---------------------------------------------------------------------------
void bcTextDocumentEdit::ScrollContentUpdateContentLayout(void)noexcept(true)
{
	bcTextDocument::ScrollContentUpdateContentLayout();

	CaretChanged();
}
//---------------------------------------------------------------------------
void bcTextDocumentEdit::TextEditApplyCompositionPartStyle(cUITextStyle &Style)noexcept(true)
{
	Style.BackgroundColor=UIColorFromUInt32(0xFFCCCCCC);
	Style.TextColor=UIColorFromUInt32(0xFF000000);
}
//---------------------------------------------------------------------------
bcTextDocumentEdit* bcTextDocumentEdit::cCompositionAnnotator::GetHost(void)noexcept(true)
{
	return cnMemory::GetObjectFromMemberPointer(this,&bcTextDocumentEdit::fCompositionAnnotator);
}
//---------------------------------------------------------------------------
void bcTextDocumentEdit::cCompositionAnnotator::Setup(uIntn LineIndex)noexcept(true)
{
	auto Host=GetHost();
	fEnumDone=true;
	if(LineIndex==Host->fCaretLineIndex){
		if(Host->fCompositionString.GetLength()!=0){
			fEnumDone=false;
		}
	}
}
//---------------------------------------------------------------------------
void bcTextDocumentEdit::cCompositionAnnotator::Clear(void)noexcept(true)
{
	fEnumDone=true;
}
//---------------------------------------------------------------------------
bool bcTextDocumentEdit::cCompositionAnnotator::Process(uIntn LineTextIndex,cTextDocumentLinePart &TextPart)noexcept(true)
{
	if(fEnumDone){
		return false;
	}
	auto Host=GetHost();
	if(LineTextIndex>Host->fCaretTextIndex){
		return false;
	}
	if(LineTextIndex<Host->fCaretTextIndex){
		// truncate part for composition
		if(LineTextIndex+TextPart.Length>Host->fCaretTextIndex){
			TextPart.Length=Host->fCaretTextIndex-LineTextIndex;
		}
		return false;
	}
	// LineTextIndex==Host->fCaretTextIndex;
	// insert composition texts
	return true;
}
//---------------------------------------------------------------------------
bool bcTextDocumentEdit::cCompositionAnnotator::Fetch(uIntn &AnnotateIndex,cTextDocumentLinePart &TextPart)noexcept(true)
{
	if(fEnumDone){
		return false;
	}
	auto Host=GetHost();
	// insert composition texts
	cnLib_ASSERT(AnnotateIndex==Host->fCaretTextIndex);
	TextPart.Text=Host->fCompositionString->Pointer;
	TextPart.Length=Host->fCompositionString->Length;
	Host->TextEditApplyCompositionPartStyle(TextPart.TextStyle);
	TextPart.TextDistance=nullptr;
	TextPart.TextHeight=0.;
	fEnumDone=true;
	return true;
}
//---------------------------------------------------------------------------
void bcTextDocumentEdit::CaretChanged(void)noexcept(true)
{
	TextCaretNotifySet();
}
//---------------------------------------------------------------------------
bool bcTextDocumentEdit::TextCaretVisualPosition(cUIPoint &CaretPos,cUIPoint &CaretSize)noexcept(true)
{
	if(fControlFocused==false)
		return false;

	auto VisualInfo=TextDocumentVisualInfo();
	
	if(fCaretLineIndex-VisualInfo.LineIndex>=VisualInfo.LineCount){
		// line out of range
		return false;
	}

	// caret posit at text
	auto LineRange=GetLineContentRange(fCaretLineIndex,fCaretLineIndex+1);
	CaretPos.y=LineRange.Begin;
	CaretSize.y=LineRange.End-LineRange.Begin;
	if(CaretSize.y<=0){
		return false;
	}

	bool CaretOverwrite=false;

	if(fCompositionState){
		sfInt32 CompositionCaretPos=GetCompositionCaretPos();
		CaretPos.x=CalculateCompositionCaretTextContentX(fCaretLineIndex,CompositionCaretPos);
		CaretSize.x=2;
	}
	else{
		auto CaretTextRange=GetTextContentRange(fCaretLineIndex,fCaretTextIndex,fCaretTextIndex+1);
		bool HasText=CaretTextRange.Begin<CaretTextRange.End;
	
		CaretPos.x=CaretTextRange.Begin;

		// caret size
		CaretSize.x=2;
		if(CaretOverwrite && HasText){
			CaretSize.x=CaretTextRange.End-CaretTextRange.Begin;
		}
	}

	CaretPos.x+=VisualInfo.ContentX;
	CaretPos.y+=VisualInfo.ContentY;

	return true;
}
//---------------------------------------------------------------------------
cIndexPoint bcTextDocumentEdit::ClipCaretPosition(cIndexPoint CaretPos)noexcept(true)
{
	if(CaretPos.y<=0){
		CaretPos.y=0;
	}
	else{ // CaretPos.y>0
		uIntn CaretLine=CaretPos.y;
		uIntn LineCount=TextEditGetLineCount();
		if(CaretLine>=LineCount){
			CaretLine=LineCount-1;
		}
		CaretPos.y=CaretLine;
	}

	if(CaretPos.x<=0){
		CaretPos.x=0;
	}
	else{ // CaretPos.x>0
		uIntn CaretTextPos=static_cast<uIntn>(CaretPos.x);
		auto LineLength=TextEditGetLineLength(CaretPos.y);
		if(CaretTextPos>LineLength){
			CaretTextPos=LineLength;
		}
		CaretPos.x=CaretTextPos;
	}
	return CaretPos;
}
//---------------------------------------------------------------------------
cIndexPoint bcTextDocumentEdit::TextEditGetSelectionPoint(void)const noexcept(true)
{
	// return empty selection point
	cIndexPoint Point;
	Point.x=fCaretTextIndex;
	Point.y=fCaretLineIndex;
	return Point;
}
//---------------------------------------------------------------------------
void bcTextDocumentEdit::TextEditMoveCaretAndSelectionPoint(uIntn CaretLineIndex,uIntn CaretTextIndex,uIntn,uIntn)noexcept(true)
{
	// move caret without selection
	fCaretLineIndex=CaretLineIndex;
	fCaretTextIndex=CaretTextIndex;
	
	ScrollToCaret();

	CaretChanged();
}
//---------------------------------------------------------------------------
void bcTextDocumentEdit::SetCaretCorrectedPosition(cIndexPoint NewCaretPos,bool KeepSelection)noexcept(true)
{
	cIndexPoint SelectionPoint;
	if(KeepSelection){
		// keep selection
		SelectionPoint=TextEditGetSelectionPoint();
	}
	else{
		// reset selection
		SelectionPoint=NewCaretPos;
	}

	if(fCompositionState){
		SetNeedUpdateTextContent();
	}
	TextEditMoveCaretAndSelectionPoint(NewCaretPos.y,NewCaretPos.x,SelectionPoint.y,SelectionPoint.x);
}
//---------------------------------------------------------------------------
void bcTextDocumentEdit::CaretMoveLine(bool Upward,bool WithSelection)noexcept(true)
{
	cIndexPoint NewCaretPos;
	if(Upward){
		NewCaretPos.y=fCaretLineIndex-1;
	}
	else{
		NewCaretPos.y=fCaretLineIndex+1;
	}
	NewCaretPos.x=fCaretTextIndex;
	SetCaretPos(NewCaretPos,WithSelection);
}
//---------------------------------------------------------------------------
cIndexPoint bcTextDocumentEdit::GetTextPosByOffset(sIntn Offset)noexcept(true)
{
	cIndexPoint TextPos;
	TextPos.y=fCaretLineIndex;
	TextPos.x=fCaretTextIndex;
	if(Offset<0){
		uIntn OffsetCount=static_cast<uIntn>(-Offset);
		while(static_cast<uIntn>(TextPos.x)<OffsetCount){
			if(TextPos.y==0){
				// moved to head
				TextPos.x=0;
				return TextPos;
			}
			// move to last line end
			TextPos.y--;
			OffsetCount-=1+TextPos.x;
			TextPos.x=TextEditGetLineLength(TextPos.y);
		}
		// move to previous text
		TextPos.x-=OffsetCount;
	}
	else if(Offset>0){
		TextPos.x+=Offset;
		auto LineCount=TextEditGetLineCount();
		uIntn CurLineLength=TextEditGetLineLength(fCaretLineIndex);
		while(static_cast<uIntn>(TextPos.x)>CurLineLength){
			// passed line end
			if(static_cast<uIntn>(TextPos.y+1)>=LineCount){
				// moved to the document end
				TextPos.x=CurLineLength;
				return TextPos;
			}
			// move to next line head
			TextPos.y++;
			TextPos.x-=1+CurLineLength;
			CurLineLength=TextEditGetLineLength(TextPos.y);
		}
	}
	return TextPos;
}
//---------------------------------------------------------------------------
void bcTextDocumentEdit::CaretMoveText(bool Backward,bool WithSelection)noexcept(true)
{
	cIndexPoint NewCaretPos;
	if(Backward){
		NewCaretPos=GetTextPosByOffset(-1);
	}
	else{
		NewCaretPos=GetTextPosByOffset(1);
	}
	SetCaretCorrectedPosition(NewCaretPos,WithSelection);
}
//---------------------------------------------------------------------------
void bcTextDocumentEdit::CaretMoveTextEnd(bool Backward,bool WithSelection)noexcept(true)
{
	cIndexPoint NewCaretPos;
	NewCaretPos.y=fCaretLineIndex;

	if(Backward){
		NewCaretPos.x=0;
	}
	else{
		NewCaretPos.x=TextEditGetLineLength(fCaretLineIndex);
	}

	SetCaretCorrectedPosition(NewCaretPos,WithSelection);
}
//---------------------------------------------------------------------------
void bcTextDocumentEdit::CaretMoveLineEnd(bool Upward,bool WithSelection)noexcept(true)
{
	cIndexPoint NewCaretPos;
	NewCaretPos.x=0;

	if(Upward){
		NewCaretPos.y=0;
	}
	else{
		uIntn LastLine=TextEditGetLineCount();
		if(LastLine!=0)
			LastLine--;

		NewCaretPos.y=LastLine;
		NewCaretPos.x=TextEditGetLineLength(LastLine);
	}

	SetCaretCorrectedPosition(NewCaretPos,WithSelection);
}
//---------------------------------------------------------------------------
bool bcTextDocumentEdit::CopyToClipboardValidated(cIndexPoint Begin,cIndexPoint End)noexcept(true)
{
	auto LineCount=static_cast<sIntn>(TextEditGetLineCount());
	if(LineCount==0){
		// no line to copy
		return false;
	}
	if(End.y>=LineCount){
		// fix last copy line
		End.y=LineCount-1;
		End.x=TextEditGetLineLength(LineCount-1);
	}
	if(Begin.y>End.y){
		// no text in range
		return false;
	}

	auto ClipboardWriter=cnSystem::QueryClipboardWriter(fView);
	if(ClipboardWriter==nullptr){
		return false;	// cannot open clipboard
	}
	auto *TextWriterBuffer=ClipboardWriter->QueryText();
	if(TextWriterBuffer==nullptr)
		return false;	// cannot open write buffer

	bool ClipboardResult=false;
	if(Begin.y==End.y){
		if(End.x<=Begin.x){
			// no text in range
			return false;
		}
		// copy single line
		uIntn CopyLength=End.x-Begin.x;

		cArray<uChar16> WriteMemory=TextWriterBuffer->ReserveWriteBuffer(CopyLength);
		if(CopyLength>WriteMemory.Length)
			CopyLength=WriteMemory.Length;
		if(CopyLength!=0){
			CopyLength=TextEditCopyLineText(Begin.y,Begin.x,WriteMemory.Pointer,CopyLength);

			TextWriterBuffer->CommitWriteBuffer(CopyLength);
		}
	}
	else{
		uIntn CopyLength;
		// copy first line
		auto FirstLineLength=TextEditGetLineLength(Begin.y);
		CopyLength=FirstLineLength-Begin.x;

		cArray<uChar16> WriteMemory=TextWriterBuffer->ReserveWriteBuffer(CopyLength);
		if(CopyLength>WriteMemory.Length){
			CopyLength=WriteMemory.Length;
		}
		if(CopyLength!=0){
			CopyLength=TextEditCopyLineText(Begin.y,Begin.x,WriteMemory.Pointer,CopyLength);

			TextWriterBuffer->CommitWriteBuffer(CopyLength);
		}
		static const uChar16 LineRet[2]={'\r','\n'};
		// copy middle full line
		for(uIntn CopyLineIndex=Begin.y+1;CopyLineIndex<static_cast<uIntn>(End.y);CopyLineIndex++){
			CopyLength=TextEditGetLineLength(CopyLineIndex);

			uIntn WriteBufferLength=(2+CopyLength);
			WriteMemory=TextWriterBuffer->ReserveWriteBuffer(WriteBufferLength);
			if(WriteMemory.Length>2){
				if(WriteBufferLength>WriteMemory.Length){
					WriteBufferLength=WriteMemory.Length;
					CopyLength=WriteMemory.Length-2;
				}
				// line return
				WriteMemory.Pointer[0]=LineRet[0];
				WriteMemory.Pointer[1]=LineRet[1];

				// line text
				CopyLength=TextEditCopyLineText(CopyLineIndex,0,WriteMemory.Pointer+2,CopyLength);

				TextWriterBuffer->CommitWriteBuffer(WriteBufferLength);
			}
		}
		// copy last line
		CopyLength=End.x;

		uIntn WriteBufferLength=2+CopyLength;

		WriteMemory=TextWriterBuffer->ReserveWriteBuffer(WriteBufferLength);
		if(WriteMemory.Length>2){
			if(WriteBufferLength>WriteMemory.Length){
				WriteBufferLength=WriteMemory.Length;
				CopyLength=WriteMemory.Length-2;
			}
			// line return
			WriteMemory.Pointer[0]=LineRet[0];
			WriteMemory.Pointer[1]=LineRet[1];
			// line text
			CopyLength=TextEditCopyLineText(End.y,0,WriteMemory.Pointer+2,CopyLength);

			TextWriterBuffer->CommitWriteBuffer(WriteBufferLength);
		}
	}
	ClipboardResult=ClipboardWriter->Submit();
	TextDocumentLineEnumReset();

	return ClipboardResult;
}
//---------------------------------------------------------------------------
void bcTextDocumentEdit::CopyToClipboard(void)noexcept(true)
{
	cIndexPoint SelStart,SelEnd;
	if(GetSelection(SelStart,SelEnd)){
		CopyToClipboardValidated(SelStart,SelEnd);
	}
}
//---------------------------------------------------------------------------
void bcTextDocumentEdit::CutToClipboard(void)noexcept(true)
{
	cIndexPoint SelStart,SelEnd;
	if(GetSelection(SelStart,SelEnd)){
		if(CopyToClipboardValidated(SelStart,SelEnd)){
			ActionDelete(false);
		}
	}
}
//---------------------------------------------------------------------------
void bcTextDocumentEdit::PasteFromClipboard(void)noexcept(true)
{
	auto ClipboardReader=cnSystem::QueryClipboardReader(fView);
	auto *TextReadBuffer=ClipboardReader->QueryText();
	if(TextReadBuffer==nullptr)
		return;
	cnRTL::cSeqList<cStringBuffer<uChar16> > PasteLines;
	cArray<const uChar16> TextArray=TextReadBuffer->GatherReadBuffer(0);
	if(TextArray.Length!=0){
		uIntn ReadLength=TextArray.Length;
		if(TextArray.Pointer[ReadLength-1]==0)
			ReadLength--;
		uIntn LineIndex=0;
		uIntn LineEndIndex=0;
		for(uIntn i=0;i<ReadLength;i++){
			if(TextArray.Pointer[i]=='\r'){
				continue;
			}
			if(TextArray.Pointer[i]=='\n'){
				if(LineEndIndex>=LineIndex)
					PasteLines.AppendMake(TextArray.Pointer+LineIndex,LineEndIndex-LineIndex+1);

				LineIndex=i+1;
				LineEndIndex=i;
				continue;
			}
			LineEndIndex=i;
		}
		PasteLines.AppendMake(TextArray.Pointer+LineIndex,TextArray.Length-LineIndex);

		TextReadBuffer->DismissReadBuffer(TextArray.Length);
	}
	if(PasteLines.GetCount()<=1){
		// single line
		ActionInputText(PasteLines[0]->Pointer,PasteLines[0]->Length);
	}
	else{
		// multiple lines
		uIntn FetchIndex=0;
		cFunction<bool (cArray<const uChar16>&)noexcept(true)> FetchProc=
			[&PasteLines,&FetchIndex](cArray<const uChar16> &InsertLine)noexcept(true)
		{
			if(FetchIndex<PasteLines.GetCount()){
				InsertLine=PasteLines[FetchIndex++].Storage();
				return true;
			}
			return false;
		};
			
		ActionInsertLines(FetchProc);
	}
}
//---------------------------------------------------------------------------
bool bcTextDocumentEdit::ActionDelete(bool Backward)noexcept(true)
{
	cIndexPoint SelStart,SelEnd;
	if(GetSelection(SelStart,SelEnd)){
		// has selection, delete selection
		TextEditReplace(SelStart.y,SelStart.x,SelEnd.y,SelEnd.x,0,nullptr);
		// update contents
		ClearCache();
		SetNeedUpdateTextContent();
		// move caret and reset selection
		SetCaretPos(SelStart,false);
		// notify change
		if(OnChange!=nullptr)
			OnChange();
		return true;
	}
	else{
		// no selection, delete one
		if(Backward){
			// delete backward
			auto DeletePos=GetTextPosByOffset(-1);
			if(static_cast<uIntn>(DeletePos.x)==fCaretTextIndex && static_cast<uIntn>(DeletePos.y)==fCaretLineIndex){
				// nothing to delete
				return false;
			}
			// remove text
			if(TextEditReplace(DeletePos.y,DeletePos.x,fCaretLineIndex,fCaretTextIndex,0,nullptr)){
				// move caret
				SetCaretCorrectedPosition(DeletePos,false);

				// update contents
				ClearCache();
				SetNeedUpdateTextContent();
				// notify change
				if(OnChange!=nullptr)
					OnChange();
				return true;
			}
		}
		else{
			// delete forward
			auto DeletePos=GetTextPosByOffset(1);
			if(static_cast<uIntn>(DeletePos.x)==fCaretTextIndex && static_cast<uIntn>(DeletePos.y)==fCaretLineIndex){
				// nothing to delete
				return false;
			}
			// remove text
			if(TextEditReplace(fCaretLineIndex,fCaretTextIndex,DeletePos.y,DeletePos.x,0,nullptr)){
				// update contents
				ClearCache();
				SetNeedUpdateTextContent();
				// notify change
				if(OnChange!=nullptr)
					OnChange();
				return true;
			}
		}
	}
	return false;
}
//---------------------------------------------------------------------------
bool bcTextDocumentEdit::ActionInputText(const uChar16 *InputText,uIntn InputTextLength)noexcept(true)
{
	cIndexPoint SelStart,SelEnd;
	cIndexPoint NewCaretPos;
	if(GetSelection(SelStart,SelEnd)){
		// replace
		if(TextEditReplace(SelStart.y,SelStart.x,SelEnd.y,SelEnd.x,InputTextLength,InputText)==false)
			return false;
		NewCaretPos.y=SelStart.y;
		NewCaretPos.x=SelStart.x+InputTextLength;
	}
	else{
		// insert
		if(TextEditReplace(fCaretLineIndex,fCaretTextIndex,fCaretLineIndex,fCaretTextIndex,InputTextLength,InputText)==false)
			return false;
		NewCaretPos.y=fCaretLineIndex;
		NewCaretPos.x=fCaretTextIndex+InputTextLength;
	}
	// update contents
	ClearCache();
	SetNeedUpdateTextContent();
	// move caret, and clear selection
	SetCaretPos(NewCaretPos,false);
	// notify change
	if(OnChange!=nullptr)
		OnChange();
	return true;
}
//---------------------------------------------------------------------------
bool bcTextDocumentEdit::ActionInsertLines(iFetchLineProcedure *Procedure)noexcept(true)
{
	cIndexPoint SelStart,SelEnd;
	if(GetSelection(SelStart,SelEnd)==false){
		SelStart.y=SelEnd.y=fCaretLineIndex;
		SelStart.x=SelEnd.x=fCaretTextIndex;
	}
	// replace with lines
	cIndexPoint NewCaretPos;
	if(TextEditReplaceWithLines(NewCaretPos,SelStart.y,SelStart.x,SelEnd.y,SelEnd.x,Procedure)==false)
		return false;
	// update contents
	ClearCache();
	SetNeedUpdateTextContent();
	// move caret, and clear selection
	SetCaretPos(NewCaretPos,false);
	// notify change
	if(OnChange!=nullptr)
		OnChange();
	return true;
}
//---------------------------------------------------------------------------
bool bcTextDocumentEdit::ActionNewLine(void)noexcept(true)
{
	cIndexPoint SelStart,SelEnd;
	GetSelection(SelStart,SelEnd);
	// replace
	cFunction<bool (cArray<const uChar16>&)noexcept(true)> FetchProc=
		[FetchIndex=2](cArray<const uChar16> &InsertLine)mutable noexcept(true)
	{
		InsertLine.Pointer=nullptr;
		InsertLine.Length=0;
		if(FetchIndex){
			FetchIndex--;
			return true;
		}
		return false;
	};
	cIndexPoint NewCaretPos;
	if(TextEditReplaceWithLines(NewCaretPos,SelStart.y,SelStart.x,SelEnd.y,SelEnd.x,FetchProc)==false)
		return false;
	// update contents
	ClearCache();
	SetNeedUpdateTextContent();
	// move caret, and clear selection
	SetCaretPos(NewCaretPos,false);
	// notify change
	if(OnChange!=nullptr)
		OnChange();
	return true;
}
//---------------------------------------------------------------------------
void bcTextDocumentEdit::ScrollToCaret(void)noexcept(true)
{
	//UpdateScrollLayout();
	ScrollTextToVisible(fCaretLineIndex,fCaretTextIndex);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcStringListEdit::bcStringListEdit()noexcept(true)
{
	fSelectionLineIndex=0;
	fSelectionTextIndex=0;

	fEnumLineIndex=IndexNotFound;
	SelectionAnnotation=true;

	InsertAnnotator(&fSelectionAnnotator);
}
//---------------------------------------------------------------------------
bcStringListEdit::~bcStringListEdit()noexcept(true)
{
}
//---------------------------------------------------------------------------
void bcStringListEdit::EnableSelectionAnnotator(bool Enable)noexcept(true)
{
	if(Enable){
		InsertAnnotator(&fSelectionAnnotator);
	}
	else{
		RemoveAnnotator(&fSelectionAnnotator);
	}
}
//---------------------------------------------------------------------------
void bcStringListEdit::Update(void)noexcept(true)
{
	ClearCache();
	SetNeedUpdateTextContent();
}
//---------------------------------------------------------------------------
void bcStringListEdit::TextDocumentLineEnumReset(void)noexcept(true)
{
	fEnumLineIndex=IndexNotFound;
}
//---------------------------------------------------------------------------
bool bcStringListEdit::TextDocumentLineEnum(uIntn LineIndex)noexcept(true)
{
	fEnumLineHeight=StringListGetLineHeight();

	auto LineCount=TextEditGetLineCount();
	if(LineIndex>=LineCount){
		return false;
	}
	fEnumLineIndex=LineIndex;
	return true;
}
//---------------------------------------------------------------------------
bool bcStringListEdit::TextDocumentLineEnumNext(void)noexcept(true)
{
	auto LineCount=TextEditGetLineCount();
	if(fEnumLineIndex>=LineCount)
		return false;
	fEnumLineIndex++;
	fEnumLineFetchIndex=IndexNotFound;
	return fEnumLineIndex!=LineCount;
}
//---------------------------------------------------------------------------
Float32 bcStringListEdit::TextDocumentLineEnumHeight(void)noexcept(true)
{
	return fEnumLineHeight;
}
//---------------------------------------------------------------------------
void bcStringListEdit::TextDocumentLineEnumPartEnum(void)noexcept(true)
{
	fEnumLineFetchIndex=0;
	fEnumLineFetchString=StringListGetLine(fEnumLineIndex);
}
//---------------------------------------------------------------------------
bool bcStringListEdit::TextDocumentLineEnumPartFetch(cTextDocumentLinePart &TextPart)noexcept(true)
{
	if(fEnumLineFetchIndex>=fEnumLineFetchString.Length){
		return false;
	}
	uIntn PartTextLength=fEnumLineFetchString.Length-fEnumLineFetchIndex;
	// fetch text style
	TextPart.Text=fEnumLineFetchString.Pointer+fEnumLineFetchIndex;
	TextPart.Length=StringListApplyTextStyle(TextPart.TextStyle,fEnumLineIndex,fEnumLineFetchIndex,false);
	if(TextPart.Length>PartTextLength){
		TextPart.Length=PartTextLength;
	}
	TextPart.TextDistance=nullptr;
	TextPart.TextHeight=0;
	fEnumLineFetchIndex+=TextPart.Length;
	return true;
}
//---------------------------------------------------------------------------
Float32 bcStringListEdit::TextDocumentVerticalAlign(Float32 TotalHeight)noexcept(true)
{
	switch(TextAlign&Alignment::VCenter){
	default:
	//case Alignment::None:
	//case Alignment::Top:
		// align to top
		return 0;
	case Alignment::Bottom:
		// align to bottom
		return fScrollViewportSize[1]-TotalHeight;
	case Alignment::VCenter:
		// align to vertical center
		return (fScrollViewportSize[1]-TotalHeight)/2;
	}
}
//---------------------------------------------------------------------------
Float32 bcStringListEdit::TextDocumentHorizontalAlign(Float32 LineWidth)noexcept(true)
{
	Float32 AlignRight=fScrollViewportSize[0];
	if(AlignRight<fScrollTotalSize[0]){
		AlignRight=fScrollTotalSize[0];
	}
	switch(TextAlign&Alignment::HCenter){
	default:
	//case Alignment::None:
	//case Alignment::Left:
		// align to left
		return 0;
	case Alignment::Right:
		// align to right
		return AlignRight-LineWidth;
	case Alignment::HCenter:
		// align to horizontal center
		return (AlignRight-LineWidth)/2;
	}
}
//---------------------------------------------------------------------------
bcStringListEdit* bcStringListEdit::cSelectionAnnotator::GetHost(void)noexcept(true)
{
	return cnMemory::GetObjectFromMemberPointer(this,&bcStringListEdit::fSelectionAnnotator);
}
//---------------------------------------------------------------------------
void bcStringListEdit::cSelectionAnnotator::Setup(uIntn LineIndex)noexcept(true)
{
	auto Host=GetHost();
	fLineIndex=LineIndex;
	if(Host->SelectionAnnotation){
		Host->GetLineSelection(LineIndex,fSelLeft,fSelRight);
	}
}
//---------------------------------------------------------------------------
void bcStringListEdit::cSelectionAnnotator::Clear(void)noexcept(true)
{
	fSelLeft=fSelRight=0;
}
//---------------------------------------------------------------------------
bool bcStringListEdit::cSelectionAnnotator::Process(uIntn LineTextIndex,cTextDocumentLinePart &TextPart)noexcept(true)
{
	auto Host=GetHost();
	if(GetHost()->SelectionAnnotation==false || LineTextIndex>=fSelRight || fSelLeft>=fSelRight){
		// ignore or no selection
		return false;
	}
	uIntn PartTextLength;
	if(LineTextIndex<fSelLeft){
		// left part, not in selection
		PartTextLength=fSelLeft-LineTextIndex;
	}
	else{	// LineTextIndex<fSelRight
		// selected part
		PartTextLength=fSelRight-LineTextIndex;
		// selection style
		Host->StringListApplyTextStyle(TextPart.TextStyle,fLineIndex,LineTextIndex,true);
	}

	// check if need to divide part
	if(TextPart.Length>PartTextLength){
		TextPart.Length=PartTextLength;
	}
	return false;
}
//---------------------------------------------------------------------------
bool bcStringListEdit::cSelectionAnnotator::Fetch(uIntn&,cTextDocumentLinePart&)noexcept(true)
{
	return false;
}
//---------------------------------------------------------------------------
uIntn bcStringListEdit::TextEditCopyLineText(uIntn LineIndex,uIntn TextIndex,uChar16 *Dest,uIntn Length)noexcept(true)
{
	auto LineText=StringListGetLine(LineIndex);
	if(TextIndex>=LineText.Length)
		return 0;

	uIntn CopyLength=LineText.Length-TextIndex;
	if(CopyLength>Length){
		CopyLength=Length;
	}
	cnMemory::Copy(Dest,LineText.Pointer+TextIndex,CopyLength*sizeof(uChar16));
	return CopyLength;
}
//---------------------------------------------------------------------------
cIndexPoint bcStringListEdit::TextEditGetSelectionPoint(void)const noexcept(true)
{
	cIndexPoint SelectionPoint;
	SelectionPoint.y=fSelectionLineIndex;
	SelectionPoint.x=fSelectionTextIndex;
	return SelectionPoint;
}
//---------------------------------------------------------------------------
void bcStringListEdit::TextEditMoveCaretAndSelectionPoint(uIntn CaretLineIndex,uIntn CaretTextIndex,uIntn SelectionLineIndex,uIntn SelectionTextIndex)noexcept(true)
{
	cIndexPoint PreviousSelectionBegin;
	cIndexPoint PreviousSelectionEnd;
	bool PreviousHasSelection=GetSelection(PreviousSelectionBegin,PreviousSelectionEnd);

	bool CaretChangedFlag=( fCaretLineIndex!=CaretLineIndex || fCaretTextIndex!=CaretTextIndex );

	if(CaretChangedFlag){
		fCaretLineIndex=CaretLineIndex;
		fCaretTextIndex=CaretTextIndex;
	}

	fSelectionLineIndex=SelectionLineIndex;
	fSelectionTextIndex=SelectionTextIndex;

	cIndexPoint NewSelectionBegin;
	cIndexPoint NewSelectionEnd;
	bool NewHasSelection=GetSelection(NewSelectionBegin,NewSelectionEnd);
	bool SelectionChanged;
	if(NewHasSelection){
		// keep selection
		if(PreviousSelectionBegin.x==NewSelectionBegin.x && PreviousSelectionBegin.y==NewSelectionBegin.y &&
			PreviousSelectionEnd.x==NewSelectionEnd.x && PreviousSelectionEnd.y==NewSelectionEnd.y)
		{
			SelectionChanged=false;
		}
		else{
			SelectionChanged=true;
		}
	}
	else{
		// reset selection
		SelectionChanged=PreviousHasSelection;
	}
	if(CaretChangedFlag){
		// scroll to new caret pos
		ScrollToCaret();
	}
	if(SelectionChanged){
		// update selection
		ClearCache();
		SetNeedUpdateTextContent();
	}
	 if(CaretChangedFlag){
		// selection no change, update caret state only
		CaretChanged();
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcTextListEdit::bcTextListEdit()noexcept(true)
{
	gApplyDefaultTextStyle(TextStyle);
	SelectedTextColor=UIColorFromUInt32(0xFFFFFFFF);
	SelectedBackgroundColor=UIColorFromUInt32(0xFFD77800);
	
	SelectedBlurTextColor=TextStyle.TextColor;
	SelectedBlurBackgroundColor=UIColorFromUInt32(0xFFD9D9D9);
}
//---------------------------------------------------------------------------
bcTextListEdit::~bcTextListEdit()noexcept(true)
{
}
//---------------------------------------------------------------------------
Float32 bcTextListEdit::StringListGetLineHeight(void)noexcept(true)
{
	Float32 Value=LineHeight;
	if(Value==0){
		Value=TextStyle.FontSize+2;
	}
	return Value;
}
//---------------------------------------------------------------------------
uIntn bcTextListEdit::StringListApplyTextStyle(cUITextStyle &Style,uIntn LineIndex,uIntn TextIndex,bool Selected)noexcept(true)
{
	Style=TextStyle;

	auto LineLength=TextEditGetLineLength(LineIndex);
	if(TextIndex>=LineLength)
		return 0;

	if(Selected){
		if(fControlFocused){
			if((SelectedTextColor.Alpha)!=0){
				Style.TextColor=SelectedTextColor;
			}
			if((SelectedBackgroundColor.Alpha)!=0){
				Style.BackgroundColor=SelectedBackgroundColor;
			}
		}
		else if(HideSelectionWhenBlur==false){
			if((SelectedBlurTextColor.Alpha)!=0){
				Style.TextColor=SelectedBlurTextColor;
			}
			if((SelectedBlurBackgroundColor.Alpha)!=0){
				Style.BackgroundColor=SelectedBlurBackgroundColor;
			}
		}
	}
	return LineLength-TextIndex;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cTextListEdit::cTextListEdit()noexcept(true)
{
	TextList.Append();
}
//---------------------------------------------------------------------------
cTextListEdit::~cTextListEdit()noexcept(true)
{
}
//---------------------------------------------------------------------------
uIntn cTextListEdit::TextEditGetLineCount(void)noexcept(true)
{
	return TextList.GetCount();
}
//---------------------------------------------------------------------------
uIntn cTextListEdit::TextEditGetLineLength(uIntn LineIndex)noexcept(true)
{
	if(LineIndex>=TextList.GetCount()){
		return 0;
	}

	return TextList[LineIndex]->Length;
}
//---------------------------------------------------------------------------
cArray<const uChar16> cTextListEdit::StringListGetLine(uIntn LineIndex)noexcept(true)
{
	if(LineIndex>=TextList.GetCount()){
		cArray<const uChar16> EmptyArray;
		EmptyArray.Pointer=nullptr;
		EmptyArray.Length=0;
		return EmptyArray;
	}

	return TextList[LineIndex].Storage();
}
//---------------------------------------------------------------------------
bool cTextListEdit::TextEditReplace(uIntn LineIndex,uIntn TextIndex,uIntn LineEndIndex,uIntn TextEndIndex,uIntn ReplacementLength,const uChar16 *Replacement)noexcept(true)
{
	if(ReadOnly)
		return false;
	if(LineIndex>LineEndIndex){
		return false;
	}
	if(LineIndex>=TextList.GetCount()){
		return false;
	}
	if(LineIndex==LineEndIndex){
		auto &Text=TextList[LineIndex];
	
		Text.Replace(TextIndex,TextEndIndex-TextIndex,ReplacementLength,Replacement);
	}
	else{
		auto &Text=TextList[LineIndex];
		Text.Replace(TextIndex,Text->Length-TextIndex,ReplacementLength,Replacement);
		
		auto &LastLineText=TextList[LineEndIndex];
		Text.Append(LastLineText->Pointer+TextEndIndex,LastLineText->Length-TextEndIndex);

		TextList.RemoveAt(LineIndex+1,LineEndIndex-LineIndex);
	}
	return true;
}
//---------------------------------------------------------------------------
bool cTextListEdit::TextEditReplaceWithLines(cIndexPoint &InsertEndPos,uIntn LineIndex,uIntn TextIndex,uIntn LineEndIndex,uIntn TextEndIndex,iFetchLineProcedure *FetchProcedure)noexcept(true)
{
	if(ReadOnly)
		return false;
	uIntn LineCount=TextList.GetCount();
	if(LineEndIndex>=LineCount){
		LineEndIndex=LineCount-1;
		TextEndIndex=TextList[LineEndIndex]->Length;
	}
	if(LineIndex>LineEndIndex){
		return false;
	}
	uIntn InsertIndex=LineIndex+1;
	cArray<const uChar16> InsertLine;
	if(FetchProcedure==nullptr || FetchProcedure->Execute(InsertLine)==false){
		// insert nothing
		return TextEditReplace(LineIndex,TextIndex,LineEndIndex,TextEndIndex,0,nullptr);
	}
	cStringBuffer<uChar16> LastLineTail;
	if(LineIndex==LineEndIndex){
		auto &Line=TextList[LineIndex];
		if(LineEndIndex<Line->Length){
			// copy line tail
			LastLineTail.SetString(Line->Pointer+TextEndIndex,Line->Length-TextEndIndex);
			// cut line
			if(TextIndex>TextEndIndex){
				TextIndex=TextEndIndex;
			}
			Line.SetLength(TextIndex);
		}
		else if(LineIndex<Line->Length){
			// cut line tail and leave new line empty
			Line.SetLength(TextIndex);
		}
		// append first insert line
		Line.Append(InsertLine.Pointer,InsertLine.Length);
	}
	else{
		// keep first line head
		auto &FirstLine=TextList[LineIndex];
		FirstLine.SetLength(TextIndex);
		// append first insert line
		FirstLine.Append(InsertLine.Pointer,InsertLine.Length);
		// save last line tail
		LastLineTail=cnVar::MoveCast(TextList[LineEndIndex]);
		LastLineTail.Replace(0,TextEndIndex,0,nullptr);

		// remove lines between
		TextList.RemoveAt(InsertIndex,LineEndIndex-LineIndex);

	}
	// insert lines
	while(FetchProcedure->Execute(InsertLine)){
		// inser new lines
		TextList.InsertMakeAt(InsertIndex,cStringBuffer<uChar16>(InsertLine.Pointer,InsertLine.Length));
		InsertIndex++;
	}
	// get insert end position
	InsertEndPos.y=InsertIndex-1;
	InsertEndPos.x=TextList[InsertIndex-1].GetLength();
	// append tail to last inserted line
	TextList[InsertIndex-1].Append(LastLineTail->Pointer,LastLineTail->Length);
	return true;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cTextEdit::cTextEdit()noexcept(true)
{
}
//---------------------------------------------------------------------------
cTextEdit::~cTextEdit()noexcept(true)
{
}
//---------------------------------------------------------------------------
uIntn cTextEdit::TextEditGetLineCount(void)noexcept(true)
{
	return 1;
}
//---------------------------------------------------------------------------
uIntn cTextEdit::TextEditGetLineLength(uIntn LineIndex)noexcept(true)
{
	if(LineIndex!=0){
		return 0;
	}

	return Text->Length;
}
//---------------------------------------------------------------------------
cArray<const uChar16> cTextEdit::StringListGetLine(uIntn LineIndex)noexcept(true)
{
	if(LineIndex!=0){
		cArray<const uChar16> EmptyArray;
		EmptyArray.Pointer=nullptr;
		EmptyArray.Length=0;
		return EmptyArray;
	}

	return Text.Storage();
}
//---------------------------------------------------------------------------
bool cTextEdit::TextEditReplace(uIntn LineIndex,uIntn TextIndex,uIntn LineEndIndex,uIntn TextEndIndex,uIntn ReplacementLength,const uChar16 *Replacement)noexcept(true)
{
	if(ReadOnly)
		return false;
	if(LineIndex!=0)
		return false;

	if(LineEndIndex!=0){
		TextEndIndex=Text->Length;
	}

	Text.Replace(TextIndex,TextEndIndex-TextIndex,ReplacementLength,Replacement);
	return true;
}
//---------------------------------------------------------------------------
bool cTextEdit::TextEditReplaceWithLines(cIndexPoint &InsertEndPos,uIntn LineIndex,uIntn TextIndex,uIntn LineEndIndex,uIntn TextEndIndex,iFetchLineProcedure *FetchProcedure)noexcept(true)
{
	if(ReadOnly)
		return false;
	if(LineIndex!=0)
		return false;
	if(LineEndIndex!=0){
		TextEndIndex=Text.GetLength();
	}
	else if(TextEndIndex>Text.GetLength()){
		TextEndIndex=Text.GetLength();
	}
	cArray<const uChar16> InsertLine;
	if(FetchProcedure==nullptr || FetchProcedure->Execute(InsertLine)==false){
		InsertLine.Pointer=nullptr;
		InsertLine.Length=0;
	}
	Text.Replace(TextIndex,TextEndIndex-TextIndex,InsertLine.Length,InsertLine.Pointer);
	InsertEndPos.y=0;
	InsertEndPos.x=TextIndex+InsertLine.Length;
	return true;
}
//---------------------------------------------------------------------------
