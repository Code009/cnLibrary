/*- cnTK - Numerical Functions --------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2005-11-20                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnTK_Numerical_H__
#define __cnLibrary_cnTK_Numerical_H__
/*-------------------------------------------------------------------------*/
#include <cnTK/Common.h>
#include <cnTK/TypeTraits.h>
/*-------------------------------------------------------------------------*/
#if	cnLibrary_CPPFEATURELEVEL >= 1
//---------------------------------------------------------------------------
#include <cnTK/TKMacrosDeclare.inc>
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace TKRuntime{
//---------------------------------------------------------------------------

struct NativeByteOrder;
//{
//	static constexpr eByteOrder Value;
//};

template<uIntn IntegerSize>
struct TInteger;
//{
//	static T UnalignRead(const T *Variable)noexcept;
//	static void UnalignWrite(T *Variable,T Value)noexcept;
//	static T ReverseBytes(T Value)noexcept;
//	static sfInt8 Compare(T Src1,T Src2)noexcept;
//	static sfInt8 BytesCompare(T Src1,T Src2)noexcept;
//	static tUInt ShiftLeftInto(tUInt ValueHigh,tUInt ValueLow,ufInt8 Count)noexcept;
//	static tUInt ShiftRightInto(tUInt ValueLow,tUInt ValueHigh,ufInt8 Count)noexcept;
//};

//---------------------------------------------------------------------------
}	// namespace TKRuntime
//---------------------------------------------------------------------------
namespace CPPRuntime{
//---------------------------------------------------------------------------

template<uIntn IntegerSize>
struct TInteger
{
	typedef typename cnVar::TIntegerOfSize<IntegerSize,false>::Type tUInt;
	typedef typename cnVar::TIntegerOfSize<IntegerSize,true>::Type tSInt;

	static sfInt8 Compare(tUInt Src1,tUInt Src2)noexcept(true){
		return cnVar::DefaultCompare(Src1,Src2);
	}


	static tUInt ShiftLeftInto(tUInt ValueHigh,tUInt ValueLow,ufInt8 Count)noexcept(true)
	{
		tUInt v=ValueHigh<<Count;
		v|=ValueLow>>(IntegerSize*ByteBitCount-Count);
		return v;
	}
	static tUInt ShiftRightInto(tUInt ValueLow,tUInt ValueHigh,ufInt8 Count)noexcept(true)
	{
		tUInt v=ValueLow>>Count;
		v|=ValueHigh<<(IntegerSize*ByteBitCount-Count);
		return v;
	}
};

//---------------------------------------------------------------------------
}	// namespace CPPRuntime
//---------------------------------------------------------------------------
namespace cnMemory{
//---------------------------------------------------------------------------

cnLib_TYPELESS_ENUM_BEGIN(ByteOrder){
	LittleEndian,
	BigEndian,
}cnLib_TYPELESS_ENUM_END(ByteOrder);

	
template<eByteOrder TargetOrder,eByteOrder ValueOrder>
struct TByteOrderConvert;

template<cnMemory::eByteOrder Order>
struct TByteOrderConvert<Order,Order>
{
	template<class T>
	static T Convert(const T &Data){
		return Data;
	}
};

template<>
struct TByteOrderConvert<cnMemory::ByteOrder::LittleEndian,cnMemory::ByteOrder::BigEndian>
{	// BigEndian -> LittleEndian
	template<class T>
	static T Convert(const T &Data){
		typedef typename cnVar::TIntegerOfSize<sizeof(T),false>::Type tInt;
		return cnVar::ReturnCast<T>(
			TKRuntime::TInteger<sizeof(T)>::ReverseBytes(reinterpret_cast<const tInt&>(Data))
		);
	}
};

template<>
struct TByteOrderConvert<cnMemory::ByteOrder::BigEndian,cnMemory::ByteOrder::LittleEndian>
{	// LittleEndian -> BigEndian
	template<class T>
	static T Convert(const T &Data){
		typedef typename cnVar::TIntegerOfSize<sizeof(T),false>::Type tInt;
		return cnVar::ReturnCast<T>(
			TKRuntime::TInteger<sizeof(T)>::ReverseBytes(reinterpret_cast<const tInt&>(Data))
		);
	}
};


// SwapByteOrder
//	swap byte order between native and specified
// [in]Value		bytes of data to swap
// return converted data
template<eByteOrder Order,class T>
inline T SwapByteOrder(T Value)
{
	return TByteOrderConvert<TKRuntime::NativeByteOrder::Value,Order>::Convert(Value);
}

// SwapLittleEndian
//	swap byte order between native and little endian
// [in]Value		little endian data
// return converted data
template<class T>
inline T SwapLittleEndian(T Value)
{
	return TByteOrderConvert<TKRuntime::NativeByteOrder::Value,ByteOrder::LittleEndian>::Convert(Value);
}

// SwapBigEndian
//	swap byte order between native and big endian
// [in]Value		big endian data
// return converted data
template<class T>
inline T SwapBigEndian(T Value)
{
	return TByteOrderConvert<TKRuntime::NativeByteOrder::Value,ByteOrder::BigEndian>::Convert(Value);
}


// UnalignedRead
//	read from unaligned memory
// Pointer	[in]	pointer to memory to read
// return: content of pointer
template<class T>
inline T UnalignedRead(const T *Pointer)noexcept(true)
{
	typedef typename cnVar::TIntegerOfSize<sizeof(T),false>::Type tInt;
	return cnVar::ReturnCast<T>(
		TKRuntime::TInteger<sizeof(T)>::UnalignedRead(reinterpret_cast<const tInt*>(Pointer))
	);
}

// UnalignedWrite
//	write to unaligned memory
// Pointer	[out]	pointer to memory to write
// Value			value to write
template<class T>
inline void UnalignedWrite(T *Pointer,const cnVar::TypeDef<T> &Value)noexcept(true)
{
	typedef typename cnVar::TIntegerOfSize<sizeof(T),false>::Type tInt;
	return TKRuntime::TInteger<sizeof(T)>::UnalignedWrite(reinterpret_cast<tInt*>(Pointer),reinterpret_cast<tInt&>(Value));
}



// ShiftLeftInto
//	Shift High while moving bits from Low
// [in]High		uIntn to shift
// [in]Low		bits to shift into Dest
// [in]Count	counts to shift, must smaller than bit count of uIntn
// return: High<<Count<<Low
template<class T>
T ShiftLeftInto(typename cnVar::TTypeDef<T>::Type High,T Low,ufInt8 Count)
{
	typedef typename cnVar::TIntegerOfSize<sizeof(T),false>::Type tUInt;
	return static_cast<T>(TKRuntime::TInteger<sizeof(T)>::ShiftLeftInto(
		static_cast<tUInt>(High),
		static_cast<tUInt>(Low),
		static_cast<uInt8>(Count)
	));
}
// ShiftRightInto
//	Shift Low while moving bits from High
// [in]Low		uIntn to shift
// [in]High		bits to shift into Dest
// [in]Count	counts to shift, must smaller than bit count of uIntn
// return: Low>>Count>>High
template<class T>
T ShiftRightInto(T Low,typename cnVar::TTypeDef<T>::Type High,uIntn Count)
{
	typedef typename cnVar::TIntegerOfSize<sizeof(T),false>::Type tUInt;
	return static_cast<T>(TKRuntime::TInteger<sizeof(T)>::ShiftRightInto(
		static_cast<tUInt>(Low),
		static_cast<tUInt>(High),
		static_cast<uInt8>(Count)
	));
}

//---------------------------------------------------------------------------
}	// namespace cnMemory
//---------------------------------------------------------------------------
namespace cnMath{
//---------------------------------------------------------------------------
#if cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L

template<class T,T...>
struct TMax;

template<class T,T Val>
struct TMax<T,Val> : cnVar::TConstantValue<T,Val>{};

template<class T,T Compare,T Val,T...Vals>
struct TMax<T,Compare,Val,Vals...>
	 : cnVar::TConstantValue<T,TMax<T, (Compare>Val ? Compare : Val) ,Vals...>::Value>
{
};


template<class T,T...>
struct TMin;

template<class T,T Val>
struct TMin<T,Val> : cnVar::TConstantValue<T, Val> {};

template<class T,T Compare,T Val,T...Vals>
struct TMin<T,Compare,Val,Vals...>
	 : cnVar::TConstantValue<T,TMin<T, (Compare<Val ? Compare : Val) ,Vals...>::Value>
{
};

#endif	// cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L
//---------------------------------------------------------------------------
}	// namespace cnMath
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
namespace cnLib_THelper{
//---------------------------------------------------------------------------
namespace Integer_TH{
//---------------------------------------------------------------------------
template<class TValueDecl,class TNextDecl>
struct IntegerValueMSBCalc;

template<class T,T v1,T v>
struct IntegerValueMSBCalc< cnVar::TConstantValue<T,v1>,cnVar::TConstantValue<T,v>  >
	: IntegerValueMSBCalc<cnVar::TConstantValue<T,v>,cnVar::TConstantValue<T,v&(v-1)> >
{
};
template<class T,T v>
struct IntegerValueMSBCalc< cnVar::TConstantValue<T,v>,cnVar::TConstantValue<T,0>  >
	: cnVar::TConstantValue<T,v>
{
};


template<uIntn Size,bool Sign>
struct IntegerValue;
template<uIntn Size>
struct IntegerValue<Size,false>
{
	typedef typename cnVar::TIntegerOfSize<Size,false>::Type Type;
	// MIN
	//	unsigned min value
	static cnLib_CONSTVAR Type Min=0;
	// MAX
	//	unsigned max value
	static cnLib_CONSTVAR Type Max=static_cast<Type>(~static_cast<Type>(0));

	// MSB
	//	unsigned most sigificand bit
	static cnLib_CONSTVAR Type MSB=static_cast<Type>(static_cast<Type>(1)<<(sizeof(Type)*ByteBitCount-1));

	template<Type v>
	struct tValueLSB
		: cnVar::TConstantValue<Type,v&~(v-1)>
	{
	};

	template<Type v>
	struct tValueMSB
		: IntegerValueMSBCalc< cnVar::TConstantValue<Type,v>,cnVar::TConstantValue<Type,v&(v-1)> >
	{
	};
};
template<uIntn Size>
struct IntegerValue<Size,true>
{
	typedef typename cnVar::TIntegerOfSize<Size,true>::Type Type;
	// MAX
	//	signed max value
	//static cnLib_CONSTVAR Type Max=static_cast<Type>((~static_cast<typename cnInteger::TOfSize<Size,false>::Type>(0))>>1);
	static cnLib_CONSTVAR Type Max=static_cast<Type>(IntegerValue<Size,false>::Max>>1);
	// MIN
	//	signed min value
	static cnLib_CONSTVAR Type Min=-Max-1;

	// MSB
	//	signed min value
	static cnLib_CONSTVAR Type MSB=Min;

	template<Type v>
	struct tValueLSB
		: cnVar::TConstantValue<Type,v&~(v-1)>
	{
	};

	template<Type v>
	struct tValueMSB
		: IntegerValueMSBCalc< cnVar::TConstantValue<Type,v>,cnVar::TConstantValue<Type,v&(v-1)> >
	{
	};
};

//---------------------------------------------------------------------------
}	// namespace Integer_TH
//---------------------------------------------------------------------------
}	// namespace cnLib_THelper
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnVar{
//---------------------------------------------------------------------------

template<class T>
struct TIntegerValue : cnLib_THelper::Integer_TH::IntegerValue<sizeof(T),TIsSigned<T>::Value>
{
	static cnLib_CONSTVAR uIntn BitCount = sizeof(T)*ByteBitCount;
};

#if cnLibrary_CPPFEATURE_VARIABLE_TEMPLATES >= 201304L
	
template<class T>
static cnLib_CONSTVAR uIntn IntegerBitCount= TIntegerValue<T>::BitCount;

template<class T>
static cnLib_CONSTVAR T IntegerValueRangeMax = TIntegerValue<T>::Max;
template<class T>
static cnLib_CONSTVAR T IntegerValueRangeMin = TIntegerValue<T>::Min;
template<class T>
static cnLib_CONSTVAR T IntegerMSBValue = TIntegerValue<T>::MSB;

#endif // cnLibrary_CPPFEATURE_VARIABLE_TEMPLATES >= 201304L



template<uIntn Dest>
struct TIntegerConstantMSBPos;

template<>
struct TIntegerConstantMSBPos<1>
	: TConstantValueUIntn<0>{};

template<>
struct TIntegerConstantMSBPos<0>
	: TConstantValueUIntn<IndexNotFound>{};

template<uIntn Dest>
struct TIntegerConstantMSBPos
	: TConstantValueUIntn<1+TIntegerConstantMSBPos<(Dest>>1)>::Value>{};

template<uIntn Dest>
struct TIntegerConstantLSBPos
	: TIntegerConstantMSBPos<Dest&~(Dest-1)>{};



//consteval uIntn MSBPos(const uIntn Dest)
//{	return Dest==0? 0 : TIntegerConstantMSBPos(Dest>>1)+1;	}
//---------------------------------------------------------------------------
}	// namespace cnVar
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------

namespace cnLib_THelper{
namespace Math_TH{

	template<bool Signed>
	struct Abs
	{
		template<class T>
		static T Call(T Value)noexcept(cnLib_NOEXCEPTEXPR(Value>=0) && cnLib_NOEXCEPTEXPR(T(-Value)))
		{
			if(Value<0)
				return -Value;
			return Value;
		}

		template<class T>
		static bool Set(T Value)noexcept(cnLib_NOEXCEPTEXPR(Value>=0) && cnLib_NOEXCEPTEXPR(Value=-Value))
		{
			if(Value<0){
				Value=-Value;
				return true;
			}
			return false;
		}
	};
	template<>
	struct Abs<false>
	{
		template<class T>
		static T Call(T v)noexcept(true){
			return v;
		}
		template<class T>
		static bool Set(T &)noexcept(true){
			return false;
		}
	};

}	// namespace Math_TH
}	// namespace cnLib_THelper


//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnMath{
//---------------------------------------------------------------------------

template<class TFloat>
struct TFloatConstant
{
#if cnLibrary_CPPFEATURE_CONSTEXPR >= 200704L
	static constexpr TFloat PI			=static_cast<TFloat>(3.14159265358979323846);
	static constexpr TFloat log2e		=static_cast<TFloat>(1.4426950408889634073599246810019);
	static constexpr TFloat log2e_m1	=static_cast<TFloat>(0.4426950408889634073599246810019);
	static constexpr TFloat loge2		=static_cast<TFloat>(0.6931471805599453);
	static constexpr TFloat log2_10		=static_cast<TFloat>(3.3219280948873622);
// cnLibrary_CPPFEATURE_CONSTEXPR >= 200704L
#else
// cnLibrary_CPPFEATURE_CONSTEXPR < 200704L
	static const TFloat PI;
	static const TFloat log2e;
	static const TFloat log2e_m1;
	static const TFloat loge2;
	static const TFloat log2_10;
#endif	// cnLibrary_CPPFEATURE_CONSTEXPR < 200704L
};

//---------------------------------------------------------------------------
template<class T>
inline T Absolute(T Value)noexcept(cnLib_NOEXCEPTEXPR(cnLib_THelper::Math_TH::Abs<cnVar::TIsSigned<T>::Value>::Call(Value)))
{	return cnLib_THelper::Math_TH::Abs<cnVar::TIsSigned<T>::Value>::Call(Value);	}

template<class T>
inline bool SetAbsolute(T &Value)noexcept(cnLib_NOEXCEPTEXPR(cnLib_THelper::Math_TH::Abs<cnVar::TIsSigned<T>::Value>::Call(Value)))
{	return cnLib_THelper::Math_TH::Abs<cnVar::TIsSigned<T>::Value>::Set(Value);	}
//---------------------------------------------------------------------------
template<class T>
inline const T& Min(const T &Value1,typename cnVar::TTypeDef<T>::Type const &Value2)noexcept(cnLib_NOEXCEPTEXPR(Value1<Value2))
{
	return Value1<Value2 ? Value1 : Value2;
}
template<class T>
inline const T& Max(const T &Value1,typename cnVar::TTypeDef<T>::Type const &Value2)noexcept(cnLib_NOEXCEPTEXPR(Value1>Value2))
{
	return Value1>Value2 ? Value1 : Value2;
}
//---------------------------------------------------------------------------
}	// namespace cnMath
//---------------------------------------------------------------------------
namespace TKRuntime{
//---------------------------------------------------------------------------

template<uIntn FloatSize>
struct TMathFloat;
//{
//	static tFloat Absolute(tFloat n)noexcept;
//	static tFloat RoundNearest(tFloat n)noexcept;
//	static tFloat RoundTruncate(tFloat n)noexcept;
//	static tFloat RoundNegative(tFloat n)noexcept;
//	static tFloat RoundPositive(tFloat n)noexcept;
//	static tFloat SplitExponent(tFloat n,sfInt16 &Exponent)noexcept;
//	static tFloat Modulo(tFloat Dividend,tFloat Divisor)noexcept;
//	static tFloat SquareRoot(tFloat n)noexcept;
//	static tFloat NaturalLogarithm(tFloat n)noexcept;
//	static tFloat BinaryLogarithm(tFloat n)noexcept;
//	static tFloat NaturalExponential(tFloat n)noexcept;
//	static tFloat BinaryExponential(tFloat n)noexcept;
//	static tFloat Power(tFloat Base,tFloat Exponent)noexcept;
//	static tFloat Sine(tFloat n)noexcept;
//	static tFloat Cosine(tFloat n)noexcept;
//	static tFloat Tangent(tFloat n)noexcept;
//	static tFloat ArcSine(tFloat n)noexcept;
//	static tFloat ArcCosine(tFloat n)noexcept;
//	static tFloat ArcTangent(tFloat n)noexcept;
//	static tFloat ArcTangent2(tFloat y,tFloat x)noexcept;
//};
//---------------------------------------------------------------------------
}	// namespace TKRuntime
//---------------------------------------------------------------------------
namespace CPPRuntime{
//---------------------------------------------------------------------------

template<uIntn FloatSize>
struct TMathFloat
{
	typedef typename cnVar::TFloatOfSize<FloatSize>::Type tFloat;

	static tFloat Absolute(tFloat n)noexcept(true){
		return n<0?-n:n;
	}
	/*
	static tFloat RoundNearest(tFloat n)noexcept(true){
	}

	static tFloat RoundTruncate(tFloat n)noexcept(true){
	}

	static tFloat RoundNegative(tFloat n)noexcept(true){
	}

	static tFloat RoundPositive(tFloat n)noexcept(true){
	}

	static tFloat SplitExponent(tFloat n,sfInt16 &Exponent)noexcept(true){
	}

	static tFloat Modulo(tFloat Dividend,tFloat Divisor)noexcept(true){
	}
	static tFloat SquareRoot(tFloat n)noexcept(true){
	}
	static tFloat NaturalLogarithm(tFloat n)noexcept(true){
	}
	static tFloat BinaryLogarithm(tFloat n)noexcept(true){
	}
	static tFloat NaturalExponential(tFloat n)noexcept(true){
	}
	static tFloat BinaryExponential(tFloat n)noexcept(true){
	}
	static tFloat Power(tFloat Base,tFloat Exponent)noexcept(true){
	}
	static tFloat Sine(tFloat n)noexcept(true){
	}
	static tFloat Cosine(tFloat n)noexcept(true){
	}
	static tFloat Tangent(tFloat n)noexcept(true){
	}
	static tFloat ArcSine(tFloat n)noexcept(true){
	}
	static tFloat ArcCosine(tFloat n)noexcept(true){
	}
	static tFloat ArcTangent(tFloat n)noexcept(true){
	}
	static tFloat ArcTangent2(tFloat y,tFloat x)noexcept(true){
	}
	*/
};



//---------------------------------------------------------------------------
}	// namespace CPPRuntime
//---------------------------------------------------------------------------
namespace cnMath{
//---------------------------------------------------------------------------

template<class T>
inline typename cnVar::TFloatConversion<T>::tMatch FloatAbsolute(const T &n)noexcept(true)
{	return TKRuntime::TMathFloat<cnVar::TFloatConversion<T>::MatchSize>::Absolute(static_cast<typename cnVar::TFloatConversion<T>::tMatch>(n));	}

template<class T>
inline typename cnVar::TFloatConversion<T>::tMatch FloatRoundNearest(const T &n)noexcept(true)
{	return TKRuntime::TMathFloat<cnVar::TFloatConversion<T>::MatchSize>::RoundNearest(static_cast<typename cnVar::TFloatConversion<T>::tMatch>(n));	}

template<class T>
inline typename cnVar::TFloatConversion<T>::tMatch FloatRoundTruncate(const T &n)noexcept(true)
{	return TKRuntime::TMathFloat<cnVar::TFloatConversion<T>::MatchSize>::RoundTruncate(static_cast<typename cnVar::TFloatConversion<T>::tMatch>(n));	}

template<class T>
inline typename cnVar::TFloatConversion<T>::tMatch FloatRoundNegative(const T &n)noexcept(true)
{	return TKRuntime::TMathFloat<cnVar::TFloatConversion<T>::MatchSize>::RoundNegative(static_cast<typename cnVar::TFloatConversion<T>::tMatch>(n));	}

template<class T>
inline typename cnVar::TFloatConversion<T>::tMatch FloatRoundPositive(const T &n)noexcept(true)
{	return TKRuntime::TMathFloat<cnVar::TFloatConversion<T>::MatchSize>::RoundPositive(static_cast<typename cnVar::TFloatConversion<T>::tMatch>(n));	}

template<class T>
inline typename cnVar::TFloatConversion<T>::tMatch FloatSplitExponent(T n,sfInt16 &Exponent)noexcept(true)
{	return TKRuntime::TMathFloat<cnVar::TFloatConversion<T>::MatchSize>::SplitExponent(static_cast<typename cnVar::TFloatConversion<T>::tMatch>(n),Exponent);	}

template<class T>
inline typename cnVar::TFloatConversion<T>::tMatch FloatModulo(const T &Dividend,typename cnVar::TTypeDef<T>::Type Divisor)noexcept(true)
{	return TKRuntime::TMathFloat<cnVar::TFloatConversion<T>::MatchSize>::Modulo(static_cast<typename cnVar::TFloatConversion<T>::tMatch>(Dividend),static_cast<typename cnVar::TFloatConversion<T>::tMatch>(Divisor));	}


template<class T>
inline typename cnVar::TFloatConversion<T>::tMatch FloatSquareRoot(const T &n)noexcept(true)
{	return TKRuntime::TMathFloat<cnVar::TFloatConversion<T>::MatchSize>::SquareRoot(static_cast<typename cnVar::TFloatConversion<T>::tMatch>(n));	}

template<class T>
inline typename cnVar::TFloatConversion<T>::tMatch FloatNaturalLogarithm(const T &n)noexcept(true)
{	return TKRuntime::TMathFloat<cnVar::TFloatConversion<T>::MatchSize>::NaturalLogarithm(static_cast<typename cnVar::TFloatConversion<T>::tMatch>(n));	}

template<class T>
inline typename cnVar::TFloatConversion<T>::tMatch FloatBinaryLogarithm(const T &n)noexcept(true)
{	return TKRuntime::TMathFloat<cnVar::TFloatConversion<T>::MatchSize>::BinaryLogarithm(static_cast<typename cnVar::TFloatConversion<T>::tMatch>(n));	}

template<class T>
inline typename cnVar::TFloatConversion<T>::tMatch FloatNaturalExponential(const T &n)noexcept(true)
{	return TKRuntime::TMathFloat<cnVar::TFloatConversion<T>::MatchSize>::NaturalExponential(static_cast<typename cnVar::TFloatConversion<T>::tMatch>(n));	}

template<class T>
inline typename cnVar::TFloatConversion<T>::tMatch FloatBinaryExponential(const T &n)noexcept(true)
{	return TKRuntime::TMathFloat<cnVar::TFloatConversion<T>::MatchSize>::BinaryExponential(static_cast<typename cnVar::TFloatConversion<T>::tMatch>(n));	}

template<class T>
inline typename cnVar::TFloatConversion<T>::tMatch FloatPower(const T &Base,typename cnVar::TTypeDef<T>::Type Exponent)noexcept(true)
{	return TKRuntime::TMathFloat<cnVar::TFloatConversion<T>::MatchSize>::Power(static_cast<typename cnVar::TFloatConversion<T>::tMatch>(Base),static_cast<typename cnVar::TFloatConversion<T>::tMatch>(Exponent));	}

template<class T>
inline typename cnVar::TFloatConversion<T>::tMatch FloatSine(const T &n)noexcept(true)
{	return TKRuntime::TMathFloat<cnVar::TFloatConversion<T>::MatchSize>::Sine(static_cast<typename cnVar::TFloatConversion<T>::tMatch>(n));	}

template<class T>
inline typename cnVar::TFloatConversion<T>::tMatch FloatCosine(const T &n)noexcept(true)
{	return TKRuntime::TMathFloat<cnVar::TFloatConversion<T>::MatchSize>::Cosine(static_cast<typename cnVar::TFloatConversion<T>::tMatch>(n));	}

template<class T>
inline typename cnVar::TFloatConversion<T>::tMatch FloatTangent(const T &n)noexcept(true)
{	return TKRuntime::TMathFloat<cnVar::TFloatConversion<T>::MatchSize>::Tangent(static_cast<typename cnVar::TFloatConversion<T>::tMatch>(n));	}

template<class T>
inline typename cnVar::TFloatConversion<T>::tMatch FloatArcSine(const T &n)noexcept(true)
{	return TKRuntime::TMathFloat<cnVar::TFloatConversion<T>::MatchSize>::ArcSine(static_cast<typename cnVar::TFloatConversion<T>::tMatch>(n));	}

template<class T>
inline typename cnVar::TFloatConversion<T>::tMatch FloatArcCosine(const T &n)noexcept(true)
{	return TKRuntime::TMathFloat<cnVar::TFloatConversion<T>::MatchSize>::ArcCosine(static_cast<typename cnVar::TFloatConversion<T>::tMatch>(n));	}

template<class T>
inline typename cnVar::TFloatConversion<T>::tMatch FloatArcTangent(const T &n)noexcept(true)
{	return TKRuntime::TMathFloat<cnVar::TFloatConversion<T>::MatchSize>::ArcTangent(static_cast<typename cnVar::TFloatConversion<T>::tMatch>(n));	}

template<class T>
inline typename cnVar::TFloatConversion<T>::tMatch FloatArcTangentFloatArcTangent2(const T &y,typename cnVar::TTypeDef<T>::Type x)noexcept(true)
{	return TKRuntime::TMathFloat<cnVar::TFloatConversion<T>::MatchSize>::Power(static_cast<typename cnVar::TFloatConversion<T>::tMatch>(y),static_cast<typename cnVar::TFloatConversion<T>::tMatch>(x));	}

//---------------------------------------------------------------------------
}	//	namespace cnMath
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
namespace cnLib_THelper{
namespace Integer_TH{

template<class T>
struct IntBase2Round
{
	template<uIntn b>
	static T RoundPositive(T n)noexcept(true){
		return (n+b-1) & ~(b-1);
	}
	template<uIntn b>
	static T RoundNegative(T n)noexcept(true){
		return n & ~(b-1);
	}
};

template<class T,bool IsSigned>
struct IntBase2Round_Zero
{
	template<uIntn b>
	static T RoundToZero(T n)noexcept(true){
		if(n<0){
			return IntBase2Round<T>::RoundPositive<b>(n);
		}
		else{
			return IntBase2Round<T>::RoundNegative<b>(n);
		}
	}
	template<uIntn b>
	static T RoundAwayZero(T n)noexcept(true){
		if(n<0){
			return IntBase2Round<T>::RoundNegative<b>(n);
		}
		else{
			return IntBase2Round<T>::RoundPositive<b>(n);
		}
	}
};
template<class T>
struct IntBase2Round_Zero<T,false>
{
	template<uIntn b>
	static T RoundToZero(T n)noexcept(true){
		return IntBase2Round<T>::RoundNegative<b>(n);
	}
	template<uIntn b>
	static T RoundAwayZero(T n)noexcept(true){
		return IntBase2Round<T>::RoundPositive<b>(n);
	}
};


template<class T>
struct IntBaseNRound_ToZero
{
	template<uIntn b>
	static T RoundToZero(T n)noexcept(true){
		return n - n%b;
	}
};
template<class T,bool Signed>
struct IntBaseNRound_Zero;

template<class T>
struct IntBaseNRound_Zero<T,false>
{
	template<uIntn b>
	static T RoundAwayZero(T n)noexcept(true){
		n+=b-1;
		return n - n%b;
	}
};
template<class T,bool Signed>
struct IntBaseNRound_Zero
{
	template<uIntn b>
	static T RoundAwayZeroN(T n)noexcept(true){
		n-=b-1;
		return n - n%b;
	}

	template<int b>
	static int RoundAwayZero(int n)noexcept(true){
		if(n<0){
			return RoundAwayZeroN<b>(n);
		}
		else{
			return IntBaseNRound_ToZero<T>::RoundAwayZeroP<b>(n);
		}
	}
};

template<class T,bool IsSigned>
struct IntBaseNRound
{
	template<uIntn b>
	static T RoundPositive(T n)noexcept(true){
		if(n<0){
			return IntBaseNRound_ToZero<T>::RoundToZero<b>(n);
		}
		else{
			IntBaseNRound_Zero<T,false>::RoundAwayZero<b>(n);
		}
	}
	template<uIntn b>
	static uIntn RoundNegative(uIntn n)noexcept(true){
		if(n<0){
			return IntBaseNRound_Zero<T,true>::RoundAwayZeroN<b>(n);
		}
		else{
			return IntBaseNRound_ToZero<T>::RoundToZero<b>(n);
		}
	}

};
template<class T>
struct IntBaseNRound<T,false>
{
	template<uIntn b>
	static T RoundPositive(T n)noexcept(true){
		return IntBaseNRound_Zero<T,false>::RoundAwayZero<b>(n);
	}
	template<uIntn b>
	static T RoundNegative(T n)noexcept(true){
		return IntBaseNRound_ToZero<T>::RoundToZero<b>(n);
	}

};

template<class T,bool Base2>
struct IntRoundFunction :
	IntBase2Round<T>,
	IntBase2Round_Zero<T,cnVar::TIsSigned<T>::Value>
{};
template<class T>
struct IntRoundFunction<T,false> :
	IntBaseNRound_ToZero<T>,
	IntBaseNRound_Zero<T,cnVar::TIsSigned<T>::Value>,
	IntBaseNRound<T,cnVar::TIsSigned<T>::Value>
{};

template<class T,T r,T f,uIntn e>
struct TConstantPowI
{
	static cnLib_CONSTVAR T Value=TConstantPowI<T,r*f,f,e-1>::Value;
};
template<class T,T r,T f>
struct TConstantPowI<T,r,f,0>
{
	static cnLib_CONSTVAR T Value=r;
};

}	// namespace Integer_TH
}	// namespace cnLib_THelper
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnMath{

	
//---------------------------------------------------------------------------
template<uIntn Base,class T>
inline T IntRoundToZero(T Value)noexcept(true){
	return cnLib_THelper::Integer_TH::IntRoundFunction<T,(Base & (Base-1))==0>::template RoundToZero<Base>(Value);
}

template<uIntn Base,class T>
inline T IntRoundAwayZero(T Value)noexcept(true){
	return cnLib_THelper::Integer_TH::IntRoundFunction<T,(Base & (Base-1))==0>::template RoundAwayZero<Base>(Value);
}

template<uIntn Base,class T>
inline T IntRoundPositive(T Value)noexcept(true){
	return cnLib_THelper::Integer_TH::IntRoundFunction<T,(Base & (Base-1))==0>::template RoundPositive<Base>(Value);
}
template<uIntn Base,class T>
inline T IntRoundNegative(T Value)noexcept(true){
	return cnLib_THelper::Integer_TH::IntRoundFunction<T,(Base & (Base-1))==0>::template RoundNegative<Base>(Value);
}

// RoundUpDiv
template<class TInt>
inline cnLib_CONSTEXPR_FUNC TInt IntRoundUpDiv(TInt Diviend,typename cnVar::TTypeDef<TInt>::Type Divisor)noexcept(true)
{
	return (Diviend+Divisor-1)/Divisor;
}

// RoundDown
template<class TInt>
inline cnLib_CONSTEXPR_FUNC TInt IntRoundDown(TInt Value,typename cnVar::TTypeDef<TInt>::Type Base)noexcept(true)
{
	return  (Base&(Base-1))
		? ( Value-Value%Base )
		: ( (Value)&(~(Base-1)) );
}
// RoundUp
template<class TInt>
inline cnLib_CONSTEXPR_FUNC TInt IntRoundUp(TInt Value,typename cnVar::TTypeDef<TInt>::Type Base)noexcept(true)
{
	return IntRoundDown(Value+Base-1,Base);
}



template<class T,T n,uIntn e>
struct TConstantPowI : cnLib_THelper::Integer_TH::TConstantPowI<T,1,n,e>
{
};

template<class T>
inline T PowI(T n,uIntn e)noexcept(true)
{
	T r;
	if(e&1){r=n;}else{r=static_cast<T>(1);}
	T factor=n;
	for(e>>=1;e!=0;e>>=1){
		factor*=factor;
		if(e&1){
			r*=factor;
		}
	}
	return r;
}
//---------------------------------------------------------------------------
}	// namespace cnMath
//---------------------------------------------------------------------------
}   // namespace cnLibrary
//---------------------------------------------------------------------------
#include <cnTK/TKMacrosCleanup.inc>
//---------------------------------------------------------------------------
#endif  /* cnLibrary_CPPFEATURELEVEL >= 1 */
/*-------------------------------------------------------------------------*/
#endif

