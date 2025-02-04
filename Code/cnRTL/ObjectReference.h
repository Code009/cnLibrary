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
class bcWeakReference
{
public:

	void ObjectResetReference(void)noexcept(true);
	void ObjectIncreaseReference(void)noexcept(true);
	void ObjectDecreaseReference(void)noexcept(true);

	iReferenceObserver* CreateReferenceObserver(iReference *Reference,iProcedure *Procedure)noexcept(true);

protected:
	virtual void cnLib_FUNC ObjectDeleted(void)noexcept(true)=0;

private:
	cAtomicVar<uIntn> fRefCount;
	cAtomicVar<uIntn> fWeakCount;
	cAsyncNotifySet<void> fNotifySet;

	class cWeakObserver : public bcReferenceObserver<cWeakObserver>, public cRTLAllocator
	{
	public:
		cWeakObserver(bcWeakReference *Owner,iReference *Reference,iProcedure *Procedure)noexcept(true);

		bool ObserverMakeStrongReference(void)noexcept(true);
		void ObserverInvalidate(bool Notify)noexcept(true);
		void ObserverDelete(void)noexcept(true);

	protected:
		bcWeakReference *fOwner;
		rPtr<iReference> fProcReference;
		iProcedure *fProcedure;
	};
	void WeakUnregister(cWeakObserver *Observer)noexcept(true);
	bool WeakToStrong(void)noexcept(true);
};
//---------------------------------------------------------------------------
class bcRegisteredReference
{
public:
	bcRegisteredReference(bool InitialReference)noexcept(true);

	void IncRef(void)noexcept(true);
	void DecRef(void)noexcept(true);
	bool MakeStrongReference(void)noexcept(true);

	rPtr<iLibraryReference> CreateReference(iLibraryReferrer *Referrer)noexcept(true);
	bool UpdateReferenceSet(void)noexcept(true);
	void ReportReferenceSet(cStringBuffer<uChar16> &Buffer)noexcept(true);
protected:

	virtual void ReferenceUpdate(void)noexcept(true)=0;
	virtual void ReferenceShutdown(void)noexcept(true)=0;

private:
	cAtomicVar<uIntn> fRefCount;

	class cReference : public iLibraryReference, public cRTLAllocator
	{
	public:
		cReference *Next;
		cReference *Prev;
		bool Released;
		bool DescriptionUpdated;

		cReference(bcRegisteredReference *Owner,iLibraryReferrer *Referrer)noexcept(true);
		~cReference()noexcept(true);

		void WeakIncRef(void)noexcept(true);
		void WeakDecRef(void)noexcept(true);
		bool MakeStrongReference(void)noexcept(true);
		bcRegisteredReference *GetOwner(void)const noexcept(true);
		iLibraryReferrer *GetReferrer(void)const noexcept(true);

		virtual	void cnLib_FUNC IncreaseReference(void)noexcept(true)override;
		virtual	void cnLib_FUNC DecreaseReference(void)noexcept(true)override;

		virtual	iReferenceObserver* cnLib_FUNC CreateReferenceObserver(iReference *NotifyReference,iProcedure *NotifyProcedure)noexcept(true)override;

		virtual void cnLib_FUNC UpdateDescription(void)noexcept(true)override;

	private:
		bcRegisteredReference *fOwner;
		iLibraryReferrer *fReferrer;
		cAtomicVar<uIntn> fWeakRefCount;

	};
	cLinkItemList<cReference> fReferenceList;


	class cObserver : public bcReferenceObserver<cObserver>, public cRTLAllocator
	{
	public:
		cObserver(cReference *Reference,iReference *NotifyReference,iProcedure *NotifyProcedure)noexcept(true);
		~cObserver()noexcept(true);

		bool ObserverMakeStrongReference(void)noexcept(true);
		void ObserverInvalidate(bool Notify)noexcept(true);
		void ObserverDelete(void)noexcept(true);

	private:
		cReference *fReference;
		rPtr<iReference> fNotifyReference;
		iProcedure *fNotifyProcedure;
	};

