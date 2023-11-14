#include "Mac_Locale.h"

using namespace cnLibrary;
using namespace cnRTL;
using namespace cnMac;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cTextEncodingConvetThrough::cTextEncodingConvetThrough(iTextEncodingConverter *From,iTextEncodingConverter *To)
	:fFrom(From),fTo(To)
{
}
//---------------------------------------------------------------------------
uIntn cTextEncodingConvetThrough::Convert(void *Dest,uIntn DestSize,const void *Src,uIntn SrcSize,uIntn *SrcConvertedSize)
{
	static constexpr uIntn TempBufferTotalSize=32;
	uIntn SrcIndex=0;
	uIntn DestIndex=0;
	uIntn BufferRemainSize=0;
	while(SrcIndex<SrcSize && DestIndex<DestSize){
		uInt8 Buffer[TempBufferTotalSize];

		cnLib_ASSERT(BufferRemainSize<TempBufferTotalSize);

		uIntn SrcConvertedSize;
		uIntn TempBufferSize=fFrom->Convert(Buffer+BufferRemainSize,TempBufferTotalSize-BufferRemainSize,cnMemory::PointerAddByteOffset(Src,SrcIndex),SrcSize-SrcIndex,&SrcConvertedSize);
		if(TempBufferSize==0){
			break;
		}
		TempBufferSize+=BufferRemainSize;

		uIntn BufferConvertedSize;
		uIntn DestConvertedSize=fTo->Convert(cnMemory::PointerAddByteOffset(Dest,DestIndex),DestSize-DestIndex,Buffer,TempBufferSize,&BufferConvertedSize);
		if(DestConvertedSize==0)
			break;
		SrcIndex+=SrcConvertedSize;
		DestIndex+=DestConvertedSize;

		if(BufferConvertedSize<TempBufferSize){
			BufferRemainSize=TempBufferSize-BufferConvertedSize;
			cnMemory::Copy(Buffer,Buffer+BufferConvertedSize,BufferRemainSize);
		}
		else{
			BufferRemainSize=0;
		}
	}
	if(SrcConvertedSize!=nullptr){
		*SrcConvertedSize=SrcIndex;
	}
	return DestIndex;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cCFStringUTF16FromEncoding::cCFStringUTF16FromEncoding()
{
}
//---------------------------------------------------------------------------
cCFStringUTF16FromEncoding::~cCFStringUTF16FromEncoding()
{
}
//---------------------------------------------------------------------------
uIntn cCFStringUTF16FromEncoding::Convert(void *Dest,uIntn DestSize,const void *Src,uIntn SrcSize,uIntn *SrcConvertedSize)
{
	auto TempStr=CFStringCreateWithBytesNoCopy(NULL,static_cast<const UInt8*>(Src),SrcSize,FromEncoding,false,kCFAllocatorNull);
	CFRange range={0,CFStringGetLength(TempStr)};
	CFIndex DestLength=static_cast<CFIndex>(DestSize/2);
	if(range.length>DestLength)
		range.length=DestLength;

	CFStringGetCharacters(TempStr,range,reinterpret_cast<UniChar*>(Dest));

	if(SrcConvertedSize!=nullptr){
		CFIndex DestSizeCopied;
		CFStringGetBytes(TempStr,range,FromEncoding,'?',false,nullptr,0,&DestSizeCopied);
		*SrcConvertedSize=DestSizeCopied;
	}

	CFRelease(TempStr);
	return range.length*2;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cCFStringUTF16ToEncoding::cCFStringUTF16ToEncoding()
{
}
//---------------------------------------------------------------------------
cCFStringUTF16ToEncoding::~cCFStringUTF16ToEncoding()
{
}
//---------------------------------------------------------------------------
uIntn cCFStringUTF16ToEncoding::Convert(void *Dest,uIntn DestSize,const void *Src,uIntn SrcSize,uIntn *SrcConvertedSize)
{
	CFIndex CFStrLength=static_cast<CFIndex>(SrcSize/2);
	auto TempStr=CFStringCreateWithCharactersNoCopy(NULL,reinterpret_cast<const UniChar*>(Src),CFStrLength,kCFAllocatorNull);
	CFIndex DestSizeCopied;
	CFRange range={0,CFStrLength};
	CFIndex SrcLengthConverted=CFStringGetBytes(TempStr,range,ToEncoding,'?',false,static_cast<UInt8*>(Dest),DestSize,&DestSizeCopied);
	if(SrcConvertedSize!=nullptr){
		*SrcConvertedSize=SrcLengthConverted*2;
	}
	CFRelease(TempStr);
	return DestSizeCopied;
}
//---------------------------------------------------------------------------

