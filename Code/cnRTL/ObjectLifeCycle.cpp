//---------------------------------------------------------------------------
#include "ObjectLifeCycle.h"

using namespace cnLibrary;
using namespace cnRTL;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void bcVirtualLifeCycle::VirtualDelete(void)noexcept(true)
{
	LifeCycleManager->Dispose(this);
}
//---------------------------------------------------------------------------
void bcVirtualLifeCycle::bcManager::Manage(bcVirtualLifeCycle *Object)noexcept(true)
{
	Object->LifeCycleManager=this;
}
//---------------------------------------------------------------------------
void cCPPLifeCycleRecyclableInstance::bcManager::Manage(cCPPLifeCycleRecyclableInstance *Object)noexcept(true)
{
	Object->LifeCycleManager=this;
}
//---------------------------------------------------------------------------
