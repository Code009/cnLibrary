/*- cnTK - Linked Data Struct ---------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2019-08-03                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnTK_LinkStruct_H__
#define	__cnLibrary_cnTK_LinkStruct_H__
/*-------------------------------------------------------------------------*/
#include <cnTK/Common.hpp>
#include <cnTK/TypeTraits.hpp>
#include <cnTK/Memory.hpp>
#include <cnTK/StructItem.hpp>
/*-------------------------------------------------------------------------*/
#if	cnLibrary_CPPFEATURELEVEL >= 1
//---------------------------------------------------------------------------
#include <cnTK/TKMacrosDeclare.inc>
//---------------------------------------------------------------------------
#if cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnDataStruct{
//---------------------------------------------------------------------------
template<class TSingleLinkedItemOperator,class TItem>
struct cSingleLinkedItemIterator
{
	TItem *ItemPtr;

	cSingleLinkedItemIterator()noexcept(true){}
	explicit cSingleLinkedItemIterator(TItem *p)noexcept(true):ItemPtr(p){}

	TItem* operator *()const noexcept(true){
		return ItemPtr;
	}
	TItem* operator ->()const noexcept(true){
		return ItemPtr;
	}

	bool operator ==(const cSingleLinkedItemIterator &Dest)const noexcept(true){
		return ItemPtr==Dest.ItemPtr;
	}
	bool operator !=(const cSingleLinkedItemIterator &Dest)const noexcept(true){
		return ItemPtr!=Dest.ItemPtr;
	}
	cSingleLinkedItemIterator& operator ++(void)noexcept(true){
		ItemPtr=TSingleLinkedItemOperator::GetNext(ItemPtr);
		return *this;
	}
	void operator ++(int)noexcept(true){	operator ++();	}
};
//---------------------------------------------------------------------------
template<class TSingleLinkedItemOperator>
class cLinkItemStack
{
public:
	typedef typename TSingleLinkedItemOperator::tItem tItem;

	typedef cSingleLinkedItemIterator<TSingleLinkedItemOperator,tItem> tIterator;
	typedef cSingleLinkedItemIterator<TSingleLinkedItemOperator,const tItem> tConstIterator;

	cLinkItemStack()noexcept(true):fTop(nullptr){}
	cLinkItemStack(tItem *InitalTop)noexcept(true):fTop(InitalTop){}
	~cLinkItemStack()noexcept(true){	cnLib_ASSERT(fTop==nullptr);	}

	cLinkItemStack(const cLinkItemStack &Src)=delete;
	// = copy
	cLinkItemStack& operator =(const cLinkItemStack &Src)=delete;

#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

	cLinkItemStack(cLinkItemStack &&Src)noexcept(true){
		fTop=Src.fTop;
		Src.fTop=nullptr;
	}
	// = move
	cLinkItemStack& operator =(cLinkItemStack &&Src)noexcept(true){
		fTop=Src.fTop;
		Src.fTop=nullptr;
		return *this;
	}
#endif	// cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

	// GetCount
	// return number items in the list
    uIntn GetCount(void)const noexcept(true){
		uIntn Count=0;
		auto Node=fTop;
		while(Node!=nullptr){
			Count++;
			Node=TSingleLinkedItemOperator::GetNext(Node);
		}
		return Count;
	}

	// for range
	tIterator	begin(void)const noexcept(true){	return tIterator(fTop);	}
	tIterator	end(void)const noexcept(true){		return tIterator(nullptr);	}
	tConstIterator	cbegin(void)const noexcept(true){	return tConstIterator(fTop);	}
	tConstIterator	cend(void)const noexcept(true){	return tConstIterator(nullptr);	}



	bool IsEmpty(void)const noexcept(true){
		return fTop==nullptr;
	}

	// Push
	//	Add a data item to the specifed position of the list
	void Push(tItem *NewItem)noexcept(true){
		TSingleLinkedItemOperator::SetNext(NewItem,fTop);
		fTop=NewItem;
	}
	void Push(tItem *NewItem,tItem *NewTailItem)noexcept(true){
		cnLib_ASSERT(SingleLinkedListItemIsConnected<TSingleLinkedItemOperator>(NewItem,NewTailItem));
		TSingleLinkedItemOperator::SetNext(NewTailItem,fTop);
		fTop=NewItem;
	}

	// Pop
	//	Pop items from top of the stack
	tItem* Pop(void)noexcept(true){
		if(fTop==nullptr)
			return nullptr;
		auto RemovedItem=fTop;
		fTop=TSingleLinkedItemOperator::GetNext(fTop);

		return RemovedItem;
	}
	tItem* Peek(void)const noexcept(true){
		return fTop;
	}

	tItem* Swap(void)noexcept(true){
		tItem *Swapped=fTop;
		fTop=nullptr;
		return Swapped;
	}

	tItem* Swap(tItem *Item)noexcept(true){
		if(fTop==nullptr){
			fTop=Item;
			return nullptr;
		}
		if(Item==nullptr){
			Item=fTop;
			fTop=nullptr;
			return Item;
		}

		// break link to Node
		auto SwapTop=fTop;
		while(SwapTop!=nullptr){
			auto SwapNext=TSingleLinkedItemOperator::GetNext(SwapTop);
			if(SwapNext==Item){
				TSingleLinkedItemOperator::SetNext(SwapTop,nullptr);
				break;
			}
			SwapTop=SwapNext;
		}
		// swap
		SwapTop=fTop;
		fTop=Item;
		return SwapTop;
	}
protected:
	tItem *fTop;
};
//---------------------------------------------------------------------------
template<class TSingleLinkedItemOperator>
class cLinkItemQueue
{
public:
	typedef typename TSingleLinkedItemOperator::tItem tItem;

	typedef cSingleLinkedItemIterator<TSingleLinkedItemOperator,tItem> tIterator;
	typedef cSingleLinkedItemIterator<TSingleLinkedItemOperator,const tItem> tConstIterator;

	cLinkItemQueue()noexcept(true){
		Reset();
	}
	~cLinkItemQueue()noexcept(true){
		cnLib_ASSERT(IsEmpty());
	}
	cLinkItemQueue(const cLinkItemQueue &Src)=delete;
	// = copy
	cLinkItemQueue& operator =(const cLinkItemQueue &Src)=delete;

#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L
	cLinkItemQueue(cLinkItemQueue &&Src)noexcept(true){
		tItem *SrcCenterNode=TSingleLinkedItemOperator::LinkNode(Src.fCenterLink);
		if(Src.fTailItem==SrcCenterNode){
			Reset();
		}
		else{
			tItem *CenterNode=TSingleLinkedItemOperator::LinkNode(fCenterLink);
			fTailItem=Src.fTailItem;

			TSingleLinkedItemOperator::SetNext(CenterNode,TSingleLinkedItemOperator::GetNext(SrcCenterNode));
			TSingleLinkedItemOperator::SetNext(fTailItem,CenterNode);
			Src.Reset();
		}
	}
	// = move
	cLinkItemQueue& operator =(cLinkItemQueue &&Src)noexcept(true){
		tItem *CenterNode=TSingleLinkedItemOperator::LinkNode(fCenterLink);
		if(fTailItem==CenterNode){
			// queue must be empty to accept other queue
			tItem *SrcCenterNode=TSingleLinkedItemOperator::LinkNode(Src.fCenterLink);
			if(Src.fTailItem!=SrcCenterNode){
				// surce queue is not empty
				fTailItem=Src.fTailItem;

				TSingleLinkedItemOperator::SetNext(CenterNode,TSingleLinkedItemOperator::GetNext(SrcCenterNode));
				TSingleLinkedItemOperator::SetNext(fTailItem,CenterNode);
				Src.Reset();
			}
		}
		return *this;
	}
#endif	// cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

	bool IsEmpty(void)const noexcept(true){
		return fTailItem==GetCenterNode();
	}

	// GetCount
	// return number items in the list
    uIntn GetCount(void)const noexcept(true){
		uIntn Count=0;
		const tItem *CenterNode=GetCenterNode();
		auto Item=TSingleLinkedItemOperator::GetNext(CenterNode);
		while(Item!=CenterNode){
			Count++;
			Item=TSingleLinkedItemOperator::GetNext(Item);
		}
		return Count;
	}


	// for range
	tIterator	begin(void)const noexcept(true){
		tItem *CenterNode=TSingleLinkedItemOperator::LinkNode(fCenterLink);
		return tIterator(TSingleLinkedItemOperator::GetNext(CenterNode));

	}
	tIterator	end(void)const noexcept(true){
		return tIterator(TSingleLinkedItemOperator::LinkNode(fCenterLink));
	}
	tConstIterator	cbegin(void)const noexcept(true){
		tItem *CenterNode=TSingleLinkedItemOperator::LinkNode(fCenterLink);
		return tConstIterator(TSingleLinkedItemOperator::GetNext(CenterNode));

	}
	tConstIterator	cend(void)const noexcept(true){
		return tConstIterator(TSingleLinkedItemOperator::LinkNode(fCenterLink));
	}

	tItem* Peek(void)noexcept(true){
		tItem *CenterNode=TSingleLinkedItemOperator::LinkNode(fCenterLink);
		return TSingleLinkedItemOperator::GetNext(CenterNode);
	}

	//tItem* GetHead(void)const noexcept(true){
	//	tItem *CenterNode=TSingleLinkedItemOperator::LinkNode(fCenterLink);
	//	return TSingleLinkedItemOperator::GetNext(CenterNode);
	//}
	//tItem* GetTail(void)const noexcept(true){
	//	return fTailItem;
	//}

	void Enqueue(tItem *NewItem)noexcept(true){
		TSingleLinkedItemOperator::SetNext(fTailItem,NewItem);
		tItem *CenterNode=TSingleLinkedItemOperator::LinkNode(fCenterLink);
		TSingleLinkedItemOperator::SetNext(NewItem,CenterNode);
		fTailItem=NewItem;
	}
	// Enqueue
	void Enqueue(tItem *NewItem,tItem *NewTailItem)noexcept(true){
		cnLib_ASSERT(SingleLinkedListItemIsConnected<TSingleLinkedItemOperator>(NewItem,NewTailItem));
		TSingleLinkedItemOperator::SetNext(fTailItem,NewItem);
		tItem *CenterNode=TSingleLinkedItemOperator::LinkNode(fCenterLink);
		TSingleLinkedItemOperator::SetNext(NewTailItem,CenterNode);
		fTailItem=NewTailItem;
	}

	// Dequeue
	//	dequeue an item from the beginning of queue
	tItem* Dequeue(void)noexcept(true){
		tItem *CenterNode=TSingleLinkedItemOperator::LinkNode(fCenterLink);
		auto HeadNode=TSingleLinkedItemOperator::GetNext(CenterNode);
		if(HeadNode==CenterNode)
			return nullptr;

		auto NextHead=TSingleLinkedItemOperator::GetNext(HeadNode);
		TSingleLinkedItemOperator::SetNext(CenterNode,NextHead);
		if(NextHead==CenterNode){
			fTailItem=CenterNode;
		}
		return HeadNode;
	}
	tItem* DequeueAll(void)noexcept(true){
		tItem *CenterNode=TSingleLinkedItemOperator::LinkNode(fCenterLink);
		auto CurHead=TSingleLinkedItemOperator::GetNext(CenterNode);
		if(CurHead==CenterNode)
			return nullptr;
		TSingleLinkedItemOperator::SetNext(fTailItem,nullptr);

		TSingleLinkedItemOperator::SetNext(CenterNode,CenterNode);
		fTailItem=CenterNode;

		return CurHead;
	}


	tItem* DequeueTo(tItem *Item)noexcept(true){
		tItem *CenterNode=TSingleLinkedItemOperator::LinkNode(fCenterLink);
		cnLib_ASSERT(SingleLinkedListItemIsConnected<TSingleLinkedItemOperator>(CenterNode,Item));

		auto DividedHead=TSingleLinkedItemOperator::GetNext(CenterNode);

		auto NewHead=TSingleLinkedItemOperator::GetNext(Item);
		TSingleLinkedItemOperator::SetNext(CenterNode,NewHead);
		if(NewHead==CenterNode){
			fTailItem=CenterNode;
		}

		TSingleLinkedItemOperator::SetNext(Item,nullptr);
		return DividedHead;
	}

protected:
	typename TSingleLinkedItemOperator::tLinkInstance fCenterLink;
	tItem *fTailItem;

	tItem *GetCenterNode(void)noexcept(true){
		return TSingleLinkedItemOperator::LinkNode(fCenterLink);
	}
	const tItem *GetCenterNode(void)const noexcept(true){
		return TSingleLinkedItemOperator::LinkNode(const_cast<typename TSingleLinkedItemOperator::tLinkInstance&>(fCenterLink));
	}

	void Reset(void)noexcept(true){
		tItem *CenterNode=TSingleLinkedItemOperator::LinkNode(fCenterLink);
		TSingleLinkedItemOperator::SetNext(CenterNode,CenterNode);
		fTailItem=CenterNode;
	}
};
//---------------------------------------------------------------------------
template<class TSingleLinkedItemOperator,class TSingleLinkedItemContentOperator,class TContent>
struct cSingleLinkedItemContentIterator
{
	typedef typename TSingleLinkedItemOperator::tItem tItem;
	tItem *ItemPtr;

	cSingleLinkedItemContentIterator()noexcept(true){}
	cSingleLinkedItemContentIterator(tItem *p)noexcept(true):ItemPtr(p){}

	operator tItem*()const noexcept(true){	return ItemPtr;	}

	TContent& operator *(void)const noexcept(true){
		return *TSingleLinkedItemContentOperator::ContentPtr(ItemPtr);
	}
	TContent* operator ->(void)const noexcept(true){
		return TSingleLinkedItemContentOperator::ContentPtr(ItemPtr);
	}

	bool operator ==(const cSingleLinkedItemContentIterator &Dest)const noexcept(true){
		return ItemPtr==Dest.ItemPtr;
	}
	bool operator !=(const cSingleLinkedItemContentIterator &Dest)const noexcept(true){
		return ItemPtr!=Dest.ItemPtr;
	}
	cSingleLinkedItemContentIterator& operator ++(void)noexcept(true){
		ItemPtr=TSingleLinkedItemOperator::GetNext(ItemPtr);
		return *this;
	}
	void operator ++(int)noexcept(true){	operator ++();	}
};
//---------------------------------------------------------------------------
template<class TSingleLinkedItemOperator,class TSingleLinkedItemContentOperator>
class cLinkStack
{
public:
	typedef typename TSingleLinkedItemOperator::tItem tItem;

	typedef cSingleLinkedItemContentIterator<TSingleLinkedItemOperator,TSingleLinkedItemContentOperator,tItem> tIterator;
	typedef cSingleLinkedItemContentIterator<TSingleLinkedItemOperator,TSingleLinkedItemContentOperator,const tItem> tConstIterator;

	cLinkStack()noexcept(true):fTop(nullptr){}
	~cLinkStack()noexcept(true){
		Clear();
	}
	cLinkStack(const cLinkStack &Src)noexcept(true){
		fTop=DuplicateStackNode(Src.fTop);
	}

	// = copy
	cLinkStack& operator =(const cLinkStack &Src)noexcept(true){
		if(this!=&Src){
			Clear();
			fTop=DuplicateStackNode(Src.fTop);
		}
		return *this;
	}

#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L
	cLinkStack(cLinkStack &&Src)noexcept(true){
		fTop=Src.fTop;
		Src.fTop=nullptr;
	}

	// = move
	cLinkStack& operator =(cLinkStack &&Src)noexcept(true){
		fTop=Src.fTop;
		Src.fTop=nullptr;
		return *this;
	}
#endif	// cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

	// GetCount
	// return number items in the list
    uIntn GetCount(void)const noexcept(true){
		uIntn Count=0;
		auto Node=fTop;
		while(Node!=nullptr){
			Count++;
			Node=TSingleLinkedItemOperator::GetNext(Node);
		}
		return Count;
	}

	// for range
	tIterator	begin(void)const noexcept(true){	return fTop;	}
	tIterator	end(void)const noexcept(true){		return nullptr;	}
	tConstIterator	cbegin(void)const noexcept(true){	return fTop;	}
	tConstIterator	cend(void)const noexcept(true){	return nullptr;	}



	// Clear
	//	remove all items
	void Clear(void)noexcept(true){
		if(fTop==nullptr)
			return;
		tItem *CurItem=fTop;
		fTop=nullptr;
		do{
			auto RemovedItem=CurItem;
			CurItem=TSingleLinkedItemOperator::GetNext(CurItem);

			TSingleLinkedItemContentOperator::DestroyNode(RemovedItem);
		}while(CurItem!=nullptr);
	}

	bool IsEmpty(void)const noexcept(true){
		return fTop==nullptr;
	}

	// Push
	//	Add a data item to the specifed position of the list
	template<class...VT>
	void Push(VT cnLib_UREF...Args)noexcept(true){
		tItem *NewItem=TSingleLinkedItemContentOperator::CreateNode(cnLib_UREFCAST(VT)(Args)...);
		TSingleLinkedItemOperator::SetNext(NewItem,fTop);
		fTop=NewItem;
	}

	// Pop
	//	Pop items from top of the stack
	void Pop(void)noexcept(true){
		if(fTop==nullptr)
			return;
		auto RemoveNode=fTop;
		fTop=TSingleLinkedItemOperator::GetNext(fTop);

		TSingleLinkedItemContentOperator::DestroyNode(RemoveNode);
	}
	typename TSingleLinkedItemContentOperator::tContent& Peek(void)noexcept(true){
		return *TSingleLinkedItemContentOperator::ContentPtr(fTop);
	}

protected:
	tItem *fTop;

	static tItem* DuplicateStackNode(tItem *CopyTop)noexcept(true){
		cnLib_ASSERT(CopyTop!=nullptr);
		auto Top=TSingleLinkedItemContentOperator::CreateNode(*TSingleLinkedItemContentOperator::ContentPtr(CopyTop));
		auto LastItem=Top;
		CopyTop=TSingleLinkedItemOperator::GetNext(CopyTop);
		while(CopyTop!=nullptr){
			auto NewItem=TSingleLinkedItemContentOperator::CreateNode(*TSingleLinkedItemContentOperator::ContentPtr(CopyTop));
			TSingleLinkedItemOperator::SetNext(LastItem,NewItem);
			CopyTop=TSingleLinkedItemOperator::GetNext(CopyTop);
			LastItem=NewItem;
		}
		TSingleLinkedItemOperator::SetNext(LastItem,nullptr);
		return Top;
	}
};
//---------------------------------------------------------------------------
template<class TSingleLinkedItemOperator,class TSingleLinkedItemContentOperator>
class cLinkQueue
{
public:
	typedef typename TSingleLinkedItemOperator::tItem tItem;
	typedef typename TSingleLinkedItemContentOperator::tContent tContent;
	
	typedef cSingleLinkedItemContentIterator<TSingleLinkedItemOperator,TSingleLinkedItemContentOperator,tContent> tIterator;
	typedef cSingleLinkedItemContentIterator<TSingleLinkedItemOperator,TSingleLinkedItemContentOperator,const tContent> tConstIterator;

	cLinkQueue()noexcept(true){
		Reset();
	}
	~cLinkQueue()noexcept(true){
		Clear();
	}
	cLinkQueue(const cLinkQueue &Src)noexcept(true){
		Reset();
		EnqueueRange(Src.begin(),Src.end());
	}
	// = copy
	cLinkQueue& operator =(const cLinkQueue &Src)noexcept(true){
		if(this!=&Src){
			Clear();
			EnqueueRange(Src.begin(),Src.end());
		}
		return *this;
	}

#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L
	cLinkQueue(cLinkQueue &&Src)noexcept(true){
		tItem *SrcCenterNode=TSingleLinkedItemOperator::LinkNode(Src.fCenterLink);
		if(Src.fTailItem==SrcCenterNode){
			Reset();
		}
		else{
			tItem *CenterNode=TSingleLinkedItemOperator::LinkNode(fCenterLink);
			fTailItem=Src.fTailItem;

			TSingleLinkedItemOperator::SetNext(CenterNode,TSingleLinkedItemOperator::GetNext(SrcCenterNode));
			TSingleLinkedItemOperator::SetNext(fTailItem,CenterNode);
			Src.Reset();
		}
	}

	// = move
	cLinkQueue& operator =(cLinkQueue &&Src)noexcept(true){
		tItem *CenterNode=TSingleLinkedItemOperator::LinkNode(fCenterLink);
		if(fTailItem==CenterNode){
			// queue must be empty to accept other queue
			tItem *SrcCenterNode=TSingleLinkedItemOperator::LinkNode(Src.fCenterLink);
			if(Src.fTailItem!=SrcCenterNode){
				// surce queue is not empty
				fTailItem=Src.fTailItem;

				TSingleLinkedItemOperator::SetNext(CenterNode,TSingleLinkedItemOperator::GetNext(SrcCenterNode));
				TSingleLinkedItemOperator::SetNext(fTailItem,CenterNode);
				Src.Reset();
			}
		}
		return *this;
	}
#endif	// cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

	bool IsEmpty(void)const noexcept(true){
		return fTailItem==GetCenterNode();
	}

	// GetCount
	// return number items in the list
    uIntn GetCount(void)const noexcept(true){
		uIntn Count=0;
		const tItem *CenterNode=GetCenterNode();
		tItem *Item=TSingleLinkedItemOperator::GetNext(CenterNode);
		while(Item!=CenterNode){
			Count++;
			Item=TSingleLinkedItemOperator::GetNext(Item);
		}
		return Count;
	}


	// for range
	tIterator	begin(void)const noexcept(true){
		tItem *CenterNode=TSingleLinkedItemOperator::LinkNode(fCenterLink);
		return TSingleLinkedItemOperator::GetNext(CenterNode);

	}
	tIterator	end(void)const noexcept(true){
		return TSingleLinkedItemOperator::LinkNode(fCenterLink);
	}
	tConstIterator	cbegin(void)const noexcept(true){
		tItem *CenterNode=TSingleLinkedItemOperator::LinkNode(fCenterLink);
		return TSingleLinkedItemOperator::GetNext(CenterNode);

	}
	tConstIterator	cend(void)const noexcept(true){
		return TSingleLinkedItemOperator::LinkNode(fCenterLink);
	}

	typename TSingleLinkedItemContentOperator::tContent& Peek(void)noexcept(true){
		tItem *CenterNode=TSingleLinkedItemOperator::LinkNode(fCenterLink);
		return *TSingleLinkedItemContentOperator::ContentPtr(TSingleLinkedItemOperator::GetNext(CenterNode));
	}

	// Clear
	//	remove all items
	void Clear(void)noexcept(true){
		tItem *CenterNode=TSingleLinkedItemOperator::LinkNode(fCenterLink);
		tItem *Item=TSingleLinkedItemOperator::GetNext(CenterNode);
		while(Item!=CenterNode){
			tItem *DeleteItem=Item;
			Item=TSingleLinkedItemOperator::GetNext(Item);

			TSingleLinkedItemContentOperator::DestroyNode(DeleteItem);
		}
		TSingleLinkedItemOperator::SetNext(CenterNode,CenterNode);
		fTailItem=CenterNode;
	}

	// Enqueue
	//	append an item item to the end of queue
	// Args		arguments for creating item
	template<class...VT>
	void Enqueue(VT cnLib_UREF...Args)noexcept(true){
		tItem *NewItem=TSingleLinkedItemContentOperator::CreateNode(cnLib_UREFCAST(VT)(Args)...);
		
		TSingleLinkedItemOperator::SetNext(fTailItem,NewItem);
		tItem *CenterNode=TSingleLinkedItemOperator::LinkNode(fCenterLink);
		TSingleLinkedItemOperator::SetNext(NewItem,CenterNode);
		fTailItem=NewItem;
	}

	template<class TIterator>
	void EnqueueRange(TIterator Begin,TIterator End)noexcept(true){
		tItem *LastItem=fTailItem;
		while(Begin!=End){
			auto NewItem=TSingleLinkedItemContentOperator::CreateNode(*Begin);
			TSingleLinkedItemOperator::SetNext(LastItem,NewItem);
			LastItem=NewItem;

			++Begin;
		}
		fTailItem=LastItem;
		tItem *CenterNode=TSingleLinkedItemOperator::LinkNode(fCenterLink);
		TSingleLinkedItemOperator::SetNext(LastItem,CenterNode);
	}

	// Dequeue
	//	dequeue an item from the beginning of queue
	void Dequeue(void)noexcept(true){
		tItem *CenterNode=TSingleLinkedItemOperator::LinkNode(fCenterLink);
		tItem *HeadItem=TSingleLinkedItemOperator::GetNext(CenterNode);
		if(HeadItem==CenterNode)
			return;

		auto NextHead=TSingleLinkedItemOperator::GetNext(HeadItem);
		TSingleLinkedItemOperator::SetNext(CenterNode,NextHead);
		if(NextHead==CenterNode){
			fTailItem=CenterNode;
		}
		TSingleLinkedItemContentOperator::DestroyNode(HeadItem);
	}

protected:
	typename TSingleLinkedItemOperator::tLinkInstance fCenterLink;
	tItem *fTailItem;

	tItem *GetCenterNode(void)noexcept(true){
		return TSingleLinkedItemOperator::LinkNode(fCenterLink);
	}
	const tItem *GetCenterNode(void)const noexcept(true){
		return TSingleLinkedItemOperator::LinkNode(const_cast<typename TSingleLinkedItemOperator::tLinkInstance&>(fCenterLink));
	}

	void Reset(void)noexcept(true){
		tItem *CenterNode=TSingleLinkedItemOperator::LinkNode(fCenterLink);
		TSingleLinkedItemOperator::SetNext(CenterNode,CenterNode);
		fTailItem=CenterNode;
	}
};
//---------------------------------------------------------------------------
template<class TDualLinkListItemOperator,class TItem>
struct cDualLinkedItemIterator
{
	TItem *ItemPtr;

	cDualLinkedItemIterator()noexcept(true){}
	explicit cDualLinkedItemIterator(TItem *p)noexcept(true):ItemPtr(p){}

	TItem* operator *(void)const noexcept(true){
		return ItemPtr;
	}
	TItem* operator ->(void)const noexcept(true){
		return ItemPtr;
	}

	bool operator ==(const cDualLinkedItemIterator &Dest)const noexcept(true){
		return this->ItemPtr==Dest.ItemPtr;
	}
	bool operator !=(const cDualLinkedItemIterator &Dest)const noexcept(true){
		return this->ItemPtr!=Dest.ItemPtr;
	}

	cDualLinkedItemIterator& operator ++(void)noexcept(true){
		this->ItemPtr=TDualLinkListItemOperator::GetNext(this->ItemPtr);
		return *this;
	}
	void operator ++(int)noexcept(true){	operator ++();	}
	cDualLinkedItemIterator& operator --(void)noexcept(true){
		this->ItemPtr=TDualLinkListItemOperator::GetPrev(this->ItemPtr);
		return *this;
	}
	void operator --(int)noexcept(true){	operator --();	}
};
//---------------------------------------------------------------------------
template<class TDualLinkListItemOperator>
class cLinkItemList
{
public:
	typedef typename TDualLinkListItemOperator::tItem tItem;

	typedef cDualLinkedItemIterator<TDualLinkListItemOperator,tItem> tIterator;
	typedef cDualLinkedItemIterator<TDualLinkListItemOperator,const tItem> tConstIterator;

	cLinkItemList()noexcept(true){
		Reset();
	}
	~cLinkItemList()noexcept(true){
		cnLib_ASSERT(IsEmpty());
	}
	cLinkItemList(const cLinkItemList &Src)=delete;
	// = copy
	cLinkItemList& operator =(const cLinkItemList &Src)=delete;

#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

	cLinkItemList(cLinkItemList &&Src)noexcept(true){
		tItem *CenterNode=TDualLinkListItemOperator::LinkNode(fCenterLink);
		tItem *SrcCenterNode=TDualLinkListItemOperator::LinkNode(Src.fCenterLink);
		if(TDualLinkListItemOperator::GetNext(SrcCenterNode)==SrcCenterNode){
			Reset();
		}
		else{
			TDualLinkListItemOperator::SetNext(CenterNode,TDualLinkListItemOperator::GetNext(SrcCenterNode));
			TDualLinkListItemOperator::SetPrev(CenterNode,TDualLinkListItemOperator::GetPrev(SrcCenterNode));
			Src.Reset();
		}
	}

	// = move
	cLinkItemList& operator =(cLinkItemList &&Src)noexcept(true){
		tItem *CenterNode=TDualLinkListItemOperator::LinkNode(fCenterLink);
		if(TDualLinkListItemOperator::GetNext(CenterNode)==CenterNode){
			tItem *SrcCenterNode=TDualLinkListItemOperator::LinkNode(Src.fCenterLink);
			if(TDualLinkListItemOperator::GetNext(SrcCenterNode)!=SrcCenterNode){
				TDualLinkListItemOperator::SetNext(CenterNode,TDualLinkListItemOperator::GetNext(SrcCenterNode));
				TDualLinkListItemOperator::SetPrev(CenterNode,TDualLinkListItemOperator::GetPrev(SrcCenterNode));
				Src.Reset();
			}
		}
		return *this;
	}
#endif	// cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

	bool IsEmpty(void)const noexcept(true){
		const tItem *CenterNode=TDualLinkListItemOperator::LinkNode(fCenterLink);
		return TDualLinkListItemOperator::GetNext(CenterNode)==CenterNode;
	}

	// GetCount
	// return number items in the list
    uIntn GetCount(void)const noexcept(true){
		uIntn Count=0;
		auto CenterNode=TDualLinkListItemOperator::LinkNode(fCenterLink);
		auto Node=TDualLinkListItemOperator::GetNext(CenterNode);
		while(Node!=CenterNode){
			Count++;
			Node=TDualLinkListItemOperator::GetNext(Node);
		}
		return Count;
	}


	// for range
	tIterator	begin(void)noexcept(true){	return tIterator(TDualLinkListItemOperator::GetNext(TDualLinkListItemOperator::LinkNode(fCenterLink)));	}
	tIterator	end(void)noexcept(true){	return tIterator(TDualLinkListItemOperator::LinkNode(fCenterLink));	}
	tIterator	begin(void)const noexcept(true){	return tIterator(TDualLinkListItemOperator::GetNext(TDualLinkListItemOperator::LinkNode(fCenterLink)));	}
	tIterator	end(void)const noexcept(true){	return tIterator(TDualLinkListItemOperator::LinkNode(fCenterLink));	}
	tConstIterator	cbegin(void)const noexcept(true){	return tConstIterator(TDualLinkListItemOperator::GetNext(TDualLinkListItemOperator::LinkNode(fCenterLink)));	}
	tConstIterator	cend(void)const noexcept(true){	return tConstIterator(TDualLinkListItemOperator::LinkNode(fCenterLink));	}

	// RemoveAll
	//	remove all items
	tItem* RemoveAll(void)noexcept(true){
		tItem *CenterNode=TDualLinkListItemOperator::LinkNode(fCenterLink);
		tItem *HeadItem=TDualLinkListItemOperator::GetNext(CenterNode);
		if(HeadItem==CenterNode)
			return nullptr;
		tItem *TailItem=TDualLinkListItemOperator::GetPrev(CenterNode);
		TDualLinkListItemOperator::SetPrev(HeadItem,nullptr);
		TDualLinkListItemOperator::SetNext(TailItem,nullptr);
		TDualLinkListItemOperator::SetNext(CenterNode,CenterNode);
		TDualLinkListItemOperator::SetPrev(CenterNode,CenterNode);
		return HeadItem;
	}

	// Insert
	//	Insert an item before the specifed item
	void InsertBefore(tIterator Pos,tItem *NewItem)noexcept(true){
		cnLib_ASSERT(SingleLinkedListItemIsConnected<TDualLinkListItemOperator>(TDualLinkListItemOperator::LinkNode(fCenterLink),NewItem)==false);
		DualLinkedListItemInsertBefore<TDualLinkListItemOperator>(Pos,NewItem,NewItem);
	}
	// Insert
	//	Insert an item before the specifed item
	void InsertBefore(tIterator Pos,tItem *NewHeadItem,tItem *NewTailItem)noexcept(true){
		cnLib_ASSERT(SingleLinkedListItemIsConnected<TDualLinkListItemOperator>(TDualLinkListItemOperator::LinkNode(fCenterLink),NewHeadItem)==false);
		cnLib_ASSERT(DualLinkedListItemIsConnected<TDualLinkListItemOperator>(NewHeadItem,NewTailItem));

		DualLinkedListItemInsertBefore<TDualLinkListItemOperator>(Pos,NewHeadItem,NewTailItem);
	}

	// Insert
	//	Insert an item before the specifed item
	void InsertAfter(tIterator Pos,tItem *NewItem)noexcept(true){
		cnLib_ASSERT(SingleLinkedListItemIsConnected<TDualLinkListItemOperator>(TDualLinkListItemOperator::LinkNode(fCenterLink),NewItem)==false);
		DualLinkedListItemInsertAfter<TDualLinkListItemOperator>(Pos,NewItem,NewItem);
	}
	// Insert
	//	Insert an item before the specifed item
	void InsertAfter(tIterator Pos,tItem *NewHeadItem,tItem *NewTailItem)noexcept(true){
		cnLib_ASSERT(SingleLinkedListItemIsConnected<TDualLinkListItemOperator>(TDualLinkListItemOperator::LinkNode(fCenterLink),NewHeadItem)==false);
		cnLib_ASSERT(DualLinkedListItemIsConnected<TDualLinkListItemOperator>(NewHeadItem,NewTailItem));
		DualLinkedListItemInsertAfter<TDualLinkListItemOperator>(Pos,NewHeadItem,NewTailItem);
	}

	// InsertHead
	//	Insert an item to the beginning of the list
	void InsertHead(tItem *NewItem)noexcept(true){
		tItem *CenterNode=TDualLinkListItemOperator::LinkNode(fCenterLink);
		cnLib_ASSERT(SingleLinkedListItemIsConnected<TDualLinkListItemOperator>(CenterNode,NewItem)==false);
		DualLinkedListItemInsertAfter<TDualLinkListItemOperator>(CenterNode,NewItem,NewItem);
	}
	// InsertHead
	//	Insert an item to the beginning of the list
	void InsertHead(tItem *NewHeadItem,tItem *NewTailItem)noexcept(true){
		tItem *CenterNode=TDualLinkListItemOperator::LinkNode(fCenterLink);
		cnLib_ASSERT(SingleLinkedListItemIsConnected<TDualLinkListItemOperator>(CenterNode,NewHeadItem)==false);
		cnLib_ASSERT(DualLinkedListItemIsConnected<TDualLinkListItemOperator>(NewHeadItem,NewTailItem));
		DualLinkedListItemInsertAfter<TDualLinkListItemOperator>(CenterNode,NewHeadItem,NewTailItem);
	}
	// InsertTail
	//	Insert an item item to the end of list
	void InsertTail(tItem *NewItem)noexcept(true){
		tItem *CenterNode=TDualLinkListItemOperator::LinkNode(fCenterLink);
		cnLib_ASSERT(SingleLinkedListItemIsConnected<TDualLinkListItemOperator>(CenterNode,NewItem)==false);
		DualLinkedListItemInsertBefore<TDualLinkListItemOperator>(CenterNode,NewItem,NewItem);
	}
	// InsertTail
	//	Insert an item item to the end of list
	void InsertTail(tItem *HeadItem,tItem *TailItem)noexcept(true){
		tItem *CenterNode=TDualLinkListItemOperator::LinkNode(fCenterLink);
		cnLib_ASSERT(SingleLinkedListItemIsConnected<TDualLinkListItemOperator>(CenterNode,HeadItem)==false);
		cnLib_ASSERT(DualLinkedListItemIsConnected<TDualLinkListItemOperator>(HeadItem,TailItem));
		DualLinkedListItemInsertBefore<TDualLinkListItemOperator>(CenterNode,HeadItem,TailItem);
	}

	tItem* Remove(tIterator RemoveHead,tIterator RemoveTail)noexcept(true){
		tItem *CenterNode=TDualLinkListItemOperator::LinkNode(fCenterLink);
		tItem *RemoveHeadItem=*RemoveHead;
		if(RemoveHeadItem==CenterNode){
			RemoveHeadItem=TDualLinkListItemOperator::GetNext(CenterNode);
			if(RemoveHeadItem==CenterNode)
				return nullptr;
		}
		tItem *RemoveTailItem=*RemoveTail;
		if(RemoveTailItem==CenterNode){
			RemoveTailItem=TDualLinkListItemOperator::GetPrev(CenterNode);
		}
		cnLib_ASSERT(SingleLinkedListItemIsConnected<TDualLinkListItemOperator>(TDualLinkListItemOperator::LinkNode(fCenterLink),RemoveHeadItem));
		cnLib_ASSERT(SingleLinkedListItemIsConnected<TDualLinkListItemOperator>(RemoveHeadItem,RemoveTailItem));
		DualLinkedListItemRemove<TDualLinkListItemOperator>(RemoveHeadItem,RemoveTailItem);
		return RemoveHeadItem;
	}
	tItem* Remove(tIterator RemovePos)noexcept(true){
		tItem *CenterNode=TDualLinkListItemOperator::LinkNode(fCenterLink);
		tItem *RemoveItem=*RemovePos;
		if(RemoveItem==CenterNode)
			return nullptr;
		cnLib_ASSERT(SingleLinkedListItemIsConnected<TDualLinkListItemOperator>(CenterNode,*RemovePos));

		DualLinkedListItemRemove<TDualLinkListItemOperator>(RemoveItem,RemoveItem);
		return RemoveItem;
	}

	// RemoveBefore
	//	Remove item and its following
	// RemovePos	position of item to remove
	// RemoveCount	number of item to remove
	tItem* RemoveBefore(tIterator RemovePos,uIntn RemoveCount)noexcept(true){
		if(RemoveCount==0)
			return nullptr;
		tItem *CenterNode=TDualLinkListItemOperator::LinkNode(fCenterLink);
		cnLib_ASSERT(SingleLinkedListItemIsConnected<TDualLinkListItemOperator>(CenterNode,*RemovePos));
		tItem *RemoveTailItem=*RemovePos;
		if(RemoveTailItem==CenterNode){
			if(--RemoveCount==0)
				return nullptr;
			RemoveTailItem=TDualLinkListItemOperator::GetPrev(CenterNode);
			if(RemoveTailItem==CenterNode){
				return nullptr;
			}
		}
		tItem *RemoveHeadItem=DualLinkedListItemFindBefore<TDualLinkListItemOperator>(RemoveTailItem,RemoveCount,CenterNode);
		DualLinkedListItemRemove<TDualLinkListItemOperator>(RemoveHeadItem,RemoveTailItem);
		return RemoveHeadItem;
	}

	// RemoveAfter
	//	Remove item and its following
	// RemovePos	position of item to remove
	// RemoveCount	number of item to remove
	tItem* RemoveAfter(tIterator RemovePos,uIntn RemoveCount)noexcept(true){
		if(RemoveCount==0)
			return nullptr;
		auto CenterNode=TDualLinkListItemOperator::LinkNode(fCenterLink);
		cnLib_ASSERT(SingleLinkedListItemIsConnected<TDualLinkListItemOperator>(CenterNode,*RemovePos));
		tItem *RemoveHeadItem=*RemovePos;
		if(RemoveHeadItem==CenterNode){
			if(--RemoveCount==0)
				return nullptr;
			RemoveHeadItem=TDualLinkListItemOperator::GetNext(CenterNode);
			if(RemoveHeadItem==CenterNode){
				return nullptr;
			}
		}
		tItem *RemoveTailItem=DualLinkedListItemFindAfter<TDualLinkListItemOperator>(RemoveHeadItem,RemoveCount,CenterNode);
		DualLinkedListItemRemove<TDualLinkListItemOperator>(RemoveHeadItem,RemoveTailItem);
		return RemoveHeadItem;
	}

	// RemoveHead
	//	Remove first item of the list
	tItem* RemoveHead(void)noexcept(true){
		tItem *CenterNode=TDualLinkListItemOperator::LinkNode(fCenterLink);
		tItem *RemoveItem=TDualLinkListItemOperator::GetNext(CenterNode);
		if(RemoveItem==CenterNode)
			return nullptr;
		DualLinkedListItemRemove<TDualLinkListItemOperator>(RemoveItem,RemoveItem);
		return RemoveItem;
	}
	// RemoveTail
	//	Remove last items of the list
	tItem* RemoveTail(void)noexcept(true){
		tItem *CenterNode=TDualLinkListItemOperator::LinkNode(fCenterLink);
		tItem *RemoveItem=TDualLinkListItemOperator::GetPrev(CenterNode);
		if(RemoveItem==CenterNode)
			return nullptr;
		DualLinkedListItemRemove<TDualLinkListItemOperator>(RemoveItem,RemoveItem);
		return RemoveItem;
	}
	// RemoveHead
	//	Remove items from the beginning of list
	// Count	number of item to remove
	tItem* RemoveHead(uIntn RemoveCount)noexcept(true){
		tItem *CenterNode=TDualLinkListItemOperator::LinkNode(fCenterLink);
		tItem *RemoveHeadItem=TDualLinkListItemOperator::GetNext(CenterNode);
		if(RemoveHeadItem==CenterNode){
			return nullptr;
		}
		tItem *RemoveTailItem=DualLinkedListItemFindAfter<TDualLinkListItemOperator>(RemoveHeadItem,RemoveCount,CenterNode);
		DualLinkedListItemRemove<TDualLinkListItemOperator>(RemoveHeadItem,RemoveTailItem);
		return RemoveHeadItem;
	}
	// RemoveTail
	//	Remove items from the end of list
	// Count	number of item to remove
	tItem* RemoveTail(uIntn RemoveCount)noexcept(true){
		tItem *CenterNode=TDualLinkListItemOperator::LinkNode(fCenterLink);
		tItem *RemoveTailItem=TDualLinkListItemOperator::GetPrev(CenterNode);
		if(RemoveTailItem==CenterNode)
			return nullptr;
		tItem *RemoveHeadItem=DualLinkedListItemFindBefore<TDualLinkListItemOperator>(RemoveTailItem,RemoveCount,CenterNode);
		DualLinkedListItemRemove<TDualLinkListItemOperator>(RemoveHeadItem,RemoveTailItem);
		return RemoveHeadItem;
	}

	void Remove(tItem *Item)noexcept(true){
		Remove(tIterator(Item));
	}
	void Remove(tItem *RemoveHead,tItem *RemoveTail)noexcept(true){
		Remove(tIterator(RemoveHead),tIterator(RemoveTail));
	}
protected:
	typename TDualLinkListItemOperator::tLinkInstance fCenterLink;

	void Reset(void)noexcept(true){
		tItem *CenterNode=TDualLinkListItemOperator::LinkNode(fCenterLink);
		TDualLinkListItemOperator::SetNext(CenterNode,CenterNode);
		TDualLinkListItemOperator::SetPrev(CenterNode,CenterNode);
	}
};
//---------------------------------------------------------------------------
template<class TDualLinkListItemOperator,class TDualLinkedItemContentOperator,class TContent>
struct cDualLinkedIterator
{
	typedef typename cnVar::TCopyCV<typename TDualLinkListItemOperator::tItem,TContent>::Type tItem;
	tItem *ItemPtr;

	cDualLinkedIterator()noexcept(true){}
	cDualLinkedIterator(tItem *p)noexcept(true):ItemPtr(p){}


	operator tItem*()const noexcept(true){
		return ItemPtr;
	}
	operator TContent*()const noexcept(true){
		return TDualLinkedItemContentOperator::ContentPtr(ItemPtr);
	}

	TContent& operator *(void)const noexcept(true){
		return *TDualLinkedItemContentOperator::ContentPtr(ItemPtr);
	}
	TContent* operator ->(void)const noexcept(true){
		return TDualLinkedItemContentOperator::ContentPtr(ItemPtr);
	}

	bool operator ==(const cDualLinkedIterator &Dest)const noexcept(true){
		return this->ItemPtr==Dest.ItemPtr;
	}
	bool operator !=(const cDualLinkedIterator &Dest)const noexcept(true){
		return this->ItemPtr!=Dest.ItemPtr;
	}

	cDualLinkedIterator& operator ++(void)noexcept(true){
		this->ItemPtr=TDualLinkListItemOperator::GetNext(this->ItemPtr);
		return *this;
	}
	void operator ++(int)noexcept(true){	operator ++();	}
	cDualLinkedIterator& operator --(void)noexcept(true){
		this->ItemPtr=TDualLinkListItemOperator::GetPrev(this->ItemPtr);
		return *this;
	}
	void operator --(int)noexcept(true){	operator --();	}
};
//---------------------------------------------------------------------------
template<class TDualLinkListItemOperator,class TDualLinkedItemContentOperator>
class cLinkList
{
public:
	typedef typename TDualLinkListItemOperator::tItem tItem;

	typedef cDualLinkedIterator<TDualLinkListItemOperator,TDualLinkedItemContentOperator,typename TDualLinkedItemContentOperator::tContent> tIterator;
	typedef cDualLinkedIterator<TDualLinkListItemOperator,TDualLinkedItemContentOperator,typename TDualLinkedItemContentOperator::tContent const> tConstIterator;

	cLinkList()noexcept(true){
		Reset();
	}
	~cLinkList()noexcept(true){
		Clear();
	}
	cLinkList(const cLinkList &Src)noexcept(true){
		tItem *NewChain=DualLinkedListItemDuplicateRing(Src.begin(),Src.end());
		if(NewChain==nullptr){
			Reset();
		}
		else{
			tItem *CenterNode=TDualLinkListItemOperator::LinkNode(fCenterLink);
			DualLinkedListItemInsertBefore<TDualLinkListItemOperator>(NewChain,CenterNode,CenterNode);
		}
	}

	// = copy
	cLinkList& operator =(const cLinkList &Src)noexcept(true){
		if(this!=&Src){
			DeleteAll();
			tItem *NewChain=DualLinkedListItemDuplicateRing(Src.begin(),Src.end());
			if(NewChain==nullptr){
				Reset();
			}
			else{
				auto CenterNode=TDualLinkListItemOperator::LinkNode(fCenterLink);
				DualLinkedListItemInsertBefore<TDualLinkListItemOperator>(NewChain,CenterNode,CenterNode);
			}
		}
		return *this;
	}

#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L
	cLinkList(cLinkList &&Src)noexcept(true){
		auto SrcCenterNode=TDualLinkListItemOperator::LinkNode(Src.fCenterLink);
		auto CenterNode=TDualLinkListItemOperator::LinkNode(fCenterLink);
		TDualLinkListItemOperator::SetNext(CenterNode,TDualLinkListItemOperator::GetNext(SrcCenterNode));
		TDualLinkListItemOperator::SetPrev(CenterNode,TDualLinkListItemOperator::GetPrev(SrcCenterNode));
		Src.Reset();
	}

	// = move
	cLinkList& operator =(cLinkList &&Src)noexcept(true){
		DeleteAll();
		auto SrcCenterNode=TDualLinkListItemOperator::LinkNode(Src.fCenterLink);
		auto CenterNode=TDualLinkListItemOperator::LinkNode(fCenterLink);
		TDualLinkListItemOperator::SetNext(CenterNode,TDualLinkListItemOperator::GetNext(SrcCenterNode));
		TDualLinkListItemOperator::SetPrev(CenterNode,TDualLinkListItemOperator::GetPrev(SrcCenterNode));
		Src.Reset();
		return *this;
	}
#endif	// cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

	bool IsEmpty(void)const noexcept(true){
		tItem *CenterNode=TDualLinkListItemOperator::LinkNode(fCenterLink);
		return TDualLinkListItemOperator::GetNext(CenterNode)==CenterNode;
	}

	// GetCount
	// return number items in the list
    uIntn GetCount(void)const noexcept(true){
		uIntn Count=0;
		auto CenterNode=TDualLinkListItemOperator::LinkNode(fCenterLink);
		auto Node=TDualLinkListItemOperator::GetNext(CenterNode);
		while(Node!=CenterNode){
			Count++;
			Node=TDualLinkListItemOperator::GetNext(Node);
		}
		return Count;
	}


	// for range
	tIterator	begin(void)noexcept(true){	return tIterator(TDualLinkListItemOperator::GetNext(TDualLinkListItemOperator::LinkNode(fCenterLink)));	}
	tIterator	end(void)noexcept(true){		return tIterator(TDualLinkListItemOperator::LinkNode(fCenterLink));	}
	tConstIterator	cbegin(void)const noexcept(true){	return tConstIterator(TDualLinkListItemOperator::GetNext(TDualLinkListItemOperator::LinkNode(fCenterLink)));	}
	tConstIterator	cend(void)const noexcept(true){	return tConstIterator(TDualLinkListItemOperator::LinkNode(fCenterLink));	}

	// Clear
	//	remove all items
	void Clear(void)noexcept(true){
		DeleteAll();
		tItem *CenterNode=TDualLinkListItemOperator::LinkNode(fCenterLink);
		TDualLinkListItemOperator::SetNext(CenterNode,CenterNode);
		TDualLinkListItemOperator::SetPrev(CenterNode,CenterNode);
	}

	// Insert
	//	Insert an item before the specifed item
	template<class...VT>
	tIterator InsertBefore(tIterator Pos,VT cnLib_UREF...ItemContent)noexcept(true){
		auto NewItem=TDualLinkListItemOperator::Create(cnLib_UREFCAST(VT)(ItemContent)...);
		DualLinkedListItemInsertBefore<TDualLinkListItemOperator>(Pos,NewItem,NewItem);
		return NewItem;
	}

	// Insert
	//	Insert an item before the specifed item
	template<class...VT>
	tIterator InsertAfter(tIterator Pos,VT cnLib_UREF...ItemContent)noexcept(true){
		auto NewItem=TDualLinkedItemContentOperator::CreateNode(cnLib_UREFCAST(VT)(ItemContent)...);
		DualLinkedListItemInsertAfter<TDualLinkListItemOperator>(Pos,NewItem,NewItem);
		return NewItem;
	}
	// InsertHead
	//	Insert an item to the beginning of the list
	template<class...VT>
	tIterator InsertHead(VT cnLib_UREF...ItemContent)noexcept(true){
		tItem *CenterNode=TDualLinkListItemOperator::LinkNode(fCenterLink);
		auto NewItem=TDualLinkedItemContentOperator::CreateNode(cnLib_UREFCAST(VT)(ItemContent)...);
		DualLinkedListItemInsertAfter<TDualLinkListItemOperator>(CenterNode,NewItem,NewItem);
		return NewItem;
	}
	// InsertTail
	//	Insert an item item to the end of list
	template<class...VT>
	tIterator InsertTail(VT cnLib_UREF...ItemContent)noexcept(true){
		tItem *CenterNode=TDualLinkListItemOperator::LinkNode(fCenterLink);
		auto NewItem=TDualLinkedItemContentOperator::CreateNode(cnLib_UREFCAST(VT)(ItemContent)...);
		DualLinkedListItemInsertBefore<TDualLinkListItemOperator>(CenterNode,NewItem,NewItem);
		return NewItem;
	}


	void Remove(tIterator RemoveHead,tIterator RemoveTail)noexcept(true){
		tItem *CenterNode=TDualLinkListItemOperator::LinkNode(fCenterLink);
		tItem *RemoveHeadItem=RemoveHead;
		cnLib_ASSERT(SingleLinkedListItemIsConnected<TDualLinkListItemOperator>(TDualLinkListItemOperator::LinkNode(fCenterLink),RemoveHeadItem));
		if(RemoveHeadItem==CenterNode){
			RemoveHeadItem=TDualLinkListItemOperator::GetNext(RemoveHeadItem);
			if(RemoveHeadItem==CenterNode){
				return;
			}
		}
		tItem *RemoveTailItem=RemoveTail;
		cnLib_ASSERT(SingleLinkedListItemIsConnected<TDualLinkListItemOperator>(RemoveHeadItem,RemoveTailItem));
		if(RemoveTailItem==CenterNode){
			RemoveTailItem=TDualLinkListItemOperator::GetPrev(RemoveTailItem);
		}
		DualLinkedListItemRemove<TDualLinkListItemOperator>(RemoveHeadItem,RemoveTailItem);
		SingleLinkedListItemDeleteChain<TDualLinkListItemOperator,TDualLinkedItemContentOperator>(RemoveHeadItem,RemoveTailItem);
	}
	bool Remove(tIterator RemovePos)noexcept(true){
		tItem *RemoveItem=RemovePos;
		tItem *CenterNode=TDualLinkListItemOperator::LinkNode(fCenterLink);
		if(RemoveItem==CenterNode)
			return false;
		cnLib_ASSERT(SingleLinkedListItemIsConnected<TDualLinkListItemOperator>(CenterNode,RemovePos));
		DualLinkedListItemRemove<TDualLinkListItemOperator>(RemoveItem,RemoveItem);
		TDualLinkedItemContentOperator::DestroyNode(RemoveItem);
		return true;
	}

	// RemoveBefore
	//	Remove item and its following
	// RemovePos	position of item to remove
	// RemoveCount	number of item to remove
	uIntn RemoveBefore(tIterator RemovePos,uIntn RemoveCount)noexcept(true){
		if(RemoveCount==0)
			return 0;
		tItem *CenterNode=TDualLinkListItemOperator::LinkNode(fCenterLink);
		tItem *RemoveTailItem=RemovePos;
		cnLib_ASSERT(SingleLinkedListItemIsConnected<TDualLinkListItemOperator>(CenterNode,RemoveTailItem));
		if(RemoveTailItem==CenterNode){
			if(--RemoveCount==0)
				return 0;
			RemoveTailItem=TDualLinkListItemOperator::GetPrev(RemoveTailItem);
			if(RemoveTailItem==CenterNode)
				return 0;
		}
		tItem *RemoveHeadItem=DualLinkedListItemFindBefore<TDualLinkListItemOperator>(RemoveTailItem,RemoveCount,CenterNode);

		DualLinkedListItemRemove<TDualLinkListItemOperator>(RemoveHeadItem,RemoveTailItem);
		return SingleLinkedListItemCountDeleteChain<TDualLinkListItemOperator,TDualLinkedItemContentOperator>(RemoveHeadItem,RemoveTailItem);
	}

	// RemoveAfter
	//	Remove item and its following
	// RemovePos	position of item to remove
	// RemoveCount	number of item to remove
	uIntn RemoveAfter(tIterator RemovePos,uIntn RemoveCount)noexcept(true){
		if(RemoveCount==0)
			return 0;
		tItem *CenterNode=TDualLinkListItemOperator::LinkNode(fCenterLink);
		tItem *RemoveHeadItem=RemovePos;
		cnLib_ASSERT(SingleLinkedListItemIsConnected<TDualLinkListItemOperator>(CenterNode,RemoveHeadItem));
		if(RemoveHeadItem==CenterNode){
			if(--RemoveCount==0)
				return 0;
			RemoveHeadItem=TDualLinkListItemOperator::GetNext(RemoveHeadItem);
			if(RemoveHeadItem==CenterNode){
				return 0;
			}
		}
		tItem *RemoveTailItem=DualLinkedListItemFindAfter<TDualLinkListItemOperator>(RemoveHeadItem,RemoveCount,CenterNode);

		DualLinkedListItemRemove<TDualLinkListItemOperator>(RemoveHeadItem,RemoveTailItem);
		return SingleLinkedListItemCountDeleteChain<TDualLinkListItemOperator,TDualLinkedItemContentOperator>(RemoveHeadItem,RemoveTailItem);
	}

	// RemoveHead
	//	Remove first item of the list
	bool RemoveHead(void)noexcept(true){
		tItem *CenterNode=TDualLinkListItemOperator::LinkNode(fCenterLink);
		tItem *RemoveItem=TDualLinkListItemOperator::GetNext(CenterNode);
		if(RemoveItem==CenterNode)
			return false;
		DualLinkedListItemRemove<TDualLinkListItemOperator>(RemoveItem,RemoveItem);
		TDualLinkedItemContentOperator::DestroyNode(RemoveItem);
		return true;
	}
	// RemoveTail
	//	Remove last items of the list
	bool RemoveTail(void)noexcept(true){
		tItem *CenterNode=TDualLinkListItemOperator::LinkNode(fCenterLink);
		tItem *RemoveItem=TDualLinkListItemOperator::GetPrev(CenterNode);
		if(RemoveItem==CenterNode)
			return false;
		DualLinkedListItemRemove<TDualLinkListItemOperator>(RemoveItem,RemoveItem);
		TDualLinkedItemContentOperator::DestroyNode(RemoveItem);
		return true;
	}
	// RemoveHead
	//	Remove items from the beginning of list
	// Count	number of item to remove
	uIntn RemoveHead(uIntn RemoveCount)noexcept(true){
		tItem *CenterNode=TDualLinkListItemOperator::LinkNode(fCenterLink);
		tItem *RemoveHeadItem=TDualLinkListItemOperator::GetNext(CenterNode);
		if(RemoveHeadItem==CenterNode){
			RemoveHeadItem=TDualLinkListItemOperator::GetNext(RemoveHeadItem);
			if(RemoveHeadItem==CenterNode)
				return 0;
		}
		
		tItem *RemoveTailItem=DualLinkedListItemFindAfter<TDualLinkListItemOperator>(RemoveHeadItem,RemoveCount,CenterNode);
		DualLinkedListItemRemove<TDualLinkListItemOperator>(RemoveHeadItem,RemoveTailItem);
		return SingleLinkedListItemCountDeleteChain<TDualLinkListItemOperator,TDualLinkedItemContentOperator>(RemoveHeadItem,RemoveTailItem);
	}
	// RemoveTail
	//	Remove items from the end of list
	// Count	number of item to remove
	uIntn RemoveTail(uIntn RemoveCount)noexcept(true){
		tItem *CenterNode=TDualLinkListItemOperator::LinkNode(fCenterLink);
		tItem *RemoveTailItem=TDualLinkListItemOperator::GetPrev(CenterNode);
		if(RemoveTailItem==CenterNode){
			RemoveTailItem=TDualLinkListItemOperator::GetPrev(RemoveTailItem);
			if(RemoveTailItem==CenterNode)
				return 0;
		}
		
		tItem *RemoveHeadItem=DualLinkedListItemFindBefore<TDualLinkListItemOperator>(RemoveTailItem,RemoveCount,CenterNode);
		DualLinkedListItemRemove<TDualLinkListItemOperator>(RemoveHeadItem,RemoveTailItem);
		return SingleLinkedListItemCountDeleteChain<TDualLinkListItemOperator,TDualLinkedItemContentOperator>(RemoveHeadItem,RemoveTailItem);
	}


protected:
	typename TDualLinkListItemOperator::tLinkInstance fCenterLink;

	void Reset(void)noexcept(true){
		tItem *CenterNode=TDualLinkListItemOperator::LinkNode(fCenterLink);
		TDualLinkListItemOperator::SetNext(CenterNode,CenterNode);
		TDualLinkListItemOperator::SetPrev(CenterNode,CenterNode);
	}
	void DeleteAll(void)noexcept(true){
		tItem *CenterNode=TDualLinkListItemOperator::LinkNode(fCenterLink);
		tItem *DeleteHeadItem=TDualLinkListItemOperator::GetNext(CenterNode);
		if(DeleteHeadItem==CenterNode)
			return;
		tItem *DeleteTailItem=TDualLinkListItemOperator::GetPrev(CenterNode);
		SingleLinkedListItemDeleteChain<TDualLinkListItemOperator,TDualLinkedItemContentOperator>(DeleteHeadItem,DeleteTailItem);
	}
};
//---------------------------------------------------------------------------
template<class TColorBinaryTreeNodeOperator>
class cRBTree
{
public:
	typedef typename TColorBinaryTreeNodeOperator::tNode* tNodePtr;

	static bool SubNodeIsRed(tNodePtr Parent,bool Branch)noexcept(true){
		ufInt8 NodeColor=TColorBinaryTreeNodeOperator::NodeColorRef(Parent);
		return (NodeColor&(Branch+1))!=0;
	}

	static bool NodeIsRed(tNodePtr Node)noexcept(true){
		tNodePtr Parent=TColorBinaryTreeNodeOperator::GetParent(Node);
		return SubNodeIsRed(Parent,TColorBinaryTreeNodeOperator::GetParentBranch(Parent,Node));
	}

	cRBTree()noexcept(true){
		auto LinkNode=TColorBinaryTreeNodeOperator::LinkNode(fRootLink);
		TColorBinaryTreeNodeOperator::SetChild(LinkNode,true,LinkNode);
		TColorBinaryTreeNodeOperator::SetChildNull(LinkNode,false);
		auto &LinkColor=TColorBinaryTreeNodeOperator::NodeColorRef(LinkNode);
		LinkColor=0;
	}
	~cRBTree()noexcept(true){
		cnLib_ASSERT(GetRoot()==nullptr);
	}

	tNodePtr GetGuardLink(void)const noexcept(true){
		return TColorBinaryTreeNodeOperator::LinkNode(const_cast<typename TColorBinaryTreeNodeOperator::tLinkInstance&>(fRootLink));
	}

	tNodePtr GetRoot(void)const noexcept(true){
		return TColorBinaryTreeNodeOperator::GetChild(GetGuardLink(),false);
	}

	void InsertRoot(tNodePtr NewRoot)noexcept(true){
		auto LinkNode=TColorBinaryTreeNodeOperator::LinkNode(fRootLink);
		cnLib_ASSERT(TColorBinaryTreeNodeOperator::GetChild(LinkNode,false)==nullptr);


		TColorBinaryTreeNodeOperator::ResetNode(NewRoot);
		TColorBinaryTreeNodeOperator::SetChild(LinkNode,false,NewRoot);
	}

	void InsertNode(tNodePtr Parent,tNodePtr NewNode,bool NewNodeBranch)noexcept(true){
		TColorBinaryTreeNodeOperator::ResetNode(NewNode);

		TColorBinaryTreeNodeOperator::SetChild(Parent,NewNodeBranch,NewNode);
		SubNodeSetRed(Parent,NewNodeBranch);

		UpdateRedColorNewNode(Parent,NewNodeBranch);
	
	}
	void RemoveNode(tNodePtr Node)noexcept(true){
		auto LinkNode=TColorBinaryTreeNodeOperator::LinkNode(fRootLink);
		tNodePtr NodeParent=TColorBinaryTreeNodeOperator::GetParent(Node);
		if(NodeParent!=LinkNode){
			cnLib_ASSERT(BinaryTreeIsNodeInBranch<TColorBinaryTreeNodeOperator>(TColorBinaryTreeNodeOperator::GetChild(LinkNode,false),Node));
			return RemoveSubNode(NodeParent,Node);
		}
		// removing root node
		tNodePtr DeleteLeft=TColorBinaryTreeNodeOperator::GetChild(Node,false);
		tNodePtr DeleteRight=TColorBinaryTreeNodeOperator::GetChild(Node,true);
		if(DeleteLeft==nullptr){
			if(DeleteRight==nullptr){
				// leaf
				TColorBinaryTreeNodeOperator::SetChildNull(LinkNode,false);
				return;
			}
			// only right node
			TColorBinaryTreeNodeOperator::SetChild(LinkNode,false,DeleteRight);
			return;
		}
		if(DeleteRight==nullptr){
			// only left node
			TColorBinaryTreeNodeOperator::SetChild(LinkNode,false,DeleteLeft);
			return;
		}
		// both node
		// find leftmode node of right child
		NodeParent=ReplaceWithLeftmost(NodeParent,false,Node,DeleteRight);
		return RemoveSubNode(NodeParent,Node);
	}

	tNodePtr SwapTree(void)noexcept(true){
		tNodePtr LinkNode=TColorBinaryTreeNodeOperator::LinkNode(fRootLink);
		tNodePtr Root=TColorBinaryTreeNodeOperator::GetChild(LinkNode,false);
		if(Root==nullptr){
			return nullptr;
		}
		TColorBinaryTreeNodeOperator::SetChildNull(LinkNode,false);
		TColorBinaryTreeNodeOperator::SetParentNull(Root);

		return Root;
	}

private:
	typename TColorBinaryTreeNodeOperator::tLinkInstance fRootLink;

	static void SubNodeSetRed(tNodePtr Parent,bool Branch)noexcept(true){
		ufInt8 &NodeColor=TColorBinaryTreeNodeOperator::NodeColorRef(Parent);
		NodeColor|=Branch+1;
	}
	
	static void SubNodeSetBlack(tNodePtr Parent,bool Branch)noexcept(true){
		ufInt8 &NodeColor=TColorBinaryTreeNodeOperator::NodeColorRef(Parent);
		NodeColor&=~(Branch+1);
	}
	static void SubNodeSetBothRed(tNodePtr Parent)noexcept(true){
		ufInt8 &NodeColor=TColorBinaryTreeNodeOperator::NodeColorRef(Parent);
		NodeColor=3;
	}
	static void SubNodeSetBothBlack(tNodePtr Parent)noexcept(true){
		ufInt8 &NodeColor=TColorBinaryTreeNodeOperator::NodeColorRef(Parent);
		NodeColor=0;
	}
	static void NodeSetRed(tNodePtr Node)noexcept(true){
		tNodePtr Parent=TColorBinaryTreeNodeOperator::GetParent(Node);
		return SubNodeSetRed(Parent,TColorBinaryTreeNodeOperator::GetParentBranch(Parent,Node));
	}
	static void NodeSetBlack(tNodePtr Node)noexcept(true){
		tNodePtr Parent=TColorBinaryTreeNodeOperator::GetParent(Node);
		return SubNodeSetBlack(Parent,TColorBinaryTreeNodeOperator::GetParentBranch(Parent,Node));
	}

	static void SubNodeCheckNullColor(tNodePtr Parent)noexcept(true){
		ufInt8 ColorMask=(TColorBinaryTreeNodeOperator::GetChild(Parent,false)!=nullptr)
						| ((TColorBinaryTreeNodeOperator::GetChild(Parent,true)!=nullptr)<<1);
		ufInt8 &NodeColor=TColorBinaryTreeNodeOperator::NodeColorRef(Parent);
		NodeColor&=ColorMask;
	}

	static void SetChildCheckNull(tNodePtr Node,bool ChildBranch,tNodePtr Child)noexcept(true){
		if(Child==nullptr){
			TColorBinaryTreeNodeOperator::SetChildNull(Node,ChildBranch);
		}
		else{
			TColorBinaryTreeNodeOperator::SetChild(Node,ChildBranch,Child);
		}
	}


	void RotateNodeLeft(tNodePtr Node)noexcept(true){
		//	 N			|		NR
		// NL	NR		|	N		NRR
		//	  NRL  NRR	| NL  NRL

		// NodeRight=Node->Right
		tNodePtr NodeRight=TColorBinaryTreeNodeOperator::GetChild(Node,true);
		// Top=Node->Parent
		tNodePtr Top=TColorBinaryTreeNodeOperator::GetParent(Node);
		bool NodeBranch=TColorBinaryTreeNodeOperator::GetParentBranch(Top,Node);

		TColorBinaryTreeNodeOperator::SetChild(Top,NodeBranch,NodeRight);

		// Node->Right=NodeRight->Left;
		SetChildCheckNull(Node,true,TColorBinaryTreeNodeOperator::GetChild(NodeRight,false));
		// NodeRight->Left=Node;
		TColorBinaryTreeNodeOperator::SetChild(NodeRight,false,Node);

	}

	void RotateNodeRight(tNodePtr Node)noexcept(true){
		//		  N		|		NL
		//    NL	NR	|	NLL		N
		// NLL  NLR		|		 NLR  NR

		// NodeLeft=Node->Left
		tNodePtr NodeLeft=TColorBinaryTreeNodeOperator::GetChild(Node,false);
		// Top=Node->Parent
		tNodePtr Top=TColorBinaryTreeNodeOperator::GetParent(Node);
		bool NodeBranch=TColorBinaryTreeNodeOperator::GetParentBranch(Top,Node);

		TColorBinaryTreeNodeOperator::SetChild(Top,NodeBranch,NodeLeft);

		// Node->Left=NodeLeft->Right;
		SetChildCheckNull(Node,false,TColorBinaryTreeNodeOperator::GetChild(NodeLeft,true));
		// NodeLeft->Right=Node;
		TColorBinaryTreeNodeOperator::SetChild(NodeLeft,true,Node);
	}

	void RotateChildLeftNodeRight(tNodePtr Node)noexcept(true){
		//	     N		|	         N		|			LR
		//	 L     R	|	   LR		R	|	   L	  	   N
		// LL  LR   	|	L	 LRR		|	LL LRL		LRR	 R
		//	 LRL LRR	| LL LRL			|

		// NodeL=Node->Left
		tNodePtr NodeL=TColorBinaryTreeNodeOperator::GetChild(Node,false);
		tNodePtr NodeLR=TColorBinaryTreeNodeOperator::GetChild(NodeL,true);
		// Top=Node->Parent
		tNodePtr Top=TColorBinaryTreeNodeOperator::GetParent(Node);
		bool NodeBranch=TColorBinaryTreeNodeOperator::GetParentBranch(Top,Node);

		TColorBinaryTreeNodeOperator::SetChild(Top,NodeBranch,NodeLR);

		// NodeL->Right=NodeLR->Left
		SetChildCheckNull(NodeL,true,TColorBinaryTreeNodeOperator::GetChild(NodeLR,false));

		// Node->Left=NodeLR->Right
		SetChildCheckNull(Node,false,TColorBinaryTreeNodeOperator::GetChild(NodeLR,true));

		// NodeLR->Left=NodeL
		TColorBinaryTreeNodeOperator::SetChild(NodeLR,false,NodeL);
		// NodeLR->Right=Node
		TColorBinaryTreeNodeOperator::SetChild(NodeLR,true,Node);
	}

	void RotateChildRightNodeLeft(tNodePtr Node)noexcept(true){
		//	   N		|	N			|		   RL
		//	 L    R		|  L	RL		|	   N		R
		//		RL  RR	|	 RLL   R	|	L	RLL	 RLR RR
		//	 RLL RLR	|		RLR RR	|

		// NodeR=Node->Right
		tNodePtr NodeR=TColorBinaryTreeNodeOperator::GetChild(Node,true);
		// NodeRL=NodeR->Left
		tNodePtr NodeRL=TColorBinaryTreeNodeOperator::GetChild(NodeR,false);
		// Top=Node->Parent
		tNodePtr Top=TColorBinaryTreeNodeOperator::GetParent(Node);
		bool NodeBranch=TColorBinaryTreeNodeOperator::GetParentBranch(Top,Node);

		TColorBinaryTreeNodeOperator::SetChild(Top,NodeBranch,NodeRL);

		// NodeR->Left=NodeRL->Right
		SetChildCheckNull(NodeR,false,TColorBinaryTreeNodeOperator::GetChild(NodeRL,true));

		// Node->Right=NodeRL->Left
		SetChildCheckNull(Node,true,TColorBinaryTreeNodeOperator::GetChild(NodeRL,false));

		// NodeRL->Right=NodeR
		TColorBinaryTreeNodeOperator::SetChild(NodeRL,true,NodeR);
		// NodeRL->Left=Node
		TColorBinaryTreeNodeOperator::SetChild(NodeRL,false,Node);
	}

	void UpdateRedColorNewNode(tNodePtr Parent,bool NewNodeBranch)noexcept(true){
		tNodePtr Grandparent=TColorBinaryTreeNodeOperator::GetParent(Parent);
		auto LinkNode=TColorBinaryTreeNodeOperator::LinkNode(fRootLink);
		if(Grandparent==LinkNode){
			// parent is root
			return;
		}


		bool ParentNodeBranch=TColorBinaryTreeNodeOperator::GetParentBranch(Grandparent,Parent);
		if(SubNodeIsRed(Grandparent,ParentNodeBranch)==false){
			// parent is black
			// done
			return;
		}

		bool UncleIsRed=SubNodeIsRed(Grandparent,!ParentNodeBranch);

		if(UncleIsRed){
			// change both black and color grand parent as red
			SubNodeSetBothBlack(Grandparent);

			tNodePtr GPP=TColorBinaryTreeNodeOperator::GetParent(Grandparent);
			if(GPP==LinkNode){
				// grandparent is the root, done
				return;
			}
			// set grand parent to red

			bool GrandparentNodeBranch=TColorBinaryTreeNodeOperator::GetParentBranch(GPP,Grandparent);
			SubNodeSetRed(GPP,GrandparentNodeBranch);
			return UpdateRedColorNewNode(GPP,GrandparentNodeBranch);
		}

		// uncle is black
		// rotate tree
		if(NewNodeBranch){
			if(ParentNodeBranch){
				// RR case - rotate Grandparent left
				//	 G			|		P
				// U	P|r		|	G|r		New|r
				//	  S  New|r	|  U  S

				RotateNodeLeft(Grandparent);

				SubNodeSetBothRed(Parent);
				SubNodeSetBothBlack(Grandparent);
			}
			else{
				// LR case - rotate Parent left then rotate Grandparent right
				//	      G		|	          G		|		New
				//	 P|r    U	|	   New|r	U	|	  P|r	  G|r
				//  S  New|r   	|	P|r	   NR		|	S   NL	NR	U
				//	  NL NR		|  S  NL			|
				
				tNodePtr NewNode=TColorBinaryTreeNodeOperator::GetChild(Parent,true);
				cnLib_ASSERT(NewNode!=nullptr);
				RotateChildLeftNodeRight(Grandparent);

				SubNodeSetBothRed(NewNode);
				SubNodeSetBothBlack(Parent);
				SubNodeSetBothBlack(Grandparent);
			}
		}
		else{
			if(ParentNodeBranch){
				// RL case - rotate Parent right then rotate Grandparent left
				//	  G			|	   G			|		New
				//	U	 P|r	|	U    New|r		|	  G|r	  P|r
				//	  New|r	 S	|		NL   P|r	|	U   NL	NR	S
				//	   NL NR	|  			NR  S	|
				tNodePtr NewNode=TColorBinaryTreeNodeOperator::GetChild(Parent,false);

				RotateChildRightNodeLeft(Grandparent);

				SubNodeSetBothRed(NewNode);
				SubNodeSetBothBlack(Parent);
				SubNodeSetBothBlack(Grandparent);
			}
			else{
				// LL case  - rotate Parent right
				//	       G	|			P
				//	  P|r     U	| New|r			G|r
				// New|r  S  	|	 	       S   U

				RotateNodeRight(Grandparent);

				SubNodeSetBothRed(Parent);
				SubNodeSetBothBlack(Grandparent);
			}
		}


	}

	void RemoveSubNode(tNodePtr DeleteParent,tNodePtr DeleteNode)noexcept(true){

		bool DeleteNodeBranch=TColorBinaryTreeNodeOperator::GetParentBranch(DeleteParent,DeleteNode);
		bool DeleteNodeIsRed=SubNodeIsRed(DeleteParent,DeleteNodeBranch);

		tNodePtr DeleteLeft=TColorBinaryTreeNodeOperator::GetChild(DeleteNode,false);
		tNodePtr DeleteRight=TColorBinaryTreeNodeOperator::GetChild(DeleteNode,true);
		if(DeleteLeft==nullptr){
			if(DeleteRight==nullptr){
				// leaf
				if(DeleteNodeIsRed){
					SubNodeSetBlack(DeleteParent,DeleteNodeBranch);
					if(TColorBinaryTreeNodeOperator::GetChild(DeleteParent,!DeleteNodeBranch)!=nullptr){
						SubNodeSetRed(DeleteParent,!DeleteNodeBranch);
					}
				}
				else{
					UpdateBlackColorRemoveNode(DeleteNode);
				}
				TColorBinaryTreeNodeOperator::SetChildNull(DeleteParent,DeleteNodeBranch);

				return;
			}
			// only right node
			bool ReplacementIsRed=SubNodeIsRed(DeleteNode,true);
			TColorBinaryTreeNodeOperator::SetChild(DeleteParent,DeleteNodeBranch,DeleteRight);
			if(ReplacementIsRed || DeleteNodeIsRed){
				SubNodeSetBlack(DeleteParent,DeleteNodeBranch);
			}
			else{
				UpdateBlackColorRemoveNode(DeleteRight);
			}
			return;
		}
		if(DeleteRight==nullptr){
			// only left node
			bool ReplacementIsRed=SubNodeIsRed(DeleteNode,false);
			TColorBinaryTreeNodeOperator::SetChild(DeleteParent,DeleteNodeBranch,DeleteLeft);
			if(ReplacementIsRed || DeleteNodeIsRed){
				SubNodeSetBlack(DeleteParent,DeleteNodeBranch);
			}
			else{
				UpdateBlackColorRemoveNode(DeleteLeft);
			}
			return;
		}
		// both node
		// replace node with leftmost node of right child
		DeleteParent=ReplaceWithLeftmost(DeleteParent,DeleteNodeBranch,DeleteNode,DeleteRight);
		return RemoveSubNode(DeleteParent,DeleteNode);
	}

	tNodePtr ReplaceWithLeftmost(tNodePtr DeleteParent,bool DeleteNodeBranch,tNodePtr DeleteNode,tNodePtr ReplacementNode)noexcept(true){
		// ReplacementNode is right child of DeleteNode
		cnLib_ASSERT(TColorBinaryTreeNodeOperator::GetParentBranch(DeleteNode,ReplacementNode)==true);
		tNodePtr ReplaceLeft=TColorBinaryTreeNodeOperator::GetChild(ReplacementNode,false);
		if(ReplaceLeft==nullptr){
			//	   P		|	     P
			//	   D		|	     RE
			//	S     RE	|	 S      D
			//      N   RER |        N   RER
			tNodePtr ReplacementSibling=TColorBinaryTreeNodeOperator::GetChild(DeleteNode,false);

			tNodePtr ReplaceRight=TColorBinaryTreeNodeOperator::GetChild(ReplacementNode,true);

			TColorBinaryTreeNodeOperator::SetChild(DeleteParent,DeleteNodeBranch,ReplacementNode);

			SetChildCheckNull(ReplacementNode,false,ReplacementSibling);
			TColorBinaryTreeNodeOperator::SetChild(ReplacementNode,true,DeleteNode);


			TColorBinaryTreeNodeOperator::SetChildNull(DeleteNode,false);
			SetChildCheckNull(DeleteNode,true,ReplaceRight);
		
			// new parent
			DeleteParent=ReplacementNode;
		}
		else{
			tNodePtr ReplacementNodeParent;
			do{
				ReplacementNodeParent=ReplacementNode;
				ReplacementNode=ReplaceLeft;
			}while((ReplaceLeft=TColorBinaryTreeNodeOperator::GetChild(ReplacementNode,false))!=nullptr);
			//	    PD		|	     PD
			//	    D		|	     RE
			//	  PRE  DR	|	  PRE     DR
			//	  RE		|	  D
			//	REL RER		|	REL RER

			tNodePtr NodeDL=TColorBinaryTreeNodeOperator::GetChild(DeleteNode,false);
			tNodePtr NodeDR=TColorBinaryTreeNodeOperator::GetChild(DeleteNode,true);
			tNodePtr NodeREL=TColorBinaryTreeNodeOperator::GetChild(ReplacementNode,false);
			tNodePtr NodeRER=TColorBinaryTreeNodeOperator::GetChild(ReplacementNode,true);

			SetChildCheckNull(DeleteNode,false,NodeREL);
			SetChildCheckNull(DeleteNode,true,NodeRER);

			SetChildCheckNull(ReplacementNode,false,NodeDL);
			SetChildCheckNull(ReplacementNode,true,NodeDR);

			TColorBinaryTreeNodeOperator::SetChild(DeleteParent,DeleteNodeBranch,ReplacementNode);
			TColorBinaryTreeNodeOperator::SetChild(ReplacementNodeParent,false,DeleteNode);

			// new parent
			DeleteParent=ReplacementNodeParent;
		}

		ufInt8 &DeleteNodeSubColor=TColorBinaryTreeNodeOperator::NodeColorRef(DeleteNode);
		ufInt8 &ReplacementNodeSubColor=TColorBinaryTreeNodeOperator::NodeColorRef(ReplacementNode);

		ufInt8 TempSubColor=DeleteNodeSubColor;
		DeleteNodeSubColor=ReplacementNodeSubColor;
		ReplacementNodeSubColor=TempSubColor;
		SubNodeCheckNullColor(DeleteNode);
		SubNodeCheckNullColor(ReplacementNode);

		return DeleteParent;
	}


	void UpdateBlackColorRemoveNode(tNodePtr Node)noexcept(true){
		tNodePtr LinkNode=TColorBinaryTreeNodeOperator::LinkNode(fRootLink);
		tNodePtr Parent=TColorBinaryTreeNodeOperator::GetParent(Node);
		if(Parent==LinkNode){
			return;
		}
		bool NodeBranch=TColorBinaryTreeNodeOperator::GetParentBranch(Parent,Node);
		tNodePtr Sibling=TColorBinaryTreeNodeOperator::GetChild(Parent,!NodeBranch);
		if(Sibling==nullptr){
			return UpdateBlackColorRemoveNode(Parent);
		}
		if(SubNodeIsRed(Parent,!NodeBranch)){
			// Sibling is red
			//	    P		|	      S|b
			//	  D	  S|r	|	   P|r	SR
			//		 SL SR	|	 D	SL

			if(NodeBranch){
				// sibling at left
				RotateNodeRight(Parent);
			}
			else{
				// sibling at right
				RotateNodeLeft(Parent);
			}
			// set parent as red
			SubNodeSetRed(Sibling,NodeBranch);
			// set new sibling as black
			SubNodeSetBlack(Parent,!NodeBranch);

			// update nodes
			Sibling=TColorBinaryTreeNodeOperator::GetChild(Parent,!NodeBranch);
			// no change to NodeBranch
			// new sibling should be black
		}
		// Sibling is black

		ufInt8 SiblingSubColor=TColorBinaryTreeNodeOperator::NodeColorRef(Sibling);
		if(SiblingSubColor==0){
			// both child of sibling are black
			// set sibling red
			SubNodeSetRed(Parent,!NodeBranch);
			if(Parent==TColorBinaryTreeNodeOperator::GetChild(LinkNode,false))
				return;

			if(NodeIsRed(Parent)){
				NodeSetBlack(Parent);
				return;
			}
			return UpdateBlackColorRemoveNode(Parent);
		}
		if(NodeBranch){
			// sibling at left
			if(SiblingSubColor&1){
				bool SiblingRightColor=(SiblingSubColor&2)!=0;
				// LL case - sibling at left, sibling->left is red
				//		   P	|		S
				//	   S	 D	|	SL		P
				// SL|r	 SR|a	|		 SR|a
				RotateNodeRight(Parent);
				SubNodeSetBothBlack(Sibling);

				// restore color of SR
				if(SiblingRightColor){
					SubNodeSetRed(Parent,false);
				}
				else{
					// parent->left is still black
				}
			}
			else{
				// LR case - sibling at left, sibling->left is black and sibling->right is red
				//		 P		|	      P			|		SR|b
				//	  S		D	|	  SR|b	D		|	  S|b	P
				// SL|b	SR|r	|	 S|r			|	SL|b	 D
				//				|  SL|b				|
				
				RotateChildLeftNodeRight(Parent);

				SubNodeSetBothBlack(Sibling);
			
				// SR->left and SR->right both should still be black
				// Parent->left should still be black
			}
			return;
		}
		else{
			// sibling at right
			if(SiblingSubColor&2){
				bool SiblingLeftColor=(SiblingSubColor&1)!=0;
				// RR case - sibling at right, sibling->right is red
				//	  P			|		S
				//	D	 S		|	P		SR
				//	  SL|a SR|r	|	 SL|a

				RotateNodeLeft(Parent);
				SubNodeSetBothBlack(Sibling);

				
				// restore color of SL
				if(SiblingLeftColor){
					SubNodeSetRed(Parent,true);
				}
				else{
					// parent->right is still black
				}
			}
			else{
				// RL case - sibling at right, sibling->right is black and sibling->left is red
				//	  P			|	 P			|		SL|b
				// D	  S		| D		SL|b	|	  P	   S|b
				//	  SL|r SR|b |	 	 S|r	|	D		SR|b
				//				|		   SR|b |

				RotateChildRightNodeLeft(Parent);

				SubNodeSetBothBlack(Sibling);

				// SL->left and SL->right both should still be black
				// Parent->right should still be black
			}
			return;
		}

	}

};

//---------------------------------------------------------------------------
template<class TBinaryTreeNodeOperator,class TNode>
struct cBinaryTreeNodeIterator
{
	TNode *NodePtr;

	cBinaryTreeNodeIterator()noexcept(true){}
	explicit cBinaryTreeNodeIterator(TNode *p)noexcept(true):NodePtr(p){}


	TNode* operator *(void)const noexcept(true){
		return NodePtr;
	}
	TNode* operator ->()const noexcept(true){
		return NodePtr;
	}

	bool operator ==(const cBinaryTreeNodeIterator &Dest)const noexcept(true){
		return this->NodePtr==Dest.NodePtr;
	}
	bool operator !=(const cBinaryTreeNodeIterator &Dest)const noexcept(true){
		return this->NodePtr!=Dest.NodePtr;
	}

	cBinaryTreeNodeIterator& operator ++(void)noexcept(true){
		this->NodePtr=BinaryTreeTravelM<TBinaryTreeNodeOperator>(this->NodePtr,false);
		return *this;
	}
	void operator ++(int)noexcept(true){	operator ++();	}

	cBinaryTreeNodeIterator& operator --(void)noexcept(true){
		this->NodePtr=BinaryTreeTravelM<TBinaryTreeNodeOperator>(this->NodePtr,true);
		return *this;
	}
	void operator --(int)noexcept(true){	operator --();	}
};
//---------------------------------------------------------------------------
template<class TColorBinaryTreeNodeOperator,class TItemOrderOperator>
class cLinkItemSet
{
public:
	typedef typename TColorBinaryTreeNodeOperator::tNode tNode;

	typedef cBinaryTreeNodeIterator<TColorBinaryTreeNodeOperator,tNode> tIterator;
	typedef cBinaryTreeNodeIterator<TColorBinaryTreeNodeOperator,const tNode> tConstIterator;

	cLinkItemSet()noexcept(true):fNodeCount(0){}
	~cLinkItemSet()noexcept(true){}

	cLinkItemSet(const cLinkItemSet &Src)=delete;
	// = copy
	cLinkItemSet& operator =(const cLinkItemSet &Src)=delete;

#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

	cLinkItemSet(cLinkItemSet &&Src)noexcept(true);
	// = move
	cLinkItemSet& operator =(cLinkItemSet &&Src)noexcept(true);

#endif	// cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

	bool IsEmpty(void)const noexcept(true){
		return fTree.GetRoot()==nullptr;
	}

	// GetCount
	// return number items in the list
    uIntn GetCount(void)const noexcept(true){
    	return fNodeCount;
	}

	bool VerifyNode(tNode *Node)const noexcept(true){
		auto Root=fTree.GetRoot();
		if(Root==nullptr)
			return false;
		return BinaryTreeIsNodeInBranch<TColorBinaryTreeNodeOperator>(Root,Node);
	}

	// RemoveAll
	//	remove all items
	tNode* RemoveAll(void)noexcept(true){
		tNode *Root=fTree.SwapTree();
		fNodeCount=0;
		return Root;
	}

	template<class TDeleteNodeFunction>
	static void DeleteAllNodes(tNode *Node,TDeleteNodeFunction cnLib_UREF DeleteNodeFunction)noexcept(true){
		return BinaryTreeDeleteAll<TColorBinaryTreeNodeOperator>(Node,cnLib_UREFCAST(TDeleteNodeFunction)(DeleteNodeFunction));
	}

