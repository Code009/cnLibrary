/*- cnTK - Variant --------------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2018-08-14                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnTK_Variant_H__
#define	__cnLibrary_cnTK_Variant_H__
/*-------------------------------------------------------------------------*/
#include <cnTK/Common.h>
#include <cnTK/TypeInfo.h>
/*-------------------------------------------------------------------------*/
#if	cnLibrary_CPPFEATURELEVEL >= 1
//---------------------------------------------------------------------------
#include <cnTK/TKMacrosDeclare.inc>
//---------------------------------------------------------------------------
namespace cnLib_THelper{
//---------------------------------------------------------------------------
namespace Var_TH{
//---------------------------------------------------------------------------

template<bool Assignable>
struct AssignOrReconstruct
{
	template<class T,class TSrc>
	static void Call(T &Dest,TSrc cnLib_UREF Src)noexcept(cnLib_NOEXCEPTEXPR(Dest=cnLib_UREFCAST(TSrc)(Src)))
	{	Dest=cnLib_UREFCAST(TSrc)(Src);	}

};
template<>
struct AssignOrReconstruct<false>
{
	template<class T,class TSrc>
	static void Call(T &Dest,TSrc cnLib_UREF Src)
		noexcept(
			cnLib_NOEXCEPTEXPR(ManualDestruct(Dest) && cnLib_NOEXCEPTEXPR(T(cnLib_UREFCAST(TSrc)(Src))))
		)
	{
		ManualDestruct(Dest);
		ManualConstruct(Dest,cnLib_UREFCAST(TSrc)(Src));
	}

};

//---------------------------------------------------------------------------
}	// namespace Var_TH
//---------------------------------------------------------------------------
}	// namespace cnLib_THelper
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnVar{

template<class TDest,class TSrc>
void AssignOrReconstruct(TDest &Dest,TSrc cnLib_UREF Src)
	noexcept(
		cnLib_NOEXCEPTEXPR((cnLib_THelper::Var_TH::AssignOrReconstruct<TIsAssignable<TDest,TSrc cnLib_UREF>::Value>::Call(Dest,cnLib_UREFCAST(TSrc)(Src))))
	)
{	return cnLib_THelper::Var_TH::AssignOrReconstruct<TIsAssignable<TDest,TSrc cnLib_UREF>::Value>::Call(Dest,cnLib_UREFCAST(TSrc)(Src));	}


class cAnyPtr
{
public:

	cnLib_CONSTEXPR_FUNC cAnyPtr()noexcept(true)
		: Pointer(nullptr),TypeID(TTypeID<void>::Value){}
#ifndef cnLibrary_CPPEXCLUDE_NULLPTR
	cnLib_CONSTEXPR_FUNC cAnyPtr(tNullptr)noexcept(true)
		: Pointer(nullptr),TypeID(TTypeID<void>::Value){}
#endif
	cAnyPtr(const cAnyPtr &Src)noexcept(true){	Pointer=Src.Pointer;	TypeID=Src.TypeID;	}
	cAnyPtr(tTypeID ti,void *p)noexcept(true){	Pointer=p;	TypeID=ti;	}
	
	template<class T>
	explicit cAnyPtr(T *p)noexcept(true){	Set<T>(p);	}

#ifndef cnLibrary_CPPEXCLUDE_NULLPTR
	bool operator ==(tNullptr)const noexcept(true){	return Pointer==nullptr;	}
	bool operator !=(tNullptr)const noexcept(true){	return Pointer!=nullptr;	}
#endif

	bool operator ==(const cAnyPtr &Src)const noexcept(true){	return TypeID==Src.TypeID && Src.Pointer==Pointer;	}
	bool operator !=(const cAnyPtr &Src)const noexcept(true){	return TypeID!=Src.TypeID || Src.Pointer!=Pointer;	}
	bool operator <(const cAnyPtr &Src)const noexcept(true){	return TypeID==Src.TypeID ? Pointer<Src.Pointer : TypeID<Src.TypeID;	}
	bool operator <=(const cAnyPtr &Src)const noexcept(true){	return TypeID==Src.TypeID ? Pointer<=Src.Pointer : TypeID<Src.TypeID;	}
	bool operator >(const cAnyPtr &Src)const noexcept(true){	return TypeID==Src.TypeID ? Pointer>Src.Pointer : TypeID>Src.TypeID;	}
	bool operator >=(const cAnyPtr &Src)const noexcept(true){	return TypeID==Src.TypeID ? Pointer>=Src.Pointer : TypeID>Src.TypeID;	}
	
	template<class T>
	bool IsType(void)const noexcept(true){
		return TypeID==TTypeID<T*>::Value;
	}

	template<class T>
	bool Get(T* &Ptr)const noexcept(true){
		if(IsType<T>()){
			Ptr=static_cast<T*>(Pointer);
			return true;
		}
		return false;
	}
	template<class T>
	void Set(T *Ptr) noexcept(true){
		if(Ptr==nullptr){
			TypeID=TTypeID<void>::Value;
			Pointer=nullptr;
		}
		else{
			TypeID=TTypeID<T*>::Value;
			Pointer=Ptr;
		}
	}

	template<class T>
	cAnyPtr& operator =(T *Ptr)noexcept(true){
		Set<T>(Ptr);
		return *this;
	}
#ifndef cnLibrary_CPPEXCLUDE_NULLPTR
	cAnyPtr& operator =(tNullptr)noexcept(true){
		TypeID=TTypeID<void>::Value;
		Pointer=nullptr;
		return *this;
	}
#endif

	template<class T>
	T* Get(void)const noexcept(true){
		if(IsType<T>())
			return static_cast<T*>(Pointer);
		return nullptr;
	}

	template<class T>
	operator T *()const noexcept(true){	return Get<T>();	}

protected:
	tTypeID TypeID;
	void *Pointer;

};

template<class T>
inline cAnyPtr AnyPtr(T *Ptr)noexcept(true){
	return cAnyPtr(TTypeID<T*>::Value,Ptr);
}
#ifndef cnLibrary_CPPEXCLUDE_NULLPTR
inline cAnyPtr AnyPtr(tNullptr)noexcept(true){
	return cAnyPtr(TTypeID<void>::Value,nullptr);
}
#endif

//---------------------------------------------------------------------------
template<uIntn Size,class TAlignment>
class cPolymorphicBlock : protected cnMemory::cPlainAlignedData<Size,TAlignment>
{
public:
	void* operator &()noexcept(true){				return this;	}
	const void* operator &()const noexcept(true){	return this;	}

#if cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L

	template<class TDest,class...TArgs>
	void ConstructAs(TArgs cnLib_UREF...Args)noexcept(cnLib_NOEXCEPTEXPR(TDest(cnLib_UREFCAST(TArgs)(Args)...)))
	{
		cnLib_STATIC_ASSERT(sizeof(TDest)<=Size,"not enough size");
		ManualConstruct(*reinterpret_cast<TDest*>(this),cnLib_UREFCAST(TArgs)(Args)...);
	}

// cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L
#else
// cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES < 200704L
	template<class TDest>
	void Construct(void)noexcept(cnLib_NOEXCEPTEXPR(TDest()))
	{	ManualConstruct(*reinterpret_cast<TDest*>(this));	}
	template<class TDest,class TArg0>
	void Construct(TArg0 cnLib_UREF Arg0)
		noexcept(cnLib_NOEXCEPTEXPR(TDest(cnLib_UREFCAST(TArg0)(Arg0))))
	{
		ManualConstruct(*reinterpret_cast<TDest*>(this)
			,cnLib_UREFCAST(TArg0)(Arg0)
		);
	}
	template<class TDest,class TArg0,class TArg1>
	void Construct(TArg0 cnLib_UREF Arg0,TArg1 cnLib_UREF Arg1)
		noexcept(cnLib_NOEXCEPTEXPR(TDest(
			cnLib_UREFCAST(TArg0)(Arg0)
			,cnLib_UREFCAST(TArg1)(Arg1)
		)))
	{
		ManualConstruct(*reinterpret_cast<TDest*>(this)
			,cnLib_UREFCAST(TArg0)(Arg0)
			,cnLib_UREFCAST(TArg1)(Arg1)
		);
	}
	template<class TDest,class TArg0,class TArg1,class TArg2>
	void Construct(TArg0 cnLib_UREF Arg0,TArg1 cnLib_UREF Arg1,TArg2 cnLib_UREF Arg2)
		noexcept(cnLib_NOEXCEPTEXPR(TDest(
			cnLib_UREFCAST(TArg0)(Arg0)
			,cnLib_UREFCAST(TArg1)(Arg1)
			,cnLib_UREFCAST(TArg2)(Arg2)
		)))
	{
		ManualConstruct(*reinterpret_cast<TDest*>(this)
			,cnLib_UREFCAST(TArg0)(Arg0)
			,cnLib_UREFCAST(TArg1)(Arg1)
			,cnLib_UREFCAST(TArg2)(Arg2)
		);
	}
	template<class TDest,class TArg0,class TArg1,class TArg2,class TArg3>
	void Construct(TArg0 cnLib_UREF Arg0,TArg1 cnLib_UREF Arg1,TArg2 cnLib_UREF Arg2,TArg3 cnLib_UREF Arg3)
		noexcept(cnLib_NOEXCEPTEXPR(TDest(
			cnLib_UREFCAST(TArg0)(Arg0)
			,cnLib_UREFCAST(TArg1)(Arg1)
			,cnLib_UREFCAST(TArg2)(Arg2)
			,cnLib_UREFCAST(TArg3)(Arg3)
		)))
	{
		ManualConstruct(*reinterpret_cast<TDest*>(this)
			,cnLib_UREFCAST(TArg0)(Arg0)
			,cnLib_UREFCAST(TArg1)(Arg1)
			,cnLib_UREFCAST(TArg2)(Arg2)
			,cnLib_UREFCAST(TArg3)(Arg3)
		);
	}

#endif	// cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES < 200704L

	template<class TDest>
	void DestructAs(void)noexcept(cnLib_NOEXCEPTEXPR(cnVar::DeclVar<TDest&>().~TDest()))
	{
		cnLib_STATIC_ASSERT(sizeof(TDest)<=Size,"not enough size");
		ManualDestruct(*reinterpret_cast<TDest*>(this));
	}
};
//---------------------------------------------------------------------------

template<class TInterface,uIntn Size=sizeof(TInterface),class TAlignment=TInterface>
class cPolymorphicInterface : public cPolymorphicBlock<Size,TAlignment>
{
public:
	cnLib_STATIC_ASSERT(Size>=sizeof(TInterface),"error storage size");

	operator TInterface&()noexcept(true){					return *reinterpret_cast<TInterface*>		(this);	}
	operator const TInterface&()const noexcept(true){		return *reinterpret_cast<const TInterface*>	(this);	}
	TInterface* operator &()noexcept(true){					return reinterpret_cast<TInterface*>		(this);	}
	const TInterface* operator &()const noexcept(true){		return reinterpret_cast<const TInterface*>	(this);	}
	TInterface* operator ->()noexcept(true){				return reinterpret_cast<TInterface*>		(this);	}
	const TInterface* operator ->()const noexcept(true){	return reinterpret_cast<const TInterface*>	(this);	}
};

//---------------------------------------------------------------------------
template<uIntn Size,class TAlignment>
class cPolymorphicInterface<void,Size,TAlignment> : public cPolymorphicBlock<Size,TAlignment>
{
};

#if cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L

template<class TInterface,class...TImplementations>
using cPolymorphicObject = cPolymorphicInterface<TInterface,
		cnMath::TMax<tSize,sizeof(TImplementations)...>::Value,
		typename cnMemory::TSelectAlignmentType<TImplementations...>::Type
	>;
#endif	// cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L


//---------------------------------------------------------------------------
}	// namespace cnVar
//---------------------------------------------------------------------------
}   // namespace cnLibrary
//---------------------------------------------------------------------------
#if cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L
//---------------------------------------------------------------------------
namespace cnLib_THelper{
//---------------------------------------------------------------------------
namespace Var_TH{
//---------------------------------------------------------------------------
namespace VarPack{
//---------------------------------------------------------------------------

template<class...VT>		struct cVarPackStruct{ char n;	};	// some compiler need this usless n
template<>					struct cVarPackStruct<>{};
template<class T,class...VT>struct cVarPackStruct<T,VT...>
{
	T v;
	cVarPackStruct<VT...> n;
};


template<uIntn Offset,uIntn Index,class...VT>
struct TDataOffset;

template<uIntn Offset,uIntn Index,class T,class...VT>
struct TDataOffset<Offset,Index,T,VT...>
	: TDataOffset<Offset+reinterpret_cast<uIntn>(&static_cast<cVarPackStruct<T,VT...>*>(nullptr)->n),Index-1,VT...>
{
};

template<uIntn Offset,uIntn Index>	struct TDataOffset<Offset,Index>	: cnVar::TConstantValue<uIntn,Offset>{};
template<uIntn Offset,class...VT>	struct TDataOffset<Offset,0,VT...>	: cnVar::TConstantValue<uIntn,Offset>{};


template<uIntn Index>
struct FieldName
{
	static const uChar16 Value[8];
};

template<uIntn Index>
const uChar16 FieldName<Index>::Value[8]={
	u'M',u'V',u'A',u'R',u'0'+(Index/100)%10,u'0'+(Index/10)%10,u'0'+Index%10,0
};

template<class TDest,class TSrc>
struct Convertable 
	: cnVar::TConstantValueBool< sizeof(TDest) == sizeof(TSrc) && cnVar::template TIsConvertible<TSrc,TDest>::Value>
{};

template<bool,class...VTDest>
struct PackConvertable
{
	template<class...VTSrc>
	struct From{
		static constexpr bool Value=cnVar::TBooleanValuesAnd<Convertable<VTDest,VTSrc>::Value...>::Value;
	};

};

template<class...VTDest>
struct PackConvertable<false,VTDest...>
{
	template<class...VTSrc>
	struct From{
		static constexpr bool Value=false;
	};

};

//---------------------------------------------------------------------------
}	// namespace VarPack
//---------------------------------------------------------------------------
}	// namespace Var_TH
//---------------------------------------------------------------------------
}	// namespace cnLib_THelper
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnVar{
//---------------------------------------------------------------------------

template<class...VT>
struct cVarPackMetaClass
{
	typedef typename TMakeIndexSequence<TValueSequence<uIntn>,sizeof...(VT)>::Type tIndexSequence;
	static const cMetaClass MetaClass;
	static const cMetaClass *const Value;
};

template<class...>	class cVarPack;
template<>
class cVarPack<>
{
public:
	static cnLib_CONSTVAR uIntn Count=0;

};

template<class T>
class cVarPack<T>
{
protected:
	T fValue;
public:
	typedef T tValue;
	typedef cVarPack<> tNextPack;

	static cnLib_CONSTVAR uIntn Count=1;
	
	cnLib_CONSTEXPR_FUNC cVarPack()noexcept(true){}
	
#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L
	
	template<class TArg
#ifndef cnLibrary_CPPEXCLUDE_FUNCTION_TEMPLATE_DEFALT_ARGUMENT
		,class=typename TTypeConditional<void,TIsConstuctableBy<T,TArg&&>::Value>::Type
#endif
	>
	cVarPack(TArg &&Value)noexcept(cnLib_NOEXCEPTEXPR(T(Value)))
		: fValue(static_cast<TArg&&>(Value))
	{}
#else	// cnLibrary_CPPFEATURE_RVALUE_REFERENCES < 200610L
	template<class TArg>
	cVarPack(TArg Value)noexcept(cnLib_NOEXCEPTEXPR(T(Value)))
		: fValue(Value){}
#endif	// cnLibrary_CPPFEATURE_RVALUE_REFERENCES

	T& operator *()noexcept(true){	return fValue;	}
	const T& operator *()const noexcept(true){	return fValue;	}

	cVarPack<>& operator +(void)const noexcept(true){	return *reinterpret_cast<cVarPack<>*>(const_cast<cVarPack*>(this));	}

	template<class TDest,class TPack=typename TSelect<cnLib_THelper::Var_TH::VarPack::Convertable<TDest,T>::Value,void,cVarPack<TDest> >::Type&>
	operator cVarPack<TDest> &(void)noexcept(true){
		return reinterpret_cast<TPack>(*this);
	}
	template<class TDest,class TPack=typename TSelect<cnLib_THelper::Var_TH::VarPack::Convertable<TDest,T>::Value,void,const cVarPack<TDest> >::Type&>
	operator const cVarPack<TDest> &(void)const noexcept(true){
		return reinterpret_cast<TPack>(*this);
	}

	typename TRemoveReference<T>::Type* operator &()noexcept(true){			return cnMemory::AddressOf(fValue);	}
	const typename TRemoveReference<T>::Type* operator &()const noexcept(true){	return cnMemory::AddressOf(fValue);	}

	cAnyPtr operator [](uIntn Index)noexcept(true){
		if(Index==0){
			return cAnyPtr(TTypeID<T>::Value,this);
		}
		return nullptr;
	}

	typedef cVarPackMetaClass<T> tMetaClass;
};

template<class T,class...VT>
class cVarPack<T,VT...>
{
protected:
	T fValue;
	cVarPack<VT...> fNext;

public:
	typedef T tValue;
	typedef cVarPack<VT...> tNextPack;

	static cnLib_CONSTVAR uIntn Count=sizeof...(VT)+1;
	
	cnLib_CONSTEXPR_FUNC cVarPack()noexcept(true){}

#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

	template<class TArg,class...TArgs
#ifndef cnLibrary_CPPEXCLUDE_FUNCTION_TEMPLATE_DEFALT_ARGUMENT
		,class=typename TTypeConditional<void,TIsConstuctableBy<T,TArg&&>::Value && TIsConstuctableBy<cVarPack<VT...>,TArgs&&...>::Value>::Type
#endif
	>
	cVarPack(TArg&& Value,TArgs&&...Args)noexcept(cnLib_NOEXCEPTEXPR(T(static_cast<TArg&&>(Value))) && cnLib_NOEXCEPTEXPR(cVarPack<VT...>(static_cast<TArgs&&>(Args)...)))
		: fValue(static_cast<TArg&&>(Value))
		, fNext(static_cast<TArgs&&>(Args)...)
	{}
#else	// cnLibrary_CPPFEATURE_RVALUE_REFERENCES < 200610L
	template<class TArg,class...TArgs>
	cVarPack(TArg Value,TArgs...Args)
		: fValue(Value),fNext(Args...){}
#endif	// cnLibrary_CPPFEATURE_RVALUE_REFERENCES

	T& operator *()noexcept(true){	return fValue;	}
	const T& operator *()const noexcept(true){	return fValue;	}

	cVarPack<VT...>& operator +(void)noexcept(true){	return fNext;	}
	const cVarPack<VT...>& operator +(void)const noexcept(true){	return fNext;	}
	
	typename tNextPack::tNextPack& operator ++(void)noexcept(true){	return +fNext;	}
	typename tNextPack::tNextPack const& operator ++(void)const noexcept(true){	return +fNext;	}

	
	template<class...VTDest,class TPack=typename TSelect<cnLib_THelper::Var_TH::VarPack::PackConvertable<sizeof...(VT)+1==sizeof...(VTDest),VTDest...>::template From<T,VT...>::Value,void,cVarPack<VTDest...> >::Type&>
	operator cVarPack<VTDest...> &(void)noexcept(true){
		return reinterpret_cast<TPack>(*this);
	}
	template<class...VTDest,class TPack=typename TSelect<cnLib_THelper::Var_TH::VarPack::PackConvertable<sizeof...(VT)+1==sizeof...(VTDest),VTDest...>::template From<T,VT...>::Value,void,const cVarPack<VTDest...> >::Type&>
	operator const cVarPack<VTDest...> &(void)const noexcept(true){
		return reinterpret_cast<TPack>(*this);
	}

	typename TRemoveReference<T>::Type* operator &()noexcept(true){			return cnMemory::AddressOf(fValue);	}
	const typename TRemoveReference<T>::Type* operator &()const noexcept(true){	return cnMemory::AddressOf(fValue);	}
	

	cAnyPtr operator [](uIntn Index)noexcept(true){
		if(Index==0){
			return cAnyPtr(TTypeID<T>::Value,cnMemory::AddressOf(fValue));
		}
		return fNext[Index-1];
	}

	typedef cVarPackMetaClass<T,VT...> tMetaClass;
};

template<class T,T...Values>
template<class...VT>
struct TValueSequence<T,Values...>::Declarator< cVarPack<VT...> >
{
	static const cMetaClassField Fields[sizeof...(VT)];
};

template<class T,T...Indexes>
template<class...VT>
const cMetaClassField TValueSequence<T,Indexes...>::Declarator< cVarPack<VT...> >::Fields[sizeof...(VT)]={
	{
		cnLib_THelper::Var_TH::VarPack::FieldName<Indexes>::Value,			//const uChar16 *Name;
		TRuntimeInfo<VT>::Value,											//rtInfo RuntimeInfo;
		cnLib_THelper::Var_TH::VarPack::TDataOffset<0,Indexes,VT...>::Value,	//uIntn Offset;
	}...
};

template<class...VT>
const cMetaClass cVarPackMetaClass<VT...>::MetaClass={
	sizeof...(VT),								//ufInt16 FieldCount;
	0,											//ufInt16 FunctionCount;
	tIndexSequence::template Declarator< cVarPack<VT...> >::Fields,	//const cMetaClassField *Fields;
	nullptr,									//const cMetaClassFunction *Functions;
};

template<class...VT>
const cMetaClass*const cVarPackMetaClass<VT...>::Value=&cVarPackMetaClass<VT...>::MetaClass;


//---------------------------------------------------------------------------
}	// namespace cnVar
//---------------------------------------------------------------------------
}   // namespace cnLibrary
//---------------------------------------------------------------------------
namespace cnLib_THelper{
//---------------------------------------------------------------------------
namespace Var_TH{
//---------------------------------------------------------------------------
namespace VarPack{


template<uIntn Index,class...VT>
struct TSelectPack;

template<uIntn Index>
struct TSelectPack<Index>
{
};

template<uIntn Index,class T,class...VT>
struct TSelectPack<Index,T,VT...> : TSelectPack<Index-1,VT...>
{
	static typename TSelectPack<Index-1,VT...>::Type& Posit(cnVar::cVarPack<T,VT...> &Pack)noexcept(true)
	{
		return TSelectPack<Index-1,VT...>::Posit(+Pack);
	}
	static typename TSelectPack<Index-1,VT...>::Type const& Posit(const cnVar::cVarPack<T,VT...> &Pack)noexcept(true)
	{
		return TSelectPack<Index-1,VT...>::Posit(+Pack);
	}
};
template<class T,class...VT>
struct TSelectPack<0,T,VT...>
{
	typedef T Type;

	static T& Posit(cnVar::cVarPack<T,VT...> &Pack)noexcept(true)
	{
		return *Pack;
	}
	static const T& Posit(const cnVar::cVarPack<T,VT...> &Pack)noexcept(true)
	{
		return *Pack;
	}
};


//---------------------------------------------------------------------------
}	// namespace VarPack
//---------------------------------------------------------------------------
}	// namespace Var_TH
//---------------------------------------------------------------------------
}	// namespace cnLib_THelper
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnVar{
//---------------------------------------------------------------------------

template<uIntn Index,class...VT>
typename TSelect<Index,VT...>::Type& VarPackAt(cVarPack<VT...> &Pack)noexcept(true)
{
	return cnLib_THelper::Var_TH::VarPack::TSelectPack<Index,VT...>::Posit(Pack);
}

template<uIntn Index,class...VT>
typename TSelect<Index,VT...>::Type const& VarPackAt(const cVarPack<VT...> &Pack)noexcept(true)
{
	return cnLib_THelper::Var_TH::VarPack::TSelectPack<Index,VT...>::Posit(Pack);
}

//---------------------------------------------------------------------------
#ifndef cnLibrary_CPPEXCLUDE_NOEXCEPT

template<class TFunc,class...TVarPacks>
struct TVarPackFunctionIsNoexcept
	: TSelect<noexcept(DeclVar<TFunc>()(DeclVar<typename TVarPacks::tValue>()...))
		,TConstantValueFalse
		,TVarPackFunctionIsNoexcept<TFunc,typename TVarPacks::tNextPack...>
	>::Type{};

template<class TFunc,class...TVarPacks>
struct TVarPackFunctionIsNoexcept<TFunc,cVarPack<>,TVarPacks...>
	: TConstantValueTrue{};

// !cnLibrary_CPPEXCLUDE_NOEXCEPT
#else
// cnLibrary_CPPEXCLUDE_NOEXCEPT

template<class TFunc,class...TVarPacks>
struct TVarPackFunctionIsNoexcept
	: TConstantValueFalse{};


#endif // cnLibrary_CPPEXCLUDE_NOEXCEPT
//---------------------------------------------------------------------------

template<class TFunc,class...VT>	bool VarPackCallAt(uIntn,const TFunc &,cVarPack<>&,VT...){	return true;	}
template<class TFunc,class...TVarPacks>
bool VarPackCallAt(uIntn DestIndex,const TFunc &Func,TVarPacks&...VarPacks)noexcept((TVarPackFunctionIsNoexcept<TFunc,TVarPacks...>::Value))
{
	if(DestIndex==0){
		Func((*VarPacks)...);
		return true;
	}
	return VarPackCallAt(DestIndex-1,Func,+VarPacks...);
}
//---------------------------------------------------------------------------
template<class...VT>
cVarPack<VT...> VarPack(VT cnLib_UREF...v)noexcept(true)
{
	return {cnLib_UREFCAST(VT)(v)...};
}
//---------------------------------------------------------------------------
//template<class T> inline T DeclVarParameter(T v)noexcept(true);
//template<class...VT>
//auto VarPack(VT cnLib_UREF...v)noexcept(true)	->cVarPack<decltype(cnLib_THelper::Var_TH::VarPack::DeclVarParameter(v))...>
//{
//	return {cnLib_UREFCAST(VT)(v)...};
//}
//---------------------------------------------------------------------------
template<class...VT>
auto VarRefPack(VT &...v)noexcept(true)	->cVarPack<VT&...>
{
	return {v...};
}
//---------------------------------------------------------------------------
template<class TFunc,class...VT>	void VarPackForEach(const TFunc &,cVarPack<>&,VT...)noexcept(true){}
template<class TFunc,class...TVarPacks>
void VarPackForEach(const TFunc &Func,TVarPacks&...VarPacks)noexcept((TVarPackFunctionIsNoexcept<TFunc,TVarPacks...>::Value))
{
	Func((*VarPacks)...);
	return VarPackForEach(Func,+VarPacks...);
}
//---------------------------------------------------------------------------
template<class TFunc,class...VT>	bool VarPackForWhile(const TFunc &,cVarPack<>&,VT...)noexcept(true){	return true;	}
template<class TFunc,class...TVarPacks>
bool VarPackForWhile(const TFunc &Func,TVarPacks&...VarPacks)noexcept((TVarPackFunctionIsNoexcept<TFunc,TVarPacks...>::Value))
{
	if(Func((*VarPacks)...)==false)
		return false;
	return VarPackForWhile(Func,+VarPacks...);
}
//---------------------------------------------------------------------------
}	// namespace cnVar
//---------------------------------------------------------------------------
}   // namespace cnLibrary
//---------------------------------------------------------------------------
#endif	// cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnVar{
//---------------------------------------------------------------------------

//TVariantTypeOperator
//{
//	typedef tTypeIndex;
//
//	template<tTypeIndex TypeIndex>
//	struct tTypeByIndex;
//
//	static constexpr bool IsDefaultConstructNoexcept;
//	static constexpr bool IsDestructNoexcept;
//	static constexpr bool IsCopyConstructNoexcept;
//	static constexpr bool IsMoveConstructNoexcept;
//	static constexpr bool IsCopyAssignNoexcept;
//	static constexpr bool IsMoveAssignNoexcept;
//
//	static tTypeID GetTypeID(tTypeIndex TypeIndex);
//	static rtType GetTypeInfo(tTypeIndex TypeIndex);
//
//	static constexpr tTypeIndex DefaultTypeIndex;
//	
//	static constexpr uIntn UnionSize;
//	typedef tUnionAlignment;
//};

template<class TVariantTypeOperator>
class cVariant
{
public:
	typedef typename TVariantTypeOperator::tTypeIndex tTypeIndex;

	template<tTypeIndex TypeIndex>	typename TVariantTypeOperator::template tTypeByIndex<TypeIndex>::Type* Pointer(void)noexcept(true)
	{	return static_cast<typename TVariantTypeOperator::template tTypeByIndex<TypeIndex>::Type*>(&fContent);	}
	template<tTypeIndex TypeIndex>	const typename TVariantTypeOperator::template tTypeByIndex<TypeIndex>::Type* Pointer(void)const noexcept(true)
	{	return static_cast<typename TVariantTypeOperator::template tTypeByIndex<TypeIndex>::Type*>(&fContent);	}

	cVariant()noexcept(TIsDefaultConstructNoexcept<typename TVariantTypeOperator::template tTypeByIndex<TVariantTypeOperator::DefaultTypeIndex>::Type>::Value)
		: fTypeIndex(TVariantTypeOperator::DefaultTypeIndex)
	{
		ManualConstruct(reinterpret_cast<typename TVariantTypeOperator::template tTypeByIndex<TVariantTypeOperator::DefaultTypeIndex>::Type&>(fContent));
	}
	~cVariant()noexcept(TVariantTypeOperator::IsDestructNoexcept){
		rtType TypeInfo=TVariantTypeOperator::GetTypeInfo(fTypeIndex);
		TypeInfo->Operators->Destruct(&fContent);
	}

	cVariant(const cVariant &Src)noexcept(TVariantTypeOperator::IsCopyConstructNoexcept){
		fTypeIndex=Src.fTypeIndex;
		rtType TypeInfo=TVariantTypeOperator::GetTypeInfo(fTypeIndex);
		TypeInfo->Operators->CopyConstruct(&fContent,&Src.fContent);
	}
	
#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L
	cVariant(cVariant &&Src)noexcept(TVariantTypeOperator::IsMoveConstructNoexcept){
		fTypeIndex=Src.fTypeIndex;
		rtType TypeInfo=TVariantTypeOperator::GetTypeInfo(fTypeIndex);
		TypeInfo->Operators->MoveConstruct(&fContent,&Src.fContent);
	}
#endif

	cVariant& operator = (const cVariant &Src)noexcept(
		TVariantTypeOperator::IsCopyConstructNoexcept
		&& TVariantTypeOperator::IsCopyAssignNoexcept
		&& TVariantTypeOperator::IsDestructNoexcept
	){
		if(this==&Src)
			return *this;
		rtType TypeInfo=TVariantTypeOperator::GetTypeInfo(fTypeIndex);
		rtType NewTypeInfo;
		if(fTypeIndex==Src.fTypeIndex || TypeInfo==(NewTypeInfo=TVariantTypeOperator::GetTypeInfo(Src.fTypeIndex))){
			if(TypeInfo->IsCopyAssignable){
				TypeInfo->Operators->Copy(&fContent,&Src.fContent);
			}
			else{
				TypeInfo->Destruct(&fContent);
				TypeInfo->Operators->CopyConstruct(&fContent,&Src.fContent);
			}
		}
		else{
			TypeInfo->Operators->Destruct(&fContent);
			fTypeIndex=Src.fTypeIndex;
			NewTypeInfo->Operators->CopyConstruct(&fContent,&Src.fContent);
		}
		return *this;
	}

#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

	cVariant& operator = (cVariant &&Src)noexcept(TVariantTypeOperator::IsMoveConstructNoexcept && TVariantTypeOperator::IsMoveAssignNoexcept && TVariantTypeOperator::IsDestructNoexcept){
		rtType TypeInfo=TVariantTypeOperator::GetTypeInfo(fTypeIndex);
		rtType NewTypeInfo;
		if(fTypeIndex==Src.fTypeIndex || TypeInfo==(NewTypeInfo=TVariantTypeOperator::GetTypeInfo(Src.fTypeIndex))){
			if(TypeInfo->IsMoveAssignable){
				TypeInfo->Operators->Move(&fContent,&Src.fContent);
			}
			else{
				TypeInfo->Destruct(&fContent);
				TypeInfo->Operators->MoveConstruct(&fContent,&Src.fContent);
			}
		}
		else{
			TypeInfo->Operators->Destruct(&fContent);
			fTypeIndex=Src.fTypeIndex;
			NewTypeInfo->Operators->MoveConstruct(&fContent,&Src.fContent);
		}
		return *this;
	}
#endif // cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

	// reference directly
	template<tTypeIndex TypeIndex>	typename TVariantTypeOperator::template tTypeByIndex<TypeIndex>::Type& As(void)
		noexcept(TVariantTypeOperator::IsDefaultConstructNoexcept && TVariantTypeOperator::IsDestructNoexcept)
	{
		if(fTypeIndex!=TypeIndex){
			rtType CurTypeInfo=TVariantTypeOperator::GetTypeInfo(fTypeIndex);
			rtType TypeInfo=TRuntimeTypeInfo<typename TVariantTypeOperator::template tTypeByIndex<TypeIndex>::Type>::Value;
			if(CurTypeInfo!=TypeInfo){
				CurTypeInfo->Operator->Destruct(&fContent);
				fTypeIndex=TypeIndex;
				TypeInfo->Operators->Construct(&fContent);
			}
		}
		return *Pointer<TypeIndex>();
	}	

	// Ptr
	//	point to specified type
	//	data of previous type should be overwritten
	template<tTypeIndex TypeIndex>
	typename TVariantTypeOperator::template tTypeByIndex<TypeIndex>::Type* Ptr(void)noexcept(true){
		if(fTypeIndex==TypeIndex){
			return Pointer<TypeIndex>();
		}
		return nullptr;
	}
	// Ptr
	//	point to specified type
	template<tTypeIndex TypeIndex>
	typename TVariantTypeOperator::template tTypeByIndex<TypeIndex>::Type const* Ptr(void)const noexcept(true){
		if(fTypeIndex==TypeIndex){
			return Pointer<TypeIndex>();
		}
		return nullptr;
	}

	tTypeIndex GetTypeIndex(void)const noexcept(true){
		return fTypeIndex;
	}
	
	void SetTypeIndex(tTypeIndex NewTypeIndex)noexcept(TVariantTypeOperator::IsDefaultConstructNoexcept && TVariantTypeOperator::IsDestructNoexcept){
		if(fTypeIndex==NewTypeIndex)
			return;

		rtType CurTypeInfo=TVariantTypeOperator::GetTypeInfo(fTypeIndex);
		rtType NewTypeInfo=TVariantTypeOperator::GetTypeInfo(NewTypeIndex);
		fTypeIndex=NewTypeIndex;
		if(CurTypeInfo!=NewTypeInfo){
			CurTypeInfo->Operator->Destruct(&fContent);
			NewTypeInfo->Operator->Construct(&fContent);
		}
	}

	template<tTypeIndex TypeIndex>
	bool Get(typename TVariantTypeOperator::template tTypeByIndex<TypeIndex>::Type &Dest)const noexcept(true){
		if(fTypeIndex!=TypeIndex)
			return false;
		Dest=*Pointer<TypeIndex>();
		return true;
	}

	// assignment

