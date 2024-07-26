/*- Java - Android - Graphics ---------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2024-04-01                                          */
/*-------------------------------------------------------------------------*/
#pragma once
/*-------------------------------------------------------------------------*/
#include <Java/JNIJavaBase.hpp>
#include <Java/JNIContext.hpp>

//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace jCPP{
//---------------------------------------------------------------------------
namespace android{
//---------------------------------------------------------------------------
namespace bluetooth{
//---------------------------------------------------------------------------
struct jcBluetoothGattService;
struct jcBluetoothGattCharacteristic;
//---------------------------------------------------------------------------
struct jcBluetoothGattDescriptor : jcObject
{
	static constexpr const char jClassName[]="android/bluetooth/BluetoothGattDescriptor";

	static constexpr const char jname_DISABLE_NOTIFICATION_VALUE[]="DISABLE_NOTIFICATION_VALUE";
	static jStaticFieldAccess<jcArray<jbyte>*> DISABLE_NOTIFICATION_VALUE(void)noexcept{
		return jStaticFieldAccessMaker<jcBluetoothGattDescriptor,jname_DISABLE_NOTIFICATION_VALUE>();
	}
	static constexpr const char jname_ENABLE_INDICATION_VALUE[]="ENABLE_INDICATION_VALUE";
	static jStaticFieldAccess<jcArray<jbyte>*> ENABLE_INDICATION_VALUE(void)noexcept{
		return jStaticFieldAccessMaker<jcBluetoothGattDescriptor,jname_ENABLE_INDICATION_VALUE>();
	}
	static constexpr const char jname_ENABLE_NOTIFICATION_VALUE[]="ENABLE_NOTIFICATION_VALUE";
	static jStaticFieldAccess<jcArray<jbyte>*> ENABLE_NOTIFICATION_VALUE(void)noexcept{
		return jStaticFieldAccessMaker<jcBluetoothGattDescriptor,jname_ENABLE_NOTIFICATION_VALUE>();
	}
	

	static constexpr const char jname_getPermissions[]="getPermissions";
	jint getPermissions(JNIEnv *env)noexcept{
		return jMethodCall<jname_getPermissions,&jcBluetoothGattDescriptor::getPermissions>::Call(env,this);
	}

	static constexpr const char jname_getCharacteristic[]="getCharacteristic";
	jrLocal<jcBluetoothGattCharacteristic> getCharacteristic(JNIEnv *env)noexcept{
		return jMethodCall<jname_getCharacteristic,&jcBluetoothGattDescriptor::getCharacteristic>::Call(env,this);
	}

	static constexpr const char jname_getUuid[]="getUuid";
	jrLocal<java::util::jcUUID> getUuid(JNIEnv *env)noexcept{
		return jMethodCall<jname_getUuid,&jcBluetoothGattDescriptor::getUuid>::Call(env,this);
	}

	static constexpr const char jname_getValue[]="getValue";
	jrLocal< jcArray<jbyte> > getValue(JNIEnv *env)noexcept{
		return jMethodCall<jname_getValue,&jcBluetoothGattDescriptor::getValue>::Call(env,this);
	}

	static constexpr const char jname_setValue[]="setValue";
	jboolean setValue(JNIEnv *env,jcArray<jbyte> *value)noexcept{
		return jMethodCall<jname_setValue,&jcBluetoothGattDescriptor::setValue>::Call(env,this,value);
	}
};
//---------------------------------------------------------------------------
struct jcBluetoothGattCharacteristic : jcObject
{
	static constexpr const char jClassName[]="android/bluetooth/BluetoothGattCharacteristic";

	static constexpr jint WRITE_TYPE_NO_RESPONSE=1;
	static constexpr jint WRITE_TYPE_DEFAULT=2;
	static constexpr jint WRITE_TYPE_SIGNED=4;

