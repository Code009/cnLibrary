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
		static bool Call(T &Value)noexcept(cnLib_NOEXCEPTEXPR(Value>=0) && cnLib_NOEXCEPTEXPR(Value=-Value))
		{
			if(Value>=0)
				return false;
			Value=-Value;
			return true;
		}
	};
	template<>
	struct Abs<false>
	{
		template<class T>
		static bool Call(T &)noexcept(true){
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
inline bool Abs(T &Value)noexcept(cnLib_NOEXCEPTEXPR(cnLib_THelper::Math_TH::Abs<cnVar::TIsSigned<T>::Value>::Call(Value)))
{	return cnLib_THelper::Math_TH::Abs<cnVar::TIsSigned<T>::Value>::Call(Value);	}
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
//---------------------------------------------------------------------------
}	// namespace cnMath
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
namespace cnLib_THelper{
namespace Math_TH{


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


template<uIntn IntSize>
struct TMultiprecisionInteger_AddSub
{
	typedef typename cnVar::TIntegerOfSize<IntSize,false>::Type tUInt;
	typedef typename cnVar::TIntegerOfSize<IntSize,true>::Type tSInt;

	static bool UAdd(tUInt &Result,tUInt Augend,tUInt Addend,bool Carry)noexcept(true)
	{
		if(Carry){
			if(++Addend==0){
				Result=Augend;
				return true;
			}
		}
		tUInt r=Augend+Addend;
		Result=r;
		return r<Augend;
	}
	static bool SAdd(tSInt &Result,tSInt Augend,tSInt Addend,bool Carry)noexcept(true)
	{
		if(Carry){
			if(++Addend==cnVar::TIntegerValue<tSInt>::MSB){
				Result=Augend^cnVar::TIntegerValue<tSInt>::MSB;
				return true;
			}
		}
		tSInt r=Augend+Addend;
		Result=r;
		ufInt8 AugendSign=static_cast<ufInt8>(static_cast<tUInt>(Augend)>>(IntSize*ByteBitCount-1));
		ufInt8 AddendSign=static_cast<ufInt8>(static_cast<tUInt>(Addend)>>(IntSize*ByteBitCount-1));
		ufInt8 rSign=static_cast<ufInt8>(static_cast<tUInt>(r)>>(IntSize*ByteBitCount-1));
		rSign=~(AugendSign^AddendSign) & (AugendSign^rSign);
		return reinterpret_cast<bool&>(rSign);
	}
	static bool USub(tUInt &Result,tUInt Minuend,tUInt Subtrahend,bool Borrow)noexcept(true)
	{
		if(Borrow){
			if(++Subtrahend==0){
				Result=Minuend;
				return true;
			}
		}
		tUInt r=Minuend-Subtrahend;
		Result=r;
		return r>Minuend;
	}
	static bool SSub(tSInt &Result,tSInt Minuend,tSInt Subtrahend,bool Borrow)noexcept(true)
	{
		if(Borrow){
			if(++Subtrahend==cnVar::TIntegerValue<tSInt>::MSB){
				Result=Minuend^cnVar::TIntegerValue<tSInt>::MSB;
				return true;
			}
		}
		tSInt r=Minuend-Subtrahend;
		Result=r;
		ufInt8 MinuendSign=static_cast<ufInt8>(static_cast<tUInt>(Minuend)>>(IntSize*ByteBitCount-1));
		ufInt8 SubtrahendSign=static_cast<ufInt8>(static_cast<tUInt>(Subtrahend)>>(IntSize*ByteBitCount-1));
		ufInt8 rSign=static_cast<ufInt8>(static_cast<tUInt>(r)>>(IntSize*ByteBitCount-1));
		rSign=~(MinuendSign^SubtrahendSign) & (MinuendSign^rSign);
		return reinterpret_cast<bool&>(rSign);
	}
};

template<uIntn IntSize>
struct TMultiprecisionInteger_Shift
{
	typedef typename cnVar::TIntegerOfSize<IntSize,false>::Type tUInt;
	typedef typename cnVar::TIntegerOfSize<IntSize,true>::Type tSInt;

	static tUInt ShiftLeftInto(tUInt ValueHigh,tUInt ValueLow,ufInt8 Count)noexcept(true)
	{
		tUInt v=ValueHigh<<Count;
		v|=ValueLow>>(IntSize*ByteBitCount-Count);
		return v;
	}
	static tUInt ShiftRightInto(tUInt ValueLow,tUInt ValueHigh,ufInt8 Count)noexcept(true)
	{
		tUInt v=ValueLow>>Count;
		v|=ValueHigh<<(IntSize*ByteBitCount-Count);
		return v;
	}
	
};

template<uIntn IntSize,bool DoubleSizeAvailable>
struct TMultiprecisionInteger_MulDiv
{
	typedef typename cnVar::TIntegerOfSize<IntSize,false>::Type tUInt;
	typedef typename cnVar::TIntegerOfSize<IntSize,true>::Type tSInt;
	typedef typename cnVar::TIntegerOfSize<IntSize*2,false>::Type t2UInt;
	typedef typename cnVar::TIntegerOfSize<IntSize*2,true>::Type t2SInt;

	static tUInt UMul(tUInt &ResultHigh,tUInt Multiplicand,tUInt Multiplier)noexcept(true)
	{
		t2UInt r=static_cast<t2UInt>(Multiplicand)*Multiplier;
		ResultHigh=static_cast<tUInt>(r>>IntSize*8);
		return static_cast<tUInt>(r);
	}

	static tUInt SMul(tSInt &ResultHigh,tSInt Multiplicand,tSInt Multiplier)noexcept(true)
	{
		t2SInt r=static_cast<t2SInt>(Multiplicand)*Multiplier;
		ResultHigh=static_cast<tSInt>(r>>IntSize*8);
		return static_cast<tUInt>(r);
	}

	static tUInt UMulH(tUInt Multiplicand,tUInt Multiplier)noexcept(true)
	{
		t2UInt r=static_cast<t2UInt>(Multiplicand)*Multiplier;
		return static_cast<tUInt>(r>>IntSize*8);
	}

	static tSInt SMulH(tSInt Multiplicand,tSInt Multiplier)noexcept(true)
	{
		t2SInt r=static_cast<t2SInt>(Multiplicand)*Multiplier;
		return static_cast<tSInt>(r>>IntSize*8);
	}

	static tUInt UDiv(tUInt &Remainder,tUInt Dividend_Lo,tUInt Dividend_Hi,tUInt Divisor)noexcept(true)
	{
		t2UInt d=Dividend_Hi;
		d= (d<<IntSize*8) | Dividend_Lo;
		Remainder=static_cast<tUInt>(d%Divisor);
		return static_cast<tUInt>(d/Divisor);
	}

	static tSInt SDiv(tSInt &Remainder,tUInt Dividend_Lo,tSInt Dividend_Hi,tSInt Divisor)noexcept(true)
	{
		t2SInt d=Dividend_Hi;
		d= (d<<IntSize*8) | Dividend_Lo;
		Remainder=static_cast<tSInt>(d%Divisor);
		return static_cast<tSInt>(d/Divisor);
	}


	static tUInt UMulDiv(tUInt &Remainder,tUInt Multiplicand,tUInt Multiplier,tUInt Divisor)noexcept(true)
	{
		t2UInt d=static_cast<t2UInt>(Multiplicand)*Multiplier;
		Remainder=static_cast<tUInt>(d%Divisor);
		return static_cast<tUInt>(d/Divisor);
	}

	static tSInt SMulDiv(tSInt &Remainder,tSInt Multiplicand,tSInt Multiplier,tSInt Divisor)noexcept(true)
	{
		t2SInt d=static_cast<t2SInt>(Multiplicand)*Multiplier;
		Remainder=static_cast<tSInt>(d%Divisor);
		return static_cast<tSInt>(d/Divisor);
	}
};

template<uIntn IntSize>
struct TMultiprecisionInteger_MulDiv<IntSize,false>
{
	typedef typename cnVar::TIntegerOfSize<IntSize,false>::Type tUInt;
	typedef typename cnVar::TIntegerOfSize<IntSize,true>::Type tSInt;

	static tUInt UMul(tUInt &ResultHigh,tUInt Multiplicand,tUInt Multiplier)noexcept(true)
	{
		static cnLib_CONSTVAR tUInt hIntBitCount=IntSize*4;
		static cnLib_CONSTVAR tUInt hIntMask=(static_cast<tUInt>(1)<<hIntBitCount)-1;
		tUInt r[2];
		tUInt t;

		// - *
		// - *
		r[0]=Multiplier&hIntMask;
		r[0]*=Multiplicand&hIntMask;

		// * -
		// * -
		r[1]=Multiplicand>>hIntBitCount;
		r[1]*=Multiplier>>hIntBitCount;

		bool Carry;
		// - *
		// * -
		t=Multiplier>>hIntBitCount;
		t*=Multiplicand&hIntMask;
		Carry=TMultiprecisionInteger_AddSub<IntSize>::UAdd(r[0],r[0],t<<hIntBitCount,false);
		TMultiprecisionInteger_AddSub<IntSize>::UAdd(r[1],r[1],t>>hIntBitCount,Carry);

		// * -
		// - *
		t=Multiplicand>>hIntBitCount;
		t*=Multiplier&hIntMask;
		Carry=TMultiprecisionInteger_AddSub<IntSize>::UAdd(r[0],r[0],t<<hIntBitCount,false);
		TMultiprecisionInteger_AddSub<IntSize>::UAdd(r[1],r[1],t>>hIntBitCount,Carry);

		ResultHigh=r[1];
		return r[0];
	}

	static tUInt SMul(tSInt &ResultHigh,tSInt Multiplicand,tSInt Multiplier)noexcept(true)
	{
		static cnLib_CONSTVAR tUInt hIntBitCount=IntSize*4;
		static cnLib_CONSTVAR tUInt hIntMask=(static_cast<tUInt>(1)<<hIntBitCount)-1;
		tUInt rl;
		tSInt rh;
		tSInt t;

		// * -
		// * -
		rh=Multiplicand>>hIntBitCount;
		rh*=Multiplier>>hIntBitCount;

		// - *
		// - *
		rl=static_cast<tUInt>(Multiplier)&hIntMask;
		rl*=static_cast<tUInt>(Multiplicand)&hIntMask;

		bool Carry;
		// - *
		// * -
		t=Multiplier>>hIntBitCount;
		t*=Multiplicand&hIntMask;
		Carry=TMultiprecisionInteger_AddSub<IntSize>::UAdd(rl,rl,t<<hIntBitCount,false);
		TMultiprecisionInteger_AddSub<IntSize>::SAdd(rh,rh,t>>hIntBitCount,Carry);

		// * -
		// - *
		t=Multiplicand>>hIntBitCount;
		t*=Multiplier&hIntMask;
		Carry=TMultiprecisionInteger_AddSub<IntSize>::UAdd(rl,rl,t<<hIntBitCount,false);
		TMultiprecisionInteger_AddSub<IntSize>::SAdd(rh,rh,t>>hIntBitCount,Carry);

		ResultHigh=rh;
		return rl;
	}
	
	static tUInt UMulH(tUInt Multiplicand,tUInt Multiplier)noexcept(true)
	{
		tUInt rh;
		UMul(rh,Multiplicand,Multiplier);
		return rh;
	}
	static tSInt SMulH(tSInt Multiplicand,tSInt Multiplier)noexcept(true)
	{
		tSInt rh;
		SMul(rh,Multiplicand,Multiplier);
		return rh;
	}

	static tUInt UDiv(tUInt &Remainder,tUInt Dividend_Lo,tUInt Dividend_Hi,tUInt Divisor)noexcept(true)
	{
		if(Dividend_Hi==0){
			Remainder=Dividend_Lo%Divisor;
			return Dividend_Lo/Divisor;
		}
		if(Dividend_Hi>=Divisor){
			// overflow
			Remainder=0;
			return cnVar::TIntegerValue<tUInt>::Max;
		}
		// half integer bit count
		cnLib_CONSTVAR uIntn hiBitCount=IntSize*ByteBitCount/2;


		tUInt qh,q;
		tUInt rh,rl;
		if(Divisor<static_cast<tUInt>(1)<<hiBitCount){
			// Divisor can be used directly

			tUInt dt;
			// Dividend[1:2] / Divisor
			dt=Dividend_Hi<<hiBitCount;
			dt|=Dividend_Lo>>hiBitCount;
			qh=dt/Divisor;
			dt%=Divisor;

			// Dividend[0:1] / Divisor
			dt<<=hiBitCount;
			dt|=Dividend_Lo&((static_cast<tUInt>(1)<<hiBitCount)-1);
			q=dt/Divisor;

			Remainder=dt%Divisor;
			q|=qh<<hiBitCount;
			return q;
		}
		// scale down Divisor
		tUInt dv=Divisor;
		ufInt8 dvScale=0;
		do{
			dv>>=1;
			dvScale++;
		}while(dv>=(static_cast<tUInt>(1)<<hiBitCount));

		tUInt dt;
		ufInt8 dScale=hiBitCount-dvScale;
		// Dividend[1:2]
		if(dScale!=0){
			dt=TMultiprecisionInteger_Shift<IntSize>::ShiftLeftInto(Dividend_Hi,Dividend_Lo,dScale);
		}
		else{
			dt=Dividend_Hi;
		}
		// Dividend[1:2] / ScaledDownDivisor
		qh=dt/dv;
		tUInt dh,dl;	// remaining dividend
		bool rBorrow;
		if(qh>=static_cast<tUInt>(1)<<hiBitCount){
			dh=Dividend_Hi-Divisor;
			dl=Dividend_Lo;
			rBorrow=true;
		}
		else{
			rl=UMul(rh,qh<<hiBitCount,Divisor);
		
			dl=Dividend_Lo;
			rBorrow=TMultiprecisionInteger_AddSub<IntSize>::USub(dh,Dividend_Hi,rh,false);
		}
		if(rBorrow){
			bool c=TMultiprecisionInteger_AddSub<IntSize>::UAdd(dl,dl,Divisor<<hiBitCount,false);
			TMultiprecisionInteger_AddSub<IntSize>::UAdd(dh,dh,Divisor>>hiBitCount,c);
			qh--;
		}


		// Dividend[0:1] / Divisor
		dt=TMultiprecisionInteger_Shift<IntSize>::ShiftLeftInto(dh,dl,hiBitCount+dScale);
		q=dt/dv;

		// Remainder
		if(q>=static_cast<tUInt>(1)<<hiBitCount){
			// rh is no longer needed
			// rBorrow=true;		// should be true

			// rl=Divisor*q;
			// rl=dl-rl;
			// if(true){
			//	rl+=Divisor;
			//	q--;
			// }
			
			rl=dl-(Divisor<<hiBitCount);
			rl+=Divisor;
			q--;
		}
		else{
			rl=UMul(rh,q,Divisor);
			rBorrow=TMultiprecisionInteger_AddSub<IntSize>::USub(rl,dl,rl,false);
			rBorrow=TMultiprecisionInteger_AddSub<IntSize>::USub(rh,dh,rh,rBorrow);
			if(rBorrow){
				rl+=Divisor;
				q--;
			}
		}

		Remainder=rl;
		q|=qh<<hiBitCount;
		return q;
	}

	static tSInt SDiv(tSInt &Remainder,tUInt Dividend_Lo,tSInt Dividend_Hi,tSInt Divisor)noexcept(true)
	{

		bool Signend=Dividend_Hi<0;
		if(Signend){
			if(Dividend_Lo==0){
				Dividend_Hi=-Dividend_Hi;
			}
			else{
				Dividend_Hi=~Dividend_Hi;
				Dividend_Lo=-static_cast<tSInt>(Dividend_Lo);
			}
		}
		bool Signor=Divisor<0;
		if(Signor)
			Divisor=-Divisor;

		tUInt q,r;
		q=UDiv(reinterpret_cast<tUInt&>(r),Dividend_Lo,Dividend_Hi,Divisor);

		if(Signend ^ Signor){
			q=-q;
		}
		if(Signend){
			r=-r;
		}
		Remainder=r;
		return q;
	}

	static tUInt UMulDiv(tUInt &Remainder,tUInt Multiplicand,tUInt Multiplier,tUInt Divisor)noexcept(true)
	{
		tUInt dh,dl;
		dl=UMul(dh,Multiplicand,Multiplier);
		return UDiv(Remainder,dl,dh,Divisor);
	}

	static tSInt SMulDiv(tSInt &Remainder,tSInt Multiplicand,tSInt Multiplier,tSInt Divisor)noexcept(true)
	{
		tSInt dh;
		tUInt dl;
		dl=SMul(dh,Multiplicand,Multiplier);
		return SDiv(Remainder,dl,dh,Divisor);
	}
};

}	// namespace Math_TH
}	// namespace cnLib_THelper
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnMath{
//---------------------------------------------------------------------------

template<class T,T n,uIntn e>
struct TConstantPowI : cnLib_THelper::Math_TH::TConstantPowI<T,1,n,e>
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

template<uIntn IntSize>
struct TMultiprecisionInteger
	: cnLib_THelper::Math_TH::TMultiprecisionInteger_AddSub<IntSize>
	, cnLib_THelper::Math_TH::TMultiprecisionInteger_Shift<IntSize>
	, cnLib_THelper::Math_TH::TMultiprecisionInteger_MulDiv<IntSize,(IntSize<cnVar::MaxIntegerSize)>
{
	typedef typename cnVar::TIntegerOfSize<IntSize,false>::Type tUInt;
	typedef typename cnVar::TIntegerOfSize<IntSize,true>::Type tSInt;
};

template<class TMPInteger>
inline typename TMPInteger::tUInt MPUPowMod(typename TMPInteger::tUInt Base,uIntn Exponent,typename TMPInteger::tUInt Divisor){
	typename TMPInteger::tUInt r;
	if(Exponent&1){
		TMPInteger::UDiv(r,Base,0,Divisor);
	}
	else{
		r=1;
	}
	Exponent>>=1;
	while(Exponent!=0){
		TMPInteger::UMulDiv(Base,Base,Base,Divisor);

		if(Exponent&1){
			TMPInteger::UMulDiv(r,r,Base,Divisor);
		}

		Exponent>>=1;
	};
	return r;
}

//template<class TMPInteger>
//struct TIntegerArray;
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

