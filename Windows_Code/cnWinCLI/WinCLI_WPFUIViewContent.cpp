
#include "WinCLI_WPFUIViewContent.h"


using namespace cnLibrary;
using namespace cnRTL;
using namespace cnWin;

//---------------------------------------------------------------------------
iPtr<iUIFont> cnWin::DNetCreateFont(const uChar16 *Name,uIntn NameLength,eUIFontStyle FontStyle,Float32 FontWeight)noexcept(true)
{
	auto Typeface=iCreate<cWPFTypeface>();

	if(Typeface->Create(Name,NameLength,FontStyle,FontWeight)==false){
		return nullptr;
	}
	return Typeface;
}
//---------------------------------------------------------------------------
rPtr<iUITextLineLayout> cnWin::DNetCreateTextLineLayout(const uChar16 *Text,uIntn Length,const cUITextStyle &Style)noexcept(true)
{
	auto Layout=rCreate<cWPFTextLayout>();
	cUTFStringConverter<4,2> Converter;
	auto Text32=cnRTL::CreateStringConvertEncoding<uChar32>(Converter,Text,Length);
	Layout->Calculate(Text32->Pointer,Text32->Length,Style);

	return Layout;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWPFDrawingContext::cWPFDrawingContext()noexcept(true)
{
	fContext.mSetup();
}
//---------------------------------------------------------------------------
cWPFDrawingContext::~cWPFDrawingContext()noexcept(true)
{
	fContext.mClear();
}
//---------------------------------------------------------------------------
mcWPFDrawingContext& cWPFDrawingContext::RefDrawingContext(void)noexcept(true)
{
	return fContext;
}
//---------------------------------------------------------------------------
bool cWPFDrawingContext::Fill(cUIPoint DrawPosition,cUIPoint DrawSize,cUIColor Color)noexcept(true)
{
	return fContext.mFill(DrawPosition,DrawSize,Color);
}
//---------------------------------------------------------------------------
bool cWPFDrawingContext::Graph(cUIPoint DrawPosition,cUIPoint DrawSize,iUIGraph *Graph)noexcept(true)
{
	auto WPFSimpleGraph=iCast<miWPFUISimpleGraph>(Graph);
	if(WPFSimpleGraph!=nullptr){
		WPFSimpleGraph->Draw(fContext.GetContextHandle(),DrawPosition,DrawSize);
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------
bool cWPFDrawingContext::Image(cUIPoint DrawPosition,cUIPoint DrawSize,iUIBitmap *Bitmap,eImageBlendingOperator BlendingOperator)noexcept(true)
{
	return false;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWPFUIBitmap::cWPFUIBitmap()noexcept(true)
{
}
//---------------------------------------------------------------------------
cWPFUIBitmap::~cWPFUIBitmap()noexcept(true)
{
}
//---------------------------------------------------------------------------
cUIPoint cWPFUIBitmap::GetSize(void)noexcept(true)
{
	return fImageSource.mGetImageSize();
}
//---------------------------------------------------------------------------
void cWPFUIBitmap::TakeDrawing(cGCHandle &DrawingHandle)noexcept(true)
{
	fImageSource.mTakeDrawing(DrawingHandle);
}
//---------------------------------------------------------------------------
void cWPFUIBitmap::Draw(const cGCHandle &ContextHandle,cUIPoint DrawPosition,cUIPoint DrawSize)noexcept(true)
{
	return fImageSource.mDraw(ContextHandle,DrawPosition,DrawSize);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWPFBitmapDataSource::cWPFBitmapDataSource()noexcept(true)
{
}
//---------------------------------------------------------------------------
cWPFBitmapDataSource::~cWPFBitmapDataSource()noexcept(true)
{
}
//---------------------------------------------------------------------------
void cWPFBitmapDataSource::TakeDrawing(cGCHandle &DrawingHandle,int Width,int Height)noexcept(true)
{
	fBitmapSource.mTakeDrawing(DrawingHandle,Width,Height);
}
//---------------------------------------------------------------------------
cBitmapPixelFormat cWPFBitmapDataSource::GetPixelFormat(void)noexcept(true)
{
	return BitmapPixelFormatR8G8B8A8;
}
//---------------------------------------------------------------------------
cUIPoint cWPFBitmapDataSource::GetImageSize(void)noexcept(true)
{
	return fBitmapSource.mGetImageSize();
}
//---------------------------------------------------------------------------
uIntn cWPFBitmapDataSource::GetDataPitch(void)noexcept(true)
{
	return 0;
}
//---------------------------------------------------------------------------
uIntn cWPFBitmapDataSource::CopyPixelBuffer(uIntn Offset,void *Dest,uIntn DestSize)noexcept(true)
{
	return 0;
}
//---------------------------------------------------------------------------
const void* cWPFBitmapDataSource::AcquirePixels(void)noexcept(true)
{
	return nullptr;
}
//---------------------------------------------------------------------------
void cWPFBitmapDataSource::ReleasePixels(void)noexcept(true)
{
}
//---------------------------------------------------------------------------
bool cWPFBitmapDataSource::IsTopDown(void)noexcept(true)
{
	return false;
}
//---------------------------------------------------------------------------
void cWPFBitmapDataSource::Draw(const cGCHandle &ContextHandle,cUIPoint DrawPosition,cUIPoint DrawSize)noexcept(true)
{
	return fBitmapSource.mDraw(ContextHandle,DrawPosition,DrawSize);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWPFUISimpleBitmapCanvas::cWPFUISimpleBitmapCanvas(int Width,int Height)noexcept(true)
	: fBitmapWidth(Width)
	, fBitmapHeight(Height)
{
	fDrawingGroup.mSetup(false);
}
//---------------------------------------------------------------------------
cWPFUISimpleBitmapCanvas::~cWPFUISimpleBitmapCanvas()noexcept(true)
{
	fDrawingGroup.mClear();
}
//---------------------------------------------------------------------------
iUISimplePaintContext* cWPFUISimpleBitmapCanvas::StartContext(cUIColor Color)noexcept(true)
{
	auto &Context=fContext.RefDrawingContext();
	if(fDrawingGroup.mStartDraw(Context)==false)
		return nullptr;

	if(Color.Alpha>0){
		cUIPoint DrawPos;
		DrawPos.x=0;
		DrawPos.y=0;
		cUIPoint BitmapSize;
		BitmapSize.x=fBitmapWidth;
		BitmapSize.y=fBitmapHeight;
		Context.mFill(DrawPos,BitmapSize,Color);
	}
	return &fContext;
}
//---------------------------------------------------------------------------
void cWPFUISimpleBitmapCanvas::DiscardContext(void)noexcept(true)
{
	fDrawingGroup.mStopDraw(fContext.RefDrawingContext());
}
//---------------------------------------------------------------------------
iPtr<iUIBitmap> cWPFUISimpleBitmapCanvas::FinishBitmap(bool CopyContent)noexcept(true)
{
	if(fDrawingGroup.mStopDraw(fContext.RefDrawingContext())==false)
		return nullptr;
	auto RetBitmap=iCreate<cWPFUIBitmap>();
	RetBitmap->TakeDrawing(fDrawingGroup.Fetch());
	return RetBitmap;
}
//---------------------------------------------------------------------------
iPtr<iBitmapDataSource> cWPFUISimpleBitmapCanvas::FinishBitmapSource(bool CopyContent)noexcept(true)
{
	if(fDrawingGroup.mStopDraw(fContext.RefDrawingContext())==false)
		return nullptr;

	auto RetSource=iCreate<cWPFBitmapDataSource>();
	RetSource->TakeDrawing(fDrawingGroup.Fetch(),fBitmapWidth,fBitmapHeight);
	return RetSource;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWPFTypeface::cWPFTypeface()noexcept(true)
{
	fTypeface.mSetup();
}
//---------------------------------------------------------------------------
cWPFTypeface::~cWPFTypeface()noexcept(true)
{
	fTypeface.mClear();
}
//---------------------------------------------------------------------------
bool cWPFTypeface::Create(const uChar16 *Name,uIntn NameLength,eUIFontStyle FontStyle,Float32 FontWeight)noexcept(true)
{
	if(fTypeface.mCreate(Name,NameLength,FontStyle,FontWeight)==false){
		return false;
	}

	fName.SetString(Name,NameLength);
	fFontWeight=FontWeight;
	fFontStyle=FontStyle;
	return true;
}
//---------------------------------------------------------------------------
cArray<const uChar16> cWPFTypeface::GetName(void)noexcept(true)
{
	return fName.GetArray();
}
//---------------------------------------------------------------------------
eUIFontStyle cWPFTypeface::GetStyle(void)noexcept(true)
{
	return fFontStyle;
}
//---------------------------------------------------------------------------
Float32 cWPFTypeface::GetWeight(void)noexcept(true)
{
	return fFontWeight;
}
//---------------------------------------------------------------------------
const cGCHandle& cWPFTypeface::GetObjecHandle(void)noexcept(true)
{
	return fTypeface.GetHandle();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void cWPFTextLayout::Calculate(const uChar32 *Text,uIntn Length,const cUITextStyle &Style)noexcept(true)
{
	return mCalculate(Text,Length,Style);
}
//---------------------------------------------------------------------------
uIntn cWPFTextLayout::GetTextLength(void)noexcept(true)
{
	return fTextDistance.GetCount();
}
//---------------------------------------------------------------------------
Float32 cWPFTextLayout::GetTextHeight(void)noexcept(true)
{
	return fTextHeight;
}
//---------------------------------------------------------------------------
const Float32* cWPFTextLayout::GetTextDistance(void)noexcept(true)
{
	return fTextDistance;
}
//---------------------------------------------------------------------------
void cWPFTextLayout::TextLayoutStoreTextHeight(Float32 Height)noexcept(true)
{
	fTextHeight=Height;
}
//---------------------------------------------------------------------------
void cWPFTextLayout::TextLayoutMakeTextDistance(uIntn Count)noexcept(true)
{
	fTextDistance.SetCount(Count);
}
//---------------------------------------------------------------------------
void cWPFTextLayout::TextLayoutStoreTextDistance(uIntn Index,Float32 Distance)noexcept(true)
{
	fTextDistance[Index]=Distance;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWPFUISimpleTextGraph::cWPFUISimpleTextGraph()noexcept(true)
{
	fTextGraph.mSetup();
}
//---------------------------------------------------------------------------
cWPFUISimpleTextGraph::~cWPFUISimpleTextGraph()noexcept(true)
{
	fTextGraph.mClear();
}
//---------------------------------------------------------------------------
cUIPoint cWPFUISimpleTextGraph::GetSize(void)noexcept(true)
{
	return fTextSize;
}
//---------------------------------------------------------------------------
void cWPFUISimpleTextGraph::SetupText(const uChar32 *Text,uIntn Length,const cUITextStyle &Style,const Float32* TextDistance,Float32 TextMinHeight)noexcept(true)
{
	fTextSize=fTextGraph.mSetupText(Text,Length,Style,TextDistance,TextMinHeight);
}
//---------------------------------------------------------------------------
void cWPFUISimpleTextGraph::Draw(const cGCHandle &ContextHandle,cUIPoint DrawPosition,cUIPoint DrawSize)noexcept(true)
{
	return fTextGraph.mDraw(ContextHandle,DrawPosition,DrawSize);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWPFUIRichTextGraph::cWPFUIRichTextGraph()noexcept(true)
{
	fTextGraph.mSetup();
}
//---------------------------------------------------------------------------
cWPFUIRichTextGraph::~cWPFUIRichTextGraph()noexcept(true)
{
	fTextGraph.mClear();
}
//---------------------------------------------------------------------------
cUIPoint cWPFUIRichTextGraph::GetSize(void)noexcept(true)
{
	return fTextSize;
}
//---------------------------------------------------------------------------
void cWPFUIRichTextGraph::SetupText(const uChar16 *Text,uIntn Length,const cUITextStyle &Style,const Float32* TextDistance,Float32 TextMinHeight)noexcept(true)
{
	fTextSize=fTextGraph.mSetupText(Text,Length,Style,TextDistance,TextMinHeight);
}
//---------------------------------------------------------------------------
void cWPFUIRichTextGraph::Draw(const cGCHandle &ContextHandle,cUIPoint DrawPosition,cUIPoint DrawSize)noexcept(true)
{
	return fTextGraph.mDraw(ContextHandle,DrawPosition,DrawSize);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWPFUISimplePaintDevice::cWPFUISimplePaintDevice()noexcept(true)
{
}
//---------------------------------------------------------------------------
cWPFUISimplePaintDevice::~cWPFUISimplePaintDevice()noexcept(true)
{
}
//---------------------------------------------------------------------------
rPtr<iUISimpleBitmapCanvas> cWPFUISimplePaintDevice::CreateBitmapCanvas(cUIPoint Size)noexcept(true)
{
	return nullptr;
}
//---------------------------------------------------------------------------
iPtr<iUITextGraph> cWPFUISimplePaintDevice::CreateTextGraph(const uChar16 *Text,uIntn Length,const cUITextStyle &Style,const Float32* TextDistance,Float32 TextMinHeight)noexcept(true)
{
	cUTFStringConverter<4,2> Converter;
	auto Text32=cnRTL::CreateStringConvertEncoding<uChar32>(Converter,Text,Length);
	auto TextGraph=iCreate<cWPFUISimpleTextGraph>();
	TextGraph->SetupText(Text32->Pointer,Text32->Length,Style,TextDistance,TextMinHeight);
	return TextGraph;
}
//---------------------------------------------------------------------------
iPtr<iUIBitmap> cWPFUISimplePaintDevice::CreateBitmapCopyFromSource(iBitmapDataSource *Source)noexcept(true)
{
	return nullptr;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWPFUISimpleViewContent::cWPFUISimpleViewContent(iUISimplePainter *Painter)noexcept(true)
	: fPainter(Painter)
{
	fNeedNotifyRect=false;
	fWPFDrawing.mSetup(true);
}
//---------------------------------------------------------------------------
cWPFUISimpleViewContent::~cWPFUISimpleViewContent()noexcept(true)
{
	fWPFDrawing.mClear();
}
//---------------------------------------------------------------------------
iUIView* cWPFUISimpleViewContent::GetView(void)noexcept(true)
{
	return bcWPFViewContentDrawing::GetView();
}
//---------------------------------------------------------------------------
bool cWPFUISimpleViewContent::SetView(iUIView *View)noexcept(true)
{
	return bcWPFViewContentDrawing::SetView(View);
}
//---------------------------------------------------------------------------
bool cWPFUISimpleViewContent::GetVisible(void)noexcept(true)
{
	return bcWPFViewContentDrawing::GetVisible();
}
//---------------------------------------------------------------------------
bool cWPFUISimpleViewContent::SetVisible(bool Visible)noexcept(true)
{
	return bcWPFViewContentDrawing::SetVisible(Visible);
}
//---------------------------------------------------------------------------
Float32 cWPFUISimpleViewContent::GetZPosition(void)noexcept(true)
{
	return bcWPFViewContentDrawing::GetZPosition();
}
//---------------------------------------------------------------------------
bool cWPFUISimpleViewContent::SetZPosition(Float32 ZPosition)noexcept(true)
{
	return bcWPFViewContentDrawing::SetZPosition(ZPosition);
}
//---------------------------------------------------------------------------
Float32 cWPFUISimpleViewContent::GetContentScale(void)noexcept(true)
{
	return bcWPFViewContentDrawing::GetContentScale();
}
//---------------------------------------------------------------------------
rPtr<iUISimplePaintDevice> cWPFUISimpleViewContent::QueryDevice(void)noexcept(true)
{
	return rCreate<cWPFUISimplePaintDevice>();
}
//---------------------------------------------------------------------------
eUIState cWPFUISimpleViewContent::GetPaintState(void)noexcept(true)
{
	return UIState::Null;
}
//---------------------------------------------------------------------------
cUIPoint cWPFUISimpleViewContent::GetPaintSize(void)noexcept(true)
{
	return fViewContentSize;
}
//---------------------------------------------------------------------------
void cWPFUISimpleViewContent::QueryUpdate(void)noexcept(true)
{
	RenderVisual();
}
//---------------------------------------------------------------------------
const cGCHandle* cWPFUISimpleViewContent::GetDrawingPointer(void)noexcept(true)
{
	return &fWPFDrawing.GetHandle();
}
//---------------------------------------------------------------------------
void cWPFUISimpleViewContent::ViewContentUpdateRect(void)noexcept(true)
{
	//fWPFDrawing.mTransformScale(fLayoutScale);

	double Scale=fLayoutScale;
	double DrawX=fContentMargin.Left;
	double DrawY=fContentMargin.Top;

	double DrawWidth=fViewWidth-fContentMargin.Left-fContentMargin.Right;
	double DrawHeight=fViewHeight-fContentMargin.Left-fContentMargin.Right;
	fViewContentSize.x=static_cast<Float32>(DrawWidth);
	fViewContentSize.y=static_cast<Float32>(DrawHeight);
	DrawX/=Scale;
	DrawY/=Scale;
	
	fWPFDrawing.mTransformScaleOffset(DrawX,DrawY,Scale);

	fNeedNotifyRect=true;
	ViewContentUpdateState();
}
//---------------------------------------------------------------------------
void cWPFUISimpleViewContent::ViewContentUpdateState(void)noexcept(true)
{
	if(fPainter==nullptr)
		return;
	UpdatePainterState();

	RenderVisual();
}
//---------------------------------------------------------------------------
void cWPFUISimpleViewContent::ViewContentUpdateWindow(iWindow *Window)noexcept(true)
{
	//if(fPainter==nullptr)
	//	return;
	//
	//fWindow=Window;
	//fPainter->PaintWindowChanged();
	//
	//UpdatePainterState();
	//
	//Paint();
}
//---------------------------------------------------------------------------
eUIState cWPFUISimpleViewContent::CheckPainterState(void)noexcept(true)
{
	auto NewState=CheckViewContentState();
	switch(NewState){
	case UIState::Inactive:
	case UIState::Active:
		if(fViewWidth<=0 || fViewHeight<=0){
			NewState=UIState::Background;
		}
	}
	return NewState;
}
//---------------------------------------------------------------------------
void cWPFUISimpleViewContent::UpdatePainterState(void)noexcept(true)
{
	eUIState NewState=CheckPainterState();
	NotifyPainterState(fPainter,NewState,fPainterState);
	fPainterState=NewState;
}
//---------------------------------------------------------------------------
void cWPFUISimpleViewContent::NotifyPainterState(iUISimplePainter *Painter,eUIState NewState,eUIState OldState)noexcept(true)
{
	if(NewState==OldState)
		return;
	if(NewState>OldState){
		int IncCount=static_cast<int>(NewState)-static_cast<int>(OldState);

		switch(OldState){
		case UIState::Null:
			Painter->PaintStarted();
			if(--IncCount==0)
				break;
			cnLib_SWITCH_FALLTHROUGH;
		case UIState::Background:
			Painter->PaintShow();
			if(--IncCount==0)
				break;
			cnLib_SWITCH_FALLTHROUGH;
		case UIState::Inactive:
			Painter->PaintResume();
			break;
		}
	}
	else{
		int DecCount=static_cast<int>(OldState)-static_cast<int>(NewState);
		switch(OldState){
		case UIState::Active:
			Painter->PaintPaused();
			if(--DecCount==0)
				break;
			cnLib_SWITCH_FALLTHROUGH;
		case UIState::Inactive:
			Painter->PaintHide();
			if(--DecCount==0)
				break;
			cnLib_SWITCH_FALLTHROUGH;
		case UIState::Background:
			Painter->PaintStopped();
			break;
		}
	}
}
//---------------------------------------------------------------------------
void cWPFUISimpleViewContent::RenderVisual(void)noexcept(true)
{
	if(fPainterState<UIState::Inactive)
		return;

	if(fViewWidth<=0 || fViewHeight<=0){
		return;
	}

	if(fNeedNotifyRect){
		fNeedNotifyRect=false;
		fPainter->PaintRectChanged();
	}

	cWPFDrawingContext DrawContext;
	if(fWPFDrawing.mStartDraw(DrawContext.RefDrawingContext())){
		fPainter->Paint(&DrawContext);
		fWPFDrawing.mStopDraw(DrawContext.RefDrawingContext());
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWPFGDIThreadContext::cWPFGDIThreadContext()noexcept(true)
{
	auto UIThread=cWPFUIThread::CurrentUIThread();
	Device=rCreate<cnRTL::cnWinRTL::cDCPaintDevice>(UIThread);
}
//---------------------------------------------------------------------------
cWPFGDIThreadContext::~cWPFGDIThreadContext()noexcept(true)
{
}
//---------------------------------------------------------------------------
void cWPFGDIThreadContext::NotifyDisplayChanged(void)noexcept(true)
{
	//Device->UpdateDisplaySetting();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void cWPFGDIModule::WPFDisplaySettingsChanged(void)noexcept(true)
{
	return NotifyDisplayChanged();
}
//---------------------------------------------------------------------------
cWPFGDIModule::cWPFGDIModule()noexcept(true)
{
	mbcWPFGDI::mSetup();
}
//---------------------------------------------------------------------------
cWPFGDIModule::~cWPFGDIModule()noexcept(true)
{
	mbcWPFGDI::mClear();
}
//---------------------------------------------------------------------------
void cWPFGDIModule::NotifyDisplayChanged(void)noexcept(true)
{
	auto AutoLock=TakeLock(&fThreadCS);
	for(auto p : fThreadSet){
		p->NotifyDisplayChanged();
	}
}
//---------------------------------------------------------------------------
cWPFGDIThreadContext* cWPFGDIModule::QueryThreadContext(void)noexcept(true)
{
	auto pContext=gTLSContext->Get();
	if(pContext!=nullptr)
		return static_cast<cWPFGDIThreadContext*>(pContext);

	return gModule.MakeContext();
}
//---------------------------------------------------------------------------
cWPFGDIThreadContext* cWPFGDIModule::CurrentThreadContext(void)noexcept(true)
{
	auto pContext=gTLSContext->Get();
	if(pContext==nullptr)
		return nullptr;

	return static_cast<cWPFGDIThreadContext*>(pContext);
}
//---------------------------------------------------------------------------
void cWPFGDIModule::cThreadContextExitNotify::Execute(rPtr<iReference> Reference,void *Object)noexcept(true)
{
	auto Host=cnMemory::GetObjectFromMemberPointer(this,&cWPFGDIModule::fThreadContextExitNotify);
	Host->DestroyContext(static_cast<cWPFGDIThreadContext*>(Object));
}
//---------------------------------------------------------------------------
cWPFGDIThreadContext* cWPFGDIModule::MakeContext(void)noexcept(true)
{
	auto UIThread=cWPFUIThread::CurrentUIThread();
	if(UIThread==nullptr)
		return nullptr;
	auto NewContext=new cWPFGDIThreadContext();
	{
		auto AutoLock=TakeLock(&fThreadCS);
		fThreadSet.Insert(NewContext);
		gTLSContext->Set(nullptr,NewContext);
		gTLSContext->SetThreadExitNotify(&fThreadContextExitNotify);
	}

	return NewContext;
}
//---------------------------------------------------------------------------
void cWPFGDIModule::DestroyContext(cWPFGDIThreadContext *Context)noexcept(true)
{
	auto AutoLock=TakeLock(&fThreadCS);
	fThreadSet.Remove(Context);
	gTLSContext->Clear();
	delete Context;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWPFDCViewContent::cWPFDCViewContent(iDCPainter *Painter,bool OutputAlpha)noexcept(true)
	: fPainter(Painter)
	, fPainterOutputAlpha(OutputAlpha)
	, fPainterState(psNone)
{
	fDrawing.mSetup();
}
//---------------------------------------------------------------------------
cWPFDCViewContent::~cWPFDCViewContent()noexcept(true)
{
	fDrawing.mClear();
}
//---------------------------------------------------------------------------
const cGCHandle* cWPFDCViewContent::GetDrawingPointer(void)noexcept(true)
{
	return &fDrawing.GetDrawingHandle();
}
//---------------------------------------------------------------------------
eUIState cWPFDCViewContent::GetPaintState(void)noexcept(true)
{
	switch(fPainterState){
	default:
	case psNone:
		return UIState::Null;
	case psBackground:
		return UIState::Background;
	case psActive:
		return UIState::Active;
	}
}
//---------------------------------------------------------------------------
iUIView* cWPFDCViewContent::GetView(void)noexcept(true)
{
	return bcWPFViewContentDrawing::GetView();
}
//---------------------------------------------------------------------------
bool cWPFDCViewContent::SetView(iUIView *View)noexcept(true)
{
	return bcWPFViewContentDrawing::SetView(View);
}
//---------------------------------------------------------------------------
bool cWPFDCViewContent::GetVisible(void)noexcept(true)
{
	return bcWPFViewContentDrawing::GetVisible();
}
//---------------------------------------------------------------------------
bool cWPFDCViewContent::SetVisible(bool Visible)noexcept(true)
{
	return bcWPFViewContentDrawing::SetVisible(Visible);
}
//---------------------------------------------------------------------------
Float32 cWPFDCViewContent::GetZPosition(void)noexcept(true)
{
	return bcWPFViewContentDrawing::GetZPosition();
}
//---------------------------------------------------------------------------
bool cWPFDCViewContent::SetZPosition(Float32 ZPosition)noexcept(true)
{
	return bcWPFViewContentDrawing::SetZPosition(ZPosition);
}
//---------------------------------------------------------------------------
Float32 cWPFDCViewContent::GetContentScale(void)noexcept(true)
{
	return bcWPFViewContentDrawing::GetContentScale();
}
//---------------------------------------------------------------------------
RECT cWPFDCViewContent::GetPaintRect(void)noexcept(true)
{
	RECT rc;
	rc.left=0;
	rc.top=0;
	rc.right=fDrawWidth;
	rc.bottom=fDrawHeight;
	return rc;
}
//---------------------------------------------------------------------------
HDC	cWPFDCViewContent::GetDC(void)noexcept(true)
{
	if(fContentDC!=nullptr){
		fContentDC->RefCount++;
		return fContentDC->MemDC;
	}
	auto CPP=cWPFGDIModule::QueryThreadContext();
	fContentDC=new cContentDC;
	auto ScreenIC=CPP->Device->GetScreenIC();
	fContentDC->MemDC=::CreateCompatibleDC(ScreenIC->GetHandle());
	fContentDC->Buffer.Setup(fContentDC->MemDC,fDrawWidth,fDrawHeight);
	fContentDC->MemDCBitmap=::SelectObject(fContentDC->MemDC,fContentDC->Buffer.Bitmap);
	CPPPaintStart(fContentDC->Buffer.BitmapData,fContentDC->Buffer.Width,fContentDC->Buffer.Height);
	return fContentDC->MemDC;
}
//---------------------------------------------------------------------------
void cWPFDCViewContent::ReleaseDC(HDC DC)noexcept(true)
{
	if(fContentDC==nullptr){
		return;
	}
	fContentDC->RefCount--;
	if(fContentDC->RefCount!=0){
		return;
	}

	CPPPaintFinish(fContentDC->Buffer.BitmapData,fContentDC->Buffer.Width,fContentDC->Buffer.Height);

	// apply image
	SetupBitmap(DC);

	// copy to local buffer
	cnMemory::Copy(fBitmapBuffer.BitmapData,fContentDC->Buffer.BitmapData,fBitmapBuffer.BitmapSize);

	fDrawing.mApplyVisual(fBitmapBuffer.Section,fBitmapBuffer.Stride,fDrawWidth,fDrawHeight);
	

	// delete dc
	::SelectObject(fContentDC->MemDC,fContentDC->MemDCBitmap);
	::DeleteDC(fContentDC->MemDC);
	delete fContentDC;
	fContentDC=nullptr;
}
//---------------------------------------------------------------------------
HDC	cWPFDCViewContent::GetIC(void)noexcept(true)
{
	if(fTempIC==nullptr){
		auto GDIThreadCPP=cWPFGDIModule::QueryThreadContext();
		fTempIC=GDIThreadCPP->Device->GetScreenIC();
		fICRef=1;
	}
	else{
		fICRef++;
	}
	return fTempIC->GetHandle();
}
//---------------------------------------------------------------------------
void cWPFDCViewContent::ReleaseIC(HDC DC)noexcept(true)
{
	if(fTempIC==nullptr)
		return;

	if(DC!=fTempIC->GetHandle())
		return;

	fICRef--;
	if(fICRef==0){
		fTempIC=nullptr;
	}
}
//---------------------------------------------------------------------------
void cWPFDCViewContent::InvalidateRect(const RECT*)noexcept(true)
{
	SetNeedPaint();
}
//---------------------------------------------------------------------------
iWindow* cWPFDCViewContent::GetWindow(void)noexcept(true)
{
	return fWindow;
}
//---------------------------------------------------------------------------
cWPFDCViewContent::ePainterState cWPFDCViewContent::CheckPainterState(void)noexcept(true)
{
	switch(CheckViewContentState()){
	default:
	case UIState::Null:
		return psNone;
	case UIState::Background:
		return psBackground;
	case UIState::Inactive:
	case UIState::Active:
		if(fDrawWidth<=0 || fDrawHeight<=0){
			return psBackground;
		}
		return psActive;
	}
}
//---------------------------------------------------------------------------
void cWPFDCViewContent::NotifyPainterState(iDCPainter *Painter,ePainterState NewState,ePainterState OldState)noexcept(true)
{
	if(NewState==OldState)
		return;
	if(NewState>OldState){
		int IncCount=static_cast<int>(NewState-OldState);

		switch(OldState){
		case psNone:
			Painter->PaintStarted();
			if(--IncCount==0)
				break;
			cnLib_SWITCH_FALLTHROUGH;
		case psBackground:
			Painter->PaintShow();
			break;
		}
	}
	else{
		int DecCount=static_cast<int>(OldState-NewState);
		switch(OldState){
		case psActive:
			Painter->PaintHide();
			if(--DecCount==0)
				break;
			cnLib_SWITCH_FALLTHROUGH;
		case psBackground:
			Painter->PaintStopped();
			break;
		}
	}
}
//---------------------------------------------------------------------------
void cWPFDCViewContent::UpdatePainterState(void)noexcept(true)
{
	ePainterState NewState=CheckPainterState();
	NotifyPainterState(fPainter,NewState,fPainterState);
	fPainterState=NewState;
}
//---------------------------------------------------------------------------
void cWPFDCViewContent::ViewContentUpdateState(void)noexcept(true)
{
	if(fPainter==nullptr)
		return;
	UpdatePainterState();

	Paint();
}
//---------------------------------------------------------------------------
void cWPFDCViewContent::ViewContentUpdateWindow(iWindow *Window)noexcept(true)
{
	if(fPainter==nullptr)
		return;

	fWindow=Window;
	fPainter->PaintWindowChanged();

	UpdatePainterState();

	Paint();
}
//---------------------------------------------------------------------------
void cWPFDCViewContent::ViewContentUpdateRect(void)noexcept(true)
{
	fNeedNotifyRect=true;
	fNeedPaint=true;

	double Scale=fLayoutScale;
	double DrawX=fContentMargin.Left;
	double DrawY=fContentMargin.Top;

	double DrawWidth=fViewWidth-fContentMargin.Left-fContentMargin.Right;
	double DrawHeight=fViewHeight-fContentMargin.Left-fContentMargin.Right;
	fDrawWidth=static_cast<int>(DrawWidth);
	fDrawHeight=static_cast<int>(DrawHeight);
	DrawX/=Scale;
	DrawY/=Scale;
	DrawWidth/=Scale;
	DrawHeight/=Scale;
	fDrawing.mUpdateRect(DrawX,DrawY,DrawWidth,DrawHeight);

	ViewContentUpdateState();
}
//---------------------------------------------------------------------------
void cWPFDCViewContent::Paint(void)noexcept(true)
{
	if(fPainterState!=psActive)
		return;

	if(fNeedNotifyRect){
		fNeedNotifyRect=false;
		fPainter->PaintRectChanged();
	}

	if(fNeedPaint==false){
		return;
	}

	if(fDrawWidth<=0 || fDrawHeight<=0){
		return;
	}

	fNeedPaint=false;

	auto GDICPPContext=cWPFGDIModule::QueryThreadContext();
	// paint
	CPPPaint(GDICPPContext);

	fDrawing.mApplyVisual(fBitmapBuffer.Section,fBitmapBuffer.Stride,fDrawWidth,fDrawHeight);
}
//---------------------------------------------------------------------------
void cWPFDCViewContent::SetNeedPaint(void)noexcept(true)
{
	if(fPainter==nullptr)
		return;
	fNeedPaint=true;
	if(fView==nullptr){
		return;
	}
	Paint();
}
//---------------------------------------------------------------------------
void cWPFDCViewContent::CPPPaintStart(void *BitmapData,int Width,int Height)noexcept(true)
{
	auto Pixels=static_cast<uInt32*>(BitmapData);
	uIntn PixelCount=Width*Height;

	if(fPainterOutputAlpha){
		cnMemory::ZeroFill(Pixels,4*PixelCount);
	}
	else{
		cnMemory::Fill(Pixels,PixelCount,static_cast<uInt32>(0x01000000));
	}
}
//---------------------------------------------------------------------------
void cWPFDCViewContent::CPPPaintFinish(void *BitmapData,int Width,int Height)noexcept(true)
{
	if(fPainterOutputAlpha){
	}
	else{
		auto Pixels=static_cast<uInt32*>(BitmapData);
		uIntn PixelCount=Width*Height;
		for(uIntn i=0;i<PixelCount;i++){
			Pixels[i]+=0xFF000000;
		}
	}
}
//---------------------------------------------------------------------------
void cWPFDCViewContent::SetupBitmap(HDC DC)noexcept(true)
{
	// grow bitmap if needed
	int NewWidth=fBitmapBuffer.Width;
	int NewHeight=fBitmapBuffer.Height;

	bool GrowBitmap=false;
	if(NewWidth<fDrawWidth){
		NewWidth=fDrawWidth;
		GrowBitmap=true;
	}
	if(NewHeight<fDrawHeight){
		NewHeight=fDrawHeight;
		GrowBitmap=true;
	}
	if(GrowBitmap){
		// make top down bitmap
		fBitmapBuffer.Setup(DC,NewWidth,-NewHeight);
	}
}
//---------------------------------------------------------------------------
void cWPFDCViewContent::CPPPaint(cWPFGDIThreadContext *CPP)noexcept(true)
{
	auto TempDC=CPP->Device->QueryMemDC();
	HDC DC=TempDC->GetHandle();
	SetupBitmap(DC);

	CPPPaintStart(fBitmapBuffer.BitmapData,fBitmapBuffer.Width,fBitmapBuffer.Height);

	auto OldBmp=::SelectObject(DC,fBitmapBuffer.Bitmap);
	fPainter->Paint(DC,nullptr);
	::SelectObject(DC,OldBmp);

	CPPPaintFinish(fBitmapBuffer.BitmapData,fBitmapBuffer.Width,fBitmapBuffer.Height);
	
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cD3D9ExWindowDevice::cD3D9ExWindowDevice(iWindow *Window)noexcept(true)
	: fWindow(Window)
{
	fWindow->InsertMessageHandler(this);
}
//---------------------------------------------------------------------------
cD3D9ExWindowDevice::~cD3D9ExWindowDevice()noexcept(true)
{
	fWindow->RemoveMessageHandler(this);
}
//---------------------------------------------------------------------------
IDirect3DDevice9Ex* cD3D9ExWindowDevice::GetDevice(void)const noexcept(true)
{
	return fDevice;
}
//---------------------------------------------------------------------------
const D3DPRESENT_PARAMETERS cD3D9ExWindowDevice::DefaultPresentParameter={
	1,						// BackBufferWidth;
	1,						// BackBufferHeight;
	D3DFMT_UNKNOWN,			// BackBufferFormat;
	0,						// BackBufferCount;
	D3DMULTISAMPLE_NONE,	// MultiSampleType;
	0,						// MultiSampleQuality;
	D3DSWAPEFFECT_DISCARD,	// SwapEffect;
	nullptr,				// hDeviceWindow;
	TRUE,					// Windowed;
	FALSE,					// EnableAutoDepthStencil;
	D3DFMT_UNKNOWN,			// AutoDepthStencilFormat;
	0,						// Flags;
	0,						// FullScreen_RefreshRateInHz;
	D3DPRESENT_INTERVAL_IMMEDIATE,// PresentationInterval;
};
//---------------------------------------------------------------------------
void cD3D9ExWindowDevice::WindowAttached(void)noexcept(true)
{
	HRESULT hr;
	
	COMPtr<IDirect3D9Ex> D3D9Ex;
	hr=Direct3DCreate9Ex(D3D_SDK_VERSION,COMRetPtrT(D3D9Ex));

	HWND WindowHandle=fWindow->GetWindowHandle();

    DWORD dwVertexProcessing;
	D3DCAPS9 Caps;
	D3D9Ex->GetDeviceCaps(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,&Caps);
    if ((Caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) == D3DDEVCAPS_HWTRANSFORMANDLIGHT)
    {
        dwVertexProcessing = D3DCREATE_HARDWARE_VERTEXPROCESSING;
    }
    else
    {
        dwVertexProcessing = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
    }


	D3DPRESENT_PARAMETERS D3Dp=DefaultPresentParameter;
	D3Dp.hDeviceWindow=WindowHandle;
	hr=D3D9Ex->CreateDeviceEx( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,NULL,dwVertexProcessing|D3DCREATE_PUREDEVICE,&D3Dp,nullptr,COMRetPtrT(fDevice));
	//hr=D3D9Ex->CreateDeviceEx( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,NULL,D3DCREATE_HARDWARE_VERTEXPROCESSING,&D3Dp,nullptr,COMRetPtrT(fDevice));
	//bool SysTextureAllowed=(Caps.DevCaps & D3DDEVCAPS_TEXTURESYSTEMMEMORY)!=0;

}
//---------------------------------------------------------------------------
void cD3D9ExWindowDevice::WindowDetached(void)noexcept(true)
{
	fDevice=nullptr;
}
//---------------------------------------------------------------------------
bool cD3D9ExWindowDevice::WindowMessage(LRESULT&,const cWindowMessageParam&)noexcept(true)
{
	return false;
}
//---------------------------------------------------------------------------
void cD3D9ExWindowDevice::WindowMessageProcessed(LRESULT,const cWindowMessageParam&)noexcept(true)
{
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWPFDXGIViewContent::cWPFDXGIViewContent(iDXGIPainter *Painter)noexcept(true)
	: fPainter(Painter)
	, fPainterState(UIState::Null)
{
	mbcWPFD3DImageDrawing::mSetup();
}
//---------------------------------------------------------------------------
cWPFDXGIViewContent::~cWPFDXGIViewContent()noexcept(true)
{
	mbcWPFD3DImageDrawing::mClear();
}
//---------------------------------------------------------------------------
const cGCHandle* cWPFDXGIViewContent::GetDrawingPointer(void)noexcept(true)
{
	return &fDrawingHandle;
}
//---------------------------------------------------------------------------
eUIState cWPFDXGIViewContent::GetPaintState(void)noexcept(true)
{
	return fPainterState;
}
//---------------------------------------------------------------------------
cUIPoint cWPFDXGIViewContent::GetPaintSize(void)noexcept(true)
{
	cUIPoint Size;
	Size.x=fDrawWidth;
	Size.y=fDrawHeight;
	return Size;
}
//---------------------------------------------------------------------------
bool cWPFDXGIViewContent::GetRenderBufferSharedHandle(HANDLE &SharedHandle)noexcept(true)
{
	SharedHandle=fSurfaceSharedHandle;
	return true;
}
//---------------------------------------------------------------------------
void cWPFDXGIViewContent::UpdateRenderBuffer(void)noexcept(true)
{
	mPresent(fDrawWidth,fDrawHeight);
}
//---------------------------------------------------------------------------
iUIView* cWPFDXGIViewContent::GetView(void)noexcept(true)
{
	return bcWPFViewContentDrawing::GetView();
}
//---------------------------------------------------------------------------
bool cWPFDXGIViewContent::SetView(iUIView *View)noexcept(true)
{
	return bcWPFViewContentDrawing::SetView(View);
}
//---------------------------------------------------------------------------
bool cWPFDXGIViewContent::GetVisible(void)noexcept(true)
{
	return bcWPFViewContentDrawing::GetVisible();
}
//---------------------------------------------------------------------------
bool cWPFDXGIViewContent::SetVisible(bool Visible)noexcept(true)
{
	return bcWPFViewContentDrawing::SetVisible(Visible);
}
//---------------------------------------------------------------------------
Float32 cWPFDXGIViewContent::GetZPosition(void)noexcept(true)
{
	return bcWPFViewContentDrawing::GetZPosition();
}
//---------------------------------------------------------------------------
bool cWPFDXGIViewContent::SetZPosition(Float32 ZPosition)noexcept(true)
{
	return bcWPFViewContentDrawing::SetZPosition(ZPosition);
}
//---------------------------------------------------------------------------
Float32 cWPFDXGIViewContent::GetContentScale(void)noexcept(true)
{
	return bcWPFViewContentDrawing::GetContentScale();
}
//---------------------------------------------------------------------------
eUIState cWPFDXGIViewContent::CheckPainterState(void)noexcept(true)
{
	eUIState ContentState=CheckViewContentState();
	if(ContentState>UIState::Background){
		if(fDrawWidth<=0 || fDrawHeight<=0 || fFrontBufferAvailable==false){
			ContentState=UIState::Background;
		}
	}
	return ContentState;
}
//---------------------------------------------------------------------------
void cWPFDXGIViewContent::UpdatePainterState(void)noexcept(true)
{
	eUIState NewState=CheckPainterState();
	if(fPainterState==NewState)
		return;

	if(NewState>fPainterState){
		int IncCount=static_cast<int>(NewState)-static_cast<int>(fPainterState);

		switch(fPainterState){
		case UIState::Null:
			fPainter->PaintStarted();
			if(--IncCount==0)
				break;
			cnLib_SWITCH_FALLTHROUGH;
		case UIState::Background:
			SetupBackBuffer();

			fPainter->PaintShow();
			if(--IncCount==0)
				break;
			cnLib_SWITCH_FALLTHROUGH;
		case UIState::Inactive:
			fPainter->PaintResume();
			break;
		}
	}
	else{
		int DecCount=static_cast<int>(fPainterState)-static_cast<int>(NewState);
		switch(fPainterState){
		case UIState::Active:
			fPainter->PaintPaused();
			if(--DecCount==0)
				break;
			cnLib_SWITCH_FALLTHROUGH;
		case UIState::Inactive:
			fPainter->PaintHide();

			ClearBackBuffer();

			if(--DecCount==0)
				break;
			cnLib_SWITCH_FALLTHROUGH;
		case UIState::Background:
			fPainter->PaintStopped();
			break;
		}
	}
	fPainterState=NewState;
}
//---------------------------------------------------------------------------
void cWPFDXGIViewContent::ViewContentUpdateState(void)noexcept(true)
{
	if(fPainter==nullptr)
		return;
	UpdatePainterState();
}
//---------------------------------------------------------------------------
void cWPFDXGIViewContent::ViewContentUpdateWindow(iWindow *Window)noexcept(true)
{
	if(fPainter==nullptr)
		return;

	fWindow=Window;
	if(fWindow!=nullptr){
		fD3D9ExDevice.Affix(fWindow);
	}
	else{
		fD3D9ExDevice.Clear();
	}
	UpdatePainterState();
}
//---------------------------------------------------------------------------
void cWPFDXGIViewContent::ViewContentUpdateRect(void)noexcept(true)
{
	double Scale=fLayoutScale;
	double DrawX=fContentMargin.Left;
	double DrawY=fContentMargin.Top;

	double DrawWidth=fViewWidth-fContentMargin.Left-fContentMargin.Right;
	double DrawHeight=fViewHeight-fContentMargin.Left-fContentMargin.Right;
	fDrawWidth=static_cast<int>(DrawWidth);
	fDrawHeight=static_cast<int>(DrawHeight);
	DrawX/=Scale;
	DrawY/=Scale;
	DrawWidth/=Scale;
	DrawHeight/=Scale;
	mUpdateRect(DrawX,DrawY,DrawWidth,DrawHeight);


	if(fPainter!=nullptr)
		fPainter->PaintSizeChanged();


	if(fPainterState>UIState::Background){
		ClearBackBuffer();
		SetupBackBuffer();
	}
}
//---------------------------------------------------------------------------
void cWPFDXGIViewContent::D3DFrontBufferAvailableChanged(void)noexcept(true)
{
	UpdatePainterState();
}
//---------------------------------------------------------------------------
void cWPFDXGIViewContent::SetupBackBuffer(void)noexcept(true)
{
	if(fSurface!=nullptr){
		mApplyBackBuffer(fSurface,fViewWidth,fViewHeight);
		return;
	}
	if(fD3D9ExDevice==nullptr)
		return;
	auto Device=fD3D9ExDevice->GetDevice();
	if(Device==nullptr)
		return;

	Device->CreateRenderTarget(
		fDrawWidth,
		fDrawHeight,
		D3DFMT_A8R8G8B8,
		D3DMULTISAMPLE_NONE,
		0,
		FALSE,
		COMRetPtrT(fSurface),
		&fSurfaceSharedHandle
	);
	mSetBackBuffer(fSurface);
	
	if(fPainter!=nullptr){
		fPainter->RenderBufferChanged();
	}
}
//---------------------------------------------------------------------------
void cWPFDXGIViewContent::ClearBackBuffer(void)noexcept(true)
{
	fSurface=nullptr;
	fSurfaceSharedHandle=nullptr;
	mSetBackBuffer(nullptr);
}
//---------------------------------------------------------------------------
