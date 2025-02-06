#include "ObjectReference.h"
#include "String.h"

using namespace cnLibrary;
using namespace cnRTL;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void bcWeakReference::ObjectResetReference(void)noexcept(true)
{
	fRefCount=1;
	fWeakCount=1;
}
//---------------------------------------------------------------------------
void bcWeakReference::ObjectIncreaseReference(void)noexcept(true)
{
	++fRefCount.Free;
}
//---------------------------------------------------------------------------
void bcWeakReference::ObjectDecreaseReference(void)noexcept(true)
{
	if(--fRefCount.Free==0){

		fNotifySet.Notify();

		if(--fWeakCount.Free==0){
			ObjectDeleted();
		};
	}
}
//---------------------------------------------------------------------------
iReferenceObserver* bcWeakReference::CreateReferenceObserver(iReference *Reference,iProcedure *Procedure)noexcept(true)
{
	++fWeakCount.Free;

	auto Observer=new cWeakObserver(this,Reference,Procedure);
	fNotifySet.Insert(Observer);
	return Observer;
}
//---------------------------------------------------------------------------
void bcWeakReference::WeakUnregister(cWeakObserver *Observer)noexcept(true)
{
	fNotifySet.Remove(Observer);

	if(--fWeakCount.Free==0){
		ObjectDeleted();
	}
}
//---------------------------------------------------------------------------
bool bcWeakReference::WeakToStrong(void)noexcept(true)
{
	uIntn CurRefCount=fRefCount;
	while(CurRefCount!=0){
		if(fRefCount.Barrier.CmpXchg(CurRefCount,CurRefCount+1)){
			return true;
		}
	}
	return false;
}
//---------------------------------------------------------------------------
bcWeakReference::cWeakObserver::cWeakObserver(bcWeakReference *Owner,iReference *Reference,iProcedure *Procedure)noexcept(true)
	: fOwner(Owner)
	, fProcReference(Reference)
	, fProcedure(Procedure)
{
}
//---------------------------------------------------------------------------
bool bcWeakReference::cWeakObserver::ObserverMakeStrongReference(void)noexcept(true)
{
	return fOwner->WeakToStrong();
}
//---------------------------------------------------------------------------
void bcWeakReference::cWeakObserver::ObserverDelete(void)noexcept(true)
{
	delete this;
}
//---------------------------------------------------------------------------
void bcWeakReference::cWeakObserver::ObserverInvalidate(bool Notify)noexcept(true)
{
	if(Notify){
		if(fProcedure!=nullptr){
			fProcedure->Execute();
		}
	}
	fProcReference=nullptr;
	fOwner->WeakUnregister(this);
	fOwner=nullptr;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcRegisteredReference::bcRegisteredReference(bool InitialReference)noexcept(true)
	: fRefCount(InitialReference)
{
}
//---------------------------------------------------------------------------
void bcRegisteredReference::ResetRef(bool InitialReference)noexcept(true)
{
	fRefCount=InitialReference;
}
//---------------------------------------------------------------------------
void bcRegisteredReference::IncRef(void)noexcept(true)
{
	++fRefCount.Barrier;
}
//---------------------------------------------------------------------------
void bcRegisteredReference::DecRef(void)noexcept(true)
{
	if(--fRefCount.Barrier==0){
		fShutdownNotifySet.Notify();
		ReferenceUpdate();
	}
}
//---------------------------------------------------------------------------
bool bcRegisteredReference::MakeStrongReference(void)noexcept(true)
{
	uIntn RefCount=fRefCount;
	while(RefCount){
		if(fRefCount.Barrier.CmpXchg(RefCount,RefCount+1)){
			return true;
		}
	}
	return false;
}
//---------------------------------------------------------------------------
rPtr<iLibraryReference> bcRegisteredReference::CreateReference(iLibraryReferrer *Referrer)noexcept(true)
{
	auto Reference=new cReference(this,Referrer);
	fReferenceList.InsertTail(Reference);
	return rPtr<iLibraryReference>::TakeFromManual(Reference);
}
//---------------------------------------------------------------------------
void bcRegisteredReference::Update(void)noexcept(true)
{
	for(auto p=fReferenceList.begin();p!=fReferenceList.end();){
		auto RemoveItem=*p;
		p++;
		if(RemoveItem->Released){
			fReferenceList.Remove(RemoveItem);
			delete RemoveItem;
		}
		else if(RemoveItem->DescriptionUpdated){
			// update description
		}
	}
}
//---------------------------------------------------------------------------
bool bcRegisteredReference::CheckShutdown(void)noexcept(true)
{
	return fReferenceList.IsEmpty() && fRefCount.Acquire.Load()==0;
}
//---------------------------------------------------------------------------
void bcRegisteredReference::ReportReferenceSet(cStringBuffer<uChar16> &ReportText)noexcept(true)
{
	if(fReferenceList.GetCount()!=0){
		ReportText.Append(u"Remaining reference :\n");
		for(auto Reference : fReferenceList){
			auto Referrer=Reference->GetReferrer();
			cArray<const uChar16> DescArray;
			rPtr<iStringReference> Description=(Referrer!=nullptr ? Referrer->CreateDescription() : nullptr);
			if(Description!=nullptr){
				DescArray=Description->Get();
			}
			else{
				DescArray.Pointer=u"Unknown Object";
				DescArray.Length=14;
			}
			StringStream::WriteFormatString(ReportText.StreamWriteBuffer(),u"%.*s\n",DescArray.Length,DescArray.Pointer);
		}
		ReportText.Append(u"End of Referrer List\n");
	}
	else{
		ReportText.Append(u"No remaining referrer\n");
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcRegisteredReference::cReference::cReference(bcRegisteredReference *Owner,iLibraryReferrer *Referrer)noexcept(true)
	: fOwner(Owner)
	, fReferrer(Referrer)
	, fWeakRefCount(1)
	, Released(false)
	, DescriptionUpdated(false)
{
}
//---------------------------------------------------------------------------
bcRegisteredReference::cReference::~cReference()noexcept(true)
{
}
//---------------------------------------------------------------------------
void bcRegisteredReference::cReference::WeakIncRef(void)noexcept(true)
{
	++fWeakRefCount.Free;
}
//---------------------------------------------------------------------------
void bcRegisteredReference::cReference::WeakDecRef(void)noexcept(true)
{
	if(--fWeakRefCount.Free==0){
		Released=true;
		fOwner->ReferenceUpdate();
	}
}
//---------------------------------------------------------------------------
bool bcRegisteredReference::cReference::MakeStrongReference(void)noexcept(true)
{
	if(fOwner->MakeStrongReference()==false)
		return false;

	WeakIncRef();
	return true;
}
//---------------------------------------------------------------------------
bcRegisteredReference *bcRegisteredReference::cReference::GetOwner(void)const noexcept(true)
{
	return fOwner;
}
//---------------------------------------------------------------------------
iLibraryReferrer *bcRegisteredReference::cReference::GetReferrer(void)const noexcept(true)
{
	return fReferrer;
}
//---------------------------------------------------------------------------
void bcRegisteredReference::cReference::IncreaseReference(void)noexcept(true)
{
	WeakIncRef();

	fOwner->IncRef();
}
//---------------------------------------------------------------------------
void bcRegisteredReference::cReference::DecreaseReference(void)noexcept(true)
{
	fOwner->DecRef();

	WeakDecRef();
}
//---------------------------------------------------------------------------
iReferenceObserver* bcRegisteredReference::cReference::CreateReferenceObserver(iReference *NotifyReference,iProcedure *NotifyProcedure)noexcept(true)
{
	auto Observer=new cObserver(this,NotifyReference,NotifyProcedure);
	fOwner->fShutdownNotifySet.Insert(Observer);
	return Observer;
}
//---------------------------------------------------------------------------
void bcRegisteredReference::cReference::UpdateDescription(void)noexcept(true)
{
	DescriptionUpdated=true;
	fOwner->ReferenceUpdate();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcRegisteredReference::cObserver::cObserver(cReference *Reference,iReference *NotifyReference,iProcedure *NotifyProcedure)noexcept(true)
	: fReference(Reference)
	, fNotifyReference(NotifyReference)
	, fNotifyProcedure(NotifyProcedure)
{
	fReference->WeakIncRef();
}
//---------------------------------------------------------------------------
bcRegisteredReference::cObserver::~cObserver()noexcept(true)
{
	fReference->WeakDecRef();
}
//---------------------------------------------------------------------------
bool bcRegisteredReference::cObserver::ObserverMakeStrongReference(void)noexcept(true)
{
	return fReference->MakeStrongReference();
}
//---------------------------------------------------------------------------
void bcRegisteredReference::cObserver::ObserverInvalidate(bool Notify)noexcept(true)
{
	if(Notify){
		if(fNotifyProcedure!=nullptr){
			fNotifyProcedure->Execute();
		}
	}
	fNotifyReference=nullptr;
	auto Owner=fReference->GetOwner();
	Owner->fShutdownNotifySet.Remove(this);
}
//---------------------------------------------------------------------------
void bcRegisteredReference::cObserver::ObserverDelete(void)noexcept(true)
{
	delete this;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void cDualReference::cInner::Inc(void)noexcept(true)
{
	Ref.Free++;
}
void cDualReference::cInner::Dec(void)noexcept(true)
{
	if(Ref.Free--==1){
		auto Host=cnMemory::GetObjectFromMemberPointer(this,&cDualReference::fInnerReference);
		Host->Dispose();
	}
}
//---------------------------------------------------------------------------
