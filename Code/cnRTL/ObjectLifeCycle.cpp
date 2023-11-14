//---------------------------------------------------------------------------
#include "ObjectLifeCycle.h"

using namespace cnLibrary;
using namespace cnRTL;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void bcVirtualLifeCycle::bcManager::Manage(bcVirtualLifeCycle *Object)noexcept(true)
{
	Object->LifeCycleManager=this;
}
//---------------------------------------------------------------------------
void bcVirtualLifeCycle::bcManager::NotifyObjectStart(bcVirtualLifeCycle *Object)noexcept(true)
{
	Object->LifeCycleManager->ObjectStart(Object);
}
//---------------------------------------------------------------------------
void bcVirtualLifeCycle::VirtualDelete(void)noexcept(true)
{
	LifeCycleManager->ObjectFinished(this);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void cDualReference::cInner::IncreaseReference(void)noexcept(true)
{
	return Inc();
}
void cDualReference::cInner::DecreaseReference(void)noexcept(true)
{
	return Dec();
}
//---------------------------------------------------------------------------
void cDualReference::cInner::Inc(void)noexcept(true)
{
	cnLib_ASSERT(Ref!=cnVar::TIntegerValue<uIntn>::Max);
	Ref.Free++;
}
void cDualReference::cInner::Dec(void)noexcept(true)
{
	cnLib_ASSERT(Ref!=0);
	if(Ref.Free--==1){
		auto Host=cnMemory::GetObjectFromMemberPointer(this,&cDualReference::fInnerReference);
		Host->VirtualDelete();
	}
}
//---------------------------------------------------------------------------
