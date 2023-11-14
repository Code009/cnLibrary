#include <cnSystem/POSIXSystem.h>

using namespace cnLibrary;

#ifndef	cnLib_IDENTRY
#define	cnLib_IDENTRY	static
#endif

#ifdef cnLibrary_CPPEXCLUDE_CONSTEXPR
#define	cnLib_IDCONST	const
#else
#define	cnLib_IDCONST	constexpr
#endif	// cnLibrary_CPPEXCLUDE_CONSTEXPR


#include "POSIXSystemIID.inc"
