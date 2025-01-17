/*- cnTK - Array ----------------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2018-08-14                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnTK_Array_HPP__
#define	__cnLibrary_cnTK_Array_HPP__
/*-------------------------------------------------------------------------*/
#include <cnTK/Common.hpp>
#include <cnTK/TypeTraits.hpp>
#include <cnTK/Interface.hpp>
#include <cnTK/Memory.hpp>
#include <cnTK/Variant.hpp>
/*-------------------------------------------------------------------------*/
#if	cnLibrary_CPPFEATURELEVEL >= 1
//---------------------------------------------------------------------------
#include <cnTK/TKMacrosDeclare.inc>
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnMemory{
//---------------------------------------------------------------------------

template<class TPtr1,class TPtr2>
inline bool ViewEqual(TPtr1 Array1,TPtr2 Array2,uIntn Length)noexcept(noexcept(*Array1==*Array2) && noexcept(++Array1) && noexcept(++Array2))
{
	for(uIntn i=0;i<Length;i++){
		if(!(*Array1==*Array2)){
			return false;
		}
		++Array1;
		++Array2;
	}
	return true;
}

// ViewCompare
//	The encoding relation of string1 to string2
//	[in]Src1	string to subtract
//	[in]Src2	string to be subtracted
//	[in]Length	length of first characters in string to compare
//	return:	the differ of first differnet char, 0 if two strings matched
template<class TPtr1,class TPtr2>
inline sfInt8 ViewCompare(TPtr1 Array1,TPtr2 Array2,uIntn Length)noexcept(noexcept(*Array1==*Array2) && noexcept(*Array1<*Array2) && noexcept(++Array1) && noexcept(++Array2))
{
	for(uIntn i=0;i<Length;i++){
		if(!(*Array1==*Array2)){
			return *Array1<*Array2?-1:1;
		}
		++Array1;
		++Array2;
	}
	return 0;
}


// ViewMatchLength
//	Find match length between 2 array
//	[in]Array1
//	[in]Array2
//	[in]Length	length to compare
//	return:	the length of matched elements
//template<class TPtr1,class TPtr2>
//inline uIntn ViewMatchLength(TPtr1 Array1,TPtr2 Array2,uIntn Length)noexcept(noexcept(*Array1!=*Array2))
//{
//	for(uIntn i=0;i<Length;i++){
//		if(Array1[i]!=Array2[i]){
//			return i;
//		}
//	}
//	return Length;
//}

//---------------------------------------------------------------------------
}	// namespace cnMemory
//---------------------------------------------------------------------------
namespace TKRuntime{
//---------------------------------------------------------------------------
template<uIntn ElementSize>	
struct TArray;
//{
//	static void Fill(void *Dest,uIntn Length,TInt Data)noexcept;
//	static uIntn Search(const void *Src,uIntn Length,TInt Dest)noexcept;
//};

//---------------------------------------------------------------------------
}	// namespace TKRuntime
//---------------------------------------------------------------------------
namespace CPPRuntime{
//---------------------------------------------------------------------------
template<uIntn ElementSize>	
struct TArray
{
	typedef typename cnVar::TIntegerOfSize<ElementSize,false>::Type tUInt;

	static void Fill(void *Dest,uIntn Length,tUInt Data)noexcept(true){
		for(uIntn i=0;i<Length;i++){
			static_cast<tUInt*>(Dest)[i]=Data;
		}
	}

	static uIntn Search(const void *Src,uIntn Length,tUInt Dest)noexcept(true){
		for(uIntn i=0;i<Length;i++){
			if(static_cast<const tUInt*>(Src)[i]==Dest){
				return i;
			}
		}
		return IndexNotFound;
	}
	
	static uIntn ReverseSearch(const void *Src,uIntn Length,tUInt Dest)noexcept(true){
		while(Length!=0){
			Length--;
			if(static_cast<const tUInt*>(Src)[Length]==Dest)
				return Length;
		}
		return IndexNotFound;
	}

};

//---------------------------------------------------------------------------
}	// namespace CPPRuntime
//---------------------------------------------------------------------------
namespace cnMemory{
//---------------------------------------------------------------------------

// Fill
//	Fill Dest with Data
// [in]Dest		array to fill
// [in]Length	length to fill
// [in]Data		data
template<class T>
inline void Fill(void *Dest,uIntn Length,const T Data)noexcept(true)
{
	typedef typename cnVar::TIntegerOfSize<sizeof(T),false>::Type tUInt;
	return TKRuntime::TArray<sizeof(T)>::Fill(Dest,Length,reinterpret_cast<const tUInt&>(Data));
}

// Search
//	Search data in Src
// Src	[in]	array to search
// Length		length of array
// Dest			data to search
// Return:	the index of the first matched data, or -1 if the Dest not found
template<class T>
inline uIntn Search(const T *Src,uIntn Length,typename cnVar::TTypeDef<T>::Type Dest)noexcept(true)
{
	typedef typename cnVar::TIntegerOfSize<sizeof(T),false>::Type tUInt;
	return TKRuntime::TArray<sizeof(T)>::Search(Src,Length,reinterpret_cast<tUInt&>(Dest));
}

// ViewReverseSearch
//	Search data in Src in backward direction
// [in]Src	array to search
// [in]Length	length of array
// [in]Dest	data to search
// Return:	the index of the first matched data, or -1 if the Dest not found
template<class T>
inline uIntn ReverseSearch(const T *Src,uIntn Length,typename cnVar::TTypeDef<T>::Type Dest)noexcept(true)
{
	typedef typename cnVar::TIntegerOfSize<sizeof(T),false>::Type tUInt;
	return TKRuntime::TArray<sizeof(T)>::Search(Src,Length,reinterpret_cast<tUInt&>(Dest));
}

template<class T>
inline sfInt8 Compare(const T *Src1,uIntn Length1,const T *Src2,uIntn Length2)noexcept(true)
{
	if(Length1==Length2){
		return TKRuntime::TMemory<sizeof(T)>::Compare(Src1,Src2,Length1);
	}
	if(Length1<Length2){
		sfInt8 r=TKRuntime::TMemory<sizeof(T)>::Compare(Src1,Src2,Length1);
		return r==0?-1:r;
	}
	else{
		sfInt8 r=TKRuntime::TMemory<sizeof(T)>::Compare(Src1,Src2,Length2);
		return r==0?1:r;
	}
}

//---------------------------------------------------------------------------
}	// namespace cnMemory
//---------------------------------------------------------------------------
template<class T>
struct TArrayElement : cnVar::TTypeDef<T>{};
template<>
struct TArrayElement<void> : cnVar::TTypeDef<uInt8>{};
template<>
struct TArrayElement<const void> : cnVar::TTypeDef<const uInt8>{};
template<>
struct TArrayElement<volatile void> : cnVar::TTypeDef<volatile uInt8>{};
template<>
struct TArrayElement<const volatile void> : cnVar::TTypeDef<const volatile uInt8>{};
//---------------------------------------------------------------------------
template<class T>
struct TArrayElementSize : cnVar::TConstantValueUIntn<sizeof(T)>{};
template<>
struct TArrayElementSize<void> : cnVar::TConstantValueUIntn<1>{};
template<>
struct TArrayElementSize<const void> : cnVar::TConstantValueUIntn<1>{};
template<>
struct TArrayElementSize<volatile void> : cnVar::TConstantValueUIntn<1>{};
template<>
struct TArrayElementSize<const volatile void> : cnVar::TConstantValueUIntn<1>{};
//---------------------------------------------------------------------------
template<class T>
T* ArrayOffsetPointer(T *Pointer,uIntn Offset)noexcept(true)
{
	return static_cast<typename TArrayElement<T>::Type*>(Pointer)+Offset;
}
//---------------------------------------------------------------------------

template<class T>
struct cArrayConstant : cArray<const T>
{
};

struct cNullArray
{
	template<class T>
	operator cArray<T>()const noexcept(true){
		cArray<T> Array={nullptr,0};
		return Array;
	}
};
static cnLib_CONSTVAR cNullArray NullArray;

//---------------------------------------------------------------------------
#if cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L

template<class...T>
struct cMultiArray
{
	static cnLib_CONSTVAR uIntn PointerCount=sizeof...(T);
	cnVar::cVarPack<T*...> Pointer;
	uIntn Length;
};

#endif	// cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L

//---------------------------------------------------------------------------
namespace cnVar{
//---------------------------------------------------------------------------

template<class T,uIntn Length>
struct TArrayClassDecl;

#if cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES < 200704L

template<class T,uIntn Length>
struct TArrayClassDecl
{
	struct Type{
		T Array[Length];
	};
};

// cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES < 200704L
#else
// cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L

template<class TTypePack,class TIndexSequence>
struct cArrayClassStorage;

template<class T,class...VT,uIntn...Indics>
struct cArrayClassStorage< TTypePack<T,VT...>,TValueSequence<uIntn,Indics...> >
{

#ifndef cnLibrary_CPPEXCLUDE_CLASS_MEMBER_DEFAULT
	cArrayClassStorage()=default;
	cArrayClassStorage(const cArrayClassStorage&)=default;
#else
	cnLib_CONSTEXPR_FUNC cArrayClassStorage()noexcept(true){}
	cnLib_CONSTEXPR_FUNC cArrayClassStorage(const cArrayClassStorage &Src)
		: Array{Src.Array[Indics]...}{}
#endif // !cnLibrary_CPPEXCLUDE_CLASS_MEMBER_DEFAULT

#if cnLibrary_CPPFEATURE_INITIALIZER_LIST >= 200806L

	cnLib_CONSTEXPR_FUNC cArrayClassStorage(T v0,VT...v)noexcept(true)
		: Array{v0,v...}{}

#endif	// cnLibrary_CPPFEATURE_INITIALIZER_LIST >= 200806L
	T Array[1+sizeof...(VT)];
};

template<class T,uIntn Length>
struct TArrayClassDecl
	: TTypeDef<
		cnVar::cArrayClassStorage<
			typename TMakeTypePackRepeat< TTypePack<>,Length,T>::Type,
			typename TMakeIndexSequence< TValueSequence<uIntn>,Length>::Type
		>
	>{};

#endif	// cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L

//---------------------------------------------------------------------------
}	// namespace cnVar
//---------------------------------------------------------------------------
namespace cnDataStruct{
//---------------------------------------------------------------------------

template<class TElement>
struct cTrivialArrayMemoryOperator
{
	typedef TElement tElement;

	
	static void Copy(tElement *Dest,const tElement *Src,uIntn Length)noexcept(true){
		return TKRuntime::TMemory<sizeof(TElement)>::Copy(Dest,Src,Length);
	}
	static void CopyOverlapped(tElement *Dest,const tElement *Src,uIntn Length)noexcept(true){
		return TKRuntime::TMemory<sizeof(TElement)>::CopyOverlapped(Dest,Src,Length);
	}

	static void Move(tElement *Dest,tElement *Src,uIntn Length)noexcept(true){
		return TKRuntime::TMemory<sizeof(TElement)>::Copy(Dest,Src,Length);
	}
	static void MoveOverlapped(tElement *Dest,tElement *Src,uIntn Length)noexcept(true){
		return TKRuntime::TMemory<sizeof(TElement)>::CopyOverlapped(Dest,Src,Length);
	}
};
template<class TElement>
struct cClassArrayMemoryOperator
{
	typedef TElement tElement;

	static void Copy(tElement *Dest,const tElement *Src,uIntn Length)noexcept(cnVar::TIsCopyAssignNoexcept<TElement>::Value){
		for(uIntn i=0;i<Length;i++){
			Dest[i]=Src[i];
		}
	}
	static void CopyOverlapped(tElement *Dest,const tElement *Src,uIntn Length)noexcept(cnVar::TIsCopyAssignNoexcept<TElement>::Value){
		if(Dest<Src || Dest>=ArrayOffsetPointer(Src,Length)){
			return Copy(Dest,Src,Length);
		}
		while(Length>0){
			Length--;
			Dest[Length]=Src[Length];
		}
	}

	static void Move(tElement *Dest,tElement *Src,uIntn Length)noexcept(cnVar::TIsMoveAssignNoexcept<TElement>::Value){
		for(uIntn i=0;i<Length;i++){
			Dest[i]=cnLib_UREFCAST(tElement)(Src[i]);
		}
	}
	static void MoveOverlapped(tElement *Dest,tElement *Src,uIntn Length)noexcept(cnVar::TIsMoveAssignNoexcept<TElement>::Value){
		if(Dest<Src || Dest>=ArrayOffsetPointer(Src,Length)){
			return Move(Dest,Src,Length);
		}
		while(Length>0){
			Length--;
			Dest[Length]=cnLib_UREFCAST(tElement)(Src[Length]);
		}
	}


};


template<class TElement>
struct cArrayMemoryOperator
	: cnVar::TSelect< cnVar::TIsTrivial<TElement>::Value
		, cClassArrayMemoryOperator<TElement>
		, cTrivialArrayMemoryOperator<TElement>
	>::Type{};


//---------------------------------------------------------------------------

//TAllocationOperator
//{
//	static uIntn RoundUpCapacity(uIntn Value)noexcept;
//	static void* Allocate(uIntn Size,uIntn AlignSize)noexcept;
//	static void Deallocate(void* *Pointer,uIntn Size,uIntn AlignSize)noexcept;
//	static bool Resize(void *Pointer,uIntn Size,uIntn Size)noexcept;
//	static void* Relocate(void *Pointer,uIntn Length,uIntn AlignLength,uIntn NewLength,uIntn NewAlignLength,bool &ManualCopy)noexcept;
//};

template<class TAllocationOperator,class TElement>
struct cArrayAllocation : cArray<TElement>
{
	typedef TAllocationOperator tAllocationOperator;
	uIntn Capacity;
};

//---------------------------------------------------------------------------


template<class TAllocationOperator,class TElement>
struct cTrivialArrayMemoryFunction
{
	typedef TElement tElement;

	static tElement* Allocate(uIntn Length)noexcept(true){
		return static_cast<tElement*>(TAllocationOperator::Allocate(Length*sizeof(tElement),sizeof(tElement)));
	}
	static void Deallocate(tElement *Pointer,uIntn Length)noexcept(true){
		return TAllocationOperator::Deallocate(Pointer,Length*sizeof(tElement),sizeof(tElement));
	}

	static uIntn RoundUpCapacity(uIntn Value)noexcept(true){	return TAllocationOperator::RoundUpCapacity(Value*sizeof(tElement))/sizeof(tElement);	}

	static void Construct(tElement *,uIntn,uIntn)noexcept(true){}
	static void Destruct(tElement *,uIntn,uIntn)noexcept(true){}

	static void CopyConstruct(tElement *Array,uIntn ArrayIndex,const tElement *SrcArray,uIntn SrcIndex,uIntn Length)noexcept(true){
		TKRuntime::TMemory<sizeof(TElement)>::Copy(Array+ArrayIndex,SrcArray+SrcIndex,Length);
	}
	static void MoveConstruct(tElement *Array,uIntn ArrayIndex,tElement *SrcArray,uIntn SrcIndex,uIntn Length)noexcept(true){
		TKRuntime::TMemory<sizeof(TElement)>::Copy(Array+ArrayIndex,SrcArray+SrcIndex,Length);
	}

	static void Copy(tElement *Array,uIntn ArrayIndex,const tElement *SrcArray,uIntn SrcIndex,uIntn Length)noexcept(true){
		TKRuntime::TMemory<sizeof(TElement)>::Copy(Array+ArrayIndex,SrcArray+SrcIndex,Length);
	}

	static void MoveInArray(tElement *Array,uIntn DestIndex,uIntn SrcIndex,uIntn Length)noexcept(true){
		TKRuntime::TMemory<sizeof(TElement)>::Copy(Array+DestIndex,Array+SrcIndex,Length);
	}

	static void CopyHeadConstructTail(tElement *Array,uIntn ArrayIndex,const tElement *SrcArray,uIntn SrcIndex,uIntn,uIntn TotalLength)noexcept(true){
		TKRuntime::TMemory<sizeof(TElement)>::Copy(Array+ArrayIndex,SrcArray+SrcIndex,TotalLength);
	}

	static void MoveHeadConstructTailInArray(tElement *Array,uIntn ArrayLength,uIntn DestIndex,uIntn MoveLength)noexcept(true){
		cnLib_ASSERT(DestIndex+MoveLength>ArrayLength);
		TKRuntime::TMemory<sizeof(TElement)>::CopyOverlapped(Array+DestIndex,Array+ArrayLength-MoveLength,MoveLength);
	}

	static tElement* Relocate(tElement *Pointer,uIntn Length,uIntn NewLength,bool &ManualCopy)noexcept(true)
	{	return static_cast<tElement*>(TAllocationOperator::Relocate(Pointer,Length*sizeof(tElement),sizeof(tElement),NewLength*sizeof(tElement),sizeof(tElement),ManualCopy));	}
};

template<class TElement>
struct cClassArrayElementFunction
{
	static void Construct(TElement *Array,uIntn Index,uIntn EndIndex)noexcept(cnVar::TIsDefaultConstructNoexcept<TElement>::Value){
		for(uIntn i=Index;i<EndIndex;i++)
			cnClass::ManualConstruct(Array[i]);
	}
	static void Destruct(TElement *Array,uIntn Index,uIntn EndIndex)noexcept(cnVar::TIsDestructNoexcept<TElement>::Value){
		for(uIntn i=Index;i<EndIndex;i++)
			cnClass::ManualDestruct(Array[i]);
	}

	static void CopyConstruct(TElement *Array,uIntn ArrayIndex,const TElement *SrcArray,uIntn SrcIndex,uIntn Length)noexcept(cnVar::TIsCopyConstructNoexcept<TElement>::Value){
		Array+=ArrayIndex;
		SrcArray+=SrcIndex;
		for(uIntn i=0;i<Length;i++)
			cnClass::ManualConstruct(Array[i],SrcArray[i]);
	}
	
	static void Copy(TElement *Array,uIntn ArrayIndex,const TElement *SrcArray,uIntn SrcIndex,uIntn Length)noexcept(cnVar::TIsCopyAssignNoexcept<TElement>::Value){
		Array+=ArrayIndex;
		SrcArray+=SrcIndex;
		for(uIntn i=0;i<Length;i++){
			Array[i]=SrcArray[i];
		}
	}

	static void CopyHeadConstructTail(TElement *Array,uIntn ArrayIndex,const TElement *SrcArray,uIntn SrcIndex,uIntn HeadLength,uIntn TotalLength)
		noexcept(cnVar::TIsCopyConstructNoexcept<TElement>::Value && cnVar::TIsCopyAssignNoexcept<TElement>::Value)
	{
		Array+=ArrayIndex;
		SrcArray+=SrcIndex;
		uIntn Index;
		for(Index=0;Index<HeadLength;Index++){
			Array[Index]=SrcArray[Index];
		}
		while(Index<TotalLength){
			cnClass::ManualConstruct(Array[Index],SrcArray[Index]);
			Index++;
		}
	}

#if defined(cnLibrary_CPPEXCLUDE_NOEXCEPT)

// defined(cnLibrary_CPPEXCLUDE_NOEXCEPT)
#else
// !defined(cnLibrary_CPPEXCLUDE_NOEXCEPT)


#endif	// !defined(cnLibrary_CPPEXCLUDE_NOEXCEPT)

	static void MoveConstruct(TElement *Array,uIntn ArrayIndex,TElement *SrcArray,uIntn SrcIndex,uIntn Length)noexcept(cnVar::TIsMoveConstructNoexcept<TElement>::Value){
		Array+=ArrayIndex;
		SrcArray+=SrcIndex;
		for(uIntn i=0;i<Length;i++)
			cnClass::ManualConstruct(Array[i],cnLib_UREFCAST(TElement)(SrcArray[i]));
	}

	static void MoveInArray(TElement *Array,uIntn DestIndex,uIntn SrcIndex,uIntn Length)noexcept(cnVar::TIsMoveAssignNoexcept<TElement>::Value){
		TElement *SrcArray=Array+SrcIndex;
		Array+=DestIndex;
		for(uIntn i=0;i<Length;i++){
			cnVar::AssignOrReconstruct(Array[i],cnLib_UREFCAST(TElement)(SrcArray[i]));
		}
	}

	static void MoveHeadConstructTailInArray(TElement *Array,uIntn ArrayLength,uIntn DestIndex,uIntn MoveLength)
		noexcept(cnVar::TIsMoveConstructNoexcept<TElement>::Value && cnVar::TIsMoveAssignNoexcept<TElement>::Value)
	{
		uIntn ArrayIndex=DestIndex+MoveLength;
		cnLib_ASSERT(ArrayIndex>ArrayLength);
		uIntn SrcArrayIndex=ArrayLength;
		do{
			ArrayIndex--;
			SrcArrayIndex--;
			
			cnClass::ManualConstruct(Array[ArrayIndex],cnLib_UREFCAST(TElement)(Array[SrcArrayIndex]));
			
		}while(ArrayIndex>ArrayLength);

		while(ArrayIndex>DestIndex){
			ArrayIndex--;
			SrcArrayIndex--;
			cnVar::AssignOrReconstruct(Array[ArrayIndex],cnLib_UREFCAST(TElement)(Array[SrcArrayIndex]));

		}
	}

};

template<class TAllocationOperator,class TElement>
struct cClassArrayMemoryFunction
	: cClassArrayElementFunction<TElement>
{
	typedef TElement tElement;

	static tElement* Allocate(uIntn Length)noexcept(true)
	{	return static_cast<tElement*>(TAllocationOperator::Allocate(Length*sizeof(tElement),sizeof(tElement)));	}
	static void Deallocate(tElement *Pointer,uIntn Length)noexcept(true)
	{	return TAllocationOperator::Deallocate(Pointer,Length*sizeof(tElement),sizeof(tElement));	}

	static uIntn RoundUpCapacity(uIntn Value)noexcept(true){	return TAllocationOperator::RoundUpCapacity(Value*sizeof(tElement))/sizeof(tElement);	}

	static tElement* Relocate(tElement *Pointer,uIntn Length,uIntn NewLength,bool &ManualCopy)noexcept(true){
		if(TAllocationOperator::Resize(Pointer,Length*sizeof(tElement),NewLength*sizeof(tElement))){
			// resized
			ManualCopy=false;
			return Pointer;
		}
		// allocate new
		ManualCopy=true;
		return static_cast<tElement*>(TAllocationOperator::Allocate(NewLength*sizeof(tElement),1));
	}
};


template<class TAllocationOperator,class TElement>
struct cArrayMemoryFunction
	: cnVar::TSelect< cnVar::TIsTrivial<TElement>::Value,
		cClassArrayMemoryFunction<TAllocationOperator,TElement>,
		cTrivialArrayMemoryFunction<TAllocationOperator,TElement>
	>::Type{};

template<class TAllocationOperator>
struct cArrayMemoryFunction<TAllocationOperator,void>
{

	typedef void tElement;

	
	static void* Allocate(uIntn Size)noexcept(true)
	{
		return TAllocationOperator::Allocate(Size,1);
	}
	static void Deallocate(void *Pointer,uIntn Size)noexcept(true){
		return TAllocationOperator::Deallocate(Pointer,Size,1);
	}
	
	static uIntn RoundUpCapacity(uIntn Value)noexcept(true){	return TAllocationOperator::RoundUpCapacity(Value);	}

	static void Construct(void*,uIntn,uIntn)noexcept(true){}
	static void Destruct(void*,uIntn,uIntn)noexcept(true){}

	typedef typename cnVar::TSelect<0,TKRuntime::TMemory<1>,TAllocationOperator>::Type tMemory;

	static void CopyConstruct(void *Array,uIntn ArrayIndex,const void*SrcArray,uIntn SrcIndex,uIntn Length)noexcept(true){
		tMemory::Copy(cnMemory::PointerAddByteOffset(Array,ArrayIndex),cnMemory::PointerAddByteOffset(SrcArray,SrcIndex),Length);
	}
	static void Copy(void *Array,uIntn ArrayIndex,const void *SrcArray,uIntn SrcIndex,uIntn Length)noexcept(true){
		tMemory::Copy(cnMemory::PointerAddByteOffset(Array,ArrayIndex),cnMemory::PointerAddByteOffset(SrcArray,SrcIndex),Length);
	}

	static void CopyHeadConstructTail(void *Array,uIntn ArrayIndex,const void *SrcArray,uIntn SrcIndex,uIntn,uIntn TotalLength)noexcept(true){
		tMemory::Copy(cnMemory::PointerAddByteOffset(Array,ArrayIndex),cnMemory::PointerAddByteOffset(SrcArray,SrcIndex),TotalLength);
	}

	static void MoveConstruct(void *Array,uIntn ArrayIndex,void *SrcArray,uIntn SrcIndex,uIntn Length)noexcept(true){
		tMemory::Copy(cnMemory::PointerAddByteOffset(Array,ArrayIndex),cnMemory::PointerAddByteOffset(SrcArray,SrcIndex),Length);
	}

	static void MoveInArray(void *Array,uIntn DestIndex,uIntn SrcIndex,uIntn Length)noexcept(true){
		tMemory::Copy(cnMemory::PointerAddByteOffset(Array,DestIndex),cnMemory::PointerAddByteOffset(Array,SrcIndex),Length);
	}

	static void MoveHeadConstructTailInArray(void *Array,uIntn ArrayLength,uIntn DestIndex,uIntn MoveLength)noexcept(true){
		cnLib_ASSERT(DestIndex+MoveLength>ArrayLength);
		tMemory::CopyOverlapped(cnMemory::PointerAddByteOffset(Array,DestIndex),cnMemory::PointerAddByteOffset(Array,ArrayLength-MoveLength),MoveLength);
	}

	static void* Relocate(void *Pointer,uIntn Length,uIntn NewLength,bool &ManualCopy)noexcept(true)
	{	return TAllocationOperator::Relocate(Pointer,Length,0,NewLength,0,ManualCopy);	}
};

//---------------------------------------------------------------------------

template<class TAllocationOperator,class TElement>
struct cArrayBlock : cArray<TElement>
{
	typedef TAllocationOperator tAllocationOperator;
	typedef TElement tElement;
	typedef cArrayAllocation<TAllocationOperator,tElement> tArrayAllocation;
	typedef TElement *tPointer;
	typedef TElement const *tConstPointer;
	typedef cArray<TElement> tArray;
	typedef cArray<const TElement> tConstArray;
	typedef cArrayMemoryFunction<TAllocationOperator,TElement> TAMFunc;

#if !defined(cnLibrary_CPPEXCLUDE_NOEXCEPT) && cnLibrary_CPPFEATURE_STATIC_ASSERT >= 200410L

	static_assert(cnVar::TIsSame<tElement,void>::Value || cnVar::TIsDefaultConstructNoexcept<tElement>::Value,"noexcept default constructor required");
	static_assert(cnVar::TIsSame<tElement,void>::Value || cnVar::TIsDestructNoexcept<tElement>::Value,"noexcept destructor required");
	static_assert(cnVar::TIsSame<tElement,void>::Value || cnVar::TIsCopyConstructNoexcept<tElement>::Value,"noexcept copy constructor required");
	static_assert(cnVar::TIsSame<tElement,void>::Value || cnVar::TIsMoveConstructNoexcept<tElement>::Value,"noexcept move constructor required");
	static_assert(cnVar::TIsSame<tElement,void>::Value || cnVar::TIsCopyAssignNoexcept<tElement>::Value,"noexcept copy assignment required");

#endif	// !defined(cnLibrary_CPPEXCLUDE_NOEXCEPT) && cnLibrary_CPPFEATURE_STATIC_ASSERT >= 200410L

	void Reset(void)noexcept(true){
		this->Pointer=nullptr;
		this->Length=0;
	}
	void Reset(uIntn Length)noexcept(true){
		if(Length==0){
			return Reset();
		}
		// allocate
		this->Length=Length;
		this->Pointer=TAMFunc::Allocate(Length);
		if(this->Pointer==nullptr)
			this->Length=0;
	}

	void ResetDuplicate(tConstPointer SrcPointer,uIntn SrcLength)noexcept(true){
		if(SrcLength==0){
			return Reset();
		}
		// allocate
		this->Length=SrcLength;
		this->Pointer=TAMFunc::Allocate(SrcLength);
		TAMFunc::CopyConstruct(this->Pointer,0,SrcPointer,0,SrcLength);
	}

	cArrayBlock()noexcept(true){	Reset();	}
    ~cArrayBlock()noexcept(true)
	{
		if(this->Length!=0){
			TAMFunc::Destruct(this->Pointer,0,this->Length);
			TAMFunc::Deallocate(this->Pointer,this->Length);
		}
	}

	cArrayBlock(tConstPointer Data,uIntn Length)noexcept(true){
		ResetDuplicate(Data,Length);
	}
	
	cArrayBlock(const cArrayBlock &Src)noexcept(true){
		ResetDuplicate(Src.Pointer,Src.Length);
	}
	
	cArrayBlock(const tConstArray &Src)noexcept(true){
		ResetDuplicate(Src.Pointer,Src.Length);
	}

	cArrayBlock& operator =(const cArrayBlock &Src)noexcept(true){
		if(this!=&Src){
			Copy(Src.Pointer,Src.Length);
		}
		return *this;
	}

	cArrayBlock& operator =(const tConstArray &Src)noexcept(true){
		if(ArrayCast<tConstArray>(this)!=&Src){
			Copy(Src.Pointer,Src.Length);
		}
		return *this;
	}
	
#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L
	
	cArrayBlock(cArrayBlock &&Src)noexcept(true){
		this->Pointer=Src.Pointer;
		this->Length=Src.Length;
		Src.Reset();
	}

	void Reset(tArrayAllocation &&Src)noexcept(true){
		if(Src.Length==0){
			this->Pointer=nullptr;
			this->Length=0;
		}
		else{
			if(Src.Length==Src.Capacity){
				// take memory from src
				this->Pointer=Src.Pointer;
				this->Length=Src.Capacity;
				Src.Pointer=nullptr;
				Src.Length=0;
				Src.Capacity=0;
			}
			else{
				// take memory from src and relocate
				bool ManualCopy;
				tPointer NewArray=TAMFunc::Relocate(Src.Pointer,Src.Capacity,Src.Length,ManualCopy);
				this->Pointer=NewArray;
				this->Length=Src.Length;
				if(ManualCopy){
					TAMFunc::MoveConstruct(NewArray,0,Src.Pointer,0,Src.Length);
				}
				else{
					Src.Pointer=nullptr;
					Src.Capacity=0;
					Src.Length=0;
				}
			}
		}
	}

	cArrayBlock(tArrayAllocation &&Array)noexcept(true){
		Reset(static_cast<tArrayAllocation&&>(Array));
	}

	cArrayBlock& operator =(cArrayBlock &&Src)noexcept(true){
		if(this->Length!=0){
			TAMFunc::Destruct(this->Pointer,0,this->Length);
			TAMFunc::Deallocate(this->Pointer,this->Length);
		}

		this->Pointer=Src.Pointer;
		this->Length=Src.Length;
		Src.Reset();
		return *this;
	}

	cArrayBlock& operator =(tArrayAllocation &&Array)noexcept(true){
		if(this->Pointer!=nullptr){
			// discard previous block
			TAMFunc::Destruct(this->Pointer,0,this->Length);
			TAMFunc::Deallocate(this->Pointer,this->Length);
		}
		Reset(static_cast<tArrayAllocation&&>(Array));
		return *this;
	}

#endif	// cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L



	operator tPointer ()const noexcept(true){	return static_cast<tPointer>(this->Pointer);	}
	
	tPointer GetPointer(uIntn Offset)const noexcept(true){	return ArrayOffsetPointer(this->Pointer,Offset);	}

	cArrayBlock Relocate(uIntn NewLength)noexcept(true){
		cArrayBlock PreviousBlock;
		bool ManualCopy;
		tPointer NewArray=TAMFunc::Relocate(this->Pointer,this->Length,NewLength,ManualCopy);
		if(ManualCopy){
			PreviousBlock.Pointer=this->Pointer;
			PreviousBlock.Length=this->Length;
		}
		this->Pointer=NewArray;
		this->Length=NewLength;
		return PreviousBlock;
	}

	void Clear(void)noexcept(true){
		if(this->Length==0){
			return;
		}
		TAMFunc::Destruct(this->Pointer,0,this->Length);
		TAMFunc::Deallocate(this->Pointer,this->Length);
		Reset();
	}

	void Copy(tConstPointer Src,uIntn SrcLength)noexcept(true)
	{
		if(SrcLength==this->Length){
			TAMFunc::Copy(this->Pointer,0,Src,0,SrcLength);
		}
		else if(SrcLength<this->Length){
			// shrink array
			TAMFunc::Destruct(this->Pointer,SrcLength,this->Length);

			bool ManualCopy;
			tPointer NewArray=TAMFunc::Relocate(this->Pointer,this->Length,SrcLength,ManualCopy);
			if(ManualCopy){
				TAMFunc::Destruct(this->Pointer,0,SrcLength);
				TAMFunc::Deallocate(this->Pointer,this->Length);

				TAMFunc::CopyConstruct(NewArray,0,Src,0,SrcLength);
			}
			else{
				// copy value
				TAMFunc::Copy(NewArray,0,Src,0,SrcLength);
			}
			this->Pointer=NewArray;
			this->Length=SrcLength;
		}
		else{	// SrcLength > this->Length
			bool ManualCopy;
			tPointer NewArray=TAMFunc::Relocate(this->Pointer,this->Length,SrcLength,ManualCopy);
			if(ManualCopy){
				TAMFunc::Destruct(this->Pointer,0,this->Length);
				TAMFunc::Deallocate(this->Pointer,this->Length);

				TAMFunc::CopyConstruct(NewArray,0,Src,0,SrcLength);
			}
			else{
				// overwrite existing array
				TAMFunc::CopyHeadConstructTail(NewArray,0,Src,0,this->Length,SrcLength);
			}
			this->Pointer=NewArray;
			this->Length=SrcLength;
		}
	}

	void SetLength(uIntn NewLength)noexcept(true)
	{
		if(NewLength==this->Length){
			return;
		}
		if(this->Length==0){
			// allocate new
			this->Length=NewLength;
			this->Pointer=TAMFunc::Allocate(NewLength);
			TAMFunc::Construct(this->Pointer,0,this->Length);
			return;
		}
		if(NewLength==0){
			// dealloc
			TAMFunc::Destruct(this->Pointer,0,this->Length);
			TAMFunc::Deallocate(this->Pointer,this->Length);
			Reset();
			return;
		}
		// reallocate
		if(NewLength<this->Length){
			// destruct tail
			TAMFunc::Destruct(this->Pointer,NewLength,this->Length);
		}
		bool ManualCopy;
		tPointer NewArray=TAMFunc::Relocate(this->Pointer,this->Length,NewLength,ManualCopy);
		if(ManualCopy){
			TAMFunc::MoveConstruct(NewArray,0,this->Pointer,0,NewLength);
			TAMFunc::Destruct(this->Pointer,0,NewLength);
			TAMFunc::Deallocate(this->Pointer,this->Length);
		}
		if(NewLength>this->Length){
			// construct expanded elements
			TAMFunc::Construct(NewArray,this->Length,NewLength);
		}
		this->Pointer=NewArray;
		this->Length=NewLength;
	}
};


template<class TAllocationOperator,class TElement>
struct cArrayStorage : cArrayAllocation<TAllocationOperator,TElement>
{
	typedef TAllocationOperator tAllocationOperator;
	typedef TElement tElement;
	typedef cArrayAllocation<TAllocationOperator,tElement> tArrayAllocation;
	typedef TElement *tPointer;
	typedef TElement const *tConstPointer;
	typedef cArray<TElement> tArray;
	typedef cArray<const TElement> tConstArray;
	typedef cArrayMemoryFunction<TAllocationOperator,TElement> TAMFunc;

	//cnLib_STATIC_ASSERT(cnVar::TIsSame<tElement,void>::Value || cnVar::TIsDefaultConstructNoexcept<tElement>::Value,"noexcept default constructor required");
	//cnLib_STATIC_ASSERT(cnVar::TIsSame<tElement,void>::Value || cnVar::TIsDestructNoexcept<tElement>::Value,"noexcept destructor required");
	//cnLib_STATIC_ASSERT(cnVar::TIsSame<tElement,void>::Value || cnVar::TIsCopyConstructNoexcept<tElement>::Value,"noexcept copy constructor required");
	//cnLib_STATIC_ASSERT(cnVar::TIsSame<tElement,void>::Value || cnVar::TIsMoveConstructNoexcept<tElement>::Value,"noexcept move constructor required");
	//cnLib_STATIC_ASSERT(cnVar::TIsSame<tElement,void>::Value || cnVar::TIsCopyAssignNoexcept<tElement>::Value,"noexcept copy assignment required");

	void Reset(void)noexcept(true){
		this->Pointer=nullptr;
		this->Length=0;
		this->Capacity=0;
	}

	void ResetDuplicate(tConstPointer SrcPointer,uIntn SrcLength,uIntn NewCapacity)noexcept(noexcept(TAMFunc::CopyConstruct(0,0,0,0,0))){
		if(SrcLength==0){
			Reset();
			return;
		}
		cnLib_ASSERT(NewCapacity>=SrcLength);
		// allocate
		NewCapacity=TAMFunc::RoundUpCapacity(NewCapacity);
		this->Capacity=NewCapacity;
		this->Pointer=TAMFunc::Allocate(NewCapacity);

		// data
		this->Length=SrcLength;
		TAMFunc::CopyConstruct(this->Pointer,0,SrcPointer,0,SrcLength);
	}
	void ResetDuplicate(tConstPointer SrcPointer,uIntn SrcLength)noexcept(noexcept(TAMFunc::CopyConstruct(0,0,0,0,0))){
		return ResetDuplicate(SrcPointer,SrcLength,SrcLength);
	}

	cArrayStorage()noexcept(true){	Reset();	}
    ~cArrayStorage()noexcept(noexcept(TAMFunc::Destruct(0,0,0))){
		if(this->Length!=0)
			TAMFunc::Destruct(this->Pointer,0,this->Length);
		if(this->Capacity!=0)
			TAMFunc::Deallocate(this->Pointer,this->Capacity);
	}

	cArrayStorage(tConstPointer SrcData,uIntn SrcLength,uIntn InitialCapacity=0)noexcept(noexcept(TAMFunc::CopyConstruct(0,0,0,0,0))){
		if(InitialCapacity<SrcLength){
			InitialCapacity=SrcLength;
		}
		ResetDuplicate(SrcData,SrcLength,InitialCapacity);
	}
	
	cArrayStorage(const cArrayStorage &Src)noexcept(noexcept(TAMFunc::CopyConstruct(0,0,0,0,0))){
		ResetDuplicate(Src.Pointer,Src.Length);
	}

	cArrayStorage(const tConstArray &Src)noexcept(noexcept(TAMFunc::CopyConstruct(0,0,0,0,0))){
		ResetDuplicate(Src.Pointer,Src.Length);
	}
	cArrayStorage& operator =(const cArrayStorage &Src)noexcept(noexcept(Copy(0,0))){
		if(this!=cnMemory::AddressOf(Src)){
			Copy(Src.Pointer,Src.Length);
		}
		return *this;
	}
	
	cArrayStorage& operator =(const tConstArray &Src)noexcept(noexcept(Copy(0,0))){
		if(ArrayCast<tConstArray>(this)!=&Src){
			Copy(Src.Pointer,Src.Length);
		}
		return *this;
	}

#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L
	
	cArrayStorage(cArrayStorage &&Src)noexcept(true){
		this->Pointer=Src.Pointer;
		this->Length=Src.Length;
		this->Capacity=Src.Capacity;
		Src.Reset();
	}

	cArrayStorage(tArrayAllocation &&Src)noexcept(true){
		this->Pointer=Src.Pointer;
		this->Length=Src.Length;
		this->Capacity=Src.Capacity;
		Src.Pointer=nullptr;
		Src.Length=0;
		Src.Capacity=0;
	}

	cArrayStorage& operator =(cArrayStorage &&Src)noexcept(noexcept(TAMFunc::Destruct(0,0,0))){
		return operator=(static_cast<tArrayAllocation&&>(Src));
	}
	
	cArrayStorage& operator =(tArrayAllocation &&Src)noexcept(noexcept(TAMFunc::Destruct(0,0,0))){
		if(this->Capacity!=0){
			if(this->Length!=0){
				TAMFunc::Destruct(this->Pointer,0,this->Length);
			}
			TAMFunc::Deallocate(this->Pointer,this->Capacity);
		}
		this->Pointer=Src.Pointer;
		this->Length=Src.Length;
		this->Capacity=Src.Capacity;
		Src.Length=0;
		Src.Capacity=0;
		return *this;
	}

#endif	// cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

	cArrayStorage Relocate(uIntn NewCapacity)noexcept(noexcept(TAMFunc::Destruct(0,0,0))){
		cArrayStorage PreviousArray;
		NewCapacity=TAllocationOperator::RoundUpCapacity(NewCapacity);
	
		if(NewCapacity<this->Length){
			TAMFunc::Destruct(this->Pointer,NewCapacity,this->Length);
			this->Length=NewCapacity;
		}
	
		bool ManualCopy;
		tPointer NewArray=TAMFunc::Relocate(this->Pointer,this->Capacity,NewCapacity,ManualCopy);
		if(ManualCopy){
			PreviousArray.Pointer=this->Pointer;
			PreviousArray.Length=this->Length;
			PreviousArray.Capacity=this->Capacity;
			this->Length=0;
		}
		this->Pointer=NewArray;
		this->Capacity=NewCapacity;
		return PreviousArray;
	}

	void Clear(void)noexcept(noexcept(TAMFunc::Destruct(0,0,0))){
		if(this->Length!=0){
			TAMFunc::Destruct(this->Pointer,0,this->Length);
			this->Length=0;
		}
	}

	void GrowCapacityTo(uIntn NewCapacity)noexcept(noexcept(TAMFunc::MoveConstruct(0,0,0,0,0))){
		cnLib_ASSERT(NewCapacity>this->Capacity);
		NewCapacity=TAMFunc::RoundUpCapacity(NewCapacity);
		if(this->Capacity==0){
			this->Capacity=NewCapacity;
			this->Pointer=TAMFunc::Allocate(NewCapacity);
			return;
		}

		// grow
		bool ManualCopy;
		tPointer NewArray=TAMFunc::Relocate(this->Pointer,this->Capacity,NewCapacity,ManualCopy);
		if(ManualCopy){
			TAMFunc::MoveConstruct(NewArray,0,this->Pointer,0,this->Length);
			TAMFunc::Deallocate(this->Pointer,this->Capacity);
		}
		this->Pointer=NewArray;
		this->Capacity=NewCapacity;
	}

	void Truncate(uIntn RemoveLength)noexcept(noexcept(TAMFunc::Destruct(0,0,0))){
		if(RemoveLength>=this->Length){
			// delete all
			Clear();
			return;
		}
		if(RemoveLength!=0){
			// remove tail
			uIntn OldLength=this->Length;
			this->Length=OldLength-RemoveLength;
			TAMFunc::Destruct(this->Pointer,this->Length,OldLength);
		}
	}
	void Remove(uIntn Index,uIntn RemoveLength)noexcept(noexcept(TAMFunc::MoveInArray(0,0,0,0)) && noexcept(TAMFunc::Destruct(0,0,0))){
		if(RemoveLength==0)
			return;
		uIntn CopySrc=Index+RemoveLength;
		if(CopySrc>=this->Length){
			// remove tail
			if(this->Length>Index){
				TAMFunc::Destruct(this->Pointer,Index,this->Length);
				this->Length=Index;
			}
			return;
		}
		// remove at index
		uIntn CopyLength=this->Length-CopySrc;
		uIntn OldLength=this->Length;
		this->Length=OldLength-RemoveLength;
		if(CopyLength!=0){
			TAMFunc::MoveInArray(this->Pointer,Index,CopySrc,CopyLength);
		}
		// destruct unused tail
		TAMFunc::Destruct(this->Pointer,this->Length,OldLength);
	}
	
	void SetLength(uIntn NewLength)noexcept(noexcept(TAMFunc::Construct(0,0,0)) && noexcept(TAMFunc::Destruct(0,0,0))){
		if(NewLength==this->Length)
			return;
		if(NewLength<this->Length){
			// truncate
			TAMFunc::Destruct(this->Pointer,NewLength,this->Length);
		}
		else{
			// append
			if(NewLength>this->Capacity){
				// grow
				GrowCapacityTo(NewLength);
			}
			// append
			TAMFunc::Construct(this->Pointer,this->Length,NewLength);
		}
		this->Length=NewLength;
	}

	void SetCapacity(uIntn NewCapacity)noexcept(noexcept(TAMFunc::Construct(0,0,0)) && noexcept(TAMFunc::Destruct(0,0,0)) && noexcept(TAMFunc::MoveConstruct(0,0,0,0,0))){
		if(this->Capacity==NewCapacity)
			return;
		if(NewCapacity>this->Capacity){
			return GrowCapacityTo(NewCapacity);
		}
		if(NewCapacity==0){
			// deallocate

			if(this->Length!=0){
				TAMFunc::Destruct(this->Pointer,0,this->Length);
			}
			TAMFunc::Deallocate(this->Pointer,this->Capacity);
			Reset();
			return;
		}
		// shrink
		if(this->Length>NewCapacity){
			TAMFunc::Destruct(this->Pointer,NewCapacity,this->Length);
			this->Length=NewCapacity;
		}
		bool ManualCopy;
		tPointer NewArray=TAMFunc::Relocate(this->Pointer,this->Capacity,NewCapacity,ManualCopy);
		if(ManualCopy){
			TAMFunc::MoveConstruct(NewArray,0,this->Pointer,0,this->Length);
			TAMFunc::Destruct(this->Pointer,0,this->Length);
			TAMFunc::Deallocate(this->Pointer,this->Capacity);
		}
		this->Pointer=NewArray;
		this->Capacity=NewCapacity;
	}

	void ReplaceWithUninitialized(uIntn Index,uIntn ReplaceLength,uIntn ReplacementLength)noexcept(noexcept(TAMFunc::Destruct(0,0,0)) && noexcept(TAMFunc::MoveConstruct(0,0,0,0,0))){
		cnLib_ASSERT(Index<=this->Length);
		uIntn TailSrc=Index+ReplaceLength;
		uIntn TailDest=Index+ReplacementLength;
		if(TailSrc>=this->Length){
			//				|xxxxxxxxxxxxxxxxxxx|
			//	Array		Index	NewLength	CurLength	NewLength
			//						TailDest	TailSrc		TailDest
			// replace tail
			if(Index<this->Length){
				// destruct replaced
				TAMFunc::Destruct(this->Pointer,Index,this->Length);
				this->Length=Index;
			}
			// grow capacity
			if(this->Capacity<TailDest){
				GrowCapacityTo(TailDest);
			}
			this->Length=TailDest;
			return;
		}
		// destruct replaced
		uIntn TailLength=this->Length-TailSrc;
		uIntn NewLength=TailDest+TailLength;
		if(NewLength>this->Capacity){
			// grow
			uIntn NewCapacity=TAMFunc::RoundUpCapacity(NewLength);

			bool ManualCopy;
			tPointer NewArray=TAMFunc::Relocate(this->Pointer,this->Capacity,NewCapacity,ManualCopy);
			if(ManualCopy){
				// move array as replaced layout to new array
				TAMFunc::MoveConstruct(NewArray,0,this->Pointer,0,Index);
				TAMFunc::MoveConstruct(NewArray,TailDest,this->Pointer,TailSrc,TailLength);

				// delete old array
				TAMFunc::Destruct(this->Pointer,0,this->Length);
				TAMFunc::Deallocate(this->Pointer,this->Capacity);

				this->Length=NewLength;
				this->Pointer=NewArray;
				this->Capacity=NewCapacity;
				return;
			}
			this->Pointer=NewArray;
			this->Capacity=NewCapacity;
		}
		// replace in array
		if(TailSrc==TailDest){
			TAMFunc::Destruct(this->Pointer,Index,TailDest);
			return;
		}
		if(TailSrc<TailDest){
			//						|=======================| --------->|
			//				|xxxxxxxxxxxxxxxxxxx|===========++++++++++++|
			//	Array		Index	TailSrc		TailDest	OldLength	NewLength
			TAMFunc::MoveHeadConstructTailInArray(this->Pointer,this->Length,TailDest,TailLength);
			// destruct replaced item
			TAMFunc::Destruct(this->Pointer,Index,TailDest);
		}
		else{	// TailSrc>TailDest
			//						|<---------	|=======================|
			//				|xxxxxxx|=======================|xxxxxxxxxxx|
			//	Array		Index	TailDest	TailSrc		NewLength	OldLength

			TAMFunc::MoveInArray(this->Pointer,TailDest,TailSrc,TailLength);
			// destruct replaced item
			TAMFunc::Destruct(this->Pointer,Index,TailDest);
			// destruct tail
			TAMFunc::Destruct(this->Pointer,NewLength,this->Length);
		}
		this->Length=NewLength;
	}

	void SetRange(uIntn Index,uIntn CopyLength,tConstPointer Src)noexcept(noexcept(TAMFunc::Destruct(0,0,0)) && noexcept(TAMFunc::CopyHeadConstructTail(0,0,0,0,0))){
		if(Index>this->Length){
			return;
		}

		ReplaceWithUninitialized(Index,CopyLength,CopyLength);

		TAMFunc::CopyConstruct(this->Pointer,Index,Src,0,CopyLength);
	}
	void Copy(tConstPointer Src,uIntn SrcLength)noexcept(noexcept(TAMFunc::Destruct(0,0,0)) && noexcept(TAMFunc::CopyHeadConstructTail(0,0,0,0,0,0))){
		if(SrcLength==0){
			this->Length=0;
			return;
		}
		if(this->Length==0){
			if(SrcLength>this->Capacity){
				GrowCapacityTo(SrcLength);
			}
			TAMFunc::CopyConstruct(this->Pointer,0,Src,0,SrcLength);
		}
		else if(SrcLength<=this->Length){
			TAMFunc::Copy(this->Pointer,0,Src,0,SrcLength);
		}
		else if(SrcLength<=this->Capacity){
			TAMFunc::CopyHeadConstructTail(this->Pointer,0,Src,0,this->Length,SrcLength);
		}
		else{	// SrcLength>this->Capacity
			uIntn NewCapacity=TAMFunc::RoundUpCapacity(SrcLength);
			bool ManualCopy;
			tPointer NewArray=TAMFunc::Relocate(this->Pointer,this->Capacity,NewCapacity,ManualCopy);
			if(ManualCopy){
				TAMFunc::Destruct(this->Pointer,0,this->Capacity);
				TAMFunc::Deallocate(this->Pointer,this->Capacity);

				TAMFunc::CopyConstruct(NewArray,0,Src,0,SrcLength);
			}
			else{
				TAMFunc::CopyHeadConstructTail(NewArray,0,Src,0,this->Length,SrcLength);
			}
			this->Pointer=NewArray;
			this->Capacity=NewCapacity;
		}
		this->Length=SrcLength;
	}

	void MoveRange(uIntn Index,uIntn CopyLength,tPointer Src,uIntn SrcIndex)noexcept(noexcept(ReplaceWithUninitialized(0,0,0))){
		if(Index>this->Length){
			return;
		}

		ReplaceWithUninitialized(Index,CopyLength,CopyLength);

		TAMFunc::MoveConstruct(this->Pointer,Index,Src,SrcIndex,CopyLength);
	}
	void Move(tPointer Src,uIntn SrcLength)noexcept(noexcept(ReplaceWithUninitialized(0,0,0))){
		ReplaceWithUninitialized(0,SrcLength,SrcLength);

		TAMFunc::MoveConstruct(this->Pointer,0,Src,0,SrcLength);
	}


	void Replace(uIntn Index,uIntn ReplaceLength,uIntn ReplacementLength)noexcept(noexcept(TAMFunc::Construct(0,0,0)) && noexcept(ReplaceWithUninitialized(0,0,0))){
		ReplaceWithUninitialized(Index,ReplaceLength,ReplacementLength);

		TAMFunc::Construct(this->Pointer,Index,Index+ReplacementLength);
	}
};


template<class TAllocationOperator,class TElement>
class cArrayBuffer
{
public:
	typedef TAllocationOperator tAllocationOperator;
	typedef TElement tElement;
	typedef cArrayAllocation<TAllocationOperator,tElement> tArrayAllocation;
	typedef TElement *tPointer;
	typedef TElement const *tConstPointer;
	typedef cArray<TElement> tArray;
	typedef cArray<const TElement> tConstArray;
	typedef cArrayMemoryFunction<TAllocationOperator,TElement> TAMFunc;

#if !defined(cnLibrary_CPPEXCLUDE_NOEXCEPT) && cnLibrary_CPPFEATURE_STATIC_ASSERT >= 200410L

	static_assert(cnVar::TIsSame<tElement,void>::Value || cnVar::TIsDefaultConstructNoexcept<tElement>::Value,"noexcept default constructor required");
	static_assert(cnVar::TIsSame<tElement,void>::Value || cnVar::TIsDestructNoexcept<tElement>::Value,"noexcept destructor required");
	static_assert(cnVar::TIsSame<tElement,void>::Value || cnVar::TIsCopyConstructNoexcept<tElement>::Value,"noexcept copy constructor required");
	static_assert(cnVar::TIsSame<tElement,void>::Value || cnVar::TIsMoveConstructNoexcept<tElement>::Value,"noexcept move constructor required");
	static_assert(cnVar::TIsSame<tElement,void>::Value || cnVar::TIsCopyAssignNoexcept<tElement>::Value,"noexcept copy assignment required");

#endif	// !defined(cnLibrary_CPPEXCLUDE_NOEXCEPT) && cnLibrary_CPPFEATURE_STATIC_ASSERT >= 200410L

	cArrayBuffer()noexcept(true){}
    ~cArrayBuffer()noexcept(noexcept(TAMFunc::Destruct(0,0,0))){}
	cArrayBuffer(tConstPointer Data,uIntn Size,uIntn InitialCapacity=0)noexcept(noexcept(TAMFunc::CopyConstruct(0,0,0,0,0)))
		:fArray(Data,Size,InitialCapacity){}
	cArrayBuffer(const cArrayBuffer &Src)noexcept(noexcept(TAMFunc::CopyConstruct(0,0,0,0,0)))
		:fArray(Src.fArray){}
	cArrayBuffer(const tConstArray &Src)noexcept(noexcept(TAMFunc::CopyConstruct(0,0,0,0,0)))
		:fArray(Src){}

#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L
	cArrayBuffer(cArrayBuffer &&Src)noexcept(true):fArray(static_cast<tArrayAllocation&&>(Src.fArray)){}
	cArrayBuffer(tArrayAllocation &&Src)noexcept(true):fArray(static_cast<tArrayAllocation&&>(Src)){}
#endif	// cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

    cArrayBuffer& operator =(const cArrayBuffer &Src)noexcept(noexcept(fArray.Copy(0,0))){
		if(this==&Src)
			return *this;
		fArray=Src.fArray;
		return *this;
	}
    cArrayBuffer& operator =(const tConstArray &Memory)noexcept(noexcept(fArray.Copy(0,0))){
		if(ArrayCast<tConstArray>(&fArray)!=&Memory){
			fArray=Memory;
		}
		return *this;
	}

#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L
	cArrayBuffer& operator =(cArrayBuffer &&Src)noexcept(true){
		fArray=static_cast<tArrayAllocation&&>(Src.fArray);
		return *this;
	}
	cArrayBuffer& operator =(tArrayAllocation &&Src)noexcept(true){
		fArray=static_cast<tArrayAllocation&&>(Src);
		return *this;
	}
#endif	// cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

	cArrayStorage<TAllocationOperator,TElement> &Storage(void)noexcept(true){	return fArray;	}
	const cArrayStorage<TAllocationOperator,TElement> &Storage(void)const noexcept(true){	return fArray;	}

	// Swap
    //	swap out array storage
    cArrayStorage<TAllocationOperator,TElement> Swap(void)noexcept(true){
		return cnLib_UREFCAST(tArrayAllocation)(fArray);
	}
	// Swap
    //	swap array storage
    cArrayStorage<TAllocationOperator,TElement> Swap(tArrayAllocation cnLib_UREF Src)noexcept(true){
		cArrayStorage<TAllocationOperator,TElement> Swapped(cnLib_UREFCAST(tArrayAllocation)(fArray));
		fArray=cnLib_UREFCAST(tArrayAllocation)(Src);
		return Swapped;
	}

// Info

	operator tPointer ()const noexcept(true){	return fArray.Pointer;	}
	const tArrayAllocation* operator ->()const noexcept(true){	return &fArray;	}
	tPointer operator [] (uIntn Offset)const noexcept(true){	return ArrayOffsetPointer(fArray.Pointer,Offset);	}

    // GetMemory
    // return pointer to the whole buffer
    tPointer GetPointer(void)const noexcept(true){				return fArray.Pointer;	}
    // GetMemory
    //  Get pointer to memory[Offset]
    tPointer GetPointer(uIntn Offset)const noexcept(true){		return ArrayOffsetPointer(fArray.Pointer,Offset);		}
    // GetSize
    //  total bytes of buffer used
    uIntn GetSize(void)const noexcept(true){				return fArray.Length;	}
	// SetSize
	void SetSize(uIntn NewSize)noexcept(noexcept(fArray.SetLength(0))){			return fArray.SetLength(NewSize);		}
    // GetCapacity
    //  total bytes of buffer capacity
    uIntn GetCapacity(void)const noexcept(true){			return fArray.Capacity;	}
	// SetCapacity
	void SetCapacity(uIntn NewCapacity)noexcept(noexcept(fArray.SetCapacity(0))){	return fArray.SetCapacity(NewCapacity);	}

	// Edit
	void Copy(tConstPointer Data,uIntn Size)noexcept(noexcept(fArray.Copy(Data,Size)))
	{	return fArray.Copy(Data,Size);	}

	void Replace(uIntn Offset,uIntn ReplaceSize,uIntn ReplacementSize)noexcept(noexcept(fArray.Replace(Offset,ReplaceSize,ReplacementSize)))
	{	return fArray.Replace(Offset,ReplaceSize,ReplacementSize);	}

	void Append(uIntn Size)noexcept(noexcept(fArray.SetLength(fArray.Length+Size)))
	{	return fArray.SetLength(fArray.Length+Size);	}
	void Append(tConstPointer Data,uIntn Size)noexcept(noexcept(fArray.SetLength(0)) && noexcept(TAMFunc::Copy(0,0,0,0,0))){
		uIntn WriteIndex=fArray.Length;
		fArray.SetLength(fArray.Length+Size);
		TAMFunc::Copy(fArray.Pointer,WriteIndex,Data,0,fArray.Length-WriteIndex);
	}
	template<class T>
	typename cnVar::TSelect<0,void
#if cnLibrary_CPPFEATURE_DECLTYPE >= 200707L
		, decltype(cnVar::DeclVal<typename TArrayElement<TElement>::Type&>()=cnVar::DeclVal<const T&>())
#endif // cnLibrary_CPPFEATURE_DECLTYPE >= 200707L
	>::Type AppendMake(const T &Data)noexcept(noexcept(fArray.SetLength(0))){
		uIntn WriteIndex=fArray.Length;
		fArray.SetLength(fArray.Length+1);
		static_cast<typename TArrayElement<TElement>::Type*>(fArray.Pointer)[WriteIndex]=Data;
	}
	void Truncate(uIntn Size)noexcept(noexcept(fArray.SetLength(0)) && noexcept(TAMFunc::Destruct(0,0,0))){
		if(Size>fArray.Length)
			fArray.Clear();
		else
			fArray.SetLength(fArray.Length-Size);
	}

	void Remove(uIntn Index,uIntn Size)noexcept(noexcept(fArray.Remove(0,0))){
		return fArray.Remove(Index,Size);
	}

	// Compact
    //  Reduce the buffer size to the number of bytes used
	void Compact(void)noexcept(noexcept(fArray.SetCapacity(fArray.Length)))
	{	return fArray.SetCapacity(fArray.Length);	}

	// Clear
    //  Clear the buffer
	void Clear(void)noexcept(noexcept(fArray.Clear()))
	{	return fArray.Clear();	}

protected:
	cArrayStorage<TAllocationOperator,TElement> fArray;
};


#if cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L



template<class...VTArrayAllocation>
struct cMultiArrayAllocation : cMultiArray<typename VTArrayAllocation::tElement...>
{
	typedef cMultiArray<typename VTArrayAllocation::tElement...> tMultiArray;
	uIntn Capacity;
};

template<class TVarPackAccessor,class TConstVarPackAccessor,class...VTArrayAllocation>
struct cMultiArrayMemoryFunction
{
	typedef typename cnVar::TSelect<TVarPackAccessor::AccessIndex,VTArrayAllocation...>::Type tArrayAllocation;
	typedef typename tArrayAllocation::tElement tElement;
	typedef cArrayMemoryFunction<typename tArrayAllocation::tAllocationOperator,tElement> TAMFunc;

	static uIntn RoundUpCapacity(uIntn Value)noexcept(true){
		Value=TAMFunc::RoundUpCapacity(Value);
		return cMultiArrayMemoryFunction<typename TVarPackAccessor::tNext,typename TConstVarPackAccessor::tNext,VTArrayAllocation...>::RoundUpCapacity(Value);
	}


	static void Allocate(const TVarPackAccessor &Accessor,uIntn AllocateLength)noexcept(true){
		*Accessor=TAMFunc::Allocate(AllocateLength);
		return cMultiArrayMemoryFunction<typename TVarPackAccessor::tNext,typename TConstVarPackAccessor::tNext,VTArrayAllocation...>::Allocate(+Accessor,AllocateLength);
	}
	static void Deallocate(const TVarPackAccessor &Accessor,uIntn Capacity)noexcept(true){
		TAMFunc::Deallocate(*Accessor,Capacity);

		return cMultiArrayMemoryFunction<typename TVarPackAccessor::tNext,typename TConstVarPackAccessor::tNext,VTArrayAllocation...>::Deallocate(+Accessor,Capacity);
	}

	static void Relocate(const TVarPackAccessor &RelocatedAccessor,const TVarPackAccessor &Accessor,uIntn Capacity,uIntn NewLength,bool *ManualCopy)noexcept(true){
		*RelocatedAccessor=TAMFunc::Relocate(*Accessor,Capacity,NewLength,*ManualCopy);
		return cMultiArrayMemoryFunction<typename TVarPackAccessor::tNext,typename TConstVarPackAccessor::tNext,VTArrayAllocation...>::Relocate(+RelocatedAccessor,+Accessor,Capacity,NewLength,ManualCopy+1);
	}

	static void ResetPointer(const TVarPackAccessor &Accessor)noexcept(true){
		*Accessor=nullptr;
		cMultiArrayMemoryFunction<typename TVarPackAccessor::tNext,typename TConstVarPackAccessor::tNext,VTArrayAllocation...>::ResetPointer(+Accessor);
	}


	static void Construct(const TVarPackAccessor &Accessor,uIntn Index,uIntn EndIndex)noexcept(true){
		TAMFunc::Construct(*Accessor,Index,EndIndex);
		return cMultiArrayMemoryFunction<typename TVarPackAccessor::tNext,typename TConstVarPackAccessor::tNext,VTArrayAllocation...>::Construct(+Accessor,Index,EndIndex);
	}

	static void Destruct(const TVarPackAccessor &Accessor,uIntn Index,uIntn EndIndex)noexcept(true){
		TAMFunc::Destruct(*Accessor,Index,EndIndex);
		return cMultiArrayMemoryFunction<typename TVarPackAccessor::tNext,typename TConstVarPackAccessor::tNext,VTArrayAllocation...>::Destruct(+Accessor,Index,EndIndex);
	}

	static void CopyConstruct(const TVarPackAccessor &DestAccessor,uIntn Index,const TConstVarPackAccessor &SrcAccessor,uIntn SrcIndex,uIntn Length)noexcept(true){
		if(*SrcAccessor!=nullptr){
			TAMFunc::CopyConstruct(*DestAccessor,Index,*SrcAccessor,SrcIndex,Length);
		}
		else{
			TAMFunc::Construct(*DestAccessor,Index,Index+Length);
		}
		return cMultiArrayMemoryFunction<typename TVarPackAccessor::tNext,typename TConstVarPackAccessor::tNext,VTArrayAllocation...>::CopyConstruct(+DestAccessor,Index,+SrcAccessor,SrcIndex,Length);
	}

	static void MoveConstruct(const TVarPackAccessor &DestAccessor,uIntn Index,const TVarPackAccessor &SrcAccessor,uIntn SrcIndex,uIntn Length)noexcept(true){
		if(*SrcAccessor!=nullptr){
			TAMFunc::MoveConstruct(*DestAccessor,Index,*SrcAccessor,SrcIndex,Length);
		}
		else{
			TAMFunc::Construct(*DestAccessor,Index,Index+Length);
		}
		return cMultiArrayMemoryFunction<typename TVarPackAccessor::tNext,typename TConstVarPackAccessor::tNext,VTArrayAllocation...>::MoveConstruct(+DestAccessor,Index,+SrcAccessor,SrcIndex,Length);
	}

	static void Copy(const TVarPackAccessor &DestAccessor,uIntn Index,uIntn Length,const TConstVarPackAccessor &SrcAccessor,uIntn SrcIndex)noexcept(true){
		if(*DestAccessor!=nullptr && *SrcAccessor!=nullptr){
			TAMFunc::Copy(*DestAccessor,Index,*SrcAccessor,SrcIndex,Length);
		}
		return cMultiArrayMemoryFunction<typename TVarPackAccessor::tNext,typename TConstVarPackAccessor::tNext,VTArrayAllocation...>::Copy(+DestAccessor,Index,Length,+SrcAccessor,SrcIndex);
	}

	static void CopyHeadConstructTail(const TVarPackAccessor &DestAccessor,uIntn ArrayIndex,const TConstVarPackAccessor &SrcAccessor,uIntn SrcIndex,uIntn HeadLength,uIntn TotalLength)noexcept(true){
		if(*SrcAccessor!=nullptr){
			TAMFunc::CopyHeadConstructTail(*DestAccessor,ArrayIndex,*SrcAccessor,SrcIndex,HeadLength,TotalLength);
		}
		return cMultiArrayMemoryFunction<typename TVarPackAccessor::tNext,typename TConstVarPackAccessor::tNext,VTArrayAllocation...>::CopyHeadConstructTail(+DestAccessor,ArrayIndex,+SrcAccessor,SrcIndex,HeadLength,TotalLength);
	}

	static void MoveInArray(const TVarPackAccessor &Accessor,uIntn DestIndex,uIntn SrcIndex,uIntn Length)noexcept(true){
		TAMFunc::MoveInArray(*Accessor,DestIndex,SrcIndex,Length);
		return cMultiArrayMemoryFunction<typename TVarPackAccessor::tNext,typename TConstVarPackAccessor::tNext,VTArrayAllocation...>::MoveInArray(+Accessor,DestIndex,SrcIndex,Length);
	}


	// Masked functions

	static void MaskByNullptr(bool *Mask,const TVarPackAccessor &Accessor)noexcept(true){
		*Mask=(*Accessor==nullptr);
		return cMultiArrayMemoryFunction<typename TVarPackAccessor::tNext,typename TConstVarPackAccessor::tNext,VTArrayAllocation...>::MaskByNullptr(+Accessor,Mask+1);
	}
	static void MaskByNotNullptr(bool *Mask,const TVarPackAccessor &Accessor)noexcept(true){
		*Mask=(*Accessor!=nullptr);
		return cMultiArrayMemoryFunction<typename TVarPackAccessor::tNext,typename TConstVarPackAccessor::tNext,VTArrayAllocation...>::MaskByNotNullptr(+Accessor,Mask+1);
	}
	static bool MaskIsAnySet(const bool *Mask)noexcept(true){
		for(uIntn i=0;i<sizeof...(VTArrayAllocation);i++){
			if(Mask[i])
				return true;
		}
		return false;
	}
	//static bool MaskIsAnyClear(const bool *Mask)noexcept(true){
	//	for(uIntn i=0;i<sizeof...(VTArrayAllocation);i++){
	//		if(Mask[i]==false)
	//			return true;
	//	}
	//	return false;
	//}

	static void MaskAllocate(const bool *Mask,const TVarPackAccessor &Accessor,uIntn AllocateLength)noexcept(true){
		if(*Mask){
			*Accessor=TAMFunc::Allocate(AllocateLength);
		}
		return cMultiArrayMemoryFunction<typename TVarPackAccessor::tNext,typename TConstVarPackAccessor::tNext,VTArrayAllocation...>::MaskAllocate(Mask+1,+Accessor,AllocateLength);
	}
	static void MaskDeallocate(const bool *Mask,const TVarPackAccessor &Accessor,uIntn Capacity)noexcept(true){
		if(*Mask){
			TAMFunc::Deallocate(*Accessor,Capacity);
		}

		return cMultiArrayMemoryFunction<typename TVarPackAccessor::tNext,typename TConstVarPackAccessor::tNext,VTArrayAllocation...>::MaskDeallocate(Mask+1,+Accessor,Capacity);
	}

	static void MaskConstruct(const bool *Mask,const TVarPackAccessor &Accessor,uIntn Index,uIntn EndIndex)noexcept(true){
		if(*Mask){
			TAMFunc::Construct(*Accessor,Index,EndIndex);
		}
		return cMultiArrayMemoryFunction<typename TVarPackAccessor::tNext,typename TConstVarPackAccessor::tNext,VTArrayAllocation...>::MaskConstruct(Mask+1,+Accessor,Index,EndIndex);
	}

	static void MaskDestruct(const bool *Mask,const TVarPackAccessor &Accessor,uIntn Index,uIntn EndIndex)noexcept(true){
		if(*Mask){
			TAMFunc::Destruct(*Accessor,Index,EndIndex);
		}
		return cMultiArrayMemoryFunction<typename TVarPackAccessor::tNext,typename TConstVarPackAccessor::tNext,VTArrayAllocation...>::MaskDestruct(Mask+1,+Accessor,Index,EndIndex);
	}

	static void MaskCopyConstruct(const bool *Mask,const TVarPackAccessor &DestAccessor,uIntn Index,const TConstVarPackAccessor &SrcAccessor,uIntn SrcIndex,uIntn Length)noexcept(true){
		if(*Mask){
			if(*SrcAccessor!=nullptr){
				TAMFunc::CopyConstruct(*DestAccessor,Index,*SrcAccessor,SrcIndex,Length);
			}
			else{
				TAMFunc::Construct(*DestAccessor,Index,Index+Length);
			}
		}
		return cMultiArrayMemoryFunction<typename TVarPackAccessor::tNext,typename TConstVarPackAccessor::tNext,VTArrayAllocation...>::MaskCopyConstruct(Mask+1,+DestAccessor,Index,+SrcAccessor,SrcIndex,Length);
	}

	static void MaskMoveConstruct(const bool *Mask,const TVarPackAccessor &DestAccessor,uIntn Index,const TVarPackAccessor &SrcAccessor,uIntn SrcIndex,uIntn Length)noexcept(true){
		if(*Mask){
			if(*SrcAccessor!=nullptr){
				TAMFunc::MoveConstruct(*DestAccessor,Index,*SrcAccessor,SrcIndex,Length);
			}
			else{
				TAMFunc::Construct(*DestAccessor,Index,Index+Length);
			}
		}
		return cMultiArrayMemoryFunction<typename TVarPackAccessor::tNext,typename TConstVarPackAccessor::tNext,VTArrayAllocation...>::MaskMoveConstruct(Mask+1,+DestAccessor,Index,+SrcAccessor,SrcIndex,Length);
	}

	static void MaskCopyHeadConstructTail(const bool *Mask,const TVarPackAccessor &DestAccessor,uIntn ArrayIndex,const TConstVarPackAccessor &SrcAccessor,uIntn SrcIndex,uIntn HeadLength,uIntn TotalLength)noexcept(true){
		if(*Mask && *SrcAccessor!=nullptr){
			TAMFunc::CopyHeadConstructTail(*DestAccessor,ArrayIndex,*SrcAccessor,SrcIndex,HeadLength,TotalLength);
		}
		return cMultiArrayMemoryFunction<typename TVarPackAccessor::tNext,typename TConstVarPackAccessor::tNext,VTArrayAllocation...>::MaskCopyHeadConstructTail(Mask+1,+DestAccessor,ArrayIndex,+SrcAccessor,SrcIndex,HeadLength,TotalLength);
	}

	static void MaskMoveHeadConstructTailInArray(const bool *Mask,const TVarPackAccessor &Accessor,uIntn ArrayLength,uIntn DestIndex,uIntn MoveLength)noexcept(true){
		if(*Mask){
			TAMFunc::MoveHeadConstructTailInArray(*Accessor,ArrayLength,DestIndex,MoveLength);
		}
		return cMultiArrayMemoryFunction<typename TVarPackAccessor::tNext,typename TConstVarPackAccessor::tNext,VTArrayAllocation...>::MaskMoveHeadConstructTailInArray(Mask+1,+Accessor,ArrayLength,DestIndex,MoveLength);
	}

	static void MaskMoveInArray(const bool *Mask,const TVarPackAccessor &Accessor,uIntn DestIndex,uIntn SrcIndex,uIntn Length)noexcept(true){
		if(*Mask){
			TAMFunc::MoveInArray(*Accessor,DestIndex,SrcIndex,Length);
		}
		return cMultiArrayMemoryFunction<typename TVarPackAccessor::tNext,typename TConstVarPackAccessor::tNext,VTArrayAllocation...>::MaskMoveInArray(Mask+1,+Accessor,DestIndex,SrcIndex,Length);
	}

	// helper function

	static void ConstructElement(const TVarPackAccessor &Accessor,uIntn Index)noexcept(true){
		cnClass::ManualConstruct((*Accessor)[Index]);
		return cMultiArrayMemoryFunction<typename TVarPackAccessor::tNext,typename TConstVarPackAccessor::tNext,VTArrayAllocation...>::ConstructElement(+Accessor,Index);
	}

	template<class TArg,class...TArgs>
	static void ConstructElement(const TVarPackAccessor &Accessor,uIntn Index,TArg cnLib_UREF Arg,TArgs cnLib_UREF...Args)noexcept(true){
		cnClass::ManualConstruct((*Accessor)[Index],cnLib_UREFCAST(TArg)(Arg));
		return cMultiArrayMemoryFunction<typename TVarPackAccessor::tNext,typename TConstVarPackAccessor::tNext,VTArrayAllocation...>::ConstructElement(+Accessor,Index,cnLib_UREFCAST(TArgs)(Args)...);
	}

};

template<class...VTArrayAllocation>
struct cMultiArrayMemoryFunction<cnVar::cVarPackNoAccess,cnVar::cVarPackNoAccess,VTArrayAllocation...>
{
	static uIntn RoundUpCapacity(uIntn Value){	return Value;	}


	static void Allocate(cnVar::cVarPackNoAccess,uIntn)noexcept(true){}
	static void Deallocate(cnVar::cVarPackNoAccess,uIntn)noexcept(true){}
	static void Relocate(cnVar::cVarPackNoAccess,cnVar::cVarPackNoAccess,uIntn,uIntn,bool*)noexcept(true){}

	static void ResetPointer(cnVar::cVarPackNoAccess)noexcept(true){}

	static void Construct(cnVar::cVarPackNoAccess,uIntn,uIntn)noexcept(true){}
	static void Destruct(cnVar::cVarPackNoAccess,uIntn,uIntn)noexcept(true){}

	static void CopyConstruct(cnVar::cVarPackNoAccess,uIntn,cnVar::cVarPackNoAccess,uIntn,uIntn)noexcept(true){}
	static void MoveConstruct(cnVar::cVarPackNoAccess,uIntn,cnVar::cVarPackNoAccess,uIntn,uIntn)noexcept(true){}

	static void Copy(cnVar::cVarPackNoAccess,uIntn,uIntn,cnVar::cVarPackNoAccess,uIntn)noexcept(true){}
	static void CopyHeadConstructTail(cnVar::cVarPackNoAccess,uIntn,cnVar::cVarPackNoAccess,uIntn,uIntn,uIntn)noexcept(true){}
	static void MoveInArray(cnVar::cVarPackNoAccess,uIntn,uIntn,uIntn)noexcept(true){}

	// Masked functions

	static void MaskByNullptr(bool*,cnVar::cVarPackNoAccess)noexcept(true){}
	static void MaskByNotNullptr(bool*,cnVar::cVarPackNoAccess)noexcept(true){}

	static void MaskAllocate(const bool*,cnVar::cVarPackNoAccess,uIntn)noexcept(true){}
	static void MaskDeallocate(const bool*,cnVar::cVarPackNoAccess,uIntn)noexcept(true){}

	static void MaskConstruct(const bool*,cnVar::cVarPackNoAccess,uIntn,uIntn)noexcept(true){}
	static void MaskDestruct(const bool*,cnVar::cVarPackNoAccess,uIntn,uIntn)noexcept(true){}

	static void MaskCopyConstruct(const bool*,cnVar::cVarPackNoAccess,uIntn,cnVar::cVarPackNoAccess,uIntn,uIntn)noexcept(true){}
	static void MaskMoveConstruct(const bool*,cnVar::cVarPackNoAccess,uIntn,cnVar::cVarPackNoAccess,uIntn,uIntn)noexcept(true){}

	static void MaskCopyHeadConstructTail(const bool*,cnVar::cVarPackNoAccess,uIntn,cnVar::cVarPackNoAccess,uIntn,uIntn,uIntn)noexcept(true){}
	static void MaskMoveHeadConstructTailInArray(const bool*,cnVar::cVarPackNoAccess,uIntn,uIntn,uIntn)noexcept(true){}
	static void MaskMoveInArray(const bool*,cnVar::cVarPackNoAccess,uIntn,uIntn,uIntn)noexcept(true){}


	// helper function

