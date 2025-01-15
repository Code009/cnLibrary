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

// 			CPP						Disposable					VirutalLifeCycle
//new-------|				new---------|					new---------|
//			|							|	<-----Reactive				|	<-----Reactive
//			|							|			|					|			|
//Start--------------		Start---------------	|		Start---------------	|
//			|							|			|				VirtualStart	|
//			|							|			|					|			|
//Stop---------------		Stop----------------	|		Stop----------------	|
//			|							|			|				VirtualStop		|
//			|							|			|					|			|
//			|						  Dispose		|				  Dispose		|
//			|							|-------->Recycle				|-------->Recycle
//			|							|								|
//		  delete					  delete						  delete

//---------------------------------------------------------------------------

//TLifeCycleActivation
//{
//	// Start
//	//	call the function to start the object's life cycle
//	template<TLifeCycleObject>
//	static void Start(TLifeCycleObject *Object)noexcept;
//	// Stop
//	//	call the function to stop the object's life cycle
//	template<TLifeCycleObject>
//	static void Stop(TLifeCycleObject *Object)noexcept;
//};
//---------------------------------------------------------------------------
struct cCPPLifeCycleActivation
{
	template<class TLifeCycleObject>
	static void Start(TLifeCycleObject*)noexcept(true){}

	template<class TLifeCycleObject>
	static void Stop(TLifeCycleObject *Object)noexcept(true){	delete Object;	}
};
//---------------------------------------------------------------------------
template<class T>
class cCPPLifeCycleSharedManager
{
public:
	typedef T tLifeCycleObject;

	static void ManageShared(tLifeCycleObject*)noexcept(true){}
};
//---------------------------------------------------------------------------
class bcDisposable
{
protected:
	void Dispose(void)noexcept(true);

public:
	struct cLifeCycleActivation
	{
		template<class TLifeCycleObject>
		static void Start(TLifeCycleObject*)noexcept(true){}
		template<class TLifeCycleObject>
		static void Stop(TLifeCycleObject *Object)noexcept(true){	Object->bcDisposable::Dispose();	}
	};

	class iDisposal
	{
	public:
		void Bind(bcDisposable *Object)noexcept(true);
		virtual void Dispose(bcDisposable *Object)noexcept(true)=0;
	};

	template<class T>
	class cDefaultDisposal : public iDisposal
	{
	public:
		virtual void Dispose(bcDisposable *Object)noexcept(true) override{
			delete static_cast<T*>(Object);
		}
	};

private:
	iDisposal *LifeCycleDisposal=nullptr;
};
//---------------------------------------------------------------------------
typedef bcDisposable::cLifeCycleActivation cDisposableLifeCycleActivation;
//---------------------------------------------------------------------------
template<class T>
class cDisposableLifeCycleSharedManager
{
public:
	typedef T tLifeCycleObject;

	static_assert(cnVar::TIsConvertible<T*,bcDisposable*>::Value,"Incorrect Type");

	static void ManageShared(tLifeCycleObject *Object)noexcept(true){
		auto Disposal=cnVar::StaticInitializedSinglton< bcDisposable::template cDefaultDisposal<T> >();
		Disposal->Bind(Object);
	}
};
//---------------------------------------------------------------------------
class bcVirtualLifeCycle : public bcDisposable
{
protected:

	// VirtualStarted
	//	will be called by final implementation when the life cycle of the object ended.
	//	default implementation is nop
	void VirtualStarted(void)noexcept(true){}
	// VirtualStopped
	//	will be called by final implementation when the life cycle of the object ended.
	//	default implementation is calling Dispose to finalize the object
	void VirtualStopped(void)noexcept(true){	Dispose();	}

public:

	struct cLifeCycleActivation
	{
		template<class TLifeCycleObject>
		static void Start(TLifeCycleObject *Object)noexcept(true){	Object->VirtualStarted();	}
		template<class TLifeCycleObject>
		static void Stop(TLifeCycleObject *Object)noexcept(true){	Object->VirtualStopped();	}
	};
};
//---------------------------------------------------------------------------
typedef bcVirtualLifeCycle::cLifeCycleActivation cVirtualLifeCycleActivation;
//---------------------------------------------------------------------------
class bcRecyclable
{
private:
	template< class TLifeCycleObject,class TRecyclableObjectAllocator> friend class bcRecyclableLifeCycleManager;
	bcRecyclable *LifeCycleManagerNext;
};
//---------------------------------------------------------------------------
template<class T>
struct cRecyclableObjectAllocator
{
	static T* New(void)noexcept(noexcept(new T)){	return new T;	}
	static void Delete(T *p)noexcept(noexcept(delete p)){	delete p;	}
};
//---------------------------------------------------------------------------
template< class TLifeCycleObject,class TRecyclableObjectAllocator=cRecyclableObjectAllocator<TLifeCycleObject> >
class bcRecyclableLifeCycleManager : public bcDisposable::iDisposal, public TRecyclableObjectAllocator
{
public:
	typedef TLifeCycleObject tLifeCycleObject;

	static_assert(cnVar::TIsConvertible<tLifeCycleObject*,bcDisposable*>::Value,"Incorrect Instance");
	static_assert(cnVar::TIsConvertible<tLifeCycleObject*,bcRecyclable*>::Value,"Incorrect Instance");

	void Manage(tLifeCycleObject *Object)noexcept(true){
		Bind(Object);
	}

	virtual void Dispose(bcDisposable *Object)noexcept(true) override{
		fRecycleStack.Push(static_cast<tLifeCycleObject*>(Object));
	}

	struct cSingleLinkItemOperator
	{
		typedef tLifeCycleObject tItem;

		typedef bcRecyclable tLinkInstance;

		static tLifeCycleObject* LinkNode(tLinkInstance &Link)noexcept(true){
			return static_cast<tLifeCycleObject*>(&Link);
		}
		static const tLifeCycleObject* LinkNode(const tLinkInstance &Link)noexcept(true){
			return reinterpret_cast<const tLifeCycleObject*>(&Link);
		}

		static tLifeCycleObject* GetNext(const tLifeCycleObject *Item)noexcept(true){
			return static_cast<tLifeCycleObject*>(Item->bcRecyclable::LifeCycleManagerNext);
		}
		static void SetNext(tLifeCycleObject *Item,tLifeCycleObject *Next)noexcept(true){
			Item->bcRecyclable::LifeCycleManagerNext=Next;
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

	tLifeCycleObject* Query(void)noexcept(noexcept(TRecyclableObjectAllocator::New())){
		auto LCObject=fRecycleStack.Pop();
		if(LCObject==nullptr){
			LCObject=TRecyclableObjectAllocator::New();
			Manage(LCObject);
		}
		Restore(LCObject);
		return LCObject;
	}

	virtual void Restore(tLifeCycleObject*)noexcept(true){}

protected:

	tLifeCycleObject* FetchObjects(void)noexcept(true){
		return fRecycleStack.Swap(nullptr);
	}

	void DeleteObjects(void)noexcept(noexcept(TRecyclableObjectAllocator::Delete(cnVar::DeclVal<tLifeCycleObject*>()))){
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
class cRecyclableLifeCycleSharedManager : public bcRecyclableLifeCycleManager<TLifeCycleObject,TRecyclableObjectAllocator>
{
public:
	typedef TLifeCycleObject tLifeCycleObject;

	cnRTL_CONSTEXPR_FUNC cRecyclableLifeCycleSharedManager()noexcept(true)
		: fRefCount(0){}

	static cRecyclableLifeCycleSharedManager* GetSharedManager(void)noexcept(true){
		return cnVar::StaticInitializedSinglton<cRecyclableLifeCycleSharedManager>();
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

	virtual void Dispose(bcDisposable *Object)noexcept(true) override{
		bcRecyclableLifeCycleManager<TLifeCycleObject,TRecyclableObjectAllocator>::Dispose(Object);
		rDecReference(this,'lcle');
	}
private:
	cAtomicVar<uIntn> fRefCount;
};
//---------------------------------------------------------------------------
template< class TLifeCycleObject,class TRecyclableObjectAllocator=cRecyclableObjectAllocator<TLifeCycleObject> >
class cRecyclableLifeCycleManager : public iReference, public bcRecyclableLifeCycleManager<TLifeCycleObject,TRecyclableObjectAllocator>
{
public:
	typedef TLifeCycleObject tLifeCycleObject;

	cRecyclableLifeCycleManager()noexcept(true){}
	~cRecyclableLifeCycleManager()noexcept(true){
		this->DeleteObjects();
	}


	virtual void Restore(TLifeCycleObject*)noexcept(true) override{
		rIncReference(this,'lcle');
	}

	virtual void Dispose(bcDisposable *Object)noexcept(true) override{
		bcRecyclableLifeCycleManager<TLifeCycleObject,TRecyclableObjectAllocator>::Dispose(Object);
		rDecReference(this,'lcle');
	}
};
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
