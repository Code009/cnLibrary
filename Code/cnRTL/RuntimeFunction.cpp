//---------------------------------------------------------------------------
#include "RuntimeFunction.h"

using namespace cnLibrary;
using namespace cnRTL;


//---------------------------------------------------------------------------
namespace cnLibrary{
namespace cnRTL{
//---------------------------------------------------------------------------
static const uInt8 BitReverse_Map[256]={
	0x00,0x80,0x40,0xC0,0x20,0xA0,0x60,0xE0,0x10,0x90,0x50,0xD0,0x30,0xB0,0x70,0xF0,
	0x08,0x88,0x48,0xC8,0x28,0xA8,0x68,0xE8,0x18,0x98,0x58,0xD8,0x38,0xB8,0x78,0xF8,
	0x04,0x84,0x44,0xC4,0x24,0xA4,0x64,0xE4,0x14,0x94,0x54,0xD4,0x34,0xB4,0x74,0xF4,
	0x0C,0x8C,0x4C,0xCC,0x2C,0xAC,0x6C,0xEC,0x1C,0x9C,0x5C,0xDC,0x3C,0xBC,0x7C,0xFC,
	0x02,0x82,0x42,0xC2,0x22,0xA2,0x62,0xE2,0x12,0x92,0x52,0xD2,0x32,0xB2,0x72,0xF2,
	0x0A,0x8A,0x4A,0xCA,0x2A,0xAA,0x6A,0xEA,0x1A,0x9A,0x5A,0xDA,0x3A,0xBA,0x7A,0xFA,
	0x06,0x86,0x46,0xC6,0x26,0xA6,0x66,0xE6,0x16,0x96,0x56,0xD6,0x36,0xB6,0x76,0xF6,
	0x0E,0x8E,0x4E,0xCE,0x2E,0xAE,0x6E,0xEE,0x1E,0x9E,0x5E,0xDE,0x3E,0xBE,0x7E,0xFE,
	0x01,0x81,0x41,0xC1,0x21,0xA1,0x61,0xE1,0x11,0x91,0x51,0xD1,0x31,0xB1,0x71,0xF1,
	0x09,0x89,0x49,0xC9,0x29,0xA9,0x69,0xE9,0x19,0x99,0x59,0xD9,0x39,0xB9,0x79,0xF9,
	0x05,0x85,0x45,0xC5,0x25,0xA5,0x65,0xE5,0x15,0x95,0x55,0xD5,0x35,0xB5,0x75,0xF5,
	0x0D,0x8D,0x4D,0xCD,0x2D,0xAD,0x6D,0xED,0x1D,0x9D,0x5D,0xDD,0x3D,0xBD,0x7D,0xFD,
	0x03,0x83,0x43,0xC3,0x23,0xA3,0x63,0xE3,0x13,0x93,0x53,0xD3,0x33,0xB3,0x73,0xF3,
	0x0B,0x8B,0x4B,0xCB,0x2B,0xAB,0x6B,0xEB,0x1B,0x9B,0x5B,0xDB,0x3B,0xBB,0x7B,0xFB,
	0x07,0x87,0x47,0xC7,0x27,0xA7,0x67,0xE7,0x17,0x97,0x57,0xD7,0x37,0xB7,0x77,0xF7,
	0x0F,0x8F,0x4F,0xCF,0x2F,0xAF,0x6F,0xEF,0x1F,0x9F,0x5F,0xDF,0x3F,0xBF,0x7F,0xFF,
};
//---------------------------------------------------------------------------
}	// namespace cnRTL
}	// namespace cnLibrary
//---------------------------------------------------------------------------
uInt8 cnRTL::BitReverse(uInt8 Src)noexcept(true)
{
	return BitReverse_Map[Src];
}