	tNode* GetLowwerNode(void)const noexcept(true){
		tNode *RootLink=fTree.GetGuardLink();
		tNode *Root=TColorBinaryTreeNodeOperator::GetChild(RootLink,false);
		if(Root==nullptr)
			return nullptr;
		return BinaryTreeFindSideLeaf<TColorBinaryTreeNodeOperator>(Root,false);
	}
	tNode* GetUpperNode(void)const noexcept(true){
		tNode *RootLink=fTree.GetGuardLink();
		tNode *Root=TColorBinaryTreeNodeOperator::GetChild(RootLink,false);
		if(Root==nullptr)
			return nullptr;
		return BinaryTreeFindSideLeaf<TColorBinaryTreeNodeOperator>(Root,true);
	}

	// for range
	tIterator	begin(void)const noexcept(true){
		tNode *Leftmost=GetLowwerNode();
		if(Leftmost==nullptr)
			return tIterator(fTree.GetGuardLink());
		return tIterator(Leftmost);
	}
	tIterator	end(void)const noexcept(true){
		tNode *EndNode=fTree.GetGuardLink();
		return tIterator(EndNode);
	}
	tConstIterator	cbegin(void)const noexcept(true){
		tNode *Leftmost=GetLowwerNode();
		if(Leftmost==nullptr)
			return tConstIterator(fTree.GetGuardLink());
		return tConstIterator(Leftmost);
	}
	tConstIterator	cend(void)const noexcept(true){
		tNode *EndNode=fTree.GetGuardLink();
		return tConstIterator(EndNode);
	}

	static void* IteratorToPointer(tIterator Iterator)noexcept(true){
		return static_cast<tNode*>(Iterator);
	}
	static tIterator IteratorFromPointer(void *Ptr)noexcept(true){
		return static_cast<tNode *>(Ptr);
	}

	// Insert
	//	Insert an item
	// [in]ItemValue
	// return true if the key inserted as new item
	bool Insert(tNode *NewNode)noexcept(true){
		if(IsEmpty()){
			// create node
			fTree.InsertRoot(NewNode);
			fNodeCount++;
			return true;
		}
		tNode *ResultNode;
#if cnLibrary_CPPFEATURE_DECLTYPE >= 200707L
		auto Relation=FindNode(ResultNode,*NewNode);
#else	// cnLibrary_CPPFEATURE_DECLTYPE < 200707L
		sfInt8 Relation=FindNode(ResultNode,*NewNode);
#endif	// cnLibrary_CPPFEATURE_DECLTYPE
		if(Relation==0){
			// already exists
			return false;
		}
		// insert node
		bool NodeBranch=Relation<0;	// true : CurNode<ItemValue | false : CurNode>ItemValue
		fTree.InsertNode(ResultNode,NewNode,NodeBranch);
		fNodeCount++;
		return true;
	}

    // Remove
    //	Remove a item has the same value specified in Item
	// [in]Item
    // return true if item is removed
    bool Remove(tNode *Node)noexcept(true){
    	if(VerifyNode(Node)==false)
    		return false;
		// found
		fTree.RemoveNode(Node);

		fNodeCount--;
		return true;
	}
	// Remove
    //	Remove a item at specified position
	// Iterator			iterator of item to be removed
    void Remove(tIterator Position)noexcept(true){
    	tNode *DeleteNode=*Position;
		fTree.RemoveNode(DeleteNode);

		fNodeCount--;
	}


    tIterator Find(tNode *Node)const noexcept(true){
    	if(VerifyNode(Node)==false)
    		return end();

		return tIterator(Node);
	}

	template<class T>
    tIterator Find(const T &Value)const noexcept(true){
		if(IsEmpty()){
			return end();
		}
		tNode *ResultNode;
#if cnLibrary_CPPFEATURE_DECLTYPE >= 200707L
		auto Relation=FindNode(ResultNode,Value);
#else	// cnLibrary_CPPFEATURE_DECLTYPE < 200707L
		sfInt8 Relation=FindNode(ResultNode,Value);
#endif	// cnLibrary_CPPFEATURE_DECLTYPE
		if(Relation==0){
			return tIterator(ResultNode);
		}
		return end();
	}

protected:
	cRBTree<TColorBinaryTreeNodeOperator> fTree;
	uIntn fNodeCount;

	tNode* GetSideLeaf(bool Branch)const noexcept(true){
		return BinaryTreeFindSideLeaf<TColorBinaryTreeNodeOperator>(fTree.GetRoot(),Branch);
	}

	template<class TValue>
	struct cCompareFunctor
	{
		const TValue &Value;
#if cnLibrary_CPPFEATURE_DECLTYPE >= 200707L
		auto operator () (tNode *Node)noexcept(true) -> decltype(TItemOrderOperator::Compare(*Node,Value))
#else	// cnLibrary_CPPFEATURE_DECLTYPE < 200707L
		sfInt8 operator () (tNode *Node)noexcept(true)
#endif	// cnLibrary_CPPFEATURE_DECLTYPE
		{
			return TItemOrderOperator::Compare(*Node,Value);
		}
	};
	template<class TFindValue>
#if cnLibrary_CPPFEATURE_DECLTYPE >= 200707L
	auto FindNode(tNode* &ResultNode,const TFindValue &FindValue)const noexcept(true) -> decltype(TItemOrderOperator::Compare(*ResultNode,FindValue))
#else	// cnLibrary_CPPFEATURE_DECLTYPE < 200707L
	sfInt8 FindNode(tNode* &ResultNode,const TFindValue &FindValue)const noexcept(true)
#endif	// cnLibrary_CPPFEATURE_DECLTYPE
	{
		cCompareFunctor<TFindValue> Compare={FindValue};
		ResultNode=fTree.GetRoot();
		return BinarySearchTreeFindNode<TColorBinaryTreeNodeOperator>(ResultNode,Compare);
	}

//	void VerifyTreeNode(uIntn &NodeCount,tNode *Node)const noexcept(true){
//		tNode *Left=TColorBinaryTreeNodeOperator::GetChild(Node,false);
//		tNode *Right=TColorBinaryTreeNodeOperator::GetChild(Node,true);
//		if(Left!=nullptr){
//			NodeCount++;
//			cnLib_ASSERT(TColorBinaryTreeNodeOperator::GetParentBranch(Node,Left)==false);
//			cnLib_ASSERT(TItemOrderOperator::Compare(*Node,*Left)>0);
//			VerifyTreeNode(NodeCount,Left);
//		}
//		if(Right!=nullptr){
//			NodeCount++;
//			cnLib_ASSERT(TColorBinaryTreeNodeOperator::GetParentBranch(Node,Right)==true);
//			cnLib_ASSERT(TItemOrderOperator::Compare(*Node,*Right)<0);
//			VerifyTreeNode(NodeCount,Right);
//		}
//	}
//	void VerifyTree(void)const noexcept(true){
//		tNode *Node=fTree.GetRoot();
//		if(Node==nullptr){
//			cnLib_ASSERT(fNodeCount==0);
//			return;
//		}
//		uIntn NodeCount=1;
//		VerifyTreeNode(NodeCount,Node);
//		cnLib_ASSERT(fNodeCount==NodeCount);
//	}
};
//---------------------------------------------------------------------------
//struct TBinaryTreeNodeContentOperator
//{
//	typedef tItem;
//	typedef tNode;
//
//	static tItem* ContentPtr(tNode *Node);
//	template<class...TArgs>
//	static tNode* CreateNode(TArgs&&...Args);
//	static void DestroyNode(tNode *Node);
//};
//---------------------------------------------------------------------------
template<class TBinaryTreeNodeOperator,class TBinaryTreeNodeContentOperator,class TContent>
struct cBinaryTreeNodeContentIterator
{
	typedef typename TBinaryTreeNodeOperator::tNode tNode;
	tNode *NodePtr;

	cBinaryTreeNodeContentIterator()noexcept(true){}
	cBinaryTreeNodeContentIterator(tNode *p)noexcept(true):NodePtr(p){}

	operator tNode*()const noexcept(true){	return NodePtr;	}

	operator TContent*()const noexcept(true){	return TBinaryTreeNodeContentOperator::ContentPtr(NodePtr);	}

	TContent& operator *(void)const noexcept(true){
		return *TBinaryTreeNodeContentOperator::ContentPtr(NodePtr);
	}
	TContent* operator ->(void)const noexcept(true){
		return TBinaryTreeNodeContentOperator::ContentPtr(NodePtr);
	}

	bool operator ==(const cBinaryTreeNodeContentIterator &Dest)const noexcept(true){
		return this->NodePtr==Dest.NodePtr;
	}
	bool operator !=(const cBinaryTreeNodeContentIterator &Dest)const noexcept(true){
		return this->NodePtr!=Dest.NodePtr;
	}

	cBinaryTreeNodeContentIterator& operator ++(void)noexcept(true){
		this->NodePtr=BinaryTreeTravelM<TBinaryTreeNodeOperator>(this->NodePtr,false);
		return *this;
	}
	void operator ++(int)noexcept(true){	operator ++();	}
	
	cBinaryTreeNodeContentIterator& operator --(void)noexcept(true){
		this->NodePtr=BinaryTreeTravelM<TBinaryTreeNodeOperator>(this->NodePtr,true);
		return *this;
	}
	void operator --(int)noexcept(true){	operator --();	}
};
//---------------------------------------------------------------------------
template<class TColorBinaryTreeNodeOperator,class TBinaryTreeNodeContentOperator,class TItemOrderOperator>
class cLinkSet
{
public:
	typedef typename TColorBinaryTreeNodeOperator::tNode tNode;
	typedef typename TBinaryTreeNodeContentOperator::tContent tContent;

	typedef cBinaryTreeNodeContentIterator<TColorBinaryTreeNodeOperator,TBinaryTreeNodeContentOperator,tContent> tIterator;
	typedef cBinaryTreeNodeContentIterator<TColorBinaryTreeNodeOperator,TBinaryTreeNodeContentOperator,const tContent> tConstIterator;

	cLinkSet()noexcept(true):fNodeCount(0){}
	~cLinkSet()noexcept(true){	Clear();	}

	cLinkSet(const cLinkSet &Src)=delete;
	// = copy
	cLinkSet& operator =(const cLinkSet &Src)=delete;

#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

	cLinkSet(cLinkSet &&Src)noexcept(true);
	// = move
	cLinkSet& operator =(cLinkSet &&Src)noexcept(true);

#endif	// cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

	bool IsEmpty(void)const noexcept(true){
		return fTree.GetRoot()==nullptr;
	}

	// GetCount
	// return number items in the list
    uIntn GetCount(void)const noexcept(true){
    	return fNodeCount;
	}


	static void DeleteNode(tNode *Node)noexcept(true){
		TBinaryTreeNodeContentOperator::DestroyNode(Node);
	}
	// Clear
	//	remove all items
	void Clear(void)noexcept(true){
		tNode *Root=fTree.SwapTree();
		if(Root!=nullptr)
			BinaryTreeDeleteAll<TColorBinaryTreeNodeOperator>(Root,DeleteNode);
		fNodeCount=0;
	}

	// for range
	tIterator	begin(void)const noexcept(true){
		tNode *RootLink=fTree.GetGuardLink();
		tNode *Root=TColorBinaryTreeNodeOperator::GetChild(RootLink,false);
		if(Root==nullptr)
			return tIterator(RootLink);
		tNode *Leftmost=BinaryTreeFindSideLeaf<TColorBinaryTreeNodeOperator>(Root,false);
		return tIterator(Leftmost);
	}
	tIterator	end(void)const noexcept(true){
		tNode *EndNode=fTree.GetGuardLink();
		return EndNode;
	}
	tConstIterator	cbegin(void)const noexcept(true){
		tNode *RootLink=fTree.GetGuardLink();
		tNode *Root=TColorBinaryTreeNodeOperator::GetChild(RootLink,false);
		if(Root==nullptr)
			return tIterator(RootLink);
		tNode *Leftmost=BinaryTreeFindSideLeaf<TColorBinaryTreeNodeOperator>(Root,false);
		return tConstIterator(Leftmost);
	}
	tConstIterator	cend(void)const noexcept(true){
		tNode *EndNode=fTree.GetGuardLink();
		return EndNode;
	}

	// Insert
	//	Insert an item
	// [in]ItemValue
	// return true if the key inserted as new item
	template<class T>
	tIterator Insert(bool &Exists,T cnLib_UREF ItemValue)noexcept(true){
		if(IsEmpty()){
			// create node
			auto NewNode=TBinaryTreeNodeContentOperator::CreateNode(cnLib_UREFCAST(T)(ItemValue));
			fTree.InsertRoot(NewNode);
			fNodeCount++;
			Exists=false;
			return NewNode;
		}
		tNode *ResultNode;
		sfInt8 Relation=FindNode(ResultNode,ItemValue);
		if(Relation==0){
			// already exists
			Exists=true;
			return nullptr;
		}
		// create node
		auto NewNode=TBinaryTreeNodeContentOperator::CreateNode(cnLib_UREFCAST(T)(ItemValue));
		// insert node
		bool NodeBranch=Relation<0;	// true : CurNode<ItemValue | false : CurNode>ItemValue
		fTree.InsertNode(ResultNode,NewNode,NodeBranch);
		fNodeCount++;
		Exists=false;
		return NewNode;
	}

	// Insert
	//	Insert an item
	// [in]ItemValue
	// return true if the key inserted as new item
	template<class T>
	bool Insert(T cnLib_UREF ItemValue)noexcept(true){
		bool Exists;
		Insert(Exists,cnLib_UREFCAST(T)(ItemValue));
		return !Exists;
	}

    // Remove
    //	Remove a item has the same value specified in Item
	// [in]Item
    // return true if item is removed
	template<class T>
    bool Remove(const T &ItemValue)noexcept(true){
		if(IsEmpty()){
			return false;
		}
		tNode *ResultNode;
		sfInt8 Relation=FindNode(ResultNode,ItemValue);
		if(Relation!=0){
			// not exists
			return false;
		}
		// found
		fTree.RemoveNode(ResultNode);

		TBinaryTreeNodeContentOperator::DestroyNode(ResultNode);
		fNodeCount--;
		return true;
	}
	// Remove
    //	Remove a item at specified position
	// Iterator			iterator of item to be removed
    void Remove(tIterator Position)noexcept(true){
    	tNode *DeleteNode=Position;
		fTree.RemoveNode(DeleteNode);

		TBinaryTreeNodeContentOperator::DestroyNode(DeleteNode);
		fNodeCount--;
	}


	template<class T>
    tIterator Find(const T &Value)const noexcept(true){
		if(IsEmpty()){
			return end();
		}
		tNode *ResultNode;
		sfInt8 Relation=FindNode(ResultNode,Value);
		if(Relation==0){
			return ResultNode;
		}
		return end();
	}


protected:
	cRBTree<TColorBinaryTreeNodeOperator> fTree;
	uIntn fNodeCount;

	tNode *GetSideLeaf(bool Branch)const noexcept(true){
		return BinaryTreeFindSideLeaf<TColorBinaryTreeNodeOperator>(fTree.GetRoot(),Branch);
	}

	template<class TValue>
	struct cCompareFunctor
	{
		const TValue &Value;

#if cnLibrary_CPPFEATURE_DECLTYPE >= 200707L
		auto operator () (tNode *Node)noexcept(true) -> decltype(TItemOrderOperator::Compare(*TBinaryTreeNodeContentOperator::ContentPtr(Node),Value))
#else
		sfInt8 operator () (tNode *Node)noexcept(true)
#endif
		{
			return TItemOrderOperator::Compare(*TBinaryTreeNodeContentOperator::ContentPtr(Node),Value);
		}
	};
	template<class T>
	sfInt8 FindNode(tNode* &ResultNode,T cnLib_UREF Value)const noexcept(true){
		cCompareFunctor<T> Compare={cnLib_UREFCAST(T)(Value)};
		ResultNode=fTree.GetRoot();
		return BinarySearchTreeFindNode<TColorBinaryTreeNodeOperator>(ResultNode,Compare);
	}

//	void VerifyTreeNode(uIntn &NodeCount,tNode *Node)noexcept(true){
//		auto NodeValue=TBinaryTreeNodeContentOperator::ContentPtr(Node);
//
//		tNode *Left=TColorBinaryTreeNodeOperator::GetChild(Node,false);
//		tNode *Right=TColorBinaryTreeNodeOperator::GetChild(Node,true);
//		if(Left!=nullptr){
//			NodeCount++;
//			cnLib_ASSERT(TColorBinaryTreeNodeOperator::GetParentBranch(Node,Left)==false);
//			auto LeftValue=TBinaryTreeNodeContentOperator::ContentPtr(Left);
//			cnLib_ASSERT(TItemOrderOperator::Compare(*NodeValue,*LeftValue)>0);
//			VerifyTreeNode(NodeCount,Left);
//		}
//		if(Right!=nullptr){
//			NodeCount++;
//			cnLib_ASSERT(TColorBinaryTreeNodeOperator::GetParentBranch(Node,Right)==true);
//			auto RightValue=TBinaryTreeNodeContentOperator::ContentPtr(Right);
//			cnLib_ASSERT(TItemOrderOperator::Compare(*NodeValue,*RightValue)<0);
//			VerifyTreeNode(NodeCount,Right);
//		}
//	}
//	void VerifyTree(void)noexcept(true){
//		tNode *Node=fTree.GetRoot();
//		if(Node==nullptr){
//			cnLib_ASSERT(fNodeCount==0);
//			return;
//		}
//		uIntn NodeCount=1;
//		VerifyTreeNode(NodeCount,Node);
//		cnLib_ASSERT(fNodeCount==NodeCount);
//	}
};
//---------------------------------------------------------------------------
template<class TKey,class TValue>
struct cLinkMapItem
{
	typedef TKey tKey;
	typedef TValue tValue;

	tKey Key;
	tValue Value;

	template<class TK,class...TV>
	cLinkMapItem(TK cnLib_UREF KeyArg,TV cnLib_UREF...ValueArgs)noexcept(true)
		: Key(cnLib_UREFCAST(TK)(KeyArg))
		, Value(cnLib_UREFCAST(TV)(ValueArgs)...)
	{}

	cLinkMapItem(const cLinkMapItem &Src)noexcept(true)
		: Key(Src.Key)
		, Value(Src.Value)
	{}
	cLinkMapItem(cLinkMapItem &&Src)noexcept(true)
		: Key(Src.Key)
		, Value(Src.Value)
	{}
	~cLinkMapItem()noexcept(true){}
};
//---------------------------------------------------------------------------
template<class TColorBinaryTreeNodeOperator,class TBinaryTreeNodeContentOperator,class TItemOrderOperator>
class cLinkMap
{
public:
	typedef typename TBinaryTreeNodeContentOperator::tContent tContent;
	typedef typename TColorBinaryTreeNodeOperator::tNode tNode;

	typedef typename tContent::tKey tKey;
	typedef typename tContent::tValue tValue;

	typedef cBinaryTreeNodeContentIterator<TColorBinaryTreeNodeOperator,TBinaryTreeNodeContentOperator,tContent> tIterator;
	typedef cBinaryTreeNodeContentIterator<TColorBinaryTreeNodeOperator,TBinaryTreeNodeContentOperator,const tContent> tConstIterator;

	cLinkMap()noexcept(true):fNodeCount(0){}
	~cLinkMap()noexcept(true){Clear();}

	cLinkMap(const cLinkMap &Src)=delete;
	// = copy
	cLinkMap& operator =(const cLinkMap &Src)=delete;

#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

	cLinkMap(cLinkMap &&Src)noexcept(true);

	// = move
	cLinkMap& operator =(cLinkMap &&Src)noexcept(true);
#endif	// cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

	bool IsEmpty(void)const noexcept(true){
		return fTree.GetRoot()==nullptr;
	}

	// GetCount
	// return number items in the list
    uIntn GetCount(void)const noexcept(true){
    	return fNodeCount;
	}

	static void DeleteNode(tNode *Node)noexcept(true){
		TBinaryTreeNodeContentOperator::DestroyNode(Node);
	}
	// Clear
	//	remove all items
	void Clear(void)noexcept(true){
		tNode *Root=fTree.SwapTree();
		if(Root!=nullptr){
			BinaryTreeDeleteAll<TColorBinaryTreeNodeOperator>(Root,DeleteNode);
			fNodeCount=0;
		}
	}

	// for range
	tIterator	begin(void)const noexcept(true){
		tNode *RootLink=fTree.GetGuardLink();
		tNode *Root=TColorBinaryTreeNodeOperator::GetChild(RootLink,false);
		if(Root==nullptr)
			return tIterator(RootLink);
		tNode *Leftmost=BinaryTreeFindSideLeaf<TColorBinaryTreeNodeOperator>(Root,false);
		return tIterator(Leftmost);
	}
	tIterator	end(void)const noexcept(true){
		tNode *EndNode=fTree.GetGuardLink();
		tIterator it={EndNode};
		return it;
	}
	tConstIterator	cbegin(void)const noexcept(true){
		tNode *RootLink=fTree.GetGuardLink();
		tNode *Root=TColorBinaryTreeNodeOperator::GetChild(RootLink,false);
		if(Root==nullptr)
			return tIterator(RootLink);
		tNode *Leftmost=BinaryTreeFindSideLeaf<TColorBinaryTreeNodeOperator>(Root,false);
		return tConstIterator(Leftmost);
	}
	tConstIterator	cend(void)const noexcept(true){
		tNode *EndNode=fTree.GetGuardLink();
		tConstIterator it={EndNode};
		return it;
	}


	// InsertPair
	//	insert a pair
	// [in]Key			key of the pair
	// [out]Exists		true if the item already exists
    // return iterator to the newly inserted item paire
	template<class TK,class...TV>
	tIterator InsertPair(TK cnLib_UREF Key,bool &Exists,TV cnLib_UREF...ValueArgs)noexcept(true){
		if(IsEmpty()){
			// create node
			Exists=false;
			auto NewNode=TBinaryTreeNodeContentOperator::CreateNode(cnLib_UREFCAST(TK)(Key),cnLib_UREFCAST(TV)(ValueArgs)...);
			fTree.InsertRoot(NewNode);
			fNodeCount++;
			return NewNode;
		}
		tNode *ResultNode;
		sfInt8 Relation=FindNode(ResultNode,Key);
		if(Relation==0){
			// already exists
			Exists=true;
			return ResultNode;
		}
		// create node
		Exists=false;
		auto NewNode=TBinaryTreeNodeContentOperator::CreateNode(cnLib_UREFCAST(TK)(Key),cnLib_UREFCAST(TV)(ValueArgs)...);
		// insert node
		bool NodeBranch=Relation<0;	// true : CurNode<ItemValue | false : CurNode>ItemValue
		fTree.InsertNode(ResultNode,NewNode,NodeBranch);
		fNodeCount++;
		return NewNode;
	}
	template<class TK>
	tIterator InsertPair(TK cnLib_UREF Key)noexcept(true){
		bool Exists;
		return InsertPair(cnLib_UREFCAST(TK)(Key),Exists);
	}

	// RemovePair
    //	Remove a pair of item at specified position
	// Iterator			iterator of item to be removed
    void RemovePair(tIterator Position)noexcept(true){
    	tNode *DeleteNode=Position;
		fTree.RemoveNode(DeleteNode);

		TBinaryTreeNodeContentOperator::DestroyNode(DeleteNode);
		fNodeCount--;
	}

	// Insert
	//	Insert a item
	// [in]Key			initializer for key
	// [in]Value		initializer for value
	//	return: true if the key inserted as new
	template<class TK,class...TV>
	bool Insert(TK cnLib_UREF Key,TV cnLib_UREF...ValueArgs)noexcept(true){
		bool Exists;
		InsertPair(cnLib_UREFCAST(TK)(Key),Exists,cnLib_UREFCAST(TV)(ValueArgs)...);
		return !Exists;
	}

	// Replace
	//	Insert a item or replace the value associated with the key
	// [in]Key			initializer for key
	// [in]Value		initializer for value
	//	return: true if the key inserted as new
	template<class TK,class TV>
	bool Replace(TK cnLib_UREF Key,TV cnLib_UREF Value)noexcept(true){
		bool Exists;
		auto Node=InsertPair(cnLib_UREFCAST(TK)(Key),Exists,cnLib_UREFCAST(TV)(Value));
		if(Exists==false){
			TBinaryTreeNodeContentOperator::ContentPtr(Node)->Value=cnLib_UREFCAST(TV)(Value);
		}
		return !Exists;
	}
    // Remove
    //	Remove a item has the same value specified in Item
	// [in]Item
    // return true if item is removed
	template<class T>
    bool Remove(const T &Key)noexcept(true){
		if(IsEmpty()){
			return false;
		}
		tNode *ResultNode;
		sfInt8 Relation=FindNode(ResultNode,Key);
		if(Relation!=0){
			// not exists
			return false;
		}
		// found
		fTree.RemoveNode(ResultNode);

		TBinaryTreeNodeContentOperator::DestroyNode(ResultNode);
		fNodeCount--;
		return true;
	}

	template<class T>
	tValue& operator[](const T &Key)noexcept(true){
		bool Exists;
		auto Node=InsertPair(Key,Exists);
		return TBinaryTreeNodeContentOperator::ContentPtr(Node)->Value;
	}

	// Get
    //	Get item pointer to the specified Key.
	// [in]Key				key
    // return pointer to the value of key
	template<class T>
	tValue* Get(const T &Key)const noexcept(true){
		if(IsEmpty()){
			return nullptr;
		}
		tNode *ResultNode;
		sfInt8 Relation=FindNode(ResultNode,Key);
		if(Relation==0){
			return cnMemory::AddressOf(TBinaryTreeNodeContentOperator::ContentPtr(ResultNode)->Value);
		}
		return nullptr;
	}

	// GetPair
    //	Get reference of item pair to the specified Key.
	// [in]Key				key
    // return iterator to the item paire
	template<class T>
    tIterator GetPair(const T &Key)const noexcept(true){
		if(IsEmpty()){
			return end();
		}
		tNode *ResultNode;
		sfInt8 Relation=FindNode(ResultNode,Key);
		if(Relation==0){
			return ResultNode;
		}
		return end();
	}

protected:
	cRBTree<TColorBinaryTreeNodeOperator> fTree;
	uIntn fNodeCount;

	tNode* GetSideLeaf(bool Branch)const noexcept(true){
		return BinaryTreeFindSideLeaf<TColorBinaryTreeNodeOperator>(fTree.GetRoot(),Branch);
	}

	template<class TKey>
	struct cCompareFunctor
	{
		const TKey &Key;

#if cnLibrary_CPPFEATURE_DECLTYPE >= 200707L
		auto operator () (tNode *Node)noexcept(true) -> decltype(TItemOrderOperator::Compare(TBinaryTreeNodeContentOperator::ContentPtr(Node)->Key,Key))
#else
		sfInt8 operator () (tNode *Node)noexcept(true)
#endif
		{
			return TItemOrderOperator::Compare(TBinaryTreeNodeContentOperator::ContentPtr(Node)->Key,Key);
		}
	};
	template<class T>
	sfInt8 FindNode(tNode* &ResultNode,T cnLib_UREF Key)const noexcept(true){
		cCompareFunctor<T> Compare={cnLib_UREFCAST(T)(Key)};
		ResultNode=fTree.GetRoot();
		return BinarySearchTreeFindNode<TColorBinaryTreeNodeOperator>(ResultNode,Compare);
	}

//		void VerifyTreeNode(uIntn &NodeCount,tNode *Node){
//			auto &NodeValue=TBinaryTreeNodeContentOperator::ContentPtr(Node)->Key;
//
//			tNode *Left=TColorBinaryTreeNodeOperator::GetChild(Node,false);
//			tNode *Right=TColorBinaryTreeNodeOperator::GetChild(Node,true);
//			if(Left!=nullptr){
//				NodeCount++;
//				cnLib_ASSERT(TColorBinaryTreeNodeOperator::GetParentBranch(Node,Left)==false);
//				auto &LeftValue=TBinaryTreeNodeContentOperator::ContentPtr(Left)->Key;
//				cnLib_ASSERT(TItemOrderOperator::Compare(NodeValue,LeftValue)>0);
//				VerifyTreeNode(NodeCount,Left);
//			}
//			if(Right!=nullptr){
//				NodeCount++;
//				cnLib_ASSERT(TColorBinaryTreeNodeOperator::GetParentBranch(Node,Right)==true);
//				auto &RightValue=TBinaryTreeNodeContentOperator::ContentPtr(Right)->Key;
//				cnLib_ASSERT(TItemOrderOperator::Compare(NodeValue,RightValue)<0);
//				VerifyTreeNode(NodeCount,Right);
//			}
//		}
//		void VerifyTree(void){
//			tNode *Node=fTree.GetRoot();
//			if(Node==nullptr){
//				cnLib_ASSERT(fNodeCount==0);
//				return;
//			}
//			uIntn NodeCount=1;
//			VerifyTreeNode(NodeCount,Node);
//			cnLib_ASSERT(fNodeCount==NodeCount);
//		}
};
//---------------------------------------------------------------------------
}	// namespace cnDataStruct
//---------------------------------------------------------------------------
}   // namespace cnLibrary
//---------------------------------------------------------------------------
#endif	// cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L
//---------------------------------------------------------------------------
#include <cnTK/TKMacrosCleanup.inc>
//---------------------------------------------------------------------------
#endif	/* cnLibrary_CPPFEATURELEVEL >= 1 */
/*-------------------------------------------------------------------------*/
#endif