	cAsyncNotifySet<void> fShutdownNotifySet;
};
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
	friend struct bcVirtualLifeCycle::cLifeCycleActivation;

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
template<class T>
using TReferenceObjectLifeCycleActivation=typename cnVar::TSelect<cnVar::TClassIsInheritFrom<T,bcVirtualLifeCycle>::Value
	, typename cnVar::TSelect<cnVar::TClassIsInheritFrom<T,bcDisposable>::Value
		, cCPPLifeCycleActivation
		, cDisposableLifeCycleActivation
	>::Type
	, cVirtualLifeCycleActivation
>::Type;
template<class T>
using TReferenceObjectLifeCycleSharedManager=typename cnVar::TSelect<cnVar::TClassIsInheritFrom<T,bcDisposable>::Value
	, cCPPLifeCycleSharedManager<T>
	, cDisposableLifeCycleSharedManager<T>
>::Type;
//---------------------------------------------------------------------------

// Automatic Object Pointer

//---------------------------------------------------------------------------
struct aPointerOwnerOperator
{
	template<class T>
	static void Release(T *Pointer)noexcept(true){
		TReferenceObjectLifeCycleActivation<T>::Stop(Pointer);
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
inline aPtr<T> aCreate(void)noexcept(true)
{
	auto Pointer=new T;
	TReferenceObjectLifeCycleSharedManager<T>::ManageShared(Pointer);
	TReferenceObjectLifeCycleActivation<T>::Start(Pointer);
	return aPtr<T>::TakeFromManual(Pointer);
}
template<class T,class...TArgs>
inline aPtr<T> aCreate(TArgs&&...Args)noexcept(true)
{
	auto Pointer=new T(static_cast<TArgs&&>(Args)...);
	TReferenceObjectLifeCycleSharedManager<T>::ManageShared(Pointer);
	TReferenceObjectLifeCycleActivation<T>::Start(Pointer);
	return aPtr<T>::TakeFromManual(Pointer);
}
//---------------------------------------------------------------------------

// Automatic Reference Pointer

//---------------------------------------------------------------------------
struct cReferenceImplementationLifeCycleActivation
{
	template<class TLifeCycleObject>
	static void Start(TLifeCycleObject *Object)noexcept(true){
		Object->LifeCycleSetup();
		TReferenceObjectLifeCycleActivation<TLifeCycleObject>::Start(Object);
	}
	template<class TLifeCycleObject>
	static void Stop(TLifeCycleObject *Object)noexcept(true){
		Object->LifeCycleClear();
		TReferenceObjectLifeCycleActivation<TLifeCycleObject>::Stop(Object);
	}
};
//---------------------------------------------------------------------------
template<class T>
class impReferenceLifeCycleObject : public T, public cRTLAllocator
{
public:
	using T::T;
	~impReferenceLifeCycleObject()=default;

	virtual void cnLib_FUNC IncreaseReference(void)noexcept(true) override{
		cnLib_ASSERT(static_cast<uIntn>(fRefCount+1)>1);	// not -1 or 0
		fRefCount.Free++;
	}
	virtual void cnLib_FUNC DecreaseReference(void)noexcept(true) override{
		cnLib_ASSERT(fRefCount!=0);
		if(fRefCount.Free--==1){
			cReferenceImplementationLifeCycleActivation::Stop(this);
		}
	}
private:
	cAtomicVar<uIntn> fRefCount;

	friend cReferenceImplementationLifeCycleActivation;
	friend bcVirtualLifeCycle::cLifeCycleActivation;

	void LifeCycleSetup(void)noexcept(true){
		fRefCount=1;
	}
	void LifeCycleClear(void)noexcept(true){
	}
};
//---------------------------------------------------------------------------
template<class T>
class impObservedReferenceLifeCycleObject : public T, protected bcWeakReference, public cRTLAllocator
{
public:
	using T::T;
	~impObservedReferenceLifeCycleObject()=default;

	virtual void cnLib_FUNC IncreaseReference(void)noexcept(true)override{
		bcWeakReference::ObjectIncreaseReference();
	}
	virtual void cnLib_FUNC DecreaseReference(void)noexcept(true)override{
		bcWeakReference::ObjectDecreaseReference();
	}

	virtual iReferenceObserver* cnLib_FUNC CreateReferenceObserver(iReference *Reference,iProcedure *Procedure)noexcept(true)override{
		return bcWeakReference::CreateReferenceObserver(Reference,Procedure);
	}

protected:
	virtual void cnLib_FUNC ObjectDeleted(void)noexcept(true)override{
		cReferenceImplementationLifeCycleActivation::Stop(this);
	}

private:
	friend cReferenceImplementationLifeCycleActivation;
	friend bcVirtualLifeCycle::cLifeCycleActivation;

	void LifeCycleSetup(void)noexcept(true){
		bcWeakReference::ObjectResetReference();
	}
	void LifeCycleClear(void)noexcept(true){
	}

};
//---------------------------------------------------------------------------
}	// namespace cnRTL
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
namespace cnLib_THelper{
namespace RTL_TH{
namespace LifeCycle{

template<class T>
struct TReferenceLiftCycleTypes
{

	typedef typename cnVar::TSelect<cnVar::TClassIsInheritFrom<T,iObservedReference>::Value
		, cnRTL::impReferenceLifeCycleObject<T>
		, cnRTL::impObservedReferenceLifeCycleObject<T>
	>::Type tLifeCycleObject;


	typedef cnRTL::TReferenceObjectLifeCycleSharedManager<tLifeCycleObject> tLifeCycleSharedManager;
};

}	// namespace LifeCycle
}	// namespace RTL_TH
}	// namespace cnLib_THelper
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnRTL{
//---------------------------------------------------------------------------
template<class T>
inline rPtr<T> rCreate(void)noexcept(true)
{
	auto Object=new typename cnLib_THelper::RTL_TH::LifeCycle::TReferenceLiftCycleTypes<T>::tLifeCycleObject;
	cnLib_THelper::RTL_TH::LifeCycle::TReferenceLiftCycleTypes<T>::tLifeCycleSharedManager::ManageShared(Object);
	cReferenceImplementationLifeCycleActivation::Start(Object);
	return rPtr<T>::TakeFromManual(Object);
}
//---------------------------------------------------------------------------
template<class T,class...TArgs>
inline rPtr<T> rCreate(TArgs&&...Args)noexcept(true)
{
	auto Object=new typename cnLib_THelper::RTL_TH::LifeCycle::TReferenceLiftCycleTypes<T>::tLifeCycleObject(cnVar::Forward<TArgs>(Args)...);
	cnLib_THelper::RTL_TH::LifeCycle::TReferenceLiftCycleTypes<T>::tLifeCycleSharedManager::ManageShared(Object);
	cReferenceImplementationLifeCycleActivation::Start(Object);
	return rPtr<T>::TakeFromManual(Object);
}
//---------------------------------------------------------------------------
}	// namespace cnRTL
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
namespace cnLib_THelper{
namespace RTL_TH{
namespace LifeCycle{

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

	typedef typename cnVar::TSelect<cnVar::TClassIsInheritFrom<T,iObservedReference>::Value
		, cnRTL::impReferenceLifeCycleObject< impRefInterface<T,typename cnVar::TClassReferenceInterface<T>::Type> >
		, cnRTL::impObservedReferenceLifeCycleObject< impObsRefInterface<T,typename cnVar::TClassReferenceInterface<T>::Type> >
	>::Type tLifeCycleObject;


	typedef cnRTL::TReferenceObjectLifeCycleSharedManager<tLifeCycleObject> tLifeCycleSharedManager;
};

}	// namespace LifeCycle
}	// namespace RTL_TH
}	// namespace cnLib_THelper
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnRTL{
//---------------------------------------------------------------------------
template<class T>
inline iPtr<T> iCreate(void)noexcept(true)
{
	auto Object=new typename cnLib_THelper::RTL_TH::LifeCycle::TInterfaceLifeCycleTypes<T>::tLifeCycleObject;
	cnLib_THelper::RTL_TH::LifeCycle::TInterfaceLifeCycleTypes<T>::tLifeCycleSharedManager::ManageShared(Object);
	cReferenceImplementationLifeCycleActivation::Start(Object);
	return iPtr<T>::TakeFromManual(Object);
}
//---------------------------------------------------------------------------
template<class T,class...TArgs>
inline iPtr<T> iCreate(TArgs&&...Args)noexcept(true)
{
	auto Object=new typename cnLib_THelper::RTL_TH::LifeCycle::TInterfaceLifeCycleTypes<T>::tLifeCycleObject(cnVar::Forward<TArgs>(Args)...);
	cnLib_THelper::RTL_TH::LifeCycle::TInterfaceLifeCycleTypes<T>::tLifeCycleSharedManager::ManageShared(Object);
	cReferenceImplementationLifeCycleActivation::Start(Object);
	return iPtr<T>::TakeFromManual(Object);
}
//---------------------------------------------------------------------------

// Recyclable Object

//---------------------------------------------------------------------------
class bcRecyclableDisposable
	: public bcDisposable
	, public bcRecyclable
{
};
template<class T>
using TSelectRecyclableBase=typename cnVar::TSelect<cnVar::TClassIsInheritFrom<T,bcDisposable>::Value
	, bcRecyclableDisposable
	, bcRecyclable
>;
//---------------------------------------------------------------------------
template<class T>
class cAutoRecyclableObject
	: public T
	, public TSelectRecyclableBase<T>::Type
{
};
//---------------------------------------------------------------------------
struct arPointerOwnerOperator
{
	template<class T>
	static void Release(T *Pointer)noexcept(true){
		TReferenceObjectLifeCycleActivation< cAutoRecyclableObject<T> >::Stop(static_cast<cAutoRecyclableObject<T>*>(Pointer));
	}
};
//---------------------------------------------------------------------------
template<class T>
using arPtr = cnVar::cPtrOwner<T,arPointerOwnerOperator>;
//---------------------------------------------------------------------------
template<class T>
inline arPtr<T> arCreateUnrecyclable(void)noexcept(true){
	
	typedef TReferenceObjectLifeCycleSharedManager< cAutoRecyclableObject<T> > tLifeCycleUnrecyclableManager;
	auto Object=new cAutoRecyclableObject<T>;
	tLifeCycleUnrecyclableManager::ManageShared(Object);
	TReferenceObjectLifeCycleActivation< cAutoRecyclableObject<T> >::Start(Object);
	return arPtr<T>::TakeFromManual(Object);
}
//---------------------------------------------------------------------------
template<class T>
class arSharedObjectRecycler
{
public:
	typedef cRecyclableLifeCycleSharedManager< cAutoRecyclableObject<T>,cRecyclableObjectAllocator< cAutoRecyclableObject<T> > > tLifeCycleRecyclableSharedManager;
	
	arSharedObjectRecycler()noexcept(true) : fManager(tLifeCycleRecyclableSharedManager::GetSharedManager()){}
	~arSharedObjectRecycler()noexcept(true){}

	arPtr<T> operator ()(void)noexcept(true){	return Query();	}
	arPtr<T> Query(void)noexcept(true){
		auto Object=fManager->Query();
		TReferenceObjectLifeCycleActivation< cAutoRecyclableObject<T> >::Start(Object);
		return arPtr<T>::TakeFromManual(Object);
	}

	static arPtr<T> QueryShared(void)noexcept(true){
		auto Manager=tLifeCycleRecyclableSharedManager::GetSharedManager();
		auto Object=Manager->Query();
		TReferenceObjectLifeCycleActivation< cAutoRecyclableObject<T> >::Start(Object);
		return arPtr<T>::TakeFromManual(Object);
	}

private:
	rPtr<tLifeCycleRecyclableSharedManager> fManager;
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
	typedef cRecyclableLifeCycleManager< cAutoRecyclableObject<T>,cRecyclableObjectAllocator< cAutoRecyclableObject<T> > > tLifeCycleRecyclableManager;

	arObjectRecycler()noexcept(true) : fManager(rCreate<tLifeCycleRecyclableManager>()){}
	~arObjectRecycler()noexcept(true){}

	arPtr<T> operator ()(void)noexcept(true){	Query();	}
	arPtr<T> Query(void)noexcept(true){
		auto Object=fManager->Query();
		TReferenceObjectLifeCycleActivation< cAutoRecyclableObject<T> >::Start(Object);
		return arPtr<T>::TakeFromManual(Object);
	}

private:
	rPtr<tLifeCycleRecyclableManager> fManager;
};
//---------------------------------------------------------------------------

// Recyclable Reference

//---------------------------------------------------------------------------
template<class T>
class impReferenceRecyclableLifeCycleObject : public T, public TSelectRecyclableBase<T>::Type, public cRTLAllocator
{
public:
	using T::T;
	~impReferenceRecyclableLifeCycleObject()=default;

	virtual void cnLib_FUNC IncreaseReference(void)noexcept(true) override{
		cnLib_ASSERT(static_cast<uIntn>(fRefCount+1)>1);	// not -1 or 0
		fRefCount.Free++;
	}
	virtual void cnLib_FUNC DecreaseReference(void)noexcept(true) override{
		cnLib_ASSERT(fRefCount!=0);
		if(fRefCount.Free--==1){
			cReferenceImplementationLifeCycleActivation::Stop(this);
		}
	}
private:
	cAtomicVar<uIntn> fRefCount;

	friend cReferenceImplementationLifeCycleActivation;
	friend bcVirtualLifeCycle::cLifeCycleActivation;

	void LifeCycleSetup(void)noexcept(true){
		fRefCount=1;
	}
	void LifeCycleClear(void)noexcept(true){
	}
};
//---------------------------------------------------------------------------
template<class T>
class impObservedReferenceRecyclableLifeCycleObject : public T, protected bcWeakReference, public TSelectRecyclableBase<T>::Type, public cRTLAllocator
{
public:
	using T::T;
	~impObservedReferenceRecyclableLifeCycleObject()=default;

	virtual void cnLib_FUNC IncreaseReference(void)noexcept(true)override{
		bcWeakReference::ObjectIncreaseReference();
	}
	virtual void cnLib_FUNC DecreaseReference(void)noexcept(true)override{
		bcWeakReference::ObjectDecreaseReference();
	}

	virtual iReferenceObserver* cnLib_FUNC CreateReferenceObserver(iReference *Reference,iProcedure *Procedure)noexcept(true)override{
		return bcWeakReference::CreateReferenceObserver(Reference,Procedure);
	}

protected:
	virtual void cnLib_FUNC ObjectDeleted(void)noexcept(true)override{
		cReferenceImplementationLifeCycleActivation::Stop(this);
	}

private:
	friend cReferenceImplementationLifeCycleActivation;

	void LifeCycleSetup(void)noexcept(true){
		bcWeakReference::ObjectResetReference();
	}
	void LifeCycleClear(void)noexcept(true){
	}

};
//---------------------------------------------------------------------------
template<class T>
struct TReferenceLifeCycleRecyclableTypes
{
	typedef typename cnVar::TSelect<cnVar::TClassIsInheritFrom<T,iObservedReference>::Value
		, impReferenceRecyclableLifeCycleObject<T>
		, impObservedReferenceRecyclableLifeCycleObject<T>
	>::Type tLifeCycleObject;

};
//---------------------------------------------------------------------------
template<class T>
class rSharedObjectRecycler
{
public:
	typedef cRecyclableLifeCycleSharedManager<typename TReferenceLifeCycleRecyclableTypes<T>::tLifeCycleObject> tLifeCycleRecyclableSharedManager;
	rSharedObjectRecycler()noexcept(true) : fManager(tLifeCycleRecyclableSharedManager::GetSharedManager()){}
	~rSharedObjectRecycler()noexcept(true){}

	rPtr<T> operator ()(void)noexcept(true){	Query();	}
	rPtr<T> Query(void)noexcept(true){
		auto Object=fManager->Query();
		cReferenceImplementationLifeCycleActivation::Start(Object);
		return rPtr<T>::TakeFromManual(Object);
	}

	static rPtr<T> QueryShared(void)noexcept(true){
		auto Manager=tLifeCycleRecyclableSharedManager::GetSharedManager();
		auto Object=Manager->Query();
		cReferenceImplementationLifeCycleActivation::Start(Object);
		return rPtr<T>::TakeFromManual(Object);
	}
private:

	rPtr<tLifeCycleRecyclableSharedManager> fManager;
};
//---------------------------------------------------------------------------
template<class T>
inline rPtr<T> rQuerySharedObject(void)noexcept(true){
	return rSharedObjectRecycler<T>::QueryShared();
}
//---------------------------------------------------------------------------
template<class T>
class rObjectRecycler
{
public:
	typedef cRecyclableLifeCycleManager<typename TReferenceLifeCycleRecyclableTypes<T>::tLifeCycleObject> tLifeCycleRecyclableManager;
	rObjectRecycler()noexcept(true) : fManager(rCreate<tLifeCycleRecyclableManager>()){}
	~rObjectRecycler()noexcept(true){}

	rPtr<T> operator ()(void)noexcept(true){	Query();	}
	rPtr<T> Query(void)noexcept(true){
		auto Object=fManager->Query();
		cReferenceImplementationLifeCycleActivation::Start(Object);
		return rPtr<T>::TakeFromManual(Object);
	}
private:
	rPtr<tLifeCycleRecyclableManager> fManager;
};
//---------------------------------------------------------------------------
template<class T>
struct TInterfaceLifeCycleRecyclableTypes
{
	typedef typename cnVar::TSelect<cnVar::TClassIsInheritFrom<T,iObservedReference>::Value
		, impReferenceRecyclableLifeCycleObject< cnLib_THelper::RTL_TH::LifeCycle::impRefInterface<T,typename cnVar::TClassReferenceInterface<T>::Type> >
		, impObservedReferenceRecyclableLifeCycleObject< cnLib_THelper::RTL_TH::LifeCycle::impObsRefInterface<T,typename cnVar::TClassReferenceInterface<T>::Type> >
	>::Type tLifeCycleObject;

};
//---------------------------------------------------------------------------
template<class T>
class iSharedObjectRecycler
{
public:
	typedef cRecyclableLifeCycleSharedManager<typename TInterfaceLifeCycleRecyclableTypes<T>::tLifeCycleObject> tLifeCycleRecyclableSharedManager;
	iSharedObjectRecycler()noexcept(true)	: fManager(tLifeCycleRecyclableSharedManager::GetSharedManager()){}
	~iSharedObjectRecycler()noexcept(true){}

	iPtr<T> operator ()(void)noexcept(true){	Query();	}
	iPtr<T> Query(void)noexcept(true){
		auto Object=fManager->Query();
		cReferenceImplementationLifeCycleActivation::Start(Object);
		return iPtr<T>::TakeFromManual(Object);
	}
	
	static iPtr<T> QueryShared(void)noexcept(true){
		auto Manager=tLifeCycleRecyclableSharedManager::GetSharedManager();
		auto Object=Manager->Query();
		cReferenceImplementationLifeCycleActivation::Start(Object);
		return iPtr<T>::TakeFromManual(Object);
	}
private:

	rPtr<tLifeCycleRecyclableSharedManager> fManager;
};
//---------------------------------------------------------------------------
template<class T>
inline iPtr<T> iQuerySharedObject(void)noexcept(true){
	return iSharedObjectRecycler<T>::QueryShared();
}
//---------------------------------------------------------------------------
template<class T>
class iObjectRecycler
{
public:
	typedef cRecyclableLifeCycleManager<typename TInterfaceLifeCycleRecyclableTypes<T>::tLifeCycleObject> tLifeCycleRecyclableManager;

	iObjectRecycler()noexcept(true) : fManager(rCreate<tLifeCycleRecyclableManager>()){}
	~iObjectRecycler()noexcept(true){}

	iPtr<T> operator ()(void)noexcept(true){	Query();	}
	iPtr<T> Query(void)noexcept(true){
		auto Object=fManager->Query();
		cReferenceImplementationLifeCycleActivation::Start(Object);
		return iPtr<T>::TakeFromManual(Object);
	}
private:
	rPtr<tLifeCycleRecyclableManager> fManager;
};
//---------------------------------------------------------------------------

// Class Reference

//---------------------------------------------------------------------------
struct cAutoClassWeakRefTokenOperator;
struct cAutoClassPointerReferenceOperator;
//---------------------------------------------------------------------------
template<class T>
class aCls : public T, protected bcWeakReference, public cRTLAllocator
{
	friend cAutoClassPointerReferenceOperator;

	friend cAutoClassWeakRefTokenOperator;


public:
	typedef T tClass;
	using T::T;
	using T::operator =;

	typedef typename cnVar::TSelect<cnVar::TClassIsInheritFrom<T,bcDisposable>::Value
		, cCPPLifeCycleSharedManager<aCls>
		, cDisposableLifeCycleSharedManager<aCls>
	>::Type tLifeCycleSharedManager;

protected:

	void RefClassAcquireReference(void)noexcept(true){	bcWeakReference::ObjectIncreaseReference();	}
	void RefClassReleaseReference(void)noexcept(true){	bcWeakReference::ObjectDecreaseReference();	}

	virtual void cnLib_FUNC ObjectDeleted(void)noexcept(true)override{
		TReferenceObjectLifeCycleActivation<aCls>::Stop(this);
	}

private:
	friend bcVirtualLifeCycle::cLifeCycleActivation;

	void LifeCycleSetup(void)noexcept(true){
		bcWeakReference::ObjectResetReference();
	}
	void LifeCycleClear(void)noexcept(true){
	}
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
class arCls : public T, public TSelectRecyclableBase<T>::Type
	, protected bcWeakReference, protected cRTLAllocator
{
	friend cAutoRecyclableClassPointerReferenceOperator;
	friend cAutoClassWeakRefTokenOperator;

public:
	typedef T tClass;
	using T::T;
	using T::operator =;

	typedef cRecyclableLifeCycleSharedManager< arCls,cRecyclableObjectAllocator<arCls> > tLifeCycleSharedManager;
	typedef cRecyclableLifeCycleManager< arCls,cRecyclableObjectAllocator<arCls> > tLifeCycleManager;

	friend bcVirtualLifeCycle::cLifeCycleActivation;
	friend cRecyclableObjectAllocator<arCls>;

protected:

	virtual void cnLib_FUNC ObjectDeleted(void)noexcept(true)override{
		TReferenceObjectLifeCycleActivation<arCls>::Stop(this);
	}
	void RefClassAcquireReference(void)noexcept(true){	bcWeakReference::ObjectIncreaseReference();	}
	void RefClassReleaseReference(void)noexcept(true){	bcWeakReference::ObjectDecreaseReference();	}

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
	typedef iReferenceObserver tToken;

	template<class TAutoClass>
	static tToken* Register(TAutoClass *Pointer)noexcept(true){
		return Pointer->CreateReferenceObserver(nullptr,nullptr);
	}
	static void Unregister(tToken *Token)noexcept(true){
		return Token->Close();
	}

	template<class TAutoClass>
	struct tReference
		: cnVar::TTypeDef< cnVar::cPtrReference<typename TAutoClass::tClass,cAutoClassPointerReferenceOperator> >{};

	template<class TAutoClass>
	static cnVar::cPtrReference<typename TAutoClass::tClass,cAutoClassPointerReferenceOperator> Reference(tToken *Token)noexcept(true){
		return Token->Reference()?cnVar::cPtrReference<typename TAutoClass::tClass,cAutoClassPointerReferenceOperator>::TakeFromManual(Token):nullptr;
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
inline aClsRef<T> aClsCreate(void)noexcept(true)
{
	auto NewObject=new aCls<T>;
	aCls<T>::tLifeCycleSharedManager::ManageShared(NewObject);
	TReferenceObjectLifeCycleActivation< aCls<T> >::Start(NewObject);
	return cnVar::cPtrReference<aCls<T>,cAutoClassPointerReferenceOperator>::TakeFromManual(NewObject);
}

template<class T,class...TArgs>
inline aClsRef<T> aClsCreate(TArgs&&...Args)noexcept(true)
{
	auto NewObject=new aCls<T>(cnVar::Forward<TArgs>(Args)...);
	aCls<T>::tLifeCycleSharedManager::ManageShared(NewObject);
	TReferenceObjectLifeCycleActivation< aCls<T> >::Start(NewObject);
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
		TReferenceObjectLifeCycleActivation< arCls<T> >::Start(Object);
		return arClsRef<T>::TakeFromManual(Object);
	}

	static arClsRef<T> QueryShared(void)noexcept(true){
		auto Manager=tLifeCycleManager::GetSharedManager();
		auto Object=Manager->Query();
		TReferenceObjectLifeCycleActivation< arCls<T> >::Start(Object);
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
		TReferenceObjectLifeCycleActivation< arCls<T> >::Start(Object);
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
