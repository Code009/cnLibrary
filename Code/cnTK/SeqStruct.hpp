/*- cnTK - Sequence Data Struct -------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2018-08-14                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnTK_DataStruct_H__
#define	__cnLibrary_cnTK_DataStruct_H__
/*-------------------------------------------------------------------------*/
#include <cnTK/Common.hpp>
#include <cnTK/TypeTraits.hpp>
#include <cnTK/Memory.hpp>
#include <cnTK/Array.hpp>
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
template<class TAllocationOperator,class TElement>
class cSeqList
{
public:
	typedef TElement tItem;
	typedef TElement *tPointer;
	typedef const TElement *tConstPointer;
	typedef cArray<TElement> tArray;
	typedef cArray<const TElement> tConstArray;
	typedef cArrayAllocation<TAllocationOperator,TElement> tArrayAllocation;
	typedef cArrayMemoryFunction<TAllocationOperator,TElement> TAMFunc;

	typedef tPointer tIterator;
	typedef tConstPointer tConstIterator;

	cSeqList()=default;
	~cSeqList()=default;
	cSeqList(const cSeqList &Src)noexcept(true)
		: fArray(Src.fArray){}
	cSeqList(const tConstArray &Src)noexcept(true)
		: fArray(Src){}

#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L
	cSeqList(cSeqList &&Src)noexcept(true)
		: fArray(static_cast<tArrayAllocation&&>(Src.fArray)){}
	cSeqList(tArrayAllocation &&Src)noexcept(true)
		: fArray(static_cast<tArrayAllocation&&>(Src)){}
#endif	// cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

	// = copy
	cSeqList& operator =(const cSeqList &Src)noexcept(true){
		if(this!=&Src){
			fArray=Src.fArray;
		}
		return *this;
	}

	// copy = common array interface
	cSeqList& operator =(const tConstArray &Src)noexcept(true){
		if(ArrayCast<tConstArray>(&fArray)!=&Src){
			fArray=Src;
		}
		return *this;
	}

#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

	// = move
	cSeqList& operator =(cSeqList &&Src)noexcept(true){
		fArray=static_cast<tArrayAllocation&&>(Src.fArray);
		return *this;
	}
	// move = common array interface
	cSeqList& operator =(tArrayAllocation &&Src)noexcept(true){
		fArray=static_cast<tArrayAllocation&&>(Src);
		return *this;
	}
#endif	// cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

	// common storage interface
	cArrayStorage<TAllocationOperator,TElement> &Storage(void)noexcept(true){	return fArray;	}
	const cArrayStorage<TAllocationOperator,TElement> &Storage(void)const noexcept(true){	return fArray;	}


	// Swap
    //	swap memory block
    cArrayStorage<TAllocationOperator,TElement> Swap(void)noexcept(true){
		cArrayStorage<TAllocationOperator,TElement> Swapped(cnLib_UREFCAST(tArrayAllocation)(fArray));
		return Swapped;
	}

#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L
	// Swap
	//	swap memory block
	cArrayStorage<TAllocationOperator,TElement> Swap(tArrayAllocation &&Src)noexcept(true){
		cArrayStorage<TAllocationOperator,TElement> Swapped(static_cast<tArrayAllocation&&>(fArray));
		fArray=static_cast<tArrayAllocation&&>(Src);
		return Swapped;
	}
#endif	// cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

	operator tPointer ()const noexcept(true){		return fArray.Pointer;	}

	// GetItems
	// return address of items in the list
	tPointer GetItems(void)const noexcept(true){	return fArray.Pointer;	}

	// GetItems
	// return address of items in the list
	tPointer GetItemAt(uIntn Index)const noexcept(true){	return fArray.Pointer+Index;	}
	const tItem& RefItemAt(uIntn Index)const noexcept(true){	return fArray.Pointer[Index];	}

	// GetCapacity
	// return current capcacity
	uIntn GetCapacity(void)const noexcept(true){	return fArray.Capacity;	}
	// SetCapacity
	//	change capacity
	// NewCapacity		new number of item can hold of the struct
	void SetCapacity(uIntn NewCapacity)noexcept(true){	return fArray.SetCapacity(NewCapacity);		}

	// GetCount
	// return number items in the list
	uIntn GetCount(void)const noexcept(true){		return fArray.Length;	}
	// SetCount
	void SetCount(uIntn NewLength)noexcept(true){	return fArray.SetLength(NewLength);	}

	// Clear
	//	remove all items
	void Clear(void)noexcept(true){	fArray.Clear();	}

	// Compact
	void Compact(void)noexcept(true){		return fArray.SetCapacity(fArray.Length);	}


	// for range
	tIterator	begin(void)const noexcept(true){	return fArray.Pointer;	}
	tIterator	end(void)const noexcept(true){		return fArray.Pointer+fArray.Length;	}


	// ReplaceAt
	//	Replace ReplaceCount data items at the specifed position with ReplacementCount data items
	// Index				index to replace
	// ReplaceCount			count of items to remove
	// ReplacementCount		count of items to insert
	// return iterator of item where replaced
	tIterator ReplaceAt(uIntn Index,uIntn ReplaceCount,uIntn ReplacementCount)noexcept(true){
		if(Index>fArray.Length)
			Index=fArray.Length;
		fArray.ReplaceWithUninitialized(Index,ReplaceCount,ReplacementCount);
		tPointer ReplacementPointer=fArray.Pointer+Index;
		if(ReplacementCount!=0){
			TAMFunc::Construct(ReplacementPointer,0,ReplacementCount);
		}
		return ReplacementPointer;
	}
	// Replace
	//	Replace ReplaceCount data items at the specifed position with ReplacementCount data items
	// Position				iterator of the position to replace
	// ReplaceCount			count of items to remove
	// ReplacementCount		count of items to insert
	// return iterator of item where replaced
	tIterator Replace(tIterator Position,uIntn ReplaceCount,uIntn ReplacementCount)noexcept(true){
		uIntn Index=Position-fArray.Pointer;
		return ReplaceAt(Index,ReplaceCount,ReplacementCount);
	}

	// ReplaceAt
	//	Replace ReplaceCount data items at the specifed position with ReplacementCount data items
	// Index					index to replace
	// ReplaceCount				count of items to remove
	// [in]ReplacementItems		item to insert
	// ReplacementCount			count of items to insert
	// return iterator of item where replaced
	tIterator ReplaceAt(uIntn Index,uIntn ReplaceCount,tConstPointer ReplacementItems,uIntn ReplacementCount)noexcept(true){
		if(ReplacementItems==nullptr){
			return ReplaceAt(Index,ReplaceCount,ReplacementCount);
		}
		if(Index>fArray.Length)
			Index=fArray.Length;
		fArray.ReplaceWithUninitialized(Index,ReplaceCount,ReplacementCount);
		tPointer ReplacementPointer=fArray.Pointer+Index;
		TAMFunc::CopyConstruct(ReplacementPointer,0,ReplacementItems,0,ReplacementCount);
		return ReplacementPointer;
	}

	// Replace
	//	Replace ReplaceCount data items at the specifed position with ReplacementCount data items
	// Iterator					iterator of the position to replace
	// ReplaceCount				count of items to remove
	// [in]ReplacementItems		item to insert
	// ReplacementCount			count of items to insert
	// return iterator of item where replaced
	tIterator Replace(tIterator Position,uIntn ReplaceCount,tConstPointer ReplacementItems,uIntn ReplacementCount)noexcept(true){
		uIntn Index=Position-fArray.Pointer;
		return ReplaceAt(Index,ReplaceCount,ReplacementItems,ReplacementCount);
	}

	// Append
	//	Add items to the end of list
	// Count		number of data in array to add
	// return iterator of item where appended
	tIterator Append()noexcept(true){
		return Append_temp(1);
	}
	tIterator Append_temp(uIntn Count=1)noexcept(true){
		auto AppendedItems=AppendUninitialized(Count);
		// default construct
		TAMFunc::Construct(AppendedItems,0,Count);
		return AppendedItems;
	}
	tIterator Append(tConstPointer Items,uIntn Count)noexcept(true){
		if(Items==nullptr){
			return Append_temp(Count);
		}
		auto AppendedItems=AppendUninitialized(Count);
		// copy construct
		TAMFunc::CopyConstruct(AppendedItems,0,Items,0,Count);
		return AppendedItems;
	}

	// AppendMake
	//	Add a item to the end of list
	// Args			item construct parameters
	// return iterator of item where appended
	template<class...VT>
	tIterator AppendMake(VT cnLib_UREF...Args)noexcept(true){
		auto AppendedItem=AppendUninitialized(1);
		cnVar::ManualConstruct(*AppendedItem,cnLib_UREFCAST(VT)(Args)...);
		return AppendedItem;
	}

	// InsertAt
	//	Add data items to the specifed position of the list, default construct
	// Index		index to insert
	// Count		number of data in array to insert
	// return iterator of newly inserted item
	tIterator InsertAt(uIntn Index,uIntn Count=1)noexcept(true){
		tPointer InsertedItem=InsertUninitializedAt(Index,Count);
		// default construct
		TAMFunc::Construct(InsertedItem,0,Count);
		return InsertedItem;
	}
	// InsertItems
	//	Add data items to the specifed position of the list, default construct
	// Index		index to insert
	// Count		number of data in array to insert
	// return iterator of newly inserted item
	tIterator Insert(tIterator Position,uIntn Count=1)noexcept(true){
		uIntn Index=Position-fArray.Pointer;
		return InsertAt(Index,Count);
	}


	// InsertAt
	//	Add data items to the specifed position of the list, default construct
	// Index		index to insert
	// [in]Items		item to insert
	// Count		number of data in array to insert
	// return iterator of newly inserted item
	tIterator InsertAt(uIntn Index,tConstPointer Items,uIntn Count)noexcept(true){
		if(Items==nullptr){
			// default construct
			return InsertAt(Index,Count);
		}

		// insert space
		tPointer InsertedItem=InsertUninitializedAt(Index,Count);
		// manually copy construct
		TAMFunc::CopyConstruct(InsertedItem,0,Items,0,Count);
		return InsertedItem;
	}

	// InsertItems
	//	Add data items to the specifed position of the list, default construct
	// Position		iterator of item to insert before
	// [in]Items		item to insert
	// Count		number of data in array to insert
	// return iterator of newly inserted item
	tIterator Insert(tIterator Position,tConstPointer Items,uIntn Count)noexcept(true){
		uIntn Index=Position-fArray.Pointer;
		return InsertAt(Index,Items,Count);
	}

	// InsertMakeAt
	//	Add a data item to the specifed position of the list
	// Index		index to insert
	// Args			data construct parameters
	// return iterator of newly inserted item
	template<class...VT>
	tIterator InsertMakeAt(uIntn Index,VT cnLib_UREF...Args)noexcept(true){
		// insert space
		tPointer InsertedItem=InsertUninitializedAt(Index,1);
		// manually copy construct
		cnVar::ManualConstruct(*InsertedItem,cnLib_UREFCAST(VT)(Args)...);
		return InsertedItem;
	}

	// InsertMake
	//	Add a data item to the specifed position of the list
	// Position		iterator of the position to insert
	// Args			data construct parameters
	// return iterator of newly inserted item
	template<class...VT>
	tIterator InsertMake(tIterator Position,VT cnLib_UREF...Args)noexcept(true){
		uIntn Index=Position-fArray.Pointer;
		return InsertMakeAt(Index,cnLib_UREFCAST(VT)(Args)...);
	}


	// RemoveTail
	//	Remove items from the end of list
	// RemoveCount		number of item to remove
	void RemoveTail(uIntn RemoveCount=1)noexcept(true){
		return fArray.Truncate(RemoveCount);
	}

	// RemoveAt
	//	Remove items at specifed position
	// Index			index of item to remove
	// RemoveCount		number of item to remove
	void RemoveAt(uIntn Index,uIntn RemoveCount=1)noexcept(true){
		return fArray.Remove(Index,RemoveCount);
	}

	// Remove
	//	Remove items at specifed position
	// Position			iterator of item to remove
	// RemoveCount		number of item to remove
	void Remove(tIterator Position,uIntn RemoveCount=1)noexcept(true){
		uIntn Index=Position-fArray.Pointer;
		return RemoveAt(Index,RemoveCount);
	}


protected:
	cArrayStorage<TAllocationOperator,TElement> fArray;

	// AppendUninitialized
	//	Add space for specifed count of items to the end of the list
	//	note: the function is to create data space, it won't do anything as well as initialize with it
	//		user MUST initialize them manually
	// Count	number of space for item
	// return address of newly added space
	tIterator AppendUninitialized(uIntn Count=1)noexcept(true){
		uIntn AppendIndex=fArray.Length;
		uIntn NewCount=AppendIndex+Count;
		if(NewCount>fArray.Capacity){
			fArray.GrowCapacityTo(NewCount);
		}
		fArray.Length=NewCount;
		return fArray.Pointer+AppendIndex;
	}
	// InsertUninitializedAt
	//	Add space for specifed count of items to specifed position of the list
	//	note: the function use to create data space, it won't do anything as well as initialize with it
	//		user MUST initialize them manually
	// Index		index of space start to add
	// Count		number of space for item
	// return address of newly added space
	tIterator InsertUninitializedAt(uIntn Index,uIntn Count=1)noexcept(true){
		if(Index>fArray.Length)
			Index=fArray.Length;
		fArray.ReplaceWithUninitialized(Index,0,Count);
		return fArray.Pointer+Index;
	}

public:

	template<class TIndex>
	cnLib_DEPRECATED typename cnVar::TTypeConditional<tPointer,
		cnVar::TIsConvertible<TIndex,uIntn>::Value
	>::Type Replace(TIndex Index,uIntn ReplaceLength,tConstPointer ReplacementData,uIntn ReplacementLength){
		return ReplaceAt(static_cast<uIntn>(Index),ReplaceLength,ReplacementLength);
	}

	cnLib_DEPRECATED tPointer AppendItems(uIntn Count);
	cnLib_DEPRECATED tPointer AppendItems(tConstPointer Data,uIntn Count);

	cnLib_DEPRECATED tPointer InsertItemsAt(uIntn Index,uIntn Count);
	cnLib_DEPRECATED tPointer InsertItems(tPointer Iterator,uIntn Count);
	cnLib_DEPRECATED tPointer InsertItemsAt(uIntn Index,tConstPointer Data,uIntn Count);
	cnLib_DEPRECATED tPointer InsertItems(tPointer Iterator,tConstPointer Data,uIntn Count);
	template<class TIndex,class...VT>
	cnLib_DEPRECATED typename cnVar::TTypeConditional<tPointer,
		cnVar::TIsConvertible<TIndex,uIntn>::Value
	>::Type Insert(TIndex Index,VT cnLib_UREF...Args){
		return InsertAt(static_cast<uIntn>(Index),cnLib_UREFCAST(VT)(Args)...);
	}
	// InsertItems
	//	Add data items to the specifed position of the list, default construct
	// Iterator		iterator of item to insert before
	// Count		number of data in array to insert
	// return iterator of newly inserted item
	template<class TIndex>
	cnLib_DEPRECATED typename cnVar::TTypeConditional<tPointer,
		cnVar::TIsConvertible<TIndex,uIntn>::Value
	>::Type InsertItems(TIndex Index,uIntn Count){
		return InsertItemsAt(static_cast<uIntn>(Index),Count);
	}
	// InsertItems
	//	Add data items to the specifed position of the list, default construct
	// Index		index to insert
	// Count		number of data in array to insert
	// return true if added, false if out of range
	template<class TIndex>
	cnLib_DEPRECATED typename cnVar::TTypeConditional<tPointer,
		cnVar::TIsConvertible<TIndex,uIntn>::Value
	>::Type InsertItems(TIndex Index,tConstPointer Data,uIntn Count){
		return InsertItemsAt(static_cast<uIntn>(Index),Data,Count);
	}

	// Remove
	//	Remove items at specifed position
	// Index			index of item to remove
	// RemoveCount		number of item to remove
	template<class TIndex>
	cnLib_DEPRECATED typename cnVar::TTypeConditional<void,
		cnVar::TIsConvertible<TIndex,uIntn>::Value
	>::Type Remove(TIndex Index,uIntn RemoveCount=1){
		return RemoveAt(static_cast<uIntn>(Index),RemoveCount);
	}


	// Search
	//	search for data
	// [in]Dest			data to search for
	// [in]StartIndex	index where begins search
	template<class T>
	cnLib_DEPRECATED uIntn Search(const T &Dest,uIntn StartIndex=0){
		tPointer *Items=fArray.Pointer;
		for(uIntn i=StartIndex,c=this->fCount;i<c;i++){
			if(Items[i]==Dest)
				return i;
		}
		return IndexNotFound;
	}


	cnLib_DEPRECATED void RemoveTailItems(uIntn RemoveCount){
		return fArray.Truncate(RemoveCount);
	}

	// Remove
	//	Remove items at specifed position
	// [in]Index	index of item to remove
	// [in]Count	number of item to remove
	// return item count removed
	cnLib_DEPRECATED void RemoveItems(uIntn Index,uIntn RemoveCount){
		return fArray.Remove(Index,RemoveCount);
	}


	cnLib_DEPRECATED static void SwapItem(tPointer Dest,tPointer Src){
		cnLib_ASSERT(Src!=nullptr);
		cnLib_ASSERT(Dest!=nullptr);

		typename cnVar::TRemovePointer<tPointer>::Type Temp(cnVar::MoveCast(*Dest));
		*Dest=cnVar::MoveCast(*Src);
		*Src=cnVar::MoveCast(Temp);
	}
	template<class CompareFunction>
	cnLib_DEPRECATED void Sort(CompareFunction &CompareLess){
		auto Items=GetItems();
		///auto Count=fList.GetCount();		
		uIntn Count=this->GetCount();

		for(uIntn i=0;i<Count;i++){
			
			for(uIntn j=i;j<Count;j++){
				bool flag = CompareLess(Items[i],Items[j]);
				if( flag == true ){
					SwapItem(Items+i,Items+j);


				}
			}
		}
	}

};

//---------------------------------------------------------------------------

template<class TAllocationOperator,class TElement,class TItemOrderOperator>
class cSeqSet
{
public:
	typedef TElement tItem;
	typedef TElement *tPointer;
	typedef const TElement *tConstPointer;
	typedef cArray<TElement> tArray;
	typedef cArray<const TElement> tConstArray;
	typedef cArrayAllocation<TAllocationOperator,TElement> tArrayAllocation;

	typedef tConstPointer tIterator;
	typedef tConstPointer tConstIterator;

	cSeqSet()=default;
	~cSeqSet()=default;

	cSeqSet(uIntn InitialCapacity)noexcept(true)
		: fArray(nullptr,0,InitialCapacity){}

	cSeqSet(const cSeqSet &Src)noexcept(true)
		: fArray(Src.fArray){}
	cSeqSet(const tConstArray &Src)noexcept(true)
		: fArray(Src){}

#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

	cSeqSet(cSeqSet &&Src)noexcept(true)
		: fArray(static_cast<tArrayAllocation&&>(Src.fArray)){}
	cSeqSet(tArrayAllocation &&Src)noexcept(true)
		: fArray(static_cast<tArrayAllocation&&>(Src)){}

#endif	// cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

	// = copy
	cSeqSet& operator =(const cSeqSet &Src)noexcept(true){
		if(this!=&Src){
			fArray=Src.fArray;
		}
		return *this;
	}

	// copy = common array interface
	cSeqSet& operator =(const tConstArray &Src)noexcept(true){
		if(ArrayCast<tConstArray>(&fArray)!=&Src){
			fArray=Src;
		}
		return *this;
	}

#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

	// = move
	cSeqSet& operator =(cSeqSet &&Src)noexcept(true){
		fArray=static_cast<tArrayAllocation&&>(Src.fArray);
		return *this;
	}

	// move = common array interface
	cSeqSet& operator =(tArrayAllocation &&Src)noexcept(true){
		fArray=static_cast<tArrayAllocation&&>(Src);
		return *this;
	}
#endif	// cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

	// common storage interface
	tArrayAllocation &Storage(void)noexcept(true){	return fArray;	}
	const tArrayAllocation &Storage(void)const noexcept(true){	return fArray;	}


	// Swap
    //	swap memory block
    cArrayStorage<TAllocationOperator,TElement> Swap(void)noexcept(true){
		cArrayStorage<TAllocationOperator,TElement> Swapped(cnLib_UREFCAST(tArrayAllocation)(fArray));
		return Swapped;
	}

#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L
	// Swap
    //	swap memory block
    cArrayStorage<TAllocationOperator,TElement> Swap(tArrayAllocation &&Src)noexcept(true){
		cArrayStorage<TAllocationOperator,TElement> Swapped(static_cast<tArrayAllocation&&>(fArray));
		fArray=static_cast<tArrayAllocation&&>(Src);
		return Swapped;
	}
#endif	// cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

	operator tConstPointer()const noexcept(true){	return fArray.Pointer;	}

	tConstPointer GetItems(void)const noexcept(true){
		return fArray.Pointer;
	}
	const tItem* GetItemAt(uIntn Index)const noexcept(true){	return fArray.Pointer+Index;	}
	const tItem& RefItemAt(uIntn Index)const noexcept(true){	return fArray.Pointer[Index];	}

	// SetCapacity
	//	change capacity
	// NewCapacity		new number of item can hold of the struct
	void SetCapacity(uIntn NewCapacity)noexcept(true){
		return fArray.SetCapacity(NewCapacity);
	}
	// GetCapacity
	// return current capcacity
    uIntn GetCapacity(void)const noexcept(true){
		return fArray.Capacity;
	}

	// GetCount
	// return number items in the list
    uIntn GetCount(void)const noexcept(true){
		return fArray.Length;
	}

	// Clear
	//	remove all items
	void Clear(void)noexcept(true){	fArray.Clear();		}

	// Compact
	void Compact(void)noexcept(true){		return fArray.SetCapacity(fArray.Length);		}

	// for range
	tIterator	begin(void)const noexcept(true){	return fArray.Pointer;	}
	tIterator	end(void)const noexcept(true){		return fArray.Pointer+fArray.Length;	}
	tIterator	cbegin(void)const noexcept(true){	return fArray.Pointer;	}
	tIterator	cend(void)const noexcept(true){		return fArray.Pointer+fArray.Length;	}

	// Insert
	//	Insert an item
	// [in]Item
	// return true if the key inserted as new item
	template<class T>
	bool Insert(T cnLib_UREF Item)noexcept(true){
		uIntn Index;
		// search
		if(Search(Index,Item)==true){
			// already exists
			return false;
		}
		// insert space
		fArray.ReplaceWithUninitialized(Index,0,1);
		// initialize key data and value
		cnVar::ManualConstruct(fArray.Pointer[Index],cnLib_UREFCAST(T)(Item));
		return true;
	}

	// Insert
	//	Insert an item
	// [in]Item
	// return true if the key inserted as new item
	template<class T>
	tIterator Insert(bool &Exists,T cnLib_UREF Item)noexcept(true){
		uIntn Index;
		// search
		Exists=Search(Index,Item);
		if(Exists==false){
			// insert space
			fArray.ReplaceWithUninitialized(Index,0,1);
			// initialize key data and value
			cnVar::ManualConstruct(fArray.Pointer[Index],cnLib_UREFCAST(T)(Item));
		}
		return GetItemAt(Index);
	}

	// Remove
    //	Remove a item has the same value specified in Item
	// [in]Item
    // return true if item is removed
	template<class T>
    bool Remove(const T &Item)noexcept(true){
		uIntn Index;
		if(Search(Index,Item)==false)
			return false;
		// found
		fArray.Remove(Index,1);
		return true;
	}
	// Remove
    //	Remove a item at specified position
	// Iterator			iterator of item to be removed
	// Count			number of items to remove
    void Remove(tIterator Position,uIntn Count=1)noexcept(true){
		uIntn Index=Position-fArray.Pointer;
		fArray.Remove(Index,Count);
	}

	// RemoveAt
    //	Remove a item at specified position
	// Index		Index of item to be removed
	// Count			number of items to remove
    void RemoveAt(uIntn Index,uIntn Count=1)noexcept(true){
		fArray.Remove(Index,Count);
	}

	template<class T>
    tIterator Find(const T &Item)const noexcept(true){
		uIntn Index;
		if(Search(Index,Item)==true){
			return GetItemAt(Index);
		}
		return end();
	}


    // IndexOf
    //	Get index of item in the set that has the same value to the parameter Item
	// [in]Item				key
    //	return: the Index of the key
	template<class T>
    uIntn IndexOf(const T &Item)const noexcept(true){
		uIntn Index;
		if(Search(Index,Item)==true){
			return Index;
		}
		return IndexNotFound;
	}

    // IndexOf
    //	Get index of iterator
	// [in]Position			iterator
    //	return: the Index of the iterator
    uIntn IndexOf(tIterator Position)const noexcept(true){
		return fArray.Pointer-Position;
	}

protected:
	cArrayStorage<TAllocationOperator,TElement> fArray;

	template<class TCompareValue>
	struct cComparisonFunctor
	{
		const TCompareValue &CompareValue;
#if cnLibrary_CPPFEATURE_DECLTYPE >= 200707L
		auto operator () (const tItem &Element)noexcept(true)->decltype(TItemOrderOperator::Compare(Element,CompareValue))
#else
		sfInt8 operator () (const tItem &Element)
#endif
		{
			return TItemOrderOperator::Compare(Element,CompareValue);
		}
	};
public:
	// Search
	//	search a data
	//	internal use
	// [out]Result		if found, it is the index of data, if not, it is the index to insert
	// [in]Item				data to find
	// return true if data found, or false if the data does not exist
	template<class TValue>
	bool Search(uIntn &Result,const TValue &Value)const noexcept(true){
		cComparisonFunctor<TValue> Comparison={Value};
		return cnMemory::ViewBinarySearch(Result,fArray.Pointer,fArray.Length,Comparison);
	}
};

template<class TKey,class TValue>
class cSeqMapIterator
{
public:
	cSeqMapIterator()noexcept(true){}
	cSeqMapIterator(const TKey *KeyPtr,TValue *ValuePtr)noexcept(true){
		fKeyPtr=KeyPtr;
		fValuePtr=ValuePtr;
	}

	struct tElementRef{
		const TKey &Key;
		TValue &Value;
	};

	const tElementRef& operator *(void)const noexcept(true){
		return *reinterpret_cast<const tElementRef*>(this);
	}
	const tElementRef* operator ->(void)const noexcept(true){
		return reinterpret_cast<const tElementRef*>(this);
	}

#ifndef cnLibrary_CPPEXCLUDE_NULLPTR
	bool operator ==(tNullptr)const noexcept(true){
		return fKeyPtr==nullptr;
	}
	bool operator !=(tNullptr)const noexcept(true){
		return fKeyPtr!=nullptr;
	}
#endif

	bool operator ==(const cSeqMapIterator &Dest)const noexcept(true){
		return fKeyPtr==Dest.fKeyPtr;
	}
	bool operator !=(const cSeqMapIterator &Dest)const noexcept(true){
		return fKeyPtr!=Dest.fKeyPtr;
	}
	cSeqMapIterator& operator ++(void)noexcept(true){
		fKeyPtr++;
		fValuePtr++;
		return *this;
	}
	void operator ++(int)noexcept(true){
		fKeyPtr++;
		fValuePtr++;
	}
	cSeqMapIterator& operator --(void)noexcept(true){
		fKeyPtr--;
		fValuePtr--;
		return *this;
	}
	void operator --(int)noexcept(true){
		fKeyPtr--;
		fValuePtr--;
	}
	cSeqMapIterator operator +(uIntn Offset)const noexcept(true){
		cSeqMapIterator p;
		p.fKeyPtr=fKeyPtr+Offset;
		p.fValuePtr=fValuePtr+Offset;
		return p;
	}
	cSeqMapIterator operator -(uIntn Offset)const noexcept(true){
		cSeqMapIterator p;
		p.fKeyPtr=fKeyPtr-Offset;
		p.fValuePtr=fValuePtr-Offset;
		return p;
	}
	cSeqMapIterator& operator +=(uIntn Offset)noexcept(true){
		fKeyPtr+=Offset;
		fValuePtr+=Offset;
		return *this;
	}
	cSeqMapIterator& operator -=(uIntn Offset)noexcept(true){
		fKeyPtr-=Offset;
		fValuePtr-=Offset;
		return *this;
	}

	sIntn operator -(const cSeqMapIterator &Ptr)const noexcept(true){
		return fKeyPtr-Ptr.fKeyPtr;
	}

	tElementRef operator [](uIntn Index)noexcept(true){
		tElementRef Ref={
			fKeyPtr+Index,
			fValuePtr+Index,
		};
		return Ref;
	}

	bool operator <(const cSeqMapIterator &Dest)const noexcept(true){
		return fKeyPtr<Dest.fKeyPtr;
	}
	bool operator <=(const cSeqMapIterator &Dest)const noexcept(true){
		return fKeyPtr<=Dest.fKeyPtr;
	}
	bool operator >(const cSeqMapIterator &Dest)const noexcept(true){
		return fKeyPtr>Dest.fKeyPtr;
	}
	bool operator >=(const cSeqMapIterator &Dest)const noexcept(true){
		return fKeyPtr>=Dest.fKeyPtr;
	}
protected:
	const TKey *fKeyPtr;
	TValue *fValuePtr;
};

