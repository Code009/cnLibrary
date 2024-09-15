#include "WinCLI_Common.h"


using namespace cnLibrary;
using namespace cnRTL;
using namespace cnWin;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cGCRef::cGCRef()noexcept
{
	GCHandleStorage=nullptr;
}
//---------------------------------------------------------------------------
cGCRef::cGCRef(cGCRef &&Src)noexcept
{
	GCHandleStorage=Src.GCHandleStorage;
	Src.GCHandleStorage=nullptr;
}
//---------------------------------------------------------------------------
cGCRef::cGCRef(cGCHandle &&Src)noexcept
{
	GCHandleStorage=static_cast<cGCRef&>(Src).GCHandleStorage;
	static_cast<cGCRef&>(Src).GCHandleStorage=nullptr;
}
//---------------------------------------------------------------------------
cGCRef::~cGCRef()noexcept
{
	if(mIsAllocated())
		mFree();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void* iCLIObject::CastInterface(iTypeID IID)noexcept
{
	if(iCLIObject::tInterfaceID::Value==IID)
		return this;
	return iInterface::CastInterface(IID);
}
//---------------------------------------------------------------------------
