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
bcModuleReference::bcModuleReference()noexcept(true)
	: fRefCount(0)
{
}
//---------------------------------------------------------------------------
void bcModuleReference::ModuleResetReference(void)noexcept(true)
{
	fRefCount=0;
}
//---------------------------------------------------------------------------
void bcModuleReference::ModuleIncreaseReference(void)noexcept(true)
{
	++fRefCount.Barrier;
}
//---------------------------------------------------------------------------
void bcModuleReference::ModuleDecreaseReference(void)noexcept(true)
{
	if(--fRefCount.Barrier==0){
		fShutdownNotifySet.Notify();
		ReferenceUpdate();
	}
}
//---------------------------------------------------------------------------
bool bcModuleReference::MakeStrongReference(void)noexcept(true)
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
iModuleHandle* bcModuleReference::CreateModuleHandle(iModuleReferrer *Referrer)noexcept(true)
{
	auto Handle=new cModuleHandle(this,Referrer);
	fReferrerList.InsertTail(Handle);
	return Handle;
}
//---------------------------------------------------------------------------
iReferenceObserver* bcModuleReference::CreateReferenceObserver(iModuleReferrer *Referrer,iReference *NotifyReference,iProcedure *NotifyProcedure)noexcept(true)
{
	auto Observer=new cObserver(this,Referrer,NotifyReference,NotifyProcedure);
	fReferrerList.InsertTail(Observer);
	fShutdownNotifySet.Insert(Observer);
	return Observer;
}
//---------------------------------------------------------------------------
void bcModuleReference::Update(void)noexcept(true)
{
	for(auto p=fReferrerList.begin();p!=fReferrerList.end();){
		auto RemoveItem=*p;
		p++;
		if(RemoveItem->fReleased){
			if(RemoveItem->fShutdownCompletionNotify!=nullptr){
				fShutdownCompletionNotifyList.AppendMake(RemoveItem->fShutdownCompletionNotify);
			}
			fReferrerList.Remove(RemoveItem);
			RemoveItem->ReferrerClosed();
		}
		else if(RemoveItem->fDescriptionUpdated){
			// update description
		}
	}
}
//---------------------------------------------------------------------------
bool bcModuleReference::CheckShutdown(void)noexcept(true)
{
	return fReferrerList.IsEmpty() && fRefCount.Acquire.Load()==0;
}
//---------------------------------------------------------------------------
cSeqList<iProcedure*>&& bcModuleReference::FetchShutdownCompletionNotifyList(void)noexcept(true)
{
	return cnVar::MoveCast(fShutdownCompletionNotifyList);
}
//---------------------------------------------------------------------------
void bcModuleReference::ReportReferenceSet(cStringBuffer<uChar16> &ReportText)noexcept(true)
{
	if(fReferrerList.GetCount()!=0){
		ReportText.Append(u"Remaining referrer :\n");
		for(auto Reference : fReferrerList){
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
bcModuleReference::bcReferrerItem::bcReferrerItem(bcModuleReference *Owner,iModuleReferrer *Referrer)noexcept(true)
	: fOwner(Owner)
	, fReferrer(Referrer)
	, fReleased(false)
	, fDescriptionUpdated(false)
{
}
//---------------------------------------------------------------------------
bcModuleReference::bcReferrerItem::~bcReferrerItem()noexcept(true)
{
}
//---------------------------------------------------------------------------
bcModuleReference *bcModuleReference::bcReferrerItem::GetOwner(void)const noexcept(true)
{
	return fOwner;
}
//---------------------------------------------------------------------------
iModuleReferrer *bcModuleReference::bcReferrerItem::GetReferrer(void)const noexcept(true)
{
	return fReferrer;
}
//---------------------------------------------------------------------------
void bcModuleReference::bcReferrerItem::Close(iProcedure *ShutdownCompletionNotify)noexcept(true)
{
	if(fReleased)
		return;
	fReleased=true;
	fShutdownCompletionNotify=ShutdownCompletionNotify;
	fOwner->ReferenceUpdate();
}
//---------------------------------------------------------------------------
void bcModuleReference::bcReferrerItem::UpdateDescription(void)noexcept(true)
{
	fDescriptionUpdated=true;
	fOwner->ReferenceUpdate();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcModuleReference::cModuleHandle::cModuleHandle(bcModuleReference *Owner,iModuleReferrer *Referrer)noexcept(true)
	: bcReferrerItem(Owner,Referrer)
{
	fOwner->ModuleIncreaseReference();
}
//---------------------------------------------------------------------------
bcModuleReference::cModuleHandle::~cModuleHandle()noexcept(true)
{
	fOwner->ModuleDecreaseReference();
}
//---------------------------------------------------------------------------
void bcModuleReference::cModuleHandle::Close(void)noexcept(true)
{
	bcReferrerItem::Close(nullptr);
}
//---------------------------------------------------------------------------
void bcModuleReference::cModuleHandle::cCloseWaitProcedure::Execute(void)noexcept(true)
{
	Notification.Notify();
}
//---------------------------------------------------------------------------
bool bcModuleReference::cModuleHandle::CloseAndWaitUnload(iProcedure *NotifyProcedure)noexcept(true)
{
	if(NotifyProcedure!=nullptr){
		bcReferrerItem::Close(NotifyProcedure);
		return true;
	}

	cCloseWaitProcedure WaitProcedure;
	bcReferrerItem::Close(&WaitProcedure);
	WaitProcedure.Notification.Wait();
	return false;
}
//---------------------------------------------------------------------------
void bcModuleReference::cModuleHandle::UpdateDescription(void)noexcept(true)
{
	return bcReferrerItem::UpdateDescription();
}
//---------------------------------------------------------------------------
void bcModuleReference::cModuleHandle::ReferrerClosed(void)noexcept(true)
{
	delete this;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcModuleReference::cObserver::cObserver(bcModuleReference *Owner,iModuleReferrer *Referrer,iReference *NotifyReference,iProcedure *NotifyProcedure)noexcept(true)
	: bcReferrerItem(Owner,Referrer)
	, fNotifyReference(NotifyReference)
	, fNotifyProcedure(NotifyProcedure)
{
}
//---------------------------------------------------------------------------
bcModuleReference::cObserver::~cObserver()noexcept(true)
{
}
//---------------------------------------------------------------------------
bool bcModuleReference::cObserver::ObserverMakeStrongReference(void)noexcept(true)
{
	return fOwner->MakeStrongReference();
}
//---------------------------------------------------------------------------
void bcModuleReference::cObserver::ObserverInvalidate(bool Notify)noexcept(true)
{
	if(Notify){
		if(fNotifyProcedure!=nullptr){
			fNotifyProcedure->Execute();
		}
	}
	fNotifyReference=nullptr;
	fOwner->fShutdownNotifySet.Remove(this);
}
//---------------------------------------------------------------------------
void bcModuleReference::cObserver::ObserverDelete(void)noexcept(true)
{
	bcReferrerItem::Close(nullptr);
}
//---------------------------------------------------------------------------
void bcModuleReference::cObserver::ReferrerClosed(void)noexcept(true)
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
