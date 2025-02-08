#include "WinCLI_Common.h"


using namespace cnLibrary;
using namespace cnRTL;
using namespace cnWin;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cGCRef::cGCRef()noexcept(true)
{
	GCHandleStorage=nullptr;
}
//---------------------------------------------------------------------------
cGCRef::cGCRef(cGCRef &&Src)noexcept(true)
{
	GCHandleStorage=Src.GCHandleStorage;
	Src.GCHandleStorage=nullptr;
}
//---------------------------------------------------------------------------
cGCRef::cGCRef(cGCHandle &&Src)noexcept(true)
{
	GCHandleStorage=static_cast<cGCRef&>(Src).GCHandleStorage;
	static_cast<cGCRef&>(Src).GCHandleStorage=nullptr;
}
//---------------------------------------------------------------------------
cGCRef::~cGCRef()noexcept(true)
{
	if(mIsAllocated())
		mFree();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void* iCLIObject::CastInterface(iTypeID IID)noexcept(true)
{
	if(iCLIObject::tInterfaceID::Value==IID)
		return this;
	return iInterface::CastInterface(IID);
}
//---------------------------------------------------------------------------