	static constexpr const char jname_getDescriptors[]="getDescriptors";
	jrLocal< java::util::jcList<jcBluetoothGattDescriptor*> > getDescriptors(JNIEnv *env)noexcept{
		return jMethodCall<jname_getDescriptors,&jcBluetoothGattCharacteristic::getDescriptors>::Call(env,this);
	}
	
	static constexpr const char jname_getDescriptor[]="getDescriptor";
	jrLocal<jcBluetoothGattDescriptor> getDescriptor(JNIEnv *env,java::util::jcUUID *ID)noexcept{
		return jMethodCall<jname_getDescriptor,&jcBluetoothGattCharacteristic::getDescriptor>::Call(env,this,ID);
	}

	static constexpr const char jname_getPermissions[]="getPermissions";
	jint getPermissions(JNIEnv *env)noexcept{
		return jMethodCall<jname_getPermissions,&jcBluetoothGattCharacteristic::getPermissions>::Call(env,this);
	}

	static constexpr const char jname_getProperties[]="getProperties";
	jint getProperties(JNIEnv *env)noexcept{
		return jMethodCall<jname_getProperties,&jcBluetoothGattCharacteristic::getProperties>::Call(env,this);
	}

	static constexpr const char jname_getService[]="getService";
	jrLocal<jcBluetoothGattService> getService(JNIEnv *env)noexcept{
		return jMethodCall<jname_getService,&jcBluetoothGattCharacteristic::getService>::Call(env,this);
	}

	static constexpr const char jname_getUuid[]="getUuid";
	jrLocal<java::util::jcUUID> getUuid(JNIEnv *env)noexcept{
		return jMethodCall<jname_getUuid,&jcBluetoothGattCharacteristic::getUuid>::Call(env,this);
	}

	static constexpr const char jname_getValue[]="getValue";
	jrLocal< jcArray<jbyte> > getValue(JNIEnv *env)noexcept{
		return jMethodCall<jname_getValue,&jcBluetoothGattCharacteristic::getValue>::Call(env,this);
	}

	static constexpr const char jname_setValue[]="setValue";
	jboolean setValue(JNIEnv *env,jcArray<jbyte> *value)noexcept{
		return jMethodCall<jname_setValue,&jcBluetoothGattCharacteristic::setValue>::Call(env,this,value);
	}

	static constexpr const char jname_getWriteType[]="getWriteType";
	jint getWriteType(JNIEnv *env)noexcept{
		return jMethodCall<jname_getWriteType,&jcBluetoothGattCharacteristic::getWriteType>::Call(env,this);
	}
	static constexpr const char jname_setWriteType[]="setWriteType";
	void setWriteType(JNIEnv *env,jint writeType)noexcept{
		return jMethodCall<jname_setWriteType,&jcBluetoothGattCharacteristic::setWriteType>::Call(env,this,writeType);
	}
};
//---------------------------------------------------------------------------
struct jcBluetoothGattService : jcObject
{
	static constexpr const char jClassName[]="android/bluetooth/BluetoothGattService";

	static constexpr const char jname_getUuid[]="getUuid";
	jrLocal<java::util::jcUUID> getUuid(JNIEnv *env)noexcept{
		return jMethodCall<jname_getUuid,&jcBluetoothGattService::getUuid>::Call(env,this);
	}

	static constexpr const char jname_getCharacteristic[]="getCharacteristic";
	jrLocal<jcBluetoothGattCharacteristic> getCharacteristic(JNIEnv *env,java::util::jcUUID *ID)noexcept{
		return jMethodCall<jname_getCharacteristic,&jcBluetoothGattService::getCharacteristic>::Call(env,this,ID);
	}
	
