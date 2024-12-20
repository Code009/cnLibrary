/*- cnRTL - Runtime Function ----------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2009-03-06                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnRTL_RuntimeFunction_H__
#define __cnLibrary_cnRTL_RuntimeFunction_H__
/*-------------------------------------------------------------------------*/
#include <cnRTL/cnRTLHeader.h>
/*-------------------------------------------------------------------------*/
#if	__cplusplus
//---------------------------------------------------------------------------
#include <cnTK/TKMacrosDeclare.inc>
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnMath{
//---------------------------------------------------------------------------

// Math

//---------------------------------------------------------------------------

static cnLib_CONSTVAR Float32 fPI=TFloatConstant<Float32>::PI;
static cnLib_CONSTVAR Float64 dPI=TFloatConstant<Float64>::PI;

static cnLib_CONSTVAR Float32 flog2e=TFloatConstant<Float32>::log2e;
static cnLib_CONSTVAR Float64 dlog2e=TFloatConstant<Float64>::log2e;

static cnLib_CONSTVAR Float32 floge2=TFloatConstant<Float32>::loge2;
static cnLib_CONSTVAR Float64 dloge2=TFloatConstant<Float64>::loge2;

static cnLib_CONSTVAR Float32 flog2_10=TFloatConstant<Float32>::log2_10;
static cnLib_CONSTVAR Float64 dlog2_10=TFloatConstant<Float64>::log2_10;

//---------------------------------------------------------------------------
}	// namespace cnMath

namespace cnMemory{

// ZeroFill
// [in]Data		array to fill
inline void ZeroFill(void *Data,uIntn Size)noexcept(true){	return TKRuntime::TMemory<1>::ZeroFill(Data,Size);	}

// ZeroFill
// [in]Data		array to fill
template<class T>
inline void ZeroFill(T &Data)noexcept(true){	return ZeroFill(&Data,sizeof(T));	}

// ZeroFill
// [in]Data		array to fill
template<class T,uIntn DataLength>
inline void ZeroFill(T (&Data)[DataLength])noexcept(true){	return ZeroFill(&Data,sizeof(T)*DataLength);	}


inline void Copy(void *Dest,const void *Src,uIntn Size)noexcept(true){	return TKRuntime::TMemory<1>::Copy(Dest,Src,Size);	}
inline void CopyO(void *Dest,const void *Src,uIntn Size)noexcept(true){	return TKRuntime::TMemory<1>::CopyOverlapped(Dest,Src,Size);	}


inline bool IsEqual(const void *Mem1,const void *Mem2,uIntn Size)noexcept(true)
{
	return TKRuntime::TMemory<1>::Equal(Mem1,Mem2,Size);
}

inline sfInt8 Compare(const void *Mem1,const void *Mem2,uIntn Size)noexcept(true)
{
	return TKRuntime::TMemory<1>::Compare(Mem1,Mem2,Size);
}


//---------------------------------------------------------------------------
}	// namespace cnMemory
//---------------------------------------------------------------------------
namespace cnRTL{
////---------------------------------------------------------------------------
//template<uIntn CacheLength,class TStreamReadBuffer>
//inline cnStream::cCachedStreamReadBuffer<TStreamReadBuffer,CacheLength> CachedStreamReadBuffer(TStreamReadBuffer&& ReadBuffer)noexcept(true)
//{
//	return cnStream::cCachedStreamReadBuffer<TStreamReadBuffer,CacheLength>(static_cast<TStreamReadBuffer&&>(ReadBuffer));
//}
////---------------------------------------------------------------------------
//template<uIntn CacheLength,class TStreamWriteBuffer>
//inline cnStream::cCachedStreamWriteBuffer<TStreamWriteBuffer,CacheLength> CachedStreamWriteBuffer(TStreamWriteBuffer&& WriteBuffer)noexcept(true)
//{
//	return cnStream::cCachedStreamWriteBuffer<TStreamWriteBuffer,CacheLength>(static_cast<TStreamWriteBuffer&&>(WriteBuffer));
//}
//---------------------------------------------------------------------------
namespace ArrayStream{
//---------------------------------------------------------------------------
// Read
//---------------------------------------------------------------------------
template<class TStreamReadBuffer>
inline bool ReadElement(TStreamReadBuffer &ReadBuffer,typename TStreamReadBuffer::tElement &Element)noexcept(true)
{
	typedef typename TStreamReadBuffer::tElement tElement;
	cArray<const tElement> Memory=ReadBuffer.GatherReadBuffer(1);
	if(Memory.Length==0)
		return false;
	Element=*Memory.Pointer;
	ReadBuffer.DismissReadBuffer(1);
	return true;
}
//---------------------------------------------------------------------------
template<class TStreamReadBuffer>
inline bool ReadElement(TStreamReadBuffer&& ReadBuffer,typename cnVar::TRemoveReference<TStreamReadBuffer>::Type::tElement &Element)noexcept(true)
{
	return ReadElement(ReadBuffer,Element);
}
//---------------------------------------------------------------------------
template<class TStreamReadBuffer>
inline bool ReadArray(TStreamReadBuffer &ReadBuffer,typename TStreamReadBuffer::tElement *Array,uIntn Length)noexcept(true)
{
	if(Length==0)
		return true;
	if(Length==1){
		return ReadElement(ReadBuffer,*Array);
	}
	typedef typename TStreamReadBuffer::tElement tElement;
	typedef cnDataStruct::cArrayMemoryOperator<tElement> tArrayOperator;
	cArray<const tElement> CurBuffer;
	while((CurBuffer=ReadBuffer.GatherReadBuffer(Length)).Length!=0){
		if(CurBuffer.Length>=Length){
			tArrayOperator::Copy(Array,CurBuffer.Pointer,Length);
			ReadBuffer.DismissReadBuffer(Length);
			return true;
		}
		// copy
		tArrayOperator::Copy(Array,CurBuffer.Pointer,CurBuffer.Length);
		// offset dest pointer
		Length-=CurBuffer.Length;
		Array=ArrayOffsetPointer(Array,CurBuffer.Length);
		// next block
		ReadBuffer.DismissReadBuffer(CurBuffer.Length);
	}
	return false;
}
//---------------------------------------------------------------------------
template<class TStreamReadBuffer>
inline bool ReadArray(TStreamReadBuffer&& ReadBuffer,typename cnVar::TRemoveReference<TStreamReadBuffer>::Type::tElement *Array,uIntn Length)noexcept(true)
{
	return ReadArray(ReadBuffer,Array,Length);
}
//---------------------------------------------------------------------------
// Write
//---------------------------------------------------------------------------
template<class TStreamWriteBuffer>
inline bool WriteElement(TStreamWriteBuffer &WriteBuffer,typename TStreamWriteBuffer::tElement Element)noexcept(true)
{
	typedef typename TStreamWriteBuffer::tElement tElement;
	cArray<tElement> Memory=WriteBuffer.ReserveWriteBuffer(1);
	if(Memory.Length==0)
		return false;
	*Memory.Pointer=Element;
	WriteBuffer.CommitWriteBuffer(1);
	return true;
}
template<class TStreamWriteBuffer>
inline bool WriteElement(TStreamWriteBuffer&& WriteBuffer,typename cnVar::TRemoveReference<TStreamWriteBuffer>::Type::tElement Element)noexcept(true)
{
	return WriteElement(WriteBuffer,Element);
}
//---------------------------------------------------------------------------
template<class TStreamWriteBuffer>
inline bool WriteFill(TStreamWriteBuffer &WriteBuffer,typename TStreamWriteBuffer::tElement Element,uIntn Count)noexcept(true)
{
	if(Count==0)
		return true;
	typedef typename TStreamWriteBuffer::tElement tElement;
	cArray<tElement> CurBuffer;
	while((CurBuffer=WriteBuffer.ReserveWriteBuffer(Count)).Length!=0){
		if(CurBuffer.Length>=Count){
			TKRuntime::TArray<sizeof(tElement)>::Fill(CurBuffer.Pointer,Count,Element);
			WriteBuffer.CommitWriteBuffer(Count);
			return true;
		}
		// fill
		TKRuntime::TArray<sizeof(tElement)>::Fill(CurBuffer.Pointer,CurBuffer.Length,Element);
		Count-=CurBuffer.Length;
		// next block
		WriteBuffer.CommitWriteBuffer(CurBuffer.Length);
	}
	return false;
}
template<class TStreamWriteBuffer>
inline bool WriteFill(TStreamWriteBuffer&& WriteBuffer,typename cnVar::TRemoveReference<TStreamWriteBuffer>::Type::tElement Element,uIntn Count)noexcept(true)
{
	return WriteFill(WriteBuffer,Element,Count);
}
//---------------------------------------------------------------------------
template<class TStreamWriteBuffer>
inline bool WriteArray(TStreamWriteBuffer &WriteBuffer,typename TStreamWriteBuffer::tElement const *Array,uIntn Length)noexcept(true)
{
	if(Length==0)
		return true;
	if(Length==1){
		return WriteElement(WriteBuffer,Array[0]);
	}
	typedef typename TStreamWriteBuffer::tElement tElement;
	typedef cnDataStruct::cArrayMemoryOperator<tElement> tArrayOperator;
	cArray<tElement> CurBuffer;
	while((CurBuffer=WriteBuffer.ReserveWriteBuffer(Length)).Length!=0){
		if(CurBuffer.Length>=Length){
			tArrayOperator::Copy(CurBuffer.Pointer,Array,Length);
			WriteBuffer.CommitWriteBuffer(Length);
			return true;
		}
		// copy
		tArrayOperator::Copy(CurBuffer.Pointer,Array,CurBuffer.Length);
		// offset dest pointer
		Length-=CurBuffer.Length;
		Array=ArrayOffsetPointer(Array,CurBuffer.Length);
		// next block
		WriteBuffer.CommitWriteBuffer(CurBuffer.Length);
	}
	return false;
}
//---------------------------------------------------------------------------
template<class TStreamWriteBuffer>
inline bool WriteArray(TStreamWriteBuffer&& WriteBuffer,typename cnVar::TRemoveReference<TStreamWriteBuffer>::Type::tElement const *Array,uIntn Length)noexcept(true)
{
	return WriteArray(WriteBuffer,Array,Length);
}
//---------------------------------------------------------------------------
template<class TStreamWriteBuffer>
inline bool WriteFillArray(TStreamWriteBuffer &WriteBuffer,typename TStreamWriteBuffer::tElement const *Array,uIntn Length,uIntn Count)noexcept(true)
{
	if(Length==0)
		return true;
	if(Length==1){
		return WriteFill(WriteBuffer,Array[0],Count);
	}
	if(Count==0)
		return true;
	if(Count==1){
		return WriteArray(WriteBuffer,Array,Length);
	}

	for(uIntn i=0;i<Count;i++){
		if(WriteArray(WriteBuffer,Array,Length)==false)
			return false;
	}
	return true;
}
//---------------------------------------------------------------------------
template<class TStreamWriteBuffer>
inline bool WriteFillArray(TStreamWriteBuffer&& WriteBuffer,typename cnVar::TRemoveReference<TStreamWriteBuffer>::Type::tElement const *Array,uIntn Length,uIntn Count)noexcept(true)
{
	return WriteFillArray(WriteBuffer,Array,Length,Count);
}
//---------------------------------------------------------------------------
}	// namespace ArrayStream
//---------------------------------------------------------------------------
template<class T>
struct cArrayStreamElement
{
	T Element;
	
	template<class TStreamWriteBuffer>
	friend bool operator +=(TStreamWriteBuffer&& WriteBuffer,const cArrayStreamElement &Src)noexcept(true){
		return ArrayStream::WriteElement(WriteBuffer,Src.Element);
	}
};

template<class T>
inline cArrayStreamElement<T> ArrayStreamElement(T&& Element)noexcept(true){
	cArrayStreamElement<T> ArrayWrite={
		static_cast<T&&>(Element)
	};
	return ArrayWrite;
}
//---------------------------------------------------------------------------
template<class T>
struct cArrayStreamFill
{
	T Element;
	uIntn Count;

	template<class TStreamWriteBuffer>
	friend bool operator +=(TStreamWriteBuffer&& WriteBuffer,const cArrayStreamFill &Src)noexcept(true){
		return ArrayStream::WriteFill(WriteBuffer,Src.Element,Src.Count);
	}
};

template<class T>
inline cArrayStreamFill<T> ArrayStreamFill(T&& Element,uIntn Count)noexcept(true){
	cArrayStreamFill<T> ArrayWrite={
		static_cast<T&&>(Element),
		Count
	};
	return ArrayWrite;
}
//---------------------------------------------------------------------------
template<class T>
struct cArrayStreamArray
{
	const T *Array;
	uIntn Length;

	template<class TStreamWriteBuffer>
	friend bool operator +=(TStreamWriteBuffer&& WriteBuffer,const cArrayStreamArray &Src)noexcept(true){
		return ArrayStream::WriteArray(WriteBuffer,Src.Array,Src.Length);
	}

};

template<class T>
inline cArrayStreamArray<T> ArrayStreamArray(const T *Array,uIntn Length)noexcept(true){
	cArrayStreamArray<T> ArrayWrite={
		Array,
		Length
	};
	return ArrayWrite;
}
template<class T>
inline cArrayStreamArray<T> ArrayStreamArray(const cArray<T> Array)noexcept(true){
	cArrayStreamArray<T> ArrayWrite={
		Array.Pointer,
		Array.Length
	};
	return ArrayWrite;
}

template<class T,uIntn Length>
inline cArrayStreamArray<T> ArrayStreamArray(const T (&Array)[Length])noexcept(true){
	cArrayStreamArray<T> ArrayWrite={
		Array,
		Length
	};
	return ArrayWrite;
}

//---------------------------------------------------------------------------

template<class TCharacter>
inline cArrayStreamArray<TCharacter> ArrayStreamCString(const TCharacter *String)noexcept(true){
	cArrayStreamArray<TCharacter> ArrayWrite={
		String,cnString::FindLength(String)
	};
	return ArrayWrite;
}

template<class TString>
inline auto ArrayStreamString(TString&& String)noexcept(true) -> decltype(ArrayStreamCString(String)){
	return ArrayStreamCString(String);
}

template<class TCharacter,uIntn Length>
inline cArrayStreamArray<TCharacter> ArrayStreamString(const TCharacter (&String)[Length])noexcept(true){
	cArrayStreamArray<TCharacter> ArrayWrite={
		String,Length-1
	};
	return ArrayWrite;
}

template<class TCharacter,uIntn Length>
inline cArrayStreamArray<TCharacter> ArrayStreamString(TCharacter (&String)[Length])noexcept(true){
	cArrayStreamArray<TCharacter> ArrayWrite={
		String,cnString::FindLength(String,Length)
	};
	return ArrayWrite;
}

//---------------------------------------------------------------------------
template<class T>
struct cArrayStreamFillArray
{
	const T *Array;
	uIntn Length;
	uIntn Count;

	template<class TStreamWriteBuffer>
	friend bool operator +=(TStreamWriteBuffer&& WriteBuffer,const cArrayStreamFillArray &Src)noexcept(true){
		return ArrayStream::WriteFillArray(WriteBuffer,Src.Array,Src.Length,Src.Count);
	}
};

template<class T>
inline cArrayStreamFillArray<T> ArrayStreamFillArray(const T *Array,uIntn Length,uIntn Count)noexcept(true){
	cArrayStreamFillArray<T> ArrayWrite={
		Array,
		Length,
		Count
	};
	return ArrayWrite;
}
//---------------------------------------------------------------------------

// Meta Class

#if cnLibrary_CPPFEATURE_CONSTEXPR >= 200704L

#endif // cnLibrary_CPPFEATURE_CONSTEXPR >= 200704L


template<class TClass,class TMember>
constexpr cnVar::cMetaClassField MakeMetaClassField(const uChar16 *Name,TMember TClass::*Pointer)
{
	return cnVar::cMetaClassField{
		Name,
		cnVar::TRuntimeDataInfo<TMember>::Value,
		reinterpret_cast<uIntn>(&(static_cast<TClass*>(nullptr)->*Pointer)),
	};

}

template<class T>
struct cMetaClassDefinition
{
	static const cnVar::cMetaClass Class;
	static const cnVar::cMetaClassField FieldList[];
	static const cnVar::cMetaClassFunction FunctionList[];
	static const cnVar::cMetaClass *const Value;
};

template<class T>
const cnVar::cMetaClassField cMetaClassDefinition<T>::FieldList[]={
	{nullptr}
};
template<class T>
const cnVar::cMetaClassFunction cMetaClassDefinition<T>::FunctionList[]={
	{nullptr}
};

template<class T>
const cnVar::cMetaClass cMetaClassDefinition<T>::Class={
	FieldList[0].Name==nullptr?0:sizeof(FieldList)/sizeof(cnVar::cMetaClassField),
	FunctionList[0].Name==nullptr?0:sizeof(FunctionList)/sizeof(cnVar::cMetaClassFunction),
	FieldList,
	FunctionList
};

template<class T>
const cnVar::cMetaClass *const cMetaClassDefinition<T>::Value=&Class;

//---------------------------------------------------------------------------

// Bits 

// BitReverse
//	Reverse bits in byte
// [in]Src
// return: reversed data
extern uInt8 BitReverse(uInt8 Src)noexcept(true);

//---------------------------------------------------------------------------
template<class TEnum>
inline bool BitsAllZero(TEnum v)noexcept(true){
	return v==static_cast<TEnum>(0);
}
//---------------------------------------------------------------------------
template<class TEnum>
inline bool BitsNonZero(TEnum v)noexcept(true){
	return v!=static_cast<TEnum>(0);
}
//---------------------------------------------------------------------------
template<class TEnum>
inline bool BitsContains(TEnum v,typename cnVar::TTypeDef<TEnum>::Type Bits)noexcept(true){
	return (v&Bits)!=static_cast<TEnum>(0);
}

//---------------------------------------------------------------------------

// Variant

template<class...VT>
struct cVariantTypeOperator
{
	template<uIntn TypeIndex>	struct tTypeByIndex : cnVar::TSelect<TypeIndex,VT...>{};
	
	static const tTypeID TypeIDList[];
	static tTypeID GetTypeID(uIntn TypeIndex)noexcept(true){
		return TypeIDList[TypeIndex];
	}

	static const rtTypeInfo TypeInfoList[];
	static rtTypeInfo GetTypeInfo(uIntn TypeIndex)noexcept(true){
		return TypeInfoList[TypeIndex];
	}

	static cnLib_CONSTVAR bool IsDefaultConstructNoexcept	=cnVar::TBooleanValuesAnd<cnVar::TIsDefaultConstructNoexcept<VT>::Value...>::Value;
	static cnLib_CONSTVAR bool IsDestructNoexcept			=cnVar::TBooleanValuesAnd<cnVar::TIsDestructNoexcept<VT>::Value...>::Value;;
	static cnLib_CONSTVAR bool IsCopyConstructNoexcept		=cnVar::TBooleanValuesAnd<cnVar::TIsCopyConstructNoexcept<VT>::Value...>::Value;
	static cnLib_CONSTVAR bool IsMoveConstructNoexcept		=cnVar::TBooleanValuesAnd<cnVar::TIsMoveConstructNoexcept<VT>::Value...>::Value;
	static cnLib_CONSTVAR bool IsCopyAssignNoexcept			=cnVar::TBooleanValuesAnd<cnVar::TIsCopyAssignNoexcept<VT>::Value...>::Value;
	static cnLib_CONSTVAR bool IsMoveAssignNoexcept			=cnVar::TBooleanValuesAnd<cnVar::TIsMoveAssignNoexcept<VT>::Value...>::Value;

	typedef ufInt8 tTypeIndex;
	static constexpr ufInt8 DefaultTypeIndex=0;

