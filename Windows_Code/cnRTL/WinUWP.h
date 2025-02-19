/*- cnRTL - Windows - UPW -------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2021-01-12                                          */
/*-------------------------------------------------------------------------*/
#pragma once
/*-------------------------------------------------------------------------*/
#include <cnRTL\WinSDKHeader.h>
#include <cnRTL\WinUWPSDKHeader.h>

#include <cnRTL\AsyncTask.h>
#include <cnRTL\WinCommon.h>
#include <cnRTL\WinCOM.h>

//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnRTL{
//---------------------------------------------------------------------------
namespace UWP{
//---------------------------------------------------------------------------
void COMRelaseAsync(COMPtr<IUnknown> Interface)noexcept(true);
//---------------------------------------------------------------------------
struct cHStringReference
{
	HSTRING_HEADER Header;
	HSTRING Handle;

	operator HSTRING()noexcept(true);

	cHStringReference()=default;
	cHStringReference(const cHStringReference&)=delete;

	template<uIntn Length>
	HRESULT Create(const wchar_t (&String)[Length])noexcept(true){
		return ::WindowsCreateStringReference(String,Length-1,&Header,&Handle);
	}

	HRESULT Create(const wchar_t *String,uIntn Length)noexcept(true);
	HRESULT Create(const wchar_t *String)noexcept(true);
};
//---------------------------------------------------------------------------
struct cHStringHandleReferenceOperator
{
	typedef wchar_t tCharacter;
	typedef HSTRING tStringToken;

	static const wchar_t* GetPointer(HSTRING StrRef)noexcept(true){
		UINT32 Length;
		return WindowsGetStringRawBuffer(StrRef,&Length);
	}
	static uIntn GetLength(const HSTRING &StrRef)noexcept(true){
		UINT32 Length;
		WindowsGetStringRawBuffer(StrRef,&Length);
		return Length;
	}

	static cArray<const wchar_t> GetArray(HSTRING StrRef)noexcept(true){
		UINT32 Length;
		cArray<const wchar_t> Array;
		Array.Pointer=WindowsGetStringRawBuffer(StrRef,&Length);
		Array.Length=Length;
		return Array;
	}

	static HSTRING Duplicate(HSTRING StrRef)noexcept(true){
		HSTRING Handle;
		if(SUCCEEDED(WindowsDuplicateString(StrRef,&Handle))){
			return Handle;
		}
		return nullptr;
	}
	static HSTRING Move(HSTRING &&StrRef)noexcept(true){
		HSTRING Handle=StrRef;
		StrRef=nullptr;
		return Handle;
	}


	static void Discard(HSTRING &StrRef)noexcept(true){
		WindowsDeleteString(StrRef);
	}

	static HSTRING MakeFrom(void)noexcept(true){
		return nullptr;
	}
	static HSTRING MakeFrom(HSTRING StringHandle)noexcept(true){
		return StringHandle;
	}

	static HSTRING MakeStringCopy(const tCharacter *String,uIntn Length)noexcept(true){
		HSTRING Handle;
		if(SUCCEEDED(WindowsCreateString(String,static_cast<UINT32>(Length),&Handle))){
			return Handle;
		}
		return nullptr;
	}



	static HSTRING MakeConstant(const cArrayConstant<wchar_t> &Array)noexcept(true){
		return MakeStringCopy(Array.Pointer,Array.Length);
	}

};

//---------------------------------------------------------------------------
class cHString : public cnString::cConstString< cHStringHandleReferenceOperator >
{
public:
	typedef cnString::cConstString< cHStringHandleReferenceOperator > TStringRef;

	using TStringRef::TStringRef;

	operator HSTRING()const noexcept(true){	return this->fStringToken;		}

	HSTRING* operator &()noexcept(true){
		if(this->fStringToken!=nullptr){
			WindowsDeleteString(this->fStringToken);
			this->fStringToken=nullptr;
		}

		return &this->fStringToken;
	}
};
//---------------------------------------------------------------------------
cStringBuffer<uChar16> CreateStringFromHandle(HSTRING String)noexcept(true);
cStringBuffer<wchar_t> CreateWStringFromHandle(HSTRING String)noexcept(true);
//---------------------------------------------------------------------------
class cUWPMemoryBufferWriteStreamBuffer : public iWriteBuffer<void>
{
public:
	cUWPMemoryBufferWriteStreamBuffer(COMPtr<ABI::Windows::Storage::Streams::IDataWriter> DataWriter)noexcept(true);
	~cUWPMemoryBufferWriteStreamBuffer()noexcept(true);

	virtual cMemory cnLib_FUNC ReserveWriteBuffer(uIntn Length)noexcept(true)override;
	virtual void cnLib_FUNC CommitWriteBuffer(uIntn Length)noexcept(true)override;

protected:
	cMemoryBlock fBuffer;
	COMPtr<ABI::Windows::Storage::Streams::IDataWriter> fDataWriter;
};
//---------------------------------------------------------------------------
#if WINDOWS_FOUNDATION_UNIVERSALAPICONTRACT_VERSION >=0x50000
//---------------------------------------------------------------------------
uInt8* GetBufferPtr(ABI::Windows::Storage::Streams::IBuffer *Buffer)noexcept(true);
cConstMemory GetBufferData(ABI::Windows::Storage::Streams::IBuffer *Buffer)noexcept(true);
COMPtr<ABI::Windows::Storage::Streams::IBuffer> MakeBufferFromData(const void *Data,uIntn Size)noexcept(true);
//---------------------------------------------------------------------------
template<class T>
class cCOMWeakReference : public IWeakReference
{
public:
	COMPtr<T> fFakeWeakRef;

	typedef TCOMInterfacePack<IWeakReference> tCOMInterfacePack;

    virtual HRESULT STDMETHODCALLTYPE Resolve( 
        /* [in] */ __RPC__in REFIID riid,
        /* [iid_is][out] */ __RPC__deref_out IInspectable **objectReference)override
	{
		return E_NOTIMPL;
		//auto Unknow=fFakeWeakRef->COMUnknown();
		//return Unknow->QueryInterface(riid,reinterpret_cast<void**>(objectReference));
	}
};
//---------------------------------------------------------------------------
class cUWPMemoryBuffer
	: public ABI::Windows::Storage::Streams::IBuffer, public Windows::Storage::Streams::IBufferByteAccess
	, public IWeakReferenceSource, public IAgileObject
	, public iWriteBuffer<void>
{
public:
	using ABI::Windows::Storage::Streams::IBuffer::AddRef;
	using ABI::Windows::Storage::Streams::IBuffer::Release;
	cMemoryBuffer Data;

	virtual cMemory cnLib_FUNC ReserveWriteBuffer(uIntn Length)noexcept(true)override;
	virtual void cnLib_FUNC CommitWriteBuffer(uIntn Length)noexcept(true)override;

	typedef TCOMInterfacePack<IAgileObject, IWeakReferenceSource
		, ABI::Windows::Storage::Streams::IBuffer, Windows::Storage::Streams::IBufferByteAccess
	> tCOMInterfacePack;

	virtual HRESULT STDMETHODCALLTYPE GetWeakReference( /* [retval][out] */ __RPC__deref_out_opt IWeakReference **weakReference)noexcept(true)override;

	virtual HRESULT STDMETHODCALLTYPE GetIids( 
        /* [out] */ __RPC__out ULONG *iidCount,
        /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids)noexcept(true)override;
        
	virtual HRESULT STDMETHODCALLTYPE GetRuntimeClassName( 
        /* [out] */ __RPC__deref_out_opt HSTRING *className)noexcept(true)override;
        
	virtual HRESULT STDMETHODCALLTYPE GetTrustLevel( 
        /* [out] */ __RPC__out TrustLevel *trustLevel)noexcept(true)override;

	// IBuffer
	virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Capacity(/* [out][retval] */ __RPC__out UINT32 *value)noexcept(true)override;
	virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Length(/* [out][retval] */ __RPC__out UINT32 *value)noexcept(true)override;
	virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_Length(/* [in] */ UINT32 value)noexcept(true)override;

	// IBufferByteAccess
	virtual HRESULT STDMETHODCALLTYPE Buffer(byte **value)noexcept(true)override;
};
//---------------------------------------------------------------------------
#endif	// WINDOWS_FOUNDATION_UNIVERSALAPICONTRACT_VERSION >=0xA0000
//---------------------------------------------------------------------------
template<class T>
class cAsyncOperationAwaiter
{
public:
	cAsyncOperationAwaiter(ABI::Windows::Foundation::IAsyncOperation<T> *Task)noexcept(true):fTask(Task){}
	~cAsyncOperationAwaiter()noexcept(true){}

	bool await_ready(void)const noexcept(true){
		COMPtr<ABI::Windows::Foundation::IAsyncInfo> AsyncInfo;
		fTask->QueryInterface(COMRetPtrT(AsyncInfo));
		if(AsyncInfo==nullptr)
			return true;

		ABI::Windows::Foundation::AsyncStatus Status;
		if(FAILED(AsyncInfo->get_Status(&Status))){
			return false;
		}
		return Status==ABI::Windows::Foundation::AsyncStatus::Completed;
	}
	template<class TCoHandle>
	bool await_suspend(TCoHandle&& CoHandle)noexcept(true){
		fHandler=COMCreate<cCompletionHandler>();
		cCoroutineHandleOperator::Assign(fHandler->CoHandle,static_cast<TCoHandle&&>(CoHandle));

		if(FAILED(fTask->put_Completed(fHandler))){
			return false;
		}
		return true;
	}
	ABI::Windows::Foundation::AsyncStatus await_resume(void)noexcept(true){
		return fHandler->RetStatus;
	}
private:
	COMPtr< ABI::Windows::Foundation::IAsyncOperation<T> > fTask;

	class cCompletionHandler : public ABI::Windows::Foundation::IAsyncOperationCompletedHandler<T>, public IAgileObject
	{
	public:
		using ABI::Windows::Foundation::IAsyncOperationCompletedHandler<T>::AddRef;
		using ABI::Windows::Foundation::IAsyncOperationCompletedHandler<T>::Release;

		ABI::Windows::Foundation::AsyncStatus RetStatus;
		cCoroutineHandleOperator::tHandle CoHandle;
        virtual HRESULT STDMETHODCALLTYPE Invoke(ABI::Windows::Foundation::IAsyncOperation<T> *,ABI::Windows::Foundation::AsyncStatus Status)noexcept(true)override{
			RetStatus=Status;
			cCoroutineHandleOperator::Resume(CoHandle);
			return S_OK;
		}

		typedef TCOMInterfacePack<IAgileObject
			, ABI::Windows::Foundation::IAsyncOperationCompletedHandler<T>
		> tCOMInterfacePack;
	};
	COMPtr< cCompletionHandler > fHandler;
};
//---------------------------------------------------------------------------
template<class T>
inline cAsyncOperationAwaiter<T> AsyncOperationAwaiter(ABI::Windows::Foundation::IAsyncOperation<T> *Task)noexcept(true)
{
	return cAsyncOperationAwaiter<T>(Task);
}
//---------------------------------------------------------------------------
template<class T>
inline cAsyncOperationAwaiter<T> AsyncOperationAwaiter(const COMPtr< ABI::Windows::Foundation::IAsyncOperation<T> > &Task)noexcept(true)
{
	return cAsyncOperationAwaiter<T>(Task);
}
//---------------------------------------------------------------------------
template<class TImplement,class TEventInvokeFunctionPtr,class TSenderDecl,class...TArgsDecl>
class cTypedEventHandlerImplementation;

template<class TImplement,class TClass,class TSender,class...TArgs,class TSenderDecl,class...TArgsDecl>
class cTypedEventHandlerImplementation<TImplement,void (TClass::*)(TSender,TArgs...)noexcept(true),TSenderDecl,TArgsDecl...> : public TImplement, public ABI::Windows::Foundation::ITypedEventHandler<TSenderDecl,TArgsDecl...>, public IAgileObject
{
public:

	typedef TCOMInterfacePack<ABI::Windows::Foundation::ITypedEventHandler<TSenderDecl,TArgsDecl...>
		,IAgileObject> tCOMInterfacePack;

	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,_COM_Outptr_ void __RPC_FAR *__RPC_FAR *ppvObject)noexcept(true)override{
		if(COMObjectQueryInterface(this,riid,ppvObject)){
			this->EventIncReference();
			return S_OK;
		}
		*ppvObject=nullptr;
		return E_NOINTERFACE;
	}


	virtual ULONG STDMETHODCALLTYPE AddRef(void)noexcept(true)override{
		this->EventIncReference();
		return 0;
	}
	virtual ULONG STDMETHODCALLTYPE Release(void)noexcept(true)override{
		this->EventDecReference();
		return 0;
	}
	virtual HRESULT STDMETHODCALLTYPE Invoke(TSender sender,TArgs...args)noexcept(true)override{
		this->EventInvoke(sender,args...);
		return S_OK;
	}
};
//---------------------------------------------------------------------------
template<class TImplement,class TSenderDecl,class...TArgsDecl>
using cTypedEventHandler=cTypedEventHandlerImplementation<TImplement,decltype(&TImplement::EventInvoke),TSenderDecl,TArgsDecl...>;
//---------------------------------------------------------------------------
template<class TImplement,class TResult>
class cAsyncOperationCompletedHandler : public TImplement, public ABI::Windows::Foundation::IAsyncOperationCompletedHandler<TResult>, public IAgileObject
{
public:

	typedef TCOMInterfacePack<ABI::Windows::Foundation::IAsyncOperationCompletedHandler<TResult>
		,IAgileObject> tCOMInterfacePack;

	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,_COM_Outptr_ void __RPC_FAR *__RPC_FAR *ppvObject)noexcept(true)override{
		if(COMObjectQueryInterface(this,riid,ppvObject)){
			this->EventIncReference();
			return S_OK;
		}
		*ppvObject=nullptr;
		return E_NOINTERFACE;
	}


	virtual ULONG STDMETHODCALLTYPE AddRef(void)noexcept(true)override{
		this->EventIncReference();
		return 0;
	}
	virtual ULONG STDMETHODCALLTYPE Release(void)noexcept(true)override{
		this->EventDecReference();
		return 0;
	}
	virtual HRESULT STDMETHODCALLTYPE Invoke(ABI::Windows::Foundation::IAsyncOperation<TResult> *AsyncOp,ABI::Windows::Foundation::AsyncStatus Status)noexcept(true)override{
		this->EventInvoke(AsyncOp,Status);
		return S_OK;
	}
};
//---------------------------------------------------------------------------
}	// namespace UWP
//---------------------------------------------------------------------------
}	// namespace cnRTL
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
