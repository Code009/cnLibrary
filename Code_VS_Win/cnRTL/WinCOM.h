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
template<class...T>
struct TCOMInterfacePack
	: cnVar::TTypePack<T...>
{
};
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
namespace cnLib_THelper{
namespace RTL_TH{
//---------------------------------------------------------------------------
template<class TEnable,class TCOMImplementation>
struct cCOMCallQueryInterface
{
	static bool COMQueryInterface(TCOMImplementation *,REFIID ,_COM_Outptr_ void __RPC_FAR *__RPC_FAR *)noexcept(true)
	{	return false;	}
};
#if !defined(cnLibrary_CPPEXCLUDE_SFINAE_DECLTYPE_EXPRESSION) && cnLibrary_CPPFEATURE_DECLTYPE >= 200707L

template<class TCOMImplementation>
struct cCOMCallQueryInterface<typename cnVar::TSelect<0,void,decltype(&TCOMImplementation::COMQueryInterface)>::Type,TCOMImplementation>
{
	static bool COMQueryInterface(TCOMImplementation *Implementation,REFIID riid,_COM_Outptr_ void __RPC_FAR *__RPC_FAR *ppvObject)noexcept(true)
	{	return Implementation->COMQueryInterface(riid,ppvObject);	}
};
#else

#ifndef cnLibrary_CPPEXCLUDE_SFINAE_SIZEOF_EXPRESSION

template<class TCOMImplementation>
struct cCOMCallQueryInterface<typename cnVar::TTypeConditional<void,sizeof(bool)==sizeof(cnVar::DeclVal<TCOMImplementation>().COMQueryInterface(0,IID_IUnknown,0))>::Type,TCOMImplementation>
{
	static bool COMQueryInterface(TImplementation *Implementation,REFIID riid,_COM_Outptr_ void __RPC_FAR *__RPC_FAR *ppvObject)noexcept(true)
	{	return Implementation->COMQueryInterfcace(riid,ppvObject);	}
};

#endif // !cnLibrary_CPPEXCLUDE_SFINAE_SIZEOF_EXPRESSION


#endif
//---------------------------------------------------------------------------
template<bool CallCOMQueryInterface,class...TInterfaces>
struct cCOMQueryInterfaceImplemenetaton;
template<bool CallCOMQueryInterface>
struct cCOMQueryInterfaceImplemenetaton<CallCOMQueryInterface>
{
	template<class TImplementation>
	static bool FindInterface(TImplementation *Implementation,REFIID riid,_COM_Outptr_ void **ppvObject)noexcept(true){
		return cCOMCallQueryInterface<void,TImplementation>::COMQueryInterface(Implementation,riid,ppvObject);
	}
};
template<>
struct cCOMQueryInterfaceImplemenetaton<false>
{
	template<class TImplementation>
	static bool FindInterface(TImplementation *Implementation,REFIID riid,_COM_Outptr_ void **ppvObject)noexcept(true){
		return false;
	}
};

template<bool CallCOMQueryInterface,class TInterface,class...TInterfaces>
struct cCOMQueryInterfaceImplemenetaton<CallCOMQueryInterface,TInterface,TInterfaces...>
{
	typedef TInterface tPrimaryInterface;

	template<class TImplementation>
	static bool FindInterface(TImplementation *Implementation,REFIID riid,_COM_Outptr_ void **ppvObject)noexcept(true){
		if(riid==__uuidof(TInterface)){
			*ppvObject=static_cast<TInterface*>(Implementation);
			return true;
		}
		return cCOMQueryInterfaceImplemenetaton<CallCOMQueryInterface,TInterfaces...>::FindInterface(Implementation,riid,ppvObject);
	}

	template<class TImplementation>
	static bool ImplementQueryInterface(TImplementation *Implementation,REFIID riid,_COM_Outptr_ void **ppvObject)noexcept(true){
		if(riid==__uuidof(IUnknown)){
			*ppvObject=static_cast<IUnknown*>(static_cast<TInterface*>(Implementation));
			return true;
		}
		return FindInterface(Implementation,riid,ppvObject);
	}
};
template<bool CallCOMQueryInterface,class...TPackInterfaces,class...TInterfaces>
struct cCOMQueryInterfaceImplemenetaton<CallCOMQueryInterface,TCOMInterfacePack<TPackInterfaces...>,TInterfaces...>
{
	typedef typename cCOMQueryInterfaceImplemenetaton<CallCOMQueryInterface,TPackInterfaces...>::tPrimaryInterface tPrimaryInterface;

	template<class TImplementation>
	static bool FindInterface(TImplementation *Implementation,REFIID riid,_COM_Outptr_ void **ppvObject)noexcept(true){
		if(cCOMQueryInterfaceImplemenetaton<false,TPackInterfaces...>::FindInterface(Implementation,riid,ppvObject)){
			return true;
		}
		return cCOMQueryInterfaceImplemenetaton<CallCOMQueryInterface,TInterfaces...>::FindInterface(Implementation,riid,ppvObject);
	}

	template<class TImplementation>
	static bool ImplementQueryInterface(TImplementation *Implementation,REFIID riid,_COM_Outptr_ void **ppvObject)noexcept(true){
		if(cCOMQueryInterfaceImplemenetaton<false,TPackInterfaces...>::ImplementQueryInterface(Implementation,riid,ppvObject)){
			return true;
		}
		return cCOMQueryInterfaceImplemenetaton<CallCOMQueryInterface,TInterfaces...>::FindInterface(Implementation,riid,ppvObject);
	}
};

template<class TInterfaceTypePack>
struct cCOMQueryInterface;

template<class...TInterfaces>
struct cCOMQueryInterface< TCOMInterfacePack<TInterfaces...> >
	: cCOMQueryInterfaceImplemenetaton<true,TInterfaces...>{};

template<class TEnable,class T>
struct COMFindPrimaryInterface
	: cnVar::TTypeDef<T>{};

template<class T>
struct COMFindPrimaryInterface<typename cnVar::TSelect< 0,void,typename T::tCOMInterfacePack>::Type,T>
	: cnVar::TTypeDef<
		typename cCOMQueryInterface<typename T::tCOMInterfacePack>::tPrimaryInterface
	>{};

//---------------------------------------------------------------------------
}	// namespace RTL_TH
}	// namespace cnLib_THelper
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
template<class TFunction>
struct TCOMFunctionInfo;

template<class TClass,class TRet,class...TArgs>
struct TCOMFunctionInfo<TRet (STDMETHODCALLTYPE TClass::*)(TArgs...)>
	: cnVar::TTypeComponent<TRet (TClass::*)(TArgs...)>
{
	typedef TClass ClassArgumentType;
};
//---------------------------------------------------------------------------
template<class T>
struct cCOMRefTokenOperator : cnVar::bcPointerRefTokenOperator<T*>
{
	//static void Acquire(T *Token)noexcept(true){	if(Token!=nullptr)	Token->AddRef();	}
	//static void Release(T *Token)noexcept(true){	if(Token!=nullptr)	Token->Release();	}
	static void Acquire(T *Token)noexcept(true){
		if(Token!=nullptr)	
			static_cast<typename cnLib_THelper::RTL_TH::COMFindPrimaryInterface<void,T>::Type*>(Token)->AddRef();
	}
	static void Release(T *Token)noexcept(true){
		if(Token!=nullptr)
			static_cast<typename cnLib_THelper::RTL_TH::COMFindPrimaryInterface<void,T>::Type*>(Token)->Release();
	}
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
template<class TCOMObject>
inline bool COMObjectQueryInterface(TCOMObject *Object,REFIID riid,_COM_Outptr_ void **ppvObject)noexcept(true)
{
	return cnLib_THelper::RTL_TH::cCOMQueryInterface<typename TCOMObject::tCOMInterfacePack>::template ImplementQueryInterface<TCOMObject>(Object,riid,ppvObject);
}
//---------------------------------------------------------------------------
// COM
//---------------------------------------------------------------------------
template<class TDestInterface,class TCOMObject>
inline TDestInterface* COMObjectQueryInterface(TCOMObject *Src)noexcept(true)
{
	if(Src==nullptr)
		return nullptr;
	TDestInterface *ci;
	if(COMObjectQueryInterface(Src,__uuidof(TDestInterface),reinterpret_cast<void**>(&ci))){
		return ci;
	}
	return nullptr;
}
//---------------------------------------------------------------------------
template<class TCOMImplementation>
class cCOMImplementation : public TCOMImplementation, public cnRTL::cRTLAllocator
{
private:
	cnRTL::cAtomicVar<uIntn> fRefCount=1;
	
public:
	using TCOMImplementation::TCOMImplementation;
	~cCOMImplementation()=default;


	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,_COM_Outptr_ void **ppvObject)noexcept(true)override{

		if(COMObjectQueryInterface<TCOMImplementation>(this,riid,ppvObject)){
			++fRefCount.Free;
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
		if(--fRefCount.Free==0){
			delete this;
		}
		return 0;
	}
};
//---------------------------------------------------------------------------
template<class T>
inline COMPtr<T> COMCreate(void)noexcept(true){
	return COMPtr<T>::TakeFromManual(new cCOMImplementation<T>);
}
template<class T,class...TArgs>
inline COMPtr<T> COMCreate(TArgs&&...Args)noexcept(true){
	return COMPtr<T>::TakeFromManual(new cCOMImplementation<T>(cnVar::Forward<TArgs>(Args)...));
}

#if cnLibrary_CPPFEATURE_DECLTYPE >=200707L && cnLibrary_CPPFEATURE_TEMPLATE_NONTYPE_AUTO >= 201606L


template<auto MemberOffset>
struct cCOMInnerMemberInterfaceTranslator
{
	typedef typename cnLib_THelper::RTL_TH::cCOMQueryInterface<typename cnVar::TTypeComponent<decltype(MemberOffset)>::tClass::tCOMInterfacePack>
		::tPrimaryInterface tPrimaryInterface;
	
	static tPrimaryInterface* GetPrimary(typename cnVar::TTypeComponent<decltype(MemberOffset)>::tMember *Object){
		return static_cast<tPrimaryInterface*>(cnMemory::GetObjectFromMemberPointer(Object,MemberOffset));
	}
};

#endif // cnLibrary_CPPFEATURE_DECLTYPE >=200707L && cnLibrary_CPPFEATURE_TEMPLATE_NONTYPE_AUTO >= 201606L

template<class T,class TInnerMemberInterfaceTranslator>
class COMInnerMember : public T
{
private:
public:
	//IUnknown *Outter;

	using T::T;

	bool COMInnerQueryInterface(REFIID riid,void **ppvObject)noexcept(true){
		return cnLib_THelper::RTL_TH::cCOMQueryInterface<typename T::tCOMInterfacePack>::template FindInterface<T>(this,riid,ppvObject);
	}

    virtual HRESULT STDMETHODCALLTYPE QueryInterface(
        /* [in] */ REFIID riid,
        /* [iid_is][out] */ _COM_Outptr_ void **ppvObject)noexcept(true)
	{
		return TInnerMemberInterfaceTranslator::GetPrimary(this)->QueryInterface(riid,ppvObject);
	}

    virtual ULONG STDMETHODCALLTYPE AddRef(void)noexcept(true){
		return TInnerMemberInterfaceTranslator::GetPrimary(this)->AddRef();
	}

    virtual ULONG STDMETHODCALLTYPE Release(void)noexcept(true){
		return TInnerMemberInterfaceTranslator::GetPrimary(this)->Release();
	}
};

//---------------------------------------------------------------------------
template<class T>
class COMInnerObject : public T
{
private:
public:
	IUnknown *Outter;

	using T::T;

	
	bool COMInnerQueryInterface(REFIID riid,void **ppvObject)noexcept(true){
		return cnLib_THelper::RTL_TH::cCOMQueryInterface<typename T::tCOMInterfacePack>::template FindInterface<T>(this,riid,ppvObject);
	}

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
	virtual void COMInnerDelete(void)noexcept(true)=0;
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
template<class TCOMImplementation>
class cCOMInnerImplementation : public cnRTL::COMInnerObject<TCOMImplementation>, public cnRTL::iCOMInnerReference, public cnRTL::cRTLAllocator
{
private:
	cnRTL::cAtomicVar<uIntn> fRefCount=1;
public:
	using cnRTL::COMInnerObject<TCOMImplementation>::COMInnerObject;
	~cCOMInnerImplementation()=default;

	virtual void COMInnerDelete(void)noexcept(true)override{
		delete this;
	}
	virtual bool COMInnerQueryInterface(REFIID riid,void **ppvObject)noexcept(true)override{
		return cnLib_THelper::RTL_TH::cCOMQueryInterface<typename TCOMImplementation::tCOMInterfacePack>::template FindInterface<TCOMImplementation>(this,riid,ppvObject);
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
			Token.Reference->COMInnerDelete();
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
