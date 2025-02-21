/*- cnUI - Text Control ---------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2009-03-22                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnUI_TextControl_H__
#define __cnLibrary_cnUI_TextControl_H__
/*-------------------------------------------------------------------------*/
#include <cnUI/ControlBase.h>
#include <cnUI/ScrollContainer.h>

#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnUI{
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//struct cArraySection
//{
//	uIntn Offset;
//	uIntn Length;
//};
typedef cnVar::cAnyVariable<sizeof(void*),void*> cAnyObject;
//---------------------------------------------------------------------------
struct cTextDocumentLinePart
{
	const uChar16 *Text;
	uIntn Length;
	cUITextStyle TextStyle;
	const Float32 *TextDistance;
	Float32 TextHeight;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE viTextDocumentData : public viData
{
public:
	cFunctionSet<void (void)noexcept(true)> TextDocumentNotifySet;
	struct cVisualInfo
	{
		Float32 ContentX;
		Float32 ContentY;
		uIntn LineIndex;
		ufInt16 LineCount;
	};
	virtual cVisualInfo TextDocumentVisualInfo(void)noexcept(true)=0;
	struct cVisualLineInfo
	{
		Float32 Left;
		Float32 Top;
		Float32 Height;
		cUIColor LineColor;
		uIntn PartCount;
	};
	virtual bool TextDocumentVisualLine(ufInt16 LineIndex,cVisualLineInfo &LineInfo)noexcept(true)=0;
	virtual bool TextDocumentVisualLinePart(ufInt16 LineIndex,ufInt16 PartIndex,cTextDocumentLinePart &TextPart)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE viTextDocument : public viControl
{
public:
};
//---------------------------------------------------------------------------
template<class TClass,class TInterface=viTextDocument>
class bwvTextDocument : public bwvControl<TClass,TInterface>
{
public:
	using bwvControl<TClass,TInterface>::bwvControl;

};
//---------------------------------------------------------------------------
class vTextDocument : public vSimpleViewPainter
{
public:
	vTextDocument(viTextDocumentData *Data=nullptr)noexcept(true);
	~vTextDocument()noexcept(true);

	static rPtr<viTextDocument> Create(viTextDocumentData *Data)noexcept(true);

	viTextDocumentData* GetData(void)const noexcept(true);
	void SetData(viTextDocumentData *Data)noexcept(true);

protected:
	dPtr<viTextDocumentData> fData;

	virtual void cnLib_FUNC Paint(iUISimplePaintContext *Context)noexcept(true)override;

	void Update(void)noexcept(true);
private:


	void DataInsertCallback(void)noexcept(true);
	void DataRemoveCallback(void)noexcept(true);

	iFunctionToken *fTextDocumentNotifyToken;

	struct cTextCache
	{
		iPtr<iUITextGraph> TextGraph;
		cUIPoint Pos;
		cUIPoint Size;
	};
	struct cTextLineCache
	{
		Float32 LineX;
		Float32 LineY;
		Float32 LineHeight;
		cUIColor LineColor;
		cUIPoint LineSize;
		cnRTL::cSeqList<cTextCache> TextList;
	};
	cnRTL::cSeqList<cTextLineCache> fCacheLineList;
	uIntn fCacheLineIndex;
	cUIPoint fDisplayPos;
	
	void UpdateCache(uIntn LineIndex,uIntn LineCount)noexcept(true);
	void SetupCacheLine(iUISimplePaintDevice *Device,cTextLineCache &Cache,uIntn LineIndex)noexcept(true);
};
//---------------------------------------------------------------------------
struct cIndexPoint
{
	sIntn x;
	sIntn y;
};
//---------------------------------------------------------------------------
class bcTextDocument : public cScrollContent, public viTextDocumentData
{
public:
	bcTextDocument()noexcept(true);
	~bcTextDocument()noexcept(true);

	Float32 ContentZPosition=ZPosition_Content;
	void UpdateZIndex(void)noexcept(true);

	void SetContent(rPtr<viTextDocument> Content)noexcept(true);
	template<class TVisual>
	void SetContent(TVisual &Visual)noexcept(true){
		SetContent(rCreate< bwvTextDocument<TVisual&> >(Visual));
	}

	// viTextDocumentData

	virtual cVisualInfo TextDocumentVisualInfo(void)noexcept(true)override;
	virtual bool TextDocumentVisualLine(ufInt16 LineIndex,cVisualLineInfo &LineInfo)noexcept(true)override;
	virtual bool TextDocumentVisualLinePart(ufInt16 LineIndex,ufInt16 PartIndex,cTextDocumentLinePart &TextPart)noexcept(true)override;

	// line layout info

	uIntn GetVisibleLineIndex(void)const noexcept(true);
	uIntn GetVisibleLineCount(void)const noexcept(true);
	Float32 GetDocumentContentX(void)noexcept(true);
	Float32 GetDocumentContentY(void)noexcept(true);
	Float32 GetLineContentY(uIntn LineIndex)const noexcept(true);
	cUIRange GetLineContentRange(uIntn StartIndex,uIntn EndIndex)noexcept(true);
	cUIRange GetTextContentRange(uIntn LineIndex,uIntn TextIndex,uIntn TextEndIndex)noexcept(true);
	sIntn FindLineInContent(Float32 ContentY)noexcept(true);
	//uIntn FindTextInContent(uIntn LineIndex,Float32 ContentX)noexcept(true);
	//uIntn FindTextInContent(uIntn LineIndex,Float32 ContentX,bool &InText)noexcept(true);
	cIndexPoint HitTest(Float32 x,Float32 y)noexcept(true);

	Float32 GetLineHeight(uIntn LineIndex)noexcept(true);
	Float32 GetLineWidth(uIntn LineIndex)noexcept(true);

	// document annotation

	class iAnnotator
	{
	public:
		virtual void Setup(uIntn LineIndex)noexcept(true)=0;
		virtual void Clear(void)noexcept(true)=0;
		virtual bool Process(uIntn LineTextIndex,cTextDocumentLinePart &TextPart)noexcept(true)=0;
		virtual bool Fetch(uIntn &AnnotateIndex,cTextDocumentLinePart &TextPart)noexcept(true)=0;
	};
	bool InsertAnnotator(iAnnotator *Annotator,sIntn Order=0)noexcept(true);
	bool RemoveAnnotator(iAnnotator *Annotator)noexcept(true);

	// document properties

	virtual void GetDefaultTextStyle(cUITextStyle &Style)noexcept(true);
	virtual uIntn GetLineLength(uIntn LineIndex)noexcept(true);
	virtual cString<uChar16> GetLineText(uIntn LineIndex)noexcept(true);

protected:

	virtual void ViewSetup(void)noexcept(true)override;
	virtual void ViewClear(void)noexcept(true)override;

	// content

	rPtr<viTextDocument> fContent;
	virtual void ControlContentSetDefault(void)noexcept(true);


	// cScrollContent
	
	virtual void ScrollContentUpdateContentLayout(void)noexcept(true)override;

	// content info

	virtual uIntn ContentItemGetCount(bool Vertical)noexcept(true);
	virtual cUIRange ContentItemGetAllRange(bool Vertical)noexcept(true);
	virtual cUIRange ContentItemGetRange(bool Vertical,uIntn Index)noexcept(true);
	virtual cAnyObject ContentItemGetObject(bool Vertical,uIntn Index)noexcept(true);

	// document layout

	virtual Float32 TextDocumentVerticalAlign(Float32 TotalHeight)noexcept(true);
	virtual Float32 TextDocumentHorizontalAlign(Float32 LineWidth)noexcept(true);
	virtual cUIColor TextDocumentGetLineColor(uIntn LineIndex)noexcept(true);
	
	// lines data

	virtual void TextDocumentLineEnumReset(void)noexcept(true)=0;
	virtual bool TextDocumentLineEnum(uIntn LineIndex)noexcept(true)=0;
	virtual bool TextDocumentLineEnumNext(void)noexcept(true)=0;
	virtual Float32 TextDocumentLineEnumHeight(void)noexcept(true)=0;

	virtual void TextDocumentLineEnumPartEnum(void)noexcept(true)=0;
	virtual bool TextDocumentLineEnumPartFetch(cTextDocumentLinePart &TextPart)noexcept(true)=0;

	void ScrollTextToVisible(uIntn LineIndex,uIntn TextIndex)noexcept(true);

	Float32 AlignDocumentVertical(Float32 TotalHeight)noexcept(true);
	Float32 AlignLineHorizontal(Float32 LineWidth)noexcept(true);

	Float32 fScrollTotalSize[2];
	

	struct cLinePartItem{
		void *Annotator;
		cString<uChar16> Text;
		cUITextStyle TextStyle;
		Float32 PartHeight;
		uIntn ContentTextIndex;
		uIntn TotalTextIndex;
		bool DefaultLayout;
	};
	struct cLineItem{
		uIntn LineIndex;
		cnRTL::cSeqList<cLinePartItem> LineParts;
		cnRTL::cSeqList<Float32> TextPosition;
		cnRTL::cSeqList<Float32> TextDistances;
		uIntn ContentLength;
		uIntn TotalLength;
		Float32 Width;
		Float32 Height;
		Float32 HeadBorder;
	};
	void ClearCache(void)noexcept(true);
	aCls<cLineItem>* QueryCacheLine(uIntn LineIndex)noexcept(true);

	void LineItemAppendTextLayout(cLineItem *LineItem,const cTextDocumentLinePart &TextPart)noexcept(true);

	uIntn LineItemFindTextIndex(cLineItem *LineItem,Float32 LineX)noexcept(true);
	uIntn LineItemFindTextPartIndex(cLineItem *LineItem,uIntn &TextIndex)noexcept(true);


	void SetNeedUpdateTextContent(void)noexcept(true);
private:
	cnRTL::cSeqSet<iAnnotator*> fAnnotators;

	cnRTL::cSeqList<Float32> fLinePosCache;
	uIntn fVisibleLineIndex;

	cnRTL::cSeqList< aClsRef<cLineItem> > fVisibleLineList;

	cnRTL::cSeqList< aClsRef<cLineItem> > fCacheLines;
	aCls<cLineItem>* QueryCacheLineInEnumeration(uIntn LineIndex)noexcept(true);
	aClsRef<cLineItem> CreateEnumCacheLine(void)noexcept(true);

	void UpdateLinePosition(uIntn LineIndex)noexcept(true);

	void UpdateVisibleLineRange(Float32 StartPos,Float32 EndPos)noexcept(true);

	void AnnotationSetup(uIntn LineIndex)noexcept(true);
	void AnnotationClear(void)noexcept(true);
	void AnnotateProcess(cLineItem *NewItem,cTextDocumentLinePart &TextPart)noexcept(true);
	void CacheLineAppendPart(cLineItem *NewItem,uIntn ContentIndex,void *Annotator,cTextDocumentLinePart &TextPart)noexcept(true);
	void CacheLineInsertPart(cLineItem *NewItem,uIntn ContentIndex,void *Annotator,cTextDocumentLinePart &AnnotatedPart)noexcept(true);
	cnRTL::cSeqSet<iAnnotator*> fTempAnnotator;

	//cUITextLineLayout fVisualEnumLineLayout;
};
//---------------------------------------------------------------------------
class bcTextList : public bcTextDocument, protected KeyHandler, protected MouseHandler, protected TouchHandler,
	public viControlStateData
{
public:
	bcTextList()noexcept(true);
	~bcTextList()noexcept(true);

	eAlignment TextAlign=Alignment::TopLeft;

	void Update(void)noexcept(true);

	virtual bool ControlHot(void)noexcept(true)override;
	virtual bool ControlFocused(void)noexcept(true)override;
	virtual bool ControlDisabled(void)noexcept(true)override;

protected:
	bool fHot		:1;
	bool fFocused	:1;

	bool fLBtnDown=false;

	virtual void ViewSetup(void)noexcept(true)override;
	virtual void ViewClear(void)noexcept(true)override;

	// key

	virtual void cnLib_FUNC KeyFocusEnterRange(iUIKeyEvent *KeyEvent)noexcept(true)override;
	virtual void cnLib_FUNC KeyFocusLeaveRange(iUIKeyEvent *KeyEvent)noexcept(true)override;
	virtual void cnLib_FUNC KeyInput(iUIKeyEvent *KeyEvent,eKeyCode KeyCode)noexcept(true)override;

	// mouse

	virtual void cnLib_FUNC MouseEnter(iUIMouseEvent *MouseEvent)noexcept(true)override;
	virtual void cnLib_FUNC MouseLeave(iUIMouseEvent *MouseEvent)noexcept(true)override;
	virtual void cnLib_FUNC MouseMove(iUIMouseEvent *MouseEvent)noexcept(true)override;
	virtual void cnLib_FUNC MouseDown(iUIMouseEvent *MouseEvent,eMouseButton Button)noexcept(true)override;
	virtual void cnLib_FUNC MouseUp(iUIMouseEvent *MouseEvent,eMouseButton Button)noexcept(true)override;
	virtual void cnLib_FUNC MouseWheel(iUIMouseEvent *MouseEvent,Float32 ScrollX,Float32 ScrollY)noexcept(true)override;


	// bcTextDocument

	virtual Float32 TextDocumentVerticalAlign(Float32 TotalHeight)noexcept(true)override;
	virtual Float32 TextDocumentHorizontalAlign(Float32 LineWidth)noexcept(true)override;

	// content

	virtual void ControlStateChanged(void)noexcept(true);

	virtual void TextListSetHotLine(sIntn LineIndex)noexcept(true)=0;
	virtual void TextListSelectLine(sIntn LineIndex)noexcept(true)=0;
	virtual void TextListMoveLine(bool Down)noexcept(true)=0;
	//virtual void TextListMoveLines(bool Down)noexcept(true)=0;

private:
	// fetch for viTextDocumentData
	//ufInt32 fLineIDSeed;
	//ufInt32 GenerateLineID(void)noexcept(true);

	sIntn fMouseDownVisibleLineIndex;
};
//---------------------------------------------------------------------------
class cTextList : public bcTextList
{
public:
	cTextList()noexcept(true);
	~cTextList()noexcept(true);

	Float32 LineHeight=0;
	cnRTL::cSeqList< cStringBuffer<uChar16> > TextList;

	cUITextStyle TextStyle;
	cUIColor SelectedTextColor;
	cUIColor SelectedBackgroundColor;
	cUIColor SelectedBlurTextColor;
	cUIColor SelectedBlurBackgroundColor;

	bool HideSelectionWhenBlur;

	sIntn GetSelectIndex(void)noexcept(true);
	void SetSelectIndex(sIntn Index)noexcept(true);

	cFunction<void (void)noexcept(true)> OnSelectionChanged;

protected:

	// bcTextDocument

	virtual cUIColor TextDocumentGetLineColor(uIntn LineIndex)noexcept(true)override;
	virtual void TextDocumentLineEnumReset(void)noexcept(true)override;
	virtual bool TextDocumentLineEnum(uIntn LineIndex)noexcept(true)override;
	virtual bool TextDocumentLineEnumNext(void)noexcept(true)override;
	virtual Float32 TextDocumentLineEnumHeight(void)noexcept(true)override;

	virtual void TextDocumentLineEnumPartEnum(void)noexcept(true)override;
	virtual bool TextDocumentLineEnumPartFetch(cTextDocumentLinePart &TextPart)noexcept(true)override;


	sIntn fHotLineIndex;
	sIntn fSelectedLineIndex;


	virtual void TextListSetHotLine(sIntn LineIndex)noexcept(true)override;
	virtual void TextListSelectLine(sIntn LineIndex)noexcept(true)override;
	virtual void TextListMoveLine(bool Down)noexcept(true)override;

private:
	cArray<const uChar16> TextListEditGetLine(uIntn LineIndex)noexcept(true);

	// text enum
	uIntn fEnumLineIndex;
	Float32 fEnumLineHeight;

	bool fEnumLinePartDone;
	/*enum FetchState
	{
		fsLeft,
		fsSelection,
		fsRight,
		fsEnd
	}fEnumLineFetchState;
	uIntn fEnumLineFetchSelLeft;
	uIntn fEnumLineFetchSelRight;*/
};
//---------------------------------------------------------------------------
class vTextSelectBox : public vSimpleViewPainter
{
public:
	vTextSelectBox(viButtonData *ButtonData=nullptr,viTextControlData *TextData=nullptr)noexcept(true);
	~vTextSelectBox()noexcept(true);

	static rPtr<viControl> Create(viButtonData *ButtonData,viTextControlData *TextData)noexcept(true);

	//viButtonData* GetButtonData(void)const;
	//void SetButtonData(viButtonData *Data);
	//viTextControlData* GetTextData(void)const;
	//void SetTextData(viTextControlData *Data);
	//void SetTextData(viButtonData *ButtonData,viTextControlData *TextData);

protected:
	viButtonData *fButtonData;
	viTextControlData *fTextData;

	iPtr<iUITextGraph> fCacheTextGraph;
	cUIPoint fTextGraphSize;
	bool fUpdateText;
	void SetupTextCache(void)noexcept(true);

	virtual void cnLib_FUNC Paint(iUISimplePaintContext *Context)noexcept(true)override;


	void Update(void)noexcept(true);
	void UpdateState(void)noexcept(true);
private:
	//void ButtonDataInsertCallback(void);
	//void ButtonDataRemoveCallback(void);
	//void TextDataInsertCallback(void);
	//void TextDataRemoveCallback(void);

	iFunctionToken *fButtonDataNotifyToken;
	iFunctionToken *fTextControlNotifyToken;

};
//---------------------------------------------------------------------------
class cTextSelectBox : public bcTextButton, private iPopupWindowControlCallback
{
public:
	cTextSelectBox()noexcept(true);
	~cTextSelectBox()noexcept(true);

	virtual const uChar16* ControlTextString(void)noexcept(true)override;
	virtual uIntn ControlTextLength(void)noexcept(true)override;

protected:

	rPtr<iPopupWindowControl> fPopupWindowControl;
	class cSelectList : public cTextList
	{
		cTextSelectBox* GetHost(void)noexcept(true);
	public:

		virtual void cnLib_FUNC KeyInput(iUIKeyEvent *KeyEvent,eKeyCode KeyCode)noexcept(true)override;
	};
	cSelectList fPopupList;

	void SelectConfirm(void)noexcept(true);
	void SelectCancel(void)noexcept(true);

	virtual void ButtonContentSetDefault(void)noexcept(true)override;
	virtual bool ButtonMouseAllowButton(eMouseButton Button)noexcept(true)override;
	virtual void ButtonClick(const cUIPoint &Pos)noexcept(true)override;

	virtual void cnLib_FUNC KeyInput(iUIKeyEvent *KeyEvent,eKeyCode KeyCode)noexcept(true)override;


	virtual void cnLib_FUNC PopupOnShow(void)noexcept(true)override;
	virtual void cnLib_FUNC PopupOnHide(void)noexcept(true)override;


	void PopupSelectWindow(void)noexcept(true);
};
//---------------------------------------------------------------------------
}	// namespace cnUI
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif
