
#include "WinDNet_WPFUIViewContent.h"


using namespace cnLibrary;
using namespace cnRTL;
using namespace cnWin;

//---------------------------------------------------------------------------
iPtr<iUIFont> cnWin::DNetCreateFont(const uChar16 *Name,uIntn NameLength,eUIFontStyle FontStyle,Float32 FontWeight)
{
	return iCreate<cWPFTypeface>(mcWPFTypeface::CreateTypeface(Name,NameLength,FontStyle,FontWeight));
}
//---------------------------------------------------------------------------
rPtr<iUITextLineLayout> cnWin::DNetCreateTextLineLayout(const uChar16 *Text,uIntn Length,const cUITextStyle &Style)
{
	auto Layout=rCreate<cWPFTextLayout>();
	cUTFStringConverter<4,2> Converter;
	auto Text32=cnRTL::CreateStringConvertEncoding<uChar32>(Converter,Text,Length);
	Layout->Setup(Text32->Pointer,Text32->Length,Style);

	return Layout;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool mcWPFDrawingContext::Fill(cUIPoint DrawPosition,cUIPoint DrawSize,cUIColor Color)
{
	return mFill(DrawPosition,DrawSize,Color);
}
//---------------------------------------------------------------------------
bool mcWPFDrawingContext::Graph(cUIPoint DrawPosition,cUIPoint DrawSize,iUIGraph *Graph)
{
	auto WPFSimpleGraph=iCast<miWPFUISimpleGraph>(Graph);
	if(WPFSimpleGraph!=nullptr){
		WPFSimpleGraph->Draw(Handle,DrawPosition,DrawSize);
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------
bool mcWPFDrawingContext::Image(cUIPoint DrawPosition,cUIPoint DrawSize,iUIBitmap *Bitmap,eImageBlendingOperator BlendingOperator)
{
	return false;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWPFUIBitmap::cWPFUIBitmap(mcWPFUIBitmap &&Bitmap)
	: fBitmap(cnVar::MoveCast(Bitmap))
{
}
//---------------------------------------------------------------------------
cWPFUIBitmap::~cWPFUIBitmap()
{
}
//---------------------------------------------------------------------------
cUIPoint cWPFUIBitmap::GetSize(void)
{
	return fBitmap.ImageSize;
}
//---------------------------------------------------------------------------
void cWPFUIBitmap::Draw(cGCRef &ContextHandle,cUIPoint DrawPosition,cUIPoint DrawSize)
{
	return fBitmap.Draw(ContextHandle,DrawPosition,DrawSize);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWPFBitmapDataSource::cWPFBitmapDataSource(mcWPFBitmapDataSource &&BitmapSource)
	: fBitmapSource(cnVar::MoveCast(BitmapSource))
{
}
//---------------------------------------------------------------------------
cWPFBitmapDataSource::~cWPFBitmapDataSource()
{
}
//---------------------------------------------------------------------------
cBitmapPixelFormat cWPFBitmapDataSource::GetPixelFormat(void)
{
	return BitmapPixelFormatR8G8B8A8;
}
//---------------------------------------------------------------------------
cUIPoint cWPFBitmapDataSource::GetImageSize(void)
{
	return fBitmapSource.ImageSize;
}
//---------------------------------------------------------------------------
uIntn cWPFBitmapDataSource::GetDataPitch(void)
{
	return 0;
}
//---------------------------------------------------------------------------
uIntn cWPFBitmapDataSource::CopyPixelBuffer(uIntn Offset,void *Dest,uIntn DestSize)
{
	return 0;
}
//---------------------------------------------------------------------------
const void* cWPFBitmapDataSource::AcquirePixels(void)
{
	return nullptr;
}
//---------------------------------------------------------------------------
void cWPFBitmapDataSource::ReleasePixels(void)
{
}
//---------------------------------------------------------------------------
bool cWPFBitmapDataSource::IsTopDown(void)
{
	return false;
}
//---------------------------------------------------------------------------
void cWPFBitmapDataSource::Draw(cGCRef &ContextHandle,cUIPoint DrawPosition,cUIPoint DrawSize)
{
	return fBitmapSource.Draw(ContextHandle,DrawPosition,DrawSize);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWPFUISimpleBitmapCanvas::cWPFUISimpleBitmapCanvas(cUIPoint Size)
	: mcWPFUISimpleBitmapCanvas(Size)
{
}
//---------------------------------------------------------------------------
cWPFUISimpleBitmapCanvas::~cWPFUISimpleBitmapCanvas()
{
}
//---------------------------------------------------------------------------
iUISimplePaintContext* cWPFUISimpleBitmapCanvas::StartContext(cUIColor Color)
{
	mcWPFUISimpleBitmapCanvas::StartContext(fContext,Color);
	return &fContext;
}
//---------------------------------------------------------------------------
void cWPFUISimpleBitmapCanvas::DiscardContext(void)
{
	return mcWPFUISimpleBitmapCanvas::DiscardContext(fContext);
}
//---------------------------------------------------------------------------
iPtr<iUIBitmap> cWPFUISimpleBitmapCanvas::FinishBitmap(bool CopyContent)
{
	bool Success;
	auto Drawing=mcWPFUISimpleBitmapCanvas::FinishContext(fContext,Success);
	if(!Success)
		return nullptr;

	return iCreate<cWPFUIBitmap>(mcWPFUIBitmap::FromDrawing(Drawing));
}
//---------------------------------------------------------------------------
iPtr<iBitmapDataSource> cWPFUISimpleBitmapCanvas::FinishBitmapSource(bool CopyContent)
{
	bool Success;
	auto Drawing=mcWPFUISimpleBitmapCanvas::FinishContext(fContext,Success);
	if(!Success)
		return nullptr;

	return iCreate<cWPFBitmapDataSource>(mcWPFBitmapDataSource::FromDrawing(Drawing,fBitmapSize));
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWPFTypeface::cWPFTypeface(mcWPFTypeface &&Typeface)
	: fTypeface(cnVar::MoveCast(Typeface))
{
}
//---------------------------------------------------------------------------
cWPFTypeface::~cWPFTypeface()
{
}
//---------------------------------------------------------------------------
rPtr< iArrayReference<const uChar16> > cWPFTypeface::GetName(void)
{
	return nullptr;
}
//---------------------------------------------------------------------------
eUIFontStyle cWPFTypeface::GetStyle(void)
{
	return fTypeface.FontStyle;
}
//---------------------------------------------------------------------------
Float32 cWPFTypeface::GetWeight(void)
{
	return fTypeface.FontWeight;
}
//---------------------------------------------------------------------------
cGCRef& cWPFTypeface::GetObjecHandle(void)
{
	return fTypeface.Handle;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
uIntn cWPFTextLayout::GetTextLength(void)
{
	return TextDistance.GetCount();
}
//---------------------------------------------------------------------------
Float32 cWPFTextLayout::GetTextHeight(void)
{
	return TextHeight;
}
//---------------------------------------------------------------------------
const Float32* cWPFTextLayout::GetTextDistance(void)
{
	return TextDistance;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWPFUISimpleTextGraph::cWPFUISimpleTextGraph()
{
}
//---------------------------------------------------------------------------
cWPFUISimpleTextGraph::~cWPFUISimpleTextGraph()
{
}
//---------------------------------------------------------------------------
cUIPoint cWPFUISimpleTextGraph::GetSize(void)
{
	return fTextSize;
}
//---------------------------------------------------------------------------
void cWPFUISimpleTextGraph::Draw(cGCRef &ContextHandle,cUIPoint DrawPosition,cUIPoint DrawSize)
{
	return mcWPFUISimpleTextGraph::Draw(ContextHandle,DrawPosition,DrawSize);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWPFUIRichTextGraph::cWPFUIRichTextGraph()
{
}
//---------------------------------------------------------------------------
cWPFUIRichTextGraph::~cWPFUIRichTextGraph()
{
}
//---------------------------------------------------------------------------
cUIPoint cWPFUIRichTextGraph::GetSize(void)
{
	return fTextSize;
}
//---------------------------------------------------------------------------
void cWPFUIRichTextGraph::Draw(cGCRef &ContextHandle,cUIPoint DrawPosition,cUIPoint DrawSize)
{
	return mcWPFUIRichTextGraph::Draw(ContextHandle,DrawPosition,DrawSize);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWPFUISimplePaintDevice::cWPFUISimplePaintDevice()
{
}
//---------------------------------------------------------------------------
cWPFUISimplePaintDevice::~cWPFUISimplePaintDevice()
{
}
//---------------------------------------------------------------------------
rPtr<iUISimpleBitmapCanvas> cWPFUISimplePaintDevice::CreateBitmapCanvas(cUIPoint Size)
{
	return nullptr;
}
//---------------------------------------------------------------------------
iPtr<iUITextGraph> cWPFUISimplePaintDevice::CreateTextGraph(const uChar16 *Text,uIntn Length,const cUITextStyle &Style,const Float32* TextDistance,Float32 TextMinHeight)
{
	cUTFStringConverter<4,2> Converter;
	auto Text32=cnRTL::CreateStringConvertEncoding<uChar32>(Converter,Text,Length);
	auto TextGraph=iCreate<cWPFUISimpleTextGraph>();
	TextGraph->SetupText(Text32->Pointer,Text32->Length,Style,TextDistance,TextMinHeight);
	return TextGraph;
}
//---------------------------------------------------------------------------
iPtr<iUIBitmap> cWPFUISimplePaintDevice::CreateBitmapCopyFromSource(iBitmapDataSource *Source)
{
	return nullptr;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cGCRef& mcWPFUIViewContentDrawingGroup::GetDrawingGroup(void)
{
	return fDrawingGroup;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWPFUISimpleViewContent::cWPFUISimpleViewContent(iUISimplePainter *Painter)
	: fPainter(Painter)
{
}
//---------------------------------------------------------------------------
cWPFUISimpleViewContent::~cWPFUISimpleViewContent()
{
}
//---------------------------------------------------------------------------
iUIView* cWPFUISimpleViewContent::GetView(void)
{
	return bcWPFViewContentDrawing::GetView();
}
//---------------------------------------------------------------------------
bool cWPFUISimpleViewContent::SetView(iUIView *View)
{
	return bcWPFViewContentDrawing::SetView(View);
}
//---------------------------------------------------------------------------
bool cWPFUISimpleViewContent::GetVisible(void)
{
	return bcWPFViewContentDrawing::GetVisible();
}
//---------------------------------------------------------------------------
bool cWPFUISimpleViewContent::SetVisible(bool Visible)
{
	return bcWPFViewContentDrawing::SetVisible(Visible);
}
//---------------------------------------------------------------------------
Float32 cWPFUISimpleViewContent::GetZPosition(void)
{
	return bcWPFViewContentDrawing::GetZPosition();
}
//---------------------------------------------------------------------------
bool cWPFUISimpleViewContent::SetZPosition(Float32 ZPosition)
{
	return bcWPFViewContentDrawing::SetZPosition(ZPosition);
}
//---------------------------------------------------------------------------
rPtr<iUISimplePaintDevice> cWPFUISimpleViewContent::QueryDevice(void)
{
	return rCreate<cWPFUISimplePaintDevice>();
}
//---------------------------------------------------------------------------
eUIState cWPFUISimpleViewContent::GetPaintState(void)
{
	return UIState::Null;
}
//---------------------------------------------------------------------------
cUIPoint cWPFUISimpleViewContent::GetPaintSize(void)
{
	return fViewContentSize;
}
//---------------------------------------------------------------------------
void cWPFUISimpleViewContent::QueryUpdate(void)
{
	RenderVisual();
}
//---------------------------------------------------------------------------
cGCRef* cWPFUISimpleViewContent::GetDrawingPointer(void)
{
	return &fWPFDrawing.GetDrawingGroup();
}
//---------------------------------------------------------------------------
void cWPFUISimpleViewContent::ViewContentUpdateRect(void)
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
void cWPFUISimpleViewContent::ViewContentUpdateState(void)
{
	if(fPainter==nullptr)
		return;
	UpdatePainterState();

	RenderVisual();
}
//---------------------------------------------------------------------------
void cWPFUISimpleViewContent::ViewContentUpdateWindow(iWindow *Window)
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
eUIState cWPFUISimpleViewContent::CheckPainterState(void)
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
void cWPFUISimpleViewContent::UpdatePainterState(void)
{
	eUIState NewState=CheckPainterState();
	NotifyPainterState(fPainter,NewState,fPainterState);
	fPainterState=NewState;
}
//---------------------------------------------------------------------------
void cWPFUISimpleViewContent::NotifyPainterState(iUISimplePainter *Painter,eUIState NewState,eUIState OldState)
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
void cWPFUISimpleViewContent::RenderVisual(void)
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
cDNetGDIThreadCPPContext::cDNetGDIThreadCPPContext()
{
	Device=rCreate<cnRTL::cnWinRTL::cDCPaintDevice>();
}
//---------------------------------------------------------------------------
cDNetGDIThreadCPPContext::~cDNetGDIThreadCPPContext()
{
}
//---------------------------------------------------------------------------
void cDNetGDIThreadCPPContext::NotifyDisplayChanged(void)
{
	Device->UpdateDisplaySetting();
}
//---------------------------------------------------------------------------
cDNetGDIThreadCPPContext* cDNetGDIThreadCPPContext::mThreadContext(void)
{
	return static_cast<cDNetGDIThreadCPPContext*>(mDNetGDIThreadContext_ThreadContext());
}
//---------------------------------------------------------------------------
void* cnWin::cDNetGDIThreadCPPContext_New(void)
{
	return new cDNetGDIThreadCPPContext;
}
//---------------------------------------------------------------------------
void cnWin::cDNetGDIThreadCPPContext_Delete(void *p)
{
	delete static_cast<cDNetGDIThreadCPPContext*>(p);
}
//---------------------------------------------------------------------------
void cnWin::cDNetGDIThreadCPPContext_NotifyDisplayChanged(void *p)
{
	static_cast<cDNetGDIThreadCPPContext*>(p)->NotifyDisplayChanged();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWPFDCViewContent::cWPFDCViewContent(iDCPainter *Painter,bool OutputAlpha)
	: fPainter(Painter)
	, fPainterOutputAlpha(OutputAlpha)
	, fPainterState(psNone)
{
}
//---------------------------------------------------------------------------
cWPFDCViewContent::~cWPFDCViewContent()
{
}
//---------------------------------------------------------------------------
cGCRef* cWPFDCViewContent::GetDrawingPointer(void)
{
	return &fDrawing.Handle;
}
//---------------------------------------------------------------------------
eUIState cWPFDCViewContent::GetPaintState(void)
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
iUIView* cWPFDCViewContent::GetView(void)
{
	return bcWPFViewContentDrawing::GetView();
}
//---------------------------------------------------------------------------
bool cWPFDCViewContent::SetView(iUIView *View)
{
	return bcWPFViewContentDrawing::SetView(View);
}
//---------------------------------------------------------------------------
bool cWPFDCViewContent::GetVisible(void)
{
	return bcWPFViewContentDrawing::GetVisible();
}
//---------------------------------------------------------------------------
bool cWPFDCViewContent::SetVisible(bool Visible)
{
	return bcWPFViewContentDrawing::SetVisible(Visible);
}
//---------------------------------------------------------------------------
Float32 cWPFDCViewContent::GetZPosition(void)
{
	return bcWPFViewContentDrawing::GetZPosition();
}
//---------------------------------------------------------------------------
bool cWPFDCViewContent::SetZPosition(Float32 ZPosition)
{
	return bcWPFViewContentDrawing::SetZPosition(ZPosition);
}
//---------------------------------------------------------------------------
RECT cWPFDCViewContent::GetPaintRect(void)
{
	RECT rc;
	rc.left=0;
	rc.top=0;
	rc.right=fDrawWidth;
	rc.bottom=fDrawHeight;
	return rc;
}
//---------------------------------------------------------------------------
HDC	cWPFDCViewContent::GetDC(void)
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
void cWPFDCViewContent::ReleaseDC(HDC DC)
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
HDC	cWPFDCViewContent::GetIC(void)
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
void cWPFDCViewContent::ReleaseIC(HDC DC)
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
void cWPFDCViewContent::InvalidateRect(const RECT*)
{
	SetNeedPaint();
}
//---------------------------------------------------------------------------
iWindow* cWPFDCViewContent::GetWindow(void)
{
	return fWindow;
}
//---------------------------------------------------------------------------
cWPFDCViewContent::ePainterState cWPFDCViewContent::CheckPainterState(void)
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
void cWPFDCViewContent::NotifyPainterState(iDCPainter *Painter,ePainterState NewState,ePainterState OldState)
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
void cWPFDCViewContent::UpdatePainterState(void)
{
	ePainterState NewState=CheckPainterState();
	NotifyPainterState(fPainter,NewState,fPainterState);
	fPainterState=NewState;
}
//---------------------------------------------------------------------------
void cWPFDCViewContent::ViewContentUpdateState(void)
{
	if(fPainter==nullptr)
		return;
	UpdatePainterState();

	Paint();
}
//---------------------------------------------------------------------------
void cWPFDCViewContent::ViewContentUpdateWindow(iWindow *Window)
{
	if(fPainter==nullptr)
		return;

	fWindow=Window;
	fPainter->PaintWindowChanged();

	UpdatePainterState();

	Paint();
}
//---------------------------------------------------------------------------
void cWPFDCViewContent::ViewContentUpdateRect(void)
{
	fNeedNotifyRect=true;
	fNeedPaint=true;

	fDrawWidth=static_cast<int>(fDrawingWidth*fLayoutScale);
	fDrawHeight=static_cast<int>(fDrawingHeight*fLayoutScale);

	fDrawing.mUpdateRect(fDrawingWidth,fDrawingHeight);

	ViewContentUpdateState();
}
//---------------------------------------------------------------------------
void cWPFDCViewContent::Paint(void)
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
void cWPFDCViewContent::SetNeedPaint(void)
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
void cWPFDCViewContent::CPPPaintStart(void *BitmapData,int Width,int Height)
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
void cWPFDCViewContent::CPPPaintFinish(void *BitmapData,int Width,int Height)
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
void cWPFDCViewContent::SetupBitmap(HDC DC)
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
void cWPFDCViewContent::CPPPaint(cDNetGDIThreadCPPContext *CPP)
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
cD3D9ExWindowDevice::cD3D9ExWindowDevice(iWindow *Window)
	: fWindow(Window)
{
	fWindow->InsertMessageHandler(this);
}
//---------------------------------------------------------------------------
cD3D9ExWindowDevice::~cD3D9ExWindowDevice()
{
	fWindow->RemoveMessageHandler(this);
}
//---------------------------------------------------------------------------
IDirect3DDevice9Ex* cD3D9ExWindowDevice::GetDevice(void)const
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
void cD3D9ExWindowDevice::WindowAttached(void)
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
void cD3D9ExWindowDevice::WindowDetached(void)
{
	fDevice=nullptr;
}
//---------------------------------------------------------------------------
bool cD3D9ExWindowDevice::WindowMessage(LRESULT&,const cWindowMessageParam&)
{
	return false;
}
//---------------------------------------------------------------------------
void cD3D9ExWindowDevice::WindowMessageProcessed(LRESULT,const cWindowMessageParam&)
{
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWPFDXGIViewContent::cWPFDXGIViewContent(iDXGIPainter *Painter)
	: fPainter(Painter)
	, fPainterState(psNone)
{
	fDrawing.FrontBufferAvailableChanged=&fFrontAvailableChangedProcedure;
}
//---------------------------------------------------------------------------
cWPFDXGIViewContent::~cWPFDXGIViewContent()
{
}
//---------------------------------------------------------------------------
cGCRef* cWPFDXGIViewContent::GetDrawingPointer(void)
{
	return &fDrawing.Handle;
}
//---------------------------------------------------------------------------
eUIState cWPFDXGIViewContent::GetPaintState(void)
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
cUIPoint cWPFDXGIViewContent::GetPaintSize(void)
{
	cUIPoint Size;
	Size.x=fDrawWidth;
	Size.y=fDrawHeight;
	return Size;
}
//---------------------------------------------------------------------------
bool cWPFDXGIViewContent::GetRenderBufferSharedHandle(HANDLE &SharedHandle)
{
	SharedHandle=fSurfaceSharedHandle;
	return true;
}
//---------------------------------------------------------------------------
void cWPFDXGIViewContent::UpdateRenderBuffer(void)
{
	fDrawing.mPresent(fDrawWidth,fDrawHeight);
}
//---------------------------------------------------------------------------
iUIView* cWPFDXGIViewContent::GetView(void)
{
	return bcWPFViewContentDrawing::GetView();
}
//---------------------------------------------------------------------------
bool cWPFDXGIViewContent::SetView(iUIView *View)
{
	return bcWPFViewContentDrawing::SetView(View);
}
//---------------------------------------------------------------------------
bool cWPFDXGIViewContent::GetVisible(void)
{
	return bcWPFViewContentDrawing::GetVisible();
}
//---------------------------------------------------------------------------
bool cWPFDXGIViewContent::SetVisible(bool Visible)
{
	return bcWPFViewContentDrawing::SetVisible(Visible);
}
//---------------------------------------------------------------------------
Float32 cWPFDXGIViewContent::GetZPosition(void)
{
	return bcWPFViewContentDrawing::GetZPosition();
}
//---------------------------------------------------------------------------
bool cWPFDXGIViewContent::SetZPosition(Float32 ZPosition)
{
	return bcWPFViewContentDrawing::SetZPosition(ZPosition);
}
//---------------------------------------------------------------------------
cWPFDXGIViewContent::ePainterState cWPFDXGIViewContent::CheckPainterState(void)
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
void cWPFDXGIViewContent::NotifyPainterState(iDXGIPainter *Painter,ePainterState NewState,ePainterState OldState)
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
void cWPFDXGIViewContent::UpdatePainterState(void)
{
	ePainterState NewState=CheckPainterState();
	NotifyPainterState(fPainter,NewState,fPainterState);
	fPainterState=NewState;
}
//---------------------------------------------------------------------------
void cWPFDXGIViewContent::ViewContentUpdateState(void)
{
	if(fPainter==nullptr)
		return;
	UpdatePainterState();
}
//---------------------------------------------------------------------------
void cWPFDXGIViewContent::ViewContentUpdateWindow(iWindow *Window)
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
void cWPFDXGIViewContent::ViewContentUpdateRect(void)
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
void cWPFDXGIViewContent::cFrontAvailableChangedProcedure::Execute(void)
{
	auto Host=cnMemory::GetObjectFromMemberPointer(this,&cWPFDXGIViewContent::fFrontAvailableChangedProcedure);
	return Host->FrontAvailableChanged();
}
//---------------------------------------------------------------------------
void cWPFDXGIViewContent::FrontAvailableChanged(void)
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
void cWPFDXGIViewContent::SetupBackBuffer(void)
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
void cWPFDXGIViewContent::ClearBackBuffer(void)
{
	fSurface=nullptr;
	fSurfaceSharedHandle=nullptr;
	fDrawing.mSetBackBuffer(nullptr);
}
//---------------------------------------------------------------------------
