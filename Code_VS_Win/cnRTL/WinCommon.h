/*- cnRTL - Windows - Common ----------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2009-02-10                                          */
/*-------------------------------------------------------------------------*/
#pragma once
/*-------------------------------------------------------------------------*/
#include <cnRTL\WinSDKHeader.h>

#include <cnSystem\cnWindows.h>
#include <cnRTL\cnRTL.h>

#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnRTL{
//---------------------------------------------------------------------------
sInt32 SysHandleToValue(HANDLE Handle);
HANDLE SysHandleFromValue(sInt32 Value);
//---------------------------------------------------------------------------
uChar16 *wtou(wchar_t *Str);
const uChar16 *wtou(const wchar_t *Str);
//---------------------------------------------------------------------------
wchar_t *utow(uChar16 *Str);
const wchar_t *utow(const uChar16 *Str);
//---------------------------------------------------------------------------
namespace cnWinRTL{
//---------------------------------------------------------------------------
class cTextEncodingConverter_UnicodeToMultiByte : public iTextEncodingConverter
{
public:
	UINT DestCodePage;
	virtual uIntn cnLib_FUNC Convert(void *Dest,uIntn DestSize,const void *Src,uIntn SrcSize,uIntn *SrcConvertedSize)override;
};
//---------------------------------------------------------------------------
class cTextEncodingConverter_MultiByteToUnicode : public iTextEncodingConverter
{
public:
	UINT SrcCodePage;
	virtual uIntn cnLib_FUNC Convert(void *Dest,uIntn DestSize,const void *Src,uIntn SrcSize,uIntn *SrcConvertedSize)override;
};
//---------------------------------------------------------------------------
struct cWin32CodePageConvertBuffer
{
	cStringBuffer<char> Result;
	cStringBuffer<wchar_t> WideCharTemp;
	void CopyUTF8(const uChar8 *Text,uIntn Length);
	void TranslateWideChar(const wchar_t *Text,uIntn Length);
	void TranslateUTF8(const uChar8 *Text,uIntn Length);
	void TranslateUTF16(const uChar16 *Text,uIntn Length);
};
//---------------------------------------------------------------------------
// for test
class cTimeMeter
{
private:
	LARGE_INTEGER fCheckPoint;
public:
	cTimeMeter();
	~cTimeMeter();
	void Reset(void);
	double Check(void);
	int Check(int Scale);
};

template<uIntn TestCount,class TFunc>
__declspec(noinline) void PerformanceCompareTest_Loop(TFunc&& Func){
	for(uIntn i=0;i<TestCount;i++){
		Func();
	}
}

template<uIntn TestCount,class TFunc1,class TFunc2>
__declspec(noinline) void PerformanceCompareTest(TFunc1&& Func1,TFunc2&& Func2){
	cTimeMeter tm;
	tm.Reset();
	PerformanceCompareTest_Loop<TestCount>(Func1);

	cStringBuffer<wchar_t> msg;

	auto sec=tm.Check();

	StringStream::WriteFormatString(msg.StreamWriteBuffer(),L"func1 %f\n",sec);
	OutputDebugStringW(msg->Pointer);

	msg.Clear();

	tm.Reset();
	PerformanceCompareTest_Loop<TestCount>(Func2);
	sec=tm.Check();

	StringStream::WriteFormatString(msg.StreamWriteBuffer(),L"func2 %f\n",sec);
	OutputDebugStringW(msg->Pointer);
}
//---------------------------------------------------------------------------
}	// namespace cnWinRTL
//---------------------------------------------------------------------------
}	// namespace cnRTL
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
