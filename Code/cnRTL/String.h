/*- cnRTL - String Function -----------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2006-10-02                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnRTL_String_H__
#define __cnLibrary_cnRTL_String_H__
//---------------------------------------------------------------------------
#include <cnRTL/cnRTLHeader.h>
#include <cnRTL/RuntimeFunction.h>
#ifdef __cplusplus
//---------------------------------------------------------------------------
#if cnLibrary_CPPFEATURE_CONSTEXPR >= 200704L
#define	cnRTL_CONSTVAR	constexpr
#define	cnRTL_CONSTEXPR_FUNC constexpr
// cnLibrary_CPPFEATURE_CONSTEXPR >= 200704L
#else
// cnLibrary_CPPFEATURE_CONSTEXPR < 200704L
#define	cnRTL_CONSTEXPR_FUNC
#define	cnRTL_CONSTVAR	const
#endif	// cnLibrary_CPPFEATURE_CONSTEXPR < 200704L

#if cnLibrary_CPPFEATURE_IF_CONSTEXPR >= 201606L
#define	cnRTL_IFCONSTEXPR	constexpr
#else
#define	cnRTL_IFCONSTEXPR
#endif

//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnString{

// Search
//	Search a substring from string
//	[in]Src	string
//	[in]Dest	substring to search
//	return: the position(index) of first matched string
//  return uintMax if not found
template<class TCharacter>
inline uIntn Search(const TCharacter *Src,const typename cnVar::TTypeDef<TCharacter>::Type *Dest)
{
	cnLib_ASSERT(Src!=nullptr);
	cnLib_ASSERT(Dest!=nullptr);

	uIntn DestLength=GetLength(Dest);
	uIntn i;
	for(i=0;Src[i]!=0;i++){
		auto Pos=DiffIndexRange(Src+i,Dest,DestLength);
		if(Pos>=DestLength){
			return i;
		}
	}
	return IndexNotFound;
}


// KMP_Search_GetShiftTable
//	Get KMP search shift table
//	[in]Dest		substring to search
//	[out]ShiftTable	shift table of substring, the table should have same length as Dest
//	[in]Length		max length of string or shift table
//	return: length
template<class TCharacter>
inline uIntn KMP_Search_GetShiftTable(const TCharacter *Dest,uIntn *ShiftTable,uIntn Length)
{
	cnLib_ASSERT(Dest!=nullptr);
	cnLib_ASSERT(ShiftTable!=nullptr);

	if(Dest[0]==0)
    	return 0;
	uIntn DIndex=0;
    uIntn k;
    k=ShiftTable[0]=IndexNotFound;
	do{
    	while(k!=IndexNotFound && Dest[DIndex]!=Dest[k])
        	k=ShiftTable[k];
        DIndex++;
		k++;
		if(DIndex>=Length)
			break;
        if(Dest[DIndex]==Dest[k])
	    	ShiftTable[DIndex]=ShiftTable[k];
        else
            ShiftTable[DIndex]=k;
	}while(Dest[DIndex]!=0);
	return Length;
}

template<class TCharacter,uIntn SrcLength,uIntn TableLength>
inline uIntn KMP_Search_GetShiftTable(const TCharacter (&Dest)[SrcLength],const uIntn (&ShiftTable)[TableLength],uIntn Length=SrcLength){
	static constexpr uIntn MaxLength=TableLength>SrcLength?SrcLength:TableLength;
	return KMP_Search_GetShiftTable(Dest,ShiftTable,Length>MaxLength?MaxLength:Length);
}

// KMP_Search
//	Search a string (KMP)
//	[in]Src	string
//	[in]Dest	substring
//	[in]ShiftTable	shift table for sub string
//	return: the position(index) of first matched string,	uintMax if not found
template<class TCharacter>
inline uIntn KMP_Search(const TCharacter *Src,const TCharacter *Dest,const uIntn *ShiftTable)
{
	cnLib_ASSERT(Src!=nullptr);
	cnLib_ASSERT(Dest!=nullptr);
	cnLib_ASSERT(ShiftTable!=nullptr);

	uIntn DestIndex;
    const TCharacter *OldSrc=Src;
    DestIndex=0;
    for(;;){
        if(DestIndex!=IndexNotFound){
        	if(Dest[DestIndex]==0)
	        	return static_cast<uIntn>(Src-OldSrc)-DestIndex;
    		if(Dest[DestIndex]!=*Src){
        		DestIndex=ShiftTable[DestIndex];
                continue;
            }
        }
        if(*Src==0)
        	return IndexNotFound;
        Src++;
        DestIndex++;
    }
}

// KMP_Search
//	Search a string (KMP)
//	[in]Src	string
//	[in]Dest	substring
//	[in]ShiftTable	shift table for sub string
//	return: the position(index) of first matched string,	uintMax if not found
template<class TSrc,class TCharacter>
inline uIntn KMP_Search(TSrc &Src,const TCharacter *Dest,const uIntn *ShiftTable)
{
	cnLib_ASSERT(Dest!=nullptr);
	cnLib_ASSERT(ShiftTable!=nullptr);

	uIntn DestIndex;
    const TCharacter *OldSrc=Src;
    DestIndex=0;
    for(;;){
        if(DestIndex!=IndexNotFound){
        	if(Dest[DestIndex]==0)
	        	return static_cast<uIntn>(Src-OldSrc)-DestIndex;
    		if(Dest[DestIndex]!=*Src){
        		DestIndex=ShiftTable[DestIndex];
                continue;
            }
        }
        if(*Src==0)
        	return IndexNotFound;
        Src++;
        DestIndex++;
    }
}
//---------------------------------------------------------------------------
}	// namespace cnString
//---------------------------------------------------------------------------
namespace cnRTL{
//---------------------------------------------------------------------------
#if cnLibrary_CPPFEATURE_USER_DEFINED_LITERALS >= 200809L
	
inline cArrayConstant<uChar8> operator ""_cArray(const uChar8 *Ptr,tSize Length){
	cArrayConstant<uChar8> r;
	r.Pointer=Ptr;
	r.Length=Length;
	return r;
}
inline cArrayConstant<uChar16> operator ""_cArray(const uChar16 *Ptr,tSize Length){
	cArrayConstant<uChar16> r;
	r.Pointer=Ptr;
	r.Length=Length;
	return r;
}
inline cArrayConstant<uChar32> operator ""_cArray(const uChar32 *Ptr,tSize Length){
	cArrayConstant<uChar32> r;
	r.Pointer=Ptr;
	r.Length=Length;
	return r;
}

inline cString<uChar8> operator ""_cString(const uChar8 *Ptr,tSize Length){
	cArrayConstant<uChar8> r;
	r.Pointer=Ptr;
	r.Length=Length;
	return r;
}
inline cString<uChar16> operator ""_cString(const uChar16 *Ptr,tSize Length){
	cArrayConstant<uChar16> r;
	r.Pointer=Ptr;
	r.Length=Length;
	return r;
}
inline cString<uChar32> operator ""_cString(const uChar32 *Ptr,tSize Length){
	cArrayConstant<uChar32> r;
	r.Pointer=Ptr;
	r.Length=Length;
	return r;
}




//---------------------------------------------------------------------------
namespace StringStream{
//---------------------------------------------------------------------------
extern const char HexBytesWrite_HexTextMap[];

template<class TStreamWriteBuffer>
inline bool WriteHexBytes(TStreamWriteBuffer&& WriteBuffer,const void *Data,uIntn Length)
{
	auto CachedWriteBuffer=cnRTL::CachedStreamWriteBuffer<16>(WriteBuffer);

	uIntn ByteIndex=0;
	while(ByteIndex<Length){
		auto Memory=CachedWriteBuffer.ReserveWriteBuffer((Length-ByteIndex)*2);
		if(Memory.Length==0){
			return false;
		}
		uIntn TextIndex=0;
		while(TextIndex+2<Memory.Length){
			uInt8 CurByte=static_cast<const uInt8*>(Data)[ByteIndex++];
			Memory.Pointer[TextIndex]=HexBytesWrite_HexTextMap[CurByte>>4];
			Memory.Pointer[TextIndex+1]=HexBytesWrite_HexTextMap[CurByte&0xF];
			TextIndex+=2;

			if(ByteIndex>=Length)
				break;
		}
		CachedWriteBuffer.CommitWriteBuffer(TextIndex);
	}

	return true;
}

//---------------------------------------------------------------------------
}	// namespace StringStream
//---------------------------------------------------------------------------

struct cStringWriteHexBytes
{
	const void *Data;
	uIntn Length;
	
	template<class TStreamWriteBuffer>
	friend bool operator +=(TStreamWriteBuffer&& WriteBuffer,const cStringWriteHexBytes &Src){
		return StringStream::WriteHexBytes(WriteBuffer,Src.Data,Src.Length);
	}
};

inline cStringWriteHexBytes StringWriteHexBytes(const void *Data,uIntn Length){
	cStringWriteHexBytes StringWrite={
		Data,
		Length,
	};
	return StringWrite;
}


//---------------------------------------------------------------------------
namespace StringStream{
//---------------------------------------------------------------------------

template<class TStreamWriteBuffer,class TSrcChar>
inline bool WriteConvertEncoding(TStreamWriteBuffer &WriteBuffer,iTextEncodingConverter *Converter,const TSrcChar *Src,uIntn SrcLength)
{
	typedef typename TStreamWriteBuffer::tElement TCharacter;
	if(Converter==nullptr){
		uIntn CopyLength=SrcLength;
		if(CopyLength==0)
			return 0;
		auto WriteMemory=WriteBuffer.ReserveWriteBuffer(CopyLength);
		if(CopyLength>WriteMemory.Length)
			CopyLength=WriteMemory.Length;
		for(uIntn i=0;i<CopyLength;i++){
			WriteMemory.Pointer[i]=static_cast<TCharacter>(Src[i]);
		}
		WriteBuffer.CommitWriteBuffer(CopyLength);
		return true;
	}
	static cnRTL_CONSTVAR uIntn ConvertScale=sizeof(TSrcChar)>sizeof(TCharacter)?sizeof(TSrcChar):sizeof(TCharacter);
	while(SrcLength>0){
		uIntn SrcConvertedSize=0;
		
		uIntn EstimateLength=SrcLength*ConvertScale/sizeof(TCharacter);
		if(EstimateLength<16)
			EstimateLength=16;

		cArray<TCharacter> WriteMemory=WriteBuffer.ReserveWriteBuffer(EstimateLength);
		if(WriteMemory.Length==0)
			break;

		uIntn StreamConvertedSize=Converter->Convert(WriteMemory.Pointer,WriteMemory.Length*sizeof(TCharacter),Src,SrcLength*sizeof(TSrcChar),&SrcConvertedSize);
		if(StreamConvertedSize==0){
			WriteBuffer.CommitWriteBuffer(0);
			break;
		}

		uIntn StreamConvertedLength=StreamConvertedSize/sizeof(TCharacter);

		WriteBuffer.CommitWriteBuffer(StreamConvertedLength);

		uIntn SrcConvertedLength=SrcConvertedSize/sizeof(TSrcChar);
		Src+=SrcConvertedLength;
		SrcLength-=SrcConvertedLength;
	}
	return true;
}

template<class TStringWriteStream,class TSrcChar>
inline bool WriteConvertEncoding(TStringWriteStream&& WriteBuffer,iTextEncodingConverter *Converter,const TSrcChar *Src,uIntn SrcLength)
{
	return WriteConvertEncoding(WriteBuffer,Converter,Src,SrcLength);
}
//---------------------------------------------------------------------------
}	// namespace StringStream
//---------------------------------------------------------------------------
template<class TCharacter>
struct cStringWriteConvertEncoding
{
	iTextEncodingConverter *Converter;
	const TCharacter *Src;
	uIntn SrcLength;
	
	template<class TStreamWriteBuffer>
	friend bool operator +=(TStreamWriteBuffer&& WriteBuffer,const cStringWriteConvertEncoding &Src){
		return StringStream::WriteConvertEncoding(WriteBuffer,Src.Converter,Src.Src,Src.SrcLength);
	}
};

template<class TCharacter>
cStringWriteConvertEncoding<TCharacter> StringWriteConvertEncoding(iTextEncodingConverter *Converter,const TCharacter *Src,uIntn SrcLength){
	cStringWriteConvertEncoding<TCharacter> StringWrite={
		Converter,
		Src,
		SrcLength
	};
	return StringWrite;
}

cnLib_ENUM_BEGIN(ufInt8,CharacterConversionSymbol)
{
	Blank,
	Dot
}cnLib_ENUM_END(CharacterConversionSymbol);

// TCharacterConversionMap
//{
//	static TCharacter Ascii(char c);
//	static TCharacter Symbol(eCharacterConversionSymbol s);
//	static TCharacter Sign(bool s);
//	static TCharacter Digit(ufInt8 Digit);
// 
//};

//---------------------------------------------------------------------------
namespace StringStream{
//---------------------------------------------------------------------------
template<class TStreamWriteBuffer,class TCharacterConversionMap>
inline bool WriteDigits(TStreamWriteBuffer &WriteBuffer,TCharacterConversionMap &ConversionMap,ufInt8 *DigitBuffer,uIntn OutputCount)
{
	if(OutputCount==0){
		return true;
	}
	cArray<typename TStreamWriteBuffer::tElement> WriteMemory;
	while((WriteMemory=WriteBuffer.ReserveWriteBuffer(OutputCount)).Length!=0){
		if(WriteMemory.Length>=OutputCount){
			for(uIntn i=0;i<OutputCount;i++){
				WriteMemory.Pointer[i]=ConversionMap.Digit(DigitBuffer[i]);
			}
			WriteBuffer.CommitWriteBuffer(OutputCount);
			return true;
		}
		for(uIntn i=0;i<WriteMemory.Length;i++){
			WriteMemory.Pointer[i]=ConversionMap.Digit(DigitBuffer[i]);
		}
		DigitBuffer+=WriteMemory.Length;
		OutputCount-=WriteMemory.Length;
		// next block
		WriteBuffer.CommitWriteBuffer(WriteMemory.Length);
	}
	return false;
}
//---------------------------------------------------------------------------

// TStringItemConversion
//	uIntn CalculateLength(uIntn LimitLength)const;
//	template<class TStreamWriteBuffer,class TCharacterConversionMap>
//	uIntn Write(TStreamWriteBuffer &Buffer,TCharacterConversionMap &ConversionMap);

//---------------------------------------------------------------------------
template<class TStreamWriteBuffer,class TStringItemConversion,class TCharacterConversionMap>
bool WriteFormatItem(TStreamWriteBuffer &WriteBuffer,TStringItemConversion &ItemConversion,TCharacterConversionMap &ConversionMap,ufInt16 Width,bool AlignLeft,bool AlignCenter)
{
	typedef typename TStreamWriteBuffer::tElement tCharacter;


	ufInt16 LeadingSpace;
	ufInt16 TailingSpace;

	if(Width==0){
		LeadingSpace=0;
		TailingSpace=0;
	}
	else{
		ufInt16 ItemLength=static_cast<ufInt16>(ItemConversion.CalculateLength());

		if(ItemLength>=Width){
			LeadingSpace=0;
			TailingSpace=0;
		}
		else{
			ufInt16 Space=Width-ItemLength;
			if(AlignCenter){
				ufInt16 SpaceHalf=Space/2;
				LeadingSpace=SpaceHalf;
				TailingSpace=SpaceHalf;
				if(Space&1){
					if(AlignLeft){
						// center left
						TailingSpace++;
					}
					else{
						// center right
						LeadingSpace++;
					}
				}
			}
			else{
				if(AlignLeft){
					// left
					LeadingSpace=0;
					TailingSpace=Space;
				}
				else{
					// right
					LeadingSpace=Space;
					TailingSpace=0;
				}
			}
		}
	}

	tCharacter BlankCharacter=ConversionMap.Symbol(CharacterConversionSymbol::Blank);
	// leading space
	if(LeadingSpace!=0){
		if(ArrayStream::WriteFill(WriteBuffer,BlankCharacter,LeadingSpace)==false)
			return false;
	}

	// Item
	if(ItemConversion.Write(WriteBuffer,ConversionMap)==false)
		return false;

	// tailing space
	if(TailingSpace!=0){
		if(ArrayStream::WriteFill(WriteBuffer,BlankCharacter,TailingSpace)==false)
			return false;
	}

	return true;
}

//---------------------------------------------------------------------------
}	// namespace StringStream
//---------------------------------------------------------------------------

template<class TStringItemConversion,class TCharacterConversionMap>
struct cStringWriteFormatItem
{
	TStringItemConversion &ItemConversion;
	TCharacterConversionMap &ConversionMap;
	ufInt16 Width;
	bool AlignLeft;
	bool AlignCenter;
	
	template<class TStreamWriteBuffer>
	friend bool operator +=(TStreamWriteBuffer&& WriteBuffer,const cStringWriteFormatItem &Src){
		return StringStream::WriteFormatItem(WriteBuffer,Src.ItemConversion,Src.ConversionMap,Src.Width,Src.AlignLeft,Src.AlignCenter);
	}
};

template<class TStringItemConversion,class TCharacterConversionMap>
cStringWriteFormatItem<TStringItemConversion,TCharacterConversionMap> StringWriteFormatItem(TStringItemConversion &ItemConversion,TCharacterConversionMap &ConversionMap,ufInt16 Width,bool AlignLeft,bool AlignCenter){
	cStringWriteFormatItem<TStringItemConversion,TCharacterConversionMap> StringWrite={
		ItemConversion,
		ConversionMap,
		Width,
		AlignLeft,
		AlignCenter
	};
	return StringWrite;
}

template<uIntn MaxDigits>
class cFormatStringIntegerConversion
{
public:
	uIntn CalculateLength(void)const{
		uIntn TotalLength=fOutputDigitCount;
		TotalLength+=fLeadingZero;
		TotalLength+=fPrefixLength;
		TotalLength+=fWriteSign!=0;
		return TotalLength;
	}

	template<class TStreamWriteBuffer,class TCharacterConversionMap>
	bool Write(TStreamWriteBuffer &WriteBuffer,TCharacterConversionMap &ConversionMap){
	
		typedef typename TStreamWriteBuffer::tElement TCharacter;
		// sign
		switch(fWriteSign){
		case 0:
			break;
		case 1:	// <sp>
			if(ArrayStream::WriteElement(WriteBuffer,ConversionMap.Symbol(CharacterConversionSymbol::Blank))==false)
				return false;
			break;
		case 2:	// +
			if(ArrayStream::WriteElement(WriteBuffer,ConversionMap.Sign(false))==false)
				return false;
			break;
		case 3:	// -
			if(ArrayStream::WriteElement(WriteBuffer,ConversionMap.Sign(true))==false)
				return false;
			break;
		}
		// prefix
		for(ufInt8 pi=0;pi<fPrefixLength;pi++){
			if(ArrayStream::WriteElement(WriteBuffer,ConversionMap.Ascii(fPrefix[pi]))==false)
				return false;
		}
		// zero
		if(ArrayStream::WriteFill(WriteBuffer,ConversionMap.Digit(0),fLeadingZero)==false)
			return false;
		// digits
		if(StringStream::WriteDigits(WriteBuffer,ConversionMap,fDigitBuffer+fDigitBufferIndex,fOutputDigitCount)==false)
			return false;

		return true;
	}

	bool PositiveSign;
	bool ReserveSignSpace;
	bool RadixPrefix;
	bool RadixPrefixUpperCase;

	template<class TInteger>
	typename cnVar::TTypeConditional<void,
		cnVar::TIntegerConversion<TInteger>::IsMatch
	>::Type SetValue(TInteger Value,ufInt8 Radix)
	{
		bool Sign;
		if cnRTL_IFCONSTEXPR(cnVar::TIsSigned<TInteger>::Value){
			Sign=cnMath::Abs(Value);
		}
		else{
			Sign=false;
		}

		fDigitBufferIndex=static_cast<ufInt16>(cnString::ConvertIntegerToDigits(Value,Radix,fDigitBuffer,MaxDigits));
		fOutputDigitCount=static_cast<ufInt16>(MaxDigits-fDigitBufferIndex);
		fLeadingZero=0;
		if(RadixPrefix){
			switch(Radix){
			case 16:
				fPrefix[0]='0';
				if(RadixPrefixUpperCase){
					fPrefix[1]='X';
				}
				else{
					fPrefix[1]='x';
				}
				fPrefixLength=2;
				break;
			default:
				fPrefixLength=0;
				break;
			}
		}
		else{
			fPrefixLength=0;
		}

		if(Sign){
			// negative sign
			fWriteSign=3;
		}
		else if(PositiveSign){
			// positive sign
			fWriteSign=2;
		}
		else if(ReserveSignSpace){
			// blank at sign
			fWriteSign=1;
		}
		else{
			fWriteSign=0;
		}


	}

	template<class T>
	typename cnVar::TTypeConditional<void,
		!cnVar::TIntegerConversion<typename cnVar::RemoveCVRef<T>::Type>::IsMatch
	>::Type SetValue(T &&Value,ufInt8 Radix){
		if cnRTL_IFCONSTEXPR(cnVar::TIntegerConversion<T>::MatchSize==0){
			fOutputDigitCount=0;
			return;
		}
		typedef typename cnVar::TIntegerOfSize<cnVar::TIntegerConversion<T>::MatchSize?cnVar::TIntegerConversion<T>::MatchSize:1,cnVar::TIntegerConversion<T>::IsSigned>::Type TInteger;
		TInteger IntValue=cnVar::TIntegerConversion<T>::template Cast<TInteger>(Value);
		static_assert(cnVar::TIntegerConversion<TInteger>::IsMatch);
		return SetValue(IntValue,Radix);
	}

	void SetPrecision(ufInt16 Precision){
		if(fOutputDigitCount<Precision){
			// need fill zeros
			fLeadingZero=Precision-fOutputDigitCount;
		}
		else{
			fLeadingZero=0;
		}
	}

	template<class T>
	void SetValueAs(T &&Value,ufInt8 Radix,ufInt8 IntegerSize,bool Signed){
		
		if(IntegerSize==0){
			IntegerSize=cnVar::TIntegerConversion<T>::MatchSize;
		}

		switch(IntegerSize){
		default:
			fOutputDigitCount=0;
			return;
		case 1:
			if(Signed){
				auto IntValue=cnVar::TIntegerConversion<T>::template Cast<sInt8>(Value);
				return SetValue(IntValue,Radix);
			}
			else{
				auto IntValue=cnVar::TIntegerConversion<T>::template Cast<uInt8>(Value);
				return SetValue(IntValue,Radix);
			}
		case 2:
			if(Signed){
				auto IntValue=cnVar::TIntegerConversion<T>::template Cast<sInt16>(Value);
				return SetValue(IntValue,Radix);
			}
			else{
				auto IntValue=cnVar::TIntegerConversion<T>::template Cast<uInt16>(Value);
				return SetValue(IntValue,Radix);
			}
		case 4:
			if(Signed){
				auto IntValue=cnVar::TIntegerConversion<T>::template Cast<sInt32>(Value);
				return SetValue(IntValue,Radix);
			}
			else{
				auto IntValue=cnVar::TIntegerConversion<T>::template Cast<uInt32>(Value);
				return SetValue(IntValue,Radix);
			}
		case 8:
			if(Signed){
				auto IntValue=cnVar::TIntegerConversion<T>::template Cast<sInt64>(Value);
				return SetValue(IntValue,Radix);
			}
			else{
				auto IntValue=cnVar::TIntegerConversion<T>::template Cast<uInt64>(Value);
				return SetValue(IntValue,Radix);
			}
		//case 16:
		//	if(Signed){
		//		auto IntValue=cnVar::TIntegerConversion<T>::template Cast<sInt128>(Value);
		//		return SetValue(IntValue,Radix);
		//	}
		//	else{
		//		auto IntValue=cnVar::TIntegerConversion<T>::template Cast<uInt128>(Value);
		//		return SetValue(IntValue,Radix);
		//	}
		}
	}

protected:
	// Digits
	uIntn fDigitBufferIndex;
	uInt8 fDigitBuffer[MaxDigits];
	ufInt16 fOutputDigitCount;
	ufInt8 fWriteSign;
	ufInt8 fPrefixLength;
	char fPrefix[2];
	
	// Formats
	ufInt16 fLeadingZero;
};

cnLib_ENUM_BEGIN(ufInt8,FloatNumberConversionFormat)
{
	Fraction,
	Scientific,
	Compact
}cnLib_ENUM_END(FloatNumberConversionFormat);

template<uIntn MaxDigits>
class cFormatStringFloatConversion
{
public:
	uIntn CalculateLength(void)const{
		uIntn TotalLength=fIntegerDigits+fFractionDigits;
		TotalLength+=fIntegerLeadingZero+fDotZero+fFractionTailingZero;
		TotalLength+=fWriteDot;
		return TotalLength;
	}

	template<class TStreamWriteBuffer,class TCharacterConversionMap>
	bool Write(TStreamWriteBuffer &WriteBuffer,TCharacterConversionMap &ConversionMap){
	
		typedef typename TStreamWriteBuffer::tElement TCharacter;
		// sign
		switch(fWriteSign){
		case 0:
			break;
		case 1:
			if(ArrayStream::WriteElement(WriteBuffer,ConversionMap.Symbol(CharacterConversionSymbol::Blank))==false)
				return false;
			break;
		case 2:
			if(ArrayStream::WriteElement(WriteBuffer,ConversionMap.Sign(false))==false)
				return false;
			break;
		case 3:
			if(ArrayStream::WriteElement(WriteBuffer,ConversionMap.Sign(true))==false)
				return false;
			break;
		}
		// prefix
		for(ufInt8 pi=0;pi<fPrefixLength;pi++){
			if(ArrayStream::WriteElement(WriteBuffer,ConversionMap.Ascii(fPrefix[pi]))==false)
				return false;
		}
		// integer leading zero
		if(ArrayStream::WriteFill(WriteBuffer,ConversionMap.Digit(0),fIntegerLeadingZero)==false)
			return false;

		if(fIntegerDigits!=0){
			// integer digits
			if(StringStream::WriteDigits(WriteBuffer,ConversionMap,fDigitBuffer,fIntegerDigits)==false)
				return false;

			if(fFractionDigits!=0){	// N.N
				// dot
				if(fWriteDot){
					if(ArrayStream::WriteElement(WriteBuffer,ConversionMap.Symbol(CharacterConversionSymbol::Dot))==false)
						return false;
				}
				// fraction signification
				if(StringStream::WriteDigits(WriteBuffer,ConversionMap,fDigitBuffer+fIntegerDigits,fFractionDigits)==false)
					return false;
			}
			else{					// N0.
				// dot left zero
				if(ArrayStream::WriteFill(WriteBuffer,ConversionMap.Digit(0),fDotZero)==false)
					return false;
				// dot
				if(fWriteDot){
					if(ArrayStream::WriteElement(WriteBuffer,ConversionMap.Symbol(CharacterConversionSymbol::Dot))==false)
						return false;
				}
			}
		}
		else{
			if(fWriteDot){
				if(ArrayStream::WriteElement(WriteBuffer,ConversionMap.Symbol(CharacterConversionSymbol::Dot))==false)
					return false;
			}
			if(fFractionDigits!=0){
				// .0N
				// zero right dot
				if(ArrayStream::WriteFill(WriteBuffer,ConversionMap.Digit(0),fDotZero)==false)
					return false;
				// fraction signification
				if(StringStream::WriteDigits(WriteBuffer,ConversionMap,fDigitBuffer,fFractionDigits)==false)
					return false;
			}
		}
		// fraction tailing zero
		if(ArrayStream::WriteFill(WriteBuffer,ConversionMap.Digit(0),fFractionTailingZero)==false)
			return false;
		return true;
	}

	bool PositiveSign;
	bool ReserveSignSpace;
	bool RadixPrefix;
	bool PrefixUpperCase;
	bool OmitDot;

	template<class T>
	typename cnVar::TTypeConditional<void,
		cnVar::TFloatConversion<T>::IsMatch
	>::Type SetValue(T Value,ufInt8 Radix){
		bool Sign=cnMath::Abs(Value);
		fDigitCount=static_cast<ufInt16>(cnString::ConvertFloatSignificandToDigits(Value,Radix,fDigitBuffer,MaxDigits,fExponent));

		if(RadixPrefix){
			switch(Radix){
			case 16:
				fPrefix[0]='0';
				if(PrefixUpperCase){
					fPrefix[1]='X';
				}
				else{
					fPrefix[1]='x';
				}
				fPrefixLength=2;
				break;
			default:
				fPrefixLength=0;
				break;
			}
		}
		else{
			fPrefixLength=0;
		}

		if(Sign){
			// negative sign
			fWriteSign=3;
		}
		else if(PositiveSign){
			// positive sign
			fWriteSign=2;
		}
		else if(ReserveSignSpace){
			// blank at sign
			fWriteSign=1;
		}
		else{
			fWriteSign=0;
		}

		fWriteDot=true;
		if(fExponent<0){
			fIntegerDigits=0;
			fFractionDigits=static_cast<ufInt8>(fDigitCount);
			fIntegerLeadingZero=0;
			fDotZero=static_cast<ufInt16>(-fExponent);
			fFractionTailingZero=0;
		}
		else if(static_cast<ufInt16>(fExponent)<fDigitCount){
			fIntegerDigits=static_cast<ufInt8>(fExponent);
			fFractionDigits=static_cast<ufInt8>(fDigitCount-fIntegerDigits);
			fIntegerLeadingZero=0;
			fDotZero=0;
			fFractionTailingZero=0;
		}
		else{
			fIntegerDigits=static_cast<ufInt8>(fDigitCount);
			fFractionDigits=0;
			fIntegerLeadingZero=0;
			if(static_cast<ufInt16>(fExponent)>fDigitCount){
				fDotZero=static_cast<ufInt16>(fExponent)-fDigitCount;
			}
			else{
				fDotZero=0;
			}
			fFractionTailingZero=0;
		}
	}
	template<class T>
	typename cnVar::TTypeConditional<void,
		!cnVar::TFloatConversion<typename cnVar::TRemoveCVRef<T>::Type>::IsMatch
	>::Type SetValue(T &&Value,ufInt8 Radix){
		if cnRTL_IFCONSTEXPR(cnVar::TFloatConversion<T>::MatchSize==0){
			fDigitCount=0;
			return;
		}
		typedef typename cnVar::TFloatOfSize<cnVar::TFloatConversion<T>::MatchSize?cnVar::TFloatConversion<T>::MatchSize:1>::Type TFloat;
		TFloat FloatValue=cnVar::TFloatOfSize<T>::template Cast<TFloat>(Value);
		static_assert(cnVar::TFloatConversion<TFloat>::IsMatch);
		return SetValue(FloatValue,Radix);
	}

	
	template<class T>
	void SetValueAs(T &&Value,ufInt8 Radix,ufInt8 FloatSize){
		
		if(FloatSize==0){
			FloatSize=cnVar::TFloatConversion<T>::MatchSize;
			fExponent=0;
		}

		switch(FloatSize){
		default:
			fDigitCount=0;
			return;
		case 4:
			{
				auto FloatValue=cnVar::TFloatConversion<T>::template Cast<Float32>(Value);
				return SetValue(FloatValue,Radix);
			}
		case 8:
			{
				auto FloatValue=cnVar::TFloatConversion<T>::template Cast<Float64>(Value);
				return SetValue(FloatValue,Radix);
			}
		}
	}


	sfInt16 GetExponent(void)const{
		return fExponent;
	}

	void SetFormat(ufInt16 IntegerPrecision,ufInt16 FractionMinPrecision,ufInt16 FractionMaxPrecision){

		if(fExponent<0){
			// 0. ...
			fIntegerDigits=0;
			fFractionDigits=static_cast<ufInt8>(fDigitCount);
			fIntegerLeadingZero=IntegerPrecision;
			fWriteDot=true;			
			// .0nnn
			fDotZero=static_cast<ufInt16>(-fExponent);
			ufInt16 TotalFractionLength=fDotZero+fFractionDigits;
			if(TotalFractionLength==FractionMaxPrecision){
				fFractionTailingZero=0;
			}
			else if(TotalFractionLength==FractionMaxPrecision){
				if(fDotZero>FractionMaxPrecision){
					fDotZero=FractionMaxPrecision;
					fFractionDigits=0;
				}
				else{
					fFractionDigits=static_cast<ufInt8>(FractionMaxPrecision-fDotZero);
				}
				fFractionTailingZero=0;
			}
			else if(TotalFractionLength<FractionMinPrecision){
				// 0.0nn0
				fFractionTailingZero=FractionMinPrecision-TotalFractionLength;
			}
			else{
				// 0.0nn
				fFractionTailingZero=0;
			}
		}
		else if(static_cast<ufInt16>(fExponent)>=fDigitCount){
			fIntegerDigits=static_cast<ufInt8>(fDigitCount);
			fFractionDigits=0;
			// nnn000.0
			fDotZero=static_cast<ufInt16>(fExponent-fIntegerDigits);
			
			ufInt16 TotalIntegerLength=static_cast<ufInt16>(fExponent);
			if(TotalIntegerLength<IntegerPrecision){
				fIntegerLeadingZero=IntegerPrecision-TotalIntegerLength;
			}
			else{
				fIntegerLeadingZero=0;
			}
			
			fFractionTailingZero=FractionMinPrecision;
			if(fFractionTailingZero==0){
				fWriteDot=!OmitDot;
			}
			else{
				fWriteDot=true;
			}
		}
		else{
			fIntegerDigits=static_cast<ufInt8>(fExponent);
			fFractionDigits=static_cast<ufInt8>(fDigitCount-fIntegerDigits);
			// 0n.n...
			if(fIntegerDigits<IntegerPrecision){
				fIntegerLeadingZero=IntegerPrecision-fIntegerDigits;
			}
			else{
				fIntegerLeadingZero=0;
			}
			
			fDotZero=0;
			fWriteDot=true;

			if(fFractionDigits>FractionMaxPrecision){
				fFractionDigits=static_cast<ufInt8>(FractionMaxPrecision);
				fFractionTailingZero=0;
			}
			else if(fFractionDigits<FractionMinPrecision){
				fFractionTailingZero=FractionMinPrecision-fFractionDigits;
			}
			else{
				fFractionTailingZero=0;
			}
		}
	}
protected:
	
	// Digits
	uInt8 fDigitBuffer[MaxDigits];
	sfInt16 fExponent;	// can support up to 128-bit float point representation
	ufInt16 fDigitCount;
	ufInt8 fWriteSign;
	ufInt8 fPrefixLength;
	char fPrefix[2];

	// Formats
	ufInt8 fIntegerDigits;
	ufInt8 fFractionDigits;
	ufInt16 fIntegerLeadingZero;
	ufInt16 fDotZero;
	ufInt16 fFractionTailingZero;
	bool fWriteDot;

};


template<uIntn MaxDigits>
class cFormatStringFloatScientificConversion : public cFormatStringFloatConversion<MaxDigits>
{
public:
	uIntn CalculateLength(void)const{
		uIntn TotalLength=cFormatStringFloatConversion<MaxDigits>::CalculateLength();
		TotalLength+=fExpPrefixLength;
		TotalLength+=fExponentConversion.CalculateLength();
		return TotalLength;
	}

	template<class TStreamWriteBuffer,class TCharacterConversionMap>
	bool Write(TStreamWriteBuffer &WriteBuffer,TCharacterConversionMap &ConversionMap){
		if(cFormatStringFloatConversion<MaxDigits>::Write(WriteBuffer,ConversionMap)==false)
			return false;
		if(fOutputExp==false)
			return true;

		// exponent prefix
		for(ufInt8 pi=0;pi<fExpPrefixLength;pi++){
			if(ArrayStream::WriteElement(WriteBuffer,ConversionMap.Ascii(fExpPrefix[pi]))==false)
				return false;
		}
		return fExponentConversion.Write(WriteBuffer,ConversionMap);
	}



	template<class T>
	void SetValue(T &&Value,ufInt8 Radix){
		cFormatStringFloatConversion<MaxDigits>::SetValue(Value,Radix);
		UpdateExponent(Radix);
	}

	template<class T>
	void SetValueAs(T &&Value,ufInt8 Radix,ufInt8 FloatSize){
		cFormatStringFloatConversion<MaxDigits>::SetValueAs(Value,Radix,FloatSize);
		UpdateExponent(Radix);
	}

	void SetFormat(eFloatNumberConversionFormat Format,ufInt16 IntegerPrecision,ufInt16 FractionMinPrecision,ufInt16 FractionMaxPrecision){
		switch(Format){
		case FloatNumberConversionFormat::Fraction:
			return SetFormatF(IntegerPrecision,FractionMinPrecision,FractionMaxPrecision);
		case FloatNumberConversionFormat::Scientific:
			return SetFormatE(IntegerPrecision,FractionMinPrecision,FractionMaxPrecision);
		case FloatNumberConversionFormat::Compact:	default:
			return SetFormatG(IntegerPrecision,FractionMinPrecision,FractionMaxPrecision);
		}
	}

	void SetFormatF(ufInt16 IntegerPrecision,ufInt16 FractionMinPrecision,ufInt16 FractionMaxPrecision){
		fOutputExp=false;
		return cFormatStringFloatConversion<MaxDigits>::SetFormat(IntegerPrecision,FractionMinPrecision,FractionMaxPrecision);
	}

	void SetFormatE(ufInt16 IntegerPrecision,ufInt16 FractionMinPrecision,ufInt16 FractionMaxPrecision){
		this->fDotZero=0;
		if(this->fDigitCount==0){
			// 0
			this->fIntegerDigits=0;
			this->fFractionDigits=0;
			this->fIntegerLeadingZero=1;
			this->fFractionTailingZero=FractionMinPrecision;
			if(this->fFractionTailingZero==0){
				this->fWriteDot=!this->OmitDot;
			}
			else{
				this->fWriteDot=true;
			}
		}
		else{
			// n.nn ...
			this->fIntegerDigits=1;
			this->fFractionDigits=static_cast<ufInt8>(this->fDigitCount-1);
			this->fDotZero=0;

			if(this->fFractionDigits | FractionMinPrecision){
				this->fWriteDot=true;
			}
			else{
				this->fWriteDot=!this->OmitDot;
			}

			if(this->fFractionDigits>FractionMaxPrecision){
				this->fFractionDigits=static_cast<ufInt8>(FractionMaxPrecision);
				this->fFractionTailingZero=0;
			}
			else if(this->fFractionDigits<FractionMinPrecision){
				this->fFractionTailingZero=FractionMinPrecision-this->fFractionDigits;
			}
			else{
				this->fFractionTailingZero=0;
			}
		}
		fOutputExp=true;
		fExponentConversion.SetPrecision(IntegerPrecision);
	}
	void SetFormatG(ufInt16 IntegerPrecision,ufInt16 FractionMinPrecision,ufInt16 FractionMaxPrecision){

		if(this->fExponent>-4 && static_cast<ufInt16>(this->fExponent)<FractionMaxPrecision){
			SetFormatF(IntegerPrecision,FractionMinPrecision,FractionMaxPrecision-1-this->fExponent);
		}
		else{
			SetFormatE(1,FractionMinPrecision-1,FractionMaxPrecision-1);
		}
	}
protected:
	cFormatStringIntegerConversion<16> fExponentConversion;
	bool fOutputExp;
	// Digits
	ufInt8 fExpPrefixLength;
	char fExpPrefix[1];

	void UpdateExponent(ufInt8 Radix){
		fExponentConversion.PositiveSign=true;
		fExponentConversion.RadixPrefix=false;
		fExponentConversion.SetValue(this->fExponent,Radix);

		switch(Radix){
		case 16:
			if(this->PrefixUpperCase){
				fExpPrefix[0]='P';
			}
			else{
				fExpPrefix[0]='p';
			}
			fExpPrefixLength=1;
			break;
		default:
			if(this->PrefixUpperCase){
				fExpPrefix[0]='E';
			}
			else{
				fExpPrefix[0]='e';
			}
			fExpPrefixLength=1;
			break;
		}
	}
};


template<class TCharacter>
struct cFormatStringCharacterConversionMap
{
	static TCharacter Ascii(char c){
		return static_cast<TCharacter>(c);
	}

	static TCharacter Symbol(eCharacterConversionSymbol s){
		switch(s){
		default:
			return static_cast<TCharacter>('?');
		case CharacterConversionSymbol::Blank:
			return static_cast<TCharacter>(' ');
		case CharacterConversionSymbol::Dot:
			return static_cast<TCharacter>('.');
		}
	}

	static TCharacter Sign(bool s){
		return static_cast<TCharacter>(s?'-':'+');
	}

	static TCharacter Digit(ufInt8 Digit){
		if(Digit<10)
			return static_cast<TCharacter>('0'+Digit);
		return static_cast<TCharacter>('?');
	}
};


template<class TCharacter>
struct cFormatStringIntegerCharacterConversionMap : cFormatStringCharacterConversionMap<TCharacter>
{
	TCharacter DigitBase=static_cast<TCharacter>('0');
	TCharacter HexBase=static_cast<TCharacter>('A');


	TCharacter Digit(ufInt8 Digit){
		if(Digit<10)
			return static_cast<TCharacter>(DigitBase+Digit);
		Digit-=10;
		if(Digit<6)
			return static_cast<TCharacter>(HexBase+Digit);
		return static_cast<TCharacter>('?');
	}
};


template<class TCharacter>
struct cFormatStringFloatCharacterConversionMap : cFormatStringIntegerCharacterConversionMap<TCharacter>
{
};



//---------------------------------------------------------------------------
namespace StringStream{
//---------------------------------------------------------------------------

template<class TStreamWriteBuffer,class T>
bool WriteFormatInt(TStreamWriteBuffer&& WriteBuffer,T Value,ufInt8 Radix,ufInt8 Precision,ufInt16 Width)
{
	typedef typename cnVar::TRemoveReference<TStreamWriteBuffer>::Type::tElement TCharacter;

	cFormatStringIntegerCharacterConversionMap<TCharacter> ConversionMap;
	cFormatStringIntegerConversion<cnVar::TIntegerConversion<T>::MatchSize*ByteBitCount> IntConversion;
	IntConversion.PositiveSign=false;
	IntConversion.ReserveSignSpace=false;
	IntConversion.RadixPrefix=false;
	IntConversion.RadixPrefixUpperCase=false;

	IntConversion.SetValue(Value,Radix);
	IntConversion.SetPrecision(Precision);

	return WriteFormatItem(WriteBuffer,IntConversion,ConversionMap,Width,false,false);
}

template<class TStreamWriteBuffer,class T>
bool WriteFormatFloat(TStreamWriteBuffer&& WriteBuffer,T Value,ufInt8 Radix,ufInt8 Precision,ufInt16 Width)
{
	typedef typename cnVar::TRemoveReference<TStreamWriteBuffer>::Type::tElement TCharacter;

	cFormatStringFloatCharacterConversionMap<TCharacter> ConversionMap;
	cFormatStringFloatConversion<cnVar::TFloatConversion<T>::MatchSize*ByteBitCount> FloatConversion;
	FloatConversion.PositiveSign=false;
	FloatConversion.ReserveSignSpace=false;
	FloatConversion.RadixPrefix=false;
	FloatConversion.PrefixUpperCase=false;
	FloatConversion.OmitDot=false;

	FloatConversion.SetValue(Value,Radix);
	FloatConversion.SetFormat(1,Precision,Precision);

	return WriteFormatItem(WriteBuffer,FloatConversion,ConversionMap,Width,false,false);
}

template<class TStreamWriteBuffer,class T>
bool WriteFormatFloatNotation(TStreamWriteBuffer&& WriteBuffer,T Value,ufInt8 Radix,ufInt16 IntegerPrecision,ufInt16 FractionMinPrecision,ufInt16 FractionMaxPrecision,eFloatNumberConversionFormat FormatNotation,ufInt16 Width)
{
	typedef typename cnVar::TRemoveReference<TStreamWriteBuffer>::Type::tElement TCharacter;

	cFormatStringFloatCharacterConversionMap<TCharacter> ConversionMap;
	cFormatStringFloatScientificConversion<cnVar::TFloatConversion<T>::MatchSize*ByteBitCount> FloatConversion;
	FloatConversion.PositiveSign=false;
	FloatConversion.ReserveSignSpace=false;
	FloatConversion.RadixPrefix=false;
	FloatConversion.PrefixUpperCase=false;
	FloatConversion.OmitDot=false;

	FloatConversion.SetValue(Value,Radix);
	FloatConversion.SetFormat(FormatNotation,IntegerPrecision,FractionMinPrecision,FractionMaxPrecision);

	return WriteFormatItem(WriteBuffer,FloatConversion,ConversionMap,Width,false,false);
}

template<class TStreamWriteBuffer,class T>
bool WriteFormatFloatF(TStreamWriteBuffer&& WriteBuffer,T Value,ufInt8 Radix,ufInt8 Precision,ufInt16 Width)
{
	return WriteFormatFloatNotation(WriteBuffer,Value,Radix,1,Precision,Precision,FloatNumberConversionFormat::Fraction,Width);
}

template<class TStreamWriteBuffer,class T>
bool WriteFormatFloatE(TStreamWriteBuffer&& WriteBuffer,T Value,ufInt8 Radix,ufInt8 Precision,ufInt16 Width)
{
	return WriteFormatFloatNotation(WriteBuffer,Value,Radix,1,Precision,Precision,FloatNumberConversionFormat::Scientific,Width);
}

template<class TStreamWriteBuffer,class T>
bool WriteFormatFloatG(TStreamWriteBuffer&& WriteBuffer,T Value,ufInt8 Radix,ufInt8 Precision,ufInt16 Width)
{
	return WriteFormatFloatNotation(WriteBuffer,Value,Radix,1,Precision,Precision,FloatNumberConversionFormat::Compact,Width);
}
//---------------------------------------------------------------------------
}	// namespace StringStream
//---------------------------------------------------------------------------

cnLib_ENUM_BEGIN(ufInt8,FormatStringTokenType)
{
	Literal,		// format string
	Escape,			// %%
	Character,		// %c
	String,			// %s
	SignedDecimal,	// %d%i
	UnsignedDecimal,// %u
	UnsignedOctal,	// %o
	UnsignedHex,	// %x%X
	FloatDecimal,	// %f%F
	ExponentDecimal,// %e%E
	FloatHex,		// %a%A
	FloatCompact,	// %g%G
	OutputLength,	// %n
	Pointer,		// %p
}cnLib_ENUM_END(FormatStringTokenType);

cnLib_ENUM_BEGIN(ufInt8,FormatStringArgumentSize)
{
	Normal,
	Small,		// hh
	Half,		// h
	Long,		// l
	Long2,		// ll
	Maximum,	// j
	NatureWord,	// z
	Offset,		// t
	Double,		// L
}cnLib_ENUM_END(FormatStringArgumentSize);

// cFormatStringTokenParameter
// string format item settings
struct cFormatStringTokenParameter
{
	void SetDefault(void);

	eFormatStringTokenType TokenType;

	eFormatStringArgumentSize SizeModifier;

	// argument index for value
	ufInt16 ValueArgumentIndex;

	// use default value
	static cnRTL_CONSTVAR ufInt16 ArgIndexDefault	=	0xFFFF;
	// use literal value from format string
	static cnRTL_CONSTVAR ufInt16 ArgIndexLiteral	=	0xFFFE;

	// argument index for width
	ufInt16 WidthArgumentIndex;

	// argument precision for width
	ufInt16 PrecisionArgumentIndex;

	// Width
	//	minium width of format item
    uIntn Width;

	// Precision
	//	precision for numeral argument conversion
	uIntn Precision;

	bool AlignLeft;

	bool PositiveSign;
	bool ReserveSignSpace;
	bool ConversionAlternativeForm;
	bool ConversionUpperCase;

	bool PadLeftZero;
};

template<class TCharacter>
struct cFormatStringToken : cFormatStringTokenParameter
{
	cArray<const TCharacter> FormatString;
};

template<class TStreamWriteBuffer,class...TArgs>
struct cFormatStringStreamOutput
{
	typedef typename cnVar::TRemoveReference<TStreamWriteBuffer>::Type::tElement tCharacter;

	cnStream::cCountingStreamWriteBuffer<TStreamWriteBuffer> WriteBuffer;
	cnVar::cVarPack<const TArgs&...> Args;

	bool Output(const cFormatStringToken<tCharacter> &Token){
		switch(Token.TokenType){
		case FormatStringTokenType::Literal:	// format string
			if(Token.FormatString.Length!=0){
				return ArrayStream::WriteArray(WriteBuffer,Token.FormatString.Pointer,Token.FormatString.Length);
			}
			return true;
		case FormatStringTokenType::Escape:	// %%
			return ArrayStream::WriteElement(WriteBuffer,'%');
		case FormatStringTokenType::Character:	// %c
			return CharacterToStream(Token);
		case FormatStringTokenType::String:	// %s
			return StringToStream(Token);
		case FormatStringTokenType::SignedDecimal:	// %d%i
			return IntegerToStream(Token,true,10);
		case FormatStringTokenType::UnsignedDecimal:// %u
			return IntegerToStream(Token,false,10);
		case FormatStringTokenType::UnsignedOctal:	// %o
			return IntegerToStream(Token,false,8);
		case FormatStringTokenType::UnsignedHex:	// %x%X
			return IntegerToStream(Token,false,16);
		case FormatStringTokenType::FloatDecimal:	// %f%F
			return FloatToStream(Token,10,FloatNumberConversionFormat::Fraction);
		case FormatStringTokenType::ExponentDecimal:// %e%E
			return FloatToStream(Token,10,FloatNumberConversionFormat::Scientific);
		case FormatStringTokenType::FloatHex:		// %a%A
			return FloatToStream(Token,16,FloatNumberConversionFormat::Scientific);
		case FormatStringTokenType::FloatCompact:	// %g%G
			return FloatToStream(Token,10,FloatNumberConversionFormat::Compact);
		case FormatStringTokenType::OutputLength:	// %n
			IntegerFromWrittenLength(Token);
			return true;
		case FormatStringTokenType::Pointer:		// %p
			return IntegerToStream(Token,false,16);
		}
		// unknown token type
		return true;
	}

protected:
	
	struct cWriter
	{
		cWriter(cFormatStringStreamOutput *Owner,const cFormatStringTokenParameter &Token)
			: WriteBuffer(Owner->WriteBuffer)
			, Token(Token)
		{
			// Width
			switch(Token.WidthArgumentIndex){
			case Token.ArgIndexDefault:
				DefaultWidth=true;
				Width=0;
				break;
			case Token.ArgIndexLiteral:
				DefaultWidth=false;
				Width=static_cast<ufInt16>(Token.Width);
				break;
			default:
				DefaultWidth=false;
				// load width from argument
				{
					cIntegerLoader Loader;
					Loader.Dest=&Width;
					if(cnVar::VarPackCallAt(Token.WidthArgumentIndex,Loader,Owner->Args)==false){
						// no argument provided, use default
						DefaultWidth=true;
					}
				}
				break;
			}
			// Precision
			switch(Token.PrecisionArgumentIndex){
			case Token.ArgIndexDefault:
				DefaultPrecision=true;
				Precision=0;
				break;
			case Token.ArgIndexLiteral:
				DefaultPrecision=false;
				Precision=static_cast<ufInt16>(Token.Precision);
				break;
			default:
				DefaultPrecision=false;
				// load precision from argument
				{
					cIntegerLoader Loader;
					Loader.Dest=&Precision;
					if(cnVar::VarPackCallAt(Token.PrecisionArgumentIndex,Loader,Owner->Args)==false){
						// no argument provided, use default
						DefaultPrecision=true;
					}
				}
				break;
			}
		}
		cnStream::cCountingStreamWriteBuffer<TStreamWriteBuffer> &WriteBuffer;
		const cFormatStringTokenParameter &Token;

		mutable bool WriteResult;

		bool DefaultWidth;
		bool DefaultPrecision;
		ufInt16 Width;
		ufInt16 Precision;

		struct cIntegerLoader
		{
			ufInt16 *Dest;

			template<class TValue>
			void operator ()(TValue Value)const{
				*Dest=cnVar::TIntegerConversion<TValue>::template Cast<ufInt16>(Value);
			}


		};

		void LoadOption(const cFormatStringTokenParameter &Token){
			Token.AlignLeft;
			Token.ConversionAlternativeForm;
			Token.ConversionUpperCase;
			Token.PadLeftZero;
			Token.PositiveSign;
			Token.ReserveSignSpace;
			Token.SizeModifier;
		}
	};

	struct cWriterCharacter : cWriter
	{
		using cWriter::cWriter;

		template<class TValue>
		void operator ()(TValue Value)const{
			tCharacter c=0;
			if(cnVar::TryCastTo(c,Value)){
				this->WriteResult=ArrayStream::WriteElement(this->WriteBuffer,c);
			}
			else{
				this->WriteResult=true;
			}
		}
	};

	bool CharacterToStream(const cFormatStringToken<tCharacter> &Token){
		cWriterCharacter Writer(this,Token);
		cnVar::VarPackCallAt(Token.ValueArgumentIndex,Writer,Args);
		return Writer.WriteResult;
	}
	
	struct cWriterString : cWriter
	{
		using cWriter::cWriter;

		template<class TValue>
		void operator ()(TValue Value)const{
			const tCharacter *pStr=0;
			if(cnVar::TryCastTo(pStr,Value)==false){
				this->WriteResult=true;
				return;
			}
			if(pStr==nullptr){
				this->WriteResult=true;
				return;
			}
			// calculate Write length
			uIntn WriteLength=cnString::GetLength(pStr,this->DefaultPrecision?cnString::StringMaxLength:this->Precision);

			// output
			this->WriteResult=ArrayStream::WriteArray(this->WriteBuffer,pStr,WriteLength);
		}
	};

	bool StringToStream(const cFormatStringToken<tCharacter> &Token){
		cWriterString Writer(this,Token);
		cnVar::VarPackCallAt(Token.ValueArgumentIndex,Writer,Args);
		return Writer.WriteResult;
	}

	struct cWriterInteger : cWriter
	{
		ufInt8 Radix;
		bool Signed;
		ufInt8 IntegerSize;

		
		cWriterInteger(cFormatStringStreamOutput *Owner,const cFormatStringTokenParameter &Token)
			: cWriter(Owner,Token)
		{
			switch(Token.SizeModifier){
			default:
			case FormatStringArgumentSize::Normal:
				IntegerSize=0;
				break;
			case FormatStringArgumentSize::Small:
				IntegerSize=1;
				break;
			case FormatStringArgumentSize::Half:
				IntegerSize=2;
				break;
			case FormatStringArgumentSize::Long:
				IntegerSize=4;
				break;
			case FormatStringArgumentSize::Long2:
				IntegerSize=8;
				break;
			case FormatStringArgumentSize::Maximum:
				IntegerSize=8;
				break;
			case FormatStringArgumentSize::NatureWord:	// uIntn;
				IntegerSize=sizeof(uIntn);
				break;
			case FormatStringArgumentSize::Offset:		// sIntn;
				Signed=true;
				IntegerSize=sizeof(sIntn);
				break;
			case FormatStringArgumentSize::Double:		// sInt64
				IntegerSize=8;
				break;
			}
		}

		template<class TValue>
		void operator ()(TValue Value)const{

			cFormatStringIntegerCharacterConversionMap<tCharacter> ConversionMap;
			cFormatStringIntegerConversion<128> IntConversion;
			IntConversion.PositiveSign=this->Token.PositiveSign;
			IntConversion.ReserveSignSpace=this->Token.PositiveSign;
			IntConversion.RadixPrefix=false;
			IntConversion.RadixPrefixUpperCase=this->Token.ConversionUpperCase;

			IntConversion.SetValueAs(Value,Radix,IntegerSize,Signed);
			IntConversion.SetPrecision(this->Precision);

			this->WriteResult=StringStream::WriteFormatItem(this->WriteBuffer,IntConversion,ConversionMap,this->Width,this->Token.AlignLeft,false);
		}

	};
	
	bool IntegerToStream(const cFormatStringToken<tCharacter> &Token,bool Signed,ufInt8 Radix){
		cWriterInteger Writer(this,Token);
		Writer.Radix=Radix;
		Writer.Signed=Signed;
		if(Writer.DefaultPrecision){
			Writer.Precision=1;
		}
		Token.SizeModifier;
		cnVar::VarPackCallAt(Token.ValueArgumentIndex,Writer,Args);
		return Writer.WriteResult;
	}
	

	
	struct cWriterFloat : cWriter
	{

		ufInt8 Radix;
		eFloatNumberConversionFormat NotationFormat;
		ufInt8 FloatSize;

		cWriterFloat(cFormatStringStreamOutput *Owner,const cFormatStringTokenParameter &Token)
			: cWriter(Owner,Token)
		{
			switch(Token.SizeModifier){
			default:
			case FormatStringArgumentSize::Offset:
			case FormatStringArgumentSize::NatureWord:
			case FormatStringArgumentSize::Normal:
				FloatSize=0;
				break;
			case FormatStringArgumentSize::Small:
				FloatSize=4;
				break;
			case FormatStringArgumentSize::Half:
				FloatSize=4;
				break;
			case FormatStringArgumentSize::Long:
				FloatSize=8;
				break;
			case FormatStringArgumentSize::Long2:
				FloatSize=8;
				break;
			case FormatStringArgumentSize::Maximum:
				FloatSize=8;
				break;
			case FormatStringArgumentSize::Double:
				FloatSize=8;
				break;
			}
		}



		template<class TValue>
		void operator ()(TValue Value)const{

			cFormatStringFloatCharacterConversionMap<tCharacter> ConversionMap;
			cFormatStringFloatScientificConversion<64> FloatConversion;
			FloatConversion.PositiveSign=this->Token.PositiveSign;
			FloatConversion.ReserveSignSpace=this->Token.PositiveSign;
			FloatConversion.RadixPrefix=false;
			FloatConversion.PrefixUpperCase=this->Token.ConversionUpperCase;
			FloatConversion.OmitDot=false;

			FloatConversion.SetValueAs(Value,Radix,FloatSize);
			FloatConversion.SetFormat(NotationFormat,1,this->Precision,this->Precision);

			this->WriteResult=StringStream::WriteFormatItem(this->WriteBuffer,FloatConversion,ConversionMap,this->Width,false,false);
		}

	};
	
	bool FloatToStream(const cFormatStringToken<tCharacter> &Token,ufInt8 Radix,eFloatNumberConversionFormat NotationFormat){
		cWriterFloat Writer(this,Token);
		Writer.Radix=Radix;
		Writer.NotationFormat=NotationFormat;
		if(Writer.DefaultPrecision){
			Writer.Precision=6;
		}
		cnVar::VarPackCallAt(Token.ValueArgumentIndex,Writer,Args);
		return Writer.WriteResult;
	}
	
	
	struct cReaderInteger
	{
		uIntn TotalLengthWritten;

		template<class TValue>
		void operator ()(TValue)const{
		}

		void operator ()(uIntn *Ptr)const{
			*Ptr=TotalLengthWritten;
		}
		template<class T>
		void operator ()(const T *)const{
		}
	};
	
	void IntegerFromWrittenLength(const cFormatStringToken<tCharacter> &Token){
		cReaderInteger Reader;
		Reader.TotalLengthWritten=WriteBuffer.TotalWritten;
		cnVar::VarPackCallAt(Token.ValueArgumentIndex,Reader,Args);
	}
	
};

template<class TStreamWriteBuffer>
struct cFormatStringStreamOutput<TStreamWriteBuffer>
{
	typedef typename TStreamWriteBuffer::tElement tCharacter;

	cnStream::cCountingStreamWriteBuffer<TStreamWriteBuffer> WriteBuffer;
	cnVar::cVarPack<> Args;

	bool Output(const cFormatStringToken<tCharacter> &Token){
		if(Token.FormatString.Length!=0){
			return ArrayStream::WriteArray(WriteBuffer,Token.FormatString.Pointer,Token.FormatString.Length);
		}
		return true;
	}
};



struct cFormatStringParseStateAction
{
	bool Output;
	bool FormatInclude;
};

// FormatParameter
// string format item settings
class cFormatStringParseStateTransistion
{
public:
	void Reset(void);
	cFormatStringParseStateAction Process(uIntn Char,cFormatStringTokenParameter &TokenParameter);
	void ProcessEnd(cFormatStringTokenParameter &TokenParameter);
protected:
	static const cFormatStringParseStateAction ActionNone;
	static const cFormatStringParseStateAction ActionOutput;
	static const cFormatStringParseStateAction ActionOutputInclude;
	uIntn fArgumentIndex;
	enum eCharType{
		ctEnd,		// invalid format char
		ctFmt,		// %
		ctFlag,		// +-#
		ctNum,		// 123456789
		ctVarNum,	// *
		ctPrec,		// .
		ctSize,		// Lhjltz
		ctType,		// EFGXcdefginopsux
		ctZero,		// 0
		ctMax
	};
	enum eState{
		sStr,
		sFmt,
		sFlag,
		sWidth,
		sPrec,
		sSize,
		sMax
	}fState;
	enum eAction{
		aNone,
		aFmtStart,
		aFmtError,
		aFmtEnd,
		aFmtChar,
		aFlag,
		aSetWidth,
		aWidth,
		aVarWidth,
		aSetPrec,
		aPrec,
		aVarPrec,
		aSize,
	};
	// FormatStringAnalyseMap
	struct cStateTransitionItem{
		// State
		//	next state
		eState Next;
		// Action
		//	action for this transition
		eAction Action;
	};
	static const cStateTransitionItem FormatStringAnalyseTransistion[sMax][ctMax];

	static eCharType CharType(uIntn c);
	static cnRTL_CONSTVAR uIntn CharMapLen='z'+1;
	static const eCharType FormatOutput_CharMap[CharMapLen];
};



template<class TCharacter>
class cFormatStringParser
{
public:
	void Reset(const TCharacter *Format,uIntn MaxLength){
		fFormatString=Format;
		fFormatStringLength=MaxLength;

		fParseCheckpoint=Format;

		fTransistion.Reset();
	}

	bool Run(void){
		while(fFormatStringLength>0){
			TCharacter CurChar=*fFormatString;
			if(CurChar==0){
				fFormatStringLength=0;
				fTransistion.ProcessEnd(fParseToken);
				static_cast<cFormatStringTokenParameter&>(fOutput)=fParseToken;
				fOutput.FormatString.Pointer=fParseCheckpoint;
				fOutput.FormatString.Length=fFormatString-fParseCheckpoint;
				return true;
			}

			auto CurString=fFormatString;
			auto Action=fTransistion.Process(CurChar,fParseToken);

			fFormatString++;
			fFormatStringLength--;

			if(Action.Output){
				static_cast<cFormatStringTokenParameter&>(fOutput)=fParseToken;

				if(Action.FormatInclude){
					CurString=fFormatString;
				}
				fOutput.FormatString.Pointer=fParseCheckpoint;
				fOutput.FormatString.Length=CurString-fParseCheckpoint;

				fParseCheckpoint=CurString;
				fParseToken.SetDefault();
				return true;
			}
		}
		return false;
	}

	cFormatStringToken<TCharacter>& Value(void){	return fOutput;	}

	bool operator ()(){	return Run();	}

	cFormatStringToken<TCharacter>& operator *(void){	return fOutput;	}

protected:
	const TCharacter *fFormatString;
	uIntn fFormatStringLength;
	cFormatStringParseStateTransistion fTransistion;
	cFormatStringTokenParameter fParseToken;
	const TCharacter *fParseCheckpoint;
	cFormatStringToken<TCharacter> fOutput;

};

//---------------------------------------------------------------------------
namespace StringStream{
//---------------------------------------------------------------------------

// WriteFormatString
//	format string and write to Stream
// WriteBuffer			destination to write
// FormatString		[in]	format string
// Args...			[in]	format arguments
// return true if write completed, or false the stream cannot write more data
template<class TStreamWriteBuffer,class TParser,class...TArgs>
bool WriteFormatString(TStreamWriteBuffer&& WriteBuffer,TParser&& Parser,const cnVar::cVarPack<TArgs...> &Args)
{
	cFormatStringStreamOutput<TStreamWriteBuffer,TArgs...> Output={
		static_cast<TStreamWriteBuffer>(WriteBuffer),
		Args
	};
	while(Parser()){
		auto &Token=*Parser;
		if(Output.Output(Token)==false){	
			return false;
		}
	}
	return true;
}

// WriteFormatString
//	format string and write to Stream
// WriteBuffer			destination to write
// FormatString		[in]	format string
// Args...			[in]	format arguments
// return true if write completed, or false the stream cannot write more data
template<class TCharacter,class TStreamWriteBuffer,class...TArgs>
bool WriteFormatString(TStreamWriteBuffer&& WriteBuffer,const TCharacter *FormatString,const TArgs&...Args)
{
	cFormatStringParser<TCharacter> FormatParser;
	FormatParser.Reset(FormatString,cnString::StringMaxLength);
	return WriteFormatString(cnVar::Forward<TStreamWriteBuffer>(WriteBuffer),FormatParser,cnVar::VarRefPack(Args...));
}


//---------------------------------------------------------------------------
}	// namespace StringStream
//---------------------------------------------------------------------------

template<class TCharacter,class...TArgs>
struct cStringWriteFormat
{
	cnVar::cVarPack<const TArgs&...> Args;
	cFormatStringParser<TCharacter> FormatParser;

	
	template<class TStreamWriteBuffer>
	friend bool operator +=(TStreamWriteBuffer&& WriteBuffer,cStringWriteFormat Src){
		return StringStream::WriteFormatString(WriteBuffer,Src.FormatParser,Src.Args);
	}
};

template<class TCharacter,class...TArgs>
cStringWriteFormat<TCharacter,TArgs...> StringWriteFormat(const TCharacter *FormatString,const TArgs&...Args){
	cStringWriteFormat<TCharacter,TArgs...> StringWrite={
		cnVar::VarRefPack(Args...)
	};
	StringWrite.FormatParser.Reset(FormatString,cnString::StringMaxLength);
	return StringWrite;
}

// SPrint
//	output format string to Dest
// [in]Dest			destination to write
// [in]DestLength	length of destination
// [in]FormatString	format string
// [in]Args...		variable argument list
// return total output length
template<class TCharacter,class...TArgs>
uIntn SPrint(TCharacter *Dest,uIntn DestLength,const TCharacter *FormatString,const TArgs&...Args)
{
	auto WriteBuffer=cnStream::ArrayStreamWriteBuffer(Dest,DestLength-1);
	cnStream::cCountingStreamWriteBuffer<decltype(WriteBuffer)> CountedWriteBuffer(WriteBuffer);

	CountedWriteBuffer+=StringWriteFormat(FormatString,Args...);

	Dest[CountedWriteBuffer.TotalWritten]=0;
	return CountedWriteBuffer.TotalWritten;
}
// SPrint
//	output format string to Dest
// [in]Dest			destination to write
// [in]DestLength	length of destination
// [in]FormatString	format string
// [in]Args...		variable argument list
// return total output length
template<class TCharacter,uIntn DestLength, class...TArgs>
uIntn SPrint(TCharacter (&Dest)[DestLength],const TCharacter *FormatString, const TArgs&...Args)
{
	return SPrint(Dest,DestLength,FormatString,Args...);
}

//---------------------------------------------------------------------------
namespace StringStream{
//---------------------------------------------------------------------------

template<class TStreamWriteBuffer>
inline bool WriteGUID(TStreamWriteBuffer&& WriteBuffer,const void *Value)
{
	// {00000000-1111-2222-3333-333333333333}
	//  33221100 1100 1100 0011 223344556677
	struct GUIDStringData{
		uInt32 v0;
		uInt16 v1;
		uInt16 v2;
		uInt8 v3[8];
	};
	auto pIDData=static_cast<const GUIDStringData*>(Value);
	if(WriteFormatInt(WriteBuffer,cnMemory::SwapLittleEndian(pIDData->v0),16,8,0)==false)
		return false;
	if(ArrayStream::WriteElement(WriteBuffer,'-')==false)
		return false;
	if(WriteFormatInt(WriteBuffer,cnMemory::SwapLittleEndian(pIDData->v1),16,4,0)==false)
		return false;
	if(ArrayStream::WriteElement(WriteBuffer,'-')==false)
		return false;
	if(WriteFormatInt(WriteBuffer,cnMemory::SwapLittleEndian(pIDData->v2),16,4,0)==false)
		return false;
	if(ArrayStream::WriteElement(WriteBuffer,'-')==false)
		return false;
	if(WriteHexBytes(WriteBuffer,pIDData->v3,2)==false)
		return false;
	if(ArrayStream::WriteElement(WriteBuffer,'-')==false)
		return false;
	if(WriteHexBytes(WriteBuffer,pIDData->v3+2,6)==false)
		return false;
	return true;
}

//---------------------------------------------------------------------------
}	// namespace StringStream
//---------------------------------------------------------------------------

struct cStringWriteGUID
{
	const void *Value;
	
	template<class TStreamWriteBuffer>
	friend bool operator +=(TStreamWriteBuffer&& WriteBuffer,const cStringWriteGUID &Src){
		return StringStream::WriteGUID(WriteBuffer,Src.Value);
	}
};
inline cStringWriteGUID StringWriteGUID(const void *Value){
	cStringWriteGUID StringWrite={
		Value,
	};
	return StringWrite;
}

//---------------------------------------------------------------------------

template<class TCharacter,class TSrcCharacter>
inline cStringBuffer<TCharacter> CreateStringConvertEncoding(iTextEncodingConverter *Converter,const TSrcCharacter *Src,uIntn SrcLength){
	cStringBuffer<TCharacter> Buffer;
	StringStream::WriteConvertEncoding(Buffer.StreamWriteBuffer(),Converter,Src,SrcLength);
	return Buffer;
}


//---------------------------------------------------------------------------
	
template<class TCharacter,class...TArgs>
inline cStringBuffer<TCharacter> CreateStringFormat(const TCharacter *FormatString,const TArgs&...Args)
{
	cStringBuffer<TCharacter> Buffer;
	StringStream::WriteFormatString(Buffer.StreamWriteBuffer(),FormatString,Args...);
	return Buffer;
}


template<class TCharacter,class TInteger>
inline cStringBuffer<TCharacter> CreateStringInteger(TInteger&& SrcInteger,ufInt8 Radix=10,ufInt8 Precision=1,ufInt16 Width=0){
	cStringBuffer<TCharacter> Buffer;
	StringStream::WriteFormatInt(Buffer.StreamWriteBuffer(),static_cast<TInteger&&>(SrcInteger),Radix,Precision,Width);
	return Buffer;
}
template<class TCharacter>
inline cStringBuffer<TCharacter> CreateStringFloat(const double &SrcData,uInt8 Radix=10,ufInt8 Precision=6,ufInt16 Width=0){
	cStringBuffer<TCharacter> Buffer;
	StringStream::WriteFormatFloat(Buffer.StreamWriteBuffer(),SrcData,Radix,Precision,Width);
	return Buffer;
}
template<class TCharacter>
inline cStringBuffer<TCharacter> CreateStringFloatE(const double &SrcData,uInt8 Radix=10,ufInt8 Precision=6,ufInt16 Width=0){
	cStringBuffer<TCharacter> Buffer;
	StringStream::WriteFormatFloatE(Buffer.StreamWriteBuffer(),SrcData,Radix,Precision,Width);
	return Buffer;
}
template<class TCharacter>
inline cStringBuffer<TCharacter> CreateStringFloatG(const double &SrcData,uInt8 Radix=10,ufInt8 Precision=6,ufInt16 Width=0){
	cStringBuffer<TCharacter> Buffer;
	StringStream::WriteFormatFloatG(Buffer.StreamWriteBuffer(),SrcData,Radix,Precision,Width);
	return Buffer;
}

//---------------------------------------------------------------------------


#endif // cnLibrary_CPPFEATURE_USER_DEFINED_LITERALS >= 200809L


//---------------------------------------------------------------------------
template<class...VT>
cnLib_DEPRECATED inline iPtr<iFileName>	FileNameT(iFileName *FileName,const uChar16 *Name,VT...Names){
	const uChar16* Path[sizeof...(VT)+1]={Name,Names...};
	return FileName->MakeName(Path,sizeof...(VT)+1);
}
//---------------------------------------------------------------------------
template<class...VT>
inline iPtr<iFileName> MakeFileNameT(iFileName *FileName,const uChar16 *Name,VT...Names){
	const uChar16* Path[sizeof...(VT)+1]={Name,Names...};
	return FileName->MakeName(Path,sizeof...(VT)+1);
}
//---------------------------------------------------------------------------
template<class TPathCharacter,class TCharacter,TCharacter SplitterCharacter,class TConverterToPath,class TConverterFromPath>
class cStringFilePathTranslated
{
public:
	cStringFilePathTranslated(){}
	~cStringFilePathTranslated(){}

	cStringFilePathTranslated(cString<TPathCharacter> Path)
		: fPath(cnVar::MoveCast(Path))
	{
	}
	cStringFilePathTranslated(const cStringFilePathTranslated &Src)
		: fPath(Src.fPath)
		, fPathItems(Src.fPathItems)
	{
	}
	cStringFilePathTranslated(cStringFilePathTranslated &&Src)
		: fPath(cnVar::MoveCast(Src.fPath))
		, fPathItems(cnVar::MoveCast(Src.fPathItems))
	{
	}


	uIntn GetDepth(void){
		SetupPathItems();
		return fPathItems.GetCount();
	}
	operator const TPathCharacter*()const{
		return fPath;
	}
	const cString<TPathCharacter>& GetPath(void)const{
		return fPath;
	}
	uIntn GetPathLength(void)const{
		return fPath->Length;
	}

	const cString<TCharacter>& GetName(){
		SetupPathItems();
		return fPathItems[fPathItems.GetCount()-1].Name;
	}
	const cString<TCharacter>& GetName(uIntn Index){
		SetupPathItems();
		return fPathItems[Index].Name;
	}

	cStringFilePathTranslated MakeParentName(void){
		SetupPathItems();

		uIntn Count=fPathItems.GetCount();
		if(Count<=1)	// empty or no path
			return cStringFilePathTranslated();


		auto &PathItem=fPathItems[Count-2];

		cArray<const cPathItem> ParentPathItems={
			fPathItems.GetItems(),
			Count-1
		};
		return cStringFilePathTranslated(cString<TPathCharacter>(fPath->Pointer,PathItem.PathEndOffset),ParentPathItems);
	}
	cStringFilePathTranslated MakeFileName(const TCharacter*const *Path,uIntn Depth){
		SetupPathItems();

		cStringBuffer<TPathCharacter> NewPath(fPath.GetArray());
		cSeqList<cPathItem> PathItems=fPathItems;
		TConverterToPath Converter;
		for(uIntn i=0;i<Depth;i++){
			NewPath.AppendChar(SplitterCharacter);
			uIntn PathOffset=NewPath->Length;

			StringStream::WriteConvertEncoding(NewPath.StreamWriteBuffer(),Converter,Path[i],cnString::GetLength(Path[i]));

			uIntn NewLength=NewPath->Length;

			if(NewLength<=PathOffset){
				// empty string, error file name
				return cStringFilePathTranslated();
			}
			// check illegal characters
			auto PathString=NewPath->Pointer;
			for(uIntn i=PathOffset;i<NewLength;i++){
				switch(PathString[i]){
				default:
					break;
				case SplitterCharacter:
					// error file name
					return cStringFilePathTranslated();
				}
			}
			auto PathItem=PathItems.Append();
			PathItem->Name=Path[i];
			PathItem->PathOffset=PathOffset;
			PathItem->PathEndOffset=NewLength;
		}
		return cStringFilePathTranslated(cnVar::MoveCast(NewPath.Swap()),cnVar::MoveCast(PathItems));
	}

protected:

	cString<TPathCharacter> fPath;
	struct cPathItem
	{
		uIntn PathOffset;
		uIntn PathEndOffset;
		cString<TCharacter> Name;
	};
	cSeqList<cPathItem> fPathItems;

	cStringFilePathTranslated(cString<TPathCharacter> Path,cSeqList<cPathItem> PathItems)
		: fPath(cnVar::MoveCast(Path))
		, fPathItems(cnVar::MoveCast(PathItems))
	{
	}

	void SetupPathItems(void){
		if(fPathItems.GetCount()!=0){
			return;
		}
		if(fPath->Length==0)
			return;

		ReloadPathItems();
	}

	void ReloadPathItems(void){
		fPathItems.Clear();
		auto PathString=fPath->Pointer;
		auto PathLength=fPath->Length;
		uIntn PathOffset=0;
		for(uIntn i=0;i<PathLength;i++){
			switch(PathString[i]){
			default:
				break;
			case SplitterCharacter:
				{
					TConverterFromPath Converter;

					auto Item=fPathItems.Append();
					Item->Name=CreateStringConvertEncoding<TCharacter>(Converter,PathString+PathOffset,i-PathOffset);
					Item->PathOffset=PathOffset;
					Item->PathEndOffset=i;
				}
				PathOffset=i+1;
				break;
			}
		}
	}

};
//---------------------------------------------------------------------------
ufInt8 UnicodeChar8To32(uChar32 &Dest,const uChar8 *Src,uIntn SrcLength);
ufInt8 UnicodeChar16To32(uChar32 &Dest,const uChar16 *Src,uIntn SrcLength);
ufInt8 UnicodeChar8From32(uChar8 *Dest,uIntn DestLength,uChar32 Src);
ufInt8 UnicodeChar16From32(uChar16 *Dest,uIntn DestLength,uChar32 Src);
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
}	// namespace cnRTL
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#undef	cnRTL_CONSTEXPR_FUNC
#undef	cnRTL_CONSTVAR
#undef	cnRTL_IFCONSTEXPR
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif
