
// {C6839BE5-E561-4404-9D8C-5B13C3416705}
static cnLib_IDCONST uInt8 LibID_cnLib[iTypeInfo::LibraryIDSize]={0xe5,0x9b,0x83,0xc6,0xe5,0x61,0x04,0x44,0x9d,0x8c,0x5b,0x13,0xc3,0x41,0x67,0x05};
//---------------------------------------------------------------------------

#define	DEFINE_IID(_class_)	cnLib_IDCONST iTypeID _class_::cnInterfaceID=&IID_##_class_

//---------------------------------------------------------------------------

cnLib_IDENTRY cnLib_IDCONST iTypeInfo IID_iPOSIXFileName={
	LibID_cnLib,
	0x1100,
};
DEFINE_IID(iPOSIXFileName);


//---------------------------------------------------------------------------

#undef	DEFINE_IID
