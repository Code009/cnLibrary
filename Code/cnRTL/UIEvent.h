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
	cOrderedObjectSet(){}
	~cOrderedObjectSet(){}

	uIntn GetCount(void){
		return fPtrSet.GetCount();
	}

	const TObject &GetAt(uIntn Index){
		auto pPair=fPtrOrderMap.GetPairAt(Index);
		return pPair->Key;
	}

	template<class TValue>
	bool Insert(TValue&& Object,TOrder Order=0){
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
	bool Remove(TValue&& Object)
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
		cIterator(){}
		cIterator(cOrderedObjectSet *Set,uIntn Index):fSet(Set),fIndex(Index){}
		void operator ++(){			fIndex++;		}
		void operator ++(int){		fIndex++;		}
		void operator --(){			fIndex--;		}
		void operator --(int){		fIndex--;		}

		bool operator !=(const cIterator &Src)const{
			return fIndex!=Src.fIndex;
		}

		const TObject &operator *()const{	return fSet->fPtrSet.GetItemAt(fIndex)->Object;	}
	};
	cIterator begin(void){
		return cIterator{this,0};
	}
	cIterator end(void){
		return cIterator{this,fPtrSet.GetCount()};
	}
protected:
	struct cOrderItem
	{
		TObject Object;
		TOrder Order;
	
		template<class TValue>
		bool operator ==(TValue&& Value)const{	return Object==Value;	}
		template<class TValue>
		bool operator !=(TValue&& Value)const{	return Object!=Value;	}

		bool operator ==(const cOrderItem &Src)const{	return Order==Src.Order && Object==Src.Object;	}
		bool operator !=(const cOrderItem &Src)const{	return Order!=Src.Order || Object!=Src.Object;	}
		bool operator <(const cOrderItem &Src)const{	return Order!=Src.Order	? Order<Src.Order : Object<Src.Object;	}
		bool operator <=(const cOrderItem &Src)const{	return Order!=Src.Order	? Order<Src.Order : Object<=Src.Object;	}
		bool operator >(const cOrderItem &Src)const{	return Order!=Src.Order	? Order>Src.Order : Object>Src.Object;	}
		bool operator >=(const cOrderItem &Src)const{	return Order!=Src.Order	? Order>Src.Order : Object>=Src.Object;	}
	};
	cSeqSet<cOrderItem> fPtrSet;
	cSeqMap<TObject,TOrder> fPtrOrderMap;
};
//---------------------------------------------------------------------------
template<class TObject,class TOrder>
class cUIOrderedObjectSet
{
public:
	cUIOrderedObjectSet(){}
	~cUIOrderedObjectSet(){}

	uIntn GetCount(void)const{	return fSet.GetCount();	}

	template<class TValue>
	bool Insert(TValue&& Object,TOrder Order){
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
	bool Remove(TValue&& Object)
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
		cEnumIndex(const cUIOrderedObjectSet *Owner):Owner(Owner){	Owner->fPendingEnumeIndex.Insert(this);	}
		cEnumIndex(const cEnumIndex &Src):Owner(Src.Owner),Index(Src.Index){	Owner->fPendingEnumeIndex.Insert(this);	}
		~cEnumIndex(){	Owner->fPendingEnumeIndex.Remove(this);	}

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
		cEnumerator(const cUIOrderedObjectSet *Owner):cEnumIndex(Owner){}
		cEnumerator(const cEnumerator &Src):cEnumIndex(Src){}
		bool Enum(void){
			this->Index=0;
			return this->Owner->fSet.GetCount()!=0;
		}
		bool Next(void){
			this->Index++;
			return this->Index<this->Owner->fSet.GetCount();
		}

		const TObject &Get()const{	return this->Owner->fSet.GetItemAt(this->Index)->Object;	}
	};
	cEnumerator Enumerator(void)const{	return this;	}
	class cReverseEnumerator : protected cEnumIndex
	{
	public:
		cReverseEnumerator(const cUIOrderedObjectSet *Owner):cEnumIndex(Owner){}
		cReverseEnumerator(const cEnumerator &Src):cEnumIndex(Src){}
		bool Enum(void){
			this->Index=this->Owner->fSet.GetCount();
			
			if(this->Index==0)
				return false;
			this->Index--;
			return true;
		}
		bool Next(void){
			if(this->Index==0)
				return false;
			this->Index--;
			return true;
		}

		const TObject &Get()const{	return this->Owner->fSet.GetItemAt(this->Index)->Object;	}
	};
	cReverseEnumerator ReverseEnumerator(void)const{	return this;	}
	class cIterator : protected cEnumIndex
	{
		friend cUIOrderedObjectSet; 
	public:
		cIterator(const cUIOrderedObjectSet *Owner,uIntn BeginIndex):cEnumIndex(Owner){	this->Index=BeginIndex;	}
		cIterator(const cIterator &Src):cEnumIndex(Src){}

		void operator ++(){		this->Index++;		}
		void operator ++(int){	this->Index++;		}

		bool operator ==(const cIterator &Src)const{	return this->Index==Src.Index;	}
		bool operator !=(const cIterator &Src)const{	return this->Index!=Src.Index;	}

		const TObject &operator *()const{	return this->Owner->fSet.GetItemAt(this->Index)->Object;	}
	};
	cIterator begin(void){
		return cIterator{this,0};
	}
	cIterator end(void){
		return cIterator{this,fSet.GetCount()};
	}
protected:
	cLinkMap<TObject,TOrder> fOrderMap;
	struct cSetItem
	{
		TObject Object;
		TOrder Order;
	
		typename cnVar::TDefaultCompareResult<TObject>::Type Compare(const cSetItem &Src)const{
			auto CompareResult=cnVar::DefaultCompare(Order,Src.Order);
			if(CompareResult!=0)
				return CompareResult;

			return cnVar::DefaultCompare(Object,Src.Object);
		}
		bool operator ==(const cSetItem &Src)const{	return Order==Src.Order && Object==Src.Object;	}
		bool operator !=(const cSetItem &Src)const{	return Order!=Src.Order || Object!=Src.Object;	}

		cnLib_DEFINE_CLASS_THREE_WAY_COMPARISON(const cSetItem &,Compare)
	};
	cSeqSet<cSetItem> fSet;

	void FixEnumeratorInsert(uIntn InsertedIndex){
		for(auto EnumIndex : fPendingEnumeIndex){
			if(EnumIndex->Index>=InsertedIndex){
				EnumIndex->Index++;
			}
		}
	}
	void FixEnumeratorRemove(uIntn RemovedIndex){
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
	cUIStateHandlerSetNotifier(const cUIOrderedObjectSet<iUIStateHandler*,sfInt16> &Set);
	void UIStarted(void);
	void UIShow(void);
	void UIResume(void);
	void UIPaused(void);
	void UIHide(void);
	void UIStopped(void);
private:
	const cUIOrderedObjectSet<iUIStateHandler*,sfInt16> &fSet;
};
//---------------------------------------------------------------------------
class cUIWindowHandlerSetNotifier
{
public:
	cUIWindowHandlerSetNotifier(const cUIOrderedObjectSet<iUIWindowHandler*,sfInt16> &Set);
	void ScreenChanged(void);
	void ParentChanged(void);
	void ContentScaleChanged(void);
	void RectangleChanged(bool Moved,bool Sized);
	void ZPositionChanged(void);
	void VisibleChanged(void);
	void EnableChanged(void);
private:
	const cUIOrderedObjectSet<iUIWindowHandler*,sfInt16> &fSet;
};
//---------------------------------------------------------------------------
class cUIViewHandlerSetNotifier
{
public:
	cUIViewHandlerSetNotifier(const cUIOrderedObjectSet<iUIViewHandler*,sfInt16> &Set);
	void UILayout(void);
	void UIMargin(cUIRectangle &Margin);
	void WindowChanged(void);
	void ParentChanged(void);
	void ContentScaleChanged(void);
	void RectangleChanged(bool Moved,bool Sized);
	void ZPositionChanged(void);
	void VisibleChanged(void);
	void EnableChanged(void);
private:
	const cUIOrderedObjectSet<iUIViewHandler*,sfInt16> &fSet;
};
//---------------------------------------------------------------------------
template<class T>
inline void UIStateNotifyChange(UIState PrevState,UIState NextState,T &Notifier)
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

	bool InsertKeyHandler(iUIKeyHandler *Handler,sfInt16 Order);
	bool RemoveKeyHandler(iUIKeyHandler *Handler);

	bool InsertKeyFilter(iUIKeyHandler *Filter,sfInt16 Order);
	bool RemoveKeyFilter(iUIKeyHandler *Filter);

	bool KeyAcquireExclusive(iUIKeyHandler *Handler);
	void KeyReleaseExclusive(iUIKeyHandler *Handler);

	bool InsertTextInputHandler(iUITextInputHandler *Handler,sfInt16 Order);
	bool RemoveTextInputHandler(iUITextInputHandler *Handler);


	void FocusEnterRange(iUIKeyEvent *KeyEvent);
	void FocusLeaveRange(iUIKeyEvent *KeyEvent);

	void HandleFocusEnter(iUIKeyEvent *KeyEvent);
	void HandleFocusLeave(iUIKeyEvent *KeyEvent);
	void HandleKeyDown(iUIKeyEvent *KeyEvent,eKeyCode KeyCode);
	void HandleKeyUp(iUIKeyEvent *KeyEvent,eKeyCode KeyCode);
	void HandleKeyInput(iUIKeyEvent *KeyEvent,eKeyCode KeyCode);

	void FilterFocusEnter(iUIKeyEvent *KeyEvent);
	void FilterFocusLeave(iUIKeyEvent *KeyEvent);
	void FilterKeyDown(iUIKeyEvent *KeyEvent,eKeyCode KeyCode);
	void FilterKeyUp(iUIKeyEvent *KeyEvent,eKeyCode KeyCode);
	void FilterKeyInput(iUIKeyEvent *KeyEvent,eKeyCode KeyCode);
protected:

	cUIOrderedObjectSet<iUIKeyHandler*,sfInt16> fKeyHandlers;
	cUIOrderedObjectSet<iUIKeyHandler*,sfInt16> fKeyFilters;
	cUIOrderedObjectSet<iUITextInputHandler*,sfInt16> fTextInputHandlers;

};
//---------------------------------------------------------------------------
class cUIMouseEventNotification
{
public:

	bool InsertMouseHandler(iUIMouseHandler *Handler,sfInt16 Order);
	bool RemoveMouseHandler(iUIMouseHandler *Handler);

	bool InsertMouseFilter(iUIMouseHandler *Filter,sfInt16 Order);
	bool RemoveMouseFilter(iUIMouseHandler *Filter);

	bool MouseAcquireExclusive(iUIMouseHandler *Handler);
	void MouseReleaseExclusive(iUIMouseHandler *Handler);


	void MouseEnterRange(iUIMouseEvent *MouseEvent);
	void MouseLeaveRange(iUIMouseEvent *MouseEvent);

	void HandleEnter(iUIMouseEvent *MouseEvent);
	void HandleLeave(iUIMouseEvent *MouseEvent);
	void HandleMove(iUIMouseEvent *MouseEvent);
	void HandleDown(iUIMouseEvent *MouseEvent,eMouseButton Button);
	void HandleUp(iUIMouseEvent *MouseEvent,eMouseButton Button);
	void HandleWheel(iUIMouseEvent *MouseEvent,Float32 ScrollX,Float32 ScrollY);

	void FilterEnter(iUIMouseEvent *MouseEvent);
	void FilterLeave(iUIMouseEvent *MouseEvent);
	void FilterMove(iUIMouseEvent *MouseEvent);
	void FilterDown(iUIMouseEvent *MouseEvent,eMouseButton Button);
	void FilterUp(iUIMouseEvent *MouseEvent,eMouseButton Button);
	void FilterWheel(iUIMouseEvent *MouseEvent,Float32 ScrollX,Float32 ScrollY);
protected:

	cUIOrderedObjectSet<iUIMouseHandler*,sfInt16> fMouseHandlers;
	cUIOrderedObjectSet<iUIMouseHandler*,sfInt16> fMouseFilters;

};
//---------------------------------------------------------------------------
class cUITouchEventNotification
{
public:

	// iUITouchControl

	bool InsertTouchHandler(iUITouchHandler *Handler,sfInt16 Order);
	bool RemoveTouchHandler(iUITouchHandler *Handler);

	bool InsertTouchFilter(iUITouchHandler *Filter,sfInt16 Order);
	bool RemoveTouchFilter(iUITouchHandler *Filter);

	bool TouchAcquireExclusive(iUITouchHandler *Handler);
	void TouchReleaseExclusive(iUITouchHandler *Handler);

	void HandleBegin(iUITouchEvent *TouchEvent);
	void HandleEnd(iUITouchEvent *TouchEvent);
	void HandleLost(iUITouchEvent *TouchEvent);
	void HandleMove(iUITouchEvent *TouchEvent);

	void FilterBegin(iUITouchEvent *TouchEvent);
	void FilterEnd(iUITouchEvent *TouchEvent);
	void FilterLost(iUITouchEvent *TouchEvent);
	void FilterMove(iUITouchEvent *TouchEvent);

protected:
	cUIOrderedObjectSet<iUITouchHandler*,sfInt16> fTouchHandlers;
	cUIOrderedObjectSet<iUITouchHandler*,sfInt16> fTouchFilters;
};
//---------------------------------------------------------------------------
class cUIViewEventNotification
{
public:
	// iUIView

	bool InsertStateHandler(iUIStateHandler *Handler,sfInt16 Order);
	bool RemoveStateHandler(iUIStateHandler *Handler);

	bool InsertViewHandler(iUIViewHandler *Handler,sfInt16 Order);
	bool RemoveViewHandler(iUIViewHandler *Handler);


	void StateChange(eUIState PrevState,eUIState NextState);

	void ContentScaleChanged(void);
	void RectangleChanged(bool Moved,bool Sized);
	void VisibleChanged(void);
	void EnableChanged(void);

	void Layout(void);
	cUIRectangle UIMargin(const cUIRectangle &Margin);
	void WindowChanged(void);
	void ParentChanged(void);
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
