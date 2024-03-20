/*- cnRTL - Interface Support ---------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2006-01-17                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnRTL_InterfaceImplementation_H__
#define __cnLibrary_cnRTL_InterfaceImplementation_H__
/*-------------------------------------------------------------------------*/
#include <cnRTL/cnRTLHeader.h>
#include <cnRTL/ThreadSynchronization.h>
#include <cnRTL/ObjectLifeCycle.h>
#include <cnRTL/ObjectReference.h>
#ifdef __cplusplus
//---------------------------------------------------------------------------
#if cnLibrary_CPPFEATURE_CONSTEXPR >= 200704L
#define	cnRTL_CONSTVAR	constexpr
#define	cnRTL_CONSTEXPR_FUNC constexpr
// cnLibrary_CPPFEATURE_CONSTEXPR >= 200704L
#else
// cnLibrary_CPPFEATURE_CONSTEXPR < 200704L
#define	cnRTL_CONSTVAR	const
#define	cnRTL_CONSTEXPR_FUNC
#endif	// cnLibrary_CPPFEATURE_CONSTEXPR < 200704L


//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnRTL{

//---------------------------------------------------------------------------
template<class T>
void* TCastInterface(T *Pointer,iTypeID InterfaceID)noexcept(true)
{
	return Pointer->T::CastInterface(InterfaceID);
}
inline void* TCastInterface(iReference *Pointer,iTypeID InterfaceID)noexcept(true)
{
	if(InterfaceID==TInterfaceID<iReference>::Value){
		return Pointer;
	}
	return nullptr;
}
//---------------------------------------------------------------------------
template<class T>
void* ImpCastInterface(T *,iTypeID)noexcept(true)
{
	return nullptr;
}
template<class TInterface,class...VT,class T>
void* ImpCastInterface(T *Pointer,iTypeID InterfaceID)noexcept(true)
{
	void *Interface;
	Interface=TCastInterface(static_cast<TInterface*>(Pointer),InterfaceID);
	if(Interface!=nullptr)
		return Interface;

	return ImpCastInterface<VT...>(Pointer,InterfaceID);
}
//---------------------------------------------------------------------------
template<class TImplementation>
class cReferenceStaticImplementT  : public TImplementation
{
private:
	virtual void cnLib_FUNC IncreaseReference(void)noexcept(true) override{}
	virtual void cnLib_FUNC DecreaseReference(void)noexcept(true) override{}
public:
#if cnLibrary_CPPFEATURE_INHERIT_CONSTRUCTORS >= 200802L
	using TImplementation::TImplementation;
#else	// cnLibrary_CPPFEATURE_INHERIT_CONSTRUCTORS < 200802L
	template<class...VT>
	cReferenceStaticImplementT(VT&&...Args):TImplementation(Forward<VT>(Args)...){}
#endif	// cnLibrary_CPPFEATURE_INHERIT_CONSTRUCTORS

	~cReferenceStaticImplementT()=default;
};
//---------------------------------------------------------------------------
class cStaticRefernce : public iReference
{
public:
	virtual void cnLib_FUNC IncreaseReference(void)noexcept(true) override;
	virtual void cnLib_FUNC DecreaseReference(void)noexcept(true) override;
};
extern iReference *const gStaticReference;
//---------------------------------------------------------------------------
template<class TImplementation>
class cInterfaceStaticImplementT  : public TImplementation
{
private:
	virtual void* cnLib_FUNC CastInterface(iTypeID InterfaceID)noexcept(true) override
	{
		if(InterfaceID==TInterfaceID<iReference>::Value)
			return gStaticReference;
		return TImplementation::CastInterface(InterfaceID);
	}
public:
#if cnLibrary_CPPFEATURE_INHERIT_CONSTRUCTORS >= 200802L
	using TImplementation::TImplementation;
#else	// cnLibrary_CPPFEATURE_INHERIT_CONSTRUCTORS < 200802L
	template<class...VT>
	cInterfaceStaticImplementT(VT&&...Args)noexcept(true):TImplementation(Forward<VT>(Args)...){}
#endif // cnLibrary_CPPFEATURE_INHERIT_CONSTRUCTORS

	~cInterfaceStaticImplementT()=default;
};
//---------------------------------------------------------------------------
template<class T>
struct cSortablePtrItemOrderOperator
{

	template<class TValue>
	static auto Compare(const T &Item,const TValue &Value) -> decltype(Item->Compare(Value)){
		return Item->Compare(Value);
	}
};
template<class T>
class cSortablePtr
{
public:
	cSortablePtr():fPtr(nullptr){}
	cSortablePtr(T *p):fPtr(p){}

	cSortablePtr(const cSortablePtr &Src):fPtr(Src.fPtr){}

	cSortablePtr& operator =(const cSortablePtr &Src){
		fPtr=Src.fPtr;
		return *this;
	}
	cSortablePtr& operator =(T *p){
		fPtr=p;
		return *this;
	}
	
	eiOrdering Compare(const cSortablePtr &Cmp)const{	return fPtr->Compare(Cmp.fPtr);	}
	eiOrdering Compare(T *Cmp)const{	return fPtr->Compare(Cmp);	}

	bool operator == (const cSortablePtr &Cmp)const
	{	return fPtr->Compare(Cmp.fPtr)==iOrdering::Equal;	}
	bool operator != (const cSortablePtr &Cmp)const
	{	return fPtr->Compare(Cmp.fPtr)!=iOrdering::Equal;	}

	cnLib_DEFINE_CLASS_THREE_WAY_COMPARISON(const cSortablePtr &,Compare)


	bool operator == (T *Cmp)const
	{	return fPtr->Compare(Cmp)==iOrdering::Equal;	}
	bool operator != (T *Cmp)const
	{	return fPtr->Compare(Cmp)!=iOrdering::Equal;	}

	cnLib_DEFINE_CLASS_THREE_WAY_COMPARISON(T*,Compare)
protected:
	T *fPtr;
};
//---------------------------------------------------------------------------
class bcInterfaceAggregate : public iInterface, public iReference
{
};
//---------------------------------------------------------------------------
class bcAggregable
{
public:
	bcInterfaceAggregate *HostAggregate=nullptr;
	
	virtual void* AggregableCastInterface(iTypeID InterfaceID)noexcept(true)=0;
	virtual void AggregableRelease(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
template<class T>
struct cAggregableOwnerToken : cnVar::bcPointerOwnerTokenOperator<T*>
{
	static void Release(T *Token){
		if(Token!=nullptr)
			Token->AggregableRelease();
	}
};
//---------------------------------------------------------------------------
template<class T>
using cAggregablePtr = cnVar::cPtrOwner< cAggregableOwnerToken<T> >;
//---------------------------------------------------------------------------
template<class TInterfaceImplementation>
class impInterfaceAggregable : public TInterfaceImplementation, public bcAggregable
{
public:
#if cnLibrary_CPPFEATURE_INHERIT_CONSTRUCTORS >= 200802L
	using TInterfaceImplementation::TInterfaceImplementation;
#else	// cnLibrary_CPPFEATURE_INHERIT_CONSTRUCTORS < 200802L
	template<class...VT>
	impInterfaceAggregable(VT&&...Args):TInterfaceImplementation(static_cast<VT&&>(Args)...){}
#endif // cnLibrary_CPPFEATURE_INHERIT_CONSTRUCTORS

#ifndef cnLibrary_CPPEXCLUDE_CLASS_MEMBER_DEFAULT
	~impInterfaceAggregable()=default;
#else
	~impInterfaceAggregable(){}
#endif

	virtual void* cnLib_FUNC CastInterface(iTypeID InterfaceID)noexcept(true) override{
		return this->HostAggregate->CastInterface(InterfaceID);
	}
	void cnLib_FUNC IncreaseReference(void)noexcept(true){
		return this->HostAggregate->IncreaseReference();
	}
	void cnLib_FUNC DecreaseReference(void)noexcept(true){
		return this->HostAggregate->DecreaseReference();
	}

	template<class TLifeCycleObject>
	struct tActivation
	{
		static void Start(TLifeCycleObject *Object)noexcept(true){
			cnVar::TSelect<cnVar::TClassIsInheritFrom<TInterfaceImplementation,bcVirtualLifeCycle>::Value
				, typename TSelectActivation< TInterfaceImplementation,cCPPLifeCycleInstance::tActivation< impInterfaceAggregable<TInterfaceImplementation> > >::Type
				, typename TSelectActivation< TInterfaceImplementation,bcVirtualLifeCycle::cActivation< impInterfaceAggregable<TInterfaceImplementation> > >::Type
			>::Type::Start(Object);
		}
		static void Stop(TLifeCycleObject *Object)noexcept(true){
			cnVar::TSelect<cnVar::TClassIsInheritFrom<TInterfaceImplementation,bcVirtualLifeCycle>::Value
				, typename TSelectActivation< TInterfaceImplementation,cCPPLifeCycleInstance::tActivation< impInterfaceAggregable<TInterfaceImplementation> > >::Type
				, typename TSelectActivation< TInterfaceImplementation,bcVirtualLifeCycle::cActivation< impInterfaceAggregable<TInterfaceImplementation> > >::Type
			>::Type::Stop(Object);
		}
	};
	friend bcVirtualLifeCycle::cActivation<impInterfaceAggregable>;

	typedef typename cnVar::TSelect<cnVar::TClassIsInheritFrom<TInterfaceImplementation,bcVirtualLifeCycle>::Value
		, cCPPLifeCycleSharedManager< impInterfaceAggregable<TInterfaceImplementation> >
		, cVirtualLifeCycleSharedManager< impInterfaceAggregable<TInterfaceImplementation> >
	>::Type tLifeCycleManager;

	virtual void* AggregableCastInterface(iTypeID InterfaceID)noexcept(true) override{	return TInterfaceImplementation::CastInterface(InterfaceID);	}
	virtual void AggregableRelease(void)noexcept(true) override{	return tLifeCycleManager::tLifeCycleActivation::Stop(this);		}

};
//---------------------------------------------------------------------------
template<uIntn AggregableCount>
class cInterfaceAggregate : public bcInterfaceAggregate
{
public:
	template<class...TAggregable>
	cInterfaceAggregate(TAggregable&&...Aggregables)noexcept(true){
		static_assert(sizeof...(TAggregable)==AggregableCount,"Incorrect Parameter Count");
		AssignAggregable(0,cnVar::Forward<TAggregable>(Aggregables)...);
	}
	~cInterfaceAggregate()noexcept(true){
	}

	virtual void* cnLib_FUNC CastInterface(iTypeID ID)noexcept(true) override{
		void *Ret;
		for(uIntn i=0;i<AggregableCount;i++){
			Ret=fAggregables[i]->AggregableCastInterface(ID);
			if(Ret!=nullptr)
				return Ret;
		}
		return nullptr;
	}
private:
	cAggregablePtr<bcAggregable> fAggregables[AggregableCount];

	void AssignAggregable(uIntn){}
	template<class TAggregable,class...VTAggregables>
	void AssignAggregable(uIntn Index,TAggregable&& Aggregable,VTAggregables&&...Aggregables)noexcept(true){
		Aggregable->HostAggregate=this;
		fAggregables[Index]=cnVar::MoveCast(Aggregable);
		return AssignAggregable(Index+1,cnVar::Forward<VTAggregables>(Aggregables)...);
	}
};

//---------------------------------------------------------------------------
template<class...TAggregable>
iPtr<iInterface> iAggregateCreate(TAggregable&&...Aggregables)
{
	return iCreate< cInterfaceAggregate<sizeof...(TAggregable)> >(cnVar::Forward<TAggregable>(Aggregables)...);
}
//---------------------------------------------------------------------------
template<class TInterfaceImplementation>
cAggregablePtr< impInterfaceAggregable<TInterfaceImplementation> > iAggregableCreate(void)
{
	typedef impInterfaceAggregable<TInterfaceImplementation> tAggregable;
	typedef typename tAggregable::tLifeCycleManager tLifeCycleManager;
	auto Aggregable=new tAggregable;
	tLifeCycleManager::ManageShared(Aggregable);
	tLifeCycleManager::tLifeCycleActivation::Start(Aggregable);
	return cAggregablePtr<tAggregable>::TakeFromManual(Aggregable);
}
//---------------------------------------------------------------------------
template<class TInterfaceImplementation,class...TArgs>
cAggregablePtr< impInterfaceAggregable<TInterfaceImplementation> > iAggregableCreate(TArgs&&...Args)
{
	typedef impInterfaceAggregable<TInterfaceImplementation> tAggregable;
	typedef typename tAggregable::tLifeCycleManager tLifeCycleManager;
	auto Aggregable=new tAggregable(cnVar::Forward<TArgs>(Args)...);
	tLifeCycleManager::ManageShared(Aggregable);
	tLifeCycleManager::tLifeCycleActivation::Start(Aggregable);
	return cAggregablePtr<tAggregable>::TakeFromManual(Aggregable);
}
//---------------------------------------------------------------------------
}   // namespace cnRTL
//---------------------------------------------------------------------------
}   // namespace cnLibrary
//---------------------------------------------------------------------------
#undef	cnRTL_CONSTEXPR_FUNC
#undef	cnRTL_CONSTVAR
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif
