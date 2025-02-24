/*- cnRTL - Thread Synchronization ----------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2009-03-06                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnRTL_ThreadSynchronization_H__
#define __cnLibrary_cnRTL_ThreadSynchronization_H__
/*-------------------------------------------------------------------------*/
#include <cnRTL/cnRTLHeader.h>
#include <cnRTL/RuntimeFunction.h>
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
class cWaitObject
{
public:
#ifdef cnLib_DEBUG
	~cWaitObject()noexcept(true){
		cnLib_ASSERT(fRefCount==0);
	}
#endif // cnLib_DEBUG


	void Acquire(void)noexcept(true);
	void Release(void)noexcept(true);

	bool Check(void)noexcept(true);
	void Wait(void)noexcept(true);
	bool WaitFor(uInt64 Duration)noexcept(true);
	//bool WaitUntil(uInt64 SystemTime)noexcept(true);
private:
	cAtomicVar<uIntn> fRefCount=0;
	cThreadSingleNotification fNotification;
};
//---------------------------------------------------------------------------
class cWaitObjectRegistration : public cWaitObject
{
public:
#ifdef cnLib_DEBUG
	void Acquire(void *Object)noexcept(true){
		Log(Object,true);
		return cWaitObject::Acquire();
	}
	void Release(void *Object)noexcept(true){
		Log(Object,false);
		return cWaitObject::Release();
	}
private:

	struct cItem
	{
		cItem *Next;
		void *Object;
		uInt32 Tag;
		bool Inc;
	};
	cAtomicQueueSO<cItem> fItemQueue;
	cAtomicStack<cItem> fRecycleStack;
	cnRTL::cExclusiveFlag fExclusiveFlag;

	void Log(void *Object,bool Inc)noexcept(true){
		auto Item=fRecycleStack.Pop();
		if(Item==nullptr){
			Item=new cItem;
		}
		Item->Object=Object;
		Item->Inc=Inc;
		fItemQueue.Enqueue(Item);
		NotifyProcess();
	}

	void NotifyProcess(void)noexcept(true){
		if(fExclusiveFlag.Acquire()==false)
			return;

		fExclusiveFlag.Continue();
		do{
			fExclusiveFlag.Continue();

			Process();

		}while(fExclusiveFlag.Release()==false);
	}
	
	cSeqMap<void*,uIntn> fObjectMap;

	void Process(void)noexcept(true){

		auto Items=fItemQueue.DequeueAll();
		while(Items!=nullptr){
			auto CurItem=Items;
			Items=Items->Next;
			if(CurItem->Inc){
				fObjectMap[CurItem->Object]++;
			}
			else{
				auto ObjectPair=fObjectMap.GetPair(CurItem->Object);
				if(ObjectPair==nullptr){
					// error! object not found
					cnSystem::AssertionMessage("object not found");
				}
				else{
					if(--ObjectPair->Value==0){
						fObjectMap.RemovePair(ObjectPair);
					}
				}
			}
			fRecycleStack.Push(CurItem);
		}
	}

#else
	void Acquire(void *)noexcept(true){	return cWaitObject::Acquire();	}
	void Release(void *)noexcept(true){	return cWaitObject::Release();	}
#endif
};
//---------------------------------------------------------------------------
class cWaitReference : public iReference, public cWaitObject
{
public:
	cWaitReference()noexcept(true);
	~cWaitReference()noexcept(true);
	virtual void cnLib_FUNC IncreaseReference(void)noexcept(true) override;
	virtual void cnLib_FUNC DecreaseReference(void)noexcept(true) override;
};
//---------------------------------------------------------------------------
class cLocalSingleThreadNotification
{
public:
	cLocalSingleThreadNotification()noexcept(true);
	~cLocalSingleThreadNotification()noexcept(true);

	void Reset(void)noexcept(true);
	void Wait(void)noexcept(true);
	bool WaitFor(ufInt64 Duration)noexcept(true);
	void Notify(void)noexcept(true);
protected:
	TKRuntime::SystemThread::tSingleNotification fNotification;
};
//---------------------------------------------------------------------------
class cAffixedVariableSet
{
public:
	cAffixedVariableSet()noexcept(true);
	~cAffixedVariableSet()noexcept(true);
	
	rPtr<iVariable> QueryAffixedVariable(const void *Token)noexcept(true);

private:

	class cVariable : public iVariable
	{
	public:

		cVariable *Parent;
		cVariable *Child[2];
		ufInt8 Branch;
		ufInt8 Color;


		cVariable(cAffixedVariableSet *Owner,const void *Token)noexcept(true);
		~cVariable()noexcept(true);

		bool MakeReference(void)noexcept(true);
		virtual void cnLib_FUNC IncreaseReference(void)noexcept(true) override;
		virtual void cnLib_FUNC DecreaseReference(void)noexcept(true) override;

		virtual void cnLib_FUNC Clear(void)noexcept(true)override;
		virtual void* cnLib_FUNC Get(void)noexcept(true)override;
		virtual void cnLib_FUNC Set(iReference *Reference,void *Value)noexcept(true)override;

		struct cItemOrderOperator
		{
			static sfInt8 Compare(const cVariable &Item,const cVariable &Value)noexcept(true);
			static sfInt8 Compare(const cVariable &Item,const void *Token)noexcept(true);

		};
	protected:
		cAffixedVariableSet *fOwner;
		const void *fToken;
		rPtr<iReference> fReference;
		void *fValue;
		cAtomicVar<uIntn> fRefCount;
	};

	rPtr<iMutex> fSetMutex;
	cnRTL::cLinkItemSet<cVariable,cVariable::cItemOrderOperator> fSet;

	void VariableReleased(cVariable *Variable)noexcept(true);
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

