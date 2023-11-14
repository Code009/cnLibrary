/*- cnLibCRT - CRT --------------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2005-11-20                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnRTL_CRTMath_H__
#define __cnLibrary_cnRTL_CRTMath_H__
/*-------------------------------------------------------------------------*/
#include <cnRTL/CRTCoreCommon.h>
#ifdef __cplusplus

//---------------------------------------------------------------------------
#if cnLibrary_CPPFEATURE_CONSTEXPR >= 200704L
#define	cnRTL_CONSTEXPR_FUNC	constexpr
#define	cnRTL_CONSTVAR	constexpr
// cnLibrary_CPPFEATURE_CONSTEXPR >= 200704L
#else
// cnLibrary_CPPFEATURE_CONSTEXPR < 200704L
#define	cnRTL_CONSTEXPR_FUNC
#define	cnRTL_CONSTVAR	const
#endif // cnLibrary_CPPFEATURE_CONSTEXPR < 200704L
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnMath{

template<class TFloat>
struct CMath;

template<>
struct CMath<float>
{
	static float fabs(float n){				return ::fabsf(n);	}
	static float round(float n){			return ::roundf(n);	}
	static float trunc(float n){			return ::truncf(n);	}
	static float floor(float n){			return ::floorf(n);	}
	static float ceil(float n){				return ::ceilf(n);	}

	static float fmod(float x,float y){		return ::fmodf(x,y);	}
	static float modf(float n,float *e){	return ::modff(n,e);	}

	static float sqrt(float n){				return ::sqrtf(n);	}
	static float log(float n){				return ::logf(n);	}
	static float log2(float n){				return ::log2f(n);	}
	static float exp(float n){				return ::expf(n);	}
	static float exp2(float n){				return ::exp2f(n);	}
	static float pow(float b,float e){		return ::powf(b,e);	}

	static float sin(float n){				return ::sinf(n);	}
	static float cos(float n){				return ::cosf(n);	}
	static float tan(float n){				return ::tanf(n);	}
	static float asin(float n){				return ::asinf(n);	}
	static float acos(float n){				return ::acosf(n);	}
	static float atan(float n){				return ::atanf(n);	}
	static float atan2(float y,float x){	return ::atan2f(y,x);	}
};

template<>
struct CMath<double>
{
	static double fabs(double n){				return ::fabs(n);	}
	static double round(double n){				return ::round(n);	}
	static double trunc(double n){				return ::trunc(n);	}
	static double floor(double n){				return ::floor(n);	}
	static double ceil(double n){				return ::ceil(n);	}

	static double fmod(double x,double y){		return ::fmod(x,y);	}
	static double modf(double n,double *e){		return ::modf(n,e);	}

	static double sqrt(double n){				return ::sqrt(n);	}
	static double log(double n){				return ::log(n);	}
	static double log2(double n){				return ::log2(n);	}
	static double exp(double n){				return ::exp(n);	}
	static double exp2(double n){				return ::exp2(n);	}
	static double pow(double b,double e){		return ::pow(b,e);	}

	static double sin(double n){				return ::sin(n);	}
	static double cos(double n){				return ::cos(n);	}
	static double tan(double n){				return ::tan(n);	}
	static double asin(double n){				return ::asin(n);	}
	static double acos(double n){				return ::acos(n);	}
	static double atan(double n){				return ::atan(n);	}
	static double atan2(double y,double x){		return ::atan2(y,x);	}
};


template<class T>
inline typename cnVar::TFloatConversion<T>::tMatch FloatAbsolute(const T &n)noexcept(true){
	typedef typename cnVar::TFloatConversion<T>::tMatch tMatch;
	return CMath<tMatch>::fabs(cnVar::TFloatConversion<T>::template Cast<tMatch>(n));
}

template<class T>
inline typename cnVar::TFloatConversion<T>::tMatch FloatRoundNearest(const T &n)noexcept(true){
	typedef typename cnVar::TFloatConversion<T>::tMatch tMatch;
	return CMath<tMatch>::round(cnVar::TFloatConversion<T>::template Cast<tMatch>(n));
}
template<class T>
inline typename cnVar::TFloatConversion<T>::tMatch FloatRoundTruncate(const T &n)noexcept(true){
	typedef typename cnVar::TFloatConversion<T>::tMatch tMatch;
	return CMath<tMatch>::trunc(cnVar::TFloatConversion<T>::template Cast<tMatch>(n));
}
template<class T>
inline typename cnVar::TFloatConversion<T>::tMatch FloatRoundNegative(const T &n)noexcept(true){
	typedef typename cnVar::TFloatConversion<T>::tMatch tMatch;
	return CMath<tMatch>::floor(cnVar::TFloatConversion<T>::template Cast<tMatch>(n));
}
template<class T>
inline typename cnVar::TFloatConversion<T>::tMatch FloatRoundPositive(const T &n)noexcept(true){
	typedef typename cnVar::TFloatConversion<T>::tMatch tMatch;
	return CMath<tMatch>::ceil(cnVar::TFloatConversion<T>::template Cast<tMatch>(n));
}


template<class T>
inline typename cnVar::TFloatConversion<T>::tMatch FloatModulo(const T &Dividend,typename cnVar::TTypeDef<T>::Type Divisor)noexcept(true){
	typedef typename cnVar::TFloatConversion<T>::tMatch tMatch;
	return CMath<tMatch>::fmod(cnVar::TFloatConversion<T>::template Cast<tMatch>(Dividend),cnVar::TFloatConversion<T>::template Cast<tMatch>(Divisor));
}


template<class T>
inline typename cnVar::TFloatConversion<T>::tMatch FloatSquareRoot(const T &n)noexcept(true){
	typedef typename cnVar::TFloatConversion<T>::tMatch tMatch;
	return CMath<tMatch>::sqrt(cnVar::TFloatConversion<T>::template Cast<tMatch>(n));
}

template<class T>
inline typename cnVar::TFloatConversion<T>::tMatch FloatNaturalLogarithm(const T &n)noexcept(true){
	typedef typename cnVar::TFloatConversion<T>::tMatch tMatch;
	return CMath<tMatch>::log(cnVar::TFloatConversion<T>::template Cast<tMatch>(n));
}

template<class T>
inline typename cnVar::TFloatConversion<T>::tMatch FloatBinaryLogarithm(const T &n)noexcept(true){
	typedef typename cnVar::TFloatConversion<T>::tMatch tMatch;
	return CMath<tMatch>::log2(cnVar::TFloatConversion<T>::template Cast<tMatch>(n));
}

template<class T>
inline typename cnVar::TFloatConversion<T>::tMatch FloatNaturalExponential(const T &n)noexcept(true){
	typedef typename cnVar::TFloatConversion<T>::tMatch tMatch;
	return CMath<tMatch>::exp(cnVar::TFloatConversion<T>::template Cast<tMatch>(n));
}

template<class T>
inline typename cnVar::TFloatConversion<T>::tMatch FloatBinaryExponential(const T &n)noexcept(true){
	typedef typename cnVar::TFloatConversion<T>::tMatch tMatch;
	return CMath<tMatch>::exp2(cnVar::TFloatConversion<T>::template Cast<tMatch>(n));
}

template<class T>
inline typename cnVar::TFloatConversion<T>::tMatch FloatPower(const T &Base,typename cnVar::TTypeDef<T>::Type Exponent)noexcept(true){
	typedef typename cnVar::TFloatConversion<T>::tMatch tMatch;
	return CMath<tMatch>::pow(cnVar::TFloatConversion<T>::template Cast<tMatch>(Base),cnVar::TFloatConversion<T>::template Cast<tMatch>(Exponent));
}

template<class T>
inline typename cnVar::TFloatConversion<T>::tMatch FloatSine(const T &n)noexcept(true){
	typedef typename cnVar::TFloatConversion<T>::tMatch tMatch;
	return CMath<tMatch>::sin(cnVar::TFloatConversion<T>::template Cast<tMatch>(n));
}
template<class T>
inline typename cnVar::TFloatConversion<T>::tMatch FloatCosine(const T &n)noexcept(true){
	typedef typename cnVar::TFloatConversion<T>::tMatch tMatch;
	return CMath<tMatch>::cos(cnVar::TFloatConversion<T>::template Cast<tMatch>(n));
}
template<class T>
inline typename cnVar::TFloatConversion<T>::tMatch FloatTangent(const T &n)noexcept(true){
	typedef typename cnVar::TFloatConversion<T>::tMatch tMatch;
	return CMath<tMatch>::tan(cnVar::TFloatConversion<T>::template Cast<tMatch>(n));
}

template<class T>
inline typename cnVar::TFloatConversion<T>::tMatch FloatArcSine(const T &n)noexcept(true){
	typedef typename cnVar::TFloatConversion<T>::tMatch tMatch;
	return CMath<tMatch>::asin(cnVar::TFloatConversion<T>::template Cast<tMatch>(n));
}
template<class T>
inline typename cnVar::TFloatConversion<T>::tMatch FloatArcCosine(const T &n)noexcept(true){
	typedef typename cnVar::TFloatConversion<T>::tMatch tMatch;
	return CMath<tMatch>::acos(cnVar::TFloatConversion<T>::template Cast<tMatch>(n));
}
template<class T>
inline typename cnVar::TFloatConversion<T>::tMatch FloatArcTangent(const T &n)noexcept(true){
	typedef typename cnVar::TFloatConversion<T>::tMatch tMatch;
	return CMath<tMatch>::atan(cnVar::TFloatConversion<T>::template Cast<tMatch>(n));
}
template<class T>
inline typename cnVar::TFloatConversion<T>::tMatch FloatArcTangentFloatArcTangent2(const T &y,typename cnVar::TTypeDef<T>::Type x)noexcept(true){
	typedef typename cnVar::TFloatConversion<T>::tMatch tMatch;
	return CMath<tMatch>::atan2(cnVar::TFloatConversion<T>::template Cast<tMatch>(y),cnVar::TFloatConversion<T>::template Cast<tMatch>(x));
}


}   // namespace cnMath
//---------------------------------------------------------------------------
namespace cnString{
//---------------------------------------------------------------------------
template<ufInt8 Radix,class T>
ufInt16 ConvertIntegerToDigitsRadix(T Value,ufInt8 *DigitBuffer,ufInt16 DigitBufferIndex)
{
	if(Value==0)
		return DigitBufferIndex;
	if(DigitBufferIndex==0)
		return 0;
	DigitBufferIndex--;
	while(Value>=Radix){
		DigitBuffer[DigitBufferIndex]=static_cast<ufInt8>(Value%Radix);
		Value/=Radix;
		if(DigitBufferIndex==0)
			return 0;
		DigitBufferIndex--;
	}
	DigitBuffer[DigitBufferIndex]=static_cast<ufInt8>(Value);
	return DigitBufferIndex;
}
//---------------------------------------------------------------------------
template<class T>
inline ufInt16 ConvertIntegerToDigits(T Value,ufInt8 Radix,ufInt8 *DigitBuffer,ufInt16 DigitBufferIndex)
{
	typedef typename cnVar::TIntegerOfSize<sizeof(T),false>::Type TUInt;
	cnMath::Abs(Value);
	switch(Radix){
	case 0:
	case 1:
	case 10:
		return ConvertIntegerToDigitsRadix<10,TUInt>(static_cast<TUInt>(Value),DigitBuffer,DigitBufferIndex);
	case 2:
		return ConvertIntegerToDigitsRadix<2,TUInt>(static_cast<TUInt>(Value),DigitBuffer,DigitBufferIndex);
	case 4:
		return ConvertIntegerToDigitsRadix<4,TUInt>(static_cast<TUInt>(Value),DigitBuffer,DigitBufferIndex);
	case 8:
		return ConvertIntegerToDigitsRadix<8,TUInt>(static_cast<TUInt>(Value),DigitBuffer,DigitBufferIndex);
	case 16:
		return ConvertIntegerToDigitsRadix<16,TUInt>(static_cast<TUInt>(Value),DigitBuffer,DigitBufferIndex);
	}

	TUInt ConvertValue=static_cast<TUInt>(Value);
	if(ConvertValue==0)
		return DigitBufferIndex;
	if(DigitBufferIndex==0)
		return 0;
	DigitBufferIndex--;
	while(ConvertValue>=Radix){
		DigitBuffer[DigitBufferIndex]=static_cast<ufInt8>(ConvertValue%Radix);
		ConvertValue/=Radix;
		if(DigitBufferIndex==0)
			return 0;
		DigitBufferIndex--;
	}
	DigitBuffer[DigitBufferIndex]=static_cast<ufInt8>(ConvertValue);
	return DigitBufferIndex;
}
//---------------------------------------------------------------------------
template<class T>
ufInt16 ConvertFloatSignificandToDigits(T Value,ufInt8 Radix,ufInt8 *DigitBuffer,uIntn DigitBufferLength,sfInt16 &Exponent)
{
	Value=::fabs(Value);
	if(Value==0){
		Exponent=0;
		return 0;
	}

	T RadixFactor=Radix;
	T log2r=cnMath::CMath<T>::log(static_cast<T>(RadixFactor))/cnMath::TFloatConstant<T>::loge2;
	uIntn MaxSignificand=static_cast<uIntn>(sizeof(T)*ByteBitCount/log2r);
	if(DigitBufferLength>MaxSignificand)
		DigitBufferLength=MaxSignificand;

	int e2;
	::frexp(Value,&e2);

	T IntValue;
	ufInt16 DigitIndex=0;
	if(e2>0){
		Value=cnMath::CMath<T>::modf(Value,&IntValue);
		T Scale=e2/log2r;
		uIntn ScaleExp=static_cast<uIntn>(Scale);
		if(ScaleExp>MaxSignificand){
			// scale down
			ScaleExp-=MaxSignificand;
			Scale=static_cast<T>(cnMath::CMath<T>::pow(RadixFactor,static_cast<T>(ScaleExp)));
			IntValue/=Scale;
		}
		else{
			ScaleExp=0;
		}
		ufInt8 IntDigits[sizeof(T)*ByteBitCount];
		uIntn IntDigitIndex=ConvertIntegerToDigits(static_cast<typename cnVar::TIntegerOfSize<sizeof(T),false>::Type>(IntValue),Radix,IntDigits,sizeof(T)*ByteBitCount);
		uIntn IntDigitCount=sizeof(T)*ByteBitCount-IntDigitIndex;
		Exponent=static_cast<sfInt16>(IntDigitCount+ScaleExp);
		ufInt8 *CopyDigits=IntDigits+IntDigitIndex;
		while(IntDigitCount>0){
			IntDigitCount--;
			DigitBuffer[DigitIndex++]=*CopyDigits++;
			if(DigitIndex>=DigitBufferLength)
				return static_cast<ufInt16>(DigitBufferLength);
		}
	}
	else{
		T Scale=(-e2)/log2r;
		uIntn ScaleExp=static_cast<uIntn>(Scale);
		Scale=static_cast<T>(cnMath::CMath<T>::pow(RadixFactor,static_cast<T>(ScaleExp)));
		Value*=Scale;
		sfInt16 e=static_cast<sfInt16>(ScaleExp);
		e=-e;
		T NextValue=Value*RadixFactor;
		while(NextValue<1.){ 
			Value=NextValue;
			e--;
			NextValue=Value*RadixFactor;
		}
		Exponent=e;
	}

	while(Value!=0){
		Value*=RadixFactor;
		Value=cnMath::CMath<T>::modf(Value,&IntValue);
		DigitBuffer[DigitIndex++]=static_cast<ufInt8>(IntValue);
		if(DigitIndex>=DigitBufferLength)
			return static_cast<ufInt16>(DigitBufferLength);
	}
	return DigitIndex;
}
//---------------------------------------------------------------------------
}	// namespace cnString
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#undef	cnRTL_CONSTVAR
#undef	cnRTL_CONSTEXPR_FUNC
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif

