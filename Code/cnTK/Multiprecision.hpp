/*- cnTK - Multipleprecision Numbers --------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2024-02-12                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnTK_Multiprecision_H__
#define __cnLibrary_cnTK_Multiprecision_H__
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

template<cnMemory::eByteOrder ByteOrder,uIntn IntegerSize,uIntn ComponentSize>
struct TIntegerComponent;
//{
//	template<ufInt8 Index>
//	static tComponentUInt Get(tUInt Value)noexcept;
//};

//---------------------------------------------------------------------------
}	// namespace TKRuntime
//---------------------------------------------------------------------------
namespace CPPRuntime{
//---------------------------------------------------------------------------

template<cnMemory::eByteOrder ByteOrder,uIntn IntegerSize,uIntn ComponentSize>
struct TIntegerComponent
{
	static cnLib_CONSTVAR uIntn Length=(IntegerSize-1)/ComponentSize+1;

	typedef typename cnVar::TIntegerOfSize<ComponentSize,false>::Type tComponentUInt;
	typedef typename cnVar::TIntegerOfSize<IntegerSize,false>::Type tUInt;

	template<ufInt8 Index>
	static tComponentUInt Get(tUInt Value)noexcept(true){
		cnLib_STATIC_ASSERT(Index<Length,"Index too large");
		Value>>=Index*ComponentSize*ByteBitCount;
		return static_cast<tComponentUInt>(Value);
	}

	template<ufInt8 Index>
	static tUInt Set(tUInt Value,tComponentUInt Component)noexcept(true){
		cnLib_STATIC_ASSERT(Index<Length,"Index too large");
		tUInt ComponentMask=static_cast<tUInt>(Component)<<Index*ComponentSize*ByteBitCount;
		Value&=~(static_cast<tUInt>(~static_cast<tComponentUInt>(0))<<Index*ComponentSize*ByteBitCount);
		return Value|ComponentMask;
	}

};

template<uIntn IntegerSize>
struct TInteger2x
{
	typedef typename cnVar::TIntegerOfSize<IntegerSize,false>::Type tUInt;
	typedef typename cnVar::TIntegerOfSize<IntegerSize/2,false>::Type tUIntH;
	typedef typename TKRuntime::TIntegerComponent<cnVar::TSelect<1,tUInt,TKRuntime::NativeByteOrder>::Type::Value,IntegerSize,IntegerSize/2> tIntegerComponent;

	static tUInt ReverseBytes(tUInt Value)noexcept(true){
		tUIntH v0=tIntegerComponent::template Get<0>(Value);
		tUIntH v1=tIntegerComponent::template Get<1>(Value);
		
		v0=TKRuntime::TInteger<IntegerSize/2>::ReverseBytes(v0);
		v1=TKRuntime::TInteger<IntegerSize/2>::ReverseBytes(v1);

		tUInt RetVal=tIntegerComponent::template Set<0>(0,v0);		
		return tIntegerComponent::template Set<1>(RetVal,v1);
	}

	static sfInt8 Compare(tUInt Src1,tUInt Src2)noexcept(true){
		tUIntH v1=tIntegerComponent::template Get<1>(Src1);
		tUIntH v2=tIntegerComponent::template Get<1>(Src2);
		
		if(v1==v2){
			v1=tIntegerComponent::template Get<0>(Src1);
			v2=tIntegerComponent::template Get<0>(Src2);
			return TKRuntime::TInteger<IntegerSize/2>::Compare(v1,v2);
		}
		return v1<v2?-1:1;
	}

	static tUInt ShiftLeftInto(tUInt ValueHigh,tUInt ValueLow,ufInt8 Count)noexcept(true){
		tUInt RetVal;
		tUIntH vh,vl,vlh;
		if(Count<IntegerSize/2*ByteBitCount){
			vh=tIntegerComponent::template Get<1>(ValueHigh);
			vl=tIntegerComponent::template Get<0>(ValueHigh);
			vlh=tIntegerComponent::template Get<1>(ValueLow);
		}
		else{
			vh=tIntegerComponent::template Get<0>(ValueHigh);
			vl=tIntegerComponent::template Get<1>(ValueLow);
			vlh=tIntegerComponent::template Get<0>(ValueLow);
		}
		vh=TKRuntime::TInteger<IntegerSize/2>::ShiftLeftInto(vh,vl,Count);
		vl=TKRuntime::TInteger<IntegerSize/2>::ShiftLeftInto(vl,vlh,Count);

		RetVal=tIntegerComponent::template Set<1>(0,vh);
		RetVal=tIntegerComponent::template Set<0>(RetVal,vl);
		return RetVal;
	}

	static tUInt ShiftRightInto(tUInt ValueLow,tUInt ValueHigh,ufInt8 Count)noexcept(true){
		tUInt RetVal;
		tUIntH vh,vl,vhl;
		if(Count<IntegerSize/2*ByteBitCount){
			vhl=tIntegerComponent::template Get<0>(ValueHigh);
			vh=tIntegerComponent::template Get<1>(ValueLow);
			vl=tIntegerComponent::template Get<0>(ValueLow);
		}
		else{
			vhl=tIntegerComponent::template Get<1>(ValueHigh);
			vh=tIntegerComponent::template Get<0>(ValueHigh);
			vl=tIntegerComponent::template Get<1>(ValueLow);
		}
		vl=TKRuntime::TInteger<IntegerSize/2>::ShiftRightInto(vl,vh,Count);
		vh=TKRuntime::TInteger<IntegerSize/2>::ShiftRightInto(vh,vhl,Count);

		RetVal=tIntegerComponent::template Set<1>(0,vh);
		RetVal=tIntegerComponent::template Set<0>(RetVal,vl);
		return RetVal;
	}

	static bool BitScanL(ufInt8 &BitIndex,tUInt Src)noexcept(true){
		tUIntH vl=tIntegerComponent::template Get<0>(Src);
		tUIntH vh=tIntegerComponent::template Get<1>(Src);
		
		if(TKRuntime::TInteger<IntegerSize/2>::BitScanL(BitIndex,vl)){
			return true;
		}

		if(TKRuntime::TInteger<IntegerSize/2>::BitScanL(BitIndex,vh)){
			BitIndex+=IntegerSize/2*ByteBitCount;
			return true;
		}
		return false;
	}

	static bool BitScanH(ufInt8 &BitIndex,tUInt Src)noexcept(true){
		tUIntH vl=tIntegerComponent::template Get<0>(Src);
		tUIntH vh=tIntegerComponent::template Get<1>(Src);

		if(TKRuntime::TInteger<IntegerSize/2>::BitScanL(BitIndex,vh)){
			BitIndex+=IntegerSize/2*ByteBitCount;
			return true;
		}

		return TKRuntime::TInteger<IntegerSize/2>::BitScanL(BitIndex,vl);
	}

	static bool BitTest(tUInt Src,ufInt8 BitIndex)noexcept(true){
		tUIntH v;
		if(BitIndex<IntegerSize/2*ByteBitCount){
			v=tIntegerComponent::template Get<0>(Src);
		}
		else{
			v=tIntegerComponent::template Get<1>(Src);
		}
		BitIndex%=IntegerSize/2*ByteBitCount;
		return TKRuntime::TInteger<IntegerSize/2>::BitTest(v,BitIndex);
	}

	static bool BitSet(tUInt &Dest,ufInt8 BitIndex)noexcept(true){
		bool Bit;
		if(BitIndex<IntegerSize/2*ByteBitCount){
			tUIntH v=tIntegerComponent::template Get<0>(Dest);
			BitIndex%=IntegerSize/2*ByteBitCount;
			Bit=TKRuntime::TInteger<IntegerSize/2>::BitSet(v,BitIndex);
			Dest=tIntegerComponent::template Set<0>(Dest,v);

		}
		else{
			tUIntH v=tIntegerComponent::template Get<1>(Dest);
			BitIndex%=IntegerSize/2*ByteBitCount;
			Bit=TKRuntime::TInteger<IntegerSize/2>::BitSet(v,BitIndex);
			Dest=tIntegerComponent::template Set<1>(Dest,v);
		}
		return Bit;
	}

	static bool BitReset(tUInt &Dest,ufInt8 BitIndex)noexcept(true){
		bool Bit;
		if(BitIndex<IntegerSize/2*ByteBitCount){
			tUIntH v=tIntegerComponent::template Get<0>(Dest);
			BitIndex%=IntegerSize/2*ByteBitCount;
			Bit=TKRuntime::TInteger<IntegerSize/2>::BitReset(v,BitIndex);
			Dest=tIntegerComponent::template Set<0>(Dest,v);
		
		}
		else{
			tUIntH v=tIntegerComponent::template Get<1>(Dest);
			BitIndex%=IntegerSize/2*ByteBitCount;
			Bit=TKRuntime::TInteger<IntegerSize/2>::BitReset(v,BitIndex);
			Dest=tIntegerComponent::template Set<1>(Dest,v);
		}
		return Bit;
	}
	static bool BitComplement(tUInt Dest,ufInt8 BitIndex)noexcept(true){
		bool Bit;
		if(BitIndex<IntegerSize/2*ByteBitCount){
			tUIntH v=tIntegerComponent::template Get<0>(Dest);
			BitIndex%=IntegerSize/2*ByteBitCount;
			Bit=TKRuntime::TInteger<IntegerSize/2>::BitComplement(v,BitIndex);
			Dest=tIntegerComponent::template Set<0>(Dest,v);

		}
		else{
			tUIntH v=tIntegerComponent::template Get<1>(Dest);
			BitIndex%=IntegerSize/2*ByteBitCount;
			Bit=TKRuntime::TInteger<IntegerSize/2>::BitComplement(v,BitIndex);
			Dest=tIntegerComponent::template Set<1>(Dest,v);
		}
		return Bit;
	}

	static tUInt RotateLeft(tUInt Src,ufInt8 Count)noexcept(true){
		tUIntH vl=tIntegerComponent::template Get<0>(Src);
		tUIntH vh=tIntegerComponent::template Get<1>(Src);

		if(Count>=IntegerSize/2*ByteBitCount){
			Count-=IntegerSize/2*ByteBitCount;
			tUIntH t=vl;
			vl=vh;
			vh=t;
		}

		tUIntH h=TKRuntime::TInteger<IntegerSize/2>::ShiftLeftInto(vh,vl,Count);
		tUIntH l=TKRuntime::TInteger<IntegerSize/2>::ShiftLeftInto(vl,vh,Count);
		tUInt RetVal=tIntegerComponent::template Set<1>(0,h);
		return tIntegerComponent::template Set<0>(RetVal,l);
	}
	static tUInt RotateRight(tUInt Src,ufInt8 Count)noexcept(true){
		tUIntH vl=tIntegerComponent::template Get<0>(Src);
		tUIntH vh=tIntegerComponent::template Get<1>(Src);

		if(Count>=IntegerSize/2*ByteBitCount){
			Count-=IntegerSize/2*ByteBitCount;
			tUIntH t=vl;
			vl=vh;
			vh=t;
		}

		tUIntH l=TKRuntime::TInteger<IntegerSize/2>::ShiftRightInto(vl,vh,Count);
		tUIntH h=TKRuntime::TInteger<IntegerSize/2>::ShiftRightInto(vh,vl,Count);
		tUInt RetVal=tIntegerComponent::template Set<1>(0,h);
		return tIntegerComponent::template Set<0>(RetVal,l);
	}

};
//---------------------------------------------------------------------------
}	// namespace CPPRuntime
//---------------------------------------------------------------------------
namespace cnMemory{
//---------------------------------------------------------------------------
template<class TComponent,ufInt8 Index,class TInteger>
inline TComponent IntegerGetComponent(TInteger Integer)noexcept(true)
{
	return static_cast<TComponent>(
		TKRuntime::TIntegerComponent<cnVar::TSelect<1,TInteger,TKRuntime::NativeByteOrder>::Type::Value,sizeof(TInteger),sizeof(TComponent)>::template Get<Index>(static_cast<typename cnVar::TIntegerOfSize<sizeof(TInteger),false>::Type>(Integer))
	);
}

template<class TComponent,ufInt8 Index,class TInteger>
inline TInteger IntegerSetComponent(TInteger Integer,TComponent Component)noexcept(true)
{
	return static_cast<TInteger>(TKRuntime::TIntegerComponent<cnVar::TSelect<1,TInteger,TKRuntime::NativeByteOrder>::Type::Value,sizeof(TInteger),sizeof(TComponent)>::template Set<Index>(
		static_cast<typename cnVar::TIntegerOfSize<sizeof(TInteger),false>::Type>(Integer),static_cast<typename cnVar::TIntegerOfSize<sizeof(TComponent),false>::Type>(Component))
	);
}
//---------------------------------------------------------------------------
}	// namespace cnMemory
//---------------------------------------------------------------------------
namespace TKRuntime{
//---------------------------------------------------------------------------

template<uIntn IntSize>
struct TMultiprecisionInteger;

// TMultiprecisionInteger
//{
//	static bool UAdd(tUInt &Result,tUInt Augend,tUInt Addend,bool Carry)noexcept(true);
//	static bool SAdd(tSInt &Result,tSInt Augend,tSInt Addend,bool Carry)noexcept(true);
//	static bool USub(tUInt &Result,tUInt Minuend,tUInt Subtrahend,bool Borrow)noexcept(true);
//	static bool SSub(tSInt &Result,tSInt Minuend,tSInt Subtrahend,bool Borrow)noexcept(true);
//	static tUInt UMul(tUInt &ResultHigh,tUInt Multiplicand,tUInt Multiplier)noexcept(true);
//	static tUInt SMul(tSInt &ResultHigh,tSInt Multiplicand,tSInt Multiplier)noexcept(true);
//	static tUInt UMulH(tUInt Multiplicand,tUInt Multiplier)noexcept(true);
//	static tSInt SMulH(tSInt Multiplicand,tSInt Multiplier)noexcept(true);
//	static tUInt UDiv(tUInt &Remainder,tUInt Dividend_Lo,tUInt Dividend_Hi,tUInt Divisor)noexcept(true);
//	static tSInt SDiv(tSInt &Remainder,tUInt Dividend_Lo,tSInt Dividend_Hi,tSInt Divisor)noexcept(true);
//	static tUInt UMulDiv(tUInt &Remainder,tUInt Multiplicand,tUInt Multiplier,tUInt Divisor)noexcept(true);
//	static tSInt SMulDiv(tSInt &Remainder,tSInt Multiplicand,tSInt Multiplier,tSInt Divisor)noexcept(true);
//};

//---------------------------------------------------------------------------
}	// namespace TKRuntime
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
namespace cnLib_THelper{
namespace CPPRT_TH{

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
			dt=TKRuntime::TInteger<IntSize>::ShiftLeftInto(Dividend_Hi,Dividend_Lo,dScale);
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
		dt=TKRuntime::TInteger<IntSize>::ShiftLeftInto(dh,dl,hiBitCount+dScale);
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

}	// namespace CPPRT_TH
}	// namespace cnLib_THelper
	//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace CPPRuntime{
//---------------------------------------------------------------------------
template<uIntn IntSize>
struct TMultiprecisionInteger
	: cnLib_THelper::CPPRT_TH::TMultiprecisionInteger_AddSub<IntSize>
	, cnLib_THelper::CPPRT_TH::TMultiprecisionInteger_MulDiv<IntSize,(IntSize<cnVar::MaxIntegerSize)>
{
	typedef typename cnVar::TIntegerOfSize<IntSize,false>::Type tUInt;
	typedef typename cnVar::TIntegerOfSize<IntSize,true>::Type tSInt;
};
//---------------------------------------------------------------------------
}   // namespace CPPRuntime
	//---------------------------------------------------------------------------
namespace cnMath{
//---------------------------------------------------------------------------


// MPUAdd
//	Perform unsigned data additive
// [out]Result	result
// [in]Augend
// [in]Addend
// return: carry sign
template<class T>
inline bool MPUAdd(T &Result,typename cnVar::TTypeDef<T>::Type Augend,typename cnVar::TTypeDef<T>::Type Addend,bool Carry=false)noexcept(true)
{
	typedef typename cnVar::TIntegerOfSize<sizeof(T),false>::Type tUInt;

	return TKRuntime::TMultiprecisionInteger<sizeof(T)>::UAdd(
		reinterpret_cast<tUInt&>(Result),
		reinterpret_cast<tUInt&>(Augend),
		reinterpret_cast<tUInt&>(Addend),
		Carry
	);
}

// MPSAdd
//	Perform signed data additive
// [out]Result	result
// [in]Augend
// [in]Addend
// return: carry sign
template<class T>
inline bool MPSAdd(T &Result,typename cnVar::TTypeDef<T>::Type Augend,typename cnVar::TTypeDef<T>::Type Addend)noexcept(true)
{
	typedef typename cnVar::TIntegerOfSize<sizeof(T),true>::Type tSInt;

	return TKRuntime::TMultiprecisionInteger<sizeof(T)>::SAdd(
		reinterpret_cast<tSInt&>(Result),
		reinterpret_cast<tSInt&>(Augend),
		reinterpret_cast<tSInt&>(Addend),
		false
	);
}

// MPUSub
//	Perform a unsigned data subtration
// [out]Result	result
// [in]Minuend
// [in]Subtrahend
// return: carry sign
template<class T>
inline bool MPUSub(T &Result,typename cnVar::TTypeDef<T>::Type Minuend,typename cnVar::TTypeDef<T>::Type Subtrahend,bool Carry=false)noexcept(true)
{
	typedef typename cnVar::TIntegerOfSize<sizeof(T),false>::Type tUInt;

	return TKRuntime::TMultiprecisionInteger<sizeof(T)>::USub(
		reinterpret_cast<tUInt&>(Result),
		reinterpret_cast<tUInt&>(Minuend),
		reinterpret_cast<tUInt&>(Subtrahend),
		Carry
	);
}


// MPSSub
//	Perform a signed data subtration
// [out]Result	result
// [in]Minuend
// [in]Subtrahend
// return: carry sign
template<class T>
inline bool MPSSub(T &Result,typename cnVar::TTypeDef<T>::Type Minuend,typename cnVar::TTypeDef<T>::Type Subtrahend,bool Carry=false)noexcept(true)
{
	typedef typename cnVar::TIntegerOfSize<sizeof(T),true>::Type tSInt;

	return TKRuntime::TMultiprecisionInteger<sizeof(T)>::SSub(
		reinterpret_cast<tSInt&>(Result),
		reinterpret_cast<tSInt&>(Minuend),
		reinterpret_cast<tSInt&>(Subtrahend),
		Carry
	);
}

// MPUMul
//	Perform unsigend data multiplication
// [out]ResultHigh	high part of result
// [in]Multiplicand
// [in]Multiplier
// return: low part of result
template<class T>
inline T MPUMul(T &ResultHigh,typename cnVar::TTypeDef<T>::Type Multiplicand,typename cnVar::TTypeDef<T>::Type Multiplier)noexcept(true)
{
	typedef typename cnVar::TIntegerOfSize<sizeof(T),false>::Type tUInt;

	return cnVar::ReturnCast<T>(
		TKRuntime::TMultiprecisionInteger<sizeof(T)>::UMul(
			reinterpret_cast<tUInt&>(ResultHigh),
			reinterpret_cast<tUInt&>(Multiplicand),
			reinterpret_cast<tUInt&>(Multiplier)
		)
		);
}

// MPSMul
//	Perform signed data multiplication
// [out]ResultHigh	high part of result
// [in]Multiplicand
// [in]Multiplier
// return: low part of result, unsigned
template<class T>
inline typename cnVar::TIntegerOfSize<sizeof(T),false>::Type MPSMul(T &ResultHigh,typename cnVar::TTypeDef<T>::Type Multiplicand,typename cnVar::TTypeDef<T>::Type Multiplier)noexcept(true)
{
	typedef typename cnVar::TIntegerOfSize<sizeof(T),true>::Type tSInt;

	return TKRuntime::TMultiprecisionInteger<sizeof(T)>::SMul(
		reinterpret_cast<tSInt&>(ResultHigh),
		reinterpret_cast<tSInt&>(Multiplicand),
		reinterpret_cast<tSInt&>(Multiplier)
	);
}

// MPUMulH
//	Perform unsigend data multiplication
// [in]Multiplicand
// [in]Multiplier
// return: high part of result
template<class T>
inline T MPUMulH(T Multiplicand,typename cnVar::TTypeDef<T>::Type Multiplier)noexcept(true)
{
	typedef typename cnVar::TIntegerOfSize<sizeof(T),false>::Type tUInt;

	return cnVar::ReturnCast<T>(
		TKRuntime::TMultiprecisionInteger<sizeof(T)>::UMulH(
			reinterpret_cast<tUInt&>(Multiplicand),
			reinterpret_cast<tUInt&>(Multiplier)
		)
		);
}

// MPUDiv
//	Perform unsigned data division
// [out]Remainder	remainder
// [in]Dividend_Lo	low part of Dividend,unsigned
// [in]Dividend_Hi	high part of Dividend
// [in]Divisor
//	return: quotient
template<class T>
inline T MPUDiv(T &Remainder,typename cnVar::TTypeDef<T>::Type Dividend_Lo,typename cnVar::TTypeDef<T>::Type Dividend_Hi,typename cnVar::TTypeDef<T>::Type Divisor)noexcept(true)
{
	cnLib_ASSERT(Divisor!=0);

	typedef typename cnVar::TIntegerOfSize<sizeof(T),false>::Type tUInt;

	return cnVar::ReturnCast<T>(
		TKRuntime::TMultiprecisionInteger<sizeof(T)>::UDiv(
			reinterpret_cast<tUInt&>(Remainder),
			reinterpret_cast<tUInt&>(Dividend_Lo),
			reinterpret_cast<tUInt&>(Dividend_Hi),
			reinterpret_cast<tUInt&>(Divisor)
		)
		);
}

// MPSDiv
//	Perform signed data division
// [out]Remainder	remainder
// [in]Dividend_Lo	low part of Dividend,unsigned
// [in]Dividend_Hi	high part of Dividend
// [in]Divisor
//	return: quotient
template<class T>
inline T MPSDiv(T &Remainder,typename cnVar::TIntegerOfSize<sizeof(T),false>::Type Dividend_Lo,typename cnVar::TTypeDef<T>::Type Dividend_Hi,typename cnVar::TTypeDef<T>::Type Divisor)noexcept(true)
{
	cnLib_ASSERT(Divisor!=0);

	typedef typename cnVar::TIntegerOfSize<sizeof(T),true>::Type tSInt;

	return cnVar::ReturnCast<T>(
		TKRuntime::TMultiprecisionInteger<sizeof(T)>::SDiv(
			reinterpret_cast<tSInt&>(Remainder),
			Dividend_Lo,
			reinterpret_cast<tSInt&>(Dividend_Hi),
			reinterpret_cast<tSInt&>(Divisor)
		)
		);
}

// MPUMulDiv
//	Perform unsigned data multiplication then division 
//	if result execces the max value of uIntn, it may cause an system exception
// [out]Remainder		remainder
// [in]Multiplicand
// [in]Multiplier
// [in]Divisor
// return: quotient
template<class T>
inline T MPUMulDiv(T &Remainder,typename cnVar::TTypeDef<T>::Type Multiplicand,typename cnVar::TTypeDef<T>::Type Multiplier,typename cnVar::TTypeDef<T>::Type Divisor)noexcept(true)
{
	cnLib_ASSERT(Divisor!=0);

	typedef typename cnVar::TIntegerOfSize<sizeof(T),false>::Type tUInt;

	return cnVar::ReturnCast<T>(
		TKRuntime::TMultiprecisionInteger<sizeof(T)>::UMulDiv(
			reinterpret_cast<tUInt&>(Remainder),
			reinterpret_cast<tUInt&>(Multiplicand),
			reinterpret_cast<tUInt&>(Multiplier),
			reinterpret_cast<tUInt&>(Divisor)
		)
		);
}

// MPSMulDiv
//	Perform signed data multiplication then division 
//	if result execces the max value of uIntn, it may cause an system exception
// [out]Remainder		remainder
// [in]Multiplicand
// [in]Multiplier
// [in]Divisor
// return: quotient
template<class T>
inline T MPSMulDiv(T &Remainder,typename cnVar::TTypeDef<T>::Type Multiplicand,typename cnVar::TTypeDef<T>::Type Multiplier,typename cnVar::TTypeDef<T>::Type Divisor)noexcept(true)
{
	cnLib_ASSERT(Divisor!=0);

	typedef typename cnVar::TIntegerOfSize<sizeof(T),true>::Type tSInt;

	return cnVar::ReturnCast<T>(
		TKRuntime::TMultiprecisionInteger<sizeof(T)>::SMulDiv(
			reinterpret_cast<tSInt&>(Remainder),
			reinterpret_cast<tSInt&>(Multiplicand),
			reinterpret_cast<tSInt&>(Multiplier),
			reinterpret_cast<tSInt&>(Divisor)
		)
		);
}


template<class T>
inline T MPUPowMod(T Base,uIntn Exponent,typename cnVar::TTypeDef<T>::Type Divisor)noexcept(true)
{
	typedef typename cnVar::TIntegerOfSize<sizeof(T),false>::Type tUInt;
	typedef typename TKRuntime::TMultiprecisionInteger<sizeof(T)> tMPInteger;
	tUInt r;
	tUInt b=static_cast<tUInt>(Base);
	tUInt d=static_cast<tUInt>(Divisor);
	if(Exponent&1){
		tMPInteger::UDiv(r,b,0,d);
	}
	else{
		r=1;
	}
	Exponent>>=1;
	while(Exponent!=0){
		tMPInteger::UMulDiv(b,b,b,d);

		if(Exponent&1){
			tMPInteger::UMulDiv(r,r,b,d);
		}

		Exponent>>=1;
	};
	return static_cast<T>(r);
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

