/*- cnUI - UI Interface Declaration ---------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2009-05-11                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnUI_UIInterface_H__
#define __cnLibrary_cnUI_UIInterface_H__

#include <cnUI/UICommon.h>

#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnUI{
//---------------------------------------------------------------------------

// View Reference

//---------------------------------------------------------------------------
class cnLib_INTERFACE iViewReference : public iReference
{
public:
	virtual iUIView* GetView(void)const noexcept(true)=0;
	virtual tTypeID GetImplementationType(void)const noexcept(true)=0;
};
template<class TControl>
class cControlViewReference : public iViewReference, public TControl
{
public:
	using TControl::TControl;

	virtual iUIView* GetView(void)const noexcept(true)override{
		return TControl::GetView();
	}
	
	virtual tTypeID GetImplementationType(void)const noexcept(true)override
	{
		return cnVar::TTypeID<cControlViewReference>::Value;
	}
};
//---------------------------------------------------------------------------
template<class TClass,class TInterface>
class bcControlInterfaceWrapper : public TClass,public TInterface
{
public:
	using TClass::TClass;
protected:
	TClass* operator ->()noexcept(true){
		return this;
	}
	const TClass* operator ->()const noexcept(true){
		return this;
	}

};
//---------------------------------------------------------------------------
template<class TClass,class TInterface>
class bcControlInterfaceWrapper<TClass&,TInterface> : public TInterface
{
public:
	bcControlInterfaceWrapper(TClass &Ref)noexcept(true):Control(Ref){}
protected:
	TClass &Control;
	TClass* operator ->()noexcept(true){
		return cnMemory::AddressOf(Control);
	}
	const TClass* operator ->()const noexcept(true){
		return cnMemory::AddressOf(Control);
	}
};
//---------------------------------------------------------------------------

// View Components

//---------------------------------------------------------------------------
class cnLib_INTERFACE iControlComponent : public iReference
{
public:
	virtual void SetView(iUIView *View)noexcept(true)=0;
	virtual void SetContentZPosition(Float32 ZPosition)noexcept(true);
};
//---------------------------------------------------------------------------
template<class TClass,class TInterface=iControlComponent>
class bwControlComponent : public bcControlInterfaceWrapper<TClass,TInterface>
{
public:
	using bcControlInterfaceWrapper<TClass,TInterface>::bcControlInterfaceWrapper;

	virtual void SetView(iUIView *View)noexcept(true)override{
		return (*this)->SetView(View);
	}
	virtual void SetContentZPosition(Float32 ZPosition)noexcept(true)override{
		return (*this)->SetContentZPosition(ZPosition);
	}

};
//---------------------------------------------------------------------------
class cnLib_INTERFACE kiControl : public iControlComponent
{
public:
	virtual void SetLayoutOrder(sfInt16 Order)noexcept(true);
};
//---------------------------------------------------------------------------
template<class TClass,class TInterface=kiControl>
class bwkControl : public bwControlComponent<TClass,TInterface>
{
public:
	using bwControlComponent<TClass,TInterface>::bwControlComponent;

	virtual void SetLayoutOrder(sfInt16 Order)noexcept(true)override{
		return (*this)->SetLayoutOrder(Order);
	}
};
//---------------------------------------------------------------------------

// Visual Components

//---------------------------------------------------------------------------
class cnLib_INTERFACE viControl : public iControlComponent
{
public:
	virtual void SetContentVisible(bool Visible)noexcept(true);
};
//---------------------------------------------------------------------------
template<class TClass,class TInterface=viControl>
class bwvControl : public bwControlComponent<TClass,TInterface>
{
public:
	using bwControlComponent<TClass,TInterface>::bwControlComponent;

	virtual void SetContentVisible(bool Visible)noexcept(true)override{
		return (*this)->SetContentVisible(Visible);
	}

};
//---------------------------------------------------------------------------
class viData
{
public:
	viData()noexcept(true);
	~viData()noexcept(true);


protected:

	void InvalidateData(void)noexcept(true);
private:
	template<class T> friend class dPtr;

	class bcNotifyToken
	{
	public:
		virtual void NotifyInvalidate(viData *Token)noexcept(true)=0;

		bcNotifyToken *Parent;
		bcNotifyToken *Child[2];
		ufInt8 Branch;
		ufInt8 Color;
	};

	void NotifyInvalidate(void)noexcept(true);
	void NotifyRegister(bcNotifyToken *NotifyToken)noexcept(true);
	void NotifyUnregister(bcNotifyToken *NotifyToken)noexcept(true);

	cnRTL::cLinkItemSet<bcNotifyToken,cnDataStruct::cItemAddressOrderOperator<bcNotifyToken> > fRefSet;
	bool fInvalidated;
	bool fInvalidating;
};
//---------------------------------------------------------------------------
template<class T>
class dPtr : public viData::bcNotifyToken
{
public:
	dPtr()noexcept(true):fPointer(nullptr){}
	~dPtr()noexcept(true){
		if(fPointer!=nullptr)
			fPointer->NotifyUnregister(this);
	}

	operator T* ()const noexcept(true){	return fPointer;	}
	T* operator -> ()const noexcept(true){	return fPointer;	}
	T* Pointer(void)const noexcept(true){	return fPointer;	}

	dPtr(T *Ptr)noexcept(true):fPointer(Ptr){
		if(fPointer!=nullptr)
			fPointer->NotifyRegister(this);
	}
	dPtr& operator =(T *Ptr)noexcept(true){
		if(fPointer!=nullptr)
			fPointer->NotifyUnregister(this);
		fPointer=Ptr;
		if(fPointer!=nullptr)
			fPointer->NotifyRegister(this);
		return *this;
	}

	dPtr(const dPtr &Src)noexcept(true)
		: fPointer(Src.Pointer())
	{
		if(fPointer!=nullptr)
			fPointer->NotifyRegister(this);
	}
protected:
	T *fPointer;

	virtual void NotifyInvalidate(viData*)noexcept(true)override{
		fPointer=nullptr;
	}
};
//---------------------------------------------------------------------------
template<class TVisualData>
class iVisualData : public iReference
{
public:
	virtual iUIThread *GetUIThread(void)noexcept(true)=0;

	virtual void InsertUpdateNotify(iProcedure *Procedure)noexcept(true)=0;
	virtual void RemoveUpdateNotify(iProcedure *Procedure)noexcept(true)=0;
	virtual const TVisualData* QueryData(rPtr<iReference> &DataReference)noexcept(true)=0;
};
//---------------------------------------------------------------------------
template<class TVisualData>
class cUIVisualStaticData : public iVisualData<TVisualData>
{
public:
	cUIVisualStaticData(iUIThread *UIThread)noexcept(true)
		: fUIThread(UIThread)
	{
	}

	virtual iUIThread *GetUIThread(void)noexcept(true)override{
		return fUIThread;
	}

	virtual void InsertUpdateNotify(iProcedure *Procedure)noexcept(true)override{
		if(fUIThread->IsCurrentThread()==false)
			return;
		fNotifySet.Insert(Procedure);
	}
	virtual void RemoveUpdateNotify(iProcedure *Procedure)noexcept(true)override{
		if(fUIThread->IsCurrentThread()==false)
			return;
		fNotifySet.Remove(Procedure);
	}
	virtual const TVisualData* QueryData(rPtr<iReference> &DataReference)noexcept(true)override{
		if(fUIThread->IsCurrentThread()==false){
			return nullptr;
		}
		DataReference=fDataReference;
		return fVisualData;
	}

	void UpdateData(iReference *DataReference,const TVisualData *VisualData)noexcept(true){
		if(fUIThread->IsCurrentThread()==false)
			return;
		fVisualData=VisualData;
		fDataReference=DataReference;
		auto Dispatch=fUIThread->GetDispatch(false);
		
		for(auto Procedure : fNotifySet){
			Procedure->Execute();
		}
	}
private:
	cnRTL::cSeqSet<iProcedure*> fNotifySet;
	rPtr<iReference> fDataReference;
	const TVisualData *fVisualData;
	iPtr<iUIThread> fUIThread;
};
//---------------------------------------------------------------------------
template<class TVisualData>
class cUIVisualDataCache : public iVisualData<TVisualData>
{
public:
	class iDataProvider
	{
	public:
		virtual const TVisualData* QueryData(rPtr<iReference> &DataReference)noexcept(true)=0;

	};

	cUIVisualDataCache(iUIThread *UIThread,iDataProvider *DataProvider)noexcept(true)
		: fDataProvider(DataProvider)
		, fUIThread(UIThread)
	{
	}

	virtual iUIThread *GetUIThread(void)noexcept(true)override{
		return fUIThread;
	}

	virtual void InsertUpdateNotify(iProcedure *Procedure)noexcept(true)override{
		if(fUIThread->IsCurrentThread()==false)
			return;
		fNotifySet.Insert(Procedure);
	}
	virtual void RemoveUpdateNotify(iProcedure *Procedure)noexcept(true)override{
		if(fUIThread->IsCurrentThread()==false)
			return;
		fNotifySet.Remove(Procedure);
	}
	virtual const TVisualData* QueryData(rPtr<iReference> &DataReference)noexcept(true)override{
		if(fUIThread->IsCurrentThread()==false)
			return nullptr;
		if(fDataReference==nullptr){
			if(fDataProvider==nullptr){
				return nullptr;
			}
			fVisualData=fDataProvider->QueryData(fDataReference);
			if(fVisualData==nullptr){
				return nullptr;
			}
		}
		DataReference=fDataReference;
		return fVisualData;
	}


	void ProviderClosed(void)noexcept(true){
		fDataProvider=nullptr;
	}

	void NotifyUpdate(void)noexcept(true){
		if(fDataReference!=nullptr){
			fDataReference=nullptr;
			NotifyCallbacks();
		}
	}

private:
	iDataProvider *fDataProvider;
	iPtr<iUIThread> fUIThread;
	cnRTL::cSeqSet<iProcedure*> fNotifySet;
	const TVisualData *fVisualData;
	rPtr<iReference> fDataReference;

	void NotifyCallbacks(void){
		for(auto *Procedure : fNotifySet){
			Procedure->Execute();
		}
	}
};
//---------------------------------------------------------------------------
}//	namespace cnUI
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif


