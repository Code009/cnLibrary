/*- cnTK - Pointer Management ---------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2018-08-14                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnTK_Pointer_H__
#define	__cnLibrary_cnTK_Pointer_H__
/*-------------------------------------------------------------------------*/
#include <cnTK/Common.h>
#include <cnTK/TypeTraits.h>
/*-------------------------------------------------------------------------*/
#if	cnLibrary_CPPFEATURELEVEL >= 1
//---------------------------------------------------------------------------
#include <cnTK/TKMacrosDeclare.inc>
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnMemory{
//---------------------------------------------------------------------------

// PointerAddByteOffset
//	offset pointer by number of bytes
// Pointer		pointer to calculate
// OffsetByte	number of bytes to add to the pointer
// return offseted pointer
template<class TPtr>
inline TPtr* PointerAddByteOffset(TPtr *Pointer,sIntn OffsetByte)noexcept(true)
{
	return reinterpret_cast<TPtr*>(
		reinterpret_cast<uIntn>(Pointer)+OffsetByte
	);
}

inline uIntn PointerDistance(const void *Dest,const void *Src)noexcept(true)
{
	return reinterpret_cast<uIntn>(static_cast<const uInt8*>(Dest)-reinterpret_cast<uIntn>(Src));
}

//---------------------------------------------------------------------------

// CastPointerAddByteOffset
//	offset pointer by number of bytes
// Pointer		pointer to calculate
// OffsetByte	number of bytes to add to the pointer
// return offseted and casted pointer
template<class TRet,class TPtr>
inline TRet* CastPointerAddByteOffset(TPtr *Pointer,sIntn OffsetByte)noexcept(true)
{
	return reinterpret_cast<TRet*>(
		reinterpret_cast<uIntn>(Pointer)+OffsetByte
	);
}

// AddressOf
//	get address of object, bypass overloaded operator
// Object		reference to the object
// return address of the object
template<class T>
inline T* AddressOf(T &Object)noexcept(true)
{
	typedef typename cnVar::TCopyCV<char,T>::Type TInterpret;
	return reinterpret_cast<T*>(&reinterpret_cast<TInterpret&>(Object));
}

// MemberOffset
//	get offset of a class member
// MemberPointer		pointer to class member
// return offset of the member
template<class TMemberClass,class TMember>
uIntn MemberOffset(TMember TMemberClass::* MemberPointer)noexcept(true)
{
	return reinterpret_cast<uIntn>( AddressOf(static_cast<TMemberClass*>(nullptr)->*MemberPointer) );
}

// GetObjectFromMemberPointer
//	caculate object address by pointer to member
// Pointer				pointer to member
// MemberPointer		class member pointer
// return pointer to class object
template<class TMemberClass,class TMember,class TPointer>
inline typename cnVar::TCopyCV<TMemberClass,TPointer>::Type* GetObjectFromMemberPointer(TPointer *Pointer,TMember TMemberClass::*MemberPointer)noexcept(true)
{
	typedef const volatile uInt8 TInterpret;
	TInterpret *MemberOffsetPointer=&reinterpret_cast<TInterpret&>(static_cast<TPointer&>(static_cast<TMemberClass*>(nullptr)->*MemberPointer));
	return reinterpret_cast<typename cnVar::TCopyCV<TMemberClass,TPointer>::Type*>(reinterpret_cast<TInterpret*>(Pointer)-MemberOffsetPointer);
}

// GetObjectFromArrayMemberPointer
//	caculate object address by pointer to member
// Pointer				pointer to member
// Index				index of the member in the array
// MemberPointer		class array member pointer
// return pointer to class object
template<class TMemberClass,class TArrayMember,class TPointer>
inline typename cnVar::TCopyCV<TMemberClass,TPointer>::Type* GetObjectFromArrayMemberPointer(TPointer *Pointer,uIntn Index,TArrayMember TMemberClass::*MemberPointer)noexcept(true)
{
	typedef const volatile uInt8 TInterpret;
	TInterpret *MemberOffsetPointer=reinterpret_cast<TInterpret*>(static_cast<TPointer*>(Index+static_cast<TMemberClass*>(nullptr)->*MemberPointer));
	return reinterpret_cast<typename cnVar::TCopyCV<TMemberClass,TPointer>::Type*>(reinterpret_cast<TInterpret*>(Pointer)-MemberOffsetPointer);
}

//---------------------------------------------------------------------------
}	// namespace cnMemory
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
namespace cnLib_THelper{
//---------------------------------------------------------------------------
namespace Var_TH{
//---------------------------------------------------------------------------
template<class TTokenOperator,class TSrc>
struct PointerTokenFromExpression
{
	template<int=sizeof(TTokenOperator::TokenFrom(cnVar::DeclVar<TSrc>()))>
	struct Test
	{
		typedef void Type;
	};
};
//---------------------------------------------------------------------------
}	// namespace Var_TH
//---------------------------------------------------------------------------
}	// namespace cnLib_THelper
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnVar{
//---------------------------------------------------------------------------
//TOwnerTokenOperator
//{
//	typedef tPtr;
//	typedef tOwnerToken; 	// assignable
//
//	static tPtr Pointer(const tOwnerToken &Token)const noexcpt;
//	static tOwnerToken TokenNull(void)noexcept;
//	template<class T>	static tOwnerToken TokenFrom(T&& Src)noexcept;
//	static void Release(tOwnerToken &Token)noexcept;
//};
//---------------------------------------------------------------------------
template<class TOwnerTokenOperator>
class cPtrOwner
{
public:
	typedef typename TOwnerTokenOperator::tPtr tPtr;
	typedef typename TOwnerTokenOperator::tOwnerToken tOwnerToken;

	tPtr Pointer(void)const noexcept(true){	return TOwnerTokenOperator::Pointer(fOwnerToken);	}

	operator tPtr ()const noexcept(true){		return Pointer();	}
	tPtr operator ->()const noexcept(true){		return Pointer();	}

	tOwnerToken &Token(void)noexcept(true)			{	return fOwnerToken;	}
	const tOwnerToken &Token(void)const	noexcept(true){	return fOwnerToken;	}

	cPtrOwner()noexcept(true)
		: fOwnerToken(TOwnerTokenOperator::TokenNull())
	{}
	~cPtrOwner()noexcept(true){
		TOwnerTokenOperator::Release(fOwnerToken);
	}

#ifndef cnLibrary_CPPEXCLUDE_NULLPTR
	// construct with null
	cPtrOwner(tNullptr)noexcept(true)
		: fOwnerToken(TOwnerTokenOperator::TokenNull())
	{}
	// assign with null

	cPtrOwner& operator =(tNullptr)noexcept(true){
		Clear();
		return *this;
	}
#endif


	void Clear(void)noexcept(true){
		TOwnerTokenOperator::Release(fOwnerToken);
		fOwnerToken=TOwnerTokenOperator::TokenNull();
	}

	template<class TSrcPtr>
	static typename TSelect<0,cPtrOwner
		, typename cnLib_THelper::Var_TH::PointerTokenFromExpression<TOwnerTokenOperator,TSrcPtr*>::template Test<>::Type
	>::Type TakeFromManual(TSrcPtr *Pointer)noexcept(true)
	{
		return cPtrOwner(TOwnerTokenOperator::TokenFrom(Pointer));
	}

	
	tPtr ExtractToManual(void)noexcept(true){
		tPtr Ptr=Pointer();
		fOwnerToken=TOwnerTokenOperator::TokenNull();
		return Ptr;
	}

#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

	// copy construct and assign

#ifndef cnLibrary_CPPEXCLUDE_CLASS_MEMBER_DELETE
	cPtrOwner(const cPtrOwner &Src)=delete;
	cPtrOwner& operator =(const cPtrOwner &Src)=delete;
#else
private:
	cPtrOwner(const cPtrOwner &Src);
	cPtrOwner& operator =(const cPtrOwner &Src);
public:
#endif

	// move construct
	cPtrOwner(cPtrOwner &&Src)noexcept(true)
		: fOwnerToken(static_cast<tOwnerToken&&>(Src.fOwnerToken))
	{
		Src.fOwnerToken=TOwnerTokenOperator::TokenNull();
	}

	// move
	cPtrOwner& operator =(cPtrOwner &&Src)noexcept(true){
		tOwnerToken SwapToken(static_cast<tOwnerToken&&>(Src.fOwnerToken));
		Src.fOwnerToken=TOwnerTokenOperator::TokenNull();

		TOwnerTokenOperator::Release(fOwnerToken);
		fOwnerToken=static_cast<tOwnerToken&&>(SwapToken);
		return *this;
	}

	// move construct by token

	explicit cPtrOwner(tOwnerToken &&SrcToken)noexcept(true)
		: fOwnerToken(static_cast<tOwnerToken&&>(SrcToken))
	{
		SrcToken=TOwnerTokenOperator::TokenNull();
	}


	// move construct with token from cPtrOwner
	
	template<class TSrcOwnerTokenOperator
#ifndef cnLibrary_CPPEXCLUDE_FUNCTION_TEMPLATE_DEFALT_ARGUMENT
		, class=cnLib_THelper::Var_TH::PointerTokenFromExpression<TOwnerTokenOperator,typename TSrcOwnerTokenOperator::tOwnerToken&>::template Test<>::Type
#endif
	>
	cPtrOwner(cPtrOwner<TSrcOwnerTokenOperator> &&Src)noexcept(true)
		: fOwnerToken(TOwnerTokenOperator::TokenFrom(Src.Token()))
	{
		Src.Token()=TSrcOwnerTokenOperator::TokenNull();
	}

	// move assign with token from cPtrOwner
	
	template<class TSrcOwnerTokenOperator>
	typename TSelect<0,cPtrOwner&
		, typename cnLib_THelper::Var_TH::PointerTokenFromExpression<TOwnerTokenOperator,typename TSrcOwnerTokenOperator::tOwnerToken&>::template Test<>::Type
	>::Type operator =(cPtrOwner<TSrcOwnerTokenOperator> &&Src)noexcept(true){
		tOwnerToken SwapToken(static_cast<tOwnerToken&&>(Src.Token()));
		Src.Token()=TSrcOwnerTokenOperator::TokenNull();

		TOwnerTokenOperator::Release(fOwnerToken);
		fOwnerToken=static_cast<tOwnerToken&&>(SwapToken);
		return *this;
	}

// cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L
#else
// cnLibrary_CPPFEATURE_RVALUE_REFERENCES < 200610L

	// move construct
	cPtrOwner(cPtrOwner &Src)noexcept(true)
		: fOwnerToken(Src.fOwnerToken)
	{
		Src.fOwnerToken=TOwnerTokenOperator::TokenNull();
	}

	// move assignment
	cPtrOwner& operator =(cPtrOwner &Src)noexcept(true){
		tOwnerToken SwapToken(Src.fOwnerToken);
		Src.fOwnerToken=TOwnerTokenOperator::TokenNull();

		TOwnerTokenOperator::Release(fOwnerToken);
		fOwnerToken=SwapToken;
		return *this;
	}
	
	// move construct by token
	
	cPtrOwner(tOwnerToken &SrcToken)noexcept(true)
		: fOwnerToken(SrcToken)
	{
		SrcToken=TOwnerTokenOperator::TokenNull();
	}
	// move assign by token

	cPtrOwner& operator =(tOwnerToken &SrcToken)noexcept(true)
	{
		tOwnerToken SwapToken(SrcToken);
		SrcToken=TOwnerTokenOperator::TokenNull();

		TOwnerTokenOperator::Release(fOwnerToken);
		fOwnerToken=SwapToken;
		return *this;
	}



	
	// construct with token from cPtrOwner

	
	template<class TSrcOwnerTokenOperator
#ifndef cnLibrary_CPPEXCLUDE_FUNCTION_TEMPLATE_DEFALT_ARGUMENT
		, class=cnLib_THelper::Var_TH::PointerTokenFromExpression<TOwnerTokenOperator,typename TSrcOwnerTokenOperator::tOwnerToken&>::template Test<>::Type
#endif
	>
	cPtrOwner(cPtrOwner<TSrcOwnerTokenOperator> &Src)noexcept(true)
		: fOwnerToken(TOwnerTokenOperator::TokenFrom(Src.Token()))
	{
		Src.Token()=TSrcOwnerTokenOperator::TokenNull();
	}

	// move assign with token from cPtrOwner
	
	template<class TSrcOwnerTokenOperator>
	typename TSelect<0,cPtrOwner&
		, typename cnLib_THelper::Var_TH::PointerTokenFromExpression<TOwnerTokenOperator,typename TSrcOwnerTokenOperator::tOwnerToken&>::template Test<>::Type
	>::Type operator =(cPtrOwner<TSrcOwnerTokenOperator> &Src)noexcept(true){
		tOwnerToken SwapToken(SrcToken);
		Src.Token()=TOwnerTokenOperator::TokenNull();

		TOwnerTokenOperator::Release(fOwnerToken);
		fOwnerToken=SwapToken;
		return *this;
	}

#endif	// cnLibrary_CPPFEATURE_RVALUE_REFERENCES < 200610L

protected:
	tOwnerToken fOwnerToken;

};
//---------------------------------------------------------------------------
template<class TPointer>
struct bcPointerOwnerTokenOperator
{
	typedef TPointer tPtr;
	typedef TPointer tOwnerToken;

	static tPtr Pointer(const tOwnerToken &OwnerToken)noexcept(true){	return OwnerToken;	}
	static tOwnerToken TokenNull(void)noexcept(true){		return nullptr;	}
	static tOwnerToken TokenFrom(tOwnerToken Ptr)noexcept(true){	return Ptr;	}
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//TRefTokenOperator
//{
//	typedef tPtr;
//	typedef tRefToken; 	// assignable
//
//	static tPtr Pointer(const tRefToken &Token)const noexcept;
//	static tRefToken TokenNull(void)noexcept;
//	template<class T>	tRefToken TokenFrom(T&& Src)noexcept;
//	static void Acquire(tRefToken &Token)noexcept;
//	static void Release(tRefToken &Token)noexcept;
//};
//---------------------------------------------------------------------------
template<class TRefTokenOperator>
class cPtrReference
{
public:
	typedef typename TRefTokenOperator::tPtr tPtr;
	typedef typename TRefTokenOperator::tRefToken tRefToken;

	tPtr Pointer(void)const noexcept(true){	return TRefTokenOperator::Pointer(fRefToken);	}

	operator tPtr ()const noexcept(true){	return Pointer();	}
	tPtr operator ->()const noexcept(true){	return Pointer();	}


	tRefToken &Token(void)noexcept(true)			{	return fRefToken;	}
	const tRefToken &Token(void)const noexcept(true){	return fRefToken;	}

	cPtrReference()noexcept(true)
		:fRefToken(TRefTokenOperator::TokenNull())
	{}
	~cPtrReference()noexcept(true){
		TRefTokenOperator::Release(fRefToken);
	}

#ifndef cnLibrary_CPPEXCLUDE_NULLPTR

	// construct with null
	cPtrReference(tNullptr)noexcept(true)
		:fRefToken(TRefTokenOperator::TokenNull())
	{}
	// assign with null
	cPtrReference& operator =(tNullptr)noexcept(true){
		Clear();
		return *this;
	}
#endif
	
	void Clear(void)noexcept(true){
		TRefTokenOperator::Release(fRefToken);
		fRefToken=TRefTokenOperator::TokenNull();
	}

	
private:
	template<class TToken>
	struct ConstructByToken
	{
		TToken Value;
	};

	static ConstructByToken<const tRefToken &> MakeConstructByToken(const tRefToken &Token){
		ConstructByToken<const tRefToken &> Ret={Token};
		return Ret;
	}
	// construct with token
	explicit cPtrReference(ConstructByToken<const tRefToken &> ByToken)noexcept(true)
		: fRefToken(ByToken.Value)
	{
		TRefTokenOperator::Acquire(fRefToken);
	}

#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

	static ConstructByToken<tRefToken &&> MakeConstructByToken(tRefToken &&Token){
		ConstructByToken<tRefToken &&> Ret={static_cast<tRefToken&&>(Token)};
		return Ret;
	}
	// move construct with token
	explicit cPtrReference(ConstructByToken<tRefToken &&> ByToken)noexcept(true)
		: fRefToken(static_cast<tRefToken&&>(ByToken.Value))
	{
	}

#endif // cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L
public:

	template<class TSrcPtr>
	static typename TSelect<0,cPtrReference
		, typename cnLib_THelper::Var_TH::PointerTokenFromExpression<TRefTokenOperator,TSrcPtr*>::template Test<>::Type
	>::Type TakeFromManual(TSrcPtr *Pointer)noexcept(true)
	{
		return cPtrReference(MakeConstructByToken(TRefTokenOperator::TokenFrom(Pointer)));
		//return cPtrReference(TRefTokenOperator::TokenFrom(Pointer));
	}

	tPtr ExtractToManual(void)noexcept(true){
		tPtr Ptr=Pointer();
		fRefToken=TRefTokenOperator::TokenNull();
		return Ptr;
	}


	void ManualIncReference(void)const noexcept(true){	TRefTokenOperator::Acquire(fRefToken);	}
	void ManualDecReference(void)const noexcept(true){	TRefTokenOperator::Release(fRefToken);	}


	// copy constructor

	cPtrReference(const cPtrReference &Src)noexcept(true)
		: fRefToken(Src.fRefToken)
	{
		TRefTokenOperator::Acquire(fRefToken);
	}

	// assign

	cPtrReference& operator =(const cPtrReference &Src)noexcept(true){
		if(this!=&Src){
			tRefToken SwapToken(cnLib_UREFCAST(tRefToken)(fRefToken));

			fRefToken=Src.fRefToken;

			TRefTokenOperator::Acquire(fRefToken);
			TRefTokenOperator::Release(SwapToken);
		}
		return *this;
	}
#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

	// move constructor

	cPtrReference(cPtrReference &&Src)noexcept(true)
		: fRefToken(static_cast<tRefToken&&>(Src.fRefToken))
	{
		Src.fRefToken=TRefTokenOperator::TokenNull();
	}

	// move

	cPtrReference& operator =(cPtrReference &&Src)noexcept(true){
		tRefToken SwapToken(static_cast<tRefToken&&>(Src.fRefToken));
		Src.fRefToken=TRefTokenOperator::TokenNull();

		TRefTokenOperator::Release(fRefToken);
		fRefToken=static_cast<tRefToken&&>(SwapToken);
		return *this;
	}

#endif // cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L


	// construct with token from cPtrReference


	template<class TSrcRefTokenOperator
#ifndef cnLibrary_CPPEXCLUDE_FUNCTION_TEMPLATE_DEFALT_ARGUMENT
		, class=cnLib_THelper::Var_TH::PointerTokenFromExpression<TRefTokenOperator,typename TSrcRefTokenOperator::tRefToken&>::template Test<>::Type
#endif
	>
	cPtrReference(const cPtrReference<TSrcRefTokenOperator> &Src)noexcept(true)
		: fRefToken(TRefTokenOperator::TokenFrom(Src.Token()))
	{
		TRefTokenOperator::Acquire(fRefToken);
	}


	// assign with token from cPtrReference
	
	template<class TSrcRefTokenOperator>
	typename TSelect<0,cPtrReference&
		, typename cnLib_THelper::Var_TH::PointerTokenFromExpression<TRefTokenOperator,typename TSrcRefTokenOperator::tRefToken&>::template Test<>::Type
	>::Type operator =(const cPtrReference<TSrcRefTokenOperator> &Src)noexcept(true){
		tRefToken SwapToken(fRefToken);

		fRefToken=TRefTokenOperator::TokenFrom(Src.Token());

		TRefTokenOperator::Acquire(fRefToken);
		TRefTokenOperator::Release(SwapToken);
		return *this;
	}

protected:
	tRefToken fRefToken;


	// make token with pointer
	template<class TSrc>
	static typename TSelect<0,tRefToken
		, typename cnLib_THelper::Var_TH::PointerTokenFromExpression<TRefTokenOperator,TSrc*>::template Test<>::Type
	>::Type MakeRefToken(TSrc *Pointer)noexcept(true){
		return TRefTokenOperator::TokenFrom(Pointer);
	}

	template<class TSrcRefTokenOperator>
	static typename TSelect<0,tRefToken
		, typename cnLib_THelper::Var_TH::PointerTokenFromExpression<TRefTokenOperator,typename TSrcRefTokenOperator::tPtr*>::template Test<>::Type
	>::Type MakeRefToken(const cPtrReference<TSrcRefTokenOperator> &Src)noexcept(true){
		return TRefTokenOperator::TokenFrom(Src.Pointer());
	}

	template<class T>
	static typename TSelect<0,tRefToken
		, typename cnLib_THelper::Var_TH::PointerTokenFromExpression<TRefTokenOperator,T cnLib_UREF>::template Test<>::Type
	>::Type MakeRefToken(T cnLib_UREF Src)noexcept(true){
		return TRefTokenOperator::TokenFrom(cnLib_UREFCAST(T)(Src));
	}
public:

	// construct by making token

	template<class T
#if cnLibrary_CPPFEATURE_DECLTYPE >= 200707L && !defined(cnLibrary_CPPEXCLUDE_FUNCTION_TEMPLATE_DEFALT_ARGUMENT)
		, class=decltype(MakeRefToken(DeclVar<T cnLib_UREF>()))
#endif
	>
	cPtrReference(T cnLib_UREF Src)noexcept(true)
		: fRefToken(TRefTokenOperator::TokenFrom(cnLib_UREFCAST(T)(Src)))
	{
		TRefTokenOperator::Acquire(fRefToken);
	}

	// assign by making token
	template<class T>
	typename TSelect<0,cPtrReference&
#if cnLibrary_CPPFEATURE_DECLTYPE >= 200707L
		, decltype(MakeRefToken(DeclVar<T cnLib_UREF>()))
#endif
	>::Type operator =(T cnLib_UREF Src)noexcept(true)
	{
		tRefToken SwapToken(fRefToken);

		fRefToken=MakeRefToken(cnLib_UREFCAST(T)(Src));

		TRefTokenOperator::Acquire(fRefToken);
		TRefTokenOperator::Release(SwapToken);
		return *this;
	}
};
//---------------------------------------------------------------------------
template<class TPointer>
struct bcPointerRefTokenOperator
{
	typedef TPointer tPtr;
	typedef TPointer tRefToken;

	static tPtr Pointer(const tRefToken &RefToken)noexcept(true){	return RefToken;	}

	static tRefToken TokenNull(void)noexcept(true){		return nullptr;	}
	static tRefToken TokenFrom(tRefToken Ptr)noexcept(true){	return Ptr;	}
};
//---------------------------------------------------------------------------
}   // namespace cnVar
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#include <cnTK/TKMacrosCleanup.inc>
//---------------------------------------------------------------------------
#endif	/* cnLibrary_CPPFEATURELEVEL >= 1 */
/*-------------------------------------------------------------------------*/
#endif
