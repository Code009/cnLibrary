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
struct rPointerReferenceOperator
{
	template<class T>
	static void Acquire(T *Pointer)noexcept(true){
		static_cast<typename TClassReferenceInterface<T>::Type*>(Pointer)->IncreaseReference();
	}
	template<class T>
	static void Release(T *Pointer)noexcept(true){
		static_cast<typename TClassReferenceInterface<T>::Type*>(Pointer)->DecreaseReference();
	}
};
//---------------------------------------------------------------------------
}	// namespace cnVar
//---------------------------------------------------------------------------

template<class T>
class rPtr : public cnVar::cPtrReference<T,cnVar::rPointerReferenceOperator>
{
public:

	template<class TSrc>
	static typename cnVar::TTypeConditional<rPtr,
		cnVar::TIsConvertible<TSrc*,T*>::Value
	>::Type TakeFromManual(TSrc *Pointer)noexcept(true)
	{
		typename cnVar::cPtrReference<T,cnVar::rPointerReferenceOperator>::ConstructReferenced Referenced={static_cast<T*>(Pointer)};
		return rPtr(Referenced);
	}

#if cnLibrary_CPPFEATURE_INHERIT_CONSTRUCTORS >= 200802L
	using cnVar::cPtrReference<T,cnVar::rPointerReferenceOperator>::cPtrReference;
	using cnVar::cPtrReference<T,cnVar::rPointerReferenceOperator>::operator =;

#else	// cnLibrary_CPPFEATURE_INHERIT_CONSTRUCTORS < 200802L

	rPtr()noexcept(true){}

	rPtr(const rPtr &Src)noexcept(true) : cnVar::cPtrReference<T,cnVar::rPointerReferenceOperator>( static_cast< const cnVar::cPtrReference<T,cnVar::rPointerReferenceOperator>&>(Src)) {}
	rPtr& operator =(const rPtr &Src)noexcept(true){
		cnVar::cPtrReference<T,cnVar::rPointerReferenceOperator>::operator =(static_cast<const cnVar::cPtrReference<T,cnVar::rPointerReferenceOperator>&>(Src));
		return *this;
	}

#ifndef cnLibrary_CPPEXCLUDE_NULLPTR

	// construct with null
	rPtr(tNullptr)noexcept(true){}

	rPtr& operator =(tNullptr)noexcept(true){
		cnVar::cPtrReference<T,cnVar::rPointerReferenceOperator>::operator =(nullptr);
		return *this;
	}
#endif

#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

	rPtr(rPtr &&Src)noexcept(true) : cnVar::cPtrReference<T,cnVar::rPointerReferenceOperator>(static_cast<cnVar::cPtrReference<T,cnVar::rPointerReferenceOperator>&&>(Src)) {}

	rPtr& operator =(rPtr &&Src)noexcept(true){
		cnVar::cPtrReference<T,cnVar::rPointerReferenceOperator>::operator =(static_cast<cnVar::cPtrReference<T,cnVar::rPointerReferenceOperator>&&>(Src));
		return *this;
	}

	// make token with pointer

	rPtr(T *Pointer)noexcept(true)
		: cnVar::cPtrReference<T,cnVar::rPointerReferenceOperator>(Pointer){}
#endif // cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

	// construct with cPtrReference

	template<class TSrc,class TSrcPointerReferenceOperator,
#ifndef cnLibrary_CPPEXCLUDE_FUNCTION_TEMPLATE_DEFALT_ARGUMENT
		, class=typename cnVar::TTypeConditional<void,
			cnVar::TIsConvertible<TSrc*,T*>::Value
		>::Type
#endif
	>
	rPtr(const cPtrReference<TSrc,TSrcPointerReferenceOperator> &Src)noexcept(true)
		: cnVar::cPtrReference<T,cnVar::rPointerReferenceOperator>(Src){}


	// assign with cPtrReference

	template<class TSrc,class TSrcPointerReferenceOperator>
	typename cnVar::TTypeConditional<cPtrReference&,
		cnVar::TIsConvertible<TSrc*,T*>::Value
	>::Type operator =(const cPtrReference<TSrc,TSrcPointerReferenceOperator> &Src)noexcept(true){
		cnVar::cPtrReference<T,cnVar::rPointerReferenceOperator>::operator =(Src);
		return *this;
	}


#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

