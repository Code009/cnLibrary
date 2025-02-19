#include <cnSystem/POSIXSystem.h>

using namespace cnLibrary;

#if cnLibrary_CPPFEATURE_CONSTEXPR >= 200704L
#define	cnLib_CONSTVAR	constexpr
#else	// cnLibrary_CPPFEATURE_CONSTEXPR < 200704L
#define	cnLib_CONSTVAR	const
#endif	// cnLibrary_CPPFEATURE_CONSTEXPR

#ifndef	cnSystem_INTERFACE_INFO_DECL
#define	cnSystem_INTERFACE_INFO_DECL static
#endif

//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace InterfaceID{
//---------------------------------------------------------------------------

// {C6839BE5-E561-4404-9D8C-5B13C3416705}
static cnLib_CONSTVAR uInt8 LibID_cnLib[16]={0xe5,0x9b,0x83,0xc6,0xe5,0x61,0x04,0x44,0x9d,0x8c,0x5b,0x13,0xc3,0x41,0x67,0x05};


cnSystem_INTERFACE_INFO_DECL cnLib_CONSTVAR iTypeInfo IID_iPOSIXFileName={
	&LibID_cnLib,
	0x1100,
};

//---------------------------------------------------------------------------
}	// namespace InterfaceID
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------

using namespace cnLibrary;
using namespace InterfaceID;

#define	cnSystem_INTERFACE_ID_DEFINE(_class_)	cnLib_CONSTVAR iTypeID _class_::tInterfaceID::Value=IID_##_class_;


#include "POSIXSystemIID.inc"
