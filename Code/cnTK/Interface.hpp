/*- cnTK - Interface Declaration ------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2018-08-14                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnTK_Interface_HPP__
#define	__cnLibrary_cnTK_Interface_HPP__
/*-------------------------------------------------------------------------*/
#include <cnTK/Common.hpp>
#include <cnTK/TypeTraits.hpp>
#include <cnTK/Pointer.hpp>
/*-------------------------------------------------------------------------*/
#if	cnLibrary_CPPFEATURELEVEL >= 1
//---------------------------------------------------------------------------
#include <cnTK/TKMacrosDeclare.inc>
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------

typedef cnMemory::cPlainData<16> cUUID;

//---------------------------------------------------------------------------

template<class T>
struct cArray
{
	typedef T tElement;
	T *Pointer;
	uIntn Length;
	
#ifndef cnLibrary_CPPEXCLUDE_FUNCTION_TEMPLATE_DEFALT_ARGUMENT
	template<class TDest,class TArray=typename cnVar::TTypeConditional<cArray<TDest>&,cnVar::TIsReinterpretable<T,TDest>::Value>::Type>
	operator cArray<TDest> &(void)noexcept(true){
		return reinterpret_cast<TArray>(*this);
	}
	template<class TDest,class TArray=typename cnVar::TTypeConditional<const cArray<TDest>&,cnVar::TIsReinterpretable<T,TDest>::Value>::Type>
	operator const cArray<TDest> &(void)const noexcept(true){
		return reinterpret_cast<TArray>(*this);
	}
// !cnLibrary_CPPEXCLUDE_FUNCTION_TEMPLATE_DEFALT_ARGUMENT
#else
// cnLibrary_CPPEXCLUDE_FUNCTION_TEMPLATE_DEFALT_ARGUMENT
	template<class TDest>
	operator cArray<TDest> &(void) noexcept(true){
		return reinterpret_cast<
			typename cnVar::TTypeConditional<cArray<TDest>&,cnVar::TIsReinterpretable<T,TDest>::Value>::Type
		>(*this);
	}
	template<class TDest>
	operator const cArray<TDest> &(void)const noexcept(true){
		return reinterpret_cast<
			typename cnVar::TTypeConditional<const cArray<TDest>&,cnVar::TIsReinterpretable<T,TDest>::Value>::Type
		>(*this);
	}
#endif	// cnLibrary_CPPEXCLUDE_FUNCTION_TEMPLATE_DEFALT_ARGUMENT
};

template<class TDestArray,class TSrc>
typename cnVar::TTypeConditional<TDestArray*,
	cnVar::TIsReinterpretable<TSrc,typename TDestArray::tElement>::Value
>::Type	ArrayCast(cArray<TSrc> *Array)noexcept(true){
	return reinterpret_cast<TDestArray*>(Array);
}
template<class TDestArray,class TSrc>
typename cnVar::TTypeConditional<const TDestArray*,
	cnVar::TIsReinterpretable<TSrc,typename TDestArray::tElement>::Value
>::Type	ArrayCast(const cArray<TSrc> *Array)noexcept(true){
	return reinterpret_cast<const TDestArray*>(Array);
}


template<class TDest,class TSrc>
typename cnVar::TTypeConditional<cArray<TDest>*,
	cnVar::TIsReinterpretable<TSrc,TDest>::Value
>::Type	ArrayCastElement(cArray<TSrc> *Array)noexcept(true){
	return reinterpret_cast<cArray<TDest>*>(Array);
}
template<class TDest,class TSrc>
typename cnVar::TTypeConditional<const cArray<TDest>*,
	cnVar::TIsReinterpretable<TSrc,TDest>::Value
>::Type	ArrayCastElement(const cArray<TSrc> *Array)noexcept(true){
	return reinterpret_cast<const cArray<TDest>*>(Array);
}



typedef cArray<void> cMemory;
typedef cArray<const void> cConstMemory;
//---------------------------------------------------------------------------

struct iTypeInfo
{
	const uInt8 (*LibraryID)[16];
	uInt32 ClassID;

	bool operator ==(const iTypeInfo &Compare)const noexcept(true){
		if(this==&Compare)
			return true;

		if(LibraryID==nullptr || Compare.LibraryID==nullptr)
			return false;

		if(ClassID!=Compare.ClassID)
			return false;

		if(LibraryID==Compare.LibraryID)
			return true;
		const uIntn Length=15/sizeof(uIntn)+1;
		for(uIntn i=0;i<Length;i++){
			if(reinterpret_cast<const uIntn*>(LibraryID)[i]!=reinterpret_cast<const uIntn*>(Compare.LibraryID)[i]){
				return false;
			}
		}
		return true;
	}
	bool operator !=(const iTypeInfo &Compare)const noexcept(true){
		return !this->operator ==(Compare);
	}
};

