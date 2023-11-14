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
	static void Release(const BSTR &Token);
};
typedef cnVar::cPtrOwner<cBSTROwnerTokenOperator> apBSTR;
//---------------------------------------------------------------------------
apBSTR MakeBSTR(const wchar_t *Text);
BSTR* apBSTRRetPtr(apBSTR &Ptr);
//---------------------------------------------------------------------------
inline IUnknown* iCastCOM(iInterface *Interface){
	auto COMInterface=iCast<iCOMInterface>(Interface);
	if(COMInterface==nullptr)
		return nullptr;
	return COMInterface->GetCOMInterface();
}
//---------------------------------------------------------------------------
template<class T>
bool impCOMQueryInterface(T*,REFIID,_COM_Outptr_ void **)
{
	return false;
}
//---------------------------------------------------------------------------
template<class TInterface,class...VT,class T>
bool impCOMQueryInterface(T *Object,REFIID riid,_COM_Outptr_ void **ppvObject)
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
	static void Acquire(T *Token){	if(Token!=nullptr)	Token->AddRef();	}
	static void Release(T *Token){	if(Token!=nullptr)	Token->Release();	}
};

template<class T>
using COMPtr=cnVar::cPtrReference< cCOMRefTokenOperator<T> >;

//---------------------------------------------------------------------------
template<class T>
COMPtr<T> COMTake(T *Src)
{
	return COMPtr<T>::TakeFromManual(Src);
}
//---------------------------------------------------------------------------
template<class T>
T* COMExtract(COMPtr<T> &Src)
{
	return Src.ExtractToManual();
}
//---------------------------------------------------------------------------
template<class TDest,class T>
TDest* COMExtractAs(COMPtr<T> &Src)
{
	return static_cast<TDest*>(Src.ExtractToManual());
}
//---------------------------------------------------------------------------
template<class T>
inline void** COMRetPtr(T* &Ptr){
	return reinterpret_cast<void**>(&Ptr);
}
//---------------------------------------------------------------------------
template<class T>
inline void** COMRetPtr(COMPtr<T> &Ptr){
	Ptr=nullptr;
	return reinterpret_cast<void**>(&Ptr.Token());
}
//---------------------------------------------------------------------------
template<class T>
inline T** COMRetPtrT(T* &Ptr){
	return &Ptr;
}
//---------------------------------------------------------------------------
template<class T>
inline T** COMRetPtrT(COMPtr<T> &Ptr){
	Ptr=nullptr;
	return &Ptr.Token();
}
//---------------------------------------------------------------------------
// COM
//---------------------------------------------------------------------------
template<class TDestInterface,class T>
COMPtr<TDestInterface> COMQueryInterface(const T &Src)
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

	virtual HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid,_COM_Outptr_ void **ppvObject)override{
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

	virtual ULONG STDMETHODCALLTYPE AddRef(void)override{
		fRefCount.Free++;
		return 0;
	}

	virtual ULONG STDMETHODCALLTYPE Release(void)override{
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
COMPtr<T> COMCreate(void){
	return COMPtr<T>::TakeFromManual(new cnLib_THelper::RTL_TH::cCOMImplementation<T>);
}
template<class T,class...TArgs>
COMPtr<T> COMCreate(TArgs&&...Args){
	return COMPtr<T>::TakeFromManual(new cnLib_THelper::RTL_TH::cCOMImplementation<T>(cnVar::Forward<TArgs>(Args)...));
}
//---------------------------------------------------------------------------
template<class T>
class COMInnerObject : public T
{
private:
public:
	IUnknown *Outter;

#if cnLibrary_CPPFEATURE_INHERIT_CONSTRUCTORS >= 200802L
	using T::T;
#else	// cnLibrary_CPPFEATURE_INHERIT_CONSTRUCTORS < 200802L
	template<class...TArgs>
	COMInnerObject(TArgs&&...Args):T(cnVar::Forward<TArgs>(Args)...){}
#endif // cnLibrary_CPPFEATURE_INHERIT_CONSTRUCTORS

    virtual HRESULT STDMETHODCALLTYPE QueryInterface(
        /* [in] */ REFIID riid,
        /* [iid_is][out] */ _COM_Outptr_ void **ppvObject)
	{
		return Outter->QueryInterface(riid,ppvObject);
	}

    virtual ULONG STDMETHODCALLTYPE AddRef(void){
		return Outter->AddRef();
	}

    virtual ULONG STDMETHODCALLTYPE Release(void){
		return Outter->Release();
	}
};
//---------------------------------------------------------------------------
class iCOMInnerReference
{
public:
	virtual void COMDelete(void)=0;
	virtual bool COMInnerQueryInterface(REFIID riid,void **ppvObject)=0;
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

	virtual void COMDelete(void)override{
		delete this;
	}
	virtual bool COMInnerQueryInterface(REFIID riid,void **ppvObject)override{
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

	static T* Pointer(const TOwnerToken &Token){	return Token.Pointer;	}

	static TOwnerToken TokenNull(void){
		TOwnerToken RetToke={nullptr,nullptr};
		return RetToke;
	}
	template<class TImp>
	static TOwnerToken TokenFrom(cnLib_THelper::RTL_TH::cCOMInnerImplementation<TImp> *p){
		TOwnerToken RetToke={p,p};
		return RetToke;
	}

	template<class TSrc>
	static typename cnVar::TTypeConditional<TOwnerToken,
		cnVar::TIsAssignable<T*&,TSrc*>::Value
	>::Type TokenFrom(cCOMInnerOwnerToken<TSrc> SrcToken){
		TOwnerToken RetToken;
		RetToken.Pointer=SrcToken.Pointer;
		RetToken.Reference=SrcToken.Reference;
		return RetToken;
	}

	static void Release(const TOwnerToken &Token){
		if(Token.Reference!=nullptr)
			Token.Reference->COMDelete();
	}
};
//---------------------------------------------------------------------------
template<class T>
using COMInnerPtr = cnVar::cPtrOwner< cCOMInnerOwnerTokenOperator<T> >;
//---------------------------------------------------------------------------
template<class T>
COMInnerPtr<T> COMInnerPtrCreate(IUnknown *Outter){
	auto Ptr=new cnLib_THelper::RTL_TH::cCOMInnerImplementation<T>;
	Ptr->Outter=Outter;
	return COMInnerPtr<T>::Take(Ptr);
}
//---------------------------------------------------------------------------
template<class T>
bool COMInnerPtrQueryInterface(const COMInnerPtr<T> &Ptr,REFIID riid,void **ppvObject){
	auto Ref=Ptr.Token().Reference;
	return Ref->COMInnerQueryInterface(riid,ppvObject);
}
//---------------------------------------------------------------------------
namespace cnWinRTL{
//---------------------------------------------------------------------------
class cOLEModule
{
public:
	cOLEModule(const wchar_t *ModuleName);
	~cOLEModule();

	HRESULT QueryFactory(REFCLSID rclsid,IClassFactory **Factory);
	HRESULT CreateObject(REFCLSID rclsid,IUnknown *pUnkOuter, REFIID riid,LPVOID *ppvObj);
	


	COMPtr<IClassFactory> QueryFactory(REFCLSID rclsid);

	template<class T>
	COMPtr<T> CreateObject(REFCLSID rclsid){
		HRESULT hr;

		COMPtr<T> Object;
		hr=CreateObject(rclsid,nullptr,__uuidof(T),COMRetPtr(Object));
		if(FAILED(hr)){
			return nullptr;
		}

		return Object;
	}
	template<class T>
	COMPtr<T> CreateAggregateObject(REFCLSID rclsid,IUnknown *pUnkOuter){
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

	bool SetupModule(void);

	typedef HRESULT CALLBACK ProcTypeDllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID * ppvObj);
	ProcTypeDllGetClassObject *fGetClassObjectProc;
};
//---------------------------------------------------------------------------
class cCOMApartmentThreading : private iProcedure
{
protected:
	cCOMApartmentThreading(aCls<cWinTLS> *TLS,iDispatch *Dispatch);
	~cCOMApartmentThreading();

	void OnDelete(void);
public:
	void IncreaseReference(void);
	void DecreaseReference(void);

	iDispatch *GetDispatch(void);

private:
	aClsRef<cWinTLS> fTLS;
	iPtr<iDispatch> fDispatch;
	cAtomicVar<uIntn> fRefCount;

	virtual void cnLib_FUNC Execute(void)override;
};
//---------------------------------------------------------------------------
class cCOMApartmentThreadingModule
{
public:
	cCOMApartmentThreadingModule();
	~cCOMApartmentThreadingModule();

	class cThreading : public iCOMApartmentThreading, public cCOMApartmentThreading, public cRTLAllocator
	{
	public:
		cThreading(aCls<cWinTLS> *TLS,iDispatch *Dispatch);

		typedef cCOMApartmentThreading tReferenceInterface;
		// iReference
		virtual void cnLib_FUNC IncreaseReference(void)noexcept(true) override;
		virtual void cnLib_FUNC DecreaseReference(void)noexcept(true) override;

		// iCOMApartmentThreading
	
		virtual iDispatch* cnLib_FUNC GetDispatch(void)override;
	private:
		virtual void cnLib_FUNC Execute(void)override;
	};

	cCOMApartmentThreading* GetThreading(void);
	rPtr<cCOMApartmentThreading> QueryThreading(iDispatch *Dispatch);

	cThreading* Get(void);
	rPtr<cThreading> Query(iDispatch *Dispatch);
private:
	aClsRef<cWinTLS> fTLS;

	class cDispatchInitialize : public iProcedure
	{
	public:
		cThreading *NewModule;
		aCls<cWinTLS> *TLS;
		iDispatch *Dispatch;
		virtual void cnLib_FUNC Execute(void)override;
	};
};
//---------------------------------------------------------------------------
class cWinRegisterInstaller
{
public:
	cWinRegisterInstaller();
	~cWinRegisterInstaller();

	operator HKEY()const;
	DWORD GetMakeError(void)const;
	bool WasnotCreated(void)const;

	template<bool Create>
	void Make(HKEY BaseKey,cString<wchar_t> Path);

	template<bool Create>
	void Make(cWinRegisterInstaller &ParentItem,cString<wchar_t> Path);

	LSTATUS Create(HKEY BaseKey,cString<wchar_t> Path,REGSAM samDesired=KEY_CREATE_SUB_KEY|KEY_SET_VALUE);
	LSTATUS Open(HKEY BaseKey,cString<wchar_t> Path,REGSAM samDesired=KEY_CREATE_SUB_KEY|DELETE|KEY_SET_VALUE);
	LSTATUS Create(cWinRegisterInstaller &Parent,cString<wchar_t> Path,REGSAM samDesired=KEY_CREATE_SUB_KEY|KEY_SET_VALUE);
	LSTATUS Open(cWinRegisterInstaller &Parent,cString<wchar_t> Path,REGSAM samDesired=KEY_CREATE_SUB_KEY|DELETE|KEY_SET_VALUE);

	void CloseTree(void);
	bool DeleteTree(void);
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