	template<tTypeIndex NewTypeIndex,class TArg>
	void Set(TArg cnLib_UREF Arg)noexcept(
		cnLib_NOEXCEPTEXPR(AssignOrReconstruct(reinterpret_cast<TNewType&>(fContent),cnLib_UREFCAST(TArg)(Arg)))
		&& cnLib_NOEXCEPTEXPR(typename TVariantTypeOperator::template tTypeByIndex<NewTypeIndex>::Type(cnLib_UREFCAST(TArg)(Arg)))
		&& TVariantTypeOperator::IsDestructNoexcept
	){
		typedef typename TVariantTypeOperator::template tTypeByIndex<NewTypeIndex>::Type TNewType;
		if(NewTypeIndex==fTypeIndex){
			AssignOrReconstruct(reinterpret_cast<TNewType&>(fContent),cnLib_UREFCAST(TArg)(Arg));
		}
		else{
			rtType TypeInfo=TVariantTypeOperator::GetTypeInfo(fTypeIndex);
			if(TypeInfo==TRuntimeTypeInfo<TNewType>::Value){
				AssignOrReconstruct(reinterpret_cast<TNewType&>(fContent),cnLib_UREFCAST(TArg)(Arg));
			}
			else{
				TypeInfo->Operators->Destruct(&fContent);
				fTypeIndex=NewTypeIndex;
				ManualConstruct(reinterpret_cast<TNewType&>(fContent),cnLib_UREFCAST(TArg)(Arg));
			}
		}
	}

#if cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L

	template<tTypeIndex NewTypeIndex,class...TArgs>
	void Set(TArgs cnLib_UREF...Args)noexcept(
		cnLib_NOEXCEPTEXPR(typename TVariantTypeOperator::template tTypeByIndex<NewTypeIndex>::Type>::Value(cnLib_UREFCAST(TArgs)(Args)...))
		&& TVariantTypeOperator::IsDestructNoexcept
	){
		typedef typename TVariantTypeOperator::template tTypeByIndex<NewTypeIndex>::Type TNewType;
		if(NewTypeIndex==fTypeIndex){
			ManualDestruct(reinterpret_cast<TNewType&>(fContent));
		}
		else{
			rtType TypeInfo=TVariantTypeOperator::GetTypeInfo(fTypeIndex);
			TypeInfo->Operators->Destruct(&fContent);
			fTypeIndex=NewTypeIndex;
		}
		ManualConstruct(reinterpret_cast<TNewType&>(fContent),cnLib_UREFCAST(TArgs)(Args)...);
	}
#endif	//	cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L

	/*
	template<class T>
	bool IsConvertible(void)const{
		cnLib_THelper::Var_TH::Variant::IsConvertibleTo<T> fo={false};
		CallByIndex(Index,fo);
		return fo.r;
	}

	template<class T>
	bool Cast(T &Dest)const{
		cnLib_THelper::Var_TH::Variant::Cast<T> fo={Dest};
		CallByIndex(Index,fo);
		return fo.r;
	}

	template<class TFunc>
	void CallByType(TFunc cnLib_UREF Func){
		return cnLib_THelper::Var_TH::Variant::CallByIndex<TFunc,VT...>(Index,&Content,Func);
	}
	template<class TFunc>
	void CallByType(TFunc cnLib_UREF Func)const{
		return cnLib_THelper::Var_TH::Variant::CallByIndex<TFunc,VT...>(Index,&Content,Func);
	}
	*/
private:
	tTypeIndex fTypeIndex;
	cnMemory::cPlainAlignedData<TVariantTypeOperator::UnionSize,typename TVariantTypeOperator::tUnionAlignment> fContent;
	/*
	template<class TFunc>
	void CallByIndex(uInt8 CallIndex,TFunc cnLib_UREF Func){
		return cnLib_THelper::Var_TH::Variant::CallByIndex<TFunc,VT...>(CallIndex,&Content,Func);
	}
	template<class TFunc>
	void CallByIndex(uInt8 CallIndex,TFunc cnLib_UREF Func)const{
		return cnLib_THelper::Var_TH::Variant::CallByIndex<TFunc,VT...>(CallIndex,&Content,Func);
	}
	*/
};

//---------------------------------------------------------------------------
}	// namespace cnVar
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#if cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L
//---------------------------------------------------------------------------
namespace cnLib_THelper{
//---------------------------------------------------------------------------
namespace Var_TH{
//---------------------------------------------------------------------------

template<uIntn Size,class TAlignment>
struct cAnyVariableContent_Value
{
	cnVar::cPolymorphicBlock<Size,TAlignment> Content;
	rtType TypeInfo;

	template<class T>
	T* Ptr(void){
		return static_cast<T*>(&this->Content);
	}

	template<class T>
	const T* Ptr(void)const{
		return static_cast<const T*>(&this->Content);
	}

	void Construct(void){
		this->TypeInfo->Operators->Construct(&this->Content);
	}
	void Destruct(void){
		this->TypeInfo->Operators->Destruct(&this->Content);
	}

	template<class T,class...TArgs>
	void ConstructAs(TArgs cnLib_UREF...Args){
		Content.template ConstructAs<T>(cnLib_UREFCAST(TArgs)(Args)...);
	}

	// reconstruct
	template<class T,class...TArgs>
	void Reconstruct(TArgs cnLib_UREF...Args){
		this->Content.template DestructAs<T>();
		this->Content.template ConstructAs<T>(cnLib_UREFCAST(TArgs)(Args)...);
	}

	template<class T,class TSrc>
	void SetVar(TSrc cnLib_UREF Src){
		cnVar::AssignOrReconstruct(reinterpret_cast<T&>(this->Content),cnLib_UREFCAST(TSrc)(Src));
	}

	template<uIntn SrcSize,class TSrcAlignment>
	void CopyConstructValue(const cAnyVariableContent_Value<SrcSize,TSrcAlignment> &Src){
		this->TypeInfo->Operators->CopyConstruct(&this->Content,&Src.Content);
	}

	template<uIntn SrcSize,class TSrcAlignment>
	void MoveConstructValue(cAnyVariableContent_Value<SrcSize,TSrcAlignment> &Src){
		this->TypeInfo->Operators->MoveConstruct(&this->Content,&Src.Content);
	}

	template<uIntn SrcSize,class TSrcAlignment>
	void CopyAssignSameType(const cAnyVariableContent_Value<SrcSize,TSrcAlignment> &Src){
		if(this->TypeInfo->IsCopyAssignable){
			this->TypeInfo->Operators->Copy(&this->Content,&Src.Content);
		}
		else{
			this->TypeInfo->Operators->Destruct(&this->Content);
			this->TypeInfo->Operators->CopyConstruct(&this->Content,&Src.Content);
		}
	}
	template<uIntn SrcSize,class TSrcAlignment>
	void MoveAssignSameType(cAnyVariableContent_Value<SrcSize,TSrcAlignment> &Src){
		if(this->TypeInfo->IsMoveAssignable){
			this->TypeInfo->Operators->Move(&this->Content,&Src.Content);
		}
		else{
			this->TypeInfo->Operators->Destruct(&this->Content);
			this->TypeInfo->Operators->MoveConstruct(&this->Content,&Src.Content);
		}
	}

};
template<uIntn Size,class TAlignment>
struct cAnyVariableContent_Pointer
{
	cnVar::cPolymorphicInterface<void*,Size,TAlignment> Pointer;
	rtType TypeInfo;

	template<class T>
	T* Ptr(void){
		return static_cast<T*>(static_cast<void*>(this->Pointer));
	}

	template<class T>
	const T* Ptr(void)const{
		return static_cast<const T*>(static_cast<const void*>(this->Pointer));
	}

	void Construct(void){
		static_cast<void*&>(this->Pointer)=this->TypeInfo->Operators->New();
		this->TypeInfo->Operators->Construct(this->Pointer);
	}

	void Destruct(void){
		this->TypeInfo->Operators->Destruct(this->Pointer);
		this->TypeInfo->Operators->Delete(this->Pointer);
	}

	template<class T,class...TArgs>
	void ConstructAs(TArgs cnLib_UREF...Args){
		reinterpret_cast<T*&>(this->Pointer)=new T(cnLib_UREFCAST(TArgs)(Args)...);
	}
	         
	// reconstruct
	template<class T,class...TArgs>
	void Reconstruct(TArgs cnLib_UREF...Args){
		cnVar::ManualDestruct<T>(*static_cast<T*>(this->Pointer));
		cnVar::ManualConstruct<T>(*static_cast<T*>(this->Pointer),cnLib_UREFCAST(TArgs)(Args)...);
	}

	template<class T,class TSrc>
	void SetVar(TSrc cnLib_UREF Src){
		cnVar::AssignOrReconstruct(*reinterpret_cast<T*&>(this->Pointer),cnLib_UREFCAST(TSrc)(Src));
	}

	
	template<uIntn SrcSize,class TSrcAlignment>
	void CopyConstructValue(const cAnyVariableContent_Pointer<SrcSize,TSrcAlignment> &Src){
		static_cast<void*&>(this->Pointer)=TypeInfo->Operators->New();
		this->TypeInfo->Operators->CopyConstruct(this->Pointer,Src.Pointer);
	}
	template<uIntn SrcSize,class TSrcAlignment>
	void MoveConstructValue(cAnyVariableContent_Pointer<SrcSize,TSrcAlignment> &Src){
		static_cast<void*&>(this->Pointer)=Src.Pointer;
		Src.TypeInfo=cnVar::TRuntimeTypeInfo<void>::Value;
	}


	template<uIntn SrcSize,class TSrcAlignment>
	void CopyAssignSameType(const cAnyVariableContent_Pointer<SrcSize,TSrcAlignment> &Src){
		if(this->TypeInfo->IsCopyAssignable){
			this->TypeInfo->Operators->Copy(this->Pointer,Src.Pointer);
		}
		else{
			this->TypeInfo->Operators->Destruct(this->Pointer);
			this->TypeInfo->Operators->CopyConstruct(this->Pointer,Src.Pointer);
		}
	}


	template<uIntn SrcSize,class TSrcAlignment>
	void MoveAssignSameType(const cAnyVariableContent_Pointer<SrcSize,TSrcAlignment> &Src){
		if(this->TypeInfo->IsMoveAssignable){
			this->TypeInfo->Operators->Move(this->Pointer,Src.Pointer);
		}
		else{
			this->TypeInfo->Operators->Destruct(this->Pointer);
			this->TypeInfo->Operators->MoveConstruct(this->Pointer,Src.Pointer);
		}
	}

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

template<uIntn Size,class TAlignment>
class cAnyVariable
{
public:

	cAnyVariable(){		fValueContent.TypeInfo=TRuntimeTypeInfo<void>::Value;	}
	~cAnyVariable(){
		if(IsPointerContent()){
			fPointerContent.Destruct();
		}
		else{
			fValueContent.Destruct();
		}
	}
	
	cAnyVariable(const cAnyVariable &Src){	CopyConstructA(Src);	}
#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L
	cAnyVariable(cAnyVariable &&Src){			CopyConstructA(static_cast<cAnyVariable &&>(Src));	}
#endif

	template<class T>
	cAnyVariable(T cnLib_UREF Src){
		cSelector<typename TRemoveCVRef<T>::Type>::CopyConstruct(this,cnLib_UREFCAST(T)(Src));
	}

	cAnyVariable& operator = (const cAnyVariable &Src){
		if(this==&Src)
			return *this;

		AssignA(Src);
		return *this;
	}
	
#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L
	cAnyVariable& operator = (cAnyVariable &&Src){
		AssignA(static_cast<cAnyVariable&&>(Src));
		return *this;
	}
#endif
	// assignment operator
	template<class T>
	cAnyVariable& operator = (T cnLib_UREF Src){
		cSelector<typename TRemoveCVRef<T>::Type>::Assign(this,cnLib_UREFCAST(T)(Src));
		return *this;
	}

	// reference directly
	template<class T>
	T& Ref(void){
		auto &Content=ContentSelector<T>();
		return *Content.template Ptr<T>();
	}
	template<class T>
	const T& Ref(void)const{
		auto &Content=ContentSelector<T>();
		return *Content.template Ptr<T>();
	}
	
	template<class T>	explicit operator T& (){			return Ref<T>();	}
	template<class T>	explicit operator const T& ()const{	return Ref<T>();	}
	// Ptr
	//	point to specified type
	//	data of previous type should be overwritten
	template<class T>
	T* Ptr(void){
		if(IsType<T>()==false)
			return nullptr;
		auto &Content=ContentSelector<T>();
		return Content.template Ptr<T>();
	}
	// Ptr
	//	point to specified type
	template<class T>
	const T* Ptr(void)const{
		if(IsType<T>()==false)
			return nullptr;
		auto &Content=ContentSelector<T>();
		return Content.template Ptr<T>();
	}

	// IsType
	//	Check if it is type T
	template<class T>
	bool IsType(void)const{
		return fValueContent.TypeInfo==TRuntimeTypeInfo<T>::Value;
	}
	template<class T>
	void SetType(void){
		return SetTypeInfo(TRuntimeTypeInfo<T>::Value);
	}

	rtType GetTypeInfo(void)const{
		return fValueContent.TypeInfo;
	}
	
	void SetTypeInfo(rtType NewTypeInfo){
		if(fValueContent.TypeInfo==NewTypeInfo)
			return;

		if(IsPointerContent()){
			fPointerContent.Destruct();
		}
		else{
			fValueContent.Destruct();
		}
		fValueContent.TypeInfo=NewTypeInfo;
		if(IsPointerContent(NewTypeInfo)){
			fPointerContent.Construct();
		}
		else{
			fValueContent.Construct();
		}
	}

	template<class T>
	bool Get(T &Dest)const{
		if(IsType<T>()==false)
			return false;
		auto &Content=ContentSelector<T>();
		Dest=*Content.template Ptr<T>();
		return true;
	}

	// assignment
	template<class T=void,class TSrc>
	void Set(TSrc cnLib_UREF Src){
		typedef typename TSelect<TIsSame<T,void>::Value,
			T,
			typename TRemoveCVRef<TSrc>::Type
		>::Type TVal;

		rtType NewTypeInfo=TRuntimeTypeInfo<TVal>::Value;
		if(NewTypeInfo==fValueContent.TypeInfo){
			auto &Content=ContentSelector<TVal>();
			Content.template SetVar<TVal>(cnLib_UREFCAST(TSrc)(Src));
		}
		else{
			if(IsPointerContent()){
				fPointerContent.Destruct();
			}
			else{
				fValueContent.Destruct();
			}
			auto &Content=ContentSelector<TVal>();
			Content.TypeInfo=NewTypeInfo;
			Content.template ConstructAs<TVal>(cnLib_UREFCAST(TSrc)(Src));
		}
	}
	// reconstruct
	template<class T,class...TArgs>
	void Set(TArgs cnLib_UREF...Args){
		rtType NewTypeInfo=TRuntimeTypeInfo<T>::Value;
		if(NewTypeInfo==fValueContent.TypeInfo){
			auto &Content=ContentSelector<T>();
			Content.template Reconstruct<T>(cnLib_UREFCAST(TArgs)(Args)...);
		}
		else{
			if(IsPointerContent()){
				fPointerContent.Destruct();
			}
			else{
				fValueContent.Destruct();
			}
			auto &Content=ContentSelector<T>();
			Content.TypeInfo=NewTypeInfo;
			Content.template ConstructAs<T>(cnLib_UREFCAST(TArgs)(Args)...);
		}
	}
	
private:
	template<uIntn,class> friend class cnLibrary::cnVar::cAnyVariable;

	static constexpr uIntn StorageSize=Size>sizeof(void*)?Size:sizeof(void*);
	typedef cnLib_THelper::Var_TH::cAnyVariableContent_Value<StorageSize,TAlignment> TValueContent;
	typedef cnLib_THelper::Var_TH::cAnyVariableContent_Pointer<StorageSize,TAlignment> TPointerContent;
	cnLib_STATIC_ASSERT(sizeof(TValueContent)==sizeof(TPointerContent),"internal error");

	union{
		TValueContent fValueContent;
		TPointerContent fPointerContent;
	};

	template<class T>
	struct cContentSelector 
		: TSelect< StorageSize<sizeof(T),
			TValueContent,
			TPointerContent
		>{};

	template<class T>
	typename cContentSelector<T>::Type& ContentSelector(void){
		return reinterpret_cast<typename cContentSelector<T>::Type&>(fValueContent);
	}
	template<class T>
	const typename cContentSelector<T>::Type& ContentSelector(void)const{
		return reinterpret_cast<const typename cContentSelector<T>::Type&>(fValueContent);
	}

	template<class T>
	static bool NeedPointer(void){
		return StorageSize<sizeof(T);
	}
	static bool IsPointerContent(rtType TypeInfo){
		return TypeInfo->ID->Size>StorageSize;
	}
	bool IsPointerContent(void)const{
		return fValueContent.TypeInfo->ID->Size>StorageSize;
	}

	template<uIntn sSize,class sTAlignment>
	void CopyConstructA(const cAnyVariable<sSize,sTAlignment> &Src){
		fValueContent.TypeInfo=Src.fValueContent.TypeInfo;
		if(IsPointerContent(Src.fValueContent.TypeInfo)){
			fPointerContent.CopyConstructValue(Src.fPointerContent);
		}
		else{
			fValueContent.CopyConstructValue(Src.fValueContent);
		}
	}

#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

	template<uIntn sSize,class sTAlignment>
	void CopyConstructA(cAnyVariable<sSize,sTAlignment> &&Src){
		fValueContent.TypeInfo=Src.fValueContent.TypeInfo;
		if(IsPointerContent(Src.fValueContent.TypeInfo)){
			fPointerContent.MoveConstructValue(Src.fPointerContent);
		}
		else{
			fValueContent.MoveConstructValue(Src.fValueContent);
		}
	}
#endif // cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

	template<uIntn sSize,class sTAlignment>
	void AssignA(const cAnyVariable<sSize,sTAlignment> &Src){
		bool ThisIsPointer=IsPointerContent();
		if(Src.fValueContent.TypeInfo==fValueContent.TypeInfo){
			if(ThisIsPointer){
				fPointerContent.CopyAssignSameType(Src.fPointerContent);
			}
			else{
				fValueContent.CopyAssignSameType(Src.fValueContent);
			}
		}
		else{
			if(ThisIsPointer){
				fPointerContent.Destruct();
			}
			else{
				fValueContent.Destruct();
			}
			fValueContent.TypeInfo=Src.fValueContent.TypeInfo;

			CopyConstructA(Src);
		}
	}

#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L
	template<uIntn sSize,class sTAlignment>
	void AssignA(cAnyVariable<sSize,sTAlignment> &&Src){
		bool ThisIsPointer=IsPointerContent();
		if(Src.fValueContent.TypeInfo==fValueContent.TypeInfo){
			if(ThisIsPointer){
				fPointerContent.MoveAssignSameType(Src.fPointerContent);
			}
			else{
				fValueContent.MoveAssignSameType(Src.fValueContent);
			}
		}
		else{
			if(ThisIsPointer){
				fPointerContent.Destruct();
			}
			else{
				fValueContent.Destruct();
			}
			fValueContent.TypeInfo=Src.fValueContent.TypeInfo;

			if(IsPointerContent(Src.fValueContent.TypeInfo)){
				fPointerContent.MoveConstructValue(Src.fPointerContent);
			}
			else{
				fValueContent.MoveConstructValue(Src.fValueContent);
			}
		}
	}
#endif // cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

	template<class T,class TSrc>
	void CopyConstructT(TSrc cnLib_UREF Src){
		auto &Content=ContentSelector<T>();
		Content.TypeInfo=TRuntimeTypeInfo<T>::Value;
		Content.template ConstructAs<T>(cnLib_UREFCAST(TSrc)(Src));
	}

	struct cFunction_cAnyVariable
	{
		template<class TSrc>
		static void CopyConstruct(cAnyVariable *pThis,TSrc cnLib_UREF Src){
			return pThis->CopyConstructA(cnLib_UREFCAST(TSrc)(Src));
		}
		template<class TSrc>
		static void Assign(cAnyVariable *pThis,TSrc cnLib_UREF Src){
			return pThis->AssignA(cnLib_UREFCAST(TSrc)(Src));
		}
	};
	template<class T>
	struct cFunction_T
	{
		template<class TSrc>
		static void CopyConstruct(cAnyVariable *pThis,TSrc cnLib_UREF Src){
			return pThis->CopyConstructT<T>(cnLib_UREFCAST(TSrc)(Src));
		}
		template<class TSrc>
		static void Assign(cAnyVariable *pThis,TSrc cnLib_UREF Src){
			return pThis->Set<T>(cnLib_UREFCAST(TSrc)(Src));
		}
	};

	template<uIntn sSize,class sTAlignment>
	static cFunction_cAnyVariable cSelector_Tester(cAnyVariable<sSize,sTAlignment> &);
	template<class T>
	static cFunction_T<T> cSelector_Tester(T &);

	template<class T>
	struct cSelector
		: decltype(cSelector_Tester(DeclVar<T&>()))
	{
	};
};

//---------------------------------------------------------------------------
}	// namespace cnVar
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif	// cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L
//---------------------------------------------------------------------------
#include <cnTK/TKMacrosCleanup.inc>
//---------------------------------------------------------------------------
#endif	/* cnLibrary_CPPFEATURELEVEL >= 1 */
/*-------------------------------------------------------------------------*/
#endif
