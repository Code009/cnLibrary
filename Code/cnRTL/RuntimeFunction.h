/*- cnRTL - Runtime Function ----------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2009-03-06                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnRTL_RuntimeFunction_H__
#define __cnLibrary_cnRTL_RuntimeFunction_H__
/*-------------------------------------------------------------------------*/
#include <cnRTL/cnRTLHeader.h>
/*-------------------------------------------------------------------------*/
#if	cnLibrary_CPPFEATURELEVEL >= 1
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
inline void ZeroFill(void *Data,uIntn Size){	return TKRuntime::TMemory<1>::ZeroFill(Data,Size);	}

// ZeroFill
// [in]Data		array to fill
template<class T>
inline void ZeroFill(T &Data){	return ZeroFill(&Data,sizeof(T));	}

// ZeroFill
// [in]Data		array to fill
template<class T,uIntn DataLength>
inline void ZeroFill(T (&Data)[DataLength]){	return ZeroFill(&Data,sizeof(T)*DataLength);	}


inline void Copy(void *Dest,const void *Src,uIntn Size){	return TKRuntime::TMemory<1>::Copy(Dest,Src,Size);	}
inline void CopyO(void *Dest,const void *Src,uIntn Size){	return TKRuntime::TMemory<1>::CopyOverlapped(Dest,Src,Size);	}


inline bool IsEqual(const void *Mem1,const void *Mem2,uIntn Size){
	return TKRuntime::TMemory<1>::Equal(Mem1,Mem2,Size);
}

inline sfInt8 Compare(const void *Mem1,const void *Mem2,uIntn Size)
{
	return TKRuntime::TMemory<1>::Compare(Mem1,Mem2,Size);
}


//---------------------------------------------------------------------------
}	// namespace cnMemory
//---------------------------------------------------------------------------
namespace cnString{
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
}	// namespace cnString
//---------------------------------------------------------------------------
namespace cnRTL{
//---------------------------------------------------------------------------

class cErrorFrame
{
public:
	cErrorFrame()noexcept(true);
	~cErrorFrame()noexcept(true);
	rPtr<iErrorReport> MakeReport(void)noexcept(true);

	cStringBuffer<uChar16> Function;
	cStringBuffer<uChar16> Action;
	cStringBuffer<uChar16> Error;

};

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
extern uInt8 BitReverse(uInt8 Src);

//---------------------------------------------------------------------------
template<class TEnum>
inline bool BitsAllZero(TEnum v){
	return v==static_cast<TEnum>(0);
}
//---------------------------------------------------------------------------
template<class TEnum>
inline bool BitsNonZero(TEnum v){
	return v!=static_cast<TEnum>(0);
}
//---------------------------------------------------------------------------
template<class TEnum>
inline bool BitsContains(TEnum v,typename cnVar::TTypeDef<TEnum>::Type Bits){
	return (v&Bits)!=static_cast<TEnum>(0);
}

//---------------------------------------------------------------------------

// Variant

template<class...VT>
struct cVariantTypeOperator
{
	template<uIntn TypeIndex>	struct tTypeByIndex : cnVar::TSelect<TypeIndex,VT...>{};
	
	static const tTypeID TypeIDList[];
	static tTypeID GetTypeID(uIntn TypeIndex){
		return TypeIDList[TypeIndex];
	}

	static const rtTypeInfo TypeInfoList[];
	static rtTypeInfo GetTypeInfo(uIntn TypeIndex){
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
	template<class TElement,uIntn ElementCount>
	operator cnVar::cVector<TElement,ElementCount> ()const{
		cnVar::cVector<TElement,ElementCount> RetValue;
		RetValue.Zero();
		return RetValue;
	}

	template<class TElement,uIntn ElementCount>
	operator cnVar::cIntegerVector<TElement,ElementCount> ()const{
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
	cVectorFillValue(TValue Value):fValue(Value){}

	template<class TElement,uIntn ElementCount>
	operator cnVar::cVector<TElement,ElementCount> ()const{
		cnVar::cVector<TElement,ElementCount> RetValue;
		RetValue.Fill(fValue);
		return RetValue;
	}

	template<class TElement,uIntn ElementCount>
	operator cnVar::cIntegerVector<TElement,ElementCount> ()const{
		cnVar::cIntegerVector<TElement,ElementCount> RetValue;
		RetValue.Fill(fValue);
		return RetValue;
	}
private:
	TValue fValue;
};

template<class TElement>
cVectorFillValue<TElement> VectorFillValue(TElement Value)
{
	return Value;
}
//---------------------------------------------------------------------------
template<uIntn ElementCount,class TElement>
inline cVector<TElement,ElementCount> VectorMake(const TElement *Value)
{
	cVector<TElement,ElementCount> RetValue;
	for(uIntn i=0;i<ElementCount;i++){
		RetValue[i]=Value[i];
	}
	return RetValue;
}
//---------------------------------------------------------------------------
template<class TElement,uIntn ElementCount>
inline cVector<TElement,ElementCount> VectorMakeZero(void)
{
	cVector<TElement,ElementCount> RetValue;
	RetValue.Zero();
	return RetValue;
}
//---------------------------------------------------------------------------
template<uIntn ElementCount,class TElement>
inline cVector<TElement,ElementCount> VectorMakeFill(TElement Value)
{
	cVector<TElement,ElementCount> RetValue;
	RetValue.Fill(Value);
	return RetValue;
}
//---------------------------------------------------------------------------

// Integer Vector

//---------------------------------------------------------------------------
template<uIntn ElementCount,class TElement>
inline cIntegerVector<TElement,ElementCount> IntegerVectorMake(const TElement *Value)
{
	cIntegerVector<TElement,ElementCount> RetValue;
	for(uIntn i=0;i<ElementCount;i++){
		RetValue[i]=Value[i];
	}
	return RetValue;
}
//---------------------------------------------------------------------------
template<class TElement,uIntn ElementCount>
inline cIntegerVector<TElement,ElementCount> IntegerVectorMakeZero(void)
{
	cIntegerVector<TElement,ElementCount> RetValue;
	RetValue.Zero();
	return RetValue;
}
//---------------------------------------------------------------------------
template<uIntn ElementCount,class TElement>
inline cIntegerVector<TElement,ElementCount> IntegerVectorMakeFill(TElement Value)
{
	cIntegerVector<TElement,ElementCount> RetValue;
	RetValue.Fill(Value);
	return RetValue;
}
//---------------------------------------------------------------------------

// Time

//---------------------------------------------------------------------------
sInt64 NanosecondsSinceUnixEPoch(iTimepoint *Time);
//---------------------------------------------------------------------------
class cTime : public iTimepoint
{
private:
	sInt64 fNanoSeconds;
public:
	static cTime TimeNow(void);

	cTime()=default;
	cTime(const cTime &Src)=default;
	explicit cnLib_CONSTEXPR_FUNC cTime(sInt64 SystemNanoSeconds):fNanoSeconds(SystemNanoSeconds){}
	cTime(iTimepoint *RefTime);
	~cTime();

	virtual sInt64 cnLib_FUNC SystemTime(void)override;
	virtual sInt64 cnLib_FUNC SinceTime(iTimepoint *Time)override;

	sInt64 Since(iTimepoint *Time)const;

	operator iTimepoint*()const;

	cTime& operator =(iTimepoint *RefTime);
	
	cTime operator + (sInt64 Src)const;
	cTime operator - (sInt64 Src)const;
	sInt64 operator - (const cTime &Relative)const;
	sInt64 operator - (iTimepoint *Relative)const;

	cTime& operator +=(sInt64 Src);
	cTime& operator -=(sInt64 Src);


	bool operator == (const cTime &Src)const;
	bool operator != (const cTime &Src)const;
	bool operator < (const cTime &Src)const;
	bool operator <= (const cTime &Src)const;
	bool operator > (const cTime &Src)const;
	bool operator >= (const cTime &Src)const;

	bool operator == (iTimepoint *Src)const;
	bool operator != (iTimepoint *Src)const;
	bool operator < (iTimepoint *Src)const;
	bool operator <= (iTimepoint *Src)const;
	bool operator > (iTimepoint *Src)const;
	bool operator >= (iTimepoint *Src)const;

	sInt64 SinceUnixEpoch(void)const;
	void SetTimeUnix(sInt64 NanoSeconds);
	void SetTimeNow(void);
	void SetSystemTime(sInt64 SystemTime);

	void Truncate(uInt64 Mod);

	friend sInt64 operator - (iTimepoint *Dest,const cTime &Src);

	friend bool operator == (iTimepoint *Dest,const cTime &Src);
	friend bool operator != (iTimepoint *Dest,const cTime &Src);
	friend bool operator < (iTimepoint *Dest,const cTime &Src);
	friend bool operator <= (iTimepoint *Dest,const cTime &Src);
	friend bool operator > (iTimepoint *Dest,const cTime &Src);
	friend bool operator >= (iTimepoint *Dest,const cTime &Src);
};

//---------------------------------------------------------------------------
template<class TSingleLinkedItemOperator>
class bcAtomicLinkRecyclePool : public cnAsync::cAtomicStack<TSingleLinkedItemOperator>
{
public:
	typedef typename TSingleLinkedItemOperator::tItem tItem;

	// Object Creation

	tItem* Fetch(void){
		return this->Pop();
	}

	void Recycle(tItem *Object){
		this->Push(Object);
	}

	tItem* Query(void){
		auto Object=this->Pop();
		if(Object==nullptr){
			Object=new tItem;
		}
		return Object;
	}

	void Clear(void){
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

	cnLib_CONSTEXPR_FUNC cObjectRecyclePool():fRefCount(0){}

	void IncreaseReference(void){
		fRefCount.Free++;
	}
	void DecreaseReference(void){
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
	cSharedLinkItemRecycler()
		: fPool(cnVar::StaticInitializedSinglton< cObjectRecyclePool<TItem> >())
	{}
	~cSharedLinkItemRecycler(){}

	TItem* Query(void)
	{
		return fPool->Query();
	}
	TItem* Fetch(void)
	{
		return fPool->Fetch();
	}
	void Recycle(TItem *Item)
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
	cnLib_CONSTEXPR_FUNC cSpinLock():fOwned(false){}
#ifdef cnLib_DEBUG
	~cSpinLock(){	cnLib_ASSERT(fOwned==false);	}
#endif // cnLib_DEBUG

	void Acquire(void);
	bool TryAcquire(void);
	void Release(void);
protected:
	cAtomicVar<bool> fOwned=false;
public:
	ufInt16 SpinCount=16384;
};
//---------------------------------------------------------------------------
template<class TPtr>
struct cLockRefTokenOperator : cnVar::bcPointerRefTokenOperator<TPtr>
{
	static void Acquire(TPtr Token){	if(Token!=nullptr)	Token->Acquire();	}
	static void Release(TPtr Token){	if(Token!=nullptr)	Token->Release();	}
};
//---------------------------------------------------------------------------
template<class TPtr>
struct cSharedLockRefTokenOperator : cnVar::bcPointerRefTokenOperator<TPtr>
{
	static void Acquire(TPtr Token){	if(Token!=nullptr)	Token->AcquireShared();	}
	static void Release(TPtr Token){	if(Token!=nullptr)	Token->ReleaseShared();	}
};
//---------------------------------------------------------------------------
template<class TPtr>
using lockPtr = cnVar::cPtrReference< cLockRefTokenOperator<TPtr> >;
template<class TPtr>
using lockSharedPtr = cnVar::cPtrReference< cSharedLockRefTokenOperator<TPtr> >;
//---------------------------------------------------------------------------
template<class TLock>
inline lockPtr<TLock*> TakeLock(TLock *Lock)
{
	return Lock;
}
//---------------------------------------------------------------------------
template<class TLock>
inline auto TakeLock(TLock &Lock) -> lockPtr<decltype(Lock.operator ->())>
{
	return Lock.operator ->();
}
//---------------------------------------------------------------------------
template<class TLock>
inline lockSharedPtr<TLock*> TakeSharedLock(TLock *Lock)
{
	return Lock;
}
//---------------------------------------------------------------------------
template<class TLock>
inline auto TakeSharedLock(TLock &Lock) -> lockSharedPtr<decltype(Lock.operator ->())>
{
	return Lock.operator ->();
}
//---------------------------------------------------------------------------

// Weak Reference

//---------------------------------------------------------------------------
template<class T>
struct rRefWeakTokenOperator
{
	typedef T* TPtr;
	typedef rPtr<T> TRef;
	typedef iObservedReference* TRegToken;
	typedef iReferenceInvalidationNotify TNotifyToken;

	static TRegToken Register(TRef Ref,TNotifyToken *NotifyToken){
		if(Ref==nullptr)
			return nullptr;
		Ref->InvalidationRegisterNotification(NotifyToken);
		return Ref;
	}
	static TRegToken Register(TPtr Pointer,TNotifyToken *NotifyToken){
		if(Pointer==nullptr)
			return nullptr;
		Pointer->InvalidationRegisterNotification(NotifyToken);
		return Pointer;
	}
	static void Unregister(TRegToken RegToken,TNotifyToken *NotifyToken){
		return RegToken->InvalidationUnregisterNotification(NotifyToken);
	}
	static TRef Reference(TRegToken RegToken){
		return RegToken->Reference()?TRef::TakeFromManual(RegToken):nullptr;
	}
};
//---------------------------------------------------------------------------
template<class T>
using rwPtr=cnVar::cPtrWeakReference< rRefWeakTokenOperator<T> >;
//---------------------------------------------------------------------------
class bcInterfaceWeakPointer : protected iReferenceInvalidationNotify
{
public:
	bcInterfaceWeakPointer();
	~bcInterfaceWeakPointer();

	bcInterfaceWeakPointer(bcInterfaceWeakPointer &Src);

	bcInterfaceWeakPointer(iInterface *Src);



protected:
	cSpinLock fTokenLock;
	iObservedReference *fWeakRef;
	iInterface *fInterface;

	void Assign(iInterface *Src);
	bool ToStrong(void);
	virtual bool cnLib_FUNC InvalidationNotify(iObservedReference *InvalidatedReference)noexcept(true) override;
};
//---------------------------------------------------------------------------
template<class T>
class iwPtr : protected bcInterfaceWeakPointer
{
public:
	iwPtr(){}
	~iwPtr(){}

	iwPtr(iwPtr &Src) : bcInterfaceWeakPointer(Src){}

	iwPtr(T *Src) : bcInterfaceWeakPointer(Src){}

	iwPtr& operator =(T *Src){
		Assign(Src);
		return *this;
	}


	iPtr<T> Ref(void){
		if(bcInterfaceWeakPointer::ToStrong()){
			return iPtr<T>::TakeFromManual(static_cast<T*>(this->fInterface));
		}

		return nullptr;
	}
	iPtr<T> operator + (void){
		return Ref();
	}
};
//---------------------------------------------------------------------------

// AsyncFlag

//---------------------------------------------------------------------------
class cExclusiveFlag
{
public:
	cnLib_CONSTEXPR_FUNC cExclusiveFlag():RunFlag(rfIdle){}
	cnLib_CONSTEXPR_FUNC cExclusiveFlag(bool InitalRun):RunFlag(InitalRun?rfPending:rfIdle){}

	// Acquire
	//	test if caller acquired the flag, if not ,request owner to continue
	// return true if the flag is acquired
	bool Acquire(void);
	// Release
	//	release the flag if no request pending
	//	if there was pending request, the thread should continue running
	//	this function must run called by owner
	// return true if the owner release the flag, false if there is pending request
	bool Release(void);

	// Continue
	//	continue execution and clear pending state
	void Continue(void);

	bool IsRunning(void)const;
private:
	static cnLib_CONSTVAR uInt8 rfIdle=0;
	static cnLib_CONSTVAR uInt8 rfExecute=1;
	static cnLib_CONSTVAR uInt8 rfPending=2;
	cAtomicVar<uInt8> RunFlag;
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

	void Log(void *Object,uInt32 Tag,bool Inc);
private:
	class cContext : public iProcedure, public iDependentInfo
	{
	public:
		virtual void cnLib_FUNC Execute(void)override;

		cSeqMap<void*, cSeqMap<uInt32,uIntn> > fObjectMap;

		void Inc(void *Object,uInt32 Tag);
		void Dec(void *Object,uInt32 Tag);

		static const uChar16 DependentName[];

		virtual rPtr< iArrayReference<const uChar16> > cnLib_FUNC DependentCreateDescription(void)override;
		virtual void cnLib_FUNC DependentShutdownNotification(void)override;
	};
	cnVar::cStaticVariable<cContext> fContext;
	cAtomicQueueSO<cItem> fItemQueue;
	cAtomicStack<cItem> fRecycleStack;

	cExclusiveFlag fExclusiveFlag;
	bool fInitialized;
	ufInt8 fSystemShutdown;

	void Process(void);

	void ThreadProcess(void);
	void NotifyProcess(void);
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
void iIncReference(T *Src,uInt32 Tag)
{
	cnRTL_DEBUG_LOG_REFERENCE_INC(Src,Tag);
	return iIncReference(Src);
}
template<class T>
void iDecReference(T *Src,uInt32 Tag)
{
	cnRTL_DEBUG_LOG_REFERENCE_DEC(Src,Tag);
	return iDecReference(Src);
}
//---------------------------------------------------------------------------
template<class T>
void iIncReference(const iPtr<T> &Src,uInt32 Tag)
{
	cnRTL_DEBUG_LOG_REFERENCE_INC(Src,Tag);
	return iIncReference(Src);
}
template<class T>
void iDecReference(const iPtr<T> &Src,uInt32 Tag)
{
	cnRTL_DEBUG_LOG_REFERENCE_DEC(Src,Tag);
	return iDecReference(Src);
}
//---------------------------------------------------------------------------
template<class T>
iPtr<T> iTake(T *Src,uInt32 Tag)
{
	cnRTL_DEBUG_LOG_REFERENCE_DEC(Src,Tag);
	return iPtr<T>::TakeFromManual(Src);
}
template<class T>
T* iExtract(iPtr<T> &Src,uInt32 Tag)
{
	cnRTL_DEBUG_LOG_REFERENCE_INC(static_cast<T*>(Src),Tag);
	return Src.ExtractToManual();
}
//---------------------------------------------------------------------------
template<class T>
void rIncReference(T *Src,uInt32 Tag)
{
	cnRTL_DEBUG_LOG_REFERENCE_INC(Src,Tag);
	return rIncReference(Src);
}
template<class T>
void rDecReference(T *Src,uInt32 Tag)
{
	cnRTL_DEBUG_LOG_REFERENCE_DEC(Src,Tag);
	return rDecReference(Src);
}
//---------------------------------------------------------------------------
template<class T>
void rIncReference(const iPtr<T> &Src,uInt32 Tag)
{
	cnRTL_DEBUG_LOG_REFERENCE_INC(Src,Tag);
	return rIncReference(Src);
}
template<class T>
void rDecReference(const iPtr<T> &Src,uInt32 Tag)
{
	cnRTL_DEBUG_LOG_REFERENCE_DEC(Src,Tag);
	return rDecReference(Src);
}
//---------------------------------------------------------------------------
template<class T>
rPtr<T> rTake(T *Src,uInt32 Tag)
{
	cnRTL_DEBUG_LOG_REFERENCE_DEC(Src,Tag);
	return rPtr<T>::TakeFromManual(Src);
}
template<class T>
T* rExtract(rPtr<T> &Src,uInt32 Tag)
{
	cnRTL_DEBUG_LOG_REFERENCE_INC(Src,Tag);
	return Src.ExtractToManual();
}
//---------------------------------------------------------------------------
namespace cnRTL{
//---------------------------------------------------------------------------
template<uIntn CacheLength,class TStreamReadBuffer>
cnStream::cCachedStreamReadBuffer<TStreamReadBuffer,CacheLength> CachedStreamReadBuffer(TStreamReadBuffer&& ReadBuffer)
{
	return cnStream::cCachedStreamReadBuffer<TStreamReadBuffer,CacheLength>(static_cast<TStreamReadBuffer&&>(ReadBuffer));
}
//---------------------------------------------------------------------------
template<uIntn CacheLength,class TStreamWriteBuffer>
cnStream::cCachedStreamWriteBuffer<TStreamWriteBuffer,CacheLength> CachedStreamWriteBuffer(TStreamWriteBuffer&& WriteBuffer)
{
	return cnStream::cCachedStreamWriteBuffer<TStreamWriteBuffer,CacheLength>(static_cast<TStreamWriteBuffer&&>(WriteBuffer));
}
//---------------------------------------------------------------------------
namespace ArrayStream{
//---------------------------------------------------------------------------
// Read
//---------------------------------------------------------------------------
template<class TStreamReadBuffer>
inline bool ReadElement(TStreamReadBuffer &ReadBuffer,typename TStreamReadBuffer::tElement &Element)
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
inline bool ReadElement(TStreamReadBuffer&& ReadBuffer,typename cnVar::TRemoveReference<TStreamReadBuffer>::Type::tElement &Element)
{
	return ReadElement(ReadBuffer,Element);
}
//---------------------------------------------------------------------------
template<class TStreamReadBuffer>
inline bool ReadArray(TStreamReadBuffer &ReadBuffer,typename TStreamReadBuffer::tElement *Array,uIntn Length)
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
inline bool ReadArray(TStreamReadBuffer&& ReadBuffer,typename cnVar::TRemoveReference<TStreamReadBuffer>::Type::tElement *Array,uIntn Length)
{
	return ReadArray(ReadBuffer,Array,Length);
}
//---------------------------------------------------------------------------
// Write
//---------------------------------------------------------------------------
template<class TStreamWriteBuffer>
inline bool WriteElement(TStreamWriteBuffer &WriteBuffer,typename TStreamWriteBuffer::tElement Element)
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
inline bool WriteElement(TStreamWriteBuffer&& WriteBuffer,typename cnVar::TRemoveReference<TStreamWriteBuffer>::Type::tElement Element)
{
	return WriteElement(WriteBuffer,Element);
}
//---------------------------------------------------------------------------
template<class TStreamWriteBuffer>
inline bool WriteFill(TStreamWriteBuffer &WriteBuffer,typename TStreamWriteBuffer::tElement Element,uIntn Count)
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
inline bool WriteFill(TStreamWriteBuffer&& WriteBuffer,typename cnVar::TRemoveReference<TStreamWriteBuffer>::Type::tElement Element,uIntn Count)
{
	return WriteFill(WriteBuffer,Element,Count);
}
//---------------------------------------------------------------------------
template<class TStreamWriteBuffer>
inline bool WriteArray(TStreamWriteBuffer &WriteBuffer,typename TStreamWriteBuffer::tElement const *Array,uIntn Length)
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
inline bool WriteArray(TStreamWriteBuffer&& WriteBuffer,typename cnVar::TRemoveReference<TStreamWriteBuffer>::Type::tElement const *Array,uIntn Length)
{
	return WriteArray(WriteBuffer,Array,Length);
}
//---------------------------------------------------------------------------
template<class TStreamWriteBuffer>
inline bool WriteFillArray(TStreamWriteBuffer &WriteBuffer,typename TStreamWriteBuffer::tElement const *Array,uIntn Length,uIntn Count)
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
inline bool WriteFillArray(TStreamWriteBuffer&& WriteBuffer,typename cnVar::TRemoveReference<TStreamWriteBuffer>::Type::tElement const *Array,uIntn Length,uIntn Count)
{
	return WriteFillArray(WriteBuffer,Array,Length,Count);
}
//---------------------------------------------------------------------------
}	// namespace ArrayStream
//---------------------------------------------------------------------------
template<class T>
struct cArrayWriteElement
{
	T Element;
	
	template<class TStreamWriteBuffer>
	friend bool operator +=(TStreamWriteBuffer&& WriteBuffer,const cArrayWriteElement &Src){
		return ArrayStream::WriteElement(WriteBuffer,Src.Element);
	}
};

template<class T>
inline cArrayWriteElement<T> ArrayWriteElement(T&& Element){
	cArrayWriteElement<T> ArrayWrite={
		static_cast<T&&>(Element)
	};
	return ArrayWrite;
}
//---------------------------------------------------------------------------
template<class T>
struct cArrayWriteFill
{
	T Element;
	uIntn Count;

	template<class TStreamWriteBuffer>
	friend bool operator +=(TStreamWriteBuffer&& WriteBuffer,const cArrayWriteFill &Src){
		return ArrayStream::WriteFill(WriteBuffer,Src.Element,Src.Count);
	}
};

template<class T>
inline cArrayWriteFill<T> ArrayWriteFill(T&& Element,uIntn Count){
	cArrayWriteFill<T> ArrayWrite={
		static_cast<T&&>(Element),
		Count
	};
	return ArrayWrite;
}
//---------------------------------------------------------------------------
template<class T>
struct cArrayWriteArray
{
	const T *Array;
	uIntn Length;

	template<class TStreamWriteBuffer>
	friend bool operator +=(TStreamWriteBuffer&& WriteBuffer,const cArrayWriteArray &Src){
		return ArrayStream::WriteArray(WriteBuffer,Src.Array,Src.Length);
	}

};

template<class T>
inline cArrayWriteArray<T> ArrayWriteArray(const T *Array,uIntn Length){
	cArrayWriteArray<T> ArrayWrite={
		Array,
		Length
	};
	return ArrayWrite;
}
//---------------------------------------------------------------------------
template<class T>
struct cArrayWriteFillArray
{
	const T *Array;
	uIntn Length;
	uIntn Count;

	template<class TStreamWriteBuffer>
	friend bool operator +=(TStreamWriteBuffer&& WriteBuffer,const cArrayWriteFillArray &Src){
		return ArrayStream::WriteFillArray(WriteBuffer,Src.Array,Src.Length,Src.Count);
	}
};

template<class T>
inline cArrayWriteFillArray<T> ArrayWriteFillArray(const T *Array,uIntn Length,uIntn Count){
	cArrayWriteFillArray<T> ArrayWrite={
		Array,
		Length,
		Count
	};
	return ArrayWrite;
}
//---------------------------------------------------------------------------
}	// namespace cnRTL
//---------------------------------------------------------------------------
}   // namespace cnLibrary
//---------------------------------------------------------------------------
#include <cnTK/TKMacrosCleanup.inc>
//---------------------------------------------------------------------------
#endif  /* cnLibrary_CPPFEATURELEVEL >= 1 */
/*-------------------------------------------------------------------------*/
#endif

