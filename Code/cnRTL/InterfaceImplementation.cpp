//---------------------------------------------------------------------------
#include "InterfaceImplementation.h"

using namespace cnLibrary;
using namespace cnRTL;

//---------------------------------------------------------------------------
//const cNullRet cnRTL::NullRet;
//---------------------------------------------------------------------------
void cStaticRefernce::IncreaseReference(void)noexcept(true)
{
}
void cStaticRefernce::DecreaseReference(void)noexcept(true)
{
}
//---------------------------------------------------------------------------
static cStaticRefernce gStaticReferenceImplement;
iReference *const cnRTL::gStaticReference=&gStaticReferenceImplement;
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
