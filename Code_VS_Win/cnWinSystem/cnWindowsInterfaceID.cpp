#include <cnSystem\cnSystem.h>
#include <cnSystem\OpenGLES.h>
#include <cnSystem\Socket.h>
#include <cnSystem\cnWindows.h>

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
static cnLib_CONSTVAR uInt8 LibID_cnWindows[16]={0x65,0x81,0x56,0x8d,0xc8,0x19,0x20,0x4a,0x97,0x1e,0x43,0xfa,0x8d,0x45,0x9b,0x33};
//---------------------------------------------------------------------------

cnSystem_INTERFACE_INFO_DECL cnLib_CONSTVAR iTypeInfo IID_iCOMInterface={
	&LibID_cnWindows,
	0x01,
};

cnSystem_INTERFACE_INFO_DECL cnLib_CONSTVAR iTypeInfo IID_iWaitableObject={
	&LibID_cnWindows,
	0x10,
};

cnSystem_INTERFACE_INFO_DECL cnLib_CONSTVAR iTypeInfo IID_iWinThread={
	&LibID_cnWindows,
	0x11,
};

cnSystem_INTERFACE_INFO_DECL cnLib_CONSTVAR iTypeInfo IID_iWinThreadPool={
	&LibID_cnWindows,
	0x12,
};

cnSystem_INTERFACE_INFO_DECL cnLib_CONSTVAR iTypeInfo IID_iFileTime={
	&LibID_cnWindows,
	0x20,
};

cnSystem_INTERFACE_INFO_DECL cnLib_CONSTVAR iTypeInfo IID_iWin32FileName={
	&LibID_cnWindows,
	0x21,
};

cnSystem_INTERFACE_INFO_DECL cnLib_CONSTVAR iTypeInfo IID_iWinFileHandle={
	&LibID_cnWindows,
	0x22,
};


cnSystem_INTERFACE_INFO_DECL cnLib_CONSTVAR iTypeInfo IID_iWinSocketMulticastAddress={
	&LibID_cnWindows,
	0x41,
};


cnSystem_INTERFACE_INFO_DECL cnLib_CONSTVAR iTypeInfo IID_iWindow={
	&LibID_cnWindows,
	0x60,
};

cnSystem_INTERFACE_INFO_DECL cnLib_CONSTVAR iTypeInfo IID_iWindowViewport={
	&LibID_cnWindows,
	0x61,
};


cnSystem_INTERFACE_INFO_DECL cnLib_CONSTVAR iTypeInfo IID_iWindowHandleProperty={
	&LibID_cnWindows,
	0x62,
};

cnSystem_INTERFACE_INFO_DECL cnLib_CONSTVAR iTypeInfo IID_iWindowClient={
	&LibID_cnWindows,
	0x63,
};

cnSystem_INTERFACE_INFO_DECL cnLib_CONSTVAR iTypeInfo IID_iWindowFrame={
	&LibID_cnWindows,
	0x64,
};

cnSystem_INTERFACE_INFO_DECL cnLib_CONSTVAR iTypeInfo IID_iMouseCursorSetter={
	&LibID_cnWindows,
	0x65,
};


cnSystem_INTERFACE_INFO_DECL cnLib_CONSTVAR iTypeInfo IID_iGDIBitmap={
	&LibID_cnWindows,
	0x71,
};


cnSystem_INTERFACE_INFO_DECL cnLib_CONSTVAR iTypeInfo IID_iWaveFormat={
	&LibID_cnWindows,
	0x90,
};

//---------------------------------------------------------------------------
}	// namespace InterfaceID
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
using namespace cnLibrary;
using namespace InterfaceID;

#define	cnSystem_INTERFACE_ID_DEFINE(_class_)	cnLib_CONSTVAR iTypeID _class_::tInterfaceID::Value=IID_##_class_;

#include "cnWindowsInterfaceIDList.inc"
