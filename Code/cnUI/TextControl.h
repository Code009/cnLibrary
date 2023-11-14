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
	cFunctionSet<void (void)> TextDocumentNotifySet;
	struct cVisualInfo
	{
		Float32 ContentX;
		Float32 ContentY;
		uIntn LineIndex;
		ufInt16 LineCount;
	};
	virtual cVisualInfo TextDocumentVisualInfo(void)=0;
	struct cVisualLineInfo
	{
		Float32 Left;
		Float32 Top;
		Float32 Height;
		cUIColor LineColor;
		uIntn PartCount;
	};
	virtual bool TextDocumentVisualLine(ufInt16 LineIndex,cVisualLineInfo &LineInfo)=0;
	virtual bool TextDocumentVisualLinePart(ufInt16 LineIndex,ufInt16 PartIndex,cTextDocumentLinePart &TextPart)=0;
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
	vTextDocument(viTextDocumentData *Data=nullptr);
	~vTextDocument();

	static rPtr<viTextDocument> Create(viTextDocumentData *Data);

	viTextDocumentData* GetData(void)const;
	void SetData(viTextDocumentData *Data);

protected:
	dPtr<viTextDocumentData> fData;

	virtual void cnLib_FUNC Paint(iUISimplePaintContext *Context)override;

	void Update(void);
private:


	void DataInsertCallback(void);
	void DataRemoveCallback(void);

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
	
	void UpdateCache(uIntn LineIndex,uIntn LineCount);
	void SetupCacheLine(iUISimplePaintDevice *Device,cTextLineCache &Cache,uIntn LineIndex);
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
	bcTextDocument();
	~bcTextDocument();

	Float32 ContentZPosition=ZPosition_Content;
	void UpdateZIndex(void);

	void SetContent(rPtr<viTextDocument> Content);
	template<class TVisual>
	void SetContent(TVisual &Visual){
		SetContent(rCreate< bwvTextDocument<TVisual&> >(Visual));
	}

	// viTextDocumentData

	virtual cVisualInfo TextDocumentVisualInfo(void)override;
	virtual bool TextDocumentVisualLine(ufInt16 LineIndex,cVisualLineInfo &LineInfo)override;
	virtual bool TextDocumentVisualLinePart(ufInt16 LineIndex,ufInt16 PartIndex,cTextDocumentLinePart &TextPart)override;

	// line layout info

	uIntn GetVisibleLineIndex(void)const;
	uIntn GetVisibleLineCount(void)const;
	Float32 GetDocumentContentX(void);
	Float32 GetDocumentContentY(void);
	Float32 GetLineContentY(uIntn LineIndex)const;
	cUIRange GetLineContentRange(uIntn StartIndex,uIntn EndIndex);
	cUIRange GetTextContentRange(uIntn LineIndex,uIntn TextIndex,uIntn TextEndIndex);
	sIntn FindLineInContent(Float32 ContentY);
	//uIntn FindTextInContent(uIntn LineIndex,Float32 ContentX);
	//uIntn FindTextInContent(uIntn LineIndex,Float32 ContentX,bool &InText);
	cIndexPoint HitTest(Float32 x,Float32 y);

	Float32 GetLineHeight(uIntn LineIndex);
	Float32 GetLineWidth(uIntn LineIndex);

	// document annotation

	class iAnnotator
	{
	public:
		virtual void Setup(uIntn LineIndex)=0;
		virtual void Clear(void)=0;
		virtual bool Process(uIntn LineTextIndex,cTextDocumentLinePart &TextPart)=0;
		virtual bool Fetch(uIntn &AnnotateIndex,cTextDocumentLinePart &TextPart)=0;
	};
	bool InsertAnnotator(iAnnotator *Annotator,sIntn Order=0);
	bool RemoveAnnotator(iAnnotator *Annotator);

	// document properties

	virtual void GetDefaultTextStyle(cUITextStyle &Style);
	virtual uIntn GetLineLength(uIntn LineIndex);
	virtual cString<uChar16> GetLineText(uIntn LineIndex);

