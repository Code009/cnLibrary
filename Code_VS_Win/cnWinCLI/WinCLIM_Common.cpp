#include "WinDNetM_Common.h"

using namespace cnLibrary;
using namespace cnWin;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
rcProcedureCaller::rcProcedureCaller(iProcedure *Procedure)
	: fProcedure(Procedure)
{
}
//---------------------------------------------------------------------------
void rcProcedureCaller::Execute(void)
{
	fProcedure->Execute();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
rcRefProcedureCaller::rcRefProcedureCaller(iReference *Reference,iProcedure *Procedure)
	: fProcedure(Procedure)
	, fReference(Reference)
{
}
//---------------------------------------------------------------------------
void rcRefProcedureCaller::Execute(void)
{
	rcRefProcedureCaller_CallProc(fReference,fProcedure);
}
//---------------------------------------------------------------------------
