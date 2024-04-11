#include "Shape.h"

using namespace cnLibrary;
using namespace cnUI;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
vSolidStaticColor::vSolidStaticColor()noexcept
{
}
//---------------------------------------------------------------------------
vSolidStaticColor::~vSolidStaticColor()noexcept
{
}
//---------------------------------------------------------------------------
void vSolidStaticColor::Paint(iUISimplePaintContext *Context)noexcept
{
	Context->Fill({0,0},fPaintSize,Color);
}
//---------------------------------------------------------------------------
void vSolidStaticColor::Update(void)noexcept
{
	if(fViewContent!=nullptr){
		fViewContent->QueryUpdate();
	}
}
//---------------------------------------------------------------------------
rPtr<viControl> vSolidStaticColor::Create(cUIColor Color)noexcept
{
	auto Content=rCreate< bwvControl<vSolidStaticColor> >();
	Content->Color=Color;
	return Content;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
vSolidColor::vSolidColor(viSolidColorData *Data)noexcept
	: fData(Data)
{
	DataInsertCallback();
}
//---------------------------------------------------------------------------
vSolidColor::~vSolidColor()noexcept
{
	DataRemoveCallback();
}
//---------------------------------------------------------------------------
void vSolidColor::Paint(iUISimplePaintContext *Context)noexcept
{
	if(fData==nullptr)
		return;

	cUIColor Color=fData->ColorValue();
	Context->Fill({0,0},fPaintSize,Color);
}
//---------------------------------------------------------------------------
viSolidColorData* vSolidColor::GetData(void)const noexcept
{
	return fData;
}
//---------------------------------------------------------------------------
void vSolidColor::SetData(viSolidColorData *Data)noexcept
{
	if(fData==Data)
		return;

	DataRemoveCallback();
	fData=Data;
	DataInsertCallback();

	Update();
}
//---------------------------------------------------------------------------
void vSolidColor::Update(void)noexcept
{
	if(fViewContent!=nullptr){
		fViewContent->QueryUpdate();
	}
}
//---------------------------------------------------------------------------
void vSolidColor::DataInsertCallback(void)noexcept
{
	if(fData!=nullptr){
		fSolidColorNotifyToken=fData->SolidColorNotifySet.Insert([this]{
			Update();
		});
	}
}
//---------------------------------------------------------------------------
void vSolidColor::DataRemoveCallback(void)noexcept
{
	if(fData!=nullptr){
		fData->SolidColorNotifySet.Remove(fSolidColorNotifyToken);
	}
}
//---------------------------------------------------------------------------
rPtr<viControl> vSolidColor::Create(viSolidColorData *Data)noexcept
{
	return rCreate< bwvControl<vSolidColor> >(Data);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
vBitmapImage::vBitmapImage()noexcept
{
}
//---------------------------------------------------------------------------
vBitmapImage::~vBitmapImage()noexcept
{
}
//---------------------------------------------------------------------------
void vBitmapImage::Paint(iUISimplePaintContext *Context)noexcept
{
	if(fBitmapCache==nullptr){
		if(BitmapSource!=nullptr){
			auto Device=fViewContent->QueryDevice();
			fBitmapCache=Device->CreateBitmapCopyFromSource(BitmapSource);
		}
	}

	if(fBitmapCache!=nullptr){
		Context->Graph({0,0},fPaintSize,fBitmapCache);
	}
}
//---------------------------------------------------------------------------
void vBitmapImage::Update(void)noexcept
{
	fBitmapCache=nullptr;
	if(fViewContent!=nullptr){
		fViewContent->QueryUpdate();
	}
}
//---------------------------------------------------------------------------
rPtr<viControl> vBitmapImage::Create(iBitmapDataSource *Source)noexcept
{
	auto Content=rCreate< bwvControl<vBitmapImage> >();
	Content->BitmapSource=Source;
	return Content;
}
//---------------------------------------------------------------------------
