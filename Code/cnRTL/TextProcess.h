/*- cnRTL - Text Process --------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2022-06-30                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnRTL_TextProcess_H__
#define __cnLibrary_cnRTL_TextProcess_H__
//---------------------------------------------------------------------------
#include <cnRTL/cnRTLHeader.h>
#include <cnRTL/String.h>
#include <cnRTL/InterfaceImplementation.h>
#ifdef __cplusplus
//---------------------------------------------------------------------------
#if cnLibrary_CPPFEATURE_CONSTEXPR >= 200704L
#define	cnRTL_CONSTVAR	constexpr
#define	cnRTL_CONSTEXPR_FUNC constexpr
// cnLibrary_CPPFEATURE_CONSTEXPR >= 200704L
#else
// cnLibrary_CPPFEATURE_CONSTEXPR < 200704L
#define	cnRTL_CONSTEXPR_FUNC
#define	cnRTL_CONSTVAR	const
#endif	// cnLibrary_CPPFEATURE_CONSTEXPR < 200704L
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnRTL{
//---------------------------------------------------------------------------
class cTextTranscoding_Copy : public iTextEncodingConverter
{
public:
	virtual uIntn cnLib_FUNC Convert(void *Dest,uIntn DestSize,const void *Src,uIntn SrcSize,uIntn *SrcConvertedSize)noexcept(true)override;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iUTF32Transcoding
{
public:
	// return size of converted source string
	virtual uIntn ToUTF32(uChar32 &Dest,const void *Src,uIntn SrcSize)const noexcept(true)=0;
	// return size of result in destination string
	virtual uIntn FromUTF32(void *Dest,uIntn DestSize,uChar32 Src)const noexcept(true)=0;
};
//---------------------------------------------------------------------------
uIntn TextTranscodingFromUTF32(const iUTF32Transcoding *ToTranscoding,void *Dest,uIntn DestSize,const void *Src,uIntn SrcSize,uIntn *SrcConvertedSize)noexcept(true);
uIntn TextTranscodingToUTF32(const iUTF32Transcoding *FromTranscoding,void *Dest,uIntn DestSize,const void *Src,uIntn SrcSize,uIntn *SrcConvertedSize)noexcept(true);
uIntn TextTranscodingThroughUTF32(const iUTF32Transcoding *FromTranscoding,const iUTF32Transcoding *ToTranscoding,void *Dest,uIntn DestSize,const void *Src,uIntn SrcSize,uIntn *SrcConvertedSize)noexcept(true);


//---------------------------------------------------------------------------
class cUTF32Transcoding_UTF16 : public iUTF32Transcoding
{
public:
	virtual uIntn ToUTF32(uChar32 &Dest,const void *Src,uIntn SrcSize)const noexcept(true)override;
	virtual uIntn FromUTF32(void *Dest,uIntn DestSize,uChar32 Src)const noexcept(true)override;
};
extern cUTF32Transcoding_UTF16 UTF32Transcoding_UTF16;
//---------------------------------------------------------------------------
class cUTF32Transcoding_UTF8 : public iUTF32Transcoding
{
public:
	virtual uIntn ToUTF32(uChar32 &Dest,const void *Src,uIntn SrcSize)const noexcept(true)override;
	virtual uIntn FromUTF32(void *Dest,uIntn DestSize,uChar32 Src)const noexcept(true)override;
};
extern cUTF32Transcoding_UTF8 UTF32Transcoding_UTF8;
//---------------------------------------------------------------------------

iTextEncodingConverter* UnicodeTranscoder(ufInt8 ToEncodingSize,ufInt8 FromEncodingSize)noexcept(true);

template<uIntn To,uIntn From>
class cUTFStringConverter
{
public:
	cUTFStringConverter()noexcept(true)
		: fConverter(UnicodeTranscoder(To,From)){}
	~cUTFStringConverter()noexcept(true){}

	operator iTextEncodingConverter* ()const noexcept(true){
		return fConverter;
	}
protected:
	iTextEncodingConverter *fConverter;
};
//---------------------------------------------------------------------------
class cTextTranscoding_FromUTF32 : public iTextEncodingConverter
{
public:
	cTextTranscoding_FromUTF32(iUTF32Transcoding *To)noexcept(true);
	virtual uIntn cnLib_FUNC Convert(void *Dest,uIntn DestSize,const void *Src,uIntn SrcSize,uIntn *SrcConvertedSize)noexcept(true)override;
private:
	iUTF32Transcoding *fTo;
};
//---------------------------------------------------------------------------
class cTextTranscoding_ToUTF32 : public iTextEncodingConverter
{
public:
	cTextTranscoding_ToUTF32(iUTF32Transcoding *From)noexcept(true);
	virtual uIntn cnLib_FUNC Convert(void *Dest,uIntn DestSize,const void *Src,uIntn SrcSize,uIntn *SrcConvertedSize)noexcept(true)override;
private:
	iUTF32Transcoding *fFrom;
};
//---------------------------------------------------------------------------
class cTextTranscoding_ThroughUTF32 : public iTextEncodingConverter
{
public:

	cTextTranscoding_ThroughUTF32(iUTF32Transcoding *From,iUTF32Transcoding *To)noexcept(true);
	virtual uIntn cnLib_FUNC Convert(void *Dest,uIntn DestSize,const void *Src,uIntn SrcSize,uIntn *SrcConvertedSize)noexcept(true)override;
private:
	iUTF32Transcoding *fFrom;
	iUTF32Transcoding *fTo;
};
//---------------------------------------------------------------------------
rPtr<iTextEncodingConverter> CreateTranscoderFromUTF32(iUTF32Transcoding *To)noexcept(true);
rPtr<iTextEncodingConverter> CreateTranscoderToUTF32(iUTF32Transcoding *From)noexcept(true);
rPtr<iTextEncodingConverter> CreateTranscoderThroughUTF32(iUTF32Transcoding *From,iUTF32Transcoding *To)noexcept(true);
//---------------------------------------------------------------------------
}	// namespace cnRTL
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#undef	cnRTL_CONSTEXPR_FUNC
#undef	cnRTL_CONSTVAR
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif
