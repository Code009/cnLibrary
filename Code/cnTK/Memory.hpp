/*- cnTK - Memory ---------------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2018-08-14                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnTK_Memory_HPP__
#define	__cnLibrary_cnTK_Memory_HPP__
/*-------------------------------------------------------------------------*/
#include <cnTK/Common.hpp>
#include <cnTK/TypeTraits.hpp>
#include <cnTK/Numerical.hpp>
/*-------------------------------------------------------------------------*/
#if	cnLibrary_CPPFEATURELEVEL >= 1
//---------------------------------------------------------------------------
#include <cnTK/TKMacrosDeclare.inc>
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace TKRuntime{
//---------------------------------------------------------------------------

template<uIntn ElementSize>	
struct TIntegerMemory;
//{
//	static T UnalignRead(const T *Variable)noexcept;
//	static void UnalignWrite(T *Variable,T Value)noexcept;
//	static sfInt8 BytesCompare(T Src1,T Src2)noexcept;
//};

template<uIntn ElementSize>	
struct TMemory;
//{
//	static void Copy(void *Dest,const void *Src,uIntn Length)noexcept;
//	static void CopyOverlapped(void *Dest,const void *Src,uIntn Length)noexcept;
//	static void ZeroFill(void *Dest,uIntn Length)noexcept;
//	static bool Equal(const void *Src1,const void *Src2,uIntn Length)noexcept;
//	static sfInt8 Compare(const void *Src1,const void *Src2,uIntn Length)noexcept;
//};

//---------------------------------------------------------------------------
}	// namespace TKRuntime
//---------------------------------------------------------------------------
namespace CPPRuntime{
//---------------------------------------------------------------------------

template<uIntn ElementSize>	
struct TIntegerMemory
{
	typedef typename cnVar::TIntegerOfSize<ElementSize,false>::Type tUInt;

	static tUInt UnalignRead(const tUInt *Variable)noexcept(true){
		tUInt RetInt;
		for(uIntn i=0;i<ElementSize;i++){
			reinterpret_cast<uInt8*>(&RetInt)[i]=reinterpret_cast<const uInt8*>(Variable)[i];
		}
		return RetInt;
	}
	static void UnalignWrite(tUInt *Variable,tUInt Value)noexcept(true){
		for(uIntn i=0;i<ElementSize;i++){
			reinterpret_cast<uInt8*>(Variable)[i]=reinterpret_cast<const uInt8*>(&Value)[i];
		}
	}
	static sfInt8 BytesCompare(tUInt Src1,tUInt Src2)noexcept(true){
		for(uIntn i=0;i<ElementSize;i++){
			if(!(static_cast<const uInt8*>(Src1)[i]==static_cast<const uInt8*>(Src2)[i])){
				return static_cast<const uInt8*>(Src1)[i]<static_cast<const uInt8*>(Src2)[i]?-1:1;
			}
		}
		return true;
	}
};

template<uIntn ElementSize>
struct TMemory
{
	static cnLib_CONSTVAR uIntn OperateSize=cnVar::TIntegerValue<uIntn>::tValueLSB<ElementSize>::Value;
	static cnLib_CONSTVAR uIntn Scale=ElementSize/OperateSize;
	typedef typename cnVar::TIntegerOfSize<OperateSize,false>::Type tUInt;

	static void Copy(void *Dest,const void *Src,uIntn Length)noexcept(true){
		uIntn CopyLength=Length*Scale;
		for(uIntn i=0;i<CopyLength;i++){
			static_cast<tUInt*>(Dest)[i]=static_cast<const tUInt*>(Src)[i];
		}
	}
	static void CopyOverlapped(void *Dest,const void *Src,uIntn Length)noexcept(true){
		uIntn CopyLength=Length*Scale;
		if(static_cast<uIntn>(static_cast<const tUInt*>(Dest)-static_cast<const tUInt*>(Src))>=CopyLength){
			// not overlapped
			for(uIntn i=0;i<CopyLength;i++){
				static_cast<tUInt*>(Dest)[i]=static_cast<const tUInt*>(Src)[i];
			}
		}
		else{
			// overlapped
			while(CopyLength!=0){
				CopyLength--;
				static_cast<tUInt*>(Dest)[CopyLength]=static_cast<const tUInt*>(Src)[CopyLength];
			}
		}
	}
	static void ZeroFill(void *Dest,uIntn Length)noexcept(true){
		for(uIntn i=0;i<Length;i++){
			static_cast<tUInt*>(Dest)[i]=static_cast<tUInt>(0);
		}
	}
	static bool Equal(const void *Src1,const void *Src2,uIntn Length)noexcept(true){
		for(uIntn i=0;i<Length;i++){
			if(!(static_cast<const tUInt*>(Src1)[i]==static_cast<const tUInt*>(Src2)[i])){
				return false;
			}
		}
		return true;
	}
	static sfInt8 Compare(const void *Src1,const void *Src2,uIntn Length)noexcept(true){
		for(uIntn i=0;i<Length;i++){
			if(!(static_cast<const tUInt*>(Src1)[i]==static_cast<const tUInt*>(Src2)[i])){
				return static_cast<const tUInt*>(Src1)[i]<static_cast<const tUInt*>(Src2)[i]?-1:1;
			}
		}
		return true;
	}
};

//---------------------------------------------------------------------------
}	// namespace CPPRuntime
//---------------------------------------------------------------------------
namespace cnMemory{
//---------------------------------------------------------------------------

// ViewBinarySearch
//	Search in ordered array
// [out]ResultIndex		result of the search,see return value
// Length				array length
// Comparison			Comparison function, compare array element at Index with dest element.
//						prototype: TOrdering (const TElement &Element);
//						return 0 if Element==CompareValue, <0 if Element<CompareValue, >0 if Element>CompareValue
// return:	true if dest elememnt is found, and the ResultIndex is the index of matched element
//			false if the Dest is not found, and the ResultIndex is proper position for dest element
// note: if source array is not ordered, the Result is undefined
template<class TPtr,class TComparison>
inline bool ViewBinarySearch(uIntn &ResultIndex,TPtr Src,uIntn SrcLength,TComparison cnLib_UREF Comparison)noexcept(cnLib_NOEXCEPTEXPR(Comparison(Src[0])))
{
	if(SrcLength==0){
		ResultIndex=0;
		return false;
	}
#if cnLibrary_CPPFEATURE_DECLTYPE >= 200707L
	auto CompareResult=Comparison(Src[0]);
#else	// cnLibrary_CPPFEATURE_DECLTYPE < 200707L
	sfInt8 CompareResult=Comparison(Src[0]);
#endif	// cnLibrary_CPPFEATURE_DECLTYPE
	if(CompareResult>0){	// Src[0]>CompareValue
		ResultIndex=0;
        return false;
	}
	if(CompareResult==0){	// Src[0]==CompareValue
		ResultIndex=0;
        return true;
	}
    // *Src<Dest
	CompareResult=Comparison(Src[SrcLength-1]);
	if(CompareResult<0){	// Src[SrcLength-1]<CompareValue
		ResultIndex=SrcLength;
        return false;
	}
	if(CompareResult==0){	// Src[SrcLength-1]==CompareValue
		ResultIndex=SrcLength-1;
        return true;
	}
    // search in range
    uIntn LeftBound=0;
    uIntn RightBound=SrcLength-1;
    while(LeftBound<RightBound){
		uIntn cPos=(LeftBound+RightBound)/2;
		CompareResult=Comparison(Src[cPos]);
		if(CompareResult==0){	// Src[cPos]==CompareValue
			// found
			ResultIndex=cPos;
            return true;
		}
        if(CompareResult<0)		// Src[cPos]<CompareValue
            LeftBound=cPos+1;	// search right
        else
            RightBound=cPos-1;	// search left
    }
	CompareResult=Comparison(Src[LeftBound]);
	if(CompareResult==0){		// Array[LeftBound]==CompareValue
		// found
		ResultIndex=LeftBound;
        return true;
	}

	// not found
    if(CompareResult<0){		// Array[LeftBound]<CompareValue
        ResultIndex=LeftBound+1;		// position to the right
	}
    else{
        ResultIndex=LeftBound;			// position to the left
	}

	return false;
}

//---------------------------------------------------------------------------
}	// namespace cnMemory
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
namespace cnLib_THelper{

//---------------------------------------------------------------------------
namespace Memory_TH{
//---------------------------------------------------------------------------
struct AlignmentTest_Byte
{
	char Val;
};

template<class T,bool IsClass>
struct AlignmentTest : AlignmentTest_Byte, public T
{
};
template<class T>
struct AlignmentTest<T,false> : AlignmentTest_Byte
{
	T Object;
};

template<class T>
struct AlignmentOf
	: cnVar::TConstantValueUIntn<sizeof(AlignmentTest<T,cnVar::TIsClass<T>::Value>)-sizeof(T)>
{
};

}	// namespace Memory_TH
//---------------------------------------------------------------------------
}	// namespace cnLib_THelper
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnMemory{


template<uIntn Size>
struct TAlignmentIntType;

template<>	struct TAlignmentIntType<1>{	typedef uInt8 Type;	};
template<>	struct TAlignmentIntType<2>{	typedef uInt16 Type;	};
template<>	struct TAlignmentIntType<4>{	typedef uInt32 Type;	};
template<>	struct TAlignmentIntType<8>{	typedef uInt64 Type;	};
template<>	struct TAlignmentIntType<16>{	typedef uInt128 Type;	};

// TAlignmentOf
//	alignment of T
template<class T>
struct TAlignmentOf
	: cnVar::TConstantValueUIntn<cnLib_THelper::Memory_TH::AlignmentOf<T>::Value==0?1:cnLib_THelper::Memory_TH::AlignmentOf<T>::Value>
{
};

#if cnLibrary_CPPFEATURE_VARIABLE_TEMPLATES >= 201304L

template<class T>
static cnLib_CONSTVAR uIntn AlignmentOf=TAlignmentOf<T>::Value;

#endif	// cnLibrary_CPPFEATURE_VARIABLE_TEMPLATES >= 201304L

#if cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L

template<class...VT>	struct TSelectAlignmentType;

template<class T,class...VT>
struct TSelectAlignmentType<T,VT...>
	: cnVar::TSelect<
		(TAlignmentOf<T>::Value>TAlignmentOf<typename TSelectAlignmentType<VT...>::Type>::Value)
		,typename TSelectAlignmentType<VT...>::Type
		,T
	>{};
template<>
struct TSelectAlignmentType<>
	: cnVar::TTypeDef<uInt8>{};

#endif	// cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L

template<class T>
bool PointerIsAligned(uIntn Pointer)noexcept(true)
{
	return Pointer%TAlignmentOf<T>::Value==0;
}
template<class T>
bool PointerIsAligned(const void *Pointer)noexcept(true)
{
	return PointerIsAligned<T>(reinterpret_cast<uIntn>(Pointer));
}
//---------------------------------------------------------------------------
template<uIntn Size,class TAlignment>
struct cPlainAlignedData
{
#ifndef cnLibrary_CPPEXCLUDE_ALIGNMENT
	alignas(TAlignment)	uInt8 _space_[Size];
#else	// cnLibrary_CPPEXCLUDE_ALIGNMENT
	union{
		uInt8 _space_[Size];
		typedef typename TAlignmentIntType<(TAlignmentOf<TAlignment>::Value>16?16:TAlignmentOf<TAlignment>::Value)>::Type _alignment_;
	};
#endif // cnLibrary_CPPEXCLUDE_ALIGNMENT
};

template<class TAlignment>
struct cPlainAlignedData<0,TAlignment>;

//---------------------------------------------------------------------------
}	// namespace cnMemory
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnVar{
//---------------------------------------------------------------------------

// ManualConstruct
//	call constructor on uninitialized object space
// Object		reference to object
template<class T>
inline void ManualConstruct(T &Object)
	noexcept(cnLib_NOEXCEPTEXPR(T()))
{
	::new( &reinterpret_cast<cNoInitialization&>(const_cast<typename TRemoveCV<T>::Type&>(Object)) )
		T;
}
template<class T>
inline void ManualConstructZero(T &Object)
	noexcept(cnLib_NOEXCEPTEXPR(T()))
{
	::new( &reinterpret_cast<cNoInitialization&>(const_cast<typename TRemoveCV<T>::Type&>(Object)) )
		T();
}

#if cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L

// ManualConstruct
//	call constructor on uninitialized object space
// Object		reference to object
// Args			arguments for constructor
template<class T,class TArg,class...TArgs>
inline void ManualConstruct(T &Object,TArg cnLib_UREF Arg,TArgs cnLib_UREF...Args)
	noexcept(cnLib_NOEXCEPTEXPR(T(cnLib_UREFCAST(TArg)(Arg),cnLib_UREFCAST(TArgs)(Args)...)))
{
	::new( &reinterpret_cast<cNoInitialization&>(const_cast<typename TRemoveCV<T>::Type&>(Object)) )
		T(cnLib_UREFCAST(TArg)(Arg),cnLib_UREFCAST(TArgs)(Args)...);
}

// cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L
#else
// cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES < 200704L

template<class T, class T0>
inline void ManualConstruct(T &Object, T0 cnLib_UREF P0)
	noexcept(cnLib_NOEXCEPTEXPR(T(
		cnLib_UREFCAST(T0)(P0)
	)))
{
	::new( &reinterpret_cast<cNoInitialization&>(const_cast<typename TRemoveCV<T>::Type&>(Object)) )
		T(cnLib_UREFCAST(T0)(P0));
}
template<class T, class T0, class T1>
inline void ManualConstruct(T &Object, T0 cnLib_UREF P0, T1 cnLib_UREF P1)
	noexcept(cnLib_NOEXCEPTEXPR(T(
		cnLib_UREFCAST(T0)(P0),
		cnLib_UREFCAST(T1)(P1)
	)))
{
	::new( &reinterpret_cast<cNoInitialization&>(const_cast<typename TRemoveCV<T>::Type&>(Object)) )
		T(
			cnLib_UREFCAST(T0)(P0),
			cnLib_UREFCAST(T1)(P1)
		);
}
template<class T, class T0, class T1, class T2>
inline void ManualConstruct(T &Object, T0 cnLib_UREF P0, T1 cnLib_UREF P1, T2 cnLib_UREF P2)
	noexcept(cnLib_NOEXCEPTEXPR(T(
		cnLib_UREFCAST(T0)(P0),
		cnLib_UREFCAST(T1)(P1),
		cnLib_UREFCAST(T1)(P2)
	)))
{
	::new( &reinterpret_cast<cNoInitialization&>(const_cast<typename TRemoveCV<T>::Type&>(Object)) )
		T(
			cnLib_UREFCAST(T0)(P0),
			cnLib_UREFCAST(T1)(P1),
			cnLib_UREFCAST(T1)(P2)
		);
}
template<class T, class T0, class T1, class T2, class T3>
inline void ManualConstruct(T &Object, T0 cnLib_UREF P0, T1 cnLib_UREF P1, T2 cnLib_UREF  P2, T3 cnLib_UREF P3)
	noexcept(cnLib_NOEXCEPTEXPR(T(
		cnLib_UREFCAST(T0)(P0),
		cnLib_UREFCAST(T1)(P1),
		cnLib_UREFCAST(T1)(P2),
		cnLib_UREFCAST(T1)(P3)
	)))
{
	::new( &reinterpret_cast<cNoInitialization&>(const_cast<typename TRemoveCV<T>::Type&>(Object)) )
		T(
			cnLib_UREFCAST(T0)(P0),
			cnLib_UREFCAST(T1)(P1),
			cnLib_UREFCAST(T1)(P2),
			cnLib_UREFCAST(T1)(P3)
		);
}

#endif	// cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES < 200704L

// ManualDestruct
//	call destructor on object space
// Object		reference to object
template<class T>
inline void ManualDestruct(T &Object)noexcept(cnLib_NOEXCEPTEXPR(Object.~T()))
{
	UnusedParameter(Object);	// prevent unused parameter warning on class do not have destructor
	Object.~T();
}
//---------------------------------------------------------------------------

template<class TObject>
class cStaticVariable : protected cnMemory::cPlainAlignedData<sizeof(TObject),TObject>
{
public:

	operator TObject&()noexcept(true){				return *reinterpret_cast<TObject*>(this);	}
	operator const TObject&()const noexcept(true){	return *reinterpret_cast<TObject*>(this);	}

	TObject* operator &()noexcept(true){				return reinterpret_cast<TObject*>(this);	}
	const TObject* operator &()const noexcept(true){	return reinterpret_cast<const TObject*>(this);	}

	TObject* operator ->()noexcept(true){				return reinterpret_cast<TObject*>(this);	}
	const TObject* operator ->()const noexcept(true){	return reinterpret_cast<const TObject*>(this);	}

#if cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L
	template<class...TArgs>
	void Construct(TArgs cnLib_UREF...Args)noexcept(cnLib_NOEXCEPTEXPR(TObject(cnLib_UREFCAST(TArgs)(Args)...)))
	{
		ManualConstruct(*reinterpret_cast<TObject*>(this),cnLib_UREFCAST(TArgs)(Args)...);
	}
// cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L
#else
// cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES < 200704L
	void Construct(void)noexcept(cnLib_NOEXCEPTEXPR(TObject()))
	{	ManualConstruct(*reinterpret_cast<TObject*>(this));	}
	template<class TArg0>
	void Construct(TArg0 cnLib_UREF Arg0)noexcept(cnLib_NOEXCEPTEXPR(TObject(cnLib_UREFCAST(TArg0)(Arg0))))
	{
		ManualConstruct(*reinterpret_cast<TObject*>(this)
			,cnLib_UREFCAST(TArg0)(Arg0)
		);
	}
	template<class TArg0,class TArg1>
	void Construct(TArg0 cnLib_UREF Arg0,TArg1 cnLib_UREF Arg1)
		noexcept(cnLib_NOEXCEPTEXPR(TObject(
			cnLib_UREFCAST(TArg0)(Arg0)
			,cnLib_UREFCAST(TArg1)(Arg1)
		)))
	{
		ManualConstruct(*reinterpret_cast<TObject*>(this)
			,cnLib_UREFCAST(TArg0)(Arg0)
			,cnLib_UREFCAST(TArg1)(Arg1)
		);
	}
	template<class TArg0,class TArg1,class TArg2>
	void Construct(TArg0 cnLib_UREF Arg0,TArg1 cnLib_UREF Arg1,TArg2 cnLib_UREF Arg2)
		noexcept(cnLib_NOEXCEPTEXPR(TObject(
			cnLib_UREFCAST(TArg0)(Arg0)
			,cnLib_UREFCAST(TArg1)(Arg1)
			,cnLib_UREFCAST(TArg2)(Arg2)
		)))
	{
		ManualConstruct(*reinterpret_cast<TObject*>(this)
			,cnLib_UREFCAST(TArg0)(Arg0)
			,cnLib_UREFCAST(TArg1)(Arg1)
			,cnLib_UREFCAST(TArg2)(Arg2)
		);
	}
	template<class TArg0,class TArg1,class TArg2,class TArg3>
	void Construct(TArg0 cnLib_UREF Arg0,TArg1 cnLib_UREF Arg1,TArg2 cnLib_UREF Arg2,TArg3 cnLib_UREF Arg3)
		noexcept(cnLib_NOEXCEPTEXPR(TObject(
			cnLib_UREFCAST(TArg0)(Arg0)
			,cnLib_UREFCAST(TArg1)(Arg1)
			,cnLib_UREFCAST(TArg2)(Arg2)
			,cnLib_UREFCAST(TArg3)(Arg3)
		)))
	{
		ManualConstruct(*reinterpret_cast<TObject*>(this)
			,cnLib_UREFCAST(TArg0)(Arg0)
			,cnLib_UREFCAST(TArg1)(Arg1)
			,cnLib_UREFCAST(TArg2)(Arg2)
			,cnLib_UREFCAST(TArg3)(Arg3)
		);
	}

#endif	// cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES < 200704L
	void Destruct(void)noexcept(cnLib_NOEXCEPTEXPR(cnVar::DeclVal<TObject&>().~TObject()))
	{
		ManualDestruct(*reinterpret_cast<TObject*>(this));
	}
};

//---------------------------------------------------------------------------
#if cnLibrary_CPPFEATURE_CONSTINIT >= 201907L

static cnLib_CONSTVAR bool StaticInitializedSingltonDefinitionGenuine=true;

template<class T>
struct cStaticInitializedSinglton
{
	typedef T tInstance;

	static T gInstance;

	static void Check(void)noexcept(true){}
};


template<class T>
constinit T cStaticInitializedSinglton<T>::gInstance;

template<class T,class TRet>
TRet StaticInitializedSinglton(void)noexcept(true){
	return reinterpret_cast<TRet>(&cStaticInitializedSinglton<T>::gInstance);
}

template<class T>
T* StaticInitializedSinglton(void)noexcept(true){
	return &cStaticInitializedSinglton<T>::gInstance;
}

// cnLibrary_CPPFEATURE_CONSTINIT >= 201907L
#else
// cnLibrary_CPPFEATURE_CONSTINIT < 201907L

static cnLib_CONSTVAR bool StaticInitializedSingltonDefinitionGenuine=false;

template<class T>
struct cStaticInitializedSinglton
{
	struct tInstance : public cnMemory::cPlainAlignedData<sizeof(T),T>
	{
		bool Initialized;
	};
	static tInstance gInstance;

	static void Check(void){
		if(!gInstance.Initialized){
			ManualConstruct(reinterpret_cast<T&>(gInstance));
			gInstance.Initialized=true;
		}
	}
};
template<class T>
typename cStaticInitializedSinglton<T>::tInstance cStaticInitializedSinglton<T>::gInstance;

#ifndef cnLibrary_CPPEXCLUDE_FUNCTION_TEMPLATE_DEFALT_ARGUMENT
template<class T,class TRet=T*>
#else	// cnLibrary_CPPEXCLUDE_FUNCTION_TEMPLATE_DEFALT_ARGUMENT
template<class T,class TRet>
#endif
TRet StaticInitializedSinglton(void){
	cStaticInitializedSinglton<T>::Check();
	return reinterpret_cast<TRet>(&cStaticInitializedSinglton<T>::gInstance);
}

#ifdef cnLibrary_CPPEXCLUDE_FUNCTION_TEMPLATE_DEFALT_ARGUMENT

template<class T>
T* StaticInitializedSinglton(void){
	return StaticInitializedSinglton<T,T*>();
}
#endif	// cnLibrary_CPPEXCLUDE_FUNCTION_TEMPLATE_DEFALT_ARGUMENT

#endif	// cnLibrary_CPPFEATURE_CONSTINIT < 201907L


#if cnLibrary_CPPFEATURE_CONSTINIT >= 201907L

static cnLib_CONSTVAR bool StaticInitializedConstSingltonDefinitionGenuine=true;

template<class T>
struct cStaticInitializedConstSinglton
{
	typedef const T tInstance;
	static const T gInstance;
	static void Check(void)noexcept(true){}
};
template<class T>
constinit const T cStaticInitializedConstSinglton<T>::gInstance;

template<class T,class TRet>
TRet StaticInitializedConstSinglton(void)noexcept(true){
	return reinterpret_cast<TRet>(&cStaticInitializedConstSinglton<T>::gInstance);
}

template<class T>
const T* StaticInitializedConstSinglton(void)noexcept(true){
	return &cStaticInitializedConstSinglton<T>::gInstance;
}

// cnLibrary_CPPFEATURE_CONSTINIT >= 201907L
#else
// cnLibrary_CPPFEATURE_CONSTINIT < 201907L

#if cnLibrary_CPPFEATURE_CONSTEXPR >= 200704L && !defined(cnLibrary_CPPEXCLUDE_CONSTEXPR_STATIC_INITIALIZATION)

static cnLib_CONSTVAR bool StaticInitializedConstSingltonDefinitionGenuine=true;

template<class T>
struct cStaticInitializedConstSinglton
{
	typedef const T tInstance;

	static constexpr T gInstance={};
	static void Check(void)noexcept(true){}
};

template<class T>
constexpr T cStaticInitializedConstSinglton<T>::gInstance;

template<class T,class TRet>
TRet StaticInitializedConstSinglton(void)noexcept(true){
	return reinterpret_cast<TRet>(&cStaticInitializedConstSinglton<T>::gInstance);
}

template<class T>
const T* StaticInitializedConstSinglton(void)noexcept(true){
	return &cStaticInitializedConstSinglton<T>::gInstance;
}

#else	// cnLibrary_CPPFEATURE_CONSTEXPR < 200704L || defined(cnLibrary_CPPEXCLUDE_CONSTEXPR_STATIC_INITIALIZATION)

static cnLib_CONSTVAR bool StaticInitializedConstSingltonDefinitionGenuine=false;

template<class T>
struct cStaticInitializedConstSinglton : cStaticInitializedSinglton<const T>
{
};

template<class T,class TRet>
TRet StaticInitializedConstSinglton(void)noexcept(true){
	return StaticInitializedSinglton<const T,TRet>();
}

template<class T>
const T* StaticInitializedConstSinglton(void)noexcept(true){
	return StaticInitializedSinglton<const T>();
}

#endif	// cnLibrary_CPPFEATURE_CONSTEXPR , defined(cnLibrary_CPPEXCLUDE_CONSTEXPR_STATIC_INITIALIZATION)

#endif	// cnLibrary_CPPFEATURE_CONSTINIT < 201907L


//---------------------------------------------------------------------------
template<uIntn Size,class TAlignment>
class cPolymorphicBlock : protected cnMemory::cPlainAlignedData<Size,TAlignment>
{
public:
	void* operator &()noexcept(true){				return this;	}
	const void* operator &()const noexcept(true){	return this;	}

#if cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L

	template<class TDest,class...TArgs>
	void ConstructAs(TArgs cnLib_UREF...Args)noexcept(cnLib_NOEXCEPTEXPR(TDest(cnLib_UREFCAST(TArgs)(Args)...)))
	{
		cnLib_STATIC_ASSERT(sizeof(TDest)<=Size,"not enough size");
		ManualConstruct(*reinterpret_cast<TDest*>(this),cnLib_UREFCAST(TArgs)(Args)...);
	}

	// cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L
#else
	// cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES < 200704L
	template<class TDest>
	void Construct(void)noexcept(cnLib_NOEXCEPTEXPR(TDest()))
	{	ManualConstruct(*reinterpret_cast<TDest*>(this));	}
	template<class TDest,class TArg0>
	void Construct(TArg0 cnLib_UREF Arg0)
		noexcept(cnLib_NOEXCEPTEXPR(TDest(cnLib_UREFCAST(TArg0)(Arg0))))
	{
		ManualConstruct(*reinterpret_cast<TDest*>(this)
			,cnLib_UREFCAST(TArg0)(Arg0)
		);
	}
	template<class TDest,class TArg0,class TArg1>
	void Construct(TArg0 cnLib_UREF Arg0,TArg1 cnLib_UREF Arg1)
		noexcept(cnLib_NOEXCEPTEXPR(TDest(
			cnLib_UREFCAST(TArg0)(Arg0)
			,cnLib_UREFCAST(TArg1)(Arg1)
		)))
	{
		ManualConstruct(*reinterpret_cast<TDest*>(this)
			,cnLib_UREFCAST(TArg0)(Arg0)
			,cnLib_UREFCAST(TArg1)(Arg1)
		);
	}
	template<class TDest,class TArg0,class TArg1,class TArg2>
	void Construct(TArg0 cnLib_UREF Arg0,TArg1 cnLib_UREF Arg1,TArg2 cnLib_UREF Arg2)
		noexcept(cnLib_NOEXCEPTEXPR(TDest(
			cnLib_UREFCAST(TArg0)(Arg0)
			,cnLib_UREFCAST(TArg1)(Arg1)
			,cnLib_UREFCAST(TArg2)(Arg2)
		)))
	{
		ManualConstruct(*reinterpret_cast<TDest*>(this)
			,cnLib_UREFCAST(TArg0)(Arg0)
			,cnLib_UREFCAST(TArg1)(Arg1)
			,cnLib_UREFCAST(TArg2)(Arg2)
		);
	}
	template<class TDest,class TArg0,class TArg1,class TArg2,class TArg3>
	void Construct(TArg0 cnLib_UREF Arg0,TArg1 cnLib_UREF Arg1,TArg2 cnLib_UREF Arg2,TArg3 cnLib_UREF Arg3)
		noexcept(cnLib_NOEXCEPTEXPR(TDest(
			cnLib_UREFCAST(TArg0)(Arg0)
			,cnLib_UREFCAST(TArg1)(Arg1)
			,cnLib_UREFCAST(TArg2)(Arg2)
			,cnLib_UREFCAST(TArg3)(Arg3)
		)))
	{
		ManualConstruct(*reinterpret_cast<TDest*>(this)
			,cnLib_UREFCAST(TArg0)(Arg0)
			,cnLib_UREFCAST(TArg1)(Arg1)
			,cnLib_UREFCAST(TArg2)(Arg2)
			,cnLib_UREFCAST(TArg3)(Arg3)
		);
	}

#endif	// cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES < 200704L

	template<class TDest>
	void DestructAs(void)noexcept(cnLib_NOEXCEPTEXPR(cnVar::DeclVal<TDest&>().~TDest()))
	{
		cnLib_STATIC_ASSERT(sizeof(TDest)<=Size,"not enough size");
		ManualDestruct(*reinterpret_cast<TDest*>(this));
	}
};
//---------------------------------------------------------------------------

template<class TInterface,uIntn Size=sizeof(TInterface),class TAlignment=TInterface>
class cPolymorphicInterface : public cPolymorphicBlock<Size,TAlignment>
{
public:
	cnLib_STATIC_ASSERT(Size>=sizeof(TInterface),"error storage size");

	operator TInterface&()noexcept(true){					return *reinterpret_cast<TInterface*>		(this);	}
	operator const TInterface&()const noexcept(true){		return *reinterpret_cast<const TInterface*>	(this);	}
	TInterface* operator &()noexcept(true){					return reinterpret_cast<TInterface*>		(this);	}
	const TInterface* operator &()const noexcept(true){		return reinterpret_cast<const TInterface*>	(this);	}
	TInterface* operator ->()noexcept(true){				return reinterpret_cast<TInterface*>		(this);	}
	const TInterface* operator ->()const noexcept(true){	return reinterpret_cast<const TInterface*>	(this);	}
};

//---------------------------------------------------------------------------
template<uIntn Size,class TAlignment>
class cPolymorphicInterface<void,Size,TAlignment> : public cPolymorphicBlock<Size,TAlignment>
{
};

#if cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L

template<class TInterface,class...TImplementations>
using cPolymorphicObject = cPolymorphicInterface<TInterface,
	cnMath::TMax<tSize,sizeof(TImplementations)...>::Value,
	typename cnMemory::TSelectAlignmentType<TImplementations...>::Type
>;
#endif	// cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L


template<class TInterface,uIntn Size=sizeof(TInterface),class TAlignment=TInterface>
class cPolymorphicClass : public cnMemory::cPlainAlignedData<Size,TAlignment>
{
public:
	cnLib_STATIC_ASSERT(sizeof(TInterface)<=Size,"not enough size");

	TInterface* operator ->()noexcept(true){				return reinterpret_cast<TInterface*>		(this);	}
	const TInterface* operator ->()const noexcept(true){	return reinterpret_cast<const TInterface*>	(this);	}

	cPolymorphicClass(){	ManualConstruct(*reinterpret_cast<TInterface*>(this));	}
	~cPolymorphicClass(){	reinterpret_cast<TInterface*>(this)->~TInterface();	}

	template<class TDest,class...TArgs>
	void Replace(TArgs cnLib_UREF...Args)noexcept(cnLib_NOEXCEPTEXPR(TDest(cnLib_UREFCAST(TArgs)(Args)...)))
	{
		reinterpret_cast<TInterface*>(this)->~TInterface();
		cnLib_STATIC_ASSERT(sizeof(TDest)<=Size,"not enough size");
		ManualConstruct(*reinterpret_cast<TDest*>(this),cnLib_UREFCAST(TArgs)(Args)...);
	}
};
//---------------------------------------------------------------------------
}	// namespace cnVar
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#include <cnTK/TKMacrosCleanup.inc>
//---------------------------------------------------------------------------
#endif	/* cnLibrary_CPPFEATURELEVEL >= 1 */
/*-------------------------------------------------------------------------*/
#endif
