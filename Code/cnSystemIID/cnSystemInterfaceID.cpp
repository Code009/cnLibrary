#include <cnSystem/cnSystem.h>
#include <cnSystem/Socket.h>

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

cnSystem_INTERFACE_INFO_DECL cnLib_CONSTVAR iTypeInfo IID_iReference={
	&LibID_cnLib,
	0x1,
};

cnSystem_INTERFACE_INFO_DECL cnLib_CONSTVAR iTypeInfo IID_iObservedReference={
	&LibID_cnLib,
	0x2,
};


cnSystem_INTERFACE_INFO_DECL cnLib_CONSTVAR iTypeInfo IID_iThread={
	&LibID_cnLib,
	0x10,
};

cnSystem_INTERFACE_INFO_DECL cnLib_CONSTVAR iTypeInfo IID_iDataCache={
	&LibID_cnLib,
	0x20,
};

//cnSystem_INTERFACE_INFO_DECL cnLib_CONSTVAR iTypeInfo IID_iFile={
//	&LibID_cnLib,
//	0x30,
//};
//DEFINE_IID(iFile);

cnSystem_INTERFACE_INFO_DECL cnLib_CONSTVAR iTypeInfo IID_iFileName={
	&LibID_cnLib,
	0x31,
};


cnSystem_INTERFACE_INFO_DECL cnLib_CONSTVAR iTypeInfo IID_iConnection={
	&LibID_cnLib,
	0x40,
};


cnSystem_INTERFACE_INFO_DECL cnLib_CONSTVAR iTypeInfo IID_iStream={
	&LibID_cnLib,
	0x41,
};

cnSystem_INTERFACE_INFO_DECL cnLib_CONSTVAR iTypeInfo IID_iEndpoint={
	&LibID_cnLib,
	0x42,
};

cnSystem_INTERFACE_INFO_DECL cnLib_CONSTVAR iTypeInfo IID_iUIView={
	&LibID_cnLib,
	0x80,
};


cnSystem_INTERFACE_INFO_DECL cnLib_CONSTVAR iTypeInfo IID_iUIWindow={
	&LibID_cnLib,
	0x81,
};

cnSystem_INTERFACE_INFO_DECL cnLib_CONSTVAR iTypeInfo IID_iUIScreen={
	&LibID_cnLib,
	0x82,
};

cnSystem_INTERFACE_INFO_DECL cnLib_CONSTVAR iTypeInfo IID_iUIKeyControl={
	&LibID_cnLib,
	0x84,
};

cnSystem_INTERFACE_INFO_DECL cnLib_CONSTVAR iTypeInfo IID_iUIMouseControl={
	&LibID_cnLib,
	0x85,
};

cnSystem_INTERFACE_INFO_DECL cnLib_CONSTVAR iTypeInfo IID_iUITouchControl={
	&LibID_cnLib,
	0x86,
};

cnSystem_INTERFACE_INFO_DECL cnLib_CONSTVAR iTypeInfo IID_iPrintPaper={
	&LibID_cnLib,
	0x87,
};

//cnSystem_INTERFACE_INFO_DECL cnLib_CONSTVAR iTypeInfo IID_iUIBitmap={
//	&LibID_cnLib,
//	0xA0,
//};


cnSystem_INTERFACE_INFO_DECL cnLib_CONSTVAR iTypeInfo IID_iStreamErrorReport={
	&LibID_cnLib,
	0x0101,
};

cnSystem_INTERFACE_INFO_DECL cnLib_CONSTVAR iTypeInfo IID_iConnectErrorReport={
	&LibID_cnLib,
	0x102,
};

#ifdef __cnLibrary_cnSystem_Socket_H__

cnSystem_INTERFACE_INFO_DECL cnLib_CONSTVAR iTypeInfo IID_iSocketAddress{
	&LibID_cnLib,
	0x211,
};

#endif // __cnLibrary_cnSystem_Socket_H__

cnSystem_INTERFACE_INFO_DECL cnLib_CONSTVAR iTypeInfo IID_iIPv4Address{
	&LibID_cnLib,
	0x212,
};

//---------------------------------------------------------------------------
}	// namespace InterfaceID
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------

	
using namespace cnLibrary;
using namespace InterfaceID;

#define	cnSystem_INTERFACE_ID_DEFINE(_class_)	cnLib_CONSTVAR iTypeID TInterfaceIDDefine<_class_>::Value=IID_##_class_;

#include "cnSystemInterfaceIDList.inc"