	static constexpr const char jname_getCharacteristics[]="getCharacteristics";
	jrLocal< java::util::jcList<jcBluetoothGattCharacteristic*> > getCharacteristics(JNIEnv *env)noexcept{
		return jMethodCall<jname_getCharacteristics,&jcBluetoothGattService::getCharacteristics>::Call(env,this);
	}
};
//---------------------------------------------------------------------------
struct jcBluetoothGattCallback : jcObject
{
	static constexpr const char jClassName[]="android/bluetooth/BluetoothGattCallback";
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
struct jcBluetoothDevice;
//---------------------------------------------------------------------------
struct jcBluetoothGatt : jcObject
{
	static constexpr const char jClassName[]="android/bluetooth/BluetoothGatt";

	static constexpr jint GATT_SUCCESS						=0;
	static constexpr jint GATT_READ_NOT_PERMITTED			=2;
	static constexpr jint GATT_WRITE_NOT_PERMITTED			=3;
	static constexpr jint GATT_INSUFFICIENT_AUTHENTICATION	=5;
	static constexpr jint GATT_REQUEST_NOT_SUPPORTED		=6;
	static constexpr jint GATT_INVALID_OFFSET				=7;
	static constexpr jint GATT_INSUFFICIENT_AUTHORIZATION	=8;
	static constexpr jint GATT_INVALID_ATTRIBUTE_LENGTH		=13;
	static constexpr jint GATT_INSUFFICIENT_ENCRYPTION		=15;
	static constexpr jint GATT_ERROR						=0x85;
	static constexpr jint GATT_CONNECTION_CONGESTED			=0x8F;
	static constexpr jint GATT_CONNECTION_TIMEOUT			=0x93;
	static constexpr jint GATT_FAILURE						=0x101;

	static constexpr jint CONNECTION_PRIORITY_LOW_POWER	=2;

	static constexpr const char jname_close[]="close";
	void close(JNIEnv *env)noexcept{
		return jMethodCall<jname_close,&jcBluetoothGatt::close>::Call(env,this);
	}

	static constexpr const char jname_connect[]="connect";
	jboolean connect(JNIEnv *env)noexcept{
		return jMethodCall<jname_connect,&jcBluetoothGatt::connect>::Call(env,this);
	}

	static constexpr const char jname_disconnect[]="disconnect";
	void disconnect(JNIEnv *env)noexcept{
		return jMethodCall<jname_disconnect,&jcBluetoothGatt::disconnect>::Call(env,this);
	}
	
	static constexpr const char jname_discoverServices[]="discoverServices";
	jboolean discoverServices(JNIEnv *env)noexcept{
		return jMethodCall<jname_discoverServices,&jcBluetoothGatt::discoverServices>::Call(env,this);
	}

	static constexpr const char jname_getService[]="getService";
	jrLocal<jcBluetoothGattService> getService(JNIEnv *env,java::util::jcUUID *uid)noexcept{
		return jMethodCall<jname_getService,&jcBluetoothGatt::getService>::Call(env,this,uid);
	}

	static constexpr const char jname_getServices[]="getServices";
	jrLocal< java::util::jcList<jcBluetoothGattService*> > getServices(JNIEnv *env)noexcept{
		return jMethodCall<jname_getServices,&jcBluetoothGatt::getServices>::Call(env,this);
	}

	static constexpr const char jname_readCharacteristic[]="readCharacteristic";
	jboolean readCharacteristic(JNIEnv *env,jcBluetoothGattCharacteristic *characteristic)noexcept{
		return jMethodCall<jname_readCharacteristic,&jcBluetoothGatt::readCharacteristic>::Call(env,this,characteristic);
	}

	static constexpr const char jname_readDescriptor[]="readDescriptor";
	jboolean readDescriptor(JNIEnv *env,jcBluetoothGattDescriptor *descriptor)noexcept{
		return jMethodCall<jname_readDescriptor,&jcBluetoothGatt::readDescriptor>::Call(env,this,descriptor);
	}

	static constexpr const char jname_readRemoteRssi[]="readRemoteRssi";
	jboolean readRemoteRssi(JNIEnv *env)noexcept{
		return jMethodCall<jname_readRemoteRssi,&jcBluetoothGatt::readRemoteRssi>::Call(env,this);
	}

	static constexpr const char jname_setCharacteristicNotification[]="setCharacteristicNotification";
	jboolean setCharacteristicNotification(JNIEnv *env,jcBluetoothGattCharacteristic *characteristic,jboolean enable)noexcept{
		return jMethodCall<jname_setCharacteristicNotification,&jcBluetoothGatt::setCharacteristicNotification>::Call(env,this,characteristic,enable);
	}

	static constexpr const char jname_writeCharacteristic[]="writeCharacteristic";
	jboolean writeCharacteristic(JNIEnv *env,jcBluetoothGattCharacteristic *characteristic)noexcept{
		return jMethodCall<jname_writeCharacteristic,&jcBluetoothGatt::writeCharacteristic>::Call(env,this,characteristic);
	}

	static constexpr const char jname_writeDescriptor[]="writeDescriptor";
	jboolean writeDescriptor(JNIEnv *env,jcBluetoothGattDescriptor *descriptor)noexcept{
		return jMethodCall<jname_writeDescriptor,&jcBluetoothGatt::writeDescriptor>::Call(env,this,descriptor);
	}
};
//---------------------------------------------------------------------------
struct jcBluetoothDevice : jcObject
{
	static constexpr const char jClassName[]="android/bluetooth/BluetoothDevice";

	static constexpr const char jname_getName[]="getName";
	jrLocal<jcString> getName(JNIEnv *env)noexcept{
		return jMethodCall<jname_getName,&jcBluetoothDevice::getName>::Call(env,this);
	}

	static constexpr const char jname_getAddress[]="getAddress";
	jrLocal<jcString> getAddress(JNIEnv *env)noexcept{
		return jMethodCall<jname_getAddress,&jcBluetoothDevice::getAddress>::Call(env,this);
	}

	static constexpr const char jname_connectGatt18[]="connectGatt";
	jrLocal<jcBluetoothGatt> connectGatt18(JNIEnv *env,android::content::jcContext *Context,jboolean AutoConnect,jcBluetoothGattCallback *Callback)noexcept{
		return jMethodCall<jname_connectGatt18,&jcBluetoothDevice::connectGatt18>::Call(env,this,Context,AutoConnect,Callback);
	}

	static constexpr const char jname_connectGatt23[]="connectGatt";
	jrLocal<jcBluetoothGatt> connectGatt23(JNIEnv *env,android::content::jcContext *Context,jboolean AutoConnect,jcBluetoothGattCallback *Callback,jint Transport)noexcept{
		return jMethodCall<jname_connectGatt23,&jcBluetoothDevice::connectGatt23>::Call(env,this,Context,AutoConnect,Callback,Transport);
	}
};
//---------------------------------------------------------------------------
struct jcBluetoothLeScanRecord : jcObject
{
	static constexpr const char jClassName[]="android/bluetooth/le/ScanRecord";

	static constexpr const char jname_getBytes[]="getBytes";
	jrLocal< jcArray<jbyte> > getBytes(JNIEnv *env)noexcept{
		return jMethodCall<jname_getBytes,&jcBluetoothLeScanRecord::getBytes>::Call(env,this);
	}

	static constexpr const char jname_getDeviceName[]="getDeviceName";
	jrLocal<jcString> getDeviceName(JNIEnv *env)noexcept{
		return jMethodCall<jname_getDeviceName,&jcBluetoothLeScanRecord::getDeviceName>::Call(env,this);
	}

	static constexpr const char jname_getServiceUuids[]="getServiceUuids";
	jrLocal< java::util::jcList<java::util::jcParcelUuid*> > getServiceUuids(JNIEnv *env)noexcept{
		return jMethodCall<jname_getServiceUuids,&jcBluetoothLeScanRecord::getServiceUuids>::Call(env,this);
	}
};
//---------------------------------------------------------------------------
struct jcBluetoothLeScanResult : jcObject
{
	static constexpr const char jClassName[]="android/bluetooth/le/ScanResult";

	//static jMethod<jboolean	()> isConnectable;
	
	static constexpr const char jname_getRssi[]="getRssi";
	jint getRssi(JNIEnv *env)noexcept{
		return jMethodCall<jname_getRssi,&jcBluetoothLeScanResult::getRssi>::Call(env,this);
	}

	static constexpr const char jname_getDevice[]="getDevice";
	jrLocal<jcBluetoothDevice> getDevice(JNIEnv *env)noexcept{
		return jMethodCall<jname_getDevice,&jcBluetoothLeScanResult::getDevice>::Call(env,this);
	}
	
	static constexpr const char jname_getScanRecord[]="getScanRecord";
	jrLocal<jcBluetoothLeScanRecord> getScanRecord(JNIEnv *env)noexcept{
		return jMethodCall<jname_getScanRecord,&jcBluetoothLeScanResult::getScanRecord>::Call(env,this);
	}
};
//---------------------------------------------------------------------------
struct jcBluetoothLeScannerCallback : jcObject
{
	static constexpr const char jClassName[]="android/bluetooth/le/ScanCallback";
};
//---------------------------------------------------------------------------
struct jcBluetoothLeScanner : jcObject
{
	static constexpr const char jClassName[]="android/bluetooth/le/BluetoothLeScanner";

	static constexpr const char jname_startScan[]="startScan";
	void startScan(JNIEnv *env,jcBluetoothLeScannerCallback *callback)noexcept{
		return jMethodCall<jname_startScan,&jcBluetoothLeScanner::startScan>::Call(env,this,callback);
	}

	static constexpr const char jname_stopScan[]="stopScan";
	void stopScan(JNIEnv *env,jcBluetoothLeScannerCallback *callback)noexcept{
		return jMethodCall<jname_stopScan,&jcBluetoothLeScanner::stopScan>::Call(env,this,callback);
	}
};
//---------------------------------------------------------------------------
struct jcBluetoothAdapter : jcObject
{
	static constexpr const char jClassName[]="android/bluetooth/BluetoothAdapter";

	static constexpr const char jname_getBluetoothLeScanner[]="getBluetoothLeScanner";
	jrLocal<jcBluetoothLeScanner> getBluetoothLeScanner(JNIEnv *env)noexcept{
		return jMethodCall<jname_getBluetoothLeScanner,&jcBluetoothAdapter::getBluetoothLeScanner>::Call(env,this);
	}

	static constexpr const char jname_getState[]="getState";
	jint getState(JNIEnv *env)noexcept{
		return jMethodCall<jname_getState,&jcBluetoothAdapter::getState>::Call(env,this);
	}

	static constexpr const char jname_isEnabled[]="isEnabled";
	jboolean isEnabled(JNIEnv *env)noexcept{
		return jMethodCall<jname_isEnabled,&jcBluetoothAdapter::isEnabled>::Call(env,this);
	}
};
//---------------------------------------------------------------------------
struct jcBluetoothManager : jcObject
{
	static constexpr const char jClassName[]="android/bluetooth/BluetoothManager";
	
	static constexpr const char jname_getAdapter[]="getAdapter";
	jrLocal<jcBluetoothAdapter> getAdapter(JNIEnv *env)noexcept{
		return jMethodCall<jname_getAdapter,&jcBluetoothManager::getAdapter>::Call(env,this);
	}

	static constexpr const char jname_getConnectionState[]="getConnectionState";
	jint getConnectionState(JNIEnv *env,jcBluetoothDevice *Device,jint profile)noexcept{
		return jMethodCall<jname_getConnectionState,&jcBluetoothManager::getConnectionState>::Call(env,this,Device,profile);
	}
		
};
//---------------------------------------------------------------------------
}	// namespace bluetooth
//---------------------------------------------------------------------------
}	// namespace android
//---------------------------------------------------------------------------
}	// namespace jCPP
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