template<class TKeyAllocationOperator,class TKey,class TValueAllocationOperator,class TValue,class TKeyItemOrderOperator>
class cSeqMap
{
public:
	typedef TKey tKey;
	typedef TValue tValue;
	typedef cMultiArray<TKey,TValue> tArray;
	typedef cArrayAllocation<TKeyAllocationOperator,TKey> tKeyArrayAllocation;
	typedef cArrayAllocation<TValueAllocationOperator,TValue> tValueArrayAllocation;
	typedef cMultiArrayAllocation<tKeyArrayAllocation,tValueArrayAllocation> tArrayAllocation;
	typedef cMultiArrayMemoryFunction<tKeyArrayAllocation,tValueArrayAllocation> TMAMFunc;
	typedef cMultiArrayStorage<tKeyArrayAllocation,tValueArrayAllocation> tMultiArrayStorage;

	typedef cSeqMapIterator<tKey,tValue> tIterator;
	typedef cSeqMapIterator<tKey,const tValue> tConstIterator;

	cSeqMap()noexcept(true){}
	~cSeqMap()noexcept(true){}
	// copy
	cSeqMap(const cSeqMap &Src)noexcept(true)
		: fArray(Src.fArray){}
#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L
	// move
	cSeqMap(cSeqMap &&Src)noexcept(true)
		: fArray(static_cast<tArrayAllocation&&>(Src.fArray)){}
#endif	// cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

	// = copy
	cSeqMap& operator =(const cSeqMap &Src)noexcept(true){
		if(this!=&Src){
			fArray=Src.fArray;
		}
		return *this;
	}
	cSeqMap(const tArray &Src)noexcept(true)
		: fArray(Src){}


    cSeqMap& operator =(const tArray &Src)noexcept(true){
		if(&fArray!=&Src){
			fArray=Src;
		}
		return *this;
	}

#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L
	// = move
	cSeqMap& operator =(cSeqMap &&Src)noexcept(true){
		fArray=static_cast<tArrayAllocation&&>(Src.fArray);
		return *this;
	}
	cSeqMap(tArrayAllocation &&Src)noexcept(true)
		: fArray(static_cast<tArrayAllocation&&>(Src)){}

	cSeqMap& operator =(tArrayAllocation &&Src)noexcept(true){
		fArray=static_cast<tArrayAllocation&&>(Src);
		return *this;
	}

#endif	// cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

	tArrayAllocation &Storage(void)noexcept(true){	return fArray;	}
	const tArrayAllocation &Storage(void)const noexcept(true){	return fArray;	}

	// Swap
    //	swap memory block
    tMultiArrayStorage Swap(void)noexcept(true){
		tMultiArrayStorage Swapped(cnLib_UREFCAST(tArrayAllocation)(fArray));
		return Swapped;
	}
#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L
	// Swap
    //	swap memory block
    tMultiArrayStorage Swap(tArrayAllocation &&Src)noexcept(true){
		tMultiArrayStorage Swapped(static_cast<tArrayAllocation&&>(fArray));
		fArray=static_cast<tArrayAllocation&&>(Src);
		return Swapped;
	}

#endif	// cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

	// GetCapacity
	// return current capcacity
    uIntn GetCapacity(void)const noexcept(true){
		return fArray.Capacity;
	}
	// SetCapacity
	//	change capacity
	// NewCapacity		new number of item can hold of the struct
	void SetCapacity(uIntn NewCapacity)noexcept(true){
		NewCapacity=tArrayAllocation::BufferSizeRoundup(NewCapacity);
		return fArray.SetCapacity(NewCapacity);
	}

	// GetCount
	// return number items in the map
    uIntn GetCount(void)const noexcept(true){
		return fArray.Length;
	}

	// Clear
	//	remove all items
	void Clear(void)noexcept(true){	fArray.SetLength(0);	}

	// Compact
	void Compact(void)noexcept(true){	return fArray.SetCapacity(fArray.Length);	}


	// IndexOf
	//	Get index of the specified Key.
	// [in]Key				key
	//	return: the Index of the key
	template<class T>
	uIntn IndexOf(const T &Key)const noexcept(true){
		uIntn Index;
		if(SearchKey(Index,Key)==true){
			return Index;
		}
		return IndexNotFound;
	}

	// GetValues
    //	Get item pointer to the value list
    // return the pointer to values
	tValue* GetValues(void)const noexcept(true){
		return *+fArray.Pointer;
	}
	// GetKeys
    //	Get item pointer to the key list
    // return the pointer to keys
	tKey* GetKeys(void)const noexcept(true){
		return *fArray.Pointer;
	}

	tIterator begin(void)const noexcept(true){	return Pair(GetKeys(),GetValues());	}
	tIterator end(void)const noexcept(true){	return Pair(GetKeys()+fArray.Length,GetValues()+fArray.Length);	}
	tConstIterator cbegin(void)const noexcept(true){	return Pair(GetKeys(),GetValues());	}
	tConstIterator cend(void)const noexcept(true){	return Pair(GetKeys()+fArray.Length,GetValues()+fArray.Length);	}


	template<class T>
	tValue& operator[](const T &Key)noexcept(true){
		uIntn Index;
		// search
		if(SearchKey(Index,Key)==false){
			// insert space
			fArray.ReplaceWithUninitialized(Index,0,1);
			cnVar::ManualConstructZero(GetValues()[Index]);
			cnVar::ManualConstruct(GetKeys()[Index],Key);
		}
		return GetValues()[Index];
	}



	// GetPair
    //	Get reference of item pair at specified index.
	// Index		index of item pair
    // return iterator to the item paire
	tIterator GetPairAt(uIntn Index)const noexcept(true){
		return Pair(GetKeys()+Index,GetValues()+Index);
	}
	// GetPair
    //	Get reference of item pair to the specified Key.
	// [in]Key				key
    // return iterator to the item paire
	template<class T>
	tIterator GetPair(const T &Key)const noexcept(true){
		uIntn Index;
		if(SearchKey(Index,Key)==false)
			return NullPair();
		return GetPairAt(Index);
	}
	// IndexOfPair
	//	Get index of the specified Key.
	// [in]Key				key
	// return index of the item pair
	uIntn IndexOfPair(const tIterator &Pair)const noexcept(true){
		uIntn Index=Pair.Key-GetKeys();
		if(Index>=fArray.Length){
			return IndexNotFound;
		}
		return Index;
	}
	// InsertPair
	//	insert a pair
	// [in]Key		key of the pair
    // return iterator to the newly inserted item paire
	template<class T>
	tIterator InsertPair(T cnLib_UREF Key)noexcept(true){
		uIntn Index;
		// search
		if(SearchKey(Index,Key)){
			// already exists
		}
		else{
			// insert space
			fArray.ReplaceWithUninitialized(Index,0,1);
			// initialize key data and value

			TMAMFunc::ConstructElement(fArray.Pointer,Index,cnLib_UREFCAST(T)(Key));
		}
		return GetPairAt(Index);
	}
	// InsertPair
	//	insert a pair
	// [in]Key			key of the pair
	// [out]Exists		true if the item already exists
    // return iterator to the newly inserted item paire
	template<class TK,class...TV>
	tIterator InsertPair(TK cnLib_UREF Key,bool &Exists,TV cnLib_UREF...ValueArgs)noexcept(true){
		uIntn Index;
		// search
		if(SearchKey(Index,Key)){
			// already exists
			Exists=true;
		}
		else{
			// insert space
			fArray.ReplaceWithUninitialized(Index,0,1);
			// initialize key data and value

			TMAMFunc::ConstructElement(fArray.Pointer,Index,cnLib_UREFCAST(TK)(Key),tValue(cnLib_UREFCAST(TV)(ValueArgs)...));
			Exists=false;
		}
		return GetPairAt(Index);
	}
	// RemovePair
	bool RemovePair(const tIterator &Pair)noexcept(true){
		uIntn Index=&Pair->Key-GetKeys();
		if(Index>=fArray.Length){
			return false;
		}
		fArray.Remove(Index,1);
		return true;
	}

	// Insert
	//	Insert a item with default value
	// [in]Key			initializer for key
	//	return: true if the key inserted as new
	template<class TK,class...TV>
	bool Insert(TK cnLib_UREF Key,TV cnLib_UREF...ValueArgs)noexcept(true){
		uIntn Index;
		// search
		if(SearchKey(Index,Key)==true){
			// already exists
			return false;
		}
		// insert space
		fArray.ReplaceWithUninitialized(Index,0,1);
		// initialize key data and value
		TMAMFunc::ConstructElement(fArray.Pointer,Index,cnLib_UREFCAST(TK)(Key),tValue(cnLib_UREFCAST(TV)(ValueArgs)...));
		return true;
	}

	// Replace
	//	Insert a item or replace the value associated with the key
	// [in]Key			initializer for key
	// [in]Value		initializer for value
	//	return: true if the key inserted as new
	template<class TK,class TV>
	bool Replace(TK cnLib_UREF Key,TV cnLib_UREF Value)noexcept(true){
		uIntn Index;
		// search
		if(SearchKey(Index,Key)){
			// already exists
			auto Values=GetValues();
			Values[Index]=cnLib_UREFCAST(TV)(Value);
			return false;
		}
		else{
			// insert space
			fArray.ReplaceWithUninitialized(Index,0,1);
			// initialize key data and value
			TMAMFunc::ConstructElement(fArray.Pointer,Index,cnLib_UREFCAST(TK)(Key),cnLib_UREFCAST(TV)(Value));
			return true;
		}
	}

	// Remove
    //	Remove a Key and its associated Value
	// [in]Key		key
    //	return: true==success
	template<class T>
    bool Remove(const T &Key)noexcept(true){
		uIntn Index;
		if(SearchKey(Index,Key)==false)
			return false;
		// found
		fArray.Remove(Index,1);
		return true;
	}
	// RemoveAt
    //	Remove pair at specified position
	// [in]Index	Index
    void RemoveAt(uIntn Index,uIntn Count=1)noexcept(true){
		fArray.Remove(Index,Count);
	}

	// Get
    //	Get item pointer to the specified Key.
	// [in]Key				key
    // return pointer to the value of key
	template<class T>
	tValue* Get(const T &Key)const noexcept(true){
		uIntn Index;
		if(SearchKey(Index,Key)==false)
			return nullptr;
		return GetValues()+Index;
	}

protected:

	static tIterator Pair(const tKey *Keys,tValue *Values)noexcept(true){	return {Keys,Values};	}
	static tIterator NullPair(void)noexcept(true){	return {nullptr,nullptr};	}

	tMultiArrayStorage fArray;

	template<class TCompareKey>
	struct cComparisonFunctor
	{
		const TCompareKey &CompareKey;

#if cnLibrary_CPPFEATURE_DECLTYPE >= 200707L
		auto operator () (const tKey &Key)noexcept(true)->decltype(TKeyItemOrderOperator::Compare(Key,CompareKey))
#else
		sfInt8 operator () (const tKey &Key)noexcept(true)
#endif
		{
			return TKeyItemOrderOperator::Compare(Key,CompareKey);
		}
	};

	// Search
	//	search a data
	//	internal use
	// [out]Result	if found, it is the index of data, if not, it is the index to insert
	// [in]Data		data to find
	// return true if data found, or false if the data does not exist
	template<class T>
	bool SearchKey(uIntn &Result,const T &Key)const noexcept(true){
		cComparisonFunctor<T> Comparison={Key};
		return cnMemory::ViewBinarySearch(Result,*fArray.Pointer,fArray.Length,Comparison);
	}
};


template <class...TElements>
struct cSeqMultiListItemRef
{
	cnVar::cVarPack<TElements*...> Pointer;

	template<uIntn Index>
	typename cnVar::TSelect<Index,TElements...>::Type& Ref(void)const noexcept(true)
	{
		return *cnVar::VarPackAt<Index>(Pointer);
	}
};

template <class...TElements>
struct cSeqMultiListIterator : cSeqMultiListItemRef<TElements...>
{
	typedef cSeqMultiListItemRef<TElements...> tElementRef;

	const tElementRef& operator *(void)const noexcept(true){
		return *this;
	}
	const tElementRef* operator ->(void)const noexcept(true){
		return this;
	}

#ifndef cnLibrary_CPPEXCLUDE_NULLPTR
	bool operator ==(tNullptr)const noexcept(true){
		return *this->Pointer==nullptr;
	}
	bool operator !=(tNullptr)const noexcept(true){
		return *this->Pointer!=nullptr;
	}
#endif

	bool operator ==(const cSeqMultiListIterator &Dest)const noexcept(true){
		return *this->Pointer==*Dest.Pointer;
	}
	bool operator !=(const cSeqMultiListIterator &Dest)const noexcept(true){
		return *this->Pointer!=*Dest.Pointer;
	}
	cSeqMultiListIterator& operator ++(void)noexcept(true){
		cnVar::VarPackForEach([](auto* &Ptr){
			Ptr++;
		},this->Pointer);
		return *this;
	}
	void operator ++(int)noexcept(true){	operator ++();	}
	cSeqMultiListIterator& operator --(void)noexcept(true){
		cnVar::VarPackForEach([](auto* &Ptr){
			Ptr--;
		},this->Pointer);
		return *this;
	}
	void operator --(int)noexcept(true){	operator --();	}

	tElementRef operator [](sIntn Index)const noexcept(true){
		tElementRef Ret;
		cnVar::VarPackForEach([Index](auto* &RetPtr,auto* &SrcPtr){
			RetPtr=SrcPtr+Index;
		},Ret.Pointer,this->Pointer);
		return Ret;
	}

	cSeqMultiListIterator operator +(sIntn Offset)noexcept(true){
		cSeqMultiListIterator Ret;
		cnVar::VarPackForEach([Offset](auto* &RetPtr,auto* &SrcPtr){
			RetPtr=SrcPtr+Offset;
		},Ret.Pointer,this->Pointer);
		return Ret;
	}
	cSeqMultiListIterator& operator +=(sIntn Offset)noexcept(true){
		cnVar::VarPackForEach([Offset](auto* &Ptr){
			Ptr+=Offset;
		},this->Pointer);
		return *this;
	}
	cSeqMultiListIterator operator -(sIntn Offset)noexcept(true){
		cSeqMultiListIterator Ret;
		cnVar::VarPackForEach([Offset](auto* &RetPtr,auto* &SrcPtr){
			RetPtr=SrcPtr-Offset;
		},Ret.Pointer,this->Pointer);
		return Ret;
	}
	cSeqMultiListIterator& operator -=(uIntn Offset)noexcept(true){
		cnVar::VarPackForEach([Offset](auto* &Ptr){
			Ptr-=Offset;
		},this->Pointer);
		return *this;
	}

	sIntn operator -(const cnVar::cVarPack<TElements*...> &SrcPointer)const noexcept(true){
		return *this->Pointer-*SrcPointer;
	}
	sIntn operator -(const cSeqMultiListIterator &Ptr)const noexcept(true){
		return *this->Pointer-*Ptr.Pointer;
	}

	bool operator <(const cSeqMultiListIterator &Dest)const noexcept(true){
		return *this->Pointer<*Dest.Pointer;
	}
	bool operator <=(const cSeqMultiListIterator &Dest)const noexcept(true){
		return *this->Pointer<=*Dest.Pointer;
	}
	bool operator >(const cSeqMultiListIterator &Dest)const noexcept(true){
		return *this->Pointer>*Dest.Pointer;
	}
	bool operator >=(const cSeqMultiListIterator &Dest)const noexcept(true){
		return *this->Pointer>=*Dest.Pointer;
	}
};

template<class...VTArrayAllocation>
class cSeqMultiList
{
public:

	typedef cMultiArrayMemoryFunction<VTArrayAllocation...> TMAMFunc;
	typedef cMultiArrayAllocation<VTArrayAllocation...> tMultiArrayAllocation;
	typedef cMultiArray<typename VTArrayAllocation::tElement...> tArray;
	typedef cMultiArray<typename VTArrayAllocation::tElement const...> tConstArray;

	typedef typename TMAMFunc::tPointer tPointer;
	typedef typename TMAMFunc::tConstPointer tConstPointer;

	typedef cSeqMultiListIterator<typename VTArrayAllocation::tElement...> tIterator;

	cSeqMultiList()noexcept(true){}
	~cSeqMultiList()noexcept(true){}
	// copy
	cSeqMultiList(const cSeqMultiList &Src)noexcept(true)
		: fArray(Src.fArray){}

#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L
	// move
	cSeqMultiList(cSeqMultiList &&Src)noexcept(true)
		: fArray(static_cast<tMultiArrayAllocation&&>(Src.fArray)){}
#endif	// cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

	// = copy
	cSeqMultiList& operator =(const cSeqMultiList &Src)noexcept(true){
		if(this!=&Src){
			fArray=Src.fArray;
		}
		return *this;
	}
	cSeqMultiList(const tArray &Src)noexcept(true)
		: fArray(Src){}

    cSeqMultiList& operator =(const tArray &Src)noexcept(true){
		if(&fArray!=&Src){
			fArray=Src;
		}
		return *this;
	}

#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

	// = move
	cSeqMultiList& operator =(cSeqMultiList &&Src)noexcept(true){
		fArray=static_cast<tMultiArrayAllocation&&>(Src.fArray);
		return *this;
	}
	cSeqMultiList(tMultiArrayAllocation &&Src)noexcept(true)
		: fArray(static_cast<tMultiArrayAllocation&&>(Src)){}

	cSeqMultiList& operator =(tMultiArrayAllocation &&Src)noexcept(true){
		fArray=static_cast<tMultiArrayAllocation&&>(Src);
		return *this;
	}
#endif	// cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L


	tMultiArrayAllocation &Storage(void)noexcept(true){	return fArray;	}
	const tMultiArrayAllocation &Storage(void)const noexcept(true){	return fArray;	}

	// Swap
    //	swap memory block
    cnDataStruct::cMultiArrayStorage<VTArrayAllocation...> Swap(void)noexcept(true){
		cnDataStruct::cMultiArrayStorage<VTArrayAllocation...> Swapped(cnLib_UREFCAST(tMultiArrayAllocation)(fArray));
		return Swapped;
	}
#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L
	// Swap
    //	swap memory block
    cnDataStruct::cMultiArrayStorage<VTArrayAllocation...> Swap(tMultiArrayAllocation &&Src)noexcept(true){
		cnDataStruct::cMultiArrayStorage<VTArrayAllocation...> Swapped(static_cast<tMultiArrayAllocation&&>(fArray));
		fArray=static_cast<tMultiArrayAllocation&&>(Src);
		return Swapped;
	}
#endif	// cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L


	template<uIntn ArrayIndex>
	typename cnVar::TSelect<ArrayIndex,typename VTArrayAllocation::tElement...>::Type* GetChannelItems(void)const noexcept(true)
	{
		return cnVar::VarPackAt<ArrayIndex>(fArray.Pointer);
	}
	template<uIntn ArrayIndex>
	typename cnVar::TSelect<ArrayIndex,typename VTArrayAllocation::tElement...>::Type& GetChannelItemAt(uIntn Index)const noexcept(true)
	{
		return cnVar::VarPackAt<ArrayIndex>(fArray.Pointer)[Index];
	}

	tIterator GetItems(uIntn Index)const noexcept(true){
		tIterator Iterator;
		Iterator.Pointer=fArray.Pointer;
		return Iterator;
	}

	tIterator GetItemAt(uIntn Index)const noexcept(true){
		tIterator Iterator;
		Iterator.Pointer=fArray.Pointer;
		cnVar::VarPackForEach([Index](auto *&Ptr){
			Ptr+=Index;
		},Iterator.Pointer);

		return Iterator;
	}

	// GetCapacity
	// return current capcacity
    uIntn GetCapacity(void)const noexcept(true){
		return fArray.Capacity;
	}
	// SetCapacity
	//	change capacity
	// NewCapacity		new number of item can hold of the struct
	void SetCapacity(uIntn NewCapacity)noexcept(true){
		NewCapacity=TMAMFunc::BufferSizeRoundup(NewCapacity);
		return fArray.SetCapacity(NewCapacity);
	}

    uIntn GetCount(void)const noexcept(true){		return fArray.Length;	}
	void SetCount(uIntn NewLength)noexcept(true){	return fArray.SetLength(NewLength);	}

	// Clear
	//	remove all items
	void Clear(void)noexcept(true){
		fArray.SetLength(0);
	}

	// Compact
	void Compact(void)noexcept(true){
		return fArray.SetCapacity(fArray.Length);
	}



	tIterator at(uIntn Index)const noexcept(true){
		return GetItemAt(Index);
	}
	tIterator begin(void)const noexcept(true){	return {fArray.Pointer};	}
	tIterator end(void)const noexcept(true){	return GetItemAt(fArray.Length);	}

	cSeqMultiListItemRef<typename VTArrayAllocation::tElement...> operator [](uIntn Index)noexcept(true){
		cSeqMultiListItemRef<typename VTArrayAllocation::tElement...> RetRef;
		cnVar::VarPackForEach([Index](auto *&RetPtr,auto *&SrcPtr){
			RetPtr=SrcPtr+Index;
		},RetRef.Pointer,fArray.Pointer);
		return RetRef;
	}

	// ReplaceAt
	//	Replace ReplaceCount items at the specifed position with ReplacementCount items
	// Index				index to replace
	// ReplaceCount			count of items to remove
	// ReplacementCount		count of items to insert
	// return iterator of item where replaced
	tIterator ReplaceAt(uIntn Index,uIntn ReplaceCount,uIntn ReplacementCount)noexcept(true){
		if(Index>fArray.Length)
			Index=fArray.Length;
		fArray.ReplaceWithUninitialized(Index,ReplaceCount,ReplacementCount);
		// initialize key data and value
		if(ReplacementCount!=0){
			TMAMFunc::Construct(fArray.Pointer,Index,Index+ReplacementCount);
		}

		return at(Index);
	}
	// Replace
	//	Replace ReplaceCount items at the specifed position with ReplacementCount items
	// Iterator				iterator of the position to replace
	// ReplaceCount			count of items to remove
	// ReplacementCount		count of items to insert
	// return iterator of item where replaced
	tIterator Replace(const tIterator &Iterator,uIntn ReplaceCount,uIntn ReplacementCount)noexcept(true){
		uIntn Index=Iterator-fArray.Pointer;
		return ReplaceAt(Index,ReplaceCount,ReplacementCount);
	}

	// ReplaceAt
	//	Replace ReplaceCount items at the specifed position with ReplacementCount items
	// Index				index to replace
	// ReplaceCount			count of items to remove
	// [in]ReplacementItems	item to insert
	// ReplacementCount		count of items to insert
	// return iterator of item where replaced
	tIterator ReplaceAt(uIntn Index,uIntn ReplaceCount,tConstPointer ReplacementItems,uIntn ReplacementCount)noexcept(true){
		if(ReplacementItems==nullptr){
			return ReplaceAt(Index,ReplaceCount,ReplacementCount);
		}
		if(Index>fArray.Length)
			Index=fArray.Length;
		fArray.ReplaceWithUninitialized(Index,ReplaceCount,ReplacementCount);
		if(ReplacementCount!=0){
			TMAMFunc::CopyConstruct(fArray.Pointer,Index,ReplacementItems,0,ReplacementCount);
		}
		return at(Index);
	}

	// Replace
	//	Replace ReplaceCount items at the specifed position with ReplacementCount items
	// Iterator					iterator of the position to replace
	// ReplaceCount				count of items to remove
	// [in]ReplacementItems		item to insert
	// ReplacementCount			count of items to insert
	// return iterator of item where replaced
	tIterator Replace(const tIterator &Iterator,uIntn ReplaceCount,tConstPointer ReplacementItems,uIntn ReplacementCount)noexcept(true){
		uIntn Index=Iterator-fArray.Pointer;
		return ReplaceAt(Index,ReplaceCount,ReplacementItems,ReplacementCount);
	}

	// Append
	//	Add data items to the end of list
	// Count		number of data in array to add
	// return iterator of item where appended
	tIterator Append(uIntn Count=1)noexcept(true){
		if(Count==0){
			return end();
		}
		auto AppendIndex=AppendUninitialized(Count);
		// default construct
		TMAMFunc::Construct(fArray.Pointer,AppendIndex,fArray.Length);
		return at(AppendIndex);
	}

	tIterator Append(tConstPointer Items,uIntn Count)noexcept(true){
		if(Count==0){
			return end();
		}
		auto AppendIndex=AppendUninitialized(Count);
		// copy construct
		TMAMFunc::CopyConstruct(fArray.Pointer,AppendIndex,Items,0,Count);
		return at(AppendIndex);
	}

	// AppendMake
	//	Add a data item to the end of list
	// Args			data construct parameters
	// return iterator of item where appended
	template<class...VT>
	tPointer AppendMake(VT cnLib_UREF...Args)noexcept(true){
		auto AppendIndex=AppendUninitialized(1);
		// default construct
		TMAMFunc::ConstructElement(fArray.Pointer,AppendIndex,cnLib_UREFCAST(VT)(Args)...);
		return at(AppendIndex);
	}

	// InsertAt
	//	Add data items to the specifed position of the list, default construct
	// Index		index to insert
	// Count		number of data in array to insert
	// return iterator of newly inserted item
	tIterator InsertAt(uIntn Index,uIntn Count=1)noexcept(true){
		if(Count==0)
			return at(Index);
		if(Index>fArray.Length)
			Index=fArray.Length;
		fArray.ReplaceWithUninitialized(Index,0,Count);
		// default construct
		TMAMFunc::Construct(fArray.Pointer,Index,Index+Count);
		return at(Index);
	}
	// Insert
	//	Add data items to the specifed position of the list, default construct
	// Index		index to insert
	// Count		number of data in array to insert
	// return iterator of newly inserted item
	tIterator Insert(const tIterator &Iterator,uIntn Count)noexcept(true){
		uIntn Index=Iterator-fArray.Pointer;
		return InsertAt(Index,Count);
	}

	// InsertAt
	//	Add data items to the specifed position of the list, default construct
	// Index		index to insert
	// Count		number of data in array to insert
	// return true if added, false if out of range
	tIterator InsertAt(uIntn Index,tConstPointer Items,uIntn Count)noexcept(true){
		if(Index>fArray.Length)
			Index=fArray.Length;
		fArray.ReplaceWithUninitialized(Index,0,Count);
		// manually copy construct
		TMAMFunc::CopyConstruct(fArray.Pointer,Index,Items,0,Count);
		return at(Index);
	}

	// Insert
	//	Add data items to the specifed position of the list, default construct
	// Iterator		iterator of item to insert before
	// Count		number of data in array to insert
	// return iterator of newly added item.
	tPointer Insert(const tIterator &Iterator,tConstPointer Items,uIntn Count)noexcept(true){
		uIntn Index=Iterator-fArray.Pointer;
		return InsertAt(Index,Items,Count);
	}

	// InsertMakeAt
	//	Add a data item to the specifed position of the list
	// Index		index to insert
	// Args			data construct parameters
	// return iterator of newly inserted item
	template<class...VT>
	tIterator InsertMakeAt(uIntn Index,VT cnLib_UREF...Args)noexcept(true){
		if(Index>fArray.Length)
			Index=fArray.Length;
		fArray.ReplaceWithUninitialized(Index,0,1);
		// default construct
		TMAMFunc::ConstructElement(fArray.Pointer,Index,cnLib_UREFCAST(VT)(Args)...);
		return at(Index);
	}
	// InsertMake
	//	Add a data item to the specifed position of the list
	// Iterator		iterator of the position to insert
	// Args			data construct parameters
	// return iterator of newly inserted item
	template<class...VT>
	tIterator InsertMake(const tIterator &Iterator,VT cnLib_UREF...Args)noexcept(true){
		uIntn Index=Iterator-fArray.Pointer;
		return InsertMakeAt(Index,cnLib_UREFCAST(VT)(Args)...);
	}


	// RemoveTail
	//	Remove items from the end of list
	// Count		number of item to remove
	void RemoveTail(uIntn RemoveCount=1)noexcept(true){
		return fArray.Truncate(RemoveCount);
	}

	// RemoveAt
	//	Remove items at specifed position
	// Index			index of item to remove
	// RemoveCount		number of item to remove
	void RemoveAt(uIntn Index,uIntn RemoveCount=1)noexcept(true){
		return fArray.Remove(Index,RemoveCount);
	}

	// Remove
	//	Remove items at specifed position
	// Iterator			iterator of item to remove
	// RemoveCount		number of item to remove
	void Remove(tPointer Iterator,uIntn RemoveCount=1)noexcept(true){
		uIntn Index=Iterator-fArray.Pointer;
		return RemoveAt(Index,RemoveCount);
	}

protected:

	cnDataStruct::cMultiArrayStorage<VTArrayAllocation...> fArray;

	static tIterator NullIterator(void)noexcept(true){
		tIterator NullIterator;
		cnVar::VarPackForEach([](auto *&Ptr){	Ptr=nullptr;	},NullIterator.Pointer);
		return NullIterator;
	}

	uIntn AppendUninitialized(uIntn Count=1)noexcept(true){
		uIntn AppendIndex=fArray.Length;
		uIntn NewLength=AppendIndex+Count;
		if(NewLength>fArray.Capacity){
			fArray.GrowCapacityTo(NewLength);
		}
		fArray.Length=NewLength;
		return AppendIndex;
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
