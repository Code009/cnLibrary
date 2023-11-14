/*- cnCRT - cnRTLCore Memory ----------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2019-01-13                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnRTL_CRTCoreMemory_H__
#define __cnLibrary_cnRTL_CRTCoreMemory_H__

#include <cnRTL/CRTCoreCommon.h>

#ifdef __cplusplus
//---------------------------------------------------------------------------

#if cnLibrary_CPPFEATURE_CONSTEXPR >= 200704L
#define	cnRTL_CONSTVAR	constexpr
// cnLibrary_CPPFEATURE_CONSTEXPR >= 200704L
#else
// cnLibrary_CPPFEATURE_CONSTEXPR < 200704L
#define	cnRTL_CONSTVAR	const
#endif // cnLibrary_CPPFEATURE_CONSTEXPR < 200704L
//---------------------------------------------------------------------------
namespace cnLib_THelper{
//---------------------------------------------------------------------------
namespace Memory_TH{

//---------------------------------------------------------------------------
template<uIntn ElementSize>
struct tArray_Fill
{
	typedef typename cnVar::TIntegerOfSize<ElementSize,false>::Type tUInt;

	static void Fill(void *Dest,uIntn Size,tUInt Src)noexcept(true){
		uIntn Length=Size/ElementSize;
		Size%=ElementSize;
		cnMemory::ViewFill(static_cast<tUInt*>(Dest),Length,Src);

		if(Size==0)
			return;
		::memcpy(cnMemory::PointerAddByteOffset(Dest,Length*ElementSize),&Src,Size);
	}
};
//---------------------------------------------------------------------------
template<>
struct tArray_Fill<1>
{
	static void Fill(void *Dest,uIntn Size,uInt8 Src)noexcept(true){
		::memset(Dest,Src,Size);
	}
};

//---------------------------------------------------------------------------
template<uIntn Size>
inline void ReverseBytes(void *Dest,const void *Src)
{
	for(uIntn i=0;i<Size;i++){
		static_cast<uInt8*>(Dest)[i]=static_cast<const uInt8*>(Src)[Size-i-1];
	}
}
//---------------------------------------------------------------------------
template<cnMemory::eByteOrder NativeOrder,cnMemory::eByteOrder SwapOrder>
struct TByteOrderConvert;

template<cnMemory::eByteOrder Order>
struct TByteOrderConvert<Order,Order>
{
	template<class T>
	static T Call(const T &Data){
		return Data;
	}
};

template<>
struct TByteOrderConvert<cnMemory::ByteOrder::LittleEndian,cnMemory::ByteOrder::BigEndian>
{	// BigEndian -> LittleEndian
	template<class T>
	static T Call(const T &Data){
		T Ret;
		ReverseBytes<sizeof(T)>(&Ret,&Data);
		return Ret;
	}
};

template<>
struct TByteOrderConvert<cnMemory::ByteOrder::BigEndian,cnMemory::ByteOrder::LittleEndian>
{	// LittleEndian -> BigEndian
	template<class T>
	static T Call(const T &Data){
		T Ret;
		ReverseBytes<sizeof(T)>(&Ret,&Data);
		return Ret;
	}
};

//---------------------------------------------------------------------------
}	// namespace Memory
}	// namespace cnLib_THelper
//---------------------------------------------------------------------------
namespace cnLibrary{



template<uIntn ElementSize>
struct TKRuntime::tMemory
{

	static void Copy(void *Dest,const void *Src,uIntn Length)noexcept(true){
		::memcpy(Dest,Src,Length*ElementSize);
	}
	static void CopyOverlapped(void *Dest,const void *Src,uIntn Length)noexcept(true){
		::memmove(Dest,Src,Length*ElementSize);
	}
	static void ZeroFill(void *Dest,uIntn Length)noexcept(true){
		::memset(Dest,0,Length*ElementSize);
	}
	static bool Equal(const void *Src1,const void *Src2,uIntn Length)noexcept(true){
		return std::char_traits<char>::compare(static_cast<const char*>(Src1),static_cast<const char*>(Src2),Length*ElementSize)==0;
	}
};


template<uIntn ElementSize>
struct TKRuntime::tArray
{
	typedef typename cnVar::TIntegerOfSize<ElementSize,false>::Type tUInt;

	static void Fill(void *Dest,uIntn Length,tUInt Data)noexcept(true){
		return cnLib_THelper::Memory_TH::tArray_Fill<ElementSize>::Fill(Dest,Length,Data);
	}
	
	static sfInt8 Compare(const void *Src1,const void *Src2,uIntn Length)noexcept(true){
		return static_cast<sfInt8>(std::char_traits<char>::compare(static_cast<const char*>(Src1),static_cast<const char*>(Src2),Length*ElementSize));
	}
};

//---------------------------------------------------------------------------
namespace cnMemory{
//---------------------------------------------------------------------------

inline void Copy(void *Dest,const void *Src,uIntn Size){
	::memcpy(Dest,Src,Size);
}
inline void CopyO(void *Dest,const void *Src,uIntn Size){
	::memmove(Dest,Src,Size);
}

inline uIntn MatchSize(const void *Mem1,const void *Mem2,uIntn Size){
	return ViewMatchLength(static_cast<const uInt8*>(Mem1),static_cast<const uInt8*>(Mem2),Size);
}


// Fill
//	Fill Dest with Data
// [in]Dest	array to fill
// [in]Size	bytes to fill
// [in]Data	data
template<class T>
inline void MatchLength(const T *Src1,typename cnVar::TTypeDef<T>::Type const *Src2,uIntn Length)noexcept(true)
{
	return ViewMatchLength(Src1,Src2,Length);
}

// Search
//	Search data in Src
// Src	[in]	array to search
// Length		length of array
// Dest			data to search
// Return:	the index of the first matched data, or -1 if the Dest not found
template<class TSearchElement>
inline uIntn Search(const TSearchElement *Src,uIntn Length,cnVar::TypeDef<TSearchElement> Dest)
{
	return ViewLinearSearch(Src,Length,[&Dest](const TSearchElement &Element){
		return Element==Dest;
	});
}
// SearchNot
//	Search Src for data is not Dest
// Src		[in]	array to search
// Length			length of array
// Dest				data to search
// Return:	the index of the first matched data, or -1 if the Dest not found
template<class TSearchElement>
inline uIntn SearchNot(const TSearchElement *Src,uIntn Length,cnVar::TypeDef<TSearchElement> Dest)
{
	return ViewLinearSearch(Src,Length,[&Dest](const TSearchElement &Element){
		return Element!=Dest;
	});
}
// SearchBackward
//	Search data in Src in backward direction
// [in]Src	array to search
// [in]Length	length of array
// [in]Dest	data to search
// Return:	the index of the first matched data, or -1 if the Dest not found
template<class TSearchElement>
inline uIntn ReverseSearch(const TSearchElement *Src,uIntn Length,cnVar::TypeDef<TSearchElement> Dest)
{
	return ViewLinearReverseSearch(Src,Length,[&Dest](const TSearchElement &Element){
		return Element==Dest;
	});
}
// SearchBackwardNot
//	Search data in Src in backward direction
// [in]Src	array to search
// [in]Length	length of array
// [in]Dest	data to search
// Return:	the index of the first matched data, or -1 if the Dest not found
template<class TSearchElement>
inline uIntn ReverseSearchNot(const TSearchElement *Src,uIntn Length,cnVar::TypeDef<TSearchElement> Dest)
{
	return ViewLinearReverseSearch(Src,Length,[&Dest](const TSearchElement &Element){
		return Element!=Dest;
	});
}

// UnalignedRead
//	read from unaligned memory
// Pointer	[in]	pointer to memory to read
// return: content of pointer
template<class T>
inline T UnalignedRead(const T *Pointer)
{
	T Ret;
	memcpy(&Ret,Pointer,sizeof(T));
	return Ret;
}

// UnalignedWrite
//	write to unaligned memory
// Pointer	[out]	pointer to memory to write
// Value			value to write
template<class T>
inline void UnalignedWrite(T *Pointer,const cnVar::TypeDef<T> &Value)
{
	memcpy(Pointer,&Value,sizeof(T));
}

// SwapByteOrder
//	swap byte order between native and specified
// [in]Value		bytes of data to swap
// return converted data
template<cnMemory::eByteOrder Order,class T>
inline T SwapByteOrder(T Value)
{
#if cnCRT_BYTEORDER_LITTLEENDIAN
#define	NATIVE_BYTE_ORDER	cnMemory::ByteOrder::LittleEndian
#endif
#if cnCRT_BYTEORDER_BIGENDIAN
#define	NATIVE_BYTE_ORDER	cnMemory::ByteOrder::BigEndian
#endif
	return cnLib_THelper::Memory_TH::TByteOrderConvert<NATIVE_BYTE_ORDER,Order>::Call(Value);
#undef	NATIVE_BYTE_ORDER
}

// SwapLittleEndian
//	swap byte order between native and little endian
// [in]Value		little endian data
// return converted data
template<class T>
inline T SwapLittleEndian(T Value)
{
	return SwapByteOrder<cnMemory::ByteOrder::LittleEndian>(Value);
}

// SwapBigEndian
//	swap byte order between native and big endian
// [in]Value		big endian data
// return converted data
template<class T>
inline T SwapBigEndian(T Value)
{
	return SwapByteOrder<cnMemory::ByteOrder::BigEndian>(Value);
}
//---------------------------------------------------------------------------
}	// namespace cnMemory
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#undef	cnRTL_CONSTVAR
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif


