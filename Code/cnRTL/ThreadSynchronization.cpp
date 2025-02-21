//---------------------------------------------------------------------------
#include "ThreadSynchronization.h"
#include "RuntimeFunction.h"

using namespace cnLibrary;
using namespace cnRTL;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void cWaitObject::Acquire(void)noexcept(true)
{
	fRefCount.Free++;
}
//---------------------------------------------------------------------------
void cWaitObject::Release(void)noexcept(true)
{
	if(fRefCount.Free--==0){
		// reference count was 0, notification should be ready
		fNotification.Notify();
	}
}
//---------------------------------------------------------------------------
bool cWaitObject::Check(void)noexcept(true)
{
	return fRefCount==0;
}
//---------------------------------------------------------------------------
void cWaitObject::Wait(void)noexcept(true)
{
	if(fRefCount==0)
		return;

	fNotification.Setup();
	if(fRefCount.Free--!=0){
		fNotification.Wait();
	}
	fRefCount.Free++;
	fNotification.Clear();
}
//---------------------------------------------------------------------------
bool cWaitObject::WaitFor(uInt64 Duration)noexcept(true)
{
	if(fRefCount==0)
		return true;

	fNotification.Setup();
	if(fRefCount.Free--!=0){
		while(fNotification.WaitFor(Duration)==false){
			// timeout
			if(++fRefCount.Free!=0){
				// state reseted
				fNotification.Clear();
				return false;
			}
			// other thread must notifing, continue wait
			fRefCount.Free--;
		}
	}
	fRefCount.Free++;
	fNotification.Clear();
	return true;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWaitReference::cWaitReference()noexcept(true)
{
}
//---------------------------------------------------------------------------
cWaitReference::~cWaitReference()noexcept(true)
{
}
//---------------------------------------------------------------------------
void cWaitReference::IncreaseReference(void)noexcept(true)
{
	Acquire();
}
//---------------------------------------------------------------------------
void cWaitReference::DecreaseReference(void)noexcept(true)
{
	Release();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cLocalSingleThreadNotification::cLocalSingleThreadNotification()noexcept(true)
{
	fNotification.Setup();
}
//---------------------------------------------------------------------------
cLocalSingleThreadNotification::~cLocalSingleThreadNotification()noexcept(true)
{
	fNotification.Clear();
}
//---------------------------------------------------------------------------
void cLocalSingleThreadNotification::Reset(void)noexcept(true)
{
	fNotification.Clear();
	fNotification.Setup();
}
//---------------------------------------------------------------------------
void cLocalSingleThreadNotification::Wait(void)noexcept(true)
{
	return fNotification.Wait();
}
//---------------------------------------------------------------------------
bool cLocalSingleThreadNotification::WaitFor(ufInt64 Duration)noexcept(true)
{
	return fNotification.WaitFor(Duration);
}
//---------------------------------------------------------------------------
void cLocalSingleThreadNotification::Notify(void)noexcept(true)
{
	return fNotification.Notify();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cAffixedVariableSet::cAffixedVariableSet()noexcept(true)
	: fSetMutex(cnSystem::CreateMutexLock())
{
}
//---------------------------------------------------------------------------
cAffixedVariableSet::~cAffixedVariableSet()noexcept(true)
{
}
//---------------------------------------------------------------------------
rPtr<iVariable> cAffixedVariableSet::QueryAffixedVariable(const void *Token)noexcept(true)
{
	auto AutoLock=TakeLock(fSetMutex);

	auto pNode=fSet.Find(Token);
	if(pNode!=fSet.end()){
		if(pNode->MakeReference()){
			return rPtr<iVariable>::TakeFromManual(*pNode);
		}
	}
	auto NewItem=new cVariable(this,Token);
	fSet.Insert(NewItem);
	return rPtr<iVariable>::TakeFromManual(NewItem);
}
//---------------------------------------------------------------------------
void cAffixedVariableSet::VariableReleased(cVariable *Variable)noexcept(true)
{
	auto AutoLock=TakeLock(fSetMutex);
	fSet.Remove(Variable);
	delete Variable;
}
//---------------------------------------------------------------------------
cAffixedVariableSet::cVariable::cVariable(cAffixedVariableSet *Owner,const void *Token)noexcept(true)
	: fOwner(Owner)
	, fToken(Token)
	, fValue(nullptr)
	, fRefCount(1)
{
}
//---------------------------------------------------------------------------
cAffixedVariableSet::cVariable::~cVariable()noexcept(true)
{
}
//---------------------------------------------------------------------------
bool cAffixedVariableSet::cVariable::MakeReference(void)noexcept(true)
{
	if(fRefCount.Free++==0){
		// is releasing by others
		return false;
	}
	return true;
}
//---------------------------------------------------------------------------
void cAffixedVariableSet::cVariable::IncreaseReference(void)noexcept(true)
{
	++fRefCount.Free;
}
//---------------------------------------------------------------------------
void cAffixedVariableSet::cVariable::DecreaseReference(void)noexcept(true)
{
	if(--fRefCount.Free==0){
		fOwner->VariableReleased(this);
	}
}
//---------------------------------------------------------------------------
void cAffixedVariableSet::cVariable::Clear(void)noexcept(true)
{
	fReference=nullptr;
	fValue=nullptr;
}
//---------------------------------------------------------------------------
void* cAffixedVariableSet::cVariable::Get(void)noexcept(true)
{
	return fValue;
}
//---------------------------------------------------------------------------
void cAffixedVariableSet::cVariable::Set(iReference *Reference,void *Value)noexcept(true)
{
	fReference=Reference;
	fValue=Value;
}
//---------------------------------------------------------------------------
sfInt8 cAffixedVariableSet::cVariable::cItemOrderOperator::Compare(const cVariable &Item,const cVariable &Value)noexcept(true)
{
	return Compare(Item,Value.fToken);
}
//---------------------------------------------------------------------------
sfInt8 cAffixedVariableSet::cVariable::cItemOrderOperator::Compare(const cVariable &Item,const void *Token)noexcept(true)
{
	if(Item.fToken<Token)
		return -1;
	if(Item.fToken==Token)
		return 0;
	return 1;
}
//---------------------------------------------------------------------------
