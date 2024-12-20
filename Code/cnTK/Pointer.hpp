/*- cnTK - Pointer Management ---------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2018-08-14                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnTK_Pointer_HPP__
#define	__cnLibrary_cnTK_Pointer_HPP__
/*-------------------------------------------------------------------------*/
#include <cnTK/Common.hpp>
#include <cnTK/TypeTraits.hpp>
/*-------------------------------------------------------------------------*/
#if	cnLibrary_CPPFEATURELEVEL >= 1
//---------------------------------------------------------------------------
#include <cnTK/TKMacrosDeclare.inc>
//---------------------------------------------------------------------------
namespace cnLib_THelper{
//---------------------------------------------------------------------------
namespace Memory_TH{
//---------------------------------------------------------------------------

template<class TEnable,class T>
struct TAddressOf
{
	static cnLib_CONSTEXPR_FUNC T* Call(T &Object)noexcept(true)
	{
		return &Object;
	}
};


#if !defined(cnLibrary_CPPEXCLUDE_SFINAE_DECLTYPE_EXPRESSION) && cnLibrary_CPPFEATURE_DECLTYPE >= 200707L

template<class T>
struct TAddressOf<decltype(UnusedParameter(&T::operator &)),T>
{
	static cnLib_CONSTEXPR_FUNC T* Call(T &Object)noexcept(true)
	{
		typedef typename cnVar::TCopyCV<char,T>::Type TInterpret;
		return reinterpret_cast<T*>(&reinterpret_cast<TInterpret&>(Object));
	}
};

// !defined(cnLibrary_CPPEXCLUDE_SFINAE_DECLTYPE_EXPRESSION) && cnLibrary_CPPFEATURE_DECLTYPE >= 200707L
#else
// defined(cnLibrary_CPPEXCLUDE_SFINAE_DECLTYPE_EXPRESSION) || cnLibrary_CPPFEATURE_DECLTYPE < 200707L

#ifndef cnLibrary_CPPEXCLUDE_SFINAE_SIZEOF_EXPRESSION

template<class T>
struct TAddressOf<typename cnVar::TTypeConditional<void,sizeof(&T::operator &)>::Type,T>
{
	static cnLib_CONSTEXPR_FUNC T* Call(T &Object)noexcept(true)
	{
		typedef typename cnVar::TCopyCV<char,T>::Type TInterpret;
		return reinterpret_cast<T*>(&reinterpret_cast<TInterpret&>(Object));
	}
};

#endif	// !cnLibrary_CPPEXCLUDE_SFINAE_SIZEOF_EXPRESSION

#endif // defined(cnLibrary_CPPEXCLUDE_SFINAE_DECLTYPE_EXPRESSION) || cnLibrary_CPPFEATURE_DECLTYPE < 200707L


//---------------------------------------------------------------------------
}	// namespace Memory_TH{
//---------------------------------------------------------------------------
}	// namespace cnLib_THelper
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnMemory{
//---------------------------------------------------------------------------

// PointerAddByteOffset
//	offset pointer by number of bytes
// Pointer		pointer to calculate
// OffsetByte	number of bytes to add to the pointer
// return offseted pointer
template<class TPtr>
inline TPtr* PointerAddByteOffset(TPtr *Pointer,sIntn OffsetByte)noexcept(true)
{
	return reinterpret_cast<TPtr*>(
		reinterpret_cast<uIntn>(Pointer)+OffsetByte
	);
}

inline uIntn PointerDistance(const void *Dest,const void *Src)noexcept(true)
{
	return reinterpret_cast<uIntn>(static_cast<const uInt8*>(Dest)-reinterpret_cast<uIntn>(Src));
}

//---------------------------------------------------------------------------

// CastPointerAddByteOffset
//	offset pointer by number of bytes
// Pointer		pointer to calculate
// OffsetByte	number of bytes to add to the pointer
// return offseted and casted pointer
template<class TRet,class TPtr>
inline TRet* CastPointerAddByteOffset(TPtr *Pointer,sIntn OffsetByte)noexcept(true)
{
	return reinterpret_cast<TRet*>(
		reinterpret_cast<uIntn>(Pointer)+OffsetByte
	);
}

// AddressOf
//	get address of object, bypass overloaded operator
// Object		reference to the object
// return address of the object
template<class T>
inline cnLib_CONSTEXPR_FUNC T* AddressOf(T &Object)noexcept(true)
{
	return cnLib_THelper::Memory_TH::TAddressOf<void,T>::Call(Object);
}

// MemberOffset
//	get offset of a class member
// MemberPointer		pointer to class member
// return offset of the member
template<class TMemberClass,class TMember>
inline uIntn MemberOffset(TMember TMemberClass::* MemberPointer)noexcept(true)
{
	return reinterpret_cast<uIntn>( AddressOf(static_cast<TMemberClass*>(nullptr)->*MemberPointer) );
}

// GetObjectFromMemberPointer
//	caculate object address by pointer to member
// Pointer				pointer to member
// MemberPointer		class member pointer
// return pointer to class object
template<class TMemberClass,class TMember,class TPointer>
inline typename cnVar::TCopyCV<TMemberClass,TPointer>::Type* GetObjectFromMemberPointer(TPointer *Pointer,TMember TMemberClass::*MemberPointer)noexcept(true)
{
	typedef const volatile uInt8 TInterpret;
	TInterpret *MemberOffsetPointer=&reinterpret_cast<TInterpret&>(static_cast<TPointer&>(static_cast<TMemberClass*>(nullptr)->*MemberPointer));
	return reinterpret_cast<typename cnVar::TCopyCV<TMemberClass,TPointer>::Type*>(reinterpret_cast<TInterpret*>(Pointer)-MemberOffsetPointer);
}

// GetObjectFromArrayMemberPointer
//	caculate object address by pointer to member
// Pointer				pointer to member
// Index				index of the member in the array
// MemberPointer		class array member pointer
// return pointer to class object
template<class TMemberClass,class TArrayMember,class TPointer>
inline typename cnVar::TCopyCV<TMemberClass,TPointer>::Type* GetObjectFromArrayMemberPointer(TPointer *Pointer,uIntn Index,TArrayMember TMemberClass::*MemberPointer)noexcept(true)
{
	typedef const volatile uInt8 TInterpret;
	TInterpret *MemberOffsetPointer=reinterpret_cast<TInterpret*>(static_cast<TPointer*>(Index+static_cast<TMemberClass*>(nullptr)->*MemberPointer));
	return reinterpret_cast<typename cnVar::TCopyCV<TMemberClass,TPointer>::Type*>(reinterpret_cast<TInterpret*>(Pointer)-MemberOffsetPointer);
}

//---------------------------------------------------------------------------
}	// namespace cnMemory
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
namespace cnLib_THelper{
//---------------------------------------------------------------------------
namespace Var_TH{
//---------------------------------------------------------------------------

template<class TEnable,class T,class TTokenOperator,class TSrc>
struct TypeIfPointerTokenFrom
{
};

#if !defined(cnLibrary_CPPEXCLUDE_SFINAE_DECLTYPE_EXPRESSION) && cnLibrary_CPPFEATURE_DECLTYPE >= 200707L

template<class T,class TTokenOperator,class TSrc>
struct TypeIfPointerTokenFrom<decltype(static_cast<void>(TTokenOperator::TokenFrom(cnVar::DeclVal<TSrc>()))),
	T,TTokenOperator,TSrc>
{
	typedef T Type;
};

#elif !defined(cnLibrary_CPPEXCLUDE_SFINAE_SIZEOF_EXPRESSION)

template<class T,class TTokenOperator,class TSrc>
struct TypeIfPointerTokenFrom<typename cnVar::TSelect<!sizeof(TTokenOperator::TokenFrom(cnVar::DeclVal<TSrc>())),void>::Type,
	T,TTokenOperator,TSrc>
{
	typedef T Type;
};

#else

template<class T,class TTokenOperator,class TSrc>
struct TypeIfPointerTokenFrom<void,
	T,TTokenOperator,TSrc>
{
	typedef T Type;
};


#endif	// cnLibrary_CPPFEATURE_DECLTYPE < 200707L

//---------------------------------------------------------------------------
}	// namespace Var_TH
//---------------------------------------------------------------------------
}	// namespace cnLib_THelper
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnVar{
//---------------------------------------------------------------------------
//TPointerOwnerOperator
//{
//	static void Release(T* &Pointer)noexcept;
//};
//---------------------------------------------------------------------------
template<class T,class TPointerOwnerOperator>
class cPtrOwner
{
public:
	T* Pointer(void)const noexcept(true){	return fPointer;	}
	T*& Pointer(void)noexcept(true){		return fPointer;	}

	operator T* ()const noexcept(true){		return fPointer;	}
	T* operator ->()const noexcept(true){	return fPointer;	}

	cPtrOwner()noexcept(true)
		: fPointer(nullptr)
	{}
	~cPtrOwner()noexcept(true){
		if(fPointer!=nullptr)
			TPointerOwnerOperator::Release(fPointer);
	}

#ifndef cnLibrary_CPPEXCLUDE_NULLPTR
	// construct with null
	cPtrOwner(tNullptr)noexcept(true)
		: fPointer(nullptr)
	{}
	// assign with null

	cPtrOwner& operator =(tNullptr)noexcept(true){
		Clear();
		return *this;
	}
#endif


	void Clear(void)noexcept(true){
		if(fPointer!=nullptr){
			TPointerOwnerOperator::Release(fPointer);
			fPointer=nullptr;
		}
	}

#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

	// move construct
	cPtrOwner(cPtrOwner &&Src)noexcept(true)
		: fPointer(Src.fPointer)
	{
		Src.fPointer=nullptr;
	}

	// move
	cPtrOwner& operator =(cPtrOwner &&Src)noexcept(true){
		if(fPointer!=nullptr)
			TPointerOwnerOperator::Release(fPointer);

		fPointer=Src.fPointer;
		Src.fPointer=nullptr;
		return *this;
	}


	// move construct with token from cPtrOwner
	
	template<class TSrc
#ifndef cnLibrary_CPPEXCLUDE_FUNCTION_TEMPLATE_DEFALT_ARGUMENT
		, class=typename TTypeConditional<void,
			TIsConvertible<TSrc*,T*>::Value
		>::Type
#endif
	>
	cPtrOwner(cPtrOwner<TSrc,TPointerOwnerOperator> &&Src)noexcept(true)
		: fPointer(Src.ExtractToManual()){}

	// move assign with token from cPtrOwner
	
	template<class TSrc>
	typename typename TTypeConditional<cPtrOwner&,
		TIsConvertible<TSrc*,T*>::Value
	>::Type operator =(cPtrOwner<TSrc,TPointerOwnerOperator> &&Src)noexcept(true){
		if(fPointer!=nullptr)
			TPointerOwnerOperator::Release(fPointer);

		fPointer=Src.ExtractToManual();
		return *this;
	}

// cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L
#else
// cnLibrary_CPPFEATURE_RVALUE_REFERENCES < 200610L

	// move construct
	cPtrOwner(cPtrOwner &Src)noexcept(true)
		: fPointer(Src.fPointer)
	{
		Src.fPointer=nullptr;
	}

	// move assignment
	cPtrOwner& operator =(cPtrOwner &Src)noexcept(true){
		if(fPointer!=nullptr)
			TPointerOwnerOperator::Release(fPointer);

		fPointer=Src.fPointer;
		Src.fPoionter=nullptr;
		return *this;
	}

	// move construct with token from cPtrOwner

	template<class TSrc
#ifndef cnLibrary_CPPEXCLUDE_FUNCTION_TEMPLATE_DEFALT_ARGUMENT
		, class=typename TTypeConditional<void,
		TIsConvertible<TSrc*,T*>::Value
		>::Type
#endif
	>
	cPtrOwner(cPtrOwner<TSrc,TPointerOwnerOperator> &Src)noexcept(true)
		: fPointer(Src.ExtractToManual()){}

	// move assign with token from cPtrOwner

	template<class TSrc>
	typename typename TTypeConditional<cPtrOwner&,
		TIsConvertible<TSrc*,T*>::Value
	>::Type operator =(cPtrOwner<TSrc,TPointerOwnerOperator> &Src)noexcept(true){
		if(fPointer!=nullptr)
			TPointerOwnerOperator::Release(fPointer);

		fPointer=Src.ExtractToManual();
		return *this;
	}

#endif	// cnLibrary_CPPFEATURE_RVALUE_REFERENCES < 200610L

protected:
	T *fPointer;

	struct ConstructReferenced
	{
		T* Pointer;
	};
	// construct with referenced
	explicit cPtrOwner(ConstructReferenced Referenced)noexcept(true)
		: fPointer(Referenced.Pointer){}

public:


	template<class TSrcPtr>
	static typename TTypeConditional<cPtrOwner,
		TIsConvertible<TSrcPtr,T*>::Value
	>::Type TakeFromManual(TSrcPtr Pointer)noexcept(true)
	{
		ConstructReferenced Referenced;
		Referenced.Pointer=Pointer;
		return cPtrOwner(Referenced);
	}


	T* ExtractToManual(void)noexcept(true){
		T *SwapPointer=fPointer;
		fPointer=nullptr;
		return SwapPointer;
	}

};
//---------------------------------------------------------------------------
//TPointerReferenceOperator
//{
//	static void Acquire(T* &Pointer)noexcept;
//	static void Release(T* &Pointer)noexcept;
//};
//---------------------------------------------------------------------------
template<class T,class TPointerReferenceOperator>
class cPtrReference
{
public:

	T* Pointer(void)const noexcept(true){	return fPointer;	}
	T*& Pointer(void)noexcept(true){		return fPointer;	}

	operator T* ()const noexcept(true){		return fPointer;	}
	T* operator ->()const noexcept(true){	return fPointer;	}


	cPtrReference()noexcept(true)
		: fPointer(nullptr)
	{}
	~cPtrReference()noexcept(true){
		if(fPointer!=nullptr){
			TPointerReferenceOperator::Release(fPointer);
		}
	}

#ifndef cnLibrary_CPPEXCLUDE_NULLPTR

	// construct with null
	cPtrReference(tNullptr)noexcept(true)
		: fPointer(nullptr)
	{}
	// assign with null
	cPtrReference& operator =(tNullptr)noexcept(true){
		Clear();
		return *this;
	}
#endif

	cPtrReference(T *Pointer)noexcept(true)
		: fPointer(Pointer)
	{
		if(fPointer!=nullptr){
			TPointerReferenceOperator::Acquire(Pointer);
		}
	}

	cPtrReference& operator =(T *Src)noexcept(true){
		T* SwapPointer=fPointer;

		fPointer=Src;
		if(fPointer!=nullptr)
			TPointerReferenceOperator::Acquire(fPointer);

		if(SwapPointer!=nullptr)
			TPointerReferenceOperator::Release(SwapPointer);
		return *this;
	}

	void Clear(void)noexcept(true){
		if(fPointer!=nullptr){
			TPointerReferenceOperator::Release(fPointer);
			fPointer=nullptr;
		}
	}

	
protected:
	struct ConstructReferenced
	{
		T* Pointer;
	};

	// construct with referenced
	explicit cPtrReference(ConstructReferenced Referenced)noexcept(true)
		: fPointer(Referenced.Pointer){}

public:

	template<class TSrcPtr>
	static typename TTypeConditional<cPtrReference,
		TIsConvertible<TSrcPtr,T*>::Value
	>::Type TakeFromManual(TSrcPtr Pointer)noexcept(true)
	{
		ConstructReferenced Referenced={static_cast<T*>(Pointer)};
		return cPtrReference(Referenced);
	}

	T* ExtractToManual(void)noexcept(true){
		T* Ptr=fPointer;
		fPointer=nullptr;
		return Ptr;
	}


	void ManualIncReference(void)const noexcept(true){
		if(fPointer!=nullptr)
			TPointerReferenceOperator::Acquire(fPointer);
	}
	void ManualDecReference(void)const noexcept(true){
		if(fPointer!=nullptr)
			TPointerReferenceOperator::Release(fPointer);
	}


	// copy constructor

	cPtrReference(const cPtrReference &Src)noexcept(true)
		: fPointer(Src.fPointer)
	{
		TPointerReferenceOperator::Acquire(fPointer);
	}

	// assign

	cPtrReference& operator =(const cPtrReference &Src)noexcept(true){
		if(this!=&Src){
			T* SwapPointer=fPointer;

			fPointer=Src.fPointer;
			if(fPointer!=nullptr)
				TPointerReferenceOperator::Acquire(fPointer);

			if(SwapPointer!=nullptr)
				TPointerReferenceOperator::Release(SwapPointer);
		}
		return *this;
	}
#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

	// move constructor

	cPtrReference(cPtrReference &&Src)noexcept(true)
		: fPointer(Src.fPointer)
	{
		Src.fPointer=nullptr;
	}

	// move

	cPtrReference& operator =(cPtrReference &&Src)noexcept(true){
		T *SwapPointer=fPointer;

		fPointer=Src.fPointer;
		Src.fPointer=nullptr;

		if(SwapPointer!=nullptr)
			TPointerReferenceOperator::Release(SwapPointer);
		return *this;
	}

#endif // cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L


	// construct from cPtrReference

	template<class TSrc,class TSrcPointerReferenceOperator
#ifndef cnLibrary_CPPEXCLUDE_FUNCTION_TEMPLATE_DEFALT_ARGUMENT
		, class=typename cnVar::TTypeConditional<void,
			cnVar::TIsConvertible<TSrc*,T*>::Value
		>::Type
#endif
	>
	cPtrReference(const cPtrReference<TSrc,TSrcPointerReferenceOperator> &Src)noexcept(true)
		: fPointer(Src.Pointer())
	{
		if(fPointer!=nullptr)
			TPointerReferenceOperator::Acquire(fPointer);
	}


	// assign from cPtrReference
	
	template<class TSrc,class TSrcPointerReferenceOperator>
	typename cnVar::TTypeConditional<cPtrReference&,
		cnVar::TIsConvertible<TSrc*,T*>::Value
	>::Type operator =(const cPtrReference<TSrc,TSrcPointerReferenceOperator> &Src)noexcept(true){
		T* SwapPointer=fPointer;

		fPointer=Src.Pointer();
		if(fPointer!=nullptr)
			TPointerReferenceOperator::Acquire(fPointer);
		if(SwapPointer!=nullptr)
			TPointerReferenceOperator::Release(SwapPointer);
		return *this;
	}

#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

	// move construct from cPtrReference

	template<class TSrc
#ifndef cnLibrary_CPPEXCLUDE_FUNCTION_TEMPLATE_DEFALT_ARGUMENT
		, class=typename cnVar::TTypeConditional<void,
		cnVar::TIsConvertible<TSrc*,T*>::Value
		>::Type
#endif
	>
	cPtrReference(cPtrReference<TSrc,TPointerReferenceOperator> &&Src)noexcept(true)
		: fPointer(Src.ExtractToManual()){}


	// move assign from cPtrReference

	template<class TSrc>
	typename cnVar::TTypeConditional<cPtrReference&,
		cnVar::TIsConvertible<TSrc*,T*>::Value
	>::Type operator =(cPtrReference<TSrc,TPointerReferenceOperator> &&Src)noexcept(true){
		T* SwapPointer=fPointer;

		fPointer=Src.ExtractToManual();
		if(SwapPointer!=nullptr)
			TPointerReferenceOperator::Release(SwapPointer);
		return *this;
	}

#endif // cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

protected:
	T* fPointer;
};
//---------------------------------------------------------------------------
//TPointerWeakReferenceOperator
//{
//	typedef tToken;
//	static tToken* Register(T *Pointer)noexcept;
//	static void Unregister(tToken *RegToken)noexcept;
//
//	tReference<T>{
//		typedef Type;
//	};
//	static tReference<T>::Type Reference(tRegToken RegToken,T *Pointer)noexcept;
//};
//---------------------------------------------------------------------------
template<class T,class TPointerWeakReferenceOperator>
class cPtrWeakReference
{
public:
	cPtrWeakReference()noexcept(true)
		: fToken(nullptr)
		, fPointer(nullptr)
	{}
	~cPtrWeakReference()noexcept(true){
		if(fToken!=nullptr){
			TPointerWeakReferenceOperator::Unregister(fToken);
		}
	}

#ifndef cnLibrary_CPPEXCLUDE_NULLPTR

	// construct with null
	cPtrWeakReference(tNullptr)noexcept(true)
		: fToken(nullptr)
		, fPointer(nullptr)
	{}
	// assign with null
	cPtrWeakReference& operator =(tNullptr)noexcept(true){
		Clear();
		return *this;
	}
#endif

	cPtrWeakReference(T *Pointer)noexcept(true){
		if(Pointer!=nullptr){
			fToken=TPointerWeakReferenceOperator::Register(Pointer);
			if(fToken!=nullptr){
				fPointer=Pointer;
			}
			else{
				fPointer=nullptr;
			}
		}
		else{
			fToken=nullptr;
			fPointer=nullptr;
		}
	}

	cPtrWeakReference& operator = (T *Pointer)noexcept(true){
		if(fPointer!=Pointer){
			Clear();
			if(Pointer!=nullptr){
				fToken=TPointerWeakReferenceOperator::Register(Pointer);
				if(fToken!=nullptr){
					fPointer=Pointer;
				}
			}
		}
		return *this;
	}

	void Clear(void)noexcept(true){
		if(fToken!=nullptr){
			TPointerWeakReferenceOperator::Unregister(fToken);
			fToken=nullptr;
			fPointer=nullptr;
		}
	}


	typename TPointerWeakReferenceOperator::template tReference<T>::Type Ref(void)noexcept(true){
		if(fToken==nullptr)
			return nullptr;

		typename TPointerWeakReferenceOperator::template tReference<T>::Type RetReference=TPointerWeakReferenceOperator::Reference(fToken,fPointer);
		if(RetReference){
			return RetReference;
		}
		TPointerWeakReferenceOperator::Unregister(fToken);
		fToken=nullptr;
		fPointer=nullptr;
		return nullptr;
	}
	typename TPointerWeakReferenceOperator::template tReference<T>::Type operator + (void)noexcept(true){
		return Ref();
	}

	operator bool ()const noexcept(true){	return fToken!=nullptr;	}


	// copy constructor

	cPtrWeakReference(const cPtrWeakReference &Src)noexcept(true){
		if(Src.fToken!=nullptr){
			typename TPointerWeakReferenceOperator::template tReference<T>::Type SrcReference=Src.Ref();
			if(SrcReference!=nullptr){
				fToken=TPointerWeakReferenceOperator::Register(static_cast<T*>(SrcReference));
				if(fToken!=nullptr){
					fPointer=SrcReference;
				}
				else{
					fPointer=nullptr;
				}
			}
			else{
				fToken=nullptr;
				fPointer=nullptr;
			}
		}
		else{
			fToken=nullptr;
			fPointer=nullptr;
		}
	}

	// assign

	cPtrWeakReference& operator =(const cPtrWeakReference &Src)noexcept(true){
		if(this!=&Src){
			Clear();
			typename TPointerWeakReferenceOperator::template tReference<T>::Type SrcReference=Src.Ref();
			if(SrcReference!=nullptr){
				fToken=TPointerWeakReferenceOperator::Register(static_cast<T*>(SrcReference));
				if(fToken!=nullptr){
					fPointer=SrcReference;
				}
			}
		}
		return *this;
	}
#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

	// move constructor

	cPtrWeakReference(cPtrWeakReference &&Src)noexcept(true)
		: fToken(Src.fToken)
		, fPointer(Src.fPointer)
	{
		Src.fToken=nullptr;
	}

	// move

	cPtrWeakReference& operator =(cPtrWeakReference &&Src)noexcept(true){
		if(fToken!=nullptr){
			TPointerWeakReferenceOperator::Unregister(fToken);
		}

		fToken=Src.fToken;
		fPointer=Src.fPointer;

		Src.fToken=nullptr;
		Src.fPointer=nullptr;
		return *this;
	}

#endif // cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L


	// construct from cPtrReference

	template<class TSrc,class TSrcPointerReferenceOperator
#ifndef cnLibrary_CPPEXCLUDE_FUNCTION_TEMPLATE_DEFALT_ARGUMENT
		, class=typename cnVar::TTypeConditional<void,
		cnVar::TIsConvertible<TSrc*,T*>::Value
		>::Type
#endif
	>
	cPtrWeakReference(const cnVar::cPtrReference<TSrc,TSrcPointerReferenceOperator> &Src)noexcept(true)
	{
		auto Pointer=Src.Pointer();
		if(Pointer!=nullptr){
			fToken=TPointerWeakReferenceOperator::Register(Pointer);
			if(fToken!=nullptr){
				fPointer=Pointer;
			}
			else{
				fPointer=nullptr;
			}
		}
		else{
			fToken=nullptr;
			fPointer=nullptr;
		}
	}


	// assign from cPtrReference

	template<class TSrc,class TSrcPointerReferenceOperator>
	typename cnVar::TTypeConditional<cPtrWeakReference&,
		cnVar::TIsConvertible<TSrc*,T*>::Value
	>::Type operator =(const cnVar::cPtrReference<TSrc,TSrcPointerReferenceOperator> &Src)noexcept(true){
		Clear();
		auto Pointer=Src.Pointer();
		if(Pointer!=nullptr){
			fToken=TPointerWeakReferenceOperator::Register(Pointer);
			if(fToken!=nullptr){
				fPointer=Pointer;
			}
		}
	}

protected:
	typename TPointerWeakReferenceOperator::tToken *fToken;
	T *fPointer;
};
//---------------------------------------------------------------------------
}   // namespace cnVar
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#include <cnTK/TKMacrosCleanup.inc>
//---------------------------------------------------------------------------
#endif	/* cnLibrary_CPPFEATURELEVEL >= 1 */
/*-------------------------------------------------------------------------*/
#endif
