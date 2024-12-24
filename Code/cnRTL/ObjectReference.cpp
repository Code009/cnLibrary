#include "ObjectReference.h"

using namespace cnLibrary;
using namespace cnRTL;

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
	, fRefCount(2)
	, fActiveFlag(cnVar::TIntegerValue<uIntn>::MSB+1)
	, fTokenDisposed(false)
	, fOwnerReleased(false)
{
}
//---------------------------------------------------------------------------
void bcWeakReference::cWeakObserver::Close(void)noexcept(true)
{
	Invalidated();
	Release();
}
//---------------------------------------------------------------------------
bool bcWeakReference::cWeakObserver::Reference(void)noexcept(true)
{
	uIntn Flag=++fActiveFlag.Free;
	if(Flag&cnVar::TIntegerValue<uIntn>::MSB){
		// token still active
		bool Referenced=fOwner->WeakToStrong();
		if(--fActiveFlag.Free==0){
			Invalidated();
		}
		return Referenced;
	}

	--fActiveFlag.Free;
	return false;
}
//---------------------------------------------------------------------------
void bcWeakReference::cWeakObserver::NotifyInsert(void)noexcept(true)
{
}
//---------------------------------------------------------------------------
void bcWeakReference::cWeakObserver::NotifyRemove(void)noexcept(true)
{
	Release();
}
//---------------------------------------------------------------------------
void bcWeakReference::cWeakObserver::NotifyExecute(void)noexcept(true)
{
	fOwnerReleased=true;
	if(fActiveFlag.Free.XchgSub(cnVar::TIntegerValue<uIntn>::MSB+1)==cnVar::TIntegerValue<uIntn>::MSB+1){
		Invalidated();
	}
}
//---------------------------------------------------------------------------
void bcWeakReference::cWeakObserver::Release(void)noexcept(true)
{
	if(--fRefCount.Free==0){
		delete this;
	}
}
//---------------------------------------------------------------------------
void bcWeakReference::cWeakObserver::Invalidated(void)noexcept(true)
{
	if(fTokenDisposed.Free.Xchg(true)==false){
		if(fOwnerReleased){
			if(fProcedure!=nullptr){
				fProcedure->Execute();
			}
		}
		fProcReference=nullptr;
		fOwner->WeakUnregister(this);
		fOwner=nullptr;
	}
}
//---------------------------------------------------------------------------
