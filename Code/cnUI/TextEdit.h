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
	cFunctionSet<void (void)> TextCaretNotifySet;
	virtual bool TextCaretVisualPosition(cUIPoint &CaretPos,cUIPoint &CaretSize)=0;
};
//---------------------------------------------------------------------------
class vTextCaret : public vSimpleViewPainter
{
public:
	vTextCaret(viTextCaretData *Data=nullptr);
	~vTextCaret();

	static rPtr<viControl> Create(viTextCaretData *Data);

	viTextCaretData* GetData(void)const;
	void SetData(viTextCaretData *Data);

protected:
	dPtr<viTextCaretData> fData;
	void Update(void);

	virtual void cnLib_FUNC Paint(iUISimplePaintContext *Context)override;
private:

	void DataInsertCallback(void);
	void DataRemoveCallback(void);

	iFunctionToken *fTextCaretNotifyToken;
};
//---------------------------------------------------------------------------
extern rPtr<viControl> (*gCreateDefaultTextCaretVisual)(viTextCaretData *Data);
//---------------------------------------------------------------------------
class bcTextDocumentEdit : public bcTextDocument, protected KeyHandler, protected MouseHandler, protected TouchHandler,
	protected iUITextInputHandler,protected iUITextCompositionViewInfo,
	public viControlStateData, public viTextCaretData
{
public:
	bcTextDocumentEdit();
	~bcTextDocumentEdit();
	
	void EnableCompositionAnnotator(bool Enable);

	Float32 CaretContentZPosition=ZPosition_Content+1;
	void SetCaretContent(rPtr<viControl> Content);
	template<class TVisual>
	void SetCaretContent(TVisual &Visual){
		SetCaretContent(rCreate< bwvControl<TVisual&> >(Visual));
	}
	void UpdateZIndex(void);

	iPtr<iInterface> MouseCursor;

	// viControlStateData

	virtual bool ControlHot(void)override;
	virtual bool ControlFocused(void)override;
	virtual bool ControlDisabled(void)override;

	// viTextCaretData

	virtual bool TextCaretVisualPosition(cUIPoint &CaretPos,cUIPoint &CaretSize)override;

	// Events

	cFunction<void (void)> OnChange;

	// caret

	cIndexPoint GetCaretPos(void);
	void SetCaretPos(cIndexPoint CaretPos,bool KeepSelection);
	void ScrollToCaret(void);

	// selection

	void SelectAll(void);
	bool GetSelection(cIndexPoint &Begin,cIndexPoint &End)const;
	void GetLineSelection(uIntn LineIndex,uIntn &Left,uIntn &Right);
	//void SelectText(sIntn SelStart,sIntn NewCaretPos);

	// text layout

	cIndexPoint GetTextPosByOffset(sIntn Offset);

	// clipboard

	virtual void CopyToClipboard(void);
	virtual void CutToClipboard(void);
	virtual void PasteFromClipboard(void);

	// edit action

	bool ActionDelete(bool Backward);
	bool ActionInputText(const uChar16 *Text,uIntn Length);
	bool ActionNewLine(void);

	typedef iFunction<bool (cArray<const uChar16>&Line)> iFetchLineProcedure;
	bool ActionInsertLines(iFetchLineProcedure *Procedure);
protected:
	bool fControlHot		:1;
	bool fControlFocused	:1;

	bool fLBtnDown=false;
	struct{
		bool Crtl:1;
		bool Shift:1;
		bool Alt:1;
	}fKeyDown;

	virtual void ViewSetup(void)override;
	virtual void ViewClear(void)override;

	// key

	virtual void cnLib_FUNC KeyFocusEnter(iUIKeyEvent *KeyEvent)override;
	virtual void cnLib_FUNC KeyFocusLeave(iUIKeyEvent *KeyEvent)override;
	virtual void cnLib_FUNC KeyDown(iUIKeyEvent *KeyEvent,eKeyCode KeyCode)override;
	virtual void cnLib_FUNC KeyUp(iUIKeyEvent *KeyEvent,eKeyCode KeyCode)override;
	virtual void cnLib_FUNC KeyInput(iUIKeyEvent *KeyEvent,eKeyCode KeyCode)override;

	// iUITextInputHandler

	virtual void cnLib_FUNC TextInput(const uChar16 *Text,uIntn Length)override;
	virtual void cnLib_FUNC TextCompositionStarted(void)override;
	virtual void cnLib_FUNC TextCompositionFinished(void)override;
	virtual void cnLib_FUNC TextCompositionUpdate(sfInt32 CaretPos,const uChar16 *Text,uIntn TextLength)override;
	
	// iUITextCompositionViewInfo

	virtual void cnLib_FUNC TextCompositionViewRect(cUIPoint &Position,cUIPoint &Size,Float32 &LineHeight)override;
	virtual cUIPoint cnLib_FUNC TextCompositionCaretPosition(uIntn CompositionTextIndex)override;

	// mouse

	virtual void cnLib_FUNC MouseEnter(iUIMouseEvent *MouseEvent)override;
	virtual void cnLib_FUNC MouseLeave(iUIMouseEvent *MouseEvent)override;
	virtual iInterface* cnLib_FUNC MouseGetCursor(iUIMouseEvent *MouseEvent)override;
	virtual void cnLib_FUNC MouseMove(iUIMouseEvent *MouseEvent)override;
	virtual void cnLib_FUNC MouseDown(iUIMouseEvent *MouseEvent,eMouseButton Button)override;
	virtual void cnLib_FUNC MouseUp(iUIMouseEvent *MouseEvent,eMouseButton Button)override;

	// content

	virtual void ControlStateChanged(void);
	virtual void CaretChanged(void);

	rPtr<viControl> fCaretContent;
	virtual void ControlCaretContentSetDefault(void);

	// bcTextDocument
	virtual void ScrollContentUpdateContentLayout(void);

	// edit

	virtual void TextEditApplyCompositionPartStyle(cUITextStyle &Style);
	virtual cIndexPoint TextEditGetSelectionPoint(void)const;
	virtual void TextEditMoveCaretAndSelectionPoint(uIntn CaretLineIndex,uIntn CaretTextIndex,uIntn SelectionLineIndex,uIntn SelectionTextIndex);
	virtual uIntn TextEditGetLineCount(void)=0;
	virtual uIntn TextEditGetLineLength(uIntn LineIndex)=0;
	virtual uIntn TextEditCopyLineText(uIntn LineIndex,uIntn TextIndex,uChar16 *Dest,uIntn Length)=0;
	virtual bool TextEditReplace(uIntn LineIndex,uIntn TextIndex,uIntn LineEndIndex,uIntn TextEndIndex,uIntn ReplacementLength,const uChar16 *Replacement)=0;
	virtual bool TextEditReplaceWithLines(cIndexPoint &InsertEndPos,uIntn LineIndex,uIntn TextIndex,uIntn LineEndIndex,uIntn TextEndIndex,iFetchLineProcedure *FetchProcedure)=0;
	
	// caret
	
	uIntn fCaretLineIndex;
	uIntn fCaretTextIndex;

	void SetCaretCorrectedPosition(cIndexPoint CaretPos,bool WithSelection);
	cIndexPoint ClipCaretPosition(cIndexPoint CaretPos);
	void CaretMoveLine(bool Upward,bool WithSelection);
	void CaretMoveText(bool Backward,bool WithSelection);
	void CaretMoveTextEnd(bool Backward,bool WithSelection);
	void CaretMoveLineEnd(bool Upward,bool WithSelection);

private:

	void PositCaret(cUIPoint Pos,bool KeepSelection);
	bool CopyToClipboardValidated(cIndexPoint Begin,cIndexPoint End);

	Float32 CalculateCompositionCaretTextContentX(uIntn LineIndex,uIntn CompositionTextIndex);

	// composition
	bool fCompositionState=false;
	rPtr<iUITextLineLayout> fCompositionTextLayout;
	cStringBuffer<uChar16> fCompositionString;
	sfInt32 fCompositionCaretPos;

	sfInt32 GetCompositionCaretPos(void);

	class cCompositionAnnotator : public iAnnotator
	{
		bcTextDocumentEdit* GetHost(void);
	public:
		virtual void Setup(uIntn LineIndex)override;
		virtual void Clear(void)override;
		virtual bool Process(uIntn LineTextIndex,cTextDocumentLinePart &TextPart)override;
		virtual bool Fetch(uIntn &AnnotateIndex,cTextDocumentLinePart &TextPart)override;
	private:
		bool fEnumDone;
		uIntn fEnumCaretIndex;
	}fCompositionAnnotator;
};
//---------------------------------------------------------------------------
class bcStringListEdit : public bcTextDocumentEdit
{
public:
	bcStringListEdit();
	~bcStringListEdit();
	eAlignment TextAlign=Alignment::TopLeft;

	void Update(void);

	void EnableSelectionAnnotator(bool Enable);
protected:
	bool SelectionAnnotation;

	// bcTextDocument

	virtual void TextDocumentLineEnumReset(void)final;
	virtual bool TextDocumentLineEnum(uIntn LineIndex)final;
	virtual bool TextDocumentLineEnumNext(void)final;
	virtual Float32 TextDocumentLineEnumHeight(void)final;

	virtual void TextDocumentLineEnumPartEnum(void)final;
	virtual bool TextDocumentLineEnumPartFetch(cTextDocumentLinePart &TextPart)final;

	virtual Float32 TextDocumentVerticalAlign(Float32 TotalHeight)override;
	virtual Float32 TextDocumentHorizontalAlign(Float32 LineWidth)override;

	// bcTextDocumentEdit

	virtual cIndexPoint TextEditGetSelectionPoint(void)const override;
	virtual void TextEditMoveCaretAndSelectionPoint(uIntn LineIndex,uIntn TextIndex,uIntn PreviousCaretLineIndex,uIntn PreviousCaretTextIndex)override;
	virtual uIntn TextEditCopyLineText(uIntn LineIndex,uIntn TextIndex,uChar16 *Dest,uIntn Length)override;

	// child class

	virtual Float32 StringListGetLineHeight(void)=0;
public:
	virtual cArray<const uChar16> StringListGetLine(uIntn LineIndex)=0;
protected:
	virtual uIntn StringListApplyTextStyle(cUITextStyle &Style,uIntn LineIndex,uIntn TextIndex,bool Selected)=0;

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
		bcStringListEdit* GetHost(void);
	public:
		virtual void Setup(uIntn LineIndex)override;
		virtual void Clear(void)override;
		virtual bool Process(uIntn LineTextIndex,cTextDocumentLinePart &TextPart)override;
		virtual bool Fetch(uIntn &AnnotateIndex,cTextDocumentLinePart &TextPart)override;
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
	bcTextListEdit();
	~bcTextListEdit();

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

	virtual Float32 StringListGetLineHeight(void)override;
	virtual uIntn StringListApplyTextStyle(cUITextStyle &Style,uIntn LineIndex,uIntn TextIndex,bool Selected)override;
};
//---------------------------------------------------------------------------
class cTextListEdit : public bcTextListEdit
{
public:
	cTextListEdit();
	~cTextListEdit();

	cnRTL::cSeqList< cStringBuffer<uChar16> > TextList;
	
protected:

	// bcStringListEdit

	virtual cArray<const uChar16> StringListGetLine(uIntn LineIndex)override;

	// bcTextLinesEdit

	virtual uIntn TextEditGetLineCount(void)override;
	virtual uIntn TextEditGetLineLength(uIntn LineIndex)override;
	virtual bool TextEditReplace(uIntn LineIndex,uIntn TextIndex,uIntn LineEndIndex,uIntn TextEndIndex,uIntn ReplacementLength,const uChar16 *Replacement)override;
	virtual bool TextEditReplaceWithLines(cIndexPoint &InsertEndPos,uIntn LineIndex,uIntn TextIndex,uIntn LineEndIndex,uIntn TextEndIndex,iFetchLineProcedure *FetchProcedure)override;

};
//---------------------------------------------------------------------------
class cTextEdit : public bcTextListEdit
{
public:
	cTextEdit();
	~cTextEdit();

	cStringBuffer<uChar16> Text;
	
protected:

	// bcStringListEdit

	virtual cArray<const uChar16> StringListGetLine(uIntn LineIndex)override;

	// bcTextLinesEdit

	virtual uIntn TextEditGetLineCount(void)override;
	virtual uIntn TextEditGetLineLength(uIntn LineIndex)override;
	virtual bool TextEditReplace(uIntn LineIndex,uIntn TextIndex,uIntn LineEndIndex,uIntn TextEndIndex,uIntn ReplacementLength,const uChar16 *Replacement)override;
	virtual bool TextEditReplaceWithLines(cIndexPoint &InsertEndPos,uIntn LineIndex,uIntn TextIndex,uIntn LineEndIndex,uIntn TextEndIndex,iFetchLineProcedure *FetchProcedure)override;
};
//---------------------------------------------------------------------------
}	// namespace cnUI
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif
