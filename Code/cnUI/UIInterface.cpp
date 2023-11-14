#include "UIInterface.h"

//---------------------------------------------------------------------------
using namespace cnLibrary;
using namespace cnUI;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void iControlComponent::SetContentZPosition(Float32)
{
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void kiControl::SetLayoutOrder(sfInt16)
{
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void viControl::SetContentVisible(bool)
{
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
viData::viData()
	: fInvalidated(false)
	, fInvalidating(false)
{
}
//---------------------------------------------------------------------------
viData::~viData()
{
	InvalidateData();
}
//---------------------------------------------------------------------------
void viData::InvalidateData(void)
{
	fInvalidated=true;
	NotifyInvalidate();
}
//---------------------------------------------------------------------------
void viData::NotifyInvalidate(void)
{
	fInvalidating=true;
	for(auto Item : fRefSet){
		Item->NotifyInvalidate(this);
	}
	fRefSet.RemoveAll();
	fInvalidating=false;
}
//---------------------------------------------------------------------------
void viData::NotifyRegister(bcNotifyToken *NotifyToken)
{
	if(fInvalidated || fInvalidating)
		return;
	fRefSet.Insert(NotifyToken);
}
//---------------------------------------------------------------------------
void viData::NotifyUnregister(bcNotifyToken *NotifyToken)
{
	if(fInvalidated || fInvalidating)
		return;
	fRefSet.Remove(NotifyToken);
}
//---------------------------------------------------------------------------
