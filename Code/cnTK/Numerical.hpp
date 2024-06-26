/*- cnTK - Numerical Functions --------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2005-11-20                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnTK_Numerical_HPP__
#define __cnLibrary_cnTK_Numerical_HPP__
/*-------------------------------------------------------------------------*/
#include <cnTK/Common.hpp>
#include <cnTK/TypeTraits.hpp>
/*-------------------------------------------------------------------------*/
#if	cnLibrary_CPPFEATURELEVEL >= 1
//---------------------------------------------------------------------------
#include <cnTK/TKMacrosDeclare.inc>
//---------------------------------------------------------------------------

#if	cnLibrary_CPPFEATURELEVEL >= 2

//---------------------------------------------------------------------------
namespace cnLib_THelper{
//---------------------------------------------------------------------------
namespace Var_TH{
//---------------------------------------------------------------------------
namespace IntegerConversion{
//---------------------------------------------------------------------------

cnVar::TIntegerOfSize<sizeof(long double),true>::Type		TestMatch(long double);
cnVar::TIntegerOfSize<sizeof(double),true>::Type			TestMatch(double);
cnVar::TIntegerOfSize<sizeof(float),true>::Type				TestMatch(float);

cnVar::TIntegerOfSize<sizeof(unsigned long long),false>::Type	TestMatch(unsigned long long);
cnVar::TIntegerOfSize<sizeof(signed long long),true>::Type		TestMatch(signed long long);
cnVar::TIntegerOfSize<sizeof(unsigned long),false>::Type		TestMatch(unsigned long);
cnVar::TIntegerOfSize<sizeof(signed long),true>::Type			TestMatch(signed long);
cnVar::TIntegerOfSize<sizeof(unsigned int),false>::Type			TestMatch(unsigned int);
cnVar::TIntegerOfSize<sizeof(signed int),true>::Type			TestMatch(signed int);
cnVar::TIntegerOfSize<sizeof(unsigned short),false>::Type		TestMatch(unsigned short);
cnVar::TIntegerOfSize<sizeof(signed short),true>::Type			TestMatch(signed short);
cnVar::TIntegerOfSize<sizeof(unsigned char),false>::Type		TestMatch(unsigned char);
cnVar::TIntegerOfSize<sizeof(signed char),true>::Type			TestMatch(signed char);

uInt8	TestMatch(uInt8);
sInt8	TestMatch(sInt8);
uInt16	TestMatch(uInt16);
sInt16	TestMatch(sInt16);
uInt32	TestMatch(uInt32);
sInt32	TestMatch(sInt32);
uInt64	TestMatch(uInt64);
sInt64	TestMatch(sInt64);

sInt32	TestMatch(Float32);
sInt64	TestMatch(Float64);

template<class TEnable,class T>
struct MatchTypeDef
	: cnVar::TTypeDef<void>{};

#if !defined(cnLibrary_CPPEXCLUDE_SFINAE_DECLTYPE_EXPRESSION) && cnLibrary_CPPFEATURE_DECLTYPE >= 200707L

template<class T>
struct MatchTypeDef<decltype(static_cast<void>(TestMatch(cnVar::DeclVal<T>()))),T>
	: cnVar::TTypeDef<decltype(TestMatch(cnVar::DeclVal<T>()))>{};

// !defined(cnLibrary_CPPEXCLUDE_SFINAE_DECLTYPE_EXPRESSION)
#else
// !defined(cnLibrary_CPPEXCLUDE_SFINAE_SIZEOF_EXPRESSION)

template<class T>
struct MatchTypeDef<typename cnVar::TTypeConditional<void,sizeof(TestMatch(cnVar::DeclVal<T>()))>::Type,T>
	: cnVar::TIntegerOfSize<sizeof(TestMatch(cnVar::DeclVal<T>())),cnVar::TIsSigned<typename cnVar::TRemoveReference<T>::Type>::Value>{};

#endif // !defined(cnLibrary_CPPEXCLUDE_SFINAE_SIZEOF_EXPRESSION)

template<class T,class TMatch>
struct Decl
{
	typedef TMatch tMatch;
	static cnLib_CONSTVAR ufInt8 MatchSize=sizeof(TMatch);
	static cnLib_CONSTVAR bool IsSigned=cnVar::TIsSigned<TMatch>::Value;
	static cnLib_CONSTVAR bool IsConvertible=true;
	static cnLib_CONSTVAR bool IsMatch=false;

	template<class TInt>
	static TInt Cast(T Value)noexcept(noexcept(static_cast<TInt>(static_cast<TMatch>(Value))))
	{	return static_cast<TInt>(static_cast<TMatch>(Value));	}

	template<class TInt>
	static TInt rtCast(void *Value)noexcept(noexcept(static_cast<TInt>(static_cast<TMatch>(*static_cast<T*>(Value)))))
	{	return static_cast<TInt>(static_cast<TMatch>(*static_cast<T*>(Value)));	}
};

template<class T>
struct Decl<T,T>
{
	typedef T tMatch;
	static cnLib_CONSTVAR ufInt8 MatchSize=sizeof(T);
	static cnLib_CONSTVAR bool IsSigned=cnVar::TIsSigned<T>::Value;
	static cnLib_CONSTVAR bool IsConvertible=true;
	static cnLib_CONSTVAR bool IsMatch=true;

	template<class TInt>
	static TInt Cast(T Value)noexcept(noexcept(static_cast<TInt>(Value)))
	{	return static_cast<TInt>(Value);	}

	template<class TInt>
	static TInt rtCast(void *Value)noexcept(noexcept(static_cast<TInt>(*static_cast<T*>(Value))))
	{	return static_cast<TInt>(*static_cast<T*>(Value));	}
};



template<class T,class TDistinct>
struct DeclUnconvertible
{
	static cnLib_CONSTVAR ufInt8 MatchSize=0;
	static cnLib_CONSTVAR bool IsSigned=false;
	static cnLib_CONSTVAR bool IsConvertible=false;
	static cnLib_CONSTVAR bool IsMatch=false;

	template<class TInt>
	static TInt Cast(T)noexcept(true){
		return 0;
	}

	template<class TInt>
	static TInt rtCast(void*)noexcept(true){
		return 0;
	}
};

template<class T,class TDistinct>
struct DeclUnconvertible<T,TDistinct*>
{
	static cnLib_CONSTVAR ufInt8 MatchSize=sizeof(void*);
	static cnLib_CONSTVAR bool IsSigned=false;
	static cnLib_CONSTVAR bool IsConvertible=true;
	static cnLib_CONSTVAR bool IsMatch=false;

	template<class TInt>
	static TInt Cast(T Value)noexcept(noexcept(static_cast<TInt>(reinterpret_cast<uIntn>(Value))))
	{	return static_cast<TInt>(reinterpret_cast<uIntn>(Value));	}

	template<class TInt>
	static TInt rtCast(void *Value)noexcept(noexcept(static_cast<TInt>(*static_cast<const uIntn*>(Value))))
	{	return static_cast<TInt>(*static_cast<const uIntn*>(Value));	}
};



template<class T,bool Convertible>
struct DeclNoMatch
	: DeclUnconvertible<T,sInt64>{};

template<class T>
struct DeclNoMatch<T,false>
	: DeclUnconvertible<T,typename cnVar::TRemoveCV<T>::Type>{};

template<class T>
struct Decl<T,void>	// fail to find exact match
	: DeclNoMatch<T,cnVar::TIsConvertible<T&,sInt64>::Value>{};

//---------------------------------------------------------------------------
}	// namespace IntegerConversion
//---------------------------------------------------------------------------
namespace FloatConversion{
//---------------------------------------------------------------------------

template<uIntn Size>	struct FloatForIntegerSize{};

template<>	struct FloatForIntegerSize<1>	: cnVar::TTypeDef<Float32>{};
template<>	struct FloatForIntegerSize<2>	: cnVar::TTypeDef<Float32>{};
template<>	struct FloatForIntegerSize<4>	: cnVar::TTypeDef<Float64>{};
template<>	struct FloatForIntegerSize<8>	: cnVar::TTypeDef<Float64>{};
template<>	struct FloatForIntegerSize<16>	: cnVar::TTypeDef<Float64>{};

cnVar::TFloatOfSize<sizeof(long double)>::Type		TestMatch(long double);
cnVar::TFloatOfSize<sizeof(double)>::Type			TestMatch(double);
cnVar::TFloatOfSize<sizeof(float)>::Type			TestMatch(float);

FloatForIntegerSize<sizeof(unsigned long long)>::Type	TestMatch(unsigned long long);
FloatForIntegerSize<sizeof(signed long long)>::Type		TestMatch(signed long long);
FloatForIntegerSize<sizeof(unsigned long)>::Type		TestMatch(unsigned long);
FloatForIntegerSize<sizeof(signed long)>::Type			TestMatch(signed long);
FloatForIntegerSize<sizeof(unsigned int)>::Type			TestMatch(unsigned int);
FloatForIntegerSize<sizeof(signed int)>::Type			TestMatch(signed int);
FloatForIntegerSize<sizeof(unsigned short)>::Type		TestMatch(unsigned short);
FloatForIntegerSize<sizeof(signed short)>::Type			TestMatch(signed short);
FloatForIntegerSize<sizeof(unsigned char)>::Type		TestMatch(unsigned char);
FloatForIntegerSize<sizeof(signed char)>::Type			TestMatch(signed char);

FloatForIntegerSize<sizeof(uInt8)>::Type	TestMatch(uInt8);
FloatForIntegerSize<sizeof(sInt8)>::Type	TestMatch(sInt8);
FloatForIntegerSize<sizeof(uInt16)>::Type	TestMatch(uInt16);
FloatForIntegerSize<sizeof(sInt16)>::Type	TestMatch(sInt16);
FloatForIntegerSize<sizeof(uInt32)>::Type	TestMatch(uInt32);
FloatForIntegerSize<sizeof(sInt32)>::Type	TestMatch(sInt32);
FloatForIntegerSize<sizeof(uInt64)>::Type	TestMatch(uInt64);
FloatForIntegerSize<sizeof(sInt64)>::Type	TestMatch(sInt64);

Float32	TestMatch(Float32);
Float64	TestMatch(Float64);


template<class TEnable,class T>
struct MatchTypeDef
	: cnVar::TTypeDef<void>{};


#ifndef cnLibrary_CPPEXCLUDE_SFINAE_DECLTYPE_EXPRESSION

template<class T>
struct MatchTypeDef<decltype(static_cast<void>(TestMatch(cnVar::DeclVal<T>()))),T>
	: cnVar::TTypeDef<decltype(TestMatch(cnVar::DeclVal<T>()))>{};

// !defined(cnLibrary_CPPEXCLUDE_SFINAE_DECLTYPE_EXPRESSION)
#else
// !defined(cnLibrary_CPPEXCLUDE_SFINAE_SIZEOF_EXPRESSION)

template<class T>
struct MatchTypeDef<typename cnVar::TTypeConditional<void,sizeof(TestMatch(cnVar::DeclVal<T>()))>::Type,T>
	: cnVar::TFloatOfSize<sizeof(TestMatch(cnVar::DeclVal<T>()))>{};

#endif // !defined(cnLibrary_CPPEXCLUDE_SFINAE_SIZEOF_EXPRESSION)


template<class T,class TMatch>
struct Decl
{
	typedef TMatch tMatch;
	static cnLib_CONSTVAR ufInt8 MatchSize=sizeof(TMatch);
	static cnLib_CONSTVAR bool IsConvertible=true;
	static cnLib_CONSTVAR bool IsMatch=false;

	template<class TFloat>
	static TFloat Cast(T Value)noexcept(noexcept(static_cast<TFloat>(static_cast<TMatch>(Value))))
	{	return static_cast<TFloat>(static_cast<TMatch>(Value));	}

	template<class TFloat>
	static TFloat rtCast(void *Value)noexcept(noexcept(static_cast<TFloat>(static_cast<TMatch>(*static_cast<T*>(Value)))))
	{	return static_cast<TFloat>(static_cast<TMatch>(*static_cast<T*>(Value)));	}
};


template<class T>
struct Decl<T,T>
{
	typedef T tMatch;
	static cnLib_CONSTVAR ufInt8 MatchSize=sizeof(T);
	static cnLib_CONSTVAR bool IsConvertible=true;
	static cnLib_CONSTVAR bool IsMatch=true;

	template<class TFloat>
	static TFloat Cast(T Value)noexcept(noexcept(static_cast<TFloat>(Value)))
	{	return static_cast<TFloat>(Value);	}

	template<class TFloat>
	static TFloat rtCast(void *Value)noexcept(noexcept(static_cast<TFloat>(*static_cast<T*>(Value))))
	{	return static_cast<TFloat>(*static_cast<T*>(Value));	}
};




template<class T,class TDistinct>
struct DeclUnconvertible
{
	static cnLib_CONSTVAR ufInt8 MatchSize=0;
	static cnLib_CONSTVAR bool IsConvertible=false;
	static cnLib_CONSTVAR bool IsMatch=false;

	template<class TFloat>
	static TFloat Cast(T)noexcept(true){
		return 0.f;
	}

	template<class TFloat>
	static TFloat rtCast(void*)noexcept(true){
		return 0.f;
	}
};

template<class T,bool Convertible>
struct DeclNoMatch
	: Decl<T,Float64>{};

template<class T>
struct DeclNoMatch<T,false>
	: DeclUnconvertible<T,typename cnVar::TRemoveCV<T>::Type>{};

template<class T>
struct Decl<T,void>	// fail to find exact match
	: DeclNoMatch<T,cnVar::TIsConvertible<T&,sInt64>::Value>{};

//---------------------------------------------------------------------------
}	// namespace FloatConversion
//---------------------------------------------------------------------------
}	// namespace Var_TH
//---------------------------------------------------------------------------
}	// namespace cnLib_THelper
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnVar{
//---------------------------------------------------------------------------

template<class T>
struct TIntegerConversion
	: cnLib_THelper::Var_TH::IntegerConversion::Decl<T
		,typename cnLib_THelper::Var_TH::IntegerConversion::MatchTypeDef<void,T>::Type
	>{};


template<class T>
struct TFloatConversion
	: cnLib_THelper::Var_TH::FloatConversion::Decl<T
		,typename cnLib_THelper::Var_TH::FloatConversion::MatchTypeDef<void,T>::Type
	>{};

//---------------------------------------------------------------------------
}	// namespace cnVar
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  // cnLibrary_CPPFEATURELEVEL >= 2
//---------------------------------------------------------------------------
namespace cnLibrary{
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
		static T Call(T Value)noexcept(noexcept(Value>=0) && noexcept(T(-Value)))
		{
			if(Value<0)
				return -Value;
			return Value;
		}

		template<class T>
		static bool Set(T Value)noexcept(noexcept(Value>=0) && noexcept(Value=-Value))
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
inline T Absolute(T Value)noexcept(noexcept(cnLib_THelper::Math_TH::Abs<cnVar::TIsSigned<T>::Value>::Call(Value)))
{	return cnLib_THelper::Math_TH::Abs<cnVar::TIsSigned<T>::Value>::Call(Value);	}

template<class T>
inline bool SetAbsolute(T &Value)noexcept(noexcept(cnLib_THelper::Math_TH::Abs<cnVar::TIsSigned<T>::Value>::Call(Value)))
{	return cnLib_THelper::Math_TH::Abs<cnVar::TIsSigned<T>::Value>::Set(Value);	}
//---------------------------------------------------------------------------
template<class T>
inline const T& Min(const T &Value1,typename cnVar::TTypeDef<T>::Type const &Value2)noexcept(noexcept(Value1<Value2))
{
	return Value1<Value2 ? Value1 : Value2;
}
template<class T>
inline const T& Max(const T &Value1,typename cnVar::TTypeDef<T>::Type const &Value2)noexcept(noexcept(Value1>Value2))
{
	return Value1>Value2 ? Value1 : Value2;
}
//---------------------------------------------------------------------------
}	// namespace cnMath
//---------------------------------------------------------------------------
namespace TKRuntime{
//---------------------------------------------------------------------------

template<uIntn FloatSize>
struct TFloatSpecialConstant;
//{
//	static const tFloat Max;
//	static const tFloat InfP;
//	static const tFloat InfN;
//	static const tFloat NaN;
//};

template<uIntn FloatSize>
struct TFloatMath;
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
struct TFloatMath
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
{	return TKRuntime::TFloatMath<cnVar::TFloatConversion<T>::MatchSize>::Absolute(static_cast<typename cnVar::TFloatConversion<T>::tMatch>(n));	}

template<class T>
inline typename cnVar::TFloatConversion<T>::tMatch FloatRoundNearest(const T &n)noexcept(true)
{	return TKRuntime::TFloatMath<cnVar::TFloatConversion<T>::MatchSize>::RoundNearest(static_cast<typename cnVar::TFloatConversion<T>::tMatch>(n));	}

template<class T>
inline typename cnVar::TFloatConversion<T>::tMatch FloatRoundTruncate(const T &n)noexcept(true)
{	return TKRuntime::TFloatMath<cnVar::TFloatConversion<T>::MatchSize>::RoundTruncate(static_cast<typename cnVar::TFloatConversion<T>::tMatch>(n));	}

template<class T>
inline typename cnVar::TFloatConversion<T>::tMatch FloatRoundNegative(const T &n)noexcept(true)
{	return TKRuntime::TFloatMath<cnVar::TFloatConversion<T>::MatchSize>::RoundNegative(static_cast<typename cnVar::TFloatConversion<T>::tMatch>(n));	}

template<class T>
inline typename cnVar::TFloatConversion<T>::tMatch FloatRoundPositive(const T &n)noexcept(true)
{	return TKRuntime::TFloatMath<cnVar::TFloatConversion<T>::MatchSize>::RoundPositive(static_cast<typename cnVar::TFloatConversion<T>::tMatch>(n));	}

template<class T>
inline typename cnVar::TFloatConversion<T>::tMatch FloatSplitExponent(T n,sfInt16 &Exponent)noexcept(true)
{	return TKRuntime::TFloatMath<cnVar::TFloatConversion<T>::MatchSize>::SplitExponent(static_cast<typename cnVar::TFloatConversion<T>::tMatch>(n),Exponent);	}

template<class T>
inline typename cnVar::TFloatConversion<T>::tMatch FloatModulo(const T &Dividend,typename cnVar::TTypeDef<T>::Type Divisor)noexcept(true)
{	return TKRuntime::TFloatMath<cnVar::TFloatConversion<T>::MatchSize>::Modulo(static_cast<typename cnVar::TFloatConversion<T>::tMatch>(Dividend),static_cast<typename cnVar::TFloatConversion<T>::tMatch>(Divisor));	}


template<class T>
inline typename cnVar::TFloatConversion<T>::tMatch FloatSquareRoot(const T &n)noexcept(true)
{	return TKRuntime::TFloatMath<cnVar::TFloatConversion<T>::MatchSize>::SquareRoot(static_cast<typename cnVar::TFloatConversion<T>::tMatch>(n));	}

template<class T>
inline typename cnVar::TFloatConversion<T>::tMatch FloatNaturalLogarithm(const T &n)noexcept(true)
{	return TKRuntime::TFloatMath<cnVar::TFloatConversion<T>::MatchSize>::NaturalLogarithm(static_cast<typename cnVar::TFloatConversion<T>::tMatch>(n));	}

template<class T>
inline typename cnVar::TFloatConversion<T>::tMatch FloatBinaryLogarithm(const T &n)noexcept(true)
{	return TKRuntime::TFloatMath<cnVar::TFloatConversion<T>::MatchSize>::BinaryLogarithm(static_cast<typename cnVar::TFloatConversion<T>::tMatch>(n));	}

template<class T>
inline typename cnVar::TFloatConversion<T>::tMatch FloatNaturalExponential(const T &n)noexcept(true)
{	return TKRuntime::TFloatMath<cnVar::TFloatConversion<T>::MatchSize>::NaturalExponential(static_cast<typename cnVar::TFloatConversion<T>::tMatch>(n));	}

template<class T>
inline typename cnVar::TFloatConversion<T>::tMatch FloatBinaryExponential(const T &n)noexcept(true)
{	return TKRuntime::TFloatMath<cnVar::TFloatConversion<T>::MatchSize>::BinaryExponential(static_cast<typename cnVar::TFloatConversion<T>::tMatch>(n));	}

template<class T>
inline typename cnVar::TFloatConversion<T>::tMatch FloatPower(const T &Base,typename cnVar::TTypeDef<T>::Type Exponent)noexcept(true)
{	return TKRuntime::TFloatMath<cnVar::TFloatConversion<T>::MatchSize>::Power(static_cast<typename cnVar::TFloatConversion<T>::tMatch>(Base),static_cast<typename cnVar::TFloatConversion<T>::tMatch>(Exponent));	}

template<class T>
inline typename cnVar::TFloatConversion<T>::tMatch FloatSine(const T &n)noexcept(true)
{	return TKRuntime::TFloatMath<cnVar::TFloatConversion<T>::MatchSize>::Sine(static_cast<typename cnVar::TFloatConversion<T>::tMatch>(n));	}

template<class T>
inline typename cnVar::TFloatConversion<T>::tMatch FloatCosine(const T &n)noexcept(true)
{	return TKRuntime::TFloatMath<cnVar::TFloatConversion<T>::MatchSize>::Cosine(static_cast<typename cnVar::TFloatConversion<T>::tMatch>(n));	}

template<class T>
inline typename cnVar::TFloatConversion<T>::tMatch FloatTangent(const T &n)noexcept(true)
{	return TKRuntime::TFloatMath<cnVar::TFloatConversion<T>::MatchSize>::Tangent(static_cast<typename cnVar::TFloatConversion<T>::tMatch>(n));	}

template<class T>
inline typename cnVar::TFloatConversion<T>::tMatch FloatArcSine(const T &n)noexcept(true)
{	return TKRuntime::TFloatMath<cnVar::TFloatConversion<T>::MatchSize>::ArcSine(static_cast<typename cnVar::TFloatConversion<T>::tMatch>(n));	}

template<class T>
inline typename cnVar::TFloatConversion<T>::tMatch FloatArcCosine(const T &n)noexcept(true)
{	return TKRuntime::TFloatMath<cnVar::TFloatConversion<T>::MatchSize>::ArcCosine(static_cast<typename cnVar::TFloatConversion<T>::tMatch>(n));	}

template<class T>
inline typename cnVar::TFloatConversion<T>::tMatch FloatArcTangent(const T &n)noexcept(true)
{	return TKRuntime::TFloatMath<cnVar::TFloatConversion<T>::MatchSize>::ArcTangent(static_cast<typename cnVar::TFloatConversion<T>::tMatch>(n));	}

template<class T>
inline typename cnVar::TFloatConversion<T>::tMatch FloatArcTangent2(const T &y,typename cnVar::TTypeDef<T>::Type x)noexcept(true)
{	return TKRuntime::TFloatMath<cnVar::TFloatConversion<T>::MatchSize>::ArcTangent2(static_cast<typename cnVar::TFloatConversion<T>::tMatch>(y),static_cast<typename cnVar::TFloatConversion<T>::tMatch>(x));	}

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

