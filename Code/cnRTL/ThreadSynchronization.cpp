//---------------------------------------------------------------------------
#include "ThreadSynchronization.h"
#include "RuntimeFunction.h"

using namespace cnLibrary;
using namespace cnRTL;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void cWaitObject::Acquire(void)noexcept
{
	fRefCount.Free++;
}
//---------------------------------------------------------------------------
void cWaitObject::Release(void)noexcept
{
	if(fRefCount.Free--==0){
		if(fWaitThread!=nullptr){
			fWaitThread->Wake(&fWaitFlag);
		}
	}
}
//---------------------------------------------------------------------------
bool cWaitObject::Check(void)noexcept
{
	return fRefCount==0;
}
//---------------------------------------------------------------------------
void cWaitObject::Wait(void)noexcept
{
	if(fRefCount==0)
		return;

	fWaitFlag=true;
	fWaitThread=cnSystem::CurrentThread::GetThread();
	if(fRefCount.Free--!=0){
		while(fWaitFlag){
			cnSystem::CurrentThread::SleepUntil(SystemTime_Never);
		}
	}
	fRefCount.Free++;
	fWaitThread=nullptr;
}
//---------------------------------------------------------------------------
bool cWaitObject::WaitUntil(uInt64 SystemTime)noexcept
{
	if(fRefCount==0)
		return true;

	fWaitFlag=true;
	fWaitThread=cnSystem::CurrentThread::GetThread();
	if(fRefCount.Free--!=0){
		while(fWaitFlag){
			if(cnSystem::CurrentThread::SleepUntil(SystemTime)==false){
				// timeout
				if(++fRefCount.Free!=0){
					// state reseted
					fWaitThread=nullptr;
					return false;
				}
				// other thread must notifing, continue wait
				fRefCount.Free--;
			}
		}
	}
	fRefCount.Free++;
	fWaitThread=nullptr;
	return true;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWaitReference::cWaitReference()noexcept
{
}
//---------------------------------------------------------------------------
cWaitReference::~cWaitReference()noexcept
{
}
//---------------------------------------------------------------------------
void cWaitReference::IncreaseReference(void)noexcept
{
	Acquire();
}
//---------------------------------------------------------------------------
void cWaitReference::DecreaseReference(void)noexcept
{
	Release();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void cThreadSingleNotification::Setup(void)noexcept
{
	fNotifyThread=cnSystem::CurrentThread::GetThread();
	fWaiting=true;
}
//---------------------------------------------------------------------------
void cThreadSingleNotification::Wait(void)noexcept
{
	while(fWaiting){
		cnSystem::CurrentThread::SleepUntil(SystemTime_Never);
	}
}
//---------------------------------------------------------------------------
void cThreadSingleNotification::Notify(void)noexcept
{
	fNotifyThread->Wake(&fWaiting);
}
//---------------------------------------------------------------------------
void cThreadSingleNotification::Clear(void)noexcept
{
	fNotifyThread=nullptr;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cThreadOneTimeNotifier::cThreadOneTimeNotifier()noexcept
{
	Reset();
}
//---------------------------------------------------------------------------
cThreadOneTimeNotifier::~cThreadOneTimeNotifier()noexcept
{
}
//---------------------------------------------------------------------------
void cThreadOneTimeNotifier::Reset(void)noexcept
{
	fNotifyThread=cnSystem::CurrentThread::GetThread();
	fWaiting=true;
}
//---------------------------------------------------------------------------
void cThreadOneTimeNotifier::Wait(void)noexcept
{
	while(fWaiting){
		cnSystem::CurrentThread::SleepUntil(SystemTime_Never);
	}
}
//---------------------------------------------------------------------------
void cThreadOneTimeNotifier::Notify(void)noexcept
{
	fNotifyThread->Wake(&fWaiting);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cAffixedVariableSet::cAffixedVariableSet()noexcept
	: fSetMutex(cnSystem::CreateMutexLock())
{
}
//---------------------------------------------------------------------------
cAffixedVariableSet::~cAffixedVariableSet()noexcept
{
}
//---------------------------------------------------------------------------
rPtr<iVariable> cAffixedVariableSet::QueryAffixedVariable(const void *Token)noexcept
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
void cAffixedVariableSet::VariableReleased(cVariable *Variable)noexcept
{
	auto AutoLock=TakeLock(fSetMutex);
	fSet.Remove(Variable);
	delete Variable;
}
//---------------------------------------------------------------------------
cAffixedVariableSet::cVariable::cVariable(cAffixedVariableSet *Owner,const void *Token)noexcept
	: fOwner(Owner)
	, fToken(Token)
	, fValue(nullptr)
	, fRefCount(1)
{
}
//---------------------------------------------------------------------------
cAffixedVariableSet::cVariable::~cVariable()noexcept
{
}
//---------------------------------------------------------------------------
bool cAffixedVariableSet::cVariable::MakeReference(void)noexcept
{
	if(fRefCount.Free++==0){
		// is releasing by others
		return false;
	}
	return true;
}
//---------------------------------------------------------------------------
void cAffixedVariableSet::cVariable::IncreaseReference(void)noexcept
{
	++fRefCount.Free;
}
//---------------------------------------------------------------------------
void cAffixedVariableSet::cVariable::DecreaseReference(void)noexcept
{
	if(--fRefCount.Free==0){
		fOwner->VariableReleased(this);
	}
}
//---------------------------------------------------------------------------
void cAffixedVariableSet::cVariable::Clear(void)noexcept
{
	fReference=nullptr;
	fValue=nullptr;
}
//---------------------------------------------------------------------------
void* cAffixedVariableSet::cVariable::Get(void)noexcept
{
	return fValue;
}
//---------------------------------------------------------------------------
void cAffixedVariableSet::cVariable::Set(iReference *Reference,void *Value)noexcept
{
	fReference=Reference;
	fValue=Value;
}
//---------------------------------------------------------------------------
sfInt8 cAffixedVariableSet::cVariable::cItemOrderOperator::Compare(const cVariable &Item,const cVariable &Value)noexcept
{
	return Compare(Item,Value.fToken);
}
//---------------------------------------------------------------------------
sfInt8 cAffixedVariableSet::cVariable::cItemOrderOperator::Compare(const cVariable &Item,const void *Token)noexcept
{
	if(Item.fToken<Token)
		return -1;
	if(Item.fToken==Token)
		return 0;
	return 1;
}
//---------------------------------------------------------------------------
