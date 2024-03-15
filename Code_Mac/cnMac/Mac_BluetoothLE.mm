#import "Mac_BluetoothLE.h"

using namespace cnLibrary;
using namespace cnRTL;
using namespace cnMac;

//---------------------------------------------------------------------------

@interface cnLib_BluetoothLEPeripheralContext()<CBPeripheralDelegate>
@end

@implementation cnLib_BluetoothLEPeripheralContext
{
@private
	cnRTL::ocStaticIVar<cBLEPeripheral> cnLib_IVar;
}

static cBLEPeripheral* CPPBluetoothLEPeripheralContext(cnLib_BluetoothLEPeripheralContext *self)
{
	return reinterpret_cast<cBLEPeripheral*>(&self->cnLib_IVar);
}

-(instancetype)init
{
	self=[super init];
	if(self){
		cnVar::ManualConstruct(*CPPBluetoothLEPeripheralContext(self),self);
	}
	return self;
}

-(void)dealloc
{
	cnVar::ManualDestruct(*CPPBluetoothLEPeripheralContext(self));

	[super dealloc];
}

#pragma mark - peripheral delegate

//-(void)peripheralDidUpdateName:(CBPeripheral *)peripheral NS_AVAILABLE(10_9, 6_0)
//{
//}

//-(void)peripheral:(CBPeripheral *)peripheral didModifyServices:(NSArray<CBService *> *)invalidatedServices NS_AVAILABLE(10_9, 7_0)
//{
//}

//-(void)peripheral:(CBPeripheral *)peripheral didReadRSSI:(NSNumber *)RSSI error:(nullable NSError *)error NS_AVAILABLE(10_13, 8_0)
//{
//}

-(void)peripheral:(CBPeripheral *)peripheral didDiscoverServices:(nullable NSError *)error
{
	auto CPP=CPPBluetoothLEPeripheralContext(self);
	CPP->PeripheralDidDiscoverService(error);
}

//-(void)peripheral:(CBPeripheral *)peripheral didDiscoverIncludedServicesForService:(CBService *)service error:(nullable NSError *)error

-(void)peripheral:(CBPeripheral *)peripheral didDiscoverCharacteristicsForService:(CBService *)service error:(nullable NSError *)error
{
	auto CPP=CPPBluetoothLEPeripheralContext(self);
	CPP->PeripheralDiscoverCharacteristicsForService(service,error);
}

-(void)peripheral:(CBPeripheral *)peripheral didUpdateValueForCharacteristic:(CBCharacteristic *)characteristic error:(nullable NSError *)error
{
	auto CPP=CPPBluetoothLEPeripheralContext(self);
	CPP->PeripheralDidUpdateValueForCharacteristic(characteristic,error);
}

-(void)peripheral:(CBPeripheral *)peripheral didWriteValueForCharacteristic:(CBCharacteristic *)characteristic error:(nullable NSError *)error
{
	auto CPP=CPPBluetoothLEPeripheralContext(self);
	CPP->PeripheralDidWriteValueForCharacteristic(characteristic,error);
}

//-(void)peripheral:(CBPeripheral *)peripheral didUpdateNotificationStateForCharacteristic:(CBCharacteristic *)characteristic error:(nullable NSError *)error
//-(void)peripheral:(CBPeripheral *)peripheral didDiscoverDescriptorsForCharacteristic:(CBCharacteristic *)characteristic error:(nullable NSError *)error
//-(void)peripheral:(CBPeripheral *)peripheral didUpdateValueForDescriptor:(CBDescriptor *)descriptor error:(nullable NSError *)error
//-(void)peripheral:(CBPeripheral *)peripheral didWriteValueForDescriptor:(CBDescriptor *)descriptor error:(nullable NSError *)error

/*!
 *  This method is invoked after a failed call to @link writeValue:forCharacteristic:type: @/link, when <i>peripheral</i> is again
 *  ready to send characteristic value updates.
 */
//-(void)peripheralIsReadyToSendWriteWithoutResponse:(CBPeripheral *)peripheral

//-(void)peripheral:(CBPeripheral *)peripheral didOpenL2CAPChannel:(nullable CBL2CAPChannel *)channel error:(nullable NSError *)error


@end


@interface cnLib_BluetoothLECentralContext()<CBCentralManagerDelegate>

@end

@implementation cnLib_BluetoothLECentralContext
{
@private
	cnRTL::ocStaticIVar<cBLEPeripheralCentral> cnLib_IVar;
}

static cBLEPeripheralCentral* CPPBluetoothLECentralContext(cnLib_BluetoothLECentralContext *self)
{
	return reinterpret_cast<cBLEPeripheralCentral*>(&self->cnLib_IVar);
}

-(instancetype)init
{
	self=[super init];
	if(self){
		cnVar::ManualConstruct(*CPPBluetoothLECentralContext(self),self);
	}
	return self;
}

-(void)dealloc
{
	cnVar::ManualDestruct(*CPPBluetoothLECentralContext(self));

	[super dealloc];
}


#pragma mark CentralManager delegate

-(void)centralManagerDidUpdateState:(CBCentralManager *)central
{
	auto CPP=CPPBluetoothLECentralContext(self);
	CPP->DidUpdateState();
}

//-(void)centralManager:(CBCentralManager *)central willRestoreState:(NSDictionary<NSString *, id> *)dict;

-(void)centralManager:(CBCentralManager *)central didDiscoverPeripheral:(CBPeripheral *)peripheral advertisementData:(NSDictionary *)advertisementData RSSI:(NSNumber *)RSSI
{
	auto CPP=CPPBluetoothLECentralContext(self);
	CPP->DidDiscoverPeripheral(peripheral,advertisementData,RSSI);
}

-(void)centralManager:(CBCentralManager *)central didConnectPeripheral:(CBPeripheral *)peripheral
{
	auto PeripheralContext=ocSafeCast<cnLib_BluetoothLEPeripheralContext>(peripheral.delegate);
	if(PeripheralContext!=nil){
		auto CPPPeripheral=CPPBluetoothLEPeripheralContext(PeripheralContext);
		CPPPeripheral->CentralDidConnect();
	}
}

-(void)centralManager:(CBCentralManager *)central didFailToConnectPeripheral:(CBPeripheral *)peripheral error:(NSError *)error
{
	auto PeripheralContext=ocSafeCast<cnLib_BluetoothLEPeripheralContext>(peripheral.delegate);
	if(PeripheralContext!=nil){
		auto CPPPeripheral=CPPBluetoothLEPeripheralContext(PeripheralContext);
		CPPPeripheral->CentralDidFailToConnect(error);
	}
}


-(void)centralManager:(CBCentralManager *)central didDisconnectPeripheral:(CBPeripheral *)peripheral error:(NSError *)error
{
	auto PeripheralContext=ocSafeCast<cnLib_BluetoothLEPeripheralContext>(peripheral.delegate);
	if(PeripheralContext!=nil){
		auto CPPPeripheral=CPPBluetoothLEPeripheralContext(PeripheralContext);
		CPPPeripheral->CentralDidDisconnect(error);
	}
}

//-(void)centralManager:(CBCentralManager *)central connectionEventDidOccur:(CBConnectionEvent)event forPeripheral:(CBPeripheral *)peripheral NS_AVAILABLE_IOS(13_0);
//-(void)centralManager:(CBCentralManager *)central didUpdateANCSAuthorizationForPeripheral:(CBPeripheral *)peripheral NS_AVAILABLE_IOS(13_0);


@end


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cBLEDescriptor::cBLEDescriptor(cBLECharacteristic *Host)
	: fHost(Host)
	, fPeripheral(fHost->GetHostPeripheral())
{
}
//---------------------------------------------------------------------------
cBLEDescriptor::~cBLEDescriptor()
{
}
//---------------------------------------------------------------------------
iDispatch*_Nonnull cBLEDescriptor::GetHandlerDispatch(void)
{
	//auto CPPPeripheral=CPPBluetoothLEPeripheral(fPeripheral);
	//return CPPPeripheral->GetDispatch();
	return nullptr;
}
//---------------------------------------------------------------------------
bool cBLEDescriptor::InsertHandler(iGATTDescriptorHandler *_Nullable Handler)
{
	return false;
}
//---------------------------------------------------------------------------
bool cBLEDescriptor::RemoveHandler(iGATTDescriptorHandler *_Nullable Handler)
{
	return false;
}
//---------------------------------------------------------------------------
eGATTFunctionState cBLEDescriptor::GetFunctionState(void)
{
	return GATTFunctionState::Absent;
}
//---------------------------------------------------------------------------
iGATTCharacteristic*_Nonnull cBLEDescriptor::GetCharacterist(void)
{
	return fHost;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cBLECharacteristic::cBLECharacteristic(cBLEService *_Nonnull Host)
	: fHost(Host)
	, fPeripheral(fHost->GetHostPeripheral())
{
}
//---------------------------------------------------------------------------
cBLECharacteristic::~cBLECharacteristic()
{
}
//---------------------------------------------------------------------------
cBLEPeripheral*_Nonnull cBLECharacteristic::GetHostPeripheral(void)
{
	return fPeripheral;
}
//---------------------------------------------------------------------------
iDispatch*_Nonnull cBLECharacteristic::GetHandlerDispatch(void)
{
	return nullptr;
}
//---------------------------------------------------------------------------
bool cBLECharacteristic::InsertHandler(iGATTCharacteristHandler *_Nullable Handler)
{
	return false;
}
//---------------------------------------------------------------------------
bool cBLECharacteristic::RemoveHandler(iGATTCharacteristHandler *_Nullable Handler)
{
	return false;
}
//---------------------------------------------------------------------------
eGATTFunctionState cBLECharacteristic::GetFunctionState(void)
{
	return GATTFunctionState::Absent;
}
//---------------------------------------------------------------------------
iGATTService*_Nonnull cBLECharacteristic::GetService(void)
{
	return fHost;
}
//---------------------------------------------------------------------------
rPtr<iGATTDescriptor> cBLECharacteristic::PersistDescriptor(const cGATTUUID &ID)
{
	return nullptr;
}
//---------------------------------------------------------------------------
rPtr< iArrayReference<const void> > cBLECharacteristic::GetValue(void)
{
	return nullptr;
}
//---------------------------------------------------------------------------
iPtr<iAsyncTask> cBLECharacteristic::Write(const void *_Nonnull Data,uIntn DataSize)
{
	return nullptr;
}
//---------------------------------------------------------------------------
bool cBLECharacteristic::WriteWithoutResponse(const void *_Nonnull Data,uIntn DataSize)
{
	return false;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cBLEService::cBLEService(cBLEPeripheral *_Nonnull Peripheral)
	: fPeripheral(Peripheral)
{
}
//---------------------------------------------------------------------------
cBLEService::~cBLEService()
{
}
//---------------------------------------------------------------------------
cBLEPeripheral*_Nonnull cBLEService::GetHostPeripheral(void)
{
	return fPeripheral;
}
//---------------------------------------------------------------------------
iDispatch*_Nonnull cBLEService::GetHandlerDispatch(void)
{
	return nullptr;
}
//---------------------------------------------------------------------------
bool cBLEService::InsertHandler(iGATTServiceHandler *_Nullable Handler)
{
	return false;
}
//---------------------------------------------------------------------------
bool cBLEService::RemoveHandler(iGATTServiceHandler *_Nullable Handler)
{
	return false;
}
//---------------------------------------------------------------------------
eGATTFunctionState cBLEService::GetFunctionState(void)
{
	return GATTFunctionState::Absent;
}
//---------------------------------------------------------------------------
iGATTPeripheral*_Nonnull cBLEService::GetPeripheral(void)
{
	return fPeripheral;
}
//---------------------------------------------------------------------------
rPtr<iGATTCharacteristic> cBLEService::PersistCharacteristic(const cGATTUUID &ID)
{
	return nullptr;
}
//---------------------------------------------------------------------------
iPtr<iGATTCharacteristicAsyncFunction> cBLEService::QueryCharacteristics(void)
{
	return nullptr;
}
//---------------------------------------------------------------------------
void cBLEService::SetScanForCharacters(bool AllowScan)
{
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cBLEPeripheral::cBLEPeripheral(cnLib_BluetoothLEPeripheralContext *_Nonnull Context)
	: fContext(Context)
	, fPeripheral(nil)
{
}
//---------------------------------------------------------------------------
cBLEPeripheral::~cBLEPeripheral()
{
	if(fPeripheral!=nil){
		[fPeripheral release];
	}
}
//---------------------------------------------------------------------------
void cBLEPeripheral::IncreaseReference(void)
{
	[fContext retain];
}
//---------------------------------------------------------------------------
void cBLEPeripheral::DecreaseReference(void)
{
	[fContext release];
}
//---------------------------------------------------------------------------
void cBLEPeripheral::InitializePeripheral(cBLEPeripheralCentral *Central,CBPeripheral *Peripheral)
{
	if(fPeripheral!=nil)
		return;

	fHost=Central;
	fPeripheral=Peripheral;
	[fPeripheral retain];
}
//---------------------------------------------------------------------------
iDispatch*_Nonnull cBLEPeripheral::GetHandlerDispatch(void)
{
	return fHost->GetDispatch();
}
//---------------------------------------------------------------------------
bool cBLEPeripheral::InsertHandler(iGATTPeripheralHandler *_Nullable Handler)
{
	return false;
}
//---------------------------------------------------------------------------
bool cBLEPeripheral::RemoveHandler(iGATTPeripheralHandler *_Nullable Handler)
{
	return false;
}
//---------------------------------------------------------------------------
eGATTFunctionState cBLEPeripheral::GetFunctionState(void)
{
	return GATTFunctionState::Absent;
}
//---------------------------------------------------------------------------
iGATTPeripheralCentral* cBLEPeripheral::GetPeripheralCentral(void)
{
	return fHost;
}
//---------------------------------------------------------------------------
rPtr<iGATTService> cBLEPeripheral::PersistService(const cGATTUUID &ID)
{
	return nullptr;
	//auto &ServiceItem=fPersistServices[ID];
	//if(ServiceItem!=nullptr){
//		return ServiceItem;
	//}

	//auto NewService=rCreate<cBLEService>(fPeripheral);
	//ServiceItem=NewService;
	//return NewService;
}
//---------------------------------------------------------------------------
iPtr<iGATTServiceAsyncFunction> cBLEPeripheral::QueryServices(void)
{
	return nullptr;
}
//---------------------------------------------------------------------------
void cBLEPeripheral::SetScanForServices(bool AllowScan)
{
}
//---------------------------------------------------------------------------
bool cBLEPeripheral::IsConnecting(void)
{
	return false;
}
//---------------------------------------------------------------------------
void cBLEPeripheral::Connect(void)
{
	//[fPeripheral connect];
}
//---------------------------------------------------------------------------
void cBLEPeripheral::Disconnect(void)
{
	//[fPeripheral disconnect];
}
//---------------------------------------------------------------------------
void cBLEPeripheral::CentralDidConnect(void)
{
}
//---------------------------------------------------------------------------
void cBLEPeripheral::CentralDidFailToConnect(NSError *_Nonnull Error)
{
}
//---------------------------------------------------------------------------
void cBLEPeripheral::CentralDidDisconnect(NSError *_Nullable Error)
{
}
//---------------------------------------------------------------------------
void cBLEPeripheral::PeripheralDidDiscoverService(NSError *_Nullable Error)
{
}
//---------------------------------------------------------------------------
void cBLEPeripheral::PeripheralDiscoverCharacteristicsForService(CBService *_Nonnull Service,NSError *_Nullable Error)
{
}
//---------------------------------------------------------------------------
void cBLEPeripheral::PeripheralDidUpdateValueForCharacteristic(CBCharacteristic *_Nonnull Characteristic,NSError *_Nullable Error)
{
}
//---------------------------------------------------------------------------
void cBLEPeripheral::PeripheralDidWriteValueForCharacteristic(CBCharacteristic *_Nonnull Characteristic,NSError *_Nullable Error)
{
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cBLEPeripheralCentral::cBLEPeripheralCentral(cnLib_BluetoothLECentralContext *_Nonnull Context)
	: fContext(Context)
	, fCentralManager(nil)
{
}
//---------------------------------------------------------------------------
cBLEPeripheralCentral::~cBLEPeripheralCentral()
{
	if(fCentralManager!=nil){
		[fCentralManager release];
	}
}
//---------------------------------------------------------------------------
void cBLEPeripheralCentral::IncreaseReference(void)
{
	[fContext retain];
}
//---------------------------------------------------------------------------
void cBLEPeripheralCentral::DecreaseReference(void)
{
	[fContext release];
}
//---------------------------------------------------------------------------
bool cBLEPeripheralCentral::SetupCentralManager(iDispatch *_Nonnull Dispatch,NSDictionary *Options)
{
	if(fCentralManager!=nil)
		return false;
	auto GCDDisp=iCast<iGCDDispatch>(Dispatch);
	if(GCDDisp==nullptr)
		return false;
	auto Queue=GCDDisp->GetGCDDispatch();
	if(Options!=nil){
		if(@available(macos 10.9,*)){
			fCentralManager=[[CBCentralManager alloc]initWithDelegate:fContext queue:Queue options:Options];
		}
		else{
			return false;
		}
	}
	else{
		fCentralManager=[[CBCentralManager alloc]initWithDelegate:fContext queue:Queue];
	}
	if(fCentralManager==nil)
		return false;
	fDispatch=Dispatch;
	return true;
}
//---------------------------------------------------------------------------
iDispatch*_Nullable cBLEPeripheralCentral::GetDispatch(void)
{
	return fDispatch;
}
//---------------------------------------------------------------------------
iDispatch*_Nullable cBLEPeripheralCentral::GetHandlerDispatch(void)
{
	return fDispatch;
}
//---------------------------------------------------------------------------
bool cBLEPeripheralCentral::InsertHandler(iGATTPeripheralCentralHandler *Handler)
{
	return false;
}
//---------------------------------------------------------------------------
bool cBLEPeripheralCentral::RemoveHandler(iGATTPeripheralCentralHandler *Handler)
{
	return false;
}
//---------------------------------------------------------------------------
rPtr<iGATTPeripheral> cBLEPeripheralCentral::AccessPeripheral(iAddress *_Nullable Address)
{
	auto Peripherals=[fCentralManager retrievePeripheralsWithIdentifiers:nil];
	CBPeripheral *Peripheral=[Peripherals objectAtIndex:0];

	auto PeripheralContext=[[cnLib_BluetoothLEPeripheralContext alloc] init];
	auto CPPPeripheral=CPPBluetoothLEPeripheralContext(PeripheralContext);
	CPPPeripheral->InitializePeripheral(this,Peripheral);
	return rPtr<iGATTPeripheral>::TakeFromManual(CPPPeripheral);
}
//---------------------------------------------------------------------------
rPtr<iGATTPeripheralObserver> cBLEPeripheralCentral::CreatePeripheralObserver(const cGATTUUID *_Nonnull Service,uIntn ServiceCount)
{
	return nullptr;
}
//---------------------------------------------------------------------------
bool cBLEPeripheralCentral::ResetPeripheralCache(void)
{
	return false;
}
//---------------------------------------------------------------------------
bool cBLEPeripheralCentral::IsEnabled(void)
{
	return false;
}
//---------------------------------------------------------------------------
bool cBLEPeripheralCentral::IsPowerOn(void)
{
//	CBManagerStateResetting,
//	CBManagerStateUnsupported,
//	CBManagerStateUnauthorized,
//	CBManagerStatePoweredOff,
//	CBManagerStatePoweredOn,
	//if(@available(macos 10.13,*)){
		return fCentralManager.state==CBManagerStatePoweredOn;
	//}
	//else{
	//	return fCentralContext.manager.state==CBCentralManagerStatePoweredOn;
	//}


}
//---------------------------------------------------------------------------
void cBLEPeripheralCentral::DidUpdateState(void)
{
}
//---------------------------------------------------------------------------
void cBLEPeripheralCentral::DidDiscoverPeripheral(CBPeripheral *_Nonnull Peripheral,NSDictionary *_Nullable AdvertisementData,NSNumber *_Nullable RSSI)
{
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cBLEPeripheral*_Nonnull cnMac::cnLib_BluetoothLEPeripheralContext_GetPeripheral(cnLib_BluetoothLEPeripheralContext *_Nonnull Context)
{
	return CPPBluetoothLEPeripheralContext(Context);
}
//---------------------------------------------------------------------------
cBLEPeripheralCentral*_Nonnull cnMac::cnLib_BluetoothLECentralContext_GetCentral(cnLib_BluetoothLECentralContext *_Nonnull Context)
{
	return CPPBluetoothLECentralContext(Context);
}
//---------------------------------------------------------------------------


#if	0

cnLib_INTERFACE_LOCALID_DEFINE(cBluetoothLEConectionDevice);

static bcBluetoothLEConectionProtocol* CPPBluetoothLEConectionProtocol(cnLib_BluetoothLEConnectionProtocol *self);
static cBluetoothLEConectionProtocol_10_7* CPPBluetoothLEConectionProtocol_10_7(cnLib_BluetoothLEConnectionProtocol *self);
static cBluetoothLEConectionProtocol_10_9* CPPBluetoothLEConectionProtocol_10_9(cnLib_BluetoothLEConnectionProtocol *self);
static cBluetoothLEConectionPeripheral* CPPBluetoothLEConectionPeripheral(cnLib_BluetoothLEConectionPeripheral *self);

//---------------------------------------------------------------------------
static CBUUID* CreateCBUUIDFromData(const void *IDData){
	auto d=[NSData dataWithBytesNoCopy:const_cast<void*>(IDData) length:16 freeWhenDone:NO];
	return [CBUUID UUIDWithData:d];
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cBluetoothLEConectionDeviceEndpoint::cBluetoothLEConectionDeviceEndpoint()
{
}
//---------------------------------------------------------------------------
cBluetoothLEConectionDeviceEndpoint::~cBluetoothLEConectionDeviceEndpoint()
{
}
//---------------------------------------------------------------------------
const void* cBluetoothLEConectionDeviceEndpoint::CastInterface(iTypeID IID)const
{
	return ImpCastInterface<iConnection,iEndpoint>(this,IID);
}
//---------------------------------------------------------------------------
void cBluetoothLEConectionDeviceEndpoint::PushData(const void *Data,uIntn Size)
{
	auto WriteMemory=fReadDataQueue.ReserveWriteBuffer(Size);
	cnMemory::Copy(WriteMemory.Pointer,Data,Size);
	fReadDataQueue.CommitWriteBuffer(Size);

	this->UpdateReadQueueState();
}
//---------------------------------------------------------------------------
cMemory cBluetoothLEConectionDeviceEndpoint::PullData(uIntn QuerySize)
{
	return fWriteDataQueue.GatherReadBuffer(QuerySize);
}
//---------------------------------------------------------------------------
void cBluetoothLEConectionDeviceEndpoint::AdvancePullData(uIntn Size)
{
	fWriteDataQueue.DismissReadBuffer(Size);
}
//---------------------------------------------------------------------------
cConstMemory cBluetoothLEConectionDeviceEndpoint::GatherReadBuffer(uIntn Size)
{
	return fReadDataQueue.GatherReadBuffer(Size);
}
//---------------------------------------------------------------------------
uIntn cBluetoothLEConectionDeviceEndpoint::DismissReadBuffer(uIntn Size)
{
	return fReadDataQueue.DismissReadBuffer(Size);
}
//---------------------------------------------------------------------------
cMemory cBluetoothLEConectionDeviceEndpoint::ReserveWriteBuffer(uIntn Size)
{
	return fWriteDataQueue.ReserveWriteBuffer(Size);
}
//---------------------------------------------------------------------------
uIntn cBluetoothLEConectionDeviceEndpoint::CommitWriteBuffer(uIntn Size)
{
	uIntn SizeCommited=fWriteDataQueue.CommitWriteBuffer(Size);

	Callback->BluetoothLENotifyWrite();
	return SizeCommited;
}
//---------------------------------------------------------------------------
void cBluetoothLEConectionDeviceEndpoint::Close(void)
{
}
//---------------------------------------------------------------------------
iReadQueue* cBluetoothLEConectionDeviceEndpoint::GetReadQueue(void)
{
	return this;
}
//---------------------------------------------------------------------------
iWriteQueue* cBluetoothLEConectionDeviceEndpoint::GetWriteQueue(void)
{
	return this;
}
//---------------------------------------------------------------------------
void cBluetoothLEConectionDeviceEndpoint::SetWriteEndMode(eEndpointWriteEndMode EndMode)
{
	WriteQueueSetEndMode(EndMode);
}
//---------------------------------------------------------------------------
void cBluetoothLEConectionDeviceEndpoint::ReadQueueClosed(void)
{
}
//---------------------------------------------------------------------------
void cBluetoothLEConectionDeviceEndpoint::WriteQueueClosed(void)
{
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool cBluetoothLEConectionDevice::cConnectTask::IsDone(void)
{
	return TaskState.IsDone();
}
//---------------------------------------------------------------------------
bool cBluetoothLEConectionDevice::cConnectTask::SetNotify(iProcedure *NotifyProcedure)
{
	return TaskState.SetNotify(NotifyProcedure);
}
//---------------------------------------------------------------------------
void cBluetoothLEConectionDevice::cConnectTask::Cancel(void)
{
}
//---------------------------------------------------------------------------
iConnection* cBluetoothLEConectionDevice::cConnectTask::GetConnection(void)
{
	return Connection;
}
//---------------------------------------------------------------------------
void cBluetoothLEConectionDevice::cConnectTask::CreateConnection(iEndpoint *Endpoint)
{
	Connection=cnRTL::CreateConnectionFromEndpoint(ConnectionTypeID,Endpoint,nullptr,nullptr,false);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cBluetoothLEConectionDevice::cDeviceEndpoint::cDeviceEndpoint()
{
}
//---------------------------------------------------------------------------
cBluetoothLEConectionDevice::cDeviceEndpoint::~cDeviceEndpoint()
{
}
//---------------------------------------------------------------------------
iReference* cBluetoothLEConectionDevice::cDeviceEndpoint::RWQueueInnerReference(void)
{
	return nullptr;
}
//---------------------------------------------------------------------------
uIntn cBluetoothLEConectionDevice::cDeviceEndpoint::DismissReadBuffer(uIntn Size)
{
	uIntn SizeDismissed=cBufferedExtEndpoint::DismissReadBuffer(Size);

	Owner->EndpointNotifyRead();

	return SizeDismissed;
}
//---------------------------------------------------------------------------
uIntn cBluetoothLEConectionDevice::cDeviceEndpoint::CommitWriteBuffer(uIntn Size)
{
	uIntn SizeCommited=cBufferedExtEndpoint::CommitWriteBuffer(Size);

	Owner->EndpointNotifyWrite();

	return SizeCommited;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cBluetoothLEConectionDevice::cBluetoothLEConectionDevice(cnLib_BluetoothLEConectionPeripheral *Peripheral,CBService *Service,CBCharacteristic *ReadChar,CBCharacteristic *WriteChar)
	: fPeripheral(Peripheral)
	, fService(Service)
	, fReadChar(ReadChar)
	, fWriteChar(WriteChar)
{
	[fPeripheral retain];
	[fService retain];
	[fReadChar retain];
	[fWriteChar retain];
}
//---------------------------------------------------------------------------
cBluetoothLEConectionDevice::~cBluetoothLEConectionDevice()
{
	[fService release];
	[fReadChar release];
	[fWriteChar release];
	[fPeripheral release];
}
//---------------------------------------------------------------------------
iPtr<iConnectionTask> cBluetoothLEConectionDevice::ConnectAsync(iTypeID ConnectionIID)
{
	if(fEndpoint!=nullptr)
		return nullptr;
	if(fConnectTask!=nullptr)
		return nullptr;
	fConnectTask=iCreate<cConnectTask>();
	fConnectTask->Owner=this;
	fConnectTask->ConnectionTypeID=ConnectionIID;

	auto CPPPeripheral=CPPBluetoothLEConectionPeripheral(fPeripheral);

	CPPPeripheral->BLEDeviceActive(this);
	
	return fConnectTask;
}
//---------------------------------------------------------------------------
void cBluetoothLEConectionDevice::EndpointNotifyRead(void)
{
}
//---------------------------------------------------------------------------
void cBluetoothLEConectionDevice::EndpointNotifyWrite(void)
{
	auto CPPPeripheral=CPPBluetoothLEConectionPeripheral(fPeripheral);
	auto Protocol=CPPPeripheral->GetProtocol();

	auto DispQueue=[Protocol queue];

	dispatch_async(DispQueue, ^{
		BLEWriteProcedure();
	});
}
//---------------------------------------------------------------------------
void cBluetoothLEConectionDevice::BLEWriteProcedure(void)
{
	if(fEndpoint==nullptr)
		return;

	auto DataToWrite=fEndpoint->QueryWriteData();

	auto Data=[[NSData alloc]initWithBytes:DataToWrite.Pointer length:DataToWrite.Length];
	[fPeripheral.peripheral writeValue:Data forCharacteristic:fWriteChar type:CBCharacteristicWriteWithoutResponse];

	fEndpoint->AdvanceWriteData(DataToWrite.Length);
}
//---------------------------------------------------------------------------
eOrdering cBluetoothLEConectionDevice::Compare(const iAddress *Dest)const
{
	return Ordering::Different;
}
//---------------------------------------------------------------------------
void cBluetoothLEConectionDevice::onDeviceChecked(void)
{
	if(fConnectTask!=nullptr){
		auto CPPPeripheral=CPPBluetoothLEConectionPeripheral(fPeripheral);
		CPPPeripheral->BLEDeviceConnect(fReadChar,this);
	}
}
//---------------------------------------------------------------------------
void cBluetoothLEConectionDevice::onDeviceRegisterNotification(bool Error)
{
	if(fConnectTask!=nullptr){
		if(Error==false){
			fEndpoint=iCreate<cDeviceEndpoint>();
			fEndpoint->Owner=this;

			fConnectTask->CreateConnection(fEndpoint);
		}
		fConnectTask->TaskState.SetDone();
		fConnectTask=nullptr;
	}
}
//---------------------------------------------------------------------------
void cBluetoothLEConectionDevice::onUpdateCharacteristicValue(void)
{
	auto Data=fReadChar.value;
	fEndpoint->PutReadData(Data.bytes,Data.length);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cBluetoothLEConnectionDeviceBrowser::cBluetoothLEConnectionDeviceBrowser(cnLib_BluetoothLEConnectionProtocol *Protocol,const cBluetoothLEConnectionServiceID *Services,uIntn ServiceCount)
	: fProtocol(Protocol)
{
	[fProtocol retain];
	fCallback=nullptr;

	for(uIntn i=0;i<ServiceCount;i++){
		auto &ServiceID=Services[i];
		auto ServiceIDItem=rClsCreate<cBluetoothLEConnectionServiceIDItem>();
		ServiceIDItem->ID=ServiceID;
		ServiceIDItem->ServiceID=CreateCBUUIDFromData(ServiceID.ServiceUUID);
		ServiceIDItem->ReadCharID=CreateCBUUIDFromData(ServiceID.ReadCharacteristUUID);
		ServiceIDItem->WriteCharID=CreateCBUUIDFromData(ServiceID.WriteCharacteristUUID);

		fServiceIDs.AppendMake(cnVar::MoveCast(ServiceIDItem));
		
	}
}
//---------------------------------------------------------------------------
cBluetoothLEConnectionDeviceBrowser::~cBluetoothLEConnectionDeviceBrowser()
{
	[fProtocol release];
}
//---------------------------------------------------------------------------
void cBluetoothLEConnectionDeviceBrowser::VirtualStopped(void)
{
	auto CPPProtocol=CPPBluetoothLEConectionProtocol_10_7(fProtocol);
	CPPProtocol->BrowserClosed(this);


	InnerDecReference('self');
}
//---------------------------------------------------------------------------
cArray< const rClsRef<cBluetoothLEConnectionServiceIDItem> > cBluetoothLEConnectionDeviceBrowser::GetServiceIDs(void)const
{
	return fServiceIDs.Storage();
}
//---------------------------------------------------------------------------
iAddress* cBluetoothLEConnectionDeviceBrowser::GetLocalAddress(void)
{
	return nullptr;
}
//---------------------------------------------------------------------------
void cBluetoothLEConnectionDeviceBrowser::BLEOnDeviceChecked(cnLib_BluetoothLEConectionPeripheral *ConnectionPeripheral)
{
	bool UpdateQueue=false;
	auto CPPConnectionPeripheral=CPPBluetoothLEConectionPeripheral(ConnectionPeripheral);
	for(auto &ServiceIDItem : fServiceIDs){
		auto Service=CPPConnectionPeripheral->GetServiceByUUID(ServiceIDItem->ID.ServiceUUID);
		if(Service!=nullptr){
			auto ReadChar=CPPConnectionPeripheral->GetCharacteristByUUID(ServiceIDItem->ID.ServiceUUID,ServiceIDItem->ID.ReadCharacteristUUID);
			auto WriteChar=CPPConnectionPeripheral->GetCharacteristByUUID(ServiceIDItem->ID.ServiceUUID,ServiceIDItem->ID.WriteCharacteristUUID);
			if(ReadChar!=nil && WriteChar!=nil){
				UpdateQueue=true;
				auto Device=iCreate<cBluetoothLEConectionDevice>(ConnectionPeripheral,Service,ReadChar,WriteChar);

				auto Item=fDeviceItemRecycler.Query();
				Item->Peripheral=ConnectionPeripheral;
				Item->Device=Device;
				fDeviceQueue.Enqueue(Item);
			}

		}
	}
	if(UpdateQueue){
		UpdateSignalState();
	}
}
//---------------------------------------------------------------------------
bool cBluetoothLEConnectionDeviceBrowser::StartNotify(iReference *Reference,iAsyncNotificationCallback *Callback)
{
	auto StartProc=StartSignal();
	if(StartProc==false){
		return false;
	}

	fCallback=Callback;
	fCallbackReference=Reference;

	StartProc.Execute();
	return true;
}
//---------------------------------------------------------------------------
void cBluetoothLEConnectionDeviceBrowser::StopNotify(void)
{
	return StopSignal();
}
//---------------------------------------------------------------------------
void cBluetoothLEConnectionDeviceBrowser::NotifyCallback(bool IdleNotify)
{
	return NotifySignal(IdleNotify);
}
//---------------------------------------------------------------------------
bool cBluetoothLEConnectionDeviceBrowser::IsClosed(void)
{
	return SignalIsClosed();
}
//---------------------------------------------------------------------------
iPtr<iAddress> cBluetoothLEConnectionDeviceBrowser::Fetch(void)
{
	auto Item=fDeviceQueue.Dequeue();
	if(Item==nullptr)
		return nullptr;

	auto Device=cnVar::MoveCast(Item->Device);
	Item->Peripheral=nil;
	fDeviceItemRecycler.Recycle(Item);

	return Device;
}
//---------------------------------------------------------------------------
bool cBluetoothLEConnectionDeviceBrowser::FetchBLEDevice(cBluetoothLEDeviceInfo &DeviceInfo)
{
	auto Item=fDeviceQueue.Dequeue();
	if(Item==nullptr)
		return false;

	auto CPPPeripheral=CPPBluetoothLEConectionPeripheral(Item->Peripheral);
	DeviceInfo.Name=CPPPeripheral->GetName().Token();
	DeviceInfo.Address=cnVar::MoveCast(Item->Device);

	Item->Peripheral=nil;
	fDeviceItemRecycler.Recycle(Item);
	return true;
}
//---------------------------------------------------------------------------
iReference* cBluetoothLEConnectionDeviceBrowser::AsyncSignalInnerReference(void)
{
	return &fInnerReference;
}
//---------------------------------------------------------------------------
void cBluetoothLEConnectionDeviceBrowser::AsyncSignalStarted(void)
{
	auto CPPProtocol=CPPBluetoothLEConectionProtocol_10_7(fProtocol);
	CPPProtocol->BrowserStarted(this);

	fCallback->AsyncStarted();
}
//---------------------------------------------------------------------------
void cBluetoothLEConnectionDeviceBrowser::AsyncSignalStopped(void)
{
	auto CPPProtocol=CPPBluetoothLEConectionProtocol_10_7(fProtocol);
	CPPProtocol->BrowserStopped(this);

	auto Callback=fCallback;
	fCallback=nullptr;
	Callback->AsyncStopped();
}
//---------------------------------------------------------------------------
cBluetoothLEConnectionDeviceBrowser::Availability cBluetoothLEConnectionDeviceBrowser::AsyncSignalAvailable(void)
{
	if(fDeviceQueue.IsEmpty()){
		return Availability::Empty;
	}
	return Availability::Available;
}
//---------------------------------------------------------------------------
void cBluetoothLEConnectionDeviceBrowser::AsyncSignalNotify(void)
{
	fCallback->AsyncNotify();
}
//---------------------------------------------------------------------------
void cBluetoothLEConnectionDeviceBrowser::AsyncSignalClosed(void)
{
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//@interface cnLib_BluetoothLEConnectionService()<>
//@end

@implementation cnLib_BluetoothLEConnectionService
{
@private
	cnRTL::ocStaticIVar<cBluetoothLEConectionService> cnLib_IVar;
}

static cBluetoothLEConectionService* CPPBluetoothLEConectionService(cnLib_BluetoothLEConnectionService *self)
{
	return reinterpret_cast<cBluetoothLEConectionService*>(&self->cnLib_IVar);
}

-(instancetype)init
{
	self=[super init];
	if(self!=nil){
		cnVar::ManualConstruct(*CPPBluetoothLEConectionService(self),self);
	}
	return self;
}

-(void)dealloc
{
	cnVar::ManualDestruct(*CPPBluetoothLEConectionService(self));
	[super dealloc];
}

-(cnLibrary::iAddress *)interfaceAddress
{
	return CPPBluetoothLEConectionService(self);
}

@end

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cBluetoothLEConectionService::cBluetoothLEConectionService(cnLib_BluetoothLEConnectionService *Service)
	: fService(Service)
{
}
//---------------------------------------------------------------------------
cBluetoothLEConectionService::~cBluetoothLEConectionService()
{
}
//---------------------------------------------------------------------------
const void *cBluetoothLEConectionService::CastInterface(iTypeID InterfaceID)const
{
	return ImpCastInterface<iAddress,iOCObject,iReference>(this,InterfaceID);
}
//---------------------------------------------------------------------------
void cBluetoothLEConectionService::IncReference(void)
{
	[fService retain];
}
//---------------------------------------------------------------------------
void cBluetoothLEConectionService::DecReference(void)
{
	[fService release];
}
//---------------------------------------------------------------------------
eOrdering cBluetoothLEConectionService::Compare(const iAddress *Dest)const
{
	return Ordering::Different;
}
//---------------------------------------------------------------------------
id cBluetoothLEConectionService::GetOCObject(void)
{
	return fService;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cBluetoothLEConnectionQueue::cCentralEndpoint::cCentralEndpoint()
{
}
//---------------------------------------------------------------------------
cBluetoothLEConnectionQueue::cCentralEndpoint::~cCentralEndpoint()
{
}
//---------------------------------------------------------------------------
iReference* cBluetoothLEConnectionQueue::cCentralEndpoint::RWQueueInnerReference(void)
{
#pragma message("TODO - Internal Reference")
	return nullptr;
}
//---------------------------------------------------------------------------
uIntn cBluetoothLEConnectionQueue::cCentralEndpoint::DismissReadBuffer(uIntn Size)
{
	uIntn SizeDismissed=cBufferedExtEndpoint::DismissReadBuffer(Size);

	Owner->EndpointNotifyRead(this);

	return SizeDismissed;
}
//---------------------------------------------------------------------------
uIntn cBluetoothLEConnectionQueue::cCentralEndpoint::CommitWriteBuffer(uIntn Size)
{
	uIntn SizeCommited=cBufferedExtEndpoint::CommitWriteBuffer(Size);

	Owner->EndpointNotifyWrite(this);

	return SizeCommited;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cBluetoothLEConnectionQueue::cBluetoothLEConnectionQueue(cnLib_BluetoothLEConnectionProtocol *Owner,cnLib_BluetoothLEConnectionService *Service)
	: fOwner(Owner)
	, fService(Service)
{
}
cBluetoothLEConnectionQueue::~cBluetoothLEConnectionQueue()
{
}
//---------------------------------------------------------------------------
void cBluetoothLEConnectionQueue::VirtualStopped(void)
{
	dispatch_async([fOwner queue],^{
		auto CPP=CPPBluetoothLEConectionProtocol_10_9(fOwner);
		CPP->QueueListenStopped(this);
		InnerDecReference('self');
	});
}
//---------------------------------------------------------------------------
cnLib_BluetoothLEConnectionService* cBluetoothLEConnectionQueue::GetService(void)const
{
	return fService;
}
//---------------------------------------------------------------------------
void cBluetoothLEConnectionQueue::OnAddServiceError(NSError *Error)
{
	CloseSignal();
}
//---------------------------------------------------------------------------
iAddress* cBluetoothLEConnectionQueue::GetLocalAddress(void)
{
	return nullptr;
}
//---------------------------------------------------------------------------
iPtr<iConnection> cBluetoothLEConnectionQueue::FetchConnection(void)
{
	cConnectionQueueItem *QueueItem;
	while((QueueItem=fConnectionQueue.Dequeue())!=nullptr){
		if(QueueItem->Endpoint!=nullptr){

			auto Connection=iCreate<cnRTL::cEndpointConnection>();
			Connection->Endpoint=cnVar::MoveCast(QueueItem->Endpoint);
			Connection->LocalAddress=[fService interfaceAddress];
			//Connection->RemoteAddress=nullptr;

			fConnectionQueueItemRecycler.Recycle(QueueItem);
			return Connection;
		}
		fConnectionQueueItemRecycler.Recycle(QueueItem);
	}
	return nullptr;
}
//---------------------------------------------------------------------------
iReference* cBluetoothLEConnectionQueue::AsyncSignalInnerReference(void)
{
#pragma message ("TODO - Internal Reference")
	return nullptr;
}
//---------------------------------------------------------------------------
void cBluetoothLEConnectionQueue::AsyncSignalStarted(void)
{
	iIncReference(this,'conn');
}
//---------------------------------------------------------------------------
void cBluetoothLEConnectionQueue::AsyncSignalStopped(void)
{

	iDecReference(this,'conn');
}
//---------------------------------------------------------------------------
cBluetoothLEConnectionQueue::Availability cBluetoothLEConnectionQueue::AsyncSignalAvailable(void)
{
	if(fConnectionQueue.IsEmpty()){
		return Availability::Empty;
	}
	return Availability::Available;
}
//---------------------------------------------------------------------------
void cBluetoothLEConnectionQueue::AsyncSignalClosed(void)
{
}
//---------------------------------------------------------------------------
cBluetoothLEConnectionQueue::cCentralEndpoint* cBluetoothLEConnectionQueue::SetupCentral(CBCentral *Central)
{
	auto &Endpoint=fCentralMap[Central];
	if(Endpoint==nullptr){
		Endpoint=iCreate<cCentralEndpoint>();
		Endpoint->Owner=this;
		Endpoint->Central=Central;
		auto Item=fConnectionQueueItemRecycler.Query();
		Item->Endpoint=Endpoint;
		fConnectionQueue.Enqueue(Item);
		UpdateSignalState();
	}
	return Endpoint;
}
//---------------------------------------------------------------------------
void cBluetoothLEConnectionQueue::OnPeripheralClosed(void)
{
	for(auto pCentralItem : fCentralMap){
		if(pCentralItem.Value!=nullptr){
			pCentralItem.Value->Close();
		}
	}
	fCentralMap.Clear();
}
//---------------------------------------------------------------------------
void cBluetoothLEConnectionQueue::OnCentralSubscribe(CBCentral *Central)
{
	SetupCentral(Central);
}
//---------------------------------------------------------------------------
void cBluetoothLEConnectionQueue::OnCentralUnsubscribe(CBCentral *Central)
{
	auto pCentralEndpoint=fCentralMap.GetPair(Central);
	if(pCentralEndpoint==nullptr)
		return;
	if(pCentralEndpoint->Value!=nullptr){
		pCentralEndpoint->Value->Close();
		pCentralEndpoint->Value=nullptr;
	}
	fCentralMap.RemovePair(pCentralEndpoint);
}
//---------------------------------------------------------------------------
bool cBluetoothLEConnectionQueue::TestRequestWritable(CBATTRequest *Request)
{
	auto WriteChar=[fService writeCharacteristic];
	if(WriteChar!=Request.characteristic)
		return false;	// incorrect characteristic

	return true;
}
//---------------------------------------------------------------------------
void cBluetoothLEConnectionQueue::OnCentralWrite(CBATTRequest *Request)
{
	auto CentralEndpoint=SetupCentral(Request.central);
	CentralEndpoint->PutReadData(Request.value.bytes,Request.value.length);
}
//---------------------------------------------------------------------------
void cBluetoothLEConnectionQueue::EndpointNotifyRead(cCentralEndpoint *Endpoint)
{
}
//---------------------------------------------------------------------------
void cBluetoothLEConnectionQueue::EndpointNotifyWrite(cCentralEndpoint *Endpoint)
{
	auto DispQueue=[fOwner queue];

	dispatch_async(DispQueue, ^{
		auto OwnerCPP=CPPBluetoothLEConectionProtocol_10_9(fOwner);
		OwnerCPP->BLEOnUpdateSubscribers();
	});
}
//---------------------------------------------------------------------------
void cBluetoothLEConnectionQueue::OnUpdateSubscribers(void)
{
	auto PeripheralManager=[fOwner peripheralManager];
	auto ReadCharacteristic=ocSafeCast<CBMutableCharacteristic>([fService readCharacteristic]);
	for(auto pCentral : fCentralMap){
		auto &Endpoint=pCentral.Value;
		auto WriteData=Endpoint->QueryWriteData();
		while(WriteData.Length!=0){
			auto ReadData=[[NSData alloc]initWithBytes:WriteData.Pointer length:WriteData.Length];
			BOOL Updated=[PeripheralManager updateValue:ReadData forCharacteristic:ReadCharacteristic onSubscribedCentrals:@[pCentral.Key]];
			if(Updated==false){
				// terminate
				return;
			}
			else{
				Endpoint->AdvanceWriteData(WriteData.Length);
			}
			WriteData=Endpoint->QueryWriteData();
		}
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

@implementation cnLib_BluetoothLEConectionPeripheral
{
@private
	cnRTL::ocStaticIVar<cBluetoothLEConectionPeripheral> cnLib_IVar;
}

static cBluetoothLEConectionPeripheral* CPPBluetoothLEConectionPeripheral(cnLib_BluetoothLEConectionPeripheral *self)
{
	return reinterpret_cast<cBluetoothLEConectionPeripheral*>(&self->cnLib_IVar);
}

-(instancetype)init
{
	self=[super init];
	if(self){
		cnVar::ManualConstruct(*CPPBluetoothLEConectionPeripheral(self),self);
	}
	return self;
}

-(void)dealloc
{
	cnVar::ManualDestruct(*CPPBluetoothLEConectionPeripheral(self));
	[super dealloc];
}

-(CBPeripheral *)peripheral
{
	auto CPP=CPPBluetoothLEConectionPeripheral(self);
	return CPP->GetPeripheral();
}

#pragma mark - peripheral delegate

-(void)peripheralDidUpdateName:(CBPeripheral *)peripheral NS_AVAILABLE(10_9, 6_0)
{
}

-(void)peripheral:(CBPeripheral *)peripheral didModifyServices:(NSArray<CBService *> *)invalidatedServices NS_AVAILABLE(10_9, 7_0)
{
}

-(void)peripheral:(CBPeripheral *)peripheral didReadRSSI:(NSNumber *)RSSI error:(nullable NSError *)error NS_AVAILABLE(10_13, 8_0)
{
}

-(void)peripheral:(CBPeripheral *)peripheral didDiscoverServices:(nullable NSError *)error
{
	auto CPP=CPPBluetoothLEConectionPeripheral(self);
	if(error){
		CPP->ocOnErrorDiscoverService(error);
	}
	else{
		CPP->ocOnDiscoveredService();
	}
}

//-(void)peripheral:(CBPeripheral *)peripheral didDiscoverIncludedServicesForService:(CBService *)service error:(nullable NSError *)error

-(void)peripheral:(CBPeripheral *)peripheral didDiscoverCharacteristicsForService:(CBService *)service error:(nullable NSError *)error
{
	auto CPP=CPPBluetoothLEConectionPeripheral(self);
	if(error){
		CPP->ocOnErrorDiscoverCharacteristic(service,error);
	}
	else{
		CPP->ocOnDiscoveredCharacteristic(service);
	}
}

-(void)peripheral:(CBPeripheral *)peripheral didUpdateValueForCharacteristic:(CBCharacteristic *)characteristic error:(nullable NSError *)error
{
	auto CPP=CPPBluetoothLEConectionPeripheral(self);
	if(error){
	}
	else{
		CPP->ocOnUpdateCharacteristValue(characteristic);
	}
}

-(void)peripheral:(CBPeripheral *)peripheral didWriteValueForCharacteristic:(CBCharacteristic *)characteristic error:(nullable NSError *)error
{
	//auto CPP=CPPBluetoothLEConectionPeripheral(self);
	//if(error){
	//}
	//else{
	//}
}

-(void)peripheral:(CBPeripheral *)peripheral didUpdateNotificationStateForCharacteristic:(CBCharacteristic *)characteristic error:(nullable NSError *)error
{
	auto CPP=CPPBluetoothLEConectionPeripheral(self);
	CPP->ocOnUpdateNotificationState(characteristic,error);
}

//-(void)peripheral:(CBPeripheral *)peripheral didDiscoverDescriptorsForCharacteristic:(CBCharacteristic *)characteristic error:(nullable NSError *)error
//-(void)peripheral:(CBPeripheral *)peripheral didUpdateValueForDescriptor:(CBDescriptor *)descriptor error:(nullable NSError *)error
//-(void)peripheral:(CBPeripheral *)peripheral didWriteValueForDescriptor:(CBDescriptor *)descriptor error:(nullable NSError *)error

/*!
 *  This method is invoked after a failed call to @link writeValue:forCharacteristic:type: @/link, when <i>peripheral</i> is again
 *  ready to send characteristic value updates.
 */
//-(void)peripheralIsReadyToSendWriteWithoutResponse:(CBPeripheral *)peripheral

//-(void)peripheral:(CBPeripheral *)peripheral didOpenL2CAPChannel:(nullable CBL2CAPChannel *)channel error:(nullable NSError *)error


@end



//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cBluetoothLEConectionPeripheral::cBluetoothLEConectionPeripheral(cnLib_BluetoothLEConectionPeripheral *ConnectionPeripheral)
	: fConnectionPeripheral(ConnectionPeripheral)
{
}
//---------------------------------------------------------------------------
cBluetoothLEConectionPeripheral::~cBluetoothLEConectionPeripheral()
{
	if(fPeripheral!=nil){
		[fPeripheral release];
	}
}
//---------------------------------------------------------------------------
void cBluetoothLEConectionPeripheral::Initialize(cnLib_BluetoothLEConnectionProtocol *Protocol,CBPeripheral *peripheral)
{
	fProtocol=Protocol;

	fPeripheral=peripheral;
	[fPeripheral retain];
	fPeripheral.delegate=fConnectionPeripheral;

	fName=NSStringToCStr(fPeripheral.name);

	fNeedScanService=true;
	NotifyPeripheralStateProcedure();
}
//---------------------------------------------------------------------------
const void *cBluetoothLEConectionPeripheral::CastInterface(iTypeID InterfaceID)const
{
	return ImpCastInterface<iOCObject,iReference>(this,InterfaceID);
}
//---------------------------------------------------------------------------
void cBluetoothLEConectionPeripheral::IncReference(void)
{
	[fConnectionPeripheral retain];
}
//---------------------------------------------------------------------------
void cBluetoothLEConectionPeripheral::DecReference(void)
{
	[fConnectionPeripheral release];
}
//---------------------------------------------------------------------------
id cBluetoothLEConectionPeripheral::GetOCObject(void)
{
	return fConnectionPeripheral;
}
//---------------------------------------------------------------------------
cnLib_BluetoothLEConnectionProtocol* cBluetoothLEConectionPeripheral::GetProtocol(void)const
{
	return fProtocol;
}
//---------------------------------------------------------------------------
CBPeripheral* cBluetoothLEConectionPeripheral::GetPeripheral(void)const
{
	return fPeripheral;
}
//---------------------------------------------------------------------------
CBService* cBluetoothLEConectionPeripheral::GetServiceByUUID(const void *ID)
{
	if(fPeripheral==nil)
		return nil;
	auto DestID=CreateCBUUIDFromData(ID);
	for(CBService *Service in fPeripheral.services){
		if([Service.UUID isEqual:DestID]){
			return Service;
		}
	}

	return nil;
}
//---------------------------------------------------------------------------
CBCharacteristic* cBluetoothLEConectionPeripheral::GetCharacteristByUUID(const void *ServiceID,const void *CharacterID)
{
	auto Service=GetServiceByUUID(ServiceID);
	if(Service==nil)
		return nil;
	auto DestID=CreateCBUUIDFromData(CharacterID);
	for(CBCharacteristic *Char in Service.characteristics){
		if([Char.UUID isEqual:DestID]){
			return Char;
		}
	}
	return nil;
}
//---------------------------------------------------------------------------
cString<uChar16> cBluetoothLEConectionPeripheral::GetName(void)const
{
	return fName;
}
//---------------------------------------------------------------------------
void cBluetoothLEConectionPeripheral::BLEUpdateRSSI(NSNumber *Value)
{
	fRSSI=Value.shortValue;
}
//---------------------------------------------------------------------------
void cBluetoothLEConectionPeripheral::BLEUpdateADData(NSDictionary *ADData)
{
	NSString *LocalName=ocSafeCast<NSString>([ADData objectForKey:CBAdvertisementDataLocalNameKey]);
	if(LocalName!=nil){
		fName=NSStringToCStr(LocalName);
	}
}
//---------------------------------------------------------------------------
void cBluetoothLEConectionPeripheral::BLEOnConnected(void)
{
	fConnected=true;
	fPeripheralCommandInProgress=false;

	NotifyPeripheralStateProcedure();
}
//---------------------------------------------------------------------------
void cBluetoothLEConectionPeripheral::BLEOnConnectFailed(void)
{
	fConnected=false;
	fPeripheralCommandInProgress=false;

	NotifyPeripheralStateProcedure();
}
//---------------------------------------------------------------------------
void cBluetoothLEConectionPeripheral::BLEOnDisconnected(void)
{
	fConnected=false;
	fPeripheralCommandInProgress=false;

	NotifyPeripheralStateProcedure();
}
//---------------------------------------------------------------------------
void cBluetoothLEConectionPeripheral::ocOnDiscoveredService(void)
{
	fServiceChecked=true;
	fPeripheralCommandInProgress=false;

	NotifyPeripheralStateProcedure();
}
//---------------------------------------------------------------------------
void cBluetoothLEConectionPeripheral::ocOnErrorDiscoverService(NSError *Error)
{
	fServiceChecked=true;
	fPeripheralCommandInProgress=false;

	NotifyPeripheralStateProcedure();
}
//---------------------------------------------------------------------------
void cBluetoothLEConectionPeripheral::ocOnDiscoveredCharacteristic(CBService *Service)
{
	fCharacteriscCheckedIndex++;
	fPeripheralCommandInProgress=false;

	NotifyPeripheralStateProcedure();
}
//---------------------------------------------------------------------------
void cBluetoothLEConectionPeripheral::ocOnErrorDiscoverCharacteristic(CBService *Service,NSError *Error)
{
	fCharacteriscCheckedIndex++;
	fPeripheralCommandInProgress=false;

	NotifyPeripheralStateProcedure();
}
//---------------------------------------------------------------------------
void cBluetoothLEConectionPeripheral::ocOnUpdateCharacteristValue(CBCharacteristic *Characteristic)
{
	auto Pair=fConnectDeviceMap.GetPair(Characteristic);
	if(Pair==nullptr)
		return;

	Pair->Value->onUpdateCharacteristicValue();
}
//---------------------------------------------------------------------------
void cBluetoothLEConectionPeripheral::ocOnUpdateNotificationState(CBCharacteristic *Characteristic,NSError *Error)
{
	auto Pair=fConnectDeviceMap.GetPair(Characteristic);
	if(Pair==nullptr)
		return;

	Pair->Value->onDeviceRegisterNotification(Error!=nil);
}
//---------------------------------------------------------------------------
void cBluetoothLEConectionPeripheral::BLEDeviceActive(cBluetoothLEConectionDevice *Device)
{
	if(fActiveDevice.Insert(Device)==false)
		return;

	fActive=true;

	PeripheralStateProcedure();
}
//---------------------------------------------------------------------------
void cBluetoothLEConectionPeripheral::BLEDeviceInactive(cBluetoothLEConectionDevice *Device)
{
	if(fActiveDevice.Remove(Device)==false)
		return;

	if(fActiveDevice.GetCount()==0){
		fActive=false;

		PeripheralStateProcedure();
	}
}
//---------------------------------------------------------------------------
void cBluetoothLEConectionPeripheral::BLEDeviceConnect(CBCharacteristic *ReadCharacteristic,cBluetoothLEConectionDevice *Device)
{
	fConnectDeviceMap[ReadCharacteristic]=Device;

	// register
	[fPeripheral setNotifyValue:YES forCharacteristic:ReadCharacteristic];
}
//---------------------------------------------------------------------------
void cBluetoothLEConectionPeripheral::BLEDeviceDisconnect(CBCharacteristic *ReadCharacteristic,cBluetoothLEConectionDevice *Device)
{
	auto Pair=fConnectDeviceMap.GetPair(ReadCharacteristic);
	if(Pair==nullptr)
		return;

	if(Pair->Value!=Device)
		return;

	// unregister
	[fPeripheral setNotifyValue:NO forCharacteristic:ReadCharacteristic];
	fConnectDeviceMap.RemovePair(Pair);
}
//---------------------------------------------------------------------------
void cBluetoothLEConectionPeripheral::NotifyPeripheralStateProcedure(void)
{
	if(fStateProcExclusiveFlag.Acquire()==false)
		return;

	PeripheralStateProcedure();
}
//---------------------------------------------------------------------------
void cBluetoothLEConectionPeripheral::PeripheralStateProcedure(void)
{
	do{
		fStateProcExclusiveFlag.Continue();

		if(fPeripheralCommandInProgress){
			continue;
		}

		if(fActive || fNeedScanService){
			PeripheralStateProcedure_Active();
		}
		else{
			PeripheralStateProcedure_Inactive();
		}

	}while(fStateProcExclusiveFlag.Release()==false);
}
//---------------------------------------------------------------------------
void cBluetoothLEConectionPeripheral::PeripheralStateProcedure_Active(void)
{
	if(fConnected==false){
		fPeripheralCommandInProgress=true;
		[fProtocol.centralManager connectPeripheral:fPeripheral options:nil];
		return;
	}
	if(fServiceChecked==false){
		fPeripheralCommandInProgress=true;
		[fPeripheral discoverServices:nil];
		return;
	}
	if(fCharacteriscCheckedIndex<fPeripheral.services.count){
		fPeripheralCommandInProgress=true;
		CBService *Service=[fPeripheral.services objectAtIndex:fCharacteriscCheckedIndex];
		[fPeripheral discoverCharacteristics:nil forService:Service];
		return;
	}

	if(fNeedScanService){
		fNeedScanService=false;

		auto CPPProtocol=CPPBluetoothLEConectionProtocol_10_7(fProtocol);
		CPPProtocol->PerpheralChecked(fConnectionPeripheral);

	}

	for(auto &Device : fActiveDevice){
		Device->onDeviceChecked();
	}
}
//---------------------------------------------------------------------------
void cBluetoothLEConectionPeripheral::PeripheralStateProcedure_Inactive(void)
{
	if(fConnected){
		fPeripheralCommandInProgress=true;
		[fProtocol.centralManager cancelPeripheralConnection:fPeripheral];
		return;
	}
}
//---------------------------------------------------------------------------
//@interface cnLib_BluetoothLEConnectionProtocol()<>
//@end

@implementation cnLib_BluetoothLEConnectionProtocol
{
@private
	cnRTL::ocStaticIVar<cBluetoothLEConectionProtocol_10_9> cnLib_IVar;
}

static bcBluetoothLEConectionProtocol* CPPBluetoothLEConectionProtocol(cnLib_BluetoothLEConnectionProtocol *self)
{
	return reinterpret_cast<bcBluetoothLEConectionProtocol*>(&self->cnLib_IVar);
}
static cBluetoothLEConectionProtocol_10_7* CPPBluetoothLEConectionProtocol_10_7(cnLib_BluetoothLEConnectionProtocol *self)
	API_AVAILABLE(macos(10.7))
{
	return reinterpret_cast<cBluetoothLEConectionProtocol_10_7*>(&self->cnLib_IVar);
}
static cBluetoothLEConectionProtocol_10_9* CPPBluetoothLEConectionProtocol_10_9(cnLib_BluetoothLEConnectionProtocol *self)
	API_AVAILABLE(macos(10.9))
{
	return reinterpret_cast<cBluetoothLEConectionProtocol_10_9*>(&self->cnLib_IVar);
}

-(instancetype)init
{
	self=[super init];
	if(self){
		if(@available(macOS 10.9, *)){
			cnVar::ManualConstruct(*CPPBluetoothLEConectionProtocol_10_9(self),self);
		}
		else if(@available(macOS 10.7, *)){
			cnVar::ManualConstruct(*CPPBluetoothLEConectionProtocol_10_7(self),self);
		}
		else{
			cnVar::ManualConstruct(*CPPBluetoothLEConectionProtocol(self),self);
		}
	}
	return self;
}

-(void)dealloc
{
	if(@available(macOS 10.9, *)){
		cnVar::ManualDestruct(*CPPBluetoothLEConectionProtocol_10_9(self));
	}
	else if(@available(macOS 10.7, *)){
		cnVar::ManualDestruct(*CPPBluetoothLEConectionProtocol_10_7(self));
	}
	else{
		cnVar::ManualDestruct(*CPPBluetoothLEConectionProtocol(self));
	}

	[super dealloc];
}

-(cnLibrary::iConnectionProtocol*)interfaceConnectionProtocol
{
	return CPPBluetoothLEConectionProtocol(self);
}


-(iPtr<iBluetoothLEConnectionBrowser>)createScanner:(cBluetoothLEConnectionServiceID)ServiceID
{
	auto CPP=CPPBluetoothLEConectionProtocol_10_7(self);
	return CPP->CreateBrowser(&ServiceID,1);
}

-(iPtr<cnLibrary::iBluetoothLEConnectionBrowser>)createScanner:(const cBluetoothLEConnectionServiceID*)ServiceIDs length:(uIntn)Length
{
	auto CPP=CPPBluetoothLEConectionProtocol_10_7(self);
	return CPP->CreateBrowser(ServiceIDs,Length);
}


#pragma mark CentralManager delegate

-(void)centralManagerDidUpdateState:(CBCentralManager *)central
{
	auto CPP=CPPBluetoothLEConectionProtocol_10_7(self);
	CPP->BLEOnCentralStateChanged();
}

//-(void)centralManager:(CBCentralManager *)central willRestoreState:(NSDictionary<NSString *, id> *)dict;

-(void)centralManager:(CBCentralManager *)central didDiscoverPeripheral:(CBPeripheral *)peripheral advertisementData:(NSDictionary *)advertisementData RSSI:(NSNumber *)RSSI
{
	auto CPP=CPPBluetoothLEConectionProtocol_10_7(self);
	CPP->BLEOnDiscovered(peripheral,advertisementData,RSSI);
}

-(void)centralManager:(CBCentralManager *)central didConnectPeripheral:(CBPeripheral *)peripheral
{
	auto CPP=CPPBluetoothLEConectionProtocol_10_7(self);
	CPP->BLEOnConnected(peripheral);
}

-(void)centralManager:(CBCentralManager *)central didFailToConnectPeripheral:(CBPeripheral *)peripheral error:(NSError *)error
{
	auto CPP=CPPBluetoothLEConectionProtocol_10_7(self);
	CPP->BLEOnConnectFailed(peripheral,error);
}


-(void)centralManager:(CBCentralManager *)central didDisconnectPeripheral:(CBPeripheral *)peripheral error:(NSError *)error
{
	auto CPP=CPPBluetoothLEConectionProtocol_10_7(self);
	CPP->BLEOnDisconnected(peripheral,error);
}

//-(void)centralManager:(CBCentralManager *)central connectionEventDidOccur:(CBConnectionEvent)event forPeripheral:(CBPeripheral *)peripheral NS_AVAILABLE_IOS(13_0);
//-(void)centralManager:(CBCentralManager *)central didUpdateANCSAuthorizationForPeripheral:(CBPeripheral *)peripheral NS_AVAILABLE_IOS(13_0);


#pragma mark PeripheralManager delegate

-(void)peripheralManagerDidUpdateState:(CBPeripheralManager *)peripheral
	API_AVAILABLE(macos(10.9))
{
	if(peripheral!=self.peripheralManager)
		return;
	auto CPP=CPPBluetoothLEConectionProtocol_10_9(self);
	CPP->BLEOnPeripheralStateChanged();
}

//-(void)peripheralManager:(CBPeripheralManager *)peripheral willRestoreState:(NSDictionary<NSString *, id> *)dict
//-(void)peripheralManagerDidStartAdvertising:(CBPeripheralManager *)peripheral error:(nullable NSError *)error
-(void)peripheralManager:(CBPeripheralManager *)peripheral didAddService:(CBService *)service error:(nullable NSError *)error
	API_AVAILABLE(macos(10.9))
{
	if(peripheral!=self.peripheralManager)
		return;
	auto CPP=CPPBluetoothLEConectionProtocol_10_9(self);
	CPP->BLEOnAddService(ocSafeCast<CBMutableService>(service),error);
}

-(void)peripheralManager:(CBPeripheralManager *)peripheral central:(CBCentral *)central didSubscribeToCharacteristic:(CBCharacteristic *)characteristic
	API_AVAILABLE(macos(10.9))
{
	if(peripheral!=self.peripheralManager)
		return;
	auto CPP=CPPBluetoothLEConectionProtocol_10_9(self);
	CPP->BLEOnSubscribe(central,characteristic);
}

-(void)peripheralManager:(CBPeripheralManager *)peripheral central:(CBCentral *)central didUnsubscribeFromCharacteristic:(CBCharacteristic *)characteristic
	API_AVAILABLE(macos(10.9))
{
	if(peripheral!=self.peripheralManager)
		return;
	auto CPP=CPPBluetoothLEConectionProtocol_10_9(self);
	CPP->BLEOnUnsubscribe(central,characteristic);
}

-(void)peripheralManager:(CBPeripheralManager *)peripheral didReceiveReadRequest:(CBATTRequest *)request
	API_AVAILABLE(macos(10.9))
{
	if(peripheral!=self.peripheralManager){
		[peripheral respondToRequest:request withResult:CBATTErrorInvalidHandle];
		return;
	}
}

-(void)peripheralManager:(CBPeripheralManager *)peripheral didReceiveWriteRequests:(NSArray<CBATTRequest *> *)requests
	API_AVAILABLE(macos(10.9))
{
	if(peripheral!=self.peripheralManager){
		[peripheral respondToRequest:requests[0] withResult:CBATTErrorInvalidHandle];
		return;
	}
	auto CPP=CPPBluetoothLEConectionProtocol_10_9(self);
	CPP->BLEOnWriteRequests(requests);
}

-(void)peripheralManagerIsReadyToUpdateSubscribers:(CBPeripheralManager *)peripheral
	API_AVAILABLE(macos(10.9))
{
	auto CPP=CPPBluetoothLEConectionProtocol_10_9(self);
	CPP->BLEOnUpdateSubscribers();
}

//- (void)peripheralManager:(CBPeripheralManager *)peripheral didPublishL2CAPChannel:(CBL2CAPPSM)PSM error:(nullable NSError *)error;
//- (void)peripheralManager:(CBPeripheralManager *)peripheral didUnpublishL2CAPChannel:(CBL2CAPPSM)PSM error:(nullable NSError *)error;
//- (void)peripheralManager:(CBPeripheralManager *)peripheral didOpenL2CAPChannel:(nullable CBL2CAPChannel *)channel error:(nullable NSError *)error;


-(void)updatePeripheralAdvertisementData API_AVAILABLE(macos(10.9))
{
	auto CPP=CPPBluetoothLEConectionProtocol_10_9(self);
	CPP->BLEOnUpdateAdvertisementData();
}

#pragma mark subclass

-(dispatch_queue_t)queue
{
	return dispatch_get_main_queue();
}

-(CBCentralManager *)centralManager
{
	return nil;
}

-(CBPeripheralManager *)peripheralManager
	API_AVAILABLE(macos(10.9))
{
	return nil;
}

-(NSDictionary *)peripheralAdvertisementData
{
	return nil;
}


#if	0

-(NSString*)deviceNameAt:(NSUInteger)index
{
	auto Lock=ScopeLock(BTSetLock);
	auto P=BTSet[index];
	auto A=BTAdvSet[index];
	NSString *Name=[A objectForKey:CBAdvertisementDataLocalNameKey];
	if(Name!=nil)
		return Name;
	return P.name;
}


-(void)applyKnownPeer
{
	if(knownPeer!=nil){
		auto KnownArray=@[knownPeer];

		auto KnownPer=[cbm retrievePeripheralsWithIdentifiers:KnownArray];
		if(KnownPer.count!=0){
			CBPeripheral *KnowPeripheral=KnownPer[0];
			[self setTargetPeripheral:KnowPeripheral advertisementData:nil];
		}

	}
}


-(void)setTargetPeripheral:(CBPeripheral *)peripheral advertisementData:(NSDictionary*)advdata
{
	if(ActiveDevice!=nil){
		auto PrevDeivce=ActiveDevice;
		ActiveDevice=nil;
		[cbm cancelPeripheralConnection:PrevDeivce.peripheral];
		[PrevDeivce closed];
	}
	if(peripheral!=nil){
		ActiveDevice=[[ASBLEDevice alloc]initWithPeripheral:peripheral advertisementData:advdata];
		ActiveDevice.host=self;

		[cbm connectPeripheral:peripheral options:nil];
	}
	[_delegate onDeviceChanged];
}


#pragma mark manager delegate


-(void)cbmUpdateState
{
	auto state=cbm.state;
	if(state!=CBManagerStatePoweredOn){
		if(ActiveDevice!=nil){
			[ActiveDevice closed];
			ActiveDevice=nil;
			[_delegate onDeviceChanged];
		}
	}
	else if(state==CBManagerStatePoweredOn){
		//[self applyKnownPeer];
		[self applyScan];
	}

	switch(state){
	case CBCentralManagerStatePoweredOn:
		[ScanVC disableMsg:nil];
		break;
	case CBCentralManagerStatePoweredOff:
		[ScanVC disableMsg:NSLocalizedString(@"蓝牙未打开",0)];
		break;
	case CBCentralManagerStateUnauthorized:
		[ScanVC disableMsg:NSLocalizedString(@"蓝牙被禁用",0)];
		break;
	case CBCentralManagerStateUnsupported:
		[ScanVC disableMsg:NSLocalizedString(@"该机器不支持蓝牙",0)];
		break;
	default:
		[ScanVC disableMsg:NSLocalizedString(@"未知蓝牙错误",0)];
		break;
	}
}



-(void)onModelOnSelected
{
	if(ActiveDevice==nullptr)
		return;
	[ScanNav dismissViewControllerAnimated:YES completion:nil];

	auto Selection=ModelVC.selection;

	[ActiveDevice selectMode:Selection];
	//knownPeer=device.peripheral.identifier;
	[_delegate onDeviceChanged];
	[_delegate onDeviceModelSelected];

}
-(void)onModelCancel
{
	if(ActiveDevice!=nil){
		[cbm cancelPeripheralConnection:ActiveDevice.peripheral];
		[ActiveDevice closed];
		ActiveDevice=nil;
	}
}

-(void)deviceSelectModel:(ASBLEDevice *)device  modelArray:(NSArray*)ModelArray
{
	if(ActiveDevice!=device)
		return;

	ModelVC.models=ModelArray;
	ModelVC.selection=0;

	// to select model
	[ScanNav pushViewController:ModelVC animated:YES];
}

-(void)deviceVerified:(ASBLEDevice*)device
{
	if(ActiveDevice!=device)
		return;
	[_delegate onDeviceConnected];
}

-(void)deviceVerifyError:(ASBLEDevice*)device
{
	if(ActiveDevice!=device)
		return;

	[cbm cancelPeripheralConnection:ActiveDevice.peripheral];
	[ActiveDevice disconnected];


	knownPeer=nil;

	dispatch_async(dispatch_get_main_queue(), ^{
		ActiveDevice=nil;
		[ScanVC deviceError];
		[_delegate onDeviceChanged];
	});

}

-(void)deviceError:(ASBLEDevice *)device error:(NSError *)error
{
	if(ActiveDevice!=device)
		return;

	if(error!=nil){
		if(error.domain==CBErrorDomain){
			if(error.code==CBErrorNotConnected){
				// reconnect
				[cbm connectPeripheral:ActiveDevice.peripheral options:nil];
				return;
			}
		}
	}

	[cbm cancelPeripheralConnection:ActiveDevice.peripheral];
	[ActiveDevice closed];

	knownPeer=nil;

	dispatch_async(dispatch_get_main_queue(), ^{
		ActiveDevice=nil;
		[ScanVC deviceError];
		[_delegate onDeviceChanged];
	});
}

-(void)disconnectDevice
{
	[self setTargetPeripheral:nil advertisementData:nil];
}

-(void)retryConnect
{
	[cbm cancelPeripheralConnection:ActiveDevice.peripheral];
}
#endif
@end

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcBluetoothLEConectionProtocol::bcBluetoothLEConectionProtocol(cnLib_BluetoothLEConnectionProtocol *Protocol)
	: fProtocol(Protocol)
{
}
//---------------------------------------------------------------------------
bcBluetoothLEConectionProtocol::~bcBluetoothLEConectionProtocol()
{
}
//---------------------------------------------------------------------------
const void* bcBluetoothLEConectionProtocol::CastInterface(iTypeID InterfaceID)const
{
	return ImpCastInterface<iReference>(this,InterfaceID);
}
//---------------------------------------------------------------------------
void bcBluetoothLEConectionProtocol::IncReference(void)
{
	[fProtocol retain];
}
//---------------------------------------------------------------------------
void bcBluetoothLEConectionProtocol::DecReference(void)
{
	[fProtocol release];
}
//---------------------------------------------------------------------------
iPtr<iConnectionConnector>	bcBluetoothLEConectionProtocol::CreateStreamConnector(iAddress*,iAddress*)
{
	return nullptr;
}
//---------------------------------------------------------------------------
iPtr<iConnectionListener>	bcBluetoothLEConectionProtocol::CreateStreamListener(iAddress*)
{
	return nullptr;
}
//---------------------------------------------------------------------------
iPtr<iConnectionQueue>		bcBluetoothLEConectionProtocol::CreateStreamConnectionQueue(iAddress*)
{
	return nullptr;
}
//---------------------------------------------------------------------------
iPtr<iConnectionConnector>	bcBluetoothLEConectionProtocol::CreateEndpointConnector(iAddress*,iAddress*)
{
	return nullptr;
}
//---------------------------------------------------------------------------
iPtr<iConnectionListener>	bcBluetoothLEConectionProtocol::CreateEndpointListener(iAddress*)
{
	return nullptr;
}
//---------------------------------------------------------------------------
iPtr<iConnectionQueue>		bcBluetoothLEConectionProtocol::CreateEndpointConnectionQueue(iAddress*)
{
	return nullptr;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cBluetoothLEConectionProtocol_10_7::cBluetoothLEConectionProtocol_10_7(cnLib_BluetoothLEConnectionProtocol *Protocol)
	: bcBluetoothLEConectionProtocol(Protocol)
{
}
//---------------------------------------------------------------------------
cBluetoothLEConectionProtocol_10_7::~cBluetoothLEConectionProtocol_10_7()
{
}
//---------------------------------------------------------------------------
iPtr<iBluetoothLEConnectionBrowser> cBluetoothLEConectionProtocol_10_7::CreateBrowser(const cBluetoothLEConnectionServiceID *Services,uIntn ServiceCount)
{
	//bool Exists;
	//auto Pair=fBrowserMap.InsertPair(cUUID(ID.ServiceUUID),Exists);
	//if(Exists){
	//	return nullptr;
	//}

	auto Browser=iCreate<cBluetoothLEConnectionDeviceBrowser>(fProtocol,Services,ServiceCount);

/*	Browser->Services.SetLength(ServiceCount);
	auto BrowserServices=Browser->Services.Pointer;
	for(uIntn i=0;i<ServiceCount;i++){
		auto &ServiceID=Services[i];
		auto &BrowserSet=fBrowserMap[cUUID(ServiceID.ServiceUUID)];
		BrowserSet.Insert(Browser);

		BrowserServices[i].ID=ServiceID;
		BrowserServices[i].ServiceID=CreateCBUUIDFromData(ServiceID.ServiceUUID);
		BrowserServices[i].ReadCharID=CreateCBUUIDFromData(ServiceID.ReadCharacteristUUID);
		BrowserServices[i].WriteCharID=CreateCBUUIDFromData(ServiceID.WriteCharacteristUUID);
	}*/

	//fBrowserSet.Insert(Browser);

	return Browser;
}
//---------------------------------------------------------------------------
void cBluetoothLEConectionProtocol_10_7::BrowserClosed(cBluetoothLEConnectionDeviceBrowser *Browser)
{
}
//---------------------------------------------------------------------------
iPtr<iConnectionConnector> cBluetoothLEConectionProtocol_10_7::CreateStreamConnector(iAddress *_Nullable LocalAddress,iAddress *_Nullable RemoteAddress)
{
	return nullptr;
}
//---------------------------------------------------------------------------
iPtr<iConnectionConnector> cBluetoothLEConectionProtocol_10_7::CreateEndpointConnector(iAddress *_Nullable LocalAddress,iAddress *_Nullable RemoteAddress)
{
	return nullptr;
}
//---------------------------------------------------------------------------
/*
iPtr<iConnectionTask> cBluetoothLEConectionProtocol_10_7::ConnectAsync(iTypeID ConnectionIID,iAddress *RemoteAddress,iAddress *LocalAddress)
{
	if(LocalAddress!=nullptr)
		return nullptr;
	auto BLEDevice=iCast<cBluetoothLEConectionDevice>(RemoteAddress);
	if(BLEDevice==nullptr){
		return nullptr;
	}

	return BLEDevice->ConnectAsync(ConnectionIID);
}
*/
//---------------------------------------------------------------------------
void cBluetoothLEConectionProtocol_10_7::BLEOnCentralStateChanged(void)
{
	auto State=fProtocol.centralManager.state;
	bool Activated;
	if(@available(macos 10.13,*)){
		Activated=(State==CBManagerStatePoweredOn);
	}
	else{
		Activated=(static_cast<CBCentralManagerState>(State)==CBCentralManagerStatePoweredOn);
	}

	if(fBLECentralActivated!=Activated){
		if(Activated){
			if(fNeedScanDevice){
				StartScan();
			}
		}
		else{
			fScanStarted=false;
		}
		fBLECentralActivated=Activated;
	}
}
//---------------------------------------------------------------------------
void cBluetoothLEConectionProtocol_10_7::BLEOnDiscovered(CBPeripheral *peripheral,NSDictionary *advertisementData,NSNumber *RSSI)
{
	auto ConnectionPeripheral=[[cnLib_BluetoothLEConectionPeripheral alloc]init];
	auto CPPConnectionPeripheral=CPPBluetoothLEConectionPeripheral(ConnectionPeripheral);

	CPPConnectionPeripheral->Initialize(fProtocol,peripheral);

	CPPConnectionPeripheral->BLEUpdateRSSI(RSSI);
	CPPConnectionPeripheral->BLEUpdateADData(advertisementData);

	fPeripherals.Insert(ocPtrTake(ConnectionPeripheral));
}
//---------------------------------------------------------------------------
void cBluetoothLEConectionProtocol_10_7::BLEOnConnected(CBPeripheral *Peripheral)
{
	auto ConnectionPeripheral=ocSafeCast<cnLib_BluetoothLEConectionPeripheral>(Peripheral.delegate);
	if(ConnectionPeripheral==nil)
		return;

	auto CPPConnectionPeripheral=CPPBluetoothLEConectionPeripheral(ConnectionPeripheral);

	CPPConnectionPeripheral->BLEOnConnected();
/*
	auto ConnectPair=fConnectionTaskMap.GetPair(Peripheral);
	if(ConnectPair==nullptr)
		return;

	auto Task=cnVar::MoveCast(ConnectPair->Value);
	fConnectionTaskMap.RemovePair(ConnectPair);

	auto Endpoint=[Task->Device createEndpoint];
	if(Endpoint!=nullptr){
		//Task->Connection=cnRTL::CreateConnectionFromEndpoint(Task->ConnectionTypeID,Endpoint,
		//	nullptr,[Task->Device cnLib_AddressInterface],false);
	}
	Task->TaskState.SetDone();*/
}
//---------------------------------------------------------------------------
void cBluetoothLEConectionProtocol_10_7::BLEOnConnectFailed(CBPeripheral *Peripheral,NSError *Error)
{
	auto ConnectionPeripheral=ocSafeCast<cnLib_BluetoothLEConectionPeripheral>(Peripheral.delegate);
	if(ConnectionPeripheral==nil)
		return;

	auto CPPConnectionPeripheral=CPPBluetoothLEConectionPeripheral(ConnectionPeripheral);

	CPPConnectionPeripheral->BLEOnConnectFailed();

/*
	auto ConnectPair=fConnectionTaskMap.GetPair(Peripheral);
	if(ConnectPair==nullptr)
		return;

	auto Task=cnVar::MoveCast(ConnectPair->Value);
	fConnectionTaskMap.RemovePair(ConnectPair);

	Task->Connection=nullptr;
	Task->TaskState.SetDone();

	*/
}
//---------------------------------------------------------------------------
void cBluetoothLEConectionProtocol_10_7::BLEOnDisconnected(CBPeripheral *Peripheral,NSError *Error)
{
	auto ConnectionPeripheral=ocSafeCast<cnLib_BluetoothLEConectionPeripheral>(Peripheral.delegate);
	if(ConnectionPeripheral==nil)
		return;

	auto CPPConnectionPeripheral=CPPBluetoothLEConectionPeripheral(ConnectionPeripheral);

	CPPConnectionPeripheral->BLEOnDisconnected();
}
//---------------------------------------------------------------------------
void cBluetoothLEConectionProtocol_10_7::PerpheralChecked(cnLib_BluetoothLEConectionPeripheral *ConnectionPeripheral)
{
	auto CPPConnectionPeripheral=CPPBluetoothLEConectionPeripheral(ConnectionPeripheral);
	auto Peripheral=CPPConnectionPeripheral->GetPeripheral();


	for(auto &Browser : fBrowserSet){
		Browser->BLEOnDeviceChecked(ConnectionPeripheral);
	}
}
//---------------------------------------------------------------------------
void cBluetoothLEConectionProtocol_10_7::BrowserStarted(cBluetoothLEConnectionDeviceBrowser *Browser)
{
	if(fBrowserSet.Insert(Browser)==false)
		return;

	auto BrowserServiceIDs=Browser->GetServiceIDs();
	for(uIntn i=0;i<BrowserServiceIDs.Length;i++){
		auto &ServiceID=BrowserServiceIDs.Pointer[i];

		auto &ServiceIDItem=fBrowserServiceIDMap[ServiceID->ID.ServiceUUID];
		if(ServiceIDItem.Count++==0){
			ServiceIDItem.IDPtr=ServiceID->ServiceID;
			fBrowserScanServiceArray=nil;
		}
	}

	if(fBLECentralActivated){
		fScanStarted=false;
		fNeedScanDevice=true;
		StartScan();
	}
}
//---------------------------------------------------------------------------
void cBluetoothLEConectionProtocol_10_7::BrowserStopped(cBluetoothLEConnectionDeviceBrowser *Browser)
{
	if(fBrowserSet.Remove(Browser)==false)
		return;

	auto BrowserServiceIDs=Browser->GetServiceIDs();
	for(uIntn i=0;i<BrowserServiceIDs.Length;i++){
		auto &ServiceID=BrowserServiceIDs.Pointer[i];

		auto ServiceMapPair=fBrowserServiceIDMap.GetPair(ServiceID->ID.ServiceUUID);
		if(ServiceMapPair!=nullptr){
			ServiceMapPair->Value.Count--;
			if(ServiceMapPair->Value.Count==0){
				fBrowserServiceIDMap.RemovePair(ServiceMapPair);
				fBrowserScanServiceArray=nil;
			}
		}
	}

	if(fBLECentralActivated){
		if(fBrowserServiceIDMap.GetCount()==0){
			fNeedScanDevice=false;
			StopScan();
		}
	}
}
//---------------------------------------------------------------------------
void cBluetoothLEConectionProtocol_10_7::StartScan(void)
{
	if(fScanStarted==false){
		auto CentralManager=fProtocol.centralManager;
		if(fBrowserScanServiceArray==nullptr){
			cArrayBlock<CBUUID*> TempArray;
			TempArray.SetLength(fBrowserServiceIDMap.GetCount());
			uIntn i=0;
			for(auto p : fBrowserServiceIDMap){
				TempArray.Pointer[i++]=p.Value.IDPtr;
			}

			fBrowserScanServiceArray=[[NSArray alloc]initWithObjects:TempArray.Pointer count:TempArray.Length];
		}
		[CentralManager scanForPeripheralsWithServices:fBrowserScanServiceArray options:nil];
		fScanStarted=true;
	}
}
//---------------------------------------------------------------------------
void cBluetoothLEConectionProtocol_10_7::StopScan(void)
{
	if(fScanStarted){
		auto CentralManager=fProtocol.centralManager;
		[CentralManager stopScan];
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cBluetoothLEConectionProtocol_10_9::cBluetoothLEConectionProtocol_10_9(cnLib_BluetoothLEConnectionProtocol *Protocol)
	: cBluetoothLEConectionProtocol_10_7(Protocol)
{
	fBLEPeripheralActivated=false;
	fBLEPeripheralAdvertisementLockCount=1;
}
//---------------------------------------------------------------------------
cBluetoothLEConectionProtocol_10_9::~cBluetoothLEConectionProtocol_10_9()
{
}
//---------------------------------------------------------------------------
iPtr<iConnectionListener> cBluetoothLEConectionProtocol_10_9::CreateStreamListener(iAddress *_Nullable LocalAddress)
{

}
//---------------------------------------------------------------------------
iPtr<iConnectionQueue> cBluetoothLEConectionProtocol_10_9::CreateStreamConnectionQueue(iAddress *_Nullable LocalAddress)
{
}
//---------------------------------------------------------------------------
iPtr<iConnectionListener> cBluetoothLEConectionProtocol_10_9::CreateEndpointListener(iAddress *_Nullable LocalAddress)
{
}
//---------------------------------------------------------------------------
iPtr<iConnectionQueue> cBluetoothLEConectionProtocol_10_9::CreateEndpointConnectionQueue(iAddress *_Nullable LocalAddress)
{
	if(@available(macOS 10.9, *)){
		if(LocalAddress==nullptr)
			return nullptr;

		if(fProtocol.peripheralManager==nil)
			return nullptr;

		auto BLEService=ociSafeCast<cnLib_BluetoothLEConnectionService>(LocalAddress);
		if(BLEService==nil)
			return nullptr;

		auto ConnectionQueue=iCreate<cBluetoothLEConnectionQueue>(fProtocol,BLEService);
		//__block iInnerPtr<cBluetoothLEConnectionQueue> QueuePtr=ConnectionQueue;
		dispatch_async(fProtocol.queue, ^{
			//QueueListenStarted(cnVar::MoveCast(QueuePtr));
			QueueListenStarted(ConnectionQueue);
		});
		return ConnectionQueue;
	}
	else{
		// not supported
		return nullptr;
	}
}
//---------------------------------------------------------------------------
void cBluetoothLEConectionProtocol_10_9::QueueListenStarted(iInnerPtr<cBluetoothLEConnectionQueue> Queue)
{
	auto PeripheralManager=fProtocol.peripheralManager;
	auto BLEService=Queue->GetService();
	auto Service=BLEService.service;
	if(fBLEPeripheralActivated){
		PauseAdvertisement();
		[PeripheralManager addService:Service];
	}
	else{
		fServiceAddSet.Insert(Queue);
	}
	if(fServiceMap.GetCount()==0){
		ResumeAdvertisement();
	}
	fServiceMap[Service]=cnVar::MoveCast(Queue);
}
//---------------------------------------------------------------------------
void cBluetoothLEConectionProtocol_10_9::QueueListenStopped(cBluetoothLEConnectionQueue *Queue)
{
	auto PeripheralManager=fProtocol.peripheralManager;
	auto BLEService=Queue->GetService();
	auto Service=BLEService.service;

	if(fServiceAddSet.Remove(Queue)==false){
		// service added

		[PeripheralManager removeService:Service];
	}
	else{
		// service not added
		ResumeAdvertisement();
	}

	fServiceMap.Remove(Service);
	if(fServiceMap.GetCount()==0){
		PauseAdvertisement();
	}
}
//---------------------------------------------------------------------------
void cBluetoothLEConectionProtocol_10_9::BLEOnPeripheralStateChanged(void)
{
	auto State=fProtocol.peripheralManager.state;
	bool Activate;
	if(@available(macos 10.13,*)){
		Activate=(State==CBManagerStatePoweredOn);
	}
	else{
		Activate=(static_cast<CBPeripheralManagerState>(State)==CBPeripheralManagerStatePoweredOn);
	}

	if(fBLEPeripheralActivated!=Activate){
		if(Activate){
			SetupPeripheral();
		}
		else{
			ClearPeripheral();
		}
		fBLEPeripheralActivated=Activate;
	}
}
//---------------------------------------------------------------------------
void cBluetoothLEConectionProtocol_10_9::SetupPeripheral(void)
{
	auto PeripheralManager=fProtocol.peripheralManager;

	for(auto Service : fServiceAddSet){
		auto BLEService=Service->GetService();
		[PeripheralManager addService:BLEService.service];

		PauseAdvertisement();
	}
	fServiceAddSet.Clear();

	if(fBLENeedStartAdvertisement && fBLEPeripheralAdvertisementLockCount==0){
		fBLENeedStartAdvertisement=false;
		auto ADData=CreateADData();
		[fProtocol.peripheralManager startAdvertising:ADData];
		[ADData release];
	}
}
//---------------------------------------------------------------------------
void cBluetoothLEConectionProtocol_10_9::ClearPeripheral(void)
{
	for(auto pService : fServiceMap){
		pService.Value->OnPeripheralClosed();
	}
}
//---------------------------------------------------------------------------
NSDictionary* cBluetoothLEConectionProtocol_10_9::CreateADData(void)
{
	auto Services=fServiceMap.GetKeys();
	auto ServiceCount=fServiceMap.GetCount();
	auto ServiceArray=[[NSMutableArray alloc]initWithCapacity:ServiceCount];
	for(uIntn i=0;i<ServiceCount;i++){
		[ServiceArray addObject:Services[i].UUID];
	}
	auto ADData=[[NSMutableDictionary alloc]initWithDictionary:fProtocol.peripheralAdvertisementData];
	ADData[CBAdvertisementDataServiceUUIDsKey]=ServiceArray;
	return ADData;
}
//---------------------------------------------------------------------------
void cBluetoothLEConectionProtocol_10_9::ResumeAdvertisement(void)
{
	fBLEPeripheralAdvertisementLockCount--;
	if(fBLEPeripheralAdvertisementLockCount==0){
		if(fBLEPeripheralActivated){
			auto ADData=CreateADData();
			[fProtocol.peripheralManager startAdvertising:ADData];
			[ADData release];
		}
		else{
			fBLENeedStartAdvertisement=true;
		}
	}
}
//---------------------------------------------------------------------------
void cBluetoothLEConectionProtocol_10_9::PauseAdvertisement(void)
{
	if(fBLEPeripheralActivated && fBLEPeripheralAdvertisementLockCount==0){
		fBLENeedStartAdvertisement=false;
		[fProtocol.peripheralManager stopAdvertising];
	}
	fBLEPeripheralAdvertisementLockCount++;
}
//---------------------------------------------------------------------------
void cBluetoothLEConectionProtocol_10_9::BLEOnUpdateAdvertisementData(void)
{
	if(fBLEPeripheralActivated){
		if(fBLEPeripheralAdvertisementLockCount==0){
			auto ADData=CreateADData();
			[fProtocol.peripheralManager startAdvertising:ADData];
			[ADData release];
		}
	}
	else{
		fBLENeedStartAdvertisement=true;
	}
}
//---------------------------------------------------------------------------
void cBluetoothLEConectionProtocol_10_9::BLEOnAddService(CBMutableService *Service,NSError *Error)
{
	auto pService=fServiceMap.GetPair(Service);
	if(pService==nullptr)
		return;

	if(Error!=nil){
		// failed to add service
		pService->Value->OnAddServiceError(Error);
	}
	else{
		// successed
		ResumeAdvertisement();
	}
}
//---------------------------------------------------------------------------
void cBluetoothLEConectionProtocol_10_9::BLEOnSubscribe(CBCentral *RemoteCentral,CBCharacteristic *Characteristic)
{
	auto pService=fServiceMap.GetPair(Characteristic.service);
	if(pService==nullptr)
		return;

	auto BLEService=pService->Value->GetService();
	if(Characteristic!=BLEService.readCharacteristic){
		// not supported characteristic
		return;
	}

	pService->Value->OnCentralSubscribe(RemoteCentral);
}
//---------------------------------------------------------------------------
void cBluetoothLEConectionProtocol_10_9::BLEOnUnsubscribe(CBCentral *RemoteCentral,CBCharacteristic *Characteristic)
{
	auto pService=fServiceMap.GetPair(Characteristic.service);
	if(pService==nullptr)
		return;
	auto BLEService=pService->Value->GetService();
	if(Characteristic!=BLEService.readCharacteristic){
		// not supported characteristic
		return;
	}

	pService->Value->OnCentralUnsubscribe(RemoteCentral);
}
//---------------------------------------------------------------------------
CBATTError cBluetoothLEConectionProtocol_10_9::TestRequestsWritable(NSArray<CBATTRequest *>*Requests)
{
	for(NSUInteger i=0,c=Requests.count;i<c;i++){
		auto Request=Requests[i];
		auto pService=fServiceMap.GetPair(Request.characteristic.service);
		if(pService==nullptr){
			return CBATTErrorAttributeNotFound;
		}
		if(pService->Value->TestRequestWritable(Request)==false){
			return CBATTErrorWriteNotPermitted;
		}
	}
	return CBATTErrorSuccess;
}
//---------------------------------------------------------------------------
void cBluetoothLEConectionProtocol_10_9::BLEOnWriteRequests(NSArray<CBATTRequest *>*Requests)
{
	auto TestResult=TestRequestsWritable(Requests);

	if(TestResult!=CBATTErrorSuccess){
		[fProtocol.peripheralManager respondToRequest:Requests[0] withResult:TestResult];
		return;
	}
	auto pService=fServiceMap.GetPair(Requests[0].characteristic.service);
	if(pService==nullptr){
		[fProtocol.peripheralManager respondToRequest:Requests[0] withResult:CBATTErrorAttributeNotFound];
		return;
	}
	auto ConnectionQueue=pService->Value;
	for(NSUInteger i=0,c=Requests.count;i<c;i++){
		ConnectionQueue->OnCentralWrite(Requests[i]);
	}

	[fProtocol.peripheralManager respondToRequest:Requests[0] withResult:CBATTErrorSuccess];
}
//---------------------------------------------------------------------------
void cBluetoothLEConectionProtocol_10_9::BLEOnUpdateSubscribers(void)
{
	for(auto pServices : fServiceMap){
		pServices.Value->OnUpdateSubscribers();
	}
}
//---------------------------------------------------------------------------


#endif

