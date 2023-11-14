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
	vSolidStaticColor();
	~vSolidStaticColor();

	static rPtr<viControl> Create(cUIColor Color);

	cUIColor Color;

	void Update(void);
protected:

	virtual void cnLib_FUNC Paint(iUISimplePaintContext *Context)override;
};
//---------------------------------------------------------------------------
class viSolidColorData : public viData
{
public:
	cFunctionSet<void (void)> SolidColorNotifySet;
	virtual cUIColor ColorValue(void)=0;
};
//---------------------------------------------------------------------------
class vSolidColor : public vSimpleViewPainter
{
public:
	vSolidColor(viSolidColorData *Data);
	~vSolidColor();

	static rPtr<viControl> Create(viSolidColorData *Data);

	viSolidColorData* GetData(void)const;
	void SetData(viSolidColorData *Data);

protected:
	dPtr<viSolidColorData> fData;
	virtual void cnLib_FUNC Paint(iUISimplePaintContext *Context)override;
private:
	void Update(void);

	void DataInsertCallback(void);
	void DataRemoveCallback(void);

	iFunctionToken *fSolidColorNotifyToken;

};
//---------------------------------------------------------------------------
class vBitmapImage : public vSimpleViewPainter
{
public:
	vBitmapImage();
	~vBitmapImage();


	static rPtr<viControl> Create(iBitmapDataSource *Source);

	iPtr<iBitmapDataSource> BitmapSource;

	void Update(void);
protected:
	iPtr<iUIBitmap> fBitmapCache;

	virtual void cnLib_FUNC Paint(iUISimplePaintContext *Context)override;
};
//---------------------------------------------------------------------------
}	// namespace cnUI
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif
