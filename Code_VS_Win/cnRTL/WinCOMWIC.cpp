#include "WinCOMWIC.h"
#include <cnRTL\UIGraph.h>
#include <mmintrin.h>

#pragma comment(lib,"msimg32.lib")
	// AlphaBlend

using namespace cnLibrary;
using namespace cnRTL;
using namespace cnWinRTL;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
COMPtr<IWICImagingFactory> cnWinRTL::WICQueryImagingFactory(void)noexcept
{
	HRESULT hr;
	COMPtr<IWICImagingFactory> ImageFactory;
	hr=::CoCreateInstance(CLSID_WICImagingFactory,nullptr,CLSCTX_INPROC_SERVER,__uuidof(IWICImagingFactory),COMRetPtr(ImageFactory));
	if(FAILED(hr)){
		return nullptr;
	}

	return ImageFactory;
}

#if 0
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cBitmapConvert::cBitmapConvert()
{
	fSize={0,0};
	fFormat=BitmapFormat::Unknown;
	fResourceData=nullptr;
	fResPixelData=nullptr;
}
//---------------------------------------------------------------------------
cBitmapConvert::~cBitmapConvert()
{
}
//---------------------------------------------------------------------------
void cBitmapConvert::ClearData(void)
{
	fResourceData=nullptr;
	fResPixelData=nullptr;
	fConvertedPixelBuffer.Clear();
	fSize={0,0};
	fFormat=BitmapFormat::Unknown;
}
//---------------------------------------------------------------------------
bool cBitmapConvert::SetIconData(const void *ResourceData,uIntn ResourceDataSize)
{
	if(fPixelLockCount!=0)	// in use
		return false;

	fResourceData=static_cast<const BITMAPINFO*>(ResourceData);

	uIntn ImageDataOffset=fResourceData->bmiHeader.biSize;

	fSize.x=fResourceData->bmiHeader.biWidth;
	fTopDown=fResourceData->bmiHeader.biHeight<0;
	if(fTopDown){
		fSize.y=-fResourceData->bmiHeader.biHeight;
	}
	else{
		fSize.y=fResourceData->bmiHeader.biHeight;
	}
	fSize.y/=2;

	// check data size

	uIntn PaletteDataSize;
	uIntn PixelDataSize;
	uIntn MaskImageSize;

	if(fResourceData->bmiHeader.biClrUsed==0){
		PaletteDataSize=1<<fResourceData->bmiHeader.biBitCount;
		PaletteDataSize*=4;
	}
	else{
		PaletteDataSize=fResourceData->bmiHeader.biClrUsed*4;
	}
	fResMaskLineSize=(fSize.x+7)/8;
	fResMaskLineSize=cnInteger::RoundPositive<4>(fResMaskLineSize);
	MaskImageSize=fResMaskLineSize*fSize.y;


	fFormat=BitmapFormat::B8G8R8PA8;
	switch(fResourceData->bmiHeader.biBitCount){
	default:
		// not supported
		ClearData();
		return false;
	case 1:
		fResImageLineSize=(fSize.x+7)/8;
		fResImageLineSize=cnInteger::RoundPositive<4>(fResImageLineSize);
		PixelDataSize=fResImageLineSize*fSize.y;
		break;
	case 4:
		fResImageLineSize=(fSize.x+1)/2;
		fResImageLineSize=cnInteger::RoundPositive<4>(fResImageLineSize);
		PixelDataSize=fResImageLineSize*fSize.y;
		break;
	case 8:
		fResImageLineSize=fSize.x;
		fResImageLineSize=cnInteger::RoundPositive<4>(fResImageLineSize);
		PixelDataSize=fResImageLineSize*fSize.y;
		break;
	case 16:
		fResImageLineSize=fSize.x*2;
		fResImageLineSize=cnInteger::RoundPositive<4>(fResImageLineSize);
		PixelDataSize=fResImageLineSize*fSize.y;
		break;
	case 24:
		fResImageLineSize=fSize.x*3;
		fResImageLineSize=cnInteger::RoundPositive<4>(fResImageLineSize);
		break;
	}
	if(ImageDataOffset+PaletteDataSize+PixelDataSize+MaskImageSize>ResourceDataSize){
		// not enough pixel data
		ClearData();
		return false;
	}

	fResPixelData=cnMemory::PointerAddByteOffset(ResourceData,ImageDataOffset+PaletteDataSize);
	fResMaskData=cnMemory::PointerAddByteOffset(ResourceData,ImageDataOffset+PaletteDataSize+PixelDataSize);

	return true;
}
//---------------------------------------------------------------------------
void cBitmapConvert::SetupPixelBuffer_ConvertBMP4(void)
{
	bool LinePixelOdd=0!=(fSize.x&1);
	for(sfInt32n LineIndex=0;LineIndex<fSize.y;LineIndex++){
		auto *SrcPixelLine=cnMemory::CastPointerAddByteOffset<const uInt8>(fResPixelData,LineIndex*fResImageLineSize);
		uInt32 *PixelLine=cnMemory::CastPointerAddByteOffset<uInt32>(fConvertedPixelBuffer->Pointer,LineIndex*fSize.x*4);

		for(sfInt32n PixelIndex=0;PixelIndex<fSize.x;PixelIndex+=2){
			uInt8 PixelBlock=SrcPixelLine[PixelIndex/2];

			uInt8 PixelLo=PixelBlock&0xF;
			uInt8 PixelHi=PixelBlock>>4;

			auto &SrcPixelQuadLo=fResourceData->bmiColors[PixelLo];
			auto &SrcPixelQuadHi=fResourceData->bmiColors[PixelHi];

			PixelLine[PixelIndex]=reinterpret_cast<const uInt32&>(SrcPixelQuadLo);
			PixelLine[PixelIndex+1]=reinterpret_cast<const uInt32&>(SrcPixelQuadHi);
		}
		if(LinePixelOdd){
			auto &SrcPixelQuad=fResourceData->bmiColors[SrcPixelLine[fSize.x/2]];
		
			PixelLine[fSize.x-1]=reinterpret_cast<const uInt32&>(SrcPixelQuad);
		}
	}
}
//---------------------------------------------------------------------------
void cBitmapConvert::SetupPixelBuffer_ConvertBMP8(void)
{
	for(sfInt32n LineIndex=0;LineIndex<fSize.y;LineIndex++){
		auto *SrcPixelLine=cnMemory::CastPointerAddByteOffset<const uInt8>(fResPixelData,LineIndex*fResImageLineSize);
		uInt32 *PixelLine=cnMemory::CastPointerAddByteOffset<uInt32>(fConvertedPixelBuffer->Pointer,LineIndex*fSize.x*4);

		for(sfInt32n PixelIndex=0;PixelIndex<fSize.x;PixelIndex++){
			auto &SrcPixelQuad=fResourceData->bmiColors[SrcPixelLine[PixelIndex]];

			PixelLine[PixelIndex]=reinterpret_cast<const uInt32&>(SrcPixelQuad);
		}
	}
}
//---------------------------------------------------------------------------
void cBitmapConvert::SetupPixelBuffer_ConvertBMP24(void)
{
	for(sfInt32n LineIndex=0;LineIndex<fSize.y;LineIndex++){
		auto *SrcPixelLine=cnMemory::CastPointerAddByteOffset<const uInt8>(fResPixelData,LineIndex*fResImageLineSize);
		uInt32 *PixelLine=cnMemory::CastPointerAddByteOffset<uInt32>(fConvertedPixelBuffer->Pointer,LineIndex*fSize.x*4);

		for(sfInt32n PixelIndex=0;PixelIndex<fSize.x;PixelIndex++){

			union{
				uInt8 PixelChannel[4];
				uInt32 Pixel;
			};
			PixelChannel[0]=SrcPixelLine[0];
			PixelChannel[1]=SrcPixelLine[1];
			PixelChannel[2]=SrcPixelLine[2];
			PixelChannel[3]=0;

			PixelLine[PixelIndex]=Pixel;
		}
	}
}
//---------------------------------------------------------------------------
void cBitmapConvert::SetupPixelBuffer(void)
{
	if(fConvertedPixelBuffer->Length!=0){
		// already loaded
		return;
	}
	
	fConvertedPixelBuffer.SetSize(fSize.x*fSize.y*4);
	switch(fResourceData->bmiHeader.biBitCount){
	//case 1:
	case 4:
		SetupPixelBuffer_ConvertBMP4();
		break;
	case 8:
		SetupPixelBuffer_ConvertBMP8();
		break;
	case 16:
		break;
	case 24:
		SetupPixelBuffer_ConvertBMP24();
		break;
	}
	// mix up mask
	
	for(sfInt32n LineIndex=0;LineIndex<fSize.y;LineIndex++){
		auto *MaskLine=cnMemory::CastPointerAddByteOffset<const uInt8>(fResMaskData,LineIndex*fResMaskLineSize);
		uInt32 *PixelLine=cnMemory::CastPointerAddByteOffset<uInt32>(fConvertedPixelBuffer->Pointer,LineIndex*fSize.x*4);

		for(sfInt32n PixelIndex=0;PixelIndex<fSize.x;PixelIndex++){

			int bitmaskindex=PixelIndex/8;
			int bitindex=PixelIndex%8;
			bool alpha=0!=((1<<(7-bitindex))&MaskLine[bitmaskindex]);

			if(alpha){
				// pixel channel ^ 0xFF

				auto *pChannel=reinterpret_cast<uInt8*>(PixelLine+PixelIndex);

				pChannel[3]=(pChannel[0]+pChannel[1]+pChannel[2])/3;

				PixelLine[PixelIndex]^=0xFFFFFF;
				pChannel[0]=pChannel[0]*pChannel[3]/255;
				pChannel[1]=pChannel[1]*pChannel[3]/255;
				pChannel[2]=pChannel[2]*pChannel[3]/255;
			}
			else{
				// set pixel opaque
				PixelLine[PixelIndex]|=0xFF000000;
			}
		}
	}
}
#endif // 0
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cBitmapDataSourceFromIconResource::cBitmapDataSourceFromIconResource()noexcept
{
	fSize={0,0};
	fPixelFormat=BitmapPixelFormatUnknow;
	fResourceData=nullptr;
	fResMaskData=nullptr;
	fPixelBufferPitch=0;
}
//---------------------------------------------------------------------------
cBitmapDataSourceFromIconResource::~cBitmapDataSourceFromIconResource()noexcept
{
}
//---------------------------------------------------------------------------
void cBitmapDataSourceFromIconResource::ClearData(void)noexcept
{
	fResourceData=nullptr;
	fResMaskData=nullptr;
	fPixelBufferPitch=0;
	fConvertedPixelBuffer.Clear();
	fSize={0,0};
	fPixelFormat=BitmapPixelFormatUnknow;
}
//---------------------------------------------------------------------------
bool cBitmapDataSourceFromIconResource::SetIconData(const void *ResourceData,uIntn ResourceDataSize)noexcept
{
	if(fPixelLockCount!=0)	// in use
		return false;

	fResourceData=static_cast<const BITMAPINFO*>(ResourceData);

	uIntn ImageDataOffset=fResourceData->bmiHeader.biSize;

	fSize.x=fResourceData->bmiHeader.biWidth;
	fTopDown=fResourceData->bmiHeader.biHeight<0;
	if(fTopDown){
		fSize.y=-fResourceData->bmiHeader.biHeight;
	}
	else{
		fSize.y=fResourceData->bmiHeader.biHeight;
	}
	fSize.y/=2;

	// check data size

	if(fResourceData->bmiHeader.biClrUsed==0){
		fPaletteDataSize=static_cast<uIntn>(1)<<fResourceData->bmiHeader.biBitCount;
		fPaletteDataSize*=4;
	}
	else{
		fPaletteDataSize=fResourceData->bmiHeader.biClrUsed*4;
	}
	fResMaskLineSize=(fSize.x+7)/8;

	fResMaskLineSize=cnMath::IntRoundPositive<4>(fResMaskLineSize);
	uIntn MaskImageSize=fResMaskLineSize*fSize.y;

	uIntn ResImageLineSize;
	fPixelFormat.AlphaPremultiplied=true;
	fPixelFormat.ChannelColor=BitmapChannelColor::BGRA;
	fPixelFormat.ChannelCount=4;
	fPixelFormat.ChannelSize=1;
	fPixelFormat.ChannelType=BitmapChannelType::UNorm;
	switch(fResourceData->bmiHeader.biBitCount){
	default:
		// not supported
		ClearData();
		return false;
	case 1:
		ResImageLineSize=(fSize.x+7)/8;
		ResImageLineSize=cnMath::IntRoundPositive<4>(ResImageLineSize);
		break;
	case 4:
		ResImageLineSize=(fSize.x+1)/2;
		ResImageLineSize=cnMath::IntRoundPositive<4>(ResImageLineSize);
		break;
	case 8:
		ResImageLineSize=fSize.x;
		ResImageLineSize=cnMath::IntRoundPositive<4>(ResImageLineSize);
		break;
	case 16:
		ResImageLineSize=fSize.x*2;
		ResImageLineSize=cnMath::IntRoundPositive<4>(ResImageLineSize);
		break;
	case 24:
		ResImageLineSize=fSize.x*3;
		ResImageLineSize=cnMath::IntRoundPositive<4>(ResImageLineSize);
		break;
	}
	uIntn PixelDataSize=ResImageLineSize*fSize.y;
	if(ImageDataOffset+fPaletteDataSize+PixelDataSize+MaskImageSize>ResourceDataSize){
		// not enough pixel data
		ClearData();
		return false;
	}
	
	fResMaskData=cnMemory::PointerAddByteOffset(ResourceData,ImageDataOffset+fPaletteDataSize+PixelDataSize);

	return true;
}
//---------------------------------------------------------------------------
void cBitmapDataSourceFromIconResource::SetupPixelBuffer(void)noexcept
{
	if(fConvertedPixelBuffer->Length!=0){
		// already loaded
		return;
	}
	
	fPixelBufferPitch=fSize.x*4;
	fConvertedPixelBuffer.SetSize(fSize.y*fPixelBufferPitch);

	BITMAPINFOHEADER bi;
	cnMemory::ZeroFill(bi);
	bi.biSize=sizeof(bi);
	bi.biBitCount=32;
	bi.biPlanes=1;
	bi.biWidth=fSize.x;
	bi.biHeight=fSize.y;
	bi.biSizeImage=static_cast<DWORD>(fConvertedPixelBuffer->Length);

	// copy icon "xor" image header, for correcting height parameter

	cMemoryBlock ResBmpInfoCopy;
	ResBmpInfoCopy.SetLength(fResourceData->bmiHeader.biSize+fPaletteDataSize);
	cnMemory::Copy(ResBmpInfoCopy.Pointer,fResourceData,ResBmpInfoCopy.Length);

	auto *BmpInfoCopy=static_cast<BITMAPINFO*>(ResBmpInfoCopy.Pointer);
	BmpInfoCopy->bmiHeader.biHeight/=2;
	
	auto pResPixelData=cnMemory::CastPointerAddByteOffset<void>(fResourceData,ResBmpInfoCopy.Length);

	// create bmp to convert icon "xor" image

	auto Device=cnWindows::QueryDCPaintDevice();

	auto ScreenIC=Device->GetScreenInfomationDC();
	auto DC=ScreenIC->GetDC();

	auto ConvertBmp=::CreateCompatibleBitmap(DC,fSize.x,fSize.y);
	// set and get pixels
	::SetDIBits(DC,ConvertBmp,0,fSize.y,pResPixelData,BmpInfoCopy,DIB_RGB_COLORS);
	::GetDIBits(DC,ConvertBmp,0,fSize.y,fConvertedPixelBuffer->Pointer,reinterpret_cast<BITMAPINFO*>(&bi),DIB_RGB_COLORS);

	::DeleteObject(ConvertBmp);

	// mix up mask
	
	for(sfInt32n LineIndex=0;LineIndex<fSize.y;LineIndex++){
		auto *MaskLine=cnMemory::CastPointerAddByteOffset<const uInt8>(fResMaskData,LineIndex*fResMaskLineSize);
		uInt32 *PixelLine=cnMemory::CastPointerAddByteOffset<uInt32>(fConvertedPixelBuffer->Pointer,LineIndex*fSize.x*4);

		for(sfInt32n PixelIndex=0;PixelIndex<fSize.x;PixelIndex++){

			int bitmaskindex=PixelIndex/8;
			int bitindex=PixelIndex%8;
			bool alpha=0!=((1<<(7-bitindex))&MaskLine[bitmaskindex]);

			if(alpha){
				// pixel channel ^ 0xFF

				auto *pChannel=reinterpret_cast<uInt8*>(PixelLine+PixelIndex);

				pChannel[3]=(pChannel[0]+pChannel[1]+pChannel[2])/3;

				PixelLine[PixelIndex]^=0xFFFFFF;
				pChannel[0]=pChannel[0]*pChannel[3]/255;
				pChannel[1]=pChannel[1]*pChannel[3]/255;
				pChannel[2]=pChannel[2]*pChannel[3]/255;
			}
			else{
				// set pixel opaque
				PixelLine[PixelIndex]|=0xFF000000;
			}
		}
	}
}
//---------------------------------------------------------------------------
cUIPoint cBitmapDataSourceFromIconResource::GetImageSize(void)noexcept
{
	return fSize;
}
//---------------------------------------------------------------------------
cBitmapPixelFormat cBitmapDataSourceFromIconResource::GetPixelFormat(void)noexcept
{
	return fPixelFormat;
}
//---------------------------------------------------------------------------
uIntn cBitmapDataSourceFromIconResource::CopyPixelBuffer(uIntn Offset,void *Dest,uIntn DestSize)noexcept
{
	SetupPixelBuffer();
	if(Offset>=fConvertedPixelBuffer->Length)
		return 0;
	uIntn PixelDataSize=fConvertedPixelBuffer->Length;
	if(Offset+DestSize<PixelDataSize)
		PixelDataSize=DestSize-Offset;
	cnMemory::Copy(Dest,cnMemory::PointerAddByteOffset(fConvertedPixelBuffer->Pointer,Offset),PixelDataSize);
	return PixelDataSize;
}
//---------------------------------------------------------------------------
const void* cBitmapDataSourceFromIconResource::AcquirePixels(void)noexcept
{
	if(fPixelLockCount==0xFFFF){
		return nullptr;
	}
	SetupPixelBuffer();
	fPixelLockCount++;
	return fConvertedPixelBuffer->Pointer;
}
//---------------------------------------------------------------------------
void cBitmapDataSourceFromIconResource::ReleasePixels(void)noexcept
{
	if(fPixelLockCount!=0){
		fPixelLockCount--;
	}
}
//---------------------------------------------------------------------------
uIntn cBitmapDataSourceFromIconResource::GetDataPitch(void)noexcept
{
	SetupPixelBuffer();
	return fPixelBufferPitch;
}
//---------------------------------------------------------------------------
uIntn cBitmapDataSourceFromIconResource::GetDataSize(void)noexcept
{
	SetupPixelBuffer();
	return fConvertedPixelBuffer->Length;
}
//---------------------------------------------------------------------------
bool cBitmapDataSourceFromIconResource::IsTopDown(void)noexcept
{
	return fTopDown;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
iPtr<iBitmapDataSource> cnWinRTL::CreateBitmapDataSourceFromResourceBitmap(HMODULE hModule,LPCWSTR lpName)noexcept
{
	auto ResourceHandle=::FindResourceW(hModule,lpName,RT_BITMAP);
	if(ResourceHandle==nullptr)
		return nullptr;
	HGLOBAL ResourceImageData=::LoadResource(hModule,ResourceHandle);
	auto pImageData=::LockResource(ResourceImageData);
	if(pImageData==nullptr){
		return nullptr;
	}

	auto ImageDataSize=::SizeofResource(hModule,ResourceHandle);
	if(ImageDataSize==0)
		return nullptr;

	auto Header=static_cast<const BITMAPINFOHEADER*>(pImageData);

	cBitmapPixelFormat Format;
	Format.ChannelColor=BitmapChannelColor::BGR;
	Format.ChannelType=BitmapChannelType::UNorm;
	uIntn Pitch;
	switch(Header->biBitCount){
	default:
		return nullptr;
	//case 8:
	//case 16:
	case 24:
		Format.ChannelCount=3;
		Format.ChannelSize=1;
		Pitch=Header->biWidth*3;
		Pitch=cnMath::IntRoundPositive<4>(Pitch);
		break;
	case 32:
		Format.ChannelCount=4;
		Format.ChannelSize=1;
		Pitch=Header->biWidth*4;
		break;
	}

	auto BitmapMemSource=iCreate<cBitmapDataSourceFromMemory>();
	if(BitmapMemSource->SetData(Format,Header->biWidth,Header->biHeight,Pitch,cnMemory::PointerAddByteOffset(pImageData,Header->biSize),Header->biHeight<0)==false)
		return nullptr;
	return BitmapMemSource;
}
//---------------------------------------------------------------------------
iPtr<iBitmapDataSource> cnWinRTL::CreateBitmapDataSourceFromResourceIconItem(HMODULE hModule,LPCWSTR lpName)noexcept
{
	auto ResourceHandle=::FindResourceW(hModule,lpName,RT_ICON);
	if(ResourceHandle==nullptr)
		return nullptr;
	HGLOBAL ResourceImageData=::LoadResource(hModule,ResourceHandle);
	auto pImageData=::LockResource(ResourceImageData);
	if(pImageData==nullptr){
		return nullptr;
	}

	auto ImageDataSize=::SizeofResource(hModule,ResourceHandle);
	if(ImageDataSize==0)
		return nullptr;

	auto Header=static_cast<const BITMAPINFOHEADER*>(pImageData);

	switch(Header->biBitCount){
	default:
		// not supported
		return nullptr;
	case 1:
	case 4:
	case 8:
	case 16:
	case 24:
		{	// convert from icon
			auto BitmapMemSource=iCreate<cBitmapDataSourceFromIconResource>();
			if(BitmapMemSource->SetIconData(pImageData,ImageDataSize)==false)
				return nullptr;
			return BitmapMemSource;
		}
	case 32:
		{	// direct bitmap
			auto BitmapMemSource=iCreate<cBitmapDataSourceFromMemory>();
			
			if(BitmapMemSource->SetData(BitmapPixelFormatR8G8B8A8,Header->biWidth,Header->biHeight/2,Header->biWidth*4,cnMemory::PointerAddByteOffset(pImageData,Header->biSize),Header->biHeight<0)==false)
				return nullptr;
			return BitmapMemSource;
		}
	}

}
//---------------------------------------------------------------------------
iPtr<iBitmapDataSource> cnWinRTL::CreateBitmapDataSourceFromResourceIcon(HMODULE hModule,LPCWSTR lpName,int LookupX,int LookupY,UINT LookupFlag)noexcept
{
	auto ResourceIconGroupHandle=::FindResourceW(hModule,lpName,RT_GROUP_ICON);
	if(ResourceIconGroupHandle==nullptr)
		return nullptr;
	HGLOBAL ResourceIconGroupData=::LoadResource(hModule,ResourceIconGroupHandle);
	auto pIconGroupData=::LockResource(ResourceIconGroupData);
	if(pIconGroupData==nullptr){
		return nullptr;
	}
	int IconID=::LookupIconIdFromDirectoryEx(static_cast<PBYTE>(pIconGroupData),TRUE,LookupX,LookupY,LookupFlag);
	if(IconID==0)
		return nullptr;

	return cnWinRTL::CreateBitmapDataSourceFromResourceIconItem(hModule,MAKEINTRESOURCE(IconID));
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWICBitmapDataSource::cWICBitmapDataSource(IWICBitmap *Bitmap)noexcept
	: fBitmapSource(Bitmap),fBitmap(Bitmap)
{
	WICPixelFormatGUID FormatGUID;
	auto hr=fBitmapSource->GetPixelFormat(&FormatGUID);
	if(FAILED(hr)){
		fPixelFormat=BitmapPixelFormatUnknow;
	}
	else{
		fPixelFormat=GUIDToFormat(FormatGUID);
	}
}
//---------------------------------------------------------------------------
cWICBitmapDataSource::cWICBitmapDataSource(IWICBitmapSource *BitmapSource)noexcept
	: fBitmapSource(BitmapSource)
{
	WICPixelFormatGUID FormatGUID;
	auto hr=fBitmapSource->GetPixelFormat(&FormatGUID);
	if(FAILED(hr)){
		fPixelFormat=BitmapPixelFormatUnknow;
	}
	else{
		fPixelFormat=GUIDToFormat(FormatGUID);
	}
}
//---------------------------------------------------------------------------
cWICBitmapDataSource::~cWICBitmapDataSource()noexcept
{
}
//---------------------------------------------------------------------------
cBitmapPixelFormat cWICBitmapDataSource::GUIDToFormat(const WICPixelFormatGUID &FormatGUID)noexcept
{

	cBitmapPixelFormat Format;
	// 32 bit with alpha

	if(FormatGUID==GUID_WICPixelFormat32bppRGBA){
		Format.ChannelColor=BitmapChannelColor::RGBA;
		Format.ChannelSize=1;
		Format.ChannelCount=4;
		Format.ChannelType=BitmapChannelType::UNorm;
		Format.AlphaPremultiplied=false;
		return Format;
	}
	if(FormatGUID==GUID_WICPixelFormat32bppPRGBA){
		Format.ChannelColor=BitmapChannelColor::RGBA;
		Format.ChannelSize=1;
		Format.ChannelCount=4;
		Format.ChannelType=BitmapChannelType::UNorm;
		Format.AlphaPremultiplied=true;
		return Format;
	}

	if(FormatGUID==GUID_WICPixelFormat32bppPBGRA){
		Format.ChannelColor=BitmapChannelColor::BGRA;
		Format.ChannelSize=1;
		Format.ChannelCount=4;
		Format.ChannelType=BitmapChannelType::UNorm;
		Format.AlphaPremultiplied=true;
		return Format;
	}
	if(FormatGUID==GUID_WICPixelFormat32bppBGRA){
		Format.ChannelColor=BitmapChannelColor::BGRA;
		Format.ChannelSize=1;
		Format.ChannelCount=4;
		Format.ChannelType=BitmapChannelType::UNorm;
		Format.AlphaPremultiplied=false;
		return Format;
	}

	// 32 bit

	if(FormatGUID==GUID_WICPixelFormat32bppBGR){
		Format.ChannelColor=BitmapChannelColor::RGB;
		Format.ChannelSize=1;
		Format.ChannelCount=4;
		Format.ChannelType=BitmapChannelType::UNorm;
		Format.AlphaPremultiplied=false;
		return Format;
	}
	if(FormatGUID==GUID_WICPixelFormat32bppRGB){
		Format.ChannelColor=BitmapChannelColor::RGB;
		Format.ChannelSize=1;
		Format.ChannelCount=4;
		Format.ChannelType=BitmapChannelType::UNorm;
		Format.AlphaPremultiplied=false;
		return Format;
	}

	// 24bit

	if(FormatGUID==GUID_WICPixelFormat24bppRGB){
		Format.ChannelColor=BitmapChannelColor::RGB;
		Format.ChannelSize=1;
		Format.ChannelCount=3;
		Format.ChannelType=BitmapChannelType::UNorm;
		Format.AlphaPremultiplied=false;
		return Format;
	}
	if(FormatGUID==GUID_WICPixelFormat24bppBGR){
		Format.ChannelColor=BitmapChannelColor::BGR;
		Format.ChannelSize=1;
		Format.ChannelCount=3;
		Format.ChannelType=BitmapChannelType::UNorm;
		Format.AlphaPremultiplied=false;
		return Format;
	}

	return BitmapPixelFormatUnknow;
}
//---------------------------------------------------------------------------
void* cWICBitmapDataSource::CastInterface(iTypeID IID)noexcept
{
	return iCOMInterface::CastInterface(IID);
}
//---------------------------------------------------------------------------
IUnknown* cWICBitmapDataSource::GetCOMInterface(void)noexcept
{
	return fBitmapSource;
}
//---------------------------------------------------------------------------
cUIPoint cWICBitmapDataSource::GetImageSize(void)noexcept
{
	cUIPoint Size;
	fBitmapSource->GetSize(reinterpret_cast<UINT*>(&Size.x),reinterpret_cast<UINT*>(&Size.y));
	return Size;
}
//---------------------------------------------------------------------------
uIntn cWICBitmapDataSource::GetDataPitch(void)noexcept
{
	SetupData();
	UINT Stride;
	if(SUCCEEDED(fDataLock->GetStride(&Stride))){
		return Stride;
	}
	return 0;
}
//---------------------------------------------------------------------------
uIntn cWICBitmapDataSource::GetDataSize(void)noexcept
{
	SetupData();
	UINT LockSize;
	WICInProcPointer DataPtr;
	if(SUCCEEDED(fDataLock->GetDataPointer(&LockSize,&DataPtr))){
		return LockSize;
	}
	return 0;
}
//---------------------------------------------------------------------------
cBitmapPixelFormat cWICBitmapDataSource::GetPixelFormat(void)noexcept
{
	return fPixelFormat;
}
//---------------------------------------------------------------------------
uIntn cWICBitmapDataSource::CopyPixelBuffer(uIntn Offset,void *Dest,uIntn DestSize)noexcept
{
#if 0

	auto FormatInfo=BitmapFormatInfo(fFormat);
	if(FormatInfo==nullptr)
		return false;
	WICRect rcCopy;
	rcCopy.X=0;
	rcCopy.Y=0;
	if(FAILED(fBitmapSource->GetSize(reinterpret_cast<UINT*>(&rcCopy.Width),reinterpret_cast<UINT*>(&rcCopy.Height)))){
		return false;
	}

	auto StrideSize=FormatInfo->PixelSize*rcCopy.Width;
	StrideSize=cnInteger::RoundPositive<4>(StrideSize);
	rcCopy.Height=-rcCopy.Height;
	HRESULT hr;
	hr=fBitmapSource->CopyPixels(&rcCopy,StrideSize,DestSize,static_cast<BYTE*>(Dest));
	if(FAILED(hr))
		return false;

	return true;
#else
	auto Pixels=AcquirePixels();
	cnMemory::Copy(Dest,cnMemory::PointerAddByteOffset(Pixels,Offset),DestSize);
	ReleasePixels();
	return true;
#endif // 0
}
//---------------------------------------------------------------------------
bool cWICBitmapDataSource::SetupData(void)noexcept
{
	if(fBitmap==nullptr){
		// create bitmap from source to access pixels
		auto Factory=WICQueryImagingFactory();
		if(Factory==nullptr)
			return false;
		HRESULT hr;
		hr=Factory->CreateBitmapFromSource(fBitmapSource,WICBitmapCacheOnLoad,COMRetPtrT(fBitmap));
		if(FAILED(hr)){
			// cannot open bitmap
			return false;
		}
	}
	// lock bitmap
	if(fDataLock==nullptr){
		WICRect rcLock;
		rcLock.X=0;
		rcLock.Y=0;
		if(FAILED(fBitmap->GetSize(reinterpret_cast<UINT*>(&rcLock.Width),reinterpret_cast<UINT*>(&rcLock.Height)))){
			return false;
		}

		if(FAILED(fBitmap->Lock(&rcLock,WICBitmapLockRead,COMRetPtrT(fDataLock)))){
			return false;
		}
	}
	return true;
}
//---------------------------------------------------------------------------
const void* cWICBitmapDataSource::AcquirePixels(void)noexcept
{
	if(SetupData()==false)
		return nullptr;

	UINT LockSize;
	WICInProcPointer DataPtr;
	if(SUCCEEDED(fDataLock->GetDataPointer(&LockSize,&DataPtr))){
		return DataPtr;
	}
	return nullptr;
}
//---------------------------------------------------------------------------
void cWICBitmapDataSource::ReleasePixels(void)noexcept
{
	fDataLock=nullptr;
}
//---------------------------------------------------------------------------
bool cWICBitmapDataSource::IsTopDown(void)noexcept
{
	return true;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
iPtr<iBitmapDataSource> cnWinRTL::CreateBitmapSourceFromWIC(IWICBitmapSource *BitmapSource)noexcept
{
	return iCreate<cWICBitmapDataSource>(BitmapSource);
}
//---------------------------------------------------------------------------
iPtr<iBitmapDataSource> cnWinRTL::CreateBitmapSourceFromWICBitmap(IWICBitmap *Bitmap)noexcept
{
	return iCreate<cWICBitmapDataSource>(Bitmap);
}
//---------------------------------------------------------------------------
/*
COMPtr<IWICBitmapSource> WICCreateConvertedImage(IWICBitmapSource *BitmapSource)
{
	COMPtr<IWICImagingFactory> pFactory;
	// Create the COM imaging factory
	HRESULT hr = CoCreateInstance(
		CLSID_WICImagingFactory,
		NULL,
		CLSCTX_INPROC_SERVER,
		__uuidof(IWICImagingFactory),
		COMRetPtr(pFactory)
	);

	COMPtr<IWICFormatConverter> ConvertedImage;
	hr=pFactory->CreateFormatConverter(COMRetPtrT(ConvertedImage));
	hr = ConvertedImage->Initialize(
			BitmapSource,                          // Input bitmap to convert
			GUID_WICPixelFormat32bppPBGRA,   // Destination pixel format
			WICBitmapDitherTypeNone,         // Specified dither pattern
			NULL,                            // Specify a particular palette 
			0.f,                             // Alpha threshold
			WICBitmapPaletteTypeCustom       // Palette translation type
			);

	return ConvertedImage;
}
*/
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWICImageFactory::cWICImageFactory()noexcept
	: fImagingFactory(WICQueryImagingFactory())
{
}
//---------------------------------------------------------------------------
cWICImageFactory::~cWICImageFactory()noexcept
{
}
//---------------------------------------------------------------------------
IWICImagingFactory* cWICImageFactory::GetImagingFactory(void)noexcept
{
	return fImagingFactory;
}
//---------------------------------------------------------------------------
COMPtr<IWICBitmapSource> cWICImageFactory::OpenImageFile(const wchar_t *FileName)noexcept
{
	if(fImagingFactory==nullptr)
		return nullptr;

	COMPtr<IWICBitmapDecoder> Decoder;
	HRESULT hr;
	
	hr=fImagingFactory->CreateDecoderFromFilename(FileName,nullptr,GENERIC_READ,WICDecodeMetadataCacheOnDemand,COMRetPtrT(Decoder));

	if(FAILED(hr)){
		return nullptr;
	}
	
	COMPtr<IWICBitmapFrameDecode> FrameDecoder;
	// get first frame
	hr=Decoder->GetFrame(0,COMRetPtrT(FrameDecoder));

	if(FAILED(hr)){
		return nullptr;
	}
	return FrameDecoder;
}
//---------------------------------------------------------------------------
COMPtr<IWICBitmapSource> cWICImageFactory::OpenImageStream(IStream *Stream)noexcept
{
	// create stream for resource
	if(fImagingFactory==nullptr)
		return nullptr;

	HRESULT hr;
	COMPtr<IWICBitmapDecoder> Decoder;
	hr=fImagingFactory->CreateDecoderFromStream(Stream,nullptr,WICDecodeMetadataCacheOnDemand,COMRetPtrT(Decoder));

	if(FAILED(hr)){
		return nullptr;
	}
	
	COMPtr<IWICBitmapFrameDecode> FrameDecoder;
	// get first frame
	hr=Decoder->GetFrame(0,COMRetPtrT(FrameDecoder));

	if(FAILED(hr)){
		return nullptr;
	}
	return FrameDecoder;
}
//---------------------------------------------------------------------------
COMPtr<IWICBitmapSource> cWICImageFactory::OpenImageResource(HMODULE hModule,LPCWSTR lpName,LPCWSTR lpType)noexcept
{
	if(fImagingFactory==nullptr)
		return nullptr;

	auto ResourceHandle=::FindResourceW(hModule,lpName,lpType);
	if(ResourceHandle==nullptr)
		return nullptr;
	HGLOBAL ResourceImageData=::LoadResource(hModule,ResourceHandle);
	auto pImageData=::LockResource(ResourceImageData);
	if(pImageData==nullptr){
		return nullptr;
	}

	auto ImageDataSize=::SizeofResource(hModule,ResourceHandle);
	if(ImageDataSize==0)
		return nullptr;

	// create stream for resource
	HRESULT hr;
	COMPtr<IWICStream> ResStream;
	hr=fImagingFactory->CreateStream(COMRetPtrT(ResStream));
	if(FAILED(hr)){
		return nullptr;
	}

	// Initialize the stream with the resource data
	hr=ResStream->InitializeFromMemory(static_cast<BYTE*>(pImageData),ImageDataSize);
	if(FAILED(hr)){
		return nullptr;
	}

	COMPtr<IWICBitmapDecoder> Decoder;
	hr=fImagingFactory->CreateDecoderFromStream(ResStream,nullptr,WICDecodeMetadataCacheOnDemand,COMRetPtrT(Decoder));

	if(FAILED(hr)){
		return nullptr;
	}
	
	COMPtr<IWICBitmapFrameDecode> FrameDecoder;
	// get first frame
	hr=Decoder->GetFrame(0,COMRetPtrT(FrameDecoder));

	if(FAILED(hr)){
		return nullptr;
	}
	return FrameDecoder;
}
//---------------------------------------------------------------------------
COMPtr<IWICBitmapSource> cWICImageFactory::ImageConvertFormat(IWICBitmapSource *pISource,REFWICPixelFormatGUID dstFormat,
	WICBitmapDitherType dither,IWICPalette *pIPalette,
	double alphaThresholdPercent,WICBitmapPaletteType paletteTranslate)noexcept
{
	if(fImagingFactory==nullptr)
		return nullptr;

	HRESULT hr;

	WICPixelFormatGUID SrcFormat;
	hr=pISource->GetPixelFormat(&SrcFormat);
	if(FAILED(hr))
		return nullptr;

	if(SrcFormat==dstFormat){
		// no need to convert
		return pISource;
	}


	COMPtr<IWICFormatConverter> Converter;
	if(FAILED(fImagingFactory->CreateFormatConverter(COMRetPtrT(Converter)))){
		// cannot create converter
		return nullptr;
	}
	hr=Converter->Initialize(pISource,dstFormat,dither,pIPalette,alphaThresholdPercent,paletteTranslate);
	if(FAILED(hr))
		return nullptr;
	return Converter;
}
//---------------------------------------------------------------------------
COMPtr<IWICBitmapSource> cWICImageFactory::ImageScale(IWICBitmapSource *Source,UINT uiWidth,UINT uiHeight,WICBitmapInterpolationMode mode)noexcept
{
	if(fImagingFactory==nullptr)
		return nullptr;

	HRESULT hr;

	UINT SrcWidth,SrcHeight;
	hr=Source->GetSize(&SrcWidth,&SrcHeight);
	if(FAILED(hr))
		return nullptr;

	if(SrcWidth==uiWidth && SrcHeight==uiHeight){
		// no need to scale
		return Source;
	}

	COMPtr<IWICBitmapScaler> Scaler;
	hr=fImagingFactory->CreateBitmapScaler(COMRetPtrT(Scaler));
	if(FAILED(hr))
		return nullptr;

	hr=Scaler->Initialize(Source,uiWidth,uiHeight,mode);

	if(FAILED(hr))
		return nullptr;

	return Scaler;
}
//---------------------------------------------------------------------------
COMPtr<IWICBitmapSource> cnWinRTL::WICOpenImageFile(const wchar_t *FileName)noexcept
{
	cWICImageFactory Factory;
	return Factory.OpenImageFile(FileName);
}
//---------------------------------------------------------------------------
COMPtr<IWICBitmapSource> cnWinRTL::WICOpenImageStream(IStream *Stream)noexcept
{
	cWICImageFactory Factory;
	return Factory.OpenImageStream(Stream);
}
//---------------------------------------------------------------------------
COMPtr<IWICBitmapSource> cnWinRTL::WICOpenImageResource(HMODULE hModule,LPCWSTR lpName,LPCWSTR lpType)noexcept
{
	cWICImageFactory Factory;
	return Factory.OpenImageResource(hModule,lpName,lpType);
}
//---------------------------------------------------------------------------
COMPtr<IWICBitmapSource> cnWinRTL::WICImageConvertFormat(IWICBitmapSource *Source,REFWICPixelFormatGUID dstFormat,WICBitmapDitherType dither,IWICPalette *pIPalette,double alphaThresholdPercent,WICBitmapPaletteType paletteTranslate)noexcept
{
	cWICImageFactory Factory;
	return Factory.ImageConvertFormat(Source,dstFormat,dither,pIPalette,alphaThresholdPercent,paletteTranslate);
}
//---------------------------------------------------------------------------
COMPtr<IWICBitmapSource> cnWinRTL::WICImageScale(IWICBitmapSource *Source,UINT uiWidth,UINT uiHeight,WICBitmapInterpolationMode mode)noexcept
{
	cWICImageFactory Factory;
	return Factory.ImageScale(Source,uiWidth,uiHeight,mode);
}
//---------------------------------------------------------------------------
