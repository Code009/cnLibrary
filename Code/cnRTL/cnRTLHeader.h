#ifndef __cnLibrary_cnRTL_cnRTLHeader_H__
#define __cnLibrary_cnRTL_cnRTLHeader_H__

#include <cnRTL/cnRTLCore.h>
#include <cnSystem/cnSystem.h>

/*-------------------------------------------------------------------------*/
#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnRTL{
//---------------------------------------------------------------------------
// cnVar

using cnVar::Forward;
using cnVar::MoveCast;

using cnVar::cAnyVariable;

using cnVar::cAnyPtr;



using cnMemory::ArrayLength;

//---------------------------------------------------------------------------
}	// namespace cnRTL
//---------------------------------------------------------------------------
}   // namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif