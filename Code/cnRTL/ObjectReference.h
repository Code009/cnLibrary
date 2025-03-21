/*- cnRTL - Object Reference ----------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2006-01-17                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnRTL_ObjectReference_H__
#define __cnLibrary_cnRTL_ObjectReference_H__
/*-------------------------------------------------------------------------*/
#include <cnRTL/cnRTLHeader.h>
#include <cnRTL/RuntimeFunction.h>
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
	using cnClass::bcDisposable;
	using cnClass::bcVirtualLifeCycle;
	using cnClass::bcRecyclable;
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

	void ResetRef(bool InitialReference)noexcept(true);
	void IncRef(void)noexcept(true);
	void DecRef(void)noexcept(true);
	bool MakeStrongReference(void)noexcept(true);

	rPtr<iLibraryReference> CreateReference(iLibraryReferrer *Referrer)noexcept(true);
	void Update(void)noexcept(true);
	bool CheckShutdown(void)noexcept(true);
	void ReportReferenceSet(cStringBuffer<uChar16> &Buffer)noexcept(true);
protected:

	virtual void ReferenceUpdate(void)noexcept(true)=0;

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
// TLibraryInitialization
//{
//	static void Initialize(void)noexcept;
//	static void Finalize(void)noexcept;
//}
// TLibraryThreadExecution
//{
//	static rPtr<iMutex> QueryMutex(void)noexcept;
//	static bool Run(iProcedure *Procedure)noexcept;
//}
//---------------------------------------------------------------------------
template<class TLibraryInitialization,class TLibraryThreadExecution>
class cLibraryReference
{
public:
	rPtr<iLibraryReference> QueryReference(iLibraryReferrer *Referrer)noexcept(true){
		if(fContext->MakeStrongReference()){
			auto AutoLock=TakeLock(fContext->LibMutex);
			return fContext->CreateReference(Referrer);
		}
		else{
			rPtr<iMutex> LibMutex=TLibraryThreadExecution::QueryMutex();
			auto AutoLock=TakeLock(LibMutex);
			switch(fState){
			case sIdle:
				fState=sStartup;
				fContext.Construct(true,cnVar::MoveCast(LibMutex));
				StartupAndNotify();
				break;
			case sShutdown:
				fState=sActive;
				cnLib_SWITCH_FALLTHROUGH;
			default:
				fContext->IncRef();
				break;
			}
			return fContext->CreateReference(Referrer);

		}
	}

	rPtr<iLibraryReference> QueryReferenceAsync(iFunction<void (iLibraryReference*)noexcept(true)> *Procedure,iLibraryReferrer *Referrer)noexcept(true)
	{
		if(Procedure==nullptr)
			return QueryReference(Referrer);
		if(fContext->MakeStrongReference()){
			auto AutoLock=TakeLock(fContext->LibMutex);
			if(fState==sStartup){
				// is starting up, notify completion after initialized
				auto *NewParam=fContext->StartupQueryList.Append();
				NewParam->Referrer=Referrer;
				NewParam->CompletionProcedure=Procedure;
				return nullptr;
			}
			return fContext->CreateReference(Referrer);
		}
		else{
			rPtr<iMutex> LibMutex=TLibraryThreadExecution::QueryMutex();
			auto AutoLock=TakeLock(LibMutex);
			switch(fState){
			case sIdle:
				fState=sStartup;
				fContext.Construct(false,cnVar::MoveCast(LibMutex));
				// use distinct thread to startup
				if(TLibraryThreadExecution::Run(&fContext->StartupThreadProc)==false){
					// cannot start new thread, initialize now
					fContext->IncRef();
					StartupAndNotify();
					break;
				}
				cnLib_SWITCH_FALLTHROUGH;
			case sStartup:
				// is starting up, notify completion after initialized
				fContext->IncRef();
				{
					auto *NewParam=fContext->StartupQueryList.Append();
					NewParam->Referrer=Referrer;
					NewParam->CompletionProcedure=Procedure;
				}
				return nullptr;
			case sShutdown:
				fState=sActive;
				cnLib_SWITCH_FALLTHROUGH;
			default:
				fContext->IncRef();
				break;
			}
			return fContext->CreateReference(Referrer);

		}
	}

private:

	struct cQueryParameter
	{
		iLibraryReferrer *Referrer;
		iFunction<void (iLibraryReference*)noexcept(true)> *CompletionProcedure;
	};

	void StartupAndNotify(void)noexcept(true){
		TLibraryInitialization::Initialize();

		for(auto Param : fContext->StartupQueryList){
			auto NewReference=fContext->CreateReference(Param.Referrer);
			Param.CompletionProcedure->Execute(NewReference);
		}
		fContext->StartupQueryList.Clear();
		fState=sActive;
	}

protected:
	class cContext : public bcRegisteredReference
	{
	public:
		cContext(bool InitialReference,rPtr<iMutex> &&Mutex)noexcept(true)
			: bcRegisteredReference(InitialReference)
			, LibMutex(cnVar::MoveCast(Mutex))
		{}
		rPtr<iMutex> LibMutex;

		iProcedure *ShutdownCompletionProcedure=nullptr;

		virtual void ReferenceUpdate(void)noexcept(true)override final{
			auto Host=cnMemory::GetObjectFromMemberPointer(this,&cLibraryReference::fContext);
			return Host->ReferenceProcess();
		}
	private:
		friend cLibraryReference;
		cSeqList<cQueryParameter> StartupQueryList;

		class cStartupThreadProc : public iProcedure
		{
			virtual void cnLib_FUNC Execute(void)noexcept(true)override{
				auto HostContext=cnMemory::GetObjectFromMemberPointer(this,&cContext::StartupThreadProc);
				auto Host=cnMemory::GetObjectFromMemberPointer(HostContext,&cLibraryReference::fContext);

				auto AutoLock=TakeLock(HostContext->LibMutex);
				Host->StartupAndNotify();
			}
		}StartupThreadProc;

		class cShutdownNotifyProc : public iProcedure
		{
			virtual void cnLib_FUNC Execute(void)noexcept(true)override{
				auto HostContext=cnMemory::GetObjectFromMemberPointer(this,&cContext::ShutdownNotifyProc);
				auto Host=cnMemory::GetObjectFromMemberPointer(HostContext,&cLibraryReference::fContext);
				return Host->ShutdownProcess();
			}
		}ShutdownNotifyProc;
	};
	cnVar::cStaticVariable<cContext> fContext;

	bool IsIdle(void)const noexcept(true){	return fState==sIdle;	}
private:
	cnRTL::cExclusiveFlag fReferenceProcessFlag;
	static constexpr ufInt8 sIdle=0;
	static constexpr ufInt8 sStartup=1;
	static constexpr ufInt8 sActive=2;
	static constexpr ufInt8 sShutdown=3;
	ufInt8 fState=0;
	void ReferenceProcess(void)noexcept(true){
		if(fReferenceProcessFlag.Acquire()==false)
			return;

		iMutex *LibMutex=fContext->LibMutex;
		rIncReference(LibMutex,'proc');
		do{
			fReferenceProcessFlag.Continue();

			auto AutoLock=TakeLock(LibMutex);

			fContext->Update();
			if(fContext->CheckShutdown()){
				// shutdown
				if(fState==sActive){
					fState=sShutdown;
					// use distinct thread to shutdown
					if(TLibraryThreadExecution::Run(&fContext->ShutdownNotifyProc)){
						return;
					}
					// failed to create thread
					fState=sActive;
				}
			}

		}while(fReferenceProcessFlag.Release()==false);
		rDecReference(LibMutex,'proc');
	}
	void ShutdownProcess(void)noexcept(true)
	{
		iProcedure *ShutdownNotifyProc=nullptr;
		iMutex *LibMutex=fContext->LibMutex;
		do{
			fReferenceProcessFlag.Continue();

			auto AutoLock=TakeLock(LibMutex);

			if(fState==sShutdown){
				// shutdown
				ShutdownNotifyProc=fContext->ShutdownCompletionProcedure;
				TLibraryInitialization::Finalize();
				fContext.Destruct();
				// notify shutdown
				fState=sIdle;
			}
			else if(fState==sActive){
				fContext->Update();
				if(fContext->CheckShutdown()){
					// shutdown
					ShutdownNotifyProc=fContext->ShutdownCompletionProcedure;
					TLibraryInitialization::Finalize();
					fContext.Destruct();
					// notify shutdown
					fState=sIdle;
				}
			}
		}while(fReferenceProcessFlag.Release()==false);
		rDecReference(LibMutex,'proc');
		if(ShutdownNotifyProc!=nullptr){
			ShutdownNotifyProc->Execute();
		}
	}
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
using rInnerPtr = cnClass::cPtrReference<T,cDualReference::cPointerReferenceOperator>;
//---------------------------------------------------------------------------
template<class T>
using TReferenceObjectLifeCycleActivation=typename cnVar::TSelect<cnVar::TClassIsInheritFrom<T,bcVirtualLifeCycle>::Value
	, typename cnVar::TSelect<cnVar::TClassIsInheritFrom<T,bcDisposable>::Value
		, cnClass::CPPLifeCycleActivation
		, cnClass::DisposableLifeCycleActivation
	>::Type
	, cnClass::VirtualLifeCycleActivation
>::Type;
template<class T>
using TReferenceObjectLifeCycleSharedManager=typename cnVar::TSelect<cnVar::TClassIsInheritFrom<T,bcDisposable>::Value
	, cnClass::CPPLifeCycleSharedManager<T>
	, cnClass::DisposableLifeCycleSharedManager<T>
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
using aPtr = cnClass::cPtrOwner<T,aPointerOwnerOperator>;
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
struct ReferenceImplementationLifeCycleActivation
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
			ReferenceImplementationLifeCycleActivation::Stop(this);
		}
	}
private:
	cAtomicVar<uIntn> fRefCount;

	friend ReferenceImplementationLifeCycleActivation;
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
		ReferenceImplementationLifeCycleActivation::Stop(this);
	}

private:
	friend ReferenceImplementationLifeCycleActivation;
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


}	// namespace LifeCycle
}	// namespace RTL_TH
}	// namespace cnLib_THelper
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnRTL{
//---------------------------------------------------------------------------
template<class T>
struct TReferenceLiftCycleObject
	: cnVar::TSelect<cnVar::TClassIsInheritFrom<T,iObservedReference>::Value
	, cnRTL::impReferenceLifeCycleObject<T>
	, cnRTL::impObservedReferenceLifeCycleObject<T>
	>{};
//---------------------------------------------------------------------------
template<class T>
inline rPtr<T> rCreate(void)noexcept(true)
{
	typedef typename TReferenceLiftCycleObject<T>::Type tLifeCycleObject;
	auto Object=new tLifeCycleObject;
	TReferenceObjectLifeCycleSharedManager<tLifeCycleObject>::ManageShared(Object);
	ReferenceImplementationLifeCycleActivation::Start(Object);
	return rPtr<T>::TakeFromManual(Object);
}
//---------------------------------------------------------------------------
template<class T,class...TArgs>
inline rPtr<T> rCreate(TArgs&&...Args)noexcept(true)
{
	typedef typename TReferenceLiftCycleObject<T>::Type tLifeCycleObject;
	auto Object=new tLifeCycleObject(cnVar::Forward<TArgs>(Args)...);
	TReferenceObjectLifeCycleSharedManager<tLifeCycleObject>::ManageShared(Object);
	ReferenceImplementationLifeCycleActivation::Start(Object);
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
}	// namespace LifeCycle
}	// namespace RTL_TH
}	// namespace cnLib_THelper
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnRTL{
//---------------------------------------------------------------------------
template<class T>
struct TInterfaceLiftCycleObject
	: cnVar::TSelect<cnVar::TClassIsInheritFrom<T,iObservedReference>::Value
		, cnRTL::impReferenceLifeCycleObject< cnLib_THelper::RTL_TH::LifeCycle::impRefInterface<T,typename cnClass::TClassReferenceInterface<T>::Type> >
		, cnRTL::impObservedReferenceLifeCycleObject< cnLib_THelper::RTL_TH::LifeCycle::impObsRefInterface<T,typename cnClass::TClassReferenceInterface<T>::Type> >
	>{};
//---------------------------------------------------------------------------
template<class T>
inline iPtr<T> iCreate(void)noexcept(true)
{
	typedef typename TInterfaceLiftCycleObject<T>::Type tLifeCycleObject;
	auto Object=new tLifeCycleObject;
	TReferenceObjectLifeCycleSharedManager<tLifeCycleObject>::ManageShared(Object);
	ReferenceImplementationLifeCycleActivation::Start(Object);
	return iPtr<T>::TakeFromManual(Object);
}
//---------------------------------------------------------------------------
template<class T,class...TArgs>
inline iPtr<T> iCreate(TArgs&&...Args)noexcept(true)
{
	typedef typename TInterfaceLiftCycleObject<T>::Type tLifeCycleObject;
	auto Object=new tLifeCycleObject(cnVar::Forward<TArgs>(Args)...);
	TReferenceObjectLifeCycleSharedManager<tLifeCycleObject>::ManageShared(Object);
	ReferenceImplementationLifeCycleActivation::Start(Object);
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
using arPtr = cnClass::cPtrOwner<T,arPointerOwnerOperator>;
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
	typedef cnClass::RecyclableLifeCycleSharedManager< cAutoRecyclableObject<T>,cnClass::RecyclableObjectAllocator< cAutoRecyclableObject<T> > > tLifeCycleRecyclableSharedManager;
	
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
	typedef cnClass::cRecyclableLifeCycleManager< cAutoRecyclableObject<T>,cnClass::RecyclableObjectAllocator< cAutoRecyclableObject<T> > > tLifeCycleRecyclableManager;

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
			ReferenceImplementationLifeCycleActivation::Stop(this);
		}
	}
private:
	cAtomicVar<uIntn> fRefCount;

	friend ReferenceImplementationLifeCycleActivation;
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
		ReferenceImplementationLifeCycleActivation::Stop(this);
	}

private:
	friend ReferenceImplementationLifeCycleActivation;

	void LifeCycleSetup(void)noexcept(true){
		bcWeakReference::ObjectResetReference();
	}
	void LifeCycleClear(void)noexcept(true){
	}

};
//---------------------------------------------------------------------------
template<class T>
struct TReferenceLifeCycleRecyclableLiftCycleObject
	: cnVar::TSelect<cnVar::TClassIsInheritFrom<T,iObservedReference>::Value
		, impReferenceRecyclableLifeCycleObject<T>
		, impObservedReferenceRecyclableLifeCycleObject<T>
	>{};
//---------------------------------------------------------------------------
template<class T>
class rSharedObjectRecycler
{
public:
	typedef cnClass::RecyclableLifeCycleSharedManager<typename TReferenceLifeCycleRecyclableLiftCycleObject<T>::Type> tLifeCycleRecyclableSharedManager;
	rSharedObjectRecycler()noexcept(true) : fManager(tLifeCycleRecyclableSharedManager::GetSharedManager()){}
	~rSharedObjectRecycler()noexcept(true){}

	rPtr<T> operator ()(void)noexcept(true){	Query();	}
	rPtr<T> Query(void)noexcept(true){
		auto Object=fManager->Query();
		ReferenceImplementationLifeCycleActivation::Start(Object);
		return rPtr<T>::TakeFromManual(Object);
	}

	static rPtr<T> QueryShared(void)noexcept(true){
		auto Manager=tLifeCycleRecyclableSharedManager::GetSharedManager();
		auto Object=Manager->Query();
		ReferenceImplementationLifeCycleActivation::Start(Object);
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
	typedef cnClass::cRecyclableLifeCycleManager<typename TReferenceLifeCycleRecyclableLiftCycleObject<T>::Type> tLifeCycleRecyclableManager;
	rObjectRecycler()noexcept(true) : fManager(rCreate<tLifeCycleRecyclableManager>()){}
	~rObjectRecycler()noexcept(true){}

	rPtr<T> operator ()(void)noexcept(true){	Query();	}
	rPtr<T> Query(void)noexcept(true){
		auto Object=fManager->Query();
		ReferenceImplementationLifeCycleActivation::Start(Object);
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
		, impReferenceRecyclableLifeCycleObject< cnLib_THelper::RTL_TH::LifeCycle::impRefInterface<T,typename cnClass::TClassReferenceInterface<T>::Type> >
		, impObservedReferenceRecyclableLifeCycleObject< cnLib_THelper::RTL_TH::LifeCycle::impObsRefInterface<T,typename cnClass::TClassReferenceInterface<T>::Type> >
	>::Type tLifeCycleObject;

};
//---------------------------------------------------------------------------
template<class T>
class iSharedObjectRecycler
{
public:
	typedef cnClass::RecyclableLifeCycleSharedManager<typename TInterfaceLifeCycleRecyclableTypes<T>::tLifeCycleObject> tLifeCycleRecyclableSharedManager;
	iSharedObjectRecycler()noexcept(true)	: fManager(tLifeCycleRecyclableSharedManager::GetSharedManager()){}
	~iSharedObjectRecycler()noexcept(true){}

	iPtr<T> operator ()(void)noexcept(true){	Query();	}
	iPtr<T> Query(void)noexcept(true){
		auto Object=fManager->Query();
		ReferenceImplementationLifeCycleActivation::Start(Object);
		return iPtr<T>::TakeFromManual(Object);
	}
	
	static iPtr<T> QueryShared(void)noexcept(true){
		auto Manager=tLifeCycleRecyclableSharedManager::GetSharedManager();
		auto Object=Manager->Query();
		ReferenceImplementationLifeCycleActivation::Start(Object);
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
	typedef cnClass::cRecyclableLifeCycleManager<typename TInterfaceLifeCycleRecyclableTypes<T>::tLifeCycleObject> tLifeCycleRecyclableManager;

	iObjectRecycler()noexcept(true) : fManager(rCreate<tLifeCycleRecyclableManager>()){}
	~iObjectRecycler()noexcept(true){}

	iPtr<T> operator ()(void)noexcept(true){	Query();	}
	iPtr<T> Query(void)noexcept(true){
		auto Object=fManager->Query();
		ReferenceImplementationLifeCycleActivation::Start(Object);
		return iPtr<T>::TakeFromManual(Object);
	}
private:
	rPtr<tLifeCycleRecyclableManager> fManager;
};
//---------------------------------------------------------------------------

// Class Reference

//---------------------------------------------------------------------------
template<bool NotifyProcedure>	struct cAutoClassWeakRefTokenOperator;
struct cAutoClassPointerReferenceOperator;
//---------------------------------------------------------------------------
template<class T>
class aCls : public T, protected bcWeakReference
{
	friend cAutoClassPointerReferenceOperator;

	template<bool NotifyProcedure>	friend struct cAutoClassWeakRefTokenOperator;


public:
	typedef T tClass;
	using T::T;
	using T::operator =;

	typedef typename cnVar::TSelect<cnVar::TClassIsInheritFrom<T,bcDisposable>::Value
		, cnClass::CPPLifeCycleSharedManager<aCls>
		, cnClass::DisposableLifeCycleSharedManager<aCls>
	>::Type tLifeCycleSharedManager;

protected:

	void RefClassAcquireReference(void)noexcept(true){	bcWeakReference::ObjectIncreaseReference();	}
	void RefClassReleaseReference(void)noexcept(true){	bcWeakReference::ObjectDecreaseReference();	}

	virtual void cnLib_FUNC ObjectDeleted(void)noexcept(true)override{
		TReferenceObjectLifeCycleActivation<aCls>::Stop(this);
	}

private:
	friend bcVirtualLifeCycle::cLifeCycleActivation;
	friend ReferenceImplementationLifeCycleActivation;
	
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
using aClsRef = cnClass::cPtrReference<aCls<T>,cAutoClassPointerReferenceOperator>;
template<class T>
using aClsConstRef = cnClass::cPtrReference<const aCls<T>,cAutoClassPointerReferenceOperator>;
template<class T>
using aClsAtomicRef = cnClass::cAtomicPtrReference<aCls<T>,cAutoClassPointerReferenceOperator>;

//---------------------------------------------------------------------------
struct cAutoRecyclableClassPointerReferenceOperator;
template<class T>	struct TAutoRecyclableClassCPPLifeCycleManagerSelect;
template<class T>	struct TAutoRecyclableClassVirtualLifeCycleManagerSelect;
//---------------------------------------------------------------------------
template<class T>
class arCls : public T, public TSelectRecyclableBase<T>::Type
	, protected bcWeakReference
{
	friend cAutoRecyclableClassPointerReferenceOperator;
	template<bool NotifyProcedure>	friend struct cAutoClassWeakRefTokenOperator;

public:
	typedef T tClass;
	using T::T;
	using T::operator =;

	typedef cnClass::RecyclableLifeCycleSharedManager< arCls,cnClass::RecyclableObjectAllocator<arCls> > tLifeCycleSharedManager;
	typedef cnClass::cRecyclableLifeCycleManager< arCls,cnClass::RecyclableObjectAllocator<arCls> > tLifeCycleManager;

	friend bcVirtualLifeCycle::cLifeCycleActivation;

protected:

	virtual void cnLib_FUNC ObjectDeleted(void)noexcept(true)override{
		TReferenceObjectLifeCycleActivation<arCls>::Stop(this);
	}
	void RefClassAcquireReference(void)noexcept(true){	bcWeakReference::ObjectIncreaseReference();	}
	void RefClassReleaseReference(void)noexcept(true){	bcWeakReference::ObjectDecreaseReference();	}
private:
	friend bcVirtualLifeCycle::cLifeCycleActivation;
	friend ReferenceImplementationLifeCycleActivation;

	void LifeCycleSetup(void)noexcept(true){
		bcWeakReference::ObjectResetReference();
	}
	void LifeCycleClear(void)noexcept(true){
	}

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
using arClsRef = cnClass::cPtrReference<arCls<T>,cAutoRecyclableClassPointerReferenceOperator>;
template<class T>
using arClsConstRef = cnClass::cPtrReference<const arCls<T>,cAutoRecyclableClassPointerReferenceOperator>;
template<class T>
using arClsAtomicRef = cnClass::cAtomicPtrReference<arCls<T>,cAutoRecyclableClassPointerReferenceOperator>;

//---------------------------------------------------------------------------
template<>
struct cAutoClassWeakRefTokenOperator<true>
{
	class tRegistration
	{
	public:
		iReference *NotifyReference=nullptr;
		iFunction<void (void)noexcept(true)> *NotifyProcedure=nullptr;
	private:
		iReferenceObserver *Observer;
		friend cAutoClassWeakRefTokenOperator<true>;
	};

	template<class TAutoClass>
	static bool Register(tRegistration &Registration,TAutoClass *Pointer)noexcept(true){
		return nullptr!=(Registration.Observer=Pointer->CreateReferenceObserver(Registration.NotifyReference,Registration.NotifyProcedure));
	}
	template<class TAutoClass>
	static void Unregister(tRegistration &Registration,TAutoClass*)noexcept(true){
		return Registration.Observer->Close();
	}

	static void Move(tRegistration &Dest,tRegistration &Src)noexcept(true){
		Dest.Observer=Src.Observer;
		Src.Observer=nullptr;
	}

	template<class TAutoClass>
	struct tReference
		: cnVar::TTypeDef< cnClass::cPtrReference<aCls<typename TAutoClass::tClass>,cAutoClassPointerReferenceOperator> >{};

	template<class TAutoClass>
	static cnClass::cPtrReference<aCls<typename TAutoClass::tClass>,cAutoClassPointerReferenceOperator> Reference(tRegistration &Registration,TAutoClass *Pointer)noexcept(true){
		return Registration.Observer->Reference()?cnClass::cPtrReference<aCls<typename TAutoClass::tClass>,cAutoClassPointerReferenceOperator>::TakeFromManual(Pointer):nullptr;
	}
};
template<>
struct cAutoClassWeakRefTokenOperator<false>
{
	struct tRegistration
	{
	private:
		iReferenceObserver *Observer;
		friend cAutoClassWeakRefTokenOperator<false>;
	};

	template<class TAutoClass>
	static bool Register(tRegistration &Registration,TAutoClass *Pointer)noexcept(true){
		return nullptr!=(Registration.Observer=Pointer->CreateReferenceObserver(nullptr,nullptr));
	}
	template<class TAutoClass>
	static void Unregister(tRegistration &Registration,TAutoClass*)noexcept(true){
		return Registration.Observer->Close();
	}

	static void Move(tRegistration &Dest,tRegistration &Src)noexcept(true){
		Dest.Observer=Src.Observer;
		Src.Observer=nullptr;
	}

	template<class TAutoClass>
	struct tReference
		: cnVar::TTypeDef< cnClass::cPtrReference<typename TAutoClass::tClass,cAutoClassPointerReferenceOperator> >{};

	template<class TAutoClass>
	static cnClass::cPtrReference<typename TAutoClass::tClass,cAutoClassPointerReferenceOperator> Reference(tRegistration &Registration,TAutoClass *Pointer)noexcept(true){
		return Registration.Observer->Reference()?cnClass::cPtrReference<typename TAutoClass::tClass,cAutoClassPointerReferenceOperator>::TakeFromManual(Pointer):nullptr;
	}
};
//---------------------------------------------------------------------------
template<class T,bool NotifyProcedure=false>
using aClsWeakRef=cnClass::cPtrWeakReference< aCls<T>,cAutoClassWeakRefTokenOperator<NotifyProcedure> >;
template<class T,bool NotifyProcedure=false>
using arClsWeakRef=cnClass::cPtrWeakReference< arCls<T>,cAutoClassWeakRefTokenOperator<NotifyProcedure> >;
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
	ReferenceImplementationLifeCycleActivation::Start(NewObject);
	return cnClass::cPtrReference<aCls<T>,cAutoClassPointerReferenceOperator>::TakeFromManual(NewObject);
}

template<class T,class...TArgs>
inline aClsRef<T> aClsCreate(TArgs&&...Args)noexcept(true)
{
	auto NewObject=new aCls<T>(cnVar::Forward<TArgs>(Args)...);
	aCls<T>::tLifeCycleSharedManager::ManageShared(NewObject);
	ReferenceImplementationLifeCycleActivation::Start(NewObject);
	return cnClass::cPtrReference<aCls<T>,cAutoClassPointerReferenceOperator>::TakeFromManual(NewObject);
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
		ReferenceImplementationLifeCycleActivation::Start(Object);
		return arClsRef<T>::TakeFromManual(Object);
	}

	static arClsRef<T> QueryShared(void)noexcept(true){
		auto Manager=tLifeCycleManager::GetSharedManager();
		auto Object=Manager->Query();
		ReferenceImplementationLifeCycleActivation::Start(Object);
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
