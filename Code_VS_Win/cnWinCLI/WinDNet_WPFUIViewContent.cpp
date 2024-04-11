
#include "WinDNet_WPFUIViewContent.h"


using namespace cnLibrary;
using namespace cnRTL;
using namespace cnWin;

//---------------------------------------------------------------------------
iPtr<iUIFont> cnWin::DNetCreateFont(const uChar16 *Name,uIntn NameLength,eUIFontStyle FontStyle,Float32 FontWeight)noexcept
{
	return iCreate<cWPFTypeface>(mcWPFTypeface::CreateTypeface(Name,NameLength,FontStyle,FontWeight));
}
//---------------------------------------------------------------------------
rPtr<iUITextLineLayout> cnWin::DNetCreateTextLineLayout(const uChar16 *Text,uIntn Length,const cUITextStyle &Style)noexcept
{
	auto Layout=rCreate<cWPFTextLayout>();
	cUTFStringConverter<4,2> Converter;
	auto Text32=cnRTL::CreateStringConvertEncoding<uChar32>(Converter,Text,Length);
	Layout->Setup(Text32->Pointer,Text32->Length,Style);

	return Layout;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool mcWPFDrawingContext::Fill(cUIPoint DrawPosition,cUIPoint DrawSize,cUIColor Color)noexcept
{
	return mFill(DrawPosition,DrawSize,Color);
}
//---------------------------------------------------------------------------
bool mcWPFDrawingContext::Graph(cUIPoint DrawPosition,cUIPoint DrawSize,iUIGraph *Graph)noexcept
{
	auto WPFSimpleGraph=iCast<miWPFUISimpleGraph>(Graph);
	if(WPFSimpleGraph!=nullptr){
		WPFSimpleGraph->Draw(Handle,DrawPosition,DrawSize);
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------
bool mcWPFDrawingContext::Image(cUIPoint DrawPosition,cUIPoint DrawSize,iUIBitmap *Bitmap,eImageBlendingOperator BlendingOperator)noexcept
{
	return false;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWPFUIBitmap::cWPFUIBitmap(mcWPFUIBitmap &&Bitmap)noexcept
	: fBitmap(cnVar::MoveCast(Bitmap))
{
}
//---------------------------------------------------------------------------
cWPFUIBitmap::~cWPFUIBitmap()noexcept
{
}
//---------------------------------------------------------------------------
cUIPoint cWPFUIBitmap::GetSize(void)noexcept
{
	return fBitmap.ImageSize;
}
//---------------------------------------------------------------------------
void cWPFUIBitmap::Draw(cGCRef &ContextHandle,cUIPoint DrawPosition,cUIPoint DrawSize)noexcept
{
	return fBitmap.Draw(ContextHandle,DrawPosition,DrawSize);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWPFBitmapDataSource::cWPFBitmapDataSource(mcWPFBitmapDataSource &&BitmapSource)noexcept
	: fBitmapSource(cnVar::MoveCast(BitmapSource))
{
}
//---------------------------------------------------------------------------
cWPFBitmapDataSource::~cWPFBitmapDataSource()noexcept
{
}
//---------------------------------------------------------------------------
cBitmapPixelFormat cWPFBitmapDataSource::GetPixelFormat(void)noexcept
{
	return BitmapPixelFormatR8G8B8A8;
}
//---------------------------------------------------------------------------
cUIPoint cWPFBitmapDataSource::GetImageSize(void)noexcept
{
	return fBitmapSource.ImageSize;
}
//---------------------------------------------------------------------------
uIntn cWPFBitmapDataSource::GetDataPitch(void)noexcept
{
	return 0;
}
//---------------------------------------------------------------------------
uIntn cWPFBitmapDataSource::CopyPixelBuffer(uIntn Offset,void *Dest,uIntn DestSize)noexcept
{
	return 0;
}
//---------------------------------------------------------------------------
const void* cWPFBitmapDataSource::AcquirePixels(void)noexcept
{
	return nullptr;
}
//---------------------------------------------------------------------------
void cWPFBitmapDataSource::ReleasePixels(void)noexcept
{
}
//---------------------------------------------------------------------------
bool cWPFBitmapDataSource::IsTopDown(void)noexcept
{
	return false;
}
//---------------------------------------------------------------------------
void cWPFBitmapDataSource::Draw(cGCRef &ContextHandle,cUIPoint DrawPosition,cUIPoint DrawSize)noexcept
{
	return fBitmapSource.Draw(ContextHandle,DrawPosition,DrawSize);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWPFUISimpleBitmapCanvas::cWPFUISimpleBitmapCanvas(cUIPoint Size)noexcept
	: mcWPFUISimpleBitmapCanvas(Size)
{
}
//---------------------------------------------------------------------------
cWPFUISimpleBitmapCanvas::~cWPFUISimpleBitmapCanvas()noexcept
{
}
//---------------------------------------------------------------------------
iUISimplePaintContext* cWPFUISimpleBitmapCanvas::StartContext(cUIColor Color)noexcept
{
	mcWPFUISimpleBitmapCanvas::StartContext(fContext,Color);
	return &fContext;
}
//---------------------------------------------------------------------------
void cWPFUISimpleBitmapCanvas::DiscardContext(void)noexcept
{
	return mcWPFUISimpleBitmapCanvas::DiscardContext(fContext);
}
//---------------------------------------------------------------------------
iPtr<iUIBitmap> cWPFUISimpleBitmapCanvas::FinishBitmap(bool CopyContent)noexcept
{
	bool Success;
	auto Drawing=mcWPFUISimpleBitmapCanvas::FinishContext(fContext,Success);
	if(!Success)
		return nullptr;

	return iCreate<cWPFUIBitmap>(mcWPFUIBitmap::FromDrawing(Drawing));
}
//---------------------------------------------------------------------------
iPtr<iBitmapDataSource> cWPFUISimpleBitmapCanvas::FinishBitmapSource(bool CopyContent)noexcept
{
	bool Success;
	auto Drawing=mcWPFUISimpleBitmapCanvas::FinishContext(fContext,Success);
	if(!Success)
		return nullptr;

	return iCreate<cWPFBitmapDataSource>(mcWPFBitmapDataSource::FromDrawing(Drawing,fBitmapSize));
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWPFTypeface::cWPFTypeface(mcWPFTypeface &&Typeface)noexcept
	: fTypeface(cnVar::MoveCast(Typeface))
{
}
//---------------------------------------------------------------------------
cWPFTypeface::~cWPFTypeface()noexcept
{
}
//---------------------------------------------------------------------------
rPtr< iArrayReference<const uChar16> > cWPFTypeface::GetName(void)noexcept
{
	return nullptr;
}
//---------------------------------------------------------------------------
eUIFontStyle cWPFTypeface::GetStyle(void)noexcept
{
	return fTypeface.FontStyle;
}
//---------------------------------------------------------------------------
Float32 cWPFTypeface::GetWeight(void)noexcept
{
	return fTypeface.FontWeight;
}
//---------------------------------------------------------------------------
cGCRef& cWPFTypeface::GetObjecHandle(void)noexcept
{
	return fTypeface.Handle;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
uIntn cWPFTextLayout::GetTextLength(void)noexcept
{
	return TextDistance.GetCount();
}
//---------------------------------------------------------------------------
Float32 cWPFTextLayout::GetTextHeight(void)noexcept
{
	return TextHeight;
}
//---------------------------------------------------------------------------
const Float32* cWPFTextLayout::GetTextDistance(void)noexcept
{
	return TextDistance;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWPFUISimpleTextGraph::cWPFUISimpleTextGraph()noexcept
{
}
//---------------------------------------------------------------------------
cWPFUISimpleTextGraph::~cWPFUISimpleTextGraph()noexcept
{
}
//---------------------------------------------------------------------------
cUIPoint cWPFUISimpleTextGraph::GetSize(void)noexcept
{
	return fTextSize;
}
//---------------------------------------------------------------------------
void cWPFUISimpleTextGraph::Draw(cGCRef &ContextHandle,cUIPoint DrawPosition,cUIPoint DrawSize)noexcept
{
	return mcWPFUISimpleTextGraph::Draw(ContextHandle,DrawPosition,DrawSize);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWPFUIRichTextGraph::cWPFUIRichTextGraph()noexcept
{
}
//---------------------------------------------------------------------------
cWPFUIRichTextGraph::~cWPFUIRichTextGraph()noexcept
{
}
//---------------------------------------------------------------------------
cUIPoint cWPFUIRichTextGraph::GetSize(void)noexcept
{
	return fTextSize;
}
//---------------------------------------------------------------------------
void cWPFUIRichTextGraph::Draw(cGCRef &ContextHandle,cUIPoint DrawPosition,cUIPoint DrawSize)noexcept
{
	return mcWPFUIRichTextGraph::Draw(ContextHandle,DrawPosition,DrawSize);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWPFUISimplePaintDevice::cWPFUISimplePaintDevice()noexcept
{
}
//---------------------------------------------------------------------------
cWPFUISimplePaintDevice::~cWPFUISimplePaintDevice()noexcept
{
}
//---------------------------------------------------------------------------
rPtr<iUISimpleBitmapCanvas> cWPFUISimplePaintDevice::CreateBitmapCanvas(cUIPoint Size)noexcept
{
	return nullptr;
}
//---------------------------------------------------------------------------
iPtr<iUITextGraph> cWPFUISimplePaintDevice::CreateTextGraph(const uChar16 *Text,uIntn Length,const cUITextStyle &Style,const Float32* TextDistance,Float32 TextMinHeight)noexcept
{
	cUTFStringConverter<4,2> Converter;
	auto Text32=cnRTL::CreateStringConvertEncoding<uChar32>(Converter,Text,Length);
	auto TextGraph=iCreate<cWPFUISimpleTextGraph>();
	TextGraph->SetupText(Text32->Pointer,Text32->Length,Style,TextDistance,TextMinHeight);
	return TextGraph;
}
//---------------------------------------------------------------------------
iPtr<iUIBitmap> cWPFUISimplePaintDevice::CreateBitmapCopyFromSource(iBitmapDataSource *Source)noexcept
{
	return nullptr;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cGCRef& mcWPFUIViewContentDrawingGroup::GetDrawingGroup(void)noexcept
{
	return fDrawingGroup;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWPFUISimpleViewContent::cWPFUISimpleViewContent(iUISimplePainter *Painter)noexcept
	: fPainter(Painter)
{
}
//---------------------------------------------------------------------------
cWPFUISimpleViewContent::~cWPFUISimpleViewContent()noexcept
{
}
//---------------------------------------------------------------------------
iUIView* cWPFUISimpleViewContent::GetView(void)noexcept
{
	return bcWPFViewContentDrawing::GetView();
}
//---------------------------------------------------------------------------
bool cWPFUISimpleViewContent::SetView(iUIView *View)noexcept
{
	return bcWPFViewContentDrawing::SetView(View);
}
//---------------------------------------------------------------------------
bool cWPFUISimpleViewContent::GetVisible(void)noexcept
{
	return bcWPFViewContentDrawing::GetVisible();
}
//---------------------------------------------------------------------------
bool cWPFUISimpleViewContent::SetVisible(bool Visible)noexcept
{
	return bcWPFViewContentDrawing::SetVisible(Visible);
}
//---------------------------------------------------------------------------
Float32 cWPFUISimpleViewContent::GetZPosition(void)noexcept
{
	return bcWPFViewContentDrawing::GetZPosition();
}
//---------------------------------------------------------------------------
bool cWPFUISimpleViewContent::SetZPosition(Float32 ZPosition)noexcept
{
	return bcWPFViewContentDrawing::SetZPosition(ZPosition);
}
//---------------------------------------------------------------------------
Float32 cWPFUISimpleViewContent::GetContentScale(void)noexcept
{
	return bcWPFViewContentDrawing::GetContentScale();
}
//---------------------------------------------------------------------------
rPtr<iUISimplePaintDevice> cWPFUISimpleViewContent::QueryDevice(void)noexcept
{
	return rCreate<cWPFUISimplePaintDevice>();
}
//---------------------------------------------------------------------------
eUIState cWPFUISimpleViewContent::GetPaintState(void)noexcept
{
	return UIState::Null;
}
//---------------------------------------------------------------------------
cUIPoint cWPFUISimpleViewContent::GetPaintSize(void)noexcept
{
	return fViewContentSize;
}
//---------------------------------------------------------------------------
void cWPFUISimpleViewContent::QueryUpdate(void)noexcept
{
	RenderVisual();
}
//---------------------------------------------------------------------------
cGCRef* cWPFUISimpleViewContent::GetDrawingPointer(void)noexcept
{
	return &fWPFDrawing.GetDrawingGroup();
}
//---------------------------------------------------------------------------
void cWPFUISimpleViewContent::ViewContentUpdateRect(void)noexcept
{
	fWPFDrawing.mSetScale(fLayoutScale);

	fViewContentSize.x=fLayoutScale*fDrawingWidth;
	fViewContentSize.y=fLayoutScale*fDrawingHeight;

	if(fPainter!=nullptr){
		fPainter->PaintRectChanged();

		RenderVisual();
	}
}
//---------------------------------------------------------------------------
void cWPFUISimpleViewContent::ViewContentUpdateState(void)noexcept
{
	if(fPainter==nullptr)
		return;
	UpdatePainterState();

	RenderVisual();
}
//---------------------------------------------------------------------------
void cWPFUISimpleViewContent::ViewContentUpdateWindow(iWindow *Window)noexcept
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
eUIState cWPFUISimpleViewContent::CheckPainterState(void)noexcept
{
	auto NewState=CheckViewContentState();
	switch(NewState){
	case UIState::Inactive:
	case UIState::Active:
		if(fDrawingWidth<=0 || fDrawingHeight<=0){
			NewState=UIState::Background;
		}
	}
	return NewState;
}
//---------------------------------------------------------------------------
void cWPFUISimpleViewContent::UpdatePainterState(void)noexcept
{
	eUIState NewState=CheckPainterState();
	NotifyPainterState(fPainter,NewState,fPainterState);
	fPainterState=NewState;
}
//---------------------------------------------------------------------------
void cWPFUISimpleViewContent::NotifyPainterState(iUISimplePainter *Painter,eUIState NewState,eUIState OldState)noexcept
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
void cWPFUISimpleViewContent::RenderVisual(void)noexcept
{
	if(fPainterState<UIState::Inactive)
		return;

	if(fDrawingWidth<=0 || fDrawingHeight<=0){
		return;
	}

	fWPFDrawing.mRenderVisual(fPainter);

}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cDNetGDIThreadCPPContext::cDNetGDIThreadCPPContext()noexcept
{
	Device=rCreate<cnRTL::cnWinRTL::cDCPaintDevice>();
}
//---------------------------------------------------------------------------
cDNetGDIThreadCPPContext::~cDNetGDIThreadCPPContext()noexcept
{
}
//---------------------------------------------------------------------------
void cDNetGDIThreadCPPContext::NotifyDisplayChanged(void)noexcept
{
	Device->UpdateDisplaySetting();
}
//---------------------------------------------------------------------------
cDNetGDIThreadCPPContext* cDNetGDIThreadCPPContext::mThreadContext(void)noexcept
{
	return static_cast<cDNetGDIThreadCPPContext*>(mDNetGDIThreadContext_ThreadContext());
}
//---------------------------------------------------------------------------
void* cnWin::cDNetGDIThreadCPPContext_New(void)noexcept
{
	return new cDNetGDIThreadCPPContext;
}
//---------------------------------------------------------------------------
void cnWin::cDNetGDIThreadCPPContext_Delete(void *p)noexcept
{
	delete static_cast<cDNetGDIThreadCPPContext*>(p);
}
//---------------------------------------------------------------------------
void cnWin::cDNetGDIThreadCPPContext_NotifyDisplayChanged(void *p)noexcept
{
	static_cast<cDNetGDIThreadCPPContext*>(p)->NotifyDisplayChanged();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWPFDCViewContent::cWPFDCViewContent(iDCPainter *Painter,bool OutputAlpha)noexcept
	: fPainter(Painter)
	, fPainterOutputAlpha(OutputAlpha)
	, fPainterState(psNone)
{
}
//---------------------------------------------------------------------------
cWPFDCViewContent::~cWPFDCViewContent()noexcept
{
}
//---------------------------------------------------------------------------
cGCRef* cWPFDCViewContent::GetDrawingPointer(void)noexcept
{
	return &fDrawing.Handle;
}
//---------------------------------------------------------------------------
eUIState cWPFDCViewContent::GetPaintState(void)noexcept
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
iUIView* cWPFDCViewContent::GetView(void)noexcept
{
	return bcWPFViewContentDrawing::GetView();
}
//---------------------------------------------------------------------------
bool cWPFDCViewContent::SetView(iUIView *View)noexcept
{
	return bcWPFViewContentDrawing::SetView(View);
}
//---------------------------------------------------------------------------
bool cWPFDCViewContent::GetVisible(void)noexcept
{
	return bcWPFViewContentDrawing::GetVisible();
}
//---------------------------------------------------------------------------
bool cWPFDCViewContent::SetVisible(bool Visible)noexcept
{
	return bcWPFViewContentDrawing::SetVisible(Visible);
}
//---------------------------------------------------------------------------
Float32 cWPFDCViewContent::GetZPosition(void)noexcept
{
	return bcWPFViewContentDrawing::GetZPosition();
}
//---------------------------------------------------------------------------
bool cWPFDCViewContent::SetZPosition(Float32 ZPosition)noexcept
{
	return bcWPFViewContentDrawing::SetZPosition(ZPosition);
}
//---------------------------------------------------------------------------
Float32 cWPFDCViewContent::GetContentScale(void)noexcept
{
	return bcWPFViewContentDrawing::GetContentScale();
}
//---------------------------------------------------------------------------
RECT cWPFDCViewContent::GetPaintRect(void)noexcept
{
	RECT rc;
	rc.left=0;
	rc.top=0;
	rc.right=fDrawWidth;
	rc.bottom=fDrawHeight;
	return rc;
}
//---------------------------------------------------------------------------
HDC	cWPFDCViewContent::GetDC(void)noexcept
{
	if(fContentDC!=nullptr){
		fContentDC->RefCount++;
		return fContentDC->MemDC;
	}
	auto CPP=cDNetGDIThreadCPPContext::mThreadContext();
	fContentDC=new cContentDC;
	auto ScreenIC=CPP->Device->GetScreenIC();
	fContentDC->MemDC=::CreateCompatibleDC(ScreenIC->GetHandle());
	fContentDC->Buffer.Setup(fContentDC->MemDC,fDrawWidth,fDrawHeight);
	fContentDC->MemDCBitmap=::SelectObject(fContentDC->MemDC,fContentDC->Buffer.Bitmap);
	CPPPaintStart(fContentDC->Buffer.BitmapData,fContentDC->Buffer.Width,fContentDC->Buffer.Height);
	return fContentDC->MemDC;
}
//---------------------------------------------------------------------------
void cWPFDCViewContent::ReleaseDC(HDC DC)noexcept
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

	fDrawing.mApplyVisual(fBitmapBuffer,fDrawWidth,fDrawHeight);
	

	// delete dc
	::SelectObject(fContentDC->MemDC,fContentDC->MemDCBitmap);
	::DeleteDC(fContentDC->MemDC);
	delete fContentDC;
	fContentDC=nullptr;
}
//---------------------------------------------------------------------------
HDC	cWPFDCViewContent::GetIC(void)noexcept
{
	if(fTempIC==nullptr){
		auto GDIThreadCPP=cDNetGDIThreadCPPContext::mThreadContext();
		fTempIC=GDIThreadCPP->Device->GetScreenIC();
		fICRef=1;
	}
	else{
		fICRef++;
	}
	return fTempIC->GetHandle();
}
//---------------------------------------------------------------------------
void cWPFDCViewContent::ReleaseIC(HDC DC)noexcept
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
void cWPFDCViewContent::InvalidateRect(const RECT*)noexcept
{
	SetNeedPaint();
}
//---------------------------------------------------------------------------
iWindow* cWPFDCViewContent::GetWindow(void)noexcept
{
	return fWindow;
}
//---------------------------------------------------------------------------
cWPFDCViewContent::ePainterState cWPFDCViewContent::CheckPainterState(void)noexcept
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
void cWPFDCViewContent::NotifyPainterState(iDCPainter *Painter,ePainterState NewState,ePainterState OldState)noexcept
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
void cWPFDCViewContent::UpdatePainterState(void)noexcept
{
	ePainterState NewState=CheckPainterState();
	NotifyPainterState(fPainter,NewState,fPainterState);
	fPainterState=NewState;
}
//---------------------------------------------------------------------------
void cWPFDCViewContent::ViewContentUpdateState(void)noexcept
{
	if(fPainter==nullptr)
		return;
	UpdatePainterState();

	Paint();
}
//---------------------------------------------------------------------------
void cWPFDCViewContent::ViewContentUpdateWindow(iWindow *Window)noexcept
{
	if(fPainter==nullptr)
		return;

	fWindow=Window;
	fPainter->PaintWindowChanged();

	UpdatePainterState();

	Paint();
}
//---------------------------------------------------------------------------
void cWPFDCViewContent::ViewContentUpdateRect(void)noexcept
{
	fNeedNotifyRect=true;
	fNeedPaint=true;

	fDrawWidth=static_cast<int>(fDrawingWidth*fLayoutScale);
	fDrawHeight=static_cast<int>(fDrawingHeight*fLayoutScale);

	fDrawing.mUpdateRect(fDrawingWidth,fDrawingHeight);

	ViewContentUpdateState();
}
//---------------------------------------------------------------------------
void cWPFDCViewContent::Paint(void)noexcept
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

	auto GDICPPContext=cDNetGDIThreadCPPContext::mThreadContext();
	// paint
	CPPPaint(GDICPPContext);

	fDrawing.mApplyVisual(fBitmapBuffer,fDrawWidth,fDrawHeight);
}
//---------------------------------------------------------------------------
void cWPFDCViewContent::SetNeedPaint(void)noexcept
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
void cWPFDCViewContent::CPPPaintStart(void *BitmapData,int Width,int Height)noexcept
{
	auto Pixels=static_cast<uInt32*>(BitmapData);
	uIntn PixelCount=Width*Height;

	if(fPainterOutputAlpha){
		cnMemory::ZeroFill(Pixels,4*PixelCount);
	}
	else{
		cnMemory::Fill(Pixels,PixelCount*sizeof(uInt32),static_cast<uInt32>(0x01000000));
	}
}
//---------------------------------------------------------------------------
void cWPFDCViewContent::CPPPaintFinish(void *BitmapData,int Width,int Height)noexcept
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
void cWPFDCViewContent::SetupBitmap(HDC DC)noexcept
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
void cWPFDCViewContent::CPPPaint(cDNetGDIThreadCPPContext *CPP)noexcept
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
cD3D9ExWindowDevice::cD3D9ExWindowDevice(iWindow *Window)noexcept
	: fWindow(Window)
{
	fWindow->InsertMessageHandler(this);
}
//---------------------------------------------------------------------------
cD3D9ExWindowDevice::~cD3D9ExWindowDevice()noexcept
{
	fWindow->RemoveMessageHandler(this);
}
//---------------------------------------------------------------------------
IDirect3DDevice9Ex* cD3D9ExWindowDevice::GetDevice(void)const noexcept
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
void cD3D9ExWindowDevice::WindowAttached(void)noexcept
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
void cD3D9ExWindowDevice::WindowDetached(void)noexcept
{
	fDevice=nullptr;
}
//---------------------------------------------------------------------------
bool cD3D9ExWindowDevice::WindowMessage(LRESULT&,const cWindowMessageParam&)noexcept
{
	return false;
}
//---------------------------------------------------------------------------
void cD3D9ExWindowDevice::WindowMessageProcessed(LRESULT,const cWindowMessageParam&)noexcept
{
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWPFDXGIViewContent::cWPFDXGIViewContent(iDXGIPainter *Painter)noexcept
	: fPainter(Painter)
	, fPainterState(psNone)
{
	fDrawing.FrontBufferAvailableChanged=&fFrontAvailableChangedProcedure;
}
//---------------------------------------------------------------------------
cWPFDXGIViewContent::~cWPFDXGIViewContent()noexcept
{
}
//---------------------------------------------------------------------------
cGCRef* cWPFDXGIViewContent::GetDrawingPointer(void)noexcept
{
	return &fDrawing.Handle;
}
//---------------------------------------------------------------------------
eUIState cWPFDXGIViewContent::GetPaintState(void)noexcept
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
cUIPoint cWPFDXGIViewContent::GetPaintSize(void)noexcept
{
	cUIPoint Size;
	Size.x=fDrawWidth;
	Size.y=fDrawHeight;
	return Size;
}
//---------------------------------------------------------------------------
bool cWPFDXGIViewContent::GetRenderBufferSharedHandle(HANDLE &SharedHandle)noexcept
{
	SharedHandle=fSurfaceSharedHandle;
	return true;
}
//---------------------------------------------------------------------------
void cWPFDXGIViewContent::UpdateRenderBuffer(void)noexcept
{
	fDrawing.mPresent(fDrawWidth,fDrawHeight);
}
//---------------------------------------------------------------------------
iUIView* cWPFDXGIViewContent::GetView(void)noexcept
{
	return bcWPFViewContentDrawing::GetView();
}
//---------------------------------------------------------------------------
bool cWPFDXGIViewContent::SetView(iUIView *View)noexcept
{
	return bcWPFViewContentDrawing::SetView(View);
}
//---------------------------------------------------------------------------
bool cWPFDXGIViewContent::GetVisible(void)noexcept
{
	return bcWPFViewContentDrawing::GetVisible();
}
//---------------------------------------------------------------------------
bool cWPFDXGIViewContent::SetVisible(bool Visible)noexcept
{
	return bcWPFViewContentDrawing::SetVisible(Visible);
}
//---------------------------------------------------------------------------
Float32 cWPFDXGIViewContent::GetZPosition(void)noexcept
{
	return bcWPFViewContentDrawing::GetZPosition();
}
//---------------------------------------------------------------------------
bool cWPFDXGIViewContent::SetZPosition(Float32 ZPosition)noexcept
{
	return bcWPFViewContentDrawing::SetZPosition(ZPosition);
}
//---------------------------------------------------------------------------
Float32 cWPFDXGIViewContent::GetContentScale(void)noexcept
{
	return bcWPFViewContentDrawing::GetContentScale();
}
//---------------------------------------------------------------------------
cWPFDXGIViewContent::ePainterState cWPFDXGIViewContent::CheckPainterState(void)noexcept
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
void cWPFDXGIViewContent::NotifyPainterState(iDXGIPainter *Painter,ePainterState NewState,ePainterState OldState)noexcept
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
void cWPFDXGIViewContent::UpdatePainterState(void)noexcept
{
	ePainterState NewState=CheckPainterState();
	NotifyPainterState(fPainter,NewState,fPainterState);
	fPainterState=NewState;
}
//---------------------------------------------------------------------------
void cWPFDXGIViewContent::ViewContentUpdateState(void)noexcept
{
	if(fPainter==nullptr)
		return;
	UpdatePainterState();
}
//---------------------------------------------------------------------------
void cWPFDXGIViewContent::ViewContentUpdateWindow(iWindow *Window)noexcept
{
	if(fPainter==nullptr)
		return;

	fWindow=Window;
	if(fWindow!=nullptr){
		fD3D9ExDevice.Affix(fWindow);
		
		if(fDrawing.FrontBufferAvailable){
			SetupBackBuffer();
		}
	}
	else{
		fD3D9ExDevice.Clear();
	}
	UpdatePainterState();
}
//---------------------------------------------------------------------------
void cWPFDXGIViewContent::ViewContentUpdateRect(void)noexcept
{
	fDrawWidth=static_cast<int>(fDrawingWidth*fLayoutScale);
	fDrawHeight=static_cast<int>(fDrawingHeight*fLayoutScale);

	fDrawing.mUpdateRect(fDrawingWidth,fDrawingHeight);

	if(fPainter!=nullptr)
		fPainter->PaintSizeChanged();


	if(fDrawing.FrontBufferAvailable){
		ClearBackBuffer();
		SetupBackBuffer();
	}
}
//---------------------------------------------------------------------------
void cWPFDXGIViewContent::cFrontAvailableChangedProcedure::Execute(void)noexcept
{
	auto Host=cnMemory::GetObjectFromMemberPointer(this,&cWPFDXGIViewContent::fFrontAvailableChangedProcedure);
	return Host->FrontAvailableChanged();
}
//---------------------------------------------------------------------------
void cWPFDXGIViewContent::FrontAvailableChanged(void)noexcept
{
	if(fDrawing.FrontBufferAvailable){
		SetupBackBuffer();
	}
	else{
		if(fD3D9ExDevice==nullptr)
			return;
		auto Device=fD3D9ExDevice->GetDevice();
		if(Device==nullptr)
			return;
		
		if(FAILED(Device->TestCooperativeLevel())){
			ClearBackBuffer();
		}
	}
}
//---------------------------------------------------------------------------
void cWPFDXGIViewContent::SetupBackBuffer(void)noexcept
{
	if(fSurface!=nullptr){
		fDrawing.mApplyBackBuffer(fSurface,fDrawingWidth,fDrawingHeight);
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
	fDrawing.mSetBackBuffer(fSurface);
	
	if(fPainter!=nullptr){
		fPainter->RenderBufferChanged();
	}
}
//---------------------------------------------------------------------------
void cWPFDXGIViewContent::ClearBackBuffer(void)noexcept
{
	fSurface=nullptr;
	fSurfaceSharedHandle=nullptr;
	fDrawing.mSetBackBuffer(nullptr);
}
//---------------------------------------------------------------------------
