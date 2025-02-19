/*- cnRTL - Windows - Common ----------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2009-02-10                                          */
/*-------------------------------------------------------------------------*/
#pragma once
/*-------------------------------------------------------------------------*/
#include <cnRTL\WinSDKHeader.h>

#include <cnSystem\cnWindows.h>
#include <cnRTL\RuntimeFunction.h>
#include <cnRTL\String.h>
#include <cnRTL\ObjectReference.h>

#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnRTL{
//---------------------------------------------------------------------------
sInt32 SysHandleToValue(HANDLE Handle)noexcept(true);
HANDLE SysHandleFromValue(sInt32 Value)noexcept(true);
//---------------------------------------------------------------------------
uChar16 *wtou(wchar_t *Str)noexcept(true);
const uChar16 *wtou(const wchar_t *Str)noexcept(true);
//---------------------------------------------------------------------------
wchar_t *utow(uChar16 *Str)noexcept(true);
const wchar_t *utow(const uChar16 *Str)noexcept(true);
//---------------------------------------------------------------------------
namespace cnWinRTL{
//---------------------------------------------------------------------------
template<HANDLE InvalidHandleValue>
struct bcWin32Handle
{
	HANDLE Handle;

	bcWin32Handle()noexcept(true):Handle(InvalidHandleValue){}
	bcWin32Handle(HANDLE InitalHandle)noexcept(true):Handle(InitalHandle){}
	~bcWin32Handle()noexcept(true){
		if(Handle!=InvalidHandleValue){
			::CloseHandle(Handle);
		}
	}

	void TransferHandle(HANDLE NewHandle)noexcept(true){
		if(Handle!=InvalidHandleValue){
			::CloseHandle(Handle);
		}
		Handle=NewHandle;
	}
	void Close(void)noexcept(true){
		TransferHandle(InvalidHandleValue);
	}
};
//---------------------------------------------------------------------------
typedef bcWin32Handle<INVALID_HANDLE_VALUE> cWin32Handle;
typedef bcWin32Handle<nullptr> cWin32PointerHandle;
//---------------------------------------------------------------------------
class cTextEncodingConverter_UnicodeToMultiByte : public iTextEncodingConverter
{
public:
	UINT DestCodePage;
	virtual uIntn cnLib_FUNC Convert(void *Dest,uIntn DestSize,const void *Src,uIntn SrcSize,uIntn *SrcConvertedSize)noexcept(true)override;
};
//---------------------------------------------------------------------------
class cTextEncodingConverter_MultiByteToUnicode : public iTextEncodingConverter
{
public:
	UINT SrcCodePage;
	virtual uIntn cnLib_FUNC Convert(void *Dest,uIntn DestSize,const void *Src,uIntn SrcSize,uIntn *SrcConvertedSize)noexcept(true)override;
};
//---------------------------------------------------------------------------
struct cWin32CodePageConvertBuffer
{
	cStringBuffer<char> Result;
	cStringBuffer<wchar_t> WideCharTemp;
	void CopyUTF8(const uChar8 *Text,uIntn Length)noexcept(true);
	void TranslateWideChar(const wchar_t *Text,uIntn Length)noexcept(true);
	void TranslateUTF8(const uChar8 *Text,uIntn Length)noexcept(true);
	void TranslateUTF16(const uChar16 *Text,uIntn Length)noexcept(true);
};
//---------------------------------------------------------------------------
// for test
class cTimeMeter
{
private:
	LARGE_INTEGER fCheckPoint;
public:
	cTimeMeter()noexcept(true);
	~cTimeMeter()noexcept(true);
	void Reset(void)noexcept(true);
	double Check(void)noexcept(true);
	int Check(int Scale)noexcept(true);
};

template<uIntn TestCount,class TFunc>
__declspec(noinline) void PerformanceCompareTest_Loop(TFunc&& Func)noexcept(true){
	for(uIntn i=0;i<TestCount;i++){
		Func();
	}
}

template<uIntn TestCount,class TFunc1,class TFunc2>
__declspec(noinline) void PerformanceCompareTest(TFunc1&& Func1,TFunc2&& Func2)noexcept(true){
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
