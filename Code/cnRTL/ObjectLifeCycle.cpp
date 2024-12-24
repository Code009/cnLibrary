//---------------------------------------------------------------------------
#include "ObjectLifeCycle.h"

using namespace cnLibrary;
using namespace cnRTL;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void bcDisposable::Dispose(void)noexcept(true)
{
	LifeCycleDisposal->Dispose(this);
}
//---------------------------------------------------------------------------
void bcDisposable::iDisposal::Bind(bcDisposable *Object)noexcept(true)
{
	Object->LifeCycleDisposal=this;
}
//---------------------------------------------------------------------------
