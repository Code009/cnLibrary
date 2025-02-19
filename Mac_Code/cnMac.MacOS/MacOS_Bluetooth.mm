#include "MacOS_Bluetooth.h"
//#include <cnRTL/Memory.h>
//#include "File.h"

#include <cnRTL/cnRTL.h>
#include <cnRTL/ocRTL.h>

using namespace cnLibrary;
using namespace cnRTL;
using namespace cnMac;

//---------------------------------------------------------------------------
@interface cnLib_BluetoothRFCOMMChannelControl : NSObject <IOBluetoothRFCOMMChannelDelegate>

@end
//---------------------------------------------------------------------------
struct cnLib_BluetoothRFCOMMChannelControlVar
{
	IOBluetoothRFCOMMChannel *Channel;
	cBluetoothRFCOMMEndpointManager EPManager;
};
@implementation cnLib_BluetoothRFCOMMChannelControl
{
	ocStaticIVar<cnLib_BluetoothRFCOMMChannelControlVar> cnLib_IVar;
}

static cnLib_BluetoothRFCOMMChannelControlVar& refRFCOMMChannelControl(cnLib_BluetoothRFCOMMChannelControl *self){
	return reinterpret_cast<cnLib_BluetoothRFCOMMChannelControlVar&>(self->cnLib_IVar);
}

static iPtr<iConnectionTask> RFCOMMChannelControl_Open(cnLib_BluetoothRFCOMMChannelControl *self,IOBluetoothDevice *Device,UInt8 ChannelID)
{
	auto &Var=refRFCOMMChannelControl(self);

	auto Task=Var.EPManager.Channel_OnStartOpen();
	// retain for connect task
	[self retain];

	IOReturn ret=[Device openRFCOMMChannelAsync:&Var.Channel withChannelID:ChannelID delegate:self];

	if(ret!=kIOReturnSuccess){
		Var.EPManager.Channel_OnFinishOpen(ret);
		// release from connect task
		[self release];
		return nullptr;
	}
	return Task;
}


static void RFCOMMChannelControl_epDisconnect(cnLib_BluetoothRFCOMMChannelControl *self)
{
	auto &Var=refRFCOMMChannelControl(self);
	[Var.Channel closeChannel];
}

static void RFCOMMChannelControl_epClosed(cnLib_BluetoothRFCOMMChannelControl *self)
{
	auto &Var=refRFCOMMChannelControl(self);
	Var.Channel.delegate=nil;
	[Var.Channel release];
	Var.Channel=nil;
}

static void RFCOMMChannelControl_epSendData(cnLib_BluetoothRFCOMMChannelControl *self,const void *Buffer,uintn Size)
{
	auto &Var=refRFCOMMChannelControl(self);
	[Var.Channel writeAsync:const_cast<void*>(Buffer) length:Size refcon:nullptr];
}


-(void)rfcommChannelData:(IOBluetoothRFCOMMChannel*)rfcommChannel data:(void *)dataPointer length:(size_t)dataLength
{
	auto &Var=refRFCOMMChannelControl(self);
	if(rfcommChannel!=Var.Channel)
		return;
	Var.EPManager.Channel_OnData(dataPointer,dataLength);
}

-(void)rfcommChannelOpenComplete:(IOBluetoothRFCOMMChannel *)rfcommChannel status:(IOReturn)error
{
	auto &Var=refRFCOMMChannelControl(self);
	if(rfcommChannel!=Var.Channel)
		return;
    if(error==kIOReturnSuccess){
		[Var.Channel setSerialParameters:57600 dataBits:8 parity:kBluetoothRFCOMMParityTypeNoParity stopBits:1];
	}
	else{
	}
	Var.EPManager.Channel_OnFinishOpen(error);

	// release from connect task
	[self release];
}


-(void)rfcommChannelClosed:(IOBluetoothRFCOMMChannel *)rfcommChannel
{
	auto &Var=refRFCOMMChannelControl(self);
	if(rfcommChannel!=Var.Channel)
		return;
	Var.EPManager.Channel_OnClosed();
}

- (void)rfcommChannelControlSignalsChanged:(IOBluetoothRFCOMMChannel*)rfcommChannel
{
	NSLog(@"rfcommChannelControlSignalsChanged %d",rfcommChannel.isTransmissionPaused);
}

-(void)rfcommChannelFlowControlChanged:(IOBluetoothRFCOMMChannel *)rfcommChannel
{
	NSLog(@"rfcommChannelFlowControlChanged %d",rfcommChannel.isTransmissionPaused);
}

-(void)rfcommChannelWriteComplete:(IOBluetoothRFCOMMChannel *)rfcommChannel refcon:(void *)refcon status:(IOReturn)error
{
	auto &Var=refRFCOMMChannelControl(self);
	if(rfcommChannel!=Var.Channel)
		return;
	Var.EPManager.Channel_OnWriteCompleted();
}

-(void)rfcommChannelQueueSpaceAvailable:(IOBluetoothRFCOMMChannel *)rfcommChannel
{
	NSLog(@"rfcommChannelQueueSpaceAvailable");
}

@end

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cExtConnectionTask::cEmptyCallback cExtConnectionTask::EmptyCallback;
//---------------------------------------------------------------------------
void cExtConnectionTask::cEmptyCallback::ConnectionOnCancel(cExtConnectionTask *Task)
{
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cExtConnectionTask::cExtConnectionTask(iCallback *Callback)
{
	if(Callback!=nullptr){
		fCallback=Callback;
	}
	else{
		fCallback=&EmptyCallback;
	}
}
//---------------------------------------------------------------------------
cExtConnectionTask::~cExtConnectionTask()
{
}
//---------------------------------------------------------------------------
void cExtConnectionTask::InterfaceDelete(void)
{
	fCallback->ConnectionOnCancel(this);
	InnerDecReference('self');
}
//---------------------------------------------------------------------------
void cExtConnectionTask::ConnectStarted(void)
{
	iIncReference(this,'conn');
}
//---------------------------------------------------------------------------
void cExtConnectionTask::ConnectCompleted(iConnection *Connection)
{
	fCompleted=true;
	fConnection=Connection;
	fNotify.Notify();

	fCallback=&EmptyCallback;
	iDecReference(this,'conn');
}
//---------------------------------------------------------------------------
void cnLib_FUNC cExtConnectionTask::Cancel(void)
{
	fCallback->ConnectionOnCancel(this);
}
//---------------------------------------------------------------------------
bool cnLib_FUNC cExtConnectionTask::IsDone(void)
{
	return fCompleted;
}
//---------------------------------------------------------------------------
bool cnLib_FUNC cExtConnectionTask::SetNotify(iProcedure *NotifyProcedure)
{
	return fNotify.SetNotify(NotifyProcedure);
}
//---------------------------------------------------------------------------
iConnection*	cnLib_FUNC cExtConnectionTask::GetConnection(void)
{
	return fConnection;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void cBluetoothRFCOMMEndpointManager::ConnectionOnCancel(cExtConnectionTask *Task)
{
#pragma message ( "TODO cancel")
}
//---------------------------------------------------------------------------
void cBluetoothRFCOMMEndpointManager::OnEndpointNotifyPush(cnRTL::cExtEndpoint *Endpoint,bool IdleNotify)
{
	fPushProc.Run();
}
//---------------------------------------------------------------------------
void cBluetoothRFCOMMEndpointManager::OnEndpointNotifyPull(cnRTL::cExtEndpoint *Endpoint,bool IdleNotify)
{
	fPullProc.Run();
}
//---------------------------------------------------------------------------
void cBluetoothRFCOMMEndpointManager::OnEndpointClose(cnRTL::cExtEndpoint *Endpoint)
{
	RFCOMMChannelControl_epDisconnect(fControl);
}
//---------------------------------------------------------------------------
bool cBluetoothRFCOMMEndpointManager::cPullProcedure::HostProcedure(void)
{
	auto Host=GetObjectFromMemberPointer(this,&cBluetoothRFCOMMEndpointManager::fPullProc);
	return Host->PullProcedure();
}
//---------------------------------------------------------------------------
bool cBluetoothRFCOMMEndpointManager::PullProcedure(void)
{
	if(fEndpoint->CheckClosed()){
		EndpointEnded();
		return false;
	}
	char PullBuffer[256];
	uintn PullDataSize;
	if(fEndpoint->Pull(PullBuffer,256,PullDataSize)==false){
		return false;
	}
	RFCOMMChannelControl_epSendData(fControl,PullBuffer,PullDataSize);

	return true;
}
//---------------------------------------------------------------------------
bool cBluetoothRFCOMMEndpointManager::cPushProcedure::HostProcedure(void)
{
	auto Host=GetObjectFromMemberPointer(this,&cBluetoothRFCOMMEndpointManager::fPushProc);
	return Host->PushProcedure();
}
//---------------------------------------------------------------------------
bool cBluetoothRFCOMMEndpointManager::PushProcedure(void)
{
	if(fEndpoint->CheckClosed()){
		EndpointEnded();
		return false;
	}
	uintn PushSize;
	auto PushBuffer=fPushQueue.Peek(PushSize);
	if(PushBuffer==nullptr){
		// no data to push
		return false;
	}
	bool Pushed=fEndpoint->Push(PushBuffer,PushSize);
	if(Pushed){
		fPushQueue.Advance(PushSize);
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------
void cBluetoothRFCOMMEndpointManager::EndpointEnded(void)
{
	RFCOMMChannelControl_epClosed(fControl);
	// release for connection
	[fControl release];
}
//---------------------------------------------------------------------------
iPtr<iConnectionTask> cBluetoothRFCOMMEndpointManager::Channel_OnStartOpen(void)
{
	auto Task=iCreate<cExtConnectionTask>(static_cast<cExtConnectionTask::iCallback*>(this));
	ObjectManualConstruct(fTask,Task);
	return Task;
}
//---------------------------------------------------------------------------
void cBluetoothRFCOMMEndpointManager::Channel_OnFinishOpen(IOReturn error)
{
//	auto &Var=refRFCOMMChannelControl(fControl);

	auto Task=MoveCast(fTask);
	ObjectManualDestruct(fTask);


    if(error==kIOReturnSuccess){
		// retain for connection
		[fControl retain];
		// start connection

		// create endpoint
		auto Endpoint=iCreate<cExtEndpoint>(static_cast<cExtEndpoint::iCallback*>(this));
		ObjectManualConstruct(fEndpoint,Endpoint);

		// create connection
		auto Connection=CreateConnectionFromEndpoint(iEndpointConnection::cnInterfaceID,Endpoint,nullptr,nullptr,false);
		Task->ConnectCompleted(Connection);

	}
	else{
		// failed
	}
}
//---------------------------------------------------------------------------
void cBluetoothRFCOMMEndpointManager::Channel_OnData(const void *Data,uintn Size)
{
	fPushQueue.Append(Data,Size);
	fPushProc.Run();
}
//---------------------------------------------------------------------------
void cBluetoothRFCOMMEndpointManager::Channel_OnWriteCompleted(void)
{
	fWriteBusy=false;
	fPullProc.Run();
}
//---------------------------------------------------------------------------
void cBluetoothRFCOMMEndpointManager::Channel_OnClosed(void)
{
	fEndpoint->CloseEndpoint();
}
/*
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cBluetoothRFCOMMEndpoint::cBluetoothRFCOMMEndpoint(cnLib_BluetoothRFCOMMChannelControl *Control)
{
	fInnerRef=2;

	fControl=Control;
	[fControl retain];
	auto &Var=refRFCOMMChannelControl(fControl);
	Var.Endpoint=this;

}
//---------------------------------------------------------------------------
cBluetoothRFCOMMEndpoint::~cBluetoothRFCOMMEndpoint()
{
	RFCOMMChannelControl_epClosed(fControl);
	[fControl release];
}
//---------------------------------------------------------------------------
void cBluetoothRFCOMMEndpoint::InterfaceDelete(void)
{
	RFCOMMChannelControl_epDisconnect(fControl);
	if(fInnerRef.Free.XchgDec()==1){
		VirtualDelete();
	}
}
//---------------------------------------------------------------------------
void cBluetoothRFCOMMEndpoint::Close(void)
{
	if(ClosePush()){
		fPushProc.Run();
	}
	if(ClosePull()){
		fPullProc.Run();
	}
}
//---------------------------------------------------------------------------
void cBluetoothRFCOMMEndpoint::NotifyPull(void)
{
	fPullProc.Run();
}
//---------------------------------------------------------------------------
void cBluetoothRFCOMMEndpoint::ResumePush(void)
{
	fPushPaused=false;
	fPushProc.Run();
}
//---------------------------------------------------------------------------
void cBluetoothRFCOMMEndpoint::PausePush(void)
{
	fPushPaused=true;
}
//---------------------------------------------------------------------------
bool cBluetoothRFCOMMEndpoint::cPullProcedure::HostProcedure(void)
{
	auto Host=GetObjectFromMemberPointer(this,&cBluetoothRFCOMMEndpoint::fPullProc);
	return Host->Pull();
}
//---------------------------------------------------------------------------
bool cBluetoothRFCOMMEndpoint::Pull(void)
{
	switch(CheckPullState()){
	case StreamState::Terminate:
		if(fPullSource!=nullptr){
			fPullSource->PullTerminate();
		}
		return false;
	case StreamState::Closed:
		return false;
	case StreamState::Ready:
		break;
	}
	char PullBuffer[256];
	uintn PullDataSize;
	if(fPullSource->Pull(PullBuffer,256,PullDataSize)==false){
		return false;
	}
	RFCOMMChannelControl_epSendData(fControl,PullBuffer,PullDataSize);
	return true;
}
//---------------------------------------------------------------------------
bool cBluetoothRFCOMMEndpoint::cPushProcedure::HostProcedure(void)
{
	auto Host=GetObjectFromMemberPointer(this,&cBluetoothRFCOMMEndpoint::fPushProc);
	return Host->Push();
}
//---------------------------------------------------------------------------
bool cBluetoothRFCOMMEndpoint::Push(void)
{
	switch(CheckPushState()){
	case StreamState::Terminate:
		if(fPushProcessor!=nullptr){
			fPushProcessor->PushTerminate(true);
		}
		return false;
	case StreamState::Closed:
		return false;
	case StreamState::Ready:
		break;
	}

	if(fPushPaused){
		return false;
	}
	if(fPushProcessor==nullptr){
		return false;
	}
	uintn PushSize;
	auto PushBuffer=fPushQueue.Peek(PushSize);
	if(PushBuffer==nullptr){
		// no data to push
		return false;
	}
	bool Pushed=fPushProcessor->Push(PushBuffer,PushSize);
	if(Pushed){
		fPushQueue.Advance(PushSize);
		return true;
	}
	fPushPaused=true;
	return false;
}
//---------------------------------------------------------------------------
void cBluetoothRFCOMMEndpoint::Channel_OnData(const void *Data,uintn Size)
{
	fPushQueue.Append(Data,Size);
	fPushProc.Run();
}
//---------------------------------------------------------------------------
void cBluetoothRFCOMMEndpoint::Channel_OnWriteCompleted(void)
{
	fWriteBusy=false;
	fPullProc.Run();
}
//---------------------------------------------------------------------------
void cBluetoothRFCOMMEndpoint::Channel_OnClosed(void)
{
	Close();
	if(fInnerRef.Free.XchgDec()==1){
		VirtualDelete();
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cBluetoothRFCOMMConnectTask::cBluetoothRFCOMMConnectTask()
{
	fControl=[[cnLib_BluetoothRFCOMMChannelControl alloc]init];
	auto &Var=refRFCOMMChannelControl(fControl);
	Var.Task=this;
}
//---------------------------------------------------------------------------
cBluetoothRFCOMMConnectTask::~cBluetoothRFCOMMConnectTask()
{
	[fControl release];
}
//---------------------------------------------------------------------------
bool cBluetoothRFCOMMConnectTask::ConnectToDevice(IOBluetoothDevice *Device,UInt8 ChannelID)
{
	iIncReference(this,'conn');
	auto &Var=refRFCOMMChannelControl(fControl);
	if([Device openRFCOMMChannelAsync:&Var.Channel withChannelID:ChannelID delegate:fControl]!=kIOReturnSuccess){
		iDecReference(this,'conn');
		return false;
	}
	return true;
}
//---------------------------------------------------------------------------
void cBluetoothRFCOMMConnectTask::Channel_OnOpenCompleted(IOReturn error)
{
	auto &Var=refRFCOMMChannelControl(fControl);

	Var.Task=nullptr;


    if(error==kIOReturnSuccess){
		auto Endpoint=iCreate<cExtEndpoint>(0);
		//auto Endpoint=iCreate<cBluetoothRFCOMMEndpoint>(fControl);
		fConnection=CreateConnectionFromEndpoint(iEndpointConnection::IID,Endpoint,nullptr,nullptr,false);
	}
	else{
		// failed
	}
	fCompleted=true;
	fNotify.Notify();

	iDecReference(this,'conn');
}
//---------------------------------------------------------------------------
void cnLib_FUNC cBluetoothRFCOMMConnectTask::Cancel(void)
{
}
//---------------------------------------------------------------------------
bool cnLib_FUNC cBluetoothRFCOMMConnectTask::IsDone(void)
{
	return fCompleted;
}
//---------------------------------------------------------------------------
bool cnLib_FUNC cBluetoothRFCOMMConnectTask::SetNotify(iProcedure *NotifyProcedure)
{
	return fNotify.SetNotify(NotifyProcedure);
}
//---------------------------------------------------------------------------
iConnection*	cnLib_FUNC cBluetoothRFCOMMConnectTask::GetConnection(void)
{
	return fConnection;
}
*/
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
iPtr<iConnectionTask> cnMac::BluetoothRFCOMMConnect(IOBluetoothDevice *Device,UInt8 ChannelID)
{
	auto Control=[[cnLib_BluetoothRFCOMMChannelControl alloc]init];

	auto Task=RFCOMMChannelControl_Open(Control,Device,ChannelID);

	[Control release];
	return Task;
}
//---------------------------------------------------------------------------

#if	0

// system bt select dialog
- (IBAction) discover:(id)sender
{
    IOBluetoothDeviceSelectorController *deviceSelector;
    IOBluetoothSDPUUID *sppServiceUUID;
    NSArray *deviceArray;

    NSLog(@"Attempting to connect\n");

    // The device selector will provide UI to the end user to find a remote device
    deviceSelector = [IOBluetoothDeviceSelectorController deviceSelector];

    if ( deviceSelector == nil ) {
        [self log: @"Error - unable to allocate IOBluetoothDeviceSelectorController.\n" ];
        return;
    }

    sppServiceUUID = [IOBluetoothSDPUUID uuid16:kBluetoothSDPUUID16ServiceClassSerialPort];
    [deviceSelector addAllowedUUID:sppServiceUUID];
    if ( [deviceSelector runModal] != kIOBluetoothUISuccess ) {
        [self log: @"User has cancelled the device selection.\n" ];
        return;
    }
	deviceArray = [deviceSelector getResults];
	if ( ( deviceArray == nil ) || ( [deviceArray count] == 0 ) ) {
    	[self log: @"Error - no selected device.  ***This should never happen.***\n" ];
	    return;
	}
	IOBluetoothDevice *device = [deviceArray objectAtIndex:0];
	IOBluetoothSDPServiceRecord     *sppServiceRecord = [device getServiceRecordForUUID:sppServiceUUID];
	if ( sppServiceRecord == nil ) {
	    [self log: @"Error - no spp service in selected device.  ***This should never happen since the selector forces the user to select only devices with spp.***\n" ];
    	return;
	}
	// To connect we need a device to connect and an RFCOMM channel ID to open on the device:
	UInt8   rfcommChannelID;
	if ( [sppServiceRecord getRFCOMMChannelID:&rfcommChannelID] != kIOReturnSuccess ) {
	    [self log: @"Error - no spp service in selected device.  ***This should never happen an spp service must have an rfcomm channel id.***\n" ];
    	return;
	}


	auto Task=cnMacOS::BluetoothRFCOMMConnect(device, rfcommChannelID);

	gApp.OnConnectDevice(Task);

}
#endif
