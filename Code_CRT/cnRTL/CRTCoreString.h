/*- cnLibCRT - CRT String -------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2019-01-13                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnRTL_CRTCRTString_H__
#define __cnLibrary_cnRTL_CRTCRTString_H__

#include <cnRTL/CRTCoreCommon.h>
#include <cnRTL/CRTCoreMemory.h>

#ifdef __cplusplus

//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------


template<uIntn ElementSize>
struct TKRuntime::tString
{
	typedef typename cnVar::TIntegerOfSize<ElementSize,false>::Type tUInt;

	static uIntn QueryLength(const void *String)noexcept{
		return std::char_traits<tUInt>::length(static_cast<const tUInt*>(String));
	}
	static uIntn QueryLength(const void *String,uIntn MaxLength)noexcept{
		const tUInt *Pos=std::char_traits<tUInt>::find(static_cast<const tUInt*>(String),MaxLength,0);
		if(Pos==nullptr)
			return MaxLength;
		return Pos-static_cast<const tUInt*>(String);
	}

	static uIntn Copy(void *Dest,uIntn DestLength,const void *Src)noexcept(true){
#if defined(_ITERATOR_DEBUG_LEVEL) && _ITERATOR_DEBUG_LEVEL>0
		// prevent VC complaint about unchecked iterator
		auto DestIterator=stdext::checked_array_iterator<tUInt*>(static_cast<tUInt*>(Dest),DestLength);
#else
		auto DestIterator=static_cast<tUInt*>(Dest);
#endif // defined(_ITERATOR_DEBUG_LEVEL)
		auto r=std::copy_if(static_cast<const tUInt*>(Src),static_cast<const tUInt*>(Src)+DestLength,DestIterator,[](const tUInt &c){	return c!=0;	});
		uIntn CopyedLength=std::distance(DestIterator,r);
		static_cast<tUInt*>(Dest)[CopyedLength]=0;
		return CopyedLength;
	}

	static bool Equal(const void *Str1,uIntn Str1Length,const void *Str2)noexcept(true){
		return std::char_traits<tUInt>::compare(static_cast<const tUInt*>(Str1),static_cast<const tUInt*>(Str2),Str1Length+1)==0;
	}
	static sfInt8 Compare(const void *Str1,uIntn Str1Length,const void *Str2)noexcept(true){
		return static_cast<sfInt8>(std::char_traits<tUInt>::compare(static_cast<const tUInt*>(Str1),static_cast<const tUInt*>(Str2),Str1Length+1));
	}
	
	static sfInt8 Compare(const void *Str1,uIntn Src1Length,const void *Str2,uIntn Src2Length)noexcept(true){
		if(Src1Length==Src2Length){
			return tArray<ElementSize>::Compare(Str1,Str2,Src1Length);
		}

		uIntn CompareLength;
		sfInt8 TailRelation;
		if(Src1Length<Src2Length){
			CompareLength=Src1Length;
			TailRelation=-1;
		}
		else{
			CompareLength=Src2Length;
			TailRelation=1;
		}

		sfInt8 Relation=tArray<ElementSize>::Compare(Str1,Str2,CompareLength);
		if(Relation!=0){
			return Relation;
		}
		return TailRelation;
	}
};

//---------------------------------------------------------------------------
namespace cnString{
//---------------------------------------------------------------------------

// DiffIndex
//	Find index of different character between 2 strings
//	[in]Str1
//	[in]Str2
//	return:	the index of first differnet char, or IndexNotFound if two strings matched
template<class TCharacter>
inline uIntn DiffIndex(const TCharacter *Str1,const typename cnVar::TTypeDef<TCharacter>::Type *Str2)
{
	return ViewDiffIndx(Str1,Str2);
}
// DiffIndexRange
//	Find index of different character between 2 strings
//	[in]Src1
//	[in]Src2
//	[in]Length	length of first characters in string to compare
//	return:	the index of first differnet char, or IndexNotFound if first length of two strings matched
template<class TCharacter>
inline uIntn DiffIndexRange(const TCharacter *Str1,const typename cnVar::TTypeDef<TCharacter>::Type *Str2,uIntn Length)
{
	return ViewDiffIndexRange(Str1,Str2,Length);
}

// Compare
//	The encoding relation of string1 to string2
//	[in]Str1	string to subtract
//	[in]Str2	string to be subtracted
//	return:	0 if matched, 1 if Str1>Str2 , -1 if Str1<Str2
template<class TCharacter>
sfInt8 Compare(const TCharacter *Str1,const typename cnVar::TTypeDef<TCharacter>::Type *Str2)
{
	return ViewCompare(Str1,Str2);
}


// Search
//	Search a character from string
//	[in]Src	string
//	[in]Dest	character to search
//	return: the index of first matched char, or uintMax if not found
template<class TCharacter>
uIntn Search(const TCharacter *Src,typename cnVar::TTypeDef<TCharacter>::Type Dest)
{
	cnLib_ASSERT(Src!=nullptr);
	auto SrcLength=std::char_traits<TCharacter>::length(Src);

	auto r=std::char_traits<TCharacter>::find(Src,SrcLength,Dest);
	if(r==nullptr)
		return IndexNotFound;
	return r-Src;
}
// Search
//	Search a character from string
//	[in]Src	string
//	[in]SrcLength	length of Src
//	[in]Dest	character to search
//	return: the index of first matched char, or IndexNotFound if not found
template<class TCharacter>
uIntn Search(const TCharacter *Src,uIntn SrcLength,typename cnVar::TTypeDef<TCharacter>::Type Dest)
{
	cnLib_ASSERT(Src!=nullptr);
	auto r=std::char_traits<TCharacter>::find(Src,SrcLength,Dest);
	if(r==nullptr)
		return IndexNotFound;
	return r-Src;
}
//---------------------------------------------------------------------------
}	// namespace cnString
//---------------------------------------------------------------------------
namespace cnString{
//---------------------------------------------------------------------------
#if 0

//---------------------------------------------------------------------------
template<class TCharacter,size_t CharacterSize>
struct cConvertFunctionBySize
{
	template<class...TArgs>
	static int SPrintF(TCharacter *Buffer,uIntn BufferLength,const TCharacter *Format,const TArgs&...Args){
		return snprintf(reinterpret_cast<char*>(Buffer),BufferLength,reinterpret_cast<const char*>(Format),Args...);
	}
};
//---------------------------------------------------------------------------
template<class TCharacter>
struct cConvertFunctionBySize<TCharacter,sizeof(char)>
{
	template<class...TArgs>
	static int SPrintF(TCharacter *Buffer,uIntn BufferLength,const TCharacter *Format,const TArgs&...Args){
		return snprintf(reinterpret_cast<char*>(Buffer),BufferLength,reinterpret_cast<const char*>(Format),Args...);
	}
};
//---------------------------------------------------------------------------
template<class TCharacter>
struct cConvertFunctionBySize<TCharacter,sizeof(wchar_t)>
{
	template<class...TArgs>
	static int SPrintF(TCharacter *Buffer,uIntn BufferLength,const TCharacter *Format,const TArgs&...Args){
		return swprintf(reinterpret_cast<wchar_t*>(Buffer),BufferLength,reinterpret_cast<const wchar_t*>(Format),Args...);
	}
};
//---------------------------------------------------------------------------
template<class TCharacter>
struct cConvertFunction : cConvertFunctionBySize<TCharacter,sizeof(TCharacter)>
{
};
template<class TStreamWriteBuffer>
inline uIntn tempWriteArray(TStreamWriteBuffer &WriteBuffer,typename TStreamWriteBuffer::tElement const *Array,uIntn Length)
{
	if(Length==0)
		return 0;
	if(Length==1){
		return WriteFill(WriteBuffer,Array[0],Length);
	}
	uIntn TotalLengthWritten=0;
	cArray<typename TStreamWriteBuffer::tElement> CurBuffer;
	while((CurBuffer=WriteBuffer.ReserveWriteBuffer(Length)).Length!=0){
		if(CurBuffer.Length>=Length){
			for(uIntn i=0;i<Length;i++){
				CurBuffer.Pointer[i]=Array[i];
			}
			WriteBuffer.CommitWriteBuffer(Length);
			return TotalLengthWritten+Length;
		}
		// copy
		for(uIntn i=0;i<CurBuffer.Length;i++){
			CurBuffer.Pointer[i]=Array[i];
		}
		// offset dest pointer
		Length-=CurBuffer.Length;
		Array=Array+CurBuffer.Length;
		TotalLengthWritten+=CurBuffer.Length;
		// next block
		WriteBuffer.CommitWriteBuffer(CurBuffer.Length);
	}
	return TotalLengthWritten;
}
// FormatToStream
//	format string and write to Stream
// WriteStream	[in]	destination to write
// FormatString	[in]	format string
// return total output length
template<class TCharacter,class TStreamWriteBuffer>
inline uIntn FormatStringToStream(TStreamWriteBuffer &&WriteBuffer,const TCharacter *FormatString)
{
	uIntn StringLength=cnString::GetLength(FormatString);
	if(StringLength==0)
		return 0;
	return tempWriteArray(WriteBuffer,FormatString,StringLength);
}

// FormatToStream
//	format string and write to Stream
// WriteStream			destination to write
// FormatString	[in]	format string
// Args...		[in]	format arguments
// return total output length
template<class TCharacter,class TStreamWriteBuffer,class T,class...TArgs>
inline uIntn FormatStringToStream(TStreamWriteBuffer &WriteBuffer,const TCharacter *FormatString,const T &Arg,const TArgs&...Args)
{
	uIntn StringLength=std::char_traits<TCharacter>::length(FormatString);
	cnStream::cMemoryCachedStreamWriteBuffer<cnRTL::cAllocationOperator,cnDataStruct::cMemoryOperatorByGeniric<TCharacter,cnMemory::cGenericMemoryOperator>,TStreamWriteBuffer> CachedWriteBuffer(&WriteBuffer);

	StringLength*=2;
	auto WriteMemory=CachedWriteBuffer.ReserveWriteBuffer(StringLength);
	if(WriteMemory.Length==0){
		return 0;
	}
	int pr=cConvertFunction<TCharacter>::SPrintF(WriteMemory.Pointer,StringLength,FormatString,Arg,Args...);
	while(pr<0 || pr>=static_cast<int>(StringLength)){
		CachedWriteBuffer.CommitWriteBuffer(0);

		StringLength*=2;
		WriteMemory=CachedWriteBuffer.ReserveWriteBuffer(StringLength);
		if(WriteMemory.Length==0){
			return 0;
		}
		pr=cConvertFunction<TCharacter>::SPrintF(WriteMemory.Pointer,StringLength,FormatString,Arg,Args...);
	}
	CachedWriteBuffer.CommitWriteBuffer(pr);
	return static_cast<uIntn>(pr);
}

template<class TCharacter,class TStreamWriteBuffer,class T,class...TArgs>
uIntn FormatStringToStream(TStreamWriteBuffer &&WriteBuffer,const TCharacter *FormatString,const T &Arg,const TArgs&...Args)
{
	return FormatStringToStream(WriteBuffer,FormatString,Arg,Args...);
}

// Integer to String

template<class TWriteStream,class T>
typename std::enable_if<
	std::is_integral<T>::value && std::is_signed<T>::value && sizeof(T)<=sizeof(int),
uIntn>::type FormatIntToStream(TWriteStream &&WriteStream,T Value,ufInt8 Radix,ufInt8 Precision,ufInt16 Width)
{
	typedef typename cnVar::TRemoveReference<TWriteStream>::Type::tElement TCharacter;
	TCharacter FormatString[]={'%','*','.','*','i',0};

	if(Radix==16){
		FormatString[4]='X';
	}

	return FormatStringToStream(WriteStream,FormatString,Width,Precision,static_cast<int>(Value));
}
template<class TWriteStream,class T>
typename std::enable_if<
	std::is_integral<T>::value && std::is_unsigned<T>::value && sizeof(T)<=sizeof(int),
uIntn>::type FormatIntToStream(TWriteStream &&WriteStream,T Value,ufInt8 Radix,ufInt8 Precision,ufInt16 Width)
{
	typedef typename cnVar::TRemoveReference<TWriteStream>::Type::tElement TCharacter;
	TCharacter FormatString[]={'%','*','.','*','u',0};

	if(Radix==16){
		FormatString[4]='X';
	}

	return FormatStringToStream(WriteStream,FormatString,Width,Precision,static_cast<unsigned int>(Value));
}
template<class TWriteStream,class T>
typename std::enable_if<
	std::is_integral<T>::value && std::is_signed<T>::value && (sizeof(T)>sizeof(int)),
uIntn>::type FormatIntToStream(TWriteStream &&WriteStream,T Value,ufInt8 Radix,ufInt8 Precision,ufInt16 Width)
{
	typedef typename cnVar::TRemoveReference<TWriteStream>::Type::tElement TCharacter;
	TCharacter FormatString[]={'%','*','.','*','l','i',0};

	if(Radix==16){
		FormatString[5]='X';
	}

	return FormatStringToStream(WriteStream,FormatString,Width,Precision,static_cast<long long>(Value));
}

template<class TWriteStream,class T>
typename std::enable_if<
	std::is_integral<T>::value && std::is_unsigned<T>::value && (sizeof(T)>sizeof(int)),
uIntn>::type FormatIntToStream(TWriteStream &&WriteStream,T Value,ufInt8 Radix,ufInt8 Precision,ufInt16 Width)
{
	typedef typename cnVar::TRemoveReference<TWriteStream>::Type::tElement TCharacter;
	TCharacter FormatString[]={'%','*','.','*','l','u',0};

	if(Radix==16){
		FormatString[5]='X';
	}

	return FormatStringToStream(WriteStream,FormatString,Width,Precision,static_cast<unsigned long long>(Value));
}

// Float to String

template<class TWriteStream,class T>
typename std::enable_if<
	std::is_convertible<T,double>::value,
bool>::type FormatFloatToStream(TWriteStream &&WriteStream,T Value,ufInt8 Radix,ufInt8 Precision,ufInt16 Width)
{UnusedParameter(Radix);
	typedef typename cnVar::TRemoveReference<TWriteStream>::Type::tElement TCharacter;
	TCharacter FormatString[]={'%','*','.','*','f',0};
	return FormatStringToStream(WriteStream,FormatString,Width,Precision,Value);
}


// EFloat to String

template<class TWriteStream,class T>
typename std::enable_if<
	std::is_convertible<T,double>::value,
bool>::type FormatEFloatToStream(TWriteStream &&WriteStream,T Value,ufInt8 Radix,ufInt8 Precision,ufInt16 Width)
{UnusedParameter(Radix);
	typedef typename cnVar::TRemoveReference<TWriteStream>::Type::tElement TCharacter;
	TCharacter FormatString[]={'%','*','.','*','e',0};
	return FormatStringToStream(WriteStream,FormatString,Width,Precision,Value);
}

#endif // 0


template<class T,class TCharacter>
uIntn ToInt(T &Result,const TCharacter *Src,uInt8 Radix=10,uIntn MaxLength=StringMaxLength)
{
	cnLib_ASSERT(Src!=nullptr);
	Result=0;
	uIntn SrcIndex=0;
	while(SrcIndex<MaxLength){
		TCharacter c=Src[SrcIndex];
		T t;
		if(c<0x30)
			break;
		if(c<=0x39){
			t=static_cast<T>(c-0x30);
		}
		else if(c<='A'){
			break;
		}
		else if(c<'F'){
			t=static_cast<T>(c-'A')+10;
		}
		else if(c<='a'){
			break;
		}
		else if(c<'f'){
			t=static_cast<T>(c-'a')+10;
		}
		else{
			break;
		}
		Result*=Radix;
		Result+=t;
		SrcIndex++;
		if(SrcIndex>=MaxLength)
			break;
	}

	return SrcIndex;
}

template<class T,class TCharacter>
uIntn ToFloat(T &Result,const TCharacter *Src,uInt8 Radix=10,uIntn MaxLength=StringMaxLength)
{
	cnLib_ASSERT(Src!=nullptr);
	Result=0;
	uIntn SrcIndex=0;
	while(SrcIndex<MaxLength){
		TCharacter c=Src[SrcIndex];
		T t;
		if(c=='.'){
			SrcIndex++;
			while(SrcIndex<MaxLength){
				c=Src[SrcIndex];
				if(c<0x30)
					break;
				if(c<=0x39){
					t=static_cast<T>(c-0x30);
				}
				else if(c<='A'){
					break;
				}
				else if(c<'F'){
					t=static_cast<T>(c-'A')+10;
				}
				else if(c<='a'){
					break;
				}
				else if(c<'f'){
					t=static_cast<T>(c-'a')+10;
				}
				else{
					break;
				}
				Result*=Radix;
				Result+=t;
				SrcIndex++;
			}
			break;
		}
		if(c<0x30)
			break;
		if(c<=0x39){
			t=static_cast<T>(c-0x30);
		}
		else if(c<='A'){
			break;
		}
		else if(c<'F'){
			t=static_cast<T>(c-'A')+10;
		}
		else if(c<='a'){
			break;
		}
		else if(c<'f'){
			t=static_cast<T>(c-'a')+10;
		}
		else{
			break;
		}
		Result*=Radix;
		Result+=t;
		SrcIndex++;
	}

	return SrcIndex;
}

//---------------------------------------------------------------------------
}	// namespace cnString
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif


