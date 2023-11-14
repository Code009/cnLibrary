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
	virtual iUIView* GetView(void)const=0;
	virtual tTypeID GetImplementationType(void)const=0;
};
template<class TControl>
class cControlViewReference : public iViewReference, public TControl
{
public:
	using TControl::TControl;

	virtual iUIView* GetView(void)const override{
		return TControl::GetView();
	}
	
	virtual tTypeID GetImplementationType(void)const override
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
	TClass* operator ->(){
		return this;
	}
	const TClass* operator ->()const{
		return this;
	}

};
//---------------------------------------------------------------------------
template<class TClass,class TInterface>
class bcControlInterfaceWrapper<TClass&,TInterface> : public TInterface
{
public:
	bcControlInterfaceWrapper(TClass &Ref):Control(Ref){}
protected:
	TClass &Control;
	TClass* operator ->(){
		return cnMemory::AddressOf(Control);
	}
	const TClass* operator ->()const{
		return cnMemory::AddressOf(Control);
	}
};
//---------------------------------------------------------------------------

// View Components

//---------------------------------------------------------------------------
class cnLib_INTERFACE iControlComponent : public iReference
{
public:
	virtual void SetView(iUIView *View)=0;
	virtual void SetContentZPosition(Float32 ZPosition);
};
//---------------------------------------------------------------------------
template<class TClass,class TInterface=iControlComponent>
class bwControlComponent : public bcControlInterfaceWrapper<TClass,TInterface>
{
public:
	using bcControlInterfaceWrapper<TClass,TInterface>::bcControlInterfaceWrapper;

	virtual void SetView(iUIView *View)override{
		return (*this)->SetView(View);
	}
	virtual void SetContentZPosition(Float32 ZPosition)override{
		return (*this)->SetContentZPosition(ZPosition);
	}

};
//---------------------------------------------------------------------------
class cnLib_INTERFACE kiControl : public iControlComponent
{
public:
	virtual void SetLayoutOrder(sfInt16 Order);
};
//---------------------------------------------------------------------------
template<class TClass,class TInterface=kiControl>
class bwkControl : public bwControlComponent<TClass,TInterface>
{
public:
	using bwControlComponent<TClass,TInterface>::bwControlComponent;

	virtual void SetLayoutOrder(sfInt16 Order)override{
		return (*this)->SetLayoutOrder(Order);
	}
};
//---------------------------------------------------------------------------

// Visual Components

//---------------------------------------------------------------------------
class cnLib_INTERFACE viControl : public iControlComponent
{
public:
	virtual void SetContentVisible(bool Visible);
};
//---------------------------------------------------------------------------
template<class TClass,class TInterface=viControl>
class bwvControl : public bwControlComponent<TClass,TInterface>
{
public:
	using bwControlComponent<TClass,TInterface>::bwControlComponent;

	virtual void SetContentVisible(bool Visible)override{
		return (*this)->SetContentVisible(Visible);
	}

};
//---------------------------------------------------------------------------
class viData
{
public:
	viData();
	~viData();


protected:

	void InvalidateData(void);
private:
	template<class T> friend class dPtr;

	class bcNotifyToken
	{
	public:
		virtual void NotifyInvalidate(viData *Token)=0;

		bcNotifyToken *Parent;
		bcNotifyToken *Child[2];
		ufInt8 Branch;
		ufInt8 Color;
	};

	void NotifyInvalidate(void);
	void NotifyRegister(bcNotifyToken *NotifyToken);
	void NotifyUnregister(bcNotifyToken *NotifyToken);

	cnRTL::cLinkItemSet<bcNotifyToken,cnDataStruct::cItemAddressOrderOperator<bcNotifyToken> > fRefSet;
	bool fInvalidated;
	bool fInvalidating;
};
//---------------------------------------------------------------------------
template<class T>
class dPtr : public viData::bcNotifyToken
{
public:
	dPtr():fPointer(nullptr){}
	~dPtr(){
		if(fPointer!=nullptr)
			fPointer->NotifyUnregister(this);
	}

	operator T* ()const{	return fPointer;	}
	T* operator -> ()const{	return fPointer;	}
	T* Pointer(void)const{	return fPointer;	}

	dPtr(T *Ptr):fPointer(Ptr){
		if(fPointer!=nullptr)
			fPointer->NotifyRegister(this);
	}
	dPtr& operator =(T *Ptr){
		if(fPointer!=nullptr)
			fPointer->NotifyUnregister(this);
		fPointer=Ptr;
		if(fPointer!=nullptr)
			fPointer->NotifyRegister(this);
		return *this;
	}

	dPtr(const dPtr &Src)
		: fPointer(Src.Pointer())
	{
		if(fPointer!=nullptr)
			fPointer->NotifyRegister(this);
	}
protected:
	T *fPointer;

	virtual void NotifyInvalidate(viData*)override{
		fPointer=nullptr;
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


