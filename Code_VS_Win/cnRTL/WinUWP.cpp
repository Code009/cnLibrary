#include "WinUWP.h"

using namespace cnLibrary;
using namespace cnRTL;
using namespace cnRTL::UWP;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cHStringReference::operator HSTRING()
{
	return Handle;
}
//---------------------------------------------------------------------------
HRESULT cHStringReference::Create(const wchar_t *String,uIntn Length)
{
	return ::WindowsCreateStringReference(String,Length,&Header,&Handle);
}
//---------------------------------------------------------------------------
HRESULT cHStringReference::Create(const wchar_t *String)
{
	uIntn Length=cnString::GetLength(String);
	return ::WindowsCreateStringReference(String,Length,&Header,&Handle);
}
//---------------------------------------------------------------------------
cStringBuffer<uChar16> UWP::CreateStringFromHandle(HSTRING StringHandle)
{
	cStringBuffer<uChar16> RetString;
	UINT StringLength;
	auto StringBuffer=::WindowsGetStringRawBuffer(StringHandle,&StringLength);
	if(StringBuffer!=nullptr){
		RetString.SetString(wtou(StringBuffer),StringLength);
	}
	return RetString;
}
//---------------------------------------------------------------------------
cStringBuffer<wchar_t> UWP::CreateWStringFromHandle(HSTRING StringHandle)
{
	cStringBuffer<wchar_t> RetString;
	UINT StringLength;
	auto StringBuffer=::WindowsGetStringRawBuffer(StringHandle,&StringLength);
	if(StringBuffer!=nullptr){
		RetString.SetString(StringBuffer,StringLength);
	}
	return RetString;
}
//---------------------------------------------------------------------------
#if WINDOWS_FOUNDATION_UNIVERSALAPICONTRACT_VERSION >=0x50000
//---------------------------------------------------------------------------
uInt8* UWP::GetBufferPtr(ABI::Windows::Storage::Streams::IBuffer *Buffer)
{
	HRESULT hr;
	byte *DataPtr;
	::Windows::Storage::Streams::IBufferByteAccess *BufferAccess;
	
	hr=Buffer->QueryInterface(&BufferAccess);
	if(FAILED(hr) || BufferAccess==nullptr)
		return nullptr;
	hr=BufferAccess->Buffer(&DataPtr);

	BufferAccess->Release();
	if(FAILED(hr))
		return nullptr;
	return DataPtr;
}
//---------------------------------------------------------------------------
cConstMemory UWP::GetBufferData(ABI::Windows::Storage::Streams::IBuffer *Buffer)
{
	HRESULT hr;
	UINT32 DataLen;
	hr=Buffer->get_Length(&DataLen);
	if(FAILED(hr))
		return NullArray;
	
	cConstMemory Memory;
	Memory.Pointer=GetBufferPtr(Buffer);

	if(Memory.Pointer==nullptr){
		Memory.Length=0;
	}
	else{
		Memory.Length=DataLen;
	}
	return Memory;
}
//---------------------------------------------------------------------------
#endif	// WINDOWS_FOUNDATION_UNIVERSALAPICONTRACT_VERSION >=0x50000
//---------------------------------------------------------------------------
