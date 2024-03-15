#include "Mac_QuartzCore.h"

using namespace cnLibrary;
using namespace cnRTL;
using namespace cnMac;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cCFCGImage::cCFCGImage(CGImageRef Image)
{
	fImage=Image;
	CGImageRetain(fImage);
}
//---------------------------------------------------------------------------
cCFCGImage::~cCFCGImage()
{
	CGImageRelease(fImage);
}
//---------------------------------------------------------------------------
void* cCFCGImage::CastInterface(iTypeID InterfaceID)
{
	return cnVar::FindInterface<iCFCGImage>(this,InterfaceID);
}
//---------------------------------------------------------------------------
bool cCFCGImage::DrawInContext(CGContextRef Context,const CGRect &Rect)
{
	if(fImage==nullptr)
		return false;
	CGContextDrawImage(Context,Rect,fImage);
	return true;
}
//---------------------------------------------------------------------------
CGImageRef cCFCGImage::GetCGImage(void)
{
	return fImage;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cBitmapSourceFromCGImage::cBitmapSourceFromCGImage(CGImageRef Image)
	: cCFCGImage(Image)
{
}
//---------------------------------------------------------------------------
cBitmapSourceFromCGImage::~cBitmapSourceFromCGImage()
{
}
//---------------------------------------------------------------------------
static const eBitmapChannelColor CGBitmapColorFormatMap[5][2]={
	// 32Little							32Big							16Little	16Big
	{	BitmapChannelColor::RGB,		BitmapChannelColor::BGR,		},	// kCGImageAlphaNone
	{	BitmapChannelColor::RGBA,		BitmapChannelColor::BGRA,		},	// kCGImageAlphaLast, kCGImageAlphaPremultipliedLast
	{	BitmapChannelColor::Unknown,	BitmapChannelColor::Unknown,	},		// kCGImageAlphaFirst, kCGImageAlphaPremultipliedFirst
	{	BitmapChannelColor::RGB,		BitmapChannelColor::BGR,	},	// kCGImageAlphaNoneSkipLast
	{	BitmapChannelColor::Unknown,	BitmapChannelColor::Unknown,	},		// kCGImageAlphaNoneSkipFirst
};
//---------------------------------------------------------------------------
cBitmapPixelFormat cBitmapSourceFromCGImage::GetPixelFormat(void)
{
	cBitmapPixelFormat PixelFormat;
	PixelFormat.AlphaPremultiplied=true;
	PixelFormat.ChannelCount=4;
	PixelFormat.ChannelSize=1;

	auto BitmapInfo=CGImageGetBitmapInfo(fImage);
	auto AlphaInfo=BitmapInfo&kCGBitmapAlphaInfoMask;
	auto ByteOrder=BitmapInfo&kCGBitmapByteOrderMask;
	if(BitmapInfo&kCGBitmapFloatComponents){
		PixelFormat.ChannelType=BitmapChannelType::Float;
	}
	else{
		PixelFormat.ChannelType=BitmapChannelType::UNorm;
	}

	ufInt8 ByteFormatIndex;


	switch(ByteOrder){
	default:
	case kCGBitmapByteOrder16Little:
	case kCGBitmapByteOrder16Big:
		PixelFormat.ChannelColor=BitmapChannelColor::Unknown;
		return PixelFormat;
	case kCGBitmapByteOrderDefault:
		ByteFormatIndex=1;
		break;
	case kCGBitmapByteOrder32Big:
		ByteFormatIndex=1;
		break;
	case kCGBitmapByteOrder32Little:
		ByteFormatIndex=0;
		break;
	}

	switch(AlphaInfo){
	case kCGImageAlphaNone:					// RGB
		PixelFormat.ChannelColor=CGBitmapColorFormatMap[0][ByteFormatIndex];
		PixelFormat.ChannelCount=3;
		break;
	case kCGImageAlphaPremultipliedLast:	// RGBPA
		PixelFormat.ChannelColor=CGBitmapColorFormatMap[1][ByteFormatIndex];
		break;
    case kCGImageAlphaPremultipliedFirst:	// PARGB
		PixelFormat.ChannelColor=CGBitmapColorFormatMap[2][ByteFormatIndex];
		break;
    case kCGImageAlphaLast:					// RGBA
		PixelFormat.ChannelColor=CGBitmapColorFormatMap[1][ByteFormatIndex];
		break;
    case kCGImageAlphaFirst:				// ARGB
		PixelFormat.ChannelColor=CGBitmapColorFormatMap[2][ByteFormatIndex];
		break;
    case kCGImageAlphaNoneSkipLast:			// RGBX
		PixelFormat.ChannelColor=CGBitmapColorFormatMap[3][ByteFormatIndex];
		break;
    case kCGImageAlphaNoneSkipFirst:		// XRGB
		PixelFormat.ChannelColor=CGBitmapColorFormatMap[4][ByteFormatIndex];
		break;
	default:
    case kCGImageAlphaOnly:					// A
		PixelFormat.ChannelColor=BitmapChannelColor::Alpha;
		PixelFormat.ChannelCount=1;
		PixelFormat.AlphaPremultiplied=false;
		break;
	}
	return PixelFormat;
}
//---------------------------------------------------------------------------
cUIPoint cBitmapSourceFromCGImage::GetImageSize(void)
{
	cUIPoint p;
	p.x=static_cast<sfInt32>(CGImageGetWidth(fImage));
	p.y=static_cast<sfInt32>(CGImageGetHeight(fImage));
	return p;
}
//---------------------------------------------------------------------------
uIntn cBitmapSourceFromCGImage::GetDataPitch(void)
{
	return 0;
}
//---------------------------------------------------------------------------
void cBitmapSourceFromCGImage::CacheData(void)
{
	auto Width=CGImageGetWidth(fImage);
	auto Height=CGImageGetHeight(fImage);

	CGColorSpaceRef ColorSpace = CGColorSpaceCreateDeviceRGB();
	size_t ImageBufferSize=Width*Height*4;
	fCacheData.SetLength(ImageBufferSize);
	cnMemory::ZeroFill(fCacheData.Pointer,fCacheData.Length);

	// convert to bitmap
	CGContextRef Context = CGBitmapContextCreate(fCacheData.Pointer,
		Width, Height, 8, Width*4,
		ColorSpace, kCGImageAlphaPremultipliedLast|kCGBitmapByteOrder32Little);



	CGRect Rect;
	Rect.origin={0,0};
	Rect.size.width=Width;
	Rect.size.height=Height;
	CGContextDrawImage(Context,Rect,fImage);

	CGContextRelease(Context);

	CGColorSpaceRelease(ColorSpace);
	
}
//---------------------------------------------------------------------------
uIntn cBitmapSourceFromCGImage::CopyPixelBuffer(uIntn Offset,void *Dest,uIntn DestSize)
{
	CacheData();
	if(Offset>=fCacheData.Length)
		return 0;

	uIntn CopySize=DestSize;
	if(Offset+CopySize>fCacheData.Length){
		CopySize=fCacheData.Length-Offset;
	}
	cnMemory::Copy(Dest,cnMemory::PointerAddByteOffset(fCacheData.Pointer,Offset),CopySize);
	return CopySize;
}
//---------------------------------------------------------------------------
const void* cBitmapSourceFromCGImage::AcquirePixels(void)
{
	CacheData();

	return fCacheData;
}
//---------------------------------------------------------------------------
void cBitmapSourceFromCGImage::ReleasePixels(void)
{
}
//---------------------------------------------------------------------------
bool cBitmapSourceFromCGImage::IsTopDown(void)
{
	return false;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CGImageRef cnMac::LoadImageFromFile(iFileName *FileName)
{
	auto MacFileName=iCast<iPOSIXFileName>(FileName);
	if(MacFileName==nullptr)
		return nullptr;
	auto DataProvider=CGDataProviderCreateWithFilename(MacFileName->GetPOSIXFileName());
	if(DataProvider==nullptr)
		return nullptr;
	// try png
	auto Image=CGImageCreateWithPNGDataProvider(DataProvider,nullptr,false,kCGRenderingIntentDefault);
	if(Image==nil){
		// try jpeg
		Image=CGImageCreateWithJPEGDataProvider(DataProvider,nullptr,false,kCGRenderingIntentDefault);
	}
	CFRelease(DataProvider);
	return Image;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void cnMac::ColorComponentFromA8R8G8B8(CGFloat Components[4],uInt32 Color)
{
	Components[0]=(Color&0xFF)/255.;
	Components[1]=((Color>>8)&0xFF)/255.;
	Components[2]=((Color>>16)&0xFF)/255.;
	Components[3]=((Color>>24)&0xFF)/255.;
}
//---------------------------------------------------------------------------
static void CGDataAtHeap_ReleaseData(void *info,const void *p,size_t size)
{
	auto Heap=static_cast<iMemoryHeap*>(info);
	Heap->Free(const_cast<void*>(p),size,0);
}
//---------------------------------------------------------------------------
CGDataProviderRef cnMac::CreateCGDataAtHeap(uIntn Size,void* &DataPointer,iMemoryHeap *Heap)
{
	auto Mem=Heap->Alloc(Size,0);
	return CGDataProviderCreateWithData(Heap,Mem,Size,CGDataAtHeap_ReleaseData);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CGDataProviderDirectCallbacks CGDataFromBitmapDataSource::GetCallbacks(void)
{
	CGDataProviderDirectCallbacks Callbacks;
	Callbacks.version=0;
	Callbacks.getBytePointer=CGDataFromBitmapDataSource::GetBytePointer;
	Callbacks.getBytesAtPosition=CGDataFromBitmapDataSource::GetBytesAtPosition;
	Callbacks.releaseBytePointer=CGDataFromBitmapDataSource::ReleaseBytePointer;
	Callbacks.releaseInfo=CGDataFromBitmapDataSource::ReleaseInfo;
	return Callbacks;
}
//---------------------------------------------------------------------------
const void* CGDataFromBitmapDataSource::GetBytePointer(void *info)
{
	auto Source=static_cast<iBitmapDataSource*>(info);
	return Source->AcquirePixels();
}
//---------------------------------------------------------------------------
void CGDataFromBitmapDataSource::ReleaseBytePointer(void *info,const void *pointer)
{
	auto Source=static_cast<iBitmapDataSource*>(info);
	Source->ReleasePixels();
}
//---------------------------------------------------------------------------
size_t CGDataFromBitmapDataSource::GetBytesAtPosition(void *info, void *buffer,off_t pos,size_t cnt)
{
	auto Source=static_cast<iBitmapDataSource*>(info);
	return Source->CopyPixelBuffer(static_cast<uIntn>(pos),buffer,cnt);
}
//---------------------------------------------------------------------------
void CGDataFromBitmapDataSource::ReleaseInfo(void *info)
{
	auto Source=static_cast<iBitmapDataSource*>(info);
	iDecReference(Source);
}
//---------------------------------------------------------------------------
CGDataProviderRef cnMac::CreateCGDataFromBitmapDataSource(iBitmapDataSource *DataSource)
{
	auto ImageSize=DataSource->GetImageSize();
	auto Pitch=DataSource->GetDataPitch();
	auto BitmapDataSize=ImageSize.y*Pitch;
	iIncReference(DataSource);
	auto Callbacks=CGDataFromBitmapDataSource::GetCallbacks();
	return CGDataProviderCreateDirect(DataSource,BitmapDataSize,&Callbacks);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cCGSimplePaintContext::cCGSimplePaintContext()
{
}
//---------------------------------------------------------------------------
cCGSimplePaintContext::~cCGSimplePaintContext()
{
}
//---------------------------------------------------------------------------
void cCGSimplePaintContext::Clear(CGSize ImageSize,uInt32 Color)
{
	CGRect rc;
	rc.origin.x=0;
	rc.origin.y=0;
	rc.size=ImageSize;
	if((Color&0xFF000000)==0){
		CGContextClearRect(Context,rc);
	}
	else{
		CGFloat RGBA[4];
		ColorComponentFromA8R8G8B8(RGBA,Color);
		CGContextSetRGBFillColor(Context,RGBA[0],RGBA[1],RGBA[2],RGBA[3]);
		CGContextFillRect(Context,rc);
	}
}
//---------------------------------------------------------------------------
bool cCGSimplePaintContext::Fill(cUIPoint DrawPosition,cUIPoint DrawSize,uInt32 Color)
{
	CGRect rc;
	rc.origin.x=DrawPosition.x;
	rc.origin.y=DrawPosition.y;
	rc.size.width=DrawSize.x;
	rc.size.height=DrawSize.y;
	float r=Color&0xFF;
	float g=(Color>>8)&0xFF;
	float b=(Color>>16)&0xFF;
	float a=(Color>>24)&0xFF;
	CGContextSetRGBFillColor(Context,r/255.,g/255.,b/255.,a/255.);
	CGContextFillRect(Context,rc);
	return true;
}
//---------------------------------------------------------------------------
bool cCGSimplePaintContext::Graph(cUIPoint DrawPosition,cUIPoint DrawSize,iUIGraph *Graph)
{
	CGRect rc;
	rc.origin.x=DrawPosition.x;
	rc.origin.y=DrawPosition.y;
	rc.size.width=DrawSize.x;
	rc.size.height=DrawSize.y;

	auto ContextDrawable=iCast<iCGContextDrawable>(Graph);
	if(ContextDrawable!=nullptr){
		return ContextDrawable->DrawInContext(Context,rc);
	}
	auto cgImage=iCast<iCFCGImage>(Graph);
	if(cgImage!=nullptr){
		CGContextDrawImage(Context,rc,cgImage->GetCGImage());
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------
CGBlendMode cCGSimplePaintContext::BlendModeFromBlendingOperator(eImageBlendingOperator BlendingOperator)
{
	CGBlendMode BlendMode;
	switch(BlendingOperator){
	case ImageBlendingOperator::Copy:
		BlendMode=kCGBlendModeCopy;
		break;
		/*
	case ImageBlendingOperator::Or:
		BlendMode=kCGBlendModeCopy;
		break;
	case ImageBlendingOperator::And:
		BlendMode=kCGBlendModeCopy;
		break;
	case ImageBlendingOperator::Xor:
		BlendMode=kCGBlendModeXOR;
		break;
*/
	case ImageBlendingOperator::Add:
		BlendMode=kCGBlendModePlusLighter;
		break;
	case ImageBlendingOperator::Mul:
		BlendMode=kCGBlendModeMultiply;
		break;
	default:
	case ImageBlendingOperator::Alpha:
		BlendMode=kCGBlendModeNormal;
		break;
	}
	return BlendMode;
}
//---------------------------------------------------------------------------
bool cCGSimplePaintContext::Image(cUIPoint DrawPosition,cUIPoint DrawSize,iUIBitmap *Bitmap,eImageBlendingOperator BlendingOperator)
{
	CGRect rc;
	rc.origin.x=DrawPosition.x;
	rc.origin.y=DrawPosition.y;
	rc.size.width=DrawSize.x;
	rc.size.height=DrawSize.y;

	if(BlendingOperator==ImageBlendingOperator::Noop){
		// draw nothing
		return true;
	}

	auto cgImage=iCast<iCFCGImage>(Bitmap);
	if(cgImage!=nullptr){
		auto BlendMode=BlendModeFromBlendingOperator(BlendingOperator);
		CGContextSetBlendMode(Context,BlendMode);
		CGContextDrawImage(Context,rc,cgImage->GetCGImage());
		CGContextSetBlendMode(Context,kCGBlendModeNormal);
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cCTTextGraph::cCTTextGraph()
{
}
//---------------------------------------------------------------------------
cCTTextGraph::~cCTTextGraph()
{
	Clear();
}
//---------------------------------------------------------------------------
const void* cCTTextGraph::CastInterface(iTypeID InterfaceID)const
{
	return ImpCastInterface<iCGContextDrawable>(this,InterfaceID);
}
//---------------------------------------------------------------------------
void cCTTextGraph::Clear(void)
{
	if(fLine!=nullptr){
		CFRelease(fLine);
		fLine=nullptr;
	}
}
//---------------------------------------------------------------------------
void cCTTextGraph::SetText(const uChar16 *Text,uIntn Length,const cUITextStyle &Style,const cUITextLineLayout *Layout)
{
	CTFontRef Font;
	if(Style.Font==nullptr){
		Font=CTFontCreateUIFontForLanguage(kCTFontUIFontSystem,Style.FontSize,nullptr);
	}
	else{
		//Font=CTFontCreateWithName(FontName,Style.FontSize,nullptr);
		return;
	}
	auto String=CFStringCreateWithCharacters(nil,reinterpret_cast<const UniChar *>(Text),Length);

	auto ColorSpace=CGColorSpaceCreateDeviceRGB();
	CGFloat RGBColor[4];
	ColorComponentFromA8R8G8B8(RGBColor,Style.TextColor);
	auto TextColor=CGColorCreate(ColorSpace,RGBColor);
	CFStringRef Keys[]={
		kCTForegroundColorAttributeName,
		kCTFontAttributeName,
	};
	const void *Values[]={
		TextColor,
		Font,
	};
	static_assert(cnMemory::ArrayLength(Keys)==cnMemory::ArrayLength(Values),"Mismatch Dictionary");
	auto OptionDict=CFDictionaryCreate(nil, (const void**)Keys, Values, cnMemory::ArrayLength(Keys), nil, &kCFTypeDictionaryValueCallBacks);

	CFAttributedStringRef attrString =
		CFAttributedStringCreate(kCFAllocatorDefault, String, OptionDict);

	fLine = CTLineCreateWithAttributedString(attrString);

	fLineWidth=CTLineGetTypographicBounds(fLine,&fLineHeight,&fLineBase,nullptr);

	CFRelease(Font);
	CGColorRelease(TextColor);
	CGColorSpaceRelease(ColorSpace);
	CFRelease(OptionDict);
	CFRelease(attrString);
}
//---------------------------------------------------------------------------
cUIPoint cCTTextGraph::GetSize(void)
{
	cUIPoint s;
	s.x=fLineWidth;
	s.y=fLineHeight;
	return s;
}
//---------------------------------------------------------------------------
uIntn cCTTextGraph::GetTextLength(void)
{
	auto Range=CTLineGetStringRange(fLine);
	if(Range.location==NSNotFound){
		return 0;
	}
	return Range.length;
}
//---------------------------------------------------------------------------
uIntn cCTTextGraph::GetTextPos(uIntn Length,uInt32 *TextPos)
{
	return 0;
}
//---------------------------------------------------------------------------
void cCTTextGraph::Draw(CGContextRef Context,float x,float y)const
{
	CGContextSetTextPosition(Context,x,y+fLineBase);
	CTLineDraw(fLine, Context);
}
//---------------------------------------------------------------------------
bool cCTTextGraph::DrawInContext(CGContextRef Context,const CGRect &Rect)
{
	if(fLine==nullptr)
		return false;
	Draw(Context,Rect.origin.x,Rect.origin.y);
	return true;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cCGImageBitmap::cCGImageBitmap(CGImageRef Image)
	: cCFCGImage(Image)
{

}
//---------------------------------------------------------------------------
cCGImageBitmap::~cCGImageBitmap()
{
}
//---------------------------------------------------------------------------
cUIPoint cCGImageBitmap::GetSize(void)
{
	auto BitmapWidth=static_cast<sfInt32>(CGImageGetWidth(fImage));
	auto BitmapHeight=static_cast<sfInt32>(CGImageGetHeight(fImage));
	return {BitmapWidth,BitmapHeight};
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cCGLayerBitmap::cCGLayerBitmap(CGLayerRef Layer)
	: fLayer(Layer)
{
	CGLayerRetain(fLayer);
}
//---------------------------------------------------------------------------
cCGLayerBitmap::~cCGLayerBitmap()
{
	CGLayerRelease(fLayer);
}
//---------------------------------------------------------------------------
const void* cCGLayerBitmap::CastInterface(iTypeID InterfaceID)const
{
	return ImpCastInterface<iCGContextDrawable>(this,InterfaceID);
}
//---------------------------------------------------------------------------
bool cCGLayerBitmap::DrawInContext(CGContextRef Context,const CGRect &Rect)
{
	CGContextDrawLayerInRect(Context,Rect,fLayer);
	return true;
}
//---------------------------------------------------------------------------
cUIPoint cCGLayerBitmap::GetSize(void)
{
	auto Size=CGLayerGetSize(fLayer);
	auto BitmapWidth=static_cast<sfInt32>(Size.width);
	auto BitmapHeight=static_cast<sfInt32>(Size.height);
	return {BitmapWidth,BitmapHeight};
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cCGSimpleBitmapCanvas::cCGSimpleBitmapCanvas(CGLayerRef Layer)
	: fDrawLayer(Layer)
{
	fImageSize=CGLayerGetSize(fDrawLayer);
	fPaintContext.Context=CGLayerGetContext(fDrawLayer);
}
//---------------------------------------------------------------------------
cCGSimpleBitmapCanvas::~cCGSimpleBitmapCanvas()
{
	CGLayerRelease(fDrawLayer);
}
//---------------------------------------------------------------------------
iUISimplePaintContext* cCGSimpleBitmapCanvas::GetContext(void)
{
	return &fPaintContext;
}
//---------------------------------------------------------------------------
void cCGSimpleBitmapCanvas::Clear(uInt32 Color)
{
	fPaintContext.Clear(fImageSize,Color);
}
//---------------------------------------------------------------------------
iPtr<iUIBitmap> cCGSimpleBitmapCanvas::FinishBitmap(bool CopyContent)
{
	if(CopyContent){
		auto Image=CGBitmapContextCreateImage(fPaintContext.Context);
		auto Bitmap=iCreate<cCGImageBitmap>(Image);
		CGImageRelease(Image);
		return Bitmap;
	}
	// swap out layer
	auto Bitmap=iCreate<cCGLayerBitmap>(fDrawLayer);
	// create new layer
	CGLayerRelease(fDrawLayer);

	fDrawLayer=CGLayerCreateWithContext(nullptr,fImageSize,nullptr);
	fPaintContext.Context=CGLayerGetContext(fDrawLayer);
	return Bitmap;
}
//---------------------------------------------------------------------------
iPtr<iBitmapDataSource> cCGSimpleBitmapCanvas::FinishBitmapSource(bool CopyContent)
{	UnusedParameter(CopyContent);

	auto SourceBuffer=iCreate<cnRTL::cBitmapBuffer>();
	cBitmapPixelFormat PixelFormat;
	PixelFormat.ChannelColor=BitmapChannelColor::RGBA;
	PixelFormat.ChannelType=BitmapChannelType::UNorm;
	PixelFormat.ChannelSize=1;
	PixelFormat.ChannelCount=4;
	PixelFormat.AlphaPremultiplied=true;
	cUIPoint ImageSize;
	ImageSize.x=fImageSize.width;
	ImageSize.y=fImageSize.height;
	SourceBuffer->SetParameter(PixelFormat,ImageSize.x,ImageSize.y,false);
	auto ColorSpaceRGB=CGColorSpaceCreateDeviceRGB();
	auto DrawContext=CGBitmapContextCreate(SourceBuffer->GetDataBuffer(),ImageSize.x,ImageSize.y,8,4*ImageSize.x,
		ColorSpaceRGB, kCGImageAlphaPremultipliedLast|kCGBitmapByteOrder32Little);

	CGContextDrawLayerAtPoint(DrawContext,{0,0},fDrawLayer);

	CGColorSpaceRelease(ColorSpaceRGB);
	CGContextRelease(DrawContext);

	return SourceBuffer;
}
#if	0
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cCGSimpleBitmapCanvas::cCGSimpleBitmapCanvas(cUIPoint ImageSize)
{
	fImageSize=ImageSize;
	auto ColorSpaceRGB=CGColorSpaceCreateDeviceRGB();
	fPaintContext.Context=CGBitmapContextCreate(nullptr,fImageSize.x,fImageSize.y,8,4*fImageSize.x,
		ColorSpaceRGB, kCGImageAlphaPremultipliedLast|kCGBitmapByteOrder32Little);
	CGColorSpaceRelease(ColorSpaceRGB);
}
//---------------------------------------------------------------------------
cCGSimpleBitmapCanvas::~cCGSimpleBitmapCanvas()
{
	CGContextRelease(fPaintContext.Context);
}
//---------------------------------------------------------------------------
iUISimplePaintContext* cCGSimpleBitmapCanvas::GetContext(void)
{
	return &fPaintContext;
}
//---------------------------------------------------------------------------
void cCGSimpleBitmapCanvas::Clear(uInt32 Color)
{
	fPaintContext.Clear(fImageSize,Color);
}
//---------------------------------------------------------------------------
iPtr<iUIBitmap> cCGSimpleBitmapCanvas::FinishBitmap(void)
{
	auto Image=CGBitmapContextCreateImage(fPaintContext.Context);
	auto Bitmap=iCreate<cCGImageBitmap>(Image);
	CGImageRelease(Image);
	return Bitmap;
}
//---------------------------------------------------------------------------
iPtr<iBitmapDataSource> cCGSimpleBitmapCanvas::FinishBitmapSource(void)
{
	auto Image=CGBitmapContextCreateImage(fPaintContext.Context);
	auto BitmapSource=iCreate<cBitmapSourceFromCGImage>(Image);
	CGImageRelease(Image);
	return BitmapSource;
}
#endif
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
rPtr<iUISimpleBitmapCanvas> cCGSimplePaintDevice::CreateBitmapCanvas(cUIPoint Size)
{
	auto Layer=CGLayerCreateWithContext(nullptr,CGSizeMake(Size.x,Size.y),nullptr);
	if(Layer==nil)
		return nullptr;
	return rCreate<cCGSimpleBitmapCanvas>(Layer);
}
//---------------------------------------------------------------------------
iPtr<iUITextGraph> cCGSimplePaintDevice::CreateTextGraph(const uChar16 *Text,uIntn Length,const cUITextStyle &Style,const cUITextLineLayout *Layout)
{
	auto TextGraph=iCreate<cCTTextGraph>();
	TextGraph->SetText(Text,Length,Style,Layout);
	return TextGraph;
}
//---------------------------------------------------------------------------
iPtr<iUIBitmap> cCGSimplePaintDevice::CreateBitmapCopyFromSource(iBitmapDataSource *Source)
{
	auto BitmapData=CreateCGDataFromBitmapDataSource(Source);
	auto ImageSize=Source->GetImageSize();
	auto colorSpace = CGColorSpaceCreateDeviceRGB();
	auto BitmapImage=CGImageCreate(ImageSize.x,ImageSize.y,8,32,4*ImageSize.x,
		colorSpace, kCGImageAlphaLast,
		BitmapData,nullptr,false,kCGRenderingIntentDefault);

	CGColorSpaceRelease(colorSpace);

	auto Bitmap=iCreate<cCGImageBitmap>(BitmapImage);
	CGImageRelease(BitmapImage);
	return Bitmap;
}
//---------------------------------------------------------------------------
#pragma mark - Simple ViewContent
//---------------------------------------------------------------------------
@interface cnLib_MacSimpleViewContent ()<CALayerDelegate>
@end

@implementation cnLib_MacSimpleViewContent
{
@private
	cnRTL::ocStaticIVar<cMacSimpleViewContent> cnLib_IVar;
}
static cMacSimpleViewContent& pViewContent(cnLib_MacSimpleViewContent *self)
{
	return reinterpret_cast<cMacSimpleViewContent&>(self->cnLib_IVar);
}
cMacSimpleViewContent* cnMac::cnLib_MacSimpleViewContent_Interface(cnLib_MacSimpleViewContent *Object)
{
	return &pViewContent(Object);
}

-(instancetype)init
{
	self=[super init];
	if(self!=nil)
		cnVar::ManualConstruct(pViewContent(self),self);
	return self;
}

-(void)dealloc
{
	cnVar::ManualDestruct(pViewContent(self));
	[super dealloc];
}

-(void)displayLayer:(CALayer *)layer
{
	auto &pt=pViewContent(self);
	pt.OnLayerDisplay();
}

//-(void)drawLayer:(CALayer *)layer inContext:(CGContextRef)ctx

//-(void)layoutSublayersOfLayer:(CALayer *)layer

-(id<CAAction>)actionForLayer:(CALayer *)layer forKey:(NSString *)event
{
	return [NSNull null];
}

@end
//---------------------------------------------------------------------------
cMacSimpleViewContent::cMacSimpleViewContent(cnLib_MacSimpleViewContent *OCHost)
{
	fOCHost=OCHost;
	fLayer=[[CALayer alloc]init];
	fLayer.delegate=fOCHost;
	fEmptySize=true;
}
//---------------------------------------------------------------------------
cMacSimpleViewContent::~cMacSimpleViewContent()
{
	[fLayer removeFromSuperlayer];
	[fLayer release];
}
//---------------------------------------------------------------------------
void cMacSimpleViewContent::IncReference(void)
{
	[fOCHost retain];
}
//---------------------------------------------------------------------------
void cMacSimpleViewContent::DecReference(void)
{
	[fOCHost release];
}
//---------------------------------------------------------------------------
const void* cMacSimpleViewContent::CastInterface(iTypeID TypeID)const
{
	return ImpCastInterface<iReference,iOCObject>(this,TypeID);
}
//---------------------------------------------------------------------------
id cMacSimpleViewContent::GetOCObject(void)
{
	return fLayer;
}
//---------------------------------------------------------------------------
CALayer* cMacSimpleViewContent::GetLayer(void)
{
	return fLayer;
}
//---------------------------------------------------------------------------
void cMacSimpleViewContent::OnLayerDisplay(void)
{
	if(fEmptySize)
		return;
	if(fPainter!=nullptr){
		cCGSimplePaintContext PaintContext;
		auto Bounds=fLayer.bounds;
		auto Scale=fLayer.contentsScale;
		cUIPoint ImageSize;
		ImageSize.x=Bounds.size.width*Scale;
		ImageSize.y=Bounds.size.height*Scale;
		auto ColorSpaceRGB=CGColorSpaceCreateDeviceRGB();
		PaintContext.Context=CGBitmapContextCreate(nullptr,ImageSize.x,ImageSize.y,8,4*ImageSize.x,
			ColorSpaceRGB, kCGImageAlphaPremultipliedLast|kCGBitmapByteOrder32Little);
		CGColorSpaceRelease(ColorSpaceRGB);

		fPainter->Paint(&PaintContext);

		auto Image=CGBitmapContextCreateImage(PaintContext.Context);
		//fLayer.contents=reinterpret_cast<id>(Image);
		fLayer.contents=(__bridge id)Image;
		CGImageRelease(Image);
		CGContextRelease(PaintContext.Context);
	}
}
//---------------------------------------------------------------------------
void cMacSimpleViewContent::OnLayerLayout(void)
{
	if(fPainter!=nullptr){
		fPainter->PaintRectChanged();
	}
}
//---------------------------------------------------------------------------
void cMacSimpleViewContent::SetLayerScale(CGFloat Scale)
{
	if(fLayer.contentsScale==Scale)
		return;
	fLayer.contentsScale=Scale;
	OnLayerLayout();
}
//---------------------------------------------------------------------------
void cMacSimpleViewContent::SetLayerFrame(CGRect Frame)
{
	fLayer.frame=Frame;
	RefreshLayer();

	if(Frame.size.width==0 && Frame.size.height==0){
		fEmptySize=true;
	}
	else{
		fEmptySize=false;
	}
	OnLayerLayout();
}
//---------------------------------------------------------------------------
void cMacSimpleViewContent::OnViewStateChanged(void)
{
	UpdatePainterState();
}
//---------------------------------------------------------------------------
void cMacSimpleViewContent::UpdatePainterState(void)
{
	if(fPainter==nullptr){
		fPainterState=UIState::Null;
		return;
	}
	auto PrevState=fPainterState;
	if(fHostView==nullptr){
		if(fPainterState!=UIState::Null){
			fPainterState=UIState::Null;
			NotifyPainterState(PrevState,UIState::Null);
		}
		return;
	}

	fPainterState=fHostView->GetUIState();

	NotifyPainterState(PrevState,fPainterState);
}
//---------------------------------------------------------------------------
void cMacSimpleViewContent::cPainterNotifier::UIStarted(void){
	This->fPainter->PaintStarted();
	if(This->fNeedNotifyRectChange){
		This->fPainter->PaintRectChanged();
		This->fNeedNotifyRectChange=false;
	}
}
void cMacSimpleViewContent::cPainterNotifier::UIShow(void){		This->fPainter->PaintShow();	}
void cMacSimpleViewContent::cPainterNotifier::UIResume(void){	This->fPainter->PaintResume();	}
void cMacSimpleViewContent::cPainterNotifier::UIPaused(void){	This->fPainter->PaintPaused();	}
void cMacSimpleViewContent::cPainterNotifier::UIHide(void){		This->fPainter->PaintHide();	}
void cMacSimpleViewContent::cPainterNotifier::UIStopped(void){	This->fPainter->PaintStopped();	}
//---------------------------------------------------------------------------
void cMacSimpleViewContent::NotifyPainterState(UIState PrevState,UIState NextState)
{
	if(fPainter==nullptr)
		return;
	cPainterNotifier Notifier={this};
	UIStateNotifyChange(PrevState,NextState,Notifier);
}
//---------------------------------------------------------------------------
rPtr<iUISimplePaintDevice> cMacSimpleViewContent::QueryDevice(void)
{
	return &gCGSimplePaintDevice;
}
//---------------------------------------------------------------------------
iUIView* cMacSimpleViewContent::GetView(void)
{
	return fHostView;
}
//---------------------------------------------------------------------------
bool cMacSimpleViewContent::SetView(iUIView *View)
{
	if(fHostView!=nullptr){
		// notify painter stopped
		if(fPainterState!=UIState::Null){
			auto PrevState=fPainterState;
			fPainterState=UIState::Null;
			NotifyPainterState(PrevState,UIState::Null);
		}
		// remove from previous host
		auto LayerHost=iCast<iUIViewCALayerHost>(fHostView);
		LayerHost->RemoveViewCALayer(this);
	}
	cnLib_ASSERT(fPainterState==UIState::Null);
	fHostView=nullptr;
	if(View==nullptr){
		return true;
	}
	auto LayerHost=iCast<iUIViewCALayerHost>(View);
	if(LayerHost==nullptr){
		return false;
	}
	if(LayerHost->InsertViewCALayer(this)==false){
		return false;
	}
	fHostView=View;
	if(fPainter!=nullptr){
		fNeedNotifyRectChange=true;
		UpdatePainterState();
	}

	return true;
}
//---------------------------------------------------------------------------
bool cMacSimpleViewContent::GetVisible(void)
{
	return !fLayer.hidden;
}
//---------------------------------------------------------------------------
bool cMacSimpleViewContent::SetVisible(bool Visible)
{
	fLayer.hidden=!Visible;
	return true;
}
//---------------------------------------------------------------------------
sfInt16 cMacSimpleViewContent::GetZIndex(void)
{
	return static_cast<sfInt16>(fLayer.zPosition);
}
//---------------------------------------------------------------------------
bool cMacSimpleViewContent::SetZIndex(sfInt16 ZIndex)
{
	fLayer.zPosition=ZIndex;
	return true;
}
//---------------------------------------------------------------------------
void cMacSimpleViewContent::SetPainter(iUISimplePainter *Painter)
{
	fPainter=Painter;
	fNeedNotifyRectChange=true;
	UpdatePainterState();
	RefreshLayer();
}
//---------------------------------------------------------------------------
void cMacSimpleViewContent::RefreshLayer(void)
{
	//fLayer.contents=nil;
	[fLayer setNeedsDisplay];
}
//---------------------------------------------------------------------------
void cMacSimpleViewContent::QueryUpdate(void)
{
	RefreshLayer();
}
//---------------------------------------------------------------------------
UIState cMacSimpleViewContent::GetPaintState(void)
{
	return UIState::Active;
}
//---------------------------------------------------------------------------
cUIPoint cMacSimpleViewContent::GetPaintSize(void)
{
	auto Scale=fLayer.contentsScale;
	auto Bounds=fLayer.bounds;
	cUIPoint r;
	r.x=Bounds.size.width*Scale;
	r.y=Bounds.size.height*Scale;
	return r;
}
//---------------------------------------------------------------------------




#if	0
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cCGImageSimplePaintBitmap::cCGImageSimplePaintBitmap(cUIPoint Size)
{
	fImageSize=Size;
	auto ColorSpaceRGB=CGColorSpaceCreateDeviceRGB();
	fPaintContext.Context=CGBitmapContextCreate(nullptr,fImageSize.x,fImageSize.y,8,4*fImageSize.x,
		ColorSpaceRGB, kCGImageAlphaPremultipliedLast|kCGBitmapByteOrder32Little);
	CGColorSpaceRelease(ColorSpaceRGB);
}
//---------------------------------------------------------------------------
cCGImageSimplePaintBitmap::~cCGImageSimplePaintBitmap()
{
	if(fCachedImage!=nullptr){
		CGImageRelease(fCachedImage);
	}
	CGContextRelease(fPaintContext.Context);
}
//---------------------------------------------------------------------------
const void* cnLib_FUNC cCGImageSimplePaintBitmap::CastInterface(iTypeID InterfaceID)const
{
	return ImpCastInterface<iCFCGImage,iCGContextDrawable>(this,InterfaceID);
}
//---------------------------------------------------------------------------
CGImageRef cnLib_FUNC cCGImageSimplePaintBitmap::GetCGImage(void)
{
	if(fCachedImage==nullptr){
		fCachedImage=CGBitmapContextCreateImage(fPaintContext.Context);
	}
	return fCachedImage;
}
//---------------------------------------------------------------------------
void cCGImageSimplePaintBitmap::Discard(void)
{
	if(fCachedImage!=nullptr){
		CGImageRelease(fCachedImage);
		fCachedImage=nullptr;
	}
}
//---------------------------------------------------------------------------
cUIPoint cnLib_FUNC cCGImageSimplePaintBitmap::GetSize(void)
{
	return fImageSize;
}
//---------------------------------------------------------------------------
void cnLib_FUNC cCGImageSimplePaintBitmap::GetInfo(cUIBitmapInfo &BitmapInfo)
{
	BitmapInfo.Format=UIBitmapFormat::R8G8B8A8;
}
//---------------------------------------------------------------------------
uintn cnLib_FUNC cCGImageSimplePaintBitmap::GetDataSize(void)
{
	return fImageSize.x*fImageSize.y*4;
}
//---------------------------------------------------------------------------
bool cnLib_FUNC cCGImageSimplePaintBitmap::CopyPixels(void *Dest,uintn DestSize)
{
	auto PixelData=CGBitmapContextGetData(fPaintContext.Context);
	uintn CopySize=GetDataSize();
	if(CopySize>DestSize)
		CopySize=DestSize;

	cnMemory::Copy(Dest,PixelData,CopySize);
	return false;
}
//---------------------------------------------------------------------------
const void* cnLib_FUNC cCGImageSimplePaintBitmap::AcquirePixels(void)
{
	return CGBitmapContextGetData(fPaintContext.Context);
}
//---------------------------------------------------------------------------
void cnLib_FUNC cCGImageSimplePaintBitmap::ReleasePixels(void)
{
}
//---------------------------------------------------------------------------
bool cnLib_FUNC cCGImageSimplePaintBitmap::DrawInContext(CGContextRef Context,const CGRect &Rect)
{
	auto Image=GetCGImage();
	if(Image==nullptr)
		return false;
	CGContextDrawImage(Context,Rect,Image);
	return true;
}
//---------------------------------------------------------------------------
iUISimplePaintContext* cnLib_FUNC cCGImageSimplePaintBitmap::Paint(void)
{
	if(fPaintCount==0xFFFF)
		return nullptr;
	fPaintCount++;
	return &fPaintContext;
}
//---------------------------------------------------------------------------
void cnLib_FUNC cCGImageSimplePaintBitmap::CommitPaint(void)
{
	if(fPaintCount==0){
		return;
	}
	fPaintCount--;
	if(fPaintCount==0){
		fCachedImage=CGBitmapContextCreateImage(fPaintContext.Context);
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cCGImageBitmapBuffer::cCGImageBitmapBuffer()
{
	fBitmapImage=nullptr;
}
//---------------------------------------------------------------------------
cCGImageBitmapBuffer::~cCGImageBitmapBuffer()
{
}
//---------------------------------------------------------------------------
const void* cnLib_FUNC cCGImageBitmapBuffer::CastInterface(iTypeID InterfaceID)const
{
	return ImpCastInterface<iUIBitmapBuffer,iCFCGImage>(this,InterfaceID);
}
//---------------------------------------------------------------------------
bool cnLib_FUNC cCGImageBitmapBuffer::DrawInContext(CGContextRef Context,const CGRect &Rect)
{
	return Draw(Context,Rect);
}
//---------------------------------------------------------------------------
CGImageRef cnLib_FUNC cCGImageBitmapBuffer::GetCGImage(void)
{
	return fBitmapImage;
}
//---------------------------------------------------------------------------
bool cCGImageBitmapBuffer::Draw(CGContextRef Context,const CGRect &Rect)
{
	if(fBitmapImage==nullptr)
		return false;
	CGContextDrawImage(Context,Rect,fBitmapImage);
	return true;
}
//---------------------------------------------------------------------------
void cCGImageBitmapBuffer::CreateBitmap(void)
{
	fBitmapDataSize=4*fBitmapWidth*fBitmapHeight;
	if(fBitmapDataSize==0)
		return;
	fBitmapData=RTLHeap::Alloc(fBitmapDataSize);

	fBitmapProvider=CGDataProviderCreateWithData(nullptr,fBitmapData,fBitmapDataSize,nullptr);

	auto colorSpace = CGColorSpaceCreateDeviceRGB();
	fBitmapImage=CGImageCreate(fBitmapWidth,fBitmapHeight,8,32,4*fBitmapWidth,
		colorSpace, kCGImageAlphaLast,
		fBitmapProvider,nullptr,false,kCGRenderingIntentDefault);

	CGColorSpaceRelease(colorSpace);
}
//---------------------------------------------------------------------------
void cCGImageBitmapBuffer::ClearBitmap(void)
{
	CGImageRelease(fBitmapImage);
	CGDataProviderRelease(fBitmapProvider);
	RTLHeap::Free(fBitmapData);
}
//---------------------------------------------------------------------------
cUIPoint cnLib_FUNC cCGImageBitmapBuffer::GetSize(void)
{
	return {fBitmapWidth,fBitmapHeight};
}
//---------------------------------------------------------------------------
void cnLib_FUNC cCGImageBitmapBuffer::GetInfo(cUIBitmapInfo &BitmapInfo)
{
	BitmapInfo.Format=UIBitmapFormat::R8G8B8A8;
}
//---------------------------------------------------------------------------
uintn cnLib_FUNC cCGImageBitmapBuffer::GetDataSize(void)
{
	return fBitmapDataSize;
}
//---------------------------------------------------------------------------
bool cnLib_FUNC cCGImageBitmapBuffer::CopyPixels(void *Dest,uintn DestSize)
{
	uintn CopySize=fBitmapDataSize;
	if(CopySize<DestSize){
		CopySize=DestSize;
	}
	cnMemory::Copy(Dest,fBitmapData,CopySize);
	return true;
}
//---------------------------------------------------------------------------
const void* cnLib_FUNC cCGImageBitmapBuffer::AcquirePixels(void)
{
	return AcquireEditPixels();
}
//---------------------------------------------------------------------------
void cnLib_FUNC cCGImageBitmapBuffer::ReleasePixels(void)
{
	return ReleaseEditPixels();
}
//---------------------------------------------------------------------------
bool cnLib_FUNC cCGImageBitmapBuffer::SetParameter(const cUIBitmapInfo &BitmapInfo,sint32 Width,sint32 Height)
{
	if(fPixelLockCount!=0)
		return false;

	if(fBitmapImage!=nullptr){
		ClearBitmap();
		fBitmapImage=nullptr;
	}


	{
		fBitmapWidth=Width;
		fBitmapHeight=Height;
	}
	CreateBitmap();
	return true;
}
//---------------------------------------------------------------------------
void* cCGImageBitmapBuffer::AcquireEditPixels(void)
{
	if(fPixelLockCount>=0xFFFF){
		return nullptr;
	}
	fPixelLockCount++;
	return fBitmapData;
}
//---------------------------------------------------------------------------
void cCGImageBitmapBuffer::ReleaseEditPixels(void)
{
	if(fPixelLockCount!=0){
		fPixelLockCount--;
	}
}

#endif
