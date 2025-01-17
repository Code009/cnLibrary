/*- cnTK - Variant --------------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2018-08-14                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnTK_Variant_HPP__
#define	__cnLibrary_cnTK_Variant_HPP__
/*-------------------------------------------------------------------------*/
#include <cnTK/Common.hpp>
#include <cnTK/TypeInfo.hpp>
#include <cnTK/Pointer.hpp>
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
	static void Call(T &Dest,TSrc cnLib_UREF Src)noexcept(noexcept(Dest=cnLib_UREFCAST(TSrc)(Src)))
	{	Dest=cnLib_UREFCAST(TSrc)(Src);	}

};
template<>
struct AssignOrReconstruct<false>
{
	template<class T,class TSrc>
	static void Call(T &Dest,TSrc cnLib_UREF Src)
		noexcept(
			noexcept(cnClass::ManualDestruct(Dest)) && noexcept(T(cnLib_UREFCAST(TSrc)(Src)))
		)
	{
		cnClass::ManualDestruct(Dest);
		cnClass::ManualConstruct(Dest,cnLib_UREFCAST(TSrc)(Src));
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
		noexcept((cnLib_THelper::Var_TH::AssignOrReconstruct<TIsAssignableFrom<TDest,TSrc cnLib_UREF>::Value>::Call(Dest,cnLib_UREFCAST(TSrc)(Src))))
	)
{	return cnLib_THelper::Var_TH::AssignOrReconstruct<TIsAssignableFrom<TDest,TSrc cnLib_UREF>::Value>::Call(Dest,cnLib_UREFCAST(TSrc)(Src));	}


class cAnyPtr
{
public:

	cnLib_CONSTEXPR_FUNC cAnyPtr()noexcept(true)
		: TypeID(TTypeID<void>::Value),Pointer(nullptr){}
#ifndef cnLibrary_CPPEXCLUDE_NULLPTR
	cnLib_CONSTEXPR_FUNC cAnyPtr(tNullptr)noexcept(true)
		: TypeID(TTypeID<void>::Value),Pointer(nullptr){}
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

template<uIntn Index,class T>
struct cVarPackVariable
{
	T Value;

#if cnLibrary_CPPFEATURE_AGGREGATE_BASE < 201603L

#ifndef cnLibrary_CPPEXCLUDE_CLASS_MEMBER_DEFAULT
	cVarPackVariable()=default;
#else
	cnLib_CONSTEXPR_FUNC cVarPackVariable(){}
#endif

	// need constructor for initialization before c++17
	template<class TArg>
	cnLib_CONSTEXPR_FUNC cVarPackVariable(TArg&& Arg)
		: Value(static_cast<TArg&&>(Arg)){}

#endif // cnLibrary_CPPFEATURE_AGGREGATE_BASE < 201603L
};

template<class TIndexSequence,class...T>
struct cVarPackStruct;


template<uIntn...Indics,class...T>
struct cVarPackStruct<TValueSequence<uIntn,Indics...>,T...>
	: cVarPackVariable<Indics,T>...
{
#if cnLibrary_CPPFEATURE_AGGREGATE_BASE < 201603L
	// need constructor for initialization before c++17
#ifndef cnLibrary_CPPEXCLUDE_CLASS_MEMBER_DEFAULT
	cVarPackStruct()=default;
#else
	cnLib_CONSTEXPR_FUNC cVarPackStruct(){}
#endif

	template<class...TArgs>
	cnLib_CONSTEXPR_FUNC cVarPackStruct(TArgs&&...Args)
		: cVarPackVariable<Indics,T>{static_cast<TArgs&&>(Args)}...{}

#endif // cnLibrary_CPPFEATURE_AGGREGATE_BASE < 201603L


#ifndef cnLibrary_CPPEXCLUDE_FUNCTION_TEMPLATE_DEFALT_ARGUMENT
	template<class...TDest,class TPack=typename TTypeConditional<cVarPackStruct<TValueSequence<uIntn,Indics...>,TDest...>&,TBooleanValuesAnd<TIsReinterpretable<T,TDest>::Value...>::Value>::Type>
	operator cVarPackStruct<TValueSequence<uIntn,Indics...>,TDest...> &(void)noexcept(true){
		return reinterpret_cast<TPack>(*this);
	}
	template<class...TDest,class TPack=typename TTypeConditional<const cVarPackStruct<TValueSequence<uIntn,Indics...>,TDest...>&,TBooleanValuesAnd<TIsReinterpretable<T,TDest>::Value...>::Value>::Type>
	operator const cVarPackStruct<TValueSequence<uIntn,Indics...>,TDest...> &(void)const noexcept(true){
		return reinterpret_cast<TPack>(*this);
	}
// !cnLibrary_CPPEXCLUDE_FUNCTION_TEMPLATE_DEFALT_ARGUMENT
#else
// cnLibrary_CPPEXCLUDE_FUNCTION_TEMPLATE_DEFALT_ARGUMENT
	template<class...TDest>
	operator cVarPackStruct<TValueSequence<uIntn,Indics...>,TDest...> &(void)noexcept(true){
		return reinterpret_cast<
			typename TTypeConditional<cVarPackStruct<TValueSequence<uIntn,Indics...>,TDest...>&,TBooleanValuesAnd<TIsReinterpretable<T,TDest>::Value...>::Value>::Type
		>(*this);
	}
	template<class...TDest>
	operator const cVarPackStruct<TValueSequence<uIntn,Indics...>,TDest...> &(void)const noexcept(true){
		return reinterpret_cast<
			typename TTypeConditional<const cVarPackStruct<TValueSequence<uIntn,Indics...>,TDest...>&,TBooleanValuesAnd<TIsReinterpretable<T,TDest>::Value...>::Value>::Type
		>(*this);
	}
#endif	// cnLibrary_CPPEXCLUDE_FUNCTION_TEMPLATE_DEFALT_ARGUMENT

	template<uIntn Index>
	struct tVariableAt
		: TSelect<Index,T...>{};

};

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

#ifndef cnLibrary_CPPEXCLUDE_NOEXCEPT

template<class TFunc,class...VTVarPackAccessor>
struct FunctionIsNoexcept;
	
template<class TFunc,class TVarPackAccessor,class...VTVarPackAccessor>
struct FunctionIsNoexcept<TFunc,TVarPackAccessor,VTVarPackAccessor...>
	: cnVar::TSelect<noexcept(cnVar::DeclVal<TFunc>()(cnVar::DeclVal<typename TVarPackAccessor::tVariable&>(),cnVar::DeclVal<typename VTVarPackAccessor::tVariable&>()...))
		, cnVar::TConstantValueFalse
		, FunctionIsNoexcept<TFunc,VTVarPackAccessor...>
	>::Type{};

template<class TFunc>
struct FunctionIsNoexcept<TFunc>
	: cnVar::TConstantValueTrue{};

// !cnLibrary_CPPEXCLUDE_NOEXCEPT
#else
// cnLibrary_CPPEXCLUDE_NOEXCEPT

template<class TFunc,class...VTVarPackAccessor>
struct FunctionIsNoexcept
	: cnVar::TConstantValueFalse{};


#endif // cnLibrary_CPPEXCLUDE_NOEXCEPT

template<uIntn Index,class TIndexSequence,class...T>
struct OffsetCalculate
	: cnVar::cVarPackStruct<TIndexSequence,T...>
{
	using cnVar::cVarPackVariable<Index,typename cnVar::TSelect<Index,T...>::Type>::Value;
};


template<class TOffsetCalculate>
struct DataOffset
	: cnVar::TConstantValueUIntn<cnLib_MEMBER_OFFSET(TOffsetCalculate,Value)>
{};


template<uIntn Index>
struct FieldName
{
	static const uChar16 Value[8];
};

template<uIntn Index>
const uChar16 FieldName<Index>::Value[8]={
	u'M',u'V',u'A',u'R',u'0'+(Index/100)%10,u'0'+(Index/10)%10,u'0'+Index%10,0
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

template<uIntn Index,uIntn Begin,uIntn End,class TPackStruct>
struct cVarPackAccessor
{
	static cnLib_CONSTVAR uIntn AccessIndex=Index;
	TPackStruct &Pack;
	cVarPackAccessor(TPackStruct &Pack):Pack(Pack){}

	template<class TSrcPackStruct>
	cVarPackAccessor(const cVarPackAccessor<Index,Begin,End,TSrcPackStruct> &Accessor)
		: Pack(Accessor.Pack){}

	typedef typename TPackStruct::template tVariableAt<Index>::Type tVariable;
	typedef typename TCopyCV<typename TPackStruct::template tVariableAt<Index>::Type,TPackStruct>::Type tRetVariable;
	tRetVariable& operator *()const noexcept(true){
		return Pack.cVarPackVariable<Index,tVariable>::Value;
	}
	typename TRemoveReference<tRetVariable>::Type* operator &()const noexcept(true){
		return cnMemory::AddressOf(Pack.cVarPackVariable<Index,tVariable>::Value);
	}

	typedef cVarPackAccessor<Index+1,Begin,End,TPackStruct> tNext;
	typedef cVarPackAccessor<Index-1,Begin,End,TPackStruct> tPrev;

	tNext operator +(void)const noexcept(true){
		tNext Accessor={Pack};
		return Accessor;
	}

	tPrev operator -(void)const noexcept(true){
		tPrev Accessor={Pack};
		return Accessor;
	}

};

struct cVarPackNoAccess{};

template<uIntn Begin,uIntn End,class TPackStruct>
struct cVarPackAccessor<Begin,Begin,End,TPackStruct>
{
	static cnLib_CONSTVAR uIntn AccessIndex=Begin;
	TPackStruct &Pack;
	cVarPackAccessor(TPackStruct &Pack):Pack(Pack){}

	template<class TSrcPackStruct>
	cVarPackAccessor(const cVarPackAccessor<Begin,Begin,End,TSrcPackStruct> &Accessor)
		: Pack(Accessor.Pack){}

	typedef typename TPackStruct::template tVariableAt<Begin>::Type tVariable;
	typedef typename TCopyCV<typename TPackStruct::template tVariableAt<Begin>::Type,TPackStruct>::Type tRetVariable;
	tRetVariable& operator *()const noexcept(true){
		return Pack.cVarPackVariable<Begin,tVariable>::Value;
	}
	typename TRemoveReference<tRetVariable>::Type* operator &()const noexcept(true){
		return cnMemory::AddressOf(Pack.cVarPackVariable<Begin,tVariable>::Value);
	}

	typedef cVarPackAccessor<Begin+1,Begin,End,TPackStruct> tNext;
	typedef cVarPackNoAccess tPrev;

	tNext operator +(void)const noexcept(true){
		tNext Accessor={this->Pack};
		return Accessor;
	}

	cVarPackNoAccess operator -(void)const noexcept(true){	return cVarPackNoAccess();	}
};

template<uIntn Begin,uIntn End,class TPackStruct>
struct cVarPackAccessor<End,Begin,End,TPackStruct>
{
	static cnLib_CONSTVAR uIntn AccessIndex=End;
	TPackStruct &Pack;

	cVarPackAccessor(TPackStruct &Pack):Pack(Pack){}

	template<class TSrcPackStruct>
	cVarPackAccessor(const cVarPackAccessor<End,Begin,End,TSrcPackStruct> &Accessor)
		: Pack(Accessor.Pack){}

	typedef typename TPackStruct::template tVariableAt<End>::Type tVariable;
	typedef typename TCopyCV<typename TPackStruct::template tVariableAt<End>::Type,TPackStruct>::Type tRetVariable;
	tRetVariable& operator *()const noexcept(true){
		return Pack.cVarPackVariable<End,tVariable>::Value;
	}
	typename TRemoveReference<tRetVariable>::Type* operator &()const noexcept(true){
		return cnMemory::AddressOf(Pack.cVarPackVariable<End,tVariable>::Value);
	}


	typedef cVarPackNoAccess tNext;
	typedef cVarPackAccessor<End-1,Begin,End,TPackStruct> tPrev;

	cVarPackNoAccess operator +(void)const noexcept(true){	return cVarPackNoAccess();	}

	tPrev operator -(void)const noexcept(true){
		tPrev Accessor={this->Pack};
		return Accessor;
	}
};

template<uIntn SingleIndex,class TPackStruct>
struct cVarPackAccessor<SingleIndex,SingleIndex,SingleIndex,TPackStruct>
{
	static cnLib_CONSTVAR uIntn AccessIndex=SingleIndex;
	TPackStruct &Pack;
	cVarPackAccessor(TPackStruct &Pack):Pack(Pack){}

	template<class TSrcPackStruct>
	cVarPackAccessor(const cVarPackAccessor<SingleIndex,SingleIndex,SingleIndex,TSrcPackStruct> &Accessor)
		: Pack(Accessor.Pack){}

	typedef typename TPackStruct::template tVariableAt<SingleIndex>::Type tVariable;
	typedef typename TCopyCV<typename TPackStruct::template tVariableAt<SingleIndex>::Type,TPackStruct>::Type tRetVariable;
	tRetVariable& operator *()const noexcept(true){
		return Pack.cVarPackVariable<SingleIndex,tVariable>::Value;
	}
	typename TRemoveReference<tRetVariable>::Type* operator &()const noexcept(true){
		return cnMemory::AddressOf(Pack.cVarPackVariable<SingleIndex,tVariable>::Value);
	}


	typedef cVarPackNoAccess tNext;
	typedef cVarPackNoAccess tPrev;

	cVarPackNoAccess operator +(void)const noexcept(true){	return cVarPackNoAccess();	}
	cVarPackNoAccess operator -(void)const noexcept(true){	return cVarPackNoAccess();	}
};

template<class...T>
struct cVarPack
	: cVarPackStruct<typename TMakeIndexSequence<TValueSequence<uIntn>,sizeof...(T)>::Type,T...>
{
#if cnLibrary_CPPFEATURE_AGGREGATE_BASE < 201603L
	// need constructor for initialization before c++17
#ifndef cnLibrary_CPPEXCLUDE_CLASS_MEMBER_DEFAULT
	cVarPack()=default;
#else
	cnLib_CONSTEXPR_FUNC cVarPack(){}
#endif
	template<class...TArgs>
	cnLib_CONSTEXPR_FUNC cVarPack(TArgs&&...Args)
		: cVarPackStruct<typename TMakeIndexSequence<TValueSequence<uIntn>,sizeof...(T)>::Type,T...>(static_cast<TArgs&&>(Args)...){}

#endif // cnLibrary_CPPFEATURE_AGGREGATE_BASE < 201603L

	typedef cVarPackStruct<typename TMakeIndexSequence<TValueSequence<uIntn>,sizeof...(T)>::Type,T...> tPackStruct;
	typedef cVarPackAccessor<0,0,sizeof...(T)-1,tPackStruct> tAllAccessor;
	typedef cVarPackAccessor<0,0,sizeof...(T)-1,const tPackStruct> tAllConstAccessor;

	tAllAccessor All(void){
		tAllAccessor Accessor={*this};
		return Accessor;
	}

	tAllConstAccessor All(void)const{
		tAllConstAccessor Accessor={*this};
		return Accessor;
	}

};

template<>
struct cVarPack<>
{
	typedef cVarPackStruct<TValueSequence<uIntn> > tPackStruct;
	typedef cVarPackNoAccess tAllAccessor;
	typedef cVarPackNoAccess tAllConstAccessor;

	tAllAccessor All(void){
		return tAllAccessor();
	}

	tAllConstAccessor All(void)const{
		return tAllConstAccessor();
	}

};


template<uIntn Index,class...T>
struct TVarPackOffsetOf
	: cnLib_THelper::Var_TH::VarPack::DataOffset< cnLib_THelper::Var_TH::VarPack::OffsetCalculate<Index,typename TMakeIndexSequence<TValueSequence<uIntn>,sizeof...(T)>::Type,T...> >
{
};



//---------------------------------------------------------------------------
template<class...VT>
cVarPack<typename cnVar::TRemoveCVRef<VT>::Type...> VarPack(VT cnLib_UREF...v)noexcept(true)
{
	cVarPack<typename cnVar::TRemoveCVRef<VT>::Type...> Pack={cnLib_UREFCAST(VT)(v)...};
	return Pack;
}
//---------------------------------------------------------------------------
template<class...VT>
cVarPack<VT&...> VarRefPack(VT &...v)noexcept(true)
{
	cVarPack<VT&...> Pack={v...};
	return Pack;
}

//---------------------------------------------------------------------------
template<uIntn Index,class...T>
typename TSelect<Index,T...>::Type& VarPackAt(cVarPack<T...> &Pack)noexcept(true)
{
	return Pack.template cVarPackVariable<Index,typename TSelect<Index,T...>::Type>::Value;
}

template<uIntn Index,class...T>
typename TSelect<Index,T...>::Type const& VarPackAt(const cVarPack<T...> &Pack)noexcept(true)
{
	return Pack.template cVarPackVariable<Index,typename TSelect<Index,T...>::Type>::Value;
}

//---------------------------------------------------------------------------

template<class TFunc,class...VTVarPackAccessor>
bool VarPackCallAt(uIntn,const TFunc &,cVarPackNoAccess,VTVarPackAccessor...)noexcept(true){
	return true;
}
template<class TFunc,class...VTVarPackAccessor>
bool VarPackCallAt(uIntn DestIndex,const TFunc &Func,const VTVarPackAccessor&...Accessor)noexcept(cnLib_THelper::Var_TH::VarPack::FunctionIsNoexcept<TFunc,VTVarPackAccessor...>::Value)
{
	if(DestIndex==0){
		Func((*Accessor)...);
		return true;
	}
	return VarPackCallAt(DestIndex-1,Func,+Accessor...);
}

//---------------------------------------------------------------------------
template<class TFunc,class...VTVarPackAccessor>
void VarPackForEach(const TFunc &,cVarPackNoAccess,const VTVarPackAccessor&...)noexcept(true)
{
}
template<class TFunc,class...VTVarPackAccessor>
void VarPackForEach(const TFunc &Func,const VTVarPackAccessor&...Accessor)noexcept(cnLib_THelper::Var_TH::VarPack::FunctionIsNoexcept<TFunc,VTVarPackAccessor...>::Value)
{
	Func((*Accessor)...);
	return VarPackForEach(Func,+Accessor...);
}
//---------------------------------------------------------------------------
template<class TFunc,class...VTVarPackAccessor>
bool VarPackForWhile(const TFunc &,cVarPackNoAccess,const VTVarPackAccessor&...)noexcept(true)
{
	return true;
}
template<class TFunc,class...VTVarPackAccessor>
bool VarPackForWhile(const TFunc &Func,const VTVarPackAccessor&...Accessor)noexcept(cnLib_THelper::Var_TH::VarPack::FunctionIsNoexcept<TFunc,VTVarPackAccessor...>::Value)
{
	if(Func((*Accessor)...)==false)
		return false;
	return VarPackForWhile(Func,+Accessor...);
}
//---------------------------------------------------------------------------


template<class...VT>
struct cVarPackMetaClass
{
	typedef typename TMakeIndexSequence<TValueSequence<uIntn>,sizeof...(VT)>::Type tIndexSequence;
	static const cMetaClass MetaClass;
	static const cMetaClass *const Value;
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
		cnLib_THelper::Var_TH::VarPack::FieldName<Indexes>::Value,	//const uChar16 *Name;
		TRuntimeTypeInfo<VT>::Value,								//rtInfo RuntimeInfo;
		TVarPackOffsetOf<Indexes,VT...>::Value,						//uIntn Offset;
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

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
}	// namespace cnVar
//---------------------------------------------------------------------------
}   // namespace cnLibrary
//---------------------------------------------------------------------------
#endif	// cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L
//---------------------------------------------------------------------------

#if	cnLibrary_CPPFEATURELEVEL >= 2

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
//	static rtTypeInfo GetTypeInfo(tTypeIndex TypeIndex);
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
		cnClass::ManualConstruct(reinterpret_cast<typename TVariantTypeOperator::template tTypeByIndex<TVariantTypeOperator::DefaultTypeIndex>::Type&>(fContent));
	}
	~cVariant()noexcept(TVariantTypeOperator::IsDestructNoexcept){
		rtTypeInfo TypeInfo=TVariantTypeOperator::GetTypeInfo(fTypeIndex);
		TypeInfo->Operators->Destruct(&fContent);
	}

	cVariant(const cVariant &Src)noexcept(TVariantTypeOperator::IsCopyConstructNoexcept){
		fTypeIndex=Src.fTypeIndex;
		rtTypeInfo TypeInfo=TVariantTypeOperator::GetTypeInfo(fTypeIndex);
		TypeInfo->Operators->CopyConstruct(&fContent,&Src.fContent);
	}
	
#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L
	cVariant(cVariant &&Src)noexcept(TVariantTypeOperator::IsMoveConstructNoexcept){
		fTypeIndex=Src.fTypeIndex;
		rtTypeInfo TypeInfo=TVariantTypeOperator::GetTypeInfo(fTypeIndex);
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
		rtTypeInfo TypeInfo=TVariantTypeOperator::GetTypeInfo(fTypeIndex);
		rtTypeInfo NewTypeInfo;
		if(fTypeIndex==Src.fTypeIndex || TypeInfo==(NewTypeInfo=TVariantTypeOperator::GetTypeInfo(Src.fTypeIndex))){
			if(TypeInfo->IsCopyAssignable){
				TypeInfo->Operators->Copy(&fContent,&Src.fContent);
			}
			else{
				TypeInfo->Operators->Destruct(&fContent);
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
		rtTypeInfo TypeInfo=TVariantTypeOperator::GetTypeInfo(fTypeIndex);
		rtTypeInfo NewTypeInfo;
		if(fTypeIndex==Src.fTypeIndex || TypeInfo==(NewTypeInfo=TVariantTypeOperator::GetTypeInfo(Src.fTypeIndex))){
			if(TypeInfo->IsMoveAssignable){
				TypeInfo->Operators->Move(&fContent,&Src.fContent);
			}
			else{
				TypeInfo->Operators->Destruct(&fContent);
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
			rtTypeInfo CurTypeInfo=TVariantTypeOperator::GetTypeInfo(fTypeIndex);
			rtTypeInfo TypeInfo=TRuntimeTypeInfo<typename TVariantTypeOperator::template tTypeByIndex<TypeIndex>::Type>::Value;
			if(CurTypeInfo!=TypeInfo){
				CurTypeInfo->Operators->Destruct(&fContent);
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

		rtTypeInfo CurTypeInfo=TVariantTypeOperator::GetTypeInfo(fTypeIndex);
		rtTypeInfo NewTypeInfo=TVariantTypeOperator::GetTypeInfo(NewTypeIndex);
		fTypeIndex=NewTypeIndex;
		if(CurTypeInfo!=NewTypeInfo){
			CurTypeInfo->Operators->Destruct(&fContent);
			NewTypeInfo->Operators->Construct(&fContent);
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
		noexcept(AssignOrReconstruct(DeclVal<typename TVariantTypeOperator::template tTypeByIndex<NewTypeIndex>::Type&>(),cnLib_UREFCAST(TArg)(Arg)))
		&& noexcept(typename TVariantTypeOperator::template tTypeByIndex<NewTypeIndex>::Type(cnLib_UREFCAST(TArg)(Arg)))
		&& TVariantTypeOperator::IsDestructNoexcept
	){
		typedef typename TVariantTypeOperator::template tTypeByIndex<NewTypeIndex>::Type TNewType;
		if(NewTypeIndex==fTypeIndex){
			AssignOrReconstruct(reinterpret_cast<TNewType&>(fContent),cnLib_UREFCAST(TArg)(Arg));
		}
		else{
			rtTypeInfo TypeInfo=TVariantTypeOperator::GetTypeInfo(fTypeIndex);
			if(TypeInfo==TRuntimeTypeInfo<TNewType>::Value){
				AssignOrReconstruct(reinterpret_cast<TNewType&>(fContent),cnLib_UREFCAST(TArg)(Arg));
			}
			else{
				TypeInfo->Operators->Destruct(&fContent);
				fTypeIndex=NewTypeIndex;
				cnClass::ManualConstruct(reinterpret_cast<TNewType&>(fContent),cnLib_UREFCAST(TArg)(Arg));
			}
		}
	}

#if cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L

	template<tTypeIndex NewTypeIndex,class...TArgs>
	void Set(TArgs cnLib_UREF...Args)noexcept(
		noexcept(typename TVariantTypeOperator::template tTypeByIndex<NewTypeIndex>::Type(cnLib_UREFCAST(TArgs)(Args)...))
		&& TVariantTypeOperator::IsDestructNoexcept
	){
		typedef typename TVariantTypeOperator::template tTypeByIndex<NewTypeIndex>::Type TNewType;
		if(NewTypeIndex==fTypeIndex){
			cnClass::ManualDestruct(reinterpret_cast<TNewType&>(fContent));
		}
		else{
			rtTypeInfo TypeInfo=TVariantTypeOperator::GetTypeInfo(fTypeIndex);
			TypeInfo->Operators->Destruct(&fContent);
			fTypeIndex=NewTypeIndex;
		}
		cnClass::ManualConstruct(reinterpret_cast<TNewType&>(fContent),cnLib_UREFCAST(TArgs)(Args)...);
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
namespace cnLib_THelper{
//---------------------------------------------------------------------------
namespace Var_TH{
//---------------------------------------------------------------------------

template<uIntn Size,class TAlignment>
struct cAnyVariableContent_Value
{
	cnVar::cPolymorphicBlock<Size,TAlignment> Content;
	rtTypeInfo TypeInfo;

	template<class T>
	T* Ptr(void)noexcept(true){
		return static_cast<T*>(&this->Content);
	}

	template<class T>
	const T* Ptr(void)const noexcept(true){
		return static_cast<const T*>(&this->Content);
	}

	void Construct(void)noexcept(true){
		this->TypeInfo->Operators->Construct(&this->Content);
	}
	void Destruct(void)noexcept(true){
		this->TypeInfo->Operators->Destruct(&this->Content);
	}

#if cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L

	template<class T,class...TArgs>
	void ConstructAs(TArgs cnLib_UREF...Args)noexcept(noexcept(T(cnLib_UREFCAST(TArgs)(Args)...))){
		Content.template ConstructAs<T>(cnLib_UREFCAST(TArgs)(Args)...);
	}

	// reconstruct
	template<class T,class...TArgs>
	void Reconstruct(TArgs cnLib_UREF...Args)noexcept(noexcept(T(cnLib_UREFCAST(TArgs)(Args)...))){
		this->Content.template DestructAs<T>();
		this->Content.template ConstructAs<T>(cnLib_UREFCAST(TArgs)(Args)...);
	}

// cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L
#else
// cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES < 200704L

	template<class T,class TArg>
	void ConstructAs(TArgs cnLib_UREF Arg)noexcept(noexcept(T(cnLib_UREFCAST(TArg)(Arg)))){
		Content.template ConstructAs<T>(cnLib_UREFCAST(TArg)(Arg));
	}

	// reconstruct
	template<class T,class TArg>
	void Reconstruct(TArgs cnLib_UREF Arg)noexcept(noexcept(T(cnLib_UREFCAST(TArg)(Arg)))){
		this->Content.template DestructAs<T>();
		this->Content.template ConstructAs<T>(cnLib_UREFCAST(TArg)(Arg));
	}

#endif	// cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES < 200704L

	template<class T,class TSrc>
	void SetVar(TSrc cnLib_UREF Src)noexcept(noexcept(cnVar::AssignOrReconstruct(cnVar::DeclVal<T>(),cnLib_UREFCAST(TSrc)(Src)))){
		cnVar::AssignOrReconstruct(reinterpret_cast<T&>(this->Content),cnLib_UREFCAST(TSrc)(Src));
	}

	template<uIntn SrcSize,class TSrcAlignment>
	void CopyConstructValue(const cAnyVariableContent_Value<SrcSize,TSrcAlignment> &Src)noexcept(true){
		this->TypeInfo->Operators->CopyConstruct(&this->Content,&Src.Content);
	}

	template<uIntn SrcSize,class TSrcAlignment>
	void MoveConstructValue(cAnyVariableContent_Value<SrcSize,TSrcAlignment> &Src)noexcept(true){
		this->TypeInfo->Operators->MoveConstruct(&this->Content,&Src.Content);
	}

	template<uIntn SrcSize,class TSrcAlignment>
	void CopyAssignSameType(const cAnyVariableContent_Value<SrcSize,TSrcAlignment> &Src)noexcept(true){
		if(this->TypeInfo->IsCopyAssignable){
			this->TypeInfo->Operators->Copy(&this->Content,&Src.Content);
		}
		else{
			this->TypeInfo->Operators->Destruct(&this->Content);
			this->TypeInfo->Operators->CopyConstruct(&this->Content,&Src.Content);
		}
	}
	template<uIntn SrcSize,class TSrcAlignment>
	void MoveAssignSameType(cAnyVariableContent_Value<SrcSize,TSrcAlignment> &Src)noexcept(true){
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
	rtTypeInfo TypeInfo;

	template<class T>
	T* Ptr(void)noexcept(true){
		return static_cast<T*>(static_cast<void*>(this->Pointer));
	}

	template<class T>
	const T* Ptr(void)const noexcept(true){
		return static_cast<const T*>(static_cast<const void*>(this->Pointer));
	}

	void Construct(void)noexcept(true){
		static_cast<void*&>(this->Pointer)=this->TypeInfo->Operators->New();
		this->TypeInfo->Operators->Construct(this->Pointer);
	}

	void Destruct(void)noexcept(true){
		this->TypeInfo->Operators->Destruct(this->Pointer);
		this->TypeInfo->Operators->Delete(this->Pointer);
	}


#if cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L

	template<class T,class...TArgs>
	void ConstructAs(TArgs cnLib_UREF...Args)noexcept(true){
		reinterpret_cast<T*&>(this->Pointer)=new T(cnLib_UREFCAST(TArgs)(Args)...);
	}
	         
	// reconstruct
	template<class T,class...TArgs>
	void Reconstruct(TArgs cnLib_UREF...Args)noexcept(true){
		cnClass::ManualDestruct<T>(*static_cast<T*>(this->Pointer));
		cnClass::ManualConstruct<T>(*static_cast<T*>(this->Pointer),cnLib_UREFCAST(TArgs)(Args)...);
	}
// cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L
#else
// cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES < 200704L

	template<class T,class TArg>
	void ConstructAs(TArgs cnLib_UREF Arg){
		reinterpret_cast<T*&>(this->Pointer)=new T(cnLib_UREFCAST(TArg)(Arg));
	}

	// reconstruct
	template<class T,class...TArg>
	void Reconstruct(TArgs cnLib_UREF Arg){
		cnClass::ManualDestruct<T>(*static_cast<T*>(this->Pointer));
		cnClass::ManualConstruct<T>(*static_cast<T*>(this->Pointer),cnLib_UREFCAST(TArg)(Arg));
	}
#endif	// cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES < 200704L

	template<class T,class TSrc>
	void SetVar(TSrc cnLib_UREF Src)noexcept(true){
		cnVar::AssignOrReconstruct(*reinterpret_cast<T*&>(this->Pointer),cnLib_UREFCAST(TSrc)(Src));
	}

	
	template<uIntn SrcSize,class TSrcAlignment>
	void CopyConstructValue(const cAnyVariableContent_Pointer<SrcSize,TSrcAlignment> &Src)noexcept(true){
		static_cast<void*&>(this->Pointer)=TypeInfo->Operators->New();
		this->TypeInfo->Operators->CopyConstruct(this->Pointer,Src.Pointer);
	}
	template<uIntn SrcSize,class TSrcAlignment>
	void MoveConstructValue(cAnyVariableContent_Pointer<SrcSize,TSrcAlignment> &Src)noexcept(true){
		static_cast<void*&>(this->Pointer)=Src.Pointer;
		Src.TypeInfo=cnVar::TRuntimeTypeInfo<typename cnVar::TSelect<0,void,TAlignment>::Type>::Value;
	}


	template<uIntn SrcSize,class TSrcAlignment>
	void CopyAssignSameType(const cAnyVariableContent_Pointer<SrcSize,TSrcAlignment> &Src)noexcept(true){
		if(this->TypeInfo->IsCopyAssignable){
			this->TypeInfo->Operators->Copy(this->Pointer,Src.Pointer);
		}
		else{
			this->TypeInfo->Operators->Destruct(this->Pointer);
			this->TypeInfo->Operators->CopyConstruct(this->Pointer,Src.Pointer);
		}
	}


	template<uIntn SrcSize,class TSrcAlignment>
	void MoveAssignSameType(const cAnyVariableContent_Pointer<SrcSize,TSrcAlignment> &Src)noexcept(true){
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

	cAnyVariable()noexcept(true){		fValueContent.TypeInfo=TRuntimeTypeInfo<typename TSelect<0,void,TAlignment>::Type>::Value;	}
	~cAnyVariable()noexcept(true){
		if(IsPointerContent()){
			fPointerContent.Destruct();
		}
		else{
			fValueContent.Destruct();
		}
	}
	
	cAnyVariable(const cAnyVariable &Src)noexcept(true){	CopyConstructA(Src);	}
#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L
	cAnyVariable(cAnyVariable &&Src)noexcept(true){			CopyConstructA(static_cast<cAnyVariable &&>(Src));	}
#endif

	template<class T>
	cAnyVariable(T cnLib_UREF Src)noexcept(true){
		cSelector<typename TRemoveCVRef<T>::Type>::CopyConstruct(this,cnLib_UREFCAST(T)(Src));
	}

	cAnyVariable& operator = (const cAnyVariable &Src)noexcept(true){
		if(this==&Src)
			return *this;

		AssignA(Src);
		return *this;
	}
	
#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L
	cAnyVariable& operator = (cAnyVariable &&Src)noexcept(true){
		AssignA(static_cast<cAnyVariable&&>(Src));
		return *this;
	}
#endif
	// assignment operator
	template<class T>
	cAnyVariable& operator = (T cnLib_UREF Src)noexcept(true){
		cSelector<typename TRemoveCVRef<T>::Type>::Assign(this,cnLib_UREFCAST(T)(Src));
		return *this;
	}

	// reference directly
	template<class T>
	T& Ref(void)noexcept(true){
		auto &Content=ContentSelector<T>();
		return *Content.template Ptr<T>();
	}
	template<class T>
	const T& Ref(void)const noexcept(true){
		auto &Content=ContentSelector<T>();
		return *Content.template Ptr<T>();
	}
	
	template<class T>	explicit operator T& ()noexcept(true){			return Ref<T>();	}
	template<class T>	explicit operator const T& ()const noexcept(true){	return Ref<T>();	}
	// Ptr
	//	point to specified type
	//	data of previous type should be overwritten
	template<class T>
	T* Ptr(void)noexcept(true){
		if(IsType<T>()==false)
			return nullptr;
		auto &Content=ContentSelector<T>();
		return Content.template Ptr<T>();
	}
	// Ptr
	//	point to specified type
	template<class T>
	const T* Ptr(void)const noexcept(true){
		if(IsType<T>()==false)
			return nullptr;
		auto &Content=ContentSelector<T>();
		return Content.template Ptr<T>();
	}

	// IsType
	//	Check if it is type T
	template<class T>
	bool IsType(void)const noexcept(true){
		return fValueContent.TypeInfo==TRuntimeTypeInfo<T>::Value;
	}
	template<class T>
	void SetType(void)noexcept(true){
		return SetTypeInfo(TRuntimeTypeInfo<T>::Value);
	}

	rtTypeInfo GetTypeInfo(void)const noexcept(true){
		return fValueContent.TypeInfo;
	}
	
	void SetTypeInfo(rtTypeInfo NewTypeInfo)noexcept(true){
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
	bool Get(T &Dest)const noexcept(true){
		if(IsType<T>()==false)
			return false;
		auto &Content=ContentSelector<T>();
		Dest=*Content.template Ptr<T>();
		return true;
	}

	// assignment
	template<class T=void,class TSrc>
	void Set(TSrc cnLib_UREF Src)noexcept(true){
		typedef typename TSelect<TIsSame<T,void>::Value,
			T,
			typename TRemoveCVRef<TSrc>::Type
		>::Type TVal;

		rtTypeInfo NewTypeInfo=TRuntimeTypeInfo<TVal>::Value;
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

#if cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L

	// reconstruct
	template<class T,class...TArgs>
	void Set(TArgs cnLib_UREF...Args)noexcept(true){
		rtTypeInfo NewTypeInfo=TRuntimeTypeInfo<T>::Value;
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
#endif	// cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L
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
	typename cContentSelector<T>::Type& ContentSelector(void)noexcept(true){
		return reinterpret_cast<typename cContentSelector<T>::Type&>(fValueContent);
	}
	template<class T>
	const typename cContentSelector<T>::Type& ContentSelector(void)const noexcept(true){
		return reinterpret_cast<const typename cContentSelector<T>::Type&>(fValueContent);
	}

	template<class T>
	static bool NeedPointer(void)noexcept(true){
		return StorageSize<sizeof(T);
	}
	static bool IsPointerContent(rtTypeInfo TypeInfo)noexcept(true){
		return TypeInfo->Size>StorageSize;
	}
	bool IsPointerContent(void)const noexcept(true){
		return fValueContent.TypeInfo->Size>StorageSize;
	}

	template<uIntn sSize,class sTAlignment>
	void CopyConstructA(const cAnyVariable<sSize,sTAlignment> &Src)noexcept(true){
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
	void CopyConstructA(cAnyVariable<sSize,sTAlignment> &&Src)noexcept(true){
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
	void AssignA(const cAnyVariable<sSize,sTAlignment> &Src)noexcept(true){
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
	void AssignA(cAnyVariable<sSize,sTAlignment> &&Src)noexcept(true){
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
	void CopyConstructT(TSrc cnLib_UREF Src)noexcept(true){
		auto &Content=ContentSelector<T>();
		Content.TypeInfo=TRuntimeTypeInfo<T>::Value;
		Content.template ConstructAs<T>(cnLib_UREFCAST(TSrc)(Src));
	}

	struct cFunction_cAnyVariable
	{
		template<class TSrc>
		static void CopyConstruct(cAnyVariable *pThis,TSrc cnLib_UREF Src)noexcept(true){
			return pThis->CopyConstructA(cnLib_UREFCAST(TSrc)(Src));
		}
		template<class TSrc>
		static void Assign(cAnyVariable *pThis,TSrc cnLib_UREF Src)noexcept(true){
			return pThis->AssignA(cnLib_UREFCAST(TSrc)(Src));
		}
	};
	template<class T>
	struct cFunction_T
	{
		template<class TSrc>
		static void CopyConstruct(cAnyVariable *pThis,TSrc cnLib_UREF Src)noexcept(true){
			return pThis->CopyConstructT<T>(cnLib_UREFCAST(TSrc)(Src));
		}
		template<class TSrc>
		static void Assign(cAnyVariable *pThis,TSrc cnLib_UREF Src)noexcept(true){
			return pThis->Set<T>(cnLib_UREFCAST(TSrc)(Src));
		}
	};

	template<uIntn sSize,class sTAlignment>
	static cFunction_cAnyVariable cSelector_Tester(cAnyVariable<sSize,sTAlignment> &);
	template<class T>
	static cFunction_T<T> cSelector_Tester(T &);

	template<class T>
	struct cSelector
		: decltype(cSelector_Tester(DeclVal<T&>()))
	{
	};
};

//---------------------------------------------------------------------------
}	// namespace cnVar
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------

#endif	// cnLibrary_CPPFEATURELEVEL >= 2

//---------------------------------------------------------------------------
#include <cnTK/TKMacrosCleanup.inc>
//---------------------------------------------------------------------------
#endif	/* cnLibrary_CPPFEATURELEVEL >= 1 */
/*-------------------------------------------------------------------------*/
#endif
