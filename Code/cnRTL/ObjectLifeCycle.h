/*- cnRTL - Object Life Cycle ---------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2021-11-20                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnRTL_ObjectLifeCycle_H__
#define __cnLibrary_cnRTL_ObjectLifeCycle_H__
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
namespace cnLib_THelper{
namespace RTL_TH{
namespace LifeCycle{


template<class TEnable,class T,class TDefaultActivation>
struct SelectActivation
	: cnVar::TTypeDef< TDefaultActivation >{};
template<class T,class TDefaultActivation>
struct SelectActivation<typename cnVar::TTypeRequireDefined<void,typename T::template tActivation<T> >::Type,T,TDefaultActivation>
	: cnVar::TTypeDef< typename T::template tActivation<T> >{};

}	// namespace LifeCycle
}	// namespace RTL_TH
}	// namespace cnLib_THelper

//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnRTL{
// 			CPP						VirutalLifeCycle
//			new							new
//			| <-------Take				|	<-----Take
//			|			|				|			|
//Start----------------	|		----------------	|
//			|			|			VirtualStart	|
//			|			|				|			|
//Stop-----------------	|		----------------	|
//			|			|				|			|
//			|			|			VirtualStop		|
//			|			|				|			|
//			|			|			VirtualDelete	|
//			|------->Recycle			|-------->Recycle
//			|							|
//		  delete					  delete
//---------------------------------------------------------------------------
//TLifeCycleInstance
//{
//	template<TLifeCycleObject>
//	TLifeCycleActivation
//	{
//		// Start
//		//	call the function to start the object's life cycle
//		static void Start(TLifeCycleObject *Object)noexcept;
//		// Stop
//		//	call the function to stop the object's life cycle
//		static void Stop(TLifeCycleObject *Object)noexcept;
//	};
//};
//---------------------------------------------------------------------------
//TLifeCycleManager
//{
//	typedef tLifeCycleObject;
//	typedef tLifeCycleActivation;
//	// Manage
//	//	call the function to start manage the object's life cycle
//	void Manage(tLifeCycleObject *Object)noexcept;
//};
//---------------------------------------------------------------------------
//TLifeCycleSharedManager
//{
//	typedef tLifeCycleObject;
//	typedef tLifeCycleActivation;
//	// Manage
//	//	call the function to start manage the object's life cycle
//	static void ManageShared(tLifeCycleObject *Object)noexcept;
//};
//---------------------------------------------------------------------------
template<class T,class TDefaultActivation>
struct TSelectActivation
	: cnLib_THelper::RTL_TH::LifeCycle::SelectActivation<void,T,TDefaultActivation>
{
};
//---------------------------------------------------------------------------
struct cCPPLifeCycleInstance
{
	template<class TLifeCycleObject>
	struct tActivation
	{
		static void Start(TLifeCycleObject*)noexcept(true){}
		static void Stop(TLifeCycleObject *Object)noexcept(true){	delete Object;	}
	};
};
//---------------------------------------------------------------------------
template<class T>
class cCPPLifeCycleSharedManager
{
public:
	typedef T tLifeCycleObject;
	typedef typename TSelectActivation< T,cCPPLifeCycleInstance::tActivation<T> >::Type tLifeCycleActivation;

	static void ManageShared(tLifeCycleObject*)noexcept(true){}
};
//---------------------------------------------------------------------------
class bcVirtualLifeCycle
{
protected:

	// VirtualStarted
	//	will be called by final implementation when the life cycle of the object ended.
	//	default implementation is nop
	void VirtualStarted(void)noexcept(true){}
	// VirtualStopped
	//	will be called by final implementation when the life cycle of the object ended.
	//	default implementation is calling VirtualDelete to finalize the object
	void VirtualStopped(void)noexcept(true){	VirtualDelete();	}

	void VirtualDelete(void)noexcept(true);

public:

	template<class TLifeCycleObject>
	struct cActivation
	{
		static void Start(TLifeCycleObject *Object)noexcept(true){	Object->VirtualStarted();	}
		static void Stop(TLifeCycleObject *Object)noexcept(true){	Object->VirtualStopped();	}
	};


	class bcManager
	{
	public:
		void Manage(bcVirtualLifeCycle *Object)noexcept(true);
		
		virtual void Dispose(bcVirtualLifeCycle *Object)noexcept(true)=0;
	};

private:
	bcManager *LifeCycleManager=nullptr;
};
//---------------------------------------------------------------------------
struct cVirtualLifeCycleInstance
{
	template<class TLifeCycleObject>
	struct tActivation
		: bcVirtualLifeCycle::cActivation<TLifeCycleObject>{};

};
//---------------------------------------------------------------------------
template<class T>
class cVirtualLifeCycleSharedManager : public bcVirtualLifeCycle::bcManager
{
public:
	typedef T tLifeCycleObject;
	typedef typename TSelectActivation< T,bcVirtualLifeCycle::cActivation<T> >::Type tLifeCycleActivation;

	static_assert(cnVar::TIsConvertible<T*,bcVirtualLifeCycle*>::Value,"Incorrect Instance");

	static cVirtualLifeCycleSharedManager* GetSharedManager(void){
		return cnVar::StaticInitializedSinglton<cVirtualLifeCycleSharedManager>();
	}

	static void ManageShared(tLifeCycleObject *Object)noexcept(true){
		auto Manager=GetSharedManager();
		Manager->Manage(Object);
	}

	virtual void Dispose(bcVirtualLifeCycle *Object)noexcept(true) override{
		delete static_cast<tLifeCycleObject*>(Object);
	}
};
//---------------------------------------------------------------------------
struct cCPPLifeCycleRecyclableInstance
{
	template<class TLifeCycleObject>
	struct tActivation
	{
		static void Start(TLifeCycleObject*)noexcept(true){}
		static void Stop(TLifeCycleObject *Object)noexcept(true){	Object->cCPPLifeCycleRecyclableInstance::LifeCycleManager->Dispose(Object);	}
	};	

	class bcManager
	{
	public:
		void Manage(cCPPLifeCycleRecyclableInstance *Object)noexcept(true);

		virtual void Dispose(cCPPLifeCycleRecyclableInstance *Object)noexcept(true)=0;
	};

	cCPPLifeCycleRecyclableInstance *Next;
	bcManager *LifeCycleManager;
};
//---------------------------------------------------------------------------
template<class T>
struct cRecyclableObjectAllocator
{
	static T* New(void)noexcept(cnLib_NOEXCEPTEXPR(new T)){	return new T;	}
	static void Delete(T *p)noexcept(cnLib_NOEXCEPTEXPR(delete p)){	delete p;	}
};
//---------------------------------------------------------------------------
template< class TLifeCycleObject,class TRecyclableObjectAllocator=cRecyclableObjectAllocator<TLifeCycleObject> >
class bcCPPLifeCycleRecyclableManager : public cCPPLifeCycleRecyclableInstance::bcManager, public TRecyclableObjectAllocator
{
public:
	typedef TLifeCycleObject tLifeCycleObject;
	typedef typename tLifeCycleObject::template tActivation<tLifeCycleObject> tLifeCycleActivation;

	static_assert(cnVar::TIsConvertible<TLifeCycleObject*,cCPPLifeCycleRecyclableInstance*>::Value,"Incorrect Instance");

	void Manage(tLifeCycleObject *Object)noexcept(true){	return bcManager::Manage(Object);	}

	virtual void Dispose(cCPPLifeCycleRecyclableInstance *Object)noexcept(true) override{
		fRecycleStack.Push(static_cast<tLifeCycleObject*>(Object));
	}

	struct cSingleLinkItemOperator
	{
		typedef tLifeCycleObject tItem;

		typedef cnMemory::cPlainAlignedData<sizeof(void*),cCPPLifeCycleRecyclableInstance> tLinkInstance;

		static tLifeCycleObject* LinkNode(tLinkInstance &Link)noexcept(true){
			return static_cast<tLifeCycleObject*>(reinterpret_cast<cCPPLifeCycleRecyclableInstance*>(&Link));
		}
		static const tLifeCycleObject* LinkNode(const tLinkInstance &Link)noexcept(true){
			return reinterpret_cast<const tLifeCycleObject*>(reinterpret_cast<cCPPLifeCycleRecyclableInstance*>(&Link));
		}

		static tLifeCycleObject* GetNext(const tLifeCycleObject *Item)noexcept(true){
			return static_cast<tLifeCycleObject*>(Item->cCPPLifeCycleRecyclableInstance::Next);
		}
		static void SetNext(tLifeCycleObject *Item,tLifeCycleObject *Next)noexcept(true){
			Item->cCPPLifeCycleRecyclableInstance::Next=Next;
		}
	};

	tLifeCycleObject* Fetch(void)noexcept(true){
		auto LCObject=fRecycleStack.Pop();
		if(LCObject==nullptr){
			return nullptr;
		}
		Restore(LCObject);
		return LCObject;
	}

	tLifeCycleObject* Query(void)noexcept(cnLib_NOEXCEPTEXPR(new tLifeCycleObject)){
		auto LCObject=fRecycleStack.Pop();
		if(LCObject==nullptr){
			LCObject=TRecyclableObjectAllocator::New();
			LCObject=new tLifeCycleObject;
			Manage(LCObject);
		}
		Restore(LCObject);
		return LCObject;
	}

	virtual void Restore(tLifeCycleObject*)noexcept(true){}

protected:

	tLifeCycleObject* FetchObjects(void){
		return fRecycleStack.Swap(nullptr);
	}

	void DeleteObjects(void)noexcept(true){
		auto ItemsToDelete=fRecycleStack.Swap(nullptr);
		while(ItemsToDelete!=nullptr){
			auto DeleteObject=ItemsToDelete;
			ItemsToDelete=cSingleLinkItemOperator::GetNext(ItemsToDelete);

			TRecyclableObjectAllocator::Delete(DeleteObject);
			delete DeleteObject;
		}
	}

private:
	cnAsync::cAtomicStack<cSingleLinkItemOperator> fRecycleStack;
};
//---------------------------------------------------------------------------
template< class TLifeCycleObject,class TRecyclableObjectAllocator=cRecyclableObjectAllocator<TLifeCycleObject> >
class cCPPLifeCycleRecyclableSharedManager : public bcCPPLifeCycleRecyclableManager<TLifeCycleObject,TRecyclableObjectAllocator>
{
public:
	typedef TLifeCycleObject tLifeCycleObject;

	cnRTL_CONSTEXPR_FUNC cCPPLifeCycleRecyclableSharedManager()noexcept(true)
		: fRefCount(0){}
	
	static cCPPLifeCycleRecyclableSharedManager* GetSharedManager(void){
		return cnVar::StaticInitializedSinglton<cCPPLifeCycleRecyclableSharedManager>();
	}

	static void ManageShared(tLifeCycleObject *Object)noexcept(true){
		auto Manager=GetSharedManager();
		Manager->Manage(Object);
	}
	

	void IncreaseReference(void)noexcept(true){
		fRefCount.Free++;
	}
	void DecreaseReference(void)noexcept(true){
		if(fRefCount.Free--==1){
			this->DeleteObjects();
		}
	}

	virtual void Restore(tLifeCycleObject*)noexcept(true) override{
		rIncReference(this,'lcle');
	}

	virtual void Dispose(cCPPLifeCycleRecyclableInstance *Object)noexcept(true) override{
		bcCPPLifeCycleRecyclableManager<TLifeCycleObject,TRecyclableObjectAllocator>::Dispose(Object);
		rDecReference(this,'lcle');
	}
private:
	cAtomicVar<uIntn> fRefCount;
};
//---------------------------------------------------------------------------
template< class TLifeCycleObject,class TRecyclableObjectAllocator=cRecyclableObjectAllocator<TLifeCycleObject> >
class cCPPLifeCycleRecyclableManager : public iReference, public bcCPPLifeCycleRecyclableManager<TLifeCycleObject,TRecyclableObjectAllocator>
{
public:
	typedef TLifeCycleObject tLifeCycleObject;

	cCPPLifeCycleRecyclableManager()noexcept(true){}
	~cCPPLifeCycleRecyclableManager()noexcept(true){
		this->DeleteObjects();
	}


	virtual void Restore(TLifeCycleObject*)noexcept(true) override{
		rIncReference(this,'lcle');
	}

	virtual void Dispose(cCPPLifeCycleRecyclableInstance *Object)noexcept(true) override{
		bcCPPLifeCycleRecyclableManager<TLifeCycleObject,TRecyclableObjectAllocator>::Dispose(Object);
		rDecReference(this,'lcle');
	}
};
//---------------------------------------------------------------------------
struct cVirtualLifeCycleRecyclableInstance
{
	cVirtualLifeCycleRecyclableInstance *LifeCycleNext;

	template<class TLifeCycleObject>
	struct tActivation
		: bcVirtualLifeCycle::cActivation<TLifeCycleObject>{};	
};
//---------------------------------------------------------------------------
template< class TLifeCycleObject,class TRecyclableObjectAllocator=cRecyclableObjectAllocator<TLifeCycleObject> >
class bcVirtualLifeCycleRecyclableManager : public bcVirtualLifeCycle::bcManager
{
public:
	typedef TLifeCycleObject tLifeCycleObject;
	typedef typename tLifeCycleObject::template tActivation<tLifeCycleObject> tLifeCycleActivation;

	static_assert(cnVar::TIsConvertible<TLifeCycleObject*,cVirtualLifeCycleRecyclableInstance*>::Value,"Incorrect Instance");
	static_assert(cnVar::TIsConvertible<TLifeCycleObject*,bcVirtualLifeCycle*>::Value,"Incorrect Instance");


	void Manage(tLifeCycleObject *Object)noexcept(true){	return bcManager::Manage(Object);	}

	virtual void Dispose(bcVirtualLifeCycle *Object)noexcept(true) override{
		fRecycleStack.Push(static_cast<tLifeCycleObject*>(Object));
	}

	struct cSingleLinkItemOperator
	{
		typedef tLifeCycleObject tItem;
		typedef cnMemory::cPlainAlignedData<sizeof(void*),cVirtualLifeCycleRecyclableInstance> tLinkInstance;

		static tLifeCycleObject* LinkNode(tLinkInstance &Link)noexcept(true){
			return static_cast<tLifeCycleObject*>(reinterpret_cast<cVirtualLifeCycleRecyclableInstance*>(&Link));
		}
		static const tLifeCycleObject* LinkNode(const tLinkInstance &Link)noexcept(true){
			return reinterpret_cast<const tLifeCycleObject*>(reinterpret_cast<cVirtualLifeCycleRecyclableInstance*>(&Link));
		}

		static tLifeCycleObject* GetNext(const tLifeCycleObject *Item)noexcept(true){
			return static_cast<tLifeCycleObject*>(Item->cVirtualLifeCycleRecyclableInstance::Next);
		}
		static void SetNext(tLifeCycleObject *Item,tLifeCycleObject *Next)noexcept(true){
			Item->cVirtualLifeCycleRecyclableInstance::Next=Next;
		}
	};


	tLifeCycleObject* Fetch(void)noexcept(true){
		auto LCObject=fRecycleStack.Pop();
		if(LCObject==nullptr){
			return nullptr;
		}
		Restore(LCObject);
		return LCObject;
	}

	tLifeCycleObject* Query(void)noexcept(cnLib_NOEXCEPTEXPR(new tLifeCycleObject)){
		auto LCObject=fRecycleStack.Pop();
		if(LCObject==nullptr){
			LCObject=TRecyclableObjectAllocator::New();
			bcVirtualLifeCycle::bcManager::Manage(LCObject);
		}
		Restore(LCObject);
		return LCObject;
	}

	virtual void Restore(tLifeCycleObject *Object)noexcept(true){}
protected:

	TLifeCycleObject* FetchObjects(void){
		return fRecycleStack.Swap(nullptr);
	}

	void DeleteObjects(void)noexcept(true){
		auto ItemsToDelete=fRecycleStack.Swap(nullptr);
		while(ItemsToDelete!=nullptr){
			auto DeleteObject=ItemsToDelete;
			ItemsToDelete=cSingleLinkItemOperator::GetNext(ItemsToDelete);

			TRecyclableObjectAllocator::Delete(DeleteObject);
		}
	}
private:
	cnAsync::cAtomicStack<cSingleLinkItemOperator> fRecycleStack;
};
//---------------------------------------------------------------------------
template< class TLifeCycleObject,class TRecyclableObjectAllocator=cRecyclableObjectAllocator<TLifeCycleObject> >
class cVirtualLifeCycleRecyclableSharedManager : public bcVirtualLifeCycleRecyclableManager<TLifeCycleObject,TRecyclableObjectAllocator>
{
public:
	typedef TLifeCycleObject tLifeCycleObject;

	cnRTL_CONSTEXPR_FUNC cVirtualLifeCycleRecyclableSharedManager()noexcept(true)
		: fRefCount(0){}

	static cVirtualLifeCycleRecyclableSharedManager* GetSharedManager(void){
		return cnVar::StaticInitializedSinglton<cVirtualLifeCycleRecyclableSharedManager>();
	}

	static void ManageShared(tLifeCycleObject *Object)noexcept(true){
		auto Manager=GetSharedManager();
		Manager->Manage(Object);
	}

	void IncreaseReference(void)noexcept(true){
		fRefCount.Free++;
	}
	void DecreaseReference(void)noexcept(true){
		if(fRefCount.Free--==1){
			this->DeleteObjects();
		}
	}

	virtual void Restore(tLifeCycleObject*)noexcept(true) override{
		rIncReference(this,'lcle');
	}

	virtual void Dispose(bcVirtualLifeCycle *Object)noexcept(true) override{
		bcVirtualLifeCycleRecyclableManager<TLifeCycleObject,TRecyclableObjectAllocator>::Dispose(Object);
		rDecReference(this,'lcle');
	}
private:
	cAtomicVar<uIntn> fRefCount;
};
//---------------------------------------------------------------------------
template< class TLifeCycleObject,class TRecyclableObjectAllocator=cRecyclableObjectAllocator<TLifeCycleObject> >
class cVirtualLifeCycleRecyclableManager : public iReference, public bcVirtualLifeCycleRecyclableManager<TLifeCycleObject,TRecyclableObjectAllocator>
{
public:
	typedef TLifeCycleObject tLifeCycleObject;

	cVirtualLifeCycleRecyclableManager()noexcept(true){}
	~cVirtualLifeCycleRecyclableManager()noexcept(true){
		this->DeleteObjects();
	}

	// iVirtualLifeCycleManager

	virtual void Restore(tLifeCycleObject*)noexcept(true) override{
		rIncReference(this,'lcle');
	}

	virtual void Dispose(bcVirtualLifeCycle *Object)noexcept(true) override{
		bcVirtualLifeCycleRecyclableManager<tLifeCycleObject,TRecyclableObjectAllocator>::Dispose(Object);
		rDecReference(this,'lcle');
	}
};
//---------------------------------------------------------------------------
class cDualReference : public bcVirtualLifeCycle
{
public:
	template<class T>
	struct cRefTokenOperator : cnVar::bcPointerRefTokenOperator<T*>
	{
		static void Acquire(T *Token)noexcept(true){	if(Token!=nullptr)	Token->fInnerReference.Inc();	}
		static void Release(T *Token)noexcept(true){	if(Token!=nullptr)	Token->fInnerReference.Dec();	}
	};
protected:
	class cInner : public iReference
	{
	public:
		cAtomicVar<uIntn> Ref=0;
		virtual void cnLib_FUNC IncreaseReference(void)noexcept(true) override;
		virtual void cnLib_FUNC DecreaseReference(void)noexcept(true) override;

		void Inc(void)noexcept(true);
		void Dec(void)noexcept(true);
	}fInnerReference;

	cDualReference()noexcept(true){}

	void InnerIncReference(uInt32 Tag)noexcept(true){
		cnRTL_DEBUG_LOG_REFERENCE_INC(this,Tag);
		return fInnerReference.Inc();
	}
	void InnerDecReference(uInt32 Tag)noexcept(true){
		cnRTL_DEBUG_LOG_REFERENCE_DEC(this,Tag);
		return fInnerReference.Dec();
	}

	// override virtual life cycle start to increase inner reference
	void VirtualStarted(void)noexcept(true){
		cnLib_ASSERT(fInnerReference.Ref==0);
		InnerIncReference('self');
	}
	// override virtual life cycle stop to decrease inner reference
	void VirtualStopped(void)noexcept(true){
		InnerDecReference('self');
	}
};
//---------------------------------------------------------------------------
template<class T>
using rInnerPtr = cnVar::cPtrReference< cDualReference::cRefTokenOperator<T> >;
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
