/*- cnRTL - Object Reference ----------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2006-01-17                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnRTL_ObjectReference_H__
#define __cnLibrary_cnRTL_ObjectReference_H__
/*-------------------------------------------------------------------------*/
#include <cnRTL/cnRTLHeader.h>
#include <cnRTL/RuntimeFunction.h>
#include <cnRTL/ObjectLifeCycle.h>
#ifdef __cplusplus
//---------------------------------------------------------------------------
#if cnLibrary_CPPFEATURE_CONSTEXPR >= 200704L
#define	cnRTL_CONSTEXPR_FUNC	constexpr
#define	cnRTL_CONSTVAR	constexpr
// cnLibrary_CPPFEATURE_CONSTEXPR >= 200704L
#else
// cnLibrary_CPPFEATURE_CONSTEXPR < 200704L
#define	cnRTL_CONSTEXPR_FUNC
#define	cnRTL_CONSTVAR	const
#endif // cnLibrary_CPPFEATURE_CONSTEXPR < 200704L


//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnRTL{
//---------------------------------------------------------------------------

// Automatic Object Pointer

//---------------------------------------------------------------------------
template<class T>
struct aTokenOperator_Auto
{
	static void Acquire(T* const &){}
	
	static void Release(T* const &Token){
		if(Token!=nullptr)
			delete Token;
	}

};
template<class T>
struct aTokenOperator_VLC
{
	class TVirtualCaller : public T, public cVirtualLifeCycleInstance
	{
	public:
		using T::VirtualStarted;
		using T::VirtualStopped;
	};

	static void Acquire(T* const &Token){
		cVirtualLifeCycleInstance::LifeCycleStart(static_cast<TVirtualCaller*>(Token));
	}

	static void Release(T* const &Token){
		if(Token!=nullptr){
			cVirtualLifeCycleInstance::LifeCycleStop(static_cast<TVirtualCaller*>(Token));
		}
	}
};
//---------------------------------------------------------------------------
template<class T>
struct aTokenOperator : cnVar::bcPointerOwnerTokenOperator<T*>
{
	static void Acquire(T* const &Token){
		return typename cnVar::TSelect<cnVar::TClassInheritFrom<T,bcVirtualLifeCycle>::Value,
			aTokenOperator_Auto<T>,
			aTokenOperator_VLC<T>
		>::Type::Acquire(Token);
	}

	static void Release(T* const &Token){
		return typename cnVar::TSelect<cnVar::TClassInheritFrom<T,bcVirtualLifeCycle>::Value,
			aTokenOperator_Auto<T>,
			aTokenOperator_VLC<T>
		>::Type::Release(Token);
	}
};
//---------------------------------------------------------------------------
template<class T>
using aPtr = cnVar::cPtrOwner< aTokenOperator<T> >;
//---------------------------------------------------------------------------
template<class T>
aPtr<T> aTake(T *Object)
{
	return aPtr<T>::TakeFromManual(Object);
}
//---------------------------------------------------------------------------
template<class T>
typename cnVar::TSelect<0,aPtr<T>
	, decltype(new T)
>::Type aCreate(void)
{
	auto Object=new T;
	aTokenOperator<T>::Acquire(Object);
	return aPtr<T>::TakeFromManual(Object);
}
template<class T,class...TArgs>
typename cnVar::TSelect<0,aPtr<T>
	, decltype(new T(cnVar::DeclVar<T>()))
>::Type aCreate(TArgs&&...Args)
{
	auto Object=new T(static_cast<TArgs&&>(Args)...);
	aTokenOperator<T>::Acquire(Object);
	return aPtr<T>::TakeFromManual(Object);
}
//---------------------------------------------------------------------------

// Automatic Reference Pointer

//---------------------------------------------------------------------------
template<class T,class TLifeCycleInstance>
class impReferenceCreateLifeCycleObject : public T, public TLifeCycleInstance, public cRTLAllocator
{
	friend TLifeCycleInstance;
public:
	using T::T;
#ifndef cnLibrary_CPPEXCLUDE_CLASS_MEMBER_DEFAULT
	~impReferenceCreateLifeCycleObject()=default;
#else
	~impReferenceCreateLifeCycleObject(){}
#endif


	static void LifeCycleStart(impReferenceCreateLifeCycleObject *Object){
		Object->fRefCount=1;

		TLifeCycleInstance::LifeCycleStart(Object);
	}

	virtual void cnLib_FUNC IncreaseReference(void)noexcept(true) override{
		cnLib_ASSERT(static_cast<uIntn>(fRefCount+1)>1);	// not -1 or 0
		fRefCount.Free++;
	}
	virtual void cnLib_FUNC DecreaseReference(void)noexcept(true) override{
		cnLib_ASSERT(fRefCount!=0);
		if(fRefCount.Free--==1){
			TLifeCycleInstance::LifeCycleStop(this);
		}
	}
private:
	cAtomicVar<uIntn> fRefCount;
};
//---------------------------------------------------------------------------
class bcObservedReference
{
public:
	class bcNotifyToken
	{
	public:
		virtual bool cnLib_FUNC InvalidationNotify(iObservedReference *Reference)=0;

		bcNotifyToken *Parent;
		bcNotifyToken *Child[2];
		ufInt8 Branch;
		ufInt8 Color;
	};
	static_assert(sizeof(bcNotifyToken)<=sizeof(iReferenceInvalidationNotify),"incompatiable iReferenceInvalidationNotify");

	void RefReset(void);
	void RefInvalidate(iObservedReference *ObservedReference);

	void RefInc(void);
	bool RefDec(void);

	void WeakRegister(iReferenceInvalidationNotify *NotifyToken);
	void WeakUnregister(iReferenceInvalidationNotify *NotifyToken);

	void WeakRegister(bcNotifyToken *NotifyToken);
	void WeakUnregister(bcNotifyToken *NotifyToken);
	bool WeakToStrong(void);
protected:
	cAtomicVar<uIntn> fRefCount;
	cAtomicVar<uIntn> fRegisterCount;
	cSpinLock fRefSetLock;
	cLinkItemSet<bcNotifyToken,cnDataStruct::cItemAddressOrderOperator<bcNotifyToken> > fRefSet;
};
//---------------------------------------------------------------------------
template<class T,class TLifeCycleInstance>
class impObservedReferenceCreateLifeCycleObject : public T, public TLifeCycleInstance, protected bcObservedReference, public cRTLAllocator
{
	friend TLifeCycleInstance;
public:
	using T::T;
#ifndef cnLibrary_CPPEXCLUDE_CLASS_MEMBER_DEFAULT
	~impObservedReferenceCreateLifeCycleObject()=default;
#else
	~impObservedReferenceCreateLifeCycleObject(){}
#endif

	
	static void LifeCycleStart(impObservedReferenceCreateLifeCycleObject *Object){
		Object->RefReset();

		TLifeCycleInstance::LifeCycleStart(Object);
	}
	static void LifeCycleStop(impObservedReferenceCreateLifeCycleObject *Object){
		Object->RefInvalidate(Object);

		TLifeCycleInstance::LifeCycleStop(Object);
	}

	virtual void cnLib_FUNC IncreaseReference(void)override{
		bcObservedReference::RefInc();
	}
	virtual void cnLib_FUNC DecreaseReference(void)override{
		if(bcObservedReference::RefDec()){
			LifeCycleStop(this);
		}
	}

	virtual	void cnLib_FUNC InvalidationRegisterNotification(iReferenceInvalidationNotify *Notify)override{
		return bcObservedReference::WeakRegister(Notify);
	}
	virtual	void cnLib_FUNC InvalidationUnregisterNotification(iReferenceInvalidationNotify *Notify)override{
		return bcObservedReference::WeakUnregister(Notify);
	}
	virtual	bool cnLib_FUNC Reference(void)override{
		return bcObservedReference::WeakToStrong();
	}

private:
	cAtomicVar<uIntn> fRefCount;
};
//---------------------------------------------------------------------------
template<class T>
struct rCreateTemplate : private T
{
	typedef typename cnVar::TSelect<cnVar::TClassInheritFrom<T,iObservedReference>::Value,
		typename cnVar::TSelect<cnVar::TClassInheritFrom<T,bcVirtualLifeCycle>::Value
			, cCPPLifeCycleManager< impReferenceCreateLifeCycleObject<T,cCPPLifeCycleInstance> >
			, cVirtualLifeCycleDefaultManager< impReferenceCreateLifeCycleObject<T,cVirtualLifeCycleInstance> >
		>::Type,
		typename cnVar::TSelect<cnVar::TClassInheritFrom<T,bcVirtualLifeCycle>::Value
			, cCPPLifeCycleManager< impObservedReferenceCreateLifeCycleObject<T,cCPPLifeCycleInstance> >
			, cVirtualLifeCycleDefaultManager< impObservedReferenceCreateLifeCycleObject<T,cVirtualLifeCycleInstance> >
		>::Type
	>::Type TLifeCycleManager;
};
//---------------------------------------------------------------------------
template<class T>
typename cnVar::TSelect<0,rPtr<T>
	, decltype(new typename rCreateTemplate<T>::TLifeCycleManager::tLifeCycleObject)
>::Type rCreate(void)
{
	typedef typename rCreateTemplate<T>::TLifeCycleManager tManager;
	auto Object=new typename tManager::tLifeCycleObject;
	tManager::ManageGlobal(Object);
	Object->LifeCycleStart(Object);
	return rPtr<T>::TakeFromManual(Object);
}
//---------------------------------------------------------------------------
template<class T,class...TArgs>
typename cnVar::TSelect<0,rPtr<T>
	, decltype(new typename rCreateTemplate<T>::TLifeCycleManager::tLifeCycleObject(cnVar::DeclVar<TArgs>()...))
>::Type rCreate(TArgs&&...Args)
{
	typedef typename rCreateTemplate<T>::TLifeCycleManager tManager;
	auto Object=new typename tManager::tLifeCycleObject(cnVar::Forward<TArgs>(Args)...);
	tManager::ManageGlobal(Object);
	Object->LifeCycleStart(Object);
	return rPtr<T>::TakeFromManual(Object);
}
//---------------------------------------------------------------------------
template<class T,class TRefereceInterace>
class impRefInterface : public T
{
public:
	using T::T;

	virtual void* cnLib_FUNC CastInterface(iTypeID InterfaceID)noexcept(true) override{
		if(InterfaceID==TInterfaceID<iReference>::Value){
			return static_cast<iReference*>(static_cast<TRefereceInterace*>(this));
		}
		return this->T::CastInterface(InterfaceID);
	}

};
//---------------------------------------------------------------------------
template<class T>
class impRefInterface<T,T> : public T, public iReference
{
public:
	using T::T;

	virtual void* cnLib_FUNC CastInterface(iTypeID InterfaceID)noexcept(true) override{
		if(InterfaceID==TInterfaceID<iReference>::Value)
			return static_cast<iReference*>(this);
		return T::CastInterface(InterfaceID);
	}

};
//---------------------------------------------------------------------------
template<class T,class TRefereceInterace>
class impObsRefInterface : public T
{
public:
	using T::T;

	virtual void* cnLib_FUNC CastInterface(iTypeID InterfaceID)override{
		if(InterfaceID==TInterfaceID<iReference>::Value){
			return static_cast<iReference*>(static_cast<TRefereceInterace*>(this));
		}
		if(InterfaceID==TInterfaceID<iObservedReference>::Value){
			return static_cast<iObservedReference*>(static_cast<TRefereceInterace*>(this));
		}
		return this->T::CastInterface(InterfaceID);
	}

};
//---------------------------------------------------------------------------
template<class T>
class impObsRefInterface<T,T> : public T, public iObservedReference
{
public:
	using T::T;

	virtual void* cnLib_FUNC CastInterface(iTypeID InterfaceID)override{
		if(InterfaceID==TInterfaceID<iReference>::Value)
			return static_cast<iReference*>(static_cast<iObservedReference*>(this));
		if(InterfaceID==TInterfaceID<iObservedReference>::Value){
			return static_cast<iObservedReference*>(this);
		}
		return T::CastInterface(InterfaceID);
	}

};
//---------------------------------------------------------------------------
template<class T>
struct iCreateTemplate
{
	typedef impRefInterface<T,typename cnVar::TClassReferenceInterface<T>::Type> TRefClass;
	typedef impObsRefInterface<T,typename cnVar::TClassReferenceInterface<T>::Type> TObsRefClass;

	typedef typename cnVar::TSelect<cnVar::TClassInheritFrom<T,iObservedReference>::Value,
		typename cnVar::TSelect<cnVar::TClassInheritFrom<T,bcVirtualLifeCycle>::Value
			, cCPPLifeCycleManager< impReferenceCreateLifeCycleObject<TRefClass,cCPPLifeCycleInstance> >
			, cVirtualLifeCycleDefaultManager< impReferenceCreateLifeCycleObject<TRefClass,cVirtualLifeCycleInstance> >
		>::Type,
		typename cnVar::TSelect<cnVar::TClassInheritFrom<T,bcVirtualLifeCycle>::Value
			, cCPPLifeCycleManager< impObservedReferenceCreateLifeCycleObject<TObsRefClass,cCPPLifeCycleInstance> >
			, cVirtualLifeCycleDefaultManager< impObservedReferenceCreateLifeCycleObject<TObsRefClass,cVirtualLifeCycleInstance> >
		>::Type
	>::Type TLifeCycleManager;
};
//---------------------------------------------------------------------------
template<class T>
typename cnVar::TSelect<0,iPtr<T>
	, decltype(new typename iCreateTemplate<T>::TLifeCycleManager::tLifeCycleObject)
>::Type iCreate(void)
{
	typedef typename iCreateTemplate<T>::TLifeCycleManager tManager;
	auto Object=new typename tManager::tLifeCycleObject;
	tManager::ManageGlobal(Object);
	Object->LifeCycleStart(Object);
	return iPtr<T>::TakeFromManual(Object);
}
//---------------------------------------------------------------------------
template<class T,class...TArgs>
typename cnVar::TSelect<0,iPtr<T>
	, decltype(new typename iCreateTemplate<T>::TLifeCycleManager::tLifeCycleObject(cnVar::DeclVar<TArgs>()...))
>::Type iCreate(TArgs&&...Args)
{
	typedef typename iCreateTemplate<T>::TLifeCycleManager tManager;
	auto Object=new typename tManager::tLifeCycleObject(cnVar::Forward<TArgs>(Args)...);
	tManager::ManageGlobal(Object);
	Object->LifeCycleStart(Object);
	return iPtr<T>::TakeFromManual(Object);
}
//---------------------------------------------------------------------------

// Recyclable Object

//---------------------------------------------------------------------------
template<class T>
class cAutoRecyclableObject
	: public T
	, public cnVar::TSelect<cnVar::TClassInheritFrom<T,bcVirtualLifeCycle>::Value
		, cCPPLifeCycleRecyclableInstance
		, cVirtualLifeCycleRecyclableInstance
	>::Type
{
	friend cVirtualLifeCycleRecyclableInstance;
};
//---------------------------------------------------------------------------
template<class T>
struct arPointerOwnerTokenOperator : cnVar::bcPointerOwnerTokenOperator<T*>
{
	static void Acquire(T* const &Token){
		if(Token!=nullptr){
			cAutoRecyclableObject<T>::LifeCycleStart(static_cast<cAutoRecyclableObject<T>*>(Token));
		}
	}

	static void Release(T* const &Token){
		if(Token!=nullptr){
			cAutoRecyclableObject<T>::LifeCycleStop(static_cast<cAutoRecyclableObject<T>*>(Token));
		}
	}
};
//---------------------------------------------------------------------------
template<class T>
using arPtr = cnVar::cPtrOwner< arPointerOwnerTokenOperator<T> >;
//---------------------------------------------------------------------------
template<class T>
inline arPtr<T> arCreateUnrecyclable(void){
	typedef typename cnVar::TSelect<cnVar::TClassInheritFrom<T,bcVirtualLifeCycle>::Value,
		cCPPLifeCycleManager< cAutoRecyclableObject<T> >,
		cVirtualLifeCycleDefaultManager< cAutoRecyclableObject<T> >
	>::Type TLifeCycleUnrecyclableManager;
	auto Object=new cAutoRecyclableObject<T>;
	TLifeCycleUnrecyclableManager::ManageGlobal(Object);
	aTokenOperator<T>::Acquire(Object);
	return arPtr<T>::TakeFromManual(Object);
}
//---------------------------------------------------------------------------
template<class T>
class arSharedObjectRecycler
{
public:
	typedef typename cnVar::TSelect<cnVar::TClassInheritFrom<T,bcVirtualLifeCycle>::Value,
		cCPPLifeCycleSharedRecyclableManager< cAutoRecyclableObject<T> >,
		cVirtualLifeCycleSharedRecyclableManager< cAutoRecyclableObject<T> >
	>::Type TLifeCycleSharedRecyclableManager;
	
	arSharedObjectRecycler() : fManager(GetSharedManager()){}
	~arSharedObjectRecycler(){}

	arPtr<T> operator ()(void){	return Query();	}
	arPtr<T> Query(void){
		auto Object=fManager->Query();
		Object->LifeCycleStart(Object);
		return arPtr<T>::TakeFromManual(Object);
	}

	static arPtr<T> QueryShared(void){
		auto Manager=GetSharedManager();
		auto Object=Manager->Query();
		Object->LifeCycleStart(Object);
		return arPtr<T>::TakeFromManual(Object);
	}

private:
	rPtr<TLifeCycleSharedRecyclableManager> fManager;
	
	static TLifeCycleSharedRecyclableManager* GetSharedManager(void){
		return cnVar::StaticInitializedSinglton<TLifeCycleSharedRecyclableManager>();
	}
};
//---------------------------------------------------------------------------
template<class T>
inline arPtr<T> arQuerySharedObject(void){
	return arSharedObjectRecycler<T>::QueryShared();
}
//---------------------------------------------------------------------------
template<class T>
class arObjectRecycler
{
public:
	typedef typename cnVar::TSelect<cnVar::TClassInheritFrom<T,bcVirtualLifeCycle>::Value,
		cCPPLifeCycleRecyclableManager< cAutoRecyclableObject<T> >,
		cVirtualLifeCycleRecyclableManager< cAutoRecyclableObject<T> >
	>::Type TLifeCycleRecyclableManager;

	arObjectRecycler() : fManager(rCreate<TLifeCycleRecyclableManager>()){}
	~arObjectRecycler(){}

	arPtr<T> operator ()(void){	Query();	}
	arPtr<T> Query(void){
		auto Object=fManager->Query();
		Object->LifeCycleStart(Object);
		return arPtr<T>::TakeFromManual(Object);
	}

private:
	rPtr<TLifeCycleRecyclableManager> fManager;
};
//---------------------------------------------------------------------------

// Recyclable Reference

//---------------------------------------------------------------------------
template<class T>
class rSharedObjectRecycler
{
public:
	typedef typename cnVar::TSelect<cnVar::TClassInheritFrom<T,iObservedReference>::Value,
		typename cnVar::TSelect<cnVar::TClassInheritFrom<T,bcVirtualLifeCycle>::Value,
			cCPPLifeCycleSharedRecyclableManager< impReferenceCreateLifeCycleObject<T,cCPPLifeCycleRecyclableInstance> >,
			cVirtualLifeCycleSharedRecyclableManager< impReferenceCreateLifeCycleObject<T,cVirtualLifeCycleRecyclableInstance> >
		>::Type,
		typename cnVar::TSelect<cnVar::TClassInheritFrom<T,bcVirtualLifeCycle>::Value,
			cCPPLifeCycleSharedRecyclableManager< impObservedReferenceCreateLifeCycleObject<T,cCPPLifeCycleRecyclableInstance> >,
			cVirtualLifeCycleSharedRecyclableManager< impObservedReferenceCreateLifeCycleObject<T,cVirtualLifeCycleRecyclableInstance> >
		>::Type
	>::Type TLifeCycleSharedRecyclableManager;
	rSharedObjectRecycler() : fManager(GetSharedManager()){}
	~rSharedObjectRecycler(){}

	rPtr<T> operator ()(void){	Query();	}
	rPtr<T> Query(void){
		auto Object=fManager->Query();
		Object->LifeCycleStart(Object);
		return rPtr<T>::TakeFromManual(Object);
	}

	static rPtr<T> QueryShared(void){
		auto Instance=GetSharedManager();
		auto Object=Instance->Query();
		Object->LifeCycleStart(Object);
		return rPtr<T>::TakeFromManual(Object);
	}
private:

	rPtr<TLifeCycleSharedRecyclableManager> fManager;
	
	static TLifeCycleSharedRecyclableManager* GetSharedManager(void){
		return cnVar::StaticInitializedSinglton<TLifeCycleSharedRecyclableManager>();
	}
};
//---------------------------------------------------------------------------
template<class T>
inline rPtr<T> rQuerySharedObject(void){
	return rSharedObjectRecycler<T>::QueryShared();
}
//---------------------------------------------------------------------------
template<class T>
class rObjectRecycler
{
public:
	typedef typename cnVar::TSelect<cnVar::TClassInheritFrom<T,iObservedReference>::Value,
		typename cnVar::TSelect<cnVar::TClassInheritFrom<T,bcVirtualLifeCycle>::Value,
			cCPPLifeCycleRecyclableManager< impReferenceCreateLifeCycleObject<T,cCPPLifeCycleRecyclableInstance> >,
			cVirtualLifeCycleRecyclableManager< impReferenceCreateLifeCycleObject<T,cVirtualLifeCycleRecyclableInstance> >
		>::Type,
		typename cnVar::TSelect<cnVar::TClassInheritFrom<T,bcVirtualLifeCycle>::Value,
			cCPPLifeCycleRecyclableManager< impObservedReferenceCreateLifeCycleObject<T,cCPPLifeCycleRecyclableInstance> >,
			cVirtualLifeCycleRecyclableManager< impObservedReferenceCreateLifeCycleObject<T,cVirtualLifeCycleRecyclableInstance> >
		>::Type
	>::Type TLifeCycleRecyclableManager;
	rObjectRecycler() : fManager(rCreate<TLifeCycleRecyclableManager>()){}
	~rObjectRecycler(){}

	rPtr<T> operator ()(void){	Query();	}
	rPtr<T> Query(void){
		auto Object=fManager->Query();
		Object->LifeCycleStart(Object);
		return rPtr<T>::TakeFromManual(Object);
	}
private:
	rPtr<TLifeCycleRecyclableManager> fManager;
};
//---------------------------------------------------------------------------
template<class T>
class iSharedObjectRecycler
{
public:
	typedef impRefInterface<T,typename cnVar::TClassReferenceInterface<T>::Type> TRefClass;
	typedef impObsRefInterface<T,typename cnVar::TClassReferenceInterface<T>::Type> TObsRefClass;
	typedef typename cnVar::TSelect<cnVar::TClassInheritFrom<T,iObservedReference>::Value,
		typename cnVar::TSelect<cnVar::TClassInheritFrom<T,bcVirtualLifeCycle>::Value,
			cCPPLifeCycleSharedRecyclableManager< impReferenceCreateLifeCycleObject<TRefClass,cCPPLifeCycleRecyclableInstance> >,
			cVirtualLifeCycleSharedRecyclableManager< impReferenceCreateLifeCycleObject<TRefClass,cVirtualLifeCycleRecyclableInstance> >
		>::Type,
		typename cnVar::TSelect<cnVar::TClassInheritFrom<T,bcVirtualLifeCycle>::Value,
			cCPPLifeCycleSharedRecyclableManager< impObservedReferenceCreateLifeCycleObject<TObsRefClass,cCPPLifeCycleRecyclableInstance> >,
			cVirtualLifeCycleSharedRecyclableManager< impObservedReferenceCreateLifeCycleObject<TObsRefClass,cVirtualLifeCycleRecyclableInstance> >
		>::Type
	>::Type TLifeCycleSharedRecyclableManager;
	iSharedObjectRecycler()	: fManager(GetSharedManager()){}
	~iSharedObjectRecycler(){}

	iPtr<T> operator ()(void){	Query();	}
	iPtr<T> Query(void){
		auto Object=fManager->Query();
		Object->LifeCycleStart(Object);
		return iPtr<T>::TakeFromManual(Object);
	}
	
	static iPtr<T> QueryShared(void){
		auto Instance=GetSharedManager();
		auto Object=Instance->Query();
		Object->LifeCycleStart(Object);
		return iPtr<T>::TakeFromManual(Object);
	}
private:

	rPtr<TLifeCycleSharedRecyclableManager> fManager;
	
	static TLifeCycleSharedRecyclableManager* GetSharedManager(void){
		return cnVar::StaticInitializedSinglton<TLifeCycleSharedRecyclableManager>();
	}
};
//---------------------------------------------------------------------------
template<class T>
inline iPtr<T> iQuerySharedObject(void){
	return iSharedObjectRecycler<T>::QueryShared();
}
//---------------------------------------------------------------------------
template<class T>
class iObjectRecycler
{
public:
	typedef impRefInterface<T,typename cnVar::TClassReferenceInterface<T>::Type> TRefClass;
	typedef impObsRefInterface<T,typename cnVar::TClassReferenceInterface<T>::Type> TObsRefClass;
	typedef typename cnVar::TSelect<cnVar::TClassInheritFrom<T,iObservedReference>::Value,
		typename cnVar::TSelect<cnVar::TClassInheritFrom<T,bcVirtualLifeCycle>::Value,
			cCPPLifeCycleRecyclableManager< impReferenceCreateLifeCycleObject<TRefClass,cCPPLifeCycleRecyclableInstance> >,
			cVirtualLifeCycleRecyclableManager< impReferenceCreateLifeCycleObject<TRefClass,cVirtualLifeCycleRecyclableInstance> >
		>::Type,
		typename cnVar::TSelect<cnVar::TClassInheritFrom<T,bcVirtualLifeCycle>::Value,
			cCPPLifeCycleRecyclableManager< impObservedReferenceCreateLifeCycleObject<TObsRefClass,cCPPLifeCycleRecyclableInstance> >,
			cVirtualLifeCycleRecyclableManager< impObservedReferenceCreateLifeCycleObject<TObsRefClass,cVirtualLifeCycleRecyclableInstance> >
		>::Type
	>::Type TLifeCycleRecyclableManager;

	iObjectRecycler() : fManager(rCreate<TLifeCycleRecyclableManager>()){}
	~iObjectRecycler(){}

	iPtr<T> operator ()(void){	Query();	}
	iPtr<T> Query(void){
		auto Object=fManager->Query();
		Object->LifeCycleStart(Object);
		return iPtr<T>::TakeFromManual(Object);
	}
private:
	rPtr<TLifeCycleRecyclableManager> fManager;
};
//---------------------------------------------------------------------------

// Class Reference

//---------------------------------------------------------------------------
class cClassReferenceWithWeakSet
{
public:
	cClassReferenceWithWeakSet();
	~cClassReferenceWithWeakSet();

	class bcNotifyToken
	{
	public:
		virtual bool cnLib_FUNC InvalidationNotify(cClassReferenceWithWeakSet *Token)=0;

		bcNotifyToken *Parent;
		bcNotifyToken *Child[2];
		ufInt8 Branch;
		ufInt8 Color;
	};

	void RefReset(void);
	void RefInvalidate(void);
	
	void RefInc(void);
	bool RefDec(void);

	void WeakRegister(bcNotifyToken *NotifyToken);
	void WeakUnregister(bcNotifyToken *NotifyToken);
	bool WeakToStrong(void);
protected:
	cAtomicVar<uIntn> fRefCount;
	cAtomicVar<uIntn> fRegisterCount;
	cSpinLock fRefSetLock;
	cLinkItemSet<bcNotifyToken,cnDataStruct::cItemAddressOrderOperator<bcNotifyToken> > fRefSet;

};
template<class T>	struct cAutoClassWeakRefTokenOperator;
//---------------------------------------------------------------------------
template<class T>	struct cAutoClassRefTokenOperator;
template<class T>
class aCls : public T
	, cnVar::TSelect<cnVar::TClassInheritFrom<T,bcVirtualLifeCycle>::Value,
		cCPPLifeCycleInstance,cVirtualLifeCycleInstance
	>::Type
	, protected cClassReferenceWithWeakSet, protected cRTLAllocator
{
	friend cAutoClassRefTokenOperator<T>;
	friend cAutoClassRefTokenOperator<const T>;

	friend cAutoClassWeakRefTokenOperator<aCls>;

	typedef typename cnVar::TSelect<cnVar::TClassInheritFrom<T,bcVirtualLifeCycle>::Value
		, cCPPLifeCycleInstance
		, cVirtualLifeCycleInstance
	>::Type tLiftCycleInstance;
	friend tLiftCycleInstance;
	typedef typename cnVar::TSelect<cnVar::TClassInheritFrom<T,bcVirtualLifeCycle>::Value
			, cCPPLifeCycleManager< aCls >
			, cVirtualLifeCycleDefaultManager< aCls >
	>::Type tLifeCycleManager;
	friend tLifeCycleManager;

public:
	typedef T tClass;
	using T::T;
	using T::operator =;
protected:

	template<class T>
	static void LifeCycleStart(T *Object){
		Object->RefReset();
		tLiftCycleInstance::LifeCycleStart(Object);
	}
	template<class T>
	static void LifeCycleStop(T *Object){
		Object->RefInvalidate();
		tLiftCycleInstance::LifeCycleStop(Object);
	}

	void RefClassAcquireReference(void){	RefInc();	}
	void RefClassReleaseReference(void){	if(RefDec()){	LifeCycleStop(this);	}	}
};
//---------------------------------------------------------------------------
template<class T>
struct cAutoClassRefTokenOperator : cnVar::bcPointerRefTokenOperator< aCls<T>* >
{
	static void Acquire(aCls<T> *Token){	if(Token!=nullptr)	Token->RefClassAcquireReference();	}
	static void Release(aCls<T> *Token){	if(Token!=nullptr)	Token->RefClassReleaseReference();	}

	template<class=decltype(new aCls<T>)>
	static cnVar::cPtrReference<cAutoClassRefTokenOperator> Create(void)
	{
		auto NewObject=new aCls<T>;
		aCls<T>::tLifeCycleManager::ManageGlobal(NewObject);
		aCls<T>::LifeCycleStart(NewObject);
		return cnVar::cPtrReference<cAutoClassRefTokenOperator>::TakeFromManual(NewObject);
	}
	template<class...TArgs>
	static typename cnVar::TSelect<0,cnVar::cPtrReference<cAutoClassRefTokenOperator>
		, decltype(new aCls<T>(cnVar::DeclVar<TArgs>()...))
	>::Type Create(TArgs&&...Args)
	{
		auto NewObject=new aCls<T>(cnVar::Forward<TArgs>(Args)...);
		aCls<T>::tLifeCycleManager::ManageGlobal(NewObject);
		aCls<T>::LifeCycleStart(NewObject);
		return cnVar::cPtrReference<cAutoClassRefTokenOperator>::TakeFromManual(NewObject);
	}

};
//---------------------------------------------------------------------------
template<class T>
struct cAutoClassRefTokenOperator<const T> : cnVar::bcPointerRefTokenOperator< const aCls<T>* >
{
	static void Acquire(const aCls<T> *Token){	if(Token!=nullptr)	const_cast<aCls<T>*>(Token)->RefClassAcquireReference();	}
	static void Release(const aCls<T> *Token){	if(Token!=nullptr)	const_cast<aCls<T>*>(Token)->RefClassReleaseReference();	}
};
//---------------------------------------------------------------------------
template<class T>
using aClsRef = cnVar::cPtrReference< cAutoClassRefTokenOperator<T> >;
template<class T>
using aClsConstRef = cnVar::cPtrReference< cAutoClassRefTokenOperator<const T> >;
template<class T>
using aClsAtomicRef = cnVar::cAtomicPtrReference< cAutoClassRefTokenOperator<T> >;

//---------------------------------------------------------------------------
template<class T>	struct cAutoRecyclableClassRefTokenOperator;
template<class T>
class arCls : public T
	, cnVar::TSelect<cnVar::TClassInheritFrom<T,bcVirtualLifeCycle>::Value,
		cCPPLifeCycleRecyclableInstance,cVirtualLifeCycleRecyclableInstance
	>::Type
	, protected cClassReferenceWithWeakSet, protected cRTLAllocator
{
	friend cAutoRecyclableClassRefTokenOperator<T>;
	friend cAutoRecyclableClassRefTokenOperator<const T>;
	friend cAutoClassWeakRefTokenOperator<arCls>;
	typedef typename cnVar::TSelect<cnVar::TClassInheritFrom<T,bcVirtualLifeCycle>::Value
		, cCPPLifeCycleRecyclableInstance
		, cVirtualLifeCycleRecyclableInstance
	>::Type tLifeCycleInstance;
	friend tLifeCycleInstance;

	typedef typename cnVar::TSelect<cnVar::TClassInheritFrom<T,bcVirtualLifeCycle>::Value
			, cCPPLifeCycleManager< arCls >
			, cVirtualLifeCycleDefaultManager< arCls >
	>::Type tLifeCycleManager;
	friend tLifeCycleManager;
public:
	typedef T tClass;
	using T::T;
	using T::operator =;
protected:

	template<class T>
	static void LifeCycleStart(T *Object){
		Object->RefReset();
		tLifeCycleInstance::LifeCycleStart(Object);
	}
	template<class T>
	static void LifeCycleStop(T *Object){
		Object->RefInvalidate();
		tLifeCycleInstance::LifeCycleStop(Object);
	}

	void RefClassAcquireReference(void){	RefInc();	}
	void RefClassReleaseReference(void){	if(RefDec()){	LifeCycleStop(this);	}	}

};
//---------------------------------------------------------------------------
template<class T>
struct cAutoRecyclableClassRefTokenOperator : cnVar::bcPointerRefTokenOperator< arCls<T>* >
{
	static void Acquire(arCls<T> *Token){	if(Token!=nullptr)	Token->RefClassAcquireReference();	}
	static void Release(arCls<T> *Token){	if(Token!=nullptr)	Token->RefClassReleaseReference();	}

	template<class=decltype(new arCls<T>)>
	static cnVar::cPtrReference<cAutoRecyclableClassRefTokenOperator> Create(void)
	{
		auto NewObject=new arCls<T>;
		arCls<T>::LifeCycleStart(NewObject);
		return cnVar::cPtrReference<cAutoRecyclableClassRefTokenOperator>::TakeFromManual(NewObject);
	}

};
//---------------------------------------------------------------------------
template<class T>
struct cAutoRecyclableClassRefTokenOperator<const T> : cnVar::bcPointerRefTokenOperator< const arCls<T>* >
{
	static void Acquire(const aCls<T> *Token){	if(Token!=nullptr)	const_cast<aCls<T>*>(Token)->RefClassAcquireReference();	}
	static void Release(const aCls<T> *Token){	if(Token!=nullptr)	const_cast<aCls<T>*>(Token)->RefClassReleaseReference();	}
};
//---------------------------------------------------------------------------
template<class T>
using arClsRef = cnVar::cPtrReference< cAutoRecyclableClassRefTokenOperator<T> >;
template<class T>
using arClsConstRef = cnVar::cPtrReference< cAutoRecyclableClassRefTokenOperator<const T> >;
template<class T>
using arClsAtomicRef = cnVar::cAtomicPtrReference< cAutoRecyclableClassRefTokenOperator<T> >;

//---------------------------------------------------------------------------
template<class TAutoClass>
struct cAutoClassWeakRefTokenOperator
{
	typedef TAutoClass* tPtr;
	typedef cnVar::cPtrReference< cAutoClassRefTokenOperator<typename TAutoClass::tClass> > tRef;
	typedef cClassReferenceWithWeakSet* tRegToken;
	typedef cClassReferenceWithWeakSet::bcNotifyToken tNotifyToken;

	static tRegToken Register(tRef Ref,tNotifyToken *NotifyToken){
		if(Ref==nullptr)
			return nullptr;
		Ref->WeakRegister(NotifyToken);
		return Ref;
	}
	static tRegToken Register(tPtr Pointer,tNotifyToken *NotifyToken){
		if(Pointer==nullptr)
			return nullptr;
		Pointer->WeakRegister(NotifyToken);
		return Pointer;
	}
	static void Unregister(tRegToken RegToken,tNotifyToken *NotifyToken){
		return RegToken->WeakUnregister(NotifyToken);
	}
	static tRef Reference(tRegToken RegToken){
		return RegToken->WeakToStrong()?tRef::TakeFromManual(static_cast<tPtr>(RegToken)):nullptr;
	}
};
//---------------------------------------------------------------------------
template<class T>
using aClsWeakRef=cnVar::cPtrWeakReference< cAutoClassWeakRefTokenOperator< aCls<T> > >;
template<class T>
using arClsWeakRef=cnVar::cPtrWeakReference< cAutoClassWeakRefTokenOperator< arCls<T> > >;
//---------------------------------------------------------------------------
template<class T>
aCls<T>* aClsFromPtr(T *Src)
{
	return static_cast<aCls<T>*>(Src);
}
//---------------------------------------------------------------------------
template<class T>
aClsRef<T> aClsTake(aCls<T> *Src,uInt32 Tag)
{
	cnRTL_DEBUG_LOG_REFERENCE_DEC(Src,Tag);
	return aClsRef<T>::TakeFromManual(Src);
}
//---------------------------------------------------------------------------
template<class T>
aCls<T>* aClsExtract(aClsRef<T> &Src,uInt32 Tag)
{
	cnRTL_DEBUG_LOG_REFERENCE_INC(static_cast<aCls<T>*>(Src),Tag);
	return Src.ExtractToManual();
}
//---------------------------------------------------------------------------
template<class T,class...TArgs>
typename cnVar::TSelect<0,aClsRef<T>
	, decltype(cAutoClassRefTokenOperator<T>::Create(cnVar::DeclVar<TArgs>()...))
>::Type aClsCreate(TArgs&&...Args)
{
	return cAutoClassRefTokenOperator<T>::Create(cnVar::Forward<TArgs>(Args)...);
}
//---------------------------------------------------------------------------
template<class T>
arCls<T>* arClsFromPtr(T *Src)
{
	return static_cast<arCls<T>*>(Src);
}
//---------------------------------------------------------------------------
template<class T>
arClsRef<T> arClsTake(arCls<T> *Src,uInt32 Tag)
{
	cnRTL_DEBUG_LOG_REFERENCE_DEC(Src,Tag);
	return arClsRef<T>::TakeFromManual(Src);
}
//---------------------------------------------------------------------------
template<class T>
arCls<T>* arClsExtract(arClsRef<T> &Src,uInt32 Tag)
{
	cnRTL_DEBUG_LOG_REFERENCE_INC(static_cast<arCls<T>*>(Src),Tag);
	return Src.ExtractToManual();
}
//---------------------------------------------------------------------------
template<class T>
class arClsSharedObjectRecycler
{
public:
	typedef typename cnVar::TSelect<cnVar::TClassInheritFrom<T,bcVirtualLifeCycle>::Value,
		cCPPLifeCycleSharedRecyclableManager< arCls<T> >,
		cVirtualLifeCycleSharedRecyclableManager< arCls<T> >
	>::Type TLifeCycleSharedRecyclableManager;
	arClsSharedObjectRecycler() : fManager(GetSharedManager()){}
	~arClsSharedObjectRecycler(){}

	arClsRef<T> operator ()(void){	return Query();	}
	arClsRef<T> Query(void){
		auto Object=fManager->Query();
		TLifeCycleSharedRecyclableManager::NotifyObjectStart(Object);
		return arClsRef<T>::TakeFromManual(Object);
	}

	static arClsRef<T> QueryShared(void){
		auto Instance=GetSharedManager();
		auto Object=Instance->Query();
		TLifeCycleSharedRecyclableManager::NotifyObjectStart(Object);
		return arClsRef<T>::TakeFromManual(Object);
	}
private:

	rPtr<TLifeCycleSharedRecyclableManager> fManager;
	
	static TLifeCycleSharedRecyclableManager* GetSharedManager(void){
		return cnVar::StaticInitializedSinglton<TLifeCycleSharedRecyclableManager>();
	}
};
//---------------------------------------------------------------------------
template<class T>
inline arClsRef<T> arClsQuerySharedObject(void){
	return arClsSharedObjectRecycler<T>::QueryShared();
}
//---------------------------------------------------------------------------
template<class T>
class arClsObjectRecycler
{
public:
	typedef typename cnVar::TSelect<cnVar::TClassInheritFrom<T,bcVirtualLifeCycle>::Value,
		cCPPLifeCycleRecyclableManager< arCls<T> >,
		cVirtualLifeCycleRecyclableManager< arCls<T> >
	>::Type TLifeCycleRecyclableManager;

	arClsObjectRecycler() : fManager(rCreate<TLifeCycleRecyclableManager>()){}
	~arClsObjectRecycler(){}

	arClsRef<T> operator ()(void){	Query();	}
	arClsRef<T> Query(void){
		auto Object=fManager->Query();
		TLifeCycleRecyclableManager::NotifyObjectStart(Object);
		return arClsRef<T>::TakeFromManual(Object);
	}
private:
	rPtr<TLifeCycleRecyclableManager> fManager;
};
//---------------------------------------------------------------------------
}	// namespace cnRTL
//---------------------------------------------------------------------------
}   // namespace cnLibrary
//---------------------------------------------------------------------------
#undef	cnRTL_CONSTEXPR_FUNC
#undef	cnRTL_CONSTVAR
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif
