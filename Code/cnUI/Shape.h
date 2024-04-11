/*- cnUI - Shape ----------------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2009-03-22                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnUI_Shape_H__
#define __cnLibrary_cnUI_Shape_H__
/*-------------------------------------------------------------------------*/
#include <cnUI/ControlBase.h>

#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnUI{
//---------------------------------------------------------------------------
class vSolidStaticColor : public vSimpleViewPainter
{
public:
	vSolidStaticColor()noexcept(true);
	~vSolidStaticColor()noexcept(true);

	static rPtr<viControl> Create(cUIColor Color)noexcept(true);

	cUIColor Color;

	void Update(void)noexcept(true);
protected:

	virtual void cnLib_FUNC Paint(iUISimplePaintContext *Context)noexcept(true)override;
};
//---------------------------------------------------------------------------
class viSolidColorData : public viData
{
public:
	cFunctionSet<void (void)noexcept(true)> SolidColorNotifySet;
	virtual cUIColor ColorValue(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class vSolidColor : public vSimpleViewPainter
{
public:
	vSolidColor(viSolidColorData *Data)noexcept(true);
	~vSolidColor()noexcept(true);

	static rPtr<viControl> Create(viSolidColorData *Data)noexcept(true);

	viSolidColorData* GetData(void)const noexcept(true);
	void SetData(viSolidColorData *Data)noexcept(true);

protected:
	dPtr<viSolidColorData> fData;
	virtual void cnLib_FUNC Paint(iUISimplePaintContext *Context)noexcept(true)override;
private:
	void Update(void)noexcept(true);

	void DataInsertCallback(void)noexcept(true);
	void DataRemoveCallback(void)noexcept(true);

	iFunctionToken *fSolidColorNotifyToken;

};
//---------------------------------------------------------------------------
class vBitmapImage : public vSimpleViewPainter
{
public:
	vBitmapImage()noexcept(true);
	~vBitmapImage()noexcept(true);


	static rPtr<viControl> Create(iBitmapDataSource *Source)noexcept(true);

	iPtr<iBitmapDataSource> BitmapSource;

	void Update(void)noexcept(true);
protected:
	iPtr<iUIBitmap> fBitmapCache;

	virtual void cnLib_FUNC Paint(iUISimplePaintContext *Context)noexcept(true)override;
};
//---------------------------------------------------------------------------
}	// namespace cnUI
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif
