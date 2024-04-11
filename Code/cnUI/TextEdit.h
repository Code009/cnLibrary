/*- cnUI - Text Edit Control ----------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2009-03-22                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnUI_TextEdit_H__
#define __cnLibrary_cnUI_TextEdit_H__
/*-------------------------------------------------------------------------*/
#include <cnUI/ControlBase.h>
#include <cnUI/ScrollContainer.h>
#include <cnUI/TextControl.h>

#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnUI{
//---------------------------------------------------------------------------
class cnLib_INTERFACE viTextCaretData : public viData
{
public:
	cFunctionSet<void (void)noexcept(true)> TextCaretNotifySet;
	virtual bool TextCaretVisualPosition(cUIPoint &CaretPos,cUIPoint &CaretSize)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class vTextCaret : public vSimpleViewPainter
{
public:
	vTextCaret(viTextCaretData *Data=nullptr)noexcept(true);
	~vTextCaret()noexcept(true);

	static rPtr<viControl> Create(viTextCaretData *Data)noexcept(true);

	viTextCaretData* GetData(void)const noexcept(true);
	void SetData(viTextCaretData *Data)noexcept(true);

protected:
	dPtr<viTextCaretData> fData;
	void Update(void)noexcept(true);

	virtual void cnLib_FUNC Paint(iUISimplePaintContext *Context)noexcept(true)override;
private:

	void DataInsertCallback(void)noexcept(true);
	void DataRemoveCallback(void)noexcept(true);

	iFunctionToken *fTextCaretNotifyToken;
};
//---------------------------------------------------------------------------
extern rPtr<viControl> (*gCreateDefaultTextCaretVisual)(viTextCaretData *Data)noexcept(true);
//---------------------------------------------------------------------------
class bcTextDocumentEdit : public bcTextDocument, protected KeyHandler, protected MouseHandler, protected TouchHandler,
	protected iUITextInputHandler,protected iUITextCompositionViewInfo,
	public viControlStateData, public viTextCaretData
{
public:
	bcTextDocumentEdit()noexcept(true);
	~bcTextDocumentEdit()noexcept(true);
	
	void EnableCompositionAnnotator(bool Enable)noexcept(true);

	Float32 CaretContentZPosition=ZPosition_Content+1;
	void SetCaretContent(rPtr<viControl> Content)noexcept(true);
	template<class TVisual>
	void SetCaretContent(TVisual &Visual)noexcept(true){
		SetCaretContent(rCreate< bwvControl<TVisual&> >(Visual));
	}
	void UpdateZIndex(void)noexcept(true);

	iPtr<iInterface> MouseCursor;

	// viControlStateData

	virtual bool ControlHot(void)noexcept(true)override;
	virtual bool ControlFocused(void)noexcept(true)override;
	virtual bool ControlDisabled(void)noexcept(true)override;

	// viTextCaretData

	virtual bool TextCaretVisualPosition(cUIPoint &CaretPos,cUIPoint &CaretSize)noexcept(true)override;

	// Events

	cFunction<void (void)noexcept(true)> OnChange;

	// caret

	cIndexPoint GetCaretPos(void)noexcept(true);
	void SetCaretPos(cIndexPoint CaretPos,bool KeepSelection)noexcept(true);
	void ScrollToCaret(void)noexcept(true);

	// selection

	void SelectAll(void)noexcept(true);
	bool GetSelection(cIndexPoint &Begin,cIndexPoint &End)const noexcept(true);
	void GetLineSelection(uIntn LineIndex,uIntn &Left,uIntn &Right)noexcept(true);
	//void SelectText(sIntn SelStart,sIntn NewCaretPos)noexcept(true);

	// text layout

	cIndexPoint GetTextPosByOffset(sIntn Offset)noexcept(true);

	// clipboard

	virtual void CopyToClipboard(void)noexcept(true);
	virtual void CutToClipboard(void)noexcept(true);
	virtual void PasteFromClipboard(void)noexcept(true);

	// edit action

	bool ActionDelete(bool Backward)noexcept(true);
	bool ActionInputText(const uChar16 *Text,uIntn Length)noexcept(true);
	bool ActionNewLine(void)noexcept(true);

	typedef iFunction<bool (cArray<const uChar16>&Line)noexcept(true)> iFetchLineProcedure;
	bool ActionInsertLines(iFetchLineProcedure *Procedure)noexcept(true);
protected:
	bool fControlHot		:1;
	bool fControlFocused	:1;

	bool fLBtnDown=false;
	struct{
		bool Crtl:1;
		bool Shift:1;
		bool Alt:1;
	}fKeyDown;

	virtual void ViewSetup(void)noexcept(true)override;
	virtual void ViewClear(void)noexcept(true)override;

	// key

	virtual void cnLib_FUNC KeyFocusEnter(iUIKeyEvent *KeyEvent)noexcept(true)override;
	virtual void cnLib_FUNC KeyFocusLeave(iUIKeyEvent *KeyEvent)noexcept(true)override;
	virtual void cnLib_FUNC KeyDown(iUIKeyEvent *KeyEvent,eKeyCode KeyCode)noexcept(true)override;
	virtual void cnLib_FUNC KeyUp(iUIKeyEvent *KeyEvent,eKeyCode KeyCode)noexcept(true)override;
	virtual void cnLib_FUNC KeyInput(iUIKeyEvent *KeyEvent,eKeyCode KeyCode)noexcept(true)override;

	// iUITextInputHandler

	virtual void cnLib_FUNC TextInput(const uChar16 *Text,uIntn Length)noexcept(true)override;
	virtual void cnLib_FUNC TextCompositionStarted(void)noexcept(true)override;
	virtual void cnLib_FUNC TextCompositionFinished(void)noexcept(true)override;
	virtual void cnLib_FUNC TextCompositionUpdate(sfInt32 CaretPos,const uChar16 *Text,uIntn TextLength)noexcept(true)override;
	
	// iUITextCompositionViewInfo

	virtual void cnLib_FUNC TextCompositionViewRect(cUIPoint &Position,cUIPoint &Size,Float32 &LineHeight)noexcept(true)override;
	virtual cUIPoint cnLib_FUNC TextCompositionCaretPosition(uIntn CompositionTextIndex)noexcept(true)override;

	// mouse

	virtual void cnLib_FUNC MouseEnter(iUIMouseEvent *MouseEvent)noexcept(true)override;
	virtual void cnLib_FUNC MouseLeave(iUIMouseEvent *MouseEvent)noexcept(true)override;
	virtual iInterface* cnLib_FUNC MouseGetCursor(iUIMouseEvent *MouseEvent)noexcept(true)override;
	virtual void cnLib_FUNC MouseMove(iUIMouseEvent *MouseEvent)noexcept(true)override;
	virtual void cnLib_FUNC MouseDown(iUIMouseEvent *MouseEvent,eMouseButton Button)noexcept(true)override;
	virtual void cnLib_FUNC MouseUp(iUIMouseEvent *MouseEvent,eMouseButton Button)noexcept(true)override;

	// content

	virtual void ControlStateChanged(void)noexcept(true);
	virtual void CaretChanged(void)noexcept(true);

	rPtr<viControl> fCaretContent;
	virtual void ControlCaretContentSetDefault(void)noexcept(true);

	// bcTextDocument
	virtual void ScrollContentUpdateContentLayout(void)noexcept(true);

	// edit

	virtual void TextEditApplyCompositionPartStyle(cUITextStyle &Style)noexcept(true);
	virtual cIndexPoint TextEditGetSelectionPoint(void)const noexcept(true);
	virtual void TextEditMoveCaretAndSelectionPoint(uIntn CaretLineIndex,uIntn CaretTextIndex,uIntn SelectionLineIndex,uIntn SelectionTextIndex)noexcept(true);
	virtual uIntn TextEditGetLineCount(void)noexcept(true)=0;
	virtual uIntn TextEditGetLineLength(uIntn LineIndex)noexcept(true)=0;
	virtual uIntn TextEditCopyLineText(uIntn LineIndex,uIntn TextIndex,uChar16 *Dest,uIntn Length)noexcept(true)=0;
	virtual bool TextEditReplace(uIntn LineIndex,uIntn TextIndex,uIntn LineEndIndex,uIntn TextEndIndex,uIntn ReplacementLength,const uChar16 *Replacement)noexcept(true)=0;
	virtual bool TextEditReplaceWithLines(cIndexPoint &InsertEndPos,uIntn LineIndex,uIntn TextIndex,uIntn LineEndIndex,uIntn TextEndIndex,iFetchLineProcedure *FetchProcedure)noexcept(true)=0;
	
	// caret
	
	uIntn fCaretLineIndex;
	uIntn fCaretTextIndex;

	void SetCaretCorrectedPosition(cIndexPoint CaretPos,bool WithSelection)noexcept(true);
	cIndexPoint ClipCaretPosition(cIndexPoint CaretPos)noexcept(true);
	void CaretMoveLine(bool Upward,bool WithSelection)noexcept(true);
	void CaretMoveText(bool Backward,bool WithSelection)noexcept(true);
	void CaretMoveTextEnd(bool Backward,bool WithSelection)noexcept(true);
	void CaretMoveLineEnd(bool Upward,bool WithSelection)noexcept(true);

private:

	void PositCaret(cUIPoint Pos,bool KeepSelection)noexcept(true);
	bool CopyToClipboardValidated(cIndexPoint Begin,cIndexPoint End)noexcept(true);

	Float32 CalculateCompositionCaretTextContentX(uIntn LineIndex,uIntn CompositionTextIndex)noexcept(true);

	// composition
	bool fCompositionState=false;
	rPtr<iUITextLineLayout> fCompositionTextLayout;
	cStringBuffer<uChar16> fCompositionString;
	sfInt32 fCompositionCaretPos;

	sfInt32 GetCompositionCaretPos(void)noexcept(true);

	class cCompositionAnnotator : public iAnnotator
	{
		bcTextDocumentEdit* GetHost(void)noexcept(true);
	public:
		virtual void Setup(uIntn LineIndex)noexcept(true)override;
		virtual void Clear(void)noexcept(true)override;
		virtual bool Process(uIntn LineTextIndex,cTextDocumentLinePart &TextPart)noexcept(true)override;
		virtual bool Fetch(uIntn &AnnotateIndex,cTextDocumentLinePart &TextPart)noexcept(true)override;
	private:
		bool fEnumDone;
		uIntn fEnumCaretIndex;
	}fCompositionAnnotator;
};
//---------------------------------------------------------------------------
class bcStringListEdit : public bcTextDocumentEdit
{
public:
	bcStringListEdit()noexcept(true);
	~bcStringListEdit()noexcept(true);
	eAlignment TextAlign=Alignment::TopLeft;

	void Update(void)noexcept(true);

	void EnableSelectionAnnotator(bool Enable)noexcept(true);
protected:
	bool SelectionAnnotation;

	// bcTextDocument

	virtual void TextDocumentLineEnumReset(void)noexcept(true)final;
	virtual bool TextDocumentLineEnum(uIntn LineIndex)noexcept(true)final;
	virtual bool TextDocumentLineEnumNext(void)noexcept(true)final;
	virtual Float32 TextDocumentLineEnumHeight(void)noexcept(true)final;

	virtual void TextDocumentLineEnumPartEnum(void)noexcept(true)final;
	virtual bool TextDocumentLineEnumPartFetch(cTextDocumentLinePart &TextPart)noexcept(true)final;

	virtual Float32 TextDocumentVerticalAlign(Float32 TotalHeight)noexcept(true)override;
	virtual Float32 TextDocumentHorizontalAlign(Float32 LineWidth)noexcept(true)override;

	// bcTextDocumentEdit

	virtual cIndexPoint TextEditGetSelectionPoint(void)const noexcept(true)override;
	virtual void TextEditMoveCaretAndSelectionPoint(uIntn LineIndex,uIntn TextIndex,uIntn PreviousCaretLineIndex,uIntn PreviousCaretTextIndex)noexcept(true)override;
	virtual uIntn TextEditCopyLineText(uIntn LineIndex,uIntn TextIndex,uChar16 *Dest,uIntn Length)noexcept(true)override;

	// child class

	virtual Float32 StringListGetLineHeight(void)noexcept(true)=0;
public:
	virtual cArray<const uChar16> StringListGetLine(uIntn LineIndex)noexcept(true)=0;
protected:
	virtual uIntn StringListApplyTextStyle(cUITextStyle &Style,uIntn LineIndex,uIntn TextIndex,bool Selected)noexcept(true)=0;

	// selection

	uIntn fSelectionLineIndex;
	uIntn fSelectionTextIndex;


private:
	// text enum
	uIntn fEnumLineIndex;
	Float32 fEnumLineHeight;

	// line enum
	uIntn fEnumLineFetchIndex;
	cArray<const uChar16> fEnumLineFetchString;

	// selection annotation

	class cSelectionAnnotator : public iAnnotator
	{
		bcStringListEdit* GetHost(void)noexcept(true);
	public:
		virtual void Setup(uIntn LineIndex)noexcept(true)override;
		virtual void Clear(void)noexcept(true)override;
		virtual bool Process(uIntn LineTextIndex,cTextDocumentLinePart &TextPart)noexcept(true)override;
		virtual bool Fetch(uIntn &AnnotateIndex,cTextDocumentLinePart &TextPart)noexcept(true)override;
	private:
		uIntn fLineIndex;
		uIntn fSelLeft;
		uIntn fSelRight;
	}fSelectionAnnotator;

};
//---------------------------------------------------------------------------
class bcTextListEdit : public bcStringListEdit
{
public:
	bcTextListEdit()noexcept(true);
	~bcTextListEdit()noexcept(true);

	Float32 LineHeight=0;

	cUITextStyle TextStyle;
	cUIColor SelectedTextColor;
	cUIColor SelectedBackgroundColor;
	cUIColor SelectedBlurTextColor;
	cUIColor SelectedBlurBackgroundColor;
	
	bool HideSelectionWhenBlur=false;
	bool ReadOnly=false;
	
protected:

	// bcStringListEdit

	virtual Float32 StringListGetLineHeight(void)noexcept(true)override;
	virtual uIntn StringListApplyTextStyle(cUITextStyle &Style,uIntn LineIndex,uIntn TextIndex,bool Selected)noexcept(true)override;
};
//---------------------------------------------------------------------------
class cTextListEdit : public bcTextListEdit
{
public:
	cTextListEdit()noexcept(true);
	~cTextListEdit()noexcept(true);

	cnRTL::cSeqList< cStringBuffer<uChar16> > TextList;
	
protected:

	// bcStringListEdit

	virtual cArray<const uChar16> StringListGetLine(uIntn LineIndex)noexcept(true)override;

	// bcTextLinesEdit

	virtual uIntn TextEditGetLineCount(void)noexcept(true)override;
	virtual uIntn TextEditGetLineLength(uIntn LineIndex)noexcept(true)override;
	virtual bool TextEditReplace(uIntn LineIndex,uIntn TextIndex,uIntn LineEndIndex,uIntn TextEndIndex,uIntn ReplacementLength,const uChar16 *Replacement)noexcept(true)override;
	virtual bool TextEditReplaceWithLines(cIndexPoint &InsertEndPos,uIntn LineIndex,uIntn TextIndex,uIntn LineEndIndex,uIntn TextEndIndex,iFetchLineProcedure *FetchProcedure)noexcept(true)override;

};
//---------------------------------------------------------------------------
class cTextEdit : public bcTextListEdit
{
public:
	cTextEdit()noexcept(true);
	~cTextEdit()noexcept(true);

	cStringBuffer<uChar16> Text;
	
protected:

	// bcStringListEdit

	virtual cArray<const uChar16> StringListGetLine(uIntn LineIndex)noexcept(true)override;

	// bcTextLinesEdit

	virtual uIntn TextEditGetLineCount(void)noexcept(true)override;
	virtual uIntn TextEditGetLineLength(uIntn LineIndex)noexcept(true)override;
	virtual bool TextEditReplace(uIntn LineIndex,uIntn TextIndex,uIntn LineEndIndex,uIntn TextEndIndex,uIntn ReplacementLength,const uChar16 *Replacement)noexcept(true)override;
	virtual bool TextEditReplaceWithLines(cIndexPoint &InsertEndPos,uIntn LineIndex,uIntn TextIndex,uIntn LineEndIndex,uIntn TextEndIndex,iFetchLineProcedure *FetchProcedure)noexcept(true)override;
};
//---------------------------------------------------------------------------
}	// namespace cnUI
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif
