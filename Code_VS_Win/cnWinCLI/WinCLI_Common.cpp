
#include "WinDNet_Common.h"


using namespace cnLibrary;
using namespace cnRTL;
using namespace cnWin;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void* iCLIObject::CastInterface(iTypeID IID)noexcept
{
	if(iCLIObject::tInterfaceID::Value==IID)
		return this;
	return iInterface::CastInterface(IID);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void cnWin::rcRefProcedureCaller_RefProc(iReference *Reference)noexcept
{
	rIncReference(Reference,'exec');
}
//---------------------------------------------------------------------------
void cnWin::rcRefProcedureCaller_CallProc(iReference *Reference,iProcedure *Procedure)noexcept
{
	Procedure->Execute();
	rDecReference(Reference,'exec');
}
//---------------------------------------------------------------------------
