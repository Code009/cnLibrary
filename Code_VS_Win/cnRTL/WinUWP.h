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
struct cHStringReference
{
	HSTRING_HEADER Header;
	HSTRING Handle;

	operator HSTRING();

	cHStringReference()=default;
	cHStringReference(const cHStringReference&)=delete;

	template<uIntn Length>
	HRESULT Create(const wchar_t (&String)[Length]){
		return ::WindowsCreateStringReference(String,Length-1,&Header,&Handle);
	}

	HRESULT Create(const wchar_t *String,uIntn Length);
	HRESULT Create(const wchar_t *String);
};
//---------------------------------------------------------------------------
struct cHStringHandleReferenceOperator
{
	typedef wchar_t tCharacter;
	typedef HSTRING tStringToken;

	static const wchar_t* GetPointer(HSTRING StrRef){
		UINT32 Length;
		return WindowsGetStringRawBuffer(StrRef,&Length);
	}
	static uIntn GetLength(const HSTRING &StrRef){
		UINT32 Length;
		WindowsGetStringRawBuffer(StrRef,&Length);
		return Length;
	}

	static cArray<const wchar_t> GetArray(HSTRING StrRef){
		UINT32 Length;
		cArray<const wchar_t> Array;
		Array.Pointer=WindowsGetStringRawBuffer(StrRef,&Length);
		Array.Length=Length;
		return Array;
	}

	static HSTRING Duplicate(HSTRING StrRef){
		HSTRING Handle;
		if(SUCCEEDED(WindowsDuplicateString(StrRef,&Handle))){
			return Handle;
		}
		return nullptr;
	}
	static HSTRING Move(HSTRING &&StrRef){
		HSTRING Handle=StrRef;
		StrRef=nullptr;
		return Handle;
	}


	static void Discard(HSTRING &StrRef){
		WindowsDeleteString(StrRef);
	}

	static HSTRING MakeFrom(void){
		return nullptr;
	}
	static HSTRING MakeFrom(HSTRING StringHandle){
		return StringHandle;
	}

	static HSTRING MakeStringCopy(const tCharacter *String,uIntn Length){
		HSTRING Handle;
		if(SUCCEEDED(WindowsCreateString(String,static_cast<UINT32>(Length),&Handle))){
			return Handle;
		}
		return nullptr;
	}



	static HSTRING MakeConstant(const cArrayConstant<wchar_t> &Array){
		return MakeStringCopy(Array.Pointer,Array.Length);
	}

};

//---------------------------------------------------------------------------
class cHString : public cnString::cConstString< cHStringHandleReferenceOperator >
{
public:
	typedef cnString::cConstString< cHStringHandleReferenceOperator > TStringRef;

	using TStringRef::TStringRef;

	operator HSTRING()const{	return this->fStringToken;		}

	HSTRING* operator &(){
		if(this->fStringToken!=nullptr){
			WindowsDeleteString(this->fStringToken);
			this->fStringToken=nullptr;
		}

		return &this->fStringToken;
	}
};
//---------------------------------------------------------------------------
cStringBuffer<uChar16> CreateStringFromHandle(HSTRING String);
cStringBuffer<wchar_t> CreateWStringFromHandle(HSTRING String);
//---------------------------------------------------------------------------
#if WINDOWS_FOUNDATION_UNIVERSALAPICONTRACT_VERSION >=0x50000
//---------------------------------------------------------------------------
uInt8* GetBufferPtr(ABI::Windows::Storage::Streams::IBuffer *Buffer);
cConstMemory GetBufferData(ABI::Windows::Storage::Streams::IBuffer *Buffer);
//---------------------------------------------------------------------------
#endif	// WINDOWS_FOUNDATION_UNIVERSALAPICONTRACT_VERSION >=0xA0000
//---------------------------------------------------------------------------
template<class T>
struct TWindowsAsyncOperationResult;
//---------------------------------------------------------------------------
template<class T>
struct TWindowsAsyncOperationResult<T *>
{
	typedef T RetType;
	static T* Out(RetType & Ret){
		return &Ret;
	}
};
//---------------------------------------------------------------------------
template<class T>
struct TWindowsAsyncOperationResult<T **>
{
	typedef COMPtr<T> RetType;
	static T** Out(RetType & Ret){
		return COMRetPtrT(Ret);
	}
};
//---------------------------------------------------------------------------
template<class T>
class cAsyncOperationAwaiter
{
	typedef typename TCOMFunctionInfo<decltype(&ABI::Windows::Foundation::IAsyncOperation<T>::GetResults)>::template Arguments<0>::Type TResuleOutParam;
	typedef typename TWindowsAsyncOperationResult<TResuleOutParam>::RetType TResult;
public:
	cAsyncOperationAwaiter(ABI::Windows::Foundation::IAsyncOperation<T> *Task):fTask(Task){}
	~cAsyncOperationAwaiter(){}

