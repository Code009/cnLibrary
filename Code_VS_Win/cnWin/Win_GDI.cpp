#include "Win_GDI.h"

using namespace cnLibrary;
using namespace cnRTL;
using namespace cnWin;

#if 0

cnLib_INTERFACE_LOCALID_DEFINE(cGDIFontName);

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
HBITMAP cnWin::DCCreateBitmapFromSource(HDC DC,IWICBitmapSource *BitmapSource,bool &HasAlpha)
{
	cnLib_ASSERT(DC!=nullptr);

	WICPixelFormatGUID FormatGUID;
	auto hr=BitmapSource->GetPixelFormat(&FormatGUID);
	if(FAILED(hr)){
		return nullptr;
	}

	auto Factory=Windows::WICQueryImagingFactory();

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
HBITMAP cnWin::DCCreateBitmapFromSource(HDC DC,iBitmapDataSource *Source,bool &HasAlpha)
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
cGDIFontName::cGDIFontName(const wchar_t *Name)
	:fName(Name)
{
}
//---------------------------------------------------------------------------
cGDIFontName::cGDIFontName(cString<wchar_t> &&Name)
	:fName(Name)
{

}
//---------------------------------------------------------------------------
const cnRTL::cString<wchar_t>& cGDIFontName::FontNameString(void)
{
	return fName;
}
//---------------------------------------------------------------------------
const wchar_t* cGDIFontName::FontName(void)
{
	return fName;
}
//---------------------------------------------------------------------------
uIntn cGDIFontName::FontLength(void)
{
	return fName->Length;
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
	if(Size<Src.Size)
		return true;
	if(Weight<Src.Weight)
		return true;
	if(Italic<Src.Italic)
		return true;
	if(Underline<Src.Underline)
		return true;
	if(Strike<Src.Strike)
		return true;
	if(Underline<Src.Underline)
		return true;
	if(FontName<Src.FontName)
		return true;
	return false;
}
bool cDCPaintDevice::cFontIndex::operator ==(const cFontIndex &Src)const
{
	if(Size!=Src.Size)
		return false;
	if(Weight!=Src.Weight)
		return false;
	if(Italic!=Src.Italic)
		return false;
	if(Underline!=Src.Underline)
		return false;
	if(Strike!=Src.Strike)
		return false;
	if(Underline!=Src.Underline)
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
	if(TextStyle.Font!=nullptr){
		auto WinFontName=iCast<cGDIFontName>(TextStyle.Font);
		if(WinFontName==nullptr){
			// not supported font
			return nullptr;
		}
		Index.FontName=WinFontName->FontNameString();
	}
	Index.Size=TextStyle.FontSize;
	Index.Weight=TextStyle.Weight;
	Index.Italic=TextStyle.Italic;
	Index.Underline=TextStyle.Underline;
	Index.Strike=TextStyle.Strike;

	auto &MapItem=fFontMap[Index];
	if(MapItem==nullptr){
		LOGFONTW lFont;
		lFont.lfHeight=-static_cast<LONG>(TextStyle.FontSize);
		lFont.lfWidth=0;
		lFont.lfEscapement=0;
		lFont.lfOrientation=0;
		lFont.lfWeight=FW_NORMAL+TextStyle.Weight*5;
		if(lFont.lfWeight<FW_THIN){
			lFont.lfWeight=FW_THIN;
		}
		else if(lFont.lfWeight>FW_HEAVY){
			lFont.lfWeight=FW_HEAVY;
		}
		lFont.lfItalic=TextStyle.Italic;
		lFont.lfUnderline=TextStyle.Underline;
		lFont.lfStrikeOut=TextStyle.Strike;
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
#endif // 0
