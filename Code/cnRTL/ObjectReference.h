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
class cDualReference : public bcVirtualLifeCycle
{
public:
	struct cPointerReferenceOperator
	{
		template<class T>
		static void Acquire(T *Pointer)noexcept(true){
			cnLib_ASSERT(Pointer->cDualReference::fInnerReference.Ref!=0);
			Pointer->fInnerReference.Inc();
		}
		template<class T>
		static void Release(T *Pointer)noexcept(true){
			cnLib_ASSERT(Pointer->cDualReference::fInnerReference.Ref!=0);
			Pointer->fInnerReference.Dec();
		}
	};
protected:
	class cInner : public iReference
	{
	public:
		cAtomicVar<uIntn> Ref=0;
		virtual void cnLib_FUNC IncreaseReference(void)noexcept(true) override{
			cnLib_ASSERT(Ref!=0);
			return Inc();
		}
		virtual void cnLib_FUNC DecreaseReference(void)noexcept(true) override{
			cnLib_ASSERT(Ref!=0);
			return Dec();
		}

		void Inc(void)noexcept(true);
		void Dec(void)noexcept(true);
	}fInnerReference;

	cDualReference()noexcept(true){}

	void InnerActivate(uInt32 Tag)noexcept(true){
		cnLib_ASSERT(fInnerReference.Ref==0);
		cnRTL_DEBUG_LOG_REFERENCE_INC(this,Tag);
		return fInnerReference.Inc();
	}

	void InnerIncReference(uInt32 Tag)noexcept(true){
		cnLib_ASSERT(fInnerReference.Ref!=0);
		cnRTL_DEBUG_LOG_REFERENCE_INC(this,Tag);
		return fInnerReference.Inc();
	}
	void InnerDecReference(uInt32 Tag)noexcept(true){
		cnLib_ASSERT(fInnerReference.Ref!=0);
		cnRTL_DEBUG_LOG_REFERENCE_DEC(this,Tag);
		return fInnerReference.Dec();
	}

	// override virtual life cycle start to increase inner reference
	void VirtualStarted(void)noexcept(true){
		InnerActivate('self');
	}
	// override virtual life cycle stop to decrease inner reference
	void VirtualStopped(void)noexcept(true){
		InnerDecReference('self');
	}
};
//---------------------------------------------------------------------------
template<class T>
using rInnerPtr = cnVar::cPtrReference<T,cDualReference::cPointerReferenceOperator>;
//---------------------------------------------------------------------------

// Automatic Object Pointer

//---------------------------------------------------------------------------
struct aPointerOwnerOperator
{
	template<class T>
	static void Manage(T *Pointer)noexcept(true){
		typedef typename cnVar::TSelect<cnVar::TClassIsInheritFrom<T,bcVirtualLifeCycle>::Value,
			cCPPLifeCycleSharedManager<T>,
			cVirtualLifeCycleSharedManager<T>
		>::Type tManager;
		tManager::ManageShared(Pointer);
		tManager::tLifeCycleActivation::Start(Pointer);
	}

	template<class T>
	static void Release(T *Pointer)noexcept(true){
		cnVar::TSelect<cnVar::TClassIsInheritFrom<T,bcVirtualLifeCycle>::Value,
			cCPPLifeCycleSharedManager<T>,
			cVirtualLifeCycleSharedManager<T>
		>::Type::tLifeCycleActivation::Stop(Pointer);
	}
};
//---------------------------------------------------------------------------
template<class T>
using aPtr = cnVar::cPtrOwner<T,aPointerOwnerOperator>;
//---------------------------------------------------------------------------
template<class T>
inline aPtr<T> aTake(T *Object)noexcept(true)
{
	return aPtr<T>::TakeFromManual(Object);
}
//---------------------------------------------------------------------------
template<class T>
//inline typename cnVar::TSelect<0,aPtr<T>
//	, decltype(new T)
//>::Type aCreate(void)noexcept(true)
inline aPtr<T> aCreate(void)noexcept(true)
{
	auto Object=new T;
	aPointerOwnerOperator::Manage(Object);
	return aPtr<T>::TakeFromManual(Object);
}
template<class T,class...TArgs>
//inline typename cnVar::TSelect<0,aPtr<T>
//	, decltype(new T(cnVar::DeclVal<T>()))
//>::Type aCreate(TArgs&&...Args)noexcept(true)
inline aPtr<T> aCreate(TArgs&&...Args)noexcept(true)
{
	auto Object=new T(static_cast<TArgs&&>(Args)...);
	aPointerOwnerOperator::Manage(Object);
	return aPtr<T>::TakeFromManual(Object);
}
//---------------------------------------------------------------------------

// Automatic Reference Pointer

//---------------------------------------------------------------------------
template<class T,class TLifeCycleInstance>
class impReferenceLifeCycleObject : public T, public TLifeCycleInstance, public cRTLAllocator
{
public:
	using T::T;
	~impReferenceLifeCycleObject()=default;

	template<class TLifeCycleObject>
	struct tActivation
	{
		static void Start(TLifeCycleObject *Object)noexcept(true){
			Object->impReferenceLifeCycleObject::fRefCount=1;
			TLifeCycleInstance::template tActivation<impReferenceLifeCycleObject>::Start(Object);
		}
		static void Stop(TLifeCycleObject *Object)noexcept(true){
			TLifeCycleInstance::template tActivation<impReferenceLifeCycleObject>::Stop(Object);
		}
	};
	friend bcVirtualLifeCycle::cActivation<impReferenceLifeCycleObject>;

	virtual void cnLib_FUNC IncreaseReference(void)noexcept(true) override{
		cnLib_ASSERT(static_cast<uIntn>(fRefCount+1)>1);	// not -1 or 0
		fRefCount.Free++;
	}
	virtual void cnLib_FUNC DecreaseReference(void)noexcept(true) override{
		cnLib_ASSERT(fRefCount!=0);
		if(fRefCount.Free--==1){
			tActivation<impReferenceLifeCycleObject>::Stop(this);
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
		virtual bool cnLib_FUNC InvalidationNotify(iObservedReference *Reference)noexcept(true)=0;

		bcNotifyToken *Parent;
		bcNotifyToken *Child[2];
		ufInt8 Branch;
		ufInt8 Color;
	};
	//static_assert(sizeof(bcNotifyToken)<=sizeof(iReferenceInvalidationNotify),"incompatiable iReferenceInvalidationNotify");

	void RefReset(void)noexcept(true);
	void RefInvalidate(iObservedReference *ObservedReference)noexcept(true);

	void RefInc(void)noexcept(true);
	bool RefDec(void)noexcept(true);

	//void WeakRegister(iReferenceInvalidationNotify *NotifyToken)noexcept(true);
	//void WeakUnregister(iReferenceInvalidationNotify *NotifyToken)noexcept(true);

	void WeakRegister(bcNotifyToken *NotifyToken)noexcept(true);
	void WeakUnregister(bcNotifyToken *NotifyToken)noexcept(true);
	bool WeakToStrong(void)noexcept(true);
protected:
	cAtomicVar<uIntn> fRefCount;
	cAtomicVar<uIntn> fRegisterCount;
	cSpinLock fRefSetLock;
	cLinkItemSet<bcNotifyToken,cnDataStruct::cItemAddressOrderOperator<bcNotifyToken> > fRefSet;
};
//---------------------------------------------------------------------------
template<class T,class TLifeCycleInstance>
class impObservedReferenceLifeCycleObject : public T, public TLifeCycleInstance, protected bcObservedReference, public cRTLAllocator
{
public:
	using T::T;
	~impObservedReferenceLifeCycleObject()=default;

	template<class TLifeCycleObject>
	struct tActivation : TLifeCycleInstance::template tActivation<TLifeCycleObject>
	{
		static void Start(TLifeCycleObject *Object)noexcept(true){
			Object->impObservedReferenceLifeCycleObject::RefReset();
			TLifeCycleInstance::template tActivation<impObservedReferenceLifeCycleObject>::Start(Object);
		}
		static void Stop(TLifeCycleObject *Object)noexcept(true){
			Object->impObservedReferenceLifeCycleObject::RefInvalidate(Object);
			TLifeCycleInstance::template tActivation<impObservedReferenceLifeCycleObject>::Stop(Object);
		}
	};

	friend bcVirtualLifeCycle::cActivation<impObservedReferenceLifeCycleObject>;

	virtual void cnLib_FUNC IncreaseReference(void)noexcept(true)override{
		bcObservedReference::RefInc();
	}
	virtual void cnLib_FUNC DecreaseReference(void)noexcept(true)override{
		if(bcObservedReference::RefDec()){
			tActivation<impObservedReferenceLifeCycleObject>::Stop(this);
		}
	}

	virtual iReferenceObserver* cnLib_FUNC CreateReferenceObserver(iReference *Reference,iProcedure *Procedure)noexcept(true)override{
		return nullptr;
	}

	//virtual	void cnLib_FUNC InvalidationRegisterNotification(iReferenceInvalidationNotify *Notify)noexcept(true)override{
	//	return bcObservedReference::WeakRegister(Notify);
	//}
	//virtual	void cnLib_FUNC InvalidationUnregisterNotification(iReferenceInvalidationNotify *Notify)noexcept(true)override{
	//	return bcObservedReference::WeakUnregister(Notify);
	//}
	//virtual	bool cnLib_FUNC Reference(void)noexcept(true)override{
	//	return bcObservedReference::WeakToStrong();
	//}

};
//---------------------------------------------------------------------------
template<class T>
struct TReferenceLifeCycleManagerSelect;
//---------------------------------------------------------------------------
template<class T>
struct TReferenceLifeCycleTypes
{
	typedef typename cnVar::TSelect<cnVar::TClassIsInheritFrom<T,bcVirtualLifeCycle>::Value
		, cCPPLifeCycleInstance
		, cVirtualLifeCycleInstance
	>::Type tLifeCycleInstance;

	typedef typename cnVar::TSelect<cnVar::TClassIsInheritFrom<T,iObservedReference>::Value
		, impReferenceLifeCycleObject<T,tLifeCycleInstance>
		, impObservedReferenceLifeCycleObject<T,tLifeCycleInstance>
	>::Type tLifeCycleObject;

	typedef typename cnVar::TSelect<cnVar::TClassIsInheritFrom<T,bcVirtualLifeCycle>::Value
		, cCPPLifeCycleSharedManager<tLifeCycleObject>
		, cVirtualLifeCycleSharedManager<tLifeCycleObject>
	>::Type tLifeCycleManager;

};
//---------------------------------------------------------------------------
template<class T>
//inline typename cnVar::TSelect<0,rPtr<T>
//	, decltype(new typename TReferenceLifeCycleTypes<T>::tLifeCycleObject)
//>::Type rCreate(void)noexcept(true)
inline rPtr<T> rCreate(void)noexcept(true)
{
	typedef typename TReferenceLifeCycleTypes<T>::tLifeCycleManager tLifeCycleManager;
	auto Object=new typename tLifeCycleManager::tLifeCycleObject;
	tLifeCycleManager::ManageShared(Object);
	tLifeCycleManager::tLifeCycleActivation::Start(Object);
	return rPtr<T>::TakeFromManual(Object);
}
//---------------------------------------------------------------------------
template<class T,class...TArgs>
//inline typename cnVar::TSelect<0,rPtr<T>
//	, decltype(new typename TReferenceLifeCycleTypes<T>::tLifeCycleObject(cnVar::DeclVal<TArgs>()...))
//>::Type rCreate(TArgs&&...Args)noexcept(true)
inline rPtr<T> rCreate(TArgs&&...Args)noexcept(true)
{
	typedef typename TReferenceLifeCycleTypes<T>::tLifeCycleManager tLifeCycleManager;
	auto Object=new typename tLifeCycleManager::tLifeCycleObject(cnVar::Forward<TArgs>(Args)...);
	tLifeCycleManager::ManageShared(Object);
	tLifeCycleManager::tLifeCycleActivation::Start(Object);
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

	virtual void* cnLib_FUNC CastInterface(iTypeID InterfaceID)noexcept(true) override{
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
struct TInterfaceLifeCycleTypes
{
	typedef typename cnVar::TSelect<cnVar::TClassIsInheritFrom<T,bcVirtualLifeCycle>::Value
		, cCPPLifeCycleInstance
		, cVirtualLifeCycleInstance
	>::Type tLifeCycleInstance;

	typedef typename cnVar::TSelect<cnVar::TClassIsInheritFrom<T,iObservedReference>::Value
		, impReferenceLifeCycleObject<impRefInterface<T,typename cnVar::TClassReferenceInterface<T>::Type>,tLifeCycleInstance>
		, impObservedReferenceLifeCycleObject<impObsRefInterface<T,typename cnVar::TClassReferenceInterface<T>::Type>,tLifeCycleInstance>
	>::Type tLifeCycleObject;

	typedef typename cnVar::TSelect<cnVar::TClassIsInheritFrom<T,bcVirtualLifeCycle>::Value
		, cCPPLifeCycleSharedManager<tLifeCycleObject>
		, cVirtualLifeCycleSharedManager<tLifeCycleObject>
	>::Type tLifeCycleManager;

};
//---------------------------------------------------------------------------
template<class T>
//inline typename cnVar::TSelect<0,iPtr<T>
//	, decltype(new typename TInterfaceLifeCycleTypes<T>::tLifeCycleObject)
//>::Type iCreate(void)noexcept(true)
inline iPtr<T> iCreate(void)noexcept(true)
{
	typedef typename TInterfaceLifeCycleTypes<T>::tLifeCycleManager tLifeCycleManager;
	auto Object=new typename tLifeCycleManager::tLifeCycleObject;
	tLifeCycleManager::ManageShared(Object);
	tLifeCycleManager::tLifeCycleActivation::Start(Object);
	return iPtr<T>::TakeFromManual(Object);
}
//---------------------------------------------------------------------------
template<class T,class...TArgs>
//inline typename cnVar::TSelect<0,iPtr<T>
//	, decltype(new typename TInterfaceLifeCycleTypes<T>::tLifeCycleObject(cnVar::DeclVal<TArgs>()...))
//>::Type iCreate(TArgs&&...Args)noexcept(true)
inline iPtr<T> iCreate(TArgs&&...Args)noexcept(true)
{
	typedef typename TInterfaceLifeCycleTypes<T>::tLifeCycleManager tLifeCycleManager;
	auto Object=new typename tLifeCycleManager::tLifeCycleObject(cnVar::Forward<TArgs>(Args)...);
	tLifeCycleManager::ManageShared(Object);
	tLifeCycleManager::tLifeCycleActivation::Start(Object);
	return iPtr<T>::TakeFromManual(Object);
}
//---------------------------------------------------------------------------

// Recyclable Object

//---------------------------------------------------------------------------
template<class T>
class cAutoRecyclableObject
	: public T
	, public cnVar::TSelect<cnVar::TClassIsInheritFrom<T,bcVirtualLifeCycle>::Value
		, cCPPLifeCycleRecyclableInstance
		, cVirtualLifeCycleRecyclableInstance
	>::Type
{
};
//---------------------------------------------------------------------------
struct arPointerOwnerOperator
{
	template<class T>
	static void Release(T *Pointer)noexcept(true){
		cAutoRecyclableObject<T>::template tActivation< cAutoRecyclableObject<T> >::Stop(static_cast<cAutoRecyclableObject<T>*>(Pointer));
	}
};
//---------------------------------------------------------------------------
template<class T>
using arPtr = cnVar::cPtrOwner<T,arPointerOwnerOperator>;
//---------------------------------------------------------------------------
template<class T>
inline arPtr<T> arCreateUnrecyclable(void)noexcept(true){
	typedef typename cnVar::TSelect<cnVar::TClassIsInheritFrom<T,bcVirtualLifeCycle>::Value,
		cCPPLifeCycleSharedManager< cAutoRecyclableObject<T> >,
		cVirtualLifeCycleSharedManager< cAutoRecyclableObject<T> >
	>::Type tLifeCycleUnrecyclableManager;
	auto Object=new cAutoRecyclableObject<T>;
	tLifeCycleUnrecyclableManager::ManageShared(Object);
	tLifeCycleUnrecyclableManager::tLifeCycleActivation::Start(Object);
	return arPtr<T>::TakeFromManual(Object);
}
//---------------------------------------------------------------------------
template<class T>
class arSharedObjectRecycler
{
public:
	typedef typename cnVar::TSelect<cnVar::TClassIsInheritFrom<T,bcVirtualLifeCycle>::Value,
		cCPPLifeCycleRecyclableSharedManager< cAutoRecyclableObject<T>,cRecyclableObjectAllocator< cAutoRecyclableObject<T> > >,
		cVirtualLifeCycleRecyclableSharedManager< cAutoRecyclableObject<T>,cRecyclableObjectAllocator< cAutoRecyclableObject<T> > >
	>::Type tLifeCycleSharedRecyclableManager;
	
	arSharedObjectRecycler()noexcept(true) : fManager(tLifeCycleSharedRecyclableManager::GetSharedManager()){}
	~arSharedObjectRecycler()noexcept(true){}

	arPtr<T> operator ()(void)noexcept(true){	return Query();	}
	arPtr<T> Query(void)noexcept(true){
		auto Object=fManager->Query();
		tLifeCycleSharedRecyclableManager::tLifeCycleActivation::Start(Object);
		return arPtr<T>::TakeFromManual(Object);
	}

	static arPtr<T> QueryShared(void)noexcept(true){
		auto Manager=tLifeCycleSharedRecyclableManager::GetSharedManager();
		auto Object=Manager->Query();
		tLifeCycleSharedRecyclableManager::tLifeCycleActivation::Start(Object);
		return arPtr<T>::TakeFromManual(Object);
	}

private:
	rPtr<tLifeCycleSharedRecyclableManager> fManager;
};
//---------------------------------------------------------------------------
template<class T>
inline arPtr<T> arQuerySharedObject(void)noexcept(true){
	return arSharedObjectRecycler<T>::QueryShared();
}
//---------------------------------------------------------------------------
template<class T>
class arObjectRecycler
{
public:
	typedef typename cnVar::TSelect<cnVar::TClassIsInheritFrom<T,bcVirtualLifeCycle>::Value,
		cCPPLifeCycleRecyclableManager< cAutoRecyclableObject<T>,cRecyclableObjectAllocator< cAutoRecyclableObject<T> > >,
		cVirtualLifeCycleRecyclableManager< cAutoRecyclableObject<T>,cRecyclableObjectAllocator< cAutoRecyclableObject<T> > >
	>::Type tLifeCycleRecyclableManager;

	arObjectRecycler()noexcept(true) : fManager(rCreate<tLifeCycleRecyclableManager>()){}
	~arObjectRecycler()noexcept(true){}

	arPtr<T> operator ()(void)noexcept(true){	Query();	}
	arPtr<T> Query(void)noexcept(true){
		auto Object=fManager->Query();
		tLifeCycleRecyclableManager::tLifeCycleActivation::Start(Object);
		return arPtr<T>::TakeFromManual(Object);
	}

private:
	rPtr<tLifeCycleRecyclableManager> fManager;
};
//---------------------------------------------------------------------------

// Recyclable Reference


//---------------------------------------------------------------------------
template<class T>
struct TReferenceLifeCycleRecyclableSharedManagerTypes
{
	typedef typename cnVar::TSelect<cnVar::TClassIsInheritFrom<T,bcVirtualLifeCycle>::Value
		, cCPPLifeCycleRecyclableInstance
		, cVirtualLifeCycleRecyclableInstance
	>::Type tLifeCycleInstance;

	typedef typename cnVar::TSelect<cnVar::TClassIsInheritFrom<T,iObservedReference>::Value
		, impReferenceLifeCycleObject<T,tLifeCycleInstance>
		, impObservedReferenceLifeCycleObject<T,tLifeCycleInstance>
	>::Type tLifeCycleObject;

	typedef typename cnVar::TSelect<cnVar::TClassIsInheritFrom<T,bcVirtualLifeCycle>::Value
		, cCPPLifeCycleRecyclableSharedManager<tLifeCycleObject,cRecyclableObjectAllocator<tLifeCycleObject> >
		, cVirtualLifeCycleRecyclableSharedManager<tLifeCycleObject,cRecyclableObjectAllocator<tLifeCycleObject>>
	>::Type tLifeCycleManager;

};
//---------------------------------------------------------------------------
template<class T>
class rSharedObjectRecycler
{
public:

	typedef typename TReferenceLifeCycleRecyclableSharedManagerTypes<T>::tLifeCycleManager tLifeCycleSharedRecyclableManager;
	rSharedObjectRecycler()noexcept(true) : fManager(tLifeCycleSharedRecyclableManager::GetSharedManager()){}
	~rSharedObjectRecycler()noexcept(true){}

	rPtr<T> operator ()(void)noexcept(true){	Query();	}
	rPtr<T> Query(void)noexcept(true){
		auto Object=fManager->Query();
		tLifeCycleSharedRecyclableManager::tLifeCycleActivation::Start(Object);
		return rPtr<T>::TakeFromManual(Object);
	}

	static rPtr<T> QueryShared(void)noexcept(true){
		auto Manager=tLifeCycleSharedRecyclableManager::GetSharedManager();
		auto Object=Manager->Query();
		tLifeCycleSharedRecyclableManager::tLifeCycleActivation::Start(Object);
		return rPtr<T>::TakeFromManual(Object);
	}
private:

	rPtr<tLifeCycleSharedRecyclableManager> fManager;
};
//---------------------------------------------------------------------------
template<class T>
inline rPtr<T> rQuerySharedObject(void)noexcept(true){
	return rSharedObjectRecycler<T>::QueryShared();
}
//---------------------------------------------------------------------------
template<class T>
struct TReferenceLifeCycleRecyclableManagerTypes
{
	typedef typename cnVar::TSelect<cnVar::TClassIsInheritFrom<T,bcVirtualLifeCycle>::Value
		, cCPPLifeCycleRecyclableInstance
		, cVirtualLifeCycleRecyclableInstance
	>::Type tLifeCycleInstance;

	typedef typename cnVar::TSelect<cnVar::TClassIsInheritFrom<T,iObservedReference>::Value
		, impReferenceLifeCycleObject<T,tLifeCycleInstance>
		, impObservedReferenceLifeCycleObject<T,tLifeCycleInstance>
	>::Type tLifeCycleObject;

	typedef typename cnVar::TSelect<cnVar::TClassIsInheritFrom<T,bcVirtualLifeCycle>::Value
		, cCPPLifeCycleRecyclableManager< tLifeCycleObject,cRecyclableObjectAllocator<tLifeCycleObject> >
		, cVirtualLifeCycleRecyclableManager< tLifeCycleObject,cRecyclableObjectAllocator<tLifeCycleObject> >
	>::Type tLifeCycleManager;

};
//---------------------------------------------------------------------------
template<class T>
class rObjectRecycler
{
public:
	typedef typename TReferenceLifeCycleRecyclableManagerTypes<T>::tLifeCycleManager tLifeCycleRecyclableManager;
	rObjectRecycler()noexcept(true) : fManager(rCreate<tLifeCycleRecyclableManager>()){}
	~rObjectRecycler()noexcept(true){}

	rPtr<T> operator ()(void)noexcept(true){	Query();	}
	rPtr<T> Query(void)noexcept(true){
		auto Object=fManager->Query();
		tLifeCycleRecyclableManager::tLifeCycleActivation::Start(Object);
		return rPtr<T>::TakeFromManual(Object);
	}
private:
	rPtr<tLifeCycleRecyclableManager> fManager;
};
//---------------------------------------------------------------------------
template<class T>
struct TInterfaceLifeCycleRecyclableSharedManagerTypes
{
	typedef typename cnVar::TSelect<cnVar::TClassIsInheritFrom<T,bcVirtualLifeCycle>::Value
		, cCPPLifeCycleRecyclableInstance
		, cVirtualLifeCycleRecyclableInstance
	>::Type tLifeCycleInstance;

	typedef typename cnVar::TSelect<cnVar::TClassIsInheritFrom<T,iObservedReference>::Value
		, impReferenceLifeCycleObject<impRefInterface<T,typename cnVar::TClassReferenceInterface<T>::Type>,tLifeCycleInstance>
		, impObservedReferenceLifeCycleObject<impObsRefInterface<T,typename cnVar::TClassReferenceInterface<T>::Type>,tLifeCycleInstance>
	>::Type tLifeCycleObject;

	typedef typename cnVar::TSelect<cnVar::TClassIsInheritFrom<T,bcVirtualLifeCycle>::Value
		, cCPPLifeCycleRecyclableSharedManager< tLifeCycleObject,cRecyclableObjectAllocator<tLifeCycleObject> >
		, cVirtualLifeCycleRecyclableSharedManager< tLifeCycleObject,cRecyclableObjectAllocator<tLifeCycleObject> >
	>::Type tLifeCycleManager;

};
//---------------------------------------------------------------------------
template<class T>
class iSharedObjectRecycler
{
public:
	typedef typename TInterfaceLifeCycleRecyclableSharedManagerTypes<T>::tLifeCycleManager tLifeCycleSharedRecyclableManager;
	iSharedObjectRecycler()noexcept(true)	: fManager(tLifeCycleSharedRecyclableManager::GetSharedManager()){}
	~iSharedObjectRecycler()noexcept(true){}

	iPtr<T> operator ()(void)noexcept(true){	Query();	}
	iPtr<T> Query(void)noexcept(true){
		auto Object=fManager->Query();
		tLifeCycleSharedRecyclableManager::tLifeCycleActivation::Start(Object);
		return iPtr<T>::TakeFromManual(Object);
	}
	
	static iPtr<T> QueryShared(void)noexcept(true){
		auto Manager=tLifeCycleSharedRecyclableManager::GetSharedManager();
		auto Object=Manager->Query();
		tLifeCycleSharedRecyclableManager::tLifeCycleActivation::Start(Object);
		return iPtr<T>::TakeFromManual(Object);
	}
private:

	rPtr<tLifeCycleSharedRecyclableManager> fManager;
};
//---------------------------------------------------------------------------
template<class T>
inline iPtr<T> iQuerySharedObject(void)noexcept(true){
	return iSharedObjectRecycler<T>::QueryShared();
}
//---------------------------------------------------------------------------
template<class T>
struct TInterfaceLifeCycleRecyclableManagerTypes
{
	typedef typename cnVar::TSelect<cnVar::TClassIsInheritFrom<T,bcVirtualLifeCycle>::Value
		, cCPPLifeCycleRecyclableInstance
		, cVirtualLifeCycleRecyclableInstance
	>::Type tLifeCycleInstance;

	typedef typename cnVar::TSelect<cnVar::TClassIsInheritFrom<T,iObservedReference>::Value
		, impReferenceLifeCycleObject<impRefInterface<T,typename cnVar::TClassReferenceInterface<T>::Type>,tLifeCycleInstance>
		, impObservedReferenceLifeCycleObject<impObsRefInterface<T,typename cnVar::TClassReferenceInterface<T>::Type>,tLifeCycleInstance>
	>::Type tLifeCycleObject;

	typedef typename cnVar::TSelect<cnVar::TClassIsInheritFrom<T,bcVirtualLifeCycle>::Value
		, cCPPLifeCycleRecyclableManager< tLifeCycleObject,cRecyclableObjectAllocator<tLifeCycleObject> >
		, cVirtualLifeCycleRecyclableManager< tLifeCycleObject,cRecyclableObjectAllocator<tLifeCycleObject> >
	>::Type tLifeCycleManager;

};
//---------------------------------------------------------------------------
template<class T>
class iObjectRecycler
{
public:
	typedef typename TInterfaceLifeCycleRecyclableManagerTypes<T>::tLifeCycleManager tLifeCycleRecyclableManager;

	iObjectRecycler()noexcept(true) : fManager(rCreate<tLifeCycleRecyclableManager>()){}
	~iObjectRecycler()noexcept(true){}

	iPtr<T> operator ()(void)noexcept(true){	Query();	}
	iPtr<T> Query(void)noexcept(true){
		auto Object=fManager->Query();
		tLifeCycleRecyclableManager::tLifeCycleActivation::Start(Object);
		return iPtr<T>::TakeFromManual(Object);
	}
private:
	rPtr<tLifeCycleRecyclableManager> fManager;
};
//---------------------------------------------------------------------------

// Class Reference

//---------------------------------------------------------------------------
class cClassReferenceWithWeakSet
{
public:
	cClassReferenceWithWeakSet()noexcept(true);
	~cClassReferenceWithWeakSet()noexcept(true);

	class bcNotifyToken
	{
	public:
		virtual bool cnLib_FUNC InvalidationNotify(cClassReferenceWithWeakSet *Token)noexcept(true)=0;

		bcNotifyToken *Parent;
		bcNotifyToken *Child[2];
		ufInt8 Branch;
		ufInt8 Color;
	};

	void RefReset(void)noexcept(true);
	void RefInvalidate(void)noexcept(true);
	
	void RefInc(void)noexcept(true);
	bool RefDec(void)noexcept(true);

	void WeakRegister(bcNotifyToken *NotifyToken)noexcept(true);
	void WeakUnregister(bcNotifyToken *NotifyToken)noexcept(true);
	bool WeakToStrong(void)noexcept(true);
protected:
	cAtomicVar<uIntn> fRefCount;
	cAtomicVar<uIntn> fRegisterCount;
	cSpinLock fRefSetLock;
	cLinkItemSet<bcNotifyToken,cnDataStruct::cItemAddressOrderOperator<bcNotifyToken> > fRefSet;

};
//---------------------------------------------------------------------------
struct cAutoClassWeakRefTokenOperator;
//---------------------------------------------------------------------------
struct cAutoClassPointerReferenceOperator;
template<class T>
class aCls : public T, protected cClassReferenceWithWeakSet, public cRTLAllocator
{
	friend cAutoClassPointerReferenceOperator;

	friend cAutoClassWeakRefTokenOperator;


public:
	typedef T tClass;
	using T::T;
	using T::operator =;

	template<class TLifeCycleObject>
	struct tActivation
	{
		static void Start(TLifeCycleObject *Object)noexcept(true){
			Object->aCls::RefReset();
			cnVar::TSelect<cnVar::TClassIsInheritFrom<T,bcVirtualLifeCycle>::Value
				, typename TSelectActivation< T,cCPPLifeCycleInstance::tActivation<aCls> >::Type
				, typename TSelectActivation< T,bcVirtualLifeCycle::cActivation<aCls> >::Type
			>::Type::Start(Object);
		}
		static void Stop(TLifeCycleObject *Object)noexcept(true){
			Object->aCls::RefInvalidate();
			cnVar::TSelect<cnVar::TClassIsInheritFrom<T,bcVirtualLifeCycle>::Value
				, typename TSelectActivation< T,cCPPLifeCycleInstance::tActivation<aCls> >::Type
				, typename TSelectActivation< T,bcVirtualLifeCycle::cActivation<aCls> >::Type
			>::Type::Stop(Object);
		}
	};

	typedef typename cnVar::TSelect<cnVar::TClassIsInheritFrom<T,bcVirtualLifeCycle>::Value
		, cCPPLifeCycleSharedManager<aCls>
		, cVirtualLifeCycleSharedManager<aCls>
	>::Type tLifeCycleManager;

	friend bcVirtualLifeCycle::cActivation<aCls>;
	friend tLifeCycleManager;

protected:

	void RefClassAcquireReference(void)noexcept(true){	RefInc();	}
	void RefClassReleaseReference(void)noexcept(true){	if(RefDec()){	tActivation<aCls>::Stop(this);	}	}

};
//---------------------------------------------------------------------------
template<class T>
struct TAutoClassLifeCycleTypes
{
	typedef aCls<T> tLifeCycleObject;


};
//---------------------------------------------------------------------------
struct cAutoClassPointerReferenceOperator
{
	template<class T>	static void Acquire(aCls<T> *Pointer)noexcept(true){	Pointer->RefClassAcquireReference();	}
	template<class T>	static void Release(aCls<T> *Pointer)noexcept(true){	Pointer->RefClassReleaseReference();	}
	template<class T>	static void Acquire(const aCls<T> *Pointer)noexcept(true){	const_cast<aCls<T>*>(Pointer)->RefClassAcquireReference();	}
	template<class T>	static void Release(const aCls<T> *Pointer)noexcept(true){	const_cast<aCls<T>*>(Pointer)->RefClassReleaseReference();	}
};
//---------------------------------------------------------------------------
template<class T>
using aClsRef = cnVar::cPtrReference<aCls<T>,cAutoClassPointerReferenceOperator>;
template<class T>
using aClsConstRef = cnVar::cPtrReference<const aCls<T>,cAutoClassPointerReferenceOperator>;
template<class T>
using aClsAtomicRef = cnVar::cAtomicPtrReference<aCls<T>,cAutoClassPointerReferenceOperator>;

//---------------------------------------------------------------------------
struct cAutoRecyclableClassPointerReferenceOperator;
template<class T>	struct TAutoRecyclableClassCPPLifeCycleManagerSelect;
template<class T>	struct TAutoRecyclableClassVirtualLifeCycleManagerSelect;
//---------------------------------------------------------------------------
template<class T>
class arCls : public T
	, cnVar::TSelect<cnVar::TClassIsInheritFrom<T,bcVirtualLifeCycle>::Value,
		cCPPLifeCycleRecyclableInstance,cVirtualLifeCycleRecyclableInstance
	>::Type
	, protected cClassReferenceWithWeakSet, protected cRTLAllocator
{
	friend cAutoRecyclableClassPointerReferenceOperator;
	friend cAutoClassWeakRefTokenOperator;

	typedef typename cnVar::TSelect<cnVar::TClassIsInheritFrom<T,bcVirtualLifeCycle>::Value,
		cCPPLifeCycleRecyclableInstance,cVirtualLifeCycleRecyclableInstance
	>::Type tLifeCycleInstance;
public:
	typedef T tClass;
	using T::T;
	using T::operator =;

	template<class TLifeCycleObject>
	struct tActivation
	{
		static void Start(TLifeCycleObject *Object)noexcept(true){
			Object->arCls::RefReset();
			tLifeCycleInstance::template tActivation<arCls>::Start(Object);
		}
		static void Stop(TLifeCycleObject *Object)noexcept(true){
			Object->arCls::RefInvalidate();
			tLifeCycleInstance::template tActivation<arCls>::Stop(Object);
		}
	};

	typedef typename cnVar::TSelect<cnVar::TClassIsInheritFrom<T,bcVirtualLifeCycle>::Value
		, bcCPPLifeCycleRecyclableManager< arCls,cRecyclableObjectAllocator<arCls> >
		, bcVirtualLifeCycleRecyclableManager< arCls,cRecyclableObjectAllocator<arCls> >
	>::Type tBaseLifeCycleManager;

	typedef typename cnVar::TSelect<cnVar::TClassIsInheritFrom<T,bcVirtualLifeCycle>::Value
		, cCPPLifeCycleRecyclableSharedManager<arCls,cRecyclableObjectAllocator<arCls> >
		, cVirtualLifeCycleRecyclableSharedManager<arCls,cRecyclableObjectAllocator<arCls> >
	>::Type tLifeCycleSharedManager;

	typedef typename cnVar::TSelect<cnVar::TClassIsInheritFrom<T,bcVirtualLifeCycle>::Value
		, cCPPLifeCycleRecyclableManager< arCls,cRecyclableObjectAllocator<arCls> >
		, cVirtualLifeCycleRecyclableManager< arCls,cRecyclableObjectAllocator<arCls> >
	>::Type tLifeCycleManager;

	friend tLifeCycleInstance;
	friend bcVirtualLifeCycle::cActivation<arCls>;
	friend tBaseLifeCycleManager;
	friend cRecyclableObjectAllocator<arCls>;

protected:

	void RefClassAcquireReference(void)noexcept(true){	RefInc();	}
	void RefClassReleaseReference(void)noexcept(true){	if(RefDec()){	tActivation<arCls>::Stop(this);	}	}

};
//---------------------------------------------------------------------------
struct cAutoRecyclableClassPointerReferenceOperator
{
	template<class T>	static void Acquire(arCls<T> *Pointer)noexcept(true){		Pointer->RefClassAcquireReference();	}
	template<class T>	static void Release(arCls<T> *Pointer)noexcept(true){		Pointer->RefClassReleaseReference();	}
	template<class T>	static void Acquire(const arCls<T> *Pointer)noexcept(true){	const_cast<arCls<T>*>(Pointer)->RefClassAcquireReference();	}
	template<class T>	static void Release(const arCls<T> *Pointer)noexcept(true){	const_cast<arCls<T>*>(Pointer)->RefClassReleaseReference();	}
};
//---------------------------------------------------------------------------
template<class T>
using arClsRef = cnVar::cPtrReference<arCls<T>,cAutoRecyclableClassPointerReferenceOperator>;
template<class T>
using arClsConstRef = cnVar::cPtrReference<const arCls<T>,cAutoRecyclableClassPointerReferenceOperator>;
template<class T>
using arClsAtomicRef = cnVar::cAtomicPtrReference<arCls<T>,cAutoRecyclableClassPointerReferenceOperator>;

//---------------------------------------------------------------------------
struct cAutoClassWeakRefTokenOperator
{
	typedef cClassReferenceWithWeakSet tToken;

	template<class TAutoClass>
	static tToken* Register(TAutoClass *Pointer)noexcept(true){
		if(Pointer==nullptr)
			return nullptr;
		//Pointer->WeakRegister(NotifyToken);
		//return Pointer;
		return nullptr;
	}
	static void Unregister(tToken *Token)noexcept(true){
		//return Token->WeakUnregister(0);
	}

	template<class TAutoClass>
	struct tReference
		: cnVar::TTypeDef< cnVar::cPtrReference<typename TAutoClass::tClass,cAutoClassPointerReferenceOperator> >{};

	template<class TAutoClass>
	static cnVar::cPtrReference<typename TAutoClass::tClass,cAutoClassPointerReferenceOperator> Reference(tToken *Token)noexcept(true){
		return Token->WeakToStrong()?cnVar::cPtrReference<typename TAutoClass::tClass,cAutoClassPointerReferenceOperator>::TakeFromManual(Token):nullptr;
	}

};
//---------------------------------------------------------------------------
template<class T>
using aClsWeakRef=cnVar::cPtrWeakReference<aCls<T>,cAutoClassWeakRefTokenOperator>;
template<class T>
using arClsWeakRef=cnVar::cPtrWeakReference<arCls<T>,cAutoClassWeakRefTokenOperator>;
//---------------------------------------------------------------------------
template<class T>
inline aCls<T>* aClsFromPtr(T *Src)noexcept(true)
{
	return static_cast<aCls<T>*>(Src);
}
//---------------------------------------------------------------------------
template<class T>
inline aClsRef<T> aClsTake(aCls<T> *Src,uInt32 Tag)noexcept(true)
{
	cnRTL_DEBUG_LOG_REFERENCE_DEC(Src,Tag);
	return aClsRef<T>::TakeFromManual(Src);
}
//---------------------------------------------------------------------------
template<class T>
inline aCls<T>* aClsExtract(aClsRef<T> &Src,uInt32 Tag)noexcept(true)
{
	cnRTL_DEBUG_LOG_REFERENCE_INC(static_cast<aCls<T>*>(Src),Tag);
	return Src.ExtractToManual();
}
//---------------------------------------------------------------------------
template<class T>
//inline typename cnVar::TSelect<0,aClsRef<T>
//	, decltype(new aCls<T>)
//>::Type aClsCreate(void)noexcept(true)
inline aClsRef<T> aClsCreate(void)noexcept(true)
{
	auto NewObject=new aCls<T>;
	aCls<T>::tLifeCycleManager::ManageShared(NewObject);
	aCls<T>::tLifeCycleManager::tLifeCycleActivation::Start(NewObject);
	return cnVar::cPtrReference<aCls<T>,cAutoClassPointerReferenceOperator>::TakeFromManual(NewObject);
}

template<class T,class...TArgs>
//inline typename cnVar::TSelect<0,aClsRef<T>
//	, decltype(new aCls<T>(cnVar::DeclVal<TArgs>()...))
//>::Type aClsCreate(TArgs&&...Args)noexcept(true)
inline aClsRef<T> aClsCreate(TArgs&&...Args)noexcept(true)
{
	auto NewObject=new aCls<T>(cnVar::Forward<TArgs>(Args)...);
	aCls<T>::tLifeCycleManager::ManageShared(NewObject);
	aCls<T>::tLifeCycleManager::tLifeCycleActivation::Start(NewObject);
	return cnVar::cPtrReference<aCls<T>,cAutoClassPointerReferenceOperator>::TakeFromManual(NewObject);
}
//---------------------------------------------------------------------------
template<class T>
inline arCls<T>* arClsFromPtr(T *Src)noexcept(true)
{
	return static_cast<arCls<T>*>(Src);
}
//---------------------------------------------------------------------------
template<class T>
inline arClsRef<T> arClsTake(arCls<T> *Src,uInt32 Tag)noexcept(true)
{
	cnRTL_DEBUG_LOG_REFERENCE_DEC(Src,Tag);
	return arClsRef<T>::TakeFromManual(Src);
}
//---------------------------------------------------------------------------
template<class T>
inline arCls<T>* arClsExtract(arClsRef<T> &Src,uInt32 Tag)noexcept(true)
{
	cnRTL_DEBUG_LOG_REFERENCE_INC(static_cast<arCls<T>*>(Src),Tag);
	return Src.ExtractToManual();
}
//---------------------------------------------------------------------------
template<class T>
class arClsSharedObjectRecycler
{
public:
	typedef typename arCls<T>::tLifeCycleSharedManager tLifeCycleManager;
	arClsSharedObjectRecycler()noexcept(true) : fManager(tLifeCycleManager::GetSharedManager()){}
	~arClsSharedObjectRecycler()noexcept(true){}

	arClsRef<T> operator ()(void)noexcept(true){	return Query();	}
	arClsRef<T> Query(void)noexcept(true){
		auto Object=fManager->Query();
		tLifeCycleManager::tLifeCycleActivation::Start(Object);
		return arClsRef<T>::TakeFromManual(Object);
	}

	static arClsRef<T> QueryShared(void)noexcept(true){
		auto Manager=tLifeCycleManager::GetSharedManager();
		auto Object=Manager->Query();
		tLifeCycleManager::tLifeCycleActivation::Start(Object);
		return arClsRef<T>::TakeFromManual(Object);
	}
private:

	rPtr<tLifeCycleManager> fManager;
};
//---------------------------------------------------------------------------
template<class T>
inline arClsRef<T> arClsQuerySharedObject(void)noexcept(true){
	return arClsSharedObjectRecycler<T>::QueryShared();
}
//---------------------------------------------------------------------------
template<class T>
class arClsObjectRecycler
{
public:
	typedef typename arCls<T>::tLifeCycleManager tLifeCycleManager;

	arClsObjectRecycler()noexcept(true) : fManager(rCreate<tLifeCycleManager>()){}
	~arClsObjectRecycler()noexcept(true){}

	arClsRef<T> operator ()(void)noexcept(true){	Query();	}
	arClsRef<T> Query(void)noexcept(true){
		auto Object=fManager->Query();
		tLifeCycleManager::tLifeCycleActivation::Start(Object);
		return arClsRef<T>::TakeFromManual(Object);
	}
private:
	rPtr<tLifeCycleManager> fManager;
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
