/*- cnUI - UI Event -------------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2019-07-27                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnRTL_UIEvent_H__
#define __cnLibrary_cnRTL_UIEvent_H__
/*-------------------------------------------------------------------------*/
#include <cnRTL/cnRTLHeader.h>

#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnRTL{
//---------------------------------------------------------------------------
template<class TObject,class TOrder>
class cOrderedObjectSet
{
public:
	cOrderedObjectSet()noexcept(true){}
	~cOrderedObjectSet()noexcept(true){}

	uIntn GetCount(void)noexcept(true){
		return fPtrSet.GetCount();
	}

	const TObject &GetAt(uIntn Index)noexcept(true){
		auto pPair=fPtrOrderMap.GetPairAt(Index);
		return pPair->Key;
	}

	template<class TValue>
	bool Insert(TValue&& Object,TOrder Order=0)noexcept(true){
		bool Exists;
		auto Pair=fPtrOrderMap.InsertPair(Object,Exists);
		cOrderItem OrderItem;
		if(Exists){
			if(Pair->Value==Order){
				// already exists
				return false;
			}
			// reorder
			OrderItem.Object=Object;

			// remove previous order
			OrderItem.Order=Pair->Value;
			fPtrSet.Remove(OrderItem);
		}
		else{
			OrderItem.Object=Object;
		}
		// insert with new order
		Pair->Value=Order;
		OrderItem.Order=Order;
		fPtrSet.Insert(OrderItem);
		return true;
	}

	template<class TValue>
	bool Remove(TValue&& Object)noexcept(true)
	{
		auto Pair=fPtrOrderMap.GetPair(Object);
		if(Pair==nullptr)
			return false;

		cOrderItem OrderItem;
		OrderItem.Object=Object;
		OrderItem.Order=Pair->Value;
		fPtrSet.Remove(OrderItem);

		return true;
	}

	class cIterator
	{
	protected:
		cOrderedObjectSet *fSet;
		uIntn fIndex;
	public:
		cIterator()noexcept(true){}
		cIterator(cOrderedObjectSet *Set,uIntn Index)noexcept(true):fSet(Set),fIndex(Index){}
		void operator ++()noexcept(true){			fIndex++;		}
		void operator ++(int)noexcept(true){		fIndex++;		}
		void operator --()noexcept(true){			fIndex--;		}
		void operator --(int)noexcept(true){		fIndex--;		}

		bool operator !=(const cIterator &Src)const noexcept(true){
			return fIndex!=Src.fIndex;
		}

		const TObject &operator *()const noexcept(true){	return fSet->fPtrSet.GetItemAt(fIndex)->Object;	}
	};
	cIterator begin(void)noexcept(true){
		return cIterator{this,0};
	}
	cIterator end(void)noexcept(true){
		return cIterator{this,fPtrSet.GetCount()};
	}
protected:
	struct cOrderItem
	{
		TObject Object;
		TOrder Order;
	
		template<class TValue>
		bool operator ==(TValue&& Value)const noexcept(true){	return Object==Value;	}
		template<class TValue>
		bool operator !=(TValue&& Value)const noexcept(true){	return Object!=Value;	}

		bool operator ==(const cOrderItem &Src)const noexcept(true){	return Order==Src.Order && Object==Src.Object;	}
		bool operator !=(const cOrderItem &Src)const noexcept(true){	return Order!=Src.Order || Object!=Src.Object;	}
		bool operator <(const cOrderItem &Src)const noexcept(true){	return Order!=Src.Order	? Order<Src.Order : Object<Src.Object;	}
		bool operator <=(const cOrderItem &Src)const noexcept(true){	return Order!=Src.Order	? Order<Src.Order : Object<=Src.Object;	}
		bool operator >(const cOrderItem &Src)const noexcept(true){	return Order!=Src.Order	? Order>Src.Order : Object>Src.Object;	}
		bool operator >=(const cOrderItem &Src)const noexcept(true){	return Order!=Src.Order	? Order>Src.Order : Object>=Src.Object;	}
	};
	cSeqSet<cOrderItem> fPtrSet;
	cSeqMap<TObject,TOrder> fPtrOrderMap;
};
//---------------------------------------------------------------------------
template<class TObject,class TOrder>
class cUIOrderedObjectSet
{
public:
	cUIOrderedObjectSet()noexcept(true){}
	~cUIOrderedObjectSet()noexcept(true){}

	uIntn GetCount(void)const noexcept(true){	return fSet.GetCount();	}

