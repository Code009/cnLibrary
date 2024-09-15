#include <cnSystem\cnSystem.h>
#include <cnSystem\OpenGLES.h>
#include <cnSystem\Socket.h>
#include <cnSystem\cnWindows.h>
#include <cnWinCLI\cnWinWPF.h>

using namespace cnLibrary;

#if cnLibrary_CPPFEATURE_CONSTEXPR >= 200704L
#define	cnLib_CONSTVAR	constexpr
#else	// cnLibrary_CPPFEATURE_CONSTEXPR < 200704L
#define	cnLib_CONSTVAR	const
#endif	// cnLibrary_CPPFEATURE_CONSTEXPR

#ifndef	cnSystem_INTERFACE_INFO_DECL
#define	cnSystem_INTERFACE_INFO_DECL	static
#endif



//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace InterfaceID{
//---------------------------------------------------------------------------


// {8D568165-19C8-4A20-971E-43FA8D459B33}
static uInt8 LibID_cnWindows[16]={0x65,0x81,0x56,0x8d,0xc8,0x19,0x20,0x4a,0x97,0x1e,0x43,0xfa,0x8d,0x45,0x9b,0x33};

//---------------------------------------------------------------------------

cnSystem_INTERFACE_INFO_DECL cnLib_CONSTVAR iTypeInfo IID_iCLIObject={
	&LibID_cnWindows,
	0x40001,
};

//---------------------------------------------------------------------------
}	// namespace InterfaceID
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------

using namespace cnLibrary;
using namespace InterfaceID;

using namespace cnWin;

cnLib_INTERFACE_LOCALID_DEFINE(cWPFUIView);
cnLib_INTERFACE_LOCALID_DEFINE(miWPFUISimpleGraph);

#define	cnSystem_INTERFACE_ID_DEFINE(_class_)	cnLib_CONSTVAR iTypeID _class_::tInterfaceID::Value=IID_##_class_;

#include "cnWinWPFInterfaceIDList.inc"