	bool await_ready(void)const{
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
	bool await_suspend(TCoHandle&& CoHandle){
		fHandler=COMCreate<cCompletionHandler>();
		fHandler->CoHandle=static_cast<TCoHandle&&>(CoHandle);

		if(FAILED(fTask->put_Completed(fHandler))){
			return false;
		}
		return true;
	}
	TResult await_resume(void){
		TResult Result;
		fTask->GetResults(TWindowsAsyncOperationResult<TResuleOutParam>::Out(Result));
		return Result;
	}
private:
	COMPtr< ABI::Windows::Foundation::IAsyncOperation<T> > fTask;

	class cCompletionHandler : public ABI::Windows::Foundation::IAsyncOperationCompletedHandler<T>
	{
	public:
		vtCoHandle CoHandle;
        virtual HRESULT STDMETHODCALLTYPE Invoke(ABI::Windows::Foundation::IAsyncOperation<T> *,ABI::Windows::Foundation::AsyncStatus)override{
			CoHandle.Resume();
			return S_OK;
		}

		IUnknown* COMUnknown(void){	return this;	}
		bool COMQueryInterface( REFIID riid,_COM_Outptr_ void **ppvObject){
			if(riid==__uuidof(ABI::Windows::Foundation::IAsyncOperationCompletedHandler<T>)){
				*ppvObject=this;
				return true;
			}
			return false;
		}

	};
	COMPtr< cCompletionHandler > fHandler;
};
//---------------------------------------------------------------------------
template<class T>
inline cAsyncOperationAwaiter<T> AsyncOperationAwaiter(ABI::Windows::Foundation::IAsyncOperation<T> *Task)
{
	return cAsyncOperationAwaiter<T>(Task);
}
//---------------------------------------------------------------------------
template<class T>
inline cAsyncOperationAwaiter<T> AsyncOperationAwaiter(const COMPtr< ABI::Windows::Foundation::IAsyncOperation<T> > &Task)
{
	return cAsyncOperationAwaiter<T>(Task);
}
//---------------------------------------------------------------------------
#if 0
class cMemoryStreamBuffer : public ABI::Windows::Storage::Streams::IBuffer, public Windows::Storage::Streams::IBufferByteAccess
{
public:

	// IBuffer
	virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Capacity(/* [out][retval] */ __RPC__out UINT32 *value)override;
	virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Length(/* [out][retval] */ __RPC__out UINT32 *value)override;                        
	virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_Length(/* [in] */ UINT32 value)override;

	// IBufferByteAccess
	virtual HRESULT STDMETHODCALLTYPE Buffer(byte **value)override;
};
#endif // 0
//---------------------------------------------------------------------------
template<class TImplement,class TSender,class TEventInvokeFunctionPtr>
class cEventImplementTypedEventHandler;

template<class TImplement,class TSender,class TClass,class TSenderArg,class...TArgs>
class cEventImplementTypedEventHandler<TImplement,TSender,void (TClass::*)(TSenderArg,TArgs...)> : public TClass, public ABI::Windows::Foundation::ITypedEventHandler<TSender*,TArgs...>
{
public:
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,_COM_Outptr_ void __RPC_FAR *__RPC_FAR *ppvObject)override{
		if(riid==__uuidof(IUnknown)){
			this->EventIncReference();
			*ppvObject=static_cast<IUnknown*>(this);
			return S_OK;
		}
		if(riid==__uuidof(ABI::Windows::Foundation::ITypedEventHandler<TSender*,TArgs...>)){
			this->EventIncReference();
			*ppvObject=static_cast<ABI::Windows::Foundation::ITypedEventHandler<TSender*,TArgs...>*>(this);
			return S_OK;
		}
		*ppvObject=nullptr;
		return E_NOINTERFACE;
	}


	virtual ULONG STDMETHODCALLTYPE AddRef(void)override{
		this->EventIncReference();
		return 0;
	}
	virtual ULONG STDMETHODCALLTYPE Release(void)override{
		this->EventDecReference();
		return 0;
	}
	virtual HRESULT STDMETHODCALLTYPE Invoke(TSenderArg sender,TArgs...args)override{
		this->EventInvoke(sender,args...);
		return S_OK;
	}
};
//---------------------------------------------------------------------------
template<class TImplement,class TSender>
using cTypedEventHandler=cEventImplementTypedEventHandler<TImplement,TSender,decltype(&TImplement::EventInvoke)>;
//---------------------------------------------------------------------------
template<class TInterface,class TUWPResult>
class cInterfaceFromAsyncOperation : public TInterface, protected ABI::Windows::Foundation::IAsyncOperationCompletedHandler<TUWPResult*>
{
public:
	cInterfaceFromAsyncOperation(){}
	~cInterfaceFromAsyncOperation(){}
		
	bool SetupAsyncOp(COMPtr< ABI::Windows::Foundation::IAsyncOperation<TUWPResult*> > AsyncOp){
		if(fAsyncOp!=nullptr){
			return false;
		}

		fAsyncOp=cnVar::MoveCast(AsyncOp);
		HRESULT hr=fAsyncOp->put_Completed(this);
		if(FAILED(hr)){
			fAsyncOp=nullptr;
			return false;
		}
		return true;
	}
	
	void CancelAsyncOp(void){
		IAsyncInfo *AsyncInfo;
		fAsyncOp->QueryInterface(&AsyncInfo);
		if(AsyncInfo!=nullptr){
			AsyncInfo->Cancel();
			AsyncInfo->Release();
		}
	}


protected:
	COMPtr< ABI::Windows::Foundation::IAsyncOperation<TUWPResult*> > fAsyncOp;
	cAsyncTaskState fTaskState;


	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,_COM_Outptr_ void __RPC_FAR *__RPC_FAR *ppvObject)override{
		if(riid==__uuidof(IUnknown)){
			iIncReference(this,'evet');
			*ppvObject=static_cast<IUnknown*>(this);
			return S_OK;
		}
		if(riid==__uuidof(ABI::Windows::Foundation::IAsyncOperationCompletedHandler<TUWPResult*>)){
			iIncReference(this,'evet');
			*ppvObject=static_cast<ABI::Windows::Foundation::IAsyncOperationCompletedHandler<TUWPResult*>*>(this);
			return S_OK;
		}
		*ppvObject=nullptr;
		return E_NOINTERFACE;
	}

	virtual ULONG STDMETHODCALLTYPE AddRef(void)override{
		iIncReference(this,'evet');
		return 0;
	}

	virtual ULONG STDMETHODCALLTYPE Release(void)override{
		iDecReference(this,'evet');
		return 0;
	}
};
//---------------------------------------------------------------------------
}	// namespace UWP
//---------------------------------------------------------------------------
}	// namespace cnRTL
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
