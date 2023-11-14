//---------------------------------------------------------------------------
#include "TextProcess.h"

using namespace cnLibrary;
using namespace cnRTL;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
uIntn cTextTranscoding_Copy::Convert(void *Dest,uIntn DestSize,const void *Src,uIntn SrcSize,uIntn *SrcConvertedSize)
{
	uIntn CopySize=DestSize;
	if(CopySize>SrcSize)
		CopySize=SrcSize;
	if(CopySize!=0){
		cnMemory::Copy(Dest,Src,CopySize);
	}
	if(SrcConvertedSize!=nullptr){
		*SrcConvertedSize=CopySize;
	}
	return CopySize;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
uIntn cnRTL::TextTranscodingFromUTF32(const iUTF32Transcoding *ToTranscoding,void *Dest,uIntn DestSize,const void *Src,uIntn SrcSize,uIntn *SrcConvertedSize)
{
	auto pSrc=static_cast<const uChar32*>(Src);
	uIntn SrcLength=SrcSize/4;
	uIntn DestOffset=0;
	uIntn SrcIndex;
	for(SrcIndex=0;SrcIndex<SrcLength;SrcIndex++){
		if(DestOffset>=DestSize)
			break;
		uIntn DestConvertedPartSize=ToTranscoding->FromUTF32(cnMemory::PointerAddByteOffset(Dest,DestOffset),DestSize-DestOffset,pSrc[SrcIndex]);
		if(DestConvertedPartSize==0){
			break;
		}
		DestOffset+=DestConvertedPartSize;
	}
	if(SrcConvertedSize!=nullptr){
		*SrcConvertedSize=SrcIndex*4;
	}
	return DestOffset;
}
//---------------------------------------------------------------------------
uIntn cnRTL::TextTranscodingToUTF32(const iUTF32Transcoding *FromTranscoding,void *Dest,uIntn DestSize,const void *Src,uIntn SrcSize,uIntn *SrcConvertedSize)
{
	auto pDest=static_cast<uChar32*>(Dest);
	uIntn DestLength=DestSize/4;
	uIntn SrcOffset=0;
	uIntn DestIndex;
	for(DestIndex=0;DestIndex<DestLength;DestIndex++){
		if(SrcOffset>=SrcSize)
			break;

		uIntn SrcConvertedPartSize=FromTranscoding->ToUTF32(pDest[DestIndex],cnMemory::PointerAddByteOffset(Src,SrcOffset),SrcSize-SrcOffset);
		if(SrcConvertedPartSize==0){
			break;
		}
		SrcOffset+=SrcConvertedPartSize;
	}
	if(SrcConvertedSize!=nullptr){
		*SrcConvertedSize=SrcOffset;
	}
	return DestIndex*4;
}
//---------------------------------------------------------------------------
uIntn cnRTL::TextTranscodingThroughUTF32(const iUTF32Transcoding *FromTranscoding,const iUTF32Transcoding *ToTranscoding,void *Dest,uIntn DestSize,const void *Src,uIntn SrcSize,uIntn *SrcConvertedSize)
{
	uIntn SrcIndex=0;
	uIntn DestIndex=0;
	while(SrcIndex<SrcSize && DestIndex<DestSize){
		uChar32 c32;

		uIntn SrcConvertedPartSize=FromTranscoding->ToUTF32(c32,cnMemory::PointerAddByteOffset(Src,SrcIndex),SrcSize-SrcIndex);
		if(SrcConvertedPartSize==0){
			break;
		}
		
		uIntn DestConvertedPartSize=ToTranscoding->FromUTF32(cnMemory::PointerAddByteOffset(Dest,DestIndex),DestSize-DestIndex,c32);
		if(DestConvertedPartSize==0)
			break;
		SrcIndex+=SrcConvertedPartSize;
		DestIndex+=DestConvertedPartSize;
	}
	if(SrcConvertedSize!=nullptr){
		*SrcConvertedSize=SrcIndex;
	}
	return DestIndex;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
uIntn cUTF32Transcoding_UTF16::ToUTF32(uChar32 &Dest,const void *Src,uIntn SrcSize)const
{
	uIntn SrcConvertedLength=UnicodeChar16To32(Dest,static_cast<const uChar16*>(Src),SrcSize/sizeof(uChar16));
	return SrcConvertedLength*sizeof(uChar16);
}
//---------------------------------------------------------------------------
uIntn cUTF32Transcoding_UTF16::FromUTF32(void *Dest,uIntn DestSize,uChar32 Src)const
{
	uIntn DestResultLength=UnicodeChar16From32(static_cast<uChar16*>(Dest),DestSize/sizeof(uChar16),Src);
	return DestResultLength*sizeof(uChar16);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
uIntn cUTF32Transcoding_UTF8::ToUTF32(uChar32 &Dest,const void *Src,uIntn SrcSize)const
{
	return UnicodeChar8To32(Dest,static_cast<const uChar8*>(Src),SrcSize);
}
//---------------------------------------------------------------------------
uIntn cUTF32Transcoding_UTF8::FromUTF32(void *Dest,uIntn DestSize,uChar32 Src)const
{
	return UnicodeChar8From32(static_cast<uChar8*>(Dest),DestSize,Src);
}
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnRTL{
//---------------------------------------------------------------------------
class cTextTranscoding_Unicode_32From8 : public iTextEncodingConverter
{
public:
	virtual uIntn cnLib_FUNC Convert(void *Dest,uIntn DestSize,const void *Src,uIntn SrcSize,uIntn *SrcConvertedSize)override{
		auto Transcoding=cnVar::StaticInitializedConstSinglton<cUTF32Transcoding_UTF8>();
		return TextTranscodingToUTF32(Transcoding,Dest,DestSize,Src,SrcSize,SrcConvertedSize);
	}
};
//---------------------------------------------------------------------------
class cTextTranscoding_Unicode_32From16 : public iTextEncodingConverter
{
public:
	virtual uIntn cnLib_FUNC Convert(void *Dest,uIntn DestSize,const void *Src,uIntn SrcSize,uIntn *SrcConvertedSize)override{
		auto Transcoding=cnVar::StaticInitializedConstSinglton<cUTF32Transcoding_UTF16>();
		return TextTranscodingToUTF32(Transcoding,Dest,DestSize,Src,SrcSize,SrcConvertedSize);
	}
};
//---------------------------------------------------------------------------
class cTextTranscoding_Unicode_32To8 : public iTextEncodingConverter
{
public:
	virtual uIntn cnLib_FUNC Convert(void *Dest,uIntn DestSize,const void *Src,uIntn SrcSize,uIntn *SrcConvertedSize)override{
		auto Transcoding=cnVar::StaticInitializedConstSinglton<cUTF32Transcoding_UTF8>();
		return TextTranscodingFromUTF32(Transcoding,Dest,DestSize,Src,SrcSize,SrcConvertedSize);
	}
};
//---------------------------------------------------------------------------
class cTextTranscoding_Unicode_32To16 : public iTextEncodingConverter
{
public:
	virtual uIntn cnLib_FUNC Convert(void *Dest,uIntn DestSize,const void *Src,uIntn SrcSize,uIntn *SrcConvertedSize)override{
		auto Transcoding=cnVar::StaticInitializedConstSinglton<cUTF32Transcoding_UTF16>();
		return TextTranscodingFromUTF32(Transcoding,Dest,DestSize,Src,SrcSize,SrcConvertedSize);
	}
};
//---------------------------------------------------------------------------
class cTextTranscoding_Unicode_8To16 : public iTextEncodingConverter
{
public:
	virtual uIntn cnLib_FUNC Convert(void *Dest,uIntn DestSize,const void *Src,uIntn SrcSize,uIntn *SrcConvertedSize)override{
		auto Transcoding8=cnVar::StaticInitializedConstSinglton<cUTF32Transcoding_UTF8>();
		auto Transcoding16=cnVar::StaticInitializedConstSinglton<cUTF32Transcoding_UTF16>();
		return TextTranscodingThroughUTF32(Transcoding8,Transcoding16,Dest,DestSize,Src,SrcSize,SrcConvertedSize);
	}
};
//---------------------------------------------------------------------------
class cTextTranscoding_Unicode_16To8 : public iTextEncodingConverter
{
public:
	virtual uIntn cnLib_FUNC Convert(void *Dest,uIntn DestSize,const void *Src,uIntn SrcSize,uIntn *SrcConvertedSize)override{
		auto Transcoding8=cnVar::StaticInitializedConstSinglton<cUTF32Transcoding_UTF8>();
		auto Transcoding16=cnVar::StaticInitializedConstSinglton<cUTF32Transcoding_UTF16>();
		return TextTranscodingThroughUTF32(Transcoding16,Transcoding8,Dest,DestSize,Src,SrcSize,SrcConvertedSize);
	}
};
//---------------------------------------------------------------------------
static const void* UnicodeTranscoderMap[4][4]=
{
	{	// From 1
		&cnVar::cStaticInitializedConstSinglton< cReferenceStaticImplementT<cTextTranscoding_Copy> >::gInstance,			// To 1
		&cnVar::cStaticInitializedConstSinglton< cReferenceStaticImplementT<cTextTranscoding_Unicode_8To16> >::gInstance,	// To 1
		&cnVar::cStaticInitializedConstSinglton< cReferenceStaticImplementT<cTextTranscoding_Unicode_32From8> >::gInstance,	// To 4
		nullptr,	// To Invalid
	},
	{	// From 2
		&cnVar::cStaticInitializedConstSinglton< cReferenceStaticImplementT<cTextTranscoding_Unicode_16To8> >::gInstance,	// To 1
		&cnVar::cStaticInitializedConstSinglton< cReferenceStaticImplementT<cTextTranscoding_Copy> >::gInstance,			// To 2
		&cnVar::cStaticInitializedConstSinglton< cReferenceStaticImplementT<cTextTranscoding_Unicode_32From16> >::gInstance,// To 4
		nullptr,	// To Invalid
	},
	{	// From 4
		&cnVar::cStaticInitializedConstSinglton< cReferenceStaticImplementT<cTextTranscoding_Unicode_32To8> >::gInstance,	// To 1
		&cnVar::cStaticInitializedConstSinglton< cReferenceStaticImplementT<cTextTranscoding_Unicode_32To16> >::gInstance,	// To 2
		&cnVar::cStaticInitializedConstSinglton< cReferenceStaticImplementT<cTextTranscoding_Copy> >::gInstance,			// To 4
		nullptr,	// To Invalid
	},
	{	// From Invalid
		nullptr,	// To 1
		nullptr,	// To 2
		nullptr,	// To 4
		nullptr,	// To Invalid
	},
};
//---------------------------------------------------------------------------
static const ufInt8 UnicodeTranscoderEncodingSizeMap[5]={
	3,	// 0 : invalid size
	0,	// 1 : 2^0
	1,	// 2 : 2^1
	3,	// 3 : invalid size
	2,	// 4 : 2^2
};
//---------------------------------------------------------------------------
}	// namespace cnRTL
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
iTextEncodingConverter* cnRTL::UnicodeTranscoder(ufInt8 ToEncodingSize,ufInt8 FromEncodingSize)
{
	if(ToEncodingSize>4 || FromEncodingSize>4)
		return nullptr;

	cnVar::cStaticInitializedConstSinglton< cReferenceStaticImplementT<cTextTranscoding_Copy> >::Check();
	cnVar::cStaticInitializedConstSinglton< cReferenceStaticImplementT<cTextTranscoding_Unicode_8To16> >::Check();
	cnVar::cStaticInitializedConstSinglton< cReferenceStaticImplementT<cTextTranscoding_Unicode_16To8> >::Check();
	cnVar::cStaticInitializedConstSinglton< cReferenceStaticImplementT<cTextTranscoding_Unicode_32From8> >::Check();
	cnVar::cStaticInitializedConstSinglton< cReferenceStaticImplementT<cTextTranscoding_Unicode_32From16> >::Check();
	cnVar::cStaticInitializedConstSinglton< cReferenceStaticImplementT<cTextTranscoding_Unicode_32To8> >::Check();
	cnVar::cStaticInitializedConstSinglton< cReferenceStaticImplementT<cTextTranscoding_Unicode_32To16> >::Check();

	auto Transcoder=UnicodeTranscoderMap[UnicodeTranscoderEncodingSizeMap[FromEncodingSize]][UnicodeTranscoderEncodingSizeMap[ToEncodingSize]];
	return static_cast<iTextEncodingConverter*>(const_cast<void*>(Transcoder));
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cTextTranscoding_FromUTF32::cTextTranscoding_FromUTF32(iUTF32Transcoding *To)
	: fTo(To)
{
}
//---------------------------------------------------------------------------
uIntn cTextTranscoding_FromUTF32::Convert(void *Dest,uIntn DestSize,const void *Src,uIntn SrcSize,uIntn *SrcConvertedSize)
{
	return TextTranscodingFromUTF32(fTo,Dest,DestSize,Src,SrcSize,SrcConvertedSize);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cTextTranscoding_ToUTF32::cTextTranscoding_ToUTF32(iUTF32Transcoding *From)
	: fFrom(From)
{
}
//---------------------------------------------------------------------------
uIntn cTextTranscoding_ToUTF32::Convert(void *Dest,uIntn DestSize,const void *Src,uIntn SrcSize,uIntn *SrcConvertedSize)
{
	return TextTranscodingToUTF32(fFrom,Dest,DestSize,Src,SrcSize,SrcConvertedSize);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cTextTranscoding_ThroughUTF32::cTextTranscoding_ThroughUTF32(iUTF32Transcoding *From,iUTF32Transcoding *To)
	:fFrom(From),fTo(To)
{
}
//---------------------------------------------------------------------------
uIntn cTextTranscoding_ThroughUTF32::Convert(void *Dest,uIntn DestSize,const void *Src,uIntn SrcSize,uIntn *SrcConvertedSize)
{
	uIntn SrcIndex=0;
	uIntn DestIndex=0;
	while(SrcIndex<SrcSize && DestIndex<DestSize){
		uChar32 c32;

		uIntn SrcConvertedPartSize=fFrom->ToUTF32(c32,cnMemory::PointerAddByteOffset(Src,SrcIndex),SrcSize-SrcIndex);
		if(SrcConvertedPartSize==0){
			break;
		}
		
		uIntn DestConvertedPartSize=fTo->FromUTF32(cnMemory::PointerAddByteOffset(Dest,DestIndex),DestSize-DestIndex,c32);
		if(DestConvertedPartSize==0)
			break;
		SrcIndex+=SrcConvertedPartSize;
		DestIndex+=DestConvertedPartSize;
	}
	if(SrcConvertedSize!=nullptr){
		*SrcConvertedSize=SrcIndex;
	}
	return DestIndex;
}
//---------------------------------------------------------------------------
rPtr<iTextEncodingConverter> cnRTL::CreateTranscoderFromUTF32(iUTF32Transcoding *To)
{
	return rCreate<cTextTranscoding_FromUTF32>(To);
}
//---------------------------------------------------------------------------
rPtr<iTextEncodingConverter> cnRTL::CreateTranscoderToUTF32(iUTF32Transcoding *From)
{
	return rCreate<cTextTranscoding_ToUTF32>(From);
}
//---------------------------------------------------------------------------
rPtr<iTextEncodingConverter> cnRTL::CreateTranscoderThroughUTF32(iUTF32Transcoding *From,iUTF32Transcoding *To)
{
	return rCreate<cTextTranscoding_ThroughUTF32>(From,To);
}
//---------------------------------------------------------------------------
