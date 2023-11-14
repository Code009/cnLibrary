
#include "WinCommon.h"

using namespace cnLibrary;
using namespace cnRTL;
using namespace cnWinRTL;


//---------------------------------------------------------------------------
sInt32 cnRTL::SysHandleToValue(HANDLE Handle)
{
#ifdef _WIN64
	return static_cast<sInt32>(reinterpret_cast<sInt64>(Handle));
#else
	return reinterpret_cast<sInt32>(Handle);
#endif
}
//---------------------------------------------------------------------------
HANDLE cnRTL::SysHandleFromValue(sInt32 Value)
{
#ifdef _WIN64
	return reinterpret_cast<HANDLE>(static_cast<sInt64>(Value));
#else
	return reinterpret_cast<HANDLE>(Value);
#endif
}
//---------------------------------------------------------------------------
uChar16 *cnRTL::wtou(wchar_t *Str)
{
	return reinterpret_cast<uChar16*>(Str);
}
const uChar16 *cnRTL::wtou(const wchar_t *Str)
{
	return reinterpret_cast<const uChar16*>(Str);
}
//---------------------------------------------------------------------------
wchar_t *cnRTL::utow(uChar16 *Str)
{
	return reinterpret_cast<wchar_t*>(Str);
}
const wchar_t *cnRTL::utow(const uChar16 *Str)
{
	return reinterpret_cast<const wchar_t*>(Str);
}
//---------------------------------------------------------------------------
uIntn cTextEncodingConverter_UnicodeToMultiByte::Convert(void *Dest,uIntn DestSize,const void *Src,uIntn SrcSize,uIntn *SrcConvertedSize)
{
	int DestConvertedSize=::WideCharToMultiByte(DestCodePage,0,static_cast<const wchar_t*>(Src),static_cast<int>(SrcSize/2),static_cast<char*>(Dest),static_cast<int>(DestSize),NULL,NULL);
	if(DestConvertedSize==0){
		// error
		if(SrcConvertedSize!=nullptr)
			*SrcConvertedSize=0;
		return 0;
	}
	if(SrcConvertedSize!=nullptr){
		*SrcConvertedSize=static_cast<uIntn>(2)*::MultiByteToWideChar(DestCodePage,0,static_cast<const char*>(Dest),DestConvertedSize,nullptr,0);
	}
	return DestConvertedSize;
}
//---------------------------------------------------------------------------
uIntn cTextEncodingConverter_MultiByteToUnicode::Convert(void *Dest,uIntn DestSize,const void *Src,uIntn SrcSize,uIntn *SrcConvertedSize)
{
	int DestConvertedLength=::MultiByteToWideChar(SrcCodePage,0,static_cast<const char*>(Src),static_cast<int>(SrcSize),static_cast<wchar_t*>(Dest),static_cast<int>(DestSize/2));
	if(DestConvertedLength==0){
		// error
		if(SrcConvertedSize!=nullptr)
			*SrcConvertedSize=0;
		return 0;
	}
	if(SrcConvertedSize!=nullptr){
		*SrcConvertedSize=::WideCharToMultiByte(SrcCodePage,0,static_cast<wchar_t*>(Dest),DestConvertedLength,nullptr,0,NULL,NULL);
	}
	return static_cast<uIntn>(DestConvertedLength)*2;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void cWin32CodePageConvertBuffer::CopyUTF8(const uChar8 *Text,uIntn Length)
{
	Result.SetString(reinterpret_cast<const char*>(Text),Length);
}
//---------------------------------------------------------------------------
void cWin32CodePageConvertBuffer::TranslateWideChar(const wchar_t *Text,uIntn Length)
{
	int CharLength=::WideCharToMultiByte(CP_ACP,0,Text,static_cast<int>(Length),nullptr,0,nullptr,nullptr);
	if(CharLength<=0){
		Result.SetLength(0);
		return;
	}
	Result.SetLength(CharLength);
	CharLength=::WideCharToMultiByte(CP_ACP,0,Text,static_cast<int>(Length),Result->Pointer,static_cast<int>(Result->Length),nullptr,nullptr);
	if(CharLength<=0){
		Result.SetLength(0);
		return;
	}
	Result.SetLength(CharLength);
}
//---------------------------------------------------------------------------
void cWin32CodePageConvertBuffer::TranslateUTF8(const uChar8 *Text,uIntn Length)
{
	int WideCharLength=::MultiByteToWideChar(CP_UTF8,0,reinterpret_cast<const char*>(Text),static_cast<int>(Length),nullptr,0);
	if(WideCharLength<=0){
		WideCharTemp.SetLength(0);
		return;
	}
	WideCharTemp.SetLength(WideCharLength);
	WideCharLength=::MultiByteToWideChar(CP_UTF8,0,reinterpret_cast<const char*>(Text),static_cast<int>(Length),WideCharTemp->Pointer,WideCharLength);
	
	return TranslateWideChar(WideCharTemp->Pointer,WideCharLength);
}
//---------------------------------------------------------------------------
void cWin32CodePageConvertBuffer::TranslateUTF16(const uChar16 *Text,uIntn Length)
{
	return TranslateWideChar(utow(Text),Length);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cTimeMeter::cTimeMeter()
{
	Reset();
}
//---------------------------------------------------------------------------
cTimeMeter::~cTimeMeter()
{
}
//---------------------------------------------------------------------------
void cTimeMeter::Reset(void)
{
	::QueryPerformanceCounter(&fCheckPoint);
}
//---------------------------------------------------------------------------
double cTimeMeter::Check(void)
{
	LARGE_INTEGER Now;
	LARGE_INTEGER PF;
	::QueryPerformanceCounter(&Now);
	::QueryPerformanceFrequency(&PF);
	auto d=(Now.QuadPart-fCheckPoint.QuadPart);
	return double(d)/PF.QuadPart;
}
//---------------------------------------------------------------------------
int cTimeMeter::Check(int Scale)
{
	LARGE_INTEGER Now;
	LARGE_INTEGER PF;
	::QueryPerformanceCounter(&Now);
	::QueryPerformanceFrequency(&PF);
	auto d=(Now.QuadPart-fCheckPoint.QuadPart);
	return static_cast<int>(d*Scale/PF.QuadPart);
}
//---------------------------------------------------------------------------
