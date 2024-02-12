/*- cnTK - Data Struct Items ----------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2021-09-24                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnTK_StructItem_H__
#define	__cnLibrary_cnTK_StructItem_H__
/*-------------------------------------------------------------------------*/
#include <cnTK/Common.hpp>
#include <cnTK/TypeTraits.hpp>
#include <cnTK/Numerical.hpp>
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


//struct TItemOrderOperator
//{
//	static auto Compare(const auto &Item,const auto &Value)noexcept;
//};

template<class T>
struct cDefaultItemOrderOperator
{
	template<class TValue>
	static typename cnVar::TDefaultCompareResult<T,TValue>::Type Compare(const T &Item,const TValue &Value)noexcept(true)
	{
		return cnVar::DefaultCompare(Item,Value);
	}

};

template<class T>
struct cRAWDataItemOrderOperator
{
	template<class TValue>
	static sfInt8 Compare(const T &Item,const TValue &Value)noexcept(true){
		if cnLib_IFCONSTEXPR(sizeof(T)<sizeof(TValue))
			return -1;
		if cnLib_IFCONSTEXPR(sizeof(T)>sizeof(TValue))
			return 1;

		typedef typename cnVar::TIntegerOfSize<cnMath::TMin<uIntn,sizeof(uIntn),cnMemory::TAlignmentOf<T>::Value>::Value,false>::Type TCompareValue;
		return cnMemory::ViewCompare(&reinterpret_cast<const TCompareValue&>(Item),&reinterpret_cast<const TCompareValue&>(Value),sizeof(T)/sizeof(TCompareValue));
	}
};

template<class T>
struct cItemAddressOrderOperator
{
	//typedef T tItem;


#if cnLibrary_CPPFEATURE_THREE_WAY_COMPARISION >= 201907L && cnLibrary_CPPFEATURE_DECLTYPE >=200707L
	template<class TValue>
	static auto Compare(const T &Item,const TValue &Value)noexcept(true){
		auto pItem=cnMemory::AddressOf(Item);
		auto pCompare=cnMemory::AddressOf(Value);
		return pItem<=>pCompare;
	}
#else	// cnLibrary_CPPFEATURE_THREE_WAY_COMPARISION < 201907L || cnLibrary_CPPFEATURE_DECLTYPE < 200707L
	template<class TValue>
	static sfInt8 Compare(const T &Item,const TValue &Value)noexcept(true){
		auto pItem=cnMemory::AddressOf(Item);
		auto pCompare=cnMemory::AddressOf(Value);
		if(pItem==pCompare)
			return 0;
		if(pItem<pCompare)
			return -1;
		return 1;
	}
#endif	// cnLibrary_CPPFEATURE_THREE_WAY_COMPARISION , cnLibrary_CPPFEATURE_DECLTYPE
};

//---------------------------------------------------------------------------

//TIterator
//{
//	typedef TElementRef;
//
//	TElementRef operator *()noexcept;
//	auto operator ->()noexcept;
//	TIterator& operator ==(const TIterator &Src)const noexcept;
//	TIterator& operator !=(const TIterator &Src)const noexcept;
//	TIterator& operator ++()noexcept;
//	void operator ++(int)noexcept;
//};

//TReversibleIterator : TIterator
//{
//	TIterator& operator --()noexcept;
//	void operator --(int)noexcept;
//};

//TRandomAccessIterator : TReversibleIterator
//{
//	TIterator operator +(sIntn Offset)const noexcept;
//	TIterator operator -(sIntn Offset)const noexcept;
//	TIterator& operator +=(sIntn Offset)noexcept;
//	TIterator& operator -=(sIntn Offset)noexcept;
//	TElementRef operator [](uIntn Index)noexcept;
//	sIntn operator -(const TIterator &Src)const noexcept;
//	bool operator <(const TIterator &Src)const noexcept;
//	bool operator <=(const TIterator &Src)const noexcept;
//	bool operator >(const TIterator &Src)const noexcept;
//	bool operator >=(const TIterator &Src)const noexcept;
//};

//TForRanged
//{
//	typedef TIterator;
//	typedef TConstIterator;
//	TIterator begin(void)noexcept;
//	TIterator end(void)noexcept;
//	TConstIterator begin(void)const noexcept;
//	TConstIterator end(void)const noexcept;
//	TConstIterator cbegin(void)const noexcept;
//	TConstIterator cend(void)const noexcept;
//};

//---------------------------------------------------------------------------
//TLinkNodeContentOperator
//{
//	typedef tContent;
//	typedef tNode;
//
//	static tContent* ContentPtr(tNode *Node)noexcept;
//	template<class...TArgs>
//	static tNode* CreateNode(TArgs&&...Args)noexcept;
//	static void DestroyNode(tNode *Node)noexcept;
//};

//---------------------------------------------------------------------------
//struct TSingleLinkedItemOperator
//{
//	typedef tItem;
//	typedef tLinkInstance;
//
//	static tItem* LinkNode(tLinkInstance &Link)noexcept;
//
//	static tItem* GetNext(const tItem *Item)noexcept;
//	static void SetNext(tItem *Item,tItem *Next)noexcept;
//
//};
//---------------------------------------------------------------------------
template<class TSingleLinkedItemOperator>
inline bool SingleLinkedListItemIsConnected(typename TSingleLinkedItemOperator::tItem const *Head,typename TSingleLinkedItemOperator::tItem const *Tail)noexcept(true)
{
	if(Head==Tail)
		return true;
	typename TSingleLinkedItemOperator::tItem const *SearchItem=TSingleLinkedItemOperator::GetNext(Head);
	while(SearchItem!=nullptr){
		if(Tail==SearchItem){
			return true;
		}
		if(Head==SearchItem){
			return false;
		}
		SearchItem=TSingleLinkedItemOperator::GetNext(SearchItem);
	}
	return false;
}
//---------------------------------------------------------------------------
template<class TSingleLinkedItemOperator,class TLinkNodeContentOperator,class TSrcIterator>
inline typename TSingleLinkedItemOperator::tItem* SingleLinkedListItemDuplicateChain(TSrcIterator Begin,TSrcIterator End)noexcept(true)
{
	if(Begin==End)
		return nullptr;
	typedef typename TSingleLinkedItemOperator::tItem tItem;
	tItem *HeadItem=TLinkNodeContentOperator::CreateNode(*Begin);
	tItem *TailItem=HeadItem;
	for(++Begin;Begin!=End;++Begin){
		tItem *NewItem=TLinkNodeContentOperator::CreateNode(*Begin);
		TSingleLinkedItemOperator::SetNext(TailItem,NewItem);
		TailItem=NewItem;
	}
	TSingleLinkedItemOperator::SetNext(TailItem,nullptr);
	return HeadItem;
}
//---------------------------------------------------------------------------
template<class TSingleLinkedItemOperator,class TLinkNodeContentOperator>
inline void SingleLinkedListItemDeleteChain(typename TSingleLinkedItemOperator::tItem *HeadItem,typename TSingleLinkedItemOperator::tItem *TailItem)noexcept(true)
{
	while(HeadItem!=TailItem){
		typename TSingleLinkedItemOperator::tItem *DeleteItem=HeadItem;
		HeadItem=TSingleLinkedItemOperator::GetNext(HeadItem);
		TLinkNodeContentOperator::DestroyNode(DeleteItem);
	}
	TLinkNodeContentOperator::DestroyNode(HeadItem);
}
//---------------------------------------------------------------------------
template<class TSingleLinkedItemOperator,class TLinkNodeContentOperator>
inline uIntn SingleLinkedListItemCountDeleteChain(typename TSingleLinkedItemOperator::tItem *HeadItem,typename TSingleLinkedItemOperator::tItem *TailItem)noexcept(true)
{
	uIntn RemovedCount=1;
	while(HeadItem!=TailItem){
		typename TSingleLinkedItemOperator::tItem *DeleteItem=HeadItem;
		HeadItem=TSingleLinkedItemOperator::GetNext(HeadItem);
		TLinkNodeContentOperator::DestroyNode(DeleteItem);
		RemovedCount++;
	}
	TLinkNodeContentOperator::DestroyNode(HeadItem);
	return RemovedCount;
}
//---------------------------------------------------------------------------
//TSingleLinkItem		// Common Single Linked Item Definition
//{
//	TSingleLinkItem *Next;
//};
template<class TSingleLinkItem>
struct cSingleLinkItemOperator
{
	typedef TSingleLinkItem tItem;

#if cnLibrary_CPPFEATURE_DECLTYPE >= 200707L

	enum{
		LinkInstanceOffset=(&static_cast<TSingleLinkItem*>(nullptr)->Next-static_cast<decltype(&static_cast<TSingleLinkItem*>(nullptr)->Next)>(nullptr))
				*sizeof(static_cast<TSingleLinkItem*>(nullptr)->Next),
		LinkInstanceEndOffset=LinkInstanceOffset+sizeof(static_cast<TSingleLinkItem*>(nullptr)->Next),
	};


// cnLibrary_CPPFEATURE_DECLTYPE >= 200707L
#else
// cnLibrary_CPPFEATURE_DECLTYPE < 200707L

	enum{
		LinkInstanceOffset=0,
		LinkInstanceEndOffset=sizeof(TSingleLinkItem),
	};

#endif	// cnLibrary_CPPFEATURE_DECLTYPE < 200707L

	typedef cnMemory::cPlainAlignedData<LinkInstanceEndOffset-LinkInstanceOffset,TSingleLinkItem> tLinkInstance;
	
	static tItem* LinkNode(tLinkInstance &Link)noexcept(true){
		return reinterpret_cast<tItem*>(
			reinterpret_cast<uIntn>(&Link)-LinkInstanceOffset
		);
	}
	static const tItem* LinkNode(const tLinkInstance &Link)noexcept(true){
		return reinterpret_cast<const tItem*>(
			reinterpret_cast<uIntn>(&Link)-LinkInstanceOffset
		);
	}

