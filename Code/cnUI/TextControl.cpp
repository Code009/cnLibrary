#include "TextControl.h"

using namespace cnLibrary;
using namespace cnUI;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
vTextDocument::vTextDocument(viTextDocumentData *Data)noexcept
	: fData(Data)
{
	fCacheLineIndex=0;

	DataInsertCallback();
}
//---------------------------------------------------------------------------
vTextDocument::~vTextDocument()noexcept
{
	DataRemoveCallback();
}
//---------------------------------------------------------------------------
rPtr<viTextDocument> vTextDocument::Create(viTextDocumentData *Data)noexcept
{
	return rCreate< bwvTextDocument<vTextDocument> >(Data);
}
//---------------------------------------------------------------------------
viTextDocumentData* vTextDocument::GetData(void)const noexcept
{
	return fData;
}
//---------------------------------------------------------------------------
void vTextDocument::SetData(viTextDocumentData *Data)noexcept
{
	if(fData==Data)
		return;

	DataRemoveCallback();
	fData=Data;
	DataInsertCallback();

	Update();
}
//---------------------------------------------------------------------------
void vTextDocument::DataInsertCallback(void)noexcept
{
	if(fData!=nullptr){
		fTextDocumentNotifyToken=fData->TextDocumentNotifySet.Insert([this]{
			Update();
		});
	}
}
//---------------------------------------------------------------------------
void vTextDocument::DataRemoveCallback(void)noexcept
{
	if(fData!=nullptr){
		fData->TextDocumentNotifySet.Remove(fTextDocumentNotifyToken);
	}
}
//---------------------------------------------------------------------------
void vTextDocument::Update(void)noexcept
{
	fCacheLineIndex=0;
	fCacheLineList.Clear();
	if(fViewContent!=nullptr)
		fViewContent->QueryUpdate();
}
//---------------------------------------------------------------------------
void vTextDocument::Paint(iUISimplePaintContext *Context)noexcept
{
	if(fData==nullptr)
		return;

	auto VisualInfo=fData->TextDocumentVisualInfo();
	// check text cache
	UpdateCache(VisualInfo.LineIndex,VisualInfo.LineCount);

	// draw text lines
	for(auto &CacheLine : fCacheLineList){
		Float32 LineX=VisualInfo.ContentX+CacheLine.LineX;
		Float32 LineY=VisualInfo.ContentY+CacheLine.LineY;

		if(LineY>fPaintSize.y){
			break;	// line out of bottom bound
		}

		// line background color
		Context->Fill({0,LineY},{fPaintSize.x,CacheLine.LineHeight},CacheLine.LineColor);

		// draw text
		for(auto &Cache : CacheLine.TextList){
			cUIPoint DrawPos;
			DrawPos.x=LineX+Cache.Pos.x;

			if(DrawPos.x+Cache.Size.x<0)
				continue;	// text out of left bound

			if(DrawPos.x>fPaintSize.x){
				break;	// text out of right bound
			}
			DrawPos.y=LineY+Cache.Pos.y;
			Context->Graph(DrawPos,Cache.Size,Cache.TextGraph);
		}
	}
}
//---------------------------------------------------------------------------
void vTextDocument::UpdateCache(uIntn LineIndex,uIntn LineCount)noexcept
{
	auto Device=fViewContent->QueryDevice();

	uIntn CacheCount=fCacheLineList.GetCount();
	if(fCacheLineIndex==LineIndex){
		if(CacheCount<LineCount){
			uIntn AppendCount=LineCount-CacheCount;
			fCacheLineList.Append_temp(AppendCount);
			auto CacheList=fCacheLineList.GetItems();
			// append line
			for(uIntn i=CacheCount;i<LineCount;i++){
				SetupCacheLine(Device,CacheList[i],i);
			}
		}
		return;
	}
	uIntn LineEndIndex=LineIndex+LineCount;
	uIntn CacheEndIndex=fCacheLineIndex+CacheCount;
	cTextLineCache *SetupPointer;
	uIntn SetupIndex;
	uIntn SetupCount;
	if(LineEndIndex<=fCacheLineIndex || LineIndex>=CacheEndIndex){
		// clear all cache
		fCacheLineList.Clear();
		fCacheLineList.SetCount(LineCount);
		SetupPointer=fCacheLineList.GetItems();
		SetupIndex=LineIndex;
		SetupCount=LineCount;
	}
	else{
		if(fCacheLineIndex<LineIndex){
			// remove head
			fCacheLineList.RemoveAt(0,LineIndex-fCacheLineIndex);
			if(CacheEndIndex<LineEndIndex){
				// setup append tail
				SetupIndex=CacheEndIndex;
				SetupCount=LineEndIndex-CacheEndIndex;
				SetupPointer=fCacheLineList.Append_temp(SetupCount);
			}
			else{
				// tail has cached
				SetupCount=0;
				SetupIndex=LineIndex;
				SetupPointer=nullptr;
			}
		}
		else{	// fCacheLineIndex>LineIndex
			if(CacheEndIndex<LineEndIndex){
				// remove tail
				fCacheLineList.SetCount(LineEndIndex-fCacheLineIndex);
			}
			// insert head
			SetupIndex=LineIndex;
			SetupCount=fCacheLineIndex-LineIndex;
			SetupPointer=fCacheLineList.InsertAt(0,SetupCount);

			if(CacheEndIndex>LineEndIndex){
				// insert head now
				for(uIntn i=0;i<SetupCount;i++){
					SetupCacheLine(Device,SetupPointer[i],i);
				}
				// setup append tail
				SetupIndex=CacheEndIndex;
				SetupCount=LineEndIndex-LineIndex;
				SetupPointer=fCacheLineList.Append_temp(SetupCount);
			}
		}
	}
	fCacheLineIndex=LineIndex;
	for(uIntn i=0;i<SetupCount;i++){
		SetupCacheLine(Device,SetupPointer[i],SetupIndex+i);
	}
}
//---------------------------------------------------------------------------
void vTextDocument::SetupCacheLine(iUISimplePaintDevice *Device,cTextLineCache &LineCache,uIntn LineIndex)noexcept
{
	LineCache.LineSize.x=0;
	LineCache.LineSize.y=0;
	
	viTextDocumentData::cVisualLineInfo LineInfo;
	if(fData->TextDocumentVisualLine(static_cast<ufInt16>(LineIndex),LineInfo)){
		//LineCache.DataTimestamp=LineInfo.DataTimestamp;
		LineCache.LineX=LineInfo.Left;
		LineCache.LineY=LineInfo.Top;
		LineCache.LineHeight=LineInfo.Height;
		LineCache.LineColor=LineInfo.LineColor;
		cTextDocumentLinePart Part;
		for(ufInt16 i=0;i<LineInfo.PartCount;i++){
			if(fData->TextDocumentVisualLinePart(static_cast<ufInt16>(LineIndex),i,Part)){
				cTextCache CacheText;
				if(Part.Length!=0){
					CacheText.TextGraph=Device->CreateTextGraph(Part.Text,Part.Length,Part.TextStyle,Part.TextDistance,Part.TextHeight);
					CacheText.Size=CacheText.TextGraph->GetSize();
					CacheText.Pos.x=LineCache.LineSize.x;
					CacheText.Pos.y=0;
			
					if(LineCache.LineSize.y<CacheText.Size.y){
						LineCache.LineSize.y=CacheText.Size.y;
					}

					LineCache.LineSize.x+=CacheText.Size.x;
				}
				else{
					CacheText.TextGraph=nullptr;
					CacheText.Size.x=0;
					CacheText.Size.y=0;
					CacheText.Pos.x=LineCache.LineSize.x;
					CacheText.Pos.y=0;
				}
				LineCache.TextList.AppendMake(cnVar::MoveCast(CacheText));
			}
		}
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcTextDocument::bcTextDocument()noexcept
	: fScrollTotalSize{0,0}
{
}
//---------------------------------------------------------------------------
bcTextDocument::~bcTextDocument()noexcept
{
	SetView(nullptr);
	InvalidateData();
}
//---------------------------------------------------------------------------
void bcTextDocument::UpdateZIndex(void)noexcept
{
	if(fContent!=nullptr){
		fContent->SetContentZPosition(ContentZPosition);
	}
}
//---------------------------------------------------------------------------
void bcTextDocument::ControlContentSetDefault(void)noexcept
{
	SetContent(vTextDocument::Create(this));
}
//---------------------------------------------------------------------------
void bcTextDocument::SetContent(rPtr<viTextDocument> Content)noexcept
{
	if(fContent!=nullptr){
		fContent->SetView(nullptr);
	}
	fContent=cnVar::MoveCast(Content);
	if(fContent!=nullptr){
		fContent->SetContentZPosition(ContentZPosition);
		fContent->SetView(fView);
	}
}
//---------------------------------------------------------------------------
void bcTextDocument::ViewSetup(void)noexcept
{
	cScrollContent::ViewSetup();

	if(fContent!=nullptr){
		fContent->SetView(fView);
	}
	else{
		ControlContentSetDefault();
	}
}
//---------------------------------------------------------------------------
void bcTextDocument::ViewClear(void)noexcept
{
	if(fContent!=nullptr){
		fContent->SetView(nullptr);
	}

	cScrollContent::ViewClear();
}
//---------------------------------------------------------------------------
void bcTextDocument::ClearCache(void)noexcept
{
	fCacheLines.Clear();
}
//---------------------------------------------------------------------------
Float32 bcTextDocument::TextDocumentVerticalAlign(Float32)noexcept{	return 0;	}
Float32 bcTextDocument::TextDocumentHorizontalAlign(Float32)noexcept{	return 0;	}
cUIColor bcTextDocument::TextDocumentGetLineColor(uIntn)noexcept{			return UIColorFromUInt32(0);	}
//---------------------------------------------------------------------------
uIntn bcTextDocument::ContentItemGetCount(bool Vertical)noexcept
{
	if(Vertical){
		return fVisibleLineList.GetCount();
	}
	else{
		return 1;
	}
}
//---------------------------------------------------------------------------
cUIRange bcTextDocument::ContentItemGetAllRange(bool Vertical)noexcept
{
	cUIRange Range;
	if(Vertical){
		Range.Begin=fLinePosCache[fVisibleLineIndex];
		Range.End=fLinePosCache[fVisibleLineIndex+fVisibleLineList.GetCount()];
	}
	else{
		Range.Begin=0;
		Range.End=fScrollTotalSize[0];
	}
	return Range;
}
//---------------------------------------------------------------------------
cUIRange bcTextDocument::ContentItemGetRange(bool Vertical,uIntn Index)noexcept
{
	cUIRange Range;
	if(Vertical){
		Range.Begin=fLinePosCache[fVisibleLineIndex+Index];
		Range.End=fLinePosCache[fVisibleLineIndex+Index+1];
	}
	else{
		if(Index==0){
			Range.Begin=0;
		}
		else{
			Range.Begin=fScrollTotalSize[0];
		}
		Range.End=fScrollTotalSize[0];
	}
	return Range;
}
//---------------------------------------------------------------------------
cAnyObject bcTextDocument::ContentItemGetObject(bool Vertical,uIntn Index)noexcept
{
	if(Vertical){
		return fVisibleLineIndex+Index;
	}
	else{
		return nullptr;
	}
}
//---------------------------------------------------------------------------
void bcTextDocument::GetDefaultTextStyle(cUITextStyle &Style)noexcept
{
	gApplyDefaultTextStyle(Style);
}
//---------------------------------------------------------------------------
uIntn bcTextDocument::GetLineLength(uIntn LineIndex)noexcept
{
	auto CacheLine=QueryCacheLine(LineIndex);
	if(CacheLine==nullptr){
		return 0;
	}
	return CacheLine->ContentLength;
}
//---------------------------------------------------------------------------
cString<uChar16> bcTextDocument::GetLineText(uIntn LineIndex)noexcept
{
	cStringBuffer<uChar16> Text;
	auto CacheLine=QueryCacheLine(LineIndex);
	if(CacheLine!=nullptr){
		for(auto &Part : CacheLine->LineParts){
			if(Part.Annotator==nullptr){
				Text.Append(Part.Text->Pointer,Part.Text->Length);
			}
		}
	}
	return cnVar::MoveCast(Text);
}
//---------------------------------------------------------------------------
void bcTextDocument::UpdateLinePosition(uIntn LineIndex)noexcept
{
	uIntn LinePosCacheIndex=fLinePosCache.GetCount()-1;
	if(LinePosCacheIndex>LineIndex)
		return;
	Float32 CurLinePos;
	if(LinePosCacheIndex==0){
		CurLinePos=0;
		fLinePosCache.AppendMake(static_cast<Float32>(0));
	}
	else{
		CurLinePos=fLinePosCache[LinePosCacheIndex];
	}
	if(TextDocumentLineEnum(LinePosCacheIndex)){
		while(LinePosCacheIndex<=LineIndex){
			Float32 LineHeight=TextDocumentLineEnumHeight();
			CurLinePos+=LineHeight;

			fLinePosCache.AppendMake(CurLinePos);
			if(TextDocumentLineEnumNext()==false)
				break;
		}

		TextDocumentLineEnumReset();
	}
}
//---------------------------------------------------------------------------
void bcTextDocument::UpdateVisibleLineRange(Float32 StartPos,Float32 EndPos)noexcept
{
	//uIntn CacheLineIndex=fVisibleLineIndex;

	fLinePosCache.AppendMake(static_cast<Float32>(0));
	Float32 CurLinePos=0;
	fVisibleLineIndex=0;
	Float32 LineHeight=TextDocumentLineEnumHeight();
	while(CurLinePos+LineHeight<StartPos){
		CurLinePos+=LineHeight;
		if(TextDocumentLineEnumNext()==false){
			// no visible line
			fScrollNoUpperLimit[1]=false;
			fScrollTotalSize[1]=CurLinePos;
			return;
		}
		fLinePosCache.AppendMake(CurLinePos);
		fVisibleLineIndex++;
	}
	uIntn CurLineIndex=fVisibleLineIndex;
	// build up cache
	while(CurLinePos<=EndPos){
		// setup line data
		auto NewItem=QueryCacheLineInEnumeration(CurLineIndex);
		fVisibleLineList.AppendMake(cnVar::MoveCast(NewItem));

		// next line
		LineHeight=NewItem->Height;
		CurLinePos+=LineHeight;
		fLinePosCache.AppendMake(CurLinePos);
		if(TextDocumentLineEnumNext()==false){
			// no more lines
			fScrollNoUpperLimit[1]=false;
			fScrollTotalSize[1]=CurLinePos;
			break;
		}
		CurLineIndex++;
	}
	if(fScrollNoUpperLimit[1]){
		// update total size
		fScrollTotalSize[1]=CurLinePos;
	}
}
//---------------------------------------------------------------------------
bool bcTextDocument::InsertAnnotator(iAnnotator *Annotator,sIntn Order)noexcept
{UnusedParameter(Order);
	return fAnnotators.Insert(Annotator);
}
//---------------------------------------------------------------------------
bool bcTextDocument::RemoveAnnotator(iAnnotator *Annotator)noexcept
{
	return fAnnotators.Remove(Annotator);
}
//---------------------------------------------------------------------------
void bcTextDocument::AnnotationSetup(uIntn LineIndex)noexcept
{
	for(auto *Annotator : fAnnotators){
		Annotator->Setup(LineIndex);
	}
}
//---------------------------------------------------------------------------
void bcTextDocument::AnnotationClear(void)noexcept
{
	for(auto *Annotator : fAnnotators){
		Annotator->Clear();
	}
}
//---------------------------------------------------------------------------
aCls<bcTextDocument::cLineItem>* bcTextDocument::QueryCacheLine(uIntn LineIndex)noexcept
{
	// lookup visible line
	//if(IsNeededResetScrollContent()==false)
	{
		// try to lookup visible line cache
		uIntn VisibleLineIndex=LineIndex-fVisibleLineIndex;
		if(VisibleLineIndex<fVisibleLineList.GetCount()){
			// use visible line
			return fVisibleLineList[VisibleLineIndex];
		}
	}
	// lookup cache line
	for(auto &CacheLine : fCacheLines){
		if(CacheLine->LineIndex==LineIndex){
			return CacheLine;
		}
	}

	if(TextDocumentLineEnum(LineIndex)==false)
		return nullptr;

	// create line data
	AnnotationSetup(LineIndex);
	auto NewItem=CreateEnumCacheLine();
	AnnotationClear();
	NewItem->LineIndex=LineIndex;
	aCls<bcTextDocument::cLineItem> *RetItem=NewItem;

	// put to cache
	fCacheLines.AppendMake(cnVar::MoveCast(NewItem));
	
	TextDocumentLineEnumReset();
	return RetItem;
}
//---------------------------------------------------------------------------
aCls<bcTextDocument::cLineItem>* bcTextDocument::QueryCacheLineInEnumeration(uIntn LineIndex)noexcept
{
	// lookup cache line
	for(auto &CacheLine : fCacheLines){
		if(CacheLine->LineIndex==LineIndex){
			return CacheLine;
		}
	}

	// create line data
	AnnotationSetup(LineIndex);
	auto NewItem=CreateEnumCacheLine();
	AnnotationClear();
	NewItem->LineIndex=LineIndex;
	aCls<bcTextDocument::cLineItem> *RetItem=NewItem;

	// put to cache
	fCacheLines.AppendMake(cnVar::MoveCast(NewItem));
	return RetItem;
}
//---------------------------------------------------------------------------
void bcTextDocument::LineItemAppendTextLayout(cLineItem *LineItem,const cTextDocumentLinePart &TextPart)noexcept
{
	// append text layout
	LineItem->TextDistances.SetCount(LineItem->TotalLength+TextPart.Length);
	LineItem->TextPosition.SetCount(LineItem->TotalLength+TextPart.Length+1);

	Float32 *pTextPos=&LineItem->TextPosition[LineItem->TotalLength+1];
	Float32 *pTextDist=&LineItem->TextDistances[LineItem->TotalLength];

	for(uIntn i=0;i<TextPart.Length;i++){
		Float32 TextDist=TextPart.TextDistance[i];
		pTextDist[i]=TextDist;
		// accumulate line width
		LineItem->Width+=TextDist;
		pTextPos[i]=LineItem->Width;
	}
}
//---------------------------------------------------------------------------
void bcTextDocument::CacheLineAppendPart(cLineItem *NewItem,uIntn ContentIndex,void *Annotator,cTextDocumentLinePart &TextPart)noexcept
{
	auto *NewPartItem=NewItem->LineParts.Append();
	NewPartItem->ContentTextIndex=ContentIndex;
	NewPartItem->TotalTextIndex=NewItem->TotalLength;
	NewPartItem->Annotator=Annotator;
	NewPartItem->TextStyle=TextPart.TextStyle;
	NewPartItem->DefaultLayout=false;
	if(TextPart.Length!=0){
		NewPartItem->Text.SetString(TextPart.Text,TextPart.Length);
	}
	rPtr<iUITextLineLayout> DefaultPartLayoutData;
	if(TextPart.TextDistance==nullptr){
		// create default text layout
		DefaultPartLayoutData=cnSystem::CreateTextLineLayout(TextPart.Text,TextPart.Length,TextPart.TextStyle);
		TextPart.TextDistance=DefaultPartLayoutData->GetTextDistance();
		TextPart.TextHeight=DefaultPartLayoutData->GetTextHeight();
		NewPartItem->DefaultLayout=true;
	}
	NewPartItem->PartHeight=TextPart.TextHeight;

	// accumulate line layout
	LineItemAppendTextLayout(NewItem,TextPart);

	// accumulate line total length
	NewItem->TotalLength+=TextPart.Length;

}
//---------------------------------------------------------------------------
void bcTextDocument::CacheLineInsertPart(cLineItem *NewItem,uIntn ContentIndex,void *Annotator,cTextDocumentLinePart &TextPart)noexcept
{
	uIntn InsertPartIndex=0;
	uIntn TotalIndex;
	uIntn PartCount=NewItem->LineParts.GetCount();
	for(InsertPartIndex=0;InsertPartIndex<PartCount;InsertPartIndex++){
		auto &Part=NewItem->LineParts[InsertPartIndex];
		if(Part.Annotator==nullptr){
			cnLib_ASSERT(ContentIndex>=Part.ContentTextIndex);
			if(ContentIndex==Part.ContentTextIndex){
				// just before this part
				TotalIndex=Part.TotalTextIndex;
				goto Start;
			}
			if(ContentIndex<Part.ContentTextIndex+Part.Text->Length){
				// divide this part
				uIntn DivideLength=ContentIndex-Part.ContentTextIndex;
				
				// insert new part
				InsertPartIndex++;
				auto *NewPartItem=NewItem->LineParts.InsertAt(InsertPartIndex);
				auto &DividedPart=NewItem->LineParts[InsertPartIndex-1];
				NewPartItem->ContentTextIndex=ContentIndex;
				NewPartItem->TotalTextIndex=DividedPart.TotalTextIndex+DivideLength;
				NewPartItem->Annotator=nullptr;
				NewPartItem->TextStyle=DividedPart.TextStyle;
				NewPartItem->DefaultLayout=false;
				NewPartItem->Text.SetString(DividedPart.Text->Pointer+DivideLength,DividedPart.Text->Length-DivideLength);

				NewPartItem->PartHeight=DividedPart.PartHeight;

				// divide original part
				DividedPart.Text.Cut(0,DivideLength);

				// insert annotation
				TotalIndex=NewPartItem->TotalTextIndex;
				goto Start;
			}
		}
	}
	// append to the end
	TotalIndex=NewItem->TotalLength;
Start:
	auto *NewPartItem=NewItem->LineParts.InsertAt(InsertPartIndex);
	NewPartItem->ContentTextIndex=ContentIndex;
	NewPartItem->TotalTextIndex=TotalIndex;
	NewPartItem->Annotator=Annotator;
	NewPartItem->TextStyle=TextPart.TextStyle;
	NewPartItem->DefaultLayout=false;
	if(TextPart.Length!=0){
		NewPartItem->Text.SetString(TextPart.Text,TextPart.Length);
	}
	rPtr<iUITextLineLayout> DefaultPartLayoutData;
	if(TextPart.TextDistance==nullptr){
		// create default text layout
		DefaultPartLayoutData=cnSystem::CreateTextLineLayout(TextPart.Text,TextPart.Length,TextPart.TextStyle);
		TextPart.TextDistance=DefaultPartLayoutData->GetTextDistance();
		NewPartItem->DefaultLayout=true;
	}
	NewPartItem->PartHeight=TextPart.TextHeight;

	// accumulate line total length
	NewItem->TotalLength+=TextPart.Length;


	// accumulate line layout
	NewItem->TextDistances.InsertAt(TotalIndex,TextPart.TextDistance,TextPart.Length);
	NewItem->TextPosition.SetCount(TotalIndex+1);
	NewItem->TextPosition.SetCount(NewItem->TotalLength+1);

	Float32 *pTextPos=&NewItem->TextPosition[1];
	Float32 *pTextDist=&NewItem->TextDistances[0];
	Float32 CurPos=pTextPos[TotalIndex-1];
	for(uIntn PosIndex=TotalIndex;PosIndex<NewItem->TotalLength;PosIndex++){
		// accumulate line width
		CurPos+=pTextDist[PosIndex];
		pTextPos[PosIndex]=CurPos;
	}
	// correct text index
	TotalIndex+=TextPart.Length;
	PartCount=NewItem->LineParts.GetCount();
	InsertPartIndex++;
	while(InsertPartIndex<PartCount){
		auto &Part=NewItem->LineParts[InsertPartIndex];
		Part.TotalTextIndex=TotalIndex;
		TotalIndex+=Part.Text->Length;
		InsertPartIndex++;
	}

	NewItem->Width=CurPos;
}
//---------------------------------------------------------------------------
void bcTextDocument::AnnotateProcess(cLineItem *NewItem,cTextDocumentLinePart &TextPart)noexcept
{
	// part annotator
	uIntn ProcessContentIndex=0;
	cTextDocumentLinePart ProcessPart;
	do{
		// annotated part
		ProcessPart.Text=TextPart.Text+ProcessContentIndex;
		ProcessPart.Length=TextPart.Length-ProcessContentIndex;
		ProcessPart.TextStyle=TextPart.TextStyle;
		if(TextPart.TextDistance==nullptr){
			ProcessPart.TextDistance=nullptr;
		}
		else{
			ProcessPart.TextHeight=TextPart.TextHeight;
			ProcessPart.TextDistance=TextPart.TextDistance+ProcessContentIndex;
		}
		uIntn LineTextIndex=NewItem->ContentLength+ProcessContentIndex;
		// process annotator chain
		for(auto *Annotator : fAnnotators){
			if(Annotator->Process(LineTextIndex,ProcessPart)){
				// process part converted to annotation
				fTempAnnotator.Insert(Annotator);
				break;
			}
		}
		
		for(auto *Annotator : fTempAnnotator){
			cTextDocumentLinePart AnnotatedPart;
			AnnotatedPart.Text=nullptr;
			AnnotatedPart.Length=0;
			GetDefaultTextStyle(AnnotatedPart.TextStyle);
			AnnotatedPart.TextDistance=nullptr;
			AnnotatedPart.TextHeight=0;
			uIntn AnnotateIndex=LineTextIndex;
			while(Annotator->Fetch(AnnotateIndex,AnnotatedPart)){
				if(AnnotateIndex>=LineTextIndex){
					// append to part list
					CacheLineAppendPart(NewItem,LineTextIndex,Annotator,AnnotatedPart);
				}
				else{
					// insert to part list
					CacheLineInsertPart(NewItem,AnnotateIndex,Annotator,AnnotatedPart);
				}
			}
		}
		fTempAnnotator.Clear();

		// record content part
		if(ProcessPart.Length!=0){
			CacheLineAppendPart(NewItem,LineTextIndex,nullptr,ProcessPart);

			// next part
			ProcessContentIndex+=ProcessPart.Length;
		}
	}while(TextPart.Length>ProcessContentIndex);

	// accumulate line content length
	NewItem->ContentLength+=TextPart.Length;
}
//---------------------------------------------------------------------------
aClsRef<bcTextDocument::cLineItem> bcTextDocument::CreateEnumCacheLine(void)noexcept
{
	// setup line data
	auto NewItem=aClsCreate<cLineItem>();
	NewItem->Width=0;
	NewItem->Height=TextDocumentLineEnumHeight();
	NewItem->HeadBorder=0;
	NewItem->TotalLength=0;
	NewItem->ContentLength=0;
	NewItem->TextPosition.SetCount(1);
	NewItem->TextPosition[0]=0;
	TextDocumentLineEnumPartEnum();
	rPtr<iUITextLineLayout> DefaultPartLayoutData;

	cTextDocumentLinePart TextPart;
	while(TextDocumentLineEnumPartFetch(TextPart)){
		if(TextPart.Length==0){
			continue;
		}
		// prepare text distance
		NewItem->TextDistances.SetCapacity(NewItem->TotalLength+TextPart.Length);

		AnnotateProcess(NewItem,TextPart);
	}

	// call annotator for line ending
	TextPart.Text=nullptr;
	TextPart.Length=0;
	GetDefaultTextStyle(TextPart.TextStyle);
	TextPart.TextDistance=nullptr;
	TextPart.TextHeight=0;
	AnnotateProcess(NewItem,TextPart);
	return NewItem;
}
//---------------------------------------------------------------------------
void bcTextDocument::SetNeedUpdateTextContent(void)noexcept
{
}
//---------------------------------------------------------------------------
void bcTextDocument::ScrollContentUpdateContentLayout(void)noexcept
{
	cScrollContent::ScrollContentUpdateContentLayout();
	// layout texts

	
	Float32 VisibleStartPos=fScrollOffset[1];
	Float32 VisibleEndPos;
	{
		if(VisibleStartPos<0)
			VisibleStartPos=0;
		cUIPoint ViewSize=fView->GetSize();
	
		VisibleEndPos=VisibleStartPos+ViewSize.y;
	}

	fVisibleLineList.Clear();
	fLinePosCache.Clear();

	// TODO line update and use valid cache pos to start enumeration
	if(TextDocumentLineEnum(0)){

		UpdateVisibleLineRange(VisibleStartPos,VisibleEndPos);
		TextDocumentLineEnumReset();
	}
	else{
		fVisibleLineIndex=0;
	}
	// find visible width
	fScrollTotalSize[0]=0;
	if(fContent!=nullptr){
		uIntn VisibleLineEndIndex=fVisibleLineIndex+fVisibleLineList.GetCount();
		for(auto i=fVisibleLineIndex;i<VisibleLineEndIndex;i++){
			auto Range=GetTextContentRange(i,0,cnVar::TIntegerValue<uIntn>::Max);
			if(fScrollTotalSize[0]<Range.End+4){
				fScrollTotalSize[0]=Range.End+4;
			}
			
		}
	}
	TextDocumentNotifySet();
}
//---------------------------------------------------------------------------
bcTextDocument::cVisualInfo bcTextDocument::TextDocumentVisualInfo(void)noexcept
{
	cVisualInfo VisualInfo;
	VisualInfo.LineIndex=fVisibleLineIndex;
	VisualInfo.LineCount=static_cast<ufInt16>(fVisibleLineList.GetCount());
	
	// content x
	VisualInfo.ContentX=GetDocumentContentX();
	VisualInfo.ContentY=GetDocumentContentY();
	return VisualInfo;
}
//---------------------------------------------------------------------------
bool bcTextDocument::TextDocumentVisualLine(ufInt16 LineIndex,cVisualLineInfo &LineInfo)noexcept
{
	if(LineIndex<fVisibleLineIndex)
		return false;
	uIntn VisibleLineIndex=LineIndex-fVisibleLineIndex;
	if(VisibleLineIndex>=fVisibleLineList.GetCount()){
		return false;
	}
	auto &Line=*fVisibleLineList[VisibleLineIndex];
	LineInfo.Left=AlignLineHorizontal(Line.Width);
	
	LineInfo.Top=fLinePosCache[LineIndex];
	LineInfo.Height=Line.Height;
	LineInfo.LineColor=TextDocumentGetLineColor(LineIndex);
	//LineInfo.DataTimestamp=0;
	LineInfo.PartCount=static_cast<ufInt16>(Line.LineParts.GetCount());
	return true;
}
//---------------------------------------------------------------------------
bool bcTextDocument::TextDocumentVisualLinePart(ufInt16 LineIndex,ufInt16 PartIndex,cTextDocumentLinePart &TextPart)noexcept
{
	if(LineIndex<fVisibleLineIndex)
		return false;
	uIntn VisibleLineIndex=LineIndex-fVisibleLineIndex;
	if(VisibleLineIndex>=fVisibleLineList.GetCount()){
		return false;
	}
	auto &LineItem=*fVisibleLineList[VisibleLineIndex];
	if(PartIndex>=LineItem.LineParts.GetCount())
		return false;
	auto &PartItem=LineItem.LineParts[PartIndex];
	TextPart.Text=PartItem.Text;
	TextPart.Length=PartItem.Text->Length;
	TextPart.TextStyle=PartItem.TextStyle;

	if(PartItem.DefaultLayout){
		TextPart.TextDistance=nullptr;
		TextPart.TextHeight=0;
	}
	else{
		TextPart.TextHeight=PartItem.PartHeight;
		TextPart.TextDistance=&LineItem.TextDistances[PartItem.TotalTextIndex];
	}
	return true;
}
//---------------------------------------------------------------------------
Float32 bcTextDocument::AlignDocumentVertical(Float32 TotalHeight)noexcept
{
	Float32 ContentY;
	if(TotalHeight<fScrollViewportSize[1]){
		// align lines inside viewport
		ContentY=TextDocumentVerticalAlign(fScrollTotalSize[1]);
	}
	else{
		// document content overflow
		ContentY=0;
	}
	ContentY-=fScrollOffset[1];
	return ContentY;
}
//---------------------------------------------------------------------------
Float32 bcTextDocument::AlignLineHorizontal(Float32 LineWidth)noexcept
{
	if(LineWidth<fScrollViewportSize[0]){
		return TextDocumentHorizontalAlign(LineWidth);
	}
	else{
		return 0;
	}
}
//---------------------------------------------------------------------------
void bcTextDocument::ScrollTextToVisible(uIntn LineIndex,uIntn TextIndex)noexcept
{
	if(fContent==nullptr)
		return;

	if(fLinePosCache.GetCount()==0)
		return;

	UpdateLinePosition(LineIndex);
	if(LineIndex>=fLinePosCache.GetCount()){
		LineIndex=fLinePosCache.GetCount()-1;
	}

	// check line
	Float32 LinePos=fLinePosCache[LineIndex];
	Float32 LineEndPos=fLinePosCache[LineIndex+1];

	// check text
	auto TextRange=GetTextContentRange(LineIndex,TextIndex,TextIndex+1);
	if(TextRange.End-TextRange.Begin<4){
		TextRange.End=TextRange.Begin+4;
	}

	// scroll

	ScrollRangeToVisible(TextRange.Begin,LinePos,TextRange.End,LineEndPos);
}
//---------------------------------------------------------------------------
Float32 bcTextDocument::GetLineHeight(uIntn LineIndex)noexcept
{
	uIntn VisibleLineIndex=LineIndex-fVisibleLineIndex;
	if(VisibleLineIndex<fVisibleLineList.GetCount()){
		// visible line range
		auto &LineItem=*fVisibleLineList[VisibleLineIndex];
		return LineItem.Height;
	}
	// line not in visible range

	auto Line=QueryCacheLine(LineIndex);
	if(Line==nullptr)
		return 0;

	return Line->Height;
}
//---------------------------------------------------------------------------
Float32 bcTextDocument::GetLineWidth(uIntn LineIndex)noexcept
{
	uIntn VisibleLineIndex=LineIndex-fVisibleLineIndex;
	if(VisibleLineIndex<fVisibleLineList.GetCount()){
		// visible line range
		auto &LineItem=*fVisibleLineList[VisibleLineIndex];
		return LineItem.Width;
	}
	// line not in visible range

	auto Line=QueryCacheLine(LineIndex);
	if(Line==nullptr)
		return 0;

	return Line->Width;
}
//---------------------------------------------------------------------------
uIntn bcTextDocument::GetVisibleLineIndex(void)const noexcept
{
	return fVisibleLineIndex;
}
//---------------------------------------------------------------------------
uIntn bcTextDocument::GetVisibleLineCount(void)const noexcept
{
	return fVisibleLineList.GetCount();
}
//---------------------------------------------------------------------------
Float32 bcTextDocument::GetDocumentContentX(void)noexcept
{
	Float32 Offset=fScrollOffset[0];
	return -Offset;
}
//---------------------------------------------------------------------------
Float32 bcTextDocument::GetDocumentContentY(void)noexcept
{
	return AlignDocumentVertical(fScrollTotalSize[1]);
}
//---------------------------------------------------------------------------
Float32 bcTextDocument::GetLineContentY(uIntn LineIndex)const noexcept
{
	Float32 Pos;
	if(LineIndex>=fLinePosCache.GetCount()){
		Pos=fLinePosCache[fLinePosCache.GetCount()-1];
	}
	else{
		Pos=fLinePosCache[LineIndex];
	}
	return Pos;
}
//---------------------------------------------------------------------------
cUIRange bcTextDocument::GetLineContentRange(uIntn StartIndex,uIntn EndIndex)noexcept
{
	if(EndIndex<StartIndex){
		EndIndex=StartIndex;
	}
	UpdateLinePosition(EndIndex);
	if(fLinePosCache.GetCount()==0)
		return {0,0};
	cUIRange Range;
	uIntn CachedLineCount=fLinePosCache.GetCount();
	if(StartIndex>=CachedLineCount){
		Range.Begin=Range.End=fLinePosCache[CachedLineCount-1];
		return Range;
	}
	Range.Begin=fLinePosCache[StartIndex];

	if(EndIndex>=CachedLineCount){
		Range.End=fLinePosCache[CachedLineCount-1];
	}
	else{
		Range.End=fLinePosCache[EndIndex];
	}
	return Range;
}
//---------------------------------------------------------------------------
cUIRange bcTextDocument::GetTextContentRange(uIntn LineIndex,uIntn TextIndex,uIntn TextEndIndex)noexcept
{
	auto LineItem=QueryCacheLine(LineIndex);
	if(LineItem==nullptr){
		cUIRange Empty={0,0};
		return Empty;
	}
	Float32 LineLeft=AlignLineHorizontal(LineItem->Width);
	if(LineItem->TotalLength==0){
		cUIRange Empty={LineLeft,LineLeft};
		return Empty;
	}
	uIntn PartCount=LineItem->LineParts.GetCount();
	uIntn PartIndex=0;
	uIntn CurPartContentTextIndex=0;
	cUIRange Range;

	for(PartIndex=0;PartIndex<PartCount;PartIndex++){
		auto &TextPart=LineItem->LineParts[PartIndex];
		uIntn TextLength=TextPart.Text->Length;
		if(TextPart.Annotator==nullptr && TextLength!=0){
			uIntn TextIndexInPart=TextIndex-CurPartContentTextIndex;
			if(TextIndexInPart<TextLength){
				// found part
				Range.Begin=LineLeft+LineItem->TextPosition[TextPart.TotalTextIndex+TextIndexInPart];
				goto FindEndPart;
			}	
			// next part
			CurPartContentTextIndex+=TextLength;
		}
	}
	// TextIndex out of range, return end of text
	{
		Float32 TextPos=LineLeft+LineItem->TextPosition[LineItem->TotalLength];
		Range.Begin=Range.End=TextPos;
		return Range;
	}
FindEndPart:
	do{
		auto &TextPart=LineItem->LineParts[PartIndex];
		uIntn TextLength=TextPart.Text->Length;
		if(TextPart.Annotator==nullptr && TextLength!=0){
			uIntn TextIndexInPart=TextEndIndex-CurPartContentTextIndex;
			if(TextIndexInPart<=TextLength){
				// found part
				Range.End=LineLeft+LineItem->TextPosition[TextPart.TotalTextIndex+TextIndexInPart];
				return Range;
			}
			// next part
			CurPartContentTextIndex+=TextLength;
		}
		// next part
		PartIndex++;
	}while(PartIndex<PartCount);
	// TextEndIndex out of range, return end of text
	Range.End=LineLeft+LineItem->TextPosition[LineItem->TotalLength];
	return Range;
}
//---------------------------------------------------------------------------
sIntn bcTextDocument::FindLineInContent(Float32 ContentY)noexcept
{
	uIntn LineCount=fLinePosCache.GetCount();
	if(LineCount==0){
		return -1;
	}
	LineCount--;
	if(ContentY<fLinePosCache[0]){
		return -1;
	}
	for(uIntn LineIndex=0;LineIndex<LineCount;LineIndex++){
		if(ContentY<fLinePosCache[LineIndex+1]){
			return LineIndex;
		}
	}
	return LineCount;
}
//---------------------------------------------------------------------------
uIntn bcTextDocument::LineItemFindTextIndex(cLineItem *LineItem,Float32 LineX)noexcept
{
	uIntn TextIndex;
	if(cnMemory::ViewBinarySearch(TextIndex,LineItem->TextPosition,LineItem->TotalLength,[LineX](const Float32 &TextPos){
			Float32 d=TextPos-LineX;
			if(d<0)
				return -1;
			if(d>0)
				return 1;
			return 0;
		})==false
	){
		if(TextIndex!=0){
			TextIndex--;
		}
	}
	return TextIndex;
}
//---------------------------------------------------------------------------
uIntn bcTextDocument::LineItemFindTextPartIndex(cLineItem *LineItem,uIntn &TextIndex)noexcept
{
	// look for text index in part
	for(uIntn PartIndex=0,PartCount=LineItem->LineParts.GetCount();PartIndex<PartCount;PartIndex++){
		// get pos
		auto &TextPart=LineItem->LineParts[PartIndex];
		uIntn TextIndexInPart=TextIndex-TextPart.TotalTextIndex;
		if(TextIndexInPart<TextPart.Text->Length){
			// found text
			TextIndex=TextIndexInPart;
			return PartIndex;
		}
	}
	// out of range, return end of text
	TextIndex=0;
	return IndexNotFound;

}
//---------------------------------------------------------------------------
#if 0
uIntn bcTextDocument::FindTextInContent(uIntn LineIndex,Float32 ContentX)
{
	bool InLine;
	return FindTextInContent(LineIndex,ContentX,InLine);
}
//---------------------------------------------------------------------------
uIntn bcTextDocument::FindTextInContent(uIntn LineIndex,Float32 ContentX,bool &InText)
{
	auto LineItem=QueryCacheLine(LineIndex);
	if(LineItem==nullptr){
		InText=false;
		return 0;
	}

	Float32 LineLeft=AlignLineHorizontal(LineItem->Width);
	uIntn TextIndex=LineItemFindTextIndex(LineItem,ContentX-LineLeft);

	uIntn CurContentTextIndex=0;
	cnRTL::cSeqList<Float32> PartTextPosList;
	// look for text index in part
	for(uIntn PartIndex=0,PartCount=LineItem->LineParts.GetCount();PartIndex<PartCount;PartIndex++){
		// get pos
		auto &TextPart=LineItem->LineParts[PartIndex];
		uIntn TextLength=TextPart.Text->Length;
		uIntn TextIndexInPart=TextIndex-TextPart.TotalTextIndex;
		if(TextIndexInPart<TextLength){
			// found text
			if(TextPart.Annotator==nullptr){
				return CurContentTextIndex+TextIndexInPart;
			}
			else{
				// found text in annotation
				InText=false;
				return CurContentTextIndex;
			}
		}
		// next part
		if(TextPart.Annotator==nullptr){
			// content index
			CurContentTextIndex+=TextLength;
		}
	}
	// out of range, return end of text
	InText=false;
	return CurContentTextIndex;
}
#endif // 0
//---------------------------------------------------------------------------
cIndexPoint bcTextDocument::HitTest(Float32 x,Float32 y)noexcept
{
	Float32 HitX=x+fScrollOffset[0];
	Float32 HitY=y+fScrollOffset[1];
	cIndexPoint TextPoint;
	TextPoint.y=FindLineInContent(HitY);
	// restrict to valid line
	if(TextPoint.y<0){
		TextPoint.y=0;
	}
	else if(static_cast<uIntn>(TextPoint.y)>=fLinePosCache.GetCount()-1){
		TextPoint.y=fLinePosCache.GetCount()-2;
	}

	// find text
	auto LineItem=QueryCacheLine(TextPoint.y);
	if(LineItem==nullptr || LineItem->LineParts.GetCount()==0){
		TextPoint.x=0;
		return TextPoint;
	}

	// get text part
	Float32 LineLeft=AlignLineHorizontal(LineItem->Width);
	Float32 LineX=HitX-LineLeft;
	uIntn TextIndex=LineItemFindTextIndex(LineItem,LineX);
	uIntn PartTextIndex=TextIndex;
	auto PartIndex=LineItemFindTextPartIndex(LineItem,PartTextIndex);
	if(PartIndex!=IndexNotFound){
		auto &TextPart=LineItem->LineParts[PartIndex];
		if(TextPart.Annotator==nullptr){
			// found text
			TextPoint.x=TextPart.ContentTextIndex+PartTextIndex;

			// check text hit position
			Float32 TextCenter=LineItem->TextPosition[TextIndex];
			TextCenter+=LineItem->TextPosition[TextIndex+1];
			TextCenter/=2;
			if(LineX>TextCenter){
				// hit to next text for right half
				TextPoint.x++;
			}		
			return TextPoint;
		}
	}
	else{
		// out of range, index to end of text
		PartIndex=LineItem->LineParts.GetCount()-1;
	}

	auto &TextPart=LineItem->LineParts[PartIndex];
	TextPoint.x=TextPart.ContentTextIndex;
	return TextPoint;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#if 0
//---------------------------------------------------------------------------
ufInt32 bcTextList::GenerateLineID(void)
{
	fLineIDSeed++;
	if((fLineIDSeed&0x0000FFFF)==0){
		auto LineCount=VisibleContentGetCount();
		for(uIntn i=0;i<LineCount;i++){
			auto TextControl=static_cast<bcTextLineControl*>(VisibleContentGetControl(i));
			TextControl->LineID=0;
		}
	}
	return fLineIDSeed;
}
#endif // 0
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcTextList::bcTextList()noexcept
{
	fFocused=false;
	fHot=false;

	//fLineIDSeed=0;
}
//---------------------------------------------------------------------------
bcTextList::~bcTextList()noexcept
{
	SetView(nullptr);
	viControlStateData::InvalidateData();
}
//---------------------------------------------------------------------------
void bcTextList::ViewSetup(void)noexcept
{
	bcTextDocument::ViewSetup();

	ViewInsertKeyHandler(fView,this);
	ViewInsertMouseHandler(fView,this);
	ViewInsertTouchHandler(fView,this);
}
//---------------------------------------------------------------------------
void bcTextList::ViewClear(void)noexcept
{
	ViewRemoveKeyHandler(fView,this);
	ViewRemoveMouseHandler(fView,this);
	ViewRemoveTouchHandler(fView,this);

	bcTextDocument::ViewClear();
}
//---------------------------------------------------------------------------
bool bcTextList::ControlHot(void)noexcept
{
	return fHot;
}
//---------------------------------------------------------------------------
bool bcTextList::ControlFocused(void)noexcept
{
	return fFocused;
}
//---------------------------------------------------------------------------
bool bcTextList::ControlDisabled(void)noexcept
{
	return !fView->IsEnabled();
}
//---------------------------------------------------------------------------
void bcTextList::Update(void)noexcept
{
	ClearCache();
	SetNeedUpdateTextContent();
}
//---------------------------------------------------------------------------
Float32 bcTextList::TextDocumentVerticalAlign(Float32 TotalHeight)noexcept
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
Float32 bcTextList::TextDocumentHorizontalAlign(Float32 LineWidth)noexcept
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
void bcTextList::ControlStateChanged(void)noexcept
{
	ControlStateNotifySet();
}
//---------------------------------------------------------------------------
void bcTextList::MouseEnter(iUIMouseEvent *MouseEvent)noexcept
{UnusedParameter(MouseEvent);
	fHot=true;

	ControlStateChanged();
}
//---------------------------------------------------------------------------
void bcTextList::MouseLeave(iUIMouseEvent *MouseEvent)noexcept
{UnusedParameter(MouseEvent);
	fHot=false;

	ControlStateChanged();
	fLBtnDown=false;
	TextListSetHotLine(IndexNotFound);
	SetNeedUpdateTextContent();
}
//---------------------------------------------------------------------------
void bcTextList::MouseMove(iUIMouseEvent *MouseEvent)noexcept
{
	cUIPoint Pos;
	MouseEvent->GetPosition(fView,Pos);

	if(ViewIsPointInside(fView,Pos.x,Pos.y))
		fHot=true;

	if(fLBtnDown){
		return;
	}

	uIntn HotVisibleLineIndex=FindLineInContent(Pos.y+fScrollOffset[1]);
	TextListSetHotLine(HotVisibleLineIndex);

	SetNeedUpdateTextContent();

}
//---------------------------------------------------------------------------
void bcTextList::MouseDown(iUIMouseEvent *MouseEvent,eMouseButton Button)noexcept
{
	cUIPoint Pos;
	MouseEvent->GetPosition(fView,Pos);

	auto ViewKey=iCast<iUIKeyControl>(fView);
	ViewKey->SetFocus(true);
	if(Button==MouseButton::Left){
		fLBtnDown=true;
		fMouseDownVisibleLineIndex=FindLineInContent(Pos.y+fScrollOffset[1]);
	}
}
//---------------------------------------------------------------------------
void bcTextList::MouseUp(iUIMouseEvent *MouseEvent,eMouseButton Button)noexcept
{
	cUIPoint Pos;
	MouseEvent->GetPosition(fView,Pos);

	if(Button==MouseButton::Left){
		sIntn UpLineIndex=FindLineInContent(Pos.y+fScrollOffset[1]);
		if(fMouseDownVisibleLineIndex==UpLineIndex){
			TextListSelectLine(fMouseDownVisibleLineIndex);
		}

		fLBtnDown=false;
	}
	fLBtnDown=false;
}
//---------------------------------------------------------------------------
void bcTextList::MouseWheel(iUIMouseEvent*,Float32 ScrollX,Float32 ScrollY)noexcept
{
	auto Offset=GetScrollOffset();
	SetScrollOffset(Offset.x-ScrollX/3,Offset.y-ScrollY/3);
}
//---------------------------------------------------------------------------
void bcTextList::KeyFocusEnterRange(iUIKeyEvent*)noexcept
{
	fFocused=true;
	
	ControlStateChanged();
}
//---------------------------------------------------------------------------
void bcTextList::KeyFocusLeaveRange(iUIKeyEvent*)noexcept
{
	fFocused=false;
	ControlStateChanged();
}
//---------------------------------------------------------------------------
void bcTextList::KeyInput(iUIKeyEvent *KeyEvent,eKeyCode KeyCode)noexcept
{
	bool ShiftPressed=KeyEvent->IsKeyDown(KeyCode::Shift);
	//bool CtrlPressed=KeyEvent->IsKeyDown(KeyCode::Ctrl);
	switch(KeyCode){
	default:
		break;
		/*
	case KeyCode::kA:
		if(CtrlPressed){
			// Select all
			auto Length=EditGetLength();

			SetSelectionCorrected(Length,0);
		}
		break;
	case KeyCode::Home:
		break;
	case KeyCode::End:
		break;
		*/
	case KeyCode::Up:
		if(ShiftPressed){
		}
		else{
			TextListMoveLine(false);
		}
		break;
	case KeyCode::Down:
		TextListMoveLine(true);
		break;
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cTextList::cTextList()noexcept
{
	fEnumLineIndex=IndexNotFound;

	gApplyDefaultTextStyle(TextStyle);
	SelectedTextColor=UIColorFromUInt32(0xFFFFFFFF);
	SelectedBackgroundColor=UIColorFromUInt32(0xFFD77800);

	SelectedBlurTextColor=TextStyle.TextColor;
	SelectedBlurBackgroundColor=UIColorFromUInt32(0xFFD9D9D9);

	HideSelectionWhenBlur=false;

	fHotLineIndex=-1;
	fSelectedLineIndex=-1;
}
//---------------------------------------------------------------------------
cTextList::~cTextList()noexcept
{
	SetView(nullptr);
}
//---------------------------------------------------------------------------
cUIColor cTextList::TextDocumentGetLineColor(uIntn LineIndex)noexcept
{
	if(static_cast<sIntn>(LineIndex)==fSelectedLineIndex){
		return UIColorFromUInt32(0xFFD77800);
	}
	if(static_cast<sIntn>(LineIndex)==fHotLineIndex){
		return UIColorFromUInt32(0xFFE6C094);
	}

	return UIColorFromUInt32(0);
}
//---------------------------------------------------------------------------
void cTextList::TextDocumentLineEnumReset(void)noexcept
{
	fEnumLineIndex=IndexNotFound;
}
//---------------------------------------------------------------------------
bool cTextList::TextDocumentLineEnum(uIntn LineIndex)noexcept
{
	fEnumLineHeight=LineHeight;
	if(fEnumLineHeight==0){
		fEnumLineHeight=TextStyle.FontSize+2;
	}

	auto LineCount=TextList.GetCount();
	fScrollTotalSize[1]=static_cast<Float32>(LineCount)*fEnumLineHeight;
	fScrollUpperLimit[1]=false;

	if(LineIndex>=LineCount){
		return false;
	}

	fEnumLineIndex=LineIndex;
	return true;
}
//---------------------------------------------------------------------------
bool cTextList::TextDocumentLineEnumNext(void)noexcept
{
	if(fEnumLineIndex+1>=TextList.GetCount())
		return false;
	fEnumLineIndex+=1;
	fEnumLinePartDone=true;
	return true;
}
//---------------------------------------------------------------------------
Float32 cTextList::TextDocumentLineEnumHeight(void)noexcept
{
	return fEnumLineHeight;
}
//---------------------------------------------------------------------------
void cTextList::TextDocumentLineEnumPartEnum(void)noexcept
{
	fEnumLinePartDone=false;
}
//---------------------------------------------------------------------------
bool cTextList::TextDocumentLineEnumPartFetch(cTextDocumentLinePart &TextPart)noexcept
{
	if(fEnumLinePartDone){
		return false;
	}
	fEnumLinePartDone=true;

	auto LineArray=TextListEditGetLine(fEnumLineIndex);
	TextPart.Text=LineArray.Pointer;
	TextPart.Length=LineArray.Length;
	TextPart.TextDistance=nullptr;
	TextPart.TextHeight=0;
	TextPart.TextStyle=TextStyle;
	return true;
}
//---------------------------------------------------------------------------
cArray<const uChar16> cTextList::TextListEditGetLine(uIntn LineIndex)noexcept
{
	if(LineIndex>=TextList.GetCount()){
		return NullArray;
	}

	return TextList[LineIndex].Storage();
}
//---------------------------------------------------------------------------
void cTextList::TextListSetHotLine(sIntn LineIndex)noexcept
{
	fHotLineIndex=LineIndex;
}
//---------------------------------------------------------------------------
sIntn cTextList::GetSelectIndex(void)noexcept
{
	return fSelectedLineIndex;
}
//---------------------------------------------------------------------------
void cTextList::SetSelectIndex(sIntn Index)noexcept
{
	fSelectedLineIndex=Index;

	Float32 CalcHeight=LineHeight;
	if(CalcHeight==0){
		CalcHeight=TextStyle.FontSize+2;
	}

	Float32 Lowwer,Upper;
	Lowwer=CalcHeight*static_cast<Float32>(fSelectedLineIndex);
	Upper=Lowwer+CalcHeight;
	ScrollRangeToVisible(0,Lowwer,Upper);
	SetNeedUpdateTextContent();
}
//---------------------------------------------------------------------------
void cTextList::TextListSelectLine(sIntn LineIndex)noexcept
{
	if(fSelectedLineIndex!=LineIndex){
		fSelectedLineIndex=LineIndex;
		if(OnSelectionChanged!=nullptr){
			OnSelectionChanged();
		}
	}
}
//---------------------------------------------------------------------------
void cTextList::TextListMoveLine(bool Down)noexcept
{
	if(Down){
		if(static_cast<uIntn>(fSelectedLineIndex+1)>=TextList.GetCount()){
			return;
		}
		SetSelectIndex(fSelectedLineIndex+1);
	}
	else{
		if(fSelectedLineIndex<=0){
			return;
		}
		if(static_cast<uIntn>(fSelectedLineIndex-1)<TextList.GetCount()){
			SetSelectIndex(fSelectedLineIndex-1);
		}
		else{
			SetSelectIndex(TextList.GetCount()-1);
		}
	}
	SetNeedUpdateTextContent();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
vTextSelectBox::vTextSelectBox(viButtonData *ButtonData,viTextControlData *TextData)noexcept
{
	fButtonData=ButtonData;
	fTextData=TextData;


	fButtonDataNotifyToken=fButtonData->ButtonNotifySet.Insert([this]{
		UpdateState();
	});
	fTextControlNotifyToken=fTextData->ControlTextNotifySet.Insert([this]{
		Update();
	});

	fUpdateText=true;
}
//---------------------------------------------------------------------------
vTextSelectBox::~vTextSelectBox()noexcept
{
	fButtonData->ButtonNotifySet.Remove(fButtonDataNotifyToken);
	fTextData->ControlTextNotifySet.Remove(fTextControlNotifyToken);
}
//---------------------------------------------------------------------------
rPtr<viControl> vTextSelectBox::Create(viButtonData *ButtonData,viTextControlData *TextData)noexcept
{
	return rCreate< bwvControl<vTextSelectBox> >(ButtonData,TextData);
}
//---------------------------------------------------------------------------
void vTextSelectBox::SetupTextCache(void)noexcept
{
	if(fUpdateText==false){
		return;
	}
	fUpdateText=false;
	fCacheTextGraph=nullptr;
	fTextGraphSize={0,0};

	auto Length=fTextData->ControlTextLength();
	if(Length==0){
		return;
	}
	auto Device=fViewContent->QueryDevice();
	auto String=fTextData->ControlTextString();
	auto &Style=fTextData->ControlTextStyle();
	fCacheTextGraph=Device->CreateTextGraph(String,Length,Style,nullptr);
	if(fCacheTextGraph==nullptr)
		return;

	fTextGraphSize=fCacheTextGraph->GetSize();
}
//---------------------------------------------------------------------------
void vTextSelectBox::Paint(iUISimplePaintContext *Context)noexcept
{
	SetupTextCache();

	cUIPoint BtnSize;
	BtnSize.x=BtnSize.y=fPaintSize.y;
	
	cUIPoint BtnPos;
	BtnPos.x=fPaintSize.x-BtnSize.x;
	BtnPos.y=0;

	auto BtnState=fButtonData->ButtonState();
	// draw button image
	switch(BtnState){
	case ButtonState::Normal:
		Context->Fill(BtnPos,BtnSize,UIColorFromUInt32(0xFFCCCCCC));
		break;
	case ButtonState::Hot:
		Context->Fill(BtnPos,BtnSize,UIColorFromUInt32(0xFFFF0000));
		break;
	case ButtonState::Pressed:
		Context->Fill(BtnPos,BtnSize,UIColorFromUInt32(0xFF0000FF));
		break;
	case ButtonState::Inactive:
		Context->Fill(BtnPos,BtnSize,UIColorFromUInt32(0xFF00FFFF));
		break;
	case ButtonState::Disabled:
		Context->Fill(BtnPos,BtnSize,UIColorFromUInt32(0xFF000000));
		break;
	}

	if(fCacheTextGraph!=nullptr){
		cUIPoint TextAlignSize;
		TextAlignSize.x=fPaintSize.x-BtnSize.x;
		TextAlignSize.y=fPaintSize.y;
		//auto TextPos=fTextData->ControlTextPosition(TextAlignSize,fTextGraphSize);
		cUIPoint TextPos={0,0};

		Context->Graph(TextPos,fTextGraphSize,fCacheTextGraph);
	}

}
//---------------------------------------------------------------------------
void vTextSelectBox::Update(void)noexcept
{
	fUpdateText=true;
	fCacheTextGraph=nullptr;
	if(fViewContent!=nullptr){
		fViewContent->QueryUpdate();
	}
}
//---------------------------------------------------------------------------
void vTextSelectBox::UpdateState(void)noexcept
{
	if(fViewContent!=nullptr){
		fViewContent->QueryUpdate();
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cTextSelectBox::cTextSelectBox()noexcept
{
	fPopupWindowControl=cnSystem::CreatePopupWindowControl();
	fPopupWindowControl->SetCallback(this);

	auto PopupWindow=fPopupWindowControl->GetUIWindow();
	auto PopupView=fPopupList.CreateView();
	PopupWindow->SetClient(PopupView);

	fPopupList.TextList.AppendMake(u"1234");
	fPopupList.TextList.AppendMake(u"5678");
	fPopupList.TextList.AppendMake(u"9012");
	fPopupList.TextList.AppendMake(u"3456");
	fPopupList.TextList.AppendMake(u"7890");
	fPopupList.TextList.AppendMake(u"1234");
	fPopupList.Update();

	fPopupList.OnSelectionChanged=[=]{
		fPopupWindowControl->HidePopup();
		ControlTextNotifySet();
	};
}
//---------------------------------------------------------------------------
cTextSelectBox::~cTextSelectBox()noexcept
{
	fPopupWindowControl->SetCallback(nullptr);
}
//---------------------------------------------------------------------------
void cTextSelectBox::ButtonContentSetDefault(void)noexcept
{
	SetContent(vTextSelectBox::Create(this,this));
}
//---------------------------------------------------------------------------
bool cTextSelectBox::ButtonMouseAllowButton(eMouseButton Button)noexcept
{
	return Button==MouseButton::Left;
}
//---------------------------------------------------------------------------
void cTextSelectBox::ButtonClick(const cUIPoint &)noexcept
{
	PopupSelectWindow();
}
//---------------------------------------------------------------------------
void cTextSelectBox::PopupSelectWindow(void)noexcept
{
	auto PopupWindow=fPopupWindowControl->GetUIWindow();
	fPopupWindowControl->SetupOwner(fView);
	auto ViewSize=fView->GetSize();
	PopupWindow->SetPosition(fView,{0,ViewSize.y});

	PopupWindow->SetSize({ViewSize.x,200});

	fPopupWindowControl->ShowPopup();
	{
		auto Key=iCast<iUIKeyControl>(fPopupList.GetView());
		Key->SetFocus(false);
	}
}
//---------------------------------------------------------------------------
void cTextSelectBox::KeyInput(iUIKeyEvent *KeyEvent,eKeyCode KeyCode)noexcept
{
	bcTextButton::KeyInput(KeyEvent,KeyCode);
	switch(KeyCode){
	default:
		break;
	case KeyCode::Down:
		PopupSelectWindow();
		break;
	case KeyCode::Enter:
		PopupSelectWindow();
		break;
	}
}
//---------------------------------------------------------------------------
const uChar16* cTextSelectBox::ControlTextString(void)noexcept
{
	auto SelIndex=fPopupList.GetSelectIndex();
	if(static_cast<uIntn>(SelIndex)>=fPopupList.TextList.GetCount()){
		return nullptr;
	}
	return fPopupList.TextList[SelIndex]->Pointer;
}
//---------------------------------------------------------------------------
uIntn cTextSelectBox::ControlTextLength(void)noexcept
{
	auto SelIndex=fPopupList.GetSelectIndex();
	if(static_cast<uIntn>(SelIndex)>=fPopupList.TextList.GetCount()){
		return 0;
	}
	return fPopupList.TextList[SelIndex]->Length;
}
//---------------------------------------------------------------------------
void cTextSelectBox::PopupOnShow(void)noexcept
{
}
//---------------------------------------------------------------------------
void cTextSelectBox::PopupOnHide(void)noexcept
{
	auto Key=iCast<iUIKeyControl>(fView);
	Key->SetFocus(false);
}
//---------------------------------------------------------------------------
void cTextSelectBox::SelectConfirm(void)noexcept
{
	fPopupWindowControl->HidePopup();
	ControlTextNotifySet();
}
//---------------------------------------------------------------------------
void cTextSelectBox::SelectCancel(void)noexcept
{
	fPopupWindowControl->HidePopup();
	ControlTextNotifySet();
}
//---------------------------------------------------------------------------
cTextSelectBox* cTextSelectBox::cSelectList::GetHost(void)noexcept
{
	return cnMemory::GetObjectFromMemberPointer(this,&cTextSelectBox::fPopupList);
}
//---------------------------------------------------------------------------
void cTextSelectBox::cSelectList::KeyInput(iUIKeyEvent *KeyEvent,eKeyCode KeyCode)noexcept
{
	cTextList::KeyInput(KeyEvent,KeyCode);
	switch(KeyCode){
	default:
		break;
	case KeyCode::Enter:
		GetHost()->SelectConfirm();
		break;
	case KeyCode::Esc:
		GetHost()->SelectCancel();
		break;
	}
}
//---------------------------------------------------------------------------