typedef const iTypeInfo& iTypeID;

//---------------------------------------------------------------------------
class cnLib_INTERFACE iInterface
{
public:
	// CastInterface
	//	Get interface from the object
	virtual void* cnLib_FUNC CastInterface(iTypeID)noexcept(true){
		return nullptr;
	}
};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
namespace cnLib_THelper{
//---------------------------------------------------------------------------
namespace Var_TH{
//---------------------------------------------------------------------------

template<class TEnbale,class T>
struct InterfaceID
{
};

#if !defined(cnLibrary_CPPEXCLUDE_SFINAE_DECLTYPE_EXPRESSION) && cnLibrary_CPPFEATURE_DECLTYPE >= 200707L

template<class T>
struct InterfaceID<typename cnVar::TTypeConditional<void,cnVar::TIsSame<decltype(&T::CastInterface),void* (cnLib_FUNC T::*)(iTypeID)noexcept(true)>::Value & sizeof(typename T::tInterfaceID)>::Type,T>
	: T::tInterfaceID
{
	typedef T Type;
};

// !defined(cnLibrary_CPPEXCLUDE_SFINAE_DECLTYPE_EXPRESSION) && cnLibrary_CPPFEATURE_DECLTYPE >= 200707L
#else
// defined(cnLibrary_CPPEXCLUDE_SFINAE_DECLTYPE_EXPRESSION) || cnLibrary_CPPFEATURE_DECLTYPE < 200707L

#ifdef cnLibrary_CPPEXCLUDE_SFINAE_SIZEOF_EXPRESSION

template<class TClass,class TMemberClass>
inline typename cnVar::TTypeConditional<cnMemory::cPlainData<2>,cnVar::TIsSame<TClass,TMemberClass>::Value>::Type InterfaceIDTest(void* (cnLib_FUNC TMemberClass::*)(iTypeID)noexcept(true));

inline cnMemory::cPlainData<1> InterfaceIDTest(...);

template<class T>
struct InterfaceID<typename cnVar::TTypeConditional<void,sizeof(InterfaceIDTest<T>(&T::CastInterface)) + sizeof(typename T::tInterfaceID)>::Type,T>
	: T::tInterfaceID
{
	typedef T Type;
};

// !defined(cnLibrary_CPPEXCLUDE_SFINAE_SIZEOF_EXPRESSION)
#else	
// defined(cnLibrary_CPPEXCLUDE_SFINAE_SIZEOF_EXPRESSION)

template<class T>
struct InterfaceID<void,T>
	: T::tTypeID
{
	typedef T Type;
};

#endif // defined(cnLibrary_CPPEXCLUDE_SFINAE_SIZEOF_EXPRESSION)


#endif	// defined(cnLibrary_CPPEXCLUDE_SFINAE_DECLTYPE_EXPRESSION) || cnLibrary_CPPFEATURE_DECLTYPE < 200707L


#define	cnLib_INTERFACE_LOCALID_DEFINE(_interface_)	static const iTypeInfo LocalInterfaceInfo_##_interface_={};	iTypeID _interface_::tInterfaceID::Value=LocalInterfaceInfo_##_interface_;
//---------------------------------------------------------------------------
}	// namespace Var_TH
//---------------------------------------------------------------------------
}	// namespace cnLib_THelper
//---------------------------------------------------------------------------
namespace cnLibrary{


template<class T>
struct TInterfaceID
	: cnLib_THelper::Var_TH::InterfaceID<void,T>{};

//---------------------------------------------------------------------------
template<class T>
inline iTypeID iTypeOf(T *)noexcept(true){
	return TInterfaceID<T>::Value;
}

//---------------------------------------------------------------------------

template<class TDest,class TSrc>
inline typename TInterfaceID<TDest>::Type* iCast(TSrc cnLib_UREF Src)noexcept(true)
{
	if(Src==nullptr)
		return nullptr;
	return static_cast<TDest*>( Src->CastInterface(TInterfaceID<TDest>::Value) );
}
//---------------------------------------------------------------------------
class cnLib_INTERFACE iReference
{
public:
	struct tInterfaceID{	static iTypeID Value;	};
	typedef iReference tReferenceInterface;

	// IncreaseReference
	//	increase reference to the object
	virtual	void cnLib_FUNC IncreaseReference(void)noexcept(true)=0;
	// DecreaseReference
	//	decrease reference to the object
	virtual	void cnLib_FUNC DecreaseReference(void)noexcept(true)=0;
};
template<> struct TInterfaceID<iReference>:iReference::tInterfaceID{	typedef iReference Type;	};
//---------------------------------------------------------------------------
class iObservedReference;
class cnLib_INTERFACE iReferenceInvalidationNotify
{
public:
	uIntn _Internal_[4];
	virtual bool cnLib_FUNC InvalidationNotify(iObservedReference *Reference)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iObservedReference : public iReference
{
public:
	typedef iObservedReference tReferenceInterface;
	struct tInterfaceID{	static iTypeID Value;	};

	virtual	void cnLib_FUNC InvalidationRegisterNotification(iReferenceInvalidationNotify *Notify)noexcept(true)=0;
	virtual	void cnLib_FUNC InvalidationUnregisterNotification(iReferenceInvalidationNotify *Notify)noexcept(true)=0;
	virtual	bool cnLib_FUNC Reference(void)noexcept(true)=0;
};
template<> struct TInterfaceID<iObservedReference>:iObservedReference::tInterfaceID{	typedef iObservedReference Type;	};
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
namespace cnLib_THelper{
//---------------------------------------------------------------------------
namespace Var_TH{
//---------------------------------------------------------------------------
template<class TEnable,class T>
struct ClassReferenceInterface
	: cnVar::TTypeDef<T>{};

template<class T>
struct ClassReferenceInterface<typename cnVar::TSelect<0,void,typename T::tReferenceInterface>::Type,T>
	: cnVar::TTypeDef<typename T::tReferenceInterface>{};
//---------------------------------------------------------------------------
}	// namespace Var_TH
//---------------------------------------------------------------------------
}	// namespace cnLib_THelper
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnVar{
//---------------------------------------------------------------------------
template<class TImplementation>
void* FindInterface(TImplementation*,iTypeID)noexcept(true)
{
	return nullptr;
}
//---------------------------------------------------------------------------
template<class TInterface,class TImplementation>
void* FindInterface(TImplementation *Implementation,iTypeID ID)noexcept(true)
{
	return Implementation->TInterface::CastInterface(ID);
}
template<class TInterface,class TNextInterface,class...VT,class TImplementation>
void* FindInterface(TImplementation *Implementation,iTypeID ID)noexcept(true)
{
	void *RetInterface=Implementation->TInterface::CastInterface(ID);
	if(RetInterface)
		return RetInterface;

	return FindInterface<TNextInterface,VT...>(Implementation,ID);
}
template<class...VT,class TImplementation>
void* ImplementCastInterface(TImplementation *Implementation,iTypeID ID)noexcept(true)
{
	if(ID==TInterfaceID<TImplementation>::Value)
		return Implementation;
	return FindInterface<VT...>(Implementation,ID);
}
//---------------------------------------------------------------------------
template<class T>
struct TClassReferenceInterface
	: TTypeDef<typename cnLib_THelper::Var_TH::ClassReferenceInterface<void,T>::Type>{};
//---------------------------------------------------------------------------
template<class TReference>
struct rRefToken
{
	TReference *Pointer;
};
//---------------------------------------------------------------------------
template<class TReference>
struct rRefTokenOperator
{
	typedef TReference *tPtr;
	typedef rRefToken<TReference> tRefToken;

	static TReference* Pointer(const tRefToken &Token)noexcept(true){	return Token.Pointer;	}

	static tRefToken TokenNull(void)noexcept(true){
		tRefToken RetToke={nullptr};
		return RetToke;
	}
	static tRefToken TokenFrom(tPtr Pointer)noexcept(true){
		tRefToken RetToke={Pointer};
		return RetToke;
	}

	template<class TSrcReference>
	static typename TTypeConditional<tRefToken,
		TIsAssignableFrom<TReference*&,TSrcReference*>::Value
	>::Type TokenFrom(rRefToken<TSrcReference> Src)noexcept(true){
		tRefToken RetToke={Src.Pointer};
		return RetToke;
	}

	static void Acquire(const tRefToken &Token)noexcept(true){
		if(Token.Pointer!=nullptr)
			static_cast<typename TClassReferenceInterface<TReference>::Type*>(Token.Pointer)->IncreaseReference();
	}
	static void Release(const tRefToken &Token)noexcept(true){
		if(Token.Pointer!=nullptr)
			static_cast<typename TClassReferenceInterface<TReference>::Type*>(Token.Pointer)->DecreaseReference();
	}
};
//---------------------------------------------------------------------------
}	// namespace cnVar
//---------------------------------------------------------------------------

template<class T>
class rPtr : public cnVar::cPtrReference< cnVar::rRefTokenOperator<T> >
{
public:
	typedef cnVar::rRefToken<T> tRefToken;

	template<class TSrcPtr>
	static rPtr TakeFromManual(TSrcPtr *Pointer)noexcept(true)
	{		return rPtr(cnVar::cPtrReference< cnVar::rRefTokenOperator<T> >::MakeConstructByToken(cnVar::rRefTokenOperator<T>::TokenFrom(Pointer)));	}

#if cnLibrary_CPPFEATURE_INHERIT_CONSTRUCTORS >= 200802L
	using cnVar::cPtrReference< cnVar::rRefTokenOperator<T> >::cPtrReference;
	using cnVar::cPtrReference< cnVar::rRefTokenOperator<T> >::operator =;

#else	// cnLibrary_CPPFEATURE_INHERIT_CONSTRUCTORS < 200802L

	rPtr()noexcept(true){}

	rPtr(const rPtr &Src)noexcept(true) : cnVar::cPtrReference< cnVar::rRefTokenOperator<T> >( static_cast< const cnVar::cPtrReference< cnVar::rRefTokenOperator<T> >&>(Src)) {}
	rPtr& operator =(const rPtr &Src)noexcept(true){
		cnVar::cPtrReference< cnVar::rRefTokenOperator<T> >::operator =(static_cast<const cnVar::cPtrReference< cnVar::rRefTokenOperator<T> >&>(Src));
		return *this;
	}

#ifndef cnLibrary_CPPEXCLUDE_NULLPTR

	// construct with null
	rPtr(tNullptr)noexcept(true){}

	rPtr& operator =(tNullptr)noexcept(true){
		cnVar::cPtrReference< cnVar::rRefTokenOperator<T> >::operator =(nullptr);
		return *this;
	}
#endif

#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

	rPtr(rPtr &&Src)noexcept(true) : cnVar::cPtrReference< cnVar::rRefTokenOperator<T> >(static_cast<cnVar::cPtrReference< cnVar::rRefTokenOperator<T>&&>(Src)) {}

	rPtr& operator =(rPtr &&Src)noexcept(true){
		cnVar::cPtrReference< cnVar::rRefTokenOperator<T> >::operator =(static_cast<cnVar::cPtrReference< cnVar::rRefTokenOperator<T> >&&>(Src));
		return *this;
	}

#endif // cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

	// construct with token

	template<class TToken>
	explicit rPtr(typename cnVar::cPtrReference< cnVar::rRefTokenOperator<T> >::template ConstructByToken<TToken> ByToken)noexcept(true)
		: cnVar::cPtrReference< cnVar::rRefTokenOperator<T> >(ByToken){}


	
	template<class TSrcRefTokenOperator
#ifndef cnLibrary_CPPEXCLUDE_FUNCTION_TEMPLATE_DEFALT_ARGUMENT
		, class=typename cnLib_THelper::Var_TH::TypeIfPointerTokenFrom<void,void,cnVar::rRefTokenOperator<T>,typename TSrcRefTokenOperator::tRefToken&>::Type
#endif
	>
	rPtr(const cnVar::cPtrReference<TSrcRefTokenOperator> &Src)noexcept(true)
		: cnVar::cPtrReference< cnVar::rRefTokenOperator<T> >(cnVar::rRefTokenOperator<T>::TokenFrom(Src.Token())){}
	
	template<class TSrcRefTokenOperator>
	typename cnLib_THelper::Var_TH::TypeIfPointerTokenFrom<void,rPtr&,
		cnVar::rRefTokenOperator<T>,typename TSrcRefTokenOperator::tRefToken&
	>::Type operator =(const cnVar::cPtrReference<TSrcRefTokenOperator> &Src)noexcept(true){
		cnVar::cPtrReference< cnVar::rRefTokenOperator<T> >::operator =(Src);
		return *this;
	}

	
	// make token with pointer

	//rPtr(T *Pointer)noexcept(true)
//		: cnVar::cPtrReference< cnVar::rRefTokenOperator<T> >(cnVar::cPtrReference< cnVar::rRefTokenOperator<T> >::MakeConstructByToken(cnVar::rRefTokenOperator<T>::TokenFrom(Pointer))){}
	
	// construct by making token

	template<class TSrc
#if !defined(cnLibrary_CPPEXCLUDE_FUNCTION_TEMPLATE_DEFALT_ARGUMENT)
		, class=typename cnLib_THelper::Var_TH::TypeIfPointerTokenFrom<void,void,cnVar::rRefTokenOperator<T>,T cnLib_UREF>::Type
#endif
	>
	rPtr(TSrc cnLib_UREF Src)noexcept(true)
		: cnVar::cPtrReference< cnVar::rRefTokenOperator<T> >(cnVar::cPtrReference< cnVar::rRefTokenOperator<T> >::MakeConstructByToken(cnVar::rRefTokenOperator<T>::TokenFrom(cnLib_UREFCAST(TSrc)(Src))))
	{}

	// assign by making token
	template<class TSrc>
	typename cnLib_THelper::Var_TH::TypeIfPointerTokenFrom<void,rPtr&,cnVar::rRefTokenOperator<T>,TSrc cnLib_UREF>::Type
		operator =(TSrc cnLib_UREF Src)noexcept(true)
	{
		cnVar::rRefToken<T> SwapToken(this->fRefToken);

		this->fRefToken=cnVar::rRefTokenOperator<T>::TokenFrom(cnLib_UREFCAST(TSrc)(Src));

		cnVar::rRefTokenOperator<T>::Acquire(this->fRefToken);
		cnVar::rRefTokenOperator<T>::Release(SwapToken);
		return *this;
	}


#endif	// cnLibrary_CPPFEATURE_INHERIT_CONSTRUCTORS

};

//---------------------------------------------------------------------------
template<class T>
void rIncReference(T *Src)noexcept(true)
{
	static_cast<typename cnVar::TClassReferenceInterface<T>::Type*>(Src)->IncreaseReference();
}
template<class T>
void rDecReference(T *Src)noexcept(true)
{
	static_cast<typename cnVar::TClassReferenceInterface<T>::Type*>(Src)->DecreaseReference();
}
//---------------------------------------------------------------------------
template<class T>
inline void rIncReference(const rPtr<T> &Src)noexcept(true)
{
	Src.ManualIncReference();
}
template<class T>
inline void rDecReference(const rPtr<T> &Src)noexcept(true)
{
	Src.ManualDecReference();
}
//---------------------------------------------------------------------------
template<class T>
inline iReference* rGetReference(T *Src)noexcept(true)
{
	return static_cast<typename cnVar::TClassReferenceInterface<T>::Type*>(Src);
}
//---------------------------------------------------------------------------
template<class T>
rPtr<T> rTakeFromManual(T *Src)noexcept(true)
{
	return rPtr<T>::TakeFromManual(Src);
}
//---------------------------------------------------------------------------
namespace cnVar{
//---------------------------------------------------------------------------
template<class TInterface>
struct iRefToken
{
	TInterface *Pointer;
	iReference *Reference;
};
//---------------------------------------------------------------------------
template<class TInterface>
struct iRefTokenOperator
{
	typedef TInterface *tPtr;
	typedef iRefToken<TInterface> tRefToken;

	static TInterface* Pointer(const tRefToken &Token)noexcept(true){	return Token.Pointer;	}

	static tRefToken TokenNull(void)noexcept(true){
		tRefToken RetToke={nullptr,nullptr};
		return RetToke;
	}
	static tRefToken TokenFrom(tPtr Pointer)noexcept(true){
		tRefToken RetToke;
		RetToke.Pointer=Pointer;
		if(Pointer!=nullptr)
			RetToke.Reference=iCast<iReference>(Pointer);
		else
			RetToke.Reference=nullptr;
		return RetToke;
	}

	template<class TSrcInterface>
	static typename TTypeConditional<tRefToken,
		TIsAssignableFrom<TInterface*&,TSrcInterface*>::Value
	>::Type TokenFrom(iRefToken<TSrcInterface> Src)noexcept(true){
		tRefToken RetToken;
		RetToken.Pointer=Src.Pointer;
		RetToken.Reference=Src.Reference;
		return RetToken;
	}

	static void Acquire(const tRefToken &Token)noexcept(true){
		if(Token.Reference!=nullptr)
			Token.Reference->IncreaseReference();
	}
	static void Release(const tRefToken &Token)noexcept(true){
		if(Token.Reference!=nullptr)
			Token.Reference->DecreaseReference();

	}
};
//---------------------------------------------------------------------------
}	// namespace cnVar
//---------------------------------------------------------------------------



template<class T>
class iPtr : public cnVar::cPtrReference< cnVar::iRefTokenOperator<T> >
{
public:

	template<class TSrcPtr>
	static iPtr TakeFromManual(TSrcPtr *Pointer)noexcept(true){
		return iPtr(cnVar::cPtrReference< cnVar::iRefTokenOperator<T> >::MakeConstructByToken(cnVar::iRefTokenOperator<T>::TokenFrom(Pointer)));
	}

#if cnLibrary_CPPFEATURE_INHERIT_CONSTRUCTORS >= 200802L
	using cnVar::cPtrReference< cnVar::iRefTokenOperator<T> >::cPtrReference;
	using cnVar::cPtrReference< cnVar::iRefTokenOperator<T> >::operator =;

#else	// cnLibrary_CPPFEATURE_INHERIT_CONSTRUCTORS < 200802L

	iPtr()noexcept(true){}

