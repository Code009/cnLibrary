/*- cnTK - Atomic ---------------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2010-12-23                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnTK_Atomic_H__
#define	__cnLibrary_cnTK_Atomic_H__
/*-------------------------------------------------------------------------*/
#include <cnTK/Common.h>
#include <cnTK/TypeTraits.h>
#include <cnTK/Pointer.h>
#include <cnTK/Memory.h>
/*-------------------------------------------------------------------------*/
#if	cnLibrary_CPPFEATURELEVEL >= 1
//---------------------------------------------------------------------------
#include <cnTK/TKMacrosDeclare.inc>
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace TKRuntime{
//---------------------------------------------------------------------------
struct tAtomic;
//{
//
//	static void WatchPause(void);
//};
	
template<class TInteger>
struct tAtomicInteger;
//{
//	typedef tAtomic;
//	typedef TInteger tVariable;
//
//	static TInteger Get(const tAtomic &a)noexcept;
//	static void Set(tAtomic &a,const TInteger &v)noexcept;
//
//	static TInteger FreeLoad(const tAtomic &a)noexcept;
//	static TInteger AcquireLoad(const tAtomic &a)noexcept;
//
//	static void FreeStore(tAtomic &a,const TInteger &v)noexcept;
//	static void ReleaseStore(tAtomic &a,const TInteger &v)noexcept;
//
//	static TInteger FreeExchange(tAtomic &a,const TInteger &v)noexcept;
//	static TInteger AcquireExchange(tAtomic &a,const TInteger &v)noexcept;
//	static TInteger ReleaseExchange(tAtomic &a,const TInteger &v)noexcept;
//	static TInteger BarrierExchange(tAtomic &a,const TInteger &v)noexcept;
//
//	static bool FreeCompareStore(tAtomic &a,const TInteger &c,const TInteger &v)noexcept;
//	static bool AcquireCompareStore(tAtomic &a,const TInteger &c,const TInteger &v)noexcept;
//	static bool ReleaseCompareStore(tAtomic &a,const TInteger &c,const TInteger &v)noexcept;
//	static bool BarrierCompareStore(tAtomic &a,const TInteger &c,const TInteger &v)noexcept;

//	static bool FreeCompareExchange(tAtomic &a,TInteger &c,const TInteger &v)noexcept;
//	static bool AcquireCompareExchange(tAtomic &a,TInteger &c,const TInteger &v)noexcept;
//	static bool ReleaseCompareExchange(tAtomic &a,TInteger &c,const TInteger &v)noexcept;
//	static bool BarrierCompareExchange(tAtomic &a,TInteger &c,const TInteger &v)noexcept;
//
//	static TInteger FreeAdd(tAtomic &a,const TInteger &v)noexcept;
//	static TInteger FreeAddN(tAtomic &a,const TInteger &v)noexcept;
//	static TInteger AcquireAdd(tAtomic &a,const TInteger &v)noexcept;
//	static TInteger AcquireAddN(tAtomic &a,const TInteger &v)noexcept;
//	static TInteger ReleaseAdd(tAtomic &a,const TInteger &v)noexcept;
//	static TInteger ReleaseAddN(tAtomic &a,const TInteger &v)noexcept;
//	static TInteger BarrierAdd(tAtomic &a,const TInteger &v)noexcept;
//	static TInteger BarrierAddN(tAtomic &a,const TInteger &v)noexcept;
//
//	static TInteger FreeSub(tAtomic &a,const TInteger &v)noexcept;
//	static TInteger FreeSubN(tAtomic &a,const TInteger &v)noexcept;
//	static TInteger AcquireSub(tAtomic &a,const TInteger &v)noexcept;
//	static TInteger AcquireSubN(tAtomic &a,const TInteger &v)noexcept;
//	static TInteger ReleaseSub(tAtomic &a,const TInteger &v)noexcept;
//	static TInteger ReleaseSubN(tAtomic &a,const TInteger &v)noexcept;
//	static TInteger BarrierSub(tAtomic &a,const TInteger &v)noexcept;
//	static TInteger BarrierSubN(tAtomic &a,const TInteger &v)noexcept;
//};

template<class TPointer>
struct tAtomicLink;
//{
//	typedef tPointer;
//	typedef tLink;
//
//	static tPointer Get(const tLink &Link)noexcept;
//	static void Set(tLink &Link,tPointer p)noexcept;
//
//	static tPointer AcquireLoad(const tLink &Link)noexcept;
//	static void ReleaseStore(tLink &Link,tPointer p)noexcept;
//
//	static tPointer Exchange(tLink &Link,tPointer New)noexcept;
//
//	template<class TSingleLinkedItemOperator>
//	static tPointer Append(tLink &Link,tPointer First,tPointer Last)noexcept;
//	template<class TSingleLinkedItemOperator>
//	static tPointer Extract(tLink &Link,tPointer &TopNext)noexcept;
//};
//---------------------------------------------------------------------------
}	// namespace TKRuntime
//---------------------------------------------------------------------------
namespace cnAsync{

template<class TVariable,class TIntegerAtomicOperator>
struct TAtomicVariableOperatorByInteger
{
	typedef TVariable tVariable;
	typedef typename TIntegerAtomicOperator::tAtomic tAtomic;
	typedef typename TIntegerAtomicOperator::tVariable tInteger;

	
#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

	template<class T>
	static tVariable CastReturn(T&& Src)noexcept(true){
		return reinterpret_cast<tVariable&&>(Src);
	}

#else	// cnLibrary_CPPFEATURE_RVALUE_REFERENCES <= 200610L

	template<class T>
	static const tVariable& CastReturn(const T &Src)noexcept(true){
		return reinterpret_cast<const tVariable&>(Src);
	}

#endif	// cnLibrary_CPPFEATURE_RVALUE_REFERENCES


	static tVariable Get(const tAtomic &av)noexcept(true){
		return CastReturn( TIntegerAtomicOperator::Get(av) );
	}
	static void Set(tAtomic &av,const tVariable &v)noexcept(true){
		return TIntegerAtomicOperator::Set(av,reinterpret_cast<const tInteger&>(v));
	}

	static tVariable FreeLoad(const tAtomic &av)noexcept(true){
		return CastReturn( TIntegerAtomicOperator::FreeLoad(av) );
	}
	static tVariable AcquireLoad(const tAtomic &av)noexcept(true){
		return CastReturn( TIntegerAtomicOperator::AcquireLoad(av) );
	}
	static void FreeStore(tAtomic &av,const tVariable &v)noexcept(true){
		return TIntegerAtomicOperator::FreeStore(av,reinterpret_cast<const tInteger&>(v));
	}
	static void ReleaseStore(tAtomic &av,const tVariable &v)noexcept(true){
		return TIntegerAtomicOperator::ReleaseStore(av,reinterpret_cast<const tInteger&>(v));
	}
	
	static tVariable FreeExchange(tAtomic &av,const tVariable &v)noexcept(true){
		return CastReturn( TIntegerAtomicOperator::FreeExchange(av,reinterpret_cast<const tInteger&>(v)) );
	}
	static tVariable AcquireExchange(tAtomic &av,const tVariable &v)noexcept(true){
		return CastReturn( TIntegerAtomicOperator::AcquireExchange(av,reinterpret_cast<const tInteger&>(v)) );
	}
	static tVariable ReleaseExchange(tAtomic &av,const tVariable &v)noexcept(true){
		return CastReturn( TIntegerAtomicOperator::ReleaseExchange(av,reinterpret_cast<const tInteger&>(v)) );
	}
	static tVariable BarrierExchange(tAtomic &av,const tVariable &v)noexcept(true){
		return CastReturn( TIntegerAtomicOperator::BarrierExchange(av,reinterpret_cast<const tInteger&>(v)) );
	}

	static bool FreeCompareStore(tAtomic &av,const tVariable &c,const tVariable &v)noexcept(true){
		return TIntegerAtomicOperator::FreeCompareStore(av,reinterpret_cast<const tInteger&>(c),reinterpret_cast<const tInteger&>(v));
	}
	static bool AcquireCompareStore(tAtomic &av,const tVariable &c,const tVariable &v)noexcept(true){
		return TIntegerAtomicOperator::AcquireCompareStore(av,reinterpret_cast<const tInteger&>(c),reinterpret_cast<const tInteger&>(v));
	}
	static bool ReleaseCompareStore(tAtomic &av,const tVariable &c,const tVariable &v)noexcept(true){
		return TIntegerAtomicOperator::ReleaseCompareStore(av,reinterpret_cast<const tInteger&>(c),reinterpret_cast<const tInteger&>(v));
	}
	static bool BarrierCompareStore(tAtomic &av,const tVariable &c,const tVariable &v)noexcept(true){
		return TIntegerAtomicOperator::BarrierCompareStore(av,reinterpret_cast<const tInteger&>(c),reinterpret_cast<const tInteger&>(v));
	}

	static bool FreeCompareExchange(tAtomic &av,tVariable &c,const tVariable &v)noexcept(true){
		return TIntegerAtomicOperator::FreeCompareExchange(av,reinterpret_cast<tInteger&>(c),reinterpret_cast<const tInteger&>(v));
	}
	static bool AcquireCompareExchange(tAtomic &av,tVariable &c,const tVariable &v)noexcept(true){
		return TIntegerAtomicOperator::AcquireCompareExchange(av,reinterpret_cast<tInteger&>(c),reinterpret_cast<const tInteger&>(v));
	}
	static bool ReleaseCompareExchange(tAtomic &av,tVariable &c,const tVariable &v)noexcept(true){
		return TIntegerAtomicOperator::ReleaseCompareExchange(av,reinterpret_cast<tInteger&>(c),reinterpret_cast<const tInteger&>(v));
	}
	static bool BarrierCompareExchange(tAtomic &av,tVariable &c,const tVariable &v)noexcept(true){
		return TIntegerAtomicOperator::BarrierCompareExchange(av,reinterpret_cast<tInteger&>(c),reinterpret_cast<const tInteger&>(v));
	}

	static tVariable FreeAdd(tAtomic &av,const tVariable &v)noexcept(true){
		return CastReturn( TIntegerAtomicOperator::FreeAdd(av,reinterpret_cast<const tInteger&>(v)) );
	}
	static tVariable FreeAddN(tAtomic &av,const tVariable &v)noexcept(true){
		return CastReturn( TIntegerAtomicOperator::FreeAddN(av,reinterpret_cast<const tInteger&>(v)) );
	}
	static tVariable AcquireAdd(tAtomic &av,const tVariable &v)noexcept(true){
		return CastReturn( TIntegerAtomicOperator::AcquireAdd(av,reinterpret_cast<const tInteger&>(v)) );
	}
	static tVariable AcquireAddN(tAtomic &av,const tVariable &v)noexcept(true){
		return CastReturn( TIntegerAtomicOperator::AcquireAddN(av,reinterpret_cast<const tInteger&>(v)) );
	}
	static tVariable ReleaseAdd(tAtomic &av,const tVariable &v)noexcept(true){
		return CastReturn( TIntegerAtomicOperator::ReleaseAdd(av,reinterpret_cast<const tInteger&>(v)) );
	}
	static tVariable ReleaseAddN(tAtomic &av,const tVariable &v)noexcept(true){
		return CastReturn( TIntegerAtomicOperator::ReleaseAddN(av,reinterpret_cast<const tInteger&>(v)) );
	}
	static tVariable BarrierAdd(tAtomic &av,const tVariable &v)noexcept(true){
		return CastReturn( TIntegerAtomicOperator::BarrierAdd(av,reinterpret_cast<const tInteger&>(v)) );
	}
	static tVariable BarrierAddN(tAtomic &av,const tVariable &v)noexcept(true){
		return CastReturn( TIntegerAtomicOperator::BarrierAddN(av,reinterpret_cast<const tInteger&>(v)) );
	}

	static tVariable FreeSub(tAtomic &av,const tVariable &v)noexcept(true){
		return CastReturn( TIntegerAtomicOperator::FreeSub(av,reinterpret_cast<const tInteger&>(v)) );
	}
	static tVariable FreeSubN(tAtomic &av,const tVariable &v)noexcept(true){
		return CastReturn( TIntegerAtomicOperator::FreeSubN(av,reinterpret_cast<const tInteger&>(v)) );
	}
	static tVariable AcquireSub(tAtomic &av,const tVariable &v)noexcept(true){
		return CastReturn( TIntegerAtomicOperator::AcquireSub(av,reinterpret_cast<const tInteger&>(v)) );
	}
	static tVariable AcquireSubN(tAtomic &av,const tVariable &v)noexcept(true){
		return CastReturn( TIntegerAtomicOperator::AcquireSubN(av,reinterpret_cast<const tInteger&>(v)) );
	}
	static tVariable ReleaseSub(tAtomic &av,const tVariable &v)noexcept(true){
		return CastReturn( TIntegerAtomicOperator::ReleaseSub(av,reinterpret_cast<const tInteger&>(v)) );
	}
	static tVariable ReleaseSubN(tAtomic &av,const tVariable &v)noexcept(true){
		return CastReturn( TIntegerAtomicOperator::ReleaseSubN(av,reinterpret_cast<const tInteger&>(v)) );
	}
	static tVariable BarrierSub(tAtomic &av,const tVariable &v)noexcept(true){
		return CastReturn( TIntegerAtomicOperator::BarrierSub(av,reinterpret_cast<const tInteger&>(v)) );
	}
	static tVariable BarrierSubN(tAtomic &av,const tVariable &v)noexcept(true){
		return CastReturn( TIntegerAtomicOperator::BarrierSubN(av,reinterpret_cast<const tInteger&>(v)) );
	}
};

template<class TVariable>
union cAtomicVariable
{
	typedef typename typename cnVar::TIntegerOfSize<sizeof(TVariable),false>::Type tInteger;
	typedef typename TKRuntime::template tAtomicInteger<tInteger> tAtomicIntegerOperator;
	typedef typename tAtomicIntegerOperator::tAtomic tAtomic;
	typedef TVariable tVariable;
	typedef typename cnVar::TSelect<
		cnVar::TIsSame<tInteger,tVariable>::Value
		, TAtomicVariableOperatorByInteger<TVariable,tAtomicIntegerOperator>
		, tAtomicIntegerOperator
	>::Type tAtomicOperator;
	

	tAtomic _atomic_;

	cnLib_CONSTEXPR_FUNC cAtomicVariable()noexcept(true){}
#ifndef cnLibrary_CPPEXCLUDE_CLASS_MEMBER_DEFAULT
	~cAtomicVariable()=default;
#endif

	cnLib_CONSTEXPR_FUNC cAtomicVariable(const tVariable &Value)noexcept(true)
		: _atomic_((tInteger)Value){}

	cAtomicVariable(const cAtomicVariable &Src)noexcept(true){	tAtomicOperator::Set(_atomic_,tAtomicOperator::Get(Src._atomic_));	}

	// operator tVariable
	//	get current value non-atomicly
	operator tVariable ()const noexcept(true){	return tAtomicOperator::Get(_atomic_);	}

	// Get
	//	get current value non-atomicly
	tVariable Get(void)const noexcept(true){	return tAtomicOperator::Get(_atomic_);	}


	// operator =
	//	assign value non-atomicly
	void operator = (const tVariable &Value)noexcept(true){	tAtomicOperator::Set(_atomic_,Value);	}
	// Set
	//	assign value non-atomicly
	void Set(const tVariable &Value)noexcept(true){			tAtomicOperator::Set(_atomic_,Value);	}


	bool WatchUntilEqual(const tVariable &Value,uIntn Count)noexcept(true){
		for(uIntn i=0;i<Count;i++){
			if(tAtomicOperator::FreeLoad(_atomic_)==Value){
				return true;
			}
			TKRuntime::tAtomic::WatchPause();
		}
		return false;
	}

	bool WatchUntilNotEqual(const tVariable &Value,uIntn Count)noexcept(true){
		for(uIntn i=0;i<Count;i++){
			if(tAtomicOperator::FreeLoad(_atomic_)!=Value){
				return true;
			}
			TKRuntime::tAtomic::WatchPause();
		}
		return false;
	}

	struct Free
	{
		//	free ordering
		tAtomic _atomic_;


		// operator tVariable
		//	get current value atomicly
		operator tVariable ()const noexcept(true){	return tAtomicOperator::FreeLoad(_atomic_);	}
		// Load
		//	get current value atomicly.
		tVariable Load(void)const noexcept(true){	return tAtomicOperator::FreeLoad(_atomic_);	}
		// operator =
		//	set current value atomicly
		void operator = (const tVariable &Value)noexcept(true){	tAtomicOperator::FreeStore(_atomic_,Value);	}
		// Store
		//	set current value atomicly
		void Store(tVariable Value)noexcept(true){				tAtomicOperator::FreeStore(_atomic_,Value);	}

		// Xchg
		//	load current value and store new value
		// [in]Value		new value to store
		// return previous value
		tVariable Xchg(const tVariable &Value)noexcept(true){			return tAtomicOperator::FreeExchange(_atomic_,Value);	}
		// operator <<=
		//	load current value and store new value
		// [in]Value		new value to store
		// return previous value
		tVariable operator <<= (const tVariable &Value)noexcept(true){	return tAtomicOperator::FreeExchange(_atomic_,Value);	}

		// XchgAdd
		//	load current value and add value to variable atomicly
		// [in]Value		addend
		// return previous value before addition
		tVariable XchgAdd(tVariable Value)noexcept(true){			return tAtomicOperator::FreeAdd(_atomic_,Value);	}
		// operator +=
		//	add value to variable atomicly
		// [in]Value		addend
		// return value after addition
		tVariable operator += (const tVariable &Value)noexcept(true){	return tAtomicOperator::FreeAddN(_atomic_,Value);	}
		// prefix operator ++
		//	add 1 to variable atomicly
		// return value after addition
		tVariable operator ++()noexcept(true){		return tAtomicOperator::FreeAddN(_atomic_,1);	}
		// suffix operator ++
		//	load current value and add 1 to variable atomicly
		// return previous value before addition
		tVariable operator ++(int)noexcept(true){		return tAtomicOperator::FreeAdd(_atomic_,1);	}

		// XchgSub
		//	load current value and subtract value to variable atomicly
		// [in]Value		subtrahend
		// return previous value before subtraction
		tVariable XchgSub(tVariable Value)noexcept(true){			return tAtomicOperator::FreeSub(_atomic_,Value);	}
		// operator -=
		//	subtract value to variable atomicly
		// [in]Value		subtrahend
		// return value after subtraction
		tVariable operator -= (const tVariable &Value)noexcept(true){	return tAtomicOperator::FreeSubN(_atomic_,Value);	}
		// prefix operator --
		//	subtract 1 to variable atomicly
		// return value after subtraction
		tVariable operator --()noexcept(true){		return tAtomicOperator::FreeSubN(_atomic_,1);	}
		// suffix operator --
		//	load current value and subtract 1 to variable atomicly
		// return previous value before subtraction
		tVariable operator --(int)noexcept(true){		return tAtomicOperator::FreeSub(_atomic_,1);	}

		// CmpStore
		//	compare current value to Compare, if match, store Src to variable. Load, compare and store will execute atomicly.
		// [in]Src		value to store
		// [in]Compare	value to compare
		// return true if it assigned Src to variable.
		bool CmpStore(const tVariable &Compare,const tVariable &Src)noexcept(true){	return tAtomicOperator::FreeCompareStore(_atomic_,Compare,Src);	}


		// CmpXchg
		//	compare current value to Compare, if match, assign Src to variable.  Load, compare and store will execute atomicly.
		// [in]Src			value to assign
		// [in]Compare		value to compare
		// [out]Compare		previous value in variable
		// return true if it assigned Src to variable.
		bool CmpXchg(tVariable &Compare,const tVariable &Src)noexcept(true){	return tAtomicOperator::FreeCompareExchange(_atomic_,Compare,Src);	}

		struct CmpXchgOp{	tAtomic &a;	tVariable &c;
			// operator <<=
			//	compare current value to c, if match, assign Src to variable.  Load, compare and store will execute atomicly.
			// [in]Value		value to assign
			// [in]c			value to compare
			// [out]c			previous value in variable
			// return compare operator
			bool operator <<=(const tVariable &Value)noexcept(true){	return tAtomicOperator::FreeCompareExchange(a,c,Value);	}
		};
		// operator[]
		//	define compare value for compare and exchange operation
		// Compare		reference to compare value
		// return compare operator
		CmpXchgOp operator[](tVariable &Compare)noexcept(true){		CmpXchgOp r={_atomic_,Compare};	return r;	}
	}Free;
	struct Acquire
	{
		//	acquire ordering on load operation. free ordering on store operation
		tAtomic _atomic_;

		// operator tVariable
		//	get current value atomicly
		tVariable Load(void)const noexcept(true){	return tAtomicOperator::AcquireLoad(_atomic_);	}
		// Load
		//	get current value atomicly.
		operator tVariable ()const noexcept(true){	return tAtomicOperator::AcquireLoad(_atomic_);	}

		// Xchg
		//	load current value and store new value
		// [in]Value		new value to store
		// return previous value
		tVariable Xchg(const tVariable &Value)noexcept(true){				return tAtomicOperator::AcquireExchange(_atomic_,Value);	}
		// operator <<=
		//	load current value and store new value
		// [in]Value		new value to store
		// return previous value
		tVariable operator <<= (const tVariable &Value)noexcept(true){	return tAtomicOperator::AcquireExchange(_atomic_,Value);	}

		// XchgAdd
		//	load current value and add value to variable atomicly
		// [in]Value		addend
		// return previous value before addition
		tVariable XchgAdd(tVariable Value)noexcept(true){			return tAtomicOperator::AcquireAdd(_atomic_,Value);	}
		// operator +=
		//	add value to variable atomicly
		// [in]Value		addend
		// return value after addition
		tVariable operator += (const tVariable &Value)noexcept(true){	return tAtomicOperator::AcquireAddN(_atomic_,Value);	}
		// prefix operator ++
		//	add 1 to variable atomicly
		// return value after addition
		tVariable operator ++()noexcept(true){		return tAtomicOperator::AcquireAddN(_atomic_,1);	}
		// suffix operator ++
		//	load current value and add 1 to variable atomicly
		// return previous value before addition
		tVariable operator ++(int)noexcept(true){		return tAtomicOperator::AcquireAdd(_atomic_,1);	}

		// XchgSub
		//	load current value and subtract value to variable atomicly
		// [in]Value		subtrahend
		// return previous value before subtraction
		tVariable XchgSub(tVariable Value)noexcept(true){			return tAtomicOperator::AcquireSub(_atomic_,Value);	}
		// operator -=
		//	subtract value to variable atomicly
		// [in]Value		subtrahend
		// return value after subtraction
		tVariable operator -= (const tVariable &Value)noexcept(true){	return tAtomicOperator::AcquireSubN(_atomic_,Value);	}
		// prefix operator --
		//	subtract 1 to variable atomicly
		// return value after subtraction
		tVariable operator --()noexcept(true){		return tAtomicOperator::AcquireSubN(_atomic_,1);	}
		// suffix operator --
		//	load current value and subtract 1 to variable atomicly
		// return previous value before subtraction
		tVariable operator --(int)noexcept(true){		return tAtomicOperator::AcquireSub(_atomic_,1);	}

		// CmpStore
		//	compare current value to Compare, if match, store Src to variable. Load, compare and store will execute atomicly.
		// [in]Src		value to store
		// [in]Compare	value to compare
		// return true if it assigned Src to variable.
		bool CmpStore(const tVariable &Compare,const tVariable &Src)noexcept(true){	return tAtomicOperator::AcquireCompareStore(_atomic_,Compare,Src);	}

		// CmpXchg
		//	compare current value to Compare, if match, assign Src to variable. Load, compare and store will execute atomicly.
		// [in]Src			value to assign
		// [in]Compare		value to compare
		// [out]Compare		previous value in variable
		// return true if it assigned Src to variable.
		bool CmpXchg(tVariable &Compare,const tVariable &Src)noexcept(true){	return tAtomicOperator::AcquireCompareExchange(_atomic_,Compare,Src);	}

		struct CmpXchgOp{	tAtomic &a;	tVariable &c;
			// operator <<=
			//	compare current value to c, if match, assign Src to variable.  Load, compare and store will execute atomicly.
			// [in]Value		value to assign
			// [in]c			value to compare
			// [out]c			previous value in variable
			// return compare operator
			bool operator <<=(const tVariable &Value)noexcept(true){	return tAtomicOperator::AcquireCompareExchange(a,c,Value);	}
		};
		// operator[]
		//	define compare value for compare and exchange operation
		// Compare		reference to compare value
		// return compare operator
		CmpXchgOp operator[](tVariable &Compare)noexcept(true){		CmpXchgOp r={_atomic_,Compare};	return r;	}

	}Acquire;
	struct Release
	{
		//	free ordering on load operation. release ordering on store operation
		tAtomic _atomic_;

		// operator =
		//	set current value atomicly
		void Store(tVariable Value)noexcept(true){				tAtomicOperator::ReleaseStore(_atomic_,Value);	}
		// Store
		//	set current value atomicly
		void operator = (const tVariable &Value)noexcept(true){	tAtomicOperator::ReleaseStore(_atomic_,Value);	}

		// Xchg
		//	load current value and store new value
		// [in]Value		new value to store
		// return previous value
		tVariable Xchg(tVariable Value)noexcept(true){			return tAtomicOperator::ReleaseExchange(_atomic_,Value);	}
		// operator <<=
		//	load current value and store new value
		// [in]Value		new value to store
		// return previous value
		tVariable operator <<= (const tVariable &Value)noexcept(true){	return tAtomicOperator::ReleaseExchange(_atomic_,Value);	}

		// XchgAdd
		//	load current value and add value to variable atomicly
		// [in]Value		addend
		// return previous value before addition
		tVariable XchgAdd(tVariable Value)noexcept(true){			return tAtomicOperator::ReleaseAdd(_atomic_,Value);	}
		// operator +=
		//	add value to variable atomicly
		// [in]Value		addend
		// return value after addition
		tVariable operator += (const tVariable &Value)noexcept(true){	return tAtomicOperator::ReleaseAddN(_atomic_,Value);	}
		// prefix operator ++
		//	add 1 to variable atomicly
		// return value after addition
		tVariable operator ++()noexcept(true){		return tAtomicOperator::ReleaseAddN(_atomic_,1);	}
		// suffix operator ++
		//	load current value and add 1 to variable atomicly
		// return previous value before addition
		tVariable operator ++(int)noexcept(true){		return tAtomicOperator::ReleaseAdd(_atomic_,1);	}

		// XchgSub
		//	load current value and subtract value to variable atomicly
		// [in]Value		subtrahend
		// return previous value before subtraction
		tVariable XchgSub(tVariable Value)noexcept(true){			return tAtomicOperator::ReleaseSub(_atomic_,Value);	}
		// operator -=
		//	subtract value to variable atomicly
		// [in]Value		subtrahend
		// return value after subtraction
		tVariable operator -= (const tVariable &Value)noexcept(true){	return tAtomicOperator::AcquireSubN(_atomic_,Value);	}
		// prefix operator --
		//	subtract 1 to variable atomicly
		// return value after subtraction
		tVariable operator --()noexcept(true){		return tAtomicOperator::ReleaseSubN(_atomic_,1);	}
		// suffix operator --
		//	load current value and subtract 1 to variable atomicly
		// return previous value before subtraction
		tVariable operator --(int)noexcept(true){		return tAtomicOperator::ReleaseSub(_atomic_,1);	}

		// CmpStore
		//	compare current value to Compare, if match, store Src to variable. Load, compare and store will execute atomicly.
		// [in]Src		value to store
		// [in]Compare	value to compare
		// return true if it assigned Src to variable.
		bool CmpStore(const tVariable &Compare,const tVariable &Src)noexcept(true){	return tAtomicOperator::ReleaseCompareStore(_atomic_,Compare,Src);	}

		// CmpXchg
		//	compare current value to Compare, if match, assign Src to variable.  Load, compare and store will execute atomicly.
		// [in]Src			value to assign
		// [in]Compare		value to compare
		// [out]Compare		previous value in variable
		// return true if it assigned Src to variable.
		bool CmpXchg(tVariable &Compare,const tVariable &Src)noexcept(true){	return tAtomicOperator::ReleaseCompareExchange(_atomic_,Compare,Src);	}

		struct CmpXchgOp{	tAtomic &a;	tVariable &c;
			// operator <<=
			//	compare current value to c, if match, assign Src to variable.  Load, compare and store will execute atomicly.
			// [in]Value		value to assign
			// [in]c			value to compare
			// [out]c			previous value in variable
			// return compare operator
			bool operator <<=(const tVariable &Value)noexcept(true){	return tAtomicOperator::ReleaseCompareExchange(a,c,Value);	}
		};
		// operator[]
		//	define compare value for compare and exchange operation
		// Compare		reference to compare value
		// return compare operator
		CmpXchgOp operator[](tVariable &Compare)noexcept(true){		CmpXchgOp r={_atomic_,Compare};	return r;	}

	}Release;
	struct Barrier
	{
		//	acquire ordering on load operation. release ordering on store operation
		tAtomic _atomic_;

		// Xchg
		//	load current value and store new value
		// [in]Value		new value to store
		// return previous value
		tVariable Xchg(const tVariable &Value)noexcept(true){			return tAtomicOperator::BarrierExchange(_atomic_,Value);	}
		// operator <<=
		//	load current value and store new value
		// [in]Value		new value to store
		// return previous value
		tVariable operator <<= (const tVariable &Value)noexcept(true){	return tAtomicOperator::BarrierExchange(_atomic_,Value);	}

		// XchgAdd
		//	load current value and add value to variable atomicly
		// [in]Value		addend
		// return previous value before addition
		tVariable XchgAdd(tVariable Value)noexcept(true){			return tAtomicOperator::BarrierAdd(_atomic_,Value);	}
		// operator +=
		//	add value to variable atomicly
		// [in]Value		addend
		// return value after addition
		tVariable operator += (const tVariable &Value)noexcept(true){	return tAtomicOperator::BarrierAddN(_atomic_,Value);	}
		// prefix operator ++
		//	add 1 to variable atomicly
		// return value after addition
		tVariable operator ++()noexcept(true){		return tAtomicOperator::BarrierAddN(_atomic_,1);	}
		// suffix operator ++
		//	load current value and add 1 to variable atomicly
		// return previous value before addition
		tVariable operator ++(int)noexcept(true){		return tAtomicOperator::BarrierAdd(_atomic_,1);	}

		// XchgSub
		//	load current value and subtract value to variable atomicly
		// [in]Value		subtrahend
		// return previous value before subtraction
		tVariable XchgSub(tVariable Value)noexcept(true){			return tAtomicOperator::BarrierSub(_atomic_,Value);	}
		// operator -=
		//	subtract value to variable atomicly
		// [in]Value		subtrahend
		// return value after subtraction
		tVariable operator -= (const tVariable &Value)noexcept(true){	return tAtomicOperator::AcquireSubN(_atomic_,Value);	}
		// prefix operator --
		//	subtract 1 to variable atomicly
		// return value after subtraction
		tVariable operator --()noexcept(true){		return tAtomicOperator::BarrierSubN(_atomic_,1);	}
		// suffix operator --
		//	load current value and subtract 1 to variable atomicly
		// return previous value before subtraction
		tVariable operator --(int)noexcept(true){		return tAtomicOperator::BarrierSub(_atomic_,1);	}

		// CmpStore
		//	compare current value to Compare, if match, store Src to variable. Load, compare and store will execute atomicly.
		// [in]Compare	value to compare
		// [in]Src		value to store
		// return true if it assigned Src to variable.
		bool CmpStore(const tVariable &Compare,const tVariable &Src)noexcept(true){	return tAtomicOperator::BarrierCompareStore(_atomic_,Compare,Src);	}

		// CmpXchg
		//	compare current value to Compare, if match, assign Src to variable.  Load, compare and store will execute atomicly.
		// [in]Compare		value to compare
		// [in]Src			value to assign
		// [out]Compare		previous value in variable
		// return true if it assigned Src to variable.
		bool CmpXchg(tVariable &Compare,const tVariable &Src)noexcept(true){	return tAtomicOperator::BarrierCompareExchange(_atomic_,Compare,Src);	}

		struct CmpXchgOp{	tAtomic &a;	tVariable &c;
			// operator <<=
			//	compare current value to c, if match, assign Src to variable.  Load, compare and store will execute atomicly.
			// [in]Value		value to assign
			// [in]c			value to compare
			// [out]c			previous value in variable
			// return compare operator
			bool operator <<=(const tVariable &Value)noexcept(true){	return tAtomicOperator::BarrierCompareExchange(a,c,Value);	}
		};
		// operator[]
		//	define compare value for compare and exchange operation
		// Compare		reference to compare value
		// return compare operator
		CmpXchgOp operator[](tVariable &Compare)noexcept(true){		CmpXchgOp r={_atomic_,Compare};	return r;	}

	}Barrier;
};

//---------------------------------------------------------------------------
}	// namespace cnAsync
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnVar{

//---------------------------------------------------------------------------
template<class TRefTokenOperator>
class cAtomicPtrReference
{
public:
	typedef typename TRefTokenOperator::tPtr tPtr;
	typedef typename TRefTokenOperator::tRefToken tRefToken;

	cAtomicPtrReference()noexcept(true):fAtomicPtr(nullptr){}
	~cAtomicPtrReference()noexcept(true){
		tRefToken RefToken=TRefTokenOperator::TokenFrom(fAtomicPtr.Get());
		TRefTokenOperator::Release(RefToken);
	}

	tPtr Peek(void)const noexcept(true){	return fAtomicPtr.Get();	}

	// <<= xchg

	cPtrReference<TRefTokenOperator> operator <<=(tPtr Pointer)noexcept(true){
		tRefToken RefToken=TRefTokenOperator::TokenFrom(Pointer);
		TRefTokenOperator::Acquire(RefToken);
		// swap
		tPtr PrevPointer=fAtomicPtr.Barrier.Xchg(Pointer);
		return cPtrReference<TRefTokenOperator>::TakeFromManual(PrevPointer);
	}

	struct CmpOp{	cnAsync::cAtomicVariable<tPtr> &fAtomicPtr;	tPtr ComparePointer;
		bool operator <<=(tPtr Pointer)noexcept(true){
			if(Pointer!=nullptr){
				tRefToken RefToken=TRefTokenOperator::TokenFrom(Pointer);
				TRefTokenOperator::Acquire(RefToken);
				if(fAtomicPtr.Barrier.CmpStore(ComparePointer,Pointer)==false){
					TRefTokenOperator::Release(RefToken);
					return false;
				}
			}
			else{
				if(fAtomicPtr.Barrier.CmpStore(ComparePointer,nullptr)==false){
					return false;
				}
			}
			tRefToken OldRefToken=TRefTokenOperator::TokenFrom(ComparePointer);
			TRefTokenOperator::Release(OldRefToken);
			return true;
		}
	};
	CmpOp operator[](tPtr Compare)noexcept(true){		return CmpOp{fAtomicPtr,Compare};	}

	// ^=  set if nullptr

	bool operator ^=(tPtr Pointer)noexcept(true){
		if(Pointer==nullptr){
			return fAtomicPtr==nullptr;
		}

		tRefToken RefToken=TRefTokenOperator::TokenFromPointer(Pointer);
		TRefTokenOperator::Acquire(RefToken);
		if(fAtomicPtr.Barrier.CmpStore(nullptr,Pointer)){
			return true;
		}
		TRefTokenOperator::Release(RefToken);
		return false;
	}
#ifndef cnLibrary_CPPEXCLUDE_CLASS_MEMBER_DELETE

	cAtomicPtrReference(const cAtomicPtrReference &)=delete;
	cAtomicPtrReference& operator =(const cAtomicPtrReference &Src)=delete;


#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L
	cAtomicPtrReference(cAtomicPtrReference &&)=delete;
	cAtomicPtrReference& operator =(cAtomicPtrReference &&Src)=delete;
#endif // cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

// !cnLibrary_CPPEXCLUDE_CLASS_MEMBER_DELETE
#else
// cnLibrary_CPPEXCLUDE_CLASS_MEMBER_DELETE
private:
	inline cAtomicPtrReference(const cAtomicPtrReference &);
	inline cAtomicPtrReference& operator =(const cAtomicPtrReference &Src);


#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L
	inline cAtomicPtrReference(cAtomicPtrReference &&);
	inline cAtomicPtrReference& operator =(cAtomicPtrReference &&Src);
#endif // cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

#endif // cnLibrary_CPPEXCLUDE_CLASS_MEMBER_DELETE
protected:
	cnAsync::cAtomicVariable<tPtr> fAtomicPtr;
};
//---------------------------------------------------------------------------
//TRegTokenOperator
//{
//	typedef tPtr;
//	typedef tRef;	// optional
//	typedef tRegToken;
//	class tNotifyToken{
//		virtual bool InvalidationNotify(tRegToken Token)noexcept=0;
//	};
//
//	static tRegToken Register(tPtr Pointer,tNotifyToken *NotifyToken)noexcept;
//	static void Unregister(tRegToken RegToken,tNotifyToken *NotifyToken)noexcept;
//	static tRef Reference(tRegToken RegToken)noexcept;	// optional: tRef
//
//};
//---------------------------------------------------------------------------
template<class TRegTokenOperator>
class cPtrWeakReference : protected TRegTokenOperator::tNotifyToken
{
public:
	typedef typename TRegTokenOperator::tRegToken tRegToken;

	cPtrWeakReference()noexcept(true)
		: fRegToken(nullptr){}
	~cPtrWeakReference()noexcept(true){
		tRegToken CurRegToken=fRegToken.Acquire<<=reinterpret_cast<tRegToken>(this);
		if(CurRegToken!=nullptr && this!=reinterpret_cast<cPtrWeakReference*>(CurRegToken)){
			TRegTokenOperator::Unregister(CurRegToken,this);
		}
	}

#ifndef cnLibrary_CPPEXCLUDE_FUNCTION_TEMPLATE_DEFALT_ARGUMENT
	template<class T
#if cnLibrary_CPPFEATURE_DECLTYPE >= 200707L
		, class=decltype(TRegTokenOperator::Register(DeclVar<T cnLib_UREF>(),nullptr))
#endif
	>
	cPtrWeakReference(T cnLib_UREF Src)noexcept(true)
		: fRegToken(TRegTokenOperator::Register(cnLib_UREFCAST(T)(Src),this)){}
#else	// cnLibrary_CPPEXCLUDE_FUNCTION_TEMPLATE_DEFALT_ARGUMENT
	template<class T>
	cPtrWeakReference(T cnLib_UREF Src)noexcept(true)
		: fRegToken(TRegTokenOperator::Register(cnLib_UREFCAST(T)(Src),this)){}
#endif	// cnLibrary_CPPEXCLUDE_FUNCTION_TEMPLATE_DEFALT_ARGUMENT

	cPtrWeakReference& operator =(cPtrWeakReference &Src)noexcept(true){
		if(this==&Src)
			return *this;

		tRegToken OldRegToken=fRegToken.Acquire<<=reinterpret_cast<tRegToken>(this);
		if(this==reinterpret_cast<cPtrWeakReference*>(OldRegToken))
			return *this;
	
		if(OldRegToken!=nullptr)
			TRegTokenOperator::Unregister(OldRegToken,this);
	
		fRegToken.Release.Store(TRegTokenOperator::Register(Src.Ref(),this));
	
		return *this;
	}

	template<class T>
	typename TSelect<0,cPtrWeakReference&
#if cnLibrary_CPPFEATURE_DECLTYPE >= 200707L
		, decltype(TRegTokenOperator::Register(DeclVar<T cnLib_UREF>(),nullptr))
#endif
	>::Type operator =(T cnLib_UREF Src)noexcept(true){
		tRegToken OldRegToken=fRegToken.Acquire<<=reinterpret_cast<tRegToken>(this);
		if(this==reinterpret_cast<cPtrWeakReference*>(OldRegToken))
			return *this;
	
		if(OldRegToken!=nullptr)
			TRegTokenOperator::Unregister(OldRegToken,this);
	
		fRegToken.Release.Store(TRegTokenOperator::Register(Src,this));
	
		return *this;
	}

	typename TRegTokenOperator::tRef Ref(void)noexcept(true){
		tRegToken CurRegToken=fRegToken.Acquire<<=reinterpret_cast<tRegToken>(this);
		if(this==reinterpret_cast<cPtrWeakReference*>(CurRegToken))
			return nullptr;

		if(CurRegToken==nullptr){
			fRegToken.Release.Store(nullptr);
			return nullptr;
		}

		typename TRegTokenOperator::tRef Reference=TRegTokenOperator::Reference(CurRegToken);
		if(Reference!=nullptr){
			fRegToken.Release.Store(CurRegToken);
		}
		else{
			TRegTokenOperator::Unregister(CurRegToken,this);
			fRegToken.Release.Store(nullptr);
		}

		return Reference;
	}
	typename TRegTokenOperator::tRef operator + (void)noexcept(true){
		return Ref();
	}

	operator bool ()const noexcept(true){	return fRegToken.Acquire.Load()!=nullptr;	}

	cPtrWeakReference(const cPtrWeakReference &Src)noexcept(true)
		: fRegToken(TRegTokenOperator::Register(Src.Ref(),this)){}

protected:
	cnAsync::cAtomicVariable<tRegToken> fRegToken;


	virtual bool cnLib_FUNC InvalidationNotify(tRegToken RegToken)noexcept(true) 
#ifndef cnLibrary_CPPEXCLUDE_VIRTUAL_OVERRIDE
		override
#endif // cnLibrary_CPPEXCLUDE_VIRTUAL_OVERRIDE
	{
		return fRegToken.Free.CmpStore(RegToken,nullptr);
	}

};
//---------------------------------------------------------------------------
}	// namespace cnVar
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#include <cnTK/TKMacrosCleanup.inc>
//---------------------------------------------------------------------------
#endif	/* cnLibrary_CPPFEATURELEVEL >= 1 */
/*-------------------------------------------------------------------------*/
#endif