	static cnLib_CONSTVAR tSize UnionSize=cnMath::TMax<tSize,sizeof(VT)...>::Value;
	typedef typename cnMemory::TSelectAlignmentType<VT...>::Type tUnionAlignment;

};

template<class...VT>	const tTypeID cVariantTypeOperator<VT...>::TypeIDList[]={cnVar::TTypeID<VT>::Value...};
template<class...VT>	const rtTypeInfo cVariantTypeOperator<VT...>::TypeInfoList[]={cnVar::TRuntimeTypeInfo<VT>::Value...};

template<class...VT>
using cVariant=cnVar::cVariant< cVariantTypeOperator<VT...> >;
//---------------------------------------------------------------------------

// Vector

//---------------------------------------------------------------------------
class cVectorZeroValue
{
public:
	template<class TVectorOperator,uIntn VectorCount>
	operator cnVar::cVectorStorage<TVectorOperator,VectorCount> ()const noexcept(true){
		cnVar::cVectorStorage<TVectorOperator,VectorCount> RetValue;
		RetValue.Zero();
		return RetValue;
	}

	template<class TElement,uIntn ElementCount>
	operator cnVar::cVector<TElement,ElementCount> ()const noexcept(true){
		cnVar::cVector<TElement,ElementCount> RetValue;
		RetValue.Zero();
		return RetValue;
	}

	template<class TElement,uIntn ElementCount>
	operator cnVar::cIntegerVector<TElement,ElementCount> ()const noexcept(true){
		cnVar::cIntegerVector<TElement,ElementCount> RetValue;
		RetValue.Zero();
		return RetValue;
	}

};
extern const cVectorZeroValue VectorZeroValue;
//---------------------------------------------------------------------------
template<class TValue>
class cVectorFillValue
{
public:
	cVectorFillValue(TValue Value)noexcept(true):fValue(Value){}

	template<class TVectorOperator,uIntn VectorCount>
	operator cnVar::cVectorStorage<TVectorOperator,VectorCount> ()const noexcept(true){
		cnVar::cVectorStorage<TVectorOperator,VectorCount> RetValue;
		RetValue.Fill(fValue);
		return RetValue;
	}

	//template<class TElement,uIntn ElementCount>
	//operator cnVar::cVector<TElement,ElementCount> ()const noexcept(true){
	//	cnVar::cVector<TElement,ElementCount> RetValue;
	//	RetValue.Fill(fValue);
	//	return RetValue;
	//}

	//template<class TElement,uIntn ElementCount>
	//operator cnVar::cIntegerVector<TElement,ElementCount> ()const noexcept(true){
	//	cnVar::cIntegerVector<TElement,ElementCount> RetValue;
	//	RetValue.Fill(fValue);
	//	return RetValue;
	//}

private:
	TValue fValue;
};

template<class TElement>
cVectorFillValue<TElement> VectorFillValue(TElement Value)noexcept(true)
{
	return Value;
}
//---------------------------------------------------------------------------
template<uIntn ElementCount,class TElement>
inline cVector<TElement,ElementCount> VectorMake(const TElement *Value)noexcept(true)
{
	cVector<TElement,ElementCount> RetValue;
	for(uIntn i=0;i<ElementCount;i++){
		RetValue[i]=Value[i];
	}
	return RetValue;
}
//---------------------------------------------------------------------------
template<class TElement,uIntn ElementCount>
inline cVector<TElement,ElementCount> VectorMakeZero(void)noexcept(true)
{
	cVector<TElement,ElementCount> RetValue;
	RetValue.Zero();
	return RetValue;
}
//---------------------------------------------------------------------------
template<uIntn ElementCount,class TElement>
inline cVector<TElement,ElementCount> VectorMakeFill(TElement Value)noexcept(true)
{
	cVector<TElement,ElementCount> RetValue;
	RetValue.Fill(Value);
	return RetValue;
}
//---------------------------------------------------------------------------

// Integer Vector

//---------------------------------------------------------------------------
template<uIntn ElementCount,class TElement>
inline cIntegerVector<TElement,ElementCount> IntegerVectorMake(const TElement *Value)noexcept(true)
{
	cIntegerVector<TElement,ElementCount> RetValue;
	for(uIntn i=0;i<ElementCount;i++){
		RetValue[i]=Value[i];
	}
	return RetValue;
}
//---------------------------------------------------------------------------
template<class TElement,uIntn ElementCount>
inline cIntegerVector<TElement,ElementCount> IntegerVectorMakeZero(void)noexcept(true)
{
	cIntegerVector<TElement,ElementCount> RetValue;
	RetValue.Zero();
	return RetValue;
}
//---------------------------------------------------------------------------
template<uIntn ElementCount,class TElement>
inline cIntegerVector<TElement,ElementCount> IntegerVectorMakeFill(TElement Value)noexcept(true)
{
	cIntegerVector<TElement,ElementCount> RetValue;
	RetValue.Fill(Value);
	return RetValue;
}
//---------------------------------------------------------------------------

// Time

//---------------------------------------------------------------------------
class cSystemTime : public iTimepoint
{
private:
	sInt64 fNanoSeconds;
public:
	static cSystemTime TimeNow(void)noexcept(true);

	cSystemTime()=default;
	cSystemTime(const cSystemTime &Src)=default;
	explicit cnLib_CONSTEXPR_FUNC cSystemTime(sInt64 SystemNanoSeconds)noexcept(true):fNanoSeconds(SystemNanoSeconds){}
	cSystemTime(iTimepoint *RefTime)noexcept(true);
	~cSystemTime()noexcept(true);

	virtual sInt64 cnLib_FUNC SystemTime(void)noexcept(true)override;
	virtual sInt64 cnLib_FUNC SinceTime(iTimepoint *Time)noexcept(true)override;

	sInt64 Since(iTimepoint *Time)const noexcept(true);

	operator iTimepoint*()const noexcept(true);

	cSystemTime& operator =(iTimepoint *RefTime)noexcept(true);
	
	cSystemTime operator + (sInt64 Src)const noexcept(true);
	cSystemTime operator - (sInt64 Src)const noexcept(true);
	sInt64 operator - (const cSystemTime &Relative)const noexcept(true);
	sInt64 operator - (iTimepoint *Relative)const noexcept(true);

	cSystemTime& operator +=(sInt64 Src)noexcept(true);
	cSystemTime& operator -=(sInt64 Src)noexcept(true);


	bool operator == (const cSystemTime &Src)const noexcept(true);
	bool operator != (const cSystemTime &Src)const noexcept(true);
	bool operator < (const cSystemTime &Src)const noexcept(true);
	bool operator <= (const cSystemTime &Src)const noexcept(true);
	bool operator > (const cSystemTime &Src)const noexcept(true);
	bool operator >= (const cSystemTime &Src)const noexcept(true);

	bool operator == (iTimepoint *Src)const noexcept(true);
	bool operator != (iTimepoint *Src)const noexcept(true);
	bool operator < (iTimepoint *Src)const noexcept(true);
	bool operator <= (iTimepoint *Src)const noexcept(true);
	bool operator > (iTimepoint *Src)const noexcept(true);
	bool operator >= (iTimepoint *Src)const noexcept(true);

	sInt64 SinceUnixEpoch(void)const noexcept(true);
	void SetTimeUnix(sInt64 NanoSeconds)noexcept(true);
	void SetTimeNow(void)noexcept(true);
	void SetSystemTime(sInt64 SystemTime)noexcept(true);

	void Truncate(uInt64 Mod)noexcept(true);

	friend sInt64 operator - (iTimepoint *Dest,const cSystemTime &Src)noexcept(true);

	friend bool operator == (iTimepoint *Dest,const cSystemTime &Src)noexcept(true);
	friend bool operator != (iTimepoint *Dest,const cSystemTime &Src)noexcept(true);
	friend bool operator < (iTimepoint *Dest,const cSystemTime &Src)noexcept(true);
	friend bool operator <= (iTimepoint *Dest,const cSystemTime &Src)noexcept(true);
	friend bool operator > (iTimepoint *Dest,const cSystemTime &Src)noexcept(true);
	friend bool operator >= (iTimepoint *Dest,const cSystemTime &Src)noexcept(true);
};
//---------------------------------------------------------------------------
uInt64 SystemTimeToUnixTimeNS(uInt64 SystemTime)noexcept(true);
uInt64 SystemTimeFromUnixTimeNS(uInt64 UnixTime)noexcept(true);
uInt64 TimeNSTruncate(uInt64 UnixTime,uInt64 Mod)noexcept(true);
//---------------------------------------------------------------------------
template<class TSingleLinkedItemOperator>
class bcAtomicLinkRecyclePool : public cnAsync::cAtomicStack<TSingleLinkedItemOperator>
{
public:
	typedef typename TSingleLinkedItemOperator::tItem tItem;

	// Object Creation

	tItem* Fetch(void)noexcept(true){
		return this->Pop();
	}

	void Recycle(tItem *Object)noexcept(true){
		this->Push(Object);
	}

	tItem* Query(void)noexcept(true){
		auto Object=this->Pop();
		if(Object==nullptr){
			Object=new tItem;
		}
		return Object;
	}

	void Clear(void)noexcept(true){
		auto ItemsToDelete=this->Swap(nullptr);
		while(ItemsToDelete!=nullptr){
			auto DeleteObject=ItemsToDelete;
			ItemsToDelete=TSingleLinkedItemOperator::GetNext(ItemsToDelete);

			delete DeleteObject;
		}
	}

};
//---------------------------------------------------------------------------
template<class T>
class cObjectRecyclePool : public bcAtomicLinkRecyclePool< cnDataStruct::cSingleLinkItemOperator<T> >
{
public:

	cnLib_CONSTEXPR_FUNC cObjectRecyclePool()noexcept(true):fRefCount(0){}

	void IncreaseReference(void)noexcept(true){
		fRefCount.Free++;
	}
	void DecreaseReference(void)noexcept(true){
		if(fRefCount.Free--==1){
			this->Clear();
		}
	}
protected:
	cAtomicVar<uIntn> fRefCount;

};
//---------------------------------------------------------------------------
template<class TItem>
class cSharedLinkItemRecycler
{
public:
	cSharedLinkItemRecycler()noexcept(true)
		: fPool(cnVar::StaticInitializedSinglton< cObjectRecyclePool<TItem> >())
	{}
	~cSharedLinkItemRecycler()noexcept(true){}

	TItem* Query(void)noexcept(true)
	{
		return fPool->Query();
	}
	TItem* Fetch(void)noexcept(true)
	{
		return fPool->Fetch();
	}
	void Recycle(TItem *Item)noexcept(true)
	{
		return fPool->Recycle(Item);
	}

private:
	rPtr< cObjectRecyclePool<TItem> > fPool;
};
//---------------------------------------------------------------------------

// Synchronization

//---------------------------------------------------------------------------
class cSpinLock
{
public:
	cnLib_CONSTEXPR_FUNC cSpinLock()noexcept(true):fOwned(false){}
#ifdef cnLib_DEBUG
	~cSpinLock()noexcept(true){	cnLib_ASSERT(fOwned==false);	}
#endif // cnLib_DEBUG

	void Acquire(void)noexcept(true);
	bool TryAcquire(void)noexcept(true);
	void Release(void)noexcept(true);
protected:
	cAtomicVar<bool> fOwned=false;
public:
	ufInt16 SpinCount=16384;
};
//---------------------------------------------------------------------------
struct cLockPointerReferenceOperator
{
	template<class T>	static void Acquire(T *Pointer)noexcept(true){	Pointer->Acquire();	}
	template<class T>	static void Release(T *Pointer)noexcept(true){	Pointer->Release();	}
};
//---------------------------------------------------------------------------
struct cSharedLockPointerReferenceOperator
{
	template<class T>	static void Acquire(T *Pointer)noexcept(true){	Pointer->AcquireShared();	}
	template<class T>	static void Release(T *Pointer)noexcept(true){	Pointer->ReleaseShared();	}
};
//---------------------------------------------------------------------------
template<class T>
using lockPtr = cnVar::cPtrReference<T,cLockPointerReferenceOperator>;
template<class T>
using lockSharedPtr = cnVar::cPtrReference<T,cSharedLockPointerReferenceOperator>;
//---------------------------------------------------------------------------
template<class TLock>
inline lockPtr<TLock> TakeLock(TLock *Lock)noexcept(true)
{
	return Lock;
}
//---------------------------------------------------------------------------
template<class TLock>
inline auto TakeLock(TLock &Lock)noexcept(true) -> lockPtr<typename cnVar::TRemovePointer<decltype(Lock.operator ->())>::Type>
{
	return Lock.operator ->();
}
//---------------------------------------------------------------------------
template<class TLock>
inline lockSharedPtr<TLock> TakeSharedLock(TLock *Lock)noexcept(true)
{
	return Lock;
}
//---------------------------------------------------------------------------
template<class TLock>
inline auto TakeSharedLock(TLock &Lock)noexcept(true) -> lockSharedPtr<typename cnVar::TRemovePointer<decltype(Lock.operator ->())>::Type>
{
	return Lock.operator ->();
}
//---------------------------------------------------------------------------

// AsyncFlag

//---------------------------------------------------------------------------
class cExclusiveFlag
{
public:
	cnLib_CONSTEXPR_FUNC cExclusiveFlag():RunFlag(rfIdle){}
	cnLib_CONSTEXPR_FUNC cExclusiveFlag(bool InitalRun):RunFlag(InitalRun?rfPending:rfIdle){}

	void Reset(void)noexcept(true);

	// Acquire
	//	test if caller acquired the flag, if not ,request owner to continue
	// return true if the flag is acquired
	bool Acquire(void)noexcept(true);
	// Release
	//	release the flag if no request pending
	//	if there was pending request, the thread should continue running
	//	this function must run called by owner
	// return true if the owner release the flag, false if there is pending request
	bool Release(void)noexcept(true);

	// Continue
	//	continue execution and clear pending state
	void Continue(void)noexcept(true);

	bool IsRunning(void)const noexcept(true);
private:
	static cnLib_CONSTVAR uInt8 rfIdle=0;
	static cnLib_CONSTVAR uInt8 rfExecute=1;
	static cnLib_CONSTVAR uInt8 rfPending=2;
	cAtomicVar<uInt8> RunFlag;
};
//---------------------------------------------------------------------------
class cResourceAvailableFlag
{
public:
	cnLib_CONSTEXPR_FUNC cResourceAvailableFlag():AvailableFlag(rfIdle){}

	void Start(void)noexcept(true);
	void Pause(void)noexcept(true);

	bool Finish(void)noexcept(true);

	bool MarkAvailable(void)noexcept(true);
private:
	static cnLib_CONSTVAR ufInt8 rfIdle=0;
	static cnLib_CONSTVAR ufInt8 rfPending=1;
	static cnLib_CONSTVAR ufInt8 rfRunning=2;
	cAtomicVar<ufInt8> AvailableFlag;
};
//---------------------------------------------------------------------------

// Reference Logger

//---------------------------------------------------------------------------
class cReferenceCountLogger
{
public:

	struct cItem
	{
		cItem *Next;
		void *Object;
		uInt32 Tag;
		bool Inc;
	};

	void Log(void *Object,uInt32 Tag,bool Inc)noexcept(true);
private:
	class cContext : public iProcedure, public iDependentInfo
	{
	public:
		virtual void cnLib_FUNC Execute(void)noexcept(true)override;

		cSeqMap<void*, cSeqMap<uInt32,uIntn> > fObjectMap;

		void Inc(void *Object,uInt32 Tag)noexcept(true);
		void Dec(void *Object,uInt32 Tag)noexcept(true);

		static const uChar16 DependentName[];

