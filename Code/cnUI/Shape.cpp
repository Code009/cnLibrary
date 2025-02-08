#include "Shape.h"

using namespace cnLibrary;
using namespace cnUI;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
vSolidStaticColor::vSolidStaticColor()noexcept(true)
{
}
//---------------------------------------------------------------------------
vSolidStaticColor::~vSolidStaticColor()noexcept(true)
{
}
//---------------------------------------------------------------------------
void vSolidStaticColor::Paint(iUISimplePaintContext *Context)noexcept(true)
{
	Context->Fill({0,0},fPaintSize,Color);
}
//---------------------------------------------------------------------------
void vSolidStaticColor::Update(void)noexcept(true)
{
	if(fViewContent!=nullptr){
		fViewContent->QueryUpdate();
	}
}
//---------------------------------------------------------------------------
rPtr<viControl> vSolidStaticColor::Create(cUIColor Color)noexcept(true)
{
	auto Content=rCreate< bwvControl<vSolidStaticColor> >();
	Content->Color=Color;
	return Content;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
vSolidColor::vSolidColor(viSolidColorData *Data)noexcept(true)
	: fData(Data)
{
	DataInsertCallback();
}
//---------------------------------------------------------------------------
vSolidColor::~vSolidColor()noexcept(true)
{
	DataRemoveCallback();
}
//---------------------------------------------------------------------------
void vSolidColor::Paint(iUISimplePaintContext *Context)noexcept(true)
{
	if(fData==nullptr)
		return;

	cUIColor Color=fData->ColorValue();
	Context->Fill({0,0},fPaintSize,Color);
}
//---------------------------------------------------------------------------
viSolidColorData* vSolidColor::GetData(void)const noexcept(true)
{
	return fData;
}
//---------------------------------------------------------------------------
void vSolidColor::SetData(viSolidColorData *Data)noexcept(true)
{
	if(fData==Data)
		return;

	DataRemoveCallback();
	fData=Data;
	DataInsertCallback();

	Update();
}
//---------------------------------------------------------------------------
void vSolidColor::Update(void)noexcept(true)
{
	if(fViewContent!=nullptr){
		fViewContent->QueryUpdate();
	}
}
//---------------------------------------------------------------------------
void vSolidColor::DataInsertCallback(void)noexcept(true)
{
	if(fData!=nullptr){
		fSolidColorNotifyToken=fData->SolidColorNotifySet.Insert([this]{
			Update();
		});
	}
}
//---------------------------------------------------------------------------
void vSolidColor::DataRemoveCallback(void)noexcept(true)
{
	if(fData!=nullptr){
		fData->SolidColorNotifySet.Remove(fSolidColorNotifyToken);
	}
}
//---------------------------------------------------------------------------
rPtr<viControl> vSolidColor::Create(viSolidColorData *Data)noexcept(true)
{
	return rCreate< bwvControl<vSolidColor> >(Data);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
vBitmapImage::vBitmapImage()noexcept(true)
{
}
//---------------------------------------------------------------------------
vBitmapImage::~vBitmapImage()noexcept(true)
{
}
//---------------------------------------------------------------------------
void vBitmapImage::Paint(iUISimplePaintContext *Context)noexcept(true)
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
void vBitmapImage::Update(void)noexcept(true)
{
	fBitmapCache=nullptr;
	if(fViewContent!=nullptr){
		fViewContent->QueryUpdate();
	}
}
//---------------------------------------------------------------------------
rPtr<viControl> vBitmapImage::Create(iBitmapDataSource *Source)noexcept(true)
{
	auto Content=rCreate< bwvControl<vBitmapImage> >();
	Content->BitmapSource=Source;
	return Content;
}
//---------------------------------------------------------------------------