	template<class TValue>
	bool Insert(TValue&& Object,TOrder Order)noexcept(true){
		bool Exists;
		auto Pair=fOrderMap.InsertPair(Object,Exists);
		cSetItem OrderItem;
		if(Exists){
			if(Pair->Value==Order){
				// already exists
				return false;
			}
			// reorder
			OrderItem.Object=Object;

			// remove previous order
			OrderItem.Order=Pair->Value;
			uIntn RemoveIndex=fSet.IndexOf(OrderItem);
			if(RemoveIndex!=IndexNotFound){
				fSet.RemoveAt(RemoveIndex);
				FixEnumeratorRemove(RemoveIndex);
			}
		}
		else{
			OrderItem.Object=Object;
		}
		// insert with new order
		Pair->Value=Order;
		OrderItem.Order=Order;
		auto Inserted=fSet.Insert(Exists,OrderItem);
		FixEnumeratorInsert(fSet.IndexOf(Inserted));
		return !Exists;
	}

	template<class TValue>
	bool Remove(TValue&& Object)noexcept(true)
	{
		auto Pair=fOrderMap.GetPair(Object);
		if(Pair==fOrderMap.end())
			return false;

		cSetItem OrderItem;
		OrderItem.Object=Object;
		OrderItem.Order=Pair->Value;
		uIntn RemoveIndex=fSet.IndexOf(OrderItem);
		if(RemoveIndex!=IndexNotFound){
			fSet.RemoveAt(RemoveIndex);
			FixEnumeratorRemove(RemoveIndex);
		}

		return true;
	}

	struct cEnumIndex
	{
		cEnumIndex(const cUIOrderedObjectSet *Owner)noexcept(true):Owner(Owner){	Owner->fPendingEnumeIndex.Insert(this);	}
		cEnumIndex(const cEnumIndex &Src)noexcept(true):Owner(Src.Owner),Index(Src.Index){	Owner->fPendingEnumeIndex.Insert(this);	}
		~cEnumIndex()noexcept(true){	Owner->fPendingEnumeIndex.Remove(this);	}

		cEnumIndex *Parent;
		cEnumIndex *Child[2];
		ufInt8 Branch;
		ufInt8 Color;
		const cUIOrderedObjectSet *Owner;
		uIntn Index;
	};
	class cEnumerator : protected cEnumIndex
	{
	public:
		cEnumerator(const cUIOrderedObjectSet *Owner)noexcept(true):cEnumIndex(Owner){}
		cEnumerator(const cEnumerator &Src)noexcept(true):cEnumIndex(Src){}
		bool Enum(void)noexcept(true){
			this->Index=0;
			return this->Owner->fSet.GetCount()!=0;
		}
		bool Next(void)noexcept(true){
			this->Index++;
			return this->Index<this->Owner->fSet.GetCount();
		}

		const TObject &Get()const noexcept(true){	return this->Owner->fSet.GetItemAt(this->Index)->Object;	}
	};
	cEnumerator Enumerator(void)const noexcept(true){	return this;	}
	class cReverseEnumerator : protected cEnumIndex
	{
	public:
		cReverseEnumerator(const cUIOrderedObjectSet *Owner)noexcept(true):cEnumIndex(Owner){}
		cReverseEnumerator(const cEnumerator &Src)noexcept(true):cEnumIndex(Src){}
		bool Enum(void)noexcept(true){
			this->Index=this->Owner->fSet.GetCount();
			
			if(this->Index==0)
				return false;
			this->Index--;
			return true;
		}
		bool Next(void)noexcept(true){
			if(this->Index==0)
				return false;
			this->Index--;
			return true;
		}

		const TObject &Get()const noexcept(true){	return this->Owner->fSet.GetItemAt(this->Index)->Object;	}
	};
	cReverseEnumerator ReverseEnumerator(void)const noexcept(true){	return this;	}
	class cIterator : protected cEnumIndex
	{
		friend cUIOrderedObjectSet; 
	public:
		cIterator(const cUIOrderedObjectSet *Owner,uIntn BeginIndex)noexcept(true):cEnumIndex(Owner){	this->Index=BeginIndex;	}
		cIterator(const cIterator &Src)noexcept(true):cEnumIndex(Src){}

		void operator ++()noexcept(true){		this->Index++;		}
		void operator ++(int)noexcept(true){	this->Index++;		}

		bool operator ==(const cIterator &Src)const noexcept(true){	return this->Index==Src.Index;	}
		bool operator !=(const cIterator &Src)const noexcept(true){	return this->Index!=Src.Index;	}

		const TObject &operator *()const noexcept(true){	return this->Owner->fSet.GetItemAt(this->Index)->Object;	}
	};
	cIterator begin(void)noexcept(true){
		return cIterator{this,0};
	}
	cIterator end(void)noexcept(true){
		return cIterator{this,fSet.GetCount()};
	}
protected:
	cLinkMap<TObject,TOrder> fOrderMap;
	struct cSetItem
	{
		TObject Object;
		TOrder Order;
	
		typename cnVar::TDefaultCompareResult<TObject>::Type Compare(const cSetItem &Src)const noexcept(true){
			auto CompareResult=cnVar::DefaultCompare(Order,Src.Order);
			if(CompareResult!=0)
				return CompareResult;

			return cnVar::DefaultCompare(Object,Src.Object);
		}
		bool operator ==(const cSetItem &Src)const noexcept(true){	return Order==Src.Order && Object==Src.Object;	}
		bool operator !=(const cSetItem &Src)const noexcept(true){	return Order!=Src.Order || Object!=Src.Object;	}

		cnLib_DEFINE_CLASS_THREE_WAY_COMPARISON(const cSetItem &,Compare)
	};
	cSeqSet<cSetItem> fSet;

	void FixEnumeratorInsert(uIntn InsertedIndex)noexcept(true){
		for(auto EnumIndex : fPendingEnumeIndex){
			if(EnumIndex->Index>=InsertedIndex){
				EnumIndex->Index++;
			}
		}
	}
	void FixEnumeratorRemove(uIntn RemovedIndex)noexcept(true){
		for(auto EnumIndex : fPendingEnumeIndex){
			if(EnumIndex->Index>=RemovedIndex){
				EnumIndex->Index--;
			}
		}
	}
	mutable cLinkItemSet< cEnumIndex,cnDataStruct::cItemAddressOrderOperator<cEnumIndex> > fPendingEnumeIndex;
};
//---------------------------------------------------------------------------
class cUIStateHandlerSetNotifier
{
public:
	cUIStateHandlerSetNotifier(const cUIOrderedObjectSet<iUIStateHandler*,sfInt16> &Set)noexcept(true);
	void UIStarted(void)noexcept(true);
	void UIShow(void)noexcept(true);
	void UIResume(void)noexcept(true);
	void UIPaused(void)noexcept(true);
	void UIHide(void)noexcept(true);
	void UIStopped(void)noexcept(true);
private:
	const cUIOrderedObjectSet<iUIStateHandler*,sfInt16> &fSet;
};
//---------------------------------------------------------------------------
class cUIWindowHandlerSetNotifier
{
public:
	cUIWindowHandlerSetNotifier(const cUIOrderedObjectSet<iUIWindowHandler*,sfInt16> &Set)noexcept(true);
	void ScreenChanged(void)noexcept(true);
	void ParentChanged(void)noexcept(true);
	void ContentScaleChanged(void)noexcept(true);
	void RectangleChanged(bool Moved,bool Sized)noexcept(true);
	void ZPositionChanged(void)noexcept(true);
	void VisibleChanged(void)noexcept(true);
	void EnableChanged(void)noexcept(true);
private:
	const cUIOrderedObjectSet<iUIWindowHandler*,sfInt16> &fSet;
};
//---------------------------------------------------------------------------
class cUIViewHandlerSetNotifier
{
public:
	cUIViewHandlerSetNotifier(const cUIOrderedObjectSet<iUIViewHandler*,sfInt16> &Set)noexcept(true);
	void UILayout(void)noexcept(true);
	void UIMargin(cUIRectangle &Margin)noexcept(true);
	void WindowChanged(void)noexcept(true);
	void ParentChanged(void)noexcept(true);
	void ContentScaleChanged(void)noexcept(true);
	void RectangleChanged(bool Moved,bool Sized)noexcept(true);
	void ZPositionChanged(void)noexcept(true);
	void VisibleChanged(void)noexcept(true);
	void EnableChanged(void)noexcept(true);
private:
	const cUIOrderedObjectSet<iUIViewHandler*,sfInt16> &fSet;
};
//---------------------------------------------------------------------------
template<class T>
inline void UIStateNotifyChange(UIState PrevState,UIState NextState,T &Notifier)noexcept(true)
{
	// state action
	switch(NextState){
	case UIState::Null:
		switch(PrevState){
		case UIState::Active:
			// pause - hide - stop
			Notifier.UIPaused();
		case UIState::Inactive:
			// hide - stop
			Notifier.UIHide();
		case UIState::Background:
			// stop
			Notifier.UIStopped();
		default:	// Null
			break;
		}
		break;
	case UIState::Background:
		switch(PrevState){
		case UIState::Active:
			// pause - hide
			Notifier.UIPaused();
		case UIState::Inactive:
			// hide
			Notifier.UIHide();
		default:	// Background
			break;
		case UIState::Null:
			// start
			Notifier.UIStarted();
		}
		break;
	case UIState::Inactive:
		switch(PrevState){
		case UIState::Active:
			// pause
			Notifier.UIPaused();
		default:	// Inactive
			break;
		case UIState::Null:
			// start - show
			Notifier.UIStarted();
		case UIState::Background:
			// show
			Notifier.UIShow();
		}
		break;
	case UIState::Active:
		switch(PrevState){
		default:	// Active
			break;
		case UIState::Null:
			// start - show - resume
			Notifier.UIStarted();
		case UIState::Background:
			// show - resume
			Notifier.UIShow();
		case UIState::Inactive:
			// resume
			Notifier.UIResume();
		}
		break;
	default:
		break;
	}
}
//---------------------------------------------------------------------------
class cUIKeyEventNotification
{
public:

	bool InsertKeyHandler(iUIKeyHandler *Handler,sfInt16 Order)noexcept(true);
	bool RemoveKeyHandler(iUIKeyHandler *Handler)noexcept(true);

	bool InsertKeyFilter(iUIKeyHandler *Filter,sfInt16 Order)noexcept(true);
	bool RemoveKeyFilter(iUIKeyHandler *Filter)noexcept(true);

	bool KeyAcquireExclusive(iUIKeyHandler *Handler)noexcept(true);
	void KeyReleaseExclusive(iUIKeyHandler *Handler)noexcept(true);

	bool InsertTextInputHandler(iUITextInputHandler *Handler,sfInt16 Order)noexcept(true);
	bool RemoveTextInputHandler(iUITextInputHandler *Handler)noexcept(true);


	void FocusEnterRange(iUIKeyEvent *KeyEvent)noexcept(true);
	void FocusLeaveRange(iUIKeyEvent *KeyEvent)noexcept(true);

	void HandleFocusEnter(iUIKeyEvent *KeyEvent)noexcept(true);
	void HandleFocusLeave(iUIKeyEvent *KeyEvent)noexcept(true);
	void HandleKeyDown(iUIKeyEvent *KeyEvent,eKeyCode KeyCode)noexcept(true);
	void HandleKeyUp(iUIKeyEvent *KeyEvent,eKeyCode KeyCode)noexcept(true);
	void HandleKeyInput(iUIKeyEvent *KeyEvent,eKeyCode KeyCode)noexcept(true);

	void FilterFocusEnter(iUIKeyEvent *KeyEvent)noexcept(true);
	void FilterFocusLeave(iUIKeyEvent *KeyEvent)noexcept(true);
	void FilterKeyDown(iUIKeyEvent *KeyEvent,eKeyCode KeyCode)noexcept(true);
	void FilterKeyUp(iUIKeyEvent *KeyEvent,eKeyCode KeyCode)noexcept(true);
	void FilterKeyInput(iUIKeyEvent *KeyEvent,eKeyCode KeyCode)noexcept(true);
protected:

	cUIOrderedObjectSet<iUIKeyHandler*,sfInt16> fKeyHandlers;
	cUIOrderedObjectSet<iUIKeyHandler*,sfInt16> fKeyFilters;
	cUIOrderedObjectSet<iUITextInputHandler*,sfInt16> fTextInputHandlers;

};
//---------------------------------------------------------------------------
class cUIMouseEventNotification
{
public:

	bool InsertMouseHandler(iUIMouseHandler *Handler,sfInt16 Order)noexcept(true);
	bool RemoveMouseHandler(iUIMouseHandler *Handler)noexcept(true);

	bool InsertMouseFilter(iUIMouseHandler *Filter,sfInt16 Order)noexcept(true);
	bool RemoveMouseFilter(iUIMouseHandler *Filter)noexcept(true);

	bool MouseAcquireExclusive(iUIMouseHandler *Handler)noexcept(true);
	void MouseReleaseExclusive(iUIMouseHandler *Handler)noexcept(true);


