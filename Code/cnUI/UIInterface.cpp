#include "UIInterface.h"

//---------------------------------------------------------------------------
using namespace cnLibrary;
using namespace cnUI;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void iControlComponent::SetContentZPosition(Float32)noexcept
{
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void kiControl::SetLayoutOrder(sfInt16)noexcept
{
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void viControl::SetContentVisible(bool)noexcept
{
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
viData::viData()noexcept
	: fInvalidated(false)
	, fInvalidating(false)
{
}
//---------------------------------------------------------------------------
viData::~viData()noexcept
{
	InvalidateData();
}
//---------------------------------------------------------------------------
void viData::InvalidateData(void)noexcept
{
	fInvalidated=true;
	NotifyInvalidate();
}
//---------------------------------------------------------------------------
void viData::NotifyInvalidate(void)noexcept
{
	fInvalidating=true;
	for(auto Item : fRefSet){
		Item->NotifyInvalidate(this);
	}
	fRefSet.RemoveAll();
	fInvalidating=false;
}
//---------------------------------------------------------------------------
void viData::NotifyRegister(bcNotifyToken *NotifyToken)noexcept
{
	if(fInvalidated || fInvalidating)
		return;
	fRefSet.Insert(NotifyToken);
}
//---------------------------------------------------------------------------
void viData::NotifyUnregister(bcNotifyToken *NotifyToken)noexcept
{
	if(fInvalidated || fInvalidating)
		return;
	fRefSet.Remove(NotifyToken);
}
//---------------------------------------------------------------------------
