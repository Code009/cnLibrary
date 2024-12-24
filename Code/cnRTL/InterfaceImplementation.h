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
	using TImplementation::TImplementation;

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
	using TImplementation::TImplementation;

	~cInterfaceStaticImplementT()=default;
};
//---------------------------------------------------------------------------
template<class T>
struct cSortablePtrItemOrderOperator
{

	template<class TValue>
	static auto Compare(const T &Item,const TValue &Value)noexcept(true) -> decltype(Item->Compare(Value)){
		return Item->Compare(Value);
	}
};
template<class T>
class cSortablePtr
{
public:
	cSortablePtr()noexcept(true):fPtr(nullptr){}
	cSortablePtr(T *p)noexcept(true):fPtr(p){}

	cSortablePtr(const cSortablePtr &Src)noexcept(true):fPtr(Src.fPtr){}

	cSortablePtr& operator =(const cSortablePtr &Src)noexcept(true){
		fPtr=Src.fPtr;
		return *this;
	}
	cSortablePtr& operator =(T *p)noexcept(true){
		fPtr=p;
		return *this;
	}
	
	eiOrdering Compare(const cSortablePtr &Cmp)const noexcept(true){	return fPtr->Compare(Cmp.fPtr);	}
	eiOrdering Compare(T *Cmp)const noexcept(true){	return fPtr->Compare(Cmp);	}

	bool operator == (const cSortablePtr &Cmp)const noexcept(true)
	{	return fPtr->Compare(Cmp.fPtr)==iOrdering::Equal;	}
	bool operator != (const cSortablePtr &Cmp)const noexcept(true)
	{	return fPtr->Compare(Cmp.fPtr)!=iOrdering::Equal;	}

	cnLib_DEFINE_CLASS_THREE_WAY_COMPARISON(const cSortablePtr &,Compare)


	bool operator == (T *Cmp)const noexcept(true)
	{	return fPtr->Compare(Cmp)==iOrdering::Equal;	}
	bool operator != (T *Cmp)const noexcept(true)
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
struct cAggregablePointerOwnerOperator
{
	template<class T>
	static void Release(T *Pointer)noexcept(true){
		Pointer->AggregableRelease();
	}
};
//---------------------------------------------------------------------------
template<class T>
using cAggregablePtr = cnVar::cPtrOwner<T,cAggregablePointerOwnerOperator>;
//---------------------------------------------------------------------------
template<class TInterfaceImplementation>
class impInterfaceAggregable : public TInterfaceImplementation, public bcAggregable
{
public:
	using TInterfaceImplementation::TInterfaceImplementation;

	~impInterfaceAggregable()=default;

	virtual void* cnLib_FUNC CastInterface(iTypeID InterfaceID)noexcept(true) override{
		return this->HostAggregate->CastInterface(InterfaceID);
	}
	void cnLib_FUNC IncreaseReference(void)noexcept(true){
		return this->HostAggregate->IncreaseReference();
	}
	void cnLib_FUNC DecreaseReference(void)noexcept(true){
		return this->HostAggregate->DecreaseReference();
	}

	typedef TReferenceObjectLifeCycleActivation<TInterfaceImplementation> tLifeCycleActivation;
	friend bcVirtualLifeCycle::cLifeCycleActivation;

	virtual void* AggregableCastInterface(iTypeID InterfaceID)noexcept(true) override{	return TInterfaceImplementation::CastInterface(InterfaceID);	}
	virtual void AggregableRelease(void)noexcept(true) override{	return tLifeCycleActivation::Stop(this);		}

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

	void AssignAggregable(uIntn)noexcept(true){}
	template<class TAggregable,class...VTAggregables>
	void AssignAggregable(uIntn Index,TAggregable&& Aggregable,VTAggregables&&...Aggregables)noexcept(true){
		Aggregable->HostAggregate=this;
		fAggregables[Index]=cnVar::MoveCast(Aggregable);
		return AssignAggregable(Index+1,cnVar::Forward<VTAggregables>(Aggregables)...);
	}
};

//---------------------------------------------------------------------------
template<class...TAggregable>
inline iPtr<iInterface> iAggregateCreate(TAggregable&&...Aggregables)noexcept(true)
{
	return iCreate< cInterfaceAggregate<sizeof...(TAggregable)> >(cnVar::Forward<TAggregable>(Aggregables)...);
}
//---------------------------------------------------------------------------
template<class TInterfaceImplementation>
inline cAggregablePtr< impInterfaceAggregable<TInterfaceImplementation> > iAggregableCreate(void)noexcept(true)
{
	typedef impInterfaceAggregable<TInterfaceImplementation> tAggregable;
	auto Aggregable=new tAggregable;
	TReferenceObjectLifeCycleSharedManager<tAggregable>::ManageShared(Aggregable);
	tAggregable::tLifeCycleActivation::Start(Aggregable);
	return cAggregablePtr<tAggregable>::TakeFromManual(Aggregable);
}
//---------------------------------------------------------------------------
template<class TInterfaceImplementation,class...TArgs>
inline cAggregablePtr< impInterfaceAggregable<TInterfaceImplementation> > iAggregableCreate(TArgs&&...Args)noexcept(true)
{
	typedef impInterfaceAggregable<TInterfaceImplementation> tAggregable;
	auto Aggregable=new tAggregable(cnVar::Forward<TArgs>(Args)...);
	TReferenceObjectLifeCycleSharedManager<tAggregable>::ManageShared(Aggregable);
	tAggregable::tLifeCycleActivation::Start(Aggregable);
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
