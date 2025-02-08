#include "UIInterface.h"

//---------------------------------------------------------------------------
using namespace cnLibrary;
using namespace cnUI;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void iControlComponent::SetContentZPosition(Float32)noexcept(true)
{
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void kiControl::SetLayoutOrder(sfInt16)noexcept(true)
{
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void viControl::SetContentVisible(bool)noexcept(true)
{
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
viData::viData()noexcept(true)
	: fInvalidated(false)
	, fInvalidating(false)
{
}
//---------------------------------------------------------------------------
viData::~viData()noexcept(true)
{
	InvalidateData();
}
//---------------------------------------------------------------------------
void viData::InvalidateData(void)noexcept(true)
{
	fInvalidated=true;
	NotifyInvalidate();
}
//---------------------------------------------------------------------------
void viData::NotifyInvalidate(void)noexcept(true)
{
	fInvalidating=true;
	for(auto Item : fRefSet){
		Item->NotifyInvalidate(this);
	}
	fRefSet.RemoveAll();
	fInvalidating=false;
}
//---------------------------------------------------------------------------
void viData::NotifyRegister(bcNotifyToken *NotifyToken)noexcept(true)
{
	if(fInvalidated || fInvalidating)
		return;
	fRefSet.Insert(NotifyToken);
}
//---------------------------------------------------------------------------
void viData::NotifyUnregister(bcNotifyToken *NotifyToken)noexcept(true)
{
	if(fInvalidated || fInvalidating)
		return;
	fRefSet.Remove(NotifyToken);
}
//---------------------------------------------------------------------------
