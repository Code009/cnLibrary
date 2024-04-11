/*- cnRTL - Windows - COM -------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2009-02-10                                          */
/*-------------------------------------------------------------------------*/
#pragma once
/*-------------------------------------------------------------------------*/
#include <cnRTL\WinThread.h>

//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnRTL{
//---------------------------------------------------------------------------
struct cBSTROwnerTokenOperator : cnVar::bcPointerOwnerTokenOperator<BSTR>
{
	static void Release(const BSTR &Token)noexcept(true);
};
typedef cnVar::cPtrOwner<cBSTROwnerTokenOperator> apBSTR;
//---------------------------------------------------------------------------
apBSTR MakeBSTR(const wchar_t *Text)noexcept(true);
BSTR* apBSTRRetPtr(apBSTR &Ptr)noexcept(true);
//---------------------------------------------------------------------------
inline IUnknown* iCastCOM(iInterface *Interface)noexcept(true){
	auto COMInterface=iCast<iCOMInterface>(Interface);
	if(COMInterface==nullptr)
		return nullptr;
	return COMInterface->GetCOMInterface();
}
//---------------------------------------------------------------------------
template<class T>
bool impCOMQueryInterface(T*,REFIID,_COM_Outptr_ void **)noexcept(true)
{
	return false;
}
//---------------------------------------------------------------------------
template<class TInterface,class...VT,class T>
bool impCOMQueryInterface(T *Object,REFIID riid,_COM_Outptr_ void **ppvObject)noexcept(true)
{
	if(__uuidof(TInterface)==riid){
		*ppvObject=static_cast<TInterface*>(Object);
		return true;
	}
	return impCOMQueryInterface<VT...>(Object,riid,ppvObject);
}
//---------------------------------------------------------------------------
template<class TFunction>
struct TCOMFunctionInfo;

template<class TClass,class TRet,class...TArgs>
struct TCOMFunctionInfo<TRet (STDMETHODCALLTYPE TClass::*)(TArgs...)>
	: cnLib_THelper::Var_TH::MemberFunctionInfo<TClass,TRet,TArgs...>
{
	typedef TClass ClassArgumentType;
};
//---------------------------------------------------------------------------
template<class T>
struct cCOMRefTokenOperator : cnVar::bcPointerRefTokenOperator<T*>
{
	static void Acquire(T *Token)noexcept(true){	if(Token!=nullptr)	Token->AddRef();	}
	static void Release(T *Token)noexcept(true){	if(Token!=nullptr)	Token->Release();	}
};

template<class T>
using COMPtr=cnVar::cPtrReference< cCOMRefTokenOperator<T> >;

//---------------------------------------------------------------------------
template<class T>
inline COMPtr<T> COMTake(T *Src)noexcept(true)
{
	return COMPtr<T>::TakeFromManual(Src);
}
//---------------------------------------------------------------------------
template<class T>
inline T* COMExtract(COMPtr<T> &Src)noexcept(true)
{
	return Src.ExtractToManual();
}
//---------------------------------------------------------------------------
template<class TDest,class T>
inline TDest* COMExtractAs(COMPtr<T> &Src)noexcept(true)
{
	return static_cast<TDest*>(Src.ExtractToManual());
}
//---------------------------------------------------------------------------
template<class T>
inline COMPtr<T> COMTake(T *Src,uInt32 Tag)noexcept(true)
{
	cnRTL_DEBUG_LOG_REFERENCE_DEC(Src,Tag);
	return COMPtr<T>::TakeFromManual(Src);
}
//---------------------------------------------------------------------------
template<class T>
inline T* COMExtract(COMPtr<T> &Src,uInt32 Tag)noexcept(true)
{
	cnRTL_DEBUG_LOG_REFERENCE_INC(static_cast<T*>(Src),Tag);
	return Src.ExtractToManual();
}
//---------------------------------------------------------------------------
template<class TDest,class T>
inline T* COMExtractAs(COMPtr<T> &Src,uInt32 Tag)noexcept(true)
{
	cnRTL_DEBUG_LOG_REFERENCE_INC(static_cast<T*>(Src),Tag);
	return static_cast<TDest*>(Src.ExtractToManual());
}
//---------------------------------------------------------------------------
template<class T>
inline void** COMRetPtr(T* &Ptr)noexcept(true){
	return reinterpret_cast<void**>(&Ptr);
}
//---------------------------------------------------------------------------
template<class T>
inline void** COMRetPtr(COMPtr<T> &Ptr)noexcept(true){
	Ptr=nullptr;
	return reinterpret_cast<void**>(&Ptr.Token());
}
//---------------------------------------------------------------------------
template<class T>
inline T** COMRetPtrT(T* &Ptr)noexcept(true){
	return &Ptr;
}
//---------------------------------------------------------------------------
template<class T>
inline T** COMRetPtrT(COMPtr<T> &Ptr)noexcept(true){
	Ptr=nullptr;
	return &Ptr.Token();
}
//---------------------------------------------------------------------------
template<class T>
inline REFIID COMUUID(T* &)noexcept(true){
	return __uuidof(T);
}
//---------------------------------------------------------------------------
template<class T>
inline REFIID COMUUID(const COMPtr<T>&)noexcept(true){
	return __uuidof(T);
}
//---------------------------------------------------------------------------
// COM
//---------------------------------------------------------------------------
template<class TDestInterface,class T>
inline COMPtr<TDestInterface> COMQueryInterface(const T &Src)noexcept(true)
{
	if(Src==nullptr)
		return nullptr;
	COMPtr<TDestInterface> ci;
	HRESULT hr = Src->QueryInterface(__uuidof(TDestInterface),COMRetPtr(ci));
	if(hr!=S_OK)
		return nullptr;
	return ci;
}
//---------------------------------------------------------------------------
}	// namespace cnRTL
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
namespace cnLib_THelper{
namespace RTL_TH{
//---------------------------------------------------------------------------
template<class TCOMInterface>
class cCOMImplementation : public TCOMInterface, public cnRTL::cRTLAllocator
{
private:
	cnRTL::cAtomicVar<uIntn> fRefCount=1;
public:
	using TCOMInterface::TCOMInterface;
	~cCOMImplementation()=default;

	virtual HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid,_COM_Outptr_ void **ppvObject)noexcept(true)override{
		if(riid==IID_IUnknown){
			*ppvObject=TCOMInterface::COMUnknown();
			fRefCount.Free++;
			return S_OK;
		}
		if(TCOMInterface::COMQueryInterface(riid,ppvObject)){
			fRefCount.Free++;
			return S_OK;
		}
		*ppvObject=nullptr;
		return E_NOINTERFACE;
	}

	virtual ULONG STDMETHODCALLTYPE AddRef(void)noexcept(true)override{
		fRefCount.Free++;
		return 0;
	}

	virtual ULONG STDMETHODCALLTYPE Release(void)noexcept(true)override{
		if(fRefCount.Free--==1){
			delete this;
		}
		return 0;
	}
};
//---------------------------------------------------------------------------
}	// namespace RTL_TH
}	// namespace cnLib_THelper
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnRTL{
//---------------------------------------------------------------------------
template<class T>
inline COMPtr<T> COMCreate(void)noexcept(true){
	return COMPtr<T>::TakeFromManual(new cnLib_THelper::RTL_TH::cCOMImplementation<T>);
}
template<class T,class...TArgs>
inline COMPtr<T> COMCreate(TArgs&&...Args)noexcept(true){
	return COMPtr<T>::TakeFromManual(new cnLib_THelper::RTL_TH::cCOMImplementation<T>(cnVar::Forward<TArgs>(Args)...));
}
//---------------------------------------------------------------------------
template<class T>
class COMInnerObject : public T
{
private:
public:
	IUnknown *Outter;

	using T::T;

    virtual HRESULT STDMETHODCALLTYPE QueryInterface(
        /* [in] */ REFIID riid,
        /* [iid_is][out] */ _COM_Outptr_ void **ppvObject)noexcept(true)
	{
		return Outter->QueryInterface(riid,ppvObject);
	}

    virtual ULONG STDMETHODCALLTYPE AddRef(void)noexcept(true){
		return Outter->AddRef();
	}

    virtual ULONG STDMETHODCALLTYPE Release(void)noexcept(true){
		return Outter->Release();
	}
};
//---------------------------------------------------------------------------
class iCOMInnerReference
{
public:
	virtual void COMDelete(void)noexcept(true)=0;
	virtual bool COMInnerQueryInterface(REFIID riid,void **ppvObject)noexcept(true)=0;
};
//---------------------------------------------------------------------------
}	// namespace cnRTL
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
namespace cnLib_THelper{
namespace RTL_TH{
//---------------------------------------------------------------------------
template<class TCOMInterface>
class cCOMInnerImplementation : public cnRTL::COMInnerObject<TCOMInterface>, public cnRTL::iCOMInnerReference, public cnRTL::cRTLAllocator
{
private:
	cnRTL::cAtomicVar<uIntn> fRefCount=1;
public:
	using cnRTL::COMInnerObject<TCOMInterface>::COMInnerObject;
	~cCOMInnerImplementation()=default;

	virtual void COMDelete(void)noexcept(true)override{
		delete this;
	}
	virtual bool COMInnerQueryInterface(REFIID riid,void **ppvObject)noexcept(true)override{
		return TCOMInterface::COMQueryInterface(riid,ppvObject);
	}
};
//---------------------------------------------------------------------------
}	// namespace RTL_TH
}	// namespace cnLib_THelper
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnRTL{
//---------------------------------------------------------------------------
template<class T>
struct cCOMInnerOwnerToken
{
	T *Pointer;
	iCOMInnerReference *Reference;
};

template<class T>
struct cCOMInnerOwnerTokenOperator
{
	typedef T *TPtr;
	typedef cCOMInnerOwnerToken<T> TOwnerToken;

	static T* Pointer(const TOwnerToken &Token)noexcept(true){	return Token.Pointer;	}

	static TOwnerToken TokenNull(void)noexcept(true){
		TOwnerToken RetToke={nullptr,nullptr};
		return RetToke;
	}
	template<class TImp>
	static TOwnerToken TokenFrom(cnLib_THelper::RTL_TH::cCOMInnerImplementation<TImp> *p)noexcept(true){
		TOwnerToken RetToke={p,p};
		return RetToke;
	}

	template<class TSrc>
	static typename cnVar::TTypeConditional<TOwnerToken,
		cnVar::TIsAssignableFrom<T*&,TSrc*>::Value
	>::Type TokenFrom(cCOMInnerOwnerToken<TSrc> SrcToken)noexcept(true){
		TOwnerToken RetToken;
		RetToken.Pointer=SrcToken.Pointer;
		RetToken.Reference=SrcToken.Reference;
		return RetToken;
	}

	static void Release(const TOwnerToken &Token)noexcept(true){
		if(Token.Reference!=nullptr)
			Token.Reference->COMDelete();
	}
};
//---------------------------------------------------------------------------
template<class T>
using COMInnerPtr = cnVar::cPtrOwner< cCOMInnerOwnerTokenOperator<T> >;
//---------------------------------------------------------------------------
template<class T>
inline COMInnerPtr<T> COMInnerPtrCreate(IUnknown *Outter)noexcept(true){
	auto Ptr=new cnLib_THelper::RTL_TH::cCOMInnerImplementation<T>;
	Ptr->Outter=Outter;
	return COMInnerPtr<T>::Take(Ptr);
}
//---------------------------------------------------------------------------
template<class T>
inline bool COMInnerPtrQueryInterface(const COMInnerPtr<T> &Ptr,REFIID riid,void **ppvObject)noexcept(true){
	auto Ref=Ptr.Token().Reference;
	return Ref->COMInnerQueryInterface(riid,ppvObject);
}
//---------------------------------------------------------------------------
namespace cnWinRTL{
//---------------------------------------------------------------------------
class cOLEModule
{
public:
	cOLEModule(const wchar_t *ModuleName)noexcept(true);
	~cOLEModule()noexcept(true);

	HRESULT QueryFactory(REFCLSID rclsid,IClassFactory **Factory)noexcept(true);
	HRESULT CreateObject(REFCLSID rclsid,IUnknown *pUnkOuter, REFIID riid,LPVOID *ppvObj)noexcept(true);
	


	COMPtr<IClassFactory> QueryFactory(REFCLSID rclsid)noexcept(true);

	template<class T>
	COMPtr<T> CreateObject(REFCLSID rclsid)noexcept(true){
		HRESULT hr;

		COMPtr<T> Object;
		hr=CreateObject(rclsid,nullptr,__uuidof(T),COMRetPtr(Object));
		if(FAILED(hr)){
			return nullptr;
		}

		return Object;
	}
	template<class T>
	COMPtr<T> CreateAggregateObject(REFCLSID rclsid,IUnknown *pUnkOuter)noexcept(true){
		HRESULT hr;

		COMPtr<T> Object;
		hr=CreateObject(rclsid,pUnkOuter,__uuidof(T),COMRetPtr(Object));
		if(FAILED(hr)){
			return nullptr;
		}

		return Object;
	}

private:
	cnRTL::cString<wchar_t> fModuleName;
	HMODULE fModuleHandle;
	bool fLoadFailed;

	bool SetupModule(void)noexcept(true);

#if cnLibrary_CPPFEATURE_NOEXCEPT_FUNC_TYPE >= 201510L
	typedef HRESULT CALLBACK ProcTypeDllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID * ppvObj)noexcept(true);
#else
	typedef HRESULT CALLBACK ProcTypeDllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID * ppvObj);
#endif
	ProcTypeDllGetClassObject *fGetClassObjectProc;
};
//---------------------------------------------------------------------------
class cCOMApartmentThreading : private iProcedure
{
protected:
	cCOMApartmentThreading(aCls<cWinTLS> *TLS,iDispatch *Dispatch)noexcept(true);
	~cCOMApartmentThreading()noexcept(true);