	void MouseEnterRange(iUIMouseEvent *MouseEvent)noexcept(true);
	void MouseLeaveRange(iUIMouseEvent *MouseEvent)noexcept(true);

	void HandleEnter(iUIMouseEvent *MouseEvent)noexcept(true);
	void HandleLeave(iUIMouseEvent *MouseEvent)noexcept(true);
	void HandleMove(iUIMouseEvent *MouseEvent)noexcept(true);
	void HandleDown(iUIMouseEvent *MouseEvent,eMouseButton Button)noexcept(true);
	void HandleUp(iUIMouseEvent *MouseEvent,eMouseButton Button)noexcept(true);
	void HandleWheel(iUIMouseEvent *MouseEvent,Float32 ScrollX,Float32 ScrollY)noexcept(true);

	void FilterEnter(iUIMouseEvent *MouseEvent)noexcept(true);
	void FilterLeave(iUIMouseEvent *MouseEvent)noexcept(true);
	void FilterMove(iUIMouseEvent *MouseEvent)noexcept(true);
	void FilterDown(iUIMouseEvent *MouseEvent,eMouseButton Button)noexcept(true);
	void FilterUp(iUIMouseEvent *MouseEvent,eMouseButton Button)noexcept(true);
	void FilterWheel(iUIMouseEvent *MouseEvent,Float32 ScrollX,Float32 ScrollY)noexcept(true);
protected:

	cUIOrderedObjectSet<iUIMouseHandler*,sfInt16> fMouseHandlers;
	cUIOrderedObjectSet<iUIMouseHandler*,sfInt16> fMouseFilters;

};
//---------------------------------------------------------------------------
class cUITouchEventNotification
{
public:

	// iUITouchControl

	bool InsertTouchHandler(iUITouchHandler *Handler,sfInt16 Order)noexcept(true);
	bool RemoveTouchHandler(iUITouchHandler *Handler)noexcept(true);

	bool InsertTouchFilter(iUITouchHandler *Filter,sfInt16 Order)noexcept(true);
	bool RemoveTouchFilter(iUITouchHandler *Filter)noexcept(true);

	bool TouchAcquireExclusive(iUITouchHandler *Handler)noexcept(true);
	void TouchReleaseExclusive(iUITouchHandler *Handler)noexcept(true);

	void HandleBegin(iUITouchEvent *TouchEvent)noexcept(true);
	void HandleEnd(iUITouchEvent *TouchEvent)noexcept(true);
	void HandleLost(iUITouchEvent *TouchEvent)noexcept(true);
	void HandleMove(iUITouchEvent *TouchEvent)noexcept(true);

	void FilterBegin(iUITouchEvent *TouchEvent)noexcept(true);
	void FilterEnd(iUITouchEvent *TouchEvent)noexcept(true);
	void FilterLost(iUITouchEvent *TouchEvent)noexcept(true);
	void FilterMove(iUITouchEvent *TouchEvent)noexcept(true);

protected:
	cUIOrderedObjectSet<iUITouchHandler*,sfInt16> fTouchHandlers;
	cUIOrderedObjectSet<iUITouchHandler*,sfInt16> fTouchFilters;
};
//---------------------------------------------------------------------------
class cUIViewEventNotification
{
public:
	// iUIView

	bool InsertStateHandler(iUIStateHandler *Handler,sfInt16 Order)noexcept(true);
	bool RemoveStateHandler(iUIStateHandler *Handler)noexcept(true);

	bool InsertViewHandler(iUIViewHandler *Handler,sfInt16 Order)noexcept(true);
	bool RemoveViewHandler(iUIViewHandler *Handler)noexcept(true);


	void StateChange(eUIState PrevState,eUIState NextState)noexcept(true);

	void ContentScaleChanged(void)noexcept(true);
	void RectangleChanged(bool Moved,bool Sized)noexcept(true);
	void VisibleChanged(void)noexcept(true);
	void EnableChanged(void)noexcept(true);

	void Layout(void)noexcept(true);
	cUIRectangle UIMargin(const cUIRectangle &Margin)noexcept(true);
	void WindowChanged(void)noexcept(true);
	void ParentChanged(void)noexcept(true);
protected:

	// event handlers
	cUIOrderedObjectSet<iUIStateHandler*,sfInt16> fStateHandlers;
	cUIOrderedObjectSet<iUIViewHandler*,sfInt16> fViewHandlers;

private:
	
};
//---------------------------------------------------------------------------
#if 0
class bcOrderedPtrSet
{
public:
	bcOrderedPtrSet();
	~bcOrderedPtrSet();

	bool Insert(void *Object,sIntn Order=0);
	bool Remove(void *Object);
protected:
	struct cOrderItem
	{
		void *Pointer;
		sIntn Order;
	
		bool operator ==(tNullptr)const;
		bool operator !=(tNullptr)const;

		bool operator ==(const cOrderItem &Src)const;
		bool operator !=(const cOrderItem &Src)const;
		bool operator <(const cOrderItem &Src)const;
		bool operator <=(const cOrderItem &Src)const;
		bool operator >(const cOrderItem &Src)const;
		bool operator >=(const cOrderItem &Src)const;
	};
	cSeqSet<cOrderItem> fPtrSet;
	cSeqMap<void*,sIntn> fPtrOrderMap;
};
//---------------------------------------------------------------------------
template<class T>
class cOrderedPtrSet : protected bcOrderedPtrSet
{
public:
	bool Insert(T *Ptr,sIntn Order=0)	{	return bcOrderedPtrSet::Insert(Ptr,Order);	}
	bool Remove(T *Ptr)					{	return bcOrderedPtrSet::Remove(Ptr);	}
	class cIterator
	{
	protected:
		cOrderedPtrSet *fSet;
		uIntn fIndex;
	public:
		cIterator(){}
		cIterator(cOrderedPtrSet *Set,uIntn Index):fSet(Set),fIndex(Index){}
		void operator ++(){			fIndex++;		}
		void operator ++(int){		fIndex++;		}
		void operator --(){			fIndex--;		}
		void operator --(int){		fIndex--;		}

		bool operator !=(const cIterator &Src)const{
			return fIndex!=Src.fIndex;
		}

		T *operator *()const{	return static_cast<T*>(fSet->fPtrSet.GetList()[fIndex].Pointer);	}
	};
	cIterator begin(void){
		return cIterator{this,0};
	}
	cIterator end(void){
		return cIterator{this,fPtrSet.GetCount()};
	}
};
//---------------------------------------------------------------------------
class bcOrderedReferenceSet
{
public:
	bcOrderedReferenceSet();
	~bcOrderedReferenceSet();

	bool Exists(iReference *Object);
	bool Insert(iReference *Object,sIntn Order=0);
	bool Remove(iReference *Object);
protected:
	struct cOrderItem
	{
		void *Pointer;
		sIntn Order;
	
		bool operator ==(tNullptr)const;
		bool operator !=(tNullptr)const;

		bool operator ==(const cOrderItem &Src)const;
		bool operator !=(const cOrderItem &Src)const;
		bool operator <(const cOrderItem &Src)const;
		bool operator <=(const cOrderItem &Src)const;
		bool operator >(const cOrderItem &Src)const;
		bool operator >=(const cOrderItem &Src)const;
	};
	cSeqSet<cOrderItem> fPtrSet;
	cSeqMap<rPtr<iReference>,sIntn> fPtrOrderMap;
};
//---------------------------------------------------------------------------
template<class T>
class cOrderedReferenceSet : protected bcOrderedReferenceSet
{
public:
	bool Exists(T *Ptr)	{	return bcOrderedReferenceSet::Exists(Ptr);	}
	bool Insert(T *Ptr,sIntn Order=0)	{	return bcOrderedReferenceSet::Insert(Ptr,Order);	}
	bool Remove(T *Ptr)					{	return bcOrderedReferenceSet::Remove(Ptr);	}
	class cIterator
	{
	protected:
		cOrderedReferenceSet *fSet;
		uIntn fIndex;
	public:
		cIterator(){}
		cIterator(cOrderedReferenceSet *Set,uIntn Index):fSet(Set),fIndex(Index){}
		void operator ++(){			fIndex++;		}
		void operator ++(int){		fIndex++;		}
		void operator --(){			fIndex--;		}
		void operator --(int){		fIndex--;		}

		bool operator !=(const cIterator &Src)const{
			return fIndex!=Src.fIndex;
		}

		T *operator *()const{	return static_cast<T*>(fSet->fPtrSet.GetList()[fIndex].Pointer);	}
	};
	cIterator begin(void){
		return cIterator{this,0};
	}
	cIterator end(void){
		return cIterator{this,fPtrSet.GetCount()};
	}
};
#endif // 0
//---------------------------------------------------------------------------
}   // namespace cnRTL
//---------------------------------------------------------------------------
}   // namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif
