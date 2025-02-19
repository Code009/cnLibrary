//
//  MacOS.mm
//  cnSystem
//
//  Created by Code009 on 16-6-27.
//  Copyright (c) 2016年 __MyCompanyName__. All rights reserved.
//
#define OS_OBJECT_USE_OBJC 0

#include <cnMac.MacOS/MacOS.h>

using namespace cnLibrary;
using namespace cnRTL;
using namespace cnMac;



cnLib_INTERFACE_LOCALID_DEFINE(iGCDDispatch);
cnLib_INTERFACE_LOCALID_DEFINE(iOCObject);
cnLib_INTERFACE_LOCALID_DEFINE(iCGContextDrawable);
cnLib_INTERFACE_LOCALID_DEFINE(iCFCGImage);
cnLib_INTERFACE_LOCALID_DEFINE(iUIViewCALayerHost);


static cReferenceStaticImplementT<cMAllocHeap> DefaultMallocHeap(malloc_default_zone());

#if	0
class cDebugLogStream : public iLogStream
{
private:
	uint8 fLogLevel=255;
public:
	cDebugLogStream();
	void SetLogLevel(uint8 Level);
	virtual void cnLib_FUNC  Log(iLogContent *Content)override;
};

cDebugLogStream::cDebugLogStream()
{
}

void cDebugLogStream::SetLogLevel(uint8 Level)
{
	fLogLevel=Level;
}

void cnLib_FUNC cDebugLogStream::Log(iLogContent *Content)
{
	/*cStringBuffer<char> ConvertBuffer;
	uintn LogLength;
	auto LogStr=Content->GetText(LogLength);
	ConvertBuffer.AppendConverted(Converter,LogStr,LogLength);
	write(STDOUT_FILENO,ConvertBuffer.CStr(),ConvertBuffer.GetLength());
	*/
	auto Level=Content->GetLevel();
	if(fLogLevel<Level)
		return;

	auto LogStr=Content->GetText();
	uIntn LogLength=Content->GetTextLength();
	auto Str=[[NSString alloc]initWithCharacters:(const UniChar*)LogStr length:LogLength];
	NSLog(@"%@",Str);
	[Str release];
}


//static cReferenceLog gDebugReferenceLog;
static cInterfaceStaticImplementT<cDebugLogStream> gDebugLogStream;

#endif

// data

const sint64 cnSystem::SystemTimeEpochSecondsSinceUnixEpoch=0;

// thread

static cReferenceStaticImplementT<siPOSIX::cMutexLock> gGlobalMutex;

siPOSIX::cPosixThread::cModule siPOSIX::cPosixThread::gModule;

static cInterfaceStaticImplementT<cGCDThreadPool> gDefaultGDCThreadPool(GCDDefaultQueue());

// File

cAIOTaskBlockGCDManager cnMac::gAIOTaskBlockGCDManager;

// network
//static cnRTL::cInterfaceStaticImplementT<cSocketConnectionProtocol> gTCP(SOCK_STREAM,IPPROTO_TCP);
//static cnRTL::cInterfaceStaticImplementT<cSocketDatagramProtocol> gUDP(SOCK_DGRAM,IPPROTO_UDP);
//iConnectionProtocol*const	cnSystem::TCP=&gTCP;
//iDatagramProtocol*const		cnSystem::UDP=&gUDP;

// UI
cMacOSMainUIThread cnMac::gMainUIThread;

cnRTL::cReferenceStaticImplementT<cCGSimplePaintDevice> cnMac::gCGSimplePaintDevice;


//---------------------------------------------------------------------------
iMemoryHeap*const		cnSystem::DefaultMemoryHeap=&DefaultMallocHeap;
//---------------------------------------------------------------------------
iMutex*const			cnSystem::GlobalMutex=&gGlobalMutex;
//---------------------------------------------------------------------------
iThreadPool*const		cnSystem::ThreadPool=&gDefaultGDCThreadPool;



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
			auto StrIndex=AppPath->Length;
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
			auto StrIndex=DataPath->Length;
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
		Doc=DataPath.Storage();

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

// cnRTL

//---------------------------------------------------------------------------
iFileName*const			cnMacOS::Folder_Application=&gFolder_App;
iFileName*const			cnMacOS::Folder_Temp=&gFolder_Tmp;
iFileName*const			cnMacOS::Folder_Document=&gFolder_Doc;

//static cnRTL::cInterfaceStaticImplementT<cSocketConnectionProtocol> gTCP(IPPROTO_TCP);
//static cnRTL::cInterfaceStaticImplementT<cSocketDatagramProtocol> gUDP(IPPROTO_UDP);
//iConnectionProtocol*const	cnSystem::TCP=&gTCP;
//iDatagramProtocol*const		cnSystem::UDP=&gUDP;


static_assert(sizeof(uChar16)==sizeof(UniChar),"incorrect char size");

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
		ToEncoding="UTF-16LE";
		break;
	case 4:
		ToEncoding="UTF-32LE";
		break;
	}
	switch(FromEncodingSize){
	default:
		return nullptr;
	case 1:
		FromEncoding="UTF-8";
		break;
	case 2:
		FromEncoding="UTF-16LE";
		break;
	case 4:
		FromEncoding="UTF-32LE";
		break;
	}
	auto conv=iconv_open(ToEncoding,FromEncoding);
	if(conv==reinterpret_cast<iconv_t>(-1))
		return nullptr;
	return rCreate<siPOSIX::cICONVTextEncodingConverter>(conv);
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

// File

//---------------------------------------------------------------------------
#if	0
rPtr<iFileEnumerator> cnSystem::FileSystem::Enum(iFileName *FolderName,const uChar16 *Filter)
{
	auto MacFileName=iCast<iMacFileName>(FolderName);
	if(MacFileName==nullptr)
		return nullptr;
	return cnMac::FileSystemEnum(MacFileName->GetMacFileName(),MacFileName->GetMacFileNameLength(),Filter);
}
//---------------------------------------------------------------------------
iPtr<iSyncStream> cnSystem::FileSystem::OpenSequentialStream(iFileName *FileName,eFileAccess AccessMode,eFileCreate CreateFlag)
{
	auto MacFileName=iCast<iMacFileName>(FileName);
	if(MacFileName==nullptr)
		return nullptr;
	return siPOSIX::FileSystemOpenSeqStream(MacFileName->GetMacFileName(),AccessMode,CreateFlag);
}
//---------------------------------------------------------------------------
iPtr<iFileStream> cnSystem::FileSystem::OpenFileStream(iFileName *FileName,eFileAccess AccessMode,eFileCreate CreateFlag)
{
	auto MacFileName=iCast<iMacFileName>(FileName);
	if(MacFileName==nullptr)
		return nullptr;

	return nullptr;
	//return RetFromPtr<iFileStream>(cnMac::FileSystemOpenFileStream(MacFileName->GetMacFileName(),AccessMode,CreateFlag));
}
//---------------------------------------------------------------------------
bool cnSystem::FileSystem::IsFileExists(iFileName *FileName)
{
	auto MacFileName=iCast<iMacFileName>(FileName);
	if(MacFileName==nullptr)
		return false;
	return siPOSIX::FileSystemIsFileExists(MacFileName->GetMacFileName());
}
//---------------------------------------------------------------------------
uInt64 cnSystem::FileSystem::GetFileSize(iFileName *FileName)
{
	auto MacFileName=iCast<iMacFileName>(FileName);
	if(MacFileName==nullptr)
		return 0;
	return siPOSIX::FileSystemGetFileSize(MacFileName->GetMacFileName());
}
#endif
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
	auto NewOCView=[[cnLib_NSView alloc]initWithFrame:NSZeroRect];
	return iPtr<iUIView>::TakeFromManual(cnLib_NSView_GetUIView(NewOCView));
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
	auto Image=cnMac::LoadImageFromFile(FileName);
	if(Image==nullptr)
		return nullptr;
	auto Souce=iCreate<cBitmapSourceFromCGImage>(Image);
	CGImageRelease(Image);
	return Souce;
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
	return rPtr<iUISimpleViewContent>::TakeFromManual(cnLib_MacSimpleViewContent_Interface(ViewContent));
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//iPtr<iThreadPool> cnMacOS::CreateThreadPool(void)

//---------------------------------------------------------------------------
rPtr<iTextEncodingConverter> cnMacOS::TextEncodingConverter(const char *ToEncoding,const char *FromEncoding)
{
	auto conv=iconv_open(ToEncoding,FromEncoding);
	if(conv==reinterpret_cast<iconv_t>(-1))
		return nullptr;
	return rCreate<siPOSIX::cICONVTextEncodingConverter>(conv);
}
//---------------------------------------------------------------------------
iUIView* cnMacOS::iUIViewFromView(cnLib_NSView *View)
{
	return cnMac::cnLib_NSView_GetUIView(View);
}
//---------------------------------------------------------------------------
iPtr<iUIWindow> cnMacOS::CreateUIWindow(void)
{
	NSWindow *NewWindow=[[NSWindow alloc]init];
	auto Window=iCreate<cUIWindow>(NewWindow);
	[NewWindow release];
	return Window;
}
//---------------------------------------------------------------------------
iPtr<iUIWindow> cnMacOS::CreateUIWindow(NSWindow *Window)
{
	return iCreate<cUIWindow>(Window);
}
//---------------------------------------------------------------------------
iPtr<iOpenGLContext> cnMacOS::CreateOpenGLContext(CGLPixelFormatObj PixelFormatObject,iOpenGLContext *SharedContext)
{
	return cnMac::CreateOpenGLContext(PixelFormatObject,SharedContext);
}
//---------------------------------------------------------------------------
rPtr<iOpenGLViewContent> cnMacOS::CreateOpenGLViewContent(iOpenGLContext *SharedContext)
{
	auto Layer=[[cnLib_OpenGLLayer alloc]init];
	auto *CPP=cnMac::ViewContentFromOpenGLViewLayer(Layer);
	CPP->SharedContext=iCast<cOpenGLContext>(SharedContext);

	return rPtr<iOpenGLViewContent>::TakeFromManual(CPP);
}
//---------------------------------------------------------------------------
rPtr<iMacOSUIApplication> cnMacOS::CreateUIApplication(void)
{
	return rCreate<cnMac::cMacOSUIApplication>();
}
//---------------------------------------------------------------------------
iPtr<iConnectionTask> cnMacOS::BluetoothRFCOMMConnect(IOBluetoothDevice *Device,UInt8 ChannelID)
{
	//return cnMac::BluetoothRFCOMMConnect(Device,ChannelID);
	return nullptr;
}
//---------------------------------------------------------------------------
