/*- cnTK - String Function ------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2005-01-12                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnTK_String_H__
#define __cnLibrary_cnTK_String_H__
/*-------------------------------------------------------------------------*/
#include <cnTK/Common.hpp>
#include <cnTK/Interface.hpp>
#include <cnTK/TypeInfo.hpp>
#include <cnTK/Memory.hpp>
#include <cnTK/Atomic.hpp>
#include <cnTK/Array.hpp>
#include <cnTK/Stream.hpp>
/*-------------------------------------------------------------------------*/
#if	cnLibrary_CPPFEATURELEVEL >= 1
//---------------------------------------------------------------------------
#include <cnTK/TKMacrosDeclare.inc>
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnString{
//---------------------------------------------------------------------------
	static cnLib_CONSTVAR uIntn StringMaxLength=cnVar::TIntegerValue<uIntn>::Max;
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
		if(*Src==0){
			break;
		}
		++Src;
		*Dest=*Src;
		++Dest;
	}
	*Dest=0;
	return Index;
}

// ViewSearch
//	Search element in Src
// [in]Src			pointer to array to search
// Length			number of elements to look into
// Comparison		Comparison function, check array element if it matches
//					prototype: bool (const T &Element);
//					return true if matches, false if unmatch
// Return:	the index of the first matched data, or -1 if the Dest not found
template<class TPtr,class TComparison>
inline uIntn ViewSearch(TPtr Src,uIntn MaxLength,TComparison cnLib_UREF Comparison)noexcept(cnLib_NOEXCEPTEXPR(Comparison(*Src)))
{
	for(uIntn Index=0;Index<MaxLength;Index++){
		if(*Src==0)
			break;
		if(Comparison(*Src))
			return Index;

		++Src;
	}
	return IndexNotFound;
}


// ViewEqual
//	test if string1 equal to string2
//	[in]Str1	string to subtract
//	[in]Str2	string to be subtracted
//	return:	0 if matched, 1 if Str1>Str2 , -1 if Str1<Str2
template<class TPtr1,class TPtr2>
inline bool ViewEqual(TPtr1 Str1,TPtr2 Str2,uIntn MaxLength)noexcept(cnLib_NOEXCEPTEXPR(*Str1==*Str2) && cnLib_NOEXCEPTEXPR(*Str1==0))
{
	for(uIntn i=0;i<MaxLength;i++){
		if(!(*Str1==*Str2)){
			return false;
		}
		if(*Str1==0){
			return true;
		}
		++Str1;
		++Str2;
	}
	return true;
}

// ViewEqual
//	test if string1 equal to string2
//	[in]Str1	string to subtract
//	[in]Str2	string to be subtracted
//	return:	0 if matched, 1 if Str1>Str2 , -1 if Str1<Str2
template<class TPtr1,class TPtr2>
inline bool ViewEqual(TPtr1 Str1,TPtr2 Str2,uIntn MaxLength,uIntn &MatchLength)noexcept(cnLib_NOEXCEPTEXPR(*Str1==*Str2) && cnLib_NOEXCEPTEXPR(*Str1==0))
{
	for(uIntn i=0;i<MaxLength;i++){
		if(!(*Str1==*Str2)){
			MatchLength=i;
			return false;
		}
		if(*Str1==0){
			MatchLength=i;
			return true;
		}
		++Str1;
		++Str2;
	}
	MatchLength=MaxLength;
	return true;
}

// ViewCompare
//	The encoding relation of string1 to string2
//	[in]Str1	string to subtract
//	[in]Str2	string to be subtracted
//	return:	0 if matched, 1 if Str1>Str2 , -1 if Str1<Str2
template<class TPtr1,class TPtr2>
inline sfInt8 ViewCompare(TPtr1 Str1,TPtr2 Str2,uIntn MaxLength)noexcept(cnLib_NOEXCEPTEXPR(*Str1<*Str2) && cnLib_NOEXCEPTEXPR(*Str1==*Str2) && cnLib_NOEXCEPTEXPR(*Str1==0))
{
	for(uIntn i=0;i<MaxLength;i++){
		if(!(*Str1==*Str2)){
			return *Str1<*Str2?-1:1;
		}
		else if(*Str1==0){
			return 0;
		}
		++Str1;
		++Str2;
	}
	return 0;
}

// ViewCompare
//	The encoding relation of string1 to string2
//	[in]Str1	string to subtract
//	[in]Str2	string to be subtracted
//	return:	0 if matched, 1 if Str1>Str2 , -1 if Str1<Str2
template<class TPtr1,class TPtr2>
inline sfInt8 ViewCompare(TPtr1 Str1,TPtr2 Str2,uIntn MaxLength,uIntn &MatchLength)noexcept(cnLib_NOEXCEPTEXPR(*Str1<*Str2) && cnLib_NOEXCEPTEXPR(*Str1==*Str2) && cnLib_NOEXCEPTEXPR(*Str1==0))
{
	for(uIntn i=0;i<MaxLength;i++){
		if(!(*Str1==*Str2)){
			MatchLength=i;
			return *Str1<*Str2?-1:1;
		}
		else if(*Str1==0){
			MatchLength=i;
			return 0;
		}
		++Str1;
		++Str2;
	}
	MatchLength=MaxLength;
	return 0;
}

template<class TPtr1,class TPtr2>
inline sfInt8 ViewCompare(TPtr1 Str1,uIntn Str1MaxLength,TPtr2 Str2,uIntn Str2MaxLength)noexcept(true)
{
	if(Str1MaxLength==Str2MaxLength){
		return ViewCompare(Str1,Str2,Str1MaxLength);
	}
	uIntn CompareLength;
	sfInt8 MatchResult;
	if(Str1MaxLength<Str2MaxLength){
		CompareLength=Str1MaxLength;
		MatchResult=-1;
	}
	else{
		CompareLength=Str2MaxLength;
		MatchResult=1;
	}
	uIntn MatchLength;
	sfInt8 r=ViewCompare(Str1,Str2,CompareLength,MatchLength);
	if(MatchLength==CompareLength){
		return MatchResult;
	}
	return r;
}
//---------------------------------------------------------------------------
} 	// namespace cnString
//---------------------------------------------------------------------------
namespace TKRuntime{
//---------------------------------------------------------------------------
template<uIntn CharacterSize>	
struct TString;
//{
//	static uIntn FindLength(const void *String,uIntn MaxLength)noexcept;
//	static void Copy(void *Dest,uIntn DestLength,const void *Src)noexcept;
//	static uIntn Search(const void *String,uIntn MaxLength,TInt Character)noexcept;
//	static bool Equal(const void *Str1,const void *Str2,uIntn MaxLength,uIntn &MatchLength)noexcept;
//	static sfInt8 Compare(const void *Str1,const void *Str2,uIntn MaxLength,uIntn &MatchLength)noexcept;
//};

template<uIntn ValueSize,ufInt8 Radix>
struct TStringConvertInteger;
//{
//	static uIntn ToDigits(tUInt Value,ufInt8 *DigitBuffer,uIntn DigitBufferIndex)noexcept;
//	static tUInt FromDigits(const uInt8 *DigitBuffer,uIntn DigitCount)noexcept;
//};
template<uIntn ValueSize,ufInt8 Radix>
struct TStringConvertFloat;
//{
//	static uIntn ToDigits(tFloat Value,ufInt8 *DigitBuffer,uIntn DigitBufferLenght,sfInt16 &Exponent)noexcept;
//	static tFloat FromDigits(sfInt16 Exponent,const uInt8 *DigitBuffer,uIntn DigitCount)noexcept;
//};
//---------------------------------------------------------------------------
}	// namespace TKRuntime
//---------------------------------------------------------------------------
namespace CPPRuntime{
//---------------------------------------------------------------------------
template<uIntn CharacterSize>	
struct TString
{
	typedef typename cnVar::TIntegerOfSize<CharacterSize,false>::Type tUInt;

	static uIntn FindLength(const void *String,uIntn MaxLength)noexcept(true){
		if(MaxLength==0)
			return 0;
		cnLib_ASSERT(String!=nullptr);

		for(uIntn i=0;i<MaxLength;i++){
			if(static_cast<const tUInt*>(String)[i]==0){
				return i;
			}
		}
		return MaxLength;
	}

	static uIntn Copy(void *Dest,uIntn DestLength,const void *Src)noexcept(true){
		return cnString::ViewCopy(static_cast<tUInt*>(Dest),DestLength,static_cast<const tUInt*>(Src));
	}
	

	struct cSearchProc
	{
		tUInt Character;
		bool operator ()(const tUInt &Element){
			return Element==Character;
		}
	};
	static uIntn Search(const void *String,uIntn MaxLength,tUInt Character)noexcept(true){
		cSearchProc SearchProc={Character};
		return cnString::ViewSearch(static_cast<const tUInt*>(String),MaxLength,SearchProc);
	}

	static bool Equal(const void *Str1,const void *Str2,uIntn MaxLength,uIntn &MatchLength)noexcept(true){
		return cnString::ViewEqual(static_cast<const tUInt*>(Str1),static_cast<const tUInt*>(Str2),MaxLength,MatchLength);
	}

	static sfInt8 Compare(const void *Str1,const void *Str2,uIntn MaxLength,uIntn &MatchLength)noexcept(true){
		return cnString::ViewCompare(static_cast<const tUInt*>(Str1),static_cast<const tUInt*>(Str2),MaxLength,MatchLength);
	}


};

//---------------------------------------------------------------------------

template<uIntn ValueSize,ufInt8 Radix>
struct TStringConvertInteger
{
	typedef typename cnVar::TIntegerOfSize<ValueSize,false>::Type tUInt;


	static uIntn ToDigits(tUInt Value,uInt8 *DigitBuffer,uIntn DigitBufferIndex)noexcept(true){
		if(Value==0)
			return DigitBufferIndex;
		if(DigitBufferIndex==0)
			return 0;
		DigitBufferIndex--;
		while(Value>=Radix){
			DigitBuffer[DigitBufferIndex]=static_cast<uInt8>(Value%Radix);
			Value/=Radix;
			if(DigitBufferIndex==0)
				return 0;
			DigitBufferIndex--;
		}
		DigitBuffer[DigitBufferIndex]=static_cast<uInt8>(Value);
		return DigitBufferIndex;
	}

	static tUInt FromDigits(const uInt8 *DigitBuffer,uIntn DigitCount)noexcept(true){
		if(DigitCount==0)
			return 0;
		tUInt Result=static_cast<tUInt>(DigitBuffer[0]);
		for(uIntn DigitIndex=1;DigitIndex<DigitCount;DigitIndex++){
			tUInt DigitValue=static_cast<tUInt>(DigitBuffer[DigitIndex]);
			Result*=Radix;
			Result+=DigitValue;
		}

		return Result;
	}

};

template<uIntn ValueSize,ufInt8 Radix>
struct TStringConvertFloat
{
	typedef typename cnVar::TIntegerOfSize<ValueSize,false>::Type tUInt;
	typedef typename cnVar::TFloatOfSize<ValueSize>::Type tFloat;
	
	static uIntn ToDigits(tFloat Value,sfInt16 &Exponent,uInt8 *DigitBuffer,uIntn DigitBufferLength)noexcept(true){
		if(Value==0){
			Exponent=0;
			return 0;
		}

		tFloat RadixFactor=Radix;
		tFloat log2r=cnMath::FloatNaturalLogarithm(static_cast<tFloat>(RadixFactor))/cnMath::TFloatConstant<tFloat>::loge2;
		uIntn MaxSignificand=static_cast<uIntn>(ValueSize*ByteBitCount/log2r);
		if(DigitBufferLength>MaxSignificand)
			DigitBufferLength=MaxSignificand;


		tFloat IntValue=cnMath::FloatRoundTruncate(Value);
		tFloat Fraction;
		uIntn DigitIndex=0;
		if(IntValue!=0){
			Fraction=Value-IntValue;
			sfInt16 e2;
			IntValue=cnMath::FloatSplitExponent(IntValue,e2);
			tFloat Scale=e2/log2r;
			uIntn ScaleExp=static_cast<uIntn>(Scale);
			if(ScaleExp>MaxSignificand){
				// scale down
				ScaleExp-=MaxSignificand;
				Scale=cnMath::FloatPower(RadixFactor,static_cast<tFloat>(ScaleExp));
				IntValue/=Scale;
			}
			else{
				ScaleExp=0;
			}
			uInt8 IntDigits[ValueSize*ByteBitCount];
			uIntn IntDigitIndex=TKRuntime::TStringConvertInteger<ValueSize,Radix>::ToDigits(static_cast<typename cnVar::TIntegerOfSize<ValueSize,false>::Type>(IntValue),IntDigits,ValueSize*ByteBitCount);
			uIntn IntDigitCount=ValueSize*ByteBitCount-IntDigitIndex;
			Exponent=static_cast<sfInt16>(IntDigitCount+ScaleExp);

			if(IntDigitCount>DigitBufferLength){
				TKRuntime::TMemory<1>::Copy(DigitBuffer,IntDigits+IntDigitIndex,DigitBufferLength);
				return DigitBufferLength;
			}

			TKRuntime::TMemory<1>::Copy(DigitBuffer,IntDigits+IntDigitIndex,IntDigitCount);
			DigitIndex=IntDigitCount;
		}
		else{
			Fraction=Value;
			sfInt16 e2;
			Fraction=cnMath::FloatSplitExponent(Fraction,e2);

			// all fraction
			tFloat Scale=(-e2)/log2r;
			uIntn ScaleExp=static_cast<uIntn>(Scale);
			Scale=cnMath::FloatPower(RadixFactor,static_cast<tFloat>(ScaleExp));
			Fraction*=Scale;
			sfInt16 e=static_cast<sfInt16>(ScaleExp);
			e=-e;
			tFloat NextValue=Fraction*RadixFactor;
			while(NextValue<1.){ 
				Fraction=NextValue;
				e--;
				NextValue=Fraction*RadixFactor;
			}
			Exponent=e;
		}

		while(Fraction!=0){
			Fraction*=RadixFactor;
			tFloat DigitValue=cnMath::FloatRoundTruncate(Fraction);
			Fraction-=DigitValue;
			DigitBuffer[DigitIndex++]=static_cast<uInt8>(DigitValue);
			if(DigitIndex>=DigitBufferLength)
				return DigitBufferLength;
		}
		return DigitIndex;
	}


	static tFloat FromDigits(sfInt16 Exponent,const uInt8 *DigitBuffer,uIntn DigitCount)noexcept(true){
		tUInt IntDigitsValue=TKRuntime::TStringConvertInteger<ValueSize,Radix>::FromDigits(DigitBuffer,DigitCount);
		if(IntDigitsValue==0)
			return static_cast<tFloat>(0);
		
		Exponent-=DigitCount;
		tFloat DigitValue=static_cast<tFloat>(IntDigitsValue)/static_cast<tFloat>(1<<(ValueSize*ByteBitCount));

		if(Exponent!=0){
			tFloat RadixFactor=cnMath::FloatPower(static_cast<tFloat>(Radix),static_cast<tFloat>(Exponent));
			DigitValue*=RadixFactor;
		}
		return DigitValue;
	}

};

//---------------------------------------------------------------------------
}	// namespace CPPRuntime
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnString{
//---------------------------------------------------------------------------

// FindLength
//	Get the length of string
// [in]String
// MaxLength		maximum length of string
// return length of string
template<class TCharacter>
inline uIntn FindLength(const TCharacter *String)
{
	return TKRuntime::TString<sizeof(TCharacter)>::FindLength(String,StringMaxLength);
}

// FindLength
//	Get the length of string
// [in]String
// MaxLength		maximum length of string
// return length of string
template<class TCharacter>
inline uIntn FindLength(const TCharacter *String,uIntn MaxLength)
{
	return TKRuntime::TString<sizeof(TCharacter)>::FindLength(String,MaxLength);
}


// Copy
//	Copy string
// [out]Dest		destination string buffer
// [in]DestMax		max length of destination buffer
// [in]Src			source string, unknow length
// return	copied length
template<class TCharacter,class TSrcPtr>
inline typename cnVar::TTypeConditional<uIntn,
	cnVar::TIsPointerOf<TCharacter,typename cnVar::TRemoveReference<TSrcPtr>::Type>::Value
>::Type Copy(TCharacter *Dest,uIntn DestMax,TSrcPtr&& Src)
{
	return TKRuntime::TString<sizeof(TCharacter)>::Copy(Dest,DestMax-1,Src);
}


// Copy
//	Copy string
// <DestMax>		max capacity of destination buffer
// [out]Dest		destination string buffer
// [in]Src			source string, unknow length
// return	copied length
template<class TCharacter,uIntn DestCapacity,class TSrcPtr>
inline typename cnVar::TTypeConditional<uIntn,
	cnVar::TIsPointerOf<TCharacter,typename cnVar::TRemoveReference<TSrcPtr>::Type>::Value
>::Type Copy(TCharacter (&Dest)[DestCapacity],TSrcPtr&& Src)
{
	return TKRuntime::TString<sizeof(TCharacter)>::Copy(Dest,DestCapacity-1,Src);
}


// Copy
//	Copy string
// [out]Dest		destination string buffer
// [in]DestMax		max length of destination buffer
// [in]Src			source string array
// return	copied string length
template<class TCharacter,uIntn SrcMax>
inline uIntn Copy(TCharacter *Dest,uIntn DestMax,const TCharacter (&Src)[SrcMax])
{
	uIntn CopyMax=DestMax-1;
	if(CopyMax>SrcMax)
		CopyMax=SrcMax;
	return TKRuntime::TString<sizeof(TCharacter)>::Copy(Dest,CopyMax,Src);
}

// Copy
//	Copy string
// [in]DestMax		max length of dest, including null-termination
// [out]Dest		destination string buffer
// [in]Src			source string array
// return	copied length
template<class TCharacter,uIntn DestMax,uIntn SrcMax>
inline uIntn Copy(TCharacter (&Dest)[DestMax],const TCharacter (&Src)[SrcMax])
{
	uIntn CopyMax=DestMax-1;
	if(CopyMax>SrcMax)
		CopyMax=SrcMax;
	return TKRuntime::TString<sizeof(TCharacter)>::Copy(Dest,CopyMax,Src);
}

// Copy
//	Copy string
// [out]Dest		destination string buffer
// [in]DestMax		max length of destination buffer
// [in]Src			source string
// [in]SrcMax		max length of source string
// return	copied string length
template<class TCharacter>
inline uIntn Copy(TCharacter *Dest,uIntn DestMax,const TCharacter *Src,uIntn SrcMax)
{
	uIntn CopyMax=DestMax-1;
	if(CopyMax>SrcMax)
		CopyMax=SrcMax;
	return TKRuntime::TString<sizeof(TCharacter)>::Copy(Dest,CopyMax,Src);
}
// Copy
//	Copy string
// <DestMax>		max length of destination buffer
// [out]Dest		destination string buffer
// [in]Src			source string
// [in]SrcMax		max length of source string
// return	copied string length
template<class TCharacter,uIntn DestMax>
inline uIntn Copy(TCharacter (&Dest)[DestMax],const TCharacter *Src,uIntn SrcMax)
{
	uIntn CopyMax=DestMax-1;
	if(CopyMax>SrcMax)
		CopyMax=SrcMax;
	return TKRuntime::TString<sizeof(TCharacter)>::Copy(Dest,CopyMax,Src);
}


// Search
//	Search a character from string
//	[in]Src	string
//	[in]Dest	character to search
//	return: the index of first matched char, or IndexNotFound if not found
template<class TCharacter>
uIntn Search(const TCharacter *Src,typename cnVar::TTypeDef<TCharacter>::Type Dest)noexcept(true)
{
	return TKRuntime::TString<sizeof(TCharacter)>::Search(Src,StringMaxLength,Dest);
}
// Search
//	Search a character from string
//	[in]Src	string
//	[in]SrcLength	length of Src
//	[in]Dest	character to search
//	return: the index of first matched char, or IndexNotFound if not found
template<class TCharacter>
uIntn Search(const TCharacter *Src,uIntn MaxLength,typename cnVar::TTypeDef<TCharacter>::Type Dest)noexcept(true)
{
	return TKRuntime::TString<sizeof(TCharacter)>::Search(Src,MaxLength,Dest);
}

// Equal
//	The encoding relation of string1 to string2
//	[in]Str1	string to subtract
//	[in]Str2	string to be subtracted
//	[in]Length	length of first characters in string to compare
//	return:	if strings matched
template<class TCharacter>
bool Equal(const TCharacter *Str1,const typename cnVar::TTypeDef<TCharacter>::Type *Str2)noexcept(true)
{
	uIntn MatchLength;
	return TKRuntime::TString<sizeof(TCharacter)>::Equal(Str1,Str2,StringMaxLength,MatchLength);
}

// Equal
//	The encoding relation of string1 to string2
//	[in]Str1	string to subtract
//	[in]Str2	string to be subtracted
//	[in]Length	length of first characters in string to compare
//	return:	if strings matched
template<class TCharacter>
bool Equal(const TCharacter *Str1,const typename cnVar::TTypeDef<TCharacter>::Type *Str2,uIntn MaxLength)noexcept(true)
{
	return TKRuntime::TString<sizeof(TCharacter)>::Equal(Str1,Str2,MaxLength,MaxLength);
}


// Compare
//	The relation of string1 to string2
//	[in]Str1	string to subtract
//	[in]Str2	string to be subtracted
//	return:	0 if matched, 1 if Str1>Str2 , -1 if Str1<Str2
template<class TCharacter>
sfInt8 Compare(const TCharacter *Str1,const typename cnVar::TTypeDef<TCharacter>::Type *Str2)noexcept(true)
{
	uIntn MatchLength;
	return TKRuntime::TString<sizeof(TCharacter)>::Compare(Str1,Str2,StringMaxLength,MatchLength);
}

// Compare
//	The encoding relation of string1 to string2
//	[in]Str1	string to subtract
//	[in]Str2	string to be subtracted
//	[in]Length	length of first characters in string to compare
//	return:	the differ of first differnet char, 0 if two strings matched
template<class TCharacter>
sfInt8 Compare(const TCharacter *Str1,const typename cnVar::TTypeDef<TCharacter>::Type *Str2,uIntn MaxLength)noexcept(true)
{
	uIntn MatchLength;
	return TKRuntime::TString<sizeof(TCharacter)>::Compare(Str1,Str2,MaxLength,MatchLength);
}


template<class TCharacter>
sfInt8 Compare(const TCharacter *Str1,uIntn Str1MaxLength,const typename cnVar::TTypeDef<TCharacter>::Type *Str2,uIntn Str2MaxLength)noexcept(true)
{
	uIntn MatchLength;
	if(Str1MaxLength==Str2MaxLength){
		return TKRuntime::TString<sizeof(TCharacter)>::Compare(Str1,Str2,Str1MaxLength,MatchLength);
	}
	uIntn CompareLength;
	sfInt8 MatchResult;
	if(Str1MaxLength<Str2MaxLength){
		CompareLength=Str1MaxLength;
		MatchResult=-1;
	}
	else{
		CompareLength=Str2MaxLength;
		MatchResult=1;
	}
	sfInt8 r=TKRuntime::TString<sizeof(TCharacter)>::Compare(Str1,Str2,CompareLength,MatchLength);
	if(MatchLength==CompareLength){
		return MatchResult;
	}
	return r;
}

//---------------------------------------------------------------------------
}	// namespace cnString
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
namespace cnLib_THelper{
namespace String_TH{

template<uIntn MatchSize,ufInt8 Radix>
struct StringConvert{

	template<class T>
	static uIntn ConvertIntegerToDigits(T Value,uInt8 *DigitBuffer,uIntn DigitBufferIndex)noexcept(true){

		typedef typename cnVar::TIntegerOfSize<MatchSize,false>::Type tUInt;

		typename cnVar::TIntegerConversion<T>::tMatch IntValue=cnVar::TIntegerConversion<T>::template Cast<typename cnVar::TIntegerConversion<T>::tMatch>(Value);
		tUInt ConvertValue=static_cast<tUInt>(cnMath::Absolute(IntValue));

		return TKRuntime::TStringConvertInteger<MatchSize,Radix>::ToDigits(ConvertValue,DigitBuffer,DigitBufferIndex);
	}

	
	template<class T>
	static uIntn ConvertFloatToDigits(T Value,sfInt16 &Exponent,uInt8 *DigitBuffer,uIntn DigitBufferLength)noexcept(true)
	{
		typedef typename cnVar::TFloatConversion<T>::tMatch tFloat;

		tFloat FloatValue=cnVar::TFloatConversion<T>::template Cast<tFloat>(Value);
		FloatValue=cnMath::FloatAbsolute(FloatValue);

		return TKRuntime::TStringConvertFloat<MatchSize,Radix>::ToDigits(FloatValue,Exponent,DigitBuffer,DigitBufferLength);
	}

	template<class T>
	static T ConvertIntegerFromDigits(const uInt8 *DigitBuffer,uIntn DigitCount)noexcept(true)
	{
		typedef typename cnVar::TIntegerOfSize<MatchSize,false>::Type tUInt;

		typename cnVar::TIntegerConversion<T>::tMatch IntValue=cnVar::TIntegerConversion<T>::template Cast<typename cnVar::TIntegerConversion<T>::tMatch>(Value);
		tUInt ConvertValue=static_cast<tUInt>(cnMath::Absolute(IntValue));

		tUInt IntValue=TKRuntime::TStringConvertInteger<MatchSize,Radix>::FromDigits(DigitBuffer,DigitBufferIndex);
		return static_cast<T>(IntValue);
	}

	template<class T>
	static T ConvertFloatFromDigits(sfInt16 Exponent,const uInt8 *DigitBuffer,uIntn DigitCount)noexcept(true)
	{
		typedef typename cnVar::TFloatConversion<T>::tMatch tFloat;

		tFloat FloatValue=TKRuntime::TStringConvertInteger<MatchSize,Radix>::FromDigits(Exponent,DigitBuffer,DigitBufferLength);;
		return static_cast<T>(FloatValue);
	}
};


template<ufInt8 Radix>
struct StringConvert<Radix,0>{

	template<class T>
	static uIntn ConvertIntegerToDigits(T,uInt8*,uIntn DigitBufferIndex)noexcept(true){
		return DigitBufferIndex;
	}

	template<class T>
	static uIntn ConvertFloatToDigits(T,sfInt16 &Exponent,uInt8*,uIntn)noexcept(true){
		Exponent=0;
		return 0;
	}

	template<class T>
	static T ConvertIntegerFromDigits(const uInt8*,uIntn)noexcept(true){
		return T();
	}

	template<class T>
	static T ConvertFloatFromDigits(sfInt16,const ufInt8*,uIntn)noexcept(true){
		return T();
	}
};
//---------------------------------------------------------------------------
}	//	namespace String_TH
}	// namespace cnLib_THelper
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnString{
//---------------------------------------------------------------------------

template<ufInt8 Radix,class T>
inline uIntn ConvertIntegerToDigits(T Value,uInt8 *DigitBuffer,uIntn DigitBufferIndex)noexcept(true)
{
	return cnLib_THelper::String_TH::StringConvert<cnVar::TIntegerConversion<T>::MatchSize,Radix>::ConvertIntegerToDigits(Value,DigitBuffer,DigitBufferIndex);
}

template<ufInt8 Radix,class T>
inline uIntn ConvertFloatToDigits(T Value,sfInt16 &Exponent,uInt8 *DigitBuffer,uIntn DigitBufferLength)noexcept(true)
{
	return cnLib_THelper::String_TH::StringConvert<cnVar::TFloatConversion<T>::MatchSize,Radix>::ConvertFloatToDigits(Value,Exponent,DigitBuffer,DigitBufferLength);
}

template<ufInt8 Radix,class T>
inline T ConvertIntegerFromDigits(const uInt8 *DigitBuffer,uIntn DigitCount)noexcept(true)
{
	return cnLib_THelper::String_TH::StringConvert<cnVar::TIntegerConversion<T>::MatchSize,Radix>::template ConvertIntegerFromDigits<T>(DigitBuffer,DigitBufferIndex);
}

template<ufInt8 Radix,class T>
inline T ConvertFloatFromDigits(sfInt16 Exponent,const uInt8 *DigitBuffer,uIntn DigitCount)noexcept(true)
{
	return cnLib_THelper::String_TH::StringConvert<cnVar::TFloatConversion<T>::MatchSize,Radix>::template ConvertFloatFromDigits<T>(Exponent,DigitBuffer,DigitBufferIndex);
}

//---------------------------------------------------------------------------
}	// namespace cnString
//---------------------------------------------------------------------------
namespace cnDataStruct{
//---------------------------------------------------------------------------

// cStringStorage
//	Has one extra Capacity for null-termination
template<class TAllocationOperator,class TCharacter>
struct cStringStorage : cArrayAllocation<TAllocationOperator,TCharacter>
{
	typedef TAllocationOperator tAllocationOperator;
	typedef TCharacter tElement;
	typedef cArrayAllocation<TAllocationOperator,tElement> tArrayAllocation;
	typedef TCharacter *tPointer;
	typedef TCharacter const *tConstPointer;
	typedef cArray<TCharacter> tArray;
	typedef cArray<TCharacter const> tConstArray;
	typedef cArrayMemoryFunction<TAllocationOperator,TCharacter> TAMFunc;

	cnLib_STATIC_ASSERT(cnVar::TIsTrivial<TCharacter>::Value,"trival character required");

	void Reset(void)noexcept(true){
		this->Pointer=nullptr;
		this->Length=0;
		this->Capacity=0;
	}

	cStringStorage()noexcept(true){	Reset();	}
    ~cStringStorage()noexcept(true){
		if(this->Capacity!=0)
			TAMFunc::Deallocate(this->Pointer,this->Capacity);
	}

	cStringStorage(tConstPointer String,uIntn Length,uIntn InitialCapacity=0)noexcept(true){
		if(Length==0 && InitialCapacity==0){
			Reset();
			return;
		}
		if(InitialCapacity<Length+1){
			InitialCapacity=Length+1;
		}
		// allocate
		InitialCapacity=TAMFunc::RoundUpCapacity(InitialCapacity);
		this->Capacity=InitialCapacity;
		this->Pointer=TAMFunc::Allocate(InitialCapacity);

		// data
		this->Length=Length;
		if(Length!=0){
			TKRuntime::TMemory<sizeof(tElement)>::Copy(this->Pointer,String,Length);
		}
	}
	
	cStringStorage(const cStringStorage &Src)noexcept(true){
		if(Src.Length==0){
			Reset();
		}
		else{
			uIntn InitialCapacity=TAMFunc::RoundUpCapacity(Src.Length+1);
			this->Capacity=InitialCapacity;
			this->Pointer=TAMFunc::Allocate(InitialCapacity,1);
			this->Length=Src.Length;
			TKRuntime::TMemory<sizeof(tElement)>::Copy(this->Pointer,Src.Pointer,Src.Length);
		}
	}

	cStringStorage(const tConstArray &Src)noexcept(true){
		uIntn InitialCapacity=TAMFunc::RoundUpCapacity(Src.Length+1);
		this->Capacity=InitialCapacity;
		this->Pointer=TAMFunc::Allocate(InitialCapacity);
		this->Length=Src.Length;
		TKRuntime::TMemory<sizeof(tElement)>::Copy(this->Pointer,Src.Pointer,Src.Length);
	}

	cStringStorage& operator =(const cStringStorage &Src)noexcept(true){
		if(this!=cnMemory::AddressOf(Src)){
			Copy(Src.Pointer,Src.Length);
		}
		return *this;
	}
	
	cStringStorage& operator =(const tConstArray &Src)noexcept(true){
		if(ArrayCast<tConstArray>(this)!=&Src){
			Copy(Src.Pointer,Src.Length);
		}
		return *this;
	}
#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L
	
	cStringStorage(cStringStorage &&Src)noexcept(true){
		this->Pointer=Src.Pointer;
		this->Length=Src.Length;
		this->Capacity=Src.Capacity;
		Src.Reset();
	}

	cStringStorage(tArrayAllocation &&Src)noexcept(true){
		this->Pointer=Src.Pointer;
		this->Length=Src.Length;
		this->Capacity=Src.Capacity;
		Src.Pointer=nullptr;
		Src.Length=0;
		Src.Capacity=0;

		if(this->Length!=0 && this->Length>=this->Capacity){
			GrowCapacityForLength(this->Length);
		}
	}

	cStringStorage& operator =(cStringStorage &&Src)noexcept(true){
		return operator=(static_cast<tArrayAllocation&&>(Src));
	}

	cStringStorage& operator =(tArrayAllocation &&Src)noexcept(true){
		if(this->Capacity!=0){
			TAMFunc::Deallocate(this->Pointer,this->Capacity);
		}
		this->Pointer=Src.Pointer;
		this->Length=Src.Length;
		this->Capacity=Src.Capacity;
		Src.Pointer=nullptr;
		Src.Length=0;
		Src.Capacity=0;

		if(this->Length!=0 && this->Length>=this->Capacity){
			GrowCapacityForLength(this->Length);
		}
		return *this;
	}

#endif	// cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

	cStringStorage Relocate(uIntn NewCapacity)noexcept(true){
		cStringStorage PreviousMemory;
		NewCapacity=TAMFunc::RoundUpCapacity(NewCapacity);
		bool ManualCopy;
		tPointer NewArray=TAMFunc::Relocate(this->Pointer,this->Capacity,1,NewCapacity,1,ManualCopy);
		if(ManualCopy){
			PreviousMemory.Pointer=this->Pointer;
			PreviousMemory.Length=this->Length;
			PreviousMemory.Capacity=this->Capacity;
			this->Length=0;
		}
		this->Pointer=NewArray;
		this->Capacity=NewCapacity;
		return PreviousMemory;
	}

	void GrowCapacityTo(uIntn NewCapacity)noexcept(true){
		cnLib_ASSERT(NewCapacity>=this->Capacity);
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
			TKRuntime::TMemory<sizeof(tElement)>::Copy(NewArray,this->Pointer,this->Length);
			TAMFunc::Deallocate(this->Pointer,this->Capacity);
		}
		this->Pointer=NewArray;
		this->Capacity=NewCapacity;
	}
	void GrowCapacityForLength(uIntn NewLength)noexcept(true){	return GrowCapacityTo(NewLength+1);	}

	void SetRange(uIntn Index,tConstPointer Src,uIntn CopyLength)noexcept(true){
		if(Index>this->Length){
			return;
		}

		uIntn NewLength=Index+CopyLength;
		if(NewLength>this->Length){
			this->Length=NewLength;
			if(NewLength>=this->Capacity){
				GrowCapacityForLength(NewLength);
			}
		}
		TKRuntime::TMemory<sizeof(tElement)>::Copy(this->Pointer+Index,Src,CopyLength);
	}
	void Copy(tConstPointer Src,uIntn SrcLength)noexcept(true){
		if(SrcLength>=this->Capacity){
			GrowCapacityTo(SrcLength+1);
		}
		this->Length=SrcLength;

		TKRuntime::TMemory<sizeof(tElement)>::Copy(this->Pointer,Src,SrcLength);
	}

	void Remove(uIntn Index,uIntn RemoveLength)noexcept(true){
		if(RemoveLength==0)
			return;
		if(Index>=this->Length)
			return;
		uIntn CopySrc=Index+RemoveLength;
		if(CopySrc>=this->Length){
			// remove tail
			this->Length=Index;
			return;
		}
		// remove at index
		uIntn CopyLength=this->Length-CopySrc;
		this->Length-=RemoveLength;
		if(CopyLength!=0){
			TKRuntime::TMemory<sizeof(tElement)>::Copy(this->Pointer+Index,this->Pointer+CopySrc,CopyLength);
		}
	}
	
	void SetLength(uIntn NewLength)noexcept(true){
		if(NewLength>=this->Capacity){
			// grow
			GrowCapacityForLength(NewLength);
		}
		this->Length=NewLength;
	}

	void SetCapacity(uIntn NewCapacity)noexcept(true){
		if(this->Capacity==NewCapacity)
			return;
		if(NewCapacity>this->Capacity){
			return GrowCapacityTo(NewCapacity);
		}
		// shrink
		if(NewCapacity==0){
			TAMFunc::Deallocate(this->Pointer,this->Capacity);
			Reset();
			return;
		}

		if(this->Length>NewCapacity){
			this->Length=NewCapacity-1;
		}
		bool ManualCopy;
		tPointer NewArray=TAMFunc::Relocate(this->Pointer,this->Capacity,NewCapacity,ManualCopy);
		if(ManualCopy){
			TKRuntime::TMemory<sizeof(tElement)>::Copy(NewArray,this->Pointer,this->Length);
			TAMFunc::Deallocate(this->Pointer,this->Capacity);
		}
		this->Pointer=NewArray;
		this->Capacity=NewCapacity;
	}

	void Replace(uIntn Index,uIntn ReplaceLength,uIntn ReplacementLength)noexcept(true){
		uIntn TailSrc=Index+ReplaceLength;
		uIntn TailDest=Index+ReplacementLength;
		if(TailSrc>=this->Length){
			if(Index>this->Length)
				return;
			this->Length=Index;
			if(this->Capacity<=TailDest){
				GrowCapacityForLength(TailDest);
			}
			this->Length=TailDest;
			return;
		}
		uIntn TailLength=this->Length-TailSrc;
		this->Length=TailDest+TailLength;
		
		if(this->Length>=this->Capacity){
			// grow
			uIntn NewCapacity=TAMFunc::RoundUpCapacity(this->Length+1);
			bool ManualCopy;
			tPointer NewArray=TAMFunc::Relocate(this->Pointer,this->Capacity,NewCapacity,ManualCopy);
			if(ManualCopy){
				TKRuntime::TMemory<sizeof(tElement)>::Copy(NewArray,this->Pointer,Index);
				TKRuntime::TMemory<sizeof(tElement)>::Copy(NewArray+TailDest,this->Pointer+TailSrc,TailLength);
				TAMFunc::Deallocate(this->Pointer,this->Capacity);
				this->Pointer=NewArray;
				this->Capacity=NewCapacity;
				return;
			}
			this->Pointer=NewArray;
			this->Capacity=NewCapacity;
		}
		if(TailSrc!=TailDest){
			TKRuntime::TMemory<sizeof(tElement)>::CopyOverlapped(this->Pointer+TailDest,this->Pointer+TailSrc,TailLength);
		}
	}
};

//---------------------------------------------------------------------------
}	// namespace cnDataStruct
//---------------------------------------------------------------------------
namespace cnStream{
//---------------------------------------------------------------------------
template<class TAllocationOperator,class TCharacter>
class cStringStorageStreamReadBuffer
{
public:
	cStringStorageStreamReadBuffer(cnDataStruct::cStringStorage<TAllocationOperator,TCharacter> &Storage,uIntn ReadIndex)noexcept(true)
		: fStorage(Storage)
		, fReadIndex(ReadIndex){}
	
	cnLib_STATIC_ASSERT(cnVar::TIsTrivial<TCharacter>::Value,"trival character required");
	
	cnDataStruct::cStringStorage<TAllocationOperator,TCharacter> &GetStorage(void)const noexcept(true){	return fStorage;	}

	// tElement
	//	type of element in buffer
	typedef TCharacter tElement;

	// TStreamReadBuffer<T>

	cArray<const tElement> GatherReadBuffer(uIntn QueryLength)noexcept(true){
		uIntn LengthAvailable=fStorage.Length-fReadIndex;
		cArray<const tElement> Buffer;
		Buffer.Pointer=fStorage.Pointer+fReadIndex;
		Buffer.Length=LengthAvailable;
		return Buffer;
	}
	void DismissReadBuffer(uIntn Length)noexcept(true){
		uIntn RemainLength=fStorage.Length-fReadIndex;
		if(Length<RemainLength){
			fReadIndex+=Length;
		}
		else{
			fReadIndex=fStorage.Length;
		}
	}
protected:
	cnDataStruct::cStringStorage<TAllocationOperator,TCharacter> &fStorage;
	uIntn fReadIndex;
};
//---------------------------------------------------------------------------
template<class TAllocationOperator,class TCharacter>
class cStringStorageStreamWriteBuffer
{
public:
	cnLib_STATIC_ASSERT(cnVar::TIsTrivial<TCharacter>::Value,"trival character required");

	cStringStorageStreamWriteBuffer(cnDataStruct::cStringStorage<TAllocationOperator,TCharacter> &Storage,uIntn Increment=0)noexcept(true)
		: fStorage(Storage)
		, fIncrement(Increment)
	{
		if(fIncrement<128)
			fIncrement=128;
	}

	~cStringStorageStreamWriteBuffer()noexcept(true){
		if(fStorage.Pointer!=nullptr)
			fStorage.Pointer[fStorage.Length]=0;
	}
	cnDataStruct::cStringStorage<TAllocationOperator,TCharacter> &GetStorage(void)const noexcept(true){	return fStorage;	}

	// tElement
	//	type of element in buffer
	typedef TCharacter tElement;

	// TStreamWriteBuffer<T>

	cArray<tElement> ReserveWriteBuffer(uIntn QueryLength)noexcept(true){
		uIntn LengthAvailable=fStorage.Capacity-fStorage.Length;
		if(LengthAvailable<=1 || LengthAvailable<QueryLength+1){

			if(QueryLength<fIncrement){
				QueryLength=fIncrement;
			}
			// mark length to preserve buffer content
			uIntn CurLength=fStorage.Length;
			fStorage.Length=fStorage.Capacity;
			// grow capacity
			fStorage.GrowCapacityTo(CurLength+QueryLength);
			LengthAvailable=fStorage.Capacity-CurLength;
			cnLib_ASSERT(LengthAvailable>0);
			// restore length
			fStorage.Length=CurLength;
		}
		cArray<tElement> Buffer;
		Buffer.Pointer=fStorage.Pointer+fStorage.Length;
		Buffer.Length=LengthAvailable-1;
		return Buffer;
	}
	void CommitWriteBuffer(uIntn Length)noexcept(true){
		uIntn RemainLength=fStorage.Capacity-fStorage.Length-1;
		if(Length<RemainLength){
			fStorage.Length+=Length;
		}
		else{
			fStorage.Length=fStorage.Capacity-1;
		}
	}
protected:
	cnDataStruct::cStringStorage<TAllocationOperator,TCharacter> &fStorage;
	uIntn fIncrement;
};
//---------------------------------------------------------------------------

// ArrayStreamReadBuffer
//	define an array read stream buffer from string strorage
// <TAllocationOperator>	allocation operator for storage
// [in,out]Storage			reference to the storage
// StartIndex				index of element to start reading
// return read stream object
template<class TAllocationOperator,class TCharacter>
inline cStringStorageStreamReadBuffer<TAllocationOperator,TCharacter> ArrayStreamReadBuffer(cnDataStruct::cStringStorage<TAllocationOperator,TCharacter> &Storage,uIntn StartIndex=0)noexcept(true)
{
	return cStringStorageStreamReadBuffer<TAllocationOperator,TCharacter>(Storage,StartIndex);
}

// ArrayStreamWriteBuffer
//	define an array write stream buffer from string strorage
// <TAllocationOperator>	allocation operator for storage
// [in,out]Storage			reference to the storage
// Increment				default increment for storage expanding
// return write stream object
template<class TAllocationOperator,class TCharacter>
inline cStringStorageStreamWriteBuffer<TAllocationOperator,TCharacter> ArrayStreamWriteBuffer(cnDataStruct::cStringStorage<TAllocationOperator,TCharacter> &Storage,uIntn Increment=0)noexcept(true)
{
	return cStringStorageStreamWriteBuffer<TAllocationOperator,TCharacter>(Storage,Increment);
}
//---------------------------------------------------------------------------
}	// namespace cnStream
//---------------------------------------------------------------------------
namespace cnString{
//---------------------------------------------------------------------------
template<class TCharacter>
struct TEmptyString
{
	static const TCharacter Value[1];
};

template<class TCharacter>
const TCharacter TEmptyString<TCharacter>::Value[1]={0};


template<class TAllocationOperator,class TCharacter>
class cStringBuffer
{
public:
	typedef TCharacter tCharacter;
	typedef cnDataStruct::cArrayAllocation<TAllocationOperator,TCharacter> tArrayAllocation;
	typedef TCharacter *tPointer;
	typedef TCharacter const *tConstPointer;
	typedef cArray<TCharacter> tArray;
	typedef cArray<const TCharacter> tConstArray;

	cnLib_STATIC_ASSERT(cnVar::TIsTrivial<TCharacter>::Value,"trival character required");


	cStringBuffer()noexcept(true){}
    ~cStringBuffer()noexcept(true){}

	cStringBuffer(const cStringBuffer &Src)noexcept(true):fString(Src.fString){	StorageSafeSetTermination();	}
	cStringBuffer(const tConstArray &Src)noexcept(true):fString(Src){	StorageSafeSetTermination();	}

#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

	cStringBuffer(cStringBuffer &&Src)noexcept(true):fString(static_cast<tArrayAllocation&&>(Src.fString)){}
	cStringBuffer(tArrayAllocation &&Src)noexcept(true):fString(static_cast<tArrayAllocation&&>(Src)){}

#endif // cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L


    cStringBuffer& operator =(const cStringBuffer &Src)noexcept(true){
		if(this==&Src)
			return *this;
		fString=Src.fString;
		StorageSafeSetTermination();
		return *this;
	}

    cStringBuffer& operator =(const tConstArray &Src)noexcept(true){
		fString=Src;
		StorageSafeSetTermination();
		return *this;
	}
#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

	cStringBuffer& operator =(cStringBuffer &&Src)noexcept(true){
		fString=static_cast<tArrayAllocation&&>(Src.fString);
		return *this;
	}

	cStringBuffer& operator =(tArrayAllocation &&Src)noexcept(true){
		fString=static_cast<tArrayAllocation&&>(Src);
		return *this;
	}

#endif // cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L


	cStringBuffer(tConstPointer String,uIntn Length,uIntn InitialCapacity=0)noexcept(true)
		: fString(String,Length,InitialCapacity)
	{
		StorageSafeSetTermination();
	}
	cStringBuffer(tConstPointer String)noexcept(true)
		: fString(String,(String!=nullptr?cnString::FindLength(String):0),0)
	{
		StorageSafeSetTermination();
	}

	cStringBuffer& operator = (tConstPointer String)noexcept(true){
		if(String!=nullptr){
			uIntn StrLength=cnString::FindLength(String);
			if(StrLength!=0){
				fString.Copy(String,StrLength);
				StorageSetTermination();
				return *this;
			}
		}
		if(fString.Capacity!=0){
			fString.Length=0;
			StorageSetTermination(0);
		}
		return *this;
	}

	tCharacter& operator [] (uIntn Index)noexcept(true){	return fString.Pointer[Index];	}


	// access to string memory block
    tConstPointer GetString(void)const noexcept(true){	return fString.Pointer;	}
    tPointer GetString(void)noexcept(true){				return fString.Pointer;	}
	
	const tArray& GetArray()const noexcept(true)
	{	return fString;	}
	const tArray& operator *()const noexcept(true)
	{	return fString;	}
	const tArray* operator ->()const noexcept(true){	return &fString;	}

	cnDataStruct::cStringStorage<TAllocationOperator,TCharacter> &Storage(void)noexcept(true){			return fString;	}
	const cnDataStruct::cStringStorage<TAllocationOperator,TCharacter> &Storage(void)const noexcept(true){return fString;	}

	// Swap
    //	swap string storage
    cnDataStruct::cStringStorage<TAllocationOperator,TCharacter> Swap(void)noexcept(true){
		cnDataStruct::cStringStorage<TAllocationOperator,TCharacter> Swapped(cnLib_UREFCAST(tArrayAllocation)(fString));
		return Swapped;
	}
#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L
	// Swap
    //	swap string storage
    cnDataStruct::cStringStorage<TAllocationOperator,TCharacter> Swap(tArrayAllocation &&Src)noexcept(true){
		cnDataStruct::cStringStorage<TAllocationOperator,TCharacter> Swapped(static_cast<tArrayAllocation&&>(fString));
		fString=static_cast<tArrayAllocation&&>(Src);
		return Swapped;
	}
#endif	// cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L
	
	void Clear(void)noexcept(true){
		if(fString.Capacity!=0){
			fString.Length=0;
			StorageSetTermination(0);
		}
	}

// string length
    uIntn GetLength(void)const noexcept(true){		return fString.Length;		}
	void SetLength(uIntn NewLength)noexcept(true){
		if(NewLength==0){
			if(fString.Length!=0){
				fString.Length=0;
				StorageSetTermination(0);
			}
		}
		else{
			fString.SetLength(NewLength);
			StorageSetTermination();
		}
	}

	uIntn GetCapacity(void)const noexcept(true){		return fString.Capacity;	}
	void SetCapacity(uIntn NewCapacity)noexcept(true){
		fString.SetCapacity(NewCapacity+1);
		StorageSetTermination();
	}
	
	void Compact(void)noexcept(true){				fString.SetCapacity(fString.Length+1);	}

	// Stream

	cnStream::cStringStorageStreamReadBuffer<TAllocationOperator,TCharacter> StreamReadBuffer(uIntn StartIndex=0)noexcept(true)
	{	return cnStream::ArrayStreamReadBuffer(fString,StartIndex);	}
	cnStream::cStringStorageStreamWriteBuffer<TAllocationOperator,TCharacter> StreamWriteBuffer(uIntn Increment=0)noexcept(true)
	{	return cnStream::ArrayStreamWriteBuffer(fString,Increment);	}

	// Edit

	void SetString(tConstPointer Src,uIntn SrcLength)noexcept(true){
		if(SrcLength!=0){
			fString.Copy(Src,SrcLength);
			StorageSetTermination();
		}
		else{
			Clear();
		}
	}

	void Replace(uIntn Index,uIntn ReplaceLength,uIntn ReplacementLength,tConstPointer Replacement=nullptr)noexcept(true){
		fString.Replace(Index,ReplaceLength,ReplacementLength);

		if(Replacement!=nullptr)
			TKRuntime::TMemory<sizeof(tCharacter)>::Copy(fString.Pointer+Index,Replacement,ReplacementLength);
		StorageSafeSetTermination();
	}

	void Append(tConstPointer Content,uIntn ContentLength)noexcept(true){
		if(ContentLength==0)
			return;
		uIntn Index=fString.Length;
		uIntn NewLength=fString.Length+ContentLength;
		if(NewLength>=fString.Capacity)
			fString.GrowCapacityForLength(NewLength+1);
		if(Content!=nullptr){
			TKRuntime::TMemory<sizeof(tCharacter)>::Copy(fString.Pointer+Index,Content,ContentLength);
		}
		fString.Length+=ContentLength;
		StorageSetTermination();
	}
	void Append(tConstPointer String)noexcept(true){
		if(String!=nullptr){
			uIntn StrLength=cnString::FindLength(String);
			Append(String,StrLength);
		}
	}

	void Truncate(uIntn Length)noexcept(true){
		if(Length<fString.Length){
			fString.Length-=Length;
			StorageSetTermination();
		}
		else{
			Clear();
		}
	}


    cStringBuffer& operator += (tConstPointer String)noexcept(true){
		Append(String);
		return *this;
	}

    cStringBuffer operator + (tConstPointer SrcString)const
#if cnLibrary_CPPFEATURE_REF_QUALIFIERS >= 200710L
	&
#endif 
		noexcept(true)
	{
		if(SrcString==0){
			return *this;
		}
		uIntn SrcLength=cnString::FindLength(SrcString);
		if(SrcLength==0)
			return *this;
		cStringBuffer r(fString.Pointer,fString.Length,fString.Length+SrcLength);
		r.Append(SrcString,SrcLength);
		return r;
	}
#if cnLibrary_CPPFEATURE_REF_QUALIFIERS >= 200710L
    cStringBuffer&& operator + (tConstPointer String)&& noexcept(true){
		Append(String);
		return static_cast<cStringBuffer&&>(*this);
	}
#endif 
	// cstr + cStringBuffer&
	friend cStringBuffer operator + (const tCharacter *String,const cStringBuffer &Src){
		cStringBuffer r=String;
		r+=Src;
		return r;
	}

#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L
	// cstr + cStringBuffer&&
	friend cStringBuffer operator + (const tCharacter *String,cStringBuffer &&Src){
		Src.Replace(0,0,GetLength(String),String);
		return static_cast<cStringBuffer&&>(Src);
	}
#endif 

	void AppendChar(tCharacter Char)noexcept(true){
		uIntn NewLength=fString.Length+1;
		if(NewLength>=fString.Capacity)
			fString.GrowCapacityForLength(NewLength+1);
		fString.Pointer[fString.Length]=Char;
		fString.Length++;
		StorageSetTermination();
	}
	void AppendChar(tCharacter Char,uIntn Count)noexcept(true){
		uIntn NewLength=fString.Length+Count;
		if(NewLength>=fString.Capacity)
			fString.GrowCapacityForLength(NewLength+1);
		TKRuntime::TArray<sizeof(tCharacter)>::Fill(fString.Pointer+fString.Length,Count,&Char);
		fString.Length+=Count;
		StorageSetTermination();
	}

	
	void Insert(uIntn InsertIndex,uIntn InsertLength)noexcept(true){
		return Replace(InsertIndex,0,InsertLength);
	}
	void Insert(uIntn InsertIndex,tConstPointer Content,uIntn ContentLength)noexcept(true){
		return Replace(InsertIndex,0,ContentLength,Content);
	}
	void Insert(uIntn InsertIndex,tConstPointer Content)noexcept(true){
		if(Content!=nullptr){
			uIntn ContentLength=cnString::FindLength(Content);
			return Replace(InsertIndex,0,ContentLength,Content);
		}
	}

protected:
	cnDataStruct::cStringStorage<TAllocationOperator,TCharacter> fString;

	void StorageSetTermination(void)noexcept(true){
		fString.Pointer[fString.Length]=0;
	}
	void StorageSetTermination(uIntn Index)noexcept(true){
		fString.Pointer[Index]=0;
	}

	void StorageSafeSetTermination(void)noexcept(true){
		if(fString.Capacity!=0){
			fString.Pointer[fString.Length]=0;
		}
	}
	void StorageSafeSetTermination(uIntn Index)noexcept(true){
		if(fString.Capacity>Index){
			fString.Pointer[Index]=0;
		}
	}

};

//---------------------------------------------------------------------------
//TConstStringTokenOperator
//{
//	typedef tCharacter;
//	typedef tStringToken;
//
//	static const tCharacter* GetPointer(const tStringToken &StrRef)noexcept;
//	static uIntn GetLength(const tStringToken &StrRef)noexcept;
//	static const cArray<const tCharacter> GetArray(const tStringToken &StrRef)noexcept;
//	static const cArray<const tCharacter>& GetArray(const tStringToken &StrRef)noexcept;
//
//	static tStringToken Duplicate(const tStringToken &StrRef)noexcept;
//	static tStringToken Move(tStringToken &&StrRef)noexcept;
//	static void Discard(tStringToken &StrRef)noexcept;
//
//	static tStringToken MakeFrom(void)noexcept;
//	static tStringToken MakeFrom(T Src)noexcept;
//	static tStringToken MakeStringCopy(const tCharacter *String,uIntn Length)noexcept;
//	static tStringToken MakeConstant(const cArrayConstant<tCharacter> &Array)noexcept;
//};
//---------------------------------------------------------------------------
}	// namespace cnString
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#if cnLibrary_CPPFEATURELEVEL >= 1
//---------------------------------------------------------------------------
namespace cnLib_THelper{
//---------------------------------------------------------------------------
namespace String_TH{
//---------------------------------------------------------------------------

template<class TStringTokenOperator>
struct StringTokenOperatorExpression
{
	typedef typename TStringTokenOperator::tCharacter tCharacter;
	typedef typename TStringTokenOperator::tStringToken tStringToken;
	template<const cArray<const tCharacter>& (*)(const tStringToken&)=&TStringTokenOperator::GetArray>
	struct TestRef
	{
		typedef void Type;
	};
};

template<class TStringTokenOperator,class=void>
struct StringTokenOperator_TokenArrayDef
	: cnVar::TTypeDef< const cArray<typename TStringTokenOperator::tCharacter const> >{};

template<class TStringTokenOperator>
struct StringTokenOperator_TokenArrayDef<TStringTokenOperator,typename StringTokenOperatorExpression<TStringTokenOperator>::TestRef<>::Type>
	: cnVar::TTypeDef< const cArray<typename TStringTokenOperator::tCharacter const>& >{};


template<class TStringTokenOperator>
struct StringTokenOperator
{
	typedef typename TStringTokenOperator::tCharacter tCharacter;
	typedef typename TStringTokenOperator::tStringToken tStringToken;


	typedef typename StringTokenOperator_TokenArrayDef<TStringTokenOperator>::Type tTokenArray;
};
//---------------------------------------------------------------------------
}	// namespace String_TH
//---------------------------------------------------------------------------
}	// namespace cnLib_THelper
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnString{
//---------------------------------------------------------------------------
template<class TConstStringTokenOperator>
class cConstString
{
public:

	typedef typename TConstStringTokenOperator::tCharacter tCharacter;
	typedef typename TConstStringTokenOperator::tStringToken tStringToken;

	typedef typename cnLib_THelper::String_TH::StringTokenOperator<TConstStringTokenOperator>::tTokenArray tTokenArray;

	cnLib_STATIC_ASSERT(cnVar::TIsTrivial<tCharacter>::Value,"trival character required");


protected:

	template<class T>
	static typename cnVar::TSelect<0,tStringToken
#if cnLibrary_CPPFEATURE_DECLTYPE >= 200707L
		, decltype(TConstStringTokenOperator::MakeFrom(cnVar::DeclVar<T cnLib_UREF>()))
#endif
	>::Type MakeFrom(T cnLib_UREF Args)noexcept(true)
	{
		return TConstStringTokenOperator::MakeFrom(cnLib_UREFCAST(T)(Args));
	}


	static tStringToken MakeFrom(const tCharacter *Src)noexcept(true){
		if(Src==nullptr){
			return TConstStringTokenOperator::MakeFrom();
		}
		return TConstStringTokenOperator::MakeStringCopy(Src,cnString::FindLength(Src));
	}

public:
	~cConstString()noexcept(true){
		TConstStringTokenOperator::Discard(fStringToken);
	}
	cConstString()noexcept(true) : fStringToken(TConstStringTokenOperator::MakeFrom()){}

	void Clear(void)noexcept(true){
		TConstStringTokenOperator::Discard(fStringToken);
		fStringToken=TConstStringTokenOperator::MakeFrom();
	}

#ifndef cnLibrary_CPPEXCLUDE_NULLPTR
	cConstString(tNullptr)noexcept(true) : fStringToken(TConstStringTokenOperator::MakeFrom()){}
	cConstString& operator =(tNullptr)noexcept(true){
		Clear();
		return *this;
	}
#endif // !cnLibrary_CPPEXCLUDE_NULLPTR

	cConstString(const cConstString &Src)noexcept(true)
		: fStringToken(TConstStringTokenOperator::Duplicate(Src.fStringToken)){}

	cConstString& operator = (const cConstString &Src)noexcept(true){
		if(this!=&Src){
			TConstStringTokenOperator::Discard(fStringToken);
			fStringToken=TConstStringTokenOperator::Duplicate(Src.fStringToken);
		}
		return *this;
	}

#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L
	cConstString(cConstString &&Src)noexcept(true)
		: fStringToken(TConstStringTokenOperator::Move(static_cast<tStringToken&&>(Src.fStringToken))){}


	cConstString& operator = (cConstString &&Src)noexcept(true){
		TConstStringTokenOperator::Discard(fStringToken);
		fStringToken=TConstStringTokenOperator::Move(static_cast<tStringToken&&>(Src.fStringToken));
		return *this;
	}
#endif	// cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L


	operator const tCharacter *()const noexcept(true){	return TConstStringTokenOperator::GetPointer(fStringToken);	}
	const tCharacter *GetString()const noexcept(true){	return TConstStringTokenOperator::GetPointer(fStringToken);	}
	uIntn GetLength(void)const noexcept(true){			return TConstStringTokenOperator::GetLength(fStringToken);	}

	tTokenArray GetArray()const noexcept(true)
	{	return TConstStringTokenOperator::GetArray(fStringToken);	}
	tTokenArray operator *()const noexcept(true)
	{	return TConstStringTokenOperator::GetArray(fStringToken);	}


#ifndef cnLibrary_CPPEXCLUDE_FUNCTION_TEMPLATE_DEFALT_ARGUMENT

	template<class=typename cnVar::TTypeConditional<void,cnVar::TIsReference<tTokenArray>::Value>::Type>
	const cArray<const tCharacter>* operator ->()const noexcept(true)
	{	return &TConstStringTokenOperator::GetArray(fStringToken);	}

	// !cnLibrary_CPPEXCLUDE_FUNCTION_TEMPLATE_DEFALT_ARGUMENT
#else
// cnLibrary_CPPEXCLUDE_FUNCTION_TEMPLATE_DEFALT_ARGUMENT

	typename cnVar::TSelect<cnVar::TIsReference<tTokenArray>::Value
		, void*
		, const cArray<const tCharacter>*
	>::Type operator ->()const noexcept(true)
	{	return &TConstStringTokenOperator::GetArray(fStringToken);	}


#endif	// cnLibrary_CPPEXCLUDE_FUNCTION_TEMPLATE_DEFALT_ARGUMENT


	tStringToken &Token(void)noexcept(true)				{	return fStringToken;	}
	const tStringToken &Token(void)const noexcept(true)	{	return fStringToken;	}

	// string by string reference
	explicit cConstString(const tStringToken &Src)noexcept(true)
		: fStringToken(TConstStringTokenOperator::Duplicate(Src)){}

	cConstString& operator =(const tStringToken &Src)noexcept(true){
		if(cnMemory::AddressOf(fStringToken)!=cnMemory::AddressOf(Src)){
			TConstStringTokenOperator::Discard(fStringToken);
			fStringToken=TConstStringTokenOperator::Duplicate(Src);
		}
		return *this;
	}

#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L
	explicit cConstString(tStringToken &&Src)noexcept(true)
		: fStringToken(TConstStringTokenOperator::Move(static_cast<tStringToken&&>(Src))){}

	cConstString& operator =(tStringToken &&Src)noexcept(true){
		TConstStringTokenOperator::Discard(fStringToken);
		fStringToken=TConstStringTokenOperator::Move(static_cast<tStringToken&&>(Src));
		return *this;
	}

#endif	// cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L


	// string by array

	cConstString(const cArray<const tCharacter> &Array)noexcept(true)
		: fStringToken(TConstStringTokenOperator::MakeStringCopy(Array.Pointer,Array.Length)){}
	cConstString& operator = (const cArray<const tCharacter> &Array)noexcept(true){
		TConstStringTokenOperator::Discard(fStringToken);
		fStringToken=TConstStringTokenOperator::MakeStringCopy(Array.Pointer,Array.Length);
		return *this;
	}

	// string from mutable array

	cConstString(const cArray<tCharacter> &Array)noexcept(true)
		: fStringToken(TConstStringTokenOperator::MakeStringCopy(Array.Pointer,Array.Length)){}
	cConstString& operator = (const cArray<tCharacter> &Array)noexcept(true){
		TConstStringTokenOperator::Discard(fStringToken);
		fStringToken=TConstStringTokenOperator::MakeStringCopy(Array.Pointer,Array.Length);
		return *this;
	}


	// string by constant

	cConstString(const cArrayConstant<tCharacter> &Array)noexcept(true)
		: fStringToken(TConstStringTokenOperator::MakeConstant(Array)){}

	cConstString& operator = (const cArrayConstant<tCharacter> &Array)noexcept(true){
		TConstStringTokenOperator::Discard(fStringToken);
		fStringToken=TConstStringTokenOperator::MakeConstant(Array);
		return *this;
	}

	// construct by making token

#ifndef cnLibrary_CPPEXCLUDE_FUNCTION_TEMPLATE_DEFALT_ARGUMENT
	template<class T
#if cnLibrary_CPPFEATURE_DECLTYPE >= 200707L
		, class=decltype(MakeFrom(cnVar::DeclVar<T cnLib_UREF>()))
#endif
	>
	cConstString(T cnLib_UREF Src)noexcept(true)
		: fStringToken(MakeFrom(cnLib_UREFCAST(T)(Src))){}
#else	// cnLibrary_CPPEXCLUDE_FUNCTION_TEMPLATE_DEFALT_ARGUMENT
	template<class T>
	cConstString(T cnLib_UREF Src)noexcept(true)
		: fStringToken(MakeFrom(cnLib_UREFCAST(T)(Src))){}
#endif	// cnLibrary_CPPEXCLUDE_FUNCTION_TEMPLATE_DEFALT_ARGUMENT


	template<class T>
	typename cnVar::TSelect<0,cConstString&
#if cnLibrary_CPPFEATURE_DECLTYPE >= 200707L
		, decltype(MakeFrom(cnVar::DeclVar<T cnLib_UREF>()))
#endif
	>::Type operator =(T cnLib_UREF Src)noexcept(true)
	{
		TConstStringTokenOperator::Discard(fStringToken);
		fStringToken=MakeFrom(cnLib_UREFCAST(T)(Src));
		return *this;
	}

	// string by C Array

	template<uIntn MaxLength>
	cConstString(const tCharacter (&Src)[MaxLength])noexcept(true)
		: fStringToken(MakeFromArray(Src,MaxLength)){}

	template<uIntn MaxLength>
	cConstString& operator = (const tCharacter (&Src)[MaxLength])noexcept(true){
		TConstStringTokenOperator::Discard(fStringToken);
		fStringToken=MakeFromArray(Src,MaxLength);
		return *this;
	}

	// string by pointer

	cConstString(const tCharacter *Src,uIntn SrcLength)noexcept(true)
		: fStringToken(TConstStringTokenOperator::MakeStringCopy(Src,SrcLength)){}

	void SetString(const tCharacter *Src)noexcept(true){
		TConstStringTokenOperator::Discard(fStringToken);
		fStringToken=MakeFrom(Src);
	}
	void SetString(const tCharacter *Src,uIntn Length)noexcept(true){
		TConstStringTokenOperator::Discard(fStringToken);
		fStringToken=TConstStringTokenOperator::MakeStringCopy(Src,Length);
	}
	cnLib_DEPRECATED void Copy(const tCharacter *Src)noexcept(true){	return SetString(Src);	}
	cnLib_DEPRECATED void Copy(const tCharacter *Src,uIntn Length)noexcept(true){	return SetString(Src,Length);	}



	// Operators - compare array

    bool operator == (const cArray<const tCharacter> &String)const noexcept(true){
    	tTokenArray StringArray=TConstStringTokenOperator::GetArray(fStringToken);
		if(StringArray.Length!=String.Length)
			return false;
		return TKRuntime::TMemory<sizeof(tCharacter)>::Equal(StringArray.Pointer,String.Pointer,StringArray.Length);
	}
    bool operator != (const cArray<const tCharacter> &String)const noexcept(true){
    	tTokenArray StringArray=TConstStringTokenOperator::GetArray(fStringToken);
		if(StringArray.Length!=String.Length)
			return true;
		return !TKRuntime::TMemory<sizeof(tCharacter)>::Equal(StringArray.Pointer,String.Pointer,StringArray.Length);
	}
    bool operator < (const cArray<const tCharacter> &String)const noexcept(true){
    	tTokenArray StringArray=TConstStringTokenOperator::GetArray(fStringToken);
		return cnString::Compare(StringArray.Pointer,StringArray.Length,String.Pointer,String.Length)<0;
	}
    bool operator <= (const cArray<const tCharacter> &String)const noexcept(true){
    	tTokenArray StringArray=TConstStringTokenOperator::GetArray(fStringToken);
		return cnString::Compare(StringArray.Pointer,StringArray.Length,String.Pointer,String.Length)<=0;
	}
    bool operator > (const cArray<const tCharacter> &String)const noexcept(true){
    	tTokenArray StringArray=TConstStringTokenOperator::GetArray(fStringToken);
		return cnString::Compare(StringArray.Pointer,StringArray.Length,String.Pointer,String.Length)>0;
	}
    bool operator >= (const cArray<const tCharacter> &String)const noexcept(true){
    	tTokenArray StringArray=TConstStringTokenOperator::GetArray(fStringToken);
		return cnString::Compare(StringArray.Pointer,StringArray.Length,String.Pointer,String.Length)>=0;
	}

	// Operators - compare cString

	template<class TSrcConstStringTokenOperator>
    bool operator == (const cConstString<TSrcConstStringTokenOperator> &String)const noexcept(true){
		return operator ==(String.GetArray());
	}
	template<class TSrcConstStringTokenOperator>
    bool operator != (const cConstString<TSrcConstStringTokenOperator> &String)const noexcept(true){
		return operator !=(String.GetArray());
	}
	template<class TSrcConstStringTokenOperator>
    bool operator < (const cConstString<TSrcConstStringTokenOperator> &String)const noexcept(true){
		return operator <(String.GetArray());
	}
	template<class TSrcConstStringTokenOperator>
    bool operator <= (const cConstString<TSrcConstStringTokenOperator> &String)const noexcept(true){
		return operator <=(String.GetArray());
	}
	template<class TSrcConstStringTokenOperator>
    bool operator > (const cConstString<TSrcConstStringTokenOperator> &String)const noexcept(true){
		return operator >(String.GetArray());
	}
	template<class TSrcConstStringTokenOperator>
    bool operator >= (const cConstString<TSrcConstStringTokenOperator> &String)const noexcept(true){
		return operator >=(String.GetArray());
	}

	// Operators - compare string

    bool operator == (const tCharacter *String)const noexcept(true){
    	tTokenArray StringArray=TConstStringTokenOperator::GetArray(fStringToken);
		return cnString::Equal(StringArray.Pointer,String,StringArray.Length);
	}
    bool operator != (const tCharacter *String)const noexcept(true){
    	tTokenArray StringArray=TConstStringTokenOperator::GetArray(fStringToken);
		return !cnString::Equal(StringArray.Pointer,String,StringArray.Length);
	}
    bool operator < (const tCharacter *String)const noexcept(true){
    	tTokenArray StringArray=TConstStringTokenOperator::GetArray(fStringToken);
		return cnString::Compare(StringArray.Pointer,String,StringArray.Length)<0;
	}
    bool operator <= (const tCharacter *String)const noexcept(true){
    	tTokenArray StringArray=TConstStringTokenOperator::GetArray(fStringToken);
		return cnString::Compare(StringArray.Pointer,String,StringArray.Length)<=0;
	}
    bool operator > (const tCharacter *String)const noexcept(true){
    	tTokenArray StringArray=TConstStringTokenOperator::GetArray(fStringToken);
		return cnString::Compare(StringArray.Pointer,String,StringArray.Length)>0;
	}
    bool operator >= (const tCharacter *String)const noexcept(true){
    	tTokenArray StringArray=TConstStringTokenOperator::GetArray(fStringToken);
		return cnString::Compare(StringArray.Pointer,String,StringArray.Length)>=0;
	}

protected:
	tStringToken fStringToken;


	static tStringToken MakeFromArray(const tCharacter *String,uIntn MaxLength)noexcept(true){
		cnLib_ASSERT(String!=nullptr);
		if(String[0]==0){
			return TConstStringTokenOperator::MakeFrom();
		}
		return TConstStringTokenOperator::MakeStringCopy(String,cnString::FindLength(String,MaxLength));
	}
};
//---------------------------------------------------------------------------
//TStringTokenOperator : TConstStringTokenOperator
//{
//	typedef tAllocationOperator;
//
//	template<class TSrcStringOperator,class TSrcAllocationOperator,class TSrcCharacter>
//	static tStringToken MakeMoveStorage(cnDataStruct::cStringStorage<TSrcStringOperator,TSrcAllocationOperator,TSrcCharacter> &&Storage)noexcept;
//	template<class TArrayAllocation>
//	static bool ExtractStorageTo(tStringToken &Token,TArrayAllocation &Storage)noexcept;
//};
//---------------------------------------------------------------------------
template<class TStringTokenOperator>
class cString : public cConstString<TStringTokenOperator>
{
public:
	typedef typename TStringTokenOperator::tCharacter tCharacter;
	typedef typename TStringTokenOperator::tStringToken tStringToken;
	typedef typename TStringTokenOperator::tAllocationOperator tAllocationOperator;

	typedef typename cConstString<TStringTokenOperator>::tTokenArray tTokenArray;

	typedef cStringBuffer<tAllocationOperator,tCharacter> tStringBuffer;

#if cnLibrary_CPPFEATURE_INHERIT_CONSTRUCTORS >= 200802L
	using cConstString<TStringTokenOperator>::cConstString;
#endif // cnLibrary_CPPFEATURE_INHERIT_CONSTRUCTORS >= 200802L

	using cConstString<TStringTokenOperator>::operator =;

	cnLib_STATIC_ASSERT(cnVar::TIsTrivial<tCharacter>::Value,"trival character required");

#ifndef cnLibrary_CPPEXCLUDE_CLASS_MEMBER_DEFAULT
	cString()=default;
#else
	cnLib_CONSTEXPR_FUNC cString(){}
#endif // !cnLibrary_CPPEXCLUDE_CLASS_MEMBER_DEFAULT


	// string by cString

	cString(const cString &Src)noexcept(true)
		: cConstString<TStringTokenOperator>(Src){}

	cString& operator = (const cString &Src)noexcept(true){
		cConstString<TStringTokenOperator>::operator =(static_cast<const cConstString<TStringTokenOperator>&>(Src));
		return *this;
	}

#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

	cString(cString &&Src)noexcept(true)
		: cConstString<TStringTokenOperator>(static_cast<cString&&>(Src)){}

	cString& operator = (cString &&Src)noexcept(true){
		cConstString<TStringTokenOperator>::operator =(static_cast<cConstString<TStringTokenOperator>&&>(Src));
		return *this;
	}

		// string by string buffer

	template<class TSrcAllocationOperator,class TSrcCharacter>
	cString(cStringBuffer<TSrcAllocationOperator,TSrcCharacter> &&Src)noexcept(true)
		: cConstString<TStringTokenOperator>(MakeFromStringBuffer(Src)){}

	template<class TSrcAllocationOperator,class TSrcCharacter>
	cString& operator = (cStringBuffer<TSrcAllocationOperator,TSrcCharacter> &&Src)noexcept(true){
		SwapStringBuffer(Src);
		return *this;
	}

#endif	//cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

	template<class TDestAllocationOperator,class TDestCharacter>
	cnDataStruct::cStringStorage<TDestAllocationOperator,TDestCharacter> Swap(void)noexcept(true)
	{
		cnDataStruct::cStringStorage<TDestAllocationOperator,TDestCharacter> Storage;
    	tTokenArray StringArray=this->GetArray();
		if(StringArray.Length==0)
			return Storage;
		if(TStringTokenOperator::ExtractStorageTo(this->fStringToken,static_cast<cnDataStruct::cArrayAllocation<TDestAllocationOperator,TDestCharacter>&>(Storage))){
			return Storage;
		}
		Storage.Copy(StringArray.Pointer,StringArray.Length);
		this->Clear();
		return Storage;
	}
	cnDataStruct::cStringStorage<tAllocationOperator,tCharacter> Swap(void)noexcept(true)
	{
		return Swap<tAllocationOperator,tCharacter>();
	}

#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

	template<class TDestAllocationOperator,class TDestCharacter,class TSrcArrayStorage>
	cnDataStruct::cStringStorage<TDestAllocationOperator,TDestCharacter> Swap(TSrcArrayStorage cnLib_UREF Src)noexcept(true){
    	tTokenArray StringArray=this->GetArray();
		// swap current
		cnDataStruct::cStringStorage<TDestAllocationOperator,TDestCharacter> Swapped;
		if(StringArray.Length!=0){
			if(TStringTokenOperator::ExtractStorageTo(this->fStringToken,Swapped.Storage())==false){
				Swapped.Copy(StringArray.Pointer,StringArray.Length);
			}
		}
		// move src
		TStringTokenOperator::Discard(this->fStringToken);
		this->fStringToken=TStringTokenOperator::MakeMoveStorage(cnLib_UREFCAST(TSrcArrayStorage)(Src));
		return Swapped;
	}

	template<class TSrcArrayStorage>
	cnDataStruct::cStringStorage<tAllocationOperator,tCharacter> Swap(TSrcArrayStorage cnLib_UREF Src)noexcept(true){
		return Swap<tAllocationOperator,TSrcArrayStorage>(cnLib_UREFCAST(TSrcArrayStorage)(Src));
	}
#endif	//cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

	tStringBuffer operator + (const tCharacter *String)const
#if cnLibrary_CPPFEATURE_REF_QUALIFIERS >= 200710L
	&
#endif
		noexcept(true)
	{
    	tTokenArray StringArray=this->GetArray();
		tStringBuffer Buffer(StringArray.Pointer,StringArray.Length);
		Buffer+=String;
		return Buffer;
	}
#if cnLibrary_CPPFEATURE_REF_QUALIFIERS >= 200710L
	tStringBuffer operator + (const tCharacter *String)&&noexcept(true)
	{
		auto Buffer=ExtractBuffer();
		Buffer+=String;
		return Buffer;
	}
#endif

	template<class TSrcStringTokenOperator>
	tStringBuffer operator + (const cString<TSrcStringTokenOperator> &Src)const
#if cnLibrary_CPPFEATURE_REF_QUALIFIERS >= 200710L
	&
#endif
		noexcept(true)
	{
    	tTokenArray StringArray=this->GetArray();
		if(StringArray.Length==0){
			return tStringBuffer(StringArray.Pointer,StringArray.Length);
		}
		else{
			tStringBuffer Buffer(StringArray.Pointer,StringArray.Length);
			Buffer.Append(Src->Pointer,Src->Length);
			return Buffer;
		}
	}


#if cnLibrary_CPPFEATURE_REF_QUALIFIERS >= 200710L
	template<class TSrcStringTokenOperator>
    tStringBuffer operator + (const cString<TSrcStringTokenOperator> &Src)&& noexcept(true){
		auto Buffer=ExtractBuffer();
		Buffer.Append(Src->Pointer,Src->Length);
		return Buffer;
	}
#endif
	
	// cstr + cString&
	friend tStringBuffer operator + (const tCharacter *String,const cString &Src){
		tStringBuffer r=String;
		r.Append(Src->Pointer,Src->Length);
		return r;
	}

#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

	template<class TSrcStringTokenOperator>
	tStringBuffer operator + (cString<TSrcStringTokenOperator> &&Src)const
#if cnLibrary_CPPFEATURE_REF_QUALIFIERS >= 200710L
	&
#endif
		noexcept(true)
	{
    	tTokenArray StringArray=this->GetArray();
		if(StringArray.Length==0){
			return Src.template Swap<tAllocationOperator,tCharacter>();
		}
		tStringBuffer Buffer(StringArray.Pointer,StringArray.Length);
		Buffer.Append(Src->Pointer,Src->Length);
		return Buffer;
	}
#if cnLibrary_CPPFEATURE_REF_QUALIFIERS >= 200710L
	template<class TSrcStringTokenOperator>
    tStringBuffer operator + (cString<TSrcStringTokenOperator> &&Src)&& noexcept(true){
    	tTokenArray StringArray=this->GetArray();
		if(StringArray.Length==0){
			return Src.template Swap<tAllocationOperator,tCharacter>();
		}
		auto Buffer=ExtractBuffer();
		Buffer.Append(StringArray.Pointer,StringArray.Length);
		return Buffer;
	}
#endif
		
	// cstr + cString&&
	template<class TSrcStringTokenOperator>
	friend tStringBuffer operator + (const tCharacter *String,cString<TSrcStringTokenOperator> &&Src){
		tStringBuffer Buffer(Src.template Swap<tAllocationOperator,tCharacter>());
		Buffer.Replace(0,0,GetLength(String),String);
		return Buffer;
	}

#endif	// cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

	cString(tCharacter Src,uIntn Length)noexcept(true){
		if(Length==0){
			return;
		}
		tStringBuffer Buffer;
		Buffer.SetLength(Length);
		TKRuntime::TArray<sizeof(tCharacter)>::Fill(Buffer->Pointer,Length,Src);
		SwapStringBuffer(Buffer);
	}

	void Append(const tCharacter *Src)noexcept(true){
		if(Src==nullptr){
			return;
		}
		uIntn Length=cnString::FindLength(Src);
		if(Length==0){
			return;
		}
		auto Buffer=ExtractBuffer();
		Buffer.Append(Src,Length);
		SwapStringBuffer(Buffer);
	}

    cString& operator += (const tCharacter *String)noexcept(true)
	{
		Append(String);
		return *this;
	}

	void Append(const tCharacter *Src,uIntn Length)noexcept(true){
		if(Src==nullptr || Length==0){
			return;
		}
		auto Buffer=ExtractBuffer();
		Buffer.Append(Src,Length);
		SwapStringBuffer(Buffer);
	}

	void Append(const cArray<tCharacter> &Array)noexcept(true){
		return Append(Array.Pointer,Array.Length);
	}
    cString& operator += (const cArray<tCharacter> &Src)noexcept(true)
	{
		Append(Src.Pointer,Src.Length);
		return *this;
	}

#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

	template<class TSrcStringTokenOperator>
    void Append(cString<TSrcStringTokenOperator> &&Src)noexcept(true)
	{
		if(Src->Length==0)
			return;
		if(this->GetLength()==0){
			TStringTokenOperator::Discard(this->fStringToken);
			this->fStringToken=TStringTokenOperator::MakeMoveStorage(Src.Swap());
			return;
		}
		auto Buffer=ExtractBuffer();
		Buffer.Append(Src->Pointer,Src->Length);
		SwapStringBuffer(Buffer);
	}
	template<class TSrcStringTokenOperator>
    cString& operator += (cString<TSrcStringTokenOperator> &&Src)noexcept(true)
	{
		Append(static_cast<cString<TSrcStringTokenOperator>&&>(Src));
		return *this;
	}
#endif	// cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

	void Insert(const tCharacter *Src,uIntn Index)noexcept(true){
		if(Src==nullptr){
			return;
		}
		uIntn Length=TKRuntime::TString<sizeof(tCharacter)>::QueryLength(Src);
		if(Length==0){
			return;
		}
		auto Buffer=ExtractBuffer();
		Buffer.Replace(Index,0,Length,Src);
		SwapStringBuffer(Buffer);
    }
	void Insert(const tCharacter *Src,uIntn Index,uIntn Length)noexcept(true){
		if(Src==nullptr){
			return;
		}
		if(Length==0){
			return;
		}
		auto Buffer=ExtractBuffer();
		Buffer.Replace(Index,0,Length,Src);
		SwapStringBuffer(Buffer);
    }
    void Remove(uIntn From,uIntn Length)noexcept(true){
		if(Length==0){
			return;
		}
		if(From==0 && Length>=this->GetLength()){
			this->Clear();
			return;
		}
		auto Buffer=ExtractBuffer();
		Buffer.Replace(From,Length,0);
		SwapStringBuffer(Buffer);
    }
	void Replace(uIntn Index,uIntn Length,const tCharacter *ReplaceString,uIntn ReplaceStringLength)noexcept(true){
		if(Index==0 && Length>=this->GetLength()){
			return this->SetString(ReplaceString,ReplaceStringLength);
		}
		auto Buffer=ExtractBuffer();
		Buffer.Replace(Index,Length,ReplaceStringLength,ReplaceString);
		SwapStringBuffer(Buffer);
	}
	void Cut(uIntn From,uIntn Length)noexcept(true){
    	tTokenArray StringArray=this->GetArray();
		if(From>StringArray.Length){
			this->Clear();
			return;
		}
		uIntn RemainLength=StringArray.Length-From;
		if(Length>RemainLength)
			Length=RemainLength;
		if(Length==0){
			this->Clear();
			return;
		}
		tStringBuffer Buffer;
		if(TStringTokenOperator::ExtractStorageTo(this->fStringToken,Buffer.Storage())){
			Buffer.Replace(0,From,0);
			Buffer.Truncate(Length);
		}
		else{
			Buffer.SetString(StringArray.Pointer+From,Length);
		}

		SwapStringBuffer(Buffer);
    }
    cString SubString(uIntn From,uIntn Length)const
#if cnLibrary_CPPFEATURE_REF_QUALIFIERS >= 200710L
	&
#endif
		noexcept(true)
	{
    	tTokenArray StringArray=this->GetArray();
		if(From>StringArray.Length){
			return cString();
		}
		uIntn RemainLength=StringArray.Length-From;
		if(Length>RemainLength)
			Length=RemainLength;
		if(Length==0)
			return cString();
		return cString(StringArray.Pointer+From,Length);
    }
#if cnLibrary_CPPFEATURE_REF_QUALIFIERS >= 200710L
    cString SubString(uIntn From,uIntn Length)const&& noexcept(true)
	{
    	tTokenArray StringArray=this->GetArray();
		if(From>StringArray.Length){
			return cString();
		}
		uIntn RemainLength=StringArray.Length-From;
		if(Length>RemainLength)
			Length=RemainLength;
		if(Length==0)
			return cString();

		tStringBuffer Buffer;
		if(TStringTokenOperator::ExtractStorageTo(this->fStringToken,Buffer.Storage())){
			Buffer.Replace(0,From,0);
			Buffer.Truncate(Length);
		}
		else{
			Buffer.SetString(StringArray.Pointer+From,Length);
		}
		SwapStringBuffer(Buffer);
		return static_cast<cString&&>(this);
    }
#endif

protected:

	tStringBuffer ExtractBuffer(void)noexcept(true)
	{
    	tTokenArray StringArray=this->GetArray();
		tStringBuffer Buffer;
		if(StringArray.Length==0)
			return Buffer;
		if(TStringTokenOperator::ExtractStorageTo(this->fStringToken,Buffer.Storage())){
			return Buffer;
		}
		Buffer.SetString(StringArray.Pointer,StringArray.Length);
		return Buffer;
	}

	template<class TSrcAllocationOperator,class TSrcCharacter>
	tStringToken MakeFromStringBuffer(cStringBuffer<TSrcAllocationOperator,TSrcCharacter> &Storage)noexcept(true){
		typedef cnDataStruct::cStringStorage<TSrcAllocationOperator,TSrcCharacter> TStringStorage;
		return TStringTokenOperator::MakeMoveStorage(cnLib_UREFCAST(TStringStorage)(Storage.Storage()));
	}
	template<class TSrcAllocationOperator,class TSrcCharacter>
	void SwapStringBuffer(cStringBuffer<TSrcAllocationOperator,TSrcCharacter> &Storage)noexcept(true){
		TStringTokenOperator::Discard(this->fStringToken);
		this->fStringToken=MakeFromStringBuffer(Storage);
	}

};

//---------------------------------------------------------------------------

template<class TCharacter>
class cRefStringArray : public iArrayReference<const TCharacter>, public cArray<const TCharacter>
{
public:
#ifndef cnLibrary_CPPEXCLUDE_CLASS_MEMBER_DEFAULT
	cRefStringArray()=default;
#else
	cnLib_CONSTEXPR_FUNC cRefStringArray()noexcept(true){};
#endif
#if cnLibrary_CPPFEATURE_INITIALIZER_LIST >= 200806L
	cnLib_CONSTEXPR_FUNC cRefStringArray(const TCharacter* String,uIntn Length)noexcept(true)
		: cArray<const TCharacter>{const_cast<TCharacter*>(String),Length} {}
#else	// cnLibrary_CPPFEATURE_INITIALIZER_LIST < 200806L
	cRefStringArray(const TCharacter* String,uIntn Length)noexcept(true){
		this->Pointer=const_cast<TCharacter*>(String);
		this->Length=Length;
	}
#endif	// cnLibrary_CPPFEATURE_INITIALIZER_LIST
	
	virtual const TCharacter* cnLib_FUNC GetArray(uIntn &RetLength)noexcept(true) override{	RetLength=this->Length;	return this->Pointer;	}
	virtual uIntn ExtractStorage(tTypeID){		return 0;	}

	virtual void cnLib_FUNC IncreaseReference(void)noexcept(true) override{}
	virtual void cnLib_FUNC DecreaseReference(void)noexcept(true) override{}

};

//---------------------------------------------------------------------------

template<class TCharacter,class TAllocationOperator>
class cRefStringArrayConstant : public cRefStringArray<TCharacter>
{
public:
	typedef TCharacter tCharacter;

	static cRefStringArrayConstant* ByConstant(const cArrayConstant<tCharacter> &Constant)noexcept(true){
		return new cRefStringArrayConstant(Constant.Pointer,Constant.Length);
	}
	

	virtual void cnLib_FUNC IncreaseReference(void)noexcept(true) override{
		fRefCount.Free++;
	}
	virtual void cnLib_FUNC DecreaseReference(void)noexcept(true) override{
		if(fRefCount.Free--==1){
			delete this;
		}
	}
protected:
	cRefStringArrayConstant(const tCharacter *Pointer,uIntn Length)noexcept(true)
		: cRefStringArray<tCharacter>(const_cast<tCharacter*>(Pointer),Length)
		, fRefCount(1)
	{
	}
	~cRefStringArrayConstant()=default;

	void* operator new (tSize Size)noexcept(true){
		return TAllocationOperator::Allocate(Size,cnMemory::TAlignmentOf<cRefStringArrayConstant>::Value);
	}
#if cnLibrary_CPPFEATURE_SIZED_DEALLOCATION >= 201309L
	void operator delete(void *Ptr,tSize Size)noexcept(true){
		TAllocationOperator::Deallocate(Ptr,Size,cnMemory::TAlignmentOf<cRefStringArrayConstant>::Value);
	}
#else	// cnLibrary_CPPFEATURE_SIZED_DEALLOCATION < 201309L
	void operator delete(void *Ptr)noexcept(true){
		TAllocationOperator::Deallocate(Ptr,sizeof(cRefStringArrayConstant),cnMemory::TAlignmentOf<cRefStringArrayConstant>::Value);
	}
#endif	// cnLibrary_CPPFEATURE_SIZED_DEALLOCATION

	cnAsync::cAtomicVariable<typename cnVar::TSelect<0,uIntn,TCharacter>::Type> fRefCount;
};
//---------------------------------------------------------------------------
template<class TAllocationOperator,class TCharacter>
class cRefStringArrayStorage : public cRefStringArray<TCharacter>
{
protected:
	typedef TCharacter tCharacter;
	cnAsync::cAtomicVariable<typename cnVar::TSelect<0,uIntn,TCharacter>::Type> fRefCount;


	static sIntn CalculateOffset(uIntn Length)noexcept(true){
		sIntn Offset=(Length+1)*sizeof(tCharacter);
		Offset=cnMath::IntRoundUp(Offset,cnMemory::TAlignmentOf<cRefStringArrayStorage>::Value);
		return Offset;
	}

	cnDataStruct::cStringStorage<TAllocationOperator,TCharacter> Extract(void)noexcept(true){
		sIntn Offset=CalculateOffset(this->Length);
		uIntn CapacitySize=Offset+sizeof(cRefStringArrayStorage);

		cnDataStruct::cStringStorage<TAllocationOperator,TCharacter> Array;
		Array.Pointer=const_cast<tCharacter*>(this->Pointer);
		Array.Length=this->Length;
		Array.Capacity=cnMath::IntRoundUpDiv(CapacitySize,sizeof(tCharacter));

		this->~cRefStringArrayStorage();
		return Array;
	}
public:
	cnLib_CONSTEXPR_FUNC cRefStringArrayStorage():fRefCount(1){}

	virtual uIntn ExtractStorage(tTypeID TypeID)noexcept(true) override{
		if(TypeID!=cnVar::TTypeID< cnDataStruct::cArrayAllocation<TAllocationOperator,TCharacter> >::Value){
			return 0;
		}
		if(fRefCount!=1){
			// cannot extract
			return 0;
		}
		auto Storage=Extract();
		uIntn Capacity=Storage.Capacity;
		Storage.Reset();
		return Capacity;
	}

	static cRefStringArrayStorage* ByCopy(const tCharacter *Src,uIntn Length)noexcept(true){
		sIntn Offset=CalculateOffset(Length);
		uIntn CapacitySize=Offset+sizeof(cRefStringArrayStorage);

		auto NewString=static_cast<tCharacter*>(TAllocationOperator::Allocate(CapacitySize,cnMemory::TAlignmentOf<cRefStringArrayStorage>::Value));
		
		auto *NewToken=cnMemory::CastPointerAddByteOffset<cRefStringArrayStorage>(NewString,Offset);
		cnVar::ManualConstruct(*NewToken);
		NewToken->Pointer=NewString;
		NewToken->Length=Length;

		TKRuntime::TMemory<sizeof(tCharacter)>::Copy(NewString,Src,Length);
		NewString[Length]=0;

		return NewToken;

	}
	
#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L
	static cRefStringArrayStorage* ByArray(cnDataStruct::cStringStorage<TAllocationOperator,TCharacter> &&Array)noexcept(true){
		sIntn Offset=CalculateOffset(Array.Length);
		uIntn NewCapacitySize=Offset+sizeof(cRefStringArrayStorage);

		tCharacter *NewPointer=Array.Pointer;
		if(Array.Capacity*sizeof(tCharacter)<NewCapacitySize){
			if(Array.Capacity==0){
				NewPointer=static_cast<tCharacter*>(TAllocationOperator::Allocate(NewCapacitySize,cnMemory::TAlignmentOf<cRefStringArrayStorage>::Value));
			}
			else{
				// grow
				bool ManualCopy;
				NewPointer=static_cast<tCharacter*>(TAllocationOperator::Relocate(Array.Pointer,Array.Capacity,sizeof(tCharacter),NewCapacitySize,cnMemory::TAlignmentOf<cRefStringArrayStorage>::Value,ManualCopy));
				if(ManualCopy){
					TKRuntime::TMemory<sizeof(tCharacter)>::Copy(NewPointer,Array.Pointer,Array.Length);
					TAllocationOperator::Deallocate(Array.Pointer,Array.Capacity,sizeof(tCharacter));
				}
			}
		}

		auto *NewToken=cnMemory::CastPointerAddByteOffset<cRefStringArrayStorage>(NewPointer,Offset);

		cnVar::ManualConstruct(*NewToken);
		NewToken->Pointer=NewPointer;
		NewToken->Length=Array.Length;

		Array.Pointer=nullptr;
		Array.Length=0;
		Array.Capacity=0;

		return NewToken;

	}
#endif	// cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L
	

	virtual void cnLib_FUNC IncreaseReference(void)noexcept(true) override{
		fRefCount.Free++;
	}
	virtual void cnLib_FUNC DecreaseReference(void)noexcept(true) override{
		if(fRefCount.Free--==1){
			Extract();
		}
	}

};


//---------------------------------------------------------------------------
#if cnLibrary_CPPFEATURE_CONSTINIT >= 201907L
// cnLibrary_CPPFEATURE_CONSTINIT >= 201907L

template<class TCharacter>
struct TRefEmptyStringArray
{
	static cRefStringArray<TCharacter> Value;
};

template<class TCharacter>
constinit cRefStringArray<TCharacter> TRefEmptyStringArray<TCharacter>::Value={TEmptyString<TCharacter>::Value,0};

template<class TCharacter>
inline cRefStringArray<TCharacter>* RefEmptyStringArray(void){
	return &TRefEmptyStringArray<TCharacter>::Value;
}

#else
// cnLibrary_CPPFEATURE_CONSTINIT < 201907L

#if cnLibrary_CPPFEATURE_CONSTEXPR >= 200704L && !defined(cnLibrary_CPPEXCLUDE_CONSTEXPR_STATIC_INITALIZATION)

template<class TCharacter>
struct TRefEmptyStringArray
{
	static constexpr cRefStringArray<TCharacter> Value={TEmptyString<TCharacter>::Value,0};
};

template<class TCharacter>
constexpr cRefStringArray<TCharacter> TRefEmptyStringArray<TCharacter>::Value;

template<class TCharacter>
inline cRefStringArray<TCharacter>* RefEmptyStringArray(void){
	return const_cast<cRefStringArray<TCharacter>*>(&TRefEmptyStringArray<TCharacter>::Value);
}
// cnLibrary_CPPFEATURE_CONSTEXPR >= 200704L && !defined(cnLibrary_CPPEXCLUDE_CONSTEXPR_STATIC_INITALIZATION)
#else
// cnLibrary_CPPFEATURE_CONSTEXPR < 200704L || defined(cnLibrary_CPPEXCLUDE_CONSTEXPR_STATIC_INITALIZATION)

template<class TCharacter>
struct TRefEmptyStringArray
{
	static bool Initialized;
	static cnVar::cStaticVariable< cRefStringArray<TCharacter> > Value;
};

template<class TCharacter>
bool TRefEmptyStringArray<TCharacter>::Initialized=false;

template<class TCharacter>
cnVar::cStaticVariable< cRefStringArray<TCharacter> > TRefEmptyStringArray<TCharacter>::Value;


template<class TCharacter>
inline cRefStringArray<TCharacter>* RefEmptyStringArray(void){
	if(TRefEmptyStringArray<TCharacter>::Initialized){
		return &TRefEmptyStringArray<TCharacter>::Value;
	}
	TRefEmptyStringArray<TCharacter>::Initialized=true;
	TRefEmptyStringArray<TCharacter>::Value.Construct(TEmptyString<TCharacter>::Value,0);
	return &TRefEmptyStringArray<TCharacter>::Value;
}

#endif	// cnLibrary_CPPFEATURE_CONSTEXPR < 200704L || defined(cnLibrary_CPPEXCLUDE_CONSTEXPR_STATIC_INITALIZATION)

#endif // cnLibrary_CPPFEATURE_CONSTINIT < 201907L

//---------------------------------------------------------------------------

template<class TAllocationOperator,class TCharacter>
struct cRefStringArrayTokenOperator
{
	typedef TAllocationOperator tAllocationOperator;
	typedef TCharacter tCharacter;
	typedef cRefStringArray<TCharacter>* tStringToken;

	static const tCharacter* GetPointer(const tStringToken &Token)noexcept(true){	return Token->Pointer;	}
	static uIntn GetLength(const tStringToken &Token)noexcept(true){				return Token->Length;	}

	static const cArray<const tCharacter>& GetArray(const tStringToken &Token)noexcept(true){	return *Token;	};

	static tStringToken Duplicate(const tStringToken &Token)noexcept(true){	Token->IncreaseReference();	return Token;	}

#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L
	static tStringToken Move(tStringToken &&Token)noexcept(true){	tStringToken MovedToken=Token;	Token=MakeFrom();	return MovedToken;	}
#endif

	static void Discard(tStringToken &Token)noexcept(true){	Token->DecreaseReference();	}

	static tStringToken MakeFrom(void)noexcept(true){
		return RefEmptyStringArray<tCharacter>();
	}
	static tStringToken MakeStringCopy(const tCharacter *String,uIntn Length)noexcept(true){
		if(Length==0){
			return MakeFrom();
		}
		cnLib_ASSERT(String!=nullptr);
		return cRefStringArrayStorage<TAllocationOperator,TCharacter>::ByCopy(String,Length);
	}

	static tStringToken MakeConstant(const cArrayConstant<tCharacter> &Array)noexcept(true){
		return cRefStringArrayConstant<tCharacter,TAllocationOperator>::ByConstant(Array);
	}

	// TStringTokenOperator


#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L
	template<class TSrcAllocationOperator,class TSrcCharacter>
	static tStringToken MakeMoveStorage(cnDataStruct::cStringStorage<TSrcAllocationOperator,TSrcCharacter> &&Storage)noexcept(true){
		if(Storage.Length==0){
			return MakeFrom();
		}
		else{
			return cRefStringArrayStorage<TSrcAllocationOperator,TSrcCharacter>::ByArray(
				static_cast<cnDataStruct::cArrayAllocation<TSrcAllocationOperator,TSrcCharacter>&&>(Storage)
			);
		}
	}
#endif	// cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

	template<class TArrayAllocation>
	static bool ExtractStorageTo(tStringToken &Token,TArrayAllocation &Storage)noexcept(true){

		Storage.Pointer=const_cast<tCharacter*>(Token->Pointer);
		Storage.Length=Token->Length;
		Storage.Capacity=Token->ExtractStorage(cnVar::TTypeID<TArrayAllocation>::Value);
		if(Storage.Capacity){
			Token=MakeFrom();
			return true;
		}
		// failed to extract
		Storage.Pointer=nullptr;
		Storage.Length=0;
		return false;
	}


};

//---------------------------------------------------------------------------
} 	// namespace cnString
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif // cnLibrary_CPPFEATURELEVEL >= 3
//---------------------------------------------------------------------------
#include <cnTK/TKMacrosCleanup.inc>
//---------------------------------------------------------------------------
#endif  /* cnLibrary_CPPFEATURELEVEL >= 1 */
/*-------------------------------------------------------------------------*/
#endif
