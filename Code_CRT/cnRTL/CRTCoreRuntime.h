/*- cnLibCRT - CRT Template Kit -------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2021-09-04                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnRTL_CRTCoreRuntime_H__
#define __cnLibrary_cnRTL_CRTCoreRuntime_H__

#include <cnRTL/CRTCoreCommon.h>

#ifdef __cplusplus

//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------

template<>
struct TKRuntime::TMathFloat<4>
{
	static float Absolute(float n){				return ::fabsf(n);	}
	static float RoundNearest(float n){			return ::roundf(n);	}
	static float RoundTruncate(float n){		return ::truncf(n);	}
	static float RoundNegative(float n){		return ::floorf(n);	}
	static float RoundPositive(float n){		return ::ceilf(n);	}

	static float SplitExponent(float n,sfInt16 &Exponent)noexcept(true){	int e;	float r=::frexpf(n,&e);	Exponent=static_cast<sfInt16>(e);	return r;	}

	static float Modulo(float x,float y){		return ::fmodf(x,y);	}

	static float SquareRoot(float n){			return ::sqrtf(n);	}
	static float NaturalLogarithm(float n){		return ::logf(n);	}
	static float BinaryLogarithm(float n){		return ::log2f(n);	}
	static float NaturalExponential(float n){	return ::expf(n);	}
	static float BinaryExponential(float n){	return ::exp2f(n);	}
	static float Power(float b,float e){		return ::powf(b,e);	}

	static float Sine(float n){					return ::sinf(n);	}
	static float Cosine(float n){				return ::cosf(n);	}
	static float Tangent(float n){				return ::tanf(n);	}
	static float ArcSine(float n){				return ::asinf(n);	}
	static float ArcCosine(float n){			return ::acosf(n);	}
	static float ArcTangent(float n){			return ::atanf(n);	}
	static float ArcTangent2(float y,float x){	return ::atan2f(y,x);	}
};

template<>
struct TKRuntime::TMathFloat<8>
{
	
	static double Absolute(double n){				return ::fabs(n);	}
	static double RoundNearest(double n){			return ::round(n);	}
	static double RoundTruncate(double n){			return ::trunc(n);	}
	static double RoundNegative(double n){			return ::floor(n);	}
	static double RoundPositive(double n){			return ::ceil(n);	}

	static double SplitExponent(double n,sfInt16 &Exponent)noexcept(true){	int e;	double r=::frexp(n,&e);	Exponent=static_cast<sfInt16>(e);	return r;	}

	static double Modulo(double x,double y){		return ::fmod(x,y);	}

	static double SquareRoot(double n){				return ::sqrt(n);	}
	static double NaturalLogarithm(double n){		return ::log(n);	}
	static double BinaryLogarithm(double n){		return ::log2(n);	}
	static double NaturalExponential(double n){		return ::exp(n);	}
	static double BinaryExponential(double n){		return ::exp2(n);	}
	static double Power(double b,double e){			return ::pow(b,e);	}

	static double Sine(double n){					return ::sin(n);	}
	static double Cosine(double n){					return ::cos(n);	}
	static double Tangent(double n){				return ::tan(n);	}
	static double ArcSine(double n){				return ::asin(n);	}
	static double ArcCosine(double n){				return ::acos(n);	}
	static double ArcTangent(double n){				return ::atan(n);	}
	static double ArcTangent2(double y,double x){	return ::atan2(y,x);	}

};

#if cnCRT_BYTEORDER_LITTLEENDIAN
struct TKRuntime::NativeByteOrder
	: cnVar::TConstantValue<cnMemory::eByteOrder,cnMemory::ByteOrder::LittleEndian>
{};
#endif
#if cnCRT_BYTEORDER_BIGENDIAN
struct TKRuntime::NativeByteOrder
	: cnVar::TConstantValue<cnMemory::eByteOrder,cnMemory::ByteOrder::BigEndian>
{};
#endif


template<uIntn IntegerSize>
struct TKRuntime::TInteger
{
	typedef typename cnVar::TIntegerOfSize<IntegerSize,false>::Type tUInt;

	static tUInt UnalignRead(const tUInt *Variable){
		tUInt Ret;
		memcpy(&Ret,Pointer,IntegerSize);
		return Ret;
	}
	static void UnalignWrite(tUInt *Variable,tUInt Value){
		memcpy(Variable,&Value,IntegerSize);
	}
	static tUInt ReverseBytes(tUInt Value){
		tUInt Dest;
		for(uIntn i=0;i<IntegerSize;i++){
			reinterpret_cast<uInt8*>(&Dest)[i]=reinterpret_cast<const uInt8*>(&Value)[IntegerSize-i-1];
		}
		return Dest;
	}
};

template<uIntn ElementSize>
struct TKRuntime::TMemory
{

	static void Copy(void *Dest,const void *Src,uIntn Length)noexcept(true){
		::memcpy(Dest,Src,Length*ElementSize);
	}
	static void CopyOverlapped(void *Dest,const void *Src,uIntn Length)noexcept(true){
		::memmove(Dest,Src,Length*ElementSize);
	}
	static void ZeroFill(void *Dest,uIntn Length)noexcept(true){
		::memset(Dest,0,Length*ElementSize);
	}
	static bool Equal(const void *Src1,const void *Src2,uIntn Length)noexcept(true){
		return std::char_traits<char>::compare(static_cast<const char*>(Src1),static_cast<const char*>(Src2),Length*ElementSize)==0;
	}

	static sfInt8 Compare(const void *Src1,const void *Src2,uIntn Length)noexcept(true){
		return static_cast<sfInt8>(::memcmp(Src1,Src2,Length*ElementSize));
	}
};


template<uIntn ElementSize>
struct TKRuntime::TArray : CPPRuntime::TArray<ElementSize>
{
};

template<>
struct TKRuntime::TArray<1> : CPPRuntime::TArray<1>
{
	static void Fill(void *Dest,uIntn Size,uInt8 Src)noexcept(true){
		::memset(Dest,Src,Size);
	}

	static uIntn Search(const void *Src,uIntn Size,uInt8 Data)noexcept(true){
		const void *pResult=::memchr(const_cast<void*>(Src),static_cast<int>(Data),Size);
		return static_cast<const uInt8*>(pResult)-static_cast<const uInt8*>(Src);
	}

};


template<uIntn ElementSize>
struct TKRuntime::TString : CPPRuntime::TString<ElementSize>
{
	typedef typename cnVar::TIntegerOfSize<ElementSize,false>::Type tUInt;

	static uIntn FindLength(const void *String,uIntn MaxLength)noexcept{
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

	//static bool Equal(const void *Str1,uIntn Str1Length,const void *Str2)noexcept(true){
	//	return std::char_traits<tUInt>::compare(static_cast<const tUInt*>(Str1),static_cast<const tUInt*>(Str2),Str1Length+1)==0;
	//}
	//static sfInt8 Compare(const void *Str1,const void *Str2,uIntn MaxLength)noexcept(true){
	//	return static_cast<sfInt8>(std::char_traits<tUInt>::compare(static_cast<const tUInt*>(Str1),static_cast<const tUInt*>(Str2),MaxLength));
	//}

};


template<uIntn ValueSize,ufInt8 Radix>
struct TKRuntime::TStringConvertInteger : CPPRuntime::TStringConvertInteger<ValueSize,Radix>
{
};
template<uIntn ValueSize,ufInt8 Radix>
struct TKRuntime::TStringConvertFloat : CPPRuntime::TStringConvertFloat<ValueSize,Radix>
{
};


template<class T>
struct TKRuntime::TAtomicInteger
{
	typedef std::atomic<T> tAtomic;
	typedef T tVariable;

	static tVariable Get(const tAtomic &a){
		return a;
	}
	static void Set(tAtomic &a,const tVariable &v){
		a=v;
	}

	static tVariable FreeLoad(const tAtomic &a){
		return a.load(std::memory_order::memory_order_relaxed);
	}
	static tVariable AcquireLoad(const tAtomic &a){
		return a.load(std::memory_order::memory_order_acquire);
	}

	static bool WatchEqual(const tAtomic &a,const tVariable &Value,uIntn Count)noexcept{
		for(uIntn i=0;i<Count;i++){
			if(a.load(std::memory_order::memory_order_relaxed)==Value){
				return true;
			}
		}
		return false;
	}
	static bool WatchNotEqual(const tAtomic &a,const tVariable &Value,uIntn Count)noexcept{
		for(uIntn i=0;i<Count;i++){
			if(a.load(std::memory_order::memory_order_relaxed)!=Value){
				return true;
			}
		}
		return false;
	}


	static void FreeStore(tAtomic &a,const tVariable &v){
		a.store(v,std::memory_order::memory_order_relaxed);
	}
	static void ReleaseStore(tAtomic &a,const tVariable &v){
		a.store(v,std::memory_order::memory_order_release);
	}

	static tVariable FreeExchange(tAtomic &a,const tVariable &v){
		return a.exchange(v,std::memory_order::memory_order_relaxed);
	}
	static tVariable AcquireExchange(tAtomic &a,const tVariable &v){
		return a.exchange(v,std::memory_order::memory_order_acquire);
	}
	static tVariable ReleaseExchange(tAtomic &a,const tVariable &v){
		return a.exchange(v,std::memory_order::memory_order_release);
	}
	static tVariable BarrierExchange(tAtomic &a,const tVariable &v){
		return a.exchange(v,std::memory_order::memory_order_acq_rel);
	}

	static bool FreeCompareStore(tAtomic &a,tVariable c,const tVariable &v){
		return a.compare_exchange_strong(c,v,std::memory_order::memory_order_relaxed);
	}
	static bool AcquireCompareStore(tAtomic &a,tVariable c,const tVariable &v){
		return a.compare_exchange_strong(c,v,std::memory_order::memory_order_acquire);
	}
	static bool ReleaseCompareStore(tAtomic &a,tVariable c,const tVariable &v){
		return a.compare_exchange_strong(c,v,std::memory_order::memory_order_release);
	}
	static bool BarrierCompareStore(tAtomic &a,tVariable c,const tVariable &v){
		return a.compare_exchange_strong(c,v,std::memory_order::memory_order_acq_rel);
	}

	static bool FreeCompareExchange(tAtomic &a,tVariable &c,const tVariable &v){
		return a.compare_exchange_strong(c,v,std::memory_order::memory_order_relaxed);
	}
	static bool AcquireCompareExchange(tAtomic &a,tVariable &c,const tVariable &v){
		return a.compare_exchange_strong(c,v,std::memory_order::memory_order_acquire);
	}
	static bool ReleaseCompareExchange(tAtomic &a,tVariable &c,const tVariable &v){
		return a.compare_exchange_strong(c,v,std::memory_order::memory_order_release);
	}
	static bool BarrierCompareExchange(tAtomic &a,tVariable &c,const tVariable &v){
		return a.compare_exchange_strong(c,v,std::memory_order::memory_order_acq_rel);
	}

	static tVariable FreeAdd(tAtomic &a,const tVariable &v){
		return a.fetch_add(v,std::memory_order::memory_order_relaxed);
	}
	static tVariable FreeAddN(tAtomic &a,const tVariable &v){
		return a.fetch_add(v,std::memory_order::memory_order_relaxed)+v;
	}
	static tVariable AcquireAdd(tAtomic &a,const tVariable &v){
		return a.fetch_add(v,std::memory_order::memory_order_acquire);
	}
	static tVariable AcquireAddN(tAtomic &a,const tVariable &v){
		return a.fetch_add(v,std::memory_order::memory_order_acquire)+v;
	}
	static tVariable ReleaseAdd(tAtomic &a,const tVariable &v){
		return a.fetch_add(v,std::memory_order::memory_order_release);
	}
	static tVariable ReleaseAddN(tAtomic &a,const tVariable &v){
		return a.fetch_add(v,std::memory_order::memory_order_release)+v;
	}
	static tVariable BarrierAdd(tAtomic &a,const tVariable &v){
		return a.fetch_add(v,std::memory_order::memory_order_acq_rel);
	}
	static tVariable BarrierAddN(tAtomic &a,const tVariable &v){
		return a.fetch_add(v,std::memory_order::memory_order_acq_rel)+v;
	}

	static tVariable FreeSub(tAtomic &a,const tVariable &v){
		return a.fetch_sub(v,std::memory_order::memory_order_relaxed);
	}
	static tVariable FreeSubN(tAtomic &a,const tVariable &v){
		return a.fetch_sub(v,std::memory_order::memory_order_relaxed)-v;
	}
	static tVariable AcquireSub(tAtomic &a,const tVariable &v){
		return a.fetch_sub(v,std::memory_order::memory_order_acquire);
	}
	static tVariable AcquireSubN(tAtomic &a,const tVariable &v){
		return a.fetch_sub(v,std::memory_order::memory_order_acquire)-v;
	}
	static tVariable ReleaseSub(tAtomic &a,const tVariable &v){
		return a.fetch_sub(v,std::memory_order::memory_order_release);
	}
	static tVariable ReleaseSubN(tAtomic &a,const tVariable &v){
		return a.fetch_sub(v,std::memory_order::memory_order_release)-v;
	}
	static tVariable BarrierSub(tAtomic &a,const tVariable &v){
		return a.fetch_sub(v,std::memory_order::memory_order_acq_rel);
	}
	static tVariable BarrierSubN(tAtomic &a,const tVariable &v){
		return a.fetch_sub(v,std::memory_order::memory_order_acq_rel)-v;
	}
};


template<class TPointer>
struct TKRuntime::TAtomicLink
{
		
	typedef TPointer tPointer;
	typedef std::atomic<TPointer> tLink;

	static tPointer Get(const tLink &Link){		return Link;	}
	static void Set(tLink &Link,tPointer p){	Link=p;	}

	static tPointer AcquireLoad(const tLink &Link){	return Link.load();	}
	static void ReleaseStore(tLink &Link,tPointer p){	Link.store(p);	}

	static tPointer Exchange(tLink &Link,tPointer New){	return static_cast<tPointer>(Link.exchange(New));	}
	
	template<class TSingleLinkItemOperator>
	static tPointer Append(tLink &Link,tPointer First,tPointer Last){

		tPointer CurTop=Link.load();
		do{
			TSingleLinkItemOperator::SetNext(Last,CurTop);
			
		}while(!Link.compare_exchange_weak(CurTop,First));
		return CurTop;
	}

	template<class TSingleLinkItemOperator>
	static tPointer Extract(tLink &Link,tPointer &NextTop){
		tPointer CurTop=Link.load();
		do{
			if(CurTop==nullptr)
				return nullptr;

			NextTop=TSingleLinkItemOperator::GetNext(CurTop);
		}while(!Link.compare_exchange_weak(CurTop,NextTop));
		return CurTop;
	}
};

#ifndef cnLibrary_MANAGED_CRTEXCLUDE

struct TKRuntime::ThreadNotification
{
	std::mutex SignalMutex;
	std::condition_variable Signal;

	void Start(void){
		Notified=false;
	}
	void Finish(void){
	}
	void Wait(void){
		std::unique_lock<std::mutex> lk(SignalMutex);
		while(Notified==false){
			Signal.wait(lk);
		}
	}
	void Notify(void){
		{
			std::unique_lock<std::mutex> lk(SignalMutex);
			Notified=true;
		}
		Signal.notify_one();
	}

	bool Notified;
};

#endif // !cnLibrary_MANAGED_CRTEXCLUDE

template<class TElement>
struct TKRuntime::tVectorOperatorEnumeration
{
	typedef cnVar::TTypePack<
		cnVar::cScalerFloatVectorOperator<TElement>
	> tFloatOperatorPack;

	typedef cnVar::TTypePack<
		cnVar::cScalerIntegerVectorOperator<TElement>
	> tIntegerOperatorPack;
};

//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif


