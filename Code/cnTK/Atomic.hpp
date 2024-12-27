/*- cnTK - Atomic ---------------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2010-12-23                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnTK_Atomic_HPP__
#define	__cnLibrary_cnTK_Atomic_HPP__
/*-------------------------------------------------------------------------*/
#include <cnTK/Common.hpp>
#include <cnTK/TypeTraits.hpp>
#include <cnTK/Pointer.hpp>
/*-------------------------------------------------------------------------*/
#if	cnLibrary_CPPFEATURELEVEL >= 1
//---------------------------------------------------------------------------
#include <cnTK/TKMacrosDeclare.inc>
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace TKRuntime{
//---------------------------------------------------------------------------

template<uIntn Size>
struct TAtomicInteger;
//{
//	typedef tAtomic;
//	typedef tInteger;
//
//	static tInteger Get(const tAtomic &a)noexcept;
//	static void Set(tAtomic &a,tInteger v)noexcept;
//
//	static bool WatchEqual(tAtomic &a,tInteger Value,uIntn Count)noexcept;
//	static bool WatchNotEqual(tAtomic &a,tInteger Value,uIntn Count)noexcept;
//
//	static tInteger FreeLoad(const tAtomic &a)noexcept;
//	static tInteger AcquireLoad(const tAtomic &a)noexcept;
//
//	static void FreeStore(tAtomic &a,tInteger v)noexcept;
//	static void ReleaseStore(tAtomic &a,tInteger v)noexcept;
//
//	static tInteger FreeExchange(tAtomic &a,tInteger v)noexcept;
//	static tInteger AcquireExchange(tAtomic &a,tInteger v)noexcept;
//	static tInteger ReleaseExchange(tAtomic &a,tInteger v)noexcept;
//	static tInteger BarrierExchange(tAtomic &a,tInteger v)noexcept;
//
//	static bool FreeCompareStore(tAtomic &a,tInteger c,tInteger v)noexcept;
//	static bool AcquireCompareStore(tAtomic &a,tInteger c,tInteger v)noexcept;
//	static bool ReleaseCompareStore(tAtomic &a,tInteger c,tInteger v)noexcept;
//	static bool BarrierCompareStore(tAtomic &a,tInteger c,tInteger v)noexcept;

//	static bool FreeCompareExchange(tAtomic &a,tInteger &c,tInteger v)noexcept;
//	static bool AcquireCompareExchange(tAtomic &a,tInteger &c,tInteger v)noexcept;
//	static bool ReleaseCompareExchange(tAtomic &a,tInteger &c,tInteger v)noexcept;
//	static bool BarrierCompareExchange(tAtomic &a,tInteger &c,tInteger v)noexcept;
//
//	static tInteger FreeAdd(tAtomic &a,tInteger v)noexcept;
//	static tInteger FreeAddN(tAtomic &a,tInteger v)noexcept;
//	static tInteger AcquireAdd(tAtomic &a,tInteger v)noexcept;
//	static tInteger AcquireAddN(tAtomic &a,tInteger v)noexcept;
//	static tInteger ReleaseAdd(tAtomic &a,tInteger v)noexcept;
//	static tInteger ReleaseAddN(tAtomic &a,tInteger v)noexcept;
//	static tInteger BarrierAdd(tAtomic &a,tInteger v)noexcept;
//	static tInteger BarrierAddN(tAtomic &a,tInteger v)noexcept;
//
//	static tInteger FreeSub(tAtomic &a,tInteger v)noexcept;
//	static tInteger FreeSubN(tAtomic &a,tInteger v)noexcept;
//	static tInteger AcquireSub(tAtomic &a,tInteger v)noexcept;
//	static tInteger AcquireSubN(tAtomic &a,tInteger v)noexcept;
//	static tInteger ReleaseSub(tAtomic &a,tInteger v)noexcept;
//	static tInteger ReleaseSubN(tAtomic &a,tInteger v)noexcept;
//	static tInteger BarrierSub(tAtomic &a,tInteger v)noexcept;
//	static tInteger BarrierSubN(tAtomic &a,tInteger v)noexcept;
//};


template<class TPointer>
struct TAtomicLink;
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
	typedef typename TIntegerAtomicOperator::tAtomic tAtomic;
	typedef typename TIntegerAtomicOperator::tInteger tInteger;

	typedef typename TIntegerAtomicOperator::template tDebugCast<TVariable>::Type tDebugDisplay;

	static TVariable Get(const tAtomic &av)noexcept(true){
		return cnVar::ReturnCast<TVariable>( TIntegerAtomicOperator::Get(av) );
	}
	static void Set(tAtomic &av,const TVariable &v)noexcept(true){
		return TIntegerAtomicOperator::Set(av,reinterpret_cast<const tInteger&>(v));
	}

	static bool WatchEqual(const tAtomic &av,const TVariable &Value,uIntn Count)noexcept(true){
		return TIntegerAtomicOperator::WatchEqual(av,reinterpret_cast<const tInteger&>(Value),Count);
	}
	static bool WatchNotEqual(const tAtomic &av,const TVariable &Value,uIntn Count)noexcept(true){
		return TIntegerAtomicOperator::WatchNotEqual(av,reinterpret_cast<const tInteger&>(Value),Count);
	}

	static TVariable FreeLoad(const tAtomic &av)noexcept(true){
		return cnVar::ReturnCast<TVariable>( TIntegerAtomicOperator::FreeLoad(av) );
	}
	static TVariable AcquireLoad(const tAtomic &av)noexcept(true){
		return cnVar::ReturnCast<TVariable>( TIntegerAtomicOperator::AcquireLoad(av) );
	}
	static void FreeStore(tAtomic &av,const TVariable &v)noexcept(true){
		return TIntegerAtomicOperator::FreeStore(av,reinterpret_cast<const tInteger&>(v));
	}
	static void ReleaseStore(tAtomic &av,const TVariable &v)noexcept(true){
		return TIntegerAtomicOperator::ReleaseStore(av,reinterpret_cast<const tInteger&>(v));
	}
	
	static TVariable FreeExchange(tAtomic &av,const TVariable &v)noexcept(true){
		return cnVar::ReturnCast<TVariable>( TIntegerAtomicOperator::FreeExchange(av,reinterpret_cast<const tInteger&>(v)) );
	}
	static TVariable AcquireExchange(tAtomic &av,const TVariable &v)noexcept(true){
		return cnVar::ReturnCast<TVariable>( TIntegerAtomicOperator::AcquireExchange(av,reinterpret_cast<const tInteger&>(v)) );
	}
	static TVariable ReleaseExchange(tAtomic &av,const TVariable &v)noexcept(true){
		return cnVar::ReturnCast<TVariable>( TIntegerAtomicOperator::ReleaseExchange(av,reinterpret_cast<const tInteger&>(v)) );
	}
	static TVariable BarrierExchange(tAtomic &av,const TVariable &v)noexcept(true){
		return cnVar::ReturnCast<TVariable>( TIntegerAtomicOperator::BarrierExchange(av,reinterpret_cast<const tInteger&>(v)) );
	}

	static bool FreeCompareStore(tAtomic &av,const TVariable &c,const TVariable &v)noexcept(true){
		return TIntegerAtomicOperator::FreeCompareStore(av,reinterpret_cast<const tInteger&>(c),reinterpret_cast<const tInteger&>(v));
	}
	static bool AcquireCompareStore(tAtomic &av,const TVariable &c,const TVariable &v)noexcept(true){
		return TIntegerAtomicOperator::AcquireCompareStore(av,reinterpret_cast<const tInteger&>(c),reinterpret_cast<const tInteger&>(v));
	}
	static bool ReleaseCompareStore(tAtomic &av,const TVariable &c,const TVariable &v)noexcept(true){
		return TIntegerAtomicOperator::ReleaseCompareStore(av,reinterpret_cast<const tInteger&>(c),reinterpret_cast<const tInteger&>(v));
	}
	static bool BarrierCompareStore(tAtomic &av,const TVariable &c,const TVariable &v)noexcept(true){
		return TIntegerAtomicOperator::BarrierCompareStore(av,reinterpret_cast<const tInteger&>(c),reinterpret_cast<const tInteger&>(v));
	}

	static bool FreeCompareExchange(tAtomic &av,TVariable &c,const TVariable &v)noexcept(true){
		return TIntegerAtomicOperator::FreeCompareExchange(av,reinterpret_cast<tInteger&>(c),reinterpret_cast<const tInteger&>(v));
	}
	static bool AcquireCompareExchange(tAtomic &av,TVariable &c,const TVariable &v)noexcept(true){
		return TIntegerAtomicOperator::AcquireCompareExchange(av,reinterpret_cast<tInteger&>(c),reinterpret_cast<const tInteger&>(v));
	}
	static bool ReleaseCompareExchange(tAtomic &av,TVariable &c,const TVariable &v)noexcept(true){
		return TIntegerAtomicOperator::ReleaseCompareExchange(av,reinterpret_cast<tInteger&>(c),reinterpret_cast<const tInteger&>(v));
	}
	static bool BarrierCompareExchange(tAtomic &av,TVariable &c,const TVariable &v)noexcept(true){
		return TIntegerAtomicOperator::BarrierCompareExchange(av,reinterpret_cast<tInteger&>(c),reinterpret_cast<const tInteger&>(v));
	}

	static TVariable FreeAdd(tAtomic &av,const TVariable &v)noexcept(true){
		return cnVar::ReturnCast<TVariable>( TIntegerAtomicOperator::FreeAdd(av,reinterpret_cast<const tInteger&>(v)) );
	}
	static TVariable FreeAddN(tAtomic &av,const TVariable &v)noexcept(true){
		return cnVar::ReturnCast<TVariable>( TIntegerAtomicOperator::FreeAddN(av,reinterpret_cast<const tInteger&>(v)) );
	}
	static TVariable AcquireAdd(tAtomic &av,const TVariable &v)noexcept(true){
		return cnVar::ReturnCast<TVariable>( TIntegerAtomicOperator::AcquireAdd(av,reinterpret_cast<const tInteger&>(v)) );
	}
	static TVariable AcquireAddN(tAtomic &av,const TVariable &v)noexcept(true){
		return cnVar::ReturnCast<TVariable>( TIntegerAtomicOperator::AcquireAddN(av,reinterpret_cast<const tInteger&>(v)) );
	}
	static TVariable ReleaseAdd(tAtomic &av,const TVariable &v)noexcept(true){
		return cnVar::ReturnCast<TVariable>( TIntegerAtomicOperator::ReleaseAdd(av,reinterpret_cast<const tInteger&>(v)) );
	}
	static TVariable ReleaseAddN(tAtomic &av,const TVariable &v)noexcept(true){
		return cnVar::ReturnCast<TVariable>( TIntegerAtomicOperator::ReleaseAddN(av,reinterpret_cast<const tInteger&>(v)) );
	}
	static TVariable BarrierAdd(tAtomic &av,const TVariable &v)noexcept(true){
		return cnVar::ReturnCast<TVariable>( TIntegerAtomicOperator::BarrierAdd(av,reinterpret_cast<const tInteger&>(v)) );
	}
	static TVariable BarrierAddN(tAtomic &av,const TVariable &v)noexcept(true){
		return cnVar::ReturnCast<TVariable>( TIntegerAtomicOperator::BarrierAddN(av,reinterpret_cast<const tInteger&>(v)) );
	}

	static TVariable FreeSub(tAtomic &av,const TVariable &v)noexcept(true){
		return cnVar::ReturnCast<TVariable>( TIntegerAtomicOperator::FreeSub(av,reinterpret_cast<const tInteger&>(v)) );
	}
	static TVariable FreeSubN(tAtomic &av,const TVariable &v)noexcept(true){
		return cnVar::ReturnCast<TVariable>( TIntegerAtomicOperator::FreeSubN(av,reinterpret_cast<const tInteger&>(v)) );
	}
	static TVariable AcquireSub(tAtomic &av,const TVariable &v)noexcept(true){
		return cnVar::ReturnCast<TVariable>( TIntegerAtomicOperator::AcquireSub(av,reinterpret_cast<const tInteger&>(v)) );
	}
	static TVariable AcquireSubN(tAtomic &av,const TVariable &v)noexcept(true){
		return cnVar::ReturnCast<TVariable>( TIntegerAtomicOperator::AcquireSubN(av,reinterpret_cast<const tInteger&>(v)) );
	}
	static TVariable ReleaseSub(tAtomic &av,const TVariable &v)noexcept(true){
		return cnVar::ReturnCast<TVariable>( TIntegerAtomicOperator::ReleaseSub(av,reinterpret_cast<const tInteger&>(v)) );
	}
	static TVariable ReleaseSubN(tAtomic &av,const TVariable &v)noexcept(true){
		return cnVar::ReturnCast<TVariable>( TIntegerAtomicOperator::ReleaseSubN(av,reinterpret_cast<const tInteger&>(v)) );
	}
	static TVariable BarrierSub(tAtomic &av,const TVariable &v)noexcept(true){
		return cnVar::ReturnCast<TVariable>( TIntegerAtomicOperator::BarrierSub(av,reinterpret_cast<const tInteger&>(v)) );
	}
	static TVariable BarrierSubN(tAtomic &av,const TVariable &v)noexcept(true){
		return cnVar::ReturnCast<TVariable>( TIntegerAtomicOperator::BarrierSubN(av,reinterpret_cast<const tInteger&>(v)) );
	}
};

template<class TVariable>
union cAtomicVariable
{
	typedef typename TKRuntime::template TAtomicInteger<sizeof(TVariable)> tAtomicInteger;
	typedef typename tAtomicInteger::tInteger tInteger;
	typedef typename tAtomicInteger::tAtomic tAtomic;
	typedef TVariable tVariable;
	typedef typename cnVar::TSelect<
		cnVar::TIsSame<tInteger,tVariable>::Value
		, TAtomicVariableOperatorByInteger<TVariable,tAtomicInteger>
		, tAtomicInteger
	>::Type tAtomicOperator;
	

	tAtomic _atomic_;

	cAtomicVariable()noexcept(true){}
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
	void operator = (const tVariable &Value)noexcept(true){	return tAtomicOperator::Set(_atomic_,Value);	}
	// Set
	//	assign value non-atomicly
	void Set(const tVariable &Value)noexcept(true){			return tAtomicOperator::Set(_atomic_,Value);	}




	bool WatchEqual(const tVariable &Value,uIntn Count)const noexcept(true){
		return tAtomicOperator::WatchEqual(_atomic_,Value,Count);
	}
	bool WatchNotEqual(const tVariable &Value,uIntn Count)const noexcept(true){
		return tAtomicOperator::WatchNotEqual(_atomic_,Value,Count);
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
template<class T,class TPointerReferenceOperator>
class cAtomicPtrReference
{
public:
	cAtomicPtrReference()noexcept(true):fAtomicPtr(nullptr){}
	~cAtomicPtrReference()noexcept(true){
		T *Pointer=fAtomicPtr.Get();
		if(Pointer!=nullptr){
			TPointerReferenceOperator::Release(Pointer);
		}
	}

	T* Peek(void)const noexcept(true){	return fAtomicPtr.Get();	}

	// <<= xchg

	cPtrReference<T,TPointerReferenceOperator> operator <<=(T *Pointer)noexcept(true){
		if(Pointer!=nullptr)
			TPointerReferenceOperator::Acquire(Pointer);
		// swap
		T *PrevPointer=fAtomicPtr.Barrier.Xchg(Pointer);
		return cPtrReference<T,TPointerReferenceOperator>::TakeFromManual(PrevPointer);
	}

	struct CmpOp{	cnAsync::cAtomicVariable<T*> &fAtomicPtr;	T *ComparePointer;
		bool operator <<=(T *Pointer)noexcept(true){
			if(Pointer!=nullptr){
				TPointerReferenceOperator::Acquire(Pointer);
				if(fAtomicPtr.Barrier.CmpStore(ComparePointer,Pointer)==false){
					TPointerReferenceOperator::Release(Pointer);
					return false;
				}
			}
			else{
				if(fAtomicPtr.Barrier.CmpStore(ComparePointer,nullptr)==false){
					return false;
				}
			}
			if(ComparePointer!=nullptr)
				TPointerReferenceOperator::Release(ComparePointer);
			return true;
		}
	};
	CmpOp operator[](T *Compare)noexcept(true){		return CmpOp{fAtomicPtr,Compare};	}

	// ^=  set if nullptr

	bool operator ^=(T *Pointer)noexcept(true){
		if(Pointer==nullptr){
			return fAtomicPtr==nullptr;
		}

		TPointerReferenceOperator::Acquire(Pointer);
		if(fAtomicPtr.Barrier.CmpStore(nullptr,Pointer)){
			return true;
		}
		TPointerReferenceOperator::Release(Pointer);
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
	cnAsync::cAtomicVariable<T*> fAtomicPtr;
};
#if 0
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
		, class=decltype(TRegTokenOperator::Register(DeclVal<T cnLib_UREF>(),nullptr))
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
		, decltype(TRegTokenOperator::Register(DeclVal<T cnLib_UREF>(),nullptr))
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
#endif // 0
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
