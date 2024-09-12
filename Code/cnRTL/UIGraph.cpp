//---------------------------------------------------------------------------
#include "UIGraph.h"
#include "DataModel.h"

using namespace cnLibrary;
using namespace cnRTL;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void cUIFontInfo::SetFontInfo(cString<uChar16> &&Name,eUIFontStyle FontStyle,Float32 Weight)noexcept
{
	fName=cnVar::MoveCast(Name);
	fFontStyle=FontStyle;
	fWeight=Weight;
}
//---------------------------------------------------------------------------
cArray<const uChar16> cUIFontInfo::GetName(void)noexcept
{
	return fName.GetArray();
}
//---------------------------------------------------------------------------
eUIFontStyle cUIFontInfo::GetStyle(void)noexcept
{
	return fFontStyle;
}
//---------------------------------------------------------------------------
Float32 cUIFontInfo::GetWeight(void)noexcept
{
	return fWeight;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
sfInt8 cSortableUIPoint::Compare(const cUIPoint &Cmp)const noexcept
{
	if(x<Cmp.x)
		return -1;
	if(x==Cmp.x){
		if(y==Cmp.y)
			return 0;
		if(y<Cmp.y)
			return -1;
	}
	// x>Cmp.x or x==Cmp.x&&y>Cmp.y
	return 1;
}
//---------------------------------------------------------------------------
bool cSortableUIPoint::operator ==(const cUIPoint &Src)const noexcept
{
	return x==Src.x && y==Src.y;
}
//---------------------------------------------------------------------------
bool cSortableUIPoint::operator !=(const cUIPoint &Src)const noexcept
{
	return x!=Src.x || y!=Src.y;
}
//---------------------------------------------------------------------------
const cUIPoint cnRTL::UIPointZero={0,0};
const cUIRect cnRTL::UIRectZero={{0,0},{0,0}};
const cUIRectangle cnRTL::UIRectangleZero={0,0,0,0};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cBitmapSourceRGBASampler::cBitmapSourceRGBASampler()noexcept
{
	fSamplingPixelData=nullptr;
}
//---------------------------------------------------------------------------
cBitmapSourceRGBASampler::~cBitmapSourceRGBASampler()noexcept
{
}
//---------------------------------------------------------------------------
void cBitmapSourceRGBASampler::BeginSample(cUIPoint Size)noexcept
{
	if(QuerySource!=nullptr){
		fSamplingBitmap=QuerySource(Size);
		if(fSamplingBitmap==nullptr){
			fSamplingBitmap=Source;
		}
	}
	else{
		fSamplingBitmap=Source;
	}

	if(fSamplingBitmap==nullptr){
		fSampler.ConstructAs<cSampling>();
		return;
	}
	auto SourcePixelFormat=Source->GetPixelFormat();
	//ufInt8 SourceChannelCount=SourcePixelFormat.ChannelCount;
	switch(SourcePixelFormat.ChannelSize){
	default:
		fSamplingBitmap=nullptr;
		fSampler.ConstructAs<cSampling>();
		return;
	case 1:
		SetupSampling8(SourcePixelFormat);
		break;
	case 2:
		SetupSampling16(SourcePixelFormat);
		break;
	case 4:
		SetupSampling32(SourcePixelFormat);
		break;
	}
	fSampler->PixelData=fSamplingBitmap->AcquirePixels();
	if(fSampler->PixelData==nullptr){
		fSamplingBitmap=nullptr;
		fSampler->~cSampling();
		fSampler.ConstructAs<cSampling>();
		return;
	}
	fSampler->Pitch=fSamplingBitmap->GetDataPitch();
	fSampler->ImageSize=fSamplingBitmap->GetImageSize();
}
//---------------------------------------------------------------------------
void cBitmapSourceRGBASampler::EndSample(void)noexcept
{
	if(fSamplingBitmap!=nullptr){
		fSamplingBitmap->ReleasePixels();
		fSamplingPixelData=nullptr;

		fSamplingBitmap=nullptr;
	}
	fSampler->~cSampling();
}
//---------------------------------------------------------------------------
cIntegerVector<uInt8,4> cBitmapSourceRGBASampler::SampleUNorm8(sfInt32 x,sfInt32 y)noexcept
{
	return fSampler->SampleUNorm8(x,y);
}
//---------------------------------------------------------------------------
cIntegerVector<uInt16,4> cBitmapSourceRGBASampler::SampleUNorm16(sfInt32 x,sfInt32 y)noexcept
{
	return fSampler->SampleUNorm16(x,y);
}
//---------------------------------------------------------------------------
cVector<Float32,4> cBitmapSourceRGBASampler::SampleFloat32(sfInt32 x,sfInt32 y)noexcept
{
	return fSampler->SampleFloat32(x,y);
}
//---------------------------------------------------------------------------
void cBitmapSourceRGBASampler::SetupSampling8(const cBitmapPixelFormat &PixelFormat)noexcept
{
	ufInt8 SourceChannelCount=PixelFormat.ChannelCount;
	switch(PixelFormat.ChannelType){
	default:
		fSamplingBitmap=nullptr;
		fSampler.ConstructAs<cSampling>();
		return;

	case BitmapChannelType::UNorm:
		switch(PixelFormat.ChannelColor){
		default:
			fSamplingBitmap=nullptr;
			fSampler.ConstructAs<cSampling>();
			return;
		case BitmapChannelColor::Luminance:
			fSampler.ConstructAs<cSamplingLAUNorm8>();
			fSampler->ReadChannelCount=1;
			break;
		case BitmapChannelColor::Alpha:
			fSampler.ConstructAs<cSamplingAlphaUNorm8>();
			break;
		case BitmapChannelColor::LA:
			fSampler.ConstructAs<cSamplingLAUNorm8>();
			fSampler->ReadChannelCount=cnMath::Max<ufInt8>(2,SourceChannelCount);
			break;
		case BitmapChannelColor::RGB:
			fSampler.ConstructAs<cSamplingRGBAUNorm8>();
			fSampler->ReadChannelCount=cnMath::Max<ufInt8>(3,SourceChannelCount);
			break;
		case BitmapChannelColor::RGBA:
			if(SourceChannelCount>=4){
				fSampler.ConstructAs<cSamplingRGBAUNorm8x4>();
			}
			else{
				fSampler.ConstructAs<cSamplingRGBAUNorm8>();
				fSampler->ReadChannelCount=SourceChannelCount;
			}
			break;
		case BitmapChannelColor::BGR:
			fSampler.ConstructAs<cSamplingBGRAUNorm8>();
			fSampler->ReadChannelCount=cnMath::Max<ufInt8>(3,SourceChannelCount);
			break;
		case BitmapChannelColor::BGRA:
			fSampler.ConstructAs<cSamplingBGRAUNorm8>();
			fSampler->ReadChannelCount=cnMath::Max<ufInt8>(4,SourceChannelCount);
			break;
		}
		break;
	}
	fSampler->ChannelCount=SourceChannelCount;
}
//---------------------------------------------------------------------------
void cBitmapSourceRGBASampler::SetupSampling16(const cBitmapPixelFormat &PixelFormat)noexcept
{
	ufInt8 SourceChannelCount=PixelFormat.ChannelCount;
	switch(PixelFormat.ChannelType){
	default:
		fSamplingBitmap=nullptr;
		fSampler.ConstructAs<cSampling>();
		return;

	case BitmapChannelType::UNorm:
		switch(PixelFormat.ChannelColor){
		default:
			fSamplingBitmap=nullptr;
			fSampler.ConstructAs<cSampling>();
			return;
		case BitmapChannelColor::Luminance:
			fSampler.ConstructAs<cSamplingLAUNorm16>();
			fSampler->ReadChannelCount=1;
			break;
		case BitmapChannelColor::Alpha:
			fSampler.ConstructAs<cSamplingAlphaUNorm16>();
			break;
		case BitmapChannelColor::LA:
			fSampler.ConstructAs<cSamplingLAUNorm16>();
			fSampler->ReadChannelCount=cnMath::Max<ufInt8>(2,SourceChannelCount);
			break;
		case BitmapChannelColor::RGB:
			fSampler.ConstructAs<cSamplingRGBAUNorm16>();
			fSampler->ReadChannelCount=cnMath::Max<ufInt8>(3,SourceChannelCount);
			break;
		case BitmapChannelColor::RGBA:
			if(SourceChannelCount>=4){
				fSampler.ConstructAs<cSamplingRGBAUNorm16x4>();
			}
			else{
				fSampler.ConstructAs<cSamplingRGBAUNorm16>();
				fSampler->ReadChannelCount=SourceChannelCount;
			}
			break;
		case BitmapChannelColor::BGR:
			fSampler.ConstructAs<cSamplingBGRAUNorm16>();
			fSampler->ReadChannelCount=cnMath::Max<ufInt8>(3,SourceChannelCount);
			break;
		case BitmapChannelColor::BGRA:
			fSampler.ConstructAs<cSamplingBGRAUNorm16>();
			fSampler->ReadChannelCount=cnMath::Max<ufInt8>(4,SourceChannelCount);
			break;
		}
		break;
	}
	fSampler->ChannelCount=SourceChannelCount;
}
//---------------------------------------------------------------------------
void cBitmapSourceRGBASampler::SetupSampling32(const cBitmapPixelFormat &PixelFormat)noexcept
{
	ufInt8 SourceChannelCount=PixelFormat.ChannelCount;
	switch(PixelFormat.ChannelType){
	default:
		fSamplingBitmap=nullptr;
		fSampler.ConstructAs<cSampling>();
		return;

	case BitmapChannelType::Float:
		switch(PixelFormat.ChannelColor){
		default:
			fSamplingBitmap=nullptr;
			fSampler.ConstructAs<cSampling>();
			return;
		case BitmapChannelColor::Luminance:
			fSampler.ConstructAs<cSamplingLAFloat32>();
			fSampler->ReadChannelCount=1;
			break;
		case BitmapChannelColor::Alpha:
			fSampler.ConstructAs<cSamplingAlphaFloat32>();
			break;
		case BitmapChannelColor::LA:
			fSampler.ConstructAs<cSamplingLAFloat32>();
			fSampler->ReadChannelCount=cnMath::Max<ufInt8>(2,SourceChannelCount);
			break;
		case BitmapChannelColor::RGB:
			fSampler.ConstructAs<cSamplingRGBAFloat32>();
			fSampler->ReadChannelCount=cnMath::Max<ufInt8>(3,SourceChannelCount);
			break;
		case BitmapChannelColor::RGBA:
			if(SourceChannelCount>=4){
				fSampler.ConstructAs<cSamplingRGBAFloat32x4>();
			}
			else{
				fSampler.ConstructAs<cSamplingRGBAFloat32>();
				fSampler->ReadChannelCount=SourceChannelCount;
			}
			break;
		case BitmapChannelColor::BGR:
			fSampler.ConstructAs<cSamplingBGRAFloat32>();
			fSampler->ReadChannelCount=cnMath::Max<ufInt8>(3,SourceChannelCount);
			break;
		case BitmapChannelColor::BGRA:
			fSampler.ConstructAs<cSamplingBGRAFloat32>();
			fSampler->ReadChannelCount=cnMath::Max<ufInt8>(4,SourceChannelCount);
			break;

		}
		break;
	}
	fSampler->ChannelCount=SourceChannelCount;

}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cBitmapSourceRGBASampler::cSampling::~cSampling()noexcept
{
}
//---------------------------------------------------------------------------
cIntegerVector<uInt8,4> cBitmapSourceRGBASampler::cSampling::SampleUNorm8(sfInt32 x,sfInt32 y)noexcept
{
	return VectorZeroValue;
}
//---------------------------------------------------------------------------
cIntegerVector<uInt16,4> cBitmapSourceRGBASampler::cSampling::SampleUNorm16(sfInt32 x,sfInt32 y)noexcept
{
	return VectorZeroValue;
}
//---------------------------------------------------------------------------
cVector<Float32,4> cBitmapSourceRGBASampler::cSampling::SampleFloat32(sfInt32 x,sfInt32 y)noexcept
{
	return VectorZeroValue;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cIntegerVector<uInt8,4> cBitmapSourceRGBASampler::cSamplingRGBAUNorm8x4::SampleUNorm8(sfInt32 x,sfInt32 y)noexcept
{
	auto RowPixel=cnMemory::CastPointerAddByteOffset<const uInt8>(PixelData,y*Pitch);
	return IntegerVectorMake<4>(RowPixel+x*ChannelCount);
}
//---------------------------------------------------------------------------
cIntegerVector<uInt16,4> cBitmapSourceRGBASampler::cSamplingRGBAUNorm8x4::SampleUNorm16(sfInt32 x,sfInt32 y)noexcept
{
	auto RowPixel=cnMemory::CastPointerAddByteOffset<const uInt8>(PixelData,y*Pitch);
	alignas(cIntegerVector<uInt16,4>) uInt16 RetColor[4];

	auto Pixel=RowPixel+x*ChannelCount;
	RetColor[0]=Pixel[0]<<8;
	RetColor[1]=Pixel[1]<<8;
	RetColor[2]=Pixel[2]<<8;
	RetColor[3]=Pixel[3]<<8;

	return IntegerVectorMake<4>(RetColor);
}
//---------------------------------------------------------------------------
cVector<Float32,4> cBitmapSourceRGBASampler::cSamplingRGBAUNorm8x4::SampleFloat32(sfInt32 x,sfInt32 y)noexcept
{
	auto RowPixel=cnMemory::CastPointerAddByteOffset<const uInt8>(PixelData,y*Pitch);
	cVector<Float32,4> RetColor;
	auto Pixel=RowPixel+x*ChannelCount;
	RetColor[0]=Pixel[0];
	RetColor[1]=Pixel[1];
	RetColor[2]=Pixel[2];
	RetColor[3]=Pixel[3];

	RetColor*=VectorFillValue(1/255.f);
	return RetColor;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cIntegerVector<uInt8,4> cBitmapSourceRGBASampler::cSamplingRGBAUNorm8::SampleUNorm8(sfInt32 x,sfInt32 y)noexcept
{
	auto RowPixel=cnMemory::CastPointerAddByteOffset<const uInt8>(PixelData,y*Pitch);
	cIntegerVector<uInt8,4> RetColor;
	RetColor[0]=0;
	RetColor[1]=0;
	RetColor[2]=0;
	RetColor[3]=255;
	auto Pixel=RowPixel+x*ChannelCount;
	cnMemory::Copy(RetColor,Pixel,sizeof(uInt8)*ReadChannelCount);
	return RetColor;
}
//---------------------------------------------------------------------------
cIntegerVector<uInt16,4> cBitmapSourceRGBASampler::cSamplingRGBAUNorm8::SampleUNorm16(sfInt32 x,sfInt32 y)noexcept
{
	auto RowPixel=cnMemory::CastPointerAddByteOffset<const uInt8>(PixelData,y*Pitch);
	cIntegerVector<uInt16,4> RetColor;
	RetColor[0]=0;
	RetColor[1]=0;
	RetColor[2]=0;
	RetColor[3]=65535;

	auto Pixel=RowPixel+x*ChannelCount;
	for(ufInt8 i=0;i<ReadChannelCount;i++){
		RetColor[i]=Pixel[i]<<8;
	}

	return RetColor;
}
//---------------------------------------------------------------------------
cVector<Float32,4> cBitmapSourceRGBASampler::cSamplingRGBAUNorm8::SampleFloat32(sfInt32 x,sfInt32 y)noexcept
{
	auto RowPixel=cnMemory::CastPointerAddByteOffset<const uInt8>(PixelData,y*Pitch);
	cVector<Float32,4> RetColor;
	RetColor[0]=0.f;
	RetColor[1]=0.f;
	RetColor[2]=0.f;
	RetColor[3]=1.f;

	auto Pixel=RowPixel+x*ChannelCount;
	for(ufInt8 i=0;i<ReadChannelCount;i++){
		RetColor[i]=Pixel[i]/255.f;
	}

	return RetColor;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cIntegerVector<uInt8,4> cBitmapSourceRGBASampler::cSamplingBGRAUNorm8::SampleUNorm8(sfInt32 x,sfInt32 y)noexcept
{
	auto RowPixel=cnMemory::CastPointerAddByteOffset<const uInt8>(PixelData,y*Pitch);
	cIntegerVector<uInt8,4> RetColor;
	RetColor[0]=0;
	RetColor[1]=0;
	RetColor[2]=0;
	RetColor[3]=255;
	auto Pixel=RowPixel+x*ChannelCount;
	for(ufInt8 i=0;i<ReadChannelCount;i++){
		RetColor[i]=Pixel[i];
	}
	auto t=RetColor[0];
	RetColor[0]=RetColor[2];
	RetColor[2]=t;
	return RetColor;
}
//---------------------------------------------------------------------------
cIntegerVector<uInt16,4> cBitmapSourceRGBASampler::cSamplingBGRAUNorm8::SampleUNorm16(sfInt32 x,sfInt32 y)noexcept
{
	auto RowPixel=cnMemory::CastPointerAddByteOffset<const uInt8>(PixelData,y*Pitch);
	cIntegerVector<uInt16,4> RetColor;
	RetColor[0]=0;
	RetColor[1]=0;
	RetColor[2]=0;
	RetColor[3]=65535;

	auto Pixel=RowPixel+x*ChannelCount;
	for(ufInt8 i=0;i<ChannelCount;i++){
		RetColor[i]=Pixel[i]<<8;
	}
	auto t=RetColor[0];
	RetColor[0]=RetColor[2];
	RetColor[2]=t;

	return RetColor;
}
//---------------------------------------------------------------------------
cVector<Float32,4> cBitmapSourceRGBASampler::cSamplingBGRAUNorm8::SampleFloat32(sfInt32 x,sfInt32 y)noexcept
{
	auto RowPixel=cnMemory::CastPointerAddByteOffset<const uInt8>(PixelData,y*Pitch);
	cVector<Float32,4> RetColor;
	RetColor[0]=0.f;
	RetColor[1]=0.f;
	RetColor[2]=0.f;
	RetColor[3]=1.f;

	auto Pixel=RowPixel+x*ChannelCount;
	for(ufInt8 i=0;i<ChannelCount;i++){
		RetColor[i]=Pixel[i]/255.f;
	}
	auto t=RetColor[0];
	RetColor[0]=RetColor[2];
	RetColor[2]=t;

	return RetColor;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cIntegerVector<uInt8,4> cBitmapSourceRGBASampler::cSamplingAlphaUNorm8::SampleUNorm8(sfInt32 x,sfInt32 y)noexcept
{
	auto RowPixel=cnMemory::CastPointerAddByteOffset<const uInt8>(PixelData,y*Pitch);
	cIntegerVector<uInt8,4> RetColor;
	RetColor[0]=0;
	RetColor[1]=0;
	RetColor[2]=0;
	auto Pixel=RowPixel+x*ChannelCount;
	RetColor[3]=*Pixel;
	return RetColor;
}
//---------------------------------------------------------------------------
cIntegerVector<uInt16,4> cBitmapSourceRGBASampler::cSamplingAlphaUNorm8::SampleUNorm16(sfInt32 x,sfInt32 y)noexcept
{
	auto RowPixel=cnMemory::CastPointerAddByteOffset<const uInt8>(PixelData,y*Pitch);
	cIntegerVector<uInt16,4> RetColor;
	RetColor[0]=0;
	RetColor[1]=0;
	RetColor[2]=0;

	auto Pixel=RowPixel+x*ChannelCount;
	RetColor[3]=*Pixel<<8;

	return RetColor;
}
//---------------------------------------------------------------------------
cVector<Float32,4> cBitmapSourceRGBASampler::cSamplingAlphaUNorm8::SampleFloat32(sfInt32 x,sfInt32 y)noexcept
{
	auto RowPixel=cnMemory::CastPointerAddByteOffset<const uInt8>(PixelData,y*Pitch);
	cVector<Float32,4> RetColor;
	RetColor[0]=0;
	RetColor[1]=0;
	RetColor[2]=0;

	auto Pixel=RowPixel+x*ChannelCount;
	RetColor[3]=*Pixel/255.f;

	return RetColor;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cIntegerVector<uInt8,4> cBitmapSourceRGBASampler::cSamplingLAUNorm8::SampleUNorm8(sfInt32 x,sfInt32 y)noexcept
{
	auto RowPixel=cnMemory::CastPointerAddByteOffset<const uInt8>(PixelData,y*Pitch);
	cIntegerVector<uInt8,4> RetColor;
	auto Pixel=RowPixel+x*ChannelCount;
	RetColor[0]=RetColor[1]=RetColor[2]=Pixel[0];
	if(ReadChannelCount>=2){
		RetColor[3]=Pixel[1];
	}
	else{
		RetColor[3]=255;
	}
	return RetColor;
}
//---------------------------------------------------------------------------
cIntegerVector<uInt16,4> cBitmapSourceRGBASampler::cSamplingLAUNorm8::SampleUNorm16(sfInt32 x,sfInt32 y)noexcept
{
	auto RowPixel=cnMemory::CastPointerAddByteOffset<const uInt8>(PixelData,y*Pitch);
	cIntegerVector<uInt16,4> RetColor;

	auto Pixel=RowPixel+x*ChannelCount;
	uInt16 LValue=Pixel[0]<<8;
	RetColor[0]=RetColor[1]=RetColor[2]=LValue;
	if(ReadChannelCount>=2){
		RetColor[3]=Pixel[1]<<8;
	}
	else{
		RetColor[3]=65535;
	}

	return RetColor;
}
//---------------------------------------------------------------------------
cVector<Float32,4> cBitmapSourceRGBASampler::cSamplingLAUNorm8::SampleFloat32(sfInt32 x,sfInt32 y)noexcept
{
	auto RowPixel=cnMemory::CastPointerAddByteOffset<const uInt8>(PixelData,y*Pitch);
	cVector<Float32,4> RetColor;

	auto Pixel=RowPixel+x*ChannelCount;
	RetColor[0]=Pixel[0];
	RetColor[0]/=255.f;
	RetColor[1]=RetColor[2]=RetColor[0];
	if(ReadChannelCount>=2){
		RetColor[3]=Pixel[1]/255.f;
	}
	else{
		RetColor[3]=1.f;
	}

	return RetColor;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cIntegerVector<uInt8,4> cBitmapSourceRGBASampler::cSamplingRGBAUNorm16x4::SampleUNorm8(sfInt32 x,sfInt32 y)noexcept
{
	auto RowPixel=cnMemory::CastPointerAddByteOffset<const uInt16>(PixelData,y*Pitch);
	cIntegerVector<uInt8,4> RetColor;

	auto Pixel=RowPixel+x*ChannelCount;
	RetColor[0]=Pixel[0]>>8;
	RetColor[1]=Pixel[1]>>8;
	RetColor[2]=Pixel[2]>>8;
	RetColor[3]=Pixel[3]>>8;

	return RetColor;
}
//---------------------------------------------------------------------------
cIntegerVector<uInt16,4> cBitmapSourceRGBASampler::cSamplingRGBAUNorm16x4::SampleUNorm16(sfInt32 x,sfInt32 y)noexcept
{
	auto RowPixel=cnMemory::CastPointerAddByteOffset<const uInt16>(PixelData,y*Pitch);
	return IntegerVectorMake<4>(RowPixel+x*ChannelCount);
}
//---------------------------------------------------------------------------
cVector<Float32,4> cBitmapSourceRGBASampler::cSamplingRGBAUNorm16x4::SampleFloat32(sfInt32 x,sfInt32 y)noexcept
{
	auto RowPixel=cnMemory::CastPointerAddByteOffset<const uInt16>(PixelData,y*Pitch);
	cVector<Float32,4> RetColor;

	auto Pixel=RowPixel+x*ChannelCount;
	RetColor[0]=Pixel[0];
	RetColor[1]=Pixel[1];
	RetColor[2]=Pixel[2];
	RetColor[3]=Pixel[3];

	RetColor*=VectorFillValue(1/65535.f);
	return RetColor;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cIntegerVector<uInt8,4> cBitmapSourceRGBASampler::cSamplingRGBAUNorm16::SampleUNorm8(sfInt32 x,sfInt32 y)noexcept
{
	auto RowPixel=cnMemory::CastPointerAddByteOffset<const uInt16>(PixelData,y*Pitch);
	cIntegerVector<uInt8,4> RetColor;
	RetColor[0]=0;
	RetColor[1]=0;
	RetColor[2]=0;
	RetColor[3]=255;
	auto Pixel=RowPixel+x*ChannelCount;
	for(ufInt8 i=0;i<ReadChannelCount;i++){
		RetColor[i]=Pixel[i]>>8;
	}
	return RetColor;
}
//---------------------------------------------------------------------------
cIntegerVector<uInt16,4> cBitmapSourceRGBASampler::cSamplingRGBAUNorm16::SampleUNorm16(sfInt32 x,sfInt32 y)noexcept
{
	auto RowPixel=cnMemory::CastPointerAddByteOffset<const uInt16>(PixelData,y*Pitch);
	cIntegerVector<uInt16,4> RetColor;
	RetColor[0]=0;
	RetColor[1]=0;
	RetColor[2]=0;
	RetColor[3]=65535;

	auto Pixel=RowPixel+x*ChannelCount;
	cnMemory::Copy(RetColor,Pixel,sizeof(uInt16)*ReadChannelCount);

	return RetColor;
}
//---------------------------------------------------------------------------
cVector<Float32,4> cBitmapSourceRGBASampler::cSamplingRGBAUNorm16::SampleFloat32(sfInt32 x,sfInt32 y)noexcept
{
	auto RowPixel=cnMemory::CastPointerAddByteOffset<const uInt16>(PixelData,y*Pitch);
	cVector<Float32,4> RetColor;
	RetColor[0]=0.f;
	RetColor[1]=0.f;
	RetColor[2]=0.f;
	RetColor[3]=1.f;

	auto Pixel=RowPixel+x*ChannelCount;
	for(ufInt8 i=0;i<ReadChannelCount;i++){
		RetColor[i]=Pixel[i]/65535.f;
	}

	return RetColor;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cIntegerVector<uInt8,4> cBitmapSourceRGBASampler::cSamplingBGRAUNorm16::SampleUNorm8(sfInt32 x,sfInt32 y)noexcept
{
	auto RowPixel=cnMemory::CastPointerAddByteOffset<const uInt16>(PixelData,y*Pitch);
	cIntegerVector<uInt8,4> RetColor;
	RetColor[0]=0;
	RetColor[1]=0;
	RetColor[2]=0;
	RetColor[3]=255;
	auto Pixel=RowPixel+x*ChannelCount;
	for(ufInt8 i=0;i<ReadChannelCount;i++){
		RetColor[i]=Pixel[i]>>8;
	}
	auto t=RetColor[0];
	RetColor[0]=RetColor[2];
	RetColor[2]=t;
	return RetColor;
}
//---------------------------------------------------------------------------
cIntegerVector<uInt16,4> cBitmapSourceRGBASampler::cSamplingBGRAUNorm16::SampleUNorm16(sfInt32 x,sfInt32 y)noexcept
{
	auto RowPixel=cnMemory::CastPointerAddByteOffset<const uInt16>(PixelData,y*Pitch);
	cIntegerVector<uInt16,4> RetColor;
	RetColor[0]=0;
	RetColor[1]=0;
	RetColor[2]=0;
	RetColor[3]=65535;

	auto Pixel=RowPixel+x*ChannelCount;
	for(ufInt8 i=0;i<ChannelCount;i++){
		RetColor[i]=Pixel[i];
	}
	auto t=RetColor[0];
	RetColor[0]=RetColor[2];
	RetColor[2]=t;

	return RetColor;
}
//---------------------------------------------------------------------------
cVector<Float32,4> cBitmapSourceRGBASampler::cSamplingBGRAUNorm16::SampleFloat32(sfInt32 x,sfInt32 y)noexcept
{
	auto RowPixel=cnMemory::CastPointerAddByteOffset<const uInt16>(PixelData,y*Pitch);
	cVector<Float32,4> RetColor;
	RetColor[0]=0.f;
	RetColor[1]=0.f;
	RetColor[2]=0.f;
	RetColor[3]=1.f;

	auto Pixel=RowPixel+x*ChannelCount;
	for(ufInt8 i=0;i<ChannelCount;i++){
		RetColor[i]=Pixel[i]/65535.f;
	}
	auto t=RetColor[0];
	RetColor[0]=RetColor[2];
	RetColor[2]=t;

	return RetColor;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cIntegerVector<uInt8,4> cBitmapSourceRGBASampler::cSamplingAlphaUNorm16::SampleUNorm8(sfInt32 x,sfInt32 y)noexcept
{
	auto RowPixel=cnMemory::CastPointerAddByteOffset<const uInt16>(PixelData,y*Pitch);
	cIntegerVector<uInt8,4> RetColor;
	RetColor[0]=0;
	RetColor[1]=0;
	RetColor[2]=0;
	auto Pixel=RowPixel+x*ChannelCount;
	RetColor[3]=*Pixel>>8;
	return RetColor;
}
//---------------------------------------------------------------------------
cIntegerVector<uInt16,4> cBitmapSourceRGBASampler::cSamplingAlphaUNorm16::SampleUNorm16(sfInt32 x,sfInt32 y)noexcept
{
	auto RowPixel=cnMemory::CastPointerAddByteOffset<const uInt16>(PixelData,y*Pitch);
	cIntegerVector<uInt16,4> RetColor;
	RetColor[0]=0;
	RetColor[1]=0;
	RetColor[2]=0;

	auto Pixel=RowPixel+x*ChannelCount;
	RetColor[3]=*Pixel;

	return RetColor;
}
//---------------------------------------------------------------------------
cVector<Float32,4> cBitmapSourceRGBASampler::cSamplingAlphaUNorm16::SampleFloat32(sfInt32 x,sfInt32 y)noexcept
{
	auto RowPixel=cnMemory::CastPointerAddByteOffset<const uInt16>(PixelData,y*Pitch);
	cVector<Float32,4> RetColor;
	RetColor[0]=0.f;
	RetColor[1]=0.f;
	RetColor[2]=0.f;

	auto Pixel=RowPixel+x*ChannelCount;
	RetColor[3]=*Pixel/65535.f;

	return RetColor;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cIntegerVector<uInt8,4> cBitmapSourceRGBASampler::cSamplingLAUNorm16::SampleUNorm8(sfInt32 x,sfInt32 y)noexcept
{
	auto RowPixel=cnMemory::CastPointerAddByteOffset<const uInt16>(PixelData,y*Pitch);
	cIntegerVector<uInt8,4> RetColor;
	auto Pixel=RowPixel+x*ChannelCount;
	RetColor[0]=RetColor[1]=RetColor[2]=Pixel[0]>>8;
	if(ReadChannelCount>=2){
		RetColor[3]=Pixel[1]>>8;
	}
	else{
		RetColor[3]=255;
	}
	return RetColor;
}
//---------------------------------------------------------------------------
cIntegerVector<uInt16,4> cBitmapSourceRGBASampler::cSamplingLAUNorm16::SampleUNorm16(sfInt32 x,sfInt32 y)noexcept
{
	auto RowPixel=cnMemory::CastPointerAddByteOffset<const uInt16>(PixelData,y*Pitch);
	cIntegerVector<uInt16,4> RetColor;
	RetColor[0]=0;
	RetColor[1]=0;
	RetColor[2]=0;

	auto Pixel=RowPixel+x*ChannelCount;
	RetColor[0]=RetColor[1]=RetColor[2]=Pixel[0];
	if(ReadChannelCount>=2){
		RetColor[3]=Pixel[1];
	}
	else{
		RetColor[3]=65535;
	}

	return RetColor;
}
//---------------------------------------------------------------------------
cVector<Float32,4> cBitmapSourceRGBASampler::cSamplingLAUNorm16::SampleFloat32(sfInt32 x,sfInt32 y)noexcept
{
	auto RowPixel=cnMemory::CastPointerAddByteOffset<const uInt8>(PixelData,y*Pitch);
	cVector<Float32,4> RetColor;

	auto Pixel=RowPixel+x*ChannelCount;
	RetColor[0]=Pixel[0]/65535.f;

	RetColor[1]=RetColor[2]=RetColor[0];
	if(ReadChannelCount>=2){
		RetColor[3]=Pixel[1]/65535.f;
	}
	else{
		RetColor[3]=1.f;
	}

	return RetColor;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cIntegerVector<uInt8,4> cBitmapSourceRGBASampler::cSamplingRGBAFloat32x4::SampleUNorm8(sfInt32 x,sfInt32 y)noexcept
{
	auto RowPixel=cnMemory::CastPointerAddByteOffset<const Float32>(PixelData,y*Pitch);
	cIntegerVector<uInt8,4> RetColor;

	auto Pixel=RowPixel+x*ChannelCount;
	RetColor[0]=static_cast<uInt8>(Pixel[0]*255.f);
	RetColor[1]=static_cast<uInt8>(Pixel[1]*255.f);
	RetColor[2]=static_cast<uInt8>(Pixel[2]*255.f);
	RetColor[3]=static_cast<uInt8>(Pixel[3]*255.f);

	return RetColor;
}
//---------------------------------------------------------------------------
cIntegerVector<uInt16,4> cBitmapSourceRGBASampler::cSamplingRGBAFloat32x4::SampleUNorm16(sfInt32 x,sfInt32 y)noexcept
{
	auto RowPixel=cnMemory::CastPointerAddByteOffset<const Float32>(PixelData,y*Pitch);
	cIntegerVector<uInt16,4> RetColor;

	auto Pixel=RowPixel+x*ChannelCount;
	RetColor[0]=static_cast<uInt16>(Pixel[0]*65535.f);
	RetColor[1]=static_cast<uInt16>(Pixel[1]*65535.f);
	RetColor[2]=static_cast<uInt16>(Pixel[2]*65535.f);
	RetColor[3]=static_cast<uInt16>(Pixel[3]*65535.f);

	return RetColor;
}
//---------------------------------------------------------------------------
cVector<Float32,4> cBitmapSourceRGBASampler::cSamplingRGBAFloat32x4::SampleFloat32(sfInt32 x,sfInt32 y)noexcept
{
	auto RowPixel=cnMemory::CastPointerAddByteOffset<const Float32>(PixelData,y*Pitch);
	return VectorMake<4>(RowPixel+x*ChannelCount);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cIntegerVector<uInt8,4> cBitmapSourceRGBASampler::cSamplingRGBAFloat32::SampleUNorm8(sfInt32 x,sfInt32 y)noexcept
{
	auto RowPixel=cnMemory::CastPointerAddByteOffset<const Float32>(PixelData,y*Pitch);
	cIntegerVector<uInt8,4> RetColor;
	RetColor[0]=0;
	RetColor[1]=0;
	RetColor[2]=0;
	RetColor[3]=255;
	auto Pixel=RowPixel+x*ChannelCount;
	for(ufInt8 i=0;i<ReadChannelCount;i++){
		RetColor[i]=static_cast<uInt8>(Pixel[i]*255.f);
	}
	return RetColor;
}
//---------------------------------------------------------------------------
cIntegerVector<uInt16,4> cBitmapSourceRGBASampler::cSamplingRGBAFloat32::SampleUNorm16(sfInt32 x,sfInt32 y)noexcept
{
	auto RowPixel=cnMemory::CastPointerAddByteOffset<const Float32>(PixelData,y*Pitch);
	cIntegerVector<uInt16,4> RetColor;
	RetColor[0]=0;
	RetColor[1]=0;
	RetColor[2]=0;
	RetColor[3]=65535;

	auto Pixel=RowPixel+x*ChannelCount;
	for(ufInt8 i=0;i<ReadChannelCount;i++){
		RetColor[i]=static_cast<uInt16>(Pixel[i]*65535.f);
	}

	return RetColor;
}
//---------------------------------------------------------------------------
cVector<Float32,4> cBitmapSourceRGBASampler::cSamplingRGBAFloat32::SampleFloat32(sfInt32 x,sfInt32 y)noexcept
{
	auto RowPixel=cnMemory::CastPointerAddByteOffset<const Float32>(PixelData,y*Pitch);
	cVector<Float32,4> RetColor;
	RetColor[0]=0.f;
	RetColor[1]=0.f;
	RetColor[2]=0.f;
	RetColor[3]=1.f;

	auto Pixel=RowPixel+x*ChannelCount;
	cnMemory::Copy(RetColor,Pixel,sizeof(Float32)*ReadChannelCount);

	return RetColor;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cIntegerVector<uInt8,4> cBitmapSourceRGBASampler::cSamplingBGRAFloat32::SampleUNorm8(sfInt32 x,sfInt32 y)noexcept
{
	auto RowPixel=cnMemory::CastPointerAddByteOffset<const Float32>(PixelData,y*Pitch);
	cIntegerVector<uInt8,4> RetColor;
	auto Pixel=RowPixel+x*ChannelCount;
	for(ufInt8 i=0;i<ReadChannelCount;i++){
		RetColor[i]=static_cast<uInt8>(Pixel[i]*255.f);
	}
	auto t=RetColor[0];
	RetColor[0]=RetColor[2];
	RetColor[2]=t;
	return RetColor;
}
//---------------------------------------------------------------------------
cIntegerVector<uInt16,4> cBitmapSourceRGBASampler::cSamplingBGRAFloat32::SampleUNorm16(sfInt32 x,sfInt32 y)noexcept
{
	auto RowPixel=cnMemory::CastPointerAddByteOffset<const Float32>(PixelData,y*Pitch);
	cIntegerVector<uInt16,4> RetColor;
	RetColor[0]=0;
	RetColor[1]=0;
	RetColor[2]=0;
	RetColor[3]=65535;

	auto Pixel=RowPixel+x*ChannelCount;
	for(ufInt8 i=0;i<ChannelCount;i++){
		RetColor[i]=static_cast<uInt16>(Pixel[i]*65535.f);
	}
	auto t=RetColor[0];
	RetColor[0]=RetColor[2];
	RetColor[2]=t;

	return RetColor;
}
//---------------------------------------------------------------------------
cVector<Float32,4> cBitmapSourceRGBASampler::cSamplingBGRAFloat32::SampleFloat32(sfInt32 x,sfInt32 y)noexcept
{
	auto RowPixel=cnMemory::CastPointerAddByteOffset<const Float32>(PixelData,y*Pitch);
	cVector<Float32,4> RetColor;
	RetColor[0]=0.f;
	RetColor[1]=0.f;
	RetColor[2]=0.f;
	RetColor[3]=1.f;

	auto Pixel=RowPixel+x*ChannelCount;
	for(ufInt8 i=0;i<ChannelCount;i++){
		RetColor[i]=Pixel[i];
	}
	auto t=RetColor[0];
	RetColor[0]=RetColor[2];
	RetColor[2]=t;

	return RetColor;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cIntegerVector<uInt8,4> cBitmapSourceRGBASampler::cSamplingAlphaFloat32::SampleUNorm8(sfInt32 x,sfInt32 y)noexcept
{
	auto RowPixel=cnMemory::CastPointerAddByteOffset<const Float32>(PixelData,y*Pitch);
	cIntegerVector<uInt8,4> RetColor;
	auto Pixel=RowPixel+x*ChannelCount;
	RetColor[3]=static_cast<uInt8>(*Pixel*255.f);
	return RetColor;
}
//---------------------------------------------------------------------------
cIntegerVector<uInt16,4> cBitmapSourceRGBASampler::cSamplingAlphaFloat32::SampleUNorm16(sfInt32 x,sfInt32 y)noexcept
{
	auto RowPixel=cnMemory::CastPointerAddByteOffset<const Float32>(PixelData,y*Pitch);
	cIntegerVector<uInt16,4> RetColor;
	RetColor[0]=0;
	RetColor[1]=0;
	RetColor[2]=0;

	auto Pixel=RowPixel+x*ChannelCount;
	RetColor[3]=static_cast<uInt16>(*Pixel*65535.f);

	return RetColor;
}
//---------------------------------------------------------------------------
cVector<Float32,4> cBitmapSourceRGBASampler::cSamplingAlphaFloat32::SampleFloat32(sfInt32 x,sfInt32 y)noexcept
{
	auto RowPixel=cnMemory::CastPointerAddByteOffset<const Float32>(PixelData,y*Pitch);
	cVector<Float32,4> RetColor;
	RetColor[0]=0.f;
	RetColor[1]=0.f;
	RetColor[2]=0.f;

	auto Pixel=RowPixel+x*ChannelCount;
	RetColor[3]=*Pixel;

	return RetColor;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cIntegerVector<uInt8,4> cBitmapSourceRGBASampler::cSamplingLAFloat32::SampleUNorm8(sfInt32 x,sfInt32 y)noexcept
{
	auto RowPixel=cnMemory::CastPointerAddByteOffset<const Float32>(PixelData,y*Pitch);
	cIntegerVector<uInt8,4> RetColor;
	auto Pixel=RowPixel+x*ChannelCount;
	RetColor[0]=RetColor[1]=RetColor[2]=static_cast<uInt8>(Pixel[0]*255.f);
	if(ReadChannelCount>=2){
		RetColor[3]=static_cast<uInt8>(Pixel[1]*255.f);
	}
	else{
		RetColor[3]=255;
	}
	return RetColor;
}
//---------------------------------------------------------------------------
cIntegerVector<uInt16,4> cBitmapSourceRGBASampler::cSamplingLAFloat32::SampleUNorm16(sfInt32 x,sfInt32 y)noexcept
{
	auto RowPixel=cnMemory::CastPointerAddByteOffset<const Float32>(PixelData,y*Pitch);
	cIntegerVector<uInt16,4> RetColor;

	auto Pixel=RowPixel+x*ChannelCount;
	uInt16 LValue=static_cast<uInt16>(Pixel[0]*65535.f);
	RetColor[0]=RetColor[1]=RetColor[2]=LValue;
	if(ReadChannelCount>=2){
		RetColor[3]=static_cast<uInt16>(Pixel[1]*65535.f);
	}
	else{
		RetColor[3]=65535;
	}

	return RetColor;
}
//---------------------------------------------------------------------------
cVector<Float32,4> cBitmapSourceRGBASampler::cSamplingLAFloat32::SampleFloat32(sfInt32 x,sfInt32 y)noexcept
{
	auto RowPixel=cnMemory::CastPointerAddByteOffset<const Float32>(PixelData,y*Pitch);
	cVector<Float32,4> RetColor;

	auto Pixel=RowPixel+x*ChannelCount;
	RetColor[0]=RetColor[1]=RetColor[2]=Pixel[0];
	if(ReadChannelCount>=2){
		RetColor[3]=Pixel[1];
	}
	else{
		RetColor[3]=1.f;
	}

	return RetColor;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/*
bool cBitmapRGBAResampleSource::SetBlendProcess(iBitmapDataSource *Source,eImageBlendingOperator Operator,uInt32 OpColor,uInt32 AddColor)
{
	if(Source==nullptr)
		return false;

	if(fPixelLockCount!=0)	// in use
		return false;

	auto Format=Source->GetPixelFormat();
	auto FormatInfo=BitmapPixelFormatInfo(Format);
	if(FormatInfo==nullptr){
		ClearData();
		return false;
	}
	switch(Format){
	default:
		// unsupported format
		return false;
	case BitmapPixelFormat::R8G8B8X8:
	case BitmapPixelFormat::R8G8B8A8:
	case BitmapPixelFormat::R8G8B8PA8:
	case BitmapPixelFormat::B8G8R8X8:
	case BitmapPixelFormat::B8G8R8A8:
	case BitmapPixelFormat::B8G8R8PA8:
		break;
	}

	fSource=Source;

	fSize=fSource->GetImageSize();
	fPitch=fSource->GetDataPitch();

	fPixelSize=FormatInfo->PixelSize;

	fOperator=Operator;
	fOpConstant=OpColor;
	fAddConstant=AddColor;
	return true;
}
*/

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void cBitmapRGBAFloatRender::BeginSample(cUIPoint Size)noexcept
{
	SourceSampler->BeginSample(Size);
}
//---------------------------------------------------------------------------
void cBitmapRGBAFloatRender::EndSample(void)noexcept
{
	SourceSampler->EndSample();
}
//---------------------------------------------------------------------------
cIntegerVector<uInt8,4> cBitmapRGBAFloatRender::SampleUNorm8(sfInt32 x,sfInt32 y)noexcept
{
	auto Pixel=Sample(x,y);
	Pixel*=VectorFillValue(255.f);

	cIntegerVector<uInt8,4> RetValue;
	RetValue[0]=static_cast<uInt8>(Pixel[0]);
	RetValue[1]=static_cast<uInt8>(Pixel[1]);
	RetValue[2]=static_cast<uInt8>(Pixel[2]);
	RetValue[3]=static_cast<uInt8>(Pixel[3]);

	return RetValue;
}
//---------------------------------------------------------------------------
cIntegerVector<uInt16,4> cBitmapRGBAFloatRender::SampleUNorm16(sfInt32 x,sfInt32 y)noexcept
{
	auto Pixel=Sample(x,y);
	Pixel*=VectorFillValue(65535.f);

	cIntegerVector<uInt16,4> RetValue;
	RetValue[0]=static_cast<uInt16>(Pixel[0]);
	RetValue[1]=static_cast<uInt16>(Pixel[1]);
	RetValue[2]=static_cast<uInt16>(Pixel[2]);
	RetValue[3]=static_cast<uInt16>(Pixel[3]);

	return RetValue;
}
//---------------------------------------------------------------------------
cVector<Float32,4> cBitmapRGBAFloatRender::SampleFloat32(sfInt32 x,sfInt32 y)noexcept
{
	return Sample(x,y);
}
//---------------------------------------------------------------------------
cVector<Float32,4> cBitmapRGBAFloatRender::Sample(sfInt32 x,sfInt32 y)noexcept
{
	auto SourcePixel=SourceSampler->SampleFloat32(x,y);

	for(auto &Func : PixelProcessors){
		SourcePixel=Func(SourcePixel);
	}
	return SourcePixel;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
static ufInt8 CalcAdd255(ufInt8 v1,ufInt8 v2)noexcept
{
	// dest += constant
	if(static_cast<sInt8>(v1)<0 && static_cast<sInt8>(v2)<0){
		// overflow
		return 0xFF;
	}
	else{
		return v1+v2;
	}
}
//---------------------------------------------------------------------------
static ufInt8 CalcMulDiv255(ufInt32 v1,ufInt32 v2)noexcept
{
	ufInt32 t=v1*v2;
	t=(t<<8)+t+0x100;
	return static_cast<ufInt8>(t>>16);
}
/*

//---------------------------------------------------------------------------
void cBitmapRGBAResampleSource::ProcessSource(void)
{
	if(fUpdate==false)
		return;
	fUpdate=false;

	if(SourceSampler==nullptr){
		fPitch=0;
		fPixelBuffer.SetLength(0);
		return;
	}

	auto Format=SourceSampler->GetPixelFormat();

	fPixelBuffer.SetLength(fSource->GetDataSize());
	bool LoadSource=true;
	if(fOperator==ImageBlendingOperator::Copy){
		LoadSource=true;
	}
	if(LoadSource){
		if(fSource->CopyPixelBuffer(0,fPixelBuffer.Pointer,fPixelBuffer.Length)==false){
			ClearData();
			return;
		}
	}
	else{
		cnMemory::ZeroFill(fPixelBuffer.Pointer,fPixelBuffer.Length);
	}

	switch(fOperator){
	default:
		break;
	case ImageBlendingOperator::Alpha:
		BlendFunction_Alpha();
		break;
	case ImageBlendingOperator::Copy:
		BlendFunction_Copy();
		break;
	case ImageBlendingOperator::And:
		BlendFunction_Alpha();
		break;
	case ImageBlendingOperator::Or:
		BlendFunction_Or();
		break;
	case ImageBlendingOperator::Xor:
		BlendFunction_Xor();
		break;
	case ImageBlendingOperator::Add:
		BlendFunction_Add();
		break;
	case ImageBlendingOperator::Mul:
		BlendFunction_Mul();
		break;
	}
	if(fAddConstant!=0){
		BlendFunction_Add(fAddConstant);
	}
}

void cBitmapRGBAResampleSource::BlendFunction_Add(uInt32 Constant)
{
	auto *pSrcChannel=reinterpret_cast<uInt8*>(&Constant);
	bool pSrcChannelSign[4];
	for(ufInt8 i=0;i<4;i++){
		pSrcChannelSign[i]=static_cast<sInt8>(pSrcChannel[i])<0;
	}
	uIntn PixelCount=fSize.x*fSize.y;
	uInt32 *DestPixels=static_cast<uInt32*>(fPixelBuffer.Pointer);
	for(uIntn PixelIndex=0;PixelIndex<PixelCount;PixelIndex++){

		auto *pDestChannel=reinterpret_cast<uInt8*>(DestPixels+PixelIndex);

		for(ufInt8 i=0;i<4;i++){
			// dest += constant
			if(static_cast<sInt8>(pDestChannel[i])<0 && pSrcChannelSign[i]){
				// overflow
				pDestChannel[i]=0xFF;
			}
			else{
				pDestChannel[i]+=pSrcChannel[i];
			}
		}
	}

}
//---------------------------------------------------------------------------
void cBitmapRGBAResampleSource::BlendFunction_Copy(void)
{
	for(sfInt32 LineIndex=0;LineIndex<fSize.y;LineIndex++){
		uInt32 *PixelLine=cnMemory::CastPointerAddByteOffset<uInt32>(fPixelBuffer.Pointer,LineIndex*fSize.x*4);

		for(sfInt32 PixelIndex=0;PixelIndex<fSize.x;PixelIndex++){

			PixelLine[PixelIndex]=fOpConstant;
		}
	}
}
//---------------------------------------------------------------------------
void cBitmapRGBAResampleSource::BlendFunction_And(void)
{
	for(sfInt32 LineIndex=0;LineIndex<fSize.y;LineIndex++){
		uInt32 *PixelLine=cnMemory::CastPointerAddByteOffset<uInt32>(fPixelBuffer.Pointer,LineIndex*fSize.x*4);

		for(sfInt32 PixelIndex=0;PixelIndex<fSize.x;PixelIndex++){

			PixelLine[PixelIndex]&=fOpConstant;
		}
	}
}
//---------------------------------------------------------------------------
void cBitmapRGBAResampleSource::BlendFunction_Or(void)
{
	for(sfInt32 LineIndex=0;LineIndex<fSize.y;LineIndex++){
		uInt32 *PixelLine=cnMemory::CastPointerAddByteOffset<uInt32>(fPixelBuffer.Pointer,LineIndex*fSize.x*4);

		for(sfInt32 PixelIndex=0;PixelIndex<fSize.x;PixelIndex++){

			PixelLine[PixelIndex]|=fOpConstant;
		}
	}
}
//---------------------------------------------------------------------------
void cBitmapRGBAResampleSource::BlendFunction_Xor(void)
{
	for(sfInt32 LineIndex=0;LineIndex<fSize.y;LineIndex++){
		uInt32 *PixelLine=cnMemory::CastPointerAddByteOffset<uInt32>(fPixelBuffer.Pointer,LineIndex*fSize.x*4);

		for(sfInt32 PixelIndex=0;PixelIndex<fSize.x;PixelIndex++){

			PixelLine[PixelIndex]^=fOpConstant;
		}
	}
}
//---------------------------------------------------------------------------
void cBitmapRGBAResampleSource::BlendFunction_Add(void)
{
	return BlendFunction_Add(fOpConstant);
}
//---------------------------------------------------------------------------
void cBitmapRGBAResampleSource::BlendFunction_Mul(void)
{
	auto pConstChannel=reinterpret_cast<uInt8*>(&fOpConstant);
	for(sfInt32 LineIndex=0;LineIndex<fSize.y;LineIndex++){
		uInt32 *PixelLine=cnMemory::CastPointerAddByteOffset<uInt32>(fPixelBuffer.Pointer,LineIndex*fSize.x*4);

		for(sfInt32 PixelIndex=0;PixelIndex<fSize.x;PixelIndex++){

			auto *pChannel=reinterpret_cast<uInt8*>(PixelLine+PixelIndex);
			for(ufInt8 i=0;i<4;i++){
				pChannel[i]=CalcMulDiv255(pChannel[i],pConstChannel[i]);
			}
		}
	}
}
//---------------------------------------------------------------------------
void cBitmapRGBAResampleSource::BlendFunction_Alpha(void)
{
	auto pConstChannel=reinterpret_cast<uInt8*>(&fOpConstant);
	ufInt8 SrcPRGB[3];
	for(ufInt8 i=0;i<3;i++){
		SrcPRGB[i]=CalcMulDiv255(pConstChannel[i],pConstChannel[3]);
	}
	ufInt8 DestAlpha=0xFF-pConstChannel[3];
	for(sfInt32 LineIndex=0;LineIndex<fSize.y;LineIndex++){
		uInt32 *PixelLine=cnMemory::CastPointerAddByteOffset<uInt32>(fPixelBuffer.Pointer,LineIndex*fSize.x*4);

		for(sfInt32 PixelIndex=0;PixelIndex<fSize.x;PixelIndex++){

			auto *pChannel=reinterpret_cast<uInt8*>(PixelLine+PixelIndex);
			for(ufInt8 i=0;i<4;i++){
				pChannel[i]=CalcMulDiv255(pChannel[i],pConstChannel[i]);
			}

			for(ufInt8 i=0;i<3;i++){
				// dest.rgb = dest*(1-src.a) + src.rgb*src.a
				pChannel[i]=CalcMulDiv255(pChannel[i],DestAlpha);
				pChannel[i]+=SrcPRGB[i];
			}
			// dest.a = dest.a*(1-src.a) + src.a
			pChannel[3]=CalcMulDiv255(pChannel[3],DestAlpha);
			pChannel[3]+=pConstChannel[3];
		}
	}
}
*/
#if	0
//---------------------------------------------------------------------------
iPtr<cBitmapRGBAResampleSource> cnRTL::CreateBitmapByPixelProcess(iBitmapDataSource *Source,eImageBlendingOperator Operator,uInt32 OpColor,uInt32 AddColor)
{
	auto Bitmap=iCreate<cBitmapRGBAResampleSource>();
	if(Bitmap->SetBlendProcess(Source,Operator,OpColor,AddColor)==false)
		return nullptr;
	return Bitmap;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cBitmapDataSourceFromBlendingRGBA32::cBitmapDataSourceFromBlendingRGBA32()
{
	fPixelFormat=BitmapPixelFormat::Unknown;
	fSize={0,0};
}
//---------------------------------------------------------------------------
cBitmapDataSourceFromBlendingRGBA32::~cBitmapDataSourceFromBlendingRGBA32()
{
}
//---------------------------------------------------------------------------
void cBitmapDataSourceFromBlendingRGBA32::ClearData(void)
{
	fPixelFormat=BitmapPixelFormat::Unknown;
	fPixelBuffer.SetLength(0);
	fSize={0,0};
}
//---------------------------------------------------------------------------
bool cBitmapDataSourceFromBlendingRGBA32::SetBlending(uIntn StageCount,const cImageBlendingStage *Stages,sfInt32 Width,sfInt32 Height,eBitmapPixelFormat PixelFormat)
{
	if(fPixelLockCount!=0)	// in use
		return false;

	if(StageCount==0){
		ClearData();
		return true;
	}

	switch(PixelFormat){
	default:
		// unsupported format
		return false;
	case BitmapPixelFormat::R8G8B8X8:
	case BitmapPixelFormat::R8G8B8A8:
	case BitmapPixelFormat::R8G8B8PA8:
	case BitmapPixelFormat::B8G8R8X8:
	case BitmapPixelFormat::B8G8R8A8:
	case BitmapPixelFormat::B8G8R8PA8:
		break;
	}
	fPixelFormat=PixelFormat;

	uIntn StageIndex=0;
	if(Width==0 || Height==0){
		// look for a source
		while(Stages[StageIndex].Source==nullptr){
			StageIndex++;
			if(StageIndex>=StageCount){
				// no any source, user must set size in this situation
				return false;
			}
		}
		// get size from first stage with source
		auto Size=Stages[StageIndex].Source->GetImageSize();
		Width=Size.x;
		Height=Size.y;
		StageIndex++;
	}
	// check stage sizes
	while(StageIndex<StageCount){
		if(Stages[StageIndex].Source!=nullptr){
			auto Size=Stages[StageIndex].Source->GetImageSize();
			if(Width!=Size.x && Height!=Size.y){
				// support only images with same size
				return false;
			}
		}
		StageIndex++;
	}
	fSize.x=Width;
	fSize.y=Height;

	// create buffer for rgba pixel buffer
	fPixelBufferPitch=fSize.x*4;
	fPixelBuffer.SetLength(fSize.y*fPixelBufferPitch);

	// create stage structure until image is processed

	fBlendStages.SetCount(StageCount);

	for(StageIndex=0;StageIndex<StageCount;StageIndex++){
		auto &Stage=Stages[StageIndex];

		auto &BlendStage=fBlendStages[StageIndex];
		BlendStage.Source=Stage.Source;
		BlendStage.BlendFunction=nullptr;
		if(Stage.Source!=nullptr){
			switch(Stage.Operator){
			default:
				break;
			case ImageBlendingOperator::Alpha:
				BlendStage.BlendFunction=&cBitmapDataSourceFromBlendingRGBA32::BlendFunction_Alpha;
				break;
			case ImageBlendingOperator::Copy:
				BlendStage.BlendFunction=&cBitmapDataSourceFromBlendingRGBA32::BlendFunction_Copy;
				break;
			case ImageBlendingOperator::Or:
				BlendStage.BlendFunction=&cBitmapDataSourceFromBlendingRGBA32::BlendFunction_Or;
				break;
			case ImageBlendingOperator::And:
				BlendStage.BlendFunction=&cBitmapDataSourceFromBlendingRGBA32::BlendFunction_And;
				break;
			case ImageBlendingOperator::Xor:
				BlendStage.BlendFunction=&cBitmapDataSourceFromBlendingRGBA32::BlendFunction_Xor;
				break;
			case ImageBlendingOperator::Add:
				BlendStage.BlendFunction=&cBitmapDataSourceFromBlendingRGBA32::BlendFunction_Add;
				break;
			case ImageBlendingOperator::Mul:
				BlendStage.BlendFunction=&cBitmapDataSourceFromBlendingRGBA32::BlendFunction_Mul;
				break;
			}
		}
	}
	return true;
}
//---------------------------------------------------------------------------
cUIPoint cBitmapDataSourceFromBlendingRGBA32::GetImageSize(void)
{
	return fSize;
}
//---------------------------------------------------------------------------
eBitmapPixelFormat cBitmapDataSourceFromBlendingRGBA32::GetPixelFormat(void)
{
	return fPixelFormat;
}
//---------------------------------------------------------------------------
uIntn cBitmapDataSourceFromBlendingRGBA32::CopyPixelBuffer(uIntn Offset,void *Dest,uIntn DestSize)
{
	BlendStages();
	if(Offset>fPixelBuffer.Length)
		return 0;
	uIntn CopySize=DestSize;
	if(Offset+DestSize>fPixelBuffer.Length){
		CopySize=fPixelBuffer.Length-Offset;
	}
	if(CopySize!=0)
		cnMemory::Copy(Dest,cnMemory::PointerAddByteOffset(fPixelBuffer.Pointer,Offset),CopySize);
	return CopySize;
}
//---------------------------------------------------------------------------
const void* cBitmapDataSourceFromBlendingRGBA32::AcquirePixels(void)
{
	if(fPixelLockCount==0xFFFF){
		return nullptr;
	}
	BlendStages();
	fPixelLockCount++;
	return fPixelBuffer.Pointer;
}
//---------------------------------------------------------------------------
void cBitmapDataSourceFromBlendingRGBA32::ReleasePixels(void)
{
	if(fPixelLockCount!=0){
		fPixelLockCount--;
	}
}
//---------------------------------------------------------------------------
uIntn cBitmapDataSourceFromBlendingRGBA32::GetDataPitch(void)
{
	return fPixelBufferPitch;
}
//---------------------------------------------------------------------------
uIntn cBitmapDataSourceFromBlendingRGBA32::GetDataSize(void)
{
	uIntn PixelDataSize=fPixelBuffer.Length;
	if(PixelDataSize!=0)
		return PixelDataSize;
	return fSize.x*fSize.y*4;
}
//---------------------------------------------------------------------------
bool cBitmapDataSourceFromBlendingRGBA32::IsTopDown(void)
{
	return fTopDown;
}
//---------------------------------------------------------------------------
void cBitmapDataSourceFromBlendingRGBA32::BlendStages(void)
{
	if(fBlendStages.GetCount()==0){
		// already loaded
		return;
	}


	cnMemory::ZeroFill(fPixelBuffer.Pointer,fPixelBuffer.Length);

	for(auto &BlendStage : fBlendStages){
		if(BlendStage.BlendFunction!=nullptr){
			(this->*BlendStage.BlendFunction)(BlendStage.Source);
		}
	}
	fBlendStages.Clear();
}
//---------------------------------------------------------------------------
void cBitmapDataSourceFromBlendingRGBA32::BlendFunction_Alpha(iBitmapDataSource *Source)
{
	auto SourcePixelBuffer=Source->AcquirePixels();
	for(sfInt32 LineIndex=0;LineIndex<fSize.y;LineIndex++){
		const uInt32 *SrcPixelLine=cnMemory::CastPointerAddByteOffset<const uInt32>(SourcePixelBuffer,LineIndex*fSize.x*4);
		uInt32 *DestPixelLine=cnMemory::CastPointerAddByteOffset<uInt32>(fPixelBuffer.Pointer,LineIndex*fSize.x*4);

		for(sfInt32 PixelIndex=0;PixelIndex<fSize.x;PixelIndex++){

			auto *pSrcChannel=reinterpret_cast<const uInt8*>(SrcPixelLine+PixelIndex);
			auto *pDestChannel=reinterpret_cast<uInt8*>(DestPixelLine+PixelIndex);
			ufInt8 DestAlpha=0xFF-pSrcChannel[3];
			for(ufInt8 i=0;i<3;i++){
				// dest = dest*(1-src.a) + src.rgb*src.a
				pDestChannel[i]=CalcMulDiv255(pDestChannel[i],DestAlpha);
				ufInt8 Src=CalcMulDiv255(pSrcChannel[i],pSrcChannel[3]);

				pDestChannel[i]+=Src;
			}
			pDestChannel[3]=CalcMulDiv255(pDestChannel[3],DestAlpha);
			pDestChannel[3]+=pSrcChannel[3];
		}
	}
	Source->ReleasePixels();
}
//---------------------------------------------------------------------------
void cBitmapDataSourceFromBlendingRGBA32::BlendFunction_Copy(iBitmapDataSource *Source)
{
	auto SourcePixelBuffer=Source->AcquirePixels();
	for(sfInt32 LineIndex=0;LineIndex<fSize.y;LineIndex++){
		const uInt32 *SrcPixelLine=cnMemory::CastPointerAddByteOffset<const uInt32>(SourcePixelBuffer,LineIndex*fSize.x*4);
		uInt32 *DestPixelLine=cnMemory::CastPointerAddByteOffset<uInt32>(fPixelBuffer.Pointer,LineIndex*fSize.x*4);

		for(sfInt32 PixelIndex=0;PixelIndex<fSize.x;PixelIndex++){
			// dest = src
			DestPixelLine[PixelIndex]=SrcPixelLine[PixelIndex];
		}
	}
	Source->ReleasePixels();
}
//---------------------------------------------------------------------------
void cBitmapDataSourceFromBlendingRGBA32::BlendFunction_Or(iBitmapDataSource *Source)
{
	auto SourcePixelBuffer=Source->AcquirePixels();
	for(sfInt32 LineIndex=0;LineIndex<fSize.y;LineIndex++){
		const uInt32 *SrcPixelLine=cnMemory::CastPointerAddByteOffset<const uInt32>(SourcePixelBuffer,LineIndex*fSize.x*4);
		uInt32 *DestPixelLine=cnMemory::CastPointerAddByteOffset<uInt32>(fPixelBuffer.Pointer,LineIndex*fSize.x*4);

		for(sfInt32 PixelIndex=0;PixelIndex<fSize.x;PixelIndex++){
			// dest = dest or src
			DestPixelLine[PixelIndex]|=SrcPixelLine[PixelIndex];
		}
	}
	Source->ReleasePixels();
}
//---------------------------------------------------------------------------
void cBitmapDataSourceFromBlendingRGBA32::BlendFunction_And(iBitmapDataSource *Source)
{
	auto SourcePixelBuffer=Source->AcquirePixels();
	for(sfInt32 LineIndex=0;LineIndex<fSize.y;LineIndex++){
		const uInt32 *SrcPixelLine=cnMemory::CastPointerAddByteOffset<const uInt32>(SourcePixelBuffer,LineIndex*fSize.x*4);
		uInt32 *DestPixelLine=cnMemory::CastPointerAddByteOffset<uInt32>(fPixelBuffer.Pointer,LineIndex*fSize.x*4);

		for(sfInt32 PixelIndex=0;PixelIndex<fSize.x;PixelIndex++){
			// dest = dest and src
			DestPixelLine[PixelIndex]&=SrcPixelLine[PixelIndex];
		}
	}
	Source->ReleasePixels();
}
//---------------------------------------------------------------------------
void cBitmapDataSourceFromBlendingRGBA32::BlendFunction_Xor(iBitmapDataSource *Source)
{
	auto SourcePixelBuffer=Source->AcquirePixels();
	for(sfInt32 LineIndex=0;LineIndex<fSize.y;LineIndex++){
		const uInt32 *SrcPixelLine=cnMemory::CastPointerAddByteOffset<const uInt32>(SourcePixelBuffer,LineIndex*fSize.x*4);
		uInt32 *DestPixelLine=cnMemory::CastPointerAddByteOffset<uInt32>(fPixelBuffer.Pointer,LineIndex*fSize.x*4);

		for(sfInt32 PixelIndex=0;PixelIndex<fSize.x;PixelIndex++){
			// dest = dest xor src
			DestPixelLine[PixelIndex]^=SrcPixelLine[PixelIndex];
		}
	}
	Source->ReleasePixels();
}
//---------------------------------------------------------------------------
void cBitmapDataSourceFromBlendingRGBA32::BlendFunction_Add(iBitmapDataSource *Source)
{
	auto SourcePixelBuffer=Source->AcquirePixels();
	for(sfInt32 LineIndex=0;LineIndex<fSize.y;LineIndex++){
		const uInt32 *SrcPixelLine=cnMemory::CastPointerAddByteOffset<const uInt32>(SourcePixelBuffer,LineIndex*fSize.x*4);
		uInt32 *DestPixelLine=cnMemory::CastPointerAddByteOffset<uInt32>(fPixelBuffer.Pointer,LineIndex*fSize.x*4);

		for(sfInt32 PixelIndex=0;PixelIndex<fSize.x;PixelIndex++){

			auto *pSrcChannel=reinterpret_cast<const uInt8*>(SrcPixelLine+PixelIndex);
			auto *pDestChannel=reinterpret_cast<uInt8*>(DestPixelLine+PixelIndex);

			for(ufInt8 i=0;i<4;i++){
				// dest = dest + src
				pDestChannel[i]=CalcAdd255(pDestChannel[i],pSrcChannel[i]);
			}
		}
	}
	Source->ReleasePixels();
}
//---------------------------------------------------------------------------
void cBitmapDataSourceFromBlendingRGBA32::BlendFunction_Mul(iBitmapDataSource *Source)
{
	auto SourcePixelBuffer=Source->AcquirePixels();
	for(sfInt32 LineIndex=0;LineIndex<fSize.y;LineIndex++){
		const uInt32 *SrcPixelLine=cnMemory::CastPointerAddByteOffset<const uInt32>(SourcePixelBuffer,LineIndex*fSize.x*4);
		uInt32 *DestPixelLine=cnMemory::CastPointerAddByteOffset<uInt32>(fPixelBuffer.Pointer,LineIndex*fSize.x*4);

		for(sfInt32 PixelIndex=0;PixelIndex<fSize.x;PixelIndex++){

			auto *pSrcChannel=reinterpret_cast<const uInt8*>(SrcPixelLine+PixelIndex);
			auto *pDestChannel=reinterpret_cast<uInt8*>(DestPixelLine+PixelIndex);

			for(ufInt8 i=0;i<4;i++){
				// dest = dest * src / 0xFF
				pDestChannel[i]=CalcMulDiv255(pDestChannel[i],pSrcChannel[i]);
			}
		}
	}
	Source->ReleasePixels();
}
//---------------------------------------------------------------------------
iPtr<iBitmapDataSource> cnRTL::CreateBitmapSourceFromBlendingRGBA32(uIntn StageCount,const cImageBlendingStage *Stages,sfInt32 Width,sfInt32 Height,eBitmapPixelFormat PixelFormat)
{
	auto BitmapMemSource=iCreate<cBitmapDataSourceFromBlendingRGBA32>();
	if(BitmapMemSource->SetBlending(StageCount,Stages,Width,Height,PixelFormat)==false)
		return nullptr;
	return BitmapMemSource;
}
#endif

#if	0
//---------------------------------------------------------------------------
cBitmapRGBAResampleSource::cBitmapRGBAResampleSource()
{
	fUpdate=false;
	ImageSize.x=0;
	ImageSize.y=0;
	PixelFormat.ChannelType=BitmapChannelType::UNorm;
	PixelFormat.ChannelColor=BitmapChannelColor::RGBA;
	PixelFormat.ChannelCount=4;
	PixelFormat.ChannelSize=1;
	PixelFormat.AlphaPremultiplied=true;
}
//---------------------------------------------------------------------------
cBitmapRGBAResampleSource::~cBitmapRGBAResampleSource()
{
}
//---------------------------------------------------------------------------
void cBitmapRGBAResampleSource::Update(void)
{
	fUpdate=true;
}
//---------------------------------------------------------------------------
cUIPoint cBitmapRGBAResampleSource::GetImageSize(void)
{
	return ImageSize;
}
//---------------------------------------------------------------------------
cBitmapPixelFormat cBitmapRGBAResampleSource::GetPixelFormat(void)
{
	return PixelFormat;
}
//---------------------------------------------------------------------------
uIntn cBitmapRGBAResampleSource::CopyPixelBuffer(uIntn Offset,void *Dest,uIntn DestSize)
{
	if(fPixelLockCount==0){
		ProcessSource();
	}
	if(Offset>fPixelBuffer.Length)
		return 0;
	uIntn CopySize=DestSize;
	if(Offset+DestSize>fPixelBuffer.Length){
		CopySize=fPixelBuffer.Length-Offset;
	}
	if(CopySize!=0)
		cnMemory::Copy(Dest,cnMemory::PointerAddByteOffset(fPixelBuffer.Pointer,Offset),CopySize);
	return CopySize;
}
//---------------------------------------------------------------------------
const void* cBitmapRGBAResampleSource::AcquirePixels(void)
{
	if(fPixelLockCount==0xFFFF){
		return nullptr;
	}
	if(fPixelLockCount==0){
		ProcessSource();
	}
	fPixelLockCount++;
	return fPixelBuffer.Pointer;
}
//---------------------------------------------------------------------------
void cBitmapRGBAResampleSource::ReleasePixels(void)
{
	if(fPixelLockCount!=0){
		fPixelLockCount--;
	}
}
//---------------------------------------------------------------------------
uIntn cBitmapRGBAResampleSource::GetDataPitch(void)
{
	if(SourceSampler==nullptr)
		return 0;
	return fPitch;
}
//---------------------------------------------------------------------------
uIntn cBitmapRGBAResampleSource::GetDataSize(void)
{
	if(fPixelLockCount==0){
		ProcessSource();
	}
	return fPixelBuffer.Length;
}
//---------------------------------------------------------------------------
bool cBitmapRGBAResampleSource::IsTopDown(void)
{
	return TopDown;
}
//---------------------------------------------------------------------------
void cBitmapRGBAResampleSource::ProcessSource(void)
{
	if(fUpdate==false)
		return;
	fUpdate=false;

	if(PixelFormat.ChannelSize==0 || PixelFormat.ChannelCount==0
		|| SourceSampler==nullptr || ImageSize.x==0 || ImageSize.y==0
	){
		fPitch=0;
		fPixelBuffer.SetLength(0);
		return;
	}

	fPitch=ImageSize.x*PixelFormat.ChannelSize*PixelFormat.ChannelCount;
	fPitch=cnMath::IntRoundPositive<4>(fPitch);
	uIntn DataSize=fPitch*ImageSize.y;
	fPixelBuffer.SetLength(DataSize);

	SourceSampler->BeginSample(ImageSize);

	switch(PixelFormat.ChannelSize){
	default:
		SampleProcessError();
		break;
	case 1:
		Sample8Process();
		break;
	case 2:
		Sample16Process();
		break;
	case 4:
		Sample32Process();
		break;
	}

	SourceSampler->EndSample();

}
//---------------------------------------------------------------------------
void cBitmapRGBAResampleSource::SampleProcessError(void)
{
	cnMemory::ZeroFill(fPixelBuffer.Pointer,fPixelBuffer.Length);
}
//---------------------------------------------------------------------------
void cBitmapRGBAResampleSource::Sample8Process(void)
{
	switch(PixelFormat.ChannelType){
	default:
	//case BitmapChannelType::Float:
		return SampleProcessError();

	case BitmapChannelType::UNorm:
		switch(PixelFormat.ChannelColor){
		case BitmapChannelColor::Unknown:
		case BitmapChannelColor::Alpha:
			Sample8ProcessUNormA();
			break;
		case BitmapChannelColor::Luminance:
			Sample8ProcessUNormL();
			break;
		case BitmapChannelColor::LA:
			Sample8ProcessUNormLA();
			break;
		case BitmapChannelColor::RGB:
		case BitmapChannelColor::RGBA:
			Sample8ProcessUNormRGBA();
			break;
		case BitmapChannelColor::BGR:
		case BitmapChannelColor::BGRA:
			Sample8ProcessUNormBGRA();
			break;
		}
		break;
	//case BitmapChannelType::SNorm:
	//	break;
	//case BitmapChannelType::SIntegral:
	//	break;
	//case BitmapChannelType::UIntegral:
	//	break;
	}
}
//---------------------------------------------------------------------------
void cBitmapRGBAResampleSource::Sample16Process(void)
{
	switch(PixelFormat.ChannelType){
	default:
	//case BitmapChannelType::Float:
		return SampleProcessError();

	case BitmapChannelType::UNorm:
		switch(PixelFormat.ChannelColor){
		case BitmapChannelColor::Unknown:
		case BitmapChannelColor::Alpha:
			Sample8ProcessUNormA();
			break;
		case BitmapChannelColor::Luminance:
			Sample8ProcessUNormL();
			break;
		case BitmapChannelColor::LA:
			Sample8ProcessUNormLA();
			break;
		case BitmapChannelColor::RGB:
		case BitmapChannelColor::RGBA:
			Sample8ProcessUNormRGBA();
			break;
		case BitmapChannelColor::BGR:
		case BitmapChannelColor::BGRA:
			Sample8ProcessUNormBGRA();
			break;
		}
		break;
	//case BitmapChannelType::SNorm:
	//	break;
	//case BitmapChannelType::SIntegral:
	//	break;
	//case BitmapChannelType::UIntegral:
	//	break;
	}
}
//---------------------------------------------------------------------------
void cBitmapRGBAResampleSource::Sample32Process(void)
{
	switch(PixelFormat.ChannelType){
	default:
		return SampleProcessError();

	/*
	case BitmapChannelType::UNorm:
		switch(PixelFormat.ChannelColor){
		case BitmapChannelColor::Unknown:
		case BitmapChannelColor::Alpha:
			Sample8ProcessUNormA();
			break;
		case BitmapChannelColor::Luminance:
			Sample8ProcessUNormL();
			break;
		case BitmapChannelColor::LA:
			Sample8ProcessUNormLA();
			break;
		case BitmapChannelColor::RGB:
		case BitmapChannelColor::RGBA:
			Sample8ProcessUNormRGBA();
			break;
		case BitmapChannelColor::BGR:
		case BitmapChannelColor::BGRA:
			Sample8ProcessUNormBGRA();
			break;
		}
		break;
	//case BitmapChannelType::SNorm:
	//	break;
	*/
	case BitmapChannelType::Float:
		switch(PixelFormat.ChannelColor){
		case BitmapChannelColor::Unknown:
		case BitmapChannelColor::Alpha:
			Sample32ProcessFloatA();
			break;
		case BitmapChannelColor::Luminance:
			Sample32ProcessFloatL();
			break;
		case BitmapChannelColor::LA:
			Sample32ProcessFloatLA();
			break;
		case BitmapChannelColor::RGB:
		case BitmapChannelColor::RGBA:
			Sample32ProcessFloatRGBA();
			break;
		case BitmapChannelColor::BGR:
		case BitmapChannelColor::BGRA:
			Sample32ProcessFloatBGRA();
			break;
		}
		break;
	/*
	//case BitmapChannelType::SIntegral:
	//	break;
	//case BitmapChannelType::UIntegral:
	//	break;
	*/
	}
}
//---------------------------------------------------------------------------
void cBitmapRGBAResampleSource::Sample8ProcessUNormRGBA(void)
{
	uInt8 *Pixels=static_cast<uInt8*>(fPixelBuffer.Pointer);
	ufInt8 ChannelCount=PixelFormat.ChannelCount;
	if(PixelFormat.ChannelCount==4){
		for(sfInt32 y=0;y<ImageSize.y;y++){
			auto *RowPixels=Pixels+fPitch;
			for(sfInt32 x=0;x<ImageSize.x;x++){
				auto ColorValue=SourceSampler->SampleUNorm8(x,y);

				ColorValue.Store(RowPixels);
				RowPixels+=4;
			}
		}
	}
	else if(PixelFormat.ChannelCount>4){
		cnMemory::ZeroFill(fPixelBuffer.Pointer,fPixelBuffer.Length);
		for(sfInt32 y=0;y<ImageSize.y;y++){
			auto *RowPixels=Pixels+fPitch;
			for(sfInt32 x=0;x<ImageSize.x;x++){
				auto ColorValue=SourceSampler->SampleUNorm8(x,y);

				ColorValue.Store(RowPixels);
				RowPixels+=ChannelCount;
			}
		}
	}
	else{	//  PixelFormat.ChannelCount<4
		for(sfInt32 y=0;y<ImageSize.y;y++){
			auto *RowPixels=Pixels+fPitch;
			for(sfInt32 x=0;x<ImageSize.x;x++){
				auto ColorValue=SourceSampler->SampleUNorm8(x,y);

				ColorValue.Store(RowPixels,ChannelCount);

				RowPixels+=ChannelCount;
			}
		}
	}
}
//---------------------------------------------------------------------------
void cBitmapRGBAResampleSource::Sample8ProcessUNormBGRA(void)
{
	uInt8 *Pixels=static_cast<uInt8*>(fPixelBuffer.Pointer);
	ufInt8 ChannelCount=PixelFormat.ChannelCount;
	if(PixelFormat.ChannelCount>=4){
		cnMemory::ZeroFill(fPixelBuffer.Pointer,fPixelBuffer.Length);
		for(sfInt32 y=0;y<ImageSize.y;y++){
			auto *RowPixels=Pixels+fPitch;
			for(sfInt32 x=0;x<ImageSize.x;x++){
				auto ColorValue=SourceSampler->SampleUNorm8(x,y);
				alignas(cColorVector<uInt8,4>) uInt8 PixelValue[4];
				ColorValue.StoreA(PixelValue);

				RowPixels[0]=PixelValue[2];
				RowPixels[1]=PixelValue[1];
				RowPixels[2]=PixelValue[0];
				RowPixels[3]=PixelValue[3];

				RowPixels+=ChannelCount;
			}
		}
	}
	else{	//  PixelFormat.ChannelCount<4
		for(sfInt32 y=0;y<ImageSize.y;y++){
			auto *RowPixels=Pixels+fPitch;
			for(sfInt32 x=0;x<ImageSize.x;x++){
				auto ColorValue=SourceSampler->SampleUNorm8(x,y);
				alignas(cColorVector<uInt8,4>) uInt8 PixelValue[4];
				ColorValue.StoreA(PixelValue);

				uInt8 t=PixelValue[2];
				PixelValue[2]=PixelValue[0];
				PixelValue[0]=t;

				for(ufInt8 i=0;i<ChannelCount;i++){
					RowPixels[i]=PixelValue[i];
				}

				RowPixels+=ChannelCount;
			}
		}
	}
}
//---------------------------------------------------------------------------
void cBitmapRGBAResampleSource::Sample8ProcessUNormA(void)
{
	uInt8 *Pixels=static_cast<uInt8*>(fPixelBuffer.Pointer);
	ufInt8 ChannelCount=PixelFormat.ChannelCount;
	cnMemory::ZeroFill(fPixelBuffer.Pointer,fPixelBuffer.Length);
	for(sfInt32 y=0;y<ImageSize.y;y++){
		auto *RowPixels=Pixels+fPitch;
		for(sfInt32 x=0;x<ImageSize.x;x++){
			auto ColorValue=SourceSampler->SampleUNorm8(x,y);
			alignas(cColorVector<uInt8,4>) uInt8 PixelValue[4];
			ColorValue.StoreA(PixelValue);

			RowPixels[0]=PixelValue[3];

			RowPixels+=ChannelCount;
		}
	}
}
//---------------------------------------------------------------------------
void cBitmapRGBAResampleSource::Sample8ProcessUNormL(void)
{
	uInt8 *Pixels=static_cast<uInt8*>(fPixelBuffer.Pointer);
	ufInt8 ChannelCount=PixelFormat.ChannelCount;
	cnMemory::ZeroFill(fPixelBuffer.Pointer,fPixelBuffer.Length);
	for(sfInt32 y=0;y<ImageSize.y;y++){
		auto *RowPixels=Pixels+fPitch;
		for(sfInt32 x=0;x<ImageSize.x;x++){
			auto ColorValue=SourceSampler->SampleUNorm8(x,y);
			alignas(cColorVector<uInt8,4>) uInt8 PixelValue[4];
			ColorValue.StoreA(PixelValue);

			ufInt16 LValue=PixelValue[0];
			LValue+=PixelValue[1];
			LValue+=PixelValue[2];
			RowPixels[0]=static_cast<uInt8>(LValue/3);

			RowPixels+=ChannelCount;
		}
	}
}
//---------------------------------------------------------------------------
void cBitmapRGBAResampleSource::Sample8ProcessUNormLA(void)
{
	if(PixelFormat.ChannelCount<=1){
		return Sample8ProcessUNormL();
	}
	uInt8 *Pixels=static_cast<uInt8*>(fPixelBuffer.Pointer);
	ufInt8 ChannelCount=PixelFormat.ChannelCount;
	cnMemory::ZeroFill(fPixelBuffer.Pointer,fPixelBuffer.Length);
	for(sfInt32 y=0;y<ImageSize.y;y++){
		auto *RowPixels=Pixels+fPitch;
		for(sfInt32 x=0;x<ImageSize.x;x++){
			auto ColorValue=SourceSampler->SampleUNorm8(x,y);
			alignas(cColorVector<uInt8,4>) uInt8 PixelValue[4];
			ColorValue.StoreA(PixelValue);

			ufInt16 LValue=PixelValue[0];
			LValue+=PixelValue[1];
			LValue+=PixelValue[2];
			RowPixels[0]=static_cast<uInt8>(LValue/3);
			RowPixels[1]=PixelValue[3];

			RowPixels+=ChannelCount;
		}
	}
}

//---------------------------------------------------------------------------
void cBitmapRGBAResampleSource::Sample32ProcessFloatRGBA(void)
{
	Float32 *Pixels=static_cast<Float32*>(fPixelBuffer.Pointer);
	ufInt8 ChannelCount=PixelFormat.ChannelCount;
	if(PixelFormat.ChannelCount==4){
		for(sfInt32 y=0;y<ImageSize.y;y++){
			auto *RowPixels=cnMemory::PointerAddByteOffset(Pixels,fPitch);
			for(sfInt32 x=0;x<ImageSize.x;x++){
				auto ColorValue=SourceSampler->SampleFloat32(x,y);

				ColorValue.Store(RowPixels);
				RowPixels+=4;
			}
		}
	}
	else if(PixelFormat.ChannelCount>4){
		cnMemory::ZeroFill(fPixelBuffer.Pointer,fPixelBuffer.Length);
		for(sfInt32 y=0;y<ImageSize.y;y++){
			auto *RowPixels=cnMemory::PointerAddByteOffset(Pixels,fPitch);
			for(sfInt32 x=0;x<ImageSize.x;x++){
				auto ColorValue=SourceSampler->SampleFloat32(x,y);

				ColorValue.Store(RowPixels);
				RowPixels+=ChannelCount;
			}
		}
	}
	else{	//  PixelFormat.ChannelCount<4
		for(sfInt32 y=0;y<ImageSize.y;y++){
			auto *RowPixels=cnMemory::PointerAddByteOffset(Pixels,fPitch);
			for(sfInt32 x=0;x<ImageSize.x;x++){
				auto ColorValue=SourceSampler->SampleFloat32(x,y);

				ColorValue.Store(RowPixels,ChannelCount);

				RowPixels+=ChannelCount;
			}
		}
	}
}
//---------------------------------------------------------------------------
void cBitmapRGBAResampleSource::Sample32ProcessFloatBGRA(void)
{
	Float32 *Pixels=static_cast<Float32*>(fPixelBuffer.Pointer);
	ufInt8 ChannelCount=PixelFormat.ChannelCount;
	if(PixelFormat.ChannelCount>=4){
		cnMemory::ZeroFill(fPixelBuffer.Pointer,fPixelBuffer.Length);
		for(sfInt32 y=0;y<ImageSize.y;y++){
			auto *RowPixels=cnMemory::PointerAddByteOffset(Pixels,fPitch);
			for(sfInt32 x=0;x<ImageSize.x;x++){
				auto ColorValue=SourceSampler->SampleFloat32(x,y);
				alignas(cVector<Float32,4>) Float32 PixelValue[4];
				ColorValue.StoreA(PixelValue);

				RowPixels[0]=PixelValue[2];
				RowPixels[1]=PixelValue[1];
				RowPixels[2]=PixelValue[0];
				RowPixels[3]=PixelValue[3];

				RowPixels+=ChannelCount;
			}
		}
	}
	else{	//  PixelFormat.ChannelCount<4
		for(sfInt32 y=0;y<ImageSize.y;y++){
			auto *RowPixels=cnMemory::PointerAddByteOffset(Pixels,fPitch);
			for(sfInt32 x=0;x<ImageSize.x;x++){
				auto ColorValue=SourceSampler->SampleFloat32(x,y);
				alignas(cVector<Float32,4>) Float32 PixelValue[4];
				ColorValue.StoreA(PixelValue);

				Float32 t=PixelValue[2];
				PixelValue[2]=PixelValue[0];
				PixelValue[0]=t;

				for(ufInt8 i=0;i<ChannelCount;i++){
					RowPixels[i]=PixelValue[i];
				}

				RowPixels+=ChannelCount;
			}
		}
	}
}
//---------------------------------------------------------------------------
void cBitmapRGBAResampleSource::Sample32ProcessFloatA(void)
{
	Float32 *Pixels=static_cast<Float32*>(fPixelBuffer.Pointer);
	ufInt8 ChannelCount=PixelFormat.ChannelCount;
	cnMemory::ZeroFill(fPixelBuffer.Pointer,fPixelBuffer.Length);
	for(sfInt32 y=0;y<ImageSize.y;y++){
		auto *RowPixels=cnMemory::PointerAddByteOffset(Pixels,fPitch);
		for(sfInt32 x=0;x<ImageSize.x;x++){
			auto ColorValue=SourceSampler->SampleFloat32(x,y);
			alignas(cVector<Float32,4>) Float32 PixelValue[4];
			ColorValue.StoreA(PixelValue);

			RowPixels[0]=PixelValue[3];

			RowPixels+=ChannelCount;
		}
	}
}
//---------------------------------------------------------------------------
void cBitmapRGBAResampleSource::Sample32ProcessFloatL(void)
{
	Float32 *Pixels=static_cast<Float32*>(fPixelBuffer.Pointer);
	ufInt8 ChannelCount=PixelFormat.ChannelCount;
	cnMemory::ZeroFill(fPixelBuffer.Pointer,fPixelBuffer.Length);
	for(sfInt32 y=0;y<ImageSize.y;y++){
		auto *RowPixels=cnMemory::PointerAddByteOffset(Pixels,fPitch);
		for(sfInt32 x=0;x<ImageSize.x;x++){
			auto ColorValue=SourceSampler->SampleFloat32(x,y);
			alignas(cVector<Float32,4>) Float32 PixelValue[4];
			ColorValue.StoreA(PixelValue);

			Float32 LValue=PixelValue[0];
			LValue+=PixelValue[1];
			LValue+=PixelValue[2];
			RowPixels[0]=LValue/3;

			RowPixels+=ChannelCount;
		}
	}
}
//---------------------------------------------------------------------------
void cBitmapRGBAResampleSource::Sample32ProcessFloatLA(void)
{
	if(PixelFormat.ChannelCount<=1){
		return Sample32ProcessFloatL();
	}
	Float32 *Pixels=static_cast<Float32*>(fPixelBuffer.Pointer);
	ufInt8 ChannelCount=PixelFormat.ChannelCount;
	cnMemory::ZeroFill(fPixelBuffer.Pointer,fPixelBuffer.Length);
	for(sfInt32 y=0;y<ImageSize.y;y++){
		auto *RowPixels=Pixels+fPitch;
		for(sfInt32 x=0;x<ImageSize.x;x++){
			auto ColorValue=SourceSampler->SampleFloat32(x,y);
			alignas(cVector<Float32,4>) Float32 PixelValue[4];
			ColorValue.StoreA(PixelValue);

			Float32 LValue=PixelValue[0];
			LValue+=PixelValue[1];
			LValue+=PixelValue[2];
			RowPixels[0]=LValue/3;
			RowPixels[1]=PixelValue[3];

			RowPixels+=ChannelCount;
		}
	}
}
#endif


//---------------------------------------------------------------------------
const cBitmapPixelFormat cnRTL::BitmapPixelFormatUnknow={
	BitmapChannelColor::Unknown,// ChannelColor
	BitmapChannelType::UNorm,	// ChannelType
	0,							// ChannelCount
	0,							// ChannelSize
	false,						// AlphaPremultiplied
};

const cBitmapPixelFormat cnRTL::BitmapPixelFormatR8G8B8A8={
	BitmapChannelColor::RGBA,	// ChannelColor
	BitmapChannelType::UNorm,	// ChannelType
	4,							// ChannelCount
	1,							// ChannelSize
	false,						// AlphaPremultiplied
};


//---------------------------------------------------------------------------
bool cnRTL::BitmapColorHasAlpha(eBitmapChannelColor Color)noexcept
{
	switch(Color){
	default:
		return false;
	case BitmapChannelColor::Alpha:
	case BitmapChannelColor::LA:
	case BitmapChannelColor::BGRA:
	case BitmapChannelColor::RGBA:
		return true;
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool cnRTL::UIRectIsEqual(const cUIRect &Rect1,const cUIRect &Rect2)noexcept
{
	if(Rect1.Size.x!=Rect2.Size.x)
		return false;
	if(Rect1.Size.y!=Rect2.Size.y)
		return false;
	if(Rect1.Pos.x!=Rect2.Pos.x)
		return false;
	if(Rect1.Pos.y!=Rect2.Pos.y)
		return false;
	return true;
}
//---------------------------------------------------------------------------
bool cnRTL::UIRectangleIsEqual(const cUIRectangle &Rect1,const cUIRectangle &Rect2)noexcept
{
	if(Rect1.Left!=Rect2.Left)
		return false;
	if(Rect1.Top!=Rect2.Top)
		return false;
	if(Rect1.Right!=Rect2.Right)
		return false;
	if(Rect1.Bottom!=Rect2.Bottom)
		return false;
	return true;
}

//---------------------------------------------------------------------------
//void cBitmapDataSourceFromBlendingRGBA32::BlendAddConstant(uInt32 Constant)
//{
//	auto *pSrcChannel=reinterpret_cast<uInt8*>(&Constant);
//	bool pSrcChannelSign[4];
//	for(ufInt8 i=0;i<4;i++){
//		pSrcChannelSign[i]=static_cast<sInt8>(pSrcChannel[i])<0;
//	}
//	uIntn PixelCount=fSize.x*fSize.y;
//	uInt32 *DestPixels=static_cast<uInt32*>(fPixelBuffer.Pointer);
//	for(sfInt32 PixelIndex=0;PixelIndex<PixelCount;PixelIndex++){
//			
//		auto *pDestChannel=reinterpret_cast<uInt8*>(DestPixels+PixelIndex);
//
//		for(ufInt8 i=0;i<4;i++){
//			// dest += constant
//			if(static_cast<sInt8>(pDestChannel[i])<0 && pSrcChannelSign[i]){
//				// overflow
//				pDestChannel[i]=0xFF;
//			}
//			else{
//				pDestChannel[i]+=pSrcChannel[i];
//			}
//		}
//	}
//
//}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cBitmapBuffer::cBitmapBuffer()noexcept
{
	fWidth=0;
	fHeight=0;
	fPixelFormat=BitmapPixelFormatUnknow;
}
//---------------------------------------------------------------------------
cBitmapBuffer::~cBitmapBuffer()noexcept
{
}
//---------------------------------------------------------------------------
bool cBitmapBuffer::SetParameter(cBitmapPixelFormat PixelFormat,sfInt32 Width,sfInt32 Height,bool TopDown)noexcept
{
	if(fBufferLockCount!=0)	// in use
		return false;

	fPixelFormat=PixelFormat;
	fWidth=Width;
	fHeight=Height;
	fTopDown=TopDown;
	ResetFrameBuffer();

	return true;
}
//---------------------------------------------------------------------------
cUIPoint cBitmapBuffer::GetImageSize(void)noexcept
{
	cUIPoint Ret;
	Ret.x=static_cast<Float32>(fWidth);
	Ret.y=static_cast<Float32>(fHeight);
	return Ret;
}
//---------------------------------------------------------------------------
void cBitmapBuffer::ResetFrameBuffer(void)noexcept
{
	uIntn PixelSize=fPixelFormat.ChannelCount*fPixelFormat.ChannelSize;
	fPixelBufferPitch=fWidth*PixelSize;
	if(fPixelBufferPitch==0 || fHeight==0){
		fPixelBuffer.SetLength(0);
		return;
	}
	fPixelBufferPitch=cnMath::IntRoundPositive<4>(fPixelBufferPitch);
	uIntn NewImageSize=fHeight*fPixelBufferPitch;

	fPixelBuffer.SetLength(NewImageSize);
	cnMemory::ZeroFill(fPixelBuffer.Pointer,fPixelBuffer.Length);
}
//---------------------------------------------------------------------------
cBitmapPixelFormat cBitmapBuffer::GetPixelFormat(void)noexcept
{
	return fPixelFormat;
}
//---------------------------------------------------------------------------
uIntn cBitmapBuffer::CopyPixelBuffer(uIntn Offset,void *Dest,uIntn DestSize)noexcept
{
	if(Offset>fPixelBuffer.Length){
		return 0;
	}
	uIntn CopySize=DestSize;
	if(Offset+DestSize>fPixelBuffer.Length){
		CopySize=fPixelBuffer.Length-Offset;
	}
	if(CopySize!=0)
		cnMemory::Copy(Dest,cnMemory::PointerAddByteOffset(fPixelBuffer.Pointer,Offset),CopySize);
	return CopySize;
}
//---------------------------------------------------------------------------
const void* cBitmapBuffer::AcquirePixels(void)noexcept
{
	if(fBufferLockCount==0xFFFF){
		return nullptr;
	}
	fBufferLockCount++;
	return fPixelBuffer.Pointer;
}
//---------------------------------------------------------------------------
void cBitmapBuffer::ReleasePixels(void)noexcept
{
	if(fBufferLockCount!=0){
		fBufferLockCount--;
	}
}
//---------------------------------------------------------------------------
uIntn cBitmapBuffer::GetDataPitch(void)noexcept
{
	return fPixelBufferPitch;
}
//---------------------------------------------------------------------------
uIntn cBitmapBuffer::GetDataSize(void)const noexcept
{
	return fPixelBuffer.Length;
}
//---------------------------------------------------------------------------
void* cBitmapBuffer::GetDataBuffer(void)const noexcept
{
	return fPixelBuffer.Pointer;
}
//---------------------------------------------------------------------------
bool cBitmapBuffer::IsTopDown(void)noexcept
{
	return fTopDown;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cBitmapDataSourceFromMemory::cBitmapDataSourceFromMemory()noexcept
{
	fWidth=0;
	fHeight=0;
	fPixelFormat=BitmapPixelFormatUnknow;
}
//---------------------------------------------------------------------------
cBitmapDataSourceFromMemory::~cBitmapDataSourceFromMemory()noexcept
{
}
//---------------------------------------------------------------------------
bool cBitmapDataSourceFromMemory::SetData(cBitmapPixelFormat PixelFormat,sfInt32 Width,sfInt32 Height,uIntn PixelDataPitch,const void *PixelData,bool TopDown)noexcept
{
	if(fBufferLockCount!=0)	// in use
		return false;

	fPixelFormat=PixelFormat;
	fWidth=Width;
	fHeight=Height;
	fPixelDataPitch=PixelDataPitch;
	fPixelData=PixelData;
	fTopDown=TopDown;

	uIntn PixelSize=fPixelFormat.ChannelCount*fPixelFormat.ChannelSize;
	sfInt32 NewImageLineSize=fWidth*PixelSize;
	NewImageLineSize=cnMath::IntRoundPositive<4>(NewImageLineSize);
	fPixelDataSize=fHeight*NewImageLineSize;
	return true;
}
//---------------------------------------------------------------------------
cBitmapPixelFormat cBitmapDataSourceFromMemory::GetPixelFormat(void)noexcept
{
	return fPixelFormat;
}
//---------------------------------------------------------------------------
cUIPoint cBitmapDataSourceFromMemory::GetImageSize(void)noexcept
{
	cUIPoint Ret;
	Ret.x=static_cast<Float32>(fWidth);
	Ret.y=static_cast<Float32>(fHeight);
	return Ret;
}
//---------------------------------------------------------------------------
uIntn cBitmapDataSourceFromMemory::CopyPixelBuffer(uIntn Offset,void *Dest,uIntn DestSize)noexcept
{
	if(Offset>fPixelDataSize)
		return 0;
	uIntn CopySize=DestSize;
	if(Offset+DestSize>fPixelDataSize){
		CopySize=fPixelDataSize-Offset;
	}
	if(CopySize!=0)
		cnMemory::Copy(Dest,cnMemory::PointerAddByteOffset(fPixelData,Offset),CopySize);
	return CopySize;
}
//---------------------------------------------------------------------------
const void* cBitmapDataSourceFromMemory::AcquirePixels(void)noexcept
{
	if(fBufferLockCount==0xFFFF){
		return nullptr;
	}
	fBufferLockCount++;
	return fPixelData;
}
//---------------------------------------------------------------------------
void cBitmapDataSourceFromMemory::ReleasePixels(void)noexcept
{
	if(fBufferLockCount!=0){
		fBufferLockCount--;
	}
}
//---------------------------------------------------------------------------
uIntn cBitmapDataSourceFromMemory::GetDataPitch(void)noexcept
{
	return fPixelDataPitch;
}
//---------------------------------------------------------------------------
const void* cBitmapDataSourceFromMemory::GetDataBuffer(void)const noexcept
{
	return fPixelData;
}
//---------------------------------------------------------------------------
uIntn cBitmapDataSourceFromMemory::GetDataSize(void)const noexcept
{
	return fPixelDataSize;
}
//---------------------------------------------------------------------------
bool cBitmapDataSourceFromMemory::IsTopDown(void)noexcept
{
	return fTopDown;
}
//---------------------------------------------------------------------------
iPtr<cBitmapDataSourceFromMemory> cnRTL::CreateBitmapFromMemory(cBitmapPixelFormat PixelFormat,sfInt32 Width,sfInt32 Height,uIntn PixelDataPitchSize,const void *PixelData,bool TopDown)noexcept
{
	auto BitmapMemSource=iCreate<cBitmapDataSourceFromMemory>();
	if(BitmapMemSource->SetData(PixelFormat,Width,Height,PixelDataPitchSize,PixelData,TopDown)==false)
		return nullptr;
	return BitmapMemSource;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void cBitmapResampleBuffer::cSampler::Start(sfInt32,sfInt32)noexcept
{
}
//---------------------------------------------------------------------------
void cBitmapResampleBuffer::cSampler::Close(void)noexcept
{
}
//---------------------------------------------------------------------------
void cBitmapResampleBuffer::cSampler::Pixel(void*)noexcept
{
}
//---------------------------------------------------------------------------
void cBitmapResampleBuffer::cSampler::SetPixelFormat(cBitmapPixelFormat &Format)noexcept
{
	Format.ChannelCount=0;
	Format.ChannelSize=0;
	Format.ChannelColor=BitmapChannelColor::Unknown;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cBitmapResampleBuffer::cBitmapResampleBuffer()noexcept
{
	fUpdate=false;
	fBufferImageWidth=0;
	fBufferImageHeight=0;
	fBufferPixelFormat=BitmapPixelFormatUnknow;
	fSampler.ConstructAs<cSampler>();
}
//---------------------------------------------------------------------------
cBitmapResampleBuffer::~cBitmapResampleBuffer()noexcept
{
	fSampler->~cSampler();
}
//---------------------------------------------------------------------------
cUIPoint cBitmapResampleBuffer::GetImageSize(void)noexcept
{
	cUIPoint Ret;
	Ret.x=static_cast<Float32>(fBufferImageWidth);
	Ret.y=static_cast<Float32>(fBufferImageHeight);
	return Ret;
}
//---------------------------------------------------------------------------
cBitmapPixelFormat cBitmapResampleBuffer::GetPixelFormat(void)noexcept
{
	return fBufferPixelFormat;
}
//---------------------------------------------------------------------------
uIntn cBitmapResampleBuffer::CopyPixelBuffer(uIntn Offset,void *Dest,uIntn DestSize)noexcept
{
	if(fBufferLockCount==0){
		ProcessSource();
	}
	if(Offset>fBufferPixelData.Length)
		return 0;
	uIntn CopySize=DestSize;
	if(Offset+DestSize>fBufferPixelData.Length){
		CopySize=fBufferPixelData.Length-Offset;
	}
	if(CopySize!=0)
		cnMemory::Copy(Dest,cnMemory::PointerAddByteOffset(fBufferPixelData.Pointer,Offset),CopySize);
	return CopySize;
}
//---------------------------------------------------------------------------
const void* cBitmapResampleBuffer::AcquirePixels(void)noexcept
{
	if(fBufferLockCount==0){
		ProcessSource();
	}
	fBufferLockCount++;
	return fBufferPixelData.Pointer;
}
//---------------------------------------------------------------------------
void cBitmapResampleBuffer::ReleasePixels(void)noexcept
{
	if(fBufferLockCount!=0){
		fBufferLockCount--;
	}
}
//---------------------------------------------------------------------------
uIntn cBitmapResampleBuffer::GetDataPitch(void)noexcept
{
	return fBufferPitch;
}
//---------------------------------------------------------------------------
bool cBitmapResampleBuffer::IsTopDown(void)noexcept
{
	return TopDown;
}
//---------------------------------------------------------------------------
void cBitmapResampleBuffer::ProcessSource(void)noexcept
{
	if(fUpdate==false)
		return;
	fUpdate=false;

	fBufferImageWidth=fSampleImageWidth;
	fBufferImageHeight=fSampleImageHeight;
	fSampler->SetPixelFormat(fBufferPixelFormat);
	uIntn PixelSize=fBufferPixelFormat.ChannelSize*fBufferPixelFormat.ChannelCount;
	if(PixelSize==0 || fBufferImageWidth==0 || fBufferImageHeight==0){
		// empty image
		fBufferPitch=0;
		fBufferPixelData.SetLength(0);
		return;
	}
	fBufferPixelFormat.AlphaPremultiplied=AlphaPremultiplied;
	fBufferPixelFormat.ChannelColor=ChannelColor;

	fBufferPitch=fBufferImageWidth*PixelSize;
	fBufferPitch=cnMath::IntRoundPositive<4>(fBufferPitch);
	uIntn DataSize=fBufferPitch*fBufferImageHeight;
	fBufferPixelData.SetLength(DataSize);

	void *PixelRowData=fBufferPixelData.Pointer;

	fSampler->Start(fBufferImageWidth,fBufferImageHeight);

	for(sfInt32 y=0;y<fBufferImageHeight;y++){
		void *PixelCell=PixelRowData;
		for(sfInt32 x=0;x<fBufferImageWidth;x++){
			fSampler->Pixel(PixelCell);

			PixelCell=cnMemory::PointerAddByteOffset(PixelCell,PixelSize);
		}

		PixelRowData=cnMemory::PointerAddByteOffset(PixelRowData,fBufferPitch);
	}

	fSampler->Close();
}
//---------------------------------------------------------------------------
