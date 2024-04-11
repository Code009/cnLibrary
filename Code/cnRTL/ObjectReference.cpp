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
		Host->VirtualDelete();
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void bcObservedReference::RefReset(void)noexcept
{
	fRefCount=1;
	fRegisterCount=0;
}
//---------------------------------------------------------------------------
void bcObservedReference::RefInc(void)noexcept
{
	cnLib_ASSERT(fRefCount!=cnVar::TIntegerValue<uIntn>::Max);
	cnLib_ASSERT(fRefCount!=0);
	fRefCount.Free++;
}
//---------------------------------------------------------------------------
bool bcObservedReference::RefDec(void)noexcept
{
	cnLib_ASSERT(fRefCount!=0);
	return fRefCount.Free--==1;
}
//---------------------------------------------------------------------------
void bcObservedReference::RefInvalidate(iObservedReference *ObservedReference)noexcept
{
	{
		auto AutoLock=TakeLock(&fRefSetLock);
		auto RefIterator=fRefSet.begin();
		auto RefIteratorEnd=fRefSet.end();
		while(RefIterator!=RefIteratorEnd){
			bool Remove=RefIterator->InvalidationNotify(ObservedReference);
			auto DelIterator=RefIterator;
			RefIterator++;
			if(Remove){
				fRegisterCount.Free--;
				fRefSet.Remove(DelIterator);
			}
		}
	}
	// wait for all unregistered
	while(fRegisterCount.Acquire.Load()!=0){
		if(fRegisterCount.WatchEqual(0,16384)==false){
			cnSystem::CurrentThread::SwitchThread();
		}
	}
}
//---------------------------------------------------------------------------
void bcObservedReference::WeakRegister(iReferenceInvalidationNotify *NotifyToken)noexcept
{
	return WeakRegister(reinterpret_cast<bcNotifyToken*>(NotifyToken));
}
void bcObservedReference::WeakUnregister(iReferenceInvalidationNotify *NotifyToken)noexcept
{
	return WeakUnregister(reinterpret_cast<bcNotifyToken*>(NotifyToken));
}
//---------------------------------------------------------------------------
void bcObservedReference::WeakRegister(bcNotifyToken *NotifyToken)noexcept
{
	auto AutoLock=TakeLock(&fRefSetLock);

	if(fRefSet.Insert(NotifyToken)){
		fRegisterCount.Free++;
	}
}
//---------------------------------------------------------------------------
void bcObservedReference::WeakUnregister(bcNotifyToken *NotifyToken)noexcept
{
	auto AutoLock=TakeLock(&fRefSetLock);
	
	if(fRefSet.Remove(NotifyToken)){
		fRegisterCount.Free--;
	}
}
//---------------------------------------------------------------------------
bool bcObservedReference::WeakToStrong(void)noexcept
{
	auto AutoLock=TakeLock(&fRefSetLock);

	uIntn CurRefCount=fRefCount;
	while(CurRefCount!=0){
		if(fRefCount.Barrier.CmpXchg(CurRefCount,CurRefCount+1)){
			return true;
		}
	}
	return false;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cClassReferenceWithWeakSet::cClassReferenceWithWeakSet()noexcept
{
}
//---------------------------------------------------------------------------
cClassReferenceWithWeakSet::~cClassReferenceWithWeakSet()noexcept
{
}
//---------------------------------------------------------------------------
void cClassReferenceWithWeakSet::RefInc(void)noexcept
{
	cnLib_ASSERT(fRefCount!=0);
	fRefCount.Free++;
}
//---------------------------------------------------------------------------
bool cClassReferenceWithWeakSet::RefDec(void)noexcept
{
	cnLib_ASSERT(fRefCount!=0);
	if(fRefCount.Free--==1){
		RefInvalidate();
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------
void cClassReferenceWithWeakSet::RefReset(void)noexcept
{
	fRefCount=1;
	fRegisterCount=0;
}
//---------------------------------------------------------------------------
void cClassReferenceWithWeakSet::RefInvalidate(void)noexcept
{
	{
		auto AutoLock=TakeLock(&fRefSetLock);
		auto RefIterator=fRefSet.begin();
		auto RefIteratorEnd=fRefSet.end();
		while(RefIterator!=RefIteratorEnd){
			bool Remove=RefIterator->InvalidationNotify(this);
			auto DelIterator=RefIterator;
			RefIterator++;
			if(Remove){
				fRegisterCount.Free--;
				fRefSet.Remove(DelIterator);
			}
		}
	}
	// wait for all unregistered
	while(fRegisterCount.Acquire.Load()!=0){
		if(fRegisterCount.WatchEqual(0,16384)==false){
			cnSystem::CurrentThread::SwitchThread();
		}
	}
}
//---------------------------------------------------------------------------
void cClassReferenceWithWeakSet::WeakRegister(bcNotifyToken *NotifyToken)noexcept
{
	auto AutoLock=TakeLock(&fRefSetLock);

	if(fRefSet.Insert(NotifyToken)){
		fRegisterCount.Free++;
	}
}
//---------------------------------------------------------------------------
void cClassReferenceWithWeakSet::WeakUnregister(bcNotifyToken *NotifyToken)noexcept
{
	auto AutoLock=TakeLock(&fRefSetLock);
	
	if(fRefSet.Remove(NotifyToken)){
		fRegisterCount.Free--;
	}
}
//---------------------------------------------------------------------------
bool cClassReferenceWithWeakSet::WeakToStrong(void)noexcept
{
	auto AutoLock=TakeLock(&fRefSetLock);

	uIntn CurRefCount=fRefCount;
	while(CurRefCount!=0){
		if(fRefCount.Barrier.CmpXchg(CurRefCount,CurRefCount+1)){
			return true;
		}
	}
	return false;
}
//---------------------------------------------------------------------------
