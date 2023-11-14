#include "ProtocolProcessor.h"
//---------------------------------------------------------------------------
using namespace cnLibrary;
using namespace cnNet;
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#if 0

bool cEndpointFromProtocol::InputPush(const void *Buffer,uIntn BufferSize)
{
	// Push to read buffer
}
//---------------------------------------------------------------------------
void cEndpointFromProtocol::InputClose(bool GracefulClose)
{
}
//---------------------------------------------------------------------------
bool cEndpointFromProtocol::OutputPull(void *Buffer,uIntn BufferSize,uIntn &SizeReturn)
{
	// Pull from write buffer
}
//---------------------------------------------------------------------------
void cEndpointFromProtocol::OutputClose(bool GracefulClose)
{
}
//---------------------------------------------------------------------------
bool cEndpointFromProtocol::StartWrite(iWriteStreamCallback *Callback)
{
}
//---------------------------------------------------------------------------
void cEndpointFromProtocol::StopWrite(bool Terminate)
{
}
//---------------------------------------------------------------------------
void cEndpointFromProtocol::NotifyWrite(bool IdleNotify)
{
}
//---------------------------------------------------------------------------
bool cEndpointFromProtocol::QueryWriteBuffer(cMemory &Buffer)
{
	// ret WriteBuffer
}
//---------------------------------------------------------------------------
void cEndpointFromProtocol::CommitWriteBuffer(uIntn Size)
{
	// notify write data available
	fHost->OutputNotify(false);
}
//---------------------------------------------------------------------------
bool cEndpointFromProtocol::StartRead(iReadStreamCallback *Callback)
{
}
//---------------------------------------------------------------------------
void cEndpointFromProtocol::StopRead(void)
{
}
//---------------------------------------------------------------------------
void cEndpointFromProtocol::NotifyRead(bool IdleNotify)
{
}
//---------------------------------------------------------------------------
bool cEndpointFromProtocol::QueryReadBuffer(cConstMemory &Buffer)
{

	// ret ReadBuffer
}
//---------------------------------------------------------------------------
void cEndpointFromProtocol::CompleteReadBuffer(void)
{
	// notify read buffer available
	fHost->InputNotify(false);
}
//---------------------------------------------------------------------------
#endif // 0