protected:

	virtual void ViewSetup(void)override;
	virtual void ViewClear(void)override;

	// content

	rPtr<viTextDocument> fContent;
	virtual void ControlContentSetDefault(void);


	// cScrollContent
	
	virtual void ScrollContentUpdateContentLayout(void);

	// content info

	virtual uIntn ContentItemGetCount(bool Vertical);
	virtual cUIRange ContentItemGetAllRange(bool Vertical);
	virtual cUIRange ContentItemGetRange(bool Vertical,uIntn Index);
	virtual cAnyObject ContentItemGetObject(bool Vertical,uIntn Index);

	// document layout

	virtual Float32 TextDocumentVerticalAlign(Float32 TotalHeight);
	virtual Float32 TextDocumentHorizontalAlign(Float32 LineWidth);
	virtual cUIColor TextDocumentGetLineColor(uIntn LineIndex);
	
	// lines data

	virtual void TextDocumentLineEnumReset(void)=0;
	virtual bool TextDocumentLineEnum(uIntn LineIndex)=0;
	virtual bool TextDocumentLineEnumNext(void)=0;
	virtual Float32 TextDocumentLineEnumHeight(void)=0;

	virtual void TextDocumentLineEnumPartEnum(void)=0;
	virtual bool TextDocumentLineEnumPartFetch(cTextDocumentLinePart &TextPart)=0;

	void ScrollTextToVisible(uIntn LineIndex,uIntn TextIndex);

	Float32 AlignDocumentVertical(Float32 TotalHeight);
	Float32 AlignLineHorizontal(Float32 LineWidth);

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
	void ClearCache(void);
	aCls<cLineItem>* QueryCacheLine(uIntn LineIndex);

	void LineItemAppendTextLayout(cLineItem *LineItem,const cTextDocumentLinePart &TextPart);

	uIntn LineItemFindTextIndex(cLineItem *LineItem,Float32 LineX);
	uIntn LineItemFindTextPartIndex(cLineItem *LineItem,uIntn &TextIndex);


	void SetNeedUpdateTextContent(void);
private:
	cnRTL::cSeqSet<iAnnotator*> fAnnotators;

	cnRTL::cSeqList<Float32> fLinePosCache;
	uIntn fVisibleLineIndex;

	cnRTL::cSeqList< aClsRef<cLineItem> > fVisibleLineList;

	cnRTL::cSeqList< aClsRef<cLineItem> > fCacheLines;
	aCls<cLineItem>* QueryCacheLineInEnumeration(uIntn LineIndex);
	aClsRef<cLineItem> CreateEnumCacheLine(void);

	void UpdateLinePosition(uIntn LineIndex);

	void UpdateVisibleLineRange(Float32 StartPos,Float32 EndPos);

	void AnnotationSetup(uIntn LineIndex);
	void AnnotationClear(void);
	void AnnotateProcess(cLineItem *NewItem,cTextDocumentLinePart &TextPart);
	void CacheLineAppendPart(cLineItem *NewItem,uIntn ContentIndex,void *Annotator,cTextDocumentLinePart &TextPart);
	void CacheLineInsertPart(cLineItem *NewItem,uIntn ContentIndex,void *Annotator,cTextDocumentLinePart &AnnotatedPart);
	cnRTL::cSeqSet<iAnnotator*> fTempAnnotator;

	//cUITextLineLayout fVisualEnumLineLayout;
};
//---------------------------------------------------------------------------
class bcTextList : public bcTextDocument, protected KeyHandler, protected MouseHandler, protected TouchHandler,
	public viControlStateData
{
public:
	bcTextList();
	~bcTextList();

	eAlignment TextAlign=Alignment::TopLeft;

	void Update(void);

	virtual bool ControlHot(void)override;
	virtual bool ControlFocused(void)override;
	virtual bool ControlDisabled(void)override;

protected:
	bool fHot		:1;
	bool fFocused	:1;

	bool fLBtnDown=false;

	virtual void ViewSetup(void)override;
	virtual void ViewClear(void)override;

	// key

	virtual void cnLib_FUNC KeyFocusEnterRange(iUIKeyEvent *KeyEvent)override;
	virtual void cnLib_FUNC KeyFocusLeaveRange(iUIKeyEvent *KeyEvent)override;
	virtual void cnLib_FUNC KeyInput(iUIKeyEvent *KeyEvent,eKeyCode KeyCode)override;

	// mouse

	virtual void cnLib_FUNC MouseEnter(iUIMouseEvent *MouseEvent)override;
	virtual void cnLib_FUNC MouseLeave(iUIMouseEvent *MouseEvent)override;
	virtual void cnLib_FUNC MouseMove(iUIMouseEvent *MouseEvent)override;
	virtual void cnLib_FUNC MouseDown(iUIMouseEvent *MouseEvent,eMouseButton Button)override;
	virtual void cnLib_FUNC MouseUp(iUIMouseEvent *MouseEvent,eMouseButton Button)override;
	virtual void cnLib_FUNC MouseWheel(iUIMouseEvent *MouseEvent,Float32 ScrollX,Float32 ScrollY)override;


	// bcTextDocument

	virtual Float32 TextDocumentVerticalAlign(Float32 TotalHeight)override;
	virtual Float32 TextDocumentHorizontalAlign(Float32 LineWidth)override;

	// content

	virtual void ControlStateChanged(void);

	virtual void TextListSetHotLine(sIntn LineIndex)=0;
	virtual void TextListSelectLine(sIntn LineIndex)=0;
	virtual void TextListMoveLine(bool Down)=0;
	//virtual void TextListMoveLines(bool Down)=0;

private:
	// fetch for viTextDocumentData
	//ufInt32 fLineIDSeed;
	//ufInt32 GenerateLineID(void);

	sIntn fMouseDownVisibleLineIndex;
};
//---------------------------------------------------------------------------
class cTextList : public bcTextList
{
public:
	cTextList();
	~cTextList();

	Float32 LineHeight=0;
	cnRTL::cSeqList< cStringBuffer<uChar16> > TextList;

	cUITextStyle TextStyle;
	cUIColor SelectedTextColor;
	cUIColor SelectedBackgroundColor;
	cUIColor SelectedBlurTextColor;
	cUIColor SelectedBlurBackgroundColor;

	bool HideSelectionWhenBlur;

	sIntn GetSelectIndex(void);
	void SetSelectIndex(sIntn Index);

	cFunction<void (void)> OnSelectionChanged;

protected:

	// bcTextDocument

	virtual cUIColor TextDocumentGetLineColor(uIntn LineIndex)override;
	virtual void TextDocumentLineEnumReset(void)override;
	virtual bool TextDocumentLineEnum(uIntn LineIndex)override;
	virtual bool TextDocumentLineEnumNext(void)override;
	virtual Float32 TextDocumentLineEnumHeight(void)override;

	virtual void TextDocumentLineEnumPartEnum(void)override;
	virtual bool TextDocumentLineEnumPartFetch(cTextDocumentLinePart &TextPart)override;


	sIntn fHotLineIndex;
	sIntn fSelectedLineIndex;


	virtual void TextListSetHotLine(sIntn LineIndex)override;
	virtual void TextListSelectLine(sIntn LineIndex)override;
	virtual void TextListMoveLine(bool Down)override;

private:
	cArray<const uChar16> TextListEditGetLine(uIntn LineIndex);

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
	vTextSelectBox(viButtonData *ButtonData=nullptr,viTextControlData *TextData=nullptr);
	~vTextSelectBox();

	static rPtr<viControl> Create(viButtonData *ButtonData,viTextControlData *TextData);

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
	void SetupTextCache(void);

	virtual void cnLib_FUNC Paint(iUISimplePaintContext *Context)override;


	void Update(void);
	void UpdateState(void);
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
	cTextSelectBox();
	~cTextSelectBox();

	virtual const uChar16* ControlTextString(void)override;
	virtual uIntn ControlTextLength(void)override;

protected:

	rPtr<iPopupWindowControl> fPopupWindowControl;
	class cSelectList : public cTextList
	{
		cTextSelectBox* GetHost(void);
	public:

		virtual void cnLib_FUNC KeyInput(iUIKeyEvent *KeyEvent,eKeyCode KeyCode)override;
	};
	cSelectList fPopupList;

	void SelectConfirm(void);
	void SelectCancel(void);

	virtual void ButtonContentSetDefault(void)override;
	virtual bool ButtonMouseAllowButton(eMouseButton Button)override;
	virtual void ButtonClick(const cUIPoint &Pos)override;

	virtual void cnLib_FUNC KeyInput(iUIKeyEvent *KeyEvent,eKeyCode KeyCode)override;


	virtual void cnLib_FUNC PopupOnShow(void)override;
	virtual void cnLib_FUNC PopupOnHide(void)override;


	void PopupSelectWindow(void);
};
//---------------------------------------------------------------------------
}	// namespace cnUI
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif
