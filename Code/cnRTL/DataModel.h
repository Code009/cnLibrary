/*- cnRTL - DataModel -----------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2019-12-27                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnRTL_DataModel_H__
#define __cnLibrary_cnRTL_DataModel_H__
/*-------------------------------------------------------------------------*/
#include <cnRTL/cnRTLHeader.h>
#include <cnRTL/FunctionSet.h>
#include <cnRTL/ObjectReference.h>
#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnRTL{
//---------------------------------------------------------------------------
#if 0

class idbSystem : public iReference
{
public:
	virtual iSharedMutex* GetDBLock(void)=0;
	virtual iDispatch* GetDBDispatch(void)=0;
};
//---------------------------------------------------------------------------
class idbIdentity : public iReference
{
public:
	virtual void RegisterNotifyChange(iProcedure *Procedure)=0;
	virtual void UnregisterNotifyChange(iProcedure *Procedure)=0;
	virtual void NotifyChange(void)=0;
protected:
};
//---------------------------------------------------------------------------
template<class T>
struct cDBObjectRefTokenOperator
{
	struct TRefToken{
		T *Pointer;
		iReference *Reference;
	};
	typedef const T *TPtr;
	static const T* Pointer(const TRefToken &RefToken){	return RefToken.Pointer;	}

	static TRefToken TokenNull(void){
		TRefToken Token={nullptr,nullptr};
		return Token;
	}

	template<class TSrc>
	static typename cnVar::TTypeConditional<TRefToken,
		cnVar::TIsAssignable<iReference*,TSrc&&>::Value && cnVar::TIsAssignable<T*&,TSrc&&>::Value
	>::Type TokenFrom(TSrc&& Src)
	{
		TRefToken Token={Src,Src};
		return Token;
	}

	static void Acquire(const TRefToken &RefToken){	if(RefToken.Reference!=nullptr)	RefToken.Reference->IncreaseReference();	}
	static void Release(const TRefToken &RefToken){	if(RefToken.Reference!=nullptr)	RefToken.Reference->DecreaseReference();	}
};
//---------------------------------------------------------------------------
template<class T>
using dbPtr=cnVar::cPtrReference< cDBObjectRefTokenOperator<T> >;
//---------------------------------------------------------------------------
template<class TDBS,class TInterface>
class dbsID : public TInterface
{
public:
	dbsID(TDBS *DBS)
		: fRefCount(0),fDBS(DBS)//,fChangeCallback(nullptr)
	{
	}
	~dbsID(){
	}

	virtual	void cnLib_FUNC IncReference(void)override{
		if(fRefCount.Free++==0){
			fDBS->IncReference();
		}
	}
	virtual	void cnLib_FUNC DecReference(void)override{
		if(--fRefCount.Free==0){
			auto DBS=fDBS;
			IDOnFree();
			// this may be deleted by IDOnFree
			DBS->DecReference();
		}
	}

	virtual void RegisterNotifyChange(iProcedure *Procedure)override{
		auto AutoLock=TakeLock(fDBS->GetDBLock());
		fChangeCallback.Insert(Procedure);
	}
	virtual void UnregisterNotifyChange(iProcedure *Procedure)override{
		auto AutoLock=TakeLock(fDBS->GetDBLock());
		fChangeCallback.Remove(Procedure);
	}

	virtual void NotifyChange(void)override{
		auto Dispatch=fDBS->GetDBDispatch();

		if(Dispatch==nullptr){
			CallNotify();
		}
		else{
			Dispatch->Execute(this,&fCallNotityProcedure);
		}
	}
protected:
	TDBS *fDBS;
	cAtomicVar<uIntn> fRefCount;

	cMTFunctionSet<void (void)> fChangeCallback;

	class CallNotifyProcedure : public iProcedure
	{
	public:
		virtual void cnLib_FUNC Execute(void)override{
			auto Host=cnMemory::GetObjectFromMemberPointer(this,&dbsID::fCallNotityProcedure);
			Host->CallNotify();
		}
	}fCallNotityProcedure;
	void CallNotify(void){

		cSeqList<iProcedure*> CallbackList;
		{
			fChangeCallback();
			//auto AutoLock=TakeSharedLock(fDBS->GetDBLock());
			//CallbackList=fChangeCallback.RefFunctionList();
		}
		for(auto Func : CallbackList){
			Func->Execute();
		}
	}

	virtual void IDOnFree(void){}
};
//---------------------------------------------------------------------------
template<class TDBObject>
class cDBSharedObject
{
public:
	cDBSharedObject(){
		fObjectLock=cnSystem::CreateMutexLock();
	}
	~cDBSharedObject(){
		Clear();
	}
	iReference *Reference;
	/*
	template<class TFunctor>
	dbPtr<TDBObject> Replace(TFunctor&& SetupFunction){
		dbPtr<TDBObject> SharedObject;
		cSharedObject *ClearedObject=nullptr;
		{
			auto AutoLock=ScopeResLock(fObjectLock);

			// swap out previous object
			if(fSharedObject!=nullptr){
				ClearedObject=fSharedObject;
			}
			// create new object
			fSharedObject=new cSharedObject(this);
			SetupFunction(fSharedObject);

			// get return object
			SharedObject=fSharedObject;
		}
		// clear previous object
		if(ClearedObject!=nullptr){
			if(ClearedObject->CheckReference()==false){
				// no more reference
				delete fSharedObject;
			}
		}
		return SharedObject;
	}
	*/
	template<class TFunctor>
	dbPtr<TDBObject> Query(TFunctor&& SetupFunction){
		dbPtr<TDBObject> SharedObject;

		{
			auto AutoLock=TakeLock(fObjectLock);
			if(fSharedObject==nullptr){
				fSharedObject=new cSharedObject(this);
				SetupFunction(fSharedObject);
			}
			SharedObject=fSharedObject;
		}

		return SharedObject;
	}
	void Clear(void){
		cSharedObject *ClearedObject=nullptr;
		{
			auto AutoLock=TakeLock(fObjectLock);
			if(fSharedObject!=nullptr){
				ClearedObject=fSharedObject;
				fSharedObject=nullptr;
			}
		}
		if(ClearedObject!=nullptr){
			ClearedObject->Discard();
		}
	}
private:
	class cSharedObject : public iReference, public TDBObject
	{
	public:
		cSharedObject(cDBSharedObject *Owner)
			: fRefCount(0),fOwner(Owner)
		{
		}

		~cSharedObject(){
		}

		void Discard(void){
			if(fRefCount.Free++!=0){
				// still has reference
				// release as usual, and let the common decreasement routine delete this object
				if(--fRefCount.Free==0){
					fOwner->ObjectReleased(this);
				}
			}
			else{
				// has no more reference
				//	in this situation this object's reference cannot be increased since there is no any referencer out there
				delete this;
			}
		}

		virtual	void cnLib_FUNC IncreaseReference(void)noexcept override{
			if(fRefCount.Free++==0){
				fOwner->ObjectAcquired();
			}
		}
		virtual	void cnLib_FUNC DecreaseReference(void)noexcept override{
			if(--fRefCount.Free==0){
				fOwner->ObjectReleased(this);
			}
		}
	private:
		cDBSharedObject *fOwner;
		cAtomicVar<uIntn> fRefCount;
	};
	rPtr<iMutex> fObjectLock;
	cSharedObject *fSharedObject;

	void ObjectAcquired(void){
		Reference->IncReference();
	}
	void ObjectReleased(cSharedObject *Object){

		{
			auto AutoLock=TakeLock(fObjectLock);
			if(fSharedObject!=Object){
				delete Object;
			}
		}

		Reference->DecReference();
	}
};

#endif // 0

//---------------------------------------------------------------------------
/*
//---------------------------------------------------------------------------
template<class T,class TAtomicOperator,class TAllocator>
class impDBIDCreate_Reference : public T, public TAllocator
{
public:
#ifndef cnLibrary_CPPEXCLUDE_INHERIT_CONSTRUCTOR
	using T::T;
#else
	template<class...VT>
	impDBIDCreate_Reference(VT&&...Args):T(static_cast<VT&&>(Args)...){}
#endif // !cnLibrary_CPPEXCLUDE_INHERIT_CONSTRUCTOR

	virtual void cnLib_FUNC IncReference(void)override{
		cnLib_ASSERT(fRefCount!=0);
		rIncReference(this->fDBS,'dbid');
		fRefCount.Free++;
	}
	virtual void cnLib_FUNC DecReference(void)override{
		cnLib_ASSERT(fRefCount!=0);
		rDecReference(this->fDBS,'dbid');
		if(fRefCount.Free--==1){
			delete this;
		}
	}
private:
	cnAsync::cAtomicVariable<TAtomicOperator> fRefCount=1;
	~impDBIDCreate_Reference(){}
};
//---------------------------------------------------------------------------
template<class T,class TAtomicOperator,class TAllocator>
class impDBIDCreate_VirtualReference : public T, public TAllocator
{
public:
#ifndef cnLibrary_CPPEXCLUDE_INHERIT_CONSTRUCTOR
	using T::T;
#else
	template<class...VT>
	impDBIDCreate_VirtualReference(VT&&...Args):T(static_cast<VT&&>(Args)...){}
#endif // !cnLibrary_CPPEXCLUDE_INHERIT_CONSTRUCTOR
	virtual void cnLib_FUNC IncReference(void)override{
		cnLib_ASSERT(fRefCount!=0);
		rIncReference(this->fDBS,'dbid');
		fRefCount.Free++;
	}
	virtual void cnLib_FUNC DecReference(void)override{
		cnLib_ASSERT(fRefCount!=0);
		rDecReference(this->fDBS,'dbid');
		if(fRefCount.Free--==1){
			this->VirtualStopped();
		}
	}
private:
	cnAsync::cAtomicVariable<TAtomicOperator> fRefCount=1;
	virtual void VirtualDelete(void)override{
		delete this;
	}
	~impDBIDCreate_VirtualReference(){}

};
//---------------------------------------------------------------------------
template<class T,class TAtomicOperator,class TAllocator>
class impDBIDCreate_SelectClass : T
{
private:
	static cnVar::TTypeDef< T	>																ImpTypeTest(...);
	static cnVar::TTypeDef< impDBIDCreate_Reference		<T,TAtomicOperator,TAllocator> >	ImpTypeTest(iReference*);
	static cnVar::TTypeDef< impDBIDCreate_VirtualReference	<T,TAtomicOperator,TAllocator> >	ImpTypeTest(bcVirtualLifeCycle*);
public:
	typedef decltype(ImpTypeTest(cnVar::DeclVal<T*>())) TypeDef;
};
template<class T,class TAtomicOperator,class TAllocator>
struct TDBIDCreateImplementClass
	: impDBIDCreate_SelectClass<T,TAtomicOperator,TAllocator>::TypeDef {};
//---------------------------------------------------------------------------
template<class T>
rPtr<T> dbidCreate(void)
{
	return rPtr<T>::Take(new typename TRefCreateImplementClass<T>::Type);
}
template<class T,class...TArgs>
rPtr<T> dbidCreate(TArgs&&...Args)
{
	return rPtr<T>::Take(new typename TRefCreateImplementClass<T>::Type(cnVar::Forward<TArgs>(Args)...));
}
*/
//---------------------------------------------------------------------------
}   // namespace cnRTL
//---------------------------------------------------------------------------
}   // namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif

