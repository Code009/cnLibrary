#include "WinUWP.h"

using namespace cnLibrary;
using namespace cnRTL;
using namespace cnRTL::UWP;

//---------------------------------------------------------------------------
namespace cnLibrary::cnRTL::UWP{
//---------------------------------------------------------------------------
class cCOMReleaseProcedure : public iProcedure,public bcRecyclableDisposable,public cRTLAllocator
{
public:
	COMPtr<IUnknown> Interface;

	virtual void cnLib_FUNC Execute(void)noexcept(true)override{
		Interface=nullptr;
	
		Dispose();
	}
};
//---------------------------------------------------------------------------
}	// namespace cnLibrary::cnRTL::UWP
//---------------------------------------------------------------------------
void UWP::COMRelaseAsync(COMPtr<IUnknown> Interface)noexcept
{
	typedef cnClass::RecyclableLifeCycleSharedManager< cCOMReleaseProcedure,cnClass::RecyclableObjectAllocator<cCOMReleaseProcedure> > tLifeCycleManager;

	auto *Manager=tLifeCycleManager::GetSharedManager();
	auto ReleaseProc=Manager->Query();

	ReleaseProc->Interface=cnVar::MoveCast(Interface);

	cnSystem::DefaultThreadPool->Execute(nullptr,ReleaseProc);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cHStringReference::operator HSTRING()noexcept
{
	return Handle;
}
//---------------------------------------------------------------------------
HRESULT cHStringReference::Create(const wchar_t *String,uIntn Length)noexcept
{
	return ::WindowsCreateStringReference(String,Length,&Header,&Handle);
}
//---------------------------------------------------------------------------
HRESULT cHStringReference::Create(const wchar_t *String)noexcept
{
	uIntn Length=cnString::FindLength(String);
	return ::WindowsCreateStringReference(String,Length,&Header,&Handle);
}
//---------------------------------------------------------------------------
cStringBuffer<uChar16> UWP::CreateStringFromHandle(HSTRING StringHandle)noexcept
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
cStringBuffer<wchar_t> UWP::CreateWStringFromHandle(HSTRING StringHandle)noexcept
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
uInt8* UWP::GetBufferPtr(ABI::Windows::Storage::Streams::IBuffer *Buffer)noexcept
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
cConstMemory UWP::GetBufferData(ABI::Windows::Storage::Streams::IBuffer *Buffer)noexcept
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
COMPtr<ABI::Windows::Storage::Streams::IBuffer> UWP::MakeBufferFromData(const void *Data,uIntn Size)noexcept
{
	HRESULT hr;
	COMPtr<ABI::Windows::Storage::Streams::IDataWriter> DataWriter;
	cHString RCName_DataWriter=RuntimeClass_Windows_Storage_Streams_DataWriter;
	hr=Windows::Foundation::ActivateInstance(RCName_DataWriter,COMRetPtrT(DataWriter));
	if(FAILED(hr)){
		return nullptr;
	}
	
	hr=DataWriter->WriteBytes(Size,static_cast<BYTE*>(const_cast<void*>(Data)));
	if(FAILED(hr))
		return nullptr;
	
	COMPtr<ABI::Windows::Storage::Streams::IBuffer> DataBuffer;
	hr=DataWriter->DetachBuffer(COMRetPtrT(DataBuffer));
	if(FAILED(hr))
		return nullptr;

	return DataBuffer;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cUWPMemoryBufferWriteStreamBuffer::cUWPMemoryBufferWriteStreamBuffer(COMPtr<ABI::Windows::Storage::Streams::IDataWriter> DataWriter)noexcept
	: fDataWriter(cnVar::MoveCast(DataWriter))
{
}
//---------------------------------------------------------------------------
cUWPMemoryBufferWriteStreamBuffer::~cUWPMemoryBufferWriteStreamBuffer()noexcept
{
}
//---------------------------------------------------------------------------
cMemory cUWPMemoryBufferWriteStreamBuffer::ReserveWriteBuffer(uIntn Length)noexcept
{
	if(fBuffer.Length<Length){
		fBuffer.SetLength(Length);
	}

	return fBuffer;
}
//---------------------------------------------------------------------------
void cUWPMemoryBufferWriteStreamBuffer::CommitWriteBuffer(uIntn Length)noexcept
{
	fDataWriter->WriteBytes(Length,static_cast<BYTE*>(fBuffer.Pointer));
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cUWPMemoryBuffer::GetWeakReference( /* [retval][out] */ __RPC__deref_out_opt IWeakReference **weakReference)noexcept
{
	auto WeakRef=COMCreate<cCOMWeakReference<cUWPMemoryBuffer> >();
	WeakRef->fFakeWeakRef=this;
	*weakReference=COMExtract(WeakRef);
	return S_OK;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cUWPMemoryBuffer::GetIids( 
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids)noexcept
{
	*iidCount=0;
	return S_OK;
}
HRESULT STDMETHODCALLTYPE cUWPMemoryBuffer::GetRuntimeClassName( 
    /* [out] */ __RPC__deref_out_opt HSTRING *className)noexcept
{
	return WindowsCreateString(InterfaceName_Windows_Storage_Streams_IBuffer,ArrayLength(InterfaceName_Windows_Storage_Streams_IBuffer)-1,className);
}
HRESULT STDMETHODCALLTYPE cUWPMemoryBuffer::GetTrustLevel( 
    /* [out] */ __RPC__out TrustLevel *trustLevel)noexcept
{
	*trustLevel=BaseTrust;
	return S_OK;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cUWPMemoryBuffer::get_Capacity(/* [out][retval] */ __RPC__out UINT32 *value)noexcept
{
	*value=static_cast<UINT32>(Data.GetCapacity());
	return S_OK;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cUWPMemoryBuffer::get_Length(/* [out][retval] */ __RPC__out UINT32 *value)noexcept
{
	*value=static_cast<UINT32>(Data.GetSize());
	return S_OK;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cUWPMemoryBuffer::put_Length(/* [in] */ UINT32 value)noexcept
{
	Data.SetSize(static_cast<uIntn>(value));
	return S_OK;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cUWPMemoryBuffer::Buffer(byte **value)noexcept
{
	*value=reinterpret_cast<byte*>(Data.GetPointer());
	return S_OK;
}
//---------------------------------------------------------------------------
cMemory cUWPMemoryBuffer::ReserveWriteBuffer(uIntn Length)noexcept
{
	uIntn Capacity=Data.GetCapacity();
	uIntn Size=Data.GetSize();
	uIntn SizeRemain=Capacity-Size;
	if(SizeRemain<Length){
		Data.SetCapacity(Size+Length);
	}
	cMemory RetMem;
	RetMem.Pointer=Data.GetPointer(Size);
	Capacity=Data.GetCapacity();
	RetMem.Length=Capacity-Size;
	return RetMem;
}
//---------------------------------------------------------------------------
void cUWPMemoryBuffer::CommitWriteBuffer(uIntn Length)noexcept
{
	uIntn Capacity=Data.GetCapacity();
	uIntn Size=Data.GetSize();
	uIntn SizeRemain=Capacity-Size;
	if(SizeRemain<Length){
		Data.SetSize(Capacity);
	}
	else{
		Data.SetSize(Size+Length);
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#endif	// WINDOWS_FOUNDATION_UNIVERSALAPICONTRACT_VERSION >=0x50000
//---------------------------------------------------------------------------
