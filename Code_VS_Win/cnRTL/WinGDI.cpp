#include "WinGDI.h"
#include <cnRTL\InterfaceImplementation.h>

using namespace cnLibrary;
using namespace cnRTL;
using namespace cnWinRTL;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
COLORREF cnWinRTL::COLORREFFromUIColor(const cUIColor &UIColor)
{
#pragma message (cnLib_FILE_LINE ": TODO - use vector")
	return static_cast<uInt32>(UIColor.Red*255.f) |
		(static_cast<uInt32>(UIColor.Green*255.f)<<8) |
		(static_cast<uInt32>(UIColor.Blue*255.f)<<16);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cGDIBitmapBuffer::cGDIBitmapBuffer()
{
	Bitmap=nullptr;
	Width=0;
	Height=0;
	Stride=0;
	BitmapSize=0;
	BitmapData=nullptr;
}
//---------------------------------------------------------------------------
cGDIBitmapBuffer::~cGDIBitmapBuffer()
{
	if(Bitmap!=nullptr){
		::DeleteObject(Bitmap);
	}
}
//---------------------------------------------------------------------------
void cGDIBitmapBuffer::Clear(void)
{
	if(Bitmap!=nullptr){
		::DeleteObject(Bitmap);
		Bitmap=nullptr;
	}
	Width=0;
	Height=0;
	Stride=0;
	BitmapSize=0;
	BitmapData=nullptr;
}
//---------------------------------------------------------------------------
void cGDIBitmapBuffer::Setup(HDC DC,int NewWidth,int NewHeight)
{
	// clear old bitmap
	Clear();
	if(NewWidth==0 || NewHeight==0){
		return;
	}

	BITMAPINFO bi;
	bi.bmiHeader.biSize=sizeof(bi.bmiHeader);
	bi.bmiHeader.biHeight=NewHeight;
	bi.bmiHeader.biWidth=NewWidth;

	Width=NewWidth;
	if(NewHeight<0)
		Height=-NewHeight;
	else
		Height=NewHeight;

	bi.bmiHeader.biPlanes=1;
	bi.bmiHeader.biBitCount=32;

	Stride=Width*bi.bmiHeader.biBitCount/ByteBitCount;
	//fStride=cnInteger::RoundPositive<4>(fStride);

	BitmapSize=Height*Stride;

	bi.bmiHeader.biSizeImage=BitmapSize;
	bi.bmiHeader.biCompression=BI_RGB;
	bi.bmiHeader.biXPelsPerMeter=0;
	bi.bmiHeader.biYPelsPerMeter=0;
	bi.bmiHeader.biClrUsed=0;
	bi.bmiHeader.biClrImportant=0;

	Bitmap=::CreateDIBSection(DC,&bi,DIB_RGB_COLORS,&BitmapData,nullptr,0);
	if(Bitmap==nullptr){
		Clear();
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cGDIBitmapSection::cGDIBitmapSection()
{
	Section=nullptr;
	Bitmap=nullptr;
	Width=0;
	Height=0;
}
//---------------------------------------------------------------------------
cGDIBitmapSection::~cGDIBitmapSection()
{
	if(Bitmap!=nullptr){
		::DeleteObject(Bitmap);
		::CloseHandle(Section);
	}
}
//---------------------------------------------------------------------------
void cGDIBitmapSection::Clear(void)
{
	if(Bitmap!=nullptr){
		::DeleteObject(Bitmap);
		::CloseHandle(Section);
		Bitmap=nullptr;
		Section=nullptr;
	}
	Width=0;
	Height=0;
	Stride=0;
	BitmapSize=0;
	BitmapData=nullptr;
}
//---------------------------------------------------------------------------
void cGDIBitmapSection::Setup(HDC DC,int NewWidth,int NewHeight)
{
	// clear old bitmap
	Clear();
	if(NewWidth==0 || NewHeight==0){
		return;
	}

	BITMAPINFO bi;
	bi.bmiHeader.biSize=sizeof(bi.bmiHeader);
	bi.bmiHeader.biHeight=NewHeight;
	bi.bmiHeader.biWidth=NewWidth;

	Width=NewWidth;
	if(NewHeight<0)
		Height=-NewHeight;
	else
		Height=NewHeight;

	bi.bmiHeader.biPlanes=1;
	bi.bmiHeader.biBitCount=32;

	Stride=Width*bi.bmiHeader.biBitCount/ByteBitCount;
	//fStride=cnInteger::RoundPositive<4>(fStride);

	BitmapSize=Height*Stride;

	bi.bmiHeader.biSizeImage=BitmapSize;
	bi.bmiHeader.biCompression=BI_RGB;
	bi.bmiHeader.biXPelsPerMeter=0;
	bi.bmiHeader.biYPelsPerMeter=0;
	bi.bmiHeader.biClrUsed=0;
	bi.bmiHeader.biClrImportant=0;

	Section=::CreateFileMappingW(INVALID_HANDLE_VALUE,nullptr,PAGE_READWRITE,0,BitmapSize,nullptr);
	
	Bitmap=::CreateDIBSection(DC,&bi,DIB_RGB_COLORS,&BitmapData,Section,0);
	if(Bitmap==nullptr){
		::CloseHandle(Section);
		Section=nullptr;
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cGDIBitmap::cGDIBitmap(HBITMAP Bitmap,bool HasAlpha)
{
	fBitmap=Bitmap;
	fHasAlpha=HasAlpha;
}
//---------------------------------------------------------------------------
cGDIBitmap::~cGDIBitmap()
{
	if(fBitmap!=nullptr){
		::DeleteObject(fBitmap);
	}
}
//---------------------------------------------------------------------------
void* cGDIBitmap::CastInterface(iTypeID InterfaceID)noexcept(true)
{
	return ImpCastInterface<iGDIBitmap>(this,InterfaceID);
}
//---------------------------------------------------------------------------
cUIPoint cGDIBitmap::GetSize(void)
{
	BITMAP BmpInfo;
	if(::GetObjectW(fBitmap,sizeof(BmpInfo),&BmpInfo)){
		return {static_cast<Float32>(BmpInfo.bmWidth),static_cast<Float32>(BmpInfo.bmHeight)};
	}
	return {0,0};
}
//---------------------------------------------------------------------------
HBITMAP cGDIBitmap::GetBitmap(void)
{
	return fBitmap;
}
//---------------------------------------------------------------------------
bool cGDIBitmap::HasAlpha(void)
{
	return fHasAlpha;
}
//---------------------------------------------------------------------------
bool cnWinRTL::GDIDrawBitmap(HDC DC,int DrawX,int DrawY,int DrawWidth,int DrawHeight,HDC MemDC,HBITMAP Bitmap,int BmpWidth,int BmpHeight,bool HasAlpha)
{
	auto OldBitmap=::SelectObject(MemDC,Bitmap);
	BOOL r;
	if(HasAlpha){
		BLENDFUNCTION bf;
		bf.BlendOp=AC_SRC_OVER;
		bf.BlendFlags=0;
		bf.SourceConstantAlpha=0xFF;
		bf.AlphaFormat=AC_SRC_ALPHA;
		r=::AlphaBlend(DC,DrawX,DrawY,DrawWidth,DrawHeight,MemDC,0,0,BmpWidth,BmpHeight,bf);
	}
	else{
		if(DrawWidth==BmpWidth && DrawHeight==BmpHeight){
			r=::BitBlt(DC,DrawX,DrawY,DrawWidth,DrawHeight,MemDC,0,0,SRCCOPY);
		}
		else{
			r=::StretchBlt(DC,DrawX,DrawY,DrawWidth,DrawHeight,MemDC,0,0,BmpWidth,BmpHeight,SRCCOPY);
		}
	}

	// restore bitmap
	::SelectObject(MemDC,OldBitmap);
	return r!=0;
}
//---------------------------------------------------------------------------
static const BITMAPINFOHEADER DCBitmapHeader_BGR32={
	sizeof(BITMAPINFOHEADER),	//biSize;
	0,	//biWidth;
	0,	//biHeight;
	1,	//biPlanes;
	32,	//biBitCount;
	BI_RGB,	//biCompression;
	0,	//biSizeImage;
	0,	//biXPelsPerMeter;
	0,	//biYPelsPerMeter;
	0,	//biClrUsed;
	0,	//biClrImportant;
};
//---------------------------------------------------------------------------
static const BITMAPV4HEADER DCBitmapV4Header_BGR32={
	sizeof(BITMAPV4HEADER),	//bV4Size;
	0,	//bV4Width;
	0,	//bV4Height;
	1,	//bV4Planes;
	32,	//bV4BitCount;
	BI_BITFIELDS,	//bV4V4Compression;
	0,	//bV4SizeImage;
	0,	//bV4XPelsPerMeter;
	0,	//bV4YPelsPerMeter;
	0,	//bV4ClrUsed;
	0,	//bV4ClrImportant;
	0xFF0000,	//bV4RedMask;
	0xFF00,		//bV4GreenMask;
	0xFF,		//bV4BlueMask;
	0xFF000000,	//bV4AlphaMask;
	LCS_WINDOWS_COLOR_SPACE,	//bV4CSType;
	0,	//bV4Endpoints;
	0,	//bV4GammaRed;
	0,	//bV4GammaGreen;
	0,	//bV4GammaBlue;
};
//---------------------------------------------------------------------------
static const BITMAPV4HEADER DCBitmapV4Header_RGB32={
	sizeof(BITMAPV4HEADER),	//bV4Size;
	0,	//bV4Width;
	0,	//bV4Height;
	1,	//bV4Planes;
	32,	//bV4BitCount;
	BI_BITFIELDS,	//bV4V4Compression;
	0,	//bV4SizeImage;
	0,	//bV4XPelsPerMeter;
	0,	//bV4YPelsPerMeter;
	0,	//bV4ClrUsed;
	0,	//bV4ClrImportant;
	0xFF,		//bV4RedMask;
	0xFF00,		//bV4GreenMask;
	0xFF0000,	//bV4BlueMask;
	0xFF000000,	//bV4AlphaMask;
	LCS_WINDOWS_COLOR_SPACE,	//bV4CSType;
	0,	//bV4Endpoints;
	0,	//bV4GammaRed;
	0,	//bV4GammaGreen;
	0,	//bV4GammaBlue;
};
//---------------------------------------------------------------------------
HBITMAP cnWinRTL::DCCreateBitmapFromSource(HDC DC,IWICBitmapSource *BitmapSource,bool &HasAlpha)
{
	cnLib_ASSERT(DC!=nullptr);

	WICPixelFormatGUID FormatGUID;
	auto hr=BitmapSource->GetPixelFormat(&FormatGUID);
	if(FAILED(hr)){
		return nullptr;
	}

	auto Factory=cnWinRTL::WICQueryImagingFactory();

	COMPtr<IWICBitmap> Bitmap;
	if(FormatGUID==GUID_WICPixelFormat32bppBGR || FormatGUID==GUID_WICPixelFormat32bppPBGRA){
		HasAlpha=(FormatGUID==GUID_WICPixelFormat32bppPBGRA);
		// use source format
		hr=Factory->CreateBitmapFromSource(BitmapSource,WICBitmapCacheOnLoad,COMRetPtrT(Bitmap));
		if(FAILED(hr)){
			return nullptr;
		}
	}
	else{
		COMPtr<IWICFormatConverter> ImageConverter;
		hr=Factory->CreateFormatConverter(COMRetPtrT(ImageConverter));
		if(FAILED(hr)){
			return nullptr;
		}

		COMPtr<IWICComponentInfo> ComponentInfo;
		hr=Factory->CreateComponentInfo(FormatGUID,COMRetPtrT(ComponentInfo));
		if(FAILED(hr)){
			return nullptr;
		}
		COMPtr<IWICPixelFormatInfo> PixelFormatInfo;
		hr=ComponentInfo->QueryInterface(COMRetPtrT(PixelFormatInfo));
		if(FAILED(hr)){
			return nullptr;
		}

		bool ConvertWithAlpha=false;
		UINT ChannelCount;
		hr=PixelFormatInfo->GetChannelCount(&ChannelCount);
		if(SUCCEEDED(hr)){
			if(ChannelCount>=4){
				ConvertWithAlpha=true;
			}
		}

		if(ConvertWithAlpha){
			// use BGRPA
			FormatGUID=GUID_WICPixelFormat32bppPBGRA;
			HasAlpha=true;
		}
		else{
			// convert to BGRX
			FormatGUID=GUID_WICPixelFormat32bppBGR;
			HasAlpha=false;
		}
		
		hr=ImageConverter->Initialize(BitmapSource,FormatGUID,WICBitmapDitherTypeNone,nullptr,0.f,WICBitmapPaletteTypeCustom);
		if(FAILED(hr)){
			return nullptr;
		}

		// create bitmap

		hr=Factory->CreateBitmapFromSource(ImageConverter,WICBitmapCacheOnLoad,COMRetPtrT(Bitmap));
		if(FAILED(hr)){
			return nullptr;
		}
	}
	WICRect ImageRect;
	ImageRect.X=0;
	ImageRect.Y=0;
	Bitmap->GetSize(reinterpret_cast<UINT*>(&ImageRect.Width),reinterpret_cast<UINT*>(&ImageRect.Height));

	union{
		BITMAPINFO BitmapInfo;
		BITMAPINFOHEADER BitmapHeader;
	};
	BitmapHeader=DCBitmapHeader_BGR32;
	BitmapHeader.biWidth=ImageRect.Width;
	BitmapHeader.biHeight=-ImageRect.Height;
	BitmapHeader.biSizeImage=ImageRect.Width*ImageRect.Height*4;
	
	{
		COMPtr<IWICBitmapLock> BitmapLock;
		hr=Bitmap->Lock(&ImageRect,WICBitmapLockRead,COMRetPtrT(BitmapLock));
		if(FAILED(hr)){
			return nullptr;
		}
		
		UINT BufferSize;
		WICInProcPointer Buffer;
		hr=BitmapLock->GetDataPointer(&BufferSize,&Buffer);
		if(FAILED(hr)){
			return nullptr;
		}

		auto GDIBitmap=::CreateDIBitmap(DC,&BitmapInfo.bmiHeader,CBM_INIT,Buffer,&BitmapInfo,DIB_RGB_COLORS);

		union{
			BITMAPINFO BitmapInfov4;
			BITMAPV4HEADER BitmapHeaderv4;
		};
		BitmapHeaderv4.bV4Size=sizeof(BitmapHeaderv4);
		::GetObjectW(GDIBitmap,sizeof(BitmapHeaderv4),&BitmapHeaderv4);

		if(BitmapHeaderv4.bV4AlphaMask==0){
		}

		return GDIBitmap;
	}

}
//---------------------------------------------------------------------------
HBITMAP cnWinRTL::DCCreateBitmapFromSource(HDC DC,iBitmapDataSource *Source,bool &HasAlpha)
{
	cnLib_ASSERT(DC!=nullptr);
	// load COM
	auto COMSource=iCast<iCOMInterface>(Source);
	if(COMSource!=nullptr){
		auto *pCOM=COMSource->GetCOMInterface();
		COMPtr<IWICBitmapSource> BitmapSource;
		if(SUCCEEDED(pCOM->QueryInterface(COMRetPtrT(BitmapSource)))){
			return DCCreateBitmapFromSource(DC,BitmapSource,HasAlpha);
		}
	}
	// load normally
	HasAlpha=false;
	auto SrcSize=Source->GetImageSize();

	union{
		BITMAPINFO SrcBitmapInfo;
		BITMAPV4HEADER SrcBitmapV4Header;
	};
	
	auto Format=Source->GetPixelFormat();
	if(Format.ChannelType!=BitmapChannelType::UNorm || Format.ChannelSize!=1){
		// not supported
		return nullptr;
	}
	
	switch(Format.ChannelColor){
	default:
	case BitmapChannelColor::Unknown:
		return nullptr;
	case BitmapChannelColor::BGR:
		if(Format.ChannelCount==3){
			SrcBitmapInfo.bmiHeader=DCBitmapHeader_BGR32;
		}
		else{
			SrcBitmapV4Header=DCBitmapV4Header_RGB32;
			SrcBitmapV4Header.bV4AlphaMask=0;
		}
		break;
	case BitmapChannelColor::BGRA:
		SrcBitmapInfo.bmiHeader=DCBitmapHeader_BGR32;
		break;
	case BitmapChannelColor::RGB:
		SrcBitmapV4Header=DCBitmapV4Header_RGB32;
		SrcBitmapV4Header.bV4AlphaMask=0;
		break;
	case BitmapChannelColor::RGBA:
		SrcBitmapV4Header=DCBitmapV4Header_RGB32;
		break;
	}

	SrcBitmapV4Header.bV4BitCount=Format.ChannelCount*Format.ChannelSize*8;
	SrcBitmapV4Header.bV4Width=SrcSize.x;
	SrcBitmapV4Header.bV4Height=SrcSize.y;
	SrcBitmapV4Header.bV4SizeImage=SrcSize.x*SrcSize.y*4;


	BITMAPINFOHEADER BmpHeader;
	cnMemory::ZeroFill(BmpHeader);
	BmpHeader.biSize=sizeof(BmpHeader);
	BmpHeader.biWidth=SrcSize.x;
	BmpHeader.biHeight=SrcSize.y;

	if(Source->IsTopDown()){
		SrcBitmapV4Header.bV4Height=-SrcBitmapV4Header.bV4Height;
		BmpHeader.biHeight=-BmpHeader.biHeight;
	}

	HBITMAP Bitmap;
	BITMAPINFOHEADER DestBitmapHeader;
	cnMemory::ZeroFill(DestBitmapHeader);
	DestBitmapHeader.biSize=sizeof(DestBitmapHeader);
	DestBitmapHeader.biWidth=SrcSize.x;
	DestBitmapHeader.biHeight=SrcSize.y;
	if(Format.AlphaPremultiplied==false){
		auto SrcBuffer=Source->AcquirePixels();

		Bitmap=::CreateDIBitmap(DC,&BmpHeader,CBM_INIT,SrcBuffer,&SrcBitmapInfo,DIB_RGB_COLORS);


		Source->ReleasePixels();

	}
	else{
		cMemoryBlock ConvertBlock;
		auto DataPitch=Source->GetDataPitch();
		uIntn DataSize=DataPitch*SrcSize.y;
		ConvertBlock.SetLength(DataSize);
		// copy pixel
		Source->CopyPixelBuffer(0,ConvertBlock.Pointer,DataSize);

		uIntn PixelCount=SrcSize.x*SrcSize.y;
		auto *pPixel=static_cast<uInt32*>(ConvertBlock.Pointer);
		for(uIntn PixelIndex=0;PixelIndex<PixelCount;PixelIndex++){
			auto pChannels=reinterpret_cast<uInt8*>(pPixel+PixelIndex);
			pChannels[0]=pChannels[0]*pChannels[3]/255;
			pChannels[1]=pChannels[1]*pChannels[3]/255;
			pChannels[2]=pChannels[2]*pChannels[3]/255;
		}
		Bitmap=::CreateDIBitmap(DC,&BmpHeader,CBM_INIT,ConvertBlock.Pointer,&SrcBitmapInfo,DIB_RGB_COLORS);
	}	
	return Bitmap;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cGDIDC::cGDIDC(HDC DC)
	: fDC(DC)
{
}
//---------------------------------------------------------------------------
cGDIDC::~cGDIDC()
{
	if(fDC!=nullptr){
		::DeleteDC(fDC);
	}
}
//---------------------------------------------------------------------------
HDC cGDIDC::GetHandle(void)
{
	return fDC;
}
//---------------------------------------------------------------------------
HDC cGDIDC::GetDC(void)
{
	return fDC;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cGDIFontHandle::cGDIFontHandle(HFONT FontHandle)
	: fFontHandle(FontHandle)
{
	
}
//---------------------------------------------------------------------------
cGDIFontHandle::~cGDIFontHandle()
{
	::DeleteObject(fFontHandle);
}
//---------------------------------------------------------------------------
HFONT cGDIFontHandle::GetFontHandle(void)
{
	return fFontHandle;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cDCPaintDevice::cMemDC::cMemDC()
	: cGDIDC(nullptr)
{
}
//---------------------------------------------------------------------------
void cDCPaintDevice::cMemDC::SetupDC(HDC IC,uIntn DCNumber)
{
	if(fDC!=nullptr){
		if(fDCNumber==DCNumber)
			return;

		::DeleteDC(fDC);
	}
	fDC=::CreateCompatibleDC(IC);
	fDCNumber=DCNumber;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool cDCPaintDevice::cFontIndex::operator <(const cFontIndex &Src)const
{
	if(InfoValue<InfoValue)
		return true;
	if(FontName<Src.FontName)
		return true;
	return false;
}
bool cDCPaintDevice::cFontIndex::operator ==(const cFontIndex &Src)const
{
	if(InfoValue!=InfoValue)
		return false;
	if(FontName!=Src.FontName)
		return false;
	return true;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cDCPaintDevice::cBitmap::cBitmap(cDCPaintDevice *Owner,iBitmapDataSource *Source,HBITMAP Bitmap,bool HasAlpha)
	: cGDIBitmap(Bitmap,HasAlpha),fSource(Source)
{
	fOwner=Owner;
	fOwner->fBitmapCache[fSource]=this;
}
//---------------------------------------------------------------------------
iBitmapDataSource *cDCPaintDevice::cBitmap::GetSource(void)const
{
	return fSource;
}
//---------------------------------------------------------------------------
void cDCPaintDevice::cBitmap::VirtualStopped(void)
{
	if(fOwner->fUIThread->IsCurrentThread()){
		ThreadDelete();
	}
	else{
		auto Dispatch=fOwner->fUIThread->GetDispatch(true);
		Dispatch->ExecuteAsync(nullptr,&fThreadDeleteProcedure);
	}
}
//---------------------------------------------------------------------------
void cDCPaintDevice::cBitmap::ThreadDelete(void)
{
	// delete
	fOwner->fBitmapCache.Remove(fSource);

	VirtualDelete();
}
//---------------------------------------------------------------------------
void cDCPaintDevice::cBitmap::cThreadDeleteProcedure::Execute(void)
{
	auto Host=cnMemory::GetObjectFromMemberPointer(this,&cBitmap::fThreadDeleteProcedure);
	Host->ThreadDelete();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cDCPaintDevice::cDCPaintDevice()
	: fUIThread(cnWindows::CreateUIThreadOnCurrentThread())
{
	fDCNumber=0;
	UpdateDisplaySetting();
}
//---------------------------------------------------------------------------
cDCPaintDevice::~cDCPaintDevice()
{
}
//---------------------------------------------------------------------------
bool cDCPaintDevice::CheckThread(void)
{
	return fUIThread->IsCurrentThread();
}
//---------------------------------------------------------------------------
HDC cDCPaintDevice::GetScreenICHandle(void)
{
	return fScreenIC->GetHandle();
}
//---------------------------------------------------------------------------
cGDIDC* cDCPaintDevice::GetScreenIC(void)
{
	return fScreenIC;
}
//---------------------------------------------------------------------------
iGDIDC* cDCPaintDevice::GetScreenInfomationDC(void)
{
	if(CheckThread()==false)
		return nullptr;
	return fScreenIC;
}
//---------------------------------------------------------------------------
rPtr<cGDIDC> cDCPaintDevice::QueryMemDC(void)
{
	auto MemDC=fMemDCRecycler.Query();
	MemDC->SetupDC(fScreenIC->GetHandle(),fDCNumber);
	return MemDC;
}
//---------------------------------------------------------------------------
rPtr<iGDIDC> cDCPaintDevice::QueryMemoryDC(void)
{
	if(CheckThread()==false)
		return nullptr;
	return QueryMemDC();
}
//---------------------------------------------------------------------------
void cDCPaintDevice::UpdateDisplaySetting(void)
{
	fDM.dmSize=sizeof(fDM);
	fDM.dmDriverExtra=0;
	EnumDisplaySettingsW(nullptr,ENUM_CURRENT_SETTINGS,&fDM);

	HDC IC=::CreateICW(L"DISPLAY",0,0,&fDM);

	fScreenIC=rCreate<cGDIDC>(IC);

	fDCNumber++;

}
//---------------------------------------------------------------------------
rPtr<iGDIFontHandle> cDCPaintDevice::QueryFont(const cUITextStyle &TextStyle)
{
	if(CheckThread()==false)
		return nullptr;

	cFontIndex Index;
	Index.InfoValue=0;
	if(TextStyle.Font!=nullptr){
		auto TextFontNameArray=TextStyle.Font->GetName();
		uIntn TextFontNameLength;
		auto TextFontName=TextFontNameArray->GetArray(TextFontNameLength);
		Index.FontName.SetString(utow(TextFontName),TextFontNameLength);

		Float32 FontWeight=TextStyle.Font->GetWeight();
		LONG GDIFontWeight=static_cast<LONG>(FW_NORMAL+FontWeight*500);
		
		if(GDIFontWeight<FW_THIN){
			GDIFontWeight=FW_THIN;
		}
		else if(GDIFontWeight>FW_HEAVY){
			GDIFontWeight=FW_HEAVY;
		}
		Index.Info.Weight=GDIFontWeight;

		switch(TextStyle.Font->GetStyle()){
		default:
		case UIFontStyle::Normal:
			Index.Info.Italic=0;
			break;
		case UIFontStyle::Italic:
			Index.Info.Italic=1;
			break;
		}
	}
	else{
		Index.Info.Weight=FW_NORMAL;
	}
	Index.Info.Size=TextStyle.FontSize;
	Index.Info.Underline=TextStyle.Underline;
	Index.Info.Strike=TextStyle.Strike;

	auto &MapItem=fFontMap[Index];
	if(MapItem==nullptr){
		LOGFONTW lFont;
		lFont.lfHeight=-Index.Info.Size;
		lFont.lfWidth=0;
		lFont.lfEscapement=0;
		lFont.lfOrientation=0;
		lFont.lfWeight=Index.Info.Weight;
		lFont.lfItalic=Index.Info.Italic;
		lFont.lfUnderline=Index.Info.Underline;
		lFont.lfStrikeOut=Index.Info.Strike;
		lFont.lfCharSet=DEFAULT_CHARSET;
		lFont.lfOutPrecision=OUT_DEFAULT_PRECIS;
		lFont.lfClipPrecision=CLIP_DEFAULT_PRECIS;
		lFont.lfQuality=DEFAULT_QUALITY;
		lFont.lfPitchAndFamily=DEFAULT_PITCH|FF_DONTCARE;
		
		if(Index.FontName->Length!=0){
			cnString::Copy(lFont.lfFaceName,Index.FontName->Pointer,Index.FontName->Length);
		}
		else{
			HFONT SysFont=(HFONT)::GetStockObject(SYSTEM_FONT);
			LOGFONT SysFontInfo;
			::GetObjectW(SysFont,sizeof(SysFontInfo),&SysFontInfo);
		
			cnString::Copy(lFont.lfFaceName,SysFontInfo.lfFaceName);
		}

		auto FontHandle=::CreateFontIndirectW(&lFont);
		if(FontHandle==nullptr)
			return nullptr;
		MapItem=rCreate<cGDIFontHandle>(FontHandle);
	}
	return MapItem;
}
//---------------------------------------------------------------------------
iPtr<iGDIBitmap> cDCPaintDevice::CreateBitmapCopyFromSource(iBitmapDataSource *Source)
{
	if(CheckThread()==false)
		return nullptr;

	if(Source==nullptr)
		return nullptr;
	// check cache
	auto Pair=fBitmapCache.GetPair(Source);
	if(Pair!=nullptr){
		return Pair->Value;
	}

	bool Alpha;
	auto Bitmap=DCCreateBitmapFromSource(fScreenIC->GetHandle(),Source,Alpha);

	if(Bitmap==nullptr)
		return nullptr;

	return iCreate<cBitmap>(this,Source,Bitmap,Alpha);
}
//---------------------------------------------------------------------------
