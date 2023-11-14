#include "Shape.h"

using namespace cnLibrary;
using namespace cnUI;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
vSolidStaticColor::vSolidStaticColor()
{
}
//---------------------------------------------------------------------------
vSolidStaticColor::~vSolidStaticColor()
{
}
//---------------------------------------------------------------------------
void vSolidStaticColor::Paint(iUISimplePaintContext *Context)
{
	Context->Fill({0,0},fPaintSize,Color);
}
//---------------------------------------------------------------------------
void vSolidStaticColor::Update(void)
{
	if(fViewContent!=nullptr){
		fViewContent->QueryUpdate();
	}
}
//---------------------------------------------------------------------------
rPtr<viControl> vSolidStaticColor::Create(cUIColor Color)
{
	auto Content=rCreate< bwvControl<vSolidStaticColor> >();
	Content->Color=Color;
	return Content;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
vSolidColor::vSolidColor(viSolidColorData *Data)
	: fData(Data)
{
	DataInsertCallback();
}
//---------------------------------------------------------------------------
vSolidColor::~vSolidColor()
{
	DataRemoveCallback();
}
//---------------------------------------------------------------------------
void vSolidColor::Paint(iUISimplePaintContext *Context)
{
	if(fData==nullptr)
		return;

	cUIColor Color=fData->ColorValue();
	Context->Fill({0,0},fPaintSize,Color);
}
//---------------------------------------------------------------------------
viSolidColorData* vSolidColor::GetData(void)const
{
	return fData;
}
//---------------------------------------------------------------------------
void vSolidColor::SetData(viSolidColorData *Data)
{
	if(fData==Data)
		return;

	DataRemoveCallback();
	fData=Data;
	DataInsertCallback();

	Update();
}
//---------------------------------------------------------------------------
void vSolidColor::Update(void)
{
	if(fViewContent!=nullptr){
		fViewContent->QueryUpdate();
	}
}
//---------------------------------------------------------------------------
void vSolidColor::DataInsertCallback(void)
{
	if(fData!=nullptr){
		fSolidColorNotifyToken=fData->SolidColorNotifySet.Insert([this]{
			Update();
		});
	}
}
//---------------------------------------------------------------------------
void vSolidColor::DataRemoveCallback(void)
{
	if(fData!=nullptr){
		fData->SolidColorNotifySet.Remove(fSolidColorNotifyToken);
	}
}
//---------------------------------------------------------------------------
rPtr<viControl> vSolidColor::Create(viSolidColorData *Data)
{
	return rCreate< bwvControl<vSolidColor> >(Data);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
vBitmapImage::vBitmapImage()
{
}
//---------------------------------------------------------------------------
vBitmapImage::~vBitmapImage()
{
}
//---------------------------------------------------------------------------
void vBitmapImage::Paint(iUISimplePaintContext *Context)
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
void vBitmapImage::Update(void)
{
	fBitmapCache=nullptr;
	if(fViewContent!=nullptr){
		fViewContent->QueryUpdate();
	}
}
//---------------------------------------------------------------------------
rPtr<viControl> vBitmapImage::Create(iBitmapDataSource *Source)
{
	auto Content=rCreate< bwvControl<vBitmapImage> >();
	Content->BitmapSource=Source;
	return Content;
}
//---------------------------------------------------------------------------
