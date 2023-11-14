//
//  cnMacOSX108_System.mm
//  cnPLMac_OSX108
//
//  Created by Code009 on 12-11-28.
//  Copyright (c) 2012年 Code009. All rights reserved.
//

#include <cnMac/Sys.h>
#include <cnMac/Memory.h>
#include <cnMac/Thread.h>
#include <cnMac/KernelQueue.h>
#include <cnMac/File.h>
#include <cnMac/Network.h>

#include <cnRTL/MemoryManagement.h>
#include <cnRTL/String.h>
#include <cnRTL/Interface.h>
#include <cnRTL/Debug.h>

#include <cnILib/cnILib.h>

using namespace cnLib;
using namespace cnRTL;
using namespace cnPLMac;

/*static const cInterfaceInfo IID_iUIViewPointer={};
static const cInterfaceInfo IID_iUIWindowPointer={};
static const cInterfaceInfo IID_iUIViewPainter={};

#define	DEFINE_IID(_class_)	const tiid _class_::IID=&IID_##_class_;

DEFINE_IID(iUIViewPointer)
DEFINE_IID(iUIWindowPointer)
DEFINE_IID(iUIViewPainter)
*/

#ifdef	cnLib_DEBUG
rReferenceTagMap cnRTL::gReferenceTagMap;
#endif

// memory
cVirtualMemoryManager cnPLMac::gVirtualMemoryManager;
cCFDefaultAllocatorHeap cnPLMac::gDefaultHeap;
// thread

cCurrentThread cnPLMac::gCurrentThread;
cGCDThreadPool cnPLMac::gGDCThreadPool;

static cThreadLocalPointerPool gThreadLocalPointerPool;
static cLastError gLastError;

// KernelQueue
cIOKernelQueue cnPLMac::gIOKernelQueue;

// Network

static cnRTL::cReferenceStaticImplementT<cSocketNetwork> sTCPNetwork;

// UI
//cUIMainGDCQueue cnPLMac_iOS5::gMainGCDQueue;


// cnRTL
iVirtualMemoryManager *const cnRTL::VMM=&gVirtualMemoryManager;
iMemoryHeap *const cnRTL::RTLMemoryHeap=&gDefaultHeap;
iThreadPool *const cnRTL::DefaultThreadPool=&gGDCThreadPool;

iLastError *const cnRTL::LastError=&gLastError;


cThreadLocalPointerPool*const cnRTL::ThreadLocalPointerPool=&gThreadLocalPointerPool;

cFolder_App cnPLMac::gFolder_App;
cFolder_Doc cnPLMac::gFolder_Doc;
cFolder_Tmp cnPLMac::gFolder_Tmp;



static_assert(sizeof(uchar16)==sizeof(UniChar),"incorrect char size");
/*
//---------------------------------------------------------------------------
bool					cnLib_Func cnSystem::ReadUnsafeMemory(const void *Pointer,ubyte &Data)
{
	try{
		Data=*static_cast<const ubyte*>(Pointer);
		return true;
	}
	catch(...){
	}
	return false;
}
//---------------------------------------------------------------------------
bool					cnLib_Func cnSystem::ReadUnsafeMemory(const void *Pointer,word &Data)
{
	try{
		Data=*static_cast<const word*>(Pointer);
		return true;
	}
	catch(...){
	}
	return false;
}
//---------------------------------------------------------------------------
bool					cnLib_Func cnSystem::ReadUnsafeMemory(const void *Pointer,dword &Data)
{
	try{
		Data=*static_cast<const dword*>(Pointer);
		return true;
	}
	catch(...){
	}
	return false;
}
//---------------------------------------------------------------------------
bool					cnLib_Func cnSystem::ReadUnsafeMemory(const void *Pointer,qword &Data)
{
	try{
		Data=*static_cast<const qword*>(Pointer);
		return true;
	}
	catch(...){
	}
	return false;
}
*/
//---------------------------------------------------------------------------
void					cnLib_Func cnSystem::ErrorReport(const uchar16 *Message)
{
	CFIndex CFStrLength=cnString::GetLength(Message);
	auto String=CFStringCreateWithCharactersNoCopy(kCFAllocatorDefault,reinterpret_cast<const unichar*>(Message),CFStrLength,kCFAllocatorNull);
	auto UTF8BufferSize=CFStringGetMaximumSizeForEncoding(CFStrLength,kCFStringEncodingUTF8);
	UInt8 UTF8String[UTF8BufferSize];
	CFRange range={0,CFStrLength};
	CFStringGetBytes(String,range,kCFStringEncodingUTF8,'?',false,UTF8String,UTF8BufferSize,&UTF8BufferSize);
	write(STDOUT_FILENO,UTF8String,UTF8BufferSize);
	CFRelease(String);
	// debug break
	__asm( "int3");
//	kill(getpid(),SIGTRAP);
//	kill(getpid(),SIGINT);

}
//---------------------------------------------------------------------------
integer		cnLib_Func cnSystem::CharacterConvertToUTF16(uchar16 *Dest,integer DestLength,const void *Src,integer SrcSize,integer SrcCodePage)
{
	CFStringEncoding SrcEncoding=CFStringConvertWindowsCodepageToEncoding(static_cast<unsigned int>(SrcCodePage));
	auto TempStr=CFStringCreateWithBytesNoCopy(NULL,static_cast<const UInt8*>(Src),SrcSize,SrcEncoding,false,kCFAllocatorNull);
	CFRange range={0,CFStringGetLength(TempStr)};
	if(Dest==nullptr){
		CFRelease(TempStr);
		return range.length;
	}
	if(range.length>DestLength)
		range.length=DestLength;
	
	CFStringGetCharacters(TempStr,range,reinterpret_cast<UniChar*>(Dest));

	CFRelease(TempStr);
	return range.length;
}
//---------------------------------------------------------------------------
integer		cnLib_Func cnSystem::CharacterConvertFromUTF16(void *Dest,integer DestSize,integer DestCodePage,const uchar16 *Src,integer SrcLength)
{
	CFStringEncoding DestEncoding=CFStringConvertWindowsCodepageToEncoding(static_cast<unsigned int>(DestCodePage));
	CFIndex CFStrLength=cnString::GetLength(Src,SrcLength);
	auto TempStr=CFStringCreateWithCharactersNoCopy(NULL,reinterpret_cast<const UniChar*>(Src),SrcLength,kCFAllocatorNull);
	CFIndex DestSizeCopied;
	CFRange range={0,CFStrLength};
	CFStringGetBytes(TempStr,range,DestEncoding,'?',false,static_cast<UInt8*>(Dest),DestSize,&DestSizeCopied);
	CFRelease(TempStr);
	return DestSizeCopied;
}
/*
//---------------------------------------------------------------------------
iLastError*				cnLib_Func cnSystem::GetLastError(void)
{
	return nullptr;
}
//---------------------------------------------------------------------------
iConsole*		cnLib_Func cnSystem::GetConsole(void)
{
	return nullptr;
}
//---------------------------------------------------------------------------
iDebugLog*		cnLib_Func cnSystem::GetConsoleLog(void)
{
	return nullptr;
}
//---------------------------------------------------------------------------
iRandomNumberGenerator*		cnLib_Func cnSystem::GetRandomNumberGenerator(void)
{
	return nullptr;
}
*/
//---------------------------------------------------------------------------
iVirtualMemoryManager*	cnLib_Func cnSystem::GetVirtualMemoryManager(void)
{
	return &gVirtualMemoryManager;
}
/*
//---------------------------------------------------------------------------
iVirtualRingBuffer*		cnLib_Func cnSystem::CreateVirtualRingBuffer(integer &RequestedSize)
{
	return nullptr;
}
*/
//---------------------------------------------------------------------------
rPtr<iExternalMemory>	cnLib_Func cnSystem::CreateExternalMemoryFromFile(iFile *File,qword Size,integer ProtectFlag)
{
	return rCreate<cExternalMemory>(Size);
}
//---------------------------------------------------------------------------
iMemoryHeap*			cnLib_Func cnSystem::GetDefaultMemoryManagemer(void)
{
	return &gDefaultHeap;
}
/*
//---------------------------------------------------------------------------
iMemoryHeap*			cnLib_Func cnSystem::CreateMemoryManagemer(integer MaxSize)
{
	return nullptr;
}*/
//---------------------------------------------------------------------------
iResLock*				cnLib_Func cnSystem::CreateResLock(void)
{
	return new cResLock;
}
//---------------------------------------------------------------------------
iSRWLock*				cnLib_Func cnSystem::CreateSRWLock(void)
{
	return new cSRWLock;
}
/*
//---------------------------------------------------------------------------
iThreadTimer*			cnLib_Func cnSystem::CreateThreadTimer(void)
{
	return nullptr;
}*/
//---------------------------------------------------------------------------
iCurrentThread*			cnLib_Func cnSystem::GetCurrentThread(void)
{
	return &gCurrentThread;
}
//---------------------------------------------------------------------------
iThreadLocalVariable*	cnLib_Func cnSystem::CreateThreadLocalVariable(ThreadLocalVariableDestroyProc *Destructor)
{
	if(Destructor!=nullptr)
		return new cPosixThreadLocalVariable(Destructor);
	else
		return new cPosixThreadLocalVariable;

}
//---------------------------------------------------------------------------
rPtr<iThread>			cnLib_Func cnSystem::CreateThread(iThreadProcedure *ThreadProcedure)
{
	return cPosixThread::Create(ThreadProcedure);
}
//---------------------------------------------------------------------------
iThreadPool*			cnLib_Func cnSystem::GetDefaultThreadPool(void)
{
	return &gGDCThreadPool;
}
//---------------------------------------------------------------------------
rPtr<iThreadPool>		cnLib_Func cnSystem::CreateThreadPool(void)
{
	return &gGDCThreadPool;
}
//---------------------------------------------------------------------------
iFolder*				cnLib_Func cnSystem::GetFolder(integer Folder)
{
	switch(Folder){
	case cnSystem::Folder::Application:
		return &gFolder_App;
	case cnSystem::Folder::Temp:
		return &gFolder_Tmp;
	case cnSystem::Folder::Document:
		return &gFolder_Doc;
	}
	return nullptr;
}
//---------------------------------------------------------------------------
rPtr<iInterface>		cnLib_Func cnSystem::CreateAddressIPv4(dword IP,word Port)
{
	return rCreate<cSocketAddress>(IP,Port);
}
//---------------------------------------------------------------------------
iNetwork*		cnLib_Func cnSystem::GetTCPNetwork(void)
{
	return &sTCPNetwork;
}
/*
//---------------------------------------------------------------------------
iMessagePort*			cnLib_Func cnSystem::CreateUDPNetwork(void)
{
	return nullptr;
}
*/
/*
//---------------------------------------------------------------------------
iUIFont*				cnLib_Func cnSystem::CreateUIFont(const uchar16 *FontName,int Height,int Weight,bool Italic)
{
	return nullptr;
}
//---------------------------------------------------------------------------
rPtr<iUIMessageQueue>	cnLib_Func cnSystem::CreateUIMessageQueue(void)
{
	// iOS cannot create more ui thread
	return nullptr;
}
//---------------------------------------------------------------------------
iUIMessageQueue*		cnLib_Func cnSystem::GetCurrentUIMessageQueue(void)
{
	if(CurrentThreadIsMainThread())
		return &gMainGCDQueue;
	return nullptr;
}
//---------------------------------------------------------------------------
rPtr<iUIWindow>				cnLib_Func cnSystem::CreateUIWindow(void)
{
	auto *wnd=[[cnLib_cnPLMac_iOS5_cUIWindow alloc] init];
	return rPtr<iUIWindow>::NewPointer(wnd.cnLib_cnPLMac_iOS5_UIWindow);
}
//---------------------------------------------------------------------------
rPtr<iUIView>				cnLib_Func cnSystem::CreateUIView(void)
{
	auto *View=[[cnLib_cnPLMac_iOS5_cUIView alloc]init];
	return rPtr<iUIView>::NewPointer(View.cnLib_cnPLMac_iOS5_UIView);
}
//---------------------------------------------------------------------------
iUIPaintEngine*			cnLib_Func cnSystem::GetDefaultPaintEngine(void)
{
	return nullptr;
}*/
//---------------------------------------------------------------------------