	static tItem* GetNext(const tItem *Item)noexcept(true){
		return static_cast<tItem*>(Item->Next);
	}
	static void SetNext(tItem *Item,tItem *Next)noexcept(true){
		Item->Next=Next;
	}
};
//---------------------------------------------------------------------------
//TDualLinkedItemOperator : TSingleLinkedOperator
//{
//	static tItem* GetPrev(tItem *Item);
//	static void SetPrev(tItem *Item,tItem *Prev);
//};
//---------------------------------------------------------------------------
template<class TDualLinkedItemOperator>
inline bool DualLinkedListItemIsConnected(typename TDualLinkedItemOperator::tItem const *Head,typename TDualLinkedItemOperator::tItem const *Tail)noexcept(true)
{
	if(Head==Tail)
		return true;
	if(SingleLinkedListItemIsConnected(Head,Tail)==false)
		return false;
	typename TDualLinkedItemOperator::tItem const *SearchItem=TDualLinkedItemOperator::GetPrev(Tail);
	while(SearchItem!=nullptr){
		if(Head==SearchItem){
			return true;
		}
		if(Tail==SearchItem){
			return false;
		}
		SearchItem=TDualLinkedItemOperator::GetPrev(SearchItem);
	}
	return false;
}
//---------------------------------------------------------------------------
template<class TDualLinkedItemOperator>
inline void DualLinkedListItemInsertBefore(typename TDualLinkedItemOperator::tItem *Pos,typename TDualLinkedItemOperator::tItem *NewHeadItem,typename TDualLinkedItemOperator::tItem *NewTailItem)noexcept(true)
{
	typename TDualLinkedItemOperator::tItem *PrevItem=TDualLinkedItemOperator::GetPrev(Pos);
	TDualLinkedItemOperator::SetNext(PrevItem,NewHeadItem);
	TDualLinkedItemOperator::SetPrev(NewHeadItem,PrevItem);
	TDualLinkedItemOperator::SetNext(NewTailItem,Pos);
	TDualLinkedItemOperator::SetPrev(Pos,NewTailItem);
}
//---------------------------------------------------------------------------
template<class TDualLinkedItemOperator>
inline void DualLinkedListItemInsertAfter(typename TDualLinkedItemOperator::tItem *Pos,typename TDualLinkedItemOperator::tItem *NewHeadItem,typename TDualLinkedItemOperator::tItem *NewTailItem)noexcept(true)
{
	typename TDualLinkedItemOperator::tItem *NextItem=TDualLinkedItemOperator::GetNext(Pos);
	TDualLinkedItemOperator::SetNext(Pos,NewHeadItem);
	TDualLinkedItemOperator::SetPrev(NewHeadItem,Pos);
	TDualLinkedItemOperator::SetNext(NewTailItem,NextItem);
	TDualLinkedItemOperator::SetPrev(NextItem,NewTailItem);
}
//---------------------------------------------------------------------------
template<class TDualLinkedItemOperator>
inline void DualLinkedListItemRemove(typename TDualLinkedItemOperator::tItem *HeadItem,typename TDualLinkedItemOperator::tItem *TailItem)noexcept(true)
{
	typename TDualLinkedItemOperator::tItem *PrevItem=TDualLinkedItemOperator::GetPrev(HeadItem);
	typename TDualLinkedItemOperator::tItem *NextItem=TDualLinkedItemOperator::GetNext(TailItem);
	TDualLinkedItemOperator::SetNext(PrevItem,NextItem);
	TDualLinkedItemOperator::SetPrev(NextItem,PrevItem);

	TDualLinkedItemOperator::SetNext(TailItem,nullptr);
	TDualLinkedItemOperator::SetPrev(HeadItem,nullptr);
}
//---------------------------------------------------------------------------
template<class TDualLinkedItemOperator>
inline typename TDualLinkedItemOperator::tItem* DualLinkedListItemFindBefore(typename TDualLinkedItemOperator::tItem *TailItem,uIntn Count,typename TDualLinkedItemOperator::tItem *GuardItem)noexcept(true)
{
	typename TDualLinkedItemOperator::tItem *HeadItem=TailItem;
	do{
		typename TDualLinkedItemOperator::tItem *PrevItem=TDualLinkedItemOperator::GetPrev(HeadItem);
		if(PrevItem==GuardItem)
			break;
		HeadItem=PrevItem;
	}while(--Count>0);
	return HeadItem;
}
//---------------------------------------------------------------------------
template<class TDualLinkedItemOperator>
inline typename TDualLinkedItemOperator::tItem* DualLinkedListItemFindAfter(typename TDualLinkedItemOperator::tItem *HeadItem,uIntn Count,typename TDualLinkedItemOperator::tItem *GuardItem)noexcept(true)
{
	typename TDualLinkedItemOperator::tItem *TailItem=HeadItem;
	do{
		typename TDualLinkedItemOperator::tItem *NextItem=TDualLinkedItemOperator::GetNext(TailItem);
		if(NextItem==GuardItem)
			break;
		TailItem=NextItem;
	}while(--Count>0);
	return TailItem;
}
//---------------------------------------------------------------------------
template<class TDualLinkedItemOperator,class TLinkNodeContentOperator,class TSrcIterator>
inline typename TDualLinkedItemOperator::tItem* DualLinkedListItemDuplicateRing(TSrcIterator Begin,TSrcIterator End)noexcept(true)
{
	if(Begin==End)
		return nullptr;
	typedef typename TDualLinkedItemOperator::tItem tItem;
	tItem *HeadItem=TLinkNodeContentOperator::CreateNode(*Begin);
	tItem *TailItem=HeadItem;
	for(++Begin;Begin!=End;++Begin){
		tItem *NewItem=TLinkNodeContentOperator::CreateNode(*Begin);
		TDualLinkedItemOperator::SetNext(TailItem,NewItem);
		TDualLinkedItemOperator::SetPrev(NewItem,TailItem);
		TailItem=NewItem;
	}
	TDualLinkedItemOperator::SetNext(TailItem,HeadItem);
	TDualLinkedItemOperator::SetPrev(HeadItem,TailItem);
	return HeadItem;
}
//---------------------------------------------------------------------------
//TDualLinkItem		// Common Dual Linked Item Definition
//{
//	TDualLinkItem *Next;
//	TDualLinkItem *Prev;
//};
//---------------------------------------------------------------------------
template<class TDualLinkItem>
struct cDualLinkItemOperator : cSingleLinkItemOperator<TDualLinkItem>
{
	typedef TDualLinkItem tItem;

#if cnLibrary_CPPFEATURE_DECLTYPE >= 200707L

	enum{
		LinkInstanceOffset=cnMath::TMin<uIntn
			,(&static_cast<TDualLinkItem*>(nullptr)->Next-static_cast<decltype(&static_cast<TDualLinkItem*>(nullptr)->Next)>(nullptr))
				*sizeof(static_cast<TDualLinkItem*>(nullptr)->Next)
			,(&static_cast<TDualLinkItem*>(nullptr)->Prev-static_cast<decltype(&static_cast<TDualLinkItem*>(nullptr)->Prev)>(nullptr))
				*sizeof(static_cast<TDualLinkItem*>(nullptr)->Prev)
		>::Value,
		LinkInstanceEndOffset=cnMath::TMax<uIntn
			,(&static_cast<TDualLinkItem*>(nullptr)->Next-static_cast<decltype(&static_cast<TDualLinkItem*>(nullptr)->Next)>(nullptr))
				*sizeof(static_cast<TDualLinkItem*>(nullptr)->Next)+sizeof(static_cast<TDualLinkItem*>(nullptr)->Next)
			,(&static_cast<TDualLinkItem*>(nullptr)->Prev-static_cast<decltype(&static_cast<TDualLinkItem*>(nullptr)->Prev)>(nullptr))
				*sizeof(static_cast<TDualLinkItem*>(nullptr)->Prev)+sizeof(static_cast<TDualLinkItem*>(nullptr)->Prev)
		>::Value,
	};

	typedef cnMemory::cPlainAlignedData<LinkInstanceEndOffset-LinkInstanceOffset,TDualLinkItem> tLinkInstance;
	
	static tItem* LinkNode(tLinkInstance &Link)noexcept(true){
		return reinterpret_cast<tItem*>(
			reinterpret_cast<uIntn>(&Link)-LinkInstanceOffset
		);
	}
	static const tItem* LinkNode(const tLinkInstance &Link)noexcept(true){
		return reinterpret_cast<tItem*>(
			reinterpret_cast<uIntn>(&Link)-LinkInstanceOffset
		);
	}

#endif	// cnLibrary_CPPFEATURE_DECLTYPE < 200707L

	static tItem* GetPrev(tItem *Item)noexcept(true){
		return static_cast<tItem*>(Item->Prev);
	}
	static void SetPrev(tItem *Item,tItem *Prev)noexcept(true){
		Item->Prev=Prev;
	}

};
//---------------------------------------------------------------------------
//TBinaryTreeNodeOperator
//{
//	typedef tNode;
//	typedef tLinkInstance;
//	static tNode* LinkNode(tLinkInstance &Link)noexcept;
//
//	static void ResetNode(tNode *Node)noexcept;
//	static tNode* GetChild(tNode *Node,bool ChildBranch)noexcept;
//	static void SetChildNull(tNode *Node,bool ChildBranch)noexcept;
//	static void SetChild(tNode *Node,bool ChildBranch,tNode *Child)noexcept;
//	static tNode* GetParent(tNode *Node)noexcept;
//	static void SetParentNull(tNode *Node)noexcept;
//	static bool GetParentBranch(tNode *Node)noexcept;
//	static bool GetParentBranch(tNode *Parent,tNode *Node)noexcept;
//
//};
//---------------------------------------------------------------------------
template<class TBinaryTreeNodeOperator>
inline bool BinaryTreeIsNodeInBranch(typename TBinaryTreeNodeOperator::tNode *Parent,typename TBinaryTreeNodeOperator::tNode *Node)noexcept(true)
{
	do{
		if(Parent==Node)
			return true;
	}while((Node=TBinaryTreeNodeOperator::GetParent(Node))!=nullptr);
	return false;
}

template<class TBinaryTreeNodeOperator>
inline typename TBinaryTreeNodeOperator::tNode* BinaryTreeFindSideLeaf(typename TBinaryTreeNodeOperator::tNode *Node,bool Branch)noexcept(true)
{
	typename TBinaryTreeNodeOperator::tNode *NextNode;
	while((NextNode=TBinaryTreeNodeOperator::GetChild(Node,Branch))!=nullptr){
		Node=NextNode;
	}
	return Node;
}

// find node in tree branch
// [in]Node				root node to find
// [out]Node			result node
// [in]Compare			compare function, compare node and dest element.
//						prototype: sfInt8 (tNode *Node);
//						return 0 if equal, -1 if Content(Node)<CompareValue, 1 if Content(Node)>CompareValue
// return:	relation of found node and compare value.
template<class TBinaryTreeNodeOperator,class TCompareFunc>
#if cnLibrary_CPPFEATURE_DECLTYPE >= 200707L
inline auto BinarySearchTreeFindNode(typename TBinaryTreeNodeOperator::tNode* &Node,TCompareFunc cnLib_UREF Compare)noexcept(true) -> decltype(Compare(0))
#else	// cnLibrary_CPPFEATURE_DECLTYPE < 200707L
inline sfInt8 BinarySearchTreeFindNode(typename TBinaryTreeNodeOperator::tNode* &Node,TCompareFunc cnLib_UREF Compare)noexcept(true)
#endif	// cnLibrary_CPPFEATURE_DECLTYPE
{
	cnLib_ASSERT(Node!=nullptr);
	typename TBinaryTreeNodeOperator::tNode *CurNode=Node;
#if cnLibrary_CPPFEATURE_DECLTYPE >= 200707L
	decltype(Compare(CurNode)) Relation;
#else	// cnLibrary_CPPFEATURE_DECLTYPE < 200707L
	sfInt8 Relation;
#endif	// cnLibrary_CPPFEATURE_DECLTYPE
	do{
		Relation=Compare(CurNode);
		if(Relation==0){
			Node=CurNode;
			return Relation;
		}
		if(Relation<0){
			// CurNode < CompareValue
			Node=CurNode;
			CurNode=TBinaryTreeNodeOperator::GetChild(CurNode,true);
		}
		else{
			// CurNode > CompareValue
			Node=CurNode;
			CurNode=TBinaryTreeNodeOperator::GetChild(CurNode,false);
		}
	}while(CurNode!=nullptr);
	return Relation;
}


// BinaryTreeTravelM
//	Node				node to travel from
//	TravelBackward		false: forward, true: backward
// forward : Left - Mid - Right. start from left most node
// backward : Right - Mid - Left. start from right most node

template<class TBinaryTreeNodeOperator>
inline typename TBinaryTreeNodeOperator::tNode* BinaryTreeTravelM(typename TBinaryTreeNodeOperator::tNode *Node,bool TravelBackward)noexcept(true)
{
	typename TBinaryTreeNodeOperator::tNode *NextNode=TBinaryTreeNodeOperator::GetChild(Node,!TravelBackward);
	if(NextNode==nullptr){
		// reached rightmost/leftmost of current branch, find parent of the left/right branch

		while((NextNode=TBinaryTreeNodeOperator::GetParent(Node))!=nullptr){
			if(TBinaryTreeNodeOperator::GetParentBranch(NextNode,Node)==TravelBackward){
				return NextNode;
			}
			Node=NextNode;
		};
		return nullptr;
	}
	// find leftmost/rightmost node of the branch
	do{
		Node=NextNode;
		NextNode=TBinaryTreeNodeOperator::GetChild(Node,TravelBackward);
	}while(NextNode!=nullptr);
	return Node;
}

template<class TBinaryTreeNodeOperator,class TDeleteNodeFunction>
inline void BinaryTreeDeleteAll(typename TBinaryTreeNodeOperator::tNode *Node,TDeleteNodeFunction cnLib_UREF DeleteNodeFunction)noexcept(true)
{
	cnLib_ASSERT(Node!=nullptr);
	typedef typename TBinaryTreeNodeOperator::tNode* TNodePtr;
	TNodePtr CurNode;
	TNodePtr NextNode=Node;
	for(;;){
		do{
			CurNode=NextNode;
			NextNode=TBinaryTreeNodeOperator::GetChild(CurNode,false);
		}while(NextNode!=nullptr);

		while((NextNode=TBinaryTreeNodeOperator::GetChild(CurNode,true))==nullptr){
			// delete current node
			bool DeleteNodeBranch;
			do{
				NextNode=CurNode;
				TNodePtr CurNodeParent=TBinaryTreeNodeOperator::GetParent(CurNode);
				DeleteNodeBranch=TBinaryTreeNodeOperator::GetParentBranch(CurNodeParent,CurNode);
				CurNode=CurNodeParent;
				DeleteNodeFunction(NextNode);
				if(NextNode==Node)
					return;
			}while(DeleteNodeBranch);
		}
	}
}
//---------------------------------------------------------------------------
//TBinaryTreeNode			// Common Binary Tree Node Definition
//{
//	TBinaryTreeNode *Parent;
//	TBinaryTreeNode *Child[2];		// left, right
//	ufInt8 Branch;					// 0==left | 1==right
//};
//---------------------------------------------------------------------------
template<class TBinaryTreeNode>
struct cBinaryTreeNodeOperator
{
	typedef TBinaryTreeNode tNode;

#if cnLibrary_CPPFEATURE_DECLTYPE >= 200707L

	enum{
		LinkInstanceOffset=cnMath::TMin<uIntn
			,(&static_cast<TBinaryTreeNode*>(nullptr)->Parent-static_cast<decltype(&static_cast<TBinaryTreeNode*>(nullptr)->Parent)>(nullptr))
				*sizeof(static_cast<TBinaryTreeNode*>(nullptr)->Parent)
			,(&static_cast<TBinaryTreeNode*>(nullptr)->Child-static_cast<decltype(&static_cast<TBinaryTreeNode*>(nullptr)->Child)>(nullptr))
				*sizeof(static_cast<TBinaryTreeNode*>(nullptr)->Child)
			,(&static_cast<TBinaryTreeNode*>(nullptr)->Branch-static_cast<decltype(&static_cast<TBinaryTreeNode*>(nullptr)->Branch)>(nullptr))
				*sizeof(static_cast<TBinaryTreeNode*>(nullptr)->Branch)
		>::Value,
		LinkInstanceEndOffset=cnMath::TMax<uIntn
			,(&static_cast<TBinaryTreeNode*>(nullptr)->Parent-static_cast<decltype(&static_cast<TBinaryTreeNode*>(nullptr)->Parent)>(nullptr))
				*sizeof(static_cast<TBinaryTreeNode*>(nullptr)->Parent)+sizeof(static_cast<TBinaryTreeNode*>(nullptr)->Parent)
			,(&static_cast<TBinaryTreeNode*>(nullptr)->Child-static_cast<decltype(&static_cast<TBinaryTreeNode*>(nullptr)->Child)>(nullptr))
				*sizeof(static_cast<TBinaryTreeNode*>(nullptr)->Child)+sizeof(static_cast<TBinaryTreeNode*>(nullptr)->Child)
			,(&static_cast<TBinaryTreeNode*>(nullptr)->Branch-static_cast<decltype(&static_cast<TBinaryTreeNode*>(nullptr)->Branch)>(nullptr))
				*sizeof(static_cast<TBinaryTreeNode*>(nullptr)->Branch)+sizeof(static_cast<TBinaryTreeNode*>(nullptr)->Branch)
		>::Value,
	};


// cnLibrary_CPPFEATURE_DECLTYPE >= 200707L
#else
// cnLibrary_CPPFEATURE_DECLTYPE < 200707L

	enum{
		LinkInstanceOffset=0,
		LinkInstanceEndOffset=sizeof(TBinaryTreeNode),
	};

#endif	// cnLibrary_CPPFEATURE_DECLTYPE < 200707L

	typedef cnMemory::cPlainAlignedData<LinkInstanceEndOffset-LinkInstanceOffset,TBinaryTreeNode> tLinkInstance;

	static tNode* LinkNode(tLinkInstance &Link)noexcept(true){
		return reinterpret_cast<tNode*>(
			reinterpret_cast<uIntn>(&Link)-LinkInstanceOffset
		);
	}
	static const tNode* LinkNode(const tLinkInstance &Link)noexcept(true){
		return reinterpret_cast<tNode*>(
			reinterpret_cast<uIntn>(&Link)-LinkInstanceOffset
		);
	}

	static void ResetNode(tNode *Node)noexcept(true){
		Node->Parent=nullptr;
		Node->Child[0]=nullptr;
		Node->Child[1]=nullptr;
	}

	static tNode* GetChild(tNode *Node,bool ChildBranch)noexcept(true){
		return Node->Child[ChildBranch];
	}

	static void SetChildNull(tNode *Node,bool ChildBranch)noexcept(true){
		Node->Child[ChildBranch]=nullptr;
	}
	static void SetChild(tNode *Node,bool ChildBranch,tNode *Child)noexcept(true){
		Node->Child[ChildBranch]=Child;
		Child->Parent=Node;
		Child->Branch=ChildBranch;
	}

	static tNode* GetParent(tNode *Node)noexcept(true){
		return Node->Parent;
	}

	static void SetParentNull(tNode *Node)noexcept(true){
		Node->Parent=nullptr;
	}

	static bool GetParentBranch(tNode *Node)noexcept(true){
		return Node->Branch;
	}
	static bool GetParentBranch(tNode *,tNode *Node)noexcept(true){
		return Node->Branch;
	}

};
//---------------------------------------------------------------------------
//struct TColorBinaryTreeNodeOperator : TBinaryTreeNodeOperator
//{
//	static ufInt8& NodeColorRef(tNode *Node)noexcept;
//};
//---------------------------------------------------------------------------
//TColorBinaryTreeNode : TBinaryTreeNode		// Common Color Binary Tree Node Definition
//{
//	ufInt8 Color;						// Color value used by tree
//};
//---------------------------------------------------------------------------
template<class TColorBinaryTreeNode>
struct cColorBinaryTreeNodeOperator : cBinaryTreeNodeOperator<TColorBinaryTreeNode>
{
	typedef TColorBinaryTreeNode tNode;

#if cnLibrary_CPPFEATURE_DECLTYPE >= 200707L

	enum{
		LinkInstanceOffset=cnMath::TMin<uIntn
			,(&static_cast<TColorBinaryTreeNode*>(nullptr)->Parent-static_cast<decltype(&static_cast<TColorBinaryTreeNode*>(nullptr)->Parent)>(nullptr))
				*sizeof(static_cast<TColorBinaryTreeNode*>(nullptr)->Parent)
			,(&static_cast<TColorBinaryTreeNode*>(nullptr)->Child-static_cast<decltype(&static_cast<TColorBinaryTreeNode*>(nullptr)->Child)>(nullptr))
				*sizeof(static_cast<TColorBinaryTreeNode*>(nullptr)->Child)
			,(&static_cast<TColorBinaryTreeNode*>(nullptr)->Branch-static_cast<decltype(&static_cast<TColorBinaryTreeNode*>(nullptr)->Branch)>(nullptr))
				*sizeof(static_cast<TColorBinaryTreeNode*>(nullptr)->Branch)
			,(&static_cast<TColorBinaryTreeNode*>(nullptr)->Color-static_cast<decltype(&static_cast<TColorBinaryTreeNode*>(nullptr)->Color)>(nullptr))
				*sizeof(static_cast<TColorBinaryTreeNode*>(nullptr)->Color)
		>::Value,
		LinkInstanceEndOffset=cnMath::TMax<uIntn
			,(&static_cast<TColorBinaryTreeNode*>(nullptr)->Parent-static_cast<decltype(&static_cast<TColorBinaryTreeNode*>(nullptr)->Parent)>(nullptr))
				*sizeof(static_cast<TColorBinaryTreeNode*>(nullptr)->Parent)+sizeof(static_cast<TColorBinaryTreeNode*>(nullptr)->Parent)
			,(&static_cast<TColorBinaryTreeNode*>(nullptr)->Child-static_cast<decltype(&static_cast<TColorBinaryTreeNode*>(nullptr)->Child)>(nullptr))
				*sizeof(static_cast<TColorBinaryTreeNode*>(nullptr)->Child)+sizeof(static_cast<TColorBinaryTreeNode*>(nullptr)->Child)
			,(&static_cast<TColorBinaryTreeNode*>(nullptr)->Branch-static_cast<decltype(&static_cast<TColorBinaryTreeNode*>(nullptr)->Branch)>(nullptr))
				*sizeof(static_cast<TColorBinaryTreeNode*>(nullptr)->Branch)+sizeof(static_cast<TColorBinaryTreeNode*>(nullptr)->Branch)
			,(&static_cast<TColorBinaryTreeNode*>(nullptr)->Color-static_cast<decltype(&static_cast<TColorBinaryTreeNode*>(nullptr)->Color)>(nullptr))
				*sizeof(static_cast<TColorBinaryTreeNode*>(nullptr)->Color)+sizeof(static_cast<TColorBinaryTreeNode*>(nullptr)->Color)
		>::Value,
	};

	typedef cnMemory::cPlainAlignedData<LinkInstanceEndOffset-LinkInstanceOffset,TColorBinaryTreeNode> tLinkInstance;

#endif	// cnLibrary_CPPFEATURE_DECLTYPE >= 200707L


	static tNode* LinkNode(tLinkInstance &Link)noexcept(true){
		return reinterpret_cast<tNode*>(
			reinterpret_cast<uIntn>(&Link)-LinkInstanceOffset
		);
	}
	static const tNode* LinkNode(const tLinkInstance &Link)noexcept(true){
		return reinterpret_cast<tNode*>(
			reinterpret_cast<uIntn>(&Link)-LinkInstanceOffset
		);
	}



	static void ResetNode(tNode *Node)noexcept(true){
		Node->Parent=nullptr;
		Node->Child[0]=nullptr;
		Node->Child[1]=nullptr;
		Node->Color=0;
	}

	static ufInt8& NodeColorRef(tNode *Node)noexcept(true){
		return Node->Color;
	}

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