	// move construct with cPtrReference

	template<class TSrc,
#ifndef cnLibrary_CPPEXCLUDE_FUNCTION_TEMPLATE_DEFALT_ARGUMENT
		, class=typename cnVar::TTypeConditional<void,
			cnVar::TIsConvertible<TSrc*,T*>::Value
		>::Type
#endif
	>
	rPtr(cPtrReference<TSrc,cnVar::rPointerReferenceOperator> &&Src)noexcept(true)
		: cnVar::cPtrReference<T,cnVar::rPointerReferenceOperator>(static_cast<cPtrReference<TSrc,cnVar::rPointerReferenceOperator>&&>(Src)){}


	// move assign from cPtrReference

	template<class TSrc>
	typename cnVar::TTypeConditional<cPtrReference&,
		cnVar::TIsConvertible<TSrc*,T*>::Value
	>::Type operator =(cPtrReference<TSrc,cnVar::rPointerReferenceOperator> &&Src)noexcept(true){
		cnVar::cPtrReference<T,cnVar::rPointerReferenceOperator>::operator =(static_cast<cPtrReference<TSrc,cnVar::rPointerReferenceOperator>&&>(Src));
		return *this;
	}

#endif // cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L
	
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
struct iPointerReferenceOperator
{
	template<class TInterface>
	static void Acquire(TInterface *Pointer)noexcept(true){
		iReference *Reference=iCast<iReference>(Pointer);
		if(Reference!=nullptr){
			Reference->IncreaseReference();
		}
	}
	template<class TInterface>
	static void Release(TInterface *Pointer)noexcept(true){
		iReference *Reference=iCast<iReference>(Pointer);
		if(Reference!=nullptr){
			Reference->DecreaseReference();
		}
	}
};
//---------------------------------------------------------------------------
}	// namespace cnVar
//---------------------------------------------------------------------------



template<class TInterface>
class iPtr
{
public:
	//typedef TInterface tInterface;

	TInterface* Pointer(void)const noexcept(true){	return fPointer;	}
	iReference* Reference(void)const noexcept(true){	return fReference;	}

	operator TInterface* ()const noexcept(true){	return fPointer;	}
	TInterface* operator ->()const noexcept(true){	return fPointer;	}

	iPtr()noexcept(true)
		: fPointer(nullptr)
		, fReference(nullptr)
	{}
	~iPtr()noexcept(true){
		ManualDecReference();
	}

#ifndef cnLibrary_CPPEXCLUDE_NULLPTR

	// construct with null
	iPtr(tNullptr)noexcept(true)
		: fPointer(nullptr)
		, fReference(nullptr)
	{}
	// assign with null
	iPtr& operator =(tNullptr)noexcept(true){
		Clear();
		return *this;
	}
#endif

	iPtr(TInterface *Pointer)noexcept(true)
		: fPointer(Pointer)
	{
		if(Pointer!=nullptr){
			fReference=iCast<iReference>(Pointer);
			fReference->IncreaseReference();
		}
		else{
			fReference=nullptr;
		}
	}

	iPtr& operator = (TInterface *Pointer)noexcept(true)
	{
		if(fPointer!=Pointer){
			if(Pointer!=nullptr){
				iReference *SwapReference=fReference;
				fReference=iCast<iReference>(Pointer);
				fReference->IncreaseReference();
				fPointer=Pointer;

				if(SwapReference!=nullptr)
					SwapReference->DecreaseReference();
			}
			else{
				fReference=nullptr;
				fPointer=nullptr;
			}
		}
		return *this;
	}

	void Clear(void)noexcept(true){
		if(fReference!=nullptr){
			fReference->DecreaseReference();
			fReference=nullptr;
			fPointer=nullptr;
		}
	}

protected:
	TInterface *fPointer;
	iReference *fReference;

	struct ConstructToken
	{
		TInterface *Pointer;
		iReference *Reference;
	};

	// construct with token
	explicit iPtr(ConstructToken Token)noexcept(true)
		: fPointer(Token.Pointer)
		, fReference(Token.Reference)
	{
	}

public:

