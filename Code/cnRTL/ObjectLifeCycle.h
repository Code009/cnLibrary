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
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnRTL{
//---------------------------------------------------------------------------
//TLifeCycleInstance
//{
//	// LifeCycleStart
//	//	call the function to start the object's life cycle
//	static void LifeCycleStart(TLifeCycleObject *Object)noexcept;
//	// LifeCycleStop
//	//	call the function to stop the object's life cycle
//	static void LifeCycleStop(TLifeCycleObject *Object)noexcept;
//};
//---------------------------------------------------------------------------
struct cCPPLifeCycleInstance
{
	template<class TLifeCycleObject>	static void LifeCycleStart(TLifeCycleObject*)noexcept(true){}
	template<class TLifeCycleObject>	static void LifeCycleStop(TLifeCycleObject *Object)noexcept(true){	delete Object;	}
};
//---------------------------------------------------------------------------
template<class TLifeCycleObject>
class cCPPLifeCycleManager
{
public:
	typedef TLifeCycleObject tLifeCycleObject;

	static void ManageGlobal(void*)noexcept(true){}
};
//---------------------------------------------------------------------------
class iLifeCycleManager
{
public:
	virtual void ObjectStart(void *Object)noexcept(true)=0;
	virtual void ObjectFinished(void *Object)noexcept(true)=0;
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
	//	default implementation is calling LifeCycleFinish to finalize the object
	void VirtualStopped(void)noexcept(true){	VirtualDelete();	}

	void VirtualDelete(void)noexcept(true);

private:
	iLifeCycleManager *LifeCycleManager=nullptr;
public:

	class bcManager : public iLifeCycleManager
	{
	public:
		static void NotifyObjectStart(bcVirtualLifeCycle *Object)noexcept(true);
		void Manage(bcVirtualLifeCycle *Object)noexcept(true);

	};

};
//---------------------------------------------------------------------------
struct cVirtualLifeCycleInstance
{
	template<class TLifeCycleObject>
	static void LifeCycleStart(TLifeCycleObject *Object)noexcept(true){
		bcVirtualLifeCycle::bcManager::NotifyObjectStart(Object);
		Object->VirtualStarted();
	}
	template<class TLifeCycleObject>
	static void LifeCycleStop(TLifeCycleObject *Object)noexcept(true){
		Object->VirtualStopped();
	}
};
//---------------------------------------------------------------------------
template<class TLifeCycleObject>
class cVirtualLifeCycleDefaultManager : public bcVirtualLifeCycle::bcManager
{
public:
	typedef TLifeCycleObject tLifeCycleObject;

	static void ManageGlobal(bcVirtualLifeCycle *Object)noexcept(true){
		auto Manager=cnVar::StaticInitializedSinglton< cVirtualLifeCycleDefaultManager<TLifeCycleObject> >();
		Manager->Manage(Object);
	}

	virtual void ObjectStart(void*)noexcept(true) override{}
	virtual void ObjectFinished(void *Object)noexcept(true) override{
		delete static_cast<TLifeCycleObject*>(static_cast<bcVirtualLifeCycle*>(Object));
	}
};
//---------------------------------------------------------------------------
class cCPPLifeCycleRecyclableInstance
{
private:
	cCPPLifeCycleRecyclableInstance *LifeCycleNext;
	iLifeCycleManager *LifeCycleManager;

	struct cSingleLinkItemOperator
	{
		typedef cCPPLifeCycleRecyclableInstance tItem;

		typedef cnMemory::cPlainAlignedData<sizeof(void*),cCPPLifeCycleRecyclableInstance> tLinkInstance;
	
		static tItem* LinkNode(tLinkInstance &Link)noexcept(true){
			return reinterpret_cast<tItem*>(&Link);
		}
		static const tItem* LinkNode(const tLinkInstance &Link)noexcept(true){
			return reinterpret_cast<const tItem*>(&Link);
		}

		static tItem* GetNext(const tItem *Item)noexcept(true){
			return static_cast<tItem*>(Item->LifeCycleNext);
		}
		static void SetNext(tItem *Item,tItem *Next)noexcept(true){
			Item->LifeCycleNext=Next;
		}
	};

public:
	template<class TLifeCycleObject>
	static void LifeCycleStart(TLifeCycleObject *Object)noexcept(true){
		cnLib_ASSERT(Object->LifeCycleManager!=nullptr);
		Object->LifeCycleManager->ObjectStart(Object);
	}
	template<class TLifeCycleObject>
	static void LifeCycleStop(TLifeCycleObject *Object)noexcept(true){
		Object->LifeCycleManager->ObjectFinished(Object);
	}

	template<class TLifeCycleObject>
	class bcManager : public iLifeCycleManager
	{
	public:

		static_assert(cnVar::TIsConvertible<TLifeCycleObject*,cCPPLifeCycleRecyclableInstance*>::Value,"Incorrect Instance");

		void Manage(TLifeCycleObject *Object)noexcept(true){	Object->LifeCycleManager=this;	}

		TLifeCycleObject* Fetch(void)noexcept(true){
			return static_cast<TLifeCycleObject*>(fRecycleStack.Pop());
		}

		TLifeCycleObject* Query(void)noexcept(cnLib_NOEXCEPTEXPR(new TLifeCycleObject)){
			auto LCObject=Fetch();
			if(LCObject==nullptr){
				LCObject=new TLifeCycleObject;
				Manage(LCObject);
			}
			return LCObject;
		}
	protected:
		static TLifeCycleObject* NextObject(TLifeCycleObject *Object){
			return static_cast<TLifeCycleObject*>(Object->cCPPLifeCycleRecyclableInstance::LifeCycleNext);
		}

		void RecycleObject(void *Object)noexcept(true){
			auto LCObject=static_cast<TLifeCycleObject*>(Object);
			fRecycleStack.Push(LCObject);
		}

		TLifeCycleObject* FetchObjects(void)noexcept(true){
			return static_cast<TLifeCycleObject*>(fRecycleStack.Swap(nullptr));
		}
	
		void DeleteObjects(void)noexcept(true){
			auto ItemsToDelete=static_cast<TLifeCycleObject*>(fRecycleStack.Swap(nullptr));
			while(ItemsToDelete!=nullptr){
				auto DeleteObject=ItemsToDelete;
				ItemsToDelete=NextObject(ItemsToDelete);

				delete DeleteObject;
			}
		}
	private:
		cnAsync::cAtomicStack<cSingleLinkItemOperator> fRecycleStack;
	};
};
//---------------------------------------------------------------------------
template<class TLifeCycleObject>
class cCPPLifeCycleSharedRecyclableManager : public cCPPLifeCycleRecyclableInstance::bcManager<TLifeCycleObject>
{
public:
	cnRTL_CONSTEXPR_FUNC cCPPLifeCycleSharedRecyclableManager()noexcept(true)
		: fRefCount(0){}
	
	static void LifeCycleManageDefault(TLifeCycleObject *Object)noexcept(true){
		auto Manager=GetSharedManager();
		Manager->Manage(Object);
	}
	
	static cCPPLifeCycleSharedRecyclableManager* GetSharedManager(void){
		return cnVar::StaticInitializedSinglton<cCPPLifeCycleSharedRecyclableManager>();
	}

	void IncreaseReference(void)noexcept(true){
		fRefCount.Free++;
	}
	void DecreaseReference(void)noexcept(true){
		if(fRefCount.Free--==1){
			this->DeleteObjects();
		}
	}

	virtual void ObjectStart(void*)noexcept(true) override{
		rIncReference(this,'lcle');
	}

	virtual void ObjectFinished(void *Object)noexcept(true) override{
		this->RecycleObject(Object);
		rDecReference(this,'lcle');
	}

private:
	cAtomicVar<uIntn> fRefCount;
};
//---------------------------------------------------------------------------
template<class TLifeCycleObject>
class cCPPLifeCycleRecyclableManager : public iReference, public cCPPLifeCycleRecyclableInstance::bcManager<TLifeCycleObject>
{
public:

	cCPPLifeCycleRecyclableManager()noexcept(true){}
	~cCPPLifeCycleRecyclableManager()noexcept(true){
		this->DeleteObjects();
	}


	virtual void ObjectStart(void*)noexcept(true) override{
		rIncReference(this,'lcle');
	}

	virtual void ObjectFinished(void *Object)noexcept(true) override{
		this->RecycleObject(Object);
		rDecReference(this,'lcle');
	}
};
//---------------------------------------------------------------------------
class cVirtualLifeCycleRecyclableInstance
{
private:
	cVirtualLifeCycleRecyclableInstance *LifeCycleNext;
	
	struct cSingleLinkItemOperator
	{
		typedef cVirtualLifeCycleRecyclableInstance tItem;

		typedef cnMemory::cPlainAlignedData<sizeof(void*),cVirtualLifeCycleRecyclableInstance> tLinkInstance;
	
		static tItem* LinkNode(tLinkInstance &Link)noexcept(true){
			return reinterpret_cast<tItem*>(&Link);
		}
		static const tItem* LinkNode(const tLinkInstance &Link)noexcept(true){
			return reinterpret_cast<const tItem*>(&Link);
		}

		static tItem* GetNext(const tItem *Item)noexcept(true){
			return static_cast<tItem*>(Item->LifeCycleNext);
		}
		static void SetNext(tItem *Item,tItem *Next)noexcept(true){
			Item->LifeCycleNext=Next;
		}
	};

public:
	
	template<class TLifeCycleObject>
	static void LifeCycleStart(TLifeCycleObject *Object)noexcept(true){
		bcVirtualLifeCycle::bcManager::NotifyObjectStart(Object);
		Object->VirtualStarted();
	}
	template<class TLifeCycleObject>
	static void LifeCycleStop(TLifeCycleObject *Object)noexcept(true){
		Object->VirtualStopped();
	}

	template<class TLifeCycleObject>
	class bcManager : public bcVirtualLifeCycle::bcManager
	{
	public:
		typedef TLifeCycleObject tLifeCycleObject;

		TLifeCycleObject* Fetch(void)noexcept(true){
			return static_cast<TLifeCycleObject*>(fRecycleStack.Pop());
		}

		TLifeCycleObject* Query(void)noexcept(cnLib_NOEXCEPTEXPR(new TLifeCycleObject)){
			auto LCObject=Fetch();
			if(LCObject==nullptr){
				LCObject=new TLifeCycleObject;
				bcVirtualLifeCycle::bcManager::Manage(LCObject);
			}
			return LCObject;
		}
	protected:
		static TLifeCycleObject* NextObject(TLifeCycleObject *Object){
			return static_cast<TLifeCycleObject*>(Object->cVirtualLifeCycleRecyclableInstance::LifeCycleNext);
		}

		void RecycleObject(void *Object)noexcept(true){
			auto LCObject=static_cast<TLifeCycleObject*>(Object);
			fRecycleStack.Push(LCObject);
		}

		TLifeCycleObject* FetchObjects(void)noexcept(true){
			return static_cast<TLifeCycleObject*>(fRecycleStack.Swap(nullptr));
		}
	
		void DeleteObjects(void)noexcept(true){
			auto ItemsToDelete=static_cast<TLifeCycleObject*>(fRecycleStack.Swap(nullptr));
			while(ItemsToDelete!=nullptr){
				auto DeleteObject=ItemsToDelete;
				ItemsToDelete=NextObject(ItemsToDelete);

				delete DeleteObject;
			}
		}
	private:
		cnAsync::cAtomicStack<cSingleLinkItemOperator> fRecycleStack;
	};

};
//---------------------------------------------------------------------------
template<class TLifeCycleObject>
class cVirtualLifeCycleSharedRecyclableManager : public cVirtualLifeCycleRecyclableInstance::bcManager<TLifeCycleObject>
{
public:
	cnRTL_CONSTEXPR_FUNC cVirtualLifeCycleSharedRecyclableManager()noexcept(true)
		: fRefCount(0){}

	
	static void LifeCycleManageDefault(TLifeCycleObject *Object)noexcept(true){
		auto Manager=GetSharedManager();
		Manager->Manage(Object);
	}
	
	static cVirtualLifeCycleSharedRecyclableManager* GetSharedManager(void){
		return cnVar::StaticInitializedSinglton<cVirtualLifeCycleSharedRecyclableManager>();
	}

	void IncreaseReference(void)noexcept(true){
		fRefCount.Free++;
	}
	void DecreaseReference(void)noexcept(true){
		if(fRefCount.Free--==1){
			this->DeleteObjects();
		}
	}

	virtual void ObjectStart(void*)noexcept(true) override{
		rIncReference(this,'lcle');
	}

	virtual void ObjectFinished(void *Object)noexcept(true) override{
		this->RecycleObject(Object);
		rDecReference(this,'lcle');
	}

private:
	cAtomicVar<uIntn> fRefCount;
};
//---------------------------------------------------------------------------
template<class TLifeCycleObject>
class cVirtualLifeCycleRecyclableManager : public iReference, public cVirtualLifeCycleRecyclableInstance::bcManager<TLifeCycleObject>
{
public:

	~cVirtualLifeCycleRecyclableManager()noexcept(true){
		this->DeleteObjects();
	}

	// iVirtualLifeCycleManager

	virtual void ObjectStart(void*)noexcept(true) override{
		rIncReference(this,'lcle');
	}

	virtual void ObjectFinished(void *Object)noexcept(true) override{
		this->RecycleObject(Object);
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
