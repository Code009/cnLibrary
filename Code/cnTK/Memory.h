/*- cnTK - Memory ---------------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2018-08-14                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnTK_Memory_H__
#define	__cnLibrary_cnTK_Memory_H__
/*-------------------------------------------------------------------------*/
#include <cnTK/Common.h>
#include <cnTK/TypeTraits.h>
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
struct tMemory;
//{
//	static void Copy(void *Dest,const void *Src,uIntn Length)noexcept(true);
//	static void CopyOverlapped(void *Dest,const void *Src,uIntn Length)noexcept(true);
//	static void ZeroFill(void *Dest,uIntn Length)noexcept(true);
//	static bool Equal(const void *Src1,const void *Src2,uIntn Length)noexcept(true);
//};

//---------------------------------------------------------------------------
}	// namespace TKRuntime
//---------------------------------------------------------------------------
namespace cnMemory{
//---------------------------------------------------------------------------

// ViewCopy
//	Copy memory buffer
// [out]Dest	dest buffer
// [in]Src		source buffer
// Length		length of copy operation
template<class TDestPtr,class TSrcPtr>
inline void ViewCopy(TDestPtr Dest,TSrcPtr Src,uIntn Length)noexcept(true)
{
	while(Length!=0){
		*Dest=*Src;
		++Dest;
		++Src;
		Length--;
	}
}

// ViewCopyOverlapped
//	Copy memory buffer
// [out]Dest	dest buffer
// [in]Src		source buffer
// Length		length of copy operation
template<class TPtr>
inline void ViewCopyOverlapped(TPtr Dest,typename cnVar::TTypeDef<TPtr>::Type Src,uIntn Length)noexcept(true)
{
	if(static_cast<uIntn>(Dest-Src)>=Length){
		return ViewCopy(Dest,Src,Length);
	}
	// overlapped
	Src+=Length;
	Dest+=Length;
	while(Length!=0){
		--Dest;
		--Src;
		Length--;
		*Dest=*Src;
	}
}


// ViewFill
//	fill memory buffer
// [out]Dest	dest buffer
// [in]Src		source data
// Length		length of fill operation
template<class TDestPtr,class TSrc>
inline void ViewFill(TDestPtr Dest,uIntn Length,TSrc Src)noexcept(true)
{
	while(Length!=0){
		*Dest=Src;
		++Dest;
		Length--;
	}
}

// ViewLinearSearch
//	Search element in Src
// [in]Src			pointer to array to search
// Length			number of elements to look into
// Comparison		Comparison function, check array element if it matches
//					prototype: bool (const T &Element);
//					return true if matches, false if unmatch
// Return:	the index of the first matched data, or -1 if the Dest not found
template<class TPtr,class TComparison>
inline uIntn ViewLinearSearch(TPtr Src,uIntn SrcLength,TComparison cnLib_UREF Comparison)noexcept(cnLib_NOEXCEPTEXPR(Comparison(*Src)))
{
	for(uIntn i=0;i<SrcLength;i++){
		if(Comparison(*Src++)){
			return i;
		}
	}
	return IndexNotFound;
}
// ViewLinearSearch
//	Search element in Src
// [in]Src			pointer to array to search
// Length			number of elements to look into
// Comparison		Comparison function, check array element if it matches
//					prototype: bool (const T &Element);
//					return true if matches, false if unmatch
// Return:	the index of the first matched data, or -1 if the Dest not found
template<class TPtr,class TComparison>
inline uIntn ViewLinearReverseSearch(TPtr Src,uIntn SrcLength,TComparison cnLib_UREF Comparison)noexcept(cnLib_NOEXCEPTEXPR(Comparison(*Src)))
{
	Src+=SrcLength;
	while(SrcLength>0){
		SrcLength--;
		Src--;
		if(Comparison(*Src)){
			return SrcLength;
		}
	}
	return IndexNotFound;
}

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
inline bool ViewBinarySearch(uIntn &ResultIndex,TPtr Src,uIntn SrcLength,TComparison cnLib_UREF Comparison)noexcept(cnLib_NOEXCEPTEXPR(Comparison(*Src)))
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
    if(CompareResult<0)		// Array[LeftBound]<CompareValue
        ResultIndex=LeftBound+1;		// position to the right
    else
        ResultIndex=LeftBound;			// position to the left
	return false;
}

template<class TPtr1,class TPtr2>
inline bool ViewIsEqual(TPtr1 Array1,TPtr2 Array2,uIntn Length)noexcept(cnLib_NOEXCEPTEXPR(*Array1!=*Array2))
{
	for(uIntn i=0;i<Length;i++){
		if(Array1[i]!=Array2[i]){
			return false;
		}
	}
	return true;
}

// ViewMatchLength
//	Find match length between 2 array
//	[in]Array1
//	[in]Array2
//	[in]Length	length to compare
//	return:	the length of matched elements
template<class TPtr1,class TPtr2>
inline uIntn ViewMatchLength(TPtr1 Array1,TPtr2 Array2,uIntn Length)noexcept(cnLib_NOEXCEPTEXPR(*Array1!=*Array2))
{
	for(uIntn i=0;i<Length;i++){
		if(Array1[i]!=Array2[i]){
			return i;
		}
	}
	return Length;
}

// ViewCompare
//	The encoding relation of string1 to string2
//	[in]Src1	string to subtract
//	[in]Src2	string to be subtracted
//	[in]Length	length of first characters in string to compare
//	return:	the differ of first differnet char, 0 if two strings matched
template<class TPtr1,class TPtr2>
inline sfInt8 ViewCompare(TPtr1 Array1,TPtr2 Array2,uIntn Length)noexcept(cnLib_NOEXCEPTEXPR(*Array1<*Array2) && cnLib_NOEXCEPTEXPR(*Array1>*Array2))
{
	for(uIntn Index=0;Index<Length;Index++){
		if(Array1[Index]<Array2[Index])
			return -1;
		else if(Array1[Index]>Array2[Index])
			return 1;
	}
	return 0;
}

//---------------------------------------------------------------------------
}	// namespace cnMemory
//---------------------------------------------------------------------------
namespace cnString{
//---------------------------------------------------------------------------

// ViewCopy
//	Copy string
// [out]Dest	dest string buffer, buffer must has at least one character space for termination
// MaxLength	max length of copy operation, not including null-termination.
// [in]Src		source string
// return	copied string length
template<class TDestPtr,class TSrcPtr>
inline uIntn ViewCopy(TDestPtr Dest,uIntn DestLength,TSrcPtr Src)noexcept(true)
{
	if(DestLength==0)
		return 0;

	uIntn Index;
	for(Index=0;Index<DestLength;Index++){
		typename cnVar::TRemovePointer<TSrcPtr>::Type &c=*Src;
		if(c==0){
			break;
		}
		*Dest=c;
		++Dest;
		++Src;
	}
	*Dest=0;
	return Index;
}


// ViewLinearSearch
//	Search element in Src
// [in]Src			pointer to string to search
// Comparison		Comparison function, check array element if it matches
//					prototype: bool (const T &Element);
//					return true if matches, false if unmatch
// Return:	the index of the first matched data, or -1 if the Dest not found
template<class TPtr,class TComparison>
inline uIntn ViewLinearSearch(TPtr Src,TComparison cnLib_UREF Comparison)noexcept(true)
{
	uIntn Index=0;
	while(*Src!=0){
		if(Comparison(*Src))
			return Index;
		++Src;
		++Index;
	}
	return IndexNotFound;
}

// ViewLinearSearch
//	Search element in Src
// [in]Src			pointer to array to search
// Length			number of elements to look into
// Comparison		Comparison function, check array element if it matches
//					prototype: bool (const T &Element);
//					return true if matches, false if unmatch
// Return:	the index of the first matched data, or -1 if the Dest not found
template<class TPtr,class TComparison>
inline uIntn ViewLinearSearch(TPtr Src,uIntn SrcLength,TComparison cnLib_UREF Comparison)noexcept(cnLib_NOEXCEPTEXPR(Comparison(*Src)))
{
	for(uIntn Index=0;Index<SrcLength;Index++){
		if(Comparison(*Src))
			return Index;

		if(*Src==0)
			break;
		++Src;
	}
	return IndexNotFound;
}

// ViewDiffIndex
//	Find index of different character between 2 strings
//	[in]Src1
//	[in]Src2
//	return:	the index of first differnet char, or IndexNotFound if two strings matched
template<class TPtr1,class TPtr2>
inline uIntn ViewDiffIndex(TPtr1 Str1,TPtr2 Str2)noexcept(cnLib_NOEXCEPTEXPR(*Str1==*Str2))
{
	uIntn Index=0;
	while(*Str1==*Str2){
		if(*Str1==0){
			// reached the end
			return IndexNotFound;
		}
		Index++;
		++Str1;
		++Str2;
	}
	return Index;
}

// ViewDiffIndex
//	Find index of different character between 2 strings
//	[in]Str1
//	[in]Str1Length	length of first characters in string to compare
//	[in]Str2
//	return:	the index of first differnet char, or IndexNotFound if first length of two strings matched
template<class TPtr1,class TPtr2>
inline uIntn ViewDiffIndex(TPtr1 Str1,uIntn Str1Length,TPtr2 Str2)noexcept(cnLib_NOEXCEPTEXPR(*Str1!=*Str2))
{
	for(uIntn i=0;i<Str1Length;i++){
		if(*Str1!=*Str2){
			return i;
		}
		if(*Str1==0){
			// reached the end
			break;
		}
		++Str1;
		++Str2;
	}
	return IndexNotFound;
}


// ViewEqual
//	test if string1 equal to string2
//	[in]Str1	string to subtract
//	[in]Str2	string to be subtracted
//	return:	0 if matched, 1 if Str1>Str2 , -1 if Str1<Str2
template<class TPtr1,class TPtr2>
inline bool ViewEqual(TPtr1 Str1,uIntn Str1Length,TPtr2 Str2)noexcept(cnLib_NOEXCEPTEXPR(*Str1==*Str2))
{
	for(uIntn i=0;i<Str1Length;i++){
		if(!(*Str1==*Str2)){
			return false;
		}
		++Str1;
		++Str2;
	}
	cnLib_ASSERT(*Str1==0);
	return *Str2==0;
}



// ViewCompare
//	The encoding relation of string1 to string2
//	[in]Str1	string to subtract
//	[in]Str2	string to be subtracted
//	return:	0 if matched, 1 if Str1>Str2 , -1 if Str1<Str2
template<class TPtr1,class TPtr2>
inline sfInt8 ViewCompare(TPtr1 Str1,TPtr2 Str2)noexcept(cnLib_NOEXCEPTEXPR(*Str1<*Str2) && cnLib_NOEXCEPTEXPR(*Str1==*Str2))
{
	while(*Str1==*Str2){
		if(*Str1==0)
			return 0;
		++Str1;
		++Str2;
	}
	if(*Str1<*Str2)
		return -1;
	else
		return 1;
}


// ViewCompare
//	The encoding relation of string1 to string2
//	[in]Str1	string to subtract
//	[in]Str2	string to be subtracted
//	return:	0 if matched, 1 if Str1>Str2 , -1 if Str1<Str2
template<class TPtr1,class TPtr2>
inline sfInt8 ViewCompare(TPtr1 Str1,uIntn Str1Length,TPtr2 Str2)noexcept(cnLib_NOEXCEPTEXPR(*Str1<*Str2) && cnLib_NOEXCEPTEXPR(*Str1==*Str2))
{
	for(uIntn i=0;i<Str1Length;i++){
		if(!(*Str1==*Str2)){
			if(*Str1<*Str2)
				return -1;
			return 1;
		}
		++Str1;
		++Str2;
	}
	cnLib_ASSERT(*Str1==0);
	if(*Str1==*Str2)
		return 0;
	if(*Str1<*Str2)
		return -1;
	return 1;
}

// ViewCompare
//	The encoding relation of string1 to string2
//	[in]Str1	string to subtract
//	[in]Str2	string to be subtracted
//	return:	0 if matched, 1 if Str1>Str2 , -1 if Str1<Str2
template<class TPtr1,class TPtr2>
inline sfInt8 ViewCompare(TPtr1 Str1,uIntn Str1Length,TPtr2 Str2,uIntn Str2Length)noexcept(cnLib_NOEXCEPTEXPR(*Str1<*Str2) && cnLib_NOEXCEPTEXPR(*Str1>*Str2))
{
	if(Str1Length==Str2Length){
		return ViewCompare(Str1,Str1Length,Str2);
	}

	uIntn CompareLength;
	bool TailCharIsZero;
	sfInt8 TailRelation;
	if(Str1Length<Str2Length){
		CompareLength=Str1Length;
		TailCharIsZero=Str2[Str1Length]==0;
		TailRelation=-1;
	}
	else{
		CompareLength=Str2Length;
		TailCharIsZero=Str1[Str2Length]==0;
		TailRelation=1;
	}
	for(uIntn Index=0;Index<Str1Length;Index++){
		if(*Str1<*Str2)
			return -1;
		else if(*Str1>*Str2)
			return 1;
		if(*Str1==0)
			return 0;
		++Str1;
		++Str2;
	}
	if(TailCharIsZero)
		return 0;
	else
		return TailRelation;
}
//---------------------------------------------------------------------------
} 	// namespace cnString
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
inline void* operator new (cnLibrary::tSize,cnLibrary::cnVar::cNoInitialization *Pointer)noexcept(true){	return Pointer;}
inline void operator delete(void *,cnLibrary::cnVar::cNoInitialization*)noexcept(true){}
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
	void Destruct(void)noexcept(cnLib_NOEXCEPTEXPR(cnVar::DeclVar<TObject&>().~TObject()))
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

#if cnLibrary_CPPFEATURE_CONSTEXPR >= 200704L && !defined(cnLibrary_CPPEXCLUDE_CONSTEXPR_STATIC_INITALIZATION)

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

#else	// cnLibrary_CPPFEATURE_CONSTEXPR < 200704L || defined(cnLibrary_CPPEXCLUDE_CONSTEXPR_STATIC_INITALIZATION)

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

#endif	// cnLibrary_CPPFEATURE_CONSTEXPR , defined(cnLibrary_CPPEXCLUDE_CONSTEXPR_STATIC_INITALIZATION)

#endif	// cnLibrary_CPPFEATURE_CONSTINIT < 201907L

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