	template<class TSrcPtr>
	static iPtr TakeFromManual(TSrcPtr *Pointer)noexcept(true)
	{
		ConstructToken Token;
		Token.Pointer=Pointer;
		if(Pointer!=nullptr)
			Token.Reference=iCast<iReference>(Pointer);
		else
			Token.Reference=nullptr;
		return iPtr(Token);
	}
	template<class TSrcPtr>
	static iPtr TakeFromManual(TSrcPtr *Pointer,iReference *Reference)noexcept(true)
	{
		ConstructToken Token;
		Token.Pointer=Pointer;
		Token.Reference=Reference;
		return iPtr(Token);
	}


	TInterface* ExtractToManual(void)noexcept(true){
		TInterface *Ptr=fPointer;
		fPointer=nullptr;
		fReference=nullptr;
		return Ptr;
	}

	void ManualIncReference(void)const noexcept(true){
		if(fReference!=nullptr)
			fReference->IncreaseReference();
	}
	void ManualDecReference(void)const noexcept(true){
		if(fReference!=nullptr)
			fReference->DecreaseReference();
	}


	// copy constructor

	iPtr(const iPtr &Src)noexcept(true)
		: fPointer(Src.fPointer)
		, fReference(Src.fReference)
	{
		ManualIncReference();
	}

	// assign

	iPtr& operator =(const iPtr &Src)noexcept(true){
		if(this!=&Src){
			ManualDecReference();

			fPointer=Src.fPointer;
			fReference=Src.fReference;

			ManualIncReference();
		}
		return *this;
	}
#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

	// move constructor

	iPtr(iPtr &&Src)noexcept(true)
		: fPointer(Src.fPointer)
		, fReference(Src.fReference)
	{
		Src.fPointer=nullptr;
		Src.fReference=nullptr;
	}

	// move

	iPtr& operator =(iPtr &&Src)noexcept(true){
		ManualDecReference();

		fPointer=Src.fPointer;
		fReference=Src.fReference;

		Src.fPointer=nullptr;
		Src.fReference=nullptr;
		return *this;
	}

#endif // cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L


	// construct with other iPtr


	template<class TSrcInterface
#ifndef cnLibrary_CPPEXCLUDE_FUNCTION_TEMPLATE_DEFALT_ARGUMENT
		, class=typename cnVar::TTypeConditional<void,
			cnVar::TIsConvertible<TSrcInterface*,TInterface*>::Value
		>::Type
#endif
	>
	iPtr(const iPtr<TSrcInterface> &Src)noexcept(true)
		: fPointer(Src.Pointer())
		, fReference(Src.Reference())
	{
		ManualIncReference();
	}


	// assign from other iPtr

	template<class TSrcInterface>
	typename cnVar::TTypeConditional<iPtr&,
		cnVar::TIsConvertible<TSrcInterface*,TInterface*>::Value
	>::Type operator =(const iPtr<TSrcInterface> &Src)noexcept(true){
		ManualDecReference();

		fPointer=Src.Pointer();
		fReference=Src.Reference();

		ManualIncReference();
		return *this;
	}


	// construct from cPtrReference

	template<class TSrcInterface,class TSrcPointerReferenceOperator
#ifndef cnLibrary_CPPEXCLUDE_FUNCTION_TEMPLATE_DEFALT_ARGUMENT
		, class=typename cnVar::TTypeConditional<void,
			cnVar::TIsConvertible<TSrcInterface*,TInterface*>::Value
		>::Type
#endif
	>
	iPtr(const cnVar::cPtrReference<TSrcInterface,TSrcPointerReferenceOperator> &Src)noexcept(true)
		: fPointer(Src.Pointer())
	{
		if(fPointer!=nullptr){
			fReference=iCast<iReference>(fPointer);
			if(fReference!=nullptr)
				fReference->IncreaseReference();
		}
		else{
			fReference=nullptr;
		}

	}


	// assign from cPtrReference

	template<class TSrcInterface,class TSrcPointerReferenceOperator>
	typename cnVar::TTypeConditional<iPtr&,
		cnVar::TIsConvertible<TSrcInterface*,TInterface*>::Value
	>::Type operator =(const cnVar::cPtrReference<TSrcInterface,TSrcPointerReferenceOperator> &Src)noexcept(true){
		iReference* SwapReference=fReference;

		fPointer=Src.Pointer();
		if(fPointer!=nullptr){
			fReference=iCast<iReference>(fPointer);
			if(fReference!=nullptr)
				fReference->IncreaseReference();
		}
		else{
			fReference=nullptr;
		}
		if(SwapReference!=nullptr)
			SwapReference->DecreaseReference();
		return *this;
	}

#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

	// move construct from cPtrReference

	template<class TSrcInterface
#ifndef cnLibrary_CPPEXCLUDE_FUNCTION_TEMPLATE_DEFALT_ARGUMENT
		, class=typename cnVar::TTypeConditional<void,
			cnVar::TIsConvertible<TSrcInterface*,TInterface*>::Value
		>::Type
#endif
	>
	iPtr(cnVar::cPtrReference<TSrcInterface,cnVar::iPointerReferenceOperator> &&Src)noexcept(true)
		: fPointer(Src.ExtractToManual())
	{
		if(fPointer!=nullptr){
			fReference=iCast<iReference>(fPointer);
		}
	}


	// move assign from cPtrReference

	template<class TSrcInterface>
	typename cnVar::TTypeConditional<iPtr&,
		cnVar::TIsConvertible<TSrcInterface*,TInterface*>::Value
	>::Type operator =(cnVar::cPtrReference<TSrcInterface,cnVar::iPointerReferenceOperator> &&Src)noexcept(true){
		iReference* SwapReference=fReference;

		fPointer=Src.ExtractToManual();
		if(fPointer!=nullptr){
			fReference=iCast<iReference>(fPointer);
		}
		if(SwapReference!=nullptr)
			SwapReference->DecreaseReference();
		return *this;
	}

#endif // cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

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
cnLib_CONSTEXPR_FUNC bool operator <(eiOrdering Order,tOrderZero)noexcept(true){	return Order==iOrdering::Less;	}
cnLib_CONSTEXPR_FUNC bool operator >(eiOrdering Order,tOrderZero)noexcept(true){	return Order==iOrdering::Greater;	}
cnLib_CONSTEXPR_FUNC bool operator <=(eiOrdering Order,tOrderZero)noexcept(true){	return !(((static_cast<ufInt8>(Order)>>1)&1)^(~(static_cast<ufInt8>(Order)&1)));	}
cnLib_CONSTEXPR_FUNC bool operator >=(eiOrdering Order,tOrderZero)noexcept(true){	return Order>=iOrdering::Equal;	}

cnLib_CONSTEXPR_FUNC bool operator ==(tOrderZero,eiOrdering Order)noexcept(true){	return Order==iOrdering::Equal;	}
cnLib_CONSTEXPR_FUNC bool operator !=(tOrderZero,eiOrdering Order)noexcept(true){	return Order!=iOrdering::Equal;	}
cnLib_CONSTEXPR_FUNC bool operator <(tOrderZero,eiOrdering Order)noexcept(true){	return Order==iOrdering::Greater;	}
cnLib_CONSTEXPR_FUNC bool operator >(tOrderZero,eiOrdering Order)noexcept(true){	return Order==iOrdering::Less;	}
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
class cnLib_INTERFACE iReferenceObserver
{
public:
	virtual void cnLib_FUNC Close(void)noexcept(true)=0;
	virtual bool cnLib_FUNC Reference(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iObservedReference : public iReference
{
public:
	typedef iObservedReference tReferenceInterface;
	struct tInterfaceID{	static iTypeID Value;	};

	virtual	iReferenceObserver* cnLib_FUNC CreateReferenceObserver(iReference *NotifyReference,iFunction<void (void)noexcept(true)> *NotifyProcedure)noexcept(true)=0;
};
template<> struct TInterfaceID<iObservedReference>:iObservedReference::tInterfaceID{	typedef iObservedReference Type;	};
//---------------------------------------------------------------------------
namespace cnVar{
//---------------------------------------------------------------------------
struct rPointerWeakReferenceOperator
{
	typedef iReferenceObserver tToken;

	template<class T>
	static tToken* Register(T *Pointer)noexcept(true){
		return Pointer->CreateReferenceObserver(nullptr,nullptr);
	}
	static void Unregister(tToken *Token)noexcept(true){
		return Token->Close();
	}

	template<class T>
	struct tReference
		: cnVar::TTypeDef< rPtr<T> >{};

	template<class T>
	static rPtr<T> Reference(tToken *Token,T *Pointer)noexcept(true){
		return Token->Reference()?rPtr<T>::TakeFromManual(Pointer):nullptr;
	}
};
//---------------------------------------------------------------------------
struct iPointerWeakReferenceOperator
{
	typedef iReferenceObserver tToken;

	template<class T>
	static tToken* Register(T *Pointer)noexcept(true){
		iObservedReference *Reference=iCast<iObservedReference>(Pointer);
		return Reference->CreateReferenceObserver(nullptr,nullptr);
	}
	static void Unregister(tToken *Token)noexcept(true){
		return Token->Close();
	}

	template<class T>
	struct tReference
		: cnVar::TTypeDef< rPtr<T> >{};

	template<class T>
	static rPtr<T> Reference(tToken *Token,T *Pointer)noexcept(true){
		iObservedReference *Reference=iCast<iObservedReference>(Pointer);
		return Token->Reference()?iPtr<T>::TakeFromManual(Pointer,Reference):nullptr;
	}
};
//---------------------------------------------------------------------------
}	// namespace cnVar
//---------------------------------------------------------------------------
template<class T>
class rWeakPtr : public cnVar::cPtrWeakReference<T,cnVar::rPointerWeakReferenceOperator>
{
public:

#if cnLibrary_CPPFEATURE_INHERIT_CONSTRUCTORS >= 200802L
	using cnVar::cPtrWeakReference<T,cnVar::rPointerWeakReferenceOperator>::cPtrWeakReference;
	using cnVar::cPtrWeakReference<T,cnVar::rPointerWeakReferenceOperator>::operator =;

#else	// cnLibrary_CPPFEATURE_INHERIT_CONSTRUCTORS < 200802L

	rWeakPtr()noexcept(true){}

	rWeakPtr(const rWeakPtr &Src)noexcept(true) : cnVar::cPtrWeakReference<T,cnVar::rPointerWeakReferenceOperator>( static_cast< const cnVar::cPtrReference<T,cnVar::rPointerWeakReferenceOperator>&>(Src)) {}
	rWeakPtr& operator =(const rWeakPtr &Src)noexcept(true){
		cnVar::cPtrWeakReference<T,cnVar::rPointerWeakReferenceOperator>::operator =(static_cast<const cnVar::cPtrReference<T,cnVar::rPointerWeakReferenceOperator>&>(Src));
		return *this;
	}

#ifndef cnLibrary_CPPEXCLUDE_NULLPTR

	// construct with null
	rWeakPtr(tNullptr)noexcept(true){}

	rWeakPtr& operator =(tNullptr)noexcept(true){
		cnVar::cPtrWeakReference<T,cnVar::rPointerWeakReferenceOperator>::operator =(nullptr);
		return *this;
	}
#endif

#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

	rWeakPtr(rWeakPtr &&Src)noexcept(true) : cnVar::cPtrWeakReference<T,cnVar::rPointerWeakReferenceOperator>(static_cast<cnVar::cPtrReference<T,cnVar::rPointerWeakReferenceOperator>&&>(Src)) {}

	rWeakPtr& operator =(rWeakPtr &&Src)noexcept(true){
		cnVar::cPtrWeakReference<T,cnVar::rPointerWeakReferenceOperator>::operator =(static_cast<cnVar::cPtrReference<T,cnVar::rPointerWeakReferenceOperator>&&>(Src));
		return *this;
	}

	// make token with pointer

	rWeakPtr(T *Pointer)noexcept(true)
		: cnVar::cPtrWeakReference<T,cnVar::rPointerWeakReferenceOperator>(Pointer){}
#endif // cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

	// construct with cPtrReference

	template<class TSrc,class TSrcPointerReferenceOperator,
#ifndef cnLibrary_CPPEXCLUDE_FUNCTION_TEMPLATE_DEFALT_ARGUMENT
		, class=typename cnVar::TTypeConditional<void,
		cnVar::TIsConvertible<TSrc*,T*>::Value
		>::Type
#endif
	>
	rWeakPtr(const cPtrReference<TSrc,TSrcPointerReferenceOperator> &Src)noexcept(true)
		: cnVar::cPtrReference<T,cnVar::rPointerWeakReferenceOperator>(Src){}


	// assign with cPtrReference

	template<class TSrc,class TSrcPointerReferenceOperator>
	typename cnVar::TTypeConditional<cPtrReference&,
		cnVar::TIsConvertible<TSrc*,T*>::Value
	>::Type operator =(const cPtrReference<TSrc,TSrcPointerReferenceOperator> &Src)noexcept(true){
		cnVar::cPtrReference<T,cnVar::rPointerWeakReferenceOperator>::operator =(Src);
		return *this;
	}


#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

	// move construct with cPtrReference

	template<class TSrc,
#ifndef cnLibrary_CPPEXCLUDE_FUNCTION_TEMPLATE_DEFALT_ARGUMENT
		, class=typename cnVar::TTypeConditional<void,
		cnVar::TIsConvertible<TSrc*,T*>::Value
		>::Type
#endif
	>
	rWeakPtr(cPtrReference<TSrc,cnVar::rPointerWeakReferenceOperator> &&Src)noexcept(true)
		: cnVar::cPtrReference<T,cnVar::rPointerWeakReferenceOperator>(static_cast<cPtrReference<TSrc,cnVar::rPointerWeakReferenceOperator>&&>(Src)){}


	// move assign from cPtrReference

	template<class TSrc>
	typename cnVar::TTypeConditional<cPtrReference&,
		cnVar::TIsConvertible<TSrc*,T*>::Value
	>::Type operator =(cPtrReference<TSrc,cnVar::rPointerWeakReferenceOperator> &&Src)noexcept(true){
		cnVar::cPtrReference<T,cnVar::rPointerWeakReferenceOperator>::operator =(static_cast<cPtrReference<TSrc,cnVar::rPointerWeakReferenceOperator>&&>(Src));
		return *this;
	}

#endif // cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

#endif	// cnLibrary_CPPFEATURE_INHERIT_CONSTRUCTORS

};
//---------------------------------------------------------------------------
template<class TInterface>
class iWeakPtr
{
public:
	iWeakPtr()noexcept(true)
		: fObserver(nullptr)
		, fPointer(nullptr)
		, fReference(nullptr)
	{}
	~iWeakPtr()noexcept(true){
		if(fObserver!=nullptr){
			fObserver->Close();
		}
	}

#ifndef cnLibrary_CPPEXCLUDE_NULLPTR

	// construct with null
	iWeakPtr(tNullptr)noexcept(true)
		: fObserver(nullptr)
		, fPointer(nullptr)
		, fReference(nullptr)
	{}
	// assign with null
	iWeakPtr& operator =(tNullptr)noexcept(true){
		Clear();
		return *this;
	}
#endif

	iWeakPtr(TInterface *Pointer)noexcept(true){
		if(Pointer!=nullptr){
			iObservedReference *Reference=iCast<iObservedReference>(Pointer);
			fObserver=Reference->CreateReferenceObserver(nullptr,nullptr);
			if(fObserver!=nullptr){
				fPointer=Pointer;
				fReference=Reference;
			}
			else{
				fReference=nullptr;
			}
		}
		else{
			fObserver=nullptr;
			fPointer=nullptr;
			fReference=nullptr;
		}
	}

	iWeakPtr& operator = (TInterface *Pointer)noexcept(true){
		Clear();
		if(Pointer!=nullptr){
			iObservedReference *Reference=iCast<iObservedReference>(Pointer);
			fObserver=Reference->CreateReferenceObserver(nullptr,nullptr);
			if(fObserver!=nullptr){
				fPointer=Pointer;
				fReference=Reference;
			}
		}
	}

	void Clear(void)noexcept(true){
		if(fObserver!=nullptr){
			fObserver->Close();
			fObserver=nullptr;
			fPointer=nullptr;
			fReference=nullptr;
		}
	}

	iPtr<TInterface> Ref(void)noexcept(true){
		if(fObserver==nullptr)
			return nullptr;

		if(fObserver->Reference()){
			return iPtr<TInterface>::TakeFromManual(fPointer,fReference);
		}

		fObserver->Close();
		fObserver=nullptr;
		fPointer=nullptr;
		fReference=nullptr;
		return nullptr;
	}
	iPtr<TInterface> operator + (void)noexcept(true){	return Ref();	}


protected:
	iReferenceObserver *fObserver;
	TInterface *fPointer;
	iObservedReference *fReference;

public:

	// copy constructor

	iWeakPtr(const iWeakPtr &Src)noexcept(true){
		if(Src.fObserver!=nullptr){
			TInterface *Pointer=Src.fPointer;
			iObservedReference *Reference=Src.fReference;
			if(Src.fObserver->ObserverReference()){
				fObserver=Reference->CreateReferenceObserver(nullptr,nullptr);
				if(fObserver!=nullptr){
					fPointer=Pointer;
					fReference=Reference;
				}
				else{
					fPointer=nullptr;
					fReference=nullptr;
				}
				Reference->DecreaseReference();
			}
			else{
				fObserver=nullptr;
				fPointer=nullptr;
				fReference=nullptr;
			}
		}
		else{
			fObserver=nullptr;
			fPointer=nullptr;
			fReference=nullptr;
		}
	}

	// assign

	iWeakPtr& operator =(const iWeakPtr &Src)noexcept(true){
		if(this!=&Src){
			Clear();
			if(Src.fObserver!=nullptr){
				if(Src.fObserver->ObserverReference()){
					TInterface *Pointer=Src.fPointer;
					iObservedReference *Reference=Src.fReference;
					fObserver=Src.fReference->ObserverRegister(nullptr,nullptr);
					if(fObserver!=nullptr){
						fPointer=Pointer;
						fReference=Reference;
					}
					Reference->DecreaseReference();
				}
			}
		}
		return *this;
	}
#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

	// move constructor

	iWeakPtr(iWeakPtr &&Src)noexcept(true)
		: fObserver(Src.fObserver)
		, fPointer(Src.fPointer)
		, fReference(Src.fReference)
	{
		Src.fObserver=nullptr;
		Src.fPointer=nullptr;
		Src.fReference=nullptr;
	}

	// move

	iWeakPtr& operator =(iWeakPtr &&Src)noexcept(true){
		if(fObserver!=nullptr){
			fObserver->Close();
		}

		fObserver=Src.fObserver;
		fPointer=Src.fPointer;
		fReference=Src.fReference;

		Src.fObserver=nullptr;
		Src.fPointer=nullptr;
		Src.fReference=nullptr;
		return *this;
	}

#endif // cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L


	// construct from cPtrReference

	template<class TSrcInterface,class TSrcPointerReferenceOperator
#ifndef cnLibrary_CPPEXCLUDE_FUNCTION_TEMPLATE_DEFALT_ARGUMENT
		, class=typename cnVar::TTypeConditional<void,
		cnVar::TIsAssignableFrom<TSrcInterface*&,TSrcInterface*>::Value
		>::Type
#endif
	>
	iWeakPtr(const cnVar::cPtrReference<TSrcInterface,TSrcPointerReferenceOperator> &Src)noexcept(true)
	{
		auto Pointer=Src.Pointer();
		if(Pointer!=nullptr){
			iObservedReference *Reference=iCast<iObservedReference>(Pointer);
			fObserver=Reference->CreateReferenceObserver(nullptr,nullptr);
			if(fObserver!=nullptr){
				fPointer=Pointer;
				fReference=Reference;
			}
			else{
				fReference=nullptr;
			}
		}
		else{
			fObserver=nullptr;
			fPointer=nullptr;
			fReference=nullptr;
		}
	}


	// assign from cPtrReference

	template<class TSrcInterface,class TSrcPointerReferenceOperator>
	typename cnVar::TTypeConditional<iWeakPtr&,
		cnVar::TIsAssignableFrom<TInterface*&,TSrcInterface*>::Value
	>::Type operator =(const cnVar::cPtrReference<TSrcInterface,TSrcPointerReferenceOperator> &Src)noexcept(true){
		Clear();
		auto Pointer=Src.Pointer();
		if(Pointer!=nullptr){
			iObservedReference *Reference=iCast<iObservedReference>(Pointer);
			fObserver=Reference->CreateReferenceObserver(nullptr,nullptr);
			if(fObserver!=nullptr){
				fPointer=Pointer;
				fReference=Reference;
			}
		}
	}

};
//---------------------------------------------------------------------------
template<class T>
class cnLib_INTERFACE iDataReference : public iReference
{
public:
	virtual T cnLib_FUNC Get(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
typedef iDataReference< cArray<const uChar16> > iStringReference;
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#include <cnTK/TKMacrosCleanup.inc>
//---------------------------------------------------------------------------
#endif	/* cnLibrary_CPPFEATURELEVEL >= 1 */
/*-------------------------------------------------------------------------*/
#endif