//---------------------------------------------------------------------------
const cVectorZeroValue cnRTL::VectorZeroValue;
cnVar::cStaticVariable<cRTLDebuggerContext> cnRTL::gRTLDebuggerContext;
cnVar::cStaticVariable<cReferenceCountLogger> cnRTL::gStaticReferenceCountLogger;
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cSystemTime::cSystemTime(iTimepoint *RefTime)noexcept(true)
{
	if(RefTime==nullptr){
		fNanoSeconds=0;
	}
	else{
		fNanoSeconds=RefTime->SystemTime();
	}
}
//---------------------------------------------------------------------------
cSystemTime::~cSystemTime()noexcept(true)
{
}
//---------------------------------------------------------------------------
cSystemTime::operator iTimepoint*()const noexcept(true)
{
	return const_cast<cSystemTime*>(this);
}
//---------------------------------------------------------------------------
sInt64 cSystemTime::SystemTime(void)noexcept(true)
{
	return fNanoSeconds;
}
//---------------------------------------------------------------------------
sInt64 cSystemTime::Since(iTimepoint *Time)const noexcept(true)
{
	sInt64 TimeNS=Time->SystemTime();
	sInt64 delta=fNanoSeconds-TimeNS;
	return delta;
}
//---------------------------------------------------------------------------
sInt64 cSystemTime::SinceTime(iTimepoint *Time)noexcept(true)
{
	return Since(Time);
}
//---------------------------------------------------------------------------
bool cSystemTime::operator == (const cSystemTime &Src)const noexcept(true)
{
	return fNanoSeconds==Src.fNanoSeconds;
}
//---------------------------------------------------------------------------
bool cSystemTime::operator != (const cSystemTime &Src)const noexcept(true)
{
	return fNanoSeconds!=Src.fNanoSeconds;
}
//---------------------------------------------------------------------------
bool cSystemTime::operator < (const cSystemTime &Src)const noexcept(true)
{
	return fNanoSeconds<Src.fNanoSeconds;
}
//---------------------------------------------------------------------------
bool cSystemTime::operator <= (const cSystemTime &Src)const noexcept(true)
{
	return fNanoSeconds<=Src.fNanoSeconds;
}
//---------------------------------------------------------------------------
bool cSystemTime::operator > (const cSystemTime &Src)const noexcept(true)
{
	return fNanoSeconds>Src.fNanoSeconds;
}
//---------------------------------------------------------------------------
bool cSystemTime::operator >= (const cSystemTime &Src)const noexcept(true)
{
	return fNanoSeconds>=Src.fNanoSeconds;
}
//---------------------------------------------------------------------------
bool cSystemTime::operator == (iTimepoint *Src)const noexcept(true)
{
	return Since(Src)==0;
}
//---------------------------------------------------------------------------
bool cSystemTime::operator != (iTimepoint *Src)const noexcept(true)
{
	return Since(Src)!=0;
}
//---------------------------------------------------------------------------
bool cSystemTime::operator < (iTimepoint *Src)const noexcept(true)
{
	return Since(Src)<0;
}
//---------------------------------------------------------------------------
bool cSystemTime::operator <= (iTimepoint *Src)const noexcept(true)
{
	return Since(Src)<=0;
}
//---------------------------------------------------------------------------
bool cSystemTime::operator > (iTimepoint *Src)const noexcept(true)
{
	return Since(Src)>0;
}
//---------------------------------------------------------------------------
bool cSystemTime::operator >= (iTimepoint *Src)const noexcept(true)
{
	return Since(Src)>=0;
}
//---------------------------------------------------------------------------
cSystemTime& cSystemTime::operator =(iTimepoint *RefTime)noexcept(true)
{
	if(RefTime==nullptr){
		fNanoSeconds=0;
	}
	else if(this!=RefTime){
		fNanoSeconds=RefTime->SystemTime();
	}
	return *this;
}
//---------------------------------------------------------------------------
cSystemTime cSystemTime::operator + (sInt64 Src)const noexcept(true)
{
	return cSystemTime{fNanoSeconds+Src};
}
//---------------------------------------------------------------------------
cSystemTime cSystemTime::operator - (sInt64 Src)const noexcept(true)
{
	return cSystemTime{fNanoSeconds-Src};
}
//---------------------------------------------------------------------------
sInt64 cSystemTime::operator - (const cSystemTime &Relative)const noexcept(true)
{
	return fNanoSeconds-Relative.fNanoSeconds;
}
//---------------------------------------------------------------------------
sInt64 cSystemTime::operator - (iTimepoint *Relative)const noexcept(true)
{
	if(Relative==nullptr)
		return fNanoSeconds;
	sInt64 Src=Relative->SystemTime();
	return fNanoSeconds-Src;
}
//---------------------------------------------------------------------------
cSystemTime& cSystemTime::operator +=(sInt64 Src)noexcept(true)
{
	fNanoSeconds+=Src;
	return *this;
}
//---------------------------------------------------------------------------
cSystemTime& cSystemTime::operator -=(sInt64 Src)noexcept(true)
{
	fNanoSeconds-=Src;
	return *this;
}
//---------------------------------------------------------------------------
sInt64 cSystemTime::SinceUnixEpoch(void)const noexcept(true)
{
	sInt64 EpochDelta=Time_1s*cnSystem::SystemTimeEpochSecondsSinceUnixEpoch;
	return EpochDelta+fNanoSeconds;
}
//---------------------------------------------------------------------------
void cSystemTime::SetTimeUnix(sInt64 NanoSeconds)noexcept(true)
{
	sInt64 EpochDelta=Time_1s*cnSystem::SystemTimeEpochSecondsSinceUnixEpoch;
	fNanoSeconds=NanoSeconds-EpochDelta;
}
//---------------------------------------------------------------------------
void cSystemTime::SetTimeNow(void)noexcept(true)
{
	fNanoSeconds=cnSystem::GetSystemTimeNow();
}
//---------------------------------------------------------------------------
void cSystemTime::SetSystemTime(sInt64 SystemTime)noexcept(true)
{
	fNanoSeconds=SystemTime;
}
//---------------------------------------------------------------------------
void cSystemTime::Truncate(uInt64 Mod)noexcept(true)
{
	fNanoSeconds-=fNanoSeconds%Mod;
}
//---------------------------------------------------------------------------
cSystemTime cSystemTime::TimeNow(void)noexcept(true)
{
	return cSystemTime(cnSystem::GetSystemTimeNow());
}
//---------------------------------------------------------------------------
sInt64 cnRTL::operator - (iTimepoint *Dest,const cSystemTime &Src)noexcept(true)
{
	return -Src.Since(Dest);
}
//---------------------------------------------------------------------------
bool cnRTL::operator == (iTimepoint *Dest,const cSystemTime &Src)noexcept(true)
{
	return Src.operator ==(Dest);
}
bool cnRTL::operator != (iTimepoint *Dest,const cSystemTime &Src)noexcept(true)
{
	return Src.operator !=(Dest);
}
bool cnRTL::operator < (iTimepoint *Dest,const cSystemTime &Src)noexcept(true)
{
	return Src>Dest;
}
bool cnRTL::operator <= (iTimepoint *Dest,const cSystemTime &Src)noexcept(true)
{
	return Src>=Dest;
}
bool cnRTL::operator > (iTimepoint *Dest,const cSystemTime &Src)noexcept(true)
{
	return Src<Dest;
}
bool cnRTL::operator >= (iTimepoint *Dest,const cSystemTime &Src)noexcept(true)
{
	return Src<=Dest;
}
//---------------------------------------------------------------------------
uInt64 cnRTL::SystemTimeToUnixTimeNS(uInt64 SystemTime)noexcept(true)
{
	sInt64 EpochDelta=Time_1s*cnSystem::SystemTimeEpochSecondsSinceUnixEpoch;
	SystemTime+=EpochDelta;
	return SystemTime;
}
//---------------------------------------------------------------------------
uInt64 cnRTL::SystemTimeFromUnixTimeNS(uInt64 UnixTime)noexcept(true)
{
	sInt64 EpochDelta=Time_1s*cnSystem::SystemTimeEpochSecondsSinceUnixEpoch;
	return UnixTime-EpochDelta;
}
//---------------------------------------------------------------------------
uInt64 cnRTL::TimeNSTruncate(uInt64 TimeNS,uInt64 Mod)noexcept(true)
{
	TimeNS-=TimeNS%Mod;
	return TimeNS;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//constexpr uInt8 cExclusiveFlag::rfIdle;
//constexpr uInt8 cExclusiveFlag::rfExecute;
//constexpr uInt8 cExclusiveFlag::rfPending;
//---------------------------------------------------------------------------
bool cExclusiveFlag::IsRunning(void)const noexcept(true)
{
	return RunFlag!=rfIdle;
}
//---------------------------------------------------------------------------
void cExclusiveFlag::Reset(void)noexcept(true)
{
	RunFlag=rfIdle;
}
//---------------------------------------------------------------------------
bool cExclusiveFlag::Acquire(void)noexcept(true)
{
	auto PrevRunFlag=RunFlag.Release<<=rfPending;
	switch(PrevRunFlag){
	case rfIdle:
		// new start
		return true;
	case rfExecute:
		// is running
		return false;
	default:
		cnLib_UNEXPECTED_BRANCH;
		cnLib_SWITCH_FALLTHROUGH;
	case rfPending:
		// already requested to run
		return false;
	}
}
//---------------------------------------------------------------------------
bool cExclusiveFlag::Release(void)noexcept(true)
{
	// decrease state
	auto NextRunFlag=RunFlag.Release-=1;
	switch(NextRunFlag){
	default:
		cnLib_UNEXPECTED_BRANCH;
		cnLib_SWITCH_FALLTHROUGH;
	case rfIdle:
		// successfully stoped
		return true;
	case rfExecute:
		// requested to run, continue running
		return false;
	}
}
//---------------------------------------------------------------------------
void cExclusiveFlag::Continue(void)noexcept(true)
{
	RunFlag.Release=rfExecute;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void cResourceAvailableFlag::Start(void)noexcept(true)
{
	AvailableFlag.Free.Store(rfRunning);
}
//---------------------------------------------------------------------------
void cResourceAvailableFlag::Pause(void)noexcept(true)
{
	AvailableFlag.Free.Store(rfPending);
}
//---------------------------------------------------------------------------
bool cResourceAvailableFlag::Finish(void)noexcept(true)
{
	return AvailableFlag.Barrier.CmpStore(rfRunning,rfIdle);
}
//---------------------------------------------------------------------------
bool cResourceAvailableFlag::MarkAvailable(void)noexcept(true)
{
	ufInt8 LastFlag=AvailableFlag.Barrier.Xchg(rfPending);
	return LastFlag==rfIdle;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void cRTLDebuggerContext::DebugStartThread(void)noexcept(true)
{
	cnClass::ManualConstruct(fDebugThreadProcedure);
	cnSystem::StartThread(&fDebugThreadProcedure);
}
//---------------------------------------------------------------------------
void cRTLDebuggerContext::cDebugThreadProcedure::Execute(void)noexcept(true)
{
	auto Host=cnMemory::GetObjectFromMemberPointer(this,&cRTLDebuggerContext::fDebugThreadProcedure);
	Host->DebugRunThread();
}
//---------------------------------------------------------------------------
void cRTLDebuggerContext::DebugRunThread(void)noexcept(true)
{
	if(fDebugThreadRefCount!=0){
		// already running
		return;
	}

	TKRuntime::SystemThread::tSingleNotification Notification;

	Notification.Setup();
	fDebugThreadShutdownAvailable=true;
	fDebugThreadNotification=&Notification;

	if(fDebugThreadRefCount.Barrier.CmpStore(0,1)){
		do{
			if(fDebugThreadNotification!=&Notification)
				break;
			auto DebugItem=fDebugProcQueue.DequeueAll();
			while(DebugItem!=nullptr){
				auto CurDebugItem=DebugItem;
				DebugItem=DebugItem->Next;

				CurDebugItem->Procedure->Execute();

				delete CurDebugItem;
			}

			Notification.Wait();
		}while(fDebugThreadRefCount.Acquire.Load()!=0);
	}

	Notification.Clear();
}
//---------------------------------------------------------------------------
void cRTLDebuggerContext::DebugShutdownThread(void)noexcept(true)
{
	if(fDebugThreadShutdownAvailable.Barrier.Xchg(false)==false){
		// already shutdown
		return;
	}
	FinishDebugThread();
}
//---------------------------------------------------------------------------
bool cRTLDebuggerContext::QueryDebugThread(void)noexcept(true)
{
	uIntn CurRefCount=fDebugThreadRefCount;
	while(CurRefCount!=0){
		if(fDebugThreadRefCount.Barrier.CmpXchg(CurRefCount,CurRefCount+1)){
			return true;
		}
	}
	return false;
}
//---------------------------------------------------------------------------
void cRTLDebuggerContext::FinishDebugThread(void)noexcept(true)
{
	if(--fDebugThreadRefCount.Barrier==0){
		
		auto Notification=fDebugThreadNotification;
		fDebugThreadNotification=nullptr;

		Notification->Notify();
	}
}
//---------------------------------------------------------------------------
void cRTLDebuggerContext::DebuggerExecute(iProcedure *Procedure)noexcept(true)
{
	if(QueryDebugThread()){
		auto DebugItem=new cDebugProcItem;
		DebugItem->Procedure=Procedure;
		fDebugProcQueue.Enqueue(DebugItem);

		fDebugThreadNotification->Notify();
		FinishDebugThread();
	}
	else{
		Procedure->Execute();
	}

}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void cReferenceCountLogger::Log(void *Object,uInt32 Tag,bool Inc)noexcept(true)
{
	auto Item=fRecycleStack.Pop();
	if(Item==nullptr){
		Item=new cItem;
	}
	Item->Object=Object;
	Item->Tag=Tag;
	Item->Inc=Inc;
	fItemQueue.Enqueue(Item);
	NotifyProcess();
}
//---------------------------------------------------------------------------
void cReferenceCountLogger::cContext::Inc(void *Object,uInt32 Tag)noexcept(true)
{
	auto &ObjectItem=ObjectMap[Object];
	ObjectItem[Tag]++;
}
//---------------------------------------------------------------------------
void cReferenceCountLogger::cContext::Dec(void *Object,uInt32 Tag)noexcept(true)
{
	auto ObjectPair=ObjectMap.GetPair(Object);
	if(ObjectPair==nullptr){
		// error! object not found
		cnSystem::AssertionMessage("object not found");
	}
	else{
		auto TagPair=ObjectPair->Value.GetPair(Tag);
		if(TagPair==nullptr){
			// error! tag not found
			cnSystem::AssertionMessage("tag not found");
		}
		else{
			if(--TagPair->Value==0){
				ObjectPair->Value.RemovePair(TagPair);
				if(ObjectPair->Value.GetCount()==0){
					ObjectMap.RemovePair(ObjectPair);
				}
			}
		}
	}
}
//---------------------------------------------------------------------------
void cReferenceCountLogger::Process(void)noexcept(true)
{
	auto Items=fItemQueue.DequeueAll();
	if(fSystemShutdown==0){
		while(Items!=nullptr){
			auto CurItem=Items;
			Items=Items->Next;
			if(CurItem->Inc){
				fContext->Inc(CurItem->Object,CurItem->Tag);
			}
			else{
				fContext->Dec(CurItem->Object,CurItem->Tag);
			}
			fRecycleStack.Push(CurItem);
		}
	}
	else{
		while(Items!=nullptr){
			auto CurItem=Items;
			Items=Items->Next;
			if(CurItem->Inc){
				fContext->Inc(CurItem->Object,CurItem->Tag);
			}
			else{
				fContext->Dec(CurItem->Object,CurItem->Tag);
			}
			delete CurItem;
		}
		if(fSystemShutdown==1){
			fSystemShutdown=2;

			Items=fRecycleStack.Swap(nullptr);
			while(Items!=nullptr){
				auto CurItem=Items;
				Items=Items->Next;
				delete CurItem;
			}

		}
	}
}
//---------------------------------------------------------------------------
void cReferenceCountLogger::ThreadProcess(void)noexcept(true)
{
	do{
		fExclusiveFlag.Continue();

		Process();

	}while(fExclusiveFlag.Release()==false);
}
//---------------------------------------------------------------------------
void cReferenceCountLogger::cContext::cDebugThreadProc::Execute(void)noexcept(true)
{
	auto Context=cnMemory::GetObjectFromMemberPointer(this,&cContext::DebugThreadProc);
	auto Host=cnMemory::GetObjectFromMemberPointer(Context,&cReferenceCountLogger::fContext);
	Host->ThreadProcess();
}
//---------------------------------------------------------------------------
void cReferenceCountLogger::NotifyProcess(void)noexcept(true)
{
	if(fExclusiveFlag.Acquire()==false)
		return;

	if(fContextInitialized==false){
		fContext.Construct();
		fContextInitialized=true;
	}

	gRTLDebuggerContext->DebuggerExecute(&fContext->DebugThreadProc);
}
//---------------------------------------------------------------------------