	template<class...VT>
	static void ConstructElement(cnVar::cVarPackNoAccess,uIntn,VT...)noexcept(true){}

};



template<class...VTArrayAllocation>
struct cMultiArrayStorage
	: cMultiArrayAllocation<VTArrayAllocation...>
{
	typedef cnVar::cVarPack<typename VTArrayAllocation::tElement*...> tPointer;
	typedef cnVar::cVarPack<typename VTArrayAllocation::tElement const*...> tConstPointer;
	typedef cMultiArrayMemoryFunction<typename tPointer::tAllAccessor,typename tConstPointer::tAllConstAccessor,VTArrayAllocation...> TMAMFunc;
	typedef cMultiArray<typename VTArrayAllocation::tElement...> tArray;
	typedef cMultiArray<typename VTArrayAllocation::tElement const...> tConstArray;
	typedef cMultiArrayAllocation<VTArrayAllocation...> tMultiArrayAllocation;
	static cnLib_CONSTVAR uIntn PointerCount=sizeof...(VTArrayAllocation);

	void Reset(void)noexcept(true){
		TMAMFunc::ResetPointer(this->Pointer.All());
		this->Length=0;
		this->Capacity=0;
	}

	cMultiArrayStorage()noexcept(true){	Reset();	}
	~cMultiArrayStorage()noexcept(true){
		if(this->Length!=0)
			TMAMFunc::Destruct(this->Pointer.All(),0,this->Length);
		if(this->Capacity!=0)
			TMAMFunc::Deallocate(this->Pointer.All(),this->Capacity);
	}

	cMultiArrayStorage(const tPointer &InitialData,uIntn InitialLength,uIntn InitialCapacity=0)noexcept(true){
		if(InitialCapacity<InitialLength){
			InitialCapacity=InitialLength;
		}
		if(InitialCapacity==0){
			Reset();
			return;
		}
		// allocate
		this->Capacity=InitialCapacity;
		TMAMFunc::Allocate(this->Pointer.All(),InitialCapacity);
		// data
		this->Length=InitialLength;
		TMAMFunc::CopyConstruct(this->Pointer.All(),0,InitialData,0,this->Length);
	}
	
	cMultiArrayStorage(const cMultiArrayStorage &Src)noexcept(true){
		// allocate
		this->Capacity=Src.Length;
		TMAMFunc::Allocate(this->Pointer.All(),Src.Length);

		// data
		this->Length=Src.Length;
		TMAMFunc::CopyConstruct(this->Pointer.All(),0,Src.Pointer.All(),0,this->Length);
	}

#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L
	cMultiArrayStorage(cMultiArrayStorage &&Src)noexcept(true){
		this->Pointer=Src.Pointer;
		this->Length=Src.Length;
		this->Capacity=Src.Capacity;
		Src.Reset();
	}
#endif	// cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L


	cMultiArrayStorage(const tArray &Src)noexcept(true){
		Copy(Src.Pointer,0,Src.Length);
	}

	cMultiArrayStorage& operator =(const cMultiArrayStorage &Src)noexcept(true){
		if(this!=cnMemory::AddressOf(Src)){
			Copy(Src.Pointer,0,Src.Length);
		}
		return *this;
	}

	cMultiArrayStorage& operator =(const tArray &Src)noexcept(true){
		Copy(Src.Pointer,0,Src.Length);
		return *this;
	}

#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L
	
	cMultiArrayStorage(tMultiArrayAllocation &&Src)noexcept(true){
		this->Pointer=Src.Pointer;
		this->Length=Src.Length;
		this->Capacity=Src.Capacity;
		TMAMFunc::ResetPointer(Src.Pointer.All());
		Src.Length=0;
		Src.Capacity=0;
	}

	cMultiArrayStorage& operator =(cMultiArrayStorage &&Src)noexcept(true){
		return operator=(static_cast<tMultiArrayAllocation&&>(Src));
	}

	cMultiArrayStorage& operator =(tMultiArrayAllocation &&Src)noexcept(true){
		if(this->Capacity!=0){
			if(this->Length!=0){
				TMAMFunc::Destruct(this->Pointer.All(),0,this->Length);
			}
			TMAMFunc::Deallocate(this->Pointer.All(),this->Capacity);
		}
		this->Pointer=Src.Pointer;
		this->Length=Src.Length;
		this->Capacity=Src.Capacity;


		TMAMFunc::ResetPointer(Src.Pointer.All());
		Src.Capacity=0;
		Src.Length=0;
		return *this;
	}

#endif	// cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

	//cMultiArrayStorage Relocate(uIntn NewCapacity)noexcept(true){
	//		uIntn NewCapacity=TMAMFunc::RoundUpCapacity(NewLength);
	//		bool ManualCopy[TMAMFunc::PointerCount];
	//		TMAMFunc::Relocate(ReplacePointer,this->Pointer,this->Capacity,NewCapacity,ManualCopy);
	//		tPointer ManualCopyPointer=this->Pointer;
	//		if(!TMAMFunc::ResetPointerIfNot(ManualCopyPointer,ManualCopy)){
	//			// move array as replaced layout to new array
	//			TMAMFunc::MoveConstruct(ReplacePointer,0,ManualCopyPointer,0,Index);
	//			TMAMFunc::MoveConstruct(ReplacePointer,TailDest,ManualCopyPointer,TailSrc,TailLength);
	//
	//			// delete old array
	//			TMAMFunc::Destruct(ManualCopyPointer,0,this->Length);
	//			TMAMFunc::Deallocate(ManualCopyPointer,this->Capacity);
	//		}
	//		this->Pointer=ReplacePointer;
	//		this->Capacity=NewCapacity;
	//		cnLib_ASSERT(this->Capacity!=0);
	//		if(TMAMFunc::ResetPointerIf(ReplacePointer,ManualCopy)){
	//			// all array is manually copied as replaced layout
	//			this->Length=NewLength;
	//			return;
	//		}
	//		// replace remaining array in place
	//}

	void Clear(void)noexcept(true){
		if(this->Length!=0){
			TMAMFunc::Destruct(this->Pointer.All(),0,this->Length);
			this->Length=0;
		}
	}

	void GrowCapacityTo(uIntn NewCapacity)noexcept(true){
		cnLib_ASSERT(NewCapacity>this->Capacity);
		NewCapacity=TMAMFunc::RoundUpCapacity(NewCapacity);
		if(this->Capacity==0){
			this->Capacity=NewCapacity;
			TMAMFunc::Allocate(this->Pointer.All(),NewCapacity);
			return;
		}
		// grow
		bool ManualCopy[PointerCount];
		tPointer NewArray;
		TMAMFunc::Relocate(NewArray,this->Pointer.All(),this->Capacity,NewCapacity,ManualCopy);

		if(TMAMFunc::MaskIsAnySet(ManualCopy)){
			// need manual copy
			TMAMFunc::MaskMoveConstruct(ManualCopy,NewArray.All(),0,this->Pointer.All(),0,this->Length);
			TMAMFunc::MaskDestruct(ManualCopy,this->Pointer.All(),0,this->Length);
			TMAMFunc::MaskDeallocate(ManualCopy,this->Pointer.All(),this->Capacity);
		}

		this->Pointer=NewArray;
		this->Capacity=NewCapacity;
	}

	void Truncate(uIntn RemoveLength)noexcept(true){
		if(RemoveLength>=this->Length){
			// delete all
			Clear();
			return;
		}
		if(RemoveLength!=0){
			// remove tail
			uIntn OldLength=this->Length;
			this->Length=OldLength-RemoveLength;
			TMAMFunc::Destruct(this->Pointer.All(),this->Length,OldLength);
		}
	}
	void Remove(uIntn Index,uIntn RemoveLength)noexcept(true){
		if(RemoveLength==0)
			return;
		uIntn CopySrc=Index+RemoveLength;
		if(CopySrc>=this->Length){
			// remove tail
			if(this->Length>Index){
				TMAMFunc::Destruct(this->Pointer.All(),Index,this->Length);
				this->Length=Index;
			}
			return;
		}
		// remove at index
		uIntn CopyLength=this->Length-CopySrc;
		uIntn OldLength=this->Length;
		this->Length=OldLength-RemoveLength;
		if(CopyLength!=0){
			TMAMFunc::MoveInArray(this->Pointer.All(),Index,CopySrc,CopyLength);
		}
		// detruct unused tail
		TMAMFunc::Destruct(this->Pointer.All(),this->Length,OldLength);
	}
	
	void SetLength(uIntn NewLength)noexcept(true){
		if(NewLength==this->Length)
			return;
		if(NewLength<this->Length){
			// truncate
			TMAMFunc::Destruct(this->Pointer.All(),NewLength,this->Length);
		}
		else{
			// append
			if(NewLength>this->Capacity){
				// grow
				GrowCapacityTo(NewLength);
			}
			// append
			TMAMFunc::Construct(this->Pointer.All(),this->Length,NewLength);
		}
		this->Length=NewLength;
	}

	void SetCapacity(uIntn NewCapacity)noexcept(true){
		if(this->Capacity==NewCapacity)
			return;
		if(NewCapacity>this->Capacity){
			return GrowCapacityTo(NewCapacity);
		}
		if(NewCapacity==0){
			if(this->Length!=0){
				TMAMFunc::Destruct(this->Pointer.All(),0,this->Length);
			}
			TMAMFunc::Deallocate(this->Pointer.All(),this->Capacity);
			Reset();
			return;
		}

		// shrink
		if(this->Length>NewCapacity){
			TMAMFunc::Destruct(this->Pointer.All(),NewCapacity,this->Length);
			this->Length=NewCapacity;
		}

		bool ManualCopy[PointerCount];
		tPointer NewArray;
		TMAMFunc::Relocate(NewArray.All(),this->Pointer.All(),this->Capacity,NewCapacity,ManualCopy);
		cnLib_ASSERT(NewArray.Length>=NewCapacity);
		if(TMAMFunc::MaskIsAnySet(ManualCopy)){
			// need manual copy
			TMAMFunc::MaskMoveConstruct(ManualCopy,NewArray.All(),0,this->Pointer,0,this->Length);
			TMAMFunc::MaskDestruct(ManualCopy,this->Pointer.All(),0,this->Length);
			TMAMFunc::MaskDeallocate(ManualCopy,this->Pointer.All(),this->Capacity);
		}
		this->Pointer=NewArray;
		this->Capacity=NewCapacity;
	}

	void ReplaceWithUninitialized(uIntn Index,uIntn ReplaceLength,uIntn ReplacementLength)noexcept(true){
		cnLib_ASSERT(Index<=this->Length);
		uIntn TailSrc=Index+ReplaceLength;
		uIntn TailDest=Index+ReplacementLength;
		if(TailSrc>=this->Length){
			//				|xxxxxxxxxxxxxxxxxxx|
			//	Array		Index	NewLength	CurLength	NewLength
			//						TailDest	TailSrc		TailDest
			// replace tail
			if(Index<this->Length){
				// destruct replaced
				TMAMFunc::Destruct(this->Pointer.All(),Index,this->Length);
				this->Length=Index;
			}
			// grow capacity
			if(this->Capacity<TailDest){
				GrowCapacityTo(TailDest);
			}
			this->Length=TailDest;
			return;
		}
		// destruct replaced
		uIntn TailLength=this->Length-TailSrc;
		uIntn NewLength=TailDest+TailLength;
		bool ReplaceMask[PointerCount];
		if(NewLength<=this->Capacity){
			// replace array in place
			for(uIntn i=0;i<PointerCount;i++){
				ReplaceMask[i]=true;
			}
		}
		else{
			// grow
			uIntn NewCapacity=TMAMFunc::RoundUpCapacity(NewLength);
			bool ManualCopy[PointerCount];
			tPointer NewArray;
			TMAMFunc::Relocate(NewArray.All(),this->Pointer.All(),this->Capacity,NewCapacity,ManualCopy);
			bool AnyManualCopy=false;
			bool AllManualCopy=true;
			for(uIntn i=0;i<PointerCount;i++){
				if(ManualCopy[i]){
					AnyManualCopy=true;
					ReplaceMask[i]=false;
				}
				else{
					AllManualCopy=false;
					ReplaceMask[i]=true;
				}
			}
			if(AnyManualCopy){
				// move manual copy array as replaced layout to new array
				TMAMFunc::MaskMoveConstruct(ManualCopy,NewArray.All(),0,this->Pointer.All(),0,Index);
				TMAMFunc::MaskMoveConstruct(ManualCopy,NewArray.All(),TailDest,this->Pointer.All(),TailSrc,TailLength);

				// delete old array
				TMAMFunc::MaskDestruct(ManualCopy,this->Pointer.All(),0,this->Length);
				TMAMFunc::MaskDeallocate(ManualCopy,this->Pointer.All(),this->Capacity);
			}

			this->Pointer=NewArray;
			this->Capacity=NewCapacity;
			cnLib_ASSERT(this->Capacity!=0);
			if(AllManualCopy){
				// all array is manually copied as replaced layout
				this->Length=NewLength;
				return;
			}
			// replace remaining array in place
		}

		if(TailSrc==TailDest){
			TMAMFunc::MaskDestruct(ReplaceMask,this->Pointer.All(),Index,TailDest);
			return;
		}
		if(TailSrc<TailDest){
			//						|=======================| --------->|
			//				|xxxxxxxxxxxxxxxxxxx|===========++++++++++++|
			//	Array		Index	TailSrc		TailDest	OldLength	NewLength
			TMAMFunc::MaskMoveHeadConstructTailInArray(ReplaceMask,this->Pointer.All(),this->Length,TailDest,TailLength);
			// destruct replaced item
			TMAMFunc::MaskDestruct(ReplaceMask,this->Pointer.All(),Index,TailDest);
		}
		else{
			//						|<---------	|=======================|
			//				|xxxxxxx|=======================|xxxxxxxxxxx|
			//	Array		Index	TailDest	TailSrc		NewLength	OldLength

			TMAMFunc::MaskMoveInArray(ReplaceMask,this->Pointer.All(),TailDest,TailSrc,TailLength);
			// destruct replaced item
			TMAMFunc::MaskDestruct(ReplaceMask,this->Pointer.All(),Index,TailDest);
			// destruct tail
			TMAMFunc::MaskDestruct(ReplaceMask,this->Pointer.All(),NewLength,this->Length);
		}
		this->Length=NewLength;
	}

	void SetRange(uIntn Index,uIntn CopyLength,const tPointer &Src,uIntn SrcIndex)noexcept(true){
		if(Index>this->Length){
			return;
		}

		ReplaceWithUninitialized(Index,CopyLength,CopyLength);

		TMAMFunc::CopyConstruct(this->Pointer.All(),Index,Src,SrcIndex,CopyLength);
	}
	void Copy(const tPointer &Src,uIntn SrcIndex,uIntn SrcLength)noexcept(true){
		if(SrcLength==0)
			return;
		if(this->Length==0){
			if(SrcLength>this->Capacity){
				GrowCapacityTo(SrcLength);
			}
			TMAMFunc::CopyConstruct(this->Pointer.All(),0,Src.All(),SrcIndex,SrcLength);
			this->Length=SrcLength;
		}
		else if(SrcLength<=this->Length){
			TMAMFunc::Copy(this->Pointer.All(),0,SrcLength,Src.All(),SrcIndex);
		}
		else if(SrcLength<=this->Capacity){
			TMAMFunc::CopyHeadConstructTail(this->Pointer.All(),0,Src.All(),SrcIndex,this->Length,SrcLength);
		}
		else{	// SrcLength>this->Capacity
			uIntn NewCapacity=TMAMFunc::RoundUpCapacity(SrcLength);
			bool ManualCopy[PointerCount];
			bool ResizedMask[PointerCount];
			tPointer NewArray;
			TMAMFunc::Relocate(NewArray,this->Pointer.All(),this->Capacity,NewCapacity,ManualCopy);
			tPointer ManualCopyPointer=this->Pointer;
			bool AnyManualCopy=false;
			bool AnyResized=false;
			for(uIntn i=0;i<PointerCount;i++){
				if(ManualCopy[i]){
					AnyManualCopy=true;
					ResizedMask[i]=false;
				}
				else{
					AnyResized=true;
					ResizedMask[i]=true;
				}
			}
			if(AnyManualCopy){
				TMAMFunc::MaskDestruct(ManualCopy,this->Pointer.All(),0,this->Length);
				TMAMFunc::MaskDeallocate(ManualCopy,this->Pointer.All(),this->Capacity);

				TMAMFunc::MaskCopyConstruct(ManualCopy,NewArray.All(),0,Src.All(),SrcIndex,SrcLength);
			}
			this->Pointer=NewArray;
			this->Capacity=NewCapacity;
			if(AnyResized){
				TMAMFunc::MaskCopyHeadConstructTail(ResizedMask,this->Pointer.All(),0,Src.All(),SrcIndex,this->Length,SrcLength);
			}
			this->Length=SrcLength;
		}
	}

	void Move(const tPointer &Src,uIntn SrcLength)noexcept(true){
		if(this->Length!=0){
			TMAMFunc::Destruct(this->Pointer.All(),0,this->Length);
			this->Length=0;
		}
		if(SrcLength>this->Capacity){
			GrowCapacityTo(SrcLength);
		}

		TMAMFunc::MoveConstruct(this->Pointer.All(),0,Src.All(),0,SrcLength);
		this->Length=SrcLength;
	}

};

#endif // cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L

//---------------------------------------------------------------------------
}	// namespace cnDataStruct
//---------------------------------------------------------------------------
}   // namespace cnLibrary
//---------------------------------------------------------------------------
#include <cnTK/TKMacrosCleanup.inc>
//---------------------------------------------------------------------------
#endif	/* cnLibrary_CPPFEATURELEVEL >= 1 */
/*-------------------------------------------------------------------------*/
#endif
