#include "ObjectReference.h"

using namespace cnLibrary;
using namespace cnRTL;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void bcObservedReference::RefReset(void)
{
	fRefCount=1;
	fRegisterCount=0;
}
//---------------------------------------------------------------------------
void bcObservedReference::RefInc(void)
{
	cnLib_ASSERT(fRefCount!=cnVar::TIntegerValue<uIntn>::Max);
	cnLib_ASSERT(fRefCount!=0);
	fRefCount.Free++;
}
//---------------------------------------------------------------------------
bool bcObservedReference::RefDec(void)
{
	cnLib_ASSERT(fRefCount!=0);
	return fRefCount.Free--==1;
}
//---------------------------------------------------------------------------
void bcObservedReference::RefInvalidate(iObservedReference *ObservedReference)
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
		if(fRegisterCount.WatchUntilEqual(0,16384)==false){
			cnSystem::CurrentThread::SwitchThread();
		}
	}
}
//---------------------------------------------------------------------------
void bcObservedReference::WeakRegister(iReferenceInvalidationNotify *NotifyToken)
{
	return WeakRegister(reinterpret_cast<bcNotifyToken*>(NotifyToken));
}
void bcObservedReference::WeakUnregister(iReferenceInvalidationNotify *NotifyToken)
{
	return WeakUnregister(reinterpret_cast<bcNotifyToken*>(NotifyToken));
}
//---------------------------------------------------------------------------
void bcObservedReference::WeakRegister(bcNotifyToken *NotifyToken)
{
	auto AutoLock=TakeLock(&fRefSetLock);

	if(fRefSet.Insert(NotifyToken)){
		fRegisterCount.Free++;
	}
}
//---------------------------------------------------------------------------
void bcObservedReference::WeakUnregister(bcNotifyToken *NotifyToken)
{
	auto AutoLock=TakeLock(&fRefSetLock);
	
	if(fRefSet.Remove(NotifyToken)){
		fRegisterCount.Free--;
	}
}
//---------------------------------------------------------------------------
bool bcObservedReference::WeakToStrong(void)
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
cClassReferenceWithWeakSet::cClassReferenceWithWeakSet()
{
}
//---------------------------------------------------------------------------
cClassReferenceWithWeakSet::~cClassReferenceWithWeakSet()
{
}
//---------------------------------------------------------------------------
void cClassReferenceWithWeakSet::RefInc(void)
{
	cnLib_ASSERT(fRefCount!=0);
	fRefCount.Free++;
}
//---------------------------------------------------------------------------
bool cClassReferenceWithWeakSet::RefDec(void)
{
	cnLib_ASSERT(fRefCount!=0);
	if(fRefCount.Free--==1){
		RefInvalidate();
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------
void cClassReferenceWithWeakSet::RefReset(void)
{
	fRefCount=1;
	fRegisterCount=0;
}
//---------------------------------------------------------------------------
void cClassReferenceWithWeakSet::RefInvalidate(void)
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
		if(fRegisterCount.WatchUntilEqual(0,16384)==false){
			cnSystem::CurrentThread::SwitchThread();
		}
	}
}
//---------------------------------------------------------------------------
void cClassReferenceWithWeakSet::WeakRegister(bcNotifyToken *NotifyToken)
{
	auto AutoLock=TakeLock(&fRefSetLock);

	if(fRefSet.Insert(NotifyToken)){
		fRegisterCount.Free++;
	}
}
//---------------------------------------------------------------------------
void cClassReferenceWithWeakSet::WeakUnregister(bcNotifyToken *NotifyToken)
{
	auto AutoLock=TakeLock(&fRefSetLock);
	
	if(fRefSet.Remove(NotifyToken)){
		fRegisterCount.Free--;
	}
}
//---------------------------------------------------------------------------
bool cClassReferenceWithWeakSet::WeakToStrong(void)
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