		virtual rPtr<iStringReference> cnLib_FUNC DependentCreateDescription(void)noexcept(true)override;
		virtual void cnLib_FUNC DependentShutdownNotification(void)noexcept(true)override;
	};
	cnVar::cStaticVariable<cContext> fContext;
	cAtomicQueueSO<cItem> fItemQueue;
	cAtomicStack<cItem> fRecycleStack;

	cExclusiveFlag fExclusiveFlag;
	bool fInitialized;
	ufInt8 fSystemShutdown;

	void Process(void)noexcept(true);

	void ThreadProcess(void)noexcept(true);
	void NotifyProcess(void)noexcept(true);
};
extern cnVar::cStaticVariable<cReferenceCountLogger> gStaticReferenceCountLogger;
//---------------------------------------------------------------------------
}	// namespace cnRTL
//---------------------------------------------------------------------------
#ifndef cnRTL_DEBUG_LOG_REFERENCE_INC

#ifdef cnLib_DEBUG
#define	cnRTL_DEBUG_LOG_REFERENCE_INC(_object_,_tag_)	cnLibrary::cnRTL::gStaticReferenceCountLogger->Log(_object_,_tag_,true)
#else
#define	cnRTL_DEBUG_LOG_REFERENCE_INC(_object_,_tag_)	cnLibrary::UnusedParameter(_object_,_tag_)
#endif

#endif // !cnRTL_DEBUG_LOG_REFERENCE
#ifndef cnRTL_DEBUG_LOG_REFERENCE_DEC

#ifdef cnLib_DEBUG
#define	cnRTL_DEBUG_LOG_REFERENCE_DEC(_object_,_tag_)	cnLibrary::cnRTL::gStaticReferenceCountLogger->Log(_object_,_tag_,false)
#else
#define	cnRTL_DEBUG_LOG_REFERENCE_DEC(_object_,_tag_)	cnLibrary::UnusedParameter(_object_,_tag_)
#endif

#endif // !cnRTL_DEBUG_LOG_REFERENCE_DEC
//---------------------------------------------------------------------------
template<class T>
inline void iIncReference(T *Src,uInt32 Tag)noexcept(true)
{
	cnRTL_DEBUG_LOG_REFERENCE_INC(Src,Tag);
	return iIncReference(Src);
}
template<class T>
inline void iDecReference(T *Src,uInt32 Tag)noexcept(true)
{
	cnRTL_DEBUG_LOG_REFERENCE_DEC(Src,Tag);
	return iDecReference(Src);
}
//---------------------------------------------------------------------------
template<class T>
inline void iIncReference(const iPtr<T> &Src,uInt32 Tag)noexcept(true)
{
	cnRTL_DEBUG_LOG_REFERENCE_INC(Src,Tag);
	return iIncReference(Src);
}
template<class T>
inline void iDecReference(const iPtr<T> &Src,uInt32 Tag)noexcept(true)
{
	cnRTL_DEBUG_LOG_REFERENCE_DEC(Src,Tag);
	return iDecReference(Src);
}
//---------------------------------------------------------------------------
template<class T>
inline iPtr<T> iTake(T *Src,uInt32 Tag)noexcept(true)
{
	cnRTL_DEBUG_LOG_REFERENCE_DEC(Src,Tag);
	return iPtr<T>::TakeFromManual(Src);
}
template<class T>
inline T* iExtract(iPtr<T> &Src,uInt32 Tag)noexcept(true)
{
	cnRTL_DEBUG_LOG_REFERENCE_INC(static_cast<T*>(Src),Tag);
	return Src.ExtractToManual();
}
//---------------------------------------------------------------------------
template<class T>
inline void rIncReference(T *Src,uInt32 Tag)noexcept(true)
{
	cnRTL_DEBUG_LOG_REFERENCE_INC(Src,Tag);
	return rIncReference(Src);
}
template<class T>
inline void rDecReference(T *Src,uInt32 Tag)noexcept(true)
{
	cnRTL_DEBUG_LOG_REFERENCE_DEC(Src,Tag);
	return rDecReference(Src);
}
//---------------------------------------------------------------------------
template<class T>
inline void rIncReference(const iPtr<T> &Src,uInt32 Tag)noexcept(true)
{
	cnRTL_DEBUG_LOG_REFERENCE_INC(Src,Tag);
	return rIncReference(Src);
}
template<class T>
inline void rDecReference(const iPtr<T> &Src,uInt32 Tag)noexcept(true)
{
	cnRTL_DEBUG_LOG_REFERENCE_DEC(Src,Tag);
	return rDecReference(Src);
}
//---------------------------------------------------------------------------
template<class T>
inline rPtr<T> rTake(T *Src,uInt32 Tag)noexcept(true)
{
	cnRTL_DEBUG_LOG_REFERENCE_DEC(Src,Tag);
	return rPtr<T>::TakeFromManual(Src);
}
template<class T>
inline T* rExtract(rPtr<T> &Src,uInt32 Tag)noexcept(true)
{
	cnRTL_DEBUG_LOG_REFERENCE_INC(Src,Tag);
	return Src.ExtractToManual();
}
//---------------------------------------------------------------------------
}   // namespace cnLibrary
//---------------------------------------------------------------------------
#include <cnTK/TKMacrosCleanup.inc>
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif

