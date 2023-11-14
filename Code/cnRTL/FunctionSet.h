/*- cnRTL - FunctionSet ---------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2006-01-17                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnRTL_FunctionSet_H__
#define __cnLibrary_cnRTL_FunctionSet_H__
/*-------------------------------------------------------------------------*/
#include <cnRTL/cnRTLHeader.h>
#include <cnRTL/ThreadSynchronization.h>
#include <cnRTL/ObjectReference.h>
#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnRTL{
//---------------------------------------------------------------------------
class iFunctionToken
{
protected:
	virtual ~iFunctionToken()=default;
};
//---------------------------------------------------------------------------
class cFunctionSetReference
{
public:
	cFunctionSetReference(){}
	~cFunctionSetReference(){
		if(OnDelete!=nullptr)
			OnDelete();
	}

	cFunctionSetReference& operator =(tNullptr){
		if(OnDelete!=nullptr){
			OnDelete();
			OnDelete=nullptr;
		}
		return *this;
	}

	template<class TFunctionSet>
	struct cAssigner
	{
		cFunctionSetReference *Owner;
		TFunctionSet *FuncSet;

		template<class TFunc>
		void operator =(TFunc&& Func){
			if(FuncSet==nullptr){
				if(Owner->OnDelete!=nullptr){
					Owner->OnDelete();
					Owner->OnDelete=nullptr;
				}
				return;
			}
			if(Owner->OnDelete!=nullptr){
				Owner->OnDelete();
			}
			iFunctionToken *Token=FuncSet->Insert(cnVar::Forward<TFunc>(Func));
			Owner->OnDelete=[Set=this->FuncSet,Token]{
				Set->Remove(Token);
			};
		}

	};
	template<class TFunctionSet>
	cAssigner<TFunctionSet> operator [](TFunctionSet &FunctionSet){
		return {this,&FunctionSet};
	}
	template<class TFunctionSet>
	cAssigner<TFunctionSet> operator [](TFunctionSet *FunctionSet){
		return {this,FunctionSet};
	}

private:
	cFunction<void (void),sizeof(void*)*3> OnDelete;
};
//---------------------------------------------------------------------------
template<class TFunction>	struct cFunctionInterfaceFunctor;
template<class TFunction>	struct cReferencedFunctionInterfaceFunctor;

template<class TRet,class...TArgs>
struct cFunctionInterfaceFunctor<TRet (TArgs...)>
{
	iFunction<TRet (TArgs...)> *Function;

	TRet operator () (TArgs...Args){
		return Function->Execute(static_cast<TArgs&&>(Args)...);
	}
};
template<class TRet,class...TArgs>
struct cReferencedFunctionInterfaceFunctor<TRet (TArgs...)>
{
	iFunction<TRet (TArgs...)> *Function;
	rPtr<iReference> Reference;

	TRet operator () (TArgs...Args){
		return Function->Execute(static_cast<TArgs&&>(Args)...);
	}
};

template<class TFunction>
class cFunctionSet;

template<class...TArgs>
class cFunctionSet<void (TArgs...)>
{
public:
	cFunctionSet(){}
	~cFunctionSet(){Clear();}

	cFunctionSet(const cFunctionSet &Src)=delete;
	cFunctionSet& operator =(const cFunctionSet &Src)=delete;
	
	template<class...TCallArgs>
	typename cnVar::TTypeConditional<void,
		sizeof...(TCallArgs)==sizeof...(TArgs)
	>::Type operator () (TCallArgs&&...Args){
		if(fCallListCache.GetCount()!=0){
			fRepeat=true;
			return;
		}

		do{
			fRepeat=false;

			for(auto *p : fFunctionSet){
				fCallListCache.AppendMake(p);
			}

			for(auto CallProcedure : fCallListCache){
				if(CallProcedure!=nullptr){
					CallProcedure->Call(cnVar::Forward<TCallArgs&&>(Args)...);
				}
			}

			fCallListCache.Clear();
		}while(fRepeat);
	}

	void Clear(void){
		auto Node=fFunctionSet.RemoveAll();
		if(Node!=nullptr){
			fFunctionSet.DeleteAllNodes(Node,DeleteItem);
		}
	}

	template<class TFunc>
	iFunctionToken* Insert(TFunc&& Func){
		auto Item=new cFunctionItem<TFunc>(static_cast<TFunc&&>(Func));
		if(fFunctionSet.Insert(Item)){
			return Item;
		}
		delete Item;
		return nullptr;
	}

	bool Remove(iFunctionToken *Token){
		if(Token==nullptr)
			return false;
		auto FuncItem=static_cast<bcFunctionItem*>(Token);
		if(fFunctionSet.Remove(FuncItem)==false)
			return false;

		FunctionRemoved(FuncItem);
		return true;
	}


	template<class TFunc>
	iFunctionToken* InsertWithID(void *ID,TFunc&& Func){
		auto Item=new cFunctionWithIDItem<TFunc>(ID,static_cast<TFunc&&>(Func));
		if(fFunctionSet.Insert(Item)){
			return Item;
		}
		delete Item;
		return nullptr;
	}

	bool RemoveByID(void *ID){
		auto FuncPos=fFunctionSet.Find(ID);
		if(FuncPos==fFunctionSet.end())
			return false;

		bcFunctionItem *FuncItem=*FuncPos;

		fFunctionSet.Remove(FuncPos);
		FunctionRemoved(FuncItem);
		return true;
	}


	iFunctionToken* InsertFunction(iFunction<void (TArgs...)> *Function,iReference *Reference=nullptr){
		if(Reference==nullptr){
			return InsertWithID(Function,cFunctionInterfaceFunctor<void (TArgs...)>{Function});
		}
		else{
			return InsertWithID(Function,cReferencedFunctionInterfaceFunctor<void (TArgs...)>{Function,Reference});
		}
	}

	bool RemoveFunction(iFunction<void (TArgs...)> *Function){
		return RemoveByID(Function);
	}

private:
	enum class ItemType : ufInt8
	{
		Function,
		FunctionWithID,
	};

	class bcFunctionItem : public iFunctionToken, public cRTLAllocator
	{
	public:
		bcFunctionItem *Parent;
		bcFunctionItem *Child[2];
		ufInt8 Branch;
		ufInt8 Color;
		ItemType Type;

		virtual ~bcFunctionItem(){}
		virtual void Call(TArgs...)=0;
		virtual sfInt8 Compare(const bcFunctionItem *Compare)const=0;
	};

	template<class TFunc>
	class cFunctionItem : public bcFunctionItem
	{
	public:
		cFunctionItem(TFunc &&Func):Function(static_cast<TFunc&&>(Func)){
			this->Type=ItemType::Function;
		}

		TFunc Function;

		virtual void Call(TArgs...Args)override{
			Function(static_cast<TArgs&&>(Args)...);
		}
		virtual sfInt8 Compare(const bcFunctionItem *Compare)const override{
			if(this==Compare)
				return 0;
			if(this<Compare)
				return -1;
			return 1;
		}
	};

	class bcFunctionWithIDItem : public bcFunctionItem
	{
	public:
		bcFunctionWithIDItem(void *ID)
			: ItemID(ID)
		{
			this->Type=ItemType::FunctionWithID;
		}

		void *ItemID;

		sfInt8 CompareID(void *ID)const{
			if(ItemID==ID)
				return 0;
			if(ItemID<ID)
				return -1;
			return 1;
		}
		virtual sfInt8 Compare(const bcFunctionItem *Compare)const override{
			auto CmpItem=static_cast<const bcFunctionWithIDItem*>(Compare);
			return CompareID(CmpItem->ItemID);
		}
	};
	template<class TFunc>
	class cFunctionWithIDItem : public bcFunctionWithIDItem
	{
	public:
		cFunctionWithIDItem(void *ID,TFunc &&Func)
			: bcFunctionWithIDItem(ID)
			, Function(static_cast<TFunc&&>(Func))
		{
		}

		TFunc Function;

		virtual void Call(TArgs...Args)override{
			Function(static_cast<TArgs&&>(Args)...);
		}
	};


	struct cOrderOperator
	{
		static sfInt8 Compare(const bcFunctionItem &Item,const bcFunctionItem &CompareItem){
			if(Item.Type>CompareItem.Type){
				return 1;
			}
			else if(Item.Type<CompareItem.Type){
				return -1;
			}
			return Item.Compare(&CompareItem);
		}
		static sfInt8 Compare(const bcFunctionItem &Item,void *ID){
			if(Item.Type<ItemType::FunctionWithID)
				return -1;
			if(Item.Type>ItemType::FunctionWithID)
				return 1;
			auto &FuncIDItem=static_cast<const bcFunctionWithIDItem&>(Item);
			return FuncIDItem.CompareID(ID);
		}
	};

	cLinkItemSet<bcFunctionItem,cOrderOperator> fFunctionSet;
	cSeqList<bcFunctionItem*> fCallListCache;
	bool fRepeat;

	static void DeleteItem(bcFunctionItem *Item){
		delete Item;
	}


	void FunctionRemoved(bcFunctionItem *Item){
		for(auto &CallFunc : fCallListCache){
			if(CallFunc==Item){
				CallFunc=nullptr;
			}
		}
		delete Item;
	}

};
//---------------------------------------------------------------------------
template<class TFunction>
class cMTFunctionSet;

template<class...TArgs>
class cMTFunctionSet<void (TArgs...)>
{
public:
	cMTFunctionSet(){
		fMapMutex=cnSystem::CreateMutexLock();
	}
	~cMTFunctionSet(){Clear();}

	cMTFunctionSet(const cMTFunctionSet &Src)=delete;
	cMTFunctionSet& operator =(const cMTFunctionSet &Src)=delete;

	template<class...TCallArgs>
	typename cnVar::TTypeConditional<void,
		sizeof...(TCallArgs)==sizeof...(TArgs)
	>::Type  operator () (TCallArgs&&...Args){

		auto CallCache=fListCacheRecycler.Query();
		cSeqList<bcFunctionItem*> &CallList=CallCache->List;
		{
			auto AutoLock=TakeLock(fMapMutex);
			for(auto *p : fFunctionSet){
				FuncItemRefInc(p);
				CallList.AppendMake(p);

			}
		}


		for(auto CallItem : CallList){
			if(CallItem->Disable==false){
				CallItem->Call(static_cast<TCallArgs&&>(Args)...);
			}
		}
		CallCache->List.Clear();
	}


	void Clear(void){
		auto Node=fFunctionSet.RemoveAll();
		if(Node!=nullptr){
			fFunctionSet.DeleteAllNodes(Node,FuncItemRefDec);
		}
	}

	template<class TFunc>
	iFunctionToken* Insert(TFunc&& Func){
		auto Item=new cFunctionItem<TFunc>(static_cast<TFunc&&>(Func));
		Item->Disable=false;
		Item->RefCount=1;

		{
			auto AutoLock=TakeLock(fMapMutex);

			if(fFunctionSet.Insert(Item)){
				return Item;
			}
		}
		delete Item;
		return nullptr;
	}


	bool Remove(iFunctionToken *Token){
		auto AutoLock=TakeLock(fMapMutex);

		auto FuncItem=static_cast<bcFunctionItem*>(Token);
		if(fFunctionSet.Remove(FuncItem)==false)
			return false;

		FuncItem->Disable=true;
		FuncItemRefDec(FuncItem);
		return true;
	}


	template<class TFunc>
	iFunctionToken* InsertWithID(void *ID,TFunc&& Func){

		auto Item=new cFunctionWithIDItem<TFunc>(ID,static_cast<TFunc&&>(Func));
		Item->Disable=false;
		Item->RefCount=1;

		{
			auto AutoLock=TakeLock(fMapMutex);
			if(fFunctionSet.Insert(Item)){
				return Item;
			}
		}
		delete Item;
		return nullptr;
	}

	bool RemoveByID(void *ID){
		bcFunctionItem *FuncItem;
		{
			auto AutoLock=TakeLock(fMapMutex);

			auto FuncPos=fFunctionSet.Find(ID);
			if(FuncPos==fFunctionSet.end())
				return false;
			FuncItem=*FuncPos;

			if(fFunctionSet.Remove(FuncPos)==false)
				return false;
		}

		FuncItem->Disable=true;
		FuncItemRefDec(FuncItem);
		return true;
	}


	iFunctionToken* InsertFunction(iFunction<void (TArgs...)> *Function,iReference *Reference=nullptr){
		if(Reference==nullptr){
			return InsertWithID(Function,cFunctionInterfaceFunctor<void (TArgs...)>(Function));
		}
		else{
			return InsertWithID(Function,cReferencedFunctionInterfaceFunctor<void (TArgs...)>(Function,Reference));
		}
	}

	bool RemoveFunction(iFunction<void (TArgs...)> *Function){
		return RemoveByID(Function);
	}

private:
	rPtr<iMutex> fMapMutex;

	enum class ItemType : ufInt8
	{
		FunctionItem,
		FunctionWithIDItem,
	};

	class bcFunctionItem : public iFunctionToken, public cRTLAllocator
	{
	public:
		bcFunctionItem *Parent;
		bcFunctionItem *Child[2];
		ufInt8 Branch;
		ufInt8 Color;
		ItemType Type;
		bool Disable;
		cAtomicVar<ufInt8> RefCount;

		virtual ~bcFunctionItem(){}
		virtual void Call(TArgs...)=0;
	};

	template<class TFunc>
	class cFunctionItem : public bcFunctionItem
	{
	public:
		cFunctionItem(TFunc &&Func)
			: Function(static_cast<TFunc&&>(Func))
		{
			this->Type=ItemType::FunctionItem;
		}

		TFunc Function;

		virtual void Call(TArgs...Args)override{
			Function(static_cast<TArgs&&>(Args)...);
		}
	};

	class bcFunctionWithIDItem : public bcFunctionItem
	{
	public:
		bcFunctionWithIDItem(void *ID)
			: ItemID(ID)
		{
			this->Type=ItemType::FunctionItemWithID;
		}

		void *ItemID;

		sfInt8 CompareID(void *ID)const{
			if(ItemID==ID)
				return 0;
			if(ItemID<ID)
				return -1;
			return 1;
		}
		virtual sfInt8 Compare(const bcFunctionItem *Compare)const override{
			auto CmpItem=static_cast<const bcFunctionWithIDItem*>(Compare);
			return CompareID(CmpItem->ItemID);
		}
	};
	template<class TFunc>
	class cFunctionWithIDItem : public bcFunctionWithIDItem
	{
	public:
		cFunctionWithIDItem(void *ID,TFunc &&Func)
			: bcFunctionWithIDItem(ID)
			, Function(static_cast<TFunc&&>(Func))
		{
		}

		TFunc Function;

		virtual void Call(TArgs...Args)override{
			Function(static_cast<TArgs&&>(Args)...);
		}
	};

	static void FuncItemRefInc(bcFunctionItem *Item){
		Item->RefCount.Free++;
	}
	static void FuncItemRefDec(bcFunctionItem *Item){
		if(--Item->RefCount.Free==0){
			delete Item;
		}
	}

	struct cOrderOperator
	{
		static sfInt8 Compare(const bcFunctionItem &Item,const bcFunctionItem &CompareItem){
			const void *pItem=&Item;
			const void *pCompareItem=&CompareItem;
			if(pItem==pCompareItem)
				return 0;
			if(pItem<pCompareItem)
				return -1;
			return 1;
		}
	};

	cLinkItemSet<bcFunctionItem,cOrderOperator> fFunctionSet;
	struct cListCacheItem
	{
		cSeqList<bcFunctionItem*> List;
	};
	arSharedObjectRecycler<cListCacheItem> fListCacheRecycler;
};
//---------------------------------------------------------------------------
typedef cFunctionSet<void (void)> cCallbackSet;
//---------------------------------------------------------------------------
}   // namespace cnRTL
//---------------------------------------------------------------------------
}   // namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif

