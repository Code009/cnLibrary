//
//  iOS.mm
//  cnSystem
//
//  Created by Code009 on 12-5-28.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//
#define OS_OBJECT_USE_OBJC 0

#include <cnSystem/iOS.h>

#include <siOS/POSIX/POSIX_Memory.h>
#include <siOS/POSIX/POSIX_Thread.h>


#include <cnRTL/cnRTL.h>

#include <cnMac/Mac_Memory.h>
#include <cnMac/Mac_Thread.h>
#include <cnMac/Mac_GCD.h>
#include <cnMac/Mac_File.h>
#include <cnMac/Mac_Socket.h>
#include <cnMac/Mac_UIThread.h>
#include <cnMac/Mac_QuartzCore.h>

#include <cnMac.iOS/iOS_UIPaintTarget.h>
#include <cnMac.iOS/iOS_UIApp.h>
#include <cnMac.iOS/iOS_UIView.h>

using namespace cnLibrary;
using namespace cnRTL;
using namespace cnMac;
using namespace cniOS;


#define	DEFINE_IID(_class_)	const iTypeID _class_::cnInterfaceID=&IID_##_class_

static const iTypeInfo IID_iGCDDispatch={};
DEFINE_IID(iGCDDispatch);

static const iTypeInfo IID_iOCObject={};
DEFINE_IID(iOCObject);

static const iTypeInfo IID_iCFCGImage={};
DEFINE_IID(iCFCGImage);

static const iTypeInfo IID_iCGContextDrawable={};
DEFINE_IID(iCGContextDrawable);

static const iTypeInfo IID_iUIViewCALayerHost={};
DEFINE_IID(iUIViewCALayerHost);





static cReferenceStaticImplementT<cMAllocHeap> DefaultMallocHeap(malloc_default_zone());

#if	0
class cDebugLogStream : public iLogStream
{
private:
	rPtr<iTextEncodingConverter> Converter;
	uInt8 fLogLevel;
public:
	cDebugLogStream();
	virtual void Log(iLogContent *Content)override;
	void SetLevel(uInt8 Level);

};

cDebugLogStream::cDebugLogStream()
	: Converter(cnSystem::UnicodeTranscoder(1,2))
{
	fLogLevel=255;
}
void cDebugLogStream::SetLevel(uInt8 Level)
{
	fLogLevel=Level;
}

void cDebugLogStream::Log(iLogContent *Content)
{
	auto Level=Content->GetLevel();
	if(Level>fLogLevel)
		return;
	/*cStringBuffer<char> ConvertBuffer;
	uintn LogLength;
	auto LogStr=Content->GetText(LogLength);
	ConvertBuffer.AppendConverted(Converter,LogStr,LogLength);
	write(STDOUT_FILENO,ConvertBuffer.CStr(),ConvertBuffer.GetLength());
	*/
	uIntn LogLength=Content->GetTextLength();
	auto LogStr=Content->GetText();
	auto Str=[[NSString alloc]initWithCharacters:(const UniChar*)LogStr length:LogLength];
	NSLog(@"%@",Str);
	[Str release];
}
//static cReferenceLog gDebugReferenceLog;
//static cInterfaceStaticImplementT<cDebugLogStream> gDebugLogStream;

#endif

// data
const sInt64 cnSystem::SystemTimeEpochSecondsSinceUnixEpoch=0;


// thread

static cReferenceStaticImplementT<siPOSIX::cMutexLock> gGlobalMutex;

siPOSIX::cPosixThread::cModule siPOSIX::cPosixThread::gModule;

static cInterfaceStaticImplementT<cGCDThreadPool> gDefaultGDCThreadPool(GCDDefaultQueue());

// File

cAIOTaskBlockGCDManager cnMac::gAIOTaskBlockGCDManager;

// UI
ciOSMainUIThread cnMac::gMainUIThread;

cnRTL::cReferenceStaticImplementT<cCGSimplePaintDevice> cnMac::gCGSimplePaintDevice;

//---------------------------------------------------------------------------
iMemoryHeap*const		cnSystem::DefaultMemoryHeap=&DefaultMallocHeap;
//---------------------------------------------------------------------------
iMutex*const			cnSystem::GlobalMutex=&gGlobalMutex;
//---------------------------------------------------------------------------
iThreadPool*const		cnSystem::ThreadPool=&gDefaultGDCThreadPool;


//iLogStream*const		cnSystem::DebugLogStream=&gDebugLogStream;


namespace{
struct cFolderNames
{
	cString<char> App;
	cString<char> Doc;
	cString<char> Tmp;

	cFolderNames(){
		@autoreleasepool{
			auto MainPath=[[NSBundle mainBundle] bundlePath];
			cStringBuffer<char> AppPath;
			CFIndex CFStrLength=MainPath.length;
			CFRange range={0,CFStrLength};

			auto StrLength=CFStrLength*2;
			auto StrIndex=AppPath.GetLength();
			AppPath.Append(StrLength);
			CFIndex StrSizeCopied;
			CFStringGetBytes((__bridge CFStringRef)MainPath,range,kCFStringEncodingUTF8,'?',false,
				reinterpret_cast<UInt8*>(&AppPath[StrIndex]),StrLength,&StrSizeCopied);
			AppPath.SetLength(StrSizeCopied);

			App=cnVar::MoveCast(AppPath);
		}


		cStringBuffer<char> DataPath;
		uIntn DataPathLength;
		@autoreleasepool{
			auto home=NSHomeDirectory();
			CFIndex CFStrLength=home.length;
			CFRange range={0,CFStrLength};

			auto StrLength=CFStrLength*2;
			auto StrIndex=DataPath.GetLength();
			DataPath.Append(StrLength);
			CFIndex StrSizeCopied;
			CFStringGetBytes((__bridge CFStringRef)home,range,kCFStringEncodingUTF8,'?',false,
				reinterpret_cast<UInt8*>(&DataPath[StrIndex]),StrLength,&StrSizeCopied);
			DataPathLength=StrSizeCopied;
		}

		DataPath.SetLength(DataPathLength);

		static const char DocFolder[]="/Documents";
		static const uIntn DocFolderLength=sizeof(DocFolder)-1;
		DataPath.Append(DocFolder,DocFolderLength);
		Doc=cnVar::MoveCast(DataPath);

		DataPath.SetLength(DataPathLength);

		static const char TmpFolder[]="/tmp";
		static const uIntn TmpFolderLength=sizeof(TmpFolder)-1;
		DataPath.Append(TmpFolder,TmpFolderLength);
		Tmp=cnVar::MoveCast(DataPath);
	}
	~cFolderNames(){
	}
};
static cFolderNames gFolderNames;
}
//---------------------------------------------------------------------------
static cnRTL::cInterfaceStaticImplementT<siPOSIX::cStringFileName> gFolder_App(gFolderNames.App);
static cnRTL::cInterfaceStaticImplementT<siPOSIX::cStringFileName> gFolder_Doc(gFolderNames.Doc);
static cnRTL::cInterfaceStaticImplementT<siPOSIX::cStringFileName> gFolder_Tmp(gFolderNames.Tmp);
//---------------------------------------------------------------------------

// cniOS

//---------------------------------------------------------------------------
iFileName*const			cniOS::Folder_Application=&gFolder_App;
iFileName*const			cniOS::Folder_Temp=&gFolder_Tmp;
iFileName*const			cniOS::Folder_Document=&gFolder_Doc;

// cnRTL

//static cnRTL::cInterfaceStaticImplementT<cSocketConnectionProtocol> gTCP(SOCK_STREAM,IPPROTO_TCP);
//static cnRTL::cInterfaceStaticImplementT<cSocketDatagramProtocol> gUDP(SOCK_DGRAM,IPPROTO_UDP);
//iConnectionProtocol*const	cnSystem::TCP=&gTCP;
//iDatagramProtocol*const		cnSystem::UDP=&gUDP;

iAudioDevice*const			cnSystem::DefaultAudioDevice=nullptr;

static_assert(sizeof(uChar16)==sizeof(UniChar),"incorrect char size");

//---------------------------------------------------------------------------
/*
void					cnLib_FUNC cnSystem::AssertReport(const char *File,int LineNumber,const char *Expression)
{
	auto ExpLen=strlen(Expression);
	write(STDOUT_FILENO,Expression,ExpLen);
	// debug break
#ifdef	DEBUG
#if TARGET_CPU_X86
	__asm( "int3");
#endif
#endif
//	kill(getpid(),SIGTRAP);
//	kill(getpid(),SIGINT);

}
*/
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void* cnSystem::DefaultHeap::Alloc(uIntn Size,uIntn Alignment)
{
	return DefaultMallocHeap.Alloc(Size,Alignment);
}
//---------------------------------------------------------------------------
bool cnSystem::DefaultHeap::Free(void *Pointer,uIntn Size,uIntn Alignment)
{
	return DefaultMallocHeap.Free(Pointer,Size,Alignment);
}
//---------------------------------------------------------------------------
bool cnSystem::DefaultHeap::Resize(void *Pointer,uIntn Size,uIntn NewSize)
{
	return DefaultMallocHeap.Resize(Pointer,Size,NewSize);
}
//---------------------------------------------------------------------------
void* cnSystem::DefaultHeap::Relocate(void *Pointer,uIntn Size,uIntn Alignment,uIntn NewSize,uIntn NewAlignment,bool &ManualCopy)
{
	return DefaultMallocHeap.Relocate(Pointer,Size,Alignment,NewSize,NewAlignment,ManualCopy);
}
//---------------------------------------------------------------------------
uIntn cnSystem::DefaultHeap::SizeOf(void *Pointer)
{
	return DefaultMallocHeap.SizeOf(Pointer);
}
//---------------------------------------------------------------------------
rPtr<iTextEncodingConverter> cnSystem::UnicodeTranscoder(uInt8 ToEncodingSize,uInt8 FromEncodingSize)
{
	const char *ToEncoding;
	const char *FromEncoding;
	switch(ToEncodingSize){
	default:
		return nullptr;
	case 1:
		ToEncoding="UTF-8";
		break;
	case 2:
		ToEncoding="UTF-16";
		break;
	case 4:
		ToEncoding="UTF-32";
		break;
	}
	switch(FromEncodingSize){
	default:
		return nullptr;
	case 1:
		FromEncoding="UTF-8";
		break;
	case 2:
		FromEncoding="UTF-16";
		break;
	case 4:
		FromEncoding="UTF-32";
		break;
	}
	//auto conv=iconv_open(ToEncoding,FromEncoding);
	//if(conv==reinterpret_cast<iconv_t>(-1))
	//	return NullRet;
	//return rCreateRet<iCharacterSetConverter,siPOSIX::cCharacterSetConverter>(conv);

	return nullptr;
}
//---------------------------------------------------------------------------
void cnSystem::DebugLogReferenceInc(void *Object,uInt32 Tag)
{
	//gDebugReferenceLog.Add(Object,Tag);
}
void cnSystem::DebugLogReferenceDec(void *Object,uInt32 Tag)
{
	//gDebugReferenceLog.Sub(Object,Tag);
}
//---------------------------------------------------------------------------
sInt64 cnSystem::GetSystemTimeNow(void)
{
	cClockService Clock;
	return Clock.GetSystemTimeNow();
}
//---------------------------------------------------------------------------
bool cnSystem::UTCGregorianDateFromSystemTime(cDateTime &DateTime,sInt64 SystemTime)
{
	auto timeZone=CFTimeZoneCreateWithTimeIntervalFromGMT(nullptr,0);
	auto cftime=cnSystemTimeToCFTime(SystemTime);
	auto Calendar=CFCalendarCopyCurrent();
	int y,M,d,h,m,s;
	bool ret=CFCalendarDecomposeAbsoluteTime(Calendar,cftime,"yMdhms",&y,&M,&d,&h,&m,&s);

	CFRelease(Calendar);
	CFRelease(timeZone);
	if(ret==false)
		return false;

	DateTime.Year=y;
	DateTime.Month=M;
	DateTime.Day=d;
	DateTime.Hour=h;
	DateTime.Minute=m;
	DateTime.Second=s;
	DateTime.NanoSecond=0;
	//DateTime.NanoSecond=(s-DateTime.Second)*1'000'000'000;
	return true;
}
//---------------------------------------------------------------------------
bool cnSystem::UTCGregorianDateToSystemTime(sInt64 &SystemTime,const cDateTime &DateTime)
{
	auto timeZone=CFTimeZoneCreateWithTimeIntervalFromGMT(nullptr,0);
	CFAbsoluteTime cftime;
	auto Calendar=CFCalendarCopyCurrent();
	bool ret=CFCalendarComposeAbsoluteTime(Calendar,&cftime,"yMdhms",
		DateTime.Year,DateTime.Month,DateTime.Day,DateTime.Hour,DateTime.Minute,DateTime.Second);

	CFRelease(Calendar);
	CFRelease(timeZone);
	if(ret==false)
		return false;

	SystemTime=CFTimeTocnSystemTime(cftime);
	return true;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
rPtr<iMutex> cnSystem::CreateMutexLock(void)
{
	return rCreate<siPOSIX::cMutexLock>();
}
//---------------------------------------------------------------------------
rPtr<iSharedMutex> cnSystem::CreateSharedLock(void)
{
	return rCreate<siPOSIX::cSharedMutex>();
}
//---------------------------------------------------------------------------
rPtr<iThreadLocalVariable> cnSystem::CreateThreadLocalVariable(void (cnLib_FUNC *Destructor)(void*))
{
	return rCreate<siPOSIX::cThreadLocalVariable>(Destructor);
}
//---------------------------------------------------------------------------
iPtr<iThread> cnSystem::StartThread(iProcedure *ThreadProcedure)
{
	return siPOSIX::cPosixThread::StartThread(ThreadProcedure);
}
//---------------------------------------------------------------------------
iPtr<iDispatch> cnSystem::CreateDispatchThread(void)
{
	auto Queue=dispatch_queue_create(DISPATCH_QUEUE_PRIORITY_DEFAULT,DISPATCH_QUEUE_SERIAL);
	auto Dispatch=iCreate<cGCDDispatch>(Queue);

	dispatch_release(Queue);
	return Dispatch;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool cnSystem::CurrentThread::SleepUntil(const iTimepoint *Time,sInt64 Delay)
{
	auto Thread=siPOSIX::cPosixThread::CurrentThread();
	if(Time!=nullptr){
		timespec tv;
		siPOSIX::iTimepoint2timespec(tv,Time,Delay);
		return Thread->SleepUntil(&tv);
	}
	else{
		return Thread->SleepUntil(nullptr);
	}
}
//---------------------------------------------------------------------------
void cnSystem::CurrentThread::SwitchThread(void)
{
	return siPOSIX::CurrentPosixThread::SwitchThread();
}
//---------------------------------------------------------------------------
iThread* cnSystem::CurrentThread::GetThread(void)
{
	return siPOSIX::cPosixThread::CurrentThread();
}
//---------------------------------------------------------------------------
bool cnSystem::CurrentThread::SetPriority(sInt8 Priority)
{
	return siPOSIX::CurrentPosixThread::SetPriority(Priority);
}
//---------------------------------------------------------------------------
sInt8 cnSystem::CurrentThread::GetPriority(void)
{
	return siPOSIX::CurrentPosixThread::GetPriority();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

// File

//---------------------------------------------------------------------------
rPtr<iSwapMemory> cnSystem::CreateSwapMemoryFromFile(iFile *File,uInt64 Size,eFileAccess AccessMode)
{
	if(File==nullptr){
		return siPOSIX::FileSystemCreateSwapMemoryFromTemporaryFile(Size);
	}
	auto POSIXFileName=iCast<iPOSIXFileName>(File);
	if(POSIXFileName!=nullptr){
		return siPOSIX::FileSystemCreateSwapMemoryFromFile(POSIXFileName->GetPOSIXFileName(),Size,AccessMode);
	}
	return nullptr;
}
//---------------------------------------------------------------------------
iPtr<iIPv4Address> cnSystem::CreateAddressIPv4(uInt32 IP,uInt16 Port)
{
	return cnMac::CreateSocketAddress_IPv4(IP,Port);
}
//---------------------------------------------------------------------------
iPtr<iSocketAddress> cnSystem::CreateSocketAddress(const sockaddr *addr,socklen_t len)
{
	return cnMac::CreateSocketAddress(addr,len);
}
//---------------------------------------------------------------------------
iPtr<iUIView> cnSystem::CreateUIView(void)
{
	auto NewOCView=[[cnLib_UIView alloc]initWithFrame:CGRectZero];
	auto NewView=cnLib_UIView_GetUIView(NewOCView);
	return iPtr<iUIView>::Take(NewView);
}
//---------------------------------------------------------------------------
rPtr<iPopupWindowControl> cnSystem::CreatePopupWindowControl(void)
{
	return nullptr;
}
//---------------------------------------------------------------------------
iUIThread* cnSystem::CurrentUIThread::GetUIThread(void)
{
	return &gMainUIThread;
}
//---------------------------------------------------------------------------
iDispatch* cnSystem::CurrentUIThread::GetDispatch(bool HighPriority)
{
	return gMainUIThread.GetDispatch(HighPriority);
}
//---------------------------------------------------------------------------
iInterface* cnSystem::GetSysMouseCursor(eSysMouseCursor CursorID)
{
	return nullptr;
}
//---------------------------------------------------------------------------
rPtr<iClipboardReader> cnSystem::QueryClipboardReader(iUIView *View)
{
	return nullptr;
}
//---------------------------------------------------------------------------
rPtr<iClipboardWriter> cnSystem::QueryClipboardWriter(iUIView *View)
{
	return nullptr;
}
//---------------------------------------------------------------------------
iPtr<iInterface> cnSystem::QueryFont(const uChar16 *Name,uIntn NameLength)
{
	return nullptr;
}
//---------------------------------------------------------------------------
rPtr<iUITextLineLayout> cnSystem::CreateTextLineLayout(const uChar16 *Text,uIntn Length,const cUITextStyle &Style)
{
	return nullptr;
}
//---------------------------------------------------------------------------
iPtr<iBitmapDataSource> cnSystem::CreateBitmapSourceFromFile(iFileName *FileName)
{
	return nullptr;
}
//---------------------------------------------------------------------------
rPtr<iUISimplePaintDevice> cnSystem::QueryUISimplePaintDevice(iUIView *ForView)
{
	return &gCGSimplePaintDevice;
}
//---------------------------------------------------------------------------
rPtr<iUISimpleViewContent> cnSystem::CreateUISimpleViewContent(void)
{
	auto ViewContent=[[cnLib_MacSimpleViewContent alloc]init];
	return rPtr<iUISimpleViewContent>::Take(cnLib_MacSimpleViewContent_Interface(ViewContent));
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
iPtr<iThreadPool> cniOS::CreateThreadPool(void)
{
	auto Queue=dispatch_queue_create(DISPATCH_QUEUE_PRIORITY_DEFAULT,DISPATCH_QUEUE_CONCURRENT);
	if(Queue==nullptr)
		return nullptr;
	auto ThreadPool=iCreate<cGCDThreadPool>(Queue);
	dispatch_release(Queue);
	return ThreadPool;
}
//---------------------------------------------------------------------------
iPtr<iOpenGLContext> cniOS::CreateOpenGLESContext(EAGLRenderingAPI API,iOpenGLContext *SharedContext)
{
	auto Sharegroup=GetEAGLSharegroup(SharedContext);
	auto NewContext=[[EAGLContext alloc]initWithAPI:API sharegroup:Sharegroup];
	if(NewContext==nil)
		return nullptr;
	return iCreate<cOpenGLESContext>(ocAttach(NewContext));
}
//---------------------------------------------------------------------------
rPtr<iOpenGLViewContent> cniOS::CreateOpenGLESViewContent(iOpenGLContext *Context)
{
	auto ViewLayer=iCreate<cOpenGLESViewLayer>();
	ViewLayer->SetGLESContext(Context);
	return ViewLayer.Pointer();
}
//---------------------------------------------------------------------------
iUIView* cniOS::iUIViewFromView(cnLib_UIView *View)
{
	return cnLib_UIView_GetUIView(View);
}
//---------------------------------------------------------------------------
iUIWindow* cniOS::iUIWindowFromViewController(cnLib_UIViewController *ViewController)
{
	return cnLib_UIViewController_GetInterface(ViewController);
}
//---------------------------------------------------------------------------
cnLib_UIView* cniOS::GetWindowClientUIView(cnLib_UIViewController *ViewController)
{
	auto Interface=cnLib_UIViewController_GetInterface(ViewController);
	return Interface->VCGetClientView();
}
//---------------------------------------------------------------------------
rPtr<iiOSUIApplication> cniOS::CreateUIApplication(void)
{
	return rCreate<ciOSUIApplication>();
}
//---------------------------------------------------------------------------