	void OnDelete(void)noexcept(true);
public:
	void IncreaseReference(void)noexcept(true);
	void DecreaseReference(void)noexcept(true);

	iDispatch *GetDispatch(void)noexcept(true);

private:
	aClsRef<cWinTLS> fTLS;
	iPtr<iDispatch> fDispatch;
	cAtomicVar<uIntn> fRefCount;

	virtual void cnLib_FUNC Execute(void)noexcept(true)override;
};
//---------------------------------------------------------------------------
class cCOMApartmentThreadingModule
{
public:
	cCOMApartmentThreadingModule()noexcept(true);
	~cCOMApartmentThreadingModule()noexcept(true);

	class cThreading : public iCOMApartmentThreading, public cCOMApartmentThreading, public cRTLAllocator
	{
	public:
		cThreading(aCls<cWinTLS> *TLS,iDispatch *Dispatch)noexcept(true);

		typedef cCOMApartmentThreading tReferenceInterface;
		// iReference
		virtual void cnLib_FUNC IncreaseReference(void)noexcept(true) override;
		virtual void cnLib_FUNC DecreaseReference(void)noexcept(true) override;

		// iCOMApartmentThreading
	
		virtual iDispatch* cnLib_FUNC GetDispatch(void)noexcept(true)override;
	private:
		virtual void cnLib_FUNC Execute(void)noexcept(true)override;
	};

	cCOMApartmentThreading* GetThreading(void)noexcept(true);
	rPtr<cCOMApartmentThreading> QueryThreading(iDispatch *Dispatch)noexcept(true);

	cThreading* Get(void)noexcept(true);
	rPtr<cThreading> Query(iDispatch *Dispatch)noexcept(true);
private:
	aClsRef<cWinTLS> fTLS;

	class cDispatchInitialize : public iProcedure
	{
	public:
		cThreading *NewModule;
		aCls<cWinTLS> *TLS;
		iDispatch *Dispatch;
		virtual void cnLib_FUNC Execute(void)noexcept(true)override;
	};
};
//---------------------------------------------------------------------------
class cWinRegisterInstaller
{
public:
	cWinRegisterInstaller()noexcept(true);
	~cWinRegisterInstaller()noexcept(true);

	operator HKEY()const noexcept(true);
	DWORD GetMakeError(void)const noexcept(true);
	bool WasnotCreated(void)const noexcept(true);

	template<bool Create>
	void Make(HKEY BaseKey,cString<wchar_t> Path)noexcept(true);

	template<bool Create>
	void Make(cWinRegisterInstaller &ParentItem,cString<wchar_t> Path)noexcept(true);

	LSTATUS Create(HKEY BaseKey,cString<wchar_t> Path,REGSAM samDesired=KEY_CREATE_SUB_KEY|KEY_SET_VALUE)noexcept(true);
	LSTATUS Open(HKEY BaseKey,cString<wchar_t> Path,REGSAM samDesired=KEY_CREATE_SUB_KEY|DELETE|KEY_SET_VALUE)noexcept(true);
	LSTATUS Create(cWinRegisterInstaller &Parent,cString<wchar_t> Path,REGSAM samDesired=KEY_CREATE_SUB_KEY|KEY_SET_VALUE)noexcept(true);
	LSTATUS Open(cWinRegisterInstaller &Parent,cString<wchar_t> Path,REGSAM samDesired=KEY_CREATE_SUB_KEY|DELETE|KEY_SET_VALUE)noexcept(true);

	void CloseTree(void)noexcept(true);
	bool DeleteTree(void)noexcept(true);
protected:
	HKEY fBaseKey;
	HKEY fKey;
	DWORD fErrorCode;
	cString<wchar_t> fPath;

	struct cLinkItem
	{
		cLinkItem *Next;
		cLinkItem *Prev;
	}fLinkItem;

	cLinkItemList<cLinkItem> fSubItems;
};
//---------------------------------------------------------------------------
}	// namespace cnWinRTL
//---------------------------------------------------------------------------
}	// namespace cnRTL
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