	iPtr(const iPtr &Src)noexcept(true) : cnVar::cPtrReference< cnVar::iRefTokenOperator<T> >(static_cast< const cnVar::cPtrReference< cnVar::iRefTokenOperator<T> >&>(Src)) {}
	iPtr& operator =(const iPtr &Src)noexcept(true){
		cnVar::cPtrReference< cnVar::iRefTokenOperator<T> >::operator =(static_cast< const cnVar::cPtrReference< cnVar::iRefTokenOperator<T> >&>(Src));
		return *this;
	}

#ifndef cnLibrary_CPPEXCLUDE_NULLPTR

	iPtr& operator =(tNullptr)noexcept(true){
		cnVar::cPtrReference< cnVar::iRefTokenOperator<T> >::operator =(nullptr);
		return *this;
	}
#endif

#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

	iPtr(iPtr &&Src)noexcept(true) : cnVar::cPtrReference< cnVar::iRefTokenOperator<T> >(static_cast<cnVar::cPtrReference< cnVar::iRefTokenOperator<T>&&>(Src)) {}

	iPtr& operator =(iPtr &&Src)noexcept(true){
		cnVar::cPtrReference< cnVar::iRefTokenOperator<T> >::operator =(static_cast<cnVar::cPtrReference< cnVar::iRefTokenOperator<T> >&&>(Src));
		return *this;
	}
#endif

	// construct with token

	template<class TToken>
	explicit iPtr(typename cnVar::cPtrReference< cnVar::iRefTokenOperator<T> >::template ConstructByToken<TToken> ByToken)noexcept(true)
		: cnVar::cPtrReference< cnVar::iRefTokenOperator<T> >(ByToken){}


	template<class TSrcRefTokenOperator
#ifndef cnLibrary_CPPEXCLUDE_FUNCTION_TEMPLATE_DEFALT_ARGUMENT
		, class=typename cnLib_THelper::Var_TH::TypeIfPointerTokenFrom<void,void,cnVar::iRefTokenOperator<T>,typename TSrcRefTokenOperator::tRefToken&>::Type
#endif
	>
	iPtr(const cnVar::cPtrReference<TSrcRefTokenOperator> &Src)noexcept(true)
		: cnVar::cPtrReference< cnVar::iRefTokenOperator<T> >(cnVar::iRefTokenOperator<T>::TokenFrom(Src.Token())){}


	template<class TSrcRefTokenOperator>
	typename cnLib_THelper::Var_TH::TypeIfPointerTokenFrom<void,iPtr&,
		cnVar::iRefTokenOperator<T>,typename TSrcRefTokenOperator::tRefToken&
	>::Type operator =(const cnVar::cPtrReference<TSrcRefTokenOperator> &Src)noexcept(true){
		cnVar::cPtrReference< cnVar::iRefTokenOperator<T> >::operator =(Src);
		return *this;
	}

	
	// construct by making token

	template<class TSrc
#if cnLibrary_CPPFEATURE_DECLTYPE >= 200707L && !defined(cnLibrary_CPPEXCLUDE_FUNCTION_TEMPLATE_DEFALT_ARGUMENT)
		, class=typename cnLib_THelper::Var_TH::TypeIfPointerTokenFrom<void,void,cnVar::iRefTokenOperator<T>,TSrc cnLib_UREF>::Type
#endif
	>
	iPtr(TSrc cnLib_UREF Src)noexcept(true)
		: cnVar::cPtrReference< cnVar::iRefTokenOperator<T> >(cnVar::cPtrReference< cnVar::iRefTokenOperator<T> >::MakeConstructByToken(cnVar::iRefTokenOperator<T>::TokenFrom(cnLib_UREFCAST(T)(Src))))
	{}

	// assign by making token
	template<class TSrc>
	typename cnLib_THelper::Var_TH::TypeIfPointerTokenFrom<void,iPtr&,cnVar::iRefTokenOperator<T>,TSrc cnLib_UREF>::Type
		operator =(TSrc cnLib_UREF Src)noexcept(true)
	{
		cnVar::iRefToken<T> SwapToken(this->fRefToken);

		this->fRefToken=cnVar::iRefTokenOperator<T>::TokenFrom(cnLib_UREFCAST(TSrc)(Src));

		cnVar::iRefTokenOperator<T>::Acquire(this->fRefToken);
		cnVar::iRefTokenOperator<T>::Release(SwapToken);
		return *this;
	}

#endif	// cnLibrary_CPPFEATURE_INHERIT_CONSTRUCTORS

	

};

//---------------------------------------------------------------------------
template<class T>
inline iTypeID iTypeOf(iPtr<T> *)noexcept(true){
	return TInterfaceID<T>::Value;
}
//---------------------------------------------------------------------------
template<class IDest,class ISrc>
inline bool iCast(IDest* &Dest,ISrc Src)noexcept(true){
	IDest *pDest=iCast<IDest>(Src);
	Dest=pDest;
	return pDest!=nullptr;
}
//---------------------------------------------------------------------------
template<class IDest,class ISrc>
inline bool iCast(iPtr<IDest> &Dest,ISrc Src)noexcept(true){
	IDest *pDest=iCast<IDest>(Src);
	Dest=pDest;
	return pDest!=nullptr;
}
//---------------------------------------------------------------------------
template<class T>
inline void iIncReference(T *Src)noexcept(true)
{
	iReference *Ref=iCast<iReference>(Src);
	if(Ref!=nullptr)
		Ref->IncreaseReference();
}
template<class T>
inline void iDecReference(T *Src)noexcept(true)
{
	iReference *Ref=iCast<iReference>(Src);
	if(Ref!=nullptr)
		Ref->DecreaseReference();
}
//---------------------------------------------------------------------------
template<class T>
inline void iIncReference(const iPtr<T> &Src)noexcept(true)
{
	Src.ManualIncReference();
}
template<class T>
inline void iDecReference(const iPtr<T> &Src)noexcept(true)
{
	Src.ManualDecReference();
}
//---------------------------------------------------------------------------
template<class T>
inline iReference* iGetReference(const iPtr<T> &Src)noexcept(true)
{
	return Src.Token().Reference;
}
//---------------------------------------------------------------------------
template<class T>
iPtr<T> iTakeFromManual(T *Src)noexcept(true)
{
	return iPtr<T>::TakeFromManual(Src);
}
//---------------------------------------------------------------------------

cnLib_ENUM_BEGIN(sfInt8,iOrdering)
{
	Different=-2,
	Less=-1,
	Equal=0,
	Greater=1,
}cnLib_ENUM_END(iOrdering);


#ifdef cnLibrary_CPPEXCLUDE_NULLPTR
typedef eiOrdering* tOrderZero;
#else
typedef tNullptr tOrderZero;
#endif

#ifndef cnLibrary_CPPEXCLUDE_ENUMCLASS


cnLib_CONSTEXPR_FUNC bool operator ==(eiOrdering o1,eiOrdering o2)noexcept(true){	return static_cast<sfInt8>(o1)==static_cast<sfInt8>(o2);	}
cnLib_CONSTEXPR_FUNC bool operator !=(eiOrdering o1,eiOrdering o2)noexcept(true){	return static_cast<sfInt8>(o1)!=static_cast<sfInt8>(o2);	}

cnLib_CONSTEXPR_FUNC bool operator ==(eiOrdering Order,tOrderZero)noexcept(true){	return Order==iOrdering::Equal;	}
cnLib_CONSTEXPR_FUNC bool operator !=(eiOrdering Order,tOrderZero)noexcept(true){	return Order!=iOrdering::Equal;	}
cnLib_CONSTEXPR_FUNC bool operator <(eiOrdering Order,tOrderZero)noexcept(true){		return Order==iOrdering::Less;	}
cnLib_CONSTEXPR_FUNC bool operator >(eiOrdering Order,tOrderZero)noexcept(true){		return Order==iOrdering::Greater;	}
cnLib_CONSTEXPR_FUNC bool operator <=(eiOrdering Order,tOrderZero)noexcept(true){	return !(((static_cast<ufInt8>(Order)>>1)&1)^(~(static_cast<ufInt8>(Order)&1)));	}
cnLib_CONSTEXPR_FUNC bool operator >=(eiOrdering Order,tOrderZero)noexcept(true){	return Order>=iOrdering::Equal;	}

cnLib_CONSTEXPR_FUNC bool operator ==(tOrderZero,eiOrdering Order)noexcept(true){	return Order==iOrdering::Equal;	}
cnLib_CONSTEXPR_FUNC bool operator !=(tOrderZero,eiOrdering Order)noexcept(true){	return Order!=iOrdering::Equal;	}
cnLib_CONSTEXPR_FUNC bool operator <(tOrderZero,eiOrdering Order)noexcept(true){		return Order==iOrdering::Greater;	}
cnLib_CONSTEXPR_FUNC bool operator >(tOrderZero,eiOrdering Order)noexcept(true){		return Order==iOrdering::Less;	}
cnLib_CONSTEXPR_FUNC bool operator <=(tOrderZero,eiOrdering Order)noexcept(true){	return Order>=iOrdering::Equal;	}
cnLib_CONSTEXPR_FUNC bool operator >=(tOrderZero,eiOrdering Order)noexcept(true){	return !(((static_cast<ufInt8>(Order)>>1)&1)^(~(static_cast<ufInt8>(Order)&1)));	}

#endif //  cnLibrary_CPPEXCLUDE_ENUMCLASS


#if cnLibrary_CPPFEATURE_THREE_WAY_COMPARISION >= 201907L

cnLib_CONSTEXPR_FUNC eiOrdering operator <=>(eiOrdering Order,tOrderZero)noexcept(true){	return Order;	}
cnLib_CONSTEXPR_FUNC eiOrdering operator <=>(tOrderZero,eiOrdering Order)noexcept(true){	return static_cast<iOrdering>(static_cast<ufInt8>(Order)^(~(static_cast<ufInt8>(Order)&1)<<1));	}

#endif	// cnLibrary_CPPFEATURE_THREE_WAY_COMPARISION >= 201907L

template<class TFunction>
class iFunction;
//---------------------------------------------------------------------------
#if cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L

template<class TRet,class...TArgs>
class cnLib_INTERFACE iFunction<TRet (TArgs...)>
{
public:
	virtual TRet cnLib_FUNC Execute(TArgs...)=0;
};

#if cnLibrary_CPPFEATURE_NOEXCEPT_FUNC_TYPE >= 201510L

template<class TRet,class...TArgs>
class cnLib_INTERFACE iFunction<TRet (TArgs...)noexcept>
{
public:
	virtual TRet cnLib_FUNC Execute(TArgs...)noexcept(true)=0;
};

#endif // cnLibrary_CPPFEATURE_NOEXCEPT_FUNC_TYPE >= 201510L


#else	// cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES < 200704L

template<class TRet>
class cnLib_INTERFACE iFunction<TRet (void)>
{
public:
	virtual TRet cnLib_FUNC Execute(void)=0;
};

template<class TRet,class T0>
class cnLib_INTERFACE iFunction<TRet (T0)>
{
public:
	virtual TRet cnLib_FUNC Execute(T0)=0;
};

template<class TRet,class T0,class T1>
class cnLib_INTERFACE iFunction<TRet (T0,T1)>
{
public:
	virtual TRet cnLib_FUNC Execute(T0,T1)=0;
};

template<class TRet,class T0,class T1,class T2>
class cnLib_INTERFACE iFunction<TRet (T0,T1,T2)>
{
public:
	virtual TRet cnLib_FUNC Execute(T0,T1,T2)=0;
};

template<class TRet,class T0,class T1,class T2,class T3>
class cnLib_INTERFACE iFunction<TRet (T0,T1,T2,T3)>
{
public:
	virtual TRet cnLib_FUNC Execute(T0,T1,T2,T3)=0;
};

#if cnLibrary_CPPFEATURE_NOEXCEPT_FUNC_TYPE >= 201510L

template<class TRet>
class cnLib_INTERFACE iFunction<TRet (void)noexcept>
{
public:
	virtual TRet cnLib_FUNC Execute(void)noexcept(true)=0;
};

template<class TRet,class T0>
class cnLib_INTERFACE iFunction<TRet (T0)noexcept>
{
public:
	virtual TRet cnLib_FUNC Execute(T0)noexcept(true)=0;
};

template<class TRet,class T0,class T1>
class cnLib_INTERFACE iFunction<TRet (T0,T1)noexcept>
{
public:
	virtual TRet cnLib_FUNC Execute(T0,T1)noexcept(true)=0;
};

template<class TRet,class T0,class T1,class T2>
class cnLib_INTERFACE iFunction<TRet (T0,T1,T2)noexcept>
{
public:
	virtual TRet cnLib_FUNC Execute(T0,T1,T2)noexcept(true)=0;
};

template<class TRet,class T0,class T1,class T2,class T3>
class cnLib_INTERFACE iFunction<TRet (T0,T1,T2,T3)noexcept>
{
public:
	virtual TRet cnLib_FUNC Execute(T0,T1,T2,T3)noexcept(true)=0;
};

#endif // cnLibrary_CPPFEATURE_NOEXCEPT_FUNC_TYPE >= 201510L

//---------------------------------------------------------------------------
#endif // cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES < 200704L

//---------------------------------------------------------------------------
template<class T>
class cnLib_INTERFACE iArrayReference : public iReference
{
public:
	virtual T* cnLib_FUNC GetArray(uIntn &Length)noexcept(true)=0;
};
typedef iArrayReference<void> iMemoryReference;
typedef iArrayReference<const void> iConstMemoryReference;
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#include <cnTK/TKMacrosCleanup.inc>
//---------------------------------------------------------------------------
#endif	/* cnLibrary_CPPFEATURELEVEL >= 1 */
/*-------------------------------------------------------------------------*/
#endif
